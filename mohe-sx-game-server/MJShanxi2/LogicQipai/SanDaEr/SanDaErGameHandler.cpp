#include "SanDaErGameLogic.h"
#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "LMsgL2C.h"
#include "LMsgPoker.h"



/*
*  三打二Debug配牌使用
*  cbRandCard 用于输出的牌
*  m_desk Desk对象
*  conFileName 配牌文件
*  playerCount 玩家数量
*  handCardsCount 玩家手牌数量
*  cellPackCount 牌总数
*/
static bool debugSendCards_SanDaEr(Lint* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
{
	if (conFileName == NULL || std::strlen(conFileName) == 0)
	{
		return false;
	}

	//配置文件中是否配置允许调牌
	bool can_assign = gConfig.GetDebugModel();

	FILE *fp = NULL;
	try
	{
		fp = fopen(conFileName, "r");
	}
	catch (const std::exception& e)
	{
		LLOG_ERROR("File card.ini is open failed...  desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);
		if(fp) fclose(fp);
		fp = NULL;
		return false;
	}

	//允许服务器端配牌 && card.ini存在
	if (can_assign && fp)
	{
		LLOG_ERROR("Send Cards...This is debug mode!!! desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//用于存储配牌的整体变量
		Lint* t_cbRandCard = new Lint[cellPackCount];
		memset(t_cbRandCard, 0, sizeof(Lint) * cellPackCount);

		//解析card.ini得到的配牌
		std::vector<BYTE> t_sendCards;

		//解析牌后每个人配牌的合法张数
		size_t* t_sendCount = new size_t[playerCount];
		memset(t_sendCount, 0, sizeof(size_t) * playerCount);
		size_t t_count = 0;

		//给每个玩家解析有效的配牌
		for (int i = 0; i < playerCount; ++i)
		{
			t_sendCards.clear();
			//解析个人玩家手牌
			LoadPlayerCards(t_sendCards, i + 1, conFileName);
			t_count = 0;

			for (int j = 0; j < t_sendCards.size(); ++j)  //配的牌vector
			{
				for (int k = 0; k < cellPackCount; ++k)  //1副牌
				{
					if (t_sendCards[j] == cbRandCard[k])
					{
						t_cbRandCard[i * handCardsCount + t_count++] = cbRandCard[k];
						cbRandCard[k] = 0;
						break;
					}
				}
			}
			t_sendCount[i] = t_count;
		}

		//解析配牌有效底牌
		t_sendCards.clear();
		LoadBaseCards(t_sendCards, conFileName);
		size_t t_baseCount = 0;
		for (int j = 0; j < t_sendCards.size(); ++j)  //配的牌vector
		{
			for (int k = 0; k < cellPackCount; ++k)  //两副牌
			{
				if (t_sendCards[j] == cbRandCard[k])
				{
					t_cbRandCard[playerCount * handCardsCount + t_baseCount++] = cbRandCard[k];
					cbRandCard[k] = 0;
					break;
				}
			}
		}

		//补全手牌
		for (int i = 0; i < playerCount; ++i)
		{
			for (int j = i * handCardsCount + t_sendCount[i]; j < (i+1) * handCardsCount; )
			{
				for (int k = 0; k < cellPackCount; ++k)
				{
					if (cbRandCard[k] != 0)
					{
						t_cbRandCard[j++] = cbRandCard[k];
						cbRandCard[k] = 0;
						break;
					}
				}
			}
		}

		//补全底牌
		for (int j = playerCount * handCardsCount + t_baseCount; j < cellPackCount; )
		{
			for (int k = 0; k < cellPackCount; ++k)
			{
				if (cbRandCard[k] != 0)
				{
					t_cbRandCard[j++] = cbRandCard[k];
					cbRandCard[k] = 0;
					break;
				}
			}
		}

		//将配好的牌写到输出数组中
		memcpy(cbRandCard, t_cbRandCard, sizeof(Lint) * cellPackCount);

		if (t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_sendCount);
		t_sendCount = NULL;

		if (fp) fclose(fp);
		fp = NULL;
		return true;
	}

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}

/* 三打二游戏中的各个阶段（枚举） */

enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = 0,			//游戏开始
	GAME_PLAY_SEND_CARD = 1,		//发牌阶段
	GAME_PLAY_SELECT_SCORE = 2,		//叫牌分阶段
	GAME_PLAY_SELECT_MAIN = 3,		//庄家选主阶段
	GAME_PLAY_BASE_CARDS = 4,		//庄家埋底阶段
	GAME_PLAY_SELECT_FRIEND = 5,	//庄家选对家阶段
	GAME_PLAY_OUT_CARD = 6,			//出牌阶段
	GAME_PLAY_SETTLEMENT = 7,		//结算阶段
	GAME_PLAY_END = 8,				//游戏结束
	GAME_PLAY_ZHUAGN_JIAOPAI = 9,	// 庄家交牌
};

/* 三打二游戏中的音效 */
enum GAME_PLAY_SOUND
{
	GAME_PLAY_SOUND_PA = 0,				//啪
	GAME_PLAY_SOUND_DIAOZHU = 1,		//吊主
	GAME_PLAY_SOUND_GUANSHANG = 2,		//管上
	GAME_PLAY_SOUND_DANI = 3,			//大你
	GAME_PLAY_SOUND_BIPAI = 4,			//毙牌
	GAME_PLAY_SOUND_GAIBI = 5,			//盖毙
	GAME_PLAY_SOUND_PO = 6,				//破
	GAME_PLAY_SOUND_XIANJIADEIFEN = 7,	//闲家得分
	GAME_PLAY_SOUND_DIANPAI = 8,		//我垫牌

};

enum OVER_STATE
{
	GAME_OVER_A = 0,		//a 提前结束：提前结束
	GAME_OVER_B = 1,		//b 完成比赛，庄家保底：庄家保底
	GAME_OVER_C = 2,		//c 完成比赛，庄家保底，闲家一分未得：闲家未捡分，庄家翻倍
	GAME_OVER_D = 3,		//d 完成比赛，庄家保底，闲家一份未得，庄家叫分100：闲家未捡分，庄叫分100
	GAME_OVER_E = 4,		//e 完成比赛，闲家扣底：闲家扣底，闲家翻倍
	GAME_OVER_F = 5,		//f 完成比赛，闲家扣底：闲家扣底，庄家叫分100：闲家扣底，庄叫分100
	GAME_OVER_G = 6,		//g 提前结束，庄家摊牌
	GAME_OVER_JIAOPAI = 7,  //交牌
};


/* 三打二默认操作时间 */
enum GAME_DEFAULT_TIME
{
	JIAOPAI_REQUEST_TIMEOUT = 15,
	JIAOPAI_RESPONSE_TIMEOUT = 15
};

enum JIAO_PAI_STATE
{
	JIAOPAI_STATE_INIT = 0, //默认值
	JIAOPAI_STATE_REQUESTED = 1, // 庄家已申请	 
	JIAOPAI_STATE_COMPLETE = 2, // 已完成,所有人同意
	JIAOPAI_STATE_RQUEST_TIMEOUT = 3, // 申请超时
	JIAOPAI_STATE_XIAN_NOT_AGREE = 4, // 闲家已拒绝
	JIAOPAI_STATE_XIAN_TIMEOUT = 5, // 闲家已超时
	JIAOPAI_STATE_END = 6 // 结束状态
};

enum SANDAER_OVER_STATE
{
	DIPAI_NONE_WIN = 0,
	DIPAI_ZHUANG_JIAOPAI = 1,           //庄家交牌
	DIPAI_ZHUANG_WIN = 2,               //庄家保底
	DIPAI_ZHUANG_WIN_XIAN_SCORE0 = 3,	//庄家保底 光头
	DIPAI_XIAN_WIN = 4                  //闲家抠底
};


/* 三打二游戏小选项(结构体) */
struct PlayType
{
protected:
	std::vector<Lint>	m_playtype;  //游戏小选项

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
	std::vector<Lint> GetType()
	{
		return m_playtype;
	}

	/******************************************************
	* 函数名：   SanDaErBaseTimes()
	* 描述：     获取客户端选择的底分
	* 详细：     底分倍数：1、1分，2、2分，3、3分
	* 返回：     (Lint)客户端选择的底分
	********************************************************/
	Lint SanDaErBaseTimes() const;

	/******************************************************
	* 函数名：   SanDaEr2IsCommMain()
	* 描述：     获取客户端选择的2是否为常主
	* 详细：     2是否为常主：false-2不是常主  true-2是常主
	* 返回：     (bool)客户端选择的2是否为常主
	********************************************************/
	bool SanDaEr2IsCommMain() const;

	/******************************************************
	* 函数名：   SanDaEr2IsCommMain()
	* 描述：     获取客户端选择的相邻位置禁止进入
	* 详细：     2是否为常主：false- 禁止进入  true-不禁止进入
	* 返回：     (bool)客户端选择的相邻位置禁止进入
	********************************************************/
	/*bool PlayType::SanDaEr2IsGPS() const;*/


	/******************************************************
	* 函数名：   SanDaEr2IsCommMain()
	* 描述：     获取客户端选择的对家要牌含十
	* 详细：     2是否为常主：false- 禁止进入  true-不禁止进入
	* 返回：     (bool)客户端选择的相邻位置禁止进入
	********************************************************/
	bool PlayType::SanDaEr2IsPokerTen() const;


	/******************************************************
	* 函数名：   SanDaEr2IsCheat()
	* 描述：     专属防作弊
	* 详细：     是否含十：false-不防  true-防
	* 返回：     (bool)客户端选择的相邻位置禁止进入
	********************************************************/
	bool PlayType::SanDaEr2IsCheat() const;

	/******************************************************
	* 函数名：   SanDaErIsPlayFu()
	* 描述：     是否允许打副
	* 详细：     是否含十：false-不允许  true-允许
	* 返回：     (bool)客户端选择
	********************************************************/
	bool PlayType::SanDaErIsPlayFu() const;

	/******************************************************
	* 函数名：   SanDaErPlayType()
	* 描述：     三打二小玩法
	* 详细：     小玩法：0：经典玩法，1：三挖子玩法
	* 返回：     (Lint)客户端选择
	********************************************************/
	Lint PlayType::SanDaErPlayType() const;

	/******************************************************
	* 函数名：   SanDaErSelectWa()
	* 描述：     三挖 & 二挖
	* 详细：     小玩法：0：不支持，1：二挖子玩法，2：三挖子
	* 返回：     (Lint)客户端选择
	********************************************************/
	Lint PlayType::SanDaErSelectWa() const;

	/******************************************************
	* 函数名：   SanDaErIsJiaoPai()
	* 描述：     是否允许庄家交牌
	* 详细：     0-不允许  1:8分交牌   2:12分交牌
	* 返回：     (Lint)是否允许庄家交牌
	********************************************************/
	Lint  SanDaErIsJiaoPai() const;
};



/******************************************************
* 函数名：   SanDaErBaseTimes()
* 描述：     获取客户端选择的底分
* 详细：     底分倍数：1、1分，2、2分，3、3分
* 返回：     (Lint)客户端选择的底分
********************************************************/
Lint PlayType::SanDaErBaseTimes() const
{
	if (m_playtype.empty())
	{
		return 1;
	}
	if (m_playtype.size() > 0 && m_playtype[0] < 1 && m_playtype[0] > 3)
	{
		return 1;
	}
	return m_playtype[0];
}

/******************************************************
* 函数名：   SanDaEr2IsCommMain()
* 描述：     获取客户端选择的2是否为常主
* 详细：     2是否为常主：false-2不是常主  true-2是常主
* 返回：     (bool)客户端选择的2是否为常主
********************************************************/
bool PlayType::SanDaEr2IsCommMain() const
{
	if (m_playtype.size() < 2)
	{
		return false;
	}

	return m_playtype[1] == 1;
}

/******************************************************
* 函数名：   SanDaEr2IsCommMain()
* 描述：     获取客户端选择的对家要牌含十
* 详细：     是否含十：false-不含  true-含十
* 返回：     (bool)客户端选择的相邻位置禁止进入
********************************************************/
//bool PlayType::SanDaEr2IsGPS() const
//{
//	if (m_playtype.size() < 3)
//	{
//		return false;
//	}
//
//	return m_playtype[2] == 1;
//}



/******************************************************
* 函数名：   SanDaEr2IsCommMain()
* 描述：     获取客户端选择的对家要牌含十
* 详细：     是否含十：false-不含  true-含十
* 返回：     (bool)客户端选择的相邻位置禁止进入
********************************************************/
bool PlayType::SanDaEr2IsPokerTen() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}

	return m_playtype[3] == 1;
}

/******************************************************
* 函数名：   SanDaEr2IsCheat()
* 描述：     专属防作弊
* 详细：     是否含十：false-不防  true-防
* 返回：     (bool)客户端选择的相邻位置禁止进入
********************************************************/
bool PlayType::SanDaEr2IsCheat() const
{
	if (m_playtype.size() < 5)
	{
		return false;
	}

	return m_playtype[4] == 1;
}

