#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "UserManager.h"
#include "User.h"
#include "Desk.h"
#include "RoomVip.h"
#include "HttpRequest.h"
#include <openssl/md5.h>
#include "ActiveManager.h"
#include "RLogHttp.h"
#include "mhmsghead.h"

using namespace boost::asio;  
using boost::asio::ip::tcp; 

//初始化
bool Work::Init()
{
	m_logFileIndex = 1;
	//初始化随机因子
	LTime timeNow;
	srand(timeNow.Secs());
	

	//设置log文件
	LLog::Instance().SetFileName("LogicQipai");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}
	
	char buf[16] = { 0 };
	itoa(gConfig.GetInsidePort(), buf, 10);
	m_logFileName = Lstring("LogicQipai_") + Lstring(buf);
	LLog::Instance().SetFileName(m_logFileName.c_str());

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());
	
	 
	//LMsgFactory::Instance().Init();
	MH_InitFactoryMessage_ForMapMessage();
	MHInitFactoryMessage();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	//if (!CardManager::Instance()->Init())
	//{
	//	LLOG_ERROR("CardManager::Instance().Init() error");
	//	return false;
	//}

	//if (!CardManagerSx::Instance().Init())
	//{
	//	LLOG_ERROR("CardManager::Instance().Init() error");
	//	return false;
	//}

	//if (!SiChuanCard_XZ::Instance()->Init())
	//{
	//	LLOG_ERROR("SiChuanCard_XZ::Instance().Init() error");
	//	return false;
	//}

	//if (!SiChuanCard_THR::Instance()->Init())
	//{
	//	LLOG_ERROR("SiChuanCard_XZ::Instance().Init() error");
	//	return false;
	//}

	//if (!CardManagerSC::Instance().Init())
	//{
	//	LLOG_ERROR("CardManagerSC::Instance().Init() error");
	//	return false;
	//}

	//if (!SiChuanCard_FourTwo::Instance()->Init())
	//{
	//	LLOG_ERROR("CardManagerFT::Instance().Init() error");
	//	return false;
	//}

	//if (!SiChuanCard_DY::Instance()->Init())
	//{
	//	LLOG_ERROR("CardManagerDY::Instance().Init() error");
	//	return false;
	//}

	//if(!Card_ManYng::Instance()->Init())
	//{
	//	LLOG_ERROR("CardManagerDY::Instance().Init() error");
	//	return false;
	//}

	//if(!gSiChuanYBCardMgr->Init())
	//{
	//	LLOG_ERROR("gSiChuanYBCardMgr::Instance().Init() error");
	//	return false;
	//}

	//if(!gCardWanZhou->Init())
	//{
	//	LLOG_ERROR("SiChuanCard_WanZhou::Instance().Init() error");
	//	return false;
	//}

	/*
	if( !gRLT.Init( gConfig.GetRemoteLogUrl(), (size_t)gConfig.GetMaxCachedLogSize(), (size_t)gConfig.GetMaxCachedLogNum() ) )
	{
		LLOG_ERROR("gRLT.Init error");
		return false;
	}
	_checkRLogStart();
	*/
	

	if(!gVipLogMgr.Init())
	{
		LLOG_ERROR("gVipLogMgr.Init() error");
		return false;
	}

	if(!gRoomVip.Init())
	{
		LLOG_ERROR("gRoomVip.Init() error");
		return false;
	}
	//初始化机器人
//if(gConfig.GetIfAddRobot() && !gRobotManager.Init("settings/RobotConfig.txt"))
//{
//	LLOG_ERROR("gRobotManager.Init() error");
//	return false;
//}

	_initTickTime();

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	ConnectToLogicManager();

	ConnectToDb();

	m_memeoryRecycle.Start();

	//gCounterManager.Start();

	//gRLT.Start();

	gInsideNet.Start();

	LRunnable::Start();
}

//等待
void Work::Join()
{
	m_memeoryRecycle.Join();

	//gCounterManager.Join();

	//gRLT.Join();

	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	m_logicManager.reset();

	m_gateInfo.clear();

	//gCounterManager.Stop();

	//gRLT.Stop();

	gInsideNet.Stop();

	LRunnable::Stop();

	m_memeoryRecycle.Stop();
}

void Work::Clear()
{

}

void Work::Run()
{
	LTime msgTime;
	srand(msgTime.Secs() + gConfig.GetServerID() * 70);
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
		}

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

