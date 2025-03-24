// SocketClientDlg.h: 헤더 파일(완)
#pragma once
#include "CClientSocket.h" 

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

	bool m_bServerConnectedMessageShown = false;

	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedSend();

	void OnReceiveMessage(CString message);
	void SetConnectionStatus(const CString& status);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	afx_msg void OnClose();
};
