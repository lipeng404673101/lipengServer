#include "UserManager.h"
//#include "clubUser.h"

bool UserManager::Init()
{
	return true;
}

bool UserManager::Final()
{
	return true;
}

void UserManager::addUser(boost::shared_ptr<User> user)
{
	if(user.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexUserQueue);
	if(m_mapId2User.find(user->m_userData.m_id) == m_mapId2User.end())
	{
		m_mapId2User[user->m_userData.m_id] = user;
		m_mapUUID2User[user->m_userData.m_unioid] = user;
	}
}

void UserManager::delUser(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexUserQueue);

	auto itUser = m_mapId2User.find(iUserId);
	if(itUser != m_mapId2User.end())
	{
		m_mapUUID2User.erase(itUser->second->m_userData.m_unioid);
		m_mapId2User.erase(itUser);
	}
}


//清空有勇士勋章标记的玩家ID集合
void UserManager::clearSetMaxMedalUserId()
{
	m_setMaxMedalUserId.clear();
}

//统计玩家需要添加勇士勋章标记
void UserManager::statisticsUserMedal()
{
	LTime t_time1, t_time2;
	t_time1.Now();

	//清理所有玩家勇士勋章的标记
	clearSetMaxMedalUserId();

	//查找需要设置勇士勋章标记的玩家集合
	m_setMaxMedalUserId = gClubManager.userStatisticsClubMedalUser();

	t_time2.Now();
	LLOG_DEBUG("UserManager::statisticsUserMedal() End... CostTimeMsec=[%d], MedalUserCount=[%d]", t_time2.MSecs() - t_time1.MSecs(),m_setMaxMedalUserId.size());
}

//根据玩家ID判断该玩家是有勇士勋章标记
bool UserManager::isMedalUserByUserId(Lint userId)
{
	if (userId <= 0)
	{
		return false;
	}

	auto t_userIdIter = m_setMaxMedalUserId.find(userId);

	if (t_userIdIter != m_setMaxMedalUserId.end())
	{
		return true;
	}

	return false;
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserByGateIdAndUUID(Lint iGateId, const Lstring& strUUID)
{
	boost::shared_ptr<User> user;

	do
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapUUID2User.find(strUUID);
		if(itUser == m_mapUUID2User.end())
		{
			break;
		}

		if(itUser->second->getUserGateID() != iGateId)
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserbyUserId(Lint iUserId)
{
	boost::shared_ptr<User> user;

	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapId2User.find(iUserId);
		if(itUser == m_mapId2User.end())
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserbyUserUUID(const Lstring& uuid)
{
	boost::shared_ptr<User> user;

	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapUUID2User.find(uuid);
		if(itUser == m_mapUUID2User.end())
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}


void UserManager::addUserBaseInfo(boost::shared_ptr<UserBaseInfo> userBaseInfo)
{
	if(userBaseInfo.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexUserBaseInfoQueue);
	m_mapUserBaseInfo[userBaseInfo->m_id] = userBaseInfo;
	m_mapUUID2UserBaseInfo[userBaseInfo->m_unionId]=userBaseInfo;
}

boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > UserManager::getUserBaseInfo(Lint iUserId)
{
	boost::shared_ptr<UserBaseInfo> userBaseInfo;
	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserBaseInfoQueue);
		auto itBaseInfo = m_mapUserBaseInfo.find(iUserId);
		if(itBaseInfo == m_mapUserBaseInfo.end())
		{
			break;
		}

		userBaseInfo = itBaseInfo->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo;
	if(userBaseInfo)
	{
		safeBaseInfo.reset(new CSafeResourceLock<UserBaseInfo>(userBaseInfo));
	}

	return safeBaseInfo;
}

boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > UserManager::getUserBaseInfoByUUID(const Lstring& strUUID)
{
	boost::shared_ptr<UserBaseInfo> userBaseInfo;
	do
	{
		boost::mutex::scoped_lock l(m_mutexUserBaseInfoQueue);
		auto itBaseInfo=m_mapUUID2UserBaseInfo.find(strUUID);
		if (itBaseInfo==m_mapUUID2UserBaseInfo.end())
		{
			break;
		}

		userBaseInfo=itBaseInfo->second;

	} while (false);

	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo;
	if (userBaseInfo)
	{
		safeBaseInfo.reset(new CSafeResourceLock<UserBaseInfo>(userBaseInfo));
	}

	return safeBaseInfo;
}

void UserManager::addUserLoginInfo(boost::shared_ptr<UserLoginInfo> userLoginInfo)
{
	if(userLoginInfo.get() == NULL)
	{
		return;
	}

	 boost::shared_ptr<UserLoginInfo> userLoginInfoBefore;

	 {
		 boost::mutex::scoped_lock l(m_mutexUserLoginInfoQueue);
		 auto itLoginInfo = m_mapUserLoginInfo.find(userLoginInfo->m_user.m_id);
		 if(itLoginInfo == m_mapUserLoginInfo.end())
		 {
			 m_mapUserLoginInfo[userLoginInfo->m_user.m_id] = userLoginInfo;
		 }
		 else
		 {
			 userLoginInfoBefore = itLoginInfo->second;
		 }

	 }

	 if(userLoginInfoBefore)  //如果存在则修改内容
	 {
		 boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > safeLoginInfo(new CSafeResourceLock<UserLoginInfo>(userLoginInfoBefore)); //对单个加锁

		 safeLoginInfo->getResource()->m_seed = userLoginInfo->m_seed;
		 safeLoginInfo->getResource()->m_time = userLoginInfo->m_time;
		 safeLoginInfo->getResource()->m_user = userLoginInfo->m_user;
	 }
}

void UserManager::delUserLoginInfo(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexUserLoginInfoQueue);
	m_mapUserLoginInfo.erase(iUserId);
}

boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > UserManager::getUserLoginInfo(Lint iUserId)
{
	boost::shared_ptr<UserLoginInfo> userLoginInfo;
	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserLoginInfoQueue);
		auto itLoginInfo = m_mapUserLoginInfo.find(iUserId);
		if(itLoginInfo == m_mapUserLoginInfo.end())
		{
			break;
		}

		userLoginInfo = itLoginInfo->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > safeLoginInfo;
	if(userLoginInfo)
	{
		safeLoginInfo.reset(new CSafeResourceLock<UserLoginInfo>(userLoginInfo));
	}

	return safeLoginInfo;
}

