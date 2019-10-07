#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "UserManager.h"
#include "User.h"
#include "ActiveManager.h"
#include <openssl/md5.h>
#include "DeskManager.h"
#include "LVipLog.h"
#include <regex>
#include "UserMessageMsg.h"
#include "RuntimeInfoMsg.h"
#include "RLogHttp.h"
#include "mhmsghead.h"
#include "mjConfig.h"
#include "HttpRequest.h"
#include "clubManager.h"
#include "SpecActive.h"


#define  _MH_ALERT_URL_MANAGER_START_FINISH "http://api.ry.haoyunlaiyule.com/server/alert?type=14"
using namespace boost::asio;  
using boost::asio::ip::tcp; 

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicManager");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	if(!gRuntimeInfoMsg.Init())
	{
		LLOG_ERROR("Fail to init runtime info");
		return false;
	}

	if (!gDeskManager.Init())
	{
		LLOG_ERROR("DeskManager.Init error");
		return false;
	}

	if (!gActiveManager.Init())
	{
		LLOG_ERROR("gActiveManager.Init error");
		return false;
	}

	if (!gClubManager.Init())
	{
		LLOG_ERROR("gClubManager.Init error");
		return false;
	}

	if( !gRLT.Init( gConfig.GetRemoteLogUrl(), (size_t)gConfig.GetMaxCachedLogSize(), (size_t)gConfig.GetMaxCachedLogNum() ) )
	{
		LLOG_ERROR("gRLT.Init error");
		return false;
	}
	_checkRLogStart();

	//LMsgFactory::Instance().Init();
	MH_InitFactoryMessage_ForMapMessage();
	MHInitFactoryMessage();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	_initTickTime();
	_checkLogicServerOnlySupportGameType();
	_checkMaxDeskCountLimitForLogicServer();

	m_bHadLoadedData = false;

	LTime tNow;
	m_last_alert_time = tNow.Secs();

	 
	char * pBufEnv = getenv("ONLINE_SERVER");
	if (pBufEnv != NULL)
	{
		LLOG_ERROR("*** enviroment var ONLINE_SERVER: %s ", pBufEnv);
	}
	else
	{
		LLOG_ERROR("***Can nott get enviroment var ONLINE_SERVER");
	}
	if (NULL != pBufEnv) 
	{
		Lstring result = "";
		HttpRequest::Instance().HttpGet(_MH_ALERT_URL_MANAGER_START_FINISH, result);
	}

	if (!gSpecActive.Init())
	{
		LLOG_ERROR("gSpecActive.Init error");
		return false;
	}

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	ConnectToCenter();

	ConnectToDb();

	//玩家消息线程开启
	gUserMessageMsg.Start();

	gRLT.Start();

	gInsideNet.Start();

	m_memeoryRecycle.Start();

	LRunnable::Start();
}

//等待
void Work::Join()
{
	//等待玩家消息线程终止
	gUserMessageMsg.Join();

	gRLT.Join();

	gInsideNet.Join();

	m_memeoryRecycle.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	m_centerClient.reset();

	m_gateInfo.clear();
	m_mapGateId.clear();

	gUserMessageMsg.Stop();

	gRLT.Stop();

	gInsideNet.Stop();

	m_memeoryRecycle.Stop();

	LRunnable::Stop();
}

void Work::Clear()
{

}

