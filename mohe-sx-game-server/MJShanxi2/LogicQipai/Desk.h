#ifndef _DESK_H_
#define _DESK_H_

#include "User.h"
#include "LTime.h"
#include "LVideo.h"
#include "GameHandler.h"
#include "GameDefine.h"
#include "ClientConstants.h"
#include "mhgamedef.h"

//#define DEBUG_MODEL 1

class Room;
struct VipLogItem;

/* 房间类型：0:普通场  1:金币场 */
enum DeskType
{
	DeskType_Common,   //普通场
	DeskType_Coins,    //金币场
};

/* 自动准备延时 */
enum USER_READY_DELAY_TIME
{
	TUITONGZI_READY_DELAY_TIME = 20,
	NIUNIU_READY_DELAY_TIME = 8,  //10
	NEWNIUNIU_READY_DELAY_TIME = 10,
	NEWNIUNIU_READY_DELAY_TIME_FAST = 5,
	//YINGSANZHANG_READY_DELAY_TIME=15,
	YINGSANZHANG_READY_DELAY_TIME = 8,
	AUTO_START_GAME_DELAY_TIME = 30,
	YINGSANZHANG_GAME_START_DELAY_TIME = 8,
	CHEXUAN_GAME_START_TIME = 40,
	CHEXUAN_GAME_FAST_START_TIME = 3,
};

/*
俱乐部信息
用于桌子记录自己的俱乐部信息
*/
struct  DeskClubInfo
{
	Lint  m_clubId;			//俱乐部ID
	Lint  m_playTypeId;
	Lint  m_clubDeskId;		//俱乐部桌子ID
	Lint  m_showDeskId;
	Lstring m_clubName;		//俱乐部名称
	Lint  m_feeCost;        //桌子房费
	std::vector<Lint> m_managerIdList;				//俱乐部管理员ID

	/* 构造函数 */
	DeskClubInfo()
	{
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_showDeskId=0;
		m_feeCost=0;
	}
	/* 赋值 */
	DeskClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId, Lint showDeskId)
	{
		m_clubId=clubId;
		m_playTypeId=playTypeId;
		m_clubDeskId=clubDeskId;
		m_showDeskId=showDeskId;
	}

	/* 赋值 */
	DeskClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId, Lint showDeskId, std::vector<Lint>& managerIdList)
	{
		m_clubId = clubId;
		m_playTypeId = playTypeId;
		m_clubDeskId = clubDeskId;
		m_showDeskId = showDeskId;
		m_managerIdList = managerIdList;
	}

	/* 赋值 */
	DeskClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId, Lint showDeskId, std::vector<Lint>& managerIdList, Lstring clubName)
	{
		m_clubId = clubId;
		m_playTypeId = playTypeId;
		m_clubDeskId = clubDeskId;
		m_showDeskId = showDeskId;
		m_managerIdList = managerIdList;
		m_clubName = clubName;
	}

	/* 重置结构体 */
	void reset()
	{
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_showDeskId=0;
		m_feeCost=0;
		m_managerIdList.clear();
		m_clubName.clear();
	}
};

/* 桌子类 */
class Desk
{
	friend class VipLogItem;
public:
	Desk();
	~Desk();

	/* 初始化桌子对象 */
	bool			initDesk(int iDeskId, QiPaiGameType gameType);

	/* 获取桌子ID */
	Lint			GetDeskId();

	/* 获取玩法限定的最大玩家数量 */
	Lint			GetPlayerCapacity();

	void			SetVip(VipLogItem* vip);
	VipLogItem*		GetVip();

	void			Tick(LTime& curr);

	void			HanderUserReady(User* pUser, LMsgC2SUserReady* msg);
	void			HanderSelectSeat(User* pUser, LMsgC2SUserSelectSeat* msg);
	void			HanderUserPlayCard(User* pUser, LMsgC2SUserPlay* msg);
	void			HanderUserStartHu(User* pUser, LMsgC2SUserStartHuSelect* msg);

	void            HanderUserTangReq(User* pUser, LMsgC2STangCard* msg);

	void			HanderUserEndSelect(User* pUser, LMsgC2SUserEndCardSelect* msg);		//玩家处理海底牌
	void			SyncMsgUserEndSelect(Lint nPos, Lint state, Lint color, Lint number);

