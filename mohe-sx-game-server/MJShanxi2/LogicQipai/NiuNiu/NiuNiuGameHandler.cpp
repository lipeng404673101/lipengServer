#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "NiuNiuGameLogic.h"
#include "LMsgL2C.h"



static int LoadPlayerCards(std::vector<BYTE> & cards, int playerIndex)
{
	std::string fileName("card.ini");
	LIniConfig config;
	config.LoadFile("card.ini");

	char szKey[32] = { 0 };
	if (playerIndex > 0 && playerIndex < 10)
	{
		sprintf(szKey, "player%dcards", playerIndex);
	}

	std::string cardsString = config.GetString(Lstring(szKey), "");
	std::vector<std::string> cardsStringVec = splitString(cardsString, ",", true);
	if (cardsStringVec.size() > 0)
	{
		cards.clear();
		for (auto it = cardsStringVec.begin(); it < cardsStringVec.end(); it++)
		{
			BYTE nValue = atoi((*it).c_str());
			MHLOG("Card: %d  %s", nValue, (*it).c_str());
			cards.push_back(nValue);
		}
	}

	return 0;
}


/* 庄家模式（枚举）*/
enum GAME_ZHUANG_MODE
{
	ZHUANG_MODE_LUN = 0,    //轮庄
							//ZHUANG_MODE_RUND = 1,   //随机庄
							ZHUANG_MODE_QIANG = 1,  //看牌抢庄
};

/* 玩法模式（枚举）*/
enum GAME_PLAY_MODE
{
	PLAY_MODE_COMMON_1 = 0,  //普通模式1
	PLAY_MODE_COMMON_2 = 1,  //普通模式2
	PLAY_MODE_BOMB = 2,      //扫雷模式
	PLAY_MODE_MINGSCORE = 3, //明牌下注
	PLAY_MODE_ANSCORE = 4,   //暗牌下注
	PLAY_MODE_X1 = 5,		 //新添加倍数1
};

/* 游戏中的各个阶段（枚举） */
enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = -1,
	GAME_PLAY_SELECT_ZHUANG,
	GAME_PLAY_ADD_SCORE,
	GAME_PLAY_SEND_CARD,
	GAME_PLAY_END,
};

/* 各个阶段的超时时间（枚举） */
enum PLAY_STATUS_DELAY_TIME
{
	//DELAY_TIME_SELECT_ZHUANG = 10,               //选庄
	////DELAY_TIME_SELECT_QIANG_ZHUANG = 30,  //抢庄
	////DELAY_TIME_ADD_SCORE = 30,                      //下注
	////DELAY_TIME_OPEN_CARD = 20,                      //亮牌
	//DELAY_TIME_SELECT_QIANG_ZHUANG = 15,  //抢庄
	//DELAY_TIME_ADD_SCORE = 15,                      //下注
	//DELAY_TIME_OPEN_CARD = 10,                      //亮牌

	DELAY_TIME_SELECT_ZHUANG = 5,               //选庄
	DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU = 7,  //抢庄
	DELAY_TIME_ADD_SCORE = 5,                      //下注
	DELAY_TIME_OPEN_CARD = 4,                      //亮牌
};

/* 牛牛游戏小选项(结构体) */
struct PlayType
{
protected:
	std::vector<Lint>	m_playtype;  //游戏小选项
									 //Index ------- 0: 选庄模式 1:扫雷模式 2:托管模式 3:花样玩法 4:暗抢庄

public:
	/* 结构体清0方法 */
	void clear()
	{
		m_playtype.clear();
	}
	/* 设置玩法小选项 */
	void setAllTypes(const std::vector<Lint>& v)
	{
		m_playtype = v;
	}
	/* 获取所有小选项 */
	const std::vector<Lint>& getAllTypes() const
	{
		return m_playtype;
	}

	/******************************************************
	* 函数名：   NiuNiuGetZhuangMode()
	* 描述：     获取客户端定义的庄家规则
	* 详细：     庄家规则：0-轮流庄、1、看牌抢庄
	* 返回：     (Lint)客户端选择的庄家规则
	********************************************************/
	Lint NiuNiuGetZhuangMode() const;

	/******************************************************
	* 函数名：   NiuNiuGetPlayMode()
	* 描述：     获取客户端选择的玩法模式
	* 详细：     玩法模式：0-普通模式1，1-普通模式2，2-扫雷模式，3-明牌下注，4-暗牌下注
	* 返回：     (Lint)客户端选择的玩法模式
	********************************************************/
	Lint NiuNiuGetPlayMode() const;

	/******************************************************
	* 函数名：   NiuNiuGetSpecPlay()
	* 描述：     获取客户端定义的花样玩法模式
	* 详细：     花样模式：false-不开启，true-开启
	* 返回：     (BOOL)客户端选择的花样模式
	********************************************************/
	bool NiuNiuGetSpecPlay() const;

	/******************************************************
	* 函数名：   NiuNiuGetAutoPlay()
	* 描述：     获取客户端超时自动操作
	* 详细：     扫雷模式：false-不开启，true-开启
	* 返回：     (bool)客户端选择超时自动操作
	********************************************************/
	bool NiuNiuGetAutoPlay() const;

	/******************************************************
	* 函数名：   NiuNiuGetDynamicIn()
	* 描述：     获取客户端是否允许动态加入选项
	* 详细：     暗抢庄：false-不予许，true-允许
	* 返回：     (bool)是否动态加入
	********************************************************/
	bool NiuNiuGetDynamicIn() const;

	/******************************************************
	* 函数名：   NiuNiuGetPlayerNum()
	* 描述：     获取客户端人数支持：6人或10人
	* 返回：     (int)总人数
	********************************************************/
	Lint NiuNiuGetPlayerNum() const;

	/******************************************************
	* 函数名：   NiuNiuQiangZhuangTimes()
	* 描述：     获取牛牛抢庄倍率（轮庄倍率为1）
	* 详细：     抢庄倍率：1-1倍、2-2倍、3-3倍、4-4倍
	* 返回：     (Lint)选择的抢庄倍率
	********************************************************/
	Lint NiuNiuQiangZhuangTimes() const;

	/******************************************************
	* 函数名：   NiuNiuAddScoreTimes()
	* 描述：     获取牛牛下注倍率（）
	* 详细：     抢庄倍率：0-小倍、1-中倍、2-大倍
	* 返回：     (Lint)选择的下注倍率
	********************************************************/
	Lint NiuNiuAddScoreTimes() const;

	/******************************************************
	* 函数名：	NiuNiuTypeTimes()
	* 描述：	牛牛新增牌形倍数牛牛X4
	* 详细：	牛七-牛八：2倍，牛九：3倍，牛牛：4倍...
	* 返回：	(int) 不选次选项返回：0， 选择此选项返回 1
	*******************************************************/
	Lint NiuNiuTypeTimes() const;

	/******************************************************
	* 函数名：	NiuNiuTuiZhu()
	* 描述：	牛牛推注选项
	* 详细：	推注倍数： 0：不推注，5：5倍推注，10：10倍推注
	* 返回：	(Lint)推注倍数
	*******************************************************/
	Lint NiuNiuTuiZhu() const;
};



/******************************************************
* 函数名：   NiuNiuGetZhuangMode()
* 描述：     获取客户端定义的庄家规则
* 详细：     庄家规则：0-轮流庄、1-看牌抢庄
* 返回：     (Lint)客户端选择的庄家定义
********************************************************/
Lint PlayType::NiuNiuGetZhuangMode() const
{
	if (m_playtype.empty())
	{
		return 0;
	}
	return m_playtype[0];
}

/******************************************************
* 函数名：   NiuNiuGetPlayMode()
* 描述：     获取客户端选择的玩法模式
* 详细：     玩法模式：0-普通模式1，1-普通模式2，2-扫雷模式，3-明牌下注，4-暗牌下注
* 返回：     (Lint)客户端选择的玩法模式
********************************************************/
Lint PlayType::NiuNiuGetPlayMode() const
{
	if (m_playtype.size() < 2)
	{
		//看牌抢庄，默认普通模式1
		if (m_playtype[0] == 1) return 0;

		//轮流坐庄，默认暗牌下注
		else if (m_playtype[0] == 0) return 4;
	}

	////看牌抢庄玩法下
	//if (m_playtype[0] == 1 && (m_playtype[1] < 0 || m_playtype[1] > 2))
	//{
	//	return 0;
	//}
	////轮庄玩法下
	//else if(m_playtype[0] == 0 && (m_playtype[1] < 2 || m_playtype[1] > 4))
	//{
	//	return 4;
	//}
	return m_playtype[1];
}