/******************************************************
* 函数名：   SanDaErIsPlayFu()
* 描述：     是否允许打副
* 详细：     是否含十：false-不允许  true-允许
* 返回：     (bool)客户端选择
********************************************************/
bool PlayType::SanDaErIsPlayFu() const
{
	if (m_playtype.size() < 6)
	{
		return false;
	}

	return m_playtype[5] == 1;
}

/******************************************************
* 函数名：   SanDaErPlayType()
* 描述：     三打二小玩法
* 详细：     小玩法：0：经典玩法，1：三挖子玩法
* 返回：     (Lint)客户端选择
********************************************************/
Lint PlayType::SanDaErPlayType() const
{
	if (m_playtype.size() < 7)
	{
		return 0;
	}

	if (m_playtype[6] != 1)
	{
		return 0;
	}

	return m_playtype[6];
}

/******************************************************
* 函数名：   SanDaErSelectWa()
* 描述：     三挖 & 二挖
* 详细：     小玩法：0：不支持，1：二挖子玩法，2：三挖子
* 返回：     (Lint)客户端选择
********************************************************/
Lint PlayType::SanDaErSelectWa() const
{
	if (m_playtype.size() < 8)
	{
		return 0;
	}
	//不是三挖子玩法，不支持
	if (m_playtype[6] != 1)
	{
		return 0;
	}

	if (m_playtype[7] != 2 && m_playtype[7] != 1)
	{
		return 1;
	}

	return m_playtype[7];
}

/******************************************************
* 函数名：   SanDaErIsJiaoPai()
* 描述：     是否允许庄家交牌
* 详细：     0-不允许  1:8分交牌   2:12分交牌
* 返回：     (Lint)是否允许庄家交牌
********************************************************/
Lint  PlayType::SanDaErIsJiaoPai() const
{
	if (m_playtype.size() < 9)
	{
		return 0;
	}

	if (m_playtype[6] != 1)
	{
		return 0;
	}

	return m_playtype[8] % 3;
}


/* 三打二每局不会初始化字段（结构体）*/
struct SanDaErRoundState__c_part
{
	Lint				m_curPos;											// 当前操作玩家
	Lint				m_firstOutPos;										//每轮第一个出牌玩家
	Lint				m_wTurnWinner;										//每轮出牌的胜利玩家
	Lint				m_friendZhuangPos;									// 副庄家位置
	Lint				m_selectFriendCard;									//玩家选对家的牌
	Lint				m_thisTurnScore;									//本轮闲家得分
	Lint				nOverState;											//本轮结束庄家闲家显示结果，庄家保底，闲家抠底...

	GAME_PLAY_STATE		m_play_status;										// 牌局状态
	OVER_STATE			m_over_state;										//小结状态
	
	Lint				m_user_status[SANDAER_PLAYER_COUNT];				// 用户状态：改位置上是否坐人了（实际有效位位置）
	BOOL				m_tick_flag;										// 定时器开关

	Lint				m_mainColor;										//主牌花色

	Lint				m_max_select_score;									//选择最大的叫牌分数
	Lint				m_select_score[SANDAER_PLAYER_COUNT];				//玩家叫牌分数

	Lint				m_base_cards_count;									//底牌数量
	Lint				m_base_cards[SANDAER_BASE_CARDS_COUNT];				//底牌

	Lint				m_hand_cards_count[SANDAER_PLAYER_COUNT];			//各个玩家手牌数量
	Lint				m_hand_cards[SANDAER_PLAYER_COUNT][SANDAER_HAND_CARDS_COUNT_MAX];		//玩家手牌

	Lint				m_out_cards[SANDAER_PLAYER_COUNT];					//玩家每轮出的牌
	Lint				m_prev_out_cards[SANDAER_PLAYER_COUNT];				//玩家每轮出的牌

	Lint				m_scoreCardsCount;									//得分扑克数目
	Lint				m_scoreCards[12 * SANDAER_PACK_COUNT];				//得分扑克
	Lint				m_leftover_score;									//本局剩余的分

	Lint				m_player_score[SANDAER_PLAYER_COUNT];				//玩家本局得分

	Lint				m_reset105[SANDAER_PLAYER_COUNT];					//满105申请结束 -2:初始状态；-1:第一次满105分等待选择状态；0:继续游戏；1:结束本局
	Lint				m_resetTime105;										//时间
	BOOL				m_b105;												//本轮是否申请过105了

	// 交牌控制
	SANDAER_OVER_STATE  m_sandaer_overstate;								// 新的结束状态		
	Lint				m_jiaoPai[SANDAER_PLAYER_COUNT];					//每个人对待交牌的处理结果 -2:初始状态， -1:等待选择， 0:不同意  1:同意 2:已超时
	JIAO_PAI_STATE		m_jiaoPaiState;                                     //交牌状态
	BOOL				m_bJiaoPai;											//本轮是否已经申请过交牌了	 
	Lint				m_nTimeSecondZhuangRequstJiaoPai;					// 记录可申请交牌开始时间
	Lint				m_nTimeSecondZhuangReqesutJiaoPaiLeft;				// 庄家剩余申请时间
	Lint				m_nTimeSecondXianResponseJiaoPai;					// 闲家操作开始时间
	Lint				m_nTimeSecondXianResponseJiaoPaiLeft;				// 闲家剩余操作时间

	/* 结构体清零函数 */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_curPos = SANDAER_INVALID_POS;
		m_firstOutPos = SANDAER_INVALID_POS;
		m_wTurnWinner = SANDAER_INVALID_POS;
		m_friendZhuangPos = SANDAER_INVALID_POS;
		m_mainColor = 0x40;
		m_base_cards_count = SANDAER_BASE_CARDS_COUNT;
		m_selectFriendCard = 0;
		m_thisTurnScore = 0;
		m_resetTime105 = 0;
		m_b105 = false;
		m_leftover_score = 100;
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			m_select_score[i] = -1;
			m_reset105[i] = -2;
		}
		m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_INIT;
		m_bJiaoPai = false;
		m_nTimeSecondZhuangRequstJiaoPai = m_nTimeSecondXianResponseJiaoPai = 0;
		m_nTimeSecondZhuangReqesutJiaoPaiLeft = m_nTimeSecondXianResponseJiaoPaiLeft = 0;
	}
};

/* 三打二每局会初始化字段（结构体）*/
struct SanDaErRoundState__cxx_part
{
	LTime m_play_status_time;

	/* 清零每局字段 */
	void clear_round()
	{
		this->~SanDaErRoundState__cxx_part();
		new (this)SanDaErRoundState__cxx_part;
	}
};

/* 三打二每场不会初始化的字段（结构体）*/
struct SanDaErMatchState__c_part
{
	Desk*		m_desk;									// 桌子对象
	Lint		m_round_offset;							// 当前局数
	Lint		m_round_limit;							// 创建房间选择的最大局数
	Lint		m_accum_score[SANDAER_PLAYER_COUNT];	//存储玩家总得分
	BOOL		m_dismissed;
	Lint		m_registered_game_type;
	Lint		m_player_count;							// 玩法核心代码所使用的玩家数量字段
	Lint		m_zhuangPos;							// 庄家位置
	BOOL		m_isFirstFlag;							// 首局显示开始按钮,true代表首局
	Lint		m_baseCardCount;						// 底牌数量
	Lint		m_firstSelectCardPos;					// 每局第一个叫牌的位置

	////玩法小选项
	Lint		m_baseTimes;							//底分倍数
	BOOL		m_is2ChangZhu;							//2是否为常主，true：2是常主  false：2不是常主
	BOOL		m_isPokertTen;							//是否含十：0-不含  1-含十
	BOOL		m_cheatAgainst;							//专属防作弊
	BOOL		m_isPlayFu;								//是否允许打副 0-不允许，1-允许
	Lint		m_playTypeWa;								//玩法：0：经典玩法，1：三挖玩法
	Lint		m_selectWa;								//选择挖数： 0：不支持， 1：二挖， 2：三挖
	Lint 		m_nJiaoPaiOption;							//是否允许庄家交牌：    0：不予许  1: 8分交，2:12分交

	Lint		m_packCount;							//牌副数

	/* 清零结构体字段 */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_baseTimes = 1;
		m_packCount = 1;
		m_zhuangPos = SANDAER_PLAYER_COUNT;
		m_firstSelectCardPos = SANDAER_PLAYER_COUNT;
	}
};

/* 三打二每场会初始化的字段（结构体）*/
struct SanDaErMatchState__cxx_part
{
	SDEGameLogic   m_gamelogic;     // 游戏逻辑
	PlayType       m_playtype;	    // 桌子玩法小选项

									/* 清空每场结构体 */
	void clear_match()
	{
		this->~SanDaErMatchState__cxx_part();
		new (this) SanDaErMatchState__cxx_part;
	}
};

/*
*  三打二每局所需要的所有字段（结构体）
*  继承 ：SanDaErRoundState__c_pard, SanDaErRoundState_cxx_part
*/
struct SanDaErRoundState : SanDaErRoundState__c_part, SanDaErRoundState__cxx_part
{
	void clear_round()
	{
		SanDaErRoundState__c_part::clear_round();
		SanDaErRoundState__cxx_part::clear_round();
	}
};

/*
*  三打二每场所需要的所有字段（结构体）
*  继承：SanDaErMatchState__c_pard, SanDaErMatchState_cxx_pard
*/
struct SanDaErMatchState : SanDaErMatchState__c_part, SanDaErMatchState__cxx_part
{
	void clear_match()
	{
		SanDaErMatchState__c_part::clear_match();
		SanDaErMatchState__cxx_part::clear_match();
	}
};

/*
*  三打二桌子状态（结构体）
*  继承：ShuangShengRoundState, NiuNiuMatchState
*/
struct SanDaErDeskState : SanDaErRoundState, SanDaErMatchState
{
	/* 清零每局数据 */
	void clear_round()
	{
		SanDaErRoundState::clear_round();
	}

	/* 清零每场数据*/
	void clear_match(int player_count)
	{
		SanDaErMatchState::clear_match();
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
		if (play_status > GAME_PLAY_BEGIN && play_status <= GAME_PLAY_END)
		{
			m_play_status = (GAME_PLAY_STATE)play_status;
		}
	}

	/* 获取该玩家在桌子上的位置 */
	Lint GetUserPos(User* pUser)
	{
		if (NULL == pUser)
		{
			return SANDAER_INVALID_POS;
		}

		Lint pos = SANDAER_INVALID_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= SANDAER_INVALID_POS)
		{
			pos = SANDAER_INVALID_POS;
		}
		return pos;
	}

	/* 获取上一个有效位置 */
	Lint GetPrePos(Lint pos)
	{
		if (m_player_count == 0)
			return 0; // 避免除零崩溃	
		Lint nextPos = (pos - 1) % m_player_count;
		for (int i = 0; i < m_player_count; i++)
		{
			nextPos = (nextPos - 1) % m_player_count;
		}
		return nextPos;
	}

	/* 获取下一个有效位置 */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0)
			return 0; // 避免除零崩溃	
		Lint nextPos = (pos + 1) % m_player_count;
		for (int i = 0; i < m_player_count; i++)
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
		return 0 <= pos && pos < SANDAER_INVALID_POS;
	}

	/* 俱乐部会长是否在明牌观战 */
	bool IsClubOwerLookOn()
	{
		if (NULL == m_desk)
		{
			return false;
		}
		if (m_desk->m_clubOwerLookOn == 1 && m_desk->m_clubOwerLookOnUser)
		{
			return true;
		}
		return 0;
	}

	/* 获取本轮以出牌的人数 */
	Lint GetHasOutPlayerCount()
	{
		int t_outPlayerCount = 0;
		if (m_firstOutPos == SANDAER_INVALID_POS
			|| !m_gamelogic.IsValidCard(m_out_cards[m_firstOutPos]))
		{
			return 0;
		}

		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			if (m_gamelogic.IsValidCard(m_out_cards[i]))
			{
				++t_outPlayerCount;
			}
		}
		return t_outPlayerCount;
	}
};

/* 三打二录像功能(结构体) */
struct SanDaErVideoSupport : SanDaErDeskState
{
	QiPaiVideoLog m_video; //录像

	///////////////////////////////////////////////////////////////////
	//这块集中存放和录像有关的函数
	///////////////////////////////////////////////////////////////////
	void VideoSave()
	{
		if (NULL == m_desk)
			return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_playType = m_desk->getPlayType();
		m_video.m_flag= m_desk->m_state;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		LLOG_DEBUG("videoSave[%s]", video.m_sql.c_str());
		gWork.SendMsgToDb(video);
	}

