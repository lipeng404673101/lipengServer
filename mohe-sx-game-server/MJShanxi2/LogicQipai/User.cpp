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
	m_isMedal = 0;
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

	// 62066：观战玩家请求入座
	case MSG_C_2_S_ADD_ROOM_SEAT_DOWN:
		HanderLookOnUserSeatDown((LMsgG2LLookOnUserSearDown*)msg);
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
	//能量不足玩家充上能量后从新点击入座 61184
	case MSG_C_2_S_STAND_PLAYER_RESEAT:
		LLOG_DEBUG("MSG_C_2_S_STAND_PLAYER_RESEAT(61184)");
		HanderStandPlayerReSeat((LMsgC2SStandPlayerReSeat*)msg);
		break;
	case MSG_C_2_S_POKER_GAME_MESSAGE:
		LLOG_DEBUG("MSG_C_2_S_POKER_GAME_MESSAGE");
		HanderPokerGameMessage((LMsgC2SGameCommon*)msg);
		break;
	case MSG_C_2_S_VIEW_LOOKON_LIST:
		LLOG_DEBUG("MSG_C_2_S_VIEW_LOOKON_LIST");
		HanderViewLookOnList((LMsgC2SViewLookOnList*)msg);
		break;
	case MSG_C_2_S_YINGSANZHANG_ADD_SCORE:
		LLOG_DEBUG("MSG_C_2_S_YINGSANZHANG_ADD_SCORE");
		HanderYingSanZhangAddScore((YingSanZhangC2SAddScore*)msg);
		break;

	//三张牌：客户端发给服务器玩家买大买小操作结果
	case MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL:
		LLOG_DEBUG("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL");
		HanderYingSanZhangUserSelectBigSmall((YingSanZhangC2SUserSelectBigSmall*)msg);
		break;

	case MSG_C_2_S_TUITONGZI_SELECT_ZHUANG:
		LLOG_DEBUG("MSG_C_2_S_TUITONGZI_SELECT_ZHUANG");
		HanderTuiTongZiSelectZhuang((TuiTongZiC2SSelectZhuang*)msg);
		break;

	case MSG_C_2_S_TUITONGZI_DO_SHAIZI:
		LLOG_DEBUG("MSG_C_2_S_TUITONGZI_DO_SHAIZI");
		HanderTuiTongZiDoShaiZi((TuiTongZiC2SDoShaiZi*)msg);
		break;

	case MSG_C_2_S_TUITONGZI_ADD_SCROE:
		LLOG_DEBUG("MSG_C_2_S_TUITONGZI_ADD_SCROE");
		HanderTuiTongZiAddScore((TuiTongZiC2SScore*)msg);
		break;

	case MSG_C_2_S_TUITONGZI_OPEN_CARD:
		LLOG_DEBUG("MSG_C_2_S_TUITONGZI_OPEN_CARD");
		HanderTuiTongOpenCard((TuiTongZiC2SOpenCard*)msg);
		break;
	//牛牛选庄
	case MSG_C_2_S_NIUNIU_SELECT_ZHUANG:
		LLOG_DEBUG("MSG_C_2_S_NIUNIU_SELECT_ZHUANG");
		HanderNiuNiuSelectZhuang((NiuNiuC2SSelectZhuang*) msg);
		break;
	//牛牛买码
	case MSG_C_2_S_NIUNIU_MAI_MA:
		LLOG_DEBUG("MSG_C_2_S_NIUNIU_MAI_MA");
		HanderNiuNiuMaiMa((NiuNiuC2SMaiMa*) msg);
		break;
	//牛牛下注
	case MSG_C_2_S_NIUNIU_ADD_SCORE:
		LLOG_DEBUG("MSG_C_2_S_NIUNIU_ADD_SCROE");
		HanderNiuNiuAddScore((NiuNiuC2SAddScore*) msg);
		break;
	//牛牛亮牌
	case MSG_C_2_S_NIUNIU_OPEN_CARD:
		LLOG_DEBUG("MSG_C_2_S_NIUNIU_OPEN_CARD");
		HanderNiuNiuOpenCard((NiuNiuC2SOpenCard*) msg);
		break;
	case MSG_C_2_S_DOUDIZHU_GAME_MESSAGE_BASE:
		HanderDouDiZhuGameMessage((MHLMsgDouDiZhuC2SGameMessage*)msg);
		break;
	//牛牛：玩家搓公牌
	case MSG_C_2_S_NIUNIU_CUO_GONG_PAI:
		HandNiuNiuCuoGongPai((NiuNiuC2SCuoGongPai*)msg);
		break;
	//牛牛托管
	case MSG_C_2_S_NIUNIU_TUO_GUAN:
		HandNiuNiuTuoGuan((NiuNiuC2STuoGuan*)msg);
		break;
	//牛牛取消托管
	case MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN:
		HandNiuNiuCancelTuoGuan((NiuNiuC2SCancelTuoGuan*) msg);
		break;
	//客户端通知服务器开始计时MSG_C_2_S_START_TIME = 61198
	case MSG_C_2_S_START_TIME:
		HandNiuNiuStartTime((NiuNiuC2SStartTime*)msg);
		break;

	//双升：C->S 玩家报主操作
	case MSG_C_2_S_SHUANGSHENG_SELECT_ZHU:
		LLOG_DEBUG("MSG_C_2_S_SHUANGSHENG_SELECT_ZHU");
		HandleShuangShengSelectZhu((ShuangShengC2SSelectZhu*)msg);
		break;
	//双升：C->S 玩家反主操作
	case MSG_C_2_S_SHUANGSHENG_FAN_ZHU:
		LLOG_DEBUG("MSG_C_2_S_SHUANGSHENG_FAN_ZHU");
		HandleShangShengFanZhu((ShuangShengC2SFanZhu*) msg);
		break;
	//双升：C->S 玩家盖底牌操作
	case MSG_C_2_S_SHUANGSHENG_BASE_CARDS:
		LLOG_DEBUG("MSG_C_2_S_SHUANGSHENG_BASE_CARDS");
		HandleShuangShengBaseCards((ShuangShengC2SBaseCards*) msg);
		break;
	//双升：C->S 玩家出牌操作
	case MSG_C_2_S_SHUANGSHENG_OUT_CARDS:
		LLOG_DEBUG("MSG_C_2_S_SHUANGSHENG_OUT_CARDS");
		HandleShuangShengOutCards((ShuangShengC2SOutCards*) msg);
		break;
	//3打2：C->S 玩家选择叫牌分 MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
	case MSG_C_2_S_SANDAER_SELECT_SCORE:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_SELECT_SCORE");
		HandleSanDaErSelectScore((SanDaErC2SSelectScore*)msg);
		break;
	//3打2：C->S 玩家选主牌 MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
	case MSG_C_2_S_SANDAER_SELECT_MAIN:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_SELECT_MAIN");
		HandleSanDaErSelectMain((SanDaErC2SSelectMain*)msg);
		break;
	//3打2：C->S 玩家埋底 MSG_C_2_S_SANDAER_BASE_CARD = 62257
	case MSG_C_2_S_SANDAER_BASE_CARD:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_BASE_CARD");
		HandleSanDaErBaseCard((SanDaErC2SBaseCard*)msg);
		break;
	//3打2：C->S 玩家选对家牌 MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
	case MSG_C_2_S_SANDAER_SELECT_FRIEND:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_SELECT_FRIEND");
		HandleSanDaErSelectFriend((SanDaErC2SSelectFriend*)msg);
		break;
	//3打2：C->S 玩家出牌 MSG_C_2_S_SANDAER_OUT_CARD = 62261
	case MSG_C_2_S_SANDAER_OUT_CARD:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_OUT_CARD");
		HandleSanDaErOutCard((SanDaErC2SOutCard*)msg);
		break;
	//3打2：玩家回复闲家得分慢105分,是否停止游戏  MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
	case MSG_C_2_S_SANDAER_SCORE_105_RET:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_SCORE_105_RET");
		HandleSanDaErScore105Ret((SanDaErC2SScore105Ret*)msg);
	//3打2：C->S 客户端庄家选择交牌
	case MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI");
		HandleSanDaErZhuangRequestJiaoPai((SanDaErC2SZhuangJiaoPai*)msg);
		break;
	//3打2：C->S 非庄家  客户端选择庄家交牌的申请
	case MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI:
		LLOG_DEBUG("MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI");
		HandleSanDaErXianSelectJiaoPai((SanDaErC2SXianSelectJiaoPai*)msg);
		break;

	//3打1：C->S 玩家选择叫牌分 MSG_C_2_S_SANDAYI_SELECT_SCORE = 62303
	case MSG_C_2_S_SANDAYI_SELECT_SCORE:
		LLOG_DEBUG("MSG_C_2_S_SANDAYI_SELECT_SCORE");
		HandleSanDaYiSelectScore((SanDaYiC2SSelectScore*) msg);
		break;
	//3打1：C->S 玩家选主牌 MSG_C_2_S_SANDAYI_SELECT_MAIN = 62305
	case MSG_C_2_S_SANDAYI_SELECT_MAIN:
		LLOG_DEBUG("MSG_C_2_S_SANDAYI_SELECT_MAIN");
		HandleSanDaYiSelectMain((SanDaYiC2SSelectMain*) msg);
		break;
	//3打1：C->S 玩家埋底 MSG_C_2_S_SANDAYI_BASE_CARD = 62307
	case MSG_C_2_S_SANDAYI_BASE_CARD:
		LLOG_DEBUG("MSG_C_2_S_SANDAYI_BASE_CARD");
		HandleSanDaYiBaseCard((SanDaYiC2SBaseCard*) msg);
		break;
	//3打1：C->S 玩家出牌 MSG_C_2_S_SANDAYI_OUT_CARD = 62309
	case MSG_C_2_S_SANDAYI_OUT_CARD:
		LLOG_DEBUG("MSG_C_2_S_SANDAYI_OUT_CARD");
		HandleSanDaYiOutCard((SanDaYiC2SOutCard*) msg);
		break;
	//3打1：玩家回复闲家得分慢105分,是否停止游戏  MSG_C_2_S_SANDAYI_SCORE_105_RET = 62314,
	case MSG_C_2_S_SANDAYI_SCORE_105_RET:
		LLOG_DEBUG("MSG_C_2_S_SANDAYI_SCORE_105_RET");
		HandleSanDaYiScore105Ret((SanDaYiC2SScore105Ret*) msg);
		break;


	//五人百分：C->S 玩家选择叫牌分 MSG_C_2_S_SANDAER_SELECT_SCORE = 62353
	case MSG_C_2_S_WURENBAIFEN_SELECT_SCORE:
		LLOG_DEBUG("MSG_C_2_S_WURENBAIFEN_SELECT_SCORE");
		HandleWuRenBaiFenSelectScore((WuRenBaiFenC2SSelectScore*)msg);
		break;
	//五人百分：C->S 玩家选主牌 MSG_C_2_S_SANDAER_SELECT_MAIN = 62355
	case MSG_C_2_S_WURENBAIFEN_SELECT_MAIN:
		LLOG_DEBUG("MSG_C_2_S_WURENBAIFEN_SELECT_MAIN");
		HandleWuRenBaiFenSelectMain((WuRenBaiFenC2SSelectMain*) msg);
		break;
	//五人百分：C->S 玩家埋底 MSG_C_2_S_SANDAER_BASE_CARD = 62357
	case MSG_C_2_S_WURENBAIFEN_BASE_CARD:
		LLOG_DEBUG("MSG_C_2_S_WURENBAIFEN_BASE_CARD");
		HandleWuRenBaiFenBaseCard((WuRenBaiFenC2SBaseCard*) msg);
		break;
	//五人百分：C->S 玩家选对家牌 MSG_C_2_S_SANDAER_SELECT_FRIEND = 62359
	case MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND:
		LLOG_DEBUG("MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND");
		HandleWuRenBaiFenSelectFriend((WuRenBaiFenC2SSelectFriend*) msg);
		break;
	//五人百分：C->S 玩家出牌 MSG_C_2_S_SANDAER_OUT_CARD = 62361
	case MSG_C_2_S_WURENBAIFEN_OUT_CARD:
		LLOG_DEBUG("MSG_C_2_S_WURENBAIFEN_OUT_CARD");
		HandleWuRenBaiFenOutCard((WuRenBaiFenC2SOutCard*) msg);
		break;
	//五人百分：玩家回复闲家得分慢105分,是否停止游戏  MSG_C_2_S_SANDAER_SCORE_105_RET = 62366,
	case MSG_C_2_S_WURENBAIFEN_SCORE_105_RET:
		LLOG_DEBUG("MSG_C_2_S_WURENBAIFEN_SCORE_105_RET");
		HandleWuRenBaiFenScore105Ret((WuRenBaiFenC2SScore105Ret*) msg);
		break;
	case MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI:
		this->HandleWuRenBaiFenZhuangJiaoPai((WuRenBaiFenC2SZhuangJiaoPai*)msg);
		break;
	case MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI:
		this->HandleWuRenBaiFenXianAckJiaoPai((WuRenBaiFenC2SXianSelectJiaoPai*)msg);
		break;
	//扯炫：C->S 处理玩家起钵钵， MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
	case MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO:
		this->HandlerPlayerSelectBoBo((CheXuanC2SPlayerSelectBoBo*)msg);
		break;
	//扯炫：C->S 处理玩家选择操作 MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
	case MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT:
		this->HandlerPlayerSelectOpt((CheXuanC2SPlayerSelectOpt*)msg);
		break;
	//扯炫：C->S 处理玩家扯牌操作 MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
	case MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI:
		this->HandlerPlayerChePai((CheXuanC2SPlayerChePai*)msg);
		break;
	//扯炫：C->S 处理玩家搓牌 MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
	case MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI:
		this->HandlerPlayerCuoPai((CheXuanC2SPlayerCuoPai*)msg);
		break;
	//扯炫：C->S 客户端牌局内请求战绩 MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
	case MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD:
		HandlerPlayerRequestRecord((CheXuanC2SPlayerRequestRecord*)msg);
		break;
	//扯炫：C->S 客户端操作默分 MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
	case MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN:
		HandlerPlayerOptMoFen((CheXuanC2SPlayerOptMoFen*)msg);
		break;
	//扯炫：C->S 客户端选择自动操作 MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
	case MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT:
	{
		HandlerPlayerSelectAutoOpt((CheXuanC2SPlayerSelectAutoOpt*)msg);
		break;
	}
	//C->S 客户端选择站起或坐下 MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
	case MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT:
	{
		HandlerPlayerStandOrSeat((CheXuanC2SPlayerSelectStandOrSeat*)msg);
		break;
	}
	

	//房间申请其他用户gps
	case MSG_C_2_S_REQUEST_USER_GPS:
		LLOG_DEBUG("MSG_C_2_S_REQUEST_USER_GPS");
		//HanderUserRequestGPS((LMsgC2SRequestUserGps*) msg);
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

