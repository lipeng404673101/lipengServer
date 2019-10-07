#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "DbServer.h"
#include "DbServerManager.h"
#include "UserMessageMsg.h"
#include "ActiveManager.h"
#include "mhmsghead.h"
#include "BcastMessage.h"

#ifdef WIN32
#include "jsoncpp/json.h"
#else
#include "json/json.h"
#endif

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicDB");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	//LMsgFactory::Instance().Init();
	
	MH_InitFactoryMessage_ForMapMessage();
	MHInitFactoryMessage();

	if(!m_dbsession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		LLOG_ERROR("m_dbsession.Init error");
		return false;
	}

	if (!m_dbsessionBK.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort()))
	{
		LLOG_ERROR("m_dbsessionBK.Init error");
		return false;
	}

	if ( !gActiveManager.Init() )
	{
		LLOG_ERROR("gActiveManager.Init error");
		return false;
	}

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	_initTickTime();

	// 加载Redis数据库配置
	{
		Lint  db_mode = 0;
		m_bDBMode = true;
		bool ret = gActiveManager.GetRedisClient().readDataByString("mj-db-src", db_mode);
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
			LLOG_ERROR("***LogicDB use master database source host %s", gConfig.GetDbHost().c_str());
		else
			LLOG_ERROR("***LogicDB use backup database source %s", gConfig.GetBKDbHost().c_str());
	}

	if (!gDbServerManager.Init())
	{
		LLOG_ERROR("gDbServerManager.Init() error");
		return false;
	}

	gBcastMessage.Init();

	return true;
}

bool Work::Final()
{
	gActiveManager.Final();

	return true;
}

//启动
void Work::Start()
{
	_loadFriendship();

	m_memeoryRecycle.Start();

	gUserMessageMsg.Start();

	gInsideNet.Start();

	gDbServerManager.Start();

	LRunnable::Start();
}

//等待
void Work::Join()
{
	m_memeoryRecycle.Join();

	gUserMessageMsg.Join();

	gInsideNet.Join();

	gDbServerManager.Join();

	//等待逻辑线程终止
	LRunnable::Join();

}

//停止
void Work::Stop()
{
	gInsideNet.Stop();

	gDbServerManager.Stop();

	gUserMessageMsg.Stop();

	m_memeoryRecycle.Stop();

	LRunnable::Stop();
}

void Work::Clear()
{

}

