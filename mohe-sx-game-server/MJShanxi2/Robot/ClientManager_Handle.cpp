#include "StdAfx.h"
#include "ClientManager.h"
#include "Net_Server_Inside.h"
#include <string>
#include "..\LogicServer\Card.h"
#include "Public_func.h"

void User::Handler_Heartbeat(LMsgS2CHeart* msg)
{
	m_Heartbase--;
}

void User::Handler_S_2_C_MSG(LMsgS2CMsg* msg)
{
	if (msg->m_errorCode !=0)
	{
		LLOG_ERROR("=== Center 登录失败 ErrCode=%d",msg->m_errorCode);
		return;
	}
	LLOG_ERROR("=== Center 登录成功=%s",m_UUID.c_str());

	m_seed		= msg->m_seed;
	m_id		= msg->m_id;
	m_gateIp	= msg->m_gateIp;
	m_gatePort	= msg->m_gatePort;
	//
	m_Socket->Stop();
	Set_State(User_State_Connect_Gete);
}

void User::Handler_LOGIN(LMsgS2CLogin* msg)
{
	if (msg->m_errorCode !=0)
	{
		LLOG_ERROR("=== Gate 登录失败 ErrCode=%d",msg->m_errorCode);
		return;
	}
	LLOG_ERROR("=== Gate 登录成功=%s",m_UUID.c_str());
	//
	Set_State(User_State_Hall);
}

void User::Handler_CreateDesk(LMsgS2CCreateDeskRet* msg)
{
	if (msg->m_errorCode !=0)
	{
		LLOG_ERROR("=== 建桌子 失败 ErrCode=%d",msg->m_errorCode);
		return;
	}
	LLOG_ERROR("=== 建桌子成功 %d Name=%s",msg->m_deskId,m_UUID.c_str());
	//
	m_PlayInfo.m_deskId = msg->m_deskId;
	gServer_UserManager().Op_Desk_UpdateOneIndex(msg->m_deskId,m_UUID);	
}

void User::Handler_IntoDesk(LMsgS2CIntoDesk* msg)
{
	LLOG_ERROR("=== 加入桌子 %d Name=%s",msg->m_deskId,m_UUID.c_str());
	m_PlayInfo.m_deskId = msg->m_deskId;
	m_PlayInfo.m_pos	= msg->m_pos;
	m_PlayInfo.m_ready	= msg->m_ready;
	Set_State(User_State_Game_Wait);
}

void User::Handler_Start(LMsgS2CPlayStart* msg)
{
	m_PlayInfo.m_cardValue.clear();
	for (int iii=0; iii<HardCardCount; iii++)
	{
		if (msg->m_cardValue[iii].m_color!=0)
		{
			Card One;
			One.m_color = msg->m_cardValue[iii].m_color;
			One.m_number= msg->m_cardValue[iii].m_number;
			m_PlayInfo.m_cardValue.push_back(One);
		}
	}
	m_PlayInfo.m_DeskBegin = true;
}

void User::Handler_Get_Card(LMsgS2COutCard* msg)
{
	//LLOG_ERROR("=== 摸牌 pos=%d , MyPos=%d , MyName=%s",msg->m_pos,m_PlayInfo.m_pos,m_UUID.c_str());
	//std::cout<<"2";
	m_PlayInfo.m_CurPos = msg->m_pos;
	if (m_PlayInfo.m_CurPos == m_PlayInfo.m_pos)
	{
		// 放入手牌;
		Card One;
		One.m_color = msg->m_curCard.m_color;
		One.m_number= msg->m_curCard.m_number;
		if (One.m_color!=0)
			m_PlayInfo.m_cardValue.push_back(One);
		// 存储思想;
		m_PlayInfo.m_think = msg->m_think;
		Set_State(User_State_Game_Get_Card);
	}
}

