#include "Desk.h"
#include "Work.h"
#include "LLog.h"
#include "Utils.h"
#include "Config.h"
#include "RoomVip.h"
#include "UserManager.h"
#include "LogicToggles.h"
#include "..\mhmessage\mhmsghead.h"

Desk::Desk()
{
	_clearData();
	LLOG_ERROR("************Test INVLID_POS %d", INVAILD_POS_QIPAI);
}

Desk::~Desk()
{

}

bool Desk::initDesk(int iDeskId, QiPaiGameType gameType)
{
	_clearData();

	switch (gameType) {

#define XX(k, v, player_count) case v: m_iPlayerCapacity = player_count; break;
		POKER_GAME_TYPE_MAP(XX)
#undef XX

	default:
		LLOG_ERROR("Desk::initDesk -- Game type is wrong. Type = %d", gameType);
		return false;
	}

	//if (gameType < QiPaiGameType::MH_GAME_TYPE_MIN || gameType > MH_GAME_TYPE_MAX)
	//{
	//
	//	LLOG_ERROR("Desk::initDesk -- Game type is wrong. Type = %d", gameType);
	//	return false;
	//}
	//m_iPlayerCapacity = GetMaxPlayerCountByType(gameType);
	m_desk_user_count = m_iPlayerCapacity;
	m_draw_user_count=0;

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

/* 获取桌子ID */
Lint Desk::GetDeskId()
{
	return m_id;
}

/* 获取玩法限定的最大玩家数量 */
Lint Desk::GetPlayerCapacity()
{
	return m_iPlayerCapacity;
}

/* 定时器 */
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
	}
	if (m_deskState == DESK_WAIT)
	{
		check_user_ready(curr);
	}
	if (m_deskState == DESK_PLAY &&  mGameHandler)
	{
		mGameHandler->Tick(curr);
	}
}

/* 设置自动出牌字段 */
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

/* 检查是否该自动出牌 */
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

/* 设置桌子类型：普通场、金币场 */
void Desk::SetDeskType( DeskType type )
{
	m_deskType = type;
}

/* 获取桌子类型：0:普通场  1:金币场 */
DeskType Desk::GetDeskType()
{
	return m_deskType;
}

void  Desk::SetSelectSeat(bool state)
{
	m_selectSeat = state;
}

/* 获取桌子创建时间 */
Lint  Desk::GetCreatDeskTime()
{
	return m_timeDeskCreate;
}

/* 设置创建房间类型：0:自己建房  1:房主建房 */
void  Desk::SetCreatType(Lint type)
{
	m_deskCreatedType = type;
}

/* 获取创建房间类型：0:自己建房  1:房主建房 */
Lint  Desk::GetCreatType()
{
	return m_deskCreatedType;
}

//通知客户端观战玩家数量变化
void Desk::UpdateLookOnUserCount()
{
	Lint t_lookOnCount = 0;
	if (!m_desk_Lookon_user.empty())
	{
		t_lookOnCount = m_desk_Lookon_user.size();
	}
	if (m_clubOwerLookOnUser)
	{
		++t_lookOnCount;
	}

	LMsgS2CShowLookOnCount showCount;
	showCount.m_lookOnCount = t_lookOnCount;

	MHBoadCastAllDeskUser(showCount);
	MHBoadCastDeskLookonUser(showCount);
	MHBoadCastClubOwerLookOnUser(showCount);
}

/* 处理玩家申请解算房间 */
void Desk::HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg)
{
	//玩家空指针判断
	if (!pUser && !msg)
	{
		LLOG_ERROR("Desk::HanderResutDesk() Error!!! user or msg is NULL... deskId=[%d]",
			this->GetDeskId());
		return;
	}

	if (!m_vip || m_deskState == DESK_FREE)
	{
		LLOG_ERROR("Desk::HanderResutDesk() Error!!! vip is NULL...");
		return;
	}

	//重复申请
	if (m_resetTime != 0)
	{
		LLOG_ERROR("Desk::HanderResutDesk() Error!!! this desk is resulting, can not resut desk again... deskId=[%d], userId=[%d]",
			this->GetDeskId(), pUser->GetUserDataId());

		return;
	}

	// 观战未入坐的俱乐部管理员
	bool b_can_dissmiss = (m_vip && m_vip->m_curCircle == 0 
		&& m_deskState == DESK_WAIT && MHIsLookonUser(pUser) 
		&& (IsClubAdminUser(pUser) || IsClubOwnerUser(pUser)));
	if (b_can_dissmiss)
	{
		LLOG_DEBUG("Desk::HanderResutDesk()  allow lookon adimin dismiss desk!!! deskid:%d  userId=[%d]  clubid[%d]", m_id, pUser->GetUserDataId(), m_clubInfo.m_clubId);
	}

	//申请玩家不在该房间 && 不是房主
	if (!MHIsRoomUser(pUser) && pUser != m_creatUser  && (!b_can_dissmiss))
	{
		LLOG_ERROR("Desk::HanderResutDesk() Error!!! pUser not in room and not m_creatUser... deskId=[%d], userid=[%d]",
			this->GetDeskId(), pUser->GetUserDataId());

		return;
	}

	//游戏未开局 && 不是房主
	if (m_vip && m_vip->m_curCircle == 0 && m_deskState == DESK_WAIT && (!b_can_dissmiss))
	{
		//观战的俱乐部管理员
		if (MHIsLookonUser(pUser) && (IsClubAdminUser(pUser) || IsClubOwnerUser(pUser)) )
		{
			LLOG_DEBUG("Desk::HanderResutDesk()  allow lookon adimin dismiss desk!!! deskid:%d  userId=[%d]  clubid[%d]", m_id, pUser->GetUserDataId(), m_clubInfo.m_clubId);
		}
		else if (m_creatUserId != pUser->GetUserDataId())
		{
			LLOG_ERROR("Desk::HanderResutDesk() Error!!! game not begin!!! userId=[%d] not creator[%d]",
				pUser->GetUserDataId(), m_creatUserId);
			return;
		}
	}

	// 游戏已开局  && 房主 && 非入坐玩家
	if (MHIsDeskMatchBegin() && m_creatUserId == pUser->GetUserDataId() && !MHIsRoomUser(pUser)  && !b_can_dissmiss)
	{
		LLOG_ERROR("代码到了这里Desk::HanderResutDesk() Error!!! game start, desk ower look on can not!!! userId=[%d] not creator[%d]",
			pUser->GetUserDataId(), m_creatUserId);

		return;
	}

	//动态加入玩家不能提解散，房主除外
	/*if (GetUserPos(pUser) != INVAILD_POS_QIPAI && m_playStatus[GetUserPos(pUser)] != 1 && pUser != m_creatUser)
	{
	LLOG_ERROR("Desk::HanderSelectResutDesk() Run Error, this player status is 2... deskId=[%d]", this->GetDeskId());
	return;
	}*/

	LLOG_ERROR("Desk::HanderResutDesk() Run... deskId=[%d], userId=[%d], pos=[%d]", 
		this->GetDeskId(), pUser->GetUserDataId(), GetUserPos(pUser));

	if (m_deskState == DESK_PLAY)
	{		
		Lint pos = GetUserPos(pUser);
		if (pos == INVAILD_POS_QIPAI)
		{
			LLOG_ERROR("Desk::HanderResutDesk() Error!!! m_deskState == DESK_PLAY, userId=[%d], pos=[%d]", 
				pUser->GetUserDataId(), pos);

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

		//给观战玩家弹窗
		if (!m_desk_Lookon_user.empty())
		{
			LMsgS2CResetDesk sendLookon;
			sendLookon.m_errorCode = 0;
			sendLookon.m_applay = m_resetUser;
			sendLookon.m_userId = pUser->m_userData.m_id;
			sendLookon.m_time = m_resetTime - gWork.GetCurTime().Secs();
			sendLookon.m_flag = 1;
			sendLookon.m_isLookOn = 1;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if (m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					sendLookon.m_agree.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					sendLookon.m_wait.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					sendLookon.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			MHBoadCastDeskLookonUser(sendLookon);
		}
	}
	else if (m_deskState == DESK_WAIT)
	{
		m_dismissUser = true;
		
		//游戏没开始直接解散
		if (!m_vip->IsBegin())
		{
			LLOG_DEBUG("Desk::HanderResutDest() Run... Desk Wait and not start game...");

			LMsgS2CResetDesk send;
			send.m_errorCode = 1;
			send.m_applay = pUser->m_userData.m_nike;
			send.m_userId = pUser->m_userData.m_id;
			send.m_flag = 1;
			send.m_isShowFee = (m_vip && m_vip->m_curCircle == 0) ? 1 : 0;
			MHBoadCastAllDeskUser(send);
			MHBoadCastDeskLookonUser(send);

			ResetEnd();
		}
		//游戏开始，结算阶段
		else
		{
			LLOG_DEBUG("Desk::HanderResutDest() Run... Desk Wait but start game...");

			Lint pos1 = GetUserPos(pUser);
			if (pos1 == INVAILD_POS_QIPAI)
			{
				LLOG_ERROR("Desk::HanderResutDesk error m_deskState == DESK_PLAY, userid[%d] pos[%d]", pUser->GetUserDataId(), pos1);
				return;
			}
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

			//给观战玩家弹窗
			if (!m_desk_Lookon_user.empty())
			{
				LMsgS2CResetDesk sendLookon;
				sendLookon.m_errorCode = 0;
				sendLookon.m_applay = m_resetUser;
				sendLookon.m_userId = pUser->m_userData.m_id;
				sendLookon.m_time = m_resetTime - gWork.GetCurTime().Secs();
				sendLookon.m_flag = 1;
				sendLookon.m_isLookOn = 1;
				for (Lint j = 0; j < m_iPlayerCapacity; ++j)
				{
					if (m_user[j] == NULL)
						continue;
					if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
					{
						sendLookon.m_agree.push_back(m_user[j]->m_userData.m_nike);
						sendLookon.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
						sendLookon.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
					}
					else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
					{
						sendLookon.m_wait.push_back(m_user[j]->m_userData.m_nike);
						sendLookon.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
						sendLookon.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
					}
					else
					{
						sendLookon.m_refluse = m_user[j]->m_userData.m_nike;
					}
				}
				MHBoadCastDeskLookonUser(sendLookon);
			}
		}
	}
}

//玩家选择解散操作
void Desk::HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg)
{
	if (!pUser || !msg)
	{
		LLOG_ERROR("Desk::HanderSelectResutDesk() Error!!!, pUser or msg is NULL!!! deskId=[%d]", 
			this->GetDeskId());

		return;
	}
	//桌子必须为解散状态中
	if (m_resetTime == 0)
	{
		LLOG_ERROR("Desk::HanderSelectResutDesk() Error!!!, Desk no user opt reset!!! deskId=[%d], userId=[%d], resetTime=[%d]",
			this->GetDeskId(), pUser->GetUserDataId(), m_resetTime);

		return;
	}
		
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::HanderSelectResutDesk() Run Error, this player is not desk user!!! deskId=[%d], userId=[%d]", 
			this->GetDeskId(), pUser->GetUserDataId());
		return;
	}

	/*if (m_playStatus[pos] != 1)
	{
		LLOG_ERROR("Desk::HanderSelectResutDesk() Run Error, this player status is 2... deskId=[%d]", this->GetDeskId());
		return;
	}*/

	LLOG_DEBUG("Desk::HanderSelectResutDesk() Run... deskId=[%d], userId=[%d], pos =[%d], userOpt=[%d]",
		this->GetDeskId(), pUser->GetUserDataId(), pos, msg->m_flag);

	if (msg->m_flag < 1 || msg->m_flag >2)
	{
		msg->m_flag = 1;
	}

	//记录该玩家选择结果
	m_reset[pos] = msg->m_flag;

	//统计玩家选择的结果
	Lint agree = 0, refluse = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_reset[i] == RESET_ROOM_TYPE_AGREE)
			agree++;
		else if (m_reset[i] == RESET_ROOM_TYPE_REFLUSE)
			refluse++;
	}

	//有人拒绝
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

		//给观战玩家弹窗
		if (!m_desk_Lookon_user.empty())
		{
			LMsgS2CResetDesk sendLookon;
			sendLookon.m_errorCode = 4;
			sendLookon.m_applay = m_resetUser;
			sendLookon.m_userId = pUser->m_userData.m_id;
			sendLookon.m_time = m_resetTime - gWork.GetCurTime().Secs();
			sendLookon.m_flag = 1;
			sendLookon.m_isLookOn = 1;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if (m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					sendLookon.m_agree.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					sendLookon.m_wait.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					sendLookon.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			MHBoadCastDeskLookonUser(sendLookon);
		}

		//m_deskState = m_deskbeforeState;
		ResetClear();
		if (mGameHandler)
		{
			mGameHandler->OnResetDeskEnd();
		}

		//由于网络原因可能某些玩家已经准备但延迟收到解散命令,当有人拒绝的时候应该再次check一下
		LLOG_DEBUG("Logwyz ....  MHCheckGameStart()    380     ");
		MHCheckGameStart();
		//CheckGameStart();
	}
	//else if (agree >= (GetPlayUserCount()))  //修改，只需要正在参与游戏玩家同意
	//所有参与游戏的玩家已经同意解散，走解散
	else if (agree >= (GetUserCount()))    //修改,解散只需要参加游戏的人同意
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
			send.m_isShowFee = (m_vip && m_vip->m_curCircle == 0) ? 1 : 0;

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

		//给观战玩家弹窗
		if (!m_desk_Lookon_user.empty())
		{
			LMsgS2CResetDesk sendLookon;
			sendLookon.m_errorCode = 2;
			sendLookon.m_applay = m_resetUser;
			sendLookon.m_userId = pUser->m_userData.m_id;
			sendLookon.m_time = m_resetTime - gWork.GetCurTime().Secs();
			sendLookon.m_flag = 1;
			sendLookon.m_isShowFee = (m_vip && m_vip->m_curCircle == 0) ? 1 : 0;
			sendLookon.m_isLookOn = 1;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if (m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					sendLookon.m_agree.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					sendLookon.m_wait.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					sendLookon.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			MHBoadCastDeskLookonUser(sendLookon);
		}

		ResetEnd();
	}
	//没人拒绝，但是仍有玩家未操作
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

		//给观战玩家弹窗
		if (!m_desk_Lookon_user.empty())
		{
			LMsgS2CResetDesk sendLookon;
			sendLookon.m_errorCode = 0;
			sendLookon.m_applay = m_resetUser;
			sendLookon.m_userId = pUser->m_userData.m_id;
			sendLookon.m_time = m_resetTime - gWork.GetCurTime().Secs();
			sendLookon.m_flag = 1;
			sendLookon.m_isLookOn = 1;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if (m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					sendLookon.m_agree.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_agreeHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_agreeUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					sendLookon.m_wait.push_back(m_user[j]->m_userData.m_nike);
					sendLookon.m_waitHeadUrl.push_back(m_user[j]->m_userData.m_headImageUrl);
					sendLookon.m_waitUserId.push_back(m_user[j]->m_userData.m_id);
				}
				else
				{
					sendLookon.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			MHBoadCastDeskLookonUser(sendLookon);
		}
	}
}

//倒计时检测是否解散结果
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
	if (!pUser || !msg)
	{
		LLOG_ERROR("Desk::HanderUserReady() Error!!!, user or msg is NULL Error!!! deskId=[%d]", this->GetDeskId());
		return;
	}

	if (m_deskState != DESK_WAIT)
	{
		LLOG_ERROR("Desk::HanderUserReady() Error!!! deskState Error!!!, deskId=[%d], userId=[%d], deskState=[%d]",
			GetDeskId(), pUser->GetUserDataId(), m_deskState);
		return;
	}

	Lint pos = GetUserPos(pUser);
	if(pos == INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::HanderUserReady() Error!!! pos is invalid!!! deskId=[%d], userId=[%d], pos=%d", 
			GetDeskId(), pUser->GetUserDataId(), pos);
		return;
	}

	LLOG_DEBUG("Desk::HanderUserReady() Run... deskId=[%d], userId=[%d], pos=[%d]", 
		GetDeskId(), pUser->GetUserDataId(), pos);	

	LMsgS2CUserReady ready;
	ready.m_pos = pos;
	ready.m_readyUserCount = MHGetUserReadyCount() + 1;
	
	if (pos != INVAILD_POS_QIPAI)
	{
		BoadCast(ready);

		//通知观战玩家，有玩家准备
		MHBoadCastDeskLookonUser(ready);

		//通知俱乐部会长明牌观战，有玩家准备
		MHBoadCastClubOwerLookOnUser(ready);

		m_readyState[pos] = 1;

		if (m_next_draw_lookon && MHGetUserReadyCount() >= 2)
		{
			record_sanzhangpai_game_start();
		}
			
		pUser->ClearInRoomTime();

		int nRet = MHCheckGameStart();
		//符合游戏开始条件
		if (m_Greater2CanStart == 1 && nRet == 1)
		{
			//房主管理员开始游戏 && 游戏未开始
			if (m_isManagerStart && !MHIsDeskMatchBegin())
			{
				Lint t_managerIdCount = 0;
				std::vector<Lint> t_managerId;
				t_managerIdCount = MHGetManagerList(t_managerId);
				int nRet = MHCheckGameStart();

				LMsgS2CUserDeskCommand command;
				command.m_StartButtonPos = INVAILD_POS_QIPAI;
				command.m_StartButtonUserId = t_managerId;
				command.m_StartButtonUserIdCount = t_managerIdCount;
				command.m_StartButtonEnable = 1;
				command.m_Nike = "Plase Wait Manager Start Game...";

				if (!t_managerId.empty())
				{
					command.m_commandType = 2;
				}
				else
				{
					command.m_commandType = 3;
				}
				
				MHBoadCastManagerAndSeatUser(command, nRet, t_managerId);
			}

			//第一个入座玩家开始游戏 && 游戏没开始 
			else if (!m_isManagerStart && !MHIsDeskMatchBegin())
			{
				m_StartGameButtonPos = MHFirstSeatPos();

				if (m_StartGameButtonPos != INVAILD_POS_QIPAI)
				{
					LMsgS2CUserDeskCommand command;
					command.m_commandType = 1;
					command.m_StartButtonPos = m_StartGameButtonPos;
					if (command.m_StartButtonPos < MAX_CHAIR_COUNT && m_user[command.m_StartButtonPos])
					{
						command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
					}

					BoadCast(command);
				}
			}
		}

		else if (nRet == 2)
		{
			return;
		}
	}

	//统计已准备人数
	Lint t_deskUserCount = 0;
	Lint readyUserCount = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_readyState[i] == 1)readyUserCount++;
		if (m_user[i]) ++t_deskUserCount;
	}
	
	//新版牛牛准备人数到达指定人数后自动开局
	if (!m_isManagerStart && m_state == NiuNiu_New && m_playtype.size() > 5 && m_playtype[4] > 1 && m_vip && m_vip->m_curCircle == 0)
	{
		if (readyUserCount >= m_playtype[4])
		{
			MHHanderStartGame();
		}
	}
	else if (m_state == NiuNiu_New && m_vip->m_curCircle != 0 && m_isCoinsGame == 1 && m_clubInfo.m_clubId != 0)
	{
		bool t_flag = true;
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_handlerPlayerStatus[i] == 1 && m_readyState[i] != 1)
			{
				t_flag = false;
				break;
			}
		}

		if (t_flag)
		{
			MHHanderStartGame();
		}
	}
	//扯炫各种玩法都有可能出现在座位上观战
	else if (m_state == CheXuan && m_vip->m_curCircle != 0)
	{
		bool t_flag = true;
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			/*
			//能量场
			if (m_isCoinsGame == 1)
			{
				if (m_standUpPlayerSeatDown[i] != 0)continue;
				if ( m_handlerPlayerStatus[i] == 1 && m_readyState[i] != 1)
				{
					t_flag = false;
					break;
				}
			}
			//普通场
			else
			{
				if (m_handlerPlayerStatus[i] == 1 && m_readyState[i] != 1)
				{
					t_flag = false;
					break;
				}
			}
			*/

			if (m_standUpPlayerSeatDown[i] != 0)continue;
			if (m_handlerPlayerStatus[i] == 1 && m_readyState[i] != 1)
			{
				t_flag = false;
				break;
			}
		}

		if (t_flag)
		{
			MHHanderStartGame();
		}
	}

	//三张牌已准备
	else if (m_state == YingSanZhang)
	{
		if (readyUserCount == m_iPlayerCapacity)
		{
			if (m_auto_start_game_time_flag == 0)
			{
				m_auto_start_game_time.Now();
				m_auto_start_game_time_flag = 1;
			}
			//MHHanderStartGame();
		}
	}

	//其他游戏人满开始游戏
	else if (m_state == NiuNiu ||
		m_state == ShuangSheng ||
		m_state == SanDaEr ||
		SanDaYi == m_state ||
		WuRenBaiFen == m_state ||
		CheXuan == m_state)
	{
		if (readyUserCount == m_iPlayerCapacity)
		{
			MHHanderStartGame();
		}
	}
}