	void			HanderUserOperCard(User* pUser, LMsgC2SUserOper* msg);
	void			HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg);

	void			HanderUserOutDesk(User* pUser);

	void			HanderResutDesk(User* pUser, LMsgC2SResetDesk* msg);

	void			HanderSelectResutDesk(User* pUser, LMsgC2SSelectResetDesk* msg);

	// 玩家换牌
	void			HanderUserChange(User* pUser, LMsgC2SUserChange* msg);

	void			HanderGameOver(Lint result);

	void			HanderDelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin, Lint feeType, Lint userId, Lstring unioid);

	void			HanderAddCardCount(Lint pos, Lint CardNum, CARDS_OPER_TYPE AddType, Lstring admin);

	//更新玩家金币数量
	void			HanderUpdateUserCoins(User* pUser, LMsgLMG2LUpdatePointCoins* msg);

	//能量不足玩家充上能量后从新点击入座 61184
	void			HanderStandPlayerReSeat(User* pUser, LMsgC2SStandPlayerReSeat* msg);

	//玩家请求观战玩家列表信息
	void			HanderViewLookOnList(User* pUser, LMsgC2SViewLookOnList* msg);

	//////////////////////////////////////////////////////////////////////////
	//扑克游戏接口  新加
	//////////////////////////////////////////////////////////////////////////
	////赢三张
	void            HanderPokerGameMessage(User* pUser, LMsgC2SGameCommon *msg);
	void            HanderYingSanZhangAddScore(User* pUser, YingSanZhangC2SAddScore*msg);
	void            HanderYingSanZhangUserSelectBigSmall(User* pUser, YingSanZhangC2SUserSelectBigSmall* msg);

	////推筒子
	void			HanderTuiTongZiAddScore(User* pUser, TuiTongZiC2SScore*msg);
	void           HanderTuiTongZiSelectZhuang(User* pUser, TuiTongZiC2SSelectZhuang*msg);
	void           HanderTuiTongZiDoShaiZi(User* pUser, TuiTongZiC2SDoShaiZi*msg);
	void           HanderTuiTongOpenCard(User* pUser, TuiTongZiC2SOpenCard*msg);

	// 斗地主
	void   HanderDouDiZhuGameMessage(User * pUser, MHLMsgDouDiZhuC2SGameMessage * msg);

	///////////////////////////////// 牛牛 //////////////////////////////////////
	/*
	牛牛：C->S 玩家选庄
	MSG_C_2_S_NIUNIU_SELECT_ZHUANG = 62071
	*/
	void           HanderNiuNiuSelectZhuang(User* pUser, NiuNiuC2SSelectZhuang* msg);

	/*
	牛牛：C->S 玩家买码
	MSG_C_2_S_NIUNIU_MAI_MA = 62082
	*/
	void		   HanderNiuNiuMaiMa(User* pUser, NiuNiuC2SMaiMa* msg);

	/*
	牛牛：C->S 玩家下注
	MSG_S_2_C_NIUNIU_ADD_SCORE = 62074
	*/
	void           HanderNiuNiuAddScore(User* pUser, NiuNiuC2SAddScore* msg);

	/*
	牛牛：C->S 玩家亮牌
	MSG_C_2_S_NIUNIU_OPEN_CARD = 62075
	*/
	void           HanderNiuNiuOpenCard(User* pUser, NiuNiuC2SOpenCard* msg);

	/*
	牛牛：玩家搓公牌
	MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
	*/
	void			HandNiuNiuCuoGongPai(User* pUser, NiuNiuC2SCuoGongPai* msg);

	/*
	牛牛：玩家选择托管消息
	MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
	*/
	void			HandNiuNiuTuoGuan(User* pUser, NiuNiuC2STuoGuan* msg);

	/*
	牛牛：玩家取消托管消息
	MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
	*/
	void			HandNiuNiuCancelTuoGuan(User* pUser, NiuNiuC2SCancelTuoGuan* msg);

	/*
	客户端通知服务器开始计时
	MSG_C_2_S_START_TIME = 61198
	*/
	void			HandNiuNiuStartTime(User* pUser, NiuNiuC2SStartTime* msg);

	//////////////////////////////// 双升 ////////////////////////////////
	/*
	双升：C->S 玩家报主操作
	MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202
	*/
	void			HandleShuangShengSelectZhu(User* pUser, ShuangShengC2SSelectZhu* msg);

	/*
	双升：C->S 玩家反主操作
	MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204
	*/
	void			HandleShangShengFanZhu(User* pUser, ShuangShengC2SFanZhu* msg);

	/*
	双升：C->S 玩家盖底牌操作
	MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206
	*/
	void			HandleShuangShengBaseCards(User* pUser, ShuangShengC2SBaseCards* msg);

	/*
	双升：C->S 玩家出牌操作
	MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208
	*/
	void			HandleShuangShengOutCards(User* pUser, ShuangShengC2SOutCards* msg);

	/*
	3打2：C->S 玩家选择叫牌分
	MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
	*/
	void		HandleSanDaErSelectScore(User* pUser, SanDaErC2SSelectScore* msg);

	/*
	3打2：C->S 玩家选主牌
	MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
	*/
	void		HandleSanDaErSelectMain(User* pUser, SanDaErC2SSelectMain* msg);

	/*
	3打2：C->S 玩家埋底
	MSG_C_2_S_SANDAER_BASE_CARD = 62257
	*/
	void		HandleSanDaErBaseCard(User* pUser, SanDaErC2SBaseCard* msg);

	/*
	3打2：C->S 玩家选对家牌
	MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
	*/
	void		HandleSanDaErSelectFriend(User* pUser, SanDaErC2SSelectFriend* msg);

	/*
	3打2：C->S 玩家出牌
	MSG_C_2_S_SANDAER_OUT_CARD = 62261
	*/
	void		HandleSanDaErOutCard(User* pUser, SanDaErC2SOutCard* msg);

	/*
	3打2：C->S 玩家回复闲家得分慢105分,是否停止游戏
	MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
	*/
	void HandleSanDaErScore105Ret(User* pUser, SanDaErC2SScore105Ret* msg);

	/*
	3打2：C->S 庄家请求底牌
	MSG_C_2_S_USER_GET_BASECARDS = 61027,		//
	*/
	void HandleSanDaErGetBaseCards(User* pUser, SanDaErC2SGetBaseCards* msg);

	/*3打2：C->S 客户端庄家选择交牌
	MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272
	*/
	bool HandleSanDaErZhuangRequestJiaoPai(User* pUser, SanDaErC2SZhuangJiaoPai* msg);

	/*
	C->S 非庄家  客户端选择庄家交牌的申请
	MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274
	*/
	bool HandleSanDaErXianSelectJiaoPai(User* pUser, SanDaErC2SXianSelectJiaoPai* msg);
	/*

	五人百分：C->S 玩家选择叫牌分
	MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
	*/
	void HandleWuRenBaiFenSelectScore(User* pUser, WuRenBaiFenC2SSelectScore* msg);

	/*
	五人百分：C->S 玩家选主牌
	MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
	*/
	void HandleWuRenBaiFenSelectMain(User* pUser, WuRenBaiFenC2SSelectMain* msg);

	/*
	五人百分：C->S 玩家埋底
	MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
	*/
	void HandleWuRenBaiFenBaseCard(User* pUser, WuRenBaiFenC2SBaseCard* msg);

	/*
	五人百分：C->S 玩家选对家牌
	MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
	*/
	void HandleWuRenBaiFenSelectFriend(User* pUser, WuRenBaiFenC2SSelectFriend* msg);

	/*
	五人百分：C->S 玩家出牌
	MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
	*/
	void HandleWuRenBaiFenOutCard(User* pUser, WuRenBaiFenC2SOutCard* msg);

	/*
	五人百分：C->S 玩家回复闲家得分慢105分,是否停止游戏
	MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,
	*/
	void HandleWuRenBaiFenScore105Ret(User* pUser, WuRenBaiFenC2SScore105Ret* msg);


	/*
	五人百分：交牌
	*/
	void HandleWuRenBaiFenZhuangJiaoPai(User* pUser, WuRenBaiFenC2SZhuangJiaoPai * msg);
	void HandleWuRenBaiFenXianAckJiaoPai(User* pUser, WuRenBaiFenC2SXianSelectJiaoPai * msg);

	/*
	3打1：C->S 玩家选择叫牌分
	MSG_C_2_S_SANDAYI_SELECT_SCORE = 62253
	*/
	void HandleSanDaYiSelectScore(User* pUser, SanDaYiC2SSelectScore* msg);

	/*
	3打1：C->S 玩家选主牌
	MSG_C_2_S_SANDAYI_SELECT_MAIN = 62255
	*/
	void HandleSanDaYiSelectMain(User* pUser, SanDaYiC2SSelectMain* msg);

	/*
	3打1：C->S 玩家埋底
	MSG_C_2_S_SANDAYI_BASE_CARD = 62257
	*/
	void HandleSanDaYiBaseCard(User* pUser, SanDaYiC2SBaseCard* msg);

	/*
	3打1：C->S 玩家出牌
	MSG_C_2_S_SANDAYI_OUT_CARD = 62261
	*/
	void HandleSanDaYiOutCard(User* pUser, SanDaYiC2SOutCard* msg);

	/*
	3打1：C->S 玩家回复闲家得分慢105分,是否停止游戏
	MSG_C_2_S_SANDAYI_SCORE_105_RET = 62266,
	*/
	void HandleSanDaYiScore105Ret(User* pUser, SanDaYiC2SScore105Ret* msg);

	/*
	扯炫：C->S 处理玩家起钵钵
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
	*/
	void HandlerPlayerSelectBoBo(User* pUser, CheXuanC2SPlayerSelectBoBo* msg);

	/*
	扯炫：C->S 处理玩家选择操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
	*/
	void HandlerPlayerSelectOpt(User* pUser, CheXuanC2SPlayerSelectOpt* msg);

	/*
	扯炫：C->S 处理玩家扯牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
	*/
	void HandlerPlayerChePai(User* pUser, CheXuanC2SPlayerChePai* msg);

	/*
	C->S 玩家搓牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
	*/
	void HandlerPlayerCuoPai(User* pUser, CheXuanC2SPlayerCuoPai* msg);

	/*
	C->S 客户端牌局内请求战绩
	MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
	*/
	void HandlerPlayerRequestRecord(User* pUser, CheXuanC2SPlayerRequestRecord* msg);

	/*
	C->S 客户端操作默分
	MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
	*/
	void HandlerPlayerOptMoFen(User* pUser, CheXuanC2SPlayerOptMoFen* msg);

	/*
	C->S 客户端选择自动操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
	*/
	void HandlerPlayerSelectAutoOpt(User* pUser, CheXuanC2SPlayerSelectAutoOpt* msg);

	/*
	C->S 客户端选择站起或坐下
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
	*/
	void HandlerPlayerStandOrSeat(User* pUser, CheXuanC2SPlayerSelectStandOrSeat* msg);


	//申请房间用户gps
	void			HanderUserRequestGPS(User* pUser, LMsgC2SRequestUserGps* msg);

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
	bool			OnUserReconnect(User* pUser);   //断线重连 

	/* 玩家加入房间观看 */
	void			OnUserInRoomLookOn(User* user);
	/* 观战玩家入座，参与游戏 */
	void			OnUserInRoomLookOnSeatDown(User* user);

	void			OnUserInRoom(User* user);       //加入房间

	/*
		处理玩家离开桌子，与桌子关联删除
		outType离开类型 0：默认类型，兼容以前，1：房间解散，2：玩家离开房间，3：被踢出房间
	*/
	void			OnUserOutRoom(User* user, Lint outType = 0);

	void			OnUserInRoom(User* user[]);

	/* 获取桌子的入座状态 */
	void			GetDeskSeatStatus(Lint deskSeatStatus[]);

	Lint			GetFreePos();                   //获取空位

	/* 获取玩家位置，玩家不是已经入座玩家，则返回21位置 */
	Lint			GetUserPos(User* user);         //获取玩家位置

	Lint			GetUserSeatPos(User* user);
	User*			GetPosUser(Lint pos);           //获取位置玩家

	Lint			GetNextPos(Lint prev);
	Lint			GetPrevPos(Lint next);

	Lint			GetUserCoins(User* user);	//获取玩家能量值
	bool			SetUserCoins(User* user);	//设置玩家能量值

	//入座玩家的人数
	Lint			GetUserCount();

	//参与游戏玩家的人数
	Lint			GetPlayUserCount();

	//获取desk上已经动态加入的玩家数量
	Lint			GetDynamicInCount();

	void			CheckGameStart();

	void			SetDeskWait();                 // 房间为等待状态
	void			OnDeskThingIng();//等待玩家思考中
	void			OnDeskWait();
	void			SetAllReady();


	void			SetDeskFree();

	void			CheckReset();
	void			ClearUser();  //清空入座玩家 outType离开类型 0：默认类型，兼容以前，1：房间解散，2：玩家离开房间，3：被踢出房间
	void			BoadCast(LMsg& msg);
	void			BoadCast2PlayingUser(LMsg& msg);
	void			BoadCastWithOutUser(LMsg& msg, User* user);

	void			ResetClear();

	void		    SpecialCardClear();   //初始化 特定牌标志

	// 设置倒计时
	void			SetAutoPlay(Lint pos, bool bauto, Lint outtime);
	void            CheckAutoPlayCard();

	void            SetDeskType(DeskType type);
	DeskType        GetDeskType();
	void            SetSelectSeat(bool state);
	Lint            GetCreatDeskTime();
	void            SetCreatType(Lint type);

	/* 获取创建房间类型：0:自己建房  1:房主建房 */
	Lint            GetCreatType();

	//通知客户端观战玩家数量变化
	void			UpdateLookOnUserCount();

	void            MHProcessRobotSelectSeat(User * pUser);
	void            MHPrintDeskStatus();

	/////////////////////////////////////////////////by wyz
	//用户是否时lookon用户
	bool           MHIsLookonUser(User* pUser);
	//用户是否在房间
	bool           MHIsRoomUser(User* pUser);
	bool           MHIsUserInRoom(User* pUser);
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
	//广播给俱乐部名牌观战会长
	void		   MHBoadCastClubOwerLookOnUser(LMsg& msg);

	//广播桌上座位用户
	void           MHBoadCastDeskSeatUser(LMsg& msg);

	//广播给座位上正在参与游戏的玩家
	void		   MHBoadCastDeskSeatPlayingUser(LMsg& msg);

	//广播给动态加入玩家
	void		   MHBoadCastDeskSeatingUser(LMsg& msg);

	//广播给入座出去自己以外的玩家
	void           MHBoadCastDeskUserWithoutUser(LMsg& msg, User* user);

	//广播给管理员和入座玩家
	void           MHBoadCastManagerAndSeatUser(LMsg& msg, Lint nRet, const std::vector<Lint>& managerId);

	//删除LookOn中指定的user
	void			MHEraseDeskLookonUserByPuser(User* user);

	//删除SertList中指定的user
	void			MHEraseDeskSeatListUserByPuser(User* user);

	//创建桌子以后设置>2ren 开局
	void          MHSetGreater2CanStartGame(Lint can, std::vector<Lint>& playType);
	//入座人数  ;GetUserCount() 是游戏人数
	Lint	          MHGetSeatUserCount();
	Lint          MHGetDeskUserCount();
	Lint          MHGetUserReadyCount();
	Lint	      MHGetHanlderPlayerRealCount();
	Lint		  MHGetNextRoundDelayTime();
	//判断是否开始游戏,返回值: 0--不可以开局  1--第一局，可以开局；  2--非第一局可以开局
	Lint          MHCheckGameStart();
	//开始游戏
	void          MHHanderStartGame();
	//三张牌特殊检查游戏开始的函数
	void         MHHanderStartGameSanZhangPai(LTime &currTime);
	//解散时，清空lookon用户， 清空m_desk_all_user变量  outType离开类型 0：默认类型，兼容以前，1：房间解散，2：玩家离开房间，3：被踢出房间
	void          MHClearLookonUser(Lint outType = 0);
	//清除俱乐部会长观战  outType离开类型 0：默认类型，兼容以前，1：房间解散，2：玩家离开房间，3：被踢出房间
	void		  MHClearClubOwerLookOn(Lint outType = 0);
	//客户端发送开始游戏指令
	void           MHHanderStartGameButton(User* pUser, LMsgC2SStartGame *msg);
	//选择特殊用户的位置位置（规则：如果创建房间的人入座了，返回其位置；如果没有选择第一个入座的人的位置
	Lint           MHSpecPersonPos();
	//获取第一个入座玩家的位置
	Lint		   MHFirstSeatPos();
	//获取Pos位置下一个入座玩家的位置
	Lint		   MHNextFirstSeatPos(Lint pos);
	//查找管理员在房间的列表
	Lint		   MHGetManagerList(std::vector<Lint>& magagerList);
	//自己建房，房主退出房间，把开始按钮位置，这个函数没有检查参数，使用者自己用  ,这个函数不用了，（现在的逻辑：房主选座后退出，再回到房间，他还是原来的座位；其他人选座后退出，再进入需要重新选座）
	void         MHSetStartButtonPos(Lint pos) { m_StartGameButtonPos = pos; }
	Lint         MHGetGreater2CanStart() { return  m_Greater2CanStart; }

	//填充数据
	void       fillJoinRoomCheck(LMsgS2CJoinRoomCheck &data, Lint userId);

	//俱乐部部分
	void       MHSetClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId, Lint showDeskId) {
		m_clubInfo.m_clubId = clubId; m_clubInfo.m_playTypeId = playTypeId; m_clubInfo.m_clubDeskId = clubDeskId; m_clubInfo.m_showDeskId = showDeskId;
	}
	Lint       MHGetClubId() { return m_clubInfo.m_clubId; }
	Lint       MHGetPlayTypeId() { return m_clubInfo.m_playTypeId; }
	Lint       MHGetClubDeskId() { return m_clubInfo.m_clubDeskId; }
	Lint       MHGetClubShowDeskId() { return m_clubInfo.m_showDeskId; }
	Lint       MHGetDeskUser(std::vector<Lstring> &seatPlayerName, std::vector<Lstring> &noSeatPlayerName);

	void       MHNotifyManagerDeskInfo(Lint roomFull, Lint currCircle = 0, Lint totalCircle = 0);

	bool       MHCheckUserGPSLimit(User *pUser, Lstring & desk_gps_list);
	bool       MHDismissDeskOnPlay();

	//判断该桌子游戏是否开始  true:已经开始
	bool	   MHIsDeskMatchBegin();

	//检测玩法是否支持观战，分两进入房间
	bool       MHIsAllowLookOnMode(int playState);

	bool		SetHandlerPlayerStatus(const Lint* handlerPlayerStatus, Lint playerCount);

	////////////////////////////////设置桌子属性//////////////////////////////////

	//设置是否允许观战
	Lint		SetIsAllowLookOn();
	//设置是否允许俱乐部会长明牌观战
	Lint		SetIsAllownClubOwerLookOn();
	//设置桌子是否为金币场
	Lint		SetIsCoinsGame();
	//设置桌子是否开始后禁止加入房间
	Lint		SetIsStartNoInRoom();
	//设置是否允许动态加入牌局
	Lint		SetIsAllowDynamicIn();
	//设置是否为房主管理员开始游戏
	Lint		SetIsManagerStart();
	//设置房间房卡消耗（有些特殊玩法房卡不一样）
	Lint		SetDeskCostEven();

	////////////////////////////////设置能量场属性/////////////////////////////////

	//设置预警分数
	void		setWarnScore();

	//判断玩家是否为有效的入座玩家（竞技场中会有能量不足的玩家）
	bool		IsValidSeatDownPlayer(User* pUser);