void Work::Tick(LTime& cur)
{
	//static bool bStartTest = true;

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		//gRobotManager.Tick();
		gRoomVip.Tick(cur);
		m_1SceTick = cur.MSecs();
	}

	//15秒循环一次
	if (cur.MSecs() - m_15SecTick > 15000)
	{
		m_15SecTick = cur.MSecs();

		//if (bStartTest)
		//{
			//this->InternelTest();
			//bStartTest = false;
		//}
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30000)  // 30*1000 = 30000
	{
		m_30SceTick = cur.MSecs();
		gVipLogMgr.Tick();

		_checkLogicManagerHeartBeat();
		_checkLogicDBHeartBeat();
		_checkCoinsServerHeartBeat();		 
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 300000)  // 5*60 * 1000 = 300000
	{
		_checkRLogStart();
		gRoomVip.MHPrintAllDeskStatus();
		m_600SceTick = cur.MSecs();

		// check log file size
		Lstring str_file_name = LTimeTool::GetLocalDateInString() + "_" + m_logFileName + ".log";
		FILE * f = fopen(str_file_name.c_str(), "rb");
		if (f)
		{
			fseek(f, 0, SEEK_END);
			long lsize = ftell(f);
			fclose(f);
			f = NULL;
			if (lsize > 1024 * 1024 * 100)
			{
				char buf[16] = { 0 };
				sprintf_s(buf, "_%02d%02d%02d%03d.log", cur.GetHour(), cur.GetMin(), cur.GetSec(), cur.GetMSec());
				Lstring backup_file_name = str_file_name + Lstring(buf);
				LLOG_ERROR("***rename log file to : %s", backup_file_name.c_str());
				rename(str_file_name.c_str(), backup_file_name.c_str());
			}
		}

	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		LLOG_ERROR("MSG_CLIENT_KICK");
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	//注册端口 或 ID冲突
	case MSG_KILL_CLIENT:
		LLOG_ERROR("MSG_KILL_CLIENT");
		HanderKillClient((LMsgKillClient*) msg);
		break;
	case MSG_G_2_L_LOGIN:
		HanderGateLogin((LMsgG2LLogin*)msg);
		break;
		//////////////////////////////////////////////////////////////////////////
		//gate 跟 logic之间的交互
	case MSG_LMG_2_L_USER_LOGIN:		
		HanderUserLogin((LMsgLMG2LLogin*)msg);
		break;

	//处理LogicManager发来的玩家创建房间消息
	//(11011)Manager发送过来创建房间
	case MSG_LMG_2_L_CREATE_DESK:
		handerLMGCreateDesk((LMsgLMG2LCreateDesk*)msg);
		break;

	case MSG_LMG_2_L_CREATE_DESK_FOR_OTHER:
		handerLMGCreateDeskForOther((LMsgLMG2LCreateDeskForOther*)msg);
		break;
	case MSG_LMG_2_L_CREATE_DESK_FOR_CLUB:
		LLOG_DEBUG("MSG_LMG_2_L_CREATE_DESK_FOR_CLUB");
		//handerLMGCreateDeskForClub((LMsgLMG2LCreateDeskForClub*)msg);
		break;
		//玩家创建俱乐部房间并加入
	case MSG_LMG_2_L_CREATE_CLUB_DESK_AND_ENTER:
		LLOG_DEBUG("MSG_LMG_2_L_CREATE_CLUB_DESK_AND_ENTER");
		handerLMGCreateClubDeskAndEnter((LMsgLMG2LCreateClubDeskAndEnter*)msg);
		break;
	//玩家加入俱乐部房间
	case MSG_LMG_2_L_ENTER_CLUB_DESK:
		LLOG_DEBUG("MSG_LMG_2_L_ENTER_CLUB_DESK");
		handerLMGEnterClubDesk((LMsgLMG2LEnterClubDesk*)msg);
		break;
	case MSG_LMG_2_L_CREATED_DESK_LIST:
		handerLMGCreatedDeskList((LMsgLMG2LCreatedDeskList*)msg);
		break;

	case MSG_LMG_2_L_DISSMISS_CREATED_DESK:
		handerLMGDisMissCreatedDesk((LMsgLMG2LDissmissCreatedDesk*)msg);
		break;
	//处理俱乐部会长强制解散房间
	case MSG_LMG_2_L_DISSMISS_CLUB_DESK:
		handerLMGDisMissClubDesk((LMsgLMG2LDissmissClubDesk*)msg);
		break;
	case MSG_LMG_2_L_API_DISMISS_ROOM:
		handerLMGAPIDisMissRoom((LMsgLMG2LAPIDissmissRoom*)msg);
		break;
	//玩家加入房间
	case MSG_LMG_2_L_ADDTO_DESK:
		handerLMGAddToDesk((LMsgLMG2LAddToDesk*)msg);
		break;
	case MSG_LMG_2_L_PAIXING_ACTIVE:
		handerPXActiveInfo((LMsgLMG2LPXActive*)msg);
		break;
	case MSG_CN_2_L_CREATE_COIN_DESK:
		HanderLMGCreateCoinDesk((LMsgCN2LCreateCoinDesk*)msg);
		break;
	//Manager更新Logic上玩家能量值
	case MSG_LMG_2_L_UPDATE_POINT_COINTS:
		LLOG_DEBUG("MSG_LMG_2_L_UPDATE_POINT_COINTS(11044)");
		HanderLMGUpdatePointCoint((LMsgLMG2LUpdatePointCoins*) msg);
		break;
	case MSG_G_2_L_USER_MSG:
		{
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				HanderGateUserMsg((LMsgG2LUserMsg*)msg,gate);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exist");
			}
		}
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		{
			LMsgG2LUserOutMsg* gmsg = (LMsgG2LUserOutMsg*)msg;
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if(gate && gmsg)
			{
				HanderUserLogout(gate->m_id, gmsg->m_strUUID);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exist");
			}
		}
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;
	case MSG_HEARDBEAT_REPLY:
		HanderHearBeatReply((LMsgHeartBeatReplyMsg*)msg);
		break;
	case MSG_LMG_2_GL_COINS_SERVER_INFO:
		HanderCoinsServerInfo((LMsgLMG2GLCoinsServerInfo*) msg);
		break;
  case MSG_LMG_2_L_G_REQUEST_SYNCHRO_DATA:
		{
			LLOG_DEBUG("MSG_LMG_2_L_G_REQUEST_SYNCHRO_DATA");
			LMsgLMG2LGRequestSynchroData *pmsg=(LMsgLMG2LGRequestSynchroData*)msg;
			if (pmsg && pmsg->m_needLoadUserInfo)
			{
				LLOG_ERROR("MSG_LMG_2_L_G_REQUEST_SYNCHRO_DATA  recv manager request  info ");
				_SynchroData2Manager();
			}
		}
		break;
	case MH_MSG_LMG_2_L_QUERY_ROOM_GPS_INFO:
		this->handerLMGQueryGPSInfo((MHLMsgLMG2LQueryRoomGPSInfo*)msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserLogin(LMsgLMG2LLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		//同步卡数
		pUser->m_userData.m_numOfCard2s = msg->m_card_num;
		pUser->m_isMedal = msg->m_isMedal;

		HanderDeskUserLogin(pUser, msg->m_gateId,  msg->m_ip,msg->m_strUUID, true, msg->m_buyInfo, msg->m_hide);
	}
	else
	{
		if ( pUser )
		{
			UserManager::Instance().DelUser(pUser);
			delete pUser;
			pUser = NULL;
		}
		LMsgL2LMGModifyUserState send;
		send.m_logicID = 0;
		send.m_strUUID = msg->m_strUUID;
		send.m_userid  = msg->m_userid;
		send.m_userstate = LGU_STATE_CENTER;
		SendToLogicManager(send);
		SendToCoinsServer( send );
		LLOG_ERROR("Work::HanderUserLogin user login ERROR!");
	}
}

void Work::HanderDeskUserLogin(User* pUser, Lint gateId, Lstring& ip,Lstring uuid, bool firstLogin , Lstring buyInfo , Lint hide )
{
	if (!pUser)
	{
		return;
	}
	UserManager::Instance().DelUser(pUser);
	pUser->SetUserGateId(gateId);
	pUser->SetIp(ip);
	if (!uuid.empty()) {
		LLOG_ERROR("HanderDeskUserLogin user[%d] user->uuid[%s] msg->uuid[%s]", pUser->GetUserDataId(), pUser->m_userData.m_unioid.c_str(), uuid.c_str());
		pUser->m_userData.m_unioid = uuid;
	}
	UserManager::Instance().AddUser(pUser);
	if (firstLogin)
	{
		pUser->SendItemInfo();
		pUser->SendLoginInfo(buyInfo, hide);
	}
	pUser->Login();
	if ( pUser->m_desk && pUser->m_desk->GetDeskType() == DeskType_Coins )
	{
		pUser->setUserState(LGU_STATE_COINDESK);
	}
	else
	{
		pUser->setUserState(LGU_STATE_DESK);
	}
	pUser->ModifyUserState();
}

