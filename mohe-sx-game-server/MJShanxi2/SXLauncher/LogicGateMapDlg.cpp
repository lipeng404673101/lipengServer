// LogicGateMapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SXLauncher.h"
#include "LogicGateMapDlg.h"
#include "DialogManagerConfig.h"
#include "DialogGateServerConfig.h"
#include "DialogLogicServerConfig.h"
#include "afxdialogex.h"


// CLogicGateMapDlg 对话框

IMPLEMENT_DYNAMIC(CLogicGateMapDlg, CDialogEx)

CLogicGateMapDlg::CLogicGateMapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_LOGIC_GATE_MAP, pParent)
	, m_bMapLogicServer(FALSE)
{

}

CLogicGateMapDlg::~CLogicGateMapDlg()
{
}

void CLogicGateMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLUB_MANAGER, m_ctrlManagerList);
	DDX_Control(pDX, IDC_LIST_SELECT_GATE, m_ctrlSelectedGateList);
	DDX_Control(pDX, IDC_LIST_ALL_GATE, m_ctrlPersonalGatesList);
	DDX_Control(pDX, IDC_LIST_AVAILABLE_LOGIC, m_ctrlAvailableLogicList);
	DDX_Check(pDX, IDC_CHECK_UPDATE_LOGIC, m_bMapLogicServer);
	DDX_Control(pDX, IDC_LIST_SELECT_LOGIC, m_ctrlSelectedLogicList);
}


BEGIN_MESSAGE_MAP(CLogicGateMapDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_GATE, &CLogicGateMapDlg::OnClickedButtonAddGate)
	ON_BN_CLICKED(IDC_BUTTON_DEL_GATE, &CLogicGateMapDlg::OnClickedButtonDelGate)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MANAGER, &CLogicGateMapDlg::OnClickedButtonAddManager)
	ON_BN_CLICKED(IDC_BUTTON_ADD_GATE_SERVER, &CLogicGateMapDlg::OnClickedButtonAddGateServer)
	ON_BN_CLICKED(IDC_BUTTON_ADD_LOGIC_SERVER, &CLogicGateMapDlg::OnClickedButtonAddLogicServer)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_CLUB_MANAGER, &CLogicGateMapDlg::OnSetfocusListClubManager)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_CLUB_MANAGER, &CLogicGateMapDlg::OnKillfocusListClubManager)
	ON_BN_CLICKED(IDC_BUTTON_MAP_LOAD_CONFIG, &CLogicGateMapDlg::OnBnClickedButtonMapLoadConfig)
	ON_BN_CLICKED(IDC_BUTTON_MAP_SAVE_CONFIG, &CLogicGateMapDlg::OnBnClickedButtonMapSaveConfig)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CLUB_MANAGER, &CLogicGateMapDlg::OnItemchangedListClubManager)
	ON_BN_CLICKED(IDC_CHECK_UPDATE_LOGIC, &CLogicGateMapDlg::OnBnClickedCheckUpdateLogic)
END_MESSAGE_MAP()


// CLogicGateMapDlg 消息处理程序


