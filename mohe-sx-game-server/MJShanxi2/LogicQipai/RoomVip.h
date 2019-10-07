#ifndef _ROOM_VIP_H
#define _ROOM_VIP_H

#include "LMsg.h"
#include "mhgamedef.h"
#include "Desk.h"
//#include "LVipLog.h"

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

struct  VipDeskLogForQiPai
{
	Lstring	 m_videoId;					//录像id
	Lint	 m_gold[MAX_CHAIR_COUNT];   //每局局分数
	Lint	 m_zhuangPos;				//坐庄的位置
	Lint	 m_time;					//本局时间

	Lstring  m_str;						//牌局记录
	
	Lint	 m_win[MAX_CHAIR_COUNT];    //收炮次数
	

	VipDeskLogForQiPai()
	{
		memset(m_gold, 0, sizeof(m_gold));		
		m_zhuangPos=INVAILD_POS_QIPAI;
		m_time=0;
		m_videoId="";	
		memset(m_win, 0, sizeof(m_win));	
	}

	Lstring	ScoreToString()
	{
		std::stringstream ss;
		ss << m_time << ";";
		ss << m_videoId << ";";
		for (Lint i = 0; i < MAX_CHAIR_COUNT; i++)
		{
			ss << m_gold[i];
			if (i < MAX_CHAIR_COUNT - 1)
				ss << ",";
		}
		//ss << ";";
		return ss.str();
	}
};


struct LVipLogItemForQiPai
{
	Lstring			m_id;							//id
	Lint			m_time;							//时间
	Lint			m_state;						//房间玩法：101：斗地主，103：牛牛，106：升级，107：3打2
	Lint			m_deskId;						//桌子id
	Lstring			m_secret;						//房间密码
	Lint			m_maxCircle;					//总圈数
	Lint			m_curCircle;					//当前圈数，会刷给客户端的，显示类似“第1/8局”字样
	Lint			m_curCircleReal;				//实际的当前圈数，因为有的玩法连庄不加圈数，该值仅用于判断第1局insert其它局update到数据库
	Lint        m_isInsertDB;                  //趣味分享，是否插入数据库数据
	Lint			m_posUserId[MAX_CHAIR_COUNT];	//各个位置上的玩家id
	Lint            m_playerState[MAX_CHAIR_COUNT];	//对应位置玩家是否参与游戏，每局开始时更新 
	Lint			m_curZhuangPos;					//当前庄家
	Lint			m_score[MAX_CHAIR_COUNT];		//各个位置上面的积分
	Lint			m_coins[MAX_CHAIR_COUNT];		 //玩家能量值
	Lint			m_reset;						//是否解散
	std::vector<Lint> m_playtype;					//玩法小选项列表
	std::vector<VipDeskLogForQiPai*> m_log;         //每一把的记录
	Lint			m_checkTing[MAX_CHAIR_COUNT];	//查听次数
	Lint			m_iPlayerCapacity;				//桌子玩家容量不能超过DESK_USER_COUNT

	Lint			m_qiangCount[MAX_CHAIR_COUNT];		//牛牛本场抢庄次数
	Lint			m_zhuangCount[MAX_CHAIR_COUNT];		//牛牛本场坐庄次数
	Lint			m_tuiCount[MAX_CHAIR_COUNT];		//牛牛本场推注次数

	LVipLogItemForQiPai();

	virtual  ~LVipLogItemForQiPai();

	virtual  Lstring		ToString();

	virtual Lstring        ScoreToString();

	virtual  void			FromString(const Lstring& str);

	virtual  Lstring		PlayTypeToStrint();

	virtual  void			PlayTypeFromString(const Lstring& str);
};

struct MHExtendVipLogItem
{
	Lstring m_tableId;
	std::map<Lint, Lstring> m_mapUserIdToDrawId;	
	virtual void CreateTableId() = 0;         // 创建表Id
	virtual void SaveDeskInfoToDB() = 0;      // 存储到数据库
	//发送大赢家给manager
	void SendBigWinnerInfo(Lint clubId, Lint deskId, Lint userId, Lstring nike, Lint state, Lint type);
};

struct VipLogItem : public LVipLogItemForQiPai, MHExtendVipLogItem
{
public:
	Desk*			m_desk;
public:
	VipLogItem();
	~VipLogItem();
	Lint		GetOwerId();

	void		AddLog(User** user, Lint* gold, HuInfo* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting);

