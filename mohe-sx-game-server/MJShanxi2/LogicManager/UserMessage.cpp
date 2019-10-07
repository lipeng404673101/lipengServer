#include "UserMessage.h"
#include "LTime.h"
#include "LMemoryRecycle.h"
#include "Config.h"
#include "Work.h"
#include "UserManager.h"
#include "RuntimeInfoMsg.h"
#include "LVipLog.h"
#include "DeskManager.h"
#include "mhmsghead.h"

#include "clubUser.h"
#include "SpecActive.h"

CUserMessage::CUserMessage()
{
}

CUserMessage::~CUserMessage()
{

}

void CUserMessage::Clear()
{

}

void CUserMessage::Run()
{
	LTime msgTime;
	int iMsgCount = 0;
	int iElapseTime = 0;
	while(!GetStop())
	{
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			iElapseTime++;
			continue;
		}
		else
		{
			HanderMsg(msg);
			m_memoryRecycle.recycleMsg(msg);

			//姣忎竴涓囦釜鍖呰緭鍑烘墍鐢ㄦ椂闂?
			iMsgCount++;
			if(iMsgCount == 10000)
			{
				LTime now;
				LLOG_ERROR("User Logic: 10000 package needs %d time.", (int)(now.MSecs() - msgTime.MSecs() - iElapseTime));

				iMsgCount = 0;
				iElapseTime = 0;

				msgTime = now;
			}
		}
	}
}

void CUserMessage::Start()
{
	m_memoryRecycle.Start();
	LRunnable::Start();
}

void CUserMessage::Stop()
{
	m_memoryRecycle.Stop();
	LRunnable::Stop();
}

void CUserMessage::Join()
{
	m_memoryRecycle.Join();
	LRunnable::Join();
}

