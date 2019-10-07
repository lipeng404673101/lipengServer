#include "User.h"
#include "Work.h"
#include "RoomVip.h"
#include "UserManager.h"
#include "Config.h"
#include "LogicToggles.h"

User::User(LUser data, Lint gateId) : m_userData(data)
{
	m_gateId = gateId;
	m_online = true;
	m_desk = NULL;
	m_Active.clear();
	m_ip = "";
	m_userState = 0;
	m_videoPermission = 1;
	m_timeInRoom.SetSecs(0);
	m_gps_lng = m_gps_lat = 0.0f;
	get_gps_pair_values(data.m_customString2, m_gps_lng, m_gps_lat);
	LLOG_DEBUG("new User::User -- GPS userid:%d m_customString2:%s [%f,%f]", m_userData.m_id, data.m_customString2.c_str(), m_gps_lat, m_gps_lng);
}
	
User::~User()
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

bool User::Login()
{
	//这个方法要改，这样调用太LOW了
	bool b_login = false;
	m_userData.m_lastLoginTime = gWork.GetCurTime().Secs();
	m_online = true;

	if(GetDesk())
	{
		b_login = GetDesk()->OnUserReconnect(this);
	}
	return b_login;
}

void User::Logout()
{
	m_online = false;
}

void User::Send(LMsg& msg)
{
	LLOG_DEBUG( "User::Send userid=%d msgid=%d", m_userData.m_id, msg.m_msgId );
	GateInfo* info = gWork.GetGateInfoById(m_gateId);
	if (info)
	{
		LLOG_DEBUG("if true User::Send userid=%d msgid=%d", m_userData.m_id, msg.m_msgId);

		LMsgL2GUserMsg send;
		send.m_strUUID = m_userData.m_unioid;
		send.m_dataBuff = msg.GetSendBuff();
		info->m_sp->Send(send.GetSendBuff());
	}
}

void User::SendLoginInfo(Lstring& buyInfo, Lint hide)
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
	send.m_state = GetDesk() ? 1 : 0;
	send.m_ip = m_ip;
	send.m_face = m_userData.m_headImageUrl;
	send.m_buyInfo = buyInfo;
	send.m_hide = hide;
	send.m_gm = m_userData.m_gm;

	send.m_card1 = 0;
	send.m_card2 = 0;
	send.m_card3 = 0;

	//俱乐部
	if (m_desk)
	{
		send.m_card2=m_desk->MHGetClubId();
		send.m_card3=m_desk->MHGetPlayTypeId();
	}

	Send(send);
	LLOG_ERROR("***返回用戶登錄 msgid:%d m_id:%d m_ip:%s m_new:%d", send.m_msgId, send.m_id, m_ip.c_str(), send.m_new);
}

void User::SendItemInfo()
{
	LMsgS2CItemInfo send;
	send.m_card1 = m_userData.m_numOfCard1s;
	send.m_card2 = m_userData.m_numOfCard2s;
	send.m_card3 = m_userData.m_numOfCard3s;
	send.m_coins = m_userData.m_coins;
	Send(send);
}

void User::SetDesk(Desk* desk)
{
	m_desk = desk;
}

Desk* User::GetDesk()
{
	return m_desk;
}

