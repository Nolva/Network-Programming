#include "pch.h"
#include "pop3Sock.h"
#include  "receive.h"
#include "global.h"


#define MAX_BUFF 20000
/////////////////////////////////////////////////////////////////////////////
// pop3Sock

int rubbish_flag;//判断是否为垃圾邮件
int tail = 1;//判断是否为文件末尾

int Div(const CString strLine, char split, CStringArray& strArray)
{
	strArray.RemoveAll();//自带清空属性
	CString temp = strLine;
	int tag = 0;
	while (1)
	{
		tag = temp.Find(split);
		if (tag >= 0)
		{
			strArray.Add(temp.Left(tag));
			temp = temp.Right(temp.GetLength() - tag - 1);
		}
		else { break; }
	}
	strArray.Add(temp);
	return strArray.GetSize();
}


// UTF8编码转为多字节编码
bool UTF8ToMB(vector<char>& pmb, const char* pu8, long long utf8Len)
{
	// convert an UTF8 string to widechar 
	long long nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, NULL, 0);
	WCHAR* lpszW = NULL;
	try {
		lpszW = new WCHAR[nLen];
	}
	catch (bad_alloc & memExp) {
		return false;
	}

	long long nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, lpszW, nLen);
	if (nRtn != nLen) {
		delete[] lpszW;
		return false;
	}

	// convert an widechar string to Multibyte 
	long long MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);
	if (MBLen <= 0) {
		return false;
	}
	pmb.resize(MBLen);
	nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, &*pmb.begin(), MBLen, NULL, NULL);
	delete[] lpszW;

	if (nRtn != MBLen) {
		pmb.clear();
		return false;
	}
	return true;
}
//构造函数，对某些成员变量初始化
pop3Sock::pop3Sock()
{
	m_pDlg = NULL;
	state = POP3FIRST;
	error = "Not connected to server\r\n";
}

pop3Sock::~pop3Sock()
{
	m_pDlg = NULL;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(pop3Sock, CAsyncSocket)
	//{{AFX_MSG_MAP(pop3Sock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// pop3Sock member functions
//接到此消息仅仅显示了一句话，开关了一些按钮连接成功后，服务器发来响应，因此
//又立即收到了OnReceive消息，后来的会话过程中，一来一往，程序总是收到
//OnReceive消息，所以真正的事情在那里作。
void pop3Sock::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0)  m_pDlg->Disp(S_CONNECT);
}

//服务器端关闭连接，才会收到此消息。
void pop3Sock::OnClose(int nErrorCode)
{
	if (nErrorCode == 0)  m_pDlg->Disp(S_CLOSE);
}

//套接字收到服务器发来的数据时，执行此函数
void pop3Sock::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		char buff[MAX_BUFF];            //接收缓冲区
		int rec = Receive(buff, MAX_BUFF); //接收服务器发来的数据
		buff[rec] = NULL;                  //结尾置为NULL。
		lastMsg = buff;
		AnalyzeMsg();                      //分析收到的数据，作不同的处理
	}
	else {
		error = "在接收数据时发送了错误!\r\n";
		m_pDlg->Disp(S_CLOSE);   //显示信息
	}
}
//设置套接字类的对话框指针变量
void pop3Sock::SetParent(CReceiveDlg* pDlg)
{
	m_pDlg = pDlg;
}

//获得第 i 封信件的标题信息
CString pop3Sock::GetMsgStuff(int i)
{
	CString ret;
	CString s;
	int where = msgs[i].text.Find("From:");

	s.Format("From:位置：%d\r\n", where);
	m_pDlg->m_Info += s;

	if (where != -1)
	{
		ret += ReadLn(where, msgs[i].text);
		ret += "\r\n";
	}

	where = msgs[i].text.Find("To:");

	s.Format("To:位置：%d\r\n", where);
	m_pDlg->m_Info += s;

	if (where != -1)
	{
		ret += ReadLn(where, msgs[i].text);
		ret += "\r\n";
	}

	where = msgs[i].text.Find("Date:");

	s.Format("Date:位置：%d\r\n", where);
	m_pDlg->m_Info += s;

	if (where != -1)
	{
		ret += ReadLn(where, msgs[i].text);
		ret += "\r\n";
	}
	ret += GetMsgSubject(i);
	ret += "\r\n";
	return ret;

}

//获得第 i 封信件的信件体
CString pop3Sock::GetMsgBody(int i)
{
	CString ret;
	int where = msgs[i].text.Find("\r\n\r\n");
	if (where != -1) where += 4;
	else          where = 0;
	ret = msgs[i].text.Right(msgs[i].text.GetLength() - where);
	ret = ret.Left(ret.GetLength() - 3);
	return ret;
}

//获得第 i 封信件的标题
CString pop3Sock::GetMsgSubject(int i)
{
	//	CString s;
	//	s.Format("%s",msgs[i].text);
	//	int where=s.Find("Subject:");

	CString ret;
	int where = msgs[i].text.Find("Subject:");
	if (where != -1) 	ret = ReadLn(where, msgs[i].text);
	else ret.Format("找不到Subject:");
	return ret;
}

//获得第 i 封信整封信的内容
CString pop3Sock::GetMsg(int i)
{
	return msgs[i].text;
}

//从src串中，从index所指的位置开始，取子字串，直到行末符"\r"为止。
//dst是一个串的地址，取出的行追加在该串的原内容之后
CString pop3Sock::ReadLn(int index, CString src)
{
	//	CString comp;
	//	CString dst;
	//	comp=src[index];
	//	while(comp!="\r")
	//	{
	//		dst+=comp;
	//		comp=src[++index];
	//	}
	//	return dst;

	CString ret;
	CString s;

	ret = src.Right(src.GetLength() - index);
	int where = ret.Find("\r");
	//	s.Format("行所在的位置：%d",where);
	//	m_pDlg->m_Info+=s;
	//	m_pDlg->UpdateData(FALSE);
	if (where != -1) where += 1;
	else          where = 0;
	//	s.Format("行所在的位置：%d, Length=%d ",where,ret.GetLength());
	//	m_pDlg->m_Info+=s;
	//	m_pDlg->UpdateData(FALSE);

	ret = ret.Left(where);
	return ret;

}


//退出服务器
void pop3Sock::Close()
{
	CString str;
	str.Format("quit%c%c", 13, 10);
	Send((LPCSTR)str, str.GetLength());
	m_pDlg->Disp(S_CLOSE);
	state = POP3FIRST;
	CAsyncSocket::Close();
	error = "执行pop3Sock::Close()命令，Not connected to server\r\n";
}

