#include "User.h"
#include "Work.h"
#include "UserManager.h"
#include "Config.h"
#include "ActiveManager.h"
#include "DeskManager.h"
#include "RuntimeInfoMsg.h"
#include "RLogHttp.h"
#include "mjConfig.h"
#include "mhmsghead.h"

#include "clubUser.h"


#define NEWUSERCOIN 50000

//和客户端约定，取不到gps，传值 999|999
#define DEFINE_GPS_INVALID_VALUE        999.00


User::User(LUser data, Lint gateId) : m_userData(data)
{
	m_gateId = gateId;
	m_online = true;
	m_userState = LGU_STATE_CENTER;
	m_logicID = 0;
	m_Active.clear();
	m_lastCreateGameType = -1;
	//m_lastClubPlayTypeId=-1;
	m_clubStatus = USER_CLUB_INIT_STATUS;
}
	
User::~User()
{
}

void User::Tick()
{

}

//获取玩家数据库里id
Lint User::GetUserDataId()
{
	return m_userData.m_id;
}

void User::SetUserGateId(Lint gateId)
{
	m_gateId = gateId;
}

Lint User::GetUserGateId()
{
	return m_gateId;
}

bool User::GetOnline()
{
	return m_online;
}

void User::Login()
{

	//LLOG_ERROR("user [%d] login logicId[%d]",GetUserDataId(), getUserLogicID());
	LLOG_ERROR("User::Login userid[%d],userUUId[%s],userlogicid[%d],changeStatus[%d]", m_userData.m_id, m_userData.m_unioid.c_str(), getUserLogicID(), m_userData.m_customInt[7]);

	LTime now;
	Lint lastLoginTime = m_userData.m_lastLoginTime;
	m_userData.m_lastLoginTime = now.Secs();
	m_online = true;
	if(getUserLogicID() > 0)
	{
		if(gWork.isLogicServerExist(getUserLogicID()))
		{
			LMsgLMG2LLogin msg;
			msg.m_userid = m_userData.m_id;
			msg.m_gateId = GetUserGateId();
			msg.m_strUUID = m_userData.m_unioid;
			msg.m_ip = m_userData.m_customString1;
			msg.m_buyInfo = gRuntimeInfoMsg.getBuyInfo(m_userData.m_id);
			msg.m_hide = gRuntimeInfoMsg.getHide();
			msg.m_card_num = m_userData.m_numOfCard2s;
			msg.m_isMedal = (Lint)gUserManager.isMedalUserByUserId(m_userData.m_id);

			gWork.SendMessageToLogic(getUserLogicID(), msg);

			gClubUserManager.userOnLine(m_userData.m_id, true);

		}
		else
		{  
			LLOG_ERROR("login user[%d] logic[%d] not exist", GetUserDataId(), getUserLogicID());
		    setUserLogicID(0);
			gClubUserManager.setUserPlayStatus(GetUserDataId(), false);
			SendItemInfo();
			SendLoginInfo(lastLoginTime);
		}
	}
	else
	{
		// 防止金币服务器有问题 如果玩家处于金币大厅 直接修改为Center状态
		if ( m_userState == LGU_STATE_COIN && !gWork.IsCoinsServerConnected() )
		{
			m_userState = LGU_STATE_CENTER;
		}
		SendItemInfo();
		SendLoginInfo(lastLoginTime);
	}

	//玩家登陆消息发送到金币服务器
	LMsgLMG2CNUserLogin loginmsg;
	loginmsg.m_strUUID = m_userData.m_unioid;
	loginmsg.m_gateId = GetUserGateId();
	loginmsg.m_ip = getUserIPStr();
	loginmsg.m_usert = m_userData;
	gWork.SendMessageToCoinsServer( loginmsg );

	//玩家登陆消息发送到DB DB给玩家发送活动信息
	LMsgLMG2LdbUserLogin dbloginmsg;
	dbloginmsg.m_userId = m_userData.m_id;
	dbloginmsg.m_strUUID = m_userData.m_unioid;
	gWork.SendMsgToDb( dbloginmsg );

	//SendHorseInfo();
	//SendFreeInfo();
	//SendExchInfo();
	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo1 = gUserManager.getUserBaseInfo(GetUserDataId());
	if (safeBaseInfo1 && safeBaseInfo1->isValid())
	{
		LLOG_DEBUG("User::Login update before userid[%d] uuionid[%s]", GetUserDataId(), safeBaseInfo1->getResource()->m_unionId.c_str());
	}

	boost::shared_ptr<UserBaseInfo> baseInfo(new UserBaseInfo());
	baseInfo->m_id = GetUserDataId();
	baseInfo->m_nike = m_userData.m_nike;
	baseInfo->m_openId = m_userData.m_openid;
	baseInfo->m_sex = m_userData.m_sex;
	baseInfo->m_headImageUrl = m_userData.m_headImageUrl;
	baseInfo->m_unionId = m_userData.m_unioid;

	gUserManager.addUserBaseInfo(baseInfo);

	//test 
	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(GetUserDataId());
	if (safeBaseInfo && safeBaseInfo->isValid())
	{
		LLOG_DEBUG("User::Login update after userid[%d] uuionid[%s]",GetUserDataId(), safeBaseInfo->getResource()->m_unionId.c_str());
	}

	// 远程日志
	/*
	RLOG("login", getUserIPStr()
		<< "|" << LTimeTool::GetLocalTimeInString()
		<< "|" << m_userData.m_id
		<< "|" << m_userData.m_numOfCard2s
		<< "|" << m_userData.m_nike
		<< "|" << gConfig.GetServerID()
		<< "|" << gConfig.GetInsideIp());
		*/

	//Save();
}

void User::Logout()
{
	m_online = false;

	// 远程日志
	/*
	RLOG("logout", getUserIPStr()
		<< "|" << LTimeTool::GetLocalTimeInString()
		<< "|" << m_userData.m_id
		<< "|" << m_userData.m_numOfCard2s
		<< "|" << m_userData.m_nike
		<< "|" << gConfig.GetServerID()
		<< "|" << gConfig.GetInsideIp()); */
}

void User::Send(LMsg& msg)
{
	LMsgL2GUserMsg send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_dataBuff = msg.GetSendBuff();

	gWork.SendMessageToGate(m_gateId, send);
}

void User::Send(const LBuffPtr& buff)
{
	LMsgL2GUserMsg send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_dataBuff = buff;

	gWork.SendMessageToGate(m_gateId, send);
}

void User::SendLoginInfo(Lint lastLoginTime)
{

	LMsgS2CLogin send;
	send.m_id = m_userData.m_id;
	send.m_errorCode = 0;
	if (m_userData.m_customInt[3]) {
		send.m_errorCode = 9;
	}
	send.m_nike = m_userData.m_nike;
	send.m_exp = 0;
	send.m_sex = m_userData.m_sex;
	send.m_new = m_userData.m_new;
	send.m_state = getUserLogicID()> 0 ? 1 : 0;
	send.m_ip = m_ip;
	send.m_face = m_userData.m_headImageUrl;
	send.m_buyInfo = gRuntimeInfoMsg.getBuyInfo(m_userData.m_id);
	send.m_hide = gRuntimeInfoMsg.getHide();
	send.m_gm = m_userData.m_gm;
	if(m_userData.m_new == 0)
	{
		Lstring strNewUser = "newUserCoin";
		Lint newUserCoin = 0;
		gMjConfig.GetCommonConfigValue(strNewUser, newUserCoin);

		// 新手送卡
		send.m_card1 = 0;
		send.m_card2 = newUserCoin;
		send.m_card3 = 0;
		AddCardCount(CARD_TYPE_8, newUserCoin, CARDS_OPER_TYPE_INIT,"system");

		// 把新用户状态变成老用户
		m_userData.m_new = 1;
		LMsgL2CeModifyUserNew save;
		save.m_strUUID = m_userData.m_unioid;
		save.m_value = 1;
		gWork.SendToCenter(save);

	}
	else
	{
		send.m_card1 = 0;
		send.m_card2 = 0;
		send.m_card3 = 0;
		send.m_loginInterval = 0;
		if(gActiveManager.OUGCChekGiveCard(lastLoginTime, send.m_card1, send.m_loginInterval))
		{
			if(send.m_card1 > 0)
			{
				AddCardCount(CARD_TYPE_8, send.m_card1, CARDS_OPER_TYPE_ACTIVE, "system_ougc");
			}
		}
	}

	Lint startTime = gConfig.GetTime();
	if(startTime > 0 && m_userData.m_regTime >= startTime)
	{
		send.m_oldUser = 0;
	}

	//添加俱乐部信息
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(GetUserDataId());
	if (safeClubUserInfo && safeClubUserInfo->isValid())
	{
		if (safeClubUserInfo->getResource()->m_currClubInfo.m_clubId!=0)
		{
			send.m_state=LGU_STATE_CLUB;
			send.m_card2=safeClubUserInfo->getResource()->m_currClubInfo.m_clubId;
			send.m_card3=safeClubUserInfo->getResource()->m_currClubInfo.m_playTypeId;

			//safeClubUserInfo->getResource()->m_online = USER_IN_CLUB_ONLINE;

		}
		gClubUserManager.getUserInAllClubCount(safeClubUserInfo->getResource()->m_allClubId, send.m_clubOnlineUserCount);
	
	}
	else
	{
		LLOG_ERROR("Club::getUsersInDeskByClubDeskId   userid=[%d]  not find ", m_userData.m_id);
	}

	Send(send);

	gClubUserManager.userOnLine(m_userData.m_id,false);

	LLOG_ERROR("***返回用户登陆LMsgS2CLogin: msgid:%d m_id:%d m_ip:%s m_new:%d m_errorCode:%d m_state:%d m_numOfCard2s:%d unioid:%s",
		send.m_msgId, send.m_id, send.m_ip.c_str(), send.m_new, send.m_errorCode, send.m_state, m_userData.m_numOfCard2s,
		m_userData.m_unioid.c_str());
}

void User::SendItemInfo()
{
	LMsgS2CItemInfo send;
	send.m_card1 = m_userData.m_numOfCard1s;
	send.m_card2 = m_userData.m_numOfCard2s;
	send.m_card3 = m_userData.m_numOfCard3s;
	send.m_coins = m_userData.m_coins;
	send.m_credit = m_userData.m_creditValue;
	Send(send);
}

void User::SendHorseInfo()
{
	LMsgS2CHorseInfo send;
	send.m_str = gRuntimeInfoMsg.getHorseInfo();
	send.m_type = 0;
	Send(send);
}


void User::S2CeUserLogin()
{
	LMsgL2CeUserServerLogin msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_openID = m_userData.m_unioid;
	msg.m_unionId = m_userData.m_unioid;
	LLOG_DEBUG("Work::HanderLogicUserLogin, userID: %s, ServerID %d", msg.m_openID.c_str(), msg.m_serverID);
	gWork.SendToCenter(msg);
}

void User::S2CeUserLogout()
{
	LMsgL2CeUserServerLogout msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_openID = m_userData.m_unioid;
	LLOG_DEBUG("Work::HanderLogicUserLogot, userID: %s, ServerID %d", msg.m_openID.c_str(), msg.m_serverID);
	gWork.SendToCenter(msg);
}

