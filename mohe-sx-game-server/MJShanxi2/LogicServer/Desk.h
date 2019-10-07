#ifndef _DESK_H_
#define _DESK_H_

#include "User.h"
#include "Card.h"
#include "AI.h"
#include "LTime.h"
#include "LVideo.h"
#include "GameHandler.h"
#include "GameDefine.h"
#include "ClientConstants.h"


class Room;
struct VipLogItem;

//#define DEBUG_MODEL

//俱乐部信息
//用于桌子记录自己的俱乐部信息
struct  DeskClubInfo
{
	Lint  m_clubId;
	Lint  m_playTypeId;
	Lint  m_clubDeskId;
	Lint  m_showDeskId;
	Lint  m_feeCost;         //桌子房费
	

	DeskClubInfo()
	{
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_showDeskId=0;
		m_feeCost=0;
		
	}
	DeskClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId,Lint showDeskId)
	{
		m_clubId=clubId;
		m_playTypeId=playTypeId;
		m_clubDeskId=clubDeskId;
		m_showDeskId=showDeskId;
	}
	void reset()
	{
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_showDeskId=0;
		m_feeCost=0;
	}
};

struct ThinkTool
{
	ThinkVec	m_thinkData;
	TingVec		m_tingData;

	void Reset()
	{
		m_thinkData.clear();
		m_tingData.clear();
	}

	ThinkTool()
	{
		Reset();
	}

	bool HasHu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
				return true;
		}
		return false;
	}
	bool HasPeng()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_PENG)
				return true;
		}
		return false;
	}

	bool HasMBu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_MBU)
				return true;
		}
		return false;
	}

	bool HasMGang()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_MGANG)
				return true;
		}
		return false;
	}

	bool HasFly()
	{
		for(Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_FLY)
				return true;
		}
		return false;
	}

	bool HasReturn()
	{
		for(Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_RETURN)
				return true;
		}
		return false;
	}

	bool HasAnGang(Card *& pCard)
	{
		bool bRet = false;
		pCard = NULL;
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_AGANG)
			{
				pCard = m_thinkData[i].m_card.back();
				bRet = true;
			}
				
		}
		return bRet;
	}

	bool HasTing()
	{
		return false;
	}
	bool NeedThink()
	{
		return !m_thinkData.empty();
	}
};

enum DeskType
{
	DeskType_Common,   //普通场
	DeskType_Coins,    //金币场
};

class Desk
{
public:
	Desk();
	~Desk();

	bool			initDesk(int iDeskId, GameType gameType);

	Lint			GetDeskId();
	Lint			GetPlayerCapacity();

	void			SetVip(VipLogItem* vip);
	VipLogItem*		GetVip();

	void			Tick(LTime& curr);

