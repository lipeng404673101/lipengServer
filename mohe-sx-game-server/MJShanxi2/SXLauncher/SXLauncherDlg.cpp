
// SXLauncherDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SXLauncher.h"
#include "SXLauncherDlg.h"
#include "LogicGateMapDlg.h"
#include "afxdialogex.h"
#include "MyHttpRequest.h"
#define _MH_ALERT_URL "http://api.ry.haoyunlaiyule.com/server/alert?type=11"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static COLORREF color_info = RGB(0, 0, 0);
static COLORREF color_time = RGB(10, 10, 10);
static COLORREF color_error = RGB(255, 0, 0);
static COLORREF color_debug = RGB(0, 0, 128);
// CSXLauncherDlg 对话框
TCHAR szGlobalMessage[256] = { 0 };


CSXLauncherDlg::CSXLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SXLAUNCHER_DIALOG, pParent)
	, m_strRedistIP(_T(""))
	, m_iRedistPort(0)
	, m_strDatabaseIP(_T(""))
	, m_strDatabaseCenterDBName(_T(""))
	, m_strDatabaseLogicDBName(_T(""))
	, m_strDatabaseUserName(_T(""))
	, m_strDatabasePassword(_T(""))
	, m_iDatabasePort(0)
	, m_strLogicDBIP(_T(""))
	, m_iLogicDBPort(0)
	, m_strCenterOutsideIP(_T(""))
	, m_iCenterOutsidePort(0)
	, m_strCenterGMIP(_T(""))
	, m_iCenterGMPort(0)
	, m_strLoginOutsideIP(_T(""))
	, m_iLoginOutsidePort(0)
	, m_strLoginInsideIP(_T(""))
	, m_iLoginInsidePort(0)
	, m_strManagerName(_T(""))
	, m_strManagerInsideIP(_T(""))
	, m_iManagerInsidePort(0)
	, m_uManagerID(0)
	, m_nLoginServerCount(0)
	, m_nGateServerCount(0)
	, m_uGateServerIDStart(0)
	, m_strGateOutsideIP(_T(""))
	, m_iGatePortStart(0)
	, m_iLogicIDStart(0)
	, m_strLogicInsideIP(_T(""))
	, m_iLogicInsidePortStart(0)
	, m_bLogicDebugMode(FALSE)
	, m_bLogicRobotEnable(FALSE)
	, m_bManagerDebugMode(FALSE)
	, m_bManagerRobot(FALSE)
	, m_nLogicServerCount(0)
	, m_iManagerDeskCountLimit(100)
	, m_iLogLevel(1)
	, m_iLogicLogLevel(0)	
	, m_iGaoFangConfig(0)
	, m_bAlertEnable(TRUE)
	, m_bEnableProtect(TRUE)
	, m_nClubManagerCount(0)
	, m_nLogicQipaiCount(0)
	, m_nLogicQipaiStartId(0)
	, m_strLogicQipaiServerIp(_T(""))
	, m_nLogicQipaiStartPort(0)
	, m_nLogicQipaiLogLevel(0)
	, m_bLogicQipaiDebugMode(FALSE)
	, m_bLogicQipaiRobot(FALSE)
	, m_strClubDBName(_T(""))
	, m_nLogicQipaiSupportGameType(0)
	, m_strDatabaseIPBk(_T(""))
	, m_strDatabasePasswordBk(_T(""))
	, m_uDatabasePortBk(0)
	, m_strDatabaseUserNameBk(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iGateInsidePortStart = m_iGatePortStart + 500;

}

void CSXLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REDIST_IP, m_strRedistIP);
	DDX_Text(pDX, IDC_EDIT_REDIST_PORT, m_iRedistPort);
	DDX_Text(pDX, IDC_EDIT_DATABASE_IP, m_strDatabaseIP);
	DDX_Text(pDX, IDC_EDIT_DATABASE_CENTER_DB, m_strDatabaseCenterDBName);
	DDX_Text(pDX, IDC_EDIT_DATABASE_LOGIC_DB, m_strDatabaseLogicDBName);
	DDX_Text(pDX, IDC_EDIT_DATABASE_USER, m_strDatabaseUserName);
	DDX_Text(pDX, IDC_EDIT_DATABASE_PASSWORD, m_strDatabasePassword);
	DDX_Text(pDX, IDC_EDIT_DATABASE_PORT, m_iDatabasePort);
	DDX_Text(pDX, IDC_EDIT_LOGICDB_IP, m_strLogicDBIP);
	DDX_Text(pDX, IDC_EDIT_LOGICDB_PORT, m_iLogicDBPort);
	//  DDX_Text(pDX, IDC_EDIT_CENTER_GM_IP, m_strCenterOutsideIP);
	//  DDX_Text(pDX, IDC_EDIT_CENTER_IP, m_iCenterOutsidePort);
	DDX_Text(pDX, IDC_EDIT_CENTER_IP, m_strCenterOutsideIP);
	DDX_Text(pDX, IDC_EDIT_CENTER_PORT, m_iCenterOutsidePort);
	DDX_Text(pDX, IDC_EDIT_CENTER_GM_IP, m_strCenterGMIP);
	DDX_Text(pDX, IDC_EDIT_CENTER_GM_PORT, m_iCenterGMPort);
	DDX_Text(pDX, IDC_EDIT_LOGIN_OUTSIDE_IP, m_strLoginOutsideIP);
	DDX_Text(pDX, IDC_EDIT_LOGIN_OUTSIDE_PORT, m_iLoginOutsidePort);
	DDX_Text(pDX, IDC_EDIT_LOGIN_INSIDE_IP, m_strLoginInsideIP);
	DDX_Text(pDX, IDC_EDIT_LOGIN_INSIDE_PORT, m_iLoginInsidePort);
	DDX_Text(pDX, IDC_EDIT_MANAGER_NAME, m_strManagerName);
	//  DDX_Text(pDX, IDC_EDIT_MANAGER_INSIDE_PORT, m_strManagerInsideIP);
	DDX_Text(pDX, IDC_EDIT_MANAGER_INSIDE_IP, m_strManagerInsideIP);
	DDX_Text(pDX, IDC_EDIT_MANAGER_INSIDE_PORT, m_iManagerInsidePort);
	DDX_Text(pDX, IDC_EDIT_MANAGER_ID, m_uManagerID);
	DDX_Text(pDX, IDC_EDIT_LOGIN_COUNT, m_nLoginServerCount);
	DDX_Text(pDX, IDC_EDIT_GATE_COUNT, m_nGateServerCount);
	DDX_Text(pDX, IDC_EDIT_GATE_ID, m_uGateServerIDStart);
	DDX_Text(pDX, IDC_EDIT_GATE_OUTSIDE_IP, m_strGateOutsideIP);
	DDX_Text(pDX, IDC_EDIT_GATE_OUTSIDE_PORT, m_iGatePortStart);
	DDX_Text(pDX, IDC_EDIT_LOGIC_ID, m_iLogicIDStart);
	DDX_Text(pDX, IDC_EDIT_LOGIC_INSIDE_IP, m_strLogicInsideIP);
	DDX_Text(pDX, IDC_EDIT_LOGIC_INSIDE_PORT, m_iLogicInsidePortStart);
	DDX_Check(pDX, IDC_CHECK_LOGIC_DEBUG_MODE, m_bLogicDebugMode);
	DDX_Check(pDX, IDC_CHECK_LOGIC_ROBOT, m_bLogicRobotEnable);
	DDX_Check(pDX, IDC_CHECK_MANAGER_DEBUG_MODE, m_bManagerDebugMode);
	DDX_Check(pDX, IDC_CHECK_MANAGER_ROBOT, m_bManagerRobot);
	DDX_Text(pDX, IDC_EDIT_LOGIC_COUNT, m_nLogicServerCount);
	DDX_Text(pDX, IDC_EDIT_MANAGER_DESK_COUNT_LIMIT, m_iManagerDeskCountLimit);
	DDX_Text(pDX, IDC_EDIT_LOGLEVEL, m_iLogLevel);
	//  DDX_Control(pDX, IDC_RICHEDIT_LOG, m_ctrlLogs);
	DDX_Control(pDX, IDC_RICHEDIT_LOG, m_ctrlLogs);
	DDX_Text(pDX, IDC_EDIT_LOGIC_LOGLEVEL, m_iLogicLogLevel);
	DDV_MinMaxInt(pDX, m_iLogicLogLevel, 1, 3);
	DDX_Radio(pDX, IDC_RADIO_GF_NO, m_iGaoFangConfig);
	DDV_MinMaxInt(pDX, m_iGaoFangConfig, 0, 2);
	DDX_Check(pDX, IDC_CHECK_ALERT, m_bAlertEnable);
	DDX_Check(pDX, IDC_CHECK_MONITOR_ENABLE, m_bEnableProtect);
	DDX_Text(pDX, IDC_EDIT_CENTER_CLUB_COUNT, m_nClubManagerCount);
	DDX_Text(pDX, IDC_EDIT_LOGIC_QIPAI_COUNT, m_nLogicQipaiCount);
	DDX_Text(pDX, IDC_EDIT_LOGIC_QIPAI_ID, m_nLogicQipaiStartId);
	DDX_Text(pDX, IDC_EDIT_LOGIC_QIPAI_INSIDE_IP, m_strLogicQipaiServerIp);
	DDX_Text(pDX, IDC_EDIT_LOGIC_QIPAI_INSIDE_PORT, m_nLogicQipaiStartPort);
	DDX_Text(pDX, IDC_EDIT_LOGIC_QIPAI_LOGLEVEL, m_nLogicQipaiLogLevel);
	DDX_Check(pDX, IDC_CHECK_LOGIC_QIPAI_DEBUG_MODE, m_bLogicQipaiDebugMode);
	DDX_Check(pDX, IDC_CHECK_LOGIC_QIPAI_ROBOT, m_bLogicQipaiRobot);
	DDX_Text(pDX, IDC_EDIT_DATABASE_CLUB_DB, m_strClubDBName);
	DDX_Text(pDX, IDC_EDIT_LOGIC_QIPAI_SUPPORT_GAME_TYPE, m_nLogicQipaiSupportGameType);
	DDX_Text(pDX, IDC_EDIT_DATABASE_IP_BK, m_strDatabaseIPBk);
	DDX_Text(pDX, IDC_EDIT_DATABASE_PASSWORD_BK, m_strDatabasePasswordBk);
	DDX_Text(pDX, IDC_EDIT_DATABASE_PORT_BK, m_uDatabasePortBk);
	DDX_Text(pDX, IDC_EDIT_DATABASE_USER_BK, m_strDatabaseUserNameBk);
}