void User::HanderMsg(LMsg* msg)
{
	LLOG_DEBUG("User::HanderMsg  msg->m_msgId=[%d]",msg->m_msgId);
	switch (msg->m_msgId)
	{
	//(61012)玩家请求创建房间
	case MSG_C_2_S_CREATE_ROOM:
	{
		if (msg != NULL)
		{
			LMsgC2SCreateDesk * createMsg = (LMsgC2SCreateDesk*)msg;
			if (createMsg->m_clubId == 0)
				HanderUserCreateDesk(createMsg);
			else
				gClubManager.userAddClubPlay(createMsg,this);
		}
		break;
	}
	//(61021)房主建房
	case MSG_C_2_S_CREATE_ROOM_FOR_OTHER:
		HanderUserCreateDeskForOther((LMsgC2SCreateDeskForOther*)msg);
		break;

	//(61101)客户端
	case MSG_C_2_S_DESK_LIST:
		HanderUserCreatedDeskListForOther((LMsgC2SDeskList*)msg);
		break;

	//(61103)房主求解散房间
	case MSG_C_2_S_CREATER_RESET_ROOM:
		HanderUserDismissCreatedDeskForOther((LMsgC2SCreaterResetRoom*)msg);
		break;

	//(61152)俱乐部会长申请解散房间
	case MSG_C_2_S_CLUB_MASTER_RESET_ROOM:
		HanderUserDismissClubDeskWithMaster((LMsgC2SClubMasterResetRoom*)msg);
		break;

	//(62063)客户端请求加入房间前，前往center检测
	case MSG_C_2_S_JOIN_ROOM_CHECK:
		HanderUserJoinRoomCheck((LMsgC2SJoinRoomCheck*)msg);
		break;

	//(61014)客户端请求加入房间
	case MSG_C_2_S_ADD_ROOM:
		HanderUserAddDesk((LMsgC2SAddDesk*)msg);
		break;
	case MSG_C_2_S_ENTER_COINS_ROOM:
		HanderUserEnterCoinsDesk((LMsgC2SEnterCoinsDesk*)msg);
		break;
	case MSG_C_2_S_QUICK_JOIN_CLUB_DESK:
		gClubManager.userQuickJoinClubDesk((LMsgC2SQuickAddDesk*) msg, this);
		break;


	//(61051)玩家在大厅点击战绩请求所有战绩列表麻將（回放：第一步，请求玩家最近10场的战绩列表）
	case MSG_C_2_S_VIP_LOG:
		HanderUserVipLog((LMsgC2SVipLog*) msg);
		break;

	case MSG_C_2_S_REQ_CREHIS:
		HanderUserCreLog((LMsgC2SCRELog*) msg);
		break;
	case MSG_C_2_S_REQ_EXCH:
		HanderUserReqEXCH((LMsgC2SREQ_Exch*) msg);
		break;
	case MSG_C_2_S_REQ_GTU:
		HanderUserReqGTU((LMsgC2SREQ_GTU*) msg);
		break;

	//(61095)玩家请求战绩列表中某个桌子的所有场次列表（回放：第二步，查找某个桌子中的每局录像ID）
	case MSG_C_2_S_ROOM_LOG:
		HanderUserRoomLog((LMsgC2SRoomLog*)msg);
		break;
	//(61053)玩家请求具体的录像ID （回放：第三步，根据回放ID返回回放数据）
	case MSG_C_2_S_VIDEO: 
		HanderUserVideo((LMsgC2SVideo*) msg);
		break;
	//(61075)根据UserID获取指定玩家的信息
	case MSG_C_2_S_GET_USER_INFO:
		HanderGetUserInfo((LMsgC2SGetUserInfo*)msg);
		break;

	case MSG_C_2_S_BIND_INVITER:    //bind邀请人
		HanderBindInviter((LMsgC2SBindInviter*)msg);
		break;

	case MSG_C_2_S_GET_INVITING_INFO:
		HanderGetInvitingInfo((LMsgC2SGetInvitingInfo *)msg);
		break;

	case MSG_C_2_S_ACTIVITY_WRITE_PHONE:
		HanderActivityPhone((LMsgC2SActivityPhone *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_LOG:
		HanderActivityRequestLog((LMsgC2SActivityRequestLog *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN:
		HanderActivityRequestDrawOpen((LMsgC2SActivityRequestDrawOpen *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW:
		HanderActivityRequestDraw((LMsgC2SActivityRequestDraw *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_SHARE:
		HanderActivityRequestShare((LMsgC2SActivityRequestShare*)msg);
		break;

		//玩家请求查看某shareid的分享录像
	case MSG_C_2_S_SHAREVIDEO:
		HanderUserShareVideo((LMsgC2SShareVideo*) msg);
		break;
		//玩家请求分享某录像
	case MSG_C_2_S_REQSHAREVIDEO:
		HanderUserReqShareVideo((LMsgC2SReqShareVideo*) msg);
		break;
		//玩家通过精确的videoid查看分享的某录像
	case MSG_C_2_S_GETSHAREVIDEO:
		HanderUserGetShareVideo((LMsgC2SGetShareVideo*) msg);
		break;

	//俱乐部
	//(61130)玩家进入俱乐部
	case MSG_C_2_S_ENTER_CLUB:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_ENTER_CLUB ");
		gClubManager.userEntryClub((LMsgC2SEnterClub*)msg, this);
		break;

	//(61132)玩家离开俱乐部
	case MSG_C_2_S_LEAVE_CLUB:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_LEAVE_CLUB ");
		gClubManager.userLeaveClub((LMsgC2SLeaveClub*)msg,this);
		break;

	//(61134)玩家在再俱乐部中切换玩法
	case MSG_C_2_S_SWITCH_PLAY_SCENE:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_SWITCH_PLAY_SCENE ");
		//HanderUserSwitchPlayType((LMsgC2SSwitchPlayScene*)msg);
		break;

	case MSG_C_2_S_REQUEST_PERSON_INFO:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_REQUEST_PERSON_INFO ");
		//HanderUserRequestPersonInfo((LMsgC2SRequestPersonInfo*)msg);
		break;
	case MH_MSG_C_2_S_QUERY_ROOM_GPS_LIMIT:
		LLOG_ERROR("ERROR  MH_MSG_C_2_S_QUERY_ROOM_GPS_LIMIT this msgid not used in baxian !!!!");
		this->HanderUserQueryRoomGPSInfo((MHLMsgC2SQueryRoomGPSLimitInfo*)msg);
		break;
	//62003：玩家请求扑克战绩（第一步：玩家在大厅界面点战绩，列出最近的10场游戏）
	case MSG_C_2_S_POKER_ROOM_LOG:
		HanderUserPokerRoomLog((LMsgC2SQiPaiLog*)msg);
		break;

	//62222：匹配入座按钮
	case MSG_S_2_C_DOUDIZHU_JOIN_CLUB_ROOM_ANONYMOUS:
		LLOG_DEBUG("MSG_S_2_C_DOUDIZHU_JOIN_CLUB_ROOM_ANONYMOUS");
		//HanderUserJoinRoomAnonymous2CheckFee((MHLMsgC2SJoinRoomAnonymous *)msg); 
		break;
	//62005：玩家请求战绩第2步，玩家请求一场比赛中每局比赛的数据列表（斗地主、升级）
	case MSG_C_2_S_POKER_MATCH_LOG:
		LLOG_DEBUG("MSG_C_2_S_POKER_MATCH_LOG");
		HanderQiPaiUserReqDraw((LMsgC2SMatchLog*)msg);
		break;
	//62105：玩家根据回放ID查找回放具体数据（回放第3步）
	case MSG_C_2_S_POKER_REQUEST_VIDEO_ID:
		LLOG_DEBUG("MSG_C_2_S_POKER_REQUEST_VIDEO_ID");
		HanderQiPaiUserReqLog((LMsgC2SPokerVideo *)msg);
		break;
	case MSG_C_2_S_POKER_EXIT_WAIT:
		LLOG_DEBUG("MSG_C_2_S_POKER_EXIT_WAIT");
		//HanderQiPaiUserLeaveWait((LMsgC2SPokerExitWait *)msg);
		break;
	case MSG_C_2_S_CLUB_CREATE_ROOM:
		LLOG_DEBUG("MSG_C_2_S_CLUB_CREATE_ROOM ERROR MSG NO USED");
		//HanderUserCreateClubDesk((LMsgC2SClubUserCreateRoom*)msg);
		break;
	case MSG_C_2_S_REQUEST_CLUB_MEMBER:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_REQUEST_CLUB_MEMBER ");
		gClubManager.userRequestClubMember((LMsgC2SRequestClubMemberList*)msg,this);
		//HanderUserRequestClubMember((LMsgC2SRequestClubMemberList*)msg);
		break;
	case MSG_C_2_S_REQUEST_CLUB_ONLINE_COUNT:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_REQUEST_CLUB_ONLINE_COUNT ");
		HanderUserRequestClubOnLineCount((LMsgC2SRequestClubOnLineCount*)msg);
		break;
	case MSG_C_2_S_REQUEST_CLUB_MEMBER_ONLINE_NOGAME:
	{
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_REQUEST_CLUB_MEMBER_ONLINE_NOGAME ");
		//Lint startTime = time(NULL);
		HanderUserRequestClubMemberOnlineNoGame((LMsgC2SRequestClubMemberOnlineNogame*)msg);
		//Lint endTime = time(NULL);
		//LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_REQUEST_CLUB_MEMBER_ONLINE_NOGAME start[%d] end[%d] =[%d]  ",startTime,endTime,endTime-startTime);
		break;
	}
	case MSG_C_2_S_INVITE_CLUB_MENBER:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_INVITE_CLUB_MENBER ");
		HanderUserInviteClubMember((LMsgC2SInviteClubMember*)msg);
		break;
	case MSG_C_2_S_REPLY_CLUB_MEMBER_INVITE:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_REPLY_CLUB_MEMBER_INVITE ");
		HanderUserReplyClubMemberInvite((LMsgC2SReplyClubMemberInvite*)msg);
		break;
	case MSG_C_2_S_CLUB_SET_ADMINISTRTOR:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_CLUB_SET_ADMINISTRTOR ");
		gClubManager.userSetClubAdministrtor((LMsgC2SSetClubAdministrtor*)msg,this);
		//HanderUserSetClubAdministrtor((LMsgC2SSetClubAdministrtor*)msg);
		break;
	case MSG_C_2_S_POWER_POINT_LIST:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_POWER_POINT_LIST ");
		gClubManager.userPointList((LMsgC2SPowerPointList*)msg, this);
		//HanderUserPointList((LMsgC2SPowerPointList*)msg);
		break;
	case MSG_C_2_S_REQUEST_POINT_RECORD:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_REQUEST_POINT_RECORD ");
		gClubManager.userRequestPointRecord((LMsgC2SRequestPointRecord*)msg ,this);
		break;
	case MSG_C_2_S_CHANGE_POINT:
		LLOG_DEBUG("User::HanderMsg  MSG_C_2_S_CHANGE_POINT ");
		gClubManager.userChangePoint((LMsgC2SChangePoint*)msg, this);
		break;
	case MSG_C_2_S_CREATE_CLUB:
		gClubManager.userCreateClub((LMsgC2SCreateClub*)msg,this);
		break;
	case MSG_C_2_S_UPDATE_CLUB:
		gClubManager.userUpdateClub((LMsgC2SUpdateClub*)msg, this);
		break;
	case MSG_C_2_S_UPDATA_PLAY_TYPE:
		gClubManager.userUpdatePlay((LMsgC2SUpdatePlay*)msg, this);
		break;
	case MSG_C_2_S_LOCK_CLUB_DESK:
		gClubManager.userLockClubDesk((LMsgC2SLockClubDesk*)msg,this);
		break;

	default:
		LLOG_ERROR("LogicManager Message Error: %d", msg->m_msgId);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//注释玩家操作

void User::HanderUserCreateDesk(LMsgC2SCreateDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("msg->m_playType.size() =[%d],msg->m_Greater2CanStart=[%d] m_GpsLimit:%d tuoguanmode:%d", msg->m_playType.size(), msg->m_Greater2CanStart, msg->m_GpsLimit, msg->m_tuoguan_mode);
	//更新用户GPS坐标
	std::ostringstream ss;
	ss << msg->m_GpsLng << "," << msg->m_GpsLat;
	m_userData.m_customString2 = ss.str();
	LLOG_DEBUG("User::HanderUserCreateDesk update m_GpsLimit[%d]  user gps: %s", msg->m_GpsLimit,m_userData.m_customString2.c_str());

	//创建房间检查gps有效
	if (msg->m_GpsLimit==1 && msg->m_GpsLng.compare("999") == 0 && msg->m_GpsLat.compare("999") == 0)
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = LMsgS2CCreateDeskRet::Err_GPS_INVALID;
		Send(ret);
		return;
	}

	LMsgL2CeUpdateCoin sendUpdateCoin;

	sendUpdateCoin.m_strUUID = m_userData.m_unioid;
	sendUpdateCoin.m_openID = m_userData.m_openid;
	sendUpdateCoin.m_serverID = gConfig.GetServerID();
	sendUpdateCoin.m_creatType = MSG_S_2_C_CREATE_ROOM;
    ///////////////////////////////////////////////////////////
	sendUpdateCoin.m_credits = msg->m_credits;

	sendUpdateCoin.m_userid = m_userData.m_id;
	sendUpdateCoin.m_gateId = GetUserGateId();
	sendUpdateCoin.m_strUUID = m_userData.m_unioid;
	sendUpdateCoin.m_ip = GetIp();
	sendUpdateCoin.m_usert = m_userData;
	sendUpdateCoin.m_flag = msg->m_flag;
	sendUpdateCoin.m_secret = msg->m_secret;
	sendUpdateCoin.m_gold = msg->m_gold;
	sendUpdateCoin.m_robotNum = msg->m_robotNum;
	sendUpdateCoin.m_playTypeCount = msg->m_playType.size();
	sendUpdateCoin.m_playType = msg->m_playType;
	sendUpdateCoin.m_cellscore = msg->m_cellscore;
	sendUpdateCoin.m_cheatAgainst = msg->m_cheatAgainst;
	sendUpdateCoin.m_feeType = msg->m_feeType;
	sendUpdateCoin.m_state = msg->m_state; 
	sendUpdateCoin.m_deskType = msg->m_deskType;
	sendUpdateCoin.m_Greater2CanStart=msg->m_Greater2CanStart;
	sendUpdateCoin.m_GpsLimit = msg->m_GpsLimit;
	sendUpdateCoin.m_allowLookOn = msg->m_allowLookOn;
	sendUpdateCoin.m_clubOwerLookOn = msg->m_clubOwerLookOn;


	//memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	memcpy(sendUpdateCoin.m_Player0CardValue, msg->m_Player0CardValue, sizeof(sendUpdateCoin.m_Player0CardValue));
	memcpy(sendUpdateCoin.m_Player1CardValue, msg->m_Player1CardValue, sizeof(sendUpdateCoin.m_Player1CardValue));
	memcpy(sendUpdateCoin.m_Player2CardValue, msg->m_Player2CardValue, sizeof(sendUpdateCoin.m_Player2CardValue));
	memcpy(sendUpdateCoin.m_Player3CardValue, msg->m_Player3CardValue, sizeof(sendUpdateCoin.m_Player3CardValue));
	memcpy(sendUpdateCoin.m_SendCardValue, msg->m_SendCardValue, sizeof(sendUpdateCoin.m_SendCardValue));
	memcpy(sendUpdateCoin.m_HaoZiCardValue, msg->m_HaoZiCardValue, sizeof(sendUpdateCoin.m_HaoZiCardValue));

	 

	gWork.SendToCenter(sendUpdateCoin);
	return;
    ///////////////////////////////////////////////////////////////////////////
	/*
	if(getUserLogicID() > 0)
	{
		LLOG_ERROR("User::HanderLMGUserCreateDesk user already in LogicServer, userid=%d", m_userData.m_id);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 2;
		Send(ret);
		return;
	}

	//房卡不足
	if( !(gRuntimeInfoMsg.isFree(-1) || gRuntimeInfoMsg.isFree(msg->m_state)) && !IfCardEnough(msg->m_flag,1))
	{
		LLOG_ERROR("User::HanderLMGUserCreateDesk Card Not Enough, userid=%d, cardnum=%d", m_userData.m_id, m_userData.m_numOfCard2s);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 1;
		Send(ret);
		return;
	}

	///是否 对信用做了限制
	Lint nCreditLimit = msg->m_credits;
	if(nCreditLimit > 0)
	{
		if(nCreditLimit > m_userData.m_creditValue)
		{
			LLOG_ERROR("User::HanderLMGUserCreateDesk Credits Not Enough, userid=%d, credits=%d", m_userData.m_id,m_userData.m_creditValue);
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CreditNotEnough;
			Send(ret);
			return;
		}
	}

	LLOG_INFO("User::HanderUserCreateDesk userid=%d flag=%d", m_userData.m_id, msg->m_flag);
	LMsgLMG2LCreateDesk send ;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_ip = GetIp();
	send.m_usert = m_userData;	
	send.m_flag = msg->m_flag;
	send.m_secret = msg->m_secret;
	send.m_gold = msg->m_gold;
	send.m_robotNum = msg->m_robotNum;
	send.m_playTypeCount = msg->m_playType.size();
	send.m_playType = msg->m_playType;
	send.m_cellscore = msg->m_cellscore;
	send.m_cheatAgainst = msg->m_cheatAgainst;
	send.m_feeType = msg->m_feeType;
	send.m_state = msg->m_state;
	send.m_deskType = msg->m_deskType;
	send.m_Greater2CanStart=msg->m_Greater2CanStart;
	//memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	memcpy(send.m_Player0CardValue, msg->m_Player0CardValue, sizeof(send.m_Player0CardValue));
	memcpy(send.m_Player1CardValue, msg->m_Player1CardValue, sizeof(send.m_Player1CardValue));
	memcpy(send.m_Player2CardValue, msg->m_Player2CardValue, sizeof(send.m_Player2CardValue));
	memcpy(send.m_Player3CardValue, msg->m_Player3CardValue, sizeof(send.m_Player3CardValue));
	memcpy(send.m_SendCardValue, msg->m_SendCardValue, sizeof(send.m_SendCardValue));
	memcpy(send.m_HaoZiCardValue, msg->m_HaoZiCardValue, sizeof(send.m_HaoZiCardValue));
	Lint iLogicServerId = getUserLogicID();
	if(!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gWork.allocateLogicServer(msg->m_state);	//获取负载最小的逻辑
	}
	else
	{
		LLOG_ERROR("User::HanderUserCreateDesk Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}
	if(iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("User::HanderLMGUserCreateDesk Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}
	send.m_deskID = gDeskManager.GetFreeDeskId(iLogicServerId,nCreditLimit);
	LLOG_DEBUG("Logwyz ........User::HanderUserCreateDesk, send.m_deskID =%d,send.m_Greater2CanStart=[%d]", send.m_deskID,send.m_Greater2CanStart);
	gWork.SendMessageToLogic(iLogicServerId, send);

	m_lastCreateGameType = msg->m_state;
	*/
}


void User::RealDoHanderUserCreateDesk(LMsgCe2LUpdateCoin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("RealDoHanderUserCreateDesk msg->m_Greater2CanStart=[%d] ", msg->m_Greater2CanStart);
	if (getUserLogicID() > 0)
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDesk user already in LogicServer, userid=%d", m_userData.m_id);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 2;
		Send(ret);
		return;
	}

	//房主建房预付费用
	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);
	Lint creatDeskCost = 0;
	if (deskList.get() != NULL && deskList->isValid())
	{
		creatDeskCost = deskList->getResource()->m_cost;
	}


	//MH房间费用检查
	deskFeeItem tempItem; 
	gMjConfig.GetFeeTypeValue(msg->m_flag, tempItem,msg->m_state);

	if (msg->m_state != 100010 && msg->m_state != 100008 && msg->m_state != 100009) {
		msg->m_Free = 0;
	}

	if ( ( msg->m_feeType == 0 && msg->m_coinNum - creatDeskCost < tempItem.m_cost && msg->m_Free == 0) || //房间类型是房主支付且房主的金币不足
		(msg->m_feeType ==1 && msg->m_coinNum - creatDeskCost < tempItem.m_even && msg->m_Free == 0))      //房卡不足
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDesk 房卡不足, userid=%d, cardnum=%d", m_userData.m_id, m_userData.m_numOfCard2s);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 1;
		Send(ret);
		return;
	}


	//房卡不足
	/*
	if (!(gRuntimeInfoMsg.isFree(-1) || gRuntimeInfoMsg.isFree(msg->m_state)) && !IfCardEnough(msg->m_flag, 1))
	{
		LLOG_ERROR("User::HanderLMGUserCreateDesk Card Not Enough, userid=%d, cardnum=%d", m_userData.m_id, m_userData.m_numOfCard2s);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 1;
		Send(ret);
		return;
	}
	*/

	if (msg->m_blocked == 1)
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDesk user already be  blocked, userid=%d", m_userData.m_id);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 3;
		Send(ret);
		return;
	}
	///是否 对信用做了限制
	Lint nCreditLimit = msg->m_credits;
	if (nCreditLimit > 0)
	{
		if (nCreditLimit > m_userData.m_creditValue)
		{
			LLOG_ERROR("User::RealDoHanderUserCreateDesk Credits Not Enough, userid=%d, credits=%d", m_userData.m_id, m_userData.m_creditValue);
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CreditNotEnough;
			Send(ret);
			return;
		}
	}

	for (int i = 0; i < msg->m_playType.size(); i++)
	{
		LLOG_DEBUG("User::RealDoHanderUserCreateDesk send.m_playType[%d]=%d", i, msg->m_playType[i]);
	}
	LLOG_DEBUG("User::RealDoHanderUserCreateDesk send.m_playType.size() =%d", msg->m_playType.size());
	LLOG_INFO("User::RealDoHanderUserCreateDesk userid=%d flag=%d", m_userData.m_id, msg->m_flag);


	//GPS信息检查
	if (msg->m_GpsLimit == 1)
	{
		double lng = 0.0f; double lat = 0.0f;
		get_gps_pair_values(m_userData.m_customString2, lng, lat);
		LLOG_DEBUG("User::RealDoHanderUserCreateDesk GPS, userid:%d gps:%s value:[%f:%f]", m_userData.m_id, m_userData.m_customString2.c_str(), lng, lat);
		LMsgS2CCreateDeskRet ret;
		//if (lng == 0.0f || lat == 0.0f) //GPS数据无效
		if (lng == DEFINE_GPS_INVALID_VALUE && lat == DEFINE_GPS_INVALID_VALUE) //GPS数据无效
		{
			LLOG_ERROR("User::RealDoHanderUserCreateDesk GPS information error, userid:%d, gps:%s [%f:%f]", m_userData.m_id, m_userData.m_customString2.c_str(), lng, lat);
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = LMsgS2CCreateDeskRet::Err_GPS_INVALID;
			Send(ret);
			return;
		}
	}
	//deskFeeItem tempItem;
	//gMjConfig.GetFeeTypeValue(msg->m_flag, tempItem);

	LMsgLMG2LCreateDesk send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_ip = msg->m_userIp;
	send.m_usert = m_userData;
	send.m_flag = msg->m_flag;
	send.m_secret = msg->m_secret;
	send.m_gold = msg->m_gold;
	send.m_robotNum = msg->m_robotNum;
	send.m_playTypeCount = msg->m_playType.size();
	send.m_playType = msg->m_playType;
	send.m_cellscore = msg->m_cellscore;
	send.m_cheatAgainst = msg->m_cheatAgainst;
	send.m_feeType = msg->m_feeType;
	send.m_state = msg->m_state;
	send.m_deskType = msg->m_deskType;
	send.m_cost = tempItem.m_cost;
	send.m_even = tempItem.m_even;
	send.m_Greater2CanStart = msg->m_Greater2CanStart;
	send.m_free = msg->m_Free;
	send.m_startTime = msg->m_startTime;
	send.m_endTime = msg->m_endTime;
	send.m_GpsLimit = msg->m_GpsLimit;
	send.m_allowLookOn = msg->m_allowLookOn;
	send.m_clubOwerLookOn = msg->m_clubOwerLookOn;
	send.m_isMedal = (Lint)gUserManager.isMedalUserByUserId(m_userData.m_id);

	
	LLOG_DEBUG("User::RealDoHanderUserCreateDesk msg->m_flag= %d send.m_cost =%d send.m_even = %d   send.m_Greater2CanStart=[%d]", msg->m_flag, send.m_cost, send.m_even, send.m_Greater2CanStart);
	//memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	memcpy(send.m_Player0CardValue, msg->m_Player0CardValue, sizeof(send.m_Player0CardValue));
	memcpy(send.m_Player1CardValue, msg->m_Player1CardValue, sizeof(send.m_Player1CardValue));
	memcpy(send.m_Player2CardValue, msg->m_Player2CardValue, sizeof(send.m_Player2CardValue));
	memcpy(send.m_Player3CardValue, msg->m_Player3CardValue, sizeof(send.m_Player3CardValue));
	memcpy(send.m_SendCardValue, msg->m_SendCardValue, sizeof(send.m_SendCardValue));
	memcpy(send.m_HaoZiCardValue, msg->m_HaoZiCardValue, sizeof(send.m_HaoZiCardValue));
	Lint iLogicServerId = getUserLogicID();
	if (!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gWork.allocateLogicServer(msg->m_state);	//获取负载最小的逻辑
	}
	else
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDesk Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}
	if (iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("User::RealDoHanderUserCreateDesk Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}
	send.m_deskID = gDeskManager.GetFreeDeskId(iLogicServerId, nCreditLimit);
	// 保存桌子扣费信息
	
	LLOG_DEBUG("User::RealDoHanderUserCreateDesk, send.m_deskID =%d,send.m_Greater2CanStart=[%d]", send.m_deskID,send.m_Greater2CanStart);
	gWork.SendMessageToLogic(iLogicServerId, send);

	m_lastCreateGameType = msg->m_state;
}

void User::HanderUserCreateDeskForOther(LMsgC2SCreateDeskForOther* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("logwyz ...........User::HanderUserCreateDeskForOther msg->m_Greater2CanStart=[%d] msg->m_GpsLimit:%d", msg->m_Greater2CanStart, msg->m_GpsLimit);
	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);

	if (deskList.get() != NULL && deskList->isValid())
	{
		std::list<int> tempDeskList = deskList->getResource()->m_DeskList;
		if (tempDeskList.size() >= _MH_DEFALUT_MAX_DESK_COUNT_FOR_OTHER) {
			LLOG_ERROR("User::HanderUserCreateDeskForOther user already is MAX =%d", tempDeskList.size());
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = 8;
			Send(ret);
			return;
		}
	}


	if (!msg->m_deskType && getUserLogicID() > 0)
	{
		LLOG_ERROR("User::HanderUserCreateDeskForOther user already in LogicServer, userid=%d", m_userData.m_id);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 2;
		Send(ret);
		return;
	}

	LMsgL2CeUpdateCoin sendUpdateCoin;

	sendUpdateCoin.m_strUUID = m_userData.m_unioid;
	sendUpdateCoin.m_openID = m_userData.m_openid;
	sendUpdateCoin.m_serverID = gConfig.GetServerID();
	sendUpdateCoin.m_creatType = MSG_C_2_S_CREATE_ROOM_FOR_OTHER;
	///////////////////////////////////////////////////////////
	sendUpdateCoin.m_credits = msg->m_credits;

	sendUpdateCoin.m_userid = m_userData.m_id;
	sendUpdateCoin.m_gateId = GetUserGateId();
	sendUpdateCoin.m_strUUID = m_userData.m_unioid;
	sendUpdateCoin.m_ip = GetIp();
	sendUpdateCoin.m_usert = m_userData;
	sendUpdateCoin.m_flag = msg->m_flag;
	sendUpdateCoin.m_secret = msg->m_secret;
	sendUpdateCoin.m_gold = msg->m_gold;
	sendUpdateCoin.m_robotNum = msg->m_robotNum;
	sendUpdateCoin.m_playTypeCount = msg->m_playType.size();
	sendUpdateCoin.m_playType = msg->m_playType;
	sendUpdateCoin.m_cellscore = msg->m_cellscore;
	sendUpdateCoin.m_cheatAgainst = msg->m_cheatAgainst;
	sendUpdateCoin.m_feeType = msg->m_feeType;
	sendUpdateCoin.m_state = msg->m_state;
	sendUpdateCoin.m_deskType = msg->m_deskType;
	sendUpdateCoin.m_Greater2CanStart=msg->m_Greater2CanStart;
	sendUpdateCoin.m_GpsLimit = msg->m_GpsLimit;
	

	//memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	memcpy(sendUpdateCoin.m_Player0CardValue, msg->m_Player0CardValue, sizeof(sendUpdateCoin.m_Player0CardValue));
	memcpy(sendUpdateCoin.m_Player1CardValue, msg->m_Player1CardValue, sizeof(sendUpdateCoin.m_Player1CardValue));
	memcpy(sendUpdateCoin.m_Player2CardValue, msg->m_Player2CardValue, sizeof(sendUpdateCoin.m_Player2CardValue));
	memcpy(sendUpdateCoin.m_Player3CardValue, msg->m_Player3CardValue, sizeof(sendUpdateCoin.m_Player3CardValue));
	memcpy(sendUpdateCoin.m_SendCardValue, msg->m_SendCardValue, sizeof(sendUpdateCoin.m_SendCardValue));
	memcpy(sendUpdateCoin.m_HaoZiCardValue, msg->m_HaoZiCardValue, sizeof(sendUpdateCoin.m_HaoZiCardValue));

	gWork.SendToCenter(sendUpdateCoin);
	return;
	/*
	//房卡不足
	if (!(gRuntimeInfoMsg.isFree(-1) || gRuntimeInfoMsg.isFree(msg->m_state)) && !IfCardEnough(msg->m_flag, 1))
	{
		LLOG_ERROR("User::HanderUserCreateDeskForOther Card Not Enough, userid=%d, cardnum=%d", m_userData.m_id, m_userData.m_numOfCard2s);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 1;
		Send(ret);
		return;
	}

	///是否 对信用做了限制
	Lint nCreditLimit = msg->m_credits;
	if (nCreditLimit > 0)
	{
		if (nCreditLimit > m_userData.m_creditValue)
		{
			LLOG_ERROR("User::HanderUserCreateDeskForOther Credits Not Enough, userid=%d, credits=%d", m_userData.m_id, m_userData.m_creditValue);
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CreditNotEnough;
			Send(ret);
			return;
		}
	}

	LLOG_INFO("User::HanderUserCreateDeskForOther userid=%d flag=%d,msg->m_Greater2CanStart=[%d]", m_userData.m_id, msg->m_flag, msg->m_Greater2CanStart);
	LMsgLMG2LCreateDeskForOther send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_ip = GetIp();
	send.m_usert = m_userData;
	send.m_flag = msg->m_flag;
	send.m_secret = msg->m_secret;
	send.m_gold = msg->m_gold;
	send.m_state = msg->m_state;
	send.m_robotNum = msg->m_robotNum;
	send.m_playTypeCount = msg->m_playType.size();
	send.m_playType = msg->m_playType;
	send.m_cellscore = msg->m_cellscore;
	send.m_deskType = msg->m_deskType;
	send.m_feeType  = msg->m_feeType;
	send.m_cheatAgainst = msg->m_cheatAgainst;
	send.m_Greater2CanStart=msg->m_Greater2CanStart;

	//memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	memcpy(send.m_Player0CardValue, msg->m_Player0CardValue, sizeof(send.m_Player0CardValue));
	memcpy(send.m_Player1CardValue, msg->m_Player1CardValue, sizeof(send.m_Player1CardValue));
	memcpy(send.m_Player2CardValue, msg->m_Player2CardValue, sizeof(send.m_Player2CardValue));
	memcpy(send.m_Player3CardValue, msg->m_Player3CardValue, sizeof(send.m_Player3CardValue));
	memcpy(send.m_SendCardValue, msg->m_SendCardValue, sizeof(send.m_SendCardValue));
	memcpy(send.m_HaoZiCardValue, msg->m_HaoZiCardValue, sizeof(send.m_HaoZiCardValue));
	Lint iLogicServerId = getUserLogicID();
	if (!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gWork.allocateLogicServer(msg->m_state);	//获取负载最小的逻辑
	}
	else
	{
		LLOG_ERROR("User::HanderUserCreateDeskForOther Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}
	if (iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("User::HanderUserCreateDeskForOther Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}
	send.m_deskID = gDeskManager.GetFreeDeskId(iLogicServerId, nCreditLimit);
	LLOG_DEBUG("User::HanderUserCreateDeskForOther, send.m_deskID =%d", send.m_deskID);

	gWork.SendMessageToLogic(iLogicServerId, send);

	m_lastCreateGameType = msg->m_state;
	*/
}

//玩家创建房间为其他玩家
void User::RealDoHanderUserCreateDeskForOther(LMsgCe2LUpdateCoin* msg)
{
	if (msg == NULL)
	{
		return;
	}

	if (msg->m_blocked == 1)
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDeskForOther user already be  blocked, userid=%d", m_userData.m_id);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 3;
		Send(ret);
		return;
	}



	if (!msg->m_deskType && getUserLogicID() > 0)
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDeskForOther user already in LogicServer, userid=%d", m_userData.m_id);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 2;
		Send(ret);
		return;
	}

	//房主建房预付费用
	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);
	Lint creatDeskCost = 0;
	if (deskList.get() != NULL && deskList->isValid())
	{
		creatDeskCost = deskList->getResource()->m_cost;
	}

	//MH房间费用检查
	deskFeeItem tempItem;
	gMjConfig.GetFeeTypeValue(msg->m_flag, tempItem, msg->m_state);
	if (msg->m_state != 100010 && msg->m_state != 100008 && msg->m_state != 100009) {
		msg->m_Free = 0;
	}
	if (msg->m_feeType == 0 && msg->m_coinNum - creatDeskCost < tempItem.m_cost  && msg->m_Free == 0) //房间类型是房主支付且房主的金币不足 
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDeskForOther 房卡不足, userid=%d, cardnum=%d", m_userData.m_id, m_userData.m_numOfCard2s);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 1;
		Send(ret);
		return;
	}

	//房卡不足
	/*
	if (!(gRuntimeInfoMsg.isFree(-1) || gRuntimeInfoMsg.isFree(msg->m_state)) && !IfCardEnough(msg->m_flag, 1))
	{
		LLOG_ERROR("User::HanderUserCreateDeskForOther Card Not Enough, userid=%d, cardnum=%d", m_userData.m_id, m_userData.m_numOfCard2s);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 1;
		Send(ret);
		return;
	}
	*/

	///是否 对信用做了限制
	Lint nCreditLimit = msg->m_credits;
	if (nCreditLimit > 0)
	{
		if (nCreditLimit > m_userData.m_creditValue)
		{
			LLOG_ERROR("User::RealDoHanderUserCreateDeskForOther Credits Not Enough, userid=%d, credits=%d", m_userData.m_id, m_userData.m_creditValue);
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CreditNotEnough;
			Send(ret);
			return;
		}
	}

	for (int i = 0; i < msg->m_playType.size(); i++)
	{
		LLOG_DEBUG("User::RealDoHanderUserCreateDeskForOther send.m_playType[%d]=%d", i, msg->m_playType[i]);
	}

	//deskFeeItem tempItem;
	//gMjConfig.GetFeeTypeValue(msg->m_flag, tempItem);

	LLOG_INFO("User::RealDoHanderUserCreateDeskForOther userid:%d flag:%d m_GpsLimit:%d", m_userData.m_id, msg->m_flag, msg->m_GpsLimit);
	LMsgLMG2LCreateDeskForOther send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_ip = msg->m_userIp;
	send.m_usert = m_userData;
	send.m_flag = msg->m_flag;
	send.m_secret = msg->m_secret;
	send.m_gold = msg->m_gold;
	send.m_state = msg->m_state;
	send.m_robotNum = msg->m_robotNum;
	send.m_playTypeCount = msg->m_playType.size();
	send.m_playType = msg->m_playType;
	send.m_cellscore = msg->m_cellscore;
	send.m_deskType = msg->m_deskType;
	send.m_feeType = msg->m_feeType;
	send.m_cheatAgainst = msg->m_cheatAgainst;
	send.m_cost = tempItem.m_cost;
	send.m_even = tempItem.m_even;
	send.m_Greater2CanStart=msg->m_Greater2CanStart;
	send.m_free = msg->m_Free;
	send.m_startTime = msg->m_startTime;
	send.m_endTime = msg->m_endTime;
	send.m_GpsLimit = msg->m_GpsLimit;


	//memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	memcpy(send.m_Player0CardValue, msg->m_Player0CardValue, sizeof(send.m_Player0CardValue));
	memcpy(send.m_Player1CardValue, msg->m_Player1CardValue, sizeof(send.m_Player1CardValue));
	memcpy(send.m_Player2CardValue, msg->m_Player2CardValue, sizeof(send.m_Player2CardValue));
	memcpy(send.m_Player3CardValue, msg->m_Player3CardValue, sizeof(send.m_Player3CardValue));
	memcpy(send.m_SendCardValue, msg->m_SendCardValue, sizeof(send.m_SendCardValue));
	memcpy(send.m_HaoZiCardValue, msg->m_HaoZiCardValue, sizeof(send.m_HaoZiCardValue));
	Lint iLogicServerId = getUserLogicID();
	if (!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gWork.allocateLogicServer(msg->m_state);	//获取负载最小的逻辑
	}
	else
	{
		LLOG_ERROR("User::RealDoHanderUserCreateDeskForOther Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}
	if (iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("User::RealDoHanderUserCreateDeskForOther Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}
	send.m_deskID = gDeskManager.GetFreeDeskId(iLogicServerId, nCreditLimit);
	LLOG_DEBUG("User::RealDoHanderUserCreateDeskForOther, send.m_deskID =%d,send.m_Greater2CanStart=[%d],userId=[%d]", send.m_deskID,send.m_Greater2CanStart, m_userData.m_id);

	gWork.SendMessageToLogic(iLogicServerId, send);

	m_lastCreateGameType = msg->m_state;

}


struct logicDeskList
{
	Lint iLogicServerId;

	std::list<Lint>  deskList;
};


void	User::HanderUserCreatedDeskListForOther(LMsgC2SDeskList* msg)
{
	if (msg == NULL)
	{
		return;
	}

	LLOG_ERROR("User::HanderUserCreatedDeskListForOther userid=%d", m_userData.m_id);


	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);

	if (deskList.get() == NULL || !deskList->isValid())
	{
		LLOG_ERROR("Work::HanderUserCreatedDeskListForOther erro");
		return;
	}
	std::list<int> tempDeskList = deskList->getResource()->m_DeskList;
	//存储 logicID相同的所有桌子
	std::map<Lint, std::list<Lint>> mapUserDeskList; 
	auto itor = tempDeskList.begin();
	for (; itor != tempDeskList.end(); itor++) 
	{
		LLOG_DEBUG("Work::HanderUserCreatedDeskListForOther sendOut");
		Lint deskId = *itor;
		Lint iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(deskId);
		LLOG_DEBUG("Work::HanderUserCreatedDeskListForOther  tempDeskList iLogicServerId = %d ", iLogicServerId);
		if (!gWork.isLogicServerExist(iLogicServerId))
		{
			iLogicServerId = INVALID_LOGICSERVERID;
		}
		if (iLogicServerId != INVALID_LOGICSERVERID) {

			auto itServerDeskList = mapUserDeskList.find(iLogicServerId);
			if (itServerDeskList == mapUserDeskList.end())
			{
				std::list<Lint>  tempList;
				LLOG_DEBUG("Work::HanderUserCreatedDeskListForOther insert  deskId = %d", deskId);
				tempList.push_back(deskId);
				mapUserDeskList.insert(std::pair<Lint, std::list<Lint>>(iLogicServerId, tempList));
			}
			else {
				LLOG_DEBUG("Work::HanderUserCreatedDeskListForOther add  deskId = %d", deskId);
				//std::list<Lint> userDeskList = itServerDeskList->second;
				itServerDeskList->second.push_back(deskId);
			}
		}
	}



	auto itorT = mapUserDeskList.begin();
	for (; itorT != mapUserDeskList.end(); itorT++)
	{
		LMsgLMG2LCreatedDeskList send;
		send.m_userid = m_userData.m_id;
		send.m_gateId = GetUserGateId();
		send.m_strUUID = m_userData.m_unioid;
		send.m_usert = m_userData;
		Lint iLogicServerId = itorT->first;
		send.m_deskListCount = itorT->second.size();

		auto tempItor = itorT->second.begin();
		for (; tempItor != itorT->second.end(); tempItor++) {
			Lint deskId = *tempItor;
			LLOG_DEBUG("Work::HanderUserCreatedDeskListForOther sendOut  deskId = %d", deskId);
			send.m_deskList.push_back(deskId);
		}
		gWork.SendMessageToLogic(iLogicServerId, send);
		LLOG_DEBUG("Work::HanderUserCreatedDeskListForOther iLogicServerId = %d ", iLogicServerId);
	}

}



void	User::HanderUserDismissCreatedDeskForOther(LMsgC2SCreaterResetRoom* msg)
{
	if (msg == NULL)
	{
		return;
	}

	LLOG_ERROR("User::HanderUserDismissCreatedDeskForOther userid=%d", m_userData.m_id);
	LMsgLMG2LDissmissCreatedDesk send;
	send.m_userid = m_userData.m_id;
	send.m_deskId = msg->m_deskId;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_usert = m_userData;

	bool isExit = gUserManager.verifyUserDeskID(send.m_userid, send.m_deskId);

	if (!isExit)
	{
		LLOG_ERROR("Work::HanderUserDismissCreatedDeskForOther erro  deskID not exist");
		return;
	}

	Lint iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(send.m_deskId);
	if (!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = INVALID_LOGICSERVERID;
	}
	if (iLogicServerId != INVALID_LOGICSERVERID) {
		gWork.SendMessageToLogic(iLogicServerId, send);
	}
}


void    User::HanderUserDismissClubDeskWithMaster(LMsgC2SClubMasterResetRoom*msg)
{
	if (msg == NULL)return;

	//桌子信息
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo =gClubDeskManager.getClubDeskInfoByClubDeskId(msg->m_clubDeskId);
	if (safeClubDeskInfo == NULL || !safeClubDeskInfo->isValid())
	{
		LMsgS2CClubMasterResetRoom ret;
		ret.m_errorCode = 2;
		ret.m_clubDeskId = msg->m_clubDeskId;
		ret.m_clubId = msg->m_clubId;
		ret.m_playTypeId = msg->m_playTypeId;
		ret.m_showDeskId = msg->m_showDeskId;
		Send(ret);
		return;
	}
	//俱乐部信息
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub =gClubManager.getClubbyClubId(msg->m_clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LMsgS2CClubMasterResetRoom ret;
		ret.m_errorCode = 2;
		ret.m_clubDeskId = msg->m_clubDeskId;
		ret.m_clubId = msg->m_clubId;
		ret.m_playTypeId = msg->m_playTypeId;
		ret.m_showDeskId = msg->m_showDeskId;
		Send(ret);
		return;
	}

	boost::shared_ptr<ClubDeskInfo> clubDesk = safeClubDeskInfo->getResource();
	boost::shared_ptr<Club> club = safeClub->getResource();

	LLOG_ERROR("User::HanderUserDismissClubDeskWithMaster userid=%d dismiss club[%d] playId[%d],clubDeskId[%d],showDeskId[%d]", m_userData.m_id, msg->m_clubId, msg->m_playTypeId, msg->m_clubDeskId, msg->m_showDeskId);

	Lint clubOwnerId = 0;
	std::set<Lint> admin;
	club->getClubOwnerAndAdmin(&clubOwnerId, admin);
	if (this->GetUserDataId() != clubOwnerId &&admin.find(this->GetUserDataId()) == admin.end())
	{
		LMsgS2CClubMasterResetRoom ret;
		ret.m_errorCode = 3;
		ret.m_clubDeskId = msg->m_clubDeskId;
		ret.m_clubId = msg->m_clubId;
		ret.m_playTypeId = msg->m_playTypeId;
		ret.m_showDeskId = msg->m_showDeskId;
		Send(ret);
		return;
	}
	if (clubDesk->getRealDeskId() == 0)
	{
		LMsgS2CClubMasterResetRoom ret;
		ret.m_errorCode = 0;
		ret.m_clubId = msg->m_clubId;
		ret.m_playTypeId = msg->m_playTypeId;
		ret.m_clubDeskId = msg->m_clubDeskId;
		ret.m_showDeskId = msg->m_showDeskId;
		ret.m_strErrorDes = "解散房间成功";
		Send(ret);
		return;
	}
	
	LMsgLMG2LDissmissClubDesk send;
	send.m_userid = m_userData.m_id;
	send.m_clubId = msg->m_clubId;
	send.m_playTypeId = msg->m_playTypeId;
	send.m_clubDeskId = msg->m_clubDeskId;
	send.m_showDeskId = msg->m_showDeskId;
	send.m_deskId = clubDesk->getRealDeskId();
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_usert = m_userData;


	Lint iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(send.m_deskId);
	if (!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = INVALID_LOGICSERVERID;
	}
	if (iLogicServerId != INVALID_LOGICSERVERID) {
		gWork.SendMessageToLogic(iLogicServerId, send);
	}
	else
	{
		LLOG_ERROR("User::HanderUserDismissClubDeskWithMaster error!!! logic server not find INVALID_LOGICSERVERID");
	}
}


void User::HanderUserAddDesk(LMsgC2SAddDesk* msg ,Lint addDeskFlag)
{
	if (msg == NULL)
	{
		return;
	}

	LLOG_DEBUG("User::HanderUserAddDesk    deskId=[%d] addDeskFlag[%d]",msg->m_deskId,addDeskFlag);
	//保存用户上传的GPS信息
	std::ostringstream ss;
	LLOG_ERROR("User::HanderUserAddDesk update gps info -- user:%d deskId[%d] GPS-msg->m_Gps_Lng:%s msg->m_Gps_Lat:%s", m_userData.m_id, msg->m_deskId,msg->m_Gps_Lng.c_str(), msg->m_Gps_Lat.c_str());
	ss << msg->m_Gps_Lng << "," << msg->m_Gps_Lat;
	m_userData.m_customString2 = ss.str();

	//这里区分一下,如果桌子号时1,000,000-9,999,999，俱乐部桌子
	//msg->m_deskId=2000000+msg->m_deskId;  //测试用
	if (msg->m_deskId>=CLUB_DESK_ID_BEGIN_NUMBER)
	{
		//HanderUserEnterClubDesk(msg);
		gClubManager.userJoinClubDesk(msg, this, addDeskFlag);
		//HanderUserAddDeskBeforeCheckFee(msg, addDeskFlag);
		return;
	}
	if (gDeskManager.IsBelongToClub(msg->m_deskId))
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId=msg->m_deskId;
		ret.m_errorCode=2;
		Send(ret);

		LLOG_DEBUG("User::HanderUserAddDesk deskid [%d]6 is belong to club", msg->m_deskId);
		return;
	}

	//MH房间用户费用检查
	Lint limit = gDeskManager.GetDeskCreditLimit(msg->m_deskId);
	if(limit > 0)
	{
		if(m_userData.m_creditValue < limit)
		{
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskId;
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_CreditNotEnough;
			Send(ret);

			LLOG_ERROR("ERROR: User::HanderUserAddDesk credits not enough, userid=%d", m_userData.m_id);
			return;
		}
	}

	
	LMsgL2CeUpdateCoinJoinDesk centerSend;
	centerSend.m_serverID = gConfig.GetServerID();
	centerSend.m_openID = m_userData.m_openid;
	centerSend.m_userid = m_userData.m_id;
	centerSend.m_gateId = GetUserGateId();
	centerSend.m_strUUID = m_userData.m_unioid;
	centerSend.m_deskID = msg->m_deskId;
	centerSend.m_ip = GetIp();
	centerSend.m_usert = m_userData;
	centerSend.m_addDeskFlag= addDeskFlag;


	gWork.SendToCenter(centerSend);

	return;

	/*
	LLOG_INFO("User::HanderUserAddDesk userid=%d deskid=%d", m_userData.m_id, msg->m_deskId);
	LMsgLMG2LAddToDesk send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_deskID = msg->m_deskId;
	send.m_ip = GetIp();
	send.m_usert = m_userData;

	Lint iLogicServerId = getUserLogicID();
	if(!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(msg->m_deskId);
		if(!gWork.isLogicServerExist(iLogicServerId))
		{
			iLogicServerId = INVALID_LOGICSERVERID;
		}
	}
	else
	{
		LLOG_ERROR("User::HanderUserAddDesk Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}

	if(iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderUserAddDesk Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}

	gWork.SendMessageToLogic(iLogicServerId, send);
	*/
}

void User::RealDoHanderUserAddDesk(LMsgCe2LUpdateCoinJoinDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	
	Lint limit = gDeskManager.GetDeskCreditLimit(msg->m_deskID);
	LLOG_DEBUG("User::RealDoHanderUserAddDesk  limit=[%d],block=[%d],coinNum=[%d]", limit, msg->m_Block, msg->m_coinNum);
	if (limit > 0)
	{
		if (m_userData.m_creditValue < limit)
		{
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskID;
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_CreditNotEnough;
			Send(ret);

			LLOG_ERROR("ERROR: User::HanderUserAddDesk credits not enough, userid=%d", m_userData.m_id);
			return;
		}
	}

	if (msg->m_Block) 
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskID;
		ret.m_errorCode = 3;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderUserAddDesk be blocked, userid=%d", m_userData.m_id);
		return;
	}



	//房主建房预付费用
	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);
	Lint creatDeskCost = 0;
	if (deskList.get() != NULL && deskList->isValid())
	{
		creatDeskCost = deskList->getResource()->m_cost;
	}

	LMsgLMG2LAddToDesk send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_deskID = msg->m_deskID;
	send.m_ip = msg->m_userIp;
	send.m_usert = m_userData;
	send.m_creatDeskCost = creatDeskCost;
	send.m_Free = msg->m_Free;
	send.m_startTime = msg->m_startTime;
	send.m_endTime = msg->m_endTime;
	send.m_addDeskFlag = msg->m_addDeskFlag;
	send.m_isMedal = gUserManager.isMedalUserByUserId(m_userData.m_id);

	Lint iLogicServerId = getUserLogicID();
	if (!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(msg->m_deskID);
		if (!gWork.isLogicServerExist(iLogicServerId))
		{
			iLogicServerId = INVALID_LOGICSERVERID;
		}
	}
	else
	{
		LLOG_ERROR("User::HanderUserAddDesk Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}

	if (iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskID;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderUserAddDesk Logic Not Exist, userid=%d, deskId=[%d]", m_userData.m_id, msg->m_deskID);
		return;
	}
	LLOG_ERROR("*********User::HanderUserAddDesk  userid=%d gps:%s, m_addDeskFlag[%d] ", m_userData.m_id, m_userData.m_customString2.c_str(), send.m_addDeskFlag);
	gWork.SendMessageToLogic(iLogicServerId, send);
}

void User::HanderUserEnterCoinsDesk(LMsgC2SEnterCoinsDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	if( !gWork.IsCoinsServerConnected() )
	{
		LMsgS2CEnterCoinsDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("User::HanderUserEnterCoinsDesk CoinsServer not Connected, userid=%d", m_userData.m_id);
		return;
	}

	if(getUserLogicID() > 0)
	{
		LMsgS2CEnterCoinsDeskRet ret;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_ERROR("User::HanderUserEnterCoinsDesk user already in LogicServer, userid=%d", m_userData.m_id);
		return;
	}

	LLOG_INFO("User::HanderUserEnterCoinsDesk userid=%d state=%d", m_userData.m_id, msg->m_state);
	LMsgLMG2CNEnterCoinDesk send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_gateId = GetUserGateId();
	send.m_ip = GetIp();
	send.m_usert = m_userData;	
	send.m_state = msg->m_state;
	send.m_robotNum = msg->m_robotNum;
	send.m_playType = msg->m_playType;
	send.m_coins = msg->m_coins;
	memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	gWork.SendMessageToCoinsServer( send );
}

//(61051)玩家在大厅点击战绩请求所有战绩列表（回放：第一步，请求玩家最近10场的战绩列表）
void User::HanderUserVipLog(LMsgC2SVipLog* msg)
{
	LLOG_DEBUG("User::HanderUserVipLog userid=%d, gm=%d clubId[%d] limist[%d] getUserId[%d] searchId[%d] time[%d]",msg->m_userId,m_userData.m_gm,msg->m_clubId,msg->m_limit,GetUserDataId(),msg->m_searchId,msg->m_time);
	LMsgL2LBDReqVipLog log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId = GetUserDataId();
	log.m_time = msg->m_time;
	log.m_clubId = msg->m_clubId;
	log.m_limit = msg->m_limit;
	if (m_userData.m_gm && msg->m_userId != 0)
	{
		log.m_reqUserId = msg->m_userId;
		LLOG_ERROR("User::HanderUserVipLog requserid=%d, userid=%d", log.m_reqUserId, log.m_userId);
	}
	else
	{
		log.m_reqUserId = GetUserDataId();
	}
	if (msg->m_searchId != 0)
	{
		log.m_reqUserId = msg->m_searchId;
	}
	if (msg->m_clubId != 0 && msg->m_searchId==0)
	{
		boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_clubId);
		if (safeClub.get() != NULL && safeClub->isValid())
		{
			boost::shared_ptr<Club> club = safeClub->getResource();
			Lint owerId = 0;
			std::set<Lint> adminUsers;
			club->getClubOwnerAndAdmin(&owerId, adminUsers);

			if (GetUserDataId() == owerId || adminUsers.find(GetUserDataId()) != adminUsers.end())
			{
				LLOG_DEBUG("User::HanderUserVipLog userId[%d] all_club=1",GetUserDataId());
				log.m_allClub = 1;
			}

		}
	}
	gWork.SendMsgToDb(log);
}

void User::HanderUserCreLog( LMsgC2SCRELog* msg )
{
	LLOG_DEBUG("User::HanderUserCreLog userid=%d",msg->m_userId);
	LMsgL2LBDReqCRELog log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId = GetUserDataId();
	log.m_time = msg->m_time;
	
	gWork.SendMsgToDb(log);
}

void User::HanderUserReqGTU( LMsgC2SREQ_GTU* msg )
{
	LLOG_DEBUG("User::HanderUserReqGTU userid=%d req dian zan zan zan....",msg->m_userId);

	User::AddCreditForUserlist(msg->m_userList);

	LMsgL2LDBDEL_GTU log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId = msg->m_userId;
	log.m_strLog = msg->m_onelog;
	gWork.SendMsgToDb(log);

	//返回客户端
	LMsgS2CREQ_GTU_RET ret;
	ret.m_index = msg->m_index;
	this->Send(ret);
}

void User::HanderUserReqEXCH( LMsgC2SREQ_Exch* msg )
{
	LLOG_DEBUG("User::HanderUserReqEXCH card=%d req exc exc....",msg->m_card);

	LMsgS2CREQ_EXCH_RET ret;
	ret.m_card = -1;  //活动未开启

	if(gActiveManager.CheckTimeValid_Exch())
	{
		bool suc = DealExchangeCard(msg->m_card);
		if(suc)
		{
			ret.m_card = msg->m_card;
		}
		else
		{
			ret.m_card = -2;   //金币不够
		}
	}
	this->Send(ret);
}

void User::HanderUserRoomLog(LMsgC2SRoomLog* msg)
{
 	LLOG_DEBUG("User::HanderUserVipLog userid=%d, deskid=%d, time=%d", m_userData.m_id, msg->m_deskId, msg->m_time);	
	LMsgL2LDBReqRoomLog log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId  = GetUserDataId();
	log.m_time	  = msg->m_time;
	log.m_deskId  = msg->m_deskId;
	log.m_pos	  = msg->m_pos;
 	gWork.SendMsgToDb(log);
}

static bool SortFun(LActive c1,LActive c2)
{
	if(c1.m_LasJoinDate > c2.m_LasJoinDate)
	{
		return true;
	}

	return false;
}

Lstring User::GetIp()
{
	return m_userData.m_customString1;
}

void User::SetIp(Lstring& ip)
{
	m_ip = ip;
}

bool User::IfCardEnough(Lint cardType, Lint num)
{
	return m_userData.m_numOfCard2s >= GetCardDelCount(cardType, num);
}

void User::AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave)
{
	LLOG_INFO("User::AddCardCount userid=%d, curcount=%d, type=%d, count=%d, operType=%d", m_userData.m_id, m_userData.m_numOfCard2s, cardType, count, operType);
	switch (cardType)
	{
	case CARD_TYPE_4:
		m_userData.m_numOfCard1s += count;
		m_userData.m_totalbuynum += count / 2;
		break;
	case CARD_TYPE_8:
		m_userData.m_numOfCard2s += count;
		m_userData.m_totalbuynum += count;
		break;
	case CARD_TYPE_16:
		m_userData.m_numOfCard3s += count;
		m_userData.m_totalbuynum += count * 2;
		break;
	case CARD_TYPE_Other:
		m_userData.m_numOfCard2s += count;
		m_userData.m_totalbuynum += count;
		break;
	default:
		break;
	}

	if (bNeedSave)
	{
		// 透传给Center
		LMsgL2LMGModifyCard msg;
		msg.admin = admin;
		msg.cardType = cardType;
		msg.cardNum = count;
		msg.isAddCard = 1;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		gWork.SendToCenter(msg);
	}
	SendItemInfo();
}

// void User::AddCardCount( Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType, const Lstring& admin )
// {
// 	// 透传给Center
// 	LMsgL2LMGModifyCard msg;
// 	msg.admin = admin;
// 	msg.cardType = cardType;
// 	msg.cardNum = count;
// 	msg.isAddCard = 1;
// 	msg.operType = operType;
// 	msg.m_userid = id;
// 	msg.m_strUUID = strUUID;
// 	gWork.SendToCenter(msg);
// }

void User::AddCreditForUserlist(const std::vector<Lint> &userList)
{
	LMsgLM2CEN_ADD_CRE msg;
	memset(msg.m_user,0,sizeof(msg.m_user));

	for(size_t i = 0; i < userList.size() && i < 4; ++i)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userList[i]);
		if(safeUser.get() == NULL || !safeUser->isValid())
		{
			continue;
		}

		boost::shared_ptr<User> user = safeUser->getResource();
		if(user->m_userData.m_id == m_userData.m_id)
		{
			continue;   //不能给自己点赞
		}

		msg.m_user[i] = userList[i];

		if (user->GetOnline() )
		{
			user->m_userData.m_creditValue++;
		}
	}

	gWork.SendToCenter(msg);
}