protected:
	void			_clearData();
	bool			_createRegister(QiPaiGameType gameType);

public:
	Lint			m_deskState;					//桌子状态  0：桌子空闲，1：桌子等待，2：游戏进行中
	Lint			m_deskbeforeState;				//解散房间前桌子状态  0：桌子空闲，1：桌子等待，2：游戏进行中
	Lint			m_deskPlayState;				//打牌状态
	Lint			m_state;						//房间大玩法  101：斗地主，106：升级
	std::vector<Lint>	m_playtype;					//玩法小选项（创建房间时候被赋值）

public:
	Lstring			m_resetUser;					//申请的玩家昵称
	Lint            m_resetUserId;                  //申请解散玩家ID
	Lint			m_resetTime;					//申请解算的时间

public:
	Lint			m_timeDeskCreate;				//桌子创建的时间（桌子对象初始化时候被赋值）
	User*           m_creatUser;					//创建桌子玩家对象指针
	Lint            m_creatUserId;                  //创建桌子的玩家Id
	Lstring         m_creatUserNike;                //创建桌子的玩家昵称
	Lstring         m_creatUserUrl;                 //创建桌子的玩家头像地址
	Lstring         m_unioid;                       //创建桌子的玩家unioid
public:
	User*			m_user[MAX_CHAIR_COUNT];		//玩家列表   //关于几个变量说明 by wyz： m_user  , m_seatUser，m_desk_Lookon_user     用户进入房间只写入m_desk_all_user，m_desk_Lookon_user 这两个变量；用户选座成功以后用户写入m_seatUser（东南西北实际位置），从m_desk_Lookon_user移除；游戏开始时，写入m_user（位置与实际位置无关，按东南西北依次排）,m_switchPos位置记录转换关系  by wyz
	std::list<User*>  m_desk_Lookon_user;           //房间未入座的人数  //关于几个变量说明 by wyz
	User*			m_clubOwerLookOnUser;			//俱乐部会长观战，会长加入桌子的时候才会赋值
	User*			m_firestUser;                   //第一个选择座位的玩家     // ????   by wyz  当入座用户退出以后，这个变量不能正确确定下个用户

	boost::recursive_mutex    m_mutexSeat;
	Lint            m_seat[MAX_CHAIR_COUNT];        //玩家座位列表
	User*			m_seatUser[MAX_CHAIR_COUNT];	//玩家列表 所有玩家选好位置后使用 index 为所选的位置号   //关于几个变量说明 by wyz： m_user  , m_seatUser，m_desk_Lookon_user     用户进入房间只写入m_desk_all_user，m_desk_Lookon_user 这两个变量；用户选座成功以后用户写入m_seatUser（东南西北实际位置），从m_desk_Lookon_user移除；游戏开始时，写入m_user（位置与实际位置无关，按东南西北依次排）,m_switchPos位置记录转换关系  by wyz
	Lint			m_readyState[MAX_CHAIR_COUNT];	//玩家准备状态 ，用户准备的index是和m_seatUser的index是一致的，和m_user的不一定（考虑到后面每局可以新加入玩家游戏）by wyz
	Lint            m_videoPermit[MAX_CHAIR_COUNT]; //视频允许
	//Llong           m_timeInRoom[MAX_CHAIR_COUNT];  //加入房间时间
	Lint            m_playStatus[MAX_CHAIR_COUNT];  //玩家游戏状态:0：初始状态，1：该局参与游戏,2:该局为新加入，不参与游戏
	Lint			m_handlerPlayerStatus[MAX_CHAIR_COUNT];			//hanler里面更新该变量，玩家在牌局中的状态
	Lint			m_standUpPlayerSeatDown[MAX_CHAIR_COUNT];		//钵钵分够不够参与游戏，可以点击入座标记   0：正常玩家，1：中途钵钵分够参与游戏且点击入座的玩家，2：中途钵钵分不足玩家  3:钵钵分够参与游戏但是没点击入座的

	GameHandler*	mGameHandler;					//具体游戏处理器
	VipLogItem*		m_vip;							//录相
	//Card            m_specialCard[CARD_COUNT];		//存储玩家定义牌序
	Lint			m_reset[MAX_CHAIR_COUNT];		//玩家装态重置
	bool            m_dismissUser;

	// 倒计时运行时数据
	bool			m_autoPlay[MAX_CHAIR_COUNT];	//是否检查自动出牌
	Lint			m_autoPlayTime[MAX_CHAIR_COUNT];//自动出牌的倒计时
	Lint			m_autoOutTime[MAX_CHAIR_COUNT]; //倒计时的超时时间

	// 配置
	Lint			m_autoChangeOutTime;			//自动换牌的时间
	Lint			m_autoPlayOutTime;				//自动出牌超时时间 配置

	Lint			m_baseScore;					//金币场基底分 可以用于表示其他卡牌场的基础分 目前为1
	Lint            m_coinsResult[MAX_CHAIR_COUNT];	//玩家结算时的金币 增量

	Lint            m_desk_user_count;
	Lint            m_draw_user_count;              //开始一局的时候，参加游戏的用户数（在解散等时候，指需要参加游戏的人同意）
	Lint			m_flag;							//房间规则
	bool            m_selectSeat;                   //选座状态  创建房间时为true 选择完座位时为false
	Lint            m_zhuangPos;                    //庄位置
	Lint            m_cost;							//房间单人支付总费用
	Lint            m_even;							//玩家分摊金币数量
	Lint            m_free;
	Lint            m_startTime;
	Lint            m_endTime;
	Lint            m_Gps_Limit;					// 是否GPS限制房间

	Lint			m_finish_first_opt;				//结束动画首次操作标记
	Lint			m_next_round_delay_time;		//下一局超时时间
	
	//能量场特殊设置
	Lint			m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	Lint			m_qiangZhuangMinCoins;			// 金币场参与抢庄的最小能量值
	Lint			m_isAllowCoinsNegative;			// 每局结算时是否允许负数能量值出现
	Lint			m_warnScore;                    // 预警分数，如果能量值低于这个值，提醒用户

	//桌子属性
	Lint			m_isCoinsGame;					// 是否为金币场  0：普通场  1：金币场
	Lint			m_allowLookOn;					// 是否允许观战
	Lint			m_clubOwerLookOn;				// 俱乐部是否允许会长观战
	Lint			m_isAllowDynamicIn;				// 是否允许动态加入牌局  0: 不允许动态加入   1：允许动态加入
	Lint			m_isStartNoInRoom;				// 是否开局禁止加入房间  0：开局后仍可以加入房间   1：开局后不可加入房间
	Lint			m_isManagerStart;				// 是否为房主开始游戏 0：首先入座开始游戏  1：房主管理员开始游戏（人满也不自动开始）


	time_t          m_gameStartTime;				 //第一局遊戲開始時間
	time_t          m_gameEndTime;					 //最后一局   //没有用到，没有赋值

	bool m_bIsDissmissed;							 //桌子非正常解散
	//存储玩家定义牌序
	//Card            m_player1Card[13];
	//Card            m_player2Card[13];
	//Card            m_player3Card[13];
	//Card            m_player4Card[13];
	//Card            m_SendCard[84];
	//Card            m_HaoZiCard[2];
