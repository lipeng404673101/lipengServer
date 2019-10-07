#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "DbServer.h"
#include "InsideNet.h"
#include "UserManager.h"
#include "HttpRequest.h"
#include "DbServerManager.h"
#include "UserMessageMsg.h"
#include "NewUserVerifyManager.h"
#include "mhmsghead.h"
#include "mjConfig.h"
#include "clubManager.h"
#include "SpecActive.h"

#include <openssl/md5.h>


#define ActiveFreeSetKey	"ActiveFreeSet"
#define ActivePXSetKey		"ActivePXSet"
#define ActiveOUGCSetKey	"ActiveOUGCSet"
#define ActiveExchangeSetKey "ActiveExchangeSet"



//初始化
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看
	LLog::Instance().SetFileName("CenterServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	//设置log级别
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	//网络消息初始化
	//LMsgFactory::Instance().Init();
	MH_InitFactoryMessage_ForMapMessage();
	MHInitFactoryMessage();

	//初始化Redis client
	if (!_initRedisAndActive())
	{
		LLOG_ERROR("Fail to init Redis ip and port");
		return false;
	}

	LLOG_ERROR("Start to test master and backup database");

	//测试数据库
	LDBSession dbSession;
	if(!dbSession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		LLOG_ERROR("Connect master database error : %s", gConfig.GetDbHost().c_str());
		return false;
	}	

	//测试备用数据库
	LDBSession dbSessionBK;
	if (!dbSessionBK.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort()))
	{
		LLOG_ERROR("Connect backup database error : %s", gConfig.GetBKDbHost().c_str());
		return false;
	}

	LLOG_ERROR("Start to config date source");

	// 配置数据源
	m_dbServer.SetDatabaseSource(true);
	m_dbServerBK.SetDatabaseSource(false);
	// 加载Redis数据库配置
	{
		Lint  db_mode = 0;
		m_bDBMode = true;
		bool ret = m_RedisClient.readDataByString("mj-db-src", db_mode);
		if (ret)
		{
			if (0 == db_mode)
			{
				m_bDBMode = true;          //主库
			}
			else if (1 == db_mode)
			{
				m_bDBMode = false;    //  备用数据源
			}
			else
			{
				LLOG_ERROR("***Error unknow data source value   mj-db-src = %d", db_mode);
			}
		}
		else
		{
			LLOG_ERROR("***Failed to read redis value mj-db-src");
		}
		if (m_bDBMode)
			LLOG_ERROR("***Center use master database source host %s", gConfig.GetDbHost().c_str());
		else
			LLOG_ERROR("***Center use backup database source %s", gConfig.GetBKDbHost().c_str());
	}

	if (!gDbServerManager.Init())
	{
		LLOG_ERROR("gDbServerManager.Init() error");
		return false;
	}

	if (!gDbNewUserManager.Init())
	{
		LLOG_ERROR("gDbNewUserManager.Init() error");
		return false;
	}

	if(!gUserManager.Init())
	{
		LLOG_ERROR("gUserManager.Init error");
		return false;
	}


	//内部端口初始化
	if(!gInsideNet.Init())
	{
		LLOG_ERROR("Fail to init inside ip and port");
		return false;
	}

	//gm访问端口初始化
	if (!gGmNet.Init())
	{
		LLOG_ERROR("Fail to init gm ip and port");
		return false;
	}

	_initTickTime();	

	m_gateIpSelect=0;

	// 加载gate ip config
	{
		Lstring str_gate_ip;
		boost::mutex::scoped_lock l(m_mutexGateInfo);
		m_RedisClient.readDataByString("ali", str_gate_ip);
		m_mapGaoFangIp["ali"] = "";
		m_mapGaoFangIp["yunman"] = "";
		if (str_gate_ip.empty())
		{
			LLOG_ERROR("***Failed to read ali gaofang ip from redis");		
		}
		else
		{
			m_mapGaoFangIp["ali"] = str_gate_ip;
			LLOG_ERROR("***Ali Gaofang ip: %s", str_gate_ip.c_str());
		}
		m_RedisClient.readDataByString("yunman", str_gate_ip);
		if (str_gate_ip.empty())
		{
			LLOG_ERROR("****Failed to read YUNMAN gaofang ip from redis");			
		}
		else
		{
			m_mapGaoFangIp["yunman"] = str_gate_ip;
			LLOG_ERROR("***Yunman Gaofang ip is: %s", str_gate_ip.c_str());
		}

	}	

	gMjConfig.Init();

	gClubManager.Init();

	gSpecActive.Init();

	LLOG_ERROR("Exit Work.Init ....");
	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	m_memeoryRecycle.Start();

	//玩家消息线程开启
	gUserMessageMsg.Start();

	//主线程开启
	LRunnable::Start();

	//数据库线程开启
	m_dbServer.Start();

	gDbServerManager.Start();

	gDbNewUserManager.Start();

	//内部网络开启
	gInsideNet.Start();

	//gm网络开启
	gGmNet.Start();
}