void Work::Push(void* msg)
{
	LMsg* pMessage = (LMsg*)msg;
	if(pMessage == NULL)
	{
		return;
	}

	LLOG_DEBUG("Work::Push  pMessage->m_msgId = %d", pMessage->m_msgId);
	switch(pMessage->m_msgId)
	{
	/*
	Center to LogicManager
	*/

	case MSG_CE_2_L_USER_LOGIN:
		{
		    LLOG_DEBUG("Work::Push  MSG_CE_2_L_USER_LOGIN=[%d]", pMessage->m_msgId);
			LMsgCe2LUserLogin* pLoginMsg = (LMsgCe2LUserLogin*)pMessage;

			if(pLoginMsg)
			{
				LLOG_ERROR("CE2LM login dispatch1 msg usert id[%d] new[%d] card2[%d] m_unionid[%s] seed[%d]",
					pLoginMsg->user.m_id, pLoginMsg->user.m_new, pLoginMsg->user.m_numOfCard2s, pLoginMsg->user.m_unioid.c_str(), pLoginMsg->m_seed);

				gUserMessageMsg.handlerMessage(pLoginMsg->user.m_unioid, pLoginMsg);
			}
			break;
		}
	case MSG_CE_2_L_GM_CHARGE:
		{
			LMsgCe2LGMCharge* pChargeMsg = (LMsgCe2LGMCharge*)pMessage;
			if(pChargeMsg)
			{
				gUserMessageMsg.handlerMessage(pChargeMsg->m_strUUID, pChargeMsg);
			}
			break;
		}
	case MSG_CE_2_L_GM_COINS:
		{
			LMsgCe2LGMCoins* pCoinsMsg = (LMsgCe2LGMCoins*)pMessage;
			if(pCoinsMsg)
			{
				gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
			}
			break;
		}

	case MSG_CE_2_LMG_UPDATE_COIN:
	{
		LMsgCe2LUpdateCoin* pCoinsMsg = (LMsgCe2LUpdateCoin*)pMessage;
		if (pCoinsMsg)
		{
			gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
		}
		break;
	}

	case MSG_CE_2_LMG_UPDATE_COIN_JOIN_DESK:
	{
		LMsgCe2LUpdateCoinJoinDesk* pCoinsMsg = (LMsgCe2LUpdateCoinJoinDesk*)pMessage;
		if (pCoinsMsg)
		{
			gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
		}
		break;
	}
	case MH_MSG_CE_2_L_UPDATE_GATE_IP:
	{
		MHLMsgCe2LUpdateGateIp * pGateMsg = (MHLMsgCe2LUpdateGateIp *)pMessage;
		if(pGateMsg) gWork.HanderUpdateGateIpFromCenter(pGateMsg);
		break;
	}

	case MSG_CE_2_LMG_CLUB:
	{
		LLOG_ERROR("Work::Push  MSG_CE_2_LMG_CLUB=[%d]   ERROR THIS MESSAGE IS NOUSER", pMessage->m_msgId);
		//HanderCenterClubInfo((LMsgCe2LClubInfo*)msg);
		break;
	}
	//center发送添加俱乐部
	case MSG_CE_2_LMG_ADD_CLUB:
	{
		LLOG_DEBUG("Work::Push  MSG_CE_2_LMG_ADD_CLUB=[%d]", pMessage->m_msgId);
		gClubManager.addClub((LMsgCe2LAddClub*)msg);
		//HanderAddClub((LMsgCe2LAddClub*)msg);
		break;
	}
	case MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE:
	{
		LLOG_DEBUG("Work::Push  MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE=[%d]", pMessage->m_msgId);
		//HanderClubAddPlayType((LMsgCe2LMGClubAddPlayType *)msg);
		gClubManager.addClubPlayType((LMsgCe2LMGClubAddPlayType *)msg);
		break;
	}
	case MSG_CE_2_LMG_CLUB_USER_INFO:
	{
		LLOG_DEBUG("Work::Push  MSG_CE_2_LMG_CLUB_USER_INFO=[%d]", pMessage->m_msgId);
		gClubManager.managerClubUserFromCenter((LMsgCe2LMGClubAddUser *)msg);
		break;
	}
	case  MSG_CE_2_LMG_CLUB_DEL_USER:
	{
		LLOG_DEBUG("Work::Push  MSG_CE_2_LMG_CLUB_DEL_USER=[%d]", pMessage->m_msgId);
		//HanderClubDelUser((LMsgCe2LMGClubDelUser *)msg);
		break;
	}
	case MSG_CE_2_LMG_CLUB_HIDE_PLAYTYPE:
	{
		LLOG_DEBUG("Work::Push  MSG_CE_2_LMG_CLUB_HIDE_PLAYTYPE=[%d]", pMessage->m_msgId);
		HanderClubHidePlayType((LMsgCe2LMGClubHidePlayType *)msg);
		break;
	}
	case MSG_CE_2_LMG_CLUB_MIDIFY_CLUB_NAME:
	{
		LLOG_DEBUG("Work::Push  ClubManager::alterClubName =[%d]", pMessage->m_msgId);
		HanderClubModifyName((LMsgCe2LMGClubModifyClubName *)msg);
		break;
	}
  case MSG_CE_2_LMG_CLUB_MODIFY_CLUB_FEE:
	{
		LLOG_DEBUG("Work::Push MSG_CE_2_LMG_CLUB_MODIFY_CLUB_FEE =[%d]", pMessage->m_msgId);
		HanderClubModifyFeeType((LMsgCe2LMGClubModifyClubFee *)msg);
		break;
	}

  case MSG_CE_2_LMG_DISMISS_DESK:
  {
	  LLOG_DEBUG("Work::Push MSG_CE_2_LMG_DISMISS_DESK =[%d]", pMessage->m_msgId);
	  HanderDismissRoom((LMsgCe2LMGDismissDesk *)msg);
	  break;
  }
  case MSG_CE_2_LMG_DEL_USER:
  {
	  LLOG_ERROR("Work::Push MSG_CE_2_LMG_DEL_USER =[%d]", pMessage->m_msgId);
	  HanderDelUserFromMap((LMsgCe2LMGDelUserFromManager *)msg);
	  break;
  }

  case MSG_CE_2_LMG_CONTROL_MSG:
  {
	  LLOG_ERROR("MSG_CE_2_LMG_CONTROL_MSG=[%d]", pMessage->m_msgId);
	  HanderControlMsgToLogic((LMsgCe2LMGControlMsg*)msg);
	  break;
  }

  case MSG_CE_2_LMG_UPDATE_POINT_RECORD:
  {
	  LMsgLMG2CEUpdatePointRecord* pPointRecord = (LMsgLMG2CEUpdatePointRecord*)pMessage;
	  if (pPointRecord)
	  {
		  gUserMessageMsg.handlerMessage(pPointRecord->m_record.m_userId, pPointRecord);
	  }
	  break;
  }
  case MSG_CE_2_LMG_POINT_RECORD_LOG:
  {
	  LMsgCE2LMGRecordLog* pPointRecord = (LMsgCE2LMGRecordLog*)pMessage;
	  if (pPointRecord)
	  {
		  gUserMessageMsg.handlerMessage(pPointRecord->m_userId, pPointRecord);
	  }
	  break;
  }
  case MSG_CE_2_LMG_UPDATE_CLUB:
  {
	  LMsgCE2LMGClubInfo* pInfo = (LMsgCE2LMGClubInfo*)pMessage;
	  if (pInfo)
	  {
		  gUserMessageMsg.handlerMessage(pInfo->m_clubId, pInfo);
	  }
	  break;
  }
  case MSG_CE_2_LMG_UPDATE_PLAY:
  {
	  LMsgCE2LMGPlayInfo* pInfo = (LMsgCE2LMGPlayInfo*)pMessage;
	  if (pInfo)
	  {
		  gUserMessageMsg.handlerMessage(pInfo->m_clubId, pInfo);
	  }
	  break;
  }

	/*
	Gate to LogicManager
	*/
	case MSG_G_2_L_USER_MSG:
		{
			LMsgG2LUserMsg* pUserMsg = (LMsgG2LUserMsg*)pMessage;
			if(pUserMsg)
			{
				gUserMessageMsg.handlerMessage(pUserMsg->m_strUUID, pUserMsg);
			}
			break;
		}
	case MSG_G_2_L_USER_OUT_MSG:
		{
			LMsgG2LUserOutMsg* pUserOutMsg = (LMsgG2LUserOutMsg*)msg;
			if(pUserOutMsg)
			{
				gUserMessageMsg.handlerMessage(pUserOutMsg->m_strUUID, pUserOutMsg);
			}
			break;
		}


	/*
	LogicServer or CoinsServer to LogicManager
	*/
	case MSG_L_2_LMG_MODIFY_USER_STATE:
		{
			LMsgL2LMGModifyUserState* pStateMsg = (LMsgL2LMGModifyUserState*)pMessage;
			if(pStateMsg)
			{
				gUserMessageMsg.handlerMessage(pStateMsg->m_strUUID, pStateMsg);
			}
			break;
		}
	case MSG_L_2_LMG_MODIFY_USER_CARD:
		{
			LMsgL2LMGModifyCard* pModifyCardMsg = (LMsgL2LMGModifyCard*)pMessage;
			if(pModifyCardMsg)
			{
				gUserMessageMsg.handlerMessage(pModifyCardMsg->m_strUUID, pModifyCardMsg);
			}
			break;
		}
	case MSG_L_2_LMG_ADD_USER_PLAYCOUNT:
		{
			LMsgL2LMGAddUserPlayCount* pPlayCountMsg = (LMsgL2LMGAddUserPlayCount*)pMessage;
			if(pPlayCountMsg)
			{
				gUserMessageMsg.handlerMessage(pPlayCountMsg->m_strUUID, pPlayCountMsg);
			}
			break;
		}

	case MSG_L_2_LMG_MODIFY_USER_CREATED_DESK:
		{
			LMsgL2LMGModifyUserCreatedDesk* pUserModifyUserCreatedDeskMsg = (LMsgL2LMGModifyUserCreatedDesk*)pMessage;
			if (pUserModifyUserCreatedDeskMsg)
			{
				gUserMessageMsg.handlerMessage(pUserModifyUserCreatedDeskMsg->m_strUUID, pUserModifyUserCreatedDeskMsg);
			}
			break;

		}

	case MSG_L_2_LMG_DELETE_USER_CREATED_DESK:
		{
		LMsgL2LMGDeleteUserCreatedDesk* pUserDeleteCreatedDeskMsg = (LMsgL2LMGDeleteUserCreatedDesk*)pMessage;
			if (pUserDeleteCreatedDeskMsg)
			{
				gUserMessageMsg.handlerMessage(pUserDeleteCreatedDeskMsg->m_strUUID, pUserDeleteCreatedDeskMsg);
			}
			break;

		}

	case MSG_L_2_LMG_UPDATE_USER_GPS:
	{
		LMsgL2LMGUpdateUserGPS* pUserUpdateUserGPSMsg = (LMsgL2LMGUpdateUserGPS*)pMessage;
		if (pUserUpdateUserGPSMsg)
		{
			gUserMessageMsg.handlerMessage(pUserUpdateUserGPSMsg->m_strUUID, pUserUpdateUserGPSMsg);
		}
		break;

	}
	case MH_MSG_L_2_LMG_NEW_AGENCY_ACTIVITY_UPDATE_PLAY_COUNT:
	{
		MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * pMsgNewAgencyActivity = (MHLMsgL2LMGNewAgencyActivityUpdatePlayCount*)pMessage;
		if (pMsgNewAgencyActivity)
		{
			gUserMessageMsg.handlerMessage(pMsgNewAgencyActivity->m_strUUID, pMsgNewAgencyActivity);
		}
		break;
	}

	case MSG_L_2_CE_CLUB_FIRST_RED_PACKET:
	{
		LMsgL2CeClubFirstRedPacket * pMRedPacket = (LMsgL2CeClubFirstRedPacket*)pMessage;
		if (pMRedPacket)
		{
			gUserMessageMsg.handlerMessage(pMRedPacket->m_unioid, pMRedPacket);
		}
		break;
	}

	case MSG_L_2_LMG_USER_ADD_CLUB_DESK:
	{
		LLOG_DEBUG("MSG_L_2_LMG_USER_ADD_CLUB_DESK");
		LMsgL2LMGUserAddClubDesk * pUserAddClubDesk=(LMsgL2LMGUserAddClubDesk*)pMessage;
		if (pUserAddClubDesk)
		{
			gUserMessageMsg.handlerMessage(pUserAddClubDesk->m_strUUID, pUserAddClubDesk);
		}
		break;
	}
	case MSG_L_2_LMG_USER_LEAVE_CLUB_DESK:
	{
		LLOG_DEBUG("MSG_L_2_LMG_USER_LEAVE_CLUB_DESK");
		LMsgL2LMGUserLeaveClubDesk * pUserLeaveClubDesk=(LMsgL2LMGUserLeaveClubDesk*)pMessage;
		if (pUserLeaveClubDesk)
		{
			gUserMessageMsg.handlerMessage(pUserLeaveClubDesk->m_strUUID, pUserLeaveClubDesk);
		}
		break;
	}
	case MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK:
	{
		LLOG_DEBUG("MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK");
		LMsgL2LMGUserListLeaveClubDesk * pUserListLeaveClubDesk = (LMsgL2LMGUserListLeaveClubDesk*)pMessage;
		if (pUserListLeaveClubDesk&&!(pUserListLeaveClubDesk->m_strUUIDList.empty()))
		{
			LLOG_DEBUG("MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK  111");
			gUserMessageMsg.handlerMessage(pUserListLeaveClubDesk->m_strUUIDList[0], pUserListLeaveClubDesk);
		}
		break;
	}
	case MSG_L_2_LMG_FRESH_DESK_INFO:
	{
		LLOG_DEBUG("MSG_L_2_LMG_FRESH_DESK_INFO");
		LMsgL2LMGFreshDeskInfo * pFreshDeskInfo = (LMsgL2LMGFreshDeskInfo*)pMessage;
		if (pFreshDeskInfo)
		{
			if (pFreshDeskInfo->m_clubId != 0 && pFreshDeskInfo->m_clubDeskId != 0)
				gUserMessageMsg.handlerMessage(pFreshDeskInfo->m_clubDeskId, pFreshDeskInfo);
		}
		break;
	}
	case  MSG_L_2_LMG_SYNCHRO_DATA:
	{
		HanderLogicSynchroData((LMsgL2LMGSynchroData*)msg);
		break;
	}
	case  MSG_G_2_LMG_SYNCHRO_DATA:
	{
		HanderGateSynchroData((LMsgG2LMGSynchroData*)msg);
		break;
	}

	/*
	DB to LogicManager
	*/
	case MSG_LDB_2_L_VIP_LOG:
		{
			LMsgLBD2LReqVipLog* pVipLogMsg = (LMsgLBD2LReqVipLog*)pMessage;
			if(pVipLogMsg)
			{
				gUserMessageMsg.handlerMessage(pVipLogMsg->m_strUUID, pVipLogMsg);
			}
			break;
		}
	case MSG_LDB_2_LM_POKER_LOG:
	{
		LLOG_DEBUG("MSG_LDB_2_LM_POKER_LOG");
		LMsgLBD2LReqPokerLog* pVipLogMsg=(LMsgLBD2LReqPokerLog*)pMessage;
		if (pVipLogMsg)
		{
			gUserMessageMsg.handlerMessage(pVipLogMsg->m_strUUID, pVipLogMsg);
		}
		break;
	}
	case MSG_LDB_2_LM_RET_CRELOGHIS:
		{
			LMsgLDB2LM_RecordCRELog* pCreLogHisMsg = (LMsgLDB2LM_RecordCRELog*)pMessage;
			if(pCreLogHisMsg)
			{
				gUserMessageMsg.handlerMessage(pCreLogHisMsg->m_strUUID,pCreLogHisMsg);
			}
			break;
		}
	case MSG_LDB_2_L_ROOM_LOG:
		{
			LMsgLBD2LReqRoomLog* pRoomLogMsg = (LMsgLBD2LReqRoomLog*)pMessage;
			if(pRoomLogMsg)
			{
				gUserMessageMsg.handlerMessage(pRoomLogMsg->m_strUUID, pRoomLogMsg);
			}
			break;
		}
	case MSG_LDB_2_L_VIDEO:
		{
			LMsgLDB2LReqVideo* pReqVideoMsg = (LMsgLDB2LReqVideo*)pMessage;
			if(pReqVideoMsg)
			{
				gUserMessageMsg.handlerMessage(pReqVideoMsg->m_strUUID, pReqVideoMsg);
			}
			break;
		}
	case MSG_LDB_2_L_POKER_VIDEO:
	{
		LMsgLDB2LPokerReqVideo* pReqPokerVideoMsg = (LMsgLDB2LPokerReqVideo*)pMessage;
		if (pReqPokerVideoMsg)
		{
			gUserMessageMsg.handlerMessage(pReqPokerVideoMsg->m_strUUID, pReqPokerVideoMsg);
		}
		break;
	}

	case MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS:
		{
			LMsgLDB2LMGBindingRelationships* pRelationshipsMsg = (LMsgLDB2LMGBindingRelationships*)pMessage;
			if(pRelationshipsMsg)
			{
				gUserMessageMsg.handlerMessage(pRelationshipsMsg->m_strUUID, pRelationshipsMsg);
			}
			break;
		}
	case MSG_LDB_2_LM_RET_INFO:
		{
			LMsgLDB_2_LM_RetInfo* pRetInfoMsg = (LMsgLDB_2_LM_RetInfo*)pMessage;
			if(pRetInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pRetInfoMsg->m_strUUID, pRetInfoMsg);
			}
			break;
		}
	case MSG_LDB_2_LMG_USER_MSG:
		{
			LMsgLDB2LMGUserMsg* pUserMsg = (LMsgLDB2LMGUserMsg*)pMessage;
			if(pUserMsg)
			{
				gUserMessageMsg.handlerMessage(pUserMsg->m_strUUID, pUserMsg);
			}
			break;
		}
	case MSG_LDB_2_L_RETSHAREVIDEOID:
		{
			LMsgLDB2LRetShareVideoId* pReqVideoIdMsg = (LMsgLDB2LRetShareVideoId*)pMessage;
			if(pReqVideoIdMsg)
			{
				gUserMessageMsg.handlerMessage(pReqVideoIdMsg->m_strUUID, pReqVideoIdMsg);
			}
			break;
		}
	case MSG_LDB_2_L_RETSHAREVIDEO:
		{
			LMsgLDB2LRetShareVideo* pReqShareVideoMsg = (LMsgLDB2LRetShareVideo*)pMessage;
			if(pReqShareVideoMsg)
			{
				gUserMessageMsg.handlerMessage(pReqShareVideoMsg->m_strUUID, pReqShareVideoMsg);
			}
			break;
		}
	case MH_MSG_LDB_2_L_SHARE_VIDEO_REULST:
	{
		MHLMsgLDB2LShareVideoResult * pResultMsg = (MHLMsgLDB2LShareVideoResult*)pMessage;
		if (pResultMsg)
		{
			gUserMessageMsg.handlerMessage(pResultMsg->m_strUUID, pResultMsg);
		}
		break;
	}

	case MSG_LDB_2_LM_BCAST:
	{
		LMsgLDB2LMBCast* pLMsgLDB2LMBCast = (LMsgLDB2LMBCast*)pMessage;
		if (pLMsgLDB2LMBCast)
		{
			gUserManager.broadcastAllUser(pLMsgLDB2LMBCast);
		}
		break;
	}
	/*
	CoinsServer to LogicManager
	*/
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		{
			LMsgCN2LMGModifyUserCoins* pActiveInfoMsg = (LMsgCN2LMGModifyUserCoins*)pMessage;
			if(pActiveInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pActiveInfoMsg->m_strUUID, pActiveInfoMsg);
			}
			break;
		}
	/*
	Other message
	*/
	default:
		LRunnable::Push(msg);
		break;
	}
}

