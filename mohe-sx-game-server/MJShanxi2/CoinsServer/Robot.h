#ifndef _ROBOT_H
#define _ROBOT_H

#include "LBase.h"
#include "User.h"

class Robot : public User
{
public:
	Robot(const LUser& data);
	~Robot(void);
	
	virtual bool IsRobot() { return true; }

	virtual void	Login();
	// 玩家退出金币桌子
	virtual void	Logout();

	// 直接发给用户
	virtual void	Send(LMsg& msg);
	virtual void	Send(const LBuffPtr& buff);

	virtual void	AddCoinsCount(Lint count, Lint operType);
	virtual void	DelCoinsCount(Lint count, Lint operType);

private:
	
};

typedef boost::shared_ptr<Robot> RobotPtr; 

#endif
