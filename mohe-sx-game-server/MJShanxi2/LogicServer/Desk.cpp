#include "Desk.h"
#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "UserManager.h"
#include "LogicToggles.h"
#include "..\mhmessage\mhmsghead.h"



Desk::Desk()
{
	_clearData();
}

Desk::~Desk()
{

}

bool Desk::initDesk(int iDeskId, GameType gameType)
{
	_clearData();

	switch (gameType) {

#define XX(k, v, player_count) case v: m_iPlayerCapacity = player_count; break;
		ShanXi_GAME_TYPE_MAP(XX)
#undef XX

	default:
		LLOG_ERROR("Desk::initDesk -- Game type is wrong. Type = %d", gameType);
		return false;
	}

	m_desk_user_count = m_iPlayerCapacity;

	if(!_createRegister(gameType))
	{
		return false;
	}

	m_id = iDeskId;

	LTime cur;
	m_timeDeskCreate = cur.Secs();
	MHLOG_DESK("****Create desk id:%d game_type:%d, m_iPlayerCapacity:%d", m_id, gameType, m_iPlayerCapacity);


	return true;
}

Lint Desk::GetDeskId()
{
	return m_id;
}

Lint Desk::GetPlayerCapacity()
{
	return m_iPlayerCapacity;
}

void Desk::Tick(LTime& curr)
{
	CheckReset();

	CheckAutoPlayCard();

	if(m_timeDeskCreate && m_deskState == DESK_WAIT)	//如果超过8小时还没有开始过就进行解散
	{
		if(m_vip == NULL || !m_vip->IsBegin())	//如果没有赋值或者没有开始过
		{
			LTime cur;
			Lint iInterval = cur.Secs() - m_timeDeskCreate;
			if(iInterval >= (2 * 60 * 60))
			{
				if (m_deskCreatedType == 1) {
					LMsgL2LMGDeleteUserCreatedDesk deleteDeskInfo;
					deleteDeskInfo.m_deskId = m_id;
					deleteDeskInfo.m_userid = m_creatUserId;
					if (m_feeType == 0)
					{
						deleteDeskInfo.m_cost = m_cost;
					}
					deleteDeskInfo.m_delType = 2;
					gWork.SendToLogicManager(deleteDeskInfo);
				}

				LLOG_ERROR("The desk(%d) doesn't begin beyond 8 hours", m_id);
				ResetEnd();
			}
		}
		{
			boost::mutex::scoped_lock l(m_mutexDeskLookonUser);
			auto it = m_desk_Lookon_user.begin();
			while (it != m_desk_Lookon_user.end())
			{
				if ((*it)->IsInRoomFor15Second())
				{
					break;
				}
				it++;
			}
			if (it != m_desk_Lookon_user.end())
			{
				LMsgC2SUserSelectSeat msg;
				for (int i = 0; i < m_iPlayerCapacity; i++)
				{
					if (NULL == m_seatUser[i])
					{
						msg.m_pos = i;
						LLOG_ERROR("Desk allocate empty pos for user. desk:%d, user:%d, pos:%d", m_id, (*it)->GetUserDataId(), i);
						this->HanderSelectSeat((*it), &msg);
						break;
					}
				}
			}
		}
	}
}

void Desk::SetAutoPlay( Lint pos, bool bauto, Lint outtime )
{
	m_autoPlay[pos] = bauto;
	if ( bauto )
	{
		LTime cur;
		m_autoPlayTime[pos] = cur.Secs();
		m_autoOutTime[pos] = outtime;
	}
}

void Desk::CheckAutoPlayCard()
{
	if(m_deskType != DeskType_Coins)
		return;

	if(m_deskState == DESK_PLAY )
	{
		LTime cur;
		for(int i = 0; i < m_iPlayerCapacity; ++i)
		{
			if ( !m_user[i] )
			{
				continue;
			}
			Lint outtime = m_autoOutTime[i];
			if ( outtime > 0 )	// <=0 不给玩家自动操作
			{
				if ( !m_user[i]->GetOnline() )	// 玩家不在线倒计时短一些
				{
					outtime = 1;
				}
				else
				{
					outtime += DIFFOPOUTTIME;	// 服务器的时间要长一些
				}
				if ( gConfig.GetDebugModel() && m_user[i]->GetUserDataId() >= 10000001 )	// 机器人倒计时
				{
					outtime = 2;
				}
				if ( m_autoPlay[i] && cur.Secs() - m_autoPlayTime[i] > outtime )
				{
					LLOG_DEBUG("Desk::CheckAutoPlayCard [deskid = %d][pos = %d] auto play card",m_id, i);
					m_autoPlay[i] = false;
					mGameHandler->ProcessAutoPlay(i,m_user[i]);
				}
			}
		}
	}
}

void Desk::SetDeskType( DeskType type )
{
	m_deskType = type;
}

DeskType Desk::GetDeskType()
{
	return m_deskType;
}


void  Desk::SetSelectSeat(bool state)
{
	m_selectSeat = state;
}


Lint  Desk::GetCreatDeskTime()
{
	return m_timeDeskCreate;
}

void  Desk::SetCreatType(Lint type)
{
	m_deskCreatedType = type;
}

Lint  Desk::GetCreatType()
{
	return m_deskCreatedType;
}


//申请解算房间
void Desk::HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg)
{
	LLOG_DEBUG("Logwyz  ...................Desk::HanderResutDesk");
	if (pUser==NULL)return;
	//重复申请
	if (m_resetTime != 0)
	{
		return;
	}

	//Lint pos = GetUserPos(pUser);
	//if(pos == INVAILD_POS)
	//{
	//	return;
	//}
	if ( (!MHIsRoomUser(pUser) )  && (pUser != m_creatUser ))
	{
		LLOG_ERROR("Desk::HanderResutDesk pUser not in room and not m_creatUser,   userid=[%d]",pUser->GetUserDataId());
		return;
	}

	//Lint pos1=GetUserSeatPos(pUser);
	LLOG_ERROR("Desk::HanderResutDesk pUser      userid=[%d],GetUserSeatPos(pUser)=[%d]", pUser->GetUserDataId(), GetUserSeatPos(pUser));
	//if(pos == INVAILD_POS)
	//{
	//	LLOG_ERROR("Desk::HanderResutDesk pUser not in seatUser,   userid=[%d]", pUser->GetUserDataId());
	//	return;
	//}

	if(!m_vip  || m_deskState == DESK_FREE)
	{
		LLOG_ERROR("Desk::HanderResutDesk error desk free");
		return;
	}

	if (m_deskState == DESK_PLAY)
	{
		Lint seatPos=GetUserSeatPos(pUser);
		Lint pos=GetUserPos(pUser);
		if (seatPos==INVAILD_POS)
		{
			//LLOG_ERROR("Desk::HanderResutDesk error m_deskState == DESK_PLAY, userid[%d]  pos[%d],SeatPos[%d]", pUser->GetUserDataId,pos, seatPos);
			return;
		}
		//将桌子状态设置为解散房间状态
		m_deskbeforeState = m_deskState;
		//m_deskState = DESK_RESET_ROOM;

		memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
		m_reset[pos] = RESET_ROOM_TYPE_AGREE;
		m_resetTime = gWork.GetCurTime().Secs() + 3 * 60;
		m_resetUser = pUser->m_userData.m_nike;
		m_resetUserId = pUser->m_userData.m_id;
		for(Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_userId = pUser->m_userData.m_id;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			send.m_userId = pUser->m_userData.m_id;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if (m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);   
					send.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					send.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);  
		}
	}
	else if (m_deskState == DESK_WAIT)
	{
		m_dismissUser = true;
		if (!m_vip->IsBegin())
		{
			LLOG_DEBUG("!m_vip->IsBegin()");
			//这里为什么要==0  ？？？？
			//if(pos == 0)
			//{
				LMsgS2CResetDesk send;
				send.m_errorCode = 1;
				send.m_applay = pUser->m_userData.m_nike;
				send.m_userId = pUser->m_userData.m_id;
				send.m_flag = 1;
				MHBoadCastAllDeskUser(send);
				//BoadCast(send);
				ResetEnd();
			//}
		}
		else
		{
			LLOG_DEBUG("!m_vip->IsBegin()   else ");
			Lint pos1=GetUserPos(pUser);
			memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
			//将桌子状态设置为解散房间状态
			m_deskbeforeState = m_deskState;
			//m_deskState = DESK_RESET_ROOM;

			m_reset[pos1] = RESET_ROOM_TYPE_AGREE;
			m_resetTime = gWork.GetCurTime().Secs() + 3 * 60;
			m_resetUser = pUser->m_userData.m_nike;
			m_resetUserId = pUser->m_userData.m_id;
			for (Lint i = 0; i < m_iPlayerCapacity; ++i)
			{
				if (m_user[i] == NULL)
					continue;

				LMsgS2CResetDesk send;
				send.m_errorCode = 0;
				send.m_applay = m_resetUser;
				send.m_userId = pUser->m_userData.m_id;
				send.m_time = m_resetTime - gWork.GetCurTime().Secs();
				send.m_flag = m_reset[i] ? 1 : 0;
				for (Lint j = 0; j < m_iPlayerCapacity; ++j)
				{
					if (m_user[j] == NULL)
						continue;
					if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
					{
						send.m_agree.push_back(m_user[j]->m_userData.m_nike);
						send.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
						send.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
					}
					else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
					{
						send.m_wait.push_back(m_user[j]->m_userData.m_nike);
						send.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
						send.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
					}
					else
					{
						send.m_refluse = m_user[j]->m_userData.m_nike;
					}
				}
				m_user[i]->Send(send);
			}
		}
	}
}

//玩家选择解散操作
void Desk::HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg)
{
	LLOG_DEBUG("Logwyz  ...................HanderSelectResutDesk");
	if(m_resetTime == 0)
		return;
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		return;
	}

	if (msg->m_flag < 1 || msg->m_flag >2)
	{
		msg->m_flag = 1;
	}

	m_reset[pos] = msg->m_flag;

	Lint agree = 0, refluse = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_reset[i] == RESET_ROOM_TYPE_AGREE)
			agree++;
		else if (m_reset[i] == RESET_ROOM_TYPE_REFLUSE)
			refluse++;
	}

	if (refluse >= 1)
	{
		LLOG_DEBUG("HanderSelectResutDesk333333333333333333333333333");
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 4;
			send.m_applay = m_resetUser;
			send.m_userId = m_resetUserId;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;

				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
					send.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					send.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		LLOG_ERROR(" m_desk[%d],m_deskState=[%d],m_deskbeforeState=[%d]", m_id,m_deskState, m_deskbeforeState);
		//m_deskState = m_deskbeforeState;
		ResetClear();

		//由于网络原因可能某些玩家已经准备但延迟收到解散命令,当有人拒绝的时候应该再次check一下
		LLOG_DEBUG("Logwyz ....  MHCheckGameStart()    380     ");
		MHCheckGameStart();
		//CheckGameStart();
	}
	else if (agree >= (m_desk_user_count))
	{
		LLOG_DEBUG("HanderSelectResutDesk222222222222222222222222222");
		//发送删除信息,删除房主创建的对应的桌子号
		LMsgL2LMGDeleteUserCreatedDesk deleteDeskInfo;
		deleteDeskInfo.m_deskId = m_id;
		deleteDeskInfo.m_userid = m_creatUserId;
		if (m_feeType == 0) 
		{
			deleteDeskInfo.m_cost = m_cost;
		}
		if (m_couFei) {
			deleteDeskInfo.m_delType = 0;
		}
		else {
			deleteDeskInfo.m_delType = 2;
		}

		gWork.SendToLogicManager(deleteDeskInfo);


		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 2;
			send.m_applay = m_resetUser;
			send.m_userId = m_resetUserId;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;

			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
					send.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					send.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}

			m_user[i]->Send(send);
		}
		ResetEnd();
	}
	else
	{
		LLOG_DEBUG("HanderSelectResutDesk111111111111111111111111111111111");
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_userId = m_resetUserId;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			send.m_agreeHeadUrl.clear();
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
					send.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					send.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
	}
}