void Work::Run()
{
	LTime msgTime;
	int iMsgCount = 0;
	int iElapseTime = 0;
	while(!GetStop())
	{
		//这里处理游戏主循环
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			Tick(cur);	
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
			m_memeoryRecycle.recycleMsg(msg);

			//每一万个包输出所用时间
			iMsgCount++;
			if(iMsgCount == 10000)
			{
				LTime now;
				LLOG_ERROR("10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

				iMsgCount = 0;
				iElapseTime = 0;

				msgTime = now;
			}
		}
	}
}

void Work::Tick(LTime& cur)
{
	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		m_1SceTick = cur.MSecs();
		//gUserManager.checkPlayTypeJoinUserInRoom(cur);

		//每天6点执行一次勇士勋章统计
		if (cur.GetHour() == 6 && cur.GetMin() == 0 && cur.GetSec() == 0)
		{
			boost::thread t(boost::bind(&UserManager::statisticsUserMedal, &gUserManager));
			t.join();
			//gUserManager.statisticsUserMedal();
		}
	}

	//5秒循环一次
	if(cur.MSecs() - m_5SecTick > 5000)
	{
		m_5SecTick = cur.MSecs();
		_checkLogicServerState();
		_checkCoinsServerState();		
	}

	//15秒循环一次
	if (cur.MSecs() - m_15SceTick > 15000)
	{
		m_15SceTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30000)  //30*1000 = 30000
	{
		m_30SceTick = cur.MSecs();
		SendGateInfoToCenter();

		if(m_bHadLoadedData)
		{
			_checkCenterHeartBeat();
			_checkLogicDBHeartBeat();
		}
		_checkFreeModel();
		_checkExchActive();
	}

	//三分钟循环一次
	if(cur.MSecs() - m_180SecTick > 180000)   // 3 * 60 * 1000 = 180000
	{
		m_180SecTick = cur.MSecs();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 300000)   // 5*60 * 1000 == 300000
	{
		m_600SceTick = cur.MSecs();
		_checkAndTouchLogicServer();
		_checkLogicServerOnlySupportGameType();
		_checkRLogStart();
		_checkMaxDeskCountLimitForLogicServer();

	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	LLOG_DEBUG("HanderMsg :: msgId=[%d]",msg->m_msgId);
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;

	//////////////////////////////////////////////////////////////////////////
	//center 跟 logicmanager之间的交互
	case MSG_CE_2_L_USER_ID_INFO:
		HanderCenterUserInInfo((LMsgCe2LUserIdInfo*)msg);
		break;
	case MSG_CE_2_L_GM_HORSE://center发送gm跑马灯
		HanderCenterGMHorse((LMsgCe2LGMHorse*) msg);
		break;
	case MSG_CE_2_L_GM_BUYINFO:
		HanderCenterGMBuyInfo((LMsgCe2LGMBuyInfo*) msg);
		break;
	case MSG_CE_2_L_GM_HIDE:
		HanderCenterGMHide((LMsgCe2LGMHide*) msg);
		break;
	case MSG_CE_2_L_SET_GAME_FREE:
		HanderLogicSetGameFree((LMsgCE2LSetGameFree*) msg);
		break;
	case MSG_CE_2_L_SET_PXACTIVE:
		HanderLogicSetPXActive((LMsgCE2LSetPXActive*)msg);
		break;
	case MSG_CE_2_L_SET_OUGCACTIVE:
		HanderLogicSetOUGCActive((LMsgCE2LSetOUGCActive*)msg);
		break;
	case MSG_CE_2_L_SET_EXCHACTIVE:
		HanderLogicSetExchActive((LMsgCE2LSetExchActive*) msg);
		break;
	case MSG_CE_2_L_SET_ACTIVITY:
		HanderLogicSetActivity((LMsgCE2LSetActivity*) msg);
		break;
	case MSG_CE_2_LMG_CONFIG:
		LLOG_DEBUG("HanderMsg :: MSG_CE_2_LMG_CONFIG");
		HanderCenterConfig((LMsgCE2LMGConfig*)msg);
		break;
	case MSG_LMG_2_L_SPEC_ACTIVE:
		LLOG_DEBUG("HanderMsg :: MSG_LMG_2_L_SPEC_ACTIVE");
		HanderCenterSpecActive((LMsgLMG2LSpecActive*)msg);
		break;

		//////////////////////////////////////////////////////////////////////////
		//logic 跟 logicmanager之间的交互
	case MSG_L_2_LMG_LOGIN:
		HanderLogicLogin((LMsgL2LMGLogin*)msg);
		break;
	case MSG_L_2_LMG_RECYLE_DESKID:
		{
			LLOG_DEBUG("MSG_L_2_LMG_RECYLE_DESKID");
			LMsgL2LMGRecyleDeskID* pRecyleMsg = (LMsgL2LMGRecyleDeskID*)msg;
			if(pRecyleMsg)
			{
				Lint iLogicServerId = gDeskManager.RecycleDeskId(pRecyleMsg->m_deskID);
				if(iLogicServerId != INVALID_LOGICSERVERID)
				{
					delDeskCountOnLogicServer(iLogicServerId);
				}
				//俱乐部房间解散
				gClubManager.recycleClubDeskId(pRecyleMsg);
			}

			break;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//gate 跟 logicmanager之间的交互
	case MSG_G_2_LMG_LOGIN:
		HanderGateLogin((LMsgG2LMGLogin*)msg);
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;
	case MSG_HEARDBEAT_REPLY:
		HanderHearBeatReply((LMsgHeartBeatReplyMsg*)msg);
		break;
	case MSG_CN_2_LMG_LOGIN:
		HanderCoinsLogin((LMsgCN2LMGLogin*)msg);
		break;
	case MSG_CN_2_LMG_FREE_DESK_REQ:
		HanderCoinFreeDeskReq((LMsgCN2LMGFreeDeskReq*)msg);
		break;
	case MSG_CN_2_LMG_RECYCLE_DESK:
		HanderCoinRecycleDesk((LMsgCN2LMGRecycleDesk*)msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if (msg->m_sp == m_centerClient)
	{
		_reconncetCenter();
	}
	else if(msg->m_sp == m_dbClient)
	{
		_reconnectDB();
	}
	else if( m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		// 金币服务器断开
		HanderCoinsLogout(msg);
	}
	else
	{
		HanderLogicLogout(msg);
		HanderGateLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(m_dbClient == msg->m_sp)
	{
		LMsgLMG2LdbLogin msg;
		msg.m_key = gConfig.GetDBKey();
		msg.m_serverID = gConfig.GetServerID();
		SendMsgToDb(msg);
	}
	else if(m_centerClient == msg->m_sp)
	{
		LMsgL2CeLogin login;
		login.m_ID = gConfig.GetServerID();
		login.m_key = gConfig.GetCenterKey();
		login.m_ServerName = gConfig.GetServerName();
		login.m_needLoadUserInfo = m_bHadLoadedData ? 0 : 1;
		SendToCenter(login);

		if(m_bHadLoadedData)
		{
			SendGateInfoToCenter();
		}
	}

	m_mapReconnect.erase(msg->m_sp);
}

void Work::ConnectToCenter()
{
	m_centerClient = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectCenter begin %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	if(m_centerClient->Connect(gConfig.GetCenterIp(), gConfig.GetCenterPort()))
	{
		LLOG_ERROR("Work::ConnectCenter end %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	}
	else
	{
		LLOG_ERROR("Fail to connect center");
		Stop();
	}
}

void Work::SendToCenter(LMsg& msg)
{
	boost::mutex::scoped_lock l(m_mutexCenterClient);

	if(m_centerClient && m_centerClient->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		m_centerClient->Send(msg.GetSendBuff());
	}
}

void Work::HanderCenterUserInInfo(LMsgCe2LUserIdInfo*msg)
{
	if(msg == NULL)
	{
		return;
	}

	for(Lint i = 0; i < msg->m_count; ++i)
	{
		boost::shared_ptr<UserBaseInfo> baseInfo(new UserBaseInfo(msg->m_info[i]));
		gUserManager.addUserBaseInfo(baseInfo);
	}

	if(msg->m_hasSentAll == 1)
	{
		m_bHadLoadedData = true;
		//加载完用户，请求logic，gate依存信息
		SendRequestInfoToAllGateOrLogic(2);
		SendRequestInfoToAllGateOrLogic(1);
		
	}
}


//center GM设置跑马灯
void Work::HanderCenterGMHorse(LMsgCe2LGMHorse* msg)
{
	if(msg == NULL)
	{
		return;
	}

	LLOG_DEBUG("Work::HanderCenterGMHorse %s",msg->m_str.c_str());
	
	gRuntimeInfoMsg.setHorseInfoAndNotify(msg->m_str);
}
void Work::HanderCenterGMBuyInfo(LMsgCe2LGMBuyInfo* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMBuyInfo %s", msg->m_str.c_str());
	gRuntimeInfoMsg.setBuyInfo(msg->m_str);
}

void Work::HanderCenterGMHide(LMsgCe2LGMHide* msg)
{
	gRuntimeInfoMsg.setHide(msg->m_hide);
}

void Work::HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	int fromwho = msg->m_fromWho;
	if ( fromwho < HEARBEAT_WHO_UNKNOWN || fromwho > HEARBEAT_WHO_NUM )
	{
		fromwho = HEARBEAT_WHO_UNKNOWN;
	}
	LLOG_ERROR("Request heartbeat from %s -- %d", HearBeat_Name[fromwho], msg->m_iServerId);

	LMsgHeartBeatReplyMsg msgReply;
	msgReply.m_fromWho = HEARBEAT_WHO_LOGICMANAGER;
	msgReply.m_iServerId = 0;

	msg->m_sp->Send(msgReply.GetSendBuff());
}

void Work::HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	int fromwho = msg->m_fromWho;
	if ( fromwho < HEARBEAT_WHO_UNKNOWN || fromwho > HEARBEAT_WHO_NUM )
	{
		fromwho = HEARBEAT_WHO_UNKNOWN;
	}
	LLOG_ERROR("Reply heartbeat from %s -- %d", HearBeat_Name[fromwho], msg->m_iServerId);

	m_mapHeartBeat.erase(msg->m_sp);
}

void Work::HanderUpdateGateIpFromCenter(MHLMsgCe2LUpdateGateIp * msg)
{
	LLOG_INFO("***Recevice center update gate ip. serverid:%d gateip:%s gaofang_flag:%d", msg->m_server_id, msg->m_gate_ip.c_str(), msg->m_gaofang_flag);
	int nServerId = gConfig.GetServerID();
	if (msg->m_server_id != gConfig.GetServerID())
	{
		LLOG_ERROR("Error server id %d:%d", nServerId, msg->m_server_id);
		return;
	}
	if (msg->m_gate_ip.empty())
	{
		LLOG_ERROR("Error empty gate ip");
		return;
	}

	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

		auto itGateInfo = m_gateInfo.begin();
		for (;itGateInfo != m_gateInfo.end();itGateInfo++)
		{
			if (itGateInfo->second.m_gaofang_flag == msg->m_gaofang_flag)
			{
				LLOG_INFO("Update gate ip succesfully. id:%d old:%s new:%s", itGateInfo->first, itGateInfo->second.m_ip.c_str(), msg->m_gate_ip.c_str());
				itGateInfo->second.m_ip = msg->m_gate_ip;
			}			 
		}
	}

	//Update to center
	SendGateInfoToCenter();
}

//处理Gate成功连接上LogicManager
void Work::HanderGateLogin(LMsgG2LMGLogin* msg)
{
	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}
	bool isRequestDataToGate = true;
	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

		//检测过来注册的GateID是否会引起冲突
		Lint t_remotePort = msg->m_sp->GetRemotePort();

		auto t_gateIdIter = m_gateInfo.find(msg->m_id);
		if (t_gateIdIter != m_gateInfo.end())
		{
			//TODO:给客户端发送一个自毁消息
			LMsgKillClient killClienet;
			msg->m_sp->Send(killClienet.GetSendBuff());
			msg->m_sp->Stop();
			LLOG_ERROR("Work::HanderGateLogin() ID Conflict Error!!!, New Start Logic Port Or Id Conflict!!! port=[%d], gateId=[%d]", t_remotePort, msg->m_id);
			return;
		}


		auto itGateInfo = m_gateInfo.find(msg->m_id);
		if(itGateInfo != m_gateInfo.end()) //重连
		{
			LLOG_ERROR("Gate reconnect logic manager. Id = %d", itGateInfo->second.m_id);
			itGateInfo->second.m_sp->Stop();
			m_gateInfo.erase(itGateInfo);
			isRequestDataToGate = false;
		}


		LLOG_ERROR("Work::HanderGateLogin Succeed! %d", msg->m_id);

		//广播GATE消息
		GateInfo info;
		info.m_id = msg->m_id;
		info.m_ip = msg->m_ip;
		info.m_port = msg->m_port;
		info.m_userCount = 0;
		info.m_sp = msg->m_sp;
		info.m_gaofang_flag = msg->m_gaofang_flag;
		m_gateInfo[info.m_id] = info;
		
		m_mapGateId.insert(msg->m_id);

	}

	//将Manager上所有注册的Logic信息发送给Gate
	SendLogicInfoToGates(msg->m_id);

	//将Manager上所有注册的Gate信息发送给Center
	SendGateInfoToCenter();

	SendCoinsInfoToGates(msg->m_id);

	//请求gate上数据

	if (isRequestDataToGate)
		SendRequestInfoToGateOrLogic(1, msg->m_id, 1);

}

