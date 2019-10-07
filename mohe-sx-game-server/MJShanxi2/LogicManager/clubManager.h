#ifndef _CLUB_MANAGER_H_
#define _CLUB_MANAGER_H_
//#include "User.h"
#include "LSingleton.h"
#include "SafeResourceLock.h"

#include "club.h"
#include "clubUser.h"
#include "clubPlay.h"



#define   CREATE_DESK_NUM_EACH_PLAY_TYPE    10
#define   CLUB_CREATOR_FEE_CHECK             10
#define   CLUB_DESK__USER_INFO_SEND_COUTN                  2
//俱乐部用户列表
#define CLUB_USER_PAGE_COUNT   10
//#define CLUB_USER_POINT_COUNT 100

#define DELAY_TIME_USER_WAIT_JOIN_IN_CLUB_DESK  300

//提前声明
class User;


struct clubFeeType
{
	Lint          m_clubFeeType;
	Lint          m_clubCreatorId;
	Lstring    m_clubCreatorName;

	Lint          m_playTypeCircle;        //这个玩法局数，需要根据它判断金币
	Lint          m_state;                           //大玩法
	std::set<Lint>  m_adminUser;

	clubFeeType()
	{
		m_clubFeeType=0;
		m_clubCreatorId=0;
		m_playTypeCircle=0;
		m_state = 0;
	}

};


struct PlayTypeJoinUserInfo:public CResourceLock
{
	Lint m_clubId;
	Lint m_userMax;
	Lint m_currUserMax;
	Lint m_state;                                          //游戏规则
	LTime m_time;
	std::set<Lint> userIdSet;
	std::map<Lint, Lstring> userGps;


	PlayTypeJoinUserInfo()
	{
		m_clubId = 0;
		m_userMax = 0;
		m_currUserMax = 0;
		m_state = 0;
		//m_time;
	}
	~PlayTypeJoinUserInfo(){}

	bool isFull()
	{
		return userIdSet.size() >= m_userMax ? true : false;
	}

	void setUserMax(Lint max)
	{
		m_userMax = max;
	}

	Lint addUser(Lint userId,Lstring stringGps)
	{
		if (userIdSet.empty())
		{
			m_time.Now();
		}
		userIdSet.insert(userId);
		userGps[userId] = stringGps;

		if (m_userMax < 3)return -1;

		//m_currUserMax++;
		if (userIdSet.size() >= m_userMax)
		{
			return 1;
		}


		
		return 0;
	}
	Lint leaveUser(Lint userId)
	{
		userIdSet.erase(userId);

		std::map<Lint, Lstring>::iterator key = userGps.find(userId);
		if (key != userGps.end())
		{
			userGps.erase(key);
		}
	
		return 0;
	}

	void clearUser()
	{
		userIdSet.clear();
		userGps.clear();
	}


};

//
struct UserAddToClubDeskInfo
{
	Lint			m_userid;
	Lint			m_gateId;
	Lstring			m_strUUID;
	Lint			m_deskID;
	Lstring			m_ip;
	LUser			m_usert;
	Lint            m_creatDeskCost;
	Lint            m_cost;
	Lint            m_even;
	Lint            m_Free;
	Lint            m_startTime;
	Lint            m_endTime;
	Lint            m_addDeskFlag;
	Lint			m_type;    

	UserAddToClubDeskInfo()
	{
		m_userid=0;
		m_gateId=0;
		m_deskID=0;
		m_creatDeskCost=0;
		m_cost=0;
		m_even=0;
		m_Free=0;
		m_startTime=0;
		m_endTime=0;
		m_addDeskFlag = 0;
		m_type = 0;
	}
};


//////////////////////////////////////////////////////////////////////////
//俱乐部管理
class ClubManager : public LSingleton<ClubManager>
{
public:
	ClubManager();
	~ClubManager();

	virtual	bool		Init();
	virtual	bool		Final();

public:
	void    resetData();

