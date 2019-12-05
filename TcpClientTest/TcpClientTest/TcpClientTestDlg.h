
// TcpClientTestDlg.h: 头文件
//

#pragma once


// CTcpClientTestDlg 对话框
class CTcpClientTestDlg : public CDialogEx
{
// 构造
public:
	CTcpClientTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCPCLIENTTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void _printLog(const CString& strData);

	static unsigned __stdcall clientThreadProc(void* lpParam);
	void _onRecvRuning(void);

public:
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnSend();
	CEdit m_editIP;
	CEdit m_editPort;
	CRichEditCtrl m_editLog;
	CRichEditCtrl m_editSend;

	SOCKET m_sockClient;
	SOCKADDR_IN m_addrSrv;
	CButton m_btnConnect;
};
