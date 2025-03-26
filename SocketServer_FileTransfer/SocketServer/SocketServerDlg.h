#pragma once
#include "afxdialogex.h"
#include "CServerSocket.h"
#include "CClientSocket.h" // ← 이게 꼭 있어야 함!

class CSocketServerDlg : public CDialogEx {
public:
    CSocketServerDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SOCKETSERVER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    DECLARE_MESSAGE_MAP()

public:
    // 서버 소켓 및 클라이언트 소켓
    CServerSocket m_serverSocket;
    CAsyncSocket* m_pClientSocket;
    CClientSocket* m_pSocket;
    // UI 컨트롤
    CEdit m_editMessage;
    CEdit m_editLog;
    CStatic m_status;

    //선택된 파일 경로 저장용
    CString m_strFilePath;
    
    // 상태 관련
    HICON m_hIcon;
    bool m_bIsServerTurn = false;
    bool m_bServerStartedMessageShown = false;
    bool m_bClientConnectedMessageShown = false;

    // 새로운 상태 변수들
    bool m_bIsConnected = false;  // 서버와 연결된 상태인지 추적하는 변수
    bool m_bIsListening = false;  // 서버가 연결을 기다리고 있는지 추적하는 변수


    // 핸들러
    afx_msg void OnBnClickedConnect();
    afx_msg void OnBnClickedSend();
    afx_msg void OnBnClickedSelectFile();
    afx_msg void OnBnClickedSendFile();
    afx_msg void OnEnChangeEditLog();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnClose();
    virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // 메시지 처리
    void SetClientSocket(CClientSocket* pClient);  // CAsyncSocket → CClientSocket
    void OnReceiveMessage(CString message);
    void AppendLogMessage(const CString& message);

private:
    // 배경색 브러시 및 폰트
    CBrush m_brushWhite;
    CFont m_fontMalgunGothic;
};