void Work::Push( void* msg )
{
	if(msg == NULL)
	{
		return;
	}

	switch(((LMsg*)msg)->m_msgId)
	{
	case MSG_L_2_LDB_VIDEO:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBReqVideo*)msg)->m_userId, (LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_VIP_LOG:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LBDReqVipLog*)msg)->m_userId, (LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_ROOM_SAVE:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBSaveLogItem*)msg)->m_userId,(LMsg*)msg);
			break;
		} 
	case MSG_L_2_LDB_VIDEO_SAVE:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBSaveLogItem*)msg)->m_userId,(LMsg*)msg);
			break;
		}

	case MSG_L_2_LDB_ROOM_LOG:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBReqRoomLog*)msg)->m_userId, (LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_ENDCRE_LOG:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBSaveCRELog*)msg)->m_id[0],(LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_CRE_LOG:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LBDReqCRELog*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_REQ_DELGTU:  
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBDEL_GTU*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_LMG_2_LDB_USER_LOGIN:  
		{
			gUserMessageMsg.handlerMessage(((LMsgLMG2LdbUserLogin*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_C_2_S_ACTIVITY_WRITE_PHONE:  
		{
			gUserMessageMsg.handlerMessage(((LMsgC2SActivityPhone*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_C_2_S_ACTIVITY_REQUEST_LOG:  
		{
			gUserMessageMsg.handlerMessage(((LMsgC2SActivityRequestLog*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN:  
		{
			gUserMessageMsg.handlerMessage(((LMsgC2SActivityRequestDrawOpen*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW:  
		{
			gUserMessageMsg.handlerMessage(((LMsgC2SActivityRequestDraw*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_C_2_S_ACTIVITY_REQUEST_SHARE:
		{
			gUserMessageMsg.handlerMessage(((LMsgC2SActivityRequestShare*)msg)->m_userId,(LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_GETSHAREVIDEOID:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBGGetShareVideoId*)msg)->m_userId, (LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_GETSHAREVIDEO:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBGetShareVideo*)msg)->m_userId, (LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBGetSsharedVideo*)msg)->m_userId, (LMsg*)msg);
			break;
		}
	case MSG_L_2_LDB_SAVESHAREVIDEOID:
		{
			gUserMessageMsg.handlerMessage(((LMsgL2LDBSaveShareVideoId*)msg)->m_userId, (LMsg*)msg);
			break;
		}

	case MSG_L_2_LDB_GETUSERPLAYDATA:
	{
		gUserMessageMsg.handlerMessage(((LMsgL2LDBGetUserPlayData*)msg)->m_userId, (LMsg*)msg);
		break;
	}

	case MSG_L_2_LDB_UPDATE_PLAYER_SCORE:
	{
		gUserMessageMsg.handlerMessage(((LMsgL2LDBUpdatePlayerScore*)msg)->m_userId, (LMsg*)msg);
		break;
	}
	case MH_MSG_L_2_LDB_QUERY_SHARE_VIDEO_BY_SHARE_ID:
	{
		gUserMessageMsg.handlerMessage(((MHLMsgL2LDBQueryShareVideoByShareId*)msg)->m_userId, (LMsg*)msg);
		break;
	}	
	default:
		LRunnable::Push(msg);
		break;
	}
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
	// 检查数据库切换是否发生
	if (gActiveManager.GetRedisClient().initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		int db_mode = 0;
		bool bChanged = false;
		if (gActiveManager.GetRedisClient().readDataByString("mj-db-src", db_mode))
		{
			if (0 == db_mode && (m_bDBMode == false))
			{
				bChanged = true;
				m_bDBMode = true; // 主库
			}
			if ((1 == db_mode) && m_bDBMode)
			{
				bChanged = true;
				m_bDBMode = false; // 备用库
			}
		}
		if (bChanged)
		{
			LLOG_ERROR("***LogicDB  database source changed");			
			if (m_bDBMode)
				LLOG_ERROR("***LogicDB use master database source host %s", gConfig.GetDbHost().c_str());
			else
				LLOG_ERROR("***LogicDB use backup database source %s", gConfig.GetBKDbHost().c_str());
			gBcastMessage.Init();
		}
	}

	//2分钟循环一次
	if (cur.MSecs() - m_200SceTick >= 120000)  //2*60*1000 = 120000
	{
		m_200SceTick = cur.MSecs();
		//删除LOG
		gBcastMessage.LoadMessageDataFromDB();
		Lint num = gBcastMessage.GetMessageListNum();
		if (num == 0)
			return;
		LMsgLDB2LMBCast tempMessage;
		gBcastMessage.produceBcastMessage(tempMessage);
		LLOG_DEBUG("send boadcast message");
		SendToLogicManager(tempMessage);

	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 300000) //2*60*1000 = 300000
	{
		m_600SceTick = cur.MSecs();
		//删除LOG
		//if (gWork.GetCurTime().GetHour() == 5 && (gWork.GetCurTime().GetMin() >= 0 && gWork.GetCurTime().GetMin() < 5) )
		//{
			//2017-12-16 不再清除过期的LOG全部保留
			//_clearOldLogVideo();		//5分钟内，必定执行一次，也只执行一次。
		//}
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
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;

	case MSG_LMG_2_LDB_LOGIN:
		HanderLogicManagerLogin((LMsgLMG2LdbLogin*)msg);
		break;
	case MSG_L_2_LDB_LOGIN:
		HanderLogicServerLogin((LMsgL2LdbLogin*)msg);
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;

	case MSG_C_2_S_GET_INVITING_INFO:
		HanderGetInvitingInfo((LMsgC2SGetInvitingInfo*)msg);
		break;
	case MSG_LM_2_LDB_BIND_INVITER:
		HanderBindInviterMsg((LMsgLM_2_LDBBindInviter*)msg);
		break;
	case MSG_LM_2_LDB_REQ_INFO:
		HanderReqInviterInfo((LMsgLM_2_LDB_ReqInfo*) msg);
		break;

	case MSG_CE_2_L_SET_ACTIVITY:
		HanderLogicSetActivity((LMsgCE2LSetActivity*) msg);
		break;

	default:
		LLOG_DEBUG("Work::HanderMsg not handle, msgid=%d", msg->m_msgId);
		break;
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if ( msg->m_sp == m_logicManager )
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		m_logicManager.reset();
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	
}

void Work::HanderLogicManagerLogin(LMsgLMG2LdbLogin* msg)
{
	LLOG_ERROR("Logic Manager login");
	boost::mutex::scoped_lock l(m_mutexLogicManager);
	m_logicManager = msg->m_sp;
}

void Work::HanderLogicServerLogin(LMsgL2LdbLogin* msg)
{
	if(NULL == msg)
	{
		return;
	}

	LLOG_ERROR("Logic Server(%d) login", msg->m_serverID);
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
	msgReply.m_fromWho = HEARBEAT_WHO_LOGICDB;
	msgReply.m_iServerId = 0;

	msg->m_sp->Send(msgReply.GetSendBuff());
}

//获取邀请信息
void Work::HanderGetInvitingInfo( LMsgC2SGetInvitingInfo* msg )
{
	LLOG_DEBUG("Work::HandleReqBindingRelationships");
	
	LMsgLDB2LMGBindingRelationships ret;
	ret.m_strUUID= msg->m_strUUID;
	ret.m_userId = msg->m_userId;
	ret.m_inviterId = -1;
	ret.m_finished = 0;
	ret.m_inviteeCount = 0;

	MYSQL* m = m_dbsession.GetMysql();
	if (m != NULL)
	{
		// read inviter
		std::stringstream ss;
		ss << "SELECT Inviter, TaskFinished FROM bindingrelationships WHERE Invitee='" << msg->m_userId << "' limit 1";
		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			MYSQL_RES* res = mysql_store_result(m);

			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				if (row)
				{
					ret.m_inviterId = atoi(*row++);
					ret.m_finished = atoi(*row++);
				}
				mysql_free_result(res);
			}

			// read invitee
			ss.str("");
			ss << "SELECT Invitee, TaskFinished FROM bindingrelationships WHERE Inviter='" << msg->m_userId << "' limit 40";

			if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
			{
				MYSQL_RES* res = mysql_store_result(m);
				if (res)
				{
					MYSQL_ROW row = mysql_fetch_row(res);

					while (row)
					{
						ret.m_invitees[ret.m_inviteeCount] = atoi(*row++);
						ret.m_taskFinished[ret.m_inviteeCount] = atoi(*row++);
						ret.m_inviteeCount++;
						row = mysql_fetch_row(res);
					}
					mysql_free_result(res);
				}
			}
		}
	}
	else
		LLOG_ERROR("Work::HandleReqTaskProgress mysql null");
		
	msg->m_sp->Send(ret.GetSendBuff());
}

//绑定好友关系
void Work::HanderBindInviterMsg( LMsgLM_2_LDBBindInviter* msg )
{
	Lint inviterId = msg->m_inviterId;
	Lint userId = msg->m_userId;

	MYSQL* m = m_dbsession.GetMysql();
	if (m != NULL)
	{
		std::stringstream ss;
		ss << "SELECT count(*) FROM bindingrelationships WHERE Invitee='" << userId << "'";

		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			MYSQL_RES* res = mysql_store_result(m);

			Lint result_count = -1;
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				if (row)
				{
					result_count = atoi(*row++);
				}
				mysql_free_result(res);
			}

			if(result_count <= 0)
			{
				ShipInfo info(inviterId,0);
		
				m_user2recommIdMap.insert(std::make_pair(userId,info));//inviterId));   //newbody 2 inviter

				ss.str("");
				ss << "INSERT INTO bindingrelationships (Invitee, Inviter, BindingTime, TaskFinished) VALUES (";
				ss << userId << ",";
				ss << inviterId << ",";
				ss << gWork.GetCurTime().Secs() << ",";
				ss << 0 << ")";

				Lstring* sql = new Lstring(ss.str());
				gDbServerManager.Push(sql);
			}
			else
			{
				LLOG_ERROR("Work::HanderBindInviterMsg user(%d) has been invited!!!",msg->m_userId);
			}
		}
	}
}

//user login request inviter id or delete task
void Work::HanderReqInviterInfo( LMsgLM_2_LDB_ReqInfo* msg )
{
	LLOG_DEBUG("Work::HanderReqInviterInfo request inviter info.");

	Lint code = msg->m_reqCode;
	if(code == LMsgLM_2_LDB_ReqInfo::ReqInviterInfo)
	{
		LMsgLDB_2_LM_RetInfo ret;
		auto it = m_user2recommIdMap.find(msg->m_userId);
		if(it != m_user2recommIdMap.end())
		{
			ret.m_userId = msg->m_userId;
			ret.m_strUUID= msg->m_strUUID;
			ret.m_inviterId = (*it).second.m_inviter;
			ret.m_finished = (*it).second.m_count;

			msg->m_sp->Send(ret.GetSendBuff());
		}
		else
		{
			LLOG_DEBUG("Work::HanderReqInviterInfo userid = %d task has finished!!! ",msg->m_userId);
		}
	}
	else if(code == LMsgLM_2_LDB_ReqInfo::TaskFinished)
	{
		LLOG_DEBUG("Work::HanderReqInviterInfo userid = %d task finished!!!",msg->m_userId);
		Lint count = msg->m_param;
		if(count > 0)
		{
			auto it = m_user2recommIdMap.find(msg->m_userId);
			if(it != m_user2recommIdMap.end())
			{
				if(count >= 8)
				{
					m_user2recommIdMap.erase(it);
				}
				else
				{
					(*it).second.m_count = count;
				}

				std::stringstream ss;
				ss << "UPDATE bindingrelationships  SET TaskFinished = ";
				ss << count;
				ss << " WHERE Invitee = ";
				ss << msg->m_userId ;

				Lstring* sql = new Lstring(ss.str());
				gDbServerManager.Push(sql);
			}
		}
	}
	else if(code == LMsgLM_2_LDB_ReqInfo::DeleteUser)
	{   
		Lint userId = msg->m_param;
		//清空无效用户
		auto it = m_user2recommIdMap.find(userId);
		if(it != m_user2recommIdMap.end())
		{
			m_user2recommIdMap.erase(it);
		}

		std::stringstream ss;
		ss << "DELETE FROM bindingrelationships  WHERE  Invitee = ";
		ss << userId;
		ss << " OR Inviter = ";
		ss << userId;

		Lstring* sql = new Lstring(ss.str());
		gDbServerManager.Push(sql);
	}
}

void Work::HanderLogicSetActivity(LMsgCE2LSetActivity* msg)
{
	if (!msg)
	{
		return;
	}
	if ( msg->m_activityId == ActivityID_Draw )
	{
		SetDrawActive(msg->m_strActivity,msg->m_activityId);
	}
	else if ( msg->m_activityId == ActivityID_Share )
	{
		SetShareActive(msg->m_strActivity,msg->m_activityId);
	}
}

void Work::SetDrawActive(const Lstring& strSet,int id)
{
	gActiveManager.SetDrawActiveConfig(strSet,id);
}

void Work::SetShareActive(const Lstring& strSet,int id)
{
	gActiveManager.UpdateShareActiveSet(strSet,id);
}

void Work::SendToLogicManager(LMsg& msg)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);
	if(m_logicManager)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
}

void Work::SendToUser( const Lstring& uuid, LMsg& msg )
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);
	if(m_logicManager)
	{
		LMsgLDB2LMGUserMsg send;
		send.m_strUUID = uuid;
		send.m_dataBuff = msg.GetSendBuff();
		m_logicManager->Send(send.GetSendBuff());
	}
}

void Work::_initTickTime()
{
	LTime cur;
	m_200SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();
}

void Work::_clearOldLogVideo()
{
	static Lint period = 7 * 24 * 60 * 60;
	//加载所有的玩家
	std::stringstream ss;	
	ss << "delete from video where time < '"; 
	ss << gWork.GetCurTime().Secs() - period << "'";

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql);

	ss.str("");
	ss << "delete from log where time < '"; 
	ss << gWork.GetCurTime().Secs() - period << "'";

	sql = new Lstring(ss.str());
	gDbServerManager.Push(sql);

	// 删除 drawinfo
	ss.str("");
	ss << "delete from drawinfo where UNIX_TIMESTAMP(ctime) < '";
	ss << gWork.GetCurTime().Secs() - period  << "'";
	sql = new Lstring(ss.str());
	gDbServerManager.Push(sql);


	// 删除 playinfo
	ss.str("");
	ss << "delete from playinfo where UNIX_TIMESTAMP(ctime) < '";
	ss << gWork.GetCurTime().Secs() - period << "'";
	sql = new Lstring(ss.str());
	gDbServerManager.Push(sql);


	// 金币场视频
	ss.str("");
	ss << "delete from videocoins where time < '"; 
	ss << gWork.GetCurTime().Secs() - 24*60*60 << "'"; 

	sql = new Lstring(ss.str());
	gDbServerManager.Push(sql);

	ss.str("");
	ss << "delete from logcoins where time < '"; 
	ss << gWork.GetCurTime().Secs() - 24*60*60 << "'"; 

	sql = new Lstring(ss.str());
	gDbServerManager.Push(sql);
	LLOG_ERROR("Work::ChearOldLogVideo");
}

void Work::_loadFriendship()
{
	MYSQL* m = m_dbsession.GetMysql();
	if (m != NULL)
	{
		// read inviter
		std::stringstream ss;
		ss << "SELECT Invitee, Inviter,TaskFinished FROM bindingrelationships WHERE TaskFinished < 8";    //只加载未完成任务的
		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			MYSQL_RES* res = mysql_store_result(m);
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				while (row)
				{
					Lint inviteeId = atoi(*row++);
					Lint inviterId = atoi(*row++);
					Lint count = atoi(*row++);
					ShipInfo info(inviterId,count);

					m_user2recommIdMap.insert(std::make_pair(inviteeId,info));//inviterId));   //newbody 2 inviter
					row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
		}
	}
	else
	{
		LLOG_ERROR("Work::LoadFriendship mysql null");
	}
}