//等待
void Work::Join()
{
	LLOG_DEBUG("----------------------Enter Work::Join");
	//等待玩家消息线程终止
	gUserMessageMsg.Join();

	gGmNet.Join();

	//等待网络线程终止
	gInsideNet.Join();

	//等待数据库线程终止
	m_dbServer.Join();

	gDbServerManager.Join();

	gDbNewUserManager.Join();

	m_memeoryRecycle.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	gUserMessageMsg.Stop();

	gGmNet.Stop();

	m_dbServer.Stop();

	gDbServerManager.Stop();

	gDbNewUserManager.Stop();

	LRunnable::Stop();

	m_memeoryRecycle.Stop();
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

	switch(pMessage->m_msgId)
	{
	case MSG_FROM_LOGINSERVER:
		{
			LMsgFromLoginServer* pFromLoginServer = (LMsgFromLoginServer*)pMessage;
			if(pFromLoginServer)
			{
				gUserMessageMsg.handlerMessage(pFromLoginServer->m_strUUID, pFromLoginServer);
			}
			break;
		}
	case MSG_L_2_CE_MODIFY_USER_NEW:
		{
			LMsgL2CeModifyUserNew* pUserNewMsg = (LMsgL2CeModifyUserNew*)pMessage;
			if(pUserNewMsg)
			{
				gUserMessageMsg.handlerMessage(pUserNewMsg->m_strUUID, pUserNewMsg);
			}
			break;
		}

	case MSG_LMG_2_CE_UPDATE_COIN:
	{
		LMsgL2CeUpdateCoin* pUpdateUserCoin = (LMsgL2CeUpdateCoin*)pMessage;
		if (pUpdateUserCoin)
		{
			gUserMessageMsg.handlerMessage(pUpdateUserCoin->m_strUUID, pUpdateUserCoin);
		}
		break;
	}


	case MSG_LMG_2_CE_UPDATE_USER_GPS:
	{
		LMsgLMG2CEUpdateUserGPS* pUpdateUserGps = (LMsgLMG2CEUpdateUserGPS*)pMessage;
		if (pUpdateUserGps)
		{
			gUserMessageMsg.handlerMessage(pUpdateUserGps->m_strUUID, pUpdateUserGps);
		}
		break;
	}
	case MSG_LMG_2_CE_UPDATE_DATA:   //接收manager更新俱乐部桌子信息
	{
		LLOG_DEBUG("MSG_LMG_2_CE_UPDATE_DATA");
		HanderManagerUpdateData((LMsgLMG2CEUpdateData*)msg);
		
		break;
	}
	case MSG_LMG_2_CE_UPDATE_COIN_JOIN_DESK:
	{
		LMsgL2CeUpdateCoinJoinDesk* pUpdateUserCoinJoinDesk = (LMsgL2CeUpdateCoinJoinDesk*)pMessage;
		if (pUpdateUserCoinJoinDesk)
		{
			gUserMessageMsg.handlerMessage(pUpdateUserCoinJoinDesk->m_strUUID, pUpdateUserCoinJoinDesk);
		}
		break;
	}

	case MSG_L_2_CE_USER_LOGIN:
		{
			LMsgL2CeUserServerLogin* pUserLoginMsg = (LMsgL2CeUserServerLogin*)pMessage;
			if(pUserLoginMsg)
			{
				gUserMessageMsg.handlerMessage(pUserLoginMsg->m_openID, pUserLoginMsg);
			}
			break;
		}
	case MSG_L_2_CE_USER_LOGOUT:
		{
			LMsgL2CeUserServerLogout* pLogoutMsg = (LMsgL2CeUserServerLogout*)pMessage;
			if(pLogoutMsg)
			{
				gUserMessageMsg.handlerMessage(pLogoutMsg->m_openID, pLogoutMsg);
			}
			break;;
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
			LMsgL2LMGAddUserPlayCount* pPlayerCount = (LMsgL2LMGAddUserPlayCount*)pMessage;
			if(pPlayerCount)
			{
				gUserMessageMsg.handlerMessage(pPlayerCount->m_strUUID, pPlayerCount);
			}
			break;
		}
	case MH_MSG_L_2_LMG_NEW_AGENCY_ACTIVITY_UPDATE_PLAY_COUNT:  // 新代理满场活动
	{
		MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * pMsgAgencyActivity = (MHLMsgL2LMGNewAgencyActivityUpdatePlayCount*)pMessage;
		LLOG_DEBUG("*********Get MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * pMsgAgencyActivity = (MHLMsgL2LMGNewAgencyActivityUpdatePlayCount*)pMessage; m_strUUID=%s",
			pMsgAgencyActivity->m_strUUID.c_str());
		if (pMsgAgencyActivity)
		{
			//gUserMessageMsg.handlerMessage(pMsgAgencyActivity->m_strUUID, pMsgAgencyActivity);
			gDbNewUserManager.Push(pMsgAgencyActivity->m_strUUID, pMsgAgencyActivity);

		}
		break;
	}
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		{
			LMsgCN2LMGModifyUserCoins* pActiveInfoMsg = (LMsgCN2LMGModifyUserCoins*)pMessage;
			if(pActiveInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pActiveInfoMsg->m_strUUID, pActiveInfoMsg);
			}
			break;
		}
	case MSG_C_2_C_ADD_CRE:
		{
			LMsgC2C_ADD_CRE* pActiveInfoMsg = (LMsgC2C_ADD_CRE*)pMessage;
			if(pActiveInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pActiveInfoMsg->m_strUUID, pActiveInfoMsg);
			}
			break;
		}
	case MSG_LMG_2_L_EXCH_CARD:
		{
			LMsgL2LMGExchCard* pCoinsMsg = (LMsgL2LMGExchCard*)pMessage;
			if(pCoinsMsg)
			{
				gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
			}
			break;
		}
	case MSG_CE_2_LMG_SQL_INFO:
	{
		LMsgLMG2CeSqlInfo* pSqlInfo = (LMsgLMG2CeSqlInfo*)pMessage;
		if (pSqlInfo)
		{
			gUserMessageMsg.handlerMessage(pSqlInfo->m_userId, pSqlInfo);
		}
		break;
	}
	case MSG_CE_2_LMG_UPDATE_POINT_RECORD:
	{
		LMsgLMG2CEUpdatePointRecord* pInfo = (LMsgLMG2CEUpdatePointRecord*)pMessage;
		if (pInfo)
		{
			gUserMessageMsg.handlerMessage(pInfo->m_record.m_userId, pInfo);
		}
		break;
	}
	case MSG_CE_2_LMG_ADD_CLUB:
	{
		LMsgCe2LAddClub* pInfo = (LMsgCe2LAddClub*)pMessage;
		if (pInfo)
		{
			gUserMessageMsg.handlerMessage(pInfo->m_ownerId, pInfo);
		}
		break;
	}
	case MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE:
	{
		LMsgCe2LMGClubAddPlayType* pInfo = (LMsgCe2LMGClubAddPlayType*)pMessage;
		if (pInfo)
		{
			gUserMessageMsg.handlerMessage(pInfo->m_playInfo.m_id, pInfo);
		}
		break;
	}
	case MSG_LMG_2_CE_POINT_RECORD_LOG:
	{
		LMsgLMG2CERecordLog* pInfo = (LMsgLMG2CERecordLog*)pMessage;
		if (pInfo)
		{
			gUserMessageMsg.handlerMessage(pInfo->m_userId, pInfo);
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

	default:
		LRunnable::Push(pMessage);
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
				LLOG_ERROR("Work Logic: 10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

				iMsgCount = 0;
				iElapseTime = 0;
				
				msgTime = now;
			}
		}
	}
}

void Work::Tick(LTime& cur)
{
	// 检查数据库切换是否发生
	if (m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		int db_mode = 0;
		bool bChanged = false;
		if (m_RedisClient.readDataByString("mj-db-src", db_mode))
		{
			if (0 == db_mode && (m_bDBMode == false))
			{
				bChanged = true;
				m_bDBMode = true; // 主库
			}
			if ( (1 == db_mode) && m_bDBMode)
			{
				bChanged = true;
				m_bDBMode = false; // 备用库
			}
		}
		if (bChanged)
		{
			LLOG_ERROR("***Center  database source changed");
			if (m_bDBMode)
				LLOG_ERROR("***Center use master database source host %s", gConfig.GetDbHost().c_str());
			else
				LLOG_ERROR("***Center use backup database source %s", gConfig.GetBKDbHost().c_str());
			gMjConfig.Init();
		}
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		SaveCurrentOnline();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 300000)   // 5*60 * 1000 == 300000
	{
		m_600SceTick = cur.MSecs();
		if (gWork.GetCurTime().GetHour() == 24 && (gWork.GetCurTime().GetMin() >= 0 && gWork.GetCurTime().GetMin() < 5))
		{
			LLOG_DEBUG("do load config db");
			gMjConfig.LoadCommonConfigDataFromDB();		//5分钟内，必定执行一次，也只执行一次。
			gMjConfig.LoadDeskFeeDataFromDB();
			LMsgCE2LMGConfig Config;
			gMjConfig.CreatToLogicConfigMessage(Config);
			SendMsgToAllLogic(Config);
		}

	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_HTTP:
		HanderHttp((LMsgHttp*)msg);
		break;
	case MSG_L_2_CE_LOGIN:
		HanderLogicLogin((LMsgL2CeLogin*)msg);
		break;
	case MSG_L_2_CE_GATE_INFO:
		HanderLogicGateInfo((LMsgL2CeGateInfo*)msg);
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;
	case MSG_LM_2_CEN_ADD_CRE:
		HanderModifyCredit((LMsgLM2CEN_ADD_CRE*) msg);
		break;
	
	default:
		break;
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if(isLogicManagerExist(msg->m_sp))
	{
		HanderLogicLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{

}

void Work::HanderLogicLogin(LMsgL2CeLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//判断key是否正确
	if (msg->m_key != gConfig.GetCenterKey())
	{
		msg->m_sp->Stop();
		return;
	}

	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);

		auto itLogicM = m_mapLogicManagerSp.find(msg->m_ID);
		if(itLogicM != m_mapLogicManagerSp.end())
		{
			if(itLogicM->second)
			{
				itLogicM->second->Stop();
			}

			m_mapLogicManagerSp.erase(itLogicM);
		}

		m_mapLogicManagerSp[msg->m_ID] = msg->m_sp;
		m_mapLogicManagerName[msg->m_ID] = msg->m_ServerName;
	}
	
	//发送俱乐部信息
	gClubManager.sendClubDataToLogicManager(msg->m_ID);

	// 发送活动配置信息
	SendActiveSetToLM(msg->m_ID);

	//发送特殊活动信息
	gSpecActive.sendToManager(msg->m_ID);

	if(msg->m_needLoadUserInfo == 1)
	{
		gUserManager.LoadUserIdInfo(msg->m_ID);
	}

	//gClubManager.sendClubDataToLogicManager(msg->m_ID);

}

void Work::HanderLogicLogout(LMsgKick*msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("Work::HanderLogicLogout");
	
	int iServerId = 0;

	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
		{
			if(itLogicM->second == msg->m_sp)
			{
				iServerId = itLogicM->first;
				m_mapLogicManagerSp.erase(itLogicM);
				break;
			}
		}
	}

	if(iServerId > 0)
	{
		boost::mutex::scoped_lock ll(m_mutexGateInfo);
		m_mapGateInfo.erase(iServerId);
		m_mapAliGaoFangGateInfo.erase(iServerId);
		m_mapYunmanGaoFangGateInfo.erase(iServerId);
	}
}

