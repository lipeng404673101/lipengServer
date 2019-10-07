#include "DeskManager.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"

bool DeskManager::Init()
{
	//初始所有的桌子号
	std::vector<Lint> m_deskVector;
	for(Lint iDeskId = 100000; iDeskId < 999999; iDeskId++)
	{
		m_deskVector.push_back(iDeskId);
	}

	//生成随机队列
	Lint nSize = m_deskVector.size();
	while(nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		m_FreeDeskList.push(m_deskVector[seed1]);
		m_sharevideoId.push(m_deskVector[seed1]);
		m_deskVector[seed1] = m_deskVector[nSize - 1];
		nSize --;
	}

	//关于俱乐部
	initClubDeskList();

	return true;
}

bool DeskManager::Final()
{
	return true;
}

Lint DeskManager::RecycleDeskId(Lint iDeskId)
{
	Lint iLogicServerId = INVALID_LOGICSERVERID;

	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if(itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		iLogicServerId = itDesk2Logic->second.m_logicServerId;
		m_mapDeskId2LogicServerId.erase(itDesk2Logic);
	}

	if(iLogicServerId != INVALID_LOGICSERVERID)	//ID放回队列
	{
		m_FreeDeskList.push(iDeskId);
	}

	//如果是俱乐部的桌子，从俱乐部桌子中清理
	auto ItDeskIdBelongToClub=m_DeskIdBelongToClub.find(iDeskId);
	if (ItDeskIdBelongToClub!=m_DeskIdBelongToClub.end())
	{
		m_DeskIdBelongToClub.erase(iDeskId);
	}

	//如果是保存桌号，去掉
	auto ItKeepDeskId=m_KeepDeskIdList.find(iDeskId);
	if (ItKeepDeskId!=m_KeepDeskIdList.end())
	{
		m_KeepDeskIdList.erase(iDeskId);
	}
	
	return iLogicServerId;
}

Lint DeskManager::GetFreeDeskId(Lint nServerID,Lint nInCredits)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	Lint id = 0;
	Lint nDeskSize = m_FreeDeskList.size();

	//LLOG_DEBUG("DeskManager::GetFreeDeskId size=%d", nDeskSize);
	while(nDeskSize > 0)
	{
		id = m_FreeDeskList.front();
		m_FreeDeskList.pop();

		auto ItKeepDeskId=m_KeepDeskIdList.find(id);
		if (ItKeepDeskId==m_KeepDeskIdList.end())
		{
			DeskInfos tDesk(nServerID, nInCredits);
			m_mapDeskId2LogicServerId[id]=tDesk;
			break;
		}
	}

	return id;
}

Lint DeskManager::GetDeskCreditLimit( Lint iDeskId )
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second.m_creditsLimit;
	}

	return -1;
}

Lint DeskManager::GetLogicServerIDbyDeskID(Lint nDeskID)
{

	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(nDeskID);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second.m_logicServerId;
	}

	return INVALID_LOGICSERVERID;
}

void DeskManager::RecycleAllDeskIdOnLogicServer(Lint nLogicID)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	for(auto itDesk2Logic = m_mapDeskId2LogicServerId.begin(); itDesk2Logic != m_mapDeskId2LogicServerId.end();)
	{
		if(itDesk2Logic->second.m_logicServerId == nLogicID)
		{
			m_mapDeskId2LogicServerId.erase(itDesk2Logic++);
		}
		else
		{
			itDesk2Logic++;
		}
	}
}

void DeskManager::RecycleAllDeskIdOnLogicServer(Lint nLogicID, std::vector<ClubInfos> &vclubInfos)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	for (auto itDesk2Logic=m_mapDeskId2LogicServerId.begin(); itDesk2Logic!=m_mapDeskId2LogicServerId.end();)
	{
		if (itDesk2Logic->second.m_logicServerId==nLogicID)
		{
			if (itDesk2Logic->second.m_clubInfo.m_clubId!=0)
			{
				vclubInfos.push_back(itDesk2Logic->second.m_clubInfo);
			}
			m_mapDeskId2LogicServerId.erase(itDesk2Logic++);
		}
		else
		{
			itDesk2Logic++;
		}
	}
}

Lint DeskManager::GetCoinsDeskSize()
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	return m_coinsDeskID.size();
}

void DeskManager::GetFreeCoinDeskID( int count, std::vector<Lint>& deskid )
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	for ( int i = 0; i < count; ++i )
	{
		if ( m_FreeDeskList.size() > 0 )
		{
			deskid.push_back( m_FreeDeskList.front() );
			m_coinsDeskID.push_back( m_FreeDeskList.front() );
			m_FreeDeskList.pop();
		}
	}
}

