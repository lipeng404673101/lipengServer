#include "mhmsghead.h"

void MHInitFactoryMessage()
{
	//170 Login
	LMsgFactory::Instance().RegestMsg(new LMsgFromLoginServer());
	LMsgFactory::Instance().RegestMsg(new LMsgG2LMGLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeGateInfo());
	LMsgFactory::Instance().RegestMsg(new MHLMsgCe2LUpdateGateIp());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserSelectSeat());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserSelectSeat());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SCreateDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SCreateDeskForOther());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LCreateDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LCreateDeskForOther());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SDeskList());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CDeskList());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGModifyUserCreatedDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGDeleteUserCreatedDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LCreatedDeskList());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CResetDesk());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SCreaterResetRoom());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CCreaterResetRoom());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LDissmissCreatedDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CIntoDesk());  

	LMsgFactory::Instance().RegestMsg(new LMsgS2CDeskState()); 
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeUpdateCoin());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LUpdateCoin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeUpdateCoinJoinDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LUpdateCoinJoinDesk());  
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeUserServerLogin());  	                                   
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeUserServerLogout()); 
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LUserIdInfo());  
	LMsgFactory::Instance().RegestMsg(new LMsgNewUserVerifyRequest());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CBcastMessage());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LMBCast());   
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LMGConfig());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SGetShareVideo());
	LMsgFactory::Instance().RegestMsg(new MHLMsgL2LDBQueryShareVideoByShareId());
	LMsgFactory::Instance().RegestMsg(new MHLMsgLDB2LShareVideoResult());

	//Logic server to manager
	LMsgFactory::Instance().RegestMsg(new LMsgC2SSendVideoInvitation());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SReceiveVideoInvitation());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SInbusyVideoInvitation());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SOnlineVideoInvitation());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SShutDownVideoInvitation());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SUploadGPSInformation());	 
	LMsgFactory::Instance().RegestMsg(new LMsgC2SUploadVideoPermission());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUpdateUserVipInfo());  
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGUpdateUserGPS());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CEUpdateUserGPS());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CVipEnd());
	//LMsgFactory::Instance().RegestMsg(new LMsgL2GUserMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserOnlineInfo());  
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserPlay());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CDeskDelUser());  
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LAddToDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SStartGame());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LAddToDesk());  
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGModifyCard());
	LMsgFactory::Instance().RegestMsg(new MHLMsgL2LMGNewAgencyActivityUpdatePlayCount());

	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBReqRoomLog());















	//特殊活动
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LSpecActive());


	//club center to manager
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LClubInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LAddClub());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGClubAddPlayType());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGClubAddUser());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGClubDelUser());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGClubHidePlayType());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGClubModifyClubName());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGClubModifyClubFee());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CEUpdateData());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGDismissDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LMGClubInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LMGPlayInfo());

	
	//发生控制信息
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGControlMsg());

	//manager to center
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CeSqlInfo());


	//club logic to manager
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGFreshDeskInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGUserAddClubDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGUserLeaveClubDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGUserListLeaveClubDesk());


	////mananger 重启， logic ,gate重连后发送数据
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGSynchroData());
	LMsgFactory::Instance().RegestMsg(new LMsgG2LMGSynchroData());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LGRequestSynchroData());

	//开局红包
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeClubFirstRedPacket());
	//能量记录
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CEUpdatePointRecord());
	//manager更新logic上玩家的能量值
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LUpdatePointCoins());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CERecordLog());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LMGRecordLog());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LUpdateManagerList());

	//club  logicmanager to logic 
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LCreateDeskForClub());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LCreateClubDeskAndEnter());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LEnterClubDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LDissmissClubDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LAPIDissmissRoom());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SQuickJoinClubDesk());
	
	//club  client  and  server
	LMsgFactory::Instance().RegestMsg(new LMsgC2SEnterClub());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SLeaveClub());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CLeaveClub());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SSwitchPlayScene());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SRequestPersonInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CClubScene());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CSwitchClubScene());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGRecyleDeskID());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CFreshClubDeskInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CClubDeskInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SClubMasterResetRoom());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CClubMasterResetRoom());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SJoinRoomCheck());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CJoinRoomCheck());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SClubUserCreateRoom());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SApplyJoinClub());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CApplyJoinClub());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SCreateClub());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CCreateClub());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SUpdateClub());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUpdateClub());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SUpdatePlay());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUpdatePlay());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SLockClubDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CLockClubDesk());


	//GPS限制房间信息
	LMsgFactory::Instance().RegestMsg(new MHLMsgC2SQueryRoomGPSLimitInfo());
	LMsgFactory::Instance().RegestMsg(new MHLMsgS2CQueryRoomGPSLimitInfoRet());
	LMsgFactory::Instance().RegestMsg(new MHLMsgLMG2LQueryRoomGPSInfo());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SGPSInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CGPSInfo());


	LMsgFactory::Instance().RegestMsg(new LMsgCe2LMGDelUserFromManager());

	//俱乐部新加功能
	///////////////////////////////////////////////////////////////////////
	LMsgFactory::Instance().RegestMsg(new LMsgC2SRequestClubMemberList());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CClubMemberList());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SRequestClubOnLineCount());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CClubOnlineCount());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CBcast2ClubUser());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CNotifyUserJoinClub());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SRequestClubMemberOnlineNogame());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CClubOnLineMemberList());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SInviteClubMember());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CInviteClubMemberReply());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CTransmitInviteClubMember());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SReplyClubMemberInvite());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CReplyClubMemberInviteReply());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CNotifyClubOwnerMember());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CNotifyUserLeaveClub());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SSetClubAdministrtor());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CSetClubAdministrtor());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SRequestDeskState());

	//能量值的消息
	LMsgFactory::Instance().RegestMsg(new LMsgC2SPowerPointList());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CPowerPointList());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SRequestPointRecord());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CReplyPointRecord());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SChangePoint());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CChangePoint());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CWarnPointLow());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CStandPlayerShowSeat());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SStandPlayerReSeat());




	//poker 添加信息
	///////////////////////////////////////////////////////////////////////
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserDeskCommand());

	LMsgFactory::Instance().RegestMsg(new YingSanZhangC2SAddScore());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SGameCommon());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CGameMessage());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CGameEndResult());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SQiPaiLog());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CQiPaiLog());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SMatchLog());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CMatchLog());

	//LMsgFactory::Instance().RegestMsg(new LMsgS2CMatchLog());

	LMsgFactory::Instance().RegestMsg(new LMsgLBD2LReqPokerLog());

	//三张牌
	LMsgFactory::Instance().RegestMsg(new YingSanZhangC2SUserSelectBigSmall());
	LMsgFactory::Instance().RegestMsg(new YingSanZhangS2CUserSelectBigSmall());

	LMsgFactory::Instance().RegestMsg(new YingSanZhangS2CChangeBigSmallMode());

	LMsgFactory::Instance().RegestMsg(new YingSanZhangS2CStartAddScore());


	//推筒子
	LMsgFactory::Instance().RegestMsg(new TuiTongZiS2CStartGame());

	LMsgFactory::Instance().RegestMsg(new TuiTongZiC2SSelectZhuang());
	LMsgFactory::Instance().RegestMsg(new TuiTongZiS2CSelectZhuang());

	LMsgFactory::Instance().RegestMsg(new TuiTongZiC2SDoShaiZi());
	LMsgFactory::Instance().RegestMsg(new TuiTongZiS2CDoShaiZi());

	LMsgFactory::Instance().RegestMsg(new TuiTongZiC2SScore());
	LMsgFactory::Instance().RegestMsg(new TuiTongZiS2CScore());

	LMsgFactory::Instance().RegestMsg(new TuiTongZiC2SOpenCard());
	LMsgFactory::Instance().RegestMsg(new TuiTongZiS2COpenCard());
	
	LMsgFactory::Instance().RegestMsg(new TuiTongZiS2CRecon());
	LMsgFactory::Instance().RegestMsg(new TuiTongZiS2CResult());

	LMsgFactory::Instance().RegestMsg(new ReadyRemainTime());

	//牛牛消息注册
	///////////////////////////////////////////////////////////////////////
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CStartGame());

	LMsgFactory::Instance().RegestMsg(new NiuNiuC2SSelectZhuang());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CSelectZhuang());

	LMsgFactory::Instance().RegestMsg(new NiuNiuC2SAddScore());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CAddScore());

	LMsgFactory::Instance().RegestMsg(new NiuNiuC2SOpenCard());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2COpenCard());

	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CResult());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CRecon());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CQiangZhuangNotify());

	LMsgFactory::Instance().RegestMsg(new NiuNiuC2SMaiMa());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CMaiMaBC());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CMingPaiAddScore());
	LMsgFactory::Instance().RegestMsg(new NiuNiuC2STuoGuan());
	LMsgFactory::Instance().RegestMsg(new NiuNiuC2SCancelTuoGuan());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CTuoGuanBC());
	LMsgFactory::Instance().RegestMsg(new NiuNiuC2SCuoGongPai());
	LMsgFactory::Instance().RegestMsg(new NiuNiuS2CCuoGongPaiBC());
	LMsgFactory::Instance().RegestMsg(new NiuNiuC2SStartTime());


	//牛牛入座功能添加
	LMsgFactory::Instance().RegestMsg(new LMsgG2LLookOnUserSearDown());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CLookOnUserSeatDownRet());

	//入座观战功能添加
	LMsgFactory::Instance().RegestMsg(new LMsgC2SViewLookOnList());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CRectLookOnList());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CShowLookOnCount());

	//更新玩家的金币值
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUpdateCoins());

	// 斗地主游戏消息
	LMsgFactory::Instance().RegestMsg(new MHLMsgDouDiZhuS2CGameMessage());
	LMsgFactory::Instance().RegestMsg(new MHLMsgDouDiZhuC2SGameMessage());
	LMsgFactory::Instance().RegestMsg(new MHLMsgDouDiZhuS2CReconn());
	LMsgFactory::Instance().RegestMsg(new MHLMsgC2SJoinRoomAnonymous());

	//斗地主回放
	LMsgFactory::Instance().RegestMsg(new LMsgC2SPokerVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CPokerVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LPokerReqVideo());

	//斗地主3v3
	LMsgFactory::Instance().RegestMsg(new LMsgS2CWaitJoinRoom());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SPokerExitWait());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CPokerExitWait());

	//请求房间其他用户gps
	LMsgFactory::Instance().RegestMsg(new LMsgC2SRequestUserGps());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CRequestUserGps());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CLookOnPlayerFill());

	//双升消息注册
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CFirstRound());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CSendCards());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CNoticeSelectZhu());
	LMsgFactory::Instance().RegestMsg(new ShuangShengC2SSelectZhu());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CBCSelectZhu());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CNoticeFanZhu());
	LMsgFactory::Instance().RegestMsg(new ShuangShengC2SFanZhu());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CNoticeBaseInfo());
	LMsgFactory::Instance().RegestMsg(new ShuangShengC2SBaseCards());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CBCBaseCardOver());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CBCGameStart());
	LMsgFactory::Instance().RegestMsg(new ShuangShengC2SOutCards());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CBCOutCardsResult());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CBCDrawResult());
	LMsgFactory::Instance().RegestMsg(new ShuangShengS2CRecon());

	//三打二消息注册
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CSendCards());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CSelecScoreR());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SSelectScore());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CBaseCardsAndSelectMainN());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SSelectMain());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CSelectMainColorR());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SBaseCard());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CBaseCardR());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SSelectFriend());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CSelectFriendCardBC());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SOutCard());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2COutCardBC());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CReConnect());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CDrawResult());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CScore105());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SScore105Ret());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CScore105Result());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SGetBaseCards());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CGetBaseCardsRet());


	//手机登录消息
	LMsgFactory::Instance().RegestMsg(new LMsgC2SPhoneLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CPhoneLogin1());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CPhoneLogin2());


	LMsgFactory::Instance().RegestMsg(new SanDaErS2CZhuangCanTanPai());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SZhuangSelectTanPai());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CTanPaiRet());

	LMsgFactory::Instance().RegestMsg(new SanDaErC2SZhuangJiaoPai());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CZhuangJiaoPaiBC());
	LMsgFactory::Instance().RegestMsg(new SanDaErC2SXianSelectJiaoPai());
	LMsgFactory::Instance().RegestMsg(new SanDaErS2CJiaoPaiResult());


	//三打一消息注册
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CSendCards());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CSelecScoreR());
	LMsgFactory::Instance().RegestMsg(new SanDaYiC2SSelectScore());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CBaseCardsAndSelectMainN());
	LMsgFactory::Instance().RegestMsg(new SanDaYiC2SSelectMain());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CSelectMainColorR());
	LMsgFactory::Instance().RegestMsg(new SanDaYiC2SBaseCard());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CBaseCardR());
	LMsgFactory::Instance().RegestMsg(new SanDaYiC2SOutCard());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2COutCardBC());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CReConnect());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CDrawResult());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CScore105());
	LMsgFactory::Instance().RegestMsg(new SanDaYiC2SScore105Ret());
	LMsgFactory::Instance().RegestMsg(new SanDaYiS2CScore105Result());

	//五人百分消息注册
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CSendCards());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CSelecScoreR());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SSelectScore());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CBaseCardsAndSelectMainN());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SSelectMain());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CSelectMainColorR());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SBaseCard());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CBaseCardR());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SSelectFriend());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CSelectFriendCardBC());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SOutCard());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2COutCardBC());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CReConnect());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CDrawResult());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CScore105());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SScore105Ret());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CScore105Result());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SZhuangJiaoPai());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CZhuangJiaoPaiBC());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenC2SXianSelectJiaoPai());
	LMsgFactory::Instance().RegestMsg(new WuRenBaiFenS2CJiaoPaiResult());

	//扯炫消息注册
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CNotifyPlayerQiBoBo());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerSelectBoBo());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerSelectBoBoBC());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CNotifyPlayerAddMangGuo());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerAddMangGuo());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CNotifyPlayerAddMangGuoBC());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CSendCard());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CNotifyPlayerOpt());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerSelectOpt());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerOptBC());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CNotifyChePai());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerChePai());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerChePaiBC());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CReConnect());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CNotifyCuoPai());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerCuoPai());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerCuoPaiBC());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerRequestRecord());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerSendRecord());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerOptMoFen());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerOptMoFen());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerSelectAutoOpt());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerSelectAutoOptRet());
	LMsgFactory::Instance().RegestMsg(new CheXuanC2SPlayerSelectStandOrSeat());
	LMsgFactory::Instance().RegestMsg(new CheXuanS2CPlayerSelectStandOrSeatRet());

}

