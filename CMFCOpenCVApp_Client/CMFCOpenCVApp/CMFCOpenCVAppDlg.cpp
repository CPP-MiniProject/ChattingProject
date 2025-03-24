// CMFCOpenCVAppDlg.cpp: Client File

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


// CCMFCOpenCVAppDlg 메시지 처리기
BOOL CCMFCOpenCVAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_bCameraRunning = false;  // 초기 상태는 중지
	capture = new cv::VideoCapture(0); // 카메라 열기

	if (!capture->isOpened()) {
		MessageBox(_T("웹캡을 열 수 없습니다."));
		return FALSE;
	}

	// 웹캡 크기 설정
	capture->set(CAP_PROP_FRAME_WIDTH, 320);
	capture->set(CAP_PROP_FRAME_HEIGHT, 240);

	// Picture Control 영역을 검은색으로 채우기
	CDC* pDC = m_picture.GetDC();
	CRect rect;
	m_picture.GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(0, 0, 0));
	m_picture.ReleaseDC(pDC);

	// 클라이언트 모드로 시작
	m_bIsServer = false;

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);

	if (m_bCameraRunning)
	{
		// 웹캠에서 프레임 읽기
		capture->read(mat_frame);
		if (mat_frame.empty()) {
			MessageBox(_T("웹캡에서 프레임을 읽을 수 없습니다."));
			return;
		}

		// 프레임을 화면에 표시
		DisplayFrame(mat_frame);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CCMFCOpenCVAppDlg::DisplayFrame(Mat& frame)
{
	int bpp = 8 * frame.elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	cv::Mat mat_temp = frame; // 원본 프레임을 사용 (copyMakeBorder 제거)

	// Y-축 반전
	cv::flip(mat_temp, mat_temp, 1);

	RECT r;
	m_picture.GetClientRect(&r);
	cv::Size winSize(r.right, r.bottom);

	// CImage 객체 크기를 Picture Control 크기와 동일하게 설정
	cimage_mfc.Create(winSize.width, winSize.height, 24);

	BITMAPINFO* bitInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	if (bitInfo == nullptr) {
		// 메모리 할당 실패 처리
		cimage_mfc.Destroy();
		return;
	}

	BITMAPINFOHEADER& bih = bitInfo->bmiHeader;
	bih.biBitCount = bpp;
	bih.biWidth = mat_temp.cols;
	bih.biHeight = -mat_temp.rows;
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biCompression = BI_RGB;
	bih.biClrImportant = 0;
	bih.biClrUsed = 0;
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;

	// StretchDIBits 인자 수정
	int destx = 0, desty = 0;
	int destw = winSize.width;
	int desth = winSize.height;
	int imgx = 0, imgy = 0;
	int imgWidth = mat_temp.cols;
	int imgHeight = mat_temp.rows;

	StretchDIBits(cimage_mfc.GetDC(),
		destx, desty, destw, desth,
		imgx, imgy, imgWidth, imgHeight,
		mat_temp.data, bitInfo, DIB_RGB_COLORS, SRCCOPY);

	HDC dc = ::GetDC(m_picture.m_hWnd);
	cimage_mfc.BitBlt(dc, 0, 0);

	::ReleaseDC(m_picture.m_hWnd, dc);
	cimage_mfc.ReleaseDC();
	cimage_mfc.Destroy();
	free(bitInfo);
}

void CCMFCOpenCVAppDlg::OnBnClickedButtonStart()
{
	// 클라이언트 소켓 초기화 및 연결 확인
	if (m_socket == INVALID_SOCKET) {
		AfxMessageBox(_T("소켓이 초기화되지 않았습니다."));
		return;
	}

	// 서버에 연결
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr);
	serverAddr.sin_port = htons(8080);

	if (connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		AfxMessageBox(_T("서버 연결 실패"));
		return;
	}

	// 서버 연결 성공 후 카메라 시작
	if (!capture->isOpened()) {
		MessageBox(_T("웹캡을 열 수 없습니다."));
		return;
	}

	// UI 상태 업데이트
	SetTimer(1000, 20, NULL);
	m_bCameraRunning = true;

	// UI 버튼 상태 변경
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);

	// 클라이언트 스레드 시작
	thread clientThread(&CCMFCOpenCVAppDlg::RunClientThread, this);
	clientThread.detach(); // 스레드 분리
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
	pDC->FillSolidRect(&rect, RGB(0, 0, 0)); // 검은색으로 채우기 (다른 색상으로 변경 가능)
	m_picture.ReleaseDC(pDC);
}


// CMFCOpenCVAppDlg.cpp: Client File

UINT CCMFCOpenCVAppDlg::NetworkThread(LPVOID pParam)
{
	CCMFCOpenCVAppDlg* pDlg = (CCMFCOpenCVAppDlg*)pParam;
	pDlg->RunClientThread();
	return 0;
}


void CCMFCOpenCVAppDlg::RunClientThread()
{
	// 클라이언트 소켓 설정
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET) {
		AfxMessageBox(_T("소켓 생성 실패"));
		return;
	}

	// 서버 주소 설정
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr);
	serverAddr.sin_port = htons(8080);

	// 서버에 연결
	if (connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		AfxMessageBox(_T("서버 연결 실패"));
		return;
	}

	// 이제부터 카메라를 실행
	while (m_bCameraRunning) {
		// 서버로부터 영상 수신
		Mat receivedFrame = ReceiveFrame(m_socket);
		if (!receivedFrame.empty()) {
			ShowReceivedImage(receivedFrame);
		}

		// 클라이언트 영상 전송
		if (m_bCameraRunning) {
			capture->read(mat_frame);
			if (!mat_frame.empty()) {
				SendFrame(m_socket, mat_frame);
			}
		}
	}

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
		return Mat();  // 수신 실패 시 빈 Mat 반환
	}

	// 2. 프레임 데이터 수신
	vector<uchar> buf(frameSize);
	bytesReceived = recv(socket, (char*)buf.data(), frameSize, 0);
	if (bytesReceived <= 0) {
		return Mat();  // 수신 실패 시 빈 Mat 반환
	}

	// 3. 수신된 데이터를 OpenCV Mat 객체로 디코딩
	Mat frame = imdecode(buf, IMREAD_COLOR);
	return frame;  // 디코딩된 프레임 반환
}

// 수신된 이미지를 화면에 표시하는 함수
void CCMFCOpenCVAppDlg::ShowReceivedImage(Mat& frame)
{
	// 화면에 프레임을 표시
	if (!frame.empty()) {
		DisplayFrame(frame); // DisplayFrame 함수를 사용하여 Picture Control에 이미지 표시
	}
}