//处理观战玩家入座
void User::HanderLookOnUserSeatDown(LMsgG2LLookOnUserSearDown* msg)
{
	if (m_desk)
	{
		m_desk->OnUserInRoomLookOnSeatDown(this);
	}
	else
	{
		//LLOG_ERROR("User::Hander Look On User Seat Down Error... ,deskId=[%d] userId=[%d]", m_desk->GetDeskId(), m_userData.m_id);
		LLOG_ERROR("User::Hander Look On User Seat Down Error...  userId=[%d] m_desk==NULL", m_userData.m_id);
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
	LLOG_DEBUG("User::ModifyUserState userid[%d] uuid[%s] ",m_userData.m_id,m_userData.m_unioid.c_str());
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
		if(pos != INVAILD_POS_QIPAI)
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
	for (size_t i = 0; i < MAX_CHAIR_COUNT; i++)
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
	for (size_t i = 0; i < MAX_CHAIR_COUNT; i++)
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
	for (size_t i = 0; i < MAX_CHAIR_COUNT; i++)
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
	for (size_t i = 0; i < MAX_CHAIR_COUNT; i++)
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
	LMsgS2CUpdateUserVipInfo send;
	send.m_userId = msg->m_userId;
	send.m_strGPS = msg->m_strGPS;
	send.m_VideoPermit = m_videoPermission;
	for (size_t i = 0; i < MAX_CHAIR_COUNT; i++)
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
	
	//gWork.SendToLogicManager(sendToCe);
}

void User::HanderUploadVideoPermission(LMsgC2SUploadVideoPermission* msg)
{
	if (!m_desk)
		return;
	LMsgS2CUpdateUserVipInfo send;
	send.m_userId = msg->m_userId;
	send.m_strGPS = m_userData.m_customString2;
	m_videoPermission = msg->m_VideoPermit;
	send.m_VideoPermit = m_videoPermission;
	for (size_t i = 0; i < MAX_CHAIR_COUNT; i++)
	{
		if (m_desk->m_user[i] && m_desk->m_user[i]->m_userData.m_id != msg->m_userId) {
			m_desk->m_user[i]->Send(send);
		}
	}
}

//更新玩家金币数量
void User::HanderUpdateUserCoins(LMsgLMG2LUpdatePointCoins* msg)
{
	LLOG_DEBUG("User::HanderUpdateUserCoins");
	if (msg == NULL)return;
	if (m_desk)
	{
		m_desk->HanderUpdateUserCoins(this, msg);
	}
	else
	{
		/*LMsgS2CUpdateCoins send;
		send.m_coins = this->m_userData.m_coins;
		send.m_userId = msg->m_userId;
		this->Send(send);*/
	}
}

//能量不足玩家充上能量后从新点击入座 61187
void User::HanderStandPlayerReSeat(LMsgC2SStandPlayerReSeat* msg)
{
	LLOG_DEBUG("User::HanderStandPlayerReSeat");
	if (msg == NULL)return;
	if (m_desk)
	{
		m_desk->HanderStandPlayerReSeat(this, msg);
	}
}

//玩家请求观战玩家列表信息
void User::HanderViewLookOnList(LMsgC2SViewLookOnList* msg)
{
	LLOG_DEBUG("User::HanderViewLookOnList");
	if (msg == NULL)return;
	if (m_desk)
	{
		m_desk->HanderViewLookOnList(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderViewLookOnList no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderPokerGameMessage(LMsgC2SGameCommon *msg)
{
	LLOG_DEBUG("User::HanderPokerGameMessage");
	if (msg==NULL)return;
	if (m_desk)
	{
		m_desk->HanderPokerGameMessage(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderPokerGameMessage no desk, userid=%d", m_userData.m_id);
	}

}

void User::HanderYingSanZhangAddScore(YingSanZhangC2SAddScore*msg)
{
	LLOG_DEBUG("User::HanderPokerGameMessage");
	if (msg==NULL)return;
	if (m_desk)
	{
		m_desk->HanderYingSanZhangAddScore(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderYingSanZhangAddScore no desk, userid=%d", m_userData.m_id);
	}

}

void User::HanderYingSanZhangUserSelectBigSmall(YingSanZhangC2SUserSelectBigSmall* msg)
{
	LLOG_DEBUG("User::YingSanZhangC2SUserSelectBigSmall");
	if (msg == NULL)return;
	if (m_desk)
	{
		m_desk->HanderYingSanZhangUserSelectBigSmall(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderYingSanZhangUserSelectBigSmall no desk, userid=%d", m_userData.m_id);
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

void User::HanderTuiTongZiAddScore(TuiTongZiC2SScore*msg)
{
	LLOG_DEBUG("User::HanderTuiTongZiAddScore");
	if (msg==NULL)return;
	if (m_desk)
	{
		m_desk->HanderTuiTongZiAddScore(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderTuiTongZiAddScore no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderTuiTongZiSelectZhuang(TuiTongZiC2SSelectZhuang*msg)
{
	LLOG_DEBUG("User::HanderTuiTongZiSelectZhuang");
	if (msg==NULL)return;
	if (m_desk)
	{
		m_desk->HanderTuiTongZiSelectZhuang(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderTuiTongZiSelectZhuang no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderTuiTongZiDoShaiZi(TuiTongZiC2SDoShaiZi*msg)
{
	LLOG_DEBUG("User::HanderTuiTongZiDoShaiZi");
	if (msg==NULL)return;
	if (m_desk)
	{
		m_desk->HanderTuiTongZiDoShaiZi(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderTuiTongZiDoShaiZi no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderTuiTongOpenCard(TuiTongZiC2SOpenCard*msg)
{
	LLOG_DEBUG("User::HanderTuiTongOpenCard");
	if (msg==NULL)return;
	if (m_desk)
	{
		m_desk->HanderTuiTongOpenCard(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderTuiTongZiDoShaiZi no desk, userid=%d", m_userData.m_id);
	}
}



/* 牛牛：处理玩家选庄 */
void User::HanderNiuNiuSelectZhuang(NiuNiuC2SSelectZhuang* msg)
{
	LLOG_DEBUG("User::HanderNiuNiuSelectZhuang");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HanderNiuNiuSelectZhuang(this, msg);
	}
}

/* 牛牛：处理玩家买码 */
void User::HanderNiuNiuMaiMa(NiuNiuC2SMaiMa* msg)
{
	LLOG_DEBUG("User::HanderNiuNiuMaiMa");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HanderNiuNiuMaiMa(this, msg);
	}
}

/* 牛牛：处理玩家下注 */
void User::HanderNiuNiuAddScore(NiuNiuC2SAddScore* msg)
{
	LLOG_DEBUG("User::HanderNiuNiuAddScore");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HanderNiuNiuAddScore(this, msg);
	}
}

/* 牛牛：处理玩家亮牌 */
void User::HanderNiuNiuOpenCard(NiuNiuC2SOpenCard* msg)
{
	LLOG_DEBUG("User::HanderNiuNiuOpenCard");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HanderNiuNiuOpenCard(this, msg);
	}
}

/*
牛牛：玩家搓公牌
MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
*/
void User::HandNiuNiuCuoGongPai(NiuNiuC2SCuoGongPai* msg)
{
	LLOG_DEBUG("User::HandNiuNiuCuoGongPai");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandNiuNiuCuoGongPai(this, msg);
	}
}

/*
牛牛：玩家选择托管消息
MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
*/
void User::HandNiuNiuTuoGuan(NiuNiuC2STuoGuan* msg)
{
	LLOG_DEBUG("User::HandNiuNiuTuoGuan");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandNiuNiuTuoGuan(this, msg);
	}
}

/*
牛牛：玩家取消托管消息
MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
*/
void User::HandNiuNiuCancelTuoGuan(NiuNiuC2SCancelTuoGuan* msg)
{
	LLOG_DEBUG("User::HandNiuNiuCancelTuoGuan");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandNiuNiuCancelTuoGuan(this, msg);
	}
}

/*
客户端通知服务器开始计时
MSG_C_2_S_START_TIME = 61198
*/
void User::HandNiuNiuStartTime(NiuNiuC2SStartTime* msg)
{
	LLOG_DEBUG("User::HandNiuNiuStartTime");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandNiuNiuStartTime(this, msg);
	}
}



void User::HanderDouDiZhuGameMessage(MHLMsgDouDiZhuC2SGameMessage * msg)
{
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HanderDouDiZhuGameMessage(this, msg);
	}
}

/*
双升：C->S 玩家报主操作
MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202
*/
void User::HandleShuangShengSelectZhu(ShuangShengC2SSelectZhu* msg)
{
	LLOG_DEBUG("User::HandleShuangShengSelectZhu");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleShuangShengSelectZhu(this, msg);
	}
}

/*
双升：C->S 玩家反主操作
MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204
*/
void User::HandleShangShengFanZhu(ShuangShengC2SFanZhu* msg)
{
	LLOG_DEBUG("User::HandleShangShengFanZhu");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleShangShengFanZhu(this, msg);
	}
}

/*
双升：C->S 玩家盖底牌操作
MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206
*/
void User::HandleShuangShengBaseCards(ShuangShengC2SBaseCards* msg)
{
	LLOG_DEBUG("User::HandleShuangShengBaseCards");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleShuangShengBaseCards(this, msg);
	}
}

/*
双升：C->S 玩家出牌操作
MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208
*/
void User::HandleShuangShengOutCards(ShuangShengC2SOutCards* msg)
{
	LLOG_DEBUG("User::HandleShuangShengOutCards");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleShuangShengOutCards(this, msg);
	}
}

/*
3打2：C->S 玩家选择叫牌分
MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
*/
void User::HandleSanDaErSelectScore(SanDaErC2SSelectScore* msg)
{
	LLOG_DEBUG("User::HandleSanDaErSelectScore");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErSelectScore(this, msg);
	}
}

/*
3打2：C->S 玩家选主牌
MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
*/
void User::HandleSanDaErSelectMain(SanDaErC2SSelectMain* msg)
{
	LLOG_DEBUG("User::HandleSanDaErSelectMain");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErSelectMain(this, msg);
	}
}

/*
3打2：C->S 玩家埋底
MSG_C_2_S_SANDAER_BASE_CARD = 62257
*/
void User::HandleSanDaErBaseCard(SanDaErC2SBaseCard* msg)
{
	LLOG_DEBUG("User::HandleSanDaErBaseCard");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErBaseCard(this, msg);
	}
}

/*
3打2：C->S 玩家选对家牌
MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
*/
void User::HandleSanDaErSelectFriend(SanDaErC2SSelectFriend* msg)
{
	LLOG_DEBUG("User::HandleSanDaErSelectFriend");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErSelectFriend(this, msg);
	}
}

/*
3打2：C->S 玩家出牌
MSG_C_2_S_SANDAER_OUT_CARD = 62261
*/
void User::HandleSanDaErOutCard(SanDaErC2SOutCard* msg)
{
	LLOG_DEBUG("User::HandleSanDaErOutCard");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErOutCard(this, msg);
	}
}

/*
3打2：C->S 105退出
MSG_C_2_S_SANDAER_SCORE_105_RET = 62266
*/
void User::HandleSanDaErScore105Ret(SanDaErC2SScore105Ret* msg)
{
	LLOG_DEBUG("User::HandleSanDaErScore105Ret");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErScore105Ret(this, msg);
	}
}