void Desk::HanderSelectSeat(User* pUser, LMsgC2SUserSelectSeat* msg)
{	
	Lint pos1 = GetUserPos(pUser);
	if (pos1!=INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::HanderSelectSeat  error, userid=%d  already has seat [%d]", pUser->GetUserDataId(), pos1);
		return;
	}

	if (m_deskState != DESK_WAIT /*&&m_deskState !=DESK_COUNT_RESULT*/)
	{
		LLOG_ERROR("Desk::HanderSelectSeat state error, userid=%d deskstate=%d", pUser->GetUserDataId(), m_deskState);
		return;
	}

	//用户选择的位置
	LLOG_DEBUG("Desk::HanderSelectSeat userid=%d pos=%d", pUser->GetUserDataId(), msg->m_pos);

	if (msg->m_pos < 0 || msg->m_pos >=INVAILD_POS_QIPAI)
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
		selectSeat1.m_StartGameButtonPos=INVAILD_POS_QIPAI;

		LLOG_DEBUG("Logwyz.......m_Greater2CanStart=[%d],m_StartGameButtonPos=[%d]", m_Greater2CanStart, m_StartGameButtonPos);
		//if (m_Greater2CanStart && m_StartGameButtonPos==INVAILD_POS_QIPAI)
		//{
		//		if (MHCheckGameStart()==1)  //可以开始游戏
		//		{
		//			//开始游戏，指定开始按钮用户
		//			m_StartGameButtonPos=MHSpecPersonPos();
		//			selectSeat1.m_StartGameButtonPos=m_StartGameButtonPos;
		//			LLOG_DEBUG("Logwyz.......select m_StartGameButtonPos=[%d]", m_StartGameButtonPos);
		//		}
		//}
		//>=2可以开局
		if (m_Greater2CanStart)
		{
			if (m_StartGameButtonPos==INVAILD_POS_QIPAI)
			{
				if (MHCheckGameStart()==1)  //可以开始游戏
				{
					if (m_deskCreatedType==0)
					{
						if (GetUserPos(m_creatUser)!=INVAILD_POS_QIPAI)
						{
							//开始游戏，指定开始按钮用户为房主
							m_StartGameButtonPos= GetUserPos(m_creatUser);
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
		selectSeat2.m_pos = INVAILD_POS_QIPAI;
		LLOG_DEBUG("Desk::HanderSelectSeat after selected userid=%d pos=%d", pUser->GetUserDataId(), selectSeat2.m_pos);
		pUser->Send(selectSeat2);
	}

}

void Desk::HanderUserOutDesk(User* pUser)
{
	if (pUser == NULL)
	{
		LLOG_ERROR("Desk::HanderUserOutDesk() Error... This User is NULL... deskId=[%d]", m_id);
		return;
	}

	LLOG_DEBUG("Desk::HanderUserOutDesk() Run... deskId=[%d], userId=[%d]", m_id, pUser->GetUserDataId());

	//如果该玩家不是入座玩家，则不通知其他人
	if (!MHIsRoomUser(pUser))
	{
		LLOG_DEBUG("Desk::HanderUserOutDesk() This user is not seat down user, So don't notify other user... deskId=[%d], userId=[%d]",
			m_id, pUser->GetUserDataId());
		return;
	}

	//61031：通知入座玩家谁掉线了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 0;	
	info.m_pos=GetUserPos(pUser);
	info.m_userid = pUser->m_userData.m_id;
	MHBoadCastAllDeskUserWithOutUser(info, pUser);

	//通知look on 玩家(**Ren 2018-5-20)
	MHBoadCastDeskLookonUser(info);

	//通知俱乐部会长明牌观战，有玩家掉线
	MHBoadCastClubOwerLookOnUser(info);
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
	//if (pos == INVAILD_POS_QIPAI)
	//{
	//	return;
	//}
	if (!MHIsRoomUser(pUser))
	{
		LLOG_ERROR("Logwyz Desk::HanderUserSpeak   user not in room");
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
		//if (speak.m_pos == INVAILD_POS_QIPAI) 
		//	return;
		speak.m_userId=pUser->GetUserDataId();
		speak.m_pos= GetUserPos(pUser);
		if (speak.m_pos==INVAILD_POS_QIPAI)
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
	speak.m_pos = GetUserPos(pUser);
	//if (m_selectSeat) {
	//	speak.m_pos = m_seat[pos];
	//}

	if (speak.m_pos == INVAILD_POS_QIPAI)
		return;

	speak.m_id = msg->m_id;
	speak.m_type = msg->m_type;
	speak.m_musicUrl = msg->m_musicUrl;
	speak.m_msg = msg->m_msg;
	//BoadCast(speak);
	MHBoadCastAllDeskUser(speak);

	LLOG_DEBUG("LOGWYZ USER SPEAK userid[%d], url[%s] ", pUser->GetUserDataId(), msg->m_musicUrl.c_str());
}

//断线重连
bool Desk::OnUserReconnect(User* pUser)
{
	if (pUser == NULL)
	{
		LLOG_ERROR("Desk::OnUserReconnect() Error... User is NULL... deskId=[%d]", m_id);
		return false;
	}

	if (!m_vip)
	{
		LLOG_ERROR("Desk::OnUserReconnect() Error... vip is NULL... deskId=[%d]", m_id);
		return false;
	}

	LLOG_INFO("Desk::OnUserReconnect() Run... deskId=[%d], userId=[%d]", m_id, pUser->GetUserDataId());

	Lint pos = GetUserPos(pUser);

	//如果该玩家没有入座，也没在该房间观战，则返回...
	if (pos == INVAILD_POS_QIPAI && !MHIsLookonUser(pUser) && pUser != this->m_clubOwerLookOnUser)
	{
		LLOG_ERROR("Desk::OnUserReconnect() Error... This user is not seat down user and not look on user... deskId=[%d], userId=[%d], pos=[%d], lookon_size=[%d]",
			m_id, pUser->GetUserDataId(), pos, m_desk_Lookon_user.size());
		return false;
	}
	Lint t_boboScore = 0;
	if (pos != INVAILD_POS_QIPAI)
	{
		//更新一下能量值
		if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
		{
			pUser->m_userData.m_coins = GetUserCoins(pUser);//m_vip->m_coins[pos];
		}

		//玩家钵钵数量
		if (mGameHandler && mGameHandler->GetPlayerBoBoScore(pUser) != -100000)
		{
			t_boboScore = mGameHandler->GetPlayerBoBoScore(pUser);
		}
	}

	//如果桌子处于等待状态，且入座的有效玩家，断线回来自动准备
	if (m_deskState == DESK_WAIT && IsValidSeatDownPlayer(pUser) && m_handlerPlayerStatus[pos] != 2)
	{
		LMsgS2CUserReady ready;
		ready.m_readyUserCount = MHGetUserReadyCount() + 1;
		ready.m_pos = pos;

		//广播给入座玩家
		MHBoadCastAllDeskUser(ready);

		//广播给观战玩家
		if (!m_desk_Lookon_user.empty())
		{
			MHBoadCastDeskLookonUser(ready);
		}

		m_readyState[pos] = 1;
		if (m_next_draw_lookon &&MHGetUserReadyCount() >= 2)
			record_sanzhangpai_game_start();
	}

	//61022：同步自己的信息
	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	//三张牌下一局开始倒计时时间
	if (m_state == YingSanZhang)
	{
		send1.m_nextDrawLimitTime = YINGSANZHANG_READY_DELAY_TIME;
		if (m_next_draw_lookon)
			send1.m_nextDrawLimitTime = YINGSANZHANG_GAME_START_DELAY_TIME;
	}
	//俱乐部
	if (m_clubInfo.m_clubId != 0 && m_clubInfo.m_clubDeskId != 0 && m_clubInfo.m_showDeskId != 0)
	{
		send1.m_deskId = m_clubInfo.m_showDeskId;
		send1.m_clubName = m_clubInfo.m_clubName;
	}
	send1.m_pos = pos;

	//该玩家是入座玩家
	if(pos != INVAILD_POS_QIPAI)
	{ 
		send1.m_ready = m_readyState[pos] && m_handlerPlayerStatus[pos] != 2;
		send1.m_isClubOwerLooking = this->m_clubOwerLookOnUser ? 1 : 0;
	}
	//该玩家是观战玩家或会长明牌
	else if (pos == INVAILD_POS_QIPAI && (MHIsLookonUser(pUser) || pUser == this->m_clubOwerLookOnUser))
	{ 
		send1.m_ready = 0;
		send1.m_isLookOn = 1;
		send1.m_pos = m_iPlayerCapacity;
	}
	send1.m_score = m_vip ? m_vip->GetUserScore(pUser) : 0;
	if (pos != INVAILD_POS_QIPAI && m_clubInfo.m_clubId != 0 && m_isCoinsGame)
	{
		if (m_deskState != DESK_WAIT && m_state == NiuNiu_New && mGameHandler && mGameHandler->GetPlayerAccumCoins(pUser) != -100000)
		{
			send1.m_score = mGameHandler->GetPlayerAccumCoins(pUser);
		}
		else
		{
			send1.m_score = GetUserCoins(pUser);//m_vip->m_coins[pos];
		}
	}
	send1.m_curBoBoScore = t_boboScore;
	send1.m_coins = GetUserCoins(pUser);//m_vip->m_coins[pos];
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
	send1.m_StartGameButtonPos= INVAILD_POS_QIPAI;
	if (send1.m_StartGameButtonPos > INVAILD_POS_MANAGER_START && send1.m_StartGameButtonPos < MAX_CHAIR_COUNT && m_user[send1.m_StartGameButtonPos] != NULL)
	{
		send1.m_nike = m_user[send1.m_StartGameButtonPos]->m_userData.m_nike;
	}
	send1.m_gamePlayerCount=m_desk_user_count;
	send1.m_startButtonAppear=m_startButtonAppear;
	send1.m_clubId=m_clubInfo.m_clubId;
	send1.m_playTypeId=m_clubInfo.m_playTypeId;
	send1.m_clubDeskId = m_clubInfo.m_clubDeskId;
	send1.m_GpsLimit = this->m_Gps_Limit;
	send1.m_dynamicJoin = (m_playStatus[pos] == 2 ? 1 : 0);
	send1.m_allowLookOn = m_allowLookOn;
	send1.m_clubOwerLookOn = m_clubOwerLookOn;
	send1.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
	send1.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	send1.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
	send1.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
	send1.m_warnScore = this->m_warnScore;
	send1.m_myRole = GetClubDeskUserRole(pUser);
	send1.m_isMedal = pUser->m_isMedal;
	GetDeskSeatStatus(send1.m_deskUserStatus);
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame && m_vip->IsBegin())
	{
		memcpy(send1.m_deskUserStatus, m_handlerPlayerStatus, sizeof(Lint)*10);
	}
	pUser->Send(send1);

	LLOG_ERROR("Desk::OnUserReconnect() deskId=[%d], pos:%d, userid:%d, ip:%s, m_deskCreatedType = %d, m_StartGameButtonPos:[%d], m_desk_user_count=[%d],m_startButtonAppear=[%d],LOOKON[%d]",
		this->m_id, pos, pUser->GetUserDataId(), send1.m_userIp.c_str(), send1.m_deskCreatedType, m_StartGameButtonPos, m_desk_user_count, m_startButtonAppear, send1.m_isLookOn);

	//61023：把其他座位人广播给断线重连进来的玩家
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i]!=NULL && m_user[i] != pUser)
		{
			LMsgS2CDeskAddUser addmsg2;
			addmsg2.m_userId= m_user[i]->GetUserDataId();
			addmsg2.m_pos = i;			
			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i] && m_handlerPlayerStatus[i] != 2;
			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
			addmsg2.m_ip = m_user[i]->m_userData.m_customString1;
			addmsg2.m_score= m_vip ? m_vip->GetUserScore(m_user[i]):0;
			if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
			{
				if (m_deskState != DESK_WAIT && m_state == NiuNiu_New && mGameHandler && mGameHandler->GetPlayerAccumCoins(m_user[i]) != -100000)
				{
					addmsg2.m_score = mGameHandler->GetPlayerAccumCoins(m_user[i]);
				}
				else
				{
					addmsg2.m_score = GetUserCoins(m_user[i]);//m_vip->m_coins[pos];
				}
			}
			addmsg2.m_curBoBoScore = (mGameHandler && mGameHandler->GetPlayerBoBoScore(m_user[i]) != -100000) ? mGameHandler->GetPlayerBoBoScore(m_user[i]) : 0;
			addmsg2.m_online = m_user[i]->GetOnline();
			addmsg2.m_coins = GetUserCoins(m_user[i]);//m_vip->m_coins[i];
			addmsg2.m_credits = m_user[i]->m_userData.m_creditValue;
			addmsg2.m_userGps= m_user[i]->m_userData.m_customString2;
			addmsg2.m_cheatAgainst=m_cheatAgainst;
			addmsg2.m_videoPermission = m_user[i]->m_videoPermission;
			addmsg2.m_dynamicJoin=(m_playStatus[i]==2?1:0);
			addmsg2.m_allowLookOn = m_allowLookOn;
			addmsg2.m_clubOwerLookOn = m_clubOwerLookOn;
			addmsg2.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
			addmsg2.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
			addmsg2.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
			addmsg2.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
			addmsg2.m_isMedal = m_user[i]->m_isMedal;
			pUser->Send(addmsg2);
		}
	}

	//61031：通知谁回来了（观战玩家不需要通知其他人他回来了）
	if (pos != INVAILD_POS_QIPAI || pUser == this->m_clubOwerLookOnUser)
	{
		LMsgS2CUserOnlineInfo info;
		info.m_flag = 1;
		info.m_pos = pos;
		info.m_userid = pUser->GetUserDataId();

		//广播给出自己之外所有桌子上的玩家
		MHBoadCastDeskUserWithoutUser(info, pUser);

		//如果有观战用户则广播给观战用户
		if (!this->m_desk_Lookon_user.empty() && pUser != this->m_clubOwerLookOnUser)
		{
			MHBoadCastDeskLookonUser(info);
		}
		if (pUser != this->m_clubOwerLookOnUser && this->m_clubOwerLookOn == 1 && this->m_clubOwerLookOnUser)
		{
			//广播给俱乐部会长明牌观战
			MHBoadCastClubOwerLookOnUser(info);
		}
	}

	//观战玩家牌局中等待状态发送
	if (m_vip && m_deskState == DESK_WAIT && (pos == INVAILD_POS_QIPAI || pUser != this->m_clubOwerLookOnUser))
	{
		LMsgS2CVipInfo info;
		info.m_curCircle = m_vip->m_curCircle;
		info.m_curMaxCircle = m_vip->m_maxCircle;
		info.m_playtype = m_playtype;
		pUser->Send(info);
	}

	//游戏处于未开始阶段，开始按钮出现情况
	if (!MHIsDeskMatchBegin())
	{
		int nRet = MHCheckGameStart();

		//管理员开始游戏
		if (m_isManagerStart)
		{
			Lint t_managerIdCount = 0;
			std::vector<Lint> t_managerId;
			t_managerIdCount = MHGetManagerList(t_managerId);
			
			LMsgS2CUserDeskCommand command;
			command.m_StartButtonPos = INVAILD_POS_QIPAI;
			command.m_StartButtonUserId = t_managerId;
			command.m_StartButtonUserIdCount = t_managerIdCount;
			command.m_StartButtonEnable = (m_Greater2CanStart == 1 && nRet == 1) ? 1 : 0;
			command.m_Nike = "Plase Wait Manager Start Game...";

			if (!t_managerId.empty())
			{
				command.m_commandType = 2;
			}
			else
			{
				command.m_commandType = 3;
			}

			MHBoadCastManagerAndSeatUser(command, nRet, t_managerId);
		}
		//第一个入座玩家开始游戏 && 游戏没开始 
		else if (!m_isManagerStart)
		{
			LMsgS2CUserDeskCommand command;
			command.m_commandType = 1;
			if (m_Greater2CanStart == 1 && nRet == 1)
			{
				m_StartGameButtonPos = MHFirstSeatPos();
				command.m_StartButtonPos = m_StartGameButtonPos;
				if (command.m_StartButtonPos < MAX_CHAIR_COUNT && m_user[command.m_StartButtonPos])
				{
					command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
				}
			}
			else
			{
				m_StartGameButtonPos = INVAILD_POS_QIPAI;
				command.m_StartButtonPos = m_StartGameButtonPos;
			}
			BoadCast(command);
		}
	}

	//桌子处于未开始或者结算阶段
	if (m_deskState == DESK_WAIT && IsValidSeatDownPlayer(pUser) && m_vip->m_curCircle != 0)
	{
		//发送当前圈数信息
		if (m_vip)
		{
			m_vip->SendInfo();
			if (m_state == TuiTongZi  && (m_vip->m_curCircle != 0))
			{
				ReadyRemainTime remainTime;
				remainTime.m_currCirCle = m_vip->m_curCircle;
				remainTime.m_remainTime = TUITONGZI_READY_DELAY_TIME - (time(NULL) - m_round_end_time.Secs());
				pUser->Send(remainTime);
			}

			if (m_state == NiuNiu &&  m_vip->m_curCircle != 0)
			{
				ReadyRemainTime remainTime;
				remainTime.m_currCirCle = m_vip->m_curCircle;
				remainTime.m_remainTime = NIUNIU_READY_DELAY_TIME - (LTime().Secs() - m_round_end_time.Secs());
				if (remainTime.m_remainTime < 0) remainTime.m_remainTime = 0;
				if (remainTime.m_remainTime>  NIUNIU_READY_DELAY_TIME) remainTime.m_remainTime = NIUNIU_READY_DELAY_TIME;
				pUser->Send(remainTime);
			}
			if (NiuNiu_New == m_state && m_vip->m_curCircle != 0)
			{
				ReadyRemainTime remainTime;
				remainTime.m_currCirCle = m_vip->m_curCircle;
				remainTime.m_remainTime = NEWNIUNIU_READY_DELAY_TIME - (LTime().Secs() - m_round_end_time.Secs());
				if (remainTime.m_remainTime < 0) remainTime.m_remainTime = 0;
				if (remainTime.m_remainTime > NEWNIUNIU_READY_DELAY_TIME) remainTime.m_remainTime = NEWNIUNIU_READY_DELAY_TIME;
				pUser->Send(remainTime);
			}
			if ( m_state == YingSanZhang && m_vip&& m_vip->m_curCircle!=0)
			{
				ReadyRemainTime remainTime;
				remainTime.m_currCirCle = m_vip->m_curCircle;
				remainTime.m_remainTime = YINGSANZHANG_READY_DELAY_TIME;
				pUser->Send(remainTime);
			}
			//双升106
			if (m_state == ShuangSheng && m_vip && m_vip->m_curCircle != 0)
			{

			}
			//三打二107
			if (m_state == SanDaEr && m_vip && m_vip->m_curCircle != 0)
			{

			}
			//三打一109
			if (m_state == SanDaYi && m_vip && m_vip->m_curCircle != 0)
			{

			}
			//五人百分110
			if (WuRenBaiFen == m_state && m_vip && m_vip->m_curCircle != 0)
			{

			}
			//扯炫111
			if (CheXuan == m_state && m_vip && m_vip->m_curCircle != 0)
			{
				ReadyRemainTime remainTime;
				remainTime.m_currCirCle = m_vip->m_curCircle;
				remainTime.m_remainTime = YINGSANZHANG_READY_DELAY_TIME;
				pUser->Send(remainTime);
			}
		}
		
		int nRet = MHCheckGameStart();
		//统计已准备人数
		Lint t_deskUserCount = 0;
		Lint readyUserCount = 0;
		for (Lint i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_readyState[i] == 1)readyUserCount++;
			if (m_user[i]) ++t_deskUserCount;
		}

		//新版牛牛准备人数到达指定人数后自动开局
		if (m_state == NiuNiu_New && m_vip->m_curCircle != 0 && m_isCoinsGame == 1 && m_clubInfo.m_clubId != 0)
		{
			bool t_flag = true;
			for (Lint i = 0; i < m_iPlayerCapacity; ++i)
			{
				if (m_handlerPlayerStatus[i] == 1 && m_readyState[i] != 1)
				{
					t_flag = false;
					break;
				}
			}

			if (t_flag)
			{
				MHHanderStartGame();
			}
		}

		//三张牌已准备
		else if (m_state == YingSanZhang)
		{
			if (readyUserCount == m_iPlayerCapacity)
			{
				if (m_auto_start_game_time_flag == 0)
				{
					m_auto_start_game_time.Now();
					m_auto_start_game_time_flag = 1;
				}
				//MHHanderStartGame();
			}
		}

		//其他游戏人满开始游戏
		else if (m_state == NiuNiu ||
			m_state == ShuangSheng ||
			m_state == SanDaEr ||
			SanDaYi == m_state ||
			WuRenBaiFen == m_state ||
			CheXuan == m_state)
		{
			if (readyUserCount == m_iPlayerCapacity)
			{
				MHHanderStartGame();
			}
		}
		/*
		Lint nRet = MHCheckGameStart();
		if (m_Greater2CanStart == 1 && nRet == 1 && m_StartGameButtonPos == INVAILD_POS_QIPAI) //动态开局且满足2人准备
		{
			//推送游戏开始按钮		
			LMsgS2CUserDeskCommand command;
			command.m_commandType = 1;
			if (m_deskCreatedType == 0) //桌主参与游戏
			{
				command.m_StartButtonPos = m_StartGameButtonPos = 0;
			}
			else
			{
				command.m_StartButtonPos = m_StartGameButtonPos = MHSpecPersonPos();
			}
			if (command.m_StartButtonPos < MAX_CHAIR_COUNT  && m_user[command.m_StartButtonPos])
				command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
			BoadCast(command);
		}
		else if (m_Greater2CanStart == 1 && nRet == 1 && m_StartGameButtonPos != INVAILD_POS_QIPAI)
		{
			//推送游戏开始按钮		
			LMsgS2CUserDeskCommand command;
			command.m_commandType = 1;
			command.m_StartButtonPos = m_StartGameButtonPos;
			if (command.m_StartButtonPos < MAX_CHAIR_COUNT && m_user[command.m_StartButtonPos])
				command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
			BoadCast(command);
		}

		//统计已准备人数
		Lint t_deskUserCount = 0;
		Lint readyUserCount = 0;
		for (Lint i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_readyState[i] == 1)readyUserCount++;
			if (m_user[i]) ++t_deskUserCount;
		}

		//新版牛牛准备人数到达指定人数后自动开局
		if (m_state == NiuNiu_New && m_playtype.size() > 5 && m_playtype[4] > 1 && m_vip && m_vip->m_curCircle == 0)
		{
			if (readyUserCount >= m_playtype[4])
			{
				MHHanderStartGame();
			}
		}
		else if (m_state == NiuNiu_New && m_vip->m_curCircle != 0 && m_isCoinsGame == 1 && m_clubInfo.m_clubId != 0)
		{
			bool t_flag = true;
			for (Lint i = 0; i < m_iPlayerCapacity; ++i)
			{
				if (m_handlerPlayerStatus[i] == 1 && m_readyState[i] != 1)
				{
					t_flag = false;
					break;
				}
			}

			if (t_flag)
			{
				MHHanderStartGame();
			}
		}
		*/
	}
	//桌子处于结算等待阶段，观战玩家断线回来
	else if (m_deskState == DESK_WAIT && !IsValidSeatDownPlayer(pUser) && m_vip->m_curCircle != 0)
	{
		bool t_flagRead = true;
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_user[i] && m_readyState[i] == 1)
			{
				t_flagRead = false;
				break;
			}
		}
		if (t_flagRead && mGameHandler)
		{
			mGameHandler->LookUserReconnectOnDeskWait(pUser);
		}
	}
	//桌子处于打牌阶段，玩家断线回来
	else if (m_deskState != DESK_WAIT)    //(**Ren 2018-5-18)
	{
		//发送当前圈数信息
		//if(m_vip)
		//{
		//	m_vip->SendInfo();
		//}

		if(mGameHandler)
		{
			mGameHandler->OnUserReconnect(pUser);
		}
	}

	//该玩家是观战玩家，需要通知桌子是否已满
	if (pos == INVAILD_POS_QIPAI && MHIsLookonUser(pUser))
	{
		if (!this->GetVip() || this->GetVip()->IsFull(pUser) || this->GetUserCount() == this->GetPlayerCapacity())
		{
			LLOG_DEBUG("Desk::OnUserReconnect() Error... This desk has full, deskId=[%d], userId=[%d]", this->GetDeskId(), pUser->GetUserDataId());
			LMsgS2CLookOnPlayerFill fill;
			fill.m_errorCode = 1;
			fill.m_state = this->m_state;

			pUser->Send(fill);
		}
	}

	//充能回来的玩家
	LMsgS2CLookOnPlayerFill fill1;
	fill1.m_errorCode = 3;
	fill1.m_state = this->m_state;
	for (Lint i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_standUpPlayerSeatDown[i] != 3)continue;
		if (m_user[i])
		{
			m_user[i]->Send(fill1);
		}
	}

	

	if(m_resetTime)
	{
	//如果有人申请解散则通知解散信息
		//解散的时候是游戏已经开始，直接用m_user的位置
		Lint userPos = GetUserPos(pUser);

		LMsgS2CResetDesk send;
		send.m_errorCode = 0;
		send.m_applay = m_resetUser;
		send.m_userId = m_creatUserId;
		send.m_time = m_resetTime - gWork.GetCurTime().Secs();
		if (userPos == INVAILD_POS_QIPAI)
		{
			send.m_flag = 1;
		}
		else
		{
			send.m_flag = m_reset[userPos] ? 1 : 0;
		}

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
		pUser->Send(send);
	}

	LLOG_ERROR("*****断线重连桌子 deskID: %d, userid:%d ", m_id, pUser->GetUserDataId());
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i]) {
			LLOG_ERROR("******用户ID = %d IP = %s", m_user[i]->GetUserData().m_id, m_user[i]->m_userData.m_customString1.c_str());
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

		//子游戏根据小选项设置一些变量
		set_some_spec(vip->m_state,vip->m_playtype);
	}
}

VipLogItem* Desk::GetVip()
{
	return m_vip;
}

//获取桌子上所有座位状态
void Desk::GetDeskSeatStatus(Lint deskSeatStatus[])
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] != NULL) deskSeatStatus[i] = 1;
	}
}

Lint Desk::GetFreePos()
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] == NULL)
			return i;
	}

	return INVAILD_POS_QIPAI;
}

