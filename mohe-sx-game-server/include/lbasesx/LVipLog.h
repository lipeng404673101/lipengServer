#ifndef _LVIP_LOG_H
#define _LVIP_LOG_H

#include "LTool.h"
#include "LBuff.h"
#include "GameDefine.h"

struct API_LBASESX VipDeskLog
{
	Lstring	 m_videoId;//录像id
	Lint	 m_gold[DESK_USER_COUNT];
	Lint	 m_zhuangPos;//坐庄的位置
	Lint	 m_time;//本局时间
	Lint	 m_angang[DESK_USER_COUNT];//暗杠
	Lint	 m_mgang[DESK_USER_COUNT];//明杠
	Lint	 m_checkTing[DESK_USER_COUNT];	//查听

	Lint	m_zimo[DESK_USER_COUNT];//自摸次数
	Lint	m_bomb[DESK_USER_COUNT];//放炮次数
	Lint	m_win[DESK_USER_COUNT];//收炮次数
	Lint	m_dzimo[DESK_USER_COUNT];//大胡自摸次数
	Lint	m_dbomb[DESK_USER_COUNT];//大胡放炮次数
	Lint	m_dwin[DESK_USER_COUNT];//大胡收炮次数

	VipDeskLog()
	{
		memset(m_gold, 0, sizeof(m_gold));
		memset(m_angang, 0, sizeof(m_angang));
		memset(m_mgang, 0, sizeof(m_mgang));
		memset(m_checkTing, 0, sizeof(m_checkTing));
		m_zhuangPos = INVAILD_POS;
		m_time = 0;
		m_videoId = "";

		memset(m_zimo, 0, sizeof(m_zimo));
		memset(m_bomb, 0, sizeof(m_bomb));
		memset(m_win, 0, sizeof(m_win));
		memset(m_dzimo, 0, sizeof(m_dzimo));
		memset(m_dbomb, 0, sizeof(m_dbomb));
		memset(m_dwin, 0, sizeof(m_dwin));
	}
};

struct API_LBASESX LVipLogItem
{
	Lstring			m_id;		//id
	Lint			m_time;		//时间
	Lint			m_state;	//房间玩法0-自摸，1-点炮，2-抢杠
	Lint			m_deskId;	//桌子id
	Lstring			m_secret;	//密码
	Lint			m_maxCircle;//总圈数
	Lint			m_curCircle;//当前圈数，会刷给客户端的，显示类似“第1/8局”字样
	Lint			m_curCircleReal; // 实际的当前圈数，因为有的玩法连庄不加圈数，该值仅用于判断第1局insert其它局update到数据库
	Lint			m_posUserId[DESK_USER_COUNT];//0-3各个位置上的玩家id
	Lint			m_curZhuangPos;//当前庄家
	Lint			m_score[DESK_USER_COUNT];// 各个位置上面的积分
	Lint			m_reset;//是否解算
	std::vector<Lint> m_playtype;		//玩法列表  1-自摸加底 2-自摸加翻
	std::vector<VipDeskLog*> m_log;//每一把的记录
	Lint			m_checkTing[DESK_USER_COUNT];		//查听次数

	Lint			m_iPlayerCapacity;				//桌子玩家容量不能超过DESK_USER_COUNT

	LVipLogItem();
	virtual  ~LVipLogItem();
	virtual  Lstring		ToString();

	virtual  void			FromString(const Lstring& str);

	virtual  Lstring		PlayTypeToStrint();

	virtual  void			PlayTypeFromString(const Lstring& str);
};
#endif