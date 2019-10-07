#include "UserMessage.h"
#include "LTime.h"
#include "LMemoryRecycle.h"
#include "Config.h"
#include "Work.h"
#include "UserManager.h"
#include "NewUserVerifyManager.h"
#include "DbServerManager.h"
#include "HttpRequest.h"
#include "clubManager.h"

#define GateDomainName	"GateDomainName"
#define NoGateDomainName "NoGateDomainName"
#define GateVipIp   "GateVipIp"


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
			//Tick(cur);
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

			//每一万个包输出所用时间
			iMsgCount++;
			if(iMsgCount == 10000)
			{
				LTime now;
				LLOG_ERROR("User Logic: 10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

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
	//LLOG_DEBUG("CUserMessage::HanderMsg  msg->msgId = %ld ", msg->m_msgId);
	switch(msg->m_msgId)
	{
	case MSG_FROM_LOGINSERVER:
		HanderFromLoginServer((LMsgFromLoginServer*)msg);
		break;
	case MSG_NEWUSER_VERIFY_REPLY:
		HanderNewUserVerifyReply((LMsgNewUserVerifyReply*)msg);
		break;
	case MSG_L_2_CE_MODIFY_USER_NEW:
		HanderModifyUserNew((LMsgL2CeModifyUserNew*) msg);
		break;

	case MSG_LMG_2_CE_UPDATE_COIN:
		HanderUpdateUserCoin((LMsgL2CeUpdateCoin*)msg);
		break;

	case MSG_L_2_LMG_UPDATE_USER_GPS:
	case MSG_LMG_2_CE_UPDATE_USER_GPS:
		HanderUpdateUserGps((LMsgLMG2CEUpdateUserGPS*)msg);
		break;


	case MSG_LMG_2_CE_UPDATE_COIN_JOIN_DESK:
		HanderUpdateUserCoinJoinDesk((LMsgL2CeUpdateCoinJoinDesk*)msg);
		break;

	case MSG_L_2_CE_USER_LOGIN:
		HanderLogicUserLogin((LMsgL2CeUserServerLogin*) msg);
		break;
	case MSG_L_2_CE_USER_LOGOUT:
		HanderLogicUserLogout((LMsgL2CeUserServerLogout*) msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_CARD:
		HanderModifyCard((LMsgL2LMGModifyCard*) msg);
		break;
	case MSG_L_2_LMG_ADD_USER_PLAYCOUNT:
		HanderAddUserPlayCount((LMsgL2LMGAddUserPlayCount*) msg);
		break;
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		HanderModifyUserCoinsNum((LMsgCN2LMGModifyUserCoins*) msg);
		break;
	case MSG_C_2_C_ADD_CRE:
		HanderModifyUserCreditValue((LMsgC2C_ADD_CRE*) msg);
		break;
	case MSG_LMG_2_L_EXCH_CARD:
		HanderExchangeCard((LMsgL2LMGExchCard*) msg);
		break;
	case MSG_CE_2_LMG_SQL_INFO:
	{
		LLOG_DEBUG("MSG_CE_2_LMG_SQL_INFO");
		HanderSqlInfo((LMsgLMG2CeSqlInfo*)msg);
		break;
	}
	case MSG_CE_2_LMG_UPDATE_POINT_RECORD:
	{
		LLOG_DEBUG("MSG_CE_2_LMG_UPDATE_POINT_RECORD");
		gClubManager.updatePointRecord((LMsgLMG2CEUpdatePointRecord*)msg);
		//HanderUpdatePointRecord((LMsgLMG2CEUpdatePointRecord*)msg);
		break;
	}
	case MSG_CE_2_LMG_ADD_CLUB:
	{
		LLOG_DEBUG("MSG_CE_2_LMG_ADD_CLUB");
		HanderAddClub((LMsgCe2LAddClub*)msg);
		break;
	}
	case MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE:
	{
		LLOG_DEBUG("MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE");
		HanderClubAddPlayType((LMsgCe2LMGClubAddPlayType*)msg);
		break;
	}
	case MSG_LMG_2_CE_POINT_RECORD_LOG:
	{
		LLOG_DEBUG("MSG_LMG_2_CE_POINT_RECORD_LOG");
		gClubManager.userRequestRecordLog((LMsgLMG2CERecordLog*)msg);
		break;
	}
	case MSG_CE_2_LMG_UPDATE_CLUB:
	{
		LLOG_DEBUG("MSG_CE_2_LMG_UPDATE_CLUB");
		gClubManager.updateClubInfo((LMsgCE2LMGClubInfo*)msg);
		break;
	}
	case MSG_CE_2_LMG_UPDATE_PLAY:
	{
		LLOG_DEBUG("MSG_CE_2_LMG_UPDATE_PLAY");
		gClubManager.updatePlayInfo((LMsgCE2LMGPlayInfo*)msg);
		break;
	}

	default:
		break;
	}
}

void CUserMessage::HanderFromLoginServer(LMsgFromLoginServer* msg)
{
	if (msg == NULL || ( msg->m_userMsgId != MSG_C_2_S_MSG && msg->m_userMsgId != MSG_C_2_S_PHONE_LOGIN) )
	{
		return;
	}
	if (msg->m_userMsgId == MSG_C_2_S_PHONE_LOGIN)
	{
		HanderUserPhoneLogin((LMsgC2SPhoneLogin*)msg->m_userMsg, msg->m_sp, msg->m_gaofang_flag);
	}
	else if (msg->m_userMsgId == MSG_C_2_S_MSG)
	{
		HanderUserLogin((LMsgC2SMsg*)msg->m_userMsg, msg->m_sp, msg->m_gaofang_flag);
	}
}

//处理玩家登陆的消息 
//login_flag: 0 : normal 1:高防
void CUserMessage::HanderUserLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag)
{
	if (msg == NULL)
	{
		return;
	}
	if(msg->m_plate == "wechat")
	{
		//HanderUserWechatLogin(msg, gameloginSP, login_flag);
		HanderUserWechatLoginFirst(msg, gameloginSP, login_flag);
	}
	else if(gConfig.GetLocalLogin()=="true" && msg->m_plate == "local")
	{
		HanderUserLocalLogin(msg, gameloginSP, login_flag);
	}
	else
	{
		LLOG_ERROR("CUserMessage::HanderUserLogin plate error %s",msg->m_plate.c_str());
	}
}

void CUserMessage::HanderUserWechatLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag)
{
	if (msg == NULL)
	{
		return;
	}
	Lint errorCode = 1;
	std::string errorMsg;
	do
	{
		if (!gWork.isLogicManagerExist(msg->m_severID))
		{
			LLOG_ERROR("CUserMessage::HanderUserWechatLogin logicid not exist %d", msg->m_severID);
			errorCode = 6;
			errorMsg = "logicid not exist";
			break;
		}

		if (msg->m_uuid.empty() || msg->m_uuid.size()>32)
		{
			LLOG_ERROR("CUserMessage::HanderUserWechatLogin uuid error, uuid=%s", msg->m_uuid.c_str());
			errorCode = 7;
			errorMsg = "uuid error";
			break;
		}
		boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_uuid);
		boost::shared_ptr<DUser> user;
		//长沙麻将openid 转 uuid 的方案
		if (safeUser.get() == NULL)
		{
			safeUser = gUserManager.getUser(msg->m_openId);
			if (safeUser)
			{
				user = safeUser->getUser();
				user->m_usert.m_unioid = msg->m_uuid;

				gUserManager.delUser(msg->m_openId);
				gUserManager.addUser(user);

				//更新数据库
				std::stringstream ss;
				ss << "UPDATE User SET ";
				ss << "UnionId='" << user->m_usert.m_unioid << "' WHERE UnionId='";
				ss << msg->m_uuid << "'";

				LLOG_ERROR("CUserMessage::HanderUserWechatLogin sql =%s", ss.str().c_str());

				Lstring* sql = new Lstring(ss.str());
				gDbServerManager.Push(sql, user->m_usert.m_id);
			}
		}
		else
		{
			user = safeUser->getUser();
		}

		//if(user && m_tickTimer.Secs() - user->m_usert.m_lastLoginTime < 24*60*60)
		if (user.get() == NULL)
		{
			//处理性别值异常
			if (msg->m_sex < 1 || msg->m_sex > 2)
			{
				msg->m_sex = 2;
			}

			//昵称中的单引号处理
			for (Lsize i = 0; i < msg->m_nikename.size(); ++i)
			{
				if (msg->m_nikename[i] == '\'')
					msg->m_nikename[i] = ' ';
			}

			DUser* newUser = new DUser;
			newUser->m_usert.m_openid = msg->m_openId;
			newUser->m_usert.m_unioid = msg->m_uuid;
			newUser->m_usert.m_nike = msg->m_nikename;
			newUser->m_usert.m_sex = msg->m_sex;
			newUser->m_usert.m_headImageUrl = msg->m_imageUrl;
			newUser->m_usert.m_regTime = m_tickTimer.Secs();
			newUser->m_usert.m_lastLoginTime = m_tickTimer.Secs();
			// 验证新用户登陆
			LMsgNewUserVerifyRequest* req = new LMsgNewUserVerifyRequest;
			req->m_login_flag = login_flag;
			req->m_pUser = newUser;
			req->m_gameloginSP = gameloginSP;
			req->m_severID = msg->m_severID;
			req->m_bWechatLogin = true;
			req->m_openId = msg->m_openId;
			req->m_unionId = msg->m_uuid;
			req->m_accessToken = msg->m_accessToken;
			LLOG_ERROR("new user request    newUser->m_uniond[%s]   msg->uuid[%s]", newUser->m_usert.m_unioid.c_str(), msg->m_uuid.c_str());
			gDbNewUserManager.Push(msg->m_uuid, req);
			return;	// 直接返回
		}
		else
		{
			//昵称中的单引号处理
			for (Lsize i = 0; i < msg->m_nikename.size(); ++i)
			{
				if (msg->m_nikename[i] == '\'')
					msg->m_nikename[i] = ' ';
			}

			if ((!msg->m_nikename.empty() && user->m_usert.m_nike != msg->m_nikename) ||                 //昵称
				user->m_usert.m_sex != msg->m_sex ||                  //性别
				user->m_usert.m_headImageUrl != msg->m_imageUrl ||    //头像
				user->m_usert.m_openid != msg->m_openId)      //头像
			{
				user->m_usert.m_nike = msg->m_nikename;
				user->m_usert.m_sex = msg->m_sex;
				user->m_usert.m_headImageUrl = msg->m_imageUrl;
				user->m_usert.m_unioid = msg->m_uuid;
				user->m_usert.m_openid = msg->m_openId;

				UpdateUserInfo(user->m_usert);
			}
		}

		errorCode = 0;
		errorMsg = "";

	} while (false);


	SendUserLogin(msg->m_uuid, gameloginSP, msg->m_severID, errorCode, errorMsg, login_flag);
}

