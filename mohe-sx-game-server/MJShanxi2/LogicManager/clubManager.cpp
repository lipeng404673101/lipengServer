#include "clubManager.h"
#include "mjConfig.h"
#include "Config.h"
#include "UserManager.h"



ClubManager::ClubManager()
{
}

ClubManager::~ClubManager()
{
}

bool ClubManager::Init()
{
	LLOG_DEBUG("ClubManager::Init");
	return true;
}

bool ClubManager::Final()
{
	return true;
}


boost::shared_ptr<CSafeResourceLock<Club> > ClubManager::getClubbyClubId(Lint clubId)
{
	boost::shared_ptr<Club> club;
	do
	{
		boost::mutex::scoped_lock l(m_mutexClubListQueue);

		auto itClub = m_mapClubList.find(clubId);
		if (itClub == m_mapClubList.end())
		{
			break;
		}

		club = itClub->second;

	} while (false);    //和user保持统一，不清楚当初为啥这么写

	boost::shared_ptr<CSafeResourceLock<Club> > safeClub;
	if (club)
	{
		safeClub.reset(new CSafeResourceLock<Club>(club));
	}

	return safeClub;
}

//添加俱乐部
Lint ClubManager::addClub(LMsgCe2LAddClub *msg)
{
	if (msg==NULL)return -3;

	LLOG_DEBUG("addClub: m_id=[%d],m_clubId=[%d],m_name=[%s],m_coin=[%d],m_ownerId=[%d],m_userSum=[%d],m_clubUrl=[%s],m_status=[%d],m_type=[%d],m_feeType=[%d]", msg->m_id, \
		msg->m_clubId, \
		msg->m_name.c_str(), \
		msg->m_coin, \
		msg->m_ownerId, \
		msg->m_userSum, \
		msg->m_clubUrl.c_str(), \
		msg->m_status, \
		msg->m_type, \
		msg->m_feeType);

	if (msg->m_clubId==0)
	{
		LLOG_ERROR("Fail to add club,clubId=[%d],is 0", msg->m_clubId);
		return -1;
	}
	boost::mutex::scoped_lock l(m_mutexClubListQueue);
	auto ItClub = m_mapClubList.find(msg->m_clubId);
	if (ItClub ==m_mapClubList.end())
	{
		boost::shared_ptr<Club> newClub(new Club(msg->m_clubId));
		if (newClub==NULL)
		{
			LLOG_ERROR("Fail to add club,clubId=[%d]   new Club is null", msg->m_clubId);
			return -4;
		}
		newClub->initData(msg);
		m_mapClubList[msg->m_clubId]=newClub;

		//添加玩法
		LLOG_DEBUG("ClubManager::addClub club[%d] playTypeList.size()=[%d]", msg->m_clubId, msg->playTypeList.size());
		for (auto ItPlay = msg->playTypeList.begin(); ItPlay != msg->playTypeList.end(); ItPlay++)
		{
			gClubPlayManager.addClubPlay(*ItPlay, 0);
		}


		//添加用户
		LLOG_DEBUG("ClubManager::addClub club[%d] memberList.size()=[%d]", msg->m_clubId, msg->memberList.size());
		for (auto Ituserid = msg->memberList.begin(); Ituserid != msg->memberList.end(); Ituserid++)
		{
			if (Ituserid->m_userId == msg->m_ownerId)
			{
				Ituserid->m_type = USER_IN_CLUB_TYPE_OWNER;
				LLOG_DEBUG("updateClub clubId[%d] ownerId[%d] set type =3", msg->m_clubId, msg->m_ownerId);
				gClubUserManager.addClubUser(*Ituserid);
			}
			else
			{
				gClubUserManager.addClubUser(*Ituserid);
			}
			if (Ituserid->m_type == USER_IN_CLUB_TYPE_ADMIN)
				newClub->setAdmin(1,msg->m_ownerId, Ituserid->m_userId);

		}
	}
	else
	{
		//添加用户
		LLOG_DEBUG("ClubManager::addClub  club[%d] exist memberList.size()=[%d]", msg->m_clubId, msg->memberList.size());
		for (auto Ituserid = msg->memberList.begin(); Ituserid != msg->memberList.end(); Ituserid++)
		{
			if (Ituserid->m_userId == msg->m_ownerId)
			{
				Ituserid->m_type = USER_IN_CLUB_TYPE_OWNER;
				LLOG_DEBUG("updateClub clubId[%d] ownerId[%d] set type =3", msg->m_clubId, msg->m_ownerId);
				gClubUserManager.addClubUser(*Ituserid);
			}
			else
			{
				gClubUserManager.addClubUser(*Ituserid);
			}
			if (Ituserid->m_type == USER_IN_CLUB_TYPE_ADMIN)
				ItClub->second->setAdmin(1, msg->m_ownerId, Ituserid->m_userId);

		}
	}
	return 0;
}

void ClubManager::addClubPlayType(LMsgCe2LMGClubAddPlayType *msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("addClubPlayType  msg==NULL");
		return;
	}
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_playInfo.m_clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("addClubPlayType Fail to add club playType,clubId=[%d]  is null or invalid", msg->m_playInfo.m_clubId);
		return;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();

	if (gClubPlayManager.addClubPlay(msg->m_playInfo, club->getCurrPlayId()))
	{
		club->addClubPlayType(msg->m_playInfo);
	}
	else
	{
		LLOG_ERROR("addClubPlayType  fail [%d]",msg->m_playInfo.m_id);
	}

	return;
}

//玩家申请加入俱乐部请求，由center 发到 logicManager
Lint ClubManager::_applyClubUser(clubUser & userInfo)
{
	LLOG_DEBUG("ClubManager::_applyClubUser  clubId=[%d],userid=[%d],m_status=[%d]", userInfo.m_clubId, userInfo.m_userId, userInfo.m_status);

	clubFeeType feeType;
	gClubManager.getClubFeeType(userInfo.m_clubId, feeType);

	LLOG_DEBUG("ClubManager::_applyClubUser  clubId[%d] clubOwner[%d]", userInfo.m_clubId, feeType.m_clubCreatorId);
	LMsgS2CNotifyUserJoinClub send;
	send.m_clubId = userInfo.m_clubId;

	if (feeType.m_clubCreatorId != 0)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(feeType.m_clubCreatorId);
		if (safeUser && safeUser->isValid())
		{
			LLOG_DEBUG("Work::HanderUserApplyClub  clubId[%d] clubOwner[%d]", userInfo.m_clubId, feeType.m_clubCreatorId);
			safeUser->getResource()->Send(send.GetSendBuff());
		}
	}

	if (!feeType.m_adminUser.empty())
	{
		for (auto ItUser = feeType.m_adminUser.begin(); ItUser != feeType.m_adminUser.end(); ItUser++)
		{
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(*ItUser);
			if (safeUser && safeUser->isValid())
			{
				LLOG_DEBUG("Work::HanderUserApplyClub  clubId[%d] clubAdmin[%d]", userInfo.m_clubId, *ItUser);
				safeUser->getResource()->Send(send.GetSendBuff());
			}
		}
	}

	return 0;
}

//会长或管理员同意玩家加入俱乐部
Lint ClubManager::_addClubUser(clubUser & userInfo)
{
	LLOG_DEBUG("ClubManager::addClubUser  clubId=[%d],userid=[%d],m_status=[%d]", userInfo.m_clubId, userInfo.m_userId, userInfo.m_status);

	//管理员
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub=getClubbyClubId(userInfo.m_clubId);
	if (safeClub.get()!=NULL && safeClub->isValid())
	{
		boost::shared_ptr<Club> club = safeClub->getResource();
		if (userInfo.m_type == USER_IN_CLUB_TYPE_ADMIN)
		{
			club->setAdmin(1, club->getClubOwerId(), userInfo.m_userId);
		}

		if (userInfo.m_userId == club->getClubOwerId())
		{
			userInfo.m_type = USER_IN_CLUB_TYPE_OWNER;
		}

	}
	
	Lint line = USER_IN_CLUB_OFFLINE;
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userInfo.m_userId);
	if (safeUser && safeUser->isValid())
	{
		line = USER_IN_CLUB_ONLINE;
	}
	//添加用户
	gClubUserManager.addClubUser(userInfo, line);

	return 0;
}


Lint ClubManager::_delClubUser(clubUser & userInfo)
{
	LLOG_DEBUG("ClubManager::delClubUser  clubId=[%d],userid=[%d] type[%d]", userInfo.m_clubId, userInfo.m_userId, userInfo.m_type);
	//管理员
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(userInfo.m_clubId);
	if (safeClub.get() != NULL && safeClub->isValid())
	{
		boost::shared_ptr<Club> club = safeClub->getResource();
		if (userInfo.m_type == USER_IN_CLUB_TYPE_ADMIN)
		{
			club->setAdmin(2, club->getClubOwerId(), userInfo.m_userId);
		}
	}
	
	//删除用户
	gClubUserManager.delClubUser(userInfo);

	//boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(mclubUser.m_userId);
	//if (safeClubUserInfo && safeClubUserInfo->isValid())
	//{
	//	safeClubUserInfo->getResource()->m_adminClub.erase(userInfo.m_clubId);
	//	if (safeClubUserInfo->getResource()->m_currClubInfo.m_clubId == userInfo.m_clubId)
	//	{
	//		safeClubUserInfo->getResource()->m_currClubInfo.m_clubId = 0;
	//		
	//	}
	//}

	return 0;
}

void  ClubManager::managerClubUserFromCenter(LMsgCe2LMGClubAddUser * pUserInfo)
{
	if (pUserInfo == NULL)
	{
		LLOG_ERROR("managerClubUserFromCenter msg  is null", );
		return;
	}
	//用户申请加入
	if (pUserInfo->m_msgType == CLUB_USER_MSG_TYPE_APPLY_ADD)
	{
		_applyClubUser(pUserInfo->m_userInfo);
		//setUserApplyClubUndoCount(pUserInfo->m_userInfo.m_clubId, false);
	}
	//会长或管理员确认玩家加入俱乐部
	else if (pUserInfo->m_msgType == CLUB_USER_MSG_TYPE_ADD_USER)
	{
		_addClubUser(pUserInfo->m_userInfo);
		//setUserApplyClubUndoCount(pUserInfo->m_userInfo.m_clubId, true);
	}
	else if (pUserInfo->m_msgType == CLUB_USER_MSG_TYPE_DEL_USER)
	{
		_delClubUser(pUserInfo->m_userInfo);
	}
	//会长或管理员拒绝玩家加入俱乐部
}

Lint ClubManager::alterClubName(Lint clubId, Lstring newClubName)
{
	LLOG_DEBUG("ClubManager::alterClubName  clubId=[%d],newClubName=[%s]", clubId, newClubName.c_str());

	boost::shared_ptr<CSafeResourceLock<Club> > safeClub=getClubbyClubId(clubId);
	if (safeClub.get()==NULL||!safeClub->isValid())
	{
		LLOG_ERROR("Fail to alterClubName,clubId=[%d]  is null or invalid", clubId);
		return -3;
	}

	boost::shared_ptr<Club> club=safeClub->getResource();
	club->alterClubName(newClubName);
	return 0;
}

Lint ClubManager::alterPlayTypeName(Lint clubId, Lint playTypeId, Lstring newPlayTypeName)
{
	LLOG_DEBUG("ClubManager::alterPlayTypeName  clubId[%d],playTypeId[%d],newPlayTypeName[%s]", clubId, playTypeId, newPlayTypeName.c_str());

	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("Fail to alterClubName,clubId=[%d]  is null or invalid", clubId);
		return -3;
	}

	boost::shared_ptr<Club> club = safeClub->getResource();
	club->alterPlayTypeName(playTypeId, newPlayTypeName);
	return 0;
}

Lint ClubManager::userCreateClub(LMsgC2SCreateClub*msg, User* pUser)
{
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("userCreateClub  msg==NULL or pUser==NULL");
		return -1;
	}

	//用户是否创建或加入俱乐部
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(pUser->GetUserDataId());
	if (safeUser.get() != NULL && safeUser->isValid())
	{
		//获取用户信息
		boost::shared_ptr<ClubUser> clubUser = safeUser->getResource();
		//是否对创建俱乐部数量进行限制
		
	}
	LMsgCe2LAddClub  addClub;
	addClub.m_msgType = 1;
	addClub.m_name = msg->m_name;
	addClub.m_district = msg->m_district;
	addClub.m_feeType = msg->m_fee;
	addClub.m_ownerId = pUser->GetUserDataId();
	gWork.SendToCenter(addClub);
	
	return 0;
}

Lint ClubManager::userAddClubPlay(LMsgC2SCreateDesk *msg, User* pUser)
{
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("userAddClubPlay  msg==NULL or pUser==NULL");
		return -1;
	}
	if (msg->m_clubId == 0)
	{
		LLOG_ERROR("userAddClubPlay  m_clubId=0");
		return -1;
	}


	return 0;
}

