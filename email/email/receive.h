#pragma once
#include "pop3Sock.h"

// CReceiveDlg 对话框

class CReceiveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReceiveDlg)

public:
	void Disp(LONG flag);  //在不同的会话阶段显示不同的信息
	pop3Sock pop3Socket;     //套接字类对象实例
	CReceiveDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CReceiveDlg();

// 对话框数据
	CRichEditCtrl	m_ctrInfo;
	CComboBox	m_ctrList;
	//CString	m_strPass;
	//CString	m_strServer;
	//CString	m_strUser;
	BOOL	m_bolDel;
	CString	m_Info;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RECEIVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();
	afx_msg void OnBnClickedBtnView();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnDisconect();
};
