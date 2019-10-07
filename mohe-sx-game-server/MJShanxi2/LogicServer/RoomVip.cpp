#include <openssl\md5.h>
#include "RoomVip.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"
#include "RobotManager.h"
#include "RLogHttp.h"
#include "../mhmessage/mhmsghead.h"


enum {
	FEAT_STARTUP_SCORE = 0, // 每轮初始给1000分
};


VipLogItem::VipLogItem()
{
	m_desk = NULL;
}

VipLogItem::~VipLogItem()
{

}

Lint VipLogItem::GetOwerId()
{
	return m_posUserId[0];
}

void VipLogItem::AddLog(User** user, Lint* gold, HuInfo* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting)
{
	VipDeskLog* log = new VipDeskLog();
	log->m_videoId = videoId;
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_checkTing[i] += ting[i];
		if (huinfo[i].type == WIN_SUB_ZIMO)
		{
			log->m_zimo[i] += 1;
		}
		else if (huinfo[i].type == WIN_SUB_BOMB)
		{
			log->m_win[i] += 1;
			if (huinfo[i].abombpos != INVAILD_POS)
			{
				log->m_bomb[huinfo[i].abombpos] += 1;	// 放炮者次数累加
			}
		}
		else if (huinfo[i].type == WIN_SUB_DZIMO)
		{
			log->m_dzimo[i] += 1;
		}
		else if (huinfo[i].type == WIN_SUB_DBOMB)
		{
			log->m_dwin[i] += 1;
			if (huinfo[i].abombpos != INVAILD_POS)
			{
				log->m_dbomb[huinfo[i].abombpos] += 1;	// 放炮者次数累加
			}
		}
	}
	m_log.push_back(log);

	// 这个断点：局结束的时候，还没刷新客户端，第一次断的时候m_curCircleReal为0，m_curCircleReal仅在该点递增
	m_curCircleReal += 1;
	//m_curCircle = m_curCircle; // 断点查看用
	m_curZhuangPos = zhuangPos;

	if (m_curCircleReal == 1)
	{
		InsertToDb();
		if (m_reset == 0)
		{
			this->CreateTableId();
			this->SaveDeskInfoToDB();
		}

	}
	else
	{
		UpdateToDb();
		this->UpdateDeskTotalScoreToDB();
	}

	if (m_reset == 0)
	{
		LTime curTime;
		for (int i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_posUserId[i] > 0)
			{
				//保存每个用户该局的成绩			 	 
				char buf[256];
				sprintf_s(buf, "%08d%d", m_posUserId[i], curTime.Secs());
				Lstring str_user_draw_id(buf);
				int is_win = (huinfo[i].type == 1 || huinfo[i].type == 2) ? 1 : 0;
				this->InsertUserScoreToDB(i, str_user_draw_id, is_win, gold[i], huinfo[i].hu_types);
				this->m_mapUserIdToDrawId[m_posUserId[i]] = str_user_draw_id;
			}
		}
	}

	if (m_reset) //房间解散
	{
		if (m_desk && m_desk->m_bIsDissmissed && m_curCircle == 1) //第1局中间解散
		{		 
			//此时不保存大赢家数据
			m_desk->m_bIsDissmissed = false;
		}
		else
		{
			//保存大赢家的数据到数据库
			this->InsertDeskWinnerInfo();
		}
	 
	}
	else if (this->isNormalEnd()) //打满场次结束
	{
		//保存大赢家的数据到数据库
		this->InsertDeskWinnerInfo();
	}

	//测试代码
	//this->InsertDeskWinnerInfo();

	//每圈存上玩家圈数统计和输赢状况
	/*
	for (Lint i = 0; i < m_iPlayerCapacity; i++)
	{
	if (user[i] != NULL)
	{
	user[i]->m_userData.m_customInt[0]++;
	LMsgL2LMGAddUserCircleCount msg;
	msg.m_userid = user[i]->m_userData.m_id;
	msg.m_strUUID = user[i]->m_userData.m_unioid;
	gWork.SendToLogicManager(msg);
	}
	}
	*/
	// 测试用代码
	/*
	if (user[0])
	{
		//代理满88场活动
		MHLMsgL2LMGNewAgencyActivityUpdatePlayCount msgAgency;
		msgAgency.m_openID = user[0]->m_userData.m_openid;
		msgAgency.m_userid = user[0]->GetUserDataId();
		msgAgency.m_deskId = this->m_deskId;
		for (int i = 0; i < m_iPlayerCapacity; i++)
		{
			msgAgency.m_playerId[i] = 454;// m_posUserId[i];
		}
		msgAgency.m_strUUID = user[0]->m_userData.m_unioid;
		gWork.SendToLogicManager(msgAgency);
	}
	*/
	//第八局存上圈数
	// 连庄不加局数也没问题，12345677777778 只有最后一个8
	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; i++)
		{
			if (user[i] != NULL)
			{
				user[i]->m_userData.m_totalplaynum++;
				LMsgL2LMGAddUserPlayCount msg;
				msg.m_userid = user[i]->m_userData.m_id;
				msg.m_strUUID = user[i]->m_userData.m_unioid;
				gWork.SendToLogicManager(msg);
			}
		}

		if (user[0]&& m_reset==0)
		{
			//代理满88场活动
			//MHLMsgL2LMGNewAgencyActivityUpdatePlayCount msgAgency;
			//msgAgency.m_openID = user[0]->m_userData.m_openid;
			//msgAgency.m_userid = user[0]->GetUserDataId();
			//msgAgency.m_deskId = this->m_deskId;
			//for (int i = 0; i < m_iPlayerCapacity; i++)
			//{
			//	msgAgency.m_playerId[i] = m_posUserId[i];
			//}
			//msgAgency.m_strUUID = user[0]->m_userData.m_unioid;
			//gWork.SendToLogicManager(msgAgency);

			MHLMsgL2LMGNewAgencyActivityUpdatePlayCount msgAgency;
			msgAgency.m_openID = user[0]->m_userData.m_openid;
			msgAgency.m_userid = user[0]->GetUserDataId();
			msgAgency.m_deskId = this->m_deskId;
			if (this->m_desk&&this->m_desk->m_clubInfo.m_clubId != 0 && this->m_desk->m_clubInfo.m_showDeskId != 0)
				msgAgency.m_deskId = this->m_desk->m_clubInfo.m_showDeskId;
			msgAgency.m_playType = m_state;
			msgAgency.m_flag = 1;
			msgAgency.m_playerCount = m_iPlayerCapacity;

			for (int i = 0; i<m_iPlayerCapacity; i++)
			{
				msgAgency.m_playerId[i] = m_posUserId[i];
				msgAgency.m_score[i] = m_score[i];
			}
			msgAgency.m_strUUID = user[0]->m_userData.m_unioid;

			msgAgency.m_maxCircle = m_maxCircle;
			if (m_desk)
			{
				//char tempTime[32 + 1] = {};
				//strftime(tempTime, sizeof(tempTime) - 1, "%Y-%m-%d %H:%M", localtime(&(m_desk->m_gameStartTime)));
				msgAgency.m_startTime = m_desk->m_gameStartTime;//std::string(tempTime);
			}

			gWork.SendToLogicManager(msgAgency);

		}
	}

	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}

void VipLogItem::AddLog(User** user, Lint* gold, std::vector<HuInfo>* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting)
{
	VipDeskLog* log = new VipDeskLog();
	log->m_videoId = videoId;
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_checkTing[i] += ting[i];

		for (size_t k =0; k < huinfo[i].size(); ++k)
		{
			if(huinfo[i][k].type == WIN_SUB_ZIMO)
			{
				log->m_zimo[i] += 1;
			}
			else if(huinfo[i][k].type == WIN_SUB_BOMB)
			{
				log->m_win[i] += 1;
				if ( huinfo[i][k].abombpos != INVAILD_POS )
				{
					log->m_bomb[huinfo[i][k].abombpos] += 1;	// 放炮者次数累加
				}
			}
			else if(huinfo[i][k].type == WIN_SUB_DZIMO)
			{
				log->m_dzimo[i] += 1;
			}
			else if(huinfo[i][k].type == WIN_SUB_DBOMB)
			{
				log->m_dwin[i] += 1;
				if ( huinfo[i][k].abombpos != INVAILD_POS )
				{
					log->m_dbomb[huinfo[i][k].abombpos] += 1;	// 放炮者次数累加
				}
			}
		}
	}
	m_log.push_back(log);
	
	// 这个断点：局结束的时候，还没刷新客户端，第一次断的时候m_curCircleReal为0，m_curCircleReal仅在该点递增
	m_curCircleReal += 1;
	//m_curCircle = m_curCircle; // 断点查看用
	m_curZhuangPos = zhuangPos;

	if(m_curCircleReal == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	// 连庄不加局数也没问题，12345677777778 只有最后一个8
	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; i ++)
		{
			if (user[i] != NULL)
			{
				user[i]->m_userData.m_totalplaynum ++;
				LMsgL2LMGAddUserPlayCount msg;
				msg.m_userid = user[i]->m_userData.m_id;
				msg.m_strUUID = user[i]->m_userData.m_unioid;
				gWork.SendToLogicManager(msg);
			}
		}
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}

bool VipLogItem::ExiestUser(Lint id)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (id == m_posUserId[i])
			return true;
	}
	return false;
}

