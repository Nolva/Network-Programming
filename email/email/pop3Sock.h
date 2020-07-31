#pragma once
#include <vector>
#include <strstream>
#include <string>

using namespace std;

class  CReceiveDlg;

/////////////////////////////////////////////////////////////////////////////
// mySock command target

//��ʾ��ͬ��Ϣ�ı�־
#define S_CLOSE   1  //���ر�����ʱ����ʾ�ر���Ϣ
#define S_CONNECT 2  //�����ӳɹ�ʱ����ʾ������Ϣ
#define S_RECEIVE 3  //���յ���������������Ϣʱ����ʾ����Ϣ
#define S_GETNUM  4 //�������ż�������Ϻ���ʾ�����е��ż�����
#define S_GETSIZE 5 //�������ż�������Ϻ���ʾ�����е��ż��ܴ�С
#define S_ENDRETR 6 //�������ż�������Ϻ���ʾ�����Ϣ

//����pop3�Ự״̬��ö������
typedef enum { POP3FIRST = 0, POP3USER, POP3PASS, POP3STAT, POP3LIST, POP3RETR, POP3ENDRETR, POP3DELE, POP3GOON } POP3STATE;

//��������һ���ż���Ϣ�Ľṹ
typedef struct
{
	CString text;  //�洢�ż����ı�
	int msgSize;   //�ż��Ĵ�С
	int	retrSize;  //�ż�ʵ�����صĴ�С�������ع����ж�̬�仯
} MESSAGEPROP;

//�Լ����׽�����
class pop3Sock : public CAsyncSocket
{
	// Attributes
public:
	CString lastMsg;
	CString error;
	int numMsg, sizeMsg, retrMsg;
	vector<MESSAGEPROP> msgs; //ʢ���ż���Ϣ������

// Operations
public:
	//��û������ʼ���ͷ�ֶ�: From, To, Date, Subject, Body
	CString GetMsgStuff(int i);
	CString GetMsgBody(int i);     //����ż���
	CString GetMsgSubject(int i);  //����ż�����
	CString GetMsg(int i);         //��������ŵ�����
	CString ReadLn(int index, CString src);

	void Close();            //�˳�������
	void SetParent(CReceiveDlg* pDlg);
	pop3Sock();
	virtual ~pop3Sock();

	// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(mySock)
public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(mySock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
private:
	void AnalyzeMsg();
	CReceiveDlg* m_pDlg;
	POP3STATE state;
	int delMsg;
};