//进入房间观战
void Desk::OnUserInRoomLookOn(User* user)
{
	//user为空
	if (user == NULL)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOn() user is NULL desk=[%d]", m_id);
		return;
	}

	//vip对象为空
	if (!m_vip)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOn() Error... The Vip Desk is NULL... deskId=[%d], userId=[%d]", m_id, user->GetUserDataId());
		return;
	}

	//该玩家已经入座该桌子
	if (GetUserPos(user) != INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOn() Error... This user has in this desk...deskId=[%d], userId=[%d]", m_id, user->GetUserDataId());
		return;
	}

	LLOG_ERROR("Desk::OnUserInRoomLookOn() Run... deskId=[%d], userId=[%d]", m_id, user->GetUserDataId());

	//是俱乐部 && 允许会长明牌观战 && 玩家ID为会长ID
	if (m_clubInfo.m_clubId != 0 && this->m_clubOwerLookOn == 1 && this->m_creatUserId == user->GetUserDataId())
	{
		//会长明牌观看
		m_clubOwerLookOnUser = user;
	}
	else
	{
		//添加改玩家到Look_on_user list中
		m_desk_Lookon_user.push_back(user);
	}

	//更新客户端观战玩家数量
	UpdateLookOnUserCount();

	//关联玩家与桌子
	user->SetDesk(this);

	// 61022:玩家进入房间首先成为观战玩家，将该玩家的信息发给自己
	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	//俱乐部
	if (m_clubInfo.m_clubId != 0 && m_clubInfo.m_clubDeskId != 0 && m_clubInfo.m_showDeskId != 0)
	{
		send1.m_deskId = m_clubInfo.m_showDeskId;
		send1.m_clubName = m_clubInfo.m_clubName;
	}
	send1.m_pos = m_iPlayerCapacity;
	send1.m_ready = 0;
	send1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	send1.m_curBoBoScore = 0;
	send1.m_coins = GetUserCoins(user);//user->GetUserData().m_coins;
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
	send1.m_Greater2CanStart = m_Greater2CanStart;
	send1.m_StartGameButtonPos = m_StartGameButtonPos;
	if (send1.m_StartGameButtonPos > INVAILD_POS_MANAGER_START && send1.m_StartGameButtonPos < MAX_CHAIR_COUNT && m_user[send1.m_StartGameButtonPos] != NULL)
	{
		send1.m_nike = m_user[send1.m_StartGameButtonPos]->m_userData.m_nike;
	}
	send1.m_gamePlayerCount = m_desk_user_count;
	send1.m_startButtonAppear = m_startButtonAppear;
	send1.m_clubId = m_clubInfo.m_clubId;
	send1.m_playTypeId = m_clubInfo.m_playTypeId;
	send1.m_clubDeskId = m_clubInfo.m_clubDeskId;
	send1.m_GpsLimit = m_Gps_Limit;
	send1.m_dynamicJoin = 1;   //(m_playStatus[pos] == 2 ? 1 : 0);
	send1.m_isLookOn = 1;		//设置玩家出去旁观状态
	send1.m_allowLookOn = m_allowLookOn;
	send1.m_clubOwerLookOn = m_clubOwerLookOn;
	send1.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
	send1.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	send1.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
	send1.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
	send1.m_warnScore = this->m_warnScore;
	send1.m_myRole = GetClubDeskUserRole(user);
	send1.m_isMedal = user->m_isMedal;
	GetDeskSeatStatus(send1.m_deskUserStatus);
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame && m_vip->IsBegin())
	{
		memcpy(send1.m_deskUserStatus, m_handlerPlayerStatus, sizeof(Lint) * 10);
	}
	user->Send(send1);

	// 给该旁观者推送房间已入座玩家的消息
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] != NULL && m_user[i] != user)
		{
			//61023：玩家加入房间成观战状态，服务器推送房间中所有已入座玩家信息给该观战玩家
			LMsgS2CDeskAddUser addmsg2;
			addmsg2.m_userId = m_user[i]->GetUserDataId();
			addmsg2.m_pos = i;
			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_score = m_vip ? m_vip->GetUserScore(m_user[i]) : 0;
			if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
			{
				if (m_deskState != DESK_WAIT && m_state == NiuNiu_New && mGameHandler)
				{
					addmsg2.m_score = mGameHandler->GetPlayerAccumCoins(m_user[i]);
				}
				else
				{
					addmsg2.m_score = GetUserCoins(m_user[i]);//m_vip->m_coins[pos];
				}
			}
			addmsg2.m_curBoBoScore = (mGameHandler && mGameHandler->GetPlayerBoBoScore(m_user[i]) != -100000) ? mGameHandler->GetPlayerBoBoScore(m_user[i]) : 0;
			addmsg2.m_online = m_user[i]->GetOnline();
			addmsg2.m_coins = GetUserCoins(m_user[i]);//m_vip->m_coins[i];
			addmsg2.m_credits = m_user[i]->m_userData.m_creditValue;
			addmsg2.m_userGps = m_user[i]->m_userData.m_customString2;
			addmsg2.m_cheatAgainst = m_cheatAgainst;
			addmsg2.m_videoPermission = m_user[i]->m_videoPermission;
			addmsg2.m_dynamicJoin = (m_playStatus[i] == 2 ? 1 : 0);
			addmsg2.m_allowLookOn = m_allowLookOn;
			addmsg2.m_clubOwerLookOn = m_clubOwerLookOn;
			addmsg2.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
			addmsg2.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
			addmsg2.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
			addmsg2.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
			addmsg2.m_isMedal = m_user[i]->m_isMedal;
			user->Send(addmsg2);
		}
	}

	//通知manager，有玩家进入房间
	if (m_clubInfo.m_clubId != 0 && m_clubInfo.m_clubDeskId != 0)
	{
		//LLOG_DEBUG("Desk::OnUserInRoomLookOn LMsgL2LMGUserAddClubDesk");
		LMsgL2LMGUserAddClubDesk  send;
		send.m_type = 1;   //旁观标志
		send.m_clubId = m_clubInfo.m_clubId;
		send.m_playTypeId = m_clubInfo.m_playTypeId;
		send.m_clubDeskId = m_clubInfo.m_clubDeskId;
		send.m_strUUID = user->m_userData.m_unioid;
		send.m_userInfo.m_userId = user->GetUserDataId();
		//send.m_userInfo.m_deskPlayerName = user->m_userData.m_nike;
		//send.m_userInfo.m_deskPlayerUrl = user->m_userData.m_headImageUrl;
		//send.m_userInfo.m_pos = pos;
		//if (this->m_clubOwerLookOn == 1 && this->m_clubOwerLookOnUser)
		//{
		//	send.m_isClubOwerLooking = 1;
		//}

		gWork.SendToLogicManager(send);
	}

	//俱乐部会长明牌观战，通知已入座的玩家，
	//if (m_clubInfo.m_clubId != 0 && this->m_clubOwerLookOn == 1 && this->m_creatUserId == user->GetUserDataId())
	//{
	//	LMsgS2CDeskAddUser addmsg1;
	//	addmsg1.m_userId = user->GetUserDataId();
	//	addmsg1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	//	addmsg1.m_pos = m_iPlayerCapacity;
	//	addmsg1.m_nike = user->m_userData.m_nike;
	//	addmsg1.m_sex = user->m_userData.m_sex;
	//	addmsg1.m_ip = user->m_userData.m_customString1;
	//	addmsg1.m_face = user->m_userData.m_headImageUrl;
	//	addmsg1.m_online = user->GetOnline();
	//	addmsg1.m_coins = user->GetUserData().m_coins;
	//	addmsg1.m_credits = user->m_userData.m_creditValue;
	//	addmsg1.m_userGps = user->m_userData.m_customString2;
	//	addmsg1.m_cheatAgainst = m_cheatAgainst;
	//	addmsg1.m_videoPermission = user->m_videoPermission;
	//	addmsg1.m_dynamicJoin = 0;
	//	addmsg1.m_allowLookOn = m_allowLookOn;
	//	addmsg1.m_clubOwerLookOn = m_clubOwerLookOn;
	//	addmsg1.m_isClubOwerLooking = 1;
	//	addmsg1.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
	//	addmsg1.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	//	addmsg1.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
	//	addmsg1.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
	//
	//	//广播给桌子上以入座的玩家，会长明牌观战进入房间
	//	MHBoadCastDeskSeatUser(addmsg1);
	//
	//	//通知LogicManager会长处于明牌观战
	//	LLOG_DEBUG("Desk::HanderSelectSeat LMsgL2LMGUserAddClubDesk");
	//	LMsgL2LMGUserAddClubDesk  send;
	//	send.m_clubId = m_clubInfo.m_clubId;
	//	send.m_playTypeId = m_clubInfo.m_playTypeId;
	//	send.m_clubDeskId = m_clubInfo.m_clubDeskId;
	//	send.m_userInfo.m_userId = user->GetUserDataId();
	//	send.m_strUUID = user->m_userData.m_unioid;
	//	send.m_userInfo.m_pos = m_iPlayerCapacity;
	//	send.m_isClubOwerLooking = 1;
	//
	//	gWork.SendToLogicManager(send);
	//}

	//如果桌子处于等待状态，则给所有玩家（入座+观望）推送局数消息
	if (m_deskState == DESK_WAIT)
	{
		if (m_vip)
		{
			m_vip->SendInfo();
		}
	}

	//如果桌子处于游戏中，则走断线重连为该lookon玩家推送牌局信息
	if (m_deskState == DESK_PLAY && mGameHandler)
	{
		mGameHandler->OnUserReconnect(user);
	}
	else if (m_deskState == DESK_WAIT && !IsValidSeatDownPlayer(user) && m_vip->m_curCircle != 0)
	{
		bool t_flagRead = true;
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_user[i] && m_readyState[i] == 1)
			{
				t_flagRead = false;
				break;
			}
		}
		if (t_flagRead && mGameHandler)
		{
			mGameHandler->LookUserReconnectOnDeskWait(user);
		}
	}

	//开始后不允许加入游戏
	if (MHIsDeskMatchBegin() && m_isAllowDynamicIn == 0)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOn() Error!!! Game start, not allow player dynamic in error!!! deskId=[%d], userId=[%d]",
			m_id, user->GetUserDataId());

		//62065：通知观战玩家，游戏开始后不允许动态加入
		LMsgS2CLookOnPlayerFill fill;
		fill.m_errorCode = 2;   //游戏开始不允许动态加入
		fill.m_state = this->m_state;

		//62065：广播给所有观战玩家
		MHBoadCastDeskLookonUser(fill);
	}
	//如果此时入座玩家已满，则通知所有lookon玩家不显示入座按钮
	else if (!this->GetVip() || this->GetVip()->IsFull(user) || this->GetUserCount() == this->GetPlayerCapacity())
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOn() Error!!! desk has no empty pos error!!! deskId=[%d], userId=[%d]",
			m_id, user->GetUserDataId());

		//62065：通知观战玩家，该桌子已满
		LMsgS2CLookOnPlayerFill fill;
		fill.m_errorCode = 1;
		fill.m_state = this->m_state;

		//62065：广播给所有观战玩家
		MHBoadCastDeskLookonUser(fill);
	}

	//管理员开始游戏 && 游戏未开始
	if (m_isManagerStart && !MHIsDeskMatchBegin())
	{
		Lint t_managerIdCount = 0;
		std::vector<Lint> t_managerId;
		t_managerIdCount = MHGetManagerList(t_managerId);
		int nRet = MHCheckGameStart();

		LMsgS2CUserDeskCommand command;
		command.m_StartButtonPos = INVAILD_POS_QIPAI;
		command.m_StartButtonUserId = t_managerId;
		command.m_StartButtonUserIdCount = t_managerIdCount;
		command.m_StartButtonEnable = 0;
		command.m_Nike = "Plase Wait Manager Start Game...";

		if (m_Greater2CanStart == 1 && nRet == 1)
		{
			command.m_StartButtonEnable = 1;

			if (!t_managerId.empty())
			{
				command.m_commandType = 2;
			}
			else
			{
				command.m_commandType = 3;
			}
		}
		MHBoadCastManagerAndSeatUser(command, nRet, t_managerId);
	}
}

//观战状态入座
void Desk::OnUserInRoomLookOnSeatDown(User* user)
{
	if (user == NULL)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error... user is NULL deskId=[%d]", m_id);
		return;
	}

	if (!m_vip)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error...Vip desk is NULL, deskId=[%d], userId=[%d] ", m_id, user->GetUserDataId());
		return;
	}

	//该玩家已经入座该桌子
	Lint pos1 = GetUserPos(user);
	//能量场玩家留座观战状态再次入座
	if (pos1 != INVAILD_POS_QIPAI && m_clubInfo.m_clubId != 0 && m_isCoinsGame == 1 && m_standUpPlayerSeatDown[pos1] == 3)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Run...Player in coins game reseat, deskId=[%d], userId=[%d]", 
			m_id, user->GetUserDataId());
		m_standUpPlayerSeatDown[pos1] = 1;

		LMsgS2CLookOnUserSeatDownRet ret1;
		ret1.m_errorCode = 14;
		ret1.m_pos = pos1;
		ret1.m_deskId = GetDeskId();
		user->Send(ret1);
		return;
	}
	//普通场留座观战的玩家 或 已经站起的玩家再次入座
	else if (pos1 != INVAILD_POS_QIPAI && m_standUpPlayerSeatDown[pos1] == 3)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Run...Player in common game reseat, deskId=[%d], userId=[%d]",
			m_id, user->GetUserDataId());

		m_standUpPlayerSeatDown[pos1] = 1;
		if (mGameHandler)
		{
			mGameHandler->HanderPlayerSeat(user, 0);
		}
		return;
	}
	//正常的入座命令，如果玩家在座位上，则不可以再次走入座流程
	else if (pos1 != INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Err!!! Player has in desk...deskId=[%d], userId=[%d]", 
			m_id, user->GetUserDataId());

		return;
	}

	//如果不是LookOn玩家, 也不是会长明牌观战
	if (!MHIsLookonUser(user) && user != this->m_clubOwerLookOnUser)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error...This player is no look on user, deskId=[%d], userId=[%d]", m_id, user->GetUserDataId());
		return;
	}

	LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Run...deskId=[%d], userId=[%d]", m_id, user->GetUserDataId());

	//62067：服务器返回观战玩家入座结果
	LMsgS2CLookOnUserSeatDownRet ret;
	ret.m_errorCode = 0;

	//开始后不允许加入游戏
	if (MHIsDeskMatchBegin() && !m_isAllowDynamicIn)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error!!!  This Room Is Not Allow Dynamic In... deskId=[%d], userId=[%d]",
			this->GetDeskId(), user->GetUserDataId());

		ret.m_errorCode = 11;  //不允许动态加入
		user->Send(ret);
		return;
	}

	//GPS信息检查  开启GPS检测 && 不允许观战 && 不允许会长明牌观战
	if (this->m_Gps_Limit == 1)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Run... Check User GPS Data... deskId=[%d], userId=[%d], GPS=[%s], GPSValue=[%f,%f]", 
			this->GetDeskId(), user->GetUserDataId(), user->GetUserData().m_customString2.c_str(), user->m_gps_lng, user->m_gps_lat);

		if (user->m_gps_lng == 0.0f || user->m_gps_lat == 0.0f) //GPS数据无效
		{
			LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error!!!  GPS Data Is Invalid... deskId=[%d], userId=[%d], GPS=[%s], GPSValue=[%f:%f]", 
				this->GetDeskId(), user->GetUserDataId(), user->GetUserData().m_customString2.c_str(), user->m_gps_lng, user->m_gps_lat);

			ret.m_errorCode = 9;  //GPS数据无效
			user->Send(ret);
			return;
		}

		// 入座检查GPS距离
		Lstring user_gps_list;
		if (!this->MHCheckUserGPSLimit(user, user_gps_list))
		{
			LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error!!! User GPS Near, So Can Not Set Down... deskId=[%d], userId=[%d], userList=[%s]",
				this->GetDeskId(), user->GetUserDataId(), user_gps_list.c_str());

			ret.m_errorCode = 10;  //位置离得太近
			ret.m_userGPSList = user_gps_list;
			user->Send(ret);
			return;
		}
	}

	//金币场俱乐部 入座需要判断玩家金币数量是否达到要求
	if (m_isCoinsGame == 1 && m_clubInfo.m_clubId != 0 && (user->m_userData.m_coins < this->m_inDeskMinCoins || user->m_userData.m_coins <= 0))
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error!!!  This Player Coins Not Enough... deskId=[%d], userId=[%d], coins=[%d], deskMinCoins=[%d]",
			this->GetDeskId(), user->GetUserDataId(), user->m_userData.m_coins, this->m_inDeskMinCoins);
		
		ret.m_errorCode = LMsgS2CLookOnUserSeatDownRet::Err_Coins_Not_Enough;  //能量值不够：12
		user->Send(ret);
		return;
	}

	//最后一局不允许玩家加入
	if (m_vip && m_vip->m_curCircle == m_vip->m_maxCircle - 1)
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() Error!!!  Last Circle Not Allow Seat... deskId=[%d], userId=[%d], curCircle=[%d]",
			this->GetDeskId(), user->GetUserDataId(), m_vip->m_curCircle);

		ret.m_errorCode = LMsgS2CLookOnUserSeatDownRet::Err_Last_Circle_Not_Seat;  //不允许入座
		user->Send(ret);

		//62065：通知观战玩家，游戏开始最后一局，不允许观战玩家入座
		LMsgS2CLookOnPlayerFill fill;
		fill.m_errorCode = 4;   //游戏开始最后一局，让观战玩家隐藏入座按钮
		fill.m_state = this->m_state;

		//62065：广播给所有观战玩家
		MHBoadCastDeskLookonUser(fill);
		return;
	}
	
	//获取有效的空座位
	Lint pos = GetFreePos();

	//入座不能成功的
	if(pos == INVAILD_POS_QIPAI || 
		(CheXuan == m_state && m_playtype.size() > 5 && m_playtype[4] == 0 && MHIsDeskMatchBegin()))
	{
		LLOG_DEBUG("Desk::OnUserInRoomLookOnSeatDown() Look User Find Free Seat Failure... deskId=[%d], userId=[%d]",
			m_id, user->GetUserDataId());

		ret.m_errorCode = 1;
		ret.m_deskId = this->GetDeskId();

		//62076：通知玩家入座失败
		user->Send(ret);

		//62065：通知观战玩家，该桌子已满
		LMsgS2CLookOnPlayerFill fill;
		fill.m_errorCode = 1;
		fill.m_state = this->m_state;

		//广播给所有观战玩家
		MHBoadCastDeskLookonUser(fill);

		return;
	}

	//点击入座位置玩家找到空座位，入座成功
	else
	{
		//将该玩家添加到m_user中，成为入座玩家
		m_user[pos] = user;
		m_readyState[pos] = 0;

		//记录入座玩家列表
		m_user_select_seat_turn.push_back(user);

		//更新能量值
		if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
		{
			//m_vip->m_coins[pos] = user->m_userData.m_coins;
			SetUserCoins(user);
		}

		ret.m_pos = pos;

		//如果俱乐部会长入座，则不再是明牌观战玩家
		if (m_clubInfo.m_clubId != 0 && this->m_clubOwerLookOn == 1 && this->m_creatUserId == user->GetUserDataId())
		{
			this->m_clubOwerLookOnUser = NULL;
		}
		else
		{
			//将该玩家从观战表中去除
			MHEraseDeskLookonUserByPuser(user);
		}

		ret.m_deskId = this->GetDeskId();

		//62076：通知玩家入座成功
		user->Send(ret);

		if (m_deskState == DESK_PLAY)
		{
			m_playStatus[pos] = 2;
		}

		//更新客户端观战玩家数量
		UpdateLookOnUserCount();

		LLOG_DEBUG("Desk::OnUserInRoomLookOnSeatDown() Look User Has Find Free Seat And Seat Down Sucess... deskId=[%d], userId=[%d]",
			m_id, user->GetUserDataId());
	}
	
	/*if (MHIsDeskMatchBegin())
	{
		LLOG_DEBUG("LMsgS2CJoinRoomCheck");
		LMsgS2CJoinRoomCheck send;
		send.m_errorCode = 0;
		fillJoinRoomCheck(send, user->GetUserDataId());
		user->Send(send);
	}*/

	//61022：服务同步进入玩家的信息给该玩家
	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	//俱乐部
	if (m_clubInfo.m_clubId != 0 && m_clubInfo.m_clubDeskId != 0 && m_clubInfo.m_showDeskId != 0)
	{
		send1.m_deskId = m_clubInfo.m_showDeskId;
		send1.m_clubName = m_clubInfo.m_clubName;
	}
	send1.m_pos = pos;
	send1.m_ready = 0;
	send1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
	{
		send1.m_score = GetUserCoins(user);//m_vip->m_coins[pos];
	}
	send1.m_curBoBoScore = 0;
	send1.m_coins = GetUserCoins(user);//m_vip->m_coins[pos];
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
	send1.m_Greater2CanStart = m_Greater2CanStart;
	send1.m_StartGameButtonPos = m_StartGameButtonPos;
	if (send1.m_StartGameButtonPos > INVAILD_POS_MANAGER_START && send1.m_StartGameButtonPos < MAX_CHAIR_COUNT && m_user[send1.m_StartGameButtonPos] != NULL)
	{
		send1.m_nike = m_user[send1.m_StartGameButtonPos]->m_userData.m_nike;
	}
	send1.m_gamePlayerCount = m_desk_user_count;
	send1.m_startButtonAppear = m_startButtonAppear;
	send1.m_clubId = m_clubInfo.m_clubId;
	send1.m_playTypeId = m_clubInfo.m_playTypeId;
	send1.m_clubDeskId = m_clubInfo.m_clubDeskId;
	send1.m_GpsLimit=this->m_Gps_Limit;
	send1.m_dynamicJoin = (m_playStatus[pos] == 2 ? 1 : 0);
	send1.m_allowLookOn = m_allowLookOn;
	send1.m_isClubOwerLooking = this->m_clubOwerLookOnUser ? 1 : 0;
	send1.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
	send1.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	send1.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
	send1.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
	send1.m_warnScore = this->m_warnScore;
	send1.m_myRole = GetClubDeskUserRole(user);
	send1.m_isMedal = user->m_isMedal;
	GetDeskSeatStatus(send1.m_deskUserStatus);
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame && m_vip->IsBegin())
	{
		memcpy(send1.m_deskUserStatus, m_handlerPlayerStatus, sizeof(Lint) * 10);
	}
	user->Send(send1);

	//61023：玩家状态，服务器推送该入座玩家信息给所有房间内的玩家
	LMsgS2CDeskAddUser addmsg1;
	addmsg1.m_userId = user->GetUserDataId();
	addmsg1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
	{
		addmsg1.m_score = GetUserCoins(user);//m_vip->m_coins[pos];
	}
	addmsg1.m_curBoBoScore = 0;
	addmsg1.m_pos = pos;
	LLOG_DEBUG("***用户加入房间的位置 msgid = %d userid = %d pos = %d", addmsg1.m_msgId, addmsg1.m_userId, addmsg1.m_pos);
	addmsg1.m_nike = user->m_userData.m_nike;
	addmsg1.m_ready = m_readyState[pos];
	addmsg1.m_sex = user->m_userData.m_sex;
	addmsg1.m_ip = user->m_userData.m_customString1;
	addmsg1.m_face = user->m_userData.m_headImageUrl;
	addmsg1.m_online = user->GetOnline();
	addmsg1.m_coins = GetUserCoins(user); //m_vip->m_coins[pos];
	addmsg1.m_credits = user->m_userData.m_creditValue;
	addmsg1.m_userGps = user->m_userData.m_customString2;
	addmsg1.m_cheatAgainst = m_cheatAgainst;
	addmsg1.m_videoPermission = user->m_videoPermission;
	addmsg1.m_dynamicJoin = (m_playStatus[pos] == 2 ? 1 : 0);
	addmsg1.m_allowLookOn = m_allowLookOn;
	addmsg1.m_clubOwerLookOn = m_clubOwerLookOn;
	addmsg1.m_isClubOwerLooking = this->m_clubOwerLookOnUser ? 1 : 0;
	addmsg1.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
	addmsg1.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	addmsg1.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
	addmsg1.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
	addmsg1.m_isMedal = user->m_isMedal;

	//通知桌子上已入座的其他玩家
	MHBoadCastDeskUserWithoutUser(addmsg1, user);

	// 通知观战玩家，user刚入坐
	MHBoadCastDeskLookonUser(addmsg1);

	//通知会长明牌观战，user刚入座
	MHBoadCastClubOwerLookOnUser(addmsg1);

	//发送manager,更新俱乐部桌子上玩家信息
	if (m_clubInfo.m_clubId != 0 &&m_clubInfo.m_clubDeskId!=0)
	{
		LLOG_DEBUG("Desk::HanderSelectSeat LMsgL2LMGUserAddClubDesk");
		LMsgL2LMGUserAddClubDesk  send;
		send.m_clubId = m_clubInfo.m_clubId;
		send.m_playTypeId = m_clubInfo.m_playTypeId;
		send.m_clubDeskId = m_clubInfo.m_clubDeskId;
		send.m_strUUID = user->m_userData.m_unioid;
		send.m_userInfo.m_userId = user->GetUserDataId();
		send.m_userInfo.m_deskPlayerName = user->m_userData.m_nike;
		send.m_userInfo.m_deskPlayerUrl = user->m_userData.m_headImageUrl;
		send.m_userInfo.m_pos = pos;
		if (this->m_clubOwerLookOn == 1 && this->m_clubOwerLookOnUser)
		{
			send.m_isClubOwerLooking = 1;
		}

		gWork.SendToLogicManager(send);
	}

	//如果人满,发送manager,更新俱乐部界面
	if (m_clubInfo.m_clubId != 0 && GetUserCount() == m_desk_user_count)
	{
		MHNotifyManagerDeskInfo(1);
	}

	if (m_deskState == DESK_WAIT)
	{
		if (m_vip)
		{
			m_vip->SendInfo();
		}
	}

	//如果是最后一个人坐下
	if (this->GetUserCount() == this->GetPlayerCapacity())
	{
		LLOG_ERROR("Desk::OnUserInRoomLookOnSeatDown() The last user seat down, So desk if full now... deskId=[%d], userId=[%d]",
			m_id, user->GetUserDataId());

		//62065：通知观战玩家，该桌子已满
		LMsgS2CLookOnPlayerFill fill;
		fill.m_errorCode = 1;
		fill.m_state = this->m_state;

		//广播给所有观战玩家
		MHBoadCastDeskLookonUser(fill);
	}

	/*
	if (m_Greater2CanStart == 1 && m_deskState == DESK_WAIT && m_StartGameButtonPos != INVAILD_POS_QIPAI && m_vip && m_vip->m_curCircle == 0 && m_readyState[pos] != 1)
	{
		m_readyState[pos] = 1;
		LMsgS2CUserReady ready;
		ready.m_pos = pos;
		BoadCast(ready);

		bool command_turn = true;

		Lint readyUserCount = 0;
		for (Lint i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_readyState[i] == 1)readyUserCount++;
		}

		//新版牛牛达到指定人数后开始游戏
		if (m_state == NiuNiu_New && m_playtype.size() > 5 && m_playtype[4] > 1)
		{
			if (readyUserCount >= m_playtype[4])
			{
				command_turn = false;
				MHHanderStartGame();
			}
		}
		//三张牌人满自动开始游戏
		else if (m_state == YingSanZhang)
		{
			if (readyUserCount == m_iPlayerCapacity)
			{
				if (m_auto_start_game_time_flag == 0)
				{
					m_auto_start_game_time.Now();
					m_auto_start_game_time_flag = 1;
				}
			}
		}
		//其他游戏人满自动开局
		else if(NiuNiu == m_state || CheXuan == m_state)
		{
			if (readyUserCount == m_iPlayerCapacity)
			{
				command_turn = false;
				MHHanderStartGame();
			}
		}

		if (command_turn)
		{
			LMsgS2CUserDeskCommand command;
			command.m_commandType = 1;
			command.m_StartButtonPos = m_StartGameButtonPos;
			if (command.m_StartButtonPos < MAX_CHAIR_COUNT  &&m_user[command.m_StartButtonPos])
				command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
			BoadCast(command);
		}
	}
	*/
	if (m_deskState == DESK_PLAY && mGameHandler && m_Greater2CanStart == 1)
	{
		mGameHandler->OnUserReconnect(user);
	}

	//管理员开始游戏 && 游戏未开始
	/*
	if (m_isManagerStart && !MHIsDeskMatchBegin())
	{
		Lint t_managerIdCount = 0;
		std::vector<Lint> t_managerId;
		t_managerIdCount = MHGetManagerList(t_managerId);
		int nRet = MHCheckGameStart();

		LMsgS2CUserDeskCommand command;
		command.m_StartButtonPos = INVAILD_POS_QIPAI;
		command.m_StartButtonUserId = t_managerId;
		command.m_StartButtonUserIdCount = t_managerIdCount;
		command.m_StartButtonEnable = 0;
		command.m_Nike = "等待管理员开始游戏...";

		if (m_Greater2CanStart == 1 && nRet == 1)
		{
			command.m_StartButtonEnable = 1;

			if (!t_managerId.empty())
			{
				command.m_commandType = 2;
			}
			else
			{
				command.m_commandType = 3;
			}
		}
		MHBoadCastManagerAndSeatUser(command, nRet, t_managerId);
	}*/
	return;
}

