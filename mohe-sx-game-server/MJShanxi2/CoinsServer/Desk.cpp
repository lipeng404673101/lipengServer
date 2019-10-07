#include "Desk.h"
#include "LLog.h"
#include "UserManager.h"
#include "Work.h"
#include "Config.h"
#include "RobotManager.h"

Desk::Desk( Lint runid, GameType type )
{
	m_runID = runid;
	m_gameType = type;
	m_deskMaxPlayer = 4;

	m_robotNum = 0;
}

bool Desk::CheckGameStart()
{
	if ( m_deskMaxPlayer == (int)m_users.size() )
	{
		for ( auto it = m_users.begin(); it != m_users.end(); ++it )
		{
			if ( !it->m_bReady )
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

bool Desk::OnUserInRoom( UserPtr& user )
{
	// 先计算玩家位置
	bool invalid_pos[DESK_USER_COUNT] = {false};	// 标示位置是否被占用
	for( auto userit = m_users.begin(); userit != m_users.end(); ++userit )
	{
		invalid_pos[userit->m_pos] = true;
	}
	Lint pos = INVAILD_POS;
	for ( int i = 0; i < m_deskMaxPlayer && i < DESK_USER_COUNT; ++i )
	{
		if ( !invalid_pos[i] )
		{
			pos = i;
			break;
		}
	}
	if ( pos == INVAILD_POS )
	{
		LLOG_ERROR( "Desk::OnUserInRoom not find pos, userid=%d, size=%d", user->GetUserDataId(), (int)m_users.size() );
		return false;
	}

	// 添加到
	DeskUser deskuser(user->GetUserDataId(), pos);
	// 默认给玩家准备好了
	deskuser.m_bReady = true;

	// 设置运行时桌子
	user->SetDeskRunID( m_runID );

	// 
	LMsgS2CIntoDesk send1;
	send1.m_deskId = 0;	// 此时桌子ID为0
	send1.m_pos = pos;
	send1.m_ready = deskuser.m_bReady ? 1 : 0;
	send1.m_score = 0;
	send1.m_coins = user->GetUserData().m_coins;
	send1.m_state = (Lint)m_gameType;
	send1.m_maxCircle = 1;
	send1.m_playtype = m_playType;
	send1.m_changeOutTime = gConfig.GetChangeOutTime();
	send1.m_opOutTime = gConfig.GetOpOutTime();
	send1.m_baseScore = gConfig.GetBaseScore();
	send1.m_credits = user->GetUserData().m_creditValue;
	send1.m_userIp = user->GetUserData().m_customString1;
	send1.m_userGps = user->GetUserData().m_customString2;
	user->Send(send1);

	LMsgS2CDeskAddUser addmsg1;
	addmsg1.m_userId = user->GetUserDataId();
	addmsg1.m_score = 0;
	addmsg1.m_pos = pos;
	addmsg1.m_nike = user->GetUserData().m_nike;
	addmsg1.m_ready = deskuser.m_bReady;
	addmsg1.m_sex = user->GetUserData().m_sex;
	addmsg1.m_ip = user->GetIp();
	addmsg1.m_face = user->GetUserData().m_headImageUrl;
	addmsg1.m_online = user->GetOnline();
	addmsg1.m_coins = user->GetUserData().m_coins;
	addmsg1.m_credits = user->GetUserData().m_creditValue;
	addmsg1.m_userGps = user->GetUserData().m_customString2;
	for( auto userit = m_users.begin(); userit != m_users.end(); ++userit )
	{
		if( userit->m_userID != user->GetUserDataId() )
		{
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userit->m_userID);
			if(safeUser && safeUser->isValid())
			{
				UserPtr userother = safeUser->getResource();

				LMsgS2CDeskAddUser addmsg2;
				addmsg2.m_userId = userother->GetUserDataId();
				addmsg2.m_pos = userit->m_pos;
				addmsg2.m_nike = userother->GetUserData().m_nike;
				addmsg2.m_ready = userit->m_bReady;
				addmsg2.m_sex = userother->GetUserData().m_sex;
				addmsg2.m_face = userother->GetUserData().m_headImageUrl;
				addmsg2.m_ip = userother->GetIp();
				addmsg2.m_score = 0;
				addmsg2.m_online = userother->GetOnline();
				addmsg2.m_coins = userother->GetUserData().m_coins;
				addmsg2.m_credits = userother->GetUserData().m_creditValue;
				addmsg2.m_userGps = userother->GetUserData().m_customString2;
				user->Send(addmsg2);
				userother->Send(addmsg1);
			}
		}
	}
	// 加入玩家列表
	m_users.push_back( deskuser );
	return true;
}

bool Desk::OnUserOutRoom( UserPtr& user, bool bremove )
{
	// 设置运行时桌子
	user->SetDeskRunID( 0 );

	auto finduserit = m_users.begin();
	while ( finduserit != m_users.end() )
	{
		if ( finduserit->m_userID == user->GetUserDataId() )
		{
			break;
		}
		++finduserit;
	}
	if ( finduserit == m_users.end() )
	{
		LLOG_ERROR( "Desk::OnUserOutRoom not find user, userid=%d", user->GetUserDataId() );
		return false;
	}

	LMsgS2CDeskDelUser del;
	del.m_pos = finduserit->m_pos;
	for( auto userit = m_users.begin(); userit != m_users.end(); ++userit )
	{
		if( userit->m_userID != user->GetUserDataId() )
		{
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userit->m_userID);
			if(safeUser && safeUser->isValid())
			{
				UserPtr userother = safeUser->getResource();
				userother->Send( del );
			}
		}
	}

	// 删除玩家
	if ( bremove )
	{
		m_users.erase( finduserit );
	}
	return true;
}

bool Desk::OnUserReady( UserPtr& user )
{
	auto finduserit = m_users.begin();
	while ( finduserit != m_users.end() )
	{
		if ( finduserit->m_userID == user->GetUserDataId() )
		{
			break;
		}
		++finduserit;
	}
	if ( finduserit == m_users.end() )
	{
		LLOG_ERROR( "Desk::OnUserReady not find user, userid=%d", user->GetUserDataId() );
		return false;
	}

	if ( finduserit->m_bReady )
	{
		LLOG_ERROR( "Desk::OnUserReady user already ready, userid=%d", user->GetUserDataId() );
		return true;
	}

	finduserit->m_bReady = true;

	// 广播玩家准备
	LMsgS2CUserReady ready;
	ready.m_pos = finduserit->m_pos;
	for ( auto userit = m_users.begin(); userit != m_users.end(); ++userit )
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userit->m_userID);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			user->Send( ready );
		}
	}
	return true;
}