Lint  ClubManager::userEntryClub(LMsgC2SEnterClub*msg, User* pUser)
{
	if (pUser==NULL || msg==NULL)
	{
		LLOG_ERROR("userEntryClub  msg==NULL or pUser==NULL");
		return -1;
	}
	//加入的俱乐部id，如果为0，取默认俱乐部
	Lint  enterClubId = msg->m_clubId;

	LLOG_DEBUG("ClubManager::userEntryClub  clubId=[%d],userid=[%d]", enterClubId,pUser->GetUserDataId());
	LMsgS2CClubScene scene;

	Lint clubId = enterClubId;
	if (clubId == 0)
	{
		clubId = gClubUserManager.getDefaultClubId(pUser->GetUserDataId());
	}
	if (clubId == 0)   //用户没有加入任何俱乐部
	{
		LLOG_ERROR(" user not join club  yet，clubId=[%d]  ", clubId);
		scene.m_errorCode = 4;
		pUser->Send(scene.GetSendBuff());
		return  scene.m_errorCode;
	}


	//权限-是否是会员
	if (!gClubUserManager.isUserInClub(clubId, pUser->GetUserDataId()))
	{
		scene.m_errorCode = 1;
		pUser->Send(scene.GetSendBuff());
		return  scene.m_errorCode;
	}
	//是否有玩法
	boost::shared_ptr<CSafeResourceLock<Club> > tsafeClub = getClubbyClubId(clubId);
	if (tsafeClub.get() != NULL && tsafeClub->isValid())
	{
		if (!tsafeClub->getResource()->havePlayId())
		{
			scene.m_errorCode = 2;
			pUser->Send(scene.GetSendBuff());
			return  scene.m_errorCode;
		}

	}

	//获取俱乐部界面
	bool ret = getClubScene(pUser->GetUserDataId(),clubId,scene);
	
	pUser->Send(scene.GetSendBuff());
	LLOG_DEBUG(" SEND userEntryClub SENCE  clubId=[%d],userid=[%d]", clubId, pUser->GetUserDataId());
	//更改用户状态
	if (ret)
	{
		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(pUser->GetUserDataId());
		if (safeUser == NULL || !safeUser->isValid())
		{
			return  false;
		}
		//获取用户信息
		boost::shared_ptr<ClubUser> clubUser = safeUser->getResource();
		//更新用户当前俱乐部信息
		clubUser->m_currClubInfo.reset();
		clubUser->m_currClubInfo.m_clubId = clubId;
		//clubUser->m_currClubInfo.m_playTypeId = club->getCurrPlayId();

		//更新用户基本信息
		clubUser->m_nike = pUser->m_userData.m_nike;
		clubUser->m_unionId = pUser->m_userData.m_unioid;
		clubUser->m_ip = pUser->m_ip;
		clubUser->m_headImageUrl = pUser->m_userData.m_headImageUrl;
		clubUser->m_userLevel = pUser->m_userData.m_customInt[4];

		gClubUserManager.userLoginClub(clubId, pUser->GetUserDataId());
	}


	//判断等待加入房间界面,斗地主 或 升级 或 三打二
	//if (ret == 101 || ret == 106 || ret == 107 || 109 == ret || 110 == ret)
	//{
	//	boost::shared_ptr<CSafeResourceLock<clubUserInfo> > safeClubUserInfo = getClubUserInfo(pUser->GetUserDataId());
	//	if (safeClubUserInfo && safeClubUserInfo->isValid())
	//	{
	//		LLOG_DEBUG("HHHWYZ userEntryClub  userId[%d]", pUser->GetUserDataId());
	//		Lint playTypeId = safeClubUserInfo->getResource()->getCurrWaitPlayType();
	//		LLOG_DEBUG("HHHWYZ  userEntryClub  userId[%d]  playTypeId=[%d]", pUser->GetUserDataId(), playTypeId);
	//		if (playTypeId != 0)
	//		{
	//			struct PlayTypeJoinUserInfo tempPlayTypeUserInfo;
	//			if (getPlayTypeJoinUserInRoom(playTypeId, &tempPlayTypeUserInfo))
	//			{
	//				LLOG_DEBUG("HHHWYZ  userEntryClub.......................................................");
	//				LTime nowTime;
	//				LMsgS2CWaitJoinRoom  send;
	//				send.m_MaxUser = tempPlayTypeUserInfo.m_userMax;
	//				send.m_CurrentUser = tempPlayTypeUserInfo.m_currUserMax;
	//				send.m_Time = DELAY_TIME_USER_WAIT_JOIN_IN_CLUB_DESK - (nowTime.Secs() - tempPlayTypeUserInfo.m_time.Secs());
	//
	//				pUser->Send(send.GetSendBuff());
	//			}
	//		}
	//	}
	//}
		
	return 0;
}

Lint  ClubManager::userLeaveClub(LMsgC2SLeaveClub*msg, User* pUser)
{
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("userLeaveClub  msg==NULL or pUser==NULL");
		return -1;
	}
	//加入的俱乐部id，如果为0，取默认俱乐部
	Lint  leaveClubId = msg->m_clubId;

	LLOG_DEBUG("ClubManager::userLeaveClub  clubId=[%d],userid=[%d]", leaveClubId, pUser->GetUserDataId());
	LMsgS2CLeaveClub leaveClub;
	leaveClub.m_clubId = msg->m_clubId;
	leaveClub.m_PlayTypeId = msg->m_playTypeId;

	//找到用户信息
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(pUser->GetUserDataId());
	if (safeUser == NULL || !safeUser->isValid())
	{
		//未找到俱乐部用户信息
		leaveClub.m_errorCode = 0;
		pUser->Send(leaveClub.GetSendBuff());
		return 0;
	}
	boost::shared_ptr<ClubUser> clubUser = safeUser->getResource();

	//更新用户信息
	clubUser->m_currClubInfo.reset();
	//clubUser->m_lastLeaveTime = time(NULL);
	gClubUserManager.userLogoutClub(msg->m_clubId, pUser->GetUserDataId());

	//发送数据	
	gClubUserManager.getUserInAllClubCount(clubUser->m_allClubId, leaveClub.m_clubOnlineUserCount);
	pUser->Send(leaveClub.GetSendBuff());

	return 0;
}

void  ClubManager::userJoinClubDeskFromCenter(LMsgCe2LUpdateCoinJoinDesk *msg, User* pUser)
{
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("userJoinClubDeskFromCenter  msg==NULL or pUser==NULL");
		return ;
	}

	LLOG_DEBUG("userJoinClubDeskFromCenter  m_userid=[%d], m_coinNum=[%d] m_Block=[%d],m_clubMasterCoinNum=[%d],m_clubMasterBlock=[%d]", msg->m_userid, msg->m_coinNum, msg->m_Block, msg->m_clubMasterCoinNum, msg->m_clubMasterBlock);
	if (msg->m_errorCode == 1)
	{
		//俱乐部中没有用户信息
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskID;
		ret.m_errorCode = 3;
		pUser->Send(ret);
		LLOG_ERROR("ERROR: User::HanderUserEnterClubDesk center return  m_errorCode=1 not find userinfo, userid=%d", pUser->m_userData.m_id);
		return;
	}
	//信用值
	Lint limit = gDeskManager.GetDeskCreditLimit(msg->m_deskID);
	if (limit > 0)
	{
		if (pUser->m_userData.m_creditValue < limit)
		{
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskID;
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_CreditNotEnough;
			pUser->Send(ret);

			LLOG_ERROR("ERROR: userJoinClubDeskFromCenter credits not enough, userid=%d", pUser->m_userData.m_id);
			return;
		}
	}
	//被封
	if (msg->m_Block)
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskID;
		ret.m_errorCode = 3;
		pUser->Send(ret);
		LLOG_ERROR("ERROR: userJoinClubDeskFromCenterbe blocked, userid=%d", pUser->m_userData.m_id);
		return;
	}


	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(gClubDeskManager.getDeskInClubId(msg->m_deskID));
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("Fail to userJoinClubDeskFromCenter,clubId=[%d]  is null or invalid", gClubDeskManager.getDeskInClubId(msg->m_deskID));
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskID;
		ret.m_errorCode = 8;
		pUser->Send(ret);
		return;
	}

	boost::shared_ptr<Club> club = safeClub->getResource();
	club->setCard(msg->m_clubMasterCoinNum);
	Lint clubfee = club->getClubFee();
	LLOG_DEBUG("userJoinClubDeskFromCenter club[%d]  feetype=[%d]", club->getClubId(), clubfee);

	playTypeDetail tPlayDetail =gClubPlayManager.getClubPlayInfo(club->getCurrPlayId());

	//获取加入房间费用
	deskFeeItem tempItem;
	gMjConfig.GetFeeTypeValue(tPlayDetail.m_flag, tempItem, tPlayDetail.m_state);

	//检查费用
	if (0 == clubfee)  //检查会长的 ,这里不做处理,扣费的地方处理
	{
		if (msg->m_clubMasterCoinNum <= 0)   //会长钻石券不足
		{
			LLOG_ERROR("userJoinClubDeskFromCenter club  creator [%d] <= [%d] coins not enough  joinUserId=[%d]", msg->m_clubMasterCoinNum, 0, msg->m_userID);
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskID;
			ret.m_errorCode = 7;
			pUser->Send(ret);
			return;
		}
	}
	else //均摊
	{
		if (msg->m_coinNum  < tempItem.m_even)
		{
			LLOG_ERROR("userJoinClubDeskFromCenter club  userId [%d] coins not enough  ", pUser->m_userData.m_id);
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskID;
			ret.m_errorCode = 4;
			pUser->Send(ret);
			return;
		}
	}

	//检查用户是否有logicid
	Lint iLogicServerId = pUser->getUserLogicID();
	if (gWork.isLogicServerExist(iLogicServerId) && iLogicServerId != INVALID_LOGICSERVERID)
	{
		LMsgLMG2LAddToDesk send;
		send.m_userid = pUser->m_userData.m_id;
		send.m_gateId = pUser->GetUserGateId();
		send.m_strUUID = pUser->m_userData.m_unioid;
		send.m_deskID = msg->m_deskID;
		send.m_ip = msg->m_userIp;
		send.m_usert = pUser->m_userData;
		send.m_creatDeskCost = 0;
		send.m_Free = msg->m_Free;
		send.m_startTime = msg->m_startTime;
		send.m_endTime = msg->m_endTime;
		send.m_addDeskFlag = msg->m_addDeskFlag;
		send.m_isMedal = (Lint)gUserManager.isMedalUserByUserId(pUser->m_userData.m_id);
		gWork.SendMessageToLogic(iLogicServerId, send);
		return;
	}

	UserAddToClubDeskInfo send;
	send.m_userid = pUser->m_userData.m_id;
	send.m_gateId = pUser->GetUserGateId();
	send.m_strUUID = pUser->m_userData.m_unioid;
	send.m_deskID = msg->m_deskID;
	send.m_Free = msg->m_Free;
	send.m_ip = pUser->GetIp();
	send.m_usert = pUser->m_userData;
	send.m_creatDeskCost = 0;
	send.m_cost = tempItem.m_cost;
	send.m_even = tempItem.m_even;

	send.m_addDeskFlag = msg->m_addDeskFlag;

	_userEnterClubDesk(club->getClubId(), club->getCurrPlayId(), send.m_deskID, send, pUser);
	return;
}

Lint ClubManager::hideClubPlayType(Lint clubId, Lint playTypeId,Lstring playType,Lint playTypeIdStatus)
{
	LLOG_DEBUG("ClubManager::hideClubPlayType  clubId=[%d],playTypeId=[%d],playType=[%s]", clubId, playTypeId,playType.c_str());
	
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub=getClubbyClubId(clubId);
	if (safeClub.get()==NULL||!safeClub->isValid())
	{
		LLOG_ERROR("Fail to hideClubPlayType,clubId=[%d]  is null or invalid", clubId);
		return -3;
	}
	
	boost::shared_ptr<Club> club=safeClub->getResource();
	club->hidePlayType(playTypeId,playType, playTypeIdStatus);
	return 0;
}


Lint  ClubManager::_userEnterClubDesk(Lint clubId, Lint playTypeId, Lint clubDeskId, UserAddToClubDeskInfo  userInfo,User* pUser)
{
	LLOG_DEBUG("ClubManager::userEnterClubDesk  clubId=[%d],playTypeId=[%d],clubDeskId=[%d]", clubId, playTypeId, clubDeskId);

	//桌子信息
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo =gClubDeskManager.getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo.get()==NULL ||  !safeClubDeskInfo->isValid())
	{
		LLOG_ERROR("Fail to userEnterClubDesk,clubId=[%d] clubDeskId[%d]  is null or invalid,userId=[%d]", clubId, clubDeskId,userInfo.m_userid);
		return -3;
	}
	//更新用户能量值
	pUser->m_userData.m_coins = gClubUserManager.getPoint(pUser->GetUserDataId(), clubId);
	LLOG_DEBUG("ClubManager::userEnterClubDesk  user Point[%d] ", pUser->m_userData.m_coins);


	Lint realDeskId = safeClubDeskInfo->getResource()->getRealDeskId();

	if (realDeskId == 0)  //没有桌子创建
	{
		_createClubDesk(clubId, playTypeId, clubDeskId, userInfo, pUser);

	}
	else      //有桌子，直接进入
	{
		_addClubDesk(clubId, playTypeId, clubDeskId, userInfo, pUser);
	}
	
	return 0;

}

