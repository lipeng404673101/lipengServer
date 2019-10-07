#include "LLog.h"
#include "NewUserVerify.h"
#include "Config.h"
#include "DUser.h"
#include "HttpRequest.h"
#include "Config.h"
#include "UserMessageMsg.h"
#include "mjConfig.h"
#include "clubManager.h"

//////////////////////////////////////////////////////////////////////////

NewUserVerify::NewUserVerify()
	: m_dbsession(NULL),m_needWechatVerify(1)
{
	LTime cur;
	m_600SceTick = cur.MSecs();
	m_60SceTick = cur.MSecs();
	m_bUseMasterSource = true;
}

bool NewUserVerify::Init()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
	}
	m_dbsession = new LDBSession;
	if (m_bUseMasterSource)
	{
		LLOG_DEBUG("logwyz NewUserVerify  master mysql,host[%s],port[%d],user[%s],pw[%s],dbnanme[%s]", gConfig.GetDbHost().c_str(), gConfig.GetDbPort(), gConfig.GetDbUser().c_str(), gConfig.GetDbPass().c_str(), gConfig.GetDbName().c_str());
		return m_dbsession->Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort());
	}
	LLOG_DEBUG("logwyz  NewUserVerify bak mysql,host[%s],port[%d],user[%s],pw[%s],dbnanme[%s]", gConfig.GetBKDbHost().c_str(), gConfig.GetBKDbPort(), gConfig.GetBKDbUser().c_str(), gConfig.GetBKDbPass().c_str(), gConfig.GetDbName().c_str());
	return m_dbsession->Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort());
}

bool NewUserVerify::Final()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
		m_dbsession = NULL;
	}
	return true;
}

void NewUserVerify::Clear()
{

}

void NewUserVerify::Run()
{
	if(!this->Init())
	{
		LLOG_ERROR("DbServer init Failed!!!");
		return;
	}
	
	while(!GetStop())
	{
		LTime cur;

		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			//60 second
			if (cur.MSecs() - m_60SceTick > 60*1000)
			{
				m_60SceTick = cur.MSecs();
				ClearOpenids( cur );
			}

			//5分钟循环一次
			if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
			{
				ReadConfig();
				m_600SceTick = cur.MSecs();
			}
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			HanderMsg(msg);
			delete msg;
		}
	}
	
	this->Final();
}

void NewUserVerify::HanderMsg(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_NEWUSER_VERIFY_REQUEST:
		HanderNewUserVerifyRequest((LMsgNewUserVerifyRequest*)msg);
		break;
	case MH_MSG_L_2_LMG_NEW_AGENCY_ACTIVITY_UPDATE_PLAY_COUNT:
		MHHanderNewAgencyActivity((MHLMsgL2LMGNewAgencyActivityUpdatePlayCount*)msg);
		break;
	default:
		LLOG_ERROR("***unkown message id = %d", msg->m_msgId);
		break;
	}
}

void NewUserVerify::ClearOpenids( const LTime& cur )
{
	std::map<std::string,LTime>::iterator it = m_openids.begin();
	std::map<std::string,LTime>::iterator end = m_openids.end();
	while ( it != end )
	{
		if ( cur.MSecs() - it->second.MSecs() >= 15*60*1000 ) //15分钟
		{
			m_openids.erase( it++ );
		}
		else
		{
			++it;
		}
	}
}

void NewUserVerify::HanderNewUserVerifyRequest(LMsgNewUserVerifyRequest* msg)
{
	DUser* user = (DUser*)msg->m_pUser;
	if ( user == NULL )
	{
		return;
	}

	LMsgNewUserVerifyReply* reply = new LMsgNewUserVerifyReply;
	reply->m_login_flag = msg->m_login_flag;
	reply->m_pUser = msg->m_pUser;
	reply->m_gameloginSP = msg->m_gameloginSP;
	reply->m_severID = msg->m_severID;
	reply->m_type = msg->m_type;
	do
	{
		std::map<std::string,LTime>::iterator it = m_openids.find( msg->m_unionId);
		if ( it != m_openids.end() )
		{
			// 存在加入的这个玩家直接退出 一般是客户端快速大量的发送这个消息导致
			reply->m_errorCode = 4;
			reply->m_errorMsg = "login too frequently";
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest login too frequently");
			break;
		}

		LTime cur;
		m_openids[msg->m_unionId] = cur;
		m_needWechatVerify = true;
		Lint errorCode = 0;
		Lint id = 0;
		bool bExist = false;
	
		// 数据库创建玩家
		reply->m_errorCode = 4;
		reply->m_errorMsg = "fail";
		if ( !m_dbsession )
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest DBSession null");
			break;
		}

		MYSQL* m = m_dbsession->GetMysql();
		if ( !m )
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest mysql null");
			break;
		}


		std::stringstream ss;
		LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest  else");
		ss << "INSERT INTO user (OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,TotalCircleNum,TotalWinNum,phoneUUID,userIdTempIn,userIdTempOut,loginDevice,ChangeStatus) VALUES (";
		
		ss << "'" << user->m_usert.m_openid << "',";
		ss << "'" << user->m_usert.m_nike << "',";
		ss << "'" << user->m_usert.m_sex << "',";
		ss << "'" << user->m_usert.m_provice << "',";
		ss << "'" << user->m_usert.m_city << "',";
		ss << "'" << user->m_usert.m_country << "',";
		ss << "'" << user->m_usert.m_headImageUrl << "',";
		ss << "'" << user->m_usert.m_unioid << "',";
		ss << "'" << user->m_usert.m_numOfCard1s << "',";
		ss << "'" << user->m_usert.m_numOfCard2s << "',";
		ss << "'" << user->m_usert.m_numOfCard3s << "',";
		ss << "'" << user->m_usert.m_lastLoginTime<< "',";
		ss << "'" << user->m_usert.m_regTime << "',";
		ss << "'" << user->m_usert.m_new << "',";
		ss << "'" << user->m_usert.m_gm << "',";
		ss << "'" << user->m_usert.m_totalbuynum << "',";
		ss << "'" << user->m_usert.m_totalplaynum << "',";
		ss << "'" << user->m_usert.m_customInt[0] << "',";
		ss << "'" << "0"<< "',";
		ss << "'" << user->m_usert.m_customString4 << "',";
		ss << "'" << user->m_usert.m_customString5 << "',";
		ss << "'" << user->m_usert.m_customString6 << "',";
		ss << "'" << user->m_usert.m_customString7 << "',";
		ss << "'" << msg->m_type << "'";
		ss << ")";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()) != 0)
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest sql error %s:%s", ss.str().c_str(),mysql_error(m));
			break;
		}
		user->m_usert.m_id = (Lint)mysql_insert_id(m);
		reply->m_errorCode = 0;
		reply->m_errorMsg = "";

		Lstring strNewUser = "newUserXianDan";
		Lint newUserXianDan = 0;
		gMjConfig.GetCommonConfigValue(strNewUser, newUserXianDan);

		// 插入user_info记录
		std::stringstream ss2;
		ss2 << "INSERT INTO  user_info  (user_id,level,level_remind,score,play_count,winner_count,ctime,utime) VALUES (";
		ss2 << user->m_usert.m_id << ",";
		ss2 << " 0, 0, ";
		ss2 << newUserXianDan;
		ss2 << ",0, 0,NOW(),NOW())";
		LLOG_DEBUG("user_info SQL[%s]", ss2.str().c_str());
		if (mysql_real_query(m, ss2.str().c_str(), ss2.str().size()) != 0)
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest sql error %s:%s", ss2.str().c_str(), mysql_error(m));
			break;
		}

		// 插入user_wallet记录
		std::stringstream ss3;
		ss3 << "INSERT INTO  user_wallet  (user_id,total_amount, balance,ctime,utime) VALUES (";
		ss3 << user->m_usert.m_id << ",";
		ss3 << " 0, 0,";
		ss3 << "NOW(),NOW())";
		LLOG_DEBUG("user_wallet SQL[%s]", ss3.str().c_str());
		if (mysql_real_query(m, ss3.str().c_str(), ss3.str().size()) != 0)
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest sql error %s:%s", ss3.str().c_str(), mysql_error(m));
			break;
		}
		// 插入user_ticket记录
		std::stringstream ss4;
		ss4 << "INSERT INTO  user_ticket  (user_id,gold, silver,ctime,utime) VALUES (";
		ss4 << user->m_usert.m_id << ",";
		ss4 << " 0, 0,";
		ss4 << "NOW(),NOW())";
		LLOG_DEBUG("user_ticket SQL[%s]", ss4.str().c_str());
		if (mysql_real_query(m, ss4.str().c_str(), ss4.str().size()) != 0)
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest sql error %s:%s", ss4.str().c_str(), mysql_error(m));
			break;
		}

	}
	while ( false );

	if ( reply->m_errorCode != 0 )
	{
		// 如果没有处理正确 先去掉这个记录
		m_openids.erase( msg->m_unionId);
	}
	gUserMessageMsg.handlerMessage(user->m_usert.m_unioid, reply);
}

void NewUserVerify::ReadConfig()
{
	LIniConfig	m_ini;
	m_ini.LoadFile("center_config.ini");
	m_needWechatVerify = m_ini.GetShort("NeedWechat",1);
}

Lint NewUserVerify::GetMaxUserIdFromDB() {
	MYSQL* m = m_dbsession->GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("UserManager::GetMaxUserIdFromDB mysql null");
		return 0;
	}
	////////////////////////////////////////////////////////
	Lint nInsertID = 0;
	static Lstring  SQLSelect = "select max(Id) from user limit 1";
	if (mysql_real_query(m, SQLSelect.c_str(), SQLSelect.size()))
	{
		LLOG_ERROR("GetMaxUserIdFromDB  error: %s  Sql: %s", mysql_error(m), SQLSelect.c_str());
		return 0;
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("GetMaxUserIdFromDB  error: %s ", mysql_error(m));
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("GetMaxUserIdFromDB  idbase Empty!");
		return 0;
	}
	nInsertID = atoi(*row);
	mysql_free_result(res);
	LLOG_ERROR("GetMaxUserIdFromDB return %d", nInsertID);
	return nInsertID;
}

Lint NewUserVerify::GetUniqueIdByTimestamp() {
	static int last_id;
	time_t t;
	time(&t);
	//int seconds = (int)(t - 1483200000); // 相对于 20170101 的秒数，60多年后需要升级 user 表 Id 字段的数据类型
	int seconds = (int)(t - 978278400); // 相对于 20010101 的秒数，到2069年需要升级 user 表 Id 字段的数据类型
	// 每秒最多一个新用户，如果某一秒新用户多于一个，就用下一秒的值
	// 需要在服务器重启时，从数据库 user 表中读取最大的 Id 字段
	if (last_id == 0) {
		last_id = GetMaxUserIdFromDB() + 1;
	}
	if (seconds < last_id) seconds = last_id + 1;
	last_id = seconds;
	return seconds;
}

Lint NewUserVerify::GetRandInsertIDFromDB()
{
#if defined(_WIN32)
	// 在 Windows 是测试时间戳做用户 id
	//return GetUniqueIdByTimestamp();
#endif
	MYSQL* m = m_dbsession->GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("UserManager::GetRandInsertIDFromDB mysql null");
		return 0;
	}
	////////////////////////////////////////////////////////
	Lint nInsertID = 0;
	static Lstring  SQLSelect = "select userid from idbase limit 1";
	if (mysql_real_query(m, SQLSelect.c_str(), SQLSelect.size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB1  error: %s  Sql: %s", mysql_error(m), SQLSelect.c_str());
		return 0;
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("GetRandInsertIDFromDB2  error: %s ", mysql_error(m));
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("GetRandInsertIDFromDB3  idbase Empty!");
		return 0;
	}
	nInsertID = atoi(*row);
	mysql_free_result(res);
	std::stringstream ss;
	ss << "delete from idbase where userid = " << nInsertID;
	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB4 sql error: %s  Sql: %s", mysql_error(m), ss.str().c_str());
		return 0;
	}
	return nInsertID;
}