void User::Handler_PLAY_CARD(LMsgS2CUserPlay* msg)
{
	//LLOG_ERROR("=== 广播有人出牌 pos=%d , MyPos=%d",msg->m_pos,m_PlayInfo.m_pos);
	m_PlayInfo.m_CurPos = msg->m_pos;
	if (msg->m_pos == m_PlayInfo.m_pos)
	{
		int del = 0;
		if (msg->m_card.m_type == THINK_OPERATOR_OUT)
			del = 1;
		if (msg->m_card.m_type == THINK_OPERATOR_AGANG || msg->m_card.m_type == THINK_OPERATOR_ABU)
			del = 4;
		else if (msg->m_card.m_type == THINK_OPERATOR_MGANG || msg->m_card.m_type == THINK_OPERATOR_MBU)
			del = 1;

		for (int jjj=0; jjj<msg->m_card.m_card.size(); jjj++)
		{
			R_EraseCard(m_PlayInfo.m_cardValue,&msg->m_card.m_card[jjj],del);
		}
	}
}

void User::Handler_User_Think(LMsgS2CThink* msg)
{
	//LLOG_ERROR("=== 思考 m_flag=%d , MyPos=%d",msg->m_flag,m_PlayInfo.m_pos);
	//std::cout<<"1";
	if (msg->m_flag==1)
	{
		Set_State(User_State_Game_THINK);
		m_PlayInfo.m_think.clear();
		m_PlayInfo.m_think = msg->m_think;
	}
}

void User::Handler_User_Open(LMsgS2CUserOper* msg)
{
	if (msg->m_errorCode ==0)
	{
		//LLOG_ERROR("=== 广播有人思考 Pos=%d , MyPos=%d",msg->m_pos,m_PlayInfo.m_pos);
		m_PlayInfo.m_CurPos = msg->m_pos;
		if (m_PlayInfo.m_CurPos == m_PlayInfo.m_pos)
		{
			if (msg->m_think.m_type == THINK_OPERATOR_MGANG)
				R_EraseCard(m_PlayInfo.m_cardValue,&msg->m_think.m_card[0],3);

			else if (msg->m_think.m_type == THINK_OPERATOR_PENG)
				R_EraseCard(m_PlayInfo.m_cardValue,&msg->m_think.m_card[0],2);

			else if (msg->m_think.m_type == THINK_OPERATOR_CHI)
			{
				for (int jjj=0; jjj<msg->m_think.m_card.size(); jjj++)
				{
					R_EraseCard(m_PlayInfo.m_cardValue,&msg->m_think.m_card[jjj],1);
				}
			}
		}
	}
	else
	{
		LLOG_ERROR("=== 广播有人思考 -错误- Pos=%d , MyPos=%d",msg->m_pos,m_PlayInfo.m_pos);
	}
}

void User::Handler_Desk_State(LMsgS2CDeskState* msg)
{
	m_PlayInfo.m_cardValue.clear();
	for (int iii=0; iii<HardCardCount; iii++)
	{
		if (msg->m_cardValue[iii].m_color!=0)
		{
			Card One;
			One.m_color = msg->m_cardValue[iii].m_color;
			One.m_number= msg->m_cardValue[iii].m_number;
			m_PlayInfo.m_cardValue.push_back(One);
		}
	}
	if (msg->m_state == 2)
		m_PlayInfo.m_DeskBegin = true;
}

void User::Handler_Game_Over(LMsgS2CGameOver* msg)
{
	LLOG_ERROR("########  游戏结束 , My=",m_UUID);
	m_PlayInfo.Init_GameOver();
	gServer_UserManager().GameCount++;
}

void User::Handler_Desk_Over(LMsgS2CVipEnd* msg)
{
	LLOG_ERROR("########  厅馆结束 , My=",m_UUID);
	m_PlayInfo.Init_DeskOver();
	gServer_UserManager().Op_Desk_UpdateOneIndex(0,m_UUID);	
	gServer_UserManager().DeskCount++; 
}

void User::Hander_Start_Hu(LMsgS2CUserStartHu* msg)
{
	Set_State(User_State_Game_StartHu);
	m_PlayInfo.m_startHu = msg->m_card;
}

void User::Hander_End_Select(LMsgS2CUserEndCardSelect* msg)
{
	Set_State(User_State_Game_EndSelect);
	m_PlayInfo.m_HaiDi = true;
}

void User::Handler_RESET_ROOM(LMsgS2CResetDesk* msg)
{
	if (msg->m_flag==0)
	{
		LMsgC2SSelectResetDesk SendData;
		SendData.m_flag = 1;
		Send(SendData);
	}
}
