
// emailDlg.h: 头文件
//

#pragma once
#include "setting.h"
#include "send.h"
#include "receive.h"
#include "draft.h"

// CEmailDlg 对话框
class CEmailDlg : public CDialogEx
{
// 构造
public:
	CEmailDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EMAIL_DIALOG };
#endif

//子对话框
public:
	CSetDlg cSetDlg;
	CSendDlg cSendDlg;
	CReceiveDlg cReceiveDlg;
	CDraftDlg cDraftDlg;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//主页面四个按钮
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnReceive();
	afx_msg void OnBnClickedBtnDraft();
};