BEGIN_MESSAGE_MAP(CSXLauncherDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_REDIST_START, &CSXLauncherDlg::OnClickedButtonRedistStart)
	ON_BN_CLICKED(IDC_BUTTON_LOGICDB_START, &CSXLauncherDlg::OnClickedButtonLogicdbStart)
	ON_BN_CLICKED(IDC_BUTTON_CENTER_START, &CSXLauncherDlg::OnClickedButtonCenterStart)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN_START, &CSXLauncherDlg::OnClickedButtonLoginStart)
	ON_BN_CLICKED(IDC_BUTTON_GATE_START, &CSXLauncherDlg::OnClickedButtonGateStart)
	ON_BN_CLICKED(IDC_BUTTON_LOGIC_START, &CSXLauncherDlg::OnClickedButtonLogicStart)
	ON_BN_CLICKED(IDC_BUTTON_MANAGER_START, &CSXLauncherDlg::OnClickedButtonManagerStart)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_ONEKEY_CLOSE, &CSXLauncherDlg::OnClickedButtonOnekeyClose)
	ON_MESSAGE(WM_MESSAGE_RELAUNCH_PROCESS, &CSXLauncherDlg::OnCustomMessageRelaunchProcess)
	ON_MESSAGE(WM_MESSAGE_TRACE_INFO, &CSXLauncherDlg::OnCustomMessageInsertLog)	
	ON_BN_CLICKED(IDC_CHECK_ALERT, &CSXLauncherDlg::OnClickedCheckAlert)
	ON_BN_CLICKED(IDC_CHECK_MONITOR_ENABLE, &CSXLauncherDlg::OnClickedCheckMonitorEnable)
	ON_BN_CLICKED(IDC_BUTTON_LOGIC_QIPAI_START, &CSXLauncherDlg::OnBnClickedButtonLogicQipaiStart)
	ON_BN_CLICKED(IDC_BUTTON_ONEKEY_START_TEST, &CSXLauncherDlg::OnBnClickedButtonOnekeyStartTest)
END_MESSAGE_MAP()


// CSXLauncherDlg 消息处理程序

BOOL CSXLauncherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	setlocale(LC_CTYPE, ("chs"));
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CString strServerIP(L"192.168.1.112");
	CString strDatabaseServerIP(L"58.221.58.210");
	// TODO: 在此添加额外的初始化代码
	m_strRedistIP = strServerIP;
	m_iRedistPort = 6379;

	//database
	m_strDatabaseIP = strDatabaseServerIP;
	m_iDatabasePort = 3306;
	m_strDatabaseUserName = L"root";
	m_strDatabasePassword = L"mh123456";

	m_strDatabaseIPBk = strDatabaseServerIP;
	m_uDatabasePortBk = 3306;
	m_strDatabaseUserNameBk = L"root";
	m_strDatabasePasswordBk = "mh123456";

	m_strDatabaseCenterDBName = "shanxi_center";
	m_strDatabaseLogicDBName = "shanxi_logic";
	m_strClubDBName = "shanxi_club";
	
	//center
	m_strCenterOutsideIP = strServerIP;
	m_iCenterOutsidePort = 4001;
	m_strCenterGMIP = L"127.0.0.1";
	m_iCenterGMPort = 4501;

	// logic db
	m_strLogicDBIP = strServerIP;
	m_iLogicDBPort = 5001;

	// login server
	m_strLoginInsideIP = "0.0.0.0";
	m_iLoginInsidePort = 6501;
	m_strLoginOutsideIP = strServerIP;
	m_iLoginOutsidePort = 6001;
	m_nLoginServerCount = 2;	

	// gate 
	m_strGateOutsideIP = strServerIP;
	m_iGatePortStart = 8001;
	m_nGateServerCount = 2;
	m_uGateServerIDStart = 1;
	// Gate inside start 



	// manager
	m_strManagerName = L"山西麻将";
	m_strManagerInsideIP = strServerIP;
	m_iManagerInsidePort = 7001;
	m_uManagerID = 16210;
	m_bManagerDebugMode = FALSE;
	m_bManagerRobot = FALSE;
 
	// logic
	m_strLogicInsideIP = strServerIP;
	m_iLogicInsidePortStart = 9001;
	m_iLogicIDStart = 16211;
	m_nLogicServerCount = 2;
	m_bLogicDebugMode = FALSE;
	m_bLogicRobotEnable = FALSE;
	m_iLogicLogLevel = 1;

	// logic qipai
	m_nLogicQipaiStartPort = 0;
	m_nLogicQipaiCount = 1;
	m_nLogicQipaiLogLevel = 1;
	m_strLogicQipaiServerIp = strServerIP;
	m_nLogicQipaiStartId = 16311;
	m_bLogicQipaiDebugMode = FALSE;
	m_bLogicQipaiRobot = FALSE;
	m_nLogicQipaiSupportGameType = 1;

	 
	::GetCurrentDirectory(MAX_PATH, m_szCurrentWorkPath);
	//m_strLauncherConfigFile.Format(L"%s\\launcher_config.ini", m_szCurrentWorkPath);
	lstrcpy(m_szRedisWorkPath, m_szCurrentWorkPath);
	lstrcat(m_szRedisWorkPath, L"\\redis");

	lstrcpy(m_szCenterPath, m_szCurrentWorkPath);
	lstrcat(m_szCenterPath, L"\\center");
	::CreateDirectory(m_szCenterPath, NULL);

	lstrcpy(m_szLogicDBServerPath, m_szCurrentWorkPath);
	lstrcat(m_szLogicDBServerPath, L"\\logicdb");
	::CreateDirectory(m_szLogicDBServerPath, NULL);

	lstrcpy(m_szLoginPath, m_szCurrentWorkPath);
	lstrcat(m_szLoginPath, L"\\login");
	::CreateDirectory(m_szLoginPath, NULL);

	lstrcpy(m_szLogicManagerServerPath, m_szCurrentWorkPath);
	lstrcat(m_szLogicManagerServerPath, L"\\manager");
	::CreateDirectory(m_szLogicManagerServerPath, NULL);

	lstrcpy(m_szGateServerPath, m_szCurrentWorkPath);
	lstrcat(m_szGateServerPath, L"\\gate");
	::CreateDirectory(m_szGateServerPath, NULL);

	lstrcpy(m_szLogicServerPath, m_szCurrentWorkPath);
	lstrcat(m_szLogicServerPath, L"\\logic");
	::CreateDirectory(m_szLogicServerPath, NULL); 

	lstrcpy(m_szLogicQipaiPath, m_szCurrentWorkPath);
	lstrcat(m_szLogicQipaiPath, L"\\logicqipai");
	::CreateDirectory(m_szLogicQipaiPath, NULL);

	this->LoadLauncherConfigFile();
	UpdateData(FALSE);

	// 创建守护线程
	moniter_thread = 1;
	m_pMoniterThread = ::AfxBeginThread((AFX_THREADPROC)(&CSXLauncherDlg::MoniterThread), this);
	m_myEvent.SetEvent();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSXLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSXLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CSXLauncherDlg::WriteCenterConfigFile(CString& strPath, CString& strDstPath, CString& strDstFile)
{
	CStdioFile file;
	 
	strDstPath.Format(L"%s", strPath);
	strDstFile.Format(L"%s\\center_config.ini", strDstPath);
	file.Open(strDstFile, CFile::modeCreate | CFile::modeWrite);
	CString strText;

	file.WriteString(GetIniLineText(CString(L"LogLevel"), m_iLogLevel));
	file.WriteString(GetIniLineText(CString(L"DbHost"), m_strDatabaseIP));
	file.WriteString(GetIniLineText(CString(L"DbUser"), m_strDatabaseUserName));
	file.WriteString(GetIniLineText(CString(L"DbPass"), m_strDatabasePassword));
	file.WriteString(GetIniLineText(CString(L"DbPort"), m_iDatabasePort));

	file.WriteString(GetIniLineText(CString(L"BKDbHost"), m_strDatabaseIPBk));
	file.WriteString(GetIniLineText(CString(L"BKDbUser"), m_strDatabaseUserNameBk));
	file.WriteString(GetIniLineText(CString(L"BKDbPass"), m_strDatabasePasswordBk));
	file.WriteString(GetIniLineText(CString(L"BKDbPort"), m_uDatabasePortBk));

	file.WriteString(GetIniLineText(CString(L"DbName"), m_strDatabaseCenterDBName));
	file.WriteString(GetIniLineText(CString(L"ClubDbName"), m_strClubDBName)); 

	file.WriteString(GetIniLineText(CString(L"ClubServerCount"), m_nClubManagerCount));
 

	file.WriteString(GetIniLineText(CString(L"InsideIp"), m_strCenterOutsideIP));
	file.WriteString(GetIniLineText(CString(L"InsidePort"), m_iCenterOutsidePort));
	file.WriteString(GetIniLineText(CString(L"GMIp"), m_strCenterGMIP));
	file.WriteString(GetIniLineText(CString(L"GMPort"), m_iCenterGMPort));
	file.WriteString(GetIniLineText(CString(L"LocalLogin"),  CString(L"true")));
	file.WriteString(GetIniLineText(CString(L"NeedWechat"),1));
	file.WriteString(GetIniLineText(CString(L"RedisIp"), m_strRedistIP));
	file.WriteString(GetIniLineText(CString(L"RedisPort"), m_iRedistPort)); 
	file.Close();

	return TRUE;

}

