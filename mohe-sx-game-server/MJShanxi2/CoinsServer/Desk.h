#ifndef _DESK_H_
#define _DESK_H_

#include "LBase.h"
#include "GameDefine.h"
#include "LTime.h"
#include "LVideo.h"
#include "LMsg.h"
#include "User.h"

struct DeskUser
{
	DeskUser( Lint userid, Lint pos )
		: m_userID( userid )
		, m_pos(pos)
		, m_bReady( false )
	{
	}
	//bool operator = ( const DeskUser& o ) const { return m_userID == o.m_userID; }

	Lint m_userID;
	Lint m_pos;
	bool m_bReady;
	LTime m_time;	// 状态时间 加入桌子 结算时重置
};

struct Desk
{
	Desk( Lint runid, GameType type );

	Lint m_runID;				// 桌子运行时ID
	GameType m_gameType;
	CoinsDesk m_coinDesk;		// 分配桌子号后有效
	Lint m_deskMaxPlayer;		// 最多允许的人数
	std::list<DeskUser> m_users;	// 已经加入桌子的玩家

	// 玩法相关
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
	CardValue			m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;			//玩法

	// 检查游戏桌子是否可以开始
	bool CheckGameStart();

	bool OnUserInRoom( UserPtr& user );
	bool OnUserOutRoom( UserPtr& user, bool bremove );	// bremove 表示是否从m_users中删掉
	bool OnUserReady( UserPtr& user );
	void OnGameOver();

	void CheckReadyOutTime();
	// 强制解散桌子
	void DismissDesk();

	bool OnUserReInRoom( UserPtr& user );	// 玩家重新进桌
};

typedef boost::shared_ptr<Desk> DeskPtr;

#endif