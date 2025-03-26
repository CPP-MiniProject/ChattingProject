#pragma once
#include <afxsock.h>

class CSocketServerDlg;
class CClientSocket; // forward declaration (클라이언트 소켓 클래스 선언)

class CServerSocket : public CAsyncSocket
{
public:
    CServerSocket();
    virtual ~CServerSocket();

    void SetParent(CSocketServerDlg* pDlg);

protected:
    virtual void OnAccept(int nErrorCode);  // 클라이언트 연결 수락
    virtual void OnReceive(int nErrorCode); // ?? 클라이언트로부터 데이터 수신 시 호출

private:
    CSocketServerDlg* m_pDlg;
};
