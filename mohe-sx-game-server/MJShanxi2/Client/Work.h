#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "Card.h"

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);

	LTime&			GetCurTime();
	
public:
	//处理http消息 
	//void			HanderHttp(LMsgHttp* msg);	
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);

	//处理玩家登陆的消息 
	void			HanderUserLogin(LMsgS2CLogin* msg);
	//处理玩家物品消息 
	void			HanderUserItem(LMsgS2CItemInfo* msg);

	void			HanderUserCreateRoom(LMsgS2CCreateDeskRet* msg);

	void			HanderUserAddRoom(LMsgS2CAddDeskRet* msg);

	void			HanderUserIntoDesk(LMsgS2CIntoDesk* msg);

	void			HanderUserAddDesk(LMsgS2CDeskAddUser* msg);

	//广播游戏开始
	void			HanderUserPlayStart(LMsgS2CPlayStart* msg);
	//广播玩家摸牌
	void			HanderUserGetCard(LMsgS2COutCard* msg);
	//服务器广播玩家出牌
	void			HanderUserPlayerCard(LMsgS2CUserPlay* msg);
	//服务器广播玩家思考
	void			HanderUserThink(LMsgS2CThink* msg);
	//服务器广播操作结果
	void			HanderUserOper(LMsgS2CUserOper* msg);
	//服务器广播游戏结束
	void			HanderGameOver(LMsgS2CGameOver* msg);
	//重连
	void			HanderReconnect(LMsgS2CDeskState* msg);
	// 解算房间
	void			HanderReset(LMsgS2CResetDesk* msg);

	//处理玩家登陆center的消息 
	void			HanderServerMsg(LMsgS2CMsg* msg);

	void			SendCreateRoom();

	//出牌
	void			OutCard();

private:
	
	//center发送玩家请求登录
	void			HanderCenterUserLogin(LMsgCe2LUserLogin*msg);
	//center服务器关闭
	void			HanderCenterLogout();


private:
	void			ConnectCenterServer();
	
	void			HanderLoginRet();

private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次

	Llong		m_200MSceTick;//200毫秒循环一次

	Llong		m_1SceTick;//1秒循环一次

	Llong		m_30SceTick;//30秒循环一次

	Llong		m_600SceTick;//5分钟秒循环一次


	Lint		m_state;

private:
	LSocketPtr	m_clientSocket;//

	std::map<Lint, GateInfo*> m_gateInfo;

	CardVector	m_handCard;
	CardVector	m_pengCard;
	CardVector	m_gangCard;
	CardVector	m_agangCard;

	Lint		m_myPos;
};


#define gWork Work::Instance()

#endif