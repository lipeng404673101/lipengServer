#include "clubPlay.h"
#include "Work.h"

/* 解析Json格式俱乐部玩法详情 */
bool ClubPlayInfo::parsePlayType(Lint playTypeId, const Lstring playType)
{
	m_playTypeString = playType;
	Json::Reader reader(Json::Features::strictMode());
	Json::Value value;
	try
	{
		if (reader.parse(m_playTypeString, value))
		{
			m_detail.m_id = playTypeId;
			m_detail.m_name = value["play_type_name"].asString().c_str();
			m_detail.m_cellscore = value["base_score"].asInt();
			m_detail.m_flag = value["round_count"].asInt();
			//m_detail.m_feeType = m_feeType;
			m_detail.m_cheatAgainst = value["is_vip"].asBool();
			m_detail.m_state = value["play_type"].asInt();
			m_detail.m_GpsLimit = value["gps_limit"].asInt();
			m_detail.m_allowLookOn = value["allow_look"].asInt();
			m_detail.m_clubOwerLookOn = value["club_ower_look"].asInt();
			m_detail.m_minPointInRoom = value["join_score"].asInt();
			m_detail.m_minPointXiaZhu = value["rob_score"].asInt();
			m_detail.m_smallZero = value["can_minus"].asInt();
			m_detail.m_warnScore = value["warn_score"].asInt();

			// GPS测试，这里暂时设置为1
			if (value["user_count"].asInt() == 1)
				m_detail.m_Greater2CanStart = 1;       // value["user_count"].asInt();
			else
				m_detail.m_Greater2CanStart = 0;

			const Json::Value& arrayObj = value["play_rule"];
			for (unsigned int i = 0; i<arrayObj.size(); i++)
			{
				m_detail.m_playtype.push_back(arrayObj[i].asInt());
				LLOG_DEBUG("play_rule[%d]=[%d]", i, m_detail.m_playtype[i]);
			}
		}
		else
		{
			LLOG_ERROR("Fail to parse playType");
			return false;
		}
	}
	catch (...)
	{
		LLOG_ERROR("exc exception -- json parse error!!!");
		return false;
	}

	LLOG_DEBUG("parsePlayType m_id=[%d], m_name=[%s],m_cellscore=[%d],m_flag=[%d],m_feeType=[%d],m_cheatAgainst=[%d],m_state=[%d],m_Greater2CanStart=[%d],gps_limit:%d,m_clubOwerLookOn[%d],m_minPointInRoom[%d],m_minPointXiaZhu[%d],m_smallZero[%d]",
		m_detail.m_id,
		m_detail.m_name.c_str(),
		m_detail.m_cellscore,
		m_detail.m_flag,
		m_detail.m_feeType,
		m_detail.m_cheatAgainst,
		m_detail.m_state,
		m_detail.m_Greater2CanStart,
		m_detail.m_GpsLimit,
		m_detail.m_clubOwerLookOn,
		m_detail.m_minPointInRoom,
		m_detail.m_minPointXiaZhu,
		m_detail.m_smallZero,
		m_detail.m_warnScore
	);
	return true;
}

/* 拼写Json格式俱乐部玩法详情 */
Lstring  ClubPlayInfo::composePlayType()
{
	/*
	{"play_type_name":"","play_type":108,"play_rule":[3,6,4,1,4,20,1,1,1,0,0,0,1,1,2,1,1,1,1,1,1,1,1],"user_count":6,"round_count":1,"is_vip":0,"gps_limit":0,"allow_look":0,"club_ower_look":0,"join_score":100,"rob_score":90,"can_minus":1,"club_id":41}
	*/

	return "{"+get_play_type_name() + "," +
		get_play_type() + "," +
		get_play_rule() + "," +
		get_user_count() + "," +
		get_round_count() + "," +
		get_is_vip() + "," +
		get_gps_limit() + "," +
		get_allow_look() + "," +
		get_club_ower_look() + "," +
		get_join_score() + "," +
		get_rob_score() + "," +
		get_warn_score() + "," +
		get_can_minus() + "}";




}

/////////////////////////////////////////////////////////////////////
bool ClubPlayManager::Init()
{
	return true;
}
bool ClubPlayManager::Final()
{
	return true;
}


boost::shared_ptr<CSafeResourceLock<ClubPlayInfo> > ClubPlayManager::getClubPlayInfoByClubPlayId(Lint clubPlayId)
{
	boost::shared_ptr<ClubPlayInfo> playType;

	do
	{
		boost::mutex::scoped_lock l(m_mutexClubPlayQueue);

		auto itUser = m_mapId2ClubPlayInfo.find(clubPlayId);
		if (itUser == m_mapId2ClubPlayInfo.end())
		{
			break;
		}

		playType = itUser->second;

	} while (false);

	boost::shared_ptr<CSafeResourceLock<ClubPlayInfo>> safePlay;
	if (playType)
	{
		safePlay.reset(new CSafeResourceLock<ClubPlayInfo>(playType));
	}

	return safePlay;
}


bool ClubPlayManager::addClubPlay(clubPlayType& aClubPlayType, Lint beforePlayId)
{
	if (aClubPlayType.m_playTypes.empty())
	{
		LLOG_ERROR("addClubPlay  playid[%d]  ERROR  m_playTypes.empty()",  aClubPlayType.m_id);
		return false;
	}

	boost::mutex::scoped_lock l(m_mutexClubPlayQueue);
	auto ItPlayInfo = m_mapId2ClubPlayInfo.find(aClubPlayType.m_id);
	if (ItPlayInfo == m_mapId2ClubPlayInfo.end())
	{
		boost::shared_ptr<ClubPlayInfo> clubPlayInfo(new ClubPlayInfo());

		if (clubPlayInfo->parsePlayType(aClubPlayType.m_id, aClubPlayType.m_playTypes))
		{
			m_mapId2ClubPlayInfo[aClubPlayType.m_id] = clubPlayInfo;

			if (beforePlayId != 0)
			{
				auto ItBeforePlayInfo = m_mapId2ClubPlayInfo.find(beforePlayId);
				if (ItBeforePlayInfo != m_mapId2ClubPlayInfo.end())
				{
					clubPlayInfo->m_detail.m_minPointInRoom = ItBeforePlayInfo->second->m_detail.m_minPointInRoom;
					clubPlayInfo->m_detail.m_minPointXiaZhu = ItBeforePlayInfo->second->m_detail.m_minPointXiaZhu;
					clubPlayInfo->m_detail.m_smallZero= ItBeforePlayInfo->second->m_detail.m_smallZero;
					clubPlayInfo->m_detail.m_warnScore = ItBeforePlayInfo->second->m_detail.m_warnScore;

					//扯炫竞技场修改底皮分数，检测修改进入分数和预警分数合法性
					if (clubPlayInfo->m_detail.m_state == 111 && clubPlayInfo->m_detail.m_playtype.size() >= 2)
					{
						LLOG_ERROR("ClubManager::updatePlayType() Club ower modify dipi check warn an min in score diPi=[%d], beforeInScore=[%d], beforeWarnScore=[%d]",
							clubPlayInfo->m_detail.m_playtype[1], clubPlayInfo->m_detail.m_minPointInRoom, clubPlayInfo->m_detail.m_warnScore);

						//1/3
						if (clubPlayInfo->m_detail.m_playtype[1] == 0)
						{
							if (clubPlayInfo->m_detail.m_minPointInRoom < 500) clubPlayInfo->m_detail.m_minPointInRoom = 500;
						}
						//5/1
						else if (clubPlayInfo->m_detail.m_playtype[1] == 1)
						{
							if (clubPlayInfo->m_detail.m_minPointInRoom < 1000) clubPlayInfo->m_detail.m_minPointInRoom = 1000;
						}
						//5/2
						else if (clubPlayInfo->m_detail.m_playtype[1] == 2)
						{
							if (clubPlayInfo->m_detail.m_minPointInRoom < 2000) clubPlayInfo->m_detail.m_minPointInRoom = 2000;
						}
						//2/5
						else if (clubPlayInfo->m_detail.m_playtype[1] == 3)
						{
							if (clubPlayInfo->m_detail.m_minPointInRoom < 500) clubPlayInfo->m_detail.m_minPointInRoom = 500;
						}

						//检测并修改能量预警值合法性
						if (clubPlayInfo->m_detail.m_warnScore > clubPlayInfo->m_detail.m_minPointInRoom) clubPlayInfo->m_detail.m_warnScore = 0;

						LLOG_ERROR("ClubManager::updatePlayType() Club ower modify dipi check warn an min in score diPi=[%d], afterInScore=[%d], afterWarnScore=[%d]",
							clubPlayInfo->m_detail.m_playtype[1], clubPlayInfo->m_detail.m_minPointInRoom, clubPlayInfo->m_detail.m_warnScore);
					}
				}
			}
		}
		return true;
	}
	else
	{
		LLOG_ERROR("_addClubPlay ERROR clubPlayId[%d]  ", aClubPlayType.m_id);
		return false;
	}
}

void ClubPlayManager::delClubPlay(Lint clubPlayId)
{
	LLOG_ERROR("delClubPlay  playid[%d]",clubPlayId);
	return;
}

playTypeDetail  ClubPlayManager::getClubPlayInfo(Lint clubPlayId)
{
	playTypeDetail detail;
	//LLOG_DEBUG("getClubPlayInfo playId[%d]",clubPlayId);
	boost::shared_ptr<CSafeResourceLock<ClubPlayInfo> > safeClubPlayInfo = getClubPlayInfoByClubPlayId(clubPlayId);
	if (safeClubPlayInfo.get()!=NULL && safeClubPlayInfo->isValid())
	{
		boost::shared_ptr<ClubPlayInfo> info = safeClubPlayInfo->getResource();
		detail =info->m_detail;
	}
	return detail;
}

