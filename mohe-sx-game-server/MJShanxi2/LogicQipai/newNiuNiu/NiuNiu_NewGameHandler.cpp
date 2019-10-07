#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "NiuNiu_NewGameLogic.h"
#include "LMsgL2C.h"

//测试版倒计时延长
#define DEBUG_DELAY_TIME 0

/*
*  新版牛牛Debug配牌使用
*  cbRandCard 用于输出的牌
*  m_desk Desk对象
*  conFileName 配牌文件
*  playerCount 玩家数量
*  handCardsCount 玩家手牌数量
*  cellPackCount 牌总数
*/
static bool debugSendCards_NiuNiu_New(BYTE* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
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

		memcpy(cbRandCard, t_cbRandCard, sizeof(BYTE) * cellPackCount);

		if (t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_cbRandCard);
		t_sendCount = NULL;

		if (fp) fclose(fp);
		fp = NULL;
		return true;
	}

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}


/* 牛牛大玩法标签（枚举）*/
enum GAME_MAIN_MODE
{
	MAIN_MODE_NIUNIU_SHANG_ZHUANG = 1,		//牛牛上庄
	MAIN_MODE_ZI_YOU_QIANG_ZHUANG = 2,		//自由抢庄
	MAIN_MODE_MING_PAI_QIANG_ZHUANG = 3,	//明牌抢庄
	MAIN_MODE_BA_REN_MING_PAI = 4,			//八人明牌
	MAIN_MODE_GONG_PAI_NIU_NIU = 5,			//公牌牛牛
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
	GAME_PLAY_SELECT_ZHUANG = 0,
	GAME_PLAY_MAI_MA = 1,
	GAME_PLAY_ADD_SCORE = 2,
	GAME_PLAY_SEND_CARD = 3,
	GAME_PLAY_CUO_PAI = 4,
	GAME_PLAY_END = 5,
};

/* 各个阶段的超时时间（枚举） */
enum PLAY_STATUS_DELAY_TIME
{
	DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU = 8 + DEBUG_DELAY_TIME,   //抢庄
	DELAY_TIME_ADD_SCORE = 10 + DEBUG_DELAY_TIME,                   //下注
	DELAY_TIME_OPEN_CARD = 15 + DEBUG_DELAY_TIME,                   //亮牌
};
/* 各个阶段超时时间（急速模式）*/
enum PLAY_STATUS_DELAY_TIME_FAST
{
	DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU_FAST = 5 + DEBUG_DELAY_TIME,   //抢庄
	DELAY_TIME_ADD_SCORE_FAST = 5 + DEBUG_DELAY_TIME,                   //下注
	DELAY_TIME_OPEN_CARD_FAST = 8 + DEBUG_DELAY_TIME,                   //亮牌
};

//玩家托管选项
struct TuoGuanInfo
{
	Lint m_pos;
	Lint m_tuoGuanStatus;		//托管状态 0：未托管  1：已托管
	Lint m_zhuangScore;			//抢庄分数 0：不抢   1:1倍    2:2倍    81、91、101：牛八1倍、牛九1倍、牛牛1倍 ...  82、92、102：2倍    83、93、103：3倍    84、94、104
	Lint m_addScore;			//下注分数 1:1分   2:2分    82：牛八以上2分    92：牛九以上2分   102：牛牛以上2分
	Lint m_tuiScore;			//推注分数 0：不推注   81：牛八以上推注     91：牛九以上推注    101：牛牛以上推注

	TuoGuanInfo()
	{
		memset(this, 0, sizeof(this));
		m_pos = NIUNIU_INVAILD_POS;
	}
};

//闲家赢牌信息结构
struct PlayerWinInfo
{
	Lint m_index;
	Lint m_oxNum;
	Lint m_oxTimes;
	Lint m_playerScore;

	PlayerWinInfo()
	{
		memset(this, 0, sizeof(this));
		m_index = NIUNIU_INVAILD_POS;
	}

	void operator= (PlayerWinInfo& info)
	{
		this->m_index = info.m_index;
		this->m_oxNum = info.m_oxNum;
		this->m_oxTimes = info.m_oxTimes;
		this->m_playerScore = info.m_playerScore;
	}
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
	* 函数名：   NiuNiu_NewGetMainMode()
	* 描述：     获取客户端选择的牛牛大玩法标签
	* 详细：     大玩法标签：1牛牛上庄、2自由抢庄、3名牌抢庄、4十人明牌、5公牌牛牛
	* 返回：     (Lint)客户端选择的牛牛大玩法标签
	********************************************************/
	Lint NiuNiu_NewGetMainMode() const;

	/******************************************************
	* 函数名：   NiuNiu_NewGetPlayerNum()
	* 描述：     获取客户端人数支持：6人、8人或10人
	* 返回：     (int)总人数
	********************************************************/
	Lint NiuNiu_NewGetPlayerNum() const;

	/******************************************************
	* 函数名：   NiuNiu_NewQiangZhuangTimes()
	* 描述：     获取牛牛抢庄倍率
	* 详细：     抢庄倍率：1-1倍、2-2倍、3-3倍、4-4倍
	* 返回：     (Lint)选择的抢庄倍率
	********************************************************/
	Lint NiuNiu_NewQiangZhuangTimes() const;

	/******************************************************
	* 函数名：   NiuNiu_NewAddScoreTimes()
	* 描述：     获取牛牛下注底分（）
	* 详细：     下注倍率：1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
	* 返回：     (Lint)选择的下注倍率
	********************************************************/
	Lint NiuNiu_NewAddScoreTimes() const;

	/******************************************************
	* 函数名：   NiuNiu_NewAutoStartGame()
	* 描述：     获取客户端选择自动开始
	* 详细：     自动开始：1-首先入座开始游戏，4-满4人开，5-满5人开，6-满6人开, 11-房主开始游戏
	* 返回：     (Lint)客户端选择的自动开始
	********************************************************/
	Lint NiuNiu_NewAutoStartGame() const;

	/******************************************************
	* 函数名：	NiuNiu_NewGetTuiZhu()
	* 描述：	牛牛推注选项
	* 详细：	推注倍数： 0：不推注，5：5倍推注，10：10倍推注，15:15倍推注，20:20倍推注
	* 返回：	(Lint)推注倍数
	*******************************************************/
	Lint NiuNiu_NewGetTuiZhu() const;

	/******************************************************
	* 函数名：   NiuNiu_NewGetDynamicIn()
	* 描述：     获取客户端是否允许动态加入选项
	* 详细：     动态加入：false-不予许，true-允许
	* 返回：     (bool)是否动态加入
	********************************************************/
	bool NiuNiu_NewGetDynamicIn() const;

	/******************************************************
	* 函数名：   NiuNiu_NewGetAutoPlay()
	* 描述：     获取客户端超时自动操作
	* 详细：     托管：false-不开启，true-开启
	* 返回：     (bool)客户端选择超时自动操作
	********************************************************/
	bool NiuNiu_NewGetAutoPlay() const;

	/******************************************************
	* 函数名：   NiuNiu_NewAllowLaiZi()
	* 描述：     获取客户是否允许王癞子
	* 详细：     王癞子：false-不允许，true-允许
	* 返回：     (bool)
	********************************************************/
	bool NiuNiu_NewAllowLaiZi() const;

	/******************************************************
	* 函数名：   NiuNiu_NewAllowMaiMa()
	* 描述：     获取客户是否允许买码
	* 详细：     王癞子：false-不允许，true-允许
	* 返回：     (bool)
	********************************************************/
	bool NiuNiu_NewAllowMaiMa() const;

	/******************************************************
	* 函数名：   NiuNiu_NewGetCuoPai()
	* 描述：     获取客户是否允许搓牌
	* 详细：     搓牌：false-不允许，true-允许
	* 返回：     (bool)
	********************************************************/
	bool NiuNiu_NewGetCuoPai() const;

	/******************************************************
	* 函数名：   NiuNiu_NewGetConstAddScore()
	* 描述：     获取客户是否下注限制
	* 详细：     下注限制：false-不限制，true-限制
	* 返回：     (bool)
	********************************************************/
	bool NiuNiu_NewGetConstAddScore() const;

	/******************************************************
	* 函数名：   NiuNiu_NewGetConstTuiZhu()
	* 描述：     获取客户是否推注限制
	* 详细：     推注限制：false-不限制，true-限制
	* 返回：     (bool)
	********************************************************/
	bool NiuNiu_NewGetConstTuiZhu() const;

	/******************************************************
	* 函数名：   NiuNiu_NewGetConstAddScoreDouble()
	* 描述：     获取客户是否允许加倍
	* 详细：     是否允许加倍：false-不允许，true-允许
	* 返回：     (bool)
	********************************************************/
	bool NiuNiu_NewGetConstAddScoreDouble() const;

	/******************************************************
	* 函数名：	NiuNiu_NewTypeTimes()
	* 描述：	牛牛牌型倍数
	* 详细：	1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1
	2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六-无牛x1
	3、...
	* 返回：	(int) 不选次选项返回：0， 选择此选项返回 1
	*******************************************************/
	Lint NiuNiu_NewTypeTimes() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeBiYi()
	* 描述：	牛牛特殊牌型：比翼牛4倍
	* 详细：	比翼牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeBiYi() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeShunZi()
	* 描述：	牛牛特殊牌型：顺子牛5倍
	* 详细：	顺子牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeShunZi() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeWuHua()
	* 描述：	牛牛特殊牌型：五花牛5倍
	* 详细：	五花牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeWuHua() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeTongHua()
	* 描述：	牛牛特殊牌型：同花牛6倍
	* 详细：	同花牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeTongHua() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeHuLu()
	* 描述：	牛牛特殊牌型：葫芦牛7倍
	* 详细：	葫芦牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeHuLu() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeZhaDan()
	* 描述：	牛牛特殊牌型：炸弹牛8倍
	* 详细：	炸弹牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeZhaDan() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeWuXiao()
	* 描述：	牛牛特殊牌型：五小牛9倍
	* 详细：	五小牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeWuXiao() const;

	/******************************************************
	* 函数名：	NiuNiu_NewAllowTypeKuaiLe()
	* 描述：	牛牛特殊牌型：快乐牛10倍
	* 详细：	快乐牛：false-不允许，true-允许
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeKuaiLe() const;

	/******************************************************
	* 函数名：	NiuNiu_NewFastStyle()
	* 描述：	牛牛急速模式
	* 详细：	急速模式：false-不开启，true-开启
	* 返回：	(bool)
	*******************************************************/
	bool NiuNiu_NewFastStyle() const;

};


/*****************************************************************************
* 函数名：   NiuNiu_NewGetMainMode()
* 描述：     获取客户端选择的牛牛大玩法标签
* 详细：     庄家规则：1牛牛上庄、2自由抢庄、3名牌抢庄、4十人明牌、5公牌牛牛
* 返回：     (Lint)客户端选择的牛牛大玩法标签
******************************************************************************/
Lint PlayType::NiuNiu_NewGetMainMode() const
{
	if (m_playtype.empty())
	{
		return 1;
	}

	if (m_playtype[0] <= 0 || m_playtype[0] > 5)
	{
		return 1;
	}

	return m_playtype[0];
}

/******************************************************
* 函数名：   NiuNiu_NewGetPlayerNum()
* 描述：     获取客户端人数支持：6人、8人或10人
* 返回：     (Lint)是否动态加入
********************************************************/
Lint PlayType::NiuNiu_NewGetPlayerNum() const
{
	if (m_playtype.size() < 2)
	{
		return 6;
	}
	if (m_playtype[1] != 6 || m_playtype[1] != 8 || m_playtype[1] != 10)
	{
		return 6;
	}
	return m_playtype[1];
}

/******************************************************
* 函数名：   NiuNiu_NewQiangZhuangTimes()
* 描述：     获取牛牛抢庄倍率
* 详细：     抢庄倍率：1-1倍、2-2倍、3-3倍、4-4倍
* 返回：     (Lint)选择的抢庄倍率
********************************************************/
Lint PlayType::NiuNiu_NewQiangZhuangTimes() const
{
	if (m_playtype.size() < 3)
	{
		return 1;
	}

	//明牌抢庄、八人明牌、公牌牛牛可以设置抢庄倍率
	if (m_playtype[0] == MAIN_MODE_MING_PAI_QIANG_ZHUANG
		|| m_playtype[0] == MAIN_MODE_GONG_PAI_NIU_NIU
		|| m_playtype[0] == MAIN_MODE_BA_REN_MING_PAI)
	{
		return m_playtype[2];
	}

	return 1;
}

/******************************************************
* 函数名：   NiuNiu_NewAddScoreTimes()
* 描述：     获取牛牛下注底分（）
* 详细：     下注倍率：1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
* 返回：     (Lint)选择的下注倍率
********************************************************/
Lint PlayType::NiuNiu_NewAddScoreTimes() const
{
	if (m_playtype.size() < 4)
	{
		return 1;
	}

	if (m_playtype[3] < 1 || m_playtype[3] > 5)
	{
		return 1;
	}

	return m_playtype[3];
}

/******************************************************
* 函数名：   NiuNiu_NewAutoStartGame()
* 描述：     获取客户端选择自动开始
* 详细：     自动开始：1-首先入座开始游戏，4-满4人开，5-满5人开，6-满6人开, 11-房主开始游戏
* 返回：     (Lint)客户端选择的自动开始
********************************************************/
Lint PlayType::NiuNiu_NewAutoStartGame() const
{
	if (m_playtype.size() < 5)
	{
		return 1;
	}

	if (m_playtype[4] == 1)
	{
		return 1;
	}

	if (m_playtype[4] >= 4 && m_playtype[4] <= m_playtype[1])
	{
		return m_playtype[4];
	}

	return 1;
}

/******************************************************
* 函数名：	NiuNiu_NewGetTuiZhu()
* 描述：	牛牛推注选项
* 详细：	推注倍数： 0：不推注，5：5倍推注，10：10倍推注，15:15倍推注，20:20倍推注
* 返回：	(Lint)推注倍数
*******************************************************/
Lint PlayType::NiuNiu_NewGetTuiZhu() const
{
	if (m_playtype.size() < 6)
	{
		return 0;
	}
	return m_playtype[5];
}

