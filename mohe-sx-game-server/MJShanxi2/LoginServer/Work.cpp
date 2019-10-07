#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "HttpRequest.h"
#include "mhmsghead.h"

Work::Work()
{
	m_5SceTick = 0;
	m_30SceTick = 0;
}

Work::~Work()
{

}

//初始化
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看
	LLog::Instance().SetFileName("LoginServer");

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

	//内部端口初始化
	if(!gInsideNet.Init())
	{
		LLOG_ERROR("Fail to init inside ip and port");
		return false;
	}
	
	//外部端口初始化
	if(!gOutsideNet.Init())
	{
		LLOG_ERROR("Fail to init outside ip and port");
		return false;
	}

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
	_connectCenter();

	m_memeoryRecycle.Start();

	//主线程开启
	LRunnable::Start();

	//内部网络开启
	gInsideNet.Start();

	//外部网络开启
	gOutsideNet.Start();
}

//等待
void Work::Join()
{
	//等待网络线程终止
	gInsideNet.Join();
	gOutsideNet.Join();

	m_memeoryRecycle.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	gInsideNet.Stop();
	gOutsideNet.Stop();

	LRunnable::Stop();

	m_memeoryRecycle.Stop();

	m_mapUUID2ClientInfo.clear();
	m_mapSp2ClientInfo.clear();
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
	//5秒循环一次
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		//关闭不活动的socket链接
		for (auto itSp = m_mapSp2ClientInfo.begin() ; itSp != m_mapSp2ClientInfo.end(); )
		{
			if((cur.Secs() - itSp->second->time) >= 30)
			{
				itSp->first->Stop();
				m_mapUUID2ClientInfo.erase(itSp->second->strUUID);
				m_mapSp2ClientInfo.erase(itSp++);
			}
			else
			{
				++itSp;
			}
		}

		_checkCenterHeartBeat();
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
	LLOG_DEBUG("HanderMsg id=[%d]", msg->m_msgId);
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_CONVERT_CLIENTMSG:
		HanderConvertClientMsg((LMsgConvertClientMsg*)msg);
		break;
	case MSG_TO_LOGINSERVER:
		HanderToLoginServer((LMsgToLoginServer*)msg);
		break;
	case MSG_HEARDBEAT_REPLY:
		HanderHearBeatReply((LMsgHeartBeatReplyMsg*)msg);
		break;
	//case MSG_C_2_S_PHONE_LOGIN:
	//	HanderUserPhoneLogin((LMsgC2SPhoneLogin*)pPhoneLoginMsg);
	//	break;
	default:
		{
			LLOG_ERROR("Work::HanderMsg -- Wrong msg. msgid = %d", msg->m_msgId);
			if(msg->m_sp)
			{
				msg->m_sp->Stop();
				_delSp(msg->m_sp);
			}

			break;
		}
	}
}