	//三打二 oper 操作  0--过  1--出牌   位置   手牌个数   手牌 操作的牌个数  操作的牌  得分
	void SaveUserOper(Lint oper, Lint pos, BYTE handcardCount, Lint *cbHandCardData, Lint cardCount = 0, Lint *cbCardData = NULL, Lint score = 0, Lint sound = -1)
	{
		LLOG_DEBUG("SaveUserOper  pos[%d],oper[%d]", pos, oper);

		std::vector<Lint> cards;
		std::vector<Lint> handcards;
		if (cardCount != 0)
		{
			if (cbCardData != NULL)
			{
				for (int i = 0; i < cardCount; i++)
				{
					LLOG_DEBUG("cards[%d]=[%d]", i, cbCardData[i]);
					cards.push_back(cbCardData[i]);
				}
			}
		}
		if (handcardCount != 0)
		{
			if (cbHandCardData != NULL)
			{
				for (int i = 0; i < handcardCount; i++)
				{
					LLOG_DEBUG("cbHandCardData[%d]=[%d]", i, cbHandCardData[i]);
					handcards.push_back(cbHandCardData[i]);
				}
			}
		}
		m_video.AddOper(oper, pos, cards, score, handcards, sound);
	}
};

/* 三打二游戏具体处理逻辑（结构体）*/
struct SanDaErGameCore : GameHandler, SanDaErVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	virtual void notify_lookon(LMsg &msg){
	}

	virtual void notify_club_ower_lookon(LMsg& msg){
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
		notify_lookon(msg);
		notify_club_ower_lookon(msg);
	}

	/********************************************************************************************
	* 函数名：      start_game()
	* 描述：        三打二每一局游戏开始的具体入口
	* 返回：        (void)
	*********************************************************************************************/
	void start_game()
	{
		//设置游戏状态为发牌阶段
		set_play_status(GAME_PLAY_SEND_CARD);

		//设置当前玩家为本轮第一个叫牌玩家
		m_curPos = m_firstSelectCardPos;

		//洗牌发牌
		dispatch_user_cards();
	}

	/*********************************************************************************************
	* 函数名：     start_round()
	* 描述：       双升游戏每一局的总开始函数
	* 参数：
	*  @ *player_status 玩家状态列表，表示该位置上是否有玩家，0：初始，1：该局参加，2：新加入 OR 该局不参与游戏(数组)
	* 返回：       (void)
	**********************************************************************************************/
	void start_round(Lint* player_status)
	{
		LLOG_DEBUG("SanDaErGameHandler::start_round() Run... SANDAER GAME_START!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! deskId=[%d],",
			m_desk ? m_desk->GetDeskId() : 0);

		LLOG_ERROR("SanDaErGameHandler::start_round() Run... This Round Play Type deskId=[%d], baseTimes=[%d], is2ChangZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_baseTimes, m_is2ChangZhu);

		Lint t_user_status[SANDAER_PLAYER_COUNT];
		memcpy(t_user_status, player_status, sizeof(Lint) * SANDAER_PLAYER_COUNT);

		clear_round();

		//广播当前局数给玩家
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);

		memcpy(m_user_status, t_user_status, sizeof(m_user_status));
		
		if (m_desk && m_desk->m_vip)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("desk_id=[%d], start_round=[%d/%d], player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		//游戏开始入口
		start_game();
	}

	//设置音效
	int GetSound()
	{
		int nRet = 0;
		int nOutCardCount = GetHasOutPlayerCount();					//本轮出牌的人的人数
		//m_firstOutPos;											//每轮第一个出牌玩家

		// 本轮每个玩家出的牌   第一个出牌的玩家  几个玩家
		Lint nOutMax = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, nOutCardCount);   //本轮最大的人
		Lint nFirstColor = m_gamelogic.GetCardLogicColor(m_out_cards[m_firstOutPos]);				 //第一个出牌的人的花色
		Lint nCurrColor = m_gamelogic.GetCardLogicColor(m_out_cards[m_curPos]);

		//打无主状态下声音播放
		if (m_mainColor == COLOR_NT)
		{
			//首家出牌 报牌值
			if (nOutCardCount == 1)
			{
				return GAME_PLAY_SOUND_PA;
			}
			//大你 or 管上
			if (nOutCardCount >= 2 && nOutMax == m_curPos)
			{
				if ((gWork.GetCurTime().Secs() % 2) == 0)
				{
					return GAME_PLAY_SOUND_DANI;
				}
				else
				{
					return GAME_PLAY_SOUND_GUANSHANG;
				}
			}
			//我垫牌
			if (nOutCardCount >= 2 && nCurrColor != nFirstColor)
			{
				return GAME_PLAY_SOUND_DIANPAI;
			}

			//默认播放啪
			return GAME_PLAY_SOUND_PA;
		}

		//有主打法
		if (nOutCardCount == 1 && nFirstColor == COLOR_NT)		//本轮第一个出牌,且出的主牌花色
		{
			nRet = GAME_PLAY_SOUND_DIAOZHU;
			return nRet;
		}

		//判断毙和盖毙		条件1:出牌最大.条件2:第一个人出的不是主,之前没有人毙,两个人及以上才
		if (nOutMax == m_curPos && nFirstColor != COLOR_NT && nOutCardCount >= 2 && m_gamelogic.GetCardLogicColor(m_out_cards[m_curPos]) == COLOR_NT)
		{
			int iTmpMax = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, nOutCardCount - 1);					 //除去自己,谁最大
																															 //如果最大的是主牌,则是盖毙,如果最大的不是主牌,则是毙牌
			if (m_gamelogic.GetCardLogicColor(m_out_cards[iTmpMax]) == COLOR_NT)
			{
				return GAME_PLAY_SOUND_GAIBI;
			}
			else
			{
				return GAME_PLAY_SOUND_BIPAI;
			}
		}

		//打你 or 管上
		if (nOutCardCount >= 2 && nOutMax == m_curPos)
		{
			if ((gWork.GetCurTime().Secs() % 2) == 0)
			{
				return GAME_PLAY_SOUND_DANI;
			}
			else
			{
				return GAME_PLAY_SOUND_GUANSHANG;
			}
		}

		//垫牌
		if (nOutCardCount >= 2 && nCurrColor != COLOR_NT && nCurrColor != nFirstColor)
		{
			return GAME_PLAY_SOUND_DIANPAI;
		}

		//默认啪
		return GAME_PLAY_SOUND_PA;
	}

	/* 洗牌发牌 */
	bool dispatch_user_cards()
	{
		//乱序后的扑克牌
		Lint t_randCards[SANDAER_CELL_PACK * SANDAER_PACK_COUNT];

		//回放记录玩家手牌
		std::vector<Lint> vec_handCards[SANDAER_PLAYER_COUNT];
		std::vector<Lint> vec_baseCards;

		//乱序扑克
		m_gamelogic.RandCardList(t_randCards, SANDAER_CELL_PACK * SANDAER_PACK_COUNT);

		//配牌
		debugSendCards_SanDaEr(t_randCards, m_desk, "card_sandaer.ini", SANDAER_PLAYER_COUNT, SANDAER_HAND_CARDS_COUNT, SANDAER_CELL_PACK);
		
		//给每个玩家发手牌
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; i++)
		{
			m_hand_cards_count[i] = SANDAER_HAND_CARDS_COUNT;
			memcpy(&m_hand_cards[i], &t_randCards[i * m_hand_cards_count[i]], sizeof(Lint) * m_hand_cards_count[i]);
			m_gamelogic.SortCardList(m_hand_cards[i], m_hand_cards_count[i]);

			for (int n = 0; n < m_hand_cards_count[i]; n++)
			{
				vec_handCards[i].push_back(m_hand_cards[i][n]);
			}
		}

		//底牌扑克
		m_baseCardCount = SANDAER_BASE_CARDS_COUNT;
		memcpy(m_base_cards, &t_randCards[SANDAER_CELL_PACK * SANDAER_PACK_COUNT - m_baseCardCount], sizeof(Lint) * m_baseCardCount);

		for (int i = 0; i < SANDAER_BASE_CARDS_COUNT; ++i)
		{
			vec_baseCards.push_back(m_base_cards[i]);
		}


		//给玩家发牌命令
		SanDaErS2CSendCards sendCards[SANDAER_PLAYER_COUNT];
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; i++)
		{
			sendCards[i].m_outTime = 0;
			sendCards[i].m_pos = i;
			sendCards[i].m_nextSelectScorePos = m_firstSelectCardPos;
			sendCards[i].m_handCardsCount = m_hand_cards_count[i];
			sendCards[i].m_is2ChangZhu = m_is2ChangZhu ? 1 : 0;
			for (int j = 0; j < SANDAER_HAND_CARDS_COUNT; ++j)
			{
				sendCards[i].m_handCards[j] = (int)m_hand_cards[i][j];
			}

			memcpy(sendCards[i].m_totleScore, m_accum_score, sizeof(m_accum_score));

			//发牌时候排列手牌排列手牌，主花色等牌无法排序
			m_gamelogic.SortCardList(m_hand_cards[i], m_hand_cards_count[i]);
		}
		notify_user(sendCards[0], 0);
		notify_user(sendCards[1], 1);
		notify_user(sendCards[2], 2);
		notify_user(sendCards[3], 3);
		notify_user(sendCards[4], 4);

		//记录录像（洗牌）
		m_video.DrawClear();
		m_video.DealCard(m_desk->m_vip->m_curCircle, SANDAER_PLAYER_COUNT, m_desk->m_vip->m_posUserId, vec_handCards, vec_baseCards, gWork.GetCurTime().Secs());

		//广播给观战玩家发牌
		SanDaErS2CSendCards sendCards_look;
		sendCards_look.m_outTime = 0;
		sendCards_look.m_pos = SANDAER_INVALID_POS;
		sendCards_look.m_handCardsCount = m_hand_cards_count[0];
		sendCards_look.m_is2ChangZhu = m_is2ChangZhu ? 1 : 0;
		sendCards_look.m_nextSelectScorePos = m_firstSelectCardPos;
		memcpy(sendCards_look.m_totleScore, m_accum_score, sizeof(m_accum_score));
		notify_lookon(sendCards_look);
		
		//广播俱乐部会长名牌观战
		if (IsClubOwerLookOn())
		{
			memcpy(sendCards_look.m_clubAllHandCards, m_hand_cards, sizeof(m_hand_cards));
			notify_club_ower_lookon(sendCards_look);
		}

		//玩家0发牌日志
		LLOG_DEBUG("notify_user0:Send Cards To player0 cardCount=[%d], deskId=[%d]", sendCards[0].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < SANDAER_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player0.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[0].m_handCards[i], sendCards[0].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//玩家1发牌日志
		LLOG_DEBUG("notify_user1:Send Cards To player1 cardCount=[%d], deskId=[%d]", sendCards[1].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < SANDAER_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player1.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[1].m_handCards[i], sendCards[1].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//玩家2发牌日志
		LLOG_DEBUG("notify_user2:Send Cards To player2 cardCount=[%d], deskId=[%d]", sendCards[2].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < SANDAER_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player2.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[2].m_handCards[i], sendCards[2].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//玩家3发牌日志
		LLOG_DEBUG("notify_user3:Send Cards To player3 cardCount=[%d], deskId=[%d]", sendCards[3].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < SANDAER_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player3.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[3].m_handCards[i], sendCards[3].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//玩家4发牌日志
		LLOG_DEBUG("notify_user4:Send Cards To player4 cardCount=[%d], deskId=[%d]", sendCards[4].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < SANDAER_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player4.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[4].m_handCards[i], sendCards[4].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//发完牌设置游戏状态为叫牌分状态
		set_play_status(GAME_PLAY_SELECT_SCORE);

		LLOG_DEBUG("Send Cards Over ^_^... Start Send Cards Tick... deskId=[%d], playStatus=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		//m_tick_flag = true;
		//m_play_status_time.Now();

		return true;
	}

	bool has_opt_over(GAME_PLAY_STATE playState)
	{
		switch (playState)
		{
		case GAME_PLAY_BEGIN:
			break;
		case GAME_PLAY_SEND_CARD:
			break;
		case GAME_PLAY_SELECT_SCORE:
			for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
			{
				if (m_select_score[i] == -1)
				{
					return false;
				}
			}
			return true;
			break;
		case GAME_PLAY_SELECT_MAIN:
			break;
		case GAME_PLAY_BASE_CARDS:
			break;
		case GAME_PLAY_SELECT_FRIEND:
			break;
		case GAME_PLAY_OUT_CARD:
			break;
		case GAME_PLAY_END:
			break;
		default:
			break;
		}

		return false;
	}

	/*
	3打2：C->S 玩家选择叫牌分
	MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
	*/
	bool on_event_user_select_score(Lint pos, Lint selectScore)
	{
		if ((selectScore <= m_max_select_score && selectScore != 0)
			|| m_select_score[pos] != -1
			|| m_curPos != pos)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_select_score() Error!!! selectScore <= m_max_select_score Or Has Selected Or Is Not CurrPos... deskId=[%d], selectScore=[%d], currPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, selectScore, m_curPos, pos);
			return false;
		}

		m_select_score[pos] = selectScore;

		if (selectScore > m_max_select_score)
		{
			m_max_select_score = selectScore;
		}

		LLOG_ERROR("SanDaErGameHandler::on_event_user_select_score() Select Score Over Scuess^v^... deskId=[%d], selectMaxScore=[%d], zhuangPos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_max_select_score, m_zhuangPos);
		//如果都选择完了 或 当前玩家叫牌100分，则进入下一个环节
		if (has_opt_over(GAME_PLAY_SELECT_SCORE) || m_select_score[pos] == 100)
		{
			//没人叫牌，从新开始洗牌
			if (m_max_select_score == 0)
			{
				//广播玩家叫牌结果
				SanDaErS2CSelecScoreR selectScoreR;
				selectScoreR.m_pos = pos;
				selectScoreR.m_selectScore = selectScore;
				selectScoreR.m_currMaxScore = m_max_select_score;
				selectScoreR.m_isFinish = 0;
				selectScoreR.m_nextSelectScorePos = SANDAER_INVALID_POS; //pos+1;
				notify_desk(selectScoreR);

				//广播给观战玩家
				notify_lookon(selectScoreR);

				//广播俱乐部会长名牌观战
				notify_club_ower_lookon(selectScoreR);

				LLOG_ERROR("SanDaErGameHandler::on_event_user_select_score() Error!!! All Player Select 0 Score, So Restart Game ... deskId=[%d]",
					m_desk ? m_desk->GetDeskId() : 0);
				
				start_round(m_user_status);
				return false;
			}

			
			//叫牌结束，庄家拿底牌，选主
			set_play_status(GAME_PLAY_SELECT_MAIN);
			//当前操作玩家  庄  每轮第一个出牌玩家  是叫分最大者
			m_curPos = 0;
			int iTmpScore = 0;
			for (Lint i = 0; i < SANDAER_PLAYER_COUNT; i++)
			{
				if (m_select_score[i] > iTmpScore)
				{
					m_curPos = i;
					iTmpScore = m_select_score[i];
				}
			}

			//m_curPos = pos;
			m_zhuangPos = m_curPos;
			m_firstOutPos = m_curPos;

			m_firstSelectCardPos = m_zhuangPos;

			//广播玩家叫牌结果
			SanDaErS2CSelecScoreR selectScoreR;
			selectScoreR.m_pos = pos;
			selectScoreR.m_selectScore = selectScore;
			selectScoreR.m_currMaxScore = m_max_select_score;
			selectScoreR.m_isFinish = 0;
			selectScoreR.m_nextSelectScorePos = selectScore == 0 ? SANDAER_INVALID_POS : m_curPos; //pos+1;
			notify_desk(selectScoreR);

			//广播给观战玩家
			notify_lookon(selectScoreR);

			//广播俱乐部会长名牌观战
			notify_club_ower_lookon(selectScoreR);


			//通知庄家拿底牌和选主花色
			for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
			{
				SanDaErS2CBaseCardsAndSelectMainN baseCardN;

				LLOG_ERROR("SanDaErGameHandler::on_event_user_select_score() Select Score Over Scuess^v^...i = [%d], deskId=[%d],pos=[%d],m_zhuangPos = [%d], selectMaxScore=[%d], zhuangPos=[%d]",
					i,m_desk ? m_desk->GetDeskId() : 0, pos, m_zhuangPos, m_max_select_score, m_zhuangPos);

				baseCardN.m_postmd = i;
				baseCardN.m_maxSelectScore = m_max_select_score;
				baseCardN.m_zhuangPos = m_zhuangPos;
				//庄家拿底牌
				if (m_zhuangPos == i)
				{
					baseCardN.m_baseCardsCount = SANDAER_BASE_CARDS_COUNT;
					for(Lint j = 0; j < SANDAER_BASE_CARDS_COUNT; ++j)
						baseCardN.m_baseCards[j] = m_base_cards[j];

					//俱乐部会长名牌观战
					notify_club_ower_lookon(baseCardN);

					// 记录交牌开始时间
					if (1) 
					{
						m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_INIT;
						m_nTimeSecondZhuangReqesutJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_REQUEST_TIMEOUT;
						m_nTimeSecondZhuangRequstJiaoPai = LTime().Secs();
					}
				}
				notify_user(baseCardN, i);
			}

			//广播给观战玩家
			SanDaErS2CBaseCardsAndSelectMainN baseCardN_look;
			baseCardN_look.m_postmd = SANDAER_INVALID_POS;
			baseCardN_look.m_maxSelectScore = m_max_select_score;
			baseCardN_look.m_zhuangPos = m_zhuangPos;
			notify_lookon(baseCardN_look);

			
		}
		else
		{
			//获取下一个人的位置
			m_curPos = GetNextPos(m_curPos);

			//广播玩家叫牌结果
			SanDaErS2CSelecScoreR selectScoreR;
			selectScoreR.m_pos = pos;
			selectScoreR.m_selectScore = selectScore;
			selectScoreR.m_currMaxScore = m_max_select_score;
			selectScoreR.m_isFinish = 0;
			selectScoreR.m_nextSelectScorePos = m_curPos != m_firstSelectCardPos ? m_curPos : SANDAER_INVALID_POS; //pos+1;
			notify_desk(selectScoreR);

			//广播给观战玩家
			notify_lookon(selectScoreR);

			//广播俱乐部会长名牌观战
			notify_club_ower_lookon(selectScoreR);

			LLOG_ERROR("SanDaErGameHandler::on_event_user_select_score() Scuess^v^... deskId=[%d], selectScore=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, selectScore, pos);
		}

		//保存录像（玩家叫分）
		SaveUserOper(GAME_PLAY_SELECT_SCORE, pos, m_hand_cards_count[pos], m_hand_cards[pos], 4, m_base_cards, selectScore);

		return true;
	}

	/*
	3打2：C->S 玩家选主牌
	MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
	*/
	bool on_event_user_select_main(Lint pos, Lint mainColor)
	{
		if (pos != m_zhuangPos)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_select_main() Error!!! Select Main Pos Is Not Zhuang... deskId=[%d], zhuangPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos);
			return false;
		}

		//不允许打副，选择的主花色不是方梅红黑
		if (!m_gamelogic.IsValidColor(mainColor) && !m_isPlayFu)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_select_main() Error!!! Select Main Not Valid... deskId=[%d], zhuangPos=[%d], optPos=[%d], selectMain=[0x%2x]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos, mainColor);
			return false;
		}

		m_mainColor = m_gamelogic.GetCardColor(mainColor);

		//设置主花色
		m_gamelogic.SetMainColor(m_mainColor);

		SanDaErS2CSelectMainColorR selectMainR;
		selectMainR.m_pos = pos;
		selectMainR.m_selectMainColor = m_mainColor;

		notify_desk(selectMainR);

		//广播给观战玩家
		notify_lookon(selectMainR);

		//广播给俱乐部会长明牌观战
		notify_club_ower_lookon(selectMainR);

		LLOG_ERROR("SanDaErGameHandler::on_event_user_select_main() Sucess^v^... deskId=[%d], zhuangPos=[%d], optPos=[%d], selectMain=[0x%2x]",
			m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos, mainColor);

		//设置游戏状态为庄家埋底
		set_play_status(GAME_PLAY_BASE_CARDS);

		Lint nVideoCara[1];
		nVideoCara[0] = mainColor;

		//保存录像（玩家选主花色）
		SaveUserOper(GAME_PLAY_SELECT_MAIN, pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, nVideoCara, 0);
	}

	/*
	3打2：C->S 玩家埋底
	MSG_C_2_S_SANDAER_BASE_CARD = 62257
	*/
	bool on_event_user_base_cards(Lint pos, Lint baseCards[], Lint baseCardsCount)
	{
		//Lint nVideoCard[8] = {0};
		if (pos != m_zhuangPos)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_base_cards() Error!!! Base Cards Pos Is Not Zhuang... deskId=[%d], zhuangPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos);
			return false;
		}

		if (baseCardsCount != SANDAER_BASE_CARDS_COUNT)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_base_cards() Error!!! Base Cards Count Error... deskId=[%d], zhuangPos=[%d], baseCardsCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, baseCardsCount);
			return false;
		}

		//庄家手牌与抠底牌
		Lint t_handBaseCards[SANDAER_HAND_CARDS_COUNT_MAX];
		memset(t_handBaseCards, 0, sizeof(t_handBaseCards));

		for (Lint i = 0; i < SANDAER_HAND_CARDS_COUNT; ++i)
		{
			t_handBaseCards[i] = m_hand_cards[pos][i];
		}
		for (Lint i = SANDAER_HAND_CARDS_COUNT; i < SANDAER_HAND_CARDS_COUNT_MAX; ++i)
		{
			t_handBaseCards[i] = m_base_cards[i - SANDAER_HAND_CARDS_COUNT];
			//nVideoCard[i - SANDAER_HAND_CARDS_COUNT] = m_base_cards[i - SANDAER_HAND_CARDS_COUNT];
		}

		//玩家盖底牌是否成功标志
		bool t_flag = false;

		//埋底成功
		if (m_gamelogic.RemoveCard(baseCards, baseCardsCount, t_handBaseCards, SANDAER_HAND_CARDS_COUNT_MAX))
		{
			t_flag = true;

			m_hand_cards_count[pos] = SANDAER_HAND_CARDS_COUNT;
			memcpy(m_hand_cards[pos], t_handBaseCards, sizeof(Lint) * SANDAER_HAND_CARDS_COUNT);
			memcpy(m_base_cards, baseCards, sizeof(Lint) * SANDAER_BASE_CARDS_COUNT);

			for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
			{
				SanDaErS2CBaseCardR baseCardR;
				baseCardR.m_pos = pos;
				baseCardR.m_flag = 1;
				baseCardR.m_pokerTen = m_isPokertTen ? 1 : 0;
				if (i == pos)
				{
					baseCardR.m_handCardsCount = m_hand_cards_count[pos];
					memcpy(baseCardR.m_handCards, m_hand_cards[pos], sizeof(Lint) * m_hand_cards_count[pos]);
					memcpy(baseCardR.m_baseCards, m_base_cards, sizeof(Lint) * SANDAER_BASE_CARDS_COUNT);

					//广播给俱乐部会长名牌观战
					notify_club_ower_lookon(baseCardR);
				}
				notify_user(baseCardR, i);
			}
			/*for (int i=0; i<4; i++)
			{
			nVideoCard[4 + i] = m_base_cards[i];
			}*/
			
			//广播观战玩家
			SanDaErS2CBaseCardR baseCardR_look;
			baseCardR_look.m_pos = pos;
			baseCardR_look.m_flag = 1;
			baseCardR_look.m_pokerTen = m_isPokertTen ? 1 : 0;
			notify_lookon(baseCardR_look);

			//更改游戏阶段为选对家
			set_play_status(GAME_PLAY_SELECT_FRIEND);

			//保存录像（玩家埋底）
			SaveUserOper(GAME_PLAY_BASE_CARDS, pos, m_hand_cards_count[pos], m_hand_cards[pos],4, m_base_cards,0);
			
			return true;
		}

		//埋底失败
		LLOG_ERROR("SanDaErGameHandler::on_event_user_base_cards() Failed!!! Remove Base Cards Faild... deskId=[%d], optPos=[%d]", 
			m_desk ? m_desk->GetDeskId() : 0, pos);
		for(Lint i = 0; i < SANDAER_BASE_CARDS_COUNT; ++i)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_base_cards()...................selectBaseCard[%d]=[0x%02x|%2d]", 
				i, baseCards[i], baseCards[i]);
		}
		SanDaErS2CBaseCardR baseCardR;
		baseCardR.m_pos = pos;
		baseCardR.m_flag = 0;
		notify_desk(baseCardR);

		//广播给观战玩家
		notify_lookon(baseCardR);

		//广播给俱乐部会长名牌观战
		notify_club_ower_lookon(baseCardR);
		return false;
	}

	/*
	3打2：C->S 玩家选对家牌
	MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
	*/
	bool on_event_user_select_friend(Lint pos, Lint selectCard)
	{
		if (pos != m_zhuangPos)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_select_friend() Error!!! Select Friend Pos Is Not Zhuang... deskId=[%d], zhuangPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos);
			return false;
		}

		if (!m_gamelogic.IsValidFirendCard(selectCard, m_max_select_score))
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_select_friend() Error!!! Select Friend Cards Is Not Valid... deskId=[%d], zhuangPos=[%d], selectMaxScore=[%d], selectCard=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_max_select_score, selectCard);
			return false;
		}

		LLOG_ERROR("SanDaErGameHandler::on_event_user_select_friend() Sucess^v^... deskId=[%d], zhuangPos=[%d], selectMaxScore=[%d], selectCard=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_max_select_score, selectCard);

		m_friendZhuangPos = pos;
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			if (m_gamelogic.IncludeCard(m_hand_cards[i], m_hand_cards_count[i], selectCard))
			{
				m_friendZhuangPos = i;
				break;
			}
		}
		m_selectFriendCard = selectCard;
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			SanDaErS2CSelectFriendCardBC sendSelectFriend;
			sendSelectFriend.m_pos = pos;
			sendSelectFriend.m_selectFriendCard = selectCard;
			sendSelectFriend.m_nextOutCardPos = pos;


			if (i == m_friendZhuangPos)
			{
				sendSelectFriend.m_firendZhuangPos = m_friendZhuangPos;

				//广播给俱乐部会长名牌观战
				notify_club_ower_lookon(sendSelectFriend);
			}
			//更改游戏阶段为出牌
			set_play_status(GAME_PLAY_OUT_CARD);
			notify_user(sendSelectFriend, i);
		}
		std::vector<Lint> retCard;
		for (Lint j = 0; j < SANDAER_BASE_CARDS_COUNT; ++j)
		{
			retCard.push_back(m_base_cards[j]);
		}

		m_video.SetZhuang(retCard, m_zhuangPos, m_friendZhuangPos, m_max_select_score);

		Lint nvideoCard[1] = { 0 };
		nvideoCard[0] = selectCard;

		//保存录像（玩家选对家）
		SaveUserOper(GAME_PLAY_SELECT_FRIEND, pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, nvideoCard, m_friendZhuangPos);

		//广播给观战玩家
		SanDaErS2CSelectFriendCardBC sendSelectFriend_look;
		sendSelectFriend_look.m_pos = pos;
		sendSelectFriend_look.m_selectFriendCard = selectCard;
		sendSelectFriend_look.m_nextOutCardPos = pos;
		notify_lookon(sendSelectFriend_look);
	}

	/*
	3打2：C->S 玩家出牌
	MSG_C_2_S_SANDAER_OUT_CARD = 62261
	*/
	bool on_event_user_out_card(Lint pos, Lint outCard)
	{
		LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Player Out Card Run... desk_id=[%d], pos=[%d], outCard=[0x%2x|%2d], is_first_out=[%d]"
			, m_desk ? m_desk->GetDeskId() : 0, pos, outCard, outCard, pos == m_firstOutPos);

		if (pos != m_curPos)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Error!!! Out Card Pos Is Not CurrPos... deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
				m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
			return false;
		}
		
		//检测出的牌是否包含在手牌中
		if (!m_gamelogic.IncludeCard(m_hand_cards[pos], m_hand_cards_count[pos], outCard))
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Error!!! Out Card Is Not Include In Hand Cards... deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
				m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
			return false;
		}
		
		//跟出牌玩家
		if(pos != m_firstOutPos)
		{
			//校验跟出牌
			if (!m_gamelogic.EfficacyOutCard(outCard, m_hand_cards[pos], m_hand_cards_count[pos], m_out_cards[m_firstOutPos]))
			{
				LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Error!!! Fllow Out Card Is Not Valid... deskId=[%d], optPos=[%d], outCard=[0x%2x|%2d], firstOutPos=[%d], OutCard=[0x%2x|%2d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, outCard, outCard, m_firstOutPos, m_out_cards[m_firstOutPos], m_out_cards[m_firstOutPos]);
				
				//出牌不符合,需要返回客户端
				SanDaErS2COutCardBC outCardBC;
				outCardBC.m_pos = pos;
				outCardBC.m_mode = -1;
				memcpy(outCardBC.m_handCardCount, m_hand_cards_count, sizeof(m_hand_cards_count));
				notify_user(outCardBC, pos);

				return false;
			}
		}

		//将出的牌从手牌中删除
		Lint t_outCard[1];
		t_outCard[0] = outCard;
		if (!m_gamelogic.RemoveCard(t_outCard, 1, m_hand_cards[pos], m_hand_cards_count[pos]))
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Error!!! Remove Out Card Failed... deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
				m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
			return false;
		}

		//最后一轮出牌
		if (m_hand_cards_count[pos] == 1)
		{
			//Lint nOverState = 0;
			//将每个人的手牌复制到出牌中
			for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
			{
				if (i == m_firstOutPos)
				{
					m_out_cards[i] = outCard;
				}
				else
				{
					m_out_cards[i] = m_hand_cards[i][0];
				}
				m_hand_cards_count[i] = 0;
			}
			memset(m_hand_cards, 0, sizeof(m_hand_cards));

			//本轮闲家得分情况
			int t_thisTurnScore = 0;
			
			//闲家总得分
			int t_getTotleScore = 0;

			//判断最后一轮出牌的胜者,必定判断5家，没毛病
			m_wTurnWinner = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos);

			
			//本局闲家得分
			if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos)
			{
				//提取出牌中的分牌
				int t_turnScoreCardsCount = 0;
				int t_turnScoreCards[5];
				memset(t_turnScoreCards, 0, sizeof(t_turnScoreCards));
				t_turnScoreCardsCount = m_gamelogic.GetScoreCards(m_out_cards, 5, t_turnScoreCards);

				//将分牌复制到总得分牌中,更新分牌数量
				memcpy(&m_scoreCards[m_scoreCardsCount], t_turnScoreCards, sizeof(int) * t_turnScoreCardsCount);
				m_scoreCardsCount += t_turnScoreCardsCount;

				//计算本轮闲家得分
				t_thisTurnScore = m_gamelogic.GetCardsScore(t_turnScoreCards, t_turnScoreCardsCount);
			}

			//计算闲家总得分
			t_getTotleScore = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);

			//在此统一判断状态
			if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos)
			{
				if (m_max_select_score == 100)
				{
					nOverState = GAME_OVER_F;
				}
				else
				{
					nOverState = GAME_OVER_E;
				}
			}
			//庄家扣底
			else
			{
				if (t_getTotleScore == 0 && m_max_select_score == 100)
				{
					nOverState = GAME_OVER_D;
				}
				else if(t_getTotleScore == 0 && m_max_select_score < 100)
				{
					nOverState = GAME_OVER_C;
				}
				else
				{
					nOverState = GAME_OVER_B;
				}
			}
			
			//依次广播每个玩家的出牌消息
			for (int i = m_firstOutPos; i < m_player_count + m_firstOutPos; ++i)
			{
				SanDaErS2COutCardBC outCardBC;
				outCardBC.m_pos = i % m_player_count;
				outCardBC.m_outCard = m_out_cards[outCardBC.m_pos];
				outCardBC.m_nextPos = (i + 1) % m_player_count;
				outCardBC.m_thisTurnScore = t_thisTurnScore;
				outCardBC.m_getTotleScore = t_getTotleScore;
				outCardBC.m_scoreCardsCount = m_scoreCardsCount;
				outCardBC.m_soundType = -1;//最后一圈不要音效 GetSound();
				memcpy(outCardBC.m_scoreCards, m_scoreCards, sizeof(m_scoreCards));
				memcpy(outCardBC.m_handCardCount, m_hand_cards_count, sizeof(m_hand_cards_count));
				memcpy(outCardBC.m_outCardArray, m_out_cards, sizeof(m_out_cards));
				//第一个出牌
				if (i == m_firstOutPos)
				{
					outCardBC.m_turnStart = 1;
				}

				//最后一个出牌玩家
				else if (i == (m_player_count + m_firstOutPos - 1))
				{
					outCardBC.m_turnOver = 1;
					outCardBC.m_turnWiner = m_wTurnWinner;
					outCardBC.m_currBig = m_wTurnWinner;
					outCardBC.m_nextPos = SANDAER_INVALID_POS;
				}

				notify_desk(outCardBC);
				Lint voputcard[1] = { outCardBC.m_outCard };

				//保存录像（玩家出牌）
				SaveUserOper(GAME_PLAY_OUT_CARD, outCardBC.m_pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, voputcard, t_getTotleScore, (i == m_firstOutPos) ? -2 : outCardBC.m_soundType);

				//广播给观战玩家
				notify_lookon(outCardBC);

				//广播给俱乐部会长明牌观战
				notify_club_ower_lookon(outCardBC);
			}

			//设置下一轮第一个叫牌的人
			m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);

			m_over_state = (OVER_STATE)nOverState;

			finish_round();
			//CalcSore();
			//OnGameOver(WIN_INVALID, nOverState, 1);
			
		}
		//中间出牌
		else
		{
			//减少手牌数量
			m_hand_cards_count[pos] -= 1;

			//记录本人出的牌
			m_out_cards[pos] = outCard;

			int t_soundType = GetSound();

			//切换当前位置为下一个出牌玩家位置
			m_curPos = (pos + 1) % m_player_count;

			//如果下一个出牌玩家位置 与 第一个出牌玩家位置相同，则本轮出牌结束
			if (m_curPos == m_firstOutPos)
			{
				m_curPos = SANDAER_INVALID_POS;
			}

			//目前最大出牌玩家
			int t_currBigPos = SANDAER_INVALID_POS;

			SanDaErS2COutCardBC outCardBC;
			outCardBC.m_pos = pos;
			outCardBC.m_nextPos = m_curPos;
			outCardBC.m_outCard = outCard;
			outCardBC.m_soundType = t_soundType;
			memcpy(outCardBC.m_handCardCount, m_hand_cards_count, sizeof(m_hand_cards_count));
			memcpy(outCardBC.m_outCardArray, m_out_cards, sizeof(m_out_cards));
			//本轮第一个出牌玩家
			if (pos == m_firstOutPos)
			{
				outCardBC.m_turnStart = 1;
			}
			//跟出牌玩家
			else
			{
				t_currBigPos = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, GetHasOutPlayerCount());
				outCardBC.m_currBig = t_currBigPos;
			}

			Lint vScore = -1;
			//本轮最后一个玩家出牌：确定本轮赢家，闲家得分情况，下轮首出牌玩家
			if (m_curPos == SANDAER_INVALID_POS)
			{
				//计算本轮出牌的赢家
				m_wTurnWinner = t_currBigPos;

				//本轮闲家得分
				int t_thisTurnScore = 0;

				//提取出牌中的分牌
				int t_turnScoreCardsCount = 0;				//几张牌得分了
				int t_turnScoreCards[5];					//得分的牌有那些
				memset(t_turnScoreCards, 0, sizeof(t_turnScoreCards));
				t_turnScoreCardsCount = m_gamelogic.GetScoreCards(m_out_cards, 5, t_turnScoreCards);

				//本局闲家得分
				if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos)
				{

					//将分牌复制到总得分牌中,更新分牌数量
					memcpy(&m_scoreCards[m_scoreCardsCount], t_turnScoreCards, sizeof(int) * t_turnScoreCardsCount);
					m_scoreCardsCount += t_turnScoreCardsCount;

					//计算本轮闲家得分
					t_thisTurnScore = m_gamelogic.GetCardsScore(t_turnScoreCards, t_turnScoreCardsCount);
				}

				m_leftover_score -= m_gamelogic.GetCardsScore(t_turnScoreCards, t_turnScoreCardsCount);

				m_thisTurnScore = t_thisTurnScore;

				LLOG_DEBUG("SanDaErGameHandler::on_event_user_out_card() 一轮结束了2, deskId=[%d],m_thisTurnScore =[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_thisTurnScore, m_thisTurnScore);

				outCardBC.m_turnStart = 0;
				outCardBC.m_turnOver = 1;
				outCardBC.m_nextPos = m_wTurnWinner;
				outCardBC.m_turnWiner = m_wTurnWinner;
				outCardBC.m_thisTurnScore = t_thisTurnScore;
				outCardBC.m_leftover_score = m_leftover_score;

				m_curPos = m_wTurnWinner;		//本轮出牌的赢家为下轮出牌的第一个人

				outCardBC.m_getTotleScore = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);
				outCardBC.m_scoreCardsCount = m_scoreCardsCount;
				memcpy(outCardBC.m_scoreCards, m_scoreCards, sizeof(m_scoreCards));
				memcpy(outCardBC.m_prevOutCard, m_out_cards, sizeof(m_out_cards));
				memcpy(m_prev_out_cards, m_out_cards, sizeof(m_out_cards));

				//m_wTurnWinner = SANDAER_INVALID_POS;		//一轮出完了,最大的重置
				
				LLOG_DEBUG("SanDaErGameHandler::on_event_user_out_card() 一轮结束了1, deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
					m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
				//设置设置下轮出牌第一个出牌的玩家，清空本轮出的牌
				if (m_wTurnWinner != SANDAER_INVALID_POS && m_hand_cards[m_wTurnWinner] != 0)
				{
					m_firstOutPos = m_wTurnWinner;
					memset(m_out_cards, 0, sizeof(m_out_cards));
				}
				vScore = 0;
				vScore = m_thisTurnScore;
			}
			if (vScore == -1)
			{
				Lint voputcard[1] = { outCard };

				//保存录像（玩家出牌）
				SaveUserOper(GAME_PLAY_OUT_CARD, pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, voputcard, m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount), (pos == m_firstOutPos) ? -2 : t_soundType);
			}
			else
			{
				Lint voputcard[2] = { outCard,0 };

				//保存录像（玩家出牌）
				SaveUserOper(GAME_PLAY_OUT_CARD, pos, m_hand_cards_count[pos], m_hand_cards[pos], 2, voputcard, m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount), (pos == m_firstOutPos) ? -2 : t_soundType);
			}
			
			notify_desk(outCardBC);

			//广播给观战玩家
			notify_lookon(outCardBC);

			//广播给俱乐部明牌观战
			notify_club_ower_lookon(outCardBC);

			//判断下家得分是否大于105
			if ( (outCardBC.m_getTotleScore + m_max_select_score) >= SANDAER_WIN_SCORE_MAX && !m_b105)
			{
				for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
				{
					m_reset105[i] = -1;
				}

				m_b105 = true;
				//广播通知玩家是否结束战斗
				SanDaErS2CScore105 score105;
				score105.m_Score = outCardBC.m_getTotleScore + m_max_select_score;
				notify_desk(score105);

				//倒计时开始
				m_play_status_time.Now();
			}

			//二挖情况下，闲家抓50分强制结束游戏
			if (m_playTypeWa == 1 && m_selectWa == 1 && outCardBC.m_getTotleScore >= 50)
			{
				m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);				//设置下一轮第一个叫牌的人
				m_over_state = GAME_OVER_A;
				m_wTurnWinner = m_zhuangPos;
				finish_round();
			}

			//判断庄家是否可以摊牌
			if (0)
			{
				bool t_isTanPai = m_gamelogic.JudgeXianLose(m_hand_cards, m_hand_cards_count, m_player_count, m_zhuangPos, m_firstOutPos);

				if (t_isTanPai && m_desk->m_user[m_zhuangPos])
				{
					SanDaErS2CZhuangCanTanPai tanPai;
					tanPai.m_pos = m_zhuangPos;

					m_desk->m_user[m_zhuangPos]->Send(tanPai);
				}
			}
		}

		return true;

	} //bool on_event_user_out_card(Lint pos, Lint outCard) END!!!

	/*
	  3打2：C->S 客户端庄家选择交牌
	  MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62368
	*/
	bool on_event_zhuang_request_jiao_pai(Lint pos, bool bJiaoPai = false)
	{
		LLOG_DEBUG("User Request to jiaopai !!! Desk: %d pos:%d state:%d ", m_desk->GetDeskId(), pos, m_jiaoPaiState);
		if (pos != m_zhuangPos)
		{
			return false;
		}

		//检查交牌状态
		if (m_jiaoPaiState != JIAO_PAI_STATE::JIAOPAI_STATE_INIT)
		{
			LLOG_ERROR("ERROR jiaopai state!!! Desk: %d pos:%d state:%d ", m_desk->GetDeskId(), pos, m_jiaoPaiState);
			return false;
		}

		//广播通知闲家是否同意庄家交牌
		SanDaErS2CZhuangJiaoPaiBC jiaoPaiBC;
		Lint jiaopai_request = 1; //交
								  //庄家拒绝交牌，游戏继续
		if (!bJiaoPai)
		{
			jiaopai_request = 2;
			m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_END;
			m_nTimeSecondXianResponseJiaoPaiLeft = 0;
			m_nTimeSecondXianResponseJiaoPai = LTime().Secs();
			MHLOG_PLAYCARD("庄家取消交牌 desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
		}
		else
		{
			MHLOG_PLAYCARD("庄家申请交牌 desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
			for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
			{
				m_jiaoPai[i] = -1;
			}
			m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_REQUESTED; // 庄家已申请
			jiaopai_request = 1;
			m_jiaoPai[m_zhuangPos] = 1;
			//交牌标志
			m_bJiaoPai = true;
			m_nTimeSecondXianResponseJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_RESPONSE_TIMEOUT;
			m_nTimeSecondXianResponseJiaoPai = LTime().Secs();
		}

		jiaoPaiBC.m_jiaopaiState = m_jiaoPaiState;
		jiaoPaiBC.m_zhuangPos = m_zhuangPos;
		jiaoPaiBC.m_jiaopaiRequest = jiaopai_request;
		jiaoPaiBC.m_xianResponseLeft = m_nTimeSecondXianResponseJiaoPaiLeft;
		notify_desk(jiaoPaiBC);
		return true;
	}

	/*
	  3打2：C->S 非庄家客户端选择庄家交牌的申请
	  MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62370
	*/
	bool on_event_xian_select_jiao_pai(Lint pos, Lint selectRet)
	{
		if (selectRet != 0 && selectRet != 1)
		{
			return false;
		}

		if (pos == m_zhuangPos)
		{
			return false;
		}

		if (m_jiaoPaiState != JIAO_PAI_STATE::JIAOPAI_STATE_REQUESTED) //庄家已申请
		{
			LLOG_ERROR("ERROR jiaopai state!!! Desk: %d pos:%d state:%d ", m_jiaoPaiState, m_desk->GetDeskId(), pos);
			return false;
		}

		if (m_jiaoPai[pos] != -1)
		{
			return false;
		}

		//设置状态
		m_jiaoPai[pos] = selectRet;

		//广播消息
		SanDaErS2CJiaoPaiResult send;
		send.m_jiaoPaiSate = m_jiaoPaiState;
		send.m_zhuangPos = m_zhuangPos;
		memcpy(send.m_agree, m_jiaoPai, sizeof(m_jiaoPai));
		notify_desk(send);

		int iRet = 1;
		for (int i = 0; i < SANDAER_PLAYER_COUNT; i++)
		{
			if (i == m_zhuangPos)
			{
				continue;
			}

			if (m_jiaoPai[i] == 0)		//有人拒绝
			{
				iRet = 0;
				//清空交牌状态
				for (Lint n = 0; n < SANDAER_PLAYER_COUNT; ++n)
				{
					m_jiaoPai[n] = -2;
				}
				m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_XIAN_NOT_AGREE; //闲家已拒绝			
				break;
			}
			if (m_jiaoPai[i] == -1)		//等待中
			{
				iRet = 0;
				//break;
			}
		}
		if (iRet == 1) //全部同意
		{
			MHLOG_PLAYCARD("庄家交牌成功 desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
			m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_COMPLETE; //全部同意
			m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);		//设置下一轮第一个叫牌的人
			m_over_state = GAME_OVER_A;
			m_sandaer_overstate = SANDAER_OVER_STATE::DIPAI_ZHUANG_JIAOPAI;
			m_wTurnWinner = m_zhuangPos;
			//庄家交牌
			SaveUserOper(GAME_PLAY_STATE::GAME_PLAY_ZHUAGN_JIAOPAI, m_zhuangPos, 0, NULL);
			finish_round();
		}
		else if (iRet == 0)  // 有人拒绝或者闲家未响应
		{
		}
		return true;
	}

	//庄家摊牌
	bool on_event_zhuang_select_tan_pai(Lint pos, Lint selectRet)
	{
		if (pos != m_zhuangPos)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Error!!! Select TanPai Pos Is Not Zhuang... deskId=[%d], zhuangPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos);
			return false;
		}

		if (selectRet != 0 || selectRet != 1)
		{
			LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Error!!! Select TanPai Result Pos Is Not Zhuang... deskId=[%d], selectRet=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, selectRet, pos);
			return false;
		}

		LLOG_ERROR("SanDaErGameHandler::on_event_user_out_card() Run... deskId=[%d], selectRet=[%d], optPos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, selectRet, pos);

		SanDaErS2CTanPaiRet tanPaiRet;
		tanPaiRet.m_selectRet = selectRet;
		if (selectRet)
		{
			memcpy(tanPaiRet.m_tanPaiCount, m_hand_cards_count, sizeof(m_hand_cards_count));
			for (int i = 0; i < m_player_count; ++i)
			{
				memcpy(tanPaiRet.m_tanPai[i], m_hand_cards[i], sizeof(Lint) * SANDAER_HAND_CARDS_COUNT);
			}
		}

		notify_desk(tanPaiRet);

		if (selectRet)
		{
			m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);				//设置下一轮第一个叫牌的人
			m_over_state = GAME_OVER_G;
			m_wTurnWinner = m_zhuangPos;
			finish_round();
		}
	}


	//算分
	void CalcSore()
	{
		Lint  t_thisTurnScore = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);
		int  nZhuangMoney = 0;				//本轮庄家积分
		int  nViceZhuangMoney = 0;			//本轮副庄积分
		int  nXianMoney = 0;				//本轮闲家积分
											//计算本局玩家积分


		//庄交牌
		if (m_nJiaoPaiOption != 0 && m_sandaer_overstate == SANDAER_OVER_STATE::DIPAI_ZHUANG_JIAOPAI)
		{
			Lint this_score = 0;
			if (m_nJiaoPaiOption == 1)
			{
				this_score = 2;
			}
			else if (2 == m_nJiaoPaiOption)
			{
				this_score = 3;
			}
			this_score *= m_baseTimes;

			for (int i = 0; i < SANDAER_PLAYER_COUNT; i++)
			{
				if (i == m_zhuangPos)  continue;
				m_player_score[i] += this_score;
				m_player_score[m_zhuangPos] -= this_score;
			}

			for (int i = 0; i < SANDAER_PLAYER_COUNT; i++)
			{
				m_accum_score[i] += m_player_score[i];
			}

			LLOG_DEBUG("CalcScore...  desk_id=[%d],庄=[%d],副庄=[%d],庄叫分=[%d],底分=[%d],庄家积分=[%d],副庄积分=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_friendZhuangPos, m_max_select_score, m_baseTimes,
				m_player_score[m_zhuangPos], m_player_score[m_friendZhuangPos]);

			return;
		}

		if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos) //闲家扣底，不算抓分,得分翻倍
		{
			nZhuangMoney = ((m_max_select_score - 75) / 5) * -4;
			nViceZhuangMoney = ((m_max_select_score - 75) / 5) * -2;
			nXianMoney = ((m_max_select_score - 75) / 5) * 2;

			//三挖、闲家抠底、抓分50 再翻倍
			if (m_playTypeWa == 1 && m_selectWa == 2 && t_thisTurnScore >= 50)
			{
				nZhuangMoney = nZhuangMoney * 2;
				nViceZhuangMoney = nViceZhuangMoney * 2;
				nXianMoney = nXianMoney * 2;
			}

		}
		else  //庄家保底
		{
			int iTmpScore = m_max_select_score + t_thisTurnScore;		//庄家叫分+闲家得分
			if (iTmpScore >= 105)
			{
				if (m_max_select_score == 100 || (m_playTypeWa == 1 && t_thisTurnScore >= 50))  //叫分100，得分翻倍
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * -4;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * -2;
					nXianMoney = ((m_max_select_score - 75) / 5) * 2;
				}
				else  //叫分小于100，得分不翻倍
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * -2;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * -1;
					nXianMoney = ((m_max_select_score - 75) / 5) * 1;
				}
			}
			else if (iTmpScore < 105)
			{
				if (m_max_select_score == 100 || t_thisTurnScore == 0)  //叫分100，或者光头，得分翻倍
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * 4;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * 2;
					nXianMoney = ((m_max_select_score - 75) / 5) * -2;
				}
				else
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * 2;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * 1;
					nXianMoney = ((m_max_select_score - 75) / 5) * -1;
				}

				/*if (t_thisTurnScore > 0)
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * 2;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * 1;
					nXianMoney = ((m_max_select_score - 75) / 5) * -1;
				}
				else
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * 4;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * 2;
					nXianMoney = ((m_max_select_score - 75) / 5) * -2;
				}*/
			}

		}
		Lint base = m_playtype.SanDaErBaseTimes();

		nZhuangMoney = nZhuangMoney*base;
		nViceZhuangMoney = nViceZhuangMoney*base;
		nXianMoney = nXianMoney*base;

		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			if (i == m_zhuangPos)
			{
				m_player_score[m_zhuangPos] = nZhuangMoney;
				if (i == m_friendZhuangPos)
				{
					m_player_score[m_zhuangPos] = nXianMoney * -4;
				}
				m_accum_score[m_zhuangPos] += m_player_score[m_zhuangPos];
			}
			else if (i == m_friendZhuangPos)
			{
				m_accum_score[m_friendZhuangPos] += nViceZhuangMoney;
				m_player_score[m_friendZhuangPos] = nViceZhuangMoney;
			}
			else
			{
				m_accum_score[i] += nXianMoney;
				m_player_score[i] = nXianMoney;
			}
			//m_desk->m_vip->m_score[i] = m_accum_score[i];
		}
		LLOG_DEBUG("CalcScore...  desk_id=[%d],庄=[%d],副庄=[%d],庄叫分=[%d],闲家得分=[%d],底分=[%d],庄家积分=[%d],副庄积分=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_friendZhuangPos, m_max_select_score, t_thisTurnScore, base,
			m_player_score[m_zhuangPos], m_player_score[m_friendZhuangPos]);
	}

	/**/
	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	//结算
	void finish_round(BOOL jiesan = false)
	{
		//玩家解散房间，不发小结算消息
		if (true == jiesan)
		{
			LLOG_ERROR("ShuangShengGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());
			SanDaErS2CDrawResult jiesan;
			jiesan.m_isFinish = 1;
			jiesan.m_type = 1;

			memcpy(jiesan.m_totleScore, m_accum_score, sizeof(m_accum_score));
			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
			{
				if (m_desk->m_user[i])
				{
					jiesan.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
					jiesan.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
					jiesan.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
				}
			}

			notify_desk(jiesan);

			LLOG_ERROR("Game Jie San Over... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

			set_desk_state(DESK_WAIT);

			//结束游戏
			if (m_desk) m_desk->HanderGameOver(1);
			return;
		}

		//算分
		CalcSore();

		//保存录像（游戏结束）
		SaveUserOper(GAME_PLAY_END, -1, 0, nullptr, 0, nullptr, 0);

		//保存回访
		m_video.addUserScore(m_accum_score);
		if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
		{
			VideoSave();
		}

		//增加局数
		increase_round();

		add_round_log(m_player_score, m_wTurnWinner);

		//推小结算消息
		SanDaErS2CDrawResult rest;
		rest.m_zhuangPos = m_zhuangPos;
		rest.m_type = (m_round_offset >= m_round_limit) ? 1 : 0;
		rest.m_state = nOverState;
		rest.m_fuzhuangPos = m_friendZhuangPos;
		rest.m_jiaofen = m_max_select_score;
		rest.m_defen = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);;
		memcpy(rest.m_baseCards, m_base_cards, sizeof(Lint) * SANDAER_BASE_CARDS_COUNT);
		rest.m_isFinish = 0;
		rest.m_winnerPos = m_wTurnWinner;

		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			rest.m_score[i] = m_player_score[i];
			rest.m_totleScore[i] = m_accum_score[i];

			if (m_desk->m_user[i])
			{
				rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
				rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
				rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
			}
		}
		notify_desk(rest);

		//广播观战玩家
		notify_lookon(rest);

		//广播会长观战
		notify_club_ower_lookon(rest);

		set_desk_state(DESK_WAIT);

		m_desk->HanderGameOver(1);
	}

};