Lint  ClubManager::_createClubDesk(Lint clubId, Lint playTypeId, Lint clubDeskId, UserAddToClubDeskInfo  userInfo, User* pUser)
{
	LLOG_DEBUG("ClubManager::_createClubDesk  clubId=[%d],playTypeId=[%d],clubDeskId=[%d]", clubId, playTypeId, clubDeskId);
	//俱乐部
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("Fail to userEnterClubDesk,clubId=[%d]  is null or invalid,userId=[%d]", clubId, userInfo.m_userid);
		return -3;
	}
	//桌子信息
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo.get() == NULL || !safeClubDeskInfo->isValid())
	{
		LLOG_ERROR("Fail to userEnterClubDesk,clubId=[%d] clubDeskId[%d]  is null or invalid,userId=[%d]", clubId, clubDeskId, userInfo.m_userid);
		return -3;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();
	boost::shared_ptr<ClubDeskInfo> clubDesk = safeClubDeskInfo->getResource();

	if (clubDesk->getRealDeskId() == 0)  //没有桌子创建
	{
		//获取玩法
		playTypeDetail playDetail = gClubPlayManager.getClubPlayInfo(playTypeId);

		//申请桌子
		Lint iLogicServerId = gWork.allocateLogicServer(playDetail.m_state);	//获取负载最小的逻辑
		if (iLogicServerId == INVALID_LOGICSERVERID)
		{
			LLOG_ERROR("ClubManager::userEnterClubDesk  allocateLogicServer error userId[%d] ", pUser->GetUserDataId());
			return -3;
		}
		Lint realDeskId = gDeskManager.ClubGetFreeDeskId(iLogicServerId, 0, clubId, playTypeId, clubDeskId);

		//设置俱乐部真实桌子号
		clubDesk->setRealDeskId(realDeskId,playTypeId);

		LMsgLMG2LCreateClubDeskAndEnter send;
		//赋值
		{
			send.m_clubId = club->getClubId();
			send.m_playTypeId = playTypeId;
			send.m_clubDeskId = clubDeskId;
			send.m_showDeskId = clubDesk->getShowDeskId();
			send.m_clubName = club->getClubName();

			send.m_deskId = realDeskId;

			send.m_userid = club->getClubOwerId();
			send.m_gateId = pUser->GetUserGateId();
			send.m_strUUID = club->getClubOwerUUID();
			send.m_ip = "";
			send.m_usert = pUser->m_userData;
			send.m_usert.m_nike = club->getClubOwerNike();

			send.m_flag = playDetail.m_flag;
			send.m_secret = "";
			//send.m_gold=msg->m_gold;
			send.m_state = playDetail.m_state;
			send.m_robotNum = 0;
			send.m_playTypeCount = playDetail.m_playtype.size();
			send.m_playType = playDetail.m_playtype;
			send.m_cellscore = playDetail.m_cellscore;
			send.m_deskType = 1;                                                                      //1房主建房--->0改为自主建房
			send.m_feeType = club->getClubFee();
			send.m_cheatAgainst = playDetail.m_cheatAgainst;
			send.m_cost = userInfo.m_cost;
			send.m_even = userInfo.m_even;
			send.m_free = 0;
			send.m_startTime = 0;
			send.m_endTime = 0;
			send.m_Greater2CanStart = playDetail.m_Greater2CanStart;

			send.m_allowLookOn = playDetail.m_allowLookOn;
			send.m_clubOwerLookOn = playDetail.m_clubOwerLookOn;
			//GPS配置
			send.m_GpsLimit = playDetail.m_GpsLimit;
			send.m_type = userInfo.m_type;  //创建房间类型  3：俱乐部匹配进入创建房间

			send.m_joinUserid = pUser->GetUserDataId();
			send.m_joinUserIp = pUser->m_ip;
			send.m_joinUserGateId = pUser->GetUserGateId();
			send.m_joinUserStrUUID = pUser->m_userData.m_unioid;
			send.m_joinUserUsert = pUser->m_userData;
			send.m_joinUserFree = userInfo.m_Free;

			send.m_clubType = club->getClubType();
			send.m_minPointInRoom = playDetail.m_minPointInRoom;
			send.m_minPointXiaZhu = playDetail.m_minPointXiaZhu;
			send.m_smallZero = playDetail.m_smallZero;
			send.m_warnValue = playDetail.m_warnScore;
			send.m_isMedal = gUserManager.isMedalUserByUserId(pUser->m_userData.m_id);

			Lint owerId = 0;
			std::set<Lint> adminUsers;
			club->getClubOwnerAndAdmin(&owerId, adminUsers);
			for (auto ItAdmin = adminUsers.begin(); ItAdmin != adminUsers.end(); ItAdmin++)
				send.m_managerList.push_back(*ItAdmin);
			send.m_adminCount = send.m_managerList.size();
		}


		if (pUser)
			pUser->setUserClubStatus(USER_CLUB_DESK_STATUS);

		gWork.SendMessageToLogic(iLogicServerId, send);
		//LLOG_ERROR("Club::createDeskbyPlayType club_id=[%d] ,playTypeId=[%d] clubDeskId=[%d] realDeskId[%d] userId[%d] logicId[%d]", m_id, playTypeId, clubDeskId, send.m_deskId, send.m_joinUserid, iLogicServerId);

	}

	return 0;
}

Lint  ClubManager::_addClubDesk(Lint clubId, Lint playTypeId, Lint clubDeskId, UserAddToClubDeskInfo  userInfo, User* pUser)
{
	LLOG_DEBUG("ClubManager::userEnterClubDesk  clubId=[%d],playTypeId=[%d],clubDeskId=[%d]", clubId, playTypeId, clubDeskId);
	//俱乐部
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("Fail to userEnterClubDesk,clubId=[%d]  is null or invalid,userId=[%d]", clubId, userInfo.m_userid);
		return -3;
	}
	//桌子信息
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(clubDeskId);
	if (safeClubDeskInfo.get() == NULL || !safeClubDeskInfo->isValid())
	{
		LLOG_ERROR("Fail to userEnterClubDesk,clubId=[%d] clubDeskId[%d]  is null or invalid,userId=[%d]", clubId, clubDeskId, userInfo.m_userid);
		return -3;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();
	boost::shared_ptr<ClubDeskInfo> clubDesk = safeClubDeskInfo->getResource();

	if(clubDesk->getRealDeskId() != 0  )    //有桌子，直接进入
	{
		Lint iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(clubDesk->getRealDeskId());
		if (!gWork.isLogicServerExist(iLogicServerId))
		{
			iLogicServerId = INVALID_LOGICSERVERID;
		}
		if (iLogicServerId != INVALID_LOGICSERVERID) {
			LMsgLMG2LEnterClubDesk send;
			send.m_clubId = club->getClubOwerId();
			send.m_playTypeId = playTypeId;
			send.m_clubDeskId = clubDeskId;

			send.m_deskId = clubDesk->getRealDeskId();

			send.m_joinUserid = userInfo.m_userid;
			send.m_joinUserIp = userInfo.m_ip;
			send.m_joinUserGateId = userInfo.m_gateId;
			send.m_joinUserStrUUID = userInfo.m_strUUID;
			send.m_joinUserUsert = pUser->m_userData;
			send.m_joinUserFree = userInfo.m_Free;
			send.m_addDeskFlag = userInfo.m_addDeskFlag;
			send.m_type = userInfo.m_type;
			send.m_isMedal = gUserManager.isMedalUserByUserId(userInfo.m_userid);
			if (pUser)
				pUser->setUserClubStatus(USER_CLUB_DESK_STATUS);

			gWork.SendMessageToLogic(iLogicServerId, send);
		}
	}


	return 0;

}

void  ClubManager::broadcastClubScene(Lint clubId, Lint exUser)
{
	LLOG_DEBUG("broadcastClubScene clubId[%d]", clubId);
	std::set<Lint>  userInPlayTypeScene = gClubUserManager.getNeedFreshSceneUserId(clubId);
	for (auto ItuserInPlayType = userInPlayTypeScene.begin(); ItuserInPlayType != userInPlayTypeScene.end(); ItuserInPlayType++)
	{
		if (exUser != 0 && *ItuserInPlayType == exUser)continue;
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(*ItuserInPlayType);
		if (safeUser && safeUser->isValid())
		{
			LMsgS2CClubScene  scene;
			getClubScene(safeUser->getResource()->GetUserDataId(), clubId, scene);
			safeUser->getResource()->Send(scene.GetSendBuff());
		}
	}
}

void  ClubManager::broadcastClubMsg(Lint clubId, LMsgSC& msg, Lint userId)
{
	LLOG_DEBUG("broadcastClubMsg clubId[%d] userId[%d]", clubId,userId);
	std::set<Lint>  userInPlayTypeScene = gClubUserManager.getNeedFreshSceneUserId(clubId);
	for (auto ItuserInPlayType = userInPlayTypeScene.begin(); ItuserInPlayType != userInPlayTypeScene.end(); ItuserInPlayType++)
	{
		if (userId != 0 && *ItuserInPlayType == userId)continue;
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(*ItuserInPlayType);
		if (safeUser && safeUser->isValid())
		{
			safeUser->getResource()->Send(msg.GetSendBuff());
		}
	}
}

void  ClubManager::userAddClubDeskFromLogic(LMsgL2LMGUserAddClubDesk*msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("userAddClubDeskFromLogic  msg==NULL");
		return;
	}
	if (msg->m_clubId == 0)
	{
		LLOG_ERROR("userAddClubDeskFromLogic ERROR club[%d] == 0",msg->m_clubId);
		return;
	}

	LLOG_DEBUG("userAddClubDeskFromLogic  club[%d] deskid[%d] user[%d]", msg->m_clubId,msg->m_clubDeskId,msg->m_userInfo.m_userId);
	if (msg->m_type == 1){  //旁观
	
		gClubDeskManager.addDesk(msg);
	}
	else {              //入座
		//更新用户
		gClubUserManager.userAddDesk(msg);
		//更新俱乐部桌子
		gClubDeskManager.addDesk(msg);

		//向其他用户广播有用户加入
		broadcastClubScene(msg->m_clubId);
	}
	return;
}

void  ClubManager::userLeaveClubDeskFromLogic(LMsgL2LMGUserLeaveClubDesk* msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("userLeaveClubDeskFromLogic  msg==NULL");
		return;
	}
	if (msg->m_clubId == 0)
	{
		LLOG_ERROR("userLeaveClubDeskFromLogic ERROR club[%d] == 0", msg->m_clubId);
		return;
	}
	LLOG_DEBUG("userLeaveClubDeskFromLogic  club[%d] deskid[%d] user[%d]", msg->m_clubId, msg->m_clubDeskId, msg->m_userId);
	//更新用户
	gClubUserManager.userLeaveDesk(msg);
	//更新俱乐部桌子
	gClubDeskManager.leaveDesk(msg);

	//向其他用户广播有用户加入
	broadcastClubScene(msg->m_clubId);
	return;
}

void  ClubManager::userLeaveClubDeskFromLogic(LMsgL2LMGUserListLeaveClubDesk* msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("userLeaveClubDeskFromLogic  msg==NULL");
		return;
	}
	if (msg->m_clubId == 0)
	{
		LLOG_ERROR("userLeaveClubDeskFromLogic ERROR club[%d] == 0", msg->m_clubId);
		return;
	}
	LLOG_DEBUG("userLeaveClubDeskFromLogic  club[%d] deskid[%d] user.size[%d]", msg->m_clubId, msg->m_clubDeskId,msg->m_userIdList.size() );
	//更新用户
	gClubUserManager.userLeaveDesk(msg);
	//更新俱乐部桌子
	gClubDeskManager.leaveDesk(msg);

	//向其他用户广播有用户加入
	broadcastClubScene(msg->m_clubId);
	return;
}

void  ClubManager::freshClubDeskInfoFromLogic(LMsgL2LMGFreshDeskInfo*msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("freshClubDeskInfoFromLogic  msg==NULL");
		return;
	}
	if (msg->m_clubId == 0)
	{
		LLOG_ERROR("freshClubDeskInfoFromLogic ERROR club[%d] == 0", msg->m_clubId);
		return;
	}
	LLOG_DEBUG("freshClubDeskInfoFromLogic  club[%d] deskid[%d] ", msg->m_clubId, msg->m_clubDeskId);
	//更新桌子信息
	gClubDeskManager.updateDeskInfo(msg);

	LMsgS2CFreshClubDeskInfo  send;
	send.m_clubDeskId = msg->m_clubDeskId;
	send.m_roomFull = msg->m_roomFull;
	send.m_currCircle = msg->m_currCircle;
	send.m_totalCircle = msg->m_totalCircle;
	send.m_showDeskId = msg->m_showDeskId;
	broadcastClubMsg(msg->m_clubId,send);
	return;
}

void  ClubManager::recycleClubDeskId(LMsgL2LMGRecyleDeskID*msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("freshClubDeskInfoFromLogic  msg==NULL");
		return;
	}
	if (msg->m_clubId == 0)
	{
		LLOG_ERROR("freshClubDeskInfoFromLogic ERROR club[%d] == 0", msg->m_clubId);
		return;
	}
	LLOG_DEBUG("ClubManager::RecycleRealDeskId  clubId=[%d],playTypeId=[%d],clubDeskId=[%d],realDeskId=[%d]", msg->m_clubId, msg->m_clubDeskId, msg->m_deskID);

	recycleClubDeskId(msg->m_clubDeskId);
	return;
}

void  ClubManager::recycleClubDeskId(Lint clubDeskId)
{
	if (clubDeskId == 0)
	{
		LLOG_ERROR("recycleClubDeskId ERROR clubdeskId[%d] == 0", clubDeskId);
		return;
	}

	gClubDeskManager.recycleClubDesk(clubDeskId);
	return;
}

Lint    ClubManager::getClubFeeType(Lint clubId, clubFeeType& feeType,Lint playTypeId)
{
	LLOG_DEBUG("ClubManager::getClubFeeType   clubId=[%d] playTypeId=[%d]", clubId, playTypeId);
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub=getClubbyClubId(clubId);
	if (safeClub.get()==NULL||!safeClub->isValid())
	{
		LLOG_ERROR("Fail to userLeaveClub,clubId=[%d]  is null or invalid", clubId);
		return -3;
	}
	boost::shared_ptr<Club> club=safeClub->getResource();

	feeType.m_clubFeeType = club->getClubFee();
	club->getClubOwnerAndAdmin(&(feeType.m_clubCreatorId), feeType.m_adminUser);
	feeType.m_clubCreatorId = club->getClubOwerId();

	if (playTypeId != 0)
	{
		playTypeDetail tpDetail = gClubPlayManager.getClubPlayInfo(playTypeId);
		feeType.m_playTypeCircle = tpDetail.m_flag;
		feeType.m_state = tpDetail.m_state;
	}
	 //club->getClubFeeType(feeType,playTypeId);
	return 0;
}


void ClubManager::modifyClubDeskInfoFromLogic(const LogicDeskInfo & deskInfo)
{
	LLOG_DEBUG("updateClubDeskInfoFromLogic   clubId=[%d] playTypeId=[%d]", deskInfo.m_clubId, deskInfo.m_playTypeId);
	//更新用户数据
	for (auto ItUser=deskInfo.m_seatUser.begin(); ItUser!=deskInfo.m_seatUser.end(); ItUser++)
	{
		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(ItUser->m_userId);
		if (safeUser && safeUser->isValid())
		{
			safeUser->getResource()->m_nike = ItUser->m_nike;
			safeUser->getResource()->m_headImageUrl = ItUser->m_headUrl;
			safeUser->getResource()->m_ip = "";
		}
		
	}
	for (auto ItUser=deskInfo.m_noSeatUser.begin(); ItUser!=deskInfo.m_noSeatUser.end(); ItUser++)
	{
		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(ItUser->m_userId);
		if (safeUser && safeUser->isValid())
		{
			safeUser->getResource()->m_nike = ItUser->m_nike;
			safeUser->getResource()->m_headImageUrl = ItUser->m_headUrl;
			safeUser->getResource()->m_ip = ItUser->m_ip;
		}
	}
	//填充桌子
	gClubDeskManager.modifyClubDeskInfoFromLogic(deskInfo);
	
	return;
}