void DeskManager::RecycleCoinsDeskId( const std::vector<int>& deskid )
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	for ( auto it = deskid.begin(); it != deskid.end(); ++it )
	{
		auto cit = std::find( m_coinsDeskID.begin(), m_coinsDeskID.end(), *it );
		if ( cit != m_coinsDeskID.end() )
		{
			m_FreeDeskList.push( *cit );
			m_coinsDeskID.erase( cit );
		}
	}
}

void DeskManager::RecycleCoinsDeskId()
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	for ( auto it = m_coinsDeskID.begin(); it != m_coinsDeskID.end(); ++it )
	{
		m_FreeDeskList.push( *it );
	}
	m_coinsDeskID.clear();
}

Lint DeskManager::GetShareVideoId()
{
	Lint id = m_sharevideoId.front();
	m_sharevideoId.pop();
	m_sharevideoId.push(id);
	return id;
}

void   DeskManager::SetDeskId2LogicServerId(Lint deskId, Lint serverId, Lint creditsLimit, Lint  MHfeeType, Lint MHfeeCost, Lint MHMaxCircle)
{
	LLOG_DEBUG("DeskManager::SetDeskId2LogicServerId  deskId = [%d]  serverId=[%d]", deskId, serverId);
	boost::mutex::scoped_lock l(m_mutexClubDeskId);

	DeskInfos tDesk(serverId, creditsLimit);
	tDesk.m_MHfeeCost=MHfeeCost;
	tDesk.m_MHfeeType=MHfeeType;
	tDesk.m_MHMaxCircle=MHMaxCircle;
	m_mapDeskId2LogicServerId[deskId]=tDesk;

}

////////////////////////////////////////////////////////////////////////////////////
//关于俱乐部
void DeskManager::initClubDeskList()
{
	//初始所有的桌子号
	std::vector<Lint> m_deskVector;
	for (Lint iDeskId=1000000; iDeskId < 9999999; iDeskId++)
	{
		m_deskVector.push_back(iDeskId);
	}

	//生成随机队列
	Lint nSize=m_deskVector.size();
	while (nSize > 0)
	{
		Lint seed1=L_Rand(0, nSize-1);
		m_FreeClubDeskList.push(m_deskVector[seed1]);
		m_deskVector[seed1]=m_deskVector[nSize-1];
		nSize--;
	}
}

Lint DeskManager::GetFreeClubDeskIds(Lint clubId, Lint playTypeId, Lint deskIdCount, std::set<Lint>& clubDeskIds)
{
	LLOG_DEBUG("DeskManager::GetFreeClubDeskIds  ClubId = [%d]  deskIdCount=[%d]", clubId, deskIdCount);
	boost::mutex::scoped_lock l(m_mutexClubDeskId);

	if (m_FreeClubDeskList.size()<deskIdCount)
	{
		LLOG_ERROR("DeskManager::GetFreeClubDeskIds  error. deskid not enough");
		return 0;
	}
	int deskIdNum=0;
	for (; deskIdNum<deskIdCount; deskIdNum++)
	{
		if (m_FreeClubDeskList.size()<=0)break;

		while (m_FreeClubDeskList.size()>0)
		{
			Lint id=m_FreeClubDeskList.front();
			m_FreeClubDeskList.pop();

			auto ItKeepDeskId=m_KeepClubDeskIdList.find(id);
			if (ItKeepDeskId==m_KeepClubDeskIdList.end())
			{
				clubDeskIds.insert(id);
				ClubInfos mClubInfo(clubId, playTypeId, id);
				m_mapClubDeskId2ClubInfo[id]=mClubInfo;
				break;
			}
			//else
			//{
			//	LLOG_DEBUG("Logwyz ....GetFreeClubDeskIds  hhhhhhhhhhhhhhhhhhhhhhh   hhhhh");
			//}
		}
	}

	//LLOG_DEBUG("DeskManager::GetFreeClubDeskIds  ClubId = [%d]  deskIdCount=[%d] deskIdNum=[%d]", clubId, deskIdCount, deskIdNum);
	//for (int i=0; i<clubDeskIds.size(); i++)
	//{
	//	LLOG_DEBUG("DeskManager::GetFreeClubDeskIds  [%d]= [%d] ", i, clubDeskIds[i]);
	//}

	return deskIdNum;
}

void  DeskManager::SetClubDeskIds(Lint clubId, Lint playTypeId, Lint deskIdCount, const std::vector<Lint>& clubDeskIds)
{
	LLOG_DEBUG("DeskManager::SetClubDeskIds  ClubId = [%d]  deskIdCount=[%d]", clubId, clubDeskIds.size());
	
	for (int i = 0; i<clubDeskIds.size(); i++)
	{
		ClubInfos mClubInfo(clubId, playTypeId, clubDeskIds[i]);
		m_mapClubDeskId2ClubInfo[clubDeskIds[i]]=mClubInfo;
		m_KeepClubDeskIdList.insert(clubDeskIds[i]);

	}

}

