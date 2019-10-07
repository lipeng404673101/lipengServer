#pragma once
#include <sstream>
#include <string>
#include "LBase.h"
#include "LVideo.h"
#include "LMsg.h"
#include "..\LogicServer\Card.h"

const Lint TimeSpace_Login			= 10;
const Lint TimeSpace_Reconnect		= 60;
const Lint TimeSpace_HeartbeatTime	= 5;
const Lint TimeSpace_Ready			= 0;
struct TimeSpace
{
	int Test;
	int	Login;
	int	HeartbeatTime;
	int	DeskOpTime;
	int	OutCard;
	int	OutThink;
	int Wait;
	void Init()
	{
		memset(this,0,sizeof(TimeSpace));
	}
};
#define TimeSpaceCheck(A,B)	if (time(NULL)< ((A) + (B)))\
								return;\
							A=time(NULL);


const Lint RoomAddRobot		= 0;
const Lint HardCardCount	= 14;


enum User_State
{
	User_State_Connect_Nothing = 0,	// 初始状态;
	User_State_Connect_Center,		// 连接Center;
	User_State_Connect_Center_Wait,	// 
	User_State_Connect_Gete,		// 连接Gate;
	User_State_Connect_Gete_Wait,	//
	User_State_Hall,				// 大厅中;
	User_State_Game_GoReady,		// 游戏等待继续中;
	User_State_Game_Wait,			// 游戏中 - 等待;
	User_State_Game_Get_Card,		// 游戏中 - 摸牌;
	User_State_Game_THINK,			// 游戏中 - 思考;
	User_State_Game_StartHu,		// 游戏中 - 起手胡;
	User_State_Game_EndSelect,		// 游戏中 - 海底牌
	User_State_Desk_Exit,			// 退桌;
};

struct User_Play_Info
{
	Lint					m_deskId;	// 桌号;
	Lint					m_pos;		// 我的Pos;
	Lint					m_ready;	// 是否准备号;
	Lint					m_CurPos;	// 当前操作的人;
	std::vector<Card>		m_cardValue;
	std::vector<ThinkData>	m_think;//思考的内容;
	User_State				m_state;
	bool					m_DeskBegin;
	bool					m_HaiDi;
	int						m_WaitCount;
	std::vector<StartHuInfo> m_startHu;	//起手胡内容
	void Init()
	{
		Init_DeskOver();
		m_state = User_State_Connect_Center;
	}
	void Init_GameOver()
	{
		m_ready = 0;
		m_CurPos = 4;
		m_cardValue.clear();
		m_think.clear();
		m_startHu.clear();
		m_state = User_State_Game_GoReady;
		m_DeskBegin = false;
		m_HaiDi = false;
	}
	void Init_DeskOver()
	{
		Init_GameOver();
		m_deskId = -1;
		m_pos    = 4;
		m_state = User_State_Hall;
	}
};

struct DeskInfo
{
	Lint Index;
	std::vector<Lstring> UserLise;
};
