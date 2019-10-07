#include "ActiveManager.h"
#include "LTabFile.h"
#include "LTime.h"
#include "Work.h"
#include "LBase.h"

bool ActiveManager::Init()
{
	m_PXActiveMsg.m_vecPXActive.clear();
	return true;
}

bool ActiveManager::Final()
{
	m_PXActiveMsg.m_vecPXActive.clear();
	return true;
}

std::map<Lint,Lint>& ActiveManager::GetExchRule()
{
	return m_ExchActiveSet.m_mapCard2Gold;
}

Lstring ActiveManager::GetExchRuleInfo()
{
	return m_strExchRule;
}

bool ActiveManager::CheckTimeValid_Exch()
{
	return checkTimeValid(m_ExchActiveSet.m_timeBeginTime,m_ExchActiveSet.m_timeEndTime);
}

bool ActiveManager::CheckHasExch()
{
	LTime curTime;
	time_t curSecond = curTime.Secs();
	return curSecond < m_ExchActiveSet.m_timeEndTime;
}

void ActiveManager::ExchReloadConfig( const Lstring& strActiveSet )
{
	boost::mutex::scoped_lock l(m_ExchMutexActive);
	m_ExchActiveSet.m_timeBeginTime = 0;
	m_ExchActiveSet.m_timeEndTime = 0;
	m_ExchActiveSet.m_mapCard2Gold.clear();
	if(strActiveSet.empty())
	{
		LLOG_ERROR("ActiveManager::ExchReloadConfig strActiveSet is empty, already clear now set!");
		return;
	}

	Json::Reader reader(Json::Features::strictMode());
	Json::Value value;
	try
	{
		if (reader.parse(strActiveSet, value))
		{
			time_t stime = value["StartTime"].asUInt();
			time_t etime = value["EndTime"].asUInt();

			if(!checkTimeValid(stime, etime))
			{
				LLOG_ERROR("ActiveManager::ExchReloadConfig Time InValid !");
				return;
			}

			m_strExchRule = strActiveSet;
			m_ExchActiveSet.m_timeBeginTime = stime;
			m_ExchActiveSet.m_timeEndTime = etime;

			Lint card = 0;
			for(size_t i = 0; i < value["Exchange"].size(); ++i)
			{
				if(i % 2 == 1)
				{
					Lint gold = value["Exchange"][i].asInt();
					m_ExchActiveSet.m_mapCard2Gold[card] = gold;
				}
				else
				{
					card = value["Exchange"][i].asInt();
				}
			}
		}
	}
	catch (...)
	{
		LLOG_ERROR("exc exception -- json parse error!!!");
	}
		
}

/****************** 老玩家登录送房卡活动*********************/
void ActiveManager::OUGCReloadConfig(const Lstring& strActiveSet)
{
 	boost::mutex::scoped_lock l(m_OUGCMutexActive);
	m_OUGCActiveSet.m_timeBeginTime = 0;			// 清空配置
	m_OUGCActiveSet.m_timeEndTime = 0;
	m_OUGCActiveSet.m_mapDaysToCardnums.clear();
	if(strActiveSet.empty())
	{
		LLOG_ERROR("ActiveManager::OUGCReloadConfig strActiveSet is empty, already clear now set!");
		return;
	}

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	if (!reader.parse(strActiveSet, value))
	{
		LLOG_ERROR("ActiveManager::OUGCReloadConfig parsr json error %s", strActiveSet.c_str());
		return;
	}

	if(value["BeginTime"].isNull() || value["EndTime"].isNull() || value["ActiveSet"].isNull())
	{
		LLOG_ERROR("ActiveManager::OUGCReloadConfig has null item BeginTime=%s, Endtime=%s, OUGCSet=%s", 
			value["BeginTime"].asString().c_str(), value["EndTime"].asString().c_str(), value["OUGCSet"].asString().c_str());
		return;
	}

	if(!value["BeginTime"].isString() || !value["EndTime"].isString() || !value["ActiveSet"].isString())
	{
		LLOG_ERROR("ActiveManager::OUGCReloadConfig has item value not a string %s", strActiveSet.c_str());
		return;
	}

	m_OUGCActiveSet.m_timeBeginTime = convert_string_to_time_t(value["BeginTime"].asString().c_str());
	m_OUGCActiveSet.m_timeEndTime = convert_string_to_time_t(value["EndTime"].asString().c_str());

	std::vector<Lstring> vecDaysAndCards;
	L_ParseString(value["ActiveSet"].asString(), vecDaysAndCards, "|");
	for(Lsize i = 0; i < vecDaysAndCards.size(); ++i)
	{
		std::vector<Lstring> vecData;
		L_ParseString(vecDaysAndCards[i], vecData, "_");
		if(vecData.size() != 2)
		{
			LLOG_ERROR("ActiveManager::OUGCReloadConfig is wrong. %s", vecDaysAndCards[i].c_str());
			continue;
		}
		m_OUGCActiveSet.m_mapDaysToCardnums[atoi(vecData[0].c_str())] = atoi(vecData[1].c_str());
	}
	LLOG_ERROR("ActiveManager::OUGCReloadConfig set %s", strActiveSet.c_str());
}

