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


void CClientSocket::OnReceive(int nErrorCode) {
    if (nErrorCode == 0 && m_pDlg) {
        char buffer[1024] = { 0 };
        int bytes = Receive(buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';

            // ? UTF-8 → 유니코드 디코딩
            CA2W wide(buffer, CP_UTF8);
            CString msg(wide);
            m_pDlg->OnReceiveMessage(msg);
        }
    }
    CAsyncSocket::OnReceive(nErrorCode);
}
