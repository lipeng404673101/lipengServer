#pragma once


// CDialogManagerConfig 对话框

class CDialogManagerConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogManagerConfig)

public:
	CDialogManagerConfig(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogManagerConfig();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANAGER_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_Id;
	CString m_Ip;
	CString m_Name;
	int m_Port;
	BOOL m_bClub;
};
