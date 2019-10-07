#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "LMsgL2C.h"
#include "TuiTongZiGameLogic.h"


enum GAMG_CONCLUDE_REASON
{
	GAMG_CONCLUDE_COMPARECARD,
	GAMG_CONCLUDE_NOPLAYER,
	GAMG_CONCLUDE_SYSTEM_COMPARECARD,
	GAMG_CONCLUDE_DISMISS
};

enum GAME_ZHUANG_MODE
{
	GAME_MODE_LUN_ZHUANG=1,
	GAME_MODE_QIANG_ZHUANG=2,
	GAME_MODE_BAWANG_ZHUANG=3,
};

enum GAME_PLAY_STATUS
{
	GAME_PLAY_BEGIN = -1,
	GAME_PLAY_SELECT_ZHUANG = 0,
	GAME_PLAY_DO_SHAI_ZI,
	GAME_PLAY_ADD_SCORE,
	GAME_PLAY_SEND_CARD,
	GAME_PLAY_END,
};

enum PLAY_STATUS_DELAY_TIME
{
	DELAY_TIME_SELECT_ZHUANG=10,
	DELAY_TIME_SELECT_QIANG_ZHUANG=30,
	DELAY_TIME_DO_SHAI_ZI=10,
	DELAY_TIME_ADD_SCORE=30 + 7,
	DELAY_TIME_OPEN_CARD=20,
};

//玩法小选项
struct PlayType
{
public:
	void clear() { m_playtype.clear(); }
	void setAllTypes(const std::vector<Lint>& v) { m_playtype=v; }
	const std::vector<Lint>& getAllTypes() const { return m_playtype; }

	Lint   TuiTongZiGetScoreTime() const;                               //1--倍数�?  实际数�?  1�?�?�?  
	Lint   TuiTongZiGetGameMode() const;                            //2--庄的模式1：轮�?  2：抢�? 3：霸王庄
	Lint   TuiTongZiGetAuto() const;                                        //3--托管   0--没有选择 1--选择
	Lint   TuiTongZiGetCuoPai() const;                                   //4--搓牌     0--没有选择 1--选择
	Lint   TuiTongZiGetGameType() const;                           //5--游戏类型 2--经典  1--晋北
	Lint   TuiTongZiGet2or8() const;                                     //6--2-8人游�?0--没有选择 1--选择
	Lint   TuiTongZiGetDynamic() const;                            //7--动态加�? 0--没有选择 1--选择
	Lint    TuiTongZiGetBaiBan() const;                              //8--白版   0--没有选择 1--选择

protected:
	std::vector<Lint>	m_playtype;
};

Lint   PlayType::TuiTongZiGetScoreTime() const
{
	if (m_playtype.empty())return 1;
	return m_playtype[0];
}

Lint   PlayType::TuiTongZiGetGameMode() const
{
	if (m_playtype.size()<2)return 1;
	return m_playtype[1];
}

Lint   PlayType::TuiTongZiGetAuto() const
{
	if (m_playtype.size()<3)return 0;
	return m_playtype[2];
}

Lint   PlayType::TuiTongZiGetCuoPai() const
{
	if (m_playtype.size()<4)return 0;
	return m_playtype[3];
}

Lint   PlayType::TuiTongZiGetGameType() const
{
	if (m_playtype.size()<5)return 1;
	return m_playtype[4];
}

Lint   PlayType::TuiTongZiGet2or8() const
{
	if (m_playtype.size()<6)return 0;
	return m_playtype[5];
}

Lint   PlayType::TuiTongZiGetDynamic() const
{
	if (m_playtype.size()<7)return 0;
	return m_playtype[6];
}

Lint   PlayType::TuiTongZiGetBaiBan() const
{
	if (m_playtype.size()<8)return 0;
	return m_playtype[7];
}


// 不会自动初始化的字段放这里，每盘自动重置
struct TuiTongZiRoundState__c_part
{
	Lint         m_curPos;
	Lint         m_user_status[TUITONGZI_PLAY_USER_COUNT];         //用户状�?
	Lint         m_play_status;
	//Lint         m_shaiZi[2];
	BYTE      m_hand_card_data[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_HAND_CARD_MAX];           //用户手牌
	Lint         m_score[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_XIAZHU_XIANG];          //用户选分  押道软红 0123
	Lint         m_openCard[TUITONGZI_PLAY_USER_COUNT];           // 0-没有开�?  1-开�?
	Lint         m_selectZhuang[TUITONGZI_PLAY_USER_COUNT];            //用户选庄  1-选庄 2-没有选庄 
	//Lint        m_zhuangConfirm;                                                                        //庄已经确定，不再接收选庄命令
	Lint         m_resultScore[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_XIAZHU_XIANG];
	bool        m_win[TUITONGZI_PLAY_USER_COUNT];
	Lint         m_currSelectZhuang;
	

	/* 清零结果�?*/
	void clear_round()
	{
		//LLOG_DEBUG("TuiTongZiRoundState__c_part clear_round");
		memset(this, 0, sizeof(*this));
		m_play_status = GAME_PLAY_BEGIN;
		//memset(m_hand_card_data, 0, sizeof(m_hand_card_data));
		//
		LLOG_DEBUG("m_curPos=[%d]",m_curPos);
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
		{
			LLOG_DEBUG("m_user_status[%d]=[%d]",i, m_user_status[i]);
			LLOG_DEBUG("m_openCard[%d]=[%d]", i, m_openCard[i]);
			LLOG_DEBUG("m_selectZhuang[%d]=[%d]", i, m_selectZhuang[i]);
			LLOG_DEBUG("m_win[%d]=[%d]", i, m_win[i]);
			LLOG_DEBUG("m_socre[%d],[%d][%d][%d][%d]", i, m_score[i][0], m_score[i][1], m_score[i][2], m_score[i][3]);
		//
		//	m_curPos = TUITONGZI_INVAILD_POS;
		//	m_selectZhuang[i] = 0;
		//	m_openCard[i] = 0;
		//	m_win[i] = 0;
		//	memset(m_score[i],0,sizeof(Lint)*TUITONGZI_XIAZHU_XIANG);
		//	memset(m_resultScore[i], 0, sizeof(Lint)*TUITONGZI_XIAZHU_XIANG);
		//	m_currSelectZhuang = TUITONGZI_INVAILD_POS;
		//
		}

		
	}

};

// 有构造函数的字段放这里，每盘自动重置
struct TuiTongZiRoundState__cxx_part
{
	//PlayerState  m_ps[TUITONGZI_PLAY_USER_COUNT];   //用于存储每个玩家手牌以及输赢结果
	TuiTongZiS2CDoShaiZi  m_shaiZi;
	LTime m_play_status_time;

	void clear_round() 
	{
		this->~TuiTongZiRoundState__cxx_part();
		new (this) TuiTongZiRoundState__cxx_part;

		m_shaiZi.reset();
	}
};

struct TuiTongZiMatchState__c_part
{
	Desk*         m_desk;
	int           m_round_offset;
	int           m_round_limit;
	int           m_accum_score[TUITONGZI_PLAY_USER_COUNT];
	bool          m_dismissed;

	int           m_registered_game_type;
	int           m_player_count;   // 玩法核心代码所使用的玩家数量字�?
	Lint          m_zhuangPos;      //庄家位置

	void clear_match() 
	{
		memset(this, 0, sizeof(*this));
	}
};

struct TuiTongZiMatchState__cxx_part 
{
	TGameLogic	  m_gamelogic;	  //游戏逻辑
								  // 初始化赋值一次就可以
	PlayType m_playtype;	// 桌子玩法

