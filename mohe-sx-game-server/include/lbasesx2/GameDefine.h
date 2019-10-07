#ifndef _GAME_DEFINE_H_
#define _GAME_DEFINE_H_

#include "LBase.h"

#if defined(DEBUG) || defined(_DEBUG)
// 定义这个宏表示 不 启用心跳检查
//#define UNCHECKHEARTBEAT
#endif

#define BASE_CARD_COUNT 36
#define LAI_ZI_COUNT 4
#define CARD_COUNT 136
#define WIND_CARD_COUNT 28
#define MAX_HANDCARD_COUNT 13

enum CARD_COLOR
{
	CARD_COLOR_WAN = 1, // 万
	CARD_COLOR_TUO = 2,	// 饼
	CARD_COLOR_SUO = 3, // 条
	CARD_COLOR_ZI  = 4, // 东南西北中发白
};

//玩家思考类型
enum THINK_OPERATOR
{
	THINK_OPERATOR_NULL = 0,
	THINK_OPERATOR_OUT = 1,
	THINK_OPERATOR_BOMB = 2,//胡
	THINK_OPERATOR_AGANG = 3,//暗杠
	THINK_OPERATOR_MGANG = 4,//明杠
	THINK_OPERATOR_PENG = 5,//碰
	THINK_OPERATOR_CHI = 6,//吃
	THINK_OPERATOR_TING = 7,//听
	THINK_OPERATOR_MBU = 8, // 补杠

	THINK_OPERATOR_DINGQUE = -3,
	THINK_OPERATOR_CHANGE = -2,
	THINK_OPERATOR_REOUT = -1, // 断线续完再次告诉玩家打出去的牌
	//THINK_OPERATOR_NULL  = 0, 
	//THINK_OPERATOR_OUT   = 1,
	//THINK_OPERATOR_BOMB  = 2, // 胡
	//THINK_OPERATOR_AGANG = 3, // 暗杠	目前没用到
	//THINK_OPERATOR_MGANG = 4, // 明杠
	//THINK_OPERATOR_PENG  = 5, // 碰
	//THINK_OPERATOR_CHI   = 6, // 吃     目前没用到
	THINK_OPERATOR_ABU = 12, // 暗杠
	THINK_OPERATOR_DIANGANGPAO = 14,// 玩家点杠后自摸胡算点炮胡操作
	THINK_OPERATOR_QIANGANGH = 15, //抢杠胡
	THINK_OPERATOR_TINGCARD = 16,  //听牌
	THINK_OPERATOR_FLY = 17,   //飞
	THINK_OPERATOR_RETURN = 18,   //提
	THINK_OPERATOR_MUSTHU = 19,   //必须胡
	THINK_OPERATOR_HONGTONG_YINGKOU = 20, //洪洞王牌硬扣选择
	THINK_OPERATOR_HONGTONG_YINGKOU_NOT = 21, //洪洞王牌不硬扣的选择
	THINK_OPERATOR_HONGTONG_XUANWANG=22  //洪洞王牌双王选王（**Ren 2017-12-12）

};
#define DESK_USER_COUNT 4	//一桌玩家数量

#define INVAILD_POS		4     //无效的位置
#define INVAILD_POS_QIPAI 21  //无效的位置
#define INVAILD_POS_MANAGER_START -1    //房主开始游戏

#define	PLAY_CARD_TIME		10  //出牌时间
#define WAIT_READY_TIME		15  //准备时间

//桌子当前状态
enum DESK_STATE
{
	DESK_FREE =		0,	//桌子空闲状态
	DESK_WAIT,			//桌子等待状态
	DESK_PLAY,			//正在游戏中
	//	DESK_COUNT_RESULT,	//正在结算中
	//	DESK_RESET_ROOM		//正在申请解散房间中
};

