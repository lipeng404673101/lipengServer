#ifndef _L_MSG_CLUB_H_
#define _L_MSG_CLUB_H_

#include "LMsgBase.h"


//服务器端跟客户端俱乐部通讯的消息号
enum LMSG_CLUB_ID
{
	//////////////////////begin  61130 --61200关于俱乐部, client--><--server   
	MSG_C_2_S_ENTER_CLUB = 61130, //301,      // 进入俱乐部
	MSG_S_2_C_CLUB_SCENE = 61131, // 302,      //俱乐部界面内容

	MSG_C_2_S_LEAVE_CLUB = 61132, // 303,     //离开俱乐部   
	MSG_S_2_C_LEAVE_CLUB = 61133, // 304,      //离开俱乐部

	MSG_C_2_S_SWITCH_PLAY_SCENE = 61134,//  305,   //俱乐部界面切换玩法
	MSG_S_2_C_SWITCH_PLAY_SCENE = 61135, // 306,     //返回俱乐部界面信息

	MSG_C_2_S_REQUEST_PERSON_INFO = 61136, // 307,  //获取个人信息
	MSG_S_2_C_REPLY_PERSON_INFO = 61137, // 308,   //获取个人信息

	MSG_C_2_S_FRESH_CLUB_DESK_INFO = 61138, //309,   //刷新俱乐部桌子信息

	MSG_S_2_C_CLUB_DESK_INFO = 61139,    //俱乐部桌子信息

	//////////////////////////////////
	MSG_C_2_S_REQUEST_CLUB_MEMBER = 61140,   //客户端请求俱乐部成员
	MSG_S_2_C_CLUB_MEMBER_INFO = 61141,        //服务器返回俱乐部成员信息

	MSG_C_2_S_REQUEST_CLUB_MEMBER_ONLINE_NOGAME = 61142,  //用户在房间中请求亲友圈可以游戏的用户列表
	MSG_S_2_C_CLUB_MEMBER_ONLINE_NOGAME = 61143,                   //服务器返回内容

	MSG_C_2_S_INVITE_CLUB_MENBER = 61144,    //邀请俱乐部成员参加游戏
	MSG_S_2_C_INVITE_CLUB_MEMBER_REPLY = 61145,   //返回邀请结果，只有错误状态才返回

	MSG_S_2_C_TRANSMIT_INVITE_CLUB_MENBER = 61146,  //转发用户邀请
	MSG_C_2_S_REPLY_CLUB_MEMBER_INVITE = 61147,        //回复俱乐部成员的邀请
	MSG_S_2_C_REPLY_CLUB_MEMBER_INVITE_REPLY = 61148,        //回复俱乐部成员的邀请的回复，只有在接收邀请的但失败才会有

	MSG_S_2_C_NOTIFY_USER_JOIN_CLUB = 61149,     //服务器通知用户申请加入俱乐部

	//////////////////////////////////

	MH_MSG_C_2_S_QUERY_ROOM_GPS_LIMIT = 61150, //查询房间的GPS距离限制配置信息
	MH_MSG_S_2_C_QUERY_ROOM_GPS_LIMIT_RET = 61151,   //返回房间的GPS距离限制配置信息

	MSG_C_2_S_CLUB_MASTER_RESET_ROOM = 61152,   //俱乐部会长申请解散房间
	MSG_S_2_C_CLUB_MASTER_RESET_ROOM = 61153, //服务器返回申请解散房间结果

	MSG_C_2_S_CLUB_CREATE_ROOM = 61154,   //用户创建俱乐部房间

	MSG_C_2_S_QUICK_JOIN_CLUB_DESK = 61155,  //用户快速加入俱乐部房间

	MSG_S_2_C_CLUB_USER_BCAST_MESSAGE = 61156,  //服务器向俱乐部用户广播消息

	MSG_C_2_S_REQUEST_CLUB_ONLINE_COUNT = 61157,         //客户端请求俱乐部在线人数
	MSG_S_2_C_CLUB_ONLINE_COUNT = 61158,                           //服务器返回俱乐部在线人数

	MSG_S_2_C_NOTIFY_CLUBOWNER_MEMBER = 61159,       //通知会长俱乐部成员情况