//处理Gate断开
void Work::HanderGateLogout(LMsgKick* msg)
{
	int iDelId = -1;
	
	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);
		for(auto itGateInfo = m_gateInfo.begin(); itGateInfo != m_gateInfo.end(); itGateInfo++)
		{
			if(itGateInfo->second.m_sp == msg->m_sp)
			{
				iDelId = itGateInfo->first;
				m_gateInfo.erase(itGateInfo);
				break;
			}
		}
	}

	if(iDelId != -1)
	{
		LLOG_ERROR("Work::HanderGateLogout Succeed! %d", iDelId);
		SendGateInfoToCenter();
	}
}

void Work::SendGateInfoToCenter()
{
	LMsgL2CeGateInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;

	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

		for(auto itGate = m_gateInfo.begin(); itGate != m_gateInfo.end(); ++itGate)
		{
			itGate->second.m_userCount = gRuntimeInfoMsg.getOnlineNum(itGate->first);
			info.m_gate[info.m_count++] = (itGate->second);
		}
	}
	
	SendToCenter(info);
}

Lint Work::getGateIdBySp(LSocketPtr sp)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	for(auto itGateInfo = m_gateInfo.begin(); itGateInfo != m_gateInfo.end(); itGateInfo++)
	{
		if(itGateInfo->second.m_sp == sp)
		{
			return itGateInfo->first;
		}
	}

	return INVALID_GATEID;
}