void MH_InitFactoryMessage_ForMapMessage()
{
	LMsgFactory::Instance().RegestMsg(new LMsgIn());
	LMsgFactory::Instance().RegestMsg(new LMsgKick());
	LMsgFactory::Instance().RegestMsg(new LMsgHttp());
	LMsgFactory::Instance().RegestMsg(new LMsgKillClient());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CMsg());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CKillClient());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SHeart());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CHeart());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SLoginGate());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CLoginGate());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SNotice());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CNotice());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeUserServerLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeUserServerLogout());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CItemInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CHorseInfo());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CIntoDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CDeskAddUser());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CDeskDelUser());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SSelectResetDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CResetDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SResetDesk());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CDeskState());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserReady());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserReady());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CPlayStart());
	//LMsgFactory::Instance().RegestMsg(new LMsgS2COutCard());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserPlay());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserPlay());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CThink());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserOper());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserOper());
	//LMsgFactory::Instance().RegestMsg(new LMsgS2CGameOver());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserAIOper());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserAIOper());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserStartHu());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserStartHuSelect());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserStartHuSelect());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserGangCard());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserEndCardSelect());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserEndCardSelect());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserEndCardGet());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserOutRoom());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserOutRoom());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SLeaveDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CLeaveDesk());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserSpeak());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserSpeak());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SCreateDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CCreateDeskRet());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SAddDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CAddDeskRet());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SEnterCoinsDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CEnterCoinsDeskRet());


	LMsgFactory::Instance().RegestMsg(new LMsgS2CVipEnd());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SVipLog());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CVipLog());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CRemoveQiangGangCard());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SCRELog());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CCRELogRet());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SREQ_GTU());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CREQ_GTU_RET());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SREQ_Exch());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CREQ_EXCH_RET());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CActivityInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SActivityPhone());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CActivityPhone());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SActivityRequestLog());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CActivityReplyLog());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SActivityRequestDrawOpen());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CActivityReplyDrawOpen());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SActivityRequestDraw());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CActivityReplyDraw());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SActivityRequestShare());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CActivityFinishShare());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SRoomLog());
	LMsgFactory::Instance().RegestMsg(new LMsgLBD2LReqRoomLog());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CRoomLog());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CInvitingInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SBindInviter());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CBindInviter());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SGetUserInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SGetInvitingInfo());


	LMsgFactory::Instance().RegestMsg(new LMsgL2CeLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeGateInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LUserLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LUserIdInfo());

	LMsgFactory::Instance().RegestMsg(new LMsgG2LLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgG2LUserMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgL2GUserMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeModifyUserNew());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CeSaveLogItem());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LGMCharge());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LGMHorse());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LGMBuyInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LGMCoins());
	LMsgFactory::Instance().RegestMsg(new LMsgCe2LGMHide());

	LMsgFactory::Instance().RegestMsg(new LMsgG2LUserOutMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgL2GUserOutMsg());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SVerifyHead());

	LMsgFactory::Instance().RegestMsg(new LMsgL2LdbLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBSaveLogItem());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBSaveVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LBDReqVipLog());
	LMsgFactory::Instance().RegestMsg(new LMsgLBD2LReqVipLog());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LReqVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBReqVideo());


	LMsgFactory::Instance().RegestMsg(new LMsgL2LBDReqCRELog());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBSaveCRELog());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LM_RecordCRELog());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBDEL_GTU());

	LMsgFactory::Instance().RegestMsg(new LMsgLM2CEN_ADD_CRE());

	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LLogInfo());

	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserChange());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SUserChange());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CUserChangeResult());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CGameResult());
	LMsgFactory::Instance().RegestMsg(new LMsgC2STangCard());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CTangCardRet());


	LMsgFactory::Instance().RegestMsg(new LMsgLMG2GateLogicInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2GHorseInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGModifyUserState());
	LMsgFactory::Instance().RegestMsg(new LMsgL2GModifyUserState());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LCreateDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LAddToDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGModifyCard());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGAddUserPlayCount());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LPXActive());
	LMsgFactory::Instance().RegestMsg(new LMsgG2LMGLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGRecyleDeskID());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LdbUserLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LMGUserMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2LdbLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LSetGameFree());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LSetPXActive());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LSetOUGCActive());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LSetExchActive());
	LMsgFactory::Instance().RegestMsg(new LMsgCE2LSetActivity());

	LMsgFactory::Instance().RegestMsg(new LMsgLM_2_LDBBindInviter());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LMGBindingRelationships());
	LMsgFactory::Instance().RegestMsg(new LMsgLM_2_LDB_ReqInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB_2_LM_RetInfo());

	LMsgFactory::Instance().RegestMsg(new LMsgFromLoginServer());
	LMsgFactory::Instance().RegestMsg(new LMsgToLoginServer());

	LMsgFactory::Instance().RegestMsg(new LMsgConvertClientMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgHeartBeatRequestMsg());
	LMsgFactory::Instance().RegestMsg(new LMsgHeartBeatReplyMsg());

	LMsgFactory::Instance().RegestMsg(new LMsgNewUserVerifyRequest());

	LMsgFactory::Instance().RegestMsg(new LMsgCN2LMGLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CNLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgCN2LMGFreeDeskReq());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CNFreeDeskReply());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CNUserLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CNEnterCoinDesk());
	LMsgFactory::Instance().RegestMsg(new LMsgCN2LMGModifyUserCoins());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LMGExchCard());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2CNGetCoins());
	LMsgFactory::Instance().RegestMsg(new LMsgLMG2GLCoinsServerInfo());
	LMsgFactory::Instance().RegestMsg(new LMsgG2CNLogin());
	LMsgFactory::Instance().RegestMsg(new LMsgL2CNLogin());
	//LMsgFactory::Instance().RegestMsg(new LMsgCN2LCreateCoinDesk());
	//LMsgFactory::Instance().RegestMsg(new LMsgL2CNCreateCoinDeskRet());
	//LMsgFactory::Instance().RegestMsg(new LMsgL2CNEndCoinDesk());
	//LMsgFactory::Instance().RegestMsg(new LMsgC2SGoOnCoinsDesk());
	//LMsgFactory::Instance().RegestMsg(new LMsgS2CGoOnCoinsDeskRet());
	//LMsgFactory::Instance().RegestMsg(new LMsgS2CKickCoinsDesk());
	//LMsgFactory::Instance().RegestMsg(new LMsgS2CGiveCoins());
	//LMsgFactory::Instance().RegestMsg(new LMsgC2SGetCoins());
	//LMsgFactory::Instance().RegestMsg(new LMsgS2CGetCoins());
	//LMsgFactory::Instance().RegestMsg(new LMsgS2COtherCoins());

	LMsgFactory::Instance().RegestMsg(new LMsgC2SShareVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CShareVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SReqShareVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgC2SGetShareVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgS2CRetShareVideoId());

	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBGGetShareVideoId());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBGetSsharedVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBGetShareVideo());
	LMsgFactory::Instance().RegestMsg(new LMsgL2LDBSaveShareVideoId());

	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LRetShareVideoId());
	LMsgFactory::Instance().RegestMsg(new LMsgLDB2LRetShareVideo());

}