/* 三打二游戏处理器 */
struct SanDaErGameHandler : SanDaErGameCore
{
	/* 构造函数 */
	SanDaErGameHandler()
	{
		LLOG_DEBUG("ShuangSheng GameHandler Init Over...");
	}

	/* 给玩家发消息 */
	void notify_user(LMsg &msg, int pos) override
	{
		if (NULL == m_desk)
			return;
		if (pos < 0 || pos >= m_player_count)
			return;
		User *u = m_desk->m_user[pos];
		if (NULL == u)
			return;
		u->Send(msg);
	}

	/* 广播消息 */
	void notify_desk(LMsg &msg) override
	{
		if (NULL == m_desk)
			return;
		m_desk->BoadCast(msg);
	}

	/* 广播观战玩家 */
	void notify_lookon(LMsg &msg) override
	{
		if (NULL == m_desk)
		{
			return;
		}
		if (m_desk->m_desk_Lookon_user.size() == 0)
		{
			return;
		}
		m_desk->MHBoadCastDeskLookonUser(msg);
	}

	/* 广播俱乐部会长明牌观战 */
	void notify_club_ower_lookon(LMsg& msg) override
	{
		if (NULL == m_desk)
		{
			return;
		}
		if (m_desk->m_clubOwerLookOn == 1 && m_desk->m_clubOwerLookOnUser)
		{
			m_desk->MHBoadCastClubOwerLookOnUser(msg);
		}
	}