void CUserMessage::HanderMsg(LMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("CUserMessage::HanderMsg  msg->m_msgId = %d", msg->m_msgId);
	switch (msg->m_msgId)
	{
		/*
		Center to LogicManager
		*/
	case MSG_CE_2_L_USER_LOGIN:
		LLOG_DEBUG("CUserMessage::HanderMsg   MSG_CE_2_L_USER_LOGIN= %d", msg->m_msgId);
		HanderCenterUserLogin((LMsgCe2LUserLogin*)msg);
		break;
	case MSG_CE_2_L_GM_CHARGE:
		HanderCenterGMCharge((LMsgCe2LGMCharge*)msg);
		break;
	case MSG_CE_2_L_GM_COINS:
		HanderCenterGMCoins((LMsgCe2LGMCoins*)msg);
		break;

	//(6408)Center返回的玩家金币更新
	case MSG_CE_2_LMG_UPDATE_COIN:
		HanderCenterUpdateCoins((LMsgCe2LUpdateCoin*)msg);
		break;

	case MSG_CE_2_LMG_UPDATE_COIN_JOIN_DESK:
		HanderCenterUpdateCoinsJoinDesk((LMsgCe2LUpdateCoinJoinDesk*)msg);
		break;

	case MSG_CE_2_LMG_UPDATE_POINT_RECORD:
		gClubManager.userChangePointFromCenter((LMsgLMG2CEUpdatePointRecord*)msg);
		break;
	case MSG_CE_2_LMG_POINT_RECORD_LOG:
		gClubManager.userRequestPointRecordFromCenter((LMsgCE2LMGRecordLog*)msg);
		break;
	case MSG_CE_2_LMG_UPDATE_CLUB:
		gClubManager.userUpdateClubFromCenter((LMsgCE2LMGClubInfo*)msg);
		break;
	case MSG_CE_2_LMG_UPDATE_PLAY:
		gClubManager.userUpdatePlayFromCenter((LMsgCE2LMGPlayInfo*)msg);
		break;

		/*
		LogicServer or CoinsServer to LogicManager
		*/
	case MSG_L_2_LMG_MODIFY_USER_STATE:
		HanderModifyUserState((LMsgL2LMGModifyUserState*)msg);
		break;

	case MSG_L_2_LMG_MODIFY_USER_CARD:
		HanderModifyUserCardNum((LMsgL2LMGModifyCard*)msg);
		break;
	case MSG_L_2_LMG_ADD_USER_PLAYCOUNT:
		HanderAddUserPlayCount((LMsgL2LMGAddUserPlayCount*)msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_CREATED_DESK:
		HanderModifyUserCreatedDesk((LMsgL2LMGModifyUserCreatedDesk*)msg);
		break;
	case MSG_L_2_LMG_DELETE_USER_CREATED_DESK:
		HanderDeleteUserCreatedDesk((LMsgL2LMGDeleteUserCreatedDesk*)msg);
		break;
	case MSG_L_2_LMG_UPDATE_USER_GPS:
		HanderModifyUserGps((LMsgL2LMGUpdateUserGPS*)msg);
		break;
	case MH_MSG_L_2_LMG_NEW_AGENCY_ACTIVITY_UPDATE_PLAY_COUNT:
		MHHanderNewAgencyActivityUpdatePlayCount((MHLMsgL2LMGNewAgencyActivityUpdatePlayCount*)msg);
		break;
	case MSG_L_2_LMG_USER_ADD_CLUB_DESK:
	{

		gClubManager.userAddClubDeskFromLogic((LMsgL2LMGUserAddClubDesk*)msg);
		//LTime ts;
		//HanderUserAddClubDesk((LMsgL2LMGUserAddClubDesk*)msg);
		//LTime te;
		//Llong t = te.MSecs() - ts.MSecs();
		//Lint c  = ((LMsgL2LMGUserAddClubDesk*)msg)->m_clubId;
		//LLOG_ERROR("**MSG_L_2_LMG_USER_ADD_CLUB_DESK use time  [%lld] ms, clubid [%5d]", t, c);
		break;
	}
	case MSG_L_2_LMG_USER_LEAVE_CLUB_DESK:
	{
		gClubManager.userLeaveClubDeskFromLogic((LMsgL2LMGUserLeaveClubDesk*)msg);
		//LTime ts;
		// HanderUserLeaveClubDesk((LMsgL2LMGUserLeaveClubDesk*)msg);
		//LTime te;
		//Llong t = te.MSecs() - ts.MSecs();
		//Lint c = ((LMsgL2LMGUserLeaveClubDesk*)msg)->m_clubId;
		//LLOG_ERROR("**MSG_L_2_LMG_USER_LEAVE_CLUB_DESK use time  [%lld] ms, clubid [%5d]", t, c);

		break;
	}
	case MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK:
	{
		gClubManager.userLeaveClubDeskFromLogic((LMsgL2LMGUserListLeaveClubDesk*)msg);
		//LLOG_ERROR("**MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK  2222");
		//LTime ts;
		//HanderUserListLeaveClubDesk((LMsgL2LMGUserListLeaveClubDesk*)msg);
		//LTime te;
		//Llong t = te.MSecs() - ts.MSecs();
		//Lint c = ((LMsgL2LMGUserListLeaveClubDesk*)msg)->m_clubId;
		//LLOG_ERROR("**MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK use time  [%lld] ms, clubid [%5d]", t, c);

		break;
	}
	case MSG_L_2_LMG_FRESH_DESK_INFO:
	{		
		gClubManager.freshClubDeskInfoFromLogic((LMsgL2LMGFreshDeskInfo*)msg);
		//LTime ts;
		//HanderUserFreshClubInfo((LMsgL2LMGFreshDeskInfo*)msg);
		//LTime te;
		//Llong t = te.MSecs() - ts.MSecs();		 
		//Lint c = ((LMsgL2LMGFreshDeskInfo*)msg)->m_clubId;
		//LLOG_ERROR("**MSG_L_2_LMG_FRESH_DESK_INFO use time  [%lld] ms, clubid [%d]", t, c);
	    break;
	}
	//红包
	case MSG_L_2_CE_CLUB_FIRST_RED_PACKET:
	{
		LLOG_DEBUG("MSG_L_2_CE_CLUB_FIRST_RED_PACKET");
		LMsgL2CeClubFirstRedPacket redPacket = *((LMsgL2CeClubFirstRedPacket *)msg);
		gWork.SendToCenter(redPacket);
		break;
	}


	/*
	Gate to LogicManager
	*/
	case MSG_G_2_L_USER_MSG:
		HanderUserMsg((LMsgG2LUserMsg*)msg);
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		HanderUserOutMsg((LMsgG2LUserOutMsg*)msg);
		break;

	/*
	DB to LogicManager
	*/
	case MSG_LDB_2_L_VIP_LOG:
		HanderDBReqVipLog((LMsgLBD2LReqVipLog*) msg);
		break;
	//玩家大厅请求的第一步战绩从DB查询出来发给Manager处理后发给玩家
	case MSG_LDB_2_LM_POKER_LOG:
		HanderDBReqPokerLog((LMsgLBD2LReqPokerLog*)msg);
		break;
	case MSG_LDB_2_LM_RET_CRELOGHIS:
		HanderDBReqCRELogHis((LMsgLDB2LM_RecordCRELog*)msg);
		break;
	case MSG_LDB_2_L_ROOM_LOG:
	{
		LMsgLBD2LReqRoomLog *pMsg = (LMsgLBD2LReqRoomLog*)msg;
		if (pMsg&&pMsg->m_type == 0)
			HanderDBReqRoomLog(pMsg);
		else
			HanderDBQiPaiReqRoomLog(pMsg);
		break;
	}
	case MSG_LDB_2_L_VIDEO:
		HanderDBReqVideo((LMsgLDB2LReqVideo*)msg);
		break;
	case MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS:
		HanderDBRetBindingRelation((LMsgLDB2LMGBindingRelationships*)msg);
		break;
	case MSG_LDB_2_LM_RET_INFO:
		HanderDBRetInfo((LMsgLDB_2_LM_RetInfo*)msg);
		break;
	case MSG_LDB_2_LMG_USER_MSG:
		HanderDBUserMsg((LMsgLDB2LMGUserMsg*) msg);
		break;
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		HanderModifyUserCoinsNum((LMsgCN2LMGModifyUserCoins*) msg);
		break;
		//logicdb杩斿洖鏌愬綍鍍忔槸鍚︽湁shareid锛屾病鏈夊垯灏嗗叾浠巚ideo涓Щ鑷硈harevideo涓?
	case MSG_LDB_2_L_RETSHAREVIDEOID:
		HanderDBGetShareVideoID((LMsgLDB2LRetShareVideoId*) msg);
		break;
		//logicdb 杩斿洖 閫氳繃shareid鏌ヨ鍒扮殑鍙兘鐨勫綍鍍?
	case MSG_LDB_2_L_RETSHAREVIDEO:
		HanderDBRetShareVideo((LMsgLDB2LRetShareVideo*) msg);
		break;
	case MH_MSG_LDB_2_L_SHARE_VIDEO_REULST:
		HanderDBSharedVideoResult((MHLMsgLDB2LShareVideoResult*)msg);
		break;
	//玩家请求回放第3步，根据videoId返回具体回放数据
	case MSG_LDB_2_L_POKER_VIDEO:
	{
		LLOG_DEBUG("MSG_LDB_2_L_POKER_VIDEO");
		HanderDBQiPaiReqVideo((LMsgLDB2LPokerReqVideo*)msg);
		break;
	}
	default:
		break;
	}
}

void CUserMessage::HanderCenterUserLogin(LMsgCe2LUserLogin* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<UserLoginInfo> userLoginInfo(new UserLoginInfo);
	userLoginInfo->m_seed = msg->m_seed;
	userLoginInfo->m_time = GetCurTime().Secs();
	userLoginInfo->m_user = msg->user;


	LLOG_ERROR("HanderCenterUserLogin usert id[%d], m_unionid[%s], changeStats[%d]",
		userLoginInfo->m_user.m_id, userLoginInfo->m_user.m_unioid.c_str(), userLoginInfo->m_user.m_customInt[7]);

	gUserManager.addUserLoginInfo(userLoginInfo);
}

void CUserMessage::HanderCenterGMCharge(LMsgCe2LGMCharge* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMCharge %d:%d:%d:%d", msg->m_userid, msg->m_cardType, msg->m_cardCount, msg->m_oper);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser && safeUser->isValid())
	{
		safeUser->getResource()->AddCardCount(msg->m_cardType, msg->m_cardCount, msg->m_oper, msg->m_admin, false);
	}
	else
	{
		LLOG_ERROR("Work::HanderCenterGMCharge user not exiest %d:%d:%d:%d",  msg->m_userid, msg->m_cardType, msg->m_cardCount,msg->m_oper);
	}
}

void CUserMessage::HanderCenterGMCoins(LMsgCe2LGMCoins* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMCoins %d:%d:%d", msg->m_userid, msg->m_coins, msg->m_totalcoins);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser && safeUser->isValid())
	{
		if ( msg->m_coins > 0 )
		{
			safeUser->getResource()->AddCoinsCount(msg->m_coins, COINS_OPER_TYPE_CHARGE, false);
		}
		else
		{
			safeUser->getResource()->DelCoinsCount(msg->m_coins, COINS_OPER_TYPE_CHARGE, false);
		}
	}
	else
	{
		LLOG_ERROR("Work::HanderCenterGMCoins user not exiest %d:%d:%d", msg->m_userid, msg->m_coins, msg->m_totalcoins);
	}
}

void CUserMessage::HanderCenterUpdateCoins(LMsgCe2LUpdateCoin* msg)
{
	LLOG_DEBUG("CUserMessage::HanderCenterUpdateCoins");
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userID);
	if (safeUser && safeUser->isValid())
	{
		boost::shared_ptr<User> user = safeUser->getResource();
		user->m_userData.m_numOfCard2s = msg->m_coinNum;
		user->m_userData.m_customString1 = msg->m_userIp;
		//user->m_userData.m_customString2 = msg->m_userGps;
		user->m_userData.m_customInt[0] = msg->m_usert.m_customInt[0];
		user->UpdateUserData(msg->m_usert);
		user->SetIp(msg->m_userIp);
		if (msg->m_creatType == MSG_S_2_C_CREATE_ROOM) 
		{
			//玩家真正创建房间
			user->RealDoHanderUserCreateDesk(msg);
		}
		else if (msg->m_creatType == MSG_C_2_S_CREATE_ROOM_FOR_OTHER)
		{
			user->RealDoHanderUserCreateDeskForOther(msg);
		}
	}
}

