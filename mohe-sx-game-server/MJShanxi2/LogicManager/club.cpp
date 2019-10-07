#include "club.h"
#include "clubUser.h"


Club::Club(Lint clubId)
{
	m_id=clubId;
	m_coin=0;
	m_ownerId=0;
	//m_userSum=0;
	m_onLineUserCount=0;
	m_status=0;
	m_type=0;
	m_feeType=0;
	m_valid=false;

}


Club::~Club()
{
}

void  Club::_clearData()
{
	LLOG_DEBUG("Club::_clearData");
	m_id=0;
	m_coin=0;
	m_ownerId=0;
	//m_userSum=0;
	m_onLineUserCount=0;
	m_status=0;
	m_type=0;
	m_feeType=0;

	//m_playTypeList.clear();
	m_showPlayType.clear();
	//m_playTypeDeskList.clear();
	//m_playTypeDetailList.clear();

	//m_playTypeUserIdList.clear();
	//m_onLineUserList.clear();
	//m_UserList.clear();
	m_valid=false;


}

void Club::initData(LMsgCe2LAddClub *msg)
{
	if (msg==NULL)return;
	m_sq = msg->m_id;
	m_id=msg->m_clubId;
	m_name=msg->m_name;
	m_coin=msg->m_coin;
	m_ownerId=msg->m_ownerId;
	m_clubUrl=msg->m_clubUrl;
	m_status=msg->m_status;
	m_clubType =msg->m_type;
	m_feeType=msg->m_feeType;
	m_redPacket.init(m_id);

	m_adminAuth = msg->m_adminAuth;
	LLOG_DEBUG("club[%d] changePoint[%d]  lookupPoint[%d] table[%d]", getClubId(), m_adminAuth.m_changePoint, m_adminAuth.m_lookupPoint, msg->m_createDeskCount);


	//对桌子数判断，最大100
	Lint maxDeskCount = (msg->m_createDeskCount > MAX_DESK_COUNT_EVERY_TYPE) ? MAX_DESK_COUNT_EVERY_TYPE : msg->m_createDeskCount;

	//创建俱乐部桌子  	
	if (maxDeskCount >0 && msg->m_clubDeskList.size() >= maxDeskCount)
	{
		_fillClubDeskList(msg->m_clubDeskList,0);
		
	}
	else if(maxDeskCount >0)
	{
		if (_createClubDeskList(maxDeskCount, 0) == 0)
		{
			
		}
	}
	else
	{
		LLOG_ERROR("Club::initData clubId[%d] create desk ERROR , deskCount[%d]", m_id,maxDeskCount);
	}


	//玩法
	LLOG_DEBUG("Club::initData clubId[%d],playTypeList.size()=[%d]", msg->m_clubId, msg->playTypeList.size());
	for (auto ItPlayType = msg->playTypeList.begin() ; ItPlayType != msg->playTypeList.end(); ItPlayType++)
	{
		//更新玩法
		addClubPlayType(*ItPlayType);
	}

}

void Club::resetData()
{
	LLOG_DEBUG("Club::resetData");
	
}

bool  Club::hidePlayType(Lint playTypeId,Lstring playType, Lint playTypeIdStatus)
{
	LLOG_DEBUG("Club::hidePlayType  playTypeId=[%d],playTypeIdStatus=[%d],playType=[%s]", playTypeId, playTypeIdStatus,playType.c_str());
	
	if (m_showPlayType.find(playTypeId)!=m_showPlayType.end())
	{
		m_showPlayType.erase(playTypeId);
	}
	else
	{
		LLOG_ERROR("Fail to hidePlayType,playTypeId=[%d]  not find", playTypeId);
	}

	////桌子没有开始游戏就解散，回收红包
	//Lint deskId = m_redPacket.getRedPacketDeskId(playTypeId);
	//if (deskId != 0 )
	//{
	//	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfo(deskId);
	//	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	//	{
	//		auto  ItClubDeskUser = m_clubDesk2UsersInDesk.find(deskId);
	//		if (ItClubDeskUser != m_clubDesk2UsersInDesk.end())  //桌子在
	//		{
	//			if (safeClubDeskInfo->getResource()->m_currCircle == 0 && ItClubDeskUser->second.size() == 0)
	//			{
	//				LLOG_ERROR(" Club::hidePlayType club[%d]  clubDeskId=[%d]  recycle red packet", m_id, deskId);
	//				m_redPacket.unsetRedPacketDeskId(playTypeId, 1);
	//			}
	//		}
	//
	//	}
	//}

	return true;
}

