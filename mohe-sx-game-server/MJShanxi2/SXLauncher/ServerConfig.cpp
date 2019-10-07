#include "stdafx.h"
#include "ServerConfig.h"

ServerConfigHelper gConfigHelper;

LogicManagerConfig default_manager_Config;

ServerConfigHelper::ServerConfigHelper()
{

}


ServerConfigHelper::~ServerConfigHelper()
{
}

ServerConfigHelper & ServerConfigHelper::GetInstance()
{
	return gConfigHelper;
}

CString ServerConfigHelper::GetGateType(int type)
{
	CString strText = 0;
	switch (type)
	{
	case 0:
		strText = L"ÎÞ";
		break;
	case 1:
		strText = L"°¢Àï";
		break;
	case 2:
		strText = L"ÔÆÂþ";
		break;
	default:
		strText = L"Î´Öª";
		break;
	}
	return strText;
}

void ServerConfigHelper::SplitStringByChar(CString &src, TCHAR ch, std::vector<CString> & strings)
{
	int nStartPos = 0;
	strings.clear();
	do
	{
		int pos = src.Find(ch, nStartPos);
		if (pos > -1)
		{
		  CString strResult = src.Mid(nStartPos, pos - nStartPos);
		  strings.push_back(strResult.Trim());
		  nStartPos = pos + 1;
		}
		else
		{
			break;
		}
	} while (1);	
}

const LogicManagerConfig& ServerConfigHelper::GetMappedLogicManagerByGateId(int gateId)
{

	for (int j = 0; j < m_gates.size(); j++)
	{
		if (m_gates[j].ID == gateId)
		{
			for (int i = 0; i < m_managers.size(); i++)
			{
				if (m_managers[i].ID == m_gates[j].Magager_id)
				{
					return m_managers[i];
				}
			}
		}
	}

	for (int i = 0; i < m_managers.size(); i++)
	{
		if (m_managers[i].ID == 16210)
		{
			return m_managers[i];
		}
	}
	return default_manager_Config;
}
const LogicManagerConfig& ServerConfigHelper::GetMappedLogicManagerByLogicId(int logicId)
{

	for (int j = 0; j < m_logics.size(); j++)
	{
		if (m_logics[j].ID == logicId)
		{
			for (int i = 0; i < m_managers.size(); i++)
			{
				if (m_managers[i].ID == m_logics[j].Magager_id)
				{
					return m_managers[i];
				}
			}
		}
	}

	for (int i = 0; i < m_managers.size(); i++)
	{
		if (m_managers[i].ID == 16210)
		{
			return m_managers[i];
		}
	}
	
	return default_manager_Config;
}
