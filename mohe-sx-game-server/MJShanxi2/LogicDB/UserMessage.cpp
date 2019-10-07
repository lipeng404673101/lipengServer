#include "UserMessage.h"
#include "LTime.h"
#include "LMemoryRecycle.h"
#include "Config.h"
#include "DbServerManager.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "ActiveManager.h"
#include "UserPlayData.h"
#include "mhmsghead.h"

#define CREDIT_KEY_TTL_TIME    (3*24*60*60)   //2016-12-26 modify three days
#define CREDIT_HISTORY_LIMIT   (20)             //限制条数

CUserMessage::CUserMessage():m_dbsession(NULL), m_dbsessionBK(NULL)
{
}

CUserMessage::~CUserMessage()
{

}

void CUserMessage::Clear()
{

}

void CUserMessage::Run()
{
	if(!this->Init())
	{
		LLOG_ERROR("DbServer init Failed!!!");
		return;
	}

	if(!this->_initRedisConnect())
	{
		LLOG_ERROR("_initRedisConnect Failed!!!");
		return;
	}

	LTime msgTime;
	int iMsgCount = 0;
	int iElapseTime = 0;
	while(!GetStop())
	{
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			iElapseTime++;
			continue;
		}
		else
		{
			HanderMsg(msg);
			m_memoryRecycle.recycleMsg(msg);

			//每一万个包输出所用时间
			iMsgCount++;
			if(iMsgCount == 10000)
			{
				LTime now;
				LLOG_ERROR("User Logic: 10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

				iMsgCount = 0;
				iElapseTime = 0;

				msgTime = now;
			}
		}
	}

	this->Final();
}

void CUserMessage::Start()
{
	m_memoryRecycle.Start();
	LRunnable::Start();
}

void CUserMessage::Stop()
{
	m_memoryRecycle.Stop();
	LRunnable::Stop();
}

void CUserMessage::Join()
{
	m_memoryRecycle.Join();
	LRunnable::Join();
}

void CUserMessage::HanderMsg(LMsg* msg)
{
	switch(msg->m_msgId)
	{
	//玩家videoID录像 msg_id = 15003 （请求回放：第三步）
	case MSG_L_2_LDB_VIDEO:
	{
		LMsgL2LDBReqVideo* pMsg = (LMsgL2LDBReqVideo*)msg;
		if (pMsg&&pMsg->m_flag == 0)
			HanderLogicReqVideo(pMsg);
		else
			HanderLogicQiPaiReqVideo(pMsg);
		break;
	}
	//玩家大厅请求战绩列表（最近10场，请求战绩：第一步）
	case MSG_L_2_LDB_VIP_LOG:
	{
		LMsgL2LBDReqVipLog* pMsg=(LMsgL2LBDReqVipLog*)msg;
		if (pMsg && pMsg->m_flag==0)
			HanderLogicReqVipLog(pMsg);
		else
			HanderLogicReqPokerLog(pMsg);
		
		break;
	}
	case MSG_L_2_LDB_CRE_LOG:
		HanderLogicReqCRELog((LMsgL2LBDReqCRELog*) msg);
		break;
	case MSG_L_2_LDB_ROOM_SAVE:
		HanderLogicReqSaveLog((LMsgL2LDBSaveLogItem*)msg);
		break;
	case MSG_L_2_LDB_VIDEO_SAVE:
		HanderLogicReqSaveVideo((LMsgL2LDBSaveVideo*)msg);
		break;
	//玩家请求战绩 msg_id = 15014 （请求回放：第二步）
	case MSG_L_2_LDB_ROOM_LOG:
	{
		LMsgL2LDBReqRoomLog* pMsg = (LMsgL2LDBReqRoomLog*)msg;
		if (pMsg && pMsg->m_flag == 0)
			HanderLogicReqRoomLog(pMsg);
		else
			HanderLogicQiPaiReqRoomLog(pMsg);
		break;
	}
	case MSG_L_2_LDB_ENDCRE_LOG:
		HanderLogicSaveCRELog((LMsgL2LDBSaveCRELog*)msg);
		break;
	case MSG_L_2_LDB_REQ_DELGTU:
		HanderLogicDelCRELog((LMsgL2LDBDEL_GTU*) msg);
		break;
	case MSG_LMG_2_LDB_USER_LOGIN:
		HanderUserLogin((LMsgLMG2LdbUserLogin*) msg);
		break;
	case MSG_C_2_S_ACTIVITY_WRITE_PHONE:
		HanderActivityPhone((LMsgC2SActivityPhone *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_LOG:
		HanderActivityRequestLog((LMsgC2SActivityRequestLog *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN:
		HanderActivityRequestDrawOpen((LMsgC2SActivityRequestDrawOpen *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW:
		HanderActivityRequestDraw((LMsgC2SActivityRequestDraw *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_SHARE:
		HanderActivityRequestShare((LMsgC2SActivityRequestShare *)msg);
		break;
		//logic 玩家请求分享某录像，先去sharevideo中查看是否已被分享
	case MSG_L_2_LDB_GETSHAREVIDEOID:
		HanderLogicGetShareVideoId((LMsgL2LDBGGetShareVideoId*) msg);
		break;
		//logic 请求保存某已转移到sharevideo中的某录像的shareid
	case MSG_L_2_LDB_SAVESHAREVIDEOID:
		HanderLogicSaveShareVideoId((LMsgL2LDBSaveShareVideoId*) msg);
		break;
		//logic 玩家通过shareid来获取该分享码下的录像 在sharevideo中查询
	case MSG_L_2_LDB_GETSHAREVIDEO:
		HanderLogicReqShareVideo((LMsgL2LDBGetShareVideo*) msg);
		break;
		//loigc 通过精确的videoid来查找sharevideo中的对应录像
	case MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID:
		HanderLogicGetShareVideo((LMsgL2LDBGetSsharedVideo*) msg);
		break;

		//loigc 通过Userid来查找玩家统计数据
	case MSG_L_2_LDB_GETUSERPLAYDATA:
		HanderLogicGetUserPlayData((LMsgL2LDBGetUserPlayData*)msg);
		break;

		//loigc 通过Userid来更新玩家统计数据
	case MSG_L_2_LDB_UPDATE_PLAYER_SCORE:
		HanderLogicGetShareVideo((LMsgL2LDBGetSsharedVideo*)msg);
		break;
	case MH_MSG_L_2_LDB_QUERY_SHARE_VIDEO_BY_SHARE_ID:
		LLOG_DEBUG("MH_MSG_L_2_LDB_QUERY_SHARE_VIDEO_BY_SHARE_ID, msgid=%d", msg->m_msgId);
		HandlerLogicQueryShareVideoByShareId2((MHLMsgL2LDBQueryShareVideoByShareId*)msg);
		break;

	default:
		LLOG_DEBUG("CUserMessage::HanderMsg not handle, msgid=%d", msg->m_msgId);
		break;
	}
}

//logic请求玩家录像
void CUserMessage::HanderLogicReqVideo(LMsgL2LDBReqVideo* msg)
{
	LMsgLDB2LReqVideo log;
	log.m_strUUID = msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	LLOG_ERROR("Request video. %d:%s",msg->m_userId, msg->m_videoId.c_str());
	//log id
	std::stringstream ss;	
	ss << "SELECT Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data, PlayType FROM video WHERE Id='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqVideo sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return ;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;

		info.m_str_playtype = *row++;
		log.m_count = 1;
	}

	mysql_free_result(res);

	msg->m_sp->Send(log.GetSendBuff());
}

//logic请求玩家majiang战绩
void CUserMessage::HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg)
{
	LLOG_DEBUG("HanderLogicReqVipLog userId[%d] clubId[%d] limit[%d] allclub[%d] time[%d]", msg->m_reqUserId, msg->m_clubId, msg->m_limit,msg->m_allClub,msg->m_time);
	LMsgLBD2LReqVipLog log;
	log.m_strUUID= msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if(pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	LLOG_DEBUG("Request log. %d:%d  ,clubId[%d],page[%d]",msg->m_userId,msg->m_reqUserId,msg->m_clubId,msg->m_limit);

	LTime cur;
	std::stringstream ss;
	if (msg->m_allClub == 1 && msg->m_clubId!=0)
	{
		ss << "SELECT Id,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,Reset,PlayType,ShowDeskId,CellScore FROM log WHERE ( ClubId=";
		ss << msg->m_clubId << " ) ";
		if (msg->m_time > 0)
		{
			ss << " AND Time <" << msg->m_time;
		}

		ss << " ORDER BY Time DESC LIMIT 10";
	}
	else
	{

		ss << "SELECT Id,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,Reset,PlayType,ShowDeskId,CellScore FROM log WHERE (Pos1=";
		ss << msg->m_reqUserId << " OR Pos2=";
		ss << msg->m_reqUserId << " OR Pos3=";
		ss << msg->m_reqUserId << " OR Pos4=";
		ss << msg->m_reqUserId << " ) ";
		if (msg->m_clubId != 0)
		{
			ss << "AND  ClubId=" << msg->m_clubId;
		}
		if (msg->m_time > 0)
		{
			ss << "AND Time <" << msg->m_time;
		}

		ss << " ORDER BY Time DESC LIMIT 10";
	}

	LLOG_DEBUG(" sql [%s]", ss.str().c_str());

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Query sql error - %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		LogInfo& info = log.m_info[log.m_count++];
		info.m_clubId = msg->m_clubId;
		info.m_id = *row++;
		info.m_time = atoi(*row++);
		info.m_posUserId[0] = atoi(*row++);
		info.m_posUserId[1] = atoi(*row++);
		info.m_posUserId[2] = atoi(*row++);
		info.m_posUserId[3] = atoi(*row++);
		info.m_flag = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		info.m_secret = *row++;
		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);
		info.m_reset = atoi(*row++);
		info.m_playtype = *row++;
		info.m_showDeskId = atoi(*row++);
		if (row && (*row))
		{
			info.m_cellScore = atoi(*row);
		}
		else
		{
			info.m_cellScore = 1;
		}
		row++;
	

		row = mysql_fetch_row(res);
		if (log.m_count >= 10)   //每次返回10条
			break;
	}

	mysql_free_result(res);

	msg->m_sp->Send(log.GetSendBuff());
}

//logic请求玩家poker战绩
void CUserMessage::HanderLogicReqPokerLog(LMsgL2LBDReqVipLog* msg)
{
	LLOG_DEBUG("CUserMessage::HanderLogicReqPokerLog m_allclub[%d]  m_clubid[%d]  m_time[%d]",msg->m_allClub,msg->m_clubId,msg->m_time);

	LMsgLBD2LReqPokerLog log;
	log.m_strUUID=msg->m_strUUID;
	log.m_userid=msg->m_userId;
	log.m_count=0;

	//if (msg->m_flag!=102)
	//{
	//	msg->m_flag=102;
	//}
	
	
	MYSQL* pMySQL=NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if (pMySQL==NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}
	
	LLOG_ERROR("Request log. %d:%d,   flag=[%d]", msg->m_userId, msg->m_reqUserId,msg->m_flag);
	
	//LTime cur;
	//log id
	std::stringstream ss;
	if (msg->m_allClub == 1 && msg->m_clubId!=0)
	{
		ss << "SELECT ";
		ss << "Id, Time, ClubId, DeskFlag, CreateUserId, Pos1, Pos2, Pos3, Pos4, Pos5, Pos6, Pos7, Pos8, Pos9, Pos10, DeskId, MaxCircle, CurCircle, ";
		ss << "Score1, Score2, Score3, Score4, Score5, Score6, Score7, Score8, Score9, Score10, DeskPlayType, OptData ";
		ss << "FROM qipaiplayinfo WHERE ( ClubId=";
		ss << msg->m_clubId << " ) ";
		ss << " AND (CurCircle = MaxCircle OR IsDismiss = 1) ";
		if (msg->m_time > 0)
		{
			ss << " AND Time <" << msg->m_time;
		}
		ss << " ORDER BY Time DESC LIMIT 10";
	}
	else {
		ss << "SELECT ";
		ss << "Id, Time, ClubId, DeskFlag, CreateUserId, Pos1, Pos2, Pos3, Pos4, Pos5, Pos6, Pos7, Pos8, Pos9, Pos10, DeskId, MaxCircle, CurCircle, ";
		ss << "Score1, Score2, Score3, Score4, Score5, Score6, Score7, Score8, Score9, Score10, DeskPlayType, OptData ";
		ss << "FROM qipaiplayinfo WHERE (Pos1=";
		ss << msg->m_reqUserId << " OR Pos2=";
		ss << msg->m_reqUserId << " OR Pos3=";
		ss << msg->m_reqUserId << " OR Pos4=";
		ss << msg->m_reqUserId << " OR Pos5=";
		ss << msg->m_reqUserId << " OR Pos6=";
		ss << msg->m_reqUserId << " OR Pos7=";
		ss << msg->m_reqUserId << " OR Pos8=";
		ss << msg->m_reqUserId << " OR Pos9=";
		ss << msg->m_reqUserId << " OR Pos10=";
		ss << msg->m_reqUserId << " ) ";
		//ss<<" AND DeskFlag="<<msg->m_flag;
		ss << " AND DeskFlag in (" << 102;
		ss << "," << 104;
		ss << "," << 101;
		ss << "," << 103;
		ss << "," << 106;  //扑克升级战绩
		ss << "," << 107;  //扑克三打二战绩
		ss << "," << 108;  //新版牛牛
		ss << "," << 109;  //扑克三打一战绩
		ss << "," << 110;  //五人百分战绩
		ss << "," << 111;	//扯炫
		ss << ") ";
		ss << " AND (CurCircle = MaxCircle OR IsDismiss = 1) ";
		if (msg->m_clubId != 0)
			ss << "  AND ClubId=  " << msg->m_clubId;
		ss << " ORDER BY Time DESC LIMIT 10";
	}
	
	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Query sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}
	
	MYSQL_RES* res=mysql_store_result(pMySQL);
	MYSQL_ROW row=mysql_fetch_row(res);
	while (row)
	{
		LogPokerInfo& info=log.m_info[log.m_count++];
		info.m_id = *row++;
		info.m_time = atoi(*row++);
		info.m_clubId = atoi(*row++);
		info.m_flag = atoi(*row++);
		info.m_createUserId = atoi(*row++);
		info.m_posUserId[0] = atoi(*row++);
		info.m_posUserId[1] = atoi(*row++);
		info.m_posUserId[2] = atoi(*row++);
		info.m_posUserId[3] = atoi(*row++);
		info.m_posUserId[4] = atoi(*row++);
		info.m_posUserId[5] = atoi(*row++);
		info.m_posUserId[6] = atoi(*row++);
		info.m_posUserId[7] = atoi(*row++);
		info.m_posUserId[8] = atoi(*row++);
		info.m_posUserId[9] = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);
		info.m_score[4] = atoi(*row++);
		info.m_score[5] = atoi(*row++);
		info.m_score[6] = atoi(*row++);
		info.m_score[7] = atoi(*row++);
		info.m_score[8] = atoi(*row++);
		info.m_score[9] = atoi(*row++);
		if (row + 1)
		{
			info.m_playtype = *row++;   //玩法小选项
		}
		if (row + 1)
		{
			info.m_optData = *row++;	//牛牛每场总抢庄、坐庄、推注次数数据
		}
	
		row=mysql_fetch_row(res);
		if (log.m_count>=10)   //每次返回10条
			break;
	}
	
	mysql_free_result(res);
	
	LLOG_DEBUG("CUserMessage::HanderLogicReqPokerLog m_allclub[%d]  m_clubid[%d]  m_time[%d]  count[%d]", msg->m_allClub, msg->m_clubId, msg->m_time, log.m_count);
	msg->m_sp->Send(log.GetSendBuff());
}

void CUserMessage::HanderLogicReqCRELog( LMsgL2LBDReqCRELog* msg )
{
	LLOG_DEBUG("CUserMessage::Request CRE log. %d ------",msg->m_userId);

	LMsgLDB2LM_RecordCRELog msgRet;
	msgRet.m_strUUID = msg->m_strUUID;
	msgRet.m_userId = msg->m_userId;

	char name[64] = {0};  
	sprintf(name, "CREDIT_USER_%d",msg->m_userId);

	Json::Reader reader(Json::Features::strictMode());
	Json::Value value;

	std::set<Lint> userList;

	std::list<Lstring> temp;
	if(m_RedisClient.readDataByList(std::string(name),0,30,temp))
	{
		m_RedisClient.setKeyTTLTime(std::string(name),CREDIT_KEY_TTL_TIME);    //Update TTL time of the key
		for(auto it = temp.begin(); it != temp.end(); ++it)
		{
			Lstring & strValue = *it;
			msgRet.m_count++;
			msgRet.m_record.push_back(strValue);

			try
			{
				if (reader.parse(strValue, value))
				{
					Lint len = value["User"].size();
					for(Lint i = 0; i < len; ++i)
					{
						Lint id = value["User"][i].asInt();
						userList.insert(id);
					}
				}
			}
			catch (...)
			{
				LLOG_ERROR("exception -- json parse error!!!");
			}
		}

		Lint index = 0;
		for(auto it = userList.begin(); it != userList.end(); ++it)
		{
			msgRet.m_user[index++] = *it;
			if(index >= LMsgLDB2LM_RecordCRELog::Length)
			{
				break;
			}
		}
	}
	else
	{
		LLOG_ERROR("readDataByList error!!!");
	}

	msg->m_sp->Send(msgRet.GetSendBuff());
}

void CUserMessage::HanderLogicDelCRELog( LMsgL2LDBDEL_GTU* msg )
{
	char name[64] = {0};  
	sprintf(name, "CREDIT_USER_%d",msg->m_userId);

	bool ret = m_RedisClient.rmListDataByValue(std::string(name),msg->m_strLog);
	if(ret)
	{
		LLOG_DEBUG("Request Del CRE log. ------succeed..");
	}
	else
	{
		LLOG_DEBUG("Request Del CRE log.  ------failed..");
	}
}

void CUserMessage::HanderLogicReqSaveLog( LMsgL2LDBSaveLogItem* msg )
{
	if(msg)
	{
		LLOG_DEBUG("Save log. %d:%s",msg->m_type,msg->m_sql.c_str());
		Excute(msg->m_sql);
	}
}

void CUserMessage::HanderLogicReqSaveVideo( LMsgL2LDBSaveVideo* msg )
{
	if(msg)
	{
		LLOG_DEBUG("Work::HanderLogincSaveVideo save %d:%s", msg->m_type, msg->m_sql.c_str());
		Excute(msg->m_sql);
	}

}

bool CUserMessage::Excute( const std::string& str )
{

	if( m_dbsession  || m_dbsessionBK)
	{
		MYSQL* mysql = NULL;
		

		//m_dbsession->GetMysql();
		//if(m_dbsession)
		//{
		//	pMySQL = m_dbsession->GetMysql();
		//}
		mysql = this->GetMySQL();

		if(mysql == NULL)
		{
			LLOG_ERROR("DbServer::Excute MYSQL NULL");
			return false;
		}
		if(mysql_real_query(mysql,str.c_str(),str.size()))
		{
			LLOG_ERROR("DbServer::Excute error = %s,sql = %s ", mysql_error(mysql),str.c_str());
			return false;
		}
		return true;
	}
	else
	{
		LLOG_ERROR("DbServer::Excute dbsession is NULL, sql = %s ",str.c_str());
	}
	return false;
}

bool CUserMessage::Init()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
	}
	if (m_dbsessionBK)
	{
		delete m_dbsessionBK;
		
	}
	m_dbsessionBK = new LDBSession;
	m_dbsessionBK->Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort());

	m_dbsession = new LDBSession;
	return m_dbsession->Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort());


}

bool CUserMessage::Final()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
		m_dbsession = NULL;
	}

	if (m_dbsessionBK)
	{
		delete m_dbsessionBK;
		m_dbsessionBK = NULL;
	}
	return true;
}

void CUserMessage::HanderLogicReqRoomLog(LMsgL2LDBReqRoomLog * msg)
{
	if(NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("Request room Log. userid = %d : deskid = %d : time = %d : pos = %d",msg->m_userId,msg->m_deskId,msg->m_time,msg->m_pos);
	
	LMsgLBD2LReqRoomLog log;
	log.m_strUUID= msg->m_strUUID;
	log.m_userid = msg->m_userId;

	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if(pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	std::stringstream ss;	
	ss << "SELECT Flag,Data FROM log WHERE (Time=";
	ss << msg->m_time << " AND Pos1=";
	ss << msg->m_pos << " AND DeskId=";
	ss << msg->m_deskId << " )";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Query sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return ;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row)
	{
		log.m_flag = atoi(*row++);
		log.m_data = *row++;
	}
	mysql_free_result(res);
	
	msg->m_sp->Send(log.GetSendBuff());
}

void CUserMessage::HanderLogicSaveCRELog( LMsgL2LDBSaveCRELog* msg )
{
	Json::Value root;
	root["DeskId"] = msg->m_deskID;
	root["LogId"] = msg->m_strLogId;
	root["Time"] = msg->m_time;
	for(Lint i = 0; i < 4; ++i)
	{
		root["User"].append(msg->m_id[i]);
	}

	std::string strValue = root.toStyledString();

	for(Lint j = 0; j < 4; ++j)
	{
		if(msg->m_id[j] <= 0)
			continue;

		char name[64] = {0};
		sprintf(name, "CREDIT_USER_%d",msg->m_id[j]);
		bool ret = m_RedisClient.writeDataByList(std::string(name),strValue,CREDIT_HISTORY_LIMIT);  ///如果超过20条，删除过时的记录
		if(ret)
		{
			m_RedisClient.setKeyTTLTime(std::string(name),CREDIT_KEY_TTL_TIME);
		}
		else
		{
			LLOG_DEBUG("HanderLogicSaveCRELog Failed !!!");
		}
	}
}

void CUserMessage::HanderUserLogin(LMsgLMG2LdbUserLogin* msg)
{
	// 活动信息
	LMsgS2CActivityInfo active;

	// 填充转盘信息
	if ( gActiveManager.CheckDrawActiveValid() )
	{
		LMsgS2CActivityInfo::Info draw;
		draw.m_id = gActiveManager.GetDrawActiveId();
		char time[16];
		sprintf( time, "%d", gActiveManager.GetDrawBeginTime() );
		draw.m_attri.push_back( LMsgS2CActivityInfo::Value() );
		draw.m_attri.back().m_key = "BeginTime";
		draw.m_attri.back().m_value = time;
		sprintf( time, "%d", gActiveManager.GetDrawEndTime() );
		draw.m_attri.push_back( LMsgS2CActivityInfo::Value() );
		draw.m_attri.back().m_key = "EndTime";
		draw.m_attri.back().m_value = time;
		active.m_activities.push_back(draw);
	}

	// 填充分享信息
	if(gActiveManager.CheckShareActiveStart())  //分享活动开启
	{
		LMsgS2CActivityInfo::Info share;
		share.m_id = gActiveManager.GetShareActiveId();
		share.m_info = gActiveManager.GetShareActiveSet();

		char name[64] = {0};
		sprintf(name, "ActivityShare_userid_%d",msg->m_userId);

		bool suc = false;
		int time = 0;
		bool ret = m_RedisClient.readDataByString(std::string(name),time);
		if(ret)
		{
			if(time != 0)
			{
				LTime lastTime;
				int curDay = lastTime.GetDate();
				lastTime.SetSecs(time);
				int lastDay = lastTime.GetDate();
				if(curDay != lastDay)
					suc = true;
			}
			else
			{
				suc = true;
			}
		}

		LMsgS2CActivityInfo::ATTRI attri;
		attri.m_key = "canshare";
		attri.m_value = "0";
		if(suc)
		{
			attri.m_value = "1";
		}

		share.m_attri.push_back(attri);
		active.m_activities.push_back(share);
	}
	
	//gWork.SendToUser( msg->m_strUUID, active );
}

void CUserMessage::HanderActivityPhone(LMsgC2SActivityPhone *msg)
{
	// 保存手机号
	std::stringstream ss;	
	ss << "UPDATE activitylog SET Phone = '" << msg->m_PhoneNum << "' WHERE";
	ss << " AutoId = " << msg->m_AutoId;
	ss << " AND UserId = " << msg->m_userId;

	LMsgS2CActivityPhone send;
	if ( Excute( ss.str() ) )
	{
		send.m_errorCode = 0;
	}
	else
	{
		send.m_errorCode = 1;
	}
	gWork.SendToUser( msg->m_strUUID, send );
}

void CUserMessage::HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg)
{
	LMsgS2CActivityReplyLog send;
	send.m_errorCode = -1;
	if ( msg->m_activeId == ActivityID_Draw )
	{
		// 从redis里查询
		std::list<std::string> logs;
		std::ostringstream str;
		str << ActivityDraw_Win_ << msg->m_userId;
		m_RedisClient.readDataByList( str.str(), 0, ActivityDraw_WinCount, logs );

		for ( auto it = logs.begin(); it != logs.end(); ++it )
		{
			Json::Reader reader(Json::Features::strictMode());
			Json::Value  value;
			if (!reader.parse(*it, value))
			{
				LLOG_ERROR("CUserMessage::HanderActivityRequestLog draw parsr json error %s", reader.getFormatedErrorMessages().c_str());
				continue;
			}
			send.m_logs.push_back(  ActivityLog() );
			ActivityLog& log = send.m_logs.back();
			log.AutoId = value["AutoId"].isInt() ? value["AutoId"].asInt() : 0;
			log.Id = value["Id"].isInt() ? value["Id"].asInt() : 0;
			log.SpendType = value["SpendType"].isInt() ? value["SpendType"].asInt() : 0;
			log.SpendCount = value["SpendCount"].isInt() ? value["SpendCount"].asInt() : 0;
			log.GiftName = value["GiftName"].isString() ? value["GiftName"].asString() : "";
			log.GiftType = value["GiftType"].isInt() ? value["GiftType"].asInt() : 0;
			log.GiftCount = value["GiftCount"].isInt() ? value["GiftCount"].asInt() : 0;
			log.Date = value["Date"].isInt() ? value["Date"].asInt() : 0;
			log.Phone = value["Phone"].isString() ? value["Phone"].asString() : "";
		}
	}
	else
	{
		// 从数据库里查询
		std::stringstream ss;
		ss << "SELECT AutoId, Id, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date, Phone FROM activitylog WHERE";
		if ( msg->m_activeId != 0 )
		{
			ss << " Id = " << msg->m_activeId << " AND";
		}
		ss << " UserId = " << msg->m_userId;

		MYSQL* m = NULL;
		//if(m_dbsession)
		//{
		//	pMySQL = m_dbsession->GetMysql();
		//}
		m = this->GetMySQL();

		if ( m )
		{
			if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
			{
				send.m_errorCode = 0;
				MYSQL_RES* res = mysql_store_result(m);
				if (res)
				{
					MYSQL_ROW row = mysql_fetch_row(res);
					int iCount = 0;
					while (row)
					{
						send.m_logs.push_back(  ActivityLog() );
						ActivityLog& log = send.m_logs.back();

						log.AutoId = atoi(*row++);
						log.Id = atoi(*row++);
						log.SpendType = atoi(*row++);
						log.SpendCount = atoi(*row++);
						log.GiftName = *row++;
						log.GiftType = atoi(*row++);
						log.GiftCount = atoi(*row++);
						log.Date = atoi(*row++);
						log.Phone = *row++;

						row = mysql_fetch_row(res);

						iCount++;
						if(iCount >= ActivityDraw_WinCount)
						{
							break;
						}
					}
					mysql_free_result(res);
				}
			}
		}
	}
	gWork.SendToUser( msg->m_strUUID, send );
}

void CUserMessage::HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg)
{
	LMsgS2CActivityReplyDrawOpen active;
	// 读取中奖的玩家
	if ( gActiveManager.CheckDrawActiveValid() )
	{
		active.m_errorCode = 0;
		active.m_SpendType = gActiveManager.GetDrawSpendType();
		active.m_SpendCount = gActiveManager.GetDrawSpendCount();

		std::list<std::string> winusers;
		m_RedisClient.readDataByList( ActivityDraw_WinUsers, 0, ActivityDraw_WinUsersCount, winusers );

		for ( auto it = winusers.begin(); it != winusers.end(); ++it )
		{
			active.m_winUsers.push_back( ActivityDrawWinUser() );

			ActivityDrawWinUser& user = active.m_winUsers.back();
			size_t pos = it->find( "|" );
			if ( pos != std::string::npos )
			{
				user.m_userId = it->substr(0,pos);
				user.m_giftName = it->substr(pos+1);
			}
		}
	}
	else
	{
		active.m_errorCode = 1;
	}
	gWork.SendToUser( msg->m_strUUID, active );
}

void CUserMessage::HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg)
{
	LMsgS2CActivityReplyDraw active;
	if ( !gActiveManager.CheckDrawActiveValid() )
	{
		active.m_errorCode = 2;
		gWork.SendToUser( msg->m_strUUID, active );
		return;
	}
	
	ActivityDrawGift fGift;
	ActivityDrawGift gift = gActiveManager.GetDrawGift( fGift );

	// 消耗
	Lint spendtype = gActiveManager.GetDrawSpendType();
	Lint spendcount = gActiveManager.GetDrawSpendCount();

	if ( spendtype == 1 && spendcount > msg->m_cardNum )
	{
		active.m_errorCode = 1;
	}
	else if ( gift.m_GiftIndex != -1 )
	{
		// 抽奖成功
		active.m_errorCode = 0;

		active.m_GiftIndex = gift.m_GiftIndex;
		active.m_GiftName = gift.m_GiftName;
		active.m_GiftType = gift.m_GiftType;
		active.m_GiftCount = gift.m_GiftCount;
		if ( spendtype == 1 )	// 房卡
		{
			LMsgL2LMGModifyCard modify;
			modify.admin = "system";
			modify.cardType = CARD_TYPE_Other;
			modify.cardNum = spendcount;
			modify.isAddCard = 0;
			modify.operType = CARDS_OPER_TYPE_ACTIVE_DRAW;
			modify.m_userid = msg->m_userId;
			modify.m_strUUID = msg->m_strUUID;
			gWork.SendToLogicManager( modify );
		}

		// 奖励
		if ( gift.m_GiftType == 1 )
		{
			LMsgL2LMGModifyCard modify;
			modify.admin = "system";
			modify.cardType = CARD_TYPE_Other;
			modify.cardNum = gift.m_GiftCount;
			modify.isAddCard = 1;
			modify.operType = CARDS_OPER_TYPE_ACTIVE;
			modify.m_userid = msg->m_userId;
			modify.m_strUUID = msg->m_strUUID;
			gWork.SendToLogicManager( modify );
		}

		// mysql记录中间信息
		LTime cur;
		MYSQL* m = NULL;
		//if(m_dbsession)
		//{
		//	pMySQL = m_dbsession->GetMysql();
		//}
		m = this->GetMySQL();

		if ( m )
		{
			std::stringstream ss;	
			ss << "insert into activitylog(Id, UserId, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date) values(";
			ss << gActiveManager.GetDrawActiveId() << ",";
			ss << msg->m_userId << ",";
			ss << spendtype << ",";
			ss << spendcount << ",";
			ss << "'" << gift.m_GiftName << "',";
			ss << gift.m_GiftType << ",";
			ss << gift.m_GiftCount << ",";
			ss << cur.Secs() << ")";

			if (mysql_real_query(m, ss.str().c_str(), ss.str().size()) == 0)
			{
				// 如果奖励的类型不是房卡 让玩家填写手机号
				if ( gift.m_GiftType != 1 )
				{
					active.m_NeedPhoneNum = true;
				}
				active.m_AutoId = (Lint)mysql_insert_id(m);
			}
		}

		// redis记录中奖信息
		{
			Json::Value root;   // 构建对象 
			root["AutoId"] = active.m_AutoId;
			root["Id"] = msg->m_userId;
			root["SpendType"] = spendtype;
			root["SpendCount"] = spendcount;
			root["GiftName"] = gift.m_GiftName;
			root["GiftType"] = gift.m_GiftType;
			root["GiftCount"] = gift.m_GiftCount;
			root["Date"] = cur.Secs();
			root["Phone"] = "";

			std::ostringstream str;
			str << ActivityDraw_Win_ << msg->m_userId;
			m_RedisClient.writeDataByList( str.str(), root.toStyledString(), ActivityDraw_WinCount );
			m_RedisClient.setKeyTTLTime( str.str(), gActiveManager.GetDrawEndTime()-cur.Secs() );
		}

		// 前WinUsersLogCount的中奖纪录
		{
			std::ostringstream str;
			str << "***";
			str << msg->m_userId%1000;
			str << "|";
			str << gift.m_GiftName;
			m_RedisClient.writeDataByList( ActivityDraw_WinUsers, str.str(), ActivityDraw_WinUsersCount );
			m_RedisClient.setKeyTTLTime( ActivityDraw_WinUsers, gActiveManager.GetDrawEndTime()-cur.Secs() );
		}

		// 假数据
		if ( fGift.m_GiftIndex != -1 )
		{
			std::ostringstream str;
			str << "***";
			str << L_Rand( 100, 999 );
			str << "|";
			str << fGift.m_GiftName;
			m_RedisClient.writeDataByList( ActivityDraw_WinUsers, str.str(), ActivityDraw_WinUsersCount );
		}
	}
	else
	{
		active.m_errorCode = -1;
	}

	gWork.SendToUser( msg->m_strUUID, active );
}

void CUserMessage::HanderActivityRequestShare( LMsgC2SActivityRequestShare *msg )
{
	LMsgS2CActivityFinishShare share;
	share.m_ErrorCode = -1;
	share.m_GiftCount = -1;
	share.m_GiftType = -1;

	if(!gActiveManager.CheckShareActiveStart())
	{
		//活动未开启，不在活动时间内
		gWork.SendToUser(msg->m_strUUID,share);
		return;
	}

	char name[64] = {0};
	sprintf(name, "ActivityShare_userid_%d",msg->m_userId);

	bool suc = false;
	int time = 0;
	bool ret = m_RedisClient.readDataByString(std::string(name),time);
	if(ret)
	{
		if(time != 0)
		{
			LTime lastTime;
			int curDay = lastTime.GetDate();
			lastTime.SetSecs(time);
			int lastDay = lastTime.GetDate();
			if(curDay != lastDay)
			{
				suc = true;
			}
			//else LLOG_DEBUG("HanderLogicShareActive user = %d last share time = %d",userId,time);
		}
		else
		{
			suc = true;
		}
	}

	share.m_ErrorCode = 0;
	share.m_GiftCount = 0;
	share.m_GiftType = 1;

	if(suc)
	{
		const ActivityShare set = gActiveManager.GetShareSet();

		if ( set.m_GiftType == 1 )	// 只有房卡
		{
			LMsgL2LMGModifyCard modify;
			modify.admin = "system";
			modify.cardType = CARD_TYPE_Other;
			modify.cardNum = set.m_GiftCount;
			modify.isAddCard = 1;
			modify.operType = CARDS_OPER_TYPE_ACTIVE;
			modify.m_userid = msg->m_userId;
			modify.m_strUUID = msg->m_strUUID;
			gWork.SendToLogicManager( modify );
		}

		share.m_ErrorCode = 0;  //成功
		share.m_GiftCount = set.m_GiftCount;  //数量
		share.m_GiftType = set.m_GiftType;   //房卡

		LTime cur;
		ret = m_RedisClient.writeDataByString(std::string(name),cur.Secs());
		if(ret)
		{
			m_RedisClient.setKeyTTLTime(std::string(name),(24*60*60));
		}
	}

	gWork.SendToUser(msg->m_strUUID,share);

	LLOG_DEBUG("HanderLogicShareActive user = %d last share card = %d",msg->m_userId,share.m_GiftCount);
}

bool CUserMessage::_initRedisConnect()
{
	return m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort());
}

//////////////////////////////////共享录像相关
void CUserMessage::HanderLogicReqShareVideo(LMsgL2LDBGetShareVideo* msg)
{
	if(msg==NULL)
	{
		return;
	}
	LMsgLDB2LRetShareVideo log;
	log.m_strUUID= msg->m_strUUID;
	log.m_userid = msg->m_userId;

	MYSQL* m = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	m = this->GetMySQL();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT VideoId,Time,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4 FROM sharevideo WHERE ShareID=";
	ss << msg->m_shareId;

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqShareVideo sql error %s", mysql_error(m));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		ShareVideoInfo info;
		info.m_videoId = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);
		if (info.m_userId[3] == 0 && info.m_userId[2] == 0)
		{
			info.m_user_count = 2;
		}
		else if (info.m_userId[3] == 0)
		{
			info.m_user_count = 3;
		}
		log.m_count++;
		log.m_info.push_back(info);
		row = mysql_fetch_row(res);
	}
	mysql_free_result(res);
	msg->m_sp->Send(log.GetSendBuff());
}

void CUserMessage::HanderLogicGetShareVideoId(LMsgL2LDBGGetShareVideoId* msg)
{
	if(msg==NULL)
	{
		return;
	}
	LMsgLDB2LRetShareVideoId sharevideo_id;
	sharevideo_id.m_strUUID= msg->m_strUUID;
	sharevideo_id.m_userId = msg->m_userId;
	sharevideo_id.m_videoId = msg->m_videoId;
	//加载所有的玩家

	MYSQL* m = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	m = this->GetMySQL();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		msg->m_sp->Send(sharevideo_id.GetSendBuff());
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT ShareId FROM sharevideo WHERE VideoId='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 1 _ sql error %s", mysql_error(m));
		msg->m_sp->Send(sharevideo_id.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		sharevideo_id.m_shareId = atoi(*row++);
	}
	mysql_free_result(res);

	if(sharevideo_id.m_shareId==0)
	{
		std::stringstream ss;	
		ss << "SELECT UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,Time,Zhuang,Data,PlayType,DeskId,Flag,CurCircle,MaxCircle FROM video WHERE Id='";
		ss << msg->m_videoId<<"'";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 2 _ sql error %s", mysql_error(m));
			msg->m_sp->Send(sharevideo_id.GetSendBuff());
			return;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		Lint userId[4];
		Lint Score[4];
		Lint time;
		Lint zhuang;
		Lstring data;
		Lstring playtype;
		Lint deskid;
		Lint flag;
		Lint curcircle;
		Lint maxcircle;

		if (row)
		{
			userId[0]= atoi(*row++);
			userId[1]= atoi(*row++);
			userId[2]= atoi(*row++);
			userId[3]= atoi(*row++);
			Score[0] = atoi(*row++);
			Score[1] = atoi(*row++);
			Score[2] = atoi(*row++);
			Score[3] = atoi(*row++);
			time = atoi(*row++);
			zhuang = atoi(*row++);
			data = *row++;
			playtype = *row++;
			deskid = atoi(*row++);
			flag = atoi(*row++);
			curcircle = atoi(*row++);
			maxcircle = atoi(*row++);

			std::stringstream ss1;	
			ss1 << "insert into sharevideo(VideoId,time,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,Zhuang,Data,PlayType,DeskId,Flag,MaxCircle,CurCircle) values('";
			ss1 << msg->m_videoId.c_str() <<"',"<<time<<","<<userId[0]<<","<<userId[1]<<","<<userId[2]<<","<<userId[3]<<","<<Score[0]<<","<<Score[1];
			ss1 << ","<<Score[2]<<","<<Score[3] << ","<<zhuang <<",'"<< data.c_str() <<"','" << playtype.c_str() <<"'," << deskid << "," << flag << "," << maxcircle <<"," << curcircle <<")";

			LLOG_DEBUG("%s", ss1.str().c_str());
			Excute(ss1.str());
		}
		else{
			sharevideo_id.m_shareId = -1;
		}

		mysql_free_result(res);

	}
	msg->m_sp->Send(sharevideo_id.GetSendBuff());
}

void CUserMessage::HanderLogicSaveShareVideoId(LMsgL2LDBSaveShareVideoId* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_videoId != "" )
	{
		//log id
		std::stringstream ss;
		ss << "update sharevideo set ShareId = "<< msg->m_shareId <<" where Videoid = '"<< msg->m_videoId << "' ";

		LLOG_DEBUG("%s", ss.str().c_str());
		Excute(ss.str());
	}
}



void CUserMessage::HanderLogicGetShareVideo(LMsgL2LDBGetSsharedVideo* msg)
{
	LMsgLDB2LReqVideo log;
	log.m_strUUID= msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;


	MYSQL* m = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	m = this->GetMySQL();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT VideoId,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data,PlayType FROM sharevideo WHERE VideoId='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideo sql error %s", mysql_error(m));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);
		if (info.m_userId[3] == 0 && info.m_userId[2] == 0)
		{
			//log.m_userCount = 2;
		}
		else if (info.m_userId[3] == 0)
		{
			//log.m_userCount = 3;
		}
		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		info.m_maxCircle = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;

		info.m_str_playtype = *row++;
		log.m_count = 1;
	}

	mysql_free_result(res);
	msg->m_sp->Send(log.GetSendBuff());
}


void CUserMessage::HanderLogicGetUserPlayData(LMsgL2LDBGetUserPlayData* msg)
{

	LMsgLDB2LGetUserPlayData log;
	log.m_strUUID = msg->m_strUUID;
	log.m_userId = msg->m_userId;
	log.m_allAround = 0;
	log.m_winAround = 0;

	gPlayData.GetUserPlayTotalAround(msg, log.m_allAround, log.m_winAround);

	LLOG_DEBUG("Work::HanderLogicGetUserPlayData");
	msg->m_sp->Send(log.GetSendBuff());
	return;

}


void CUserMessage::HanderLogicUpdateUserPlayData(LMsgL2LDBUpdatePlayerScore* msg)
{

	LMsgLDB2LGetUserPlayData log;
	log.m_strUUID = msg->m_strUUID;
	log.m_userId = msg->m_userId;
	log.m_allAround = 0;
	log.m_winAround = 0;

	gPlayData.UpdateUserPlayScore(msg, log.m_allAround, log.m_winAround);

	LLOG_DEBUG("Work::HanderLogicGetUserPlayData");
	msg->m_sp->Send(log.GetSendBuff());
	return;

}

void CUserMessage::HandlerLogicQueryShareVideoByShareId2(MHLMsgL2LDBQueryShareVideoByShareId * msg)
{
	MHLMsgLDB2LShareVideoResult log;
	log.m_strUUID = msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();


	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MHLOG("****Process user request sharevideo msg. userid:%d, uuid:%s videoid= %s", msg->m_userId, log.m_strUUID.c_str(), msg->m_videoId.c_str());
	//log id
	std::stringstream ss;
	ss << "SELECT Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data, PlayType FROM video WHERE Id='";
	ss << msg->m_videoId << "'";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("CUserMessage::HandlerLogicQueryShareVideoByShareId2 sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;

		info.m_str_playtype = *row++;
		log.m_count = 1;
	}

	mysql_free_result(res);

	msg->m_sp->Send(log.GetSendBuff());
	MHLOG("*****End query sharevideo. userid:%d, uuid:%s videoid:%s, count:%d", msg->m_userId, log.m_strUUID.c_str(), msg->m_videoId.c_str(), log.m_count);
	return;
}

void CUserMessage::InsertShareVideoRecordByVideoRecord(Lstring & str_videoid, Lstring & str_shareid )
{

	//LMsgLDB2LRetShareVideo log;
	//log.m_strUUID = msg->m_strUUID;
	//log.m_userid = msg->m_userId;	 
	return;
}

MYSQL * CUserMessage::GetMySQL()
{
	if (gWork.GetDBMode())
	{
		LLOG_DEBUG("PUSH SQL TO MANSTER MYSQL");
		if(m_dbsession) return m_dbsession->GetMysql();
	}
	else
	{
		LLOG_DEBUG("PUSH SQL TO BAK MYSQL");
		if (m_dbsessionBK) return m_dbsessionBK->GetMysql();
	}
}

//请求回放：第二步，返回playinfo.ScoreData，确定的桌子中每局的粗略
void CUserMessage::HanderLogicQiPaiReqRoomLog(LMsgL2LDBReqRoomLog * msg)
{
	if (NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("HanderLogicQiPaiReqRoomLog. userid = %d : deskid = %d : time = %d : pos = %d", msg->m_userId, msg->m_deskId, msg->m_time, msg->m_pos);

	LMsgLBD2LReqRoomLog log;
	log.m_strUUID = msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_type = 1;

	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	std::stringstream ss;
	//ss << "SELECT Flag,Data FROM log WHERE (Time=";
	//ss << msg->m_time << " AND Pos1=";
	//ss << msg->m_pos << " AND DeskId=";
	//ss << msg->m_deskId << " )";
	ss << "SELECT DeskFlag,ScoreData FROM qipaiplayinfo WHERE ";
	ss << "Id = " << msg->m_id;
	/*
	ss << "(Pos1=";
	ss << msg->m_userId << " OR Pos2=";
	ss << msg->m_userId << " OR Pos3=";
	ss << msg->m_userId << " OR Pos4=";
	ss << msg->m_userId << " OR Pos5=";
	ss << msg->m_userId << " OR Pos6=";
	ss << msg->m_userId << " OR Pos7=";
	ss << msg->m_userId << " OR Pos8=";
	ss << msg->m_userId << " OR Pos9=";
	ss << msg->m_userId << " OR Pos10=";
	ss << msg->m_userId << ")  and DeskId=";
	ss << msg->m_deskId;
	*/

	LLOG_DEBUG("HanderLogicQiPaiReqRoomLog  sql[%s]",ss.str().c_str());
	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Query sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		log.m_flag = atoi(*row++);
		log.m_data = *row++;
	}
	mysql_free_result(res);

	LLOG_DEBUG("HanderLogicQiPaiReqRoomLog flag=[%d],data[%s]",log.m_flag,log.m_data.c_str());
	msg->m_sp->Send(log.GetSendBuff());
}

//扑克请求玩家录像
void CUserMessage::HanderLogicQiPaiReqVideo(LMsgL2LDBReqVideo* msg)
{
	LLOG_DEBUG("HanderLogicQiPaiReqVideo");
	if (msg == NULL)
		return;

	LMsgLDB2LPokerReqVideo log;
	log.m_strUUID = msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	LLOG_ERROR("Request qipai video. %d:%s", msg->m_userId, msg->m_videoId.c_str());
	//log id
	std::stringstream ss;
	ss << "SELECT Id,Time,UserId1,UserId2,UserId3,UserId4,UserId5,UserId6,UserId7,UserId8,UserId9,UserId10,Zhuang,DeskId,MaxCircle,CurCircle,";
	ss << "Score1,Score2,Score3,Score4,Score5,Score6,Score7,Score8,Score9,Score10,Flag,Data,PlayType,fuzhuang FROM video WHERE Id='";
	ss << msg->m_videoId << "'";

	LLOG_DEBUG("HanderLogicQiPaiReqVideo  SQL[%s]",ss.str().c_str());

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqVideo sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		QiPaiVideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);
		info.m_userId[4] = atoi(*row++);
		info.m_userId[5] = atoi(*row++);
		info.m_userId[6] = atoi(*row++);
		info.m_userId[7] = atoi(*row++);
		info.m_userId[8] = atoi(*row++);
		info.m_userId[9] = atoi(*row++);

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);
		info.m_score[4] = atoi(*row++);
		info.m_score[5] = atoi(*row++);
		info.m_score[6] = atoi(*row++);
		info.m_score[7] = atoi(*row++);
		info.m_score[8] = atoi(*row++);
		info.m_score[9] = atoi(*row++);

		info.m_flag = atoi(*row++);
		info.m_str = *row++;
		info.m_str_playtype = *row++;
		if (row != NULL && *row)
		{
			info.m_fuZhuang = atoi(*row++);
		}
		else
		{
			*row++;
		}


		log.m_count = 1;
	}
	
	mysql_free_result(res);

	LLOG_DEBUG("HanderLogicReqVideo  str[%s],playtype=[%s]",log.m_video.m_str.c_str(), log.m_video.m_str_playtype.c_str());

	msg->m_sp->Send(log.GetSendBuff());
}
