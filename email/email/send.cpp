// smtp.cpp: 实现文件
//

#include "pch.h"
#include "email.h"
#include "send.h"
#include "afxdialogex.h"
#include "global.h"
#include "smtpSock.h"
#include "Base64.h"

#define SMTP_MAXLINE  76

// smtp 对话框

IMPLEMENT_DYNAMIC(CSendDlg, CDialogEx)

CSendDlg::CSendDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SEND, pParent)
{
	//m_Addr = _T("");
	m_Attach = _T("");
	m_BCC = _T("");
	m_CC = _T("");
	m_Letter = _T("");
	m_Name = _T("");
	//m_Pass = _T("");
	//m_Port = 0;
	m_Receiver = _T("");
	//m_Server = _T("");
	m_Title = _T("");
	//m_User = _T("");
	m_Info = _T("");
}

CSendDlg::~CSendDlg()
{
}


void CSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_EDIT_ADDR, m_Addr);
	DDX_Text(pDX, IDC_EDIT_SEND_ATTACH, m_Attach);
	DDX_Text(pDX, IDC_EDIT_SEND_BCC, m_BCC);
	DDX_Text(pDX, IDC_EDIT_SEND_CC, m_CC);
	DDX_Text(pDX, IDC_EDIT_SEND_CONTENT, m_Letter);
	//DDX_Text(pDX, IDC_EDIT_NAME, m_Name);
	//DDX_Text(pDX, IDC_EDIT_PASS, m_Pass);
	//DDX_Text(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Text(pDX, IDC_EDIT_SEND_RECEIVE, m_Receiver);
	//DDX_Text(pDX, IDC_EDIT_SERVER, m_Server);
	DDX_Text(pDX, IDC_EDIT_SEND_TITLE, m_Title);
	DDX_Text(pDX, IDC_EDIT_SEND_SEND, m_Name);
	DDX_Text(pDX, IDC_RICH_SEND, m_Info);
}



BEGIN_MESSAGE_MAP(CSendDlg, CDialogEx)
	//ON_BN_CLICKED(IDC_BTN_BROWSE, &CSendDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CSendDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_SEND, &CSendDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// smtp 消息处理程序
void CSendDlg::OnCancel()
{
	DestroyWindow();
}

//void CSendDlg::OnBnClickedBtnBrowse()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	TRACE(m_strPOP3_ser);
//}


void CSendDlg::OnBnClickedBtnBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		CString sNewFile = dlg.GetPathName();
		if (m_Attach.GetLength())
		{
			m_Attach += _T(", ");
			m_Attach += sNewFile;
		}
		else
			m_Attach = sNewFile;
		UpdateData(FALSE);
	}
	LPSTR pszBody = NULL;
	int nBodySize = 0;
	if (!GetBody(pszBody, nBodySize))
	{
		TRACE(_T("Failed in call to send body parts body, GetLastError returns: %d\n"), GetLastError());
	}
	CString s;
	s = pszBody;
	m_Info += s;
	UpdateData(FALSE);
}


void CSendDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码
		//设定smtp类的变量，使之指向本对话框，以便传递信息
	smtpSocket.SetParent(this);
	UpdateData(TRUE);           //取来用户在对话框中输入的数据
	smtpSocket.Create();        //创建套接字对象的底层套接字
	smtpSocket.Connect((LPCSTR)m_strSMTP_ser, atoi(m_strSMTP_port)); //连接pop3服务器
	//列表框清空
	//while (m_listInfo.GetCount()!=0)
	 //          m_listInfo.DeleteString(0);

	//UpdateData(FALSE);          //更新用户界面
}


void CSendDlg::Disp(LONG flag)
{
	CString s;
	switch (flag)
	{
	case S_CONNECT: //已连接到服务器，显示信息
		s = "已经连接到" + m_strSMTP_ser + "服务器\r\n";
		m_Info += s;
		//m_listInfo.AddString(s);
		break;
	case S_RECEIVE: //收到服务器发来的数据,显示该数据
		m_Info += smtpSocket.lastMsg;
		//m_listInfo.AddString(smtpSocket.lastMsg);
		break;
	case S_CLOSE: //显示关闭连接的信息
		m_Info += smtpSocket.error;
		//		m_listInfo.AddString(smtpSocket.error);
		s = "连接已经关闭\r\n";
		m_Info += s;
		//m_listInfo.AddString(s); 
		break;
	}
	UpdateData(FALSE);   //更新用户界面
}

void CSendDlg::GetHeader()
{
	UpdateData(TRUE);
	CString sepa;
	CString sReply;
	sReply = _T("");

	//创建 "Date:" 标题行内容
	CTime now(CTime::GetCurrentTime());
	CString sDate(now.Format(_T("%a, %d %b %Y %H:%M:%S ")));
	sDate += "+0800 (CST)";

	CString sBuf(sReply);
	if (m_Attach.GetLength())
	{
		sReply.Format(_T("MIME-Version: 1.0\r\n"));
		sBuf += sReply;
	}

	//添加 From 和 to 字段
	//From: "=?GB2312?B?XXXXXXXXXX=?=" // 发信人，进行了编码
	//To: Li@163.com 

	coder.Encode(m_strUser_name);
	sReply.Format(_T("From: %s\r\n"), m_strUser_name);
	//	sReply.Format(_T("From: =?gb2312?B?%s?=\r\n"), coder.EncodedMessage());
	sBuf += sReply;

	sReply.Format(_T("To: %s\r\n"), m_Receiver);
	sBuf += sReply;

	//添加 Date字段
	//Date: Wed, 8 Apr 2004 16:16:16 +0800 // 发信的日期时间
	sReply.Format(_T("Date: %s\r\n"), sDate);
	sBuf += sReply;

	//添加 subject字段
	//Subject: =?GB2312?B?XXXXXX=?= // 主题，进行了编码
	coder.Encode(m_Title);
	sReply.Format(_T("Subject: %s\r\n"), m_Title);
	//	sReply.Format(_T("Subject: =?gb2312?B?%s?=\r\n"),coder.EncodedMessage());
	sBuf += sReply;

	//如果有，添加 Cc 字段
	if (m_CC.GetLength())
	{
		sReply.Format(_T("Cc: %s\r\n"), m_CC);
		sBuf += sReply;
	}

	//如果有，添加Bcc 字段
	if (m_BCC.GetLength())
	{
		sReply.Format(_T("Bcc: %s\r\n"), m_BCC);
		sBuf += sReply;
	}

	//如果需要，添加 Mime 字段
	//MIME-Version: 1.0               // MIME版本
	//Content-type: multipart/mixed;  // 内容类型是多部分/混合型
	//boundary = "NextPart_000_00A"  // 指定一级边界特征字符串

	sepa = _T("Boundary-=_HfNFaIwtPvzJDUQrvChaEKIMklNx");
	if (m_Attach.GetLength())
	{
		//		sReply.Format(_T("MIME-Version: 1.0\r\n"));
		//		sBuf += sReply;
		sReply.Format("Content-Type:Multipart/mixed;boundary=%s\r\n", sepa);
		sBuf += sReply;
		sBuf += _T("\r\n");
	}
	else {
		sBuf += _T("\r\n"); sBuf += _T("data:");
		sReply.Format(_T(" %s\r\n"), m_Letter);
		m_Letter += "\0";
		char* mLetter = m_Letter.GetBuffer();
		size_t count = 0;
		
		count = strlen(mLetter) + 1;
		char* B = RC4(mLetter, key);
		CString cs = B;
		TRACE(mLetter);
		TRACE(cs);
		cs.Delete(cs.GetLength()-1);
		sBuf += (CString)B;

		//sBuf += sReply;
		sReply.Format(_T("%c%c.%c%c"), 13, 10, 13, 10);
		sBuf += sReply;
	}
	smtpSocket.Send((LPCSTR)sBuf, sBuf.GetLength());
	m_Info += sBuf;

	if (m_Attach.GetLength())
	{
		sReply.Format(_T("--%s\r\n"), sepa);
		sBuf = sReply;

		sBuf += _T("Content-Type: text/plain; charset='gb2312'\r\n");
		sBuf += _T("Content-Transfer-Encoding: base64\r\n");
		sBuf += _T("\r\n");

		coder.Encode(m_Letter);
		sReply.Format(_T("%s\r\n"), coder.EncodedMessage());
		sBuf += sReply;

		sReply.Format(_T("--%s\r\n"), sepa);
		sBuf += sReply;

		sBuf += _T("Content-Type: text/plain; charset='gb2312'\r\n");
		sBuf += _T("Content-Transfer-Encoding: base64\r\n");
		sBuf += _T("\r\n");

		//add Letter	
		LPSTR pszBody = NULL;
		int nBodySize = 0;
		if (!GetBody(pszBody, nBodySize))
		{
			TRACE(_T("Failed in call to send body parts body, GetLastError returns: %d\n"), GetLastError());
		}
		sReply = pszBody;
		sBuf += sReply;
		sReply.Format(_T("--%s\r\n"), sepa);
		sBuf += sReply;

		sReply.Format(_T("%c%c.%c%c"), 13, 10, 13, 10);
		sBuf += sReply;
		smtpSocket.Send((LPCSTR)sBuf, sBuf.GetLength());
		m_Info += sBuf;
	}
	UpdateData(FALSE);
}

BOOL CSendDlg::GetBody(LPSTR& pszBody, int& nBodySize)
{
	BOOL bSuccess = FALSE;

	//打开附件文件
	CFile infile;
	if (infile.Open(m_Attach, CFile::modeRead | CFile::shareDenyWrite))
	{
		DWORD dwSize = infile.GetLength();
		if (dwSize)
		{
			//读入数据
			BYTE* pszIn = new BYTE[dwSize];
			try
			{
				infile.Read(pszIn, dwSize);
				bSuccess = TRUE;
			}
			catch (CFileException* pEx)
			{
				bSuccess = FALSE;
				pEx->Delete();
			}

			if (bSuccess)
			{
				coder.Encode(pszIn, dwSize); //编码
				delete[] pszIn;               //删除了输入缓冲区
				infile.Close();                //关闭输入文件

				//形成编码后的发送内容
				LPSTR pszEncoded = coder.EncodedMessage();
				int nEncodedSize = coder.EncodedMessageSize();
				nBodySize = nEncodedSize + (((nEncodedSize / 76) + 1) * 2) + 1;
				pszBody = new char[nBodySize];
				--nBodySize;

				int nInPos = 0;
				int nOutPos = 0;
				while (nInPos < nEncodedSize)
				{
					int nThisLineSize = min(nEncodedSize - nInPos, SMTP_MAXLINE);
					CopyMemory(&pszBody[nOutPos], &pszEncoded[nInPos], nThisLineSize);
					nOutPos += nThisLineSize;
					CopyMemory(&pszBody[nOutPos], "\r\n", 2);
					nOutPos += 2;
					nInPos += nThisLineSize;
				}
				pszBody[nOutPos] = '\0'; //以空字符串结束
			}
		}
		else {
			bSuccess = TRUE;
			pszBody = NULL;
			nBodySize = 0;
		}
	}
	else
		TRACE(_T("No bodypart body text or filename specified!\n"));

	return bSuccess;
}