bool  ClubPlayManager::updatePlayType(LMsgC2SUpdatePlay *msg, Lint playId)
{
	if (msg == NULL)return false;

	boost::shared_ptr<CSafeResourceLock<ClubPlayInfo> > safeClubPlayInfo = getClubPlayInfoByClubPlayId(playId);
	if (safeClubPlayInfo.get() != NULL && safeClubPlayInfo->isValid())
	{
		boost::shared_ptr<ClubPlayInfo> playInfo = safeClubPlayInfo->getResource();
		if (msg->m_msgType == 1)
			playInfo->m_detail.m_minPointInRoom = msg->m_minPointInRoom;
		else if (msg->m_msgType == 2)
			playInfo->m_detail.m_minPointXiaZhu = msg->m_minPointXiaZhu;
		else if (msg->m_msgType == 3)
			playInfo->m_detail.m_smallZero = msg->m_smallZero;
		else if (msg->m_msgType == 4)
			playInfo->m_detail.m_warnScore = msg->m_warnScore;
		//扯炫修改最大投注上线，总钵钵数量
		else if (msg->m_msgType == 5 && playInfo->m_detail.m_state == 111 && playInfo->m_detail.m_playtype.size() >= 3)
			playInfo->m_detail.m_playtype[2] = msg->m_maxLimitBo;

		//扯炫竞技场修改底皮分数，检测修改进入分数和预警分数合法性
		else if (msg->m_msgType == 6 && playInfo->m_detail.m_state == 111 &&  playInfo->m_detail.m_playtype.size() >= 2)
		{
			LLOG_ERROR("ClubManager::updatePlayType() Club ower modify dipi check warn an min in score msgType=[%d], beforeInScore=[%d], beforeWarnScore=[%d]", 
				msg->m_msgType, playInfo->m_detail.m_minPointInRoom, playInfo->m_detail.m_warnScore);

			//1/3
			if (playInfo->m_detail.m_playtype[1] == 0)
			{
				if (playInfo->m_detail.m_minPointInRoom < 500) playInfo->m_detail.m_minPointInRoom = 500;
			}
			//5/1
			else if (playInfo->m_detail.m_playtype[1] == 1)
			{
				if (playInfo->m_detail.m_minPointInRoom < 1000) playInfo->m_detail.m_minPointInRoom = 1000;
			}
			//5/2
			else if (playInfo->m_detail.m_playtype[1] == 2)
			{
				if (playInfo->m_detail.m_minPointInRoom < 2000) playInfo->m_detail.m_minPointInRoom = 2000;
			}
			//2/5
			else if (playInfo->m_detail.m_playtype[1] == 3)
			{
				if (playInfo->m_detail.m_minPointInRoom < 500) playInfo->m_detail.m_minPointInRoom = 500;
			}
			
			//检测并修改能量预警值合法性
			if (playInfo->m_detail.m_warnScore > playInfo->m_detail.m_minPointInRoom) playInfo->m_detail.m_warnScore = 0;

			LLOG_ERROR("ClubManager::updatePlayType() Club ower modify dipi check warn an min in score msgType=[%d], afterInScore=[%d], afterWarnScore=[%d]",
				msg->m_msgType, playInfo->m_detail.m_minPointInRoom, playInfo->m_detail.m_warnScore);
		}

		updatePlayTypeToDB(playInfo);

		return true;
	}
	return false;
}

bool  ClubPlayManager::updatePlayType(Lint playId, Lint value, Lint type)
{
	boost::shared_ptr<CSafeResourceLock<ClubPlayInfo> > safeClubPlayInfo = getClubPlayInfoByClubPlayId(playId);
	if (safeClubPlayInfo.get() != NULL && safeClubPlayInfo->isValid())
	{
		boost::shared_ptr<ClubPlayInfo> playInfo = safeClubPlayInfo->getResource();
		if (type == 1)
		{
			if (playInfo->m_detail.m_minPointInRoom < 100)
			{
				LLOG_DEBUG("updatePlayType playId[%d]  [%d] < 100", playInfo->m_detail.m_minPointInRoom);
				playInfo->m_detail.m_minPointInRoom = 100;
			}
		}

		return true;
	}
	return false;
}

bool ClubPlayManager::updatePlayTypeToDB(boost::shared_ptr<ClubPlayInfo> playInfo)
{
	std::stringstream ss;
	ss << "UPDATE club_playtypes";
	ss << " SET playType = '" << playInfo->composePlayType() << "'";
	ss << " WHERE id = '" << playInfo->m_detail.m_id << "'";
	
	LMsgLMG2CeSqlInfo send;
	send.m_type = 1;
	send.m_sql = ss.str();
	gWork.SendToCenter(send);
	LLOG_DEBUG("sql[%s]",ss.str().c_str());
	return true;
}

Lint  ClubPlayManager::getPlayMaxUser(Lint playId)
{
	Lint maxUserCount = 0;
	playTypeDetail detail = getClubPlayInfo(playId);
	switch (detail.m_state)
	{
		case 108:
		{
			if(detail.m_playtype.size()>=2)
				return detail.m_playtype[1];
		}
		//扯炫
		case 111:
		{
			return detail.m_playtype[3];
		}
		default:
			break;
	}
	LLOG_ERROR(" playId[%d]  maxUser[%d]",playId, maxUserCount);
	return maxUserCount;
}