bool User::DealExchangeCard(Lint inCard)
{
	std::map<Lint,Lint>& rule = gActiveManager.GetExchRule();
	auto it = rule.find(inCard);
	if(it != rule.end() && rule[inCard] <= m_userData.m_coins)
	{
		Lint delCoin = rule[inCard];

		m_userData.m_coins -= delCoin;
		m_userData.m_numOfCard2s += inCard;

		LMsgL2LMGExchCard exch;
		exch.m_add = inCard;
		exch.m_del = delCoin;
		exch.m_strUUID = m_userData.m_unioid;
		exch.m_userid = m_userData.m_id;
		exch.admin = "system";
		exch.cardType = CARDS_OPER_TYPE_EXCHANGE;
		exch.operType = CARD_TYPE_Other;

		gWork.SendToCenter(exch);

		SendItemInfo();

		return true;
	}
	return false;
}

void User::DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave)
{
	LLOG_INFO("User::DelCardCount userid=%d, curcount=%d, type=%d, count=%d, operType=%d", m_userData.m_id, m_userData.m_numOfCard2s, cardType, count, operType);
	//m_userData.m_numOfCard2s = ::DelCardCount(m_userData.m_numOfCard2s, cardType, count);
	if (m_userData.m_numOfCard2s - count >= 0) {
		m_userData.m_numOfCard2s = m_userData.m_numOfCard2s - count;
	}
	else {
		m_userData.m_numOfCard2s = 0;
	}

	if ( bNeedSave )
	{
		// 透传给Center
		LMsgL2LMGModifyCard msg;
		msg.admin = admin;
		msg.cardType = cardType;
		msg.cardNum = count;
		msg.isAddCard = 0;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		LLOG_DEBUG("User::DelCardCount   count= %d, msg.m_msgId =%d", msg.m_msgId, operType);
		gWork.SendToCenter(msg);
	}

	SendItemInfo();
}