public:
	Lint            m_cheatAgainst;                 //防作弊
	Lint            m_feeType;                      //费用类型
	Lint            m_cellscore;                    //底分
	Lint            m_couFei;                       //是否已经扣费

private:
	Lint			m_id;							//桌子ID，（桌子对象初始化时候被赋值）
	Lint			m_iPlayerCapacity;				//游戏大玩法所对应的最大人数，桌子玩家容量不能超过MAX_CHAIR_COUNT

	DeskType		m_deskType;						//桌子类型  0：普通场， 1：金币场
	Lint            m_deskCreatedType;              //房间创建类型  0:自己建房， 1:房主建房

	////////////////////////////////
private:
	Lint             m_Greater2CanStart;             //>=2人选座就可以开始游戏
	//std::list<User*>  m_desk_all_user;             //房间总人数用户  //关于几个变量说明 by wyz   //  by wyz 不再使用这个变量，seat+lookon=all，维护及同步这个变量没有必要，容易出错
	Lint             m_switchPos[MAX_CHAIR_COUNT];   //m_user和m_seatUser位置转换关系
	Lint             m_StartGameButtonPos;           //开始游戏按钮位置用户，东南西北（0-3）
	Lint             m_startButtonAppear;
	std::list<User*> m_user_select_seat_turn;        // 选座用户时间排序，退出用户再进入选座排在后面，第一局开始游戏后清空变量

