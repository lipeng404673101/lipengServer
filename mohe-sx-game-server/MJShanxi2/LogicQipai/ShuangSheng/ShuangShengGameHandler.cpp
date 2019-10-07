#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "ShuangShengGameLogic.h"
#include "LMsgL2C.h"

/*配牌文件的文件名，card.ini*/
#define DEBUG_CARD_FILE_SHUANGSHENG "card_shuangsheng.ini"

//配牌用，解析打几
static int LoadGradeCard(std::vector<Lint>& grade, const char* conFileName)
{
	if (conFileName == NULL || std::strlen(conFileName) == 0)
	{
		return -1;
	}
	std::string fileName(conFileName);
	LIniConfig config;
	config.LoadFile(conFileName);

	std::string gradeString = config.GetString(Lstring("grade"), "");
	std::vector<std::string> gradeStringVec = splitString(gradeString, ",", true);

	if (gradeStringVec.size() > 0)
	{
		grade.clear();
		for (auto it = gradeStringVec.begin(); it < gradeStringVec.end(); it++)
		{
			char *str;
			int i = (int)strtol((*it).c_str(), &str, 16);
			BYTE nValue = (BYTE)i;
			grade.push_back(nValue);
		}
	}
	return 0;
}

//双生Debug配牌使用
static bool debugSendCards_Shuangsheng(BYTE* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
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
		BYTE* t_cbRandCard = new BYTE[cellPackCount];
		memset(t_cbRandCard, 0, sizeof(BYTE) * cellPackCount);

		//解析card.ini得到的配牌
		std::vector<BYTE> t_sendCards;

		//解析牌后每个人配牌的合法张数
		size_t* t_sendCount = new size_t[playerCount];
		memset(t_sendCount, 0, sizeof(size_t) * playerCount);

		//给每个玩家解析有效的配牌
		for (int i = 0; i < playerCount; ++i)
		{
			t_sendCards.clear();
			//解析个人玩家手牌
			LoadPlayerCards(t_sendCards, i + 1, conFileName);
			size_t t_count = 0;

			for (int j = 0; j < t_sendCards.size(); ++j)  //配的牌vector
			{
				for (int k = 0; k < cellPackCount; ++k)  //两副牌
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

		memcpy(cbRandCard, t_cbRandCard, sizeof(BYTE) * cellPackCount);

		if (t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_cbRandCard);
		t_sendCount = NULL;

		if(fp) fclose(fp);
		fp = NULL;
		return true;
	}

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}

/* 游戏中的各个阶段（枚举） */
static enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = -1,
	GAME_PLAY_SEND_CARDS = 1,	//发牌阶段
	GAME_PLAY_SELECT_ZHU = 2,	//报主
	GAME_PLAY_FAN_ZHU = 3,		//反主
	GAME_PLAY_BASE_CARD = 4,	//埋底阶段
	GAME_PLAY_PLAY_CARD = 5,	//出牌阶段
	GAME_PLAY_END = 6,			//出牌结束
};

/* 默认操作时间 */
static enum GAME_DEFAULT_TIME
{
	DEF_TIME_NULL = 0,
	DEF_TIME_SEND_CARDS = 10,
	DEF_TIME_SELECT_ZHU = 12,
	DEF_TIME_BASE_CARDS = 30,
	DEF_TIME_OUT_CARDS = 30,
};

/* 双升游戏小选项(结构体) */
struct PlayType
{
protected:
	std::vector<Lint>	m_playtype;  //游戏小选项
									 //Index ------- 0: 玩法模式 1:2常主 

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
	* 函数名：   ShuangShengBaseScore()
	* 描述：     获取客户端选择的底分
	* 详细：     玩法模式：1、1分，2、2分，3、3分
	* 返回：     (Lint)客户端选择的底分
	********************************************************/
	Lint ShuangShengBaseTimes() const;

	/******************************************************
	* 函数名：   ShuangShengRandZhu()
	* 描述：     获取客户端选择第一局随机主牌
	* 详细：     玩法模式：0-2或3开始主，1、随机主
	* 返回：     (Lint)客户端选择的是否为随机主
	********************************************************/
	bool ShuangShengIsRandZhu() const;

	/******************************************************
	* 函数名：   ShuangShengAntiCheating()
	* 描述：     获取客户端选择是否开启防作弊选项
	* 详细：     玩法模式：0-不开启、1、开启防作弊
	* 返回：     (Lint)客户端选择的玩法模式
	********************************************************/
	bool ShuangShengIsAntiCheating() const;

	/******************************************************
	* 函数名：   ShuangSheng2IsChangZhu()
	* 描述：     获取客户端选择的2是否为常主
	* 详细：     2是否为常主：false-2不是常主  true-2是常主
	* 返回：     (bool)客户端选择的2是否为常主
	********************************************************/
	bool ShuangSheng2IsChangZhu() const;



	/******************************************************
	* 函数名：   ShuangShengPlayMode()
	* 描述：     获取客户端选择的玩法模式
	* 详细：     玩法模式：0-经典模式、1、抄底模式
	* 返回：     (Lint)客户端选择的玩法模式
	********************************************************/
	Lint ShuangShengPlayMode() const;

	/******************************************************
	* 函数名：   ShuangShengPackCount()
	* 描述：     获取客户端选择牌的副数
	* 详细：     副数：1：1副牌，2：2副牌...
	* 返回：     (int)客户端选择牌副数
	********************************************************/
	Lint ShuangShengPackCount() const;
};





/******************************************************
* 函数名：   ShuangShengBaseScore()
* 描述：     获取客户端选择的底分
* 详细：     玩法模式：1、1分，2、2分，3、3分
* 返回：     (Lint)客户端选择的底分
********************************************************/
Lint PlayType::ShuangShengBaseTimes() const
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
* 函数名：   ShuangShengRandZhu()
* 描述：     获取客户端选择第一局随机主牌
* 详细：     玩法模式：2-2或3开始主，1、随机主
* 返回：     (Lint)客户端选择的是否为随机主
********************************************************/
bool PlayType::ShuangShengIsRandZhu() const
{
	if (m_playtype.size() < 2)
	{
		return 1;
	}
	return m_playtype[1] == 1;
}

/******************************************************
* 函数名：   ShuangShengAntiCheating()
* 描述：     获取客户端选择是否开启防作弊选项
* 详细：     玩法模式：0-不开启、1、开启防作弊
* 返回：     (Lint)客户端选择的玩法模式
********************************************************/
bool PlayType::ShuangShengIsAntiCheating() const
{
	if (m_playtype.size() < 3)
	{
		return false;
	}
	return m_playtype[2] == 1;
}

/******************************************************
* 函数名：   ShuangSheng2IsChangZhu()
* 描述：     获取客户端选择的2是否为常主
* 详细：     2是否为常主：false-2不是常主  true-2是常主
* 返回：     (bool)客户端选择的2是否为常主
********************************************************/
bool PlayType::ShuangSheng2IsChangZhu() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}

	return m_playtype[3] == 1;
}






/******************************************************
* 函数名：   ShuangShengPlayMode()
* 描述：     获取客户端选择的玩法模式
* 详细：     玩法模式：0-经典模式、1、抄底模式
* 返回：     (Lint)客户端选择的玩法模式
********************************************************/
Lint PlayType::ShuangShengPlayMode() const
{
	if (m_playtype.empty())
	{
		return 0;
	}
	return m_playtype[0];
}

/******************************************************
* 函数名：   ShuangShengPackCount()
* 描述：     获取客户端选择牌的副数
* 详细：     副数：1：1副牌，2：2副牌...(暂时默认两副牌)
* 返回：     (int)客户端选择牌副数
********************************************************/
Lint PlayType::ShuangShengPackCount() const
{
	return 2;
}

//////////////////////////////////////////牌局变量////////////////////////////////////////////

/* 双升每局不会初始化字段（结构体）*/
struct ShuangShengRoundState__c_part
{
	Lint		m_curPos;						                   // 当前操作玩家
	Lint		m_firstOutPos;										//每轮第一个出牌玩家
	Lint		m_wTurnWinner;										//每轮出牌的胜利玩家
	Lint		m_play_status;                                     // 牌局状态
	Lint		m_user_status[SHUANGSHENG_PLAY_USER_COUNT];        // 用户状态：改位置上是否坐人了（实际有效位位置）
	BOOL        m_tick_flag;                                       // 定时器开关

	//Lint		m_curGrade;										   // 当前级数
	BYTE		m_mainColor;										//主牌花色
	BYTE		m_mainValue;										//主牌数值
	BYTE		m_logicMainCard;									//逻辑主牌，如果无主，则为64

	bool		m_hasSelectZhu[4];									//叫牌标志
	BYTE		m_selectCard;										//玩家叫的牌
	BYTE		m_selectCardCount;									//叫牌数量
	Lint		m_selectCardPos;									//叫牌玩家位置

	BYTE		m_base_cards_count;									//底牌数量
	BYTE		m_base_cards[SHUANGSHENG_BASE_CARDS_COUNT];			//底牌

	BYTE		m_hand_cards_count[SHUANGSHENG_PLAY_USER_COUNT];	//各个玩家手牌数量
	BYTE		m_hand_cards[4][SHUANGSHENG_MAX_HAND_CARDS_COUNT];	//玩家手牌

	BYTE		m_out_cards_count[SHUANGSHENG_PLAY_USER_COUNT];		//玩家出牌数
	BYTE		m_out_cards[4][SHUANGSHENG_HAND_CARDS_COUNT];	//玩家每轮出的牌
	BYTE		m_cbLastTurnCard[4][SHUANGSHENG_HAND_CARDS_COUNT];		//上次出牌
	BYTE		m_cbLastTurnCount[4];								//上次出牌数目

	BYTE		m_cbScoreCardCount;									//得分扑克数目
	BYTE		m_cbScoreCardData[12 * SHUANGSHENG_PACK_COUNT];	    //得分扑克
	Lint		m_player_score[SHUANGSHENG_PLAY_USER_COUNT];        //玩家本局得分

	/* 清零每局字段 */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_curPos = SHUANGSHENG_INVALID_POS;
		m_firstOutPos = SHUANGSHENG_INVALID_POS;
		m_wTurnWinner = SHUANGSHENG_INVALID_POS;
		m_selectCardPos = SHUANGSHENG_INVALID_POS;
		m_mainColor = 0x40;
		m_logicMainCard = 0x40;
	}
};

/* 双升每局会初始化字段（结构体）*/
struct ShuangShengRoundState__cxx_part
{
	LTime m_play_status_time;

