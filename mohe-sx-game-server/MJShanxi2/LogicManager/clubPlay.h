#pragma once
#ifndef _CLUB_PLAY_H_
#define _CLUB_PLAY_H_

#include "LMsgS2S.h"
#include "SafeResourceLock.h"

////////////////////////////////////////////////////////////////////////////////////////////
struct ClubPlayInfo :public CResourceLock 
{
	/* 俱乐部玩法名称 */
	Lstring           m_playTypeString;

	/* 俱乐部玩法详情 */
	playTypeDetail    m_detail;

	ClubPlayInfo() {}

	~ClubPlayInfo(){}

	/* 重载=操作符，结构体赋值 */
	void copy(playTypeDetail& detail)
	{
		m_detail = detail;
	}

	Lstring getPlayString()
	{
		return m_playTypeString;
	}

	void updatePlayString(Lstring str)
	{
		m_playTypeString = str;
	}

	

	/* 解析Json格式俱乐部玩法详情 */
	bool     parsePlayType(Lint playTypeId, const Lstring playType);

	/* 拼写Json格式俱乐部玩法详情 */
	Lstring  composePlayType();

	Lstring spec(Lstring str)
	{
		return "\"" + str + "\""+":";
	}

	/*
	数据库club_playtypes.PlayType字段样例
	{
		"play_type_name":"",
		"play_type":111,
		"play_rule":[1,0,500,5,0,0,0,0,0,0,0,1,1,0],
		"user_count":5,"round_count":1,"is_vip":0,
		"gps_limit":0,
		"allow_look":0,
		"club_ower_look":0,
		"join_score":0,
		"rob_score":0,
		"can_minus":0,
		"warn_score":0,
		"club_id":12356
	}
	*/

	/* 获取玩法名称 */
	Lstring get_play_type_name()
	{
		Lstring str;
		if (m_detail.m_name.empty())
			str = spec("play_type_name") + "\"\"";
		else
			str = spec("play_type_name") + m_detail.m_name;
		return  str;
	}
	Lstring get_play_type()
	{
		return spec("play_type") + std::to_string(m_detail.m_state);
	}
	Lstring get_play_rule()
	{
		Lstring str("[");
		for (auto It = m_detail.m_playtype.begin(); It != m_detail.m_playtype.end(); It++)
		{
			str.append(std::to_string(*It));
			if (It + 1 != m_detail.m_playtype.end())
				str.append(",");
		}
		str.append("]");

		return spec("play_rule") + str;
	}
	Lstring get_user_count()
	{
		return spec("user_count") + std::to_string(m_detail.m_Greater2CanStart);
	}
	Lstring get_round_count()
	{
		return  spec("round_count") + std::to_string(m_detail.m_flag);
	}
	Lstring get_is_vip()
	{
		return  spec("is_vip") + std::to_string(m_detail.m_cheatAgainst);
	}
	Lstring get_gps_limit()
	{
		return  spec("gps_limit") + std::to_string(m_detail.m_GpsLimit);
	}
	Lstring get_allow_look()
	{
		return  spec("allow_look") + std::to_string(m_detail.m_allowLookOn);
	}
	Lstring get_club_ower_look()
	{
		return  spec("club_ower_look") + std::to_string(m_detail.m_clubOwerLookOn);
	}
	Lstring get_join_score()
	{
		return  spec("join_score") + std::to_string(m_detail.m_minPointInRoom);
	}
	Lstring get_rob_score()
	{
		return  spec("rob_score") + std::to_string(m_detail.m_minPointXiaZhu);
	}
	Lstring get_can_minus()
	{
		return  spec("can_minus") + std::to_string(m_detail.m_smallZero);
	}
	Lstring get_warn_score()
	{
		return  spec("warn_score") + std::to_string(m_detail.m_warnScore);
	}
};
////////////////////////////////////////////////////////////////////////////////////////////
class ClubPlayManager :public LSingleton<ClubPlayManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:

	/* 根据玩法ID，获取带锁的玩法详细信息对象的指针 */
	boost::shared_ptr<CSafeResourceLock<ClubPlayInfo>> getClubPlayInfoByClubPlayId(Lint clubPlayId);

	/* 添加玩法信息 */
	bool addClubPlay(clubPlayType& aClubPlayType, Lint beforePlayId);

	/* 根据玩法信息ID，删除玩法详细信息 */
	void delClubPlay(Lint clubPlayId);

	/* 根据玩法ID，获取玩法详细信息结构对象 */
	playTypeDetail  getClubPlayInfo(Lint clubPlayId);

	/* 更新玩法详细信息*/
	bool updatePlayType(LMsgC2SUpdatePlay* msg, Lint playId);

	/* 更新玩法详细信息中进入房间的允许的最小能量值（竞技场）*/
	bool updatePlayType(Lint playId, Lint value, Lint type);

	/* 根据玩法信息ID，获取玩法详细信息中的最大人数 */
	Lint  getPlayMaxUser(Lint playId);

private:

	/* 更新玩法详细信息到数据库 */
	bool updatePlayTypeToDB(boost::shared_ptr<ClubPlayInfo> playInfo);
	
private:

	/* 所有玩法信息表的锁 */
	boost::mutex m_mutexClubPlayQueue;

	/* 所有玩法详细信息的map表，map<玩法ID，玩法详细信息>*/
	std::map<Lint, boost::shared_ptr<ClubPlayInfo>> m_mapId2ClubPlayInfo;
};

#define gClubPlayManager ClubPlayManager::Instance()

#endif
