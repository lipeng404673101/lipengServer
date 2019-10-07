#ifndef _CARD_H_
#define _CARD_H_


#include "LBase.h"
#include "GameDefine.h"
#include "ClientConstants.h"
#include "LVideo.h"

// 这个枚举不能变值了，因为在重连消息的 m_iTing 字段用到，会传给客户端
enum ting_state {
	Ting_Null, // 0: 不能听
	Ting_ChooseTing, // 1: 在选择听，估计是以前的项目定义的，现在没用了
	Ting_waitOutput, // 2: 选择听后，等待出牌确定
	Ting_waitHu, // 3: 听状态下，等待胡牌
};

enum GangThink {
	GangThink_qianggang,
	GangThink_gangshangpao,
	GangThink_over,
};

struct Card
{
	Lint m_number;	//牌的号码1-9
	Lint m_color;   //牌的花色1-3 1-万，2-筒，3-条

	Card(Lint number = 0,Lint color = 0)
	{
		m_number = number;
		m_color = color;
	}
	// 让Card和CardValue可以相互隐式转换
	Card(CardValue const &v): m_number(v.kNumber), m_color(v.kColor) {}
	char ToChar() const { return (char)(m_color*10 + m_number); }
	operator CardValue() const {
		CardValue v;
		v.kColor = m_color;
		v.kNumber = m_number;
		return v;
	}
	std::string ToString()
	{
		char text[16];
		text[0] = 0;
		switch (m_color)
		{
		case 1:
			_snprintf_s(text, 16, "%d%s", m_number, "W");
			break;
		case 2: 
			_snprintf_s(text, 16, "%d%s", m_number, "B");
			break;
		case 3: 
			_snprintf_s(text, 16, "%d%s", m_number, "T");
			break;
		case 4:	
			_snprintf_s(text, 16, "%d%s", m_number, "Z");
			break;
		default:
			break;
		}
		return std::string(text);
	}
};
typedef std::vector<Card*> CardVector;
inline bool operator <(const Card& lhs, const Card& rhs) { return lhs.ToChar() < rhs.ToChar(); }
inline bool operator ==(const Card& lhs, const Card& rhs) { return lhs.m_color == rhs.m_color && lhs.m_number == rhs.m_number; }
inline bool operator !=(const Card& lhs, const Card& rhs) { return !(lhs == rhs); }
// 11-19万 21-29饼 31-39条 41-47东南西北中发白
inline bool operator ==(const Card& lhs, char v) { return lhs.ToChar() == v; }
inline bool operator ==(char v, const Card& rhs) { return rhs == v; }
inline bool operator !=(const Card& lhs, char v) { return !(lhs == v); }
inline bool operator !=(char v, const Card& rhs) { return rhs != v; }
struct CardPtrEqualChar {
	bool operator ()(const Card* lhs, char v) { return lhs ? *lhs == v : Card() == v; }
	bool operator ()(char v, const Card* rhs) { return operator ()(rhs, v); }
};
struct CardPtrPredicate {
	Card expected;
	CardPtrPredicate(Card const &expected): expected(expected) {}
	bool operator ()(Card const *actual) { return actual && *actual == expected; }
};

struct HuUnit {
	Lint  m_hu_basicType;//基础胡牌型1.小胡，2夹胡
	Card* hu_card;
	Lint  hu_type;

	// 这俩字段用于抠点：最后胡的是个耗子，得计算那种胡法分值最高
	// 不过现在捉耗子不允许加清一色、一条龙了，这个暂时也就没必要加了，就按最后一张牌的点数比较吧
	unsigned long long fans;
	int score;
	//int zui_score;

	void clear() {
		hu_card = NULL;
		hu_type = THINK_OPERATOR_NULL;
		m_hu_basicType = 1;
		fans = 0;
		score = 0;
		//zui_score = 0;
	}
};

struct Hu_type
{
	Lint				m_hu_type;//胡牌的类型
	std::vector<Lint>	m_hu_way; //胡牌的方式 ：1:自摸胡 、2接炮胡
	Lint				m_hufan;	// 计算胡的番数
	Hu_type():m_hu_type(0),m_hufan(0)
	{
	}
	void clear()
	{
		m_hu_type = 0;
		m_hu_way.clear();
	}
};

struct TingCard
{
	TingCard()
		: m_hufan(0)
		, m_gen(0)
	{
	}