bool Club::alterClubName(Lstring newClubName)
{
	LLOG_DEBUG("Club::alterClubName  newClubName=[%s] ,old_name=[%s]", newClubName.c_str(), m_name.c_str());
	m_name=newClubName;
	return true;
}

bool Club::alterPlayTypeName(Lint playType, Lstring newPlayTypeName)
{
	LLOG_DEBUG("Club::alterPlayTypeName playTypeId[%d]  newClubName[%s]", playType, newPlayTypeName.c_str());

	//auto itPlayType = m_playTypeDetailList.find(playType);
	//if (itPlayType != m_playTypeDetailList.end())
	//{
	//	itPlayType->second.m_name = newPlayTypeName;
	//}
	//else
	//{
	//	LLOG_ERROR("Club::alterPlayTypeName  ERROR .playTypeId[%d]  newClubName[%s]", playType, newPlayTypeName.c_str());
	//}

	return true;
}

bool  Club::_addClubPlayType(Lint playId)
{
	LLOG_DEBUG(" add club[%d] playId[%d] ",getClubId(), playId);
	m_showPlayType.clear();
	m_showPlayType.insert(playId);

	return true;
}

bool Club::_addClubPlayTypeWithDesk(Lint playId, Lint deskCount, std::vector<ManagerClubDeskInfo>& clubdeskList)
{
	LLOG_DEBUG(" add playId[%d]   with desk   deskCount[%d]  deskList[%d] ", playId, deskCount,clubdeskList.size());
	if (deskCount > 0 && clubdeskList.size() >deskCount )
	{
		_fillClubDeskList(clubdeskList, playId);
	}
	else if(deskCount> 0)
	{
		_createClubDeskList(deskCount, playId);
	}
	return true;
}

bool  Club::addClubPlayType(clubPlayType &  aClubPlayType)
{
	LLOG_DEBUG(" add  playId=[%d],playStatus=[%d],playType=[%s],deskCount=[%d]", aClubPlayType.m_id, aClubPlayType.m_status, aClubPlayType.m_playTypes.c_str(), aClubPlayType.m_tableNum);

	//对桌子数判断，最大100
	Lint maxDeskCount = (aClubPlayType.m_tableNum>MAX_DESK_COUNT_EVERY_TYPE) ? MAX_DESK_COUNT_EVERY_TYPE : aClubPlayType.m_tableNum;

	//if (m_clubDeskList.empty())   //如果俱乐部桌子为空，创建桌子
	//{
	//	_addClubPlayTypeWithDesk(aClubPlayType.m_id, maxDeskCount, aClubPlayType.m_clubdeskList);
	//}
	//更新玩法
	_addClubPlayType(aClubPlayType.m_id);

	return true;
}

