// CClientSocket.h
#pragma once
#include <afxsock.h>

class CSocketClientDlg; // ? ���� ����

class CClientSocket : public CAsyncSocket
{
public:
    CClientSocket();
    virtual ~CClientSocket();

    void SetParentDlg(CSocketClientDlg* pDlg);

protected:
    virtual void OnConnect(int nErrorCode);
    virtual void OnReceive(int nErrorCode);

private:
    CSocketClientDlg* m_pDlg;
};