void CUserMessage::HanderCenterUpdateCoinsJoinDesk(LMsgCe2LUpdateCoinJoinDesk* msg)
{
	LLOG_DEBUG("CUserMessage::HanderCenterUpdateCoinsJoinDesk");
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userID);
	if (safeUser && safeUser->isValid())
	{
		boost::shared_ptr<User> user = safeUser->getResource();
		user->m_userData.m_numOfCard2s=msg->m_coinNum;
		user->m_userData.m_customString1 = msg->m_usert.m_customString1;
		//user->m_userData.m_customString2 = msg->m_usert.m_customString2;
		user->m_userData.m_customInt[0] = msg->m_usert.m_customInt[0];
		user->UpdateUserData(msg->m_usert);
		user->SetIp(msg->m_userIp);

		//俱乐部防作弊匹配入座按钮加入
		if (msg->m_type == 3)
		{
			//user->HanderUserJoinRoomAnonymous2(msg);
			return;
		}

		if (msg->m_type == 4) {
			//user->RealDoHanderUserCreateClubDesk(msg);
			return;
		}

		//俱乐部
		if (msg->m_deskID >= CLUB_DESK_ID_BEGIN_NUMBER)
		{
			//user->HanderUserEnterClubDesk(msg);
			gClubManager.userJoinClubDeskFromCenter(msg, user.get());
		}
		else 
			user->RealDoHanderUserAddDesk(msg);
	}
}

void CUserMessage::HanderModifyUserState(LMsgL2LMGModifyUserState* msg)
{
	if(msg == NULL)
	{
		return;
	}
	
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("Work::HanderModifyUserState ERROR ID:%d, State:%d, serverID: %d, user not exist", msg->m_userid, msg->m_userstate, msg->m_logicID);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();

	LLOG_ERROR("Work::HanderModifyUserState after ID:%d, State:%d, serverID: %d", user->m_userData.m_id, msg->m_userstate, msg->m_logicID);
	user->setUserState(msg->m_userstate);
	user->setUserLogicID(msg->m_logicID);
	if(msg->m_logicID==0)
		gClubUserManager.setUserPlayStatus(user->GetUserDataId(), false);
	else
		gClubUserManager.setUserPlayStatus(user->GetUserDataId(), true);

}

void CUserMessage::HanderModifyUserCardNum(LMsgL2LMGModifyCard* msg)
{
	if(msg == NULL)
	{
		return;
	}
	//LLOG_DEBUG("CUserMessage::HanderModifyUserCardNum 44444444444444444");
	//淇变箰閮ㄩ捇鐭冲埜
	if (CARDS_OPER_TYPE_CLUB_DIAMOND==msg->operType)
	{
		LLOG_DEBUG("CUserMessage::HanderModifyUserCardNum :CARDS_OPER_TYPE_CLUB_DIAMOND==msg->operType");
		this->modifyCardCountSendToCenter(msg);
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("Work::HanderModifyUserCardNum user not exiest, userid=%d, type=%d, count=%d, operType=%d",  msg->m_userid, msg->cardType, msg->cardNum, msg->operType);
		this->modifyCardCountSendToCenter(msg);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();

	if(msg->isAddCard)
	{
		user->AddCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
	}
	else
	{
		if ( msg->operType == CARDS_OPER_TYPE_CREATE_ROOM )
		{
			if (!gRuntimeInfoMsg.isFree(-1) && !gRuntimeInfoMsg.isFree(user->getLastCreateGameType()))
			{
				LLOG_DEBUG("CUserMessage::HanderModifyUserCardNum 111111111111111111");
				user->DelCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
			}
			else
			{
				//LLOG_DEBUG("CUserMessage::HanderModifyUserCardNum 2222222222222222222");
				LLOG_ERROR("CUserMessage::HanderModifyUserCardNum Now Free, userid=%d, type=%d, count=%d, operType=%d gametype=%d",  msg->m_userid, msg->cardType, msg->cardNum, msg->operType, user->getLastCreateGameType());
			}
		}
		else
		{
			//LLOG_DEBUG("CUserMessage::HanderModifyUserCardNum 3333333333333333333");
			user->DelCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
		}
	}
}

void CUserMessage::modifyCardCountSendToCenter(LMsgL2LMGModifyCard* L2LmgMsg, bool bNeedSave)
{
	if (bNeedSave)
	{
		// 閫忎紶缁機enter
		gWork.SendToCenter(*L2LmgMsg);
	}
}

void CUserMessage::HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_DEBUG("Work::HanderModifyUserPlayCount user not exiest, userid=%d",  msg->m_userid);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	user->AddPlayCount();
}

void CUserMessage::HanderModifyUserCreatedDesk(LMsgL2LMGModifyUserCreatedDesk* msg)
{
	LLOG_DEBUG("Work::HanderModifyUserCreatedDesk");
	gUserManager.addUserDeskId(msg->m_userid, msg->m_deskId, msg->m_cost);
}

void CUserMessage::HanderDeleteUserCreatedDesk(LMsgL2LMGDeleteUserCreatedDesk* msg)
{
	gUserManager.delUserDeskId(msg->m_userid, msg->m_deskId, msg->m_cost, msg->m_delType);
}