Lint Club::_createClubDeskList(Lint deskCount, Lint playId)
{
	std::set<Lint>  tempDeskIds;
	//发center记录
	LMsgLMG2CEUpdateData send2Center;
	send2Center.m_clubId = getClubId();
	send2Center.m_playTypeId = playId;

	if (gDeskManager.GetFreeClubDeskIds(getClubId(), playId, deskCount, tempDeskIds) == deskCount)
	{
		//为了打印
		std::string printMsg;
		//保存玩法桌子号
		//for (int i = 0; i<tempDeskIds.size(); i++)
		//m_clubDeskList = tempDeskIds;
		//int sq = 0;
		for(auto ItClubDeskId = tempDeskIds.begin(); ItClubDeskId != tempDeskIds.end(); ItClubDeskId++)
		{
			//sq = m_clubDeskList.size();
			//展示房间号定义规则
			//把俱乐部桌子号记录俱乐部中
			 Lint showDeskId =  insertClubDesk(*ItClubDeskId);
			 if (showDeskId == 0)continue;
			//m_clubDeskList.insert(*ItClubDeskId);
			//sq++;
			gClubDeskManager.addClubDesk(getClubId(), *ItClubDeskId, showDeskId, playId);

			//填充消息
			ManagerClubDeskInfo tempDeskInfo;
			tempDeskInfo.m_clubDeskId = *ItClubDeskId;
			tempDeskInfo.m_showDeskId = showDeskId;
			send2Center.m_info.push_back(tempDeskInfo);

			printMsg.append(std::to_string(*ItClubDeskId) + ",");

		}
		LLOG_ERROR("Club::createDeskIdbyPlayType new club_id=[%d] ,playTypeId=[%d],deskCount=[%d] deskids=[%s]", send2Center.m_clubId, send2Center.m_playTypeId, deskCount, printMsg.c_str());
	}
	else
	{
		LLOG_ERROR("Club::createDeskIdbyPlayType club_id=[%d] ,playTypeId=[%d],deskCount=[%d]  error", getClubId(), playId, deskCount);
		return -1;
	}

	//发center记录
	send2Center.m_count = send2Center.m_info.size();
	gWork.SendToCenter(send2Center);
	
	return 0;
}

Lint Club::_fillClubDeskList(const std::vector<ManagerClubDeskInfo> &clubDeskinfo, Lint playId)
{
	//为了打印
	std::string printMsg;
	std::vector<Lint> clubDeskIds;
	for (int i = 0; i<clubDeskinfo.size(); i++)
	{
		//保存俱乐部桌子
		Lint showDeskId = insertClubDesk(clubDeskinfo[i].m_clubDeskId);
		
		//m_clubDeskList.insert(clubDeskinfo[i].m_clubDeskId);
		gClubDeskManager.addClubDesk(getClubId(), clubDeskinfo[i].m_clubDeskId, showDeskId, playId);
		
		clubDeskIds.push_back(clubDeskinfo[i].m_clubDeskId);
		printMsg.append(std::to_string(clubDeskinfo[i].m_clubDeskId) + ",");

	}
	LLOG_ERROR("Club::fillClubDeskList  club_id=[%d] ,playTypeId=[%d], deskList[%d] deskids=[%s]", m_id, playId, clubDeskinfo.size(), printMsg.c_str());
	//记录
	gDeskManager.SetClubDeskIds(getClubId(), playId, clubDeskIds.size(), clubDeskIds);
	return 0;
	
}

//客户端显示桌号的规则
Lint  Club::getShowDeskId(Lint sq)
{
	return sq;
}


bool Club::modifyClubFeeType(Lint feeType)
{
	if (feeType != 0 && feeType != 1)
	{
		LLOG_ERROR("Club::modifyClubFeeType error!!!  feeType [%d]", feeType);
		return false;
	}
	
	m_feeType = feeType;
	LLOG_ERROR("modifyClubFeeType,club[%d] feetype[%d]-->[%d]",m_id,m_feeType,feeType);
	return true;
}

Lint   Club::_setAdmin(Lint type, Lint setUserId, Lint adminUserId)
{
	LLOG_DEBUG("_setAdmin type[%d] setUserId[%d],adminUserId[%d]", type, setUserId, adminUserId);
	std::stringstream ss;
	ss << "UPDATE club_users";
	ss << " SET type = '" << 2 << "'";
	ss << " WHERE clubId = '" << m_sq << "'";
	ss << " AND userId = '" << adminUserId << "'";

	LMsgLMG2CeSqlInfo send;
	send.m_type = 1;
	send.m_sql = ss.str();
	gWork.SendToCenter(send);

	//设置管理员
	m_adminUser.insert(adminUserId);
	gClubUserManager.setUserAdmin(type, m_id, adminUserId);

	return 0;
}