void VipLogItem::InsertToDb()
{
	std::stringstream ss;
	ss << "INSERT INTO";
	//if ( m_state > 1000 )	// > 1000 存放到金币表中
	//{
	//	ss << " logcoins ";
	//}
	//else
	//{
	//	ss << " log ";
	//}
	ss << " log ";
	ss << "(Id,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,Reset,Data,PlayType) VALUES (";
	ss << "'" << m_id << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_posUserId[0]<< "',";
	ss << "'" << m_posUserId[1] << "',";
	ss << "'" << m_posUserId[2] << "',";
	ss << "'" << m_posUserId[3] << "',";
	ss << "'" << m_state << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_secret << "',";
	ss << "'" << m_score[0] << "',";
	ss << "'" << m_score[1] << "',";
	ss << "'" << m_score[2] << "',";
	ss << "'" << m_score[3] << "',";
	ss << "'" << m_reset << "',";
	ss << "'" << ToString() << "',";
	ss << "'" << PlayTypeToStrint() << "')";

	LMsgL2LDBSaveLogItem send;

	send.m_userId = m_posUserId[0];
	send.m_type = 0;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsBegin()
{
	return m_curCircle != 0;
}

void VipLogItem::RemoveUser(Lint id)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (id == m_posUserId[i])
			m_posUserId[i] = 0;
	}
}

void VipLogItem::UpdateToDb()
{
	std::stringstream ss;
	ss << "UPDATE";
	//if ( m_state > 1000 )	// > 1000 存放到金币表中
	//{
	//	ss << " logcoins ";
	//}
	//else
	//{
	//	ss << " log ";
	//}
	ss << " log ";
	ss << "SET CurCircle=";
	ss << "'" << m_curCircle << "',Pos1=";
	ss << "'" << m_posUserId[0] << "',Pos2=";
	ss << "'" << m_posUserId[1] << "',Pos3=";
	ss << "'" << m_posUserId[2] << "',Pos4=";
	ss << "'" << m_posUserId[3] << "',Score1=";
	ss << "'" << m_score[0] << "',Score2=";
	ss << "'" << m_score[1] << "',Score3=";
	ss << "'" << m_score[2] << "',Score4=";
	ss << "'" << m_score[3] << "',Reset=";
	ss << "'" << m_reset << "',Pass=";
	ss << "'" << m_secret << "',Data=";
	ss << "'" << ToString()  << "' WHERE Id=";
	ss << "'" << m_id << "'";

	LMsgL2LDBSaveLogItem send;

	send.m_userId = m_posUserId[0];
	send.m_type = 1;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsFull(User* user)
{
	Lint count = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_posUserId[i] == 0 || user->GetUserDataId() == m_posUserId[i])
			return false;
	}

	return true;
}

void VipLogItem::SendInfo()
{
	LMsgS2CVipInfo info;
	info.m_curCircle = m_curCircle ;
	info.m_curMaxCircle = m_maxCircle ;
	info.m_playtype = m_playtype;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		User* user = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user)
			user->Send(info);
	}
}

void VipLogItem::SendEnd()
{
	LLOG_DEBUG("VipLogItem::SendEnd()");
 	LMsgS2CVipEnd msg;
 	User* user[DESK_USER_COUNT] = { NULL };
 	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
 	{
 		user[i] = gUserManager.GetUserbyDataId(m_posUserId[i]);
 		if (user[i])
 		{
 			msg.m_nike[i] = user[i]->m_userData.m_nike;
 		}
		msg.m_id[i] = m_posUserId[i];
 	}
	LTime cur;
	Llong time = cur.MSecs();
	memset(msg.m_gold,0,sizeof(msg.m_gold));
	memset(msg.m_zimo,0,sizeof(msg.m_zimo));
	memset(msg.m_win,0,sizeof(msg.m_win));
	memset(msg.m_bomb,0,sizeof(msg.m_bomb));
	memset(msg.m_dzimo,0,sizeof(msg.m_dzimo));
	memset(msg.m_dwin,0,sizeof(msg.m_dwin));
	memset(msg.m_dbomb,0,sizeof(msg.m_dbomb));
	memset(msg.m_agang,0,sizeof(msg.m_agang));
	memset(msg.m_mgang,0,sizeof(msg.m_mgang));
	memset(msg.m_CheckTing,0,sizeof(msg.m_CheckTing));

 	for (Lsize i = 0; i < m_log.size(); ++i)
 	{
 		for(Lint  j = 0; j < m_iPlayerCapacity; ++j)
 		{
 			msg.m_gold[j] += m_log[i]->m_gold[j];
			msg.m_mgang[j] += m_log[i]->m_mgang[j];
			msg.m_agang[j] += m_log[i]->m_angang[j];
			msg.m_CheckTing[j] += m_log[i]->m_checkTing[j];

			msg.m_zimo[j] += m_log[i]->m_zimo[j];
			msg.m_bomb[j] += m_log[i]->m_bomb[j];
			msg.m_win[j] += m_log[i]->m_win[j];
			msg.m_dzimo[j] += m_log[i]->m_dzimo[j];
			msg.m_dbomb[j] += m_log[i]->m_dbomb[j];
			msg.m_dwin[j] += m_log[i]->m_dwin[j];
 		}
 	}
	msg.m_time = time;
	msg.m_creatorId = m_desk->m_creatUserId;
	msg.m_creatorNike = m_desk->m_creatUserNike;
	msg.m_creatorHeadUrl = m_desk->m_creatUserUrl;
	LLOG_DEBUG("Logwyz .... wyz m_creatorId=[%d] m_creatorNike=[%s],m_creatorHeadUrl=[%s]", msg.m_creatorId, msg.m_creatorNike.c_str(), msg.m_creatorHeadUrl.c_str());
	//-111测试
	//msg.m_gold[0] = 111; msg.m_gold[1] = -111; msg.m_gold[2] = 110; msg.m_gold[3] = -110;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user[i])
		{
			LLOG_DEBUG("VipLogItem::SendEnd()  send userid [%d]",user[i]->GetUserDataId());
			user[i]->Send(msg);
		}
	}

	// 远程日志
	RLOG("bswf", LTimeTool::GetLocalTimeInString()
		<< "|" << 15001
		<< "|" << m_state
		<< "|" << PlayTypeToString4RLog()
		<< "|" << m_maxCircle
		<< "|" << m_curCircle
		<< "|" << m_reset
		<< "|" << m_posUserId[0]
		<< ";" << m_posUserId[1]
		<< ";" << m_posUserId[2]
		<< ";" << m_posUserId[3]
		<< "|" << gConfig.GetInsideIp()
		<< "|" << gConfig.GetInsidePort()
		<< "|" << gConfig.GetServerID()
		);
}

Lstring VipLogItem::PlayTypeToString4RLog()
{
	std::stringstream ss;
	if (!m_playtype.empty())
	{
		ss << m_playtype.front();
	}

	for (Lsize i = 1; i < m_playtype.size(); ++i)
	{
		ss << ";" << m_playtype[i];
	}
	return ss.str();
}

bool VipLogItem::isEnd()
{
	return m_maxCircle <= m_curCircle || m_reset == 1;
}

bool VipLogItem::isNormalEnd()
{
	return m_maxCircle == m_curCircle;
}

Lint VipLogItem::GetUserPos(User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return i;
	}

	return INVAILD_POS;
}

Lint VipLogItem::GetUserScore(User* user)
{
	Lint pos = GetUserPos(user);
	if (pos != INVAILD_POS)
	{
		return m_score[pos];
	}
	return 0;
}

bool VipLogItem::AddUser(User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return true;
		
		if (m_posUserId[i] == 0)
		{
			m_posUserId[i] = user->GetUserDataId();
			return true;
		}
	}

	return false;
}

bool  VipLogItem::ResetUser(User *user[] )
{

	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if(user[i])
			m_posUserId[i] = user[i]->GetUserDataId();
	}
	return true;
}

bool VipLogMgr::Init()
{
	m_id = 1;
	m_videoId = 1;
	return true;
}

bool VipLogMgr::Final()
{
	return true;
}

void VipLogMgr::SetVipId(Lint id)
{
	m_id = id;
}

Lstring VipLogMgr::GetVipId()
{
	++m_id;
	m_id = m_id % 5000;
	char mVipLogID[64] = { 0 };
	sprintf_s(mVipLogID, sizeof(mVipLogID)-1, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_id);
	return mVipLogID;
}

void VipLogMgr::SetVideoId(Lint id)
{
	m_videoId = id;
}

Lstring VipLogMgr::GetVideoId()
{
	++m_videoId;
	m_videoId = m_videoId % 50000;
	char mVipVideoID[64] = { 0 };
	sprintf_s(mVipVideoID, sizeof(mVipVideoID)-1, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_videoId);
	return mVipVideoID;
}

VipLogItem*	 VipLogMgr::GetNewLogItem(Lint card,Lint usrId)
{
	VipLogItem* item = new VipLogItem();
	item->m_id = GetVipId();
	item->m_maxCircle = card;
	item->m_time = gWork.GetCurTime().Secs();
	m_item[item->m_id]= item;
	return item;
}

VipLogItem*	VipLogMgr::GetLogItem(Lstring& logId)
{
	if (m_item.count(logId))
	{
		return m_item[logId];
	}

	return NULL;
}