	void clear_match() {
		this->~TuiTongZiMatchState__cxx_part();
		new (this) TuiTongZiMatchState__cxx_part;
	}
};


struct TuiTongZiRoundState : TuiTongZiRoundState__c_part, TuiTongZiRoundState__cxx_part {
	// 不要在这里加字段
	void clear_round() {
		TuiTongZiRoundState__c_part::clear_round();
		TuiTongZiRoundState__cxx_part::clear_round();
	}
};

// 一场Match会有好几盘Round
struct TuiTongZiMatchState : TuiTongZiMatchState__c_part, TuiTongZiMatchState__cxx_part {
	// 不要在这里加字段
	void clear_match() {
		TuiTongZiMatchState__c_part::clear_match();
		TuiTongZiMatchState__cxx_part::clear_match();
	}
};

struct TuiTongZiDeskState : TuiTongZiRoundState, TuiTongZiMatchState {
	void clear_match(int player_count) {
		TuiTongZiMatchState::clear_match();
		clear_round();
		m_player_count=player_count;
	}

	void clear_round() {
		TuiTongZiRoundState::clear_round();
	}

	void setup_match(std::vector<Lint> &l_playtype, int round_limit) {
		m_round_limit=round_limit;
		//// 小选项
		m_playtype.setAllTypes(l_playtype);
	}

	void increase_round() {
		++m_round_offset;
		if (m_desk && m_desk->m_vip) ++m_desk->m_vip->m_curCircle;
	}

	void set_desk_state(DESK_STATE v) {
		if (m_desk) {
			if (v==DESK_WAIT) m_desk->SetDeskWait();
			else m_desk->setDeskState(v);
		}
	}
	DESK_STATE get_desk_state() {
		// 桌子�?DESK_PLAY/DESK_WAIT/DESK_FREE 三种状�?
		// 单元测试时没有Desk对象，一直是 DESK_PLAY 状�?
		if (m_desk) return (DESK_STATE)m_desk->getDeskState();
		else return DESK_PLAY;
	}

	void set_play_status(Lint play_status)
	{
		if(play_status>=GAME_PLAY_SELECT_ZHUANG&&play_status<=GAME_PLAY_END)
			m_play_status = play_status;
	}

	int GetUserPos(User* pUser)
	{
		if (pUser==NULL)return TUITONGZI_INVAILD_POS;
		Lint pos=TUITONGZI_INVAILD_POS;
		if (m_desk)
			pos=m_desk->GetUserPos(pUser);
		if (pos>=TUITONGZI_PLAY_USER_COUNT)
			pos=TUITONGZI_INVAILD_POS;

		return pos;
	}

	//取的有效的下个位�?
	int GetNextPos(int pos) {
		if (m_player_count==0) return 0; // 避免除零崩溃	
		Lint nextPos=(pos+1)%m_player_count;
		for (int i=0; (i<m_player_count)&&(m_user_status[nextPos]!=1); i++)
		{
			nextPos=(nextPos+1)%m_player_count;
		}
		return nextPos;
	}

	bool is_pos_valid(int pos) {
		if (NULL==m_desk) return false;
		return 0<=pos && pos < TUITONGZI_PLAY_USER_COUNT;
	}

	Lint   GetPlayingUserCount()
	{
		Lint wPlayerCount = 0;
		for (Lint i = 0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] == TRUE) wPlayerCount++;
		}
		return wPlayerCount;
	}

};


struct TuiTongZiVideoSupport : TuiTongZiDeskState {
	VideoLog		m_video;						//录像

///////////////////////////////////////////////////////////////////
// 这块集中存放和录像有关的函数
///////////////////////////////////////////////////////////////////
	void VideoSave() {
		if (NULL==m_desk) return;

		m_video.m_Id=gVipLogMgr.GetVideoId();
		m_video.m_playType=m_desk->getPlayType();
		//m_video.m_flag=m_registered_game_type;
		LMsgL2LDBSaveVideo video;
		video.m_type=0;
		video.m_sql=m_video.GetInsertSql();
		gWork.SendMsgToDb(video);
	}
};

struct TuiTongZiGameCore : GameHandler, TuiTongZiVideoSupport {

	//色子
	Lint getShaiZiPos(Lint dian1, Lint dian2)
	{
		Lint dian=dian1+dian2;
		if (dian==2||dian==6||dian==10) //出门  庄家右边
		{
			return (m_zhuangPos+1)% TUITONGZI_JINBEI_4_USER;
		}
		else if(dian==3||dian==7||dian==11)   //对面
		{
			return (m_zhuangPos+2)% TUITONGZI_JINBEI_4_USER;
		}
		else if (dian==4||dian==8||dian==12)  //倒门 左边
		{
		   return (m_zhuangPos+3)% TUITONGZI_JINBEI_4_USER;
		}
		else if (dian==5||dian==9)
		{
			return m_zhuangPos;
		}
		else
		{
			LLOG_ERROR("getShaiZiPos  dian[%d]  error !!!!", dian);
		}

		
	}

