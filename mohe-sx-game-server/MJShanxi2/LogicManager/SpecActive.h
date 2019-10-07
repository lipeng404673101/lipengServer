#pragma once

#ifndef _SPEC_ACTIVE_H
#define _SPEC_ACTIVE_H
#include "LSingleton.h"
#include "LMsgS2S.h"



class SpecActive : public LSingleton<SpecActive>
{
public:
	SpecActive();
	~SpecActive();

	virtual	bool		Init();
	virtual	bool		Final();

	void addSpecActive(const LMsgLMG2LSpecActive* specList);
	void sendToLogic(Lint logicId,Lint id = 0);    //id=0，全部发， id !=0 ,发指定id的
	void sendToAllLogic(Lint id = 0);    //id=0，全部发， id !=0 ,发指定id的

private:

	std::map<Lint, boost::shared_ptr<SpecActiveItem>>   m_mapId2SpecActive;
	void addSpecActiveItem(boost::shared_ptr<SpecActiveItem> specItem);      //添加和更新，存在更新，不存在更新



	//////////////////////////////////////////////////////////////////////////////////////
	//特殊牌型
	Lstring getMessage(Lint userId, Lstring nike, Lint time, std::vector<Lint> paixing);
private:
	void Init_paixing();
	std::map<Lint, Lstring>                                                           m_mapId2PaiXing;
	std::vector<Lint>   m_paiXingNo{ 1003,1006,1005,1004,1007,1027,1028,1030,1029 };
	std::vector<Lstring>  m_paiXingName{ " 七小对 "," 一条龙 "," 清一色 "," 豪华七小对 "," 十三幺 "," 青龙 "," 清七对 "," 清豪华七小对","字一色 " };

	//////////////////////////////////////////////////////////////////////////////////////

};

#define gSpecActive SpecActive::Instance()








#endif
