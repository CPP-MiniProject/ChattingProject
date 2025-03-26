#pragma once
#include "CClientSocket.h" 
#define WM_SOCKET (WM_USER + 1)

// CSocketClientDlg 대화 상자
class CSocketClientDlg : public CDialogEx
{
    // 생성입니다.
public:
    CSocketClientDlg(CWnd* pParent = nullptr); // 표준 생성자입니다.
    void AppendLogMessage(const CString& message);

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SOCKETCLIENT_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다.
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private:
    CBrush m_brEditBg;
    COLORREF m_editBgColor = RGB(255, 255, 255); // 이미 있으니까 OK

public:
    CClientSocket m_clientSocket;
    HICON m_hIcon;
    CEdit m_write;
    CEdit m_editLog;
    CStatic m_status;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor); // 배경 흰색 지정

    afx_msg void OnBnClickedConnect();
    afx_msg void OnBnClickedSend();

    void OnReceiveMessage(CString message);
    void SetConnectionStatus(const CString& status);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnOK();
    afx_msg void OnClose();

private:
    bool m_bIsConnected = false;  // 서버 연결 여부를 추적하는 변수
    bool m_bIsConnecting = false; // 연결 시도 중 상태를 추적하는 변수
    bool m_bSocketInitialized = false;  // 소켓 초기화 여부를 추적하는 변수
    bool m_bSocketCreated = false;     // 소켓 생성 여부를 추적하는 변수
    bool m_bTimerSet = false; // 타이머 설정 여부를 추적하는 변수
};
