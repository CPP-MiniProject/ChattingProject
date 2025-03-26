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
    ON_BN_CLICKED(IDC_BTN_SELECT_FILE_C, &CSocketClientDlg::OnBnClickedSelectFile)
    ON_BN_CLICKED(IDC_BTN_SEND_FILE_C, &CSocketClientDlg::OnBnClickedSendFile)
    ON_WM_KEYDOWN()  // 키보드 이벤트 처리 추가
    ON_WM_CLOSE()
    ON_WM_TIMER()
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

void CSocketClientDlg::OnBnClickedSelectFile()
{
    CFileDialog dlg(TRUE);
    if (dlg.DoModal() == IDOK)
    {
        m_strSelectedFilePath = dlg.GetPathName();
        AfxMessageBox(_T("파일 선택됨: ") + m_strSelectedFilePath);
    }
}





HCURSOR CSocketClientDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CSocketClientDlg::OnBnClickedConnect() {
    // 이미 연결 중인 경우 처리
    if (m_bIsConnected) {
        AfxMessageBox(_T("이미 서버에 연결되어 있습니다."));
        return;
    }

    // 연결 시도 중인 상태에서 중복 연결을 방지
    if (m_bIsConnecting) {
        AfxMessageBox(_T("현재 연결 시도 중입니다. 잠시 후 다시 시도해주세요."));
        return;
    }

    // 소켓 초기화 및 생성
    if (!m_bSocketInitialized) {
        if (!AfxSocketInit()) {
            AfxMessageBox(_T("소켓 초기화 실패"));
            return;
        }
        m_bSocketInitialized = true;
    }

    if (!m_bSocketCreated) {
        if (!m_clientSocket.Create()) {
            AfxMessageBox(_T("클라이언트 소켓 생성 실패"));
            return;
        }
        m_bSocketCreated = true;
    }

    // 연결 시도
    m_bIsConnecting = true;
    GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
    UpdateWindow();

    // 서버 연결 시도 (비동기 방식이 아니라 동기 방식으로 처리)
    BOOL result = m_clientSocket.Connect(_T("127.0.0.1"), 8080);
    if (result) {
        // 연결 성공
        m_bIsConnected = true;
        m_bIsConnecting = false;
        AppendLogMessage(_T("서버에 연결되었습니다."));
        GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
        UpdateWindow();

        // 타이머 종료 (연결이 완료되었으므로 타이머는 더 이상 필요 없음)
        KillTimer(1);
    }
    else {
        // 연결 실패
        int errorCode = m_clientSocket.GetLastError();

        if (errorCode == WSAEWOULDBLOCK) {
            // 비동기 연결 진행 중 (이 경우 연결이 진행 중임)
            AppendLogMessage(_T("서버 연결 진행 중..."));
            // 비동기 연결이므로 잠시 기다리기
            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(m_clientSocket, &writeSet);

            // 소켓이 연결될 때까지 기다리기
            timeval timeout = { 5, 0 }; // 5초 타임아웃
            int result = select(0, NULL, &writeSet, NULL, &timeout);

            if (result > 0) {
                // 연결 성공
                m_bIsConnected = true;
                m_bIsConnecting = false;
                //AppendLogMessage(_T("서버에 연결되었습니다."));
                GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
                UpdateWindow();

                KillTimer(1);
            }
            else {
                // 연결 실패 (타임아웃)
                CString errorMessage = _T("서버 연결 실패: 시간 초과");
                AppendLogMessage(errorMessage);
                m_bIsConnecting = false;
                GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
                UpdateWindow();
            }
        }
        else {
            // 다른 오류가 발생한 경우
            CString errorMessage;
            errorMessage.Format(_T("서버 연결 실패. 오류 코드: %d"), errorCode);
            AppendLogMessage(errorMessage);
            m_bIsConnecting = false;
            GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
            UpdateWindow();
        }
    }
}

void CSocketClientDlg::OnTimer(UINT_PTR nIDEvent) {
    // 타이머 이벤트 ID가 1일 때만 처리
    if (nIDEvent == 1) {
        // ID가 1인 타이머가 호출되었을 때의 처리
        // 연결 상태를 확인하고, 시간 초과 등의 처리를 진행
        if (!m_bIsConnected && m_bIsConnecting) {
            CString errorMessage = _T("서버 연결 실패: 시간 초과");
            AppendLogMessage(errorMessage);
            //AfxMessageBox(errorMessage);

            // 연결 실패 시 버튼을 다시 활성화
            m_bIsConnecting = false;
            GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
            UpdateWindow();

            // 타이머 종료
            KillTimer(1); // 타이머 ID가 1인 타이머를 종료
        }
    }

    CDialogEx::OnTimer(nIDEvent);  // 기본 타이머 처리 호출
}

void CSocketClientDlg::OnBnClickedSendFile()
{
    if (m_strSelectedFilePath.IsEmpty())
    {
        AfxMessageBox(_T("파일을 먼저 선택하세요."));
        return;
    }

    // 1. 파일 열기
    CFile file;
    if (!file.Open(m_strSelectedFilePath, CFile::modeRead | CFile::typeBinary))
    {
        AfxMessageBox(_T("파일 열기 실패"));
        return;
    }

    // 2. 파일 크기 확인 및 버퍼 준비
    ULONGLONG fileSize = file.GetLength();
    char* buffer = new char[(UINT)fileSize];
    file.Read(buffer, (UINT)fileSize);
    file.Close();

    // 3. 파일명만 추출해서 UTF-8로 변환
    CString fileName = PathFindFileName(m_strSelectedFilePath);  // 경로 제외
    CT2A utf8Name(fileName, CP_UTF8);
    int nameLen = (int)strlen(utf8Name);

    // 4. 전송 순서
    // ① 'F' 헤더
    char header = 'F';
    m_clientSocket.Send(&header, 1);

    // ② 파일명 길이
    m_clientSocket.Send(&nameLen, sizeof(int));

    // ③ 파일명 전송
    m_clientSocket.Send((LPCSTR)utf8Name, nameLen);

    // ④ 파일 내용 전송
    m_clientSocket.Send(buffer, (int)fileSize);

    delete[] buffer;

    CString msg;
    msg.Format(_T("파일 전송 완료: %s"), fileName);
    AfxMessageBox(msg);
}



void CSocketClientDlg::OnBnClickedSend()
{
    CString strMessage;
    m_write.GetWindowText(strMessage);
    if (strMessage.Trim().IsEmpty()) return;

    AppendLogMessage(_T("클라이언트: ") + strMessage);
    m_write.SetWindowText(_T(""));

    if (m_clientSocket.m_hSocket == INVALID_SOCKET)
    {
        AfxMessageBox(_T("서버와 연결되어 있지 않습니다."));
        return;
    }

    // 1. UTF-8 인코딩
    CW2A utf8(strMessage, CP_UTF8);
    int msgLen = (int)strlen(utf8);

    // 2. 전송 순서
    // ① 'M' 헤더 전송
    char header = 'M';
    if (m_clientSocket.Send(&header, 1) == SOCKET_ERROR)
    {
        AfxMessageBox(_T("헤더 전송 실패"));
        return;
    }

    // ② 메시지 길이 전송
    if (m_clientSocket.Send(&msgLen, sizeof(int)) == SOCKET_ERROR)
    {
        AfxMessageBox(_T("길이 전송 실패"));
        return;
    }

    // ③ 본문 전송
    if (m_clientSocket.Send(utf8, msgLen) == SOCKET_ERROR)
    {
        AfxMessageBox(_T("메시지 전송 실패"));
        return;
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