	void			HanderUserReady(User* pUser,LMsgC2SUserReady* msg);
	void			HanderSelectSeat(User* pUser, LMsgC2SUserSelectSeat* msg);
	void			HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	void			HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg);

	void            HanderUserTangReq(User* pUser,LMsgC2STangCard* msg);

	void			HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg);		//玩家处理海底牌
	void			SyncMsgUserEndSelect(Lint nPos, Lint state, Lint color, Lint number);

	void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg);
	void			HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg);

	void			HanderUserOutDesk(User* pUser);

	void			HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg);

	void			HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg);

	// 玩家换牌
	void			HanderUserChange(User* pUser, LMsgC2SUserChange* msg);

	void			HanderGameOver(Lint result);

	void			HanderDelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin, Lint feeType, Lint userId, Lstring unioid);

	void			HanderAddCardCount(Lint pos, Lint CardNum, CARDS_OPER_TYPE AddType, Lstring admin);

	bool            DissmissAllplayer();
	Lint            GetFirstZhuangPos();

	//////////////////////////////////////////////////////////////////////////
	Lint			getGameType() const;
	const std::vector<Lint>&	getPlayType() const;
	std::vector<std::string>    getAllPlayerName();        //返回所有用户名字
	Lint			getDeskState() const;
	void			setDeskState(Lint nState);
	Lint			getDeskPlayState();
	void			setDeskPlayState(Lint nDeskPlayState);
	//////////////////////////////////////////////////////////////////////////

	void			ResetEnd();
	bool			OnUserReconnect(User* pUser);

	void			OnUserInRoom(User* user);
	void			OnUserOutRoom(User* user);

	void			OnUserInRoom(User* user[]);

	Lint			GetFreePos(); 
	Lint			GetUserPos(User* user);
	Lint			GetUserSeatPos(User* user);
	User*			GetPosUser(Lint pos);

	Lint			GetNextPos(Lint prev);
	Lint			GetPrevPos(Lint next);

	Lint			GetUserCount();


	void			CheckGameStart();   //添加开始按钮以后，这个函数不再使用了，改用MHCheckGameStart()   

	void			SetDeskWait();
	void			OnDeskThingIng();//等待玩家思考中
	void			OnDeskWait();
	void			SetAllReady();


	void			SetDeskFree();

	void			CheckReset();
	void			ClearUser();
	void			BoadCast(LMsg& msg);
	void			BoadCastWithOutUser(LMsg& msg,User* user);

	void			ResetClear();

	void		    SpecialCardClear();   //初始化 特定牌标志

	// 设置倒计时
	void			SetAutoPlay( Lint pos, bool bauto, Lint outtime );
	void            CheckAutoPlayCard();

	void            SetDeskType(DeskType type);
	DeskType        GetDeskType();
	void            SetSelectSeat(bool state);
	Lint            GetCreatDeskTime();
	void            SetCreatType(Lint type);
	Lint            GetCreatType();
	void            MHProcessRobotSelectSeat(User * pUser);
	void            MHPrintDeskStatus();

	/////////////////////////////////////////////////by wyz
	//用户是否时lookon用户
	bool           MHIsLookonUser(User* pUser);
	//用户是否在房间
	bool           MHIsRoomUser(User* pUser);
	//用户是否入座
	bool          MHIsSeatUser(User* pUser);
	//用户是否加入座位记录
	bool		  MHIsInSeatTurn(User* pUser);
	//广播桌子所有用户
	void			MHBoadCastAllDeskUser(LMsg& msg);
	void           MHBoadCastAllDeskUserWithOutUser(LMsg& msg, User* user);
	//广播桌子Lookon用户
	void           MHBoadCastDeskLookonUser(LMsg& msg);
	void           MHBoadCastDeskLookonUserWithOutUser(LMsg& msg, User* user);
	//广播桌上座位用户
	void           MHBoadCastDeskSeatUser(LMsg& msg);
	void           MHBoadCastDeskSeatUserWithOutUser(LMsg& msg, User* user);

	//创建桌子以后设置>2ren 开局
	void          MHSetGreater2CanStartGame(Lint can) { m_Greater2CanStart=can; }
	//入座人数  ;GetUserCount() 是游戏人数
	Lint	          MHGetSeatUserCount();
	Lint          MHGetDeskUserCount();
	//判断是否开始游戏,返回值: 0--不可以开局  1--第一局，可以开局；  2--非第一局可以开局
	Lint          MHCheckGameStart();
	//开始游戏
	void          MHHanderStartGame();
	//解散时，清空lookon用户， 清空m_desk_all_user变量
	void          MHClearLookonUser();
	//客户端发送开始游戏指令
	void           MHHanderStartGameButton(User* pUser, LMsgC2SStartGame *msg);
	//选择特殊用户的位置位置（规则：如果创建房间的人入座了，返回其位置；如果没有选择第一个入座的人的位置
	Lint           MHSpecPersonPos();
	//自己建房，房主退出房间，把开始按钮位置，这个函数没有检查参数，使用者自己用  ,这个函数不用了，（现在的逻辑：房主选座后退出，再回到房间，他还是原来的座位；其他人选座后退出，再进入需要重新选座）
	void         MHSetStartButtonPos(Lint pos) { m_StartGameButtonPos=pos; }
	Lint         MHGetGreater2CanStart() { return  m_Greater2CanStart; }
	//返回房主在m_seatUser 中的位置,不存在返回INVAILD_POS
	Lint         MHGetCreatorSeatPos();    
	//返回房主在m_user 中的位置,不存在返回INVAILD_POS
	Lint         MHGetCreatorPos();

	//俱乐部部分
	void       MHSetClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId,Lint showDeskId) {
		m_clubInfo.m_clubId=clubId; m_clubInfo.m_playTypeId=playTypeId; m_clubInfo.m_clubDeskId=clubDeskId; m_clubInfo.m_showDeskId=showDeskId;
	}
	Lint       MHGetClubId() { return m_clubInfo.m_clubId; }
	Lint       MHGetPlayTypeId() { return m_clubInfo.m_playTypeId; }
	Lint       MHGetClubDeskId() { return m_clubInfo.m_clubDeskId; }
	Lint       MHGetClubShowDeskId() { return m_clubInfo.m_showDeskId; }
	Lint       MHGetDeskUser(std::vector<Lstring> &seatPlayerName, std::vector<Lstring> &noSeatPlayerName);

	void       MHNotifyManagerDeskInfo(Lint roomFull, Lint currCircle=0, Lint totalCircle=0);

	bool       MHCheckUserGPSLimit(User *pUser, Lstring & desk_gps_list);

	bool      MHDismissDeskOnPlay();
	/////////////////////////////////////////////////

