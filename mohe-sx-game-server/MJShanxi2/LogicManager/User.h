#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "..\LogicQipai\LMsgPoker.h"
#include "LActive.h"

#include "SafeResourceLock.h"

#include "clubManager.h"

enum USER_CLUB_STATUS
{
	USER_CLUB_INIT_STATUS = 0,
	USER_CLUB_PIPEI_STATUS = 1,
	USER_CLUB_DESK_STATUS = 2,
};

class User : public CResourceLock
{
public:
	User(LUser data, Lint gateId);
	virtual ~User();

	virtual void	Tick();

	//获取玩家数据库里id
	Lint	GetUserDataId();

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	bool	GetOnline();

	void	Login();
	void	Logout();

	void	Send(LMsg& msg);
	void	Send(const LBuffPtr& buff);

	void	SendLoginInfo(Lint lastLoginTime);
	void	SendItemInfo();
	void	SendHorseInfo();

	void    SendFreeInfo();
	void    SendExchInfo();

	void	S2CeUserLogin();
	void	S2CeUserLogout();

	void	HanderMsg(LMsg* msg);
	//玩家创建房间
	void	HanderUserCreateDesk(LMsgC2SCreateDesk* msg);
	//实际玩家创建房间
	void	RealDoHanderUserCreateDesk(LMsgCe2LUpdateCoin* msg);
	//玩家创建房间为其他玩家
	void	HanderUserCreateDeskForOther(LMsgC2SCreateDeskForOther* msg);

	//玩家创建房间为其他玩家
	void	RealDoHanderUserCreateDeskForOther(LMsgCe2LUpdateCoin* msg);

	//获取玩家创建房间列表
	void	HanderUserCreatedDeskListForOther(LMsgC2SDeskList* msg);

	//获取玩家创建房间列表
	void	HanderUserDismissCreatedDeskForOther(LMsgC2SCreaterResetRoom* msg);

	//会长请求解散俱乐部桌子
	void    HanderUserDismissClubDeskWithMaster(LMsgC2SClubMasterResetRoom*msg);

	//玩家加入房间
	void   HanderUserJoinRoomCheck(LMsgC2SJoinRoomCheck*msg);

	//玩家进入房间
	void	HanderUserAddDesk(LMsgC2SAddDesk* msg,Lint addDeskFlag =0);

	//玩家进入房间
	void	RealDoHanderUserAddDesk(LMsgCe2LUpdateCoinJoinDesk* msg);

	//进步金币桌子
	void	HanderUserEnterCoinsDesk(LMsgC2SEnterCoinsDesk* msg);
	//查询桌子GPS信息
	void    HanderUserQueryRoomGPSInfo(MHLMsgC2SQueryRoomGPSLimitInfo * msg);

	
	//玩家请求战绩
	void	HanderUserVipLog(LMsgC2SVipLog* msg);

	void    HanderUserCreLog(LMsgC2SCRELog* msg);

	void    HanderUserReqGTU(LMsgC2SREQ_GTU* msg);

	void    HanderUserReqEXCH(LMsgC2SREQ_Exch* msg);

	void	HanderUserRoomLog(LMsgC2SRoomLog* msg);

	//玩家请求录像
	void	HanderUserVideo(LMsgC2SVideo* msg);

	//获取指定玩家信息
	void	HanderGetUserInfo(LMsgC2SGetUserInfo* msg);

	void    HanderBindInviter(LMsgC2SBindInviter* msg);

	void    HanderGetInvitingInfo(LMsgC2SGetInvitingInfo *msg);

