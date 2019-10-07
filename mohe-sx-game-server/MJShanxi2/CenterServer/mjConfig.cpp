
#include "mjConfig.h"
#include "config.h"
#include "DbServerManager.h"
#include "Work.h"

mjConfig::mjConfig()
{
	m_dbsession = NULL;
}
bool mjConfig::Init()
{
	if (m_dbsession)
	{
		delete m_dbsession;
	}
	m_dbsession = new LDBSession;
	if (gWork.GetDBMode())
	{
		m_dbsession->Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort());
	}
	else
	{
		m_dbsession->Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort());	 
	}
	//m_dbsession->Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort());
	this->LoadCommonConfigDataFromDB();
	this->LoadDeskFeeDataFromDB();
	return true;
}

bool mjConfig::Final()
{
	if (m_dbsession)
	{
		delete m_dbsession;
		m_dbsession = NULL;
	}
	return true;
}

bool   mjConfig::LoadDeskFeeDataFromDB()
{
	MYSQL* pMySQL = NULL;
	if (m_dbsession)
	{
		pMySQL = m_dbsession->GetMysql();
	}

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		return false;
	}

	Lint sumAround = 0;
	std::stringstream ss;

	ss << "SELECT Id, DeskType, Round, Cost, Even,roomCost FROM desk";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_DEBUG("mjConfig::LoadDeskFeeDataFromDB sql error %s", mysql_error(pMySQL));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		deskFeeItem* pDeskFeeItem = new deskFeeItem();
		if (pDeskFeeItem == NULL)
			return false;
		pDeskFeeItem->m_id = atoi(*row++);
		pDeskFeeItem->m_name = *row++;
		pDeskFeeItem->m_round = atoi(*row++);
		pDeskFeeItem->m_cost = atoi(*row++);
		pDeskFeeItem->m_even = atoi(*row++);
		pDeskFeeItem->m_roomCost = atoi(*row++);
		auto itor = m_feeList.find(pDeskFeeItem->m_id);
		if (itor != m_feeList.end()) {
			deskFeeItem* tempItem = itor->second;
			tempItem->m_id = pDeskFeeItem->m_id;
			tempItem->m_name = pDeskFeeItem->m_name;
			tempItem->m_round = pDeskFeeItem->m_round;
			tempItem->m_cost = pDeskFeeItem->m_cost;
			tempItem->m_even = pDeskFeeItem->m_even;
			tempItem->m_roomCost = pDeskFeeItem->m_roomCost;
			delete pDeskFeeItem;
		}
		else {

			m_feeList.insert(std::map<int, deskFeeItem*>::value_type(pDeskFeeItem->m_id, pDeskFeeItem));
		}
		row = mysql_fetch_row(res);
	}
	mysql_free_result(res);
	return true;

}
////////////////////////////
bool  mjConfig::LoadCommonConfigDataFromDB()
{
	MYSQL* pMySQL = NULL;
	if (m_dbsession)
	{
		pMySQL = m_dbsession->GetMysql();
	}

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		return false;
	}

	Lint sumAround = 0;
	std::stringstream ss;

	ss << "SELECT Id, Name, Value FROM commconfig";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_DEBUG("mjConfig::LoadCommonConfigDataFromDB sql error %s", mysql_error(pMySQL));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		commonItem* pCommonItem = new commonItem();
		if (pCommonItem == NULL)
			return false;
		pCommonItem->m_id = atoi(*row++);
		pCommonItem->m_name = *row++;
		pCommonItem->m_value = atoi(*row++);
		LLOG_DEBUG("LoadCommonConfigDataFromDB m_id = %d , m_name = %s ,m_value =  %d ", pCommonItem->m_id, pCommonItem->m_name.c_str(), pCommonItem->m_value);
		auto itor = m_configList.find(pCommonItem->m_name);
		if (itor != m_configList.end()) {
			commonItem* tempItem = itor->second;
			tempItem->m_id = pCommonItem->m_id;
			tempItem->m_name = pCommonItem->m_name;
			tempItem->m_value = pCommonItem->m_value;
			delete pCommonItem;
		}
		else {
			m_configList.insert(std::map<std::string, commonItem*>::value_type(pCommonItem->m_name, pCommonItem));
		}
		row = mysql_fetch_row(res);
	}
	mysql_free_result(res);
	return true;
}

bool  mjConfig::CreatToLogicConfigMessage(LMsgCE2LMGConfig& Config)
{
	auto itor1 = gMjConfig.m_configList.begin();
	while (itor1 != gMjConfig.m_configList.end()) {
		commonItem tempItem;
		tempItem.m_id = itor1->second->m_id;
		tempItem.m_name = itor1->second->m_name;
		tempItem.m_value = itor1->second->m_value;
		Config.m_commonConList.push_back(tempItem);
		itor1++;
	}
	Config.m_commonConLength = Config.m_commonConList.size();

	auto itor2 = gMjConfig.m_feeList.begin();
	while (itor2 != gMjConfig.m_feeList.end()) {
		deskFeeItem tempItem;
		tempItem.m_id = itor2->second->m_id;
		tempItem.m_name = itor2->second->m_name;
		tempItem.m_round = itor2->second->m_round;
		tempItem.m_cost = itor2->second->m_cost;
		tempItem.m_even = itor2->second->m_even;
		tempItem.m_roomCost = itor2->second->m_roomCost;
		Config.m_deskFeeList.push_back(tempItem);
		itor2++;
	}
	Config.m_deskConLength = Config.m_deskFeeList.size();
	return true;
}
bool   mjConfig::GetCommonConfigValue(std::string name, Lint& nValue)
{
	auto itor = m_configList.begin();
	itor = m_configList.find(name);
	if (itor != m_configList.end()) {
		nValue = itor->second->m_value;
		return true;
	}
	return false;
}

bool   mjConfig::GetFeeTypeValue(Lint id, deskFeeItem* feeItem)
{
	auto itor = m_feeList.begin();
	itor = m_feeList.find(id);
	if (itor != m_feeList.end()) {
		feeItem = itor->second;
		return true;
	}
	return false;
}
