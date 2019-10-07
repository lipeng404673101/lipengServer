#pragma once
#ifndef _CLUB_DESK_H_
#define _CLUB_DESK_H_
#include "LBase.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "SafeResourceLock.h"
#include "LTime.h"
#include "clubPlay.h"

////////////////////////////////////////////////////////////////////////////////////////////
//俱乐部桌子
//保存俱乐部中桌子的信息
struct ClubDeskInfo :public CResourceLock {

private:
	Lint            m_isUsed;                              // 0 -没有  1-使用

	Lint            m_clubId;                              //俱乐部Id
	Lint            m_playTypeId;                    //玩法Id
	Lint            m_clubDeskId;                    //桌子
	Lint            m_showDeskId;                 //展示的桌子号
	Lint            m_realDeskId;                   //真实桌子
	Lint            m_lock;                               //是否锁  0-正常  1-被锁
	
public:
	std::list<deskUserInfo>   m_deskUserInfo;           //房间入座用户列表
	std::set<Lint>      m_lookonInfo;                              //房间旁观用户列表

	Lint            m_roomFull;                      //房间是否满
	Lint            m_currCircle;                   //当前局数
	Lint            m_totalCircle;                  //房间总局数
	Lint			    m_isClubOwerLooking;  //俱乐部会长是否正在明牌观战   0：不在    1：正在观战

	Lint            m_redPacketFlag;         //红包标志   0--没有  1--有
	Lint            m_flag;                            //玩法
	Lint            m_maxUserCount;              //最大人数

	//playTypeDetail  m_playDetail;

	ClubDeskInfo(Lint clubId, Lint deskId, Lint showDeskId, Lint playTypeId=0)
	{
		//桌子基本信息
		m_clubId = clubId;
		m_clubDeskId = deskId;
		m_showDeskId = showDeskId;
		
		rebootDesk();
		m_playTypeId = playTypeId;
		m_lock = 0;
	}

	void rebootDesk()
	{
		m_isUsed = 0;
		m_playTypeId = 0;
		m_realDeskId = 0;

		m_roomFull = 0;
		m_currCircle = 0;
		m_totalCircle = 0;

		
		m_redPacketFlag = 0;
		m_isClubOwerLooking = 0;
		m_flag = 0;
		m_maxUserCount = DESK_USER_COUNT;
		m_deskUserInfo.clear();
		m_lookonInfo.clear();
		//m_playDetail.clear();
	}

	//所属俱乐部
	Lint getClubId() { return m_clubId; }
	//7位桌号，唯一性
	Lint getClubDeskId() { return m_clubDeskId; }
	//展示桌子号  1，2，3
	Lint getShowDeskId() { return m_showDeskId; }
	//锁定
	bool canBeLock() { if (getUserCount() > 0)return false; return true; }
	Lint getLock() { return m_lock; }
	bool isLock() { return m_lock == 1 ? true : false; }
	void setLock() { m_lock = 1; }
	void delLock() { m_lock = 0; }
	//玩法
	Lint getPlayId() { return m_playTypeId; }
	void setPlayTypeId(Lint playTypeId)
	{
		m_playTypeId = playTypeId;
	}
	//真实桌子号
	Lint getRealDeskId() { return m_realDeskId; }
	void setRealDeskId(Lint realId,Lint playId)
	{
		m_realDeskId = realId;
		m_playTypeId = playId;
	}
	//桌子成员
	bool  empty() { return (m_deskUserInfo.empty() && m_lookonInfo.empty()) ? true : false; }
	Lint  getUserCount() { return m_deskUserInfo.size(); }
	void insertUserInfo(deskUserInfo &userInfo)
	{
		auto ItUser = m_deskUserInfo.begin();
		for (; ItUser != m_deskUserInfo.end(); ItUser++)
		{
			if (ItUser->m_userId == userInfo.m_userId) break;
		 }
		if (ItUser == m_deskUserInfo.end())
		{
			m_deskUserInfo.push_back(userInfo);
		}
	}
	void delUserInfo(Lint  userId)
	{
		auto ItUser = m_deskUserInfo.begin();
		for (; ItUser != m_deskUserInfo.end(); ItUser++)
		{
			if (ItUser->m_userId == userId)
			{
				m_deskUserInfo.erase(ItUser);
				break;
			}
		}
	}

	void insertLookon(Lint userId) {
		m_lookonInfo.insert(userId);
	}
	void delLookon(Lint userId) {
		m_lookonInfo.erase(userId);
	}

	//是否创建过
	bool isUsed()
	{
		return m_realDeskId == 0 ? false : true;
	}

};


class ClubDeskManager :public LSingleton<ClubDeskManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:
	
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > getClubDeskInfoByClubDeskId(Lint clubDeskId);                           //获取桌子数据信息

/////////////////////////////////////////////////////////////////
//对桌子进行设置
	//添加桌子信息
	void addClubDesk(Lint clubId,Lint clubDeskId, Lint showDeskId,Lint playId);                //添加桌子
	void delClubDesk(Lint clubDeskId);                                                                                                 //删除桌子

	void setPlayId(Lint clubDeskId, Lint playId, playTypeDetail & pDetail);                                                   //对桌子设置玩法
	void setRealId(Lint clubDeskId, Lint realDeskId, Lint playId, playTypeDetail & pDetail);                  //对桌子设置真实桌号

	desksInfo  getClubDeskInfo(Lint clubDeskId,Lint currPlayId);                                                                            //获取桌子信息
	Lint          getDeskInClubId(Lint clubDeskId);                                                                               //获取桌子所在的俱乐部ID

/////////////////////////////////////////////////////////////////
//用户对桌子影响，从logic反馈的消息
	void  addDesk(LMsgL2LMGUserAddClubDesk * addDesk);                                                     //加入桌子
	void  leaveDesk(LMsgL2LMGUserLeaveClubDesk* leaveDesk);                                              //离开桌子
	void leaveDesk(LMsgL2LMGUserListLeaveClubDesk* leaveDesk);                                       // 多人同时离开桌子
	void  updateDeskInfo(LMsgL2LMGFreshDeskInfo * freshDeskInfo);                                  //更新桌子数据
	void  recycleClubDesk(Lint clubDeskId);                                                                                      //解散桌子

	void modifyClubDeskInfoFromLogic(const LogicDeskInfo & deskInfo);                             //接收logic上数据，修复俱乐部桌子信息
	
	//重置一定数量的俱乐部桌子
	//void ResetClubDeskInfo(int clubId, int count);
	
private:


private:
	boost::mutex m_mutexClubDeskQueue;
	std::map<Lint, boost::shared_ptr<ClubDeskInfo> > m_mapId2ClubDeskInfo;          //用户数据  key=userId， value=俱乐部桌子数据



};






#define gClubDeskManager ClubDeskManager::Instance()


#endif