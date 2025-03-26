#include "pch.h"
#include "CServerSocket.h"
#include "SocketServerDlg.h"
#include "CClientSocket.h"

CServerSocket::CServerSocket() {
    m_pDlg = nullptr;
}

CServerSocket::~CServerSocket() {}

void CServerSocket::SetParent(CSocketServerDlg* pDlg) {
    m_pDlg = pDlg;
}

void CServerSocket::OnAccept(int nErrorCode) {
    if (nErrorCode == 0) {
        CClientSocket* pClientSocket = new CClientSocket();
        pClientSocket->SetParentDlg(m_pDlg);

        if (Accept(*pClientSocket)) {
            if (m_pDlg) {
                m_pDlg->SetClientSocket(pClientSocket);
                m_pDlg->m_status.SetWindowText(_T("클라이언트 연결됨"));
                AfxMessageBox(_T("클라이언트가 연결되었습니다."));
            }
        }
        else {
            delete pClientSocket;
        }
    }

    CAsyncSocket::OnAccept(nErrorCode);
}

// ? 불필요한 수신 로직 제거 (모든 수신은 CClientSocket에서 수행)
void CServerSocket::OnReceive(int nErrorCode)
{
    // 비워둠 또는 삭제해도 무방
    CAsyncSocket::OnReceive(nErrorCode);
}