void Work::HanderUserLogout(Lint gateId, const Lstring& strUUID)
{
	User* user = UserManager::Instance().GetUserByGateIdAndUUID(gateId, strUUID);
	if (user && user->GetDesk())
	{
		LLOG_DEBUG("Work::HanderUserLogout %d", user->GetUserDataId());

		user->Logout();
		//在桌子中
		user->GetDesk()->HanderUserOutDesk(user);
		if ( user->GetDesk()->GetDeskType() == DeskType_Coins )
		{
			user->setUserState(LGU_STATE_COIN);
		}
		else
		{
			user->setUserState(LGU_STATE_CENTER);
		}
		user->ModifyUserState();
	}
	else
	{
		if ( user )
		{
			UserManager::Instance().DelUser(user);
			delete user;
			user = NULL;
		}
		LLOG_DEBUG("Work::HanderUserLogout user not find %d:%s", gateId, strUUID.c_str());
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if (msg->m_sp == m_logicManager)
	{
		_reconnectLogicManager();
	}
	else if(msg->m_sp == m_dbClient)
	{
		_reconnectLogicDB();
	}
	else if(m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		_reconnectCoinsServer();
	}
	else
	{
		HanderGateLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(msg->m_sp == m_logicManager)
	{
		LMsgL2LMGLogin login;
		login.m_id = gConfig.GetServerID();
		login.m_ip = gConfig.GetInsideIp();
		login.m_key = gConfig.GetCenterKey();
		login.m_port = gConfig.GetInsidePort();
		login.m_type= gConfig.GetSubGameType();
		if (login.m_type == LOGIC_SERVER_MJ)
			login.m_type = LOGIC_SERVER_QP;
		LLOG_ERROR("logic id [%d] register  manager server  game type=[%d]", login.m_id, login.m_type);

		SendToLogicManager(login);
	}
	else if(msg->m_sp == m_dbClient)
	{
		LMsgL2LdbLogin msg;
		msg.m_key = gConfig.GetDBKey();
		msg.m_serverID = gConfig.GetServerID();
		SendMsgToDb(msg);
	}
	else if(m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		LMsgL2CNLogin login;
		login.m_id = gConfig.GetServerID();
		login.m_ip = gConfig.GetInsideIp();
		login.m_key = gConfig.GetCenterKey();
		login.m_port = gConfig.GetInsidePort();
		m_coinsServer.m_sp->Send(login.GetSendBuff());
	}
	m_mapReconnect.erase(msg->m_sp);
}

//处理客户端注册时端口 或 ID 冲突，程序退出
void Work::HanderKillClient(LMsgKillClient* msg)
{
	for (int i = 0; i < 3; ++i)
	{
		LLOG_ERROR("******************************** ERROR:ID Conflict *********************************************");
	}
	LLOG_ERROR("Work::HanderKillClient() ID Conflict Error!!!, New Start Client Id Conflict!!! Id=[%d], port=[%d]", 
		gConfig.GetServerID(), gConfig.GetInsidePort());
	for (int i = 0; i < 3; ++i)
	{
		LLOG_ERROR("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ERROR:ID Conflict !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}

	//msg->m_sp->Stop();
	Work::Stop();
	return;
}

void Work::ConnectToLogicManager()
{
	m_logicManager = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectToLogicManager begin %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	if(m_logicManager->Connect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort()))
	{
		LLOG_ERROR("Work::ConnectToLogicManager end %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	}
	else
	{
		LLOG_ERROR("Fail to connect logic manager");
		Stop();
	}
}

void Work::SendToLogicManager(LMsg& msg)
{
	if(m_logicManager && m_logicManager->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
}

void Work::HanderGateLogin(LMsgG2LLogin* msg)
{
	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	auto itGateInfo = m_gateInfo.find(msg->m_id);
	if(itGateInfo != m_gateInfo.end())
	{
		LLOG_ERROR("Gate reconnect logicserver. %d", msg->m_id);
		itGateInfo->second.m_sp->Stop();
		m_gateInfo.erase(itGateInfo);
	}

	GateInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_userCount = 0;
	info.m_sp = msg->m_sp;
	m_gateInfo[info.m_id] = info;

	//SendGateInfoToCenter();
}

void Work::HanderGateLogout(LMsgKick* msg)
{
	GateInfo* info = GetGateInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR(" Work::HanderGateLogout: %d", info->m_id);
		DelGateInfo(info->m_id);
	}
}

GateInfo* Work::GetGateInfoBySp(LSocketPtr sp)
{
	std::map<Lint, GateInfo>::iterator it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

GateInfo* Work::GetGateInfoById(Lint id)
{
	if (m_gateInfo.count(id))
		return &m_gateInfo[id];

	return NULL;
}

void Work::DelGateInfo(Lint id)
{
	if (m_gateInfo.count(id))
	{
		m_gateInfo.erase(m_gateInfo.find(id));
	}
}

void Work::HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate)
{
	if(msg == NULL || gate == NULL || msg->m_userMsg == NULL)
	{
		return;
	}

	Lint gateId = gate->m_id;
	User* user = gUserManager.GetUserByGateIdAndUUID(gateId, msg->m_strUUID);
	if(user)
	{
		user->HanderMsg(msg->m_userMsg);
	}
	else
	{
		LLOG_DEBUG("Work::HanderGateUserMsg user not exist %d:%s:%d", gateId, msg->m_strUUID.c_str(), msg->m_userMsgId);
	}
}

void Work::HanderCoinsServerInfo(LMsgLMG2GLCoinsServerInfo* msg)
{
	LLOG_ERROR( "Work::HanderCoinsServerInfo ip=%s port=%d", msg->m_conis.m_ip.c_str(), msg->m_conis.m_port );
	// 每次重新连接就行
	m_coinsServer.m_ip = msg->m_conis.m_ip;
	m_coinsServer.m_port = msg->m_conis.m_port;
	if ( !m_coinsServer.m_ip.empty() )
	{
		m_coinsServer.m_sp = gInsideNet.GetNewSocket();
		m_coinsServer.m_sp->Connect(msg->m_conis.m_ip, msg->m_conis.m_port);
	}
	else
	{
		// 如果金币服务器的ip为空 表示金币服务器失去了Manager的连接的
		m_coinsServer.m_sp.reset();
		m_mapReconnect.erase(m_coinsServer.m_sp);
	}
}

void Work::SendToCoinsServer(LMsg& msg)
{
	if ( m_coinsServer.m_sp && m_coinsServer.m_sp->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED )
	{
		m_coinsServer.m_sp->Send(msg.GetSendBuff());
	}
}

Lstring Work::Md5Hex(const void* src ,Lsize len)
{
	unsigned char sign[17];
	memset(sign,0,sizeof(sign));

	MD5((const unsigned char*)src,len,sign);

	//转换成16进制
	char signHex[33];
	memset(signHex,0,sizeof(signHex));
	for(int i = 0 ; i < 16; ++i)
	{
		sprintf(signHex + i*2,"%02x", sign[i]);
	}

	return std::string(signHex);
}

//////////////////////////////////////////////////////////////////////////
//DB链接相关
void Work::ConnectToDb()
{
	m_dbClient = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectToDb begin %s:%d", gConfig.GetDBIp().c_str(), gConfig.GetDBPort());
	if(m_dbClient->Connect(gConfig.GetDBIp(), gConfig.GetDBPort()))
	{
		LLOG_ERROR("Work::ConnectToDb end");
	}
	else
	{
		LLOG_ERROR("Fail to connect DB");
		Stop();
	}
}

void Work::SendMsgToDb(LMsg& msg)
{
	if(m_dbClient)
	{
		m_dbClient->Send(msg.GetSendBuff());
	}
}

/* 处理LogicManager发来的玩家创建房间消息 */
void Work::handerLMGCreateDesk(LMsgLMG2LCreateDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		pUser->m_userData.m_numOfCard2s = msg->m_usert.m_numOfCard2s;
		pUser->m_userData.m_customString1 = msg->m_usert.m_customString1;
		pUser->m_userData.m_customString2 = msg->m_usert.m_customString2;
		pUser->m_isMedal = msg->m_isMedal;

		//找到玩家了， 判断是否有桌子，有就加入桌子
		HanderDeskUserLogin(pUser, msg->m_gateId,  msg->m_ip,msg->m_usert.m_unioid);
		RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
	}
	else
	{
		if ( pUser )
		{
			// 把之前可能存在的玩家删掉
			UserManager::Instance().DelUser(pUser);
			delete pUser;
			pUser = NULL;
		}
		//没有的话，覆盖数据，创建桌子
		pUser = new User(msg->m_usert, msg->m_gateId);
		if (pUser)
		{
			pUser->m_isMedal = msg->m_isMedal;
			pUser->SetIp(msg->m_ip);
			UserManager::Instance().AddUser(pUser);
			pUser->Login();	

			//创建桌子
			Lint nErrorCode = pUser->HanderUserCreateDesk(msg);
			if (nErrorCode == 0)
			{
				//创建房间成功
				pUser->setUserState(LGU_STATE_DESK);
				pUser->ModifyUserState();
			}
			else
			{
				//创建房间失败
				LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed! ErrorCode: %d", nErrorCode);
				HanderUserLogout(msg->m_gateId, msg->m_strUUID);
				RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
			}
		}
		else
		{
			//创建房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed No User!");
			HanderUserLogout(msg->m_gateId, msg->m_strUUID);
			RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
		}
	}
	//崩溃测试
	/*
	User* pUser1 = NULL;
	pUser1->GetDesk();
	*/

}



void Work::handerLMGCreateDeskForOther(LMsgLMG2LCreateDeskForOther* msg)
{
	if (msg == NULL)
	{
		return;
	}

	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);

	if (pUser == NULL) {
		LLOG_DEBUG("Work::handerLMGCreateDeskForOther have no user need create", );
		LLOG_DEBUG("Work::handerLMGCreateDeskForOther msg->m_usert.m_id = %d ", msg->m_usert.m_id);
		pUser = new User(msg->m_usert, msg->m_gateId); 
	}else {
		pUser->m_userData.m_numOfCard2s = msg->m_usert.m_numOfCard2s;
		pUser->m_userData.m_customString1 = msg->m_usert.m_customString1;
		pUser->m_userData.m_customString2 = msg->m_usert.m_customString2;
	}

	if (pUser)
	{
		//创建桌子
		Lint nErrorCode = pUser->HanderUserCreateDeskForOther(msg);
		if (nErrorCode == 0)
		{
			//创建成功
		}
		else
		{
			//创建房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed! ErrorCode: %d", nErrorCode);

			RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
		}

		if (gUserManager.GetUserbyDataId(msg->m_userid) == NULL) {
			//delete(pUser);
			//pUser = NULL;
		}
	}
	else
	{
		//创建房间失败
		LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed No User!");
		RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
	}

}

void Work::handerLMGCreatedDeskList(LMsgLMG2LCreatedDeskList* msg)
{	
	LLOG_DEBUG("Work::handerLMGCreatedDeskList send to User!");
	if (msg == NULL)
	{
		return;
	}

	for (Lint i = 0; i < msg->m_deskList.size(); i++) {
		LLOG_DEBUG("Work::handerLMGCreatedDeskList msg->m_deskId =  %d", msg->m_deskId);
		Desk* m_pDesk = gRoomVip.GetDeskById(msg->m_deskList[i]);
		if (!m_pDesk) {
			LLOG_ERROR("Work::handerLMGCreateDesk find desk  Failed! deskId =  %d ", msg->m_deskId);
			continue;
		}

		LMsgS2CDeskList deskData;
		deskData.m_errorCode = 0;
		deskData.m_deskId = m_pDesk->GetDeskId();
		LLOG_DEBUG("Work::handerLMGCreatedDeskList deskData.m_deskId =  %d", deskData.m_deskId);
		deskData.m_creatTime = m_pDesk->GetCreatDeskTime();
		deskData.m_playtype = m_pDesk->getPlayType();
		deskData.m_deskPlayerName = m_pDesk->getAllPlayerName();
		deskData.m_cheatAgainst = m_pDesk->m_cheatAgainst;
		deskData.m_feeType = m_pDesk->m_feeType;
		deskData.m_cellscore = m_pDesk->m_cellscore;

		deskData.m_flag = m_pDesk->m_flag;
		deskData.m_state = m_pDesk->m_state;
		deskData.m_Greater2CanStart=m_pDesk->MHGetGreater2CanStart();
    deskData.m_GpsLimit = m_pDesk->m_Gps_Limit;
		Lint deskState = m_pDesk->getDeskState();
		if (DESK_PLAY == deskState) {
			deskData.m_share = 0;
			deskData.m_dissmiss = 0;
		}


		User* pUser = new User(msg->m_usert, msg->m_gateId);

		if (pUser)
		{
			LLOG_DEBUG("Work::handerLMGCreatedDeskList send to User!  deskData.m_deskId = %d ", deskData.m_deskId);

			//发送玩家桌子数据
			pUser->Send(deskData);
		}

		//if (!gUserManager.GetUserbyDataId(msg->m_userid))
		delete(pUser);
	}

}


void Work::handerLMGDisMissCreatedDesk(LMsgLMG2LDissmissCreatedDesk* msg)
{

	LLOG_DEBUG("Work::handerLMGDisMissCreatedDesk delete desk!");
	if (msg==NULL)
	{
		return;
	}

	LMsgS2CCreaterResetRoom send;

	send.m_errorCode=0;
	send.m_deskId=msg->m_deskId;
	send.m_strErrorDes="解散房间成功";

	User* pUser=new User(msg->m_usert, msg->m_gateId);

	Desk* m_pDesk=gRoomVip.GetDeskById(msg->m_deskId);
	if (!m_pDesk) {
		LLOG_ERROR("Work::handerLMGDisMissCreatedDesk find desk  Failed! deskId =  %d ", msg->m_deskId);
		send.m_errorCode=1;
		send.m_strErrorDes="room not exist";

		if (pUser)
		{
			//发送玩家桌子数据
			pUser->Send(send);
			delete pUser;
		}
		return;
	}

	if (m_pDesk!=NULL  &&  m_pDesk->m_vip==NULL)
	{
		LLOG_ERROR("Work::handerLMGDisMissCreatedDesk delete desk!  m_pDesk->m_vip==NULL  gateId=[%d] ", msg->m_gateId);
	}

	Lint temp=m_pDesk->getDeskState();
	if (m_pDesk->getDeskState()==DESK_PLAY||(m_pDesk->m_vip!=NULL  && m_pDesk->m_vip->m_curCircle>1)||m_pDesk->m_selectSeat==false)
	{
		send.m_errorCode=2;
		send.m_strErrorDes="房间已经开局";

		if (pUser)
		{
			//发送玩家桌子数据
			pUser->Send(send);
			delete pUser;
		}
		return;

	}

	if (m_pDesk!=NULL&&(m_pDesk->getDeskState()==DESK_WAIT||m_pDesk->getDeskState()==DESK_FREE))
	{
		LLOG_ERROR("Work::DisMissCreatedDesk, userid=%d deskid=%d deskstate=%d", msg->m_userid, m_pDesk->GetDeskId(), m_pDesk->getDeskState());


		//发送删除信息,删除房主创建的对应的桌子号
		LMsgL2LMGDeleteUserCreatedDesk deleteDeskInfo;
		deleteDeskInfo.m_deskId=msg->m_deskId;
		deleteDeskInfo.m_userid=msg->m_userid;
		if (m_pDesk->m_feeType==0)
		{
			deleteDeskInfo.m_cost=m_pDesk->m_cost;
		}
		deleteDeskInfo.m_delType=2;
		gWork.SendToLogicManager(deleteDeskInfo);


		if (m_pDesk->getDeskState()==DESK_WAIT)
		{
			m_pDesk->m_dismissUser=true;
			//m_pDesk->BoadCast(send);
			m_pDesk->MHBoadCastAllDeskUser(send);
			bool bDissmiss=m_pDesk->DissmissAllplayer();
		}


	}

	//User* pUser = new User(msg->m_usert, msg->m_gateId);

	if (pUser)
	{
		//发送玩家桌子数据
		pUser->Send(send);
		delete pUser;
	}


}

void Work::handerLMGAddToDesk(LMsgLMG2LAddToDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		pUser->m_userData.m_numOfCard2s = msg->m_usert.m_numOfCard2s;
		pUser->m_userData.m_customString1 = msg->m_usert.m_customString1;
		pUser->m_userData.m_customString2 = msg->m_usert.m_customString2;
		pUser->m_isMedal = msg->m_isMedal;
		//找到玩家了， 判断是否有桌子，有就加入桌子
		HanderDeskUserLogin(pUser, msg->m_gateId,  msg->m_ip,msg->m_usert.m_unioid);
	}
	else
	{
		if ( pUser )
		{
			// 把之前可能存在的玩家删掉
			UserManager::Instance().DelUser(pUser);
			delete pUser;
			pUser = NULL;
		}
		pUser = new User(msg->m_usert, msg->m_gateId);
		if (pUser)
		{
			pUser->m_isMedal = msg->m_isMedal;
			pUser->SetIp(msg->m_ip);
			UserManager::Instance().AddUser(pUser);
			pUser->Login();
			Lint nError = pUser->HanderUserAddDesk(msg);
			if (nError == 0)
			{
				//加入房间成功
				pUser->setUserState(LGU_STATE_DESK);
				pUser->ModifyUserState();
			}
			else
			{
				if (nError == -1)return;

				//加入房间失败
				LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed! ErrorCode: %d", nError);
				HanderUserLogout(msg->m_gateId, msg->m_strUUID);
			}
		}
		else
		{
			//加入房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed No User!");
			HanderUserLogout(msg->m_gateId, msg->m_strUUID);
		}
	}
}

//处理LM发送的牌型活动配置信息
void Work::handerPXActiveInfo(LMsgLMG2LPXActive* msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("Work::handerPXActiveInfo msg is null!");
		return;
	}
	gLogicActiveManager.PXReloadConfig(msg);
}

void Work::HanderLMGCreateCoinDesk( LMsgCN2LCreateCoinDesk* msg )
{
	LMsgL2CNCreateCoinDeskRet ret;
	ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_NoError;
	ret.m_deskId = msg->m_deskId;   //用于coinserver回收桌子号

	std::vector<LMsgCN2LCreateCoinDesk::User> &userList = msg->m_users;

	if(userList.size() != DESK_USER_COUNT)
	{
		LLOG_ERROR("Work::HanderLMGCreateCoinDesk usersize error, size=%d", userList.size());
		ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_SizeError;   //人数错误
		SendToCoinsServer(ret);
		return;
	}

	for(Lsize i = 0; i < userList.size(); ++i)
	{
		User* pUser = gUserManager.GetUserbyDataId(userList[i].m_usert.m_id);
		if (pUser && pUser->GetDesk())
		{
			LLOG_ERROR("Work::HanderLMGCreateCoinDesk user already in desk error, userid=%d", pUser->GetUserDataId());
			ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_UserError;   //用户已经在桌子上
			SendToCoinsServer(ret);
			return;
		}
	}

	User* userArray[DESK_USER_COUNT] = {0};	// 按照位置排序
	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		Lint pos = userList[i].m_pos;
		if ( pos >= 0 && pos < DESK_USER_COUNT && !userArray[pos] )
		{
			userArray[pos] = new User(userList[i].m_usert, userList[i].m_gateId);
			if (userArray[pos])
			{
				userArray[pos]->SetIp(userList[i].m_ip);
			}
		}
		else
		{
			// 删除玩家
			for(Lint k = 0; k < DESK_USER_COUNT; ++k)
			{
				if(userArray[k])
				{
					delete userArray[k];
				}
			}
			LLOG_ERROR("Work::HanderLMGCreateCoinDesk pos error, pos=%d", i);
			ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_PosError;
			SendToCoinsServer(ret);
			return;
		}
	}

	//创建桌子
	Lint nErrorCode = gRoomVip.CreateVipCoinDesk(msg,userArray);
	if (nErrorCode == 0)  //创建房间成功
	{
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			UserManager::Instance().AddUser(userArray[i]);
			userArray[i]->setUserState(LGU_STATE_COINDESK);
			userArray[i]->ModifyUserState();
		}
	}
	else
	{
		//创建房间失败
		LLOG_ERROR("Work::HanderLMGCreateCoinDesk CreateTale Failed! ErrorCode: %d", nErrorCode);
		ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_Unknown;
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			if(userArray[i])
			{
				delete userArray[i];
				userArray[i] = NULL;
			}
		}
	}
	SendToCoinsServer(ret);
}