	//获取俱乐部
	boost::shared_ptr<CSafeResourceLock<Club> > getClubbyClubId(Lint clubId);

////////////////////////////////////////////////////////////////////////////////////////////
//俱乐部操作
	//俱乐部添加/删除/更新 操作
	//Lint    updateClub(LMsgCe2LClubInfo*msg);
	Lint 	addClub(LMsgCe2LAddClub *msg);                                   
	Lint    delClub(Lint clubId);



public:
	void    addClubPlayType(LMsgCe2LMGClubAddPlayType *msg);
	Lint    hideClubPlayType(Lint clubId, Lint playTypeId, Lstring playType, Lint playTypeIdStatus);               //隐藏玩法
	Lint    alterClubName(Lint clubId, Lstring newClubName);                                                                                      //修改俱乐部名字
	Lint    modifyClubFeeType(Lint clubId, Lint feeType);                                                                                              //修改付费方式
	Lint    alterPlayTypeName(Lint clubId, Lint playTypeId, Lstring newPlayTypeName);                                 //修改玩法名称


////////////////////////////////////////////////////////////////////////////////////////////
//俱乐部会员操作
	//会长和管理员的特权行为
	Lint    userCreateClub(LMsgC2SCreateClub*msg, User* pUser);                   //创建俱乐部
	Lint    userAddClubPlay(LMsgC2SCreateDesk *msg, User* pUser);             //添加玩法
	Lint    userUpdateClub(LMsgC2SUpdateClub *msg, User* pUser);             // 更新俱乐部
	Lint    userUpdatePlay(LMsgC2SUpdatePlay *msg, User *pUser);            //更新玩法
	void   userChangePoint(LMsgC2SChangePoint*msg,User* pUser);            //改变能量值
	void   userSetClubAdministrtor(LMsgC2SSetClubAdministrtor* msg, User* pUser);     //设置管理员
	void   userLockClubDesk(LMsgC2SLockClubDesk* msg, User* pUser);            //锁定桌子
	
	//成员动作
	Lint    userEntryClub(LMsgC2SEnterClub*msg, User* pUser);                                                                                                  //用户加入俱乐部
	Lint    userLeaveClub(LMsgC2SLeaveClub*msg, User* pUser);                                                                                                  //用户加入俱乐部
	void   userJoinClubDesk(LMsgC2SAddDesk* msg, User* pUser, Lint addDeskType = 0);                                                                            //用户加入俱乐部房间
	void    userQuickJoinClubDesk(LMsgC2SQuickAddDesk* msg,User* pUser);                                                        //用户快速加入房间

	//查询俱乐部信息，成员，能量值，能量改变记录
	void     userRequestClubMember(LMsgC2SRequestClubMemberList *msg, User *pUser);
	void     userPointList(LMsgC2SPowerPointList* msg, User *pUser);
	void     userRequestPointRecord(LMsgC2SRequestPointRecord*msg, User *pUser);

	//统计俱乐部中勋章玩家
	std::set<Lint>	userStatisticsClubMedalUser();


private:
	Lint    _userEnterClubDesk(Lint clubId, Lint playTypeId, Lint clubDeskId, UserAddToClubDeskInfo  userInfo, User* pUser);         //玩家加入俱乐部桌子
	Lint   _createClubDesk(Lint clubId, Lint playTypeId, Lint clubDeskId, UserAddToClubDeskInfo  userInfo, User* pUser);                //创建桌子
	Lint   _addClubDesk(Lint clubId, Lint playTypeId, Lint clubDeskId, UserAddToClubDeskInfo  userInfo, User* pUser);                   //加入桌子
	//广播需要刷新俱乐部界面的用户
	void  broadcastClubScene(Lint clubId,Lint exUser=0);
	void  broadcastClubMsg(Lint clubId, LMsgSC& msg,Lint userId=0);
	//成员列表排序  按照自己，会长，管理员，在线，游戏中，离线顺序排序
	std::vector<boost::shared_ptr<ClubUser> >   _sortClubUserList(Lint clubId,Lstring nike , std::vector<Lint> & delUser, Lint myId);

	//logic 返回信息，刷新俱乐部信息
public:
	void  userAddClubDeskFromLogic(LMsgL2LMGUserAddClubDesk*msg);                               //用户加入桌子
	void  userLeaveClubDeskFromLogic(LMsgL2LMGUserLeaveClubDesk* msg);                       //用户离开桌子
	void  userLeaveClubDeskFromLogic(LMsgL2LMGUserListLeaveClubDesk* msg);      
	void  freshClubDeskInfoFromLogic(LMsgL2LMGFreshDeskInfo*msg );                                  //更新桌子
	void  recycleClubDeskId(LMsgL2LMGRecyleDeskID *msg);                                                        //解散桌子
	void recycleClubDeskId(Lint clubDeskId);

