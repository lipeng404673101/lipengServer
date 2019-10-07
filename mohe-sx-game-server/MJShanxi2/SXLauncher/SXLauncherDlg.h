
// SXLauncherDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "ServerConfig.h"

#define  WM_MESSAGE_RELAUNCH_PROCESS (WM_USER+100)
#define  WM_MESSAGE_TRACE_INFO (WM_USER+101)
enum ServerType
{
	ServerType_NULL = 0,
	ServerType_Center = 1,
	ServerType_LogicDB = 2,
	ServerType_Manager = 3,
	ServerType_Gate = 4,
	ServerType_Logic = 5,
	ServerType_Login = 6,
	ServerType_Redis = 7,
	ServerType_Logic_QiPai = 8,
	ServerType_Unknow = 9
};

enum LogLevel
{
	LogLevel_ERROR = 0,
	LogLevel_INFO = 1,
	LogLevel_DEBUG = 2
};

struct ProcessInfo
{
	ServerType server_type;
	HANDLE process_handle;
	DWORD processId;
	DWORD start_time;
	int nIden;
	USHORT port;
	TCHAR szTitle[MAX_PATH];
	TCHAR szWorkPath[MAX_PATH];
	TCHAR szConfigPath[MAX_PATH];
	BOOL m_bInProtected;
	ProcessInfo() : process_handle(INVALID_HANDLE_VALUE), server_type(ServerType_NULL),
		           processId(0), start_time(0), port(0), nIden(0), m_bInProtected(TRUE)
	{
		memset(szTitle, 0, sizeof(szTitle));
		memset(szWorkPath, 0, sizeof(szWorkPath));
		memset(szConfigPath, 0, sizeof(szConfigPath));
	}
};


 


// CSXLauncherDlg 对话框
class CSXLauncherDlg : public CDialogEx
{
// 构造
public:
	CSXLauncherDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SXLAUNCHER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strRedistIP;
	short m_iRedistPort;
	// 数据库服务器地址
	CString m_strDatabaseIP;
	// Center Server数据库名称
	CString m_strDatabaseCenterDBName;
	// Logic数据库名称
	CString m_strDatabaseLogicDBName;
	// 数据库用户名
	CString m_strDatabaseUserName;
	// 访问秘钥
	CString m_strDatabasePassword;
	// 服务端口
	short m_iDatabasePort;
	CString m_strLogicDBIP;
	short m_iLogicDBPort;
//	CString m_strCenterOutsideIP;
//	short m_iCenterOutsidePort;
	CString m_strCenterOutsideIP;
	short m_iCenterOutsidePort;
	CString m_strCenterGMIP;
	short m_iCenterGMPort;
	CString m_strLoginOutsideIP;
	short m_iLoginOutsidePort;
	CString m_strLoginInsideIP;
	short m_iLoginInsidePort;

	BOOL m_bLogicDebugMode;
	BOOL m_bLogicRobotEnable;
	BOOL m_bManagerDebugMode;
	BOOL m_bManagerRobot;

	CString m_strManagerName;
	//	CString m_strManagerInsideIP;
	CString m_strManagerInsideIP;
	short m_iManagerInsidePort;
	UINT m_uManagerID;
	int m_nLoginServerCount;
	int m_nGateServerCount;
	UINT m_uGateServerIDStart;
	CString m_strGateOutsideIP;
	short m_iGatePortStart;
	UINT m_iLogicIDStart;
	CString m_strLogicInsideIP;
	short m_iLogicInsidePortStart;
	int m_nLogicServerCount;

	short m_iGateInsidePortStart;

	TCHAR m_szCurrentWorkPath[MAX_PATH];
	TCHAR m_szRedisWorkPath[MAX_PATH];
	TCHAR m_szCenterPath[MAX_PATH];
	TCHAR m_szLoginPath[MAX_PATH];
	TCHAR m_szLogicDBServerPath[MAX_PATH];
	TCHAR m_szLogicManagerServerPath[MAX_PATH];
	TCHAR m_szGateServerPath[MAX_PATH];
	TCHAR m_szLogicServerPath[MAX_PATH];
	TCHAR m_szLogicQipaiPath[MAX_PATH];

	CMap<DWORD, DWORD, ProcessInfo *, ProcessInfo *> m_mapProcessInfos; // 进程信息
	CEvent m_myEvent;
	CString m_strLauncherConfigFile;

	std::vector<LogicManagerConfig> m_managers;
	std::vector<LogicServerConfig> m_logics;
	std::vector<GateServerConfig> m_gates;
public:
	//logic gate map
	void InitGateMap();