////////////////////////////////
private:
	//棋牌开局倒计时
	bool              m_flush_round_end_time;
	LTime             m_round_end_time;
	void              check_user_ready_before_game_begin(LTime &currTime);
	void              check_user_ready(LTime &currTime);
	void              record_round_end_time() { m_round_end_time.Now(); m_flush_round_end_time = true; }
	Lint              m_yingsanzhang_compare_count;
	Lint              m_yingsanzhang_finish_flag;

	LTime             m_sanzhangpai_gameStart_time_limit;
	bool                m_sanzhangpai_gameStart_time_flag;
	void                 record_sanzhangpai_game_start() {
		m_sanzhangpai_gameStart_time_limit.Now(); m_sanzhangpai_gameStart_time_flag = true;
	}


	//自动开局
	LTime             m_auto_start_game_time;           //自动开局时间
	Lint              m_auto_start_game_time_flag;		//自动开局计时标志
	void              check_auto_start_game(LTime &currTime);          //自动开始
	

public:
	void              set_some_spec(Lint state, const std::vector<Lint>& playtype);
	bool              m_next_draw_lookon;            //下一局可以旁观功能
	Lint			  m_next_delay_time;				//下一局因小结算等动画额外延迟时间

public:
	void              set_ysz_compare_count(Lint flag, Lint count);

	// 子游戏局数
	Lint                    getDrawType(Lint game_state, Lint drawCount);
	//再次设置用户人数--通过小选项
	void                  setDeskUserMax(Lint userMax) { m_iPlayerCapacity = userMax; m_desk_user_count = m_iPlayerCapacity; }

	//俱乐部部分