BOOL CSXLauncherDlg::WriteLoginConfigFile(CString & strPath, short portVar, CString &strDestFile, CString &strDestPath)
{
	CString strTemp;
	CStdioFile file;
	strDestPath.Format(L"%s\\%04d", strPath, m_iLoginOutsidePort + portVar);
	::CreateDirectory(strDestPath, NULL);
	strTemp.Format(L"%s\\loginserver_config.ini", strDestPath);
	file.Open(strTemp, CFile::modeCreate | CFile::modeWrite);

	file.WriteString(GetIniLineText(CString(L"LogLevel"), m_iLogLevel));
	file.WriteString(GetIniLineText(CString(L"OutsideIp"), m_strLoginOutsideIP));
	file.WriteString(GetIniLineText(CString(L"OutsidePort"), m_iLoginOutsidePort + portVar));
	file.WriteString(GetIniLineText(CString(L"InsideIp"), m_strLoginInsideIP));
	file.WriteString(GetIniLineText(CString(L"InsidePort"), m_iLoginInsidePort + portVar));
	file.WriteString(GetIniLineText(CString(L"CenterIp"), m_strCenterOutsideIP));
	file.WriteString(GetIniLineText(CString(L"CenterPort"), m_iCenterOutsidePort));	
	file.WriteString(GetIniLineText(CString(L"GaoFangFlag"), m_iGaoFangConfig));

	file.Close();
	strDestFile = strTemp;
	return TRUE;
}

BOOL CSXLauncherDlg::WriteLogicDBServerConfigFile(CString & strPath, CString &strDstPath, CString &strDstFile)
{
	CString strTemp;
	CStdioFile file;
	strDstPath.Format(L"%s", strPath, m_szLogicDBServerPath);
	::CreateDirectory(strDstPath, NULL);

	strDstFile.Format(L"%s\\logic_db_config.ini", strDstPath);
	file.Open(strDstFile, CFile::modeCreate | CFile::modeWrite);

	file.WriteString(GetIniLineText(CString(L"LogLevel"), m_iLogLevel));
	file.WriteString(GetIniLineText(CString(L"DbHost"), m_strDatabaseIP));
	file.WriteString(GetIniLineText(CString(L"DbPort"), m_iDatabasePort));
	file.WriteString(GetIniLineText(CString(L"DbUser"), m_strDatabaseUserName));
	file.WriteString(GetIniLineText(CString(L"DbPass"), m_strDatabasePassword));
	file.WriteString(GetIniLineText(CString(L"DbName"), m_strDatabaseLogicDBName));
	 

	file.WriteString(GetIniLineText(CString(L"BKDbHost"), m_strDatabaseIPBk));
	file.WriteString(GetIniLineText(CString(L"BKDbPort"), m_uDatabasePortBk));
	file.WriteString(GetIniLineText(CString(L"BKDbUser"), m_strDatabaseUserNameBk));
	file.WriteString(GetIniLineText(CString(L"BKDbPass"), m_strDatabasePasswordBk));
	 

	file.WriteString(GetIniLineText(CString(L"LogicDBIp"), m_strLogicDBIP));
	file.WriteString(GetIniLineText(CString(L"LogicDBPort"), m_iLogicDBPort));
	file.WriteString(GetIniLineText(CString(L"RedisIp"), m_strRedistIP));
	file.WriteString(GetIniLineText(CString(L"RedisPort"), m_iRedistPort));

	file.Close();
	return TRUE;
}
BOOL CSXLauncherDlg::WriteLogicManagerServerConfigFile(CString & strPath, CString &strDstPath, CString &strDstFile)
{
	CString strTemp;
	CStdioFile file;
	strDstPath.Format(L"%s", strPath, m_szLogicDBServerPath);
	::CreateDirectory(strDstPath, NULL);

	strDstFile.Format(L"%s\\logic_manager_config.ini", strDstPath);
	file.Open(strDstFile, CFile::modeCreate | CFile::modeWrite);

	file.WriteString(GetIniLineText(CString(L"LogLevel"), m_iLogLevel));
	file.WriteString(GetIniLineText(CString(L"ServerID"), m_uManagerID ));
	file.WriteString(GetIniLineText(CString(L"ServerName"), m_strManagerName));
	file.WriteString(GetIniLineText(CString(L"CenterIp"), m_strCenterOutsideIP));
	file.WriteString(GetIniLineText(CString(L"CenterPort"), m_iCenterOutsidePort));
	file.WriteString(GetIniLineText(CString(L"InsideIp"), m_strManagerInsideIP));
	file.WriteString(GetIniLineText(CString(L"InsidePort"), m_iManagerInsidePort));
	file.WriteString(GetIniLineText(CString(L"DBIp"), m_strLogicDBIP));
	file.WriteString(GetIniLineText(CString(L"DBPort"), m_iLogicDBPort));
	file.WriteString(GetIniLineText(CString(L"RedisIp"), m_strRedistIP));
	file.WriteString(GetIniLineText(CString(L"RedisPort"), m_iRedistPort));
	file.WriteString(GetIniLineText(CString(L"AddRobot"), m_bManagerRobot ? 1 : 0));
	file.WriteString(GetIniLineText(CString(L"DebugModel"), m_bManagerDebugMode ? 1 : 0));
	file.WriteString(GetIniLineText(CString(L"DeskCountLimitForLogicServer"), m_iManagerDeskCountLimit));
	file.WriteString(GetIniLineText(CString(L"FailoverLogics"), CString("")));
	//file.WriteString(GetIniLineText(CString(L"StartTime"), 1472140800));
	file.Close();
	return TRUE;
}
BOOL CSXLauncherDlg::WriteGateServerConfigFile(CString &strPath, CString & strDstPath, CString & strDstFile, int iVarible, LogicManagerConfig &manager)
{
 
	CStdioFile file;

	int port = m_iGatePortStart + iVarible;
	int id = m_uGateServerIDStart + iVarible;
	strDstPath.Format(L"%s\\%02d_%04d", strPath, id, port);
	::CreateDirectory(strDstPath, NULL);
	strDstFile.Format(L"%s\\gate_config.ini", strDstPath);
	file.Open(strDstFile, CFile::modeCreate | CFile::modeWrite);

	file.WriteString(GetIniLineText(CString(L"LogLevel"), m_iLogLevel));
	file.WriteString(GetIniLineText(CString(L"Id"), id));

	file.WriteString(GetIniLineText(CString(L"LogicManagerIp"), manager.IP));
	file.WriteString(GetIniLineText(CString(L"LogicManagerPort"), manager.Port));
	file.WriteString(GetIniLineText(CString(L"OutsideIp"), m_strGateOutsideIP));
	file.WriteString(GetIniLineText(CString(L"OutsidePort"), port));
	file.WriteString(GetIniLineText(CString(L"InsideIp"), CString(L"127.0.0.1")));
	file.WriteString(GetIniLineText(CString(L"InsidePort"), port + 500));
	file.WriteString(GetIniLineText(CString(L"GaoFangFlag"), m_iGaoFangConfig));

	file.Close();	 
	return TRUE;
}
BOOL CSXLauncherDlg::WriteLogicServerConfigFile(CString &strPath, CString &strDstPath, CString & strDstFile, int iVarible, LogicManagerConfig &manager)
{
	CStdioFile file;

	int port =  m_iLogicInsidePortStart + iVarible;
	int id = m_iLogicIDStart + iVarible;
	strDstPath.Format(L"%s\\%04d_%04d", strPath, id, port);
	::CreateDirectory(strDstPath, NULL);
	strDstFile.Format(L"%s\\logic_config.ini", strDstPath);
	file.Open(strDstFile, CFile::modeCreate | CFile::modeWrite);

	file.WriteString(GetIniLineText(CString(L"LogLevel"), m_iLogicLogLevel));
	file.WriteString(GetIniLineText(CString(L"ServerID"), id));

	//这个端口要区分
	file.WriteString(GetIniLineText(CString(L"LogicManagerIp"), manager.IP));
	file.WriteString(GetIniLineText(CString(L"LogicManagerPort"), manager.Port));
	 
	file.WriteString(GetIniLineText(CString(L"InsideIp"), m_strLogicInsideIP));
	file.WriteString(GetIniLineText(CString(L"InsidePort"), port));

	file.WriteString(GetIniLineText(CString(L"DBIp"), m_strLogicDBIP));
	file.WriteString(GetIniLineText(CString(L"DBPort "), m_iLogicDBPort));
	file.WriteString(GetIniLineText(CString(L"AddRobot"), m_bLogicRobotEnable ? 1 : 0));
	file.WriteString(GetIniLineText(CString(L"DebugModel"), m_bLogicDebugMode ? 1 : 0));
	file.Close();
	return TRUE;
}