	/* 每局开始*/
	bool startup(Desk *desk) {
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}
	/* 每局结束*/
	void shutdown(void)
	{
		clear_match(0);
	}


	/* 设置玩法小选项，被Desk::SetVip()调用 */
	void SetPlayType(std::vector<Lint>& l_playtype)
	{
		// 被 Desk 的 SetVip 调用，此时能得到 m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		//小选项：底分
		m_baseTimes = m_playtype.SanDaErBaseTimes();

		//小选项：2是否为常主
		m_is2ChangZhu = m_playtype.SanDaEr2IsCommMain();

		//小选项：是否含十
		m_isPokertTen = m_playtype.SanDaEr2IsPokerTen();

		//小选项：是否开启专属防作弊
		m_cheatAgainst = m_playtype.SanDaEr2IsCheat();

		//小选项：是否允许打副
		m_isPlayFu = m_playtype.SanDaErIsPlayFu();

		//小选项：玩法  0-经典玩法  1-三挖子玩法
		m_playTypeWa = m_playtype.SanDaErPlayType();

		//小选项：几挖  0-不允许  1-二挖  2-三挖
		m_selectWa = m_playtype.SanDaErSelectWa();

		//小选项：是否允许交牌  0-不允许  1-8分交牌  2-12分交牌
		m_nJiaoPaiOption = m_playtype.SanDaErIsJiaoPai();

		
		//设置常主
		if (m_playtype.SanDaEr2IsCommMain())
		{
			m_gamelogic.SetCommMainValue(2);
		}

		//三挖子玩法默认为允许打副
		if (m_playTypeWa == 1)
		{
			m_isPlayFu = true;
		}
		m_gamelogic.SetPlayFu(m_isPlayFu);

		m_video.FillDeskBaseInfo(m_desk ? m_desk->GetDeskId() : 0, 0, m_round_limit, l_playtype);
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
		LLOG_DEBUG("SanDaErGameHandler::MHSetDeskPlay() Run... deskId=[%d], play_user_count=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("SanDaErGameHandler::MHSetDeskPlay() Error!!!!  m_desk or m_desk->m_vip is NULL");
			return;
		}

		m_player_count = play_user_count;

		//设置房主为第一个叫牌分的玩家
		if (m_round_offset == 0)
		{
			m_firstSelectCardPos = m_desk->MHSpecPersonPos();
		}

		start_round(player_status);
	}