boost::shared_ptr<CSafeResourceLock<UserDeskList> > UserManager::getUserDeskListbyUserId(Lint iUserId)
{
	boost::shared_ptr<UserDeskList> userDeskList;
	do
	{
		boost::mutex::scoped_lock l(m_mutexUserDeskListQueue);
		auto itUserDeskList = m_mapUserDeskList.find(iUserId);
		if (itUserDeskList == m_mapUserDeskList.end())
		{
			break;
		}

		userDeskList = itUserDeskList->second;

	} while (false);

	boost::shared_ptr<CSafeResourceLock<UserDeskList> > safeuserDeskList;
	if (userDeskList)
	{
		safeuserDeskList.reset(new CSafeResourceLock<UserDeskList>(userDeskList));
	}

	return safeuserDeskList;

}


void UserManager::delUserDeskId(Lint iUserId, Lint iDeskId, Lint ifee, Lint delType)
{
	boost::shared_ptr<UserDeskList> userDeskList;
	do
	{
		boost::mutex::scoped_lock l(m_mutexUserDeskListQueue);
		auto itUserDeskList = m_mapUserDeskList.find(iUserId);
		if (itUserDeskList == m_mapUserDeskList.end())
		{
			return;
			//break;
		}

		userDeskList = itUserDeskList->second;

	} while (false);


	auto itor = userDeskList->m_DeskList.begin();
	for (; itor != userDeskList->m_DeskList.end();) {
		if (*itor == iDeskId)
		{
			//0: 只是删除桌子 1: 只是减少金币 2：减少金币和桌子

			if (delType == 1 || delType == 2)
			{
				userDeskList->m_cost -= ifee;
			}

			if (delType == 0 || delType == 2) {
				itor = userDeskList->m_DeskList.erase(itor);
			}
			break;
		}
		else {
			itor++;
		}
	}

}