	void		AddLog(User** user, Lint* gold, std::vector<HuInfo>* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting);

	/**********************************************************************************************
	***user: user refer, from desk
	***gold: score array
	***playerState: user jion game state
	***zhuangpos: zhuang pos
	**********************************************************************************************/

	/* 更新玩家能量值 */
	void		UpdatePlayerCoins(Lint* changeCoins, Lint playerCount);
	
	//更新牛牛玩家每场操作的统计
	void		UpdateNiuNiuOptCount(Lint* qiangCount, Lint* zhuangCount, Lint* tuiCount, Lint playerCount);

	//牛牛玩家操作统计序列化（玩家1抢庄次数，玩家2抢庄次数...|玩家1坐庄次数，玩家2坐庄次数...|玩家1推注次数，玩家2退出次数）
	Lstring		NiuNiuOptCountToString();

	void		AddLogForQiPai(User** user, Lint* gold, Lint * playerState, Lint zhuangPos, Lstring& videoId, const  std::vector<std::string> & srcPaiXing = std::vector<std::string>());

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

	//场次结束，大结算
	void		SendEnd();

	Lstring		PlayTypeToString4RLog();

	void		SendInfo();

	bool		IsBegin();

	void		RemoveUser(Lint id);

	Lint        GetCurrentPlayerCount();

	virtual void CreateTableId();        // 创建表Id
	virtual void SaveDeskInfoToDB();      // 存储到数据库	
	virtual void UpdateDeskTotalFeeValueToDB(Lint nFeeValue); // 修改桌子总费用
	virtual void InsertUserScoreToDB(Lint pos, Lstring &drawId, Lint isWin, Lint score, const std::string & srcPaiXing);
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

	VipLogItem*		FindUserPlayingItem(Lint id);//查询玩家正在进行中的桌子

	void			AddPlayingItem(VipLogItem* item);//添加未完成的桌子

	void			Tick();
private:
	std::map<Lstring,VipLogItem*> m_item;
	Lint			m_id;

	Lint			m_videoId;
};

#define gVipLogMgr VipLogMgr::Instance()


//vip房间
class RoomVip:public LSingleton<RoomVip>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	/* 定时器 */
	void			Tick(LTime& cur);

	/* 寻找一个空桌子，并初始化桌子ID 和游戏大玩法*/
	Desk*			GetFreeDesk(Lint nDeskID, QiPaiGameType gameType);

	/* 根据桌子ID，获取桌子对象*/
	Desk*			GetDeskById(Lint id);

	/* 玩家创建桌子 */
	Lint			CreateVipDesk(LMsgLMG2LCreateDesk* pMsg,User* pUser);

	/* 玩家为其他玩家创建桌子 */
	Lint			CreateVipDeskForOther(LMsgLMG2LCreateDeskForOther* pMsg, User* pUser);

	//俱乐部  这个暂时没有用，创建多个桌子的
	Lint			CreateVipDeskForClub(LMsgLMG2LCreateDeskForClub* pMsg);

	Lint			CreateVipDeskForClub(LMsgLMG2LCreateClubDeskAndEnter* pMsg, User* pJoinUser);

	Lint			AddToVipClubDesk(LMsgLMG2LEnterClubDesk* pMsg, User* pJoinUser);


	// pUsers的位置是根据位置排好序的
	Lint			CreateVipCoinDesk(LMsgCN2LCreateCoinDesk*pMsg,User* pUsers[]);

	Lint			AddToVipDesk(User* pUser, Lint nDeskID);

	Lint			AddToVipDesk(User* pUser, LMsgLMG2LAddToDesk* msg);

	//处理观战玩家入座
	Lint			VipLookOnUserSeatDown(User* pUser, LMsgG2LLookOnUserSearDown* msg);

	Lint			SelectSeatInVipDesk(User* pUser, Lint nDeskID);

	bool			LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser);
	void            MHPrintAllDeskStatus();

//与manager重新连接后，向manager同步桌子信息
	void SynchroDeskData(std::vector<LogicDeskInfo> & DeskInfo);

	//子游戏局数
	Lint  getGameDrawCount(Lint game_state, Lint no);

	//根据小选项判断游戏人数
	bool getDeskUserMax(Lint game_state, std::vector<Lint>& playType, Lint * userMax);

private:
	DeskMap		m_deskMap;

	std::queue<Lint>  m_deskId;
};

#define gRoomVip RoomVip::Instance()



#endif