void User::AddCoinsCount(Lint count, Lint operType, bool bNeedSave)
{
	m_userData.m_coins += count;
	if ( bNeedSave )
	{
		// 透传给Center
		LMsgCN2LMGModifyUserCoins msg;
		msg.coinsNum = count;
		msg.isAddCoins = 1;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		gWork.SendToCenter(msg);
	}
	SendItemInfo();
}

void User::DelCoinsCount(Lint count, Lint operType, bool bNeedSave)
{
	if ( m_userData.m_coins < count )
	{
		m_userData.m_coins = 0;
	}
	else
	{
		m_userData.m_coins -= count;
	}
	if ( bNeedSave )
	{
		// 透传给Center
		LMsgCN2LMGModifyUserCoins msg;
		msg.coinsNum = count;
		msg.isAddCoins = 0;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		gWork.SendToCenter(msg);
	}
	SendItemInfo();
}

void User::AddPlayCount()
{
	LLOG_INFO("User::AddPlayCount userid=%d", m_userData.m_id);
	m_userData.m_totalplaynum++;

	// 透传给Center
	LMsgL2LMGAddUserPlayCount msg;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID= m_userData.m_unioid;
	gWork.SendToCenter(msg);
}

void User::HanderGetUserInfo( LMsgC2SGetUserInfo* msg )
{
	LMsgS2CUserInfo ret;
	ret.m_error = ErrorCode::UserNotExist;

	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_userId);
	if(safeBaseInfo && safeBaseInfo->isValid())
	{
		ret.m_user.m_id			= safeBaseInfo->getResource()->m_id;
		ret.m_user.m_unionId	= safeBaseInfo->getResource()->m_unionId;
		ret.m_user.m_nickName	= safeBaseInfo->getResource()->m_nike;
		ret.m_user.m_headImgUrl = safeBaseInfo->getResource()->m_headImageUrl;
		ret.m_error = ErrorCode::ErrorNone;
	}
	Send(ret);
}

void User::HanderGetInvitingInfo( LMsgC2SGetInvitingInfo *msg )
{
	LMsgC2SGetInvitingInfo log;
	log.m_userId = m_userData.m_id;
	log.m_strUUID= m_userData.m_unioid;

	gWork.SendMsgToDb(log);
}

void User::HanderActivityPhone(LMsgC2SActivityPhone *msg)
{
	LMsgC2SActivityPhone send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg)
{
	LMsgC2SActivityRequestLog send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg)
{
	LMsgC2SActivityRequestDrawOpen send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg)
{
	LMsgC2SActivityRequestDraw send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	send.m_cardNum = m_userData.m_numOfCard2s;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestShare(LMsgC2SActivityRequestShare *msg)
{
	LMsgC2SActivityRequestShare send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderBindInviter( LMsgC2SBindInviter* msg )
{
	ErrorCode code = ErrorCode::ErrorNone;

	if(msg->m_inviterId != m_userData.m_id)
	{
		boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_inviterId);
		if(safeBaseInfo && safeBaseInfo->isValid())
		{
			LLOG_INFO("User: HanderBindInviter bind inviterid = %d  msg!!!",msg->m_inviterId);

			LMsgLM_2_LDBBindInviter msg2db;
			msg2db.m_inviterId = msg->m_inviterId;
			msg2db.m_userId = m_userData.m_id;
			gWork.SendMsgToDb(msg2db);
		}
		else
		{
			LLOG_ERROR("User: bind error %d not exist!!!",msg->m_inviterId);
			code = ErrorCode::UserNotExist;
		}
	}
	else
	{   //绑定 
		code = ErrorCode::BindingSelf;
	}

	LMsgS2CBindInviter ret;
	ret.m_error = code;
	if(ret.m_error == ErrorCode::ErrorNone)
	{
		boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_inviterId);
		if(safeBaseInfo && safeBaseInfo->isValid())
		{
			ret.m_inviter.m_headImgUrl	= safeBaseInfo->getResource()->m_headImageUrl;
			ret.m_inviter.m_nickName	= safeBaseInfo->getResource()->m_nike;
			ret.m_inviter.m_unionId		= safeBaseInfo->getResource()->m_unionId;
			ret.m_inviter.m_id			= msg->m_inviterId;
		}
	}

	Send(ret);
}

void User::HanderUserVideo(LMsgC2SVideo* msg)
{
	LMsgL2LDBReqVideo video;
	video.m_strUUID = m_userData.m_unioid;
	video.m_userId = GetUserDataId();
	video.m_videoId = msg->m_videoId;

	gWork.SendMsgToDb(video);
}

void User::HanderUserShareVideo(LMsgC2SShareVideo* msg)
{
	LMsgL2LDBGetShareVideo get_video;
	if(msg)
	{
		get_video.m_strUUID = m_userData.m_unioid;
		get_video.m_shareId = msg->m_shareID;
		get_video.m_userId = m_userData.m_id;
		gWork.SendMsgToDb(get_video);
	}
}

