#ifndef _RUNTIME_INFO_MSG_H_
#define _RUNTIME_INFO_MSG_H_

#include "LBase.h"
#include "LSingleton.h"
#include "LMsg.h"

class CRuntimeInfoMsg : public LSingleton<CRuntimeInfoMsg>
{
public:
	CRuntimeInfoMsg();
	~CRuntimeInfoMsg();
public:
	virtual bool Init();
	virtual bool Final();
public:
	void setHorseInfoAndNotify(const Lstring& strInfo);
	Lstring getHorseInfo();

	void setBuyInfo(const Lstring& strInfo);
	Lstring getBuyInfo(int iUserId);

	void addNotice(const Notice& notice);
	std::vector<Notice> getNotice();

	void setHide(Lint iHide);
	Lint getHide();
public:
	void changeOnlineNum(Lint iGateId, Lint iUserId, bool bInc);
	Lint getOnlineNum(Lint iGateId);
	void clearOnlineNum(Lint iGateId);
public:
	void	setFreeTimeAndNotify(Lstring& strFreeTime);
	Lstring getFreeTime();
	bool	isFree( Lint gametype );	// gametype=-1表示是否全部免费
	bool	updateFree();	// 更新免费 即更新m_strFreeTime 返回值表示m_strFreeTime有修改

private:
	//后台推过来的
	boost::mutex m_mutexForBackground;

	Lstring	m_strHorseInfo;		//跑马灯
	Lstring	m_strBuyInfo;		//够买提示框
	std::vector<Lstring>	m_vecBuyInfo;	//购买信息
	std::vector<Notice>		m_vecNotice;	//通知
	Lint	m_iHide;
private:
	boost::mutex m_mutexForOnlineNum;
	std::map<Lint, std::map<int , int> > m_mapOnlinNumOnGate;	//gate上的在线人数
private:
	//程序本身内部

	// 免费配置
	boost::recursive_mutex m_mutexForFree;
	Lstring m_strFreeTime;			//免费配置 json格式 并不是后台传送过来的json 只包含已经开启的json配置
	struct BeginEnd
	{
		BeginEnd()
		{
			m_bstart = false;
		}
		time_t m_begin;
		time_t m_end;

		// 用于更新m_strFreeTime
		bool m_bstart;	// 是否已经开启
		Json::Value m_config;
	};
	std::map<Lint,BeginEnd> m_freeSet;	// -1表示全部免费的索引 其他表示对应GameType免费
};

#define gRuntimeInfoMsg  CRuntimeInfoMsg::Instance()

#endif