#pragma once



#include "LMsg.h"


/*
LMsgL2C.h 用于存放仅用于 LogicServer 发给 Client 的消息，这类消息具有以下特点：
1、不被其它 Server 引用
2、不需要注册到 LMsgFactory

*/



// 方便 msgpack 序列化 4 个 int 的数组
struct DeskIntArray {
	int kData[4];
	DeskIntArray() { memset(this, 0, sizeof(*this)); }
	int &operator [](int pos) { return kData[pos & 0x3]; } // 安全处理，避免越界
	operator int *() { return kData; } // 能隐式转成指针
	MSGPACK_DEFINE_ARRAY(kData[0], kData[1], kData[2], kData[3]);
};


struct TingChuKouUnit {
	unsigned char kOutTile;
	// 用 std::vector<unsigned char> 会被自动打成字符串
	std::vector<unsigned short> kTingKou;
	MSGPACK_DEFINE_MAP(kOutTile, kTingKou);
	TingChuKouUnit() : kOutTile(0) {}
};

// 修改客户端消息内容
struct LMsgS2COutCard :public LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2COutCard(); }
	virtual bool Write(msgpack::packer<msgpack::sbuffer> &packer) { packer.pack(*this); return true; }

	MSGPACK_DEFINE_MAP(kMId, kPos, kTime, kDCount, kColor, kNumber, kFlag, kTingCards
		, kThink, kTing, /*kCheckTing,*/ kGang, kEnd, kBOnlyHu, kTingChuKou);
	Lint        kMId;
	Lint		kPos;//当前出牌中的玩家
	Lint		kTime;//出牌倒计时
	Lint		kDCount;//桌面剩余的牌
	Lint		kFlag;//当前是否需要摸牌，0-需要，1-不需要
	Lint		kGang;//是否杠过，0-没有，1-杠过
	Lint		kEnd;//是否海底 0-不是，1-是
	// 摸上来的牌，这个只有 m_pos == mypos m_flag == 0 的时候才有
	// 之所以是两个字段，是因为之前对客户端的协议就是这样
	//CardValue	m_curCard;//摸上来的牌，这个只有 m_pos == mypos m_flag == 0 的时候才有
	Lint kColor;
	Lint kNumber;
	void SetCurCard(CardValue const &card) {
		kColor = card.kColor;
		kNumber = card.kNumber;
	}
	CardValue GetCurCard() const {
		return CardValue(kColor, kNumber);
	}
	bool		kBOnlyHu;		// 最后四张有胡必胡
	std::vector<CardValue> kTingCards;//听牌状态下可出的牌
	std::vector<ThinkData> kThink;//思考的内容
	Lint		kTing;//是否听状态，0-非听，1-听
	//Lint        kCheckTing;  //用于不用报听玩法中查听（**Ren 2017-*12-15）(暂时不启用)
	std::vector<TingChuKouUnit> kTingChuKou; // 可听口：表示刚点了“听”按钮后，可以出的牌的集合

	LMsgS2COutCard() : LMsgSC(MSG_S_2_C_GET_CARD) {
		kPos = INVAILD_POS;
		kTime = 0;
		kDCount = 0;
		kFlag = 0;
		kGang = 0;
		kEnd = 0;
		//kCheckTing = 0;
		kBOnlyHu = false;
		kMId = m_msgId;
	}

};



