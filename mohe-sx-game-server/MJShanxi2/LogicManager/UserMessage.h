#ifndef _USER_MESSAGE_H_
#define _USER_MESSAGE_H_

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "LTime.h"

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
	//From CenterServer
	void	HanderCenterUserLogin(LMsgCe2LUserLogin* msg);
	void	HanderCenterGMCharge(LMsgCe2LGMCharge* msg);
	void	HanderCenterGMCoins(LMsgCe2LGMCoins* msg);
	void	HanderCenterUpdateCoins(LMsgCe2LUpdateCoin* msg);
	void	HanderCenterUpdateCoinsJoinDesk(LMsgCe2LUpdateCoinJoinDesk* msg);
	//From LogicServer
	void	HanderModifyUserState(LMsgL2LMGModifyUserState* msg);
	void	HanderModifyUserCardNum(LMsgL2LMGModifyCard* msg);
	void	HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg);
	void	HanderModifyUserCreatedDesk(LMsgL2LMGModifyUserCreatedDesk* msg);
	void	HanderDeleteUserCreatedDesk(LMsgL2LMGDeleteUserCreatedDesk* msg);
	void    HanderModifyUserGps(LMsgL2LMGUpdateUserGPS* msg);
	void    MHHanderNewAgencyActivityUpdatePlayCount(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);

	void    HanderUserAddClubDesk(LMsgL2LMGUserAddClubDesk*msg);  //更新俱乐部桌子
	void    HanderUserLeaveClubDesk(LMsgL2LMGUserLeaveClubDesk* msg); //更新俱乐部桌子
	void   HanderUserFreshClubInfo(LMsgL2LMGFreshDeskInfo* msg);

	//From GateSever
	void	HanderUserMsg(LMsgG2LUserMsg* msg);
	void	HanderUserOutMsg(LMsgG2LUserOutMsg* msg);

	//From DB
	void	HanderDBReqVipLog(LMsgLBD2LReqVipLog* msg);
	void    HanderDBReqPokerLog(LMsgLBD2LReqPokerLog* msg);
	void	HanderDBReqRoomLog(LMsgLBD2LReqRoomLog* msg);
	void	HanderDBReqVideo(LMsgLDB2LReqVideo* msg);
	void    HanderDBRetBindingRelation(LMsgLDB2LMGBindingRelationships* msg);
	void    HanderDBRetInfo(LMsgLDB_2_LM_RetInfo* msg);
	void	HanderDBUserMsg(LMsgLDB2LMGUserMsg* msg);

	void	HanderDBRetShareVideo(LMsgLDB2LRetShareVideo* msg);
	void	HanderDBGetShareVideoID(LMsgLDB2LRetShareVideoId* msg);

	void    HanderDBReqCRELogHis(LMsgLDB2LM_RecordCRELog *msg);
	
	void    HanderDBSharedVideoResult(MHLMsgLDB2LShareVideoResult * msg);

	//处理用户不在的情况
	//修改房卡
	void	modifyCardCountSendToCenter(LMsgL2LMGModifyCard* L2LmgMsg, bool bNeedSave = true);
	//扑克回放
	void	HanderDBQiPaiReqRoomLog(LMsgLBD2LReqRoomLog* msg);
	void   HanderDBQiPaiReqVideo(LMsgLDB2LPokerReqVideo* msg);

	//From CoinsServer
	void	HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg);
protected:
	//处理玩家登陆的消息 
	void	HanderUserLogin(Lint id, Lint seed, Lint gateId, Lstring& md5, Lstring& ip);
protected:
	LTime&	GetCurTime();
private:
	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;
};

#endif