void Work::HanderLogicGateInfo(LMsgL2CeGateInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderLogicGateInfo %d", msg->m_ID);
		return;
	}

	if(!isLogicManagerExist(msg->m_ID))
	{
		LLOG_ERROR("LogicManger's id isn't exist. %d", msg->m_ID);
		msg->m_sp->Stop();
		return;
	}

	boost::mutex::scoped_lock l(m_mutexGateInfo);
	m_mapGateInfo[msg->m_ID].clear();
	m_mapAliGaoFangGateInfo[msg->m_ID].clear();
	m_mapYunmanGaoFangGateInfo[msg->m_ID].clear();
	for (Lint i = 0; i < msg->m_count; ++i)
	{
		if (msg->m_gate[i].m_gaofang_flag == 2)
		{
			m_mapYunmanGaoFangGateInfo[msg->m_ID][msg->m_gate[i].m_id] = msg->m_gate[i];
			LLOG_DEBUG("***gate info: i:%d m_gaofang_flag:%d, ip:%s, port:%d", i, msg->m_gate[i].m_gaofang_flag, msg->m_gate[i].m_ip.c_str(), msg->m_gate[i].m_port);
		}
		else if (msg->m_gate[i].m_gaofang_flag == 1)
		{
			m_mapAliGaoFangGateInfo[msg->m_ID][msg->m_gate[i].m_id] = msg->m_gate[i];
			LLOG_DEBUG("***gate info: i:%d m_gaofang_flag:%d, ip:%s, port:%d", i, msg->m_gate[i].m_gaofang_flag, msg->m_gate[i].m_ip.c_str(), msg->m_gate[i].m_port);
		}
		else
		{
			LLOG_DEBUG("***gate info: i:%d m_gaofang_flag:%d, ip:%s, port:%d", i, msg->m_gate[i].m_gaofang_flag, msg->m_gate[i].m_ip.c_str(), msg->m_gate[i].m_port);
			m_mapGateInfo[msg->m_ID][msg->m_gate[i].m_id] = msg->m_gate[i];
		}
	}
}

void Work::HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg)
{
	LLOG_ERROR("Recieve heartbeat from %d", msg->m_fromWho);

	LMsgHeartBeatReplyMsg msgReply;
	msgReply.m_fromWho = HEARBEAT_WHO_CENTER;
	msgReply.m_iServerId = 0;

	msg->m_sp->Send(msgReply.GetSendBuff());
}

void Work::HanderModifyCredit( LMsgLM2CEN_ADD_CRE* msg )
{
	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		if(msg->m_user[i] > 0)
		{
			boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_user[i]);
			if(safeUser.get() == NULL || !safeUser->isValid())
			{
				LLOG_DEBUG("#ERROR SAVE CREDIT USER NOT EXIST = %d",msg->m_user[i]);
				continue;
			}

			boost::shared_ptr<DUser> user = safeUser->getUser();
			LMsgC2C_ADD_CRE* cmsg = new LMsgC2C_ADD_CRE();
			cmsg->m_strUUID = user->m_usert.m_unioid;
			cmsg->m_userId = user->m_usert.m_id;
			Push(cmsg);
		}
	}
}

Lint Work::getGateInfoSize(Lint severID)
{
	Lint size=0;
	boost::mutex::scoped_lock l(m_mutexGateInfo);
	auto itGateInfo=m_mapGateInfo.find(severID);
	if(itGateInfo!=m_mapGateInfo.end())
		 size=itGateInfo->second.size();
	return size;
}


GateInfo Work::GetGateUsefulInfo(Lint logicID)
{
	static Lint sindex = 0;
	GateInfo gateinfo;

	boost::mutex::scoped_lock l(m_mutexGateInfo);
	auto itGateInfo = m_mapGateInfo.find(logicID);
	if(itGateInfo != m_mapGateInfo.end())
	{
		Lint index = 0;
		auto it = itGateInfo->second.begin();
		for (; it != itGateInfo->second.end(); ++it)
		{
			if (index++ % itGateInfo->second.size() == sindex % itGateInfo->second.size())
			{
				gateinfo = it->second;
				break;
			}
		}

		sindex++;
	}

	return gateinfo;
}

GateInfo Work::GetAliGaoFangGateUsefulInfo(Lint logicID)
{
	static Lint sindex1 = 0;
	GateInfo gateinfo;

	boost::mutex::scoped_lock l(m_mutexGateInfo);
	auto itGateInfo =  m_mapAliGaoFangGateInfo.find(logicID);
	if (itGateInfo != m_mapAliGaoFangGateInfo.end())
	{
		Lint index = 0;
		auto it = itGateInfo->second.begin();
		for (; it != itGateInfo->second.end(); ++it)
		{
			if (index++ % itGateInfo->second.size() == sindex1 % itGateInfo->second.size())
			{
				gateinfo = it->second;
				break;
			}
		}

		sindex1++;
	}

	return gateinfo;
}


GateInfo Work::GetYunManGaoFangGateUsefulInfo(Lint logicID)
{
	static Lint sindex2 = 0;	
	GateInfo gateinfo;

	boost::mutex::scoped_lock l(m_mutexGateInfo);
	auto itGateInfo =  m_mapYunmanGaoFangGateInfo.find(logicID);
	if (itGateInfo != m_mapYunmanGaoFangGateInfo.end())
	{
		Lint index = 0;
		auto it = itGateInfo->second.begin();
		for (; it != itGateInfo->second.end(); ++it)
		{
			if (index++ % itGateInfo->second.size() == sindex2 % itGateInfo->second.size())
			{
				gateinfo = it->second;
				break;
			}
		}

		sindex2++;
	}

	return gateinfo;
}

Lstring Work::getServerNameByID(Lint serverID)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	auto itor = m_mapLogicManagerName.find(serverID);
	if (itor != m_mapLogicManagerName.end())
	{
		return itor->second;
	}

	return "未知麻将";
}

void Work::SendMsgToLogic(LMsg& msg, Lint logicID)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	auto itLogicM = m_mapLogicManagerSp.find(logicID);
	if(itLogicM != m_mapLogicManagerSp.end())
	{
		itLogicM->second->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("void Work::SendMsgToLogic ERROR: %d is invilid", logicID);
	}
}