void CUserMessage::HanderModifyUserGps(LMsgL2LMGUpdateUserGPS* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("CUserMessage::HanderModifyUserGps userid:%d gps:%s", msg->m_userid, msg->m_Gps.c_str());

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_DEBUG("Work::HanderModifyUserGps user not exiest, userid=%d", msg->m_userid);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	user->m_userData.m_customString2 = msg->m_Gps;

	LMsgLMG2CEUpdateUserGPS send;
	send.m_userid = msg->m_userid;
	send.m_Gps = msg->m_Gps;
	send.m_strUUID = msg->m_strUUID;
	gWork.SendToCenter(send);
}

void CUserMessage::HanderUserMsg(LMsgG2LUserMsg* msg)
{
	LLOG_DEBUG("Work::HanderUserMsg(LMsgG2LUserMsg* msg)");
	if(msg == NULL)
	{
		return;
	}

	Lint gateId = gWork.getGateIdBySp(msg->m_sp);
	if(gateId == INVALID_GATEID)
	{
		LLOG_ERROR("%s have not gate", msg->m_strUUID.c_str());
		return;
	}

	if (msg->m_userMsg)
	{
		switch (msg->m_userMsgId)
		{
		case MSG_C_2_S_LOGIN:
			HanderUserLogin(((LMsgC2SLogin*)msg->m_userMsg)->m_id, ((LMsgC2SLogin*)msg->m_userMsg)->m_seed, gateId, ((LMsgC2SLogin*)msg->m_userMsg)->m_md5, msg->m_ip);
			break;
		case MSG_C_2_S_GET_COINS:
			{
				LMsgLMG2CNGetCoins send;
				send.m_strUUID	= msg->m_strUUID;
				send.m_userid	= ((LMsgC2SGetCoins*)msg->m_userMsg)->m_userid;
				send.m_gateid   = gateId;
				gWork.SendMessageToCoinsServer( send );
			}
			break;
		default:
			{
				boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserByGateIdAndUUID(gateId, msg->m_strUUID);
				if(safeUser && safeUser->isValid())
				{
					safeUser->getResource()->HanderMsg(msg->m_userMsg);
				}
				else
				{
					LLOG_DEBUG("Work::HanderUserMsg user not exist %d:%s:%d", gateId, msg->m_strUUID.c_str(), msg->m_userMsgId);
				}
			}
			break;
		}
	}
	else
	{
		LLOG_DEBUG("Work::HanderUserMsg msg error %d:%s:%d", gateId, msg->m_strUUID.c_str(), msg->m_userMsgId);
	}
}

void CUserMessage::HanderUserOutMsg(LMsgG2LUserOutMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	Lint gateId = gWork.getGateIdBySp(msg->m_sp);
	if(gateId == INVALID_GATEID)
	{
		LLOG_ERROR("%s have not gate", msg->m_strUUID.c_str());
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = UserManager::Instance().getUserByGateIdAndUUID(gateId, msg->m_strUUID);
	if(safeUser && safeUser->isValid())
	{
		boost::shared_ptr<User> user = safeUser->getResource();

		LLOG_DEBUG("Work::HanderUserLogout %d", user->GetUserDataId());

		user->Logout();
		gClubUserManager.userOffLine(user->m_userData.m_id);

		gRuntimeInfoMsg.changeOnlineNum(gateId, user->GetUserDataId(), false);

		if( (!gWork.isLogicServerExist(user->getUserLogicID()) ) && ( user->getUserClubStatus()!=USER_CLUB_PIPEI_STATUS ) && (user->getUserClubStatus() != USER_CLUB_DESK_STATUS))
		{
			user->S2CeUserLogout();
			UserManager::Instance().delUser(user->m_userData.m_id);
		}
	}
	else
	{
		LLOG_DEBUG("Work::HanderUserLogout user not find %d:%s", gateId, msg->m_strUUID.c_str());
	}
}

void CUserMessage::HanderUserLogin(Lint id, Lint seed, Lint gateId, Lstring& md5,Lstring& ip)
{

	LLOG_ERROR("***LMsgC2SLogin: msgid:%d m_id:%d m_ip:%s m_seed:%d gateId:%d",
		11, id, ip.c_str(), seed, gateId);
	boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > safeLoginInfo = gUserManager.getUserLoginInfo(id);
	if(safeLoginInfo.get() == NULL || !safeLoginInfo->isValid())
	{
	
		LLOG_ERROR("Work::HanderUserLogin erro %d:%d:%d", id, gateId,seed); 
		return;
	}

	if(safeLoginInfo->getResource()->m_seed != seed)
	{
		LLOG_ERROR("Work::HanderUserLogin seed error %d:%d:%d:%d", id, gateId, seed, safeLoginInfo->getResource()->m_seed);
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(id);
	boost::shared_ptr<User> user;
	if(safeUser && safeUser->isValid())
	{
		user = safeUser->getResource();

		bool bBeforeOnlineState = user->GetOnline();

		if(user->GetUserGateId() != gateId) //
		{
			LLOG_DEBUG("Work::HanderUserLogin user online %d:%d", user->GetUserDataId(), user->GetUserGateId());
			LMsgL2GUserOutMsg msgOut;
			msgOut.m_strUUID = user->m_userData.m_unioid;
			gWork.SendMessageToGate(user->GetUserGateId(), msgOut);

			gRuntimeInfoMsg.changeOnlineNum(user->GetUserGateId(), user->GetUserDataId(), false);
		}

		UserManager::Instance().delUser(id);

		user->SetUserGateId(gateId);
		user->m_userData.m_new = safeLoginInfo->getResource()->m_user.m_new;
		user->m_userData.m_unioid = safeLoginInfo->getResource()->m_user.m_unioid;
		user->m_userData.m_openid = safeLoginInfo->getResource()->m_user.m_openid;
		user->m_userData.m_customInt[7] = safeLoginInfo->getResource()->m_user.m_customInt[7];   //更新用户状态changestatus
		//user->SetIp(ip);
		UserManager::Instance().addUser(user);
		user->Login();

		if(!bBeforeOnlineState)
		{
			gUserManager.delUserLoginInfo(user->GetUserDataId());
		}
	}
	else
	{
		user.reset(new User(safeLoginInfo->getResource()->m_user, gateId));

		//user->SetIp(ip);
		UserManager::Instance().addUser(user);
		user->S2CeUserLogin();
		user->Login();
		gUserManager.delUserLoginInfo(user->GetUserDataId());

		LLOG_DEBUG("Work::HanderUserLogin user login success %d:%d:%d:%s", user->GetUserDataId(), user->GetUserDataId(), user->GetUserGateId(),user->GetIp().c_str());
	}

	if(user)
	{
		gRuntimeInfoMsg.changeOnlineNum(gateId, user->GetUserDataId(), true);
	}
}

//db鍙戦�佺帺瀹舵垬缁?
void CUserMessage::HanderDBReqVipLog(LMsgLBD2LReqVipLog* msg)
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline())
	{
		LMsgS2CVipLog log;
		log.m_size = msg->m_count;
		for(Lint i = 0 ; i < msg->m_count; ++i)
		{
			VipLogItemInfo& info = log.m_info[i];
			info.m_id = msg->m_info[i].m_id;
			info.m_time = msg->m_info[i].m_time;
			info.m_flag = msg->m_info[i].m_flag;
			info.m_reset = msg->m_info[i].m_reset;
			info.m_deskId = msg->m_info[i].m_deskId;
			info.m_clubId = msg->m_info[i].m_clubId;
			info.m_showDeskId = msg->m_info[i].m_showDeskId;
			info.m_cellScore = msg->m_info[i].m_cellScore;
			memcpy(info.m_score,msg->m_info[i].m_score,sizeof(info.m_score));
			memcpy(info.m_userid,msg->m_info[i].m_posUserId,sizeof(info.m_userid));
			//鏌ユ壘鐜╁鏄电О
			for (Lint j = 0; j < 4; ++j)
			{
				boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(info.m_userid[j]);
				if(safeBaseInfo && safeBaseInfo->isValid())
				{
					info.m_nike[j] = safeBaseInfo->getResource()->m_nike;
				}
			}
		}
		user->Send(log);
	}
}

