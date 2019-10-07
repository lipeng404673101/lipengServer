#include "StdAfx.h"
#include "ClientManager.h"
#include "Net_Server_Inside.h"
#include <string>
#include "Default.h"
#include "Public_func.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
User::User(LSocketPtr Socket, Lstring IP, Lint Prot,Lstring UUID)
{
	m_Socket	= Socket;
	m_IP		= IP;
	m_Prot		= Prot;
	m_UUID		= UUID;
	m_PlayInfo.Init();
	m_TimeSpace.Init();
	m_Wait = 0;
	m_Heartbase = 0;
	m_connectTime = time(NULL);
	m_IsDead = false;
}

User::~User()
{
}

void User::Send(LMsg& msg)
{
	m_Socket->Send(msg.GetSendBuff());
}

void User::HanderMsg(LMsg* msg)
{
	switch (msg->m_msgId)
	{
	case MSG_S_2_C_HEART:
		Handler_Heartbeat((LMsgS2CHeart*)msg);
		break;
	case MSG_S_2_C_MSG:
		Handler_S_2_C_MSG((LMsgS2CMsg*)msg);
		break;
	case MSG_S_2_C_LOGIN:
		Handler_LOGIN((LMsgS2CLogin*)msg);
		break;
	case MSG_S_2_C_CREATE_ROOM:
		Handler_CreateDesk((LMsgS2CCreateDeskRet*)msg);
		break;
	case MSG_S_2_C_ADD_ROOM:
		Handler_Add_Room((LMsgS2CAddDeskRet*)msg);
		break;
	case MSG_S_2_C_INTO_DESK:
		Handler_IntoDesk((LMsgS2CIntoDesk*)msg);
		break;
	case MSG_S_2_C_START:
		Handler_Start((LMsgS2CPlayStart*)msg);
		break;
	case MSG_S_2_C_GET_CARD:
		Handler_Get_Card((LMsgS2COutCard*)msg);
		break;
	case MSG_S_2_C_PLAY_CARD:
		Handler_PLAY_CARD((LMsgS2CUserPlay*)msg);
		break;
	case MSG_S_2_C_USER_THINK:
		Handler_User_Think((LMsgS2CThink*)msg);
		break;
	case MSG_S_2_C_USER_OPER:
		Handler_User_Open((LMsgS2CUserOper*)msg);
		break;
	case MSG_S_2_C_DESK_STATE:
		Handler_Desk_State((LMsgS2CDeskState*)msg);
		break;
	case MSG_S_2_C_GAME_OVER:
		Handler_Game_Over((LMsgS2CGameOver*)msg);
		break;
	case MSG_S_2_C_VIP_END:
		Handler_Desk_Over((LMsgS2CVipEnd*)msg);
		break;

	////
	case MSG_S_2_C_START_HU:
		Hander_Start_Hu((LMsgS2CUserStartHu*) msg);
		break;
	case MSG_S_2_C_END_CARD_SELECT:
		Hander_End_Select((LMsgS2CUserEndCardSelect*) msg);
		break;

//case MSG_S_2_C_START_HU_SELECT:
//case MSG_S_2_C_GANG_RESULT:
//case MSG_S_2_C_END_GET:

	case MSG_S_2_C_RESET_ROOM:
		Handler_RESET_ROOM((LMsgS2CResetDesk*)msg);
		break;
	default:
		break;
	}
}

LSocketPtr* User::GetSocket()
{
	return &m_Socket;
}

void User::Update()
{
	switch (m_PlayInfo.m_state)
	{
	case User_State_Connect_Center:
		Op_Connect_Center();
		break;
	case User_State_Connect_Center_Wait:
		Op_Connect_CenterWait();
		break;
	case User_State_Connect_Gete:
		Op_Connect_Gate();
		break;
	case User_State_Connect_Gete_Wait:
		Op_Connect_GateWait();
		break;
	case User_State_Hall:
		Op_StateHall();
		break;
	case User_State_Game_GoReady:
		Op_StateReady();
		break;
	case User_State_Game_Wait:
		Op_StateWait();
		break;
	case User_State_Game_Get_Card:
		Op_StateGetCard();
		break;
	case User_State_Game_THINK:
		Op_StateThink();
		break;
	case User_State_Game_StartHu:
		OP_StartHu();
		break;
	case User_State_Game_EndSelect:
		OP_EndSelect();
		break;
	case User_State_Desk_Exit:
		Op_DeskExit();
		break;
	default:
		break;
	}
	Op_Heartbeat();
	OP_Reconnect();
}

