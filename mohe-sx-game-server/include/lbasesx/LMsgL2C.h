#pragma once



#include "LMsg.h"


/*
LMsgL2C.h 用于存放仅用于 LogicServer 发给 Client 的消息，这类消息具有以下特点：
1、不被其它 Server 引用
2、不需要注册到 LMsgFactory

*/



// 方便 msgpack 序列化 4 个 int 的数组
struct DeskIntArray {
	int data[4];
	DeskIntArray() { memset(this, 0, sizeof(*this)); }
	int &operator [](int pos) { return data[pos & 0x3]; } // 安全处理，避免越界
	operator int *() { return data; } // 能隐式转成指针
	MSGPACK_DEFINE_ARRAY(data[0], data[1], data[2], data[3]);
};


struct TingChuKouUnit {
	unsigned char m_outTile;
	// 用 std::vector<unsigned char> 会被自动打成字符串
	std::vector<unsigned short> m_tingKou;
	MSGPACK_DEFINE_MAP(m_outTile, m_tingKou);
	TingChuKouUnit() : m_outTile(0) {}
};

struct LMsgS2COutCard :public LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2COutCard(); }
	virtual bool Write(msgpack::packer<msgpack::sbuffer> &packer) { packer.pack(*this); return true; }

	MSGPACK_DEFINE_MAP(m_msgId, m_pos, m_time, m_dCount, m_color, m_number, m_flag, m_tingCards
		, m_think, m_ting, m_gang, m_end, m_bOnlyHu, m_tingChuKou);

	Lint		m_pos;//当前出牌中的玩家
	Lint		m_time;//出牌倒计时
	Lint		m_dCount;//桌面剩余的牌
	Lint		m_flag;//当前是否需要摸牌，0-需要，1-不需要
	Lint		m_gang;//是否杠过，0-没有，1-杠过
	Lint		m_end;//是否海底 0-不是，1-是
	// 摸上来的牌，这个只有 m_pos == mypos m_flag == 0 的时候才有
	// 之所以是两个字段，是因为之前对客户端的协议就是这样
	//CardValue	m_curCard;//摸上来的牌，这个只有 m_pos == mypos m_flag == 0 的时候才有
	Lint m_color;
	Lint m_number;
	void SetCurCard(CardValue const &card) {
		m_color = card.m_color;
		m_number = card.m_number;
	}
	CardValue GetCurCard() const {
		return CardValue(m_color, m_number);
	}
	bool		m_bOnlyHu;		// 最后四张有胡必胡
	std::vector<CardValue> m_tingCards;//听牌状态下可出的牌
	std::vector<ThinkData> m_think;//思考的内容
	Lint		m_ting;//是否听状态，0-非听，1-听
	Lint        m_check_ting;  //不用听牌的玩法里面添加查听功能，0：不需要查听，1：需要查听（注意不是真正听牌）（**Ren 2017-12-13）
	std::vector<TingChuKouUnit> m_tingChuKou; // 可听口：表示刚点了“听”按钮后，可以出的牌的集合

	LMsgS2COutCard() : LMsgSC(MSG_S_2_C_GET_CARD) {
		m_pos = INVAILD_POS;
		m_time = 0;
		m_dCount = 0;
		m_flag = 0;
		m_gang = 0;
		m_end = 0;
		m_check_ting = 0;
		m_bOnlyHu = false;
	}

};



