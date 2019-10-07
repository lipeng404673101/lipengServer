#ifndef _ACTIVE_MANAGER_H
#define _ACTIVE_MANAGER_H

#include "LSingleton.h"
#include "LMsgS2S.h"
#include "LRedisClient.h"

// 转盘
#define ActivityDraw_Config "ActivityDraw_Config"						// 配置
#define ActivityDraw_RemainTotalCount_ "ActivityDraw_RemainTotalCount_"	// 剩余库存数量
#define ActivityDraw_RemainTotalDate "ActivityDraw_RemainTotalDate"		// 记录剩余库存日期

#define ActivityDraw_WinUsers "ActivityDraw_WinUsers"	// 中的中奖纪录key
#define ActivityDraw_WinUsersCount	300					// 总的中奖 纪录条数
#define ActivityDraw_Win_ "ActivityDraw_Win_"			// 个人中奖纪录key
#define ActivityDraw_WinCount	100						// 个人 纪录的条数

// 分享
#define ActivityShare_Config "ActivityShare_Config"

struct Activity
{
	Activity()
	{
		m_ActiveID = 0;
		m_BeginTime = -1;
		m_EndTime = -1;
	}

	Lint	m_ActiveID;		// ActivityID类型
	Lstring	m_ActiveName;
	Lstring	m_Desc;
	Lint  m_BeginTime;
	Lint  m_EndTime;

	Lstring m_strConfig;		// 后台配置文件
};

struct ActivityDrawGift
{
	ActivityDrawGift()
	{
		m_GiftIndex = -1;
		m_GiftType = 0;
		m_GiftCount = 0;
		m_GiftPercent = 0;
		m_GiftTotalCount = 0;
		m_GiftRemainTotalCount = 0;
		m_GiftEnsure = false;
		m_GiftResumeNextDay = false;
	}
	Lint m_GiftIndex;			// 奖励索引 自动生成
	Lstring m_GiftName;			// 奖励名称
	Lint m_GiftType;			// 奖励类型 1房卡（单位个） 2话费（单位元） 3 乐视电视（单位个） 4 IPad min4（单位个） 5 iPhone7 plus（单位个）
	Lint m_GiftCount;			// 奖励数量
	Lint m_GiftPercent;			// 奖励权值
	Lint m_GiftTotalCount;		// 配置的库存 -1表示无限
	Lint m_GiftRemainTotalCount;// 剩余的库存  -1表示无限
	bool m_GiftEnsure;			// 其他库存为0时 确保中的奖品
	bool m_GiftResumeNextDay;	// 隔日是否恢复库存
};

struct ActivityDraw : public Activity
{
	ActivityDraw()
	{
		m_SpendType = 0;
		m_SpendCount = 0;
		m_GiftTotalDate = 0;
		m_GiftTime = 0;
		m_TotalGiftPercent = 0;
	}

	Lint m_SpendType;			// 消耗类型 0免费 1房卡（单位个）
	Lint m_SpendCount;			// 消耗数量
	Lint m_GiftTotalDate;		// 库存日志 用于隔日恢复
	Lint m_GiftTime;			// 表示抽奖的次数
	std::vector<ActivityDrawGift> m_Gift;
	Lint m_TotalGiftPercent;	// 中奖总的权值
};

struct ActivityShare : public Activity
{
	ActivityShare()
	{
		m_GiftType = 0;
		m_GiftCount = 0;
	}

	Lstring m_GiftName;			// 奖励名称
	Lint m_GiftType;			// 励类型 1房卡（单位个）
	Lint m_GiftCount;			// 奖励数量
};

class ActiveManager:public LSingleton<ActiveManager>
{
public:
	virtual	bool		Init();
	virtual	bool		Final();

	// 转盘接口====================================================
	// 设置转盘数据 内部清除、解析和保存
	void SetDrawActiveConfig(const Lstring& strSet,int id);
	// 清除转盘数据
	void DrawSetClear();
	// 解析转盘数据 内部调用
	void _UpdateDrawActiveSet(const Lstring& strSet,int id);
	// 保存读取转盘配置
	void SaveDrawActiveSet();
	void ReadDrawActiveSet();
	// 是否开启转盘
	bool CheckDrawActiveValid();
	// 转盘ID
	int GetDrawActiveId();
	int GetDrawBeginTime();
	int GetDrawEndTime();
	// 转盘消耗
	int GetDrawSpendType();
	int GetDrawSpendCount();
	// 计算转盘中奖 fGift表示要生成的假数据
	ActivityDrawGift GetDrawGift( ActivityDrawGift& fGift );

	// 分享接口
	void UpdateShareActiveSet(const Lstring& strSet,int id);
	Lstring GetShareActiveSet(); 
	int GetShareActiveId();

	ActivityShare GetShareSet();

	bool CheckShareActiveValid();
	bool CheckShareActiveStart();
	void ShareSetClear();

	inline CRedisClient & GetRedisClient() { return m_RedisClient; }

private:
	boost::recursive_mutex m_Mutex;
	CRedisClient m_RedisClient;

	ActivityDraw m_Draw;
	ActivityShare m_Share;
};

#define gActiveManager ActiveManager::Instance()



#endif

