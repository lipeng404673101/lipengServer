#ifndef _CARD_SX_H_
#define _CARD_SX_H_

#include "LSingleton.h"
#include "GameDefine.h"
#include "LMsg.h"
#include "Card.h"

#include "ShanXiFeatureToggles.h"



tile_mask_t calc_pg_mask(CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard);

class CardManagerSx:public LSingleton<CardManagerSx>
{
public:
	virtual bool		Init();
	virtual bool		Final();
	
	//void				AddZiPai();
	//发牌
	void				DealCard(CardVector& m_3_laizi, CardVector hand_card[],Lint user_count,CardVector& rest_card,Lint game_type, GameOptionSet& playtype);
	//特定牌局 发牌
	void				DealCard2(CardVector& m_3_laizi, CardVector hand_card[],Lint user_count,CardVector& rest_card, const Card player1Card[13], const Card player2Card[13], const Card player3Card[13], const Card player4Card[13], const Card SendCard[84],Lint game_type, GameOptionSet& playtype);
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
	//不报听的查听选项（**Ren 2017-12-27）
	ThinkVec            CheckCanTing_Check(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, CardVector &suoCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles);
	//别人出牌 检测操作类型
	ThinkVec			CheckOutCardOperator(PlayerState &ps, TingState& tingstate, CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, CardVector &suoCard, Card* outCard,OperateState& gameInfo, ShanXiFeatureToggles const &toggles);
	//自己摸牌 检测操作类型
	ThinkVec			CheckGetCardOperator(PlayerState &ps, TingState& tingstate, CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, CardVector &suoCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles);
	//听牌后别人出牌 检测操作类型
	ThinkVec			CheckOutCardOpeartorAfterTing(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, CardVector &suoCard, Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles);
	//听牌后自己摸牌出牌 检测操作类型
	ThinkVec			CheckGetCardOpeartorAfterTing(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, CardVector &suoCard, Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles);
	//碰牌后检测是否可以听
	ThinkVec CheckAfterPeng(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles);

	ThinkVec			CheckDuiBao(TingState& tingstate, GameOptionSet& playTypeInfo);

	Lint				CheckLaiziNum(CardVector& handcard, OperateState& gameInfo);

	Lint				get_card_index(Card* card);

	Card*				get_card(Lint index);
	Card*				get_card2(Lint index); 
private:
	bool				xiaohu_CheckHu(CardVector card_vector,Card* curCard, bool hu_37jia=false);
	bool				Hu_explorer_zheng_function(std::vector<Lint> card_vector, Lint index);
	Lint				judge_ke(std::vector<Lint>& targe_vector, Lint index);
	Lint				judge_shun(std::vector<Lint>& targe_vector,Lint index);
	bool				xiaohu_CheckTing(CardVector handcard, OperateState& gameInfo,CardVector& tingCard, bool have_shun, bool have_kezi);
	void				Ting_explorer_zheng_function(std::vector<Lint> card_vector, Lint index,Lint Lai_num, CardVector& tingcards, Card* tingcard = NULL,bool hu_37jia=false, bool have_shun=false, bool have_kezi=false);
	bool				explorer_zheng_function(std::vector<Lint> cards, Lint index, Lint Lai_num);

	Lint				ting_judge_shun(std::vector<Lint>& targe_vector, Lint index);

	bool				HasKezi(CardVector handcard, CardVector& pengCard, OperateState& gameInfo);

	bool                HongTong_CheckQiXiaoDui_Ting(PlayerState &ps, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<TingUnit>& tingInfo);
	bool				all_CheckTing(PlayerState &ps, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<TingUnit>& tingunit);
public:
	//听牌 胡牌的必要条件
	bool				CheckBaseNeed(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo);


	void calc_ting_kou(PlayerState &ps, CardVector &handCard, tile_mask_t pg_mask, OperateState &gameInfo, ShanXiFeatureToggles const &toggles, std::vector<HuUnit>& tingInfo);

	//检测是否听牌
	bool				CheckCanTing(PlayerState &ps, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<HuUnit>& tingInfo);
	//检测是否胡牌
	bool				CheckCanHu(PlayerState &ps, TingState& tingstate,CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<Hu_type>& vec, bool isGetCard);
	//听后检测是否胡牌
	bool				CheckCanHuAfterTing(PlayerState &ps, TingState& tingstate,Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<Hu_type>& vec, bool isGetCard);

	//检测是否杠 
	bool				CheckCanGang(CardVector& handcard, CardVector &suoCard, Card* outCard, bool huType, bool pt_laizi, OperateState& gameInfo, ShanXiFeatureToggles const &toggles);
	//检测是否碰 
	bool				CheckCanPeng(CardVector& handcard, CardVector &suoCard, Card* outCard, bool huType, bool pt_laizi = false);
	//检测暗杠 
	bool				CheckAnGang(CardVector& handcard, CardVector &suoCard, std::vector<Card*>& vec, OperateState& gameInfo, ShanXiFeatureToggles const &toggles);
	//检测明杠（可胡手牌）
	bool				CheckMGang(CardVector& handcard,CardVector& pengCard, CardVector &suoCard, std::vector<Card*>& vec, OperateState& gameInfo, ShanXiFeatureToggles const &toggles,bool pt_laizi=false);

	//检测明杠（只胡摸到的牌）
	bool				CheckMGang(Card* getCard, CardVector& pengCard, std::vector<Card*>& vec, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, bool pt_laizi = false);

	//检测小七对,是否是豪华，special = 1 
	bool				CheckXiaoqidui(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo);

	//十三幺			
	bool				CheckShiSanYiao(CardVector& handcard, Card* outCard);

	//检测清一色
	bool				CheckQingYiSe(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo);

	//检测和牌有几种牌色
	bool				CheckQueMen(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo);


	bool				CheckHasTriplet(CardVector& handcard);
	void Merge_TingInfo(std::vector<TingUnit> & tingInfoTarget, std::vector<TingUnit> & tingInfoAdd, ShanXiFeatureToggles const &toggles);
public:
	bool				IsContinue(CardVector& result);
	bool				IsSame(CardVector& result);
	bool				IsSame(Card* c1,Card* c2);
	bool				IsNineOne(Card* c);
	bool				GetSpecialOneTwoThreeFour(CardVector& src,CardVector& one,CardVector& two,CardVector& three,CardVector& four);

	void				Calc(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, std::vector<TingUnit>& tingInfo);
	virtual Lint		CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, std::vector<HuUnit>& winCards, const Card* ignore = NULL);
	bool				CheckCanHu333(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);
	void				CheckCanHuHun(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);
	bool				CheckHooWithHun( int cards[4][9], int hunnum );

	int					ToBeShunKeNeedHun( const int card[9], bool bzipai );

	int					ToBeShunKeNeedHun( const int card[9], bool bzipai, int cardindex[9] );
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

private:
	//// 临汾一门牌听牌时是否时混一色或者清一色 by wyz
	bool              CheckCanTing_yimenpai_spec(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard);

};

#define gCardMgrSx CardManagerSx::Instance()

#endif