void Desk::CheckReset()
{
	if (m_resetTime && gWork.GetCurTime().Secs() > m_resetTime)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue; 
			LMsgS2CResetDesk send;
			send.m_errorCode = 3;
			send.m_applay = m_resetUser;
			send.m_userId = m_resetUserId;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					//LLOG_DEBUG("HanderResutDesk  nike = %s ", m_user[j]->m_userData.m_nike.c_str());
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
					//LLOG_DEBUG("HanderResutDesk  headImageUrl = %s ",  m_user[j]->m_userData.m_headImageUrl.c_str());
					send.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					send.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		ResetEnd();
	}
}

void Desk::HanderUserReady(User* pUser,LMsgC2SUserReady* msg)
{
	LLOG_ERROR("Logwyz   ....  Desk::HanderUserReady");
	LMsgS2CUserReady ready;
	Lint pos = GetUserPos(pUser);
	Lint seatID = GetUserSeatPos(pUser);

	if(m_deskState != DESK_WAIT/*&&m_deskState !=DESK_COUNT_RESULT*/)
	{
		LLOG_ERROR("Desk::HanderUserReady state error, userid=%d deskstate=%d", pUser->GetUserDataId(), m_deskState);
		return;
	}

	if(seatID== INVAILD_POS)
	{
		LLOG_ERROR("Desk::HanderUserReady pos error, userid=%d pos=%d,seatID=%d", pUser->GetUserDataId(), pos, seatID);
		return;
	}

	//日志太多暂时屏蔽
	//LLOG_INFO("Desk::HanderUserReady userid=%d pos=%d", pUser->GetUserDataId(), pos);

	//ready.m_pos = pos;
	//准备的人实际的位置
	ready.m_pos=seatID;

	if (seatID!= INVAILD_POS)
	{
		//BoadCast(ready);
		//m_readyState[pos] = 1;
		MHBoadCastAllDeskUser(ready);
		m_readyState[seatID]=1;
	}
	LLOG_DEBUG("Logwyz ....  MHCheckGameStart()    557     ");
	MHCheckGameStart();
}

void Desk::HanderSelectSeat(User* pUser, LMsgC2SUserSelectSeat* msg)
{
	Lint pos1 = GetUserSeatPos(pUser);    //用户在用户队列中的位置
	if (pos1!=INVAILD_POS)
	{
		LLOG_ERROR("Desk::HanderSelectSeat  error, userid=%d  already has seat [%d]", pUser->GetUserDataId(), pos1);
		return;
	}

	if (m_deskState != DESK_WAIT/*&&m_deskState !=DESK_COUNT_RESULT*/)
	{
		LLOG_ERROR("Desk::HanderSelectSeat state error, userid=%d deskstate=%d", pUser->GetUserDataId(), m_deskState);
		return;
	}

	//if (pos == INVAILD_POS)
	//{
	//	LLOG_ERROR("Desk::HanderSelectSeat user pos error, userid=%d pos=%d", pUser->GetUserDataId(), pos);
	//	return;
	//}

	//用户选择的位置
	LLOG_DEBUG("Desk::HanderSelectSeat userid=%d pos=%d", pUser->GetUserDataId(), msg->m_pos);

	if (msg->m_pos < 0 || msg->m_pos >=INVAILD_POS)
	{
		return;
	}

	for (int i = 0; i < 4; i++) {
		if (m_seatUser[i] != NULL && m_seatUser[i] == pUser)
			return;
	}

	if (m_firestUser == NULL) {
		m_firestUser = pUser;
	}
	
	boost::recursive_mutex::scoped_lock l(m_mutexSeat);

	//判断用户选择的位置是否被占用
	if(m_seatUser[msg->m_pos] == NULL)
	{
		LMsgS2CUserSelectSeat selectSeat1;
		//m_seatUser[msg->m_pos] = m_user[pos];  //m_seatUser的Index就是座位号
		m_seatUser[msg->m_pos]=pUser;
		//从lookon用户中退出
		m_desk_Lookon_user.remove(pUser);
		//m_seat[pos] = msg->m_pos;              //用户选择的位置   这个状态在开始游戏以后填写
		//用户选座顺序记录
		m_user_select_seat_turn.push_back(pUser);

		m_readyState[msg->m_pos] = 1;
		selectSeat1.m_id = pUser->GetUserDataId();
		selectSeat1.m_pos = msg->m_pos;
		selectSeat1.m_StartGameButtonPos=INVAILD_POS;

		pUser->ClearInRoomTime();
		LLOG_DEBUG("Logwyz.......m_Greater2CanStart=[%d],m_StartGameButtonPos=[%d]", m_Greater2CanStart, m_StartGameButtonPos);
	
		//发送manager,
		if (m_clubInfo.m_clubId != 0)
		{
			LLOG_DEBUG("Desk::HanderSelectSeat LMsgL2LMGUserAddClubDesk");
			LMsgL2LMGUserAddClubDesk  send;
			send.m_clubId = m_clubInfo.m_clubId;
			send.m_playTypeId = m_clubInfo.m_playTypeId;
			send.m_clubDeskId = m_clubInfo.m_clubDeskId;
			//send.m_userId = pUser->GetUserDataId();
			send.m_strUUID = pUser->m_userData.m_unioid;
			//send.m_pos = msg->m_pos;

			gWork.SendToLogicManager(send);
		}

		//>=2可以开局
		if (m_Greater2CanStart&&MHGetSeatUserCount()!=m_iPlayerCapacity)
		{
			if (m_StartGameButtonPos==INVAILD_POS)
			{
				if (MHCheckGameStart()==1)  //可以开始游戏
				{
					if (m_deskCreatedType==0)
					{
						if (GetUserSeatPos(m_creatUser)!=INVAILD_POS)
						{
							//开始游戏，指定开始按钮用户为房主
							m_StartGameButtonPos=GetUserSeatPos(m_creatUser);
							selectSeat1.m_StartGameButtonPos=m_StartGameButtonPos;
						}
					}
					else
					{
						//开始游戏，指定开始按钮用户
						m_StartGameButtonPos=MHSpecPersonPos();
						selectSeat1.m_StartGameButtonPos=m_StartGameButtonPos;
						LLOG_DEBUG("Logwyz.......select m_StartGameButtonPos=[%d]", m_StartGameButtonPos);
					}
				}
			}
			else
				selectSeat1.m_StartGameButtonPos=m_StartGameButtonPos;

			 //广播用户选择位置
			MHBoadCastAllDeskUser(selectSeat1);

		}
		else                	//规定人数开局
		{
			//广播用户选择位置
			MHBoadCastAllDeskUser(selectSeat1);
			MHCheckGameStart();
		}
		
		if (m_Greater2CanStart&&MHGetSeatUserCount() == m_iPlayerCapacity)
		{
			MHHanderStartGame();
		}


		//BoadCast(selectSeat1);                  //广播用户选择位置
		//MHBoadCastAllDeskUser(selectSeat1);

		LLOG_DEBUG("Desk::HanderSelectSeat after selected userid=%d pos=%d", pUser->GetUserDataId(), msg->m_pos);

		// 机器人随机分配到其他座位放在这里？
		/*
			LLOG_DEBUG("******************Desk::HanderSelectSeat 开始处理机器人位置选择****************");
			if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
			{
				std::queue<int> robotPos;
				for (int i = 0; i < DESK_USER_COUNT; i++)
				{
					if (i != msg->m_pos)
					{
						robotPos.push(i);
					}
				}
				for (int i = 0; i < this->m_desk_user_count; i++)
				{
					User * pTableUser = this->GetPosUser(i); //队列中用户, i是用户的队列位置
					if (pTableUser && (pUser != pTableUser) && (pTableUser->getUserGateID() > 10000)) //机器人
					{
						// 寻找一个空的位置
						int selectPos = robotPos.front();
						m_seat[i] = selectPos;
						selectSeat.m_id = pTableUser->GetUserDataId();
						selectSeat.m_pos = selectPos;
						BoadCast(selectSeat);
						m_readyState[i] = 1;
						m_seatUser[selectPos] = m_user[i];
						robotPos.pop(); // 去掉占用的位置
						LLOG_DEBUG("*****************Robot UserID: %d,  选择位置 %d, rebotPos.size() = %d****************", pTableUser->GetUserDataId(), selectPos, robotPos.size());

					}
				}
			}
			*/
	}
	else {
		LMsgS2CUserSelectSeat selectSeat2;
		selectSeat2.m_id = pUser->GetUserDataId();
		selectSeat2.m_pos = INVAILD_POS;
		LLOG_DEBUG("Desk::HanderSelectSeat after selected userid=%d pos=%d", pUser->GetUserDataId(), selectSeat2.m_pos);
		pUser->Send(selectSeat2);
	}

}


void Desk::HanderUserOutDesk(User* pUser)
{
	LLOG_DEBUG("Logwyz   Desk::HanderUserOutDesk");
	//Lint pos = GetUserPos(pUser);
	//if(pos == INVAILD_POS)
	//{
	//	return;
	//}
	if (pUser==NULL)return;
	if (!MHIsRoomUser(pUser))
	{
		LLOG_DEBUG("Logwyz   Desk::HanderUserOutDesk   user[%d] not in room",pUser->GetUserDataId());
		return;
	}

	//谁回来了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 0;
	//if (m_selectSeat) {
	//	info.m_pos = m_seat[pos];
	//}
	//else {
	//	info.m_pos = pos;
	//}
	info.m_pos=GetUserSeatPos(pUser);
	info.m_userid = pUser->m_userData.m_id;
	MHBoadCastAllDeskUserWithOutUser(info, pUser);
}

void Desk::HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg)
{
	if(m_deskPlayState != DESK_PLAY_END_CARD)
	{
		LLOG_DEBUG("Desk::HanderUserEndSelect state error %d:%d:%d",m_deskState,m_deskPlayState,msg->m_flag);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserEndSelect(pUser, msg);
	}
}

void Desk::HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg)
{
	LLOG_DEBUG("Desk::HanderUserStartHu %d:%d",msg->m_type,msg->m_card.size());

	if(m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_START_HU)
	{
		LLOG_DEBUG("Desk::HanderUserStartHu state error %d:%d:%d",m_deskState,m_deskPlayState,msg->m_type);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserStartHu(pUser, msg);
	}
}

void Desk::HanderUserTangReq( User* pUser,LMsgC2STangCard* msg )
{
	if(mGameHandler)
		mGameHandler->HanderUserTangReq(pUser,msg);
}

void Desk::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
{
	if(m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_GET_CARD)
	{
		//pUser->Send(sendMsg);
		LLOG_ERROR("Desk::HanderUserPlayCard state error %s ,m_deskState = %d m_deskPlayState = %d",pUser->m_userData.m_nike.c_str(),m_deskState, m_deskPlayState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserPlayCard(pUser, msg);
	}
}

void Desk::HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg)
{
	if (m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_THINK_CARD)
	{
		LLOG_DEBUG("Desk::HanderUserOperCard state error %s, m_deskState = %d m_deskPlayState = %d", pUser->m_userData.m_nike.c_str(), m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserOperCard(pUser, msg);
	}
}

void Desk::HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg)
{
	LLOG_DEBUG("Logwyz Desk::HanderUserSpeak ");
	if(mGameHandler)
	{
		mGameHandler->HanderUserSpeak(pUser,msg);
	}

	//Lint pos = GetUserPos(pUser);
	//if (pos == INVAILD_POS)
	//{
	//	return;
	//}
	if (!MHIsRoomUser(pUser))
	{
		LLOG_DEBUG("Logwyz Desk::HanderUserSpeak   user not in room");
		return;
	}


	if (msg->m_msg == "555666+") {
		LMsgS2CUserSpeak speak;
		//speak.m_userId = m_user[pos]->GetUserDataId();
		//speak.m_pos = pos;
		//if (m_selectSeat) {
		//	speak.m_pos = m_seat[pos];
		//}
		//speak.m_pos = pos;
		//if (speak.m_pos == INVAILD_POS) 
		//	return;
		speak.m_userId=pUser->GetUserDataId();
		speak.m_pos=GetUserSeatPos(pUser);
		if (speak.m_pos==INVAILD_POS)
			return;
		speak.m_id = msg->m_id;
		speak.m_type = msg->m_type;
		speak.m_musicUrl = msg->m_musicUrl;
		speak.m_msg = __DATE__ " " __TIME__;
		pUser->Send(speak);
		return;
	}

	LMsgS2CUserSpeak speak;
	//speak.m_userId = m_user[pos]->GetUserDataId();
	speak.m_userId=pUser->GetUserDataId();
	speak.m_pos = GetUserSeatPos(pUser);
	//if (m_selectSeat) {
	//	speak.m_pos = m_seat[pos];
	//}

	if (speak.m_pos == INVAILD_POS)
		return;

	speak.m_id = msg->m_id;
	speak.m_type = msg->m_type;
	speak.m_musicUrl = msg->m_musicUrl;
	speak.m_msg = msg->m_msg;
	//BoadCast(speak);
	MHBoadCastAllDeskUser(speak);
}

