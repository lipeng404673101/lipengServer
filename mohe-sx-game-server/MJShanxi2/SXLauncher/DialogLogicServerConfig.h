#pragma once


// CDialogLogicServerConfig 对话框

class CDialogLogicServerConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogLogicServerConfig)

public:
	CDialogLogicServerConfig(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogLogicServerConfig();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIC_SERVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nServerCount;
	int m_nServerIdStart;
	CString m_strServerIP;
	int m_nServerPortStart;
	int m_nLogicServerType;
};