void User::HandleSanDaErGetBaseCards(SanDaErC2SGetBaseCards* msg)
{
	LLOG_DEBUG("User::HandleSanDaErGetBaseCards");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErGetBaseCards(this, msg);
	}
}

/*
3打2：C->S 客户端庄家选择交牌
MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272
*/
void User::HandleSanDaErZhuangRequestJiaoPai(SanDaErC2SZhuangJiaoPai* msg)
{
	LLOG_DEBUG("User::HandleSanDaErZhuangRequestJiaoPai");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErZhuangRequestJiaoPai(this, msg);
	}
}

/*
C->S 非庄家  客户端选择庄家交牌的申请
MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274
*/
void User::HandleSanDaErXianSelectJiaoPai(SanDaErC2SXianSelectJiaoPai* msg)
{
	LLOG_DEBUG("User::HandleSanDaErXianSelectJiaoPai");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaErXianSelectJiaoPai(this, msg);
	}
}


/*
3打1：C->S 玩家选择叫牌分
MSG_C_2_S_SANDAYI_SELECT_SCORE = 62253
*/
void User::HandleSanDaYiSelectScore(SanDaYiC2SSelectScore* msg)
{
	LLOG_DEBUG("User::HandleSanDaYiSelectScore");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaYiSelectScore(this, msg);
	}
}

