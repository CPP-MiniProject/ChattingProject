#pragma once
#include <afxsock.h>

class CSocketServerDlg;

class CServerSocket : public CAsyncSocket
{
public:
    CServerSocket();
    virtual ~CServerSocket();

    void SetParent(CSocketServerDlg* pDlg);
    virtual void OnAccept(int nErrorCode);
    virtual void OnReceive(int nErrorCode);  // ���� ������ ����ֵ� ��

private:
    CSocketServerDlg* m_pDlg;
};
