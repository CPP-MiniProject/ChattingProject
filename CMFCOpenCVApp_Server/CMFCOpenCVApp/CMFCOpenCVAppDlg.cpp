// CMFCOpenCVAppDlg.cpp: Sever File

#include "pch.h"
#include "framework.h"
#include "CMFCOpenCVApp.h"
#include "CMFCOpenCVAppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCMFCOpenCVAppDlg 대화 상자



CCMFCOpenCVAppDlg::CCMFCOpenCVAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CMFCOPENCVAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCMFCOpenCVAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
}

BEGIN_MESSAGE_MAP(CCMFCOpenCVAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CCMFCOpenCVAppDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CCMFCOpenCVAppDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


BOOL CCMFCOpenCVAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Winsock 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		AfxMessageBox(_T("Winsock 초기화 실패"));
		return FALSE; // 초기화 실패 시 FALSE 반환
	}

	// 웹캡 크기 설정
	capture->set(CAP_PROP_FRAME_WIDTH, 320);
	capture->set(CAP_PROP_FRAME_HEIGHT, 240);

	return TRUE;
}


void CCMFCOpenCVAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCMFCOpenCVAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// m_picture 컨트롤을 검은색으로 다시 채우기
		CDC* pDC = m_picture.GetDC();
		CRect rect;
		m_picture.GetClientRect(&rect);
		pDC->FillSolidRect(&rect, RGB(0, 0, 0));
		m_picture.ReleaseDC(pDC);

		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCMFCOpenCVAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCMFCOpenCVAppDlg::OnDestroy()
{
	// 부모 클래스의 OnDestroy 호출
	CDialogEx::OnDestroy();

	// OpenCV 캡처 객체 해제
	if (capture != nullptr)
	{
		capture->release();
		delete capture;
		capture = nullptr;
	}

	// 소켓 닫기 및 Winsock 종료
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket); // 소켓 닫기
	}

	WSACleanup(); // Winsock 종료
}


void CCMFCOpenCVAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);

	if (m_bCameraRunning && m_socket != INVALID_SOCKET)
	{
		// 서버 영상 전송
		capture->read(mat_frame);
		if (!mat_frame.empty()) {
			SendFrame(m_socket, mat_frame);
		}
	}
}



void CCMFCOpenCVAppDlg::OnBnClickedButtonStart()
{
	if (!capture->isOpened()) {
		MessageBox(_T("웹캡을 열 수 없습니다."));
		return;
	}

	// UI 상태 업데이트
	// UI 업데이트는 반드시 메인 스레드에서 이루어져야 하므로,
	// PostMessage 또는 SendMessage를 활용해서 UI를 업데이트하는 방법을 고려해야 합니다.
	SetTimer(1000, 20, NULL);  // 타이머 설정
	m_bCameraRunning = true;

	// UI 버튼 상태 변경
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}

void CCMFCOpenCVAppDlg::OnBnClickedButtonStop()
{
	KillTimer(1000);
	m_bCameraRunning = false;
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);

	// Picture Control 영역을 지우는 코드 추가
	CDC* pDC = m_picture.GetDC();
	CRect rect;
	m_picture.GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(0, 0, 0)); // 검은색으로 채우기
	m_picture.ReleaseDC(pDC);

	// 카메라 송출 중지
	capture->release();  // 웹캠을 닫음
}

// CMFCOpenCVAppDlg.cpp: Server File

UINT CCMFCOpenCVAppDlg::NetworkThread(LPVOID pParam)
{
	CCMFCOpenCVAppDlg* pDlg = (CCMFCOpenCVAppDlg*)pParam;
	pDlg->RunServerThread();
	return 0;
}


void CCMFCOpenCVAppDlg::RunServerThread()
{
	// 1. 소켓 생성 확인
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET) {
		AfxMessageBox(_T("소켓 생성 실패"));
		return;
	}

	// 2. 서버 주소 초기화
	sockaddr_in serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP에서 접속 가능
	serverAddr.sin_port = htons(8081); // 원하는 포트 번호

	// 3. 재사용 가능한 소켓 옵션 설정
	int optval = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

	// 4. 소켓 바인딩
	if (bind(m_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		AfxMessageBox(_T("bind() 실패"));
		closesocket(m_socket);
		return;
	}

	// 5. 소켓 리스닝
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR) {
		AfxMessageBox(_T("listen() 실패"));
		closesocket(m_socket);
		return;
	}

	// 6. 클라이언트 연결 대기 및 수락
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	SOCKET clientSocket = accept(m_socket, (SOCKADDR*)&clientAddr, &clientAddrSize);
	if (clientSocket == INVALID_SOCKET) {
		AfxMessageBox(_T("accept() 실패"));
		closesocket(m_socket);
		return;
	}

	// 7. 데이터 송수신
	while (m_bCameraRunning) {
		// 클라이언트로부터 영상 수신
		Mat receivedFrame = ReceiveFrame(clientSocket);
		if (receivedFrame.empty()) {
			// 클라이언트 연결 끊김
			m_socket = INVALID_SOCKET;
			break;
		}

		// 서버 영상 전송
		if (m_bCameraRunning && clientSocket != INVALID_SOCKET) {
			capture->read(mat_frame);
			if (!mat_frame.empty()) {
				SendFrame(clientSocket, mat_frame);
			}
		}
	}

	// 8. 소켓 닫기
	closesocket(clientSocket);
	closesocket(m_socket);
}



void CCMFCOpenCVAppDlg::SendFrame(SOCKET socket, Mat& frame)
{
	// 프레임을 네트워크로 전송
	vector<uchar> buf;
	imencode(".jpg", frame, buf);
	int frameSize = buf.size();

	// 먼저 크기 전송
	int sentBytes = send(socket, (char*)&frameSize, sizeof(frameSize), 0);
	if (sentBytes == SOCKET_ERROR) {
		AfxMessageBox(_T("프레임 크기 전송 실패"));
		return;
	}

	// 실제 이미지 데이터 전송
	sentBytes = send(socket, (char*)buf.data(), frameSize, 0);
	if (sentBytes == SOCKET_ERROR) {
		AfxMessageBox(_T("프레임 데이터 전송 실패"));
		return;
	}
}


// 서버로부터 프레임을 수신하는 함수
Mat CCMFCOpenCVAppDlg::ReceiveFrame(SOCKET socket)
{
	// 1. 프레임 크기 수신
	int frameSize = 0;
	int bytesReceived = recv(socket, (char*)&frameSize, sizeof(frameSize), 0);
	if (bytesReceived <= 0) {
		// 연결이 끊어졌거나 에러가 발생한 경우 에러 메시지를 출력하고 빈 Mat 반환
		AfxMessageBox(_T("클라이언트 연결이 끊어졌습니다."));
		return Mat();
	}

	// 2. 프레임 데이터 수신
	vector<uchar> buf(frameSize);
	bytesReceived = recv(socket, (char*)buf.data(), frameSize, 0);
	if (bytesReceived <= 0) {
		// 데이터 수신 실패 시 에러 메시지를 출력하고 빈 Mat 반환
		AfxMessageBox(_T("프레임 데이터 수신 실패"));
		return Mat();
	}

	// 3. 수신된 데이터를 OpenCV Mat 객체로 디코딩
	Mat frame = imdecode(buf, IMREAD_COLOR);
	return frame; // 디코딩된 프레임 반환
}