	// 活动相关
	void    HanderActivityPhone(LMsgC2SActivityPhone *msg);
	void    HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg);
	void    HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg);
	void    HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg);
	void    HanderActivityRequestShare(LMsgC2SActivityRequestShare *msg);

	//玩家请求获取某分享id录像
	void    HanderUserShareVideo(LMsgC2SShareVideo* msg);

	//玩家请求分享某录像
	void    HanderUserReqShareVideo(LMsgC2SReqShareVideo* msg);

	//玩家请求获取分享的录像
	void	HanderUserGetShareVideo(LMsgC2SGetShareVideo* msg); 

	Lstring GetIp();

	void	SetIp(Lstring& ip);
	
	//房卡相关操作
	//获取房卡数量
	bool	IfCardEnough(Lint cardType, Lint num);
	//增加房卡
	void	AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave = true);
	//删除房卡
	void	DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave = true);
	//修改玩家打的局数，更改数据
	void	AddPlayCount();

	//static void	AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType, const Lstring& admin);
	void    AddCreditForUserlist(const std::vector<Lint> &userList);

	bool    DealExchangeCard(Lint inCard);

	void	AddCoinsCount(Lint count, Lint operType, bool bNeedSave = true);
	void	DelCoinsCount(Lint count, Lint operType, bool bNeedSave = true);

	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserLogicID(){return m_logicID;}
	void	setUserLogicID(Lint nValue){m_logicID = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	Lstring&	getUserIPStr(){return m_ip;}

	Lint	getLastCreateGameType() { return m_lastCreateGameType; }
	/////////////////////////////////////////////////////////////////////////////
	//关于俱乐部
	//加入俱乐部之前检查房费
	void   HanderUserAddDeskBeforeCheckFee(LMsgC2SAddDesk* msg,Lint addDeskType=0);

	void HanderUserPokerRoomLog(LMsgC2SQiPaiLog* msg);

	//斗地主俱乐部匿名
	void  HanderUserJoinRoomAnonymous(MHLMsgC2SJoinRoomAnonymous *msg);
	//俱乐部防作弊入座匹配进入俱乐部方式
	void  HanderUserJoinRoomAnonymous2CheckFee(MHLMsgC2SJoinRoomAnonymous *msg);
	void  HanderUserJoinRoomAnonymous2(LMsgCe2LUpdateCoinJoinDesk *msg);
	void  HanderQiPaiUserLeaveWait(LMsgC2SPokerExitWait *msg);

	//棋牌-斗地主请求战绩
	void HanderQiPaiUserReqDraw(LMsgC2SMatchLog *msg);
	void HanderQiPaiUserReqLog(LMsgC2SPokerVideo *msg);

	//

	void UpdateUserData(const LUser & userData);

	//创建俱乐部桌子
	void HanderUserCreateClubDesk(LMsgC2SClubUserCreateRoom* msg);
	void RealDoHanderUserCreateClubDesk(LMsgCe2LUpdateCoinJoinDesk* msg);


	void HanderUserRequestClubOnLineCount(LMsgC2SRequestClubOnLineCount *msg);

	//俱乐部邀请成员
	void HanderUserRequestClubMemberOnlineNoGame(LMsgC2SRequestClubMemberOnlineNogame *msg);

	void HanderUserInviteClubMember(LMsgC2SInviteClubMember *msg);

	void HanderUserReplyClubMemberInvite(LMsgC2SReplyClubMemberInvite *msg);

	//能力值
	void HanderUserPointList(LMsgC2SPowerPointList* msg);
	
	/////////////////////////////////////////////////////////////////////////////
	void setUserClubStatus(Lint status) { m_clubStatus = status; }
	Lint getUserClubStatus() { return m_clubStatus; }
	
public:
	LUser	m_userData;					//玩家数据
	std::vector< LActive >	m_Active;	//活动信息
	Lint	m_gateId;					//玩家在哪个gate上面
	Lint	m_logicID;					//玩家在哪个logic上面
	Lint	m_userState;				//玩家当前状态
	bool	m_online;
	
	Lstring	m_ip;

	Lint m_lastCreateGameType;			//玩家最近一次创建的游戏类型

	Lint m_clubStatus;


};

class UserBaseInfo : public CResourceLock
{
public:
	UserBaseInfo()
	{
		m_id  = 0;
		m_sex = 1;
	}
	UserBaseInfo(const UserIdInfo& info)
	{
		copy(info);
	}

	virtual ~UserBaseInfo()
	{

	}
public:
	UserBaseInfo& operator =(const UserIdInfo& info)
	{
		copy(info);
		return *this;
	}

	void copy(const UserIdInfo& info)
	{
		m_id			= info.m_id;
		m_openId		= info.m_openId;
		m_unionId       = info.m_unionId;
		m_nike			= info.m_nike;
		m_headImageUrl	= info.m_headImageUrl;
		m_sex			= info.m_sex;
	}
public:
	Lint		m_id;
	Lstring		m_openId;
	Lstring		m_unionId;
	Lstring		m_nike;
	Lstring		m_headImageUrl;
	Lint		m_sex;
};

class UserLoginInfo : public CResourceLock
{
public:
	UserLoginInfo()
	{
		m_seed = 0;
		m_time = 0;
	}
	UserLoginInfo(Lint iSeed, Lint iTime, LUser& user)
	{
		m_seed = iSeed;
		m_time = iTime;
		m_user = user;
	}

	virtual ~UserLoginInfo()
	{

	}

public:
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

class UserDeskList : public CResourceLock
{
public:
	UserDeskList()
	{
		m_cost = 0;
	}


	virtual ~UserDeskList()
	{

	}

public:
	std::list<int> m_DeskList;
	Lint m_cost;
};


#endif