	MSG_S_2_C_NOTIFY_USER_LEAVE_CLUB = 61160,           //通知用户他被退出俱乐部



	MH_MSG_C_2_S_GAME_RUNTIME_CONFIG = 61161, //玩家请求游戏参数
	MH_MSG_S_2_C_GAME_RUNTIME_CONFIG = 61162,  // 服务器发送参数结果

	MSG_C_2_S_CLUB_SET_ADMINISTRTOR = 61163,    //设置俱乐部管理员
	MSG_S_2_C_CLUB_SET_ADMINSTRTOR = 61164,       //设置俱乐部管理员返回

	MSG_C_2_S_POWER_POINT_LIST = 61165,                //能量列表请求
	MSG_S_2_C_POWER_POINT_LIST = 61166,               //能量列表返回

	MSG_C_2_S_REQUEST_POINT_RECORD = 61167,                //能量查询
	MSG_S_2_C_REPLY_POINT_RECORD = 61168,               //能量查询返回

	MSG_C_2_S_CHANGE_POINT = 61169,                //能量改变
	MSG_S_2_C_CHANGE_POINT = 61170,               //能量改变返回

	MSG_C_2_S_APPLY_JOIN_CLUB = 61171,    //用户申请加入俱乐部
	MSG_S_2_C_APPLY_JOIN_CLUB = 61172,

	MSG_C_2_S_REPLY_ADD_CLUB = 61173,       //申请加入俱乐部响应
	MSG_S_2_C_REPLY_ADD_CLUB = 61174,      //

	MSG_C_2_S_DEL_CLUB_USER = 61175,      //退出俱乐部
	MSG_S_2_C_EXIT_CLUB = 61176,

	MSG_C_2_S_CREATE_CLUB = 61177,      //创建俱乐部
	MSG_S_2_C_CREATE_CLUB=61178,

	MSG_C_2_S_UPDATA_PLAY_TYPE = 61179,  //更新玩法
	MSG_S_2_C_UPDATA_PLAY_TYPE = 61180,

	MSG_C_2_S_UPDATE_CLUB=61181,  //更新俱乐部信息
	MSG_S_2_C_UPDATE_CLUB=61182,

	MSG_C_2_S_LOCK_CLUB_DESK=61183,    //锁住桌子
	MSG_S_2_C_LOCK_CLUB_DESK=61184,


};

///////////////////////////////////////////////////////////////////////////////////////
//用户申请加入俱乐部
struct LMsgC2SApplyJoinClub :public LMsgSC
{
	Lint		m_clubId;//俱乐部id

	LMsgC2SApplyJoinClub() :LMsgSC(MSG_C_2_S_APPLY_JOIN_CLUB)
	{
		m_clubId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SApplyJoinClub(); }
};

//用户申请加入俱乐部
struct LMsgS2CApplyJoinClub :public LMsgSC
{
	Lint     m_errorCode;            // 0--请求成功， 1-请求失败，后台原因  2-失败，俱乐部不存在
	Lint     m_clubId;
	
	LMsgS2CApplyJoinClub() :LMsgSC(MSG_S_2_C_APPLY_JOIN_CLUB)
	{
		m_errorCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CApplyJoinClub(); }
};

///////////////////////////////////////////////////////////////////////////////////////
//用户创建俱乐部
struct LMsgC2SCreateClub :public LMsgSC
{
	Lstring m_name;
	Lstring m_district;
	Lint       m_fee;


	LMsgC2SCreateClub() :LMsgSC(MSG_C_2_S_CREATE_CLUB)
	{
		m_fee = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kName), m_name);
		ReadMapData(obj, NAME_TO_STR(kDistrict), m_district);
		ReadMapData(obj, NAME_TO_STR(kFee), m_fee);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SCreateClub(); }
};

struct LMsgS2CCreateClub :public LMsgSC
{
	Lint     m_errorCode;            // 0--请求成功， 1-请求失败，后台原因  2-失败，没有资格创建俱乐部 3-失败，创建俱乐部数量饱和
	Lint     m_clubId;
	Lstring m_name;
	Lstring m_district;
	Lint       m_fee;

	LMsgS2CCreateClub() :LMsgSC(MSG_S_2_C_CREATE_CLUB)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_fee = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kName), m_name);
		WriteKeyValue(pack, NAME_TO_STR(kDistrict), m_district);
		WriteKeyValue(pack, NAME_TO_STR(kFee), m_fee);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CCreateClub(); }
};

