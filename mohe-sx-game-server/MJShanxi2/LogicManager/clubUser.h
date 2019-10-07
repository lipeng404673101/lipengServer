#pragma once

#ifndef _CLUB_USER_H_
#define _CLUB_USER_H_

#include "SafeResourceLock.h"
#include "LSingleton.h"
#include "LMsg.h"
#include "LMsgS2S.h"


/* 玩家在线状态（0：不在线  1：在线）*/
enum USER_IN_CLUB_STATUS
{
	USER_IN_CLUB_OFFLINE = 0,	//不在线
	USER_IN_CLUB_ONLINE = 1,	//在线
};

/* 玩家是否游戏状态（0：不在游戏中  1：在游戏中）*/
enum USER_STATE
{
	USER_STATE_INIT = 0,	//不在游戏中
	USER_STATE_PLAY = 1,	//游戏中
};

/* 玩家能量值结构体 */
struct UserPowerPoint
{
	Lint  m_userId;			//玩家ID
	Lint  m_clubId;			//玩家俱乐部ID
	Lint  m_point;			//玩家在该俱乐部中能量值
	Lint  m_medalPoint;		//能量场玩家输赢分数统计，用于勇士勋章

	UserPowerPoint(Lint userId, Lint clubId, Lint point)
	{
		Lint t_oldPoint = m_point;

		m_userId = userId;
		m_clubId = clubId;
		m_point = point;
		m_medalPoint = 0;

		LLOG_ERROR("clubUser::UserPowerPoint Init... userId=[%d] clubId=[%d] cheangePoint=[%d], oldPoint[%d], medalPoint=[%d]", 
			m_userId, m_clubId, point, t_oldPoint, m_medalPoint);
	}

	void clear()
	{
		m_userId = 0;
		m_clubId = 0;
		m_point = 0;
		m_medalPoint = 0;
	}

	/* 获取玩家能量值 */
	Lint getPoint() { return m_point; }

	/* 设置玩家能量值 */
	void setPoint(Lint point) { m_point = point; }

	/* 给玩家添加能量值 */
	void addPoint(Lint addPoint) 
	{  
		if (addPoint > 0) m_point += addPoint;
		LLOG_ERROR("clubUser::UserPowerPoint::addPoint() Run... userId=[%d] clubId=[%d] addPoint=[%d] totalPoint=[%d]", 
			m_userId, m_clubId, addPoint, m_point);
	}

	/* 给玩家减少能量值 */
	void reducePoint(Lint  delPoint) 
	{
		if (delPoint > 0) m_point -= delPoint;
		LLOG_ERROR("clubUser::UserPowerPoint::reducePoint() Run... userId=[%d] clubId=[%d] reducePoint=[%d] totalPoint=[%d]", 
			m_userId, m_clubId, delPoint, m_point);
	}
};

/* 玩家所在当前俱乐部信息 */
struct  UserClubInfo
{
	Lint  m_clubId;			//当前所在俱乐部ID
	Lint  m_playTypeId;		//当前玩法信息ID
	Lint  m_clubDeskId;		//当前所在俱乐部中桌子ID
	//Lint  m_pos;			//在桌子上的位置

	UserClubInfo()
	{
		m_clubId = 0;
		m_playTypeId = 0;
		m_clubDeskId = 0;
		//m_pos = INVAILD_POS;
	}
	UserClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId)
	{
		m_clubId = clubId;
		m_playTypeId = playTypeId;
		m_clubDeskId = clubDeskId;
	}
	void reset()
	{
		m_clubId = 0;
		m_playTypeId = 0;
		m_clubDeskId = 0;
		//m_pos = 21;
	}
};

class ClubUserManager;

/* 玩家所有俱乐部信息 */
struct ClubUser :public CResourceLock
{
	friend ClubUserManager;
private:
	Lint            m_userId;				//用户id

protected:
	Lint            m_online;               // 玩家是否在线 0--不在线  1-在线 
	Lint            m_userState;            // 玩家是否在游戏中 0--不在游戏中 ，1--游戏中

	/* 玩家所有俱乐部中能量值 map<clubId, 玩家俱乐部中的能量信息> */
	std::map <Lint, boost::shared_ptr<UserPowerPoint>>  m_powerPoint;

public:
	//用户基本信息
	Lstring			m_nike;                 //用户昵称
	Lstring			m_headImageUrl;         //用户头像
	Lstring			m_ip;					//用户ip
	Lstring			m_unionId;				//用户昵称
	Lint            m_userLevel;
	Lint			m_offLineTime;          //掉线时间
	Lint            m_lastEnterTime;        //最新加入俱乐部时间     //暂时不用
	Lint            m_lastLeaveTime;        //最新离开俱乐部时间     //暂时不用

	UserClubInfo    m_currClubInfo;			//用户当前俱乐部信息

	 //用户所有俱乐部信息
	std::set<Lint>  m_allClubId;			//用户加入的所有俱乐部Id
	std::set<Lint>  m_adminClub;			//用户是管理员俱乐部id
	std::set<Lint>  m_ownerClub;			//用户是会长俱乐部id