void pop3Sock::AnalyzeMsg()
{
	int  ix;
	int garbageBool = 0;
	CString temp1;
	CString temp2;

	CString garbage;
	CStringArray garbageArray;
	CFile file;
	CFileException fe;
	if (file.Open("key.txt", CFile::modeRead | CFile::typeBinary, &fe)) {
		char* ptchBuffer = NULL;
		vector<char> pmb;
		int nCount = file.GetLength();
		ptchBuffer = new char[nCount + 1];
		ptchBuffer[nCount] = '\0';
		file.Read(ptchBuffer, file.GetLength());
		UTF8ToMB(pmb, ptchBuffer, nCount + 1);
		for (int i = 1; i < pmb.size(); i++)
			garbage += pmb.at(i);
		//因为有new就要有delete，所以要释放内存；
		if (NULL != ptchBuffer)
		{
			delete[] ptchBuffer;
			ptchBuffer = NULL;
		}
		Div(garbage, '|', garbageArray);
	}

	CString s;
	strstream str;
	string check;
	str << (LPCSTR)lastMsg; //将收到的信息拷贝到字符流变量中  
	str >> check;           //提取所收到信息的第一段代码
	if (check == "-ERR")     //如果有错误
	{
		error = "Received -ERR from server :" + lastMsg;
		Close(); //断开连接，然后关闭
		return;
	}

	//如果没有错误，则根据不同的会话阶段响应来处理
	switch (state)
	{
	case POP3FIRST: //如果已经连接成功，类初始化的时候state为FIRST
		msgs.clear();
		//显示连接成功后服务器返回的消息
		m_pDlg->Disp(S_RECEIVE);
		//发送 user 用户名 命令
		s.Format("user %s%c%c", m_strUser_name, 13, 10);
		Send((LPCSTR)s, s.GetLength()); //发送用户帐号
		state = POP3USER;
		break;

	case POP3USER:
		//显示服务器对于user命令的响应
		m_pDlg->Disp(S_RECEIVE);
		//发送 pass 口令 命令，发送密码
		s.Format("pass %s%c%c", m_strUser_pass, 13, 10);
		Send((LPCSTR)s, s.GetLength());
		state = POP3PASS;
		break;

	case POP3PASS:
		//显示服务器对 pass 命令的响应
		m_pDlg->Disp(S_RECEIVE);
		//发送 stat 命令，请求服务器返回信箱中
		//邮件的数量和所占空间的大小
		s.Format("stat%c%c", 13, 10);
		Send((LPCSTR)s, s.GetLength());
		state = POP3STAT;
		break;

	case POP3STAT:
	{
		string s1;
		str.seekg(0);  //将字符流的当前指针置为0
		str >> s1 >> numMsg >> sizeMsg; //获得数量和大小
		flush(str);
		m_pDlg->Disp(S_GETNUM);
		m_pDlg->Disp(S_GETSIZE);
		if (numMsg > 0) //如果有邮件，则发送RETR获得邮件信息
		{
			retrMsg = 1;        //要取第一封信
			MESSAGEPROP prop; //准备存放信件的地方
			prop.msgSize = 0;   //成员置初值
			prop.retrSize = 0;
			prop.text = "";
			msgs.push_back(prop); //压入向量表
			s.Format("retr 1%c%c", 13, 10);
			Send((LPCSTR)s, s.GetLength());
			state = POP3RETR;
		}
		else //如果没有邮件，则断开
		{
			error = "No new messages\r\n";
			Close();
		}
	}
	break;

	case POP3RETR:
	{
		tail = 1;
		//显示取回来的信件文本
		rubbish_flag = 0;
		s.Format("numMsg= %d  retrMsg =%d", numMsg, retrMsg);
		m_pDlg->m_Info += s;
		CString context;
		int first = lastMsg.Find("data:");
		if (lastMsg.Right(5) == "\r\n.\r\n") {
			//TRACE(lastMsg);
			context = lastMsg.Mid(first + 5, lastMsg.GetLength() - first - 10);
			lastMsg.Delete(first + 5, lastMsg.GetLength() - first - 5);
		}
		else {
			context = lastMsg.Mid(first + 5, lastMsg.GetLength() - first - 5);
			lastMsg.Delete(first + 5, lastMsg.GetLength() - first - 5);
			tail = 0;
		}
		char* cipher = context.GetBuffer();
		char* C = RC4(cipher, key);
		CString cstr = C;
		lastMsg += C;
		if (tail == 1)
			lastMsg += "\r\n.\r\n";
		for (int i = 0; i < garbageArray.GetCount(); i++) {
			garbage = garbageArray.GetAt(i);
			if (cstr.Find(garbage) != -1) {
				s.Format("信件号%d: ,信件大小%d, 为垃圾邮件  信件结束%c%c", retrMsg - 1, lastMsg.GetLength(), 13, 10);
				rubbish_flag = 1;
				break;
			}
		}
		if (rubbish_flag == 0) {
			s.Format("信件号%d: ,信件大小%d, 信件内容： %s   信件结束%c%c",
				retrMsg - 1, lastMsg.GetLength(), lastMsg, 13, 10);
			//将数据追加保存到text成员变量中
			msgs[retrMsg - 1].text += lastMsg;
			//将数据大小累加到retrSize成员变量中，代表已经取回来的数量
			msgs[retrMsg - 1].retrSize += lastMsg.GetLength();
		}
		m_pDlg->m_Info += s;
		m_pDlg->UpdateData(FALSE);
		m_pDlg->Disp(S_RECEIVE);
		//一封信的内容可能多次才能接受回来，但是对于一封信，第一次
		//接收到数据的时候，对应向量结构的msgSize成员变量为0
		//if(msgs[retrMsg-1].msgSize==0) 
		//{
//			string temp;
//			str.seekg(0);
			//将该信件的大小存放到成员变量中
//			str>>temp>>msgs[retrMsg-1].msgSize; 
//		}
		//ix=lastMsg.Find("\r\n.\r\n");
		//if (ix == -1)
		//{
		//	msgs[retrMsg-1].text+=lastMsg;
		//	msgs[retrMsg-1].retrSize+=lastMsg.GetLength();
		//} else  {
		//	temp1=lastMsg.Left(ix+5);
		//	temp2=lastMsg.Right(lastMsg.GetLength()-ix-5);
		//	msgs[retrMsg-1].text+=temp1;
		//	msgs[retrMsg-1].retrSize +=temp1.GetLength();
		//}

		//判断是否获得了这封信的所有数据
		//if(msgs[retrMsg-1].retrSize>=msgs[retrMsg-1].msgSize)
		//{  //检查是否有其他邮件
		if (tail == 1)	//找到了这封信的结尾
		{
			//检查是否有其他邮件
			if (retrMsg < numMsg) //如果还有，则继续发送retr命令
			{
				MESSAGEPROP prop;
				prop.msgSize = 0;
				prop.retrSize = 0;
				prop.text = "";
				msgs.push_back(prop);
				retrMsg++;
				//发retr命令取下一封信
				s.Format("retr %d%c%c", retrMsg, 13, 10);
				Send((LPCSTR)s, s.GetLength());
			}
			else {
				//如果全部信件接收完毕，判断是否要删除

				if (m_pDlg->m_bolDel && numMsg > 0)
				{
					state = POP3DELE;
					delMsg = 1;
					s.Format("dele %d%c%c", delMsg, 13, 10);
					Send((LPCSTR)s, s.GetLength());
				}
				else {                      //否则退出
					state = POP3ENDRETR;
					m_pDlg->Disp(S_ENDRETR);
					error = "信件接收完,不删邮件，会话结束\r\n";
					s.Format("quit%c%c", 13, 10);
					Send((LPCSTR)s, s.GetLength());
					Close();

					for (int i = 0; i < retrMsg; i++)
					{
						s.Format("%d:  %s", i, GetMsgSubject(i));
						//m_pDlg->m_Info+=s;

						m_pDlg->m_ctrList.AddString(s);
						//		m_pDlg->m_ctrList.AddString((LPCSTR)GetMsgSubject(i));
					}
					//m_pDlg->UpdateData(FALSE);
					m_pDlg->m_ctrList.SetCurSel(0);
				}
			}
		}  //DEBUG 
		//}
	}
	break;

	case POP3DELE:
	{
		//删除剩余邮件
		if (delMsg < numMsg)
		{
			delMsg++;
			s.Format("dele %d%c%c", delMsg, 13, 10);
			Send((LPCSTR)s, s.GetLength());
		}
		else //如果已经删除完毕
		{
			m_pDlg->Disp(S_ENDRETR);
			state = POP3GOON;
			error = "Deleted all messages\r\n";
			s.Format("quit%c%c", 13, 10);
			Send((LPCSTR)s, s.GetLength());
			Close();
		}
	}
	break;

	case POP3GOON:

	default:
		m_pDlg->Disp(S_RECEIVE);
		break;
	}
}