VipLogItem*	VipLogMgr::FindUserPlayingItem(Lint id)
{
	auto it = m_item.begin();
	for(; it != m_item.end(); ++it)
	{
		VipLogItem* item = it->second;
		if(!item->isEnd() && item->ExiestUser(id))
		{
			return item;
		}
	}
	return NULL;
}

void VipLogMgr::AddPlayingItem(VipLogItem* item)
{
	m_item[item->m_id] = item;
}

void VipLogMgr::Tick()
{
	auto it = m_item.begin();
	for (; it != m_item.end();)
	{
		if (it->second->isEnd())
		{
			LLOG_INFO("VipLogMgr::Tick vip end %s",it->first.c_str());
			delete it->second;
			m_item.erase(it++);
		}
		else
		{
			it++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Add New log table
void VipLogItem::CreateTableId() // 创建表Id
{
	char buf[256];
	sprintf_s(buf, "%d%d%d", gConfig.GetServerID(), m_desk->GetDeskId(), LTime().Secs());
	m_tableId=buf;
	LLOG_DEBUG("***generate table id: %s", buf);
}

void VipLogItem::SaveDeskInfoToDB()      // 存储到数据库
{
	if (!m_desk||m_tableId.empty()||m_id.empty())
	{
		LLOG_ERROR("erorr desk state: %d: %s: %s", m_deskId, m_tableId.c_str(), m_id.c_str());
	}
	std::stringstream ss;
	ss<<"INSERT INTO playinfo ";
	ss<<"(Id, LogId, ServerId, ClubId, ClubPlayTypeId, DeskId, DeskIsDymic, CreateUserId, DeskFlag, DeskPlayType, CurCircle, MaxCircle, IsDismiss, DismissUserId, DeskFeeType, DeskFeeTotal, PlayerCount, Pos1, Pos2, Pos3, Pos4, WinnerCount, TotalScore1, TotalScore2, TotalScore3, TotalScore4, ctime, utime) VALUES (";
	ss<<"'"<<m_tableId<<"',";
	ss<<"'"<<m_id<<"',";
	ss<<"'"<<gConfig.GetServerID()<<"',";
	ss<<"'"<< m_desk->m_clubInfo.m_clubId <<"',";
	ss<<"'"<< m_desk->m_clubInfo.m_playTypeId<< "',";
	ss<<"'"<<m_deskId<<"',";
	ss<<"'"<< m_desk->m_Greater2CanStart <<"',";
	ss<<"'"<<m_desk->m_creatUserId<<"',";
	ss<<"'"<<m_desk->m_state<<"',";
	ss<<"'"<<PlayTypeToStrint()<<"',";
	ss << "'" << m_curCircle << "',";
	ss<<"'"<<m_maxCircle<<"',";
	ss<<"'"<<0<<"',";
	ss<<"'"<<0<<"',";
	ss<<"'"<<m_desk->m_feeType<<"',";
	ss<<"'"<<0<<"',";
	ss<<"'"<<m_desk->m_desk_user_count<<"',";
	ss << "'" << m_posUserId[0] << "',";
	ss << "'" << m_posUserId[1] << "',";
	ss << "'" << m_posUserId[2] << "',";
	ss << "'" << m_posUserId[3] << "',";
	ss << "'" << 0 << "',";
	ss << "'" << m_score[0] << "',";
	ss << "'" << m_score[1] << "',";
	ss << "'" << m_score[2] << "',";
	ss << "'" << m_score[3] << "',";
	ss<<"NOW(), NOW())";
	LMsgL2LDBSaveLogItem send;

	send.m_userId=m_posUserId[0];
	send.m_type=0;
	send.m_sql=ss.str();
	gWork.SendMsgToDb(send);
	LLOG_DEBUG("***Insert to playinfo: sql=%s", send.m_sql.c_str());

}

void VipLogItem::UpdateDeskTotalScoreToDB()    // 修改分数数据到数据库
{	
	std::stringstream ss;
	ss<<"UPDATE playinfo SET ";
	ss<<" IsDismiss = '"<<m_reset<<"',";
	ss<<" DismissUserId ='"<<(m_reset ? m_desk->m_resetUserId : 0)<<"',";
	ss << " CurCircle ='" << m_curCircle << "',";
	ss << " DismissUserId ='" << (m_reset ? m_desk->m_resetUserId : 0) << "',";
	ss<<" TotalScore1 = '"<< m_score[0] <<"',";
	ss<<" TotalScore2 = '"<< m_score[1] <<"',";
	ss<<" TotalScore3 = '"<< m_score[2] <<"',";
	ss<<" TotalScore4 = '"<< m_score[3] <<"' WHERE Id = '"<< m_tableId <<"'";

	LMsgL2LDBSaveLogItem send;
	send.m_userId=m_posUserId[0];
	send.m_type=1;
	send.m_sql=ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
	LLOG_DEBUG("**** Update total score to playinfo: sql=%s", send.m_sql.c_str());
}

void VipLogItem::UpdateDeskTotalFeeValueToDB(Lint nFeeValue)
{
	std::stringstream ss;
	ss<<"UPDATE playinfo";
	ss<<" SET DeskFeeTotal = '"<<nFeeValue<<"'";
	ss<<" WHERE Id = '"<<m_tableId<<"'";

	LMsgL2LDBSaveLogItem send;
	send.m_userId=m_posUserId[0];
	send.m_type=1;
	send.m_sql=ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
	LLOG_ERROR("***Update desk fee to playinfo: sql=%s", send.m_sql.c_str());
}

void VipLogItem::InsertUserScoreToDB(Lint pos, Lstring &drawId, Lint isWin, Lint score, std::vector<Lint> & srcPaiXing)
{
	if (pos < 0||pos > INVAILD_POS)
	{
		LLOG_ERROR("erorr pos: %d:pos:%d", m_deskId, pos);
		return;
	}
	if (!m_desk||m_tableId.empty()||m_id.empty())
	{
		LLOG_ERROR("erorr desk state: %d: %s: %s", m_deskId, m_tableId.c_str(), m_id.c_str());
		return;
	}

	std::vector<Lint> paixingList(srcPaiXing);
	if (isNormalEnd()&&(!m_reset))
	{
		if (m_score[pos]==-111)
		{
			srcPaiXing.push_back(WIN_SCORE_111_N);         //负111分
		}
		else if (m_score[pos]==111)
		{
			paixingList.push_back(WIN_SCORE_111_OP);     //正111分   
		}
	}
	if (m_JinAnGangCount[pos] > 0)
	{
		paixingList.push_back(WIN_ANGANG_JIN); //暗杠耗子
	}

	std::stringstream ss;
	ss<<"INSERT INTO drawinfo ";
	ss<<"(Id, PlayId, UserId, DeskId, ClubId, UserPos, CurCircle, Score, IsWinner, WinPaiXing1, WinPaiXingSpec, FeeValue, ctime ) VALUES (";
	ss<<"'"<<drawId<<"',";
	ss<<"'"<<m_tableId<<"',";
	ss<<"'"<<m_posUserId[pos]<<"',";
	ss<<"'"<<m_deskId<<"',";
	ss<<"'"<< (m_desk ? m_desk->m_clubInfo.m_clubId : 0) <<"',";
	ss<<"'"<<pos<<"',";
	ss<<"'"<<m_curCircleReal<<"',";
	ss<<"'"<<score<<"',";
	ss<<"'"<<isWin<<"',";
	ss<<"'"<<gPaiXingMgr.GetSpecPaiXingString(isWin, m_state, paixingList, false)<<"',";
	ss<<"'"<<gPaiXingMgr.GetSpecPaiXingString(isWin, m_state, paixingList, true)<<"',";
	ss<<"'"<<0<<"',";
	ss<<"NOW())";


	LMsgL2LDBSaveLogItem send;
	send.m_userId=m_posUserId[0];
	send.m_type=0;
	send.m_sql=ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
	LLOG_ERROR("***Insert to drawinfo: sql=%s", send.m_sql.c_str());

}

void VipLogItem::UpdateUserFeeValueToDB(Lint userId, Lint feeValue)
{
	std::map<Lint, Lstring>::iterator it=m_mapUserIdToDrawId.find(userId);
	if (it==m_mapUserIdToDrawId.end())
	{
		return;
	}

	std::stringstream ss;
	ss<<"UPDATE drawinfo ";
	ss<<" SET FeeValue = '"<<feeValue<<"'";
	ss<<" WHERE Id = '"<<m_mapUserIdToDrawId[userId]<<"'";

	LMsgL2LDBSaveLogItem send;
	send.m_userId=m_posUserId[0];
	send.m_type=1;
	send.m_sql=ss.str();

	gWork.SendMsgToDb(send);
	LLOG_ERROR("*****Update user fee to drawinfo: sql=%s", send.m_sql.c_str());
}

// 计算改场大赢家，保存到数据库
void VipLogItem::InsertDeskWinnerInfo()
{
	std::vector<MHUserScore> score_vec(4);
	std::vector<MHUserScore> win_user_vec(4);
	score_vec.clear();
	win_user_vec.clear();

	for (int i = 0; i < this->m_iPlayerCapacity; i++)
	{
		if (m_posUserId[i] > 0)
		{
			score_vec.push_back(MHUserScore(i, m_posUserId[i], m_score[i]));
		}
	}
	std::sort(score_vec.begin(), score_vec.end());
	int  size = (int)score_vec.size();
	for (int i = size - 1; i > 0; i--)
	{
		if (true) // 不判断0了，每个桌子上只随机选择1个
		{
			win_user_vec.push_back(score_vec[i]);
			if (score_vec[i] > score_vec[i - 1]) // 前面位置
			{
				break;
			}
		}
	}

	char record_id[48];
	LTime curTime;
	LLOG_DEBUG("***** Desk:%d  winner_count:%d", m_deskId, win_user_vec.size());

	LMsgL2LDBSaveLogItem send;
	std::stringstream ss;
	ss << "UPDATE playinfo SET ";
	ss << " WinnerCount = '" << 1 << "',";
	ss << " utime = NOW() ";
	ss << " WHERE Id = '" << m_tableId << "'";
	send.m_userId = m_posUserId[0];
	send.m_type = 1;
	send.m_sql = ss.str();
	gWork.SendMsgToDb(send);
	LLOG_DEBUG("***Update winner count: sql: 【%s】", send.m_sql.c_str());

	int nIndex = 0;
	// 随机选择一个玩家为大赢家
	if (win_user_vec.size() > 0)
	{
		nIndex = curTime.Secs() % win_user_vec.size();
	}
	if (nIndex < win_user_vec.size())
	{
		std::stringstream ss;
		sprintf_s(record_id, 48, "%08d%d", win_user_vec[nIndex].m_userId, curTime.Secs());
		ss << "INSERT INTO winnerinfo ";
		ss << "(Id, PlayId, UserId, ClubId, Score, ctime, utime) VALUES (";
		ss << "'" << Lstring(record_id) << "',";
		ss << "'" << m_tableId << "',";
		ss << "'" << win_user_vec[nIndex].m_userId << "',";
		ss << "'" << (m_desk ? m_desk->m_clubInfo.m_clubId : 0) << "',";
		ss << "'" << win_user_vec[nIndex].m_score << "',";
		ss << "" << "NOW()" << ",";
		ss << "NOW())";

		LMsgL2LDBSaveLogItem send;
		send.m_userId = win_user_vec[nIndex].m_userId;
		send.m_type = 0;
		send.m_sql = ss.str();
		gWork.SendMsgToDb(send);
		LLOG_DEBUG("***Insert into winnerinfo: sql: 【%s】", send.m_sql.c_str());
	}

}

//////////////////////////////////////////////////////////////////////////////////


bool RoomVip::Init()
{

	return true;
}

bool RoomVip::Final()
{
	return true;
}

void RoomVip::Tick(LTime& cur)
{
	DeskMap::iterator it = m_deskMap.begin();
	for (; it != m_deskMap.end(); ++it)
	{
		it->second->Tick(cur);
	}
}

Desk* RoomVip::GetFreeDesk(Lint nDeskID, GameType gameType)
{
	Desk* ret = NULL;
	
	LLOG_DEBUG("RoomVip::m_deskMap size=%d", m_deskMap.size());

	DeskMap::iterator it = m_deskMap.begin();
	for (; it != m_deskMap.end(); ++it)
	{
		if (it->second->getDeskState() == DESK_FREE)
		{
			ret = it->second;
			m_deskMap.erase(it);
			break;
		}
	}
	
	if(ret == NULL)
	{
		ret = new Desk();
	}

	if(!ret->initDesk(nDeskID, gameType))
	{
		delete ret;
		ret = NULL;
		LLOG_ERROR("RoomVip::GetFreeDesk -- Fail to init desk");
	}
	else
	{
		m_deskMap[nDeskID] = ret;
		ret->SetDeskWait();
		LLOG_ERROR("RoomVip::GetFreeDesk size=%d", m_deskMap.size());
	}
	
	return ret;
}

Desk* RoomVip::GetDeskById(Lint id)
{
	if(m_deskMap.count(id))
	{
		return m_deskMap[id];
	}

	return NULL;
}

Lint RoomVip::CreateVipDesk(LMsgLMG2LCreateDesk* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		return -1;
	}
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;

	LLOG_INFO("Logwyz.................RoomVip::CreateVipDesk   pMsg->m_Greater2CanStart=[%d] pMsg->m_GpsLimit:%d",pMsg->m_Greater2CanStart, pMsg->m_GpsLimit);
	//if(pMsg->m_playType.empty())
	//{
	//	LLOG_ERROR("RoomVip::CreateVipDesk play type is empty, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
	//	ret.m_errorCode = 5;
	//	pUser->Send(ret);
	//	return ret.m_errorCode;
	//}

	MHLOG("********************* RoomVip::CreateVipDesk(LMsgLMG2LCreateDesk* pMsg, User* pUser)");
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_gateId %d", pMsg->m_gateId);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_flag %d", pMsg->m_flag);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_gold %d", pMsg->m_gold);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_strUUID %s", pMsg->m_strUUID.c_str());
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_userid %d", pMsg->m_userid);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_robotNum %d", pMsg->m_robotNum);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg-m_playType.size() = %d", pMsg->m_playType.size());;
	for (int i = 0; i < pMsg->m_playType.size(); i++)
	{
		MHLOG("********************* LMsgLMG2LCreateDesk i = %d pMsg-m_playType[i] = %d", i, pMsg->m_playType[i]);;
	}

	if (pMsg->m_flag < CARD_TYPE_4 || pMsg->m_flag > CARD_TYPE_16)
	{
		pMsg->m_flag = CARD_TYPE_8;
	}

	Lint itemId = 0;
	Lint circle = 0;
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
#ifndef _MH_DEBUG
		circle = 8;
