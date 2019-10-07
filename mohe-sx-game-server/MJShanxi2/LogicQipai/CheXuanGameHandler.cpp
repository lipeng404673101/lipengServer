#include "CheXuanGameLogic.h"
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
static bool debugSendCards_CheXuan(Lint* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
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
		if (fp) fclose(fp);
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


		//补全手牌
		for (int i = 0; i < playerCount; ++i)
		{
			for (int j = i * handCardsCount + t_sendCount[i]; j < (i + 1) * handCardsCount; )
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

/* 扯炫游戏中的各个阶段（枚举） */
enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = 0,			/* 0游戏开始 */
	GAME_PLAY_QIBOBO,				/* 1下钵钵 */
	GAME_PLAY_ADD_MANGGUO,			/* 2下芒果 */
	GAME_PLAY_ADD_MANGSCORE,		/* 3下芒注 */
	GAME_PLAY_SEND_CARD,			/* 4发牌 */
	GAME_PLAY_ADD_SCORE,			/* 5下注 */
	GAME_PLAY_CUO_PAI,				/* 6搓牌 */
	GAME_PLAY_CHE_PAI,				/* 7扯牌 */
	GAME_PLAY_END,					/* 8游戏结束 */
};

/* 下注操作类型 */
enum ADD_SCORE_OPT
{
	ADD_OPT_INVALID = -1,			/* 无效操作 */
	ADD_OPT_FIRST_QIAO = 0,			/* 瞧：挡家第一轮中一次下注(之前为发招) */
	ADD_OPT_GEN = 1,				/* 跟 */
	ADD_OPT_FA_ZHAO = 2,			/* 发招，之前为大 */
	ADD_OPT_QIAO_BO = 3,			/* 敲钵 */
	ADD_OPT_XIU = 4,				/* 休 */
	ADD_OPT_SHUAI = 5,				/* 甩 */
	ADD_OPT_SANHUA = 6,				/* 三花 */
	ADD_OPT_MAX_INVALID				/* 最大无效操作 */
};

/* 扯炫游戏中的音效 */
enum GAME_PLAY_SOUND
{

};

enum OVER_STATE
{

};

#define DEF_BASE_TIME 0
/* 扯炫默认操作时间 */
enum GAME_DEFAULT_TIME
{
	DEF_TIME_INVALID = -1,
	DEF_TIME_QIBOBO = DEF_BASE_TIME + 60,		//起钵钵
	DEF_TIME_OPTSCORE = DEF_BASE_TIME + 60,		//前两轮下注
	DEF_TIME_3RD_OPTSCORE = DEF_BASE_TIME + 65,	//第三轮下注
	DEF_TIME_CUOPAI = DEF_BASE_TIME + 60,		//搓牌
	DEF_TIME_CHEPAI = DEF_BASE_TIME + 60,		//扯牌

	DEF_TIME_FINISH = DEF_BASE_TIME + 40,		//自动下一局时间，desk上的时间要同步改

	DEF_TIME_COINS_LOW = 30,					//玩家能量值不做提示的间隔时间
};

enum GAME_DEFAULT_TIME_FAST
{
	DEF_TIME_FAST_INVALID = -1,
	DEF_TIME_FAST_QIBOBO = DEF_BASE_TIME + 5,		//起钵钵
	DEF_TIME_FAST_OPTSCORE = DEF_BASE_TIME + 10,		//前两轮下注
	DEF_TIME_FAST_3RD_OPTSCORE = DEF_BASE_TIME + 15,	//第三轮下注
	DEF_TIME_FAST_CUOPAI = DEF_BASE_TIME + 5,		//搓牌
	DEF_TIME_FAST_CHEPAI = DEF_BASE_TIME + 20,		//扯牌

	DEF_TIME_FAST_FINISH = DEF_BASE_TIME + 3,		//自动下一局时间，desk上的时间要同步改

	DEF_TIME_FAST_COINS_LOW = 30,					//玩家能量值不做提示的间隔时间
};


/* 扯炫游戏小选项(结构体) */
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
	* 函数名：   CheXuanBaseTimes()
	* 描述：     获取客户端选择的底分
	* 详细：     底分倍数：1、1分，2、2分，3、3分
	* 返回：     (Lint)客户端选择的底分
	********************************************************/
	Lint PlayType::CheXuanBaseTimes() const;

	/******************************************************
	* 函数名：   CheXuanDiPi()
	* 描述：     客户端选择底、皮
	* 详细：     0：1/3     1：5/1(5/10)     2：5/2(5/20)		3：2/5
	* 返回：     (Lint)客户端选择的底皮
	********************************************************/
	Lint PlayType::CheXuanDiPi() const;

	/******************************************************
	* 函数名：   CheXuanMaxBoBo()
	* 描述：     客户端允许的最大钵数
	* 详细：     500   1000   2000
	* 返回：     (Lint)
	********************************************************/
	Lint PlayType::CheXuanMaxBoBo() const;

	/******************************************************
	* 函数名：   CheXuanPlayerCount()
	* 描述：     客户端规定的最大人数
	* 详细：     2：2人    5:5人    8：8人
	* 返回：     (Lint)
	********************************************************/
	Lint PlayType::CheXuanPlayerCount() const;

	/******************************************************
	* 函数名：   CheXuanDynamicStart()
	* 描述：     是否动态开局
	* 详细：     0:不支持   1：支持
	* 返回：     (bool)
	********************************************************/
	bool PlayType::CheXuanDynamicIn() const;

	/******************************************************
	* 函数名：   CheXuanIsZouMang()
	* 描述：     是否允许揍芒
	* 详细：     0：不揍芒    1：揍芒
	* 返回：     (bool)
	********************************************************/
	bool PlayType::CheXuanIsZouMang() const;

	/******************************************************
	* 函数名：   CheXuanIsXiuMang()
	* 描述：     是否允许休芒
	* 详细：     0：不休芒    1：休芒
	* 返回：     (bool)
	********************************************************/
	bool PlayType::CheXuanIsXiuMang() const;

	/******************************************************
	* 函数名：   CheXuanIsShouShouMang()
	* 描述：     是否允许手手芒
	* 详细：     0：不手手芒    1：手手芒
	* 返回：     (bool)
	********************************************************/
	bool PlayType::CheXuanIsShouShouMang() const;

	/******************************************************
	* 函数名：   CheXuanIsLianXuDaMangFanBei()
	* 描述：     连续打芒翻倍
	* 详细：     0：不翻倍    1：翻倍
	* 返回：     (bool)
	********************************************************/
	bool PlayType::CheXuanIsLianXuDaMangFanBei() const;

	/******************************************************
	* 函数名：   CheXuanIsFanBeiFengDing()
	* 描述：     是否允许翻倍封顶
	* 详细：     0：不封顶    1：封顶（封顶多少？？？）
	* 返回：     (Lint)是否允许庄家交牌
	********************************************************/
	bool  PlayType::CheXuanIsFanBeiFengDing() const;

	/******************************************************
	* 函数名：   CheXuanIsClearPool()
	* 描述：     是否每局清空芒果池
	* 详细：     0：不允许    1：允许
	* 返回：     (Lint)是否允每局清空芒果、皮池
	********************************************************/
	bool  PlayType::CheXuanIsClearPool() const;

	/******************************************************
	* 函数名：   CheXuanLookOnDarkResult()
	* 描述：     是否观战暗牌结算
	* 详细：     0：否    1：是
	* 返回：     (Lint)
	********************************************************/
	bool  PlayType::CheXuanLookOnDarkResult() const;

	/******************************************************
	* 函数名：   CheXuanStartNotInRoom()
	* 描述：     是否开局后禁止加入房间
	* 详细：     0：开始后不限制    1：开始后不允许加入房间
	* 返回：     (Lint)
	********************************************************/
	bool  PlayType::CheXuanStartNotInRoom() const;

	/******************************************************
	* 函数名：   CheXuanMoFen()
	* 描述：     扯炫默分选项
	* 详细：     0：不默分    1：默分
	* 返回：     (Lint)
	********************************************************/
	bool PlayType::CheXuanMoFen() const;

	/******************************************************
	* 函数名：   IsFirstAddBigMang()
	* 描述：     扯炫是否首家分大于芒果
	* 详细：     0：正常模式    1：首家分大于芒果
	* 返回：     (Lint)
	********************************************************/
	bool PlayType::IsFirstAddBigMang() const;

	/******************************************************
	* 函数名：   IsAllowDiJiu()
	* 描述：     扯炫是否允许地九王
	* 详细：     0：不允许地九王    1：允许地九王
	* 返回：     (Lint)
	********************************************************/
	bool PlayType::IsAllowDiJiu() const;

	/******************************************************
	* 函数名：   IsFastMode()
	* 描述：     扯炫是否快速模式
	* 详细：     0：正常模式    1：急速模式
	* 返回：     (Lint)
	********************************************************/
	bool PlayType::IsFastMode() const;

	/******************************************************
	* 函数名：   IsQiPaiNotShowCards()
	* 描述：     扯炫是否弃牌后小结算不显示牌型
	* 详细：     0：正常模式    1：弃牌后小结算不显示牌型
	* 返回：     (Lint)
	********************************************************/
	bool PlayType::IsQiPaiNotShowCards() const;

	/******************************************************
	* 函数名：   IsLongXinFeiMaxScore()
	* 描述：     扯炫是否输分龙心肺封顶分
	* 详细：     0：不开启   1：开启
	* 返回：     (Lint)
	********************************************************/
	bool PlayType::IsLongXinFeiMaxScore() const;
};



/******************************************************
* 函数名：   CheXuanBaseTimes()
* 描述：     获取客户端选择的底分
* 详细：     底分倍数：1、1分，2、2分，3、3分
* 返回：     (Lint)客户端选择的底分
********************************************************/
Lint PlayType::CheXuanBaseTimes() const
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
* 函数名：   CheXuanDiPi()
* 描述：     客户端选择底、皮
* 详细：     0：1/3     1：5/1(5/10)     2：5/2(5/20)		3：2/5
* 返回：     (Lint)客户端选择的底皮
********************************************************/
Lint PlayType::CheXuanDiPi() const
{
	if (m_playtype.size() < 2)
	{
		return false;
	}

	return m_playtype[1] % 4;
}

/******************************************************
* 函数名：   CheXuanMaxBoBo()
* 描述：     客户端允许的最大钵数
* 详细：     500   1000   2000   3000   5000
* 返回：     (Lint)
********************************************************/
Lint PlayType::CheXuanMaxBoBo() const
{
	if (m_playtype.size() < 3)
	{
		return false;
	}

	return m_playtype[2];
}



/******************************************************
* 函数名：   CheXuanPlayerCount()
* 描述：     客户端规定的最大人数
* 详细：     2：2人    5:5人    8：8人
* 返回：     (Lint)
********************************************************/
Lint PlayType::CheXuanPlayerCount() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}

	return m_playtype[3];
}

/******************************************************
* 函数名：   CheXuanDynamicStart()
* 描述：     是否动态开局
* 详细：      0:不支持   1：支持
* 返回：     (bool)
********************************************************/
bool PlayType::CheXuanDynamicIn() const
{
	if (m_playtype.size() < 5)
	{
		return false;
	}

	return m_playtype[4];
}

/******************************************************
* 函数名：   CheXuanIsZouMang()
* 描述：     是否允许揍芒
* 详细：     0：不揍芒    1：揍芒
* 返回：     (bool)
********************************************************/
bool PlayType::CheXuanIsZouMang() const
{
	if (m_playtype.size() < 6)
	{
		return false;
	}

	return m_playtype[5] == 1;
}

/******************************************************
* 函数名：   CheXuanIsXiuMang()
* 描述：     是否允许休芒
* 详细：     0：不休芒    1：休芒
* 返回：     (bool)
********************************************************/
bool PlayType::CheXuanIsXiuMang() const
{
	if (m_playtype.size() < 7)
	{
		return false;
	}

	return m_playtype[6] == 1;
}

/******************************************************
* 函数名：   CheXuanIsShouShouMang()
* 描述：     是否允许手手芒
* 详细：     0：不手手芒    1：手手芒
* 返回：     (bool)
********************************************************/
bool PlayType::CheXuanIsShouShouMang() const
{
	if (m_playtype.size() < 8)
	{
		return 0;
	}
	return m_playtype[7] == 1;
}

/******************************************************
* 函数名：   CheXuanIsLianXuDaMangFanBei()
* 描述：     连续打芒翻倍
* 详细：     0：不翻倍    1：翻倍
* 返回：     (bool)
********************************************************/
bool PlayType::CheXuanIsLianXuDaMangFanBei() const
{
	if (m_playtype.size() < 9)
	{
		return 0;
	}
	return m_playtype[8] == 1;
}

/******************************************************
* 函数名：   CheXuanIsFanBeiFengDing()
* 描述：     是否允许翻倍封顶
* 详细：     0：不封顶    1：封顶（封顶多少？？？）
* 返回：     (Lint)是否允许庄家交牌
********************************************************/
bool  PlayType::CheXuanIsFanBeiFengDing() const
{
	if (m_playtype.size() < 10)
	{
		return 0;
	}
	return m_playtype[9] == 1;
}

/******************************************************
* 函数名：   CheXuanIsClearPool()
* 描述：     是否每局清空芒果池
* 详细：     0：不清空    1：清空
* 返回：     (Lint)是否允每局清空芒果、皮池
********************************************************/
bool  PlayType::CheXuanIsClearPool() const
{
	if (m_playtype.size() < 11)
	{
		return 1;
	}
	return m_playtype[10] == 0;
}

/******************************************************
* 函数名：   CheXuanLookOnDarkResult()
* 描述：     是否观战暗牌结算
* 详细：     0：否    1：是
* 返回：     (Lint)
********************************************************/
bool  PlayType::CheXuanLookOnDarkResult() const
{
	if (m_playtype.size() < 12)
	{
		return false;
	}
	return m_playtype[11] == 1;
}

/******************************************************
* 函数名：   CheXuanStartNotInRoom()
* 描述：     是否开局后禁止加入房间
* 详细：     0：开始后不限制    1：开始后不允许加入房间
* 返回：     (Lint)
********************************************************/
bool  PlayType::CheXuanStartNotInRoom() const
{
	if (m_playtype.size() < 13)
	{
		return false;
	}

	return m_playtype[12] == 1;
}

/******************************************************
* 函数名：   CheXuanMoFen()
* 描述：     扯炫默分选项
* 详细：     0：不默分    1：默分
* 返回：     (Lint)
********************************************************/
bool PlayType::CheXuanMoFen() const
{
	if (m_playtype.size() < 14)
	{
		return false;
	}

	return m_playtype[13] == 1;
}

/******************************************************
* 函数名：   IsFirstAddBigMang()
* 描述：     扯炫是否首家分大于芒果
* 详细：     0：正常模式    1：首家分大于芒果
* 返回：     (Lint)
********************************************************/
bool PlayType::IsFirstAddBigMang() const
{
	if (m_playtype.size() < 15)
	{
		return false;
	}

	return m_playtype[14] == 1;
}

/******************************************************
* 函数名：   IsAllowDiJiu()
* 描述：     扯炫是否允许地九王
* 详细：     0：不允许地九王    1：允许地九王
* 返回：     (Lint)
********************************************************/
bool PlayType::IsAllowDiJiu() const
{
	if (m_playtype.size() < 16)
	{
		return false;
	}

	return m_playtype[15] == 1;
}

/******************************************************
* 函数名：   IsFastMode()
* 描述：     扯炫是否快速模式
* 详细：     0：正常模式    1：急速模式
* 返回：     (Lint)
********************************************************/
bool PlayType::IsFastMode() const
{
	if (m_playtype.size() < 17)
	{
		return false;
	}

	return m_playtype[16] == 1;
}

/******************************************************
* 函数名：   IsQiPaiNotShowCards()
* 描述：     扯炫是否弃牌后小结算不显示牌型
* 详细：     0：正常模式    1：弃牌后小结算不显示牌型
* 返回：     (Lint)
********************************************************/
bool PlayType::IsQiPaiNotShowCards() const
{
	if (m_playtype.size() < 18)
	{
		return false;
	}

	return m_playtype[17] == 1;
}

/******************************************************
* 函数名：   IsLongXinFeiMaxScore()
* 描述：     扯炫是否输分龙心肺封顶分
* 详细：     0：不开启   1：开启
* 返回：     (Lint)
********************************************************/
bool PlayType::IsLongXinFeiMaxScore() const
{
	if (m_playtype.size() < 20)
	{
		return false;
	}

	return m_playtype[19] == 1;
}

/* 扯炫每局不会初始化字段（结构体）*/
struct CheXuanRoundState__c_part
{
	GAME_PLAY_STATE		m_play_status;										//牌局状态

	Lint				m_user_status[CHEXUAN_PLAYER_COUNT];				//玩家状态：1：该玩家参与游戏

	Lint				m_finish_round_type;								//本局结束的类型：0：正常结束   1：休过河  2：休芒   3：一人外所有人弃牌   4：揍芒

	Lint				m_curPos;											//当前操作玩家
	Lint				m_dang_player_pos;									//挡家位置
	Lint				m_turn_first_speak_pos;								//每轮下注，发招的玩家位置，每轮发牌起始玩家的下家
	Lint				m_firstOutPos;										//每轮第一个出牌玩家
	Lint				m_qipai_last_pos;									//最后一个弃牌玩家的位置
	BOOL				m_oneNotQiao;										//一人操作没有敲，其他人都敲了  false：不是   true：是
	BOOL				m_OnlyQiaoAndXiu;									//该局只有敲和休的操作，且休的玩家分的根平  false：不是   true:是
	Lint				m_allQiao;											//所有人都敲了
	Lint				m_selectMoFen;										//选择默分选择  -1：默认初始值   0：不看  1：看牌

	Lint				m_rest_remain_time;									//提出解散距离定时器已经执行的时间  -1：初始值
	BOOL				m_tick_flag;										//定时器开关
	Lint				m_turn_count;										//第几轮下注：0：发两张牌时候第一轮下注，  1：发三张牌时候第二轮下注   2：发四张牌时候第三轮下注
	Lint				m_shuai_count;										//弃牌玩家数量，三花不算在里面

	Lint				m_hand_cards_count[CHEXUAN_PLAYER_COUNT];			//各个玩家手牌数量
	Lint				m_hand_cards[CHEXUAN_PLAYER_COUNT][CHEXUAN_HAND_CARDS_COUNT];		//玩家手牌
	Lint				m_hand_cards_backup[CHEXUAN_PLAYER_COUNT][CHEXUAN_HAND_CARDS_COUNT];		//玩家手牌(原始，没有扯牌排序过的)
	Lint				m_hand_cards_type[CHEXUAN_PLAYER_COUNT][2];			//手牌两组类型

	Lint				m_player_score[CHEXUAN_PLAYER_COUNT];				//玩家本局得分
	Lint				m_total_add_mang_score[CHEXUAN_PLAYER_COUNT];		//本局所有玩家下的芒果数
	Lint				m_total_add_score[CHEXUAN_PLAYER_COUNT];			//本局下注的总分
	Lint				m_qipai_score[CHEXUAN_PLAYER_COUNT];				//玩家弃牌的时候，已经下的分数
	Lint				m_player_add_score[CHEXUAN_PLAYER_COUNT];			//玩家下注的分累计，只有弃牌后是0(以前是龙心肺)
	Lint				m_player_pi_score[CHEXUAN_PLAYER_COUNT];			//玩家赢的皮分
	Lint				m_player_mang_score[CHEXUAN_PLAYER_COUNT];			//玩家赢的芒果分
	Lint				m_player_bobo_add_di_pi[CHEXUAN_PLAYER_COUNT];		//起钵钵需要额外考虑的多起的部分，用来开局一次下注

	Lint				m_allow_opt_type[CHEXUAN_PLAYER_COUNT][10];			//每个玩家当前允许的操作类型
	Lint				m_allow_opt_min_score[CHEXUAN_PLAYER_COUNT][10];	//每个玩家当前允许操作对应的最小值
	Lint				m_allow_opt_max_score[CHEXUAN_PLAYER_COUNT][10];	//每个玩家当前允许操作对应的最大值

	Lint				m_turn_round_opt[CHEXUAN_PLAYER_COUNT];				//每一轮中每一圈下注标志，0不需要下注  1需要下注
	Lint				m_turn_opt_type[3][CHEXUAN_PLAYER_COUNT];			//一轮下注中玩家最近的一次操作
	Lint				m_turn_opt_score[3][CHEXUAN_PLAYER_COUNT];			//一轮下注中玩家操作的总分
	Lint				m_turn_first_opt_type[3][CHEXUAN_PLAYER_COUNT];		//一轮下注中首次下注的类型
	Lint				m_turn_first_opt_score[3][CHEXUAN_PLAYER_COUNT];	//一轮下注中首次下注的分数
	Lint				m_player_last_opt[CHEXUAN_PLAYER_COUNT];			//玩家最有一次操作类型

	Lint				m_is_need_add_bo[CHEXUAN_PLAYER_COUNT];				//是否需要起钵分  0：不需要  1：需要

	Lint				m_qibobo_status[CHEXUAN_PLAYER_COUNT];				//每个玩家起钵钵状态  0：未操作  1：已操作
	Lint				m_xiu_status[CHEXUAN_PLAYER_COUNT];					//每个玩家休的状态 0：未休  1：已经选择休
	Lint				m_qipai_status[CHEXUAN_PLAYER_COUNT];				//每个玩家是否弃牌：0：未弃牌  1：已弃牌  2：三花牌型
	Lint				m_cuopai_status[CHEXUAN_PLAYER_COUNT];				//每个玩家搓牌状态  0：未搓牌  1：已经搓牌
	Lint				m_chepai_status[CHEXUAN_PLAYER_COUNT];				//每个人扯牌操作状态 0：未扯牌  1：已经扯牌过了
	Lint				m_qiao_status[CHEXUAN_PLAYER_COUNT];				//每个玩家敲的状态  0：没有敲  1：敲了

	Lint				m_san_hua_status[CHEXUAN_PLAYER_COUNT];				//是否是三花
	Lint				m_select_san_hua_status[CHEXUAN_PLAYER_COUNT];		//选择三花的状态：0：没有选择   1：选择三花
	Lint				m_double_san_hua_status[CHEXUAN_PLAYER_COUNT];		//三花对状态：  0：不是三花对   1：三花对

	Lint				m_min_qi_bo_score[CHEXUAN_PLAYER_COUNT];			//每个玩家允许的起钵钵的最小值
	Lint				m_max_qi_bo_score[CHEXUAN_PLAYER_COUNT];			//每个玩家允许的起钵钵的最大值

	Lint				m_first_shou_mang_time;								//首次发牌的手手芒延迟时间
	Lint				m_first_xiu_mang_time;								//首次发牌的休芒延迟时间
	Lint				m_first_zou_mang_time;								//首次发牌的揍芒延迟时间

	Lint				m_real_player_count;								//结算时候参与游戏的实际人数，不包括已经弃牌玩家
	Lint				m_after_turn_shuai_delay;							//二三轮中玩家弃牌、三花，此时将有一个动画，下家发牌会有一个延时时间
	bool				m_is_first_opt_fa_zhao;								//是否有人第一次操作大（首次发招玩家大于芒果选项）

	HandCardsTypeInfo	m_handCardsTypeInfo[CHEXUAN_PLAYER_COUNT];			//玩家手牌信息进过排序后的结果

	//自动操作选项
	Lint				m_playerAutoOpt[CHEXUAN_PLAYER_COUNT];				//玩家选择自动操作选项  0：未选择   1：自动弃牌   2：自动弃牌休牌   3：自动瞧


	/* 结构体清零函数 */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_dang_player_pos = CHEXUAN_INVALID_POS;
		m_turn_first_speak_pos = CHEXUAN_INVALID_POS;
		m_qipai_last_pos = CHEXUAN_INVALID_POS;
		m_rest_remain_time = -1;
		m_selectMoFen = -1;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < CHEXUAN_PLAYER_COUNT; ++j)
			{
				m_turn_opt_type[i][j] = ADD_OPT_INVALID;
			}
		}

		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				m_allow_opt_type[i][j] = -1;
			}
			m_player_last_opt[i] = -1;
		}
	}
};

/* 扯炫每局会初始化字段（结构体）*/
struct CheXuanRoundState__cxx_part
{
	LTime m_play_status_time;
	LTime m_is_coins_low_time[CHEXUAN_PLAYER_COUNT];

	/* 清零每局字段 */
	void clear_round()
	{
		this->~CheXuanRoundState__cxx_part();
		new (this)CheXuanRoundState__cxx_part;
	}
};

/* 扯炫每场不会初始化的字段（结构体）*/
struct CheXuanMatchState__c_part
{
	Desk*		m_desk;									// 桌子对象
	Lint		m_round_offset;							// 当前局数
	Lint		m_round_limit;							// 创建房间选择的最大局数
	Lint		m_accum_score[CHEXUAN_PLAYER_COUNT];	// 存储玩家总得分
	BOOL		m_dismissed;
	Lint		m_registered_game_type;
	Lint		m_player_count;							// 玩法核心代码所使用的玩家数量字段，根据小选项选择的人数会变化
	Lint		m_zhuangPos;							// 庄家位置
	BOOL		m_isFirstFlag;							// 首局显示开始按钮,true代表首局
	Lint		m_frist_send_card;						// 每局第一个叫牌的位置

	Lint		m_scoreBase;							//最小下注分数  1/3：3     5/1：10    5/2: 20
	Lint		m_scoreTimes;							//下注加倍率分：1/3：1     5/1：5    5/2:5

	Lint		m_per_pi_pool;							//上一局皮池
	Lint		m_per_mang_pool;						//上一局芒果池

	Lint		m_last_get_yu_mang;						//解散或者牌局结束起身玩家分的芒果平分过后剩余余分的玩家位置

	Lint		m_pi_pool;								//皮池
	Lint		m_mang_pool;							//芒果池
	Lint		m_rest_can_start_bo_score[CHEXUAN_PLAYER_COUNT];  //剩余可起钵钵的数量
	Lint		m_has_start_total_bo_score[CHEXUAN_PLAYER_COUNT];	//已经起过的钵钵数
	Lint		m_last_bo_score[CHEXUAN_PLAYER_COUNT];	//每个玩家最后起的钵钵数
	Lint		m_total_bo_score[CHEXUAN_PLAYER_COUNT];	//每个玩家当前钵分
	Lint		m_total_bo_score_after_bobo[CHEXUAN_PLAYER_COUNT];	//每局起钵钵后的玩家钵钵中总分数，用于记录可以赢多少分
	Lint		m_is_coins_low[CHEXUAN_PLAYER_COUNT];	//玩家能量值低于预警  0：能量值正常  1：能量值低于预警

	Lint		m_mangScore;							//盲注
	Lint		m_commonMangCount;
	Lint		m_xiuMangCount;

