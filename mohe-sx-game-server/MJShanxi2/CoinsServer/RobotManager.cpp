#include "RobotManager.h"
#include "UserManager.h"
#include "LMsg.h"
#include "User.h"
#include "LTabFile.h"
#include "Config.h"
#include "DeskManager.h"
#include "Work.h"

RobotManager::RobotManager(void)
{
}


RobotManager::~RobotManager(void)
{
}

bool RobotManager::Init()
{
	for ( int i = 0; i < 50000; ++i )
	{
		LUser* user = new LUser();
		user->m_id = 10000001 + i;

		std::ostringstream str;
		str << "Robot" << user->m_id;
		
		user->m_openid = "";
		user->m_nike = str.str();
		user->m_sex =  i%2;
		user->m_provice = "";
		user->m_city = "";
		user->m_country = "";
		user->m_headImageUrl = "";
		user->m_unioid =  str.str();
		user->m_numOfCard1s = 0;
		user->m_numOfCard2s = 0;
		user->m_numOfCard3s = 0;
		user->m_regTime =  0;
		user->m_lastLoginTime = 0;
		user->m_new = 0;
		user->m_gm = 0;
		user->m_totalbuynum =  0;
		user->m_totalplaynum =  0;
		user->m_customInt[0] = 0;   //总共玩的圈数
		user->m_customInt[1] = 0;  //总共赢的圈数
		RobotPtr robot(new Robot(*user));
		m_RobotList[user->m_id] = robot;
		m_RobotFreeList.push(user->m_id);
		robot->SetOnline( false );	// 默认机器人不在线
	}
	return true;
}

bool RobotManager::Final()
{
	return true;
}

void RobotManager::Tick()
{
	// 自动配桌 压测
	if ( gConfig.GetDebugModel() && gWork.GetGateCount() > 0 )
	{
		std::vector<Lint> playtype;
		//playtype.push_back( PT_ZIMOADDFAN );
		//playtype.push_back( PT_DIANGANG_THR );
		//playtype.push_back( PT_EIGHTFAN );
		CardValue cardvlaue[CARD_COUNT];

		int number = 0;		// 每秒进桌多少机器人
		int enternumber = 0;	// 统计进入机器人的数量
		for ( int i = 0; i < number; ++i )
		{
			if ( gUserManager.GetUsersCount() <= gConfig.GetMaxUserCount() )
			{
				RobotPtr robot = CaptureRotot();
				if ( robot )
				{
					gUserManager.AddUser( robot );
					LLOG_DEBUG("RobotManager::Tick robot enterdesk, usersize=%d", gUserManager.GetUsersCount());

					gDeskManager.UserEnterDesk( robot->GetUserDataId(), SiChuanMaJinagxlc, playtype, 0, cardvlaue );
					robot->Login();
					enternumber++;
				}
			}
		}
		if ( enternumber > 0 )
		{
			LLOG_ERROR("RobotManager::Tick robot enter desk, usersize=%d, enternumber=%d", gUserManager.GetUsersCount(), enternumber);
		}
	}
}

RobotPtr RobotManager::CaptureRotot()
{
	boost::mutex::scoped_lock l(m_mutex);
	if ( m_RobotFreeList.size() > 0 )
	{
		RobotPtr robot = m_RobotList[m_RobotFreeList.front()];
		m_RobotUseList.insert(m_RobotFreeList.front());
		m_RobotFreeList.pop();
		return robot;
	}
	return RobotPtr();
}

void RobotManager::ReleaseRotot( Lint id )
{
	boost::mutex::scoped_lock l(m_mutex);
	auto it = m_RobotUseList.find( id );
	if ( it != m_RobotUseList.end() )
	{
		m_RobotFreeList.push( id );
		m_RobotUseList.erase( it );
	}
	else
	{
		LLOG_ERROR( "RobotManager::ReleaseRotot not find id=%d", id );
	}
}