BOOL CSXLauncherDlg::WriteLogicServerConfigFileForQiPai(CString &strPath, CString &strDstPath, CString & strDstFile, int iVarible, LogicManagerConfig &manager)
{
	CStdioFile file;

	int port = m_nLogicQipaiStartPort + iVarible;
	int id = m_nLogicQipaiStartId + iVarible;
	strDstPath.Format(L"%s\\%04d_%04d", strPath, id, port);
	::CreateDirectory(strDstPath, NULL);
	strDstFile.Format(L"%s\\qipai_config.ini", strDstPath);
	file.Open(strDstFile, CFile::modeCreate | CFile::modeWrite);

	file.WriteString(GetIniLineText(CString(L"LogLevel"), m_iLogicLogLevel));
	file.WriteString(GetIniLineText(CString(L"ServerID"), id));

	//这个端口要区分
	file.WriteString(GetIniLineText(CString(L"LogicManagerIp"), manager.IP));
	file.WriteString(GetIniLineText(CString(L"LogicManagerPort"), manager.Port));

	file.WriteString(GetIniLineText(CString(L"InsideIp"), m_strLogicQipaiServerIp));
	file.WriteString(GetIniLineText(CString(L"InsidePort"), port));

	file.WriteString(GetIniLineText(CString(L"DBIp"), m_strLogicDBIP));
	file.WriteString(GetIniLineText(CString(L"DBPort "), m_iLogicDBPort));
	file.WriteString(GetIniLineText(CString(L"OnlySupportGameType"), m_nLogicQipaiSupportGameType));
	file.WriteString(GetIniLineText(CString(L"AddRobot"), m_bLogicQipaiRobot ? 1 : 0));
	file.WriteString(GetIniLineText(CString(L"DebugModel"), m_bLogicQipaiDebugMode ? 1 : 0));
	file.Close();
	return TRUE;
}

BOOL CSXLauncherDlg::LaunchProcess(LPCTSTR szAppName, LPCTSTR szTitle, LPCTSTR szWorkPath, LPCTSTR szConfigFilePath, ServerType type, int nIden, ProcessInfo * pProcessInfo)
{
	m_myEvent.ResetEvent();
	TCHAR szTemp[MAX_PATH] = {0};
	TCHAR szCommandLine[MAX_PATH] = { 0 };
	TCHAR szMessage[256] = { 0 };
	STARTUPINFO info = { 0 };
	info.cb = sizeof(info);
	if (szTitle)
	{
		lstrcpyn(szTemp, szTitle, MAX_PATH);
		info.lpTitle = szTemp;
	} 
	PROCESS_INFORMATION pInfo = {0};
	CString strAppName; 
	if (type == ServerType_Redis)
	{
		strAppName = szWorkPath;
		_sntprintf_s(szCommandLine, MAX_PATH, L"\"%s\\%s\" %s", szWorkPath, szAppName, szConfigFilePath);
		 
	}
	else
	{
		strAppName = m_szCurrentWorkPath;		
	}
	strAppName += L"\\"; strAppName += szAppName;

	 
	BOOL b = FALSE;
	if (type == ServerType_Redis)
	{
	  	b = ::CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, szWorkPath, &info, &pInfo);
	}
	else
	{
		b = ::CreateProcess(strAppName, NULL, NULL, NULL, FALSE, 0, NULL, szWorkPath, &info, &pInfo);
	}
	if (b)
	{
		
		::CloseHandle(pInfo.hThread); //关闭线程句柄
		if (NULL == pProcessInfo) // 添加节点
		{
			ProcessInfo * pProcess = new ProcessInfo();
			if (pProcess)
			{
				pProcess->port = GetProcessPort(type, nIden);
				pProcess->processId = pInfo.dwProcessId;
				pProcess->process_handle = pInfo.hProcess;
				pProcess->nIden = nIden;
				pProcess->server_type = type;
				pProcess->start_time = (DWORD)time(NULL);
				pProcess->m_bInProtected = m_bEnableProtect;
				lstrcpyn(pProcess->szWorkPath, szWorkPath, MAX_PATH);
				lstrcpyn(pProcess->szConfigPath, szConfigFilePath, MAX_PATH);
				if(szTitle)
				   lstrcpyn(pProcess->szTitle, szTitle, MAX_PATH);
				lstrcpyn(pProcess->szWorkPath, szWorkPath, MAX_PATH);
				ProcessInfo * p = NULL;
				if (!m_mapProcessInfos.Lookup(pInfo.dwProcessId, p)) //加入新表项目
				{
					m_mapProcessInfos[pInfo.dwProcessId] = pProcess;
				}
				_sntprintf_s(szMessage, 256, L"服务启动成功 type=%d, port=%d processid=%d name=%s ", pProcess->server_type, pProcess->port, pProcess->processId,(LPCTSTR)strAppName);
				this->InsertLogString(LogLevel_INFO, szMessage);
			}
			
		}
		else //替换节点
		{
			::CloseHandle(pProcessInfo->process_handle); // 关闭原来句柄
			ProcessInfo * pProcess = new ProcessInfo();
			if (pProcess)
			{
				pProcess->port = GetProcessPort(type, nIden);
				pProcess->processId = pInfo.dwProcessId;
				pProcess->process_handle = pInfo.hProcess;
				pProcess->nIden = nIden;
				pProcess->server_type = type;
				pProcess->start_time = (DWORD)time(NULL);
				pProcess->m_bInProtected = m_bEnableProtect;
				lstrcpyn(pProcess->szWorkPath, szWorkPath, MAX_PATH);
				lstrcpyn(pProcess->szConfigPath, szConfigFilePath, MAX_PATH);
				if (szTitle)
					lstrcpyn(pProcess->szTitle, szTitle, MAX_PATH);
				lstrcpyn(pProcess->szWorkPath, szWorkPath, MAX_PATH);
				ProcessInfo * p = NULL;
				DWORD old_process_id = pProcessInfo->processId;
				delete m_mapProcessInfos[old_process_id]; //清除旧的进程信息
				m_mapProcessInfos.RemoveKey(old_process_id); //删除表项
				if (!m_mapProcessInfos.Lookup(pInfo.dwProcessId, p)) //加入新表项目
				{
					m_mapProcessInfos[pInfo.dwProcessId] = pProcess;
				}
				_sntprintf_s(szMessage, 256, L"服务重新启动成功 type=%d, port=%d processid=%d name=%s", pProcess->server_type, pProcess->port, pProcess->processId, (LPCTSTR)strAppName);
				this->InsertLogString(LogLevel_ERROR, szMessage);
			}
			 
		}


		
	}
	m_myEvent.SetEvent();
	return b;
}