Lint ClubManager::modifyClubFeeType(Lint clubId, Lint feeType)
{
	LLOG_DEBUG("ClubManager::modifyClubFeeType  clubId=[%d],feeType=[%d]", clubId, feeType);

	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("Fail to modifyClubFeeType,clubId=[%d]  is null or invalid", clubId);
		return -3;
	}

	boost::shared_ptr<Club> club = safeClub->getResource();
	club->modifyClubFeeType(feeType);
	return 0;
}




Lint  ClubManager::getClubDeskCanJoin(Lint clubId, Lint playTypeId)
{
//	LLOG_DEBUG("ClubManager::getClubDeskCanJoin  clubId=[%d],playTypeId=[%d]", clubId, playTypeId);
//
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
//	if (safeClub.get() == NULL || !safeClub->isValid())
//	{
//		LLOG_ERROR("Fail to getClubDeskCanJoin,clubId=[%d]  is null or invalid", clubId);
//		return -3;
//	}
//
//	boost::shared_ptr<Club> club = safeClub->getResource();
//	return club->userGetDeskCanJoin(playTypeId);
	return 0;
}


boost::shared_ptr<CSafeResourceLock<PlayTypeJoinUserInfo> > ClubManager::getUserInfobyPlayTypeId(Lint playTypeId)
{
	boost::shared_ptr<PlayTypeJoinUserInfo> UserInfo;
	do
	{
		boost::mutex::scoped_lock l(m_mutexPlayTypeJoinUserInfo);

		auto itUserInfo = m_mapPlayTypeJoinUserInfo.find(playTypeId);
		if (itUserInfo == m_mapPlayTypeJoinUserInfo.end())
		{
			break;
		}

		UserInfo = itUserInfo->second;

	} while (false);    //和user保持统一，不清楚当初为啥这么写

	boost::shared_ptr<CSafeResourceLock<PlayTypeJoinUserInfo> > safeUserInfo;
	if (UserInfo)
	{
		safeUserInfo.reset(new CSafeResourceLock<PlayTypeJoinUserInfo>(UserInfo));
	}

	return safeUserInfo;
}

// 1--人满
//Lint   ClubManager::userApplyJoinPlayTypeDesk(Lint clubId, Lint playTypeId, User* pUser, struct PlayTypeJoinUserInfo *playTypeUser, Lint *isCheckGps)
//{
//	LLOG_DEBUG("ClubManager::userApplyJoinPlayTypeDesk  clubId=[%d],playTypeId=[%d]", clubId, playTypeId);
//	if (pUser == NULL)return -1;
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
//	if (safeClub.get() == NULL || !safeClub->isValid())
//	{
//		LLOG_ERROR("Fail to userApplyJoinPlayTypeDesk,clubId=[%d]  is null or invalid", clubId);
//		return -3;
//	}
//	boost::shared_ptr<Club> club = safeClub->getResource();
//
//	Lint userMax = 0;
//	playTypeDetail tempPlayTypeDetail;
//	if (!(club->IsPlayTypeValid(playTypeId, &userMax, &tempPlayTypeDetail)))
//	{
//		LLOG_ERROR("userApplyJoinPlayTypeDesk ,IsPlayTypeValid  ERROR false");
//		return -4;
//	}
//	playTypeUser->m_state = tempPlayTypeDetail.m_state;
//
//	boost::shared_ptr<CSafeResourceLock<PlayTypeJoinUserInfo> > safeUserInfo = getUserInfobyPlayTypeId(playTypeId);
//	if (safeUserInfo.get() == NULL || !safeUserInfo->isValid())
//	{
//		LLOG_DEBUG("userApplyJoinPlayTypeDesk,playTypeId=[%d]  is null or invalid", playTypeId);
//		boost::shared_ptr<PlayTypeJoinUserInfo> newPlayTypeJoinUserInfo(new PlayTypeJoinUserInfo());
//		if (newPlayTypeJoinUserInfo == NULL)
//		{
//			LLOG_ERROR("Fail to create PlayTypeUserInfo,playTypeId=[%d]   new playTypeUserInfo is null", playTypeId);
//			return -5;
//		}
//		LLOG_DEBUG("init playTypeId...");
//		newPlayTypeJoinUserInfo.get()->setUserMax(userMax);
//		newPlayTypeJoinUserInfo.get()->addUser(pUser->GetUserDataId(), pUser->m_userData.m_customString2);
//		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(pUser->GetUserDataId());
//		if (safeClubUserInfo && safeClubUserInfo->isValid())
//		{
//			safeClubUserInfo->getResource()->setCurrWaitPlayType(playTypeId);
//		}
//
//		m_mapPlayTypeJoinUserInfo[playTypeId] = newPlayTypeJoinUserInfo;
//		playTypeUser->m_userMax = newPlayTypeJoinUserInfo.get()->m_userMax;
//		playTypeUser->m_currUserMax = newPlayTypeJoinUserInfo.get()->userIdSet.size();
//		playTypeUser->m_time = newPlayTypeJoinUserInfo.get()->m_time;
//		for (auto itUser = newPlayTypeJoinUserInfo.get()->userIdSet.begin(); itUser != newPlayTypeJoinUserInfo.get()->userIdSet.end(); itUser++)
//		{
//			playTypeUser->userIdSet.insert(*itUser);
//		}
//		LLOG_DEBUG("userApplyJoinPlayTypeDesk  m_GpsLimit=[%d]", tempPlayTypeDetail.m_GpsLimit);
//		if (tempPlayTypeDetail.m_GpsLimit == 1)
//			*isCheckGps = 1;
//		playTypeUser->userGps = newPlayTypeJoinUserInfo.get()->userGps;
//		return 0;
//	}
//	else
//	{
//		boost::shared_ptr<PlayTypeJoinUserInfo> UserInfo = safeUserInfo->getResource();
//		Lint ret = UserInfo->addUser(pUser->GetUserDataId(), pUser->m_userData.m_customString2);
//		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(pUser->GetUserDataId());
//		if (safeClubUserInfo && safeClubUserInfo->isValid())
//		{
//			safeClubUserInfo->getResource()->setCurrWaitPlayType(playTypeId);
//		}
//
//		playTypeUser->m_userMax = UserInfo->m_userMax;
//		playTypeUser->m_currUserMax = UserInfo->userIdSet.size();
//		playTypeUser->m_time = UserInfo->m_time;
//		for (auto itUser = UserInfo->userIdSet.begin(); itUser != UserInfo->userIdSet.end(); itUser++)
//		{
//			playTypeUser->userIdSet.insert(*itUser);
//		}
//		if (tempPlayTypeDetail.m_GpsLimit == 1)
//			*isCheckGps = 1;
//		playTypeUser->userGps = UserInfo->userGps;
//		if (ret == 1)
//		{
//			updateClubUserPlayTypeWait0(UserInfo->userIdSet);
//			UserInfo->clearUser();
//
//			return 1;
//		}
//		return 0;
//	}
//
//}

bool    ClubManager::getPlayTypeJoinUserInRoom(Lint playTypeId, struct PlayTypeJoinUserInfo *playTypeUser)
{
	LLOG_DEBUG("playTypeJoinUserInRoom  playType=[%d]",playTypeId);

	boost::shared_ptr<CSafeResourceLock<PlayTypeJoinUserInfo> > safeUserInfo = getUserInfobyPlayTypeId(playTypeId);
	if (safeUserInfo.get() == NULL || !safeUserInfo->isValid())
	{
		LLOG_ERROR("Fail to playTypeJoinUserInRoom,playTypeId=[%d]  is null or invalid", playTypeId);
		return false;
	}
	boost::shared_ptr<PlayTypeJoinUserInfo> UserInfo = safeUserInfo->getResource();

	playTypeUser->m_clubId = UserInfo->m_clubId;
	playTypeUser->m_userMax = UserInfo->m_userMax;
	playTypeUser->m_time = UserInfo->m_time;
	for (auto itUser = UserInfo->userIdSet.begin(); itUser != UserInfo->userIdSet.end(); itUser++)
	{
		playTypeUser->userIdSet.insert(*itUser);
	}
	playTypeUser->m_currUserMax = playTypeUser->userIdSet.size();

	return true;

}

Lint    ClubManager::userApplyLeavePlayTypeDesk(Lint clubId, Lint playTypeId, User * pUser, struct PlayTypeJoinUserInfo *playTypeUser)
{
	LLOG_DEBUG("userApplyLeavePlayTypeDesk  playType=[%d]", playTypeId);

	boost::shared_ptr<CSafeResourceLock<PlayTypeJoinUserInfo> > safeUserInfo = getUserInfobyPlayTypeId(playTypeId);
	if (safeUserInfo.get() == NULL || !safeUserInfo->isValid())
	{
		LLOG_ERROR("sFail to userApplyLeavePlayTypeDesk,playTypeId=[%d]  is null or invalid", playTypeId);
		return -4;
	}
	boost::shared_ptr<PlayTypeJoinUserInfo> UserInfo = safeUserInfo->getResource();

	UserInfo->leaveUser(pUser->GetUserDataId());
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(pUser->GetUserDataId());
	if (safeClubUserInfo && safeClubUserInfo->isValid())
	{
		LLOG_DEBUG("userApplyLeavePlayTypeDesk");
		safeClubUserInfo->getResource()->setCurrWaitPlayType(0);
	}

	playTypeUser->m_userMax = UserInfo->m_userMax;
	playTypeUser->m_currUserMax = UserInfo->userIdSet.size();
	playTypeUser->m_time = UserInfo->m_time;
	for (auto itUser = UserInfo->userIdSet.begin(); itUser != UserInfo->userIdSet.end(); itUser++)
	{
		playTypeUser->userIdSet.insert(*itUser);
	}
	return 0;
}





void ClubManager::updateClubUserPlayTypeWait0(std::set<Lint> &userIds)
{

	for (auto itUserId = userIds.begin(); itUserId != userIds.end(); itUserId++)
	{
		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(*itUserId);
		if (safeClubUserInfo && safeClubUserInfo->isValid())
		{
			safeClubUserInfo->getResource()->m_currWaitPlayType = 0;
		}
	}
}

Lint   ClubManager::userCreateClubDesk(User* pUser, Lint clubId, Lint playTypeId, UserAddToClubDeskInfo & userInfo)
{
	if (pUser == NULL)return -4;
	LLOG_DEBUG("ClubManager::userCreateClubDesk  userid=[%d] clubId=[%d],playTypeId=[%d]", pUser->GetUserDataId(),clubId, playTypeId );

	//boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
	//if (safeClub.get() == NULL || !safeClub->isValid())
	//{
	//	LLOG_ERROR("Fail to userEnterClubDesk,clubId=[%d]  is null or invalid", clubId);
	//	return -3;
	//}
	//boost::shared_ptr<Club> club = safeClub->getResource();
	//club->userCreateClubDesk(pUser,playTypeId, userInfo);
	return 0;
}




Lint ClubManager::_getClubScene(Lint clubId, LMsgS2CClubScene & scene)
{
	//LMsgS2CClubScene scene;
	if (clubId == 0)
	{
		scene.m_errorCode = 3;
		return scene.m_errorCode = 3;
	}
	//获取俱乐部信息
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("Fail to _getClubScene,clubId=[%d]  is null or invalid", clubId);
		//未找到俱乐部俱乐部里面
		scene.m_errorCode = 3;
		return scene.m_errorCode = 3;
	}

	boost::shared_ptr<Club> club = safeClub->getResource();
	//俱乐部信息
	scene.m_clubId = club->getClubId();
	scene.m_clubName = club->getClubName();
	scene.m_card = club->getCard();
	scene.m_presidentId = club->getClubOwerId();
	scene.m_OnlineUserCount = gClubUserManager.getClubUserCountOnline(club->getClubId());
	scene.m_UserCount = gClubUserManager.getClubUserCount(club->getClubId());

	//玩法信息
	scene.m_playTypeDetail = gClubPlayManager.getClubPlayInfo(club->getCurrPlayId());
	scene.m_playTypeDetail.m_feeType = club->getClubFee();

	//桌子信息
	std::list<Lint>  clubDeskList =club->getClubDeskList();
	LLOG_DEBUG("club[%d] deskList.size[%d]",club->getClubId(),clubDeskList.size());
	//std::vector<Lint>  clubDeskList = _orderClubDesk(club->getClubDeskList());
	for (auto ItDesk = clubDeskList.begin(); ItDesk != clubDeskList.end(); ItDesk++)
	{
		//LLOG_DEBUG("club[%d]  desk[%d]", club->getClubId(),*ItDesk);
		scene.m_desksInfo.push_back(gClubDeskManager.getClubDeskInfo(*ItDesk,club->getCurrPlayId()));
	}
	scene.m_deskCount = scene.m_desksInfo.size();

	return 0;
}


bool  ClubManager::getClubScene(Lint userId, Lint clubId, LMsgS2CClubScene& scene)
{
	//获取俱乐部界面信息
	 Lint ret = _getClubScene(clubId,scene);

	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(userId);
	if (safeUser == NULL || !safeUser->isValid())
	{
		return  false;
	}
	//获取用户信息
	boost::shared_ptr<ClubUser> clubUser = safeUser->getResource();

	//获取用户所有俱乐部
	scene.m_myClubNum = clubUser->m_ownerClub.size();
	scene.m_joinClubNum = clubUser->m_allClubId.size();
	for (auto ItClubId = clubUser->m_allClubId.begin(); ItClubId != clubUser->m_allClubId.end(); ItClubId++)
	{
		boost::shared_ptr<CSafeResourceLock<Club> > tsafeClub = getClubbyClubId(*ItClubId);
		if (tsafeClub.get() != NULL && tsafeClub->isValid())
		{
			clubInfo tClubInfo(*ItClubId);
			tClubInfo.m_name = tsafeClub->getResource()->getClubName();
			tClubInfo.m_clubType = tsafeClub->getResource()->getClubType();
			scene.m_clubinfo.push_back(tClubInfo);
			//scene.m_clubinfo.push_back(tsafeClub->getResource()->getClubInfo());
		}
	}
	//获取用户当前俱乐部的身份
	scene.m_userState = clubUser->getUserType(clubId);
	

	if (scene.m_errorCode == 0)
		return true;
	else
		return false;
}

