
// TcpClientTestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TcpClientTest.h"
#include "TcpClientTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString UTF82WCS(const char* szU8)
{
	//预转换，得到所需空间的大小;
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);

	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	//转换
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);

	//最后加上'\0'
	wszString[wcsLen] = '\0';

	CString unicodeString(wszString);

	delete[] wszString;
	wszString = NULL;

	return unicodeString;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTcpClientTestDlg 对话框



CTcpClientTestDlg::CTcpClientTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TCPCLIENTTEST_DIALOG, pParent)
{
	m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTcpClientTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, m_editIP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSend);
	DDX_Control(pDX, IDC_BTN_CONNECT, m_btnConnect);
}

BEGIN_MESSAGE_MAP(CTcpClientTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CTcpClientTestDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_SEND, &CTcpClientTestDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CTcpClientTestDlg 消息处理程序

BOOL CTcpClientTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_editIP.SetWindowText(L"127.0.0.1"/*L"172.26.65.17"*/);
	m_editPort.SetWindowText(L"18866");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTcpClientTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTcpClientTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTcpClientTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTcpClientTestDlg::OnBnClickedBtnConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	m_btnConnect.EnableWindow(FALSE);

	CString strTxt;
	m_btnConnect.GetWindowText(strTxt);
	if (L"连接" == strTxt)
	{
		char cBuffer[256] = { 0 };
		SOCKADDR_IN addrSrv;
		::GetWindowTextA(m_editIP.m_hWnd, cBuffer, 256);
		addrSrv.sin_addr.S_un.S_addr = inet_addr(cBuffer);
		addrSrv.sin_family = AF_INET;
		::GetWindowTextA(m_editPort.m_hWnd, cBuffer, 256);
		addrSrv.sin_port = htons(atoi(cBuffer));
		int nRet = connect(m_sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
		if (SOCKET_ERROR == nRet)
		{
			int nErr = WSAGetLastError();
			CString str;
			str.Format(L"连接失败！err=%d", nErr);
			_printLog(str);
			m_btnConnect.EnableWindow(TRUE);
		}
		else
		{
			_printLog(L"连接服务器成功!");
			m_btnConnect.SetWindowText(L"关闭");
			unsigned int nThreadId = 0;
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, clientThreadProc, (void*)this, 0, &nThreadId);
		}
	}
	else
	{

	}
}


void CTcpClientTestDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	char cBuffer[10240] = { 0 };
	::GetWindowTextA(m_editSend.m_hWnd, cBuffer, 10240);
	std::string strSend = cBuffer;
	if (strSend.empty())
	{
		return;
	}
	int nRet = send(m_sockClient, strSend.c_str(), strSend.size(), 0);
	if (nRet > 0)
	{
		CString strText;
		m_editSend.GetWindowText(strText);
		CString strTemp;
		strTemp.Format(L"发送成功，data=%s", strText);
		_printLog(strTemp);
	}
	else
	{
		int nErr = WSAGetLastError();
		CString str;
		str.Format(L"发送失败！err=%d", nErr);
		_printLog(str);
		m_btnConnect.EnableWindow(TRUE);
	}
}

void CTcpClientTestDlg::_printLog(const CString& strData)
{
	CString strPrint;
	struct tm localTime;
	time_t t = 0;
	t = time(NULL);
	localtime_s(&localTime, &t);
	//char cBuf[64] = { 0 };
	//sprintf_s(cBuf, 64, "%02d:%02d:%02d", localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
	strPrint.Format(L"%02d:%02d:%02d:%03d>>%s\n", localTime.tm_hour, localTime.tm_min, localTime.tm_sec, GetTickCount()%1000, strData);

	m_editLog.SetSel(-1, -1);
	m_editLog.ReplaceSel((LPCTSTR)strPrint);
	m_editLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

unsigned __stdcall CTcpClientTestDlg::clientThreadProc(void* lpParam)
{
	CTcpClientTestDlg* _this = (CTcpClientTestDlg*)lpParam;
	_this->_onRecvRuning();
	return 0;
}

void CTcpClientTestDlg::_onRecvRuning(void)
{
	int nBufLen = 1024 * 64;
	char cBuffer[1024 * 64] = { 0 };
	int nRet = 1;
	while (0 < nRet)
	{
		nRet = recv(m_sockClient, cBuffer, nBufLen, 0);
		if (nRet > 0)
		{
			cBuffer[nRet] = '\0';
			CString strData = UTF82WCS(cBuffer);
			CString strTxt;
			strTxt.Format(L"收到数据，data=%s", strData);
			_printLog(strTxt);
		}
	}
	_printLog(L"与服务器连接已断开！");
}