BOOL CLogicGateMapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlManagerList.InsertColumn(0, L"名称", 0, 80);
	m_ctrlManagerList.InsertColumn(1, L"ID", 0, 60);
	m_ctrlManagerList.InsertColumn(2, L"IP", 0, 100);
	m_ctrlManagerList.InsertColumn(3, L"PORT", 0, 60);
	m_ctrlManagerList.InsertColumn(4, L"类型", 0, 60);

	m_ctrlPersonalGatesList.InsertColumn(0, L"ID", 0, 80);
	m_ctrlPersonalGatesList.InsertColumn(1, L"IP", 0, 100);
	m_ctrlPersonalGatesList.InsertColumn(2, L"PORT", 0, 60);
	m_ctrlPersonalGatesList.InsertColumn(3, L"类型", 0, 60);

	m_ctrlSelectedGateList.InsertColumn(0, L"ID", 0, 80);
	m_ctrlSelectedGateList.InsertColumn(1, L"IP", 0, 100);
	m_ctrlSelectedGateList.InsertColumn(2, L"PORT", 0, 60);
	m_ctrlSelectedGateList.InsertColumn(3, L"类型", 0, 60);

	m_ctrlAvailableLogicList.InsertColumn(0, L"ID", 0, 80);
	m_ctrlAvailableLogicList.InsertColumn(1, L"IP", 0, 100);
	m_ctrlAvailableLogicList.InsertColumn(2, L"PORT", 0, 60);
	m_ctrlAvailableLogicList.InsertColumn(3, L"Type", 0, 80);

	m_ctrlSelectedLogicList.InsertColumn(0, L"ID", 0, 80);
	m_ctrlSelectedLogicList.InsertColumn(1, L"IP", 0, 100);
	m_ctrlSelectedLogicList.InsertColumn(2, L"PORT", 0, 60);
	m_ctrlSelectedLogicList.InsertColumn(3, L"Type", 0, 80);


	m_ctrlManagerList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlPersonalGatesList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlSelectedGateList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlAvailableLogicList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlSelectedLogicList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect rect;
	m_ctrlPersonalGatesList.GetWindowRect(rect);
	this->ScreenToClient(rect);
	m_ctrlAvailableLogicList.MoveWindow(rect);
	m_ctrlSelectedGateList.GetWindowRect(rect);
	this->ScreenToClient(rect);
	m_ctrlSelectedLogicList.MoveWindow(rect);

	m_bMapLogicServer = TRUE;
	UpdateData(FALSE);
	this->OnBnClickedCheckUpdateLogic();		
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 选择GATE
void CLogicGateMapDlg::OnClickedButtonAddGate()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bMapLogicServer)
	{
		POSITION pos = m_ctrlAvailableLogicList.GetFirstSelectedItemPosition();
		POSITION posManager = m_ctrlManagerList.GetFirstSelectedItemPosition();
		if (NULL == pos || NULL == posManager)
			return;
		int nManagerIndex = m_ctrlManagerList.GetNextSelectedItem(posManager);
		LogicManagerConfig * pManagerCfg = (LogicManagerConfig*)m_ctrlManagerList.GetItemData(nManagerIndex);
		if (NULL == pManagerCfg) return;

		int nIndex = m_ctrlAvailableLogicList.GetNextSelectedItem(pos);
		LogicServerConfig * pCfg = (LogicServerConfig*)m_ctrlAvailableLogicList.GetItemData(nIndex);
		if (NULL == pCfg) return;

		//Bind
		pCfg->Magager_id = pManagerCfg->ID;

		this->UpdateGateLogicMap();
		this->UIUpdateAvailiableLogicsList(16210);
		this->UIUpdateSelectedLogicsList(pManagerCfg->ID);
		return;
	}
    POSITION pos = m_ctrlPersonalGatesList.GetFirstSelectedItemPosition();
	POSITION posManager = m_ctrlManagerList.GetFirstSelectedItemPosition();
	if (NULL == pos || NULL == posManager)
		return;
	int nManagerIndex = m_ctrlManagerList.GetNextSelectedItem(posManager);
	LogicManagerConfig * pManagerCfg = (LogicManagerConfig*)m_ctrlManagerList.GetItemData(nManagerIndex);
	if (NULL == pManagerCfg) return;

	int nIndex = m_ctrlPersonalGatesList.GetNextSelectedItem(pos);
	GateServerConfig * pCfg = (GateServerConfig*)m_ctrlPersonalGatesList.GetItemData(nIndex);
	if (NULL == pCfg) return;

	//Bind
	pCfg->Magager_id = pManagerCfg->ID;

	this->UpdateGateLogicMap();
	this->UIUpdateAvailiableGatesList(16210);
	this->UIUpdateSelectedGatesList(pManagerCfg->ID);
}


