#include "SpecActive.h"
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
	Init_paixing();
	return true;
}

bool SpecActive::Final()
{
	return true;
}

void SpecActive::addSpecActive(const LMsgLMG2LSpecActive* specList)
{
	if (specList == NULL)return;
	for (auto ItSpecItem = specList->m_ActiveList.begin(); ItSpecItem != specList->m_ActiveList.end(); ItSpecItem++)
	{
		//LLOG_ERROR("SpecActive::loadActiveInfo: id[%d] active_type[%d] gameType[%d] game_state[%d] limitFlag[%d] beginTime[%d] endTime[%d] otherInt1[%d] otherInt2[%d] otherInt3[%d] otherInt4[%d] otherInt5[%d] otherData1[%s] otherData2[%s]", ItSpecItem->m_id, ItSpecItem->m_activeType, ItSpecItem->m_gameType, ItSpecItem->m_gameState, ItSpecItem->m_limitFlag, ItSpecItem->m_beginTime, ItSpecItem->m_endTime, ItSpecItem->m_otherInt1, ItSpecItem->m_otherInt2, ItSpecItem->m_otherInt3, ItSpecItem->m_otherInt4, ItSpecItem->m_otherInt5, ItSpecItem->m_otherData1.c_str(), ItSpecItem->m_otherData2.c_str());

		boost::shared_ptr<SpecActiveItem> specItem(new  SpecActiveItem(*ItSpecItem));
		

		addSpecActiveItem(specItem);
	}
	LLOG_DEBUG("SpecActive::addSpecActive  size[%d], load size[%d]",specList->m_ActiveList.size(),m_mapId2SpecActive.size());

	sendToAllLogic();
}

void SpecActive::addSpecActiveItem(boost::shared_ptr<SpecActiveItem> specItem)
{
	if (specItem == NULL)
	{
		LLOG_ERROR("SpecActive::addSpecActiveItem specItem == NULL");
		return;
	}
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

void SpecActive::sendToLogic(Lint logicId,Lint id )
{
	LMsgLMG2LSpecActive  send;
	if (id == 0)
	{
		for (auto ItSpecItem = m_mapId2SpecActive.begin(); ItSpecItem != m_mapId2SpecActive.end(); ItSpecItem++)
		{
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
	gWork.SendMessageToLogic(logicId,send );

}

void SpecActive::sendToAllLogic(Lint id)
{
	LMsgLMG2LSpecActive  send;
	if (id == 0)
	{
		for (auto ItSpecItem = m_mapId2SpecActive.begin(); ItSpecItem != m_mapId2SpecActive.end(); ItSpecItem++)
		{
			//LLOG_ERROR("SpecActive::loadActiveInfo: id[%d] active_type[%d] gameType[%d] game_state[%d] limitFlag[%d] beginTime[%d] endTime[%d] otherInt1[%d] otherInt2[%d] otherInt3[%d] otherInt4[%d] otherInt5[%d] otherData1[%s] otherData2[%s]", ItSpecItem->second->m_id, ItSpecItem->second->m_activeType, ItSpecItem->second->m_gameType, ItSpecItem->second->m_gameState, ItSpecItem->second->m_limitFlag, ItSpecItem->second->m_beginTime, ItSpecItem->second->m_endTime, ItSpecItem->second->m_otherInt1, ItSpecItem->second->m_otherInt2, ItSpecItem->second->m_otherInt3, ItSpecItem->second->m_otherInt4, ItSpecItem->second->m_otherInt5, ItSpecItem->second->m_otherData1.c_str(), ItSpecItem->second->m_otherData2.c_str());
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
	LLOG_ERROR("SpecActive  size[%d]", send.m_ActiveList.size());
	gWork.SendMessageToAllLogic(send);

}
//////////////////////////////////////////////////////////////////////////////////////
//特殊牌型活动
void SpecActive::Init_paixing()
{
	for (Lint i = 0; i < m_paiXingNo.size(); i++)
	{
		m_mapId2PaiXing[m_paiXingNo[i]] = m_paiXingName[i];
	}

	LLOG_DEBUG("SpecActive::Init() size[%d] ", m_mapId2PaiXing.size());
	for (auto It = m_mapId2PaiXing.begin(); It != m_mapId2PaiXing.end(); It++)
	{
		LLOG_DEBUG("SpecActive::Init() paixing[%s] ", It->second.c_str());
	}
}

Lstring SpecActive::getMessage(Lint userId, Lstring nike, Lint mtime, std::vector<Lint> paixing)
{
	char buf[128] = { 0 };
	time_t t  = time(NULL); //获取目前秒时间  
	tm * local = localtime(&t); //转为本地时间  
	strftime(buf, 64, "  %Y-%m-%d %H:%M:%S ", local);

	std::ostringstream ss;
	ss << nike << " [";
	ss << userId << "]";
	ss << buf;
	ss << " 打出 ";
	
	for (auto ItPaixing = paixing.begin(); ItPaixing != paixing.end(); ItPaixing++)
	{
		auto ItPaiXingName = m_mapId2PaiXing.find(*ItPaixing);
		if (ItPaiXingName != m_mapId2PaiXing.end())
		{
			ss << ItPaiXingName->second;
		}
		else
		{
			ss << *ItPaixing;
		}
	}

	return ss.str();

}
