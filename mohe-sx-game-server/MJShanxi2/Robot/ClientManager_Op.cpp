#include "StdAfx.h"
#include "ClientManager.h"
#include "Net_Server_Inside.h"
#include <string>
#include "..\LogicServer\Card.h"
#include "Public_func.h"
#include "Config.h"
using namespace boost::asio;  
using boost::asio::ip::tcp;  

void User::Op_Connect_Center()
{
	boost::asio::ip::address add;  
	m_Socket->AsyncConnect(tcp::endpoint(add.from_string(m_IP), m_Prot));
	Set_State(User_State_Connect_Center_Wait);
}

void User::Op_Connect_CenterWait()
{
	if (m_Socket->IsStart())
	{
		LMsgC2SMsg SendData;
		SendData.m_severID	= gConfig.GetServerID();
		SendData.m_plate	= "local";
		SendData.m_openId	= m_UUID;
		SendData.m_nike		= m_UUID;
		Send(SendData);
		Set_State(User_State_Connect_Nothing);
	}
}

void User::Op_Connect_Gate()
{
	try
	{
		boost::asio::ip::address add;  
		m_Socket->AsyncConnect(tcp::endpoint(add.from_string(m_gateIp), m_gatePort));
		Set_State(User_State_Connect_Gete_Wait);
	}
	catch (...)
	{
		m_Socket->Stop();
	}
}

void User::Op_Connect_GateWait()
{
	if (m_Socket->IsStart())
	{
		TimeSpaceCheck(m_TimeSpace.Login,TimeSpace_Login) 
		LMsgC2SLogin SendData;
		SendData.m_seed	= m_seed;
		SendData.m_id	= m_id;
		std::stringstream ss;
		ss << m_seed << "3c6e0b8a9c15224a8228b9a98ca1531d";
		SendData.m_md5	= Md5Hex(ss.str().c_str(),ss.str().size());
		Send(SendData);
	}
}

void User::Op_Heartbeat()
{
	TimeSpaceCheck(m_TimeSpace.HeartbeatTime,TimeSpace_HeartbeatTime)
	LMsgC2SHeart SendData;
	Send(SendData);
	m_Heartbase++;
}

void User::Op_StateHall()
{
	TimeSpaceCheck(m_TimeSpace.DeskOpTime,5);
	int RoomId = gServer_UserManager().Op_Desk_GetOneIndex(m_UUID);
	if (RoomId==-2)
	{
		return;
	}
	else if (RoomId==-1)
	{
		LMsgC2SCreateDesk SendData;
		SendData.m_flag		= 1;
		SendData.m_secret	= "123456";
		SendData.m_gold		= 1;
		SendData.m_state	= gConfig.GetPlayType();
		SendData.m_robotNum = RoomAddRobot;
		SendData.m_playtype.push_back(1);
		SendData.m_playtype.push_back(3);
		SendData.m_playtype.push_back(4);
		Send(SendData);
		LLOG_DEBUG("==== 建桌  Name=%s", m_UUID.c_str());
	}
	else
	{
		LMsgC2SAddDesk SendData;
		SendData.m_deskId = RoomId;
		Send(SendData);
		LLOG_DEBUG("==== 加桌  deskId=%d , Name=%s", RoomId,m_UUID.c_str());
	}
}

void User::Op_StateWait()
{
	TimeSpaceCheck(m_TimeSpace.Wait,1);
	if (m_Wait>10 && m_PlayInfo.m_state>User_State_Hall)
	{
		Set_State(User_State_Desk_Exit);
	}
	m_Wait++;
	//LLOG_DEBUG("==== Op_StateWait  Name=%s , MyPos=%d", m_UUID.c_str(),m_PlayInfo.m_pos);
}

void UserManager::Op_Desk_UpdateOneIndex(Lint RoomIndex, Lstring Name)
{
	auto Creater = m_UserList_CreateDesk.find(Name);
	if (m_UserList_CreateDesk.end() != Creater)
	{
		Creater->second.Index = RoomIndex;
	}
}