bool Desk::OnUserReconnect(User* pUser)
{
	//Lint pos = GetUserPos(pUser);
	//if (pos == INVAILD_POS)
	//{
	//	LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
	//	return false;
	//}
	if (pUser==NULL)return false;
	if (!MHIsRoomUser(pUser))
	{
		LLOG_ERROR("Desk::OnUserReconnect [%d] not in room", pUser->GetUserDataId());
		return false;
	}
  
	Lint reUserPos=GetUserSeatPos(pUser);

	//m_readyState[pos] = 1;
	if(m_deskState == DESK_WAIT && reUserPos!=INVAILD_POS)	//如果是等待状态回来时需要把准备
	{
		LMsgS2CUserReady ready;
		ready.m_pos =reUserPos;
		//if (m_selectSeat) {
		//	ready.m_pos = m_seat[pos];
		//}
		
		//BoadCast(ready);
		MHBoadCastAllDeskUser(ready);
		m_readyState[reUserPos]=1;
	}

	//把自己加进来
	LMsgS2CIntoDesk send1;
	send1.m_deskId=m_id;
	//俱乐部
	if (m_clubInfo.m_clubId!=0&&m_clubInfo.m_clubDeskId!=0&&m_clubInfo.m_showDeskId!=0)
		send1.m_deskId=m_clubInfo.m_showDeskId;
	
	//send1.m_pos = pos;
	//if (m_selectSeat) {
	//	send1.m_pos = m_seat[pos];
	//}
	//else {
	//	send1.m_pos = pos;
	//}
	//send1.m_pos = m_seat[pos];
	send1.m_pos=reUserPos;
	if(reUserPos!=INVAILD_POS)
		send1.m_ready = m_readyState[reUserPos];
	else
		send1.m_ready=0;
	//send1.m_ready=(reUserPos!=INVAILD_POS)?m_readyState[reUserPos]:0;
	send1.m_score = m_vip ? m_vip->GetUserScore(pUser) : 0;
	send1.m_coins = pUser->GetUserData().m_coins;
	send1.m_state = m_state;
	send1.m_maxCircle = m_vip ? m_vip->m_maxCircle : 0;
	send1.m_playtype = m_playtype;
	send1.m_changeOutTime = m_autoChangeOutTime;
	send1.m_opOutTime = m_autoPlayOutTime;
	send1.m_baseScore = m_baseScore;
	send1.m_credits = pUser->m_userData.m_creditValue;
	send1.m_cellscore = m_cellscore;
	send1.m_flag = m_flag;
	send1.m_feeType = m_feeType;
	send1.m_cheatAgainst = m_cheatAgainst;
	send1.m_deskType = m_deskType;
	send1.m_userIp = pUser->m_userData.m_customString1;
	send1.m_userGps = pUser->m_userData.m_customString2;
	send1.m_createUserId = m_creatUserId;
	send1.m_deskCreatedType = m_deskCreatedType;
	send1.m_Greater2CanStart=m_Greater2CanStart;
	send1.m_StartGameButtonPos=m_StartGameButtonPos;
	send1.m_gamePlayerCount=m_desk_user_count;
	send1.m_startButtonAppear=m_startButtonAppear;
	send1.m_clubId=m_clubInfo.m_clubId;
	send1.m_playTypeId=m_clubInfo.m_playTypeId;
	LLOG_DEBUG("OnUserReconnect send1.m_deskCreatedType = %d,m_StartGameButtonPos=[%d],m_desk_user_count=[%d],m_startButtonAppear=[%d],m_clubId=[%d],m_playTypeId=[%d]", send1.m_deskCreatedType, m_StartGameButtonPos, m_desk_user_count, m_startButtonAppear, send1.m_clubId, send1.m_playTypeId);
	LLOG_DEBUG("OnUserReconnect send1.m_userIp = %s", send1.m_userIp.c_str());
	pUser->Send(send1);

	/*
 	//把其他人加进来
 	for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != pUser )
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId();
			addmsg2.m_pos = i;
			if (m_selectSeat) {
				addmsg2.m_pos = m_seat[i];
			}

 			//addmsg2.m_pos = m_seat[i];
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->m_userData.m_customString1;
			addmsg2.m_score = m_vip ? m_vip->GetUserScore(m_user[i]) : 0;
			addmsg2.m_online = m_user[i]->GetOnline(); 
			addmsg2.m_coins = m_user[i]->GetUserData().m_coins;
			addmsg2.m_credits = m_user[i]->m_userData.m_creditValue;
			addmsg2.m_userGps = m_user[i]->m_userData.m_customString2;
			addmsg2.m_cheatAgainst = m_cheatAgainst;
			addmsg2.m_videoPermission = m_user[i]->m_videoPermission;
 			pUser->Send(addmsg2);
 		}
 	}
	*/
	//把其他座位人加进来
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i]!=NULL && m_seatUser[i]!=pUser)
		{
			LMsgS2CDeskAddUser addmsg2;
			addmsg2.m_userId=m_seatUser[i]->GetUserDataId();
			addmsg2.m_pos=i;
			//if (m_selectSeat) {
			//	addmsg2.m_pos=m_seat[i];
			//}

			//addmsg2.m_pos = m_seat[i];
			addmsg2.m_nike=m_seatUser[i]->m_userData.m_nike;
			addmsg2.m_ready=m_readyState[i];
			addmsg2.m_sex=m_seatUser[i]->m_userData.m_sex;
			addmsg2.m_face=m_seatUser[i]->m_userData.m_headImageUrl;
			addmsg2.m_ip=m_seatUser[i]->m_userData.m_customString1;
			addmsg2.m_score=m_vip?m_vip->GetUserScore(m_seatUser[i]):0;
			addmsg2.m_online=m_seatUser[i]->GetOnline();
			addmsg2.m_coins=m_seatUser[i]->GetUserData().m_coins;
			addmsg2.m_credits=m_seatUser[i]->m_userData.m_creditValue;
			addmsg2.m_userGps=m_seatUser[i]->m_userData.m_customString2;
			addmsg2.m_cheatAgainst=m_cheatAgainst;
			addmsg2.m_videoPermission=m_seatUser[i]->m_videoPermission;
			pUser->Send(addmsg2);
		}
	}

	//把Lookon用户加进来
	std::list<User*>::iterator userIt;
	for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); ++userIt)
	{
		User* LookonUser=*userIt;
		if (LookonUser==NULL ||pUser==LookonUser)continue;

		LMsgS2CDeskAddUser addmsg2;
		addmsg2.m_userId=LookonUser->GetUserDataId();
		addmsg2.m_pos=INVAILD_POS;
		addmsg2.m_nike=LookonUser->m_userData.m_nike;
		addmsg2.m_ready=0;
		addmsg2.m_sex=LookonUser->m_userData.m_sex;
		addmsg2.m_face=LookonUser->m_userData.m_headImageUrl;
		addmsg2.m_ip=LookonUser->m_userData.m_customString1;
		addmsg2.m_score=m_vip?m_vip->GetUserScore(LookonUser):0;
		addmsg2.m_online=LookonUser->GetOnline();
		addmsg2.m_coins=LookonUser->GetUserData().m_coins;
		addmsg2.m_credits=LookonUser->m_userData.m_creditValue;
		addmsg2.m_userGps=LookonUser->m_userData.m_customString2;
		addmsg2.m_cheatAgainst=m_cheatAgainst;
		addmsg2.m_videoPermission=LookonUser->m_videoPermission;
		pUser->Send(addmsg2);

	}

	//谁回来了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 1;
	//if (m_selectSeat) {
	//	info.m_pos = m_seat[pos];
	//}
	//else {
	//	info.m_pos = pos;
	//}
	info.m_pos=GetUserSeatPos(pUser);
	info.m_userid = pUser->m_userData.m_id;
	//info.m_pos = pos;
	//BoadCastWithOutUser(info, pUser);
	MHBoadCastAllDeskUserWithOutUser(info, pUser);

	//不在游戏中，不需要后面的数据
	if(m_deskState == DESK_WAIT)
	{
		LLOG_DEBUG("Logwyz ....  MHCheckGameStart()    981     ");
		MHCheckGameStart();
		//CheckGameStart();
	}
	else
	{
		//发送当前圈数信息
		if(m_vip)
		{
			m_vip->SendInfo();
		}

		if(mGameHandler)
		{
			mGameHandler->OnUserReconnect(pUser);
		}
	}

	//如果有人申请解散则通知解散信息
	if(m_resetTime)
	{
		// 如果是lookon用户，解散房间不需要他们同意或者拒绝
		if (!MHIsLookonUser(pUser))
		{
			LMsgS2CResetDesk send;
			send.m_errorCode=0;
			send.m_applay=m_resetUser;
			send.m_userId=m_creatUserId;
			send.m_time=m_resetTime-gWork.GetCurTime().Secs();
			//解散的时候是游戏已经开始，直接用m_user的位置
			Lint userPos=GetUserPos(pUser);
			send.m_flag=m_reset[userPos]?1:0;
			for (Lint j=0; j<m_iPlayerCapacity; ++j)
			{
				if (m_user[j]==NULL)
					continue;
				if (m_reset[j]==RESET_ROOM_TYPE_AGREE)
				{
					//LLOG_DEBUG("OnUserReconnect  nike = %s ", m_user[j]->m_userData.m_nike.c_str());
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
					//LLOG_DEBUG("OnUserReconnect  headImageUrl = %s ",  m_user[j]->m_userData.m_headImageUrl.c_str());
					send.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j]==RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					send.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					send.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					send.m_refluse=m_user[j]->m_userData.m_nike;
				}
			}
			pUser->Send(send);
		}
		else
		{
			//解散时候lookon用户需要做什么
		}
	}

	LLOG_ERROR("**************断线重连桌子ID: %d ", m_id);
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i]) {
			LLOG_ERROR("********** 用户ID = %d IP = %s", m_user[i]->GetUserData().m_id, m_user[i]->m_userData.m_customString1.c_str());
		}
	}

	return true;
}

// 玩家换牌
void Desk::HanderUserChange(User* pUser, LMsgC2SUserChange* msg)
{
	if (mGameHandler)
	{
		mGameHandler->HanderUserChange(pUser, msg);
	}
}

void Desk::SetVip(VipLogItem* vip)
{
	m_vip = vip;
	if(m_vip)
	{
		m_state = vip->m_state;
		m_playtype = vip->m_playtype;
		mGameHandler->SetPlayType(vip->m_playtype);
	}
}

VipLogItem* Desk::GetVip()
{
	return m_vip;
}

Lint Desk::GetFreePos()
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] == NULL)
			return i;
	}

	return INVAILD_POS;

}

