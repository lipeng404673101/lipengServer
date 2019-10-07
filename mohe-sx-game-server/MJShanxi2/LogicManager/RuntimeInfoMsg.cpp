#include "RuntimeInfoMsg.h"
#include "LTime.h"
#include "LMsg.h"
#include "Work.h"

CRuntimeInfoMsg::CRuntimeInfoMsg()
{
	m_iHide = 0;
}

CRuntimeInfoMsg::~CRuntimeInfoMsg()
{

}

bool CRuntimeInfoMsg::Init()
{
	m_strHorseInfo	= "欢迎来到闲来游戏，请玩家文明娱乐，严禁赌博，如发现有赌博行为，将封停账号，并向公安机关举报！";
	m_strBuyInfo	= "购买房卡请加微信 ronaldregal";


	LTime now;

	Notice N;
	N.m_time	= now.Secs();
	N.m_title	= "hello";
	N.m_content = "happy new";
	m_vecNotice.push_back(N);

	N.m_time	= now.Secs() - 60*60;
	N.m_title	= "游戏说明";
	N.m_content = "您在游戏中产生的任何纠纷与本平台无关";
	m_vecNotice.push_back(N);

	m_iHide = 0;

	return true;
}

bool CRuntimeInfoMsg::Final()
{
	return true;
}

void CRuntimeInfoMsg::setHorseInfoAndNotify(const Lstring& strInfo)
{
	boost::mutex::scoped_lock l(m_mutexForBackground);
	m_strHorseInfo = strInfo;

	LMsgLMG2GHorseInfo msgHorse;
	msgHorse.m_str	= strInfo;
	msgHorse.m_type = 0;
	gWork.SendMessageToAllGate(msgHorse);

	LLOG_ERROR("Send Horse Info !!!!. Horse Info = %s", strInfo.c_str());
}

Lstring CRuntimeInfoMsg::getHorseInfo()
{
	boost::mutex::scoped_lock l(m_mutexForBackground);
	return m_strHorseInfo;
}

void CRuntimeInfoMsg::setBuyInfo(const Lstring& strInfo)
{
	boost::mutex::scoped_lock l(m_mutexForBackground);

	m_strBuyInfo = strInfo;
	m_vecBuyInfo.clear();

	std::string::iterator begin_index;
	bool first_flag=true;
	for(auto x = m_strBuyInfo.begin(); x != m_strBuyInfo.end();)
	{
		int tmp=3;
		bool end_flag = false;
		begin_index = x;
		while(tmp>0)
		{
			if(x != m_strBuyInfo.end())
			{
				if(*x==',')
				{
					tmp--;
				}
				x++;
			}
			else
			{
				end_flag= true;
				break;
			}
		}
		if(!end_flag)
		{
			if(first_flag)
			{
				m_vecBuyInfo.push_back(std::string(begin_index,x-1));
				first_flag=false;
			}
			else
			{
				m_vecBuyInfo.push_back(std::string(begin_index,x-1));
			}
		}
		else
		{
			if(tmp==1)
			{
				m_vecBuyInfo.push_back(std::string(begin_index, m_strBuyInfo.end()));
			}
		}
	}
}

Lstring CRuntimeInfoMsg::getBuyInfo(int iUserId)
{
	boost::mutex::scoped_lock l(m_mutexForBackground);

	if(!m_vecBuyInfo.empty())
	{
		Lint tempID = iUserId / 200;
		int l_index = tempID % m_vecBuyInfo.size();
		return m_vecBuyInfo[l_index];
	}

	return m_strBuyInfo;
}

void CRuntimeInfoMsg::addNotice(const Notice& notice)
{
	boost::mutex::scoped_lock l(m_mutexForBackground);
	m_vecNotice.push_back(notice);
}

std::vector<Notice> CRuntimeInfoMsg::getNotice()
{
	boost::mutex::scoped_lock l(m_mutexForBackground);
	return m_vecNotice;
}

void CRuntimeInfoMsg::setHide(Lint iHide)
{
	boost::mutex::scoped_lock l(m_mutexForBackground);
	m_iHide = iHide;
}

Lint CRuntimeInfoMsg::getHide()
{
	boost::mutex::scoped_lock l(m_mutexForBackground);
	return m_iHide;
}

void CRuntimeInfoMsg::changeOnlineNum(Lint iGateId, Lint iUserId, bool bInc)
{
	boost::mutex::scoped_lock l(m_mutexForOnlineNum);

	auto itOnline = m_mapOnlinNumOnGate.find(iGateId);
	if(itOnline == m_mapOnlinNumOnGate.end())
	{
		std::map<int, int> mapId;
		m_mapOnlinNumOnGate[iGateId] = mapId;
		itOnline = m_mapOnlinNumOnGate.find(iGateId);
	}

	if(itOnline == m_mapOnlinNumOnGate.end())
	{
		LLOG_ERROR("Don't find the gate id. Id = %d", iGateId);
		return;
	}

	if(bInc)
	{
		itOnline->second[iUserId] = iUserId;
	}
	else
	{
		itOnline->second.erase(iUserId);
	}
}