//玩家正常加入房间
void Desk::OnUserInRoom(User* user)
{
	if (user == NULL)
	{
		LLOG_ERROR("Desk::OnUserInRoom() Error!!! user is NULL deskId=[%d]", m_id);
		return;
	}

	if (GetUserPos(user) != INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::OnUserInRoom is in desk:%d already, user:%d", m_id, user->GetUserDataId());
		return;
	}

	if (!m_vip)
	{
		LLOG_ERROR("Desk::OnUserInRoom desk vip item is empty user:%d desk:%d", user->GetUserDataId(), m_id);
		return;
	}

	LLOG_ERROR("Desk::OnUserInRoom() Run... deskId=[%d], userId=[%d]", m_id, user->GetUserDataId());

	//关于几个变量说明 by wyz： m_user  , m_seatUser，m_desk_Lookon_user     用户进入房间只写入m_desk_all_user，m_desk_Lookon_user 这两个变量；用户选座成功以后用户写入m_seatUser（东南西北实际位置），从m_desk_Lookon_user移除；游戏开始时，写入m_user（位置与实际位置无关，按东南西北依次排）,m_switchPos位置记录转换关系  by wyz

	Lint pos = GetFreePos();
	if(pos == INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::OnUserInRoom INVAILD_POS_QIPAI desk:%d user:%d",m_id, user->GetUserDataId());
		return;
	}	
	
	user->SetDesk(this);
	user->updateInRoomTime();
	m_user[pos] = user;
	m_readyState[pos] = 0;

	//更新能量值
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
	{
		//m_vip->m_coins[pos] = user->m_userData.m_coins;
		SetUserCoins(user);
	}

	//m_timeInRoom[pos] = LTime().Secs(); // 更新进入时间	
	
	m_playStatus[pos]=0;          // 设置游戏状态
	if(m_deskState==DESK_PLAY)
		m_playStatus[pos] = 2;          // 设置游戏状态
	


	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	//三张牌下一局开始倒计时时间
	if (m_state == YingSanZhang)
	{
		send1.m_nextDrawLimitTime = YINGSANZHANG_READY_DELAY_TIME;
		if (m_next_draw_lookon)
			send1.m_nextDrawLimitTime = YINGSANZHANG_GAME_START_DELAY_TIME;
	}
	//俱乐部
	if (m_clubInfo.m_clubId != 0 && m_clubInfo.m_clubDeskId != 0 && m_clubInfo.m_showDeskId != 0)
	{
		send1.m_deskId = m_clubInfo.m_showDeskId;
	}
	send1.m_pos = pos;	
	send1.m_ready = 0;
	send1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
	{
		send1.m_score = GetUserCoins(user);//m_vip->m_coins[pos];
	}
	send1.m_curBoBoScore = 0;
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
	send1.m_Greater2CanStart = m_Greater2CanStart;
	send1.m_StartGameButtonPos = m_StartGameButtonPos;
	if(send1.m_StartGameButtonPos > INVAILD_POS_MANAGER_START && send1.m_StartGameButtonPos < MAX_CHAIR_COUNT && m_user[send1.m_StartGameButtonPos] != NULL)
	{
		send1.m_nike = m_user[send1.m_StartGameButtonPos]->m_userData.m_nike;
	}
	send1.m_gamePlayerCount = m_desk_user_count;
	send1.m_startButtonAppear=m_startButtonAppear;	 
	send1.m_clubId=m_clubInfo.m_clubId;
	send1.m_playTypeId=m_clubInfo.m_playTypeId;
	send1.m_clubDeskId = m_clubInfo.m_clubDeskId;
	send1.m_GpsLimit=this->m_Gps_Limit;
	send1.m_dynamicJoin = (m_playStatus[pos] == 2 ? 1 : 0);
	send1.m_allowLookOn = m_allowLookOn;
	send1.m_isClubOwerLooking = this->m_clubOwerLookOnUser ? 1 : 0;
	send1.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
	send1.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	send1.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
	send1.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
	send1.m_warnScore = this->m_warnScore;
	send1.m_myRole = GetClubDeskUserRole(user);
	send1.m_isMedal = user->m_isMedal;
	user->Send(send1);

 	LMsgS2CDeskAddUser addmsg1;
 	addmsg1.m_userId = user->GetUserDataId();
 	addmsg1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
	{
		addmsg1.m_score = GetUserCoins(user);//m_vip->m_coins[pos];
	}
	addmsg1.m_curBoBoScore = 0;
	addmsg1.m_pos = pos; 	
	LLOG_DEBUG("***用户加入房间的位置 msgid = %d userid = %d pos = %d", addmsg1.m_msgId, addmsg1.m_userId, addmsg1.m_pos);
 	addmsg1.m_nike = user->m_userData.m_nike;
 	addmsg1.m_ready = m_readyState[pos];
 	addmsg1.m_sex = user->m_userData.m_sex;
 	addmsg1.m_ip = user->m_userData.m_customString1;
 	addmsg1.m_face = user->m_userData.m_headImageUrl;
	addmsg1.m_online = user->GetOnline();
	addmsg1.m_coins = GetUserCoins(user);
	addmsg1.m_credits = user->m_userData.m_creditValue;
	addmsg1.m_userGps = user->m_userData.m_customString2;
	addmsg1.m_cheatAgainst = m_cheatAgainst;
	addmsg1.m_videoPermission = user->m_videoPermission;
	addmsg1.m_dynamicJoin=(m_playStatus[pos]==2?1:0);
	addmsg1.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
	addmsg1.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	addmsg1.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
	addmsg1.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
	addmsg1.m_isMedal = user->m_isMedal;

	// 通知房间玩家
 	for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != user)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId();			
 			addmsg2.m_pos = i;			
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_score = m_vip ? m_vip->GetUserScore(m_user[i]) : 0;
			if (m_clubInfo.m_clubId != 0 && m_isCoinsGame)
			{
				addmsg2.m_score = GetUserCoins(m_user[i]);//m_vip->m_coins[i];
			}
			addmsg2.m_curBoBoScore = 0;
			addmsg2.m_online = m_user[i]->GetOnline();
			addmsg2.m_coins = GetUserCoins(m_user[i]);//m_vip->m_coins[i];
			addmsg2.m_credits = m_user[i]->m_userData.m_creditValue;
			addmsg2.m_userGps = m_user[i]->m_userData.m_customString2;
			addmsg2.m_cheatAgainst = m_cheatAgainst;
			addmsg2.m_videoPermission = m_user[i]->m_videoPermission;
			addmsg2.m_dynamicJoin=(m_playStatus[i]==2?1:0);
			addmsg2.m_isCoinsGame = this->m_isCoinsGame;						//是否为金币场  0：普通场  1：金币场
			addmsg2.m_inDeskMinCoins = this->m_inDeskMinCoins;				// 金币场入座所需的最小能量值
			addmsg2.m_qiangZhuangMinCoins = this->m_qiangZhuangMinCoins;		// 金币场参与抢庄的最小能量值
			addmsg2.m_isAllowCoinsNegative = this->m_isAllowCoinsNegative;	// 每局结算时是否允许负数能量值出现
			addmsg2.m_isMedal = m_user[i]->m_isMedal;

 			user->Send(addmsg2);
			m_user[i]->Send(addmsg2);
 		}
 	}

	//发送Lookon用户的
	/*
	LLOG_INFO("Logwyz-------------m_desk_Lookon_user=[%d]", m_desk_Lookon_user.size());
	std::list<User*>::iterator userIt;
	for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); ++userIt)
	{
		User* temp=*userIt;
		if (temp==NULL)continue;
		if (temp!=user)
		{
			LMsgS2CDeskAddUser addmsg2;
			addmsg2.m_userId=temp->GetUserDataId();
			addmsg2.m_pos=INVAILD_POS_QIPAI;
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
	***/
	//CheckGameStart();

	//发送manager,
	if (m_clubInfo.m_clubId!=0)
	{
		LLOG_DEBUG("Desk::HanderSelectSeat LMsgL2LMGUserAddClubDesk");
		LMsgL2LMGUserAddClubDesk  send;
		send.m_clubId=m_clubInfo.m_clubId;
		send.m_playTypeId=m_clubInfo.m_playTypeId;
		send.m_clubDeskId=m_clubInfo.m_clubDeskId;
		send.m_userInfo.m_userId=user->GetUserDataId();
		send.m_strUUID = user->m_userData.m_unioid;
		send.m_userInfo.m_deskPlayerName = user->m_userData.m_nike;
		send.m_userInfo.m_deskPlayerUrl = user->m_userData.m_headImageUrl;
		send.m_userInfo.m_pos=pos;
		send.m_logicId = gConfig.GetServerID();

		gWork.SendToLogicManager(send);
	}

	//如果人满,发送manager,更新俱乐部界面
	if (m_clubInfo.m_clubId!=0 && GetUserCount() == m_desk_user_count)
	{
		MHNotifyManagerDeskInfo(1);
	}

	if (m_deskState == DESK_WAIT)
	{
		if (m_vip)
		{
			m_vip->SendInfo();
		}

		//俱乐部匹配进入房间
		if ((m_state == DouDiZhu && m_playtype.size()>=6 && m_playtype[4] == 1 && m_playtype[5]!=0) ||
			(m_state == ShuangSheng && m_playtype.size() >= 10 && m_playtype[2] == 1 && m_playtype[9] != 0) ||
			(m_state == SanDaEr && m_playtype.size() >= 10 && m_playtype[9] != 0) ||
			(m_state == SanDaYi && m_playtype.size() >= 10 && m_playtype[9] != 0) ||
			(m_state == WuRenBaiFen && m_playtype.size() >= 10 && m_playtype[9] != 0))
		{
			m_readyState[pos] = 1;
			LMsgS2CUserReady ready;
			ready.m_readyUserCount = MHGetUserReadyCount() + 1;
			ready.m_pos = pos;
			BoadCast(ready);
			if (m_next_draw_lookon &&MHGetUserReadyCount() >= 2)
				record_sanzhangpai_game_start();

			MHCheckGameStart();
		}
	}
	else if (m_deskState == DESK_PLAY && mGameHandler && m_Greater2CanStart == 1)
	{
		mGameHandler->OnUserReconnect(user);
	}


	//if (m_Greater2CanStart == 0 && m_deskState == DESK_WAIT &&  m_vip && m_vip->m_curCircle == 0 && m_readyState[pos] == 0)
	//{
	//	//LLOG_ERROR("****Doudizhu set user ready for first game... desk:%d user:%d, pos:%d", m_id, user->GetUserDataId(), pos);
	//	//m_readyState[pos] = 1;
	//	//LMsgS2CUserReady ready;
	//	//ready.m_pos = pos;
	//	//BoadCast(ready);
	//
	//	bool command_turn = true;
	//	//人满开始游戏
	//	if (m_state == YingSanZhang || m_state == NiuNiu || m_state == DouDiZhu)
	//	{
	//		Lint readyUserCount = 0;
	//		for (Lint i = 0; i < m_iPlayerCapacity; i++)
	//		{
	//			if (m_readyState[i] == 1)readyUserCount++;
	//		}
	//		if (readyUserCount == m_iPlayerCapacity)
	//		{
	//			command_turn = false;
	//			MHHanderStartGame();
	//		}
	//	}
	//}

	
	if (m_Greater2CanStart == 1 && m_deskState == DESK_WAIT && m_StartGameButtonPos != INVAILD_POS_QIPAI && m_vip && m_vip->m_curCircle == 0 && m_readyState[pos] != 1)
	{
		m_readyState[pos]=1;
		LMsgS2CUserReady ready;
		ready.m_readyUserCount = MHGetUserReadyCount() + 1;
		ready.m_pos=pos;
		BoadCast(ready);
		if (m_next_draw_lookon &&MHGetUserReadyCount() >= 2)
			record_sanzhangpai_game_start();

		bool command_turn = true;
		//人满开始游戏
		if (m_state == YingSanZhang || 
			m_state == NiuNiu || 
			m_state == NiuNiu_New ||
			CheXuan == m_state)
		{
			Lint readyUserCount = 0;
			for (Lint i = 0; i < m_iPlayerCapacity; i++)
			{
				if (m_readyState[i] == 1)readyUserCount++;
			}
			if (readyUserCount == m_iPlayerCapacity)
			{
				//command_turn = false;
				//MHHanderStartGame();
				if (m_auto_start_game_time_flag == 0)
				{
					m_auto_start_game_time.Now();
					m_auto_start_game_time_flag = 1;
				}
			}
		}

		if (command_turn)
		{
			LMsgS2CUserDeskCommand command;
			command.m_commandType = 1;
			command.m_StartButtonPos = m_StartGameButtonPos;
			if (command.m_StartButtonPos < MAX_CHAIR_COUNT  &&m_user[command.m_StartButtonPos])
				command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
			BoadCast(command);
		}

	}

	/*if (m_deskState==DESK_PLAY&&mGameHandler)
	{
		mGameHandler->OnUserReconnect(user);
	}*/

}

//多个玩家正常加入房间
void Desk::OnUserInRoom(User* user[])
{

	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		Lint pos = GetFreePos();
		if(pos == INVAILD_POS_QIPAI)
		{
			LLOG_ERROR("Desk::OnUserInRoom INVAILD_POS_QIPAI %d",user[0]->GetUserDataId());
			return;
		}

		m_user[pos] = user[i];
		user[i]->SetDesk(this);
		m_readyState[pos] = 1;
	}

	CheckGameStart();
}

/*
  处理玩家离开桌子，与桌子关联删除
  outType离开类型 0：默认类型，兼容以前，1：房间解散，2：玩家离开房间，3：被踢出房间
*/
void Desk::OnUserOutRoom(User* user, Lint outType)
{
	if (!user)
	{
		return;
	}

	LLOG_ERROR("Desk::OnUserOutRoom User Will Out Room...  userId=[%d], deskId=[%d] ", user->GetUserDataId(), m_id);

 	Lint pos = GetUserPos(user);

	//该玩家既没有入座，也不是观望玩家  也不是会长
	if (pos == INVAILD_POS_QIPAI && !this->MHIsLookonUser(user) && this->m_clubOwerLookOnUser != user)
	{
		LLOG_ERROR("Desk::OnUserOutRoom Error... User is not on this desk... deskId=[%d], userId=[%d]", m_id, user->GetUserDataId());
		return;
	}
	//如果该玩家是已入座玩家
	if (pos != INVAILD_POS_QIPAI)
	{
		LLOG_DEBUG("Desk::OnUserOutRoom Play User Out Room Sucess...  userId=[%d], deskId=[%d] ", user->GetUserDataId(), m_id);

		//从seatList列表中删除
		MHEraseDeskSeatListUserByPuser(user);

		//61024:服务器返回玩家删除该玩家
		LMsgS2CDeskDelUser del;
		if (m_dismissUser)
		{
			del.m_DismissName = m_creatUserNike;
		}
		del.m_userId = user->m_userData.m_id;
		del.m_pos = pos;
		del.m_StartGameButtonPos = INVAILD_POS_QIPAI;
		del.m_outType = outType;
		//重新选择第一个选座的人
		if (m_firestUser == user)
		{
			m_firestUser = NULL;
		}
		MHBoadCastAllDeskUser(del);

		//广播给观望玩家
		if (!this->m_desk_Lookon_user.empty())
		{
			MHBoadCastDeskLookonUser(del);
		}
		m_user[pos] = NULL;
		m_readyState[pos] = 0;
		m_playStatus[pos] = 0;     // 设置游戏状态
		m_vip->m_coins[pos] = 0;
	}
	//该玩家是LookOn玩家 或者 是会长观战离开
	else if (this->MHIsLookonUser(user) || user == this->m_clubOwerLookOnUser)
	{
		LLOG_DEBUG("Desk::OnUserOutRoom LookOn User Out Room Sucess...  userId=[%d], deskId=[%d], m_iPlayerCapacity=[%d]", user->GetUserDataId(), m_id, m_iPlayerCapacity);

		//61024:服务器返回玩家删除该玩家
		LMsgS2CDeskDelUser lookon;
		if (m_dismissUser)
		{
			lookon.m_DismissName = m_creatUserNike;
		}
		lookon.m_userId = user->m_userData.m_id;
		lookon.m_pos = m_iPlayerCapacity;
		lookon.m_StartGameButtonPos = INVAILD_POS_QIPAI;
		lookon.m_outType = outType;
		user->Send(lookon);

		if (user == this->m_clubOwerLookOnUser)
		{
			this->m_clubOwerLookOnUser = NULL;
			//61024:会长名牌观战离开房间，通知再座玩家名牌观战的会长已经离开
			lookon.m_clubOwerLookingLeave = 1;
			MHBoadCastAllDeskUser(lookon);

			//更新俱乐部大厅，会长明牌观战离开
			LMsgL2LMGUserLeaveClubDesk  send;
			send.m_clubId = m_clubInfo.m_clubId;
			send.m_playTypeId = m_clubInfo.m_playTypeId;
			send.m_clubDeskId = m_clubInfo.m_clubDeskId;
			send.m_userId = user->GetUserDataId();
			send.m_strUUID = user->m_userData.m_unioid;
			send.m_isClubOwerLooking = 0;

			gWork.SendToLogicManager(send);
		}
		else
		{
			//从该桌子的观战玩家列表中删除该观战玩家
			this->MHEraseDeskLookonUserByPuser(user);
		}

		//修改观战玩家状态为在大厅，并同步该玩家状态到Gate 和 Manager
		user->setUserState(m_clubInfo.m_clubId != 0 ? LGU_STATE_CLUB : LGU_STATE_CENTER);
		user->ModifyUserState(true);

		//清除观战玩家与该桌子关联
		user->SetDesk(NULL);

		//更新客户端观战玩家数量
		UpdateLookOnUserCount();

		//如果是最后一个人离开房间，则解散桌子
		if (outType != 1 && m_clubInfo.m_clubId != 0 && MHGetDeskUserCount() == 0)
		{
			ResetEnd();
		}
		return;
	}
	
	//修改状态
	user->setUserState(m_clubInfo.m_clubId != 0 ? LGU_STATE_CLUB : LGU_STATE_CENTER);
	user->ModifyUserState(true);

	//清除状态
	user->SetDesk(NULL);

	if (outType != 1 && !MHIsDeskMatchBegin())
	{
		int nRet = MHCheckGameStart();
		
		//房主管理员开始游戏
		if (m_isManagerStart)
		{
			Lint t_managerIdCount = 0;
			std::vector<Lint> t_managerId;
			t_managerIdCount = MHGetManagerList(t_managerId);

			LMsgS2CUserDeskCommand command;
			command.m_StartButtonPos = INVAILD_POS_QIPAI;
			command.m_StartButtonUserId = t_managerId;
			command.m_StartButtonUserIdCount = t_managerIdCount;
			command.m_StartButtonEnable = 0;
			command.m_Nike = "Plase Wait Manager Start Game...";

			if (m_Greater2CanStart == 1 && nRet == 1)
			{
				command.m_StartButtonEnable = 1;

				if (!t_managerId.empty())
				{
					command.m_commandType = 2;
				}
				else
				{
					command.m_commandType = 3;
				}
			}
			MHBoadCastManagerAndSeatUser(command, nRet, t_managerId);
		}
		//第一个入座玩家开始游戏
		else if(!m_isManagerStart)
		{
			LMsgS2CUserDeskCommand command;
			command.m_commandType = 1;
			if (m_Greater2CanStart == 1 && nRet == 1)
			{
				m_StartGameButtonPos = MHFirstSeatPos();
				command.m_StartButtonPos = m_StartGameButtonPos;
				if (command.m_StartButtonPos < MAX_CHAIR_COUNT && m_user[command.m_StartButtonPos])
				{
					command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
				}
			}
			else
			{
				m_StartGameButtonPos = INVAILD_POS_QIPAI;
				command.m_StartButtonPos = m_StartGameButtonPos;
			}
			BoadCast(command);
		}

		/*
		//61200：服务器返回添加开始按钮的位置
		LMsgS2CUserDeskCommand command;
		command.m_commandType = 1;
		if (m_Greater2CanStart == 1 && m_StartGameButtonPos != INVAILD_POS_QIPAI && m_vip && !m_vip->IsBegin() && !(m_resetTime > 0 && m_resetUserId > 0))
		{
		command.m_StartButtonPos = INVAILD_POS_QIPAI;
		if (pos == m_StartGameButtonPos && GetUserCount() >= 2)
		{
		int old_pos = m_Greater2CanStart;
		command.m_StartButtonPos = m_StartGameButtonPos = MHSpecPersonPos();
		LLOG_DEBUG("Refresh game start button, Desk:%d old pos:%d, m_StartGameButtonPos:%d", m_id, old_pos, m_StartGameButtonPos);
		if (command.m_StartButtonPos < MAX_CHAIR_COUNT && m_user[command.m_StartButtonPos])
		command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
		BoadCast(command);
		}
		else if (GetUserCount() < 2)
		{
		command.m_StartButtonPos = m_StartGameButtonPos = INVAILD_POS_QIPAI;
		LLOG_DEBUG("Hide game start button Desk:%d, m_StartGameButtonPos:%d", m_id, m_StartGameButtonPos);
		if (command.m_StartButtonPos < MAX_CHAIR_COUNT && m_user[command.m_StartButtonPos])
		command.m_Nike = m_user[command.m_StartButtonPos]->m_userData.m_nike;
		BoadCast(command);
		}
		}
		else if (m_Greater2CanStart == 1 && m_StartGameButtonPos == INVAILD_POS_QIPAI && m_vip && !m_vip->IsBegin() && !(m_resetTime > 0 && m_resetUserId > 0))
		{
		Lint readyUserCount = 0;
		for (Lint i = 0; i < m_iPlayerCapacity; i++)
		{
		if (m_user[i] && m_readyState[i] == 1)
		readyUserCount++;
		}
		if (readyUserCount >= 2)
		{
		command.m_StartButtonPos = m_StartGameButtonPos = MHSpecPersonPos();
		LLOG_DEBUG(" game start button, Desk:%d , m_StartGameButtonPos:%d", m_id, m_StartGameButtonPos);
		BoadCast(command);
		}
		}
		*/
	}

	//发送manager，更新俱乐部
	if (m_clubInfo.m_clubId != 0 && outType != 1)
	{
		//LLOG_DEBUG("Desk::OnUserOutRoom LMsgL2LMGUserLeaveClubDesk");
		LMsgL2LMGUserLeaveClubDesk  send;
		send.m_clubId = m_clubInfo.m_clubId;
		send.m_playTypeId = m_clubInfo.m_playTypeId;
		send.m_clubDeskId = m_clubInfo.m_clubDeskId;
		send.m_userId = user->GetUserDataId();
		send.m_strUUID = user->m_userData.m_unioid;
		send.m_isClubOwerLooking = 0;
		gWork.SendToLogicManager(send);

		//如果人满,发送manager,更新俱乐部界面
		if (GetUserCount() == m_desk_user_count - 1)
		{
			MHNotifyManagerDeskInfo(0);
		}
	}
	
	//删除玩家 不删除机器人
	if (user->getUserGateID() != 0xffff)
	{
		UserManager::Instance().DelUser(user);
		delete user;
	}

	if (outType!=1 && m_clubInfo.m_clubId != 0 && MHGetDeskUserCount() == 0)
	{
		ResetEnd();
	}

}