void UserManager::addUserDeskId(Lint iUserId, Lint iDeskId, Lint ifee)
{
	LLOG_DEBUG("~~~~~~~~~~~~~addUserDeskId~~~~~~~~~~~~~~~~~~~");
	boost::shared_ptr<UserDeskList> userDeskList;
	bool doAdd = false;
	do
	{
		boost::mutex::scoped_lock l(m_mutexUserDeskListQueue);
		auto itUserDeskList = m_mapUserDeskList.find(iUserId);
		if (itUserDeskList == m_mapUserDeskList.end())
		{
			boost::shared_ptr<UserDeskList> tempUserDeskList(new UserDeskList);
			tempUserDeskList->m_DeskList.push_back(iDeskId);
			m_mapUserDeskList.insert(std::pair<Lint, boost::shared_ptr<UserDeskList>>(iUserId, tempUserDeskList));
			userDeskList = tempUserDeskList;
			userDeskList->m_cost += ifee;
			break;
		}
		else {
			userDeskList = itUserDeskList->second;
			userDeskList->m_cost += ifee;
			doAdd = true;
		}


	} while (false);

	if (false == doAdd) {
		return;
	}

	bool bExist = false;
	auto itor = userDeskList->m_DeskList.begin();
	for (; itor != userDeskList->m_DeskList.end(); itor++)
	{
		if (*itor == iDeskId)
		{
			bExist = true;

		}
	}

	if (!bExist) {
		userDeskList->m_DeskList.push_back(iDeskId);
	}

}

bool UserManager::verifyUserDeskID(Lint iUserId, Lint iDeskId)
{
	LLOG_DEBUG("~~~~~~~~~~~~~verifyUserDeskID~~~~~~~~~~~~~~~~~~~");
	boost::shared_ptr<UserDeskList> userDeskList;
	bool doAdd = false;
	do
	{
		boost::mutex::scoped_lock l(m_mutexUserDeskListQueue);
		auto itUserDeskList = m_mapUserDeskList.find(iUserId);
		if (itUserDeskList == m_mapUserDeskList.end())
		{
			return false;
		}
		else {
			userDeskList = itUserDeskList->second;

		}

	} while (false);

	auto itor = userDeskList->m_DeskList.begin();
	for (; itor != userDeskList->m_DeskList.end(); itor++)
	{
		if (*itor == iDeskId)
		{
			return true;
		}
	}

	return false;
}


void UserManager::broadcastAllUser(LMsgLDB2LMBCast* pMessage)
{
	boost::shared_ptr<User> user;

	do
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itor = m_mapId2User.begin();
		while (itor != m_mapId2User.end())
		{
			user = itor->second;

			if (user.get() == NULL)
			{
				continue;
			}
			LMsgS2CBcastMessage tempMessage;
			for (int i = 0; i < pMessage->m_message.size(); i++) {
				std::string temp = pMessage->m_message[i];
				tempMessage.m_MessageList.push_back(temp);
			}

			LLOG_DEBUG("send boadCast");
			user->Send(tempMessage);
			itor++;
		}

	} while (false);
}
//boost::shared_ptr<CSafeResourceLock<UserIdInfo> > UserManager::GetUserIdInfo(Lint id)
//{
//	boost::shared_ptr<UserIdInfo> user;
//	auto iter = m_userIdInfo.find(id);
//	if (iter != m_userIdInfo.end())
//	{
//		return &iter->second;
//	}
//
//	return NULL;
//}

////俱乐部会员入座，需要更新俱乐部桌子
//void  UserManager::UserAddClubDesk(LMsgL2LMGUserAddClubDesk*msg)
//{
//	LLOG_DEBUG("UserManager::UserAddClubDesk");
//	if (msg==NULL)return;
//	if (msg->m_clubId == 0) return;
//
//	//更新用户
//	gClubUserManager.userAddDesk(msg);
//	//更新俱乐部桌子
//	gClubDeskManager.addDesk(msg);
//	
//	broadcastClubScene(msg->m_clubId);
//
//	if (msg->m_clubId != 0)
//	{
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser = getUserbyUserId(msg->m_userInfo.m_userId);
//		if (safeUser && safeUser->isValid())
//		{
//			if (safeUser->getResource()->getUserLogicID() == 0 && msg->m_logicId != 0)
//			{
//				safeUser->getResource()->setUserLogicID(msg->m_logicId);
//				LLOG_ERROR("UserAddClubDesk  userId[%d],logicId[%d]", safeUser->getResource()->GetUserDataId(),msg->m_logicId);
//
//			}
//		}
//		else
//		{
//			LLOG_ERROR("UserManager::UserAddClubDesk userId=[%d],club[%d],logic[%d]", msg->m_userInfo.m_userId, msg->m_clubId, msg->m_logicId);
//		}
//	}
//
//}
//
//void  UserManager::UserLeaveClubDesk(LMsgL2LMGUserLeaveClubDesk* msg)
//{
//	if (msg==NULL)
//	{
//		return;
//	}
//
//	if (msg->m_clubId != 0)
//	{
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser = getUserbyUserId(msg->m_userId);
//		if (safeUser && safeUser->isValid())
//		{
//			safeUser->getResource()->setUserClubStatus(USER_CLUB_INIT_STATUS);
//		}
//		else
//		{
//			LLOG_ERROR("Work::UserLeaveClubDesk user not exiest %d", msg->m_userId);
//		}
//	}
//
//	LLOG_DEBUG("UserManager::UserLeaveClubDesk  clubId=[%d],playtypeId=[%d]",msg->m_clubId,msg->m_playTypeId);
//	//更新用户
//	gClubUserManager.userLeaveDesk(msg);
//	//更新俱乐部桌子
//	gClubDeskManager.leaveDesk(msg);
//	
//	broadcastClubScene(msg->m_clubId);
//}