void Work::SendMessageToAllGate(LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	auto gate = m_gateInfo.begin();
	for (; gate != m_gateInfo.end(); ++gate)
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToGate(Lint gateID, LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	auto gate = m_gateInfo.find(gateID);
	if ( gate != m_gateInfo.end())
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

//////////////////////////////////////////////////////////////////////////
//DB链接相关
void Work::ConnectToDb()
{
	m_dbClient = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectToDb begin %s:%d", gConfig.GetDBIp().c_str(), gConfig.GetDBPort());
	if(m_dbClient->Connect(gConfig.GetDBIp(), gConfig.GetDBPort()))
	{
		LLOG_ERROR("Connect DB successfully");
	}
	else
	{
		LLOG_ERROR("Fail to connect db");
		Stop();
	}
}

void Work::SendMsgToDb(LMsg& msg)
{
	boost::mutex::scoped_lock l(m_mutexDBClient);

	if(m_dbClient && m_dbClient->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		m_dbClient->Send(msg.GetSendBuff());
	}
}

//处理Logic成功连接上LogicManager
void Work::HanderLogicLogin(LMsgL2LMGLogin* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}
	bool isRequestDataToLogic = true;
	{
		boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

		//检测过来注册的LogicID是否会引起冲突
		Lint t_remotePort = msg->m_sp->GetRemotePort();

		auto t_logicIdIter = m_logicServerInfo.find(msg->m_id);
		if (t_logicIdIter != m_logicServerInfo.end() && t_logicIdIter->second.m_tickTime == 0)
		{
			//TODO:给客户端发送一个自毁消息
			LMsgKillClient killClienet;
			msg->m_sp->Send(killClienet.GetSendBuff());
			msg->m_sp->Stop();
			LLOG_ERROR("Work::HanderLogicLogin() ID Conflict Error!!!, New Start Client Id Conflict!!! port=[%d], logicId=[%d]", t_remotePort, msg->m_id);
			return;
		}

		/*
		for (auto t_logicIdIter = m_logicServerInfo.begin(); t_logicIdIter != m_logicServerInfo.end(); t_logicIdIter++)
		{
			if ((t_logicIdIter->first == msg->m_id || t_logicIdIter->second.m_logicInfo.m_sp->GetRemotePort() == t_remotePort) &&
				t_logicIdIter->second.m_tickTime == 0)
			{
				msg->m_sp->Stop();
				LLOG_ERROR("Work::HanderGateLogin() Conflict Error!!!, New Start Logic Port Or Id Conflict!!! port=[%d], logicId=[%d]", t_remotePort, msg->m_id);
				return;
			}
		}
		*/

		//原来的逻辑
		auto itLogic = m_logicServerInfo.find(msg->m_id);
		if(itLogic != m_logicServerInfo.end())
		{
			itLogic->second.m_logicInfo.m_sp->Stop();
			m_logicServerInfo.erase(itLogic);
			isRequestDataToLogic = false;
		}

		//广播Logic消息
		LLOG_ERROR("Work::HanderLogicLogin Succeed! %d", msg->m_id);
		LOGIC_SERVER_INFO logicInfo;

		logicInfo.m_logicInfo.m_id = msg->m_id;
		logicInfo.m_logicInfo.m_ip = msg->m_ip;
		logicInfo.m_logicInfo.m_port = msg->m_port;
		logicInfo.m_logicInfo.m_deskCount = 0;
		logicInfo.m_logicInfo.m_sp = msg->m_sp;
		logicInfo.m_logicInfo.m_flag = 0;
		logicInfo.m_logicInfo.m_type=msg->m_type;
		logicInfo.m_tickTime = 0;
		m_logicServerInfo[logicInfo.m_logicInfo.m_id] = logicInfo;
		gActiveManager.PXSendConfig(logicInfo.m_logicInfo.m_id);		// logic登录，发送牌型活动信息


		//特殊活动
		gSpecActive.sendToLogic(logicInfo.m_logicInfo.m_id);
	}
	
	SendLogicInfoToGates();

	SendCoinsInfoToLogic( msg->m_id );

	if (isRequestDataToLogic)
		SendRequestInfoToGateOrLogic(2, msg->m_id, 1);
}

void Work::HanderLogicLogout(LMsgKick* msg)
{
	if (msg == NULL)
	{
		return;
	}

	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end(); itLogic++)
	{
		if(itLogic->second.m_logicInfo.m_sp == msg->m_sp)
		{
			LLOG_ERROR("%d Logic server is kicked", itLogic->second.m_logicInfo.m_id);

			//仅仅只是设置踢出时间，然后3分钟都连接不上才进行删除
			LTime curTime;
			itLogic->second.m_tickTime = curTime.MSecs();
		}
	}
}

void Work::_checkLogicServerState()
{
	bool bDelLogic = false;

	{
		boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

		LTime cur;
		for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end();)
		{
			if(itLogic->second.m_tickTime == 0)
			{
				itLogic++;
				continue;
			}

			Llong lDiff = cur.MSecs() - itLogic->second.m_tickTime;
			if(lDiff >= 3 * 60 * 1000)
			{
				std::vector<ClubInfos>  tempClubInfos;
				gDeskManager.RecycleAllDeskIdOnLogicServer(itLogic->second.m_logicInfo.m_id, tempClubInfos);
				m_logicServerInfo.erase(itLogic++);
				bDelLogic = true;

				for (int i=0; i<tempClubInfos.size(); i++)
				{
					gClubManager.recycleClubDeskId(tempClubInfos[i].m_clubDeskId);
				}
			}
			else
			{
				itLogic++;
			}
		}
	}
	
	if(bDelLogic)
	{
		SendLogicInfoToGates();
	}
}

void Work::_checkAndTouchLogicServer()
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

	if(m_logicServerInfo.empty())
		return;

	LIniConfig	m_ini;
	std::list<std::string> fields;
	m_ini.LoadFile("logic_manager_config.ini");
	Lstring line=m_ini.GetString("FailoverLogics","");

	std::vector<Lstring> des;
	L_ParseString(line, des, ",");

	//添加，支持区间  eg: 1-10
	for (int i=0; i<des.size(); i++)
	{
		std::vector<Lstring> subDes;
		L_ParseString(des[i], subDes,"-");
		if (subDes.size()!=2)continue;
		if (atoi(subDes[0].c_str())==0)continue;
		des[i]=subDes[0];
		for (int j= atoi(subDes[0].c_str())+1; j<=atoi(subDes[1].c_str()); j++)
		{
			des.push_back(std::to_string(j));
		}
	}

	//for test by wyz
	std::string  tempLog;
	for (int i=0; i<des.size(); i++)
	{
		std::string temp="|"+des[i];
		tempLog.append(temp);
	}
	LLOG_DEBUG("Logwyz  ... failLove[%s]", tempLog.c_str());
	

	//去除某些逻辑
	auto iBegin = m_logicServerInfo.begin();
	for (; iBegin != m_logicServerInfo.end(); ++iBegin)
	{
		iBegin->second.m_logicInfo.m_flag = 0;
		for (Lsize i = 0; i < des.size(); i ++)
		{
			if (atoi(des[i].c_str()) == iBegin->first)
			{
				iBegin->second.m_logicInfo.m_flag = -1;
				break;
			}
		}
	}
	
	//打印桌子数
	auto iLogic = m_logicServerInfo.begin();
	for (; iLogic != m_logicServerInfo.end(); ++iLogic)
	{
		LLOG_ERROR("LogicIndex: %d Current Desk: %d", iLogic->first, iLogic->second.m_logicInfo.m_deskCount);
	}
}

void Work::SendLogicInfoToGates(Lint nGateID)
{
	LMsgLMG2GateLogicInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;

	{
		boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

		for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end(); itLogic++)
		{
			if(itLogic->second.m_tickTime != 0)
			{
				continue;
			}
			info.m_logic[info.m_count++] = itLogic->second.m_logicInfo;
		}
	}

	if (nGateID > 0)
	{
		SendMessageToGate(nGateID, info);
	}
	else
	{
		SendMessageToAllGate(info);
	}
}

void Work::HanderCoinsLogin(LMsgCN2LMGLogin* msg)
{
	LMsgLMG2CNLogin login;
	if (msg->m_key.empty())
	{
		login.m_result = 1;
		msg->m_sp->Send( login.GetSendBuff() );

		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderCoinsLogin key error %s", msg->m_key.c_str());
		return;
	}

	if ( m_coinsServer.m_sp )
	{
		if ( m_coinsServer.m_closeTickTime == 0 )
		{
			login.m_result = 2;
			msg->m_sp->Send( login.GetSendBuff() );

			msg->m_sp->Stop();
			// 金币服务器有连接 并且没有关闭倒计时，说明金币服务器重复连接了
			LLOG_ERROR("Work::HanderCoinsLogin CoinsServer already Exist");
			return;
		}
	}

	LLOG_ERROR("Work::HanderCoinsLogin Succeed! %s", msg->m_ip.c_str());

	// 桌子数不一致
	if ( gDeskManager.GetCoinsDeskSize() != msg->m_deskcount )
	{
		LLOG_ERROR("Work::HanderCoinsLogin desk count error, coinsserver count=%d, this count=%d", msg->m_deskcount, gDeskManager.GetCoinsDeskSize() );

		// 金币桌内的玩家状态 待完善

		// 回收金币桌子ID
		gDeskManager.RecycleCoinsDeskId();
	}

	m_coinsServer.m_ip = msg->m_ip;
	m_coinsServer.m_port = msg->m_port;
	m_coinsServer.m_sp = msg->m_sp;
	m_coinsServer.m_closeTickTime = 0;

	login.m_result = 0;
	login.m_deskcount = gDeskManager.GetCoinsDeskSize();
	SendMessageToCoinsServer( login );

	SendCoinsInfoToGates();
	SendCoinsInfoToLogic();
}

void Work::HanderCoinsLogout(LMsgKick* msg)
{
	if (msg == NULL || !m_coinsServer.m_sp || msg->m_sp != m_coinsServer.m_sp)
	{
		return;
	}

	//仅仅只是设置踢出时间，然后3分钟都连接不上才进行删除
	LTime curTime;
	m_coinsServer.m_closeTickTime = curTime.MSecs();
}

void Work::HanderCoinFreeDeskReq(LMsgCN2LMGFreeDeskReq* msg)
{
	LLOG_ERROR( "Work::HanderCoinFreeDeskReq req coindesk, reqcount=%d, count=%d", msg->m_count, gDeskManager.GetCoinsDeskSize() );

	std::vector<Lint> deskid;
	gDeskManager.GetFreeCoinDeskID( msg->m_count, deskid );

	LMsgLMG2CNFreeDeskReply reply;
	reply.m_gameType = msg->m_gameType;
	for ( auto it = deskid.begin(); it != deskid.end(); ++it )
	{
		Lint iLogicServerId = gWork.allocateLogicServer( msg->m_gameType );
		if(iLogicServerId == INVALID_LOGICSERVERID)
		{
			continue;
		}
		CoinsDesk desk;
		desk.m_id = *it;
		desk.m_logicID = iLogicServerId;
		reply.m_desk.push_back( desk );
	}
	SendMessageToCoinsServer( reply );
}

void Work::HanderCoinRecycleDesk(LMsgCN2LMGRecycleDesk* msg)
{
	gDeskManager.RecycleCoinsDeskId( msg->m_deskid );
}