void CUserMessage::HanderDBReqPokerLog(LMsgLBD2LReqPokerLog* msg)
{
	LLOG_DEBUG("CUserMessage::HanderDBReqPokerLog");
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if (user && user->GetOnline())
	{
		LMsgS2CQiPaiLog log;
		log.m_size = msg->m_count;

		for (Lint i = 0; i < msg->m_count; ++i)
		{
			if (i >= 20)break;
			QiPaiLogItemInfo& info = log.m_info[i];
			info.m_id = msg->m_info[i].m_id;
			info.m_time = msg->m_info[i].m_time;
			if (msg->m_info[i].m_clubId != 0)
			{
				auto safeClub = gClubManager.getClubbyClubId(msg->m_info[i].m_clubId);
				if (safeClub && safeClub->isValid())
				{
					info.m_clubId = msg->m_info[i].m_clubId;
					info.m_clubName = safeClub->getResource()->getClubName();
				}
			}
			info.m_flag = msg->m_info[i].m_flag;
			info.m_deskId = msg->m_info[i].m_deskId;
			info.m_maxCircle = msg->m_info[i].m_maxCircle;
			info.m_curCircle = msg->m_info[i].m_curCircle;
			info.m_createUserId = msg->m_info[i].m_createUserId;
			memcpy(info.m_score, msg->m_info[i].m_score, sizeof(info.m_score));
			memcpy(info.m_userid, msg->m_info[i].m_posUserId, sizeof(info.m_userid));
			std::vector<Lstring>  str2;
			if (msg->m_info[i].m_playtype.find(";") != std::string::npos)
			{
				L_ParseString(msg->m_info[i].m_playtype, str2, ";");
				for (size_t j = 0; j < str2.size(); ++j)
				{
					info.m_playType.push_back(atoi(str2[j].c_str()));
				}
			}

			std::vector<Lstring> strUser, strOpt;
			if (msg->m_info[i].m_optData.find(";") != std::string::npos)
			{
				L_ParseString(msg->m_info[i].m_optData, strUser, ";");
				for (size_t j = 0; j < strUser.size(); ++j)
				{
					if (strUser[j].find(",") != std::string::npos)
					{
						L_ParseString(strUser[j], strOpt, ",");
						if (strOpt.size() >= 1)
						{
							info.m_qiangCount[j] = atoi(strOpt[0].c_str());
						}
						if (strOpt.size() >= 2)
						{
							info.m_zhuangCount[j] = atoi(strOpt[1].c_str());
						}
						if (strOpt.size() >= 3)
						{
							info.m_tuiCount[j] = atoi(strOpt[2].c_str());
						}
					}
				}
			}

			//赢三张，先这么写
			switch (info.m_flag)
			{
			case 101:
			{
				info.m_userCount = 3;
				break;
			}
			case 102:  //赢三张
			{
				info.m_userCount = 8;
				break;
			}
			//牛牛
			case 103:
			{
				info.m_userCount = 10;
				break;
			}
			//新版牛牛
			case 108:
			{
				info.m_userCount = 10;
				break;
			}
			case 104:  //推筒子
			{
				info.m_userCount = 8;
				break;
			}
			case 106:  //升级
			{
				info.m_userCount = 4;
				break;
			}
			case 107:  //三打二
			{
				info.m_userCount = 5;
				break;
			}
			case 109:  //三打一
			{
				info.m_userCount = 4;
				break;
			}
			case 110:  //五人百分
			{
				info.m_userCount = 5;
				break;
			}
			case 111:  //扯炫
			{
				info.m_userCount = 8;
				break;
			}
			}

			//查找玩家昵称
			for (Lint j = 0; j < info.m_userCount; ++j)
			{
				boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(info.m_userid[j]);
				if (safeBaseInfo && safeBaseInfo->isValid())
				{
					info.m_nike[j] = safeBaseInfo->getResource()->m_nike;
					info.m_headUrl[j] = safeBaseInfo->getResource()->m_headImageUrl;
				}
			}
		}
		LLOG_DEBUG("HanderDBReqPokerLog:m_count[%d]", msg->m_count);
		user->Send(log);
	}
}


