#include "pch.h"
#include "pop3Sock.h"
#include  "receive.h"
#include "global.h"


#define MAX_BUFF 20000
/////////////////////////////////////////////////////////////////////////////
// pop3Sock

int rubbish_flag;//�ж��Ƿ�Ϊ�����ʼ�
int tail = 1;//�ж��Ƿ�Ϊ�ļ�ĩβ

int Div(const CString strLine, char split, CStringArray& strArray)
{
	strArray.RemoveAll();//�Դ��������
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


// UTF8����תΪ���ֽڱ���
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
//���캯������ĳЩ��Ա������ʼ��
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
//�ӵ�����Ϣ������ʾ��һ�仰��������һЩ��ť���ӳɹ��󣬷�����������Ӧ�����
//�������յ���OnReceive��Ϣ�������ĻỰ�����У�һ��һ�������������յ�
//OnReceive��Ϣ��������������������������
void pop3Sock::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0)  m_pDlg->Disp(S_CONNECT);
}

//�������˹ر����ӣ��Ż��յ�����Ϣ��
void pop3Sock::OnClose(int nErrorCode)
{
	if (nErrorCode == 0)  m_pDlg->Disp(S_CLOSE);
}

//�׽����յ�����������������ʱ��ִ�д˺���
void pop3Sock::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		char buff[MAX_BUFF];            //���ջ�����
		int rec = Receive(buff, MAX_BUFF); //���շ���������������
		buff[rec] = NULL;                  //��β��ΪNULL��
		lastMsg = buff;
		AnalyzeMsg();                      //�����յ������ݣ�����ͬ�Ĵ���
	}
	else {
		error = "�ڽ�������ʱ�����˴���!\r\n";
		m_pDlg->Disp(S_CLOSE);   //��ʾ��Ϣ
	}
}
//�����׽�����ĶԻ���ָ�����
void pop3Sock::SetParent(CReceiveDlg* pDlg)
{
	m_pDlg = pDlg;
}

//��õ� i ���ż��ı�����Ϣ
CString pop3Sock::GetMsgStuff(int i)
{
	CString ret;
	CString s;
	int where = msgs[i].text.Find("From:");

	s.Format("From:λ�ã�%d\r\n", where);
	m_pDlg->m_Info += s;

	if (where != -1)
	{
		ret += ReadLn(where, msgs[i].text);
		ret += "\r\n";
	}

	where = msgs[i].text.Find("To:");

	s.Format("To:λ�ã�%d\r\n", where);
	m_pDlg->m_Info += s;

	if (where != -1)
	{
		ret += ReadLn(where, msgs[i].text);
		ret += "\r\n";
	}

	where = msgs[i].text.Find("Date:");

	s.Format("Date:λ�ã�%d\r\n", where);
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

//��õ� i ���ż����ż���
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

//��õ� i ���ż��ı���
CString pop3Sock::GetMsgSubject(int i)
{
	//	CString s;
	//	s.Format("%s",msgs[i].text);
	//	int where=s.Find("Subject:");

	CString ret;
	int where = msgs[i].text.Find("Subject:");
	if (where != -1) 	ret = ReadLn(where, msgs[i].text);
	else ret.Format("�Ҳ���Subject:");
	return ret;
}

//��õ� i ���������ŵ�����
CString pop3Sock::GetMsg(int i)
{
	return msgs[i].text;
}

//��src���У���index��ָ��λ�ÿ�ʼ��ȡ���ִ���ֱ����ĩ��"\r"Ϊֹ��
//dst��һ�����ĵ�ַ��ȡ������׷���ڸô���ԭ����֮��
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
	//	s.Format("�����ڵ�λ�ã�%d",where);
	//	m_pDlg->m_Info+=s;
	//	m_pDlg->UpdateData(FALSE);
	if (where != -1) where += 1;
	else          where = 0;
	//	s.Format("�����ڵ�λ�ã�%d, Length=%d ",where,ret.GetLength());
	//	m_pDlg->m_Info+=s;
	//	m_pDlg->UpdateData(FALSE);

	ret = ret.Left(where);
	return ret;

}