	Lint getShaiZiPos_2and8(Lint dian1, Lint dian2)
	{
		Lint dian = dian1 + dian2;
		Lint randNum=  dian%GetPlayingUserCount();

		Lint count = 0;
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] == 1&& count==randNum)
			{
				return i;
			}
		   if (m_user_status[i])count++;
		}

	}


	void   create_shaiZi()
	{
		m_shaiZi.m_dian1=rand()%6+1;
		m_shaiZi.m_dian2=(rand()+time(NULL))%6+1;
		if(m_playtype.TuiTongZiGet2or8()==1)
			m_shaiZi.m_pos = getShaiZiPos_2and8(m_shaiZi.m_dian1, m_shaiZi.m_dian2);
		else
			m_shaiZi.m_pos=getShaiZiPos(m_shaiZi.m_dian1, m_shaiZi.m_dian2);
		LLOG_DEBUG("create_shaiZi  dian1=[%d],dian2=[%d],pos=[%d],zhuangPos=[%d]", m_shaiZi.m_dian1, m_shaiZi.m_dian2,m_shaiZi.m_pos,m_zhuangPos);
	}

	//计算分数
	//经典
	Lint getYaScoreJingDain(Lint pos)
	{
		if ( m_win[pos] && m_gamelogic.isDui(m_hand_card_data[pos]) ) return 2 * m_score[pos][0];
		else if (m_win[pos]) return m_score[pos][0];
		else return (-1)* m_score[pos][0];
	}
	//晋北玩法
	Lint getYaScore(Lint pos)
	{
		return m_win[pos]?m_score[pos][0]:(-1)*m_score[pos][0];
	}
	Lint getDaoScore(Lint pos)
	{
		if(m_win[pos])
		{
			if (m_gamelogic.isDao(m_hand_card_data[pos]))
				return m_score[pos][1];
			else
				return 0;
				//return m_score[pos][1]*(0.5);
		}
		else
		{
			if (m_gamelogic.isDao(m_hand_card_data[m_zhuangPos]))
				return (-1)*m_score[pos][1];
			else
				return 0;
				//return (-1)*m_score[pos][1]*(0.5);
		}
	}
	Lint getRuanScore(Lint pos)
	{
		if (_getXianWinCount()>=2) return m_score[pos][2];
		else return (-1)*m_score[pos][2];
	
	}
	Lint getHongScore(Lint pos)
	{
		if (_getXianWinCount()>=3) return 3*m_score[pos][3];
		else return (-1)*m_score[pos][3];
	}
	//闲家胜利人数(不是什么时候都有效�?
	Lint _getXianWinCount()
	{
		Lint userSelectCount=0;
		for (Lint i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (i==m_zhuangPos)continue;
			if (m_win[i])userSelectCount++;
		}
		return userSelectCount;
	}

	void accum_score_jinBei()
	{
		for (Lint i=0; i<TUITONGZI_JINBEI_4_USER; i++)
		{
			if (i==m_zhuangPos)continue;
			Lint ret=m_gamelogic.CompareCard(m_hand_card_data[m_zhuangPos], m_hand_card_data[i], TUITONGZI_HAND_CARD_MAX);
			if (ret==0)
				m_win[i]=true;
			else
				m_win[i]=false;
		}

		for (Lint i=0; i<TUITONGZI_JINBEI_4_USER; i++)
		{
			if (i==m_zhuangPos)continue;
			m_resultScore[i][0]=getYaScore(i);
			m_resultScore[i][1]=getDaoScore(i);
			m_resultScore[i][2]=getRuanScore(i);
			m_resultScore[i][3]=getHongScore(i);

			m_resultScore[m_zhuangPos][0]+=(-1)*m_resultScore[i][0];
			m_resultScore[m_zhuangPos][1]+=(-1)*m_resultScore[i][1];
			m_resultScore[m_zhuangPos][2]+=(-1)*m_resultScore[i][2];
			m_resultScore[m_zhuangPos][3]+=(-1)*m_resultScore[i][3];
		}

	}

	void accum_score_jingDian()
	{
		for (Lint i = 0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] != 1)continue;
			Lint ret = m_gamelogic.CompareCard(m_hand_card_data[m_zhuangPos], m_hand_card_data[i], TUITONGZI_HAND_CARD_MAX);
			if (ret == 0)
				m_win[i] = true;
			else
				m_win[i] = false;
		}

		for (Lint i = 0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] != 1)continue;
			m_resultScore[i][0] = getYaScoreJingDain(i);
		
			m_resultScore[m_zhuangPos][0] += (-1)*m_resultScore[i][0];
			
		}

	}

	void accum_score()
	{
		if (m_playtype.TuiTongZiGetGameType() == 2)
			accum_score_jingDian();
		else if (m_playtype.TuiTongZiGetGameType() == 1)
			accum_score_jinBei();
		else
		{
			LLOG_ERROR("accum_score ERROR ,m_playtype.TuiTongZiGetGameType()=[%d] ", m_playtype.TuiTongZiGetGameType());
		}
	}

	//庄位�?
	void send_zhuang_cmd(Lint pos)
	{
		if (pos<0||pos>TUITONGZI_PLAY_USER_COUNT)
		{
			LLOG_ERROR("send_zhuang_cmd pos error!!!");
			return;
		}
		//TuiTongZiS2CStartGame send;
		//send.m_pos=pos;

		if (pos == TUITONGZI_INVAILD_POS)
		{
			for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
			{
				TuiTongZiS2CStartGame send;
				send.m_pos = i;
				notify_user(send, i);
			}
		}
		else
		{
			TuiTongZiS2CStartGame send;
			send.m_pos = pos;
			m_currSelectZhuang = pos;
			notify_desk(send);
		}

	}

	void notify_zhuang(Lint pos)
	{
		if (pos<0||pos>TUITONGZI_PLAY_USER_COUNT)
		{
			LLOG_ERROR("send_zhuang_cmd pos error!!!");
			return;
		}
		TuiTongZiS2CSelectZhuang send;
		send.m_pos=pos;
		send.m_value=2;

		notify_desk(send);
	}

	void notify_yes_zhuang(Lint pos)
	{
		if (pos<0 || pos>TUITONGZI_PLAY_USER_COUNT)
		{
			LLOG_ERROR("notify_yes_zhuang pos error!!!");
			return;
		}
		TuiTongZiS2CSelectZhuang send;
		send.m_pos = pos;
		send.m_value = 1;

		notify_desk(send);
	}

	void notify_no_zhuang(Lint pos)
	{
		if (pos<0 || pos>TUITONGZI_PLAY_USER_COUNT)
		{
			LLOG_ERROR("notify_no_zhuang pos error!!!");
			return;
		}
		TuiTongZiS2CSelectZhuang send;
		send.m_pos = pos;
		send.m_value = 0;

		notify_desk(send);
	}

	Lint rand_zhuang()
	{
		Lint randNum = rand()%GetPlayingUserCount();
		Lint count = 0;
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] == 1 && count == randNum)
			{
				return i;
			}
			if (m_user_status[i])count++;
		}

	}
	Lint qiang_zhuang()
	{
		std::vector<Lint> tempPos;
		for (int i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_selectZhuang[i]==1)
				tempPos.push_back(i);
		}
		if (tempPos.size()==0)
			return rand_zhuang();
		else
			return tempPos[rand()%tempPos.size()];
	}
	void set_zhuang(Lint zhuangPos)
	{
		//m_zhuangConfirm=TRUE;
		notify_zhuang(zhuangPos);
		set_play_status(GAME_PLAY_DO_SHAI_ZI);
		m_play_status_time.Now();
		//篩子
		create_shaiZi();
		m_shaiZi.m_type=1;
		notify_user(m_shaiZi, zhuangPos);

	}

	//判断阶段
	bool select_zhuang_over()
	{
		Lint userSelectCount=0;
		for (Lint i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_selectZhuang[i]!=0)userSelectCount++;
		}
		if (userSelectCount == GetPlayingUserCount())
		{
			return true;
		}
		else
			return false;
		
	}
	bool add_score_over()
	{
		Lint userSelectCount=1;
		for (Lint i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_score[i][0]!=0&&i!=m_zhuangPos)userSelectCount++;
		}
		if (userSelectCount == GetPlayingUserCount())
		{
			//m_play_status = GAME_PLAY_SEND_CARD;
			set_play_status(GAME_PLAY_SEND_CARD);
			m_play_status_time.Now();
		}
		return userSelectCount== GetPlayingUserCount() ?true:false;
	}
	bool open_card_over()
	{
		Lint userSelectCount=0;
		for (Lint i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_openCard[i]!=0)userSelectCount++;
		}
		if (userSelectCount == GetPlayingUserCount())
		{
			set_play_status(GAME_PLAY_END);
			//m_play_status = GAME_PLAY_END;
			return true;
		}
		else
			return false;
	}

	//发牌
	bool send_user_card(Lint pos, Lint broadcast=false)
	{
		TuiTongZiS2COpenCard send;
		if (pos>=0&&pos<TUITONGZI_PLAY_USER_COUNT)
		{
			send.m_pos=pos;
			for (Lint i=0; i<TUITONGZI_HAND_CARD_MAX; i++)
				send.m_userCard[i]=m_hand_card_data[pos][i];
		}
		else
		{
			return false;
		}
		if (broadcast)
		{
			send.m_showCard = 1;
			notify_desk(send);
		}
		else
		{
			send.m_showCard = 0;
			notify_user(send, pos);
		}

		return true;
	}

	void select_zhuang()
	{
		//选庄
		switch (m_playtype.TuiTongZiGetGameMode())
		{
		case GAME_MODE_LUN_ZHUANG:
		{
			if (m_round_offset == 0)
			{
				LLOG_DEBUG("m_round_offset==0");
				Lint pos = 0;
				if (m_desk) pos = m_desk->MHSpecPersonPos();
				send_zhuang_cmd(pos);
			}
			else
			{
				m_zhuangPos = GetNextPos(m_zhuangPos);
				set_zhuang(m_zhuangPos);
			}
			break;
		}
		case GAME_MODE_QIANG_ZHUANG:
		{
			send_zhuang_cmd(TUITONGZI_INVAILD_POS);
			break;
		}
		case GAME_MODE_BAWANG_ZHUANG:
		{
			if (m_round_offset == 0)
			{
				Lint pos = 0;
				if (m_desk) pos = m_desk->MHSpecPersonPos();
				send_zhuang_cmd(pos);
			}
			else
			{
				set_zhuang(m_zhuangPos);
			}

			break;
		}
		default:
		{
			LLOG_ERROR("m_playtype.gametypeGetGameMode ERROR undefined [%d]", m_playtype.TuiTongZiGetGameMode());
			break;
		}

		}
	}

	//一局游戏开始路�?
	void start_game()
	{
		LLOG_DEBUG("start_game  ....[%d]",m_play_status);
		//分发扑克
		m_gamelogic.RandCardList(m_hand_card_data[0], sizeof(m_hand_card_data)/sizeof(m_hand_card_data[0][0]),m_playtype.TuiTongZiGetBaiBan());

		set_play_status(GAME_PLAY_SELECT_ZHUANG);
		m_play_status_time.Now();
		//选庄
		select_zhuang();

	}

	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	virtual void notify_desk_playing_user(LMsg &msg) {
	}

	virtual void notify_desk_without_user(LMsg &msg, User* pUser) {
	}

	//广播游戏局数给桌上用户
	void notify_desk_match_state() {
		LMsgS2CVipInfo msg;
		msg.m_curCircle=m_round_offset+1;
		msg.m_curMaxCircle=m_round_limit;
		//m_playtype.fill_vector_for_client(msg.m_playtype, FEAT_GAME_TYPE);
		notify_desk(msg);
	}

	void add_round_log(Lint *score, int zhuangPos) {

		// 保存结果	
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, score, m_user_status, zhuangPos, m_video.m_Id);
			//m_desk->m_vip->ResetJinAnGangCount(); // 使用后清除掉
		}

	}

	//开始一局
	void start_round(Lint * PlayerStatus) {
		LLOG_DEBUG("Logwyz  ...    START  GAME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

		clear_round();
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);

		memcpy(m_user_status, PlayerStatus, sizeof(m_user_status));

		//通知manager局数变�?
		if (m_desk && m_desk->m_vip)
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle+1, m_desk->m_vip->m_maxCircle);
		LLOG_ERROR("****Desk %d start round %d/%d   player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);
		LLOG_DEBUG("TuiTongZiGetGameMode=[%d],TuiTongZiGetScoreTime=[%d],TuiTongZiGetAuto=[%d],TuiTongZiGetCuoPai=[%d],TuiTongZiGetGameType()=[%d]", m_playtype.TuiTongZiGetGameMode(), m_playtype.TuiTongZiGetScoreTime(), m_playtype.TuiTongZiGetAuto(), m_playtype.TuiTongZiGetCuoPai(), m_playtype.TuiTongZiGetGameType());

		

		//游戏开始路�?
		start_game();
		LLOG_DEBUG("****Desk %d start round %d/%d, player_count=%d", m_desk->GetDeskId(), m_round_offset, m_round_limit, m_player_count);
	}

	//结束一局
	void  finish_round()
	{
		LLOG_DEBUG("finish_round");

		increase_round();

		TuiTongZiS2CResult send;

		if(m_desk)
			memcpy(send.m_UserStatus, m_desk->m_playStatus, sizeof(send.m_UserStatus));
		else
			memcpy(send.m_UserStatus, m_user_status, sizeof(send.m_UserStatus));

		send.m_ZhuangPos=m_zhuangPos;
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] != 1)continue;
			for (Lint j = 0; j < TUITONGZI_HAND_CARD_MAX; j++)
				send.m_CardData[i][j] = m_hand_card_data[i][j];

			memcpy(send.m_Score[i], m_score[i], sizeof(Lint)*TUITONGZI_XIAZHU_XIANG);
		}
		
		send.m_FinalDw=m_dismissed||m_round_offset>=m_round_limit;

		//统计分数
		memcpy(send.m_ResultScore, m_resultScore, sizeof(send.m_ResultScore));
		for (Lint i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] != 1)continue;
			for (Lint j=0; j<TUITONGZI_XIAZHU_XIANG; j++)
			{
				send.m_GameScore[i]+=send.m_ResultScore[i][j];
			}
		}

		Lint wWinner=m_zhuangPos;
		Lint maxScore=send.m_GameScore[m_zhuangPos];
		for (Lint i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (send.m_GameScore[i]>maxScore)
			{
				maxScore=send.m_GameScore[i];
				wWinner=i;
			}
		}
	
		// 保存录像
		//VideoSave();
		add_round_log(send.m_GameScore, wWinner);

		//test log
		LLOG_DEBUG("GAME_END m_FinalDw=[%d]", send.m_FinalDw);
		LLOG_DEBUG("GAME_END m_ZhuangPos=[%d]", send.m_ZhuangPos);
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
		{
			LLOG_DEBUG("GAME_END m_CardData[%d][0]=[%d],m_CardData[%d][1]=[%d]",i,send.m_CardData[i][0],i, send.m_CardData[i][1]);
			LLOG_DEBUG("GAME_END m_GameScore[%d]=[%d]",i, send.m_GameScore[i]);
			LLOG_DEBUG("GAME_END m_Score[%d][0]=[%d],m_Score[%d][1]=[%d],m_Score[%d][2]=[%d],m_Score[%d][3]=[%d]", i,send.m_Score[i][0],i, send.m_Score[i][1],i, send.m_Score[i][2],i, send.m_Score[i][3]);
			LLOG_DEBUG("GAME_END m_ResultScore[%d][0]=[%d],m_ResultScore[%d][1]=[%d],m_ResultScore[%d][2]=[%d],m_ResultScore[%d][3]=[%d]", i, send.m_ResultScore[i][0], i, send.m_ResultScore[i][1], i, send.m_ResultScore[i][2], i, send.m_ResultScore[i][3]);
	
		}

		for (Lint i = 0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			if (m_desk && m_desk->m_vip)
			{
				send.m_TotalScore[i] = m_desk->m_vip->m_score[i];
				LLOG_DEBUG("send.m_TotalScore[%d]=[%d]", i, send.m_TotalScore[i]);
			}
			else
				LLOG_ERROR("m_desk or  m_desk->m_vip is null ,TuiTongZiEndGameRound");
		}


		notify_desk(send);

	
		set_desk_state(DESK_WAIT);
		if (m_desk) m_desk->setDynamicToPlay();
		//结束游戏
		if (m_desk) m_desk->HanderGameOver(1);
		


		//switch (result)
		//{
		//case GAMG_CONCLUDE_SYSTEM_COMPARECARD:  //系统比牌结束
		//case GAMG_CONCLUDE_COMPARECARD:	//比牌结束
		//case GAMG_CONCLUDE_NOPLAYER:		//没有玩家
		//{
		//	if (result!=GAMG_CONCLUDE_SYSTEM_COMPARECARD && m_GameEnd)return;
		//	m_GameEnd=true;
		//
		//	//定下局�?-最后大赢家为下局�?
		//	//m_zhuangpos=0;
		//	
		//
		//	m_zhuangpos=winpos;
		//	Lint wWinner=winpos;  //最后大赢家
		//	Lint  lWinnerScore=0;              //计算大赢家总注
		//	LLOG_DEBUG("finish_round   winPos=[%d]", winpos);
		//
		//	//结束数据
		//	YingSanZhangEndGameRound GameEnd;
		//
		//	GameEnd.m_FinalDw=m_dismissed||m_round_offset>=m_round_limit;
		//
		//	//统计分数
		//	for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//	{
		//		//获取用户
		//		if (i==wWinner)continue;
		//		if (m_user_status[i]!=1)continue;
		//
		//		GameEnd.m_GameScore[i]=-m_TableScore[i];
		//		//添加特殊加减分项
		//		GameEnd.m_GameScore[i]+=(-1)*spec_add_score(wWinner);
		//		lWinnerScore+=(-1)*(GameEnd.m_GameScore[i]);
		//	}
		//	GameEnd.m_GameScore[wWinner]=lWinnerScore;
		//
		//	// 保存录像
		//	VideoSave();
		//	add_round_log(GameEnd.m_GameScore, wWinner);
		//
		//	//记录总分
		//	for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//	{
		//		if (m_desk && m_desk->m_vip)
		//		{
		//			GameEnd.m_TotalScore[i]=m_desk->m_vip->m_score[i];
		//			LLOG_DEBUG("GameEnd.m_TotalScore[%d]=[%d]", i, GameEnd.m_TotalScore[i]);
		//		}
		//		else
		//			LLOG_ERROR("m_desk or  m_desk->m_vip is null ,YingSanZhangEndGameRound");
		//	}
		//
		//	//比牌数组
		//	for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//	{
		//		if (m_user_status[i]!=1)continue;
		//		auto ItUser=m_CompardUser.find(i);
		//		if (ItUser!=m_CompardUser.end())
		//		{
		//			Lint count=0;
		//			for (auto ItCompareUser=ItUser->second.begin(); count<4&&ItCompareUser!=ItUser->second.end(); ItCompareUser++)
		//			{
		//				GameEnd.m_CompareUser[i][count++]=(*ItCompareUser);
		//			}
		//		}
		//		else
		//		{
		//			//LLOG_DEBUG("m_CompardUser not find [%d]", i);
		//		}
		//	}
		//
		//	//结束信息
		//	for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//	{
		//		LLOG_DEBUG("i=[%d]---------------------------------------------------------------------wWinner=[%d]", i, wWinner);
		//		GameEnd.reset_part();
		//		//大赢�?
		//		if (wWinner<YINGSANZHANG_PLAY_USER_MAX)
		//		{
		//			for (int cardNum=0; cardNum<YINGSANZHANG_HAND_CARD_MAX; cardNum++)
		//				GameEnd.m_CardData[wWinner][cardNum]=(Lint)m_hand_card_data[wWinner][cardNum];
		//			GameEnd.m_CardType[wWinner]=(Lint)m_HandCardType[wWinner];
		//		}
		//
		//		if (m_user_status[i]!=1)
		//		{
		//			if (delaySend)
		//				memcpy(m_StGameEnd+i, &GameEnd, sizeof(YingSanZhangEndGameRound));
		//			else
		//				notify_user(GameEnd, i);
		//			continue;
		//		}
		//
		//		//自己
		//		if (i!=wWinner)
		//		{
		//			for (int cardNum=0; cardNum<YINGSANZHANG_HAND_CARD_MAX; cardNum++)
		//				GameEnd.m_CardData[i][cardNum]=(Lint)m_hand_card_data[i][cardNum];
		//			GameEnd.m_CardType[i]=(Lint)m_HandCardType[i];
		//		}
		//		//比牌�?
		//		for (Lint j=0; j<YINGSANZHANG_PLAY_USER_MAX-1; j++)
		//		{
		//			if (GameEnd.m_CompareUser[i][j]==INVAILD_POS)continue;
		//			Lint userChairId=GameEnd.m_CompareUser[i][j];
		//			if (userChairId<0||userChairId>=YINGSANZHANG_PLAY_USER_MAX)continue;
		//
		//			//if (userChairId==wWinner)continue;
		//			for (int cardNum=0; cardNum<YINGSANZHANG_HAND_CARD_MAX; cardNum++)
		//			{
		//				GameEnd.m_CardData[userChairId][cardNum]=(Lint)m_hand_card_data[userChairId][cardNum];
		//
		//			}
		//			GameEnd.m_CardType[userChairId]=(Lint)m_HandCardType[userChairId];
		//		}
		//		if (delaySend)
		//		{
		//			memcpy(m_StGameEnd+i, &GameEnd, sizeof(YingSanZhangEndGameRound));
		//		}
		//		else
		//			notify_user(GameEnd, i);
		//	}
		//
		//
		//	//if (!delaySend)
		//	//{
		//	//	set_desk_state(DESK_WAIT);
		//	//	//结束游戏
		//	//	if (m_desk) m_desk->HanderGameOver(1);
		//	//}
		//	return;
		//
		//}
		//case GAMG_CONCLUDE_DISMISS:
		//{
		//	LLOG_DEBUG("GAMG_CONCLUDE_DISMISS");
		//	//结束数据
		//	YingSanZhangEndGameRound GameEnd;
		//	for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//	{
		//		if (m_desk && m_desk->m_vip)
		//			GameEnd.m_TotalScore[i]=m_desk->m_vip->m_score[i];
		//		else
		//			LLOG_ERROR("m_desk or  m_desk->m_vip is null ,YingSanZhangEndGameRound GAMG_CONCLUDE_DISMISS");
		//	}
		//	GameEnd.m_FinalDw=1;
		//
		//	notify_desk(GameEnd);
		//
		//	set_desk_state(DESK_WAIT);
		//	//结束游戏
		//	if (m_desk) m_desk->HanderGameOver(1);
		//	return;
		//}
		//default:
		//{
		//	LLOG_ERROR("finish_round  reason error");
		//	break;
		//}
		//
		//}   //end switch
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//子游戏事�?
	//////////////////////////////////////////////////////////////////////////
	//加注
	bool on_event_user_add_score(Lint pos, Lint ya, Lint dao,Lint ruan ,Lint hong)
	{
		LLOG_ERROR("deskId=[%d]on_event_user_add_score,pos[%d],ya[%d],dao[%d],ruan[%d],hong[%d]",m_desk?m_desk->GetDeskId():0, pos, ya, dao, ruan,hong);
		
		if (ya==0)
		{
			LLOG_DEBUG("on_event_user_add_score error !!!!  ya==0 error pos[%d]", pos);
			return false;
		}

		m_score[pos][0]=ya;
		m_score[pos][1]=dao;
		m_score[pos][2]=ruan;
		m_score[pos][3]=hong;

		TuiTongZiS2CScore send;
		send.m_pos = pos;
		memcpy(send.m_score, m_score[pos], sizeof(send.m_score));
		notify_desk(send);

		if (add_score_over())
		{
			for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
			{
				if (m_user_status[i] != 1)continue;
				send_user_card(i);
			}
			//结算分数
			accum_score();
		}


		return true;
	}
	//选庄
	bool on_event_user_select_zhuang(Lint pos, Lint yes)
	{
		LLOG_ERROR("deskId=[%d] on_event_user_select_zhuang pos=[%d],yes=[%d]",m_desk?m_desk->GetDeskId():0,pos,yes);
		//if (m_zhuangConfirm)
		//{
		//	LLOG_ERROR("HanderTuiTongZiSelectZhuang error!!! m_zhuangConfirm=true");
		//	return false;
		//}
		//选庄
		switch (m_playtype.TuiTongZiGetGameMode())
		{
		case GAME_MODE_QIANG_ZHUANG:
		{
			LLOG_DEBUG("GAME_MODE_QIANG_ZHUANG");
			if (m_selectZhuang[pos] != 0)
			{
				LLOG_ERROR("on_event_user_select_zhuang qiang error ,userPos[%d]  already select [%d]", pos,m_selectZhuang[pos]);
				return false;
			}
			if (yes == 0)
			{
				m_selectZhuang[pos] = 2;
				notify_no_zhuang(pos);
			}
			else
			{
				m_selectZhuang[pos] = 1;
				notify_yes_zhuang(pos);
			}

			if (select_zhuang_over())
			{
				m_zhuangPos=qiang_zhuang();
				set_zhuang(m_zhuangPos);
			}

			break;
		}
		case GAME_MODE_BAWANG_ZHUANG:
		{
			LLOG_DEBUG("GAME_MODE_BAWANG_ZHUANG");
		}
		case GAME_MODE_LUN_ZHUANG:
		{
			LLOG_DEBUG("GAME_MODE_LUN_ZHUANG");
			if (pos != m_currSelectZhuang)
			{
				LLOG_ERROR("UserPos[%d] is not  m_currSelectZhuang[%d]",pos, m_currSelectZhuang);
				return false;
			}
			if (m_round_offset==0)
			{
				//用户选庄
				if (m_selectZhuang[pos]==0)
				{
					if (yes==0)
					{
						m_selectZhuang[pos]=2;
						notify_no_zhuang(pos);
						if (select_zhuang_over())
						{
							m_zhuangPos=rand_zhuang();
							set_zhuang(m_zhuangPos);
						}
						else
						{
							send_zhuang_cmd(GetNextPos(pos));
							m_play_status_time.Now();
							
						}

					}
					else
					{
						m_selectZhuang[pos]=1;
						m_zhuangPos=pos;
						notify_yes_zhuang(pos);
						set_zhuang(m_zhuangPos);
						return true;
					}
				}
				else
				{
					LLOG_ERROR("HanderTuiTongZiSelectZhuang GAME_MODE_LUN_ZHUANG userPos[%d] already select zhuang[%d]",pos, m_selectZhuang[pos]);
				}
			}
		
			break;
		}
		default:
		{
			LLOG_ERROR("m_playtype.gametypeGetGameMode ERROR undefined [%d]", m_playtype.TuiTongZiGetGameMode());
			break;
		}

		}
	}
	//开�?
	bool on_event_user_open_card(Lint pos)
	{
		LLOG_ERROR("deskId=[%d]on_event_user_open_card pos=[%d]",m_desk?m_desk->GetDeskId():0, pos);
		m_openCard[pos]=1;
		send_user_card(pos, true);
		if (open_card_over())
		{
			//结算
			finish_round();
		}
		return true;
	}
	//筛子
	bool on_event_do_shai_zi()
	{
		//m_play_status = GAME_PLAY_ADD_SCORE;
		set_play_status(GAME_PLAY_ADD_SCORE);
		m_play_status_time.Now();
		TuiTongZiS2CDoShaiZi send;
		send.m_type = 0;
		send.m_dian1 = m_shaiZi.m_dian1;
		send.m_dian2 = m_shaiZi.m_dian2;
		send.m_pos = m_shaiZi.m_pos;
		notify_desk(send);
	
		return true;
	}
	
};