/*
3打1：C->S 玩家选主牌
MSG_C_2_S_SANDAYI_SELECT_MAIN = 62255
*/
void User::HandleSanDaYiSelectMain(SanDaYiC2SSelectMain* msg)
{
	LLOG_DEBUG("User::HandleSanDaYiSelectMain");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaYiSelectMain(this, msg);
	}
}

/*
3打1：C->S 玩家埋底
MSG_C_2_S_SANDAYI_BASE_CARD = 62307
*/
void User::HandleSanDaYiBaseCard(SanDaYiC2SBaseCard* msg)
{
	LLOG_DEBUG("User::HandleSanDaYiBaseCard");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaYiBaseCard(this, msg);
	}
}


/*
3打1：C->S 玩家出牌
MSG_C_2_S_SANDAYI_OUT_CARD = 62261
*/
void User::HandleSanDaYiOutCard(SanDaYiC2SOutCard* msg)
{
	LLOG_DEBUG("User::HandleSanDaYiOutCard");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaYiOutCard(this, msg);
	}
}

/*
3打1：C->S 玩家回复闲家得分慢105分,是否停止游戏
MSG_C_2_S_SANDAYI_SCORE_105_RET = 62266,
*/
void User::HandleSanDaYiScore105Ret(SanDaYiC2SScore105Ret* msg)
{
	LLOG_DEBUG("User::HandleSanDaYiScore105Ret");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleSanDaYiScore105Ret(this, msg);
	}
}