	Lint		m_is_need_add_xiuMang[CHEXUAN_PLAYER_COUNT];		//是否需要下休芒分  0：不需要  1：需要
	Lint		m_zouMangCount;
	Lint		m_is_need_add_zouMang[CHEXUAN_PLAYER_COUNT];		//是否需要下揍芒分  0：不需要  1：需要
	Lint		m_shouMangCount;
	Lint		m_is_need_add_shouMang[CHEXUAN_PLAYER_COUNT];	//是否需要下手手芒  0：不需要  1：需要

	//站起功能
	Lint		m_player_stand_up[CHEXUAN_PLAYER_COUNT];		//玩家站起坐下标志  0：没有操作(默认坐下状态)   1：当局点击站起(当局还未站起)    2：已经站起状态     3：当局点击坐下（当局未坐下）

	//牌局内战绩记录
	Lint		m_has_record_bobo[CHEXUAN_PLAYER_COUNT];		//是否已经赋值给钵钵数了  0：没有赋值过   1：已经赋值过了
	Lint		m_record_bobo_score[CHEXUAN_PLAYER_COUNT];		//本场最大可起钵钵数 竞技场可能会变化，非竞技场不会变化

	Lint		m_tui_mang_score[CHEXUAN_PLAYER_COUNT];			//芒果不清空，房间结束时退的芒果
	Lint		m_tui_pi_score[CHEXUAN_PLAYER_COUNT];			//皮不清空，房间结束时退的皮

	//各个阶段延时时间
	Lint		m_def_time_qibobo;						//起钵钵
	Lint		m_def_time_optscore;					//前两轮下注
	Lint		m_def_time_3rd_optscore;				//第三轮下注
	Lint		m_def_time_cuopai;						//搓牌
	Lint		m_def_time_chepai;						//扯牌
	Lint		m_def_time_finish;						//自动下一局时间，desk上的时间要同步改


	////玩法小选项
	Lint		m_baseTimes;							//底分倍数
	Lint		m_diPi;									//底、皮  0：1 / 3     1：5 / 1(5 / 10)     2：5 / 2(5 / 20)		3：2/5
	Lint		m_maxBoBo;								//允许的最大钵数  0:500   1:1000   2:2000
	Lint		m_playCount;							//客户端选择的允许最大人数   2：2人    5:5人    8：8人
	BOOL		m_isDynamicIn;							//是否允许动态加入  true:允许   false:不允许
	BOOL		m_isZouMang;							//是否允许揍芒
	BOOL		m_isXiuMang;							//是否允许休芒
	BOOL		m_isShouMang;							//是否允许手手芒
	BOOL		m_isDaMangFanBei;						//是否允许打芒翻倍
	BOOL 		m_isFanBeiFengDing;						//是否允许是否翻倍封顶  0:封顶  1：不封顶
	Lint		m_isClearPool;							//是否允许每局清空皮、芒果池 0：不清空   1：清空
	Lint		m_isLookOnDarkResult;					//是否观战暗牌解散  0：否   1：观战暗牌结算
	Lint		m_isStartNotInRoom;						//是否游戏开始禁止加入房间  0：不限制  1：游戏开始后禁止加入
	bool		m_isMoFen;								//是否开启默分 false:不开启   true:开启
	bool		m_isFirstAddBigMang;					//是否首家分大于芒果  0：正常   1：首家大于芒果
	bool		m_isAllowDiJiu;							//是否允许地九王  0：不允许地九王  1：允许地九王
	bool		m_isFastMode;							//是否使用急速模式  0：正常模式  1：急速模式
	bool		m_isQiPaiNotShowCards;					//是否弃牌后小结算不显示牌型  0：正常模式    1：弃牌后小结算不显示牌型
	bool		m_isLongXinFeiMaxScore;					//是否开启龙心肺输分最大分数选项  0：不开启   1：开启

	/* 清零结构体字段 */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_baseTimes = 1;
		m_playCount = CHEXUAN_PLAYER_COUNT;
		m_last_get_yu_mang = CHEXUAN_INVALID_POS;
	}
};

/* 扯炫每场会初始化的字段（结构体）*/
struct CheXuanMatchState__cxx_part
{
	CXGameLogic   m_gamelogic;     // 游戏逻辑
	PlayType      m_playtype;	    // 桌子玩法小选项

									/* 清空每场结构体 */
	void clear_match()
	{
		this->~CheXuanMatchState__cxx_part();
		new (this) CheXuanMatchState__cxx_part;
	}
};

/*
*  扯炫每局所需要的所有字段（结构体）
*  继承 ：CheXuanRoundState__c_pard, CheXuanRoundState_cxx_part
*/
struct CheXuanRoundState : CheXuanRoundState__c_part, CheXuanRoundState__cxx_part
{
	void clear_round()
	{
		CheXuanRoundState__c_part::clear_round();
		CheXuanRoundState__cxx_part::clear_round();
	}
};

/*
*  扯炫每场所需要的所有字段（结构体）
*  继承：CheXuanMatchState__c_pard, CheXuanMatchState_cxx_pard
*/
struct CheXuanMatchState : CheXuanMatchState__c_part, CheXuanMatchState__cxx_part
{
	void clear_match()
	{
		CheXuanMatchState__c_part::clear_match();
		CheXuanMatchState__cxx_part::clear_match();
	}
};

/*
*  扯炫桌子状态（结构体）
*  继承：CheXuanRoundState, CheXuanMatchState
*/
struct CheXuanDeskState : CheXuanRoundState, CheXuanMatchState
{
	/* 清零每局数据 */
	void clear_round()
	{
		CheXuanRoundState::clear_round();
	}

	/* 清零每场数据*/
	void clear_match(int player_count)
	{
		CheXuanMatchState::clear_match();
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
			return CHEXUAN_INVALID_POS;
		}

		Lint pos = CHEXUAN_INVALID_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= CHEXUAN_INVALID_POS)
		{
			pos = CHEXUAN_INVALID_POS;
		}
		return pos;
	}

	/* 获取上一个有效位置 */
	Lint GetPrePos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS; // 避免除零崩溃
		}
		Lint t_prePos = pos % m_player_count;
		do
		{
			t_prePos = (t_prePos + m_player_count - 1) % m_player_count;
		} while (m_user_status[t_prePos] != 1 || m_qipai_status[t_prePos] != 0);

		return t_prePos;
	}

	/* 获取上一个位置(有人就行) */
	Lint GetRealPrePos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS; // 避免除零崩溃
		}
		Lint t_prePos = pos % m_player_count;
		do
		{
			t_prePos = (t_prePos + m_player_count - 1) % m_player_count;
		} while (m_user_status[t_prePos] != 1);

		return t_prePos;
	}


	/* 获取下一个有效位置 */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS;
		}

		Lint t_nextPos = pos % m_player_count;
		do
		{
			t_nextPos = (t_nextPos + 1) % m_player_count;
			LLOG_DEBUG("GetNextPos() do...  pos=[%d], status=[%d], qiPai=[%d]", t_nextPos, m_user_status[t_nextPos], m_qipai_status[t_nextPos]);
		} while (m_user_status[t_nextPos] != 1 || m_qipai_status[t_nextPos] != 0);

		return t_nextPos;
	}
	
	/* 获取下一个人的位置 */
	Lint GetRealNextPos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS;
		}

		Lint t_nextPos = pos % m_player_count;
		do
		{
			t_nextPos = (t_nextPos + 1) % m_player_count;
			LLOG_DEBUG("GetRealNextPos() do...  pos=[%d], status=[%d], qiPai=[%d]", t_nextPos, m_user_status[t_nextPos], m_qipai_status[t_nextPos]);
		} while (m_user_status[t_nextPos] != 1);

		return t_nextPos;
	}

	/* 判断该玩家下注是否为拖 */
	Lint CheckIsTuo(Lint pos)
	{
		if (m_player_count == 0)
		{
			return 0; // 避免除零崩溃
		}

		Lint t_tuoPrePos = CHEXUAN_INVALID_POS;
		Lint t_tmpPos = pos % m_player_count;

		for (int i = pos; i < pos + m_player_count; ++i)
		{
			t_tmpPos = (t_tmpPos + m_player_count - 1) % m_player_count;

			if (m_user_status[t_tmpPos] != 1) continue;

			if (t_tmpPos == m_turn_first_speak_pos && m_turn_opt_score[m_turn_count][t_tmpPos] == 0)
			{
				break;
			}

			if (m_turn_opt_score[m_turn_count][t_tmpPos] != 0)
			{
				t_tuoPrePos = t_tmpPos;
				break;
			}
		}

		if (t_tuoPrePos != CHEXUAN_INVALID_POS && m_turn_opt_score[m_turn_count][pos] >= m_turn_opt_score[m_turn_count][t_tuoPrePos] * 2)
		{
			return 1;
		}

		return 0;
	}

	/* 判断位置是否为有效位置 */
	bool is_pos_valid(Lint pos)
	{
		if (NULL == m_desk)
		{
			return false;
		}
		return 0 <= pos && pos < CHEXUAN_INVALID_POS;
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
};

/* 扯炫录像功能(结构体) */
struct CheXuanVideoSupport : CheXuanDeskState
{
	QiPaiVideoLog m_video; //录像

	void VideoSave()
	{
		if (NULL == m_desk)
			return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_deskId = m_desk->GetDeskId();
		m_video.m_playType = m_desk->getPlayType();
		m_video.m_flag = m_desk->m_state;
		m_video.m_maxCircle = m_round_offset;

		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		LLOG_DEBUG("videoSave[%s]", video.m_sql.c_str());
		gWork.SendMsgToDb(video);
	}

	// oper 操作  0--过  1--出牌   位置   手牌个数   手牌 操作的牌个数  操作的牌  得分
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

/* 扯炫游戏具体处理逻辑（结构体）*/
struct CheXuanGameCore : GameHandler, CheXuanVideoSupport
{
	//通知给具体玩家
	virtual void notify_user(LMsg &msg, int pos) {}

	//广播给桌子里所有入座玩家
	virtual void notify_desk(LMsg &msg) {}

	//广播给当前参与玩家
	virtual void notify_desk_playing_user(LMsg &msg) {}

	//广播给当前入座但是没有参与游戏玩家
	virtual void notify_desk_seat_no_playing_user(LMsg &msg) {}

	//广播给未入座的观战玩家
	virtual void notify_lookon(LMsg &msg) {}

	//
	virtual void notify_desk_seating_user(LMsg &msg) {}

	//
	virtual void notify_desk_without_user(LMsg &msg, User* pUser) {}

	/*
	virtual void notify_club_ower_lookon(LMsg& msg) {}
	*/

	/* 广播游戏局数给桌上的所有玩家 */
	void notify_desk_match_state()
	{
		LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;
		notify_desk(msg);
		notify_lookon(msg);
		//notify_club_ower_lookon(msg);
	}