void CUserMessage::HanderUserLocalLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag)
{
	if (msg == NULL)
	{
		return;
	}

	int errorCode = 1;
	std::string errorMsg;
	do
	{
		msg->m_uuid = msg->m_openId;
		if (!gWork.isLogicManagerExist(msg->m_severID))
		{
			LLOG_ERROR("CUserMessage::HanderUserLocalLogin logicid not exist %d", msg->m_severID);
			errorCode = 6;
			errorMsg = "logicid not exist";
			break;
		}

		if (msg->m_uuid.empty() || msg->m_uuid.size() > 32)
		{
			LLOG_ERROR("CUserMessage::HanderUserLocalLogin uuid error, uuid=%s", msg->m_uuid.c_str());
			errorCode = 2;
			errorMsg = "uuid error";
			break;
		}

		boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_uuid);
		boost::shared_ptr<DUser> user;

		if (safeUser.get() == NULL)
		{
			DUser* newUser = new DUser;
			newUser->m_usert.m_openid = msg->m_openId;
			newUser->m_usert.m_unioid = msg->m_uuid;
			//newUser->m_usert.m_nike = msg->m_nike;
			newUser->m_usert.m_nike = msg->m_nikename;
			newUser->m_usert.m_sex = msg->m_sex;
			newUser->m_usert.m_headImageUrl = msg->m_imageUrl;
			newUser->m_usert.m_regTime = m_tickTimer.Secs();
			newUser->m_usert.m_lastLoginTime = m_tickTimer.Secs();
			// 验证新用户登陆
			LMsgNewUserVerifyRequest* req = new LMsgNewUserVerifyRequest;
			req->m_login_flag = login_flag;
			req->m_pUser = newUser;
			req->m_gameloginSP = gameloginSP;
			req->m_severID = msg->m_severID;
			req->m_openId = msg->m_openId;
			req->m_unionId = msg->m_uuid;
			LLOG_ERROR("new user request    newUser->m_uniond[%s]   msg->uuid[%s]", newUser->m_usert.m_unioid.c_str(), msg->m_uuid.c_str());
			gDbNewUserManager.Push(msg->m_uuid, req);

			return;	// 直接返回
		}
		else
		{
			user = safeUser->getUser();
		}

		errorCode = 0;
		errorMsg = "";

	} while (false);


	SendUserLogin(msg->m_uuid, gameloginSP, msg->m_severID, errorCode, errorMsg, login_flag);
}

void CUserMessage::HanderNewUserVerifyReply(LMsgNewUserVerifyReply* msg)
{
	if(!msg)		return;
	DUser* user = (DUser*)msg->m_pUser;
	if (!user)		return;

	boost::shared_ptr<DUser> newUser(user);
	LMsgS2CMsg ret;
	ret.m_errorCode = msg->m_errorCode;
	if ( msg->m_errorCode == 0 )
	{
		gUserManager.addUser( newUser );
	}
	LLOG_ERROR("new user reply  newUser->m_id[%d]   newUser->m_uniond[%s]", user->m_usert.m_id, user->m_usert.m_unioid.c_str());
	SendUserLogin( user->m_usert.m_unioid, msg->m_gameloginSP, msg->m_severID, msg->m_errorCode, msg->m_errorMsg, msg->m_login_flag);
}