//Lint ClubManager::_getFreshClubScene(Lint clubId, LMsgS2CSwitchClubScene & scene)
//{
//	if (clubId == 0)
//	{
//		scene.m_errorCode = 3;
//		return scene.m_errorCode = 3;
//	}
//	//获取俱乐部信息
//	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
//	if (safeClub.get() == NULL || !safeClub->isValid())
//	{
//		LLOG_ERROR("Fail to _getClubScene,clubId=[%d]  is null or invalid", clubId);
//		//未找到俱乐部俱乐部里面
//		scene.m_errorCode = 3;
//		return scene.m_errorCode = 3;
//	}
//
//	boost::shared_ptr<Club> club = safeClub->getResource();
//	//俱乐部信息
//	scene.m_clubId = club->getClubId();
//	scene.m_clubName = club->getClubName();
//	scene.m_card = club->getCard();
//	scene.m_presidentId = club->getClubOwerId();
//	scene.m_OnlineUserCount = gClubUserManager.getClubUserCountOnline(club->getClubId());
//	scene.m_UserCount = gClubUserManager.getClubUserCount(club->getClubId());
//
//	//玩法信息
//	scene.m_playTypeDetail = gClubPlayManager.getClubPlayInfo(club->getCurrPlayId());
//
//	//桌子信息
//	std::set<Lint> deskList = club->getPlayDeskList();
//	for (auto ItDesk = deskList.begin(); ItDesk != deskList.end(); ItDesk++)
//	{
//		scene.m_desksInfo.push_back(gClubDeskManager.getClubDeskInfo(*ItDesk,club->getCurrPlayId()));
//	}
//	scene.m_deskCount = scene.m_desksInfo.size();
//
//	return 0;
//}


//bool ClubManager::getFreshClubScene(Lint userId, Lint clubId, LMsgS2CSwitchClubScene& scene)
//{
//	//获取俱乐部界面信息
//	_getFreshClubScene(clubId, scene);
//
//	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(userId);
//	if (safeUser == NULL || !safeUser->isValid())
//	{
//		return  false;
//	}
//	//获取用户信息
//	boost::shared_ptr<ClubUser> clubUser = safeUser->getResource();
//
//	//获取用户所有俱乐部
//	for (auto ItClubId = clubUser->m_allClubId.begin(); ItClubId != clubUser->m_allClubId.end(); ItClubId++)
//	{
//		boost::shared_ptr<CSafeResourceLock<Club> > tsafeClub = getClubbyClubId(*ItClubId);
//		if (tsafeClub.get() != NULL && tsafeClub->isValid())
//		{
//			clubInfo tClubInfo(*ItClubId);
//			tClubInfo.m_name = tsafeClub->getResource()->getClubName();
//			scene.m_clubinfo.push_back(tClubInfo);
//			//scene.m_clubinfo.push_back(tsafeClub->getResource()->getClubInfo());
//		}
//	}
//	//获取用户当前俱乐部的身份
//	scene.m_userState = clubUser->getUserType(clubId);
//
//	if (scene.m_errorCode == 0)
//		return true;
//	else
//		return false;
//}

Lint ClubManager::userUpdateClub(LMsgC2SUpdateClub *msg, User* pUser)
{
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("userUpdateClub  msg==NULL or pUser==NULL");
		return -1;
	}
	LLOG_ERROR("userUpdateClub club[%d] type[%d]  clubtype[%d] AdminCanChange[%d] AdminCanChange[%d] table[%d]",msg->m_clubId,msg->m_msgType,msg->m_clubType,msg->m_adminCanChange,msg->m_adminCanSeeAll,msg->m_tableCount);

	LMsgS2CUpdateClub updateClub;
	updateClub.m_clubId = msg->m_clubId;
	updateClub.m_msgType = msg->m_msgType;

	if (msg->m_clubId == 0)
	{
		updateClub.m_errorCode = 3;
		LLOG_ERROR("userUpdateClub ERROR m_clubId=0");
		pUser->Send(updateClub.GetSendBuff());
		return -1;
	}

	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		updateClub.m_errorCode = 3;
		pUser->Send(updateClub.GetSendBuff());
		return -1;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();

	//是否有权限
	if (!club->isLeader(pUser->GetUserDataId()))
	{
		updateClub.m_errorCode = 2;
		pUser->Send(updateClub.GetSendBuff());
		return -1;
	}

	//查询直接返回
	if (msg->m_msgType == 0)
	{
		LMsgS2CUpdateClub updateClub;
		updateClub.m_clubId = msg->m_clubId;
		updateClub.m_msgType = msg->m_msgType;
		updateClub.m_clubType = club->getClubType();
		updateClub.m_adminCanChange = club->getAdminCanChange();
		updateClub.m_adminCanSeeAll = club->getAdminCanSeeAll();
		updateClub.m_tableCount = club->getCurrDeskCount();

		pUser->Send(updateClub.GetSendBuff());
		return true;
	}

	//判断桌子数目
	if (msg->m_tableCount < club->getCurrDeskCount())
	{
		if (club->getNotEmptyDeskCount() > msg->m_tableCount)
		{
			LLOG_ERROR("userUpdateClub  user set club[%d]  table[%d]  < notEmptyDesk[%d]",club->getClubId(),msg->m_tableCount, club->getNotEmptyDeskCount());
			updateClub.m_errorCode = 4;
			pUser->Send(updateClub.GetSendBuff());
			return  false;
		}
	}

	
	LMsgCE2LMGClubInfo toCenter;
	toCenter.m_clubId = msg->m_clubId;
	toCenter.m_operId = pUser->GetUserDataId();
	toCenter.m_clubSq = safeClub->getResource()->getClubSq();
	toCenter.m_clubType = msg->m_clubType;
	toCenter.m_adminCanSeeAll = msg->m_adminCanSeeAll;
	toCenter.m_adminCanChange = msg->m_adminCanChange;
	toCenter.m_tableCount = msg->m_tableCount;
	if (toCenter.m_tableCount <= 0)
		toCenter.m_tableCount = club->getCurrDeskCount();

	gWork.SendToCenter(toCenter);

	return 0;
}

Lint ClubManager::userUpdatePlay(LMsgC2SUpdatePlay *msg, User *pUser)
{
	if (pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("userUpdatePlay  msg==NULL or pUser==NULL");
		return -1;
	}
	LLOG_ERROR("userUpdatePlay  userId[%d] update play  club[%d] ");

	LMsgS2CUpdatePlay updatePlay;
	updatePlay.m_clubId = msg->m_clubId;
	updatePlay.m_msgType = msg->m_msgType;
	updatePlay.m_minPointInRoom = msg->m_minPointInRoom;
	updatePlay.m_minPointXiaZhu = msg->m_minPointXiaZhu;
	updatePlay.m_smallZero = msg->m_smallZero;
	updatePlay.m_warnScore = msg->m_warnScore;
	updatePlay.m_maxLimitBo = msg->m_maxLimitBo;

	if (msg->m_clubId == 0)
	{
		LLOG_ERROR("userUpdatePlay  m_clubId=0");
		return -1;
	}

	if (msg->m_msgType == 5 && msg->m_maxLimitBo < 500)
	{
		LLOG_ERROR("ClubManager::userUpdatePlay() Err!!!  m_maxLimitBo=[%d]", msg->m_maxLimitBo);
		return -1;
	}

	//扯炫竞技场修改底皮分数，检测修改进入分数和预警分数合法性
	/*if (msg->m_msgType == 6)
	{
		boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
		if (safeClub.get() != NULL && safeClub->isValid())
		{
			boost::shared_ptr<Club> club = safeClub->getResource();
			gClubPlayManager.updatePlayType(msg, club->getCurrPlayId());
		}
		LLOG_ERROR("ClubManager::userUpdatePlay() Club ower modify dipi check warn an min in score msgType=[%d]", msg->m_msgType);
		return 0;
	}*/

	bool  isSendTrue = false;
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	if (safeClub.get() != NULL && safeClub->isValid())
	{
		boost::shared_ptr<Club> club = safeClub->getResource();
		if (gClubPlayManager.updatePlayType(msg,club->getCurrPlayId()))
		{
			isSendTrue = true;
			pUser->Send(updatePlay.GetSendBuff());
			//return 0;
		}

	}
	if(!isSendTrue)
	{
		updatePlay.m_errorCode = 3;
		pUser->Send(updatePlay.GetSendBuff());
	}
	if (isSendTrue)
	{
		broadcastClubScene(msg->m_clubId);
	}

	return 0;
}

void ClubManager::userChangePointFromCenter(LMsgLMG2CEUpdatePointRecord*msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("CUserMessage::userChangePointFromCenter");
		return;
	}

	LLOG_ERROR("userChangePointFromCenter  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", msg->m_record.m_operId, msg->m_record.m_type, msg->m_record.m_clubId, msg->m_record.m_userId, msg->m_record.m_point, msg->m_record.m_operR.c_str());

	LMsgS2CChangePoint ChangePoint;
	ChangePoint.m_clubId = msg->m_record.m_clubId;
	ChangePoint.m_userPInfo.m_userId = msg->m_record.m_userId;

	if (msg->m_errorCode != 0)
	{
		//返回充值用户-充值失败   
		boost::shared_ptr<CSafeResourceLock<User> > safeOperUser = gUserManager.getUserbyUserId(msg->m_record.m_operId);
		if (safeOperUser && safeOperUser->isValid())
		{
			ChangePoint.m_errorCode = 1;
			safeOperUser->getResource()->Send(ChangePoint.GetSendBuff());
			LLOG_ERROR("userChangePointFromCenter ERROR1 operId[%d]   type[%d] charge  userId[%d]  point[%d] r[%s] ", msg->m_record.m_operId, msg->m_record.m_type, msg->m_record.m_userId, msg->m_record.m_point, msg->m_record.m_operR.c_str());
		}
		return;
	}

	//用户改变能量值
	Lint ret = 0;
	if (CHANGE_POINT_TYPE_ADD == msg->m_record.m_type)
	{
		ret = gClubUserManager.addPoint(msg->m_record.m_userId, msg->m_record.m_clubId, msg->m_record.m_point);
	}
	else if (CHANGE_POINT_TYPE_REDUCE == msg->m_record.m_type)
	{
		ret = gClubUserManager.reducePoint(msg->m_record.m_userId, msg->m_record.m_clubId, msg->m_record.m_point);
	}

	//用户当前能量值
	ChangePoint.m_userPInfo.m_point = gClubUserManager.getPoint(msg->m_record.m_userId, msg->m_record.m_clubId);

	if (ret == 0)
	{
		//填充记录数据
		boost::shared_ptr<UserPowerRecord> change(new UserPowerRecord(time(NULL), msg->m_record.m_userId, msg->m_record.m_type, msg->m_record.m_point, msg->m_record.m_operId));
		//充值原因
		change->m_operR = msg->m_record.m_operR;
		//给who充值
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_record.m_userId);
		if (safeUser && safeUser->isValid())
		{
			change->m_nike = safeUser->getResource()->m_userData.m_nike;
			change->m_photo = safeUser->getResource()->m_userData.m_headImageUrl;

		}
		//操作者
		boost::shared_ptr<CSafeResourceLock<User> > safeOperUser = gUserManager.getUserbyUserId(msg->m_record.m_operId);
		if (safeOperUser && safeOperUser->isValid())
		{
			change->m_operNike = safeOperUser->getResource()->m_userData.m_nike;
			change->m_operPhoto = safeOperUser->getResource()->m_userData.m_headImageUrl;

		}

		//记录用户数据
		boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_record.m_clubId);
		if (safeClub.get() != NULL && safeClub->isValid())
		{
			boost::shared_ptr<Club> club = safeClub->getResource();
			club->LogPowerRecord(msg->m_record.m_type, change);
		}


		//充值并记录成功，发送给客户端
		if (safeOperUser && safeOperUser->isValid())
		{
			safeOperUser->getResource()->Send(ChangePoint.GetSendBuff());
		}
		//如果用户在游戏中，更新
		boost::shared_ptr<CSafeResourceLock<User> > safeUser1 = gUserManager.getUserbyUserId(msg->m_record.m_userId);
		if (safeUser1 && safeUser1->isValid())
		{
			boost::shared_ptr<User> user = safeUser1->getResource();
			if (user->getUserLogicID() > 0 && msg->m_record.m_type == CHANGE_POINT_TYPE_ADD)  //在牌局中只有加能量实时更新
			{
				LMsgLMG2LUpdatePointCoins  updatePoint;
				updatePoint.m_userId = user->GetUserDataId();
				updatePoint.m_currCoins = ChangePoint.m_userPInfo.m_point;
				updatePoint.m_changeCoins = msg->m_record.m_point;
				updatePoint.m_clubId = msg->m_record.m_clubId;
				if (msg->m_record.m_type == CHANGE_POINT_TYPE_REDUCE)
				{
					updatePoint.m_changeCoins = (-1)*msg->m_record.m_point;
				}
				
				gWork.SendMessageToLogic(user->getUserLogicID(), updatePoint);
			}

		}


		LLOG_ERROR("userChangePointFromCenter success  operId[%d]   type[%d] charge  userId[%d]  point[%d] r[%s]   curPoint[%d]", msg->m_record.m_operId, msg->m_record.m_type, msg->m_record.m_userId, msg->m_record.m_point, msg->m_record.m_operR.c_str(), ChangePoint.m_userPInfo.m_point, ChangePoint.m_userPInfo.m_point);

	}
	else
	{
		//返回充值用户-充值失败   
		boost::shared_ptr<CSafeResourceLock<User> > safeOperUser = gUserManager.getUserbyUserId(msg->m_record.m_operId);
		if (safeOperUser && safeOperUser->isValid())
		{
			ChangePoint.m_errorCode = 1;
			safeOperUser->getResource()->Send(ChangePoint.GetSendBuff());
			LLOG_ERROR("userChangePointFromCenter ERROR2 operId[%d]   type[%d] charge  userId[%d]  point[%d] r[%s] ", msg->m_record.m_operId, msg->m_record.m_type, msg->m_record.m_userId, msg->m_record.m_point, msg->m_record.m_operR.c_str());
		}
		return;
	}
	return;

}

