#pragma once
#include "afxcmn.h"
#include "LogicGateMapDlg.h"
#include "SXLauncherDlg.h"


// CDailogMainWnd 对话框

class CDailogMainWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CDailogMainWnd)

public:
	CDailogMainWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDailogMainWnd();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
 
	TCHAR m_szCurrentWorkPath[MAX_PATH];
	CTabCtrl m_ctrlTab;
	CLogicGateMapDlg m_dlgLogicGateMap;
	CSXLauncherDlg m_dlgServerConfig;
	CString m_strConfigFile;
	
	afx_msg void OnSelchangeTabServerConfig(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonSxmainLoadConfig();
	afx_msg void OnBnClickedButtonSxmainSaveConfig();
};