//////////////////////////////////////////////////////////////////////////
//服务器广播游戏结束
struct LMsgS2CGameOver : LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2CGameOver(); }

	Lint		kResult;//0-自摸，1-点炮，2-慌庄
	DeskIntArray kWin;//4家胡牌情况，0-没胡，1-自摸，2-收炮，3-点炮, 7-有 叫
	DeskIntArray kScore;//东 南 西 北 各家输赢积分，这里需要更新玩家积分
	DeskIntArray kCoins;//金币
	DeskIntArray kTotalcoins;//玩家结算完的总金币
	DeskIntArray kAgang;//暗杠数量
	DeskIntArray kMgang;//明杠数量 别人点的
	DeskIntArray kMbgang;//巴杠(补杠)的数量
	DeskIntArray kDgang;//点别人杠的数量
	DeskIntArray kGsgang;//过手杠数量
	DeskIntArray kStartHu;//起手胡牌
	DeskIntArray kTing;//是否停牌
	std::vector<Lint> &m_hu(int pos) { return (&kHu1)[pos]; } // 各个玩家的胡牌番型
	std::vector<Lint> kHu1;
	std::vector<Lint> kHu2;
	std::vector<Lint> kHu3;
	std::vector<Lint> kHu4;

	std::vector<Lint> &m_dian(int pos) { return (&kDian1)[pos]; } // 点炮类型
	std::vector<Lint> kDian1;
	std::vector<Lint> kDian2;
	std::vector<Lint> kDian3;
	std::vector<Lint> kDian4;


	std::vector<Lint>       kPlayType;     //桌子类型
	std::vector<Lint>		kHuWay[4];		//和牌方式

	std::vector<Lint> &m_myFan(int pos) { return (&kMyFan1)[pos]; } //4x4 赢的玩家(绵阳为番数)(宜宾为分数)
	std::vector<Lint> kMyFan1;
	std::vector<Lint> kMyFan2;
	std::vector<Lint> kMyFan3;
	std::vector<Lint> kMyFan4;


	DeskIntArray                    kYbPiao;
	std::vector<CardValue>	kHunCard;

	std::vector<Lint>		kGangScore;
	std::vector<Lint>		kHuScore;//胡牌的数组
									  //四家手上的牌
	DeskIntArray		kCount;
	CardValue	kCard[4][14];

	std::vector<CardValue> &m_hand_cards(int pos) { return (&kArray0)[pos]; } // 各个玩家的手牌
	std::vector<CardValue> kArray0;
	std::vector<CardValue> kArray1;
	std::vector<CardValue> kArray2;
	std::vector<CardValue> kArray3;

	//玩家躺下的牌
	DeskIntArray		kTangNum;
	CardValue	kTangCard[4][14];
	std::vector<CardValue> &m_tang_cards(int pos) { return (&kTangCard0)[pos]; } // 玩家躺下的牌
	std::vector<CardValue> kTangCard0;
	std::vector<CardValue> kTangCard1;
	std::vector<CardValue> kTangCard2;
	std::vector<CardValue> kTangCard3;

	std::vector<CardValue> &m_hucards(int pos) { return (&kHucards1)[pos]; }
	std::vector<CardValue> kHucards1;
	std::vector<CardValue> kHucards2;
	std::vector<CardValue> kHucards3;
	std::vector<CardValue> kHucards4;

	Lint		kEnd;
	DeskIntArray		kFan;
	DeskIntArray		kGen;
	DeskIntArray		kWinList;
	Lint		kMaxFan;	// 封顶番数
	DeskIntArray		kCheckTing;		// 查听
	DeskIntArray		kRewardCardNum;		//胡牌奖励房卡数
	DeskIntArray		kRewardHu;			//胡牌被奖励的牌型
	DeskIntArray		kChangeScore;		//买牌的分

	Lint		kUser_count;

	// 贴金新增金分字段
	DeskIntArray kJinScore;
	DeskIntArray kJinNum;


	LMsgS2CGameOver() :LMsgSC(MSG_S_2_C_GAME_OVER)
	{
		kResult = -1;
		for (int i = 0; i < 4; ++i)
		{
			kWin[i] = 0;
			kScore[i] = 0;
			kCoins[i] = 0;
			kTotalcoins[i] = 0;
			kAgang[i] = 0;
			kMgang[i] = 0;
			kMbgang[i] = 0;
			kDgang[i] = 0;
			kGsgang[i] = 0;
			kStartHu[i] = 0;
			kCount[i] = 0;

			kFan[i] = 0;
			kGen[i] = 0;
			kWinList[i] = 0;
			kCheckTing[i] = 0;
			kTangNum[i] = 0;
			kYbPiao[i] = 0;
			kRewardCardNum[i] = 0;
			kRewardHu[i] = 0;
			kChangeScore[i] = 0;
			kTing[i] = 0;
		}
		kEnd = 0;
		kMaxFan = 0;
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
		pack.pack("kMId"); pack.pack(m_msgId);
		A(kResult); A(kEnd); A(kGangScore); A(kHuScore); A(kWin);
		//A(m_msgId); A(m_result); A(m_end); A(m_gangScore); A(m_huScore); A(m_win);
		A(kYbPiao);
		//17
		A(kHu1); A(kHu2); A(kHu3); A(kHu4);
		A(kMyFan1); A(kMyFan2); A(kMyFan3); A(kMyFan4);
		A(kDian1); A(kDian2); A(kDian3); A(kDian4);
		A(kGsgang);
		A(kHucards1); A(kHucards2); A(kHucards3); A(kHucards4);
		//9
		A(kPlayType); A(kAgang); A(kStartHu); A(kMgang); A(kMbgang); A(kDgang); A(kScore);
		A(kCoins); A(kTotalcoins);
		//12
		// 这些名字都要对客户端保持兼容的
		A(kArray0); A(kArray1); A(kArray2); A(kArray3);
		A(kTangCard0); A(kTangCard1); A(kTangCard2); A(kTangCard3);
		A(kFan); A(kGen); A(kWinList); A(kCheckTing);
		//6
		A(kRewardCardNum); A(kRewardHu);
		A(kHunCard); A(kMaxFan); A(kChangeScore); A(kTing);
		//2
		A(kJinScore);
		A(kJinNum);
#undef A
		return true;

	}

};


