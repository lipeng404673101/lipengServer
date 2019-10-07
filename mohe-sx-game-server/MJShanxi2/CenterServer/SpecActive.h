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

	
	void sendToManager(Lint manageId,Lint id = 0);    //id=0，全部发， id !=0 ,发指定id的
	bool loadData();

private:

	void addSpecActiveItem(boost::shared_ptr<SpecActiveItem> specItem);      //添加和更新，存在更新，不存在更新

	std::map<Lint, boost::shared_ptr<SpecActiveItem>>   m_mapId2SpecActive;
};

#define gSpecActive SpecActive::Instance()








#endif
