#include "SpecActive.h"
#include "LDBSession.h"
#include "Config.h"
#include "LLog.h"
#include "LTime.h"
#include "Work.h"


SpecActive::SpecActive()
{
	
}

SpecActive::~SpecActive()
{
	
}

bool SpecActive::Init()
{
	loadData();
	return true;
}

bool SpecActive::Final()
{
	return true;
}


bool SpecActive::loadData()
{
	LDBSession dbSession;
	if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session  SpecActive::loadData");
		return false ;
	}

	std::stringstream sql;
	sql << "SELECT id,activeType,gameType,gameState,limitFlag,beginTime,endTime,otherInt1,otherInt2,otherInt3,otherInt4,otherInt5,otherData1,otherData2 FROM spec_active";

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("SpecActive::loadData sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("SpecActive::loadData  Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("SpecActive::loadData  Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	while (row)
	{
		boost::shared_ptr<SpecActiveItem> specItem(new  SpecActiveItem());

		specItem->m_id = atoi(*row++);
		specItem->m_activeType = atoi(*row++);
		specItem->m_gameType = atoi(*row++);
		specItem->m_gameState = atoi(*row++);
		specItem->m_limitFlag = atoi(*row++);
		specItem->m_beginTime = atoi(*row++);
		specItem->m_endTime = atoi(*row++);
		specItem->m_otherInt1= atoi(*row++);
		specItem->m_otherInt2 = atoi(*row++);
		specItem->m_otherInt3 = atoi(*row++);
		specItem->m_otherInt4 = atoi(*row++);
		specItem->m_otherInt5 = atoi(*row++);
		specItem->m_otherData1 = *row++;
		specItem->m_otherData2 = *row++;
		//LLOG_ERROR("OTHERDATA1 [%s][%s]", specItem->m_otherData1.c_str(), specItem->m_otherData2.c_str());
		addSpecActiveItem(specItem);
		
		row = mysql_fetch_row(res);
	}

	mysql_free_result(res);

	return true;
}


void SpecActive::addSpecActiveItem(boost::shared_ptr<SpecActiveItem> specItem)
{
	if (specItem == NULL)return;

	//LLOG_ERROR("SpecActive::loadActiveInfo: id[%d] active_type[%d] gameType[%d] game_state[%d] limitFlag[%d] beginTime[%d] endTime[%d] otherInt1[%d] otherInt2[%d] otherInt3[%d] otherInt4[%d] otherInt5[%d] otherData1[%s] otherData2[%s]", specItem->m_id, specItem->m_activeType, specItem->m_gameType, specItem->m_gameState, specItem->m_limitFlag, specItem->m_beginTime, specItem->m_endTime, specItem->m_otherInt1, specItem->m_otherInt2, specItem->m_otherInt3, specItem->m_otherInt4, specItem->m_otherInt5, specItem->m_otherData1.c_str(), specItem->m_otherData2.c_str());


	auto ItSpecItem = m_mapId2SpecActive.find(specItem->m_id);
	if (ItSpecItem != m_mapId2SpecActive.end())
	{
		ItSpecItem->second->update(specItem);
	}
	else
	{
		m_mapId2SpecActive[specItem->m_id] = specItem;
	}

}

void SpecActive::sendToManager(Lint manageId,Lint id )
{
	LMsgLMG2LSpecActive  send;
	if (id == 0)
	{
		for (auto ItSpecItem = m_mapId2SpecActive.begin(); ItSpecItem != m_mapId2SpecActive.end(); ItSpecItem++)
		{
			//LLOG_ERROR("SpecActive::sendToManager: id[%d] active_type[%d] gameType[%d] game_state[%d] limitFlag[%d] beginTime[%d] endTime[%d] otherInt1[%d] otherInt2[%d] otherInt3[%d] otherInt4[%d] otherInt5[%d] otherData1[%s] otherData2[%s]", ItSpecItem->second->m_id, ItSpecItem->second->m_activeType, ItSpecItem->second->m_gameType, ItSpecItem->second->m_gameState, ItSpecItem->second->m_limitFlag, ItSpecItem->second->m_beginTime, ItSpecItem->second->m_endTime, ItSpecItem->second->m_otherInt1, ItSpecItem->second->m_otherInt2, ItSpecItem->second->m_otherInt3, ItSpecItem->second->m_otherInt4, ItSpecItem->second->m_otherInt5, ItSpecItem->second->m_otherData1.c_str(), ItSpecItem->second->m_otherData2.c_str());

			SpecActiveItem specItem(ItSpecItem->second);
			send.m_ActiveList.push_back(specItem);
		}
	}
	else
	{
		auto ItSpecItem = m_mapId2SpecActive.find(id);
		if (ItSpecItem != m_mapId2SpecActive.end())
		{
			SpecActiveItem specItem(ItSpecItem->second);
			send.m_ActiveList.push_back(specItem);
		}
	}


	LLOG_ERROR("SpecActive  size[%d]",send.m_ActiveList.size());
	gWork.SendMsgToLogic(send, manageId);

}