Lint   Club::_delAdmin(Lint type, Lint setUserId, Lint adminUserId)
{
	LLOG_DEBUG("_delAdmin type[%d] setUserId[%d],adminUserId[%d]", type, setUserId, adminUserId);

	auto ItAdmin = m_adminUser.find(adminUserId);
	if (ItAdmin != m_adminUser.end())
	{
		std::stringstream ss;
		ss << "UPDATE club_users";
		ss << " SET type = '" << 1 << "'";
		ss << " WHERE clubId = '" << m_sq << "'";
		ss << " AND userId = '" << adminUserId << "'";

		LMsgLMG2CeSqlInfo send;
		send.m_type = 1;
		send.m_sql = ss.str();
		gWork.SendToCenter(send);

		m_adminUser.erase(adminUserId);
		gClubUserManager.setUserAdmin(type, m_id, adminUserId);
		return 0;
	}
	else
	{
		return 7;
	}

}

Lint   Club::setAdmin(Lint type,Lint setUserId, Lint adminUserId)
{
	LLOG_DEBUG("setAdmin type[%d] setUserId[%d],adminUserId[%d]", type,setUserId, adminUserId);
	if (m_ownerId != setUserId)return 1;
	if (m_ownerId == adminUserId) return 6;
	//if (!isInClub(adminUserId))return 4;

	if (type == 1)    //设置管理员
	{
		return _setAdmin(type,setUserId,adminUserId);
	}
	else if (type == 2)    //取消管理员
	{
		return _delAdmin(type, setUserId, adminUserId);
	}
	else
	{
		return 2;
	}


}



//Lint Club::getClubAdmin(Lint *clubOwner, std::set<Lint> & adminUsers)
//{
//	*clubOwner = m_ownerId;
//	adminUsers = m_adminUser;
//	return 0;
//}

void  Club::LogPowerRecord(Lint type, boost::shared_ptr<UserPowerRecord>  change)
{

	if (change.get() == NULL || change==NULL)return;
	if (change->getTime() == 0 || change->getUserId() == 0 || change->getPoint() <= 0 || change->getOperId() == 0)
	{
		LLOG_ERROR("LogPowerRecord ERROR time[%d] user[%d] point[%d] operId[%d]", change->getTime(), change->getUserId(), change->getPoint(), change->getOperId());
		return;
	}
	if (CHANGE_POINT_TYPE_ADD  == type)
	{
		m_PowerAddRecord.push_front(change);
		
		//记录用户变化记录
		auto ItRecord = m_mapUserRecordAdded.find(change->getUserId());
		if (ItRecord != m_mapUserRecordAdded.end())
		{
			ItRecord->second.push_front(change);
		}
		else
		{
			std::list<boost::shared_ptr<UserPowerRecord> > recordList;
			recordList.push_front(change);
			m_mapUserRecordAdded[change->getUserId()] = recordList;
		}
		//记录用户操作记录
		auto ItOperRecord = m_mapUserAddOperRecord.find(change->getOperId());
		if (ItOperRecord != m_mapUserAddOperRecord.end())
		{
			ItOperRecord->second.push_front(change);
		}
		else
		{
			std::list<boost::shared_ptr<UserPowerRecord> > recordList;
			recordList.push_front(change);
			m_mapUserAddOperRecord[change->getOperId()] = recordList;
		}


	}
	else if (CHANGE_POINT_TYPE_REDUCE == type)
	{
		m_PowerReduceRecord.push_front(change);

		//记录用户变化记录
		auto ItRecord = m_mapUserRecordReduced.find(change->getUserId());
		if (ItRecord != m_mapUserRecordReduced.end())
		{
			ItRecord->second.push_front(change);
		}
		else
		{
			std::list<boost::shared_ptr<UserPowerRecord> > recordList;
			recordList.push_front(change);
			m_mapUserRecordReduced[change->getUserId()] = recordList;
		}
		//记录用户操作记录
		auto ItOperRecord = m_mapUserReduceOperRecord.find(change->getOperId());
		if (ItOperRecord != m_mapUserReduceOperRecord.end())
		{
			ItOperRecord->second.push_front(change);
		}
		else
		{
			std::list<boost::shared_ptr<UserPowerRecord> > recordList;
			recordList.push_front(change);
			m_mapUserReduceOperRecord[change->getOperId()] = recordList;
		}
	}

}