//void  UserManager::UserListLeaveClubDesk(LMsgL2LMGUserListLeaveClubDesk* msg)
//{
//	if (msg == NULL)
//	{
//		return;
//	}
//
//	if (msg->m_clubId != 0)
//	{
//		for (int i = 0; i < msg->m_userIdList.size(); i++)
//		{
//			boost::shared_ptr<CSafeResourceLock<User> > safeUser = getUserbyUserId(msg->m_userIdList[i]);
//			if (safeUser && safeUser->isValid())
//			{
//				safeUser->getResource()->setUserClubStatus(USER_CLUB_INIT_STATUS);
//			}
//			else
//			{
//				LLOG_ERROR("Work::UserListLeaveClubDesk user not exiest %d", msg->m_userIdList[i]);
//			}
//		}
//	}
//
//	LLOG_DEBUG("UserManager::UserListLeaveClubDesk  clubId=[%d],playtypeId=[%d]", msg->m_clubId, msg->m_playTypeId);
//	//更新用户
//	gClubUserManager.userLeaveDesk(msg);
//	//更新俱乐部桌子
//	gClubDeskManager.leaveDesk(msg);
//	
//	broadcastClubScene(msg->m_clubId);
//}


//void  UserManager::FreshDeskInfo(LMsgL2LMGFreshDeskInfo* msg)
//{
//	if (msg==NULL)
//	{
//		return;
//	}
//	LLOG_DEBUG("UserManager::FreshDeskInfo  clubId=[%d],playtypeId=[%d]", msg->m_clubId, msg->m_playTypeId);
//	if (msg->m_clubId == 0)return;
//	//gClubManager.updateClubDeskInfo(msg);
//	//更新桌子信息
//	gClubDeskManager.updateDeskInfo(msg);
//
//
//	//if (msg->m_currCircle == 1)
//	//{
//	//	LMsgS2CSwitchClubScene  send;
//	//	std::vector<desksBaseInfo> desksbaseInfo;
//	//	gClubManager.switchPlayTypeScene(msg->m_clubId, msg->m_playTypeId, send, desksbaseInfo);
//	//	broadcastClubPlayTypeSceneUser(msg->m_clubId, msg->m_playTypeId, send, desksbaseInfo);
//	//}
//	//else
//	{
//		LMsgS2CFreshClubDeskInfo  send;
//		send.m_clubDeskId = msg->m_clubDeskId;
//		send.m_roomFull = msg->m_roomFull;
//		send.m_currCircle = msg->m_currCircle;
//		send.m_totalCircle = msg->m_totalCircle;
//		send.m_showDeskId = msg->m_showDeskId;
//		broadcastClubPlayTypeSceneUser(msg->m_clubId, msg->m_playTypeId, send);
//	}
//}

