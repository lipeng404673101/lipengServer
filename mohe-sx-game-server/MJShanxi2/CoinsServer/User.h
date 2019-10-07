#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"

#include "SafeResourceLock.h"

class User : public CResourceLock
{
public:
	User(const LUser& data, Lint gateId, const Lstring& ip);
	virtual ~User();

	virtual bool IsRobot() { return false; }

	void	SetUserData( const LUser& data ) { m_userData = data; }
	const LUser& GetUserData() const { return m_userData; }

	//获取玩家数据库里id
	Lint	GetUserDataId() const { return m_userData.m_id; };

	// 玩家登陆到金币桌子
	virtual void	Login();
	// 玩家退出金币桌子
	virtual void	Logout();

	// 直接发给用户
	virtual void	Send(LMsg& msg);
	virtual void	Send(const LBuffPtr& buff);

	static void	AddCoinsCount(Lint count, Lint operType, Lint userid, const Lstring& uuid);
	virtual void	AddCoinsCount(Lint count, Lint operType);
	virtual void	DelCoinsCount(Lint count, Lint operType);

	void	SetDeskRunID(Lint nValue) { m_deskRunID = nValue; }
	Lint	GetDeskRunID() const { return m_deskRunID; }

	void	SetDeskID(Lint nValue) { m_deskID = nValue; }
	Lint	GetDeskID() const { return m_deskID; }

	LGUserMsgState	GetUserState() const { return m_userState; }
	void	SetUserState(LGUserMsgState nValue) { m_userState = nValue; }

	void	SetUserLogicID(Lint nValue) { m_logicID = nValue; }
	Lint	GetUserLogicID() const { return m_logicID; }

	void	SetOnline( bool b ) { m_online = b; }
	bool	GetOnline() const { return m_online; }

	void	SetIp(const Lstring& ip) { m_ip = ip; }
	const Lstring& GetIp() const { return m_ip; }

	void	SetUserGateID(Lint nValue){ m_gateId = nValue; }
	Lint	GetUserGateID() const { return m_gateId; }

protected:
	LUser	m_userData;					//玩家数据

	// 游戏桌信息
	Lint	m_deskRunID;				//桌子运行时ID
	Lint	m_deskID;					//玩家桌子ID 玩家已开始就设置桌子

	// 玩家状态信息
	LGUserMsgState	m_userState;				//玩家当前状态
	Lint	m_logicID;					//玩家在哪个logic上面 玩家进入桌子成功 返回记录的 要考虑服务器慢的时候 

	// 在线信息
	bool	m_online;
	Lint	m_gateId;					//玩家在哪个gate上面
	Lstring	m_ip;
};

typedef boost::shared_ptr<User> UserPtr;

#endif