	Lint			m_currWaitPlayType;


	ClubUser()
	{
		m_userId = 0;
		m_currWaitPlayType = 0;
		m_userLevel = 0;
		m_online = 0;
		m_lastEnterTime = 0;
		m_lastLeaveTime = 0;
		m_offLineTime = 0;
		m_userState = USER_STATE_INIT;
		//m_type = 1;
	}

	~ClubUser() {}

	ClubUser(Lint userId, Lint clubId)
	{
		m_userId = userId;
		m_allClubId.insert(clubId);
		m_userLevel = 0;
		m_online = 0;
		m_lastEnterTime = 0;
		m_lastLeaveTime = 0;
		m_offLineTime = time(NULL);
		m_userState = USER_STATE_INIT;
		//m_type = 1;
	}

	/* 获取玩家ID */
	Lint getID() { return m_userId; }

	/* 获取玩家是否在俱乐部 */
	Lint getLine() { return m_online; }

	/* 设置玩家是否在俱乐部中的标志 */
	void setLine(Lint line) { m_online = line; }

	/* 判断玩家是否在游戏中 */
	bool isPlay() { return m_userState == USER_STATE_PLAY ? true : false; }

	/* 设置玩家是否在游戏中的标志 */
	void setState(Lint userState) { m_userState = userState; }

	//返回给客户端状态  0-不在线（读取时间）  1-在线（未参加游戏） 2-在线（游戏中）
	Lint getClientState()
	{
		Lint  clientState = getLine();
		if (isPlay())clientState = 2;
		return clientState;
	}

	/* 玩家以type身份加入clubId的俱乐部中 */
	void addNewClub(Lint clubId, Lint type)
	{
		//该玩家所有的俱乐部
		m_allClubId.insert(clubId);

		//玩家为该俱乐部的管理员
		if (type == USER_IN_CLUB_TYPE_ADMIN)
		{
			m_adminClub.insert(clubId);
		}
		//玩家为该俱乐部的会长
		else if (type == USER_IN_CLUB_TYPE_OWNER)
		{
			m_ownerClub.insert(clubId);
		}
	}

	/* 从clubId的俱乐部中删除该玩家 */
	void delFromClub(Lint clubId, Lint type)
	{
		m_allClubId.erase(clubId);
		m_adminClub.erase(clubId);
		m_ownerClub.erase(clubId);
	}

	void setCurrWaitPlayType(Lint playType)
	{
		m_currWaitPlayType = playType;
	}

	Lint getCurrWaitPlayType()
	{
		//LLOG_DEBUG("HHHWYZ getCurrWaitPlayType userId[%d]  m_currWaitPlayType=[%d]", m_userId, m_currWaitPlayType);
		return m_currWaitPlayType;
	}

	/* 获取用户在clubId的俱乐部中身份  1：普通会员  2：管理员  3：会长 */
	Lint getUserType(Lint clubId)
	{
		if (m_ownerClub.find(clubId) != m_ownerClub.end())return USER_IN_CLUB_TYPE_OWNER;
		if (m_adminClub.find(clubId) != m_adminClub.end()) return USER_IN_CLUB_TYPE_ADMIN;
		return USER_IN_CLUB_TYPE_NORMAL;
	}

	/* 获取用户在clubId的俱乐部中能量值 */
	Lint getClubPoint(Lint clubId)
	{
		auto ItPoint = m_powerPoint.find(clubId);
		if (ItPoint != m_powerPoint.end())
		{
			return ItPoint->second->m_point;
		}
		else
		{
			return 0;
		}
	}

	//只有在生成用户的时候用到
	void setClubPoint(Lint clubId, Lint point)
	{
		if (clubId == 0)return;
		auto ItPoint = m_powerPoint.find(clubId);
		if (ItPoint != m_powerPoint.end())
		{
			ItPoint->second->m_point = point;
			ItPoint->second->m_medalPoint = 0;
		}
		else
		{
			boost::shared_ptr<UserPowerPoint> userPoint(new UserPowerPoint(getID(), clubId, point));
			m_powerPoint[clubId] = userPoint;
		}
	}

	/* 更新玩家在clubId的俱乐部中能量值 */
	bool updateClubPoint(Lint clubId, Lint point)
	{
		if (clubId == 0)return false;
		auto ItPoint = m_powerPoint.find(clubId);
		if (ItPoint != m_powerPoint.end())
		{
			ItPoint->second->m_point += point;
			ItPoint->second->m_medalPoint += point;
		}
		else
		{
			boost::shared_ptr<UserPowerPoint> userPoint(new UserPowerPoint(getID(), clubId, point));
			m_powerPoint[clubId] = userPoint;
		}
	}

};

struct ClubOwner :public ClubUser
{
	
	//用户所创建的俱乐部
	std::set<Lint>   m_CreateClubId;

	ClubOwner()
	{
		
	}
	~ClubOwner() 
	{

	}
};