	Card m_card;
	std::vector<int> m_hu; // 胡的番型
	Lint m_hufan;	// 胡的番数 包括根和最大番限制
	Lint m_gen;		// 根的数量
};

struct TingUnit {
	Card* out_card;				 //出哪张牌可以听
	std::vector<HuUnit> hu_units;//出了这张牌后可以胡什么牌以及类型
	std::vector<TingCard> m_tingcard; //churunmin: Alt+Shift+F 查找发现无人引用
	inline TingUnit(): out_card(NULL) {}
};
typedef std::vector<TingUnit> TingVec;

struct ThinkUnit
{
	ThinkUnit():m_type(0)
	{
	}

	void Clear()
	{
		m_type = 0;
		m_card.clear();
		m_hu.clear();
		ting_unit.clear();
	}
	Lint m_type;
	CardVector	m_card;
	std::vector<Hu_type>    m_hu;
	std::vector<TingUnit>	ting_unit;//可听的牌以及听的类型
};
typedef std::vector<ThinkUnit> ThinkVec;

//struct ThinkUnit
//{
//	ThinkUnit()
//	{
//		m_type = THINK_OPERATOR_NULL;
//		m_gen  = 0;
//		m_isOnlyhu = false;
//	}
//
//	void Clear()
//	{
//		m_type = THINK_OPERATOR_NULL;
//		m_card.clear();
//		m_hu.clear();
//	}
//
//	int GetHuFan( Lint fanshu[HU_ALLTYPE] ) const
//	{
//		if ( m_type != THINK_OPERATOR_BOMB )
//		{
//			return 0;
//		}
//		Lint fan = 0;
//		for (Lsize k = 0; k < m_hu.size(); ++k)
//		{
//			if ( fanshu[m_hu[k]] > 0 )
//			{
//				fan += fanshu[m_hu[k]];
//			}
//		}
//		return fan;
//	}
//	// 获取最大的番
//	int GetMaxFan( Lint fanshu[HU_ALLTYPE] ) const
//	{
//		if ( m_type != THINK_OPERATOR_BOMB )
//		{
//			return 0;
//		}
//		Lint fan = 0;
//		for (Lsize k = 0; k < m_hu.size(); ++k)
//		{
//			if ( fan < fanshu[m_hu[k]] )
//			{
//				fan = fanshu[m_hu[k]];
//			}
//		}
//		return fan;
//	}
//
//	bool HasQianggh()
//	{
//		for(Lsize i = 0; i < m_hu.size(); ++i)
//		{
//			if(m_hu.at(i) == HU_QIANGGANGHU)
//			{
//				return true;
//			}
//		}
//		return false;
//	}
//
//	THINK_OPERATOR m_type;
//	std::vector<Card*>	m_card;
//	std::vector<Lint>   m_hu;//胡牌的类型
//	//for yb
//	int                 m_gen;
//	bool                m_isOnlyhu;
//};

typedef std::vector<ThinkUnit> ThinkVec;

struct TingState {
	ting_state	m_ting_state;
	Card*		m_out_card;
	std::vector<TingUnit> ting_unit;//可听的牌以及听的类型
	std::vector<HuUnit> hu_units;//听牌后 可胡的牌以及胡的类型
	void clear()
	{
		hu_units.clear();
		m_ting_state = Ting_Null;
		ting_unit.clear();
		m_out_card = NULL;
	}
};

struct PlayType 
{
public:
	void clear() { m_playtype.clear(); }
	void setAllTypes( const std::vector<Lint>& v ) { m_playtype = v; }
	const std::vector<Lint>& getAllTypes() const { return m_playtype; }

	// 点炮胡是否胡 转转玩法
	bool gametypeDianPao() const;
	// 小七对 转转玩法
	bool gametypeQiXiaoDui() const;

	// 换三张
	bool gametypeHuanSanZhang() const;

	//玩家选择（二翻，三翻，四翻）
	Lint gametypeGetFan() const;

	//玩家选择 将对
	bool gametypeGetJiangdui() const;

	//玩家选择 天地胡
	bool gametypeGetTiandihu() const;

	//玩家选择（点杠花 1家or3家）
	Lint gametypeGetDGH() const;

	//卡心五中，玩家选择打几门
	Lint gametypeGetMenNum() const;

	// 门前清 断幺九
	bool gametypeGetMenZhang() const;

	// 是否卡2条
	bool   gametypeIsKa2Tiao();

	//是否点炮可以平胡 for3
	bool   gametypeDBombCanPingh();