void CSXLauncherDlg::OnClickedButtonRedistStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CString strConfigFile;
	if (CheckServerIsInRunning(ServerType_Redis, m_iRedistPort))
	{
		CString strTitle;
		strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!", m_iRedistPort);
		AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);
		return;
	}

	strConfigFile.Format(L"redis.windows.conf");
	BOOL bRet = LaunchProcess(L"redis-server.exe", NULL, m_szRedisWorkPath, strConfigFile, ServerType_Redis, 0);
	if (bRet)
	{
	}
	else
	{
		AfxMessageBox(L"启动失败!", MB_OK | MB_ICONERROR);
	}	 
	return;
}




void CSXLauncherDlg::OnClickedButtonLogicdbStart()
{
	//TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CString strLogicDBWorkPath;
	CString strLogicDBConfigFile;
	CString strLogicDBTitle;
	if (CheckServerIsInRunning(ServerType_LogicDB, m_iLogicDBPort))
	{
		CString strTitle;
		strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!", m_iLogicDBPort);
		AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);
		return;
	}
	strLogicDBTitle.Format(L"LogicDB_Port_%04d", m_iLogicDBPort);
	WriteLogicDBServerConfigFile(CString(m_szLogicDBServerPath), strLogicDBWorkPath, strLogicDBConfigFile);
	BOOL b = LaunchProcess(L"LogicDB.exe", strLogicDBTitle, strLogicDBWorkPath, strLogicDBConfigFile, ServerType_LogicDB, 0);
}


void CSXLauncherDlg::OnClickedButtonCenterStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CString strCenterWorkPath, strConfigFile;
	if (CheckServerIsInRunning(ServerType_Center, m_iCenterOutsidePort))
	{
		CString strTitle;
		strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!", m_iCenterOutsidePort);
		AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);
		return;
	}
	if (WriteCenterConfigFile(CString(m_szCenterPath), strCenterWorkPath, strConfigFile))
	{
		CString strTitle;
		strTitle.Format(L"CenterServer_Port_%04d", m_iCenterOutsidePort);
		LaunchProcess(L"CenterServer.exe", strTitle, strCenterWorkPath, strConfigFile, ServerType_Center, 0);
	}
}


void CSXLauncherDlg::OnClickedButtonLoginStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CString strFilePath, strDestFileName, strTitle;	
	strDestFileName.Format(L"%s\\loginserver_config.ini", m_szCurrentWorkPath);
	for (int i = 0; i < m_nLoginServerCount; i++)
	{
		CString strConfigFile, strConfigFilePath;
		if (CheckServerIsInRunning(ServerType_Login,  m_iLoginOutsidePort + i))
		{
			CString strTitle;
			strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!", m_iLoginOutsidePort + i);
			AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);	
			continue;
		}
		 

		if (WriteLoginConfigFile(CString(m_szLoginPath), i, strConfigFile, strConfigFilePath))
		{	
			strTitle.Format(L"LoginServer_Port_%d", m_iLoginOutsidePort + i);
			LaunchProcess(L"LoginServer.exe", strTitle, strConfigFilePath, strConfigFilePath, ServerType_Login, i);
			Sleep(1000);
		}
	}
}


void CSXLauncherDlg::OnClickedButtonGateStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CString strConfigFile, strConfigFilePath, strTitle;
	for (int i = 0; i < m_nGateServerCount; i++)
	{
		if (CheckServerIsInRunning(ServerType_Gate, m_iGatePortStart + i))
		{
			CString strTitle;
			strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!",  m_iGatePortStart + i);
			AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);
			continue;
		}

		LogicManagerConfig manager = ServerConfigHelper::GetInstance().GetMappedLogicManagerByGateId(m_uGateServerIDStart + i);
		if (!manager.IsValid())
		{
			CString strText;
			strText.Format(L"未找到LogicManager. GateIP=%s, ID=%d, port= %d", m_strGateOutsideIP, m_uGateServerIDStart + i, m_iGatePortStart + i);
			MessageBox(strText);
			break;
		}
		if (WriteGateServerConfigFile(CString(m_szGateServerPath), strConfigFilePath, strConfigFile, i, manager))
		{
			strTitle.Format(L"GateServer_Port_%d", m_iGatePortStart + i);
			LaunchProcess(L"GateServer.exe", strTitle, strConfigFilePath, strConfigFilePath, ServerType_Gate, i);
			Sleep(1000);
		}
	}

}


void CSXLauncherDlg::OnClickedButtonLogicStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strConfigFile, strConfigFilePath, strTitle;
	UpdateData();
	for (int i = 0; i < m_nLogicServerCount; i++)
	{	
		if (CheckServerIsInRunning(ServerType_Logic,  m_iLogicInsidePortStart + i))
		{
			CString strTitle;
			strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!", m_iLogicInsidePortStart + i);
			AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);
			continue;
		}

		LogicManagerConfig manager = ServerConfigHelper::GetInstance().GetMappedLogicManagerByLogicId(m_iLogicIDStart + i);
		if (!manager.IsValid())
		{
			CString strText;
			strText.Format(L"未找到LogicManager.LogicIP=%s, ID=%d, port= %d", m_strLogicInsideIP, m_iLogicIDStart + i, m_iLogicInsidePortStart + i);
			MessageBox(strText);
			break;
		}

		if (WriteLogicServerConfigFile(CString(m_szLogicServerPath), strConfigFilePath, strConfigFile, i, manager))
		{
			strTitle.Format(L"LogicServer_Port_%04d", m_iLogicInsidePortStart + i);
			LaunchProcess(L"LogicServer.exe", strTitle, strConfigFilePath, strConfigFile, ServerType_Logic, i);
			Sleep(1000);
		}
	}
}


void CSXLauncherDlg::OnClickedButtonManagerStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CString strManagerWorkPath, strConfigFile;
	if (CheckServerIsInRunning(ServerType_Manager, m_iManagerInsidePort))
	{
		CString strTitle;
		strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!", m_iManagerInsidePort);
		AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);
		return;
	}
	BOOL bRet = WriteLogicManagerServerConfigFile(CString(m_szLogicManagerServerPath), strManagerWorkPath, strConfigFile);
	if(bRet)
	{
		CString strTitle;
		strTitle.Format(L"LogicManager_Port_%04d", m_iManagerInsidePort);
		LaunchProcess(L"LogicManager.exe", strTitle, strManagerWorkPath, strConfigFile, ServerType_Manager, 0);
	}

}

BOOL CSXLauncherDlg::ReLuanchProcess(ProcessInfo & processInfo)
{
	switch (processInfo.server_type)
	{
	case ServerType_Center:
		LaunchProcess(L"CenterServer.exe", processInfo.szTitle, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_Center, processInfo.nIden, &processInfo);
		break;
	case ServerType_Login:
		LaunchProcess(L"LoginServer.exe", processInfo.szTitle, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_Login, processInfo.nIden, &processInfo);
		break;
	case ServerType_Manager:
		LaunchProcess(L"LogicManager.exe", processInfo.szTitle, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_Manager, processInfo.nIden, &processInfo);
		break;
	case ServerType_LogicDB:
		LaunchProcess(L"LogicDB.exe", processInfo.szTitle, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_LogicDB, processInfo.nIden, &processInfo);
		break;
	case ServerType_Logic:
		LaunchProcess(L"LogicServer.exe", processInfo.szTitle, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_Logic, processInfo.nIden, &processInfo);
		break;
	case ServerType_Gate:
		LaunchProcess(L"GateServer.exe", processInfo.szTitle, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_Gate, processInfo.nIden, &processInfo);
		break;
	case ServerType_Redis:
		LaunchProcess(L"redis-server.exe", NULL, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_Redis, processInfo.nIden, &processInfo);
		break;
	case ServerType_Logic_QiPai:
		LaunchProcess(L"LogicQipai.exe", processInfo.szTitle, processInfo.szWorkPath, processInfo.szConfigPath, ServerType_Logic_QiPai, processInfo.nIden, &processInfo);
		break;
	default: break;
	}
	return true;
}