void Desk::OnUserInRoom(User* user)
{
	//if (GetUserPos(user) != INVAILD_POS)
	//{
	//	LLOG_ERROR("Desk::OnUserInRoom is in desk %d", user->GetUserDataId());
	//	return;
	//}
	if (MHIsRoomUser(user))
	{
		LLOG_ERROR("Desk::OnUserInRoom is in desk %d", user->GetUserDataId());
		return;
	}

	//关于几个变量说明 by wyz： m_user  , m_seatUser，m_desk_Lookon_user     用户进入房间只写入m_desk_all_user，m_desk_Lookon_user 这两个变量；用户选座成功以后用户写入m_seatUser（东南西北实际位置），从m_desk_Lookon_user移除；游戏开始时，写入m_user（位置与实际位置无关，按东南西北依次排）,m_switchPos位置记录转换关系  by wyz

	//Lint pos = GetFreePos();
	//if(pos == INVAILD_POS)
	//{
	//	LLOG_ERROR("Desk::OnUserInRoom INVAILD_POS %d",user->GetUserDataId());
	//	return;
	//}
	Lint pos=INVAILD_POS;

	m_selectSeat = true;
	//m_user[pos] = user;
	//没有选座的用户都是lookon用户                
	//m_desk_all_user.push_back(user);
	m_desk_Lookon_user.push_back(user);

	user->SetDesk(this);
	user->updateInRoomTime();
	//m_readyState[pos] = 0;
	//Lint realSeat = m_seat[pos];
	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	//俱乐部
	if (m_clubInfo.m_clubId!=0&&m_clubInfo.m_clubDeskId!=0&&m_clubInfo.m_showDeskId!=0)
		send1.m_deskId=m_clubInfo.m_showDeskId;

	send1.m_pos = pos;
	//if (m_selectSeat) {
	//	send1.m_pos = realSeat;
	//}

	//send1.m_ready = m_readyState[pos];
	send1.m_ready=0;
	send1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	send1.m_coins = user->GetUserData().m_coins;
	send1.m_state = m_state;
	send1.m_maxCircle = m_vip ? m_vip->m_maxCircle : 0;
	send1.m_playtype = m_playtype;
	send1.m_changeOutTime = m_autoChangeOutTime;
	send1.m_opOutTime = m_autoPlayOutTime;
	send1.m_baseScore = m_baseScore;
	send1.m_credits = user->m_userData.m_creditValue;
	send1.m_createUserId = m_creatUserId;
	send1.m_deskCreatedType = m_deskCreatedType;
	send1.m_cellscore = m_cellscore;

	send1.m_flag = m_flag;
	send1.m_feeType = m_feeType;
	send1.m_cheatAgainst = m_cheatAgainst;
	send1.m_deskType = m_deskType;
	send1.m_userIp = user->m_userData.m_customString1;
	send1.m_userGps = user->m_userData.m_customString2;
	send1.m_Greater2CanStart=m_Greater2CanStart;
	send1.m_StartGameButtonPos=m_StartGameButtonPos;
	send1.m_gamePlayerCount=m_desk_user_count;
	send1.m_startButtonAppear=m_startButtonAppear;
	send1.m_clubId=m_clubInfo.m_clubId;
	send1.m_playTypeId=m_clubInfo.m_playTypeId;
	send1.m_GpsLimit = this->m_Gps_Limit;
	LLOG_DEBUG("OnUserReconnect send1.m_deskCreatedType = %d,m_StartGameButtonPos=[%d],m_Greater2CanStart[%d],m_desk_user_count=[%d],m_startButtonAppear=[%d],m_clubId=[%d],m_playTypeId=[%d]", send1.m_deskCreatedType, m_StartGameButtonPos, m_StartGameButtonPos, m_desk_user_count, m_startButtonAppear, send1.m_clubId, send1.m_playTypeId);

	LLOG_DEBUG("OnUserReconnect send1.m_userIp = %s, LMsgS2CIntoDesk.m_GpsLimit:%d", send1.m_userIp.c_str(), send1.m_GpsLimit);
	user->Send(send1);

 	LMsgS2CDeskAddUser addmsg1;
 	addmsg1.m_userId = user->GetUserDataId();
 	addmsg1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
 
	addmsg1.m_pos = pos;  
	//if (m_selectSeat) {
	//	addmsg1.m_pos = realSeat;
	//}
	LLOG_DEBUG("*******************用户加入房间的位置 msgid = %d userid = %d pos = %d", addmsg1.m_msgId, addmsg1.m_userId, addmsg1.m_pos);
 	addmsg1.m_nike = user->m_userData.m_nike;
 	//addmsg1.m_ready = m_readyState[pos];
	addmsg1.m_ready=0;
 	addmsg1.m_sex = user->m_userData.m_sex;
 	addmsg1.m_ip = user->m_userData.m_customString1;
 	addmsg1.m_face = user->m_userData.m_headImageUrl;
	addmsg1.m_online = user->GetOnline();
	addmsg1.m_coins = user->GetUserData().m_coins;
	addmsg1.m_credits = user->m_userData.m_creditValue;
	addmsg1.m_userGps = user->m_userData.m_customString2;
	addmsg1.m_cheatAgainst = m_cheatAgainst;
	addmsg1.m_videoPermission = user->m_videoPermission;

	//在座的人
 	for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
 	{
 		if(m_seatUser[i] != NULL && m_seatUser[i] != user)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId =m_seatUser[i]->GetUserDataId();
			//Lint realSeat = m_seat[i];
 			addmsg2.m_pos = i;
			//if (m_selectSeat) {
			//	addmsg2.m_pos = realSeat;
			//}
 			addmsg2.m_nike =m_seatUser[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex =m_seatUser[i]->m_userData.m_sex;
 			addmsg2.m_face =m_seatUser[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip =m_seatUser[i]->GetIp();
			addmsg2.m_score = m_vip ? m_vip->GetUserScore(m_seatUser[i]) : 0;
			addmsg2.m_online =m_seatUser[i]->GetOnline();
			addmsg2.m_coins =m_seatUser[i]->GetUserData().m_coins;
			addmsg2.m_credits =m_seatUser[i]->m_userData.m_creditValue;
			addmsg2.m_userGps =m_seatUser[i]->m_userData.m_customString2;
			addmsg2.m_cheatAgainst = m_cheatAgainst;
			addmsg2.m_videoPermission =m_seatUser[i]->m_videoPermission;
 			user->Send(addmsg2);
			m_seatUser[i]->Send(addmsg1);
 		}
 	}

	//发送Lookon用户的
	LLOG_DEBUG("Logwyz-------------m_desk_Lookon_user=[%d]", m_desk_Lookon_user.size());
	std::list<User*>::iterator userIt;
	for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); ++userIt)
	{
		User* temp=*userIt;
		if (temp==NULL)continue;
		if (temp!=user)
		{
			LMsgS2CDeskAddUser addmsg2;
			addmsg2.m_userId=temp->GetUserDataId();
			addmsg2.m_pos=INVAILD_POS;
			addmsg2.m_nike=temp->m_userData.m_nike;
			addmsg2.m_ready=0;
			addmsg2.m_sex=temp->m_userData.m_sex;
			addmsg2.m_face=temp->m_userData.m_headImageUrl;
			addmsg2.m_ip=temp->GetIp();
			addmsg2.m_score=m_vip?m_vip->GetUserScore(temp):0;
			addmsg2.m_online=temp->GetOnline();
			addmsg2.m_coins=temp->GetUserData().m_coins;
			addmsg2.m_credits=temp->m_userData.m_creditValue;
			addmsg2.m_userGps=temp->m_userData.m_customString2;
			addmsg2.m_cheatAgainst=m_cheatAgainst;
			addmsg2.m_videoPermission=temp->m_videoPermission;
			user->Send(addmsg2);
			temp->Send(addmsg1);
		}
	}

	//如果人满,发送manager,更新俱乐部界面
	if (m_clubInfo.m_clubId!=0&&MHGetDeskUserCount()==m_desk_user_count)
	{
		MHNotifyManagerDeskInfo(1);
	}



	//发送manager,   添加用户
	//if (m_clubInfo.m_clubId!=0)
	//{
	//	LLOG_DEBUG("Desk::OnUserInRoom LMsgL2LMGUserAddClubDesk");
	//	LMsgL2LMGUserAddClubDesk  send;
	//	send.m_clubId=m_clubInfo.m_clubId;
	//	send.m_playTypeId=m_clubInfo.m_playTypeId;
	//	send.m_clubDeskId=m_clubInfo.m_clubDeskId;
	//	send.m_userId=user->GetUserDataId();
	//	send.m_strUUID=user->m_userData.m_unioid;
	//
	//	gWork.SendToLogicManager(send);
	//}
	//CheckGameStart();
}

void Desk::OnUserInRoom(User* user[])
{
	LLOG_DEBUG("Desk::OnUserInRoom(User* user[])");
	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		Lint pos = GetFreePos();
		if(pos == INVAILD_POS)
		{
			LLOG_ERROR("Desk::OnUserInRoom INVAILD_POS %d",user[0]->GetUserDataId());
			return;
		}

		m_user[pos] = user[i];
		user[i]->SetDesk(this);
		m_readyState[pos] = 1;
	}

	CheckGameStart();
}

void Desk::OnUserOutRoom(User* user)
{
	boost::mutex::scoped_lock(m_mutexDeskLookonUser);
	if (user==NULL)
		return;
	LLOG_DEBUG("Desk::OnUserOutRoom,     userid=[%d]", user->GetUserDataId());

 	//Lint pos = GetUserPos(user);
 	//if(pos == INVAILD_POS)
 	//{
 	//	LLOG_ERROR("Desk::OnUserOutRoom INVAILD_POS %d",user->GetUserDataId());
 	//	return;
 	//}

	//if(GetUserPos(user)!= INVAILD_POS)
	//{
	//	LLOG_ERROR("Desk::OnUserOutRoom INVAILD_POS %d",user->GetUserDataId());
	//	return;
	//}

	if (!MHIsRoomUser(user))
	{
		LLOG_ERROR("Desk::OnUserOutRoom  userid[%d] not in room", user->GetUserDataId());
		return;
	}

 	LMsgS2CDeskDelUser del;
	if (m_dismissUser) {
		del.m_DismissName = m_creatUserNike;
	}
	del.m_userId = user->m_userData.m_id;
	if (MHIsSeatUser(user))
		del.m_pos=GetUserSeatPos(user);
	else
		del.m_pos=INVAILD_POS;

	del.m_StartGameButtonPos=m_StartGameButtonPos;


	//重新选择第一个选座的人
	if (m_firestUser==user)m_firestUser=NULL;
	if (MHIsInSeatTurn(user))
	{
		LLOG_DEBUG("Logwyz ...  Desk::OnUserOutRoom  user[%d] out , m_user_select_seat_turn.remove  ", user->GetUserDataId());
		m_user_select_seat_turn.remove(user);
	}

	LLOG_DEBUG("Logwyz ...  Desk::OnUserOutRoom  user[%d] out ,   m_StartGameButtonPos=[%d] ", user->GetUserDataId(),m_StartGameButtonPos);

	//开始删除桌子上用户信息
	bool  userIsLookOnUser=false;
	int delUserPos=GetUserPos(user);
	if (delUserPos!=INVAILD_POS)
		m_user[delUserPos]=NULL;
	
	int delUserSeatPos=GetUserSeatPos(user);
	if (delUserSeatPos!=INVAILD_POS) {
		m_seatUser[delUserSeatPos]=NULL;
		m_readyState[delUserSeatPos]=0;
	}
	else
	{
		m_desk_Lookon_user.remove(user);
		userIsLookOnUser=true;
	}

	//退出用户是否是开始按钮用户,如果不是，检查入座人数
	if (m_StartGameButtonPos!=INVAILD_POS &&(!userIsLookOnUser) && (delUserSeatPos==m_StartGameButtonPos ||  MHGetSeatUserCount()<2))
	{
		LLOG_DEBUG("Logwyz ...  Desk::OnUserOutRoom  user[%d] out ,  userPos=[%d],seatCount=[%d],m_StartGameButtonPos=[%d] ", user->GetUserDataId(), GetUserSeatPos(user), MHGetSeatUserCount(),m_StartGameButtonPos);
		if (MHGetSeatUserCount()>=2)
		{
			m_StartGameButtonPos=MHSpecPersonPos();
			del.m_StartGameButtonPos=m_StartGameButtonPos;

		}
		else
		{
			m_StartGameButtonPos=INVAILD_POS;
			del.m_StartGameButtonPos=m_StartGameButtonPos;
		}
	}
		
 	//for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
 	//{
 	//	if(m_user[i] != NULL)
 	//	{
 	//		m_user[i]->Send(del);
 	//	}
	//}
	//向房间所有人广播
	MHBoadCastAllDeskUser(del);
	user->Send(del);

	//修改状态
	user->setUserState(LGU_STATE_CENTER);
	user->ModifyUserState(true);

	//清除状态
	user->SetDesk(NULL);
	////开始删除用户  ,移到上面
	//int delUserPos=GetUserPos(user);
	//if (delUserPos!=INVAILD_POS)
	//	m_user[delUserPos]=NULL;
	//
	//int delUserSeatPos=GetUserSeatPos(user);
	//if (delUserSeatPos!=INVAILD_POS) {
	//	m_seatUser[delUserSeatPos]=NULL;
	//	m_readyState[delUserSeatPos]=0;
	//}
	//else
	//{
	//	m_desk_Lookon_user.remove(user);
	//}

	//发送manager
	if (m_clubInfo.m_clubId!=0)
	{
		LLOG_DEBUG("Desk::OnUserOutRoom LMsgL2LMGUserLeaveClubDesk");
		LMsgL2LMGUserLeaveClubDesk  send;
		send.m_clubId=m_clubInfo.m_clubId;
		send.m_playTypeId=m_clubInfo.m_playTypeId;
		send.m_clubDeskId=m_clubInfo.m_clubDeskId;
		send.m_userId=user->GetUserDataId();
		send.m_strUUID=user->m_userData.m_unioid;

		gWork.SendToLogicManager(send);
	}

	//如果人满,发送manager,更新俱乐部界面
	if (m_clubInfo.m_clubId!=0&&MHGetDeskUserCount()==m_desk_user_count-1)
	{
		MHNotifyManagerDeskInfo(0);
	}

	
	//if (m_seat[pos] != INVAILD_POS) {
	//	m_seatUser[m_seat[pos]] = NULL;
	//}

	//m_seat[pos] = INVAILD_POS;

	//删除玩家 不删除机器人
	if (user->getUserGateID() != 65535)		//不是机器人删除掉
	{
		UserManager::Instance().DelUser(user);
		delete user;
	}

}