/*
五人百分：C->S 玩家选择叫牌分
MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
*/
void User::HandleWuRenBaiFenSelectScore(WuRenBaiFenC2SSelectScore* msg)
{
	LLOG_DEBUG("User::HandleWuRenBaiFenSelectScore");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenSelectScore(this, msg);
	}
}

/*
五人百分：C->S 玩家选主牌
MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
*/
void User::HandleWuRenBaiFenSelectMain(WuRenBaiFenC2SSelectMain* msg)
{
	LLOG_DEBUG("User::HandleWuRenBaiFenSelectMain");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenSelectMain(this, msg);
	}
}

/*
五人百分：C->S 玩家埋底
MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
*/
void User::HandleWuRenBaiFenBaseCard(WuRenBaiFenC2SBaseCard* msg)
{
	LLOG_DEBUG("User::HandleWuRenBaiFenBaseCard");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenBaseCard(this, msg);
	}
}

/*
五人百分：C->S 玩家选对家牌
MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
*/
void User::HandleWuRenBaiFenSelectFriend(WuRenBaiFenC2SSelectFriend* msg)
{
	LLOG_DEBUG("User::HandleWuRenBaiFenSelectFriend");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenSelectFriend(this, msg);
	}
}

/*
五人百分：C->S 玩家出牌
MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
*/
void User::HandleWuRenBaiFenOutCard(WuRenBaiFenC2SOutCard* msg)
{
	LLOG_DEBUG("User::HandleWuRenBaiFenOutCard");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenOutCard(this, msg);
	}
}

