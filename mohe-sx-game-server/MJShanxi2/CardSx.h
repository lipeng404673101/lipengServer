#ifndef _CARD_H_
#define _CARD_H_

#include "LSingleton.h"
#include "GameDefine.h"
#include "LMsg.h"
enum ting_state {
	Ting_Null,//不能听
	Ting_ChooseTing,//在选择听
	Ting_waitOutput,//选择听 等待出牌确定
	Ting_waitHu,//听状态下 等待胡牌
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

	Card()
	{
		m_number = 0;
		m_color = 0;
	}
};
typedef std::vector<Card*> CardVector;


struct Hu_type
{
	Lint   m_hu_basicType;//基础胡牌型1.小胡，2夹胡
	Lint   m_hu_type;//胡牌的类型
	std::vector<Lint> m_hu_basicTypes;//胡牌基本类型
	std::vector<Lint>	m_hu_way; //胡牌的方式
	Hu_type():m_hu_type(0)
	{
	}
	void clear()
	{
		m_hu_basicType = 1;
		m_hu_type = 0;
		m_hu_way.clear();
	}
};


struct HuUnit {
	Lint  m_hu_basicType;//基础胡牌型1.小胡，2夹胡
	std::vector<Lint> m_hu_basicTypes;//胡牌基本类型
	Card* hu_card;
	Lint  hu_type;
	void clear() {
		hu_card = NULL;
		hu_type = THINK_OPERATOR_NULL;
		m_hu_basicType = 1;
	}
};

struct TingUnit {
	Card* out_card;				 //出哪张牌可以听
	std::vector<HuUnit> hu_units;//出了这张牌后可以胡什么牌以及类型
};


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



struct bao {
	Card*		m_baocard;
	Lint		m_position;
	bao():m_baocard(NULL),m_position(-1)
	{

	}
	void clear()
	{
		m_baocard = NULL;
		m_position = -1;
	}
};
struct TingState {
	bao			bao_unit;
	ting_state	m_ting_state;
	Card*		m_out_card;
	std::vector<TingUnit> ting_unit;//可听的牌以及听的类型
	std::vector<HuUnit> hu_units;//听牌后 可胡的牌以及胡的类型
	void clear()
	{
		hu_units.clear();
		m_ting_state = Ting_Null;
		bao_unit.clear();
		ting_unit.clear();
		m_out_card = NULL;
	}
};

struct OperateState
{
	Lint m_GameType;//玩法类型
	bool b_CanHu;//可以胡
	bool b_CanChi;//可以吃
	bool b_CanAnGang;//可以暗杠
	bool b_CanMingGang;//可以明杠
	bool b_CanDianGang;//可以点杠
	bool b_CanPeng;//可以碰
	bool b_CanTing;	//可以听
	bool b_37jia;//可胡37夹
	bool b_PiaoHuBuXianSeAnd19; //飘胡不限色和19

	bool b_GiaHu;//夹胡
	bool b_BaoTing;//报听
	bool b_DaiFeng;//带风
	bool b_ZiMoHu;//只可自摸和

	Lint b_HuType;//1小胡，2大胡
	Card* LaiziCard;
	OperateState() :m_GameType(0), b_CanHu(false), b_CanChi(false), b_CanAnGang(false), b_37jia(false), b_CanMingGang(false),b_CanTing(false),b_CanDianGang(false),b_CanPeng(false),
		b_BaoTing(false), b_DaiFeng(false), b_ZiMoHu(false), b_HuType(1),LaiziCard(NULL)
	{

	}
};

class CardManager:public LSingleton<CardManager>
{
public:
	virtual bool		Init();
	virtual bool		Final();
	
	//void				AddZiPai();
	//发牌
	void				DealCard(CardVector& m_3_laizi, CardVector hand_card[],Lint user_count,CardVector& rest_card,Lint game_type, PlayTypeInfo& playtype);
	//特定牌局 发牌
	void				DealCard2(CardVector& m_3_laizi, CardVector hand_card[],Lint user_count,CardVector& rest_card, const Card player1Card[13], const Card player2Card[13], const Card player3Card[13], const Card player4Card[13], const Card SendCard[84],Lint game_type, PlayTypeInfo& playtype);
	//对牌进行排序，万，筒，条，红中
	void				SortCard(CardVector& vec);
	//把des中的相，从src中删除
	void				EraseCard(CardVector& src,CardVector& des);
	//把pCard从src中删除
	void				EraseCard(CardVector& src,Card* pCard);
	//删除src 与pCard 数据一样的牌 N张
	void				EraseCard(CardVector& src,Card* pCard,Lint n);
	//删除一手牌中，相同的牌字，比如2个三万，删除其中一个
	void				EarseSameCard(CardVector& src);
	//别人出牌 检测操作类型
	ThinkVec			CheckOutCardOperator(TingState& tingstate, CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);
	//自己摸牌 检测操作类型
	ThinkVec			CheckGetCardOperator(TingState& tingstate, CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard, OperateState& gameInfo);
	//听牌后别人出牌 检测操作类型
	ThinkVec			CheckOutCardOpeartorAfterTing(TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo);
	//听牌后自己摸牌出牌 检测操作类型
	ThinkVec			CheckGetCardOpeartorAfterTing(TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo);
	