Lint Desk::GetNextPos(Lint prev)
{
	if(prev < 0 || prev >= m_iPlayerCapacity)
	{
		return INVAILD_POS;
	}

	return prev == (m_iPlayerCapacity - 1) ? 0 : (prev + 1);
}

Lint Desk::GetPrevPos(Lint next)
{
	if(next < 0 || next >= m_iPlayerCapacity)
	{
		return INVAILD_POS;
	}

	return next == 0 ? (m_iPlayerCapacity - 1) : (next - 1);
}

Lint Desk::GetUserPos(User* user)
{
	if(user == NULL)
	{
		return INVAILD_POS;
	}

	for(Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if(m_user[i] == user)
			return i;
	}

	return INVAILD_POS;
}

Lint Desk::GetUserSeatPos(User* user)
{
	if (user == NULL)
	{
		return INVAILD_POS;
	}

	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i] == user)
			return i;
	}

	return INVAILD_POS;
}

User* Desk::GetPosUser(Lint pos)
{
	if(pos<0 || pos >= INVAILD_POS)
	{
		return NULL;
	}

	return m_user[pos];
}

Lint Desk::GetUserCount()
{
	Lint cnt = 0;
	for(Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if(m_user[i])
		{
			cnt += 1;
		}
	}

	return cnt;
}


void Desk::SetDeskWait()
{
	memset(m_readyState, 0, sizeof(m_readyState));
	m_deskState = DESK_WAIT;
}

void Desk::OnDeskWait()
{
	
}

void Desk::CheckGameStart()
{
	MHLOG("*********************************************Desk::CheckGameStart()");
	if(m_deskState != DESK_WAIT)	//只有桌子为等待状态才继续
	{
		LLOG_DEBUG("Desk's status is wrong. Desk state = %d.", m_deskState);
		return;
	}

	if(m_resetTime != 0)
	{
		LLOG_DEBUG("Desk ising reseted");
		return;
	}

	//判断所有的人是否都已经准备
	if(GetUserCount() != m_iPlayerCapacity)
	{
		LLOG_DEBUG("The player num isn't enought");
		return;
	}

	for(int i = 0; i < m_iPlayerCapacity; i++)
	{
		if(m_user[i] == NULL || !m_readyState[i])
		{
			LLOG_DEBUG("The player hasn't  been ready");
			return;
		}
	}

	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_seat[i] == INVAILD_POS)
		{
			LLOG_DEBUG("The player hasn't  been select seat");
			return;
		}
	}



	m_selectSeat = false;

	for (int i = 0; i < DESK_USER_COUNT; i++)
	{
		m_user[i]  = m_seatUser[i];     //如果都准备好，把m_seatUser所有用户拷贝到m_user，这样m_user的索引号为座位号
	}

	GetVip()->ResetUser(m_user);

	bool bFind = false;

	for (int i = 0; i < DESK_USER_COUNT; i++)
	{
		if (m_user[i]) {
			if (m_creatUserId == m_user[i]->GetUserDataId()) {
				LLOG_DEBUG("m_zhuangPos = %d", i);
				m_zhuangPos = i;
				bFind = true;
				break;
			}
		}
	}


	if (!bFind && m_firestUser) {
		for (int i = 0; i < DESK_USER_COUNT; i++)
		{
			if (m_firestUser == m_user[i]) {
				LLOG_DEBUG("m_zhuangPos = %d", i);
				m_zhuangPos = i;
				bFind = true;
				break;
			}
		}
	}
	
	if(mGameHandler)
	{		 
		mGameHandler->SetDeskPlay();
	}
	else
	{
		LLOG_ERROR("Game handler han't beed created");
	}
	LLOG_ERROR("**************开始游戏时桌子ID: %d ", m_id);
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i]) {
			LLOG_ERROR("********** 用户ID %d IP = %s", m_user[i]->GetUserData().m_id, m_user[i]->m_userData.m_customString1.c_str());
		}
	}
}

void Desk::SetDeskFree()
{
	m_deskState = DESK_FREE;
	m_timeDeskCreate = 0;

	if(mGameHandler)
	{
		GameHandlerFactory::getInstance()->destroy((GameType)m_state,mGameHandler);
		LLOG_DEBUG("Desk::SetDeskFree    mHander");
		mGameHandler = NULL;
	}
	if( m_deskType == DeskType_Coins )
	{
		// 金币桌子由金币服务器负责回收自己回收
	}
	else
	{
		//gWork.RecycleDeskId(m_id);
		gWork.RecycleDeskId(m_id, m_clubInfo.m_clubId, m_clubInfo.m_playTypeId, m_clubInfo.m_clubDeskId);
		MHLOG_DESK("***Send to logic manager to recycle desk id: %d", m_id);
	}
}

void Desk::BoadCast(LMsg& msg)
{
	LLOG_DEBUG("BoadCast m_iPlayerCapacity = %d", m_iPlayerCapacity);
	for(Lint i = 0 ; i < m_iPlayerCapacity ;++i)
	{
		if(m_user[i])
			m_user[i]->Send(msg);
	}
}

void Desk::BoadCastWithOutUser(LMsg& msg, User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && m_user[i] != user)
			m_user[i]->Send(msg);
	}
}

std::vector<std::string>  Desk::getAllPlayerName()
{
	std::vector<std::string> tempNameList;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		//if (m_user[i]) {
		//	std::string name = m_user[i]->GetUserData().m_nike;
		if (m_seatUser[i]) {
			std::string name =m_seatUser[i]->GetUserData().m_nike;
			tempNameList.push_back(name);
		}

	}

	return tempNameList;
}

void Desk::ClearUser()
{
	if( m_deskType == DeskType_Common )
	{
		LLOG_DEBUG("m_iPlayerCapacity = %d", m_iPlayerCapacity);
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			//if (m_user[i])
			if (m_seatUser[i])
				OnUserOutRoom(m_seatUser[i]);
		}
	}
	else
	{
		for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i])
			{
				m_user[i]->setUserState(LGU_STATE_COIN);
				m_user[i]->ModifyUserState(true);   //修改user状态，通知金币服务器

				m_user[i]->SetDesk(NULL);

				//删除玩家 不删除机器人
				if (m_user[i]->getUserGateID() != 65535)		//不是机器人删除掉
				{
					UserManager::Instance().DelUser(m_user[i]);
					delete m_user[i];
					m_user[i] = NULL;
				}

				m_readyState[i] = 0;
			}
		}
	}
}

void Desk::SetAllReady()
{
	memset(m_readyState, 1, sizeof(m_readyState));
}

void Desk::ResetClear()
{
	m_resetTime = 0;
	m_resetUser.clear();
	memset(m_reset, 0, sizeof(m_reset));
}