Lint UserManager::Op_Desk_GetOneIndex(Lstring Name)
{
	auto Creater = m_UserList_CreateDesk.find(Name);
	if (m_UserList_CreateDesk.end() != Creater)
	{
		if (Creater->second.Index == 0)
			return -1;
		else
			return -2;
	}
	else
	{
		for (auto itr=m_UserList_CreateDesk.begin(); itr!=m_UserList_CreateDesk.end(); itr++)
		{
			if (itr->second.UserLise.end() != std::find(itr->second.UserLise.begin(),itr->second.UserLise.end(),Name))
			{
				if (itr->second.Index == 0)
					return -2;
				else
					return itr->second.Index;
			}
		}
	}
	return -2;
}

void User::Op_StateGetCard()
{
	TimeSpaceCheck(m_TimeSpace.OutCard,gConfig.GetWaitSecond())
	if (!m_PlayInfo.m_DeskBegin)
		return;
	//std::cout<<"1";
	LMsgC2SUserPlay SendData;
	if (m_PlayInfo.m_think.size()>0)
	{
		for (int i = 0; i < m_PlayInfo.m_think.size(); i++)
		{
			SendData.m_thinkInfo.m_type = m_PlayInfo.m_think[i].m_type;
			std::vector<CardValue>& mCard = m_PlayInfo.m_think[i].m_card;
			for (int j = 0; j <  mCard.size(); j++)
			{
				CardValue card;
				card.m_color = mCard[j].m_color;
				card.m_number =  mCard[j].m_number;
				SendData.m_thinkInfo.m_card.push_back(card) ;
			}
			break;
		}
	}
	else
	{
		SendData.m_thinkInfo.m_type = THINK_OPERATOR_OUT;
		CardValue card;
		int irand=rand()%m_PlayInfo.m_cardValue.size();
		card.m_color = m_PlayInfo.m_cardValue[irand].m_color;
		card.m_number = m_PlayInfo.m_cardValue[irand].m_number;
		SendData.m_thinkInfo.m_card.push_back(card);
	}
	Send(SendData);
	Set_State(User_State_Game_Wait);
}

void User::Op_StateThink()
{
	TimeSpaceCheck(m_TimeSpace.OutThink,gConfig.GetWaitSecond())
	if (!m_PlayInfo.m_DeskBegin)
		return;
	//std::cout<<"1";
	LMsgC2SUserOper SendData;
	for (int i = 0; i < m_PlayInfo.m_think.size(); i ++)
	{
		SendData.m_think.m_type = m_PlayInfo.m_think[i].m_type;
		std::vector<CardValue>& mCard = m_PlayInfo.m_think[i].m_card;
		for (int j = 0; j <  mCard.size(); j++)
		{
			CardValue card;
			card.m_color = mCard[j].m_color;
			card.m_number =  mCard[j].m_number;
			SendData.m_think.m_card.push_back(card) ;
		}
		break;
	}
	Send(SendData);
	Set_State(User_State_Game_Wait);


	//std::cout<<"==== 思想 Type="<<SendData.m_think.m_type<<std::endl;
	//for (auto iii=0; iii<SendData.m_think.m_card.size(); iii++)
	//{
	//	std::cout<<"====      Card Color="	<<SendData.m_think.m_card[iii].m_color
	//		<<"Number="<<SendData.m_think.m_card[iii].m_number<<std::endl;
	//}
}

void User::Op_StateReady()
{
	LMsgC2SUserReady SendData;
	Send(SendData);
	Set_State(User_State_Game_Wait);
}

void User::Op_DeskExit()
{
	LMsgC2SResetDesk msg;
	Send(msg);
	Set_State(User_State_Game_Wait);
}

void User::OP_StartHu()
{
	if (m_PlayInfo.m_startHu.size() > 0)
	{
		LMsgC2SUserStartHuSelect msg;
		auto startHu = m_PlayInfo.m_startHu.begin();
		if (startHu->m_type == START_HU_QUEYISE || startHu->m_type == START_HU_BANBANHU)
		{
			msg.m_type = startHu->m_type;
		}
		else
		{
			msg.m_type = START_HU_NULL;
		}
		Send(msg);
	}
}
void User::OP_EndSelect()
{
	LMsgC2SUserEndCardSelect msg;
	msg.m_flag = 1;
	Send(msg);
}