/******************************************************
* 函数名：   NiuNiuGetZhuangMode()
* 描述：     获取客户端定义的花样玩法模式
* 详细：     花样模式：false-不开启，true-开启
* 返回：     (bool)客户端选择的花样模式
********************************************************/
bool PlayType::NiuNiuGetSpecPlay() const
{
	if (m_playtype.size() < 3)
	{
		return false;
	}
	return m_playtype[2] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiuGetAutoPlay()
* 描述：     获取客户端超时自动操作
* 详细：     扫雷模式：false-不开启，true-开启
* 返回：     (bool)客户端选择超时自动操作
********************************************************/
bool PlayType::NiuNiuGetAutoPlay() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}
	return m_playtype[3] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiuGetDynamicIn()
* 描述：     获取客户端是否允许动态加入选项
* 详细：     暗抢庄：false-不予许，true-允许
* 返回：     (Lint)是否动态加入
********************************************************/
bool PlayType::NiuNiuGetDynamicIn() const
{
	if (m_playtype.size() < 5)
	{
		return false;
	}
	return m_playtype[4] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiuGetPlayerNum()
* 描述：     获取客户端人数支持：6人或10人
* 返回：     (Lint)是否动态加入
********************************************************/
Lint PlayType::NiuNiuGetPlayerNum() const
{
	if (m_playtype.size() < 6)
	{
		return 6;
	}
	if (m_playtype[5] != 6 || m_playtype[5] != 10)
	{
		return 6;
	}
	return m_playtype[5];
}

/******************************************************
* 函数名：   NiuNiuQiangZhuangTimes()
* 描述：     获取牛牛抢庄倍率（轮庄倍率为1）
* 详细：     抢庄倍率：1-1倍、2-2倍、3-3倍、4-4倍
* 返回：     (Lint)选择的抢庄倍率
********************************************************/
Lint PlayType::NiuNiuQiangZhuangTimes() const
{
	if (m_playtype.size() < 7)
	{
		return 1;
	}
	//轮庄规则下抢庄倍率为1倍
	if (m_playtype[6] < 0 || m_playtype[6] > 3 || 0 == m_playtype[0])
	{
		return 1;
	}
	return m_playtype[6] + 1;
}

/******************************************************
* 函数名：   NiuNiuAddScoreTimes()
* 描述：     获取牛牛下注倍率（）
* 详细：     抢庄倍率：0-小倍、1-中倍、2-大倍
* 返回：     (Lint)选择的下注倍率
********************************************************/
Lint PlayType::NiuNiuAddScoreTimes() const
{
	if (m_playtype.size() < 8)
	{
		return 0;
	}
	//普通模式1，扫雷模式下只有小倍
	if (m_playtype[7] < 0 || m_playtype[7] > 2)
	{
		return 0;
	}
	return m_playtype[7];
}

/******************************************************
* 函数名：	NiuNiuTypeTimes()
* 描述：	牛牛新增牌形倍数牛牛X4
* 详细：	牛七-牛八：2倍，牛九：3倍，牛牛：4倍...
* 返回：	(int) 不选次选项返回：0， 选择此选项返回 1
*******************************************************/
Lint PlayType::NiuNiuTypeTimes() const
{
	if (m_playtype.size() < 9)
	{
		return 0;
	}
	return m_playtype[8];
}

/******************************************************
* 函数名：	NiuNiuTuiZhu()
* 描述：	牛牛推注选项
* 详细：	推注倍数： 0：不推注，5：5倍推注，10：10倍推注
* 返回：	(Lint)推注倍数
*******************************************************/
Lint PlayType::NiuNiuTuiZhu() const
{
	if (m_playtype.size() < 10)
	{
		return 0;
	}
	return m_playtype[9];
}


/* 牛牛每局不会初始化字段（结构体）*/
struct NiuNiuRoundState__c_part
{
	Lint		 m_curPos;						                    // 当前操作玩家
	Lint         m_play_status;                                     // 牌局状态
	BOOL         m_tick_flag;                                       // 定时器开关
	Lint		 m_player_tui_score[NIUNIU_PLAY_USER_COUNT][4];		// 玩家可以推注的分数

	Lint         m_user_status[NIUNIU_PLAY_USER_COUNT];             // 用户状态：改位置上是否坐人了（实际有效位置）
	Lint         m_play_add_score[NIUNIU_PLAY_USER_COUNT];          // 玩家下注分数
	Lint         m_play_qiang_zhuang[NIUNIU_PLAY_USER_COUNT];       // 玩家抢庄分数： 默认：-1，无效位置未操作， 0，不抢庄， >0抢庄分数
	bool         m_isOpenCard[NIUNIU_PLAY_USER_COUNT];              // 玩家是否开牌：false-未开牌，true-已开牌,初始为：false
	BYTE         m_player_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];   //用户手牌																			 //玩家本局输赢分数
	Lint         m_player_score[NIUNIU_PLAY_USER_COUNT];            // 玩家本局得分
	Lint         m_player_oxnum[NIUNIU_PLAY_USER_COUNT];            // 玩家牛数

																	//BYTE         m_cbDynamicJoin[NIUNIU_PLAY_USER_COUNT];           // 动态加入
																	//BYTE         m_cbLeaveGame[NIUNIU_PLAY_USER_COUNT];             // 中途离场
																	//BYTE		 m_cbPlayStatus[NIUNIU_PLAY_USER_COUNT];			// 游戏状态
																	//BYTE		 m_cbCallStatus[NIUNIU_PLAY_USER_COUNT];			// 叫庄状态
	BYTE		 m_cbOxCard[NIUNIU_PLAY_USER_COUNT];				// 牛牛数据
																	//BYTE		 m_cbFirest[NIUNIU_PLAY_USER_COUNT];				// 选无牛次

																	/* 清零每局字段 */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));

		//某些变量需要赋特殊初始值
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			m_play_qiang_zhuang[i] = -1;
			m_player_oxnum[i] = -1;
			m_isOpenCard[i] = false;
		}
	}
};

/* 牛牛每局会初始化字段（结构体）*/
struct NiuNiuRoundState__cxx_part
{
	LTime m_play_status_time;
	/* 清零每局字段 */
	void clear_round()
	{
		this->~NiuNiuRoundState__cxx_part();
		new (this) NiuNiuRoundState__cxx_part;
	}
};

/* 牛牛每场不会初始化的字段（结构体）*/
struct NiuNiuMatchState__c_part
{
	Desk*         m_desk;           // 桌子对象
	Lint          m_round_offset;   // 当前局数
	Lint          m_round_limit;    // 创建房间选择的最大局数
	Lint          m_accum_score[NIUNIU_PLAY_USER_COUNT];
	BOOL          m_dismissed;
	int           m_registered_game_type;
	int           m_player_count;   // 玩法核心代码所使用的玩家数量字段
	Lint          m_zhuangPos;      // 庄家位置
	bool          m_isFirstFlag;    // 首局显示开始按钮,true代表首局
	bool		  m_canTuiZhu[NIUNIU_PLAY_USER_COUNT];  // 是否可以推注 0 不可以推注，1可以推注
														/////玩法小选项
	Lint          m_selectZhuangMode;      // 小选项：选庄规则：0-轮庄，1-看牌抢庄
	Lint          m_playMode;              // 小选项：玩法模式：0-普通模式1（看抢），1-普通模式2（看抢），2-扫雷模式（通用），3-明牌下注（轮庄），4-暗牌下注（轮庄）
	BOOL          m_isSpacePlay;           // 小选项：是否开启花样玩法
	BOOL          m_isAutoPlay;            // 小选项：是否允许托管
	BOOL          m_isDynamicIn;           // 小选项：是否支持动态加入
	Lint          m_playerNum;             // 小选项：人数6人或10人
	Lint          m_qiangZhuangTimes;      // 小选项：抢庄倍率：1-1倍，2-2倍，3-3倍，4-4倍
	Lint          m_addScoreTimes;         // 小选项：下注倍率：0-小倍，1-中倍，2-大倍
	Lint          m_scoreTimes;			   // 小选项：牌形算分倍数 0：普通倍率  1：扫雷模式倍率  2：新增牛牛X4倍率（**Ren 2018-05-17）
	Lint		  m_tuiZhuTimes;		   // 小选项：推注分数（**Ren 2018-05-19）
										   //BOOL          m_isSweepMode;           // 小选项：是否开启扫雷模式
										   //BOOL          m_isMingPaiAddScore;     // 小选项：是否明牌下注

										   /* 清零结构体字段 */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_zhuangPos = NIUNIU_INVAILD_POS;
		m_isFirstFlag = true;
		m_playMode = 4;          //默认暗牌下注
		m_qiangZhuangTimes = 1;  //默认抢庄倍率1倍
		m_playerNum = 6;
	}
};