void Desk::ResetEnd()
{
	LLOG_DEBUG("Logwyz ...............Desk::ResetEnd()");
	if(m_vip)
	{
		m_vip->m_reset = 1;
		if (m_vip->m_curCircle >= 1 && m_deskState == DESK_WAIT) // 局结束,下局未开始点
		{
			m_vip->InsertDeskWinnerInfo();
			m_vip->UpdateDeskTotalScoreToDB();
		}
	}
	
	if (m_deskState == DESK_PLAY)
	{
		LLOG_DEBUG("Logwyz ...............Desk::ResetEnd(),m_deskState == DESK_PLAY");
		m_bIsDissmissed = true;
		if (mGameHandler)
		{
			mGameHandler->OnGameOver(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
		}
		
	}
	else
	{

		if (m_deskCreatedType == 1) {
			LMsgL2LMGDeleteUserCreatedDesk deleteDeskInfo;
			deleteDeskInfo.m_deskId = m_id;
			deleteDeskInfo.m_userid = m_creatUserId;
			if (m_feeType == 0)
			{
				deleteDeskInfo.m_cost = m_cost;
			}

			if (m_couFei) {
				deleteDeskInfo.m_delType = 0;
			}else {
				deleteDeskInfo.m_delType = 2;
			}
			gWork.SendToLogicManager(deleteDeskInfo);
		}


		if (m_vip && m_vip->IsBegin())
		{
			LLOG_DEBUG("Logwyz ...............Desk::ResetEnd(),m_vip->SendEnd()");
			m_vip->SendEnd();
		}
		SetVip(NULL);
		ClearUser();
		//清空lookon用户
		MHClearLookonUser();
		ResetClear();
		SetDeskFree();
	}
}

void Desk::SpecialCardClear()
{
	memset(m_specialCard,0,sizeof(m_specialCard));
}


//////////////////////////////////////////////////////////////////////////

Lint Desk::getGameType() const
{
	return m_state;
}

const std::vector<Lint>& Desk::getPlayType() const
{
	return m_playtype;
}

Lint Desk::getDeskState() const
{
	return m_deskState;
}

void Desk::setDeskState(Lint nState)
{
	m_deskState = nState;	 
}

Lint Desk::getDeskPlayState()
{
	return m_deskPlayState;
}

void Desk::setDeskPlayState(Lint nDeskPlayState)
{
	m_deskPlayState = nDeskPlayState;
}

void Desk::HanderGameOver(Lint result)
{
	LLOG_DEBUG("Desk::HanderGameOver");
	if (m_vip == NULL)
	{
		return;
	}


	if( m_deskType == DeskType_Common )
	{
		LLOG_DEBUG("Desk::HanderGameOver   m_deskType == DeskType_Common");
		//检测扣房卡
		if (m_vip->m_curCircle == 1 && !m_vip->m_reset)
		{
			Lint cardType = CARD_TYPE_4;
			if (m_vip->m_maxCircle == 8)
				cardType = CARD_TYPE_8;
			else if (m_vip->m_maxCircle == 16)
				cardType = CARD_TYPE_16;

			LLOG_ERROR("*****每局结束桌子ID: %d ", m_id);
			for (int i = 0; i < m_iPlayerCapacity; i++)
			{
				if (m_user[i]) {
					LLOG_ERROR("**** 用户ID = %d IP = %s", m_user[i]->GetUserData().m_id, m_user[i]->m_userData.m_customString1.c_str());
				}
			}

			//发送删除信息,删除房主创建的对应的桌子号
			if (m_deskCreatedType == 1) {
				LMsgL2LMGDeleteUserCreatedDesk deleteDeskInfo;
				deleteDeskInfo.m_deskId = m_id;
				deleteDeskInfo.m_userid = m_creatUserId;
				if (m_feeType == 0)
				{
					deleteDeskInfo.m_cost = m_cost;
				}
				deleteDeskInfo.m_delType = 1;
				gWork.SendToLogicManager(deleteDeskInfo);
			}
			
			//扣费标志 重要
			m_couFei = true;

			//扣费信息
			Lint nFree = 0;
			Lint nTotalDeskFeeValue = 0;
			LTime cur;
			Lint curTime = cur.Secs();
			//这个是时间，也没有什么用
			if (curTime > m_startTime && curTime < m_endTime)
			{
				nFree = 1;
			}
			//这个是做活动的，没有什么用
			if (m_state != 100010 && m_state != 100008 && m_state != 100009) {
				nFree = 0;
			}
			if (nFree) {
				LLOG_ERROR("******DeskId:%d 房主扣费    在免费时间 不需要扣费", m_id);
			}

			//普通房间扣费
			if (m_clubInfo.m_clubId==0&&m_clubInfo.m_clubDeskId==0)  //普通桌子
			{
				if (m_feeType==0) {
					LLOG_ERROR("******DeskId:%d 房主扣费  creatUserId = %d  cost = %d", m_id, m_creatUserId, m_cost);
					if (nFree==0)
					{
						User* pCreatUser=gUserManager.GetUserbyDataId(m_creatUserId);
						if (pCreatUser) {
							pCreatUser->DelCardCount(cardType, m_cost, CARDS_OPER_TYPE_CREATE_ROOM, "system", m_feeType);  //房主扣费，房主在线

						}
						else {
							HanderDelCardCount(cardType, m_cost, CARDS_OPER_TYPE_CREATE_ROOM, "system", m_feeType, m_creatUserId, m_unioid); //房主不在线
						}
						m_vip->UpdateDeskTotalFeeValueToDB(m_cost);
					}

				}
				else {
					LLOG_ERROR("*****DeskId:%d 分摊房费  平均费用 = %d", m_id, m_even);
					if (nFree==0)
					{
						Lint nTotalDeskFeeValue=0;
						for (int i=0; i<m_iPlayerCapacity; i++)
						{

							if (m_user[i])
							{
								m_user[i]->DelCardCount(cardType, m_even, CARDS_OPER_TYPE_CREATE_ROOM, "system", m_feeType); //分摊房费
								m_vip->UpdateUserFeeValueToDB(m_user[i]->GetUserDataId(), m_even); // 修改数据库
								nTotalDeskFeeValue+=m_even;
							}
						}
						m_vip->UpdateDeskTotalFeeValueToDB(nTotalDeskFeeValue);
					}
				}
			}
			else   //俱乐部桌子扣费
			{
				if (m_feeType==0)         //会长扣费
				{      
					LLOG_ERROR("******clubShowDesk:%d  会长扣费  creatUserId = %d  cost = %d", m_clubInfo.m_showDeskId, m_creatUserId, m_cost);
					//m_vip->UpdateClubCreatorNumDiamondToDB(m_cost, m_creatUserId);
					HanderDelCardCount(cardType, m_cost, CARDS_OPER_TYPE_CLUB_DIAMOND, "system", m_feeType, m_creatUserId, m_unioid); //房主不在线
					m_vip->UpdateDeskTotalFeeValueToDB(m_cost);
					}
				else {
					LLOG_ERROR("*****DeskId:%d 分摊房费  平均费用 = %d", m_id, m_even);
					if (nFree==0)
					{
						Lint nTotalDeskFeeValue=0;
						for (int i=0; i<m_iPlayerCapacity; i++)
						{

							if (m_user[i])
							{
								m_user[i]->DelCardCount(cardType, m_even, CARDS_OPER_TYPE_CREATE_ROOM, "system", m_feeType); //分摊房费
								m_vip->UpdateUserFeeValueToDB(m_user[i]->GetUserDataId(), m_even); // 修改数据库
								nTotalDeskFeeValue+=m_even;
							}
						}
						m_vip->UpdateDeskTotalFeeValueToDB(nTotalDeskFeeValue);
					}
				}

			}
	
		}
	}

	if(m_vip->isNormalEnd())
	{
		LLOG_DEBUG("Desk::HanderGameOver   m_vip->isNormalEnd()");
		//清空lookon用户
		MHClearLookonUser();

		if(m_deskType == DeskType_Common)
		{
			LMsgL2LDBSaveCRELog log;
			if(m_user[0]!=NULL)
				log.m_strUUID = m_user[0]->m_userData.m_unioid;
			log.m_deskID = this->m_id;
			log.m_strLogId = m_vip->m_id;
			log.m_time = m_vip->m_time;
			for(Lint i = 0; i < m_iPlayerCapacity; ++i)
			{
				if(m_user[i]!=NULL)
					log.m_id[i] = m_user[i]->m_userData.m_id;
			}
			gWork.SendMsgToDb(log);
		}
	}

	if(m_vip->isEnd())
	{
		LLOG_DEBUG("Desk::HanderGameOver   m_vip->isEnd()");
		if (m_deskCreatedType == 1) {
			LMsgL2LMGDeleteUserCreatedDesk deleteDeskInfo;
			deleteDeskInfo.m_deskId = m_id;
			deleteDeskInfo.m_userid = m_creatUserId;
			if (m_feeType == 0)
			{
				deleteDeskInfo.m_cost = m_cost;
			}

			if (m_couFei) {
				deleteDeskInfo.m_delType = 0;
			}
			else {
				deleteDeskInfo.m_delType = 2;
			}
			gWork.SendToLogicManager(deleteDeskInfo);
		}

		//logic manager
		//发送manager
		if (m_clubInfo.m_clubId!=0)
		{
			LLOG_DEBUG("Desk::HanderGameOver LMsgL2LMGUserLeaveClubDesk");
			LMsgL2LMGUserLeaveClubDesk  send;
			send.m_clubId=m_clubInfo.m_clubId;
			send.m_playTypeId=m_clubInfo.m_playTypeId;
			send.m_clubDeskId=m_clubInfo.m_clubDeskId;
			for (int i=0; i<m_iPlayerCapacity; i++)
			{
				if (m_seatUser[i])
				{
					send.m_userId=m_seatUser[i]->GetUserDataId();
					send.m_strUUID=m_seatUser[i]->m_userData.m_unioid;
					gWork.SendToLogicManager(send);
				}
			}
		}


		LLOG_DEBUG("Desk::HanderGameOver  m_vip->SendEnd()");
		m_vip->SendEnd();
		m_vip->m_desk = NULL;
		ClearUser();
		SetDeskFree();
		SetVip(NULL);
		ResetClear();
	}
	SpecialCardClear();
}


void Desk::HanderAddCardCount(Lint pos, Lint CardNum, CARDS_OPER_TYPE AddType, Lstring admin)
{
	if(pos < 0 || pos >= INVAILD_POS)
	{
		LLOG_ERROR("Desk::HanderAddCardCount pos = %d error", pos);
		return;
	}

	if(!m_user[pos])
	{
		LLOG_ERROR("Desk::HanderAddCardCount user = %d is null", pos);
		return;
	}
	if(CardNum <= 0)		return;
	m_user[pos]->AddCardCount(CARD_TYPE_8, CardNum, AddType, admin);
}


void Desk::HanderDelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin, Lint feeType, Lint userId, Lstring unioid)
{
	LLOG_ERROR("*****DeskId:%d 房主建房 Desk::HanderDelCardCount type=%d,count=%d,operType=%d,FORCE_ROOM_CARD_FREE=%d", m_id, cardType, count, operType, FORCE_ROOM_CARD_FREE);
	if (FORCE_ROOM_CARD_FREE) {
		return;
	}

	//会长钻石券
	if (CARDS_OPER_TYPE_CLUB_DIAMOND==operType)
	{
		LMsgL2LMGModifyCard msg;
		msg.admin=admin;
		msg.cardType=cardType;
		msg.cardNum=count;
		msg.isAddCard=0;
		msg.operType=operType;
		msg.m_userid=userId;
		msg.m_strUUID=unioid;

		gWork.SendToLogicManager(msg);
		return;

	}

	/*
	int delCount = ::GetCardDelCount(cardType, count);

	if (!feeType) {
		delCount = delCount * 4;
	}*/

	User* user = gUserManager.GetUserbyDataId(userId);
	if (user) {
		if (user->m_userData.m_numOfCard2s - count >= 0)
			user->m_userData.m_numOfCard2s = user->m_userData.m_numOfCard2s - count;
		else
			user->m_userData.m_numOfCard2s = 0;
	}

	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 0;
	msg.operType = operType;
	msg.m_userid = userId;
	msg.m_strUUID = unioid;

	gWork.SendToLogicManager(msg);
}

bool Desk::DissmissAllplayer()
{
	if (getDeskState() == DESK_WAIT)
	{
		LLOG_DEBUG("Desk::DissmissAllplayer deskid=%d deskstate=%d", GetDeskId() , getDeskState());
		ResetEnd();
		return true;
	}
	return false;
}

Lint Desk::GetFirstZhuangPos()
{
	return m_zhuangPos;
}

void Desk::_clearData()
{
	m_id = 0;
	
	m_deskbeforeState = DESK_FREE;
	m_deskState = DESK_FREE;
	m_deskPlayState = -1;

	memset(m_user, 0, sizeof(m_user));
	m_vip = 0;
	for (int i = 0; i < DESK_USER_COUNT; i++) {
		m_seat[i] = INVAILD_POS;
	}
	//memset(m_seat, 0, sizeof(m_seat));
	memset(m_readyState, 0, sizeof(m_readyState));
	m_state = 0;
	m_playtype.clear();
	ResetClear();
	SpecialCardClear();
	m_resetUser = "";					//申请的玩家
	m_resetTime = 0;					//申请解算的时间
	mGameHandler = NULL;

	m_iPlayerCapacity = 0;

	m_timeDeskCreate = 0;

	m_cost = 0;
	m_even = 0;
	m_free = 0;
	m_startTime = 0;
	m_endTime = 0;
	m_gameStartTime = 0;

	memset( m_autoPlay, 0, sizeof(m_autoPlay) );
	memset( m_autoPlayTime, 0, sizeof(m_autoPlayTime) );
	memset( m_autoOutTime, 0, sizeof(m_autoOutTime) );
	m_autoChangeOutTime = 0;
	m_autoPlayOutTime = 0;
	m_baseScore = 1;
	memset( m_coinsResult, 0, sizeof(m_coinsResult) );
	m_creatUser = NULL;
	m_deskType = DeskType_Common;
	m_selectSeat = false;                   //选座状态  创建房间时为true 选择完座位时为false
	m_firestUser = NULL;
	for (int i = 0; i < 4; i++)
	{
		m_seatUser[i] = NULL;
		m_videoPermit[i] = 1;
	}
	m_zhuangPos = 0;
	m_dismissUser = false;
	m_couFei = false;
	//清空
	memset(m_switchPos, 0x00, sizeof(m_switchPos));
	//m_desk_all_user.clear();
	m_desk_Lookon_user.clear();
	m_user_select_seat_turn.clear();
	m_Greater2CanStart=0;
	m_StartGameButtonPos=INVAILD_POS;
	m_startButtonAppear=0;
	m_clubInfo.reset();
	m_bIsDissmissed = false;
	m_Gps_Limit = 0;
}

