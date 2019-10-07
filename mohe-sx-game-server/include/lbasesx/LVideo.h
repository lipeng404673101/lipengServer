//查牌功能


#ifndef _L_VIDEO_H_
#define _L_VIDEO_H_

#include "LTool.h"
#include "LBuff.h"
#include "GameDefine.h"

struct CardValue
{
	Lint m_color;
	Lint m_number;

	MSGPACK_DEFINE(m_color, m_number); 
	CardValue()
	{
		m_color = 0;
		m_number = 0;
	}
	CardValue(int color, int number) {
		m_color = color;
		m_number = number;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_color), m_color);
		ReadMapData(obj, NAME_TO_STR(m_number), m_number);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_color), m_color);
		WriteKeyValue(pack, NAME_TO_STR(m_number), m_number);
		return true;
	}
};

// 胡牌信息
struct HuInfo
{
	WIN_TYPE_SUB type;	// 胡牌的类型
	Lint abombpos;		// 如果是点炮胡，表示放炮的位置
	std::vector<CardValue> wincards;	// 玩家胡的牌 理论上只有一张 目前只有倒倒胡中使用 其他逻辑还是用m_winCard 会逐渐的用这里的wincards替换掉
	std::vector<Lint> hu_types;

	HuInfo( WIN_TYPE_SUB t = WIN_SUB_NONE, Lint abomb = INVAILD_POS )
		: type(t)
		, abombpos(abomb)
	{
	}

	void Clear()
	{
		type = WIN_SUB_NONE;
		abombpos = INVAILD_POS;
		wincards.clear();
		hu_types.clear();
	}
};

enum VIDEO_OPER
{
	VIDEO_OPER_GET_CARD		= 1,		//摸牌
	VIDEO_OPER_OUT_CARD		= 2,		//出牌
	VIDEO_OPER_AN_GANG		= 3,		//暗杠
	VIDEO_OPER_MING_GANG	= 4,		//我出牌明杠
	VIDEO_OPER_PENG_CARD	= 5,		//碰牌
	VIDEO_OPER_GANG			= 6,		//别人出牌，我杠
	VIDEO_OPER_SHOUPAO		= 7,		//收炮
	VIDEO_OPER_ZIMO			= 8,		//自摸
	VIDEO_OPER_HUANGZHUANG	= 9,		//流局
	VIDEO_OPER_EAT			= 10,		//吃
	VIDEO_OPER_SELF_BU		= 11,		//明补自已的
	VIDEO_OPER_OTHER_BU		= 12,		//明补他人的
	VIDEO_OPER_AN_BU		= 13,		//暗补
	
	VIDEO_OPEN_THINK		=21,		//玩家思想
	VIDEO_OPEN_DOING		=22,		//玩家决定

	VIDEO_OPER_QIANGGANGHU	= 60,		//抢杠胡
	VIDEO_OPER_DISS			= 61,		// 散桌
	VIDEO_OPER_ZIMO2		= 62,		//抢杠胡 点杠花自摸
	VIDEO_OPER_FANPIGU_CARDS= 63,		//翻屁股的两张牌
	VIDEO_OPER_TANG			= 64,		//躺牌

	VIDEO_Oper_Haozi		= 70,		//看耗子

};

struct VideoOper
{
	Lint		m_pos;
	Lint		m_oper;
	std::vector<CardValue> m_cards;		//第一个为目标牌（吃，碰，等），后面的是已有的牌

	MSGPACK_DEFINE(m_pos,m_oper,m_cards); 
	VideoOper()
	{
		m_pos = 0;
		m_oper = 0;
		m_cards.clear();
	}

	Lstring		ToString()
	{
		std::stringstream ss;
		ss << m_pos << ",";
		ss << m_oper << ",";
		for (size_t i = 0; i< m_cards.size(); i++)
		{
			ss << m_cards[i].m_color << "|" << m_cards[i].m_number;
			if (i < m_cards.size() - 1)
			{
				ss << ",";
			}
		}
		ss << ";";
		return ss.str();
	}

	void	FromString(Lstring str)
	{
		std::vector<Lstring> des;
		L_ParseString(str, des, ",");
		if (des.size() >= 2)
		{
			m_pos = atoi(des[0].c_str());
			m_oper = atoi(des[1].c_str());
			std::vector<Lstring> splids;
			for (size_t i = 2; i < des.size(); i ++)
			{
				L_ParseString(des[i], splids, "|");
				if (splids.size() >= 2)
				{
					CardValue card;
					card.m_color = atoi(splids[0].c_str());
					card.m_number = atoi(splids[1].c_str());
					m_cards.push_back(card);
				}
			}
		}
	}
};

class API_LBASESX VideoLog
{
public:
	VideoLog();
	virtual~VideoLog();
	void		AddOper(Lint oper,Lint pos, std::vector<CardValue>& cards);
	void		DealCard(Lint* userId, std::vector<CardValue>* vec, Lint time,Lint zhuang,Lint* score,Lint deskId,Lint cur,Lint max,Lint flag, const std::vector<Lint>& playType);

	Lstring		ToString();

	void		FromString(Lstring str);

	void		Write(LBuff& buff);

	void		Read(LBuff& buff);

	void		Clear();

	Lstring		GetInsertSql();

	Lstring		PlayTypeToStrint();

	void		PlayTypeFromString(const Lstring& str);
public:
	Lint							m_time;
	Lstring							m_Id;
	Lint							m_zhuang;
	Lint							m_deskId;
	Lint							m_curCircle;
	Lint							m_maxCircle;
	Lint							m_score[4];
	Lint							m_userId[4];
	Lint							m_flag;//牌局玩法
	std::vector<CardValue>			m_handCard[4];
	std::vector<VideoOper>			m_oper;
	std::vector<Lint>				m_playType;
	Lstring							m_str;
	Lstring							m_str_playtype;
};


#endif