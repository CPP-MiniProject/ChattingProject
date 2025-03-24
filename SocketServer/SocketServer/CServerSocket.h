#pragma once
#include <afxsock.h>

class CSocketServerDlg;
class CClientSocket; // forward declaration (Ŭ���̾�Ʈ ���� Ŭ���� ����)

class CServerSocket : public CAsyncSocket
{
public:
    CServerSocket();
    virtual ~CServerSocket();

    void SetParent(CSocketServerDlg* pDlg);

protected:
    virtual void OnAccept(int nErrorCode);  // Ŭ���̾�Ʈ ���� ����
    virtual void OnReceive(int nErrorCode); // ?? Ŭ���̾�Ʈ�κ��� ������ ���� �� ȣ��

private:
    CSocketServerDlg* m_pDlg;
};
