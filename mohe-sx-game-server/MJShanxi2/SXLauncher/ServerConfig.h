#pragma once
#ifndef  _CONFIG_HEADER
#define  _CONFIG_HEADER

struct LogicManagerConfig
{
	CString Name;
	int ID;
	CString IP;
	int Port;
	int Type; //0:16210 1: club server
	LogicManagerConfig()
	{
		ID = 0;
		Port = 0;
		Type = 0;
	}
	bool IsValid()
	{
		return (Port != 0 && ID != 0);
	}
};

struct LogicServerConfig
{
	int ID; //ID
	CString IP;
	int Port;
	int nType; ;//0:Âé½«£¬1£¬ÆåÅÆ
	// ServerID
	int Magager_id;
};

struct GateServerConfig
{
	int ID; //ID
	CString IP;
	int Port;
	int Type; //0:ÎÞ 1:°¢Àï 2£ºÔÆÂþ
	// ServerID
	int Magager_id;
};


class ServerConfigHelper
{
public:
	ServerConfigHelper();
	~ServerConfigHelper();
	static CString GetGateType(int type);
	static ServerConfigHelper & GetInstance();

	std::vector<LogicManagerConfig> m_managers;
	std::vector<LogicServerConfig> m_logics;
	std::vector<GateServerConfig> m_gates;
public:
	CString m_strConfigFilePath;
	inline void SetConfigFilePath(CString config_path) { m_strConfigFilePath = config_path; }
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
		return WritePrivateProfileString(szSectionName, szKey, szText, m_strConfigFilePath);
	}
	inline BOOL WriteIniInt(LPCTSTR szSectionName, LPCTSTR szKey, int value)
	{
		CString strContent;
		strContent.Format(L"%d", value);
		return WriteIniString(szSectionName, szKey, strContent);
	}
	inline CString GetIniString(LPCTSTR szSectionName, LPCTSTR szKey, LPCTSTR szDefault)
	{
		TCHAR buf[512] = { 0 };
		if (GetPrivateProfileString(szSectionName, szKey, szDefault, buf, 512, m_strConfigFilePath))
		{
			return CString(buf);
		}
		return CString();
	}
	inline int GetIniInt(LPCTSTR szSectionName, LPCTSTR szKey, int defaultValue)
	{
		return (int)GetPrivateProfileInt(szSectionName, szKey, defaultValue, m_strConfigFilePath);
	}

	void SplitStringByChar(CString &src, TCHAR ch, std::vector<CString>& strings);
	const LogicManagerConfig& GetMappedLogicManagerByGateId(int gateId);
	const LogicManagerConfig& GetMappedLogicManagerByLogicId(int logicId);
};
 
#endif // ! _CONFIG_HEADER