///////////////////////////////////////////////////////////////////////////////////////
//用户更新俱乐部
struct LMsgC2SUpdateClub :public LMsgSC
{
	Lint  m_clubId;
	Lint  m_msgType;     //更新俱乐部类型

	//////////  msgType = 1
	Lint m_clubType;                     //类型，0-普通   1-竞技场
	Lint m_adminCanChange;     // 管理员可以调整能量   0-没有选择  1-选择
	Lint m_adminCanSeeAll;        //管理员可以查询所有能量变化原因  0-没有  1-选择
	Lint m_tableCount;         //俱乐部桌子个数

	
	LMsgC2SUpdateClub() :LMsgSC(MSG_C_2_S_UPDATE_CLUB)
	{
		m_clubId = 0;
		m_msgType = 0;

		m_clubType = 0;
		m_adminCanChange = 0;
		m_adminCanSeeAll = 0;
		m_tableCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kMsgType), m_msgType);
		ReadMapData(obj, NAME_TO_STR(kClubType), m_clubType);
		ReadMapData(obj, NAME_TO_STR(kAdminCanChange), m_adminCanChange);
		ReadMapData(obj, NAME_TO_STR(kAdminCanSeeAll), m_adminCanSeeAll);
		ReadMapData(obj, NAME_TO_STR(kTableCount), m_tableCount);
	
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SUpdateClub(); }
};

struct LMsgS2CUpdateClub :public LMsgSC
{
	Lint     m_errorCode;            // 0--请求成功， 1-请求失败，后台原因  2-失败，没有资格 3-没有这个俱乐部  4--设置俱乐部桌子个数失败
	Lint     m_clubId;
	Lint  m_msgType;     //更新俱乐部类型

	 //////////  msgType = 1
	Lint m_clubType;                     //类型，0-普通   1-竞技场
	Lint m_adminCanChange;     // 管理员可以调整能量   0-没有选择  1-选择
	Lint m_adminCanSeeAll;        //管理员可以查询所有能量变化原因  0-没有  1-选择
	Lint m_tableCount;         //俱乐部桌子个数


	LMsgS2CUpdateClub() :LMsgSC(MSG_S_2_C_UPDATE_CLUB)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_msgType = 0;

		m_clubType = 0;
		m_adminCanChange = 0;
		m_adminCanSeeAll = 0;
		m_tableCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kMsgType), m_msgType);
		WriteKeyValue(pack, NAME_TO_STR(kClubType), m_clubType);
		WriteKeyValue(pack, NAME_TO_STR(kAdminCanChange), m_adminCanChange);
		WriteKeyValue(pack, NAME_TO_STR(kAdminCanSeeAll), m_adminCanSeeAll);
		WriteKeyValue(pack, NAME_TO_STR(kTableCount), m_tableCount);
	
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUpdateClub(); }
};

///////////////////////////////////////////////////////////////////////////////////////
//用户更新玩法
struct LMsgC2SUpdatePlay :public LMsgSC
{
	Lint  m_clubId;
	Lint  m_msgType;					//更新俱乐部类型   5：扯炫最大总钵钵分数
	//////////  msgType = 1
	Lint  m_minPointInRoom;             //加入游戏最小值
	Lint  m_minPointXiaZhu;             //最小下注值
	Lint  m_smallZero;                  //是否可负分   0 不可以  1  可以
	Lint  m_warnScore;                  //预警分数
	Lint  m_maxLimitBo;					//扯炫最大总钵钵分数

	LMsgC2SUpdatePlay() :LMsgSC(MSG_C_2_S_UPDATA_PLAY_TYPE)
	{
		m_clubId = 0;
		m_msgType = 0;

		m_minPointInRoom = 0;
		m_minPointXiaZhu = 0;
		m_smallZero = 0;
		m_warnScore = 0;
		m_maxLimitBo = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kMsgType), m_msgType);
		ReadMapData(obj, NAME_TO_STR(kMinPointInRoom), m_minPointInRoom);
		ReadMapData(obj, NAME_TO_STR(kMinPointXiaZhu), m_minPointXiaZhu);
		ReadMapData(obj, NAME_TO_STR(kFree), m_smallZero);
		ReadMapData(obj, NAME_TO_STR(kWarn), m_warnScore);
		ReadMapData(obj, NAME_TO_STR(kMaxLimitBo), m_maxLimitBo);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SUpdatePlay(); }
};

