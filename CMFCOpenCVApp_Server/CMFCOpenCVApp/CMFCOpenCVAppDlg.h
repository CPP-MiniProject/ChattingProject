#pragma once
#include <opencv2/opencv.hpp>
#include <winsock2.h>
#include "pch.h"
#include "framework.h"
#include <mutex> // std::mutex와 std::lock_guard를 사용하기 위해 필요
#pragma comment(lib, "ws2_32.lib")

using namespace cv;
using namespace std;

// CCMFCOpenCVAppDlg 대화 상자
class CCMFCOpenCVAppDlg : public CDialogEx
{
public:
    CCMFCOpenCVAppDlg(CWnd* pParent = nullptr); // 생성자 선언
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

    int m_socket;
    mutex m_socketMutex; // 소켓 동기화를 위한 mutex

    UINT NetworkThread(LPVOID pParam);
    void SendFrame(SOCKET socket, Mat& frame);
    void RunServerThread();
    Mat ReceiveFrame(SOCKET socket);

    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnBnClickedButtonStop();
};