void  ClubManager::userRequestClubMember(LMsgC2SRequestClubMemberList *msg, User *pUser)
{
	if (msg == NULL || pUser==NULL)
	{
		return;
	}

	LMsgS2CClubMemberList  send;
	send.m_ClubId = msg->m_ClubId;
	send.m_Time = time(NULL);
	//send.m_Page = msg->m_Page;

	send.m_OnlineUserCount = gClubUserManager.getClubUserCountOnline(msg->m_ClubId);
	send.m_UserCount = gClubUserManager.getClubUserCount(msg->m_ClubId);

	LLOG_ERROR("HanderUserRequestClubMember userId[%d] clubId[%d] oldUser.size[%d] search[%s] onlineUserCount[%d] userCount[%d]", pUser->GetUserDataId(), msg->m_ClubId, msg->m_oldUser.size(), msg->m_Search.c_str(), send.m_OnlineUserCount, send.m_UserCount);

	std::vector<boost::shared_ptr<ClubUser>  > pageUser = _sortClubUserList(msg->m_ClubId, msg->m_Search, msg->m_oldUser, pUser->GetUserDataId());

	for (auto itUserInfo = pageUser.begin(); itUserInfo != pageUser.end(); itUserInfo++)
		{
			if ((*itUserInfo) == NULL)continue;
			ClubMember  tempClubMember;
			tempClubMember.m_Id = (*itUserInfo)->getID();
			tempClubMember.m_Nike = (*itUserInfo)->m_nike;
			tempClubMember.m_photo = (*itUserInfo)->m_headImageUrl;
			tempClubMember.m_State = (*itUserInfo)->getClientState();
			tempClubMember.m_LastTime = (*itUserInfo)->m_offLineTime;
			if ((*itUserInfo)->m_offLineTime == 0)
				tempClubMember.m_LastTime = time(NULL);
			tempClubMember.m_status = (*itUserInfo)->getUserType(msg->m_ClubId);
			LLOG_DEBUG("HanderUserRequestClubMember :userId[%d],nike[%s],state[%d],lastTime[%d] type[%d]", tempClubMember.m_Id, tempClubMember.m_Nike.c_str(), tempClubMember.m_State, tempClubMember.m_LastTime, tempClubMember.m_status);
			send.m_MemberList.push_back(tempClubMember);
		}
	send.m_Count = send.m_MemberList.size();

	if (pageUser.size() < CLUB_USER_PAGE_COUNT)
	{
		send.m_HasSendAll = 1;
	}

	pUser->Send(send.GetSendBuff());

}

// 查询记录从内存中取
/*
void ClubManager::userRequestPointRecord(LMsgC2SRequestPointRecord*msg, User *pUser)
{
	if (msg == NULL || pUser==NULL)
	{
		LLOG_ERROR("userRequestPointRecord msg or pUser is null");
		return;
	}

	LLOG_DEBUG("userRequestPointRecord  userId[%d] request club[%d] searchId[%d] type[%d] ", pUser->GetUserDataId(), msg->m_clubId, msg->m_searchId, msg->m_type);

	LMsgS2CReplyPointRecord  replyRecord;
	replyRecord.m_clubId = msg->m_clubId;
	replyRecord.m_type = msg->m_type;

	//查询用户是否在俱乐部中
	if (!gClubUserManager.isUserInClub(msg->m_clubId, pUser->GetUserDataId()))
	{
		replyRecord.m_clubId = msg->m_clubId;
		replyRecord.m_errorCode = 3;
		pUser->Send(replyRecord.GetSendBuff());
		LLOG_ERROR("userRequestPointRecord ERROR USER NOT IN CLUB userId[%d] request club[%d] searchId[%d] ", pUser->GetUserDataId(), msg->m_clubId, msg->m_searchId);
		return;
	}

	//俱乐部是否存在
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		replyRecord.m_errorCode = 4;
		pUser->Send(replyRecord.GetSendBuff());
		LLOG_ERROR("userRequestPointRecord ERROR CLUB  INFO NULL userId[%d] request club[%d] searchId[%d] ", pUser->GetUserDataId(), msg->m_clubId, msg->m_searchId);
		return;
	}

	//用户当前能量值
	replyRecord.m_point = gClubUserManager.getPoint(pUser->GetUserDataId(), msg->m_clubId);

	boost::shared_ptr<Club> club = safeClub->getResource();
	std::list<boost::shared_ptr<UserPowerRecord> > recordList;

	//获取记录页面的类型
	Lint mode = club->getRecordMode(pUser->GetUserDataId());
	replyRecord.m_mode = mode;

	if (1 == mode)  //被动看结果
	{
		recordList = club->getUserRecord(pUser->GetUserDataId(), msg->m_type);
		for (auto ItRecord = recordList.begin(); ItRecord != recordList.end(); ItRecord++)
		{
			UserPowerRecord  tRecord;
			tRecord.setMode1(*ItRecord, gClubUserManager.getPoint(pUser->GetUserDataId(), replyRecord.m_clubId));
			replyRecord.m_List.push_back(tRecord);
		}

	}
	else if (2 == mode)
	{
		recordList = club->getUserOperRecord(pUser->GetUserDataId(), msg->m_type);
		for (auto ItRecord = recordList.begin(); ItRecord != recordList.end(); ItRecord++)
		{
			UserPowerRecord  tRecord;
			tRecord.setMode2(*ItRecord);
			replyRecord.m_List.push_back(tRecord);
		}
	}
	else if (3 == mode)
	{
		recordList = club->getClubOperRecord(msg->m_type);
		for (auto ItRecord = recordList.begin(); ItRecord != recordList.end(); ItRecord++)
		{
			if (*ItRecord == NULL || ItRecord->get() == NULL)continue;
			UserPowerRecord  tRecord;
			tRecord.setMode2(*ItRecord);
			replyRecord.m_List.push_back(tRecord);
		}
	}

	//发送数据
	replyRecord.m_count = replyRecord.m_List.size();
	replyRecord.m_HasSendAll = 1;
	pUser->Send(replyRecord.GetSendBuff());

	return;
}
*/

//查询记录从数据库中读取
void ClubManager::userRequestPointRecord(LMsgC2SRequestPointRecord*msg, User *pUser)
{
	if (msg == NULL || pUser == NULL)
	{
		LLOG_ERROR("ClubManager::userRequestPointRecord() Error!!! msg or pUser is null");
		return;
	}

	LLOG_DEBUG("ClubManager::userRequestPointRecord() Run... userId[%d] request club[%d] searchId[%s] type[%d] ", pUser->GetUserDataId(), msg->m_clubId, msg->m_search.c_str(), msg->m_type);

	LMsgS2CReplyPointRecord  replyRecord;
	replyRecord.m_clubId = msg->m_clubId;
	replyRecord.m_type = msg->m_type;

	//查询用户是否在俱乐部中
	if (!gClubUserManager.isUserInClub(msg->m_clubId, pUser->GetUserDataId()))
	{
		replyRecord.m_clubId = msg->m_clubId;
		replyRecord.m_errorCode = 3;
		pUser->Send(replyRecord.GetSendBuff());

		LLOG_DEBUG("ClubManager::userRequestPointRecord() Error!!! USER NOT IN CLUB userId[%d] request club[%d] searchId[%s] type[%d] ", 
			pUser->GetUserDataId(), msg->m_clubId, msg->m_search.c_str(), msg->m_type);
		return;
	}

	//俱乐部是否存在
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		replyRecord.m_errorCode = 4;
		pUser->Send(replyRecord.GetSendBuff());

		LLOG_DEBUG("ClubManager::userRequestPointRecord() Error!!! ERROR CLUB  INFO NULL userId[%d] request club[%d] searchId[%s] type[%d] ",
			pUser->GetUserDataId(), msg->m_clubId, msg->m_search.c_str(), msg->m_type);
		return;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();

	//模糊查询，将符合条件的玩家ID筛选出来到searchList
	std::vector<Lint> searchList;
	if (!msg->m_search.empty())
	{
		std::set<boost::shared_ptr<ClubUser>> userNikeSet;
		userNikeSet = gClubUserManager.getClubUserbyUserNike(msg->m_clubId, msg->m_search);
		for (auto t_itUserNike = userNikeSet.begin(); t_itUserNike != userNikeSet.end(); t_itUserNike++)
		{
			if (*t_itUserNike)
			{
				searchList.push_back((*t_itUserNike)->getID());
			}
		}
		
		Lint searchId = atoi(msg->m_search.c_str());
		if (searchId != 0)
		{
			boost::shared_ptr<CSafeResourceLock<ClubUser>> safeClubUserInfo = gClubUserManager.getClubUserbyClubIdUserId(msg->m_clubId, searchId);
			if (safeClubUserInfo && safeClubUserInfo->isValid())
			{
				searchList.push_back(safeClubUserInfo->getResource()->getID());
			}
		}
	}

	if (!msg->m_search.empty() && searchList.empty())
	{
		LLOG_ERROR("userRequestPointRecord  userId[%d] request club[%d] search[%s] searchList[%d] ", pUser->GetUserDataId(), msg->m_clubId, msg->m_search.c_str(), searchList.size());
		pUser->Send(replyRecord);
		return;
	}

	LMsgLMG2CERecordLog  toCenter;
	toCenter.m_clubId = club->getClubId();
	toCenter.m_userId = pUser->GetUserDataId();
	toCenter.m_clubSq = club->getClubSq();
	toCenter.m_type = msg->m_type;
	toCenter.m_time = msg->m_time;
	toCenter.m_searchCount = searchList.size();
	toCenter.m_search = searchList;
	toCenter.m_mode = club->getRecordMode(pUser->GetUserDataId());

	gWork.SendToCenter(toCenter);
}

//统计俱乐部中勋章玩家
std::set<Lint> ClubManager::userStatisticsClubMedalUser()
{
	//查找每个俱乐部中最大赢的ID
	std::set<Lint> t_clubMaxMedalId;
	std::list<Lint> t_clubMaxMedalIdList;
	Lint t_userId = 0;
	for (auto t_iter = m_mapClubList.begin(); t_iter != m_mapClubList.end(); t_iter++)
	{
		t_clubMaxMedalIdList.clear();
		if (t_iter->first != 0)
		{
			t_clubMaxMedalIdList = gClubUserManager.getMaxPointUserId(t_iter->first);
			
			for (auto t_clubMaxMedalUserIdIter = t_clubMaxMedalIdList.begin(); t_clubMaxMedalUserIdIter != t_clubMaxMedalIdList.end(); t_clubMaxMedalUserIdIter++)
			{
				t_clubMaxMedalId.insert(*t_clubMaxMedalUserIdIter);
			}
		}
	}

	//清除俱乐部中玩家之前赢的能量值
	gClubUserManager.clearClubUserMedal();

	return t_clubMaxMedalId;
}

void ClubManager::userRequestPointRecordFromCenter(LMsgCE2LMGRecordLog *msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("userRequestPointRecordFromCenter msg==NULL");
		return;
	}
	LLOG_ERROR("userRequestPointRecordFromCenter   user[%d] search[%d]  mode[%d] request clubid[%d] clubsq[%d] type[%d] record[%d] ", msg->m_userId, msg->m_search.size(), msg->m_mode, msg->m_clubId, msg->m_clubSq, msg->m_type, msg->m_List.size());

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userId);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("userRequestPointRecordFromCenter  user[%d] is null", msg->m_userId);
		return;
	}
	boost::shared_ptr<User> pUser = safeUser->getResource();

	LMsgS2CReplyPointRecord  replyRecord;
	replyRecord.m_clubId = msg->m_clubId;
	replyRecord.m_type = msg->m_type;
	replyRecord.m_mode = msg->m_mode;

	//俱乐部是否存在
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		replyRecord.m_errorCode = 4;
		pUser->Send(replyRecord.GetSendBuff());
		LLOG_ERROR("userRequestPointRecord ERROR CLUB  INFO NULL userId[%d] request club[%d] searchId[%d] ", pUser->GetUserDataId(), msg->m_clubId, msg->m_userId);
		return;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();

	//用户当前能量值
	replyRecord.m_point = gClubUserManager.getPoint(pUser->GetUserDataId(), msg->m_clubId);

	//对结构体赋值
	for (auto ItRecord = msg->m_List.begin(); ItRecord != msg->m_List.end(); ItRecord++)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(ItRecord->m_userId);
		if (safeUser && safeUser->isValid())
		{
			ItRecord->m_nike = safeUser->getResource()->m_userData.m_nike;
			ItRecord->m_photo = safeUser->getResource()->m_userData.m_headImageUrl;
		}

		boost::shared_ptr<CSafeResourceLock<User> > safeOperUser = gUserManager.getUserbyUserId(ItRecord->m_operId);
		if (safeOperUser && safeOperUser->isValid())
		{
			ItRecord->m_operNike = safeOperUser->getResource()->m_userData.m_nike;
			ItRecord->m_operPhoto = safeOperUser->getResource()->m_userData.m_headImageUrl;
		}

	}
	
	//获取记录页面的类型
	Lint mode = msg->m_mode;                   // club->getRecordMode(pUser->GetUserDataId());
	replyRecord.m_mode = mode;

	if (1 == mode)  //被动看结果
	{
	
		for (auto ItRecord = msg->m_List.begin(); ItRecord != msg->m_List.end(); ItRecord++)
		{
			UserPowerRecord  tRecord;
			tRecord.setMode1(*ItRecord, gClubUserManager.getPoint(ItRecord->m_userId, replyRecord.m_clubId));
			replyRecord.m_List.push_back(tRecord);
		}

	}
	else if (2 == mode)
	{
		for (auto ItRecord = msg->m_List.begin(); ItRecord != msg->m_List.end(); ItRecord++)
		{
			UserPowerRecord  tRecord;
			tRecord.setMode2(*ItRecord);
			replyRecord.m_List.push_back(tRecord);
		}
	}
	else if (3 == mode)
	{
		for (auto ItRecord = msg->m_List.begin(); ItRecord != msg->m_List.end(); ItRecord++)
		{
			UserPowerRecord  tRecord;
			tRecord.setMode2(*ItRecord);
			replyRecord.m_List.push_back(tRecord);
		}
	}

	//发送数据
	replyRecord.m_count = replyRecord.m_List.size();
	//replyRecord.m_HasSendAll = 1;
	pUser->Send(replyRecord.GetSendBuff());

	LLOG_ERROR("userRequestPointRecordFromCenter   user[%d] search[%d]  mode[%d] point[%d] request clubid[%d] clubsq[%d] type[%d] record[%d] ", msg->m_userId, msg->m_search.size(), replyRecord.m_mode, replyRecord.m_point, replyRecord.m_clubId, msg->m_clubSq, replyRecord.m_type, replyRecord.m_List.size());
	return;
}