//manager更新logic上玩家的能量值
void Work::HanderLMGUpdatePointCoint(LMsgLMG2LUpdatePointCoins* msg)
{
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userId);
	if (pUser)
	{
		LLOG_ERROR("HanderLMGUpdatePointCoint  userId[%d] point[%d] currPoint[%d]", msg->m_userId,msg->m_changeCoins,msg->m_currCoins);
		pUser->m_userData.m_coins += msg->m_changeCoins;

		pUser->HanderUpdateUserCoins(msg);
	}
	else
	{
		LLOG_ERROR("HanderLMGUpdatePointCoint not find userId[%d]",msg->m_userId);
	}
}

void Work::RecycleDeskId(Lint deskID)
{
	LMsgL2LMGRecyleDeskID msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_deskID = deskID;
	gWork.SendToLogicManager(msg);
}

//俱乐部桌子
void   Work::RecycleDeskId(Lint deskID, Lint clubId=0, Lint playTypeId=0, Lint clubDeskId=0)
{
	LLOG_DEBUG("Work::RecycleDeskId  deskID=[%d], clubId=[%d],playTypeId=[%d],clubDeskId=[%d]", deskID, clubId, playTypeId, clubDeskId);
	LMsgL2LMGRecyleDeskID msg;
	msg.m_serverID=gConfig.GetServerID();
	msg.m_deskID=deskID;
	msg.m_clubId=clubId;
	msg.m_playTypeId=playTypeId;
	msg.m_clubDeskId=clubDeskId;
	gWork.SendToLogicManager(msg);

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

	LMsgHeartBeatReplyMsg replyMsg;
	replyMsg.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	replyMsg.m_iServerId = gConfig.GetServerID();

	msg->m_sp->Send(replyMsg.GetSendBuff());
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

void Work::_initTickTime()
{
	LTime cur;
	m_1SceTick = cur.MSecs();
	m_15SecTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();
}

void Work::_reconnectLogicManager()
{
	if(!m_logicManager)
	{
		LLOG_ERROR("Logic manager socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic manager...");
	LTime curTime;
	Llong lOldTime = 0;
	auto itReconnect = m_mapReconnect.find(m_logicManager);
	if(itReconnect != m_mapReconnect.end()) //说明已经存在重连了
	{
		if((curTime.MSecs() - itReconnect->second) > 3 * 60 * 1000) //超过3分钟重连不再重连了
		{
			LLOG_ERROR("Reconnect time is more than 3 minutes. Stop to reconnect logic manager");
			lOldTime = curTime.MSecs();
		}
		else
		{
			lOldTime = itReconnect->second;
		}
	}

	m_mapReconnect.erase(m_logicManager);
	m_mapHeartBeat.erase(m_logicManager);

	std::ostringstream ss;
	Lstring result;
	ss << "http://api.ry.haoyunlaiyule.com/server/alert?type=20&server_id=" << gConfig.GetServerID();
	//非Debug模式下连不上manager才会报警
	if (!gConfig.GetDebugModel())
	{
		if (HttpRequest::Instance().HttpGet(ss.str(), result))
		{
			LLOG_ERROR("Send alter message: %s", ss.str().c_str());
		}
		else
		{
			LLOG_ERROR("Fail to send alter message: %s", ss.str().c_str());
		}
	}

	m_logicManager->Stop();
	m_logicManager = gInsideNet.GetNewSocket();
	m_logicManager->AsyncConnect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort());

	if(lOldTime != 0)
	{
		m_mapReconnect[m_logicManager] = lOldTime;
	}
	else
	{
		m_mapReconnect[m_logicManager] = curTime.MSecs();
	}
}

void Work::_reconnectLogicDB()
{
	if(!m_dbClient)
	{
		LLOG_ERROR("Logic DB socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic DB...");
	m_mapReconnect.erase(m_dbClient);
	m_mapHeartBeat.erase(m_dbClient);

	m_dbClient->Stop();
	m_dbClient = gInsideNet.GetNewSocket();
	m_dbClient->AsyncConnect(gConfig.GetDBIp(), gConfig.GetDBPort());

	m_mapReconnect[m_dbClient] = true;
}

void Work::_reconnectCoinsServer()
{
	if(!m_coinsServer.m_sp)
	{
		LLOG_ERROR("CoinsServer socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic CoinsServer...");
	m_mapReconnect.erase(m_coinsServer.m_sp);
	m_mapHeartBeat.erase(m_coinsServer.m_sp);

	m_coinsServer.m_sp->Stop();
	m_coinsServer.m_sp = gInsideNet.GetNewSocket();
	m_coinsServer.m_sp->AsyncConnect(m_coinsServer.m_ip, m_coinsServer.m_port);

	m_mapReconnect[m_coinsServer.m_sp] = true;
}

void Work::_checkLogicManagerHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连LogicManager，则不进行心跳
	if(m_mapReconnect.find(m_logicManager) != m_mapReconnect.end())
	{
		return;
	}

	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_logicManager) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("LogicManager network had problem. Reconnect....");
		std::ostringstream ss;
		Lstring result;
		ss << "http://api.ry.haoyunlaiyule.com/server/alert?type=20&server_id=" << gConfig.GetServerID();
		if (HttpRequest::Instance().HttpGet(ss.str(), result))
		{
			LLOG_ERROR("Send alter message: %s", ss.str().c_str());
		}
		else
		{
			LLOG_ERROR("Fail to send alter message: %s", ss.str().c_str());
		}
		_reconnectLogicManager();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	msgHeatbeat.m_iServerId = gConfig.GetServerID();

	SendToLogicManager(msgHeatbeat);

	m_mapHeartBeat[m_logicManager] = true;
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
		_reconnectLogicDB();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	msgHeatbeat.m_iServerId = gConfig.GetServerID();

	SendMsgToDb(msgHeatbeat);

	m_mapHeartBeat[m_dbClient] = true;
}

void Work::_checkCoinsServerHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连CoinsServer，则不进行心跳
	if(m_mapReconnect.find(m_coinsServer.m_sp) != m_mapReconnect.end())
	{
		return;
	}

	if ( !m_coinsServer.m_sp )
	{
		// 金币服务器没有连接
		return;
	}
	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_coinsServer.m_sp) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("CoinsServer network had problem. Reconnect....");
		_reconnectCoinsServer();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	msgHeatbeat.m_iServerId = gConfig.GetServerID();

	m_coinsServer.m_sp->Send(msgHeatbeat.GetSendBuff());

	m_mapHeartBeat[m_coinsServer.m_sp] = true;
}

