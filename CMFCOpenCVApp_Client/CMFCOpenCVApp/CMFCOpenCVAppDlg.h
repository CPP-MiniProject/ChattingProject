#pragma once
#include <opencv2/opencv.hpp>
#include <winsock2.h>
#include <mutex>  // std::mutex와 std::lock_guard를 사용하기 위해 필요

#if (_WIN32_WINNT >= 0x0600) // Windows Vista 이상
#include <ws2tcpip.h> // InetPton 함수를 사용하기 위한 헤더 파일
#else
#include <ws2ipdef.h> // inet_addr 함수를 사용하기 위한 헤더 파일
#endif

using namespace cv;
using namespace std;

// CCMFCOpenCVAppDlg 대화 상자
class CCMFCOpenCVAppDlg : public CDialogEx
{
public:
    CCMFCOpenCVAppDlg(CWnd* pParent = nullptr);  // 생성자 선언
    virtual BOOL OnInitDialog();
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CMFCOPENCVAPP_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;
    VideoCapture* capture;
    Mat mat_frame;
    CStatic m_picture;
    CImage cimage_mfc;
    bool m_bCameraRunning; // 카메라 캡처 상태

    SOCKET m_socket;
    bool m_bIsServer; // 서버 또는 클라이언트 여부
    mutex m_socketMutex;  // 소켓 동기화를 위한 mutex

    UINT NetworkThread(LPVOID pParam);
    void SendFrame(SOCKET socket, Mat& frame);
    Mat ReceiveFrame(SOCKET socket);
    void ShowReceivedImage(Mat& frame);
    void RunServer();
    void RunClientThread();
    void CCMFCOpenCVAppDlg::DisplayFrame(Mat& frame);


    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnBnClickedButtonStop();
};