	BOOL SaveParamConfigFile();
	BOOL LoadLauncherConfigFile();
	BOOL WriteCenterConfigFile(CString& strPath, CString& strDstPath, CString& strDstFile);
	BOOL WriteLoginConfigFile(CString & strPath, short portVar, CString & strDestFile, CString &strDestPath);
	BOOL WriteLogicDBServerConfigFile(CString & strPath, CString &strDstPath, CString &strDstFile);
	BOOL WriteLogicManagerServerConfigFile(CString & strPath, CString &strDstPath, CString &strDstFile);
	BOOL WriteGateServerConfigFile(CString &strPath, CString & strDstPath, CString & strDstFile, int iVarible, LogicManagerConfig & manager);
	BOOL WriteLogicServerConfigFile(CString &strPath, CString &strDstPath, CString & strDstFile, int iVarible, LogicManagerConfig &manager);
	BOOL WriteLogicServerConfigFileForQiPai(CString &strPath, CString &strDstPath, CString & strDstFile, int iVarible, LogicManagerConfig &manager);
	BOOL LaunchProcess(LPCTSTR szAppName, LPCTSTR szTitle, LPCTSTR szWorkPath, LPCTSTR szConfigFilePath, ServerType type, int nIden, ProcessInfo * pProcessInfo = NULL);
	BOOL ReLuanchProcess(ProcessInfo & processInfo);
	BOOL CheckServerIsInRunning(ServerType server_type, int nIden);
	inline int GetProcessPort(ServerType server_type, int nIden = 0, bool bOutSidePort = true)
	{
		int nPort = 0;
		switch (server_type)
		{
		case ServerType_Center:
			nPort = m_iCenterOutsidePort + nIden;
			break;
		case ServerType_Manager:
			nPort = m_iManagerInsidePort + nIden;
			break;
		case ServerType_LogicDB:
			nPort = m_iLogicDBPort + nIden;
			break;
		case ServerType_Logic:
			nPort = m_iLogicInsidePortStart + nIden;
			break;
		case ServerType_Gate:
			nPort = m_iGatePortStart + nIden;
			break;
		case ServerType_Login:
			nPort = m_iLoginOutsidePort + nIden;
			break;
		case ServerType_Redis:
			nPort = m_iRedistPort;
			break;
		case ServerType_Logic_QiPai:
			nPort = m_nLogicQipaiStartPort + nIden;
			break;
		default:
			break;
		}
		return nPort;
	}
	inline COLORREF GetTextColorByLevel(LogLevel level);
 
	CString GetIniLineText(CString & strKey, CString &strValue)
	{
		CString strText;
		strText.Format(L"%s = %s\r\n\r\n", strKey, strValue);
		return strText;
	}
	CString GetIniLineText(CString & strKey, int value)
	{
		CString strText;
		strText.Format(L"%s = %d\r\n\r\n", strKey, value);
		return strText;
	}

	inline BOOL WriteIniString(LPCTSTR szSectionName, LPCTSTR szKey, LPCTSTR szText)
	{
		return WritePrivateProfileString(szSectionName, szKey, szText, m_strLauncherConfigFile);
	}
	inline BOOL WriteIniInt(LPCTSTR szSectionName, LPCTSTR szKey, int value)
	{
		CString strContent;
		strContent.Format(L"%d", value);
		return WriteIniString(szSectionName, szKey, strContent);
	}
	inline CString GetIniString(LPCTSTR szSectionName, LPCTSTR szKey, LPCTSTR szDefault)
	{
		TCHAR buf[512] = {0};
		if (GetPrivateProfileString(szSectionName, szKey, szDefault, buf, 512, m_strLauncherConfigFile))
		{
			return CString(buf);
		}
		return CString();
	}
	inline int GetIniInt(LPCTSTR szSectionName, LPCTSTR szKey, int defaultValue)
	{
		return (int)GetPrivateProfileInt(szSectionName, szKey, defaultValue, m_strLauncherConfigFile);
	}
	//
	CWinThread * m_pMoniterThread = NULL;
	static UINT __cdecl MoniterThread(LPVOID pVoid);
	volatile int moniter_thread = 0;
	
	
	afx_msg void OnClickedButtonRedistStart();
	afx_msg void OnClickedButtonLogicdbStart();
	afx_msg void OnClickedButtonCenterStart();
	afx_msg void OnClickedButtonLoginStart();
	afx_msg void OnClickedButtonGateStart();
	afx_msg void OnClickedButtonLogicStart();
	afx_msg void OnClickedButtonManagerStart();
	afx_msg void OnClose();
	int m_iManagerDeskCountLimit;
	int m_iLogLevel;
//	CRichEditCtrl m_ctrlLogs;
	CRichEditCtrl m_ctrlLogs;
	void InsertLogString(LogLevel level, LPCTSTR szText);
	afx_msg void OnClickedButtonOnekeyClose();
	afx_msg LRESULT OnCustomMessageRelaunchProcess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomMessageInsertLog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClickedButtonLoadConfig();
	afx_msg void OnClickedButtonSaveConfig();
	int m_iLogicLogLevel;
	int m_iGaoFangConfig;
	BOOL m_bAlertEnable;
	afx_msg void OnClickedCheckAlert();
	BOOL m_bEnableProtect;
	afx_msg void OnClickedCheckMonitorEnable();
	int m_nClubManagerCount;
	int m_nLogicQipaiCount;
	int m_nLogicQipaiStartId;
	CString m_strLogicQipaiServerIp;
	int m_nLogicQipaiStartPort;
	int m_nLogicQipaiLogLevel;
	BOOL m_bLogicQipaiDebugMode;
	BOOL m_bLogicQipaiRobot;
	afx_msg void OnBnClickedButtonLogicQipaiStart();
	afx_msg void OnBnClickedButtonOnekeyStartTest();
	CString m_strClubDBName;
	int m_nLogicQipaiSupportGameType;
	CString m_strDatabaseIPBk;
	CString m_strDatabasePasswordBk;
	short m_uDatabasePortBk;
	CString m_strDatabaseUserNameBk;
};