void ClubManager::userChangePoint(LMsgC2SChangePoint*msg, User* pUser)
{
	if (msg == NULL|| pUser==NULL)
	{
		LLOG_ERROR("HanderUserChange msg is null or pUse is null");
		return;
	}

	LLOG_ERROR("HanderUserChange  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());

	LMsgS2CChangePoint ChangePoint;
	ChangePoint.m_clubId = msg->m_clubId;

	//冲的能量值为负
	if (msg->m_point <= 0 || (msg->m_type != CHANGE_POINT_TYPE_ADD && msg->m_type != CHANGE_POINT_TYPE_REDUCE))
	{
		ChangePoint.m_errorCode = 1;
		pUser->Send(ChangePoint.GetSendBuff());
		LLOG_ERROR("HanderUserChange ERROR type or point<=0 operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
		return;
	}

	//权限
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		ChangePoint.m_errorCode = 2;
		pUser->Send(ChangePoint.GetSendBuff());
		LLOG_ERROR("HanderUserChange ERROR club null operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
		return;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();
	if (!club->canChangePoint(pUser->GetUserDataId()))   //没有权限
	{
		ChangePoint.m_errorCode = 4;
		pUser->Send(ChangePoint.GetSendBuff());
		LLOG_ERROR("HanderUserChange ERROR  no  quanxian  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
		return;
	}

	//是否是会员
	if (!gClubUserManager.isUserInClub(msg->m_clubId, msg->m_userId))
	{
		ChangePoint.m_errorCode = 3;
		pUser->Send(ChangePoint.GetSendBuff());
		LLOG_ERROR("HanderUserChange ERROR user not in club  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
		return;
	}
	if (!gClubUserManager.isUserInClub(msg->m_clubId, pUser->GetUserDataId()))
	{
		ChangePoint.m_errorCode = 3;
		pUser->Send(ChangePoint.GetSendBuff());
		LLOG_ERROR("HanderUserChange ERROR operId not in club  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
		return;
	}

	//减分没有原因
	if (msg->m_type == CHANGE_POINT_TYPE_REDUCE && msg->m_operR.empty())
	{
		ChangePoint.m_errorCode = 5;
		pUser->Send(ChangePoint.GetSendBuff());
		LLOG_ERROR("HanderUserChange ERROR no reson  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
		return;
	}
	if (msg->m_type == CHANGE_POINT_TYPE_REDUCE)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userId);
		//减能量，用户在房间中
		if (safeUser && safeUser->isValid()&& safeUser->getResource()->getUserLogicID()>0)
		{
			ChangePoint.m_errorCode = 6;
			pUser->Send(ChangePoint.GetSendBuff());
			LLOG_ERROR("HanderUserChange ERROR user in room operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
			return;
		}

		Lint point = gClubUserManager.getPoint(msg->m_userId, msg->m_clubId);
		if (point <= 0)
		{
			ChangePoint.m_errorCode = 7;
			pUser->Send(ChangePoint.GetSendBuff());
			LLOG_ERROR("HanderUserChange ERROR user point <=0  operId[%d]   type[%d] charge club[%d] userId[%d]  point[%d] r[%s]", pUser->GetUserDataId(), msg->m_type, msg->m_clubId, msg->m_userId, msg->m_point, msg->m_operR.c_str());
			return;
		}
	}


	//发生center update insert 数据库,从center 返回相同的消息，再发给客户端
	LMsgLMG2CEUpdatePointRecord  send;
	UserPowerRecord  record(time(NULL), msg->m_userId, msg->m_type, msg->m_point, pUser->GetUserDataId());
	record.m_operR = msg->m_operR;
	record.m_clubId = club->getClubId();
	record.m_clubSq = club->getClubSq();

	send.m_record = record;
	gWork.SendToCenter(send);

	return;
}


void ClubManager::userSetClubAdministrtor(LMsgC2SSetClubAdministrtor* msg, User* pUser)
{
	if (msg == NULL || pUser ==NULL)
	{
		LLOG_ERROR("userSetClubAdministrtor msg is null or pUser is null");
		return;
	}
	LMsgS2CSetClubAdministrtor send;
	send.m_ClubId = msg->m_ClubId;
	send.m_type = msg->m_type;

	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_ClubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("userSetClubAdministrtor,clubId=[%d]  is null or invalid", msg->m_ClubId);
		send.m_error_code = 3;
		pUser->Send(send.GetSendBuff());
		return;
	}
	if (msg->m_type != 1 && msg->m_type != 2)
	{
		LLOG_ERROR("userSetClubAdministrtor ERROR  type[%d]  ", msg->m_type);
		send.m_error_code = 2;
		pUser->Send(send.GetSendBuff());
		return;
	}
	if (!gClubUserManager.isUserInClub(msg->m_ClubId,msg->m_adminUserId))
	{
		LLOG_ERROR("userSetClubAdministrtor ERROR user[%d] not in clubId=[%d] ", msg->m_adminUserId,msg->m_ClubId);
		send.m_error_code = 4;
		pUser->Send(send.GetSendBuff());
		return;
	}

	boost::shared_ptr<Club> club = safeClub->getResource();
	send.m_error_code = club->setAdmin(msg->m_type, pUser->GetUserDataId(), msg->m_adminUserId);

	LLOG_DEBUG("userSetClubAdministrtor, ERROR_CODE=%d", send.m_error_code);
	pUser->Send(send.GetSendBuff());

	if (send.m_error_code == 0)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_adminUserId);
		if (safeUser && safeUser->isValid())
		{
			LMsgS2CClubScene  scene;
			getClubScene(msg->m_adminUserId, msg->m_ClubId, scene);
			safeUser->getResource()->Send(scene.GetSendBuff());
		}
	}
	return;
}

void   ClubManager::userJoinClubDesk(LMsgC2SAddDesk* msg, User* pUser, Lint addDeskType)
{
	if (msg == NULL || pUser ==NULL )
	{
		return;
	}
	
	//根据桌子获取俱乐部信息
	Lint clubId = 0;
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(msg->m_deskId);
	if (safeClubDeskInfo && safeClubDeskInfo->isValid())
	{
		boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
		if (info->isLock())
		{
			LLOG_ERROR("userJoinClubDesk  clubdeskId=[%d] not find clubId", msg->m_deskId);
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskId;
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_DESK_LOCK;
			pUser->Send(ret);
			return;
		}
		clubId = info->getClubId();
	}
	if (clubId == 0)
	{
		LLOG_ERROR("userJoinClubDesk  clubdeskId=[%d] not find clubId", msg->m_deskId);
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return;
	}

	//取得俱乐部的付费信息
	Lint  feeType = 0;
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(clubId);
	if (safeClub.get() == NULL || !safeClub->isValid())
	{
		LLOG_ERROR("userJoinClubDesk  clubdeskId=[%d]  club[%d]  not find clubIdInfo", msg->m_deskId,clubId);
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return;
	}
	boost::shared_ptr<Club> club = safeClub->getResource();
	

	////取得俱乐部的付费信息
	//clubFeeType  tempClubFeeType;
	//Lint ret = gClubManager.getClubFeeType(clubId, tempClubFeeType); //0:俱乐部会长付费 1:玩家分摊
	//if (ret<0)
	//{
	//	LLOG_ERROR("userJoinClubDesk  userId[%d] club[%d]  ret [%d] < 0", pUser->GetUserDataId(), clubId, ret);
	//	return;
	//}

	LMsgL2CeUpdateCoinJoinDesk centerSend;
	centerSend.m_serverID = gConfig.GetServerID();
	centerSend.m_openID = pUser->m_userData.m_openid;
	centerSend.m_userid = pUser->GetUserDataId();
	centerSend.m_gateId = pUser->GetUserGateId();
	centerSend.m_strUUID = pUser->m_userData.m_unioid;
	centerSend.m_deskID = msg->m_deskId;
	centerSend.m_ip = pUser->GetIp();
	centerSend.m_usert = pUser->m_userData;
	centerSend.m_clubMasterId = club->getClubOwerId();
	centerSend.m_addDeskFlag = addDeskType;
	centerSend.m_type = 1;

	gWork.SendToCenter(centerSend);

	return;
}

void ClubManager::updateClubUserPointFromLogic(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg)
{
	if (msg == NULL)return;
	LLOG_DEBUG("updateClubUserPointFromLogic flag[%d] clubid[%d]  playerCount[%d]", msg->m_flag, msg->m_clubId, msg->m_playerCount);
	if (msg->m_flag !=4 || msg->m_clubId == 0)return;

	for (Lint i = 0; i < msg->m_playerCount; i++)
	{
		if (msg->m_playerId[i] == 0)continue;
		LLOG_ERROR("updateClubUserPointFromLogic userId[%d] club[%d] point[%d]", msg->m_playerId[i], msg->m_clubId, msg->m_score[i]);
		gClubUserManager.updatePoint(msg->m_playerId[i], msg->m_clubId, msg->m_score[i]);
	}
}

Lint   ClubManager::userUpdateClubFromCenter(LMsgCE2LMGClubInfo *msg)
{
	if ( msg == NULL)
	{
		LLOG_ERROR("userUpdateClubFromCenter  msg==NULL");
		return -1;
	}
	LLOG_ERROR("userUpdateClubFromCenter club[%d]  operId[%d] clubtype[%d] AdminCanChange[%d] AdminCanChange[%d] ", msg->m_clubId,msg->m_operId,  msg->m_clubType, msg->m_adminCanChange, msg->m_adminCanSeeAll);

	LMsgS2CUpdateClub updateClub;
	updateClub.m_clubId = msg->m_clubId;
	updateClub.m_msgType = 1;

	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	if (safeClub.get() != NULL && safeClub->isValid())
	{
		boost::shared_ptr<Club> club = safeClub->getResource();
	
		club->setClubType(msg->m_clubType);
		club->setAdminCanChange(msg->m_adminCanChange);
		club->setAdminCanSeeAll(msg->m_adminCanSeeAll);
		if(msg->m_tableCount>0)
			club->ChangeShowDesk(msg->m_tableCount);
	

		updateClub.m_clubType = club->getClubType();
		updateClub.m_adminCanChange = club->getAdminCanChange();
		updateClub.m_adminCanSeeAll = club->getAdminCanSeeAll();
		updateClub.m_tableCount = club->getCurrDeskCount();

	}
	else
	{
		updateClub.m_errorCode = 3;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeOperUser = gUserManager.getUserbyUserId(msg->m_operId);
	if (safeOperUser && safeOperUser->isValid())
	{
		safeOperUser->getResource()->Send(updateClub.GetSendBuff());
	}

	//更新界面
	broadcastClubScene(msg->m_clubId);

	return 0;
}

Lint   ClubManager::userUpdatePlayFromCenter(LMsgCE2LMGPlayInfo *msg)
{
	//if ( msg == NULL)
	//{
	//	LLOG_ERROR("userUpdatePlay  msg==NULL ");
	//	return -1;
	//}
	//LLOG_ERROR("userUpdatePlay  userId[%d] update play  club[%d] ",msg->m_operId,msg->m_clubId);
	//
	//LMsgS2CUpdatePlay updatePlay;
	//updatePlay.m_clubId = msg->m_clubId;
	////updatePlay.m_msgType = msg->m_msgType;
	//updatePlay.m_minPointInRoom = msg->m_minPointInRoom;
	//updatePlay.m_minPointXiaZhu = msg->m_minPointXiaZhu;
	//updatePlay.m_smallZero = msg->m_smallZero;
	//
	//bool  isSendTrue = false;
	//boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(msg->m_clubId);
	//if (safeClub.get() != NULL && safeClub->isValid())
	//{
	//	boost::shared_ptr<Club> club = safeClub->getResource();
	//	if (gClubPlayManager.updatePlayType(msg, club->getCurrPlayId()))
	//	{
	//		isSendTrue = true;
	//		pUser->Send(updatePlay.GetSendBuff());
	//		return 0;
	//	}
	//
	//}
	//if (!isSendTrue)
	//{
	//	updatePlay.m_errorCode = 3;
	//	pUser->Send(updatePlay.GetSendBuff());
	//}
	//if (isSendTrue)
	//{
	//	broadcastClubScene(msg->m_clubId);
	//}

	return 0;
}

Lint  ClubManager::_getFreeClubDeskId(Lint clubId)
{
	LLOG_DEBUG("_getFreeClubDeskId clubId[%d]", clubId);
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
	if (safeClub.get() != NULL && safeClub->isValid())
	{
		std::list<Lint>  clubDeskList = safeClub->getResource()->getClubDeskList();
		for (auto ItClubDesk = clubDeskList.begin(); ItClubDesk != clubDeskList.end(); ItClubDesk++)
		{
			boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(*ItClubDesk);
			if (safeClubDeskInfo.get() != NULL && safeClubDeskInfo->isValid())
			{
				boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
				LLOG_DEBUG("_getFreeClubDeskId clubId[%d] show[%d] clubdesk[%d] lock[%d]  roomfull[%d]", clubId, info->getShowDeskId(),info->getClubDeskId(),info->getLock(),info->m_roomFull);
				//锁上的桌子不允许加入-Asui
				if (safeClubDeskInfo->getResource()->isLock())
					continue;
				if (0 == safeClubDeskInfo->getResource()->m_roomFull)
					return safeClubDeskInfo->getResource()->getClubDeskId();
			}
		}

	}

	return 0;
}

void    ClubManager::userQuickJoinClubDesk(LMsgC2SQuickAddDesk* msg, User* pUser)
{
	if (msg == NULL || pUser == NULL)
	{
		return;
	}
	//LLOG_DEBUG("userQuickJoinClubDesk  user[%d]  club[%d]",pUser->GetUserDataId(),msg->m_clubId);
	Lint clubDeskId = _getFreeClubDeskId(msg->m_clubId);
	if (clubDeskId == 0)
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = 0;
		ret.m_errorCode = 14;//LMsgS2CAddDeskRet::Err_DESK_LOCK;
		pUser->Send(ret);

		LLOG_ERROR("userQuickJoinClubDesk ERROR  user[%d]  club[%d] clubDeskId==0", pUser->GetUserDataId(), msg->m_clubId);
		return;
	}
	LLOG_DEBUG("userQuickJoinClubDesk  user[%d]  club[%d] clubdeskId[%d]", pUser->GetUserDataId(), msg->m_clubId, clubDeskId);
	LMsgC2SAddDesk  pAddDesk;
	pAddDesk.m_deskId = clubDeskId;

	pUser->HanderUserAddDesk(&pAddDesk);

	return;
}

void     ClubManager::userPointList(LMsgC2SPowerPointList* msg, User *pUser)
{
	if (msg == NULL || pUser == NULL)
	{
		return;
	}
	

	LMsgS2CPowerPointList pointList;
	pointList.m_clubId = msg->m_clubId;

	//判断是否是俱乐部成员
	if (!gClubUserManager.isUserInClub(msg->m_clubId, pUser->GetUserDataId()))
	{
		pointList.m_errorCode = 3;
		pUser->Send(pointList.GetSendBuff());
		LLOG_ERROR("LMsgC2SPowerPointList  ERROR NOT MEMBER userId[%d] get club[%d]  point List", pUser->GetUserDataId(), msg->m_clubId);
		return;
	}

	//权限
	Lint canPointList = 0;
	boost::shared_ptr<CSafeResourceLock<Club> > safeClub = gClubManager.getClubbyClubId(msg->m_clubId);
	if (safeClub.get() && safeClub->isValid())
	{
		canPointList = safeClub->getResource()->canPowPointList(pUser->GetUserDataId());

		pointList.m_isChange = safeClub->getResource()->canChangePoint(pUser->GetUserDataId())?1:0;
	}
	if (0==canPointList)
	{
		pointList.m_errorCode = 4;
		pUser->Send(pointList.GetSendBuff());
		LLOG_ERROR("LMsgC2SPowerPointList  ERROR CAN NOT SEE  LIST  userId[%d] get club[%d]  point List", pUser->GetUserDataId(), msg->m_clubId);
		return;
	}
	
	//查询
	std::vector<boost::shared_ptr<ClubUser> > pageUser = _sortClubUserList(msg->m_clubId,msg->m_search,msg->m_oldUser,pUser->GetUserDataId());

	LLOG_ERROR("userPointList  userId[%d] get club[%d] search[%s]  oldlist[%d] point List[%d]", pUser->GetUserDataId(), msg->m_clubId, msg->m_search.c_str(), msg->m_oldUser.size(),pageUser.size());
	for (auto ItClubUser = pageUser.begin(); ItClubUser != pageUser.end(); ItClubUser++)
	{
		if (ItClubUser->get() == NULL)continue;
		UserPowerInfo temp;
		temp.m_userId = ItClubUser->get()->getID();
		temp.m_nike = ItClubUser->get()->m_nike;
		temp.m_photo = ItClubUser->get()->m_headImageUrl;
		temp.m_state = ItClubUser->get()->getClientState();
		if( 1== canPointList )
			temp.m_point = ItClubUser->get()->getClubPoint(msg->m_clubId);
		else if(1 != canPointList && temp.m_userId== pUser->GetUserDataId())
			temp.m_point = ItClubUser->get()->getClubPoint(msg->m_clubId);
		temp.m_type = ItClubUser->get()->getUserType(msg->m_clubId);
		//打印
		temp.print();
		pointList.m_userList.push_back(temp);
	}

	if (2 == canPointList)
		pointList.m_type = 1;
	pointList.m_count = pointList.m_userList.size();
	if(pointList.m_userList.size()<CLUB_USER_PAGE_COUNT)
		pointList.m_HasSendAll = 1;

	pUser->Send(pointList.GetSendBuff());

}

//成员列表排序  按照自己，会长，管理员，在线，游戏中，离线顺序排序
std::vector<boost::shared_ptr<ClubUser> >   ClubManager::_sortClubUserList(Lint clubId, Lstring nike, std::vector<Lint> & delUser,Lint myId)
{
	//std::vector<boost::shared_ptr<ClubUser>  > clubUserList;
	std::vector<boost::shared_ptr<ClubUser> > pageUser;
	//根据昵称查询
	if (!nike.empty())
	{
		//昵称
		std::set<boost::shared_ptr<ClubUser>  >   userNikeList;
		userNikeList = gClubUserManager.getClubUserbyUserNike(clubId, nike);
		for (auto ItUserNike = userNikeList.begin(); ItUserNike != userNikeList.end(); ItUserNike++)
			pageUser.push_back(*ItUserNike);
		//id
		Lint searchId = atoi(nike.c_str());
		if (searchId != 0 && gClubUserManager.isUserInClub(clubId, searchId))
		{
			boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(searchId);
			if (safeClubUserInfo && safeClubUserInfo->isValid())
			{
				pageUser.push_back(safeClubUserInfo->getResource());
			}
		}
	}
	else          //全查  或者 根据userId查询
	{
		std::set<Lint> oldUserSet;
		for (auto ItOld = delUser.begin(); ItOld != delUser.end(); ItOld++)
		{
			oldUserSet.insert(*ItOld);
		}

		std::set<boost::shared_ptr<ClubUser>  > clubUserInGame = gClubUserManager.getClubUserInGame(clubId);
		std::set<boost::shared_ptr<ClubUser>  > clubUserOnlineNoGame = gClubUserManager.getClubAllOnLineUserNoGame(clubId);
		std::set<boost::shared_ptr<ClubUser>  > clubUserOfflineNoGame = gClubUserManager.getClubAllOffLineUserNoGame(clubId);

		
		std::set<Lint>  pageUserId;

		//获取会长和管理员
		Lint clubOwnerId = 0;
		std::set<Lint> admin;
		boost::shared_ptr<CSafeResourceLock<Club> > safeClub = getClubbyClubId(clubId);
		if (safeClub.get() && safeClub->isValid())
		{
			boost::shared_ptr<Club> club = safeClub->getResource();
			club->getClubOwnerAndAdmin(&clubOwnerId, admin);
		}

		std::vector<Lint>  prePageId;
		//自己
		if (myId != clubOwnerId &&oldUserSet.find(myId) == oldUserSet.end())
		{
			prePageId.push_back(myId);
		}
		//会长
		if (clubOwnerId != 0 && oldUserSet.find(clubOwnerId) == oldUserSet.end())
		{
			prePageId.push_back(clubOwnerId);
		}

		//管理员
		for (auto ItAdmin = admin.begin(); ItAdmin != admin.end(); ItAdmin++)
		{
			if (*ItAdmin != 0 && *ItAdmin != myId && oldUserSet.find(*ItAdmin) == oldUserSet.end())
			{
				prePageId.push_back(*ItAdmin);
			}
		}

		bool isEnd = false;
		for (auto ItPrePageId = prePageId.begin(); ItPrePageId != prePageId.end(); ItPrePageId++)
		{
			boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(*ItPrePageId);
			if (safeUser && safeUser->isValid())
			{
				pageUser.push_back(safeUser->getResource());
				pageUserId.insert(safeUser->getResource()->getID());
				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
				{
					isEnd = true;
					break;
				}
			}
		}

		//在线
		if (!isEnd)
		{
			for (auto ItOnLineUser = clubUserOnlineNoGame.begin(); ItOnLineUser != clubUserOnlineNoGame.end(); ItOnLineUser++)
			{
				if (ItOnLineUser->get() && oldUserSet.find(ItOnLineUser->get()->getID()) != oldUserSet.end())
				{
					continue;
				}
				if (pageUserId.find((*ItOnLineUser)->getID()) != pageUserId.end())continue;
				pageUser.push_back(*ItOnLineUser);
				pageUserId.insert((*ItOnLineUser)->getID());
				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
				{
					isEnd = true;
					break;
				}

			}
		}

		//游戏中
		if (!isEnd)
		{
			for (auto ItInGameUser = clubUserInGame.begin(); ItInGameUser != clubUserInGame.end(); ItInGameUser++)
			{
				if (ItInGameUser->get() && oldUserSet.find(ItInGameUser->get()->getID()) != oldUserSet.end())
				{
					continue;
				}
				if (pageUserId.find((*ItInGameUser)->getID()) != pageUserId.end())continue;
				pageUser.push_back(*ItInGameUser);
				pageUserId.insert((*ItInGameUser)->getID());
				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
				{
					isEnd = true;
					break;
				}

			}
		}

		//离线
		if (!isEnd)
		{
			for (auto ItOffLineUser = clubUserOfflineNoGame.begin(); ItOffLineUser != clubUserOfflineNoGame.end(); ItOffLineUser++)
			{
				if (ItOffLineUser->get() && oldUserSet.find(ItOffLineUser->get()->getID()) != oldUserSet.end())
				{
					continue;
				}
				if (pageUserId.find((*ItOffLineUser)->getID()) != pageUserId.end())continue;
				pageUser.push_back(*ItOffLineUser);
				pageUserId.insert((*ItOffLineUser)->getID());

				if (pageUser.size() == CLUB_USER_PAGE_COUNT)
				{
					break;
				}
			}

		}

	}

	return pageUser;
}

//对俱乐部桌子排序
std::vector<Lint>  ClubManager::_orderClubDesk(const std::set<Lint> clubDeskSet)
{
	Lint deskCount = clubDeskSet.size();
	std::vector<Lint>  orderedDeskV;
	orderedDeskV.reserve(deskCount);

	for (auto ItDesk = clubDeskSet.begin(); ItDesk != clubDeskSet.end(); ItDesk++)
	{
		boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(*ItDesk);
		if (safeClubDeskInfo && safeClubDeskInfo->isValid())
		{
			boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
			if (info->getShowDeskId() <= deskCount)
				orderedDeskV[info->getShowDeskId()-1] = *ItDesk;
		}
	}
	return orderedDeskV;
}

void ClubManager::userLockClubDesk(LMsgC2SLockClubDesk* msg, User* pUser)
{
	if (msg == NULL || pUser == NULL)
	{
		LLOG_ERROR("userLockClubDesk  msg==NULL or pUser == NULL");
		return;
	}
	LLOG_DEBUG("userLockClubDesk user[%d] lock desk[%d] club[%d]  type[%d]",pUser->GetUserDataId(),msg->m_clubDeskId,msg->m_clubId,msg->m_type);
	if (msg->m_clubDeskId == 0)
	{
		LLOG_ERROR("userLockClubDesk  clubDesk[%d]",msg->m_clubDeskId);
		return;
	}
	LMsgS2CLockClubDesk  send;
	send.m_clubId = msg->m_clubId;
	send.m_clubDeskId = msg->m_clubDeskId;
	send.m_type = msg->m_type;

	if (msg->m_clubDeskId != 0)
	{
		boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > safeClubDeskInfo = gClubDeskManager.getClubDeskInfoByClubDeskId(msg->m_clubDeskId);
		if (safeClubDeskInfo && safeClubDeskInfo->isValid())
		{
			boost::shared_ptr<ClubDeskInfo> info = safeClubDeskInfo->getResource();
			if (msg->m_type == 0) info->delLock();
			else if (msg->m_type == 1) info->setLock();
		}
	}
	pUser->Send(send);

	LMsgS2CClubScene  scene;
	getClubScene(pUser->GetUserDataId(), msg->m_clubId, scene);
	pUser->Send(scene.GetSendBuff());


	broadcastClubMsg(msg->m_clubId, send,pUser->GetUserDataId());

	broadcastClubScene(msg->m_clubId, pUser->GetUserDataId());
	return;
}

//修改玩家申请加入俱乐部未被处理的玩家数量,setFlag  0：增加  1：减少
bool ClubManager::setUserApplyClubUndoCount(Lint clubId, bool setFlag)
{
	if (clubId == 0)
	{
		return false;
	}

	boost::mutex::scoped_lock l(m_mutexUserApplyClubUndo);

	auto t_clubIter = m_mapUserApplyClubUndo.find(clubId);

	//玩家申请加入俱乐部
	if (setFlag == false)
	{
		//该俱乐部已经存在未被处理的玩家申请
		if (t_clubIter != m_mapUserApplyClubUndo.end())
		{
			++(t_clubIter->second);
		}
		//该俱乐部没有未被处理的玩家申请
		else
		{
			m_mapUserApplyClubUndo[clubId] = 1;
		}
	}
	//会长或管理员处理玩家的申请
	else
	{
		//该俱乐部已经存在未被处理的玩家申请
		if (t_clubIter != m_mapUserApplyClubUndo.end())
		{
			t_clubIter->second = t_clubIter->second > 1 ? t_clubIter->second - 1 : 0;
		}
	}
	
	return true;
}

//根据俱乐部ID获取申请该俱乐部且未被处理的玩家数量
Lint ClubManager::getUserApplyClubUndoCount(Lint clubId)
{
	if (clubId == 0)
	{
		return 0;
	}

	boost::mutex::scoped_lock l(m_mutexUserApplyClubUndo);

	auto t_clubIter = m_mapUserApplyClubUndo.find(clubId);

	if (t_clubIter == m_mapUserApplyClubUndo.end())
	{
		return 0;
	}

	return t_clubIter->second;
}