void CUserMessage::HanderDBReqRoomLog( LMsgLBD2LReqRoomLog* msg )
{
	class ItemLog
	{
	public:
		~ItemLog()
		{
			for(auto it = m_log.begin(); it != m_log.end(); ++it)
			{
				if(*it)
				{
					delete *it;
					*it = NULL;
				}
			}
			m_log.clear();
		}
		void FromString(const Lstring& str)
		{
			std::vector<Lstring> str1, str2;

			if ( str.find( "|" ) != std::string::npos )
			{
				L_ParseString(str, str2, "|");
				for(size_t j = 0 ; j < str2.size(); ++j)
				{
					str1.clear();
					L_ParseString(str2[j], str1, ";");
					if ( str1.size() >= 8 )
					{
						VipDeskLog* log = new VipDeskLog();

						sscanf( str1[0].c_str(), "%d,%d,%d,%d", &(log->m_gold[0]), &(log->m_gold[1]), &(log->m_gold[2]), &(log->m_gold[3]) );
						sscanf( str1[1].c_str(), "%d,%d,%d,%d", &(log->m_mgang[0]), &(log->m_mgang[1]), &(log->m_mgang[2]), &(log->m_mgang[3]) );
						sscanf( str1[2].c_str(), "%d,%d,%d,%d", &(log->m_angang[0]), &(log->m_angang[1]), &(log->m_angang[2]), &(log->m_angang[3]) );
						log->m_zhuangPos = atoi(str1[3].c_str());
						log->m_time = atoi(str1[4].c_str());
						log->m_videoId = str1[5].c_str();
						sscanf( str1[6].c_str(), "%d,%d,%d,%d", &(log->m_checkTing[0]), &(log->m_checkTing[1]), &(log->m_checkTing[2]), &(log->m_checkTing[3]) );
						sscanf( str1[7].c_str(), "%d,%d,%d,%d", &(log->m_zimo[0]), &(log->m_zimo[1]), &(log->m_zimo[2]), &(log->m_zimo[3]) );
						sscanf( str1[8].c_str(), "%d,%d,%d,%d", &(log->m_bomb[0]), &(log->m_bomb[1]), &(log->m_bomb[2]), &(log->m_bomb[3]) );
						sscanf( str1[9].c_str(), "%d,%d,%d,%d", &(log->m_win[0]), &(log->m_win[1]), &(log->m_win[2]), &(log->m_win[3]) );
						sscanf( str1[10].c_str(), "%d,%d,%d,%d", &(log->m_dzimo[0]), &(log->m_dzimo[1]), &(log->m_dzimo[2]), &(log->m_dzimo[3]) );
						sscanf( str1[11].c_str(), "%d,%d,%d,%d", &(log->m_dbomb[0]), &(log->m_dbomb[1]), &(log->m_dbomb[2]), &(log->m_dbomb[3]) );
						sscanf( str1[12].c_str(), "%d,%d,%d,%d", &(log->m_dwin[0]), &(log->m_dwin[1]), &(log->m_dwin[2]), &(log->m_dwin[3]) );
						m_log.push_back(log);
					}
				}
			}
		}
	public:
		std::vector<VipDeskLog*> m_log;//姣忎竴鎶婄殑璁板綍 
	};

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline())
	{
		ItemLog item;
		item.FromString(msg->m_data);

		LMsgS2CRoomLog log;
		log.m_size = item.m_log.size();
		log.m_flag = msg->m_flag;

		for(size_t j = 0; j < item.m_log.size(); ++j)
		{
			if(j < 32)
			{
				VipLog& l = log.m_item[j];
				VipDeskLog* dl = item.m_log[j];
				memcpy(l.m_score,dl->m_gold,sizeof(l.m_score));
				l.m_time = dl->m_time;
				l.m_videoId = dl->m_videoId;
				//printf("=%d %d %d\n",l.m_time,l.m_score[0],l.m_videoId);
			}
		}

		user->Send(log);
	}
}

void CUserMessage::HanderDBReqVideo(LMsgLDB2LReqVideo* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline() && msg->m_count)
	{
		LMsgS2CVideo video;
		video.m_video = msg->m_video;
		video.m_video.FromString(video.m_video.m_str);
		video.m_video.PlayTypeFromString(video.m_video.m_str_playtype);
		video.m_deskId = msg->m_video.m_deskId;
		video.m_curCircle = msg->m_video.m_curCircle;
		video.m_maxCircle = msg->m_video.m_maxCircle;
		video.m_zhuang = msg->m_video.m_zhuang;
		video.m_flag = msg->m_video.m_flag;
		memcpy(video.m_score,msg->m_video.m_score,sizeof(video.m_score));

		for (Lint i = 0; i < 4; ++i)
		{
			if ( msg->m_video.m_userId[i] == 0 )
			{
				continue;
			}

			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_video.m_userId[i]);
			if(safeBaseInfo && safeBaseInfo->isValid())
			{
				video.m_nike[i]		= safeBaseInfo->getResource()->m_nike;
				video.m_sex[i]		= safeBaseInfo->getResource()->m_sex;
				video.m_imageUrl[i] = safeBaseInfo->getResource()->m_headImageUrl;
			}
		}

		user->Send(video);
	}
}

void CUserMessage::HanderDBRetBindingRelation( LMsgLDB2LMGBindingRelationships* msg )
{
}

void CUserMessage::HanderDBRetInfo(LMsgLDB_2_LM_RetInfo* msg)
{
}

void CUserMessage::HanderDBUserMsg(LMsgLDB2LMGUserMsg* msg)
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(msg->m_strUUID);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}
	boost::shared_ptr<User> user = safeUser->getResource();
	user->Send( msg->m_dataBuff );
}

void CUserMessage::HanderDBReqCRELogHis( LMsgLDB2LM_RecordCRELog *msg )
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userId);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline())
	{
		LMsgS2CCRELogRet ret;

		//鏌ユ壘鐜╁鏄电О
		for (Lint j = 0; j < LMsgLDB2LM_RecordCRELog::Length; ++j)
		{
			if(msg->m_user[j] == 0)
				continue;

			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_user[j]);
			if(safeBaseInfo && safeBaseInfo->isValid())
			{
				auto tmp = safeBaseInfo->getResource();
				UserBaseInfo_s info;
				info.m_strNike = tmp->m_nike;
				info.m_userId = msg->m_user[j];
				info.m_strHeadUrl = tmp->m_headImageUrl;
				info.m_sex = tmp->m_sex;
			
				ret.m_info.push_back(info);
			}
			else
			{
				LLOG_DEBUG("##Warnning ReqCRELog not find the user = %d",msg->m_user[j]);
			}
		}

		for(auto it = msg->m_record.begin(); it != msg->m_record.end(); ++it)
		{
			ret.m_record.push_back(*it);
		}

		user->Send(ret);
	}
}

void CUserMessage::HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg)
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyUserCoinsNum user not exiest, userid=%d, count=%d, operType=%d",  msg->m_userid, msg->coinsNum, msg->operType);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();

	if(msg->isAddCoins)
	{
		user->AddCoinsCount(msg->coinsNum, msg->operType);
	}
	else
	{
		user->DelCoinsCount(msg->coinsNum, msg->operType);
	}
}

void CUserMessage::HanderDBRetShareVideo(LMsgLDB2LRetShareVideo* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline() && msg->m_count)
	{
		LMsgS2CShareVideo video;
		video.m_errorId = 1;
		video.m_count = msg->m_count;
		for(int x=0;x<msg->m_count;x++)
		{
			S2CShareVideo the_video;
			the_video.m_videoId = msg->m_info[x].m_videoId;
			the_video.m_time = msg->m_info[x].m_time;
			the_video.m_user_count = msg->m_info[x].m_user_count;
			for(int i=0;i<msg->m_info[x].m_user_count;i++)
			{
				boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_info[x].m_userId[i]);
				if(safeBaseInfo && safeBaseInfo->isValid())
				{
					auto tmp = safeBaseInfo->getResource();
					the_video.m_userNike[i] = tmp->m_nike;
				}
				the_video.m_score[i] = msg->m_info[x].m_score[i];
			}
			video.m_vec_share_video.push_back(the_video);
		}
		user->Send(video);
	}
	else{
		LMsgS2CShareVideo video;
		video.m_errorId = 0;
		user->Send(video);
	}
}