//询问是否某videoid的录像
void User::HanderUserReqShareVideo(LMsgC2SReqShareVideo* msg)
{
	if(msg)
	{
		LLOG_INFO("User search for share id,video id:%s",msg->m_videoid.c_str());
		LMsgL2LDBGGetShareVideoId get_share_id;
		get_share_id.m_strUUID = m_userData.m_unioid;
		get_share_id.m_userId = GetUserDataId();
		get_share_id.m_videoId = msg->m_videoid;
		gWork.SendMsgToDb(get_share_id);
	}
}
void User::HanderUserGetShareVideo(LMsgC2SGetShareVideo* msg)
{
	/*
	if(msg)
	{
		LMsgL2LDBGetSsharedVideo video;
		video.m_strUUID = m_userData.m_unioid;
		video.m_userId = GetUserDataId();
		video.m_videoId = msg->m_videoId;
		MHLOG("****user query share video log with video id UUID:%s, userid:%d, videid:%s", video.m_strUUID.c_str(), video.m_userId, video.m_videoId.c_str());
		gWork.SendMsgToDb(video);
	}
	*/

	if (msg)
	{
		MHLMsgL2LDBQueryShareVideoByShareId video;
		video.m_strUUID = m_userData.m_unioid;
		video.m_userId = GetUserDataId();
		video.m_videoId = msg->m_videoId;
		MHLOG("****Send user query share video to LogicDB. UUID:%s, userid:%d, videoid:%s", video.m_strUUID.c_str(), video.m_userId, video.m_videoId.c_str());
		gWork.SendMsgToDb(video);
	}
}

void User::SendFreeInfo()
{
	Lstring str = gRuntimeInfoMsg.getFreeTime();
	
	LMsgS2CHorseInfo send;
	send.m_str = str;
	send.m_type = 1;
	Send(send);
}

void User::SendExchInfo()
{
	if(gActiveManager.CheckHasExch())   //有活动推送活动
	{
		Lstring str = gActiveManager.GetExchRuleInfo();

		LMsgS2CHorseInfo send;
		send.m_str = str;
		send.m_type = LMsgS2CHorseInfo::TInfo_Exch;
		Send(send);
	}
}

/////////////////////////////////////////////////////////
//俱乐部
//客户端用户加入房间
//void User::HanderUserEnterClubDesk(LMsgCe2LUpdateCoinJoinDesk* msg)
//{
//	LLOG_DEBUG("User::HanderUserEnterClubDesk");
//	if (msg == NULL)
//	{
//		return;
//	}
//
//	LLOG_DEBUG("User::HanderUserEnterClubDesk  m_userid=[%d], m_coinNum=[%d] m_Block=[%d],m_clubMasterCoinNum=[%d],m_clubMasterBlock=[%d]", msg-//>m_userid, msg->m_coinNum, msg->m_Block, msg->m_clubMasterCoinNum, msg->m_clubMasterBlock);
//	if (msg->m_errorCode == 1)
//	{
//		//俱乐部中没有用户信息
//		LMsgS2CAddDeskRet ret;
//		ret.m_deskId = msg->m_deskID;
//		ret.m_errorCode = 3;
//		Send(ret);
//		LLOG_ERROR("ERROR: User::HanderUserEnterClubDesk center return  m_errorCode=1 not find userinfo, userid=%d", m_userData.m_id);
//		return;
//	}
//
//	Lint limit = gDeskManager.GetDeskCreditLimit(msg->m_deskID);
//	if (limit > 0)
//	{
//		if (m_userData.m_creditValue < limit)
//		{
//			LMsgS2CAddDeskRet ret;
//			ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = LMsgS2CAddDeskRet::Err_CreditNotEnough;
//			Send(ret);
//
//			LLOG_ERROR("ERROR: User::HanderUserEnterClubDesk credits not enough, userid=%d", m_userData.m_id);
//			return;
//		}
//	}
//
//	if (msg->m_Block)
//	{
//		LMsgS2CAddDeskRet ret;
//		ret.m_deskId = msg->m_deskID;
//		ret.m_errorCode = 3;
//		Send(ret);
//
//		LLOG_ERROR("ERROR: User::HanderUserEnterClubDesk be blocked, userid=%d", m_userData.m_id);
//		return;
//	}
//
//	//用户的房主建房预付费用
//	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);
//	Lint creatDeskCost = 0;
//	if (deskList.get() != NULL && deskList->isValid())
//	{
//		creatDeskCost = deskList->getResource()->m_cost;
//	}
//
//	//根据桌号获取桌子所在俱乐部信息
//	ClubInfos  tempClubInfos;
//	if (!gDeskManager.GetClubInfoByClubDeskId(msg->m_deskID, tempClubInfos))
//	{
//
//		LLOG_ERROR("User::HanderUserEnterClubDesk  clubdeskId=[%d] not find clubInfo userId[%d]", msg->m_deskID, this->GetUserDataId());
//
//		LMsgS2CAddDeskRet ret;
//		ret.m_deskId = msg->m_deskID;
//		ret.m_errorCode = 2;
//		Send(ret);
//		return;
//	}
//
//	//取得俱乐部的付费信息
//	clubFeeType  tempClubFeeType;
//	Lint ret = gClubManager.getClubFeeType(tempClubInfos.m_clubId, tempClubFeeType, tempClubInfos.m_playTypeId); //0:俱乐部会长付费 1:玩家分摊
//	if (ret < 0)
//	{
//		LLOG_ERROR("User::HanderUserEnterClubDesk   club[%d]  ret [%d] < 0,userId[%d]", tempClubInfos.m_clubId, ret, this->GetUserDataId());
//		return;
//	}
//
//	LLOG_DEBUG("User::HanderUserEnterClubDesk club  feetype=[%d]", tempClubFeeType.m_clubFeeType);
//
//	//获取加入房间费用
//	deskFeeItem tempItem;
//	gMjConfig.GetFeeTypeValue(tempClubFeeType.m_playTypeCircle, tempItem, tempClubFeeType.m_state);
//
//	//检查费用
//	if (0 == tempClubFeeType.m_clubFeeType)  //检查会长的 ,这里不做处理,扣费的地方处理
//	{
//		if (msg->m_clubMasterCoinNum <= 0)   //会长钻石券不足
//		{
//
//			LLOG_ERROR("User::HanderUserEnterClubDesk club  creator [%d] <= [%d] coins not enough  joinUserId=[%d]", msg->m_clubMasterCoinNum, 0, msg->m_userID);
//			LMsgS2CAddDeskRet ret;
//			ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = 7;
//			Send(ret);
//			return;
//		}
//	}
//	else //均摊
//	{
//		if (msg->m_coinNum - creatDeskCost < tempItem.m_even)
//		{
//			LLOG_ERROR("User::HanderUserEnterClubDesk club  userId [%d] coins not enough  ", m_userData.m_id);
//			LMsgS2CAddDeskRet ret;
//			ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = 4;
//			Send(ret);
//			return;
//		}
//	}
//
//	//检查用户是否有logicid
//	Lint iLogicServerId = getUserLogicID();
//	if (gWork.isLogicServerExist(iLogicServerId) && iLogicServerId != INVALID_LOGICSERVERID)
//	{
//		LMsgLMG2LAddToDesk send;
//		send.m_userid = m_userData.m_id;
//		send.m_gateId = GetUserGateId();
//		send.m_strUUID = m_userData.m_unioid;
//		send.m_deskID = msg->m_deskID;
//		send.m_ip = msg->m_userIp;
//		send.m_usert = m_userData;
//		send.m_creatDeskCost = creatDeskCost;
//		send.m_Free = msg->m_Free;
//		send.m_startTime = msg->m_startTime;
//		send.m_endTime = msg->m_endTime;
//		send.m_addDeskFlag = msg->m_addDeskFlag;
//		gWork.SendMessageToLogic(iLogicServerId, send);
//		return;
//	}
//
//	UserAddToClubDeskInfo send;
//	send.m_userid = m_userData.m_id;
//	send.m_gateId = GetUserGateId();
//	send.m_strUUID = m_userData.m_unioid;
//	send.m_deskID = msg->m_deskID;
//	send.m_Free = msg->m_Free;
//	send.m_ip = GetIp();
//	send.m_usert = m_userData;
//	send.m_creatDeskCost = creatDeskCost;
//	send.m_cost = tempItem.m_cost;
//	send.m_even = tempItem.m_even;
//
//	send.m_addDeskFlag = msg->m_addDeskFlag;
//
//	LLOG_DEBUG("User::HanderUserEnterClubDesk  deskId=[%d] , clubId=[%d],playTypeId=[%d]", msg->m_deskID, tempClubInfos.m_clubId, tempClubInfos.m_playTypeId);
//	gClubManager.userEnterClubDesk(tempClubInfos.m_clubId, tempClubInfos.m_playTypeId, send.m_deskID, send, this);
//	return;
//}

//logic 返回的确认用户加入房间
//void User::HanderUserEnterClub(LMsgC2SEnterClub*msg)
//{
//	//LLOG_DEBUG("User::HanderUserEnterClub");
//	if (msg==NULL)
//	{
//		return;
//	}
//	LLOG_DEBUG("User::HanderUserEnterClub  clubId=[%d]", msg->m_clubId);
//	gClubManager.userEntryClub(msg->m_clubId, this);
//}

//logic 返回的确认用户离开房间
//void User::HanderUserLeaveClub(LMsgC2SLeaveClub*msg)
//{
//	LLOG_DEBUG("User::HanderUserLeaveClub ");
//	if (msg==NULL)
//	{
//		return;
//	}
//	LLOG_DEBUG("User::HanderUserLeaveClub  clubId=[%d]", msg->m_clubId);
//
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub =gClubManager.getClubbyClubId(msg->m_clubId);
//	if (safeClub.get() == NULL || !safeClub->isValid())
//	{
//		LLOG_ERROR("Fail to user[%d] LeaveClub,clubId=[%d]  is null or invalid", GetUserDataId(), msg->m_clubId);
//		//未找到俱乐部
//		LMsgS2CLeaveClub  send;
//		send.m_errorCode = 1;
//		this->Send(send.GetSendBuff());
//		return ;
//	}
//
//	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(GetUserDataId());
//	if (safeUser == NULL || !safeUser->isValid())
//	{
//		//未找到俱乐部用户信息
//		LMsgS2CLeaveClub  send;
//		send.m_errorCode = 0;
//		this->Send(send.GetSendBuff());
//		return ;
//
//	}
//
//	Lint userCurrentPlayTypeId = 0;
//	boost::shared_ptr<Club> club = safeClub->getResource();
//	boost::shared_ptr<ClubUser> clubUser = safeUser->getResource();
//
//	if (safeUser->getResource()->m_currClubInfo.m_clubId == msg->m_clubId)
//	{
//		userCurrentPlayTypeId = safeUser->getResource()->m_currClubInfo.m_playTypeId;
//	}
//
//	if (  true)//club->userLeaveClub(msg->m_playTypeId, this, userCurrentPlayTypeId))
//	{
//		//更新用户信息
//		clubUser->m_currClubInfo.reset();
//		//clubUser->m_lastLeaveTime = time(NULL);
//
//		//用户离开俱乐部
//		//this->setUserState(LGU_STATE_CENTER);
//
//		LMsgS2CLeaveClub  send;
//		send.m_clubId = msg->m_clubId;
//		send.m_PlayTypeId = msg->m_playTypeId;
//		send.m_errorCode = 0;
//		gClubUserManager.getUserInAllClubCount(clubUser->m_allClubId, send.m_clubOnlineUserCount);
//
//		this->Send(send.GetSendBuff());
//
//		gClubUserManager.userLogoutClub(msg->m_clubId, this->GetUserDataId());
//
//	}
//
//}

//void User::HanderUserSwitchPlayType(LMsgC2SSwitchPlayScene*msg)
//{
//	LLOG_DEBUG("User::HanderUserSwitchPlayType");
//	if (msg==NULL)
//	{
//		return;
//	}
//	LLOG_DEBUG("User::HanderUserSwitchPlayType  clubId=[%d]", msg->m_clubId);
//
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_clubId);
//	if (safeClub.get() == NULL || !safeClub->isValid())
//	{
//		LMsgS2CSwitchClubScene send;
//		send.m_errorCode = 3;
//		this->Send(send.GetSendBuff());
//		return ;
//	}
//	Lint userCurrentPlayTypeId = 0;
//	boost::shared_ptr<Club> club = safeClub->getResource();
//	club->userSwitchPlayType(msg->m_switchToType, msg->m_currPlayType, this, &userCurrentPlayTypeId);
//
//	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(GetUserDataId());
//	if (safeUser && safeUser->isValid())
//	{
//		if (safeUser->getResource()->m_currClubInfo.m_clubId == msg->m_clubId&& userCurrentPlayTypeId!=0)
//		{
//			safeUser->getResource()->m_currClubInfo.m_playTypeId = userCurrentPlayTypeId;
//		}
//	}
//
//
//
//
//	//gClubManager.userSwitchPlayType(msg->m_clubId, msg->m_switchToType,msg->m_currPlayType, this);
//
//}

//void User::HanderUserAddDeskBeforeCheckFee(LMsgC2SAddDesk* msg,Lint addDeskType)
//{
//	if (msg==NULL)
//	{
//		return;
//	}
//
//	ClubInfos  tempClubInfos;
//	if (!gDeskManager.GetClubInfoByClubDeskId(msg->m_deskId, tempClubInfos))
//	{
//		LLOG_ERROR("User::HanderUserAddDeskBeforeCheckFee  clubdeskId=[%d] not find clubInfo", msg->m_deskId);
//		LMsgS2CAddDeskRet ret;
//		ret.m_deskId=msg->m_deskId;
//		ret.m_errorCode=2;
//		Send(ret);
//		return;
//	}
//
//	//取得俱乐部的付费信息
//	clubFeeType  tempClubFeeType;
//	 Lint ret = gClubManager.getClubFeeType(tempClubInfos.m_clubId, tempClubFeeType); //0:俱乐部会长付费 1:玩家分摊
//	if (ret<0)
//	{
//		LLOG_ERROR("User::HanderUserAddDeskBeforeCheckFee  userId[%d] club[%d]  ret [%d] < 0",this->GetUserDataId(), tempClubInfos.m_clubId, ret);
//		return;
//	}
//
//	LMsgL2CeUpdateCoinJoinDesk centerSend;
//	centerSend.m_serverID=gConfig.GetServerID();
//	centerSend.m_openID=m_userData.m_openid;
//	centerSend.m_userid=m_userData.m_id;
//	centerSend.m_gateId=GetUserGateId();
//	centerSend.m_strUUID=m_userData.m_unioid;
//	centerSend.m_deskID=msg->m_deskId;
//	centerSend.m_ip=GetIp();
//	centerSend.m_usert=m_userData;
//
//	if (1==tempClubFeeType.m_clubFeeType)  // 1--分摊
//	{
//		centerSend.m_clubMasterId=0;
//	}
//	if(0==tempClubFeeType.m_clubFeeType)  //2--会长
//	{
//		centerSend.m_clubMasterId=tempClubFeeType.m_clubCreatorId;
//	}
//	centerSend.m_addDeskFlag = addDeskType;
//
//	LLOG_DEBUG("gWork.SendToCenter(centerSend)");
//	gWork.SendToCenter(centerSend);
//
//	return;
//
//}

