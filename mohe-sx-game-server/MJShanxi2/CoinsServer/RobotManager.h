#ifndef _ROBOTMANAGER_H
#define _ROBOTMANAGER_H
#include "LSingleton.h"
#include "Robot.h"
class RobotManager :public LSingleton<RobotManager>
{
public:
	RobotManager(void);
	~RobotManager(void);

	virtual	bool	Init();
	virtual	bool	Final();

	void	Tick();

	RobotPtr	CaptureRotot();
	void		ReleaseRotot( Lint id );
private:
	boost::mutex		m_mutex;
	std::map<Lint,RobotPtr>		m_RobotList;
	std::queue<Lint>		m_RobotFreeList;	// 自由的AI
	std::set<Lint>			m_RobotUseList;		// 正在使用的AI
};
#define gRobotManager RobotManager::Instance()


#endif