	ThinkVec			CheckDuiBao(TingState& tingstate, PlayTypeInfo& playTypeInfo);

	Lint				CheckLaiziNum(CardVector& handcard, OperateState& gameInfo);

	Lint				get_card_index(Card* card);

	Card*				get_card(Lint index);
	Card*				get_card2(Lint index); 
private:
	bool				CheckPiaoHu(CardVector& handcard, CardVector& eatCard, Card* outCard);
	bool				xiaohu_CheckHu(CardVector card_vector,Card* curCard, bool hu_37jia=false);
	bool				xiaohu_CheckHu2(CardVector& card_vector, Card* curCard);

	bool				Hu_explorer_zheng_function(std::vector<Lint> card_vector, Lint index);
	Lint				judge_ke(std::vector<Lint>& targe_vector, Lint index);
	Lint				judge_shun(std::vector<Lint>& targe_vector,Lint index);
	bool				xiaohu_CheckTing(CardVector handcard, OperateState& gameInfo,CardVector& tingCard, bool have_shun, bool have_kezi);
	bool				xiaohu_CheckTing2(CardVector handcard, OperateState& gameInfo,CardVector& tingCard, bool have_shun, bool have_kezi);
	void				Ting_explorer_zheng_function(std::vector<Lint> card_vector, Lint index,Lint Lai_num, CardVector& tingcards, Card* tingcard = NULL,bool hu_37jia=false, bool have_shun=false, bool have_kezi=false);
	bool				explorer_zheng_function(std::vector<Lint> cards, Lint index, Lint Lai_num);
	//new
	void				xiao_hu_Ting_explorer_zheng_function(std::vector<Lint> card_vector, Lint index, Lint Lai_num, CardVector& tingcards, Card* tingcard, bool have_shun, bool have_kezi);

	Lint				ting_judge_shun(std::vector<Lint>& targe_vector, Lint index);

	bool				HasKezi(CardVector handcard, CardVector& pengCard, OperateState& gameInfo);

	bool				all_CheckTing(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo, std::vector<TingUnit>& tingunit);
public:
	//听牌 胡牌的必要条件
	bool				CheckBaseNeed(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo);
	//检测是否听牌
	bool				CheckCanTing(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, std::vector<HuUnit>& tingInfo);
	//检测是否胡牌
	bool				CheckCanHu(TingState& tingstate,CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, std::vector<Hu_type>& vec, bool isGetCard);
	//听后检测是否胡牌
	bool				CheckCanHuAfterTing(TingState& tingstate,Card* outCard, OperateState& gameInfo, std::vector<Hu_type>& vec, bool isGetCard);

	//检测是否杠 
	bool				CheckCanGang(CardVector& handcard, Card* outCard, bool huType, bool pt_laizi = false);
	//检测是否碰 
	bool				CheckCanPeng(CardVector& handcard, Card* outCard, bool huType, bool pt_laizi = false);
	//检查吃	·
	bool				CheckCanChi(CardVector& handcard,Card* outCard,  std::vector<Card*>& vec);
	//检测暗杠 
	bool				CheckAnGang(CardVector& handcard,  std::vector<Card*>& vec,bool pt_laizi=false);
	//检测明杠（可胡手牌）
	bool				CheckMGang(CardVector& handcard,CardVector& pengCard,  std::vector<Card*>& vec,bool pt_laizi=false);

	//检测明杠（只胡摸到的牌）
	bool				CheckMGang(Card* getCard, CardVector& pengCard, std::vector<Card*>& vec, bool pt_laizi = false);

	//检测一条龙
	bool				CheckYiTiaoLong(CardVector& handcard, Card* winCard);

	//检测小七对,是否是豪华，special = 1 
	bool				CheckXiaoqidui(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo);

	//十三幺			
	bool				CheckShiSanYiao(CardVector& handcard, Card* outCard);

	//检测清一色
	bool				CheckQingYiSe(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo);

	bool				CheckHasTriplet(CardVector& handcard);
public:
	bool				IsContinue(CardVector& result);
	bool				IsSame(CardVector& result);
	bool				IsSame(Card* c1,Card* c2);
	bool				IsNineOne(Card* c);
	bool				GetSpecialOneTwoThreeFour(CardVector& src,CardVector& one,CardVector& two,CardVector& three,CardVector& four);
private:
	void				SwapCardBySpecial(CardVector& cvIn, const Card player1Card[13], const Card player2Card[13], const Card player3Card[13], const Card player4Card[13], const Card SendCard[84]);
private:
	Card		wan_base_card[BASE_CARD_COUNT];
	Card        bing_base_card[BASE_CARD_COUNT];
	Card        tiao_base_card[BASE_CARD_COUNT];
	Card        Lai_zi_card[LAI_ZI_COUNT];
	Card        zi_base_card[BASE_CARD_COUNT];
	Card		yao_13[MAX_HANDCARD_COUNT];

	CardVector  m_3_laizi;	//全部3副牌加癞子

	CardVector  m_13yao; //13幺牌型
};

#define gCardMgr CardManager::Instance()

#endif