	void updateClubUserPointFromLogic(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);   //更新分数
	void modifyClubDeskInfoFromLogic(const LogicDeskInfo & deskInfo);                                 //logic同步桌子信息，同步到manager

// center 返回信息
public:
	//会员加入房间
	void    userJoinClubDeskFromCenter(LMsgCe2LUpdateCoinJoinDesk *msg, User* pUser);                                                                    //用户加入俱乐部房间
	//对会员能力值改变
	void    userChangePointFromCenter(LMsgLMG2CEUpdatePointRecord*msg);
	//对俱乐部会员管理
	void    managerClubUserFromCenter(LMsgCe2LMGClubAddUser * userInfo);
	//查询能量变化日志
	void userRequestPointRecordFromCenter(LMsgCE2LMGRecordLog *msg);
	//更新俱乐部
	Lint    userUpdateClubFromCenter(LMsgCE2LMGClubInfo *msg);
	//更新玩法
	Lint    userUpdatePlayFromCenter(LMsgCE2LMGPlayInfo *msg);
	
private:
	Lint    _addClubUser(clubUser & userInfo);             //加入会员
	Lint    _delClubUser(clubUser & userInfo);             //删除会员
	Lint   _applyClubUser(clubUser & userInfo);        //申请加入
	Lint  _getFreeClubDeskId(Lint clubId);               //获取空闲桌子




public:
//获取俱乐部界面
	bool getClubScene(Lint userId, Lint clubId, LMsgS2CClubScene& scene);                                                                        //获取用户俱乐部界面
	//bool getFreshClubScene(Lint userId, Lint clubId, LMsgS2CSwitchClubScene& scene);                                                       //获取用户俱乐部界面
private:
	Lint  _getClubScene(Lint clubId, LMsgS2CClubScene & scene);                                                                                              //获取俱乐部界面
	//Lint  _getFreshClubScene(Lint clubId, LMsgS2CSwitchClubScene & scene);                                                                         //获取俱乐部界面
	std::vector<Lint>  _orderClubDesk(const std::set<Lint> clubDeskSet);       //对俱乐部桌子排序


public:
	

	Lint    userApplyJoinPlayTypeDesk(Lint clubId, Lint playTypeId, User* pUser, struct PlayTypeJoinUserInfo *playTypeUser, Lint *isCheckGps);             //
	Lint    userApplyLeavePlayTypeDesk(Lint clubId, Lint playTypeId, User * pUser, struct PlayTypeJoinUserInfo *playTypeUser);          //

	Lint    getClubFeeType(Lint clubId, clubFeeType& feeType, Lint playTypeId=0);
	
	bool    getPlayTypeJoinUserInRoom(Lint playTypeId, struct PlayTypeJoinUserInfo *playTypeUser);



	Lint   userCreateClubDesk(User* pUser, Lint clubId, Lint playTypeId, UserAddToClubDeskInfo & userInfo);

	Lint    getClubDeskCanJoin(Lint clubId, Lint playTypeId);              //获取俱乐部空桌子号


	boost::shared_ptr<CSafeResourceLock<PlayTypeJoinUserInfo> > getUserInfobyPlayTypeId(Lint playTypeId);
	void updateClubUserPlayTypeWait0(std::set<Lint> &userIds);

	//修改玩家申请加入俱乐部未被处理的玩家数量,setFlag  0：增加  1：减少
	bool setUserApplyClubUndoCount(Lint clubId, bool setFlag);

	//根据俱乐部ID获取申请该俱乐部且未被处理的玩家数量
	Lint getUserApplyClubUndoCount(Lint clubId);


	
private:
	//俱乐部列表
	boost::mutex m_mutexClubListQueue;
	std::map<Lint, boost::shared_ptr<Club> > m_mapClubList;        //first=clubId   俱乐部列表     
	
	//玩法大厅的用户
	boost::mutex m_mutexPlayTypeJoinUserInfo;
	std::map<Lint, boost::shared_ptr<PlayTypeJoinUserInfo>> m_mapPlayTypeJoinUserInfo;

	//玩家申请加入俱乐部未被处理的玩家数量
	boost::mutex m_mutexUserApplyClubUndo;
	std::map<Lint, Lint> m_mapUserApplyClubUndo;	//map<clubId, applyUserCount>
	
};

#define gClubManager ClubManager::Instance()

#endif