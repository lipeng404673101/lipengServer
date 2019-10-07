#ifndef _CARDBASE_H_
#define _CARDBASE_H_

#include "LBase.h"
#include "GameDefine.h"
#include "Card.h"
#include "LVideo.h"

class CardBase
{
public:
	virtual ~CardBase(){};

	// 需要子类实现的函数

	//别人出牌 检测操作类型
	virtual ThinkVec			CheckOutCardOperator( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo ) = 0;

	//自己摸牌 检测操作类型
	virtual ThinkVec			CheckGetCardOperator( CardVector& handcard,CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo ) = 0;

	//检测是否胡牌	--- 检测胡牌，各自实现自已的。
	virtual bool				CheckCanHu( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, std::vector<Hu_type>& vec ) = 0;

	// 查听 玩家手里的牌是3n+1 返回番值最大的winCards索引 -1表示没有听
	virtual Lint				CheckTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, std::vector<TingCard>& winCards, const Card* ignore = NULL ) = 0;

public:
	//初始化
	virtual bool				Init();
	//发牌
	virtual void				DealCard(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5, bool needwind = false);
	//特定牌局 发牌
	virtual void				DealCard2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5,const Card specialCard[CARD_COUNT], bool needwind = false);

	virtual void				DealCardThr(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5, bool needwind);

	virtual void				DealCardThr2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5,const Card specialCard[CARD_COUNT], bool needwind);
	//配牌使用
	void				        SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT], bool needwind);

	virtual bool				IsSame(Card* c1,Card* c2);

	//对牌进行排序，万，筒，条，红中
	virtual void				SortCard(CardVector& vec);

	//把des中的相，从src中删除
	virtual void				EraseCard(CardVector& src,CardVector& des);
	//把pCard从src中删除
	virtual void				EraseCard(CardVector& src,Card* pCard);
	//删除src 与pCard 数据一样的牌 N张
	virtual bool				EraseCard(CardVector& src,Card* pCard,Lint n);

	//检测是否杠 
	virtual bool				CheckCanGang(CardVector& handcard, Card* outCard);
	//检测是否杠 返回值表示杠需要的混数 如果为为0表示不需要混能直接杠
	virtual int					CheckCanGangNeedHun(CardVector& handcard, Card* outCard);
	//检测是否碰 
	virtual bool				CheckCanPeng(CardVector& handcard, Card* outCard);
	//检测是否碰 返回值表示碰需要的混数 如果为为0表示不需要混能直接碰
	virtual int					CheckCanPengNeedHun(CardVector& handcard, Card* outCard);
	//检查是否吃
	virtual bool				CheckCanChi(CardVector& handcard, Card* outCard, std::vector<Card*>& vec);
	//检测暗杠
	virtual bool				CheckAnGang(CardVector& handcard, std::vector<Card*>& vec);
	//检测暗杠
	virtual bool				CheckAnGangWithHun(CardVector& handcard, CardVector& huncards, std::vector< std::vector<Card*> >& vec);
	//检测明杠
	virtual bool				CheckMGang(CardVector& handcard, CardVector& pengCard, std::vector<Card*>& vec);
	//检测明杠
	virtual bool				CheckMGangWithHun(CardVector& handcard, CardVector& huncards, CardVector& pengCard, std::vector< std::vector<Card*> >& vec);

	//检测大胡

	//检测小七对，并验证是否为龙爪背
	virtual bool				CheckXiaoqidui(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, int& special, bool& isLongZhuaBei);

	//检测小七对 special 表示成杠的数量
	virtual bool				CheckXiaoqidui(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, int& special);
	virtual bool				CheckXiaoqidui(const int handcard[4][9],CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard, int& special);

	//检测小七对 返回需要的混数 -1表示成不了七对 special 表示已经成杠的数量 
	virtual int					CheckXiaoqiduiNeedHun(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, int& special, std::vector<Card>& huncards);
	virtual int					CheckXiaoqiduiNeedHun(const int cardnum[4][9],CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard, int& special, std::vector<Card>& huncards);
	//检测清一色
	virtual bool				CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	//检测混一色
	virtual bool				CheckHunyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	//检测碰碰胡
	virtual bool				CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard);
	virtual bool				CheckPengpenghu(const int cardnum[4][9],CardVector& eatCard);
	// 碰碰胡需要的混数 需要外部判断有没有吃牌 无法组成对对胡返回-1
	virtual int					CheckPengpenghuNeedHun(const int cardnum[4][9]);
	//检测全球人 注意 全求人 一般outCard需要别人点炮 这个外部判断
	virtual bool				CheckQuanqiuren(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);

	//枪杠胡
	virtual bool				CheckQiangganghu(OperateState& gameInfo);
	//检测杠上开花
	virtual bool				CheckGangshangkaihua(OperateState& gameInfo);
	//抢杠胡
	virtual bool				CheckGangshangpao(OperateState& gameInfo);
	//海底捞
	virtual bool				CheckHaiDiLao( OperateState& gameInfo);
	//海底炮			
	virtual bool				CheckHaiDiPao(OperateState& gameInfo);

	// 检查是否全部是258
	virtual bool                Check258( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard );

	//金钩钓
	virtual bool				CheckJinGouDiao( CardVector& handcard, Card* outCard);

	// 判断手牌是否花猪
	virtual bool				IsHuaZhu( CardVector& handcard, Lint dingque ){ return false; };
	
	// 算根
	virtual Lint				CheckGen( CardVector& handcard, CardVector& pengCard, CardVector& agangcard, CardVector& mgangcard, CardVector& eatCard, Card* getCard );

	//全幺九
	virtual bool                CheckOneNine( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard );
	// 断幺九
	virtual bool                CheckNoOneNine( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard );
	// 门前清
	virtual bool                CheckMenQing( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard );

	// 卡二条
	virtual bool                CheckMidCard( const CardVector& handcard, const Card& mid, const Card* outCard);
	//1条龙
	virtual bool                CheckOne2Nine( const CardVector& handcard, const Card* outCard );  
	//姐们对
	virtual bool                CheckSisterPairs( const CardVector& handcard, const Card* outCard );

	// 烂牌 147 258 369
	virtual bool				CheckBadCard( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin );
	virtual bool				CheckBadCard( const int handcard[4][9], CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, bool& isqixin );
	virtual bool				CheckBadCardWithHun( CardVector& handcard, int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin );
	
	// 烂牌 间隔2
	virtual bool				CheckBadCard2( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin );
	virtual bool				CheckBadCard2( const int handcard[4][9], CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, bool& isqixin );
	virtual bool				CheckBadCardWithHun2( CardVector& handcard, int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin );
	
	// 幺牌
	virtual bool				YaoCard( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard,CardVector& mgangcard, Card* outCard );

	// 大小三元 zike表示 表示中发白刻的个数 来区分是大小三元
	virtual bool				CheckDXSanYuan( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, Lint& zike );
	// 大小三元 zike表示 表示中发白刻的个数 来区分是大小三元 needhun表示需要混数
	virtual bool				CheckDXSanYuanWithHun( const int cards[4][9], int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Lint& zike, Lint& needhun );
	// 注意次函数会修改参数cards 外部需要拷贝 方便外部使用 修改后的值表示出去组成大小三元的牌
	virtual bool				CheckDXSanYuanWithHun2( int cards[4][9], int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Lint& zike, Lint& needhun );

	// 查听 玩家手里的牌是3n+2 最终掉用的是CheckTing
	virtual TingVec				CheckGetCardTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo );

	// 胡牌函数
	// 检查胡 3n+2牌型
	bool                        CheckHoo( const CardVector& handcard, const Card* outCard );
	// 检查胡 3n+2牌型
	bool                        CheckHoo( int cards[4][9] );
	// 检查胡 3n+2牌型，获取胡牌的将牌
	bool                        CheckHoo( int cards[4][9], CardValue& jiangCard );

	// 能否成顺刻
	bool						ToBeShunKe( const int card[9], bool bzipai );


	// 检查3n+2牌型 带混的牌handcard里没有混 hunnum表示混的数量
	bool                        CheckHooWithHun( const CardVector& handcard, const Card* outCard, int hunnum );
	bool                        CheckHooWithHun( int cards[4][9], int hunnum );
	// 成顺刻需要的混的数量
	int							ToBeShunKeNeedHun( const int card[9], bool bzipai );
	// cardindex表示组牌的顺序
	int							ToBeShunKeNeedHun( const int card[9], bool bzipai, int cardindex[9] );
	
public:
	Card		m_card[CARD_COUNT];	// 初始化之后 值是固定的
};


#endif