//�˳�������
void pop3Sock::Close()
{
	CString str;
	str.Format("quit%c%c", 13, 10);
	Send((LPCSTR)str, str.GetLength());
	m_pDlg->Disp(S_CLOSE);
	state = POP3FIRST;
	CAsyncSocket::Close();
	error = "ִ��pop3Sock::Close()���Not connected to server\r\n";
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
		//��Ϊ��new��Ҫ��delete������Ҫ�ͷ��ڴ棻
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
	str << (LPCSTR)lastMsg; //���յ�����Ϣ�������ַ���������  
	str >> check;           //��ȡ���յ���Ϣ�ĵ�һ�δ���
	if (check == "-ERR")     //����д���
	{
		error = "Received -ERR from server :" + lastMsg;
		Close(); //�Ͽ����ӣ�Ȼ��ر�
		return;
	}

	//���û�д�������ݲ�ͬ�ĻỰ�׶���Ӧ������
	switch (state)
	{
	case POP3FIRST: //����Ѿ����ӳɹ������ʼ����ʱ��stateΪFIRST
		msgs.clear();
		//��ʾ���ӳɹ�����������ص���Ϣ
		m_pDlg->Disp(S_RECEIVE);
		//���� user �û��� ����
		s.Format("user %s%c%c", m_strUser_name, 13, 10);
		Send((LPCSTR)s, s.GetLength()); //�����û��ʺ�
		state = POP3USER;
		break;

	case POP3USER:
		//��ʾ����������user�������Ӧ
		m_pDlg->Disp(S_RECEIVE);
		//���� pass ���� �����������
		s.Format("pass %s%c%c", m_strUser_pass, 13, 10);
		Send((LPCSTR)s, s.GetLength());
		state = POP3PASS;
		break;

	case POP3PASS:
		//��ʾ�������� pass �������Ӧ
		m_pDlg->Disp(S_RECEIVE);
		//���� stat ����������������������
		//�ʼ�����������ռ�ռ�Ĵ�С
		s.Format("stat%c%c", 13, 10);
		Send((LPCSTR)s, s.GetLength());
		state = POP3STAT;
		break;

	case POP3STAT:
	{
		string s1;
		str.seekg(0);  //���ַ����ĵ�ǰָ����Ϊ0
		str >> s1 >> numMsg >> sizeMsg; //��������ʹ�С
		flush(str);
		m_pDlg->Disp(S_GETNUM);
		m_pDlg->Disp(S_GETSIZE);
		if (numMsg > 0) //������ʼ�������RETR����ʼ���Ϣ
		{
			retrMsg = 1;        //Ҫȡ��һ����
			MESSAGEPROP prop; //׼������ż��ĵط�
			prop.msgSize = 0;   //��Ա�ó�ֵ
			prop.retrSize = 0;
			prop.text = "";
			msgs.push_back(prop); //ѹ��������
			s.Format("retr 1%c%c", 13, 10);
			Send((LPCSTR)s, s.GetLength());
			state = POP3RETR;
		}
		else //���û���ʼ�����Ͽ�
		{
			error = "No new messages\r\n";
			Close();
		}
	}
	break;

	case POP3RETR:
	{
		tail = 1;
		//��ʾȡ�������ż��ı�
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
				s.Format("�ż���%d: ,�ż���С%d, Ϊ�����ʼ�  �ż�����%c%c", retrMsg - 1, lastMsg.GetLength(), 13, 10);
				rubbish_flag = 1;
				break;
			}
		}
		if (rubbish_flag == 0) {
			s.Format("�ż���%d: ,�ż���С%d, �ż����ݣ� %s   �ż�����%c%c",
				retrMsg - 1, lastMsg.GetLength(), lastMsg, 13, 10);
			//������׷�ӱ��浽text��Ա������
			msgs[retrMsg - 1].text += lastMsg;
			//�����ݴ�С�ۼӵ�retrSize��Ա�����У������Ѿ�ȡ����������
			msgs[retrMsg - 1].retrSize += lastMsg.GetLength();
		}
		m_pDlg->m_Info += s;
		m_pDlg->UpdateData(FALSE);
		m_pDlg->Disp(S_RECEIVE);
		//һ���ŵ����ݿ��ܶ�β��ܽ��ܻ��������Ƕ���һ���ţ���һ��
		//���յ����ݵ�ʱ�򣬶�Ӧ�����ṹ��msgSize��Ա����Ϊ0
		//if(msgs[retrMsg-1].msgSize==0) 
		//{
//			string temp;
//			str.seekg(0);
			//�����ż��Ĵ�С��ŵ���Ա������
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

		//�ж��Ƿ���������ŵ���������
		//if(msgs[retrMsg-1].retrSize>=msgs[retrMsg-1].msgSize)
		//{  //����Ƿ��������ʼ�
		if (tail == 1)	//�ҵ�������ŵĽ�β
		{
			//����Ƿ��������ʼ�
			if (retrMsg < numMsg) //������У����������retr����
			{
				MESSAGEPROP prop;
				prop.msgSize = 0;
				prop.retrSize = 0;
				prop.text = "";
				msgs.push_back(prop);
				retrMsg++;
				//��retr����ȡ��һ����
				s.Format("retr %d%c%c", retrMsg, 13, 10);
				Send((LPCSTR)s, s.GetLength());
			}
			else {
				//���ȫ���ż�������ϣ��ж��Ƿ�Ҫɾ��

				if (m_pDlg->m_bolDel && numMsg > 0)
				{
					state = POP3DELE;
					delMsg = 1;
					s.Format("dele %d%c%c", delMsg, 13, 10);
					Send((LPCSTR)s, s.GetLength());
				}
				else {                      //�����˳�
					state = POP3ENDRETR;
					m_pDlg->Disp(S_ENDRETR);
					error = "�ż�������,��ɾ�ʼ����Ự����\r\n";
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
		//ɾ��ʣ���ʼ�
		if (delMsg < numMsg)
		{
			delMsg++;
			s.Format("dele %d%c%c", delMsg, 13, 10);
			Send((LPCSTR)s, s.GetLength());
		}
		else //����Ѿ�ɾ�����
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