#ifndef _CARD_KOUDIAN_H_
#define _CARD_KOUDIAN_H_

#include "CardBase.h"

class ShanXiCardKouDian : public CardBase
{
public:
	ShanXiCardKouDian();
	virtual ~ShanXiCardKouDian(){};

	static	ShanXiCardKouDian*	Instance()
	{
		if (!mInstance)
		{
			mInstance = new ShanXiCardKouDian();
		}
		return mInstance;
	}

	//别人出牌 检测操作类型
	virtual ThinkVec	CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);

	//自己摸牌 检测操作类型
	virtual ThinkVec	CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard,OperateState& gameInfo);
	
	//检测是否胡牌
	virtual bool		CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Hu_type>& vec);

	void				CheckCanHuNormal(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);
	void				CheckCanHuHun(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);
	
	// 查听 玩家手里的牌是3n+1 返回番值最大的winCards索引 -1表示没有听
	virtual Lint		CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, std::vector<TingCard>& winCards, const Card* ignore = NULL);

	virtual TingVec		CheckGetCardTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo );

private:
	static	ShanXiCardKouDian* mInstance;
};

#define gCardKouDian ShanXiCardKouDian::Instance()

#endif