void User::HanderUserQueryRoomGPSInfo(MHLMsgC2SQueryRoomGPSLimitInfo * msg)
{
	if (msg == NULL)
	{
		return;
	}
	
	MHLMsgLMG2LQueryRoomGPSInfo send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_deskID = msg->m_deskId;	
	send.m_usert = m_userData;
	Lint iLogicServerId = getUserLogicID();
	if (!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(msg->m_deskId);
		if (!gWork.isLogicServerExist(iLogicServerId))
		{
			iLogicServerId = INVALID_LOGICSERVERID;
		}
	}
	else
	{
		LLOG_ERROR("User::HanderUserQueryRoomGPSInfo Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}

	// 获取桌子信息失败
	if (iLogicServerId == INVALID_LOGICSERVERID)
	{
		MHLMsgS2CQueryRoomGPSLimitInfoRet ret;
		ret.m_error_code = 1;
		ret.m_gps_limit = 0; 
		Send(ret);

		LLOG_ERROR("User::HanderUserQueryRoomGPSInfo Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}
	LLOG_ERROR("*****User::HanderUserQueryRoomGPSInfo  userid=%d gps:%s", m_userData.m_id, m_userData.m_customString2.c_str());
	gWork.SendMessageToLogic(iLogicServerId, send);	
}

void User::HanderUserPokerRoomLog(LMsgC2SQiPaiLog* msg)
{
	LLOG_DEBUG("User::HanderUserPokerRoomLog user[%d] clubId[%d] msg->userId[%d] ,time[%d]",GetUserDataId(),msg->m_clubId, msg->m_userId,msg->m_time);
	LMsgL2LBDReqVipLog log;
	log.m_strUUID=m_userData.m_unioid;
	log.m_userId=GetUserDataId();
	log.m_time=msg->m_time;
	log.m_clubId = msg->m_clubId;
	log.m_flag=1;
	if (m_userData.m_gm && msg->m_userId != 0)
	{
		log.m_reqUserId = msg->m_userId;
	}
	else
	{
		log.m_reqUserId = GetUserDataId();
	}
	if (msg->m_searchId != 0)
	{
		log.m_reqUserId = msg->m_searchId;
	}
	if (msg->m_clubId != 0 && msg->m_searchId == 0)
	{
		boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_clubId);
		if (safeClub.get() != NULL && safeClub->isValid())
		{
			boost::shared_ptr<Club> club = safeClub->getResource();
			Lint owerId = 0;
			std::set<Lint> adminUsers;
			club->getClubOwnerAndAdmin(&owerId, adminUsers);

			if (GetUserDataId() == owerId || adminUsers.find(GetUserDataId()) != adminUsers.end())
			{
				LLOG_DEBUG("User::HanderUserVipLog userId[%d] all_club=1", GetUserDataId());
				log.m_allClub = 1;
			}

		}
	}


	gWork.SendMsgToDb(log);

}

void User::HanderUserJoinRoomCheck(LMsgC2SJoinRoomCheck*msg)
{
	LLOG_DEBUG("HanderUserJoinRoomCheck");
	if (msg == NULL)
	{
		return;
	}
	LMsgC2SAddDesk tempMsg;
	tempMsg.m_deskId = msg->m_deskId;
	tempMsg.m_Gps_Lat = msg->m_Gps_Lat;
	tempMsg.m_Gps_Lng = msg->m_Gps_Lng;

	return HanderUserAddDesk(&tempMsg,1);
}


void  User::HanderUserJoinRoomAnonymous(MHLMsgC2SJoinRoomAnonymous *msg)
{
	LLOG_DEBUG("HanderUserJoinRoomAnonymous");
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("HanderUserJoinRoomAnonymous  clubId[%d],playType[%d]",msg->m_clubId,msg->m_playTypeId);
	Lint clubDeskId = gClubManager.getClubDeskCanJoin(msg->m_clubId, msg->m_playTypeId);
	if (clubDeskId <= 0)
	{
		LLOG_ERROR("HanderUserJoinRoomAnonymous, user[%d] join club[%d][%d] error",GetUserDataId(),msg->m_clubId,msg->m_playTypeId);
		return;
	}

	LMsgC2SAddDesk  send;
	send.m_deskId = clubDeskId;
	send.m_Gps_Lng = msg->m_Gps_Lng;
	send.m_Gps_Lat = msg->m_Gps_Lat;

	HanderUserAddDesk(&send);
}

void User::HanderQiPaiUserReqDraw(LMsgC2SMatchLog *msg)
{
	LLOG_DEBUG("User::HanderQiPaiUserReqDraw userid=%d, deskid=%d, time=%d pos=%d", m_userData.m_id, msg->m_deskId, msg->m_time, msg->m_pos);
	LMsgL2LDBReqRoomLog log;
	log.m_id = msg->m_id;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId = GetUserDataId();
	log.m_time = msg->m_time;
	log.m_deskId = msg->m_deskId;
	log.m_pos = msg->m_pos;
	log.m_flag = 1;
	gWork.SendMsgToDb(log);

	/////////////////
	//QiPaiLog  tempQipaiLog;
	//tempQipaiLog.m_time = 1515824023;
	//tempQipaiLog.m_userCount = 3;
	//tempQipaiLog.m_videoId = "12345678";
	//tempQipaiLog.m_score[0] = -1;
	//tempQipaiLog.m_score[1] = -1;
	//tempQipaiLog.m_score[2] = 2;
	//
	//LMsgS2CMatchLog tempSend;
	//tempSend.m_flag = 101;
	//tempSend.m_size = 1;
	//tempSend.m_item[0] = tempQipaiLog;
	//
	//Send(tempSend.GetSendBuff());
		


}

void User::HanderQiPaiUserReqLog(LMsgC2SPokerVideo *msg)
{
	LLOG_DEBUG("User::HanderQiPaiUserReqLog userid=%d, videoId=[%s]", GetUserDataId(),msg->m_videoId.c_str());
	LMsgL2LDBReqVideo video;
	video.m_strUUID = m_userData.m_unioid;
	video.m_userId = GetUserDataId();
	video.m_videoId = msg->m_videoId;
	video.m_flag = 1;
	
	gWork.SendMsgToDb(video);

	//LMsgS2CPokerVideo send;
	//Send(send.GetSendBuff());

}

void User::UpdateUserData(const LUser & userData)
{
	m_userData.m_customInt[0] = userData.m_customInt[0];
	m_userData.m_customInt[4] = userData.m_customInt[4];
}


//处理玩家点击俱乐部中匹配按钮,将玩家信息发送到Center验证
//void  User::HanderUserJoinRoomAnonymous2CheckFee(MHLMsgC2SJoinRoomAnonymous *msg)
//{
//	LLOG_DEBUG("HanderUserJoinRoomAnonymous2CheckFee");
//
//	if (msg == NULL)
//	{
//		return;
//	}
//	if (this->getUserLogicID() > 0)
//	{
//		LLOG_ERROR("User::HanderUserJoinRoomAnonymous2CheckFee userId[%d] logicId[%d] return", m_userData.m_id, this->getUserLogicID());
//		LMsgS2CAddDeskRet ret;
//		ret.m_deskId = 1;
//		ret.m_errorCode = 5;
//		this->Send(ret);
//
//		return;
//	}
//	//保存用户上传的GPS信息
//	std::ostringstream ss;
//	LLOG_ERROR("User::HanderUserJoinRoomAnonymous2CheckFee update gps info -- user:%d GPS-msg->m_Gps_Lng:%s msg->m_Gps_Lat:%s", m_userData.m_id, msg-//>m_Gps_Lng.c_str(), msg->m_Gps_Lat.c_str());
//	ss << msg->m_Gps_Lng << "," << msg->m_Gps_Lat;
//	m_userData.m_customString2 = ss.str();
//
//	//取得俱乐部的付费信息
//	clubFeeType  tempClubFeeType;
//	Lint ret = gClubManager.getClubFeeType(msg->m_clubId, tempClubFeeType); //0:俱乐部会长付费 1:玩家分摊
//	if (ret<0)
//	{
//		LLOG_ERROR("User::HanderUserJoinRoomAnonymous2CheckFee   club[%d]  ret [%d] < 0", msg->m_clubId, ret);
//		return;
//	}
//
//	LMsgL2CeUpdateCoinJoinDesk centerSend;
//	centerSend.m_serverID = gConfig.GetServerID();
//	centerSend.m_openID = m_userData.m_openid;
//	centerSend.m_userid = m_userData.m_id;
//	centerSend.m_gateId = GetUserGateId();
//	centerSend.m_strUUID = m_userData.m_unioid;
//	centerSend.m_ip = GetIp();
//	centerSend.m_usert = m_userData;
//	centerSend.m_clubId = msg->m_clubId;
//	centerSend.m_playTypeId = msg->m_playTypeId;
//	centerSend.m_type = 3;  //玩家俱乐部匹配加入房间
//
//	if (1 == tempClubFeeType.m_clubFeeType)  // 1--分摊
//	{
//		centerSend.m_clubMasterId = 0;
//	}
//	if (0 == tempClubFeeType.m_clubFeeType)  //2--会长
//	{
//		centerSend.m_clubMasterId = tempClubFeeType.m_clubCreatorId;
//	}
//
//	LLOG_DEBUG("gWork.SendToCenter(centerSend)");
//	gWork.SendToCenter(centerSend);
//
//
//}

//Center返回玩家在俱乐部中匹配消息
//void  User::HanderUserJoinRoomAnonymous2(LMsgCe2LUpdateCoinJoinDesk *msg)
//{
//	LLOG_DEBUG("HanderUserJoinRoomAnonymous2");
//	if (msg == NULL)
//	{
//		return;
//	}
//	if (this->getUserLogicID() > 0)
//	{
//		LLOG_ERROR("User::HanderUserJoinRoomAnonymous2 userId[%d] logicId[%d] return", m_userData.m_id, this->getUserLogicID());
//		LMsgS2CAddDeskRet ret;
//		ret.m_deskId = 1;
//		ret.m_errorCode = 5;
//		this->Send(ret);
//		return;
//	}
//
//	LLOG_DEBUG("User::HanderUserJoinRoomAnonymous2  m_userid=[%d], m_coinNum=[%d] m_Block=[%d],m_clubMasterCoinNum=[%d],m_clubMasterBlock=[%d]", msg-//>m_userid, msg->m_coinNum, msg->m_Block, msg->m_clubMasterCoinNum, msg->m_clubMasterBlock);
//	if (msg->m_errorCode == 1)
//	{
//		//俱乐部中没有用户信息
//		LMsgS2CAddDeskRet ret;
//		//ret.m_deskId = msg->m_deskID;
//		ret.m_errorCode = 3;
//		Send(ret);
//		LLOG_ERROR("ERROR: User::HanderUserJoinRoomAnonymous2 center return  m_errorCode=1 not find userinfo, userid=%d", m_userData.m_id);
//		return;
//	}
//
//	if (msg->m_Block)
//	{
//		LMsgS2CAddDeskRet ret;
//		ret.m_deskId = msg->m_deskID;
//		ret.m_errorCode = 3;
//		Send(ret);
//
//		LLOG_ERROR("ERROR: User::HanderUserJoinRoomAnonymous2 be blocked, userid=%d", m_userData.m_id);
//		return;
//	}
//
//	//用户的房主建房预付费用
//	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);
//	Lint creatDeskCost = 0;
//	if (deskList.get() != NULL && deskList->isValid())
//	{
//		creatDeskCost = deskList->getResource()->m_cost;
//	}
//
//	//取得俱乐部的付费信息
//	clubFeeType  tempClubFeeType;
//	Lint ret = gClubManager.getClubFeeType(msg->m_clubId, tempClubFeeType, msg->m_playTypeId); //0:俱乐部会长付费 1:玩家分摊
//	if (ret<0)
//	{
//		LLOG_ERROR("User::HanderUserJoinRoomAnonymous2   club[%d]  ret [%d] < 0", ret);
//		return;
//	}
//
//	LLOG_DEBUG("User::HanderUserJoinRoomAnonymous2 club[%d]  feetype=[%d]", msg->m_clubId, tempClubFeeType.m_clubFeeType);
//
//	//获取加入房间费用
//	deskFeeItem tempItem;
//	gMjConfig.GetFeeTypeValue(tempClubFeeType.m_playTypeCircle, tempItem, tempClubFeeType.m_state);
//
//	//检查费用
//	if (0 == tempClubFeeType.m_clubFeeType)  //检查会长的 ,这里不做处理,扣费的地方处理
//	{
//		if (msg->m_clubMasterCoinNum <= 0)   //会长钻石券不足
//		{
//			LLOG_ERROR("User::HanderUserJoinRoomAnonymous2 club  creator [%d] <= [%d] coins not enough  joinUserId=[%d]", msg->m_clubMasterCoinNum, 0, msg-//>m_userID);
//			LMsgS2CAddDeskRet ret;
//			//ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = 7;
//			Send(ret);
//			return;
//		}
//	}
//	else //均摊
//	{
//		if (msg->m_coinNum - creatDeskCost<tempItem.m_even)
//		{
//			LLOG_ERROR("User::HanderUserJoinRoomAnonymous2 club  userId [%d] coins not enough  ", msg->m_userid);
//			LMsgS2CAddDeskRet ret;
//			//ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = 4;
//			Send(ret);
//			return;
//		}
//	}
//
//	struct PlayTypeJoinUserInfo playTypeUser;
//	Lint isCheckGPs = 0;
//	LLOG_DEBUG("HanderUserJoinRoomAnonymous2  clubId[%d],playType[%d]", msg->m_clubId, msg->m_playTypeId);
//	Lint ret1 = gClubManager.userApplyJoinPlayTypeDesk(msg->m_clubId, msg->m_playTypeId, this, &playTypeUser, &isCheckGPs);
//	if (ret1 < 0)
//	{
//		LLOG_ERROR("HanderUserJoinRoomAnonymous2, user[%d] join club[%d][%d] error", GetUserDataId(), msg->m_clubId, msg->m_playTypeId);
//		return;
//	}
//
//	//gps 判断
//	LLOG_DEBUG("HanderUserJoinRoomAnonymous2  isCheckGPs=[%d]", isCheckGPs);
//	if (isCheckGPs == 1)
//	{
//		double meLat = 0.0, meLng = 0.0;
//		get_gps_pair_values(m_userData.m_customString2, meLat, meLng);
//
//		for (auto ItUserGas = playTypeUser.userGps.begin(); ItUserGas != playTypeUser.userGps.end(); ItUserGas++)
//		{
//			LLOG_DEBUG("HanderUserJoinRoomAnonymous2 GPSString[%s]", ItUserGas->second.c_str());
//			if (ItUserGas->first == GetUserDataId())continue;
//			double youLat = 0.0, youLng = 0.0;
//			get_gps_pair_values(ItUserGas->second, youLat, youLng);
//
//			double distance = calc_gps_distance(meLat, meLng, youLat, youLng);
//			if (distance <= _MH_GPS_DISTANCE_LIMIT)
//			{
//				struct PlayTypeJoinUserInfo playTypeUser;
//				Lint ret = gClubManager.userApplyLeavePlayTypeDesk(msg->m_clubId, msg->m_playTypeId, this, &playTypeUser);
//
//				//距离近
//				LTime  nowTime;
//				LMsgS2CWaitJoinRoom  send;
//				send.m_ErrorCode = 1;
//				send.m_MaxUser = playTypeUser.m_userMax;
//				send.m_CurrentUser = playTypeUser.m_currUserMax;
//				send.m_Time = DELAY_TIME_USER_WAIT_JOIN_IN_CLUB_DESK - (nowTime.Secs() - playTypeUser.m_time.Secs());
//
//
//				int userCount = 0;
//				for (auto ItUserGas = playTypeUser.userGps.begin(); ItUserGas != playTypeUser.userGps.end(); ItUserGas++)
//				{
//					std::stringstream ss;
//					ss << "Anonymous" << ",";
//					ss << ItUserGas->second;
//
//					if (userCount < playTypeUser.userGps.size() - 1)
//						ss << "|";
//					if (playTypeUser.userGps.size() == 1)
//						ss << "|";
//
//					userCount++;
//
//					send.m_UseGpsList.append(ss.str());
//
//				}
//
//				LLOG_DEBUG("HanderUserJoinRoomAnonymous2  userid=[%d],gpslist=[%s]", GetUserDataId(), send.m_UseGpsList.c_str());
//				//ret.m_userGPSList = ItUserGas->second;
//
//				Send(send.GetSendBuff());
//				return;
//
//			}
//		}
//	}
//
//	LTime  nowTime;
//	LMsgS2CWaitJoinRoom  send;
//	send.m_MaxUser = playTypeUser.m_userMax;
//	send.m_CurrentUser = playTypeUser.m_currUserMax;
//	send.m_Time = DELAY_TIME_USER_WAIT_JOIN_IN_CLUB_DESK - (nowTime.Secs() - playTypeUser.m_time.Secs());
//
//	LLOG_DEBUG("HanderUserJoinRoomAnonymous2  MAX[%d],CURRENT[%d],TIME[%d] usersize[%d]", send.m_MaxUser, send.m_CurrentUser, send.m_Time, //playTypeUser.userIdSet.size());
//
//	Send(send.GetSendBuff());
//	for (auto itUser = playTypeUser.userIdSet.begin(); itUser != playTypeUser.userIdSet.end(); itUser++)
//	{
//		LTime  nowTime;
//		LMsgS2CWaitJoinRoom  send;
//		send.m_MaxUser = playTypeUser.m_userMax;
//		send.m_CurrentUser = playTypeUser.m_currUserMax;
//		send.m_Time = DELAY_TIME_USER_WAIT_JOIN_IN_CLUB_DESK - (nowTime.Secs() - playTypeUser.m_time.Secs());
//
//		if (GetUserDataId() == *itUser)continue;
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(*itUser);
//		if (safeUser && safeUser->isValid())
//		{
//			boost::shared_ptr<User> user = safeUser->getResource();
//			LLOG_DEBUG("HanderUserJoinRoomAnonymous2 boardcast to user[%d]", user->GetUserDataId());
//			user->Send(send.GetSendBuff());
//		}
//	}
//
//	this->setUserClubStatus(USER_CLUB_PIPEI_STATUS);
//
//	if (ret1 == 1)
//	{
//		LLOG_DEBUG("HanderUserJoinRoomAnonymous2 START GAME");
//		//3人匹配
//		std::vector<Lint> waitUserList;
//		for (auto itUser = playTypeUser.userIdSet.begin(); itUser != playTypeUser.userIdSet.end(); itUser++)
//		{
//			waitUserList.push_back(*itUser);
//		}
//
//		//玩法规定人数
//		Lint t_maxPlyerCount = 0;
//		if (playTypeUser.m_state == 101) t_maxPlyerCount = 3;
//		else if (playTypeUser.m_state == 106) t_maxPlyerCount = 4;
//		else if (playTypeUser.m_state == 107) t_maxPlyerCount = 5;
//		else if (playTypeUser.m_state == 109) t_maxPlyerCount = 4;
//		else if (playTypeUser.m_state == 110) t_maxPlyerCount = 5;
//
//		Lint count = waitUserList.size() / t_maxPlyerCount;
//		LLOG_DEBUG("HanderUserJoinRoomAnonymous2 START GAME  count=[%d]", count);
//		for (int i = 0; i < count; i++)
//		{
//
//			Lint clubDeskId = gClubManager.getClubDeskCanJoin(msg->m_clubId, msg->m_playTypeId);
//			if (clubDeskId <= 0)
//			{
//				LLOG_ERROR("HanderUserJoinRoomAnonymous2, user[%d] join club[%d][%d] error", GetUserDataId(), msg->m_clubId, msg->m_playTypeId);
//				return;
//			}
//
//			//开始游戏
//			//for (auto itUser = playTypeUser.userIdSet.begin(); itUser != playTypeUser.userIdSet.end(); itUser++)
//			for (int j = 0 + i * t_maxPlyerCount; j < t_maxPlyerCount + i * t_maxPlyerCount; j++)
//			{
//				//LMsgC2SAddDesk msgAddDesk;
//				//msgAddDesk.m_deskId = clubDeskId;
//				//
//				if (waitUserList[j] == GetUserDataId())
//				{
//					UserAddToClubDeskInfo send;
//					send.m_userid = this->GetUserDataId();
//					send.m_gateId = this->GetUserGateId();
//					send.m_strUUID = this->m_userData.m_unioid;
//					send.m_deskID = clubDeskId;
//					send.m_Free = msg->m_Free;
//					send.m_ip = this->GetIp();
//					send.m_usert = this->m_userData;
//					send.m_creatDeskCost = 0;
//					send.m_cost = tempItem.m_cost;
//					send.m_even = tempItem.m_even;
//					send.m_addDeskFlag = msg->m_addDeskFlag;
//					send.m_type = msg->m_type;
//					gClubManager.userEnterClubDesk(msg->m_clubId, msg->m_playTypeId, send.m_deskID, send, this);
//
//					//this->setUserClubStatus(USER_CLUB_DESK_STATUS);
//				}
//				else
//				{
//					boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(waitUserList[j]);
//					if (safeUser && safeUser->isValid())
//					{
//						boost::shared_ptr<User> user = safeUser->getResource();
//						LLOG_DEBUG("HanderUserJoinRoomAnonymous2  userId[%d] join desk[%d]", waitUserList[j], clubDeskId);
//						UserAddToClubDeskInfo send;
//						send.m_userid = user->GetUserDataId();
//						send.m_gateId = user->GetUserGateId();
//						send.m_strUUID = user->m_userData.m_unioid;
//						send.m_deskID = clubDeskId;
//						send.m_Free = msg->m_Free;
//						send.m_ip = user->GetIp();
//						send.m_usert = user->m_userData;
//						send.m_creatDeskCost = 0;
//						send.m_cost = tempItem.m_cost;
//						send.m_even = tempItem.m_even;
//						send.m_addDeskFlag = msg->m_addDeskFlag;
//						send.m_type = msg->m_type;
//						gClubManager._userEnterClubDesk(msg->m_clubId, msg->m_playTypeId, send.m_deskID, send, user.get());
//
//						//user->setUserClubStatus(USER_CLUB_DESK_STATUS);
//					}
//					else
//					{
//						LLOG_ERROR("ERROR HanderUserJoinRoomAnonymous2 user[%d]  join desk[%d]", waitUserList[j], clubDeskId);
//						//boost::shared_ptr<CSafeResourceLock<clubUserInfo> >  safeClubUserInfo = gClubManager.getClubUserInfo(waitUserList[j]);
//						//if (safeClubUserInfo && safeClubUserInfo->isValid())
//						//{
//						//	LUser tempLUser;
//						//	tempLUser.m_id = safeClubUserInfo->getResource()->m_userId;
//						//	tempLUser.m_unioid = safeClubUserInfo->getResource()->m_unionId;
//						//	tempLUser.m_nike = safeClubUserInfo->getResource()->m_nike;
//						//	tempLUser.m_headImageUrl = safeClubUserInfo->getResource()->m_headImageUrl;
//						//	//tempLUser.m_sex = safeClubUserInfo->getResource()->m_sex;
//						//
//						//	boost::shared_ptr<User> user;
//						//	user.reset(new User(tempLUser, 1));
//						//	UserManager::Instance().addUser(user);
//						//
//						//	UserAddToClubDeskInfo send;
//						//	send.m_userid = tempLUser.m_id;
//						//	send.m_gateId = GetUserGateId();
//						//	send.m_strUUID = tempLUser.m_unioid;
//						//	send.m_deskID = clubDeskId;
//						//	send.m_Free = msg->m_Free;
//						//	send.m_ip = GetIp();
//						//	send.m_usert = tempLUser;
//						//	send.m_creatDeskCost = 0;
//						//	send.m_cost = tempItem.m_cost;
//						//	send.m_even = tempItem.m_even;
//						//	send.m_addDeskFlag = msg->m_addDeskFlag;
//						//
//						//	LLOG_DEBUG("HanderUserJoinRoomAnonymous2 user[%d] uuid[%s]  join desk[%d]", waitUserList[j], tempLUser.m_unioid.c_str(),clubDeskId);
//						//	gClubManager.userEnterClubDesk(msg->m_clubId, msg->m_playTypeId, send.m_deskID, send);
//						//
//						//}
//					}
//				}
//			}
//		}
//
//	}
//}

//void  User::HanderQiPaiUserLeaveWait(LMsgC2SPokerExitWait *msg)
//{
//	LLOG_DEBUG("HanderQiPaiUserLeaveWait   userId=[%d] leave clubId[%d] playTypeId[%d]", GetUserDataId(), msg->m_clubId, msg->m_playTypeId);
//	struct PlayTypeJoinUserInfo playTypeUser;
//	Lint ret = gClubManager.userApplyLeavePlayTypeDesk(msg->m_clubId, msg->m_playTypeId, this, &playTypeUser);
//
//	LTime nowTime;
//	LMsgS2CWaitJoinRoom  send;
//	send.m_MaxUser = playTypeUser.m_userMax;
//	send.m_CurrentUser = playTypeUser.m_currUserMax;
//	send.m_Time = DELAY_TIME_USER_WAIT_JOIN_IN_CLUB_DESK - (nowTime.Secs() - playTypeUser.m_time.Secs());
//
//	LMsgS2CPokerExitWait send1;
//	if (ret == 0)
//		send1.m_errorCode = 0;
//	else
//		send1.m_errorCode = 0;
//
//	Send(send1.GetSendBuff());
//
//	for (auto itUser = playTypeUser.userIdSet.begin(); itUser != playTypeUser.userIdSet.end(); itUser++)
//	{
//		if (GetUserDataId() == *itUser)continue;
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(*itUser);
//		if (safeUser && safeUser->isValid())
//		{
//			LTime  nowTime;
//			LMsgS2CWaitJoinRoom  send;
//			send.m_MaxUser = playTypeUser.m_userMax;
//			send.m_CurrentUser = playTypeUser.m_currUserMax;
//			send.m_Time = DELAY_TIME_USER_WAIT_JOIN_IN_CLUB_DESK - (nowTime.Secs() - playTypeUser.m_time.Secs());
//
//			boost::shared_ptr<User> user = safeUser->getResource();
//			LLOG_DEBUG("HanderQiPaiUserLeaveWait boardcast to user[%d]", user->GetUserDataId());
//			user->Send(send.GetSendBuff());
//		}
//	}
//
//	this->setUserClubStatus(USER_CLUB_INIT_STATUS);
//
//}

//void User::HanderUserCreateClubDesk(LMsgC2SClubUserCreateRoom* msg)
//{
//	if (msg == NULL)
//	{
//		return;
//	}
//
//	LLOG_ERROR("User::HanderUserCreateClubDesk  -- user:%d GPS-msg->m_Gps_Lng:%s msg->m_Gps_Lat:%s", m_userData.m_id, msg->m_Gps_Lng.c_str(), msg-//>m_Gps_Lat.c_str());
//	//保存用户上传的GPS信息
//	std::ostringstream ss;
//
//	ss << msg->m_Gps_Lng << "," << msg->m_Gps_Lat;
//	m_userData.m_customString2 = ss.str();
//
//	//取得俱乐部的付费信息
//	clubFeeType  tempClubFeeType;
//	Lint ret = gClubManager.getClubFeeType(msg->m_clubId, tempClubFeeType); //0:俱乐部会长付费 1:玩家分摊
//	if (ret<0)
//	{
//		LLOG_ERROR("User::HanderUserAddDeskBeforeCheckFee   club[%d]  ret [%d] < 0", ret);
//		return;
//	}
//
//	LMsgL2CeUpdateCoinJoinDesk centerSend;
//	centerSend.m_serverID = gConfig.GetServerID();
//	centerSend.m_openID = m_userData.m_openid;
//	centerSend.m_userid = m_userData.m_id;
//	centerSend.m_gateId = GetUserGateId();
//	centerSend.m_strUUID = m_userData.m_unioid;
//	centerSend.m_deskID = 0;
//	centerSend.m_ip = GetIp();
//	centerSend.m_usert = m_userData;
//	centerSend.m_clubId = msg->m_clubId;
//	centerSend.m_playTypeId = msg->m_playTypeId;
//	centerSend.m_type = 4;
//
//	if (1 == tempClubFeeType.m_clubFeeType)  // 1--分摊
//	{
//		centerSend.m_clubMasterId = 0;
//	}
//	if (0 == tempClubFeeType.m_clubFeeType)  //2--会长
//	{
//		centerSend.m_clubMasterId = tempClubFeeType.m_clubCreatorId;
//	}
//	centerSend.m_addDeskFlag = 0;
//
//
//	gWork.SendToCenter(centerSend);
//
//	return;
//}
//
////客户端用户加入房间
//void User::RealDoHanderUserCreateClubDesk(LMsgCe2LUpdateCoinJoinDesk* msg)
//{
//	LLOG_DEBUG("User::RealDoHanderUserCreateClubDesk");
//	if (msg == NULL)
//	{
//		return;
//	}
//	LLOG_DEBUG("User::RealDoHanderUserCreateClubDesk  m_userid=[%d], m_coinNum=[%d] m_Block=[%d],m_clubMasterCoinNum=[%d],m_clubMasterBlock=[%d]", msg-//>m_userid, msg->m_coinNum, msg->m_Block, msg->m_clubMasterCoinNum, msg->m_clubMasterBlock);
//	if (msg->m_errorCode == 1)
//	{
//		//俱乐部中没有用户信息
//		LMsgS2CCreateDeskRet ret;
//		ret.m_deskId = msg->m_deskID;
//		ret.m_errorCode = 4;
//		Send(ret);
//		LLOG_ERROR("ERROR: User::RealDoHanderUserCreateClubDesk center return  m_errorCode=4 not find userinfo, userid=%d", m_userData.m_id);
//		return;
//	}
//
//	Lint limit = gDeskManager.GetDeskCreditLimit(msg->m_deskID);
//	if (limit > 0)
//	{
//		if (m_userData.m_creditValue < limit)
//		{
//			LMsgS2CCreateDeskRet ret;
//			ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CreditNotEnough;
//			Send(ret);
//
//			LLOG_ERROR("ERROR: User::RealDoHanderUserCreateClubDesk credits not enough, userid=%d", m_userData.m_id);
//			return;
//		}
//	}
//
//	if (msg->m_Block)
//	{
//		LMsgS2CCreateDeskRet ret;
//		ret.m_deskId = msg->m_deskID;
//		ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CreditNotEnough;
//		Send(ret);
//
//		LLOG_ERROR("ERROR: User::RealDoHanderUserCreateClubDesk be blocked, userid=%d", m_userData.m_id);
//		return;
//	}
//
//	//用户的房主建房预付费用
//	boost::shared_ptr<CSafeResourceLock<UserDeskList>> deskList = gUserManager.getUserDeskListbyUserId(m_userData.m_id);
//	Lint creatDeskCost = 0;
//	if (deskList.get() != NULL && deskList->isValid())
//	{
//		creatDeskCost = deskList->getResource()->m_cost;
//	}
//
//
//	//取得俱乐部的付费信息
//	clubFeeType  tempClubFeeType;
//	Lint ret = gClubManager.getClubFeeType(msg->m_clubId, tempClubFeeType, msg->m_playTypeId); //0:俱乐部会长付费 1:玩家分摊
//	if (ret<0)
//	{
//		LLOG_ERROR("User::RealDoHanderUserCreateClubDesk   club[%d]  ret [%d] < 0", ret);
//		return;
//	}
//
//	LLOG_DEBUG("User::RealDoHanderUserCreateClubDesk club  feetype=[%d]", tempClubFeeType.m_clubFeeType);
//
//	//获取加入房间费用
//	deskFeeItem tempItem;
//	gMjConfig.GetFeeTypeValue(tempClubFeeType.m_playTypeCircle, tempItem, tempClubFeeType.m_state);
//
//	//检查费用
//	if (0 == tempClubFeeType.m_clubFeeType)  //检查会长的 ,这里不做处理,扣费的地方处理
//	{
//		if (msg->m_clubMasterCoinNum <= 0)   //会长钻石券不足
//		{
//			LLOG_ERROR("User::RealDoHanderUserCreateClubDesk club  creator [%d] <= [%d] coins not enough  joinUserId=[%d]", msg->m_clubMasterCoinNum, 0, msg-//>m_userID);
//			LMsgS2CCreateDeskRet ret;
//			ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CLUB_MASTER_FEE_NOT_ENOUGH;
//
//			Send(ret);
//			return;
//		}
//	}
//	else //均摊
//	{
//		if (msg->m_coinNum - creatDeskCost<tempItem.m_even)
//		{
//			LLOG_ERROR("User::RealDoHanderUserCreateClubDesk club  userId [%d] coins not enough  ", msg->m_userid);
//			LMsgS2CCreateDeskRet ret;
//			ret.m_deskId = msg->m_deskID;
//			ret.m_errorCode = 1;
//			Send(ret);
//			return;
//		}
//	}
//
//	//检查用户是否有logicid
//	Lint iLogicServerId = getUserLogicID();
//	if (gWork.isLogicServerExist(iLogicServerId) && iLogicServerId != INVALID_LOGICSERVERID)
//	{
//		LMsgLMG2LAddToDesk send;
//		send.m_userid = m_userData.m_id;
//		send.m_gateId = GetUserGateId();
//		send.m_strUUID = m_userData.m_unioid;
//		send.m_deskID = msg->m_deskID;
//		send.m_ip = msg->m_userIp;
//		send.m_usert = m_userData;
//		send.m_creatDeskCost = creatDeskCost;
//		send.m_Free = msg->m_Free;
//		send.m_startTime = msg->m_startTime;
//		send.m_endTime = msg->m_endTime;
//		send.m_addDeskFlag = msg->m_addDeskFlag;
//		gWork.SendMessageToLogic(iLogicServerId, send);
//		return;
//
//	}
//
//	UserAddToClubDeskInfo send;
//	send.m_userid = m_userData.m_id;
//	send.m_gateId = GetUserGateId();
//	send.m_strUUID = m_userData.m_unioid;
//	send.m_deskID = msg->m_deskID;
//	send.m_Free = msg->m_Free;
//	send.m_ip = GetIp();
//	send.m_usert = m_userData;
//	send.m_creatDeskCost = creatDeskCost;
//	send.m_cost = tempItem.m_roomCost;
//	send.m_even = tempItem.m_even;
//	send.m_addDeskFlag = msg->m_addDeskFlag;
//
//	LLOG_DEBUG("User::HanderUserEnterClubDesk  deskId=[%d] , clubId=[%d],playTypeId=[%d]", msg->m_deskID, msg->m_clubId, msg->m_playTypeId);
//	gClubManager.userCreateClubDesk(this, msg->m_clubId, msg->m_playTypeId, send);
//
//	return;
//}

//void User::HanderUserRequestClubMember(LMsgC2SRequestClubMemberList *msg)
//{
//	if (msg == NULL)
//	{
//		return;
//	}
//
//	LMsgS2CClubMemberList  send;
//	send.m_ClubId = msg->m_ClubId;
//	send.m_Time = time(NULL);
//	//send.m_Page = msg->m_Page;
//
//	send.m_OnlineUserCount = gClubUserManager.getClubUserCountOnline(msg->m_ClubId);
//	send.m_UserCount = gClubUserManager.getClubUserCount(msg->m_ClubId);
//
//	LLOG_ERROR("HanderUserRequestClubMember userId[%d] clubId[%d] page[%d] userId[%d] onlineUserCount[%d] userCount[%d]", GetUserDataId(), msg->m_ClubId, msg-//>m_Page, msg->m_SearchUserId, send.m_OnlineUserCount, send.m_UserCount);
//
//	//根据昵称查询
//	if (!msg->m_Nike.empty())
//	{
//		std::set<boost::shared_ptr<ClubUser>  > clubUserList = gClubUserManager.getClubUserbyUserNike(msg->m_ClubId, msg->m_Nike);
//		for (auto itUserInfo = clubUserList.begin(); itUserInfo != clubUserList.end(); itUserInfo++)
//		{
//			if (itUserInfo->get()==NULL || (*itUserInfo) == NULL)continue;
//			ClubMember  tempClubMember;
//			tempClubMember.m_Id = (*itUserInfo)->getID();
//			tempClubMember.m_Nike = (*itUserInfo)->m_nike;
//			tempClubMember.m_photo = (*itUserInfo)->m_headImageUrl;
//			tempClubMember.m_State = (*itUserInfo)->getClientState();
//			tempClubMember.m_LastTime = (*itUserInfo)->m_offLineTime;
//			if ((*itUserInfo)->m_offLineTime == 0)
//				tempClubMember.m_LastTime = time(NULL);
//			tempClubMember.m_status = (*itUserInfo)->getUserType(msg->m_ClubId);
//			LLOG_ERROR("HanderUserRequestClubMember :userId[%d],nike[%s],state[%d],lastTime[%d] type[%d]", tempClubMember.m_Id, tempClubMember.m_Nike.c_str(), //tempClubMember.m_State, tempClubMember.m_LastTime, tempClubMember.m_status);
//			send.m_MemberList.push_back(tempClubMember);
//		}
//		Send(send.GetSendBuff());
//		return;
//	}
//
//	//全查  或者 根据userId查询
//	if (msg->m_SearchUserId == 0)
//	{
//		std::set<Lint> oldUserSet;
//		for (auto ItOld = msg->m_oldUser.begin(); ItOld != msg->m_oldUser.end(); ItOld++)
//		{
//			oldUserSet.insert(*ItOld);
//		}
//
//		std::set<boost::shared_ptr<ClubUser>  > clubUserInGame = gClubUserManager.getClubUserInGame(msg->m_ClubId);
//		std::set<boost::shared_ptr<ClubUser>  > clubUserOnlineNoGame = gClubUserManager.getClubAllOnLineUserNoGame(msg->m_ClubId);
//		std::set<boost::shared_ptr<ClubUser>  > clubUserOfflineNoGame = gClubUserManager.getClubAllOffLineUserNoGame(msg->m_ClubId);
//		
//
//		
//		std::vector<boost::shared_ptr<ClubUser> > pageUser;
//		std::set<Lint>  pageUserId;
//
//		//获取会长和管理员
//		Lint clubOwnerId = 0;
//		std::set<Lint> admin;
//		boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_ClubId);
//		if (safeClub.get() && safeClub->isValid())
//		{
//			boost::shared_ptr<Club> club = safeClub->getResource();
//			club->getClubOwnerAndAdmin(&clubOwnerId, admin);
//		}
//
//		std::vector<Lint>  prePageId;
//		//自己
//		if (GetUserDataId()== clubOwnerId &&oldUserSet.find(GetUserDataId()) == oldUserSet.end() )
//		{
//			prePageId.push_back(GetUserDataId());
//		}
//		//会长
//		if (clubOwnerId != 0 && oldUserSet.find(clubOwnerId) == oldUserSet.end() )
//		{
//			prePageId.push_back(clubOwnerId);
//		}
//
//		//管理员
//		for (auto ItAdmin = admin.begin(); ItAdmin != admin.end(); ItAdmin++)
//		{
//			if (*ItAdmin != 0 && *ItAdmin != GetUserDataId() && oldUserSet.find(*ItAdmin) == oldUserSet.end())
//			{
//				prePageId.push_back(*ItAdmin);
//			}
//		}
//
//
//		bool isEnd = false;
//		for (auto ItPrePageId = prePageId.begin(); ItPrePageId != prePageId.end(); ItPrePageId++)
//		{
//			boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(*ItPrePageId);
//			if (safeUser && safeUser->isValid())
//			{
//				pageUser.push_back(safeUser->getResource());
//				pageUserId.insert(safeUser->getResource()->getID());
//				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
//				{
//					isEnd = true;
//					break;
//				}
//			}
//		}
//
//		//在线
//		if (!isEnd)
//		{
//			for (auto ItOnLineUser = clubUserOnlineNoGame.begin(); ItOnLineUser != clubUserOnlineNoGame.end(); ItOnLineUser++)
//			{
//				if (ItOnLineUser->get() && oldUserSet.find(ItOnLineUser->get()->getID()) != oldUserSet.end())
//				{
//					continue;
//				}
//				if (pageUserId.find((*ItOnLineUser)->getID()) != pageUserId.end())continue;
//				pageUser.push_back(*ItOnLineUser);
//				pageUserId.insert((*ItOnLineUser)->getID());
//				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
//				{
//					isEnd = true;
//					break;
//				}
//
//			}
//		}
//
//		//游戏中
//		if (!isEnd)
//		{
//			for (auto ItInGameUser = clubUserInGame.begin(); ItInGameUser != clubUserInGame.end(); ItInGameUser++)
//			{
//				if (ItInGameUser->get() && oldUserSet.find(ItInGameUser->get()->getID()) != oldUserSet.end())
//				{
//					continue;
//				}
//				if (pageUserId.find((*ItInGameUser)->getID()) != pageUserId.end())continue;
//				pageUser.push_back(*ItInGameUser);
//				pageUserId.insert((*ItInGameUser)->getID());
//				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
//				{
//					isEnd = true;
//					break;
//				}
//
//			}
//		}
//
//		//离线
//		if (!isEnd)
//		{
//			for (auto ItOffLineUser = clubUserOfflineNoGame.begin(); ItOffLineUser != clubUserOfflineNoGame.end(); ItOffLineUser++)
//			{
//				if (ItOffLineUser->get() && oldUserSet.find(ItOffLineUser->get()->getID()) != oldUserSet.end())
//				{
//					continue;
//				}
//				if (pageUserId.find((*ItOffLineUser)->getID()) != pageUserId.end())continue;
//				pageUser.push_back(*ItOffLineUser);
//				pageUserId.insert((*ItOffLineUser)->getID());
//
//				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
//				{
//					break;
//				}
//			}
//
//		}
//
//		if (pageUser.size() < CLUB_USER_PAGE_COUNT)
//		{
//			send.m_HasSendAll = 1;
//		}
//
//		for (auto itUserInfo = pageUser.begin(); itUserInfo != pageUser.end(); itUserInfo++)
//		{
//			if ((*itUserInfo) == NULL)continue;
//			ClubMember  tempClubMember;
//			tempClubMember.m_Id = (*itUserInfo)->getID();
//			tempClubMember.m_Nike = (*itUserInfo)->m_nike;
//			tempClubMember.m_photo = (*itUserInfo)->m_headImageUrl;
//			tempClubMember.m_State = (*itUserInfo)->getClientState();
//			tempClubMember.m_LastTime = (*itUserInfo)->m_offLineTime;
//			if ((*itUserInfo)->m_offLineTime == 0)
//				tempClubMember.m_LastTime = time(NULL);
//			tempClubMember.m_status = (*itUserInfo)->getUserType(msg->m_ClubId);
//			LLOG_ERROR("HanderUserRequestClubMember :userId[%d],nike[%s],state[%d],lastTime[%d] type[%d]", tempClubMember.m_Id,tempClubMember.m_Nike.c_str(), //tempClubMember.m_State, tempClubMember.m_LastTime, tempClubMember.m_status);
//			send.m_MemberList.push_back(tempClubMember);
//		}
//		send.m_Count = send.m_MemberList.size();
//		
//
//	}
//	else
//	{
//		send.m_HasSendAll = 1;
//		if (!gClubUserManager.isUserInClub(msg->m_ClubId, msg->m_SearchUserId))
//		{
//			send.m_ErrorCode = 1;
//		}
//		else
//		{
//			boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(msg->m_SearchUserId);
//			if (safeClubUserInfo && safeClubUserInfo->isValid())
//			{
//				ClubMember  tempClubMember;
//				tempClubMember.m_Id = (safeClubUserInfo->getResource())->getID();
//				tempClubMember.m_Nike = (safeClubUserInfo->getResource())->m_nike;
//				tempClubMember.m_photo = (safeClubUserInfo->getResource())->m_headImageUrl;
//				tempClubMember.m_State = (safeClubUserInfo->getResource())->getClientState();
//				tempClubMember.m_LastTime = (safeClubUserInfo->getResource())->m_offLineTime;
//				if ((safeClubUserInfo->getResource())->m_offLineTime == 0)
//					tempClubMember.m_LastTime = time(NULL);
//				tempClubMember.m_status = (safeClubUserInfo->getResource())->getUserType(msg->m_ClubId);
//				LLOG_ERROR("HanderUserRequestClubMember userId[%d],nike[%s],state[%d],lastTime[%d] type[%d]", tempClubMember.m_Id, tempClubMember.m_Nike.c_str(), //tempClubMember.m_State, tempClubMember.m_LastTime, tempClubMember.m_status);
//				send.m_MemberList.push_back(tempClubMember);
//			}
//		}
//	}
//
//	Send(send.GetSendBuff());
//}

void User::HanderUserRequestClubOnLineCount(LMsgC2SRequestClubOnLineCount *msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("HanderUserRequestClubOnLineCount  clubId[%d]", msg->m_ClubId);
	LMsgS2CClubOnlineCount send;

	send.m_clubId = msg->m_ClubId;
	send.m_OnlineCount = gClubUserManager.getClubUserCountOnline(msg->m_ClubId);
	send.m_totalCount = gClubUserManager.getClubUserCount(msg->m_ClubId);
	LLOG_ERROR("HanderUserRequestClubOnLineCount  clubId[%d] onlineUserSize[%d] total[%d]", send.m_clubId, send.m_OnlineCount, send.m_totalCount);
	Send(send.GetSendBuff());

}

void User::HanderUserRequestClubMemberOnlineNoGame(LMsgC2SRequestClubMemberOnlineNogame *msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("HanderUserRequestClubMemberOnlineNoGame msg is null");
		return;
	}

	//LLOG_DEBUG("HanderUserRequestClubMemberOnlineNoGame  userId[%d] clubId[%d]", GetUserDataId(), msg->m_ClubId);
	LMsgS2CClubOnLineMemberList  send;
	send.m_ClubId = msg->m_ClubId;
	if (msg->m_ClubId != 0)
	{
		std::set<boost::shared_ptr<ClubUser>  >  clubOnlineNoGameList = gClubUserManager.getClubAllOnLineUserNoGame(msg->m_ClubId);
		for (auto itOnlineNoGameUser = clubOnlineNoGameList.begin(); itOnlineNoGameUser != clubOnlineNoGameList.end(); itOnlineNoGameUser++)
		{
			if (itOnlineNoGameUser->get() == NULL)continue;
			if (itOnlineNoGameUser->get()->getID() == GetUserDataId())continue;
			ClubMemberOnLine  member;
			member.m_Id = itOnlineNoGameUser->get()->getID();
			member.m_Nike = itOnlineNoGameUser->get()->m_nike;
			member.m_photo = itOnlineNoGameUser->get()->m_headImageUrl;
			member.m_state = itOnlineNoGameUser->get()->getClientState();

			//LLOG_ERROR("HanderUserRequestClubMemberOnlineNoGame userId[%d]   user[%d]", GetUserDataId(), member.m_Id);
			send.m_MemberList.push_back(member);
		}
	}
	else
	{
		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(GetUserDataId());
		if (safeClubUserInfo && safeClubUserInfo->isValid())
		{
			for (auto ItClubId = safeClubUserInfo->getResource()->m_allClubId.begin(); ItClubId != safeClubUserInfo->getResource()->m_allClubId.end(); ItClubId++)
			{
				std::set<boost::shared_ptr<ClubUser>  >  clubOnlineNoGameList = gClubUserManager.getClubAllOnLineUser(*ItClubId);

				for (auto itOnlineNoGameUser = clubOnlineNoGameList.begin(); itOnlineNoGameUser != clubOnlineNoGameList.end(); itOnlineNoGameUser++)
				{
					if (itOnlineNoGameUser->get() == NULL)continue;
					if (itOnlineNoGameUser->get()->getID() == GetUserDataId())continue;
					ClubMemberOnLine  member;
					member.m_Id = itOnlineNoGameUser->get()->getID();
					member.m_Nike = itOnlineNoGameUser->get()->m_nike;
					member.m_photo = itOnlineNoGameUser->get()->m_headImageUrl;
					member.m_state = itOnlineNoGameUser->get()->getClientState();

					send.m_MemberList.push_back(member);
				}
			}

		}
		else
		{
			send.m_ErrorCode = 2;
		}
	}


	this->Send(send.GetSendBuff());

}

void User::HanderUserInviteClubMember(LMsgC2SInviteClubMember *msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("HanderUserInviteClubMember msg is null");
		return;
	}

	if (msg->m_ClubId == 0 || msg->m_InviteeId == 0)
	{
		LLOG_ERROR("HanderUserInviteClubMember clubId == 0 ||m_InviteeId ==0 ");
		return;
	}
	LLOG_ERROR("HanderUserInviteClubMember userId[%d] clubId[%d] showdeskId[%d] inviteeId[%d] state[%d] clubdesk[%d]", this->GetUserDataId(), msg->m_ClubId, msg->m_ClubShowDeskId, msg->m_InviteeId, msg->m_state,msg->m_clubDeskId);

	LMsgS2CInviteClubMemberReply send;
	send.m_ClubId = msg->m_ClubId;
	send.m_InviteeId = msg->m_InviteeId;



	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_InviteeId);
	if (safeUser.get() != NULL && safeUser->isValid())
	{
		boost::shared_ptr<User> InviteeUser = safeUser->getResource();
		LLOG_ERROR("HanderUserInviteClubMember userId[%d] inviteeId[%d] logicId[%d]", this->GetUserDataId(), msg->m_ClubId, InviteeUser->getUserLogicID());

		if (InviteeUser->getUserLogicID() > 0)
		{
			send.m_ErrorCode = 2;
			this->Send(send.GetSendBuff());
		}
		else
		{
			this->Send(send.GetSendBuff());

			//转发邀请的消息
			LMsgS2CTransmitInviteClubMember inviteMsg;
			inviteMsg.m_ClubId = msg->m_ClubId;
			inviteMsg.m_ClubShowDeskId = msg->m_ClubShowDeskId;
			inviteMsg.m_clubDeskId = msg->m_clubDeskId;
			inviteMsg.m_InviterId = this->GetUserDataId();
			inviteMsg.m_InviterNike = this->m_userData.m_nike;
			inviteMsg.m_InviterPhoto = this->m_userData.m_headImageUrl;
			inviteMsg.m_state = msg->m_state;
			inviteMsg.m_playType = msg->m_playType;

			InviteeUser->Send(inviteMsg.GetSendBuff());

		}

	}
	else
	{
		LLOG_ERROR("HanderUserInviteClubMember userId[%d] inviteeId[%d] not in manager", this->GetUserDataId(), msg->m_InviteeId);
		send.m_ErrorCode = 1;
		this->Send(send.GetSendBuff());
	}

}

