#include "pch.h"
#include "CClientSocket.h"
#include "SocketClientDlg.h"
#include <atlconv.h> // 꼭 상단에 추가

CClientSocket::CClientSocket() : m_pDlg(nullptr) {}

CClientSocket::~CClientSocket() {}

void CClientSocket::SetParentDlg(CSocketClientDlg* pDlg)
{
    m_pDlg = pDlg;
}


void CClientSocket::OnConnect(int nErrorCode)
{
    if (nErrorCode == 0 && m_pDlg)
    {
        // 상태 텍스트 갱신
        m_pDlg->SetConnectionStatus(_T("서버에 연결되었습니다."));

        // 연결 성공 메시지
        AfxMessageBox(_T("서버에 연결되었습니다."));
    }

    CAsyncSocket::OnConnect(nErrorCode);
}



void CClientSocket::OnReceive(int nErrorCode)
{
    if (nErrorCode != 0 || m_pDlg == nullptr)
    {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }

    char header;
    int headerBytes = Receive(&header, 1);  // 수신 타입 구분: 'M' or 'F'

    if (headerBytes <= 0)
    {
        AfxMessageBox(_T("헤더 수신 실패"));
        return;
    }

    if (header == 'M')  // 메시지 수신
    {
        char buffer[1024] = { 0 };
        int bytes = Receive(buffer, sizeof(buffer) - 1);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';

            // UTF-8 → 유니코드 변환
            CA2W wide(buffer, CP_UTF8);
            CString msg(wide);
            m_pDlg->OnReceiveMessage(msg);
        }
    }
    else if (header == 'F')  // 파일 수신
    {
        // 1. 파일명 길이 수신
        int nameLen = 0;
        int lenBytes = Receive(&nameLen, sizeof(int));
        if (lenBytes != sizeof(int))
        {
            AfxMessageBox(_T("파일명 길이 수신 실패"));
            return;
        }

        // 2. 파일명 수신
        char nameBuffer[260] = { 0 };
        int nameBytes = Receive(nameBuffer, nameLen);
        if (nameBytes != nameLen)
        {
            AfxMessageBox(_T("파일명 수신 실패"));
            return;
        }

        CString fileName = CA2W(nameBuffer, CP_UTF8);  // UTF-8 → 유니코드

        // 3. 저장할 파일 열기
        CFile file;
        if (!file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
        {
            AfxMessageBox(_T("파일 저장 실패: ") + fileName);
            return;
        }

        // 4. 파일 데이터 수신
        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        int nReceived = 0;

        while ((nReceived = Receive(buffer, BUFFER_SIZE)) > 0)
        {
            file.Write(buffer, nReceived);

            // 종료 조건: Receive가 더 이상 읽을 게 없을 때
            if (nReceived < BUFFER_SIZE)
                break;
        }

        file.Close();
        AfxMessageBox(_T("파일 수신 완료: ") + fileName);
    }

    CAsyncSocket::OnReceive(nErrorCode);
}
