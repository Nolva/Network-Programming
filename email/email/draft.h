#pragma once


// CDraftDlg 对话框

class CDraftDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDraftDlg)

public:
	CDraftDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDraftDlg();


// 对话框数据


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DRAFT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();
};