	/*
	3打2：C->S 玩家选择叫牌分
	MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
	*/
	bool HandleSanDaErSelectScore(User* pUser, SanDaErC2SSelectScore* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectScore() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_SELECT_SCORE != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectScore() Error!!! Current Play Status is not GAME_PLAY_SELECT_SCORE... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectScore() Error!!! Select Score Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//处理玩家叫牌分
		on_event_user_select_score(GetUserPos(pUser), msg->m_selecScore);
	}

	/*
	3打2：C->S 玩家选主牌
	MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
	*/
	bool HandleSanDaErSelectMain(User* pUser, SanDaErC2SSelectMain* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectMain() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_SELECT_MAIN != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectMain() Error!!! Current Play Status is not GAME_PLAY_SELECT_MAIN... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectMain() Error!!! Select Main Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		on_event_user_select_main(GetUserPos(pUser), msg->m_selectMainColor);
		//
	}

	/*
	3打2：C->S 玩家埋底
	MSG_C_2_S_SANDAER_BASE_CARD = 62257
	*/
	bool HandleSanDaErBaseCard(User* pUser, SanDaErC2SBaseCard* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErBaseCard() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_BASE_CARDS != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErBaseCard() Error!!! Current Play Status is not GAME_PLAY_BASE_CARDS... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErBaseCard() Error!!! Base Cards Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//
		on_event_user_base_cards(GetUserPos(pUser), msg->m_baseCards, msg->m_baseCardsCount);
	}

