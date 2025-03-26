#include "pch.h"
#include "CClientSocket.h"
#include "SocketServerDlg.h"
#include <atlconv.h>  // CA2W for UTF-8 → CString

CClientSocket::CClientSocket() : m_pDlg(nullptr) {}

CClientSocket::~CClientSocket() {}

void CClientSocket::SetParentDlg(CSocketServerDlg* pDlg)
{
    m_pDlg = pDlg;
}

void CClientSocket::OnReceive(int nErrorCode)
{
    if (nErrorCode != 0 || m_pDlg == nullptr)
    {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }

    char header = 0;
    if (Receive(&header, 1) <= 0)
        return;

    if (header == 'M')  // 메시지 수신
    {
        int msgLen = 0;
        if (Receive(&msgLen, sizeof(int)) != sizeof(int))
        {
            AfxMessageBox(_T("메시지 길이 수신 실패"));
            return;
        }

        char* buffer = new char[msgLen + 1];
        int totalReceived = 0;
        while (totalReceived < msgLen)
        {
            int ret = Receive(buffer + totalReceived, msgLen - totalReceived);
            if (ret <= 0)
            {
                AfxMessageBox(_T("메시지 수신 실패"));
                delete[] buffer;
                return;
            }
            totalReceived += ret;
        }
        buffer[msgLen] = '\0';

        CString msg(CA2W(buffer, CP_UTF8));
        m_pDlg->OnReceiveMessage(msg);
        delete[] buffer;
    }
    else if (header == 'F')  // 파일 수신
    {
        int nameLen = 0;
        if (Receive(&nameLen, sizeof(int)) != sizeof(int))
        {
            AfxMessageBox(_T("파일명 길이 수신 실패"));
            return;
        }

        char nameBuffer[260] = { 0 };
        if (Receive(nameBuffer, nameLen) != nameLen)
        {
            AfxMessageBox(_T("파일명 수신 실패"));
            return;
        }

        CString fileName = CA2W(nameBuffer, CP_UTF8);

        CFile file;
        if (!file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
        {
            AfxMessageBox(_T("서버에서 파일 저장 실패"));
            return;
        }

        // 파일 크기 정보를 받지 않으면 수신 종료 조건이 모호할 수 있음
        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        int nReceived = 0;

        while ((nReceived = Receive(buffer, BUFFER_SIZE)) > 0)
        {
            file.Write(buffer, nReceived);
            if (nReceived < BUFFER_SIZE)
                break;
        }

        file.Close();
        AfxMessageBox(_T("파일 수신 완료: ") + fileName);
    }

    CAsyncSocket::OnReceive(nErrorCode);
}