Lint CRuntimeInfoMsg::getOnlineNum(Lint iGateId)
{
	boost::mutex::scoped_lock l(m_mutexForOnlineNum);

	auto itOnline = m_mapOnlinNumOnGate.find(iGateId);
	if(itOnline == m_mapOnlinNumOnGate.end())
	{
		return 0;
	}

	return itOnline->second.size();
}

void CRuntimeInfoMsg::clearOnlineNum(Lint iGateId)
{
	boost::mutex::scoped_lock l(m_mutexForOnlineNum);
	m_mapOnlinNumOnGate.erase(iGateId);
}

//FreeSet = 2016/8/12 09:00:00#2016/9/27 09:00:00"
void CRuntimeInfoMsg::setFreeTimeAndNotify(Lstring& strFreeTime)
{
	boost::recursive_mutex::scoped_lock l(m_mutexForFree);

	LLOG_ERROR("CRuntimeInfoMsg::setFreeTimeAndNotify %s",strFreeTime.c_str());

	// 清空免费数据
	m_strFreeTime = "";
	m_freeSet.clear();

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	if (!reader.parse(strFreeTime, value))
	{
		LLOG_ERROR("CRuntimeInfoMsg::setFreeTimeAndNotify parsr json error %s", reader.getFormatedErrorMessages().c_str());
		return;
	}

	const Json::Value& arrayObj = value["Free"];
	for(unsigned int i = 0; i < arrayObj.size(); i++)
	{
		if ( arrayObj[i]["GameType"].isNull() || !arrayObj[i]["GameType"].isString() )
		{
			LLOG_ERROR("CRuntimeInfoMsg::setFreeTimeAndNotify GameType Error");
			continue;
		}
		if ( arrayObj[i]["Begin"].isNull() || !arrayObj[i]["Begin"].isString() )
		{
			LLOG_ERROR("CRuntimeInfoMsg::setFreeTimeAndNotify Begin Error");
			continue;
		}
		if ( arrayObj[i]["End"].isNull() || !arrayObj[i]["End"].isString() )
		{
			LLOG_ERROR("CRuntimeInfoMsg::setFreeTimeAndNotify End Error");
			continue;
		}

		BeginEnd time;
		time.m_begin = convert_string_to_time_t( arrayObj[i]["Begin"].asString() );
		time.m_end = convert_string_to_time_t( arrayObj[i]["End"].asString() );
		time.m_config = arrayObj[i];

		std::string gametype = arrayObj[i]["GameType"].asString();
		if ( gametype == "All" )	// 全局免费
		{
			m_freeSet[-1] = time;	// -1表示全局免费
		}
		else
		{
			Lint type = atoi( gametype.c_str() );
			m_freeSet[type] = time;
		}
	}

	updateFree();

	LMsgLMG2GHorseInfo msgFreeTime;
	msgFreeTime.m_type = 1;
	msgFreeTime.m_str = getFreeTime();
	gWork.SendMessageToAllGate(msgFreeTime);
}

Lstring CRuntimeInfoMsg::getFreeTime()
{
	boost::recursive_mutex::scoped_lock l(m_mutexForFree);
	return m_strFreeTime;
}

bool CRuntimeInfoMsg::isFree( Lint gametype )
{
	boost::recursive_mutex::scoped_lock l(m_mutexForFree);

	std::map<Lint,BeginEnd>::iterator it = m_freeSet.find( gametype );
	if ( it != m_freeSet.end() )
	{
		return checkTimeValid(it->second.m_begin, it->second.m_end);
	}
	return false;
}

bool CRuntimeInfoMsg::updateFree()
{
	boost::recursive_mutex::scoped_lock l(m_mutexForFree);
	bool bchange = false;
	for ( auto it = m_freeSet.begin(); it != m_freeSet.end(); ++it )
	{
		if ( checkTimeValid(it->second.m_begin, it->second.m_end) != it->second.m_bstart )
		{
			it->second.m_bstart = !it->second.m_bstart;
			bchange = true;
		}
	}
	if ( bchange )
	{
		LTime cur;
		Json::Value root;   // 构建对象  

		for ( auto it = m_freeSet.begin(); it != m_freeSet.end(); ++it )
		{
			if ( it->second.m_bstart )
			{
				root.append( it->second.m_config );
			}
		}
		m_strFreeTime = root.toStyledString();
		LLOG_ERROR("CRuntimeInfoMsg::updateFree %s",m_strFreeTime.c_str());
	}
	return bchange;
}