UINT CSXLauncherDlg::MoniterThread(LPVOID pVoid)
{
	CSXLauncherDlg * p = (CSXLauncherDlg*)pVoid;
	TCHAR szText[256];
	_sntprintf_s(szText, 256, L"**进程守护线程启动** ThreadID: %d", GetCurrentThreadId());
	p->SendMessage(WM_MESSAGE_TRACE_INFO, LogLevel_DEBUG, (LPARAM)szText);
	POSITION pos = NULL;
	ProcessInfo * pEndProcess = NULL;
	do
	{
		Sleep(1000);	 
		p->m_myEvent.ResetEvent();
		pEndProcess = NULL;
		pos =  p->m_mapProcessInfos.GetStartPosition();
		while(pos)
		{
			DWORD dwID = 0;
			ProcessInfo * pInfo = NULL;
			p->m_mapProcessInfos.GetNextAssoc(pos, dwID, pInfo);
			if (NULL != pInfo)
			{
				if ( WAIT_OBJECT_0 ==  WaitForSingleObject(pInfo->process_handle, 1))
				{
					_sntprintf_s(szText, 256, L"**捕获进程关闭事件: ProcessID:%d", pInfo->processId);
					p->SendMessage(WM_MESSAGE_TRACE_INFO, LogLevel_DEBUG, (LPARAM)szText);
					pEndProcess = pInfo;
					break;
				}
			}
			 
		}

		if (pEndProcess && (!pEndProcess->m_bInProtected))
		{
			DWORD id = pEndProcess->processId;

			//进程已经关闭，不在保护状态，直接删除掉
			::CloseHandle(pEndProcess->process_handle);
			delete pEndProcess;
			pEndProcess = NULL;
			p->m_mapProcessInfos.RemoveKey(id);
		}

		p->m_myEvent.SetEvent();
		if (pEndProcess)
		{
			_sntprintf_s(szText, 256, L"**发送重新启动进程 ProcessID:%d", pEndProcess->processId);
			p->SendMessage(WM_MESSAGE_TRACE_INFO, LogLevel_DEBUG, (LPARAM)szText);
			// Restarted
			p->SendMessage(WM_MESSAGE_RELAUNCH_PROCESS, (WPARAM)pEndProcess);			 
		}
		
	} while (p->moniter_thread);
	//p->m_pMoniterThread = NULL;
	_sntprintf_s(szGlobalMessage, 256, L"**进程守护线程结束** threadid: %d", GetCurrentThreadId());
    p->PostMessage(WM_MESSAGE_TRACE_INFO, LogLevel_DEBUG, (LPARAM)szGlobalMessage);
	return 0;
}


void CSXLauncherDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int ret = AfxMessageBox(L"确定要关闭服务器启动守护程序? 关闭后所有服务器进程可以单独关闭！", MB_OKCANCEL | MB_ICONQUESTION);
	if (ret == IDOK)
	{
	}
	else
	{
		return;
	}	

	if (moniter_thread && m_pMoniterThread)
	{
		moniter_thread = 0;
	}
	::WaitForSingleObject(m_pMoniterThread->m_hThread, 10000);
	TRACE(L"*********End thread func--CSXLauncherDlg::OnClose\r\n");

	// Release process map table
	POSITION p = m_mapProcessInfos.GetStartPosition();
	while (p)
	{
		ProcessInfo * pInfo = NULL;
		DWORD process_id = 0;
		m_mapProcessInfos.GetNextAssoc(p, process_id, pInfo);
		if (pInfo)
		{
			::CloseHandle(pInfo->process_handle);
			delete pInfo;
		}
	}	
	CDialogEx::OnClose();
}

BOOL CSXLauncherDlg::CheckServerIsInRunning(ServerType server_type, int port)
{
	
	m_myEvent.ResetEvent();
	POSITION pos = m_mapProcessInfos.GetStartPosition();
	while (pos)
	{
	   DWORD dwProcessID;
	   ProcessInfo * pProcessInfo = NULL;
	   m_mapProcessInfos.GetNextAssoc(pos, dwProcessID, pProcessInfo);
	   if (pProcessInfo && (pProcessInfo->port == port) && (pProcessInfo->server_type == server_type))
	   {
		   return TRUE;
	   }
	}
	m_myEvent.SetEvent();

	
	return FALSE;
}


void CSXLauncherDlg::OnClickedButtonOnekeyClose()
{
	// TODO: 在此添加控件通知处理程序代码
	if(IDOK != AfxMessageBox(L"确定关闭所有服务程序?", MB_OKCANCEL | MB_ICONQUESTION))
	{
	    return;
    }
	m_myEvent.ResetEvent();
	//终止所有监听的进程
	// Release process map table
	ServerType types[] = {ServerType_Login, ServerType_Gate, ServerType_Logic, ServerType_Logic_QiPai, ServerType_Manager,
	  ServerType_Center, ServerType_LogicDB, ServerType_Redis};
	for (int i = 0; i < sizeof(types); i++)
	{
		POSITION p = m_mapProcessInfos.GetStartPosition();
		while (p)
		{
			ProcessInfo * pInfo = NULL;
			DWORD process_id = 0;
			m_mapProcessInfos.GetNextAssoc(p, process_id, pInfo);
			if (pInfo && pInfo->server_type == types[i])
			{
				::TerminateProcess(pInfo->process_handle, -1);
				::CloseHandle(pInfo->process_handle);
				CString strText;
				strText.Format(L"服务器关闭. Type:%d port:%d index:%d processid:%d title:%s", pInfo->server_type, pInfo->port,  pInfo->nIden, pInfo->processId, pInfo->szTitle);
				delete pInfo;
				this->InsertLogString(LogLevel_INFO, strText);
				Sleep(1000);
			}
		}
	}
	// 清空MAP
	m_mapProcessInfos.RemoveAll();
	m_myEvent.SetEvent();
}

COLORREF CSXLauncherDlg::GetTextColorByLevel(LogLevel level)
{
	switch (level)
	{
	case LogLevel_ERROR:
		return color_error;
	case LogLevel_INFO:
		return color_info;
	case LogLevel_DEBUG:
		return color_debug;
	default:
		return RGB(0, 0, 0);
	}
	return 0;
}

