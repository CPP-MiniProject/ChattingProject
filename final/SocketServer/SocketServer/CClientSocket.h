#pragma once
#include <afxsock.h>

class CSocketServerDlg;

class CClientSocket : public CAsyncSocket
{
public:
    CClientSocket();
    virtual ~CClientSocket();

    void SetParentDlg(CSocketServerDlg* pDlg);

protected:
    virtual void OnReceive(int nErrorCode);

private:
    CSocketServerDlg* m_pDlg;
};