/*
五人百分：C->S 玩家回复闲家得分慢105分,是否停止游戏
MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,
*/
void User::HandleWuRenBaiFenScore105Ret(WuRenBaiFenC2SScore105Ret* msg)
{
	LLOG_DEBUG("User::HandleWuRenBaiFenScore105Ret");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenScore105Ret(this, msg);
	}
}

void User::HanderUserRequestGPS(LMsgC2SRequestUserGps* msg)
{
	if (msg == NULL)return;
	LLOG_DEBUG("HanderUserRequestGPS userId[%d] requet userId[%d] gps",GetUserDataId(),msg->m_userId);
	if (m_desk)
	{
		m_desk->HanderUserRequestGPS(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserSpeak no desk, userid=%d", m_userData.m_id);
	}

}

/*
C->S 客户端庄家选择交牌
MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI = 62368
*/
void User::HandleWuRenBaiFenZhuangJiaoPai(WuRenBaiFenC2SZhuangJiaoPai * msg)
{
	LLOG_DEBUG("User::WuRenBaiFenC2SZhuangJiaoPai");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenZhuangJiaoPai(this, msg);
	}
}

/*
C->S 非庄家客户端选择庄家交牌的申请
MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI = 62370
*/
void User::HandleWuRenBaiFenXianAckJiaoPai(WuRenBaiFenC2SXianSelectJiaoPai * msg)
{
	LLOG_DEBUG("User::HandleWuRenBaiFenXianAckJiaoPai");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandleWuRenBaiFenXianAckJiaoPai(this, msg);
	}
}