void CLogicGateMapDlg::OnClickedButtonDelGate()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bMapLogicServer)
	{
		POSITION pos = m_ctrlSelectedLogicList.GetFirstSelectedItemPosition();
		POSITION posManager = m_ctrlManagerList.GetFirstSelectedItemPosition();
		if (NULL == pos || NULL == posManager)
			return;
		int nManagerIndex = m_ctrlManagerList.GetNextSelectedItem(posManager);
		LogicManagerConfig * pManagerCfg = (LogicManagerConfig*)m_ctrlManagerList.GetItemData(nManagerIndex);
		if (NULL == pManagerCfg) return;

		int nIndex = m_ctrlSelectedLogicList.GetNextSelectedItem(pos);
		LogicServerConfig * pCfg = (LogicServerConfig*)m_ctrlSelectedLogicList.GetItemData(nIndex);
		if (NULL == pCfg) return;

		//Un Bind
		pCfg->Magager_id = 16210;

		this->UpdateGateLogicMap();
		this->UIUpdateAvailiableLogicsList(16210);
		this->UIUpdateSelectedLogicsList(pManagerCfg->ID);
		return;
	}
	POSITION pos =  m_ctrlSelectedGateList.GetFirstSelectedItemPosition();
	POSITION posManager = m_ctrlManagerList.GetFirstSelectedItemPosition();
	if (NULL == pos || NULL == posManager)
		return;
	int nManagerIndex = m_ctrlManagerList.GetNextSelectedItem(posManager);
	LogicManagerConfig * pManagerCfg = (LogicManagerConfig*)m_ctrlManagerList.GetItemData(nManagerIndex);
	if (NULL == pManagerCfg) return;

	int nIndex = m_ctrlSelectedGateList.GetNextSelectedItem(pos);
	GateServerConfig * pCfg = (GateServerConfig*)m_ctrlSelectedGateList.GetItemData(nIndex);
	if (NULL == pCfg) return;

	//Un Bind
	pCfg->Magager_id = 16210;

	this->UpdateGateLogicMap();
	this->UIUpdateAvailiableGatesList(16210);
	this->UIUpdateSelectedGatesList(pManagerCfg->ID);
}

// 添加Manager
void CLogicGateMapDlg::OnClickedButtonAddManager()
{
	
	// TODO: 在此添加控件通知处理程序代码
	CDialogManagerConfig configDlg;
	if (IDOK == configDlg.DoModal())
	{
		LogicManagerConfig config;
		config.ID = configDlg.m_Id;
		config.IP = configDlg.m_Ip;
		config.Name = configDlg.m_Name;
		config.Port = configDlg.m_Port;
		config.Type = configDlg.m_bClub ? 1 : 0;
		this->AddManagerServer(config);
		
		//保存到配置的数据
		CString strText;
		strText.Format(L"%s,%d,%s,%d,%d,|", config.Name, config.ID, config.IP, config.Port, config.Type);
		m_strManagerList += strText;

		this->RefreshListControl(-1);
	}
}


void CLogicGateMapDlg::OnClickedButtonAddGateServer()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogGateServerConfig configdlg;
	if (IDOK == configdlg.DoModal())
	{
		for (int i = 0; i < configdlg.m_nServerCount; i++)
		{
			GateServerConfig config;
			config.ID = configdlg.m_IdStart + i;
			config.IP = configdlg.m_Ip;
			config.Port = configdlg.m_PortStart + i;
			config.Type = configdlg.m_iGaoFangType;
			config.Magager_id = 16210;
			this->AddGateServer(config);
		}

		//保存到配置文件
		CString strText;
		strText.Format(L"%d,%s,%d,%d,%d,|", configdlg.m_nServerCount, configdlg.m_Ip, configdlg.m_IdStart, configdlg.m_PortStart, configdlg.m_iGaoFangType);
		m_strGateList += strText;		
		this->UIUpdateAvailiableGatesList(16210);
	}

}


void CLogicGateMapDlg::OnClickedButtonAddLogicServer()
{
	// TODO: 在此添加控件通知处理程序代码	
	CDialogLogicServerConfig configdlg;
	if (IDOK == configdlg.DoModal())
	{
		for (int i = 0; i < configdlg.m_nServerCount; i++)
		{
			LogicServerConfig config;
			config.ID = configdlg.m_nServerIdStart + i;
			config.IP = configdlg.m_strServerIP;
			config.Port = configdlg.m_nServerPortStart + i;
			config.nType = configdlg.m_nLogicServerType;
			config.Magager_id = 16210;
			this->AddLogicServer(config);
		}

		//保存到配置文件
		CString strText;
		strText.Format(L"%d,%s,%d,%d,%d,|", configdlg.m_nServerCount, configdlg.m_strServerIP, configdlg.m_nServerIdStart, configdlg.m_nServerPortStart,configdlg.m_nLogicServerType);
		m_strLogicList += strText;
		this->UIUpdateAvailiableLogicsList(16210);
	}
    
}