#else
		circle = 1;   //for test 局数相关
#endif
		break;
	case CARD_TYPE_8: 
#ifndef _MH_DEBUG
		circle = 16;   //for test 局数相关
#else
		circle = 2;
#endif
		break;
	case CARD_TYPE_16:
		circle = 16;
		break;
	}
	
	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk in desk, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk deskID is in Use, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//创建桌子失败
	Desk* desk = GetFreeDesk(pMsg->m_deskID, (GameType)pMsg->m_state);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed! userid=%d deskid=%d gametype=%d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	LLOG_INFO("RoomVip::CreateVipDesk userid=%d deskid=%d gametype=%d  cost = %d even = %d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state, pMsg->m_cost, pMsg->m_even);

	desk->SetSelectSeat(true);  //设置目前处于选座状态
	desk->m_creatUser = pUser;
	desk->m_creatUserId = pUser->GetUserDataId();
	desk->m_creatUserNike = pUser->m_userData.m_nike;
	desk->m_creatUserUrl = pUser->m_userData.m_headImageUrl;
	desk->m_unioid = pUser->m_userData.m_unioid;
	desk->m_cheatAgainst = pMsg->m_cheatAgainst;
	desk->m_feeType = pMsg->m_feeType;
	desk->m_cellscore = pMsg->m_cellscore;
	desk->m_state = pMsg->m_state;
	desk->m_flag = pMsg->m_flag;
	desk->m_cost = pMsg->m_cost;
	desk->m_even = pMsg->m_even;

	desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart);

	desk->m_free = pMsg->m_free;
	desk->m_startTime = pMsg->m_startTime;
	desk->m_endTime = pMsg->m_endTime;

	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUser->GetUserDataId());
	desk->SetCreatType(pMsg->m_deskType);
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_curCircleReal = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	auto iPlayType = pMsg->m_playType.begin();
	for (; iPlayType != pMsg->m_playType.end(); iPlayType ++)
	{
		log->m_playtype.push_back( *iPlayType );
	}
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	if (FEAT_STARTUP_SCORE) {
		for (int x = 0; x < DESK_USER_COUNT; ++x) log->m_score[x] = FEAT_STARTUP_SCORE;
	}

	log->m_desk->SetVip(log);

	ret.m_deskId = log->m_desk->GetDeskId();
	log->m_desk->m_Gps_Limit = pMsg->m_GpsLimit;  //保存GPS限制信息
	desk->m_allowLookOn = pMsg->m_allowLookOn;    //保存是否允许观战

	pUser->Send(ret);

	log->AddUser(pUser);
	log->m_desk->OnUserInRoom(pUser);
	
	MHLOG("*******gConfig.GetDebugModel() = %d , gConfig.GetIfAddRobot()= %d", gConfig.GetDebugModel(), gConfig.GetIfAddRobot());
	MHLOG("*******pMsg->m_robotNum = %d , log->m_deskId= %d", pMsg->m_robotNum, log->m_deskId);
	//判断是否需要加电脑， 增加电脑
	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		/*for (Lint i = 0; i < CARD_COUNT; i ++)
		{
			log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}*/

		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player1Card[i].m_color = pMsg->m_Player0CardValue[i].kColor;
			log->m_desk->m_player1Card[i].m_number = pMsg->m_Player0CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player2Card[i].m_color = pMsg->m_Player1CardValue[i].kColor;
			log->m_desk->m_player2Card[i].m_number = pMsg->m_Player1CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player3Card[i].m_color = pMsg->m_Player2CardValue[i].kColor;
			log->m_desk->m_player3Card[i].m_number = pMsg->m_Player2CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player4Card[i].m_color = pMsg->m_Player3CardValue[i].kColor;
			log->m_desk->m_player4Card[i].m_number = pMsg->m_Player3CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 84; i++)
		{
			log->m_desk->m_SendCard[i].m_color = pMsg->m_SendCardValue[i].kColor;
			log->m_desk->m_SendCard[i].m_number = pMsg->m_SendCardValue[i].kNumber;
		}
		for (Lint i = 0; i < 2; i++)
		{
			log->m_desk->m_HaoZiCard[i].m_color = pMsg->m_HaoZiCardValue[i].kColor;
			log->m_desk->m_HaoZiCard[i].m_number = pMsg->m_HaoZiCardValue[i].kNumber;
		}
		
		if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
		{
			gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
		}

	}
	return ret.m_errorCode;
}