struct TuiTongZiGameHandler : TuiTongZiGameCore {

	void notify_user(LMsg &msg, int pos) override {
		if (NULL==m_desk) return;
		if (pos < 0||pos>=m_player_count) return;
		User *u=m_desk->m_user[pos];
		if (NULL==u) return;
		u->Send(msg);
	}

	void notify_desk(LMsg &msg) override {
		if (NULL==m_desk) return;
		m_desk->BoadCast(msg);
	}

	void notify_desk_playing_user(LMsg &msg) {
		if (NULL==m_desk) return;
		m_desk->BoadCast2PlayingUser(msg);
	}

	void notify_desk_without_user(LMsg &msg, User* pUser)
	{
		if (NULL==m_desk) return;
		m_desk->BoadCastWithOutUser(msg, pUser);
	}

	bool startup(Desk *desk) {
		if (NULL==desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk=desk;
		return true;
	}

	void shutdown(void) {
		clear_match(0);
	}

	void SetPlayType(std::vector<Lint> &l_playtype) {
		// �?Desk �?SetVip 调用，此时能得到 m_vip->m_maxCircle
		int round_limit=m_desk && m_desk->m_vip?m_desk->m_vip->m_maxCircle:0;
		setup_match(l_playtype, round_limit);
	}

	void MHSetDeskPlay(Lint PlayUserCount, Lint * PlayerStatus, Lint PlayerCount) {
		LLOG_DEBUG("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		if (!m_desk||!m_desk->m_vip) {
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}
		if (m_playtype.TuiTongZiGet2or8() == 1)
		{
			if (PlayUserCount != TUITONGZI_PLAY_USER_COUNT || PlayerStatus == NULL || PlayerCount != TUITONGZI_PLAY_USER_COUNT)
			{
				LLOG_ERROR("MHSetDeskPlay  error  !!!!PlayUserCount=[%d] ", PlayUserCount);
				return;
			}
		}
		else
		{
			if (PlayUserCount != TUITONGZI_JINBEI_4_USER || PlayerStatus == NULL || PlayerCount != TUITONGZI_JINBEI_4_USER)
			{
				LLOG_ERROR("MHSetDeskPlay  error  !!!!PlayUserCount=[%d] !=[%d] ", PlayUserCount, TUITONGZI_JINBEI_4_USER);
				return;
			}
		}

		m_player_count=PlayUserCount;
		//memcpy(m_user_playing_status, PlayerStatus, sizeof(m_user_playing_status));
		//
		//for(int i=0; i<YINGSANZHANG_PLAY_USER_MAX;i++)
		//	LLOG_DEBUG("m_user_playing_status[%d]=[%d]",i, m_user_playing_status[i]);

		start_round(PlayerStatus);
	}

	//游戏结束
	void OnGameOver(Lint result, Lint winpos, Lint bombpos) {
		if (m_desk==NULL||m_desk->m_vip==NULL) {
			LLOG_ERROR("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed=!!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d", m_desk?m_desk->GetDeskId():0, m_round_limit, m_round_offset, m_dismissed);
		finish_round();
	}

	//游戏中断线重�?
	void OnUserReconnect(User *pUser) {
		if (pUser==NULL||m_desk==NULL) {
			return;
		}

		// 发送当前圈数信�?
		// 为什么要发给桌子，而不是只发给这个短线重连的玩家？
		notify_desk_match_state();

		Lint pos=GetUserPos(pUser);
		MHLOG_PLAYCARD("*******断线重连desk:%d userid:%d pos:%d m_curPos=%d ",
			m_desk->GetDeskId(), pUser->GetUserDataId(), pos, m_curPos);

		if (pos == TUITONGZI_INVAILD_POS)
		{
			LLOG_ERROR("OnUserReconnect user[%d] pos error!!!",pUser->GetUserDataId());
			return;
		}

		TuiTongZiS2CRecon reconn;
		reconn.m_play_status = m_play_status;

		//新加字段
		if (m_user_status[pos] == 1)
			reconn.m_DynamicJoin = 0;
		else
			reconn.m_DynamicJoin = 1;

		switch (m_play_status)
		{
		case GAME_PLAY_SEND_CARD:
		{
			if (m_openCard[pos])
				reconn.m_isOpenCard = 1;
			for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
			{
				if (m_user_status[i] != 1)continue;
				if (m_openCard[i] || (i==pos && m_playtype.TuiTongZiGetCuoPai()==1) )
				{
					for (Lint card = 0; card < TUITONGZI_HAND_CARD_MAX; card++)
					{
						reconn.m_CardData[i][card] = (Lint)m_hand_card_data[i][card];
					}
				}
					//memcpy(reconn.m_CardData[i], m_hand_card_data[i], sizeof(Lint)*TUITONGZI_HAND_CARD_MAX);
			}
			
		}
		case GAME_PLAY_ADD_SCORE:
		{
			memcpy(reconn.m_score, m_score, sizeof(Lint)*TUITONGZI_PLAY_USER_COUNT*TUITONGZI_XIAZHU_XIANG);
			if (m_score[pos][0] != 0)
				reconn.m_isScore = 1;
		}
		case GAME_PLAY_DO_SHAI_ZI:
		{
			LLOG_DEBUG("OnUserReconnect:GAME_PLAY_DO_SHAI_ZI");
			reconn.m_zhuangPos = m_zhuangPos;
			//reconn.m_shaiZi = 1;
		}
		case GAME_PLAY_SELECT_ZHUANG:
		{
			LLOG_DEBUG("OnUserReconnect:GAME_PLAY_SELECT_ZHUANG");
			memcpy(reconn.m_userZhuang, m_selectZhuang, sizeof(reconn.m_userZhuang));
			reconn.m_currSelectPos = m_currSelectZhuang;
			switch (m_playtype.TuiTongZiGetGameMode())
			{
			case GAME_MODE_QIANG_ZHUANG:
			{
				reconn.m_isSelectZhuang = m_selectZhuang[pos];
				break;
			}
			case GAME_MODE_BAWANG_ZHUANG:
			case GAME_MODE_LUN_ZHUANG:
			{
				if (m_round_offset == 0)
					reconn.m_isSelectZhuang =(m_currSelectZhuang == pos?1: 0);
				else
				{
					reconn.m_isSelectZhuang = 1;
					reconn.m_zhuangPos = m_zhuangPos;
				}
				break;
			}
			}
		}
		
		}
		LTime curTime;
		Lint time_delay = 0;
		switch (m_play_status)
		{
		case GAME_PLAY_SEND_CARD:
			time_delay = (Lint)DELAY_TIME_OPEN_CARD;
			break;
		case GAME_PLAY_ADD_SCORE:
			time_delay = (Lint)DELAY_TIME_ADD_SCORE - 7;
			break;
		case GAME_PLAY_DO_SHAI_ZI:
			time_delay = (Lint)DELAY_TIME_DO_SHAI_ZI;
			break;
		case GAME_PLAY_SELECT_ZHUANG:
			time_delay = (Lint)DELAY_TIME_SELECT_ZHUANG;
			if (m_playtype.TuiTongZiGetGameMode() == GAME_MODE_QIANG_ZHUANG)
			{
				time_delay = (Lint)DELAY_TIME_SELECT_QIANG_ZHUANG;
			}
			break;
		default:
			break;
		}

		Lint  time_escaped = curTime.Secs() - m_play_status_time.Secs();
		Lint  time_remain_in_second = 0;
		time_remain_in_second = time_delay - time_escaped;
		if (time_remain_in_second < 0)  time_remain_in_second = 0;
		if (time_remain_in_second > time_delay) time_remain_in_second = time_delay;
		LLOG_ERROR("desk:%d 断线重连 delay:%d  escap:%d remain:%d", m_desk ? m_desk->GetDeskId() : 0, time_delay, time_escaped, time_remain_in_second);
		//Lint ret_time = time(NULL) - m_play_status_time.Secs();
		Lint ret_time = time_remain_in_second;
		if (ret_time >= 0 && ret_time < 60)
			reconn.m_remain_time = ret_time;
		notify_user(reconn, pos);

		////m_needGetCard，m_curGetCard?ToTile(m_curGetCard):0, m_curOutCard?ToTile(m_outCard):0);
		//Lint nCurPos=m_curPos;
		//// 如果在一盘结束那个地方重连，该是个什么情况？
		////Lint nDeskPlayType=m_play_state;
		//LLOG_DEBUG("Desk::OnUserReconnect send out");
		//YingSanZhangS2CPlayScene reconn;
		//
		////新加字段
		//if (m_user_status[pos]==1)
		//	reconn.m_DynamicJoin=0;
		//else
		//	reconn.m_DynamicJoin=1;
		//
		//reconn.m_TableTotalScore=m_tableScoreTotal;
		//
		////用户弃牌输牌状�?
		//for (WORD i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//{
		//	if (m_GiveUpUser[i])
		//	{
		//		reconn.m_UserStatus[i]=1;
		//		continue;
		//	}
		//	if (m_LostUser[i])
		//	{
		//		reconn.m_UserStatus[i]=2;
		//		continue;
		//	}
		//}
		////桌面筹码情况
		//memcpy(reconn.m_TableScoreCount, m_tableScoreCount, sizeof(m_tableScoreCount));
		//
		//reconn.m_AutoScore=m_AutoScore[pos];
		//
		////加注信息
		//reconn.m_MaxCellScore=0;
		//reconn.m_CellScore=m_CurrentTimes;
		//reconn.m_CurrentTimes=m_CurrentTimes;
		////reconn.m_UserMaxScore=m_UserMaxScore[pos];
		//
		////设置变量
		//reconn.m_BankerUser=m_zhuangpos;
		//reconn.m_CurrentUser=m_curPos;
		//for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//{
		//	reconn.m_MingZhu[i]=m_MingZhu[i];
		//	reconn.m_TableScore[i]=m_TableScore[i];
		//	reconn.m_PlayStatus[i]=m_user_status[i];
		//}
		//reconn.m_CompareState=m_GameEnd;
		//
		////设置扑克
		//if (m_MingZhu[pos]&&m_user_playing_status[pos])
		//{
		//	for (int i=0; i<YINGSANZHANG_HAND_CARD_MAX; i++)
		//		reconn.m_HandCardData[i]=(Lint)m_hand_card_data[pos][i];
		//
		//	reconn.m_CardType=m_HandCardType[pos];
		//}
		//
		//notify_user(reconn, pos);
		//
		////广播轮数
		//YingSanZhangS2CLun send;
		//send.m_value=m_curLun;
		//notify_user(send, pos);

	}

	bool HanderTuiTongZiAddScore(User* pUser, TuiTongZiC2SScore*msg)
	{
		TuiTongZiC2SScore* pAddScore=msg;
		if (pUser==NULL||pAddScore==NULL)return false;

		////检查位置和状�?
		LLOG_DEBUG("deskId=[%d] HanderTuiTongZiAddScore: userPos = [%d],ya=[%d],dao=[%d],ruan=[%d],hong=[%d]", m_desk?m_desk->GetDeskId():0,GetUserPos(pUser), pAddScore->m_Ya, pAddScore->m_Dao, pAddScore->m_Ruan,pAddScore->m_Hong);

		if (m_play_status != GAME_PLAY_ADD_SCORE)
		{
			LLOG_ERROR("HanderTuiTongZiAddScore error, user[%d] , status not GAME_PLAY_ADD_SCORE, [%d]",pUser->GetUserDataId(),m_play_status);
			return false;
		}

		if (GetUserPos(pUser)==TUITONGZI_INVAILD_POS)
		{
			LLOG_ERROR("HanderTuiTongZiAddScore  user[%d] pos not ok [%d] ", pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		////消息处理
		return on_event_user_add_score(GetUserPos(pUser), pAddScore->m_Ya, pAddScore->m_Dao, pAddScore->m_Ruan, pAddScore->m_Hong);
	}

	bool HanderTuiTongZiSelectZhuang(User* pUser, TuiTongZiC2SSelectZhuang*msg)
	{
		LLOG_DEBUG("HanderTuiTongZiSelectZhuang  ....");
		if (pUser==NULL||msg==NULL)
		{
			LLOG_ERROR("HanderTuiTongZiSelectZhuang  error,  pUser==NULL or msg==NULL");
			return false;
		}
		if (m_play_status != GAME_PLAY_SELECT_ZHUANG)
		{
			LLOG_ERROR("HanderTuiTongZiSelectZhuang error, user[%d] , status not GAME_PLAY_SELECT_ZHUANG, [%d]", pUser->GetUserDataId(), m_play_status);
			return false;
		}

		Lint pos=GetUserPos(pUser);
		if (pos==TUITONGZI_INVAILD_POS)
		{
			LLOG_ERROR("HanderTuiTongZiSelectZhuang GAME_MODE_LUN_ZHUANG  user pos error!!!  userId[%d] ", pUser->GetUserDataId());
			return false;
		}

		on_event_user_select_zhuang(pos, msg->m_value);
		return true;
	}

	bool HanderTuiTongOpenCard(User* pUser, TuiTongZiC2SOpenCard*msg)
	{
		if (pUser==NULL||msg==NULL)return false;

		if (m_play_status != GAME_PLAY_SEND_CARD)
		{
			LLOG_ERROR("HanderTuiTongOpenCard error, user[%d] , status not GAME_PLAY_SEND_CARD, [%d]", pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser)==TUITONGZI_INVAILD_POS)
		{
			LLOG_ERROR("HanderTuiTongOpenCard  user[%d] pos not ok [%d] ", pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		on_event_user_open_card(GetUserPos(pUser));
	}

	bool HanderTuiTongZiDoShaiZi(User* pUser, TuiTongZiC2SDoShaiZi*msg)
	{
		if (pUser==NULL)
		{
			LLOG_ERROR("HanderTuiTongZiDoShaiZi  error,  pUser==NULL or msg==NULL");
			return false;
		}
		if (m_play_status != GAME_PLAY_DO_SHAI_ZI)
		{
			LLOG_ERROR("HanderTuiTongZiDoShaiZi error, user[%d] , status not GAME_PLAY_DO_SHAI_ZI, [%d]", pUser->GetUserDataId(), m_play_status);
			return false;
		}

		Lint pos=GetUserPos(pUser);
		if (pos==TUITONGZI_INVAILD_POS)
		{
			LLOG_ERROR("HanderTuiTongZiDoShaiZi   user pos error!!!  userId[%d] ", pUser->GetUserDataId());
			return false;
		}
		if (pos!=m_zhuangPos)
		{
			LLOG_ERROR("HanderTuiTongZiDoShaiZi error!!! pos[%d] not equal zhuang[%d]", pos, m_zhuangPos);
			return false;
		}
		on_event_do_shai_zi();

		return true;
	}

	// 桌子解散失败结束事件
   void   OnResetDeskEnd()
   {
	   if (m_playtype.TuiTongZiGetAuto())
	   {
	        m_play_status_time.Now();
		   LLOG_ERROR("Desk:%d OnResetDeskEnd 重置定时器时�? %d", m_desk ? m_desk->GetDeskId() : 0,  m_play_status_time.Secs());
	   }
   }

	void Tick(LTime& curr)
	{
		if (m_playtype.TuiTongZiGetAuto() == 0)return;
		if (m_desk && m_desk->m_resetTime > 0 && m_desk->m_resetUserId > 0)
		{
			LLOG_ERROR("Desk:%d TuiTongZiGameHandler--Tick 房间解散�? 不做处理", m_desk->GetDeskId());
			return;
		}
		
		LLOG_DEBUG("TuiTongZiGameHandler--Tick,  play_status=[%d]",m_play_status);
		switch (m_play_status)
		{
		case GAME_PLAY_SELECT_ZHUANG:
		{
			switch (m_playtype.TuiTongZiGetGameMode())
			{
			case GAME_MODE_QIANG_ZHUANG:
			{
				if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_SELECT_QIANG_ZHUANG)
				{
					LLOG_DEBUG("GAME_MODE_QIANG_ZHUANG time over!");
					for (Lint i=0; i < TUITONGZI_PLAY_USER_COUNT; i++)
					{
						if(m_selectZhuang[i]==0 && m_user_status[i]==1)
							on_event_user_select_zhuang(i, 0);
					}
				}
				break;
			}
			case GAME_MODE_BAWANG_ZHUANG:
			case GAME_MODE_LUN_ZHUANG:
			{
				if (m_round_offset != 0)return;
				if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_SELECT_ZHUANG)
				{
					LLOG_DEBUG("GAME_PLAY_SELECT_ZHUANG time over!");
					on_event_user_select_zhuang(m_currSelectZhuang, 0);
				}
			}
			
			}
		
			break;
		}
		case GAME_PLAY_DO_SHAI_ZI:
		{
			LLOG_DEBUG("GAME_PLAY_DO_SHAI_ZI time over!");
			if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_DO_SHAI_ZI)
				on_event_do_shai_zi();
			break;
		}
		case GAME_PLAY_ADD_SCORE:
		{
			LLOG_DEBUG("GAME_PLAY_ADD_SCORE time over!");
			if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_ADD_SCORE)
			{
				for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
				{
					if (i == m_zhuangPos)continue;
					if (m_score[i][0] == 0 &&  m_user_status[i] == 1)
					{
						Lint ya = 2;
						switch (m_playtype.TuiTongZiGetScoreTime())
						{
						case 1:
						{
							ya = 2;
							break;
						}
						case 2:
						{
							ya = 5;
							break;
						}
						case 3:
						{
							ya = 8;
							break;
						}
						}

						on_event_user_add_score(i, ya, 0, 0, 0);
					}
				}
			}
			break;
		}
		case GAME_PLAY_SEND_CARD:
		{
			LLOG_DEBUG("GAME_PLAY_SEND_CARD time over!");
			if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_OPEN_CARD)
			{
				for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; i++)
				{
					if (m_openCard[i] == 0 && m_user_status[i] == 1)
						on_event_user_open_card(i);
				}
			}
			break;
		}
		case GAME_PLAY_END:
		{
			break;
		}

		}
	}

};


