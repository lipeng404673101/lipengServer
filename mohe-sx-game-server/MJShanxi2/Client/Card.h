#ifndef _CARD_H_
#define _CARD_H_

#include "LSingleton.h"

#define CARD_COUNT 108

enum CARD_COLOR
{
	CARD_COLOR_WAN = 1,
	CARD_COLOR_TUO = 2,
	CARD_COLOR_SUO = 3,
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

typedef std::vector<Lint> ThinkVec;

//玩家思考类型
enum THINK_OPERATOR
{
	THINK_OPERATOR_NO = 0,
	THINK_OPERATOR_BOMB,//收炮
	THINK_OPERATOR_GANG,//杠
	THINK_OPERATOR_PENG,//碰
};

//玩家出牌操作
enum OUT_CARD_TYPE
{
	OUT_CARD_TYPE_NORMAL,//普通出牌
	OUT_CARD_TYPE_GANG,//出一张牌,自己可以杠
	OUT_CARD_TYPE_AN_GANG,//自己暗杠
};


class CardManager:public LSingleton<CardManager>
{
public:
	virtual bool		Init();
	virtual bool		Final();

	//发牌
	void				DealCard(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5);
	//对牌进行排序，万，筒，条，红中
	void				SortCard(CardVector& vec);
	//把des中的相，从src中删除
	void				EraseCard(CardVector& src,CardVector& des);
	//把pCard从src中删除
	void				EraseCard(CardVector& src,Card* pCard);
	//删除一手牌中，相同的牌字，比如2个三万，删除其中一个
	void				EarseSameCard(CardVector& src);

	//检测操作类型
	ThinkVec			CheckOperator(CardVector& handcard, Card* outCard);
	//检测是否胡牌
	bool				CheckCanHu(CardVector& handcard, Card* outCard);
	//检测是否杠 
	bool				CheckCanGang(CardVector& handcard,  Card* outCard);
	//检测是否碰 
	bool				CheckCanPeng(CardVector& handcard,  Card* outCard);

	bool				CheckCanTing(CardVector& handcard);

	bool				IsContinue(CardVector& result);

	bool				IsSame(CardVector& result);

	bool				IsSame(Card* c1,Card* c2);

	bool				IsNineOne(Card* c);

	bool				GetSpecialOneTwoThreeFour(CardVector& src,CardVector& one,CardVector& two,CardVector& three,CardVector& four);

	Card*				GetCard(Lint color,Lint number);
private:
	Card		m_card[CARD_COUNT];
	CardVector	m_cardVec;
};

#define gCardMgr CardManager::Instance()

#endif