	/* 
	  每局结束根据输赢分数更新能量场玩家能量值
	*/
	void update_vip_player_coins()
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->UpdatePlayerCoins(m_player_score, m_player_count);
		}
	}

	/* 判断是否有人敲了 */
	bool check_has_player_qiao()
	{
		bool t_flag = false;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_qiao_status[i] == 1)
			{
				t_flag = true;
				break;
			}
		}
		return t_flag;
	}

	//判断操作是否完成
	bool has_opt_over(GAME_PLAY_STATE playState)
	{
		switch (playState)
		{
		case GAME_PLAY_BEGIN:
			break;

		//起钵钵阶段
		case GAME_PLAY_QIBOBO:
		{
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_qibobo_status[i] != 1 && m_user_status[i] == 1)
				{
					return false;
				}
			}
			return true;
			break;
		}

		//下注阶段
		case GAME_PLAY_ADD_SCORE:
		{
			//总共下注最大值
			Lint t_totalMaxAddScore = 0;

			//本轮最大下注值
			Lint t_maxAddSocre = 0;

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_total_add_score[i] > t_totalMaxAddScore)
				{
					t_totalMaxAddScore = m_total_add_score[i];
				}
				if (m_turn_opt_score[m_turn_count][i] > t_maxAddSocre)
				{
					t_maxAddSocre = m_turn_opt_score[m_turn_count][i];
				}
			}
			if (t_maxAddSocre < 0 || t_totalMaxAddScore < 0)
			{
				return false;
			}

			int t_playerCount = 0;
			int t_qipaiCount = 0;
			int t_xiuCount = 0;
			int t_qiaoCount = 0;
			bool t_flag = true;

			//每个有效玩家下注类型和下注分数和最大值相比
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1) ++t_playerCount;
				if (m_qipai_status[i] != 0) ++t_qipaiCount;
				else if (m_xiu_status[i] == 1) ++t_xiuCount;
				else if (m_qiao_status[i] != 0) ++t_qiaoCount;

				//最后一个操作玩家之前的玩家全都弃牌，则不用最后一个玩家操作了，判断结束，并且为揍芒
				if (t_playerCount - t_qipaiCount == 1 && i == m_player_count - 1)
				{
					return true;
				}
				//所有玩家都休了
				else if (t_qipaiCount + t_xiuCount == t_playerCount && i == m_player_count - 1)
				{
					return true;
				}
				/*else if (t_qiaoCount + t_xiuCount == t_playerCount && i == m_player_count - 1)
				{
					return true;
				}*/
				

				//只有一个人没有敲，且操作过了
				else if ((t_playerCount - t_qipaiCount - t_qiaoCount) == 1 && i == m_player_count - 1)
				{
					Lint t_noQiaoPos = CHEXUAN_INVALID_POS;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_user_status[i] == 1 && m_qipai_status[i] == 0 && m_qiao_status[i] == 0)
						{
							t_noQiaoPos = i;
							break;
						}
					}

					//if (t_noQiaoPos != CHEXUAN_INVALID_POS && m_turn_opt_score[m_turn_count][t_noQiaoPos] >= t_maxAddSocre &&
					if (t_noQiaoPos != CHEXUAN_INVALID_POS && m_total_add_score[t_noQiaoPos] >= t_totalMaxAddScore &&
						(m_turn_opt_type[m_turn_count][t_noQiaoPos] == ADD_OPT_FIRST_QIAO ||
							m_turn_opt_type[m_turn_count][t_noQiaoPos] == ADD_OPT_GEN ||
							m_turn_opt_type[m_turn_count][t_noQiaoPos] == ADD_OPT_FA_ZHAO)
						)
					{
						//设置标记只有一人操作没有敲，其他人都敲了
						m_oneNotQiao = true;
						return true;
					}
				}

				//本局操作只有敲和休，切休的玩家分数已经根平
				else if (t_qiaoCount + t_xiuCount + t_qipaiCount == t_playerCount && t_qiaoCount > 0 && t_xiuCount > 0 && i == m_player_count - 1)
				{
					m_OnlyQiaoAndXiu = true;
					for (Lint j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] != 1 || m_qiao_status[j] == 1 || m_qipai_status[j] != 0) continue;

						if (m_total_add_score[j] != t_totalMaxAddScore)
						{
							m_OnlyQiaoAndXiu = false;
							return false;
						}
					}
					return true;
				}


				if (m_user_status[i] != 1 || m_qipai_status[i] != 0)
				{
					continue;
				}

				//玩家是否可以三花
				bool t_isCanSanHua = false;
				bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[i], m_hand_cards_count[i]);
				bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[i], m_hand_cards_count[i]);
				t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[i];

				if (
					!(
					(m_turn_opt_type[m_turn_count][i] == ADD_OPT_FIRST_QIAO && m_turn_opt_score[m_turn_count][i] == t_maxAddSocre) ||
						(m_turn_opt_type[m_turn_count][i] == ADD_OPT_GEN && m_turn_opt_score[m_turn_count][i] == t_maxAddSocre) ||
						(m_turn_opt_type[m_turn_count][i] == ADD_OPT_FA_ZHAO && m_turn_opt_score[m_turn_count][i] == t_maxAddSocre) ||
						m_turn_opt_type[m_turn_count][i] == ADD_OPT_QIAO_BO ||
						m_turn_opt_type[m_turn_count][i] == ADD_OPT_SHUAI ||
						m_turn_opt_type[m_turn_count][i] == ADD_OPT_SANHUA ||
						(m_qiao_status[i] != 0 && !t_isCanSanHua)
						)
					)
				{
					t_flag = false;
				}
			}
			return t_flag;
			break;
		}
		//扯牌
		case GAME_PLAY_CHE_PAI:
		{
			int t_playerCount = 0;
			int t_chePaiCount = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0) ++t_playerCount;
				if (m_chepai_status[i] == 1) ++t_chePaiCount;
			}
			if (t_playerCount == t_chePaiCount) return true;
			else return false;
			break;
		}
		//搓牌
		case GAME_PLAY_CUO_PAI:
		{
			int t_playerCount = 0;
			int t_cuoPaiCount = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0) ++t_playerCount;
				if (m_cuopai_status[i] == 1) ++t_cuoPaiCount;
			}
			if (t_playerCount <= t_cuoPaiCount) return true;
			else return false;
			break;
		}

		case GAME_PLAY_END:
			break;
		default:
			break;
		}

		return false;
	}

	/*
	判断是否为休芒
	所有玩家都休，或者至少有一个休，其他全都是弃牌，则为休芒
	*/
	bool is_xiu_mang()
	{
		Lint t_playerCount = 0;
		Lint t_optShuaiCount = 0;
		Lint t_optXiuCount = 0;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_user_status[i] == 1)
			{
				++t_playerCount;
			}

			if (m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU)
			{
				++t_optXiuCount;
			}

			if (m_qipai_status[i] != 0)
			{
				++t_optShuaiCount;
			}
		}
		return t_playerCount == (t_optXiuCount + t_optShuaiCount);
	}

	/*
	判断是否揍芒
	1人除休、弃牌以外的操作，其他人都弃牌
	*/
	bool is_zou_mang()
	{
		Lint t_playerCount = 0;
		Lint t_optShuaiCount = 0;
		Lint t_optXiuCount = 0;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_user_status[i] == 1)
			{
				++t_playerCount;
			}

			if (m_qipai_status[i] != 0)
			{
				++t_optShuaiCount;
			}

			if (m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU)
			{
				++t_optXiuCount;
			}

		}

		if (t_playerCount - t_optShuaiCount == 1 && t_optXiuCount == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/* 查找最大值, 返回最大值 */
	Lint find_max_score(Lint arr[], Lint arrSize, Lint* maxScoreIndex)
	{
		Lint t_index = 0;
		Lint t_max = -1;
		if (arr == NULL || arrSize < 1)
		{
			return -1;
		}

		for (int i = 0; i < m_player_count; ++i)
		{
			if (arr[i] > t_max)
			{
				t_index = i;
				t_max = arr[i];
			}
		}
		*maxScoreIndex = t_index;
		return t_max;
	}

	/* 计算当前玩家可操作的类型以及操作值范围 */
	void calc_opt_score()
	{
		//上家位置
		Lint t_prePos = GetPrePos(m_curPos);

		//上个有人的位置
		Lint t_preRealPos = GetRealPrePos(m_curPos);

		//下家位置
		//Lint t_nextPos = GetNextPos(m_curPos);

		//是否为第一轮下注,  true:第一轮   false:第二三轮
		bool t_isFirstTurn = (m_turn_count == 0);

		//是否为挡家：第一轮下注中首个说话人的位置
		bool t_isDangJia = (m_curPos == m_dang_player_pos && m_turn_count == 0);

		//是否为第一个说话人位置
		bool t_isFirstSpeaker = (m_curPos == m_turn_first_speak_pos);

		//本人是否为本轮第一次操作
		bool t_isTurnFirstOpt = (m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID);

		//挡家是否选择默分中的看牌：挡家只能操作瞧
		bool t_isDangPosMoLookCard = (m_isMoFen && t_isDangJia && m_selectMoFen == 1);

		//当前最大下注分数的玩家位置
		Lint t_currMaxIndex = CHEXUAN_INVALID_POS;

		//当前下注的最大分数
		Lint t_currMaxScore = find_max_score(m_turn_opt_score[m_turn_count], m_player_count, &t_currMaxIndex);

		//当前玩家允许的最小下注分数
		Lint t_minScore = t_currMaxScore <= 0 ? m_scoreBase : t_currMaxScore;		//Lint t_minScore = m_scoreBase > t_currMaxScore ? m_scoreBase : t_currMaxScore;

		//临时位置变量
		Lint t_tmpPos = m_curPos;

		//挡家是否可以瞧（之前为：发招）：挡家第一轮第一次下注
		bool t_isEnoughFirstQiao = m_total_bo_score[m_curPos] >= (m_scoreBase - m_mangScore);
		bool t_isCanFirstQiao = t_isEnoughFirstQiao && t_isDangJia && t_isTurnFirstOpt;
		Lint t_minFirstQiaoScore = t_isCanFirstQiao ? (m_scoreBase - m_mangScore) : 0;

		//玩家是否可以跟（之前为：瞧）：不是每轮下注第一个说话人的第一次操作位置
		bool t_isEnoughGen = m_total_bo_score[m_curPos] > (t_minScore - m_turn_opt_score[m_turn_count][m_curPos]);
		bool t_isCanOptGen = m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID ? false : true;
		t_tmpPos = m_turn_first_speak_pos;
		for (Lint i = m_turn_first_speak_pos; i < m_turn_first_speak_pos + m_player_count && !t_isCanOptGen; ++i)
		{
			t_tmpPos = i % m_player_count;

			if (m_user_status[t_tmpPos] != 1) continue;
			
			if (m_curPos == m_turn_first_speak_pos)
			{
				t_isCanOptGen = false;
				break;
			}
			else if (t_tmpPos == m_curPos)
			{
				break;
			}
			else if (m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_SANHUA &&
				m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_SHUAI &&
				m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_XIU &&
				m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_INVALID)
			{
				t_isCanOptGen = true;
				break;
			}
		}
		bool t_isCanGen = t_isEnoughGen && !(t_isFirstSpeaker && t_isTurnFirstOpt) && t_isCanOptGen;	//bool t_isCanGen = t_isEnoughGen && !(t_isFirstSpeaker && t_isTurnFirstOpt);
		Lint t_minGenScore = t_isCanGen ? t_minScore - m_turn_opt_score[m_turn_count][m_curPos] : 0;

		//玩家是否可以发招（之前为：大）：不是第一轮第一个说话位置的首次操作
		bool t_isEnoughFaZhao = false;
		bool t_isCanFaZhao = false;
		Lint t_minFaZhaoScore = 0;
		//挡家第一轮第一次操作可以大
		if (t_isDangJia && t_isTurnFirstOpt)
		{
			if (m_diPi == 0)
			{
				t_minFaZhaoScore = 10;
			}
			else if (m_diPi == 1)
			{
				t_minFaZhaoScore = 30;
			}
			else if (m_diPi == 2)
			{
				t_minFaZhaoScore = 50;
			}
			else if (m_diPi == 3)
			{
				t_minFaZhaoScore = 10;
			}

			//选择首次发招大于芒果 && 第一次操作发招 && 芒果池大于0
			if (m_isFirstAddBigMang && !m_is_first_opt_fa_zhao && m_mang_pool > 0)
			{
				t_minFaZhaoScore = m_mang_pool;
			}

			t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= t_minFaZhaoScore;
			t_isCanFaZhao = t_isEnoughFaZhao && !t_isDangPosMoLookCard;
			t_minFaZhaoScore = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		}
		//每轮第一个说话玩家操作可以操作大，大的最小值必须为上局平分的2倍
		else if (!t_isFirstTurn && t_isFirstSpeaker && t_isTurnFirstOpt)
		{
			//上局平分
			Lint t_prePingScore = 0;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				//修改大的下分分数（Ren:2019-02-25）
				if (m_total_add_score[i] > t_prePingScore)
				{
				t_prePingScore = m_total_add_score[i];
				}
			}

			t_minFaZhaoScore = 2 * t_prePingScore;
			if (m_diPi == 0 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}
			else if (m_diPi == 1 && t_minFaZhaoScore < 30)
			{
				t_minFaZhaoScore = 30;
			}
			else if (m_diPi == 2 && t_minFaZhaoScore < 50)
			{
				t_minFaZhaoScore = 50;
			}
			else if (m_diPi == 3 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}

			//选择首次发招大于芒果 && 第一次操作发招 && 芒果池大于0
			if (m_isFirstAddBigMang && m_mang_pool > 0)
			{
				if (!m_is_first_opt_fa_zhao)
				{
					t_minFaZhaoScore = m_mang_pool;
				}
				else
				{
					t_minFaZhaoScore = 2 * t_prePingScore;
				}
			}
			/*
			if (m_isFirstAddBigMang && !m_is_first_opt_fa_zhao && m_mang_pool > 0)
			{
				t_minFaZhaoScore = m_mang_pool;
			}
			else if (m_isFirstAddBigMang && m_is_first_opt_fa_zhao)
			{
				t_minFaZhaoScore = 2 * t_prePingScore;
			}
			*/

			//t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= (t_prePingScore * 2);
			t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= t_minFaZhaoScore;
			t_isCanFaZhao = t_isEnoughFaZhao && !(t_isDangJia && t_isTurnFirstOpt);
			t_minFaZhaoScore = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		}
		//其他玩家，大的分数必须至少为上一个正常玩家的2倍（敲的玩家不算）
		else
		{
			//当前最大下注值
			Lint t_tCurMaxScore = 0;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;
				
				if (m_total_add_score[i] >= t_tCurMaxScore)
				{
					t_tCurMaxScore = m_total_add_score[i];
				}
			}

			t_minFaZhaoScore = 2 * t_tCurMaxScore;
			if (m_diPi == 0 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}
			else if (m_diPi == 1 && t_minFaZhaoScore < 30)
			{
				t_minFaZhaoScore = 30;
			}
			else if (m_diPi == 2 && t_minFaZhaoScore < 50)
			{
				t_minFaZhaoScore = 50;
			}
			else if (m_diPi == 3 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}

			//选择首次发招大于芒果 && 第一次操作发招 && 芒果池大于0
			if (m_isFirstAddBigMang && m_mang_pool > 0)
			{
				if (!m_is_first_opt_fa_zhao)
				{
					t_minFaZhaoScore = m_mang_pool;
				}
				else
				{
					t_minFaZhaoScore = 2 * t_tCurMaxScore;
				}
			}
			/*
			if (m_isFirstAddBigMang && !m_is_first_opt_fa_zhao && m_mang_pool > 0)
			{
				t_minFaZhaoScore = m_mang_pool;
			}
			else if (m_isFirstAddBigMang && m_is_first_opt_fa_zhao)
			{
				t_minFaZhaoScore = 2 * t_tCurMaxScore;
			}
			*/

			t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= t_minFaZhaoScore;
			t_isCanFaZhao = t_isEnoughFaZhao && !(t_isDangJia && t_isTurnFirstOpt);
			t_minFaZhaoScore = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		}

		//玩家是否可以休
		bool t_isCanXiu = false;
		bool t_allPrePlayerXiu = true;  //当前玩家之前所有玩家是否都是选择的休/弃牌
		t_tmpPos = m_turn_first_speak_pos;
		for (int i = m_turn_first_speak_pos; i < m_player_count + m_turn_first_speak_pos; ++i)
		{
			t_tmpPos = i % m_player_count;
			if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0) continue;

			//该玩家是已经下过注的，这次又轮到他操作，所以不能休
			if (t_tmpPos == m_curPos && m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_INVALID)
			{
				t_allPrePlayerXiu = false;
				break;
			}

			//判断结束条件
			if (t_tmpPos == m_curPos)
			{
				break;
			}

			if (t_tmpPos == m_turn_first_speak_pos && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID)
			{
			}
			//从挡家开始，在该玩家之前的玩家如果选择的不是休/弃牌，则没有休的权利
			else if (m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_XIU && m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_SHUAI && m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_INVALID)
			{
				t_allPrePlayerXiu = false;
				break;
			}
		}
		t_isCanXiu = t_allPrePlayerXiu && !t_isFirstTurn;

		//玩家是否可以三花
		bool t_isCanSanHua = false;
		bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[m_curPos], m_hand_cards_count[m_curPos]);
		bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[m_curPos], m_hand_cards_count[m_curPos]);
		t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[m_curPos];
		// m_san_hua_status[m_curPos] = t_isSanHua ? 1 : 0;
		//if (t_isCanSanHua && !m_san_hua_status[m_curPos]) m_san_hua_status[m_curPos] = 1;

		//玩家是否可以弃牌（之前为：甩）
		bool t_isCanShuai = !((t_isDangJia && t_isTurnFirstOpt) || t_isCanSanHua);
		Lint t_shuaiScore = 0;
		//if (t_isCanShuai && t_isFirstTurn && m_turn_opt_score[0][m_curPos] == 0)
		if (t_isCanShuai && t_isFirstTurn && m_total_add_score[m_curPos] == 0)
		{
			if (m_diPi == 1) t_shuaiScore = 5;
			else if (m_diPi == 2) t_shuaiScore = 5;
			else if (m_diPi == 3) t_shuaiScore = 2;
			else t_shuaiScore = 1;
		}

		//玩家是否可以敲钵钵
		//bool t_isCanQiaoBo = !(t_isDangJia && t_isTurnFirstOpt) || (t_isDangJia && t_isTurnFirstOpt && !t_isEnoughFirstQiao);
		bool t_isCanQiaoBo = m_qiao_status[m_curPos] == 0 && !(t_isDangPosMoLookCard && t_isDangJia && t_isTurnFirstOpt && t_isFirstTurn);
		Lint t_minQiaoBoScore = t_isCanQiaoBo ? m_total_bo_score[m_curPos] : 0;


		//可操作的选项
		m_allow_opt_type[m_curPos][ADD_OPT_FIRST_QIAO] = t_isCanFirstQiao ? ADD_OPT_FIRST_QIAO : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_GEN] = t_isCanGen ? ADD_OPT_GEN : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_FA_ZHAO] = t_isCanFaZhao ? ADD_OPT_FA_ZHAO : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_QIAO_BO] = t_isCanQiaoBo ? ADD_OPT_QIAO_BO : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_XIU] = t_isCanXiu ? ADD_OPT_XIU : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_SHUAI] = t_isCanShuai ? ADD_OPT_SHUAI : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] = t_isCanSanHua ? ADD_OPT_SANHUA : ADD_OPT_INVALID;

		//可操作选项的最小操作值
		m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO] = t_isCanFirstQiao ? t_minFirstQiaoScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_GEN] = t_isCanGen ? t_minGenScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_FA_ZHAO] = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_QIAO_BO] = t_isCanQiaoBo ? t_minQiaoBoScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_XIU] = 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_SHUAI] = t_isCanShuai ? t_shuaiScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_SANHUA] = (int)t_isSanHuaDouble;

		//可操作选项的最大操作值
		m_allow_opt_max_score[m_curPos][ADD_OPT_FIRST_QIAO] = t_isCanFirstQiao ? t_minFirstQiaoScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_GEN] = t_isCanGen ? t_minGenScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_FA_ZHAO] = t_isCanFaZhao ? m_total_bo_score[m_curPos] : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_QIAO_BO] = t_isCanQiaoBo ? t_minQiaoBoScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_XIU] = 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_SHUAI] = t_isCanShuai ? t_shuaiScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_SANHUA] = (int)t_isSanHuaDouble;


		LLOG_ERROR("CheXuanGameHandler::calc_opt_score() Run..., deskId = [%d], curPos = [%d], allowOptType=[%d, %d, %d, %d, %d, %d, %d], allowOptMinScore=[%d, %d, %d, %d, %d, %d, %d], allowOptMaxScore=[%d, %d, %d, %d, %d, %d, %d]",
			m_desk ? m_desk->GetDeskId() : 0, m_curPos,
			m_allow_opt_type[m_curPos][0], m_allow_opt_type[m_curPos][1], m_allow_opt_type[m_curPos][2], m_allow_opt_type[m_curPos][3], m_allow_opt_type[m_curPos][4], m_allow_opt_type[m_curPos][5], m_allow_opt_type[m_curPos][6],
			m_allow_opt_min_score[m_curPos][0], m_allow_opt_min_score[m_curPos][1], m_allow_opt_min_score[m_curPos][2], m_allow_opt_min_score[m_curPos][3], m_allow_opt_min_score[m_curPos][4], m_allow_opt_min_score[m_curPos][5], m_allow_opt_min_score[m_curPos][6],
			m_allow_opt_max_score[m_curPos][0], m_allow_opt_max_score[m_curPos][1], m_allow_opt_max_score[m_curPos][2], m_allow_opt_max_score[m_curPos][3], m_allow_opt_max_score[m_curPos][4], m_allow_opt_max_score[m_curPos][5], m_allow_opt_max_score[m_curPos][6]);
	}

	/* 计算实际参与游戏的玩家数量 */
	Lint calc_real_player_count()
	{
		Lint t_realPlayerCount = 0;
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1)
			{
				++t_realPlayerCount;
			}
		}

		return t_realPlayerCount;
	}

	/* 离挡家最近的且牌最小的玩家位置 */
	Lint findMinCardsAndNearDangPos(Lint* minCount /*out*/, Lint minPos[] /*out*/)
	{
		HandCardsTypeInfo t_minTypeInfo = m_handCardsTypeInfo[0];

		//逆时针离挡家最近最小玩家的位置
		Lint t_minAndNearDangPos = m_frist_send_card;

		//初始化储最小牌玩家的位置
		for (Lint i = 0; i < m_player_count; ++i)
		{
			minPos[i] = CHEXUAN_INVALID_POS;
		}

		//查找最小牌型
		for (int i = 1; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType <= t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType <= t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType)
				)
			{
				t_minTypeInfo = m_handCardsTypeInfo[i];
			}
		}

		//根据查找到的最小牌型，查找出所有最小牌型的玩家位置
		for (int i = 0; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType > t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType > t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType == t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType == t_minTypeInfo.m_secondType)
				)
			{
				minPos[(*minCount)++] = m_handCardsTypeInfo[i].m_userIndex;
			}
		}

		//查找选出的最小牌型的玩家位置逆时针离挡家最近的位置
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (int i = t_minAndNearDangPos; i < m_player_count + t_minAndNearDangPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 0) continue;

			for (int j = 0; j < *minCount; ++j)
			{
				if (t_pos == minPos[j])
				{
					t_minAndNearDangPos = minPos[j];
					t_flag = true;
					break;
				}
			}
			if (t_flag) break;
		}

		return t_minAndNearDangPos;
	}

	/* 离挡家最远的且牌最小的玩家位置 */
	Lint findMinCardsAndFarDangPos(Lint* minCount /*out*/, Lint minPos[] /*out*/)
	{
		HandCardsTypeInfo t_minTypeInfo = m_handCardsTypeInfo[0];

		//逆时针离挡家最近最小玩家的位置
		Lint t_minAndFarDangPos = m_frist_send_card;

		//初始化储最小牌玩家的位置
		for (Lint i = 0; i < m_player_count; ++i)
		{
			minPos[i] = CHEXUAN_INVALID_POS;
		}

		//查找最小牌型
		for (int i = 1; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType <= t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType <= t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType)
				)
			{
				t_minTypeInfo = m_handCardsTypeInfo[i];
			}
		}

		//根据查找到的最小牌型，查找出所有最小牌型的玩家位置
		for (int i = 0; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType > t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType > t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType == t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType == t_minTypeInfo.m_secondType)
				)
			{
				minPos[(*minCount)++] = m_handCardsTypeInfo[i].m_userIndex;
			}
		}

		//查找选出的最小牌型的玩家位置逆时针离挡家最远的位置
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (int i = t_minAndFarDangPos; i < m_player_count + t_minAndFarDangPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 1) continue;

			for (int j = 0; j < *minCount; ++j)
			{
				if (t_pos == minPos[j])
				{
					t_minAndFarDangPos = minPos[j];
				}
			}
		}

		return t_minAndFarDangPos;
	}

	/* 离挡家最远的且牌最小的玩家位置（如果有弃牌的，弃牌玩家是最小的）*/
	Lint findMinCardsAndFarDangPos_QiPai()
	{
		//存储最小牌玩家的位置
		Lint t_minPos[CHEXUAN_PLAYER_COUNT];

		//最小牌玩家数量
		Lint t_minCount = 0;

		//玩家最小牌的集合
		HandCardsTypeInfo t_minTypeInfo = m_handCardsTypeInfo[0];

		//逆时针离挡家最近最小玩家的位置
		Lint t_minAndFarDangPos = m_frist_send_card;

		//初始化储最小牌玩家的位置
		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_minPos[i] = CHEXUAN_INVALID_POS;
		}

		//有弃牌玩家（或三花），则弃牌玩家最小，查找离挡家最远的玩家位置
		if (m_shuai_count > 0)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] != 0)
				{
					t_minPos[t_minCount++] = i;
				}
			}
		}
		//没有弃牌（或三花）玩家
		else
		{
			//查找最小牌型
			for (int i = 1; i < m_real_player_count; ++i)
			{
				if (
					(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType <= t_minTypeInfo.m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType <= t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType)
					)
				{
					t_minTypeInfo = m_handCardsTypeInfo[i];
				}
			}

			//根据查找到的最小牌型，查找出所有最小牌型的玩家位置
			for (int i = 0; i < m_real_player_count; ++i)
			{
				if (
					(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType > t_minTypeInfo.m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType > t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType == t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType == t_minTypeInfo.m_secondType)
					)
				{
					t_minPos[t_minCount++] = m_handCardsTypeInfo[i].m_userIndex;
				}
			}
		}
		
		//查找选出的最小牌型的玩家位置逆时针离挡家最远的位置
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (int i = t_minAndFarDangPos; i < m_player_count + t_minAndFarDangPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 1) continue;

			for (int j = 0; j < t_minCount; ++j)
			{
				if (t_pos == t_minPos[j])
				{
					t_minAndFarDangPos = t_minPos[j];
				}
			}
		}

		return t_minAndFarDangPos;
	}

	/* 离挡家最远的且输分最多的玩家位置 */
	Lint findMaxLoseAndFarDangPos(const Lint playerScore[] /*out*/)
	{
		Lint t_maxLoseAndFarPos = m_dang_player_pos;
		Lint t_maxLoseScore = 0;
		//查找最小分数
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;
			if (playerScore[i] < t_maxLoseScore)
			{
				t_maxLoseScore = playerScore[i];
			}
		}

		//查找选出的最小牌型的玩家位置逆时针离挡家最远的位置
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (Lint i = t_maxLoseAndFarPos; i < m_player_count + t_maxLoseAndFarPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 1) continue;

			if (playerScore[i] == t_maxLoseScore)
			{
				t_maxLoseAndFarPos = i;
			}
		}

		return t_maxLoseAndFarPos;
	}

	/* 离挡家最远的，且输分最多（包括起身离座玩家）*/
	Lint findMaxLoseAndFarDangPos_QiShen(const Lint playerScore[] /*out*/)
	{
		Lint t_maxLoseAndFarPos = m_dang_player_pos;
		Lint t_maxLoseScore = 0;
		//查找最小分数
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;
			if (playerScore[i] < t_maxLoseScore)
			{
				t_maxLoseScore = playerScore[i];
			}
		}

		//查找选出的最小牌型的玩家位置逆时针离挡家最远的位置
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (Lint i = t_maxLoseAndFarPos; i < m_player_count + t_maxLoseAndFarPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] == 0) continue;

			if (playerScore[t_pos] == t_maxLoseScore)
			{
				t_maxLoseAndFarPos = t_pos;
			}
		}

		return t_maxLoseAndFarPos;
	}

	/* 计算可以起钵钵的范围 */
	void calc_start_bobo_limit(int* minSelectBoScore, int* maxSelectBoScore, Lint pos = CHEXUAN_INVALID_POS)
	{
		//底皮：2:5/20 ,首钵不低于1000，之后每钵不低于500
		if (m_diPi == 2)
		{
			*minSelectBoScore = 500;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 1000;
			}
		}
		//底皮：1:5/10，首钵不低于500，之后每钵不低于300
		else if (m_diPi == 1)
		{
			*minSelectBoScore = 300;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 500;
			}
		}
		//底皮：2/5，首钵不低于200， 之后每钵不低于100
		else if (m_diPi == 3)
		{
			*minSelectBoScore = 100;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 200;
			}
		}
		//底皮：0：1/3，首钵不低于100，之后每钵不低于50
		else
		{
			*minSelectBoScore = 50;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 100;
			}
		}

		if (pos == CHEXUAN_INVALID_POS || !(m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1))
		{
			*maxSelectBoScore = m_maxBoBo;
		}
		else
		{
			*maxSelectBoScore = m_rest_can_start_bo_score[pos];
		}

	}

	/* 判断是否因钵钵分数不够必须起身离座 */
	bool must_stand_up()
	{
		//需要下芒果分数
		Lint t_needAddMangScore = 0;
		memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			//计算需要多起的钵钵分（芒果、芒注、第一次下注）
			if (GetNextPos(m_dang_player_pos) == i)
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
			}
			else
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
			}

			//设置玩家是否需要手手芒的标志
			m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

			t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];
			
			/*
			//能量场
			if (m_desk && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
			{
				//能量值不足的玩家
				if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
				{
					m_desk->m_standUpPlayerSeatDown[i] = 2;
					m_user_status[i] = 2;
				}
				//上局能量值不足，本局添加能量值后没有点击继续参与游戏按钮，仍然不可以参与游戏
				else if (m_desk->m_standUpPlayerSeatDown[i] == 2  || m_desk->m_standUpPlayerSeatDown[i] == 3)
				{
					m_user_status[i] = 2;
				}
				//上局能量值不足，本局添加能量值后并且点击继续参与游戏按钮，参与游戏
				else if(m_desk->m_standUpPlayerSeatDown[i] == 1)
				{
					m_user_status[i] = 1;
					m_desk->m_standUpPlayerSeatDown[i] = 0;
				}

				LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], totalBoScore=[%d], deskStandUp=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore + m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i]);
			}
			//普通场
			else
			{
				//玩家手里剩余的钵钵数 + 可起最大钵钵数 < 需要下芒果的数量
				if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
				{
					m_user_status[i] = 2;
					LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., Player has no enought bo score, So stand up, deskId = [%d], pos = [%d], needAddMang=[%d], totalBoScore=[%d]",
						m_desk ? m_desk->GetDeskId() : 0, i, t_needAddMangScore + m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i]);
				}
			}
			*/

			
			//能量值不足的玩家
			if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
			{
				m_desk->m_standUpPlayerSeatDown[i] = 2;
				m_user_status[i] = 2;
			}
			//上局能量值不足，本局添加能量值后没有点击继续参与游戏按钮，仍然不可以参与游戏
			else if (m_desk->m_standUpPlayerSeatDown[i] == 2 || m_desk->m_standUpPlayerSeatDown[i] == 3)
			{
				m_user_status[i] = 2;
			}
			//上局能量值不足，本局添加能量值后并且点击继续参与游戏按钮，参与游戏
			else if (m_desk->m_standUpPlayerSeatDown[i] == 1)
			{
				m_user_status[i] = 1;
				m_desk->m_standUpPlayerSeatDown[i] = 0;
			}

			LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], mangSocre=[%d], totalBoScore=[%d], deskStandUp=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore, m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i]);

		}

		Lint t_playerCount = 0;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1) ++t_playerCount;
		}
		if (t_playerCount <= 1)
		{
			//人数不够的解散需要从新计算这局钵钵数量
			memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

			LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., Has one player real play,so game finish..., deskId = [%d], realPlayerCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, t_playerCount);
			set_play_status(GAME_PLAY_END);
			finish_round(2);
			return true;
		}
		return false;
	}

	/* 检查玩家能量值低于预警 */
	bool cheack_player_coins_warn(Lint pos)
	{
		if (!m_desk || m_desk->m_clubInfo.m_clubId == 0 || !m_desk->m_isCoinsGame)
		{
			return false;
		}
		if (m_rest_can_start_bo_score[pos] + m_total_bo_score[pos] <= m_desk->m_warnScore && m_is_coins_low[pos] != 1 && m_user_status[pos] != 0)
		{
			//设置玩家能量值过低标记
			m_is_coins_low[pos] = 1;

			//激活该玩家能量值过低的定时器
			m_is_coins_low_time[pos].Now();

			//通知玩家能量值过低
			LMsgS2CWarnPointLow warnPoint;
			warnPoint.m_msgType = 0;
			if (m_desk->m_user[pos])
			{
				warnPoint.m_userId = m_desk->m_user[pos]->GetUserDataId();
				warnPoint.m_nike = m_desk->m_user[pos]->m_userData.m_nike;
				warnPoint.m_point = m_rest_can_start_bo_score[pos] + m_total_bo_score[pos];
				warnPoint.m_warnPoint = m_desk->m_warnScore;
				m_desk->m_user[pos]->Send(warnPoint);
			}
		}
		else if (m_rest_can_start_bo_score[pos] + m_total_bo_score[pos] > m_desk->m_warnScore && m_user_status[pos] != 0)
		{
			//设置玩家能量值过低标记
			m_is_coins_low[pos] = 0;
		}
	}

	/********************************************************************************************
	* 函数名：		start_bobo()
	* 描述：		扯炫起钵钵
	* 返回：		(void)
	*********************************************************************************************/
	void start_bobo()
	{
		//设置当前状态为起钵钵状态
		set_play_status(GAME_PLAY_QIBOBO);

		//每局下芒果前，将是否需要下手手芒的标志清0
		memset(m_is_need_add_shouMang, 0, sizeof(m_is_need_add_shouMang));

		bool t_nextStatus = true;
		Lint t_playerMaxSelectBoScore = 0;
		Lint t_playerMinSelectBoScore = 0;
		Lint t_needAddMang = 0;

		memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}

			//计算需要多起的钵钵分（芒果、芒注、第一次下注）
			if (m_dang_player_pos == i)
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
			}
			else
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
			}

			//TODO:确定本次起钵钵的范围
			Lint t_minSelectBoScore = 50;
			Lint t_maxSelectBoScore = 500;

			calc_start_bobo_limit(&t_minSelectBoScore, &t_maxSelectBoScore, i);

			//第一局每个玩家都需要起钵钵
			if (0 == m_round_limit)
			{
				m_is_need_add_bo[i] = 1;
			}
			//不是第一局的，要判断是否需要起钵钵，钵钵为空，钵钵不足以下芒果两种情况需要起钵钵
			else
			{
				//本轮需要下的芒果数量
				Lint t_needAddMangScore = 0;

				//设置玩家是否需要手手芒的标志
				m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

				//t_needAddMangScore = m_total_bo_score[i] - m_is_need_add_shouMang[i] * m_shouMangCount - m_is_need_add_xiuMang[i] * m_xiuMangCount - m_is_need_add_zouMang[i] * m_zouMangCount - m_mangScore;
				t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];

				//如果玩家钵钵为空 || 钵钵数量不够需要下的芒果数量了，则该玩家需要起钵钵
				if (m_total_bo_score[i] <= 0 || m_total_bo_score[i] < t_needAddMangScore)
				{
					m_is_need_add_bo[i] = 1;
				}
			}

			t_playerMaxSelectBoScore = t_maxSelectBoScore;
			t_playerMinSelectBoScore = t_minSelectBoScore;

			t_needAddMang = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];
			t_playerMaxSelectBoScore = t_maxSelectBoScore;
			t_playerMinSelectBoScore = t_minSelectBoScore;

			if (m_rest_can_start_bo_score[i] < t_playerMaxSelectBoScore)
			{
				t_playerMaxSelectBoScore = m_rest_can_start_bo_score[i];
			}
			if (t_playerMinSelectBoScore + m_total_bo_score[i] < t_needAddMang)
			{
				t_playerMinSelectBoScore = t_needAddMang - m_total_bo_score[i];
			}

			if (m_rest_can_start_bo_score[i] < t_playerMinSelectBoScore)
			{
				t_playerMinSelectBoScore = m_rest_can_start_bo_score[i];
				t_playerMaxSelectBoScore = m_rest_can_start_bo_score[i];
			}

			//记录每个玩家起钵钵的最大最小值
			m_min_qi_bo_score[i] = t_playerMinSelectBoScore;
			m_max_qi_bo_score[i] = t_playerMaxSelectBoScore;

			t_nextStatus = false;

			if (m_rest_can_start_bo_score[i] <= 0)
			{
				m_qibobo_status[i] = 0;
				m_min_qi_bo_score[i] = 0;
				m_max_qi_bo_score[i] = 0;
			}

			//通知玩家起钵钵
			CheXuanS2CNotifyPlayerQiBoBo qiBoBo;
			qiBoBo.m_pos = i;
			qiBoBo.m_restTime = m_def_time_qibobo;
			qiBoBo.m_dangPos = GetNextPos(m_frist_send_card);
			qiBoBo.m_piPool = m_pi_pool;
			qiBoBo.m_mangPool = m_mang_pool;
			qiBoBo.m_minSelectBoScore = m_min_qi_bo_score[i];
			qiBoBo.m_maxSelectBoScore = m_max_qi_bo_score[i];
			memcpy(qiBoBo.m_userStatus, m_user_status, sizeof(qiBoBo.m_userStatus));
			memcpy(qiBoBo.m_userMustQiBoBo, m_is_need_add_bo, sizeof(qiBoBo.m_userMustQiBoBo));
			memcpy(qiBoBo.m_playerStandUp, m_player_stand_up, sizeof(qiBoBo.m_playerStandUp));

			notify_desk(qiBoBo);
			//广播观战玩家：起钵钵
			notify_lookon(qiBoBo);

			//开始计时
			m_play_status_time.Now();
		}

		//如果所有人都不需要起钵钵则直接进入下芒果状态
		if (t_nextStatus)
		{
			set_play_status(GAME_PLAY_ADD_MANGGUO);
			//下芒果
			add_mang_guo();
		}
	}

	/* 下芒果 */
	void add_mang_guo()
	{
		//每局下芒果之前记录这局的钵钵数，用于计算
		memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

		//需要下的芒果总数 = 手手芒 + 休芒 + 揍芒
		Lint t_addMangCount = 0;
		//需要下的手手芒
		Lint t_addShouMangCount = 0;
		//需要下的休芒
		Lint t_addXiuMangCount = 0;
		//需要下的揍芒
		Lint t_addZouMangCount = 0;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			t_addShouMangCount = m_isShouMang ? (m_is_need_add_shouMang[i] * m_shouMangCount) : 0;
			t_addXiuMangCount = m_isXiuMang ? (m_is_need_add_xiuMang[i] * m_xiuMangCount) : 0;
			t_addZouMangCount = m_isZouMang ? (m_is_need_add_zouMang[i] * m_zouMangCount) : 0;
			t_addMangCount = t_addShouMangCount + t_addXiuMangCount + t_addZouMangCount;


			m_first_shou_mang_time = t_addShouMangCount ? t_addShouMangCount : m_first_shou_mang_time;
			m_first_xiu_mang_time = t_addXiuMangCount ? t_addXiuMangCount : m_first_xiu_mang_time;
			m_first_zou_mang_time = t_addZouMangCount ? t_addZouMangCount : m_first_zou_mang_time;

			//钵钵里减去玩家下的芒果
			m_total_bo_score[i] -= t_addMangCount;

			//芒果池中加上玩家下的芒果
			m_mang_pool += t_addMangCount;

			//记录芒果输赢分数，下芒果先当做输的芒果
			m_player_mang_score[i] -= t_addMangCount;

			//将玩家下芒果的结果广播给所有玩家
			CheXuanS2CNotifyPlayerAddMangGuoBC addMangGuo;
			addMangGuo.m_pos = i;
			//addMangGuo.m_restTime = m_isMoFen ? ((Lint)m_def_time_optscore + 2 + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0)) : 0;
			addMangGuo.m_totalBoScore = m_total_bo_score[i];
			addMangGuo.m_totalAddMangScore = t_addMangCount;
			addMangGuo.m_addShouShouMangCount = t_addShouMangCount;
			addMangGuo.m_addXiuMangCount = t_addXiuMangCount;
			addMangGuo.m_addZouMangCount = t_addZouMangCount;
			addMangGuo.m_mangPoolCount = m_mang_pool;
			addMangGuo.m_piPoolCount = m_pi_pool;
			addMangGuo.m_mangScore = m_mangScore;
			addMangGuo.m_isZouMang = m_zouMangCount == 0 ? 0 : 1;
			addMangGuo.m_isXiuMang = m_xiuMangCount == 0 ? 0 : 1;

			notify_desk(addMangGuo);

			//广播观战玩家：起钵钵
			notify_lookon(addMangGuo);
			//广播俱乐部会长明牌观战：起钵钵
			//notify_club_ower_lookon(addMangGuo);

			//记录玩家本局下的所有芒果数量
			m_total_add_mang_score[i] = t_addMangCount;
		}

		//下完芒果清空本局所有芒果的标志
		memset(m_is_need_add_shouMang, 0, sizeof(m_is_need_add_shouMang));
		memset(m_is_need_add_xiuMang, 0, sizeof(m_is_need_add_xiuMang));
		memset(m_is_need_add_zouMang, 0, sizeof(m_is_need_add_zouMang));

		//下盲注
		set_play_status(GAME_PLAY_ADD_MANGSCORE);
		add_mang_score();
	}

	/* 下盲注 */
	void add_mang_score()
	{
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			m_total_bo_score[i] -= m_mangScore;
			m_total_add_score[i] += m_mangScore;
			m_player_add_score[i] += m_mangScore;

			//检查玩家能量值是否低于预警
			cheack_player_coins_warn(i);
		}

		//发牌
		send_card();
	}

	/********************************************************************************************
	* 函数名：		send_card()
	* 描述：		发牌，第1轮发牌两张暗牌， 第2轮发牌一张明牌，第3轮发牌一张暗牌
	* 参数：
	*  @turnCount 第几轮发牌
	* 返回：		（bool）
	*********************************************************************************************/
	void send_card()
	{
		set_play_status(GAME_PLAY_SEND_CARD);
		Lint t_pos = 0;

		memset(m_turn_round_opt, 0, sizeof(m_turn_round_opt));
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1 || m_qipai_status[i] != 0 || m_qiao_status[i] != 0)
			{
				continue;
			}
			m_turn_round_opt[i] = 1;
		}

		//第1轮发牌：发2张暗牌
		if (0 == m_turn_count)
		{
			for (int cardIndex = 0; cardIndex < 2; ++cardIndex)
			{
				for (int i = m_frist_send_card; i < m_player_count + m_frist_send_card; ++i)
				{
					//计算玩家实际位置
					t_pos = i % m_player_count;

					//如果该位置没人 或 改玩家已经弃牌，则不给该玩家发牌
					if (m_user_status[t_pos] != 1 || m_qipai_status[t_pos] != 0) continue;

					//该位置手牌数量增加
					++m_hand_cards_count[t_pos];

					for (int j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] != 1) continue;
						CheXuanS2CSendCard sendCard;
						sendCard.m_pos = t_pos;
						sendCard.m_turnCount = m_turn_count;
						sendCard.m_handCardCount = m_hand_cards_count[t_pos];
						if (j == t_pos)
						{
							memcpy(sendCard.m_handCards, m_hand_cards[t_pos], sizeof(Lint) * m_hand_cards_count[t_pos]);
							LLOG_ERROR("CheXuanGameHandler::send_card() Run(msg)..., deskId=[%d], turnCount=[%d], handCard%d=[%x,%x,%x,%x], handCardsCount=[%d]",
								m_desk ? m_desk->GetDeskId() : 0, sendCard.m_turnCount, sendCard.m_pos,
								sendCard.m_handCards[0], sendCard.m_handCards[1], sendCard.m_handCards[2], sendCard.m_handCards[3], sendCard.m_handCardCount);
						}
						notify_user(sendCard, j);
					}
					//广播给普通观战玩家
					CheXuanS2CSendCard sendCard_look;
					sendCard_look.m_pos = t_pos;
					sendCard_look.m_turnCount = m_turn_count;
					sendCard_look.m_handCardCount = m_hand_cards_count[t_pos];
					notify_lookon(sendCard_look);
					notify_desk_seat_no_playing_user(sendCard_look);
				}
			}

			//更改牌局状态为：下注
			set_play_status(GAME_PLAY_ADD_SCORE);

			//TODO:通知第一个说话的人开始发招
			m_curPos = m_turn_first_speak_pos;

			//如果选择默分
			if (m_isMoFen)
			{
				//TODO：开始计时，等待挡家操作默分选择

				//开始记录倒计时
				m_play_status_time.Now();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = (Lint)m_def_time_optscore;// +(m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
				notifyOpt.m_isMoFen = 1;
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);

			}
			//不需要默分
			else
			{
				bool t_flag = false;
				//如果该玩家已经敲了...查找下一个没有敲的玩家 或 三花牌型玩家 位置
				if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
				{
					Lint t_curPos = m_curPos;
					m_curPos = CHEXUAN_INVALID_POS;
					int t_tmpPos = CHEXUAN_INVALID_POS;

					//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
					for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
					{
						t_tmpPos = i % m_player_count;
						if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
						{
							continue;
						}

						//先找最大下注值(Ren：2019-02-25)
						Lint t_maxAddSocre = 0;
						for (int i = 0; i < m_player_count; ++i)
						{
							if (m_total_add_score[i] > t_maxAddSocre)
							{
								t_maxAddSocre = m_total_add_score[i];
							}
						}

						//判断没敲 或者是 首次出现单三花牌型，要给该玩家发送操作提示消息
						bool t_isCanSanHua = false;
						bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
						bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
						t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
						//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
						//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
						if (
							(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
							(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
							(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
							(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
							t_isCanSanHua) //(Ren:2019-03-15)
						{
							m_turn_round_opt[t_tmpPos] = 1;
							m_curPos = t_tmpPos;
							t_flag = true;
							break;
						}
						m_curPos = CHEXUAN_INVALID_POS;
					}
				}

				//找到下一个没有敲/三花牌型的玩家位置，给这个玩家推送消息
				if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
				{
					//计算该玩家允许的操作
					calc_opt_score();

					CheXuanS2CNotifyPlayerOpt notifyOpt;
					notifyOpt.m_pos = m_curPos;
					notifyOpt.m_restTime = (Lint)m_def_time_optscore;// + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
					notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
					for (int i = 0; i < 10; ++i)
					{
						notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
						notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
						notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
					}
					notify_desk(notifyOpt);
					notify_lookon(notifyOpt);
					//notify_club_ower_lookon(notifyOpt);

					//开始记录倒计时
					m_play_status_time.Now();

					return;
				}
				//都敲了，或者只有一个人操作不是敲
				else
				{
					++m_turn_count;
					send_card();
					return;
				}
			}
		}
		//第2轮发牌，发1张明牌
		else if (1 == m_turn_count)
		{
			for (int i = m_frist_send_card; i < m_player_count + m_frist_send_card; ++i)
			{
				t_pos = i % m_player_count;

				if (m_user_status[t_pos] != 1 || m_qipai_status[t_pos] != 0) continue;

				++m_hand_cards_count[t_pos];

				for (int j = 0; j < m_player_count; ++j)
				{
					if (m_user_status[j] != 1) continue;

					CheXuanS2CSendCard sendCard;
					sendCard.m_pos = t_pos;
					sendCard.m_turnCount = m_turn_count;
					sendCard.m_currSendCard = m_hand_cards[t_pos][m_hand_cards_count[t_pos] - 1];
					sendCard.m_handCardCount = m_hand_cards_count[t_pos];

					memset(sendCard.m_handCards, 0, sizeof(sendCard.m_handCards));
					if (t_pos == j)
					{
						memcpy(sendCard.m_handCards, m_hand_cards[t_pos], sizeof(Lint) * m_hand_cards_count[t_pos]);
						LLOG_ERROR("CheXuanGameHandler::send_card() Run(msg)..., deskId=[%d], turnCount=[%d], handCard%d=[%x,%x,%x,%x], handCardsCount=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, sendCard.m_turnCount, sendCard.m_pos,
							sendCard.m_handCards[0], sendCard.m_handCards[1], sendCard.m_handCards[2], sendCard.m_handCards[3], sendCard.m_handCardCount);
					}
					else
					{
						sendCard.m_handCards[2] = m_hand_cards[t_pos][2];
					}
					notify_user(sendCard, j);
				}

				//广播给普通观战玩家
				CheXuanS2CSendCard sendCard_look;
				sendCard_look.m_pos = t_pos;
				sendCard_look.m_turnCount = m_turn_count;
				sendCard_look.m_handCardCount = m_hand_cards_count[t_pos];
				//暗牌观战
				if (m_isLookOnDarkResult != 1)
				{
					sendCard_look.m_handCards[2] = m_hand_cards[t_pos][2];
				}
				notify_lookon(sendCard_look);
				notify_desk_seat_no_playing_user(sendCard_look);
				//notify_desk_seating_user(sendCard_look);
			}

			//更改牌局状态为：下注
			set_play_status(GAME_PLAY_ADD_SCORE);

			//找出第三张牌最大的玩家位置
			Lint t_thredCards[CHEXUAN_PLAYER_COUNT];
			memset(t_thredCards, 0, sizeof(Lint)*CHEXUAN_PLAYER_COUNT);
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				t_thredCards[i] = m_hand_cards[i][2];
			}
			m_turn_first_speak_pos = m_gamelogic.getMaxCardPos(t_thredCards, m_player_count, m_player_count, m_dang_player_pos);
			m_curPos = m_turn_first_speak_pos;


			bool t_flag = false;
			//如果该玩家已经敲了...查找下一个没有敲的玩家 或 三花牌型玩家 位置
			if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
			{
				Lint t_curPos = m_curPos;
				m_curPos = CHEXUAN_INVALID_POS;
				int t_tmpPos = CHEXUAN_INVALID_POS;
				//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
				for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
				{
					t_tmpPos = i % m_player_count;
					if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
					{
						continue;
					}

					//先找最大下注值(Ren：2019-02-25)
					Lint t_maxAddSocre = 0;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_total_add_score[i] > t_maxAddSocre)
						{
							t_maxAddSocre = m_total_add_score[i];
						}
					}

					//判断没敲 或者是 首次出现单三花牌型，要给该玩家发送操作提示消息
					bool t_isCanSanHua = false;
					bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
					//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
					//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
					if (
						(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || 
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
						t_isCanSanHua) //(Ren:2019-03-15)
					{
						m_turn_round_opt[t_tmpPos] = 1;
						m_curPos = t_tmpPos;
						t_flag = true;
						break;
					}
					m_curPos = CHEXUAN_INVALID_POS;
				}
			}
			//找到下一个没有敲/三花牌型的玩家位置，给这个玩家推送消息
			if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
			{
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = m_def_time_optscore;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				for (int i = 0; i < 10; ++i)
				{
					notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
					notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
					notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
				}
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);
				//notify_club_ower_lookon(notifyOpt);

				//开始记录倒计时
				m_play_status_time.Now();
				return;
			}
			//都敲了
			else
			{
				++m_turn_count;
				send_card();
				return;
			}
		}
		//第3轮发牌，发1张暗牌，添加搓牌阶段
		else
		{
			for (int i = m_frist_send_card; i < m_player_count + m_frist_send_card; ++i)
			{
				t_pos = i % m_player_count;

				if (m_user_status[t_pos] != 1 || m_qipai_status[t_pos] != 0) continue;

				++m_hand_cards_count[t_pos];

				for (int j = 0; j < m_player_count; ++j)
				{
					if (m_user_status[j] != 1) continue;

					CheXuanS2CSendCard sendCard;
					sendCard.m_pos = t_pos;
					sendCard.m_turnCount = m_turn_count;
					sendCard.m_handCardCount = m_hand_cards_count[t_pos];

					memset(sendCard.m_handCards, 0, sizeof(sendCard.m_handCards));
					if (t_pos == j)
					{
						memcpy(sendCard.m_handCards, m_hand_cards[t_pos], sizeof(Lint) * m_hand_cards_count[t_pos]);
						LLOG_ERROR("CheXuanGameHandler::send_card() Run(msg)..., deskId=[%d], turnCount=[%d], handCard%d=[%x,%x,%x,%x], handCardsCount=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, sendCard.m_turnCount, sendCard.m_pos,
							sendCard.m_handCards[0], sendCard.m_handCards[1], sendCard.m_handCards[2], sendCard.m_handCards[3], sendCard.m_handCardCount);
					}
					else
					{
						sendCard.m_handCards[2] = m_hand_cards[t_pos][2];
					}
					notify_user(sendCard, j);
				}

				//广播给普通观战玩家
				CheXuanS2CSendCard sendCard_look;
				sendCard_look.m_pos = t_pos;
				sendCard_look.m_turnCount = m_turn_count;
				sendCard_look.m_handCardCount = m_hand_cards_count[t_pos];
				//暗牌结算观战
				if (m_isLookOnDarkResult != 1)
				{
					sendCard_look.m_handCards[2] = m_hand_cards[t_pos][2];
				}
				notify_lookon(sendCard_look);
				notify_desk_seat_no_playing_user(sendCard_look);
				//notify_desk_seating_user(sendCard_look);
			}

			//更改牌局状态为搓牌状态
			set_play_status(GAME_PLAY_CUO_PAI);
			CheXuanS2CNotifyCuoPai cuoPai;
			cuoPai.m_restTime = m_def_time_cuopai;
			memcpy(cuoPai.m_userStatus, m_user_status, sizeof(cuoPai.m_userStatus));
			memcpy(cuoPai.m_cuoPaiStatus, m_cuopai_status, sizeof(cuoPai.m_cuoPaiStatus));
			memcpy(cuoPai.m_qiaoStatus, m_qiao_status, sizeof(cuoPai.m_qiaoStatus));
			memcpy(cuoPai.m_qiPaiStatus, m_qipai_status, sizeof(cuoPai.m_qiPaiStatus));

			notify_desk(cuoPai);
			notify_lookon(cuoPai);

			//开始记录倒计时
			m_play_status_time.Now();
			return;
		}
	}

	/********************************************************************************************
	* 函数名：      start_game()
	* 描述：        扯炫每一局游戏开始的具体入口
	* 返回：        (void)
	*********************************************************************************************/
	void start_game()
	{
		//设置扯炫每局结束，下一局固定的延时时间
		if (m_desk)
		{
			m_desk->m_next_delay_time = 7;
			if (m_isFastMode)
			{
				m_desk->m_next_delay_time = 3;
			}
		}

		//每局下芒果前，将是否需要下手手芒的标志清0
		memset(m_is_need_add_shouMang, 0, sizeof(m_is_need_add_shouMang));

		//设置起钵钵分不够的玩家留座观战
		if (must_stand_up())
		{
			return;
		}

		if (m_round_offset != 0)
		{
			m_frist_send_card = GetNextPos(m_frist_send_card);
		}

		//挡家位置
		m_dang_player_pos = GetNextPos(m_frist_send_card);

		//设置本次下注开始说话的位置
		m_turn_first_speak_pos = m_dang_player_pos;

		//设置开始说话人的位置为当前位置
		m_curPos = m_turn_first_speak_pos;

		//回放记录公共数据
		m_video.setCommonInfo(gWork.GetCurTime().Secs(), m_round_offset, m_player_count, m_desk->m_vip->m_posUserId, m_frist_send_card);

		//洗牌
		dispatch_user_cards();

		//起钵钵，系统自动下芒果
		start_bobo();

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
		LLOG_DEBUG("CheXuanGameHandler::start_round() Run... SANDAER GAME_START!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! deskId=[%d],",
			m_desk ? m_desk->GetDeskId() : 0);

		Lint t_user_status[CHEXUAN_PLAYER_COUNT];
		memcpy(t_user_status, player_status, sizeof(Lint) * CHEXUAN_PLAYER_COUNT);

		clear_round();

		//广播当前局数给玩家
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);

		memcpy(m_user_status, t_user_status, sizeof(m_user_status));

		//每局下芒果之前记录这局的钵钵数，用于计算
		memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

		//竞技场初始化每个玩家可起钵钵数量，每个人只会初始化一次
		if (m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 0 && !m_has_record_bobo[i])
				{
					m_rest_can_start_bo_score[i] = m_desk->m_vip->m_coins[i];	//能量场赋值
					m_record_bobo_score[i] = m_desk->m_vip->m_coins[i];
					m_has_record_bobo[i] = true;
				}
			}
		}
		
		if (m_desk && m_desk->m_vip)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("desk_id=[%d], start_round=[%d/%d], player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		//游戏开始入口
		start_game();
	}

	/* 洗牌，并在服务端给每个玩家分配好所有的牌 */
	bool dispatch_user_cards()
	{
		//乱序后的扑克牌(32张牌)
		Lint t_randCards[CHEXUAN_CELL_PACK];

		//回放记录玩家手牌
		//std::vector<Lint> vec_handCards[CHEXUAN_CELL_PACK];
		//std::vector<Lint> vec_baseCards;

		//乱序扑克
		m_gamelogic.RandCardList(t_randCards, CHEXUAN_CELL_PACK);

		//配牌
		debugSendCards_CheXuan(t_randCards, m_desk, "card_chexuan.ini", m_player_count, CHEXUAN_HAND_CARDS_COUNT, CHEXUAN_CELL_PACK);

		//这里只是一次性把所有牌分配给每个玩家，并不立即都发给玩家
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			memcpy(m_hand_cards[i], t_randCards + (i * 4), sizeof(Lint) * CHEXUAN_HAND_CARDS_COUNT);

			LLOG_ERROR("CheXuanGameHandler::dispatch_user_cards() Run... deskId=[%d], Player%d handCards=[%x, %x, %x, %x]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_hand_cards[i][0], m_hand_cards[i][1], m_hand_cards[i][2], m_hand_cards[i][3]);
		}
		memcpy(m_hand_cards_backup, m_hand_cards, sizeof(m_hand_cards_backup));

		return true;
	}

	/* 处理玩家起钵钵 */
	bool on_event_player_select_bobo(Lint pos, Lint selectBoScore)
	{
		//TODO:确定本次起钵钵的范围
		/*Lint t_minSelectBoScore = 50;
		Lint t_maxSelectBoScore = 500;

		calc_start_bobo_limit(&t_minSelectBoScore, &t_maxSelectBoScore);
		if (m_rest_can_start_bo_score[pos] < t_maxSelectBoScore)
		{
		t_maxSelectBoScore = m_rest_can_start_bo_score[pos];
		}
		if (t_playerMinSelectBoScore + m_total_bo_score[i] < t_needAddMang)
		{
		t_playerMinSelectBoScore = t_needAddMang - m_total_bo_score[i];
		}
		if (m_rest_can_start_bo_score[pos] < t_minSelectBoScore)
		{
		t_minSelectBoScore = m_rest_can_start_bo_score[pos];
		t_maxSelectBoScore = m_rest_can_start_bo_score[pos];
		}*/

		//不是起钵钵阶段不处理
		if (GAME_PLAY_QIBOBO != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Error!!! this play status is not GAME_PLAY_QIBOBO...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}

		//已经操作过的，不能重复操作
		if (m_qibobo_status[pos] == 1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Error!!! this palyer has opt selectBoScore ...,deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		//校验selectBoScore是否符合规范
		if (!(selectBoScore == 0 && (m_is_need_add_bo[pos] != 1 || m_rest_can_start_bo_score[pos] == 0)) && (selectBoScore < m_min_qi_bo_score[pos] || selectBoScore > m_max_qi_bo_score[pos]))
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Error!!! selectBoScore is invalid...,deskId=[%d], pos=[%d], minSelectBoScore=[%d], maxSelectBoScore=[%d], selectBoScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_min_qi_bo_score[pos], m_max_qi_bo_score[pos], selectBoScore);
			return false;
		}

		//打印该玩家成功起钵钵数据
		LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Player Start BoBo Sucess^_^..., deskId=[%d], pos=[%d], selectBoScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, selectBoScore);

		m_last_bo_score[pos] = selectBoScore; // ? selectBoScore : m_last_bo_score[pos];
		m_total_bo_score[pos] += selectBoScore;
		m_total_bo_score_after_bobo[pos] = m_total_bo_score[pos];

		if (m_rest_can_start_bo_score[pos] - selectBoScore < 0)
		{
			selectBoScore = m_rest_can_start_bo_score[pos];
			m_rest_can_start_bo_score[pos] = 0;
		}
		else
		{
			m_rest_can_start_bo_score[pos] -= selectBoScore;
		}
		m_has_start_total_bo_score[pos] += selectBoScore;
		m_is_need_add_bo[pos] = 0;
		m_qibobo_status[pos] = 1;

		//起钵钵
		CheXuanS2CPlayerSelectBoBoBC qiBoBoBC;
		qiBoBoBC.m_pos = pos;
		memcpy(qiBoBoBC.m_lastBoScore, m_last_bo_score, sizeof(qiBoBoBC.m_lastBoScore));
		memcpy(qiBoBoBC.m_totalBoScore, m_total_bo_score, sizeof(qiBoBoBC.m_totalBoScore));
		memcpy(qiBoBoBC.m_userStatus, m_user_status, sizeof(qiBoBoBC.m_userStatus));
		memcpy(qiBoBoBC.m_optBoBoStatus, m_qibobo_status, sizeof(qiBoBoBC.m_optBoBoStatus));

		notify_desk(qiBoBoBC);
		notify_lookon(qiBoBoBC);

		//所有该起钵钵的玩家操作完成后，进入下芒果阶段
		if (has_opt_over(GAME_PLAY_QIBOBO))
		{
			//每局下芒果之前记录这局的钵钵数，用于计算
			memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

			//更改游戏状态为下芒果阶段
			set_play_status(GAME_PLAY_ADD_MANGGUO);

			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() All Player Opt Start BoBo Over^o^..., deskId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0);

			//起钵钵阶段结束清所有玩家是否需要钵钵状态
			memset(m_is_need_add_bo, 0, sizeof(m_is_need_add_bo));

			//进入下芒果
			add_mang_guo();
		}

		return true;
	}

	/* 处理玩家下注 */
	bool on_event_player_select_opt(Lint pos, Lint selectOptType, Lint selectOptScore)
	{
		//不是下注阶段不处理
		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! this play status is not GAME_PLAY_ADD_SCORE...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}

		//此处校验玩家选择参数是否合法
		if (pos != m_curPos)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! pos is not curPos!!! deskId=[%d], turnCount=[%d], pos=[%d], curPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, m_curPos);
			return false;
		}
		if (m_qipai_status[pos] != 0)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! this user has qi pai!!! deskId=[%d], turnCount=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos);
			return false;
		}
		if (selectOptType < 0 || selectOptType >= ADD_OPT_MAX_INVALID)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! selectOptType is not valid!!! deskId=[%d], turnCount=[%d], pos=[%d], curPos=[%d], selectOptType=[%d], selectOptScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, m_curPos, selectOptType, selectOptScore);
			return false;
		}
		if (m_allow_opt_type[pos][selectOptType] == -1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! not allow [%d] opt!!! deskId=[%d], turnCount=[%d], pos=[%d], selectOptType=[%d], selectOptScore=[%d]",
				selectOptType, m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, selectOptType, selectOptScore);
			return false;
		}
		if (selectOptScore < m_allow_opt_min_score[pos][selectOptType] || selectOptScore > m_allow_opt_max_score[pos][selectOptType])
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! selectOptScore is not valid!!! deskId=[%d], turnCount=[%d], pos=[%d], selectOptType=[%d], selectOptScore=[%d], minOptScore=[%d], maxOptScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, selectOptType, selectOptScore, m_allow_opt_min_score[pos][selectOptType], m_allow_opt_max_score[pos][selectOptType]);
			return false;
		}

		//下的分数超过当前钵钵总数
		if (selectOptScore > m_total_bo_score[pos])
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! selectOptScore has more than total bo score deskId=[%d], turnCount=[%d], pos=[%d], totalBoScore=[%d], selectOptType=[%d], selectOptScore=[%d], minOptScore=[%d], maxOptScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, m_total_bo_score[pos], selectOptType, selectOptScore, m_allow_opt_min_score[pos][selectOptType], m_allow_opt_max_score[pos][selectOptType]);
			return false;
		}

		//玩家下注操作数据
		LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Player Opt Sucess^o^... deskId=[%d], turnCount=[%d], pos=[%d], selectOptType=[%d], selectOptScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, selectOptType, selectOptScore);

		//操作成功后清除，二三轮弃牌有一个动画延时
		m_after_turn_shuai_delay = 0;

		//记录一轮中每位玩家首次操作的分数
		if (m_turn_opt_type[m_turn_count][pos] == ADD_OPT_INVALID)
		{
			m_turn_first_opt_score[m_turn_count][pos] = selectOptScore;
		}

		//如果每轮下注中，下了一圈以后，之前休的状态要清空了
		if (pos == m_turn_first_speak_pos && m_turn_opt_type[m_turn_count][pos] != ADD_OPT_INVALID)
		{
			memset(m_xiu_status, 0, sizeof(m_xiu_status));
		}

		//大的特殊处理，大到选的分数（Ren:2019-02-25）
		if (selectOptType == ADD_OPT_FA_ZHAO)
		{
			selectOptScore -= m_total_add_score[pos];

			//将第一次操作大标记上
			m_is_first_opt_fa_zhao = true;
		}

		m_turn_opt_type[m_turn_count][pos] = selectOptType;
		m_turn_opt_score[m_turn_count][pos] += selectOptScore;
		m_total_bo_score[pos] -= selectOptScore;
		m_total_add_score[pos] += selectOptScore;
		m_player_add_score[pos] += selectOptScore;
		m_turn_round_opt[pos] = 0;
		m_player_last_opt[pos] = selectOptType;

		//检查玩家能量值是否
		cheack_player_coins_warn(pos);

		bool t_isAllHasOptFalg = true;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_turn_round_opt[i] != 0)
			{
				t_isAllHasOptFalg = false;
			}
		}
		if (t_isAllHasOptFalg)
		{
			memset(m_turn_round_opt, 0, sizeof(m_turn_round_opt));
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0)
				{
					m_turn_round_opt[i] = 1;
				}
			}
		}


		//玩家选择弃牌，则所有下注放到皮池中，并标记该玩家弃牌
		if (selectOptType == ADD_OPT_SHUAI)
		{
			m_pi_pool += m_total_add_score[pos];
			//记录该玩家弃牌时候已经下的分
			m_qipai_score[pos] = m_total_add_score[pos];
			//记录玩家输赢的皮分
			m_player_pi_score[pos] -= m_total_add_score[pos];
			m_total_add_score[pos] = 0;
			m_player_add_score[pos] = 0;
			m_qipai_status[pos] = 1;	//标记已经弃牌
			m_qipai_last_pos = pos;		//记录最有一个弃牌的玩家位置

			//二三轮弃牌有一个动画延时
			if (m_turn_count != 0)
			{
				m_after_turn_shuai_delay = 0;
			}
			++m_shuai_count;
		}
		//玩家选择了休，则标记上
		else if (selectOptType == ADD_OPT_XIU)
		{
			m_xiu_status[pos] = 1;
		}
		//玩家选择了敲，标记敲了
		else if (m_qiao_status[pos] == 0 && (selectOptType == ADD_OPT_QIAO_BO || m_total_bo_score[pos] == 0))
		{
			m_qiao_status[pos] = 1;

			//如果敲后的总下注分大于芒果，标记第一次发招
			if (m_total_add_score[pos] >= m_mang_pool && !m_is_first_opt_fa_zhao)
			{
				//m_is_first_opt_fa_zhao = true;
			}
		}
		//玩家选择三花，则所有下注拿回，不包括芒果，并按弃牌标记
		else if (selectOptType == ADD_OPT_SANHUA)
		{
			m_select_san_hua_status[pos] = 1;
			
			//玩家拿回下注分数
			m_total_bo_score[pos] += m_total_add_score[pos];

			//清空已经下注的分数
			m_total_add_score[pos] = 0;

			//标记为已经选择三花（弃牌）
			m_qipai_status[pos] = 2;

			//二三轮弃牌有一个动画延时
			if (m_turn_count != 0)
			{
				m_after_turn_shuai_delay = 0;
			}
		}

		//第一次出现三花，但是没有选择三花，以后将不可以选择三花
		if (m_allow_opt_type[pos][ADD_OPT_SANHUA] != -1 && selectOptType != ADD_OPT_SANHUA)
		{
			m_san_hua_status[pos] = 1;
		}

		//判断本轮操作是否结束, 如果该玩家是倒数第二个玩家，切前面都弃牌了，则不用最后一个玩家操作了，结束
		bool t_isOver = has_opt_over(GAME_PLAY_ADD_SCORE);

		CheXuanS2CPlayerOptBC optBC;
		optBC.m_pos = pos;
		optBC.m_selectType = selectOptType;
		optBC.m_selectScore = selectOptScore;
		optBC.m_turnAddTotalScore = m_turn_opt_score[m_turn_count][pos];
		optBC.m_addTotalScore = m_total_add_score[pos];
		optBC.m_totalBoScore = m_total_bo_score[pos];
		optBC.m_piScore = m_pi_pool;
		optBC.m_mangScore = m_mang_pool;
		optBC.m_nextOptPos = t_isOver ? CHEXUAN_INVALID_POS : GetNextPos(pos);
		optBC.m_is2TimesPre = CheckIsTuo(pos);  //判断是否为拖
		optBC.m_delayTime = m_after_turn_shuai_delay;
		notify_desk(optBC);
		notify_lookon(optBC);

		//如果这轮下注没有结束，则通知下一位下注玩家
		if (!t_isOver)
		{
			m_curPos = GetNextPos(m_curPos);

			bool t_flag = false;
			//如果该玩家已经敲了...查找下一个没有敲的玩家 或 三花牌型玩家 位置
			if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
			{
				Lint t_curPos = m_curPos;
				m_curPos = CHEXUAN_INVALID_POS;
				int t_tmpPos = CHEXUAN_INVALID_POS;
				//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
				for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
				{
					t_tmpPos = i % m_player_count;
					if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
					{
						continue;
					}

					//先找最大下注值(Ren：2019-02-25)
					Lint t_maxAddSocre = 0;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_total_add_score[i] > t_maxAddSocre)
						{
							t_maxAddSocre = m_total_add_score[i];
						}
					}

					m_curPos = t_tmpPos;
					//判断没敲 或者是 首次出现单三花牌型，要给该玩家发送操作提示消息
					bool t_isCanSanHua = false;
					bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
					//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
					//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
					if (
						(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
						t_isCanSanHua) //(Ren:2019-03-15)
					{
						m_curPos = t_tmpPos;
						t_flag = true;
						break;
					}
					m_curPos = CHEXUAN_INVALID_POS;
				}
			}

			//找到下一个没有敲/三花牌型的玩家位置，给这个玩家推送消息
			if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
			{
				Lint t_isMeiDaDong = 0;
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = m_def_time_optscore;
				notifyOpt.m_isMeiDaDong = t_isMeiDaDong;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				memcpy(notifyOpt.m_optType, m_allow_opt_type[m_curPos], sizeof(notifyOpt.m_optType));
				memcpy(notifyOpt.m_optMinScore, m_allow_opt_min_score[m_curPos], sizeof(notifyOpt.m_optMinScore));
				memcpy(notifyOpt.m_optMaxScore, m_allow_opt_max_score[m_curPos], sizeof(notifyOpt.m_optMaxScore));
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);

				//开始记录倒计时
				m_play_status_time.Now();
				return true;
			}
			//如果都敲了
			else
			{
				if (m_turn_count < 2)
				{
					++m_turn_count;
					send_card();
				}
				//第三轮下注结束，进入扯牌阶段
				else if (m_turn_count == 2)
				{
					//本局没有休芒 或 揍芒，则清空休芒、揍芒的倍数
					m_xiuMangCount = 0;
					m_zouMangCount = 0;

					set_play_status(GAME_PLAY_CHE_PAI);
					CheXuanS2CNotifyChePai chePai;
					chePai.m_restTime = m_def_time_chepai;
					memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
					memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
					memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
					memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
					notify_desk(chePai);
					notify_lookon(chePai);

					m_play_status_time.Now();
				}
				return true;
			}
		}

		//如果本轮下注结束，进入下一轮发牌、下注
		if (t_isOver)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() All Player Opt Over this turn count^o^... deskId=[%d], turnCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count);

			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_player_last_opt[i] != ADD_OPT_QIAO_BO && m_player_last_opt[i] != ADD_OPT_SHUAI && m_player_last_opt[i] != ADD_OPT_SANHUA)
				{
					m_player_last_opt[i] = ADD_OPT_INVALID;
				}
			}
			
			m_curPos = CHEXUAN_INVALID_POS;

			//如果所有玩家都休了，则结束本轮，设置下局休芒及倍数
			if (is_xiu_mang())
			{
				//设置本局结束状态为休过河
				m_finish_round_type = 1;

				if (m_isXiuMang)
				{
					//设置本局结束状态为休芒
					m_finish_round_type = 2;
					//计算下一局休芒的数量
					if (m_xiuMangCount == 0) m_xiuMangCount = m_commonMangCount;
					else if (m_isDaMangFanBei) m_xiuMangCount *= 2;  //连续揍芒翻倍
					else m_xiuMangCount = m_commonMangCount;

					//翻倍封顶
					if (!m_isFanBeiFengDing)
					{
						if (m_xiuMangCount > m_commonMangCount * 4)
						{
							m_xiuMangCount = m_commonMangCount * 4;
						}
					}

					//设置下一局需要休芒的玩家标志
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_user_status[i] == 1)
						{
							m_is_need_add_xiuMang[i] = 1;
						}
					}
				}

				//清空揍芒数
				m_zouMangCount = 0;

				finish_round();
				return true;
			}

			//如果有人打芒成功，则结束本轮，设置下局打芒及倍数
			if (is_zou_mang())
			{
				//设置本局结束状态为一人外其他人都弃牌
				m_finish_round_type = 3;

				if (m_isZouMang && m_turn_count == 0)
				{
					//设置本局结束状态为揍芒
					m_finish_round_type = 4;

					//设置下一局揍芒的倍数
					if (m_zouMangCount == 0) m_zouMangCount = m_commonMangCount;
					else if (m_isDaMangFanBei) m_zouMangCount *= 2;  //连续揍芒翻倍
					else m_zouMangCount = m_commonMangCount;

					//翻倍封顶
					if (!m_isFanBeiFengDing)
					{
						if (m_zouMangCount > m_commonMangCount * 4)
						{
							m_zouMangCount = m_commonMangCount * 4;
						}
					}

					//设置下一局需要揍芒的玩家标志
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_qipai_status[i] == 1 && m_user_status[i] == 1)
						{
							m_is_need_add_zouMang[i] = 1;
						}
					}
				}
				else
				{
					m_zouMangCount = 0;
				}

				//清空休芒数
				m_xiuMangCount = 0;

				finish_round();
				return true;
			}

			//不是以上种情况继续
			memset(m_xiu_status, 0, sizeof(m_xiu_status));

			//第二轮、第三轮发牌
			if (m_turn_count < 2)
			{
				++m_turn_count;
				send_card();
			}
			//第三轮下注结束，进入扯牌阶段
			else if (m_turn_count == 2)
			{
				//本局没有休芒 或 揍芒，则清空休芒、揍芒的倍数
				m_xiuMangCount = 0;
				m_zouMangCount = 0;

				set_play_status(GAME_PLAY_CHE_PAI);

				CheXuanS2CNotifyChePai chePai;
				chePai.m_restTime = m_def_time_chepai;
				memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
				memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
				memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
				memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
				notify_desk(chePai);
				notify_lookon(chePai);

				m_play_status_time.Now();
			}
		}
		return true;
	}

	/* 处理玩家扯牌 */
	bool on_event_player_che_pai(Lint pos, Lint* handCards, Lint handCardsCount)
	{
		//不是下扯牌阶段不处理
		if (GAME_PLAY_CHE_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Error!!! this play status is not GAME_PLAY_CHE_PAI...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}

		//TODO:校验牌是否与原始手牌一致
		if (!m_gamelogic.checkTwoHandCardsIsSame(m_hand_cards[pos], handCards, handCardsCount))
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Error!!! srcCards are not same as desCards!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		if (m_chepai_status[pos] != 0)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Error!!! this player has che pai!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		//排序手牌中的两组牌，大的一组在前
		m_gamelogic.sortHandCardsByDouble(handCards, handCardsCount, m_hand_cards_type[pos]);
		memcpy(m_hand_cards[pos], handCards, sizeof(Lint) * handCardsCount);

		//标记已经扯过牌了
		m_chepai_status[pos] = 1;

		CheXuanS2CPlayerChePaiBC palyerChePaiBC;
		palyerChePaiBC.m_pos = pos;
		palyerChePaiBC.m_totalPiScore = 0;
		palyerChePaiBC.m_totalMangScore = 0;
		palyerChePaiBC.m_totalAddScore = 0;
		memcpy(palyerChePaiBC.m_handCards[pos], m_hand_cards[pos], sizeof(Lint) * 4);
		memcpy(palyerChePaiBC.m_cardsType[pos], m_hand_cards_type[pos], sizeof(Lint) * 2);
		memcpy(palyerChePaiBC.m_userStatus, m_user_status, sizeof(palyerChePaiBC.m_userStatus));
		notify_desk(palyerChePaiBC);

		notify_lookon(palyerChePaiBC);

		LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Run... Player che pai Sucess^o^... deskId=[%d], pos=[%d], cardsTypes=[%d, %d], handCards=[%x, %x, %x, %x], handCardsCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, palyerChePaiBC.m_cardsType[pos][0], palyerChePaiBC.m_cardsType[pos][1],
			palyerChePaiBC.m_handCards[pos][0], palyerChePaiBC.m_handCards[pos][1], palyerChePaiBC.m_handCards[pos][2], palyerChePaiBC.m_handCards[pos][3],
			m_hand_cards_count[pos]);

		//所有玩家扯牌结束
		if (has_opt_over(GAME_PLAY_CHE_PAI))
		{
			set_play_status(GAME_PLAY_END);
			//设置扯炫每局结束，下一局固定的延时时间
			if (m_desk)
			{
				m_desk->m_next_delay_time = 10;
				if (m_isFastMode)
				{
					m_desk->m_next_delay_time = 6;
				}
			}

			Lint t_count = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				m_handCardsTypeInfo[t_count].m_userIndex = i;
				m_handCardsTypeInfo[t_count].m_firstType = m_gamelogic.getDoubleCardsTypeValue(m_hand_cards[i][0], m_hand_cards[i][1]);
				m_handCardsTypeInfo[t_count].m_secondType = m_gamelogic.getDoubleCardsTypeValue(m_hand_cards[i][2], m_hand_cards[i][3]);
				m_handCardsTypeInfo[t_count].m_pretHandCards[0] = m_hand_cards[i][0];
				m_handCardsTypeInfo[t_count].m_pretHandCards[1] = m_hand_cards[i][1];
				m_handCardsTypeInfo[t_count].m_nextHandCards[0] = m_hand_cards[i][2];
				m_handCardsTypeInfo[t_count].m_nextHandCards[1] = m_hand_cards[i][3];
				++t_count;
			}

			//保存实际参与游戏的人数
			m_real_player_count = t_count;

			//将第二组牌按从小到大排序
			m_gamelogic.sortAllPlayerCards(m_handCardsTypeInfo, t_count, GetNextPos(m_frist_send_card), m_player_count);

			for (size_t i = 0; i < t_count; ++i)
			{
				LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai(), deskId=[%d], No.%d, pos=[%d], cardType=[%d, %d] handCards=[%x, %x, %x, %x], handCardsCount=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_handCardsTypeInfo[i].m_userIndex, m_handCardsTypeInfo[i].m_firstType, m_handCardsTypeInfo[i].m_secondType,
					m_handCardsTypeInfo[i].m_pretHandCards[0], m_handCardsTypeInfo[i].m_pretHandCards[1],
					m_handCardsTypeInfo[i].m_nextHandCards[0], m_handCardsTypeInfo[i].m_nextHandCards[1],
					m_hand_cards_count[m_handCardsTypeInfo[i].m_userIndex]);
			}

			//所有玩家下注的分数
			Lint t_totalAddScore = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0)
				{
					t_totalAddScore += m_total_add_score[i];
				}
			}

			//暗牌结算观战
			if (m_isLookOnDarkResult)
			{
				CheXuanS2CPlayerChePaiBC chePaiBC;
				chePaiBC.m_pos = CHEXUAN_INVALID_POS;
				chePaiBC.m_totalPiScore = m_pi_pool;
				chePaiBC.m_totalMangScore = m_mang_pool;
				chePaiBC.m_totalAddScore = t_totalAddScore;
				memcpy(chePaiBC.m_userStatus, m_user_status, sizeof(chePaiBC.m_userStatus));
				memcpy(chePaiBC.m_qiPaiStatus, m_qipai_status, sizeof(chePaiBC.m_qiPaiStatus));
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0)
					{
						continue;
					}
					memcpy(chePaiBC.m_handCards[i], m_hand_cards[i], sizeof(Lint) * 4);
					memcpy(chePaiBC.m_cardsType[i], m_hand_cards_type[i], sizeof(Lint) * 2);
				}
				notify_desk_playing_user(chePaiBC);

				//暗牌广播给观战玩家
				CheXuanS2CPlayerChePaiBC gz_chePaiBC;
				gz_chePaiBC.m_pos = CHEXUAN_INVALID_POS;
				gz_chePaiBC.m_totalPiScore = m_pi_pool;
				gz_chePaiBC.m_totalMangScore = m_mang_pool;
				gz_chePaiBC.m_totalAddScore = t_totalAddScore;
				memcpy(gz_chePaiBC.m_userStatus, m_user_status, sizeof(gz_chePaiBC.m_userStatus));
				memcpy(gz_chePaiBC.m_qiPaiStatus, m_qipai_status, sizeof(gz_chePaiBC.m_qiPaiStatus));
				
				notify_desk_seat_no_playing_user(gz_chePaiBC);
				notify_lookon(gz_chePaiBC);
			}
			else
			{
				CheXuanS2CPlayerChePaiBC chePaiBC;
				chePaiBC.m_pos = CHEXUAN_INVALID_POS;
				chePaiBC.m_totalPiScore = m_pi_pool;
				chePaiBC.m_totalMangScore = m_mang_pool;
				chePaiBC.m_totalAddScore = t_totalAddScore;
				memcpy(chePaiBC.m_userStatus, m_user_status, sizeof(chePaiBC.m_userStatus));
				memcpy(chePaiBC.m_qiPaiStatus, m_qipai_status, sizeof(chePaiBC.m_qiPaiStatus));
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0)
					{
						continue;
					}
					memcpy(chePaiBC.m_handCards[i], m_hand_cards[i], sizeof(Lint) * 4);
					memcpy(chePaiBC.m_cardsType[i], m_hand_cards_type[i], sizeof(Lint) * 2);
				}

				notify_desk(chePaiBC);
				notify_lookon(chePaiBC);
			}

			finish_round();
		}
		return true;
	}

	/* 处理玩家搓牌 */
	bool on_event_player_cuo_pai(Lint pos)
	{
		//不是搓牌阶段不处理
		if (GAME_PLAY_CUO_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Error!!! this play status is not GAME_PLAY_CUO_PAI...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}
		//
		if (m_qipai_status[pos] != 0 || m_user_status[pos] != 1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Error!!! this player can not cuo pai!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}
		if (m_cuopai_status[pos] == 1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Error!!! this player has cuo pai!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Run... Player cuo pai... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		//标记该玩家已经搓牌过
		m_cuopai_status[pos] = 1;

		//暗牌观战
		if (m_isLookOnDarkResult)
		{
			CheXuanS2CPlayerCuoPaiBC cuoPaiBC;
			cuoPaiBC.m_pos = pos;
			cuoPaiBC.m_cuoCard = m_hand_cards[pos][3];
			notify_desk_playing_user(cuoPaiBC);
			
			CheXuanS2CPlayerCuoPaiBC gz_cuoPaiBC;
			gz_cuoPaiBC.m_pos = pos;
			notify_desk_seat_no_playing_user(gz_cuoPaiBC);
			notify_lookon(gz_cuoPaiBC);
		}
		else
		{
			CheXuanS2CPlayerCuoPaiBC cuoPaiBC;
			cuoPaiBC.m_pos = pos;
			cuoPaiBC.m_cuoCard = m_hand_cards[pos][3];
			notify_desk(cuoPaiBC);
			notify_lookon(cuoPaiBC);
		}

		//所有人搓牌结束
		if (has_opt_over(GAME_PLAY_CUO_PAI))
		{
			//更改牌局状态为：下注
			set_play_status(GAME_PLAY_ADD_SCORE);
			//找出第四张牌最大的玩家位置
			Lint t_thredCards[CHEXUAN_PLAYER_COUNT];
			memset(t_thredCards, 0, sizeof(Lint)*CHEXUAN_PLAYER_COUNT);
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				t_thredCards[i] = m_hand_cards[i][3];
			}
			m_turn_first_speak_pos = m_gamelogic.getMaxCardPos(t_thredCards, m_player_count, m_player_count, m_dang_player_pos);
			m_curPos = m_turn_first_speak_pos;

			bool t_flag = false;
			//如果该玩家已经敲了...查找下一个没有敲的玩家 或 三花牌型玩家 位置
			if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
			{
				Lint t_curPos = m_curPos;
				m_curPos = CHEXUAN_INVALID_POS;
				int t_tmpPos = m_curPos;
				//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
				for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
				{
					t_tmpPos = i % m_player_count;
					if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
					{
						continue;
					}

					//先找最大下注值(Ren：2019-02-25)
					Lint t_maxAddSocre = 0;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_total_add_score[i] > t_maxAddSocre)
						{
							t_maxAddSocre = m_total_add_score[i];
						}
					}

					//判断没敲 或者是 首次出现单三花牌型，要给该玩家发送操作提示消息
					bool t_isCanSanHua = false;
					bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
					//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
					//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
					if (
						(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
						t_isCanSanHua) //(Ren:2019-03-15)
					{
						m_turn_round_opt[t_tmpPos] = 1;
						m_curPos = t_tmpPos;
						t_flag = true;
						break;
					}
					m_curPos = CHEXUAN_INVALID_POS;
				}
			}

			//找到下一个没有敲/三花牌型的玩家位置，给这个玩家推送消息
			if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
			{
				Lint t_isMeiDaDong = 0;
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = m_def_time_3rd_optscore;
				notifyOpt.m_isMeiDaDong = t_isMeiDaDong;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				for (int i = 0; i < 10; ++i)
				{
					notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
					notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
					notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
				}
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);

				//开始记录倒计时
				m_play_status_time.Now();
				return true;
			}
			else
			{
				//本局没有休芒 或 揍芒，则清空休芒、揍芒的倍数
				m_xiuMangCount = 0;
				m_zouMangCount = 0;

				set_play_status(GAME_PLAY_CHE_PAI);
				CheXuanS2CNotifyChePai chePai;
				chePai.m_restTime = m_def_time_chepai;
				memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
				memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
				memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
				memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
				notify_desk(chePai);
				notify_lookon(chePai);

				m_play_status_time.Now();
				return true;
			}
		}

		return true;
	}

	/* 处理玩家选择摸 */
	bool on_event_player_opt_mo_fen(Lint pos, Lint opt)
	{
		if (pos != m_dang_player_pos)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_opt_mo_fen() Error!!! opt mo fen is not dang pos...,deskId=[%d], pos=[%d], dangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_dang_player_pos);
			return false;
		}

		if (m_turn_count != 0)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_opt_mo_fen() Error!!! opt mo fen is not dang pos...,deskId=[%d], pos=[%d], dangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_dang_player_pos);
			return false;
		}

		if (m_selectMoFen != -1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_opt_mo_fen() Error!!! has opt mo fen ...,deskId=[%d], pos=[%d], selectMoFen=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_selectMoFen);
			return false;
		}

		//选择的默分
		m_selectMoFen = opt;

		//给玩家返回操作结果通知
		CheXuanS2CPlayerOptMoFen moFen;
		moFen.m_pos = pos;
		moFen.m_optMo = opt;
		notify_desk(moFen);
		notify_lookon(moFen);

		//计算玩家可选操作和分数
		bool t_flag = false;
		//如果该玩家已经敲了...查找下一个没有敲的玩家 或 三花牌型玩家 位置
		if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
		{
			Lint t_curPos = m_curPos;
			m_curPos = CHEXUAN_INVALID_POS;
			int t_tmpPos = CHEXUAN_INVALID_POS;
			//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
			for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
			{
				t_tmpPos = i % m_player_count;
				if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
				{
					continue;
				}

				//先找最大下注值(Ren：2019-02-25)
				Lint t_maxAddSocre = 0;
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_total_add_score[i] > t_maxAddSocre)
					{
						t_maxAddSocre = m_total_add_score[i];
					}
				}

				//判断没敲 或者是 首次出现单三花牌型，要给该玩家发送操作提示消息
				bool t_isCanSanHua = false;
				bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
				bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
				t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
				//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
				//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
				if (
					(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
					(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
					(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
					(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
					t_isCanSanHua) //(Ren:2019-03-15)
				{
					m_turn_round_opt[t_tmpPos] = 1;
					m_curPos = t_tmpPos;
					t_flag = true;
					break;
				}
				m_curPos = CHEXUAN_INVALID_POS;
			}
		}

		//找到下一个没有敲/三花牌型的玩家位置，给这个玩家推送消息
		if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
		{
			//计算该玩家允许的操作
			calc_opt_score();

			CheXuanS2CNotifyPlayerOpt notifyOpt;
			notifyOpt.m_pos = m_curPos;
			notifyOpt.m_restTime = 0;
			notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
			for (int i = 0; i < 10; ++i)
			{
				notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
				notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
				notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
			}
			notify_desk(notifyOpt);
			notify_lookon(notifyOpt);
			//notify_club_ower_lookon(notifyOpt);

			//开始记录倒计时
			//m_play_status_time.Now();

			return true;
		}
		//都敲了，或者只有一个人操作不是敲
		else
		{
			++m_turn_count;
			send_card();
			return true;
		}

		return true;
	}

	/* 处理玩家选择自动操作 */
	bool on_event_player_select_auto_opt(Lint pos, Lint autoOptType)
	{
		if (autoOptType < 0 || autoOptType > 3)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_auto_opt() Error!!! Auto opt type is invalid...,deskId=[%d], pos=[%d], autoOptType=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, autoOptType);

			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::on_event_player_select_auto_opt() Run... deskId=[%d], pos=[%d], autoOptType=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, autoOptType);

		m_playerAutoOpt[pos] = autoOptType;
		
		//通知玩家操作结果
		CheXuanS2CPlayerSelectAutoOptRet optAutoRet;
		optAutoRet.m_pos = pos;
		optAutoRet.m_autoOptType = autoOptType;
		notify_user(optAutoRet, pos);

		return true;
	}

	/* 处理玩家选择站起或坐下操作 */
	bool on_event_player_stand_or_seat(Lint pos, Lint standOrSeat)
	{
		//玩家已经操作过站起或者坐下
		if (m_player_stand_up[pos] == 1 || m_player_stand_up[pos] == 3)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_stand_or_seat() Error!!! Player has select stand or seat!!!,deskId=[%d], pos=[%d], m_player_stand_up=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_player_stand_up[pos]);

			return false;
		}
		//玩家不能重复操作
		if ((m_player_stand_up[pos] == 0 && standOrSeat == 0) || (m_player_stand_up[pos] == 2 && standOrSeat == 1))
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_stand_or_seat() Error!!! Player can not repeat opt stand or seat!!!,deskId=[%d], pos=[%d], m_player_stand_up=[%d], standOrSeat=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_player_stand_up[pos], standOrSeat);

			return false;
		}

		//坐下
		if (standOrSeat == 0)
		{
			m_player_stand_up[pos] = 3;
		}
		//站起
		else if (standOrSeat == 1)
		{
			m_player_stand_up[pos] = 1;
		}
		CheXuanS2CPlayerSelectStandOrSeatRet ret;
		ret.m_pos = pos;
		ret.m_standOrSeat = m_player_stand_up[pos];
		notify_user(ret, pos);

		return true;
	}

	//计算分数
	void calc_score(Lint finishRoundType, Lint* maxWinnerScore, Lint* maxWinnerPos)
	{
		LLOG_ERROR("CheXuanGameHandler::calc_score() Run... this round finish type=[%d]... round=[%d/%d], deskId=[%d]",
			finishRoundType, m_round_offset, m_round_limit, m_desk ? m_desk->GetDeskId() : 0);

		switch (finishRoundType)
		{
		//正常结束
		case 0:
			if (m_isClearPool)
			{
				calc_score_nomal();
			}
			else
			{
				calc_score_nomal_noClear();

				if (m_round_offset == m_round_limit - 1)
				{
					LLOG_ERROR("CheXuanGameHandler::calc_score() Run... clear all score... deskId=[%d]", 
						m_desk ? m_desk->GetDeskId() : 0);
					clear_all_score();
				}
			}
			break;
		//休过河、休芒
		case 1:
		case 2:
			if (m_isClearPool)
			{
				calc_score_xiuMang();
			}
			else
			{
				calc_score_xiuMang_noClear();

				if (m_round_offset == m_round_limit - 1)
				{
					LLOG_ERROR("CheXuanGameHandler::calc_score() Run... clear all score...");
					clear_all_score();
				}
			}
			break;
			//除一人外所有人弃牌、揍芒
		case 3:
		case 4:
			if (m_isClearPool || m_round_offset == m_round_limit - 1)
			{
				calc_score_zouMang();
			}
			else
			{
				calc_score_zouMang_noClear();

				if (m_round_offset == m_round_limit - 1)
				{
					LLOG_ERROR("CheXuanGameHandler::calc_score() Run... clear all score...");
					clear_all_score();
				}
			}
			break;
		default:
			break;
		}

		Lint t_maxWinner = 0;
		Lint t_maxWinnerPos = CHEXUAN_INVALID_POS;
		//计算每个玩家本局得的分
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;

			m_player_score[i] = m_total_bo_score[i] - m_total_bo_score_after_bobo[i];
		}

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_player_score[i] > t_maxWinner)
			{
				t_maxWinner = m_player_score[i];
				t_maxWinnerPos = i;
			}
		}
		*maxWinnerScore = t_maxWinner;
		*maxWinnerPos = t_maxWinnerPos;
	}

	//正常结算玩家输赢下注分数、皮分、芒果分数
	void calc_common_score()
	{
		//选择龙心肺输分封顶选项：
		//每家每局输的下注分上限 = 牌比自己大的玩家中下注分最多的玩家分值，输到分后便不再输分，余分会在比牌结束后拿回
		Lint t_playerLoseMaxScore[CHEXUAN_PLAYER_COUNT];
		memcpy(t_playerLoseMaxScore, m_total_add_score, sizeof(t_playerLoseMaxScore));
		LLOG_ERROR("CheXuanGameHandler::calc_common_score() Run... deskId=[%d], m_isLongXinFeiMaxScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_isLongXinFeiMaxScore);
		if (m_isLongXinFeiMaxScore)
		{
			memset(t_playerLoseMaxScore, 0, sizeof(t_playerLoseMaxScore));
			for (int i = 0; i < m_real_player_count; ++i)
			{
				for (int j = 0; j < i; ++j)
				{
					//i玩家之前的玩家只要大于i的就更新一下i玩家最大输的分数
					if (
						(m_handCardsTypeInfo[j].m_firstType > m_handCardsTypeInfo[i].m_firstType && m_handCardsTypeInfo[j].m_secondType >= m_handCardsTypeInfo[i].m_secondType) ||
						(m_handCardsTypeInfo[j].m_firstType >= m_handCardsTypeInfo[i].m_firstType && m_handCardsTypeInfo[j].m_secondType > m_handCardsTypeInfo[i].m_secondType)
						)
					{
						if (m_total_add_score[m_handCardsTypeInfo[j].m_userIndex] > t_playerLoseMaxScore[m_handCardsTypeInfo[i].m_userIndex])
						{
							t_playerLoseMaxScore[m_handCardsTypeInfo[i].m_userIndex] = m_total_add_score[m_handCardsTypeInfo[j].m_userIndex];
						}
					}
				}
			}

			//根据玩家具体下的分数再次更新玩家输的最大分数
			Lint t_playerRealPos = CHEXUAN_INVALID_POS;
			for (int i = 0; i < m_real_player_count; ++i)
			{
				t_playerRealPos = m_handCardsTypeInfo[i].m_userIndex;
				t_playerLoseMaxScore[t_playerRealPos] = (t_playerLoseMaxScore[t_playerRealPos] > m_total_add_score[t_playerRealPos]) ? m_total_add_score[t_playerRealPos] : t_playerLoseMaxScore[t_playerRealPos];
			}
		}

		//每个人依次比牌
		for (int i = 0; i < m_real_player_count - 1; ++i)
		{
			//每个人跟其后面的玩家比牌，输赢下注分数
			for (int j = i + 1; j < m_real_player_count; ++j)
			{
				Lint t_winPos = CHEXUAN_INVALID_POS;
				Lint t_losPos = CHEXUAN_INVALID_POS;
				Lint t_pingPos1 = CHEXUAN_INVALID_POS;
				Lint t_pingPos2 = CHEXUAN_INVALID_POS;

				//i > j
				if (
					(m_handCardsTypeInfo[i].m_firstType > m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType >= m_handCardsTypeInfo[j].m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType >= m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType > m_handCardsTypeInfo[j].m_secondType)
					)
				{
					t_winPos = m_handCardsTypeInfo[i].m_userIndex;
					t_losPos = m_handCardsTypeInfo[j].m_userIndex;
				}
				// i < j
				else if (
					(m_handCardsTypeInfo[i].m_firstType < m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType <= m_handCardsTypeInfo[j].m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType <= m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType < m_handCardsTypeInfo[j].m_secondType)
					)
				{
					t_losPos = m_handCardsTypeInfo[i].m_userIndex;
					t_winPos = m_handCardsTypeInfo[j].m_userIndex;
				}
				// i == j
				else
				{
					t_pingPos1 = i;
					t_pingPos2 = j;
				}

				//分出大小，牌大的玩家赢牌小玩家的下注分数
				if (t_winPos != CHEXUAN_INVALID_POS && t_losPos != CHEXUAN_INVALID_POS &&
					t_pingPos1 == CHEXUAN_INVALID_POS && t_pingPos2 == CHEXUAN_INVALID_POS)
				{
					//赢的玩家分数(之前逻辑)
					/*
					if (m_total_add_score[t_losPos] >= m_total_bo_score_after_bobo[t_winPos] && m_total_add_score[t_losPos] > 0)
					{
						m_total_add_score[t_losPos] -= m_total_bo_score_after_bobo[t_winPos];
						m_total_bo_score[t_winPos] += m_total_bo_score_after_bobo[t_winPos];
					}
					else if (m_total_add_score[t_losPos] < m_total_bo_score_after_bobo[t_winPos] && m_total_add_score[t_losPos] > 0)
					{
						m_total_bo_score[t_winPos] += m_total_add_score[t_losPos];
						m_total_add_score[t_losPos] = 0;
					}
					*/
					//赢的玩家分数(兼容龙心肺输分封顶的逻辑)
					if (t_playerLoseMaxScore[t_losPos] >= m_total_bo_score_after_bobo[t_winPos] && t_playerLoseMaxScore[t_losPos] > 0)
					{
						t_playerLoseMaxScore[t_losPos] -= m_total_bo_score_after_bobo[t_winPos];
						m_total_add_score[t_losPos] -= m_total_bo_score_after_bobo[t_winPos];
						m_total_bo_score[t_winPos] += m_total_bo_score_after_bobo[t_winPos];
					}
					else if (t_playerLoseMaxScore[t_losPos] < m_total_bo_score_after_bobo[t_winPos] && t_playerLoseMaxScore[t_losPos] > 0)
					{
						m_total_bo_score[t_winPos] += t_playerLoseMaxScore[t_losPos];
						m_total_add_score[t_losPos] -= t_playerLoseMaxScore[t_losPos];
						t_playerLoseMaxScore[t_losPos] = 0;
					}

				}
				//比牌结果平局，谁也不赢谁的下注分数
				else
				{

				}
			}

			//本次比牌赢皮、芒果玩家位置
			Lint t_turnWinPos = m_handCardsTypeInfo[i].m_userIndex;

			//跟所有未弃牌的玩家比牌结束后，开始赢弃牌（三花除外）玩家皮
			if (m_shuai_count > 0)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 1 || t_turnWinPos == i) continue;

					if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_turnWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_total_bo_score_after_bobo[t_turnWinPos];
						m_qipai_score[i] -= m_total_bo_score_after_bobo[t_turnWinPos];
						m_total_bo_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
						m_player_pi_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
					}
					else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_turnWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_qipai_score[i];
						m_total_bo_score[t_turnWinPos] += m_qipai_score[i];
						m_player_pi_score[t_turnWinPos] += m_qipai_score[i];
						m_qipai_score[i] = 0;
					}
				}
			}

			//赢完所有弃牌人的皮后，开始赢一次芒果池
			if (m_mang_pool > 0)
			{
				if (m_mang_pool >= m_total_bo_score_after_bobo[t_turnWinPos])
				{
					m_mang_pool -= m_total_bo_score_after_bobo[t_turnWinPos];
					m_total_bo_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
					m_player_mang_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
				}
				else if (m_mang_pool < m_total_bo_score_after_bobo[t_turnWinPos])
				{
					m_total_bo_score[t_turnWinPos] += m_mang_pool;
					m_player_mang_score[t_turnWinPos] += m_mang_pool;
					m_mang_pool = 0;
				}
			}
		}

		if (m_real_player_count > 0)
		{
			//如果有弃牌玩家，则没有弃牌的最小牌玩家要赢每个弃牌玩家的皮一次
			Lint t_tureMinWinPos = m_handCardsTypeInfo[m_real_player_count - 1].m_userIndex;

			//最小牌玩家赢每个弃牌玩家皮一次
			if (m_shuai_count > 0)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 1) continue;

					if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_tureMinWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_total_bo_score_after_bobo[t_tureMinWinPos];
						m_qipai_score[i] -= m_total_bo_score_after_bobo[t_tureMinWinPos];
						m_total_bo_score[t_tureMinWinPos] += m_total_bo_score_after_bobo[t_tureMinWinPos];
						m_player_pi_score[t_tureMinWinPos] += m_total_bo_score_after_bobo[t_tureMinWinPos];
					}
					else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_tureMinWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_qipai_score[i];
						m_total_bo_score[t_tureMinWinPos] += m_qipai_score[i];
						m_player_pi_score[t_tureMinWinPos] += m_qipai_score[i];
						m_qipai_score[i] = 0;
					}
				}
			}
		}

		//玩家下注的分有剩余，各自拿回
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_total_add_score[i] > 0 && m_user_status[i] == 1 && m_qipai_status[i] == 0)
			{
				m_total_bo_score[i] += m_total_add_score[i];
				m_total_add_score[i] = 0;
			}
		}
	}

	//清空芒、皮池 正常小结算
	void calc_score_nomal()
	{
		//正常结算玩家输赢下注分数、皮分、芒果分数
		calc_common_score();

		/*
		  清空芒、皮：芒果池剩余分数平分所有在局玩家（不包括留座起身玩家、观战玩家），
		  有余分的给在局玩家牌型最小（如果在局玩家中有弃牌，弃牌最小，三花视同弃牌）的玩家，
		  当出现多个牌型最小玩家，则余分给离挡家最远的玩家；
		*/

		//参与本局游戏实际人数（包括弃牌、三花玩家）
		Lint t_playerCount = calc_real_player_count();

		//获得芒果池平分后余分的玩家位置：牌最小且逆时针离挡家最远的玩家位置
		Lint t_getYuMangPos = findMinCardsAndFarDangPos_QiPai();

		//获得皮池平分后余分的玩家位置：离挡家最远的玩家位置
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);
		

		//芒果池有剩余：平分所有在局玩家（不包括留座起身玩家），有余分的给在局玩家牌型最小的玩家，当出现多个牌型最小玩家，则余分给离挡家最远的玩家
		if (m_mang_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_playerCount != 0)
			{
				t_yushu = m_mang_pool % t_playerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_playerCount;
			}
			
			for (int i = 0; i < t_playerCount; ++i)
			{
				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
			}

			//不能平分芒果余数给离挡家最近的最小的玩家
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;

			//清空芒果池
			m_mang_pool = 0;
		}

		//皮池有剩余：皮池剩余分数平分所有在局玩家（不包括留座起身玩家），有余分时给本局中最后弃牌的玩家，当本局无人弃牌时，则余分给离挡家最远的玩家；
		/*
		if (m_pi_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_playerCount != 0)
			{
				t_yushu = m_pi_pool % t_playerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_playerCount;
			}

			for (int i = 0; i < t_playerCount; ++i)
			{
				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
			}

			//如果本局有最后玩家弃牌，则余分分给最后弃牌玩家
			t_getYuPiPos = m_qipai_last_pos == CHEXUAN_INVALID_POS ? t_getYuPiPos : m_qipai_last_pos;

			//不能平分皮余数给离挡家最近的最小的玩家
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;

			//清空皮池
			m_pi_pool = 0;
		}
		*/
	}

	//不清空芒果、皮池 正常算分
	void calc_score_nomal_noClear()
	{
		calc_common_score();
	}

	//清空芒果、皮池 休芒算分
	void calc_score_xiuMang()
	{
		//恢复每个玩家的分数到本局开始之前的分数
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			m_total_bo_score[i] = m_total_bo_score_after_bobo[i];
			m_player_mang_score[i] = 0;
			m_player_pi_score[i] = 0;
			m_total_add_score[i] = 0;
		}
		m_mang_pool = 0;
		m_pi_pool = 0;
	}

	//不清空芒果、皮池 休芒算分
	void calc_score_xiuMang_noClear()
	{
		//休的玩家拿回自己下注分数
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_qipai_status[i] == 0)
			{
				m_total_bo_score[i] += m_total_add_score[i];
				m_total_add_score[i] = 0;
			}
		}

		//弃牌玩家拿回自己下注的分数
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_qipai_status[i] == 1)
			{
				if (m_pi_pool - m_qipai_score[i] >= 0 && m_pi_pool >= 0)
				{
					m_total_bo_score[i] += m_qipai_score[i];
					m_pi_pool -= m_qipai_score[i];
					m_player_pi_score[i] += m_qipai_score[i];
				}
				else if (m_pi_pool - m_qipai_score[i] < 0 && m_pi_pool >= 0)
				{
					m_total_bo_score[i] += m_pi_pool;
					m_player_pi_score[i] += m_pi_pool;
					m_pi_pool = 0;
				}
			}
		}
	}

	//清空芒果、皮池 揍芒算分
	void calc_score_zouMang()
	{
		//本局开始，所有参与游戏的玩家数量即：m_user_state[i] == 1
		Lint t_playerCount = 0;

		//休的玩家数量
		Lint t_xiuCount = 0;

		//弃牌玩家数量
		Lint t_shuaiCount = 0;

		//揍芒玩家的位置
		Lint t_zouMangPos = CHEXUAN_INVALID_POS;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1) ++t_playerCount;
			if (m_qipai_status[i] == 1) ++t_shuaiCount;

			if (m_user_status[i] != 1 || m_qipai_status[i] != 0 || m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU) continue;

			t_zouMangPos = i;
		}

		//揍芒玩家赢所有弃牌玩家皮
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1 || m_qipai_status[i] != 1 || i == t_zouMangPos) continue;

			if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_qipai_score[i] -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
				m_player_pi_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			}
			else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_qipai_score[i];
				m_total_bo_score[t_zouMangPos] += m_qipai_score[i];
				m_player_pi_score[t_zouMangPos] += m_qipai_score[i];
				m_qipai_score[i] = 0;
			}
		}
		
		//揍芒人赢一次芒果
		if (m_mang_pool >= m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_mang_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
			m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			m_player_mang_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
		}
		else if (m_mang_pool < m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_total_bo_score[t_zouMangPos] += m_mang_pool;
			m_player_mang_score[t_zouMangPos] += m_mang_pool;
			m_mang_pool = 0;
		}

		//揍芒玩家拿回自己下注的分数
		m_total_bo_score[t_zouMangPos] += m_total_add_score[t_zouMangPos];
		m_total_add_score[t_zouMangPos] = 0;



		//本局实际参与人数
		Lint t_realPlayerCount = calc_real_player_count();

		//获得芒果池平分后余分的玩家位置：有余分给离挡家最远的玩家
		Lint t_getYuMangPos = GetRealPrePos(m_dang_player_pos);

		//获得皮池平分后余分的玩家位置：离挡家最远的玩家位置
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);

		//剩余的芒果：芒果池剩余分数平分所有在局玩家（不包括留座起身玩家、观战玩家），有余分给离挡家最远的玩家；
		if (m_mang_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_mang_pool % t_realPlayerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
			}

			//不能平分芒果余数给离挡家最近的最小的玩家
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;

			//清空芒果池
			m_mang_pool = 0;
		}

		//剩下的皮:平分所有在局玩家（不包括留座起身玩家），有余分时给本局中离挡家最远的玩家；
		/*
		if (m_pi_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_pi_pool % t_realPlayerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
			}

			//如果本局有最后玩家弃牌，则余分分给最后弃牌玩家
			t_getYuPiPos = m_qipai_last_pos == CHEXUAN_INVALID_POS ? t_getYuPiPos : m_qipai_last_pos;

			//不能平分皮余数给离挡家最近的最小的玩家
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;

			//清空皮池
			m_pi_pool = 0;
		}
		*/
	}

	//不清空芒果、皮池 揍芒算分
	void calc_score_zouMang_noClear()
	{
		//本局开始，所有参与游戏的玩家数量即：m_user_state[i] == 1
		Lint t_playerCount = 0;

		//休的玩家数量
		Lint t_xiuCount = 0;

		//弃牌玩家数量
		Lint t_shuaiCount = 0;

		//揍芒玩家的位置
		Lint t_zouMangPos = CHEXUAN_INVALID_POS;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1) ++t_playerCount;
			if (m_qipai_status[i] == 1) ++t_shuaiCount;

			if (m_user_status[i] != 1 || m_qipai_status[i] != 0 || m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU) continue;

			t_zouMangPos = i;
		}

		//揍芒玩家赢所有弃牌玩家皮
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1 || m_qipai_status[i] != 1 || i == t_zouMangPos) continue;

			if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_qipai_score[i] -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
				m_player_pi_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			}
			else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_qipai_score[i];
				m_total_bo_score[t_zouMangPos] += m_qipai_score[i];
				m_player_pi_score[t_zouMangPos] += m_qipai_score[i];
				m_qipai_score[i] = 0;
			}
		}

		//揍芒人赢一次芒果
		if (m_mang_pool >= m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_mang_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
			m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			m_player_mang_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
		}
		else if (m_mang_pool < m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_total_bo_score[t_zouMangPos] += m_mang_pool;
			m_player_mang_score[t_zouMangPos] += m_mang_pool;
			m_mang_pool = 0;
		}

		//揍芒玩家拿回自己下注的分数
		m_total_bo_score[t_zouMangPos] += m_total_add_score[t_zouMangPos];
		m_total_add_score[t_zouMangPos] = 0;
	}

	//清空所有分,在不清空每局分的选项下，最后一局清空所有分数
	void clear_all_score()
	{
		//本局实际参与人数
		Lint t_realPlayerCount = calc_real_player_count();

		//获得芒果池平分后余分的玩家位置：牌最小且逆时针离挡家最远的玩家位置
		Lint t_getYuMangPos = findMaxLoseAndFarDangPos_QiShen(m_accum_score);

		//获得皮池平分后余分的玩家位置：离挡家最远的玩家位置
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);

		
		//芒果池有剩余:芒果池剩余分数平分所有在局玩家（不包括留座起身玩家），有余分的给前面几局输分最多的玩家，当出现多个输分最多玩家，则余分给最后一局离挡家最远的玩家；
		if (m_mang_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_realPlayerCount > 0)
			{
				t_yushu = m_mang_pool % t_realPlayerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
				m_tui_mang_score[i] = t_tuihui;
			}

			//有余分的给前面几局输分最多的玩家（包括留座起身玩家），当出现多个输分最多玩家，则余分给本局离挡家最远的玩家；
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;
			m_tui_mang_score[t_getYuMangPos] += t_yushu;

			//设置获得余分芒果起身玩家位置
			if (t_getYuMangPos != CHEXUAN_INVALID_POS && m_user_status[t_getYuMangPos] == 2 && t_yushu > 0)
			{
				m_last_get_yu_mang = t_getYuMangPos;
			}

			//清空芒果池
			m_mang_pool = 0;
		}

		//皮池有剩余:皮池剩余分数平分所有在局玩家（不包括留座起身玩家），有余分给离挡家最远的玩家；
		/*
		if (m_pi_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_realPlayerCount > 0)
			{
				t_yushu = m_pi_pool % t_realPlayerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
			}

			//不能平分皮余数给离挡家最近的最小的玩家
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;

			//清空皮池
			m_pi_pool = 0;
		}
		*/
	}

	//牌局中解散清空所有下注分数
	void jie_san_clear_all_score()
	{
		//本局实际参与人数
		Lint t_realPlayerCount = calc_real_player_count();

		//所有人恢复到本局开局以前的总分数
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			m_total_bo_score[i] = m_total_bo_score_after_bobo[i];
			m_player_mang_score[i] = 0;
			m_player_pi_score[i] = 0;
			m_total_add_score[i] = 0;
		}

		//芒果皮池恢复到上局结束时的芒果皮池
		m_pi_pool = m_per_pi_pool;
		m_mang_pool = m_per_mang_pool;

		//获得芒果池平分后余分的玩家位置：牌最小且逆时针离挡家最远的玩家位置
		Lint t_getYuMangPos = findMaxLoseAndFarDangPos_QiShen(m_accum_score);
		//获得皮池平分后余分的玩家位置：离挡家最远的玩家位置
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);

		//芒果池剩余分数平分所有在局玩家（不包括留座起身玩家），有余分的给前面几局输分最多的玩家，当出现多个输分最多玩家，则余分给本局离挡家最远的玩家；
		if (m_mang_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_mang_pool % t_realPlayerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
				m_tui_mang_score[i] = t_tuihui;
			}

			//有余分的给前面几局输分最多的玩家（包括留座起身玩家），当出现多个输分最多玩家，则余分给本局离挡家最远的玩家；
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;
			m_tui_mang_score[t_getYuMangPos] += t_yushu;

			//设置获得余分芒果起身玩家位置
			if (t_getYuMangPos != CHEXUAN_INVALID_POS && m_user_status[t_getYuMangPos] == 2 && t_yushu > 0)
			{
				m_last_get_yu_mang = t_getYuMangPos;
			}

			//清空芒果池
			m_mang_pool = 0;
		}

		//皮池剩余分数平分所有在局玩家（不包括留座起身玩家），有余分给离挡家最远的玩家
		/*
		if (m_pi_pool > 0)
		{
			//不能平分的余数
			Lint t_yushu = 0;

			//可以平分的退回分数
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_pi_pool % t_realPlayerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
				m_tui_pi_score[i] = t_tuihui;
			}

			//不能平分皮余数给离挡家最近的最小的玩家
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;
			m_tui_pi_score[t_getYuPiPos] += t_yushu;

			//清空皮池
			m_pi_pool = 0;
		}
		*/

		//计算每个玩家本局得的分
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;

			m_player_score[i] = m_total_bo_score[i] - m_total_bo_score_after_bobo[i];
		}
	}

	/* 记录本局信息到数据库 */
	void add_round_log(Lint* player_score, Lint win_pos)
	{
		Lint t_user_status[CHEXUAN_PLAYER_COUNT];
		memcpy(t_user_status, m_user_status, sizeof(t_user_status));
		if (m_last_get_yu_mang != CHEXUAN_INVALID_POS)
		{
			t_user_status[m_last_get_yu_mang] = 1;
		}

		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, player_score, t_user_status, win_pos, m_video.m_Id);
		}

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;

			m_accum_score[i] += player_score[i];
		}
	}

	//结算
	void finish_round(Lint jieSan = 0)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::finish_round() Error!!! m_desk is NULL or m_desk->m_vip is NULL");
			return;
		}

		//玩家解散房间，不发小结算消息
		if (0 != jieSan)
		{
			LLOG_ERROR("CheXuanGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

			//如果不清空，则解散时候要清空一下，并更新一下
			if (!m_isClearPool)
			{
				//不清空的玩法下解散
				jie_san_clear_all_score();

				//[0]类型  [1]退芒果  [2]退皮  [3][4]无效手牌  [5]剩余分数
				std::vector<Lint> t_handCards[CHEXUAN_PLAYER_COUNT];
				Lint t_optType = 100;
				for (int i = 0; i < m_player_count; ++i)
				{
					t_handCards[i].clear();
					if (m_user_status[i] != 1) continue;

					// 100:正常  101:休牌   102：揍牌   103：弃牌   104：三花   105：本局解散清空芒果和皮池
					t_optType = 105;

					//记录类型
					t_handCards[i].push_back(t_optType);

					//记录退的芒果、皮
					t_handCards[i].push_back(m_tui_mang_score[i]);
					t_handCards[i].push_back(m_tui_pi_score[i]);

					//记录手牌，不够4张的用0补齐
					for (int j = 0; j < CHEXUAN_HAND_CARDS_COUNT - 2; ++j)
					{
						t_handCards[i].push_back(0);
					}

					//剩余分数
					t_handCards[i].push_back(m_rest_can_start_bo_score[i] + m_total_bo_score[i]);
				}

				//保存手牌
				m_video.setHandCards(m_player_count, t_handCards);

				//保存回访
				m_video.addUserScore(m_accum_score);
				if (m_desk && m_desk->m_vip)
				{
					VideoSave();
				}

				add_round_log(m_player_score, CHEXUAN_INVALID_POS);
			}

			CheXuanS2CDrawResult jiesanRet;
			jiesanRet.m_isFinish = 1;
			jiesanRet.m_type = 1;
			jiesanRet.m_curCircle = m_round_offset + 1;
			jiesanRet.m_curMaxCircle = m_round_limit;
			jiesanRet.m_lastGetYuMang = m_last_get_yu_mang;
			memcpy(jiesanRet.m_score, m_player_score, sizeof(jiesanRet.m_score));
			memcpy(jiesanRet.m_totleScore, m_accum_score, sizeof(m_accum_score));
			memcpy(jiesanRet.m_curBoBoScore, m_total_bo_score, sizeof(jiesanRet.m_curBoBoScore));
			memcpy(jiesanRet.m_userStatus, m_user_status, sizeof(jiesanRet.m_userStatus));
			memcpy(jiesanRet.m_playerPiScore, m_tui_pi_score, sizeof(jiesanRet.m_playerPiScore));
			memcpy(jiesanRet.m_playerMangScore, m_tui_mang_score, sizeof(jiesanRet.m_playerMangScore));
			memcpy(jiesanRet.m_handCardsCount, m_hand_cards_count, sizeof(jiesanRet.m_handCardsCount));
			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
			{
				memcpy(jiesanRet.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);
				if (m_desk->m_user[i])
				{
					jiesanRet.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
					jiesanRet.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
					jiesanRet.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
				}
			}
			notify_desk(jiesanRet);
			notify_lookon(jiesanRet);

			LLOG_ERROR("Game Jie San Over... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

			set_desk_state(DESK_WAIT);

			//结束游戏
			if (m_desk) m_desk->HanderGameOver(jieSan);
			return;
		}

		//算分
		Lint t_maxWinnerScore = 0;
		Lint t_maxWinnerPos = CHEXUAN_INVALID_POS;
		calc_score(m_finish_round_type, &t_maxWinnerScore, &t_maxWinnerPos);

		//计算龙心肺（自己输的下注分数）
		Lint t_longXinFeiScore[CHEXUAN_PLAYER_COUNT];
		memset(t_longXinFeiScore, 0, sizeof(t_longXinFeiScore));

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			t_longXinFeiScore[i] = m_player_score[i] - m_player_mang_score[i] - m_player_pi_score[i];
		}

		//计算输、赢、平的标记
		Lint t_notAllowMangScore = 0;
		Lint t_loseOrWin[CHEXUAN_PLAYER_COUNT];		// 0:默认状态  -1：再来   1：胜利   2：平局
		memset(t_loseOrWin, 0, sizeof(t_loseOrWin));

		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_notAllowMangScore = 0;
			if (m_user_status[i] != 1) continue;

			t_notAllowMangScore = m_player_score[i] - m_player_mang_score[i];

			if (t_notAllowMangScore > 0) t_loseOrWin[i] = 1;
			else if (t_notAllowMangScore < 0) t_loseOrWin[i] = -1;
			else t_loseOrWin[i] = 2;
		}

		std::vector<Lint> t_handCards[CHEXUAN_PLAYER_COUNT];
		Lint t_optType = 100;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			// 100:正常  101:休牌   102：揍牌   103：弃牌   104：三花   105：本局解散清空芒果和皮池   106：揍芒(没选择揍芒)
			t_optType = 100;

			//修过河，都显示休牌
			if (m_finish_round_type == 1 || m_finish_round_type == 2) t_optType = 101;
			//揍牌
			else if (m_finish_round_type == 4 && m_qipai_status[i] == 0) t_optType = 102;
			//揍牌(没选揍芒)
			else if (m_finish_round_type == 3 && m_qipai_status[i] == 0) t_optType = 106;
			//弃牌
			else if (m_qipai_status[i] == 1) t_optType = 103;
			//三花
			else if (m_qipai_status[i] == 2) t_optType = 104;
			//正常
			else t_optType = 100;

			//记录类型
			t_handCards[i].push_back(t_optType);

			//记录手牌，不够4张的用0补齐
			for (int j = 0; j < m_hand_cards_count[i]; ++j)
			{
				t_handCards[i].push_back(m_hand_cards[i][j]);
			}
			for (int j = 0; j < CHEXUAN_HAND_CARDS_COUNT - m_hand_cards_count[i]; ++j)
			{
				t_handCards[i].push_back(0);
			}

			//剩余分数
			t_handCards[i].push_back(m_rest_can_start_bo_score[i] + m_total_bo_score[i]);
		}

		//保存手牌
		m_video.setHandCards(m_player_count, t_handCards);

		//保存回访
		m_video.addUserScore(m_accum_score);
		if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
		{
			VideoSave();
		}

		//判断下一局是否需要起身离座
		Lint t_standUpStatus[CHEXUAN_PLAYER_COUNT];
		memset(t_standUpStatus, 0, sizeof(t_standUpStatus));
		Lint t_needAddMangScore = 0;

		//通知客户端需要点击下一局的玩家的状态
		memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
		for (int i = 0; i < m_player_count; ++i)
		{
			m_desk->m_handlerPlayerStatus[i] = 0;
			if (m_user_status[i] == 0) continue;

			//计算需要多起的钵钵分（芒果、芒注、第一次下注）
			if (GetRealNextPos(m_dang_player_pos) == i)
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
			}
			else
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
			}

			//检查玩家能量值是否低于预警值
			cheack_player_coins_warn(i);

			//设置玩家是否需要手手芒的标志
			//m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

			t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];

			/*
			//玩家手里剩余的钵钵数 + 可起最大钵钵数 < 需要下芒果的数量
			if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore || m_user_status[i] == 2)
			{
				t_standUpStatus[i] = 1;
				//m_user_status[i] = 2;
				//m_desk->m_handlerPlayerStatus[i] = 2;

				//钵钵分不够,已经站起
				m_player_stand_up[i] = 2;
				m_desk->m_standUpPlayerSeatDown[i] = 2;

				LLOG_ERROR("CheXuanGameHandler::finish_round() Run..., Player has no enought bo score, So stand up, deskId = [%d], pos = [%d], needAddMang=[%d], totalBoScore=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, t_needAddMangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i]);
			}
			else
			{
				m_desk->m_handlerPlayerStatus[i] = 1;
			}
			*/
			if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore ||		//分不够下局下芒果的
				m_user_status[i] == 2 ||														//已经是牌局中留座观战的
				m_player_stand_up[i] == 1 ||													//本局已经申请站起的
				m_player_stand_up[i] == 2 ||													//已经站起的玩家的
				m_player_stand_up[i] == 3
				)
			{
				t_standUpStatus[i] = 1;
				//m_user_status[i] = 2;
			}
		}

		//增加局数
		increase_round();

		add_round_log(m_player_score, t_maxWinnerPos);

		//记录上一局皮池和芒果
		m_per_pi_pool = m_pi_pool;
		m_per_mang_pool = m_mang_pool;


		//暗牌观战
		if (m_isLookOnDarkResult)
		{
			//弃牌不显示牌
			if (m_isQiPaiNotShowCards)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					CheXuanS2CDrawResult rest;
					rest.m_nextRoundTime = m_def_time_finish;
					rest.m_firstSendCardsPos = m_frist_send_card;
					rest.m_dangPlayerPos = m_dang_player_pos;
					rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
					rest.m_curCircle = m_round_offset;
					rest.m_curMaxCircle = m_round_limit;
					rest.m_piPool = m_pi_pool;
					rest.m_mangPool = m_mang_pool;
					rest.m_state = m_finish_round_type;
					rest.m_isFinish = 0;
					rest.m_lastGetYuMang = m_last_get_yu_mang;
					memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
					memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
					memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
					memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
					memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
					memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
					memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
					memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
					memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
					memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
					memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
					memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));

					for (Lint j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] == 0) continue;

						if (m_desk && m_desk->m_user[j])
						{
							rest.m_userIds[j] = m_desk->m_user[j]->GetUserDataId();
							rest.m_userNames[j] = m_desk->m_user[j]->GetUserData().m_nike;
							rest.m_headUrl[j] = m_desk->m_user[j]->GetUserData().m_headImageUrl;
						}

						if (m_qipai_status[j] == 0 || m_qipai_status[j] == 2 || i == j)
						{
							memcpy(rest.m_handCards[j], m_hand_cards[j], sizeof(Lint) * m_hand_cards_count[j]);
						}
						else
						{
							for (Lint k = 0; k < m_hand_cards_count[j]; ++k)
							{
								if (k == 0 || k == 1)
								{
									rest.m_handCards[j][k] = 0;
								}
								else
								{
									rest.m_handCards[j][k] = m_hand_cards[j][k];
								}
							}
						}
					}
					
					notify_user(rest, i);
				}
			}
			else
			{
				CheXuanS2CDrawResult rest;
				rest.m_nextRoundTime = m_def_time_finish;
				rest.m_firstSendCardsPos = m_frist_send_card;
				rest.m_dangPlayerPos = m_dang_player_pos;
				rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
				rest.m_curCircle = m_round_offset;
				rest.m_curMaxCircle = m_round_limit;
				rest.m_piPool = m_pi_pool;
				rest.m_mangPool = m_mang_pool;
				rest.m_state = m_finish_round_type;
				rest.m_isFinish = 0;
				rest.m_lastGetYuMang = m_last_get_yu_mang;
				memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
				memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
				memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
				memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
				memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
				memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
				memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
				memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
				memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
				memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
				memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
				memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					memcpy(rest.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);

					if (m_desk && m_desk->m_user[i])
					{
						rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
						rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
						rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
					}
				}
				notify_desk_playing_user(rest);
				notify_desk_seat_no_playing_user(rest);
			}

			//广播观战玩家暗牌
			CheXuanS2CDrawResult gz_rest;
			gz_rest.m_nextRoundTime = m_def_time_finish;
			gz_rest.m_firstSendCardsPos = m_frist_send_card;
			gz_rest.m_dangPlayerPos = m_dang_player_pos;
			gz_rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
			gz_rest.m_curCircle = m_round_offset;
			gz_rest.m_curMaxCircle = m_round_limit;
			gz_rest.m_piPool = m_pi_pool;
			gz_rest.m_mangPool = m_mang_pool;
			gz_rest.m_state = m_finish_round_type;
			gz_rest.m_isFinish = 0;
			gz_rest.m_lastGetYuMang = m_last_get_yu_mang;
			memcpy(gz_rest.m_userStatus, m_user_status, sizeof(gz_rest.m_userStatus));
			memcpy(gz_rest.m_qiPaiStatus, m_qipai_status, sizeof(gz_rest.m_qiPaiStatus));
			memcpy(gz_rest.m_standUpStatus, t_standUpStatus, sizeof(gz_rest.m_standUpStatus));
			memcpy(gz_rest.m_handCardsCount, m_hand_cards_count, sizeof(gz_rest.m_handCardsCount));
			memcpy(gz_rest.m_playerAddScore, t_longXinFeiScore, sizeof(gz_rest.m_playerAddScore));
			memcpy(gz_rest.m_playerPiScore, m_player_pi_score, sizeof(gz_rest.m_playerPiScore));
			memcpy(gz_rest.m_playerMangScore, m_player_mang_score, sizeof(gz_rest.m_playerMangScore));
			memcpy(gz_rest.m_curBoBoScore, m_total_bo_score, sizeof(gz_rest.m_curBoBoScore));
			memcpy(gz_rest.m_score, m_player_score, sizeof(gz_rest.m_score));
			memcpy(gz_rest.m_totleScore, m_accum_score, sizeof(gz_rest.m_totleScore));
			memcpy(gz_rest.m_lossOrWin, t_loseOrWin, sizeof(gz_rest.m_lossOrWin));
			
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 0) continue;

				if (m_desk && m_desk->m_user[i])
				{
					gz_rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
					gz_rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
					gz_rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
				}
			}

			notify_lookon(gz_rest);

			if (m_isQiPaiNotShowCards)
			{
				notify_desk_seat_no_playing_user(gz_rest);
			}
		}
		//正常观战模式
		else
		{
			//弃牌不显示牌
			if (m_isQiPaiNotShowCards)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					CheXuanS2CDrawResult rest;
					rest.m_nextRoundTime = m_def_time_finish;
					rest.m_firstSendCardsPos = m_frist_send_card;
					rest.m_dangPlayerPos = m_dang_player_pos;
					rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
					rest.m_curCircle = m_round_offset;
					rest.m_curMaxCircle = m_round_limit;
					rest.m_piPool = m_pi_pool;
					rest.m_mangPool = m_mang_pool;
					rest.m_state = m_finish_round_type;
					rest.m_isFinish = 0;
					rest.m_lastGetYuMang = m_last_get_yu_mang;
					memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
					memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
					memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
					memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
					memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
					memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
					memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
					memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
					memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
					memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
					memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
					memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));

					for (int j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] == 0) continue;

						if (m_desk && m_desk->m_user[j])
						{
							rest.m_userIds[j] = m_desk->m_user[j]->GetUserDataId();
							rest.m_userNames[j] = m_desk->m_user[j]->GetUserData().m_nike;
							rest.m_headUrl[j] = m_desk->m_user[j]->GetUserData().m_headImageUrl;
						}

						if (m_qipai_status[j] == 0 || m_qipai_status[j] == 2 || i == j)
						{
							memcpy(rest.m_handCards[j], m_hand_cards[j], sizeof(Lint) * m_hand_cards_count[j]);
						}
						else
						{
							for (Lint k = 0; k < m_hand_cards_count[j]; ++k)
							{
								if (k == 0 || k == 1)
								{
									rest.m_handCards[j][k] = 0;
								}
								else
								{
									rest.m_handCards[j][k] = m_hand_cards[j][k];
								}
							}
						}
					}
					notify_user(rest, i);
				}

				//观战玩家
				CheXuanS2CDrawResult gz_rest;
				gz_rest.m_nextRoundTime = m_def_time_finish;
				gz_rest.m_firstSendCardsPos = m_frist_send_card;
				gz_rest.m_dangPlayerPos = m_dang_player_pos;
				gz_rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
				gz_rest.m_curCircle = m_round_offset;
				gz_rest.m_curMaxCircle = m_round_limit;
				gz_rest.m_piPool = m_pi_pool;
				gz_rest.m_mangPool = m_mang_pool;
				gz_rest.m_state = m_finish_round_type;
				gz_rest.m_isFinish = 0;
				gz_rest.m_lastGetYuMang = m_last_get_yu_mang;
				memcpy(gz_rest.m_userStatus, m_user_status, sizeof(gz_rest.m_userStatus));
				memcpy(gz_rest.m_qiPaiStatus, m_qipai_status, sizeof(gz_rest.m_qiPaiStatus));
				memcpy(gz_rest.m_standUpStatus, t_standUpStatus, sizeof(gz_rest.m_standUpStatus));
				memcpy(gz_rest.m_handCardsCount, m_hand_cards_count, sizeof(gz_rest.m_handCardsCount));
				memcpy(gz_rest.m_playerAddScore, t_longXinFeiScore, sizeof(gz_rest.m_playerAddScore));
				memcpy(gz_rest.m_playerPiScore, m_player_pi_score, sizeof(gz_rest.m_playerPiScore));
				memcpy(gz_rest.m_playerMangScore, m_player_mang_score, sizeof(gz_rest.m_playerMangScore));
				memcpy(gz_rest.m_curBoBoScore, m_total_bo_score, sizeof(gz_rest.m_curBoBoScore));
				memcpy(gz_rest.m_score, m_player_score, sizeof(gz_rest.m_score));
				memcpy(gz_rest.m_totleScore, m_accum_score, sizeof(gz_rest.m_totleScore));
				memcpy(gz_rest.m_cardsType, m_hand_cards_type, sizeof(gz_rest.m_cardsType));
				memcpy(gz_rest.m_lossOrWin, t_loseOrWin, sizeof(gz_rest.m_lossOrWin));

				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					if (m_desk && m_desk->m_user[i])
					{
						gz_rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
						gz_rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
						gz_rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
					}

					if (m_qipai_status[i] == 0 || m_qipai_status[i] == 2)
					{
						memcpy(gz_rest.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);
					}
					else
					{
						for (Lint k = 0; k < m_hand_cards_count[i]; ++k)
						{
							if (k == 0 || k == 1)
							{
								gz_rest.m_handCards[i][k] = 0;
							}
							else
							{
								gz_rest.m_handCards[i][k] = m_hand_cards[i][k];
							}
						}
					}
				}

				//广播观战玩家
				notify_lookon(gz_rest);

				//广播会长观战
				//notify_club_ower_lookon(gz_rest);
			}
			else
			{
				CheXuanS2CDrawResult rest;
				rest.m_nextRoundTime = m_def_time_finish;
				rest.m_firstSendCardsPos = m_frist_send_card;
				rest.m_dangPlayerPos = m_dang_player_pos;
				rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
				rest.m_curCircle = m_round_offset;
				rest.m_curMaxCircle = m_round_limit;
				rest.m_piPool = m_pi_pool;
				rest.m_mangPool = m_mang_pool;
				rest.m_state = m_finish_round_type;
				rest.m_isFinish = 0;
				rest.m_lastGetYuMang = m_last_get_yu_mang;
				memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
				memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
				memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
				memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
				memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
				memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
				memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
				memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
				memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
				memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
				memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
				memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));

				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					memcpy(rest.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);

					if (m_desk && m_desk->m_user[i])
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
				//notify_club_ower_lookon(rest);
			}
		}
		
		set_desk_state(DESK_WAIT);

		m_desk->HanderGameOver(1);


		//金币场下一局游戏有谁可以参加
		if (m_desk && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
		{
			memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
			for (int i = 0; i < m_player_count; ++i)
			{
				m_desk->m_handlerPlayerStatus[i] = 0;

				if (m_user_status[i] == 0) continue;

				//设置玩家是否需要手手芒的标志
				//m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

				//计算需要多起的钵钵分（芒果、芒注、第一次下注）
				if (GetNextPos(m_dang_player_pos) == i)
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
				}
				else
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
				}

				t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];

				//玩家手里剩余的钵钵数 + 可起最大钵钵数 < 需要下芒果的数量
				if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
				{
					m_user_status[i] = 2;
					//设置desk上的可以进行下一局的玩家标记
					m_desk->m_handlerPlayerStatus[i] = 2;
					//该玩家能量值不足
					m_desk->m_standUpPlayerSeatDown[i] = 2;
				}
				else
				{
					m_desk->m_handlerPlayerStatus[i] = 1;
				}

				LLOG_ERROR("CheXuanGameHandler::finish_round() next round player status..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], mangSocre=[%d], totalBoScore=[%d], deskStandUp=[%d], deskPlayerStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore, m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i], m_desk->m_handlerPlayerStatus[i]);
			}
		}
		//非金币场，处理申请起身和坐下
		else
		{
			memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
			for (int i = 0; i < m_player_count; ++i)
			{
				m_desk->m_handlerPlayerStatus[i] = 0;

				if (m_user_status[i] == 0) continue;

				//计算需要多起的钵钵分（芒果、芒注、第一次下注）
				if (GetRealNextPos(m_dang_player_pos) == i)
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
				}
				else
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
				}

				//设置玩家是否需要手手芒的标志
				//m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

				t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];
			
				//玩家本局点击起身的
				if (m_player_stand_up[i] == 1)
				{
					//设置为已站起状态
					m_player_stand_up[i] = 2;

					//m_user_status[i] = 2;
					//设置desk上的可以进行下一局的玩家标记
					m_desk->m_handlerPlayerStatus[i] = 2;
					//该玩家能量值不足 / 玩家起身成功，可是随时再坐下
					m_desk->m_standUpPlayerSeatDown[i] = 3;
				}
				//玩家本局点击坐下的
				else if (m_player_stand_up[i] == 3)
				{
					//玩家手里剩余的钵钵数 + 可起最大钵钵数 < 需要下芒果的数量
					if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
					{
						//坐下失败
						m_player_stand_up[i] = 2;

						//m_user_status[i] = 2;
						//设置desk上的可以进行下一局的玩家标记
						m_desk->m_handlerPlayerStatus[i] = 2;
						//该玩家能量值不足
						m_desk->m_standUpPlayerSeatDown[i] = 2;
					}
					else
					{
						//坐下成功
						m_player_stand_up[i] = 0;

						//m_user_status[i] = 1;
						m_desk->m_handlerPlayerStatus[i] = 1;
						m_desk->m_standUpPlayerSeatDown[i] = 1;
					}
				}
				//本局未操作的
				else
				{
					//玩家手里剩余的钵钵数 + 可起最大钵钵数 < 需要下芒果的数量
					if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
					{
						//直接站起
						m_player_stand_up[i] = 2;

						//m_user_status[i] = 2;
						//设置desk上的可以进行下一局的玩家标记
						m_desk->m_handlerPlayerStatus[i] = 2;
						//该玩家能量值不足
						m_desk->m_standUpPlayerSeatDown[i] = 2;
					}
					else if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] > t_needAddMangScore &&
						(m_user_status[i] == 2 || m_player_stand_up[i] == 2))
					{
						//直接站起
						m_player_stand_up[i] = 2;

						//m_user_status[i] = 2;
						//设置desk上的可以进行下一局的玩家标记
						m_desk->m_handlerPlayerStatus[i] = 2;
						//该玩家能量值不足
						m_desk->m_standUpPlayerSeatDown[i] = 3;
					}
					else
					{
						m_desk->m_handlerPlayerStatus[i] = 1;
						//m_user_status[i] = 1;
					}
				}

				LLOG_ERROR("CheXuanGameHandler::finish_round() next round player status..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], mangSocre=[%d], totalBoScore=[%d], deskStandUp=[%d], deskPlayerStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore, m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i], m_desk->m_handlerPlayerStatus[i]);
			}
		}
	}

};

