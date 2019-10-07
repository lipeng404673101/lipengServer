#ifndef _ROOM_VIP_H
#define _ROOM_VIP_H

#include "LMsg.h"
#include "Desk.h"
#include "LVipLog.h"
struct MHUserScore
{
	int m_pos;
	int m_userId;
	int m_score;
	MHUserScore() :m_pos(-1), m_userId(-1), m_score(-1) {}
	MHUserScore(int pos, int userId, int score) : m_pos(pos), m_userId(userId), m_score(score) {}

};

inline bool operator < (const MHUserScore & l, const MHUserScore &r)
{
	return l.m_score < r.m_score;
}
inline bool operator == (const MHUserScore & l, const MHUserScore &r)
{
	return l.m_score == r.m_score;
}
inline bool operator > (const MHUserScore & l, const MHUserScore &r)
{
	return l.m_score > r.m_score;
}


struct MHExtendVipLogItem
{
	Lstring m_tableId;
	std::map<Lint, Lstring> m_mapUserIdToDrawId;

	Lint m_JinAnGangCount[4];         // 每局金暗杠数量
	virtual void CreateTableId() = 0; // 创建表Id
	virtual void SaveDeskInfoToDB() = 0;      // 存储到数据库 
	inline void ResetJinAnGangCount()
	{
		memset(m_JinAnGangCount, 0, sizeof(m_JinAnGangCount));
	}
	 
};

struct VipLogItem :public LVipLogItem, MHExtendVipLogItem
{
public:
	Desk*			m_desk;	
public:
	VipLogItem();
	~VipLogItem();
	Lint		GetOwerId();

	void		AddLog(User** user, Lint* gold, HuInfo* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting);

	void		AddLog(User** user, Lint* gold, std::vector<HuInfo>* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting);

	bool		ExiestUser(Lint id);

	void		InsertToDb();

	void		UpdateToDb();

	bool		AddUser(User* user);

	bool        ResetUser( User  *user[] );

	Lint		GetUserPos(User* user);

	Lint		GetUserScore(User* user);

	bool		isEnd();

	bool        isNormalEnd();

	bool		IsFull(User* user);

	void		SendEnd();

	Lstring		PlayTypeToString4RLog();

	void		SendInfo();

	bool		IsBegin();

	void		RemoveUser(Lint id);

	virtual void CreateTableId(); // 创建表Id
	virtual void SaveDeskInfoToDB();            // 存储桌子信息到数据库
	virtual void UpdateDeskTotalScoreToDB();    // 修改桌子分数数据到数据库
	virtual void UpdateDeskTotalFeeValueToDB(Lint nFeeValue); // 修改桌子总费用
	virtual void InsertUserScoreToDB(Lint pos, Lstring &drawId, Lint isWin, Lint score, std::vector<Lint> & srcPaiXing);
	virtual void UpdateUserFeeValueToDB(Lint userId, Lint feeValue);
	virtual void InsertDeskWinnerInfo();

};

class VipLogMgr :public LSingleton<VipLogMgr>
{
public:	
	virtual	bool	Init();
	virtual	bool	Final();

	void			SetVipId(Lint id);

	Lstring			GetVipId();

	void			SetVideoId(Lint id);

	Lstring			GetVideoId();

	VipLogItem*		GetNewLogItem(Lint maxCircle,Lint usrId);

	VipLogItem*		GetLogItem(Lstring& logId);

	VipLogItem*	FindUserPlayingItem(Lint id);//查询玩家正在进行中的桌子

	void		AddPlayingItem(VipLogItem* item);//添加未完成的桌子

	void		Tick();
private:
	std::map<Lstring,VipLogItem*> m_item;
	Lint	m_id;

	Lint	m_videoId;
};

#define gVipLogMgr VipLogMgr::Instance()


//vip房间
class RoomVip:public LSingleton<RoomVip>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	void			Tick(LTime& cur);

	Desk*			GetFreeDesk(Lint nDeskID, GameType gameType);

	Desk*			GetDeskById(Lint id);

	Lint			CreateVipDesk(LMsgLMG2LCreateDesk* pMsg,User* pUser);

	Lint			CreateVipDeskForOther(LMsgLMG2LCreateDeskForOther* pMsg, User* pUser);

	//俱乐部  这个暂时没有用，创建多个桌子的
	Lint			CreateVipDeskForClub(LMsgLMG2LCreateDeskForClub* pMsg);

	Lint       CreateVipDeskForClub(LMsgLMG2LCreateClubDeskAndEnter* pMsg, User* pJoinUser);

	Lint        AddToVipClubDesk(LMsgLMG2LEnterClubDesk* pMsg, User* pJoinUser);

	// pUsers的位置是根据位置排好序的
	Lint			CreateVipCoinDesk(LMsgCN2LCreateCoinDesk*pMsg,User* pUsers[]);

	Lint			AddToVipDesk(User* pUser, Lint nDeskID);

	Lint			AddToVipDesk(User* pUser, LMsgLMG2LAddToDesk* msg);

	Lint			SelectSeatInVipDesk(User* pUser, Lint nDeskID);

	bool			LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser);
	void            MHPrintAllDeskStatus();

	//与manager重新连接后，向manager同步桌子信息
	void SynchroDeskData(std::vector<LogicDeskInfo> & DeskInfo);

private:
	DeskMap		m_deskMap;

	std::queue<Lint>  m_deskId;
};

#define gRoomVip RoomVip::Instance()

class SpecPaiXingMgr : public LSingleton<SpecPaiXingMgr>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();
	bool FiltSpecPaiXingByFlag(Lint flag, const std::vector<Lint>& src, std::vector<int>&dst);

	Lstring GetSpecPaiXingString(Lint isWin, Lint flag, const std::vector<Lint> &src, bool bEnableFilter);

private:
	std::map<Lint, boost::shared_ptr<std::vector<Lint>>> m_mapFlagToSpecPaixing;
};
#define gPaiXingMgr SpecPaiXingMgr::Instance()

#endif