void CUserMessage::SendUserLogin( const Lstring& uuid, LSocketPtr gameloginSP, Lint severID, Lint errorCode, const Lstring& errorMsg, Lint login_flag)
{
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(uuid);
	boost::shared_ptr<DUser> user;

	LLOG_DEBUG("****Suijun SendUserLogin  login_flag = %d", login_flag);
	if ( safeUser )
	{
		user = safeUser->getUser();
		//for 给玩家添加金币
		Lint goldNum = 0;
		Lstring userIp, userGps;
		Lint blocked = 0;
		bool bGet = gUserManager.loadUserInfoFromDBByUserId(user->m_usert.m_id, goldNum, userIp, userGps, blocked);
		if (bGet) {
			user->m_usert.m_numOfCard2s = goldNum;
			user->m_usert.m_customString1 = userIp;
			//user->m_usert.m_customString2 = userGps;
			user->m_usert.m_customInt[3] = blocked;  //loadUserInfoFromDBByUserId
		}
		//end 给玩家添加金币
	}


	LMsgS2CMsg ret;
	ret.m_errorCode = errorCode;
	ret.m_errorMsg = errorMsg;
	while ( user && errorCode == 0 )
	{
		if (user->m_logicID != severID && user->m_logicID != 0)
		{
			//不是在本服务器登陆的玩家, 发送消息，提示退出原APP
			LLOG_ERROR("CUserMessage::SendUserLogin logicid not empty %d : %d", severID, user->m_logicID);
			ret.m_errorCode = 5;
			ret.m_errorMsg = string_To_UTF8(gWork.getServerNameByID(user->m_logicID));
			break;
		}
		user->m_lastReqTime = m_tickTimer.Secs();

		LMsgCe2LUserLogin send;
		send.m_seed = L_Rand(10000000, 99999999);
		send.user = user->m_usert;
		//LLOG_ERROR("CE2LM login usert m_id[%d] new[%d] m_numsCard2[%d] m_unionid[%s] msg usert id[%d] new[%d] card2[%d] m_unionid[%s] seed[%d]",
		//	user->m_usert.m_id, user->m_usert.m_new, user->m_usert.m_numOfCard2s, user->m_usert.m_unioid.c_str(),
		//	send.user.m_id, send.user.m_new, send.user.m_numOfCard2s, send.user.m_unioid.c_str(), send.m_seed);
		gWork.SendMsgToLogic(send, severID);

		//GateInfo  gateInfo=gWork.GetGateUsefulInfo(severID);
		//ret.m_gateIp=gateInfo.m_ip;
		/*
		Lstring vipIp;
		if (!m_RedisClient.readDataByString(GateVipIp, vipIp))
		{
			LLOG_ERROR("CUserMessage::SendUserLogin get  GateVipIp from redis is failed, now set is empty");
		}

		Lstring compareIp;
		if (vipIp.empty())
		{
			compareIp.assign("47.93.174.45");
		}
		else
			compareIp=vipIp;

		LLOG_DEBUG("Logwyz   ........  compareIp=[%s],m_gateIpSelect=[%d]", compareIp.c_str(), gWork.m_gateIpSelect);
		Lint gateInfoSize=gWork.getGateInfoSize(severID);
		GateInfo  gateInfo;
		int i=0;
		for(;i<gateInfoSize;i++)
		{
			gateInfo=gWork.GetGateUsefulInfo(severID);
			if (1==gWork.m_gateIpSelect)
			{
				if (gateInfo.m_ip.compare(compareIp)==0)
					break;
				else
					continue;
			}
			else if (2==gWork.m_gateIpSelect)
			{
				if (gateInfo.m_ip.compare(compareIp)==0)
					continue;
				else
					break;
			}
			else
				break;
		}
		ret.m_gateIp=gateInfo.m_ip;
		LLOG_DEBUG("Logwyz ...........ret.m_gateIp=[%s] i=[%d]", ret.m_gateIp.c_str(),i);

		if (!m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
		{
			return;
		}
		

		Lstring strGateDomainName;
		if (!m_RedisClient.readDataByString(GateDomainName, strGateDomainName))
		{
			LLOG_ERROR("Work::_initRedisAndActive ActiveFreeSet get from redis is failed, now set is empty");
		}
		else {
			if (strGateDomainName != "") 
			{
				
				Lstring noDomainName;
				if (!m_RedisClient.readDataByString(NoGateDomainName, noDomainName))
				{
					ret.m_gateIp=strGateDomainName;
					LLOG_ERROR("Work::_initRedisAndActive ActiveFreeSet get   NoGateDomainName from redis is failed, now set is empty");
				}
				else
				{
					if(noDomainName.compare(ret.m_gateIp)!=0)
						ret.m_gateIp=strGateDomainName;
				}
			}
		}
		LLOG_DEBUG("Logwyz  .....................ip = [%s]", ret.m_gateIp.c_str());

		ret.m_gatePort = gateInfo.m_port;
		if ( gateInfo.m_id == 0 && gateInfo.m_ip.empty() )
		{
			LLOG_ERROR("CUserMessage::SendUserLogin not find gate");
		}
		*/
		GateInfo gateRet;
		if (login_flag == 1) //  ali高防
		{
			gateRet = gWork.GetAliGaoFangGateUsefulInfo(severID);
			gateRet.m_ip = gWork.GetGaofangIp(login_flag);
		}
		else if(login_flag == 2) // Yunman高防
		{
			gateRet = gWork.GetYunManGaoFangGateUsefulInfo(severID);
			gateRet.m_ip = gWork.GetGaofangIp(login_flag);			
		}
		else
		{
			gateRet = gWork.GetGateUsefulInfo(severID);
		}

		if (gateRet.m_id == 0 && gateRet.m_ip.empty())
		{
			LLOG_ERROR("****CUserMessage::SendUserLogin not find gate");
		}
		ret.m_gateIp = gateRet.m_ip;
		ret.m_gatePort = gateRet.m_port;
		LLOG_ERROR("Logwyz  .....................ip = [%s] port:[%d]", ret.m_gateIp.c_str(), ret.m_gatePort);

		ret.m_id = user->m_usert.m_id;
		ret.m_seed = send.m_seed;
		ret.m_totalPlayNum = user->m_usert.m_totalplaynum;
		ret.m_TotalCircleNum = user->m_usert.m_customInt[0];
		ret.m_totalWinNum = user->m_usert.m_customInt[1];

		break; // 直接退出
	}

	if ( gameloginSP )
	{
		LMsgToLoginServer retMsg;
		retMsg.m_strUUID = uuid;
		retMsg.m_dataBuff= ret.GetSendBuff();
		LLOG_DEBUG("CUserMessage::SendUserLogin~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		gameloginSP->Send(retMsg.GetSendBuff());
	}
}

void CUserMessage::HanderModifyUserNew(LMsgL2CeModifyUserNew* msg)
{
	if (msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_strUUID);
	if(safeUser && safeUser->getUser())
	{
		safeUser->getUser()->ModifyNew( msg->m_value );
	}
	else
	{
		LLOG_ERROR("CUserMessage::HanderModifyUserNew user not exiest, userid=%s",  msg->m_strUUID.c_str());
	}
}

void CUserMessage::HanderUpdateUserCoin(LMsgL2CeUpdateCoin* msg)
{
	LLOG_DEBUG("CUserMessage::HanderUpdateUserCoin");
	if (msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		//没找到玩家
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	//for 给玩家添加金币
	Lint goldNum = 0;
	Lstring userIp, userGps;
	Lint blocked = 0;
	bool bGet = gUserManager.loadUserInfoFromDBByUserId(user->m_usert.m_id, goldNum, userIp, userGps, blocked);
	LLOG_DEBUG("CUserMessage::HanderUpdateUserCoin  bget[%d],goldNum=[%d],blocked=[%d],ip[%s],gps=[%s]", bGet, goldNum, blocked, userIp.c_str(), userGps.c_str());

	if (bGet) {
		user->m_usert.m_numOfCard2s = goldNum;
		user->m_usert.m_customString1 = userIp;
		//user->m_usert.m_customString2 = userGps;
		user->m_usert.m_customInt[3] = blocked;
	}
	//end 给玩家添加金币
	msg->m_usert.m_numOfCard2s = goldNum;
	msg->m_usert.m_customString1 = userIp;	
	user->m_usert.m_customString2 = msg->m_usert.m_customString2;

	Lint nFree=0;     // gUserManager.CheckInFreeTime();
	Lint nfreeStart = 0;
	Lint nfreeEnd = 0;
	nfreeStart=1509421800;
	nfreeEnd=1510070401;
	//gUserManager.GetFreeTime(nfreeStart, nfreeEnd);

	LMsgCe2LUpdateCoin send;
	send.m_userID = user->m_usert.m_id;
	send.m_coinNum = goldNum;
	send.m_userIp = userIp;
	send.m_userGps = userGps;
	send.m_blocked = blocked;
	send.m_Free = nFree;
	send.m_creatType = msg->m_creatType;
	send.m_startTime = nfreeStart;
	send.m_endTime = nfreeEnd;

    ////////////////////////////////////////////////////////
	send.m_credits = msg->m_credits;
	send.m_userid = msg->m_userid;
	send.m_gateId = msg->m_gateId;
	send.m_strUUID = msg->m_strUUID;
	send.m_ip = msg->m_ip;
	send.m_usert = msg->m_usert;
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
	send.m_GpsLimit = msg->m_GpsLimit; //GPS信息配置
	send.m_allowLookOn = msg->m_allowLookOn;  //是否允许观战
	send.m_clubOwerLookOn = msg->m_clubOwerLookOn;   //是否允许会长名牌

	//LLOG_DEBUG("HanderUpdateUserCoin msg->m_playType.size() =%d", msg->m_playType.size());
	//memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	memcpy(send.m_Player0CardValue, msg->m_Player0CardValue, sizeof(send.m_Player0CardValue));
	memcpy(send.m_Player1CardValue, msg->m_Player1CardValue, sizeof(send.m_Player1CardValue));
	memcpy(send.m_Player2CardValue, msg->m_Player2CardValue, sizeof(send.m_Player2CardValue));
	memcpy(send.m_Player3CardValue, msg->m_Player3CardValue, sizeof(send.m_Player3CardValue));
	memcpy(send.m_SendCardValue, msg->m_SendCardValue, sizeof(send.m_SendCardValue));
	memcpy(send.m_HaoZiCardValue, msg->m_HaoZiCardValue, sizeof(send.m_HaoZiCardValue));

	gWork.SendMsgToLogic(send, msg->m_serverID);
}

void CUserMessage::HanderUpdateUserGps(LMsgLMG2CEUpdateUserGPS* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("CUserMessage::HanderUpdateUserGps userId=[%d] gps:%s", msg->m_userid, msg->m_Gps.c_str());
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		//没找到玩家
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	user->m_usert.m_customString2 = msg->m_Gps;
	gUserManager.SaveUserLastGPS(user.get());
}

void CUserMessage::HanderUpdateUserCoinJoinDesk(LMsgL2CeUpdateCoinJoinDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("CUserMessage::HanderUpdateUserCoinJoinDesk, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderUpdateUserCoinJoinDesk not find userId=[%d] info", msg->m_userid);
		//没找到玩家
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	//for 给玩家添加金币
	Lint goldNum = 0;
	Lstring userIp, userGps;
	Lint blocked = 0;
	bool bGet = gUserManager.loadUserInfoFromDBByUserId(user->m_usert.m_id, goldNum, userIp, userGps, blocked);
	if (bGet) {
		user->m_usert.m_numOfCard2s = goldNum;
		user->m_usert.m_customString1 = userIp;
		//user->m_usert.m_customString2 = userGps;
		user->m_usert.m_customInt[3] = blocked;
	}
	//end 给玩家添加金币

	msg->m_usert.m_numOfCard2s = goldNum;
	msg->m_usert.m_customString1 = userIp;
	//msg->m_usert.m_customString2 = userGps;
	user->m_usert.m_customString2 = msg->m_usert.m_customString2; //更新玩家最后GPS位置

	Lint nFree=0; // gUserManager.CheckInFreeTime();

	Lint nfreeStart = 0;
	Lint nfreeEnd = 0;
	nfreeStart=1509421800;
	nfreeEnd=1510070401;
	//gUserManager.GetFreeTime(nfreeStart, nfreeEnd);

	LMsgCe2LUpdateCoinJoinDesk send;
	send.m_userID = msg->m_userid;
	send.m_coinNum = goldNum;
	send.m_userIp = userIp;
	send.m_userGps = userGps;
	send.m_Block = blocked;
	send.m_Free = nFree;
	send.m_startTime = nfreeStart;
	send.m_endTime = nfreeEnd;
	////////////////////////////////////////////////////////
	send.m_userid = msg->m_userid;
	send.m_gateId = msg->m_gateId;
	send.m_strUUID = msg->m_strUUID;
	send.m_deskID = msg->m_deskID;
	send.m_ip = userIp;
	send.m_usert = msg->m_usert;
	send.m_addDeskFlag = msg->m_addDeskFlag;

	send.m_type = msg->m_type;
	send.m_clubId = msg->m_clubId;
	send.m_playTypeId = msg->m_playTypeId;

	LLOG_DEBUG("CUserMessage::HanderUpdateUserCoinJoinDesk  msg->m_clubMasterId=[%d]", msg->m_clubMasterId);
	//俱乐部需要检查会长金币值
	if (msg->m_clubMasterId!=0)
	{
	
		boost::shared_ptr<CSafeUser> safeUser=gUserManager.getUser(msg->m_clubMasterId);
		if (safeUser.get()==NULL||!safeUser->isValid())
		{
			send.m_errorCode=1;
			LLOG_ERROR("CUserMessage::HanderUpdateUserCoinJoinDesk not find m_clubMasterId=[%d]  info ", msg->m_clubMasterId);
			//这里直接return不好，不给回复，manager会不知道原因,导致客户端也不知道原因
			//return;
		}
		else
		{
			boost::shared_ptr<DUser> user=safeUser->getUser();

			Lint goldNum=0;
			Lstring userIp, userGps;
			Lint blocked=0;
			Lint NumsDiamond=0;
			bool bGet=gUserManager.loadUserInfoFromDBByUserId(user->m_usert.m_id, goldNum, userIp, userGps, blocked, NumsDiamond);
			if (bGet) {
				user->m_usert.m_numOfCard2s=goldNum;
				user->m_usert.m_customString1=userIp;
				//user->m_usert.m_customString2=userGps;
				user->m_usert.m_customInt[3]=blocked;

				send.m_clubMasterCoinNum= goldNum;
				send.m_clubMasterBlock=blocked;
				LLOG_DEBUG("CUserMessage::HanderUpdateUserCoinJoinDesk club master id=[%d] ,goldNum[%d] diamondNum=[%d]", user->m_usert.m_id, send.m_clubMasterCoinNum,NumsDiamond);
			}
		}
	}

	gWork.SendMsgToLogic(send, msg->m_serverID);
}


//logicmanage发送玩家已经登录的信息
void CUserMessage::HanderLogicUserLogin(LMsgL2CeUserServerLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}

	//LLOG_DEBUG("CUserMessage::HanderLogicUserLogin, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_openID);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		//没找到玩家
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	//for 给玩家添加金币
	Lint goldNum = 0;
	Lstring userIp, userGps;
	Lint blocked = 0;
	bool bGet = gUserManager.loadUserInfoFromDBByUserId(user->m_usert.m_id, goldNum,userIp, userGps, blocked);
	if (bGet) {
		user->m_usert.m_numOfCard2s = goldNum;
		user->m_usert.m_customString1 = userIp;
		user->m_usert.m_customString2 = userGps;
		user->m_usert.m_customInt[3] = blocked;
	}
	//end 给玩家添加金币

	if(!gWork.isLogicManagerExist(msg->m_serverID))
	{
		user->m_logicID = 0;
		return;
	}

	if (blocked) {
		return;
	}

	user->m_logicID = msg->m_serverID;
	user->m_usert.m_lastLoginTime = m_tickTimer.Secs();
	gUserManager.SaveUserLastLogin(user.get());		//玩家最后登陆时间
}

void CUserMessage::HanderLogicUserLogout(LMsgL2CeUserServerLogout* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//LLOG_DEBUG("CUserMessage::HanderLogicUserLogot, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_openID);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		//没找到玩家
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	if(!gWork.isLogicManagerExist(msg->m_serverID))
	{
		user->m_logicID = 0;
		return;
	}

	user->m_logicID = 0;
	//gUserManager.SaveUser(user.get());		//玩家离线，保存玩家数据
}

void CUserMessage::HanderModifyCard(LMsgL2LMGModifyCard* msg)
{
	if(!msg)
	{
		return;
	}

	if (CARDS_OPER_TYPE_CLUB_DIAMOND==msg->operType)
	{
		gUserManager.UpdateClubCreatorNumDiamond(msg->m_userid, msg->cardNum);
		return;
	}

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyCard user not exiest, userid=%d, type=%d, count=%d, operType=%d",  msg->m_userid, msg->cardType, msg->cardNum, msg->operType);
		modifyCardCount(msg);
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	if (msg->isAddCard)
	{
		user->AddCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
	}
	else
	{
		user->DelCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
	}
}

void CUserMessage::modifyCardCount(LMsgL2LMGModifyCard* L2LmgMsg, bool bNeedSave)
{
	if (bNeedSave)
	{
		Lint count = L2LmgMsg->cardNum;
		if (!L2LmgMsg->isAddCard) {
			//count = count * (-1);
		}

		gUserManager.UpdateUserNumsCard2(L2LmgMsg->m_userid, count);

		gUserManager.SaveCardInfo(L2LmgMsg->m_userid, L2LmgMsg->cardType, L2LmgMsg->cardNum, L2LmgMsg->operType, L2LmgMsg->admin);
	}
}

void CUserMessage::HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyPlayCount user not exiest, userid=%d",  msg->m_userid);
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	user->AddPlayCount();
}