void Work::_checkRLogStart()
{
	LIniConfig	m_ini;
	m_ini.LoadFile("qipai_config.ini");

	gRLT.SetOpen( m_ini.GetInt("RemoteLogStart", 0) != 0 );
	gRLT.SetURL( m_ini.GetString("RemoteLogUrl", "") );
}

// 测试函数
//void Work::InternelTest()
//{
//	//模拟创建房间
//	LMsgLMG2LCreateDesk   createdesk;	 
//	createdesk.m_state = 102;
//	createdesk.m_flag = 1;
//	createdesk.m_usert.m_id = 11;
//	createdesk.m_usert.m_unioid = "Test111";
//	createdesk.m_usert.m_openid = "Test111";
//	createdesk.m_usert.m_nike = "Test111";
//	createdesk.m_gateId = 1;
//	createdesk.m_deskID = 666666;
//	this->handerLMGCreateDesk(&createdesk);
//
//	//模拟加入房间
//	LMsgLMG2LAddToDesk adddesk2;
//	adddesk2.m_deskID = 666666;
//	adddesk2.m_usert.m_id = 12;
//	adddesk2.m_usert.m_unioid = "Test222";
//	adddesk2.m_usert.m_openid = "Test222";
//	createdesk.m_usert.m_nike = "Test222";
//	adddesk2.m_gateId = 1;
//	this->handerLMGAddToDesk(&adddesk2);
//
//	//模拟加入房间
//	LMsgLMG2LAddToDesk adddesk;
//	adddesk.m_deskID = 666666;
//	adddesk.m_usert.m_id = 13;
//	adddesk.m_usert.m_unioid = "Test333";
//	adddesk.m_usert.m_openid = "Test333";
//	createdesk.m_usert.m_nike = "Test333";
//	adddesk.m_gateId = 1;
//	this->handerLMGAddToDesk(&adddesk);
//
//	//模拟加入房间	
//	adddesk.m_deskID = 666666;
//	adddesk.m_usert.m_id = 14;
//	adddesk.m_usert.m_unioid = "Test444";
//	adddesk.m_usert.m_openid = "Test444";
//	createdesk.m_usert.m_nike = "Test444";
//	adddesk.m_gateId = 1;
//	this->handerLMGAddToDesk(&adddesk);
//
//	//模拟加入房间	
//	adddesk.m_deskID = 666666;
//	adddesk.m_usert.m_id = 15;
//	adddesk.m_usert.m_unioid = "Test555";
//	adddesk.m_usert.m_openid = "Test555";
//	createdesk.m_usert.m_nike = "Test555";
//	adddesk.m_gateId = 1;
//	this->handerLMGAddToDesk(&adddesk);
//
//
//	//模拟用户准备
//	LMsgC2SUserReady ready;
//	ready.m_pos = 0;
//	Desk * pDesk = gRoomVip.GetDeskById(666666);
//	User * pUser1 = gUserManager.GetUserbyDataId(11);
//	User * pUser2 = gUserManager.GetUserbyDataId(12);
//	User * pUser3 = gUserManager.GetUserbyDataId(13);
//	User * pUser4 = gUserManager.GetUserbyDataId(14);
//	User * pUser5 = gUserManager.GetUserbyDataId(15);
//	pDesk->HanderUserReady(pUser1, &ready);
//
//	ready.m_pos = 1;
//	pDesk->HanderUserReady(pUser2, &ready);
//
//	ready.m_pos = 2;
//	pDesk->HanderUserReady(pUser3, &ready);
//
//	ready.m_pos = 3;
//	pDesk->HanderUserReady(pUser4, &ready);
//
//	ready.m_pos = 4;
//	pDesk->HanderUserReady(pUser5, &ready);
//}