//////////////////////////////////////////////////////////////////////////
//服务器广播游戏结束
struct LMsgS2CGameOver : LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2CGameOver(); }

	Lint		 m_result;//0-自摸，1-点炮，2-慌庄，4-明杠开花
	DeskIntArray m_win;//4家胡牌情况，0-没胡，1-自摸，2-收炮，3-点炮, 7-有 叫
	DeskIntArray m_score;//东 南 西 北 各家输赢积分，这里需要更新玩家积分
	DeskIntArray m_coins;//金币
	DeskIntArray m_totalcoins;//玩家结算完的总金币
	DeskIntArray m_agang;//暗杠数量
	DeskIntArray m_mgang;//明杠数量 别人点的
	DeskIntArray m_mbgang;//巴杠(补杠)的数量
	DeskIntArray m_dgang;//点别人杠的数量
	DeskIntArray m_gsgang;//过手杠数量
	DeskIntArray m_startHu;//起手胡牌
	DeskIntArray m_ting;//是否停牌
	std::vector<Lint> &m_hu(int pos) { return (&m_hu1)[pos]; } // 各个玩家的胡牌番型
	std::vector<Lint> m_hu1;
	std::vector<Lint> m_hu2;
	std::vector<Lint> m_hu3;
	std::vector<Lint> m_hu4;

	std::vector<Lint> &m_dian(int pos) { return (&m_dian1)[pos]; } // 点炮类型
	std::vector<Lint> m_dian1;
	std::vector<Lint> m_dian2;
	std::vector<Lint> m_dian3;
	std::vector<Lint> m_dian4;


	std::vector<Lint>       m_playType;     //桌子类型
	std::vector<Lint>		m_huWay[4];		//和牌方式

	std::vector<Lint> &m_myFan(int pos) { return (&m_myFan1)[pos]; } //4x4 赢的玩家(绵阳为番数)(宜宾为分数)
	std::vector<Lint> m_myFan1;
	std::vector<Lint> m_myFan2;
	std::vector<Lint> m_myFan3;
	std::vector<Lint> m_myFan4;


	DeskIntArray                    m_ybPiao;
	std::vector<CardValue>	m_hunCard;

	std::vector<Lint>		m_gangScore;
	std::vector<Lint>		m_huScore;//胡牌的数组
									  //四家手上的牌
	DeskIntArray		m_count;
	CardValue	m_card[4][14];

	std::vector<CardValue> &m_hand_cards(int pos) { return (&array0)[pos]; } // 各个玩家的手牌
	std::vector<CardValue> array0;
	std::vector<CardValue> array1;
	std::vector<CardValue> array2;
	std::vector<CardValue> array3;

	//玩家躺下的牌
	DeskIntArray		m_TangNum;
	CardValue	m_TangCard[4][14];
	std::vector<CardValue> &m_tang_cards(int pos) { return (&m_TangCard0)[pos]; } // 玩家躺下的牌
	std::vector<CardValue> m_TangCard0;
	std::vector<CardValue> m_TangCard1;
	std::vector<CardValue> m_TangCard2;
	std::vector<CardValue> m_TangCard3;

	std::vector<CardValue> &m_hucards(int pos) { return (&m_hucards1)[pos]; }
	std::vector<CardValue> m_hucards1;
	std::vector<CardValue> m_hucards2;
	std::vector<CardValue> m_hucards3;
	std::vector<CardValue> m_hucards4;

	Lint		m_end;
	DeskIntArray		m_fan;
	DeskIntArray		m_gen;
	DeskIntArray		m_winList;
	Lint		m_maxFan;	// 封顶番数
	DeskIntArray		m_checkTing;		// 查听
	DeskIntArray		m_rewardCardNum;		//胡牌奖励房卡数
	DeskIntArray		m_rewardHu;			//胡牌被奖励的牌型
	DeskIntArray		m_changeScore;		//买牌的分

	Lint		m_user_count;

	// 贴金新增金分字段
	DeskIntArray m_jinScore;
	DeskIntArray m_jinNum;


	LMsgS2CGameOver() :LMsgSC(MSG_S_2_C_GAME_OVER)
	{
		m_result = -1;
		for (int i = 0; i < 4; ++i)
		{
			m_win[i] = 0;
			m_score[i] = 0;
			m_coins[i] = 0;
			m_totalcoins[i] = 0;
			m_agang[i] = 0;
			m_mgang[i] = 0;
			m_mbgang[i] = 0;
			m_dgang[i] = 0;
			m_gsgang[i] = 0;
			m_startHu[i] = 0;
			m_count[i] = 0;

			m_fan[i] = 0;
			m_gen[i] = 0;
			m_winList[i] = 0;
			m_checkTing[i] = 0;
			m_TangNum[i] = 0;
			m_ybPiao[i] = 0;
			m_rewardCardNum[i] = 0;
			m_rewardHu[i] = 0;
			m_changeScore[i] = 0;
			m_ting[i] = 0;
		}
		m_end = 0;
		m_maxFan = 0;
	}

	/* 参数太多不能这么定义，最多支持16个吧
	MSGPACK_DEFINE_MAP(m_msgId, m_result, m_end, m_gangScore, m_huScore, m_win
	, m_hu1, m_hu2, m_hu3, m_hu4
	, m_hucards1, m_hucards2, m_hucards3, m_hucards4
	, m_playType, m_agang, m_mgang, m_dgang, m_score
	, array0, array1, array2, array3
	, m_rewardCardNum, m_rewardHu
	, m_hunCard, m_ting
	);
	*/

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		//pack.pack(*this);
		//return true;

		// map的字段数要核对准确，增删字段要同时修改这里
		pack.pack_map(51 + 2);
#define A(name) pack.pack(#name).pack(name)
		//7
		A(m_msgId); A(m_result); A(m_end); A(m_gangScore); A(m_huScore); A(m_win);
		A(m_ybPiao);
		//17
		A(m_hu1); A(m_hu2); A(m_hu3); A(m_hu4);
		A(m_myFan1); A(m_myFan2); A(m_myFan3); A(m_myFan4);
		A(m_dian1); A(m_dian2); A(m_dian3); A(m_dian4);
		A(m_gsgang);
		A(m_hucards1); A(m_hucards2); A(m_hucards3); A(m_hucards4);
		//9
		A(m_playType); A(m_agang); A(m_startHu); A(m_mgang); A(m_mbgang); A(m_dgang); A(m_score);
		A(m_coins); A(m_totalcoins);
		//12
		// 这些名字都要对客户端保持兼容的
		A(array0); A(array1); A(array2); A(array3);
		A(m_TangCard0); A(m_TangCard1); A(m_TangCard2); A(m_TangCard3);
		A(m_fan); A(m_gen); A(m_winList); A(m_checkTing);
		//6
		A(m_rewardCardNum); A(m_rewardHu);
		A(m_hunCard); A(m_maxFan); A(m_changeScore); A(m_ting);
		//2
		A(m_jinScore);
		A(m_jinNum);
#undef A
		return true;

	}

};