void CUserMessage::HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg)
{
	if(msg == NULL)
	{
		return;
	}
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyUserCoinsNum user not exiest, userid=%d, count=%d, operType=%d",  msg->m_userid, msg->coinsNum, msg->operType);
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	if(msg->isAddCoins)
	{
		user->AddCoinsCount(msg->coinsNum, msg->operType);
	}
	else
	{
		user->DelCoinsCount(msg->coinsNum, msg->operType);
	}
}

void CUserMessage::HanderModifyUserCreditValue( LMsgC2C_ADD_CRE* msg )
{
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userId);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	user->AddCreditValue();
}

void CUserMessage::HanderExchangeCard( LMsgL2LMGExchCard* msg )
{
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	user->ExchangeCardCoin(msg->m_add,msg->m_del,msg->operType,msg->cardType,msg->admin);
}

void CUserMessage::UpdateUserInfo(const LUser& usert)
{
	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "Nike='"<<  usert.m_nike <<"',";
	ss << "Sex='"<<  usert.m_sex <<"',";
	ss << "Provice='"<<  usert.m_provice <<"',";
	ss << "City='"<<  usert.m_city <<"',";
	ss << "Country='"<<  usert.m_country<<"',";
	ss << "OpenId='" << usert.m_openid << "',";
	ss << "HeadImageUrl='"<<  usert.m_headImageUrl<< "' WHERE UnionId='";
	ss << usert.m_unioid << "'";

	Lstring* sql = new Lstring(ss.str());

	gDbServerManager.Push(sql,usert.m_id);
}

