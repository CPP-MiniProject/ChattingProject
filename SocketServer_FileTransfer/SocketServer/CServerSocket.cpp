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
                m_pDlg->m_status.SetWindowText(_T("Ŭ���̾�Ʈ �����"));
                AfxMessageBox(_T("Ŭ���̾�Ʈ�� ����Ǿ����ϴ�."));
            }
        }
        else {
            delete pClientSocket;
        }
    }

    CAsyncSocket::OnAccept(nErrorCode);
}

// ? ���ʿ��� ���� ���� ���� (��� ������ CClientSocket���� ����)
void CServerSocket::OnReceive(int nErrorCode)
{
    // ����� �Ǵ� �����ص� ����
    CAsyncSocket::OnReceive(nErrorCode);
}
