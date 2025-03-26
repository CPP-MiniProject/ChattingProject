#include "pch.h"
#include "CServerSocket.h"
#include "SocketServerDlg.h"
#include "CClientSocket.h"
#include <atlconv.h>  // UTF-8 ��ȯ�� ���� ���

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

                // ��Ȯ�� ������ ���
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

void CServerSocket::OnReceive(int nErrorCode) {
    if (nErrorCode == 0 && m_pDlg && m_pDlg->m_pClientSocket) {
        char buffer[1024] = { 0 };
        int bytesRead = m_pDlg->m_pClientSocket->Receive(buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';

            if (!m_pDlg->m_bClientConnectedMessageShown) {
                m_pDlg->AppendLogMessage(_T("Ŭ���̾�Ʈ�� ����Ǿ����ϴ�."));
                m_pDlg->m_bClientConnectedMessageShown = true;
            }

            // UTF-8 �� �����ڵ�(CString) ��ȯ
            int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
            if (wlen > 0) {
                wchar_t* wideBuffer = new wchar_t[wlen];
                MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, wlen);
                CString strMessage(wideBuffer);
                delete[] wideBuffer;

                m_pDlg->OnReceiveMessage(strMessage);
            }
        }
    }

    CAsyncSocket::OnReceive(nErrorCode);
}