//void UserManager::broadcastClubScene(Lint clubId)
//{
//	LLOG_DEBUG("UserManager::broadcastClubScene clubId[%d]",clubId);
//	
//	std::set<Lint>  userInPlayTypeScene = gClubUserManager.getNeedFreshSceneUserId(clubId);
//	LLOG_DEBUG("UserManager::broadcastClubScene  clubId=[%d]user size=[%d]", clubId,userInPlayTypeScene.size());
//	for (auto ItuserInPlayType = userInPlayTypeScene.begin(); ItuserInPlayType != userInPlayTypeScene.end(); ItuserInPlayType++)
//	{
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser = getUserbyUserId(*ItuserInPlayType);
//		if (safeUser && safeUser->isValid())
//		{
//			LMsgS2CSwitchClubScene  scene;
//			gClubManager.getFreshClubScene(safeUser->getResource()->GetUserDataId(), clubId, scene);
//			safeUser->getResource()->Send(scene.GetSendBuff());
//		}
//	}
//
//}


//void UserManager::broadcastClubPlayTypeSceneUser(Lint clubId, Lint playType,   LMsgSC& msg)
//{
//	std::set<Lint>  userInPlayTypeScene = gClubUserManager.getNeedFreshSceneUserId(clubId);
//
//	LLOG_DEBUG("UserManager::broadcastClubPlayTypeSceneUser  clubId=[%d],playType=[%d],user size=[%d]", clubId, playType,userInPlayTypeScene.size());
//	for (auto ItuserInPlayType=userInPlayTypeScene.begin(); ItuserInPlayType!=userInPlayTypeScene.end(); ItuserInPlayType++)
//	{
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser=getUserbyUserId(*ItuserInPlayType);
//		if (safeUser && safeUser->isValid())
//		{
//			safeUser->getResource()->Send(msg.GetSendBuff());
//		}
//	}
//
//}
//
//void UserManager::broadcastClubPlayTypeSceneUser(Lint clubId, Lint playType, LMsgSC& msg, std::vector<desksBaseInfo>& desksbaseInfo)
//{
//
//	LLOG_DEBUG("UserManager::broadcastClubPlayTypeSceneUser");
//	std::set<Lint>  userInPlayTypeScene = gClubUserManager.getClubPlayTypeUserId(clubId, playType);
//	//gClubManager.getPlayTypeSceneUser(clubId, playType, userInPlayTypeScene);
//
//	LLOG_DEBUG("UserManager::broadcastClubPlayTypeSceneUser  clubId=[%d],playType=[%d],user size=[%d]", clubId, playType, userInPlayTypeScene.size());
//	for (auto ItuserInPlayType=userInPlayTypeScene.begin(); ItuserInPlayType!=userInPlayTypeScene.end(); ItuserInPlayType++)
//	{
//		boost::shared_ptr<CSafeResourceLock<User> > safeUser=getUserbyUserId(*ItuserInPlayType);
//		if (safeUser && safeUser->isValid())
//		{
//			safeUser->getResource()->Send(msg.GetSendBuff());
//		}
//		else
//		{
//			//LLOG_ERROR("UserManager::broadcastClubPlayTypeSceneUser user[%d] not exiest", *ItuserInPlayType);
//			continue;
//		}
//	}
//
//}
//
//
//void UserManager::checkPlayTypeJoinUserInRoom(LTime& cur)
//{
//	LLOG_ERROR("checkPlayTypeJoinUserInRoom");
//	//std::set<Lint> leaveUserList = gClubManager.checkPlayTypeJoinUserInRoom(cur);
//	//LMsgS2CPokerExitWait send;
//	//send.m_errorCode = 0;
//	//send.m_type = 1;
//	//
//	//for (auto itLeaveUser = leaveUserList.begin(); itLeaveUser != leaveUserList.end(); itLeaveUser++)
//	//{
//	//	LLOG_DEBUG("leaveUserList userId=[%d]", *itLeaveUser);
//	//	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(*itLeaveUser);
//	//	if (safeUser && safeUser->isValid())
//	//	{
//	//		boost::shared_ptr<User> user = safeUser->getResource();
//	//		user->Send(send.GetSendBuff());
//	//	}
//	//}
//}



//void UserManager::broadcastUserList(std::set<Lint> userList, LMsgSC& msg)
//{
//	boost::shared_ptr<User> user;
//
//	boost::mutex::scoped_lock l(m_mutexUserQueue);
//
//	for (auto ItUserId = userList.begin(); ItUserId != userList.end(); ItUserId++)
//	{
//		auto itor = m_mapId2User.find(*ItUserId);
//		if (itor != m_mapId2User.end())
//		{
//			user = itor->second;
//
//			if (user.get() == NULL)
//			{
//				continue;
//			}
//
//			user->Send(msg);
//		}
//
//	}
//}