//牌局当前状态
enum DESK_PLAY_STATE
{
	DESK_PLAY_START_HU,		//起手胡状态
	DESK_PLAY_GET_CARD,		//摸牌
	DESK_PLAY_THINK_CARD,	//思考
	DESK_PLAY_END_CARD,		//海底牌
	DESK_PLAY_DINGQUE,		//定缺
	DESK_PLAY_CHANGE,		//换牌
	DESK_PLAY_FANPIGU,		//翻屁股补牌
};

//家胡牌情况，0-没胡，1-自摸，2-收炮，3-点炮
enum WIN_TYPE
{
	WIN_INVALID = -1,
	WIN_ZIMO = 0,	//自摸
	WIN_BOMB,		//点炮
	WIN_NONE,		//慌庄
	WIN_DISS,       //解散房间
	WIN_MINGGANGHUA,//明杠开花 ：4  撵中子：明杠刚上开花不算自摸（**Ren 2017-12-01）
};

//胡牌方式
enum WIN_TYPE_SUB
{
	WIN_SUB_NONE = 0,
	WIN_SUB_ZIMO = 1,		//自摸
	WIN_SUB_BOMB = 2,		//收炮
	WIN_SUB_ABOMB = 3,		//点炮

	WIN_SUB_DZIMO = 4,		//大胡自摸
	WIN_SUB_DBOMB = 5,		//大胡收炮
	WIN_SUB_DABOMB = 6,		//大胡点炮
	WIN_SUB_TINGCARD = 7,   //有叫 3/4血战，2房
	WIN_SUB_MINGGANGHUA=8,//明杠开花：8   撵中子：明杠刚上开花不算自摸（**Ren 2017-12-01）
	WIN_SUB_ZIMOANGANGHUA=9,    //自摸(暗杠开花) //添加刚上开花功能（**Ren 2017-12-11）
	WIN_SUB_ZIMOMINGGANGHUA=10, //自摸(明杠开花) //添加刚上开花功能（**Ren 2017-12-11）

};

//起手胡牌类型
enum START_HU_INFO
{
	START_HU_NULL = 0,
	START_HU_QUEYISE=1,	//缺一色
	START_HU_BANBANHU,		//板板胡
	START_HU_DASIXI,		//大四喜
	START_HU_LIULIUSHUN,	//六六顺
};

//麻将类型
typedef int GameType;
enum {
	SiChuanMaJiang		= 101,	//血战到底
	SiChuanMaJinagxl	= 102,	//血流成河
	SiChuanMaJinagThr   = 103,  //3人血战麻将
	SiChuanMaJinagZZ	= 2001,	//倒到胡
	SiChuanMaJiangFourTwo = 105,  //4人2房
	SiChuanMaJiangDeYang = 106,   //德阳麻将
	SiChuanMaJiangThrThr = 107,   //三人三房
	SiChuanMaJiangManYng = 108,  //绵阳
	SiChuanMaJiangYiBin  = 109,  //四川宜宾
	SiChuanMaJiangWanZhou= 110,	// 万州
	YunNanMajiang		= 201,	// 云南麻将
	KunMingMajiang		= 202,	// 昆明麻将
	LiJiangKaXinWu		= 203,  // 丽江卡心五
	YunNanFeiXiaoJi     = 204,  // 云南飞小鸡
	SiChuanMaJinagxlc	= 1102,	//血流成河 金币场

};

enum HU_WAY
{
	HUWAY_ZiMo =1,
	HUWAY_MoBao =2,
	HUWAY_DuiBao =3,
};

//请求解散房间
enum RESET_ROOM_TYPE
{
	RESET_ROOM_TYPE_WAIT,
	RESET_ROOM_TYPE_AGREE,
	RESET_ROOM_TYPE_REFLUSE,
};


enum ErrorCode
{
	ErrorNone = 0,
	UserNotExist = 1,
	TaskNotExist = 2,
	ExceedMaximum = 3,
	AlreadyBinded = 4,
	NotQualified = 5,
	TaskNotFinished = 6,
	Expired = 7,
	BindingSelf = 8,
	Unknown = 100
};



#endif