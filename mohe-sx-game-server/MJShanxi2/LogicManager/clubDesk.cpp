#include "clubDesk.h"


bool ClubDeskManager::Init()
{
	return true;
}
bool ClubDeskManager::Final()
{
	return true;
}


boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > ClubDeskManager::getClubDeskInfoByClubDeskId(Lint iUserId)
{
	boost::shared_ptr<ClubDeskInfo> desk;

	do
	{
		boost::mutex::scoped_lock l(m_mutexClubDeskQueue);

		auto itUser = m_mapId2ClubDeskInfo.find(iUserId);
		if (itUser == m_mapId2ClubDeskInfo.end())
		{
			break;
		}

		desk = itUser->second;

	} while (false);

	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeDesk;
	if (desk)
	{
		safeDesk.reset(new CSafeResourceLock<ClubDeskInfo>(desk));
	}

	return safeDesk;
}


void ClubDeskManager::addClubDesk(Lint clubId, Lint clubDeskId, Lint showDeskId,Lint playId)
{
	boost::mutex::scoped_lock l(m_mutexClubDeskQueue);
	auto ItClubDesk = m_mapId2ClubDeskInfo.find(clubDeskId);
	if (ItClubDesk == m_mapId2ClubDeskInfo.end())
	{
		boost::shared_ptr<ClubDeskInfo> clubDeskInfo(new ClubDeskInfo(clubId, clubDeskId, showDeskId, playId));
		m_mapId2ClubDeskInfo[clubDeskId] = clubDeskInfo;
	}
	else
	{
		LLOG_ERROR("addClubDesk clubDeskId[%d]  fail in club[%d] play[%d]",clubDeskId, clubId,playId);
	}
}

void ClubDeskManager::delClubDesk(Lint clubDeskId)
{
	LLOG_ERROR("delClubDesk   not write");
	return;
}

void ClubDeskManager::setPlayId(Lint clubDeskId, Lint playId, playTypeDetail & pDetail)
{
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		safeClubDeskInfo->getResource()->setPlayTypeId(playId);
	}
}

void ClubDeskManager::setRealId(Lint clubDeskId, Lint realDeskId,Lint playId, playTypeDetail & pDetail)
{
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		safeClubDeskInfo->getResource()->setRealDeskId(realDeskId, playId);
	}
}

desksInfo  ClubDeskManager::getClubDeskInfo(Lint clubDeskId, Lint currPlayId)
{
	desksInfo  tempDeskInfo;
	tempDeskInfo.m_deskId = clubDeskId;
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		tempDeskInfo.m_showDeskId = info->getShowDeskId();
		tempDeskInfo.m_roomFull = info->m_roomFull;
		tempDeskInfo.m_currCircle = info->m_currCircle;
		tempDeskInfo.m_totalCircle = info->m_totalCircle;
		tempDeskInfo.m_isClubOwerLooking = info->m_isClubOwerLooking;
		tempDeskInfo.m_redPacketFlag = info->m_redPacketFlag;
		if (info->isUsed())
			tempDeskInfo.m_userCount = gClubPlayManager.getPlayMaxUser(info->getPlayId()); 
		else
			tempDeskInfo.m_userCount = gClubPlayManager.getPlayMaxUser(currPlayId);

		//LLOG_DEBUG("tempDeskInfo.m_userCount[%d]", tempDeskInfo.m_userCount);
		tempDeskInfo.m_flag = info->m_flag;
		tempDeskInfo.m_lock = info->getLock();
		for(auto ItUser = info->m_deskUserInfo.begin(); ItUser != info->m_deskUserInfo.end(); ItUser++)
			tempDeskInfo.m_deskUserInfo.push_back(*ItUser);
	
	}

	return tempDeskInfo;
}

Lint  ClubDeskManager::getDeskInClubId(Lint clubDeskId)
{
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		return safeClubDeskInfo->getResource()->getClubId();
	}
	return 0;
}

//void  ClubDeskManager::_joinDesk(Lint clubDeskId, deskUserInfo& userInfo)
//{
//	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(clubDeskId);
//	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
//	{
//		safeClubDeskInfo->getResource()->insertUserInfo(userInfo);
//	}
//
//}
//
//void  ClubDeskManager::_leaveDesk(Lint clubDeskId, Lint userId)
//{
//	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(clubDeskId);
//	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
//	{
//		safeClubDeskInfo->getResource()->delUserInfo(userId);
//	}
//
//}

void  ClubDeskManager::addDesk(LMsgL2LMGUserAddClubDesk * addDesk)
{
	if (addDesk == NULL)return;
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(addDesk->m_clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		if (addDesk->m_type == 1) {
			info->insertLookon(addDesk->m_userInfo.m_userId);
		}
		else {
			info->m_isClubOwerLooking = addDesk->m_isClubOwerLooking;
			info->insertUserInfo(addDesk->m_userInfo);

			info->delLookon(addDesk->m_userInfo.m_userId);
		}
	}
}

void  ClubDeskManager::leaveDesk(LMsgL2LMGUserLeaveClubDesk* leaveDesk)
{
	if (leaveDesk == NULL)return;
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(leaveDesk->m_clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		info->m_isClubOwerLooking = leaveDesk->m_isClubOwerLooking;
		info->delUserInfo(leaveDesk->m_userId);
		
		//ลินÛ
		info->delLookon(leaveDesk->m_userId);
	}
}

void  ClubDeskManager::leaveDesk(LMsgL2LMGUserListLeaveClubDesk* leaveDesk)
{
	if (leaveDesk == NULL)return;
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(leaveDesk->m_clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		//info->m_isClubOwerLooking = leaveDesk->m_isClubOwerLooking;
		for (auto ItUser = leaveDesk->m_userIdList.begin(); ItUser != leaveDesk->m_userIdList.end(); ItUser++)
		{
			info->delUserInfo(*ItUser);
		}
		info->rebootDesk();
	
	}
}


void  ClubDeskManager::updateDeskInfo(LMsgL2LMGFreshDeskInfo * pInfo)
{
	if (pInfo == NULL)return;
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(pInfo->m_clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		info->m_roomFull = pInfo->m_roomFull;
		info->m_currCircle = pInfo->m_currCircle;
		info->m_totalCircle = pInfo->m_totalCircle;
	}
}

void  ClubDeskManager::recycleClubDesk(Lint clubDeskId)
{
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		info->rebootDesk();
	}
}

void ClubDeskManager::modifyClubDeskInfoFromLogic(const LogicDeskInfo & deskInfo)
{
	LLOG_ERROR("modifyClubDeskInfoFromLogic  clubId[%d] showdeskId[%d]", deskInfo.m_clubId, deskInfo.m_clubDeskId);

	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfoByClubDeskId(deskInfo.m_clubDeskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		info->setRealDeskId(deskInfo.m_deskId,deskInfo.m_playTypeId);
		info->m_roomFull = deskInfo.m_roomFull;
		info->m_currCircle = deskInfo.m_currCircle;
		info->m_totalCircle = deskInfo.m_totalCircle;

		for (int i = 0; i<deskInfo.m_seatUser.size(); i++)
		{
			struct deskUserInfo   userInfo;
			userInfo.m_userId = deskInfo.m_seatUser[i].m_userId;
			userInfo.m_deskPlayerUrl = deskInfo.m_seatUser[i].m_headUrl;
			userInfo.m_deskPlayerName = deskInfo.m_seatUser[i].m_nike;
			userInfo.m_pos = deskInfo.m_seatUser[i].m_pos;

			info->m_deskUserInfo.push_back(userInfo);
		}
	}

	return;
}