struct LMsgS2CUpdatePlay :public LMsgSC
{
	Lint    m_errorCode;            // 0--请求成功， 1-请求失败，后台原因  2-失败，没有资格
	Lint    m_clubId;
	Lint	m_msgType;				//更新俱乐部类型

	//////////  msgType = 1
	Lint	m_minPointInRoom;       //加入游戏最小值
	Lint	m_minPointXiaZhu;       //最小下注值
	Lint	m_smallZero;            //是否可负分   0 不可以  1  可以
	Lint	m_warnScore;            //预警分数
	Lint	m_maxLimitBo;				//扯炫普通俱乐部中最大钵钵总数

	LMsgS2CUpdatePlay() :LMsgSC(MSG_S_2_C_UPDATA_PLAY_TYPE)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_msgType = 0;

		m_minPointInRoom = 0;
		m_minPointXiaZhu = 0;
		m_smallZero = 0;
		m_warnScore = 0;
		m_maxLimitBo = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kMsgType), m_msgType);
		WriteKeyValue(pack, NAME_TO_STR(kMinPointInRoom), m_minPointInRoom);
		WriteKeyValue(pack, NAME_TO_STR(kMinPointXiaZhu), m_minPointXiaZhu);
		WriteKeyValue(pack, NAME_TO_STR(kFree), m_smallZero);
		WriteKeyValue(pack, NAME_TO_STR(kWarn), m_warnScore);
		WriteKeyValue(pack, NAME_TO_STR(kMaxLimitBo), m_maxLimitBo);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUpdateClub(); }
};

///////////////////////////////////////////////////////////////////////////////////////
//用户更新玩法
struct LMsgC2SQuickJoinClubDesk :public LMsgSC
{
	Lint		       m_clubId;//桌子id
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;

	LMsgC2SQuickJoinClubDesk() :LMsgSC(MSG_C_2_S_QUICK_JOIN_CLUB_DESK)
	{
		m_clubId = 0;
		m_Gps_Lng = "0";
		m_Gps_Lat = "0";
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SQuickJoinClubDesk(); }
};

///////////////////////////////////////////////////////////////////////////////////////
struct LMsgC2SQuickAddDesk :public LMsgSC
{
	Lint		m_clubId;//俱乐部id
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;

	LMsgC2SQuickAddDesk() :LMsgSC(MSG_C_2_S_QUICK_JOIN_CLUB_DESK)
	{
		m_clubId = 0;
		m_Gps_Lng = "0";
		m_Gps_Lat = "0";
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		// ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SQuickAddDesk(); }
};

///////////////////////////////////////////////////////////////////////////////////////
//锁定俱乐部桌子
struct LMsgC2SLockClubDesk :public LMsgSC
{
	Lint  m_clubId;
	Lint  m_clubDeskId;     //桌子id
	Lint  m_type;               //  0==释放  1==锁定

	LMsgC2SLockClubDesk() :LMsgSC(MSG_C_2_S_LOCK_CLUB_DESK)
	{
		m_clubId = 0;
		m_clubDeskId = 0;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kDesk), m_clubDeskId);
		ReadMapData(obj, NAME_TO_STR(kType), m_type);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SLockClubDesk(); }
};

struct LMsgS2CLockClubDesk :public LMsgSC
{
	Lint     m_errorCode;            // 0--请求成功， 1-请求失败，后台原因  2-失败，房间有人  3-失败房间游戏中
	Lint     m_clubId;
	Lint     m_clubDeskId;     //更新俱乐部类型
	Lint     m_type;                  //  0==释放  1==锁定
		
	LMsgS2CLockClubDesk() :LMsgSC(MSG_S_2_C_LOCK_CLUB_DESK)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_clubDeskId = 0;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kDesk), m_clubDeskId);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CLockClubDesk(); }
};

///////////////////////////////////////////////////////////////////////////////////////







///////////////////////////////////////////////////////////////////////////////////


#endif