void CSXLauncherDlg::InsertLogString(LogLevel level, LPCTSTR szText)
{
	if (NULL == szText)
		return;
	SYSTEMTIME stm = { 0 };
	TCHAR szTimeBuf[64] = { 0 };
	TCHAR szLineText[512];
	GetLocalTime(&stm);
	_sntprintf_s(szTimeBuf, 64, L"【 %04d-%02d-%02d %02d:%02d:%02d 】", stm.wYear, stm.wMonth, stm.wDay, stm.wHour, stm.wMinute, stm.wSecond);
	_sntprintf_s(szLineText, 512, L"%s\r\n", szText);

	CHARFORMAT2 charFmt;
	ZeroMemory(&charFmt, sizeof(CHARFORMAT2));
	charFmt.cbSize = sizeof(charFmt);
	charFmt.crTextColor = RGB(10,10,10);
	charFmt.dwMask = CFM_COLOR;

	// 插入时间
	LONG lTextLen = m_ctrlLogs.GetTextLengthEx(GTL_NUMCHARS);
	m_ctrlLogs.SetSel(lTextLen, lTextLen);
	m_ctrlLogs.SetSelectionCharFormat(charFmt);
	m_ctrlLogs.ReplaceSel(szTimeBuf);

	// 插入数据
	charFmt.crTextColor = GetTextColorByLevel(level);
	lTextLen = m_ctrlLogs.GetTextLengthEx(GTL_NUMCHARS);
	m_ctrlLogs.SetSel(lTextLen, lTextLen);
	m_ctrlLogs.SetSelectionCharFormat(charFmt);
	m_ctrlLogs.ReplaceSel(szLineText);

	PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

LRESULT CSXLauncherDlg::OnCustomMessageRelaunchProcess(WPARAM wParam, LPARAM lParam)
{
	ASSERT(NULL != wParam); 
	if (NULL == wParam)
		return -1;
	ProcessInfo * p = (ProcessInfo*)wParam;
	CString strText;
	strText.Format(L"服务器关闭，重新启动中...");
	InsertLogString(LogLevel_ERROR, strText);
	strText.Format(L"进程信息 processid:%d, port:%d, server type=%d", p->processId, p->port, p->server_type);
	this->InsertLogString(LogLevel_ERROR, strText);
	this->ReLuanchProcess(*(ProcessInfo*)wParam);

	string result;
	//报警
	if (m_bAlertEnable)
	{
		CMyHttpRequest::InvokeGet(string(_MH_ALERT_URL), result, false, 15);
	}
	return 0;
}

LRESULT CSXLauncherDlg::OnCustomMessageInsertLog(WPARAM wParam, LPARAM lParam)
{
	ASSERT(NULL != wParam);
	ASSERT(NULL != lParam);
	if (NULL == wParam || NULL == lParam)
		return -1;
	this->InsertLogString((LogLevel)wParam, (LPCTSTR)lParam);
	return 0;
}

BOOL CSXLauncherDlg::SaveParamConfigFile()
{
	//redis
	WriteIniString(L"redis", L"ip", m_strRedistIP);
	WriteIniInt(L"redis", L"port", m_iRedistPort);

	//db server
	WriteIniString(L"dbserver", L"ip", m_strDatabaseIP);
	WriteIniInt(L"dbserver", L"port", m_iDatabasePort);
	WriteIniString(L"dbserver", L"user", m_strDatabaseUserName);
	WriteIniString(L"dbserver", L"password", m_strDatabasePassword);

	WriteIniString(L"dbserver", L"ip_bk", m_strDatabaseIPBk);
	WriteIniInt(L"dbserver", L"port_bk", m_uDatabasePortBk);
	WriteIniString(L"dbserver", L"user_bk", m_strDatabaseUserNameBk);
	WriteIniString(L"dbserver", L"password_bk", m_strDatabasePasswordBk);

	WriteIniString(L"dbserver", L"center_db_name", m_strDatabaseCenterDBName);	
	WriteIniString(L"dbserver", L"logic_db_name", m_strDatabaseLogicDBName);
	WriteIniString(L"dbserver", L"club_db_name", m_strClubDBName);

	// center
	WriteIniString(L"center", L"inside_ip", m_strCenterOutsideIP);
	WriteIniInt(L"center", L"inside_port", m_iCenterOutsidePort);
	WriteIniString(L"center", L"gm_ip", m_strCenterGMIP);
	WriteIniInt(L"center", L"gm_port", m_iCenterGMPort);
	WriteIniInt(L"center", L"club_server_count", m_nClubManagerCount);

	// logic db
	WriteIniString(L"logicdb", L"ip", m_strLogicDBIP);
	WriteIniInt(L"logicdb", L"port", m_iLogicDBPort);

	// login
	WriteIniString(L"login", L"outside_ip", m_strLoginOutsideIP);
	WriteIniInt(L"login", L"outside_port_start", m_iLoginOutsidePort);
	WriteIniString(L"login", L"inside_ip", m_strLoginInsideIP);
	WriteIniInt(L"login", L"inside_port_start", m_iLoginInsidePort);
	WriteIniInt(L"login", L"count", m_nLoginServerCount);
	//WriteIniInt(L"login", L"gaofang_flag", (int)m_bLoginGaoFang);

	// logic manager
	WriteIniString(L"manager", L"ip", m_strManagerInsideIP);
	WriteIniInt(L"manager", L"port", m_iManagerInsidePort);
	WriteIniString(L"manager", L"server_name", m_strManagerName);
	WriteIniInt(L"manager", L"id", m_uManagerID);
	WriteIniInt(L"manager", L"debug_mode", (int)m_bManagerDebugMode);
	WriteIniInt(L"manager", L"robot", (int)m_bManagerRobot);
	WriteIniInt(L"manager", L"desk_limit", m_iManagerDeskCountLimit);
	
	//Logic
	WriteIniString(L"logic", L"inside_ip_start", m_strLogicInsideIP);
	WriteIniInt(L"logic", L"inside_port_start", m_iLogicInsidePortStart);
	WriteIniInt(L"logic", L"id_start", m_iLogicIDStart);
	WriteIniInt(L"logic", L"count", m_nLogicServerCount);
	WriteIniInt(L"logic", L"debug_mode", (int)m_bLogicDebugMode);
	WriteIniInt(L"logic", L"robot", (int)m_bLogicRobotEnable);
	WriteIniInt(L"logic", L"log_level", m_iLogicLogLevel);
	
	//Logic qipai
	WriteIniString(L"logic_qipai", L"inside_ip_start", m_strLogicQipaiServerIp);
	WriteIniInt(L"logic_qipai", L"inside_port_start", m_nLogicQipaiStartPort);
	WriteIniInt(L"logic_qipai", L"id_start", m_nLogicQipaiStartId);
	WriteIniInt(L"logic_qipai", L"count", m_nLogicQipaiCount);	 
	WriteIniInt(L"logic_qipai", L"debug_mode", m_bLogicQipaiDebugMode ? 1 : 0);
	WriteIniInt(L"logic_qipai", L"robot",  m_bLogicQipaiRobot ? 1 : 0);
	WriteIniInt(L"logic_qipai", L"log_level", m_nLogicQipaiLogLevel);
	WriteIniInt(L"logic_qipai", L"only_support_gametype", m_nLogicQipaiSupportGameType);

	//gate
	WriteIniString(L"gate", L"outside_ip_start", m_strGateOutsideIP);
	WriteIniInt(L"gate", L"outside_port_start", m_iGatePortStart);	 
	WriteIniInt(L"gate", L"count", m_nGateServerCount);
	WriteIniInt(L"gate", L"id_start", m_uGateServerIDStart);
	//WriteIniInt(L"gate", L"gaofang_flag", (int)m_bGateGaoFang);
	 
	//Log level
	WriteIniInt(L"LOG", L"log_level", m_iLogLevel);
	WriteIniInt(L"LOG", L"gaofang_config", m_iGaoFangConfig);
	WriteIniInt(L"LOG", L"alert_enable", m_bAlertEnable);
	WriteIniInt(L"LOG", L"protect_enable", m_bEnableProtect);
	return true;
}

BOOL CSXLauncherDlg::LoadLauncherConfigFile()
{
#define LOAD_PARAM_STRING(s, k, v) v = GetIniString(s,k,v);
#define LOAD_PARAM_INT(s, k, v) v = GetIniInt(s, k, (int)v);
	//redis
	LOAD_PARAM_STRING(L"redis", L"ip", m_strRedistIP);
	LOAD_PARAM_INT(L"redis", L"port", m_iRedistPort);	

	//db server
	LOAD_PARAM_STRING(L"dbserver", L"ip", m_strDatabaseIP);
	LOAD_PARAM_INT(L"dbserver", L"port", m_iDatabasePort);
	LOAD_PARAM_STRING(L"dbserver", L"user", m_strDatabaseUserName);
	LOAD_PARAM_STRING(L"dbserver", L"password", m_strDatabasePassword);
	LOAD_PARAM_STRING(L"dbserver", L"center_db_name", m_strDatabaseCenterDBName);
	LOAD_PARAM_STRING(L"dbserver", L"logic_db_name", m_strDatabaseLogicDBName);
	LOAD_PARAM_STRING(L"dbserver", L"club_db_name",  m_strClubDBName);

	LOAD_PARAM_STRING(L"dbserver", L"ip_bk", m_strDatabaseIPBk);
	LOAD_PARAM_INT(L"dbserver", L"port_bk", m_uDatabasePortBk);
	LOAD_PARAM_STRING(L"dbserver", L"user_bk", m_strDatabaseUserNameBk);
	LOAD_PARAM_STRING(L"dbserver", L"password_bk", m_strDatabasePasswordBk);

	// center
	LOAD_PARAM_STRING(L"center", L"inside_ip", m_strCenterOutsideIP);
	LOAD_PARAM_INT(L"center", L"inside_port", m_iCenterOutsidePort);
	LOAD_PARAM_STRING(L"center", L"gm_ip", m_strCenterGMIP);
	LOAD_PARAM_INT(L"center", L"gm_port", m_iCenterGMPort);
	LOAD_PARAM_INT(L"center", L"club_server_count", m_nClubManagerCount);

	// logic db
	LOAD_PARAM_STRING(L"logicdb", L"ip", m_strLogicDBIP);
	LOAD_PARAM_INT(L"logicdb", L"port", m_iLogicDBPort);

	// login
	LOAD_PARAM_STRING(L"login", L"outside_ip", m_strLoginOutsideIP);
	LOAD_PARAM_INT(L"login", L"outside_port_start", m_iLoginOutsidePort);
	LOAD_PARAM_STRING(L"login", L"inside_ip", m_strLoginInsideIP);
	LOAD_PARAM_INT(L"login", L"inside_port_start", m_iLoginInsidePort);
	LOAD_PARAM_INT(L"login", L"count", m_nLoginServerCount);	
	//LOAD_PARAM_INT(L"login", L"gaofang_flag", m_bLoginGaoFang);
	 

	// logic manager
	LOAD_PARAM_STRING(L"manager", L"ip", m_strManagerInsideIP);
	LOAD_PARAM_INT(L"manager", L"port", m_iManagerInsidePort);
	LOAD_PARAM_STRING(L"manager", L"server_name", m_strManagerName);
	LOAD_PARAM_INT(L"manager", L"id", m_uManagerID);
	LOAD_PARAM_INT(L"manager", L"debug_mode", m_bManagerDebugMode);
	LOAD_PARAM_INT(L"manager", L"robot", m_bManagerRobot);
	LOAD_PARAM_INT(L"manager", L"desk_limit", m_iManagerDeskCountLimit);


	//Logic
	LOAD_PARAM_STRING(L"logic", L"inside_ip_start", m_strLogicInsideIP);
	LOAD_PARAM_INT(L"logic", L"inside_port_start", m_iLogicInsidePortStart);
	LOAD_PARAM_INT(L"logic", L"id_start", m_iLogicIDStart);
	LOAD_PARAM_INT(L"logic", L"count", m_nLogicServerCount);
	LOAD_PARAM_INT(L"logic", L"debug_mode", m_bLogicDebugMode);
	LOAD_PARAM_INT(L"logic", L"robot", m_bLogicRobotEnable);
	LOAD_PARAM_INT(L"logic", L"log_level", m_iLogicLogLevel);

	//Logic qipai
	LOAD_PARAM_STRING(L"logic_qipai", L"inside_ip_start", m_strLogicQipaiServerIp);
	LOAD_PARAM_INT(L"logic_qipai", L"inside_port_start", m_nLogicQipaiStartPort);
	LOAD_PARAM_INT(L"logic_qipai", L"id_start", m_nLogicQipaiStartId);
	LOAD_PARAM_INT(L"logic_qipai", L"count", m_nLogicQipaiCount);
	LOAD_PARAM_INT(L"logic_qipai", L"debug_mode", m_bLogicQipaiDebugMode);
	LOAD_PARAM_INT(L"logic_qipai", L"robot", m_bLogicQipaiRobot);
	LOAD_PARAM_INT(L"logic_qipai", L"log_level", m_nLogicQipaiLogLevel);
	LOAD_PARAM_INT(L"logic_qipai", L"only_support_gametype", m_nLogicQipaiSupportGameType);

	//gate
	LOAD_PARAM_STRING(L"gate", L"outside_ip_start", m_strGateOutsideIP);
	LOAD_PARAM_INT(L"gate", L"outside_port_start", m_iGatePortStart);
	LOAD_PARAM_INT(L"gate", L"count", m_nGateServerCount);
	LOAD_PARAM_INT(L"gate", L"id_start", m_uGateServerIDStart);
	//LOAD_PARAM_INT(L"gate", L"gaofang_flag", m_bGateGaoFang);
	

	//Log level
	LOAD_PARAM_INT(L"LOG", L"log_level", m_iLogLevel);
	LOAD_PARAM_INT(L"LOG", L"gaofang_config", m_iGaoFangConfig);
	LOAD_PARAM_INT(L"LOG", L"alert_enable", m_bAlertEnable);
	LOAD_PARAM_INT(L"LOG", L"protect_enable", m_bEnableProtect);
#undef LOAD_PARAM_STRING
#undef LOAD_PARAM_INT
	return true;
}


void CSXLauncherDlg::OnClickedButtonLoadConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	this->LoadLauncherConfigFile();
	UpdateData(FALSE);
}