/******************************************************
* 函数名：   NiuNiu_NewGetDynamicIn()
* 描述：     获取客户端是否允许动态加入选项
* 详细：     暗抢庄：false-不予许，true-允许
* 返回：     (Lint)是否动态加入
********************************************************/
bool PlayType::NiuNiu_NewGetDynamicIn() const
{
	if (m_playtype.size() < 7)
	{
		return false;
	}
	return m_playtype[6] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiu_NewGetAutoPlay()
* 描述：     获取客户端超时自动操作
* 详细：     扫雷模式：false-不开启，true-开启
* 返回：     (bool)客户端选择超时自动操作
********************************************************/
bool PlayType::NiuNiu_NewGetAutoPlay() const
{
	if (m_playtype.size() < 8)
	{
		return false;
	}
	return m_playtype[7] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiu_NewAllowLaiZi()
* 描述：     获取客户是否允许王癞子
* 详细：     王癞子：false-不允许，true-允许
* 返回：     (bool)
********************************************************/
bool PlayType::NiuNiu_NewAllowLaiZi() const
{
	if (m_playtype.size() < 9)
	{
		return false;
	}
	return m_playtype[8] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiu_NewAllowMaiMa()
* 描述：     获取客户是否允许买码
* 详细：     王癞子：false-不允许，true-允许
* 返回：     (bool)
********************************************************/
bool PlayType::NiuNiu_NewAllowMaiMa() const
{
	if (m_playtype.size() < 10)
	{
		return false;
	}
	return m_playtype[9] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiu_NewGetCuoPai()
* 描述：     获取客户是否允许搓牌
* 详细：     搓牌：false-不允许，true-允许
* 返回：     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetCuoPai() const
{
	if (m_playtype.size() < 11)
	{
		return false;
	}
	return m_playtype[10] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiu_NewGetConstAddScore()
* 描述：     获取客户是否下注限制
* 详细：     下注限制：false-不限制，true-限制
* 返回：     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetConstAddScore() const
{
	if (m_playtype.size() < 12)
	{
		return false;
	}
	return m_playtype[11] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiu_NewGetConstTuiZhu()
* 描述：     获取客户是否推注限制
* 详细：     推注限制：false-不限制，true-限制
* 返回：     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetConstTuiZhu() const
{
	if (m_playtype.size() < 13)
	{
		return false;
	}
	return m_playtype[12] == 0 ? false : true;
}

/******************************************************
* 函数名：   NiuNiu_NewGetConstAddScoreDouble()
* 描述：     获取客户是否允许加倍
* 详细：     是否允许加倍：false-不允许，true-允许
* 返回：     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetConstAddScoreDouble() const
{
	if (m_playtype.size() < 14)
	{
		return false;
	}
	return m_playtype[13] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewTypeTimes()
* 描述：	牛牛牌型倍数
* 详细：	1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1
2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六-无牛x1
3、...
* 返回：	(int)
*******************************************************/
Lint PlayType::NiuNiu_NewTypeTimes() const
{
	if (m_playtype.size() < 15)
	{
		return 1;
	}
	if (m_playtype[14] < 1 || m_playtype[14] > 2)
	{
		return 1;
	}

	return m_playtype[14];
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeBiYi()
* 描述：	牛牛特殊牌型：比翼牛4倍
* 详细：	比翼牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeBiYi() const
{
	if (m_playtype.size() < 16)
	{
		return false;
	}
	return m_playtype[15] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeShunZi()
* 描述：	牛牛特殊牌型：顺子牛5倍
* 详细：	顺子牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeShunZi() const
{
	if (m_playtype.size() < 17)
	{
		return false;
	}
	return m_playtype[16] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeWuHua()
* 描述：	牛牛特殊牌型：五花牛5倍
* 详细：	五花牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeWuHua() const
{
	if (m_playtype.size() < 18)
	{
		return false;
	}
	return m_playtype[17] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeTongHua()
* 描述：	牛牛特殊牌型：同花牛6倍
* 详细：	同花牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeTongHua() const
{
	if (m_playtype.size() < 19)
	{
		return false;
	}
	return m_playtype[18] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeHuLu()
* 描述：	牛牛特殊牌型：葫芦牛7倍
* 详细：	葫芦牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeHuLu() const
{
	if (m_playtype.size() < 20)
	{
		return false;
	}
	return m_playtype[19] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeZhaDan()
* 描述：	牛牛特殊牌型：炸弹牛8倍
* 详细：	炸弹牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeZhaDan() const
{
	if (m_playtype.size() < 21)
	{
		return false;
	}
	return m_playtype[20] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeWuXiaoNiu()
* 描述：	牛牛特殊牌型：五小牛9倍
* 详细：	五小牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeWuXiao() const
{
	if (m_playtype.size() < 22)
	{
		return false;
	}
	return m_playtype[21] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewAllowTypeKuaiLe()
* 描述：	牛牛特殊牌型：快乐牛10倍
* 详细：	快乐牛：false-不允许，true-允许
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeKuaiLe() const
{
	if (m_playtype.size() < 23)
	{
		return false;
	}
	return m_playtype[22] == 0 ? false : true;
}

/******************************************************
* 函数名：	NiuNiu_NewFastStyle()
* 描述：	牛牛急速模式
* 详细：	急速模式：false-不开启，true-开启
* 返回：	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewFastStyle() const
{
	if (m_playtype.size() < 24)
	{
		return false;
	}
	return m_playtype[23] == 0 ? false : true;
}



/* 牛牛每局不会初始化字段（结构体）*/
struct NiuNiu_NewRoundState__c_part
{
	Lint		 m_curPos;						                    // 当前操作玩家
	Lint         m_play_status;                                     // 牌局状态
	bool         m_tick_flag;                                       // 定时器开关
	Lint		 m_player_tui_score[NIUNIU_PLAY_USER_COUNT][4];	// 玩家可以推注的分数：最多4个推注选项

	Lint         m_user_status[NIUNIU_PLAY_USER_COUNT];             // 用户状态：改位置上是否坐人了（实际有效位置）
	Lint         m_play_add_score[NIUNIU_PLAY_USER_COUNT];          // 玩家下注分数
	Lint		 m_noQiangZhuang[NIUNIU_PLAY_USER_COUNT];			//能量值不够不可以抢庄
	Lint         m_play_qiang_zhuang[NIUNIU_PLAY_USER_COUNT];       // 玩家抢庄分数： 默认：-1，无效位置未操作， 0，不抢庄， >0抢庄分数
	bool         m_isOpenCard[NIUNIU_PLAY_USER_COUNT];              // 玩家是否开牌：false-未开牌，true-已开牌,初始为：false
	BYTE         m_player_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];   //用户手牌	
	BYTE		 m_show_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];	 //玩家本局输赢分数
	Lint         m_player_score[NIUNIU_PLAY_USER_COUNT];            // 玩家本局得分
	Lint		 m_player_coins[NIUNIU_PLAY_USER_COUNT];			// 玩家本局实际输赢能量值
	Lint		 m_accum_coins[NIUNIU_PLAY_USER_COUNT];				//玩家能量值记录
	Lint         m_player_oxnum[NIUNIU_PLAY_USER_COUNT];            // 玩家牛数
	Lint         m_player_oxtimes[NIUNIU_PLAY_USER_COUNT];			//玩家牛牛倍数;
	Lint		 m_player_coins_status[NIUNIU_PLAY_USER_COUNT];		//玩家是能量值结算时是否够用  0：够用  1：不够用
																	//BYTE		 m_cbOxCard[NIUNIU_PLAY_USER_COUNT];				// 牛牛数据
	Lint		 m_player_maima[NIUNIU_PLAY_USER_COUNT][2];			// 玩家买码情况：0-买码分数，1买的玩家位置
	Lint		 m_canXiaMinZhu[NIUNIU_PLAY_USER_COUNT];			//下注限制：抢庄抢最大倍数没有抢到的玩家，下注的时候不能下最小分  0:可以下注最小分  1：不可以下注最小分
	Lint		 m_canAddScoreDouble[NIUNIU_PLAY_USER_COUNT];		//是否下注翻倍：手动参与抢庄且倍数高但没有抢到庄家的玩家下注时，可以使用加倍功能。  0：不能翻倍   1：可以翻倍
	Lint		 m_add_score_double_status[NIUNIU_PLAY_USER_COUNT]; //是否加倍状态： 0玩家没有加倍   1：玩家加倍
	BYTE		 m_gong_card;										//公牌
	Lint		 m_open_gong_pos;									//搓公牌的玩家位置
	Lstring		 m_cuo_pai;

	Lint		 m_real_player_count;								//本局玩家实际参与人数
	Lint		 m_qiang_zhuang_count;								//本局内抢庄玩家数量
	Lint		 m_next_round_cartoon_time;							//本局结束动画时间
	Lint		 m_add_score_delay_time;							//抢庄延时时间


	/* 清零每局字段 */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_open_gong_pos = NIUNIU_INVAILD_POS;

		//某些变量需要赋特殊初始值
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			m_noQiangZhuang[i] = 1;
			m_play_qiang_zhuang[i] = -1;
			m_player_oxnum[i] = -1;
			m_isOpenCard[i] = false;
			m_player_maima[i][0] = -1;
			m_player_maima[i][1] = NIUNIU_INVAILD_POS;
		}
	}
};

/* 牛牛每局会初始化字段（结构体）*/
struct NiuNiu_NewRoundState__cxx_part
{
	LTime m_play_status_time;
	/* 清零每局字段 */
	void clear_round()
	{
		this->~NiuNiu_NewRoundState__cxx_part();
		new (this) NiuNiu_NewRoundState__cxx_part;
	}
};

/* 牛牛每场不会初始化的字段（结构体）*/
struct NiuNiu_NewMatchState__c_part
{
	Desk*         m_desk;           // 桌子对象
	Lint          m_round_offset;   // 当前局数
	Lint          m_round_limit;    // 创建房间选择的最大局数
	Lint          m_accum_score[NIUNIU_PLAY_USER_COUNT];	//玩家总分记录
	
	Lint		  m_per_round_score[NIUNIU_PLAY_USER_COUNT];  //上一居的分数
	bool          m_dismissed;
	int           m_registered_game_type;
	int           m_player_count;   // 玩法核心代码所使用的玩家数量字段
	Lint          m_zhuangPos;      // 庄家位置
	Lint		  m_carToon;		// 是否播选庄的动画  0：不需要   1：庄家没有没有能量值播放随机选庄动画
	bool          m_isFirstFlag;    // 首局显示开始按钮,true代表首局
	bool		  m_canTuiZhu[NIUNIU_PLAY_USER_COUNT];  // 是否可以推注 0 不可以推注，1可以推注
	TuoGuanInfo   m_tuo_guan_info[NIUNIU_PLAY_USER_COUNT];  //玩家托管消息

	Lint		  m_qiangCount[NIUNIU_PLAY_USER_COUNT];		//玩家抢庄次数
	Lint		  m_zhuangCount[NIUNIU_PLAY_USER_COUNT];	//玩家坐庄次数
	Lint		  m_tuiCount[NIUNIU_PLAY_USER_COUNT];		//玩家推注次数

	//默认倒计时
	Lint		  m_time_qiang_zhuang;
	Lint		  m_time_add_score;
	Lint		  m_time_open_card;
	
	/////玩法小选项
	Lint          m_mainMode;				// 小选项[0]：大玩法标签：1牛牛上庄、2自由抢庄、3名牌抢庄、4十人明牌、5公牌牛牛
	Lint          m_playerNum;              // 小选项[1]：人数6人 8人 10人
	Lint          m_qiangZhuangTimes;       // 小选项[2]：抢庄倍率：1-1倍，2-2倍，3-3倍，4-4倍
	Lint          m_addScoreTimes;          // 小选项[3]：下注倍率：1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
	Lint		  m_autoStart;				// 小选项[4]：自动开始：1-满2人随时开始，4-满4人开，5-满5人开，6-满6人开，4-暗牌下注
	Lint		  m_tuiZhuTimes;		    // 小选项[5]：推注分数： 0：不推注，5：5倍推注，10：10倍推注，15:15倍推注，20:20倍推注
	bool          m_isDynamicIn;            // 小选项[6]：是否开局后禁止加入房间：false-开局后可以加入房间，true-开局后不可加入房间
	bool          m_isAutoPlay;             // 小选项[7]：是否允许托管：false-不允许，true-允许
	bool		  m_isLaiZi;				// 小选项[8]：是否允许癞子：false-不允许，true-允许
	bool		  m_isMaiMa;				// 小选项[9]：是否允许买码：false-不允许，true-允许
	bool		  m_isCuoPai;				// 小选项[10]：是否允许搓牌：false-不允许，true-允许
	bool		  m_isConstAddScore;		// 小选项[11]：是否下注限制：false-不限制，true-限制
	bool		  m_isConstTuiZhu;			// 小选项[12]：是否推注限制：false-不限制，true-限制
	bool		  m_isAddScoreDouble;		// 小选项[13]：是否下注加倍：false-不允许加倍，true-允许加倍
	Lint		  m_niuNiuTimes;			// 小选项[14]：1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1   2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六 - 无牛x1
	bool		  m_isBiYi;					// 小选项[15]：是否允许比翼牛：false-不允许，true-允许
	bool		  m_isShunZi;				// 小选项[16]：是否允许顺子牛：false-不允许，true-允许
	bool		  m_isWuHua;				// 小选项[17]：是否允许五花牛：false-不允许，true-允许
	bool		  m_isTongHua;				// 小选项[18]：是否允许同花牛：false-不允许，true-允许
	bool		  m_isHuLu;					// 小选项[19]：是否允许葫芦牛：false-不允许，true-允许
	bool		  m_isZhaDan;				// 小选项[20]：是否允许炸弹牛：false-不允许，true-允许
	bool		  m_isWuXiao;				// 小选项[21]：是否允许五小牛：false-不允许，true-允许
	bool		  m_isKuaiLe;				// 小选项[22]：是否允许快乐牛：false-不允许，true-允许
	bool		  m_fastStyle;				// 小选项[23]：是否是急速模式：false-普通模式， true-极速模式



	/* 清零结构体字段 */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_zhuangPos = NIUNIU_INVAILD_POS;
		m_isFirstFlag = true;
		m_qiangZhuangTimes = 1;  //默认抢庄倍率1倍
		m_playerNum = 6;
		m_niuNiuTimes = 1;
	}
};

/* 牛牛每场会初始化的字段（结构体）*/
struct NiuNiu_NewMatchState__cxx_part
{
	NNGameLogic    m_gamelogic;     // 游戏逻辑
	PlayType       m_playtype;	    // 桌子玩法小选项

									/* 清空每场结构体 */
	void clear_match()
	{
		this->~NiuNiu_NewMatchState__cxx_part();
		new (this) NiuNiu_NewMatchState__cxx_part;
	}
};

/*
*  牛牛每局所需要的所有字段（结构体）
*  继承 ：NiuNiuRoundState__c_pard, NiuNiuRoundState_cxx_part
*/
struct NiuNiu_NewRoundState : NiuNiu_NewRoundState__c_part, NiuNiu_NewRoundState__cxx_part
{
	void clear_round()
	{
		NiuNiu_NewRoundState__c_part::clear_round();
		NiuNiu_NewRoundState__cxx_part::clear_round();
	}
};

/*
*  牛牛每场所需要的所有字段（结构体）
*  继承：NiuNiuMatchState__c_pard, NiuNiuMatchState_cxx_pard
*/
struct NiuNiu_NewMatchState : NiuNiu_NewMatchState__c_part, NiuNiu_NewMatchState__cxx_part
{
	void clear_match()
	{
		NiuNiu_NewMatchState__c_part::clear_match();
		NiuNiu_NewMatchState__cxx_part::clear_match();
	}
};

/*
*  牛牛桌子状态（结构体）
*  继承：NiuNiuRoundState, NiuNiuMatchState
*/
struct NiuNiu_NewDeskState : NiuNiu_NewRoundState, NiuNiu_NewMatchState
{

	/* 清零每局数据 */
	void clear_round()
	{
		NiuNiu_NewRoundState::clear_round();
	}

	/* 清零每场数据*/
	void clear_match(int player_count)
	{
		NiuNiu_NewMatchState::clear_match();
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
		updateFirstFlag();

		if (play_status >= GAME_PLAY_SELECT_ZHUANG && play_status <= GAME_PLAY_END)
		{
			m_play_status = play_status;
		}
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
		if (pos >= m_player_count)
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
		for (Lint i = 0; (i < m_player_count) && (m_user_status[nextPos] != 1); i++)
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
		return 0 <= pos && pos < m_player_count;
	}

	void updateFirstFlag()
	{
		if (m_desk)
		{
			m_desk->m_finish_first_opt = 0;
		}
	}
};

/* 牛牛录像功能(结构体) */
struct NiuNiu_NewVideoSupport : NiuNiu_NewDeskState
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
};

/* 牛牛游戏具体处理逻辑（结构体）*/
struct NiuNiu_NewGameCore : GameHandler, NiuNiu_NewVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	/* 广播给所有入座参与游戏的玩家*/
	virtual void notify_seat_playing(LMsg& msg) {}

	/* 广播给所有入座观战的玩家 */
	virtual void notify_seat_lookon(LMsg& msg) {}

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
	void notify_zhuang(Lint select_zhuang_pos, Lint allPlayerNoQiang, Lint score_times = 1)
	{
		if (select_zhuang_pos < 0 || select_zhuang_pos >m_player_count)
		{
			LLOG_ERROR("desk_id=[%d]send_zhuang_cmd pos error!!!", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}
		if (select_zhuang_pos >= 0 &&
			select_zhuang_pos < m_player_count &&
			m_user_status[select_zhuang_pos] == 1)
		{
			m_zhuangPos = select_zhuang_pos;

			calc_tui_zhu_score();

			//回放记录公共数据
			m_video.setCommonInfo(gWork.GetCurTime().Secs(), m_round_offset, m_player_count, m_desk->m_vip->m_posUserId, m_zhuangPos);

			NiuNiuS2CStartGame send_start;
			send_start.m_remainTime = m_time_add_score;
			send_start.m_zhuangPos = select_zhuang_pos;
			send_start.m_scoreTimes = score_times;
			send_start.m_maiMa = (m_isMaiMa && calc_vaild_player_count() > 2) ? 1 : 0;
			send_start.m_isAllNoQiang = allPlayerNoQiang;
			//推注：玩家可推注状态
			memcpy(send_start.m_isCanTui, m_canTuiZhu, sizeof(send_start.m_isCanTui));
			memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
			memcpy(send_start.m_playerXiaScore, m_canXiaMinZhu, sizeof(send_start.m_playerXiaScore));
			memcpy(send_start.m_playerAddScoreDouble, m_canAddScoreDouble, sizeof(send_start.m_playerAddScoreDouble));
			memcpy(send_start.m_playerStatus, m_user_status, sizeof(m_user_status));
			memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
			if (m_mainMode != MAIN_MODE_NIUNIU_SHANG_ZHUANG && m_mainMode != MAIN_MODE_ZI_YOU_QIANG_ZHUANG)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1) continue;

					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; ++j)
					{
						send_start.m_playerHandCard[j] = m_player_hand_card[i][j];
					}
					notify_user(send_start, i);
				}
				notify_seat_lookon(send_start);
				//广播观战玩家：游戏开始
				notify_lookon(send_start);
			}
			else
			{
				notify_desk(send_start);
				//广播观战玩家：游戏开始
				notify_lookon(send_start);
			}

			//记录坐庄次数
			++m_zhuangCount[m_zhuangPos];
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	/* 计算该桌子上有多少人入座（有效位置）*/
	Lint calc_vaild_player_count()
	{
		Lint tmp_player_count = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] == 1)
			{
				++tmp_player_count;
			}
		}
		return tmp_player_count;
	}

	/* 确定最小的下注分数 */
	Lint min_add_score(Lint pos = NIUNIU_INVAILD_POS)
	{
		if (pos == NIUNIU_INVAILD_POS)
		{
			return 0;
		}
		// 下注限制 && 不可最小下注 && 选择加倍
		if (m_isConstAddScore && m_canXiaMinZhu[pos] == 1 && m_add_score_double_status[pos] == 1)
		{
			return 4 * m_addScoreTimes;
		}
		//下注限制 && 不可最小下注
		else if (m_isConstAddScore && m_canXiaMinZhu[pos] == 1)
		{
			return 2 * m_addScoreTimes;
		}
		//选择下注加倍
		else if (m_add_score_double_status[pos] == 1)
		{
			return 2 * m_addScoreTimes;
		}
		//正常下注
		else
		{
			return m_addScoreTimes;
		}
	}

	/* 判断看牌选庄玩法下是否所有玩家已经对选庄操作完成 */
	bool is_select_zhuang_over()
	{
		Lint userSelectZhuang = 0;
		for (Lint i = 0; i < m_player_count; i++)
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

	/* 判断是否所有玩家买码完成 */
	bool is_mai_ma_over()
	{
		Lint userMaiMaCount = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_player_maima[i][0] != -1 && m_user_status[i] == 1)
			{
				++userMaiMaCount;
			}
		}

		if (userMaiMaCount == calc_vaild_player_count() - 1)
		{
			return true;
		}
		return false;
	}

	/* 判断是否所有玩家都下完注了 */
	bool is_add_score_over()
	{
		Lint userSelectScore = 0;
		for (Lint i = 0; i < m_player_count; i++)
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

	/* 计算延迟时间 */
	Lint calc_delay_time(GAME_PLAY_STATE state)
	{
		double t_delayTime = 0.00;
		Lint t_realPlayCount = calc_vaild_player_count();
		if (GAME_PLAY_ADD_SCORE == state || GAME_PLAY_MAI_MA == state)
		{
			if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode)
			{
				if (m_round_offset == 0 || m_carToon)
				{
					t_delayTime = 0.2 * 4 * t_realPlayCount;
				}
			}
			else
			{
				if (m_qiang_zhuang_count == 0)
				{
					t_delayTime = 0.2 * 4 * t_realPlayCount;
				}
				else if (m_qiang_zhuang_count > 1)
				{
					t_delayTime = 0.2 * 4 * m_qiang_zhuang_count;
				}
				else if (m_qiang_zhuang_count == 1)
				{
					t_delayTime = 0.00;
				}
			}
		}
		//本局结束动画时间
		else if (GAME_PLAY_END == state)
		{
			//亮牌时间
			t_delayTime = 1.5 * t_realPlayCount;

			//特殊牌型时间
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				//比翼牛
				if (m_player_oxnum[i] == 100) t_delayTime += 1.0;
				//顺子牛
				else if (m_player_oxnum[i] == 102) t_delayTime += 1.0;
				//五花牛
				else if (m_player_oxnum[i] == 103) t_delayTime += 1.0;
				//同花牛
				else if (m_player_oxnum[i] == 107) t_delayTime += 1.0;
				//葫芦牛
				else if (m_player_oxnum[i] == 108) t_delayTime += 1.0;
				//炸弹牛
				else if (m_player_oxnum[i] == 109) t_delayTime += 1.0;
				//五小牛
				else if (m_player_oxnum[i] == 110) t_delayTime += 1.0;
				//炸弹牛
				else if (m_player_oxnum[i] == 111) t_delayTime += 1.0;
			}
			
			//庄家全赢时间
			t_delayTime += 1.0;
		}
		
		//返回时间
		if (t_delayTime == 0.00) return 0;
		else if (t_delayTime == (double)((int)t_delayTime)) return (int)t_delayTime;
		else return (int)(t_delayTime + 1);
	}

	///////////////////////////////////////////////////////////////////////////////////

	//托管抢庄倍数
	int tuoguan_qiang_zhuang(Lint pos)
	{
		//牛牛上庄、自由抢庄、公牌牛牛 抢庄托管为不抢
		if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
		{
			return 0;
		}
		//明牌抢庄、八人明牌
		if (m_tuo_guan_info[pos].m_zhuangScore == 0)
		{
			return 0;
		}
		int t_4cardsOxNum = m_gamelogic.GetCardsOxNumber(m_player_hand_card[pos], 4);
		int t_shiWeiNum = m_tuo_guan_info[pos].m_zhuangScore / 10;
		int t_geWeiNum = m_tuo_guan_info[pos].m_zhuangScore % 10;

		if (t_shiWeiNum < 8 || t_geWeiNum < 1 || t_geWeiNum > 4 || t_4cardsOxNum < t_shiWeiNum)
		{
			return 0;
		}

		return t_geWeiNum;
	}

	//托管自动下分
	int tuoguan_add_score(Lint pos)
	{
		if (m_tuo_guan_info[pos].m_addScore / m_addScoreTimes == 2)
		{
			return 2 * m_addScoreTimes;
		}
		else if (m_tuo_guan_info[pos].m_addScore / m_addScoreTimes == 1)
		{
			return m_addScoreTimes;
		}

		if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
		{
			return m_addScoreTimes;
		}

		int t_4cardsOxNum = m_gamelogic.GetCardsOxNumber(m_player_hand_card[pos], 4);
		int t_shiWeiNum = m_tuo_guan_info[pos].m_addScore / 10;
		int t_geWeiNum = m_tuo_guan_info[pos].m_addScore % 10;

		if (t_shiWeiNum < 8 || t_geWeiNum != 2 || t_4cardsOxNum < t_shiWeiNum)
		{
			return m_addScoreTimes;
		}
		else
		{
			return 2 * m_addScoreTimes;
		}


	}

	//托管推注
	int tuoguan_tui_score(Lint pos)
	{
		if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
		{
			return 0;
		}

		if (m_tuo_guan_info[pos].m_tuiScore == 0)
		{
			return 0;
		}

		int t_4cardsOxNum = m_gamelogic.GetCardsOxNumber(m_player_hand_card[pos], 4);
		int t_shiWeiNum = m_tuo_guan_info[pos].m_addScore / 10;
		int t_geWeiNum = m_tuo_guan_info[pos].m_addScore % 10;

		if (t_shiWeiNum < 8 || t_geWeiNum != 1 || t_4cardsOxNum < t_shiWeiNum)
		{
			return 0;
		}
		else
		{
			return m_player_tui_score[pos][0];
		}
	}

	/*计算玩家推注分数*/
	void calc_tui_zhu_score()
	{
		LLOG_ERROR("NiuNiu_NewGameHandler::start_game() Run... calc palyer tui zhu score deskId=[%d]",
			m_desk ? m_desk->GetDeskId() : 0);

		int t_tuiScore = m_tuiZhuTimes;

		//最大推注分数
		int t_maxTuiZhuScore = 0;

		//四个推注分数
		int t_tuiZhuScore[4];
		memset(t_tuiZhuScore, 0, sizeof(t_tuiZhuScore));

		int t_tuiGread = 0;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			memset(t_tuiZhuScore, 0, sizeof(t_tuiZhuScore));
			if (m_canTuiZhu[i])
			{
				t_maxTuiZhuScore = (m_per_round_score[i] + m_addScoreTimes * 2) < (m_addScoreTimes * m_tuiZhuTimes) ? (m_per_round_score[i] + m_addScoreTimes * 2) : (m_addScoreTimes * m_tuiZhuTimes);
				t_tuiGread = t_maxTuiZhuScore / (5 * m_addScoreTimes);

				for (Lint j = 0; j <= t_tuiGread; ++j)
				{
					if (t_maxTuiZhuScore == 5 * j * m_addScoreTimes)
					{
						break;
					}
					if (j == t_tuiGread)
					{
						if (t_maxTuiZhuScore > 2 * m_addScoreTimes)
						{
							t_tuiZhuScore[j] = t_maxTuiZhuScore;
						}
						break;
					}
					t_tuiZhuScore[j] = (j + 1)*(5 * m_addScoreTimes);
				}
				memcpy(m_player_tui_score[i], t_tuiZhuScore, sizeof(int) * 4);

			}
			else
			{
				memset(m_player_tui_score[i], 0, sizeof(int) * 4);
			}
			LLOG_ERROR("NiuNiu_NewGameHandler::start_game() Run... deskId=[%d], m_plauer_tui_score[%d]=[%d | %d | %d | %d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_player_tui_score[i][0], m_player_tui_score[i][1], m_player_tui_score[i][2], m_player_tui_score[i][3]);
		}
	}
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
		if (pos == NIUNIU_INVAILD_POS) return false;

		//亮牌
 		if (show_card)
		{
			if (pos != m_zhuangPos)
			{
				NiuNiuS2COpenCard open_card;
				open_card.m_remainTime = m_time_open_card;
				open_card.m_pos = pos;
				//消息标记为亮牌
				open_card.m_show = 1;
				//发送每个玩家的状态
				memcpy(open_card.m_playerStatus, m_user_status, sizeof(open_card.m_playerStatus));
				open_card.m_oxNum = m_player_oxnum[pos];
				open_card.m_oxTimes = m_player_oxtimes[pos];
				//如果这个位置是刚刚操作的位置，则把手牌复制给这个人
				for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
				{
					open_card.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
					open_card.m_showHandCard[j] = (Lint)m_show_hand_card[pos][j];
				}
				notify_desk(open_card);
				notify_lookon(open_card);
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					NiuNiuS2COpenCard open_card;
					open_card.m_remainTime = m_time_open_card;
					open_card.m_pos = pos;
					//消息标记为亮牌
					open_card.m_show = 1;
					//发送每个玩家的状态
					memcpy(open_card.m_playerStatus, m_user_status, sizeof(open_card.m_playerStatus));
					if (i == m_zhuangPos)
					{
						open_card.m_oxNum = m_player_oxnum[pos];
						open_card.m_oxTimes = m_player_oxtimes[pos];
						//如果这个位置是刚刚操作的位置，则把手牌复制给这个人
						for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
						{
							open_card.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
							open_card.m_showHandCard[j] = (Lint)m_show_hand_card[pos][j];
						}
					}
					notify_user(open_card, i);
				}

				//广播观战玩家
				NiuNiuS2COpenCard gz_open_card;
				gz_open_card.m_remainTime = m_time_open_card;
				gz_open_card.m_pos = pos;
				//消息标记为亮牌
				gz_open_card.m_show = 1;
				//发送每个玩家的状态
				memcpy(gz_open_card.m_playerStatus, m_user_status, sizeof(gz_open_card.m_playerStatus));

				notify_seat_lookon(gz_open_card);
				notify_lookon(gz_open_card);
			}
		}
		//发牌
		else
		{
			//计算牛牛牌型
			m_player_oxnum[pos] = m_gamelogic.GetCardType(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX, m_show_hand_card[pos]);
			//计算牛牛倍数
			m_player_oxtimes[pos] = m_gamelogic.GetTimes(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX);

			NiuNiuS2COpenCard send_card;
			send_card.m_pos = pos;

			//消息标记为发牌
			send_card.m_show = 0;
			send_card.m_gongCard = m_gong_card;
			send_card.m_openGongPos = m_open_gong_pos;
			//发送每个玩家的状态
			memcpy(send_card.m_playerStatus, m_user_status, sizeof(m_user_status));

			//给该玩家发手牌
			for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; i++)
			{
				send_card.m_playerHandCard[i] = m_player_hand_card[pos][i];
				send_card.m_showHandCard[i] = (Lint)m_show_hand_card[pos][i];
			}
			send_card.m_oxNum = m_player_oxnum[pos];
			send_card.m_oxTimes = m_player_oxtimes[pos];

			//发牌通知玩家
			notify_user(send_card, pos);

			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || i == pos)
					{
						continue;
					}
					NiuNiuS2COpenCard go_send_card;
					go_send_card.m_pos = i;

					//消息标记为发牌
					go_send_card.m_show = 0;
					go_send_card.m_gongCard = m_gong_card;
					go_send_card.m_openGongPos = m_open_gong_pos;
					//发送每个玩家的状态
					memcpy(go_send_card.m_playerStatus, m_user_status, sizeof(go_send_card.m_playerStatus));

					//给该玩家发手牌
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
						go_send_card.m_playerHandCard[j] = m_player_hand_card[i][j];
					}

					//发牌通知玩家
					notify_user(go_send_card, pos);
				}
			}

			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
			{
				//广播观战玩家：玩家发牌
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1)
					{
						continue;
					}
					NiuNiuS2COpenCard gz_goSendCard;
					gz_goSendCard.m_pos = i;

					//消息标记为发牌
					gz_goSendCard.m_show = 0;
					gz_goSendCard.m_gongCard = m_gong_card;
					gz_goSendCard.m_openGongPos = m_open_gong_pos;
					//发送每个玩家的状态
					memcpy(gz_goSendCard.m_playerStatus, m_user_status, sizeof(gz_goSendCard.m_playerStatus));

					//给该玩家发手牌
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
						gz_goSendCard.m_playerHandCard[j] = m_player_hand_card[i][j];
					}
					notify_seat_lookon(gz_goSendCard);
					notify_lookon(gz_goSendCard);
				}
			}
			else if(MAIN_MODE_GONG_PAI_NIU_NIU != m_mainMode)
			{
				//广播观战玩家：玩家发牌
				NiuNiuS2COpenCard gz_sendCard;
				gz_sendCard.m_pos = pos;
				gz_sendCard.m_show = 0;
				memcpy(gz_sendCard.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_seat_lookon(gz_sendCard);
				notify_lookon(gz_sendCard);
			}
			
		}
		return true;
	}

	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->UpdateNiuNiuOptCount(m_qiangCount, m_zhuangCount, m_tuiCount, m_player_count);
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	//俱乐部算分
	void calc_club_score()
	{
		//允许负分，本局输赢
		if (m_desk->m_isAllowCoinsNegative)
		{
			memcpy(m_player_coins, m_player_score, sizeof(m_player_coins));
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1)
				{
					continue;
				}

				m_accum_coins[i] += m_player_score[i];
			}
			return;
		}

		Lint t_zhuangWinScore = 0;
		Lint t_xianCoins = 0;
		Lint t_playerWinInfoCount = 0;
		//记录每个玩家本局开始时候的能量值，用于最多可赢取的值
		Lint t_playerCanWinMaxCoins[NIUNIU_PLAY_USER_COUNT];
		memcpy(t_playerCanWinMaxCoins, m_accum_coins, sizeof(t_playerCanWinMaxCoins));
		for (Lint i = 0; i < m_player_count; ++i)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::calc_club_score() Run... deskId=[%d], player[%d] before [coins | playScore | oxNum | oxTimes]=[%d | %d | %d | %d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_accum_coins[i], m_player_score[i], m_player_oxnum[i], m_player_oxtimes[i]);
		}

		PlayerWinInfo t_playerWinInfo[NIUNIU_PLAY_USER_COUNT];
		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_zhuangWinScore = 0;
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			//闲家输的分数
			if (m_player_score[i] <= 0)
			{
				t_xianCoins = m_accum_coins[i];
				//t_zhuangWinScore = -m_player_score[i];
				t_zhuangWinScore = (t_playerCanWinMaxCoins[m_zhuangPos] < (-1 * m_player_score[i])) ? t_playerCanWinMaxCoins[m_zhuangPos] : (-1 * m_player_score[i]);

				if (!m_desk->m_isAllowCoinsNegative && t_zhuangWinScore > t_xianCoins)
				{
					m_player_coins[m_zhuangPos] += t_xianCoins;
					m_accum_coins[m_zhuangPos] += t_xianCoins;

					m_player_coins[i] -= t_xianCoins;
					m_accum_coins[i] -= t_xianCoins;

					m_player_coins_status[i] = 1;
				}
				else
				{
					m_player_coins[m_zhuangPos] += t_zhuangWinScore;
					m_accum_coins[m_zhuangPos] += t_zhuangWinScore;

					m_player_coins[i] -= t_zhuangWinScore;
					m_accum_coins[i] -= t_zhuangWinScore;
				}
			}
			//闲家赢的，需要加入结构体排序后再计算分数
			else if (m_player_score[i] > 0)
			{
				t_playerWinInfo[t_playerWinInfoCount].m_index = i;
				t_playerWinInfo[t_playerWinInfoCount].m_oxNum = m_player_oxnum[i];
				t_playerWinInfo[t_playerWinInfoCount].m_oxTimes = m_player_oxtimes[i];
				t_playerWinInfo[t_playerWinInfoCount].m_playerScore = m_player_score[i];
				++t_playerWinInfoCount;
			}
		}

		//按牛牛大小排序赢庄家的闲家
		PlayerWinInfo t_tmp;
		for (Lint i = 0; i < t_playerWinInfoCount - 1; ++i)
		{
			for (Lint j = 0; j < t_playerWinInfoCount - 1 - i; ++j)
			{
				if (t_playerWinInfo[j].m_oxNum < t_playerWinInfo[j + 1].m_oxNum)
				{
					t_tmp = t_playerWinInfo[j];
					t_playerWinInfo[j] = t_playerWinInfo[j + 1];
					t_playerWinInfo[j + 1] = t_tmp;
				}
				else if (t_playerWinInfo[j].m_oxNum == t_playerWinInfo[j + 1].m_oxNum)
				{
					if (!m_gamelogic.CompareCard(m_player_hand_card[t_playerWinInfo[j].m_index], m_player_hand_card[t_playerWinInfo[j + 1].m_index], 5, t_playerWinInfo[j].m_oxNum, t_playerWinInfo[j + 1].m_oxNum))
					{
						t_tmp = t_playerWinInfo[j];
						t_playerWinInfo[j] = t_playerWinInfo[j + 1];
						t_playerWinInfo[j + 1] = t_tmp;
					}
				}
			}
		}
		//庄家输的分数
		Lint t_zhuangCoins = m_accum_coins[m_zhuangPos];
		Lint t_zhuangLoseScore = 0;
		Lint t_xianPos = NIUNIU_INVAILD_POS;
		for (Lint i = 0; i < t_playerWinInfoCount; ++i)
		{
			t_zhuangCoins = m_accum_coins[m_zhuangPos];
			t_xianPos = t_playerWinInfo[i].m_index;
			//t_zhuangLoseScore = t_playerWinInfo[i].m_playerScore;
			t_zhuangLoseScore = (t_playerCanWinMaxCoins[t_xianPos] < t_playerWinInfo[i].m_playerScore) ? t_playerCanWinMaxCoins[t_xianPos] : t_playerWinInfo[i].m_playerScore;

			if (!m_desk->m_isAllowCoinsNegative && t_zhuangLoseScore > t_zhuangCoins)
			{
				m_player_coins[m_zhuangPos] -= t_zhuangCoins;
				m_accum_coins[m_zhuangPos] -= t_zhuangCoins;

				m_player_coins[t_xianPos] += t_zhuangCoins;
				m_accum_coins[t_xianPos] += t_zhuangCoins;

				m_player_coins_status[m_zhuangPos] = 1;
			}
			else
			{
				m_player_coins[m_zhuangPos] -= t_zhuangLoseScore;
				m_accum_coins[m_zhuangPos] -= t_zhuangLoseScore;

				m_player_coins[t_xianPos] += t_zhuangLoseScore;
				m_accum_coins[t_xianPos] += t_zhuangLoseScore;
			}
		}

		memcpy(m_player_score, m_player_coins, sizeof(m_player_score));
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;
			LLOG_ERROR("NiuNiu_NewGameHandler::calc_club_score() Run... deskId=[%d], player[%d] after [coins | playScore | oxNum | oxTimes]=[%d | %d | %d | %d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_accum_coins[i], m_player_score[i], m_player_oxnum[i], m_player_oxtimes[i]);
		}
	}

	//俱乐部算分1，废弃
	void calc_club_score1(const Lint* winCount, const Lint* winTimes)
	{
		Lint t_zhuangWinScore = 0;
		Lint t_zhuangWinMaiScore = 0;
		Lint t_xianCoins = 0;
		Lint t_playerWinInfoCount = 0;
		PlayerWinInfo t_playerWinInfo[NIUNIU_PLAY_USER_COUNT];
		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_zhuangWinScore = 0;
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			//闲家赢的，需要加入结构体排序后再计算分数
			if (winCount[i] > 0)
			{
				t_playerWinInfo[t_playerWinInfoCount].m_index = i;
				t_playerWinInfo[t_playerWinInfoCount].m_oxNum = m_player_oxnum[i];
				t_playerWinInfo[t_playerWinInfoCount].m_oxTimes = m_player_oxtimes[i];
				++t_playerWinInfoCount;
			}
			//庄家赢的
			else
			{
				t_xianCoins = m_desk->m_user[i]->m_userData.m_coins;
				t_zhuangWinScore = m_play_add_score[m_zhuangPos] * m_play_add_score[i] * winTimes[m_zhuangPos];

				if (!m_desk->m_isAllowCoinsNegative && t_zhuangWinScore > t_xianCoins)
				{
					m_player_score[m_zhuangPos] += t_xianCoins;
					m_player_score[i] -= t_xianCoins;
				}
				else
				{
					m_player_score[m_zhuangPos] += t_zhuangWinScore;
					m_player_score[i] -= t_zhuangWinScore;
				}

				//闲家输的买码分数
				for (Lint m = 0; m < m_player_count; ++m)
				{
					if (m_zhuangPos == m || m_player_maima[m][1] == m_zhuangPos) continue;

					if (m_player_maima[m][1] == i)
					{
						t_xianCoins = m_desk->m_user[m]->m_userData.m_coins;
						t_zhuangWinMaiScore = m_play_add_score[m_zhuangPos] * m_player_maima[m][0] * winTimes[m_zhuangPos];
						if (!m_desk->m_isAllowCoinsNegative && t_zhuangWinMaiScore > t_xianCoins)
						{
							m_player_score[m_zhuangPos] += t_xianCoins;
							m_player_score[m] -= t_xianCoins;
						}
						else
						{
							m_player_score[m_zhuangPos] += t_zhuangWinMaiScore;
							m_player_score[m] -= t_zhuangWinMaiScore;
						}
					}
				}
			}
		}

		//按牛牛大小排序赢庄家的闲家
		PlayerWinInfo t_tmp;
		for (Lint i = 0; i < m_player_count - 1; ++i)
		{
			for (Lint j = 0; j < m_player_count - 1 - j; ++j)
			{
				if (t_playerWinInfo[j].m_oxNum < t_playerWinInfo[j + 1].m_oxNum)
				{
					t_tmp = t_playerWinInfo[j];
					t_playerWinInfo[j] = t_playerWinInfo[j + 1];
					t_playerWinInfo[j + 1] = t_tmp;
				}
				else if (t_playerWinInfo[j].m_oxNum == t_playerWinInfo[j + 1].m_oxNum)
				{
					if (!m_gamelogic.CompareCard(m_player_hand_card[t_playerWinInfo[j].m_index], m_player_hand_card[t_playerWinInfo[j + 1].m_index], 5, t_playerWinInfo[j].m_oxNum, t_playerWinInfo[j + 1].m_oxNum))
					{
						t_tmp = t_playerWinInfo[j];
						t_playerWinInfo[j] = t_playerWinInfo[j + 1];
						t_playerWinInfo[j + 1] = t_tmp;
					}
				}
			}
		}
		//庄家输的分数
		Lint t_zhuangCoins = m_desk->m_user[m_zhuangPos]->m_userData.m_coins;
		Lint t_zhuangLoseScore = 0;
		Lint t_xianPos = NIUNIU_INVAILD_POS;
		for (Lint i = 0; i < t_playerWinInfoCount; ++i)
		{
			t_zhuangCoins = m_desk->m_user[m_zhuangPos]->m_userData.m_coins;
			t_xianPos = t_playerWinInfo[i].m_index;
			t_zhuangLoseScore = m_play_add_score[m_zhuangPos] * m_play_add_score[t_xianPos] * winTimes[t_xianPos];
			if (!m_desk->m_isAllowCoinsNegative && t_zhuangLoseScore > t_zhuangCoins)
			{
				m_player_score[m_zhuangPos] -= t_zhuangCoins;
				m_player_score[i] += t_zhuangCoins;
			}
			else
			{
				m_player_score[m_zhuangPos] -= t_zhuangLoseScore;
				m_player_score[i] += t_zhuangLoseScore;
			}
		}
		//庄家输的闲家买码分
		for (Lint i = 0; i < t_playerWinInfoCount; ++i)
		{
			t_xianPos = t_playerWinInfo[i].m_index;
			for (Lint m = 0; m < m_player_count; ++m)
			{
				if (m_user_status[m] != 1 || m == m_zhuangPos || m_player_maima[m][1] == m_zhuangPos) continue;

				if (m_player_maima[m][1] == t_xianPos)
				{
					t_zhuangCoins = m_desk->m_user[m_zhuangPos]->m_userData.m_coins;
					t_zhuangLoseScore = m_play_add_score[m_zhuangPos] * m_player_maima[m][0] * winTimes[t_xianPos];
				}

				if (!m_desk->m_isAllowCoinsNegative && t_zhuangLoseScore > t_zhuangCoins)
				{
					m_player_score[m_zhuangPos] -= t_zhuangCoins;
					m_player_score[m] += t_zhuangCoins;
				}
				else
				{
					m_player_score[m_zhuangPos] -= t_zhuangLoseScore;
					m_player_score[m] += t_zhuangLoseScore;
				}
			}
		}
	}

	//小结算
	void finish_round(Lint jiesan = 0)
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler::finish_round() Run... This round is finished...deskId=[%d], round=[%d/%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit);

		if (0 != jiesan)
		{
			//发给客户端小结算
			NiuNiuS2CResult jiesan_result;
			jiesan_result.m_isShowFee = m_round_offset == 0 ? 1 : 0;
			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1)
				{
					continue;
				}
				for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
				{
					jiesan_result.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
				}
			}
			memcpy(jiesan_result.m_oxNum, m_player_oxnum, sizeof(jiesan_result.m_oxNum));
			memcpy(jiesan_result.m_oxTimes, m_player_oxtimes, sizeof(jiesan_result.m_oxTimes));
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				memcpy(jiesan_result.m_totleScore, m_desk->m_vip->m_coins, sizeof(jiesan_result.m_totleScore));
			}
			else
			{
				memcpy(jiesan_result.m_totleScore, m_desk->m_vip->m_score, sizeof(jiesan_result.m_totleScore));
			}
			notify_desk(jiesan_result);
			//广播观战玩家：小结算
			notify_lookon(jiesan_result);

			LLOG_ERROR("NiuNiu_NewGameHandler::finish_round() ... This has is dissolved... desk_id=[%d], desk_status=[%d], jie_san game over",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			set_desk_state(DESK_WAIT);

			//结束游戏
			if (m_desk) m_desk->HanderGameOver(jiesan);
			return;
		}

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
		CopyMemory(tmp_player_hand_card, m_player_hand_card, sizeof(tmp_player_hand_card));

		//庄家倍数
		tmp_win_times[m_zhuangPos] = m_gamelogic.GetTimes(tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX);
		LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[m_zhuangPos] = %d", m_desk ? m_desk->GetDeskId() : 0, tmp_win_times[m_zhuangPos]);

		//对比玩家
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			//获取倍数
			tmp_win_times[i] = m_gamelogic.GetTimes(tmp_player_hand_card[i], NIUNIU_HAND_CARD_MAX);
			if (m_gamelogic.CompareCard(tmp_player_hand_card[i], tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX, m_player_oxnum[i], m_player_oxnum[m_zhuangPos]))
			{
				++tmp_win_count[i];
				LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[%d] = %d", m_desk ? m_desk->GetDeskId() : 0, i, tmp_win_times[i]);
			}
			else
			{
				++tmp_win_count[m_zhuangPos];
			}
		}

		//统计玩家得分
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			if (tmp_win_count[i] > 0)
			{
				//闲家赢
				m_player_score[i] = m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[i];

				//推注：闲家赢，且上局没有推注，则本局可以推注（**Ren 2018-5-19）
				//if (m_tuiZhuTimes)
				//{
				//	//m_per_round_score[i] = m_player_score[i];
				//	m_canTuiZhu[i] = !m_canTuiZhu[i];
				//}

				LLOG_ERROR("desk_id = [%d] *****ceshi,Xian_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[%d] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], i, tmp_win_times[i]);
			}
			else
			{
				//庄家赢
				m_player_score[i] = (-1) * m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[m_zhuangPos];

				//推注：庄家赢了，闲家下局肯定没有推注权利
				/*if (m_tuiZhuTimes)
				{
					m_canTuiZhu[i] = false;
				}*/

				LLOG_ERROR("desk_id = [%d] *****ceshi,Zhuang_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[m_zhuangPos] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], tmp_win_times[m_zhuangPos]);
			}

			//计算买码得分
			if (m_player_maima[i][0] > 0)
			{
				if (tmp_win_count[m_player_maima[i][1]] > 0)
				{
					m_player_score[i] += m_play_add_score[m_zhuangPos] * m_player_maima[i][0] * tmp_win_times[m_player_maima[i][1]];
				}
				else
				{
					m_player_score[i] += (-1) * m_play_add_score[m_zhuangPos] * m_player_maima[i][0] * tmp_win_times[m_zhuangPos];
				}
			}


			m_player_score[m_zhuangPos] -= m_player_score[i];


		}

		//计算玩家金币输赢
		if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
		{
			calc_club_score();
		}

		//找出本局赢家
		Lint wWinner = m_zhuangPos;
		Lint maxScore = m_player_score[m_zhuangPos];
		for (Lint i = 0; i < m_player_count; i++)
		{
			m_accum_score[i] += m_player_score[i];
			if (m_player_score[i] > maxScore)
			{
				maxScore = m_player_score[i];
				wWinner = i;
			}
		}

		//推注记录上局玩家得分
		if (m_tuiZhuTimes)
		{
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_zhuangPos != i && m_player_score[i] > 0)
				{
					m_canTuiZhu[i] = !m_canTuiZhu[i];
					m_per_round_score[i] = m_player_score[i];
				}
				else
				{
					m_canTuiZhu[i] = false;
				}
			}
		}

		//玩家手牌[0-4] + 手牌类型[5] + 类型倍数[6] + 抢庄[7] + 下注[8]
		std::vector<Lint> t_handCards[NIUNIU_PLAY_USER_COUNT];

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; ++j)
			{
				t_handCards[i].push_back(m_player_hand_card[i][j]);
			}
			t_handCards[i].push_back(m_player_oxnum[i]);
			t_handCards[i].push_back(tmp_win_times[i]);
			t_handCards[i].push_back(m_play_qiang_zhuang[i]);
			t_handCards[i].push_back(m_play_add_score[i]);
			t_handCards[i].push_back(((m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0 && m_accum_coins[i] <= 0) ? 1 : 0));
		}

		//保存手牌 + 手牌类型 + 手牌倍数
		m_video.setHandCards(m_player_count, t_handCards);

		//回放保存记玩家本局得分
		m_video.setRoundScore(m_player_count, m_player_score);

		//保存回访
		//m_video.addUserScore(m_accum_score);
		if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
		{
			VideoSave();
		}

		//发给客户端小结算
		NiuNiuS2CResult send_result;
		//一局结束写日志
		add_round_log(m_player_score, wWinner);

		send_result.m_zhuangPos = m_zhuangPos;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				send_result.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
				send_result.m_showHandCard[i][j] = (Lint)m_show_hand_card[i][j];
			}
		}
		if (calc_vaild_player_count() > 2)
		{
			send_result.m_isZhuangAllWin = 1;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_zhuangPos == i) continue;

				if (m_player_score[i] > 0)
				{
					send_result.m_isZhuangAllWin = 0;
					break;
				}
			}
			/*if (tmp_win_count[m_zhuangPos] == calc_vaild_player_count() - 1)
			{
				send_result.m_isZhuangAllWin = 1;
			}
			else if (tmp_win_count[m_zhuangPos] == 0)
			{
				send_result.m_isZhuangAllWin = 2;
			}*/
		}
		memcpy(send_result.m_playScore, m_player_score, sizeof(send_result.m_playScore));
		memcpy(send_result.m_oxNum, m_player_oxnum, sizeof(send_result.m_oxNum));
		memcpy(send_result.m_oxTimes, tmp_win_times, sizeof(send_result.m_oxTimes));
		memcpy(send_result.m_playerCoinsStatus, m_player_coins_status, sizeof(send_result.m_playerCoinsStatus));
		if (m_desk && m_desk->m_vip)
		{
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				//俱乐部中能量值不够了，设置为入座观战状态
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (!m_desk->m_user[i]) continue;

					if (m_desk->m_vip->m_coins[i] <= 0)
					{
						m_user_status[i] = 2;
					}
					else
					{
						m_user_status[i] = 1;
					}
				}

				m_desk->SetHandlerPlayerStatus(m_user_status, m_player_count);
				memcpy(send_result.m_totleScore, m_accum_coins, sizeof(send_result.m_totleScore));
			}
			else
			{
				memcpy(send_result.m_totleScore, m_desk->m_vip->m_score, sizeof(send_result.m_totleScore));
			}

			LLOG_DEBUG("desk_id=[%d], send.m_TotalScore[%d %d %d %d %d %d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_totleScore[0],
				send_result.m_totleScore[1], send_result.m_totleScore[2],
				send_result.m_totleScore[3], send_result.m_totleScore[4],
				send_result.m_totleScore[5]);
		}
		else
		{
			LLOG_ERROR("m_desk or  m_desk->m_vip is null ");
		}
		memcpy(send_result.m_playerStatus, m_user_status, sizeof(send_result.m_playerStatus));

		LLOG_DEBUG("desk_id=[%d], GAME_END m_ZhuangPos=[%d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_zhuangPos);
		for (Lint i = 0; i < m_player_count; i++)
		{
			LLOG_DEBUG("GAME_END playerPos = %d", i);
			LLOG_ERROR("DeskId=[%d],GAME_END player%dHandCard = [%d,%d,%d,%d,%d]", m_desk ? m_desk->GetDeskId() : 0, i, m_player_hand_card[i][0], m_player_hand_card[i][1], m_player_hand_card[i][2], m_player_hand_card[i][3], m_player_hand_card[i][4]);
			LLOG_DEBUG("GAME_END m_Score[%d]", send_result.m_playScore[i]);
		}

		notify_desk(send_result);
		//广播观战玩家：小结算
		notify_lookon(send_result);
		set_desk_state(DESK_WAIT);

		if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
		{
			//俱乐部中能量值不够了，设置为入座观战状态
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (!m_desk->m_user[i]) continue;

				//每局开始更新玩家牌局中总能量值副本
				m_accum_coins[i] = m_desk->m_vip->m_coins[i];

				if (m_accum_coins[i] <= 0)
				{
					m_user_status[i] = 2;
				}
				else
				{
					m_user_status[i] = 1;
				}
			}
			m_desk->SetHandlerPlayerStatus(m_user_status, m_player_count);
			//如果只有一个人在座，则直接解散房间
			if (calc_vaild_player_count() <= 1)
			{
				LLOG_ERROR("NiuNiu_NewGameHandler::finish()... Player count <= 1, Game end... deskId=[%d], startRound=[%d/%d],  playerCount=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

				m_desk->HanderGameOver(2);
				return;
			}
		}
		//牛牛上庄，确定下局庄家
		if (m_mainMode == MAIN_MODE_NIUNIU_SHANG_ZHUANG)
		{
			//播放动画标记复位
			m_carToon = 0;

			Lint t_maxOxPos = m_zhuangPos;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_player_oxnum[i] != 10 || i == m_zhuangPos || m_user_status[i] != 1)
				{
					continue;
				}

				if (m_player_oxnum[t_maxOxPos] != 10)
				{
					t_maxOxPos = i;
				}
				else if (m_gamelogic.CompareCard(tmp_player_hand_card[i], tmp_player_hand_card[t_maxOxPos], NIUNIU_HAND_CARD_MAX, m_player_oxnum[i], m_player_oxnum[t_maxOxPos]))
				{
					t_maxOxPos = i;
				}
			}
			m_zhuangPos = t_maxOxPos;

			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0 && m_user_status[m_zhuangPos] != 1)
			{
				Lint tmp_rand_zhuang_pos = 0;
				//检测是否是有效随机出来的位置是否是有效位置
				do
				{
					tmp_rand_zhuang_pos = rand() % m_player_count;
					if (m_user_status[tmp_rand_zhuang_pos] == 1)
					{
						break;
					}
				} while (true);

				m_zhuangPos = tmp_rand_zhuang_pos;
				m_carToon = 1;
			}
		}

		
		//if (m_desk) m_desk->setDynamicToPlay();
		LLOG_ERROR("desk_id=[%d], desk_status=[%d], round_finish game over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		//结束游戏
		if (m_desk) m_desk->HanderGameOver(1);

	}

	///////////////////////////////////////////////////////////////////////////////

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
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d], qiangScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos], m_play_qiang_zhuang[pos]);
			return false;
		}
		if (m_play_qiang_zhuang[pos] != -1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Error!!! Player has opt qiang zhuang... deskId=[%d], pos=[%d], qiangScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_qiang_zhuang[pos]);
			return false;
		}
		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Run... Player Qiang Zhuang... deskId=[%d], pos=[%d], qiangScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, zhuang_score);

		//记录该玩家抢庄分数; -1：无效位置，0：不抢庄，>0：抢庄分数
		m_play_qiang_zhuang[pos] = zhuang_score;

		// 通知叫庄结果 
		NiuNiuS2CQiangZhuangNotify send;
		send.m_pos = pos;
		send.m_qiangZhuangScore = zhuang_score;
		notify_desk(send);

		//记录抢庄次数
		if (m_play_qiang_zhuang[pos] > 0)
		{
			++m_qiangCount[pos];

			//记录本局抢庄玩家数量
			++m_qiang_zhuang_count;
		}

		//广播观战玩家
		notify_lookon(send);

		if (is_select_zhuang_over())
		{
			LLOG_DEBUG("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Run... All Player Has Qiang ZhuangOver, Zhen Go to Next Status Add Score... deskId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0);
			
			Lint t_allPlayerNoQiang = 0;				//是否所有玩家都没有抢庄   1：所有玩家都没有抢庄    0：有玩家抢庄
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
				t_allPlayerNoQiang = 1;
				m_play_qiang_zhuang[m_zhuangPos] = 1;
			}

			//记录选庄倍数
			m_play_add_score[m_zhuangPos] = m_play_qiang_zhuang[m_zhuangPos];

			//推注：庄家确定了，庄家没有推注的权利(**Ren 2018-5-19)
			if (m_tuiZhuTimes)
			{
				m_canTuiZhu[m_zhuangPos] = false;
				memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);
			}

			//下注限制：手动抢庄没抢到的玩家不能下最小分，没参与抢庄的玩家不能推注
			if (m_isConstAddScore)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;

					//自由抢庄
					if (m_mainMode == MAIN_MODE_ZI_YOU_QIANG_ZHUANG || m_mainMode == MAIN_MODE_NIUNIU_SHANG_ZHUANG)
					{
						if (m_user_status[i] == 1 && m_play_qiang_zhuang[i] > 0 )
						{
							m_canXiaMinZhu[i] = 1;
						}
						if (m_user_status[i] == 1 && m_tuiZhuTimes && m_play_qiang_zhuang[i] <= 0)
						{
							m_canTuiZhu[i] = false;
						}
					}
					//明牌抢庄、八人明牌、十人明牌、公牌牛牛
					else
					{
						if (m_user_status[i] == 1 && m_play_qiang_zhuang[i] > 0 && m_play_qiang_zhuang[i] == m_play_qiang_zhuang[m_zhuangPos])
						{
							m_canXiaMinZhu[i] = 1;
						}
						if (m_user_status[i] == 1 && m_tuiZhuTimes && (m_play_qiang_zhuang[i] <= 0 || m_play_qiang_zhuang[i] < m_play_qiang_zhuang[m_zhuangPos]))
						{
							m_canTuiZhu[i] = false;
						}
					}
				}
			}
			//推注限制:手动参与抢庄没抢到庄家的玩家下注时，可选择小分，大分或推注；不抢庄的玩家不能推注
			else if (m_isConstTuiZhu && m_tuiZhuTimes)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;

					//自由抢庄
					if (m_mainMode == MAIN_MODE_ZI_YOU_QIANG_ZHUANG || m_mainMode == MAIN_MODE_NIUNIU_SHANG_ZHUANG)
					{
						if (m_user_status[i] != 1 || m_play_qiang_zhuang[i] <= 0)
						{
							m_canTuiZhu[i] = false;
						}
					}
					//明牌抢庄、八人明牌、十人明牌、公牌牛牛
					else
					{
						if (m_user_status[i] == 1 && (m_play_qiang_zhuang[i] <= 0 || m_play_qiang_zhuang[i] < m_play_qiang_zhuang[m_zhuangPos]))
						{
							m_canTuiZhu[i] = false;
						}
					}
				}
			}

			//下注翻倍：手动参与抢庄且倍数高但没有抢到庄家的玩家下注时，可以使用加倍功能。
			if (m_isAddScoreDouble)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;
					if (m_user_status[i] == 1 && m_play_qiang_zhuang[i] > 0 && m_play_qiang_zhuang[i] == m_play_qiang_zhuang[m_zhuangPos])
					{
						m_canAddScoreDouble[i] = 1;
					}
				}
			}

			//确定庄家后广播选庄结果
			notify_zhuang(m_zhuangPos, t_allPlayerNoQiang, m_play_qiang_zhuang[m_zhuangPos]);

			//选庄结束，将游戏状态设置为下注或买码
			if (m_isMaiMa && calc_vaild_player_count() > 2)
			{
				set_play_status(GAME_PLAY_MAI_MA);
				m_add_score_delay_time = m_time_add_score + calc_delay_time(GAME_PLAY_MAI_MA);
			}
			else
			{
				set_play_status(GAME_PLAY_ADD_SCORE);
				m_add_score_delay_time =  m_time_add_score + calc_delay_time(GAME_PLAY_ADD_SCORE);
			}

			//游戏开始，下注倒计时
			m_play_status_time.Now();
			m_tick_flag = true;

			return true;
		}
		return false;
	}

	/***************************************************
	* 函数名：    on_event_user_mai_ma()
	* 描述：      处理玩家买码操作
	* 参数：
	*  @ pos 玩家在桌子上的位置
	*  @ maiPos 玩家买码的位置
	*  @ maiScore 玩家
	* 返回：      (bool)
	****************************************************/
	bool on_event_user_mai_ma(Lint pos, Lint maiPos, Lint maiScore)
	{
		//校验数据
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d], maiScore=[%d], maiPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos], maiScore, maiPos);
			return false;
		}
		//如果庄家买码 或 买庄家的码，不允许
		if (pos == m_zhuangPos || maiPos == m_zhuangPos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() error!!! mai ma player pos invalid!!! deskId=[%d], pos=[%d], maiScore=[%d], maiPos=[%d], zhuangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, maiScore, maiPos, m_zhuangPos);
			return false;
		}

		//买码的分数不能为负数
		if (maiScore < 0)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() error!!! mai ma score invalid!!! deskId=[%d], pos=[%d], maiScore=[%d], maiPos=[%d], zhuangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, maiScore, maiPos, m_zhuangPos);
			return false;
		}

		//不能重复买码
		if (m_player_maima[pos][0] != -1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() Error!!!  Player Has Mai Ma!!! deskId=[%d], pos=[%d], maiPos=[%d], maiScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_player_maima[pos][1], m_player_maima[pos][0]);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() Run... Player Mai Ma... deskId=[%d], pos=[%d], maiScore=[%d], maiPos=[%d], zhuangPos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, maiScore, maiPos, m_zhuangPos);

		m_player_maima[pos][0] = maiScore;
		m_player_maima[pos][1] = maiPos;

		NiuNiuS2CMaiMaBC maiMabc;
		maiMabc.m_pos = pos;
		maiMabc.m_maiPos = maiPos;
		maiMabc.m_maiScore = maiScore;
		memcpy(maiMabc.m_maiMa, m_player_maima, sizeof(m_player_maima));
		notify_desk(maiMabc);
		notify_lookon(maiMabc);

		if (is_mai_ma_over())
		{
			//买码结束，下注倒计时（买码包含在下注时间里面）
			//m_play_status_time.Now();
			//m_tick_flag = true;

			//通知客户端买码结束，开始下注
			set_play_status(GAME_PLAY_ADD_SCORE);

			NiuNiuS2CMaiMaBC maiMabc;
			memcpy(maiMabc.m_maiMa, m_player_maima, sizeof(m_player_maima));
			notify_desk(maiMabc);
			notify_lookon(maiMabc);
		}
		return true;
	}

	/***************************************************
	* 函数名：    on_event_user_add_score()
	* 描述：      处理玩家下注
	* 参数：
	*  @ pos 玩家在桌子上的位置
	*  @ score 玩家下注分数
	*  @ addDouble 是否选择加倍
	*  @ tuiZhu 是否为推注消息   0:正常下注   1：推注下注
	* 返回：      (bool)
	****************************************************/
	bool on_event_user_add_score(Lint pos, Lint score, Lint addDouble, Lint tuiZhu)
	{
		//校验数据
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_add_score() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		//不是加倍选项且下注分数为0
		if (0 == score && 0 == addDouble)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!! Player add score invalid!!! desk_id=[%d], pos[%d], addScore=[%d], zhuangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, score, m_zhuangPos);
			return false;
		}
		//庄家不能下注
		if (pos == m_zhuangPos)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!!, zhuang can not add score!!! deskId=[%d], pos=[%d], zhuang=[%d] addScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_zhuangPos, score);
			return false;
		}
		//检测重复下注
		if (m_play_add_score[pos] > 0)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!!, this player has add score!!! deskId=[%d], pos=[%d], hasAddScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_add_score[pos]);
			return false;
		}

		if (m_canTuiZhu[pos] == 0 && tuiZhu == 1)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!!, this player can not tui zhu!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() Run... desk_id=[%d], pos[%d], zhuangPos=[%d], addScore=[%d], addDouble=[%d], tuiZhu=[%d], canTuiZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, m_zhuangPos, score, addDouble, tuiZhu, m_canTuiZhu[pos]);

		//下注
		if (addDouble == 0 && score > 0)
		{
			m_play_add_score[pos] = score;
			
			//记录推注次数
			if (tuiZhu == 1 && m_canTuiZhu[pos] == 1)
			{
				++m_tuiCount[pos];
			}
			//本局不推注下局可以连续推注
			else if (tuiZhu == 0 && m_canTuiZhu[pos] == 1)
			{
				m_canTuiZhu[pos] = 0;
			}
		}
		//加倍
		else
		{
			m_add_score_double_status[pos] = 1;
		}

		//玩家下注完成后广播下注分数
		NiuNiuS2CAddScore score_notify;
		score_notify.m_pos = pos;
		score_notify.m_score = score;
		memcpy(score_notify.m_addScoreDoubleStatus, m_add_score_double_status, sizeof(score_notify.m_addScoreDoubleStatus));
		notify_desk(score_notify);
		notify_lookon(score_notify);

		//广播观战玩家：玩家下注
		notify_lookon(score_notify);

		if (is_add_score_over())
		{

			//所有下注完成，修改游戏状态为开牌状态
			set_play_status(GAME_PLAY_SEND_CARD);

			//打印日志：
			LLOG_ERROR("desk_id=[%d], deskStatus=[%d],clent add score over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			//如果是公牌牛牛，则确定搓公牌的玩家位置
			/*
			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
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

				m_open_gong_pos = tmp_rand_zhuang_pos;
			}*/
			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
			{
				Lint t_addMaxScore = 0;
				Lint t_maxScoreCount = 0;
				Lint t_playAddScore[NIUNIU_PLAY_USER_COUNT];
				memcpy(t_playAddScore, m_play_add_score, sizeof(t_playAddScore));
				Lint t_maxScoreIndex[NIUNIU_PLAY_USER_COUNT];
				memset(t_maxScoreIndex, 0, sizeof(t_maxScoreIndex));
				Lint t_maxXianScore = 0;
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;
					if (t_playAddScore[i] > t_maxXianScore)
					{
						t_maxXianScore = t_playAddScore[i];
					}
				}
				if (t_playAddScore[m_zhuangPos] > t_maxXianScore)
				{
					t_playAddScore[m_zhuangPos] = 0;
				}

				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1) continue;

					if (t_playAddScore[i] > t_addMaxScore)
					{
						t_addMaxScore = t_playAddScore[i];
						memset(t_maxScoreIndex, 0, sizeof(t_maxScoreIndex));
						t_maxScoreIndex[0] = i;
						t_maxScoreCount = 1;
					}
					else if (t_playAddScore[i] < t_addMaxScore)
					{
						continue;
					}
					else
					{
						t_maxScoreIndex[++t_maxScoreCount] = i;
					}
				}

				if (t_maxScoreCount == 1)
				{
					m_open_gong_pos = t_maxScoreIndex[0];
				}
				else
				{
					m_open_gong_pos = m_zhuangPos;
				}
			}

			//所有人下注完成，给每个玩家发牌
			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1)continue;
				send_user_card(i, false);
			}

			//玩家下注完成，进入亮牌阶段（包括翻牌、搓牌时间）
			m_play_status_time.Now();
			m_tick_flag = true;
		}
		return true;
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
		if (GAME_PLAY_SEND_CARD != m_play_status)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_open_card() Error..., status not GAME_PLAY_SEND_CARD, deskId=[%d], pos[%d], palyStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);

			return false;
		}
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_open_card() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_open_card() Run... Player Open Card... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		//标记该用户为已开牌
		m_isOpenCard[pos] = true;

		if (m_mainMode == MAIN_MODE_GONG_PAI_NIU_NIU)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && pos != i)
				{
					m_isOpenCard[i] = true;
				}
			}
		}

		//广播该用户开牌结果
		send_user_card(pos, true);

		//计算该玩家牌形
		//m_player_oxnum[pos] = m_gamelogic.GetCardType(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX);

		if (is_open_card_over() || (m_mainMode == MAIN_MODE_GONG_PAI_NIU_NIU && m_isOpenCard[pos]))
		{
			set_play_status(GAME_PLAY_END);
			//广播每个人亮牌
			//结算
			finish_round();
		}
		return true;
	}

	//处理玩家搓共牌
	bool on_event_user_cuo_gong_card(Lint pos, Lstring cuoPai)
	{
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cuo_gong_card() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		if (pos != m_open_gong_pos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cuo_gong_card() Error!!! This palyer is not cuo gong pai pos!!! deskId=[%d], pos=[%d], cuoGongPaiPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_open_gong_pos);
			return false;
		}
		m_cuo_pai = cuoPai;
		NiuNiuS2CCuoGongPaiBC send;
		send.m_cuoPai = cuoPai;
		notify_desk(send);
		notify_lookon(send);

		return true;
	}

	//处理玩家托管
	bool on_event_user_tuo_guan(Lint pos, Lint zhuangScore, Lint addScore, Lint tuiScore)
	{
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		if (m_tuo_guan_info[pos].m_tuoGuanStatus == 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Error!!! This palyer has in tuo guan status!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		if ((MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode) &&
			(addScore > 2 || addScore < 1 || zhuangScore != 0 || tuiScore != 0))
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Error!!! Tuo Guan operators is invalid!!! deskId=[%d], pos=[%d], mainMode=[%d], zhuangScore=[%d], addScore=[%d], tuiScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_mainMode, zhuangScore, addScore, tuiScore);
			return false;
		}


		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Run... deskId=[%d], pos=[%d], tuoZhuangScore=[%d], tuoAddScore=[%d], tuoTuiScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, zhuangScore, addScore, tuiScore);

		m_tuo_guan_info[pos].m_pos = pos;
		m_tuo_guan_info[pos].m_tuoGuanStatus = 1;
		m_tuo_guan_info[pos].m_zhuangScore = zhuangScore;
		m_tuo_guan_info[pos].m_addScore = addScore;
		m_tuo_guan_info[pos].m_tuiScore = tuiScore;

		return true;
	}

	//处理取消托管
	bool on_event_user_cancel_tuo_guan(Lint pos)
	{
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cancel_tuo_guan() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		if (m_tuo_guan_info[pos].m_tuoGuanStatus != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cancel_tuo_guan() Error!!! This palyer is not in tuo guan status!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cancel_tuo_guan() Run... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		m_tuo_guan_info[pos].m_pos = NIUNIU_HAND_CARD_MAX;
		m_tuo_guan_info[pos].m_tuoGuanStatus = 0;
		m_tuo_guan_info[pos].m_zhuangScore = 0;
		m_tuo_guan_info[pos].m_addScore = 0;
		m_tuo_guan_info[pos].m_tuiScore = 0;

		return true;
	}

	//处理开始计时
	bool on_event_start_time(Lint pos)
	{
		//自由抢庄开始计时，抢庄阶段
		m_play_status_time.Now();
		m_tick_flag = true;
		return true;
	}

	/* 牛牛上庄 */
	void niuniu_shang_zhuang()
	{
		//第一局随机庄家
		if (m_round_offset == 0)
		{
			Lint tmp_rand_zhuang_pos = 0;
			//检测是否是有效随机出来的位置是否是有效位置
			do
			{
				tmp_rand_zhuang_pos = rand() % m_player_count;
				if (m_user_status[tmp_rand_zhuang_pos] == 1)
				{
					break;
				}
			} while (true);

			m_zhuangPos = tmp_rand_zhuang_pos;
		}
		//以后每局庄家都是直接确定的
		else
		{
			//推注：庄家确定了，庄家没有推注的权利(**Ren 2018-5-19)
			if (m_tuiZhuTimes)
			{
				m_canTuiZhu[m_zhuangPos] = false;
				memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);
			}
		}

		//回放记录公共数据
		m_video.setCommonInfo(gWork.GetCurTime().Secs(), m_round_offset, m_player_count, m_desk->m_vip->m_posUserId, m_zhuangPos);
		
		m_play_qiang_zhuang[m_zhuangPos] = 1;

		NiuNiuS2CStartGame send_start;
		send_start.m_remainTime = m_time_add_score;
		send_start.m_zhuangPos = m_zhuangPos;
		send_start.m_scoreTimes = 1;  //非抢庄模式下默认倍数为一倍
		send_start.m_maiMa = (m_isMaiMa && calc_vaild_player_count() > 2) ? 1 : 0;
		send_start.m_carToon = m_carToon;
		//推注：玩家可推注状态
		memcpy(send_start.m_isCanTui, m_canTuiZhu, sizeof(send_start.m_isCanTui));
		memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
		memcpy(send_start.m_playerAddScoreDouble, m_canAddScoreDouble, sizeof(send_start.m_playerAddScoreDouble));
		memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
		//memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
		m_play_add_score[m_zhuangPos] = 1;
		notify_desk(send_start);

		//广播观战玩家
		notify_lookon(send_start);

		//记录坐庄次数
		++m_zhuangCount[m_zhuangPos];

		//选庄结束，设置游戏状态为买码或下注状态
		if (m_isMaiMa && calc_vaild_player_count() > 2)
		{
			set_play_status(GAME_PLAY_MAI_MA);
			m_add_score_delay_time = m_time_add_score + calc_delay_time(GAME_PLAY_MAI_MA);
		}
		else
		{
			set_play_status(GAME_PLAY_ADD_SCORE);
			m_add_score_delay_time = m_time_add_score + calc_delay_time(GAME_PLAY_ADD_SCORE);
		}
		//播放动画标记复位
		//m_carToon = 0;
		//牛牛上庄不需要抢庄，直接到下注
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* 自由抢庄 */
	void free_zhuang_pos()
	{
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			//62072:通知客户端抢庄
			NiuNiuS2CSelectZhuang qiang_zhuang;
			qiang_zhuang.m_remainTime = m_time_qiang_zhuang;
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				if (m_accum_coins[i] < m_desk->m_qiangZhuangMinCoins)
				{
					//on_event_user_select_zhuang(i, 0);
					m_noQiangZhuang[i] = 0;
					qiang_zhuang.m_noQiangZhuang = 0;
				}
			}
			qiang_zhuang.m_pos = i;
			qiang_zhuang.m_qingZhuang = 0;  //设置为抢庄模式
			qiang_zhuang.m_qiangTimes = 1;  //只有抢与不抢
			memcpy(qiang_zhuang.m_isCanTui, m_canTuiZhu, sizeof(qiang_zhuang.m_isCanTui));
			memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_user(qiang_zhuang, i);

			if (m_noQiangZhuang[i] == 0)
			{
				on_event_user_select_zhuang(i, 0);
			}
		}

		//广播观战玩家
		NiuNiuS2CSelectZhuang gz_zhuang;
		gz_zhuang.m_remainTime = m_time_qiang_zhuang;
		gz_zhuang.m_pos = m_player_count;
		gz_zhuang.m_qingZhuang = 1;
		gz_zhuang.m_qiangTimes = 1;  //只有抢与不抢
		gz_zhuang.m_noQiangZhuang = 0;
		memcpy(gz_zhuang.m_isCanTui, m_canTuiZhu, sizeof(gz_zhuang.m_isCanTui));
		memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
		notify_seat_lookon(gz_zhuang);
		notify_lookon(gz_zhuang);

		//选庄结束，设置游戏状态为买码或下注状态
		/*if (m_isMaiMa)
		{
		set_play_status(GAME_PLAY_MAI_MA);
		}
		else
		{
		set_play_status(GAME_PLAY_ADD_SCORE);
		}*/

		//自由抢庄开始计时，抢庄阶段
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* 看牌抢庄 */
	void qiang_zhuang_pos()
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler::qiang_zhuang_pos() Run... This mode is QiangZhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			
			//62072:通知客户端抢庄
			NiuNiuS2CSelectZhuang qiang_zhuang;
			qiang_zhuang.m_remainTime = m_time_qiang_zhuang;
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				if (m_accum_coins[i] < m_desk->m_qiangZhuangMinCoins)
				{
					//on_event_user_select_zhuang(i, 0);
					m_noQiangZhuang[i] = 0;
					qiang_zhuang.m_noQiangZhuang = 0;
				}
			}
			qiang_zhuang.m_pos = i;         //玩家位置
			qiang_zhuang.m_qingZhuang = 1;  //设置为抢庄模式
			memcpy(qiang_zhuang.m_isCanTui, m_canTuiZhu, sizeof(qiang_zhuang.m_isCanTui));
			//将每个玩家的手牌写入看牌选庄的消息中,每个玩家先只发4张牌
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
			{
				qiang_zhuang.m_playerHandCard[j] = m_player_hand_card[i][j];
			}
			memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_user(qiang_zhuang, i);

			if (m_noQiangZhuang[i] == 0)
			{
				on_event_user_select_zhuang(i, 0);
			}
		}

		//广播观战玩家
		NiuNiuS2CSelectZhuang gz_zhuang;
		gz_zhuang.m_remainTime = m_time_qiang_zhuang;
		gz_zhuang.m_pos = m_player_count;
		gz_zhuang.m_qingZhuang = 1;
		gz_zhuang.m_noQiangZhuang = 0;
		memcpy(gz_zhuang.m_isCanTui, m_canTuiZhu, sizeof(gz_zhuang.m_isCanTui));
		memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
		notify_seat_lookon(gz_zhuang);
		notify_lookon(gz_zhuang);

		LLOG_DEBUG("NiuNiu_NewGameHandler::qiang_zhuang_pos() Run... Has Notify Client Start Qiang Zhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());

		//看牌抢庄开始计时，抢庄阶段
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* 选庄 */
	void select_zhuang()
	{
		switch (m_mainMode)
		{

			//牛牛上庄
		case MAIN_MODE_NIUNIU_SHANG_ZHUANG:
		{
			niuniu_shang_zhuang();
			break;
		}

		//自由抢庄
		case MAIN_MODE_ZI_YOU_QIANG_ZHUANG:
		{
			free_zhuang_pos();
			break;
		}

		//明牌抢庄
		case MAIN_MODE_MING_PAI_QIANG_ZHUANG:
		{
			//通知玩家下注抢庄
			qiang_zhuang_pos();
			break;
		}

		//八人明牌
		case MAIN_MODE_BA_REN_MING_PAI:
		{
			//通知玩家下注抢庄
			qiang_zhuang_pos();
			break;
		}

		//公牌牛牛
		case MAIN_MODE_GONG_PAI_NIU_NIU:
		{
			//通知玩家下注抢庄
			qiang_zhuang_pos();

			//公牌
			Lint tmp_gong_pos = 0;
			//检测是否是有效随机出来的位置是否是有效位置
			do
			{
				tmp_gong_pos = rand() % m_player_count;
				if (m_user_status[tmp_gong_pos] == 1)
				{
					break;
				}
			} while (true);

			m_gong_card = m_player_hand_card[tmp_gong_pos][4];

			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (tmp_gong_pos == i)
				{
					continue;
				}
				m_player_hand_card[i][4] = m_player_hand_card[tmp_gong_pos][4];
			}
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
		LLOG_DEBUG("NiuNiu_NewGameHandler::start_game() Run... Game Start... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//乱序后的扑克牌
		BYTE cbRandCard[NIUNIU_MAX_POKER_COUNT];

		//乱序扑克，带王癞子
		if (m_isLaiZi)
		{
			//乱序扑克
			m_gamelogic.RandCardList(cbRandCard, NIUNIU_LAIZI_POKER_COUNT);
			debugSendCards_NiuNiu_New(cbRandCard, m_desk, "card_niuniu_new.ini", NIUNIU_PLAY_USER_COUNT, NIUNIU_HAND_CARD_MAX, 54);
		}
		//乱序扑克，不带癞子
		else
		{
			//乱序扑克
			m_gamelogic.RandCardList(cbRandCard, NIUNIU_NORMAL_POKER_COUNT);
			debugSendCards_NiuNiu_New(cbRandCard, m_desk, "card_niuniu_new.ini", NIUNIU_PLAY_USER_COUNT, NIUNIU_HAND_CARD_MAX, 52);
		}

		//给每个玩家发牌
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			memcpy(&m_player_hand_card[i], &cbRandCard[i*NIUNIU_HAND_CARD_MAX], sizeof(BYTE)*NIUNIU_HAND_CARD_MAX);
		}

		//每局重置玩家可以推注的分数
		if (m_tuiZhuTimes)
		{
			calc_tui_zhu_score();
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
		LLOG_DEBUG("NiuNiu_NewGameHandler::start_round() Run... Game Start Round........................... deskId=[%d]",
			m_desk ? m_desk->GetDeskId() : 0);

		clear_round();
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);
		memcpy(m_user_status, player_status, sizeof(m_user_status));

		if (m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);

			if(m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				//俱乐部中能量值不够了，设置为入座观战状态
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (!m_desk->m_user[i]) continue;

					//每局开始更新玩家牌局中总能量值副本
					m_accum_coins[i] = m_desk->m_vip->m_coins[i];

					LLOG_ERROR("NiuNiu_NewGameHandler::start_round() Run... deskId=[%d], player[%d] roundStartCoins=[%d]",
						m_desk ? m_desk->GetDeskId() : 0, i, m_accum_coins[i]);

					if (m_accum_coins[i] <= 0)
					{
						m_user_status[i] = 2;
					}
					else
					{
						m_user_status[i] = 1;
					}

					LMsgS2CUpdateCoins updateCoins;
					updateCoins.m_pos = i;
					updateCoins.m_userId = m_desk->m_user[i]->m_userData.m_id;
					updateCoins.m_coins = m_accum_coins[i];
					notify_desk(updateCoins);
					notify_lookon(updateCoins);
					
				}

				m_desk->SetHandlerPlayerStatus(m_user_status, m_player_count);

				//如果只有一个人在座，则直接解散房间
				if (calc_vaild_player_count() <= 1)
				{
					LLOG_ERROR("NiuNiu_NewGameHandler::start_round()... Player count <= 1, Game end... deskId=[%d], startRound=[%d/%d],  playerCount=[%d]",
						m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

					finish_round(2);
					return;
				}
			}
		}

		//计算本局有实际参与玩家数量
		m_real_player_count = calc_vaild_player_count();

		LLOG_ERROR("NiuNiu_NewGameHandler::start_round()... Game Round Info... deskId=[%d], startRound=[%d/%d],  playerCount=[%d], realPlayerCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count, m_real_player_count);
		
		//游戏开始入口
		start_game();
	}
};


struct NiuNiu_NewGameHandler : NiuNiu_NewGameCore
{

	/* 构造函数 */
	NiuNiu_NewGameHandler()
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler Init...");
	}

	/* 给玩家发消息 */
	void notify_user(LMsg &msg, int pos) override
	{
		if (NULL == m_desk) return;
		if (pos < 0 || pos >= m_player_count) return;
		User *u = m_desk->m_user[pos];
		if (NULL == u) return;
		u->Send(msg);
	}

	/* 广播给所有入座的玩家 */
	void notify_desk(LMsg &msg) override
	{
		if (NULL == m_desk) return;
		m_desk->BoadCast(msg);
	}

	/* 广播给所有入座参与游戏的玩家*/
	void notify_seat_playing(LMsg& msg)
	{
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1)
			{
				notify_user(msg, i);
			}
		}
	}

	/* 广播给所有入座观战的玩家 */
	void notify_seat_lookon(LMsg& msg)
	{
		for(Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 2 || m_desk->m_playStatus[i] == 2)
			{
				notify_user(msg, i);
			}
		}
	}

	/* 广播给观战玩家 */
	void notify_lookon(LMsg &msg) override
	{
		if (NULL == m_desk) return;
		m_desk->MHBoadCastDeskLookonUser(msg);
	}

	/* */
	bool startup(Desk *desk)
	{
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

		// 小选项[0]：大玩法标签：1牛牛上庄、2自由抢庄、3名牌抢庄、4、十人明牌、5公牌牛牛
		m_mainMode = m_playtype.NiuNiu_NewGetMainMode();

		// 小选项[1]：人数6人 8人 10人
		m_playerNum = m_playtype.NiuNiu_NewGetPlayerNum();

		// 小选项[2]：抢庄倍率：1-1倍，2-2倍，3-3倍，4-4倍
		m_qiangZhuangTimes = m_playtype.NiuNiu_NewQiangZhuangTimes();

		// 小选项[3]：下注倍率：1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
		m_addScoreTimes = m_playtype.NiuNiu_NewAddScoreTimes();

		// 小选项[4]：自动开始：1-满2人随时开始，4-满4人开，5-满5人开，6-满6人开，4-暗牌下注
		m_autoStart = m_playtype.NiuNiu_NewAutoStartGame();

		// 小选项[5]：推注分数： 0：不推注，5：5倍推注，10：10倍推注，15:15倍推注，20:20倍推注
		m_tuiZhuTimes = m_playtype.NiuNiu_NewGetTuiZhu();

		// 小选项[6]：是否支持动态加入：false-不允许，true-允许
		m_isDynamicIn = m_playtype.NiuNiu_NewGetDynamicIn();

		// 小选项[7]：是否允许托管：false-不允许，true-允许
		m_isAutoPlay = m_playtype.NiuNiu_NewGetAutoPlay();

		// 小选项[8]：是否允许癞子：false-不允许，true-允许
		m_isLaiZi = m_playtype.NiuNiu_NewAllowLaiZi();

		// 小选项[9]：是否允许买码：false-不允许，true-允许
		m_isMaiMa = m_playtype.NiuNiu_NewAllowMaiMa();

		// 小选项[10]：是否允许搓牌：false-不允许，true-允许
		m_isCuoPai = m_playtype.NiuNiu_NewGetCuoPai();

		// 小选项[11]：是否下注限制：false-不限制，true-限制
		m_isConstAddScore = m_playtype.NiuNiu_NewGetConstAddScore();

		// 小选项[12]：是否推注限制：false-不限制，true-限制
		m_isConstTuiZhu = m_playtype.NiuNiu_NewGetConstTuiZhu();

		// 小选项[13]：是否允许加倍：false-不允许，true-允许
		m_isAddScoreDouble = m_playtype.NiuNiu_NewGetConstAddScoreDouble();

		// 小选项[14]：1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1   2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六 - 无牛x1
		m_niuNiuTimes = m_playtype.NiuNiu_NewTypeTimes();

		// 小选项[15]：是否允许比翼牛：false-不允许，true-允许
		m_isBiYi = m_playtype.NiuNiu_NewAllowTypeBiYi();

		// 小选项[16]：是否允许顺子牛：false-不允许，true-允许
		m_isShunZi = m_playtype.NiuNiu_NewAllowTypeShunZi();

		// 小选项[17]：是否允许五花牛：false-不允许，true-允许
		m_isWuHua = m_playtype.NiuNiu_NewAllowTypeWuHua();

		// 小选项[18]：是否允许同花牛：false-不允许，true-允许
		m_isTongHua = m_playtype.NiuNiu_NewAllowTypeTongHua();

		// 小选项[19]：是否允许葫芦牛：false-不允许，true-允许
		m_isHuLu = m_playtype.NiuNiu_NewAllowTypeHuLu();

		// 小选项[20]：是否允许炸弹牛：false-不允许，true-允许
		m_isZhaDan = m_playtype.NiuNiu_NewAllowTypeZhaDan();

		// 小选项[21]：是否允许五小牛：false-不允许，true-允许
		m_isWuXiao = m_playtype.NiuNiu_NewAllowTypeWuXiao();

		// 小选项[22]：是否允许快乐牛：false-不允许，true-允许
		m_isKuaiLe = m_playtype.NiuNiu_NewAllowTypeKuaiLe();
		// 小选项[23]：是否是急速模式：false-普通模式， true-极速模式
		m_fastStyle = m_playtype.NiuNiu_NewFastStyle();				


		//设置牛牛倍数选择
		m_gamelogic.SetNiuNiuTimes(m_niuNiuTimes);

		//设置是否允许癞子
		m_gamelogic.SetAllowLaiZi(m_isLaiZi);

		//设置允许的特殊牌型
		m_gamelogic.SetAllowSecialType(m_isBiYi, m_isShunZi, m_isWuHua, m_isTongHua, m_isHuLu, m_isZhaDan, m_isWuXiao, m_isKuaiLe);

		if (m_fastStyle)
		{
			m_time_qiang_zhuang = DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU_FAST;
			m_time_add_score = DELAY_TIME_ADD_SCORE_FAST;
			m_time_open_card = DELAY_TIME_OPEN_CARD_FAST;
		}
		else
		{
			m_time_qiang_zhuang = DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU;
			m_time_add_score = DELAY_TIME_ADD_SCORE;
			m_time_open_card = DELAY_TIME_OPEN_CARD;
		}

		//打印创建房间的游戏小选项
		std::stringstream ss;
		ss << "NiuNiu_NewGameHandler::SetPlayType() Run... This Desk Play Type List ... deskId=[%d], ";
		ss << "mainMode=[%d], playerNum=[%d], qiangZhuangTimes=[%d], addScoreTimes=[%d], autoStart=[%d], tuiZhuTimes=[%d], ";
		ss << "isDynamicIn=[%d], isAutoPlay=[%d], isLaiZi=[%d], isMaiMa=[%d], isCuoPai=[%d], isConstAddScore=[%d], isConstTuiZhu=[%d], ";
		ss << "niuNiuTimes=[%d], isKuaiLe=[%d], isZhaDan=[%d], isHuLu=[%d], isTongHua=[%d], isShunZi=[%d], isWuHua=[%d]";
		LLOG_ERROR(ss.str().c_str(), m_desk ? m_desk->GetDeskId() : 0,
			m_mainMode, m_playerNum, m_qiangZhuangTimes, m_addScoreTimes, m_autoStart, m_tuiZhuTimes,
			m_isDynamicIn, m_isAutoPlay, m_isLaiZi, m_isMaiMa, m_isCuoPai, m_isConstAddScore, m_isConstTuiZhu,
			m_niuNiuTimes, m_isKuaiLe, m_isZhaDan, m_isHuLu, m_isTongHua, m_isShunZi, m_isWuHua);
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
		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::MHSetDeskPlay() error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		LLOG_DEBUG("NiuNiu_NewGameHandler::MHSetDeskPlay() Run... deskId=[%d], playerCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

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
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuSelsectZhuang() error!!!, status not GAME_PLAY_SELECT_ZHUANG!!!, deskId=[%d], userId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuSelsectZhuang() error!!! Player pos is invalid!!! deskId=[%d], userId=[%d], playStatus=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, m_play_status);

			return false;
		}
		//只有看牌抢庄这个玩法才会走这个
		return on_event_user_select_zhuang(GetUserPos(pUser), msg->m_qingScore);
	}

	/* 处理牛牛玩家买码 */
	bool HanderNiuNiuMaiMa(User* pUser, NiuNiuC2SMaiMa* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GAME_PLAY_MAI_MA != m_play_status)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuMaiMa() Error!!! Game Status is not GAME_PLAY_MAI_MA!!! deskId=[%d], userId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuMaiMa() Error!!! Player Pos is invalid!!! deskId=[%d], userId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		return on_event_user_mai_ma(GetUserPos(pUser), msg->m_maiPos, msg->m_maiScore);

	}

	/* 处理牛牛玩家下注 */
	bool HanderNiuNiuAddScore(User* pUser, NiuNiuC2SAddScore* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuAddScore() error!!! player pos is invalid!!! deskId=[%d], userId=[%d], playStatus=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, m_play_status);

			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status && !(GAME_PLAY_MAI_MA == m_play_status && m_player_maima[GetUserPos(pUser)][0] != -1))
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuAddScore() error!!! status not GAME_PLAY_ADD_SCORE!!! deskId=[%d], userId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}
		
		return on_event_user_add_score(GetUserPos(pUser), msg->m_score, msg->m_isAddScoreDouble, msg->m_isTuiScore);

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
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuOpenCard() Error..., status not GAME_PLAY_SEND_CARD, deskId=[%d], user[%d], palyStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuOpenCard() Error..., player pos is invalid, deskId=[%d], userId[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, m_play_status);

			return false;
		}
		if (m_mainMode == MAIN_MODE_GONG_PAI_NIU_NIU && t_pos != m_open_gong_pos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuOpenCard() Error..., open gong pai player pos is invalid, deskId=[%d], userId[%d], pos=[%d], gongPos=[%d], mainMode=[&d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, t_pos, m_open_gong_pos, m_mainMode, m_play_status);
			return false;
		}

		return on_event_user_open_card(GetUserPos(pUser));
	}

	/*
	牛牛：玩家搓公牌
	MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
	*/
	bool HandNiuNiuCuoGongPai(User* pUser, NiuNiuC2SCuoGongPai* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuCuoGongPai() Error!!! The user or desk is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuCuoGongPai() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}

		return on_event_user_cuo_gong_card(GetUserPos(pUser), msg->m_cuoPai);
	}
	/*
	牛牛：玩家选择托管消息
	MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
	*/
	bool HandNiuNiuTuoGuan(User* pUser, NiuNiuC2STuoGuan* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! The user or desk is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}

		return on_event_user_tuo_guan(GetUserPos(pUser), msg->m_qiangScore, msg->m_addScore, msg->m_tuiScore);
	}

	/*
	牛牛：玩家取消托管消息
	MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
	*/
	bool HandNiuNiuCancelTuoGuan(User* pUser, NiuNiuC2SCancelTuoGuan* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! The user or desk is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}
		return on_event_user_cancel_tuo_guan(GetUserPos(pUser));
	}

	/*
	客户端通知服务器开始计时
	MSG_C_2_S_START_TIME = 61198
	*/
	bool HandNiuNiuStartTime(User* pUser, NiuNiuC2SStartTime* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuStartTime() Error!!! The user or desk is NULL...");
			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuStartTime() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}

		return on_event_start_time(GetUserPos(pUser));
	}

	//获取牌局中参加的真是人数
	Lint GetRealPlayerCount() 
	{ 
		return m_real_player_count; 
	}

	/* 处理玩家断线重连 */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnUserReconnect() Error... The user or desk is NULL...");
			return;
		}

		LLOG_ERROR("NiuNiuGameHandle::OnUserReconnect() Run... deskId=[%d], reconnUserId=[%d], curPos=[%d], playStatus=[%d]",
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


		Lint play_delay_time = 0;
		NiuNiuS2CRecon reconn;
		reconn.m_playStatus = m_play_status;
		memcpy(reconn.m_playerTuiScore, m_player_tui_score, sizeof(reconn.m_playerTuiScore));
		memcpy(reconn.m_user_status, this->m_user_status, sizeof(m_user_status));
		memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
		memcpy(reconn.m_addScore, m_play_add_score, sizeof(m_play_add_score));
		memcpy(reconn.m_playerAddScoreDouble, m_canAddScoreDouble, sizeof(reconn.m_playerAddScoreDouble));
		memcpy(reconn.m_addScoreDoubleStatus, m_add_score_double_status, sizeof(reconn.m_addScoreDoubleStatus));
		memcpy(reconn.m_playerXiaScore, m_canXiaMinZhu, sizeof(reconn.m_playerXiaScore));
		memcpy(reconn.m_isCanTui, m_canTuiZhu, sizeof(reconn.m_isCanTui));
		if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
		{
			reconn.m_isCoinsGame = 1;
			memcpy(reconn.m_totleScore, m_accum_coins, sizeof(reconn.m_totleScore));
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_desk->m_user[i] && m_desk->m_playStatus[i] == 2 && m_user_status[i] == 0)
				{
					reconn.m_totleScore[i] = m_desk->GetUserCoins(m_desk->m_user[i]);
				}
			}
		}

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
			LLOG_DEBUG("NiuNiu_NewGameHandler::OnUserReconnect()  1 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
		}

		switch (m_play_status)
		{
			//选庄阶段断线
		case GAME_PLAY_SELECT_ZHUANG:
		{
			reconn.m_noQiangZhuang = pos != INVAILD_POS_QIPAI ? m_noQiangZhuang[pos] : 0;

			switch (m_mainMode)
			{
				//牛牛上庄
			case MAIN_MODE_NIUNIU_SHANG_ZHUANG:
			{
				reconn.m_hasSelectZhuang = 1;
				reconn.m_zhuangPos = m_zhuangPos;
				break;
			}
			//自由抢庄
			case MAIN_MODE_ZI_YOU_QIANG_ZHUANG:
			{
				reconn.m_hasSelectZhuang = 0;
				if (NIUNIU_INVAILD_POS > pos && m_play_qiang_zhuang[pos] >= 0)
				{
					//该用户抢庄押注阶段已经操作过了
					reconn.m_hasSelectZhuang = 1;
				}
				play_delay_time = m_time_qiang_zhuang;
				break;
			}
			//八人明牌、明牌抢庄、公牌牛牛
			case MAIN_MODE_BA_REN_MING_PAI:
			case MAIN_MODE_MING_PAI_QIANG_ZHUANG:
			case MAIN_MODE_GONG_PAI_NIU_NIU:
			{
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
				play_delay_time = m_time_qiang_zhuang;
				break;
			}

			}
			break;
		}

		//买码阶段断线
		case GAME_PLAY_MAI_MA:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				reconn.m_maiMaPos[i] = m_player_maima[i][1];
				reconn.m_maiMaScore[i] = m_player_maima[i][0];
				if (m_player_maima[i][0] != -1)
				{
					reconn.m_hasMaiMa[i] = 1;
				}
				if (m_play_add_score[i] != 0)
				{
					//表示已经下过注了
					reconn.m_hasScore[i] = 1;
				}
			}
			//如果不是牛牛上庄和自由抢庄 玩家的前4张牌推给该玩家
			if (MAIN_MODE_NIUNIU_SHANG_ZHUANG != m_mainMode && MAIN_MODE_ZI_YOU_QIANG_ZHUANG != m_mainMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}
			//买码和下注公用一个时间
			play_delay_time = m_add_score_delay_time;
			break;
		}

		//下注阶段断线
		case GAME_PLAY_ADD_SCORE:
		{
			reconn.m_zhuangPos = m_zhuangPos;

			for (Lint i = 0; i < m_player_count; i++)
			{
				reconn.m_maiMaPos[i] = m_player_maima[i][1];
				reconn.m_maiMaScore[i] = m_player_maima[i][0];
				if (m_player_maima[i][0] != -1)
				{
					reconn.m_hasMaiMa[i] = 1;
				}

				if (m_play_add_score[i] != 0)
				{
					//表示已经下过注了
					reconn.m_hasScore[i] = 1;
				}
			}
			//如果不是牛牛上庄和自由抢庄 玩家的前4张牌推给该玩家
			if (MAIN_MODE_NIUNIU_SHANG_ZHUANG != m_mainMode && MAIN_MODE_ZI_YOU_QIANG_ZHUANG != m_mainMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}
			play_delay_time = m_add_score_delay_time;
			break;
		}
		//发牌 or 开牌
		case GAME_PLAY_SEND_CARD:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			reconn.m_openGongPos = m_open_gong_pos;
			reconn.m_gongCard = m_gong_card;
			reconn.m_cuoPai = m_cuo_pai;
			if (pos != NIUNIU_INVAILD_POS)
			{
				reconn.m_player_ox[pos] = m_player_oxnum[pos];
				reconn.m_player_oxnum[pos] = m_player_oxnum[pos];
				reconn.m_oxTimes[pos] = m_player_oxtimes[pos];
			}

			for (Lint i = 0; i < m_player_count; i++)
			{
				reconn.m_maiMaPos[i] = m_player_maima[i][1];
				reconn.m_maiMaScore[i] = m_player_maima[i][0];
				if (m_player_maima[i][0] != -1)
				{
					reconn.m_hasMaiMa[i] = 1;
				}

				if (m_play_add_score[i] != 0)
				{
					//表示已经下过注了
					reconn.m_hasScore[i] = 1;
				}
			}

			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1) continue;

				reconn.m_hasOpenCard[i] = m_isOpenCard[i] ? 1 : 0;
				if (pos == i) // 自己的手牌
				{
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
						reconn.m_showHandCard[i][card] = m_show_hand_card[i][card];
					}
				}

				if (m_isOpenCard[i]) // 其他玩家手牌
				{
					reconn.m_player_ox[i] = m_player_oxnum[i];
					reconn.m_player_oxnum[i] = m_player_oxnum[i];
					reconn.m_oxTimes[i] = m_player_oxtimes[i];
					//其他点击开牌的玩家
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
						reconn.m_showHandCard[i][card] = m_show_hand_card[i][card];
					}
				}
				else if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
				{
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX - 1; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
					}
				}
			}
			play_delay_time = m_time_open_card;
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

			reconn.m_remainTime = time_remain;
			LLOG_ERROR("****断线重连desk_id=[%d], OnUserReconnect play_status:%d , delay:%d , escaped:%d  remain:%d",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status, play_delay_time, time_escaped, time_remain);

		}
		if (NIUNIU_INVAILD_POS > pos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnUserReconnect()  2 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			pUser->Send(reconn);
		}
		else if (m_desk->MHIsLookonUser(pUser))
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnUserReconnect()  3 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			pUser->Send(reconn);
		}
	}

	void LookUserReconnectOnDeskWait(User* pUser) 
	{
		if (!pUser)
		{

			return;
		}
		NiuNiuS2CResult lookWaitResult;
		if (m_desk && m_desk->m_vip)
		{
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				memcpy(lookWaitResult.m_totleScore, m_desk->m_vip->m_coins, sizeof(lookWaitResult.m_totleScore));
			}
			else
			{
				memcpy(lookWaitResult.m_totleScore, m_desk->m_vip->m_score, sizeof(lookWaitResult.m_totleScore));
			}
		}
		lookWaitResult.m_zhuangPos = m_zhuangPos;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				lookWaitResult.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
				lookWaitResult.m_showHandCard[i][j] = (Lint)m_show_hand_card[i][j];
			}
		}
		memcpy(lookWaitResult.m_playScore, m_player_score, sizeof(lookWaitResult.m_playScore));
		memcpy(lookWaitResult.m_oxNum, m_player_oxnum, sizeof(lookWaitResult.m_oxNum));
		memcpy(lookWaitResult.m_oxTimes, m_player_oxtimes, sizeof(lookWaitResult.m_oxTimes));
		memcpy(lookWaitResult.m_playerStatus, m_user_status, sizeof(lookWaitResult.m_playerStatus));

		pUser->Send(lookWaitResult);
	}

	Lint GetPlayerAccumCoins(User* pUser)
	{
		if (!pUser)
		{

			return -100000;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos != NIUNIU_INVAILD_POS)
		{
			return m_accum_coins[t_pos];
		}
		else
		{
			return pUser->m_userData.m_coins;
		}
	}
	/* 处理游戏结束 */
	void OnGameOver(Lint result, Lint winpos, Lint bombpos)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnGameOver() Error... This desk is NULL");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d",
			m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);

		//解散房间，游戏结束
		finish_round(1);
	}

	/* 定时器 */
	void Tick(LTime& curr)
	{
		//桌子对象为空
		if (m_desk == NULL)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::Tick() Error... This is desk NULL... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		//没有开启超时自动操作选项  或  定时器有效
		/*if (m_isAutoPlay == false || m_tick_flag == false)
		{
		return;
		}*/

		//房间解散中，定时器不做处理
		if (m_desk && m_desk->m_resetTime > 0 && m_desk->m_resetUserId > 0)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::Tick() The desk is reseting... So tick do nothing...", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::Tick() Run... deskId=[%d], playStatus=[%d], autoPlay=[%d], tickFlag=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_play_status, m_isAutoPlay, m_tick_flag);

		switch (m_play_status)
		{

			//选庄阶段
		case GAME_PLAY_SELECT_ZHUANG:
		{
			switch (m_mainMode)
			{
				//轮庄模式
			case MAIN_MODE_NIUNIU_SHANG_ZHUANG:
			{

				break;
			}
			//自由抢庄、十人明牌、明牌抢庄、公牌牛牛
			case MAIN_MODE_ZI_YOU_QIANG_ZHUANG:
			case MAIN_MODE_BA_REN_MING_PAI:
			case MAIN_MODE_MING_PAI_QIANG_ZHUANG:
			case MAIN_MODE_GONG_PAI_NIU_NIU:
			{
				LLOG_DEBUG("NiuNiu_NewGameHandler::Tick() Run... Tick in Select Zhuang Status, Now is Qiang Zhuang... deskId=[%d], playStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_play_status);

				if (curr.Secs() >= m_play_status_time.Secs() + m_time_qiang_zhuang)
				{
					for (Lint i = 0; i < m_player_count; i++)
					{
						if (m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
						{
							LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TimeOverRun... Qiang Zhuang Time over!!!, Auto Qiang Zhuang... deskId=[%d], pos=[%d]",
								m_desk ? m_desk->GetDeskId() : 0, i);

							on_event_user_select_zhuang(i, 0);
						}
					}
					//执行完倒计时，禁用倒计时
					//m_tick_flag = false;
				}
				//托管执行
				else
				{
					for (Lint i = 0; i < m_player_count; ++i)
					{
						if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
						{
							LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Qiang Zhuang... deskId=[%d], pos=[%d]",
								m_desk ? m_desk->GetDeskId() : 0, i);

							on_event_user_select_zhuang(i, tuoguan_qiang_zhuang(i));
						}
					}
				}
				break;
			}

			}

			break;
		}

		//买码阶段
		case GAME_PLAY_MAI_MA:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_add_score_delay_time - 1)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_player_maima[i][0] == -1 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TimeOverRun... Mai Ma Time over!!!, Auto Mai Ma... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_mai_ma(i, NIUNIU_INVAILD_POS, 0);
					}
				}
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_player_maima[i][0] == -1 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Mai Ma... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_mai_ma(i, NIUNIU_INVAILD_POS, 0);
					}
				}
			}

			break;
		}

		//下注阶段
		case GAME_PLAY_ADD_SCORE:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_add_score_delay_time)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_play_add_score[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TimeOverRun... Add Score Time over!!!, Auto Add Score... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_add_score(i, min_add_score(i), 0, 0);
					}
				}
				//执行完倒计时，禁用倒计时
				//m_tick_flag = false;
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_play_add_score[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Add Score... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);
						int t_tuoGuanTuiScore = tuoguan_tui_score(i);
						int t_tuoGuanAddScore = tuoguan_add_score(i);
						int t_addScore = t_tuoGuanAddScore > t_tuoGuanTuiScore ? t_tuoGuanAddScore : t_tuoGuanTuiScore;
						int t_isTuiScore = t_tuoGuanAddScore > t_tuoGuanTuiScore ? 0 : 1;
						on_event_user_add_score(i, t_addScore, 0, t_isTuiScore);
					}
				}
			}

			break;
		}

		//开牌阶段
		case GAME_PLAY_SEND_CARD:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_time_open_card)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_play_status != GAME_PLAY_SEND_CARD) break;

					if (m_isOpenCard[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_DEBUG("NiuNiu_NewGameHandler::Tick() TimeOverRun... Open Card Time over!!!, Auto Open Card... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_open_card(i);
					}
				}
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_play_status != GAME_PLAY_SEND_CARD) break;

					if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_isOpenCard[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Open Card... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_open_card(i);
					}
				}
			}

			break;
		}

		}//switch (m_play_status) end...
	}
};

DECLARE_GAME_HANDLER_CREATOR(108, NiuNiu_NewGameHandler);