void User::HanderUserReplyClubMemberInvite(LMsgC2SReplyClubMemberInvite *msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("HanderUserReplyClubMemberInvite msg is null");
		return;
	}

	if (msg->m_ClubId == 0 || msg->m_InviterId == 0)
	{
		LLOG_ERROR("HanderUserReplyClubMemberInvite clubId == 0 ||m_InviteeId ==0 ");
		return;
	}

	LLOG_ERROR("HanderUserReplyClubMemberInvite club[%d] showDesk[%d] clubdesk[%d]",msg->m_ClubId,msg->m_ClubShowDeskId,msg->m_clubDeskId);

	if (msg->m_reply == 1)   //用户同意邀请
	{
		//拿到桌号
		Lint  deskId = msg->m_clubDeskId;     //gClubManager.getClubDeskByShowDeskId(msg->m_ClubId, msg->m_ClubShowDeskId);
		if (deskId <= 0)
		{
			LLOG_DEBUG("HanderUserReplyClubMemberInvite clubDeskId[%d] <=0", deskId);
			LMsgS2CReplyClubMemberInviteReply sendReply;
			sendReply.m_ErrorCode = 2;
			sendReply.m_ClubId = msg->m_ClubId;
			sendReply.m_ClubShowDeskId = msg->m_ClubShowDeskId;
			sendReply.m_InviterId = msg->m_InviterId;

			this->Send(sendReply.GetSendBuff());

			return;
		}

		LLOG_DEBUG("HanderUserReplyClubMemberInvite userId[%d] clubDeskId[%d] ", GetUserDataId(), deskId);

		LMsgS2CReplyClubMemberInviteReply sendReply;
		sendReply.m_ErrorCode = 0;
		sendReply.m_ClubId = msg->m_ClubId;
		sendReply.m_ClubShowDeskId = msg->m_ClubShowDeskId;
		sendReply.m_InviterId = msg->m_InviterId;

		this->Send(sendReply.GetSendBuff());



		LMsgC2SAddDesk addDesk;
		addDesk.m_deskId = deskId;
		addDesk.m_Gps_Lat = msg->m_Gps_Lat;
		addDesk.m_Gps_Lng = msg->m_Gps_Lng;

		this->HanderUserAddDesk(&addDesk);

	}
	else       //用户拒绝
	{
		//暂时不处理
	}

}