void CLogicGateMapDlg::RefreshListControl(int nSelectedServerId)
{
	int nItem = 0; CString strText;
	m_ctrlManagerList.DeleteAllItems();
	for (int i = 0; i < ServerConfigHelper::GetInstance().m_managers.size(); i++)
	{
		nItem = m_ctrlManagerList.GetItemCount();
		nItem = m_ctrlManagerList.InsertItem(nItem, ServerConfigHelper::GetInstance().m_managers[i].Name);
		strText.Format(L"%d", ServerConfigHelper::GetInstance().m_managers[i].ID);
		m_ctrlManagerList.SetItemText(nItem, 1, strText);

		m_ctrlManagerList.SetItemText(nItem, 2, ServerConfigHelper::GetInstance().m_managers[i].IP);
		strText.Format(L"%d", ServerConfigHelper::GetInstance().m_managers[i].Port);
		m_ctrlManagerList.SetItemText(nItem, 3, strText);
		strText.Format(L"%s", ServerConfigHelper::GetInstance().m_managers[i].Type == 0 ? L"大厅" : L"棋牌室");
		m_ctrlManagerList.SetItemText(nItem, 4, strText);
		m_ctrlManagerList.SetItemData(nItem, (DWORD_PTR)&ServerConfigHelper::GetInstance().m_managers[i]);
	}	 
}

void CLogicGateMapDlg::SaveLogicGateMapInfo()
{
	ServerConfigHelper::GetInstance().WriteIniString(L"logic_gate_map", L"managers", m_strManagerList);
	ServerConfigHelper::GetInstance().WriteIniString(L"logic_gate_map", L"gates", m_strGateList);
	ServerConfigHelper::GetInstance().WriteIniString(L"logic_gate_map", L"logics", m_strLogicList);
	ServerConfigHelper::GetInstance().WriteIniString(L"logic_gate_map", L"gate_map", m_strGateMap);
	ServerConfigHelper::GetInstance().WriteIniString(L"logic_gate_map", L"logic_map", m_strLogicMap);
}