bool ActiveManager::OUGCChekGiveCard(Lint lastLoginTime, Lint& iGiveCardNums, Lint& iLoginIntervalDays)
{
 	boost::mutex::scoped_lock l(m_OUGCMutexActive);
	iGiveCardNums = 0;		iLoginIntervalDays = 0;
	if(!checkTimeValid(m_OUGCActiveSet.m_timeBeginTime, m_OUGCActiveSet.m_timeEndTime))
	{
		return false;
	}

	LTime curTime;
	Lint iIntervalTime = curTime.Secs() - lastLoginTime;		// 距离上次登录时间的秒数
	
	// 逆序查找
	for (auto it = m_OUGCActiveSet.m_mapDaysToCardnums.crbegin(); it != m_OUGCActiveSet.m_mapDaysToCardnums.crend(); ++it)
	{
		if(iIntervalTime >= (it->first * 24 * 60 * 60))
		{
			iGiveCardNums = it->second;
			iLoginIntervalDays = it->first;
			return true;
		}
	}
	return false;
}


/*********************** 牌型活动****************************/

void ActiveManager::PXReloadConfig(const Lstring& strActiveSet)
{
	boost::mutex::scoped_lock l(m_PXMutexActive);
	m_PXActiveMsg.m_vecPXActive.clear();				// 清空当前配置
	if(strActiveSet.empty())
	{
		LLOG_ERROR("ActiveManager::PXReloadConfig strActiveSet is empty, already clear now set!");
		return;
	}

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	if (!reader.parse(strActiveSet, value))
	{
		LLOG_ERROR("ActiveManager::PXReloadConfig parsr json error %s", strActiveSet.c_str());
		return;
	}

	if(value["ItemCount"].isNull())
	{
		LLOG_ERROR("ActiveManager::PXReloadConfig ItemCount is null");
		return;
	}

	if(!value["ItemCount"].isString())
	{
		LLOG_ERROR("ActiveManager::PXReloadConfig ItemCount value not a string");
		return;
	}

	const Lint ItemCount = atoi(value["ItemCount"].asString().c_str());
	char gametype[255] = { 0 }, pxaward[255] = { 0 }, 
		begintime[255] = { 0 }, endtime[255] = { 0 };
	for(Lint i = 0; i < ItemCount; ++i)
	{
		sprintf(gametype, "GameType%d", i+1);		sprintf(pxaward, "PXAward%d", i+1);
		sprintf(begintime, "BeginTime%d", i+1);		sprintf(endtime, "EndTime%d", i+1);

		if(value[gametype].isNull() || value[pxaward].isNull() || value[begintime].isNull() || value[endtime].isNull())
		{
			LLOG_ERROR("ActiveManager::PXReloadConfig has null item column GameType=%s, PXAward=%s, BeginTime=%s, EndTime=%s, itemnum=%d",
				value[gametype].asString().c_str(),	value[pxaward].asString().c_str(),
				value[begintime].asString().c_str(),value[endtime].asString().c_str(),i+1);
			continue;
		}

		if(!value[gametype].isString() || !value[pxaward].isString() || !value[begintime].isString() || !value[endtime].isString())
		{
			LLOG_ERROR("ActiveManager::PXReloadConfig has item value not a string %s", strActiveSet.c_str());
			return;
		}

		if(checkActiveTimeSetVaild(value[begintime].asString(), value[endtime].asString()))
		{
			Lint gameid = atoi(value[gametype].asString().c_str());
	
			PXActive active;
			active.m_iGameId = gameid;
			std::vector<Lstring> vecIdAndCards;
			L_ParseString(value[pxaward].asString(), vecIdAndCards, "|");
			for(Lsize i = 0; i < vecIdAndCards.size(); ++i)
			{
				std::vector<Lstring> vecData;
				L_ParseString(vecIdAndCards[i], vecData, "_");
				if(vecData.size() != 2)
				{
					LLOG_ERROR("ActiveManager::PXReloadConfig is wrong. %s", vecIdAndCards[i].c_str());
					continue;
				}
				active.m_vecPXAndReward.emplace_back(PXActiveItem(atoi(vecData[0].c_str()), atoi(vecData[1].c_str())));
			}

			active.m_strBeginTime = value[begintime].asString().c_str();
			active.m_strEndTime = value[endtime].asString().c_str();
			m_PXActiveMsg.m_vecPXActive.push_back(active);
		}
		else
		{
			LLOG_ERROR("ActiveManager::PXReloadConfig is wrong time item=%d begintime=%s, endtime=%s",
				i+1, value[begintime].asString().c_str(), value[endtime].asString().c_str());
		}
	}
	LMsgLMG2LPXActive msg;
	PXSetMsgContent(msg);
	gWork.SendMessageToAllLogic(msg);
	LLOG_ERROR("ActiveManager::PXReloadConfig set %s", strActiveSet.c_str());
	LLOG_ERROR("ActiveManager::PXReloadConfig send msg count = %d to every logic ", msg.m_vecPXActive.size());
}

void ActiveManager::PXSendConfig(const Lint LogicId)
{
	boost::mutex::scoped_lock l(m_PXMutexActive);
	LMsgLMG2LPXActive msg;
	PXSetMsgContent(msg);
	gWork.SendMessageToLogic(LogicId, msg);
}

void ActiveManager::PXSetMsgContent(LMsgLMG2LPXActive& msg)
{
	for(Lsize i = 0; i < m_PXActiveMsg.m_vecPXActive.size(); ++i)
	{
		msg.m_vecPXActive.push_back(m_PXActiveMsg.m_vecPXActive[i]);
	}
}
