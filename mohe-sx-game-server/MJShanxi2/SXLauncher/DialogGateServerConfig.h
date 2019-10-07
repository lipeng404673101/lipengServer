#pragma once


// CDialogGateServerConfig 对话框

class CDialogGateServerConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogGateServerConfig)

public:
	CDialogGateServerConfig(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogGateServerConfig();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GATE_SERVER_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_IdStart;
	CString m_Ip;
	int m_PortStart;
	int m_iGaoFangType;
	int m_nServerCount;
};
