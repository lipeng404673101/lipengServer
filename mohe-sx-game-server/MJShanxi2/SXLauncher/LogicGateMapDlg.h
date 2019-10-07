#pragma once
#include "afxcmn.h"
#include "ServerConfig.h"

////////////////////////////////////////////////////////////////////
//[logic_gate_map]
//managers=name,id,ip,port,type|name,id,ip,port,type
//logics=count,ip,idstart,portstart|
//gates=count,ip,idstart,portstart,type|
//logic_map=logicid,serverid|
//gate_map=gateid,serverid,|
/////////////////////////////////////////////////////////////////////

// CLogicGateMapDlg 对话框

class CLogicGateMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogicGateMapDlg)

public:
	CLogicGateMapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogicGateMapDlg();

	CString m_strManagerList;
	CString m_strLogicList;
	CString m_strGateList;
	CString m_strLogicMap;
	CString m_strGateMap;

	int m_dwItemMaskManagerList;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIC_GATE_MAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlManagerList;
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlSelectedGateList;
	CListCtrl m_ctrlPersonalGatesList;
	CListCtrl m_ctrlAvailableLogicList;
	BOOL m_bMapLogicServer;
	CListCtrl m_ctrlSelectedLogicList;

	void AddManagerServer(LogicManagerConfig &server);
	void AddGateServer(GateServerConfig & gateServer);
	void AddLogicServer(LogicServerConfig & logicServer);
	void RefreshListControl(int nSelectServerId);
	void UIUpdateAvailiableGatesList(int nSelectServerId);
	void UIUpdateSelectedGatesList(int nSelectServerId);
	void UIUpdateAvailiableLogicsList(int nSelectServerId);
	void UIUpdateSelectedLogicsList(int nSelectServerId);
	void SaveLogicGateMapInfo();
	void LoadLogicGateMapInfo();
	void UpdateGateLogicMap();
	afx_msg void OnClickedButtonAddGate();
	afx_msg void OnClickedButtonDelGate();
	afx_msg void OnClickedButtonAddManager();
	afx_msg void OnClickedButtonAddGateServer();
	afx_msg void OnClickedButtonAddLogicServer();
	afx_msg void OnSetfocusListClubManager(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillfocusListClubManager(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonMapLoadConfig();
	afx_msg void OnBnClickedButtonMapSaveConfig();
	afx_msg void OnItemchangedListClubManager(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedCheckUpdateLogic();
};