/*
扯炫：C->S 处理玩家起钵钵
MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
*/
void User::HandlerPlayerSelectBoBo(CheXuanC2SPlayerSelectBoBo* msg)
{
	LLOG_DEBUG("User::HandlerPlayerSelectBoBo");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerSelectBoBo(this, msg);
	}
}

/*
扯炫：C->S 处理玩家选择操作
MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
*/
void User::HandlerPlayerSelectOpt(CheXuanC2SPlayerSelectOpt* msg)
{
	LLOG_DEBUG("User::HandlerPlayerSelectOpt");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerSelectOpt(this, msg);
	}
}

/*
扯炫：C->S 处理玩家扯牌操作
MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
*/
void User::HandlerPlayerChePai(CheXuanC2SPlayerChePai* msg)
{
	LLOG_DEBUG("User::HandlerPlayerChePai");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerChePai(this, msg);
	}
}

/*
C->S 玩家搓牌操作
MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
*/
void User::HandlerPlayerCuoPai(CheXuanC2SPlayerCuoPai* msg)
{
	LLOG_DEBUG("User::HandlerPlayerCuoPai");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerCuoPai(this, msg);
	}
}

/*
C->S 客户端牌局内请求战绩
MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
*/
void User::HandlerPlayerRequestRecord(CheXuanC2SPlayerRequestRecord* msg)
{
	LLOG_DEBUG("User::HandlerPlayerRequestRecord");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerRequestRecord(this, msg);
	}
}

/*
C->S 客户端操作默分
MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
*/
void User::HandlerPlayerOptMoFen(CheXuanC2SPlayerOptMoFen* msg)
{
	LLOG_DEBUG("User::HandlerPlayerOptMoFen");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerOptMoFen(this, msg);
	}
}

/*
C->S 客户端选择自动操作
MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
*/
void User::HandlerPlayerSelectAutoOpt(CheXuanC2SPlayerSelectAutoOpt* msg)
{
	LLOG_DEBUG("User::HandlerPlayerSelectAutoOpt");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerSelectAutoOpt(this, msg);
	}
}

/*
C->S 客户端选择站起或坐下
MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
*/
void User::HandlerPlayerStandOrSeat(CheXuanC2SPlayerSelectStandOrSeat* msg)
{
	LLOG_DEBUG("User::HandlerPlayerStandOrSeat");
	if (NULL == msg) return;
	if (m_desk)
	{
		m_desk->HandlerPlayerStandOrSeat(this, msg);
	}
}

