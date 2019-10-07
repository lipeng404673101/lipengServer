#include "ActiveManager.h"
#include "LTime.h"


bool ActiveManager::Init()
{
	return true;
}

bool ActiveManager::Final()
{ 
	m_mapGameidToActive.clear();
	return true;
}

void ActiveManager::PXReloadConfig(LMsgLMG2LPXActive* msg)
{
	if(msg == NULL)			return;
	boost::recursive_mutex::scoped_lock l(m_MutexActive);
	m_mapGameidToActive.clear();		// «Âø’≈‰÷√
	std::vector<PXActive>& vecActive = msg->m_vecPXActive;
	for (Lsize i = 0; i < vecActive.size(); ++i)
	{
		PXActive& active = vecActive[i];
		boost::shared_ptr<PXActiveSet> activeSet(new PXActiveSet);

		activeSet->m_timeBeginTime = convert_string_to_time_t(active.m_strBeginTime);
		activeSet->m_timeEndTime = convert_string_to_time_t(active.m_strEndTime);
		for(auto it = active.m_vecPXAndReward.begin(); it != active.m_vecPXAndReward.end(); ++it)
		{
			activeSet->m_mapPXidToCardnums[it->m_iPXId] = it->m_iCardNum;
		}
		m_mapGameidToActive[active.m_iGameId] = activeSet;
	}
	LLOG_ERROR("ActiveManager::PXReloadConfig sets count is %d", vecActive.size());
}

//void ActiveManager::getHupaiRewardCard(Lint gameid, const Lint (&HuType)[HU_ALLTYPE], Lint& rewardCardNum, Lint& rewardHu)
//{
//	boost::recursive_mutex::scoped_lock l(m_MutexActive);
//	rewardCardNum = 0;		rewardHu = HU_NULL;
//	if(!checkGameidTimeValid(gameid))		return;
//	if(m_mapGameidToActive.size() <= 0)		return;
//	if(m_mapGameidToActive.find(gameid) == m_mapGameidToActive.end())	return;
//	getCardAndHu(m_mapGameidToActive[gameid], HuType, rewardCardNum, rewardHu);
//}

//void ActiveManager::checkAddCard(Lint gameid, LMsgS2CGameOver& over, Desk * const m_desk)
//{
//	if(!m_desk)
//	{
//		LLOG_ERROR("ActiveManager::checkAddCard m_desk is null");
//		return;
//	}
//	boost::recursive_mutex::scoped_lock l(m_MutexActive);
//	if(!checkGameidTimeValid(gameid))		return;
//	if(m_mapGameidToActive.size() <= 0)		return;
//	if(m_mapGameidToActive.find(gameid) == m_mapGameidToActive.end())		return;
//	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
//	{
//		if(over.kWin[i] == WIN_SUB_TINGCARD)	continue;
//		if(getCardAndHu(m_mapGameidToActive[gameid], over.m_hu(i), over.kRewardCardNum[i], over.kRewardHu[i]))
//		{
//			m_desk->HanderAddCardCount(i, over.kRewardCardNum[i], CARDS_OPER_TYPE_ACTIVE, "system_paixing");
//		}
//	}
//}

bool ActiveManager::checkGameidTimeValid(Lint gameid)
{
	boost::recursive_mutex::scoped_lock l(m_MutexActive);
	if(m_mapGameidToActive.size() <= 0)		return false;
	if(m_mapGameidToActive.find(gameid) != m_mapGameidToActive.end())
	{
		boost::shared_ptr<PXActiveSet>& active = m_mapGameidToActive[gameid];
		if(!active)			return false;
		return checkTimeValid(active->m_timeBeginTime, active->m_timeEndTime);
	}
	return false;
}

//bool ActiveManager::getCardAndHu(boost::shared_ptr<PXActiveSet>& active, const Lint (&HuType)[HU_ALLTYPE], Lint& rewardCardNum, Lint& rewardHu)
//{
//	bool	retReward = false;
//	if(!active)			return retReward;
//	rewardCardNum = 0;		rewardHu = HU_NULL;
//	for (Lint i = 0; i < HU_ALLTYPE; ++i)
//	{
//		if(HuType[i] != 0 && active->m_mapPXidToCardnums.find(i) != active->m_mapPXidToCardnums.end())
//		{
//			if(rewardCardNum < active->m_mapPXidToCardnums[i])
//			{
//				retReward = true;
//				rewardCardNum = active->m_mapPXidToCardnums[i];
//				rewardHu = i;
//			}
//		}
//	}
//	return retReward;
//}
//
//bool ActiveManager::getCardAndHu(boost::shared_ptr<PXActiveSet>& active, std::vector<Lint>& vecHu, Lint& rewardCardNum, Lint& rewardHu)
//{
//	bool	retReward = false;
//	if(!active)			return retReward;
//	rewardCardNum = 0;		rewardHu = HU_NULL;
//	for (Lsize i = 0; i < vecHu.size(); ++i)
//	{
//		if(active->m_mapPXidToCardnums.find(vecHu[i]) != active->m_mapPXidToCardnums.end())
//		{
//			if(rewardCardNum < active->m_mapPXidToCardnums[vecHu[i]])
//			{
//				retReward = true;
//				rewardCardNum = active->m_mapPXidToCardnums[vecHu[i]];
//				rewardHu = vecHu[i];
//			}
//		}
//	}
//	return retReward;
//}