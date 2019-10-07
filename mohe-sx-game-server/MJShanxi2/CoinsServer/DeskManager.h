#ifndef _DESK_MANAGER_H
#define _DESK_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "Desk.h"
#include "User.h"
#include "Robot.h"

#define FREE_DESK_MAX 256	// 最大闲置桌子的数量
#define FREE_DESK_REQ 128	// 一次申请桌子的数量

class DeskManager:public LSingleton<DeskManager>, public LRunnable
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	virtual void	Clear() {}
	virtual void	Run();
	// 每帧更新
	void	Tick();

	// 1秒更新
	void	TickOneSecond();

	// 5秒更新
	void	TickFiveSecond();

public:
	// 所有的桌子号的数量
	int GetALLDeskIDCount();

	// 添加金币桌子
	void	AddFreeDeskID( GameType type, const std::vector<CoinsDesk>& deskid );

	void	ClearDesk();

	// 给玩家分配桌子 返回运行时桌子ID
	// 这个消息也会相关的广播消息
	Lint	UserEnterDesk( Lint userid, GameType type, const std::vector<Lint>& playtype, Lint robotnum, CardValue cardvlaue[CARD_COUNT] );
	bool	UserLeaveDesk( Lint userid );
	bool	UserReady( Lint deskrunid, Lint userid );
	void	GameOver( Lint deskid, Lint score[4], const LBuffPtr& resultmsg );
	void	DismissPlayDesk( Lint deskid );

	// 玩家重新进桌
	bool	UserReEnterDesk( Lint userid );

	// 广播玩家金币
	void	CastUserCoins( Lint userid );

private:
	CoinsDesk _getCoinsDesk( GameType gameType );
	void	_initTickTime();
	LTime		m_tickTimer;
	Llong		m_1SceTick;//30秒循环一次
	Llong		m_5SceTick;//15秒循环一次

	boost::recursive_mutex		m_mutexDesk;
	std::map< GameType, std::queue<CoinsDesk> > m_freeDeskList;	// 金币桌子列表

	Lint				m_LastRequestTime;	// 标记是否正在申请桌子倒计时 防止在短时间内多次申请

	// deskrunid : DeskPtr
	std::map< Lint, DeskPtr > m_allDeskList;	// 所有的桌子列表

	std::list< DeskPtr > m_deskWaitList;	// 分配的桌子 还没有桌子号
	
	// deskid : DeskPtr
	std::map< Lint, DeskPtr > m_deskPlayList;	// 正在打牌的桌子列表 占用了桌子号
};

#define gDeskManager DeskManager::Instance()

#endif