class ClubUserManager :public LSingleton<ClubUserManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:
	void addClubUser(clubUser &userInfo, Lint online = USER_IN_CLUB_OFFLINE);
	//void addClubUser(Lint clubId, Lint iUserId, Lstring nike, Lstring headurl,Lint type,Lint online= USER_IN_CLUB_OFFLINE);
	//void delClubUser(Lint clubId, Lint iUserId);
	void delClubUser(clubUser &userInfo);

	void userOffLine(Lint iUseId);
	void userOnLine(Lint iUseId , bool IsPlay);
	void setUserPlayStatus(Lint iUseId, bool IsPlay);

	void userLoginClub(Lint clubId, Lint userId);
	void userLogoutClub(Lint clubId, Lint userId);

	void  userAddDesk(LMsgL2LMGUserAddClubDesk*msg);
	void  userLeaveDesk(LMsgL2LMGUserLeaveClubDesk* msg);
	void  userLeaveDesk(LMsgL2LMGUserListLeaveClubDesk* msg);



	void setUserAdmin(Lint type, Lint clubId, Lint userId);    	//设置俱乐部管理员
	bool isUserInClub(Lint clubId, Lint userId);                           //判断用户是否在俱乐部中
	Lint  getDefaultClubId(Lint userId);                                         //进入俱乐部时获取用户默认的clubId

/////////////////////////////////////////////////////////////////////////////////////////
//俱乐部能量点
public:
	Lint  addPoint(Lint userId, Lint clubId, Lint point) { return _changePoint(userId, clubId, point, CHANGE_POINT_TYPE_ADD); }
	Lint  reducePoint(Lint userId, Lint clubId, Lint point) { return _changePoint(userId, clubId, point, CHANGE_POINT_TYPE_REDUCE); }
	Lint  updatePoint(Lint userId, Lint clubId,Lint point);
	Lint  getPoint(Lint userId, Lint clubId);

	//获取俱乐部中玩家赢的最多的能量值玩家ID
	std::list<Lint>  getMaxPointUserId(Lint clubId);

	//置零俱乐部中所有玩家赢的能量值
	void  clearClubUserMedal();
private:
	
	Lint  _changePoint(Lint userId, Lint clubId, Lint point, Lint type);             // type = 0 减   1 =加
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	boost::shared_ptr<CSafeResourceLock<ClubUser> > getClubUserbyUserId(Lint iUserId);                           //获取用户数据信息
	boost::shared_ptr<CSafeResourceLock<ClubUser>> getClubUserbyClubIdUserId(Lint clubId, Lint iUserId);        //获取用户数据信息根据俱乐部ID和玩家ID
	std::set<boost::shared_ptr<ClubUser>  > getClubUserbyUserNike(Lint clubId, Lstring nike);                      //根据昵称获取用户数据信息

	
	std::set<boost::shared_ptr<ClubUser>  > getClubAllUser(Lint clubId);                            //获取俱乐部全部用户数据信息
	std::set<boost::shared_ptr<ClubUser>  > getClubAllOnLineUser(Lint clubId);               //获取俱乐部在线用户数据信息

	std::set<boost::shared_ptr<ClubUser>  > getClubUserInGame(Lint clubId);                      //获取在游戏中用户信息
	std::set<boost::shared_ptr<ClubUser>  > getClubAllOnLineUserNoGame(Lint clubId);   //获取不在游戏中在线用户信息
	std::set<boost::shared_ptr<ClubUser>  > getClubAllOffLineUserNoGame(Lint clubId);  //获取不在游戏中掉线用户信息

	std::set<Lint> getClubAllOnLineUserId(Lint clubId);                                               //获取俱乐部在线用户Id
	std::set<Lint> getClubPlayTypeUserId(Lint clubId, Lint playTypeId);               //获取俱乐部玩法的用户id 包括在房间的
	std::set<Lint> getNeedFreshSceneUserId(Lint clubId);                                         //获取需要刷新这个俱乐部大厅的用户（在俱乐部同时没有在房间里用户)

	Lint getUserInAllClubCount(const std::set<Lint> &clubIdList, std::vector<clubOnlineUserSize> &clubOnlineSize); // 获取俱乐部在线人数（多个俱乐部的）

	Lint getClubUserCountOnline(Lint clubId);        //获取俱乐部在线人数
	Lint getClubUserCount(Lint clubId);         //获取俱乐部人数


private:
	boost::mutex m_mutexClubUserQueue;
	std::map<Lint, boost::shared_ptr<ClubUser> > m_mapId2ClubUser;          //用户数据  key=userId， value=俱乐部用户数据

	std::map <Lint, std::set<Lint> > m_mapClubId2ClubUserIdList;                  //俱乐部用户列表 key=clubId   value=所用俱乐部用户

	std::map<Lint, std::set<Lint> >   m_mapClubId2ClubOnLineUserIdList;   //俱乐部用户在俱乐部大厅用户列表  key=clubId  value=在俱乐部的用户列表（可能掉线）

};


#define gClubUserManager ClubUserManager::Instance()








#endif
