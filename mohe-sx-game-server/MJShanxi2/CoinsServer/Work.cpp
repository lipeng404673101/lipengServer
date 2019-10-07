#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "UserMessageMsg.h"
#include "DeskManager.h"
#include "UserManager.h"
#include "RobotManager.h"
#include "mhmsghead.h"

Work::Work()
{
	LTime cur;
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();
}

Work::~Work()
{

}

//初始化
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看
	LLog::Instance().SetFileName("CoinsServer");

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

	//初始化机器人
	if(gConfig.GetDebugModel() && !gRobotManager.Init())
	{
		LLOG_ERROR("gRobotManager.Init() error");
		return false;
	}

	//内部端口初始化
	if(!gInsideNet.Init())
	{
		LLOG_ERROR("Fail to init inside ip and port");
		return false;
	}

	_initTickTime();

	return true;
}

bool Work::Final()
{
	return true;
}

void Work::Push( void* msg )
{
	if(msg == NULL)
	{
		return;
	}

	switch(((LMsg*)msg)->m_msgId)
	{
	case MSG_G_2_L_USER_MSG:
		{
			LMsgG2LUserMsg* pUserMsg = (LMsgG2LUserMsg*)msg;
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
	case MSG_LMG_2_CN_USER_LOGIN:
		{
			LMsgLMG2CNUserLogin* pLoginMsg = (LMsgLMG2CNUserLogin*)msg;
			if(pLoginMsg)
			{
				gUserMessageMsg.handlerMessage(pLoginMsg->m_strUUID, pLoginMsg);
			}
			break;
		}
	case MSG_LMG_2_CN_ENTER_COIN_DESK:
		{
			LMsgLMG2CNEnterCoinDesk* pLoginMsg = (LMsgLMG2CNEnterCoinDesk*)msg;
			if(pLoginMsg)
			{
				gUserMessageMsg.handlerMessage(pLoginMsg->m_strUUID, pLoginMsg);
			}
			break;
		}
	case MSG_LMG_2_CN_GET_COINS:
		{
			LMsgLMG2CNGetCoins* pGetCoins = (LMsgLMG2CNGetCoins*)msg;
			if(pGetCoins)
			{
				gUserMessageMsg.handlerMessage(pGetCoins->m_strUUID, pGetCoins);
			}
			break;
		}
	case MSG_L_2_LMG_MODIFY_USER_STATE:
		{
			LMsgL2LMGModifyUserState* pStateMsg = (LMsgL2LMGModifyUserState*)msg;
			if(pStateMsg)
			{
				gUserMessageMsg.handlerMessage(pStateMsg->m_strUUID, pStateMsg);
			}
			break;
		}
		
	default:
		LRunnable::Push(msg);
		break;
	}
}

//启动
void Work::Start()
{
	_connectLogicManager();

	m_memeoryRecycle.Start();

	gUserMessageMsg.Start();

	gDeskManager.Start();

	//主线程开启
	LRunnable::Start();

	//内部网络开启
	gInsideNet.Start();
}

//等待
void Work::Join()
{
	//等待网络线程终止
	gInsideNet.Join();

	gUserMessageMsg.Join();

	gDeskManager.Join();

	m_memeoryRecycle.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	m_gateInfo.clear();

	gInsideNet.Stop();

	gUserMessageMsg.Stop();

	gDeskManager.Stop();

	LRunnable::Stop();

	m_memeoryRecycle.Stop();
}

void Work::Clear()
{

}

void Work::Run()
{
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
			continue;
		}
		else
		{
			HanderMsg(msg);
			m_memeoryRecycle.recycleMsg(msg);
		}
	}
}

