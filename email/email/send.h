#pragma once
#include "smtpSock.h"
#include  "Base64.h"



class CSendDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSendDlg)

public:
	void Disp(LONG flag);
	void GetHeader();

	smtpSock smtpSocket;
	CSendDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSendDlg();

// 对话框数据
	//CString	m_Addr;
	CString	m_Attach;
	CString	m_BCC;
	CString	m_CC;
	CString	m_Letter;
	CString	m_Name;
	//CString	m_Pass;
	//UINT	m_Port;
	CString	m_Receiver;
	//CString	m_Server;
	CString	m_Title;
	//CString	m_User;
	CString	m_Info;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SEND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();
	//afx_msg void OnBnClickedBtnBrowse();
private:
	CBase64 coder;
	BOOL GetBody(LPSTR& pszBody, int& nBodySize);
public:
	afx_msg void OnBnClickedBtnBrowse();
	afx_msg void OnBnClickedBtnSend();
};