void CUserMessage::HanderDBGetShareVideoID(LMsgLDB2LRetShareVideoId* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userId);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> the_user = safeUser->getResource();
	LMsgS2CRetShareVideoId share_Id;
	if(the_user && msg->m_shareId ==-1)
	{
		if(the_user)
		{
			share_Id.m_errorId = 2;
			the_user->Send(share_Id);
		}
	}
	else if(msg->m_shareId !=0)
	{
		share_Id.m_shareId = msg->m_shareId;
		if(the_user)
		{
			share_Id.m_errorId = 0;
			the_user->Send(share_Id);
		}
	}
	else
	{
		share_Id.m_shareId = gDeskManager.GetShareVideoId();
		if(the_user)
		{
			share_Id.m_errorId = 0;
			the_user->Send(share_Id);
		}
		LMsgL2LDBSaveShareVideoId save_id;
		save_id.m_userId = msg->m_userId;
		save_id.m_shareId = share_Id.m_shareId ;
		save_id.m_videoId = msg->m_videoId;
		gWork.SendMsgToDb(save_id);
		LLOG_INFO("save share video id:%s,shareID :%d",msg->m_videoId.c_str(),share_Id.m_shareId);
	}
}


//杩斿洖瀹㈡埛绔綍鍍忕粨鏋?
void CUserMessage::HanderDBSharedVideoResult(MHLMsgLDB2LShareVideoResult * msg)
{
	if (msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if (user && user->GetOnline() && msg->m_count)
	{
		LMsgS2CVideo video;
		video.m_video = msg->m_video;
		video.m_video.FromString(video.m_video.m_str);
		video.m_video.PlayTypeFromString(video.m_video.m_str_playtype);
		video.m_deskId = msg->m_video.m_deskId;
		video.m_curCircle = msg->m_video.m_curCircle;
		video.m_maxCircle = msg->m_video.m_maxCircle;
		video.m_zhuang = msg->m_video.m_zhuang;
		video.m_flag = msg->m_video.m_flag;
		memcpy(video.m_score, msg->m_video.m_score, sizeof(video.m_score));

		for (Lint i = 0; i < 4; ++i)
		{
			if (msg->m_video.m_userId[i] == 0)
			{
				continue;
			}

			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_video.m_userId[i]);
			if (safeBaseInfo && safeBaseInfo->isValid())
			{
				video.m_nike[i] = safeBaseInfo->getResource()->m_nike;
				video.m_sex[i] = safeBaseInfo->getResource()->m_sex;
				video.m_imageUrl[i] = safeBaseInfo->getResource()->m_headImageUrl;
			}
		}

		user->Send(video);
		MHLOG("****Send user share video result userid:%d uuid:%s videoid:%s", user->GetUserDataId(), user->m_userData.m_unioid.c_str(), video.m_video.m_Id.c_str());
	}
}

void CUserMessage::MHHanderNewAgencyActivityUpdatePlayCount(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg)
{
	if (msg == NULL)return;

	LLOG_DEBUG("MHHanderNewAgencyActivityUpdatePlayCount flag[%d]", msg->m_flag);

	if (msg->m_flag == AgencyActivityUpdatePlayCountType_ChangePoint)
	{
		gClubManager.updateClubUserPointFromLogic(msg);
		boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_clubId);
		if (safeClub.get() != NULL && safeClub->isValid())
		{
			boost::shared_ptr<Club> club = safeClub->getResource();
			msg->m_deskId = club->getClubSq();
		}

	}

	gWork.SendToCenter((*msg));

	return;
}

LTime& CUserMessage::GetCurTime()
{
	return m_tickTimer;
}

//俱乐部会员入座，更新俱乐部桌子
//void  CUserMessage::HanderUserAddClubDesk(LMsgL2LMGUserAddClubDesk*msg)
//{
//	LLOG_DEBUG("CUserMessage::HanderUserAddClubDesk");
//	gUserManager.UserAddClubDesk(msg);
//}

//俱乐部会员离开桌子，更新俱乐部桌子
//void   CUserMessage::HanderUserLeaveClubDesk(LMsgL2LMGUserLeaveClubDesk* msg)
//{
//	LLOG_DEBUG("CUserMessage::HanderUserLeaveClubDesk");
//	gUserManager.UserLeaveClubDesk(msg);
//}

