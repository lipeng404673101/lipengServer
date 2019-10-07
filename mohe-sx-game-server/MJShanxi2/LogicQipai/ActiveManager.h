#ifndef _ACTIVE_MANAGER_LOGIC_H_
#define _ACTIVE_MANAGER_LOGIC_H_

#include "LMsgS2S.h"
#include "LMsgL2C.h"
#include "LSingleton.h"
#include "Desk.h"


struct PXActiveSet
{
	time_t		m_timeBeginTime;
	time_t		m_timeEndTime;
	std::map<Lint, Lint> m_mapPXidToCardnums;

	PXActiveSet()
	{
		m_timeBeginTime = 0;
		m_timeEndTime = 0;
	}
};

class ActiveManager : public LSingleton<ActiveManager>
{
public:
	virtual	bool		Init();
	virtual	bool		Final();

	void				PXReloadConfig(LMsgLMG2LPXActive* msg);		
	//void				getHupaiRewardCard(Lint gameid, const Lint (&HuType)[HU_ALLTYPE], Lint& rewardCardNum, Lint& rewardHu);
	void				checkAddCard(Lint gameid, LMsgS2CGameOver& over, Desk * const m_desk);

private:
	bool				checkGameidTimeValid(Lint gameid);
	//bool				getCardAndHu(boost::shared_ptr<PXActiveSet>& active, const Lint (&HuType)[HU_ALLTYPE], Lint& rewardCardNum, Lint& rewardHu);
	//bool				getCardAndHu(boost::shared_ptr<PXActiveSet>& active, std::vector<Lint>& vecHu, Lint& rewardCardNum, Lint& rewardHu);

private:
	boost::recursive_mutex m_MutexActive;
	std::map<Lint, boost::shared_ptr<PXActiveSet> > m_mapGameidToActive;
};


#define gLogicActiveManager ActiveManager::Instance()

#endif