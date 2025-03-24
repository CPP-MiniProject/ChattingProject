#include "pch.h"
#include "CClientSocket.h"
#include "SocketClientDlg.h"
#include <atlconv.h> // �� ��ܿ� �߰�

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
        // ���� �ؽ�Ʈ ����
        m_pDlg->SetConnectionStatus(_T("������ ����Ǿ����ϴ�."));

        // ���� ���� �޽���
        AfxMessageBox(_T("������ ����Ǿ����ϴ�."));
    }

    CAsyncSocket::OnConnect(nErrorCode);
}


void CClientSocket::OnReceive(int nErrorCode) {
    if (nErrorCode == 0 && m_pDlg) {
        char buffer[1024] = { 0 };
        int bytes = Receive(buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';

            // ? UTF-8 �� �����ڵ� ���ڵ�
            CA2W wide(buffer, CP_UTF8);
            CString msg(wide);
            m_pDlg->OnReceiveMessage(msg);
        }
    }
    CAsyncSocket::OnReceive(nErrorCode);
}