void User::HanderMsg(LMsg* msg)
{
	LLOG_DEBUG( "User::HanderMsg userid=%d msgid=%d", m_userData.m_id, msg->m_msgId );
	switch (msg->m_msgId)
	{
	case MSG_C_2_S_START_GAME:
		LLOG_DEBUG("MSG_C_2_S_START_GAME");
		HanderUserStartGameButton((LMsgC2SStartGame*)msg);
		break;
	case MSG_C_2_S_USER_SELECT_SEAT:
		LLOG_DEBUG("User::HanderUserSelectSeat");
		HanderUserSelectSeat((LMsgC2SUserSelectSeat*)msg);
		break;

	case MSG_C_2_S_READY:
		HanderUserReady((LMsgC2SUserReady*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM:
		HanderResutDesk((LMsgC2SResetDesk*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM_SELECT:
		HanderSelectResutDesk((LMsgC2SSelectResetDesk*) msg);
		break;
	case MSG_C_2_S_LEAVE_ROOM:
		HanderUserLeaveDesk((LMsgC2SLeaveDesk*) msg);
		break;
	case MSG_C_2_S_PLAY_CARD:
		HanderUserPlayCard((LMsgC2SUserPlay*) msg);
		break;
	case MSG_C_2_S_USER_OPER:
		HanderUserThink((LMsgC2SUserOper*) msg);
		break;
	case MSG_C_2_S_START_HU_SELECT:
		HanderStartHuSelect((LMsgC2SUserStartHuSelect*) msg);
		break;
	case MSG_C_2_S_END_SELECT:
		HanderEndCardSelect((LMsgC2SUserEndCardSelect*) msg);
		break;
	case MSG_C_2_S_USER_SPEAK:
		HanderUserSpeak((LMsgC2SUserSpeak*) msg);
		break;
	case MSG_C_2_S_USER_CHANGE:
		HanderUserChange((LMsgC2SUserChange*) msg);
		break;
	case MSG_C_2_S_TANGCARD:
		HanderUserTangReq((LMsgC2STangCard*) msg);
		break;
	case MSG_C_2_S_USER_AIOPER:
		HanderUserAIOper((LMsgC2SUserAIOper*) msg);
		break;
	case MSG_C_2_S_SEND_VIDEO_INVITATION:
		HanderSendVideoInvitation((LMsgC2SSendVideoInvitation*) msg);
		break;

	case MSG_C_2_S_INBUSY_VIDEO_INVITATION:
		HanderInbusyVideoInvitation((LMsgC2SReceiveVideoInvitation*) msg);
		break;

	case MSG_C_2_S_ONLINE_VIDEO_INVITATION:
		HanderOnlineVideoInvitation((LMsgC2SOnlineVideoInvitation*) msg);
		break;

	case MSG_C_2_S_SHUTDOWN_VIDEO_INVITATION:
		HanderShutDownVideoInvitation((LMsgC2SShutDownVideoInvitation*) msg);
		break;

	case MSG_C_2_S_UPLOAD_GPS_INFORMATION:
		HanderUploadGpsInformation((LMsgC2SUploadGPSInformation*) msg);
		break;

	case MSG_C_2_S_UPLOAD_VIDEO_PERMISSION:
		HanderUploadVideoPermission((LMsgC2SUploadVideoPermission*) msg);
		break;

	default:
		LLOG_ERROR("LogicServer Message Error: %d", msg->m_msgId);
		break;
	}
}

Lint User::HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg)
{
	return gRoomVip.CreateVipDesk(msg, this);
}

Lint User::HanderUserCreateDeskForOther(LMsgLMG2LCreateDeskForOther* msg)
{
	return gRoomVip.CreateVipDeskForOther(msg, this);
}

//Lint User::HanderUserCreateDeskForClub(LMsgLMG2LCreateDeskForClub* msg)
//{
//	return gRoomVip.CreateVipDeskForClub(msg, this);
//}

Lint User::HanderUserAddDesk(Lint nDeskID)
{
	return gRoomVip.AddToVipDesk(this, nDeskID);
}


Lint User::HanderUserAddDesk(LMsgLMG2LAddToDesk* msg)
{
	return gRoomVip.AddToVipDesk(this, msg);
}

Lint	 User::HanderUserAddClubDesk(LMsgLMG2LEnterClubDesk* msg)
{
	return gRoomVip.AddToVipClubDesk( msg,this);
}


void User::HanderUserSelectSeat(LMsgC2SUserSelectSeat*msg)
{
	//return gRoomVip.AddToVipDesk(this, nDeskID);

	if (m_desk)
	{
		m_desk->HanderSelectSeat(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserSelectSeat no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserStartGameButton(LMsgC2SStartGame*msg)
{
	if (m_desk)
	{
		m_desk->MHHanderStartGameButton(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserStartGameButton no desk, userid=%d", m_userData.m_id);
	}
}


void User::HanderUserReady(LMsgC2SUserReady*msg)
{
	if (m_desk)
	{
		m_desk->HanderUserReady(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserReady no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderResutDesk(LMsgC2SResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderResutDesk(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderResutDesk no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderSelectResutDesk(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderSelectResutDesk no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg)
{
	gRoomVip.LeaveToVipDesk(msg,this);
}

void User::HanderUserPlayCard(LMsgC2SUserPlay* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserPlayCard(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserPlayCard no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserThink(LMsgC2SUserOper* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserOperCard(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserThink no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserStartHu(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderStartHuSelect no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserEndSelect(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderEndCardSelect no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserSpeak(LMsgC2SUserSpeak* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserSpeak(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserSpeak no desk, userid=%d", m_userData.m_id);
	}
}

// 玩家换牌
void User::HanderUserChange(LMsgC2SUserChange* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserChange(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserChange no desk, userid=%d", m_userData.m_id);
	}
}

Lstring User::GetIp()
{
	return m_ip;
}

void User::SetIp(Lstring& ip)
{
	m_ip = ip;
}

void User::AddCardCount(Lint cardType, Lint count, Lint operType,Lstring admin, bool bNeedSave)
{
	LLOG_INFO("User::AddCardCount type=%d,count=%d,operType=%d", cardType, count, operType);
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

	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 1;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID = m_userData.m_unioid;
	gWork.SendToLogicManager(msg);
}

void User::DelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin, Lint feeType)
{
	LLOG_INFO("User::DelCardCount type=%d,count=%d,operType=%d,FORCE_ROOM_CARD_FREE=%d", cardType, count, operType, FORCE_ROOM_CARD_FREE);
	if (FORCE_ROOM_CARD_FREE) {
		return;
	}

	if (m_userData.m_numOfCard2s - count >= 0)
		m_userData.m_numOfCard2s = m_userData.m_numOfCard2s - count;
	else
		m_userData.m_numOfCard2s = 0;


	LLOG_DEBUG("User::DelCardCount m_userData.m_id = %d  m_userData.m_numOfCard2s = %d", m_userData.m_id, m_userData.m_numOfCard2s);
	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 0;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID = m_userData.m_unioid;

	gWork.SendToLogicManager(msg);
}

void User::ModifyUserState(bool bLeaveServer)
{
	LLOG_DEBUG("User::ModifyUserState ");
	LMsgL2GModifyUserState msg_gate;
	msg_gate.m_strUUID = m_userData.m_unioid;
	msg_gate.m_logicID =  bLeaveServer? 0 : gConfig.GetServerID();
	msg_gate.m_userstate = getUserState();
	GateInfo* gateInfo = gWork.GetGateInfoById(GetUserGateId());

	if (gateInfo)
	{
		gateInfo->m_sp->Send(msg_gate.GetSendBuff());
	}

	LMsgL2LMGModifyUserState msg_logic;
	msg_logic.m_strUUID = m_userData.m_unioid; 
	msg_logic.m_userid  = m_userData.m_id;
	msg_logic.m_logicID = bLeaveServer? 0 : gConfig.GetServerID();
	msg_logic.m_userstate = getUserState();

	gWork.SendToLogicManager(msg_logic);
	gWork.SendToCoinsServer(msg_logic);
}

void User::AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType,Lstring admin)
{
	LLOG_INFO("User::AddCardCount type=%d,count=%d,operType=%d", cardType, count, operType);
	
	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 1;
	msg.operType = operType;
	msg.m_userid = id;
	msg.m_strUUID = strUUID;
	gWork.SendToLogicManager(msg);
}

void User::HanderUserTangReq( LMsgC2STangCard* msg )
{
	if(m_desk)
		m_desk->HanderUserTangReq(this,msg);
}

void User::HanderUserAIOper(LMsgC2SUserAIOper* msg)
{
	if(m_desk)
	{
		Lint pos = m_desk->GetUserPos( this );
		if(pos != INVAILD_POS)
		{
			if ( !m_desk->m_autoPlay[pos] )
			{
				LLOG_ERROR("HanderUserAIOper user not have action %s", m_userData.m_nike.c_str());
			}
			else
			{
				m_desk->m_autoPlayTime[pos] = 0;	// 改写记录的倒计时为0
				m_desk->CheckAutoPlayCard();
			}
		}
		else
		{
			LLOG_INFO("HanderUserAIOper pos error %s", m_userData.m_nike.c_str());
		}
	}
}

void User::HanderSendVideoInvitation(LMsgC2SSendVideoInvitation* msg)
{
	if (!m_desk)
		return;
	LMsgC2SReceiveVideoInvitation send;
	send.m_userId = msg->m_userId;
	send.m_strUUID = msg->m_strUUID;
	send.m_reqUserId = msg->m_reqUserId;
	for (size_t i = 0; i < 4; i++)
	{
		if (m_desk->m_user[i] == NULL)
			continue;

		if ( m_desk->m_user[i]->m_userData.m_id == msg->m_userId) {
			m_desk->m_user[i]->Send(send);
			break;
		}
	}

}


void User::HanderInbusyVideoInvitation(LMsgC2SReceiveVideoInvitation* msg)
{
	if (!m_desk)
		return;
	for (size_t i = 0; i < 4; i++)
	{
		if (m_desk->m_user[i] == NULL)
			continue;

		if ( m_desk->m_user[i]->m_userData.m_id == msg->m_reqUserId) {
			m_desk->m_user[i]->Send(*msg);
			break;
		}
	}
}

void User::HanderOnlineVideoInvitation(LMsgC2SOnlineVideoInvitation* msg)
{
	if (!m_desk)
		return;
	for (size_t i = 0; i < 4; i++)
	{
		if (m_desk->m_user[i] && m_desk->m_user[i]->m_userData.m_id == msg->m_reqUserId) {
			m_desk->m_user[i]->Send(*msg);
		}
	}
}

void User::HanderShutDownVideoInvitation(LMsgC2SShutDownVideoInvitation* msg)
{
	if (!m_desk)
		return;
	for (size_t i = 0; i < 4; i++)
	{
		if (m_desk->m_user[i] && m_desk->m_user[i]->m_userData.m_id == msg->m_userId) {
			m_desk->m_user[i]->Send(*msg);
			break;
		}
	}
}

void User::HanderUploadGpsInformation(LMsgC2SUploadGPSInformation* msg)
{
	if (!m_desk)
		return;
	LLOG_ERROR("desk:%d User[%d] upload GPS info %s", m_desk->GetDeskId(), GetUserDataId(), msg->m_strGPS.c_str());
	LMsgS2CUpdateUserVipInfo send;
	send.m_userId = msg->m_userId;
	send.m_strGPS = msg->m_strGPS;
	send.m_VideoPermit = m_videoPermission;
	for (size_t i = 0; i < 4; i++)
	{
		if (m_desk->m_user[i] && m_desk->m_user[i]->m_userData.m_id != msg->m_userId) {
			m_desk->m_user[i]->Send(send);
			break;
		}
	}

	m_userData.m_customString2 = msg->m_strGPS;

	LMsgL2LMGUpdateUserGPS sendToCe;
	sendToCe.m_userid = msg->m_userId;
	sendToCe.m_Gps = msg->m_strGPS;
	sendToCe.m_strUUID = msg->m_strUUID;
	
	//这里暂时不修改GPS信息
	//gWork.SendToLogicManager(sendToCe);
}

void User::HanderUploadVideoPermission(LMsgC2SUploadVideoPermission* msg)
{
	if (!m_desk)
		return;
	LLOG_ERROR("desk:%d User[%d] upload VideoPermission info %d", m_desk->GetDeskId(), GetUserDataId(), msg->m_VideoPermit);
	LMsgS2CUpdateUserVipInfo send;
	send.m_userId = msg->m_userId;
	send.m_strGPS = m_userData.m_customString2;
	m_videoPermission = msg->m_VideoPermit;
	send.m_VideoPermit = m_videoPermission;
	for (size_t i = 0; i < 4; i++)
	{
		if (m_desk->m_user[i] && m_desk->m_user[i]->m_userData.m_id != msg->m_userId) {
			m_desk->m_user[i]->Send(send);
		}
	}
}

bool User::IsInLimitGPSPosition(User & u)
{
	if (u.GetUserDataId() == m_userData.m_id)
	{
		return false;
	}
	double distance = calc_gps_distance(m_gps_lat, m_gps_lng, u.m_gps_lat, u.m_gps_lng);
	LLOG_DEBUG("***calc_gps_distance:[%d: %f,%f]-[%d: %f,%f] distance:%f", m_userData.m_id, m_gps_lat, m_gps_lng, u.GetUserDataId(), u.m_gps_lat, u.m_gps_lng, distance);
	return distance <= _MH_GPS_DISTANCE_LIMIT;
}

