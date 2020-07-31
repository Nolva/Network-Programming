// draftDlg.cpp: 实现文件
//

#include "pch.h"
#include "email.h"
#include "draft.h"
#include "afxdialogex.h"
#include "global.h"


// CDraftDlg 对话框

IMPLEMENT_DYNAMIC(CDraftDlg, CDialogEx)

CDraftDlg::CDraftDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DRAFT, pParent)
{

}

CDraftDlg::~CDraftDlg()
{
}

void CDraftDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDraftDlg, CDialogEx)
END_MESSAGE_MAP()


// CDraftDlg 消息处理程序
void CDraftDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
	DestroyWindow();
}