void Work::_checkCoinsServerState()
{
	if ( !m_coinsServer.m_sp )
	{
		return;
	}

	if( m_coinsServer.m_closeTickTime == 0 )
	{
		return;
	}

	LTime cur;
	Llong lDiff = cur.MSecs() - m_coinsServer.m_closeTickTime;
	if(lDiff >= 3 * 60 * 1000)
	{
		m_coinsServer.m_sp.reset();
		m_coinsServer.m_ip = "";
		m_coinsServer.m_port = 0;
		m_coinsServer.m_closeTickTime = 0;

		// 金币桌内的玩家状态 待完善

		// 回收金币桌子ID
		gDeskManager.RecycleCoinsDeskId();

		SendCoinsInfoToGates();
		SendCoinsInfoToLogic();
	}
}

void Work::SendCoinsInfoToGates(Lint nGateID)
{
	LMsgLMG2GLCoinsServerInfo info;
	info.m_conis.m_ip = m_coinsServer.m_ip;
	info.m_conis.m_port = m_coinsServer.m_port;

	if (nGateID > 0)
	{
		SendMessageToGate(nGateID, info);
	}
	else
	{
		SendMessageToAllGate(info);
	}
}

void Work::SendCoinsInfoToLogic(Lint nLogicID)
{
	LMsgLMG2GLCoinsServerInfo info;
	info.m_conis.m_ip = m_coinsServer.m_ip;
	info.m_conis.m_port = m_coinsServer.m_port;

	if (nLogicID > 0)
	{
		SendMessageToLogic(nLogicID, info);
	}
	else
	{
		SendMessageToAllLogic(info);
	}
}

bool Work::IsCoinsServerConnected()
{
	if ( m_coinsServer.m_sp && m_coinsServer.m_sp->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED )
	{
		return true;
	}
	return false;
}

void Work::SendMessageToCoinsServer(LMsg& msg)
{
	if ( m_coinsServer.m_sp && m_coinsServer.m_sp->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED )
	{
		m_coinsServer.m_sp->Send(msg.GetSendBuff());
	}
}

bool Work::isLogicServerExist(int iLogicServerId)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogicServer = m_logicServerInfo.find(iLogicServerId);
	if(itLogicServer == m_logicServerInfo.end())
	{
		return false;
	}

	return true;
}

void Work::SendMessageToAllLogic(LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogic = m_logicServerInfo.begin();
	for (; itLogic != m_logicServerInfo.end(); ++itLogic)
	{
		itLogic->second.m_logicInfo.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToLogic(Lint iLogicServerId, LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogic = m_logicServerInfo.find(iLogicServerId);
	if(itLogic != m_logicServerInfo.end() && itLogic->second.m_tickTime == 0)
	{
		itLogic->second.m_logicInfo.m_sp->Send(msg.GetSendBuff());
	}
}

Lint Work::allocateLogicServer(int iGameType)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

	Lint nMiniCouunt = 9999999;
	bool bGameTypeIsLimited = false;

	auto itAllocLogic = m_logicServerInfo.end();
	LLOG_DEBUG("Work::allocateLogicServer  m_mapLogicServerOnlySupportType size=[%d]", m_mapLogicServerOnlySupportType.size());
	for(auto itOnlySurpport = m_mapLogicServerOnlySupportType.begin(); itOnlySurpport != m_mapLogicServerOnlySupportType.end(); itOnlySurpport++)
	{
		LLOG_DEBUG("in for");
		if(itOnlySurpport->second.find(iGameType) != itOnlySurpport->second.end())
		{
			auto itAllocLogicTmp = m_logicServerInfo.find(itOnlySurpport->first);
			if( itAllocLogicTmp != m_logicServerInfo.end()						&&
				itAllocLogicTmp->second.m_logicInfo.m_deskCount < nMiniCouunt	&&
				itAllocLogicTmp->second.m_logicInfo.m_flag != -1				&&
				itAllocLogicTmp->second.m_tickTime == 0)
			{
				nMiniCouunt = itAllocLogicTmp->second.m_logicInfo.m_deskCount;
				itAllocLogic = itAllocLogicTmp;
			}

			bGameTypeIsLimited = true;
		}
	}
	LLOG_DEBUG("out  for");
	if(itAllocLogic != m_logicServerInfo.end())
	{
		itAllocLogic->second.m_logicInfo.m_deskCount++;
		return itAllocLogic->first;
	}

	if(bGameTypeIsLimited)	//如果已经被限制分配，则只能分配到规定的服务器中，如果没有此服务器则失败
	{
		return INVALID_LOGICSERVERID;
	}

	nMiniCouunt = 9999999;
	Lint max_desk_count_limit = this->m_maxDeskCountLimitForEachLogic;
	Lint all_used_desk_count = 0;
	LLOG_DEBUG("Work::allocateLogicServer  m_logicServerInfo size=[%d],iGameType=[%d]", m_logicServerInfo.size(), iGameType);
	for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end(); itLogic++)
	{
		if( itLogic->second.m_logicInfo.m_deskCount < nMiniCouunt	&& 
			itLogic->second.m_logicInfo.m_flag != -1				&&
			itLogic->second.m_tickTime == 0							&&
			m_mapLogicServerOnlySupportType.find(itLogic->first) == m_mapLogicServerOnlySupportType.end()  &&
			( (iGameType<1000 && itLogic->second.m_logicInfo.m_type == iGameType)  //指定的logic
				||(iGameType<1000 &&itLogic->second.m_logicInfo.m_type==LOGIC_SERVER_QP)    //棋牌通用logic
				|| (iGameType>1000&&itLogic->second.m_logicInfo.m_type==LOGIC_SERVER_MJ)    //麻将通用logic
			)    
			)	//此玩法不能出现在限制服务器中
		{
			nMiniCouunt = itLogic->second.m_logicInfo.m_deskCount;
			itAllocLogic = itLogic;
		}

		if (itLogic->second.m_logicInfo.m_deskCount >= max_desk_count_limit)
		{
			LLOG_ERROR("LogicServer desk count has reached max limit count: %d, serverID: %d", max_desk_count_limit, itLogic->first);
		}
		else
		{
			LLOG_DEBUG("LogicServer desk count: %d, serverID: %d", itLogic->second.m_logicInfo.m_deskCount, itLogic->first);
		}
		all_used_desk_count += itLogic->second.m_logicInfo.m_deskCount;
	}

	size_t server_count = m_logicServerInfo.size();
	Lint total_desk_count = server_count * max_desk_count_limit;
	Lint remain_desk_count = total_desk_count - all_used_desk_count;
	Ldouble remain_desk_rate = 1.0;
	if(total_desk_count > 0) remain_desk_rate = (Ldouble)remain_desk_count / (Ldouble)(total_desk_count);

	// 桌子数量小于10%, 报警间隔设置为1分钟
	if ( remain_desk_rate < 0.10f  && (GetCurTime().Secs() - m_last_alert_time) > 60)
	{
		MHLOG_DESK("****Start Alert, desk will not enough total_count:%d, reamin_count:%d reamin_rate: %f", total_desk_count, remain_desk_count, remain_desk_rate);
		Lstring result;
		Lstring httpUrl(_MH_ALERT_URL_ROOM_NOT_ENOUGH);
		//HttpRequest::Instance().HttpGet(httpUrl, result);
		m_last_alert_time = GetCurTime().Secs();
	}

	if(itAllocLogic != m_logicServerInfo.end() && nMiniCouunt < max_desk_count_limit)
	{
		LLOG_DEBUG("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh");
		itAllocLogic->second.m_logicInfo.m_deskCount++;		//分配桌子后增加一
		return itAllocLogic->first;
	}

	if (remain_desk_rate > 0.1f) // 不是桌子数量引起的，不报警，可能是保护了服务器桌子 m_flag = -1;
	{
	}
	else if (GetCurTime().MSecs() - m_last_alert_time > 60)
	{
		// 代码走到这里，分配不到最小数量的桌子，报警吧
		MHLOG_DESK("****Start Alert, no available logic server to use");
		Lstring httpUrl(_MH_ALERT_URL_ROOM_NOT_ENOUGH);
		Lstring result;
		//HttpRequest::Instance().HttpGet(httpUrl, result);
		m_last_alert_time = GetCurTime().Secs();
	}
	return INVALID_LOGICSERVERID;
}

void Work::delDeskCountOnLogicServer(Lint iLogicServerId, int iDelCount)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

	auto itLogicServer = m_logicServerInfo.find(iLogicServerId);
	if(itLogicServer != m_logicServerInfo.end())
	{
		itLogicServer->second.m_logicInfo.m_deskCount -= iDelCount;
		if(itLogicServer->second.m_logicInfo.m_deskCount < 0)
		{
			itLogicServer->second.m_logicInfo.m_deskCount = 0;
		}
	}
}

/////////////////////////////////////

void Work::HanderLogicSetGameFree(LMsgCE2LSetGameFree* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gRuntimeInfoMsg.setFreeTimeAndNotify(msg->m_strFreeSet);
}

void Work::HanderLogicSetPXActive(LMsgCE2LSetPXActive* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gActiveManager.PXReloadConfig(msg->m_strActiveSet);
}

void Work::HanderLogicSetOUGCActive(LMsgCE2LSetOUGCActive* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gActiveManager.OUGCReloadConfig(msg->m_strActiveSet);
}

void Work::HanderLogicSetExchActive( LMsgCE2LSetExchActive* msg )
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gActiveManager.ExchReloadConfig(msg->m_strActiveSet);
}

void Work::HanderLogicSetActivity(LMsgCE2LSetActivity* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}
	// 直接转发给DB
	LMsgCE2LSetActivity active;
	active.m_ServerID = msg->m_ServerID;
	active.m_activityId = msg->m_activityId;
	active.m_strActivity = msg->m_strActivity;
	gWork.SendMsgToDb( *msg );
}

void Work::HanderCenterConfig(LMsgCE2LMGConfig* msg)
{
	gMjConfig.UpdateConfigFromCenter(msg);
}



void Work::_initTickTime()
{
	LTime cur;
	m_1SceTick   = cur.MSecs();
	m_5SecTick   = cur.MSecs();
	m_15SceTick  = cur.MSecs();
	m_30SceTick  = cur.MSecs();
	m_180SecTick = cur.MSecs();
	m_600SceTick = cur.MSecs();
}