void Work::HanderConvertClientMsg(LMsgConvertClientMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(msg->msgHead.get() == NULL || msg->msgEntity.get() == NULL)
	{
		LLOG_ERROR("The message pointer is null");

		msg->m_sp->Stop();
		_delSp(msg->m_sp);
		return;
	}

	LLOG_DEBUG("HanderConvertClientMsg m_msgId[%d] msg.msgHead.m_strUserId[%s]", msg->msgEntity->m_msgId, msg->msgHead->m_strUserId.c_str());
	if (msg->msgEntity->m_msgId == MSG_C_2_S_PHONE_LOGIN)
	{
		LMsgC2SPhoneLogin* pPhoneLoginMsg = (LMsgC2SPhoneLogin*)(msg->msgEntity.get());
		auto itClientInfo = m_mapSp2ClientInfo.find(msg->m_sp);
		if (itClientInfo == m_mapSp2ClientInfo.end())
		{
			msg->m_sp->Stop();
			return;
		}

		itClientInfo->second->strUUID = pPhoneLoginMsg->m_Phone;
		m_mapUUID2ClientInfo[itClientInfo->second->strUUID] = itClientInfo->second;
		
		HanderUserPhoneLogin(pPhoneLoginMsg);

		return;
	}

	auto itClientInfo = m_mapSp2ClientInfo.find(msg->m_sp);
	if(itClientInfo == m_mapSp2ClientInfo.end())
	{
		msg->m_sp->Stop();
		return;
	}

	itClientInfo->second->strUUID = msg->msgHead->m_strUserId;
	m_mapUUID2ClientInfo[itClientInfo->second->strUUID] = itClientInfo->second;

	//以后这里会加入策略

	if(msg->msgEntity->m_msgId != MSG_C_2_S_MSG)
	{
		//LLOG_ERROR("The message isn't login. Id = %d", msg->msgEntity->m_msgId);
		LLOG_ERROR("***The message isn't login. connect was kicked. m_msgId:%d m_strUserId: %s", msg->msgEntity->m_msgId, msg->msgHead->m_strUserId.c_str());
		msg->m_sp->Stop();
		_delSp(msg->m_sp);
		return;
	}

	LMsgC2SMsg* pLoginMsg = (LMsgC2SMsg*)(msg->msgEntity.get());
	LLOG_INFO("***Start process login. m_msgId:%d pLoginMsg.m_uuid: %s - msg.msgHead.m_strUserId: %s", pLoginMsg->m_msgId, pLoginMsg->m_uuid.c_str(), msg->msgHead->m_strUserId.c_str());
	if(pLoginMsg->m_uuid != msg->msgHead->m_strUserId)
	{
		LLOG_ERROR("UUID isn't same: %s  ---  %s", pLoginMsg->m_uuid.c_str(), msg->msgHead->m_strUserId.c_str());
		msg->m_sp->Stop();
		_delSp(msg->m_sp);
		return;
	}

	HanderUserLogin(pLoginMsg, itClientInfo->second);
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(msg->m_sp == m_socketCenter)
	{
		LLOG_ERROR("Work::HanderUserKick -- Login server is kicked");
		_reconnectCenter();
	}
	else
	{
		_delSp(msg->m_sp);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if(msg == NULL)
	{
		return;
	}

	m_mapReconnect.erase(msg->m_sp);

	if(msg->m_sp == m_socketCenter)
	{
		return;
	}

	auto itSp = m_mapSp2ClientInfo.find(msg->m_sp);
	if(itSp != m_mapSp2ClientInfo.end())
	{
		return;
	}

	LTime now;
	boost::shared_ptr<CLIENT_INFO> clientInfo(new CLIENT_INFO);
	clientInfo->sp = msg->m_sp;
	clientInfo->time = now.Secs(); 
	m_mapSp2ClientInfo[msg->m_sp] = clientInfo;
}

//处理玩家登陆的消息 
void Work::HanderUserLogin(LMsgC2SMsg* msg, boost::shared_ptr<CLIENT_INFO> clientInfo)
{
	if (msg == NULL || m_socketCenter.get() == NULL)
	{
		return;
	}

	if(msg->m_plate == "wechat") //如果是微信则需要进行MD5验证,center将不再验证
	{
		LMsgS2CMsg ret;
		ret.m_errorCode = 0;
		if(msg->m_uuid.empty() || msg->m_uuid.size()>32)
		{
			ret.m_errorCode = 7;
		}
		else
		{
			Lstring str = msg->m_openId;
			str += msg->m_accessToken;
			str += msg->m_refreshToken;
			str += msg->m_uuid;
			str += "3c6e0b8a9c15224a8228b9a98ca1531d";
			if(HttpRequest::Instance().Md5Hex(str.c_str(),str.size()) != msg->m_md5)
			{
				LLOG_DEBUG("Work::HanderUserLogin msd5 error %s : %s : %s",msg->m_openId.c_str(),msg->m_accessToken.c_str(),msg->m_md5.c_str());
				ret.m_errorCode = 7;
			}
		}
		
		if(ret.m_errorCode != 0)
		{
			//回消息
			msg->m_sp->Send(ret.GetSendBuff());
			return;
		}
	}

	msgpack::sbuffer buffer;
	msgpack::packer<msgpack::sbuffer> pac(&buffer);
	msg->Write(pac);
	
	LMsgFromLoginServer msgLogin;
	msgLogin.m_gaofang_flag = gConfig.GetLoginFlag();
	msgLogin.m_strUUID = clientInfo->strUUID;
	msgLogin.m_dataBuff.reset(new LBuff());
	msgLogin.m_dataBuff->Write(buffer.data(), buffer.size());
	
	m_socketCenter->Send(msgLogin.GetSendBuff());
}

void Work::HanderToLoginServer(LMsgToLoginServer* msg)
{
	if(msg == NULL || msg->m_dataBuff.get() == NULL)
	{
		return;
	}

	auto itClientInfo = m_mapUUID2ClientInfo.find(msg->m_strUUID);
	if(itClientInfo == m_mapUUID2ClientInfo.end() || itClientInfo->second->sp.get() == NULL)
	{
		LLOG_DEBUG("****Can not find client info  m_strUUID: %s", msg->m_strUUID.c_str());
		return;
	}
	LLOG_INFO("***Send User login successfully. m_msgId: %d m_strUUID: %s", msg->m_msgId, msg->m_strUUID.c_str());
	itClientInfo->second->sp->Send(msg->m_dataBuff);

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
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
}

void Work::_connectCenter()
{
	m_socketCenter = gInsideNet.GetNewSocket();

	LLOG_ERROR("Work::ConnectCenter begin %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	if(!m_socketCenter->Connect(gConfig.GetCenterIp(), gConfig.GetCenterPort()))
	{
		LLOG_ERROR("Fail to connect center");
		Stop();
	}
	else
	{
		LLOG_ERROR("Work::ConnectCenter end %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	}
}

void Work::_delSp(LSocketPtr sp)
{
	auto itSp2Client = m_mapSp2ClientInfo.find(sp);
	if(itSp2Client != m_mapSp2ClientInfo.end())
	{
		if(!itSp2Client->second->strUUID.empty())
		{
			m_mapUUID2ClientInfo.erase(itSp2Client->second->strUUID);
		}

		m_mapSp2ClientInfo.erase(itSp2Client);
	}
}

void Work::_reconnectCenter()
{
	if(!m_socketCenter)
	{
		LLOG_ERROR("Center socket is null");
		return;
	}

	LLOG_ERROR("Reconnect center...");
	m_mapReconnect.erase(m_socketCenter);
	m_mapHeartBeat.erase(m_socketCenter);

	m_socketCenter->Stop();
	m_socketCenter = gInsideNet.GetNewSocket();
	m_socketCenter->AsyncConnect(gConfig.GetCenterIp(), gConfig.GetCenterPort());

	m_mapReconnect[m_socketCenter] = true;
}

void Work::_checkCenterHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连LogicManager，则不进行心跳
	if(m_mapReconnect.find(m_socketCenter) != m_mapReconnect.end())
	{
		return;
	}

	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_socketCenter) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("Center network had problem. Reconnect....");
		_reconnectCenter();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGINSERVER;
	msgHeatbeat.m_iServerId = 0;

	m_socketCenter->Send(msgHeatbeat.GetSendBuff());

	m_mapHeartBeat[m_socketCenter] = true;
}

//处理玩家登陆的消息 
bool Work::HanderUserPhoneLogin(LMsgC2SPhoneLogin* msg)
{
	if (msg == NULL )
	{
		return false;
	}
	LLOG_DEBUG("HanderUserPhoneLogin type[%d] phone[%s],vcode[%s]",msg->m_type,msg->m_Phone.c_str(),msg->m_Vcode.c_str());
	bool phoneNoError = false;
	if (msg->m_type == PHONE_LOGIN_TYPE_MSG || msg->m_type == PHONE_LOGIN_TYPE_VOICE || msg->m_type == PHONE_LOGIN_TYPE_CHECK)
	{
		LMsgS2CPhoneLogin1 ret;
		ret.m_error_code = 0;
		if (msg->m_Phone.empty() || msg->m_Phone.size() < 11)
		{
			ret.m_error_code = 12;
			phoneNoError = true;
			
		}

		if ( phoneNoError && msg->m_type != PHONE_LOGIN_TYPE_CHECK)
		{
			//回消息
			msg->m_sp->Send(ret.GetSendBuff());
			return false;
		}

	}

	if (msg->m_type == PHONE_LOGIN_TYPE_CHECK) //如果是微信则需要进行MD5验证,center将不再验证
	{
		LMsgS2CPhoneLogin2 ret;
		ret.m_error_code = 0;

		if (phoneNoError)
		{
			ret.m_error_code = 13;
		}

		if (msg->m_Vcode.empty())
		{
			ret.m_error_code = 12;
		}
		else
		{
			
		}

		if (ret.m_error_code != 0)
		{
			//回消息
			msg->m_sp->Send(ret.GetSendBuff());
			return false;
		}
	}

	msgpack::sbuffer buffer;
	msgpack::packer<msgpack::sbuffer> pac(&buffer);
	msg->Write(pac);

	LMsgFromLoginServer msgLogin;
	msgLogin.m_gaofang_flag = gConfig.GetLoginFlag();
	//msgLogin.m_login_type = 1;
	//msgLogin.m_strUUID = clientInfo->strUUID;
	msgLogin.m_dataBuff.reset(new LBuff());
	msgLogin.m_dataBuff->Write(buffer.data(), buffer.size());

	m_socketCenter->Send(msgLogin.GetSendBuff());
}