void CLogicGateMapDlg::LoadLogicGateMapInfo()
{
	CString strText = ServerConfigHelper::GetInstance().GetIniString(L"logic_gate_map", L"managers", L"");

	//Load Managers
	ServerConfigHelper::GetInstance().m_managers.clear();
	std::vector<CString>  manager_strings;
	ServerConfigHelper::GetInstance().SplitStringByChar(strText, L'|', manager_strings);
	for (int i = 0; i < manager_strings.size(); i++)
	{
		std::vector<CString> config_strings;
		ServerConfigHelper::GetInstance().SplitStringByChar(manager_strings[i], L',', config_strings);
		if (config_strings.size() >= 4)
		{
			LogicManagerConfig config;
			config.Name = config_strings[0];
			config.ID = _ttoi(config_strings[1]);
			config.IP = config_strings[2];
			config.Port = _ttoi(config_strings[3]);
			config.Type = _ttoi(config_strings[4]);
			this->AddManagerServer(config);
		}
	}
	m_strManagerList = strText;

	//Load gate maps
    m_strGateMap = strText = ServerConfigHelper::GetInstance().GetIniString(L"logic_gate_map", L"gate_map", L"");
	std::vector<CString>  gate_map_pair_strings;
	std::map<int, int> gate_map;
	ServerConfigHelper::GetInstance().SplitStringByChar(strText, L'|', gate_map_pair_strings);
	for (int i = 0; i < gate_map_pair_strings.size(); i++)
	{
		std::vector<CString> map_pair_string;
		map_pair_string.clear();
		ServerConfigHelper::GetInstance().SplitStringByChar(gate_map_pair_strings[i], L',', map_pair_string);
		if (map_pair_string.size() >= 2)
		{
			gate_map[_ttoi(map_pair_string[0])] = _ttoi(map_pair_string[1]);
		}
	}

	

	//Load gates
	std::vector<CString>  gate_strings;
	ServerConfigHelper::GetInstance().m_gates.clear();
	strText = ServerConfigHelper::GetInstance().GetIniString(L"logic_gate_map", L"gates", L"");
	ServerConfigHelper::GetInstance().SplitStringByChar(strText, L'|', gate_strings);
	for (int i = 0; i < gate_strings.size(); i++)
	{
		std::vector<CString> config_strings;
		ServerConfigHelper::GetInstance().SplitStringByChar(gate_strings[i], L',', config_strings);
		if (config_strings.size() >= 5)
		{
			int nServerCount = _ttoi(config_strings[0]);
			if (nServerCount > 0)
			{
				for (int j = 0; j < nServerCount; j++)
				{
					GateServerConfig config;
					config.IP = config_strings[1];
					config.ID = _ttoi(config_strings[2]) + j;
					config.Port = _ttoi(config_strings[3]) + j;					 
					config.Type = _ttoi(config_strings[4]);
					if (gate_map.count(config.ID) == 1)
					{
						config.Magager_id = gate_map[config.ID];
					}
					else
					{
						config.Magager_id = 16210;
					}
					this->AddGateServer(config);
				}
			}
		}
	}
	m_strGateList = strText;

	//Load logic maps
	m_strLogicMap = strText = ServerConfigHelper::GetInstance().GetIniString(L"logic_gate_map", L"logic_map", L"");
	std::vector<CString>  logic_map_pair_strings;
	std::map<int, int> logic_map;
	ServerConfigHelper::GetInstance().SplitStringByChar(strText, L'|', logic_map_pair_strings);
	for (int i = 0; i < logic_map_pair_strings.size(); i++)
	{
		std::vector<CString> map_pair_string;
		map_pair_string.clear();
		ServerConfigHelper::GetInstance().SplitStringByChar(logic_map_pair_strings[i], L',', map_pair_string);
		if (map_pair_string.size() >= 2)
		{
			logic_map[_ttoi(map_pair_string[0])] = _ttoi(map_pair_string[1]);
		}
	}



	//Load logics
	std::vector<CString>  logic_strings;
	ServerConfigHelper::GetInstance().m_logics.clear();
	strText = ServerConfigHelper::GetInstance().GetIniString(L"logic_gate_map", L"logics", L"");
	ServerConfigHelper::GetInstance().SplitStringByChar(strText, L'|', logic_strings);
	for (int i = 0; i < logic_strings.size(); i++)
	{
		std::vector<CString> config_strings;
		ServerConfigHelper::GetInstance().SplitStringByChar(logic_strings[i], L',', config_strings);
		if (config_strings.size() >= 5)
		{
			int nServerCount = _ttoi(config_strings[0]);
			if (nServerCount > 0)
			{
				for (int j = 0; j < nServerCount; j++)
				{
					LogicServerConfig config;
					config.IP = config_strings[1];
					config.ID = _ttoi(config_strings[2]) + j;
					config.Port = _ttoi(config_strings[3]) + j;		
					config.nType = _ttoi(config_strings[4]);
					if (logic_map.count(config.ID) == 1)
					{
						config.Magager_id = logic_map[config.ID];
					}
					else
					{
						config.Magager_id = 16210;
					}
					this->AddLogicServer(config);
				}
			}
		}
	}
	m_strLogicList = strText;
}

void CLogicGateMapDlg::AddManagerServer(LogicManagerConfig &server)
{
	CString strText;
	ServerConfigHelper::GetInstance().m_managers.push_back(server);	
}
void CLogicGateMapDlg::AddGateServer(GateServerConfig & gateServer)
{
	ServerConfigHelper::GetInstance().m_gates.push_back(gateServer);
}
void CLogicGateMapDlg::AddLogicServer(LogicServerConfig & logicServer)
{
	ServerConfigHelper::GetInstance().m_logics.push_back(logicServer);
}