protected:
	void			_clearData();
	bool			_createRegister(GameType gameType);

public:
	Lint			m_deskState;					//桌子状态
	Lint			m_deskbeforeState;				//解散房间前桌子状态
	Lint			m_deskPlayState;				//打牌状态
	Lint			m_state;						//房间规则
	std::vector<Lint>	m_playtype;					//玩法规则
	Lstring			m_resetUser;					//申请的玩家
	Lint            m_resetUserId;                  //申请解散玩家ID
	Lint			m_resetTime;					//申请解算的时间
	Lint			m_timeDeskCreate;				//桌子创建的时间
public:
	User*           m_creatUser;                  //添加房主建房以后，不要再用这个变量判断是否房主了，改用m_creatUserId；
	Lint            m_creatUserId;                  //创建桌子的玩家Id
	Lstring         m_creatUserNike;                //创建桌子的玩家昵称
	Lstring         m_creatUserUrl;                 //创建桌子的玩家头像地址
	Lstring         m_unioid;                       //创建桌子的玩家unioid
public:
	User*			m_user[DESK_USER_COUNT];		//玩家列表   //关于几个变量说明 by wyz： m_user  , m_seatUser，m_desk_Lookon_user     用户进入房间只写入m_desk_all_user，m_desk_Lookon_user 这两个变量；用户选座成功以后用户写入m_seatUser（东南西北实际位置），从m_desk_Lookon_user移除；游戏开始时，写入m_user（位置与实际位置无关，按东南西北依次排）,m_switchPos位置记录转换关系  by wyz
	User*			m_firestUser;                   //第一个选择座位的玩家     // ????   by wyz  当入座用户退出以后，这个变量不能正确确定下个用户

	boost::recursive_mutex    m_mutexSeat;
	Lint            m_seat[DESK_USER_COUNT];        //玩家座位列表
	User*			m_seatUser[DESK_USER_COUNT];	//玩家列表 所有玩家选好位置后使用 index 为所选的位置号   //关于几个变量说明 by wyz： m_user  , m_seatUser，m_desk_Lookon_user     用户进入房间只写入m_desk_all_user，m_desk_Lookon_user 这两个变量；用户选座成功以后用户写入m_seatUser（东南西北实际位置），从m_desk_Lookon_user移除；游戏开始时，写入m_user（位置与实际位置无关，按东南西北依次排）,m_switchPos位置记录转换关系  by wyz
	Lint			m_readyState[DESK_USER_COUNT];	//玩家准备状态 ，用户准备的index是和m_seatUser的index是一致的，和m_user的不一定（考虑到后面每局可以新加入玩家游戏）by wyz
	Lint            m_videoPermit[DESK_USER_COUNT]; //视频允许
	GameHandler*	mGameHandler;					//具体游戏处理器
	VipLogItem*		m_vip;							//录相
	Card            m_specialCard[CARD_COUNT];		//存储玩家定义牌序
	Lint			m_reset[DESK_USER_COUNT];		//玩家装态重置
	bool            m_dismissUser;

	// 倒计时运行时数据
	bool			m_autoPlay[DESK_USER_COUNT];	//是否检查自动出牌
	Lint			m_autoPlayTime[DESK_USER_COUNT];//自动出牌的倒计时
	Lint			m_autoOutTime[DESK_USER_COUNT]; //倒计时的超时时间

	// 配置
	Lint			m_autoChangeOutTime;			//自动换牌的时间
	Lint			m_autoPlayOutTime;				//自动出牌超时时间 配置

	Lint			m_baseScore;					//金币场基底分 可以用于表示其他卡牌场的基础分 目前为1
	Lint            m_coinsResult[DESK_USER_COUNT];	//玩家结算时的金币 增量

	Lint            m_desk_user_count;
	Lint			m_flag;							//房间规则
	bool            m_selectSeat;                   //选座状态  创建房间时为true 选择完座位时为false
	Lint            m_zhuangPos;                    //庄位置
	Lint            m_cost;                                //?创建者费用
	Lint            m_even;                                //?平摊每个人费用
	Lint            m_free;
	Lint            m_startTime;
	Lint            m_endTime;
	Lint            m_Gps_Limit;                  // 是否GPS限制房间
	Lint			m_allowLookOn;					//是否支持观战

	time_t       m_gameStartTime;

	//存储玩家定义牌序
	Card            m_player1Card[13];
	Card            m_player2Card[13];
	Card            m_player3Card[13];
	Card            m_player4Card[13];
	Card            m_SendCard[84];
	Card            m_HaoZiCard[2];