Lint RoomVip::CreateVipDeskForOther(LMsgLMG2LCreateDeskForOther* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		return -1;
	}
	LLOG_INFO("Logwyz.................RoomVip::CreateVipDeskForOther   pMsg->m_Greater2CanStart=[%d] pMsg->m_GpsLimit:%d", pMsg->m_Greater2CanStart, pMsg->m_GpsLimit);

	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;

	//if(pMsg->m_playType.empty())
	//{
	//	LLOG_ERROR("RoomVip::CreateVipDesk play type is empty, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
	//	ret.m_errorCode = 5;
	//	pUser->Send(ret);
	//	return ret.m_errorCode;
	//}

	if (pMsg->m_flag < CARD_TYPE_4 || pMsg->m_flag > CARD_TYPE_16)
	{
		pMsg->m_flag = CARD_TYPE_8;
	}

	Lint itemId = 0;
	Lint circle = 0;
#ifndef _MH_DEBUG
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle = 8;
		break;
	case CARD_TYPE_8:
		circle = 16;
		break;
	case CARD_TYPE_16:
		circle = 16;
		break;
	}
#else
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle = 1;
		break;
	case CARD_TYPE_8:
		circle = 2;
		break;
	case CARD_TYPE_16:
		circle = 16;
		break;
	}
#endif

	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk deskID is in Use, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//创建桌子失败
	Desk* desk = GetFreeDesk(pMsg->m_deskID, (GameType)pMsg->m_state);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed! userid=%d deskid=%d gametype=%d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	LLOG_DEBUG("RoomVip::CreateVipDesk userid=%d deskid=%d gametype=%d pMsg->m_deskType = %d  cost = %d even = %d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state, pMsg->m_deskType, pMsg->m_cost, pMsg->m_even);
	desk->SetSelectSeat(true);  //设置目前处于选座状态
	desk->SetCreatType(pMsg->m_deskType);
	desk->m_creatUser = pUser;
	desk->m_creatUserId = pUser->GetUserDataId();
	desk->m_creatUserNike = pUser->m_userData.m_nike;
	desk->m_creatUserUrl = pUser->m_userData.m_headImageUrl;
	desk->m_unioid = pUser->m_userData.m_unioid;
	desk->m_cheatAgainst = pMsg->m_cheatAgainst;
	desk->m_feeType = pMsg->m_feeType;
	desk->m_cellscore = pMsg->m_cellscore;
	desk->m_state = pMsg->m_state;
	desk->m_flag = pMsg->m_flag;
	desk->m_cost = pMsg->m_cost;
	desk->m_even = pMsg->m_even;
	desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart);
	desk->m_free = pMsg->m_free;

	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUser->GetUserDataId());
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_curCircleReal = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	log->m_desk->m_Gps_Limit = pMsg->m_GpsLimit;
	auto iPlayType = pMsg->m_playType.begin();
	for (; iPlayType != pMsg->m_playType.end(); iPlayType++)
	{
		log->m_playtype.push_back(*iPlayType);
	}
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	if (FEAT_STARTUP_SCORE) {
		for (int x = 0; x < DESK_USER_COUNT; ++x) log->m_score[x] = FEAT_STARTUP_SCORE;
	}

	log->m_desk->SetVip(log);

	ret.m_deskId = log->m_desk->GetDeskId();
	pUser->Send(ret);

	
	LMsgL2LMGModifyUserCreatedDesk  sendLMData;
	sendLMData.m_strUUID = pMsg->m_strUUID;
	sendLMData.m_userid = pMsg->m_userid;
	sendLMData.m_deskId = desk->GetDeskId();
	if (desk->m_feeType == 0) 
	{
		sendLMData.m_cost = desk->m_cost;
	}
	gWork.SendToLogicManager(sendLMData);
	

	//MHLOG("******************gConfig.GetDebugModel() = %d , gConfig.GetIfAddRobot()= %d", gConfig.GetDebugModel(),  gConfig.GetIfAddRobot());
	//MHLOG("******************pMsg->m_robotNum = %d , log->m_deskId= %d", pMsg->m_robotNum, log->m_deskId);
	//判断是否需要加电脑， 增加电脑
	
	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		//for (Lint i = 0; i < CARD_COUNT; i ++)
		//{
		//log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
		//log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		//}

		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player1Card[i].m_color = pMsg->m_Player0CardValue[i].kColor;
			log->m_desk->m_player1Card[i].m_number = pMsg->m_Player0CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player2Card[i].m_color = pMsg->m_Player1CardValue[i].kColor;
			log->m_desk->m_player2Card[i].m_number = pMsg->m_Player1CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player3Card[i].m_color = pMsg->m_Player2CardValue[i].kColor;
			log->m_desk->m_player3Card[i].m_number = pMsg->m_Player2CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 13; i++)
		{
			log->m_desk->m_player4Card[i].m_color = pMsg->m_Player3CardValue[i].kColor;
			log->m_desk->m_player4Card[i].m_number = pMsg->m_Player3CardValue[i].kNumber;
		}
		for (Lint i = 0; i < 84; i++)
		{
			log->m_desk->m_SendCard[i].m_color = pMsg->m_SendCardValue[i].kColor;
			log->m_desk->m_SendCard[i].m_number = pMsg->m_SendCardValue[i].kNumber;
		}
		for (Lint i = 0; i < 2; i++)
		{
			log->m_desk->m_HaoZiCard[i].m_color = pMsg->m_HaoZiCardValue[i].kColor;
			log->m_desk->m_HaoZiCard[i].m_number = pMsg->m_HaoZiCardValue[i].kNumber;
		}

		if (_MH_ROBOT_PERF_TEST)
		{
			gRobotManager.AddRobotToDesk(log->m_deskId, desk->GetPlayerCapacity());
		}
		else if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
		{
			gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
		}

	}
	
	return ret.m_errorCode;
}

//俱乐部  这个暂时没有用，创建多个桌子的
Lint RoomVip::CreateVipDeskForClub(LMsgLMG2LCreateDeskForClub* pMsg)
{
	LLOG_DEBUG("RoomVip::CreateVipDeskForClub");
	if (!pMsg)
	{
		return -1;
	}
	LLOG_DEBUG("RoomVip::CreateVipDeskForClub,   creatorId = [%d],m_deskCount=[%d]",pMsg->m_userid, pMsg->m_deskCount);

	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode=0;

	//if(pMsg->m_playType.empty())
	//{
	//	LLOG_ERROR("RoomVip::CreateVipDesk play type is empty, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
	//	ret.m_errorCode = 5;
	//	pUser->Send(ret);
	//	return ret.m_errorCode;
	//}

	if (pMsg->m_flag < CARD_TYPE_4||pMsg->m_flag > CARD_TYPE_16)
	{
		pMsg->m_flag=CARD_TYPE_8;
	}

	Lint itemId=0;
	Lint circle=0;
#ifndef _MH_DEBUG
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle=8;
		break;
	case CARD_TYPE_8:
		circle=16;
		break;
	case CARD_TYPE_16:
		circle=16;
		break;
	}
#else
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle=1;
		break;
	case CARD_TYPE_8:
		circle=2;
		break;
	case CARD_TYPE_16:
		circle=16;
		break;
	}
#endif
	for(int i=0;i<pMsg->m_deskCount;i++)
	{
		if (i>pMsg->m_deskIds.size())break;

		Lint m_deskID=pMsg->m_deskIds[i];
		LLOG_DEBUG("RoomVip::CreateVipDeskForClub  [%d]=[%d]", i, m_deskID);
		if (GetDeskById(m_deskID)!=NULL)
		{
			LLOG_ERROR("RoomVip::CreateVipDeskForClub deskID is in Use, userid=%d deskid=%d", pMsg->m_userid, m_deskID);
			ret.m_errorCode=3;
			//pUser->Send(ret);
			break;
		}

		//创建桌子失败
		Desk* desk=GetFreeDesk(m_deskID, (GameType)pMsg->m_state);
		if (desk==NULL)
		{
			LLOG_ERROR("RoomVip::CreateVipDeskForClub create Desk Failed! userid=%d deskid=%d gametype=%d", pMsg->m_userid, m_deskID, pMsg->m_state);
			ret.m_errorCode=4;
			//pUser->Send(ret);
			return ret.m_errorCode;
		}
		LLOG_DEBUG("RoomVip::CreateVipDeskForClub userid=%d deskid=%d gametype=%d pMsg->m_deskType = %d  cost = %d even = %d", pMsg->m_userid, m_deskID, pMsg->m_state, pMsg->m_deskType,		pMsg->m_cost, pMsg->m_even);
		desk->SetSelectSeat(true);  //设置目前处于选座状态
		desk->SetCreatType(pMsg->m_deskType);
		desk->m_creatUser=NULL;
		desk->m_creatUserId=pMsg->m_userid;
		//desk->m_creatUserNike=pUser->m_userData.m_nike;
		//desk->m_creatUserUrl=pUser->m_userData.m_headImageUrl;
		//desk->m_unioid=pUser->m_userData.m_unioid;
		desk->m_cheatAgainst=pMsg->m_cheatAgainst;
		desk->m_feeType=pMsg->m_feeType;
		desk->m_cellscore=pMsg->m_cellscore;
		desk->m_state=pMsg->m_state;
		desk->m_flag=pMsg->m_flag;
		desk->m_cost=pMsg->m_cost;
		desk->m_even=pMsg->m_even;
		desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart);
		desk->m_free=pMsg->m_free;
		//desk->MHSetClubInfo(pMsg->m_clubId, pMsg->m_playTypeId);

		//////////////////////////////////////////////////////////////////////////
		VipLogItem* log=gVipLogMgr.GetNewLogItem(circle, pMsg->m_userid);
		log->m_desk=desk;
		log->m_curCircle=0;
		log->m_curCircleReal=0;
		log->m_maxCircle=circle;
		log->m_state=pMsg->m_state;
		log->m_deskId=log->m_desk->GetDeskId();
		auto iPlayType=pMsg->m_playType.begin();
		for (; iPlayType!=pMsg->m_playType.end(); iPlayType++)
		{
			log->m_playtype.push_back(*iPlayType);
		}
		log->m_iPlayerCapacity=desk->GetPlayerCapacity();
		if (FEAT_STARTUP_SCORE) {
			for (int x=0; x<DESK_USER_COUNT; ++x) log->m_score[x]=FEAT_STARTUP_SCORE;
		}

		log->m_desk->SetVip(log);
	}

	//ret.m_deskId=log->m_desk->GetDeskId();
	//pUser->Send(ret);

	//这个需要处理 ？？？？？
	//LMsgL2LMGModifyUserCreatedDesk  sendLMData;
	//sendLMData.m_strUUID=pMsg->m_strUUID;
	//sendLMData.m_userid=pMsg->m_userid;
	//sendLMData.m_deskId=desk->GetDeskId();
	//if (desk->m_feeType==0)
	//{
	//	sendLMData.m_cost=desk->m_cost;
	//}
	//gWork.SendToLogicManager(sendLMData);


	//MHLOG("******************gConfig.GetDebugModel() = %d , gConfig.GetIfAddRobot()= %d", gConfig.GetDebugModel(),  gConfig.GetIfAddRobot());
	//MHLOG("******************pMsg->m_robotNum = %d , log->m_deskId= %d", pMsg->m_robotNum, log->m_deskId);
	//判断是否需要加电脑， 增加电脑

	//if (gConfig.GetDebugModel()&&gConfig.GetIfAddRobot())
	//{
	//	//for (Lint i = 0; i < CARD_COUNT; i ++)
	//	//{
	//	//log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
	//	//log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
	//	//}
	//
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player1Card[i].m_color=pMsg->m_Player0CardValue[i].m_color;
	//		log->m_desk->m_player1Card[i].m_number=pMsg->m_Player0CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player2Card[i].m_color=pMsg->m_Player1CardValue[i].m_color;
	//		log->m_desk->m_player2Card[i].m_number=pMsg->m_Player1CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player3Card[i].m_color=pMsg->m_Player2CardValue[i].m_color;
	//		log->m_desk->m_player3Card[i].m_number=pMsg->m_Player2CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player4Card[i].m_color=pMsg->m_Player3CardValue[i].m_color;
	//		log->m_desk->m_player4Card[i].m_number=pMsg->m_Player3CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 84; i++)
	//	{
	//		log->m_desk->m_SendCard[i].m_color=pMsg->m_SendCardValue[i].m_color;
	//		log->m_desk->m_SendCard[i].m_number=pMsg->m_SendCardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 2; i++)
	//	{
	//		log->m_desk->m_HaoZiCard[i].m_color=pMsg->m_HaoZiCardValue[i].m_color;
	//		log->m_desk->m_HaoZiCard[i].m_number=pMsg->m_HaoZiCardValue[i].m_number;
	//	}
	//
	//	if (_MH_ROBOT_PERF_TEST)
	//	{
	//		gRobotManager.AddRobotToDesk(log->m_deskId, desk->GetPlayerCapacity());
	//	}
	//	else if (pMsg->m_robotNum > 0&&pMsg->m_robotNum < 4)
	//	{
	//		gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
	//	}
	//
	//}

	return ret.m_errorCode;
}

Lint RoomVip::CreateVipDeskForClub(LMsgLMG2LCreateClubDeskAndEnter* pMsg, User* pJoinUser)
{
	LLOG_DEBUG("RoomVip::CreateVipDeskForClub");
	if (!pMsg ||pJoinUser==NULL)
	{
		return -1;
	}
	LLOG_DEBUG("RoomVip::CreateVipDeskForClub, creatorId = [%d],m_deskId=[%d] m_GpsLimit:%d", pMsg->m_userid, pMsg->m_deskId, pMsg->m_GpsLimit);

	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode=0;

	if (pMsg->m_flag < CARD_TYPE_4||pMsg->m_flag > CARD_TYPE_16)
	{
		pMsg->m_flag=CARD_TYPE_8;
	}

	Lint itemId=0;
	Lint circle=0;
#ifndef _MH_DEBUG
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle=8;
		break;
	case CARD_TYPE_8:
		circle=16;
		break;
	case CARD_TYPE_16:
		circle=16;
		break;
	}
#else
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle=1;
		break;
	case CARD_TYPE_8:
		circle=2;
		break;
	case CARD_TYPE_16:
		circle=16;
		break;
	}
#endif
	
	ret.m_deskId=pMsg->m_clubDeskId;

	Lint m_deskID=pMsg->m_deskId;
	if (GetDeskById(m_deskID)!=NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDeskForClub deskID is in Use, userid=%d deskid=%d", pMsg->m_userid, m_deskID);
		ret.m_errorCode=3;
		//这里不做处理，根据返回值上层函数处理
		//pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	//创建桌子失败
	Desk* desk=GetFreeDesk(m_deskID, (GameType)pMsg->m_state);
	if (desk==NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDeskForClub create Desk Failed! userid=%d deskid=%d gametype=%d", pMsg->m_userid, m_deskID, pMsg->m_state);
		ret.m_errorCode=4;
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}
	LLOG_DEBUG("RoomVip::CreateVipDeskForClub userid=%d deskid=%d gametype=%d pMsg->m_deskType = %d  cost = %d even = %d", pMsg->m_userid, m_deskID, pMsg->m_state, pMsg->m_deskType, pMsg->m_cost, pMsg->m_even);

	desk->SetSelectSeat(true);  //设置目前处于选座状态
	desk->SetCreatType(pMsg->m_deskType);
	desk->m_creatUser=NULL;
	desk->m_creatUserId=pMsg->m_userid;
	desk->m_creatUserNike=pMsg->m_usert.m_nike;
	desk->m_creatUserUrl=pMsg->m_usert.m_headImageUrl;
	desk->m_unioid=pMsg->m_usert.m_unioid;
	desk->m_cheatAgainst=pMsg->m_cheatAgainst;
	desk->m_feeType=pMsg->m_feeType;
	desk->m_cellscore=pMsg->m_cellscore;
	desk->m_state=pMsg->m_state;
	desk->m_flag=pMsg->m_flag;
	desk->m_cost=pMsg->m_cost;
	desk->m_even=pMsg->m_even;
	desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart);
	desk->m_free=pMsg->m_free;
	desk->MHSetClubInfo(pMsg->m_clubId, pMsg->m_playTypeId,pMsg->m_clubDeskId,pMsg->m_showDeskId);
	
	//这里暂时不支持俱乐部的GPS距离检查
	desk->m_Gps_Limit = pMsg->m_GpsLimit;

	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log=gVipLogMgr.GetNewLogItem(circle, pMsg->m_userid);
	log->m_desk=desk;
	log->m_curCircle=0;
	log->m_curCircleReal=0;
	log->m_maxCircle=circle;
	log->m_state=pMsg->m_state;
	log->m_deskId=log->m_desk->GetDeskId();
	auto iPlayType=pMsg->m_playType.begin();
	for (; iPlayType!=pMsg->m_playType.end(); iPlayType++)
	{
		log->m_playtype.push_back(*iPlayType);
	}
	log->m_iPlayerCapacity=desk->GetPlayerCapacity();
	if (FEAT_STARTUP_SCORE) {
		for (int x=0; x<DESK_USER_COUNT; ++x) log->m_score[x]=FEAT_STARTUP_SCORE;
	}

	log->m_desk->SetVip(log);
	
	pJoinUser->Login();

	pJoinUser->setUserState(LGU_STATE_DESK);
	pJoinUser->ModifyUserState();
	//加入房间
	log->AddUser(pJoinUser);
	log->m_desk->OnUserInRoom(pJoinUser);

	//ret.m_deskId=log->m_desk->GetDeskId();
	//pUser->Send(ret);

	//这个需要处理 ？？？，这个不需要处理，这是通知manager 创建房间预付款的
	//LMsgL2LMGModifyUserCreatedDesk  sendLMData;
	//sendLMData.m_strUUID=pMsg->m_strUUID;
	//sendLMData.m_userid=pMsg->m_userid;
	//sendLMData.m_deskId=desk->GetDeskId();
	//if (desk->m_feeType==0)
	//{
	//	sendLMData.m_cost=desk->m_cost;
	//}
	//gWork.SendToLogicManager(sendLMData);

	return ret.m_errorCode;
}

Lint RoomVip::AddToVipClubDesk(LMsgLMG2LEnterClubDesk* pMsg, User* pJoinUser)
{
	LLOG_DEBUG("RoomVip::AddToVipClubDesk");
	if (!pMsg||pJoinUser==NULL)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId=pMsg->m_deskId;
	ret.m_errorCode=0;

	if (pJoinUser->GetDesk())
	{
		//房主建房的有可能，不做处理
		//LLOG_ERROR("RoomVip::AddToVipClubDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
		//ret.m_errorCode=3;
		//pUser->Send(ret);
		//return ret.m_errorCode;
	}

	Desk* desk=GetDeskById(pMsg->m_deskId);

	if (!desk)
	{
		LLOG_ERROR("RoomVip::AddToVipClubDesk not find desk, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
		ret.m_errorCode=2;
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	//GPS信息检查
	if (desk->m_Gps_Limit == 1)
	{
		LLOG_DEBUG("RoomVip::AddToVipClubDesk GPS, userid:%d gps:%s value:[%f:%f]", pJoinUser->GetUserDataId(), pJoinUser->GetUserData().m_customString2.c_str(), pJoinUser->m_gps_lng, pJoinUser->m_gps_lat);
		if (pJoinUser->m_gps_lng == 0.0f || pJoinUser->m_gps_lat == 0.0f) //GPS数据无效
		{
			LLOG_ERROR("RoomVip::AddToVipClubDesk GPS Error, userid:%d, gps:%s- [%f:%f]", pJoinUser->GetUserDataId(), pJoinUser->GetUserData().m_customString2.c_str(), pJoinUser->m_gps_lng, pJoinUser->m_gps_lat);
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_INVALID;
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}
	}

	//desk->m_startTime=msg->m_startTime;
	//desk->m_endTime=msg->m_endTime;

	if (!desk->GetVip()||desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::AddToVipClubDesk desk already begin, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
		ret.m_errorCode=3;
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	//Lint nFree=0;
	//LTime cur;
	//Lint curTime=cur.Secs();
	//if (curTime>msg->m_startTime && curTime<msg->m_endTime)
	//{
	//	nFree=1;
	//}
	//
	//if (desk->m_state!=100010&&desk->m_state!=100008&&desk->m_state!=100009) {
	//	nFree=0;
	//}
	//房费这里临时去掉，以后统一处理房费
	//if (desk->m_feeType==1&&pJoinUser->m_userData.m_numOfCard2s-pMsg->m_creatDeskCost < desk->m_even )  //房间费用分摊时 金币不足
	//{
	//	LLOG_ERROR("RoomVip::AddToVipClubDesk coin is not enough, userid=%d deskid=%d free = %d", pJoinUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_Free);
	//	ret.m_errorCode=4;
	//	pJoinUser->Send(ret);
	//	return ret.m_errorCode;
	//}

	if (desk->GetVip()->IsFull(pJoinUser)||desk->GetUserCount()==desk->GetPlayerCapacity()||desk->getDeskState()!=DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::AddToVipClubDesk desk full, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
		ret.m_errorCode=1;
		desk->MHGetDeskUser(ret.m_deskSeatPlayerName, ret.m_deskNoSeatPlayerName);
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	// GPS距离检查
	Lstring user_gps_list;
	if (desk->m_Gps_Limit && !desk->MHCheckUserGPSLimit(pJoinUser, user_gps_list))
	{
		ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_Limit;
		ret.m_userGPSList = user_gps_list;
		pJoinUser->Send(ret);
		LLOG_ERROR("RoomVip::AddToVipDesk user GPS is forbitten, userid=%d deskid=%d userlist=%s", pJoinUser->GetUserDataId(), desk->GetDeskId(), ret.m_userGPSList.c_str());
		return ret.m_errorCode;
	}

	if (!desk->GetVip()->AddUser(pJoinUser))
	{
		LLOG_ERROR("RoomVip::AddToVipClubDesk desk adduser fail, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
		ret.m_errorCode=1;
		desk->MHGetDeskUser(ret.m_deskSeatPlayerName, ret.m_deskNoSeatPlayerName);
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	LLOG_INFO("RoomVip::AddToVipClubDesk userid=%d deskid=%d deskusercount=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId, desk->GetUserCount());
	desk->OnUserInRoom(pJoinUser);

	pJoinUser->Send(ret);
	return ret.m_errorCode;



}

Lint RoomVip::CreateVipCoinDesk(LMsgCN2LCreateCoinDesk*pMsg,User* pUsers[])
{
	if(!pMsg) 
		return -1;

	Lint circle = 1;

	//创建桌子失败
	Desk* desk = GetFreeDesk(pMsg->m_deskId, (GameType)pMsg->m_state);
	if (desk == NULL)
	{
		return -1;
	}

	desk->SetDeskType( DeskType_Coins );   //金币场桌子
	desk->m_baseScore = pMsg->m_baseScore;
	desk->m_autoChangeOutTime = pMsg->m_changeOutTime;
	desk->m_autoPlayOutTime = pMsg->m_opOutTime;
	
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUsers[0]->GetUserDataId());
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_curCircleReal = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	auto iPlayType = pMsg->m_playType.begin();
	for (; iPlayType != pMsg->m_playType.end(); iPlayType ++)
	{
		log->m_playtype.push_back( *iPlayType );
	}
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	log->m_desk->SetVip(log);

	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		log->AddUser(pUsers[i]);
	}

	// 是否配牌
	if (gConfig.GetDebugModel())
	{
		for (Lint i = 0; i < CARD_COUNT; i ++)
		{
			log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].kColor;
			log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].kNumber;
		}
	}

	log->m_desk->OnUserInRoom(pUsers);

	return 0;
}

Lint RoomVip::AddToVipDesk(User* pUser, Lint nDeskID)
{
	if (!pUser)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = nDeskID;
	ret.m_errorCode = 0;

	if(pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk )
	{
		LLOG_ERROR("RoomVip::AddToVipDesk not find desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Lstring user_gps_list;
	if (desk->m_Gps_Limit && !desk->MHCheckUserGPSLimit(pUser, user_gps_list))
	{
		LLOG_ERROR("RoomVip::AddToVipDesk user GPS is forbitten, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_Limit;
		ret.m_userGPSList = user_gps_list;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->m_feeType == 1 && pUser->m_userData.m_numOfCard2s < desk->m_even)  //房间费用分摊时 金币不足
	{
		LLOG_ERROR("RoomVip::AddToVipDesk coin is not enough, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity() || desk->getDeskState() !=  DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if ( !desk->GetVip()->AddUser(pUser) )
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	LLOG_INFO("RoomVip::AddToVipDesk userid=%d deskid=%d deskusercount=%d", pUser->GetUserDataId(), nDeskID, desk->GetUserCount());
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);
	return ret.m_errorCode;
}

Lint RoomVip::AddToVipDesk(User* pUser, LMsgLMG2LAddToDesk* msg)
{
	if (!pUser)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = msg->m_deskID;
	ret.m_errorCode = 0;

	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(msg->m_deskID);

	if (!desk)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk not find desk, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//GPS信息检查
	if (desk->m_Gps_Limit == 1)
	{		
		LLOG_DEBUG("RoomVip::AddToVipDesk GPS, userid:%d gps:%s value:[%f:%f]",pUser->GetUserDataId(), pUser->GetUserData().m_customString2.c_str(), pUser->m_gps_lng, pUser->m_gps_lat);		 
		if (pUser->m_gps_lng == 0.0f || pUser->m_gps_lat == 0.0f) //GPS数据无效
		{
			LLOG_ERROR("RoomVip::AddToVipDesk GPS Error, userid:%d, gps:%s- [%f:%f]", pUser->GetUserDataId(), pUser->GetUserData().m_customString2.c_str(), pUser->m_gps_lng, pUser->m_gps_lat);			
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_INVALID;
			pUser->Send(ret);
			return ret.m_errorCode;
		}
	}

	desk->m_startTime = msg->m_startTime;
	desk->m_endTime = msg->m_endTime;

	if (!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Lint nFree = 0;
	LTime cur;
	Lint curTime = cur.Secs();
	if (curTime>msg->m_startTime && curTime<msg->m_endTime) 
	{
		nFree = 1;
	}

	if (desk->m_state != 100010 && desk->m_state != 100008 && desk->m_state != 100009) {
		nFree = 0;
	}

	LLOG_DEBUG("Logwyz ................ desk->m_feeType=[%d], m_numOfCard2s=[%d],desk->m_even=[%d]", desk->m_feeType, pUser->m_userData.m_numOfCard2s, desk->m_even);

	if (desk->m_feeType == 1 && pUser->m_userData.m_numOfCard2s - msg->m_creatDeskCost < desk->m_even && nFree == 0)  //房间费用分摊时 金币不足
	{
		LLOG_ERROR("RoomVip::AddToVipDesk coin is not enough, userid=%d deskid=%d free = %d", pUser->GetUserDataId(), msg->m_deskID, msg->m_Free);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	// GPS距离检查
	Lstring user_gps_list;
	if (desk->m_Gps_Limit && !desk->MHCheckUserGPSLimit(pUser, user_gps_list))
	{
		 
		ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_Limit;
		ret.m_userGPSList = user_gps_list;
		pUser->Send(ret);
		LLOG_ERROR("RoomVip::AddToVipDesk user GPS is forbitten, userid=%d deskid=%d userlist=%s", pUser->GetUserDataId(), desk->GetDeskId(), ret.m_userGPSList.c_str());
		return ret.m_errorCode;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity() || desk->getDeskState() != DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
		ret.m_errorCode = 1;
		desk->MHGetDeskUser(ret.m_deskSeatPlayerName, ret.m_deskNoSeatPlayerName);
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip()->AddUser(pUser))
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
		ret.m_errorCode = 1;
		desk->MHGetDeskUser(ret.m_deskSeatPlayerName, ret.m_deskNoSeatPlayerName);
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	LLOG_INFO("RoomVip::AddToVipDesk userid=%d deskid=%d deskusercount=%d", pUser->GetUserDataId(), msg->m_deskID, desk->GetUserCount());
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);
	return ret.m_errorCode;

}

Lint RoomVip::SelectSeatInVipDesk(User* pUser, Lint nDeskID)
{
	if (!pUser)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = nDeskID;
	ret.m_errorCode = 0;

	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk not find desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity() || desk->getDeskState() != DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip()->AddUser(pUser))
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	LLOG_INFO("RoomVip::AddToVipDesk userid=%d deskid=%d deskusercount=%d", pUser->GetUserDataId(), nDeskID, desk->GetUserCount());
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);
	return ret.m_errorCode;
}


bool RoomVip::LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser)
{
	if (!pMsg || !pUser)
	{
		return false;
	}
	LMsgS2CLeaveDesk send;
	send.m_errorCode = 0;

	Desk* desk = pUser->GetDesk();
	if(desk == NULL || desk->getDeskState() != DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk fail, userid=%d deskid=%d deskstate=%d", pUser->GetUserDataId(), desk ? desk->GetDeskId() : 0, desk ? desk->getDeskState() : -1);
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}

	if(!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), desk->GetDeskId());
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}

	LLOG_INFO("RoomVip::LeaveToVipDesk userid=%d deskid=%d", pUser->GetUserDataId(), desk->GetDeskId());
	pUser->Send(send);


	//房主
	if(desk->GetVip()->GetOwerId() ==  pUser->GetUserDataId() && desk->GetCreatType() == 0)
	{
		LLOG_INFO("RoomVip::LeaveToVipDesk Ower leave");
		//把开始按钮位置空,这个函数不用了，（现在的逻辑：房主选座后退出，再回到房间，他还是原来的座位；其他人选座后退出，再进入需要重新选座）
		//desk->MHSetStartButtonPos(INVAILD_POS);
		//房间保留，不做任何操作
		gWork.HanderUserLogout(pUser->getUserGateID() , pUser->m_userData.m_unioid);
	}
	else
	{
		LLOG_INFO("RoomVip::LeaveToVipDesk not Ower leave");
		desk->GetVip()->RemoveUser(pUser->GetUserDataId());
		desk->OnUserOutRoom(pUser);
	}
	return true;
}

void RoomVip::MHPrintAllDeskStatus()
{
	Lint free = 0;
	Lint wait = 0;
	Lint play = 0;
	DeskMap::iterator it = m_deskMap.begin();	
	for (; it != m_deskMap.end(); it++)
	{
		if (it->second)
		{
			it->second->MHPrintDeskStatus();
			Lint state = it->second->getDeskState();
			switch (state)
			{
			case DESK_FREE:
				free += 1;
				break;
			case DESK_WAIT:
				wait += 1;
				break;
			case DESK_PLAY:
				play += 1;
				break;
			default:
				break;
			}
		}
	}
	MHLOG_DESK("***Total desk number: %d wait:%d, free:%d, play:%d", m_deskMap.size(), wait, free, play);
}


//与manager重新连接后，向manager同步桌子信息
void RoomVip::SynchroDeskData(std::vector<LogicDeskInfo> & DeskInfo)
{
	LLOG_DEBUG("RoomVip::SynchroDeskData  deskSize=[%d]",m_deskMap.size());
	for (auto ItDesk=m_deskMap.begin(); ItDesk!=m_deskMap.end(); ItDesk++)
	{
		if (ItDesk->second==NULL)continue;
		LogicDeskInfo tempDeskInfo;
		tempDeskInfo.m_deskId=ItDesk->second->GetDeskId();

		tempDeskInfo.m_clubId=ItDesk->second->MHGetClubId();
		tempDeskInfo.m_playTypeId=ItDesk->second->MHGetPlayTypeId();
		tempDeskInfo.m_clubDeskId=ItDesk->second->MHGetClubDeskId();
		tempDeskInfo.m_showDeskId=ItDesk->second->MHGetClubShowDeskId();

		tempDeskInfo.m_roomFull=ItDesk->second->m_deskState==DESK_PLAY?1:0;   //？？
		if (ItDesk->second->m_vip!=NULL)
		{
			tempDeskInfo.m_currCircle=ItDesk->second->m_vip->m_curCircle+1;
			tempDeskInfo.m_totalCircle=ItDesk->second->m_vip->m_maxCircle;
		}
		else
		{
			LLOG_ERROR("RoomVip::SynchroDeskData  deskId[%d] m_vip==NULL",ItDesk->first);
		}

		//桌子用户
		for (int i=0; i<DESK_USER_COUNT; i++)
		{
			if (ItDesk->second->m_seatUser[i])
			{
				LogicDeskUserInfo  tempDeskUserInfo;
				tempDeskUserInfo.m_userId=ItDesk->second->m_seatUser[i]->GetUserDataId();
				tempDeskUserInfo.m_pos=i;
				tempDeskUserInfo.m_headUrl=ItDesk->second->m_seatUser[i]->m_userData.m_headImageUrl;
				tempDeskUserInfo.m_nike=ItDesk->second->m_seatUser[i]->m_userData.m_nike;
				tempDeskUserInfo.m_ip=ItDesk->second->m_seatUser[i]->m_ip;
				tempDeskInfo.m_seatUser.push_back(tempDeskUserInfo);
			}
		}
		tempDeskInfo.m_seatUserCount=tempDeskInfo.m_seatUser.size();

		if (ItDesk->second->m_deskState==DESK_WAIT)
		{
			for (auto ItLookonUser=ItDesk->second->m_desk_Lookon_user.begin(); ItLookonUser!=ItDesk->second->m_desk_Lookon_user.end(); ItLookonUser++)
			{
				if ((*ItLookonUser)!=NULL)
				{
					LogicDeskUserInfo  tempDeskUserInfo;
					tempDeskUserInfo.m_userId=(*ItLookonUser)->GetUserDataId();
					tempDeskUserInfo.m_pos=INVAILD_POS;
					tempDeskUserInfo.m_headUrl=(*ItLookonUser)->m_userData.m_headImageUrl;
					tempDeskUserInfo.m_nike=(*ItLookonUser)->m_userData.m_nike;
					tempDeskUserInfo.m_ip=(*ItLookonUser)->m_ip;
					tempDeskInfo.m_noSeatUser.push_back(tempDeskUserInfo);
				}
			}
		}

		tempDeskInfo.m_noSeatUserCount=tempDeskInfo.m_noSeatUser.size();

		DeskInfo.push_back(tempDeskInfo);
	}
}


bool SpecPaiXingMgr::Init()
{
	m_mapFlagToSpecPaixing[100001] = boost::shared_ptr<std::vector<Lint>>(new std::vector<Lint>());
	m_mapFlagToSpecPaixing[100001]->push_back(HU_Qixiaodui);
	return true;
}
bool SpecPaiXingMgr::Final()
{
	m_mapFlagToSpecPaixing.clear();
	return true;
}
bool SpecPaiXingMgr::FiltSpecPaiXingByFlag(Lint flag, const std::vector<Lint>& src, std::vector<int>&dst)
{
	std::map<Lint, boost::shared_ptr<std::vector<Lint>>>::iterator it = m_mapFlagToSpecPaixing.find(flag);
	if (it == m_mapFlagToSpecPaixing.end())
	{
		return false;
	}
	dst.clear();
	std::vector<Lint>::const_iterator it2 = src.begin();
	while (it2 < src.end())
	{
		std::vector<Lint>::const_iterator it3 = std::find(it->second->begin(), it->second->end(), *it2);
		if (it3 < it->second->end())
		{
			dst.push_back(*it3);
		}
		it2++;
	}
	return true;
}

Lstring SpecPaiXingMgr::GetSpecPaiXingString(Lint isWin, Lint flag, const std::vector<Lint> &src, bool bEnableFilter)
{
	std::vector<Lint> dst;
	std::vector<Lint> out;
	std::ostringstream ss;
	Lstring strResult;	
	if (bEnableFilter)
	{
		FiltSpecPaiXingByFlag(flag, src, dst);
		out = dst;
	}
	else
	{
		out = src;
	}

	for (int i = 0; i < out.size(); i++)
	{
		if (i < out.size() - 1)
		{
			ss << out[i] << ',';
		}
		else
		{
			ss << out[i];
		}
	}	
	return ss.str();
}