Lint Desk::GetNextPos(Lint prev)
{
	if(prev < 0 || prev >= m_iPlayerCapacity)
	{
		return INVAILD_POS_QIPAI;
	}

	return prev == (m_iPlayerCapacity - 1) ? 0 : (prev + 1);
}

Lint Desk::GetPrevPos(Lint next)
{
	if(next < 0 || next >= m_iPlayerCapacity)
	{
		return INVAILD_POS_QIPAI;
	}

	return next == 0 ? (m_iPlayerCapacity - 1) : (next - 1);
}

/*
  获取玩家user位置
  玩家不是已经入座玩家，则返回21位置
*/
Lint Desk::GetUserPos(User* user)
{
	if(user == NULL)
	{
		return INVAILD_POS_QIPAI;
	}

	for(Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if(m_user[i] == user)
			return i;
	}

	return INVAILD_POS_QIPAI;
}


User* Desk::GetPosUser(Lint pos)
{
	if(pos<0 || pos >= INVAILD_POS_QIPAI)
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

Lint Desk::GetUserCoins(User* user)
{
	if (!user || !m_vip)
	{
		return 0;
	}
	Lint pos = GetUserPos(user);
	if (pos != INVAILD_POS_QIPAI)
	{
		return m_vip->m_coins[pos];
	}
	return 0;
}
bool Desk::SetUserCoins(User* user)
{
	if (!user || !m_vip)
	{
		return false;
	}
	Lint pos = GetUserPos(user);
	if (pos != INVAILD_POS_QIPAI)
	{
		m_vip->m_coins[pos] = user->m_userData.m_coins;
		return true;
	}
	return false;
}

Lint Desk::GetPlayUserCount()
{
	Lint cnt = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && m_playStatus[i] == 1)
		{
			cnt += 1;
		}
	}
	return cnt;
}

//获取desk上已经动态加入的玩家数量
Lint Desk::GetDynamicInCount()
{
	Lint cnt = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && m_playStatus[i] == 2)
		{
			++cnt;
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

	GetVip()->ResetUser(m_user);

	bool bFind = false;
	if (m_creatUser) {
		for (int i = 0; i < MAX_CHAIR_COUNT; i++)
		{
			if ( m_creatUser == m_user[i]) {
				LLOG_DEBUG("m_zhuangPos = %d", i);
				m_zhuangPos = i;
				bFind = true;
				break;
			}
		}
	}


	if (!bFind && m_firestUser) {
		for (int i = 0; i < MAX_CHAIR_COUNT; i++)
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
	LLOG_ERROR("********开始游戏**** 桌子ID: %d ", m_id);
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i]) {
			LLOG_ERROR("**** 用户ID %d IP = %s", m_user[i]->GetUserData().m_id, m_user[i]->m_userData.m_customString1.c_str());
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

void Desk::BoadCast2PlayingUser(LMsg& msg)
{
	LLOG_DEBUG("BoadCast2PlayingUser m_iPlayerCapacity = %d", m_iPlayerCapacity);
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i]&&m_playStatus[i]==1)
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

/*
返回user[]中所有玩家的昵称
*/
std::vector<std::string>  Desk::getAllPlayerName()
{
	std::vector<std::string> tempNameList;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i]) {
			std::string name = m_user[i]->GetUserData().m_nike;
			tempNameList.push_back(name);
		}
	}

	return tempNameList;
}

//清空已入座玩家
void Desk::ClearUser()
{
	LLOG_ERROR("clearUser: deskId=[%d]", this->GetDeskId());
	if( m_deskType == DeskType_Common )
	{
		//LLOG_ERROR("Desk::ClearUser() Begin clear common desk type m_user[]... deskId=[%d]", this->GetDeskId());
		//for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		//{
		//	if (m_user[i])		
		//		OnUserOutRoom(m_user[i], 1);
		//}
		//LLOG_ERROR("Desk::ClearUser() Begin clear common desk type m_user[]... deskId=[%d]", this->GetDeskId());
		LMsgL2LMGUserListLeaveClubDesk send;
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_user[i]) {
				send.m_userIdList.push_back(m_user[i]->GetUserDataId());
				send.m_strUUIDList.push_back(m_user[i]->m_userData.m_unioid);
				OnUserOutRoom(m_user[i], 1);
			}
		}
		send.m_userCount = send.m_userIdList.size();
		send.m_clubId = m_clubInfo.m_clubId;
		send.m_playTypeId = m_clubInfo.m_playTypeId;
		send.m_clubDeskId = m_clubInfo.m_clubDeskId;
		if (send.m_clubId != 0 && !send.m_userIdList.empty())
		{
			LLOG_DEBUG("SEND LMsgL2LMGUserListLeaveClubDesk count[%d] idsize[%d],uuidSize[%d]", send.m_userCount, send.m_userIdList.size(), send.m_strUUIDList.size());
			gWork.SendToLogicManager(send);
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
	m_resetUserId = 0;

	LLOG_DEBUG("Desk::ResetClear() Run... Desk reset be cancel, clear reset desk status... deskId=[%d]",
		this->GetDeskId());
}

//房间解散结束
void Desk::ResetEnd()
{
	LLOG_DEBUG("Logwyz ...............Desk::ResetEnd()");
	if(m_vip)
	{
		m_vip->m_reset = 1;
		if (m_vip->m_curCircle >= 1) // 局结束,下局未开始点
		{
			m_vip->InsertDeskWinnerInfo();			
			m_vip->UpdateToDb();
		}
	}
	
	//牌局中解散成功
	if (m_deskState == DESK_PLAY)
	{
		LLOG_DEBUG("Logwyz ...............Desk::ResetEnd(),m_deskState == DESK_PLAY");
		m_bIsDissmissed = true;
		if (mGameHandler)
		{
			mGameHandler->OnGameOver(WIN_NONE, INVAILD_POS_QIPAI, INVAILD_POS_QIPAI);
			//通知子游戏桌子重置
		}
	}
	else
	{
		if (m_deskCreatedType == 1) 
		{
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

			//是否要推送消息给LookOn玩家
		}

		//清空入座的玩家
		ClearUser();

		//清空lookon用户
		MHClearLookonUser(1);

		//清空会长观战
		MHClearClubOwerLookOn(1);

		SetVip(NULL);
		ResetClear();
		SetDeskFree();
	}
}

void Desk::SpecialCardClear()
{
	//memset(m_specialCard,0,sizeof(m_specialCard));
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
		LLOG_ERROR("Desk::HanderGameOver() Error!!! vip is NULL!!! deskId=[%d]", this->m_id);
		return;
	}

	//普通场
	if( m_deskType == DeskType_Common )
	{
		LLOG_DEBUG("Desk::HanderGameOver   m_deskType == DeskType_Common");

		//记录一局结束时间
		record_round_end_time();

		//检测扣房卡
		if (m_vip && m_vip->m_curCircle == 1 && !m_vip->m_reset)
		{
			Lint cardType = CARD_TYPE_4;
			//if (m_vip->m_maxCircle == 8)
			//	cardType = CARD_TYPE_8;
			//else if (m_vip->m_maxCircle == 16)
			//	cardType = CARD_TYPE_16;
			cardType = getDrawType(m_state, m_vip->m_maxCircle);

			LLOG_ERROR("**************每局结束桌子ID: %d ", m_id);
			for (int i = 0; i < m_iPlayerCapacity; i++)
			{
				if (m_user[i]) {
					LLOG_ERROR("********** 用户ID = %d IP = %s", m_user[i]->GetUserData().m_id, m_user[i]->m_userData.m_customString1.c_str());
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
			m_couFei = true;

			Lint nFree = 0;
			//LTime cur;
			//Lint curTime = cur.Secs();
			//if (curTime > m_startTime && curTime < m_endTime)
			//{
			//	nFree = 1;
			//}
			//
			//if (m_state != 100010 && m_state != 100008 && m_state != 100009) {
			//	nFree = 0;
			//}
			//if (nFree) {
			//	LLOG_INFO("**************房主扣费    在免费时间 不需要扣费");
			//}

			//普通房间扣费
			//if (m_clubInfo.m_clubId==0&&m_clubInfo.m_clubDeskId==0)  //普通桌子
			if(true)
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
						if(m_vip)
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
								if(m_vip)
									m_vip->UpdateUserFeeValueToDB(m_user[i]->GetUserDataId(), m_even); // 修改数据库
								nTotalDeskFeeValue+=m_even;
								m_alreadyFee[i] = 1;
							}
						}
						if(m_vip)
							m_vip->UpdateDeskTotalFeeValueToDB(nTotalDeskFeeValue);
					}
				}
			}
			else   //俱乐部桌子扣费
			{
				if (m_feeType==0)         //会长扣费
				{
					Lint temp_cost = m_cost;
					switch (m_state)
					{
						case YingSanZhang:
						{
							//5-8人
							if (m_playtype.size() >= 7 && m_playtype[6] == 8)
							{
								temp_cost = 2 * m_cost;
							}
							break;
						}
						case NiuNiu:
						{
							if (m_playtype.size() >= 6 && m_playtype[5] == 10)
							{
								temp_cost = 2 * m_cost;
							}
							break;
						}
						case NiuNiu_New:
						{
							if (m_playtype.size() >= 6 && m_playtype[5] == 10)
							{
								temp_cost = 2 * m_cost;
							}
							break;
						}
						case CheXuan:
						{
							//TODO:
							break;
						}
						
					}
					LLOG_ERROR("******clubShowDesk:%d  会长扣费  creatUserId = %d  cost = %d", m_clubInfo.m_showDeskId, m_creatUserId, temp_cost);
					//m_vip->UpdateClubCreatorNumDiamondToDB(m_cost, m_creatUserId);
					HanderDelCardCount(cardType, temp_cost, CARDS_OPER_TYPE_CLUB_DIAMOND, "system", m_feeType, m_creatUserId, m_unioid); //房主不在线
					if(m_vip)
						m_vip->UpdateDeskTotalFeeValueToDB(temp_cost);
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
								if(m_vip)
									m_vip->UpdateUserFeeValueToDB(m_user[i]->GetUserDataId(), m_even); // 修改数据库
								nTotalDeskFeeValue+=m_even;
								m_alreadyFee[i] = 1;
							}
						}
						if(m_vip)
							m_vip->UpdateDeskTotalFeeValueToDB(nTotalDeskFeeValue);
					}
				}

			}
	
		}

		//针对动态加入的扣费问题
		if (m_state == YingSanZhang || 
			TuiTongZi == m_state || 
			NiuNiu == m_state || 
			NiuNiu_New == m_state ||
			CheXuan == m_state)
		{
			if (m_vip && m_vip->m_curCircle > 1  &&!m_vip->m_reset && 1 == m_feeType)
			{
				LLOG_DEBUG("Desk::HanderGameOver  m_curCircle  > 1 check fee");
				Lint cardType = CARD_TYPE_8;
				//if (m_vip->m_maxCircle == 8)
				//	cardType = CARD_TYPE_8;
				//else if (m_vip->m_maxCircle == 16)
				//	cardType = CARD_TYPE_16;
				cardType = getDrawType(m_state, m_vip->m_maxCircle);

				bool TotalFeeChange = false;
				for (int i = 0; i < m_iPlayerCapacity; i++)
				{
					if (m_user[i] == NULL || m_alreadyFee[i] == 1 || m_playStatus[i]!=1)continue;
					m_user[i]->DelCardCount(cardType, m_even, CARDS_OPER_TYPE_CREATE_ROOM, "system", m_feeType); //分摊房费
					if (m_vip)
						m_vip->UpdateUserFeeValueToDB(m_user[i]->GetUserDataId(), m_even); // 修改数据库
					m_alreadyFee[i] = 1;
					TotalFeeChange = true;
				}
				if (TotalFeeChange)
				{
					Lint nTotalDeskFeeValue = 0;
					for (int i = 0; i < m_iPlayerCapacity; i++)
					{
						if (m_alreadyFee[i] == 1)
							nTotalDeskFeeValue += m_even;
					}
					if (m_vip)
						m_vip->UpdateDeskTotalFeeValueToDB(nTotalDeskFeeValue);
				}

			}
		}
	}

	if(m_vip->isNormalEnd())
	{
		LLOG_DEBUG("Desk::HanderGameOver   m_vip->isNormalEnd()");
		//不再定时准备
		m_flush_round_end_time = false;

		//清空lookon用户
		//MHClearLookonUser(1);

		//删除会长观战
		//MHClearClubOwerLookOn(1);

		if(m_deskType == DeskType_Common)
		{
			//LMsgL2LDBSaveCRELog log;
			//if(m_user[0]!=NULL)
			//	log.m_strUUID = m_user[0]->m_userData.m_unioid;
			//log.m_deskID = this->m_id;
			//log.m_strLogId = m_vip->m_id;
			//log.m_time = m_vip->m_time;
			//for(Lint i = 0; i < m_iPlayerCapacity; ++i)
			//{
			//	if(m_user[i]!=NULL)
			//		log.m_id[i] = m_user[i]->m_userData.m_id;
			//}
			//gWork.SendMsgToDb(log);
		}
	}

	if(m_vip->isEnd() || result == 2)
	{
		LLOG_DEBUG("Desk::HanderGameOver() Send Last Result To Client...  deskId=[%d]", this->GetDeskId());
		//不再定时准备
		m_flush_round_end_time = false;

		//俱乐部会长在明牌观战中
		/*if (m_clubInfo.m_clubId != 0 && this->m_clubOwerLookOnUser)
		{
			LLOG_DEBUG("Desk::HanderGameOver LMsgL2LMGUserLeaveClubDesk");
			LMsgL2LMGUserLeaveClubDesk  send;
			send.m_clubId = m_clubInfo.m_clubId;
			send.m_playTypeId = m_clubInfo.m_playTypeId;
			send.m_clubDeskId = m_clubInfo.m_clubDeskId;
			send.m_isClubOwerLooking = 0;
			send.m_userId = this->m_clubOwerLookOnUser->GetUserDataId();
			send.m_strUUID = this->m_clubOwerLookOnUser->m_userData.m_unioid;
			gWork.SendToLogicManager(send);
		}*/

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

		//发送manager
		//if (m_clubInfo.m_clubId!=0)
		//{
		//	LLOG_DEBUG("Desk::HanderGameOver LMsgL2LMGUserLeaveClubDesk");
		//	LMsgL2LMGUserLeaveClubDesk  send;
		//	send.m_clubId=m_clubInfo.m_clubId;
		//	send.m_playTypeId=m_clubInfo.m_playTypeId;
		//	send.m_clubDeskId=m_clubInfo.m_clubDeskId;
		//	for (int i=0; i<m_iPlayerCapacity; i++)
		//	{
		//		if (m_user[i])
		//		{
		//			send.m_isClubOwerLooking = 0;
		//			send.m_userId=m_user[i]->GetUserDataId();
		//			send.m_strUUID=m_user[i]->m_userData.m_unioid;
		//			gWork.SendToLogicManager(send);
		//		}
		//	}
		//}

		//竞技场牌局中只有一个玩家能量值符合条件，自动强制解散
		if (result == 2)
		{
			LMsgS2CResetDesk send;
			send.m_errorCode = 6;  //只有一人能量值符合条件，自动强制解散
			MHBoadCastAllDeskUser(send);
			MHBoadCastDeskLookonUser(send);
			MHBoadCastClubOwerLookOnUser(send);

			if (m_vip)
			{
				m_vip->m_reset = 1;
				if (m_vip->m_curCircle >= 1) // 局结束,下局未开始点
				{
					m_vip->UpdateToDb();
				}
			}
		}

		LLOG_DEBUG("Desk::HanderGameOver  m_vip->SendEnd()");

		//match结束，推送大结算
		m_vip->SendEnd();

		m_vip->m_desk = NULL;
		//清除入座玩家
		ClearUser();
		//游戏结束剔除lookon玩家
		MHClearLookonUser(1);
		//清除会长观战
		MHClearClubOwerLookOn(1);

		SetDeskFree();
		SetVip(NULL);
		ResetClear();
	}
	SpecialCardClear();
}


