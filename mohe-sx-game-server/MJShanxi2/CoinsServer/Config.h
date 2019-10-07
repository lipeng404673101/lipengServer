#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();


	Lint	GetLogLevel();

	Lstring GetCoinsKey();
	Lstring GetInsideIp();
	Lshort	GetInsidePort();

	Lstring GetLogicManagerIp();
	Lshort	GetLogicManagerPort();

	// 玩家换牌倒计时
	Lint	GetChangeOutTime();

	// 玩家操作倒计时 单位秒
	Lint	GetOpOutTime();

	// 金币场最多的人数
	Lint	GetMaxUserCount();

	// 每天赠送金币的次数
	Lint	GetMaxGiveCount();

	// 每次赠送金币的数量
	Lint	GetGiveCoinsCount();

	// 是否服务器主动赠送金币
	bool	GetIsGiveCoins();

	// 进入房间最小的金币数
	Lint	GetMinCoins();

	// 底分
	Lint	GetBaseScore();

	// 是否调试模式
	Lint	GetDebugModel();

private:
	boost::mutex m_mutex;
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif