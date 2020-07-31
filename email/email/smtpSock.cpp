#include "pch.h"
#include "smtpSock.h"
#include  "send.h"    //自己添加的包含语句
#include  "Base64.h"

#define MAX_BUFF 20000
/////////////////////////////////////////////////////////////////////////////
// smtpSock

smtpSock::smtpSock()
{
	m_pDlg = NULL;
	state = SMTPFIRST;
	error = "连接不上服务器\r\n";
}

smtpSock::~smtpSock()
{
	m_pDlg = NULL;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(smtpSock, CAsyncSocket)
	//{{AFX_MSG_MAP(smtpSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// smtpSock member functions

//当套接字收到FD_CONNECT消息时，执行此函数，表明连接已建立
void smtpSock::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0)  m_pDlg->Disp(S_CONNECT);
}

//当套接字收到服务器发来的数据时，执行此函数
void smtpSock::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		char buff[MAX_BUFF];             //接收缓冲区
		int rec = Receive(buff, MAX_BUFF);  //接收服务器发来的数据
		buff[rec] = NULL;                  //结尾置为NULL。
		lastMsg = buff;
		AnalyzeMsg();             //分析收到的数据，作不同的处理
	}
	else {
		error = "接收期间发生了错误!\r\n";
		m_pDlg->Disp(S_CLOSE);   //显示信息
	}
}

//当关闭连接时，执行此函数
void smtpSock::OnClose(int nErrorCode)
{
	if (nErrorCode == 0)  m_pDlg->Disp(S_CLOSE);
}
//使套接字类的对话框指针变量指向主对话框
void smtpSock::SetParent(CSendDlg* pDlg)
{
	m_pDlg = pDlg;
}

//退出服务器
void smtpSock::Close()
{
	CString str;
	str.Format("quit%c%c", 13, 10);
	Send((LPCSTR)str, str.GetLength());
	m_pDlg->Disp(S_CLOSE);
	state = SMTPFIRST;
	CAsyncSocket::Close();
	error = "与服务器的连接已经断开\r\n";
}

//分析服务器发来的数据，做出相应的处理
void smtpSock::AnalyzeMsg()
{
	CString s;
	strstream str;
	string check;
	str << (LPCSTR)lastMsg;
	str >> check;

	//根据smtp会话所处的状态做出不同的处理	
	switch (state)
	{
	case SMTPFIRST: //如果已经连接成功，类初始化的时候state为FIRST
		m_pDlg->Disp(S_RECEIVE); //发送消息到窗体
		if (!(check == "220")) {}
		s.Format("EHLO %s%c%c", m_pDlg->m_Name, 13, 10);
		Send((LPCSTR)s, s.GetLength()); //发送EHLO命令
		state = SMTPEHLO;
		break;

	case SMTPEHLO:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "250")) {}
		s.Format("AUTH LOGIN%c%c", 13, 10);
		Send((LPCSTR)s, s.GetLength()); //发送AUTH LOGIN命令
		state = SMTPAUTH;
		break;

	case SMTPAUTH:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "334")) {}
		coder.Encode(m_strUser_name);
		s.Format(_T("%s%c%c"), coder.EncodedMessage(), 13, 10);
		//	AfxMessageBox(s);

		//		s.Format("bXVfbA==%c%c",13,10);  ///mu_l
		//		s.Format("bXVfbGxs%c%c",13,10);  //mu_lll
		Send((LPCSTR)s, s.GetLength()); //发送用户名
		state = SMTPUSER;
		break;

	case SMTPUSER:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "334")) {}

		coder.Encode(m_strUser_pass);
		s.Format(_T("%s%c%c"), coder.EncodedMessage(), 13, 10);
		//	AfxMessageBox(s);
		//		s.Format("NDkxMDE1%c%c",13,10); //491015
		Send((LPCSTR)s, s.GetLength()); //发送口令
		state = SMTPPASS;
		break;

	case SMTPPASS:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "235")) {}
		s.Format("MAIL FROM:<%s>%c%c", m_strUser_name, 13, 10);
		//		s.Format("MAIL FROM:<mu_lm@sina.com>%c%c",13,10); 
		Send((LPCSTR)s, s.GetLength()); //发送MAIL FROM命令
		state = SMTPMAIL;
		break;
	case SMTPMAIL:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "250")) {}
		//		s.Format("RCPT TO: <mu_lm@sina.com>%c%c",13,10); 
		s.Format("RCPT TO: <%s>%c%c", m_pDlg->m_Receiver, 13, 10);
		Send((LPCSTR)s, s.GetLength()); //发送RECP命令
		state = SMTPRCPT;
		break;
	case SMTPRCPT:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "250")) {}
		s.Format("DATA%c%c", 13, 10);
		Send((LPCSTR)s, s.GetLength()); //发送DATA命令
		state = SMTPDATA;
		break;
	case SMTPDATA:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "354")) {}
		//		s.Format("From: %s%c%c",m_pDlg->m_Addr,13,10); 
		//		Send((LPCSTR)s,s.GetLength()); //发送信件内容

		//	s.Format("To: %s%c%c",m_pDlg->m_Receiver,13,10); 
		//	Send((LPCSTR)s,s.GetLength());

		//	s.Format("Date: Tue,04 Dec 2005 16:18:08 +800%c%c",13,10); 
		//	Send((LPCSTR)s,s.GetLength());

		//	s.Format("Subject: %s%c%c",m_pDlg->m_Title,13,10); 
		//	Send((LPCSTR)s,s.GetLength());

		//	s.Format("%c%c",13,10); 
		//	Send((LPCSTR)s,s.GetLength());

		//	s.Format("%s%c%c",m_pDlg->m_Letter,13,10); 
		//	Send((LPCSTR)s,s.GetLength());

		//		s.Format("%c%c.%c%c",13,10,13,10); 
		//		Send((LPCSTR)s,s.GetLength());

		m_pDlg->GetHeader();
		state = SMTPQUIT;
		break;
	case SMTPQUIT:
		m_pDlg->Disp(S_RECEIVE);
		if (!(check == "250")) {}
		s.Format("QUIT%c%c", 13, 10);
		Send((LPCSTR)s, s.GetLength()); //发送QUIT命令
		state = SMTPDATA;
		break;
	default:
		m_pDlg->Disp(S_RECEIVE);
		break;
	}
}