void Desk::HanderAddCardCount(Lint pos, Lint CardNum, CARDS_OPER_TYPE AddType, Lstring admin)
{
	if(pos < 0 || pos >= INVAILD_POS_QIPAI)
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

//Manager更新Logic上玩家金币数量
void Desk::HanderUpdateUserCoins(User* pUser, LMsgLMG2LUpdatePointCoins* msg)
{
	if (pUser == nullptr || msg == nullptr)
	{
		LLOG_ERROR("Desk::HanderUpdateUserCoins() Error!!! user is NULL deskId=[%d]", m_id);
		return;
	}
	if (!m_vip)
	{
		LLOG_ERROR("Desk::HanderUpdateUserCoins() Error!!! vip is NULL deskId=[%d]", m_id);
		return;
	}
	if (m_clubInfo.m_clubId == 0 || m_isCoinsGame == 0 || msg->m_clubId == 0 || msg->m_clubId != m_clubInfo.m_clubId)
	{
		LLOG_ERROR("Desk::HanderUpdateUserCoins() Error!!! is not CoinsGame or palyer is not in this club deskId=[%d]", m_id);
		return;
	}

	Lint pos = GetUserPos(pUser);
	LLOG_ERROR("Desk::HanderUpdateUserCoins() Run... user is NULL deskId=[%d], userId=[%d], pos=[%d], changeCoins=[%d], coins=[%d]", 
		this->m_id, pUser->m_userData.m_id, pos, msg->m_changeCoins, pUser->m_userData.m_coins);

	if(pos != INVAILD_POS_QIPAI)
	{ 
		m_vip->m_coins[pos] += msg->m_changeCoins;
		pUser->m_userData.m_coins = m_vip->m_coins[pos];
	}
	
	/*
	LMsgS2CUpdateCoins updateCoins;
	updateCoins.m_pos = pos;
	updateCoins.m_userId = pUser->m_userData.m_id;
	updateCoins.m_coins = pUser->m_userData.m_coins;

	if (pos != INVAILD_POS_QIPAI)
	{
		MHBoadCastAllDeskUser(updateCoins);
		MHBoadCastDeskLookonUser(updateCoins);
		MHBoadCastClubOwerLookOnUser(updateCoins);
	}
	else
	{
		pUser->Send(updateCoins);
	}
	*/
	//已入座玩家
	if (pos != INVAILD_POS_QIPAI)
	{
		if (CheXuan == m_state && mGameHandler)
		{
			mGameHandler->UpdatePlayerAccumCoins(pUser, msg->m_changeCoins);
		}
	}
	//未入座玩家
	else if (pos == INVAILD_POS_QIPAI && GetUserCount() < GetPlayerCapacity() && m_isCoinsGame == 1 && 
		m_clubInfo.m_clubId != 0 && pUser->m_userData.m_coins >= this->m_inDeskMinCoins)
	{
		//62065：能量值足够加入房间
		LMsgS2CLookOnPlayerFill notFull;
		notFull.m_errorCode = 3;  //0：入座按钮从先显示
		notFull.m_state = m_state;
		pUser->Send(notFull);
	}
}

//能量不足玩家充上能量后从新点击入座 61187
void Desk::HanderStandPlayerReSeat(User* pUser, LMsgC2SStandPlayerReSeat* msg)
{
	if (!pUser || !msg)
	{
		LLOG_ERROR("Desk::HanderStandPlayerReSeat() Error!!! pUser or msg is NULL!!! deskId=[%d], userId=[%d]",
			this->m_id);
		return;
	}

	if (msg->m_pos == INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("Desk::HanderStandPlayerReSeat() Error!!! Opt reseat button pos is invalid!!! deskId=[%d], userId=[%d], pos=[%d]",
			this->m_id, pUser->m_userData.m_id, msg->m_pos);
		return;
	}

	if (m_standUpPlayerSeatDown[msg->m_pos] != 3)
	{
		LLOG_ERROR("Desk::HanderStandPlayerReSeat() Error!!! User conins deskId=[%d], userId=[%d], pos=[%d]",
			this->m_id, pUser->m_userData.m_id, msg->m_pos);
		return;
	}

	LLOG_ERROR("Desk::HanderStandPlayerReSeat() Run... Stand up user reseat sucess... deskId=[%d], userId=[%d], pos=[%d]",
		this->m_id, pUser->m_userData.m_id, msg->m_pos);

	m_standUpPlayerSeatDown[msg->m_pos] = 1;
}

//玩家请求观战玩家列表信息
void Desk::HanderViewLookOnList(User* pUser, LMsgC2SViewLookOnList* msg)
{
	if (!pUser || !msg)
	{
		LLOG_ERROR("Desk::HanderViewLookOnList() Error!!! user is NULL deskId=[%d]", m_id);
		return;
	}

	LMsgS2CRectLookOnList lookOnList;
	
	if (!m_desk_Lookon_user.empty())
	{
		auto lookIter = m_desk_Lookon_user.begin();
		for (; lookIter != m_desk_Lookon_user.end(); lookIter++)
		{
			if (*lookIter == NULL) continue;

			++lookOnList.m_lookOnCount;
			lookOnList.m_lookOnId.push_back((*lookIter)->GetUserDataId());
			lookOnList.m_lookOnNike.push_back((*lookIter)->GetUserData().m_nike);
			lookOnList.m_lookOnHead.push_back((*lookIter)->GetUserData().m_headImageUrl);
		}
	}

	if (m_clubOwerLookOnUser)
	{
		++lookOnList.m_lookOnCount;
		lookOnList.m_lookOnId.push_back(m_clubOwerLookOnUser->GetUserDataId());
		lookOnList.m_lookOnNike.push_back(m_clubOwerLookOnUser->GetUserData().m_nike);
		lookOnList.m_lookOnHead.push_back(m_clubOwerLookOnUser->GetUserData().m_headImageUrl);
	}

	pUser->Send(lookOnList);
}

void Desk::HanderDelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin, Lint feeType, Lint userId, Lstring unioid)
{
	LLOG_INFO(" 房主建房 User::DelCardCount type=%d,count=%d,operType=%d,FORCE_ROOM_CARD_FREE=%d", cardType, count, operType, FORCE_ROOM_CARD_FREE);
	if (FORCE_ROOM_CARD_FREE) {
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

//清空桌子对象数据，恢复默认值
void Desk::_clearData()
{
	m_id = 0;
	
	m_deskbeforeState = DESK_FREE;
	m_deskState = DESK_FREE;
	m_deskPlayState = -1;

	memset(m_user, 0, sizeof(m_user));
	m_vip = NULL;
	for (int i = 0; i < MAX_CHAIR_COUNT; i++) {
		m_seat[i] = INVAILD_POS_QIPAI;
	}
	//memset(m_seat, 0, sizeof(m_seat));
	memset(m_readyState, 0, sizeof(m_readyState));
	//memset(m_timeInRoom, 0, sizeof(m_timeInRoom));
	memset(m_playStatus, 0, sizeof(m_playStatus));
	memset(m_handlerPlayerStatus, 0, sizeof(m_handlerPlayerStatus));
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
	m_gameEndTime = 0;

	memset( m_autoPlay, 0, sizeof(m_autoPlay) );
	memset( m_autoPlayTime, 0, sizeof(m_autoPlayTime) );
	memset( m_autoOutTime, 0, sizeof(m_autoOutTime) );
	m_autoChangeOutTime = 0;
	m_autoPlayOutTime = 0;
	m_baseScore = 1;
	memset( m_coinsResult, 0, sizeof(m_coinsResult) );
	m_creatUser = NULL;
	m_creatUserId = 0;
	m_deskType = DeskType_Common;
	m_selectSeat = false;                   //选座状态  创建房间时为true 选择完座位时为false
	m_firestUser = NULL;
	for (int i = 0; i < MAX_CHAIR_COUNT; i++)
	{
		m_seatUser[i] = NULL;
		m_videoPermit[i] = 1;
	}
	m_zhuangPos = 0;
	m_dismissUser = false;
	m_couFei = false;
	m_finish_first_opt = 0;
	m_next_round_delay_time = 0;
	//清空
	memset(m_switchPos, 0x00, sizeof(m_switchPos));
	//m_desk_all_user.clear();
	m_desk_Lookon_user.clear();
	m_user_select_seat_turn.clear();
	m_Greater2CanStart=0;
	m_StartGameButtonPos=INVAILD_POS_QIPAI;
	m_startButtonAppear=0;
	m_clubInfo.reset();
    m_bIsDissmissed=false;
	m_Gps_Limit=0;
	m_flush_round_end_time = false;
	memset(m_alreadyFee, 0x00, sizeof(m_alreadyFee));
	m_yingsanzhang_compare_count = 0;
	m_yingsanzhang_finish_flag = 0;
	m_auto_start_game_time_flag = 0;
	m_next_delay_time = 0;
	memset(m_standUpPlayerSeatDown, 0, sizeof(m_standUpPlayerSeatDown));

	m_sanzhangpai_gameStart_time_flag = false;
	m_next_draw_lookon = false;
	m_allowLookOn = 0;
	m_clubOwerLookOn = 0;
	m_clubOwerLookOnUser = NULL;
	m_isCoinsGame = 0;					// 是否为金币场  0：普通场  1：金币场
	m_inDeskMinCoins = 0;				// 金币场入座所需的最小能量值
	m_qiangZhuangMinCoins = 0;			// 金币场参与抢庄的最小能量值
	m_isAllowCoinsNegative = 0;			// 每局结算时是否允许负数能量值出现
	m_isStartNoInRoom = 0;				// 是否开局禁止加入房间  0：开局后仍可以加入房间   1：开局后不可加入房间
	m_isAllowDynamicIn = 0;				// 是否允许动态加入牌局  0: 不允许动态加入   1：允许动态加入
	m_isManagerStart = 0;				// 是否为房主开始游戏 0：首先入座开始游戏  1：房主管理员开始游戏（人满也不自动开始）
	m_warnScore = 0;

}

//根据玩法编号，实例化具体玩法的Handler
bool Desk::_createRegister(QiPaiGameType gameType)
{
	LLOG_DEBUG("Desk::_createRegister  gameType=[%d]",gameType);
	mGameHandler = GameHandlerFactory::getInstance()->create(gameType);
	if (!mGameHandler)
	{
		LLOG_ERROR("No game handler for game type %d found.", gameType);
		return false;
	}

	mGameHandler->startup(this);
	return true;
}

bool Desk::SetHandlerPlayerStatus(const Lint* handlerPlayerStatus, Lint playerCount)
{
	if (!handlerPlayerStatus) return false;
	for (Lint i = 0; i < playerCount; ++i)
	{
		m_handlerPlayerStatus[i] = handlerPlayerStatus[i];
	}
	return true;
}

////////////////////////////////设置桌子属性/////////////////////////////

//设置是否允许观战
Lint Desk::SetIsAllowLookOn()
{
	if (m_allowLookOn == 1)
	{
		return m_allowLookOn;
	}
	
	if (NiuNiu_New == m_state)
	{
		m_allowLookOn = 1;
		return m_allowLookOn;
	}
	else if (CheXuan == m_state)
	{
		m_allowLookOn = 1;
		return m_allowLookOn;
	}
	else
	{
		m_allowLookOn = 0;
		return m_allowLookOn;
	}
	
}

//设置是否允许俱乐部会长明牌观战
Lint Desk::SetIsAllownClubOwerLookOn()
{
	if (this->m_clubInfo.m_clubId == 0)
	{
		m_clubOwerLookOn = 0;
		return m_clubOwerLookOn;
	}

	if (NiuNiu_New == m_state || NiuNiu == m_state)
	{
		m_clubOwerLookOn = 0;
		return m_clubOwerLookOn;
	}
	else if (CheXuan == m_state)
	{
		m_clubOwerLookOn = 0;
		return m_clubOwerLookOn;
	}
	
	return m_clubOwerLookOn;
}

//设置桌子是否为金币场
Lint Desk::SetIsCoinsGame()
{
	if (m_isCoinsGame == 1)
	{
		return m_isCoinsGame;
	}

	else
	{
		m_isCoinsGame = 0;
		return m_isCoinsGame;
	}
}

//设置桌子是否开始后禁止加入房间
Lint Desk::SetIsStartNoInRoom()
{
	if (m_isStartNoInRoom == 1)
	{
		return m_isStartNoInRoom;
	}

	if (NiuNiu_New == m_state && m_playtype.size() >= 7 && m_playtype[6] == 1)
	{
		m_isStartNoInRoom = 1;
		return m_isStartNoInRoom;
	}
	else if (CheXuan == m_state && m_playtype.size() >= 13 && m_playtype[12] == 1)
	{
		m_isStartNoInRoom = 1;
		return m_isStartNoInRoom;
	}

	else
	{
		m_isStartNoInRoom = 0;
		return m_isStartNoInRoom;
	}
}

//设置是否允许动态加入牌局
Lint Desk::SetIsAllowDynamicIn()
{
	if (NiuNiu_New == m_state)
	{
		m_isAllowDynamicIn = 1;
		return m_isAllowDynamicIn;
	}
	else if (m_state == YingSanZhang && m_playtype.size() >= 10 && m_playtype[9] == 1)
	{
		m_isAllowDynamicIn = 1;
		return m_isAllowDynamicIn;
	}
	else if (m_state == NiuNiu && m_playtype.size() >= 5 && m_playtype[4] == 1)
	{
		m_isAllowDynamicIn = 1;
		return m_isAllowDynamicIn;
	}
	else if (m_state == CheXuan)
	{
		m_isAllowDynamicIn = 1;
		return m_isAllowDynamicIn;
	}

	else
	{
		m_isAllowDynamicIn = 0;
		return m_isAllowDynamicIn;
	}
}

//设置是否为房主管理员开始游戏
Lint Desk::SetIsManagerStart()
{
	if (NiuNiu_New == m_state && m_playtype.size() >= 5 && m_playtype[4] == 11)
	{
		//设置开始按钮位置为-1
		m_StartGameButtonPos = INVAILD_POS_MANAGER_START;

		m_isManagerStart = 1;
		return m_isManagerStart;
	}
	else
	{
		m_isManagerStart = 0;
		return m_isManagerStart;
	}
}

//设置房间房卡消耗（有些特殊玩法房卡不一样）
Lint Desk::SetDeskCostEven()
{
	if (!m_vip)
	{
		return -1;
	}
	//牛牛十人局房卡消耗
	if (m_state == NiuNiu_New && m_playtype.size() >= 1 && m_playtype[0] == 4)
	{
		//15
		if (m_vip->m_maxCircle == 15)
		{
			m_cost = 6;
		}
		//20
		else if (m_vip->m_maxCircle == 20)
		{
			m_cost = 8;
		}
		//30
		else if (m_vip->m_maxCircle == 30)
		{
			m_cost = 12;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

//判断玩家是否为有效的入座玩家（竞技场中会有能量不足的玩家）
bool Desk::IsValidSeatDownPlayer(User* pUser)
{
	if (!pUser || !m_vip)
	{
		return false;
	}
	Lint t_pos = GetUserPos(pUser);

	//未入座状态，绝对不是有效入座玩家
	if (t_pos == INVAILD_POS_QIPAI)
	{
		return false;
	}

	//已入座 && 普通场
	if (!m_isCoinsGame)
	{
		if (CheXuan != m_state)
		{
			return true;
		}
		else if (CheXuan == m_state && m_handlerPlayerStatus[t_pos] == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	//已入座 && 第一局 
	else if (m_vip->m_curCircle == 0)
	{
		return true;
	}
	//已入座 && 竞技场 && 非第一局 && 牌局中的玩家状态有效
	else if (m_isCoinsGame && m_handlerPlayerStatus[t_pos] == 1)
	{
		return true;
	}

	else
	{
		return false;
	}
}

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
	if (pos == INVAILD_POS_QIPAI)
	{
		LLOG_ERROR("error user pos, userid = %d, pos = %d", pUser->GetUserDataId(), pos);
		return;
	}

	Lint pos_user_selected = INVAILD_POS_QIPAI;
	User * pActiveUser = 0;
	for (int i = 0; i < MAX_CHAIR_COUNT; i++)
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
		Lint pos_will_selected = INVAILD_POS_QIPAI;
		for (int j = 0; j < MAX_CHAIR_COUNT; j++)
		{
			bool bSelected = false;
			for (int i = 0; i < MAX_CHAIR_COUNT; i++)
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
		if (pos_will_selected != INVAILD_POS_QIPAI)
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
	for (int i = 0; i < MAX_CHAIR_COUNT; i++)
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

	if (pUser == NULL)
	{
		LLOG_ERROR("Desk::MHIsLookonUser() Error... The user is NULL... deskId=[%d]", m_id);
		return false;
	}

	LLOG_DEBUG("Desk::MHIsLookonUser() Run... deskId=[%d], userId=[%d]", m_id, pUser->GetUserDataId());

	auto t_gz_iter = this->m_desk_Lookon_user.begin();
	for (; t_gz_iter != this->m_desk_Lookon_user.end(); t_gz_iter++)
	{
		if (*t_gz_iter == NULL)
		{
			continue;
		}

		if (pUser == *t_gz_iter || pUser->GetUserDataId() == (*t_gz_iter)->GetUserDataId())
		{
			LLOG_DEBUG("Desk::MHIsLookonUser() This user is finded in look on list... deskId=[%d], pUser=[%08x], *userIter=[%08x], pUserId=[%d], *userIterId=[%d]",
				this->GetDeskId(), pUser, *t_gz_iter, pUser->GetUserDataId(), (*t_gz_iter)->GetUserDataId());
			return true;
		}
	}
	LLOG_DEBUG("Desk::MHIsLookonUser() Fail... This user is not in look on list, But user is in m_user=[%d]... deskId=[%d], pUser=[%08x], pUserId=[%d] ",
		this->MHIsRoomUser(pUser), this->GetDeskId(), pUser, pUser->GetUserDataId());

	return false;
}

//判断该玩家是否为已入座玩家
bool Desk::MHIsRoomUser(User* pUser)
{	
	return INVAILD_POS_QIPAI != GetUserPos(pUser);
	//return (MHIsLookonUser(pUser)||MHIsSeatUser(pUser));
}

bool Desk::MHIsUserInRoom(User* pUser)
{
	if (!pUser)
	{
		return false;
	}

	for (Lint i = 0; i<m_iPlayerCapacity; ++i)
	{
		if (m_user[i] == NULL)continue;
		if (m_user[i]->GetUserDataId() == pUser->GetUserDataId())
		{
			return true;
		}
	}
	return MHIsLookonUser(pUser);
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
	//广播座位用户
	MHBoadCastDeskSeatUser(msg);
	//广播lookon用户
	//MHBoadCastDeskLookonUser(msg);

}

//广播桌子所有用户without usr
void Desk::MHBoadCastAllDeskUserWithOutUser(LMsg& msg, User* user)
{
	LLOG_DEBUG("Logwyz-------------Desk::MHBoadCastAllDeskUser(LMsg& msg) ");	
	MHBoadCastDeskUserWithoutUser(msg, user);
	//MHBoadCastDeskLookonUserWithOutUser(msg, user);
}

//广播桌子Lookon用户
void Desk::MHBoadCastDeskLookonUser(LMsg& msg)
{
	std::list<User*>::iterator userIt;
	for (userIt=m_desk_Lookon_user.begin(); userIt!=m_desk_Lookon_user.end(); userIt++)
	{
		User* temp=*userIt;
		if (temp==NULL)continue;
		temp->Send(msg);
	}
	LLOG_ERROR("Desk::MHBoadCastDeskLookonUser() Run... deskId=[%d], look_on_user_size=[%d]", m_id, m_desk_Lookon_user.size());
}

//广播桌子Lookon用户without user
void Desk::MHBoadCastDeskLookonUserWithOutUser(LMsg& msg, User* user)
{
	std::list<User*>::iterator userIt;
	for (userIt = m_desk_Lookon_user.begin(); userIt != m_desk_Lookon_user.end(); userIt++)
	{
		User* temp = *userIt;
		if (temp == NULL || temp == user)continue;
		temp->Send(msg);
	}
	LLOG_ERROR("Desk::MHBoadCastDeskLookonUserWithOutUser() Run... deskId=[%d], look_on_user_size=[%d]", m_id, m_desk_Lookon_user.size());
}

//广播给俱乐部名牌观战会长
void Desk::MHBoadCastClubOwerLookOnUser(LMsg& msg)
{
	if (this->m_clubOwerLookOn == 0)
	{
		return;
	}
	if (m_clubOwerLookOnUser)
	{
		LLOG_ERROR("Desk::MHBoadCastClubOwerLookOnUser() Run... deskId=[%d], allow_clubOwerLookOn=[%d], clubOwerLookOnUserId=[%d]",
			m_id, m_clubOwerLookOn, m_clubOwerLookOnUser ? m_clubOwerLookOnUser->GetUserDataId() : 0);
		m_clubOwerLookOnUser->Send(msg);
	}
}

//广播桌上座位用户
void Desk::MHBoadCastDeskSeatUser(LMsg& msg)
{
	LLOG_DEBUG("MHBoadCastDeskSeatUser m_iPlayerCapacity = %d", m_iPlayerCapacity);
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i])
			m_user[i]->Send(msg);
	}
}

//广播给座位上正在参与游戏的玩家
void Desk::MHBoadCastDeskSeatPlayingUser(LMsg& msg)
{
	Lint t_bCount = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && m_playStatus[i] == 1)
		{ 
			++t_bCount;
			m_user[i]->Send(msg);
		}
	}
	LLOG_ERROR("Desk::MHBoadCastDeskSeatPlayingUser() Run... deskId=[%d], PlayerCapacity=[%d], boadCastCount=[%d]", 
		this->m_id, m_iPlayerCapacity, t_bCount);
}

//广播给动态加入玩家
void Desk::MHBoadCastDeskSeatingUser(LMsg& msg)
{
	Lint t_bCount = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && m_playStatus[i] == 2)
		{
			++t_bCount;
			m_user[i]->Send(msg);
		}
	}
	LLOG_ERROR("Desk::MHBoadCastDeskSeatingUser() Run... deskId=[%d], PlayerCapacity=[%d], boadCastCount=[%d]", 
		this->m_id, m_iPlayerCapacity, t_bCount);
}

//广播桌上座位用户without user
void Desk::MHBoadCastDeskUserWithoutUser(LMsg& msg, User* user)
{
	Lint t_bCount = 0;
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && m_user[i] != user)
		{
			++t_bCount;
			m_user[i]->Send(msg);
		}
	}
	LLOG_ERROR("Desk::MHBoadCastDeskUserWithoutUser() Run... deskId=[%d], PlayerCapacity=[%d], boadCastCount=[%d]",
		this->m_id, m_iPlayerCapacity, t_bCount);
}

//广播给管理员和入座玩家
void Desk::MHBoadCastManagerAndSeatUser(LMsg& msg, Lint nRet, const std::vector<Lint>& managerId)
{
	for (int i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && (VecHas(managerId, m_user[i]->GetUserDataId()) || (m_Greater2CanStart == 1 && nRet == 1)))
		{
			m_user[i]->Send(msg);
		}
	}

	for (auto i = m_desk_Lookon_user.begin(); i != m_desk_Lookon_user.end(); i++)
	{
		if (*i && VecHas(managerId, (*i)->GetUserDataId()))
		{
			(*i)->Send(msg);
		}
	}
}

//删除LookOn中指定的user
void Desk::MHEraseDeskLookonUserByPuser(User* user)
{
	if (user == NULL)
	{
		LLOG_ERROR("Desk::MHEraseDeskLookonUserByPuser() Error!!!, Will erase user is NULL... deskId=[%d]", m_id);
		return;
	}

	auto t_gz_iter = this->m_desk_Lookon_user.begin();
	for (; t_gz_iter != this->m_desk_Lookon_user.end(); t_gz_iter++)
	{
		if (*t_gz_iter == NULL)
		{
			continue;
		}
		if (user == *t_gz_iter || user->GetUserDataId() == (*t_gz_iter)->GetUserDataId())
		{
			this->m_desk_Lookon_user.erase(t_gz_iter);
			LLOG_DEBUG("Desk::MHEraseDeskLookonUserByPuser() Erase this user sucess... deskId=[%d]", m_id);
			return;
		}
	}
	return;
}

//删除SertList中指定的user
void Desk::MHEraseDeskSeatListUserByPuser(User* user)
{
	if (user == NULL)
	{
		LLOG_ERROR("Desk::MHEraseDeskSeatListUserByPuser() Error!!!, Will erase user is NULL... deskId=[%d]", m_id);
		return;
	}

	auto t_sl_iter = this->m_user_select_seat_turn.begin();
	for (; t_sl_iter != this->m_user_select_seat_turn.end(); t_sl_iter++)
	{
		if (*t_sl_iter == NULL)
		{
			continue;
		}
		if (user == *t_sl_iter || user->GetUserDataId() == (*t_sl_iter)->GetUserDataId())
		{
			this->m_user_select_seat_turn.erase(t_sl_iter);
			LLOG_DEBUG("Desk::MHEraseDeskSeatListUserByPuser() Erase this user sucess... deskId=[%d]", m_id);
			return;
		}
	}
	return;
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

//判断是否开始游戏
Lint Desk::MHCheckGameStart()
{
	LLOG_DEBUG("*********************************************Desk::MHCheckGameStart()");
	// 只有桌子为等待状态才继续
	if (m_deskState != DESK_WAIT)
	{
		LLOG_ERROR("Desk::MHCheckGameStart() Error!!! desk is wait state error!!! deskId=[%d], deskState=[%d]", 
			GetDeskId(), m_deskState);

		return 0;
	}

	//桌子处于解散状态不做处理
	if (m_resetTime != 0)
	{
		LLOG_ERROR("Desk::MHCheckGameStart() Error!!! desk is in reseting state error!!! deskId=[%d], deskState=[%d], resetTime=[%d]",
			GetDeskId(), m_deskState, m_resetTime);

		return 0;
	}

	//判断所有的人是否都已经准备
	LLOG_DEBUG("Logwyz  ..............twoCanStartGame=[%d]", m_Greater2CanStart);
	if (m_Greater2CanStart)   // >2 可以开始
	{
		if (GetUserCount() < 2)
		{
			LLOG_DEBUG("The player num isn't enought");
			return 0;
		}
	}
	else
	{
		if (GetUserCount() != m_iPlayerCapacity)
		{
			LLOG_DEBUG("The player num isn't enought");
			return  0;
		}
	}

	if (m_Greater2CanStart == 1) // 满2人准备开局
	{
		int ready_user_count = 0;
		//座位上人数准备状况
		for (int i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_user[i] && m_readyState[i] == 1)
			{
				ready_user_count += 1;
			}
		}

		if (m_vip->m_curCircle == 0 && ready_user_count >= GetUserCount())
		{
			LLOG_DEBUG("Ready player count[%d] >= 2, m_Greater2CanStart = 1", ready_user_count);
			return 1;
		}

		else if (m_vip->m_curCircle == 0 && ready_user_count < GetUserCount())
		{
			LLOG_DEBUG("Ready player count[%d] < 2 or m_Greater2CanStart = 1", ready_user_count);
			return 0;
		}

		else if (ready_user_count < GetUserCount())
		{
			LLOG_DEBUG("Ready player count[%d] <  desk user[%d]", ready_user_count, GetUserCount());
			return 0;
		}
	}
	else
	{
		//座位上人数准备状况
		for (int i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_user[i] && m_readyState[i] == 0)
			{
				LLOG_DEBUG("The player[%d] hasn't  been ready", m_user[i]->GetUserDataId());
				return  0;
			}
		}
	}
	if(!m_next_draw_lookon)  
		MHHanderStartGame();
	return 2;
}

//开始游戏
void Desk::MHHanderStartGame()
{
	LLOG_DEBUG("Logwyz....................MHHanderStartGame() desk:%d", m_id);

	if (m_deskState != DESK_WAIT)	// 只有桌子为等待状态才继续
	{
		LLOG_ERROR("MHHanderStartGame  Desk's status is wrong. Desk state = %d.", m_deskState);
		return;
	}

	if (m_resetTime != 0)
	{
		LLOG_ERROR("MHHanderStartGame Desk ising reseted");
		return;
	}
	if (!m_vip || m_vip->m_curCircle >= m_vip->m_maxCircle)
	{
		LLOG_ERROR("MHHanderStartGame !m_vip || m_vip->m_curCircle >= m_vip->m_maxCircle  ");
		return;
	}

	for (int i=0; i<m_iPlayerCapacity; i++)
	{
		if (m_user[i])
		{
			m_playStatus[i]=1; //设置玩家为参数游戏状态
			m_draw_user_count++;
		}
	}
	
	if (m_vip&& !m_vip->IsBegin())
	{
		m_gameStartTime = time(NULL);
	}
 
	m_zhuangPos = MHSpecPersonPos();
	LLOG_DEBUG("Logwyz MHHanderStartGame desk:%d, m_zhuangPos:%d", m_id, m_zhuangPos);

	//更新参与游戏用户到vip log中
	if (m_vip)
	{
		m_vip->ResetUser(m_user);		
	}
	
	if (mGameHandler)
	{
		//最后一局观战玩家不显示入座按钮
		if (m_vip && m_vip->m_curCircle == m_vip->m_maxCircle - 1)
		{
			//62065：通知观战玩家，游戏开始最后一局，不允许观战玩家入座
			LMsgS2CLookOnPlayerFill fill;
			fill.m_errorCode = 4;   //游戏开始最后一局，让观战玩家隐藏入座按钮
			fill.m_state = this->m_state;

			//62065：广播给所有观战玩家
			MHBoadCastDeskLookonUser(fill);
		}

		mGameHandler->MHSetDeskPlay(m_iPlayerCapacity, m_playStatus, m_iPlayerCapacity);

		if(m_next_draw_lookon)
			m_sanzhangpai_gameStart_time_flag = false;  //游戏开始，自动检测标志变为false，定时器不在其有作用
		
		//开始后不允许加入游戏
		if (MHIsDeskMatchBegin() && m_isAllowDynamicIn == 0)
		{
			//62065：通知观战玩家，游戏开始不允许动态加入
			LMsgS2CLookOnPlayerFill fill;
			fill.m_errorCode = 2;   //游戏开始不允许动态加入
			fill.m_state = this->m_state;

			//62065：广播给所有观战玩家
			MHBoadCastDeskLookonUser(fill);
		}

		LMsgS2CLookOnPlayerFill fill1;
		fill1.m_errorCode = 3;
		fill1.m_state = this->m_state;
		for (Lint i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_standUpPlayerSeatDown[i] != 3)continue;
			if (m_user[i])
			{
				m_user[i]->Send(fill1);
			}
		}

	}
	else
	{
		LLOG_ERROR("Game handler has not been created");
	}

	LLOG_ERROR("***Desk start game desk id:%d ", m_id);
	for (int i=0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i]) {
			LLOG_ERROR("***desk:%d, pos:%d, userId:%d, nike:%s IP:%s", m_id, i, m_user[i]->GetUserDataId(), m_user[i]->GetUserData().m_nike.c_str(), m_user[i]->m_userData.m_customString1.c_str());
		}
	}

}