public:
	Lint            m_cheatAgainst;                 //防作弊
	Lint            m_feeType;                      //费用类型
	Lint            m_cellscore;                    //底分
	Lint            m_couFei;                       //是否已经扣费

private:
	Lint			m_id;							//桌子ID
	Lint			m_iPlayerCapacity;				//桌子玩家容量不能超过DESK_USER_COUNT

	// 桌子类型
	DeskType		m_deskType;
	Lint            m_deskCreatedType;              //房间创建类型 0:自己建房 1:房主建房

	////////////////////////////////
public:
	Lint                       m_Greater2CanStart;                       //>=2人选座就可以开始游戏
	//std::list<User*>  m_desk_all_user;                        //房间总人数用户  //关于几个变量说明 by wyz   //  by wyz 不再使用这个变量，seat+lookon=all，维护及同步这个变量没有必要，容易出错
	boost::mutex m_mutexDeskLookonUser;
	std::list<User*>  m_desk_Lookon_user;                 //房间未入座的人数  //关于几个变量说明 by wyz
	Lint                         m_switchPos[DESK_USER_COUNT];   //m_user和m_seatUser位置转换关系
	Lint                         m_StartGameButtonPos;                //开始游戏按钮位置用户，东南西北（0-3）
	Lint                         m_startButtonAppear;
	std::list<User*>  m_user_select_seat_turn;              // 选座用户时间排序，退出用户再进入选座排在后面，第一局开始游戏后清空变量
	////////////////////////////////
	bool m_bIsDissmissed;  // 每局游戏结束状态 false:游戏正常结束 true:游戏中途解散
	//俱乐部部分
public:
	DeskClubInfo        m_clubInfo;                                         //俱乐部部分

	///////////////////////////////



};

typedef std::map<DWORD,Desk*> DeskMap;
typedef std::vector<Desk*> DeskVec;
#endif