void Work::SendMsgToLogic(LMsg& msg, Lint logicID, int iLimitCountInSendPool)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	auto itLogicM = m_mapLogicManagerSp.find(logicID);
	if (itLogicM != m_mapLogicManagerSp.end())
	{
		while(itLogicM->second->getSendPoolSize() > iLimitCountInSendPool)
		{
			boost::this_thread::sleep(boost::posix_time::millisec(30));
		}

		itLogicM->second->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("void Work::SendMsgToLogic ERROR: %d is invilid", logicID);
	}
}

void Work::SendMsgToAllLogic(LMsg& msg)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
	{
		itLogicM->second->Send(msg.GetSendBuff());
	}
}

static const char* ADMIN_PATH = "/cgi-bin/admin";

static void SendRet(Lstring errorInfo, LSocketPtr send)
{
	char strOK[512] = { 0 };

	std::string strSendBuf;
	strSendBuf.reserve(1024);
	strSendBuf += "HTTP/1.1 200 OK\r\n";

	sprintf_s(strOK, sizeof(strOK)-1, "Content-Length:%d\r\n", errorInfo.size());
	strSendBuf += strOK;
	strSendBuf += "Content-Type: text/html; charset=utf-8\r\n\r\n";

	memset(strOK,0,sizeof(strOK));
	sprintf_s(strOK, sizeof(strOK)-1, "%s", errorInfo.c_str());
	strSendBuf += strOK;

	LBuffPtr buff(new LBuff());
	buff->Write(strSendBuf.c_str(), strSendBuf.size());
	send->Send(buff);
}

/*
errorCode 类型
0	-	请求成功
1	-	请求url错误
2	-	消息类型不存
3	-	sign不正确
4	-	请求超时
5	-	请求方法错误
6	-	请求参数错误
*/

void Work::HanderHttp(LMsgHttp* msg)
{	

	if (msg == NULL)
	{
		return;
	}

	if (msg->m_head.size() > 4 && msg->m_head.substr(0, 4) == "GET ")
	{
		std::string::size_type pos = msg->m_head.find("\r\n");
		if (pos == std::string::npos || pos < 8)
		{
			LLOG_ERROR("Work::HanderHttp GET error");
			SendRet("{\"errorCode\":5,\"errorMsg\":\"req function error\"}",msg->m_sp);
			return;
		}

		// GET /cgi-bin/admin?msg=getUserInfo&openId=122&time=1466238689575&sign=bf4c9859c07c9687a29796ba723eff62 HTTP/1.1
		std::string totalUrl = msg->m_head.substr(4, pos-13);	// 去掉前面的Get+空格 去掉后面的空格+HTTP/1.1\r\n
		std::string::size_type pos2 = totalUrl.find('?', 0);
		if (pos2 == std::string::npos)
		{
			LLOG_ERROR("Work::HanderHttp GET param error");
			SendRet("{\"errorCode\":6,\"errorMsg\":\"req param error\"}", msg->m_sp);
			return;
		}

		std::string reqType = totalUrl.substr(0, pos2);
		std::string reqParam = totalUrl.substr(pos2 + 1, totalUrl.size() - pos2);
		if (reqType == ADMIN_PATH )
		{
			std::map<Lstring, Lstring> paraMaps;
			HttpRequest::Instance().DeodeUrlParams(reqParam, paraMaps, true);

			if (paraMaps["msg"]=="gatecmd")
			{
				if (paraMaps.count("code") < 1)
				{
					SendRet("{\"errorCode\":11,\"errorMsg\":\"Error no command code\"}", msg->m_sp);//命令错误
					return;
				}
				if (paraMaps["code"]=="0")
				{
					m_gateIpSelect=0;
					LLOG_DEBUG("Logwyz .............................m_gateIpSelect=[%d] ", m_gateIpSelect);
				}
				else if (paraMaps["code"]=="1")
				{
					m_gateIpSelect=1;
					LLOG_DEBUG("Logwyz .............................m_gateIpSelect=[%d] ", m_gateIpSelect);
				}
				else if (paraMaps["code"]=="2")
				{
					m_gateIpSelect=2;
					LLOG_DEBUG("Logwyz .............................m_gateIpSelect=[%d] ", m_gateIpSelect);
				}
				else
				{
					SendRet("{\"errorCode\":12,\"errorMsg\":\"Error unknow command code\"}", msg->m_sp);//命令错误
					return;
				}
				//LLOG_DEBUG("Logwyz .............................m_gateIpSelect=[%d] ", m_gateIpSelect);
				SendRet("{\"errorCode\":0,\"errorMsg\":\"Processed gate cmd successfully\"}", msg->m_sp);//签名错误
				return;
			}
			// GET http://127.0.0.1/cgi-bin/admin?msg=gatecfg&flag=yunman&ip=127.0.0.1
		    if (paraMaps["msg"] == "gatecfg")
			{
				this->LoadGaoFangIpConfigByHttpCmd(paraMaps, msg->m_sp);
				return;
			}
			// GET http://127.0.0.1/cgi-bin/admin?msg=dbcfg&db=1
			if (paraMaps["msg"] == "dbcfg")
			{
				this->LoadDatabaseConfigByHttpCmd(paraMaps, msg->m_sp);			
				return;
			}

			// GET http://127.0.0.1/cgi-bin/admin?msg=adjustuser&uid1=1&uid2=2         uid2的用户替换 uid1
			if (paraMaps["msg"] == "adjustuser")
			{
				this->AdjustUserByHttpCmd(paraMaps, msg->m_sp);
				return;
			}
			//else
			//{
			//	SendRet("{\"errorCode\":1,\"errorMsg\":\"Error - Unkown commad\"}", msg->m_sp);//签名错误
			//	return;
			//}
			//
			//if (!HanderCheckMd5(paraMaps))
			//{
			//	SendRet("{\"errorCode\":3,\"errorMsg\":\"sign error\"}", msg->m_sp);//签名错误
			//	return;
			//}
			
			//lmiao:暂时屏蔽掉，不判断时间，由于服务器被攻击，有可能长时间收不到
			// 			Lint time = atoi(paraMaps["time"].c_str());
			// 			if (gWork.GetCurTime().Secs() - time > 5)
			// 			{
			// 				SendRet("{\"errorCode\":4,\"errorMsg\":\"time out\"}", msg->m_sp);//超时
			// 				return;
			// 			}
			if (paraMaps["msg"] == "charge")
				HanderGMCharge(paraMaps,msg->m_sp);
			else if(paraMaps["msg"] == "exchange")    //换房卡了
				HanderSetExchange(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "coins")
				HanderGMCoins(paraMaps,msg->m_sp);
			else if (paraMaps["msg"] == "horse")
				HanderGMHorse(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "buy")
				HanderGMBuyInfo(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "online")
				HanderGMOnlineCount(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "close")
				HanderGMCloseServer(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "Free")
				HanderSetFreeSet(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "PXActive")
				HanderSetPXActive(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "OldUGiveCardActive")
				HanderSetOUGCActive(paraMaps, msg->m_sp);	// 老玩家送房卡活动
			else if(paraMaps["msg"] == "ActivityDraw")
				HanderActivityDrawSet(paraMaps,msg->m_sp);
			else if(paraMaps["msg"] == "ActivityShare")
				HanderActivityShareSet(paraMaps,msg->m_sp);
			else if (paraMaps["msg"] == "addClub")
				HanderClubAddClub(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "addClubPlayType")
				HanderClubAddPlayType(paraMaps, msg->m_sp);
			//会长或管理员同意申请玩家加入俱乐部
			else if (paraMaps["msg"] == "addClubUser")
				HanderClubAddUser(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "delClubUser")
				HanderClubDelUser(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "hideClubPlayType")
				HanderClubHidePlayType(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "alterClubName")
				HanderClubAlterClubName(paraMaps, msg->m_sp);
             else if (paraMaps["msg"] == "alterFeeType")
				HanderClubAlterClubFee(paraMaps, msg->m_sp);
			 else if (paraMaps["msg"] == "dismissDesk")
				 HanderDismissRoom(paraMaps, msg->m_sp);
			 else if (paraMaps["msg"] == "dismissClubDesk")
				 HanderDismissClubRoom(paraMaps, msg->m_sp);
			 else if (paraMaps["msg"] == "delUserFromManager")
				 HanderDelUserFromManager(paraMaps, msg->m_sp);
			 else if (paraMaps["msg"] == "frushSpecActive")
				 HanderFrushSpecActive(paraMaps, msg->m_sp);
			//玩家申请加入俱乐部
			 else if (paraMaps["msg"] == "applyClub")
				 HanderClubUserApplyClub(paraMaps, msg->m_sp);
			else
				SendRet("{\"errorCode\":2,\"errorMsg\":\"msg type error\"}", msg->m_sp);//消息类型错误
		}
		else
		{
			SendRet("{\"errorCode\":1,\"errorMsg\":\"get url error\"}", msg->m_sp);//请求链接错误
		}
	}
}

