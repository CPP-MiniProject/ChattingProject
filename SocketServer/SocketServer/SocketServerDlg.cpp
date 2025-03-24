#include "pch.h"
#include "framework.h"
#include "SocketServer.h"
#include "SocketServerDlg.h"
#include "CServerSocket.h"
#include "afxdialogex.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSocketServerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CTLCOLOR() // ⬆️ Add this
    ON_BN_CLICKED(IDC_CONNECT, &CSocketServerDlg::OnBnClickedConnect)
    ON_BN_CLICKED(IDC_SEND, &CSocketServerDlg::OnBnClickedSend)
    ON_EN_CHANGE(IDC_EDIT_LOG, &CSocketServerDlg::OnEnChangeEditLog)
    ON_WM_KEYDOWN()  // 키보드 이벤트 처리 추가
    ON_WM_CLOSE()
END_MESSAGE_MAP()

CSocketServerDlg::CSocketServerDlg(CWnd* pParent)
    : CDialogEx(IDD_SOCKETSERVER_DIALOG, pParent), m_pClientSocket(nullptr) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_brushWhite.CreateSolidBrush(RGB(255, 255, 255)); // ⬆️ Create white brush
}

void CSocketServerDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
    DDX_Control(pDX, IDC_WRITE3, m_editMessage);
    DDX_Control(pDX, IDC_STATUS, m_status);
}

BOOL CSocketServerDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_status.SetWindowText(_T("통신 상태: 없음"));
    m_editLog.ModifyStyle(0, ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL);
    m_editMessage.ModifyStyle(ES_WANTRETURN, 0);

    static CFont font;
    font.CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));

    m_editLog.SetFont(&font);
    m_editMessage.SetFont(&font);
    m_status.SetFont(&font);

    return TRUE;
}

HBRUSH CSocketServerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (pWnd->GetDlgCtrlID() == IDC_EDIT_LOG) {
        pDC->SetBkColor(RGB(255, 255, 255));
        return m_brushWhite;
    }
    return hbr;
}

void CSocketServerDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    CDialogEx::OnSysCommand(nID, lParam);
}

void CSocketServerDlg::OnPaint() {
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

HCURSOR CSocketServerDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CSocketServerDlg::AppendLogMessage(const CString& message) {
    int len = m_editLog.GetWindowTextLength();
    m_editLog.SetSel(len, len);
    m_editLog.ReplaceSel(message + _T("\r\n"));
}

void CSocketServerDlg::OnBnClickedConnect() {
    m_status.SetWindowText(_T("연결 시도 중..."));
    AppendLogMessage(_T("서버 연결 시도 중..."));
    if (!AfxSocketInit()) {
        AfxMessageBox(_T("소켓 초기화 실패"));
        m_status.SetWindowText(_T("통신 상태: 실패"));
        return;
    }
    if (!m_serverSocket.Create(8080)) {
        AfxMessageBox(_T("서버 소켓 생성 실패"));
        m_status.SetWindowText(_T("통신 상태: 실패"));
        return;
    }
    if (!m_serverSocket.Listen()) {
        AfxMessageBox(_T("서버 리슨 실패"));
        m_status.SetWindowText(_T("통신 상태: 실패"));
        return;
    }
    m_serverSocket.SetParent(this);
    m_status.SetWindowText(_T("서버 대기 중"));
    if (!m_bServerStartedMessageShown) {
        AppendLogMessage(_T("서버가 시작되었습니다."));
        m_bServerStartedMessageShown = true;
    }
}

void CSocketServerDlg::OnBnClickedSend() {
    CString strMessage;
    m_editMessage.GetWindowText(strMessage);
    if (strMessage.IsEmpty()) return;
    AppendLogMessage(_T("서버: ") + strMessage);
    m_editMessage.SetWindowText(_T(""));
    if (m_pClientSocket) {
        CW2A utf8(strMessage, CP_UTF8);
        m_pClientSocket->Send(utf8, strlen(utf8));
    }
    else {
        AfxMessageBox(_T("클라이언트가 연결되지 않았습니다."));
    }
}

void CSocketServerDlg::SetClientSocket(CAsyncSocket* pClient) {
    if (m_pClientSocket) delete m_pClientSocket;
    m_pClientSocket = pClient;
    m_status.SetWindowText(_T("클라이언트 연결됨"));
    if (!m_bClientConnectedMessageShown) {
        AppendLogMessage(_T("클라이언트가 연결되었습니다."));
        m_bClientConnectedMessageShown = true;
    }
}

void CSocketServerDlg::OnReceiveMessage(CString message) {
    AppendLogMessage(_T("클라이언트: ") + message);
}
void CSocketServerDlg::OnEnChangeEditLog()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CDialogEx::OnInitDialog() 함수를 재지정 
    //하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
    // ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CSocketServerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 엔터키가 눌렸을 때
    if (nChar == VK_RETURN && GetFocus() == GetDlgItem(IDC_WRITE3)) {
        // 텍스트 입력 컨트롤에서 메시지 가져오기
        CString strMessage;
        m_editMessage.GetWindowText(strMessage);

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

BOOL CSocketServerDlg::PreTranslateMessage(MSG* pMsg)
{
    TRACE("PreTranslateMessage called: message = %u, wParam = %u\n", pMsg->message, pMsg->wParam);

    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
        CString strMessage;
        m_editMessage.GetWindowText(strMessage);
        if (!strMessage.Trim().IsEmpty()) {
            OnBnClickedSend();
            return TRUE;
        }
        else {
            AfxMessageBox(_T("메시지가 비어 있습니다. 내용을 입력해주세요."));
            return TRUE;
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CSocketServerDlg::OnOK()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

     CDialogEx::OnOK();
}

void CSocketServerDlg::OnClose()
{
    CDialogEx::OnCancel();  // 다이얼로그 종료
}