	/* 清零每局字段 */
	void clear_round()
	{
		this->~ShuangShengRoundState__cxx_part();
		new (this) ShuangShengRoundState__cxx_part;
	}
};

/* 双升每场不会初始化的字段（结构体）*/
struct ShuangShengMatchState__c_part
{
	Desk*		m_desk;           // 桌子对象
	Lint		m_round_offset;   // 当前局数
	Lint		m_round_limit;    // 创建房间选择的最大局数
	Lint		m_accum_score[SHUANGSHENG_PLAY_USER_COUNT];  //存储玩家总得分
	Lint		m_gradeCard[2];	 //两队级数（打几）（默认2不是常主）
	BOOL		m_dismissed;
	int			m_registered_game_type;
	int			m_player_count;   // 玩法核心代码所使用的玩家数量字段
	Lint		m_zhuangPos;      // 庄家位置
	bool		m_isFirstFlag;    // 首局显示开始按钮,true代表首局
	int			m_baseCardCount;  //底牌数量

	////玩法小选项
	int			m_baseTimes;		//底分倍数
	bool		m_isRandZhu;		//是否随机主 true:随机主，false:从2或3开始打
	bool		m_isAntiCheating;	//是否开启防作弊 true:开启防作弊  false:不开启防作弊
	bool		m_is2ChangZhu;		//2是否为常主，true：2是常主  false：2不是常主

	int			m_playMode;			//玩法模式，0：经典玩法  1：抄底模式
	int			m_packCount;		//牌副数

	/* 清零结构体字段 */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));

		//默认2不是常主，从2开始打
		m_gradeCard[0] = 2;  
		m_gradeCard[1] = 2;

		m_baseTimes = 1;
		m_isRandZhu = 0;
		m_packCount = 2;
		m_zhuangPos = SHUANGSHENG_INVALID_POS;
	}
};

/* 双升每场会初始化的字段（结构体）*/
struct ShuangShengMatchState__cxx_part
{
	SSGameLogic    m_gamelogic;     // 游戏逻辑
	PlayType       m_playtype;	    // 桌子玩法小选项

									/* 清空每场结构体 */
	void clear_match()
	{
		this->~ShuangShengMatchState__cxx_part();
		new (this) ShuangShengMatchState__cxx_part;
	}
};


/*
*  双升每局所需要的所有字段（结构体）
*  继承 ：ShuangShengRoundState__c_pard, ShuangShengRoundState_cxx_part
*/
struct ShuangShengRoundState : ShuangShengRoundState__c_part, ShuangShengRoundState__cxx_part
{
	void clear_round()
	{
		ShuangShengRoundState__c_part::clear_round();
		ShuangShengRoundState__cxx_part::clear_round();
	}
};


/*
*  双升每场所需要的所有字段（结构体）
*  继承：ShuangShengMatchState__c_pard, ShuangShengMatchState_cxx_pard
*/
struct ShuangShengMatchState : ShuangShengMatchState__c_part, ShuangShengMatchState__cxx_part
{
	void clear_match()
	{
		ShuangShengMatchState__c_part::clear_match();
		ShuangShengMatchState__cxx_part::clear_match();
	}
};

/*
*  双升桌子状态（结构体）
*  继承：ShuangShengRoundState, NiuNiuMatchState
*/
struct ShuangShengDeskState : ShuangShengRoundState, ShuangShengMatchState
{

	/* 清零每局数据 */
	void clear_round()
	{
		ShuangShengRoundState::clear_round();
	}

	/* 清零每场数据*/
	void clear_match(int player_count)
	{
		ShuangShengMatchState::clear_match();
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
		if (play_status >= GAME_PLAY_SEND_CARDS && play_status <= GAME_PLAY_END)
		{ 
			m_play_status = play_status;
		}
	}

	/* 获取该玩家在桌子上的位置 */
	Lint GetUserPos(User* pUser)
	{
		if (NULL == pUser)
		{
			return SHUANGSHENG_INVALID_POS;
		}
		Lint pos = SHUANGSHENG_INVALID_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= SHUANGSHENG_PLAY_USER_COUNT)
		{
			pos = SHUANGSHENG_INVALID_POS;
		}
		return pos;
	}

	/* 获取下一个有效位置 */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0) return 0; // 避免除零崩溃	
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
		return 0 <= pos && pos < SHUANGSHENG_INVALID_POS;
	}

	//获取以出牌的人数
	Lint GetHasOutPlayerCount(Lint firstPos, Lint currPos)
	{
		if (!is_pos_valid(firstPos) || !is_pos_valid(currPos))
		{
			return 0;
		}
		Lint outPlayerCount = 1;
		for (int i = firstPos; i % SHUANGSHENG_INVALID_POS != currPos; i++)
		{
			++outPlayerCount;
		}
		return outPlayerCount;
	}
	
	/* 获取本轮以出牌的人数 */
	Lint GetHasOutPlayerCount()
	{
		int t_outPlayerCount = 0;
		if (m_firstOutPos == SHUANGSHENG_INVALID_POS
			|| m_out_cards_count[m_firstOutPos] <= 0
			|| !m_gamelogic.IsValidCard(m_out_cards[m_firstOutPos][0]))
		{
			return 0;
		}

		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			if (m_out_cards_count[i] > 0 && m_gamelogic.IsValidCard(m_out_cards[i][0]))
			{
				++t_outPlayerCount;
			}
		}
		return t_outPlayerCount;
	}

};

/* 双升录像功能(结构体) */
struct ShuangShengVideoSupport : ShuangShengDeskState 
{
	//升级记录录像的结构体
	QiPaiVideoLog_ShengJi		m_video;

	void VideoSave()
	{
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_playType = m_desk->getPlayType();
		//m_video.m_flag=m_registered_game_type;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.getInsertSql();
		gWork.SendMsgToDb(video);
	}

	/*************************************************************************************
	* 函数名：	saveUserOper()
	* 描述：	双升：将玩家操作添加的QiPaiVideoLog_ShengJi 结构体重
	* 参数：
	*  @ pos 当前操作玩家位置
	*  @ oper 操作类型：1拿底牌，2埋底牌，3普通出牌，4甩牌
	*  @ firstOutPos 每轮第一个出牌玩家位置
	*  @ operCardsCount 操作牌的张数
	*  @ operCards 操作牌的集合
	*  @ handCardsCount 该玩家手牌
	*  @ handCards 手牌的集合
	*  @ totleScore 闲家总抓分
	* 返回：	(void)
	**************************************************************************************/
	void saveUserOper(Lint pos, Lint oper, Lint firstOutPos, Lint operCardsCount, BYTE* operCards,  BYTE handCardsCount, BYTE* handCards, Lint totleScore = 0)
	{
		LLOG_DEBUG("..................SaveUserOper  pos[%d], oper[%d]", pos, oper);

		std::vector<Lint> t_operCards;	//操作的牌
		std::vector<Lint> t_handCards;	//手牌
		if (operCardsCount != 0)
		{
			if (operCards != NULL)
			{
				for (int i = 0; i < operCardsCount; i++)
				{
					LLOG_DEBUG("..................operCards[%2d]=[%2d]", i, operCards[i]);
					t_operCards.push_back((Lint)operCards[i]);
				}
			}
		}
		if (handCardsCount != 0)
		{
			if (handCards != NULL)
			{
				for (int i = 0; i < handCardsCount; i++)
				{
					LLOG_DEBUG("..................cbHandCardData[%2d]=[%2d]", i, handCards[i]);
					t_handCards.push_back((Lint)handCards[i]);
				}
			}

		}
		m_video.addOper(pos, oper, firstOutPos, totleScore, t_operCards, t_handCards);
	}
};