void Desk::OnGameOver()
{
	// 先把结算消息发给客户端 和 金币修改
	LTime cur;
	for ( auto userit = m_users.begin(); userit != m_users.end(); ++userit )
	{
		userit->m_bReady = false;
		userit->m_time = cur;
	}

	// 不在线的玩家 和 机器人 直接删掉
	for ( auto userit = m_users.begin(); userit != m_users.end(); )
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userit->m_userID);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			bool bstate = (user->GetUserState() == LGU_STATE_COIN || user->GetUserState() == LGU_STATE_COINDESK);	// 玩家的状态
			if ( !user->GetOnline() || !bstate || user->IsRobot() )
			{
				// 玩家离开桌子
				OnUserOutRoom( user, false );

				user->Logout();
				gUserManager.DelUser( userit->m_userID );

				if ( user->IsRobot() )
				{
					gRobotManager.ReleaseRotot( userit->m_userID );
				}

				m_users.erase( userit++ );
				continue;
			}
		}
		else
		{
			// 可能是在CUserMessage::HanderModifyUserState中把玩家给踢掉了 告诉其他玩家删除这个玩家
			LMsgS2CDeskDelUser del;
			del.m_pos = userit->m_pos;
			for( auto userit2 = m_users.begin(); userit2 != m_users.end(); ++userit2 )
			{
				if( userit2->m_userID != userit->m_userID )
				{
					boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userit2->m_userID);
					if(safeUser && safeUser->isValid())
					{
						UserPtr userother = safeUser->getResource();
						userother->Send( del );
					}
				}
			}
			m_users.erase( userit++ );
			continue;
		}
		++userit;
	}
}

