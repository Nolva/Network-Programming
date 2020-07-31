// receiveDlg.cpp: 实现文件
//

#include "pch.h"
#include "email.h"
#include "receive.h"
#include "afxdialogex.h"
#include "global.h"


// CReceiveDlg 对话框

IMPLEMENT_DYNAMIC(CReceiveDlg, CDialogEx)

CReceiveDlg::CReceiveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RECEIVE, pParent)
{
	m_bolDel = FALSE;
	m_Info = _T("");
}

CReceiveDlg::~CReceiveDlg()
{
}

void CReceiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_RECEIVE, m_ctrInfo);
	DDX_Control(pDX, IDC_COMBO_RECEIVE, m_ctrList);
	//DDX_Text(pDX, IDC_EDIT_PASS, m_strPass);
	//DDX_Text(pDX, IDC_EDIT_SERVER, m_strServer);
	//DDX_Text(pDX, IDC_EDIT_USER, m_strUser);
	DDX_Check(pDX, IDC_CHECK_RECEIVE, m_bolDel);
	DDX_Text(pDX, IDC_RICHEDIT_RECEIVE, m_Info);
}


BEGIN_MESSAGE_MAP(CReceiveDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_VIEW, &CReceiveDlg::OnBnClickedBtnView)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CReceiveDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CReceiveDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_DISCONECT, &CReceiveDlg::OnBnClickedBtnDisconect)
END_MESSAGE_MAP()


// CReceiveDlg 消息处理程序
void CReceiveDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
	DestroyWindow();
}

//在不同的情况下，向用户显示不同的信息
void CReceiveDlg::Disp(LONG flag)
{
	CString s;
	switch (flag)
	{
	case S_CONNECT: //已连接到服务器，显示信息
		s = "已连接到" + m_strPOP3_ser + "...\r\n";
		m_Info += s;
		//禁用连接按钮
		GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
		//启用断开按钮
		GetDlgItem(IDC_BTN_DISCONECT)->EnableWindow(TRUE);
		//禁用查看邮件按钮
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(FALSE);
		break;
	case S_RECEIVE: //收到服务器发来的响应，显示它的内容
		//m_Info += pop3Socket.lastMsg;
		break;
	case S_CLOSE: //显示关闭连接的信息
		m_Info += pop3Socket.error;
		s = "连接已经关闭\r\n";
		m_Info += s;
		m_Info += "----------------------------\n";
		//启用连接按钮
		GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
		//禁用断开按钮
		GetDlgItem(IDC_BTN_DISCONECT)->EnableWindow(FALSE);
		break;
	case S_GETNUM: //已经知道了邮箱中信件的总数
		s.Format("邮箱中共有 %d 封信\r\n", pop3Socket.numMsg);
		m_Info += s;
		break;
	case S_GETSIZE: //显示信件的大小
		s.Format("信箱中信件大小为：%d字节\r\n", pop3Socket.sizeMsg);
		m_Info += s;
		break;
	case S_ENDRETR: //所有信件已下载完毕，显示信息
		s.Format("共下载了 %d 封信件\r\n", pop3Socket.msgs.size());
		m_Info += s;
		//如果有信件下载，启用查看邮件按钮
		if ((pop3Socket.msgs.size()) > 0)
			GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
		break;
	}
	UpdateData(FALSE);   //更新用户界面
}



void CReceiveDlg::OnBnClickedBtnView()
{
	// TODO: 在此添加控件通知处理程序代码
	int i;
	i = m_ctrList.GetCurSel();  //获得用户在组合框中的选择
	//获得并显示指定信件的基本属性及信件内容
	m_Info += pop3Socket.GetMsgStuff(i);
	m_Info += pop3Socket.GetMsgBody(i);
	m_Info += "-------------------\n";
	//	CString s;
	//	for(int i=0;i<pop3Socket.retrMsg;i++)
	//	{
	//		s.Format("%c%c第 %d 封信 %c%c",13,10,i,13,10);
	//		m_Info +=s;
	//		m_Info += pop3Socket.msgs[i].text;
	//	}
	UpdateData(FALSE);
}


void CReceiveDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	int i;
	CString s;
	i = m_ctrList.GetCurSel();
	//获得指定信件的基本属性及信件内容
	s = pop3Socket.GetMsgStuff(i);
	s += pop3Socket.GetMsgBody(i);

	CFileDialog dlg(FALSE);
	if (dlg.DoModal() == IDOK)
	{
		CFile file(dlg.GetPathName(),
			CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write((LPCSTR)s, s.GetLength());
		file.Close();
	}
}


void CReceiveDlg::OnBnClickedBtnConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	//设定pop3类的对话框指针变量，使之指向本对话框，以便传递信息
	pop3Socket.SetParent(this);
	UpdateData(TRUE);           //取来用户在对话框中输入的数据
	pop3Socket.Create();        //创建套接字对象的底层套接字
	pop3Socket.Connect((LPCSTR)m_strPOP3_ser, atoi(m_strPOP3_port)); //连接pop3服务器
	m_Info = "";                //多文本列表框清空
	UpdateData(FALSE);          //更新用户界面
}


void CReceiveDlg::OnBnClickedBtnDisconect()
{
	// TODO: 在此添加控件通知处理程序代码
	pop3Socket.Close();
}