/* 扯炫游戏处理器 */
struct CheXuanGameHandler : CheXuanGameCore
{
	/* 构造函数 */
	CheXuanGameHandler()
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

	/* 广播给桌子里所有入座玩家消息 */
	void notify_desk(LMsg &msg) override
	{
		if (NULL == m_desk)
			return;
		m_desk->BoadCast(msg);
	}

	/* 广播给桌子里当前参与玩家 */
	void notify_desk_playing_user(LMsg &msg) override
	{
		if (!m_desk) return;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1 && m_desk->m_user[i])
			{
				m_desk->m_user[i]->Send(msg);
			}
		}
	}

	//广播给当前入座但是没有参与游戏玩家
	void notify_desk_seat_no_playing_user(LMsg &msg) override
	{
		if (!m_desk) return;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 2 && m_desk->m_user[i])
			{
				m_desk->m_user[i]->Send(msg);
			}
		}
		m_desk->MHBoadCastDeskSeatingUser(msg);
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

	//广播给动态加入玩家
	void notify_desk_seating_user(LMsg &msg)
	{
		if (NULL == m_desk)
		{
			return;
		}
		m_desk->MHBoadCastDeskSeatingUser(msg);
	}

	/* 广播俱乐部会长明牌观战 */
	/*void notify_club_ower_lookon(LMsg& msg) override
	{
	if (NULL == m_desk)
	{
	return;
	}
	if (m_desk->m_clubOwerLookOn == 1 && m_desk->m_clubOwerLookOnUser)
	{
	m_desk->MHBoadCastClubOwerLookOnUser(msg);
	}
	}*/

	/* 每局开始*/
	bool startup(Desk *desk) 
	{
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
		m_baseTimes = m_playtype.CheXuanBaseTimes();

		//小选项：底、皮  0：1 / 3     1：5 / 1(5 / 10)     2：5 / 2(5 / 20)
		m_diPi = m_playtype.CheXuanDiPi();

		//小选项：允许的最大钵数  0:500   1 : 1000   2 : 2000
		m_maxBoBo = m_playtype.CheXuanMaxBoBo();

		//小选项：是否开启专属防作弊
		m_playCount = m_playtype.CheXuanPlayerCount();

		//小选项：是否允许动态加入  true:允许   false:不允许
		m_isDynamicIn = m_playtype.CheXuanDynamicIn();

		//小选项：否允许揍芒
		m_isZouMang = m_playtype.CheXuanIsZouMang();

		//小选项：是否允许休芒
		m_isXiuMang = m_playtype.CheXuanIsXiuMang();

		//小选项：是否允许手手芒
		m_isShouMang = m_playtype.CheXuanIsShouShouMang();

		//小选项：是否允许打芒翻倍
		m_isDaMangFanBei = m_playtype.CheXuanIsLianXuDaMangFanBei();

		//小选项：是否允许是否翻倍封顶
		m_isFanBeiFengDing = m_playtype.CheXuanIsFanBeiFengDing();

		//小选项：是否允许每局清空皮、芒果池 0：不清空   1：清空
		m_isClearPool = m_playtype.CheXuanIsClearPool();

		//小选项：是否是观战玩家暗牌结算  0：正常   1：暗牌结算
		m_isLookOnDarkResult = m_playtype.CheXuanLookOnDarkResult();

		//小选项：是否为游戏开始后禁止进入房间  0：正常   1：开始后禁止加入房间
		m_isStartNotInRoom = m_playtype.CheXuanStartNotInRoom();

		//小选项：是否允许默分  false:不允许  true:允许
		m_isMoFen = m_playtype.CheXuanMoFen();

		//小选项：是否选择首加分大于芒果
		m_isFirstAddBigMang = m_playtype.IsFirstAddBigMang();

		//小选项：是否选择允许地九王
		m_isAllowDiJiu = m_playtype.IsAllowDiJiu();

		//小选项：是否使用急速模式
		m_isFastMode = m_playtype.IsFastMode();

		//小选项：是否不能看别人弃牌的牌
		m_isQiPaiNotShowCards = m_playtype.IsQiPaiNotShowCards();

		//小选项：是否开启输分龙心肺为最大分选项  false:不开启   true:开启
		m_isLongXinFeiMaxScore = m_playtype.IsLongXinFeiMaxScore();

		//最大可以起钵钵总数
		//Lint t_playerMaxBoBo = 500;

		//设置一些全局变量
		if (0 == m_diPi)		//1/3
		{
			m_scoreBase = 3;
			m_scoreTimes = 1;
			m_commonMangCount = 5;
			if (m_isShouMang) m_shouMangCount = 5;
			m_mangScore = 1;

			/*if (m_maxBoBo == 0) t_playerMaxBoBo = 500;
			else if (m_maxBoBo == 1) t_playerMaxBoBo = 1000;
			else if (m_maxBoBo == 2) t_playerMaxBoBo = 2000;*/
		}
		else if (1 == m_diPi)	//5/1
		{
			m_scoreBase = 10;
			m_scoreTimes = 5;
			m_commonMangCount = 10;
			if (m_isShouMang) m_shouMangCount = 10;
			m_mangScore = 5;

			/*if (m_maxBoBo == 0) t_playerMaxBoBo = 1000;
			else if (m_maxBoBo == 1) t_playerMaxBoBo = 2000;
			else if (m_maxBoBo == 2) t_playerMaxBoBo = 3000;*/
		}
		else if (2 == m_diPi)	//5/2
		{
			m_scoreBase = 20;
			m_scoreTimes = 5;
			m_commonMangCount = 20;
			if (m_isShouMang) m_shouMangCount = 20;
			m_mangScore = 5;

			/*if (m_maxBoBo == 0) t_playerMaxBoBo = 2000;
			else if (m_maxBoBo == 1) t_playerMaxBoBo = 4000;
			else if (m_maxBoBo == 2) t_playerMaxBoBo = 6000;*/
		}
		else if (3 == m_diPi)
		{
			m_scoreBase = 5;
			m_scoreTimes = 2;
			m_commonMangCount = 10;
			if (m_isShouMang) m_shouMangCount = 10;
			m_mangScore = 2;
		}

		//设置每个人可以起的钵钵数
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1)
			{
				//m_rest_can_start_bo_score[i] = m_desk->m_vip->m_coins[i];	//能量场赋值
			}
			else
			{
				m_rest_can_start_bo_score[i] = m_maxBoBo;
				m_record_bobo_score[i] = m_maxBoBo;
			}
		}
		//设置是否允许天九王
		if (m_isAllowDiJiu)
		{
			m_gamelogic.setDiJiu(m_isAllowDiJiu);
		}

		//设置各个阶段默认时间
		if (m_isFastMode)
		{
			m_def_time_qibobo = DEF_TIME_FAST_QIBOBO;					//起钵钵
			m_def_time_optscore = DEF_TIME_FAST_OPTSCORE;				//前两轮下注
			m_def_time_3rd_optscore = DEF_TIME_FAST_3RD_OPTSCORE;		//第三轮下注
			m_def_time_cuopai = DEF_TIME_FAST_CUOPAI;					//搓牌
			m_def_time_chepai = DEF_TIME_FAST_CHEPAI;					//扯牌
			m_def_time_finish = DEF_TIME_FAST_FINISH;					//自动下一局时间，desk上的时间要同步改
		}
		else
		{
			m_def_time_qibobo = DEF_TIME_QIBOBO;						//起钵钵
			m_def_time_optscore = DEF_TIME_OPTSCORE;					//前两轮下注
			m_def_time_3rd_optscore = DEF_TIME_3RD_OPTSCORE;			//第三轮下注
			m_def_time_cuopai = DEF_TIME_CUOPAI;						//搓牌
			m_def_time_chepai = DEF_TIME_CHEPAI;						//扯牌
			m_def_time_finish = DEF_TIME_FINISH;						//自动下一局时间，desk上的时间要同步改
		}
		//m_video.FillDeskBaseInfo(m_desk ? m_desk->GetDeskId() : 0, 0, m_round_limit, l_playtype);
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
		LLOG_DEBUG("CheXuanGameHandler::MHSetDeskPlay() Run... deskId=[%d], play_user_count=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("CheXuanGameHandler::MHSetDeskPlay() Error!!!!  m_desk or m_desk->m_vip is NULL");
			return;
		}

		m_player_count = play_user_count;

		//设置第一个进入房间的玩家为第一个发牌的玩家
		if (m_round_offset == 0)
		{
			Lint t_first_send_card = CHEXUAN_INVALID_POS;
			for (size_t i = 0; i < m_player_count; ++i)
			{
				if (m_desk->m_user[i] && m_desk->m_playStatus[i] == 1)
				{
					t_first_send_card = i;
					break;
				}

			}
			if (t_first_send_card == CHEXUAN_INVALID_POS)
			{
				return;
			}
			m_frist_send_card = t_first_send_card;
		}

		start_round(player_status);
	}

	/* 定时器 */
	void Tick(LTime& curr)
	{
		//桌子对象为空
		if (m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::Tick() Error... This is desk NULL... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		//房间解散中，定时器不做处理
		if (m_desk && m_desk->m_resetTime > 0 && m_desk->m_resetUserId > 0)
		{
			if (m_rest_remain_time == -1)
			{
				m_rest_remain_time = curr.Secs() - m_play_status_time.Secs();
			}
			//刷新记录时间为当前时间
			m_play_status_time.Now();

			LLOG_ERROR("CheXuanGameHandler::Tick() The desk is reseting... So tick do nothing...", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}
		//解散房间失败从新计算剩余时间
		if (m_rest_remain_time != -1)
		{
			m_play_status_time.SetSecs(m_play_status_time.Secs() - m_rest_remain_time);
			//重置解散距离定时器的时间
			m_rest_remain_time = -1;
		}

		LLOG_DEBUG("CheXuanGameHandler::Tick() Run... deskId=[%d], playStatus=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_play_status);

		//能量场玩家能量值低于预警值后每隔30s通知一下玩家
		if (m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 0) continue;
				else if (m_is_coins_low[i] != 1) continue;

				//每隔30s通知一下客户端
				if (curr.Secs() >= m_is_coins_low_time[i].Secs() + DEF_TIME_COINS_LOW)
				{
					//通知玩家能量值过低
					LMsgS2CWarnPointLow warnPoint;
					warnPoint.m_msgType = 0;
					if (m_desk->m_user[i])
					{
						warnPoint.m_userId = m_desk->m_user[i]->GetUserDataId();
						warnPoint.m_nike = m_desk->m_user[i]->m_userData.m_nike;
						warnPoint.m_point = m_rest_can_start_bo_score[i] + m_total_bo_score[i];
						warnPoint.m_warnPoint = m_desk->m_warnScore;
						m_desk->m_user[i]->Send(warnPoint);
					}

					//刷新定时器
					m_is_coins_low_time[i].Now();
				}
			}
		}

		switch (m_play_status)
		{
		case GAME_PLAY_QIBOBO:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_def_time_qibobo)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_user_status[i] != 1 || m_qibobo_status[i] != 0) continue;

					LLOG_ERROR("CheXuanGameHandler::Tick() Qi Bo Bo AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_QIBOBO time over!",
						m_desk ? m_desk->GetDeskId() : 0, i);
					if (m_is_need_add_bo[i] == 1)
					{
						on_event_player_select_bobo(i, m_min_qi_bo_score[i]);
					}
					else
					{
						on_event_player_select_bobo(i, 0);
					}
				}
			}

			break;
		}
		//下注阶段
		case GAME_PLAY_ADD_SCORE:
		{
			//添加自动操作
			if (m_playerAutoOpt[m_curPos] != 0 && m_user_status[m_curPos] == 1 && m_qipai_status[m_curPos] == 0)
			{
				LLOG_DEBUG("AutoPlay() Run... Current player auto play, deskId=[%d], curPos=[%d], autoPlayStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_curPos, m_playerAutoOpt[m_curPos]);
				//自动弃牌：如果能三花则三花，不能三花则弃牌
				if (m_playerAutoOpt[m_curPos] == 1)
				{
					//三花
					if (m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] == ADD_OPT_SANHUA)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_SANHUA, 0);
					}
					//弃牌
					else
					{
						Lint t_shuaiScore = 0;
						if (m_turn_count == 0 && m_total_add_score[m_curPos] == 0)
						{
							if (m_diPi == 1) t_shuaiScore = 5;
							else if (m_diPi == 2) t_shuaiScore = 5;
							else if (m_diPi == 3) t_shuaiScore = 2;
							else t_shuaiScore = 1;
						}

						if (t_shuaiScore > m_total_bo_score[m_curPos])
						{
							t_shuaiScore = m_total_bo_score[m_curPos];
						}

						on_event_player_select_opt(m_curPos, ADD_OPT_SHUAI, t_shuaiScore);
					}
				}
				//自动休或丢:能休则休  能三花则三花  最后弃牌
				else if (m_playerAutoOpt[m_curPos] == 2)
				{
					//休
					if (m_allow_opt_type[m_curPos][ADD_OPT_XIU] == ADD_OPT_XIU)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_XIU, 0);
					}
					//三花
					else if (m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] == ADD_OPT_SANHUA)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_SANHUA, 0);
					}
					//弃牌
					else
					{
						Lint t_shuaiScore = 0;
						if (m_turn_count == 0 && m_total_add_score[m_curPos] == 0)
						{
							if (m_diPi == 1) t_shuaiScore = 5;
							else if (m_diPi == 2) t_shuaiScore = 5;
							else if (m_diPi == 3) t_shuaiScore = 2;
							else t_shuaiScore = 1;
						}

						if (t_shuaiScore > m_total_bo_score[m_curPos])
						{
							t_shuaiScore = m_total_bo_score[m_curPos];
						}

						on_event_player_select_opt(m_curPos, ADD_OPT_SHUAI, t_shuaiScore);
					}
				}
				//自动瞧
				else if (m_playerAutoOpt[m_curPos] == 3)
				{
					//瞧
					if (m_allow_opt_type[m_curPos][ADD_OPT_FIRST_QIAO] == ADD_OPT_FIRST_QIAO)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
					}
					//跟
					else if (m_allow_opt_type[m_curPos][ADD_OPT_GEN] == ADD_OPT_GEN)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_GEN, m_allow_opt_min_score[m_curPos][ADD_OPT_GEN]);
					}
					//大
					else if (m_allow_opt_type[m_curPos][ADD_OPT_FA_ZHAO] == ADD_OPT_FA_ZHAO)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_FA_ZHAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FA_ZHAO]);
					}
					//敲
					else
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_QIAO_BO, m_allow_opt_min_score[m_curPos][ADD_OPT_QIAO_BO]);
					}
				}
			}
			//超时自动操作
			else
			{
				Lint t_defTime = (Lint)m_def_time_optscore;
				if (m_turn_count == 2)
				{
					t_defTime = (Lint)m_def_time_3rd_optscore;
				}
				else if (m_turn_count == 0 && m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID && m_curPos == m_turn_first_speak_pos)
				{
					t_defTime = (Lint)m_def_time_optscore + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
				}

				if (m_turn_count != 0 && m_after_turn_shuai_delay)
				{
					t_defTime += m_after_turn_shuai_delay;
				}
				
				if (curr.Secs() >= (m_play_status_time.Secs() + t_defTime) && m_curPos != CHEXUAN_INVALID_POS && m_turn_round_opt[m_curPos] == 1 && m_user_status[m_curPos] == 1)
				{
					LLOG_ERROR("CheXuanGameHandler::Tick() Add Score AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_ADD_SCORE time over!",
						m_desk ? m_desk->GetDeskId() : 0, m_curPos);

					//是否为第一轮挡家第一次操作
					//bool t_firstTurnSpeak = m_turn_count == 0 && m_curPos == m_turn_first_speak_pos && m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID;
					bool t_firstTurnSpeak = m_allow_opt_type[m_curPos][ADD_OPT_FIRST_QIAO] == ADD_OPT_FIRST_QIAO;

					//是否允许三花操作
					bool t_isCanSanHua = m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] == ADD_OPT_SANHUA;

					//挡家第一次操作
					if (t_firstTurnSpeak || (m_isMoFen && m_turn_count == 0 && m_curPos == m_dang_player_pos && m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID))
					{
						//选项不默分
						if (!m_isMoFen)
						{
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
						//选项默分，选择不看牌
						else if (m_isMoFen && m_selectMoFen == 0)
						{
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
						//选项默分，选择看牌
						else if (m_isMoFen && m_selectMoFen == 1)
						{
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
						//选项默分，没有操作 看牌/不看牌
						else if (m_isMoFen && m_selectMoFen == -1)
						{
							on_event_player_opt_mo_fen(m_dang_player_pos, 1);
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
					}
					else if (t_isCanSanHua)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_SANHUA, 0);
					}
					else
					{
						Lint t_shuaiScore = 0;
						//if (m_turn_count == 0 && m_turn_opt_score[0][m_curPos] == 0)
						if (m_turn_count == 0 && m_total_add_score[m_curPos] == 0)
						{
							if (m_diPi == 1) t_shuaiScore = 5;
							else if (m_diPi == 2) t_shuaiScore = 5;
							else if (m_diPi == 3) t_shuaiScore = 2;
							else t_shuaiScore = 1;
						}

						if (t_shuaiScore > m_total_bo_score[m_curPos])
						{
							t_shuaiScore = m_total_bo_score[m_curPos];
						}

						on_event_player_select_opt(m_curPos, ADD_OPT_SHUAI, t_shuaiScore);
					}
				}
			}
			break;
		}
		//搓牌阶段
		case GAME_PLAY_CUO_PAI:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_def_time_cuopai)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

					if (m_cuopai_status[i] == 0 && m_user_status[i] == 1 && m_qipai_status[i] == 0)
					{
						LLOG_ERROR("CheXuanGameHandler::Tick() Cuo Pai AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_CUO_PAI time over!",
							m_desk ? m_desk->GetDeskId() : 0, i);
						on_event_player_cuo_pai(i);
					}
				}
			}
			break;
		}

		//扯牌阶段
		case GAME_PLAY_CHE_PAI:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_def_time_chepai)
			{

				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

					if (m_chepai_status[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("CheXuanGameHandler::Tick() Che Pai AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_CHE_PAI time over!",
							m_desk ? m_desk->GetDeskId() : 0, i);
						on_event_player_che_pai(i, m_hand_cards[i], 4);
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}

	/* 处理玩家断线重连 */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("CheXuanGameHandler::OnUserReconnect() Error!!! pUser or m_desk Is NULL...");
			return;
		}

		Lint pos = GetUserPos(pUser);

		if (CHEXUAN_INVALID_POS == pos && !(m_desk->MHIsLookonUser(pUser) || m_desk->m_clubOwerLookOnUser == pUser))
		{
			LLOG_ERROR("CheXuanGameHandler::OnUserReconnect() Error!!! Reconnect Pos is invial... desk_id=[%d], userId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		//Desk 已经发送
		notify_desk_match_state();

		LLOG_ERROR("CheXuanGameHandler::OnUserReconnect() Run... deskId=[%d], userId=[%d], pos=[%d], curPos=[%d], zhuangPos=[%d], playStatus=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), pos, m_curPos, m_zhuangPos, m_play_status);
		LTime t_currTime;
		t_currTime.Now();

		CheXuanS2CReConnect reconn;
		reconn.m_pos = pos;
		reconn.m_curPos = m_curPos;
		reconn.m_dangPos = GetNextPos(m_frist_send_card);
		reconn.m_curStatus = m_play_status;
		reconn.m_turnCount = m_turn_count;
		reconn.m_firstSendCardPos = m_frist_send_card;
		reconn.m_turnFirstSpeakPos = m_turn_first_speak_pos;
		reconn.m_selectMoFen = m_selectMoFen;
		memcpy(reconn.m_userMustQiBoBo, m_is_need_add_bo, sizeof(reconn.m_userMustQiBoBo));
		memcpy(reconn.m_qiBoBoStatus, m_qibobo_status, sizeof(reconn.m_qiBoBoStatus));
		memcpy(reconn.m_userStatus, m_user_status, sizeof(reconn.m_userStatus));
		memcpy(reconn.m_qiPaiStatus, m_qipai_status, sizeof(reconn.m_qiPaiStatus));
		memcpy(reconn.m_xiuStatus, m_xiu_status, sizeof(reconn.m_xiuStatus));
		memcpy(reconn.m_qiaoStatus, m_qiao_status, sizeof(reconn.m_qiaoStatus));
		memcpy(reconn.m_cuoPaiStatus, m_cuopai_status, sizeof(reconn.m_cuoPaiStatus));
		memcpy(reconn.m_chePaiStatus, m_chepai_status, sizeof(reconn.m_chePaiStatus));
		memcpy(reconn.m_playerLastOpt, m_player_last_opt, sizeof(reconn.m_playerLastOpt));
		reconn.m_piPool = m_pi_pool;
		reconn.m_mangPool = m_mang_pool;

		memcpy(reconn.m_lastBoScore, m_last_bo_score, sizeof(reconn.m_lastBoScore));
		memcpy(reconn.m_playerTotalBoScore, m_total_bo_score, sizeof(reconn.m_playerTotalBoScore));
		memcpy(reconn.m_playerTotalAddScore, m_total_add_score, sizeof(reconn.m_playerTotalAddScore));
		memcpy(reconn.m_playerTotalAddMang, m_total_add_mang_score, sizeof(reconn.m_playerTotalAddMang));
		memcpy(reconn.m_turnOptType, m_turn_opt_type[m_turn_count], sizeof(reconn.m_turnOptType));
		memcpy(reconn.m_turnOptScore, m_turn_opt_score[m_turn_count], sizeof(reconn.m_turnOptScore));
		memcpy(reconn.m_playerAutoOpt, m_playerAutoOpt, sizeof(reconn.m_playerAutoOpt));
		memcpy(reconn.m_playerStandUp, m_player_stand_up, sizeof(reconn.m_playerStandUp));

		memcpy(reconn.m_handCardsCount, m_hand_cards_count, sizeof(reconn.m_handCardsCount));

		//能量场中玩家能量值过低提示
		if (pos != CHEXUAN_INVALID_POS && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
		{
			if (m_user_status[pos] != 0 && m_is_coins_low[pos] == 1)
			{
				reconn.m_isCoinsLow = 1;
				//激活该玩家能量值过低的定时器
				m_is_coins_low_time[pos].Now();
			}

			if (m_user_status[pos] == 2 && m_rest_can_start_bo_score[pos] + m_total_bo_score[pos] > m_desk->m_inDeskMinCoins)
			{
				reconn.m_isStandShowSeat = 1;
			}
		}

		switch (m_play_status)
		{

		//起钵钵阶段
		case GAME_PLAY_QIBOBO:
		{
			//当前断线消息广播给断线用户
			pUser->Send(reconn);

			//如果该用户是当前参与游戏的玩家 && 需要起钵钵，则再发送起钵钵消息给该玩家
			if (pos != CHEXUAN_INVALID_POS && m_user_status[pos] == 1 && m_qibobo_status[pos] == 0)
			{
				//通知玩家起钵钵
				CheXuanS2CNotifyPlayerQiBoBo qiBoBo;
				qiBoBo.m_pos = pos;
				qiBoBo.m_restTime = m_def_time_qibobo - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				qiBoBo.m_dangPos = GetNextPos(m_frist_send_card);
				qiBoBo.m_piPool = m_pi_pool;
				qiBoBo.m_mangPool = m_mang_pool;
				qiBoBo.m_minSelectBoScore = m_min_qi_bo_score[pos];
				qiBoBo.m_maxSelectBoScore = m_max_qi_bo_score[pos];
				memcpy(qiBoBo.m_userStatus, m_user_status, sizeof(qiBoBo.m_userStatus));
				memcpy(qiBoBo.m_userMustQiBoBo, m_is_need_add_bo, sizeof(qiBoBo.m_userMustQiBoBo));
				memcpy(qiBoBo.m_playerStandUp, m_player_stand_up, sizeof(qiBoBo.m_playerStandUp));
				notify_user(qiBoBo, pos);
			}

			break;
		}

		//下芒果阶段
		case GAME_PLAY_ADD_MANGGUO:
		{
			pUser->Send(reconn);

			break;
		}

		//发牌阶段 && 下注
		case GAME_PLAY_SEND_CARD:
		case GAME_PLAY_ADD_SCORE:
		{
			Lint t_defTime = (Lint)m_def_time_optscore;
			if (m_turn_count == 2)
			{
				t_defTime = (Lint)m_def_time_3rd_optscore;
			}
			else if (pos == m_turn_first_speak_pos && m_turn_count == 0 && m_turn_opt_type[m_turn_count][pos] == ADD_OPT_INVALID)
			{
				t_defTime = (Lint)m_def_time_optscore + 2 + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
			}
			
			if(!m_isLookOnDarkResult || (pos != CHEXUAN_INVALID_POS && m_user_status[pos] == 1))
			{
				for (int i = 0; i < m_player_count; ++i)
				{
					for (int j = 0; j < m_hand_cards_count[i]; ++j)
					{
						if (i == pos)
						{
							reconn.m_handCards[i][j] = m_hand_cards[i][j];
						}
						else if (j == 2)
						{
							reconn.m_handCards[i][j] = m_hand_cards[i][j];
						}
						else if (j == 3)
						{
							reconn.m_handCards[i][j] = m_hand_cards[i][j];
						}
					}
				}
			}

			pUser->Send(reconn);

			if (!m_isMoFen || m_selectMoFen != -1)
			{
				//计算该玩家允许的操作
				Lint t_isMeiDaDong = 0;
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = t_defTime - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				notifyOpt.m_isMeiDaDong = t_isMeiDaDong;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				for (int i = 0; i < 10; ++i)
				{
					notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
					notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
					notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
				}
				pUser->Send(notifyOpt);
			}
			else
			{
				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = t_defTime - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				notifyOpt.m_isMoFen = 1;
				pUser->Send(notifyOpt);
			}

			break;
		}

		//搓牌阶段
		case GAME_PLAY_CUO_PAI:
		{
			if (!m_isLookOnDarkResult || (pos != CHEXUAN_INVALID_POS && m_user_status[pos] == 1))
			{
				//显示可以显示的牌
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

					reconn.m_handCards[i][2] = m_hand_cards[i][2];
					if (m_cuopai_status[i] == 1)
					{
						reconn.m_handCards[i][3] = m_hand_cards[i][3];
					}
				}
			}
			//如果该玩家是参与游戏玩家，将手牌发送给该玩家
			if (pos != CHEXUAN_INVALID_POS && m_qipai_status[pos] == 0)
			{
				memcpy(reconn.m_handCards[pos], m_hand_cards[pos], sizeof(Lint) * m_hand_cards_count[pos]);
			}
			pUser->Send(reconn);

			if (pos != CHEXUAN_INVALID_POS && m_cuopai_status[pos] == 0 && m_qipai_status[pos] == 0)
			{
				CheXuanS2CNotifyCuoPai cuoPai;
				cuoPai.m_restTime = m_def_time_cuopai - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				memcpy(cuoPai.m_userStatus, m_user_status, sizeof(cuoPai.m_userStatus));
				memcpy(cuoPai.m_cuoPaiStatus, m_cuopai_status, sizeof(cuoPai.m_cuoPaiStatus));
				memcpy(cuoPai.m_qiaoStatus, m_qiao_status, sizeof(cuoPai.m_qiaoStatus));
				memcpy(cuoPai.m_qiPaiStatus, m_qipai_status, sizeof(cuoPai.m_qiPaiStatus));

				pUser->Send(cuoPai);
			}
			break;
		}

		//扯牌阶段
		case GAME_PLAY_CHE_PAI:
		{
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				reconn.m_handCards[i][2] = m_hand_cards_backup[i][2];
				reconn.m_handCards[i][3] = m_hand_cards_backup[i][3];
				
				if (i == pos)
				{
					memcpy(reconn.m_handCards[pos], m_hand_cards[pos], sizeof(Lint) * m_hand_cards_count[pos]);
					memcpy(reconn.m_cardsType[pos], m_hand_cards_type[pos], sizeof(Lint) * 2);
				}
			}
			pUser->Send(reconn);

			if (pos != CHEXUAN_INVALID_POS && m_chepai_status[pos] == 0)
			{
				CheXuanS2CNotifyChePai chePai;
				chePai.m_restTime = m_def_time_chepai - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
				memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
				memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
				memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
				pUser->Send(chePai);
			}
			break;
		}

		//
		default:
			pUser->Send(reconn);
			break;
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

	/*
	C->S 扯炫：处理玩家起钵钵
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
	*/
	bool HandlerPlayerSelectBoBo(User* pUser, CheXuanC2SPlayerSelectBoBo* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_QIBOBO != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Play Status Error!!! Current Play Status is not GAME_PLAY_QIBOBO... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Error!!! Select BoBo Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Run... deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		//具体执行
		return on_event_player_select_bobo(GetUserPos(pUser), msg->m_selectBoScore);
	}

	/*
	扯炫：C->S 处理玩家选择操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
	*/
	bool HandlerPlayerSelectOpt(User* pUser, CheXuanC2SPlayerSelectOpt* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Play Status Error!!! Current Play Status is not GAME_PLAY_ADD_SCORE... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		//具体执行玩家下注
		return on_event_player_select_opt(GetUserPos(pUser), msg->m_selectType, msg->m_selectScore);
	}

	/*
	扯炫：C->S 处理玩家扯牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
	*/
	bool HandlerPlayerChePai(User* pUser, CheXuanC2SPlayerChePai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_CHE_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Play Status Error!!! Current Play Status is not GAME_PLAY_CHE_PAI... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		Lint t_handCards[4];
		t_handCards[0] = msg->m_handCard1;
		t_handCards[1] = msg->m_handCard2;
		t_handCards[2] = msg->m_handCard3;
		t_handCards[3] = msg->m_handCard4;

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Run..., deskId=[%d], userId=[%d], userPos=[%d], handCards=[%x, %x, %x, %x]",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser), t_handCards[0], t_handCards[1], t_handCards[2], t_handCards[3]);

		//具体执行
		return on_event_player_che_pai(GetUserPos(pUser), t_handCards, 4);
	}

	/*
	C->S 玩家搓牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
	*/
	bool HandlerPlayerCuoPai(User* pUser, CheXuanC2SPlayerCuoPai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_CUO_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Play Status Error!!! Current Play Status is not GAME_PLAY_CUO_PAI... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		return on_event_player_cuo_pai(msg->m_pos);
	}

	//获取牌局中玩家的钵钵数
	Lint GetPlayerBoBoScore(User* pUser)
	{
		if (!pUser)
		{
			return -100000;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos != CHEXUAN_INVALID_POS)
		{
			return m_total_bo_score[t_pos];
		}
		else
		{
			return -100000;
		}
	}

	//更新玩家可起钵钵数量
	bool UpdatePlayerAccumCoins(User* pUser, Lint changeCoins)
	{
		if (!pUser)
		{
			return false;
		}

		if (!(m_desk && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1))
		{
			return false;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos == CHEXUAN_INVALID_POS)
		{
			return false;
		}

		m_rest_can_start_bo_score[t_pos] += changeCoins;
		m_record_bobo_score[t_pos] += changeCoins;

		LMsgS2CStandPlayerShowSeat addCoins;
		addCoins.m_pos = t_pos;
		addCoins.m_addCoins = changeCoins;
		addCoins.m_isShowSeat = 0;
		notify_desk(addCoins);
		notify_lookon(addCoins);

		//增加能量能超过预警值，则不再预警客户端
		if (m_rest_can_start_bo_score[t_pos] + m_total_bo_score[t_pos] > m_desk->m_warnScore)
		{
			m_is_coins_low[t_pos] = 0;
		}

		//能量值不足的玩家，增加能量值超过最低进入房间值后，提示玩家可以入座
		if (m_user_status[t_pos] == 2 && m_rest_can_start_bo_score[t_pos] + m_total_bo_score[t_pos] > m_desk->m_inDeskMinCoins)
		{
			//给玩家发送加入游戏按钮
			LMsgS2CStandPlayerShowSeat showSeat;
			showSeat.m_pos = t_pos;
			showSeat.m_addCoins = 0;
			showSeat.m_isShowSeat = 1;
			if (m_desk&&m_desk->m_standUpPlayerSeatDown[t_pos] == 2)
			{
				pUser->Send(showSeat);
				m_desk->m_standUpPlayerSeatDown[t_pos] = 3;
			}
		}
		return true;
	}

	//玩家牌局内查看战绩
	bool HandlerPlayerRequestRecord(User* pUser, CheXuanC2SPlayerRequestRecord* msg)
	{
		if (!pUser || !m_desk)
		{
			return false;
		}

		CheXuanS2CPlayerSendRecord sendRecord;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0 || !m_desk->m_user[i]) continue;
			sendRecord.m_userId[i] = m_desk->m_user[i]->GetUserDataId();
			sendRecord.m_userHead[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
			sendRecord.m_userNike[i] = m_desk->m_user[i]->GetUserData().m_nike;
			sendRecord.m_nowScore[i] = m_rest_can_start_bo_score[i] + m_total_bo_score[i];
			sendRecord.m_inScore[i] = m_record_bobo_score[i];
			sendRecord.m_changeScore[i] = sendRecord.m_nowScore[i] - sendRecord.m_inScore[i];
		}

		//观战玩家信息
		if (!m_desk->m_desk_Lookon_user.empty())
		{
			auto lookIter = m_desk->m_desk_Lookon_user.begin();
			for (; lookIter != m_desk->m_desk_Lookon_user.end(); lookIter++)
			{
				if (*lookIter == NULL) continue;

				++sendRecord.m_lookOnCount;
				sendRecord.m_lookOnId.push_back((*lookIter)->GetUserDataId());
				sendRecord.m_lookOnNike.push_back((*lookIter)->GetUserData().m_nike);
				sendRecord.m_lookOnHead.push_back((*lookIter)->GetUserData().m_headImageUrl);
			}
		}

		if (m_desk->m_clubOwerLookOnUser)
		{
			++sendRecord.m_lookOnCount;
			sendRecord.m_lookOnId.push_back(m_desk->m_clubOwerLookOnUser->GetUserDataId());
			sendRecord.m_lookOnNike.push_back(m_desk->m_clubOwerLookOnUser->GetUserData().m_nike);
			sendRecord.m_lookOnHead.push_back(m_desk->m_clubOwerLookOnUser->GetUserData().m_headImageUrl);
		}
		pUser->Send(sendRecord);
		return true;
	}

	/*
	C->S 客户端操作默分
	MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
	*/
	bool HandlerPlayerOptMoFen(User* pUser, CheXuanC2SPlayerOptMoFen* msg) 
	{ 
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Play Status Error!!! Current Play Status is not GAME_PLAY_ADD_SCORE... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		return on_event_player_opt_mo_fen(GetUserPos(pUser),msg->m_optMo);
	}

	/*
	C->S 客户端选择自动操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
	*/
	bool HandlerPlayerSelectAutoOpt(User* pUser, CheXuanC2SPlayerSelectAutoOpt* msg) 
	{ 
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectAutoOpt() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectAutoOpt() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectAutoOpt() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));


		return on_event_player_select_auto_opt(GetUserPos(pUser), msg->m_autoOptType);
	}

	/*
	C->S 客户端选择站起
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
	*/
	bool HandlerPlayerStandOrSeat(User* pUser, CheXuanC2SPlayerSelectStandOrSeat* msg) 
	{ 
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		if (msg->m_standOrSeat == 0)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Error!!! This methord only stand, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));


		return on_event_player_stand_or_seat(GetUserPos(pUser), msg->m_standOrSeat);
	}

	//处理玩家坐下,desk上调用
	bool HanderPlayerSeat(User* pUser, Lint seat)
	{
		if (NULL == pUser || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Null Ptr Error!!! pUser or m_desk Is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		if (seat == 1)
		{
			LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Error!!! This methord only stand, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));


		return on_event_player_stand_or_seat(GetUserPos(pUser), seat);
	}
};


DECLARE_GAME_HANDLER_CREATOR(111, CheXuanGameHandler);

/*
//扯炫的处理器生产生类(上面的宏完成的同样的事情)
class GameHandlerCreator_CheXuan : public GameHandlerCreator
{
public:

	//构造函数：将扯炫处处理器生产类注册到工厂中
	GameHandlerCreator_CheXuan()
	{
		GameHandlerFactory::getInstance()->registerCreator(CheXuan, (GameHandlerCreator*)this);
	}

	//析构函数：将扯炫的处理器生产类从工厂中移除
	~GameHandlerCreator_CheXuan()
	{
		GameHandlerFactory::getInstance()->unregisterCreator(CheXuan);
	}

	//生产具体的扯炫处理器对象
	GameHandler* create()
	{
		return new CheXuanGameHandler();
	}
};

//静态变量，程序开始时自动加载，执行该类的自动注册到工厂中
static GameHandlerCreator_CheXuan sGameHandlerCreator_CheXuan;

*/