void CLogicGateMapDlg::UpdateGateLogicMap()
{
	CString strText;
	m_strGateMap.Empty();
	for (int i = 0; i < ServerConfigHelper::GetInstance().m_gates.size(); i++)
	{
		if (ServerConfigHelper::GetInstance().m_gates[i].Magager_id != 16210)
		{
			strText.Format(L"%d,%d,|", ServerConfigHelper::GetInstance().m_gates[i].ID, ServerConfigHelper::GetInstance().m_gates[i].Magager_id);
			m_strGateMap += strText;
		}
	}

	m_strLogicMap.Empty();
	for (int i = 0; i < ServerConfigHelper::GetInstance().m_logics.size(); i++)
	{
		if (ServerConfigHelper::GetInstance().m_logics[i].Magager_id != 16210)
		{
			strText.Format(L"%d,%d,|", ServerConfigHelper::GetInstance().m_logics[i].ID, ServerConfigHelper::GetInstance().m_logics[i].Magager_id);
			m_strLogicMap += strText;
		}
	}

}



void CLogicGateMapDlg::OnSetfocusListClubManager(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//m_dwItemMaskManagerList = m_ctrlManagerList.GetSelectionMark();
	m_ctrlManagerList.SetItemState(m_dwItemMaskManagerList, FALSE, LVIF_STATE);
	*pResult = 0;
}


void CLogicGateMapDlg::OnKillfocusListClubManager(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_dwItemMaskManagerList = m_ctrlManagerList.GetSelectionMark();
	m_ctrlManagerList.SetItemState(m_dwItemMaskManagerList, LVIS_DROPHILITED, LVIS_DROPHILITED);
	*pResult = 0;
}


void CLogicGateMapDlg::OnBnClickedButtonMapLoadConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	LoadLogicGateMapInfo();
	this->RefreshListControl(-1);
	this->UIUpdateAvailiableGatesList(16210);
	this->UIUpdateAvailiableLogicsList(16210);
}


void CLogicGateMapDlg::OnBnClickedButtonMapSaveConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	SaveLogicGateMapInfo();
}

void CLogicGateMapDlg::UIUpdateAvailiableGatesList(int nSelectServerId)
{
	int nItem = 0;
	CString strText;
	m_ctrlPersonalGatesList.DeleteAllItems();
	for (int i = 0; i < ServerConfigHelper::GetInstance().m_gates.size(); i++)
	{
		nItem = m_ctrlPersonalGatesList.GetItemCount();
		if (ServerConfigHelper::GetInstance().m_gates[i].Magager_id == 16210)
		{
			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_gates[i].ID);
			nItem = m_ctrlPersonalGatesList.InsertItem(nItem, strText);

			m_ctrlPersonalGatesList.SetItemText(nItem, 1, ServerConfigHelper::GetInstance().m_gates[i].IP);
			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_gates[i].Port);
			m_ctrlPersonalGatesList.SetItemText(nItem, 2, strText);

			m_ctrlPersonalGatesList.SetItemText(nItem, 3, ServerConfigHelper::GetGateType(ServerConfigHelper::GetInstance().m_gates[i].Type));

			m_ctrlPersonalGatesList.SetItemData(nItem, (DWORD_PTR)&ServerConfigHelper::GetInstance().m_gates[i]);
		}		
	}
}

void CLogicGateMapDlg::UIUpdateSelectedGatesList(int nSelectedServerId)
{
	int nItem = 0;
	CString strText;
	m_ctrlSelectedGateList.DeleteAllItems();
	for (int i = 0; i < ServerConfigHelper::GetInstance().m_gates.size(); i++)
	{
		nItem = m_ctrlSelectedGateList.GetItemCount();
		if (nSelectedServerId == ServerConfigHelper::GetInstance().m_gates[i].Magager_id)
		{
			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_gates[i].ID);
			nItem = m_ctrlSelectedGateList.InsertItem(nItem, strText);

			m_ctrlSelectedGateList.SetItemText(nItem, 1, ServerConfigHelper::GetInstance().m_gates[i].IP);

			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_gates[i].Port);
			m_ctrlSelectedGateList.SetItemText(nItem, 2, strText);

			m_ctrlSelectedGateList.SetItemText(nItem, 3, ServerConfigHelper::GetGateType(ServerConfigHelper::GetInstance().m_gates[i].Type));

			m_ctrlSelectedGateList.SetItemData(nItem, (DWORD_PTR)&ServerConfigHelper::GetInstance().m_gates[i]);
		}
	}
}