void Desk::CheckReadyOutTime()
{
	Lint outtime = gConfig.GetOpOutTime();
	if ( outtime <= 0 )
	{
		return;
	}
	LTime cur;
	for ( auto userit = m_users.begin(); userit != m_users.end(); )
	{
		if ( !userit->m_bReady && cur.Secs() - userit->m_time.Secs() > (outtime+DIFFOPOUTTIME) )
		{
			// 这个玩家准备超时 踢出去
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userit->m_userID);
			if(safeUser && safeUser->isValid())
			{
				LLOG_ERROR( "Desk::CheckReadyOutTime user ready out time, userid=%d", userit->m_userID );

				UserPtr user = safeUser->getResource();
				// 玩家离开桌子
				OnUserOutRoom( user, false );

				// 这里也要告诉user其他玩家离开了
				for( auto userit2 = m_users.begin(); userit2 != m_users.end(); ++userit2 )
				{
					if( userit2->m_userID != user->GetUserDataId() )
					{
						LMsgS2CDeskDelUser del;
						del.m_pos = userit2->m_pos;
						user->Send( del );
					}
				}

				m_users.erase( userit++ );
				continue;
			}
			else
			{
				LLOG_ERROR( "Desk::CheckReadyOutTime can not find user, userid=%d", userit->m_userID );
				m_users.erase( userit++ );
				continue;
			}
		}
		++userit;
	}
}

void Desk::DismissDesk()
{
	while ( m_users.size() > 0 )
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(m_users.front().m_userID);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			// 先让玩家离开桌子 内部会从m_users上删除
			OnUserOutRoom( user, false );

			// 踢出玩家
			LMsgS2CKickCoinsDesk kick;
			kick.m_reasonCode = 4;
			user->Send( kick );

			user->Logout();
			gUserManager.DelUser( m_users.front().m_userID );

			if ( user->IsRobot() )
			{
				gRobotManager.ReleaseRotot( m_users.front().m_userID );
			}
		}
		m_users.pop_front();
	}
}

bool Desk::OnUserReInRoom( UserPtr& user )
{
	auto finduserit = m_users.begin();
	while ( finduserit != m_users.end() )
	{
		if ( finduserit->m_userID == user->GetUserDataId() )
		{
			break;
		}
	}
	if ( finduserit == m_users.end() )
	{
		return false;
	}
	LMsgS2CIntoDesk send1;
	send1.m_deskId = 0;	// 此时桌子ID为0
	send1.m_pos = finduserit->m_pos;
	send1.m_ready = finduserit->m_bReady;
	send1.m_score = 0;
	send1.m_coins = user->GetUserData().m_coins;
	send1.m_state = (Lint)m_gameType;
	send1.m_maxCircle = 1;
	send1.m_playtype = m_playType;
	send1.m_changeOutTime = gConfig.GetChangeOutTime();
	send1.m_opOutTime = gConfig.GetOpOutTime();
	send1.m_baseScore = gConfig.GetBaseScore();
	send1.m_credits = user->GetUserData().m_creditValue;
	send1.m_userIp = user->GetUserData().m_customString1;
	send1.m_userGps = user->GetUserData().m_customString2;
	user->Send(send1);

	for( auto userit = m_users.begin(); userit != m_users.end(); ++userit )
	{
		if( userit->m_userID != user->GetUserDataId() )
		{
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userit->m_userID);
			if(safeUser && safeUser->isValid())
			{
				UserPtr userother = safeUser->getResource();

				LMsgS2CDeskAddUser addmsg2;
				addmsg2.m_userId = userother->GetUserDataId();
				addmsg2.m_pos = userit->m_pos;
				addmsg2.m_nike = userother->GetUserData().m_nike;
				addmsg2.m_ready = userit->m_bReady;
				addmsg2.m_sex = userother->GetUserData().m_sex;
				addmsg2.m_face = userother->GetUserData().m_headImageUrl;
				addmsg2.m_ip = userother->GetIp();
				addmsg2.m_score = 0;
				addmsg2.m_online = userother->GetOnline();
				addmsg2.m_coins = userother->GetUserData().m_coins;
				addmsg2.m_credits = userother->GetUserData().m_creditValue;
				addmsg2.m_userGps = userother->GetUserData().m_customString2;
				user->Send(addmsg2);
			}
		}
	}
	return true;
}