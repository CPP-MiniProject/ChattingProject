#include "pch.h"
#include "CClientSocket.h"
#include "SocketServerDlg.h"

CClientSocket::CClientSocket() : m_pDlg(nullptr) {}

CClientSocket::~CClientSocket() {}

void CClientSocket::SetParentDlg(CSocketServerDlg* pDlg)
{
    m_pDlg = pDlg;
}

void CClientSocket::OnReceive(int nErrorCode)
{
    if (nErrorCode == 0)
    {
        char buffer[1024] = { 0 };
        int bytesRead = Receive(buffer, sizeof(buffer) - 1);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';

            // ? UTF-8 ¡æ Unicode (CString)
            int wideLen = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
            if (wideLen > 0)
            {
                wchar_t* wideBuffer = new wchar_t[wideLen];
                MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, wideLen);
                CString strMessage(wideBuffer);
                delete[] wideBuffer;

                if (m_pDlg)
                    m_pDlg->OnReceiveMessage(strMessage);
            }
        }
    }

    CAsyncSocket::OnReceive(nErrorCode);
}