double convert_string2float(const Lstring &strValue)
{
	return atof(strValue.c_str());
}

void get_gps_pair_values(const Lstring & strGPS, double& value1, double& value2)
{
	size_t pos = strGPS.find_first_of(',');
	if (pos != std::string::npos)
	{
		value1 = convert_string2float(strGPS.substr(0, pos));
		value2 = convert_string2float(strGPS.substr(pos + 1, strGPS.length() - pos - 1));	 
	}
}

static double Deg2Rad(double x) 
{ 
	return (x *  3.14159265358979323846 / 180.0);
}

/////////////////////////////////根据GPS坐标计算GPS距离,单位米
double calc_gps_distance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1 = Deg2Rad(lat1);
	double radLat2 = Deg2Rad(lat2);
	double a = radLat1 - radLat2;
	double b = Deg2Rad(lng1) - Deg2Rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
	s = s * 6378.137;
	s = round(s * 10000) / 10000;
	return (s * 1000.0);
}

Lstring string_replace(Lstring &src, Lstring &replace, Lstring &value)
{
	Lstring str = src;
	while (true)
	{
		Lstring::size_type pos(0);
		if ((pos = str.find(replace)) != Lstring::npos)
		{
			str.replace(pos, replace.length(), value);
		}
		else
		{
			break;
		}
	}
	return str;
}