/* 牛牛每场会初始化的字段（结构体）*/
struct NiuNiuMatchState__cxx_part
{
	NGameLogic     m_gamelogic;     // 游戏逻辑
	PlayType       m_playtype;	    // 桌子玩法小选项

									/* 清空每场结构体 */
	void clear_match()
	{
		this->~NiuNiuMatchState__cxx_part();
		new (this) NiuNiuMatchState__cxx_part;
	}
};

/*
*  牛牛每局所需要的所有字段（结构体）
*  继承 ：NiuNiuRoundState__c_pard, NiuNiuRoundState_cxx_part
*/
struct NiuNiuRoundState : NiuNiuRoundState__c_part, NiuNiuRoundState__cxx_part
{
	void clear_round()
	{
		NiuNiuRoundState__c_part::clear_round();
		NiuNiuRoundState__cxx_part::clear_round();
	}
};

/*
*  牛牛每场所需要的所有字段（结构体）
*  继承：NiuNiuMatchState__c_pard, NiuNiuMatchState_cxx_pard
*/
struct NiuNiuMatchState : NiuNiuMatchState__c_part, NiuNiuMatchState__cxx_part
{
	void clear_match()
	{
		NiuNiuMatchState__c_part::clear_match();
		NiuNiuMatchState__cxx_part::clear_match();
	}
};

/*
*  牛牛桌子状态（结构体）
*  继承：NiuNiuRoundState, NiuNiuMatchState
*/
struct NiuNiuDeskState : NiuNiuRoundState, NiuNiuMatchState
{

	/* 清零每局数据 */
	void clear_round()
	{
		NiuNiuRoundState::clear_round();
	}

	/* 清零每场数据*/
	void clear_match(int player_count)
	{
		NiuNiuMatchState::clear_match();
		this->clear_round();
		m_player_count = player_count;
	}

	/******************************************************************
	* 函数名：    setup_match()
	* 描述：      设置本场的局数，以及玩法小选项
	* 参数：
	*  @ l_playtype 玩法小选项
	*  @ round_limit 本场最大局数
	* 返回：       (void)
	*******************************************************************/
	void setup_match(std::vector<Lint>& l_playtype, int round_limit)
	{
		//设置本场最大局数
		m_round_limit = round_limit;
		//设置玩法小选项
		m_playtype.setAllTypes(l_playtype);
	}

	/* 每局结束后增加局数 */
	void increase_round()
	{
		//增加局数
		++m_round_offset;
		//更新当前局数（如：2/8局）
		if (m_desk && m_desk->m_vip)
		{
			++m_desk->m_vip->m_curCircle;
		}
	}

	/* 设置桌子状态 */
	void set_desk_state(DESK_STATE v)
	{
		if (m_desk)
		{
			if (v == DESK_WAIT)
			{
				m_desk->SetDeskWait();
			}
			else
			{
				m_desk->setDeskState(v);
			}
		}
	}

	/* 获取桌子状态 */
	DESK_STATE get_desk_state()
	{
		if (m_desk)
		{
			return (DESK_STATE)m_desk->getDeskState();
		}
		else
		{
			return DESK_PLAY;
		}
	}

	/* 设置牌局中的状态 */
	void set_play_status(Lint play_status)
	{
		if (play_status >= GAME_PLAY_SELECT_ZHUANG && play_status <= GAME_PLAY_END)
			m_play_status = play_status;
	}

	/* 获取该玩家在桌子上的位置 */
	Lint GetUserPos(User* pUser)
	{
		if (NULL == pUser)
		{
			return NIUNIU_INVAILD_POS;
		}
		Lint pos = NIUNIU_INVAILD_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= NIUNIU_PLAY_USER_COUNT)
		{
			pos = NIUNIU_INVAILD_POS;
		}
		return pos;
	}

	/* 获取下一个有效位置 */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0) return 0; // 避免除零崩溃	
		Lint nextPos = (pos + 1) % m_player_count;
		for (int i = 0; (i<m_player_count) && (m_user_status[nextPos] != 1); i++)
		{
			nextPos = (nextPos + 1) % m_player_count;
		}
		return nextPos;
	}

	/* 判断位置是否为有效位置 */
	bool is_pos_valid(Lint pos)
	{
		if (NULL == m_desk)
		{
			return false;
		}
		return 0 <= pos && pos < NIUNIU_INVAILD_POS;
	}
};

/* 牛牛录像功能(结构体) */
struct NiuNiuVideoSupport : NiuNiuDeskState {
	VideoLog		m_video;

	void VideoSave()
	{
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_playType = m_desk->getPlayType();
		//m_video.m_flag=m_registered_game_type;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		gWork.SendMsgToDb(video);
	}
};