void CLogicGateMapDlg::UIUpdateAvailiableLogicsList(int nSelectServerId)
{
	int nItem = 0;
	CString strText;
	m_ctrlAvailableLogicList.DeleteAllItems();
	for (int i = 0; i <  ServerConfigHelper::GetInstance().m_logics.size(); i++)
	{
		nItem = m_ctrlAvailableLogicList.GetItemCount();
		if (ServerConfigHelper::GetInstance().m_logics[i].Magager_id == 16210)
		{
			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_logics[i].ID);
			nItem = m_ctrlAvailableLogicList.InsertItem(nItem, strText);

			m_ctrlAvailableLogicList.SetItemText(nItem, 1, ServerConfigHelper::GetInstance().m_logics[i].IP);

			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_logics[i].Port);
			m_ctrlAvailableLogicList.SetItemText(nItem, 2, strText);

			strText.Format(L"%s", ServerConfigHelper::GetInstance().m_logics[i].nType == 0 ? L"麻将类" : L"棋牌类");
			m_ctrlAvailableLogicList.SetItemText(nItem, 3, strText);

			//m_ctrlAvailableLogicList.SetItemText(nItem, 3, ServerConfigHelper::GetGateType(ServerConfigHelper::GetInstance().m_logics[i].Type));

			m_ctrlAvailableLogicList.SetItemData(nItem, (DWORD_PTR)&ServerConfigHelper::GetInstance().m_logics[i]);
		}
	}
}

void CLogicGateMapDlg::UIUpdateSelectedLogicsList(int nSelectedServerId)
{
	int nItem = 0;
	CString strText;
	m_ctrlSelectedLogicList.DeleteAllItems();
	for (int i = 0; i < ServerConfigHelper::GetInstance().m_logics.size(); i++)
	{
		nItem = m_ctrlSelectedLogicList.GetItemCount();
		if (nSelectedServerId == ServerConfigHelper::GetInstance().m_logics[i].Magager_id)
		{
			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_logics[i].ID);
			nItem = m_ctrlSelectedLogicList.InsertItem(nItem, strText);

			m_ctrlSelectedLogicList.SetItemText(nItem, 1, ServerConfigHelper::GetInstance().m_logics[i].IP);

			strText.Format(L"%d", ServerConfigHelper::GetInstance().m_logics[i].Port);
			m_ctrlSelectedLogicList.SetItemText(nItem, 2, strText);

			strText.Format(L"%s", ServerConfigHelper::GetInstance().m_logics[i].nType == 0 ? L"麻将类" : L"棋牌类");
			m_ctrlSelectedLogicList.SetItemText(nItem, 3, strText);

			//m_ctrlSelectedLogicList.SetItemText(nItem, 3, ServerConfigHelper::GetGateType(ServerConfigHelper::GetInstance().m_logics[i].Type));

			m_ctrlSelectedLogicList.SetItemData(nItem, (DWORD_PTR)&ServerConfigHelper::GetInstance().m_logics[i]);
		}
	}
}


void CLogicGateMapDlg::OnItemchangedListClubManager(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMLV->uNewState & LVIS_SELECTED && (!(pNMLV->uOldState & LVIS_SELECTED)))
	{
		LogicManagerConfig * pConfig = (LogicManagerConfig*)m_ctrlManagerList.GetItemData(pNMLV->iItem);
		this->UIUpdateSelectedGatesList(pConfig->ID);
		this->UIUpdateSelectedLogicsList(pConfig->ID);
		this->UIUpdateAvailiableGatesList(pConfig->ID);
		this->UIUpdateAvailiableLogicsList(pConfig->ID);
	}
	*pResult = 0;
}


void CLogicGateMapDlg::OnBnClickedCheckUpdateLogic()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_bMapLogicServer)
	{
		m_ctrlPersonalGatesList.ShowWindow(SW_HIDE);
		m_ctrlSelectedGateList.ShowWindow(SW_HIDE);
		m_ctrlAvailableLogicList.ShowWindow(SW_NORMAL);
		m_ctrlSelectedLogicList.ShowWindow(SW_NORMAL);
	}
	else
	{
		m_ctrlPersonalGatesList.ShowWindow(SW_NORMAL);
		m_ctrlSelectedGateList.ShowWindow(SW_NORMAL);
		m_ctrlAvailableLogicList.ShowWindow(SW_HIDE);
		m_ctrlSelectedLogicList.ShowWindow(SW_HIDE);
	}
}