	//对对胡 番薯
	Lint   gametypeGetDDhuFan();

	//可选夹心五
	bool   gametypeKa5Heart();

	//是否需要风牌
	bool   gameNeedWind();

	Lint   gametypeGetCardNum();

	Lint   ybGametypeGetMaxFan();

	bool   ybGametypeCanph();

	bool   ybGametypeGetPiao();

	bool   myTwoPlayerCanotTang();

	bool   hasTangMustHu();

	bool   wzGametypeXueZhan();

	bool   wzGametypeYaoJiDai();

	bool	GetNeedTing();
	
protected:
	std::vector<Lint>	m_playtype;
};

struct OperateState
{
	//用于desk 和 card 之前的参数传递 参数太多了, 有扩展请扩展此处
	bool b_canHu;		// 是否可以胡
	bool b_onlyHu;		// 是否只能操作胡
	bool m_thinkGang;	// 单独处理是不是杠的牌
	Lint m_deskState;	// 当前局牌状态
	Lint m_playerPos;	// 当前一个出牌位置
	Lint m_cardState;	// 当前一个出牌状态
	Lint m_endStartPos;	// 结束位置，即海底位置
	Lint m_MePos;		// 玩家的位置
	Lint m_dingQue;		// 玩家定缺
	Lint m_hasHu;		// 血流中使用 表示玩家是否胡牌的次数
	bool bNoCard;
	Card *m_pGscard;
	bool bChaTing;
	Lint m_upState;
	bool b_getCardFromMAGang;  //摸牌是否因为明杠还是暗杠 true:暗杠(或普通摸牌)，false:明杠 默认为：true 撵中子中使用（**Ren 2017-11-30）
	bool b_37jia;
	// 初始化赋值一次就可以
	PlayType m_playtype;	// 桌子玩法
	std::vector<Card> m_hCard;  //混牌
	std::vector<Card> m_cards_guo_gang_MH; // 过杠碰掉的牌，存到这里，以后再补杠时检测这里，不能再杠了，扣点和推倒胡使用

	//CardVector	m_suoCard[4];//加锁的牌
	CardVector	*m_suoCard;//加锁的牌
	// 这个立四锁牌状态直接引用GameHandler中的状态即可，单独做一份还需要额外维护
	// 实际上这个mGameInfo只是给CardSx传递额外参数用的

	Lint	m_pos;//当前操作的位置

	Lint m_GameType;//玩法类型
	bool b_CanHu;//可以胡
	bool b_CanChi;//可以吃
	bool b_CanAnGang;//可以暗杠
	bool b_CanMingGang;//可以明杠
	bool b_CanDianGang;//可以点杠
	bool b_CanPeng;//可以碰
	bool b_CanTing;	//可以听

	bool b_GiaHu;//夹胡
	bool b_DaiFeng;//带风
	bool b_ZiMoHu;//只可自摸和
	bool b_isHun;//是否使用万能牌

	Lint b_HuType;//1小胡，2大胡
	unsigned long long allowed_fans;

	OperateState():m_deskState(0), m_playerPos(0), m_cardState(0), m_MePos(0),
		m_endStartPos(0), m_thinkGang(false), b_canHu(true), b_onlyHu(false),
		m_dingQue(0),m_hasHu(0), b_getCardFromMAGang(true), b_37jia(false),
		bNoCard(false),m_pGscard(NULL),bChaTing(false),m_suoCard(NULL),m_upState(0),
		b_isHun(false), allowed_fans(0)
	{
	}
	void Clear()
	{
		b_canHu = true;
		b_onlyHu = false;
		m_thinkGang = false;
		m_deskState = 0;
		m_playerPos = 0;
		m_cardState = 0;
		m_endStartPos = 0;
		m_MePos = 0;
		m_dingQue = 0;
		m_hasHu = 0;
		bNoCard = false;
		bChaTing = false;
		m_suoCard = NULL;
		m_upState = 0;
		b_37jia = false;
		b_isHun = false;
		m_playtype.clear();
		m_hCard.clear();
		m_cards_guo_gang_MH.clear();
	}

};

class CardManager
{
public:
	virtual ~CardManager(){};

public:
	virtual bool			Init();
	static	CardManager*	Instance()
	{
		if (!mInstance)
		{
			mInstance = new CardManager();
		}
		return mInstance;
	}
	
	//DEBUG 发牌