void CUserMessage::UpdateUserPhoneUUID(const LUser& usert)
{
	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "phoneUUID='" << usert.m_customString4 << "',";
	ss << "userIdTempIn='" << usert.m_customString5 << "',";
	ss << "loginDevice='" << usert.m_customString7 << "',";
	ss << "userIdTempOut='" << usert.m_customString6 << "' WHERE id='";
	ss << usert.m_id << "'";

	LLOG_ERROR("UpdateUserPhoneUUID [%s]",ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());

	gDbServerManager.Push(sql, usert.m_id);
}

//第一次升级，客户端上传phoneUUID 和 userId ，服务器保存，写入数据库
void CUserMessage::HanderUserWechatLoginFirst(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("HanderUserWechatLoginFirst v[%d],nike[%s],uuid[%s],openid[%s],uuidOfzy[%s],openIdOfzy[%s],phoneuuid[%s],userId[%d],tempUseId[%d]", msg->m_appVersion,msg->m_nikename.c_str(),msg->m_uuid.c_str(), msg->m_openId.c_str(),msg->m_uuidOfzy.c_str(),msg->m_openIdOfzy.c_str(), msg->m_phoneUUID.c_str(), msg->m_UserId, msg->m_UserIdSys);

	Lint errorCode = 1;
	std::string errorMsg;
	do
	{
		if (!gWork.isLogicManagerExist(msg->m_severID))
		{
			LLOG_ERROR("CUserMessage::HanderUserWechatLogin logicid not exist %d", msg->m_severID);
			errorCode = 6;
			errorMsg = "logicid not exist";
			break;
		}

		if (msg->m_uuid.empty() || msg->m_uuid.size()>32)
		{
			LLOG_ERROR("CUserMessage::HanderUserWechatLogin uuid error, uuid=%s", msg->m_uuid.c_str());
			errorCode = 7;
			errorMsg = "uuid error";
			break;
		}

		boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_uuid);
		boost::shared_ptr<DUser> user;

		//掌赢 转换 老A 麻将
		if (safeUser.get() == NULL)
		{
			LLOG_ERROR("HanderUserWechatLoginFirst uuid[%s],openid[%s] find fail.  use  kUuidOfzy[%s] ,kOpenIdOfzy[%s] find ", msg->m_uuid.c_str(), msg->m_openId.c_str(), msg->m_uuidOfzy.c_str(),msg->m_openIdOfzy.c_str());

			if (!msg->m_uuidOfzy.empty() && msg->m_uuidOfzy.size() <= 32)
			{
				safeUser = gUserManager.getUser(msg->m_uuidOfzy); //Login update根据用户ID查询
				if (safeUser)
				{
					user = safeUser->getUser();

					LLOG_ERROR("HanderUserWechatLoginFirst  kUuidOfzy[%s],kOpenIdOfzy[%s] find  userId[%d] success!! ,uuid[%s],openid[%s]",  msg->m_uuidOfzy.c_str(), msg->m_openIdOfzy.c_str(), user==NULL?0:user->m_usert.m_id, msg->m_uuid.c_str(), msg->m_openId.c_str());

				
					//删除旧的用户
					gUserManager.delUser(user->m_usert.m_id);

					//update unioid 为新的
					user->m_usert.m_unioid = msg->m_uuid;
					user->m_usert.m_openid = msg->m_openId;
					user->m_usert.m_customInt[7] = getChangeStatusByAppV(msg->m_appVersion, false);;

					//添加新的用户
					gUserManager.addUser(user);

					//更新数据库
					std::stringstream ss;
					ss << "UPDATE user SET ";
					ss << " ChangeStatus='" << user->m_usert.m_customInt[7] << "',";
					ss << " UnionId='" << user->m_usert.m_unioid << "',";
					ss << " OpenId='" << user->m_usert.m_openid << "'";
					ss << " WHERE Id='" << user->m_usert.m_id << "'";

					Lstring* sql = new Lstring(ss.str());
					gDbServerManager.Push(sql, user->m_usert.m_id);
					LLOG_ERROR("HanderUserWechatLoginFirst sql[%s]  ", ss.str().c_str());
				}
			}

		}
		else
		{
			user = safeUser->getUser();
			LLOG_ERROR("HanderUserWechatLoginFirst uuid[%s],openid[%s] find success useId[%d]!!!  kUuidOfzy[%s],kOpenIdOfzy[%s]", msg->m_uuid.c_str(), msg->m_openId.c_str(), user == NULL?0:user->m_usert.m_id, msg->m_uuidOfzy.c_str(), msg->m_openIdOfzy.c_str());
		}

		//  新用户
		if (user.get() == NULL)
		{
			//处理性别值异常
			if (msg->m_sex < 1 || msg->m_sex > 2)
			{
				msg->m_sex = 2;
			}

			//昵称中的单引号处理
			for (Lsize i = 0; i < msg->m_nikename.size(); ++i)
			{
				if (msg->m_nikename[i] == '\'')
					msg->m_nikename[i] = ' ';
			}

			DUser* newUser = new DUser;
			newUser->m_usert.m_openid = msg->m_openId;
			newUser->m_usert.m_unioid = msg->m_uuid;
			newUser->m_usert.m_nike = msg->m_nikename;
			newUser->m_usert.m_sex = msg->m_sex;
			newUser->m_usert.m_headImageUrl = msg->m_imageUrl;
			newUser->m_usert.m_regTime = m_tickTimer.Secs();
			newUser->m_usert.m_lastLoginTime = m_tickTimer.Secs();
			newUser->m_usert.m_customString4 = msg->m_phoneUUID;
			newUser->m_usert.m_customString5 = std::to_string(msg->m_UserId);
			newUser->m_usert.m_customString6 = std::to_string(msg->m_UserIdSys);
			newUser->m_usert.m_customString7 = msg->m_device;
			newUser->m_usert.m_customInt[7] = getChangeStatusByAppV(msg->m_appVersion, true);
			
			// 验证新用户登陆
			LMsgNewUserVerifyRequest* req = new LMsgNewUserVerifyRequest;
			req->m_login_flag = login_flag;
			req->m_pUser = newUser;
			req->m_gameloginSP = gameloginSP;
			req->m_severID = msg->m_severID;
			req->m_bWechatLogin = true;
			req->m_openId = msg->m_openId;
			req->m_unionId = msg->m_uuid;
			req->m_type = newUser->m_usert.m_customInt[7];
			req->m_accessToken = msg->m_accessToken;
			
			gDbNewUserManager.Push(msg->m_uuid, req);
			return;	// 直接返回
		}
		else   //  老用户
		{
			//昵称中的单引号处理
			for (Lsize i = 0; i < msg->m_nikename.size(); ++i)
			{
				if (msg->m_nikename[i] == '\'')
					msg->m_nikename[i] = ' ';
			}

			bool isChange = false;
			if ((!msg->m_nikename.empty() && user->m_usert.m_nike != msg->m_nikename))                 //昵称
			{
				user->m_usert.m_nike = msg->m_nikename;
				user->m_usert.m_unioid = msg->m_uuid;
				isChange = true;
			}
			if (!msg->m_imageUrl.empty() && user->m_usert.m_headImageUrl != msg->m_imageUrl)     //头像
			{
				user->m_usert.m_headImageUrl = msg->m_imageUrl;
				user->m_usert.m_unioid = msg->m_uuid;
				isChange = true;
			}
			if (!msg->m_openId.empty() && user->m_usert.m_openid != msg->m_openId)
			{
				user->m_usert.m_openid = msg->m_openId;
				user->m_usert.m_unioid = msg->m_uuid;
				isChange = true;
			}
			if (user->m_usert.m_sex != msg->m_sex)                  //性别
			{
				user->m_usert.m_sex = msg->m_sex;
				user->m_usert.m_unioid = msg->m_uuid;
				isChange = true;
			}
			if (isChange)
				UpdateUserInfo(user->m_usert);

			Lint changeStatus = getChangeStatusByAppV(msg->m_appVersion, false);
			if (user->m_usert.m_customInt[7] !=  changeStatus)
			{
				Lint oldChangeStatus = user->m_usert.m_customInt[7];
				user->m_usert.m_customInt[7] = changeStatus;
				//更新数据库
				std::stringstream ss;
				ss << "UPDATE user SET ";
				ss << " ChangeStatus='" << changeStatus << "'";
				ss << " WHERE Id='" << user->m_usert.m_id << "'";

				Lstring* sql = new Lstring(ss.str());
				gDbServerManager.Push(sql, user->m_usert.m_id);
				LLOG_ERROR("userLogin change  appversion[%d] oldChangeStatus[%d]  sql[%s]  ", msg->m_appVersion, oldChangeStatus, ss.str().c_str());
			}

		}

		errorCode = 0;
		errorMsg = "";

	} while (false);


	SendUserLogin(msg->m_uuid, gameloginSP, msg->m_severID, errorCode, errorMsg, login_flag);
}