/* 牛牛游戏具体处理逻辑（结构体）*/
struct NiuNiuGameCore : GameHandler, NiuNiuVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}
	virtual void notify_lookon(LMsg &msg) {
	}

	virtual void notify_desk_playing_user(LMsg &msg) {
	}

	virtual void notify_desk_without_user(LMsg &msg, User* pUser) {
	}

	/* 广播游戏局数给桌上的所有玩家 */
	void notify_desk_match_state()
	{
		LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;
		notify_desk(msg);

		//广播给观战玩家
		notify_lookon(msg);
	}
	/*****************************************************
	* 函数名：    notify_zhuang()
	* 描述：      通知客户桌子上所有玩家庄家位置
	* 参数：
	*  @ pos      庄家位置
	* 返回：      (void)
	******************************************************/
	void notify_zhuang(Lint select_zhuang_pos, Lint score_times = 1)
	{
		if (select_zhuang_pos < 0 || select_zhuang_pos >NIUNIU_PLAY_USER_COUNT)
		{
			LLOG_ERROR("desk_id=[%d]send_zhuang_cmd pos error!!!", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}
		if (select_zhuang_pos >= 0 &&
			select_zhuang_pos < NIUNIU_PLAY_USER_COUNT &&
			m_user_status[select_zhuang_pos] == 1)
		{
			m_zhuangPos = select_zhuang_pos;
			NiuNiuS2CStartGame send_start;
			send_start.m_zhuangPos = select_zhuang_pos;
			send_start.m_scoreTimes = score_times;
			//推注：玩家可推注状态
			memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
			memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
			memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(send_start.m_qingScore));
			notify_desk(send_start);

			//广播观战玩家：游戏开始
			notify_lookon(send_start);
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	/* 计算该桌子上有多少人入座（有效位置）*/
	Lint calc_vaild_player_count()
	{
		Lint tmp_player_count = 0;
		for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i])
			{
				++tmp_player_count;
			}
		}
		return tmp_player_count;
	}

	/* 确定最小的下注分数 */
	Lint min_add_score()
	{
		//轮流坐庄 && 扫雷模式
		if (0 == m_selectZhuangMode && 2 == m_playMode)
		{
			switch (m_addScoreTimes)
			{
				//倍率：小倍（1 2 3）
			case 0:
			{
				return 1;
				break;
			}
			//倍率：大倍（4 5 6）
			case 2:
			{
				return 4;
				break;
			}
			default:
			{
				return 1;
				break;
			}
			}
		}

		//普通模式1 或 扫雷模式
		else if (m_playMode == 0 || m_playMode == 2) return 1;

		//普通模式2 或 明牌下注 或 暗牌下注
		else if (m_playMode == 1 || m_playMode == 3 || m_playMode == 4)
		{
			//倍率
			switch (m_addScoreTimes)
			{
				//倍率：小倍（2，3，4，5）
			case 0:
			{
				return 2;
				break;
			}
			//倍率：中倍（6，9，12，15）
			case 1:
			{
				return 6;
				break;
			}
			//倍率：大倍（5，10，20，30）
			case 2:
			{
				return 5;
				break;
			}
			default:
				return 2;
				break;
			}
		}

		else
		{
			return 1;
		}
	}

	/* 判断看牌选庄玩法下是否所有玩家已经对选庄操作完成 */
	bool is_select_zhuang_over()
	{
		Lint userSelectZhuang = 0;
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{

			if (m_play_qiang_zhuang[i] != -1)
			{
				++userSelectZhuang;
			}
		}

		if (userSelectZhuang == calc_vaild_player_count())
		{
			//所有玩家在规定时间内操作完成，关闭抢庄倒计时
			m_tick_flag = false;
			return true;
		}
		return false;
	}

	/* 判断是否所有玩家都下完注了 */
	bool is_add_score_over()
	{
		Lint userSelectScore = 0;
		for (int i = 0; i < m_player_count; i++)
		{
			if (m_play_add_score[i] != 0 && m_user_status[i] == 1)
			{
				++userSelectScore;
			}
		}

		if (userSelectScore == calc_vaild_player_count())
		{
			//所有玩家下注完成，设置牌局状态为发牌
			set_play_status(GAME_PLAY_SEND_CARD);
			return true;
		}

		return false;
	}

	/* 判断所有玩家是否都点开牌了 */
	bool is_open_card_over()
	{
		Lint tmp_user_open = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (true == m_isOpenCard[i])
			{
				++tmp_user_open;
			}
		}

		if (tmp_user_open == calc_vaild_player_count())
		{
			//表示所有玩家都已经选择开牌
			set_play_status(GAME_PLAY_END);
			return true;
		}
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////

	/********************************************************************
	* 函数名：      send_user_card()
	* 描述：        给玩家发牌 或 亮牌
	* 参数：
	*  @ pos 玩家位置
	*  @ show_card 是否为亮牌 默认为false
	* 返回：       (bool)
	*********************************************************************/
	bool send_user_card(Lint pos, bool show_card = false)
	{
		if (pos >= 0 && pos < m_player_count)
		{
			//亮牌
			if (show_card)
			{
				for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
				{
					if (m_user_status[i] != 1) continue;

					NiuNiuS2COpenCard send_card;
					send_card.m_pos = pos;
					//消息标记为亮牌
					send_card.m_show = 1;
					//发送每个玩家的状态
					memcpy(send_card.m_playerStatus, m_user_status, sizeof(m_user_status));
					if (i == pos)
					{
						//计算牛牛数据
						BYTE tmp_player_hand_card[NIUNIU_HAND_CARD_MAX];
						memcpy(tmp_player_hand_card, m_player_hand_card[pos], sizeof(m_player_hand_card[pos]));
						m_player_oxnum[pos] = m_gamelogic.GetCardType(tmp_player_hand_card, NIUNIU_HAND_CARD_MAX, m_isSpacePlay);
						send_card.m_oxNum = m_player_oxnum[pos];

						//如果这个位置是刚刚操作的位置，则把手牌复制给这个人
						for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
						{
							send_card.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
						}
					}
					//是该玩家操作的，将详细信息发给该玩家，不是该玩家的则只通知玩家开牌状态。
					notify_user(send_card, i);
				}

				//广播观战玩家：玩家开牌
				NiuNiuS2COpenCard gz_opCard;
				gz_opCard.m_pos = pos;
				gz_opCard.m_show = 1;
				memcpy(gz_opCard.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_lookon(gz_opCard);
			}
			//发牌
			else
			{
				NiuNiuS2COpenCard send_card;
				send_card.m_pos = pos;
				//消息标记为发牌
				send_card.m_show = 0;
				//发送每个玩家的状态
				memcpy(send_card.m_playerStatus, m_user_status, sizeof(m_user_status));
				//给该玩家发手牌
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; i++)
				{
					send_card.m_playerHandCard[i] = m_player_hand_card[pos][i];
				}
				//发牌通知玩家
				notify_user(send_card, pos);

				//广播观战玩家：玩家发牌
				NiuNiuS2COpenCard gz_sendCard;
				gz_sendCard.m_pos = pos;
				gz_sendCard.m_show = 0;
				memcpy(gz_sendCard.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_lookon(gz_sendCard);
			}
			return true;
		}
		return false;
	}

	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	//小结算
	void finish_round(BOOL jiesan = false)
	{
		LLOG_DEBUG("NiuNiuHandler::finish_round() Run... This round is finished...deskId=[%d], round=[%d/%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit);

		increase_round();
		//用户手牌副本
		BYTE tmp_player_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];
		//玩家输赢倍数
		Lint tmp_win_times[NIUNIU_PLAY_USER_COUNT];
		//玩家牛牛数，变量在每局不会自动初始化中定义了
		//Lint tmp_win_oxnum[NIUNIU_PLAY_USER_COUNT];
		//玩家输赢分数
		Lint tmp_win_count[NIUNIU_PLAY_USER_COUNT];
		//初始化数组
		memset(tmp_player_hand_card, 0x00, sizeof(tmp_player_hand_card));
		memset(tmp_win_times, 0, sizeof(tmp_win_times));
		//memset(tmp_win_oxnum, 0, sizeof(tmp_win_oxnum));
		memset(tmp_win_count, 0, sizeof(tmp_win_count));

		//将每个玩家的手牌复制一个副本做下面的计算
		CopyMemory(tmp_player_hand_card, m_player_hand_card, sizeof(m_player_hand_card));

		//庄家倍数
		tmp_win_times[m_zhuangPos] = 1;
		if (true == m_cbOxCard[m_zhuangPos])
		{
			tmp_win_times[m_zhuangPos] = m_gamelogic.GetTimes(tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX, m_isSpacePlay, m_scoreTimes);
		}
		LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[m_zhuangPos] = %d", m_desk ? m_desk->GetDeskId() : 0, tmp_win_times[m_zhuangPos]);

		//对比玩家
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] == 0) continue;

			if (m_gamelogic.CompareCard(tmp_player_hand_card[i], tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX, m_cbOxCard[i], m_cbOxCard[m_zhuangPos], m_isSpacePlay))
			{
				++tmp_win_count[i];
				//获取倍数
				tmp_win_times[i] = 1;
				if (m_cbOxCard[i] == true)
				{
					tmp_win_times[i] = m_gamelogic.GetTimes(tmp_player_hand_card[i], NIUNIU_HAND_CARD_MAX, m_isSpacePlay, m_scoreTimes);
				}
				LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[%d] = %d", m_desk ? m_desk->GetDeskId() : 0, i, tmp_win_times[i]);
			}
			else
			{
				++tmp_win_count[m_zhuangPos];
			}
		}

		//获取牛牛
		/*for (Lint i = 0; i < m_player_count; i++)
		{
		if (m_user_status[i] == 0) continue;
		tmp_win_oxnum[i] = m_gamelogic.GetCardType(tmp_player_hand_card[i], NIUNIU_HAND_CARD_MAX, m_isSpaceMode);
		}*/

		//统计玩家得分
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] == 0) continue;

			if (tmp_win_count[i] > 0)
			{
				//闲家赢
				m_player_score[i] = m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[i];

				//推注：闲家赢，且上局没有推注，则本局可以推注（**Ren 2018-5-19）
				m_canTuiZhu[i] = !m_canTuiZhu[i];

				LLOG_ERROR("desk_id = [%d] *****ceshi,Xian_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[%d] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], i, tmp_win_times[i]);
			}
			else
			{
				//庄家赢
				m_player_score[i] = (-1) * m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[m_zhuangPos];

				//推注：庄家赢了，闲家下局肯定没有推注权利
				m_canTuiZhu[i] = false;

				LLOG_ERROR("desk_id = [%d] *****ceshi,Zhuang_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[m_zhuangPos] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], tmp_win_times[m_zhuangPos]);
			}
			m_player_score[m_zhuangPos] -= m_player_score[i];


		}
		//找出本局赢家
		Lint wWinner = m_zhuangPos;
		Lint maxScore = m_player_score[m_zhuangPos];
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_player_score[i] > maxScore)
			{
				maxScore = m_player_score[i];
				wWinner = i;
			}
		}

		//发给客户端小结算
		NiuNiuS2CResult send_result;
		//一局结束写日志
		add_round_log(m_player_score, wWinner);

		if (m_desk && m_desk->m_vip)
		{
			memcpy(send_result.m_totleScore, m_desk->m_vip->m_score, sizeof(send_result.m_totleScore));
			LLOG_DEBUG("desk_id=[%d], send.m_TotalScore[%d %d %d %d %d %d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_totleScore[0],
				send_result.m_totleScore[1], send_result.m_totleScore[2],
				send_result.m_totleScore[3], send_result.m_totleScore[4],
				send_result.m_totleScore[5]);
		}
		else
		{
			LLOG_ERROR("m_desk or  m_desk->m_vip is null ");
		}
		if (true == jiesan)
		{
			set_desk_state(DESK_WAIT);
			LLOG_ERROR("NiuNiuGameHandler::finish_round() ... This has is dissolved... desk_id=[%d], desk_status=[%d], jie_san game over",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			//结束游戏
			if (m_desk) m_desk->HanderGameOver(1);
			return;
		}

		send_result.m_zhuangPos = m_zhuangPos;
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] == 0)
			{
				continue;
			}
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				send_result.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
			}
		}
		memcpy(send_result.m_playScore, m_player_score, sizeof(m_player_score));
		memcpy(send_result.m_oxNum, m_player_oxnum, sizeof(m_player_oxnum));
		memcpy(send_result.m_oxTimes, tmp_win_times, sizeof(tmp_win_times));

		LLOG_DEBUG("desk_id=[%d], GAME_END m_ZhuangPos=[%d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_zhuangPos);
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			LLOG_DEBUG("GAME_END playerPos = %d", i);
			LLOG_ERROR("DeskId=[%d],GAME_END player%dHandCard = [%d,%d,%d,%d,%d]", m_desk ? m_desk->GetDeskId() : 0, i, m_player_hand_card[i][0], m_player_hand_card[i][1], m_player_hand_card[i][2], m_player_hand_card[i][3], m_player_hand_card[i][4]);
			LLOG_DEBUG("GAME_END m_Score[%d]", m_accum_score[i]);
		}

		notify_desk(send_result);

		//广播观战玩家：小结算
		notify_lookon(send_result);

		set_desk_state(DESK_WAIT);
		//if (m_desk) m_desk->setDynamicToPlay();
		LLOG_ERROR("desk_id=[%d], desk_status=[%d], round_finish game over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		//结束游戏
		if (m_desk) m_desk->HanderGameOver(1);

	}

	///////////////////////////////////////////////////////////////////////////////

	/***************************************************
	* 函数名：    on_event_user_add_score()
	* 描述：      处理玩家下注
	* 参数：
	*  @ pos 玩家在桌子上的位置
	*  @ score 玩家下注分数
	* 返回：      (bool)
	****************************************************/
	bool on_event_user_add_score(Lint pos, Lint score)
	{
		LLOG_ERROR("desk_id=[%d], on_event_user_add_score,pos[%d],score[%d]", m_desk ? m_desk->GetDeskId() : 0, pos, score);
		if (0 == score)
		{
			LLOG_DEBUG("desk_id=[%d], on_event_user_add_score error !!!!  score==0 error pos[%d]", m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}
		m_play_add_score[pos] = score;

		//玩家下注完成后广播下注分数
		NiuNiuS2CAddScore score_notify;
		score_notify.m_pos = pos;
		score_notify.m_score = score;
		notify_desk(score_notify);

		//广播观战玩家：玩家下注
		notify_lookon(score_notify);

		if (is_add_score_over())
		{
			//所有下注完成，修改游戏状态为开牌状态
			set_play_status(GAME_PLAY_SEND_CARD);

			//打印日志：
			LLOG_ERROR("desk_id=[%d], deskStatus=[%d],clent add score over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			//所有人下注完成，给每个玩家发牌
			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1)continue;
				send_user_card(i, false);
			}

			m_play_status_time.Now();
			m_tick_flag = true;
		}
		return true;
	}


	/*********************************************************
	* 函数名：    on_event_user_select_zhuang()
	* 描述：      看牌选庄玩法下，客户端发来该玩家抢庄分数
	* 参数：
	*  @ pos 玩家位置
	*  @ zhuang_score 玩家抢庄分数
	* 返回：      (void)
	**********************************************************/
	bool on_event_user_select_zhuang(Lint pos, Lint zhuang_score)
	{
		LLOG_DEBUG("NiuNiuGameHandler::on_event_user_select_zhuang() Run... Player Qiang Zhuang... deskId=[%d], pos=[%d], qiangScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, zhuang_score);

		//记录该玩家抢庄分数; -1：无效位置，0：不抢庄，>0：抢庄分数
		m_play_qiang_zhuang[pos] = zhuang_score;

		// 通知叫庄结果 
		NiuNiuS2CQiangZhuangNotify   send;
		send.m_pos = pos;
		send.m_qiangZhuangScore = zhuang_score;
		notify_desk(send);

		//广播观战玩家
		notify_lookon(send);

		if (is_select_zhuang_over())
		{
			LLOG_DEBUG("NiuNiuGameHandler::on_event_user_select_zhuang() Run... All Player Has Qiang ZhuangOver, Zhen Go to Next Status Add Score... deskId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0);

			Lint tmp_max = m_play_qiang_zhuang[0];     //存贮暂时最大值
			Lint maxNum = 1;                           //最大抢庄分数的人数
			Lint tmp_score_max[NIUNIU_PLAY_USER_COUNT];
			ZeroMemory(tmp_score_max, sizeof(tmp_score_max));
			tmp_score_max[0] = 0;

			for (Lint i = 1; i < m_player_count; i++)
			{
				Lint ret = m_play_qiang_zhuang[i] - tmp_max;
				if (ret > 0)
				{
					tmp_max = m_play_qiang_zhuang[i];
					if (maxNum >= 1)
					{
						ZeroMemory(tmp_score_max, sizeof(tmp_score_max));
						tmp_score_max[0] = i;
						maxNum = 1;
					}
				}
				else if (ret < 0)
				{
					continue;
				}
				else
				{
					tmp_score_max[maxNum++] = i;
				}
			}

			// if 最高分有多个玩家，从中随机取一个为庄
			if (maxNum > 1)
			{
				m_zhuangPos = tmp_score_max[rand() % maxNum];
			}
			else
			{
				m_zhuangPos = tmp_score_max[0];
			}

			//解决看牌抢庄模式，用户全不抢的情况
			if (m_play_qiang_zhuang[m_zhuangPos] == 0)
			{
				m_play_qiang_zhuang[m_zhuangPos] = 1;
			}

			//记录选庄倍数
			m_play_add_score[m_zhuangPos] = m_play_qiang_zhuang[m_zhuangPos];

			//推注：庄家确定了，庄家没有推注的权利(**Ren 2018-5-19)
			m_canTuiZhu[m_zhuangPos] = false;
			memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);
			//m_player_tui_score[m_zhuangPos][0] = 0;

			//确定庄家后广播选庄结果
			notify_zhuang(m_zhuangPos, m_play_qiang_zhuang[m_zhuangPos]);

			//选庄结束，将游戏状态设置为下注
			set_play_status(GAME_PLAY_ADD_SCORE);

			//游戏开始，下注倒计时
			m_play_status_time.Now();
			m_tick_flag = true;

			return true;
		}
		return false;
	}

	/***************************************************
	* 函数名：    on_event_user_open_card()
	* 描述：      玩家点击开牌按钮
	* 参数：
	*  @ pos 玩家在桌子上的位置
	* 返回：      (bool)
	****************************************************/
	bool on_event_user_open_card(Lint pos)
	{
		LLOG_DEBUG("NiuNiuGameHandler::on_event_user_open_card() Run... Player Open Card... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		//标记该用户为已开牌
		m_isOpenCard[pos] = true;

		//广播该用户开牌结果
		send_user_card(pos, true);

		//计算该玩家牌形
		int OxNum = m_gamelogic.GetCardType(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX, m_isSpacePlay);
		if (OxNum < 0) OxNum = 0;
		m_cbOxCard[pos] = OxNum == 0 ? false : true;

		if (is_open_card_over())
		{
			//广播每个人亮牌
			//结算
			finish_round();
		}
		return true;
	}

	/* 轮庄 */
	void lun_zhuang_pos()
	{
		//轮庄玩法下，第一局需要抢庄来
		if (m_round_offset == 0)
		{
			/*if (m_desk)
			{
			m_zhuangPos = m_desk->MHSpecPersonPos();
			}*/
			for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				if (m_user_status[i] == 0)
				{
					continue;
				}
				NiuNiuS2CSelectZhuang qiang_zhuang;
				qiang_zhuang.m_pos = i;         //玩家位置
				qiang_zhuang.m_qingZhuang = 2;  //设置为轮庄第一局抢庄，客户端只显示：0-不抢，1抢庄
												//将每个玩家的手牌写入看牌选庄的消息中,每个玩家先只发4张牌
				for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1 && PLAY_MODE_MINGSCORE == m_playMode; j++)
				{
					qiang_zhuang.m_playerHandCard[j] = m_player_hand_card[i][j];
				}
				memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_user(qiang_zhuang, i);
			}

			//广播观战玩家
			NiuNiuS2CSelectZhuang gz_zhuang;
			gz_zhuang.m_pos = m_player_count;
			gz_zhuang.m_qingZhuang = 2;
			memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_lookon(gz_zhuang);

			//定时器开始计时
			m_play_status_time.Now();
			m_tick_flag = true;

			return;
		}
		else
		{
			m_zhuangPos = GetNextPos(m_zhuangPos);

			//推注：庄家确定了，庄家没有推注的权利(**Ren 2018-5-19)
			m_canTuiZhu[m_zhuangPos] = false;
			memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);

			//明牌下注模式
			if (PLAY_MODE_MINGSCORE == m_playMode)
			{
				for (Lint pos = 0; pos < NIUNIU_PLAY_USER_COUNT; pos++)
				{
					/*if (0 == m_user_status[pos])
					{
					continue;
					}
					NiuNiuS2CMingPaiAddScore send;
					send.m_pos = pos;
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
					send.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
					}
					notify_user(send, pos);*/

					if (0 == m_user_status[pos])
					{
						continue;
					}


					NiuNiuS2CStartGame send_start;
					send_start.m_zhuangPos = m_zhuangPos;
					send_start.m_scoreTimes = 1;  //非抢庄模式下默认倍数为一倍
												  //推注：玩家可推注状态
					memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
					memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
						send_start.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
					}
					memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
					notify_user(send_start, pos);
				}
				//庄家默认下注分数
				m_play_add_score[m_zhuangPos] = 1;

				//广播观战玩家
				NiuNiuS2CStartGame gz_start;
				gz_start.m_zhuangPos = m_zhuangPos;
				gz_start.m_scoreTimes = 1;  //非抢庄模式下默认倍数为一倍
											//推注：玩家可推注状态
				memcpy(gz_start.m_playerTuiScore, m_player_tui_score, sizeof(gz_start.m_playerTuiScore));
				memcpy(gz_start.m_playerStatus, m_user_status, sizeof(gz_start.m_playerStatus));
				memcpy(gz_start.m_qingScore, m_play_qiang_zhuang, sizeof(gz_start.m_qingScore));
				notify_lookon(gz_start);

			}
			//暗牌下注
			else
			{
				NiuNiuS2CStartGame send_start;
				send_start.m_zhuangPos = m_zhuangPos;
				send_start.m_scoreTimes = 1;  //非抢庄模式下默认倍数为一倍
											  //推注：玩家可推注状态
				memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
				memcpy(send_start.m_playerStatus, m_user_status, sizeof(m_user_status));
				memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
				m_play_add_score[m_zhuangPos] = 1;
				notify_desk(send_start);

				//广播观战玩家
				notify_lookon(send_start);
			}
		}

		//选庄结束，设置游戏状态为下注状态
		set_play_status(GAME_PLAY_ADD_SCORE);

		//选庄结果发送给客户端，游戏开始消息
		//NiuNiuS2CStartGame send_start;
		//send_start.m_zhuangPos = m_zhuangPos;
		//send_start.m_scoreTimes = 1;  //非抢庄模式下默认倍数为一倍
		//推注：玩家可推注状态
		//memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(m_player_tui_score));
		//memcpy(send_start.m_playerStatus, m_user_status, sizeof(m_user_status));
		//m_play_add_score[m_zhuangPos] = 1;
		//notify_desk(send_start);

		//定时器开始计时
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* 随机庄 */
	void rand_zhuang_pos()
	{
		Lint tmp_rand_zhuang_pos = 0;
		//检测是否是有效随机出来的位置是否是有效位置
		do
		{
			tmp_rand_zhuang_pos = rand() % m_player_count;
			if (m_user_status[tmp_rand_zhuang_pos])
			{
				break;
			}
		} while (true);

		//记录庄家位置
		m_zhuangPos = tmp_rand_zhuang_pos;

		//推注：庄家确定了，庄家没有推注的权利(**Ren 2018-5-19)
		m_canTuiZhu[m_zhuangPos] = false;
		memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);

		//选庄结束，设置游戏状态为下注状态
		set_play_status(GAME_PLAY_ADD_SCORE);

		//庄家比牌下分确定
		m_play_add_score[m_zhuangPos] = 1;

		//选庄结果发送给客户端，游戏开始消息
		NiuNiuS2CStartGame send_start;
		send_start.m_zhuangPos = m_zhuangPos;
		send_start.m_scoreTimes = 1;  //非看牌选庄默认为一倍
									  //推注：玩家可推注状态
		memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
		memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
		memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(send_start.m_qingScore));
		notify_desk(send_start);

		//定时器开始计时
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* 看牌抢庄 */
	void qiang_zhuang_pos()
	{
		LLOG_DEBUG("NiuNiuGameHandler::qiang_zhuang_pos() Run... This mode is QiangZhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] == 0)
			{
				continue;
			}
			//62072:通知客户端抢庄
			NiuNiuS2CSelectZhuang qiang_zhuang;
			qiang_zhuang.m_pos = i;         //玩家位置
			qiang_zhuang.m_qingZhuang = 1;  //设置为抢庄模式
											//将每个玩家的手牌写入看牌选庄的消息中,每个玩家先只发4张牌
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
			{
				qiang_zhuang.m_playerHandCard[j] = m_player_hand_card[i][j];
			}
			memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_user(qiang_zhuang, i);
		}

		//广播观战玩家
		NiuNiuS2CSelectZhuang gz_zhuang;
		gz_zhuang.m_pos = m_player_count;
		gz_zhuang.m_qingZhuang = 1;
		memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
		notify_lookon(gz_zhuang);

		LLOG_DEBUG("NiuNiuGameHandler::qiang_zhuang_pos() Run... Has Notify Client Start Qiang Zhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());

		//定时器开始计时
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* 选庄 */
	void select_zhuang()
	{
		switch (m_playtype.NiuNiuGetZhuangMode())
		{

			//轮庄
		case ZHUANG_MODE_LUN:
		{
			lun_zhuang_pos();
			break;
		}

		//随机庄
		/*case ZHUANG_MODE_RUND:
		{
		rand_zhuang_pos();
		break;
		}*/

		//看牌抢庄
		case ZHUANG_MODE_QIANG:
		{
			//通知玩家下注抢庄
			qiang_zhuang_pos();
			break;
		}
		} //switch end
	}

	/********************************************************************************************
	* 函数名：      start_game()
	* 描述：        每一局游戏开始的具体入口
	* 返回：        (void)
	*********************************************************************************************/
	void start_game()
	{
		LLOG_DEBUG("NiuNiuGameHandler::start_game() Run... Game Start... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//配置文件中是否配置允许调牌
		bool can_assign = gConfig.GetDebugModel();
		//允许调牌
		//if (can_assign)
		if (false)
		{
			std::vector<BYTE> handCard;
			handCard.resize(5);

			for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
			{
				if (m_user_status[i] != 1) continue;
				handCard.clear();
				LoadPlayerCards(handCard, i + 1);
				LLOG_DEBUG("player%dcards=[%2x,%2x,%2x,%2x,%2x]", i + 1, handCard[0], handCard[1], handCard[2], handCard[3], handCard[4]);
				//memcpy(m_player_hand_card[i], &handCard, sizeof(handCard));
				for (int j = 0; j < NIUNIU_HAND_CARD_MAX; ++j)
				{
					m_player_hand_card[i][j] = handCard[j];
				}
			}
		}
		//不允许调牌
		else
		{
			//洗牌
			m_gamelogic.RandCardList(m_player_hand_card[0], sizeof(m_player_hand_card) / sizeof(m_player_hand_card[0][0]));
		}

		//每局重置玩家可以推注的分数
		int t_tuiScore = m_tuiZhuTimes * min_add_score();
		LLOG_DEBUG("Start Game NiuNiu TuiZhu t_tuiScore=[%d]", t_tuiScore);
		for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			if (m_canTuiZhu[i])
			{
				m_player_tui_score[i][0] = t_tuiScore;
			}
			LLOG_DEBUG("Start Game NiuNiu m_plauer_tui_score[%d]=[%d | %d]", i, m_player_tui_score[i][0]);
		}

		//设置游戏状态为选庄状态
		set_play_status(GAME_PLAY_SELECT_ZHUANG);

		//选庄环节
		select_zhuang();

	}

	/*********************************************************************************************
	* 函数名：     start_round()
	* 描述：       游戏每一局的总开始函数
	* 参数：
	*  @ *player_status 玩家状态列表，表示该位置上是否有玩家，0：初始，1：该局参加，2：新加入 OR 该局不参与游戏(数组)
	* 返回：       (void)
	**********************************************************************************************/
	void start_round(Lint* player_status)
	{
		LLOG_DEBUG("NiuNiuGameHandler::start_round() Run... Game Start Round........................... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
		clear_round();
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);
		memcpy(m_user_status, player_status, sizeof(m_user_status));

		if (m_desk && m_desk->m_vip&&m_desk->m_clubInfo.m_clubId != 0)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("NiuNiuGameHandler::start_round()... Game Round Info... deskId=[%d], startRound=[%d/%d],  playerCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		LLOG_DEBUG("desk_id=[%d], NiuNiuGetZhuangMode=[%d], NiuNiuGetPlayMode=[%d], NiuNiuGetSpecMode=[%d], NiuNiuGetAutoMode=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_selectZhuangMode, m_playMode, m_isSpacePlay, m_isAutoPlay);

		//游戏开始入口
		start_game();
	}
};


struct NiuNiuGameHandler : NiuNiuGameCore
{

	/* 构造函数 */
	NiuNiuGameHandler()
	{
		LLOG_DEBUG("NiuNiuGameHandler Init...");
	}

	/* 给玩家发消息 */
	void notify_user(LMsg &msg, int pos) override {
		if (NULL == m_desk) return;
		if (pos < 0 || pos >= m_player_count) return;
		User *u = m_desk->m_user[pos];
		if (NULL == u) return;
		u->Send(msg);
	}

	/* 广播消息 */
	void notify_desk(LMsg &msg) override {
		if (NULL == m_desk) return;
		m_desk->BoadCast(msg);
	}

	/* 广播给观战玩家 */
	void notify_lookon(LMsg &msg) override
	{
		if (NULL == m_desk) return;
		m_desk->MHBoadCastDeskLookonUser(msg);
	}

	/* */
	bool startup(Desk *desk) {
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}
	/* */
	void shutdown(void) {
		clear_match(0);
	}


	/* 设置玩法小选项，被Desk::SetVip()调用 */
	void SetPlayType(std::vector<Lint>& l_playtype)
	{
		// 被 Desk 的 SetVip 调用，此时能得到 m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		//小选项：庄家规则
		m_selectZhuangMode = m_playtype.NiuNiuGetZhuangMode();

		//小选项：玩法模式
		m_playMode = m_playtype.NiuNiuGetPlayMode();

		//小选项：花样玩法
		m_isSpacePlay = m_playtype.NiuNiuGetSpecPlay();

		//小选项：是否允许托管
		m_isAutoPlay = m_playtype.NiuNiuGetAutoPlay();

		//小选项：动态加入
		m_isDynamicIn = m_playtype.NiuNiuGetDynamicIn();

		//小选项：6人房 或 10人房
		m_playerNum = m_playtype.NiuNiuGetPlayerNum();

		//小选项：抢庄倍率
		m_qiangZhuangTimes = m_playtype.NiuNiuQiangZhuangTimes();

		//小选项：下注倍率
		m_addScoreTimes = m_playtype.NiuNiuAddScoreTimes();

		//小选项：推注倍数
		m_tuiZhuTimes = m_playtype.NiuNiuTuiZhu();

		// 小选项：牌形算分倍数 0：普通倍率  1：扫雷模式倍率  2：新增牛牛X4倍率
		m_scoreTimes = 0;

		//小选项：牌形倍率牛牛X4
		int t_scoreTimes = m_playtype.NiuNiuTypeTimes();


		if (PLAY_MODE_BOMB == m_playMode)
		{
			m_scoreTimes = 1;    //对应扫雷模式
		}
		else
		{
			if (t_scoreTimes == 1)
			{
				m_scoreTimes = 2;   //新增倍率
			}
			else
			{
				m_scoreTimes = 0;   //普通倍率
			}
		}

		LLOG_ERROR("NiuNiuGameHandler::SetPlayType() Run... This Desk Play Type List ... deskIid=[%d], 庄家规则：%d，玩法模式：%d，花样玩法：%d，是否托管：%d，动态加入：%d，人数：%d，抢庄倍率：%d，下注倍率：%d, TuiZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_selectZhuangMode, m_playMode, m_isSpacePlay, m_isAutoPlay, m_isDynamicIn, m_playerNum, m_qiangZhuangTimes, m_addScoreTimes, m_tuiZhuTimes);

	}

	/***********************************************************************************************************
	* 函数名：    MHSetDeskPlay()
	* 描述：      游戏开始，被Desk::MHHanderStartGame()调用
	* 参数：
	*  @ play_user_count 该玩法所限定的最大玩家数量
	*  @ player_status 玩家状态列表，表示该位置上是否有玩家，0：初始，1：该局参加，2：新加入 OR 该局不参与游戏(数组)
	* 返回：      (void)
	************************************************************************************************************/
	void MHSetDeskPlay(Lint play_user_count, Lint* player_status, Lint player_count)
	{
		LLOG_DEBUG("desk_id=[%d], NiuNiu---------------------MHSetDeskPlay(Lint play_user_count) = [%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		m_player_count = play_user_count;
		start_round(player_status);
	}

	/********************************************************************************
	* 函数名：      HanderNiuNiuSelectZhuang()
	* 描述：
	处理牛牛玩家看牌选庄 */
	bool HanderNiuNiuSelectZhuang(User* pUser, NiuNiuC2SSelectZhuang* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}
		if (GAME_PLAY_SELECT_ZHUANG != m_play_status)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuSelsectZhuang error, user[%d] , status not GAME_PLAY_SELECT_ZHUANG, [%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuSelsectZhuang  user[%d] pos not ok [%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		//只有看牌抢庄这个玩法才会走这个
		return on_event_user_select_zhuang(GetUserPos(pUser), msg->m_qingScore);
	}

	/* 处理牛牛玩家下注 */
	bool HanderNiuNiuAddScore(User* pUser, NiuNiuC2SAddScore* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuAddScore error, user[%d] , status not GAME_PLAY_ADD_SCORE, [%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuAddScore  user[%d] pos not ok [%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

			return false;
		}
		return on_event_user_add_score(GetUserPos(pUser), msg->m_score);

	}

	/* 处理牛牛玩家亮牌 */
	bool HanderNiuNiuOpenCard(User* pUser, NiuNiuC2SOpenCard* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GAME_PLAY_SEND_CARD != m_play_status)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuOpenCard error, user[%d] , status not GAME_PLAY_SEND_CARD, [%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuOpenCard  user[%d] pos not ok [%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

			return false;
		}

		return on_event_user_open_card(GetUserPos(pUser));
	}

	/* 处理玩家断线重连 */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiuGameHandler::OnUserReconnect() Error... The user or desk is NULL...");
			return;
		}

		LLOG_DEBUG("NiuNiuGameHandle::OnUserReconnect() Run... deskId=[%d], reconnUserId=[%d], curPos=[%d], playStatus=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_curPos, m_play_status);

		//Desk 已经发送
		notify_desk_match_state();

		Lint pos = m_desk->GetUserPos(pUser);

		if (INVAILD_POS_QIPAI == pos && !m_desk->MHIsLookonUser(pUser))
		{
			LLOG_ERROR("NiuNiuGameHandle::OnUserReconnect() Error... This Reconn User is not Seat User and not Look On User... deskId=[%d], reconnUser=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		/*LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;
		notify_user(msg, pos);*/


		//获取庄家规则
		Lint tmp_zhuangMode = m_playtype.NiuNiuGetZhuangMode();
		//获取玩法模式
		Lint tmp_playMode = m_playtype.NiuNiuGetPlayMode();

		Lint play_delay_time = 0;
		NiuNiuS2CRecon reconn;
		reconn.m_playStatus = m_play_status;
		memcpy(reconn.m_playerTuiScore, m_player_tui_score, sizeof(reconn.m_playerTuiScore));
		memcpy(reconn.m_user_status, this->m_user_status, sizeof(reconn.m_user_status));
		memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(reconn.m_addZhuang));

		if (NIUNIU_INVAILD_POS > pos)
		{
			//新加字段
			if (m_user_status[pos] == 1)
				reconn.m_DynamicJoin = 0;
			else
				reconn.m_DynamicJoin = 1;
		}
		else if (m_desk->MHIsLookonUser(pUser))
		{
			reconn.m_isLookOn = 1;
			LLOG_DEBUG("NiuNiuGameHandler::OnUserReconnect()  1 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
		}

		switch (m_play_status)
		{

			//选庄阶段断线
		case GAME_PLAY_SELECT_ZHUANG:
		{
			//LLOG_DEBUG("****断线重连desk_id=[%d], OnUserReconnect:GAME_PLAY_SELECT_ZHUANG", m_desk ? m_desk->GetDeskId() : 0);
			reconn.m_hasSelectZhuang = 0;

			if (ZHUANG_MODE_LUN == tmp_zhuangMode && 0 != m_round_offset)
			{
				//轮庄模式，且非第一局
				reconn.m_hasSelectZhuang = 1;
				reconn.m_zhuangPos = m_zhuangPos;
			}
			else if (ZHUANG_MODE_QIANG == tmp_zhuangMode || (ZHUANG_MODE_LUN == tmp_zhuangMode && 0 == m_round_offset))
			{
				//抢庄模式，或 轮庄模式第一局抢庄
				//该用户还未操作看牌抢庄
				reconn.m_hasSelectZhuang = 0;
				if (NIUNIU_INVAILD_POS > pos && m_play_qiang_zhuang[pos] >= 0)
				{
					//该用户抢庄押注阶段已经操作过了
					reconn.m_hasSelectZhuang = 1;
				}
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
				//memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
				play_delay_time = DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU;
			}

			//switch (m_playtype.NiuNiuGetZhuangMode())
			//{
			////轮庄 和 随机庄 是不需要玩家选庄的
			//case ZHUANG_MODE_LUN:
			////case ZHUANG_MODE_RUND:
			//{
			//	reconn.m_hasSelectZhuang = 1;
			//	reconn.m_zhuangPos = m_zhuangPos;
			//	break;
			//}

			////看牌抢庄，需要玩家下抢庄分
			//case ZHUANG_MODE_QIANG:
			//{
			//	//该用户还未操作看牌抢庄
			//	reconn.m_hasSelectZhuang = 0;
			//	if (m_play_qiang_zhuang[pos] >= 0)
			//	{
			//		//该用户抢庄押注阶段已经操作过了
			//		reconn.m_hasSelectZhuang = 1;
			//	}
			//	for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1; i++)
			//	{
			//		reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
			//	}
			//	memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
			//	play_delay_time = DELAY_TIME_SELECT_QIANG_ZHUANG;
			//	break;
			//}

			//default:
			//	break;
			//}
			break;
		}
		//下注阶段断线
		case GAME_PLAY_ADD_SCORE:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			memcpy(reconn.m_addScore, m_play_add_score, sizeof(m_play_add_score));
			memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
			for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				if (m_play_add_score[i] != 0)
				{
					//表示已经下过注了
					reconn.m_hasScore[i] = 1;
				}
			}
			//如果抢庄规则下 或者 明牌下注 玩家的前4张牌推给该玩家
			if (ZHUANG_MODE_QIANG == tmp_zhuangMode || PLAY_MODE_MINGSCORE == tmp_playMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}
			play_delay_time = DELAY_TIME_ADD_SCORE;
			break;
		}
		//发牌 or 开牌
		case GAME_PLAY_SEND_CARD:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			memcpy(reconn.m_addScore, m_play_add_score, sizeof(m_play_add_score));
			for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				if (m_play_add_score[i] != 0)
				{
					//表示已经下过注了
					reconn.m_hasScore[i] = 1;
				}
			}
			//如果抢庄规则下 或者 明牌下注 玩家的前4张牌推给该玩家
			if (ZHUANG_MODE_QIANG == tmp_zhuangMode || PLAY_MODE_MINGSCORE == tmp_playMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}

			for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				reconn.m_hasOpenCard[i] = m_isOpenCard[i] ? 1 : 0;
				if (pos == i) // 自己的手牌
				{
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
					}
				}

				if (m_isOpenCard[i]) // 其他玩家手牌
				{
					reconn.m_player_ox[i] = m_cbOxCard[i];
					reconn.m_player_oxnum[i] = m_cbOxCard[i] ? m_player_oxnum[i] : 0;
					//其他点击开牌的玩家
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
					}
				}
			}
			play_delay_time = DELAY_TIME_OPEN_CARD;
			break;
		}
		default:
			break;
		}

		if (play_delay_time && m_isAutoPlay && m_tick_flag)
		{
			LTime curTime;
			Lint time_escaped = curTime.Secs() - m_play_status_time.Secs();
			Lint time_remain = play_delay_time - time_escaped;
			if (time_remain < 0)
			{
				time_remain = 0;
			}
			else if (time_remain > play_delay_time)
			{
				time_remain = play_delay_time;
			}
			/*if (time_remain > 0 && time_remain <= play_delay_time)
			{
			}
			else if (time_remain > play_delay_time)
			{
			time_remain = play_delay_time;
			}
			else
			{
			time_remain = 0;
			}*/
			reconn.m_remainTime = time_remain;
			LLOG_ERROR("****断线重连desk_id=[%d], OnUserReconnect play_status:%d , delay:%d , escaped:%d  remain:%d",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status, play_delay_time, time_escaped, time_remain);

		}
		if (NIUNIU_INVAILD_POS > pos)
		{
			LLOG_ERROR("NiuNiuGameHandler::OnUserReconnect()  2 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			notify_user(reconn, pos);
		}
		else if (m_desk->MHIsLookonUser(pUser))
		{
			LLOG_ERROR("NiuNiuGameHandler::OnUserReconnect()  3 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			pUser->Send(reconn);
		}
	}

	/* 处理游戏结束 */
	void OnGameOver(Lint result, Lint winpos, Lint bombpos)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_ERROR("NiuNiuGameHandler::OnGameOver() Error... This desk is NULL");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d",
			m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);

		//解散房间，游戏结束
		finish_round(true);
	}

	/* 定时器 */
	void Tick(LTime& curr)
	{
		//桌子对象为空
		if (m_desk == NULL)
		{
			LLOG_ERROR("NiuNiuGameHandler::Tick() Error... This is desk NULL... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		//没有开启超时自动操作选项  或  定时器有效
		if (m_isAutoPlay == false || m_tick_flag == false)
		{
			return;
		}

		//房间解散中，定时器不做处理
		if (m_desk && m_desk->m_resetTime > 0 && m_desk->m_resetUserId > 0)
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() The desk is reseting... So tick do nothing...", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... deskId=[%d], playStatus=[%d], autoPlay=[%d], tickFlag=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_play_status, m_isAutoPlay, m_tick_flag);

		switch (m_play_status)
		{
			//选庄阶段
		case GAME_PLAY_SELECT_ZHUANG:
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Select Zhuang Status... deskId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status, m_isAutoPlay, m_tick_flag);

			switch (m_selectZhuangMode)
			{
				//轮庄模式
			case ZHUANG_MODE_LUN:
			{
				if (m_round_offset == 0)
				{
					if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU)
					{
						LLOG_DEBUG("desk_id=[%d], GAME_MODE_QIANG_ZHUANG time over!", m_desk ? m_desk->GetDeskId() : 0);
						for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
						{
							if (m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
							{
								on_event_user_select_zhuang(i, 0);
							}
						}
						//执行完倒计时，禁用倒计时
						LLOG_ERROR("desk_id=[%d], set m_tick_flag = 0 ....", m_desk ? m_desk->GetDeskId() : 0);
						//m_tick_flag = false;
					}
				}
				break;
			}
			//随机庄模式
			/*case ZHUANG_MODE_RUND:
			{
			break;
			}*/

			//抢庄模式
			case ZHUANG_MODE_QIANG:
			{
				LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Select Zhuang Status, Now is Qiang Zhuang... deskId=[%d], playStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_play_status);

				if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU)
				{
					LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Qiang Zhuang Time over!!!, Auto Qiang Zhuang... deskId=[%d]",
						m_desk ? m_desk->GetDeskId() : 0);

					for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
					{
						if (m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
						{
							on_event_user_select_zhuang(i, 0);
						}
					}
					//执行完倒计时，禁用倒计时
					LLOG_ERROR("desk_id=[%d], set m_tick_flag = 0 ....", m_desk ? m_desk->GetDeskId() : 0);
					//m_tick_flag = false;
				}
				break;
			}

			}
			break;
		}
		//下注阶段
		case GAME_PLAY_ADD_SCORE:
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Add Score Status, Now is Add Score... deskId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);
			if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_ADD_SCORE)
			{
				LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Add Score Time over!!!, Auto Add Score... deskId=[%d]",
					m_desk ? m_desk->GetDeskId() : 0);

				for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
				{
					if (m_play_add_score[i] == 0 && m_user_status[i] == 1)
					{
						on_event_user_add_score(i, min_add_score());
					}
				}
				//执行完倒计时，禁用倒计时
				//m_tick_flag = false;
			}
			break;
		}
		//开牌阶段
		case GAME_PLAY_SEND_CARD:
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Open Cards Status, Now is Open Cards... deskId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);
			if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_OPEN_CARD)
			{
				LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Open Card Time over!!!, Auto Open Card... deskId=[%d]",
					m_desk ? m_desk->GetDeskId() : 0);

				for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
				{
					if (m_isOpenCard[i] == 0 && m_user_status[i] == 1)
					{
						on_event_user_open_card(i);
					}
				}
				//执行完倒计时，禁用倒计时
				//m_tick_flag = false;
			}
			break;
		}

		}//switch (m_play_status) end...
	}
};

DECLARE_GAME_HANDLER_CREATOR(103, NiuNiuGameHandler);