#include "pch.h"
#include "CServerSocket.h"
#include "SocketServerDlg.h"
#include "CClientSocket.h"
#include <atlconv.h>  // UTF-8 변환을 위한 헤더

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

                // 정확한 순서로 출력
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

void CServerSocket::OnReceive(int nErrorCode) {
    if (nErrorCode == 0 && m_pDlg && m_pDlg->m_pClientSocket) {
        char buffer[1024] = { 0 };
        int bytesRead = m_pDlg->m_pClientSocket->Receive(buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';

            if (!m_pDlg->m_bClientConnectedMessageShown) {
                m_pDlg->AppendLogMessage(_T("클라이언트가 연결되었습니다."));
                m_pDlg->m_bClientConnectedMessageShown = true;
            }

            // UTF-8 → 유니코드(CString) 변환
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