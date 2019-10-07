#include "User.h"
#include "Work.h"
#include "UserManager.h"
#include "Config.h"

User::User(const LUser& data, Lint gateId, const Lstring& ip) : m_userData(data)
{
	m_deskRunID = 0;
	m_deskID = 0;
	m_userState = LGU_STATE_COIN;
	m_logicID = 0;
	m_online = true;
	m_gateId = gateId;
	m_ip = ip;
}
	
User::~User()
{
}

void User::Login()
{
	LTime now;
	m_userData.m_lastLoginTime = now.Secs();
	m_online = true;

	// 修改玩家状态
	SetUserState( LGU_STATE_COIN );

	// 状态同步给Manager
	LMsgL2LMGModifyUserState msg_logic;
	msg_logic.m_strUUID = m_userData.m_unioid;
	msg_logic.m_logicID = m_logicID;
	msg_logic.m_userstate = m_userState;
	gWork.SendToLogicManager( msg_logic );

	// 状态同步给Gate
	LMsgL2GModifyUserState msg_gate;
	msg_gate.m_strUUID = m_userData.m_unioid;
	msg_gate.m_logicID = m_logicID;
	msg_gate.m_userstate = m_userState;
	gWork.SendToGate( m_gateId, msg_gate );
}

void User::Logout()
{
	m_online = false;

	SetUserState(LGU_STATE_CENTER);
	// 状态同步给Manager
	LMsgL2LMGModifyUserState msg_logic;
	msg_logic.m_strUUID = m_userData.m_unioid;
	msg_logic.m_logicID =  0;
	msg_logic.m_userstate = m_userState;
	gWork.SendToLogicManager( msg_logic );

	// 状态同步给Gate
	LMsgL2GModifyUserState msg_gate;
	msg_gate.m_strUUID = m_userData.m_unioid;
	msg_gate.m_logicID = 0;
	msg_gate.m_userstate = m_userState;
	gWork.SendToGate( m_gateId, msg_gate );
}

void User::Send(LMsg& msg)
{
	LMsgL2GUserMsg send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_dataBuff = msg.GetSendBuff();

	gWork.SendToGate(m_gateId, send);
}

void User::Send(const LBuffPtr& buff)
{
	LMsgL2GUserMsg send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_dataBuff = buff;

	gWork.SendToGate(m_gateId, send);
}

void User::AddCoinsCount(Lint count, Lint operType, Lint userid, const Lstring& uuid)
{
	LMsgCN2LMGModifyUserCoins msg;
	msg.coinsNum = count;
	msg.isAddCoins = 1;
	msg.operType = operType;
	msg.m_userid = userid;
	msg.m_strUUID= uuid;
	gWork.SendToLogicManager(msg);
}

void User::AddCoinsCount(Lint count, Lint operType)
{
	m_userData.m_coins += count;

	// 
	LMsgCN2LMGModifyUserCoins msg;
	msg.coinsNum = count;
	msg.isAddCoins = 1;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID= m_userData.m_unioid;
	gWork.SendToLogicManager(msg);
}

void User::DelCoinsCount(Lint count, Lint operType)
{
	if ( m_userData.m_coins < count )
	{
		m_userData.m_coins = 0;
	}
	else
	{
		m_userData.m_coins -= count;
	}
	LMsgCN2LMGModifyUserCoins msg;
	msg.coinsNum = count;
	msg.isAddCoins = 0;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID= m_userData.m_unioid;
	gWork.SendToLogicManager(msg);
}