bool Desk::_createRegister(GameType gameType)
{
	mGameHandler = GameHandlerFactory::getInstance()->create(gameType);
	if (!mGameHandler)
	{
		LLOG_ERROR("No game handler for game type %d found.", gameType);
		return false;
	}

	mGameHandler->startup(this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//by wyz  20171103 改好，机器人选择不能用，这个函数没有改
void Desk::MHProcessRobotSelectSeat(User * pUser)
{

	LLOG_DEBUG("******Desk::MHProcessRebootSelectSea处理机器人位置选择*********");

	if (pUser->m_desk != this)
	{
		LLOG_ERROR("Error, user did not add to desk userid=%d deskid=%d", pUser->GetUserDataId(), this->GetDeskId());
		return;
	}

	if (m_deskState != DESK_WAIT/*&&m_deskState !=DESK_COUNT_RESULT*/)
	{
		LLOG_ERROR("desk state error, userid=%d deskstate=%d", pUser->GetUserDataId(), m_deskState);
		return;
	}

	if (!m_selectSeat)
	{
		LLOG_ERROR("desk state error, desk is not in select seat. deskid = %d, selectseat=%d", GetDeskId(), m_selectSeat);
		return;
	}
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		LLOG_ERROR("error user pos, userid = %d, pos = %d", pUser->GetUserDataId(), pos);
		return;
	}

	Lint pos_user_selected = INVAILD_POS;
	User * pActiveUser = 0;
	for (int i = 0; i < DESK_USER_COUNT; i++)
	{
		if (m_user[i] != NULL && m_user[i]->GetUserDataId() < 10000000)
		{
			pActiveUser = m_user[i];
			LLOG_DEBUG("Desk::MHProcessRebootSelectSeat found user, userid = %d", pActiveUser->GetUserDataId());
			break;
		}
	}

	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		Lint pos_will_selected = INVAILD_POS;
		for (int j = 0; j < DESK_USER_COUNT; j++)
		{
			bool bSelected = false;
			for (int i = 0; i < DESK_USER_COUNT; i++)
			{
				if (j == m_seat[i])
				{
					bSelected = true;
					break;
				}
			}
			if (!bSelected)
			{
				pos_will_selected = j;
				LLOG_DEBUG("Desk::MHProcessRebootSelectSeat 找到未使用的座位号 %d", pos_will_selected);
				break;
			}
		}
		if (pos_will_selected != INVAILD_POS)
		{
			LMsgS2CUserSelectSeat selectSeat;
			m_seat[pos] = pos_will_selected;              //用户选择的位置
			selectSeat.m_id = pUser->GetUserDataId();
			selectSeat.m_pos = pos_will_selected;
			BoadCast(selectSeat);                  //广播用户选择位置
			m_readyState[pos] = 1;
			m_seatUser[pos_will_selected] = m_user[pos];  //m_seatUser的Index就是座位号
		}
		this->CheckGameStart();
	}
}

void Desk::MHPrintDeskStatus()
{
	char buf[64];
	int user_count = 0;
#ifdef WIN32
	LTime time;
	time.SetSecs(m_timeDeskCreate);
	_snprintf_s(buf, 512, "%04d%02d%02d-%02d:%02d:%02d", time.GetYear(), time.GetMonth(), time.GetMday(), time.GetHour(), time.GetMin(), time.GetSec());
	buf[63] = 0;
	for (int i = 0; i < DESK_USER_COUNT; i++)
	{
		if (m_user[i])
		{
			user_count++;
		}
	}
	MHLOG_DESK("Desk id= %d state= %d usercount=%d cratetime= %s", m_id, m_deskState, user_count, buf);
#endif //  WIN32

}

//////////////////////////////
//判断是否Lookon用户  
bool Desk::MHIsLookonUser(User* pUser)
{
	if (pUser==NULL)return false;
	std::list<User*>::iterator finduser=find(m_desk_Lookon_user.begin(), m_desk_Lookon_user.end(), pUser);
	if (finduser==m_desk_Lookon_user.end())
		return false;
	return true;
}

//判断选座用户  
bool Desk::MHIsInSeatTurn(User* pUser)
{
	if (pUser==NULL)return false;
	if (GetUserSeatPos(pUser)==INVAILD_POS)
	{
		m_user_select_seat_turn.remove(pUser);
		return false;
	}
	std::list<User*>::iterator finduser=find(m_user_select_seat_turn.begin(), m_user_select_seat_turn.end(), pUser);
	if (finduser==m_user_select_seat_turn.end())
		return false;
	return true;
}


bool Desk::MHIsRoomUser(User* pUser)
{
	//if (pUser==NULL)return false;
	//std::list<User*>::iterator finduser=find(m_desk_all_user.begin(), m_desk_all_user.end(), pUser);
	//if (finduser==m_desk_all_user.end())
	//	return false;
	//return true;
	return (MHIsLookonUser(pUser)||MHIsSeatUser(pUser));

}

bool Desk::MHIsSeatUser(User* pUser)
{
	if (pUser==NULL)return false;
	for (Lint i=0; i<m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i]==pUser)
			return true;
	}
	return false;
}

//广播桌子所有用户
void Desk::MHBoadCastAllDeskUser(LMsg& msg)
{
	LLOG_DEBUG("Logwyz-------------Desk::MHBoadCastAllDeskUser ");
	//LLOG_DEBUG("Logwyz-------------Desk::MHBoadCastAllDeskUser   size=[%d], msgid=[%d] ",m_desk_all_user.size(),msg.m_msgId);
	//std::list<User*>::iterator userIt;
	//for (userIt=m_desk_all_user.begin(); userIt!=m_desk_all_user.end(); ++userIt)
	//{
	//	User* temp=*userIt;
	//	if (temp==NULL)continue;
	//	temp->Send(msg);
	//}
	//广播座位用户
	MHBoadCastDeskSeatUser(msg);
	//广播lookon用户
	MHBoadCastDeskLookonUser(msg);

}

//广播桌子所有用户without usr
void Desk::MHBoadCastAllDeskUserWithOutUser(LMsg& msg, User* user)
{
	LLOG_DEBUG("Logwyz-------------Desk::MHBoadCastAllDeskUser(LMsg& msg) ");
	//std::list<User*>::iterator userIt;
	//for (userIt=m_desk_all_user.begin(); userIt!=m_desk_all_user.end(); ++userIt)
	//{
	//	User* temp=*userIt;
	//	if (temp==NULL||temp!=user)continue;
	//	temp->Send(msg);
	//}
	MHBoadCastDeskSeatUserWithOutUser(msg, user);
	MHBoadCastDeskLookonUserWithOutUser(msg, user);
}

//广播桌子Lookon用户
void Desk::MHBoadCastDeskLookonUser(LMsg& msg)
{
	LLOG_DEBUG("Logwyz-------------Desk::MHBoadCastDeskLookonUser(LMsg& msg) ,size=[%d]", m_desk_Lookon_user.size());
	std::list<User*>::iterator userIt;
	for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); ++userIt)
	{
		User* temp=*userIt;
		if (temp==NULL)continue;
		temp->Send(msg);
	}
}

//广播桌子Lookon用户without user
void Desk::MHBoadCastDeskLookonUserWithOutUser(LMsg& msg, User* user)
{
	LLOG_DEBUG("Logwyz-------------Desk::MHBoadCastDeskLookonUserWithOutUser");
	std::list<User*>::iterator userIt;
	for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); ++userIt)
	{
		User* temp=*userIt;
		if (temp==NULL||temp!=user)continue;
		temp->Send(msg);
	}
}

//广播桌上座位用户
void Desk::MHBoadCastDeskSeatUser(LMsg& msg)
{
	LLOG_DEBUG("MHBoadCastDeskSeatUser m_iPlayerCapacity = %d", m_iPlayerCapacity);
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i])
			m_seatUser[i]->Send(msg);
	}
}

//广播桌上座位用户without user
void Desk::MHBoadCastDeskSeatUserWithOutUser(LMsg& msg , User* user)
{
	LLOG_DEBUG("MHBoadCastDeskSeatUserWithOutUser m_iPlayerCapacity = %d", m_iPlayerCapacity);
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i]&&m_seatUser[i]!=user)
			m_seatUser[i]->Send(msg);
	}
}

//入座人数  ;GetUserCount() 是游戏人数
Lint Desk::MHGetSeatUserCount()
{
	Lint cnt=0;
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i])
		{
			cnt+=1;
		}
	}

	return cnt;
}

Lint Desk::MHGetDeskUserCount()
{
	return MHGetSeatUserCount()+m_desk_Lookon_user.size();
}



//判断是否开始游戏
Lint Desk::MHCheckGameStart()
{
	LLOG_DEBUG("*********************************************Desk::MHCheckGameStart()");
	if (m_deskState!=DESK_WAIT)	//只有桌子为等待状态才继续
	{
		LLOG_DEBUG("Desk's status is wrong. Desk state = %d.", m_deskState);
		return 0;
	}

	if (m_resetTime!=0)
	{
		LLOG_DEBUG("Desk ising reseted");
		return 0;
	}

	//判断所有的人是否都已经准备
	LLOG_DEBUG("Logwyz  ..............twoCanStartGame=[%d]", m_Greater2CanStart);
	if (m_Greater2CanStart)   //>2可以开始
	{
		if (MHGetSeatUserCount()<2)
		{
			LLOG_DEBUG("The player num isn't enought");
			return 0;
		}
			
	}
	else
	{
		if (MHGetSeatUserCount()!=m_iPlayerCapacity)
		{
			LLOG_DEBUG("The player num isn't enought");
			return  0;
		}
	}

	//座位上人数准备状况
	for (int i=0; i < m_iPlayerCapacity; i++)
	{
		if (m_seatUser[i]!=NULL &&m_readyState[i]==0)
		{
			LLOG_DEBUG("The player[%d] hasn't  been ready", m_seatUser[i]->GetUserDataId());
			return  0;
		}
	}

	//第一局
	if (m_vip->m_curCircle==0&&m_Greater2CanStart==1)
	{
		LLOG_DEBUG("first dw begin...");
		return 1;
	}
	else
	{
		LLOG_DEBUG("not  first dw begin   game...");
		MHHanderStartGame();
		return 2;
	}


	//for (int i=0; i < m_iPlayerCapacity; i++)
	//{
	//	if (m_user[i]==NULL||!m_readyState[i])
	//	{
	//		LLOG_DEBUG("The player hasn't  been ready");
	//		return;
	//	}
	//}
	//
	//for (int i=0; i < m_iPlayerCapacity; i++)
	//{
	//	if (m_seat[i]==INVAILD_POS)
	//	{
	//		LLOG_DEBUG("The player hasn't  been select seat");
	//		return;
	//	}
	//}
	//
	//
	//
	//m_selectSeat=false;
	//
	//for (int i=0; i < DESK_USER_COUNT; i++)
	//{
	//	m_user[i]=m_seatUser[i];     //如果都准备好，把m_seatUser所有用户拷贝到m_user，这样m_user的索引号为座位号
	//}
	//
	//GetVip()->ResetUser(m_user);
	//
	//bool bFind=false;
	//if (m_creatUser) {
	//	for (int i=0; i < DESK_USER_COUNT; i++)
	//	{
	//		if (m_creatUser==m_user[i]) {
	//			LLOG_DEBUG("m_zhuangPos = %d", i);
	//			m_zhuangPos=i;
	//			bFind=true;
	//			break;
	//		}
	//	}
	//}
	//
	//
	//if (!bFind && m_firestUser) {
	//	for (int i=0; i < DESK_USER_COUNT; i++)
	//	{
	//		if (m_firestUser==m_user[i]) {
	//			LLOG_DEBUG("m_zhuangPos = %d", i);
	//			m_zhuangPos=i;
	//			bFind=true;
	//			break;
	//		}
	//	}
	//}
	//
	//if (mGameHandler)
	//{
	//	mGameHandler->SetDeskPlay();
	//}
	//else
	//{
	//	LLOG_ERROR("Game handler han't beed created");
	//}
	//LLOG_ERROR("**************桌子ID: %d ", m_id);
	//for (int i=0; i < m_iPlayerCapacity; i++)
	//{
	//	if (m_user[i]) {
	//		LLOG_ERROR("********** %s IP = %s", m_user[i]->GetUserData().m_nike.c_str(), m_user[i]->m_userData.m_customString1.c_str());
	//	}
	//}
}