void CSXLauncherDlg::OnClickedButtonSaveConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	SaveParamConfigFile();
}


void CSXLauncherDlg::OnClickedCheckAlert()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(); // 修改变量
}


void CSXLauncherDlg::OnClickedCheckMonitorEnable()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_myEvent.ResetEvent();
	POSITION pos = NULL;
	CArray<DWORD, DWORD&> del;	
	do
	{		
		pos = m_mapProcessInfos.GetStartPosition();
		while (pos)
		{
			DWORD dwID = 0;
			ProcessInfo * pInfo = NULL;
			m_mapProcessInfos.GetNextAssoc(pos, dwID, pInfo);
			if (NULL != pInfo)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(pInfo->process_handle, 1))
				{
					//_sntprintf_s(szText, 256, L"**捕获进程关闭事件: ProcessID:%d", pInfo->processId);
					CString strText;
					strText.Format(L"进程已经关闭，删除节点ProcessID:%d, port=%d, workpath:%s", pInfo->processId, pInfo->port, pInfo->szWorkPath);	
					del.Add(dwID);		
					// 清除改节点	
					InsertLogString(LogLevel::LogLevel_INFO, strText);
					break;
				}
				else
				{
					pInfo->m_bInProtected = m_bEnableProtect; //修改保护状态
				}
			}
		}

		// 清除节点
		for (INT_PTR iIndex = 0; iIndex < del.GetSize(); iIndex++)
		{
			DWORD key = del.GetAt(iIndex);
			CloseHandle(m_mapProcessInfos[key]->process_handle);
			delete m_mapProcessInfos[key];
			m_mapProcessInfos.RemoveKey(key);
		}
	} while (false);
	m_myEvent.SetEvent();
}

void CSXLauncherDlg::InitGateMap()
{
	m_managers.clear();
	LogicManagerConfig config;
	config.Name = m_strManagerName;
	config.Port = m_iManagerInsidePort;
	config.IP = m_strManagerInsideIP;
	config.Type = 0;
	config.ID = m_uManagerID;
	m_managers.push_back(config);

	m_gates.clear();	
	for (int i = 0; i < m_nGateServerCount; i++)
	{
		GateServerConfig gateConfig;
		gateConfig.ID = m_uGateServerIDStart + i;
		gateConfig.Port = m_iGatePortStart + i;
		gateConfig.IP = m_strGateOutsideIP;
		gateConfig.Magager_id = 16210;
		gateConfig.Type = m_iGaoFangConfig;
		m_gates.push_back(gateConfig);
	}
}


void CSXLauncherDlg::OnBnClickedButtonLogicQipaiStart()
{
	// TODO: 在此添加控件通知处理程序代码	
	CString strConfigFile, strConfigFilePath, strTitle;
	UpdateData();
	for (int i = 0; i < m_nLogicQipaiCount ; i++)
	{
		if (CheckServerIsInRunning(ServerType_Logic_QiPai, m_nLogicQipaiStartPort + i))
		{
			CString strTitle;
			strTitle.Format(L"服务器运行中(端口 %d)，请关闭后重试!", m_nLogicQipaiStartPort + i);
			AfxMessageBox(strTitle, MB_OK | MB_ICONERROR);
			continue;
		}

		LogicManagerConfig manager = ServerConfigHelper::GetInstance().GetMappedLogicManagerByLogicId(m_nLogicQipaiStartId + i);
		if (!manager.IsValid())
		{
			CString strText;
			strText.Format(L"未找到LogicManager.LogicIP=%s, ID=%d, port= %d", m_strLogicQipaiServerIp, m_nLogicQipaiStartId + i, m_nLogicQipaiStartPort + i);
			MessageBox(strText);
			break;
		}

		if (WriteLogicServerConfigFileForQiPai(CString(m_szLogicQipaiPath), strConfigFilePath, strConfigFile, i, manager))
		{
			strTitle.Format(L"LogicQipaiServer_Port_%04d", m_nLogicQipaiStartPort + i);
			LaunchProcess(L"LogicQipai.exe", strTitle, strConfigFilePath, strConfigFile, ServerType_Logic_QiPai, i);
			Sleep(1000);
		}
	}
}


void CSXLauncherDlg::OnBnClickedButtonOnekeyStartTest()
{
	// TODO: 在此添加控件通知处理程序代码	
	this->OnClickedButtonCenterStart();	
	this->OnClickedButtonLogicdbStart();
	Sleep(3000);
	this->OnClickedButtonManagerStart();
	Sleep(3000);
	if (IDOK == MessageBox(L"您确定启动 麻将类服务LogicServer ?", L"提示", MB_OKCANCEL))
	{
		this->OnClickedButtonLogicStart();
	}
	if (IDOK == MessageBox(L"您确定启动 棋牌类服务LogicQipai Server ?", L"提示", MB_OKCANCEL))
	{
		this->OnBnClickedButtonLogicQipaiStart();
	}
	 
	this->OnClickedButtonGateStart();
	this->OnClickedButtonLoginStart();
}