Lint  Club::getNotEmptyDeskCount(){
	Lint deskCount = 0;
	std::list<Lint>  clubDesk = getClubDeskList();
	for (auto ItDesk = clubDesk.begin(); ItDesk != clubDesk.end(); ItDesk++) {
		boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(*ItDesk);
		if (safeClubDeskInfo && safeClubDeskInfo->isValid())
		{
			boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
			if (!info->empty()) {
				deskCount++;
				LLOG_DEBUG("club[%d] showdesk[%d] is not empty", info->getClubId(), info->getShowDeskId());
			}
		}
	}
	return deskCount;
}

std::list<Lint>  Club::getClubDeskList()
{
	std::list<Lint>  clubList;
	for (auto ItShow = m_showDeskList.begin(); ItShow != m_showDeskList.end(); ItShow++)
	{
		auto ItDesk = m_map2ClubDesk.find(*ItShow);
		if (ItDesk   != m_map2ClubDesk.end())
			clubList.push_back(ItDesk->second);
	}
	return clubList;
}

bool Club::ChangeShowDesk(Lint deskCount)
{
	LLOG_DEBUG("ChangeShowDesk CLUB[%d] deskCount[%d]",getClubId(),deskCount);
	if (deskCount == m_showDeskList.size())return true;
	if (deskCount > m_showDeskList.size()){     //  增加
		
		for (auto ItDesk = m_map2ClubDesk.begin(); ItDesk != m_map2ClubDesk.end(); ItDesk++)
		{
			if (m_showDeskList.find(ItDesk->first)!= m_showDeskList.end())continue;

			boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(ItDesk->second);
			if (safeClubDeskInfo && safeClubDeskInfo->isValid())
			{
				boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
				if (info->isLock())info->delLock();
			}

			m_showDeskList.insert(ItDesk->first);

			if (m_showDeskList.size() >= deskCount)break;
		}
		
		if (m_showDeskList.size() < deskCount)
		{
			Lint count = deskCount - m_showDeskList.size();

			_createClubDeskList(count, getCurrPlayId());

		}
		LLOG_DEBUG("ChangeShowDesk  add   showDeskList[%d]  deskCount[%d] ", m_showDeskList.size(), deskCount);
	}
	else {    //减少
		Lint delcount = m_showDeskList.size() - deskCount;
		std::set<Lint>  delSet;
		for (auto ItShow = m_showDeskList.rbegin(); ItShow != m_showDeskList.rend(); ItShow++)
		{
			auto ItDesk = m_map2ClubDesk.find(*ItShow);
			if (ItDesk != m_map2ClubDesk.end())
			{
				boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(ItDesk->second);
				if (safeClubDeskInfo && safeClubDeskInfo->isValid())
				{
					boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
					if (info->empty())delSet.insert(*ItShow);
				}
			}

			if (delSet.size() >= delcount)break;
		}

		for (auto It = delSet.begin(); It != delSet.end(); It++)
		{
			auto ItShowId = m_showDeskList.find(*It);
			if (ItShowId != m_showDeskList.end())
			{
				m_showDeskList.erase(ItShowId);
			}
		}
		//set_difference(m_showDeskList.begin(), m_showDeskList.end(), delSet.begin(), delSet.end(), inserter(retSet, retSet.begin()));

		LLOG_DEBUG("ChangeShowDesk  del   showDeskList[%d]  deskCount[%d] delSet[%d] ", m_showDeskList.size(), deskCount, delSet.size());
		//m_showDeskList.clear();
		//m_showDeskList = retSet;

	}
	return true;
}


