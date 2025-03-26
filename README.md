# 💬 MFC 기반 소켓 채팅 및 파일 전송 프로그램

![MFC](https://img.shields.io/badge/MFC-ChatApp-indigo)
![C++](https://img.shields.io/badge/C%2B%2B-Socket%20Programming-blueviolet)
![Winsock](https://img.shields.io/badge/Winsock-2.2-gray)
![Client-Server](https://img.shields.io/badge/Client%20%26%20Server-TCP-darkorange)
![TCP/IP](https://img.shields.io/badge/Protocol-TCP%2FIP-mediumseagreen)
![FileTransfer](https://img.shields.io/badge/File-Transfer-green)

---

## 📌 프로젝트 개요
본 프로젝트는 **Microsoft Foundation Class (MFC)** 기반의 **TCP/IP 소켓 통신 프로그램**입니다.  
**클라이언트-서버 구조**로 이루어진 이 애플리케이션은 **실시간 채팅 기능과 파일 송수신 기능**을 제공합니다.  
MFC의 GUI 컴포넌트와 이벤트 처리 흐름을 적극 활용하여, **사용자 친화적인 인터페이스와 안정적인 통신 흐름**을 구현하였습니다.

---

## 🛠️ 주요 기술 스택
- **언어**: C++ (Visual Studio, MFC)
- **프레임워크**: Microsoft Foundation Class (MFC)
- **통신 방식**: TCP/IP (**비동기 소켓 통신, `CAsyncSocket` 기반**)
- **라이브러리**: WinSock2
- **개발 도구**: Visual Studio 2022

---

## 🚀 주요 기능
1. **클라이언트-서버 실시간 채팅**
   - TCP 기반 양방향 메시지 전송
   - UTF-8 인코딩으로 한글 메시지도 정확하게 전달
2. **실시간 상태 표시**
   - 서버/클라이언트 연결 상태 시각화
3. **파일 전송 기능**
   - 텍스트/이미지/이진 파일 등 다양한 형식의 **파일 송수신 지원**
   - 파일명, 크기, 데이터 전송 순서 프로토콜 설계
   - 소형 파일 안정적 전송
4. **예외 처리 및 사용자 알림**
   - 메시지 길이 오류, 연결 실패 등 상황별 알림 (`AfxMessageBox` 활용)
5. **확장 가능한 구조**
   - 다중 클라이언트 처리, 파일 전송, 메시지 암호화 등의 기능 확장 고려
   - 추후 **영상 통화 기능** 추가 예정

---

## 🌐 연결 방식

- 서버 실행 전, 서버 PC의 **IPv4 주소**를 `ipconfig` 명령어로 확인
- 클라이언트는 해당 IP와 포트를 입력하여 연결 시도
- 테스트 환경에서는 `127.0.0.1`(localhost) 사용 가능

> 🧪 서로 다른 컴퓨터에서 테스트하려면 **같은 네트워크**에 연결되어 있어야 합니다.

---

## 📂 파일 전송 기능 설명

- **파일 전송 순서**
  1. `'F'` 헤더 전송
  2. 파일명 길이 (`int`) 전송
  3. 파일명 (UTF-8 인코딩)
  4. 파일 크기 (`int`) 전송
  5. 파일 데이터 전송

- **파일 수신 처리**
  - 정확한 바이트 수 수신을 위한 루프 처리
  - 수신 완료 후 사용자에게 알림 표시

- **지원 파일 예시**
  - `.txt`, `.jpg`, `.png`, `.pdf`, `.zip` 등

- **제한 사항**
  - 기본적으로 단일 파일만 전송 가능
  - 전송 중 중단 시 처리 미구현 (향후 예외 처리 계획 있음)

---

## 💡 사용 방법

### ▶ 메시지 전송

1. 서버 프로그램 실행 → "서버 통신" 버튼 클릭  
2. 클라이언트 프로그램 실행 → "서버 연결" 버튼 클릭  
3. 메시지 입력 후 Enter 키 또는 "메시지 전송" 버튼으로 전송

### 📎 파일 전송

1. 클라이언트에서 "파일 선택" 버튼 클릭 → 원하는 파일 선택  
2. "파일 전송" 버튼 클릭  
3. 서버에서 해당 파일이 자동 저장되며 완료 알림 출력

---

## 🔧 디렉토리 구조
```
📦 MFC_ChatApp
├── Server
│   ├── SocketServer.cpp / .h
│   ├── SocketServerDlg.cpp / .h
│   ├── CServerSocket.cpp / .h
│   └── CClientSocket.cpp / .h
├── Client
│   ├── SocketClient.cpp / .h
│   ├── SocketClientDlg.cpp / .h
│   └── CClientSocket.cpp / .h
└── README.md
```

---

## 👥 팀원 소개
| 이름 | 역할 | 주요 업무 | GitHub |
|------|------|-----------|--------|
| **이경준** | 팀장 / 개발자 | 서버/클라이언트 전체 통신 구조 설계 및 구현, 채팅 및 파일 송수신 기능 개발, 통신 프로토콜 설계 및 구현 | [![GitHub](https://img.shields.io/badge/GitHub-KYEONGJUN-LEE-black?logo=github)](https://github.com/KYEONGJUN-LEE) |
| **김다운** | 개발자 | 서버/클라이언트 전체 통신 구조 설계 및 구현, 사용자 인터페이스 개발, 통신 프로토콜 설계 및 구현 | [![GitHub](https://img.shields.io/badge/GitHub-dawoonykim-black?logo=github)](https://github.com/dawoonykim) |


---

## 💡 배운 점
- MFC 기반 UI 컴포넌트 사용 및 메시지 핸들링 이해
- C++에서 비동기 소켓 프로그래밍 구조 경험
- 실시간 데이터 송수신에서의 예외 처리 중요성 인식
- 구조 분리(CServerSocket vs CClientSocket)와 유지보수의 연관성 학습

---

## 🔮 향후 개선 계획
- 다중 클라이언트 동시 접속 지원
- 파일 전송 시 진행률 바 표시
- 전송 중 취소 기능 추가
- 메시지 암호화 및 보안 통신 적용 (TLS 등)
- 📹 **영상 통화 기능 추가 예정**