bool Work::HanderCheckMd5(std::map<Lstring, Lstring>& param)
{
	std::vector<std::string> keys;
	for (std::map<std::string, std::string>::const_iterator it = param.begin(); it != param.end(); ++it)
	{
		const std::string& strKey = it->first;
		if (strKey.compare("sign") != 0)
			keys.push_back(strKey);
	}
	std::sort(keys.begin(), keys.end());
	std::string reqsort;
	for (size_t i = 0; i < keys.size(); ++i)
	{
		reqsort += keys[i] + param[keys[i]];
	}
	reqsort += "1234567890987654321";


	std::string sign = HttpRequest::Instance().Md5Hex(reqsort.c_str(), reqsort.length());
	LLOG_DEBUG("Work::HanderCheckMd5,source:%s,sign:%s", reqsort.c_str(), sign.c_str());

	//加密参数不对
	if (sign != param["sign"])
	{
		LLOG_ERROR("Work::HanderCheckMd5 sign error %s:%s", param["sign"].c_str(), sign.c_str());
		return false;
	}

	return true;
}

void Work::HanderGMCharge(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin openId=openId cardType=1 cardNum=1 operType=1 sign=sign time=time

	LLOG_ERROR("Work::HanderGMCharge %s:%s", param["openId"].c_str(), param["admin"].c_str());

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(param["openId"]);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	Lint	errorCode = 0;
	Lint	cardType = atoi(param["cardType"].c_str());
	Lint	cardNum = atoi(param["cardNum"].c_str());
	Lint	operType = atoi(param["operType"].c_str());

	if (user->m_logicID)//这里在logic里面有备份
	{
		LMsgCe2LGMCharge gmcharge;
		gmcharge.m_cardType = cardType;
		gmcharge.m_cardCount = cardNum;
		gmcharge.m_oper = operType;
		gmcharge.m_strUUID= user->m_usert.m_unioid;
		gmcharge.m_userid = user->m_usert.m_id;
		gmcharge.m_admin = param["admin"];
		SendMsgToLogic(gmcharge, user->m_logicID);
	}

	user->AddCardCount( cardType, cardNum, operType, param["admin"] );

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMCoins(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderGMCoins %s:%s", param["openId"].c_str());

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(param["openId"]);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	Lint	errorCode = 0;
	Lint	coins = atoi(param["coins"].c_str());

	if ( coins > 0 )
	{
		user->AddCoinsCount( coins, 0 );
	}
	else
	{
		user->DelCoinsCount( coins, 0 );
	}
	
	if (user->m_logicID)//这里在logic里面有备份
	{
		LMsgCe2LGMCoins gmcharge;
		gmcharge.m_strUUID= user->m_usert.m_unioid;
		gmcharge.m_userid = user->m_usert.m_id;
		gmcharge.m_coins = coins;
		gmcharge.m_totalcoins = user->m_usert.m_coins;
		SendMsgToLogic(gmcharge, user->m_logicID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMHorse(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin horse=horse sign=sign time=time serverid=10
	LMsgCe2LGMHorse horse;
	horse.m_str = param["horse"];
	if (param["serverid"].empty())
	{
		SendMsgToAllLogic(horse);
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(horse, nServerID);
	}
	LLOG_ERROR("Work::HanderGMHorse sets horse %s", param["horse"].c_str());
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMBuyInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin buy=buy sign=sign time=time serverid=10
	LMsgCe2LGMBuyInfo buy;
	buy.m_str = param["buy"];
	if (param["serverid"].empty())
	{
		SendMsgToAllLogic(buy);
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(buy, nServerID);
	}
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMOnlineCount(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin sign=sign time=time

	boost::mutex::scoped_lock l(m_mutexGateInfo);

	std::stringstream ss;
	ss << "{\"errorCode\":0,\"errorMsg\":\"success\",\"data\":[";

	Lint nCount = 0;
	auto it = m_mapGateInfo.begin();
	for (; it != m_mapGateInfo.end(); it ++)
	{
		auto itor = it->second.begin();
		for (; itor != it->second.end(); itor ++)
		{
			nCount += itor->second.m_userCount;
		}
	}

	ss << "{\"count\":" << nCount << "},";

	Lstring tmp = ss.str().substr(0, ss.str().size() - 1);
	tmp += "]}";

	SendRet(tmp, sp);
}

void Work::HanderGMCloseServer(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_DEBUG("Work::HanderGMCloseServer %s", param["cmd"].c_str());
	if (param["cmd"] == "close_server_ty_ll")
	{
		SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
		Stop();

	}
	else
	{
		SendRet("{\"errorCode\":10,\"errorMsg\":\"close cmd error\"}", sp);//成功
	}
}

void Work::HanderSetFreeSet(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetGameFree is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"GameFree centent fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetFreeSet %s", param["content"].c_str());

	m_strActiveFreeSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActiveFreeSetKey, m_strActiveFreeSet))
	{
		LLOG_ERROR("Work::HanderSetFreeSet sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
		{
			LMsgCE2LSetGameFree free;
			free.m_ServerID = itLogicM->first;
			free.m_strFreeSet = m_strActiveFreeSet;
			itLogicM->second->Send(free.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetGameFree free;
		free.m_ServerID = nServerID;
		free.m_strFreeSet = m_strActiveFreeSet;
		SendMsgToLogic(free, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderSetPXActive(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetPXActive is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"PXActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetPXActive %s", param["content"].c_str());

	m_strActivePXSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActivePXSetKey, m_strActivePXSet))
	{
		LLOG_ERROR("Work::HanderSetPXActive sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
		{
			LMsgCE2LSetPXActive active;
			active.m_ServerID = itLogicM->first;
			active.m_strActiveSet = m_strActivePXSet;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetPXActive active;
		active.m_ServerID = nServerID;
		active.m_strActiveSet = m_strActivePXSet;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderSetOUGCActive(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetOUGCActive is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"HanderSetOUGCActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetOUGCActive %s", param["content"].c_str());

	m_strActiveOUGCSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActiveOUGCSetKey, m_strActiveOUGCSet))
	{
		LLOG_ERROR("Work::HanderSetOUGCActive sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
		{
			LMsgCE2LSetOUGCActive active;
			active.m_ServerID = itLogicM->first;
			active.m_strActiveSet = m_strActiveOUGCSet;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetOUGCActive active;
		active.m_ServerID = nServerID;
		active.m_strActiveSet = m_strActiveOUGCSet;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderSetExchange( std::map<Lstring,Lstring>& param,LSocketPtr sp )
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetExchange is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetExchange %s", param["content"].c_str());

	m_strActiveExchangeSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActiveExchangeSetKey, m_strActiveExchangeSet))
	{
		LLOG_ERROR("Work::HanderSetExchange sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
		{
			LMsgCE2LSetExchActive active;
			active.m_ServerID = itLogicM->first;
			active.m_strActiveSet = m_strActiveExchangeSet;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetExchActive active;
		active.m_ServerID = nServerID;
		active.m_strActiveSet = m_strActiveExchangeSet;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderActivityDrawSet(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderActivityShareSet is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderActivityShareSet %s", param["content"].c_str());

	Lstring strSet = param["content"];

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
		{
			LMsgCE2LSetActivity active;
			active.m_ServerID = itLogicM->first;
			active.m_strActivity = strSet;
			active.m_activityId = ActivityID_Draw;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetActivity active;
		active.m_ServerID = nServerID;
		active.m_strActivity = strSet;
		active.m_activityId = ActivityID_Draw;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderActivityShareSet( std::map<Lstring,Lstring>& param,LSocketPtr sp )   //分享送房卡
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderActivityShareSet is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderActivityShareSet %s", param["content"].c_str());

	Lstring strShareSet = param["content"];

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
		{
			LMsgCE2LSetActivity active;
			active.m_ServerID = itLogicM->first;
			active.m_strActivity = strShareSet;
			active.m_activityId = ActivityID_Share;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetActivity active;
		active.m_ServerID = nServerID;
		active.m_strActivity = strShareSet;
		active.m_activityId = ActivityID_Share;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderClubAddClub(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{

	LLOG_ERROR("Work::HanderClubAddClub ClubID = %s", param["id"].c_str());

	Lstring strId = param["id"];
	Lint id = atoi(strId.c_str());

	gClubManager.addClub(id);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderClubAddPlayType(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{

	LLOG_ERROR("Work::HanderClubAddPlayType PlayTypeID = %s", param["id"].c_str());

	Lstring strId = param["id"];
	Lint id = atoi(strId.c_str());

	gClubManager.addClubPlayType(id);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

//会长或管理员同意申请玩家加入俱乐部
void  Work::HanderClubAddUser(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{

	LLOG_ERROR("Work::HanderClubAddUser UserID = %s", param["id"].c_str());

	Lstring strId = param["id"];
	Lint id = atoi(strId.c_str());

	gClubManager.addClubUser(id);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderClubDelUser(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{

	LLOG_ERROR("Work::HanderClubDelUser UserID = %s", param["id"].c_str());

	Lstring strId = param["id"];
	Lint id = atoi(strId.c_str());

	Lstring strClubId = param["clubId"];
	Lint clubId = atoi(strClubId.c_str());

	Lstring  strType = param["type"];
	Lint type = atoi(strType.c_str());

	Lstring strUserId = param["userId"];
	Lint userId = atoi(strUserId.c_str());

	gClubManager.delClubUser(id,clubId,userId, type);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderClubHidePlayType(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{

	LLOG_ERROR("Work::HanderClubHidePlayType PlayTypeID = %s", param["id"].c_str());

	Lstring strId = param["id"];
	Lint id = atoi(strId.c_str());

	gClubManager.hideClubPlayType(id);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderClubAlterClubName(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderClubAlterClubName ClubID = %s", param["id"].c_str());

	Lstring strId = param["id"];
	Lint id = atoi(strId.c_str());

	gClubManager.alterClubName(id);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderClubAlterClubFee(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderClubAlterClubFee ClubID = %s", param["id"].c_str());

	Lstring strId = param["id"];
	Lint id = atoi(strId.c_str());

	gClubManager.modifyClubFeeType(id);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderDismissRoom(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderDismissRoom deskId = %s", param["deskId"].c_str());

	Lstring strId = param["deskId"];
	Lint deskid = atoi(strId.c_str());

	LMsgCe2LMGDismissDesk msgSend;
	msgSend.m_6DeskId=deskid;
	Lint serverID = 16210;
	LLOG_ERROR("HanderDismissRoom, deskId=[%d]", msgSend.m_6DeskId);
	if (msgSend.m_6DeskId < 100000)
	{
		LLOG_ERROR("HanderDismissRoom deskId[%d] error.");
		return;
	}
	gWork.SendMsgToLogic(msgSend, serverID);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
	return;
}

void  Work::HanderDismissClubRoom(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderDismissClubRoom ClubID = %s  showRoomId=[%s]", param["clubId"].c_str(), param["deskId"].c_str());

	Lstring strId = param["clubId"];
	Lstring strId1 = param["deskId"];

	Lint clubId = atoi(strId.c_str());
	Lint deskId = atoi(strId1.c_str());

	LMsgCe2LMGDismissDesk msgSend;
	msgSend.m_type = 1;
	msgSend.m_clubId = clubId;
	msgSend.m_showDeskId = deskId;

	Lint serverID = 16210;
	LLOG_ERROR("HanderDismissClubRoom, clubId=[%d],showDeskId=[%d]", msgSend.m_clubId, msgSend.m_showDeskId);
	if (msgSend.m_clubId <= 0 || msgSend.m_showDeskId < 100)
	{
		LLOG_ERROR("HanderDismissClubRoom, clubId=[%d],showDeskId=[%d]  ERROR", msgSend.m_clubId, msgSend.m_showDeskId);
		return;
	}
	gWork.SendMsgToLogic(msgSend, serverID);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}


void Work::SaveCurrentOnline()
{
	boost::mutex::scoped_lock l(m_mutexGateInfo);

	// 查找所有的注册服务器
	std::list <int> manager_id_list;
	for (auto it = m_mapGateInfo.begin(); it != m_mapGateInfo.end(); it++)
	{
		manager_id_list.push_back(it->first);
	}
	for (auto it = m_mapAliGaoFangGateInfo.begin(); it != m_mapAliGaoFangGateInfo.end(); it++)
	{
		auto p = std::find(manager_id_list.begin(), manager_id_list.end(), it->first);
		if (p == manager_id_list.end())  manager_id_list.push_back(it->first);
	}
	for (auto it = m_mapYunmanGaoFangGateInfo.begin(); it != m_mapYunmanGaoFangGateInfo.end(); it++)
	{
		auto p = std::find(manager_id_list.begin(), manager_id_list.end(), it->first);
		if (p == manager_id_list.end())  manager_id_list.push_back(it->first);
	}

	//统计人数
	for (auto it = manager_id_list.begin(); it != manager_id_list.end(); it++)
	{
		Lint nCount = 0;
		Lint nServerId = *it;
		if (m_mapGateInfo.count(*it) > 0)
		{
			for (auto itGate = m_mapGateInfo[*it].begin(); itGate != m_mapGateInfo[*it].end(); itGate++)
			{
				nCount += itGate->second.m_userCount;
			}
		}

		if (m_mapAliGaoFangGateInfo.count(*it) > 0)
		{
			for (auto itGate = m_mapAliGaoFangGateInfo[*it].begin(); itGate != m_mapAliGaoFangGateInfo[*it].end(); itGate++)
			{
				nCount += itGate->second.m_userCount;
			}
		}

		if (m_mapYunmanGaoFangGateInfo.count(*it) > 0)
		{
			for (auto itGate = m_mapYunmanGaoFangGateInfo[*it].begin(); itGate != m_mapYunmanGaoFangGateInfo[*it].end(); itGate++)
			{
				nCount += itGate->second.m_userCount;
			}
		}

		std::stringstream ss;
		ss << "insert into onlinelog(DateTime, serverID, serverName, onlineCount) values('";
		ss << gWork.GetCurTime().Secs() << "','";
		ss << nServerId << "','";
		ss << string_To_UTF8(getServerNameByID(nServerId)) << "','";
		ss << nCount << "')";

		Lstring* sql = new Lstring(ss.str());
		if (m_bDBMode)
		{
			m_dbServer.Push(sql);
		}
		else
		{
			m_dbServerBK.Push(sql);
		}
		LLOG_ERROR("***********Suijun online Count: %d", nCount);
	}
}

bool Work::isLogicManagerExist(Lint logicManagerID)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);
	auto itLogicM = m_mapLogicManagerSp.find(logicManagerID);
	if(itLogicM == m_mapLogicManagerSp.end())
	{
		return false;
	}

	return true;
}

bool Work::isLogicManagerExist(LSocketPtr sp)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);
	for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); itLogicM++)
	{
		if(itLogicM->second == sp)
		{
			return true;
		}
	}

	return false;
}

void Work::_initTickTime()
{
	LTime cur;
	m_600SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
}

bool Work::_initRedisAndActive()
{
	if(m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		if(!m_RedisClient.readDataByString(ActiveFreeSetKey, m_strActiveFreeSet))
		{
			m_strActiveFreeSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActiveFreeSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActiveFreeSet is %s", m_strActiveFreeSet.c_str());
		}

		if(!m_RedisClient.readDataByString(ActivePXSetKey, m_strActivePXSet))
		{
			m_strActivePXSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActivePXSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActivePXSet is %s", m_strActivePXSet.c_str());
		}

		if(!m_RedisClient.readDataByString(ActiveOUGCSetKey, m_strActiveOUGCSet))
		{
			m_strActiveOUGCSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActiveOUGCSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActiveOUGCSet is %s", m_strActiveOUGCSet.c_str());
		}

		if(!m_RedisClient.readDataByString(ActiveExchangeSetKey, m_strActiveExchangeSet))
		{
			m_strActiveExchangeSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActiveOUGCSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActiveExchangeSetKey is %s", m_strActiveExchangeSet.c_str());
		}

		return true;
	}
	else
	{
		m_strActiveFreeSet = "";
		m_strActivePXSet = "";
		m_strActiveOUGCSet = "";
		m_strActiveExchangeSet = "";
		return false;
	}
}

void Work::SendActiveSetToLM(Lint LMid)
{
	// 限时免费活动
	LMsgCE2LSetGameFree FreeActive;
	FreeActive.m_ServerID = LMid;
	FreeActive.m_strFreeSet = m_strActiveFreeSet;
	SendMsgToLogic(FreeActive, LMid);

	// 牌型活动
	LMsgCE2LSetPXActive PXActive;
	PXActive.m_ServerID = LMid;
	PXActive.m_strActiveSet = m_strActivePXSet;
	SendMsgToLogic(PXActive, LMid);

	// 老用户登录送房卡活动
	LMsgCE2LSetOUGCActive OUGCActive;
	OUGCActive.m_ServerID = LMid;
	OUGCActive.m_strActiveSet = m_strActiveOUGCSet;
	SendMsgToLogic(OUGCActive, LMid);

	LMsgCE2LSetExchActive ExchActive;
	ExchActive.m_ServerID = LMid;
	ExchActive.m_strActiveSet = m_strActiveExchangeSet;
	SendMsgToLogic(ExchActive,LMid);
	
	LMsgCE2LMGConfig Config;
	gMjConfig.CreatToLogicConfigMessage(Config);
	Config.m_ServerID = LMid;
	SendMsgToLogic(Config, LMid);
	
}

Lstring Work::GetGaofangIp(Lint gaofang_flag)
{
	boost::mutex::scoped_lock l(m_mutexGateInfo);
	switch (gaofang_flag)
	{
	case 0:
		return "";
	case 1:
		return m_mapGaoFangIp["ali"];
		break;
	case 2:
		return m_mapGaoFangIp["yunman"];
		break;
	default:		
		break;
	}	 
	return "";
}

// 通过HTTP修改高防GateIP配置
bool Work::LoadGaoFangIpConfigByHttpCmd(std::map<Lstring, Lstring> & httpParams, LSocketPtr sp)
{
	if (httpParams.count("flag") < 1)
	{
		SendRet("{\"errorCode\":21,\"errorMsg\":\"Error command - No flag parameter\"}", sp);//命令错误
		return false;
	}
	if (httpParams.count("ip") < 1)
	{
		SendRet("{\"errorCode\":22,\"errorMsg\":\"Error command - No ip parameter\"}", sp);//命令错误
		return false;
	}
	Lstring str_flag = httpParams["flag"];
	Lstring str_ip = httpParams["ip"];
	if (m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		boost::mutex::scoped_lock l(m_mutexGateInfo);
		if (str_flag == "ali")
		{
			if (m_RedisClient.writeDataByString("ali", str_ip))
			{
				m_mapGaoFangIp["ali"] = str_ip;
				LLOG_ERROR("Logwyz .......................HTTP Set ali IP= [%s]", m_mapGaoFangIp["ali"].c_str());
				SendRet("{\"errorCode\":0,\"errorMsg\":\"OK\"}", sp);
			}
			else
			{
				SendRet("{\"errorCode\":24,\"errorMsg\":\"Error - Failed to update ali IP to redis\"}", sp);
				return false;
			}
		}
		else if (str_flag == "yunman")
		{

			if (m_RedisClient.writeDataByString("yunman", str_ip))
			{
				m_mapGaoFangIp["yunman"] = str_ip;
				LLOG_ERROR("Logwyz .......................HTTP Set yumman IP= [%s]", m_mapGaoFangIp["yunman"].c_str());
				SendRet("{\"errorCode\":0,\"errorMsg\":\"OK\"}", sp);
			}
			else
			{
				SendRet("{\"errorCode\":24,\"errorMsg\":\"Error - Failed to update yunman IP to redis\"}", sp);//命令错误
				return false;
			}
		}
		else
		{
			SendRet("{\"errorCode\":29,\"errorMsg\":\"Error - Unkow flag value\"}", sp);//命令错误
			return false;
		}
	}
	else
	{
		SendRet("{\"errorCode\":23,\"errorMsg\":\"Error - Failed connect redis error\"}", sp);
		return false;
	}
	return true;

}

// 通过HTTP修改数据库配置
bool Work::LoadDatabaseConfigByHttpCmd(std::map<Lstring, Lstring> & httpParams, LSocketPtr sp)
{
	if (httpParams.count("db") < 1)
	{
		SendRet("{\"errorCode\":21,\"errorMsg\":\"Error command - Empty db parameter.\"}", sp);//命令错误
		return false;
	}
	
	Lstring db_mode = httpParams["db"];
	if (m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		Lint dbMode = atoi(db_mode.c_str());
		if ((dbMode != 0) && (dbMode != 1))
		{
			SendRet("{\"errorCode\":11, \"errorMsg\":\"Error db parameter value\"}", sp);
			return false;
		}

		bool bNewDbMode = dbMode == 0 ? true : false;
		if (m_bDBMode == bNewDbMode)
		{
			SendRet("{\"errorCode\":0, \"errorMsg\":\"OK\"}", sp);
			LLOG_ERROR("Exec http update db.  m_dbMode = %d, unchanged",  db_mode);
			return true;
		}
		else
		{
			if (m_RedisClient.writeDataByString("mj-db-src", dbMode))
			{
				SendRet("{\"errorCode\":0, \"errorMsg\":\"OK\"}", sp);			
				LLOG_ERROR("Exec http update db. m_dbMode = %d", dbMode);
				return true;
			}
			else
			{
				SendRet("{\"errorCode\":12, \"errorMsg\":\"Write redis mj-db-src error\"}", sp);
				LLOG_ERROR("Exec http update db.  m_dbMode = %d, unchanged", dbMode);
				return false;
			}
		}
	}
	else
	{
		SendRet("{\"errorCode\":23,\"errorMsg\":\"Error - Failed connect redis error\"}", sp);
		return false;
	}
	return false;

}

// 通过HTTP修改数据库配置
bool Work::AdjustUserByHttpCmd(std::map<Lstring, Lstring> & httpParams, LSocketPtr sp)
{
	if (httpParams.count("uid1") < 1)
	{
		SendRet("{\"errorCode\":21,\"errorMsg\":\"Error command - Empty uid1 parameter.\"}", sp);//命令错误
		return false;
	}

	if (httpParams.count("uid2") < 1)
	{
		SendRet("{\"errorCode\":21,\"errorMsg\":\"Error command - Empty uid2 parameter.\"}", sp);//命令错误
		return false;
	}
	Lstring uid1_str = httpParams["uid1"];
	Lstring uid2_str = httpParams["uid2"];

	Lint uid1  = atoi(uid1_str.c_str());
	Lint uid2  = atoi(uid2_str.c_str());
	LLOG_ERROR("AdjustUserByHttpCmd - uid1: %d uid2: %d", uid1, uid2);
	boost::shared_ptr<CSafeUser> safeUserTarget = gUserManager.getUser(uid1);
	boost::shared_ptr<CSafeUser> safeUserDelete = gUserManager.getUser(uid2);
	if ( safeUserTarget == NULL ||  safeUserDelete == NULL)
	{
		LLOG_ERROR("AdjustUserByHttpCmd - failed to find user uid1: %d uid2: %d", uid1, uid2);
		SendRet("{\"errorCode\":21,\"errorMsg\":\"Error fail to find user.\"}", sp);
		return false;
	}
	
	boost::shared_ptr<DUser> userTarget = safeUserTarget->getUser();
	boost::shared_ptr<DUser> userDelete = safeUserDelete->getUser();
	if (safeUserTarget == NULL || safeUserDelete == NULL)
	{
		LLOG_ERROR("AdjustUserByHttpCmd - failed to find user uid1: %d uid2: %d", uid1, uid2);
		SendRet("{\"errorCode\":21,\"errorMsg\":\"Error fail to find user.\"}", sp);
		return false;
	}

	if (userTarget->m_usert.m_unioid.empty() || userTarget->m_usert.m_unioid.length() < 16)
	{
		LLOG_ERROR("AdjustUserByHttpCmd - union id is not valid id: %d unionid: %s", uid2, userTarget->m_usert.m_unioid.c_str());
		SendRet("{\"errorCode\":21,\"errorMsg\":\"union id is not valid.\"}", sp);
		return false;
	}

	if (userTarget->m_usert.m_openid.empty() || userTarget->m_usert.m_openid.length() < 16)
	{
		LLOG_ERROR("AdjustUserByHttpCmd - open id is not valid id: %d unionid: %s", uid2, userTarget->m_usert.m_openid.c_str());
		SendRet("{\"errorCode\":21,\"errorMsg\":\"open id is not valid.\"}", sp);
		return false;
	}

	// Login update保存旧的用户ID
	Lstring old_unionId = userTarget->m_usert.m_unioid;
	Lstring old_openId = userTarget->m_usert.m_openid;

	gUserManager.delUser(userTarget->m_usert.m_unioid);
	gUserManager.delUser(userDelete->m_usert.m_unioid);

	//修改掉User1的OpenId, UnionId
	 
	userTarget->m_usert.m_unioid = userDelete->m_usert.m_unioid;
	userTarget->m_usert.m_openid = userDelete->m_usert.m_openid;
	gUserManager.addUser(userTarget);


	 
	userDelete->m_usert.m_unioid = uid2_str;
	userDelete->m_usert.m_openid = uid2_str;
	gUserManager.addUser(userDelete);	
	 

	 


	//更新数据库
	std::stringstream ss2;
	ss2 << "UPDATE user SET ";
	ss2 << " UnionId='" << userDelete->m_usert.m_unioid << "',";
	ss2 << " OpenId='" << userDelete->m_usert.m_openid << "'";
	ss2 << " WHERE Id='" << uid2 << "'";
	LLOG_ERROR("AdjustUserByHttpCmd sql =%s", ss2.str().c_str());
	LLOG_ERROR("AdjustUserByHttpCmd update user id:%d unionid: [%s]->[%s], openid: [%s]->[%s]  ", uid2, userTarget->m_usert.m_unioid.c_str(), userDelete->m_usert.m_unioid.c_str(), userTarget->m_usert.m_openid.c_str(), userDelete->m_usert.m_openid.c_str());

	Lstring* sql2 = new Lstring(ss2.str());
	gDbServerManager.Push(sql2, uid1);

	//更新数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << " UnionId='" << userTarget->m_usert.m_unioid << "',";
	ss << " OpenId='" << userTarget->m_usert.m_openid << "'";
	ss << " WHERE Id='" << uid1 << "'";

	LLOG_ERROR("AdjustUserByHttpCmd sql =%s", ss.str().c_str());
	LLOG_ERROR("AdjustUserByHttpCmd update user id:%d unionid: [%s]->[%s], openid: [%s]->[%s]  ", uid1, old_unionId.c_str(), userTarget->m_usert.m_unioid.c_str(), old_openId.c_str(), userTarget->m_usert.m_openid.c_str());

	Lstring* sql1 = new Lstring(ss.str());
	gDbServerManager.Push(sql1, uid1);
	SendRet("{\"errorCode\":0,\"errorMsg\":\"OK\"}", sp);
	return false;

}

//接收manager更新俱乐部桌子信息
void Work::HanderManagerUpdateData(LMsgLMG2CEUpdateData*msg)
{
	LLOG_DEBUG("Work::HanderManagerUpdateData");
	if (msg==NULL)return;
	gClubManager.updateClubDeskInfo(msg);

}

Lstring Work::Md5Hex(const void* src, Lsize len)
{
	unsigned char sign[17];
	memset(sign, 0, sizeof(sign));

	MD5((const unsigned char*)src, len, sign);

	//转换成16进制
	char signHex[33];
	memset(signHex, 0, sizeof(signHex));
	for (int i = 0; i < 16; ++i)
	{
		sprintf(signHex + i * 2, "%02x", sign[i]);
	}

	return std::string(signHex);
}


void  Work::HanderDelUserFromManager(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderDelUserFromManager userId = %s", param["userId"].c_str());

	Lstring strId = param["userId"];

	Lint userId = atoi(strId.c_str());

	LMsgCe2LMGDelUserFromManager msgSend;
	msgSend.m_userId = userId;

	Lint serverID = 16210;
	LLOG_ERROR("HanderDelUserFromManager, userId=[%d]", msgSend.m_userId);
	if (msgSend.m_userId == 0 )
	{
		LLOG_ERROR("HanderDelUserFromManager, userId=[%d]  ERROR", msgSend.m_userId);
		return;
	}
	gWork.SendMsgToLogic(msgSend, serverID);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void  Work::HanderFrushSpecActive(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderFrushSpecActive");
	gSpecActive.loadData();

	gSpecActive.sendToManager(16210);
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

//玩家申请加入俱乐部
void  Work::HanderClubUserApplyClub(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderClubUserApplyClub clubId = %s", param["club_no"].c_str());

	Lstring clubId = param["club_no"];
	Lint id = atoi(clubId.c_str());

	gClubManager.userApplyClub(id);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}