void NewUserVerify::MHHanderNewAgencyActivity(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("MHHanderNewAgencyActivity error  msg  is null !!!");
		return;
	}
	if (msg->m_flag == AgencyActivityUpdatePlayCountType_EveryScore)
	{
		return MHHanderOneDrawActivity(msg);
	}
	if (msg->m_flag == AgencyActivityUpdatePlayCountType_ChangePoint)
	{
		LLOG_DEBUG("MHHanderNewAgencyActivity ChangePoint  club[%d] clubsq[%d] ",msg->m_clubId,msg->m_deskId);
		return gClubManager.updateClubUserPoint(msg);
	}
	if (msg->m_flag == AgencyActivityUpdatePlayCountType_BigWin)
	{
		LLOG_DEBUG("MHHanderNewAgencyActivity falg[%d] clubId[%d] deskId[%d] userId[%d] playType[%d] paixing[%d] userCount[%d]", msg->m_flag, msg->m_clubId, msg->m_deskId, msg->m_userId, msg->m_playType, msg->m_paiXing[0], msg->m_userCount);
		MHBigWinAndSpecPaiXing SpecPaiXing;
		SpecPaiXing.Invoke(msg->m_clubId, msg->m_deskId, msg->m_userId, msg->m_userNike, msg->m_playType, msg->m_paiXing[0], msg->m_userCount);
		return;
	}


	//// 调用数据库的存储过程
	//LLOG_DEBUG("****New agency activity userId:%d, deskid:%d player:%d:%d:%d:%d", msg->m_userid, msg->m_deskId,
	//	msg->m_playerId[0], msg->m_playerId[1],
	//	msg->m_playerId[2], msg->m_playerId[3]);
	///*boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	//if (safeUser.get() == NULL || !safeUser->isValid())
	//{
	//return;
	//}*/
	//std::stringstream ss;
	//ss<<"call NewAgencyActivity('";
	//ss<<msg->m_playerId[0]<<"', '";
	//ss<<msg->m_playerId[1]<<"', '";
	//ss<<msg->m_playerId[2]<<"', '";
	//ss<<msg->m_playerId[3]<<"', ";
	//ss<<" @Ret, @strDesc, @AgencyId, @IsExpire, @UserPlayCount, @FinishTime, @ToSendAfterExpire, @HasRecharge)";


	//Lstring sql=Lstring(ss.str());
	//LLOG_DEBUG("****sql to run: %s", sql.c_str());
	//if (m_dbsession)
	//{
	//	MYSQL * mysql=m_dbsession->GetMysql();
	//	if (mysql)
	//	{
	//		int ret=mysql_real_query(mysql, sql.c_str(), sql.length());
	//		if (0==ret)
	//		{
	//			//IN UserID1 INT, IN UserID2 INT, IN UserID3 INT, IN UserID4 INT, 
	//			//OUT Ret INT, OUT strErrorDescribe varchar(127), OUT out_AgencyID INT, OUT out_IsExpire INT, OUT out_UserPlayCount INT, OUT out_FinishTime DATETIME, //OUT out_ToSendAfterExpire INT, OUT out_HasRecharge INT
	//			Lstring query_result("SELECT @Ret, @strDesc, @AgencyId, @IsExpire, @UserPlayCount, @FinishTime, @ToSendAfterExpire, @HasRecharge");
	//			ret=mysql_query(mysql, query_result.c_str());
	//			if (0==ret)
	//			{
	//				MYSQL_RES * res=mysql_store_result(mysql);
	//				if (res)
	//				{
	//					while (1)
	//					{
	//						MYSQL_ROW row=mysql_fetch_row(res);
	//						if (!row)
	//							break;
	//						int error_no=row[0]?atoi(row[0]):-1;
	//						Lstring str_desc("");
	//						if (res->field_count>=2&&row[1])
	//						{
	//							str_desc=row[1];
	//						}
	//						if (res->field_count>=8&&0==error_no)
	//						{
	//
	//							MHAgencyActivityStatHelper  helper;
	//							for (int i=0; i < 8; i++)
	//							{
	//								LLOG_DEBUG("*** i:%d v: %s", i, row[i]);
	//							}
	//							// 0:Ret, 1:strErrorDesc 2:AgencyId 3:IsExpire 4:UserPlayCount 5:FinishTime, 6:ToSendAfterExpire 7:HasRecharge
	//							helper.Invoke(msg->m_deskId, msg->m_playerId, row[2], row[3], row[4], row[5], row[6]);
	//						}
	//						else if (row[1])
	//						{
	//							LLOG_DEBUG("***NewAgencyActivity procedure return errno:%d strErrorDescribe: %s", error_no, str_desc.c_str());
	//						}
	//						break;
	//					}
	//					mysql_free_result(res);
	//				}
	//			}
	//			else
	//			{
	//				LLOG_ERROR("DbServer::MHExecuteNewAgencyActivity error:%d, sql:%s", ret, query_result.c_str());
	//				LLOG_ERROR("DbServer::MHExecuteNewAgencyActivity sql error:%d, sql:%s", mysql_errno(mysql), mysql_error(mysql));
	//			}
	//		}
	//		else
	//		{
	//			LLOG_ERROR("DbServer::MHExecuteNewAgencyActivity error:%d, sql:%s", ret, sql.c_str());
	//			LLOG_ERROR("DbServer::MHExecuteNewAgencyActivity sql error:%d, sql:%s", mysql_errno(mysql), mysql_error(mysql));
	//		}
	//	}
	//
	//}
	return;
}

void NewUserVerify::MHHanderOneDrawActivity(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg)
{

	if (msg == NULL)
	{
		LLOG_ERROR("MHHanderOneDrawActivity error msg is null!!!");
		return;
	}
	MHOneDrawActivityHelper  helper;

	helper.Invoke(msg->m_deskId, msg->m_playType, msg->m_playerId, msg->m_score, msg->m_playerCount, msg->m_maxCircle, msg->m_startTime);



	//LLOG_ERROR("target_url=[%s]", helper.target_url.c_str());

	return;
}

void NewUserVerify::SetDatabaseSource(bool  bUserMaster)
{
	m_bUseMasterSource = bUserMaster;
}