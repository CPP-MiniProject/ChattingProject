#include "pch.h"
#include "CClientSocket.h"
#include "SocketServerDlg.h"
#include <atlconv.h>  // CA2W for UTF-8 �� CString

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

    if (header == 'M')  // �޽��� ����
    {
        int msgLen = 0;
        if (Receive(&msgLen, sizeof(int)) != sizeof(int))
        {
            AfxMessageBox(_T("�޽��� ���� ���� ����"));
            return;
        }

        char* buffer = new char[msgLen + 1];
        int totalReceived = 0;
        while (totalReceived < msgLen)
        {
            int ret = Receive(buffer + totalReceived, msgLen - totalReceived);
            if (ret <= 0)
            {
                AfxMessageBox(_T("�޽��� ���� ����"));
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
    else if (header == 'F')  // ���� ����
    {
        int nameLen = 0;
        if (Receive(&nameLen, sizeof(int)) != sizeof(int))
        {
            AfxMessageBox(_T("���ϸ� ���� ���� ����"));
            return;
        }

        char nameBuffer[260] = { 0 };
        if (Receive(nameBuffer, nameLen) != nameLen)
        {
            AfxMessageBox(_T("���ϸ� ���� ����"));
            return;
        }

        CString fileName = CA2W(nameBuffer, CP_UTF8);

        CFile file;
        if (!file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
        {
            AfxMessageBox(_T("�������� ���� ���� ����"));
            return;
        }

        // ���� ũ�� ������ ���� ������ ���� ���� ������ ��ȣ�� �� ����
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
        AfxMessageBox(_T("���� ���� �Ϸ�: ") + fileName);
    }

    CAsyncSocket::OnReceive(nErrorCode);
}