void Work::handerLMGCreateDeskForClub(LMsgLMG2LCreateDeskForClub* msg)
{
	LLOG_DEBUG("Work::handerLMGCreateDeskForClub");
	if (msg==NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::handerLMGCreateDeskForClub  m_deskCount=[%d]", msg->m_deskCount);
	//User* pUser=gUserManager.GetUserbyDataId(msg->m_userid);

	//if (pUser==NULL) {
	//	//LLOG_DEBUG("Work::handerLMGCreateDeskForClub have no user need create", );
	//	LLOG_DEBUG("Work::handerLMGCreateDeskForClub   msg->m_userid =[%d],msg->m_usert.m_id =[%d] ", msg->m_userid,msg->m_usert.m_id);
	//	pUser=new User(msg->m_usert, msg->m_gateId);
	//}
	//else {
	//	pUser->m_userData.m_numOfCard2s=msg->m_usert.m_numOfCard2s;
	//	pUser->m_userData.m_customString1=msg->m_usert.m_customString1;
	//	pUser->m_userData.m_customString2=msg->m_usert.m_customString2;
	//}


	//创建桌子
	Lint nErrorCode=gRoomVip.CreateVipDeskForClub(msg);
	if (nErrorCode==0)
	{
		//创建成功
	}
	else
	{
		//创建房间失败
		LLOG_ERROR("Work::handerLMGCreateDeskForClub CreateTale Failed! ErrorCode: %d", nErrorCode);

		//RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
	}
}


void Work::handerLMGCreateClubDeskAndEnter(LMsgLMG2LCreateClubDeskAndEnter*msg)
{
	LLOG_DEBUG("Work::handerLMGCreateClubDeskAndEnter");
	if (msg==NULL)
	{
		return;
	}
	LLOG_DEBUG("handerLMGCreateClubDeskAndEnter  user[%d] club[%d]  coins[%d]",msg->m_joinUserid,msg->m_clubId,msg->m_joinUserUsert.m_coins);

	//加入用户
	User* pJoinUser=gUserManager.GetUserbyDataId(msg->m_joinUserid);
	if (pJoinUser && pJoinUser->GetDesk())
	{
		//用户创建过桌子，先解散再进入
		LLOG_ERROR("Work::handerLMGCreateClubDeskAndEnter   pJoinUser [%d] already create desk", msg->m_joinUserid);
		LMsgS2CAddDeskRet ret;
		ret.m_deskId=msg->m_deskId;
		ret.m_errorCode=5;

		pJoinUser->m_userData.m_coins = msg->m_joinUserUsert.m_coins;
		pJoinUser->m_isMedal = msg->m_isMedal;
		pJoinUser->Send(ret);
		//回收桌子号
		RecycleDeskId(msg->m_deskId, msg->m_clubId, msg->m_playTypeId, msg->m_clubDeskId);

		return;
		//pUser->m_userData.m_numOfCard2s=msg->m_usert.m_numOfCard2s;
		//pUser->m_userData.m_customString1=msg->m_usert.m_customString1;
		//pUser->m_userData.m_customString2=msg->m_usert.m_customString2;
		////找到玩家了， 判断是否有桌子，有就加入桌子
		//HanderDeskUserLogin(pUser, msg->m_gateId, msg->m_ip);
		//RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
	}
	else
	{
		if (pJoinUser)         // 这里需要这么处理吗？？？？？ 现在还不确定
		{
			// 把之前可能存在的玩家删掉
			UserManager::Instance().DelUser(pJoinUser);
			delete pJoinUser;
			pJoinUser=NULL;
		}
		//没有的话，覆盖数据，创建桌子
		pJoinUser=new User(msg->m_joinUserUsert, msg->m_joinUserGateId);
		if (pJoinUser)
		{
			pJoinUser->m_isMedal = msg->m_isMedal;
			pJoinUser->SetIp(msg->m_joinUserIp);
			UserManager::Instance().AddUser(pJoinUser);
		}
		else
		{
			LLOG_ERROR("Work::handerLMGCreateClubDeskAndEnter  error new pJoinUser");
			//回收桌子号
			RecycleDeskId(msg->m_deskId, msg->m_clubId, msg->m_playTypeId, msg->m_clubDeskId);
			return;
		}
	}

	//创建桌子
	Lint nErrorCode=gRoomVip.CreateVipDeskForClub(msg, pJoinUser);
	if (nErrorCode==0)
	{
		//创建成功
	}
	else
	{
		//创建房间失败
		LLOG_ERROR("Work::handerLMGCreateClubDeskAndEnter CreateTale Failed! ErrorCode: %d", nErrorCode);
		//用户加入桌子失败
		LMsgS2CAddDeskRet ret;
		ret.m_deskId=msg->m_deskId;
		ret.m_errorCode=8;
		pJoinUser->Send(ret);

		//回收桌子号
		RecycleDeskId(msg->m_deskId, msg->m_clubId, msg->m_playTypeId, msg->m_clubDeskId);
		//RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
	}

}

void Work::handerLMGEnterClubDesk(LMsgLMG2LEnterClubDesk*msg)
{
	LLOG_DEBUG("Work::handerLMGEnterClubDesk");
	if (msg==NULL)
	{
		return;
	}

	User* pJoinUser=gUserManager.GetUserbyDataId(msg->m_joinUserid);
	if (pJoinUser && pJoinUser->GetDesk())
	{
		//不做处理，这个时候不应该在这里
		LLOG_ERROR("Work::handerLMGEnterClubDesk   pJoinUser && pJoinUser->GetDesk()  userId[%d] have desk[%d]", pJoinUser->GetUserDataId(), pJoinUser->GetDesk()->GetDeskId());
		//LMsgS2CAddDeskRet ret;
		//ret.m_deskId=msg->m_deskId;
		//ret.m_errorCode=5;
		//pJoinUser->Send(ret);
		//return;

		pJoinUser->m_userData.m_numOfCard2s=msg->m_joinUserUsert.m_numOfCard2s;
		pJoinUser->m_userData.m_numOfCard2s=msg->m_joinUserUsert.m_numOfCard2s;
		pJoinUser->m_userData.m_customString1=msg->m_joinUserUsert.m_customString1;
		pJoinUser->m_userData.m_customString2=msg->m_joinUserUsert.m_customString2;
		pJoinUser->m_isMedal = msg->m_isMedal;
		//找到玩家了， 判断是否有桌子，有就加入桌子
		HanderDeskUserLogin(pJoinUser, msg->m_joinUserGateId, msg->m_joinUserIp,msg->m_joinUserUsert.m_unioid);
	}
	else
	{
		if (pJoinUser)
		{
			// 把之前可能存在的玩家删掉
			UserManager::Instance().DelUser(pJoinUser);
			delete pJoinUser;
			pJoinUser=NULL;
		}
		pJoinUser=new User(msg->m_joinUserUsert, msg->m_joinUserGateId);
		if (pJoinUser)
		{
			pJoinUser->m_isMedal = msg->m_isMedal;
			pJoinUser->SetIp(msg->m_joinUserIp);
			UserManager::Instance().AddUser(pJoinUser);
			pJoinUser->Login();
			Lint nError=pJoinUser->HanderUserAddClubDesk(msg);
			if (nError==0)
			{
				//加入房间成功
				pJoinUser->setUserState(LGU_STATE_DESK);
				pJoinUser->ModifyUserState();
			}
			else
			{
				if (nError == -1)
					return;
				//加入房间失败
				LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed! ErrorCode: %d userId=[%d]", nError,msg->m_joinUserid);
				HanderUserLogout(msg->m_joinUserGateId, msg->m_joinUserStrUUID);
				if (nError == 2)
				{
					//回收桌子号
					LLOG_ERROR("handerLMGEnterClubDesk error ,RecycleDeskId desk clubId[%d],playId[%d],clubDeskId[%d],deskId[%d],userId[%d]", msg->m_clubId, msg->m_playTypeId, msg->m_clubDeskId, msg->m_deskId,msg->m_joinUserid);
					RecycleDeskId(msg->m_deskId, msg->m_clubId, msg->m_playTypeId, msg->m_clubDeskId);
				}
			}
		}
		else
		{
			//加入房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed No User! userId[%d]",msg->m_joinUserid);
			HanderUserLogout(msg->m_joinUserGateId, msg->m_joinUserStrUUID);
		}
	}

}


//和manager断线重连后，向manager同步自己信息
void  Work::_SynchroData2Manager()
{
	LLOG_DEBUG("Work::_SynchroData2Manager");
	//m_needSynchroData2Manager=false;     //只发送一次

	LMsgL2LMGSynchroData send;
	send.m_logicID=gConfig.GetServerID();

	gUserManager.SynchroUserData(send.m_info);
	send.m_count=send.m_info.size();

	gRoomVip.SynchroDeskData(send.m_deskInfo);
	send.m_deskCount=send.m_deskInfo.size();

	SendToLogicManager(send);

}

void  Work::handerLMGQueryGPSInfo(MHLMsgLMG2LQueryRoomGPSInfo * msg)
{
	LLOG_ERROR("Work::handerLMGQueryGPSInfo query desk GPS information. desk:%d", msg->m_deskID);
	//创建临时用户,发送消息
	User user(msg->m_usert, msg->m_gateId);
	Desk * pDesk=gRoomVip.GetDeskById(msg->m_deskID);
	MHLMsgS2CQueryRoomGPSLimitInfoRet ret;
	ret.m_error_code=0;
	if (NULL==pDesk)
	{
		ret.m_error_code=1;
		LLOG_ERROR("Work::handerLMGQueryGPSInfo can not find desk id:%d", msg->m_deskID);
	}
	else
	{
		ret.m_error_code=0;
		ret.m_gps_limit=pDesk->m_Gps_Limit;
	}
	user.Send(ret);
}

//俱乐部会长强制解散房间
void Work::handerLMGDisMissClubDesk(LMsgLMG2LDissmissClubDesk* msg)
{
	LLOG_DEBUG("Work::handerLMGDisMissClubDesk delete desk!");
	if (msg == NULL)
	{
		LLOG_ERROR("Work::handerLMGDisMissClubDesk() Error!!! msg is NULL Error!!!");
		return;
	}

	LMsgS2CClubMasterResetRoom send;
	send.m_errorCode = 0;
	send.m_clubId = msg->m_clubId;
	send.m_playTypeId = msg->m_playTypeId;
	send.m_clubDeskId = msg->m_clubDeskId;
	send.m_showDeskId = msg->m_showDeskId;
	send.m_strErrorDes = "解散房间成功";

	User* pUser = new User(msg->m_usert, msg->m_gateId);

	Desk* m_pDesk = gRoomVip.GetDeskById(msg->m_deskId);
	// 桌子不存在
	if (!m_pDesk) 
	{
		LLOG_ERROR("Work::handerLMGDisMissClubDesk find desk  Failed! deskId =  %d ", msg->m_deskId);
		send.m_errorCode = 2;
		send.m_strErrorDes = "room not exist";
		if (pUser)
		{
			//发送玩家桌子数据
			pUser->Send(send);
			delete pUser;
		}
		return;
	}
	// 桌子状态为free
	if (m_pDesk != NULL && m_pDesk->getDeskState() == DESK_FREE)
	{
		LLOG_ERROR("Work::handerLMGDisMissClubDesk, userid=%d clubId = [%d] deskid=%d deskstate=%d", msg->m_userid, msg->m_clubId, m_pDesk->GetDeskId(), m_pDesk->getDeskState());
		send.m_errorCode = 2;
		send.m_strErrorDes = "room is free";
		if (pUser)
		{
			//发送玩家桌子数据
			pUser->Send(send);
			delete pUser;
		}
		return;
	}

	//Lint temp = m_pDesk->getDeskState();
	//if (m_pDesk->getDeskState() == DESK_PLAY || (m_pDesk->m_vip != NULL  && m_pDesk->m_vip->m_curCircle>1) || m_pDesk->m_selectSeat == false)
	//{
	//	send.m_errorCode = 2;
	//	send.m_strErrorDes = "房间已经开局";
	//
	//	if (pUser)
	//	{
	//		//发送玩家桌子数据
	//		pUser->Send(send);
	//		delete pUser;
	//	}
	//	return;
	//
	//}

	if (m_pDesk != NULL && (m_pDesk->getDeskState() == DESK_WAIT || m_pDesk->getDeskState() == DESK_PLAY))
	{
		LLOG_ERROR("Work::DisMissCreatedDesk, userid=%d clubId=[%d],showDeskId=[%d],deskid=%d deskstate=%d", msg->m_userid, msg->m_clubId,msg->m_showDeskId,m_pDesk->GetDeskId(), m_pDesk->getDeskState());
		
		//会长强制解散弹窗提示玩家
		LMsgS2CResetDesk send;
		send.m_errorCode = 5;  //俱乐部会长强制解散房间
		if (pUser)
		{
			send.m_applay = pUser->m_userData.m_nike;
			send.m_userId = pUser->m_userData.m_id;
		}
		//send.m_isShowFee = (m_pDesk->m_vip && m_pDesk->m_vip->m_curCircle == 0) ? 1 : 0;
		m_pDesk->MHBoadCastDeskSeatUser(send);
		m_pDesk->MHBoadCastDeskLookonUser(send);

		m_pDesk->m_resetUserId = send.m_userId;
		if (m_pDesk->m_vip)
		{
			m_pDesk->m_vip->m_reset = 1;
		}

		if (m_pDesk->getDeskState() == DESK_WAIT && m_pDesk->m_vip != NULL  && !m_pDesk->m_vip->IsBegin())
		{
			m_pDesk->m_dismissUser = true;
			//m_pDesk->BoadCast(send);
			//m_pDesk->MHBoadCastAllDeskUser(send);
			bool bDissmiss = m_pDesk->DissmissAllplayer();
		}
		else
		{
			if (m_pDesk)m_pDesk->MHDismissDeskOnPlay();
		}
	}

	if (pUser)
	{
		//发送玩家桌子数据
		pUser->Send(send);
		delete pUser;
	}

}

void Work::handerLMGAPIDisMissRoom(LMsgLMG2LAPIDissmissRoom*msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("handerLMGAPIDisMissRoom  deskId=[%d]",msg->m_deskId);
	
	Desk* m_pDesk = gRoomVip.GetDeskById(msg->m_deskId);
	// 桌子不存在
	if (!m_pDesk) {
		LLOG_ERROR("handerLMGAPIDisMissRoom find desk  Failed! deskId =  %d ", msg->m_deskId);
		return;
	}
	// 桌子状态为free
	if (m_pDesk != NULL && m_pDesk->getDeskState() == DESK_FREE)
	{
		LLOG_ERROR("handerLMGAPIDisMissRoom, error . deskid=%d deskstate=%d", m_pDesk->GetDeskId(), m_pDesk->getDeskState());
		return;
	}

	if (m_pDesk != NULL && (m_pDesk->getDeskState() == DESK_WAIT || m_pDesk->getDeskState() == DESK_PLAY))
	{
		LLOG_ERROR("handerLMGAPIDisMissRoom,deskid=%d deskstate=%d", m_pDesk->GetDeskId(), m_pDesk->getDeskState());
		if (m_pDesk->getDeskState() == DESK_WAIT && m_pDesk->m_vip != NULL && !m_pDesk->m_vip->IsBegin())
		{
			m_pDesk->m_dismissUser = true;
			bool bDissmiss = m_pDesk->DissmissAllplayer();
		}
		else
		{
			if (m_pDesk)m_pDesk->MHDismissDeskOnPlay();
		}
	}

}