public:
	DeskClubInfo        m_clubInfo;                                         //俱乐部部分

//针对动态加入扣费
	Lint                          m_alreadyFee[MAX_CHAIR_COUNT];  //是否已扣费, 分摊房费的时候使用

// 把动态加入的用户转换成参与用户，慎用（推筒子-在一局结束时用）
public:
	void               setDynamicToPlay();

	////////////////////////////////
	bool  IsClubAdminUser(Lint userid)
	{
		bool bRet = false; 
		if (m_clubInfo.m_clubId == 0) return bRet;
		auto it = std::find(m_clubInfo.m_managerIdList.begin(), m_clubInfo.m_managerIdList.end(), userid);
		if (it < m_clubInfo.m_managerIdList.end())
		{
			LLOG_DEBUG("Desk: %d  user:%d is admins for club: %d", m_id, userid, m_clubInfo.m_clubId);
			bRet = true;
		}
		return bRet;
	}

	// 玩家是否是俱乐部管理员
	bool  IsClubAdminUser(User * pUser)
	{
		bool bRet = false;
		if (NULL == pUser) return bRet;
		if (m_clubInfo.m_clubId == 0) return bRet;
		bRet = IsClubAdminUser(pUser->GetUserDataId());
		return bRet;
	}

	bool  IsClubOwnerUser(User * pUser)
	{
		bool bRet = false;
		if (NULL == pUser) return bRet;
		if (m_clubInfo.m_clubId == 0) return bRet;
		return m_creatUserId == pUser->GetUserDataId();
	}

	//获取当前玩家的身份信息
	// 0:普通玩家 1: 俱乐部桌子管理员
    Lint GetClubDeskUserRole(User * pUser)
	{
		Lint role = 0;
		if (IsClubAdminUser(pUser))
		{
			role = 1;
		}
		else if (IsClubOwnerUser(pUser))
		{
			role = 1;
		}
		return role;
	}
};

typedef std::map<DWORD,Desk*> DeskMap;
typedef std::vector<Desk*> DeskVec;
#endif