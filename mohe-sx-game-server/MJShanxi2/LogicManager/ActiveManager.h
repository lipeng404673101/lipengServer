#ifndef _ACTIVE_MANAGER_H
#define _ACTIVE_MANAGER_H

#include "LSingleton.h"
#include "LMsgS2S.h"
#define MAX_GIFT_NUM	(12)

enum	GIFTTYPE
{
	GT_NONE = 0,
	GT_ITEM = 1,
	GT_PHONECARD = 2,//话费
	GT_ROOMCARD = 3,//房卡
};
struct DrawActiveGift
{
	Lstring giftname;
	Lint gifttype;
	Lint giftcount;
	Lint giftpercent;
	Lint gifttotalcount;
	Lint giftsurpluscount;
	DrawActiveGift()
	{
		gifttype = 0;
		giftcount = 0;
		giftpercent = 0;
		gifttotalcount = 0;
		giftsurpluscount = 0;
	}
};
struct DrawActiveInfo
{
	Lint	mActiveID;
	Lstring	mActiveName;
	Lstring	mActiveIntroduction;
	Lstring	mBeginTime;					// 时间格式为: 年月日  如 20160506;
	Lstring	mEndTime;
	Lint	mRewardNum;
	DrawActiveGift	mGift[MAX_GIFT_NUM];
	Lint	mAllPercent;
};

// 抽奖返回;
struct DrawActiveRun_RetGift
{
	Lint	giftID;
	Lint	Gift_type;	
	Lint	Gift_count;
	DrawActiveRun_RetGift()
	{
		giftID = 0;
		Gift_type = 0;
		Gift_count = 0;
	}
};
class ActiveManager:public LSingleton<ActiveManager>
{
public:
	virtual	bool		Init();
	virtual	bool		Final();

	/***************** 老玩家登录送房卡活动*********************/
	struct OUGCActiveSet
	{
		time_t		m_timeBeginTime;
		time_t		m_timeEndTime;
		std::map<Lint, Lint> m_mapDaysToCardnums;		// 距离上次登录天数 to 奖励房卡数

		OUGCActiveSet()
		{
			m_timeBeginTime = 0;
			m_timeEndTime = 0;
		}
	};

public:
	void				OUGCReloadConfig(const Lstring& strActiveSet);		// 老玩家登录送房卡活动
	bool				OUGCChekGiveCard(Lint lastLoginTime, Lint& iGiveCardNums, Lint& iLoginIntervalDays);
private:
	boost::mutex m_OUGCMutexActive;
	OUGCActiveSet m_OUGCActiveSet;

	/*********************** 牌型活动****************************/
public:
	void				PXReloadConfig(const Lstring& strActiveSet);		// 牌型活动
	void				PXSendConfig(const Lint LogicId);
	void				PXSetMsgContent(LMsgLMG2LPXActive& msg);
private:
	boost::mutex m_PXMutexActive;
	LMsgLMG2LPXActive m_PXActiveMsg;
	
	/*********************** 金币换房卡活动****************************/

	struct ExchActiveSet
	{
		time_t		m_timeBeginTime;
		time_t		m_timeEndTime;
		std::map<Lint, Lint> m_mapCard2Gold;		// 距离上次登录天数 to 奖励房卡数

		ExchActiveSet()
		{
			m_timeBeginTime = 0;
			m_timeEndTime = 0;
		}
	};

public:
	std::map<Lint,Lint>& GetExchRule();
	Lstring         GetExchRuleInfo();
	bool            CheckTimeValid_Exch();
	bool            CheckHasExch();
	void            ExchReloadConfig( const Lstring& strActiveSet );
private:
	boost::mutex m_ExchMutexActive;
	ExchActiveSet m_ExchActiveSet;
	Lstring       m_strExchRule;


};

#define gActiveManager ActiveManager::Instance()



#endif

