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



void CClientSocket::OnReceive(int nErrorCode)
{
    if (nErrorCode != 0 || m_pDlg == nullptr)
    {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }

    char header;
    int headerBytes = Receive(&header, 1);  // ���� Ÿ�� ����: 'M' or 'F'

    if (headerBytes <= 0)
    {
        AfxMessageBox(_T("��� ���� ����"));
        return;
    }

    if (header == 'M')  // �޽��� ����
    {
        char buffer[1024] = { 0 };
        int bytes = Receive(buffer, sizeof(buffer) - 1);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';

            // UTF-8 �� �����ڵ� ��ȯ
            CA2W wide(buffer, CP_UTF8);
            CString msg(wide);
            m_pDlg->OnReceiveMessage(msg);
        }
    }
    else if (header == 'F')  // ���� ����
    {
        // 1. ���ϸ� ���� ����
        int nameLen = 0;
        int lenBytes = Receive(&nameLen, sizeof(int));
        if (lenBytes != sizeof(int))
        {
            AfxMessageBox(_T("���ϸ� ���� ���� ����"));
            return;
        }

        // 2. ���ϸ� ����
        char nameBuffer[260] = { 0 };
        int nameBytes = Receive(nameBuffer, nameLen);
        if (nameBytes != nameLen)
        {
            AfxMessageBox(_T("���ϸ� ���� ����"));
            return;
        }

        CString fileName = CA2W(nameBuffer, CP_UTF8);  // UTF-8 �� �����ڵ�

        // 3. ������ ���� ����
        CFile file;
        if (!file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
        {
            AfxMessageBox(_T("���� ���� ����: ") + fileName);
            return;
        }

        // 4. ���� ������ ����
        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        int nReceived = 0;

        while ((nReceived = Receive(buffer, BUFFER_SIZE)) > 0)
        {
            file.Write(buffer, nReceived);

            // ���� ����: Receive�� �� �̻� ���� �� ���� ��
            if (nReceived < BUFFER_SIZE)
                break;
        }

        file.Close();
        AfxMessageBox(_T("���� ���� �Ϸ�: ") + fileName);
    }

    CAsyncSocket::OnReceive(nErrorCode);
}