bool DeskManager::GetClubInfoByClubDeskId(Lint clubDeskId, ClubInfos& clubInfo)
{
	LLOG_DEBUG("DeskManager::GetClubInfoByClubDeskId clubDeskId = [%d] ", clubDeskId);
	boost::mutex::scoped_lock l(m_mutexClubDeskId);

	auto itClubInfo=m_mapClubDeskId2ClubInfo.find(clubDeskId);
	if (itClubInfo!=m_mapClubDeskId2ClubInfo.end())
	{
		clubInfo.m_clubDeskId=clubDeskId;
		clubInfo.m_clubId=itClubInfo->second.m_clubId;
		clubInfo.m_playTypeId=itClubInfo->second.m_playTypeId;
		LLOG_DEBUG("DeskManager::GetClubInfoByClubDeskId clubDeskId = [%d]  m_clubId=[%d],  m_playTypeId=[%d]", clubDeskId, clubInfo.m_clubId, clubInfo.m_playTypeId);
		 return true;
	}

	return false;
}

Lint  DeskManager::GetFreeDeskIds(Lint nServerID, Lint nInCredits, Lint requestCount, std::vector<Lint>&  deskIds)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	//Lint id=0;
	Lint nDeskSize=m_FreeDeskList.size();

	//LLOG_DEBUG("DeskManager::GetFreeDeskId size=%d", nDeskSize);
	if (requestCount>nDeskSize)return -1;
	for (int i=0; i<requestCount; i++)
	{
		if (m_FreeDeskList.size()>0)
		{
			Lint id=m_FreeDeskList.front();
			m_FreeDeskList.pop();
			DeskInfos tDesk(nServerID, nInCredits);
			m_mapDeskId2LogicServerId[id]=tDesk;
			deskIds.push_back(id);
		}
	}
	return 0;
}

Lint DeskManager::ClubGetFreeDeskId(Lint nServerID, Lint nInCredits, Lint clubId, Lint playTypeId, Lint clubDeskId)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	Lint id=0;
	Lint nDeskSize=m_FreeDeskList.size();

	//LLOG_DEBUG("DeskManager::GetFreeDeskId size=%d", nDeskSize);
	if (nDeskSize > 0)
	{
		id=m_FreeDeskList.front();
		m_FreeDeskList.pop();
		DeskInfos tDesk(nServerID, nInCredits);
		tDesk.m_clubInfo.m_clubId=clubId;
		tDesk.m_clubInfo.m_playTypeId=playTypeId;
		tDesk.m_clubInfo.m_clubDeskId=clubDeskId;
		m_mapDeskId2LogicServerId[id]=tDesk;

		m_DeskIdBelongToClub.insert(id);
	
	}

	return id;

}

bool DeskManager::IsBelongToClub(Lint deskId)
{
	auto ItDeskIdBelongToClub=m_DeskIdBelongToClub.find(deskId);
	if (ItDeskIdBelongToClub!=m_DeskIdBelongToClub.end())
		return true;

	return false;
}

void DeskManager::SynchroLogicServerDeskInfo(std::vector<LogicDeskInfo> deskInfo, Lint serverId)
{
	LLOG_DEBUG("DeskManager::SynchroLogicServerDeskInfo  deskInfo.size = [%d]  serverId=[%d]", deskInfo.size(), serverId);
	boost::mutex::scoped_lock l(m_mutexDeskId);
	
	//保存desk-logicId关系
	for (int i=0; i<deskInfo.size(); i++)
	{
		Lint deskId=deskInfo[i].m_deskId;
		//保留不再分配
		m_KeepDeskIdList.insert(deskId);
		//保存桌子信息
		auto itDesk2Logic=m_mapDeskId2LogicServerId.find(deskId);
		if (itDesk2Logic==m_mapDeskId2LogicServerId.end())
		{
			//信用值先写成0
			DeskInfos tDesk(serverId, 0);
			//tDesk.m_MHfeeCost=MHfeeCost;
			//tDesk.m_MHfeeType=MHfeeType;
			//tDesk.m_MHMaxCircle=MHMaxCircle;
			m_mapDeskId2LogicServerId[deskId]=tDesk;
		}

		//俱乐部信息
		auto itClubInfo=m_mapClubDeskId2ClubInfo.find(deskInfo[i].m_clubDeskId);
		if (itClubInfo==m_mapClubDeskId2ClubInfo.end())
		{
			ClubInfos mClubInfo(deskInfo[i].m_clubId, deskInfo[i].m_playTypeId, deskInfo[i].m_clubDeskId);
			m_mapClubDeskId2ClubInfo[deskInfo[i].m_clubDeskId]=mClubInfo;
		}
	}

	//处理club桌子内容
	for (auto ItDeskInfo=deskInfo.begin(); ItDeskInfo!=deskInfo.end(); ItDeskInfo++)
	{
		gClubManager.modifyClubDeskInfoFromLogic((*ItDeskInfo));
	}
	

}

