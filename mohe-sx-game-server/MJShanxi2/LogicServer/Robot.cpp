#include "Robot.h"
#include "Card.h"
#include "Desk.h"
#include "LMsg.h"
Robot::Robot(LUser& data, Lint gateId):User(data, gateId)
{
	mIndex = 0;
}

Robot::~Robot(void)
{
}

void Robot::Tick()
{
	if (m_desk != NULL)
	{
		Lint pos = m_desk->GetUserPos(this);
		if (pos != INVAILD_POS)
		{
			switch (m_desk->getDeskState())
			{
			case DESK_FREE:
				{

				}
			case DESK_WAIT:
				{				     
					if (m_desk->m_selectSeat && m_desk->m_seat[pos] == INVAILD_POS ) //选座位状态
					{
						LLOG_DEBUG("******Enter Robot::Tick() 机器人分配座位 userid = %d******", this->GetUserDataId());
						m_desk->MHProcessRobotSelectSeat(this); //机器人进入
					}
					if (m_desk->m_readyState[pos] != 1)
					{
						LMsgC2SUserReady msg;
						msg.m_pos = pos;
						m_desk->HanderUserReady(this, &msg);
					}
				}
				break;
			case DESK_PLAY:
				{
					//结束房间
					{
						bool bEndRoom = false;
						for (int i = 0; i < DESK_USER_COUNT; i ++)
						{
							if (m_desk->m_reset[i] == RESET_ROOM_TYPE_AGREE)
							{
								bEndRoom = true;
								break;
							}
						}
						if (bEndRoom)
						{
							if (m_desk->m_reset[pos] == RESET_ROOM_TYPE_WAIT)
							{
								LMsgC2SSelectResetDesk msg;
								msg.m_pos = pos;
								msg.m_flag = 1;
								m_desk->HanderSelectResutDesk(this, &msg);
							}	
							return;
						}	
					}			
					//处理机器人逻辑
					if (m_desk->mGameHandler)
					{
						m_desk->mGameHandler->ProcessRobot(pos, this);
					}
				}
				break;
			}			
		}
	}
}

void Robot::SetRobotIndex(Lint index)
{
	mIndex = index;
}

Lint Robot::GetRobotIndex()
{
	return mIndex;
}