//开始游戏
void Desk::MHHanderStartGame()
{
	LLOG_DEBUG("Logwyz....................MHHanderStartGame()");

	//换一个转换方法
	int j=0;
	memset(m_switchPos, 0x00, sizeof(m_switchPos));
	for (int i=0; i<DESK_USER_COUNT; i++)
	{
		if (m_seatUser[i])
		{
			m_user[j]=m_seatUser[i];
			m_switchPos[j++]=i;
		}
	}
	m_desk_user_count=j;

	//位置转换
	memcpy(m_seatUser, m_user, sizeof(m_seatUser));
	for (int i=0; i<DESK_USER_COUNT; i++)
	{
		m_switchPos[i]=i;
	}


	GetVip()->ResetUser(m_user);
	
	////这里首局选庄
	//bool bFind=false;
	//if (m_creatUser) {
	//	for (int i=0; i < DESK_USER_COUNT; i++)
	//	{
	//		if (m_creatUser==m_user[i]) {
	//			LLOG_DEBUG("m_zhuangPos = %d", i);
	//			m_zhuangPos=i;
	//			bFind=true;
	//			break;
	//		}
	//	}
	//}
	//
	//if (!bFind && m_firestUser) {
	//	for (int i=0; i < DESK_USER_COUNT; i++)
	//	{
	//		if (m_firestUser==m_user[i]) {
	//			LLOG_DEBUG("m_zhuangPos = %d", i);
	//			m_zhuangPos=i;
	//			bFind=true;
	//			break;
	//		}
	//	}
	//}
	if (m_vip && !m_vip->IsBegin())
	{
		m_gameStartTime = time(NULL);
	}

	m_zhuangPos=MHSpecPersonPos();
	LLOG_DEBUG("Logwyz  MHHanderStartGame m_zhuangPos = %d", m_zhuangPos);
	
	if (mGameHandler)
	{
		mGameHandler->MHSetDeskPlay(m_desk_user_count);
	}
	else
	{
		LLOG_ERROR("Game handler han't beed created");
	}
	//LLOG_ERROR("**************桌子ID: %d ", m_id);
	//for (int i=0; i < m_iPlayerCapacity; i++)
	//{
	//	if (m_user[i]) {
	//		LLOG_ERROR("********** %s IP = %s", m_user[i]->GetUserData().m_nike.c_str(), m_user[i]->m_userData.m_customString1.c_str());
	//	}
	//}

}


//解散时，清空lookon
void Desk::MHClearLookonUser()
{
	LLOG_DEBUG("Logwyz-------------Desk::MHClearLookonUser()");

	if (m_deskType==DeskType_Common)
	{
		LLOG_DEBUG("Desk::MHClearLookonUser(): Lookon User  size = %d", m_desk_Lookon_user.size());
		//std::list<User*>::iterator userIt;
		//for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); ++userIt)
		//{
		//	User* temp=*userIt;
		//	if (temp==NULL)continue;
		//	OnUserOutRoom(temp);
		//
		//}
		Lint  lookonUserCount=m_desk_Lookon_user.size();
		for (int i=0; i<lookonUserCount; ++i)
		{
			LLOG_DEBUG("Logwyz-------------Desk::MHClearLookonUser()  in for begin size=[%d]", m_desk_Lookon_user.size());
			if (m_desk_Lookon_user.size()==0)break;
			User* temp=m_desk_Lookon_user.back();
			if (temp==NULL)continue;
			OnUserOutRoom(temp);
			LLOG_DEBUG("Logwyz-------------Desk::MHClearLookonUser()  in for end size=[%d]", m_desk_Lookon_user.size());

		}
		LLOG_INFO("Logwyz-------------m_desk_Lookon_user.clear()");
		//m_desk_all_user.clear();
		m_desk_Lookon_user.clear();
		m_user_select_seat_turn.clear();
		//for (Lint i=0; i < m_iPlayerCapacity; ++i)
		//{
		//	if (m_user[i])
		//		OnUserOutRoom(m_user[i]);
		//}
	}
	else
	{
		// ?????  金币场 是啥，什么时候用
		LLOG_DEBUG("else ::: Lookon User  size = %d", m_desk_Lookon_user.size());
		//for (Lint i=0; i < m_iPlayerCapacity; ++i)
		//{
		//	if (m_user[i])
		//	{
		//		m_user[i]->setUserState(LGU_STATE_COIN);
		//		m_user[i]->ModifyUserState(true);   //修改user状态，通知金币服务器
		//
		//		m_user[i]->SetDesk(NULL);
		//
		//		//删除玩家 不删除机器人
		//		if (m_user[i]->getUserGateID()!=65535)		//不是机器人删除掉
		//		{
		//			UserManager::Instance().DelUser(m_user[i]);
		//			delete m_user[i];
		//			m_user[i]=NULL;
		//		}
		//
		//		m_readyState[i]=0;
		//	}
		//}
	}
}

//客户端发送开始游戏指令
void Desk::MHHanderStartGameButton(User* pUser, LMsgC2SStartGame *msg)
{
	LLOG_ERROR("Desk::MHHanderStartGameButton");
	if (pUser==NULL||msg==NULL)return;
	LMsgC2SStartGame startButton;
	Lint seatID=GetUserSeatPos(pUser);

	if (m_deskState!=DESK_WAIT/*&&m_deskState !=DESK_COUNT_RESULT*/)
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton state error, userid=%d deskstate=%d", pUser->GetUserDataId(), m_deskState);
		return;
	}

	if (seatID==INVAILD_POS)
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton pos error, userid=%d   seatID=%d", pUser->GetUserDataId(), seatID);
		return;
	}
	if (seatID!=m_StartGameButtonPos)
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton pos error, userid=%d   seatID=%d,m_StartGameButtonPos=[%d]", pUser->GetUserDataId(), seatID, m_StartGameButtonPos);
		return;
	}
	if(MHGetSeatUserCount()<2)
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton player not enough, userid=%d   seatID=%d,m_StartGameButtonPos=[%d],MHGetSeatUserCount()=[%d]", pUser->GetUserDataId(), seatID, m_StartGameButtonPos, MHGetSeatUserCount());
		//这种情况，说明有人退出了，不能开始游戏，把开始按钮收回
		m_StartGameButtonPos=INVAILD_POS;
		return;
	}
	
	//把没有选座的玩家提出房间
	LLOG_DEBUG("DeskType_Common::: Lookon User  size = %d", m_desk_Lookon_user.size());
	std::list<User*>::iterator userIt;
	//for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); ++userIt)
	//{
	//	User* temp=*userIt;
	//	if (temp==NULL)continue;
	//	OnUserOutRoom(temp);
	//
	//}
	Lint lookonUserCount=m_desk_Lookon_user.size();
	for (int i=0; i<lookonUserCount; ++i)
	{
		LLOG_DEBUG("  in  for begin  m_desk_Lookon_user.size()=[%d]", m_desk_Lookon_user.size());
		if (m_desk_Lookon_user.size()==0)break;
		User* temp=m_desk_Lookon_user.back();
		if (temp==NULL)continue;
		OnUserOutRoom(temp);
		LLOG_DEBUG("  in  for  end  m_desk_Lookon_user.size()=[%d]", m_desk_Lookon_user.size());

	}
	m_desk_Lookon_user.clear();


	LLOG_DEBUG("Logwyz ....  MHCheckGameStart()    2553     ");
	//开始按钮位置为无效
	m_StartGameButtonPos=INVAILD_POS;
	m_startButtonAppear=1;
	MHHanderStartGame();
	
}

//选择特殊用户的位置位置（规则：如果创建房间的人入座了，返回其位置；如果没有,选择第一个入座的人的位置;如果没有,东南西北依次)
Lint	 Desk::MHSpecPersonPos()
{
	//if (m_creatUser)
	//{
	//	if (GetUserSeatPos(m_creatUser)!=INVAILD_POS)
	//		return GetUserSeatPos(m_creatUser);
	//}
	if (MHGetCreatorSeatPos()!=INVAILD_POS)
		return MHGetCreatorSeatPos();
	//选座时间
	std::list<User*>::iterator userIt;
	for (userIt=m_user_select_seat_turn.begin(); userIt!=m_user_select_seat_turn.end(); ++userIt)
	{
		User* temp=*userIt;
		if (temp==NULL)continue;
		if (GetUserSeatPos(temp)!=INVAILD_POS)
			return GetUserSeatPos(temp);
	}
	//预防措施m_user_select_seat_turn没有人，按东南西北计算
	for (Lint i=0; i<m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i])return i;
	}
	return INVAILD_POS;
}

//返回房主在m_seatUser 中的位置,不存在返回INVAILD_POS
Lint  Desk::MHGetCreatorSeatPos()
{
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_seatUser[i])
		{
			if (m_seatUser[i]->GetUserDataId()==m_creatUserId)
				return i;
		}
	}

	return INVAILD_POS;
}

//返回房主在m_user 中的位置,不存在返回INVAILD_POS
Lint	  Desk::MHGetCreatorPos()
{
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i])
		{
			if (m_user[i]->GetUserDataId()==m_creatUserId)
				return i;
		}
	}
	return INVAILD_POS;
}

Lint Desk::MHGetDeskUser(std::vector<Lstring> &seatPlayerName, std::vector<Lstring> &noSeatPlayerName)
{
	//入座
	for (int i=0; i<DESK_USER_COUNT; i++)
	{
		if (m_seatUser[i])
			seatPlayerName.push_back(m_seatUser[i]->m_userData.m_nike);
	}
	//为入座
	for (auto ItLookonUser=m_desk_Lookon_user.begin(); ItLookonUser!=m_desk_Lookon_user.end(); ItLookonUser++)
	{
		noSeatPlayerName.push_back((*ItLookonUser)->m_userData.m_nike);
	}

	return 0;
}

void Desk::MHNotifyManagerDeskInfo(Lint roomFull, Lint currCircle, Lint totalCircle)
{
	LLOG_DEBUG("Desk::MHNotifyManagerDeskInfo  roomFull=[%d],currCircle=[%d], totalCircle=[%d]", roomFull, currCircle, totalCircle);
	LMsgL2LMGFreshDeskInfo send;
	send.m_clubId=m_clubInfo.m_clubId;
	send.m_playTypeId=m_clubInfo.m_playTypeId;
	send.m_clubDeskId=m_clubInfo.m_clubDeskId;
	send.m_showDeskId=m_clubInfo.m_showDeskId;
	send.m_roomFull=roomFull;
	send.m_currCircle=currCircle;
	send.m_totalCircle=totalCircle;
	gWork.SendToLogicManager(send);

}

bool Desk::MHCheckUserGPSLimit(User *pUser, Lstring & desk_gps_list)
{
	bool bRet = true;	
	if (NULL == pUser)
	{
		LLOG_ERROR("Desk::MHCheckUserGPSLimit, param error, empty user, desk:%d, GPSLimit=%d", m_id, m_Gps_Limit);
		return false;
	}
	LLOG_ERROR("Desk::MHCheckUserGPSLimit, desk:%d, user:%d", m_id, pUser->GetUserDataId());
	if (m_Gps_Limit == 0) 
		return bRet;
	std::ostringstream ss;
	Lstring user_name = string_replace(pUser->m_userData.m_nike, Lstring(","), Lstring(""));
	user_name = string_replace(user_name, Lstring("|"), Lstring(""));
	ss << user_name << ",";
	ss << pUser->m_userData.m_customString2;
	{
		//进入用户和在座用户
		boost::mutex::scoped_lock l(m_mutexDeskLookonUser);		 
		for(auto it = m_desk_Lookon_user.begin(); it != m_desk_Lookon_user.end();it++)
		{
			if ((*it)->IsInLimitGPSPosition(*pUser))
				bRet = false;
			if (pUser->GetUserDataId() != (*it)->GetUserDataId())
			{
				Lstring user_name = string_replace((*it)->m_userData.m_nike, Lstring(","), Lstring(""));
				user_name = string_replace(user_name, Lstring("|"), Lstring(""));
				ss << "|" << user_name << "," << (*it)->m_userData.m_customString2;
			}	
		}
	}
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_seatUser[i])
		{
			if (m_seatUser[i]->IsInLimitGPSPosition(*pUser))
				bRet = false;
			if (pUser->GetUserDataId() != m_seatUser[i]->GetUserDataId())
			{
				Lstring user_name = string_replace(m_seatUser[i]->m_userData.m_nike, Lstring(","), Lstring(""));
				user_name = string_replace(user_name, Lstring("|"), Lstring(""));
				ss << "|" << user_name << "," << m_seatUser[i]->m_userData.m_customString2;
			}
		}
	}
	desk_gps_list = ss.str();
	return bRet;
}


bool  Desk::MHDismissDeskOnPlay()
{
	ResetEnd();
	return true;
}