/* 双升游戏具体处理逻辑（结构体）*/
struct ShuangShengGameCore : GameHandler, ShuangShengVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
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
	}

	/********************************************************************************************
	* 函数名：      start_game()
	* 描述：        双升每一局游戏开始的具体入口
	* 返回：        (void)
	*********************************************************************************************/
	void start_game()
	{
		LLOG_DEBUG("desk_id=[%d], ShuangSheng_start_game....m_play_status:[%d]", m_desk ? m_desk->GetDeskId() : 0, m_play_status);

		//设置本局主级牌
		if (m_round_offset == 0)
		{
			m_mainValue = m_gamelogic.GetCardValue(m_gradeCard[0]);
		}
		else
		{
			m_mainValue = m_gamelogic.GetCardValue((m_zhuangPos == SHUANGSHENG_INVALID_POS) ? m_gradeCard[0] : m_gradeCard[m_zhuangPos % 2]);
		}
		
		//设置游戏状态为发牌阶段
		set_play_status(GAME_PLAY_SEND_CARDS);

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
		LLOG_DEBUG("desk_id=[%d], LogRen...SHUANGSHENG  START  GAME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", m_desk ? m_desk->GetDeskId() : 0);

		LLOG_ERROR("ShuangSheng::start_round() Run... This Round Play Type deskId=[%d], baseTimes=[%d], isRandZhu=[%d], isAntiCheating=[%d], is2ChangZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_baseTimes, m_isRandZhu, m_isAntiCheating, m_is2ChangZhu);

		clear_round();
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);
		memcpy(m_user_status, player_status, sizeof(m_user_status));

		if (m_desk && m_desk->m_vip)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("desk_id=[%d], start_round=[%d/%d], player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		//游戏开始入口
		start_game();
	}

	/* 洗牌发牌 */
	bool dispatch_user_cards()
	{

		//乱序后的扑克牌
		BYTE cbRandCard[SHUANGSHENG_CELL_PACK * SHUANGSHENG_PACK_COUNT];

		//乱序扑克
		m_gamelogic.RandCardList(cbRandCard, SHUANGSHENG_CELL_PACK * SHUANGSHENG_PACK_COUNT);

		//debug模式下配牌
		debugSendCards_Shuangsheng(cbRandCard, m_desk, DEBUG_CARD_FILE_SHUANGSHENG, SHUANGSHENG_PLAY_USER_COUNT, SHUANGSHENG_HAND_CARDS_COUNT, SHUANGSHENG_CELL_PACK * SHUANGSHENG_PACK_COUNT);
		
		//给每个玩家发牌
		for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
		{
			m_hand_cards_count[i] = SHUANGSHENG_HAND_CARDS_COUNT;
			memcpy(&m_hand_cards[i], &cbRandCard[i*m_hand_cards_count[i]], sizeof(BYTE)*m_hand_cards_count[i]);
		}

		//底牌扑克
		m_baseCardCount = (SHUANGSHENG_CELL_PACK * m_packCount) - (m_player_count * m_gamelogic.GetDispatchCount());
		memcpy(m_base_cards, &cbRandCard[SHUANGSHENG_CELL_PACK * m_packCount - m_baseCardCount], sizeof(BYTE)*m_baseCardCount);
		

		//给用户发牌
		ShuangShengS2CSendCards sendCards[SHUANGSHENG_PLAY_USER_COUNT];
		for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
		{
			sendCards[i].m_outTime = DEF_TIME_SELECT_ZHU;
			sendCards[i].m_pos = i;
			sendCards[i].m_zhuangPos = m_zhuangPos;
			sendCards[i].m_handCardsCount = m_hand_cards_count[i];
			for (int j = 0; j < SHUANGSHENG_HAND_CARDS_COUNT; ++j)
			{
				sendCards[i].m_handCards[j] = (int)m_hand_cards[i][j];
			}
			sendCards[i].m_currGrade = m_mainValue;
			sendCards[i].m_gradeCard[0] = m_gradeCard[0];
			sendCards[i].m_gradeCard[1] = m_gradeCard[1];

			memcpy(sendCards[i].m_totleScore, m_accum_score, sizeof(m_accum_score));

			//发牌时候排列手牌排列手牌，主花色等牌无法排序
			m_gamelogic.SortCardList(m_hand_cards[i], m_hand_cards_count[i]);
		}
		notify_user(sendCards[0], 0);
		notify_user(sendCards[1], 1);
		notify_user(sendCards[2], 2);
		notify_user(sendCards[3], 3);

		//玩家0发牌日志
		LLOG_DEBUG("notify_user0:Send Cards To player0 cardCount=[%d], deskId=[%d]", sendCards[0].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player0.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[0].m_handCards[i], sendCards[0].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//玩家1发牌日志
		LLOG_DEBUG("notify_user1:Send Cards To player1 cardCount=[%d], deskId=[%d]", sendCards[1].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player1.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[1].m_handCards[i], sendCards[1].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//玩家2发牌日志
		LLOG_DEBUG("notify_user2:Send Cards To player2 cardCount=[%d], deskId=[%d]", sendCards[2].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player2.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[2].m_handCards[i], sendCards[2].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//玩家3发牌日志
		LLOG_DEBUG("notify_user3:Send Cards To player3 cardCount=[%d], deskId=[%d]", sendCards[3].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player3.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[3].m_handCards[i], sendCards[3].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//发完牌自动设置游戏状态为报主状态
		set_play_status(GAME_PLAY_SELECT_ZHU);

		LLOG_DEBUG("Send Cards Over ^_^... Start Send Cards Tick... deskId=[%d], playStatus=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		m_tick_flag = true;
		m_play_status_time.Now();

		return true;
	}

	/* 玩家报主 */
	bool on_event_user_select_zhu(int pos, BYTE selectCard, BYTE selectCardCount, int kCount)
	{
		//校验：叫主牌张数 <= 0 ,不符合要求
		if (selectCardCount <= 0)
		{
			LLOG_DEBUG("Select Zhu Error !!! select card count is null !!! desk_id=[%d], select_card_cound=[%d], kClickCount=[%d],",
				m_desk ? m_desk->GetDeskId() : 0, selectCardCount, kCount);
			return false;
		}

		//校验：如果叫主牌是级牌，或者是两张王牌，符合条件
		if (!(m_gamelogic.GetCardValue(selectCard) == m_mainValue || ((selectCard == 0x4E || selectCard == 0x4F) && selectCardCount == 2)))
		{
			LLOG_DEBUG("Select Zhu Error !!! select card is not main card or not 2 same joker desk_id=[%d], select_card=[0x%x], select_card_cound=[%d], kClickCount=[%d], main_value=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, selectCard, selectCardCount, kCount, m_mainValue);
			return false;
		}

		//校验：叫主牌张数 > 手牌中主牌张数，不符合要求
		BYTE tmpSelectCardCount = 0;
		for (int i = 0; i < m_hand_cards_count[pos]; ++i)
		{
			if (m_hand_cards[pos][i] == selectCard) ++tmpSelectCardCount;
		}
		if (selectCardCount > tmpSelectCardCount)
		{
			LLOG_DEBUG("Select Zhu Error !!! select card count moer than hand this card count !!!  desk_id=[%d], pos=[%d], select_card=[%d], select_card_count=[%d], hand_select_card_count=[%d], kClickCount=[%d], main_value=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, selectCard, selectCardCount, tmpSelectCardCount, kCount, m_mainValue);
			return false;
		}

		//如果该玩家状态为已叫过主状态，且再次叫主的牌与上次不一样，且张数 <= 上次叫主张数
		if (m_hasSelectZhu[pos] && selectCard != m_selectCard && selectCardCount <= m_selectCardCount)
		{
			LLOG_DEBUG("This Player Select Zhu Repeat Error!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], kClickCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, kCount);
			return false;
		}
		
		//处理玩家定主，防止其他玩家反其他花色主
		if (pos == m_selectCardPos)
		{
			if ((selectCard != m_selectCard) || (selectCardCount <= m_selectCardCount))
			{
				LLOG_DEBUG("This Player Select Zhu Faile < !!! fix main color failure !!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d], kClickCount=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue, kCount);
				return true;
			}
		}
		//处理玩家第一次叫主 或 玩家反主
		else
		{
			//其他玩家报主牌张数 < 已经报主的张数 是绝对不能报主的
			//  selectCardCount = 0, m_selectCardCount = 1
			//  selectCardCount = 1, m_selectCardCount = 2 
			if (selectCardCount < m_selectCardCount || selectCardCount <= 0)
			{
				LLOG_DEBUG("This Player Select Zhu Faile < !!! this select zhu card count less than has select zhu card count!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}
			//大小王反无主必须是一对
			if (selectCardCount < 2 && (selectCard == 0x4E || selectCard == 0x4F))
			{
				LLOG_DEBUG("This Player Select Zhu Faile !!! select joker count less than 2 !!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}

			//  selectCardCount > 0
			//  selectCardCount = 1, m_selectCardCount = 1
			//  (selectCardCount = 2, m_selectCardCount = 2) && selectCard 不是王
			if (selectCardCount == m_selectCardCount && 
				(selectCardCount == 1 || (selectCardCount == 2 && m_gamelogic.GetCardValue(selectCard) < 0x0E)))
			{
				LLOG_DEBUG("This Player Select Zhu Faile = !!! this select zhu card count less than has select zhu card count!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}
			// selectCardCount = 1, m_selectCardCount = 0
			// selectCardCount = 2, m_selectCardCount = 1
			// selectCardCount = m_selectCardCount || selectCard 是王
			if (!(selectCardCount > m_selectCardCount || m_gamelogic.GetCardValue(selectCard) > m_gamelogic.GetCardValue(m_selectCard)))
			{
				LLOG_DEBUG("This Player Select Zhu Faile > !!! this select zhu card less than has select zhu card!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}
			//if ((selectCardCount <= m_selectCardCount) && (selectCard != 0x4E || selectCard != 0x4F)) return true;
		}

		m_selectCard = selectCard;
		m_selectCardCount = selectCardCount;
		m_selectCardPos = pos;
		//逻辑选主的牌，如果无主，则为0x40
		m_logicMainCard = (m_selectCard == 0x4F || m_selectCard == 0x4E) ? 0x40 : m_selectCard;

		LLOG_DEBUG("This Player Select Zhu Success ^_^  desk_id=[%d], pos=[%d], select_card=[%d], select_card_count=[%d], main_value=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_selectCardPos, m_selectCard, m_selectCardCount, m_mainValue);

		if (GAME_PLAY_SEND_CARDS == m_play_status)
		{
			set_play_status(GAME_PLAY_SELECT_ZHU);
		}

		//标记本次叫主玩家
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			if (i == pos) m_hasSelectZhu[pos] = true;
			else m_hasSelectZhu[i] = false;
		}

		//广播玩家叫主情况
		ShuangShengS2CBCSelectZhu bcZhu;
		bcZhu.m_outTime = DEF_TIME_SELECT_ZHU;
		bcZhu.m_pos = pos;
		bcZhu.m_selectZhuCount = m_selectCardCount;
		bcZhu.m_selectZhu = m_selectCard;
		bcZhu.m_count = kCount;
		LLOG_DEBUG("notify_desk::Player Select Zhu deskId=[%d], selectPos=[%d], selectCard=[%d], selectCardCount=[%d], kCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, bcZhu.m_pos, bcZhu.m_selectZhu, bcZhu.m_selectZhuCount, bcZhu.m_count);
		notify_desk(bcZhu);

		//大王反无主，直接结束叫主
		/*if (m_selectCard == 0x4F && m_selectCardCount == 2 && m_play_status == GAME_PLAY_SELECT_ZHU)
		{
			select_zhu_over();
		}*/
		//其他报主开启等待时间
		//else if(m_play_status == GAME_PLAY_SELECT_ZHU)
		//{
		//	//开启报主定时
		//	//m_tick_flag = true;
		//	//m_play_status_time.Now();
		//}
	}

	/* 叫主结束 */
	bool select_zhu_over()
	{
		//处理无人报主情况
		if (m_selectCard == 0 || m_selectCardCount == 0 || m_selectCardPos == SHUANGSHENG_INVALID_POS)
		{
			//大王无主
			m_selectCard = 0x4F;

			//随机定义个报主位置，第一局需要改位置确定庄家
			srand(time(NULL));
			m_selectCardPos = rand() % SHUANGSHENG_PLAY_USER_COUNT;

			LLOG_DEBUG("Select Zhu Over !!! But No Player Select, So This Round Is No Mian Color!!! desk_id=[%d], rand_pos=[%d], main_value=[%d],  main_color=[0x%x]"
				, m_desk ? m_desk->GetDeskId() : 0, m_selectCardPos, m_mainValue, m_gamelogic.GetCardColor(m_selectCard));
		}

		//逻辑选主的牌，如果无主，则为0x40
		m_logicMainCard = (m_selectCard == 0x4F || m_selectCard == 0x4E) ? 0x40 : m_selectCard;

		//第一局此时可以确定庄家
		if (m_round_offset == 0) m_zhuangPos = m_selectCardPos;

		//设置主花色
		m_mainColor = m_gamelogic.GetCardColor(m_selectCard);

		//设置首轮第一个出牌的位置
		m_firstOutPos = m_zhuangPos;

		//设置当前出牌位置
		m_curPos = m_zhuangPos;

		//叫主结束
		LLOG_DEBUG("Select Zhu Over... desk_id=[%d], zhuang_pos=[%d], main_value=[%d],  main_color=[%x]"
		, m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_mainValue, m_mainColor);

		//给庄家发送手牌+底牌33张牌，同时通知其他玩家庄家正在埋底牌
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			ShuangShengS2CNoticeBaseInfo sendBase;
			sendBase.m_outTime = DEF_TIME_BASE_CARDS;
			sendBase.m_zhuangPos = m_zhuangPos;
			sendBase.m_selectCard = m_logicMainCard;
			if (i == m_zhuangPos)
			{
				for (int j = 0; j < SHUANGSHENG_BASE_CARDS_COUNT; ++j)
				{
					sendBase.m_baseCards[j] = (int)m_base_cards[j];
				}
				for (int k = 8; k < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++k)
				{
					sendBase.m_baseCards[k] = (int)m_hand_cards[m_zhuangPos][k - 8];
				}
			}
			LLOG_DEBUG("notify_user%d::Player Recive BaseInfo deskId=[%d], basePos=[%d], selectCard=[%d], baseCard=[%d]",
				i, m_desk ? m_desk->GetDeskId() : 0, sendBase.m_zhuangPos, sendBase.m_selectCard, sendBase.m_baseCards[0]);
			notify_user(sendBase, i);
		}

		//设置游戏状态为盖底牌操作
		set_play_status(GAME_PLAY_BASE_CARD);

		//保存录像（玩家拿底牌操作）
		int t_pos = m_zhuangPos;
		int t_oper = 1;  //操作：1拿底牌
		int t_firstOutPos = SHUANGSHENG_INVALID_POS;
		int t_totleScore = 0;
		saveUserOper(t_pos, t_oper, t_firstOutPos, m_baseCardCount, m_base_cards, m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

		return true;
	}

	/* 玩家盖底牌 */
	bool on_event_user_base_cards(int pos, ShuangShengC2SBaseCards* msg)
	{
		//校验玩家盖底牌数量
		int t_baseCardsCount = msg->m_baseCardsCount;
		if (t_baseCardsCount != 8)
		{
			LLOG_DEBUG("Player Base Cards Count Error!!! desk_id=[%d], user_id=[%d] card_count=[%d] "
				, m_desk ? m_desk->GetDeskId() : 0, m_desk->m_user[pos]->GetUserDataId(), t_baseCardsCount);
			return false;
		}

		//玩家盖的底牌
		BYTE t_baseCards[SHUANGSHENG_BASE_CARDS_COUNT];
		memset(t_baseCards, 0, sizeof(t_baseCards));
		for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
		{
			t_baseCards[i] = (BYTE)msg->m_baseCards[i];
		}
		//排列底牌：庄家选择埋底的牌
		m_gamelogic.SortCardList(t_baseCards, SHUANGSHENG_BASE_CARDS_COUNT);

		//构造拿底牌玩家原始手牌家底牌数组33张牌
		BYTE t_orgHandCards[SHUANGSHENG_MAX_HAND_CARDS_COUNT];
		memset(t_orgHandCards, 0, sizeof(t_orgHandCards));
		for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
		{
			t_orgHandCards[i] = m_base_cards[i];
		}
		for (int i = SHUANGSHENG_BASE_CARDS_COUNT; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			t_orgHandCards[i] = m_hand_cards[pos][i - 8];
		}
		//排列手牌：庄家拿底牌后的33张手牌
		m_gamelogic.SortCardList(t_orgHandCards, SHUANGSHENG_MAX_HAND_CARDS_COUNT);

		//玩家盖底牌是否成功标志
		bool t_flag = false;

		//删除玩家埋的底牌，埋底牌成功
		if (m_gamelogic.RemoveCard(t_baseCards, SHUANGSHENG_BASE_CARDS_COUNT, t_orgHandCards, SHUANGSHENG_MAX_HAND_CARDS_COUNT) == true)
		{
			ShuangShengS2CBCBaseCardOver baseOver;
			baseOver.m_pos = pos;

			t_flag = true;
			//设置游戏状态为出牌状态
			set_play_status(GAME_PLAY_PLAY_CARD);
			
			//玩家埋底的牌放到底牌中
			memset(m_base_cards, 0, sizeof(m_base_cards));
			memcpy(m_base_cards, t_baseCards, sizeof(m_base_cards));

			//玩家埋底剩下的牌放到玩家手牌中
			m_hand_cards_count[pos] = SHUANGSHENG_HAND_CARDS_COUNT;
			memset(m_hand_cards[pos], 0, sizeof(m_hand_cards[pos]));
			for (int j = 0; j < SHUANGSHENG_HAND_CARDS_COUNT; ++j)
			{
				m_hand_cards[pos][j] = t_orgHandCards[j];
			}

			baseOver.m_selectCard = m_logicMainCard;
			/*if (m_gamelogic.GetCardColor(m_selectCard) == 0x40)
			{
				baseOver.m_selectCard = 0x40;
			}*/
			
			baseOver.m_handCardsCount = SHUANGSHENG_HAND_CARDS_COUNT;
			baseOver.m_flag = t_flag;
			for (int i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
			{
				baseOver.m_handCards[i] = t_orgHandCards[i];	
			}
			LLOG_DEBUG("notify_user%d::Player Base Card Over deskId=[%d], basePos=[%d], selectCard=[%d], player%dHandCard=[%d], handCardCound=[%d], baseFlag=[%d]",
				baseOver.m_pos, m_desk ? m_desk->GetDeskId() : 0, baseOver.m_pos, baseOver.m_selectCard, baseOver.m_pos, baseOver.m_handCards[0], baseOver.m_handCardsCount, baseOver.m_flag);
			notify_user(baseOver, pos);
		}

		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			if (i == pos && t_flag) continue;

			//通知玩家盖底牌结果
			ShuangShengS2CBCBaseCardOver baseOver;
			baseOver.m_pos = pos;
			baseOver.m_flag = t_flag;
			LLOG_DEBUG("notify_user%d::Player Base Card Over deskId=[%d], basePos=[%d], player%dHandCard=[%d], handCardCound=[%d], baseFlag=[%d]",
				i, m_desk ? m_desk->GetDeskId() : 0, baseOver.m_pos, i, baseOver.m_handCards[0], baseOver.m_handCardsCount, baseOver.m_flag);
			notify_user(baseOver, i);
		}
		LLOG_DEBUG("Player base cards ... desk_id=[%d], user_id=[%d], is_base_cards_success=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_desk->m_user[pos]->GetUserDataId(), t_flag);

		//玩家埋底失败
		if (!t_flag) return false;
		
		//保存录像（玩家埋底牌操作）
		int t_pos = m_zhuangPos;
		int t_oper = 2;  //操作：2埋底牌
		int t_firstOutPos = m_zhuangPos;
		int t_totleScore = 0;
		saveUserOper(t_pos, t_oper, t_firstOutPos, m_baseCardCount, m_base_cards, m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

		//开始出牌
		start_play_cards();
	}

	/* 开始出牌 */
	bool start_play_cards()
	{
		LLOG_DEBUG("Start Play Cards... desk_id=[%d], start_out_pos=[%d], main_value=[%d],  main_color=[%d]"
			, m_desk ? m_desk->GetDeskId() : 0, m_firstOutPos, m_mainValue, m_mainColor);

		//设置Logic的主花色 和 级牌
		m_gamelogic.SetMainValue(m_gamelogic.GetCardValue(m_mainValue));
		m_gamelogic.SetMainColor(m_mainColor);

		//从新排列每个玩家手牌
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			//排列手牌：每个玩家开始出牌前排序25张手牌
			m_gamelogic.SortCardList(m_hand_cards[i], SHUANGSHENG_HAND_CARDS_COUNT);
		}

		//广播游戏开始
		ShuangShengS2CBCGameStart start;
		start.m_pos = m_zhuangPos;
		notify_desk(start);

		return true;
	}

	/********************************************************
	* 函数名：	out_cards_sound()
	* 描述：	玩家出牌提示音类型， -1：默认不处理  0：管不上领牌（啪）  1：被同花色管上（大你/管上）  2：主杀
	* 参数：
	*  @ pos 当前出牌玩家
	*  @ currBig 当前出牌最大玩家
	* 返回：	(int)-1：默认不处理  0：管不上领牌（啪）  1：被同花色管上（大你/管上）  2：主杀
	*********************************************************/
	int out_cards_sound(int pos, int currBig)
	{
		//第一个出牌的人不提示
		if (m_firstOutPos == pos)
		{
			return -1;
		}
		//当前出牌的人不是最大的，提示“啪/跟牌”
		if (currBig != pos)
		{
			return 0;
		}
		int t_beforPos = (pos - 1 + SHUANGSHENG_PLAY_USER_COUNT) % SHUANGSHENG_PLAY_USER_COUNT;
		//逻辑花色
		BYTE t_outLogicColor = m_gamelogic.GetCardLogicColor(m_out_cards[pos], m_out_cards_count[pos]);
		BYTE t_beforeLogicColor = m_gamelogic.GetCardLogicColor(m_out_cards[t_beforPos], m_out_cards_count[t_beforPos]);
		BYTE t_firstLogicColor = m_gamelogic.GetCardLogicColor(m_out_cards[m_firstOutPos], m_out_cards_count[m_firstOutPos]);
		
		//当前出牌的上家是首家出牌位置
		if (t_beforPos == m_firstOutPos)
		{
			//当前出牌最大，且首家出副牌，当前出主牌，提示“毙了”
			if (t_firstLogicColor != COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 2;
			}
			//当前出牌最大，且和首家出牌同花色，提示“大你/管上”
			else if (t_firstLogicColor == t_outLogicColor)
			{
				return 1;
			}
		}
		//当前出牌的上家不是首家
		else
		{
			//当前出牌最大，且首家出副牌 && 上家出副牌 && 当前出主牌，提示“毙了”
			if (t_firstLogicColor != COLOR_NT && t_beforeLogicColor != COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 2;
			}
			//当前出牌最大 && 首家同花色 && 上家同花色，提示“大你/管上”
			if (t_firstLogicColor == t_outLogicColor && t_beforeLogicColor == t_outLogicColor)
			{
				return 1;
			}
			//当前出牌最大 && 首家出副牌 && 上家毙了 && 当前盖毙，提示“大你/管上”
			if (t_firstLogicColor != COLOR_NT && t_beforeLogicColor == COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 1;
			}
			//当前出牌最大 && 首家出主牌 && 上家不出主牌 && 当前出主牌，提示“大你/管上”
			if (t_firstLogicColor == COLOR_NT && t_beforeLogicColor != COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 1;
			}
		}
		return 0;
	}

	/* 玩家出牌 */
	bool on_event_user_out_cards(int pos, BYTE cbCardData[], BYTE cbCardCount)
	{
		LLOG_DEBUG("Player Out Cards Run... desk_id=[%d], pos=[%d], out_cards_count=[%d], is_first_out=[%d]"
			, m_desk ? m_desk->GetDeskId() : 0, pos, cbCardCount, pos == m_firstOutPos);

		//打印出的牌值
		LLOG_DEBUG("Player Out Cards={");
		for (int i = 0; i < cbCardCount; ++i)
		{
			LLOG_DEBUG("       outCard[%d]=[0x%2x|(%d)],", i, cbCardData[i], cbCardData[i]);
		}
		LLOG_DEBUG("                  }");
		LLOG_DEBUG("outCardsCount = [%d], pos = [%d], deskId = [%d]", cbCardCount, pos, m_desk ? m_desk->GetDeskId() : 0);

		//效验状态
		//ASSERT(m_pITableFrame->GetGameStatus() == GS_UG_PLAY);
		//if (m_pITableFrame->GetGameStatus() != GS_UG_PLAY) return true;

		//效验用户
		//ASSERT((wChairID == m_wCurrentUser) && (cbCardCount <= MAX_COUNT));
		if ((pos != m_curPos) || (cbCardCount > SHUANGSHENG_HAND_CARDS_COUNT))
		{
			LLOG_DEBUG("Player Out Cards Error, Pos Or Cards Count error!!! desk_id=[%d], cur_pos=[%d], pos=[%d], out_cards_count=[%d]"
				, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, cbCardCount);
			return false;
		}

		//排列出牌：排列玩家出的牌
		m_gamelogic.SortCardList(cbCardData, cbCardCount);

		//出牌类型判断
		BYTE cbCardType = m_gamelogic.GetCardType(cbCardData, cbCardCount);

		//每轮首出牌玩家
		if (m_firstOutPos == pos)
		{
			LLOG_DEBUG("Player Out Cards..., The First Out Cards^_^, desk_id=[%d], cur_pos=[%d], pos=[%d], cardType=[%d]"
				, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, cbCardType);

			//类型判断
			if (cbCardType == CT_ERROR)
			{
				LLOG_DEBUG("Player Out Cards Error..., The First Out Cards Type Error!!! desk_id=[%d], cur_pos=[%d], pos=[%d], cardType=[%d]"
					, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, cbCardType);
				return false;
			}

			//甩牌处理
			if (cbCardType == CT_THROW_CARD)
			{
				//甩牌结果判断
				tagOutCardResult OutCardResult;
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));

				LLOG_DEBUG("ShuangShengGameLogic::EfficacyThrowCard_Small() Run... ThrowPos%dOutCards, OutCardCount=[%d], handCardCount=[%d], mainColor=[0x%2x], deskId=[%d]",
					pos, cbCardCount, m_hand_cards_count[pos],m_mainColor, m_desk ? m_desk->GetDeskId() : 0);

				for (int j = 0; j < cbCardCount; ++j)
				{
					LLOG_DEBUG("ShuangShengGameLogic::EfficacyThrowCard_Small() ...............ThrowOutCard[%d]=[%d]", j, cbCardData[j]);
				}
				bool bThrowSuccess = m_gamelogic.EfficacyThrowCard_Small(cbCardData, cbCardCount, pos, m_hand_cards, m_hand_cards_count[pos], OutCardResult);

				//甩牌失败，出小的
				if (bThrowSuccess == false)  
				{
					LLOG_DEBUG("Player Throw Cards Failed... Out Cards Result...  desk_id=[%d], cur_pos=[%d], pos=[%d]"
						, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos);

					//删除手牌中甩牌失败后应该出的牌
					if (m_gamelogic.RemoveCard(OutCardResult.cbResultCard, OutCardResult.cbCardCount, m_hand_cards[pos], m_hand_cards_count[pos]) == false)
					{
						LLOG_DEBUG("Player Throw Cards Failed...  Remove Out Cards Result Error!!! desk_id=[%d], cur_pos=[%d], pos=[%d]"
							, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos);
						return false;
					}
					m_hand_cards_count[pos] -= OutCardResult.cbCardCount;

					//玩家甩牌失败，强制出小牌后，从新排列手牌
					//m_gamelogic.SortCardList(m_hand_cards[pos], m_hand_cards_count[pos]);

					//出牌记录
					m_out_cards_count[pos] = OutCardResult.cbCardCount; 
					memcpy(m_out_cards[pos], OutCardResult.cbResultCard, OutCardResult.cbCardCount * sizeof(BYTE));

					//切换用户
					m_curPos = (pos + 1) % m_player_count;

					//62209：服务器广播第一个人出牌结果
					ShuangShengS2CBCOutCardsResult outCard;
					outCard.m_pos = pos;
					outCard.m_turnStart = 1;
					outCard.m_turnOver = 0;
					outCard.m_nextPos = m_curPos;
					outCard.m_mode = cbCardType;
					//玩家甩的牌（失败的）
					outCard.m_outCardsCount = cbCardCount;
					for (int i = 0; i < cbCardCount; ++i)
					{
						outCard.m_outCards[i] = cbCardData[i];
					}
					//玩家甩牌失败后应该出的牌
					outCard.m_resultOutCardsCount = OutCardResult.cbCardCount;
					for (int i = 0; i < OutCardResult.cbCardCount; ++i)
					{
						outCard.m_resultOutCards[i] = OutCardResult.cbResultCard[i];
					}
					outCard.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
					outCard.m_scoreCardsCount = m_cbScoreCardCount;
					for (int i = 0; i < m_cbScoreCardCount; ++i)
					{
						outCard.m_scoreCards[i] = m_cbScoreCardData[i];
					}
					
					//广播给桌子
					notify_desk(outCard);

					//保存录像（玩家甩牌操作）
					int t_pos = pos;
					int t_oper = 4;  //操作：1：拿底牌，2：埋底牌，3：普通出牌，4：甩牌
					int t_firstOutPos = m_firstOutPos;
					int t_totleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
					saveUserOper(t_pos, t_oper, t_firstOutPos, m_out_cards_count[t_pos], m_out_cards[t_pos], m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

					return true;
				}
			}
		}
		//每轮跟出牌玩家（非第一个出牌玩家）
		else  
		{
			//校验跟牌玩家出牌数量是否一致
			if (cbCardCount != m_out_cards_count[m_firstOutPos])
			{
				LLOG_DEBUG("Player Follow Out Cards Failed...  Follow Out Cards Count Error!!!  desk_id=[%d], cur_pos=[%d], pos=[%d]，is_first_out=[%d]"
					, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, pos == m_firstOutPos);
				return false;
			}

			//出牌效验
			if (m_gamelogic.EfficacyOutCard(cbCardData, cbCardCount, m_out_cards[m_firstOutPos],
				m_out_cards_count[m_firstOutPos], m_hand_cards[pos], m_hand_cards_count[pos]) == false)
			{
				LLOG_DEBUG("Player Follow Out Cards Failed...  Follow Out Cards Type Error!!!  desk_id=[%d], cur_pos=[%d], pos=[%d]，is_first_out=[%d], mainValue=[%d], mainColor=[%d]"
					, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, pos == m_firstOutPos, m_gamelogic.GetMainValue(), m_gamelogic.GetMainColor());
				return false;
			}
		}

		//删除手牌中出的牌（出第一个出牌甩牌的玩家）
		if (m_gamelogic.RemoveCard(cbCardData, cbCardCount, m_hand_cards[pos], m_hand_cards_count[pos]) == false)
		{
			LLOG_ERROR("Player Out Cards...  Remove Out Cards From HandCards Error!!! deskId=[%d], outCardsCount=[%d], curPos=[%d], pos=[%d], is_firstOut=[%d], mainValue=[%d], mainColor=[%d]"
				, m_desk ? m_desk->GetDeskId() : 0, cbCardCount, m_curPos, pos, pos == m_firstOutPos, m_gamelogic.GetMainValue(), m_gamelogic.GetMainColor());
			return false;
		}
		//出牌后，从新排列手牌
		//m_gamelogic.SortCardList(m_hand_cards[pos], m_hand_cards_count[pos] - cbCardCount);

		//最后一轮甩牌，服务器端已经做出判断4家的牌，本局游戏结束
		if (m_hand_cards_count[pos] == cbCardCount)
		{
			//出牌记录
			for (WORD i = 0; i < m_player_count; i++)
			{
				m_out_cards_count[i] = cbCardCount;
				if (i == pos)
				{ 
					memcpy(m_out_cards[i], cbCardData, cbCardCount * sizeof(BYTE));
				}
				else
				{ 
					memcpy(m_out_cards[i], m_hand_cards[i], cbCardCount * sizeof(BYTE));
					//排列出牌：排列最后一手出的牌
					m_gamelogic.SortCardList(m_out_cards[i], cbCardCount);
				}
				m_hand_cards_count[i] = 0;
				memset(m_hand_cards[i], 0, sizeof(m_hand_cards[i]));
			}

			//上一轮优胜者，也就是本轮第一个出牌玩家
			Lint before_winner = m_wTurnWinner;

			//判断最后一轮出牌的胜者,必定判断4家，没毛病
			m_wTurnWinner = m_gamelogic.CompareCardArray(m_out_cards, m_out_cards_count[m_firstOutPos], m_firstOutPos);

			//计算得分
			int t_xianTureScore = 0;

			if ((m_wTurnWinner != m_zhuangPos) && ((m_wTurnWinner + 2) % m_player_count != m_zhuangPos))
			{
				BYTE tmp_turnScoreCardsCount = 0;
				BYTE tmp_turnScoreCards[24];
				memset(tmp_turnScoreCards, 0, sizeof(tmp_turnScoreCards));

				for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
				{
					BYTE * tmp_pcbScoreCard = &tmp_turnScoreCards[tmp_turnScoreCardsCount];
					BYTE tmp_cbMaxCount = CountArray(tmp_turnScoreCards) - tmp_turnScoreCardsCount;
					tmp_turnScoreCardsCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], tmp_pcbScoreCard, tmp_cbMaxCount);

					BYTE * pcbScoreCard = &m_cbScoreCardData[m_cbScoreCardCount];
					BYTE cbMaxCount = CountArray(m_cbScoreCardData) - m_cbScoreCardCount;
					m_cbScoreCardCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], pcbScoreCard, cbMaxCount);
				}
				t_xianTureScore = m_gamelogic.GetCardScore(tmp_turnScoreCards, tmp_turnScoreCardsCount);
			}
			
			for(int i = before_winner; i < m_player_count + before_winner; ++i)
			{
				ShuangShengS2CBCOutCardsResult outCard;
				outCard.m_pos = i % m_player_count;
				outCard.m_mode = m_gamelogic.GetCardType(m_out_cards[i], m_out_cards_count[i]);
				outCard.m_currScore = t_xianTureScore;
				outCard.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
				//outCard.m_nextPos = (i + 1) % m_player_count;
				outCard.m_nextPos = SHUANGSHENG_INVALID_POS;
				outCard.m_outCardsCount = m_out_cards_count[outCard.m_pos];
				for (int j = 0; j < m_out_cards_count[outCard.m_pos]; ++j)
				{
					outCard.m_outCards[j] = (Lint)m_out_cards[outCard.m_pos][j];
				}
				outCard.m_scoreCardsCount = m_cbScoreCardCount;
				for (int i = 0; i < m_cbScoreCardCount; ++i)
				{
					outCard.m_scoreCards[i] = m_cbScoreCardData[i];
				}

				//第一个出牌
				if (i == before_winner)
				{
					outCard.m_turnStart = 1;
					outCard.m_nextPos = (i + 1) % m_player_count;
				}
				//最后一个出牌玩家
				else if(i == (m_player_count + before_winner -1))
				{ 
					outCard.m_turnOver = 1;
					outCard.m_turnWiner = m_wTurnWinner;
					outCard.m_currBig = m_wTurnWinner;
					//outCard.m_nextPos = m_wTurnWinner;
				}
				//传给客户端用于提示声音
				//outCard.m_soundType = out_cards_sound(pos, outCard.m_currBig);
				notify_desk(outCard);

				//保存录像（玩家最后一轮出牌）
				int t_pos = i;
				int t_oper = 3;  //操作：1：拿底牌，2：埋底牌，3：普通出牌，4：甩牌
				int t_firstOutPos = m_firstOutPos;
				int t_totleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
				saveUserOper(t_pos, t_oper, t_firstOutPos, cbCardCount, m_out_cards[t_pos], m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

				//设置游戏状态为出牌结束
				set_play_status(GAME_PLAY_END);
			}

			//一轮游戏正常结束
			finish_round();
		}
		//不是最后一轮出牌
		else  
		{
			//手牌数量 - 出牌的数量
			m_hand_cards_count[pos] -= cbCardCount;

			//记录本人出的牌 和 出牌数量
			m_out_cards_count[pos] = cbCardCount;
			memcpy(m_out_cards[pos], cbCardData, cbCardCount * sizeof(BYTE));

			//切换当前位置为下一个出牌玩家位置
			m_curPos = (pos + 1) % m_player_count;

			//如果下一个出牌玩家位置 与 第一个出牌玩家位置相同，则本轮出牌结束
			if (m_curPos == m_firstOutPos) m_curPos = SHUANGSHENG_INVALID_POS;

			//62209：服务器广播玩家出牌结果
			ShuangShengS2CBCOutCardsResult outCard;
			outCard.m_pos = pos;
			outCard.m_nextPos = m_curPos;
			outCard.m_mode = cbCardType;

			//出的牌
			outCard.m_outCardsCount = cbCardCount;
			for (int i = 0; i < cbCardCount; ++i)
			{
				outCard.m_outCards[i] = cbCardData[i];
			}
			
			//本轮第一个出牌玩家
			if (pos == m_firstOutPos)
			{
				outCard.m_turnStart = 1;
				outCard.m_turnOver = 0;
			}
			else
			{
				//判断已出牌中最大的牌，走分支函数判断
				outCard.m_currBig = (int)m_gamelogic.CompareCardArray_n(m_out_cards, m_out_cards_count[m_firstOutPos], m_firstOutPos, GetHasOutPlayerCount());
				
				//传给客户端用于提示声音
				outCard.m_soundType = out_cards_sound(pos, outCard.m_currBig);
			}

			//用于录像记录
			BYTE t_outCard[SHUANGSHENG_HAND_CARDS_COUNT];
			memset(t_outCard, 0, sizeof(t_outCard));
			memcpy(t_outCard, m_out_cards[pos], sizeof(t_outCard));
			Lint t_outCardCount = m_out_cards_count[m_firstOutPos];

			//一轮出牌结束
			if (m_curPos == SHUANGSHENG_INVALID_POS)
			{
				//一轮出牌结束，判断胜者，必定是4家，没毛病
				m_wTurnWinner = m_gamelogic.CompareCardArray(m_out_cards, m_out_cards_count[pos], m_firstOutPos);

				//本轮出牌 闲家一方赢，抓分成功
				int t_xianTureScore = 0;

				if ((m_wTurnWinner != m_zhuangPos) && ((m_wTurnWinner + 2) % m_player_count != m_zhuangPos))
				{
					BYTE tmp_turnScoreCardsCount = 0;
					BYTE tmp_turnScoreCards[24];
					memset(tmp_turnScoreCards, 0, sizeof(tmp_turnScoreCards));
					
					for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
					{
						BYTE * tmp_pcbScoreCard = &tmp_turnScoreCards[tmp_turnScoreCardsCount];
						BYTE tmp_cbMaxCount = CountArray(tmp_turnScoreCards) - tmp_turnScoreCardsCount;
						tmp_turnScoreCardsCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], tmp_pcbScoreCard, tmp_cbMaxCount);

						BYTE * pcbScoreCard = &m_cbScoreCardData[m_cbScoreCardCount];
						BYTE cbMaxCount = CountArray(m_cbScoreCardData) - m_cbScoreCardCount;
						m_cbScoreCardCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], pcbScoreCard, cbMaxCount);
					}
					t_xianTureScore = m_gamelogic.GetCardScore(tmp_turnScoreCards, tmp_turnScoreCardsCount);
				}

				//记录上次出牌,以及出牌张数
				memcpy(m_cbLastTurnCard, m_out_cards, sizeof(m_out_cards));
				memcpy(m_cbLastTurnCount, m_out_cards_count, sizeof(m_out_cards_count));

				//设置设置下轮出牌第一个出牌的玩家，清空本轮出的牌
				if (m_hand_cards[m_wTurnWinner] != 0)
				{
					m_firstOutPos = m_wTurnWinner;
					memset(m_out_cards, 0, sizeof(m_out_cards));
					memset(m_out_cards_count, 0, sizeof(m_out_cards_count));
				}
				
				//用户切换
				m_curPos = (m_hand_cards_count[m_wTurnWinner] == 0) ? SHUANGSHENG_INVALID_POS : m_wTurnWinner;
				outCard.m_turnStart = 0;
				outCard.m_turnOver = 1;
				outCard.m_nextPos = m_curPos;
				outCard.m_turnWiner = m_wTurnWinner;
				outCard.m_currScore = t_xianTureScore;
			}
			outCard.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
			outCard.m_scoreCardsCount = m_cbScoreCardCount;
			for (int i = 0; i < m_cbScoreCardCount; ++i)
			{
				outCard.m_scoreCards[i] = m_cbScoreCardData[i];
			}
			//广播出牌消息
			notify_desk(outCard);

			//保存录像（玩家出牌操作）
			int t_pos = pos;
			int t_oper = 3;  //操作：1：拿底牌，2：埋底牌，3：普通出牌，4：甩牌
			int t_firstOutPos = m_firstOutPos;
			int t_totleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
			saveUserOper(t_pos, t_oper, t_firstOutPos, t_outCardCount, t_outCard, m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

		}
		return true;
	}

	/* 记录总分与写入数据库 */
	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	/* 小结算 */
	void finish_round(BOOL jiesan = false)
	{
		//玩家解散房间，不发小结算消息
		if (true == jiesan)
		{
			LLOG_ERROR("ShuangShengGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());
			ShuangShengS2CBCDrawResult jiesan;
			jiesan.m_isFinish = 1;

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

		//增加局数
		increase_round();

		//玩家本局结算分数
		Lint t_roundScore[SHUANGSHENG_PLAY_USER_COUNT];
		memset(t_roundScore, 0 , sizeof(t_roundScore));
		
		//保存本局庄家位置
		Lint t_zhuangPos = m_zhuangPos;

		//保存本局闲家位置
		Lint t_xianPos = (m_zhuangPos + 1) % SHUANGSHENG_PLAY_USER_COUNT;

		//庄家升级还是闲家升级？ 0：庄家升级  1：闲家升级  2：闲家上庄（都不升级）
		Lint t_whoZhuangNext = 2;

		//本局升级级数
		Lint t_shengJiShu = 0;

		//闲家总得分（后面加上闲家扣抵分数）
		Lint t_getTotleScore = 0;
		t_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);

		//闲家胜利底分倍数至少为2，如果有对子或者拖拉机之类的再加倍
		Lint t_baseTimes = 0;

		//闲家胜利扣抵分数
		Lint t_baseSecor = 0;

		//最后一次出牌闲家大，即闲家赢了
		if (m_wTurnWinner % 2 != t_zhuangPos % 2 && m_wTurnWinner >= 0 && m_wTurnWinner < SHUANGSHENG_INVALID_POS)
		{
			//闲家胜利底分倍数至少为2，如果有对子或者拖拉机之类的再加倍
			t_baseTimes = 2;

			//底牌中的分牌
			BYTE tmp_base_cards_score[SHUANGSHENG_BASE_CARDS_COUNT];
			memset(tmp_base_cards_score, 0, sizeof(tmp_base_cards_score));

			//底牌中分牌的数量
			Lint tmp_base_cards_score_count = m_gamelogic.GetScoreCard(m_base_cards, SHUANGSHENG_BASE_CARDS_COUNT, tmp_base_cards_score, SHUANGSHENG_BASE_CARDS_COUNT);
			
			//底牌中总分（实际分数，不加倍）
			Lint t_getBaseScore = m_gamelogic.GetCardScore(tmp_base_cards_score, tmp_base_cards_score_count);

			//最后一次出牌对子数量
			Lint cbMaxCount = 0;

			//最后一轮出牌分析牌型（最大的牌）
			tagAnalyseResult AnalyseResult;
			m_gamelogic.AnalyseCardData(m_out_cards[m_wTurnWinner], m_out_cards_count[m_wTurnWinner], AnalyseResult);

			//对子分析           CountArray(AnalyseResult.SameDataInfo) = 2
			for (BYTE i = 0; i < CountArray(AnalyseResult.SameDataInfo); i++)
			{
				//if (AnalyseResult.SameDataInfo[i].cbBlockCount > 0) cbMaxCount = __max(i, cbMaxCount);
				if (AnalyseResult.SameDataInfo[i].cbBlockCount > 0 && i > 0) cbMaxCount = 2;
			}

			//拖牌分析
			for (BYTE i = 0; i < CountArray(AnalyseResult.TractorDataInfo); i++)
			{
				//获取属性
				BYTE cbSameCount = i + 2;
				BYTE cbMaxLength = AnalyseResult.TractorDataInfo[i].cbTractorMaxLength;

				//设置变量
				cbMaxCount = __max(cbSameCount * cbMaxLength, cbMaxCount);
			}

			t_baseTimes = cbMaxCount + t_baseTimes;

			t_baseSecor = t_getBaseScore * t_baseTimes;

			//闲家总得分
			t_getTotleScore = t_getTotleScore + t_baseSecor;

			LLOG_DEBUG("ShuangShengGameHandler::finish_round()...  cbMaxCount=[%d], BaseTimes=[%d]", cbMaxCount,t_baseTimes);
		}
		

		//闲家抓分 < 80分，庄家升级
		if (t_getTotleScore < 80)
		{
			//下局庄家对家坐庄
			m_zhuangPos = (m_zhuangPos + 2) % m_player_count;

			//庄家升级
			t_whoZhuangNext = 0;

			//本局庄家赢分
			Lint t_roundCommonScore = 1;

			//庄家升级数
			Lint t_zhuangShengJi = 0;

			if (t_getTotleScore == 0)
			{
				t_zhuangShengJi = 3;
			}
			else
			{
				t_zhuangShengJi = 2 - (t_getTotleScore / 40);
			}

			//庄家升级后超过K
			if (m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi > 13)
			{
				if (m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi == 14)
				{
					m_gradeCard[t_zhuangPos % 2] = 1;
				}
				else if(m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi > 14)
				{ 
					m_gradeCard[t_zhuangPos % 2] = (m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi - 14) + (m_is2ChangZhu ? 3 : 2) - 1;
				}
			}
			//庄家升级后不超过K
			else
			{
				m_gradeCard[t_zhuangPos % 2] += (m_gradeCard[t_zhuangPos % 2] == 1 && m_is2ChangZhu) ? t_zhuangShengJi + 1 : t_zhuangShengJi;
			}

			//计算各个玩家得分，庄升3级闲扣8分，庄升2级闲扣4分，庄升1级闲扣2分
			for (int i = 0; i < t_zhuangShengJi; ++i)
			{
				t_roundCommonScore *= 2;
			}
			if (t_roundCommonScore >= 16)
			{
				t_roundCommonScore = 16;
			}
			t_roundCommonScore = t_roundCommonScore * m_baseTimes;
			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
			{
				if (i % 2 == t_zhuangPos % 2) t_roundScore[i] += t_roundCommonScore;
				else t_roundScore[i] -= t_roundCommonScore;
			}
			t_shengJiShu = t_zhuangShengJi;

		}
		
		//闲家抓分 >= 80分，庄家轮换，升级
		else if (t_getTotleScore >= 80)
		{
			//换庄，上一局庄家的下家坐庄
			m_zhuangPos = (m_zhuangPos + 1) % m_player_count;

			if (t_getTotleScore < 120)
			{
				t_whoZhuangNext = 2;
			}
			else if (t_getTotleScore >= 120)
			{
				t_whoZhuangNext = 1;
			}

			//本局闲家赢分
			Lint t_roundCommonScore = 1;

			//闲家升级数
			Lint t_xianShengJi = (t_getTotleScore / 40) - 2;

			//闲家升级后超过K
			if (m_gradeCard[t_xianPos % 2] + t_xianShengJi > 13)
			{
				if (m_gradeCard[t_xianPos % 2] + t_xianShengJi == 14)
				{
					m_gradeCard[t_xianPos % 2] = 1;
				}
				else if (m_gradeCard[t_xianPos % 2] + t_xianShengJi > 14)
				{
					m_gradeCard[t_xianPos % 2] = (m_gradeCard[t_xianPos % 2] + t_xianShengJi - 14) + (m_is2ChangZhu ? 3 : 2) - 1;
				}
			}
			//闲家升级后不超过K
			else
			{
				m_gradeCard[t_xianPos % 2] += (m_gradeCard[t_xianPos % 2] == 1 && m_is2ChangZhu) ? t_xianShengJi + 1 : t_xianShengJi;
			}

			//各个玩家得分
			for (int i = 0; i <= t_xianShengJi; ++i)
			{
				t_roundCommonScore *= 2;
			}
			if (t_roundCommonScore >= 16)
			{
				t_roundCommonScore = 16;
			}
			t_roundCommonScore = t_roundCommonScore * m_baseTimes;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (i % 2 == t_xianPos % 2) t_roundScore[i] += t_roundCommonScore;
				else t_roundScore[i] -= t_roundCommonScore;
			}

			t_shengJiShu = t_xianShengJi;
		}

		//算玩家总得分
		for (int i = 0; i < m_player_count; ++i)
		{
			m_accum_score[i] += t_roundScore[i];
		}

		////玩家解散房间，不发小结算消息
		//if (true == jiesan)
		//{
		//	LLOG_ERROR("ShuangShengGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());
		//	ShuangShengS2CBCDrawResult jiesan;
		//	jiesan.m_isFinish = 1;

		//	memcpy(jiesan.m_totleScore, m_accum_score, sizeof(m_accum_score));
		//	for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
		//	{
		//		if (m_desk->m_user[i])
		//		{
		//			jiesan.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
		//			jiesan.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
		//			jiesan.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
		//		}
		//	}

		//	notify_desk(jiesan);

		//	LLOG_ERROR("Game Jie San Over... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

		//	set_desk_state(DESK_WAIT);

		//	//结束游戏
		//	if (m_desk) m_desk->HanderGameOver(1);
		//	return;
		//}

		//记录总分、写入数据库信息
		add_round_log(t_roundScore, m_wTurnWinner);

		//推小结算消息
		ShuangShengS2CBCDrawResult result;
		result.m_winnerPos = m_wTurnWinner;
		result.m_zhuangPos = t_zhuangPos;  //本局玩家位置
		result.m_baseTimes = t_baseTimes;
		result.m_baseScore = t_baseSecor;
		result.m_nextZhuangPos = m_zhuangPos;  //下一局玩家位置
		for(int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
		{
			result.m_baseCards[i] = (Lint)m_base_cards[i];
		}
		result.m_getTotleScore = t_getTotleScore;
		memcpy(result.m_thisScore, t_roundScore, sizeof(t_roundScore));
		memcpy(result.m_totleScore, m_accum_score, sizeof(m_accum_score));
		memcpy(result.m_upDate, m_gradeCard, sizeof(m_gradeCard));
		
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
		{
			if(m_desk->m_user[i])
			{ 
				result.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
				result.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
				result.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
			}
		}

		result.m_type = t_whoZhuangNext;
		result.m_nextCard = t_shengJiShu;
		result.m_isFinish = (m_round_offset >= m_round_limit) ? 1 : 0;

		set_desk_state(DESK_WAIT);
		notify_desk(result);

		m_desk->HanderGameOver(1);
	}

};

/* 双升游戏处理器 */
struct ShuangShengGameHandler : ShuangShengGameCore
{

	/* 构造函数 */
	ShuangShengGameHandler()
	{
		LLOG_DEBUG("ShuangSheng GameHandler Init Over...");
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

	/* 每局开始*/
	bool startup(Desk *desk) {
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}
	/* 每局结束*/
	void shutdown(void) {
		clear_match(0);
	}


	/* 设置玩法小选项，被Desk::SetVip()调用 */
	void SetPlayType(std::vector<Lint>& l_playtype)
	{
		// 被 Desk 的 SetVip 调用，此时能得到 m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		//小选项：底分
		m_baseTimes = m_playtype.ShuangShengBaseTimes();

		//小选项：是否随机主
		m_isRandZhu = m_playtype.ShuangShengIsRandZhu();

		//小选项：是否防作弊
		m_isAntiCheating = m_playtype.ShuangShengIsAntiCheating();

		//小选项：2是否为常主
		m_is2ChangZhu = m_playtype.ShuangSheng2IsChangZhu();

		//第一局需要设置一些变量
		if (m_round_offset == 0)
		{
			//设置扑克副数
			m_gamelogic.SetPackCount(m_packCount);

			srand(time(NULL));

			//2常主，
			if (m_is2ChangZhu)
			{
				m_gamelogic.SetChangZhu(0x02);
				m_gradeCard[0] = 3;
				m_gradeCard[1] = 3;
			}
			
			//开局随机打几
			if (m_isRandZhu)
			{
				Lint t_randZhu = rand() % 11 + 3;
				m_gradeCard[0] = t_randZhu;
				m_gradeCard[1] = t_randZhu;
			}

			//配置文件中是否配置允许调牌
			bool can_assign = gConfig.GetDebugModel();

			FILE *fp = NULL;
			try
			{
				fp = fopen(DEBUG_CARD_FILE_SHUANGSHENG, "r");
			}
			catch (const std::exception& e)
			{
				LLOG_ERROR("File card.ini is open failed...  desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			}

			//允许服务器端配牌 && card.ini存在
			if (can_assign && fp)
			{
				LLOG_DEBUG("Grade Cards...This is debug mode!!! desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);
				//级牌
				std::vector<Lint> grade;
				grade.resize(2);
				LoadGradeCard(grade, DEBUG_CARD_FILE_SHUANGSHENG);

				if(grade.size() > 1 && grade[0] >= (m_is2ChangZhu ? 3 : 2) && grade[0] <= 13)
				{
					m_gradeCard[0] = grade[0];
				}
				if (grade.size() > 2 && grade[1] >= (m_is2ChangZhu ? 3 : 2) && grade[1] <= 13)
				{
					m_gradeCard[1] = grade[1];
				}
				m_gamelogic.SetMainValue(m_gradeCard[0]);

				fclose(fp);
				fp = NULL;
			}
		}
		


		//暂时不用..............................................................................
		//小选项：庄家规则
		m_playMode = m_playtype.ShuangShengPlayMode();

		//小选项：牌副数（默认为2）
		m_packCount = m_playtype.ShuangShengPackCount();

		LLOG_ERROR("ShuangSheng::SetPlayType() Run... This Match Play Type deskId=[%d], baseTimes=[%d], isRandZhu=[%d], isAntiCheating=[%d], is2ChangZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_baseTimes, m_isRandZhu, m_isAntiCheating, m_is2ChangZhu);
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
		LLOG_DEBUG("desk_id=[%d], ShuangSheng---------------------MHSetDeskPlay(Lint play_user_count) = [%d]", m_desk ? m_desk->GetDeskId() : 0, play_user_count);
		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		m_player_count = play_user_count;

		//第一局需要设置一些变量，防作弊：随机分配玩家作为
		if (m_round_offset == 0 && m_isAntiCheating)
		{
			srand(time(NULL));
			ShuangShengS2CFirstRound firstRound;
			Lint t_newPosID[4] = { 0, 1, 2, 3 };
			int t_newPos1 = SHUANGSHENG_INVALID_POS;
			int t_newPos2 = SHUANGSHENG_INVALID_POS;
			t_newPos1 = rand() % 4;
			do
			{
				t_newPos2 = rand() % 4;
			} while (t_newPos1 == t_newPos2);

			User* p_user = NULL;

			p_user = m_desk->m_user[t_newPos1];
			m_desk->m_user[t_newPos1] = m_desk->m_user[(t_newPos1 + 1) % 4];
			m_desk->m_user[(t_newPos1 + 1) % 4] = p_user;

			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
			{
				if (m_desk->m_user[i] == NULL) continue;
				t_newPosID[i] = m_desk->m_user[i]->GetUserDataId();
			}
			memcpy(firstRound.m_newSeatUserId, t_newPosID, sizeof(t_newPosID));
			notify_desk(firstRound);
		}

		start_round(player_status);
	}

	/********************************************************************************
	* 函数名：      HandleShuangShengSelectZhu()
	* 描述：		处理双升玩家报主
	* 参数：
	*  @pUser* 玩家对象
	*  @msg* 玩家报主消息
	* 返回：        (bool)
	*********************************************************************************/
	bool HandleShuangShengSelectZhu(User* pUser, ShuangShengC2SSelectZhu* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			return false;
		}
		if (!(GAME_PLAY_SELECT_ZHU == m_play_status || GAME_PLAY_SEND_CARDS == m_play_status))
		{
			LLOG_ERROR("HandleShuangShengSelectZhu() Error... Current Play Status is not GAME_PLAY_SELECT_ZHU or GAME_PLAY_SEND_CARDS... deskId=[%d], userId=[%d], currStatus=[%d]", 
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SHUANGSHENG_INVALID_POS)
		{
			LLOG_ERROR("HandleShuangShengSelectZhu() Error... Select Zhu Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		BYTE selectCard = msg->m_selectCard;
		BYTE selectCardCount = msg->m_selectCardCount;

		return on_event_user_select_zhu(GetUserPos(pUser), selectCard, selectCardCount, msg->m_count);
	}

	/* 处理玩家盖底牌 */
	bool HandleShuangShengBaseCards(User* pUser, ShuangShengC2SBaseCards* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			return false;
		}
		if (GAME_PLAY_BASE_CARD != m_play_status)
		{
			LLOG_ERROR("desk_id=[%d], HandleShuangShengBaseCards error, user[%d] , status not GAME_PLAY_BASE_CARD, [%d]", m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SHUANGSHENG_INVALID_POS)
		{
			LLOG_ERROR("desk_id=[%d], HandleShuangShengBaseCards  user[%d] pos not ok [%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		int t_baseCardsCount = msg->m_baseCardsCount;
		int t_baseCards[SHUANGSHENG_BASE_CARDS_COUNT];
		
		if (t_baseCardsCount != 8)
		{
			LLOG_ERROR("desk_id=[%d], HandleShuangShengBaseCards  user[%d] pos not ok [%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		
		return on_event_user_base_cards(GetUserPos(pUser), msg);
	}

	/* 玩家出牌 */
	bool HandleShuangShengOutCards(User* pUser, ShuangShengC2SOutCards* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			return false;
		}

		if (GAME_PLAY_PLAY_CARD != m_play_status)
		{
			LLOG_ERROR("HandleShuangShengOutCards() Error..., Current Status is not GAME_PLAY_PLAY_CARD..., deskId=[%d], userId=[%d], currStatus=[%d]", 
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == SHUANGSHENG_INVALID_POS)
		{
			LLOG_ERROR("HandleShuangShengBaseCards() Error...,  user[%d] pos not ok [%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		BYTE cbCardCount = (BYTE)msg->m_outCardsCount;
		BYTE cbCardData[SHUANGSHENG_HAND_CARDS_COUNT];
		memset(cbCardData, 0, sizeof(cbCardData));
		for (int i = 0; i < cbCardCount; ++i)
		{
			cbCardData[i] = (BYTE)msg->m_outCards[i];
		}

		return on_event_user_out_cards(GetUserPos(pUser), cbCardData, cbCardCount);
	}

	/* 处理玩家断线重连 */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk) 
		{
			LLOG_ERROR("****OnUserReconnect...desk_id=[%d], ShuangSheng : NULL user or desk poniter user:%X, m_desk:%X", m_desk, pUser, m_desk);
			return;
		}

		//Desk 已经发送
		notify_desk_match_state();

		Lint pos = GetUserPos(pUser);

		if (SHUANGSHENG_INVALID_POS == pos)
		{
			LLOG_ERROR("****OnUserReconnect...desk_id=[%d], ShuangSheng user[id:%d] pos error!!!", m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		LLOG_DEBUG("Handler::OnUserReconnect() Run... deskId=[%d], userId=[%d], pos=[%d], curPos=[%d], zhuangPos=[%d], playStatus=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), pos, m_curPos, m_zhuangPos, m_play_status);

		ShuangShengS2CRecon reconn;
		reconn.m_status = m_play_status;
		reconn.m_zhuangPos = m_zhuangPos;
		reconn.m_currGrade = m_mainValue;
		reconn.m_mainColor = m_mainColor;
		reconn.m_selectCardValue = m_selectCard;
		reconn.m_selectCard = m_logicMainCard;
		reconn.m_selectCardCount = m_selectCardCount;
		reconn.m_selectCardPos = m_selectCardPos;
		reconn.m_gradeCard[0] = m_gradeCard[0];
		reconn.m_gradeCard[1] = m_gradeCard[1];
		reconn.m_outCardPos = m_curPos;

		reconn.m_handCardsCount = m_hand_cards_count[pos];
		for (int i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			reconn.m_handCards[i] = m_hand_cards[pos][i];
		}
		
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			reconn.m_outCardsCount[i] = m_out_cards_count[i];
			for (int j = 0; j < SHUANGSHENG_HAND_CARDS_COUNT; ++j)
			{
				reconn.m_outCards[i][j] = m_out_cards[i][j];
			}
		}
		reconn.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
		reconn.m_scoreCardsCount = m_cbScoreCardCount;
		for (int i = 0; i < m_cbScoreCardCount; ++i)
		{
			reconn.m_scoreCards[i] = m_cbScoreCardData[i];
		}

		//断线重连阶段
		switch (m_play_status)
		{

		//叫主阶段断线
		case GAME_PLAY_SELECT_ZHU:
		{

			break;
		}

		//盖底牌阶段断线
		case GAME_PLAY_BASE_CARD:
		{
			if (m_zhuangPos == pos)
			{
				for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
				{
					reconn.m_handCards[m_hand_cards_count[pos] + i] = m_base_cards[i];
					reconn.m_baseCards[i] = m_base_cards[i];
				}
				reconn.m_handCardsCount = SHUANGSHENG_MAX_HAND_CARDS_COUNT;
				LLOG_DEBUG("Handler::OnUserReconnect()... Zhuang reconnect on BaseCards status... deskId=[%d], handCardCount=[%d], playStatus=[%d]", 
					m_desk ? m_desk->GetDeskId() : 0, reconn.m_handCardsCount, m_play_status);
			}
			break;
		}

		//打牌阶段断线重连
		case GAME_PLAY_PLAY_CARD:
		{
			reconn.m_firstOutPos = m_firstOutPos;
			reconn.m_currBig = (int)m_gamelogic.CompareCardArray_n(m_out_cards, m_out_cards_count[m_firstOutPos], m_firstOutPos, GetHasOutPlayerCount());
			break;
		}

		//所有人出牌结束
		case GAME_PLAY_END:
		{
			for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
			{
				reconn.m_baseCards[i] = m_base_cards[i];
			}
			break;
		}
		}
		notify_user(reconn, pos);
	}


	/* 处理游戏结束(解散) */
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


	/* 定时器 */
	void Tick(LTime& curr)
	{
		//发牌定时器，定时5s发完自动转到亮主阶段，处理都叫主
		/*if (m_tick_flag && m_play_status == GAME_PLAY_SEND_CARDS && curr.Secs() >= m_play_status_time.Secs() + DEF_TIME_SEND_CARDS)
		{
			LLOG_DEBUG("Tick: Client Send Cards Time Over, Start Select Main Color Tick... desk_id=[%d], zhuang_pos=[%d], main_value=[%d],  main_color=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_mainValue, m_mainColor);

			自动设置游戏状态为报主状态
			set_play_status(GAME_PLAY_SELECT_ZHU);
			m_play_status_time.Now();
		}*/

		//叫主定时器
		if (m_tick_flag && (m_play_status == GAME_PLAY_SELECT_ZHU || m_play_status == GAME_PLAY_SEND_CARDS) && curr.Secs() >= m_play_status_time.Secs() + DEF_TIME_SELECT_ZHU + (m_isAntiCheating ? 5 : 0))
		{
			//关闭定时器开关
			m_tick_flag = false;

			//设置游戏状态为盖底牌操作
			//set_play_status(GAME_PLAY_BASE_CARD);

			//叫主结束处理
			select_zhu_over();
		}

		/*if (m_tick_flag && m_play_status == GAME_PLAY_BASE_CARD && curr.Secs() >= m_play_status_time.Secs() + DEF_TIME_BASE_CARDS)
		{
			
		}*/
	}

};


DECLARE_GAME_HANDLER_CREATOR(106, ShuangShengGameHandler);