void Desk::MHHanderStartGameSanZhangPai(LTime &currTime)
{
	LLOG_DEBUG("Logwyz....................MHHanderStartGameSanZhangPai() desk:%d", m_id);

	if (m_deskState != DESK_WAIT)	// 只有桌子为等待状态才继续
	{
		LLOG_ERROR("MHHanderStartGameSanZhangPai  Desk's status is wrong. Desk state = %d.", m_deskState);
		return;
	}

	if (m_resetTime != 0)
	{
		LLOG_ERROR("MHHanderStartGameSanZhangPai Desk ising reseted");
		return;
	}
	if (!m_vip || m_vip->m_curCircle >= m_vip->m_maxCircle)
	{
		LLOG_ERROR("MHHanderStartGameSanZhangPai !m_vip || m_vip->m_curCircle >= m_vip->m_maxCircle  ");
		return;
	}

	memset(m_playStatus, 0x00, sizeof(m_playStatus));
	for (int i = 0; i<m_iPlayerCapacity; i++)
	{
		if (m_user[i]&&m_readyState[i]==1)
		{
			m_playStatus[i] = 1; //设置玩家为参数游戏状态
			m_draw_user_count++;
		}
	}
	Lint in_room_user_count = 0;
	for (int i = 0; i<m_iPlayerCapacity; i++)
	{
		if (m_user[i] )
		{
			in_room_user_count++;
		}
	}
	
	LLOG_DEBUG("MHHanderStartGameSanZhangPai m_draw_user_count=[%d],in_room_user_count=[%d]", m_draw_user_count, in_room_user_count);

	if (m_draw_user_count < 2)
	{
		m_draw_user_count = 0;
		return;
	}
	if ( (m_sanzhangpai_gameStart_time_flag && m_draw_user_count >= 2 && currTime.Secs() >= m_sanzhangpai_gameStart_time_limit.Secs() + YINGSANZHANG_GAME_START_DELAY_TIME)  || (m_sanzhangpai_gameStart_time_flag && m_draw_user_count== in_room_user_count))
	{
		m_sanzhangpai_gameStart_time_flag = false;
		if (m_vip && !m_vip->IsBegin())
		{
			m_gameStartTime = time(NULL);
		}

		//更新参与游戏用户到vip log中
		if (m_vip)
		{
			m_vip->ResetUser(m_user);
		}

		if (mGameHandler)
		{
			LLOG_ERROR("MHHanderStartGameSanZhangPai  MHSetDeskPlay");
			mGameHandler->MHSetDeskPlay(m_iPlayerCapacity, m_playStatus, m_iPlayerCapacity);
		}
		else
		{
			LLOG_ERROR("Game handler has not been created");
		}
	}
	else
	{
		m_draw_user_count = 0;
	}

}

//解散时，清空lookon
void Desk::MHClearLookonUser(Lint outType)
{
	//LLOG_ERROR("Desk::MHClearLookonUser() Run... deskId=[%d]", this->GetDeskId());

	if (m_deskType==DeskType_Common)
	{
		LLOG_DEBUG("Desk::MHClearLookonUser() Begin clear common desk type look on user list... deskId=[%d]", this->GetDeskId());
		while (!m_desk_Lookon_user.empty())
		{
			User* temp = m_desk_Lookon_user.back();
			if (temp == NULL) continue;
			OnUserOutRoom(temp, outType);
		}

		LLOG_ERROR("Desk::MHClearLookonUser() Clear common desk type Look On User Over... deskId=[%d], lookOnUserSize=[%d]",
			this->GetDeskId(), m_desk_Lookon_user.size());

		m_desk_Lookon_user.clear();
		m_user_select_seat_turn.clear();
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

//清除俱乐部会长观战
void Desk::MHClearClubOwerLookOn(Lint outType)
{
	//LLOG_ERROR("Desk::MHClearClubOwerLookOn() Run... deskId=[%d]", this->GetDeskId());
	if (this->m_clubOwerLookOnUser)
	{
		LLOG_ERROR("Desk::MHClearClubOwerLookOn() Run... deskId=[%d]", this->GetDeskId());
		OnUserOutRoom(this->m_clubOwerLookOnUser, outType);
	}
}

//客户端发送开始游戏指令
void Desk::MHHanderStartGameButton(User* pUser, LMsgC2SStartGame *msg)
{
	LLOG_ERROR("Desk::MHHanderStartGameButton desk:%d", m_id);
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("empty user or message desk:%d", m_id);
		return;
	}

	LMsgC2SStartGame startButton;
	Lint pos = GetUserPos(pUser);

	if (m_deskState != DESK_WAIT/*&&m_deskState !=DESK_COUNT_RESULT*/)
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton state error, userid:%d deskstate=%d, desk:%d", pUser->GetUserDataId(), m_deskState, m_id);
		return;
	}

	if ((!m_isManagerStart && pos == INVAILD_POS_QIPAI) || (m_isManagerStart && !MHIsUserInRoom(pUser)))
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton pos error, userid:%d, pos:%d, desk:%d", pUser->GetUserDataId(), pos, m_id);
		return;
	}
	if ((!m_isManagerStart && pos != m_StartGameButtonPos) || (m_isManagerStart && !MHIsUserInRoom(pUser)))
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton start button pos error, userid:%d, pos:%d, m_StartGameButtonPos:[%d], desk:%d", pUser->GetUserDataId(), pos, m_StartGameButtonPos, m_id);
		return;
	}

	if(GetUserCount() < 2)
	{
		LLOG_ERROR("Desk::MHHanderStartGameButton player not enough, userid=%d   seatID=%d,m_StartGameButtonPos=[%d],MHGetSeatUserCount()=[%d]", pUser->GetUserDataId(), pos , m_StartGameButtonPos, GetUserCount());
		//这种情况，说明有人退出了，不能开始游戏，把开始按钮收回
		m_StartGameButtonPos = INVAILD_POS_QIPAI;
		return;
	}
	
	LLOG_DEBUG("Logwyz ....  MHCheckGameStart()    2553     ");
	//开始按钮位置为无效
	m_StartGameButtonPos = INVAILD_POS_QIPAI;
	m_startButtonAppear = 1;
	MHHanderStartGame();

}

//选择特殊用户的位置位置（规则：如果创建房间的人入座了，返回其位置；如果没有,选择第一个入座的人的位置;如果没有,东南西北依次)
Lint Desk::MHSpecPersonPos()
{
	if (m_creatUserId > 0)
	{
		for (int i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_user[i] && m_user[i]->GetUserDataId() == m_creatUserId)
			{
				return i;
			}
		}
	}
	
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i])
			return i;
	}
	return INVAILD_POS_QIPAI;
}

//获取第一个入座玩家的位置
Lint Desk::MHFirstSeatPos()
{
	auto t_firstIter = m_user_select_seat_turn.begin();
	for (; t_firstIter != m_user_select_seat_turn.end(); t_firstIter++)
	{
		if (*t_firstIter)
		{
			for (int i = 0; i < m_iPlayerCapacity; i++)
			{
				if (!m_user[i]) continue;
				if (m_user[i] == *t_firstIter || m_user[i]->GetUserDataId() == (*t_firstIter)->GetUserDataId())
				{
					return i;
				}
			}
		}
	}
	
	return INVAILD_POS_QIPAI;
}

//获取Pos位置下一个入座玩家的位置
Lint Desk::MHNextFirstSeatPos(Lint pos)
{
	if (pos >= MAX_CHAIR_COUNT || pos < 0)
	{
		return INVAILD_POS_QIPAI;
	}

	Lint t_nextPos = INVAILD_POS_QIPAI;
	Lint t_tmpPos = INVAILD_POS_QIPAI;
	for (int i = pos + 1; i < MAX_CHAIR_COUNT + pos; ++i)
	{
		t_tmpPos = i % MAX_CHAIR_COUNT;
		if (m_user[t_tmpPos])
		{
			t_nextPos = t_tmpPos;
			break;
		}
	}
	return t_nextPos;
}

//查找管理员在房间的列表
Lint Desk::MHGetManagerList(std::vector<Lint>& magagerList)
{
	Lint t_count = 0;
	//查找入座中的管理员
	for (int i = 0; i < INVAILD_POS_QIPAI; ++i)
	{
		if (m_user[i] && m_user[i]->GetUserDataId() == m_creatUserId)
		{
			++t_count;
			magagerList.push_back(m_user[i]->GetUserDataId());
		}
		else if (m_user[i] && VecHas(m_clubInfo.m_managerIdList, m_user[i]->GetUserDataId()))
		{
			++t_count;
			magagerList.push_back(m_user[i]->GetUserDataId());
		}
	}
	//查找观战玩家中的管理员
	for (auto i = m_desk_Lookon_user.begin(); i != m_desk_Lookon_user.end(); i++)
	{
		if (*i && (*i)->GetUserDataId() == m_creatUserId && !VecHas(magagerList, m_creatUserId))
		{
			++t_count;
			magagerList.push_back((*i)->GetUserDataId());
		}
		else if (*i && VecHas(m_clubInfo.m_managerIdList, (*i)->GetUserDataId()))
		{
			++t_count;
			magagerList.push_back((*i)->GetUserDataId());
		}
	}
	//会长
	if (m_clubOwerLookOnUser && !VecHas(magagerList, m_clubOwerLookOnUser->GetUserDataId()))
	{
		++t_count;
		magagerList.push_back(m_clubOwerLookOnUser->GetUserDataId());
	}

	if (!VecHas(magagerList, m_creatUserId))
	{
		++t_count;
		magagerList.push_back(m_creatUserId);
	}
	return t_count;
}

void Desk::HanderPokerGameMessage(User* pUser, LMsgC2SGameCommon *msg)
{
	LLOG_DEBUG("Desk::HanderPokerGameMessage");
	if (pUser==NULL||msg==NULL)
	{
		LLOG_ERROR("Desk::HanderPokerGameMessage  pUser or msg ==NULL");
		return;
	}
	
	if (m_deskState!=DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderPokerGameMessage state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderGameMessage(pUser, msg);
	}

}

void Desk::HanderYingSanZhangAddScore(User* pUser,YingSanZhangC2SAddScore*msg)
{
	LLOG_DEBUG("Desk::HanderYingSanZhangAddScore");
	if (pUser==NULL||msg==NULL)
	{
		LLOG_ERROR("Desk::HanderYingSanZhangAddScore  pUser or msg ==NULL");
		return;
	}

	if (m_deskState!=DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderPokerGameMessage state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderYingSanZhangAddScore(pUser, msg);
	}
}

void Desk::HanderYingSanZhangUserSelectBigSmall(User* pUser, YingSanZhangC2SUserSelectBigSmall* msg)
{
	LLOG_DEBUG("Desk::YingSanZhangC2SUserSelectBigSmall");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::YingSanZhangC2SUserSelectBigSmall  pUser or msg ==NULL");
		return;
	}

	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderPokerGameMessage state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderYingSanZhangUserSelectBigSmall(pUser, msg);
	}

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
	if (send.m_clubId == 0 || send.m_playTypeId == 0)return;
		gWork.SendToLogicManager(send);

}

bool Desk::MHCheckUserGPSLimit(User *pUser, Lstring & desk_gps_list)
{
	bool bRet=true;
	if (NULL==pUser)
	{
		LLOG_ERROR("Desk::MHCheckUserGPSLimit, param error, empty user, desk:%d, GPSLimit=%d", m_id, m_Gps_Limit);
		return false;
	}
	LLOG_ERROR("Desk::MHCheckUserGPSLimit, desk:%d, user:%d", m_id, pUser->GetUserDataId());
	if (m_Gps_Limit==0)
		return bRet;
	std::ostringstream ss;
	Lstring user_name=string_replace(pUser->m_userData.m_nike, Lstring(","), Lstring(""));
	user_name=string_replace(user_name, Lstring("|"), Lstring(""));
	ss<<user_name<<",";
	ss<<pUser->m_userData.m_customString2;
	//{
	//	//进入用户和在座用户
	//	boost::mutex::scoped_lock l(m_mutexDeskLookonUser);
	//	for (auto it=m_desk_Lookon_user.begin(); it!=m_desk_Lookon_user.end(); it++)
	//	{
	//		if ((*it)->IsInLimitGPSPosition(*pUser))
	//			bRet=false;
	//		if (pUser->GetUserDataId()!=(*it)->GetUserDataId())
	//		{
	//			Lstring user_name=string_replace((*it)->m_userData.m_nike, Lstring(","), Lstring(""));
	//			user_name=string_replace(user_name, Lstring("|"), Lstring(""));
	//			ss<<"|"<<user_name<<","<<(*it)->m_userData.m_customString2;
	//		}
	//	}
	//}
	for (int i=0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i])
		{
			if (m_user[i]->IsInLimitGPSPosition(*pUser))
				bRet=false;
			if (pUser->GetUserDataId()!=m_user[i]->GetUserDataId())
			{
				Lstring user_name=string_replace(m_user[i]->m_userData.m_nike, Lstring(","), Lstring(""));
				user_name=string_replace(user_name, Lstring("|"), Lstring(""));
				ss<<"|"<<user_name<<","<<m_user[i]->m_userData.m_customString2;
			}
		}
	}
	desk_gps_list=ss.str();
	return bRet;
}

Lint Desk::MHGetDeskUserCount()
{
	return GetUserCount()+m_desk_Lookon_user.size();
}

Lint Desk::MHGetHanlderPlayerRealCount()
{
	if (mGameHandler)
	{
		return mGameHandler->GetRealPlayerCount();
	}
	return 0;
}

Lint Desk::MHGetNextRoundDelayTime()
{
	return 0;
}

Lint Desk::MHGetDeskUser(std::vector<Lstring> &seatPlayerName, std::vector<Lstring> &noSeatPlayerName)
{
	//入座
	for (int i=0; i<DESK_USER_COUNT; i++)
	{
		if (m_user[i])
			seatPlayerName.push_back(m_user[i]->m_userData.m_nike);
	}
	//为入座
	//for (auto ItLookonUser=m_desk_Lookon_user.begin(); ItLookonUser!=m_desk_Lookon_user.end(); ItLookonUser++)
	//{
	//	noSeatPlayerName.push_back((*ItLookonUser)->m_userData.m_nike);
	//}

	return 0;
}

void  Desk::HanderTuiTongZiAddScore(User* pUser, TuiTongZiC2SScore*msg)
{
	LLOG_DEBUG("Desk::HanderTuiTongZiAddScore");
	if (pUser==NULL||msg==NULL)
	{
		LLOG_ERROR("Desk::HanderTuiTongZiAddScore  pUser or msg ==NULL");
		return;
	}
	if (m_deskState!=DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderTuiTongZiAddScore state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderTuiTongZiAddScore(pUser, msg);
	}
}

void  Desk::HanderTuiTongZiSelectZhuang(User* pUser, TuiTongZiC2SSelectZhuang*msg)
{
	LLOG_DEBUG("Desk::HanderTuiTongZiSelectZhuang");
	if (pUser==NULL||msg==NULL)
	{
		LLOG_ERROR("Desk::HanderTuiTongZiSelectZhuang  pUser or msg ==NULL");
		return;
	}
	if (m_deskState!=DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderTuiTongZiSelectZhuang state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderTuiTongZiSelectZhuang(pUser, msg);
	}
}

void  Desk::HanderTuiTongZiDoShaiZi(User* pUser, TuiTongZiC2SDoShaiZi*msg)
{
	LLOG_DEBUG("Desk::HanderTuiTongZiDoShaiZi");
	if (pUser==NULL||msg==NULL)
	{
		LLOG_ERROR("Desk::HanderTuiTongZiDoShaiZi  pUser or msg ==NULL");
		return;
	}
	if (m_deskState!=DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderTuiTongZiDoShaiZi state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderTuiTongZiDoShaiZi(pUser, msg);
	}
}

void Desk::HanderTuiTongOpenCard(User* pUser, TuiTongZiC2SOpenCard*msg)
{
	LLOG_DEBUG("Desk::HanderTuiTongOpenCard");
	if (pUser==NULL||msg==NULL)
	{
		LLOG_ERROR("Desk::HanderTuiTongOpenCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState!=DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderTuiTongOpenCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderTuiTongOpenCard(pUser, msg);
	}
}

/* 牛牛：处理玩家选庄 */
void  Desk::HanderNiuNiuSelectZhuang(User* pUser,NiuNiuC2SSelectZhuang* msg)
{
	LLOG_DEBUG("Desk::HanderNiuNiuSelectZhuang");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HanderNiuNiuSelectZhuang  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderNiuNiuSelectZhuang state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderNiuNiuSelectZhuang(pUser, msg);
	}
}

/*
牛牛：C->S 玩家买码
MSG_C_2_S_NIUNIU_MAI_MA = 62082
*/
void  Desk::HanderNiuNiuMaiMa(User* pUser, NiuNiuC2SMaiMa* msg)
{
	LLOG_DEBUG("Desk::HanderNiuNiuMaiMa");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HanderNiuNiuMaiMa  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderNiuNiuMaiMa state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderNiuNiuMaiMa(pUser, msg);
	}
}

/* 牛牛：处理玩家下注 */
void  Desk::HanderNiuNiuAddScore(User* pUser, NiuNiuC2SAddScore* msg)
{
	LLOG_DEBUG("Desk::HanderNiuNiuAddScore");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HanderNiuNiuAddScore  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderNiuNiuAddScore state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderNiuNiuAddScore(pUser, msg);
	}
}

/* 牛牛：处理玩家亮牌 */
void Desk::HanderNiuNiuOpenCard(User* pUser, NiuNiuC2SOpenCard* msg)
{
	LLOG_DEBUG("Desk::HanderNiuNiuOpenCard");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HanderNiuNiuOpenCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HanderNiuNiuOpenCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderNiuNiuOpenCard(pUser, msg);
	}
}

/*
牛牛：玩家搓公牌
MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
*/
void Desk::HandNiuNiuCuoGongPai(User* pUser, NiuNiuC2SCuoGongPai* msg)
{
	LLOG_DEBUG("Desk::HandNiuNiuCuoGongPai");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandNiuNiuCuoGongPai  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandNiuNiuCuoGongPai state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandNiuNiuCuoGongPai(pUser, msg);
	}
}

/*
牛牛：玩家选择托管消息
MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
*/
void Desk::HandNiuNiuTuoGuan(User* pUser, NiuNiuC2STuoGuan* msg)
{
	LLOG_DEBUG("Desk::HandNiuNiuTuoGuan");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandNiuNiuTuoGuan  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandNiuNiuTuoGuan state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandNiuNiuTuoGuan(pUser, msg);
	}
}

/*
牛牛：玩家取消托管消息
MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
*/
void Desk::HandNiuNiuCancelTuoGuan(User* pUser, NiuNiuC2SCancelTuoGuan* msg)
{
	LLOG_DEBUG("Desk::HandNiuNiuCancelTuoGuan");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandNiuNiuCancelTuoGuan  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandNiuNiuCancelTuoGuan state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandNiuNiuCancelTuoGuan(pUser, msg);
	}
}

/*
客户端通知服务器开始计时
MSG_C_2_S_START_TIME = 61198
*/
void Desk::HandNiuNiuStartTime(User* pUser, NiuNiuC2SStartTime* msg)
{
	LLOG_DEBUG("Desk::HandNiuNiuStartTime");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandNiuNiuStartTime  pUser or msg ==NULL");
		return;
	}
	if (m_finish_first_opt != 0)
	{
		return;
	}
	++m_finish_first_opt;
	if (m_deskState == DESK_PLAY)
	{
		if (mGameHandler)
		{
			mGameHandler->HandNiuNiuStartTime(pUser, msg);
		}
	}
	else if (m_deskState == DESK_WAIT)
	{
		//急速模式
		if (m_playtype.size() >= 24 && m_playtype[23] == 1)
		{
			m_next_round_delay_time = 1;
		}
		//普通模式
		else
		{
			m_next_round_delay_time = NEWNIUNIU_READY_DELAY_TIME;
		}
		
		m_round_end_time.Now();
	}
}


//斗地主
void Desk::HanderDouDiZhuGameMessage(User * pUser, MHLMsgDouDiZhuC2SGameMessage * msg)
{
	LLOG_DEBUG("Desk::HanderDouDiZhuGameMessage");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR(" Desk::HanderDouDiZhuGameMessage  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR(" Desk::HanderDouDiZhuGameMessage state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleDouDiZhuGameMessage(pUser, msg);
	}
}

/*
双升：C->S 玩家报主操作
MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202
*/
void Desk::HandleShuangShengSelectZhu(User* pUser, ShuangShengC2SSelectZhu* msg)
{
	LLOG_DEBUG("Desk::HandleShuangShengSelectZhu");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleShuangShengSelectZhu  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleShuangShengSelectZhu state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleShuangShengSelectZhu(pUser, msg);
	}
}

/*
双升：C->S 玩家反主操作
MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204
*/
void Desk::HandleShangShengFanZhu(User* pUser, ShuangShengC2SFanZhu* msg)
{
	LLOG_DEBUG("Desk::HandleShangShengFanZhu");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleShangShengFanZhu  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleShangShengFanZhu state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleShangShengFanZhu(pUser, msg);
	}
}

/*
双升：C->S 玩家盖底牌操作
MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206
*/
void Desk::HandleShuangShengBaseCards(User* pUser, ShuangShengC2SBaseCards* msg)
{
	LLOG_DEBUG("Desk::HandleShuangShengBaseCards");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleShuangShengBaseCards  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleShuangShengBaseCards state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleShuangShengBaseCards(pUser, msg);
	}
}

/*
双升：C->S 玩家出牌操作
MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208
*/
void Desk::HandleShuangShengOutCards(User* pUser, ShuangShengC2SOutCards* msg)
{
	LLOG_DEBUG("Desk::HandleShuangShengOutCards");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleShuangShengOutCards  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleShuangShengOutCards state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleShuangShengOutCards(pUser, msg);
	}
}

/*
3打2：C->S 玩家选择叫牌分
MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
*/
void Desk::HandleSanDaErSelectScore(User* pUser, SanDaErC2SSelectScore* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErSelectScore");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErSelectScore  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErSelectScore state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErSelectScore(pUser, msg);
	}
}

/*
3打2：C->S 玩家选主牌
MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
*/
void Desk::HandleSanDaErSelectMain(User* pUser, SanDaErC2SSelectMain* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErSelectMain");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErSelectMain  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErSelectMain state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErSelectMain(pUser, msg);
	}
}

/*
3打2：C->S 玩家埋底
MSG_C_2_S_SANDAER_BASE_CARD = 62257
*/
void Desk::HandleSanDaErBaseCard(User* pUser, SanDaErC2SBaseCard* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErBaseCard");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErBaseCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErBaseCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErBaseCard(pUser, msg);
	}
}

/*
3打2：C->S 玩家选对家牌
MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
*/
void Desk::HandleSanDaErSelectFriend(User* pUser, SanDaErC2SSelectFriend* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErSelectFriend");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErSelectFriend  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErSelectFriend state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErSelectFriend(pUser, msg);
	}
}

/*
3打2：C->S 玩家出牌
MSG_C_2_S_SANDAER_OUT_CARD = 62261
*/
void Desk::HandleSanDaErOutCard(User* pUser, SanDaErC2SOutCard* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErOutCard");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErOutCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErOutCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErOutCard(pUser, msg);
	}
}

