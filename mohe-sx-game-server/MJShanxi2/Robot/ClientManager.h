#pragma once
#include <map>
#include "LMsg.h"
#include "Default.h"
#include <vector>
#include <set>
//////////////////////////////////////////////////////////////////////////
class User
{
public:
	User(LSocketPtr Socket, Lstring IP, Lint Prot,Lstring UUID);
	~User();
	LSocketPtr* GetSocket();
public:
	void Update();
	//////////////////////////////////////////////////////////////////////////
	void HanderMsg(LMsg* msg);

	//////////////////////////////////////////////////////////////////////////
	bool Is_Conenct()		{return m_Socket->IsStart() && m_PlayInfo.m_state>=User_State_Hall;};
	bool Is_Dead()			{return m_IsDead;};
	std::string GetUUID()	{return m_UUID;};
private:
	//////////////////////////////////////////////////////////////////////////
	void Set_State(User_State Type);
	void Op_Heartbeat();
	void Op_Connect_Center();
	void Op_Connect_CenterWait();
	void Op_Connect_Gate();
	void Op_Connect_GateWait();
	void Op_StateHall();
	void Op_StateWait();
	void Op_StateReady();
	void Op_StateGetCard();
	void Op_StateThink();
	void Op_DeskExit();
	void OP_Reconnect();
	void OP_StartHu();
	void OP_EndSelect();
	//////////////////////////////////////////////////////////////////////////
	void Handler_Heartbeat(LMsgS2CHeart* msg);
	void Handler_S_2_C_MSG(LMsgS2CMsg* msg);
	void Handler_LOGIN(LMsgS2CLogin* msg);
	void Handler_CreateDesk(LMsgS2CCreateDeskRet* msg);
	void Handler_Add_Room(LMsgS2CAddDeskRet* msg);
	void Handler_IntoDesk(LMsgS2CIntoDesk* msg);
	void Handler_Start(LMsgS2CPlayStart* msg);
	void Handler_Get_Card(LMsgS2COutCard* msg);
	void Handler_PLAY_CARD(LMsgS2CUserPlay* msg);
	void Handler_User_Think(LMsgS2CThink* msg);
	void Handler_User_Open(LMsgS2CUserOper* msg);
	void Handler_Game_Over(LMsgS2CGameOver* msg);
	void Handler_Desk_State(LMsgS2CDeskState* msg);
	void Handler_Desk_Over(LMsgS2CVipEnd* msg);
	void Handler_RESET_ROOM(LMsgS2CResetDesk* msg);

	void Hander_Start_Hu(LMsgS2CUserStartHu* msg);
	void Hander_End_Select(LMsgS2CUserEndCardSelect* msg);

	//////////////////////////////////////////////////////////////////////////
	void Send(LMsg& msg);
private:
	LSocketPtr			m_Socket;
	Lstring				m_TestStr;
	Lstring				m_IP;
	Lint				m_Prot;

	//
	Lstring				m_UUID;
	Lint				m_seed;
	Lint				m_connectTime;
	Lint				m_id;
	Lstring				m_gateIp;
	Lshort				m_gatePort;
	//
	User_Play_Info		m_PlayInfo;
	TimeSpace			m_TimeSpace;
	//
	int					m_Wait;
	int					m_Heartbase;
	bool				m_IsDead;
};


//////////////////////////////////////////////////////////////////////////
class UserManager
{
public:
	void Init(Lstring Ip,Lshort Port,int BeginIndex,int UserCount);

public:
	void Update();
	void Handler(LMsg* msg);

	void Op_Desk_UpdateOneIndex(Lint RoomIndex, Lstring Name);
	Lint Op_Desk_GetOneIndex(Lstring Name);

private:
	std::map<void*,User>				m_UserList;
	Lstring								m_Ip;
	Lshort								m_Port;
	Lint								m_BeginIndex;
	int									m_RobotCount;
	std::map<Lstring,DeskInfo>			m_UserList_CreateDesk;
	//
	std::map<Lint,std::set<Lint>>		m_RoomList;
	//
public:
	int GameCount;
	int DeskCount;
};


//////////////////////////////////////////////////////////////////////////
UserManager& gServer_UserManager();