void Work::_reconnectDB()
{
	if(!m_dbClient)
	{
		LLOG_ERROR("DB socket is null");
		return;
	}

	LLOG_ERROR("Reconnect DB...");
	m_mapReconnect.erase(m_dbClient);
	m_mapHeartBeat.erase(m_dbClient);

	{
		boost::mutex::scoped_lock l(m_mutexDBClient);
		m_dbClient->Stop();
		m_dbClient = gInsideNet.GetNewSocket();
		m_dbClient->AsyncConnect(gConfig.GetDBIp(), gConfig.GetDBPort());
	}
	
	m_mapReconnect[m_dbClient] = true;
}

void Work::_reconncetCenter()
{
	if(!m_centerClient)
	{
		LLOG_ERROR("Center socket is null");
		return;
	}

	LLOG_ERROR("Reconnect center...");
	m_mapReconnect.erase(m_centerClient);
	m_mapHeartBeat.erase(m_centerClient);

	{
		boost::mutex::scoped_lock l(m_mutexCenterClient);
		m_centerClient->Stop();
		m_centerClient = gInsideNet.GetNewSocket();
		m_centerClient->AsyncConnect(gConfig.GetCenterIp(), gConfig.GetCenterPort());
	}

	m_mapReconnect[m_centerClient] = true;
}

void Work::_checkCenterHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连center，则不进行心跳
	if(m_mapReconnect.find(m_centerClient) != m_mapReconnect.end())
	{
		return;
	}

	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_centerClient) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("Center network had problem. Reconnect....");
		_reconncetCenter();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICMANAGER;
	msgHeatbeat.m_iServerId = 0;

	SendToCenter(msgHeatbeat);

	m_mapHeartBeat[m_centerClient] = true;
}

void Work::_checkLogicDBHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连LogicDB，则不进行心跳
	if(m_mapReconnect.find(m_dbClient) != m_mapReconnect.end())
	{
		return;
	}

	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_dbClient) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("LogicDB network had problem. Reconnect....");
		_reconnectDB();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICMANAGER;
	msgHeatbeat.m_iServerId = 0;

	SendMsgToDb(msgHeatbeat);

	m_mapHeartBeat[m_dbClient] = true;
}

// 检查免费时间是否从有效变无效,通知客户端,30秒检查一次
void Work::_checkFreeModel()
{
	if(gRuntimeInfoMsg.updateFree())	
	{
		LMsgLMG2GHorseInfo msgFreeTime;
		msgFreeTime.m_type = 1;
		msgFreeTime.m_str = gRuntimeInfoMsg.getFreeTime();
		gWork.SendMessageToAllGate(msgFreeTime);
	}
}

void Work::_checkExchActive()
{
	static bool bExchActiveRun = false;
	bool nowRun = gActiveManager.CheckTimeValid_Exch();
	if(bExchActiveRun && !nowRun)     //活动结束 推送
	{
		LMsgLMG2GHorseInfo msgExch;
		msgExch.m_type = 2;
		msgExch.m_str = "";
		gWork.SendMessageToAllGate(msgExch);
	}
	bExchActiveRun = nowRun;
}

//Config format
//OnlySupportGameType = LogicId_GameType|LogicId_GameType ......
void Work::_checkLogicServerOnlySupportGameType()
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	
	m_mapLogicServerOnlySupportType.clear();

	LIniConfig	m_ini;
	m_ini.LoadFile("logic_manager_config.ini");

	Lstring strOnlySupportGameType = m_ini.GetString("OnlySupportGameType");
	if(strOnlySupportGameType.empty())	//don't config
	{
		return;
	}

	std::vector<std::string> vecIdAndType;
	L_ParseString(strOnlySupportGameType, vecIdAndType, "|");
	for(auto itIdType = vecIdAndType.begin(); itIdType != vecIdAndType.end(); itIdType++)
	{
		std::vector<std::string> vecData;
		L_ParseString(*itIdType, vecData, "_");
		if(vecData.size() != 2)
		{
			LLOG_ERROR("OnlySupportGameType is wrong. %s", itIdType->c_str());
			continue;
		}

		Lint iLogicId = atoi(vecData[0].c_str());
		Lint iGameType= atoi(vecData[1].c_str());

		auto itOnlySupport = m_mapLogicServerOnlySupportType.find(iLogicId);
		if(itOnlySupport == m_mapLogicServerOnlySupportType.end())
		{
			std::map<Lint, Lint> mapGameType;
			mapGameType[iGameType] = iGameType;
			m_mapLogicServerOnlySupportType[iLogicId] = mapGameType;
		}
		else
		{
			itOnlySupport->second[iGameType] = iGameType;
		}
	}
}

void Work::_checkRLogStart()
{
	LIniConfig	m_ini;
	m_ini.LoadFile("logic_manager_config.ini");

	gRLT.SetOpen( m_ini.GetInt("RemoteLogStart", 0) != 0 );
	gRLT.SetURL( m_ini.GetString("RemoteLogUrl", "") );
}

void Work::_checkMaxDeskCountLimitForLogicServer()
{
	LIniConfig m_ini;
	m_ini.LoadFile("logic_manager_config.ini");
	// Update desk count
	this->m_maxDeskCountLimitForEachLogic = m_ini.GetInt("DeskCountLimitForLogicServer", 100);
}


/////////////////////关于和center之间俱乐部接口
//void Work::HanderCenterClubInfo(LMsgCe2LClubInfo*msg)
//{
//	LLOG_DEBUG("Work::HanderCenterClubInfo");
//	if (msg==NULL)return;
//	
//	LLOG_DEBUG("LMsgCe2LClubInfo: m_id=[%d],m_clubId=[%d],m_name=[%s],m_coin=[%d],m_ownerId=[%d],m_userSum=[%d],m_clubUrl=[%s],m_status=[%d],m_type=[%//d],m_feeType=[%d]", msg->m_id,   \
//		msg->m_clubId,                    	 \
//		msg->m_name.c_str(),						 \
//		msg->m_coin,						   \
//		msg->m_ownerId,					  \
//		msg->m_userSum,					 \
//		msg->m_clubUrl.c_str(),						\
//		msg->m_status,						  \
//		msg->m_type,						   \
//		msg->m_feeType);
//
//	//gClubManager.updateClub(msg);
//}

//void Work::HanderAddClub(LMsgCe2LAddClub* msg)
//{
//	LLOG_DEBUG("Work::HanderAddClub");
//	if (msg==NULL)return;
//
//	LLOG_DEBUG("HanderAddClub: m_id=[%d],m_clubId=[%d],m_name=[%s],m_coin=[%d],m_ownerId=[%d],m_userSum=[%d],m_clubUrl=[%s],m_status=[%d],m_type=[%//d],m_feeType=[%d]", msg->m_id, \
//		msg->m_clubId, \
//		msg->m_name.c_str(), \
//		msg->m_coin, \
//		msg->m_ownerId, \
//		msg->m_userSum, \
//		msg->m_clubUrl.c_str(), \
//		msg->m_status, \
//		msg->m_type, \
//		msg->m_feeType);
//
//	gClubManager.addClub(msg);
//
//}

//void Work::HanderClubAddPlayType(LMsgCe2LMGClubAddPlayType *msg)
//{
//	LLOG_DEBUG("Work::HanderClubAddPlayType");
//	if (msg==NULL)return;
//
//	clubPlayType   aClubPlayType;
//	aClubPlayType.m_id = msg->m_id;
//	aClubPlayType.m_clubId = msg->m_clubId;
//	aClubPlayType.m_playTypes = msg->m_playTypes;
//	aClubPlayType.m_status = msg->m_status;
//	aClubPlayType.m_tableNum = msg->m_tableNum;
//
//
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_clubId);
//	if (safeClub.get() == NULL || !safeClub->isValid())
//	{
//		LLOG_ERROR("Fail to add club playType,clubId=[%d]  is null or invalid", msg->m_clubId);
//		return;
//	}
//
//	boost::shared_ptr<Club> club = safeClub->getResource();
//	club->addClubPlayType(aClubPlayType);
//}

//void Work::HanderClubAddUser(LMsgCe2LMGClubAddUser *msg)
//{
//	LLOG_DEBUG("Work::HanderClubAddUser");
//	if (msg==NULL)return;
//	//myClubUser  mClubUser;
//	//mClubUser.m_userId=msg->m_userId;
//	//mClubUser.m_status=msg->m_status;
//	//mClubUser.m_nike = msg->m_userNike;
//	//mClubUser.m_headUrl = msg->m_headurl;
//	//mClubUser.m_type = msg->m_type;
//	gClubManager.addClubUser(msg->m_userInfo);
//}

//void Work::HanderClubDelUser(LMsgCe2LMGClubDelUser *msg)
//{
//	LLOG_DEBUG("Work::HanderClubDelUser");
//	if (msg==NULL)return;
//	myClubUser  mClubUser;
//	mClubUser.m_userId=msg->m_userId;
//	mClubUser.m_status=msg->m_status;
//	mClubUser.m_type = msg->m_userType;
//	gClubManager.delClubUser(msg->m_clubId, mClubUser,msg->m_type);
//
//	if (msg->m_type == 2)
//	{
//		LLOG_ERROR("Work::HanderClubDelUser clubId[%d] type[%d],del userId[%d]",msg->m_clubId,msg->m_type,msg->m_userId);
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userId);
//		if (safeUser && safeUser->isValid())
//		{
//			LMsgS2CNotifyUserLeaveClub send;
//			send.m_clubId = msg->m_clubId;
//			send.m_clubName = msg->m_clubName;
//			safeUser->getResource()->Send(send.GetSendBuff());
//		}
//		
//
//	}
//
//}

void Work::HanderClubHidePlayType(LMsgCe2LMGClubHidePlayType *msg)
{
	LLOG_DEBUG("Work::HanderClubHidePlayType");
	if (msg==NULL)return;
	gClubManager.hideClubPlayType(msg->m_clubId,msg->m_id,msg->m_playTypes,msg->m_status);
}

