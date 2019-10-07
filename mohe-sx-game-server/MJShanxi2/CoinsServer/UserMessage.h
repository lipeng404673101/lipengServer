#ifndef _USER_MESSAGE_H_DB
#define _USER_MESSAGE_H_DB

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "LDBSession.h"
#include "LTime.h"

class CUserMessage : public LRunnable
{
public:
	CUserMessage();
	virtual ~CUserMessage();
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void Clear();
	virtual void Run();

	virtual void Start();
	virtual	void Stop();
	virtual	void Join();
protected:
	void	HanderMsg(LMsg* msg);

	// 玩家加入
	void	HanderEnterCoinDesk(LMsgLMG2CNEnterCoinDesk* msg);

	void	HanderUserMsg(LMsgG2LUserMsg* msg);

	// From Gate
	// 用户掉线
	void	HanderUserOutMsg(LMsgG2LUserOutMsg* msg);

	//玩家退出房间
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg, const std::string& uuid, Lint gateid);

	//玩家请求通知内容
	void	HanderUserReady(LMsgC2SUserReady* msg, const std::string& uuid);

	//申请解算房间
	void	HanderResetDesk(LMsgC2SResetDesk* msg, const std::string& uuid);

	// 结算继续游戏
	void	HanderGoOnCoinsDesk(LMsgC2SGoOnCoinsDesk* msg, const std::string& uuid, Lint gateid);

	// 玩家领取金币
	void	HanderGetCoins(LMsgLMG2CNGetCoins* msg);
	void	HanderGetCoins(LMsgC2SGetCoins* msg, const std::string& uuid, Lint gateid);
	void	HanderGetCoins(Lint userid, const std::string& uuid, Lint gateid);


	// From Mangager
	// 用户登陆
	void	HanderUserLogin(LMsgLMG2CNUserLogin* msg);

	// From LogicServer
	void	HanderModifyUserState(LMsgL2LMGModifyUserState* msg);

private:
	// 玩家离开
	void	_userOut( Lint userid );

	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;

};

#endif