void User::Handler_Add_Room(LMsgS2CAddDeskRet* msg)
{
	return;
}

void User::OP_Reconnect()
{
	TimeSpaceCheck(m_connectTime,TimeSpace_Reconnect);
	if (!m_Socket->IsStart())
	{
		m_IsDead = true;
		gServer_UserManager().Op_Desk_UpdateOneIndex(0,m_UUID);	
		//std::cout<<"-----  重连1:"<<m_UUID<<std::endl;
	}

	if (m_Heartbase>2)
	{
		m_IsDead = true;
		gServer_UserManager().Op_Desk_UpdateOneIndex(0,m_UUID);	
		//std::cout<<"-----  重连2:"<<m_UUID<<std::endl;
	}
}

void User::Set_State(User_State Type)
{
	m_PlayInfo.m_state = Type;
	if (Type!=User_State_Game_Wait)
		m_Wait = 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void UserManager::Init(Lstring Ip,Lshort Port,int BeginIndex,int UserCount)
{
	GameCount = 0;
	DeskCount = 0;

	m_Ip   = Ip;
	m_Port = Port;
	m_BeginIndex = BeginIndex;
	m_RobotCount = UserCount;
	int CD=0;
	for (int iii=0; iii<UserCount; iii++)
	{
		Lstring Name = "R_"+num2str(iii+BeginIndex);
		User One(gServer_InsideNet.GetNewSocket(),Ip,Port,Name);
		m_UserList.insert(std::make_pair(One.GetSocket()->get(),One));
		if (CD%4==0)
		{
			DeskInfo Two;
			Two.Index = 0;
			m_UserList_CreateDesk[Name] = Two;
		}
		else
		{
			for (auto itr=m_UserList_CreateDesk.begin(); itr!=m_UserList_CreateDesk.end(); itr++)
			{
				if (itr->second.UserLise.size()<3)
				{
					itr->second.UserLise.push_back(Name);
					break;
				}
			}
		}
		CD++;
	}
}

void UserManager::Handler(LMsg* msg)
{
	auto itr = m_UserList.find(msg->m_sp.get());
	if (itr!=m_UserList.end())
	{
		itr->second.HanderMsg(msg);
	}
}

void UserManager::Update()
{
	int OnLineCount = 0;
	std::vector<std::string> Adds;
	for (auto itr=m_UserList.begin(); itr!=m_UserList.end();)
	{
		itr->second.Update();
		if (itr->second.Is_Conenct())
			OnLineCount++;
		if (itr->second.Is_Dead())
		{
			Adds.push_back(itr->second.GetUUID());
			auto tmp=itr++;
			m_UserList.erase(tmp);
		}
		else
		{
			itr++;
		}
	}
	for (auto itr=Adds.begin(); itr!=Adds.end(); itr++)
	{
		User One(gServer_InsideNet.GetNewSocket(),m_Ip,m_Port,*itr);
		m_UserList.insert(std::make_pair(One.GetSocket()->get(),One));
	}

	static int BeginTime = time(NULL);
	static int Times = time(NULL);
	static int LastCount = 0;
	TimeSpaceCheck(Times,1)
	std::cout<<"######### 1     GameCount	= "<<GameCount/4<<"	"<<int(float(GameCount/4)/float(Times-BeginTime))<<"|s"<<"	+"<<GameCount/4-LastCount<<""<<std::endl;
	std::cout<<"#########   1   DeskCount		= "<<DeskCount/4<<std::endl;
	std::cout<<"#########     1 OnLineCount			= "<<OnLineCount<<std::endl;
	LastCount = GameCount/4;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
UserManager& gServer_UserManager()
{
	static UserManager One;
	return One;
}