void Work::HanderClubModifyName(LMsgCe2LMGClubModifyClubName *msg)
{
	LLOG_DEBUG("Work::HanderClubModifyName");
	if (msg == NULL)return;
	if (msg->m_type == 1)
	{
		gClubManager.alterPlayTypeName(msg->m_clubId, msg->m_playTypeId, msg->m_clubName);
	}
	else if(msg->m_type==0)
	{
		gClubManager.alterClubName(msg->m_clubId, msg->m_clubName);
	}
	else
	{
		LLOG_ERROR("Work::HanderClubModifyName  ERROR");
	}
}

void Work::HanderClubModifyFeeType(LMsgCe2LMGClubModifyClubFee *msg)
{
	LLOG_DEBUG("Work::HanderClubModifyFeeType");
	if (msg == NULL)return;
	gClubManager.modifyClubFeeType(msg->m_clubId, msg->m_clubFeeType);
}

void Work::HanderLogicSynchroData(LMsgL2LMGSynchroData*msg)
{
	if (msg==NULL)return;
	LLOG_ERROR("Work::HanderLogicSynchroData LogicId=[%d],userSize=[%d]", msg->m_logicID, msg->m_info.size());
	//处理用户
	for (int i=0; i<msg->m_info.size(); i++)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser=gUserManager.getUserbyUserId(msg->m_info[i].m_userId);
		if (safeUser.get()==NULL||!safeUser->isValid())
		{
			LLOG_ERROR("Work::HanderLogicSynchroData  add user[%d]", msg->m_info[i].m_userId);
			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo=gUserManager.getUserBaseInfo(msg->m_info[i].m_userId);
			if (safeBaseInfo && safeBaseInfo->isValid())
			{
				LUser tempLUser;
				tempLUser.m_id=safeBaseInfo->getResource()->m_id;
				tempLUser.m_unioid=safeBaseInfo->getResource()->m_unionId;
				tempLUser.m_nike=safeBaseInfo->getResource()->m_nike;
				tempLUser.m_headImageUrl=safeBaseInfo->getResource()->m_headImageUrl;
				tempLUser.m_sex=safeBaseInfo->getResource()->m_sex;

				boost::shared_ptr<User> user;
				user.reset(new User(tempLUser, msg->m_info[i].m_gateId));

				user->m_logicID=msg->m_logicID;
				user->m_userState=msg->m_info[i].m_userState;

				UserManager::Instance().addUser(user);
			}
		}
		else
		{
			safeUser->getResource()->m_logicID=msg->m_logicID;
			safeUser->getResource()->m_userState=msg->m_info[i].m_userState;
		}
	}
	//处理桌子
	gDeskManager.SynchroLogicServerDeskInfo(msg->m_deskInfo, msg->m_logicID);
}

void Work::HanderGateSynchroData(LMsgG2LMGSynchroData*msg)
{
	LLOG_DEBUG("Work::HanderGateSynchroData");
	if (msg==NULL)return;
	LLOG_ERROR("Work::HanderLogicSynchroData gateIdId=[%d],userSize=[%d]", msg->m_gateID, msg->m_info.size());

	//处理用户
	for (int i=0; i<msg->m_info.size(); i++)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser=gUserManager.getUserbyUserUUID(msg->m_info[i].m_strUUID);
		if (safeUser.get()==NULL||!safeUser->isValid())
		{
			LLOG_ERROR("Work::HanderLogicSynchroData  add user[%s]", msg->m_info[i].m_strUUID.c_str());
			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo=gUserManager.getUserBaseInfoByUUID(msg->m_info[i].m_strUUID);
			if (safeBaseInfo && safeBaseInfo->isValid())
			{
				LUser tempLUser;
				tempLUser.m_id=safeBaseInfo->getResource()->m_id;
				tempLUser.m_unioid=safeBaseInfo->getResource()->m_unionId;
				tempLUser.m_nike=safeBaseInfo->getResource()->m_nike;
				tempLUser.m_headImageUrl=safeBaseInfo->getResource()->m_headImageUrl;
				tempLUser.m_sex=safeBaseInfo->getResource()->m_sex;

				boost::shared_ptr<User> user;
				user.reset(new User(tempLUser, msg->m_gateID));
				user->m_userState=msg->m_info[i].m_userState;

				UserManager::Instance().addUser(user);
			}
		}
	}
}

void Work::SendRequestInfoToGateOrLogic(Lint GOrL, Lint serverId, Lint needData)
{
	LLOG_ERROR("SendRequestInfoToGateOrLogic GOrL=[%d],serverId=[%d],needData=[%d] ", GOrL, serverId, needData);
	LMsgLMG2LGRequestSynchroData send;
	send.m_ID=gConfig.GetServerID();
	send.m_needLoadUserInfo=needData;

	switch (GOrL)
	{
	case 1:
		SendMessageToGate(serverId, send);
		break;
	case 2:
		SendMessageToLogic(serverId, send);
		break;
	default:
		LLOG_ERROR("Work::SendRequestInfoToGateOrLogic  GorL =[%d]not define !", GOrL);
		break;
	}
}

void Work::SendRequestInfoToAllGateOrLogic(Lint GOrL)
{
	LLOG_DEBUG("SendRequestInfoToAllGateOrLogic GOrL=[%d] ", GOrL);
	LMsgLMG2LGRequestSynchroData send;
	send.m_ID=gConfig.GetServerID();
	send.m_needLoadUserInfo=1;

	switch (GOrL)
	{
	case 1:
		SendMessageToAllGate(send);
		break;
	case 2:
		SendMessageToAllLogic( send);
		break;
	default:
		LLOG_ERROR("Work::SendRequestInfoToAllGateOrLogic  GorL =[%d]not define !", GOrL);
		break;
	}
}

void Work::HanderDismissRoom(LMsgCe2LMGDismissDesk *msg)
{
	//if (msg == NULL)return;
	//LLOG_ERROR("Work::HanderDismissRoom  type=[%d],clubid=[%d],showdeskId=[%d],deskid=[%d]",msg->m_type,msg->m_clubId,msg->m_showDeskId,msg-/>m_6DeskId);
	//
	//Lint deskId = 0;
	//if (msg->m_type == 1)
	//{
	//	deskId = gClubManager.getRealDeskIdByShowDeskId(msg->m_clubId, msg->m_showDeskId);
	//}
	//else
	//{
	//	deskId = msg->m_6DeskId;
	//}
	//
	//if (deskId <= 0)
	//{
	//	LLOG_ERROR("HanderDismissRoom  error deskId=[%d]<=0",deskId);
	//	return;
	//}
	//
	//LMsgLMG2LAPIDissmissRoom send;
	//send.m_deskId = deskId;
	//
	//Lint iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(send.m_deskId);
	//if (!gWork.isLogicServerExist(iLogicServerId))
	//{
	//	iLogicServerId = INVALID_LOGICSERVERID;
	//}
	//if (iLogicServerId != INVALID_LOGICSERVERID) {
	//	LLOG_ERROR("User::HanderDismissRoom [%d]  on serverId[%d]", send.m_deskId, iLogicServerId);
	//	gWork.SendMessageToLogic(iLogicServerId, send);
	//}
	//else
	//{
	//	LLOG_ERROR("User::HanderDismissRoom [%d] error!!! logic server not find INVALID_LOGICSERVERID", send.m_deskId);
	//}
	return;
}


void Work::HanderDelUserFromMap(LMsgCe2LMGDelUserFromManager *msg)
{
	if (msg == NULL)return;
	LLOG_ERROR("Work::HanderDelUserFromMap  userId=[%d]", msg->m_userId);
	UserManager::Instance().delUser(msg->m_userId);
	
	return;
}

void Work::HanderCenterSpecActive(LMsgLMG2LSpecActive* msg)
{
	if (msg == NULL)return;
	LLOG_DEBUG("HanderCenterSpecActive");
	gSpecActive.addSpecActive(msg);
}

void Work::HanderControlMsgToLogic(LMsgCe2LMGControlMsg *msg)
{
	if (msg == NULL)return;
	LMsgCe2LMGControlMsg send;
	send.m_argv = msg->m_argv;
	send.m_type = msg->m_type;
	LLOG_ERROR("HanderControlMsgToLogic type[%d],argv[%d]",msg->m_type,msg->m_argv);
	SendMessageToAllLogic(send);
}

//void Work::HanderUserApplyClub(LMsgCe2LMGClubAddUser *msg)
//{
//	
//	if (msg == NULL)return;
//
//	clubFeeType feeType;
//	gClubManager.getClubFeeType(msg->m_userInfo.m_clubId, feeType);
//
//	LLOG_DEBUG("Work::HanderUserApplyClub  clubId[%d] clubOwner[%d]", msg->m_userInfo.m_clubId, feeType.m_clubCreatorId);
//	LMsgS2CNotifyUserJoinClub send;
//	send.m_clubId = msg->m_userInfo.m_clubId;
//
//	if (feeType.m_clubCreatorId != 0)
//	{
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(feeType.m_clubCreatorId);
//		if (safeUser && safeUser->isValid())
//		{
//			LLOG_DEBUG("Work::HanderUserApplyClub  clubId[%d] clubOwner[%d]", msg->m_userInfo.m_clubId, feeType.m_clubCreatorId);
//			safeUser->getResource()->Send(send.GetSendBuff());
//		}
//	}
//
//	if (!feeType.m_adminUser.empty())
//	{
//		for (auto ItUser = feeType.m_adminUser.begin(); ItUser != feeType.m_adminUser.end(); ItUser++)
//		{
//			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(*ItUser);
//			if (safeUser && safeUser->isValid())
//			{
//				LLOG_DEBUG("Work::HanderUserApplyClub  clubId[%d] clubAdmin[%d]", msg->m_userInfo.m_clubId, *ItUser);
//				safeUser->getResource()->Send(send.GetSendBuff());
//			}
//		}
//	}
//
//}
