
// emailDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "email.h"
#include "emailDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CEmailDlg 对话框



CEmailDlg::CEmailDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EMAIL_DIALOG, pParent)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEmailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEmailDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SET, &CEmailDlg::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_SEND, &CEmailDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_RECEIVE, &CEmailDlg::OnBnClickedBtnReceive)
	ON_BN_CLICKED(IDC_BTN_DRAFT, &CEmailDlg::OnBnClickedBtnDraft)
END_MESSAGE_MAP()


// CEmailDlg 消息处理程序

BOOL CEmailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//子对话框初始化
	GetDlgItem(IDC_BTN_SET)->EnableWindow(FALSE);
	cSetDlg.Create(IDD_DIALOG_SETTING, this);
	cSetDlg.SetWindowPos(NULL, 150, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	cSetDlg.ShowWindow(SW_SHOW);
	cSendDlg.Create(IDD_DIALOG_SEND, this);
	cSendDlg.SetWindowPos(NULL, 150, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	cSendDlg.ShowWindow(SW_HIDE);
	cReceiveDlg.Create(IDD_DIALOG_RECEIVE, this);
	cReceiveDlg.SetWindowPos(NULL, 150, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	cReceiveDlg.ShowWindow(SW_HIDE);
	cDraftDlg.Create(IDD_DIALOG_DRAFT, this);
	cDraftDlg.SetWindowPos(NULL, 150, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	cSendDlg.ShowWindow(SW_HIDE);

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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEmailDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEmailDlg::OnPaint()
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
HCURSOR CEmailDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEmailDlg::OnBnClickedBtnSet()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_SET)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RECEIVE)->EnableWindow(TRUE);
	//GetDlgItem(IDC_BTN_DRAFT)->EnableWindow(TRUE);
	cSetDlg.ShowWindow(SW_SHOW);
	cSendDlg.ShowWindow(SW_HIDE);
	cReceiveDlg.ShowWindow(SW_HIDE);
	//cDraftDlg.ShowWindow(SW_HIDE);
}


void CEmailDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_SET)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_RECEIVE)->EnableWindow(TRUE);
	//GetDlgItem(IDC_BTN_DRAFT)->EnableWindow(TRUE);
	cSetDlg.ShowWindow(SW_HIDE);
	cSendDlg.ShowWindow(SW_SHOW);
	cReceiveDlg.ShowWindow(SW_HIDE);
	//cDraftDlg.ShowWindow(SW_HIDE);
}


void CEmailDlg::OnBnClickedBtnReceive()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_SET)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RECEIVE)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BTN_DRAFT)->EnableWindow(TRUE);
	cSetDlg.ShowWindow(SW_HIDE);
	cSendDlg.ShowWindow(SW_HIDE);
	cReceiveDlg.ShowWindow(SW_SHOW);
	//cDraftDlg.ShowWindow(SW_HIDE);
}


void CEmailDlg::OnBnClickedBtnDraft()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_SET)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RECEIVE)->EnableWindow(TRUE);
	//GetDlgItem(IDC_BTN_DRAFT)->EnableWindow(FALSE);
	cSetDlg.ShowWindow(SW_HIDE);
	cSendDlg.ShowWindow(SW_HIDE);
	cReceiveDlg.ShowWindow(SW_HIDE);
	//cDraftDlg.ShowWindow(SW_SHOW);
}
