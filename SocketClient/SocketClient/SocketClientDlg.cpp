#include "pch.h"
#include "framework.h"
#include "SocketClient.h"
#include "SocketClientDlg.h"
#include "CClientSocket.h"
#include "afxdialogex.h"
#include <atlconv.h> // CW2A를 위한 헤더

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx {
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}
void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSocketClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_CONNECT, &CSocketClientDlg::OnBnClickedConnect)
    ON_BN_CLICKED(IDC_SEND, &CSocketClientDlg::OnBnClickedSend)
    ON_WM_KEYDOWN()  // 키보드 이벤트 처리 추가
    ON_WM_CLOSE()
END_MESSAGE_MAP()

CSocketClientDlg::CSocketClientDlg(CWnd* pParent)
    : CDialogEx(IDD_SOCKETCLIENT_DIALOG, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSocketClientDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
    DDX_Control(pDX, IDC_WRITE, m_write);
}

BOOL CSocketClientDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    static CFont font;
    font.CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));
    m_editLog.SetFont(&font);

    m_editLog.ModifyStyle(0, ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL);
    m_brEditBg.CreateSolidBrush(m_editBgColor);  // ✅ 흰 배경 브러시 생성

    m_clientSocket.SetParentDlg(this);
    return TRUE;
}

HBRUSH CSocketClientDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->GetSafeHwnd() == m_editLog.GetSafeHwnd()) {
        pDC->SetBkColor(m_editBgColor);
        pDC->SetTextColor(RGB(0, 0, 0));
        return m_brEditBg;
    }

    return hbr;
}

void CSocketClientDlg::AppendLogMessage(const CString& message) {
    CString current;
    m_editLog.GetWindowText(current);
    current += message + _T("\r\n");
    m_editLog.SetWindowText(current);
    m_editLog.LineScroll(m_editLog.GetLineCount());
}

void CSocketClientDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else if ((nID & 0xFFF0) == SC_CLOSE)
    {
        EndDialog(IDCANCEL);
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CSocketClientDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

HCURSOR CSocketClientDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CSocketClientDlg::OnBnClickedConnect() {
    if (!AfxSocketInit()) {
        AfxMessageBox(_T("소켓 초기화 실패"));
        return;
    }
    if (!m_clientSocket.Create()) {
        AfxMessageBox(_T("클라이언트 소켓 생성 실패"));
        return;
    }
    AppendLogMessage(_T("서버 연결 시도 중..."));
    if (m_clientSocket.Connect(_T("127.0.0.1"), 8080)) {
        AppendLogMessage(_T("서버에 연결되었습니다."));
        AfxMessageBox(_T("서버에 연결되었습니다."));
    }
}

void CSocketClientDlg::OnBnClickedSend() {
    CString strMessage;
    m_write.GetWindowText(strMessage);
    if (strMessage.Trim().IsEmpty()) return;

    AppendLogMessage(_T("클라이언트: ") + strMessage);
    m_write.SetWindowText(_T(""));

    CW2A utf8(strMessage, CP_UTF8);
    int result = m_clientSocket.Send(utf8, strlen(utf8));
    if (result == SOCKET_ERROR) {
        AfxMessageBox(_T("메시지 전송 실패!"));
    }
}

void CSocketClientDlg::OnReceiveMessage(CString message) {
    AppendLogMessage(_T("서버: ") + message);
}

void CSocketClientDlg::SetConnectionStatus(const CString& status) {
    AppendLogMessage(status);
}

void CSocketClientDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 엔터키가 눌렸을 때
    if (nChar == VK_RETURN && GetFocus() == GetDlgItem(IDC_WRITE)) {
        // 텍스트 입력 컨트롤에서 메시지 가져오기
        CString strMessage;
        m_write.GetWindowText(strMessage);

        // 메시지가 비어 있지 않은 경우에만 전송 처리
        if (strMessage.Trim().IsEmpty()) {
            AfxMessageBox(_T("메시지가 비어 있습니다. 내용을 입력해주세요."));
            return; // 메시지가 비어 있으면 아무것도 하지 않음
        }

        // 엔터키로 메시지를 보내는 함수 호출
        OnBnClickedSend();

        // 엔터키 처리 후 기본 동작을 막기 위해 추가
        return;  // 기본 엔터키 처리 방지
    }
    else {
        // 엔터 외의 다른 키는 기본 동작을 계속 진행
        CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}


BOOL CSocketClientDlg::PreTranslateMessage(MSG* pMsg)
{
    // WM_KEYDOWN 메시지를 처리
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
        // 엔터키가 눌렸을 때
        CString strMessage;
        m_write.GetWindowText(strMessage);

        // 메시지가 비어 있지 않으면 전송
        if (!strMessage.Trim().IsEmpty()) {
            // 메시지 전송 함수 호출
            OnBnClickedSend();
            return TRUE;  // 기본 동작을 막기 위해 TRUE를 반환
        }
        else {
            AfxMessageBox(_T("메시지가 비어 있습니다. 내용을 입력해주세요."));
            return TRUE;  // 메시지가 비어 있으면 전송을 막기 위해 TRUE를 반환
        }
    }

    // 엔터키 외의 다른 키는 기본 동작을 계속 진행
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CSocketClientDlg::OnOK()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

    // CDialogEx::OnOK();
}

void CSocketClientDlg::OnClose()
{
    CDialogEx::OnCancel();  // 다이얼로그 종료
}