DECLARE_GAME_HANDLER_CREATOR(104, TuiTongZiGameHandler);

//// 各个玩法�?ShanXiGameHandlerCreator 作为全局变量，将自己注册到工厂里面就行了，不用管注销
//// 工厂实际上就是个全局�?GameType �?GameHandlerCreator 的映�?
//// 发现工厂�?GameHandlerCreator 创建出来�?GameHandler 都不带删除的，创建多少缓存多�?
//struct ShanXiGameHandlerCreator : GameHandlerCreator {
//	GameType game_type;
//	ShanXiGameHandlerCreator(GameType game_type) : game_type(game_type) {
//		GameHandlerFactory::getInstance()->registerCreator(game_type, this);
//	}
//
//	GameHandler *create() {
//		TuiTongZiGameHandler *gh=new TuiTongZiGameHandler();
//		//gh->setup_feature_toggles(game_type);
//		return gh;
//	}
//};
//
////struct TuoTongZiGameHandlerCreator : GameHandlerCreator {
////	GameType game_type;
////	TuoTongZiGameHandlerCreator(GameType game_type) : game_type(game_type) {
////		GameHandlerFactory::getInstance()->registerCreator(game_type, this);
////	}
////
////	GameHandler *create() {
////		TuiTongZiGameHandler *gh=new TuiTongZiGameHandler();
////		//gh->setup_feature_toggles(game_type);
////		return gh;
////	}
////};
//
//
//#define XX(k, v, player_count) ShanXiGameHandlerCreator g_ShanXiGameHandlerCreator__##k(k);
//POKER_GAME_TYPE_MAP(XX)
//#undef XX