void Work::Tick(LTime& cur)
{
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		m_1SceTick = cur.MSecs();

		if ( m_msgRecv->empty() )	// 优先处理消息在分配AI
		{
			gRobotManager.Tick();
		}
	}

	//5秒循环一次
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();

		_checkLogicManagerHeartBeat();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();

		// 重新读取配置
		gConfig.Init();
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;
	case MSG_HEARDBEAT_REPLY:
		HanderHearBeatReply((LMsgHeartBeatReplyMsg*)msg);
		break;
	case MSG_LMG_2_CN_LOGIN:
		HanderCoinsLogin((LMsgLMG2CNLogin*)msg);
		break;
	case MSG_LMG_2_CN_FREE_DESK_REPLY:
		HanderFreeDeskReply((LMsgLMG2CNFreeDeskReply*)msg);
		break;
	case MSG_G_2_CN_LOGIN:
		HanderGateLogin((LMsgG2CNLogin*)msg);
		break;
	case MSG_L_2_CN_LOGIN:
		HanderLogicLogin((LMsgL2CNLogin*)msg);
		break;
	case MSG_L_2_CN_CREATE_COIN_DESK_RET:
		HanderCreateCoinDeskRet((LMsgL2CNCreateCoinDeskRet*)msg);
		break;
	case MSG_L_2_CN_END_COIN_DESK:
		HanderEndCoinDesk((LMsgL2CNEndCoinDesk*)msg);
		break;
	default:
		{
			LLOG_ERROR("Work::HanderMsg -- Wrong msg. msgid = %d", msg->m_msgId);
			
			break;
		}
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(msg->m_sp == m_logicManager)
	{
		LLOG_ERROR("Work::HanderUserKick -- Coins server is kicked");
		_reconnectLogicManager();
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

	m_mapReconnect.erase(msg->m_sp);

	if(msg->m_sp == m_logicManager)
	{
		// 向Manager注册
		LMsgCN2LMGLogin login;
		login.m_key = gConfig.GetCoinsKey();
		login.m_ip = gConfig.GetInsideIp();
		login.m_port = gConfig.GetInsidePort();
		login.m_deskcount = gDeskManager.GetALLDeskIDCount();
		SendToLogicManager(login);
	}
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
	msgReply.m_fromWho = HEARBEAT_WHO_COINSSERVER;
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

void Work::HanderCoinsLogin(LMsgLMG2CNLogin* msg)
{
	LLOG_ERROR("Work::HanderCoinsLogin Login Manager, result=%d", msg->m_result);
	if ( msg->m_result != 0 )
	{
		LLOG_ERROR("Work::HanderCoinsLogin fail, result=%d", msg->m_result);
		return;
	}
	// 桌子分配的数量不一致
	if ( msg->m_deskcount != gDeskManager.GetALLDeskIDCount() )
	{
		LLOG_ERROR("Work::HanderCoinsLogin desk count error, manager count=%d, this count=%d", msg->m_deskcount, gDeskManager.GetALLDeskIDCount() );

		gDeskManager.ClearDesk();
		gUserManager.ClearUsers();
	}
}

void Work::HanderFreeDeskReply(LMsgLMG2CNFreeDeskReply* msg)
{
	gDeskManager.AddFreeDeskID( (GameType)msg->m_gameType, msg->m_desk );
}

void Work::SendToLogicManager(LMsg& msg)
{
	if(m_logicManager && m_logicManager->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
} 

void Work::HanderGateLogin(LMsgG2CNLogin* msg)
{
	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

		auto itGateInfo = m_gateInfo.find(msg->m_id);
		if(itGateInfo != m_gateInfo.end()) //重连
		{
			LLOG_ERROR("Gate reconnect logic manager. Id = %d", itGateInfo->second.m_id);
			itGateInfo->second.m_sp->Stop();
			m_gateInfo.erase(itGateInfo);
		}


		LLOG_ERROR("Work::HanderGateLogin Succeed! %d", msg->m_id);

		//广播GATE消息
		GateInfo info;
		info.m_id = msg->m_id;
		info.m_ip = msg->m_ip;
		info.m_port = msg->m_port;
		info.m_userCount = 0;
		info.m_sp = msg->m_sp;
		m_gateInfo[info.m_id] = info;
	}
}

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
	}
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

Lint Work::GetGateCount()
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);
	return m_gateInfo.size();
}

void Work::SendToAllGate(LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	auto gate = m_gateInfo.begin();
	for (; gate != m_gateInfo.end(); ++gate)
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendToGate(Lint gateID, LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	auto gate = m_gateInfo.find(gateID);
	if ( gate != m_gateInfo.end())
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::HanderLogicLogin(LMsgL2CNLogin* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderLogicLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	{
		boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

		auto itLogic = m_logicServerInfo.find(msg->m_id);
		if(itLogic != m_logicServerInfo.end())
		{
			itLogic->second.m_logicInfo.m_sp->Stop();
			m_logicServerInfo.erase(itLogic);
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
		logicInfo.m_tickTime = 0;
		m_logicServerInfo[logicInfo.m_logicInfo.m_id] = logicInfo;
	}
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

void Work::HanderCreateCoinDeskRet(LMsgL2CNCreateCoinDeskRet* msg)
{
	if ( msg->m_errorCode != 0 )
	{
		// 解散这个桌子
		LLOG_ERROR("Work::HanderCreateCoinDeskRet desk create fail, deskid=%d code=%d", msg->m_deskId, msg->m_errorCode);
		gDeskManager.DismissPlayDesk( msg->m_deskId );
	}
	else
	{
		LLOG_INFO("Work::HanderCreateCoinDeskRet desk already start deskid=%d", msg->m_deskId);
	}
}

void Work::HanderEndCoinDesk(LMsgL2CNEndCoinDesk* msg)
{
	gDeskManager.GameOver( msg->m_deskId, msg->m_score, msg->m_dataResult );
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
				// gDeskManager.RecycleAllDeskIdOnLogicServer(itLogic->second.m_logicInfo.m_id); 回收桌子ID 待完善
				m_logicServerInfo.erase(itLogic++);
				bDelLogic = true;
			}
			else
			{
				itLogic++;
			}
		}
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

void Work::SendToAllLogic(LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogic = m_logicServerInfo.begin();
	for (; itLogic != m_logicServerInfo.end(); ++itLogic)
	{
		itLogic->second.m_logicInfo.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendToLogic(Lint iLogicServerId, LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogic = m_logicServerInfo.find(iLogicServerId);
	if(itLogic != m_logicServerInfo.end() && itLogic->second.m_tickTime == 0)
	{
		itLogic->second.m_logicInfo.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::_initTickTime()
{
	LTime cur;
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();
}

void Work::_connectLogicManager()
{
	m_logicManager = gInsideNet.GetNewSocket();

	LLOG_ERROR("Work::ConnectLogicManager begin %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	if(!m_logicManager->Connect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort()))
	{
		LLOG_ERROR("Fail to connect Manager");
		Stop();
	}
	else
	{
		LLOG_ERROR("Work::ConnectLogicManager end %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	}
}

void Work::_reconnectLogicManager()
{
	if(!m_logicManager)
	{
		LLOG_ERROR("Manager socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic manager...");
	m_mapReconnect.erase(m_logicManager);
	m_mapHeartBeat.erase(m_logicManager);

	m_logicManager->Stop();
	m_logicManager = gInsideNet.GetNewSocket();
	m_logicManager->AsyncConnect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort());

	m_mapReconnect[m_logicManager] = true;
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
		LLOG_ERROR("Manager network had problem. Reconnect....");
		_reconnectLogicManager();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_COINSSERVER;
	msgHeatbeat.m_iServerId = 0;

	SendToLogicManager(msgHeatbeat);

	m_mapHeartBeat[m_logicManager] = true;
}