void CUserMessage::SendUserPhoneLogin(const Lint userId,Lstring phone, LSocketPtr gameloginSP, Lint severID, Lint errorCode, const Lstring& errorMsg, Lint login_flag)
{
	LLOG_DEBUG("SendUserPhoneLogin userId [%d] phone[%s]",userId,phone.c_str());
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(userId);
	boost::shared_ptr<DUser> user;

	LLOG_DEBUG("****Suijun SendUserPhoneLogin  login_flag = %d", login_flag);
	if (safeUser)
	{
		user = safeUser->getUser();
		//for 给玩家添加金币
		Lint goldNum = 0;
		Lstring userIp, userGps;
		Lint blocked = 0;
		Lint level = 0;
		bool bGet = gUserManager.loadUserInfoFromDBByUserId(user->m_usert.m_id, goldNum, userIp, userGps, blocked, level);
		if (bGet) {
			user->m_usert.m_numOfCard2s = goldNum;
			user->m_usert.m_customString1 = userIp;
			//user->m_usert.m_customString2 = userGps;
			user->m_usert.m_customInt[3] = blocked;  //loadUserInfoFromDBByUserId
			user->m_usert.m_customInt[4] = level;
		}
		//end 给玩家添加金币
	}


	LMsgS2CPhoneLogin2 ret;
	ret.m_error_code = errorCode;
	ret.m_errorMsg = errorMsg;
	while (user && errorCode == 0)
	{
		if (user->m_logicID != severID && user->m_logicID != 0)
		{
			//不是在本服务器登陆的玩家, 发送消息，提示退出原APP
			LLOG_ERROR("CUserMessage::SendUserPhoneLogin logicid not empty %d : %d", severID, user->m_logicID);
			ret.m_error_code = 5;
			ret.m_errorMsg = string_To_UTF8(gWork.getServerNameByID(user->m_logicID));
			break;
		}
		user->m_lastReqTime = m_tickTimer.Secs();

		LMsgCe2LUserLogin send;
		send.m_seed = L_Rand(10000000, 99999999);
		send.user = user->m_usert;

		gWork.SendMsgToLogic(send, severID);

		GateInfo gateRet;
		if (login_flag == 1) //  ali高防
		{
			gateRet = gWork.GetAliGaoFangGateUsefulInfo(severID);
			gateRet.m_ip = gWork.GetGaofangIp(login_flag);
		}
		else if (login_flag == 2) // Yunman高防
		{
			gateRet = gWork.GetYunManGaoFangGateUsefulInfo(severID);
			gateRet.m_ip = gWork.GetGaofangIp(login_flag);
		}
		else
		{
			gateRet = gWork.GetGateUsefulInfo(severID);
		}

		if (gateRet.m_id == 0 && gateRet.m_ip.empty())
		{
			LLOG_ERROR("****CUserMessage::SendUserPhoneLogin not find gate");
		}
		ret.m_gateIp = gateRet.m_ip;
		ret.m_gatePort = gateRet.m_port;
		LLOG_ERROR("Logwyz  .....................ip = [%s] port:[%d]", ret.m_gateIp.c_str(), ret.m_gatePort);

		ret.m_id = user->m_usert.m_id;
		ret.m_seed = send.m_seed;
		ret.m_totalPlayNum = user->m_usert.m_totalplaynum;
		ret.m_TotalCircleNum = user->m_usert.m_customInt[0];
		ret.m_totalWinNum = user->m_usert.m_customInt[1];

		ret.m_imageUrl = user->m_usert.m_headImageUrl;
		ret.m_nike = user->m_usert.m_nike;
		ret.m_uuid = user->m_usert.m_unioid;
		ret.m_nikename = user->m_usert.m_nike;
		ret.m_openId = user->m_usert.m_openid;
		ret.m_sex = user->m_usert.m_sex;

		break; // 直接退出
	}

	if (gameloginSP)
	{
		LMsgToLoginServer retMsg;
		retMsg.m_strUUID = phone;
		retMsg.m_dataBuff = ret.GetSendBuff();
		LLOG_DEBUG("CUserMessage::SendUserPhoneLogin~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		gameloginSP->Send(retMsg.GetSendBuff());
	}
}

void CUserMessage::HanderUserPhoneLogin(LMsgC2SPhoneLogin* msg, LSocketPtr gameloginSP, Lint login_flag)
{
	if (msg == NULL)
	{
		LLOG_ERROR("HanderUserPhoneLogin MSG NULL ERROR");
		return;
	}
	LLOG_ERROR("HanderUserPhoneLogin type=[%d]  phone[%s] vcode[%s]", msg->m_type, msg->m_Phone.c_str(), msg->m_Vcode.c_str());

	// 是否用户是否注册
	Lint userId = 0;
	{
		if (msg->m_Phone.empty())return;
		userId = gUserManager.getUserIdByPhone(msg->m_Phone);
		if (userId == 0)
		{
			LLOG_ERROR("HanderUserPhoneLogin getUserIdByPhone [%s] return userId = 0 ", msg->m_Phone.c_str());
			if (gameloginSP&& (msg->m_type == PHONE_LOGIN_TYPE_MSG || msg->m_type == PHONE_LOGIN_TYPE_VOICE))
			{
				LMsgS2CPhoneLogin1 login1;
				login1.m_error_code = PHONE_LOGIN_ERROR_NOT_BINDING;
				LMsgToLoginServer retMsg;
				retMsg.m_strUUID =msg->m_Phone;
				retMsg.m_dataBuff = login1.GetSendBuff();
				gameloginSP->Send(retMsg.GetSendBuff());
			}
			if (gameloginSP && msg->m_type == PHONE_LOGIN_TYPE_CHECK)
			{
				LMsgS2CPhoneLogin2 login2;
				login2.m_error_code = PHONE_LOGIN_ERROR_NOT_BINDING;
				LMsgToLoginServer retMsg;
				retMsg.m_strUUID = msg->m_Phone;
				retMsg.m_dataBuff = login2.GetSendBuff();
				gameloginSP->Send(retMsg.GetSendBuff());
			}
			return;
		}

	}

	//60秒不能重复发送
	if (msg->m_type == PHONE_LOGIN_TYPE_MSG || msg->m_type == PHONE_LOGIN_TYPE_VOICE)
	{
		Lint retTime = gUserManager.getPhoneTime(msg->m_Phone);
		Lint mTime = time(NULL) - retTime;
		LLOG_DEBUG("HanderUserPhoneLogin now=[%d]  beforeTime[%d] = [%d]", time(NULL),retTime, mTime);
		if (mTime > 0 && mTime < 60)
		{
			LMsgS2CPhoneLogin1 login1;
			login1.m_error_code = PHONE_LOGIN_ERROR_TIME_NOT;
			LMsgToLoginServer retMsg;
			retMsg.m_strUUID = msg->m_Phone;
			retMsg.m_dataBuff = login1.GetSendBuff();
			gameloginSP->Send(retMsg.GetSendBuff());
			return;
		}
		else
		{
			gUserManager.setPhoneTime(msg->m_Phone);
		}

	}

	if (msg->m_type == PHONE_LOGIN_TYPE_MSG)  //获取验证码
	{
		MHPhoneVcodeHelper  phoneVcodeHelper;
		phoneVcodeHelper.Invoke(PHONE_LOGIN_TYPE_MSG,msg->m_Phone);
		{
			LMsgS2CPhoneLogin1 login1;
			login1.m_type = PHONE_LOGIN_TYPE_MSG;
			LMsgToLoginServer retMsg;
			retMsg.m_strUUID = msg->m_Phone;
			retMsg.m_dataBuff = login1.GetSendBuff();
			gameloginSP->Send(retMsg.GetSendBuff());
		}
		return;
	}
	else if (msg->m_type == PHONE_LOGIN_TYPE_VOICE)
	{
		MHPhoneVcodeHelper  phoneVcodeHelper;
		phoneVcodeHelper.Invoke(PHONE_LOGIN_TYPE_VOICE, msg->m_Phone);
		{
			LMsgS2CPhoneLogin1 login1;
			login1.m_type = PHONE_LOGIN_TYPE_VOICE;

			LMsgToLoginServer retMsg;
			retMsg.m_strUUID = msg->m_Phone;
			retMsg.m_dataBuff = login1.GetSendBuff();
			gameloginSP->Send(retMsg.GetSendBuff());
		}
		return;
	}
	else if (msg->m_type == PHONE_LOGIN_TYPE_CHECK)   //验证码
	{
		////验证码验证
		//boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(userId);
		//if (safeUser.get() == NULL || !safeUser->isValid())
		//{
		//	//没找到玩家
		//	return;
		//}
		//
		//boost::shared_ptr<DUser> user = safeUser->getUser();
		//
		////SendUserPhoneLogin(userId, gameloginSP,msg->);
		Lint ret = gUserManager.checkUserPhoneVcode(msg->m_Phone, msg->m_Vcode);

		LLOG_DEBUG("HanderUserPhoneLogin ret =checkUserPhoneVcode[%d] ",ret);

		if (ret == 0 /* || msg->m_Vcode.compare("7777")==0 */)   //验证成功
		{
			Lstring err_msg;
			SendUserPhoneLogin(userId, msg->m_Phone,gameloginSP, 16210, 0, err_msg, login_flag);
		}
		else 
		{
			LMsgS2CPhoneLogin2 send;
			send.m_error_code = ret;
		
			if (gameloginSP)
			{
				LMsgToLoginServer retMsg;
				retMsg.m_strUUID = msg->m_Phone;
				retMsg.m_dataBuff = send.GetSendBuff();
				LLOG_DEBUG("HanderUserPhoneLogin~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				gameloginSP->Send(retMsg.GetSendBuff());
			}
		
		}

	}
}

//sql   
void CUserMessage::HanderSqlInfo(LMsgLMG2CeSqlInfo * msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("Work::HanderSqlInfo  msg == NULL");
		return;
	}
	LLOG_DEBUG("Work::HanderSqlInfo  which[%d] type[%d] sql[%s]", msg->m_which, msg->m_type, msg->m_sql.c_str());

	if (msg->m_which == 1)
	{
		gClubManager.Excute(msg->m_sql);
	}

}

//void CUserMessage::HanderUpdatePointRecord(LMsgLMG2CEUpdatePointRecord*msg)
//{
//	if (msg == NULL)
//	{
//		LLOG_ERROR("Work::HanderUpdatePointRecord  msg == NULL");
//		return;
//	}
//	LLOG_ERROR("HanderUserChange  operId[%d]   type[%d] charge clubId[%d]  userId[%d]  point[%d] r[%s]", msg->m_record.m_operId, msg->m_record.m_type, msg-//>m_record.m_clubId, msg->m_record.m_userId, msg->m_record.m_point, msg->m_record.m_operR.c_str());
//
//
//	//返回给manager 通知 充值是否成功
//	LMsgLMG2CEUpdatePointRecord mSend;
//	mSend.m_msgType = 1;
//
//	//if (msg->m_record.m_clubId == 0 || msg->m_record.m_userId == 0 || msg->m_record.m_point == 0||)
//
//	//写入数据库 update club_users point
//	std::stringstream sql;
//	if (msg->m_record.m_type == CHANGE_POINT_TYPE_ADD)
//	{
//		sql << "UPDATE club_users";
//		sql << " SET point =point + '" << msg->m_record.m_point << "'";
//		sql << " WHERE clubId = '" << msg->m_record.m_clubSq << "'";
//		sql << " AND userId = '" << msg->m_record.m_userId << "'";
//	}
//	else if (msg->m_record.m_type == CHANGE_POINT_TYPE_REDUCE)
//	{
//		sql << "UPDATE club_users";
//		sql << " SET point =point - '" << msg->m_record.m_point << "'";
//		sql << " WHERE clubId = '" << msg->m_record.m_clubSq << "'";
//		sql << " AND userId = '" << msg->m_record.m_userId << "'";
//	}
//	LLOG_ERROR("HanderUpdatePointRecord update club_users sql[%s]",sql.str().c_str());
//	bool  ret = gClubManager.Excute(sql.str());                 //充值  
//
//	if (ret)
//	{
//		//auto t_iClub = m_mapId2Club.find(msg->m_record.m_clubId);
//		//if (t_iClub == m_mapId2Club.end())
//		//
//		//auto t_iterMemberEnd = t_iterClub->memberList.end();
//		//for (auto t_iterMember = t_iterClub->memberList.begin(); t_iterMember != t_iterMemberEnd; t_iterMember++)
//		//{
//		//	if ((*t_iterMember)->m_userId == msg->m_record.m_userId)
//		//	{
//		//		if (msg->m_record.m_type == CHANGE_POINT_TYPE_ADD)
//		//		{
//		//			(*t_iterMember)->m_point += msg->m_record.m_point;
//		//		}
//		//		else if (msg->m_record.m_type == CHANGE_POINT_TYPE_REDUCE)
//		//		{
//		//			(*t_iterMember)->m_point -= msg->m_record.m_point;
//		//		}
//		//		break;
//		//	}
//		//}
//
//		//记录point_log
//		std::stringstream logSql;
//		logSql << "INSERT INTO club_chargepoint_log (clubId,userId,type,point,operId,operR,Time,ctime,utime) VALUES (";
//		logSql << "'" << msg->m_record.m_clubSq << "',";
//		logSql << "'" << msg->m_record.m_userId << "',";
//		logSql << "'" << msg->m_record.m_type << "',";
//		logSql << "'" << msg->m_record.m_point << "',";
//		logSql << "'" << msg->m_record.m_operId << "',";
//		logSql << "'" << msg->m_record.m_operR << "',";
//		logSql << "'" << msg->m_record.m_time << "',";
//		logSql << " now(),now())";
//
//		LLOG_ERROR("HanderUpdatePointRecord club_chargepoint_log sql[%s]", logSql.str().c_str());
//		ret = gClubManager.Excute(logSql.str());
//	}
//	else
//	{
//		//充值失败
//		mSend.m_errorCode = 1;
//	}
//	
//	mSend.m_record = msg->m_record;
//	Lint serverID = 16210;
//	
//	gWork.SendMsgToLogic(mSend, serverID);
//
//}

void  CUserMessage::HanderAddClub(LMsgCe2LAddClub*msg)
{

}

void  CUserMessage::HanderClubAddPlayType(LMsgCe2LMGClubAddPlayType* msg)
{

}

Lint CUserMessage::getChangeStatusByAppV(Lint version, bool newUser)
{
	if (version <= 0)return 1;
	else
	{
		if (newUser)
		{
			return 2 * version + 1;
		}
		else
		{
			return 2 * version;
		}
	}

}


