#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"
#include "LTime.h"
#include "mhmsghead.h"
class Desk;

class  User
{
public:
	User(LUser data, Lint gateId);
	virtual ~User();

	//获取玩家数据库里 id
	Lint	GetUserDataId();

	const LUser& GetUserData() const { return m_userData; };

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	bool	GetOnline();
	void	SendLoginInfo(Lstring& buyInfo, Lint hide);
	void	SendItemInfo();

	bool	Login();
	void	Logout();

	void	Send(LMsg& msg);

	Desk*	GetDesk();
	void	SetDesk(Desk* desk);

	void	HanderMsg(LMsg* msg);
	//玩家创建房间
	Lint	HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg);

	Lint    HanderUserCreateDeskForOther(LMsgLMG2LCreateDeskForOther* msg);

	//Lint    HanderUserCreateDeskForClub(LMsgLMG2LCreateDeskForClub* msg);

	//玩家进入房间
	Lint	HanderUserAddDesk(Lint nDeskID);


	//玩家进入房间
	Lint	HanderUserAddDesk(LMsgLMG2LAddToDesk* msg);

	//玩家退出房间
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg);

	//俱乐部
	Lint	HanderUserAddClubDesk(LMsgLMG2LEnterClubDesk* msg);


	//玩家请求出牌
	void	HanderUserPlayCard(LMsgC2SUserPlay* msg);

	//玩家请求思考
	void	HanderUserThink(LMsgC2SUserOper* msg);

	//玩家选择起手胡
	void	HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg);

	//玩家选择海底牌
	void	HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg);

	//玩家请求通知内容
	void	HanderUserReady(LMsgC2SUserReady* msg);

	//玩家选择座位
	void	HanderUserSelectSeat(LMsgC2SUserSelectSeat* msg);

	//玩家开始按钮
	void    HanderUserStartGameButton(LMsgC2SStartGame*msg);

	//申请解算房间
	void	HanderResutDesk(LMsgC2SResetDesk* msg);

	//玩家选择解算操作
	void	HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg);

	//玩家请求聊天
	void	HanderUserSpeak(LMsgC2SUserSpeak* msg);

	// 玩家换牌
	void HanderUserChange(LMsgC2SUserChange* msg);

	void HanderUserTangReq(LMsgC2STangCard* msg);

	void HanderUserAIOper(LMsgC2SUserAIOper* msg);

	void HanderSendVideoInvitation(LMsgC2SSendVideoInvitation* msg);

	void HanderInbusyVideoInvitation(LMsgC2SReceiveVideoInvitation* msg);

	void HanderOnlineVideoInvitation(LMsgC2SOnlineVideoInvitation* msg);

	void HanderShutDownVideoInvitation(LMsgC2SShutDownVideoInvitation* msg);

	void HanderUploadGpsInformation(LMsgC2SUploadGPSInformation* msg);

	void HanderUploadVideoPermission(LMsgC2SUploadVideoPermission* msg);

	Lstring GetIp();

	void	SetIp(Lstring& ip);

	//增加房卡
	void	AddCardCount(Lint cardType, Lint count,Lint operType,Lstring admin, bool bNeedSave = true);

	static void    AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType,Lstring admin);
	//删除房卡
	void	DelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin,Lint feeTye);

	//改变玩家状态
	void	ModifyUserState(bool bLeaveServer = false);
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	inline void    updateInRoomTime() { m_timeInRoom = LTime(); }
	inline bool   IsInRoomFor15Second() { return  m_timeInRoom.Secs() > 0 && (LTime().Secs() - m_timeInRoom.Secs()) > 15 ; }
	inline void   ClearInRoomTime() { m_timeInRoom.SetSecs(0); }
	bool IsInLimitGPSPosition(User & u);	
public:
	LUser	m_userData;
	Lint	m_gateId;		//玩家在哪个gate上面
	Lint	m_userState;	//玩家当前状态
	Lint    m_videoPermission; //允许video状态
	bool	m_online;
	std::vector< LActive >	m_Active;
	Desk*	m_desk;
	Lstring	m_ip;
	LTime   m_timeInRoom;   //进入房间时间
	double   m_gps_lng;      //GPS经度
	double   m_gps_lat;      //GPS纬度
};

#endif