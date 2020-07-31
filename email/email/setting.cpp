// pop3.cpp: 实现文件
//

#include "pch.h"
#include "email.h"
#include "setting.h"
#include "afxdialogex.h"
#include "global.h"


// pop3 对话框

IMPLEMENT_DYNAMIC(CSetDlg, CDialogEx)

CSetDlg::CSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SETTING, pParent)
{

}

CSetDlg::~CSetDlg()
{
}

void CSetDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_POP3_SER, m_strPOP3_ser);
	DDX_Text(pDX, IDC_EDIT_POP3_PORT, m_strPOP3_port);
	DDX_Text(pDX, IDC_EDIT_SMTP_SER, m_strSMTP_ser);
	DDX_Text(pDX, IDC_EDIT_SMTP_PORT, m_strSMTP_port);
	DDX_Text(pDX, IDC_EDIT_USER_NAME, m_strUser_name);
	DDX_Text(pDX, IDC_EDIT_USER_PASS, m_strUser_pass);
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SET_SAVE, &CSetDlg::OnBnClickedBtnSetSave)
END_MESSAGE_MAP()


// pop3 消息处理程序


void CSetDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
	DestroyWindow();
}


void CSetDlg::OnBnClickedBtnSetSave()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
}