	/*
	3打2：C->S 玩家选对家牌
	MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
	*/
	bool HandleSanDaErSelectFriend(User* pUser, SanDaErC2SSelectFriend* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectFriend() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_SELECT_FRIEND != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectFriend() Error!!! Current Play Status is not GAME_PLAY_SELECT_FRIEND... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErSelectFriend() Error!!! Select Friend Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//
		on_event_user_select_friend(GetUserPos(pUser), msg->m_selectFriendCard);
	}

	/*
	3打2：C->S 玩家出牌
	MSG_C_2_S_SANDAER_OUT_CARD = 62261
	*/
	bool HandleSanDaErOutCard(User* pUser, SanDaErC2SOutCard* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErOutCard() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_OUT_CARD != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErOutCard() Error!!! Current Play Status is not GAME_PLAY_OUT_CARD... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErOutCard() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//
		on_event_user_out_card(GetUserPos(pUser), msg->m_outCard);
	}

	/* 处理玩家断线重连 */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("SanDaErGameHandler::OnUserReconnect() Error!!! pUser or m_desk Is NULL...");
			return;
		}

		Lint pos = GetUserPos(pUser);

		if (SANDAER_INVALID_POS == pos && !(m_desk->MHIsLookonUser(pUser) || m_desk->m_clubOwerLookOnUser == pUser))
		{
			LLOG_ERROR("SanDaErGameHandler::OnUserReconnect() Error!!! Reconnect Pos is invial... desk_id=[%d], userId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		//Desk 已经发送
		notify_desk_match_state();

		LLOG_DEBUG("SanDaErGameHandler::OnUserReconnect() Run... deskId=[%d], userId=[%d], pos=[%d], curPos=[%d], zhuangPos=[%d], playStatus=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), pos, m_curPos, m_zhuangPos, m_play_status);

		SanDaErS2CReConnect reConnect;
		reConnect.m_pos = pos;
		reConnect.m_curPos = m_curPos;
		reConnect.m_zhuang = m_zhuangPos;
		reConnect.m_fuzhuangCard = m_selectFriendCard;
		reConnect.m_state = m_play_status;
		reConnect.m_selectMainColor = m_mainColor;
		reConnect.m_Monoy = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);// m_thisTurnScore;
		reConnect.m_max_select_score = m_max_select_score;
		reConnect.m_pokerTen = m_isPokertTen;
		if (pos == m_friendZhuangPos)
		{
			reConnect.m_fuzhuangPos = m_friendZhuangPos;
		}
		reConnect.m_leftover_score = m_leftover_score;
		reConnect.m_firstOutPos = m_firstOutPos;
		memcpy(reConnect.m_handCardsCountArr, m_hand_cards_count, sizeof(m_hand_cards_count));

		for (int i = 0; i < SANDAER_PLAYER_COUNT; i++)
		{
			reConnect.m_score[i] = m_accum_score[i];
			reConnect.m_outCard[i] = m_out_cards[i];
			reConnect.m_prevOutCard[i] = m_prev_out_cards[i];
		}

		for (int i = 0; i < 12; i++)
		{
			reConnect.m_scoreCards[i] = m_scoreCards[i];
		}

		for (int i = 0; i < SANDAER_HAND_CARDS_COUNT_MAX && pos != SANDAER_INVALID_POS; i++)
		{
			reConnect.m_handCards[i] = m_hand_cards[reConnect.m_pos][i];
		}
		//判断掉线玩家如果是庄家,且状态是选底牌,则加上底牌
		if ((m_play_status == GAME_PLAY_SELECT_MAIN || m_play_status == GAME_PLAY_BASE_CARDS)
			&& reConnect.m_pos == reConnect.m_zhuang)
		{
			for (Lint j = 0; j < SANDAER_BASE_CARDS_COUNT; ++j)
				reConnect.m_handCards[10 + j] = m_base_cards[j];
		}
		//选主埋低加入交牌状态
		if (m_play_status == GAME_PLAY_SELECT_MAIN)
		{
			reConnect.m_nJiaoPaiState = m_jiaoPaiState;
			reConnect.m_nTimeLeftJiaoPaiAck = m_nTimeSecondXianResponseJiaoPaiLeft;
			reConnect.m_nTimeLeftJiaoPaiRequest = m_nTimeSecondZhuangReqesutJiaoPaiLeft;
			memcpy(reConnect.m_jiaopaiAgree, m_jiaoPai, sizeof(int) * SANDAER_PLAYER_COUNT);
		}
		for (int i = 0; i<SANDAER_BASE_CARDS_COUNT; i++)
		{
			reConnect.m_baseCards[i] = m_base_cards[i];
		}

		reConnect.m_playType[0] = m_playtype.SanDaErBaseTimes();
		reConnect.m_playType[1] = m_playtype.SanDaEr2IsCommMain();
		reConnect.m_playType[2] = m_playtype.SanDaEr2IsPokerTen();
		//reConnect.m_playType[3] = m_playtype.SanDaEr2IsGPS();
		reConnect.m_playType[4] = m_playtype.SanDaEr2IsCheat();

		if (m_play_status == GAME_PLAY_SELECT_SCORE)		//如果是叫分阶段,则m_score传送已经叫的分数
		{
			for (int i = 0; i < SANDAER_PLAYER_COUNT; i++)
			{
				reConnect.m_score[i] = m_select_score[i];
			}
		}

		if (GAME_PLAY_OUT_CARD == m_play_status)
		{
			reConnect.m_currBig = GetHasOutPlayerCount() > 1 ? m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, GetHasOutPlayerCount()) : SANDAER_INVALID_POS;
		}

		if (SANDAER_INVALID_POS != pos)
		{
			reConnect.m_hand_cards_count = m_hand_cards_count[reConnect.m_pos];
			notify_user(reConnect, reConnect.m_pos);
		}
		else if(m_desk->MHIsLookonUser(pUser))
		{
			//推送给观战玩家
			pUser->Send(reConnect);
		}
		//广播给俱乐部会长明牌观战
		else if (IsClubOwerLookOn())
		{
			memcpy(reConnect.m_clubAllHandCards, m_hand_cards, sizeof(m_hand_cards));
			reConnect.m_fuzhuangPos = m_friendZhuangPos;
			notify_club_ower_lookon(reConnect);
		}

		//判断105消息
		if (pos < SANDAER_INVALID_POS && m_reset105[pos] != -2 && m_b105)	//有105消息  且未选择
		{
			//如果有人拒绝,要继续游戏,则不发送提示105
			bool bState = true;
			for (int i = 0; i<SANDAER_PLAYER_COUNT; i++)
			{
				if (m_reset105[i] == 0)//有玩家选择继续游戏
				{
					bState = false;
					break;
				}
			}

			/*if (bState == false)
			{
				LLOG_ERROR("SanDaErHandler::OnUserReconnect() 105 Run... Now Has Player Select Contiue This Round Game... deskId=[%d], pos=[%d], m_reset105[5]=[%d, %d, %d, %d, %d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_reset105[0], m_reset105[1], m_reset105[2], m_reset105[3], m_reset105[4]);
			}*/

			if (bState)
			{
				/*LLOG_ERROR("SanDaErHandler::OnUserReconnect() 105 Run... Has No Player Select Contiue This Round Game... deskId=[%d], pos=[%d], m_reset105[5]=[%d, %d, %d, %d, %d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_reset105[0], m_reset105[1], m_reset105[2], m_reset105[3], m_reset105[4]);*/

				if (m_reset105[pos] == -1)  //已经通知,未操作
				{
					LTime t_time;
					t_time.Now();

					//广播通知玩家是否结束战斗
					SanDaErS2CScore105 score105;
					score105.m_Score = 105;
					score105.m_resTime = 15 - (t_time.Secs() - m_play_status_time.Secs());
					notify_user(score105, pos);
				}

				if (m_reset105[pos] == 1 || m_reset105[pos] == 0)  //已经同意或已经拒绝 ,通知结果
				{
					SanDaErS2CScore105Result score105Result;
					memcpy(score105Result.m_agree, m_reset105, sizeof(m_accum_score));
					notify_user(score105Result, pos);
				}
			}
		}

		//判断是或是小结阶段
		if (m_play_status == GAME_PLAY_SETTLEMENT)
		{
			//发送小结消息
			SanDaErS2CDrawResult rest;
			rest.m_zhuangPos = m_zhuangPos;
			rest.m_type = 0;
			rest.m_state = m_over_state;
			if(pos == m_friendZhuangPos)
			{ 
				rest.m_fuzhuangPos = m_friendZhuangPos;
			}
			rest.m_jiaofen = m_max_select_score;
			rest.m_defen = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);;
			memcpy(rest.m_baseCards, m_base_cards, sizeof(Lint) * SANDAER_BASE_CARDS_COUNT);

			for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
			{
				rest.m_score[i] = m_player_score[i];
			}
			notify_desk(rest);
		}
	}

	/*
	3打2：C->S 玩家回复闲家得分慢105分,是否停止游戏
	MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
	*/
	bool HandleSanDaErScore105Ret(User* pUser, SanDaErC2SScore105Ret* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_OUT_CARD != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! Current Play Status is not GAME_PLAY_OUT_CARD... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		Lint pos = GetUserPos(pUser);
		if (pos == SANDAER_INVALID_POS || pos != msg->m_pos)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, pos);
			return false;
		}


		if (msg->m_agree != 0 && msg->m_agree != 1)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! Opt Select is invial, deskId=[%d], userId=[%d] userPos=[%d], agree=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, pos, msg->m_agree);
			return false;
		}

		SanDaErS2CScore105Result score105Result;
		m_reset105[pos] = msg->m_agree;
		/*LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Run... Opt Pos=[%d], agree=[%d], deskId=[%d]", 
			pos, msg->m_agree, m_desk ? m_desk->GetDeskId() : 0);

		LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Run...deskId=[%d], m_reset105[5]=[%d, %d, %d, %d, %d]", 
			m_desk ? m_desk->GetDeskId() : 0, m_reset105[0], m_reset105[1], m_reset105[2], m_reset105[3], m_reset105[4]);*/

		memcpy(score105Result.m_agree, m_reset105, sizeof(m_accum_score));
		notify_desk(score105Result);

		int iRet = 1;
		for (int i = 0; i < SANDAER_PLAYER_COUNT; i++)
		{
			if (m_reset105[i] == 0)		//拒绝,继续游戏
			{
				iRet = 0;
				for (Lint n = 0; n < SANDAER_PLAYER_COUNT; ++n)
				{
					m_reset105[n] = -2;
				}
				break;
			}
			if (m_reset105[i] == -1)		//等待中
			{
				iRet = 0;
				//break;
			}
			if (m_reset105[i] == -2)
			{
				iRet = 0;
			}
		}
		if (iRet == 1) //全部同意
		{
			/*LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Run... All Player Select Over This Round Game... deskId=[%d], m_reset105[5]=[%d, %d, %d, %d, %d]",
				m_desk ? m_desk->GetDeskId() : 0, m_reset105[0], m_reset105[1], m_reset105[2], m_reset105[3], m_reset105[4]);*/

			m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);				//设置下一轮第一个叫牌的人
			m_over_state = GAME_OVER_A;
			m_wTurnWinner = m_zhuangPos;
			finish_round();
		}
	}

	/* 房间解散成功，desk调用该方法 */
	void OnGameOver(Lint result, Lint winpos, Lint bombpos)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL) {
			LLOG_ERROR("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("ShuangShengGameHandler::OnGameOver() Run...Desk game over. deskId=[%d] round_limit=[%d] round=[%d] dismiss=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);

		//解散房间，游戏结束
		finish_round(true);
	}

	/* 处理游戏结束(解散) */
	//void OnGameOver(Lint result, Lint winpos, Lint bombpos)
	//{
	//	//保存录像（游戏结束）
	//	SaveUserOper(GAME_PLAY_END, result, 0, nullptr, 0, nullptr, 0);

	//	//保存回访
	//	m_video.addUserScore(m_accum_score);
	//	if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
	//	{ 
	//		VideoSave();
	//	}

	//	// 保存结果
	//	if (m_desk && m_desk->m_vip)
	//	{
	//		m_desk->m_vip->AddLogForQiPai(m_desk->m_user, m_player_score, m_user_status, m_zhuangPos, m_video.m_Id);
	//	}
	//	if (WIN_DISS != result)
	//	{
	//		SanDaErS2CDrawResult rest;
	//		rest.m_zhuangPos = m_zhuangPos;
	//		rest.m_type = 0;
	//		rest.m_state = winpos;
	//		rest.m_fuzhuangPos = m_friendZhuangPos;
	//		rest.m_jiaofen = m_max_select_score;
	//		rest.m_defen = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);;
	//		memcpy(rest.m_baseCards, m_base_cards, sizeof(Lint) * SANDAER_BASE_CARDS_COUNT);

	//		//Lint		m_round_offset;							// 当前局数
	//		//Lint		m_round_limit;							// 创建房间选择的最大局数
	//		if (m_round_offset >= m_round_limit)
	//		{
	//			//rest.m_type = 1;
	//		}

	//		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
	//		{
	//			rest.m_score[i] = m_player_score[i];
	//		}
	//		notify_desk(rest);

	//		//广播观战玩家
	//		notify_lookon(rest);

	//		set_desk_state(DESK_WAIT);
	//		increase_round();
	//		clear_round();
	//	}
	//	

	//	if (m_desk)
	//		m_desk->HanderGameOver(1);
	//}

	/*
	C-S 庄家选择是否摊牌
	MSG_C_2_S_SANDAER_ZHUANG_SELECT_TAN_PAI = 62271
	*/
	bool HandleSanDaErZhuangSelectTanPai(User* pUser, SanDaErC2SZhuangSelectTanPai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErZhuangSelectTanPai() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_OUT_CARD != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErZhuangSelectTanPai() Error!!! Current Play Status is not GAME_PLAY_OUT_CARD... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErZhuangSelectTanPai() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		if (msg->m_pos != m_zhuangPos)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErZhuangSelectTanPai() Error!!! zhuangpos is error, deskId=[%d], userId=[%d] msgPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, msg->m_pos);
			return false;
		}

		return on_event_zhuang_select_tan_pai(msg->m_pos, msg->m_selectRet);
	}

	/*
	3打2：C->S 庄家请求底牌
	MSG_C_2_S_USER_GET_BASECARDS = 61027,		//
	*/
	bool HandleSanDaErGetBaseCards(User* pUser, SanDaErC2SGetBaseCards* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_OUT_CARD != m_play_status)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! Current Play Status is not GAME_PLAY_OUT_CARD... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		if (msg->m_pos != m_zhuangPos)
		{
			LLOG_ERROR("SanDaErHandler::HandleSanDaErScore105Ret() Error!!! zhuangpos is error, deskId=[%d], userId=[%d] msgPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, msg->m_pos);
			return false;
		}

		SanDaErS2CGetBaseCardsRet rest;
		rest.m_zhuang_pos = m_zhuangPos;
		memcpy(rest.m_baseCards, m_base_cards, sizeof(Lint) * SANDAER_BASE_CARDS_COUNT);
		notify_user(rest, m_zhuangPos);
	}

	/*
	3打2：C->S 客户端庄家选择交牌
	MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62368
	*/
	bool HandleSanDaErZhuangRequestJiaoPai(User* pUser, SanDaErC2SZhuangJiaoPai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenZhuangRequestJiaoPai() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		MHLOG_PLAYCARD("庄家交牌消息 desk: %d zhuangpos:%d code:%d", m_desk->GetDeskId(), m_zhuangPos, msg->m_agree);
		//交牌
		if (GAME_PLAY_SELECT_MAIN != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenZhuangRequestJiaoPai() Error!!! Current Play Status is not GAME_PLAY_SELECT_MAIN... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		Lint pos = GetUserPos(pUser);
		if (pos == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenZhuangRequestJiaoPai() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		bool bJiaoPai = false;
		if (msg->m_agree == 1) bJiaoPai = true;

		return on_event_zhuang_request_jiao_pai(pos, bJiaoPai);
	}

	/*
	C->S 非庄家  客户端选择庄家交牌的申请
	MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62370
	*/
	bool HandleSanDaErXianSelectJiaoPai(User* pUser, SanDaErC2SXianSelectJiaoPai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenXianSelectJiaoPai() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_SELECT_MAIN != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenXianSelectJiaoPai() Error!!! Current Play Status is not GAME_PLAY_SELECT_MAIN... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SANDAER_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenXianSelectJiaoPai() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		return on_event_xian_select_jiao_pai(GetUserPos(pUser), msg->m_agree);
	}

	// 检查交牌时间过期
	void Tick(LTime& curr)
	{
		Lint t = 0;
		if ((m_nJiaoPaiOption != 0) && (m_play_status == GAME_PLAY_SELECT_MAIN))
		{
			if (m_jiaoPaiState == JIAO_PAI_STATE::JIAOPAI_STATE_REQUESTED) //庄家已申请,   默认同意
			{
				t = curr.Secs() - m_nTimeSecondXianResponseJiaoPai;
				if (t > GAME_DEFAULT_TIME::JIAOPAI_RESPONSE_TIMEOUT) // 超出30秒
				{
					m_nTimeSecondXianResponseJiaoPaiLeft = 0;
					for (int i = 0; i < SANDAER_PLAYER_COUNT; i++)
					{
						//庄自己不用检查
						if (i == m_zhuangPos) continue;
						if (m_jiaoPai[i] == -1)
						{
							MHLOG_PLAYCARD("响应交牌超时,拒绝交牌 desk: %d zhuangpos:%d, userpos:%d", m_desk->GetDeskId(), m_zhuangPos, i);
							on_event_xian_select_jiao_pai(i, 0);
							break;
						}
					}
				}
				else
				{
					m_nTimeSecondXianResponseJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_RESPONSE_TIMEOUT - t;
				}
			}
			else if (m_jiaoPaiState == JIAO_PAI_STATE::JIAOPAI_STATE_INIT) //该状态下检查庄家超时未选，默认不交牌
			{
				t = curr.Secs() - m_nTimeSecondZhuangRequstJiaoPai; // 请求交牌
				if (t > GAME_DEFAULT_TIME::JIAOPAI_REQUEST_TIMEOUT) // 超出30秒
				{
					m_nTimeSecondZhuangReqesutJiaoPaiLeft = 0;
					m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_RQUEST_TIMEOUT;
					// 推庄家继续游戏
					MHLOG_PLAYCARD("庄家交牌请求超时,拒绝交牌 desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
					SanDaErS2CZhuangJiaoPaiBC bc;
					bc.m_jiaopaiState = m_jiaoPaiState;
					bc.m_jiaopaiRequest = 0;
					bc.m_zhuangPos = m_zhuangPos;
					notify_desk(bc);
				}
				else
				{
					m_nTimeSecondZhuangReqesutJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_REQUEST_TIMEOUT - t;
				}
			}
		}
	}
	
};


DECLARE_GAME_HANDLER_CREATOR(107 , SanDaErGameHandler);