void CUserMessage::HanderDBQiPaiReqRoomLog(LMsgLBD2LReqRoomLog* msg)
{
	LLOG_DEBUG("HanderDBQiPaiReqRoomLog");
	class ItemLog
	{
	public:
		~ItemLog()
		{
			for (auto it = m_log.begin(); it != m_log.end(); ++it)
			{
				if (*it)
				{
					delete *it;
					*it = NULL;
				}
			}
			m_log.clear();
		}
		void FromString(const Lstring& str)
		{
			std::vector<Lstring> str1, str2,str3;

			if (str.find("|") != std::string::npos)
			{
				L_ParseString(str, str2, "|");
				for (size_t j = 0; j < str2.size(); ++j)
				{
					LLOG_DEBUG("str2[%d]=[%s]",j,str2[j].c_str());
					str1.clear();
					L_ParseString(str2[j], str1, ";");
					
					//if (str1.size() >= 8)
					if(str1.size()>=3)
					{
						LLOG_DEBUG("str1[0]=[%s],str1[1]=[%s],str1[2]",str1[0].c_str(),str1[1].c_str(), str1[2].c_str());
						QiPaiLog* log = new QiPaiLog();

						log->m_time = atoi(str1[0].c_str());
						log->m_videoId = str1[1];

						L_ParseString(str1[2], str3, ",");

						for (size_t k = 0; k < str3.size() && k<MAX_CHAIR_COUNT; ++k)
						{
							log->m_score[k] = atoi(str3[k].c_str());
						}

						//sscanf(str1[1].c_str(), "%d,%d,%d", &(log->m_score[0]), &(log->m_score[1]), &(log->m_score[2]));
						//sscanf(str1[1].c_str(), "%d,%d,%d,%d", &(log->m_mgang[0]), &(log->m_mgang[1]), &(log->m_mgang[2]), &(log->m_mgang[3]));
						//sscanf(str1[2].c_str(), "%d,%d,%d,%d", &(log->m_angang[0]), &(log->m_angang[1]), &(log->m_angang[2]), &(log->m_angang[3]));
						//log->m_zhuangPos = atoi(str1[3].c_str());
						//log->m_time = atoi(str1[4].c_str());
						//log->m_videoId = str1[5].c_str();
						//sscanf(str1[6].c_str(), "%d,%d,%d,%d", &(log->m_checkTing[0]), &(log->m_checkTing[1]), &(log->m_checkTing[2]), &(log->m_checkTing[3]));
						//sscanf(str1[7].c_str(), "%d,%d,%d,%d", &(log->m_zimo[0]), &(log->m_zimo[1]), &(log->m_zimo[2]), &(log->m_zimo[3]));
						//sscanf(str1[8].c_str(), "%d,%d,%d,%d", &(log->m_bomb[0]), &(log->m_bomb[1]), &(log->m_bomb[2]), &(log->m_bomb[3]));
						//sscanf(str1[9].c_str(), "%d,%d,%d,%d", &(log->m_win[0]), &(log->m_win[1]), &(log->m_win[2]), &(log->m_win[3]));
						//sscanf(str1[10].c_str(), "%d,%d,%d,%d", &(log->m_dzimo[0]), &(log->m_dzimo[1]), &(log->m_dzimo[2]), &(log->m_dzimo[3]));
						//sscanf(str1[11].c_str(), "%d,%d,%d,%d", &(log->m_dbomb[0]), &(log->m_dbomb[1]), &(log->m_dbomb[2]), &(log->m_dbomb[3]));
						//sscanf(str1[12].c_str(), "%d,%d,%d,%d", &(log->m_dwin[0]), &(log->m_dwin[1]), &(log->m_dwin[2]), &(log->m_dwin[3]));
						m_log.push_back(log);
					}
					else
					{
						LLOG_ERROR("HanderDBQiPaiReqRoomLog  str1.size < 2");
					}
				}
			}
		}
	public:
		std::vector<QiPaiLog*> m_log;//每一把的记录 
	};

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if (user && user->GetOnline())
	{
		ItemLog item;
		LLOG_DEBUG("HanderDBQiPaiReqRoomLog DATA=[%s]", msg->m_data.c_str());
		item.FromString(msg->m_data);

		LMsgS2CMatchLog log;
		log.m_size = item.m_log.size();
		log.m_flag = msg->m_flag;

		LLOG_DEBUG("*******************HanderDBQiPaiReqRoomLog**************************");
		LLOG_DEBUG("kSzie=[%d], kFlag=[%d]", log.m_size, log.m_flag);

		for (size_t j = 0; j < item.m_log.size(); ++j)
		{
			if (j < 40)
			{
				QiPaiLog& l = log.m_item[j];
				QiPaiLog* dl = item.m_log[j];
				memcpy(l.m_score, dl->m_score, sizeof(l.m_score));
				l.m_time = dl->m_time;
				l.m_videoId = dl->m_videoId;
				
				LLOG_DEBUG("logItem[%d]={kTime=[%d], kUserCount=[%d], kVideoId=[%s], kScore=[%d,%d,%d,%d,%d]}", 
					j, l.m_time, l.m_userCount, l.m_videoId, l.m_score[0], l.m_score[1], l.m_score[2], l.m_score[3], l.m_score[4] );
			}
		}
		user->Send(log);
	}
}

//扑克返回回放
void CUserMessage::HanderDBQiPaiReqVideo(LMsgLDB2LPokerReqVideo* msg)
{
	LLOG_DEBUG("HanderDBQiPaiReqVideo");
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("HanderDBQiPaiReqVideo userid=[%d]",msg->m_userid);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("HanderDBQiPaiReqVideo ERROR to find user. id:%d", msg->m_userid);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if (user && user->GetOnline() && msg->m_count)
	{
		LMsgS2CPokerVideo video;
		LLOG_DEBUG("HanderDBQiPaiReqVideo m_str[%s]", video.m_video.m_str);
		LLOG_DEBUG("HanderDBQiPaiReqVideo m_str_playtype[%s]", video.m_video.m_str_playtype);
		video.m_video = msg->m_video;
		video.m_video.FromString(video.m_video.m_str, msg->m_video.m_flag);
		video.m_video.FillUserHandCards();
		video.m_video.PlayTypeFromString(video.m_video.m_str_playtype);
		video.m_deskId = msg->m_video.m_deskId;
		video.m_curCircle = msg->m_video.m_curCircle + 1;
		video.m_maxCircle = msg->m_video.m_maxCircle;
		video.m_zhuang = msg->m_video.m_zhuang;
		video.m_fuZhuang = msg->m_video.m_fuZhuang;
		video.m_flag = msg->m_video.m_flag;
		memcpy(video.m_score, msg->m_video.m_score, sizeof(video.m_score));


		for (Lint i = 0; i <QIPAI_VIDEO_USER_MAX; ++i)
		{
			if (msg->m_video.m_userId[i] == 0)
			{
				continue;
			}
			if (msg->m_video.m_userId[i] == user->GetUserDataId())
				video.m_pos = i;

			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_video.m_userId[i]);
			if (safeBaseInfo && safeBaseInfo->isValid())
			{
				video.m_nike[i] = safeBaseInfo->getResource()->m_nike;
				video.m_sex[i] = safeBaseInfo->getResource()->m_sex;
				video.m_imageUrl[i] = safeBaseInfo->getResource()->m_headImageUrl;
			}
		}

		LLOG_DEBUG("video.m_video.m_str: %s", video.m_video.m_str.c_str());
		LLOG_DEBUG("video.m_video.m_flag: %d", video.m_video.m_flag);
		LLOG_DEBUG("video.m_video.m_deskid: %d", video.m_video.m_deskId);
		LLOG_DEBUG("video.m_video.m_curCircle: %d", video.m_video.m_curCircle);
		LLOG_DEBUG("video.m_video.m_playtpe.size(): %d", video.m_video.m_playType.size());
		user->Send(video);
	}

	LLOG_DEBUG("HanderDBQiPaiReqVideo END");
}

//更新俱乐部桌子信息
//void   CUserMessage::HanderUserFreshClubInfo(LMsgL2LMGFreshDeskInfo* msg)
//{
//	LLOG_DEBUG("CUserMessage::HanderUserLeaveClubDesk");
//	gUserManager.FreshDeskInfo(msg);
//
//}

//用户离开俱乐部桌子
//void   CUserMessage::HanderUserListLeaveClubDesk(LMsgL2LMGUserListLeaveClubDesk* msg)
//{
//	LLOG_DEBUG("CUserMessage::HanderUserListLeaveClubDesk");
//	gUserManager.UserListLeaveClubDesk(msg);
//
//
//}