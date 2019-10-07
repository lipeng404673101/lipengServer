#ifndef _ROBOT_H
#define _ROBOT_H

#include "LBase.h"
#include "User.h"
#include "LTabFile.h"
class Robot : public User
{
public:
	Robot(LUser& data, Lint gateId);
	~Robot(void);
	void	Tick();
	void	SetRobotIndex(Lint index);
	Lint	GetRobotIndex();
private:
	Lint	mIndex;
};

#endif
