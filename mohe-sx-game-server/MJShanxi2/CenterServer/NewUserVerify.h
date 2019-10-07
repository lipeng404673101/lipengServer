#ifndef _NEWUSERVERIFY_H_
#define _NEWUSERVERIFY_H_

#include "LDBSession.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LTime.h"

//db服务器
class NewUserVerify : public LRunnable
{
public:
	NewUserVerify();
	void SetDatabaseSource(bool  bUserMaster);

protected:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Run();
	virtual void		Clear();

	void				HanderMsg(LMsg* msg);

	void				ClearOpenids( const LTime& cur );	// 清除过期的openid

	void                ReadConfig();

protected:
	void	HanderNewUserVerifyRequest(LMsgNewUserVerifyRequest* msg);
	void    MHHanderNewAgencyActivity(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);
	void    MHHanderOneDrawActivity(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);
	Lint	GetMaxUserIdFromDB();
	Lint GetUniqueIdByTimestamp();
	Lint	GetRandInsertIDFromDB();

private:
	bool m_bUseMasterSource;
	LDBSession* m_dbsession;

	LTime		m_tickTimer;
	Llong		m_60SceTick;//60秒循环一次

	Llong		m_600SceTick;//5分钟秒循环一次

	Lshort        m_needWechatVerify;

	// 防止相同的用户 在短时间内重复登录
	std::map<std::string,LTime> m_openids;
};

#endif