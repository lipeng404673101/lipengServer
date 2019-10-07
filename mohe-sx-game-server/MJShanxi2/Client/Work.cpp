#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "AI.h"

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("ClientServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	CardManager::Instance().Init();

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	gInsideNet.Start();

	LRunnable::Start();

	ConnectCenterServer();
}

//等待
void Work::Join()
{
	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();

}

//停止
void Work::Stop()
{
	gInsideNet.Stop();

	LRunnable::Stop();
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
			delete msg;
		}
	}
}

void Work::Tick(LTime& cur)
{
	//30毫秒循环一次
	if (cur.MSecs() - m_30MSceTick > 30)
	{
		m_30MSceTick = cur.MSecs();
	}

	//200毫秒循环一次
	if (cur.MSecs() - m_200MSceTick > 200)
	{
		m_200MSceTick = cur.MSecs();
	}

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 5000)
	{
		m_1SceTick = cur.MSecs();

		if(m_state == 1)
		{
			OutCard();
		}
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
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

	case MSG_S_2_C_MSG:
		HanderServerMsg((LMsgS2CMsg*)msg);
		break;

	//////////////////////////////////////////////////////////////////////////
	case MSG_S_2_C_LOGIN:
		HanderUserLogin((LMsgS2CLogin*)msg);
		break;
	case MSG_S_2_C_ITEM_INFO:
		HanderUserItem((LMsgS2CItemInfo*) msg);
		break;
	case MSG_S_2_C_CREATE_ROOM:
		HanderUserCreateRoom((LMsgS2CCreateDeskRet*)msg);
		break;
	case MSG_S_2_C_ADD_ROOM:
		HanderUserAddRoom((LMsgS2CAddDeskRet*) msg);
		break;
	case MSG_S_2_C_INTO_DESK:
		HanderUserIntoDesk((LMsgS2CIntoDesk*) msg);
		break;
	case MSG_S_2_C_ADD_USER:
		HanderUserAddDesk((LMsgS2CDeskAddUser*) msg);
		break;
	case MSG_S_2_C_START:
		HanderUserPlayStart((LMsgS2CPlayStart*) msg);
		break;
	case MSG_S_2_C_GET_CARD:
		HanderUserGetCard((LMsgS2COutCard*) msg);
		break;
	case MSG_S_2_C_PLAY_CARD:
		HanderUserPlayerCard((LMsgS2CUserPlay*) msg);
		break;
	case MSG_S_2_C_USER_THINK:
		HanderUserThink((LMsgS2CThink*) msg);
		break;
	case MSG_S_2_C_USER_OPER:
		HanderUserOper((LMsgS2CUserOper*) msg);
		break;
	case MSG_S_2_C_GAME_OVER:
		HanderGameOver((LMsgS2CGameOver*) msg);
		break;
	case MSG_S_2_C_DESK_STATE:
		HanderReconnect((LMsgS2CDeskState*) msg);
		break;
	case MSG_S_2_C_RESET_ROOM:
		HanderReset((LMsgS2CResetDesk*) msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserLogin(LMsgS2CLogin* msg)
{
	LLOG_DEBUG("Work::HanderUserLogin %d:%s", msg->m_id, msg->m_nike.c_str());

	SendCreateRoom();

	//LMsgC2SVipLog log;
	LMsgC2SVideo video;
	video.m_videoId = 5;
	m_clientSocket->Send(video.GetSendBuff());
}

void Work::HanderUserItem(LMsgS2CItemInfo*msg)
{
	LLOG_DEBUG("Work::HanderUserItem %d:%d:%d", msg->m_card1, msg->m_card2, msg->m_card3);
}

void Work::HanderUserCreateRoom(LMsgS2CCreateDeskRet* msg)
{
	LLOG_DEBUG("Work::HanderUserCreateRoom %d:%d", msg->m_errorCode, msg->m_deskId);

}

void Work::HanderUserAddRoom(LMsgS2CAddDeskRet* msg)
{
	LLOG_DEBUG("Work::HanderUserAddRoom %d:%d", msg->m_errorCode, msg->m_deskId);

}

void Work::HanderUserIntoDesk(LMsgS2CIntoDesk* msg)
{
	LLOG_DEBUG("Work::HanderUserIntoDesk deskid=%d pos=%d ready=%d score=%d", msg->m_deskId, msg->m_pos,msg->m_ready,msg->m_score);
	LMsgC2SUserReady ready;
	m_clientSocket->Send(ready.GetSendBuff());

	m_myPos = msg->m_pos;
}

void Work::HanderUserAddDesk(LMsgS2CDeskAddUser* msg)
{
	LLOG_DEBUG("Work::HanderUserAddDesk userId=%d pos=%d nike=%s score=%d", msg->m_userId, msg->m_pos, msg->m_nike.c_str(), msg->m_score);
}

void Work::HanderUserPlayStart(LMsgS2CPlayStart* msg)
{
	LLOG_DEBUG("Work::HanderUserPlayStart %d,%d,%d,%d,%d", msg->m_zhuang,msg->m_cardCount[0],msg->m_cardCount[1],msg->m_cardCount[2],msg->m_cardCount[3]);
	m_handCard.clear();
	m_pengCard.clear();
	m_agangCard.clear();
	m_gangCard.clear();

	for (Lint i= 0 ; i < msg->m_cardCount[0]; ++i)
	{
		LLOG_DEBUG("Work::HanderUserPlayStart mycard %d,%d", msg->m_cardValue[i].m_color,msg->m_cardValue[i].m_number);
		Card* card = gCardMgr.GetCard(msg->m_cardValue[i].m_color,msg->m_cardValue[i].m_number);
		m_handCard.push_back(card);

		LLOG_DEBUG("Work::HanderUserPlayStart getcard %d,%d", card->m_color,card->m_number);
	}
	
	m_state = 0;
}

void Work::HanderUserGetCard(LMsgS2COutCard* msg)
{
	if(m_myPos == msg->m_pos)
	{
		if(msg->m_flag == 0)
		{
			Card* card = gCardMgr.GetCard(msg->m_curCard.m_color,msg->m_curCard.m_number);
			m_handCard.push_back(card);
			gCardMgr.SortCard(m_handCard);
			LLOG_DEBUG("Work::HanderUserGetCard mycard %d,%d,%d,%d", card->m_color,card->m_number,msg->m_pos,m_myPos);
		}
		m_state = 1;
	}
	else
	{
		m_state = 0;
	}
}

void Work::HanderUserPlayerCard(LMsgS2CUserPlay* msg)
{
	LLOG_DEBUG("Work::HanderUserPlayerCard %d,%d,%d",msg->m_errorCode,msg->m_flag,msg->m_pos);
	m_state = 0;
	if(msg->m_pos == m_myPos && msg->m_errorCode == 0)
	{
		Lint count = 0;
		CardVector::iterator it = m_handCard.begin();
		for(; it!=m_handCard.end();)
		{
			if((*it)->m_number == msg->m_card.m_number && (*it)->m_color == msg->m_card.m_color)
			{
				it = m_handCard.erase(it);
				count += 1;
			}
			else
			{
				++it;
			}
			if(msg->m_flag == 1 || msg->m_flag == 3)
			{
				if(count == 1)
					break;
			}
			if(msg->m_flag == 2)
			{
				if(count == 4)
					break;
			}
		}
	}
}

void Work::HanderUserThink(LMsgS2CThink* msg)
{
	if(msg->m_flag == 1)
	{
		LLOG_DEBUG("Work::HanderUserThink %d",msg->m_count);
		//这里模拟思考
		Lint seed = L_Rand(0,msg->m_count-1);
		if(seed == msg->m_count)//过
		{
			LMsgC2SUserOper oper;
			oper.m_think = 0;
			m_clientSocket->Send(oper.GetSendBuff());
		}
		else
		{
			LMsgC2SUserOper oper;
			oper.m_think = msg->m_think[seed];
			m_clientSocket->Send(oper.GetSendBuff());
		}
	}
}

void Work::HanderUserOper(LMsgS2CUserOper* msg)
{
	if(msg->m_errorCode == 0)
	{
		if(msg->m_pos == m_myPos)
		{
			LLOG_DEBUG("Work::HanderUserOper %d",msg->m_think);
			if(msg->m_think == THINK_OPERATOR_GANG)
			{
				Lint count = 0;
				CardVector::iterator it = m_handCard.begin();
				for(; it != m_handCard.end();)
				{
					if((*it)->m_color == msg->m_card.m_color && (*it)->m_number == msg->m_card.m_number)
					{
						count++;
						it = m_handCard.erase(it);
					}
					else
					{
						++it;
					}
					if(count == 3)
						break;
				}
			}
			else if(msg->m_think == THINK_OPERATOR_PENG)
			{
				Lint count = 0;
				CardVector::iterator it = m_handCard.begin();
				for(; it != m_handCard.end();)
				{
					if((*it)->m_color == msg->m_card.m_color && (*it)->m_number == msg->m_card.m_number)
					{
						count++;
						it = m_handCard.erase(it);
					}
					else
					{
						++it;
					}
					if(count == 2)
						break;
				}
				m_state = 1;
			}
		}
	}
	else
	{
		m_state = 0;
	}
}

void Work::HanderGameOver(LMsgS2CGameOver* msg)
{
	LMsgC2SUserReady ready;
	m_clientSocket->Send(ready.GetSendBuff());
}

void Work::HanderReset(LMsgS2CResetDesk*msg)
{
	LLOG_DEBUG("Work::HanderReset %d:%d", msg->m_time, msg->m_flag);
	if (msg->m_flag == 0 && msg->m_errorCode == 0)
	{
		Lint seed = 2;//L_Rand(1, 2);

		LMsgC2SSelectResetDesk send;
		send.m_flag = seed;
		send.m_pos = m_myPos;
		m_clientSocket->Send(send.GetSendBuff());
	}
}

void Work::HanderReconnect(LMsgS2CDeskState* msg)
{
	LLOG_DEBUG("Work::HanderReconnect %d,%d,%d,%d",msg->m_state,m_myPos,msg->m_flag,msg->m_dCount);
	m_state = 0;
	if(msg->m_state == 2)
	{
		m_handCard.clear();

		for (Lint i= 0 ; i < msg->m_cardCount[m_myPos]; ++i)
		{
			LLOG_DEBUG("Work::HanderReconnect mycard %d,%d", msg->m_cardValue[i].m_color,msg->m_cardValue[i].m_number);
			Card* card = gCardMgr.GetCard(msg->m_cardValue[i].m_color,msg->m_cardValue[i].m_number);
			m_handCard.push_back(card);

			LLOG_DEBUG("Work::Work::HanderReconnect getcard %d,%d", card->m_color,card->m_number);
		}

		if(msg->m_flag == 1)
		{
			m_state = 1;
		}
		else
		{
			LMsgC2SUserOper oper;
			oper.m_think = 0;
			m_clientSocket->Send(oper.GetSendBuff());
		}
	}
	else
	{
		LMsgC2SUserReady ready;
		m_clientSocket->Send(ready.GetSendBuff());
	}

	LMsgC2SResetDesk send;
	m_clientSocket->Send(send.GetSendBuff());
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if (msg->m_sp == m_clientSocket)
	{
		HanderCenterLogout();
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	//SocketPtrManager::Instance().AddSocketPtr(msg->m_sp);
}

static char* MSDK_PATH = "/cgi-bin/sdk_pay";

static char* ADMIN_PATH = "/cgi-bin/admin";

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
	send->Kick();
}

//玩家请求登录
void Work::HanderCenterUserLogin(LMsgCe2LUserLogin*msg)
{
	LLOG_DEBUG("Work::HanderCenterUserLogin seed %d", msg->m_seed);
}

void Work::HanderCenterLogout()
{
	LLOG_ERROR("warning center server lose connect!!!!!!");
	Stop();
}


void Work::ConnectCenterServer()
{
	m_clientSocket = gInsideNet.GetNewSocket();
	m_clientSocket->Connect(gConfig.GetCenterIp(), gConfig.GetCenterPort());

	LMsgC2SMsg send;
	send.m_openId = gConfig.GetOpenId();
	send.m_nike = gConfig.GetOpenId();
	send.m_sign = gConfig.GetOpenId();
	//send.m_plate = "wechat";
	m_clientSocket->Send(send.GetSendBuff());
}

void Work::HanderLoginRet()
{

}

void Work::HanderServerMsg(LMsgS2CMsg* msg)
{
	LLOG_DEBUG("Work::HanderServerMsg %d:%d:%s:%d", msg->m_seed, msg->m_id, msg->m_gateIp.c_str(), msg->m_gatePort);
	m_clientSocket.reset();

	m_clientSocket = gInsideNet.GetNewSocket();
	m_clientSocket->Connect(msg->m_gateIp, msg->m_gatePort);

	LMsgC2SLogin send;
	send.m_seed = msg->m_seed;
	send.m_id = msg->m_id;
	m_clientSocket->Send(send.GetSendBuff());

	msg->m_sp->Stop();
}

void Work::SendCreateRoom()
{
	if (gConfig.GetRoomId() == 0)
	{
		LMsgC2SCreateDesk send;
		send.m_flag = 1;
		send.m_gold = 1;
		if (m_clientSocket)
		{
			m_clientSocket->Send(send.GetSendBuff());
		}
	}
	else
	{
		LMsgC2SAddDesk send;
		send.m_deskId = gConfig.GetRoomId();
		if (m_clientSocket)
		{
			m_clientSocket->Send(send.GetSendBuff());
		}
	}
}

void Work::OutCard()
{
	Lint flag;
	AI ai;
	Card* card = ai.CheckOutCard(m_handCard,CardVector(),CardVector(),CardVector(),flag);

	LLOG_DEBUG("Work::OutCard %d",flag);
	if(flag == 1)
	{
		LMsgC2SUserPlay play;
		play.m_flag = 1;
		play.m_card.m_color = card->m_color;
		play.m_card.m_number = card->m_number;
		m_clientSocket->Send(play.GetSendBuff());
	}
	else//暗杠
	{
		LMsgC2SUserPlay play;
		play.m_flag = 2;
		play.m_card.m_color = card->m_color;
		play.m_card.m_number = card->m_number;
		m_clientSocket->Send(play.GetSendBuff());
	}

	m_state = 0;
}