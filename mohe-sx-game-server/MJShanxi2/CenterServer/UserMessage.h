#ifndef _USER_MESSAGE_H_
#define _USER_MESSAGE_H_

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "DUser.h"
#include "LDBSession.h"
#include "LTime.h"
#include "LRedisClient.h"

class CUserMessage : public LRunnable
{
public:
	CUserMessage();
	virtual ~CUserMessage();
public:
	
	virtual void Clear();
	virtual void Run();

	virtual void Start();
	virtual	void Stop();
	virtual	void Join();
protected:
	void	HanderMsg(LMsg* msg);
protected:
	void	HanderModifyUserNew(LMsgL2CeModifyUserNew* msg);
	void	HanderUpdateUserCoin(LMsgL2CeUpdateCoin* msg);
	void	HanderUpdateUserGps(LMsgLMG2CEUpdateUserGPS* msg);
	void	HanderUpdateUserCoinJoinDesk(LMsgL2CeUpdateCoinJoinDesk* msg);
	void	HanderLogicUserLogin(LMsgL2CeUserServerLogin* msg);
	void	HanderLogicUserLogout(LMsgL2CeUserServerLogout* msg);
	void	HanderModifyCard(LMsgL2LMGModifyCard* msg);
	void	HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg);
	void	HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg);

	void    HanderModifyUserCreditValue(LMsgC2C_ADD_CRE* msg);
	void    HanderExchangeCard(LMsgL2LMGExchCard* msg);
	void    HanderSqlInfo(LMsgLMG2CeSqlInfo * msg);
	void    HanderUpdatePointRecord(LMsgLMG2CEUpdatePointRecord*msg);
	//创建俱乐部请求
	void    HanderAddClub(LMsgCe2LAddClub*msg);
	void    HanderClubAddPlayType(LMsgCe2LMGClubAddPlayType* msg);


protected:
	void	HanderFromLoginServer(LMsgFromLoginServer* msg);
	void	HanderUserLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag);
	void	HanderUserPhoneLogin(LMsgC2SPhoneLogin* msg, LSocketPtr gameloginSP, Lint login_flag);
	void	HanderUserLocalLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag);
	void	HanderUserWechatLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag);
	void	HanderNewUserVerifyReply(LMsgNewUserVerifyReply* msg);
	void	SendUserLogin( const Lstring& uuid, LSocketPtr gameloginSP, Lint severID, Lint errorCode, const Lstring& errorMsg, Lint login_flag);
	void	SendUserPhoneLogin(const Lint userId, Lstring phone, LSocketPtr gameloginSP, Lint severID, Lint errorCode, const Lstring& errorMsg, Lint login_flag);

	//第一次升级，客户端上传phoneUUID 和 userId ，服务器保存，写入数据库
	void    HanderUserWechatLoginFirst(LMsgC2SMsg* msg, LSocketPtr gameloginSP, Lint login_flag);

	//修改房卡
	void	modifyCardCount(LMsgL2LMGModifyCard* Lmg2GMsg, bool bNeedSave = true);
	Lint    getChangeStatusByAppV(Lint verson, bool newUser);

private:
	void    UpdateUserInfo(const LUser& usert);
	void    UpdateUserPhoneUUID(const LUser& usert);
private:
	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;

private:
	CRedisClient	m_RedisClient;
};

#endif