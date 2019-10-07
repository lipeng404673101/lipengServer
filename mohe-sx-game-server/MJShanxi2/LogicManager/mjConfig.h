
#ifndef _MJCONFIG_H_
#define _MJCONFIG_H_


#include "LSingleton.h"
#include "LMsgS2S.h"

class mjConfig :public LSingleton<mjConfig> 
{
public:
	virtual	bool	Init();

	virtual	bool	Final();

	bool       GetCommonConfigValue(std::string name, Lint& nValue);
	bool       GetFeeTypeValue(Lint id, deskFeeItem& feeItem, Lint game_state);

	bool       UpdateConfigFromCenter(LMsgCE2LMGConfig* msg);

private:
	std::map<std::string, commonItem*> m_configList; //新用户登录金币
	std::map<int, deskFeeItem*> m_feeList;  //桌子类型


};

#define gMjConfig mjConfig::Instance()

#endif