//void User::HanderUserSetClubAdministrtor(LMsgC2SSetClubAdministrtor* msg)
//{
//	if (msg == NULL)
//	{
//		LLOG_ERROR("HanderUserSetClubAdministrtor msg is null");
//		return;
//	}
//	LMsgS2CSetClubAdministrtor send;
//	send.m_ClubId = msg->m_ClubId;
//	send.m_type = msg->m_type;
//
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_ClubId);
//	if (safeClub.get() == NULL || !safeClub->isValid())
//	{
//		LLOG_ERROR("HanderUserSetClubAdministrtor,clubId=[%d]  is null or invalid", msg->m_ClubId);
//		send.m_error_code = 3;
//		Send(send.GetSendBuff());
//		return;
//	}
//	if (msg->m_type != 1 && msg->m_type != 2)
//	{
//		LLOG_ERROR("HanderUserSetClubAdministrtor,clubId=[%d]  is null or invalid", msg->m_ClubId);
//		send.m_error_code = 2;
//		Send(send.GetSendBuff());
//		return;
//	}
//	boost::shared_ptr<Club> club = safeClub->getResource();
//	send.m_error_code = club->setAdmin(msg->m_type, GetUserDataId(), msg->m_adminUserId);
//
//	LLOG_DEBUG("HanderUserSetClubAdministrtor, ERROR_CODE=%d", send.m_error_code);
//	Send(send.GetSendBuff());
//	return;
//}

// 俱乐部每个人能力列表
//void User::HanderUserPointList(LMsgC2SPowerPointList* msg)
//{
//	if (msg == NULL)
//	{
//		LLOG_ERROR("LMsgC2SPowerPointList msg is null");
//		return;
//	}
//
//	LLOG_DEBUG("LMsgC2SPowerPointList  userId[%d] get club[%d] searchid[%d]  point List",GetUserDataId(),msg->m_clubId, msg->m_searchId);
//
//	LMsgS2CPowerPointList pointList;
//	pointList.m_clubId = msg->m_clubId;
//
//	//判断是否是俱乐部成员
//	if (!gClubUserManager.isUserInClub(msg->m_clubId, GetUserDataId()))
//	{
//		pointList.m_errorCode = 3;
//		Send(pointList.GetSendBuff());
//		LLOG_ERROR("LMsgC2SPowerPointList  ERROR NOT MEMBER userId[%d] get club[%d]  point List", GetUserDataId(), msg->m_clubId);
//		return;
//	}
//
//	//权限
//	bool canPointList = false;
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub =gClubManager.getClubbyClubId(msg->m_clubId);
//	if (safeClub.get() && safeClub->isValid())
//	{
//		canPointList = safeClub->getResource()->canPowPointList(GetUserDataId());
//	}
//	if (!canPointList)
//	{
//		pointList.m_errorCode = 4;
//		Send(pointList.GetSendBuff());
//		LLOG_ERROR("LMsgC2SPowerPointList  ERROR CAN NOT SEE  LIST  userId[%d] get club[%d]  point List", GetUserDataId(), msg->m_clubId);
//		return;
//	}
//
//	//查询
//	std::set<boost::shared_ptr<ClubUser>  > clubUserList;
//	if (msg->m_searchId == 0)
//	{
//		clubUserList = gClubUserManager.getClubAllUser(msg->m_clubId);
//	}
//	else
//	{
//		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(msg->m_searchId);
//		if (safeUser && safeUser->isValid())
//		{
//			clubUserList.insert(safeUser->getResource());
//		}
//	}
//
//	for (auto ItClubUser = clubUserList.begin(); ItClubUser != clubUserList.end(); ItClubUser++)
//	{
//		if (ItClubUser->get() == NULL)continue;
//		UserPowerInfo temp; 
//		temp.m_userId = ItClubUser->get()->getID();
//		temp.m_nike = ItClubUser->get()->m_nike;
//		//temp.m_photo = ItClubUser->get()->m_headImageUrl;
//		temp.m_state = ItClubUser->get()->getClientState();
//		temp.m_point = ItClubUser->get()->getClubPoint(msg->m_clubId);
//		temp.m_type = ItClubUser->get()->getUserType(msg->m_clubId);
//		//打印
//		temp.print();
//		pointList.m_userList.push_back(temp);
//	}
//
//	pointList.m_count = pointList.m_userList.size();
//	pointList.m_HasSendAll = 1;
//
//	Send(pointList.GetSendBuff());
//
//	return;
//
//}

//void User::HanderUserChange(LMsgC2SChangePoint*msg)
//{
//	if (msg == NULL)
//	{
//		LLOG_ERROR("HanderUserChange msg is null");
//		return;
//	}
//
//	LLOG_ERROR("HanderUserChange  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]",GetUserDataId(),msg->m_type,msg->m_clubId,msg-//>m_userId,msg->m_point,msg->m_operR.c_str());
//
//	LMsgS2CChangePoint ChangePoint;
//	ChangePoint.m_clubId = msg->m_clubId;
//
//	//冲的能量值为负
//	if (msg->m_point <= 0 || (msg->m_type != CHANGE_POINT_TYPE_ADD && msg->m_type != CHANGE_POINT_TYPE_REDUCE))
//	{
//		ChangePoint.m_errorCode = 1;
//		Send(ChangePoint.GetSendBuff());
//		LLOG_ERROR("HanderUserChange ERROR type or point<=0 operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", GetUserDataId(), msg->m_type, msg-//>m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
//		return;
//	}
//
//	//权限
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_clubId);
//	if (safeClub.get() == NULL ||  !safeClub->isValid())
//	{
//		ChangePoint.m_errorCode = 2;
//		Send(ChangePoint.GetSendBuff());
//		LLOG_ERROR("HanderUserChange ERROR club null operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", GetUserDataId(), msg->m_type, msg-//>m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
//		return;
//	}
//	boost::shared_ptr<Club> club = safeClub->getResource();
//	if (!club->canChangePoint(GetUserDataId()))   //没有权限
//	{
//		ChangePoint.m_errorCode = 4;
//		Send(ChangePoint.GetSendBuff());
//		LLOG_ERROR("HanderUserChange ERROR  no  quanxian  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", GetUserDataId(), msg->m_type, msg-//>m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
//		return;
//	}
//
//	//是否是会员
//	if (!gClubUserManager.isUserInClub(msg->m_clubId, msg->m_userId))
//	{
//		ChangePoint.m_errorCode = 3;
//		Send(ChangePoint.GetSendBuff());
//		LLOG_ERROR("HanderUserChange ERROR user not in club  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", GetUserDataId(), msg->m_type, msg-//>m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
//		return;
//	}
//	if (!gClubUserManager.isUserInClub(msg->m_clubId, GetUserDataId()))
//	{
//		ChangePoint.m_errorCode = 3;
//		Send(ChangePoint.GetSendBuff());
//		LLOG_ERROR("HanderUserChange ERROR operId not in club  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", GetUserDataId(), msg->m_type, //msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
//		return;
//	}
//
//	//减分没有原因
//	if (msg->m_type == CHANGE_POINT_TYPE_REDUCE && msg->m_operR.empty())
//	{
//		ChangePoint.m_errorCode = 5;
//		Send(ChangePoint.GetSendBuff());
//		LLOG_ERROR("HanderUserChange ERROR no reson  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", GetUserDataId(), msg->m_type, msg-//>m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
//		return;
//	}
//	if (msg->m_type == CHANGE_POINT_TYPE_REDUCE)
//	{
//		Lint point = gClubUserManager.getPoint(msg->m_userId, msg->m_clubId);
//		if (point <= 0)
//		{
//			ChangePoint.m_errorCode = 7;
//			Send(ChangePoint.GetSendBuff());
//			LLOG_ERROR("HanderUserChange ERROR user point <=0  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", GetUserDataId(), msg->m_type, /msg-/>m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
//			return;
//		}
//	}
//
//
//	//发生center update insert 数据库,从center 返回相同的消息，再发给客户端
//	LMsgLMG2CEUpdatePointRecord  send;
//	UserPowerRecord  record(time(NULL), msg->m_userId,msg->m_type, msg->m_point, GetUserDataId());
//	record.m_operR = msg->m_operR;
//	record.m_clubId =club->getClubId();
//	record.m_clubSq = club->getClubSq();
//
//	send.m_record = record;
//	gWork.SendToCenter(send);
//
//	return;
//}