//std::deque<Lint> Club::reorderClubDesk(Lint playTypeId, std::list<Lint> playDeskIdList)
//{
//	LLOG_DEBUG("reorderClubDesk club[%d] playType[%d] redDesk[%d]", m_id, playTypeId, m_redPacket.getRedPacketDeskId(playTypeId));
//	std::deque<Lint> clubDeskList;
//	std::deque<Lint> emptyDeskList;
//	std::deque<Lint> waitDeskList;
//	std::deque<Lint> playDeskList;
//	std::deque<Lint> redPackDeskList;
//	Lint userCountIndesk = 0;
//	bool isfirst = false;
//	for (auto ItClubDeskId = playDeskIdList.begin(); ItClubDeskId != playDeskIdList.end(); ItClubDeskId++)
//	{
//		//桌子满，局数
//		boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfo(*ItClubDeskId);
//		if (safeClubDeskInfo && safeClubDeskInfo->isValid())
//		{
//			if (safeClubDeskInfo->getResource()->m_roomFull == 0 && safeClubDeskInfo->getResource()->m_currCircle == 0)
//			{
//				auto userList = m_clubDesk2UsersInDesk.find(*ItClubDeskId);
//				if (userList != m_clubDesk2UsersInDesk.end())
//				{
//					if (userList->second.size() == 0 && m_redPacket.getRedPacketDeskId(playTypeId) == *ItClubDeskId)
//					{
//						safeClubDeskInfo->getResource()->m_redPacketFlag = 1;
//						redPackDeskList.push_back(*ItClubDeskId);
//						continue;
//					}
//					if (userList->second.size() != 0 && m_redPacket.getRedPacketDeskId(playTypeId) == *ItClubDeskId)
//					{
//						safeClubDeskInfo->getResource()->m_redPacketFlag = 0;
//					}
//
//					if (userList->second.size() == 0)emptyDeskList.push_back(*ItClubDeskId);
//					else
//					{
//						if (userList->second.size() > userCountIndesk)
//						{
//							waitDeskList.push_front(*ItClubDeskId);
//							userCountIndesk = userList->second.size();
//						}
//						else
//							waitDeskList.push_back(*ItClubDeskId);
//					}
//				}
//
//			}
//			else
//			{
//				if (*ItClubDeskId == m_redPacket.getRedPacketDeskId(playTypeId))
//				{
//					safeClubDeskInfo->getResource()->m_redPacketFlag = 0;
//				}
//				playDeskList.push_back(*ItClubDeskId);
//			}
//		}
//	}
//
//	//对桌子重新排序
//	LTime curTime;
//	if (redPackDeskList.empty() && m_redPacket.getRedPacketDeskId(playTypeId) == 0 && !emptyDeskList.empty() && m_redPacket.haveRedPacket(curTime))
//	{
//		Lint firstEmptyDeskId = *(emptyDeskList.begin());
//		boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = getClubDeskInfo(firstEmptyDeskId);
//		if (safeClubDeskInfo && safeClubDeskInfo->isValid())
//		{
//			LLOG_DEBUG("reorderClubDesk club[%d] playType[%d] redDesk[%d] set [%d] redPacket", m_id, playTypeId, m_redPacket.getRedPacketDeskId(playTypeId), //safeClubDeskInfo->getResource()->m_showDeskId);
//			safeClubDeskInfo->getResource()->m_redPacketFlag = 1;
//			m_redPacket.setRedPacketDeskId(playTypeId, firstEmptyDeskId);
//			redPackDeskList.push_back(firstEmptyDeskId);
//			emptyDeskList.pop_front();
//		}
//	}
//	for (auto ItUseList = waitDeskList.begin(); ItUseList != waitDeskList.end(); ItUseList++)
//	{
//		isfirst = true;
//		clubDeskList.push_back(*ItUseList);
//	}
//	for (auto ItUseList = playDeskList.begin(); ItUseList != playDeskList.end(); ItUseList++)
//	{
//		clubDeskList.push_back(*ItUseList);
//	}
//	for (auto ItUseList = emptyDeskList.begin(); ItUseList != emptyDeskList.end(); ItUseList++)
//	{
//		if (!isfirst &&redPackDeskList.empty())
//		{
//			isfirst = true;
//			clubDeskList.push_front(*ItUseList);
//		}
//		else
//			clubDeskList.push_back(*ItUseList);
//	}
//	if (!redPackDeskList.empty())
//	{
//		clubDeskList.push_front(*(redPackDeskList.begin()));
//	}
//
//
//	return clubDeskList;
//}




/////////////////////////////////////////////////////////////


