#ifndef _L_CHARGE_H_
#define _L_CHARGE_H_

#include "LBuff.h"
#include "LLog.h"

//房卡类型
enum CARD_TYPE
{
	CARD_TYPE_4 = 1,//4局卡
	CARD_TYPE_8 = 2,//8局卡
	CARD_TYPE_16 = 3,//16局卡
	CARD_TYPE_Other = 100,   //数量
};

// 如果扣房卡的配置
// LogicServer LogicManager CenterServer 的 DelCardCount 都调用这里
inline int GetCardDelCount(int cardType, int count) {
	switch (cardType) {
	case CARD_TYPE_4: return 1;
	case CARD_TYPE_8: return 2;
	case CARD_TYPE_16: return 4;
	case CARD_TYPE_Other: return count;
	default: return 0;
	}
}
inline int DelCardCount(int balance, int cardType, int count) {
	int delCount = GetCardDelCount(cardType, count);
	LLOG_DEBUG("User::DelCardCount  delCount = %d", delCount);
	if (balance > delCount) balance -= delCount;
	else balance = 0;
	return balance;
}

enum CARDS_OPER_TYPE
{
	CARDS_OPER_TYPE_INIT = 0,//新手赠送
	CARDS_OPER_TYPE_CHARGE = 1,//充值活动
	CARDS_OPER_TYPE_FREE_GIVE = 2,//免费赠送
	CARDS_OPER_TYPE_CREATE_ROOM = 3,//创建房间扣除
	CARDS_OPER_TYPE_ACTIVE = 4,		//活动送房卡
	CARDS_OPER_TYPE_INVITED = 5,    //邀请好友
	CARDS_OPER_TYPE_EXCHANGE = 6,   //换金币
	CARDS_OPER_TYPE_ACTIVE_DRAW = 7,//转盘消耗
};

enum COINS_OPER_TYPE
{
	COINS_OPER_TYPE_DEBUG = -1,//新手赠送
	COINS_OPER_TYPE_CHARGE = 0,//充值
	COINS_OPER_TYPE_FREE_GIVE = 1,//免费赠送
	COINS_OPER_TYPE_RESULT = 2,//房间结算
	COINS_OPER_TYPE_SELF_GIVE = 3,	//玩家自己领取
	COINS_OPER_TYPE_CHANGE_CARD = 4,   //换卡
};

class API_LBASESX LCharge
{
public:
	LCharge();
	~LCharge();

	void	Read(LBuff& buff);
	void	Write(LBuff& buff);
public:
	Lint	m_time;
	Lint	m_cardType;
	Lint	m_cardNum;
	Lint	m_type;
	Lint	m_userId;
	Lstring	m_admin;
};
#endif