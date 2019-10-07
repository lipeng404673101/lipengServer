
#ifndef _MJCONFIG_H_
#define _MJCONFIG_H_


#include "LSingleton.h"
#include "LMsgS2S.h"
#include "LDBSession.h"


class mjConfig :public LSingleton<mjConfig> 
{
public:
	virtual	bool	Init();

	virtual	bool	Final();
	mjConfig();

	bool       GetCommonConfigValue(std::string name, Lint& nValue);
	bool       GetFeeTypeValue(Lint id, deskFeeItem* feeItem);

	bool       LoadDeskFeeDataFromDB();
	bool       LoadCommonConfigDataFromDB();
	bool       CreatToLogicConfigMessage(LMsgCE2LMGConfig& Config);

public:
	std::map<std::string, commonItem*> m_configList; //新用户登录金币
	std::map<int, deskFeeItem*> m_feeList;  //桌子类型

	LDBSession* m_dbsession;


};

#define gMjConfig mjConfig::Instance()

#endif