	//三人三房使用
	void				DealCardThrThr(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5,int cardNum);
	void				DealCardThrThr2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5,int cardNum,const Card specialCard[]);
	//发牌
	virtual void				DealCard(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5, bool needWind);
	//特定牌局 发牌
	virtual void				DealCard2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5,const Card specialCard[CARD_COUNT], bool needWind);
	//对牌进行排序，万，筒，条，红中
	virtual void				SortCard(CardVector& vec);
	//把des中的相，从src中删除
	virtual void				EraseCard(CardVector& src,CardVector& des);
	//把pCard从src中删除
	virtual void				EraseCard(CardVector& src,Card* pCard);

	//删除src 与pCard 数据一样的牌 N张
	virtual bool				EraseCard(CardVector& src,Card* pCard,Lint n);

	//删除一手牌中，相同的牌字，比如2个三万，删除其中一个
	virtual void				EarseSameCard(CardVector& src);

	//别人出牌 检测操作类型
	virtual ThinkVec			CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);
	
	//自己摸牌 检测操作类型
	virtual ThinkVec			CheckGetCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard, OperateState& gameInfo);
	
	//检测是否胡牌	--- 检测胡牌，各自实现自已的。
	//virtual bool				CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo,std::vector<Lint>& vec, bool isGetCard);

	virtual	bool				CheckCanHu(TingState& tingstate,CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, std::vector<Hu_type>& vec, bool isGetCard);
	//检测是否杠 
	virtual bool				CheckCanGang(CardVector& handcard,  Card* outCard);
	//检测是否碰 
	virtual bool				CheckCanPeng(CardVector& handcard,  Card* outCard);


	//检测大胡
	//检测小七对,是否是豪华，special = 1 
	bool                       CheckXiaoqidui(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard,Lint& special);
	//检测清一色
	virtual bool				CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, std::vector<Lint>& vec);
	//检测乱将胡
	virtual bool				CheckLuanjianghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	//检测碰碰胡
	virtual bool				CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard);
	//检测全球人
	virtual bool				CheckQuanqiuren(CardVector& handcard,Card* outCard);

	//枪杠胡
	virtual bool				CheckQiangganghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//检测杠上开花
	virtual bool				CheckGangshangkaihua(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//抢杠胡
	virtual bool				CheckGangshangpao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底捞
	virtual bool				CheckHaiDiLao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底炮			
	virtual bool				CheckHaiDiPao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);

	int                         CheckJiangdui( CardVector& handcard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);

	//检测暗杠 
	virtual bool				CheckAnGang(CardVector& handcard,  std::vector<Card*>& vec);

	//检测明杠 
	virtual bool				CheckMGang(CardVector& handcard,CardVector& pengCard,  std::vector<Card*>& vec);

	virtual bool				IsContinue(CardVector& result);

	virtual bool				IsSame(CardVector& result);

	virtual bool				IsSame(Card* c1,Card* c2);

	virtual bool				IsNineOne(Card* c);

	virtual Card*				GetCard(Lint color,Lint number);

	//金钩钓
	virtual bool				CheckJinGouDiao(CardVector& handcard, CardVector& eatCard,Card* outCard){return false;};

	// 判断手牌是否花猪
	virtual bool				IsHuaZhu(CardVector& handcard, Lint dingque){return false;};

	// 查听
	virtual Lint				CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, Lint fan[], Lint dingque, Card* winCard, Lint maxFan){return 0;};

	// 算根
	virtual Lint				CheckGen(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& agangcard,CardVector& mgangcard, Card* getCard);

	// 计算胡牌番数
	bool                        CheckOneNine( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	bool                        CheckNoOneNine( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard ); // 断幺九
	bool                        CheckMenQing( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard ); // 门前清

	bool                        CheckMidCard(const CardVector& handcard,const Card& mid,const Card* outCard );
	bool                        CheckOne2Nine(const CardVector& handcard,const Card* outCard);   //1条龙
	bool                        CheckSisterPairs(const CardVector& handcard,const Card* outCard);
	void				        SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT]);

	bool                        CheckHoo(const CardVector& handcard,const Card* outCard);
	bool                        CheckMyHoo( int allPai[3][10]);
	bool						Analyzefor(int* kindcard);
public:
	Card		m_card[CARD_COUNT];
	Card		m_card_Laizi[CARD_COUNT];
	CardVector	m_cardVec;
	CardVector	m_cardVec_Laizi;;
	static	CardManager* mInstance;
};

#define gCardMgr CardManager::Instance()

#endif