/*
3打2：C->S 玩家回复闲家得分慢105分,是否停止游戏
MSG_C_2_S_SANDAER_SCORE_105_RET = 62266
*/
void Desk::HandleSanDaErScore105Ret(User* pUser, SanDaErC2SScore105Ret* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErScore105Ret");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErScore105Ret  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErScore105Ret state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErScore105Ret(pUser, msg);
	}
}

/*
3打2：C->S 庄家请求底牌
MSG_C_2_S_USER_GET_BASECARDS = 61027,		//
*/
void Desk::HandleSanDaErGetBaseCards(User* pUser, SanDaErC2SGetBaseCards* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErGetBaseCards");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErGetBaseCards  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErGetBaseCards state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErGetBaseCards(pUser, msg);
	}
}

/*
3打2：C->S 客户端庄家选择交牌
MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272
*/
bool Desk::HandleSanDaErZhuangRequestJiaoPai(User* pUser, SanDaErC2SZhuangJiaoPai* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErZhuangRequestJiaoPai");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErZhuangRequestJiaoPai  pUser or msg ==NULL");
		return false;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErZhuangRequestJiaoPai state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return false;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErZhuangRequestJiaoPai(pUser, msg);
	}
}
/*
C->S 非庄家  客户端选择庄家交牌的申请
MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274
*/
bool Desk::HandleSanDaErXianSelectJiaoPai(User* pUser, SanDaErC2SXianSelectJiaoPai* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaErXianSelectJiaoPai");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaErXianSelectJiaoPai  pUser or msg ==NULL");
		return false;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaErXianSelectJiaoPai state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return false;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaErXianSelectJiaoPai(pUser, msg);
	}
}


/*
3打1：C->S 玩家选择叫牌分
MSG_C_2_S_SANDAYI_SELECT_SCORE = 62253
*/
void Desk::HandleSanDaYiSelectScore(User* pUser, SanDaYiC2SSelectScore* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaYiSelectScore");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaYiSelectScore  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaYiSelectScore state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaYiSelectScore(pUser, msg);
	}
}

/*
3打1：C->S 玩家选主牌
MSG_C_2_S_SANDAYI_SELECT_MAIN = 62255
*/
void Desk::HandleSanDaYiSelectMain(User* pUser, SanDaYiC2SSelectMain* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaYiSelectMain");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaYiSelectMain  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaYiSelectMain state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaYiSelectMain(pUser, msg);
	}
}

/*
3打1：C->S 玩家埋底
MSG_C_2_S_SANDAYI_BASE_CARD = 62307
*/
void Desk::HandleSanDaYiBaseCard(User* pUser, SanDaYiC2SBaseCard* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaYiBaseCard");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaYiBaseCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaYiBaseCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaYiBaseCard(pUser, msg);
	}
}

/*
3打1：C->S 玩家出牌
MSG_C_2_S_SANDAYI_OUT_CARD = 62261
*/
void Desk::HandleSanDaYiOutCard(User* pUser, SanDaYiC2SOutCard* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaYiOutCard");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaYiOutCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaYiOutCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaYiOutCard(pUser, msg);
	}
}

/*
3打1：C->S 玩家回复闲家得分慢105分,是否停止游戏
MSG_C_2_S_SANDAYI_SCORE_105_RET = 62266,
*/
void Desk::HandleSanDaYiScore105Ret(User* pUser, SanDaYiC2SScore105Ret* msg)
{
	LLOG_DEBUG("Desk::HandleSanDaYiScore105Ret");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleSanDaYiScore105Ret  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleSanDaYiScore105Ret state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleSanDaYiScore105Ret(pUser, msg);
	}
}


/*
五人百分：C->S 玩家选择叫牌分
MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
*/
void Desk::HandleWuRenBaiFenSelectScore(User* pUser, WuRenBaiFenC2SSelectScore* msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenSelectScore");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenSelectScore  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenSelectScore state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenSelectScore(pUser, msg);
	}
}

/*
五人百分：C->S 玩家选主牌
MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
*/
void Desk::HandleWuRenBaiFenSelectMain(User* pUser, WuRenBaiFenC2SSelectMain* msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenSelectMain");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenSelectMain  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenSelectMain state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenSelectMain(pUser, msg);
	}
}

/*
五人百分：C->S 玩家埋底
MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
*/
void Desk::HandleWuRenBaiFenBaseCard(User* pUser, WuRenBaiFenC2SBaseCard* msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenBaseCard");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenBaseCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenBaseCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenBaseCard(pUser, msg);
	}
}

/*
五人百分：C->S 玩家选对家牌
MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
*/
void Desk::HandleWuRenBaiFenSelectFriend(User* pUser, WuRenBaiFenC2SSelectFriend* msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenSelectFriend");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenSelectFriend  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenSelectFriend state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenSelectFriend(pUser, msg);
	}
}

/*
五人百分：C->S 玩家出牌
MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
*/
void Desk::HandleWuRenBaiFenOutCard(User* pUser, WuRenBaiFenC2SOutCard* msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenOutCard");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenOutCard  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenOutCard state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenOutCard(pUser, msg);
	}
}

/*
五人百分：C->S 玩家回复闲家得分慢105分,是否停止游戏
MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,
*/
void Desk::HandleWuRenBaiFenScore105Ret(User* pUser, WuRenBaiFenC2SScore105Ret* msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenScore105Ret");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenScore105Ret  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenScore105Ret state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenScore105Ret(pUser, msg);
	}
}

/*
扯炫：C->S 处理玩家起钵钵
MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
*/
void Desk::HandlerPlayerSelectBoBo(User* pUser, CheXuanC2SPlayerSelectBoBo* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerSelectBoBo");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerSelectBoBo  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandlerPlayerSelectBoBo state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerSelectBoBo(pUser, msg);
	}
}

/*
扯炫：C->S 处理玩家选择操作
MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
*/
void Desk::HandlerPlayerSelectOpt(User* pUser, CheXuanC2SPlayerSelectOpt* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerSelectOpt");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerSelectOpt  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandlerPlayerSelectOpt state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerSelectOpt(pUser, msg);
	}
}

/*
扯炫：C->S 处理玩家扯牌操作
MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
*/
void Desk::HandlerPlayerChePai(User* pUser, CheXuanC2SPlayerChePai* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerChePai");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerChePai  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandlerPlayerChePai state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerChePai(pUser, msg);
	}
}

/*
C->S 玩家搓牌操作
MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
*/
void Desk::HandlerPlayerCuoPai(User* pUser, CheXuanC2SPlayerCuoPai* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerCuoPai");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerCuoPai  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandlerPlayerCuoPai state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerCuoPai(pUser, msg);
	}
}

/*
C->S 客户端牌局内请求战绩
MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
*/
void Desk::HandlerPlayerRequestRecord(User* pUser, CheXuanC2SPlayerRequestRecord* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerRequestRecord");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerRequestRecord  pUser or msg ==NULL");
		return;
	}
	if (!MHIsDeskMatchBegin())
	{
		LLOG_ERROR("Desk::HandlerPlayerRequestRecord state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (!MHIsRoomUser(pUser))
	{
		LLOG_ERROR("Desk::HandlerPlayerRequestRecord User is not seat user userId=[%d]", pUser->m_userData.m_id);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerRequestRecord(pUser, msg);
	}
}

/*
C->S 客户端操作默分
MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
*/
void Desk::HandlerPlayerOptMoFen(User* pUser, CheXuanC2SPlayerOptMoFen* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerOptMoFen");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerOptMoFen  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandlerPlayerOptMoFen state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerOptMoFen(pUser, msg);
	}
}

/*
C->S 客户端选择自动操作
MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
*/
void Desk::HandlerPlayerSelectAutoOpt(User* pUser, CheXuanC2SPlayerSelectAutoOpt* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerSelectAutoOpt");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerSelectAutoOpt  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandlerPlayerSelectAutoOpt state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerSelectAutoOpt(pUser, msg);
	}
}

/*
C->S 客户端选择站起或坐下
MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
*/
void Desk::HandlerPlayerStandOrSeat(User* pUser, CheXuanC2SPlayerSelectStandOrSeat* msg)
{
	LLOG_DEBUG("Desk::HandlerPlayerStandOrSeat");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandlerPlayerStandOrSeat  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandlerPlayerStandOrSeat state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandlerPlayerStandOrSeat(pUser, msg);
	}
}

//设置满两人可以开局属性
void Desk::MHSetGreater2CanStartGame(Lint can, std::vector<Lint>& playType)
{
	LLOG_DEBUG("MHSetGreater2CanStartGame m_state[%d]", m_state);
	switch (m_state)
	{
	case YingSanZhang:
	{
		m_Greater2CanStart = 1;
		break;
	}
	case TuiTongZi:
	{
		if (playType.size() >= 6 && playType[5] == 1)
		{
			m_Greater2CanStart = 1;
			LLOG_DEBUG("MHSetGreater2CanStartGame m_Greater2CanStart=[%d] ", m_Greater2CanStart);
		}
		else
		{
			m_Greater2CanStart = 0;
			LLOG_DEBUG("MHSetGreater2CanStartGame m_Greater2CanStart=[%d] ", m_Greater2CanStart);
		}
		break;
	}
	case NiuNiu:
	{
		m_Greater2CanStart = 1;
		break;
	}
	case NiuNiu_New:
	{
		m_Greater2CanStart = 1;
		break;
	}
	case CheXuan:
	{
		if (playType.size() >= 5 && playType[4] == 1)
		{
			m_Greater2CanStart = 1;
			LLOG_DEBUG("MHSetGreater2CanStartGame m_Greater2CanStart=[%d] ", m_Greater2CanStart);
		}
		else
		{
			m_Greater2CanStart = 0;
			LLOG_DEBUG("MHSetGreater2CanStartGame m_Greater2CanStart=[%d] ", m_Greater2CanStart);
		}
		break;
	}
	default:
	{
		m_Greater2CanStart = can;
		break;
	}
	}

}

//游戏开始前，检查用户准备
void Desk::check_user_ready_before_game_begin(LTime &currTime)
{
	if (!m_vip || m_vip->m_curCircle != 0 || (m_vip && m_vip->m_curCircle == 0 && m_deskState != DESK_WAIT))return;
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i] == NULL)continue;
		if (m_user[i] && m_readyState[i] == 1)continue;
		if (m_user[i] && m_user[i]->IsInRoomFor15Second())
		{
			LMsgC2SUserReady msg;
			msg.m_pos = i;
			this->HanderUserReady(m_user[i], &msg);
		}
	}
}

void Desk::check_user_ready(LTime &currTime)
{
	//LLOG_DEBUG("desk:%d check_user_ready",  m_id);
	switch (m_state)
	{
	//牛牛每局自动准备
	case NiuNiu:
	{	
		//游戏开始检查
		check_user_ready_before_game_begin(currTime);

		if (m_resetTime > 0 && m_resetUserId > 0)return;
		if (m_playtype.size() >= 4 && m_playtype[3] != 1) return;
		if (m_deskState != DESK_WAIT) return;		
		if (m_vip == NULL || m_vip->m_curCircle == 0 || m_vip->m_curCircle > m_vip->m_maxCircle) return;	 
		if (!m_flush_round_end_time) return;		

		if (currTime.Secs() >= m_round_end_time.Secs() + NIUNIU_READY_DELAY_TIME)
		{
			LLOG_DEBUG("desk:%d check_user_ready NiuNiu TIME OVER", m_id);
			for (Lint i = 0; i < m_iPlayerCapacity; i++)
			{
				if (m_deskState == DESK_WAIT && m_user[i] && m_readyState[i] != 1)
				{
					LMsgC2SUserReady  msg;
					msg.m_pos = i;
					HanderUserReady(m_user[i], &msg);
				}
			}
			m_flush_round_end_time = false;
		}
		break;
	}

	case NiuNiu_New:
	{
		if (m_finish_first_opt == 0 && m_deskState == DESK_WAIT)
		{
			if (m_playtype.size() >= 24 && m_playtype[23] == 1)
			{
				//公牌牛牛
				if (m_playtype[0] == 5)
				{
					m_next_round_delay_time = NEWNIUNIU_READY_DELAY_TIME_FAST + 3;
				}
				else
				{
					m_next_round_delay_time = NEWNIUNIU_READY_DELAY_TIME_FAST;
				}
			}
			else
			{
				m_next_round_delay_time = NEWNIUNIU_READY_DELAY_TIME;
			}
		}

		//游戏开始检查
		check_user_ready_before_game_begin(currTime);

		if (m_resetTime > 0 && m_resetUserId > 0)return;
		if (m_deskState != DESK_WAIT) return;
		if (m_vip == NULL || m_vip->m_curCircle == 0 || m_vip->m_curCircle > m_vip->m_maxCircle) return;
		if (!m_flush_round_end_time) return;

		if (currTime.Secs() >= m_round_end_time.Secs() + m_next_round_delay_time)
		{
			LLOG_DEBUG("desk:%d check_user_ready NiuNiu_New TIME OVER", m_id);
			for (Lint i = 0; i < m_iPlayerCapacity; i++)
			{
				if (m_deskState == DESK_WAIT && m_user[i] && m_readyState[i] != 1)
				{
					LMsgC2SUserReady  msg;
					msg.m_pos = i;
					HanderUserReady(m_user[i], &msg);
				}
			}
			m_flush_round_end_time = false;
		}
		break;
	}

	case TuiTongZi:
	{
		if (m_playtype.size() >= 3 && m_playtype[2] != 1)return;
		if (m_deskState != DESK_WAIT)return;
		if (m_vip == NULL)return;
		if (!m_vip || m_vip->m_curCircle == 0 || m_vip->m_curCircle > m_vip->m_maxCircle)return;
		if (!m_flush_round_end_time)return;

		if (currTime.Secs() >= m_round_end_time.Secs() + TUITONGZI_READY_DELAY_TIME)
		{
			LLOG_DEBUG("check_user_ready tuitongzi TIME OVER");
			for (Lint i = 0; i < m_iPlayerCapacity; i++)
			{
				//if (m_deskState == DESK_WAIT && GetUserCount() == m_iPlayerCapacity&& m_user[i] && m_readyState[i] != 1)
				if (m_deskState == DESK_WAIT && m_user[i] && m_readyState[i] != 1)
				{
					LMsgC2SUserReady  msg;
					msg.m_pos = i;
					HanderUserReady(m_user[i], &msg);
				}
			}
			m_flush_round_end_time = false;
		}

		break;
	}
	case YingSanZhang:
	{
		if (m_vip == NULL)return;
		//游戏开始检查
		check_user_ready_before_game_begin(currTime);
		if (!m_vip || m_vip->m_curCircle == 0 || m_vip->m_curCircle > m_vip->m_maxCircle)return;
		
		//爽翻模式
		if (m_next_draw_lookon)
		{
			MHHanderStartGameSanZhangPai(currTime);
			return;
		}

		check_auto_start_game(currTime);
		if (m_playtype.size() < 6)return;
		if (m_playtype.size() >= 6 && m_playtype[5] != 1)return;
		if (m_deskState != DESK_WAIT)return;
		if (!m_flush_round_end_time)return;
		if (  (m_yingsanzhang_finish_flag==0 && currTime.Secs() >= m_round_end_time.Secs() + YINGSANZHANG_READY_DELAY_TIME+m_yingsanzhang_compare_count*5.2 +1) || (m_yingsanzhang_finish_flag==1 && currTime.Secs() >= m_round_end_time.Secs() + YINGSANZHANG_READY_DELAY_TIME +4)  )
		{
			LLOG_DEBUG("check_user_ready yingsanzhang TIME OVER  m_yingsanzhang_compare_count=[%d]", m_yingsanzhang_compare_count);
			for (Lint i = 0; i < m_iPlayerCapacity; i++)
			{
				if (m_deskState == DESK_WAIT && m_user[i] && m_readyState[i] != 1)
				{
					LMsgC2SUserReady  msg;
					msg.m_pos = i;
					HanderUserReady(m_user[i], &msg);
				}
			}
			m_flush_round_end_time = false;
		}
		break;
	}
	//扯炫每局自动准备
	case CheXuan:
	{
		//游戏开始检查
		check_user_ready_before_game_begin(currTime);

		if (m_resetTime > 0 && m_resetUserId > 0)return;
		//if (m_playtype.size() >= 8 && m_playtype[3] != 1) return;
		if (m_deskState != DESK_WAIT) return;
		if (m_vip == NULL || m_vip->m_curCircle == 0 || m_vip->m_curCircle > m_vip->m_maxCircle) return;
		if (!m_flush_round_end_time) return;

		Lint t_delayTime = (m_playtype.size() >= 17 && m_playtype[16] == 1) ? CHEXUAN_GAME_FAST_START_TIME : CHEXUAN_GAME_START_TIME;
		if (currTime.Secs() >= m_round_end_time.Secs() + t_delayTime + m_next_delay_time)
		{
			LLOG_DEBUG("desk:%d check_user_ready CheXuan TIME OVER", m_id);
			for (Lint i = 0; i < m_iPlayerCapacity; i++)
			{
				if (m_deskState == DESK_WAIT && m_user[i] && m_readyState[i] != 1)
				{
					LMsgC2SUserReady  msg;
					msg.m_pos = i;
					HanderUserReady(m_user[i], &msg);
				}
			}
			m_flush_round_end_time = false;
		}
		break;
	}
	default:
		break;
	}

}


Lint   Desk::getDrawType(Lint game_state, Lint drawCount)
{
	switch (game_state)
	{
	case YingSanZhang:
	{
		if (drawCount == 10)
			return CARD_TYPE_8;
		else if (drawCount == 20)
			return  CARD_TYPE_16;
		else if (drawCount == 16)
			return CARD_TYPE_16;

		break;
	}
	case TuiTongZi:
	{
		if (drawCount == 10)
			return CARD_TYPE_8;
		else if (drawCount == 20)
			return  CARD_TYPE_16;

		break;
	}

	case NiuNiu:
	{
		if (drawCount == 10)
			return CARD_TYPE_8;
		else if (drawCount == 20)
			return  CARD_TYPE_16;

		break;
	}

	case NiuNiu_New:
	{
		if (drawCount == 10)
			return CARD_TYPE_8;
		else if (drawCount == 20)
			return  CARD_TYPE_16;

		break;
	}

	case ShuangSheng:
	{
		if (drawCount == 1)  //6局
			return CARD_TYPE_8;
		else if (drawCount == 2)  //12局
			return  CARD_TYPE_16;

		break;
	}
	}
	return CARD_TYPE_8;
}

bool  Desk::MHDismissDeskOnPlay()
{
	ResetEnd();
	return true;
}

bool  Desk::MHIsDeskMatchBegin()
{
	if (!this->GetVip())
	{
		return false;
	}

	//局数不为零 || 局数为零且DESK_PLAY || 房间为解散状态
	if (this->GetVip()->m_curCircle > 0 || 
		(this->GetVip()->m_curCircle == 0 && this->m_deskState == DESK_PLAY) ||
		this->m_resetTime != 0)
	{
		return true;
	}
	return false;
}
//检测玩法是否支持观战，分两进入房间
bool  Desk::MHIsAllowLookOnMode(int playState)
{
	switch (playState)
	{
	case NiuNiu:
	case NiuNiu_New:
	case SanDaEr:
	case WuRenBaiFen:
	case CheXuan:
	{
		return true;
	}
	default:
	{
		return false;
	}
	}
}


void  Desk::fillJoinRoomCheck(LMsgS2CJoinRoomCheck &data,Lint userId)
{
	LLOG_DEBUG("Desk::fillJoinRoomCheck");
	if (m_vip)
	{
		data.m_curCircle = m_vip->m_curCircle;
		data.m_maxCircle = m_vip->m_maxCircle;
		data.m_state = this->m_state;
		data.m_feeType = m_feeType;
		data.m_state = this->m_state;
		if (data.m_feeType == 0 && userId != m_creatUserId)
			data.m_costFee = 0;
		else if (data.m_feeType == 0 && userId == m_creatUserId)
			data.m_costFee = m_cost;
		else
			data.m_costFee = m_even;

		for (Lint i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_user[i])
			{
				UserNikeAndPhoto tempUser;
				tempUser.m_id = m_user[i]->GetUserDataId();
				tempUser.m_nike = m_user[i]->m_userData.m_nike;
				tempUser.m_headUrl = m_user[i]->m_userData.m_headImageUrl;
				data.m_roomUser.push_back(tempUser);
			}
		}

		data.m_roomUserCount = data.m_roomUser.size();
	}
}

void Desk::set_ysz_compare_count(Lint flag, Lint count)
{
	if (flag == 1)
		m_yingsanzhang_finish_flag = 1;
	else
	{
		m_yingsanzhang_finish_flag = 0;
		m_yingsanzhang_compare_count = count;

	}
}

void Desk::setDynamicToPlay()
{
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i])
		{
			m_playStatus[i] = 1; //设置玩家为参数游戏状态
		}
	}
}

void Desk::check_auto_start_game(LTime &currTime)
{
	LLOG_DEBUG("check_auto_start_game");
	if (m_auto_start_game_time_flag!=1)return;
	if (m_deskState != DESK_WAIT)return;
	if (m_vip&&m_vip->m_curCircle != 0)return;

	LLOG_DEBUG("check_auto_start_game  .............................");
	if (currTime.Secs() - m_auto_start_game_time.Secs() >= AUTO_START_GAME_DELAY_TIME)
	{
		LLOG_DEBUG("check_auto_start_game    AUTO_START_GAME_DELAY_TIME");
		m_auto_start_game_time_flag = 2;
		m_StartGameButtonPos = INVAILD_POS_QIPAI;
		m_startButtonAppear = 1;
		MHHanderStartGame();
	}

}

void Desk::HanderUserRequestGPS(User* pUser, LMsgC2SRequestUserGps* msg)
{
	LLOG_DEBUG("Logwyz Desk::HanderUserRequestGPS ");
	if (pUser == NULL || msg == NULL)return;
	
	if (!MHIsRoomUser(pUser))
	{
		LLOG_ERROR("Desk::HanderUserRequestGPS   user[%d] not in room", pUser->GetUserDataId());
		return;
	}

	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_user[i] == NULL)continue;
		if (m_user[i]->GetUserDataId() == msg->m_userId)
		{
			LLOG_DEBUG("HanderUserRequestGPS  user[%d] gpsString[%s]",msg->m_userId, m_user[i]->m_userData.m_customString2.c_str());
			double  youLat = 0.0, youLng = 0.0;
			get_gps_pair_values(m_user[i]->m_userData.m_customString2, youLat, youLng);

			LMsgS2CRequestUserGps send;
			send.m_errorCode = 0;
			send.m_userId = msg->m_userId;
			send.m_Gps_Lat = std::to_string(youLat);
			send.m_Gps_Lng = std::to_string(youLng);

			pUser->Send(send);
			return;
		}
	}
}

//准备人数  ;
Lint Desk::MHGetUserReadyCount()
{
	Lint cnt = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_readyState[i])
		{
			cnt += 1;
		}
	}

	return cnt;
}

void Desk::set_some_spec(Lint state, const std::vector<Lint>& playtype)
{
	switch (state)
	{
	case YingSanZhang:
	{
		//中途旁观功能
		if (playtype.size() >= 15 && 1 == playtype[13] && 1 == playtype[14])
		{
			m_next_draw_lookon = true;
		}
		break;
	}
	default:
	{
		break;
	}

	}
}


/*
五人百分：交牌
*/
void Desk::HandleWuRenBaiFenZhuangJiaoPai(User* pUser, WuRenBaiFenC2SZhuangJiaoPai * msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenZhuangJiaoPai");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenZhuangJiaoPai  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenZhuangJiaoPai state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenZhuangRequestJiaoPai(pUser, msg);
	}
}
void Desk::HandleWuRenBaiFenXianAckJiaoPai(User* pUser, WuRenBaiFenC2SXianSelectJiaoPai * msg)
{
	LLOG_DEBUG("Desk::HandleWuRenBaiFenXianAckJiaoPai");
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenXianAckJiaoPai  pUser or msg ==NULL");
		return;
	}
	if (m_deskState != DESK_PLAY)
	{
		LLOG_ERROR("Desk::HandleWuRenBaiFenXianAckJiaoPai state error %d ,m_deskState = %d ", pUser->m_userData.m_id, m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HandleWuRenBaiFenXianSelectJiaoPai(pUser, msg);
	}
}
