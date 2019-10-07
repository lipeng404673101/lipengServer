#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LMsgPoker.h"
#include "LActive.h"
#include "LTime.h"

class Desk;

class  User
{
public:
	User(LUser data, Lint gateId);
	virtual ~User();

	//获取玩家数据库里 id
	Lint	GetUserDataId();

	const LUser& GetUserData() const { return m_userData; };

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	bool	GetOnline();
	void	SendLoginInfo(Lstring& buyInfo, Lint hide);
	void	SendItemInfo();

	bool	Login();
	void	Logout();

	void	Send(LMsg& msg);

	Desk*	GetDesk();
	void	SetDesk(Desk* desk);

	void	HanderMsg(LMsg* msg);
	//玩家创建房间
	Lint	HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg);

	Lint    HanderUserCreateDeskForOther(LMsgLMG2LCreateDeskForOther* msg);

	//玩家进入房间
	Lint	HanderUserAddDesk(Lint nDeskID);


	//玩家进入房间
	Lint	HanderUserAddDesk(LMsgLMG2LAddToDesk* msg);

	//玩家退出房间
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg);

//俱乐部
	Lint	HanderUserAddClubDesk(LMsgLMG2LEnterClubDesk* msg);

	//玩家请求出牌
	void	HanderUserPlayCard(LMsgC2SUserPlay* msg);

	//玩家请求思考
	void	HanderUserThink(LMsgC2SUserOper* msg);

	//玩家选择起手胡
	void	HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg);

	//玩家选择海底牌
	void	HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg);

	//玩家请求通知内容
	void	HanderUserReady(LMsgC2SUserReady* msg);

	//玩家选择座位
	void	HanderUserSelectSeat(LMsgC2SUserSelectSeat* msg);

	//处理观战玩家入座
	void    HanderLookOnUserSeatDown(LMsgG2LLookOnUserSearDown* msg);

	//玩家开始按钮
	void    HanderUserStartGameButton(LMsgC2SStartGame*msg);

	//申请解算房间
	void	HanderResutDesk(LMsgC2SResetDesk* msg);

	//玩家选择解算操作
	void	HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg);

	//玩家请求聊天
	void	HanderUserSpeak(LMsgC2SUserSpeak* msg);

	// 玩家换牌
	void HanderUserChange(LMsgC2SUserChange* msg);

	void HanderUserTangReq(LMsgC2STangCard* msg);

	void HanderUserAIOper(LMsgC2SUserAIOper* msg);

	void HanderSendVideoInvitation(LMsgC2SSendVideoInvitation* msg);

	void HanderInbusyVideoInvitation(LMsgC2SReceiveVideoInvitation* msg);

	void HanderOnlineVideoInvitation(LMsgC2SOnlineVideoInvitation* msg);

	void HanderShutDownVideoInvitation(LMsgC2SShutDownVideoInvitation* msg);

	void HanderUploadGpsInformation(LMsgC2SUploadGPSInformation* msg);

	void HanderUploadVideoPermission(LMsgC2SUploadVideoPermission* msg);

	//更新玩家金币数量
	void HanderUpdateUserCoins(LMsgLMG2LUpdatePointCoins* msg);

	//能量不足玩家充上能量后从新点击入座 61184
	void HanderStandPlayerReSeat(LMsgC2SStandPlayerReSeat* msg);

	//玩家请求观战玩家列表信息
	void HanderViewLookOnList(LMsgC2SViewLookOnList* msg);

	//处理扑克游戏的接口
	void HanderPokerGameMessage(LMsgC2SGameCommon *msg);
	void HanderYingSanZhangAddScore(YingSanZhangC2SAddScore*msg);
	void HanderYingSanZhangUserSelectBigSmall(YingSanZhangC2SUserSelectBigSmall* msg);
	////推筒子
	void  HanderTuiTongZiAddScore( TuiTongZiC2SScore*msg);
	void  HanderTuiTongZiSelectZhuang(TuiTongZiC2SSelectZhuang*msg);
	void  HanderTuiTongZiDoShaiZi( TuiTongZiC2SDoShaiZi*msg);
	void  HanderTuiTongOpenCard( TuiTongZiC2SOpenCard*msg);

	//牛牛
	void HanderNiuNiuSelectZhuang(NiuNiuC2SSelectZhuang* msg);
	void HanderNiuNiuMaiMa(NiuNiuC2SMaiMa* msg);
	void HanderNiuNiuAddScore(NiuNiuC2SAddScore* msg);
	void HanderNiuNiuOpenCard(NiuNiuC2SOpenCard* msg);

	/*
	牛牛：玩家搓公牌
	MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
	*/
	void HandNiuNiuCuoGongPai(NiuNiuC2SCuoGongPai* msg);

	/*
	牛牛：玩家选择托管消息
	MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
	*/
	void HandNiuNiuTuoGuan(NiuNiuC2STuoGuan* msg);

	/*
	牛牛：玩家取消托管消息
	MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
	*/
	void HandNiuNiuCancelTuoGuan(NiuNiuC2SCancelTuoGuan* msg);

	/*
	客户端通知服务器开始计时
	MSG_C_2_S_START_TIME = 61198
	*/
	void HandNiuNiuStartTime(NiuNiuC2SStartTime* msg);


	//斗地主
	void HanderDouDiZhuGameMessage(MHLMsgDouDiZhuC2SGameMessage * msg);

	/*
	双升：C->S 玩家报主操作
	MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202
	*/
	void HandleShuangShengSelectZhu(ShuangShengC2SSelectZhu* msg);

	/*
	双升：C->S 玩家反主操作
	MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204
	*/
	void HandleShangShengFanZhu(ShuangShengC2SFanZhu* msg);

	/*
	双升：C->S 玩家盖底牌操作
	MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206
	*/
	void HandleShuangShengBaseCards(ShuangShengC2SBaseCards* msg);

	/*
	双升：C->S 玩家出牌操作
	MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208
	*/
	void HandleShuangShengOutCards(ShuangShengC2SOutCards* msg);

	/*
	3打2：C->S 玩家选择叫牌分
	MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
	*/
	void HandleSanDaErSelectScore(SanDaErC2SSelectScore* msg);

	/*
	3打2：C->S 玩家选主牌
	MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
	*/
	void HandleSanDaErSelectMain(SanDaErC2SSelectMain* msg);

	/*
	3打2：C->S 玩家埋底
	MSG_C_2_S_SANDAER_BASE_CARD = 62257
	*/
	void HandleSanDaErBaseCard(SanDaErC2SBaseCard* msg);

	/*
	3打2：C->S 玩家选对家牌
	MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
	*/
	void HandleSanDaErSelectFriend(SanDaErC2SSelectFriend* msg);

	/*
	3打2：C->S 玩家出牌
	MSG_C_2_S_SANDAER_OUT_CARD = 62261
	*/
	void HandleSanDaErOutCard(SanDaErC2SOutCard* msg);

	/*
	3打2：C->S 玩家回复闲家得分慢105分,是否停止游戏
	MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
	*/
	void HandleSanDaErScore105Ret(SanDaErC2SScore105Ret* msg);

	/*
	3打2：C->S 庄家请求底牌
	MSG_C_2_S_USER_GET_BASECARDS = 61027,		//
	*/
	void HandleSanDaErGetBaseCards(SanDaErC2SGetBaseCards* msg);

	/*
	3打2：C->S 客户端庄家选择交牌
	MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272
	*/
	void HandleSanDaErZhuangRequestJiaoPai(SanDaErC2SZhuangJiaoPai* msg);

	/*
	C->S 非庄家  客户端选择庄家交牌的申请
	MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274
	*/
	void HandleSanDaErXianSelectJiaoPai(SanDaErC2SXianSelectJiaoPai* msg);

	/*
	3打1：C->S 玩家选择叫牌分
	MSG_C_2_S_SANDAYI_SELECT_SCORE = 62253
	*/
	void HandleSanDaYiSelectScore(SanDaYiC2SSelectScore* msg);

	/*
	3打1：C->S 玩家选主牌
	MSG_C_2_S_SANDAYI_SELECT_MAIN = 62255
	*/
	void HandleSanDaYiSelectMain(SanDaYiC2SSelectMain* msg);

	/*
	3打1：C->S 玩家埋底
	MSG_C_2_S_SANDAYI_BASE_CARD = 62257
	*/
	void HandleSanDaYiBaseCard(SanDaYiC2SBaseCard* msg);

	/*
	3打1：C->S 玩家出牌
	MSG_C_2_S_SANDAYI_OUT_CARD = 62261
	*/
	void HandleSanDaYiOutCard(SanDaYiC2SOutCard* msg);

	/*
	3打1：C->S 玩家回复闲家得分慢105分,是否停止游戏
	MSG_C_2_S_SANDAYI_SCORE_105_RET = 62266,
	*/
	void HandleSanDaYiScore105Ret(SanDaYiC2SScore105Ret* msg);

	/*
	五人百分：C->S 玩家选择叫牌分
	MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
	*/
	void HandleWuRenBaiFenSelectScore(WuRenBaiFenC2SSelectScore* msg);

	/*
	五人百分：C->S 玩家选主牌
	MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
	*/
	void HandleWuRenBaiFenSelectMain(WuRenBaiFenC2SSelectMain* msg);

	/*
	五人百分：C->S 玩家埋底
	MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
	*/
	void HandleWuRenBaiFenBaseCard(WuRenBaiFenC2SBaseCard* msg);

	/*
	五人百分：C->S 玩家选对家牌
	MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
	*/
	void HandleWuRenBaiFenSelectFriend(WuRenBaiFenC2SSelectFriend* msg);

	/*
	五人百分：C->S 玩家出牌
	MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
	*/
	void HandleWuRenBaiFenOutCard(WuRenBaiFenC2SOutCard* msg);

	/*
	五人百分：C->S 玩家回复闲家得分慢105分,是否停止游戏
	MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,
	*/
	void HandleWuRenBaiFenScore105Ret(WuRenBaiFenC2SScore105Ret* msg);
	

	/*
	五人百分：交牌	 
	*/
	void HandleWuRenBaiFenZhuangJiaoPai(WuRenBaiFenC2SZhuangJiaoPai * msg);
	void HandleWuRenBaiFenXianAckJiaoPai(WuRenBaiFenC2SXianSelectJiaoPai * msg);


	/*
	扯炫：C->S 处理玩家起钵钵
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
	*/
	void HandlerPlayerSelectBoBo(CheXuanC2SPlayerSelectBoBo* msg);

	/*
	扯炫：C->S 处理玩家选择操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
	*/
	void HandlerPlayerSelectOpt(CheXuanC2SPlayerSelectOpt* msg);

	/*
	扯炫：C->S 处理玩家扯牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
	*/
	void HandlerPlayerChePai(CheXuanC2SPlayerChePai* msg);

	/*
	C->S 玩家搓牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
	*/
	void HandlerPlayerCuoPai(CheXuanC2SPlayerCuoPai* msg);

	/*
	C->S 客户端牌局内请求战绩
	MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
	*/
	void HandlerPlayerRequestRecord(CheXuanC2SPlayerRequestRecord* msg);

	/*
	C->S 客户端操作默分
	MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
	*/
	void HandlerPlayerOptMoFen(CheXuanC2SPlayerOptMoFen* msg);

	/*
	C->S 客户端选择自动操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
	*/
	void HandlerPlayerSelectAutoOpt(CheXuanC2SPlayerSelectAutoOpt* msg);

	/*
	C->S 客户端选择站起或坐下
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
	*/
	void HandlerPlayerStandOrSeat(CheXuanC2SPlayerSelectStandOrSeat* msg);

	
	//房间里申请用户gps
	void HanderUserRequestGPS(LMsgC2SRequestUserGps* msg);

	Lstring GetIp();

	void	SetIp(Lstring& ip);

	//增加房卡
	void	AddCardCount(Lint cardType, Lint count,Lint operType,Lstring admin, bool bNeedSave = true);

	static void    AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType,Lstring admin);
	//删除房卡
	void	DelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin,Lint feeTye);

	//改变玩家状态
	void	ModifyUserState(bool bLeaveServer = false);
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
  inline void    updateInRoomTime() { m_timeInRoom = LTime(); }
	inline bool   IsInRoomFor15Second() { return  m_timeInRoom.Secs() > 0 && (LTime().Secs() - m_timeInRoom.Secs()) > 15 ; }
	inline void   ClearInRoomTime() { m_timeInRoom.SetSecs(0); }
	bool IsInLimitGPSPosition(User & u);	
public:
	LUser	m_userData;
	Lint	m_gateId;		//玩家在哪个gate上面
	Lint	m_userState;	//玩家当前状态
	Lint    m_videoPermission; //允许video状态
	bool	m_online;
	std::vector< LActive >	m_Active;
	Desk*	m_desk;
	Lstring	m_ip;
	LTime   m_timeInRoom;   //进入房间时间
	double   m_gps_lng;      //GPS经度
	double   m_gps_lat;      //GPS纬度
	Lint	m_isMedal;		 //玩家是否有勇士勋章
};

#endif