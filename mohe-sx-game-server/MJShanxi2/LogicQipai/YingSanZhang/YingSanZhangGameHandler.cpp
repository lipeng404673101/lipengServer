#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "YingSanZhangGameLogic.h"
#include "LMsgL2C.h"

enum GAMG_CONCLUDE_REASON
{
	GAMG_CONCLUDE_COMPARECARD,
	GAMG_CONCLUDE_NOPLAYER,
	GAMG_CONCLUDE_SYSTEM_COMPARECARD,
	GAMG_CONCLUDE_DISMISS
};


enum YING_SAN_ZHANG_PLAY_STATUS_DELAY_TIME
{
	DELAY_TIME_FLUSH_TIME=3,
	DELAY_TIME_START_FLUSH_TIME=4,
	DELAY_TIME_ADD_SCORE = 31,                                  //操作倒计时 30秒
	DELAY_TIME_BIG_SMALL = 10, 
	DELAY_TIME_ADD_SCORE_BIG_HAPPY=16,           //操作倒计时  15秒
	DELAY_TIME_ADD_SCORE_10 = 11,                         //操作倒计时   10秒

};


enum YING_SAN_ZHANG_PLAY_USER_COUNT
{
	YINGSANZHANG_PLAY_USER_5=5,
	YINGSANZHANG_PLAY_USER_8=8,
};

enum  YING_SAN_ZHUANG_PLAY_STATUS
{
	GAME_STATUS_PLAY=0,
	GAME_STATUS_BUY_SAMLL,
	GAME_STATUS_BUY_BIG,
};

//std::vector<std::string> splitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored)
//{
//	std::vector<std::string> resultStringVector;
//	std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}/*pred*/, delimStr.at(0));//将出现的所有分隔符都替换成为一个相同的字符（分隔符字符串的第一个）  
//	size_t pos = srcStr.find(delimStr.at(0));
//	std::string addedString = "";
//	while (pos != std::string::npos) {
//		addedString = srcStr.substr(0, pos);
//		if (!addedString.empty() || !repeatedCharIgnored) {
//			resultStringVector.push_back(addedString);
//		}
//		srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
//		pos = srcStr.find(delimStr.at(0));
//	}
//	addedString = srcStr;
//	if (!addedString.empty() || !repeatedCharIgnored) {
//		resultStringVector.push_back(addedString);
//	}
//	return resultStringVector;
//}

//玩法小选项
struct PlayType
{
public:
	void clear() { m_playtype.clear(); }
	void setAllTypes(const std::vector<Lint>& v) { m_playtype=v; }
	const std::vector<Lint>& getAllTypes() const { return m_playtype; }

	Lint   gametypeGetGameMode() const;            //0游戏模式             1：普通模式   2：大牌模式
	Lint   gametypeGetScoreRound() const;         //1下注轮数             实际数值，5--5轮   10--10轮
	Lint   gametypeGetScoreTime() const;          //2实际数值   1，2，3，4
	Lint   gametypeGetMenRound() const;             //3无必闷0， 闷1轮，闷2轮，闷3轮
	Lint   gametypeGetSpecAddScore() const;          //4特殊加分项       用户拿到豹子，同花顺牌型加分 true false
	Lint   gametypeGetAuto() const;                             //5定时器自动操作 0--没有选择 1--选择
	Lint   gametypeGetUserCount() const;                 //6游戏人数 5，8
	Lint   gametypeGetPeiKai() const;                       //7比牌，双倍开  0--没有选择 1--选择
	Lint   gametypeGetTianDiLong() const;            //8天龙地龙       0--没有选择 1--选择
	Lint   gametypeGetDynamic() const;                //9动态加入  0--没有选择 1--选择
	Lint   gametypeGetAddMin() const;              //10买大买小  0--没有选择  1--选择
	Lint   gametypeGetMinBaoziMax() const;   //11买小时，豹子可压所有牌型  0--没有选择  1--选择
	Lint   gametypeGet8Ren() const;                  //12 8人场     0--没有选择  1--选择    ---没有在用
	Lint   gametypeGetGameType() const;      //13 游戏类型  0--经典模式  1--爽翻模式
	Lint   gametypeGetLookOn() const;           //14 随时进入牌局旁观， 0--没有选择 1--选择 
	Lint   gametypeGetAutoDoTime()const;   //15 自动出牌倒计时时间   0--30秒   1--15秒
protected:
	std::vector<Lint>	m_playtype;
};

Lint   PlayType::gametypeGetGameMode() const
{
	if (m_playtype.empty())return 1;
	if (gametypeGetUserCount() == YINGSANZHANG_PLAY_USER_8)return 1;
	return m_playtype[0];
}
Lint   PlayType::gametypeGetScoreRound() const
{
	if (m_playtype.size()<2)return 5;
	return m_playtype[1];
}
Lint   PlayType::gametypeGetScoreTime() const
{
	if (m_playtype.size()<3)return 1;
	//if (m_playtype[2] == 4)return 1;
	return m_playtype[2];
}
Lint   PlayType::gametypeGetMenRound() const
{
	if (m_playtype.size()<4)return 0;
	return m_playtype[3];
}
Lint   PlayType::gametypeGetSpecAddScore() const
{
	if (m_playtype.size()<5)return 0;
	return m_playtype[4];
}
Lint   PlayType::gametypeGetAuto() const
{
	if (m_playtype.size()<6)return 0;
	return m_playtype[5];
}
Lint   PlayType::gametypeGetUserCount() const
{
	if (m_playtype.size()<7)return YINGSANZHANG_PLAY_USER_5;
	if (m_playtype[6] == YINGSANZHANG_PLAY_USER_5 || m_playtype[6] == YINGSANZHANG_PLAY_USER_8)
		return m_playtype[6];
	else
		return YINGSANZHANG_PLAY_USER_5;
}
Lint   PlayType::gametypeGetPeiKai() const
{
	if (m_playtype.size()<8)return 0;
	return  m_playtype[7];
}
Lint   PlayType::gametypeGetTianDiLong() const
{
	if (m_playtype.size()<9)return 0;
	return  m_playtype[8];
}
Lint   PlayType::gametypeGetDynamic() const
{
	if (m_playtype.size()<10)return 1;
	return  m_playtype[9];
}
/* 获取买大买小选项，默认不选 */
Lint   PlayType::gametypeGetAddMin() const
{
	if (m_playtype.size() < 11) return 0;
	return m_playtype[10];
}
Lint  PlayType::gametypeGetMinBaoziMax() const
{
	if (m_playtype.size() < 12) return 0;
	return m_playtype[11];
}
Lint   PlayType::gametypeGet8Ren() const
{
	if (m_playtype.size() < 13) return 0;
	return m_playtype[12];
}
Lint   PlayType::gametypeGetGameType() const
{
	if (m_playtype.size() < 14) return 0;
	return m_playtype[13]==1?1:0;
}
Lint   PlayType::gametypeGetLookOn() const
{
	if (m_playtype.size() < 15) return 0;
	return m_playtype[14] == 1 ? 1 : 0;
}
Lint   PlayType::gametypeGetAutoDoTime()const
{
	if (m_playtype.size() < 16) return 0;
	return m_playtype[15];
}


/* 买大买小记录的数据结构 */
//struct BigSmall
//{
//private:
//	Lint m_pos;
//	Lint m_score;
//
//public:
//	BigSmall();
//
//	BigSmall(Lint pos, Lint score):m_pos(pos),m_score(score)
//	{}
//
//	Lint getPos() const;
//	Lint getScore() const;
//
//	void setPos(Lint pos);
//	void setScore(Lint score);
//};
//
//BigSmall::BigSmall()
//{
//	m_pos = INVAILD_POS_QIPAI;
//	m_score = 0;
//}
//
//Lint BigSmall::getPos() const
//{
//	return m_pos;
//}
//
//Lint BigSmall::getScore() const
//{
//	return m_score;
//}
//
//void BigSmall::setPos(Lint pos)
//{
//	m_pos = pos;
//}
//
//void BigSmall::setScore(Lint score)
//{
//	m_score = score;
//}

// 不会自动初始化的字段放这里，每盘自动重置
struct ShanXiRoundState__c_part {
	Lint			   m_curPos;						//当前操作玩家
	Lint           m_user_status[YINGSANZHANG_PLAY_USER_MAX];                                                                                         //参加游戏的用户
	Lint           m_user_playing_status[YINGSANZHANG_PLAY_USER_MAX];                                                                       //用户状态，1--参与游戏  0-不参与游戏
	BYTE         m_hand_card_data[YINGSANZHANG_PLAY_USER_MAX][YINGSANZHANG_HAND_CARD_MAX];           //用户手牌
	BYTE        m_HandCardType[YINGSANZHANG_PLAY_USER_MAX];                                                                          //手牌牌型

	bool          m_AutoScore[YINGSANZHANG_PLAY_USER_MAX];                                                                                       //自动下注
	bool		   m_MingZhu[YINGSANZHANG_PLAY_USER_MAX];				                                                                           //看明下注
	bool          m_GiveUpUser[YINGSANZHANG_PLAY_USER_MAX];                                                                                    //弃牌
	bool          m_LostUser[YINGSANZHANG_PLAY_USER_MAX];                                                                                          //比牌输

	Lint			m_TableScore[YINGSANZHANG_PLAY_USER_MAX];				                                                           //下注数目,每局用户下注之和 +=
	Lint        m_UserAddScoreCount[YINGSANZHANG_PLAY_USER_MAX];                                                      //用户下注次数
	Lint          m_tableScoreCount[5];                                                                                                                           //桌面筹码分类个数，为断线重连恢复桌面筹码
	Lint          m_CurrentTimes;                                                                                                                     //当前倍数
	Lint          m_tableScoreTotal;                                                                                                                  //总筹码数
	Lint          m_curLun;                                                                                                                                //下注轮数
             
	 
	bool         m_GameEnd;                                                                                                                              //一局结束标志
	bool         m_GameResult;                                                                                                                         //一局结果已出
	Lint          m_CompareCount;                                                                                                                   //比牌状态
	Lint          m_sysCompareCount;                                                                                                             //系统比牌的人数，主要用倒计时

	Lint          m_play_status;                                                                                                                          //游戏状态 ，买小，买大，下注
	
	

	void clear_round() {
		memset(this, 0, sizeof(*this));
		// 上面那句清零不满足要求的话，下面写自定义重置
		//m_GangThink=GangThink_over;
		//m_beforePos=INVAILD_POS_QIPAI;
		//for (Lint i=0; i< DESK_USER_COUNT; i++) {
		//	m_kouCount[i]=-1;
		//	//撵中子：明杠摸牌标记默认为true(**Ren 2017-12-01)
		//	m_getChardFromMAGang[i]=true;
		//}
		//m_play_state=(DESK_PLAY_STATE)-1;
	}
};

// 有构造函数的字段放这里，每盘自动重置
struct ShanXiRoundState__cxx_part {
	std::map<Lint,std::set<Lint> >  m_CompardUser;
	YingSanZhangEndGameRound_s  m_StGameEnd[YINGSANZHANG_PLAY_USER_MAX];
	LTime m_play_status_time;
	bool    m_play_stauts_flag;
	Lint     m_delay_time_count;

	// 买大买小
	Lint                                        m_bigOrSmall;    // 最终确定的买大买小的模式
	Lint                                        m_bigSmallPos;  //最终确定的买大买小的玩家
	Lint                                        m_bigSmallScore;//最终确定的买大买小分数
	std::vector<Lint>     m_buySmall_2;    //买小2倍用户
	std::vector<Lint>     m_buySmall_3;      //买小3倍的用户
	std::vector<Lint>     m_buySamll_0;     // 买小过的用户

	std::vector<Lint>     m_buyBig_3;           //买大3倍的用户
	std::vector<Lint>     m_buyBig_0;           //买大过的用户
	Lint                               m_userBuySmall[YINGSANZHANG_PLAY_USER_MAX];
	Lint                                m_userBuyBig[YINGSANZHANG_PLAY_USER_MAX];


	void clear_round() {
		this->~ShanXiRoundState__cxx_part();
		new (this) ShanXiRoundState__cxx_part;
		m_CompardUser.clear();
		m_play_stauts_flag = false;
		m_delay_time_count = DELAY_TIME_ADD_SCORE;

		m_bigOrSmall = 2;
		m_bigSmallPos = INVAILD_POS_QIPAI;
		m_bigSmallScore = 0;

		m_buySmall_2.clear();
		m_buySmall_3.clear();
		m_buySamll_0.clear();
		m_buyBig_3.clear();
		m_buyBig_0.clear();


		for (Lint i=0; i< YINGSANZHANG_PLAY_USER_MAX; i++) {
			m_StGameEnd[i].reset();
			m_userBuySmall[i] = 255;
			m_userBuyBig[i] = 255;
		//	//m_thinkInfo[i].Reset();
		//	//m_thinkRet[i].Clear();
		//	//m_tingState[i].clear();
		}
	}

	
};

struct ShanXiMatchState__c_part {
	Desk *m_desk;
	int m_round_offset;
	int m_round_limit;
	int m_accum_score[DESK_USER_COUNT];
	bool m_dismissed;

	int m_registered_game_type;
	int m_player_count; // 玩法核心代码所使用的玩家数量字段
	Lint m_zhuangpos; //庄家位置

	void clear_match() {
		memset(this, 0, sizeof(*this));
		// 上面那句清零不满足要求的话，下面写自定义重置
	}
};

struct ShanXiMatchState__cxx_part {
	CGameLogic						m_gamelogic;							//游戏逻辑
	// 初始化赋值一次就可以
	PlayType m_playtype;	// 桌子玩法

	void clear_match() {
		this->~ShanXiMatchState__cxx_part();
		new (this) ShanXiMatchState__cxx_part;
	}
};

struct ShanXiRoundState : ShanXiRoundState__c_part, ShanXiRoundState__cxx_part {
	// 不要在这里加字段
	void clear_round() {
		ShanXiRoundState__c_part::clear_round();
		ShanXiRoundState__cxx_part::clear_round();
	}
};

// 一场Match会有好几盘Round
struct ShanXiMatchState :  ShanXiMatchState__c_part, ShanXiMatchState__cxx_part {
	// 不要在这里加字段
	void clear_match() {
		ShanXiMatchState__c_part::clear_match();
		ShanXiMatchState__cxx_part::clear_match();
	}
};

struct ShanXiDeskState : ShanXiRoundState, ShanXiMatchState {
	void clear_match(int player_count) {
		ShanXiMatchState::clear_match();
		clear_round();
		m_player_count=player_count;
	}

	void clear_round() {
		ShanXiRoundState::clear_round();
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

	void set_play_state(DESK_PLAY_STATE v) {
		//m_play_state=v;
		if (m_desk) m_desk->setDeskPlayState(v);
	}

	void set_desk_state(DESK_STATE v) {
		if (m_desk) {
			if (v==DESK_WAIT) m_desk->SetDeskWait();
			else m_desk->setDeskState(v);
		}
	}
	DESK_STATE get_desk_state() {
		// 桌子就 DESK_PLAY/DESK_WAIT/DESK_FREE 三种状态
		// 单元测试时没有Desk对象，一直是 DESK_PLAY 状态
		if (m_desk) return (DESK_STATE)m_desk->getDeskState();
		else return DESK_PLAY;
	}

	Lint CheckPositionPrior(Lint CurPos, Lint first_pos, Lint second_pos) {
		// 以 CurPos 为原点，计算 first_pos 和 second_pos 到 CurPos 的逆时针距离
		Lint dif1=first_pos-CurPos > 0?first_pos-CurPos:first_pos-CurPos+m_player_count;
		Lint dif2=second_pos-CurPos > 0?second_pos-CurPos:second_pos-CurPos+m_player_count;
		// 选取逆时针距离短的那个
		if (dif1 < dif2) return first_pos;
		else return second_pos;
	}

	int find_primary_winpos(int bombpos, int winpos) {
		if (bombpos==INVAILD_POS_QIPAI) return winpos;
		for (int d=0; d < m_player_count; ++d) {
			int pos=(bombpos+d)%m_player_count;
			//if (m_winConfirmed[pos]) return pos;
		}
		return winpos;
	}
	//取的有效的下个位置
	int GetNextPos(int pos) {
		if (m_player_count==0) return 0; // 避免除零崩溃	
		Lint nextPos=(pos+1)%m_player_count;
		for (int i=0; (i<m_player_count) && (m_user_playing_status[nextPos]!=1); i++)
		{
			nextPos=(nextPos+1)%m_player_count;
		}
		return nextPos;
	}

	int GetUserPos(User* pUser)
	{
		if (pUser==NULL)return INVAILD_POS_QIPAI;
		Lint pos=INVAILD_POS_QIPAI;
		if (m_desk)
			pos= m_desk->GetUserPos(pUser);
		if (pos>=YINGSANZHANG_PLAY_USER_MAX)
			pos=INVAILD_POS_QIPAI;

		return pos;
	}

	bool is_pos_valid(int pos) {
		if (NULL==m_desk) return false;
		return 0<=pos && pos < YINGSANZHANG_PLAY_USER_MAX;
	}

	bool updateGameLun()
	{
		bool updateFalg=true;
		for (WORD i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			if (m_user_playing_status[i]==FALSE)continue;
			if (m_UserAddScoreCount[i]<m_curLun)
				updateFalg=false;
		}
		if (updateFalg)
		{
			m_curLun++;
			if (m_curLun<=m_playtype.gametypeGetScoreRound())
			{
				return true;
			}
		}

		return false;
	}

	bool  CheckLimitLun(Lint  userChairId)
	{
		return (userChairId>=0)&&(userChairId<YINGSANZHANG_PLAY_USER_MAX)&&(m_UserAddScoreCount[userChairId]+1>m_playtype.gametypeGetScoreRound());
	}

	Lint   GetPlayingUserCount()
	{
		Lint wPlayerCount=0;
		for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			if (m_user_playing_status[i]==TRUE) wPlayerCount++;
		}
		return wPlayerCount;
	}

	void SetDelayTime(Lint compare=0)
	{
		Lint add_score_limit = DELAY_TIME_ADD_SCORE;
		if(m_playtype.gametypeGetGameType()==0 && m_playtype.gametypeGetAutoDoTime() ==1)
			add_score_limit = DELAY_TIME_ADD_SCORE_10;
		else if (m_playtype.gametypeGetGameType() == 0 && m_playtype.gametypeGetAutoDoTime() == 2)
			add_score_limit = DELAY_TIME_ADD_SCORE_BIG_HAPPY;

		if (m_playtype.gametypeGetGameType() == 1)
			add_score_limit = DELAY_TIME_ADD_SCORE_BIG_HAPPY;

		m_play_status_time.Now();
		m_play_stauts_flag = true;
		if (compare == 1)
			m_delay_time_count = add_score_limit + DELAY_TIME_FLUSH_TIME;
		else if (compare == 2)
			m_delay_time_count = add_score_limit + DELAY_TIME_START_FLUSH_TIME;
		else if (compare == 3)
			m_delay_time_count = DELAY_TIME_BIG_SMALL + DELAY_TIME_START_FLUSH_TIME;
		else if(compare==4)
			m_delay_time_count = DELAY_TIME_BIG_SMALL;
		else
			m_delay_time_count = add_score_limit;
	}
	void UnsetDelayTime()
	{
		m_play_stauts_flag = false;
	}

};

struct ShanXiVideoSupport : ShanXiDeskState 
{
	QiPaiVideoLog m_video;		//录像

	///////////////////////////////////////////////////////////////////
	// 这块集中存放和录像有关的函数
	///////////////////////////////////////////////////////////////////
	void VideoSave()
	{
		if (NULL == m_desk)
			return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_deskId = m_desk->GetDeskId();
		m_video.m_playType = m_desk->getPlayType();
		m_video.m_flag = m_desk->m_state;
		m_video.m_maxCircle = m_round_offset;

		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		LLOG_DEBUG("videoSave[%s]", video.m_sql.c_str());
		gWork.SendMsgToDb(video);
	}
};

struct YingSanZhangGameCore : GameHandler, ShanXiVideoSupport {

	//记录桌面筹码
	void recordTableScoreCount(WORD chairId, BOOL compareCard=false)
	{

		if (chairId==INVAILD_POS_QIPAI||chairId<0||chairId>=YINGSANZHANG_PLAY_USER_MAX)return;

		BYTE temp=1;
		temp=(m_MingZhu[chairId])?2:1;
		//比牌平开，倍开
		if(m_playtype.gametypeGetPeiKai()==1)
			temp=(compareCard)?temp*2:temp;

		//m_tableScoreCount[(m_CurrentTimes/m_Times)-1]+=temp;
		switch (m_playtype.gametypeGetScoreTime())
		{
		case 1:
			if (m_CurrentTimes==1)m_tableScoreCount[0]+=temp;
			if (m_CurrentTimes==2)m_tableScoreCount[1]+=temp;
			if (m_CurrentTimes==3)m_tableScoreCount[2]+=temp;
			if (m_CurrentTimes==4)m_tableScoreCount[3]+=temp;
			if (m_CurrentTimes==5)m_tableScoreCount[4]+=temp;
			break;
		case 2:
			if (m_CurrentTimes==2)m_tableScoreCount[0]+=temp;
			if (m_CurrentTimes==4)m_tableScoreCount[1]+=temp;
			if (m_CurrentTimes==8)m_tableScoreCount[2]+=temp;
			if (m_CurrentTimes==16)m_tableScoreCount[3]+=temp;
			if (m_CurrentTimes==20)m_tableScoreCount[4]+=temp;
			break;
		case 3:
			if (m_CurrentTimes==3)m_tableScoreCount[0]+=temp;
			if (m_CurrentTimes==6)m_tableScoreCount[1]+=temp;
			if (m_CurrentTimes==12)m_tableScoreCount[2]+=temp;
			if (m_CurrentTimes==18)m_tableScoreCount[3]+=temp;
			if (m_CurrentTimes==24)m_tableScoreCount[4]+=temp;
			break;
		case 4:
			if (m_CurrentTimes==1)m_tableScoreCount[0]+=temp;
			if (m_CurrentTimes==2)m_tableScoreCount[1]+=temp;
			if (m_CurrentTimes==4)m_tableScoreCount[2]+=temp;
			if (m_CurrentTimes==8)m_tableScoreCount[3]+=temp;
			if (m_CurrentTimes==10)m_tableScoreCount[4]+=temp;
			break;
		//case 5:
		//	if (m_CurrentTimes==5)m_tableScoreCount[0]+=temp;
		//	if (m_CurrentTimes==10)m_tableScoreCount[1]+=temp;
		//	if (m_CurrentTimes==15)m_tableScoreCount[2]+=temp;
		//	if (m_CurrentTimes==30)m_tableScoreCount[3]+=temp;
		//	if (m_CurrentTimes==45)m_tableScoreCount[4]+=temp;
		//	break;
		}
	}

	//确定买大买小用户
	void setSmallOrBigUserInfo(Lint smallOrBigFlag, Lint userPos, Lint score,Lint level)
	{
		m_bigOrSmall = smallOrBigFlag;
		m_bigSmallPos = userPos;
		m_bigSmallScore = score;

		m_TableScore[userPos] = score;// m_CurrentTimes;
		m_tableScoreTotal += score;  // m_CurrentTimes;
		m_UserAddScoreCount[userPos]++;

		//记录桌面筹码
		if (level == 1)
		{
				m_tableScoreCount[0] +=2 ;
		}
		else if (level == 2)
		{
			if(m_playtype.gametypeGetScoreTime()==1)
				m_tableScoreCount[0] += 3;
			else
				m_tableScoreCount[0] += 4;
		}

		m_CurrentTimes = score;

		SetDelayTime();
		//recordTableScoreCount(userPos);
	}

	//判断用户是否选择买大买小
	bool is_user_already_buy(Lint pos)
	{
		if (m_play_status == GAME_STATUS_BUY_SAMLL)
		{
			auto Itor_0 = std::find(m_buySamll_0.begin(), m_buySamll_0.end(), pos);
			if (Itor_0 != m_buySamll_0.end())return true;
			auto Itor_2 = std::find(m_buySmall_2.begin(), m_buySmall_2.end(), pos);
			if (Itor_2 != m_buySmall_2.end())return true;
			auto Itor_3 = std::find(m_buySmall_3.begin(), m_buySmall_3.end(), pos);
			if (Itor_3 != m_buySmall_3.end())return true;
		}
		if (m_play_status == GAME_STATUS_BUY_BIG)
		{
			auto Itor_0 = std::find(m_buyBig_0.begin(), m_buyBig_0.end(), pos);
			if (Itor_0 != m_buyBig_0.end())return true;
			auto Itor_3 = std::find(m_buyBig_3.begin(), m_buyBig_3.end(), pos);
			if (Itor_3 != m_buyBig_3.end())return true;
		}
		return false;
	}

	//判断买大买小是否结束,返回值 ==0 买大小阶段结束；   == 1，切换模式  ； ==2 当前模式未结束 
	Lint is_big_small_over()
	{
		if (m_play_status == GAME_STATUS_BUY_SAMLL)
		{
			if (m_buySmall_3.size() != 0)
			{
				LLOG_DEBUG("YingSanZhangS2CStartAddScore");
				YingSanZhangS2CStartAddScore send;
				send.m_flag = 0;
				switch (m_playtype.gametypeGetScoreTime())
				{
				case 1:
				{
					send.m_score = 3;
					break;
				}
				case 2:
				{
					send.m_score = 8;
					break;
				}
				case 3:
				{
					send.m_score = 12;
					break;
				}
				case 4:
				{
					send.m_score = 4;
					break;
				}

				}
				send.m_totalScore = m_tableScoreTotal + send.m_score;
				send.m_bigSmallPos = m_buySmall_3[0];
				send.m_addScorePos = GetNextPos(m_buySmall_3[0]);
				m_curPos = send.m_addScorePos;
				if (send.m_bigSmallPos < YINGSANZHANG_PLAY_USER_MAX && m_desk && m_desk->m_user[send.m_bigSmallPos] != NULL)
					send.m_nike = m_desk->m_user[send.m_bigSmallPos]->m_userData.m_nike;

				notify_desk(send);

				setSmallOrBigUserInfo(0, m_buySmall_3[0], send.m_score, 2);

				return 0;
			}
			if ( (m_buySamll_0.size() +m_buySmall_2.size() + m_buySmall_3.size())== GetPlayingUserCount())
			{
				//if (m_buySmall_2.size() == 1)
				//{
				//	YingSanZhangS2CStartAddScore send;
				//	send.m_addScorePos = m_curPos;
				//	send.m_flag = 0;
				//	switch (m_playtype.gametypeGetScoreTime())
				//	{
				//	case 1:
				//	{
				//		send.m_score = 2;
				//		break;
				//	}
				//	case 2:
				//	{
				//		send.m_score = 4;
				//		break;
				//	}
				//	case 3:
				//	{
				//		send.m_score = 6;
				//		break;
				//	}
				//	case 4:
				//	{
				//		send.m_score = 2;
				//		break;
				//	}
				//
				//	}
				//	send.m_totalScore = m_tableScoreTotal + send.m_score;
				//	send.m_bigSmallPos = m_buySmall_2[0];
				//	if (send.m_bigSmallPos < YINGSANZHANG_PLAY_USER_MAX && m_desk && m_desk->m_user[send.m_bigSmallPos] != NULL)
				//		send.m_nike = m_desk->m_user[send.m_bigSmallPos]->m_userData.m_nike;
				//
				//	notify_desk(send);
				//
				//	setSmallOrBigUserInfo(0, m_buySmall_2[0], send.m_score,1);
				//	
				//	return 0;
				//}
				 if (!m_buySmall_2.empty())            //切换选大模式
				{
					return 1;
				}
				else
				{
					YingSanZhangS2CStartAddScore send;
					send.m_addScorePos = m_curPos;
					send.m_flag = 2;
					send.m_score = m_CurrentTimes;
					notify_desk(send);

					m_bigOrSmall = 2;
					SetDelayTime();

					return 0;

				}


			}
		}
		if (m_play_status == GAME_STATUS_BUY_BIG)
		{
			if (m_buyBig_3.size() >= 1)
			{
				YingSanZhangS2CStartAddScore send;
				send.m_flag = 1;
				switch (m_playtype.gametypeGetScoreTime())
				{
				case 1:
				{
					send.m_score = 3;
					break;
				}
				case 2:
				{
					send.m_score = 8;
					break;
				}
				case 3:
				{
					send.m_score = 12;
					break;
				}
				case 4:
				{
					send.m_score = 4;
					break;
				}

				}
				send.m_totalScore = m_tableScoreTotal + send.m_score;
				send.m_bigSmallPos = m_buyBig_3[0];
				send.m_addScorePos = GetNextPos(m_buyBig_3[0]);
				m_curPos = send.m_addScorePos;
				if (send.m_bigSmallPos < YINGSANZHANG_PLAY_USER_MAX && m_desk && m_desk->m_user[send.m_bigSmallPos] != NULL)
					send.m_nike = m_desk->m_user[send.m_bigSmallPos]->m_userData.m_nike;

				notify_desk(send);
				setSmallOrBigUserInfo(1, m_buyBig_3[0], send.m_score, 2);

				return 0;
			}
			if ( (m_buyBig_0.size()  + m_buyBig_3.size()) == GetPlayingUserCount())
			{
				
				{
					YingSanZhangS2CStartAddScore send;
					send.m_flag = 0;
					switch (m_playtype.gametypeGetScoreTime())
					{
					case 1:
					{
						send.m_score = 2;
						break;
					}
					case 2:
					{
						send.m_score = 4;
						break;
					}
					case 3:
					{
						send.m_score = 6;
						break;
					}
					case 4:
					{
						send.m_score = 2;
						break;
					}

					}
					send.m_totalScore = m_tableScoreTotal + send.m_score;
					send.m_bigSmallPos = m_buySmall_2[0];
					send.m_addScorePos = GetNextPos(m_buySmall_2[0]);
					m_curPos = send.m_addScorePos;
					if (send.m_bigSmallPos < YINGSANZHANG_PLAY_USER_MAX && m_desk && m_desk->m_user[send.m_bigSmallPos] != NULL)
						send.m_nike = m_desk->m_user[send.m_bigSmallPos]->m_userData.m_nike;

					notify_desk(send);

					setSmallOrBigUserInfo(0, m_buySmall_2[0], send.m_score,1);
				
					return 0;
				}
			}
		}

		return 2;
	}

	//一局游戏开始路口
	void start_game()
	{
		//开局下底注
		Lint tempCurrentTimes=m_playtype.gametypeGetScoreTime();
		m_CurrentTimes = (tempCurrentTimes == 4) ? 1 : tempCurrentTimes;
		for (WORD i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			if (m_user_playing_status[i]==TRUE)
			{
				m_TableScore[i]=m_CurrentTimes;
				m_tableScoreTotal+=m_CurrentTimes;
				recordTableScoreCount(i);
			}
		}

		//分发扑克
		BYTE TempUserCardCount = sizeof(m_hand_card_data) / sizeof(m_hand_card_data[0][0]);
		if (m_playtype.gametypeGetGameMode() == 2)
			TempUserCardCount = YINGSANZHANG_PLAY_USER_5*YINGSANZHANG_HAND_CARD_MAX;
		m_gamelogic.RandCardList_PlayType(m_hand_card_data[0], TempUserCardCount, m_playtype.gametypeGetGameMode());
		//获取牌型
		for (Lint i = 0; i < m_player_count; i++)
		{
			if(m_playtype.gametypeGetTianDiLong()==1)
				m_HandCardType[i] = m_gamelogic.GetCardTypeDragon(m_hand_card_data[i], YINGSANZHANG_HAND_CARD_MAX);
			else
				m_HandCardType[i] = m_gamelogic.GetCardTypeSord(m_hand_card_data[i], YINGSANZHANG_HAND_CARD_MAX);
		}

		//m_zhuangpos=0;
		if (m_zhuangpos<0||m_zhuangpos>=YINGSANZHANG_PLAY_USER_MAX)
		{
			LLOG_ERROR("start_game m_zhuangpos error ");
			m_zhuangpos=GetNextPos(0);
		}
		//第一个反应用户, --庄家下家
		m_curPos=GetNextPos(m_zhuangpos);


		//广播游戏开始命令
		YingSanZhangStartGameRound send;
		memcpy(send.m_Status, m_user_status, sizeof(send.m_Status));
		send.m_CellScore=m_CurrentTimes;
		send.m_MaxScore=m_tableScoreTotal;
		send.m_BankerUser=m_zhuangpos;
		send.m_CurrentUser=m_curPos;
		for (Lint i = 0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			if (m_desk && m_desk->m_vip)
			{
				send.m_TotalScore[i] = m_desk->m_vip->m_score[i];
				LLOG_DEBUG("GameEnd.m_TotalScore[%d]=[%d]", i, send.m_TotalScore[i]);
			}
			else
				LLOG_ERROR("m_desk or  m_desk->m_vip is null ,YingSanZhangStartGameRound");
		}

		notify_desk(send);

		//游戏开始计时
		if (send.m_CurrentUser != INVAILD_POS_QIPAI)
		{
			if (m_playtype.gametypeGetAddMin() == 1)
			{
				SetDelayTime(3);
				m_play_status = GAME_STATUS_BUY_SAMLL;
			}
			else
			{
				SetDelayTime(2);
			}
		}
		//广播
		YingSanZhangS2CLun sendLun;
		sendLun.m_value = m_curLun;
		notify_desk(sendLun);

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

		std::vector<Lstring>  paixing;
		for (int i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			std::stringstream pai;
			for (int j = 0; j < YINGSANZHANG_HAND_CARD_MAX; j++)
			{
				pai <<  std::hex << (Lint)m_hand_card_data[i][j];
				if (j + 1 < YINGSANZHANG_HAND_CARD_MAX)
					pai << ",";
			}
			paixing.push_back(pai.str());

		}
	
		// 保存结果	
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, score, m_user_status, zhuangPos, m_video.m_Id, paixing);
			//m_desk->m_vip->ResetJinAnGangCount(); // 使用后清除掉
		}

	}

	//开始一局
	void start_round(Lint * PlayerStatus) {
		clear_round();
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);

		memcpy(m_user_playing_status, PlayerStatus, sizeof(m_user_playing_status));
		memcpy(m_user_status, PlayerStatus, sizeof(m_user_status));
		
		//通知manager局数变化
		if (m_desk && m_desk->m_vip)
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle+1, m_desk->m_vip->m_maxCircle);

		LLOG_ERROR("****Desk %d start round %d/%d   player_count=%d", m_desk?m_desk->GetDeskId():0, m_round_offset, m_round_limit, m_player_count);
		LLOG_DEBUG("gameMode=[%d],menLun=[%d],socoreRound=[%d],socreTime=[%d],spec=[%d]", m_playtype.gametypeGetGameMode(), m_playtype.gametypeGetMenRound(), m_playtype.gametypeGetScoreRound(), m_playtype.gametypeGetScoreTime(), m_playtype.gametypeGetSpecAddScore());

		//广播
		YingSanZhangS2CLun send;
		m_curLun=1;
		send.m_value=m_curLun;
		notify_desk(send);

		//对庄家进行处理
		if (m_zhuangpos >= 0 && m_zhuangpos < YINGSANZHANG_PLAY_USER_MAX&& m_user_status[m_zhuangpos] == 0)
		{
			LLOG_DEBUG("start_round  ZHUANGPOS CHANGE");
			m_zhuangpos = GetNextPos(m_zhuangpos);

		}

		//游戏开始路口
		start_game();
		LLOG_DEBUG("****Desk %d start round %d/%d, player_count=%d", m_desk->GetDeskId(), m_round_offset, m_round_limit, m_player_count);
	}

	//结束一局
	void  finish_round(Lint result, Lint winpos,  bool delaySend=false)
	{
		LLOG_DEBUG("finish_round");
		switch (result)
		{
		case GAMG_CONCLUDE_SYSTEM_COMPARECARD:  //系统比牌结束
		case GAMG_CONCLUDE_COMPARECARD:	//比牌结束
		case GAMG_CONCLUDE_NOPLAYER:		//没有玩家
		{
			if (result!=GAMG_CONCLUDE_SYSTEM_COMPARECARD && m_GameEnd)return;
			m_GameEnd=true;

			//定下局庄--最后大赢家为下局庄
			//m_zhuangpos=0;
			increase_round();

			m_zhuangpos=winpos;
			Lint wWinner=winpos;  //最后大赢家
			Lint  lWinnerScore=0;              //计算大赢家总注
			LLOG_DEBUG("finish_round   winPos=[%d]", winpos);

			//结束数据
			YingSanZhangEndGameRound_s GameEnd;

			GameEnd.m_FinalDw=m_dismissed||m_round_offset>=m_round_limit;
			for (Lint i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
			{
				GameEnd.m_GiveUp[i] = m_GiveUpUser[i] ? 1 : 0;
			}

			//统计分数
			for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
			{
				//获取用户
				if (i==wWinner)continue;
				if (m_user_status[i]!=1)continue;

				GameEnd.m_GameScore[i]=-m_TableScore[i];
				//添加特殊加减分项
				GameEnd.m_GameScore[i]+=(-1)*spec_add_score(wWinner);
				lWinnerScore+=(-1)*(GameEnd.m_GameScore[i]);
			}
			GameEnd.m_GameScore[wWinner]=lWinnerScore;

			// 保存录像
			//玩家手牌[0-2] + 手牌类型[3]
			std::vector<Lint> t_handCards[YINGSANZHANG_PLAY_USER_MAX];
			for (size_t i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				for (size_t j = 0; j < YINGSANZHANG_HAND_CARD_MAX; ++j)
				{
					t_handCards[i].push_back(m_hand_card_data[i][j]);
				}

				t_handCards[i].push_back(m_HandCardType[i]);
			}

			//保存手牌 + 手牌类型 + 手牌倍数
			m_video.setHandCards(m_player_count, t_handCards);

			//回放记录每局玩家得分
			m_video.setRoundScore(m_player_count, GameEnd.m_GameScore);

			//保存回放数据
			if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
			{
				VideoSave();
			}

			add_round_log(GameEnd.m_GameScore, wWinner);

			//记录总分
			for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
			{
				if (m_desk && m_desk->m_vip)
				{
					GameEnd.m_TotalScore[i]=m_desk->m_vip->m_score[i];
					//LLOG_DEBUG("GameEnd.m_TotalScore[%d]=[%d]", i,GameEnd.m_TotalScore[i]);
				}
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null ,YingSanZhangEndGameRound");
			}

			//比牌数组
			for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
			{
				//LLOG_DEBUG("[%d]--CompardUser: ===============================",i);
				if (m_user_status[i] != 1)
				{
					//LLOG_DEBUG("[%d].....status [%d]  return",i, m_user_status[i]);
					continue;
				}
				auto ItUser=m_CompardUser.find(i);
				if (ItUser!=m_CompardUser.end())
				{
					Lint count=0;
					for (auto ItCompareUser=ItUser->second.begin(); count<YINGSANZHANG_PLAY_USER_MAX-1&&ItCompareUser!=ItUser->second.end(); ItCompareUser++)
					{
						GameEnd.m_CompareUser[i][count++]=(*ItCompareUser);
						//LLOG_DEBUG("[%d]......[%d]", i, (*ItCompareUser));
					}
				}
				else
				{
					LLOG_DEBUG("CompardUser not find [%d]", i);
				}
			}

			//结束信息
			for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
			{
				//LLOG_DEBUG("i=[%d]---------------------------------------------------------------------wWinner=[%d]", i, wWinner);
				GameEnd.reset_part();
				//大赢家
				if (wWinner<YINGSANZHANG_PLAY_USER_MAX)
				{
					for (int cardNum=0; cardNum<YINGSANZHANG_HAND_CARD_MAX; cardNum++)
						GameEnd.m_CardData[wWinner][cardNum]=(Lint)m_hand_card_data[wWinner][cardNum];
					GameEnd.m_CardType[wWinner]=(Lint)m_HandCardType[wWinner];
				}
				
				if (m_user_status[i]!=1)
				{
					//if (delaySend)
						memcpy(m_StGameEnd+i, &GameEnd, sizeof(YingSanZhangEndGameRound_s));
					//else
						//notify_user(GameEnd, i);
					continue;
				}

				//自己
				if (i!=wWinner)
				{
					for (int cardNum=0; cardNum<YINGSANZHANG_HAND_CARD_MAX; cardNum++)
						GameEnd.m_CardData[i][cardNum]=(Lint)m_hand_card_data[i][cardNum];
					GameEnd.m_CardType[i]=(Lint)m_HandCardType[i];
				}
				//比牌人
				for (Lint j=0; j<YINGSANZHANG_PLAY_USER_MAX-1; j++)
				{
					if (GameEnd.m_CompareUser[i][j]==INVAILD_POS_QIPAI)continue;
					Lint userChairId=GameEnd.m_CompareUser[i][j];
					if (userChairId<0||userChairId>=YINGSANZHANG_PLAY_USER_MAX)continue;
					
					//LLOG_DEBUG("USER[%d]---compare[%d]",i, userChairId);
					for (int cardNum=0; cardNum<YINGSANZHANG_HAND_CARD_MAX; cardNum++)
					{
						GameEnd.m_CardData[userChairId][cardNum]=(Lint)m_hand_card_data[userChairId][cardNum];

					}
					GameEnd.m_CardType[userChairId]=(Lint)m_HandCardType[userChairId];
				}

				//log
				//for (Lint w = 0; w< YINGSANZHANG_PLAY_USER_MAX ; w++)
				//{
				//	LLOG_DEBUG("userPos[%d]  CARD [%d]=== type=[%d]", i,w,GameEnd.m_CardType[w]);
				//	LLOG_DEBUG("userPos[%d]  CARD [%d]=== card1=[%d]", i, w, GameEnd.m_CardData[w][0]);
				//	LLOG_DEBUG("userPos[%d]  CARD [%d]=== card2=[%d]", i, w, GameEnd.m_CardData[w][1]);
				//	LLOG_DEBUG("userPos[%d]  CARD [%d]=== card3=[%d]", i, w, GameEnd.m_CardData[w][2]);
				//}

				memcpy(m_StGameEnd + i, &GameEnd, sizeof(YingSanZhangEndGameRound_s));
				//if (delaySend)
				//{
					//memcpy(m_StGameEnd+i, &GameEnd, sizeof(YingSanZhangEndGameRound));
				//}
				//else
				//	notify_user(GameEnd,i);
			}

			for (Lint s = 0; s < YINGSANZHANG_PLAY_USER_MAX; s++)
			{
				YingSanZhangEndGameRound  send;
				for (int i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
				{
					if (m_desk->m_user[i])
					{
						send.m_usernikes.push_back(m_desk->m_user[i]->GetUserData().m_nike);
						send.m_userids.push_back(m_desk->m_user[i]->GetUserDataId());
						send.m_headUrl.push_back(m_desk->m_user[i]->GetUserData().m_headImageUrl);
					}
				}
				send.copy(m_StGameEnd[s]);
				notify_user(send, s);
			}
			m_GameResult=true;
		
			if (!delaySend)
			{
				set_desk_state(DESK_WAIT);

				if (result == GAMG_CONCLUDE_COMPARECARD)
				{
					if (m_desk)m_desk->set_ysz_compare_count(1,1);
				}
				else if (result == GAMG_CONCLUDE_SYSTEM_COMPARECARD)
				{
					if (m_desk)m_desk->set_ysz_compare_count(0,m_sysCompareCount-1);
				}

				//结束游戏
				if (m_desk) m_desk->HanderGameOver(1);


			}
			return;

		}
		case GAMG_CONCLUDE_DISMISS:
		{
			//LLOG_DEBUG("GAMG_CONCLUDE_DISMISS");
			LLOG_ERROR("deskId=[%d],GAMG_CONCLUDE_DISMISS",m_desk?m_desk->GetDeskId():0);
			increase_round();
			//结束数据
			YingSanZhangEndGameRound GameEnd;
			for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
			{
				if (m_desk && m_desk->m_vip)
					GameEnd.m_TotalScore[i]=m_desk->m_vip->m_score[i];
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null ,YingSanZhangEndGameRound GAMG_CONCLUDE_DISMISS");
			}
			GameEnd.m_FinalDw=1;

			add_round_log(GameEnd.m_GameScore, winpos);

			notify_desk(GameEnd);

			set_desk_state(DESK_WAIT);
			//结束游戏
			if (m_desk) m_desk->HanderGameOver(1);
			return;
		}
		default:
		{
			LLOG_ERROR("finish_round  reason error");
			break;
		}
			
		}   //end switch
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	//子游戏事件
	//////////////////////////////////////////////////////////////////////////
	//自动跟注
	bool on_event_user_auto_score(User *pUser, bool isAuto)
	{
		if (pUser==NULL)return false;

		LLOG_DEBUG("on_event_user_auto_score userPos[%d],userid[%d],isAuto", GetUserPos(pUser), pUser->GetUserDataId(), isAuto);
		if (isAuto)
		{
			m_AutoScore[GetUserPos(pUser)]=true;
		}
		else
		{
			m_AutoScore[GetUserPos(pUser)]=false;
		}

		YingSanZhangS2CAutoScore  send;
		send.m_value=1;
		pUser->Send(send);

		return true;
	}
	//加注
	bool on_event_user_add_score(User *pUser, Lint lScore, bool bGiveUp, bool bCompareCard, bool bAddScoreFlag,bool isAuto=false)
	{
		LLOG_DEBUG("on_event_user_add_score,lScore[%d],bGiveUp[%d],bCompareCard[%d],bAddScoreFlag[%d]", lScore,bGiveUp,bCompareCard,bAddScoreFlag);
		if (pUser==NULL)return false;
		Lint userPos=GetUserPos(pUser);
		//用户操作记录,判断
		m_UserAddScoreCount[userPos]++;

		//更新轮数检查
		if (updateGameLun())
		{
			//广播
			YingSanZhangS2CLun send;
			send.m_value=m_curLun;
			notify_desk(send);
		}

		if (bGiveUp==false)
		{
			//金币效验
			if (lScore<0||lScore%m_CurrentTimes!=0)
			{
				LLOG_ERROR("on_event_user_add_score  score[%d] error!!!", lScore);
				return false;
			}

			//用户注金
			m_TableScore[userPos]+=lScore;
			//桌面筹码和
			m_tableScoreTotal+=lScore;
			//桌面筹码分类
			recordTableScoreCount(userPos, bCompareCard);
		}
		//设置用户
		WORD wNextPlayer=INVAILD_POS_QIPAI;
		if (!bCompareCard)
		{
			//用户切换
			wNextPlayer=GetNextPos(userPos);
			m_curPos=wNextPlayer;
			//判断是否达到限制轮数
			if (CheckLimitLun(m_curPos))
				m_curPos=INVAILD_POS_QIPAI;
		}

		//构造数据
		YingSanZhangS2CAddScore send;

		send.m_AddScoreFlag=1;      //跟注
		if (bAddScoreFlag)send.m_AddScoreFlag=2;      //加注
		if (bCompareCard)send.m_AddScoreFlag=3;      //比牌
		if (bGiveUp)send.m_AddScoreFlag=0;                 //弃牌
		send.m_CurrentTimes=m_CurrentTimes;
		send.m_CurrentUser=m_curPos;
		send.m_AddScoreUser=userPos;
		send.m_AddScoreCount=lScore;
		send.m_UserScoreTotal=m_TableScore[userPos];
		send.m_TableScoreTotal=m_tableScoreTotal;
		if (isAuto)
			send.m_AutoAdd = 1;


		//判断状态
		send.m_CompareState=(bCompareCard)?TRUE:FALSE;

		//发送数据
		notify_desk(send);

		if (CheckLimitLun(wNextPlayer))
		{
			return on_event_system_compare_card();
		}

		//游戏开始计时
		if(send.m_CurrentUser!= INVAILD_POS_QIPAI)
			SetDelayTime();

		//if (!bCompareCard)
		//{
		//	//判断下个用户开启自动加注
		//	if (m_curPos!=INVAILD_POS_QIPAI)
		//	{
		//		//IServerUserItem * pIServreUserItemCurr=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
		//		//if (pIServreUserItemCurr!=NULL)
		//		//{
		//		//	//判断下个用户开启自动加注,且断线
		//		//	if (m_lAutoScore[m_wCurrentUser]&&pIServreUserItemCurr->GetUserStatus()==0x06)
		//		//	{
		//		//		//设置定时器
		//		//		m_pITableFrame->SetGameTimer(IDI_AUTO_SCORE_TIME, TIME_AUTO_SCORE_TIME, 1, 0);
		//		//	
		//		//	}
		//	}
		//}
		return true;
	}
	//看牌
	bool on_event_user_look_card(User* pUser)
	{
		if (pUser==NULL)return false;
		Lint userPos=GetUserPos(pUser);
		LLOG_ERROR("deskid=[%d] on_event_user_look_card userPos[%d],userid[%d]",m_desk?m_desk->GetDeskId():0,userPos,pUser->GetUserDataId());
		if (m_MingZhu[userPos])
		{
			LLOG_ERROR("on_event_user_look_card error !!! user alread lookcard .userPos[%d],userid[%d]", userPos, pUser->GetUserDataId());
			return false;
		}
		m_MingZhu[userPos]=true;

		YingSanZhangS2CLookCard send;
		send.m_LookCardUser=userPos;

		notify_desk_without_user(send, pUser);

		YingSanZhangS2CLookCard send1;
		send1.m_LookCardUser=userPos;
		send1.m_CardType=m_HandCardType[userPos];
		for (Lint i=0; i<YINGSANZHANG_HAND_CARD_MAX; i++)
		{
			send1.m_CardData[i]=(Lint)m_hand_card_data[userPos][i];
			//memcpy(send.m_CardData, m_hand_card_data[userPos], sizeof(send.m_CardData));
			
		}
		pUser->Send(send1);
		return true;
	}
	//弃牌
	bool on_event_user_give_up(User * pUser)
	{
		if (pUser==NULL)return false;
		Lint userPos=GetUserPos(pUser);
		LLOG_ERROR("deskId=[%d] on_event_user_give_up userPos[%d],userid[%d]",m_desk?m_desk->GetDeskId():0, userPos, pUser->GetUserDataId());

		//设置数据
		m_GiveUpUser[userPos]=TRUE;
		m_user_playing_status[userPos]=FALSE;
		m_AutoScore[userPos]=FALSE;

		YingSanZhangS2CGiveUp send;
		send.m_GiveUpUser=userPos;
		if (GetPlayingUserCount()<2)
			send.m_IsFinal=1;

		notify_desk(send);

		if (GetPlayingUserCount()>=2)
		{
			if (m_curPos==userPos) on_event_user_add_score(pUser, 0L, true, false, false);
		}
		else if (GetPlayingUserCount()==1)
		{
			LLOG_DEBUG("on_event_user_give_up  user count  is 1 ,finish_round");
			finish_round(GAMG_CONCLUDE_NOPLAYER, GetNextPos(userPos));
		}
		else
		{
			LLOG_ERROR("on_event_user_give_up  error no player!!!");
		}

		return true;
	}
	//比牌
	bool on_event_user_compare_card(User *pUser, Lint  wFirstChairID, Lint wNextChairID)
	{
		if (pUser==NULL)return false;

		LLOG_ERROR("deskId=[%d]on_event_user_compare_card wFirstChairID[%d],wNextChairID[%d]",m_desk?m_desk->GetDeskId():0, wFirstChairID, wNextChairID);
		
		//效验参数
		if (!(m_CompareCount>0))return false;
		//状态设置
		m_CompareCount=0;

		//判断第一轮，第一轮不允许比牌
		if (m_curLun==1)
		{
			LLOG_ERROR("on_event_user_compare_card error!!! m_curLun==1");
			return false;
		}
		//闷牌判断,闷牌阶段不让比牌
		if (m_playtype.gametypeGetMenRound()>=m_curLun)
		{
			LLOG_ERROR("on_event_user_compare_card error!!! MenRound[%d]>=currLun[%d]", m_playtype.gametypeGetMenRound(),m_curLun);
			return true;
		}
		//比较大小
		BYTE cbWin=m_gamelogic.CompareCard(m_hand_card_data[wFirstChairID], m_hand_card_data[wNextChairID], YINGSANZHANG_HAND_CARD_MAX,m_playtype.gametypeGetTianDiLong());

		//胜利用户
		Lint wLostUser, wWinUser;
		if (m_playtype.gametypeGetAddMin() == 1 && m_bigOrSmall==0)
		{
			if (cbWin == TRUE)
			{
				if (m_playtype.gametypeGetMinBaoziMax() == 1 && m_gamelogic.GetCardTypeSord(m_hand_card_data[wFirstChairID], YINGSANZHANG_HAND_CARD_MAX) == CT_BAO_ZI)
				{
					wWinUser = wFirstChairID;
					wLostUser = wNextChairID;
				}
				else
				{
					wWinUser = wNextChairID;
					wLostUser = wFirstChairID;
				}
			}
			else if (cbWin == DRAW)
			{
				wWinUser = wNextChairID;
				wLostUser = wFirstChairID;
			}
			else
			{
				if (m_playtype.gametypeGetMinBaoziMax() == 1 && m_gamelogic.GetCardTypeSord(m_hand_card_data[wNextChairID], YINGSANZHANG_HAND_CARD_MAX) == CT_BAO_ZI)
				{
					wWinUser = wNextChairID;
					wLostUser = wFirstChairID;
				}
				else
				{
					wWinUser = wFirstChairID;
					wLostUser = wNextChairID;
				}
			}

		}
		else
		{
			if (cbWin == TRUE)
			{
				wWinUser = wFirstChairID;
				wLostUser = wNextChairID;
			}
			else
			{
				wWinUser = wNextChairID;
				wLostUser = wFirstChairID;
			}
		}

		//设置数据
		m_CompardUser[wLostUser].insert(wWinUser);
		m_CompardUser[wWinUser].insert(wLostUser);
		m_user_playing_status[wLostUser]=FALSE;
		m_LostUser[wLostUser]=TRUE;

		//人数统计
		WORD wPlayerCount=GetPlayingUserCount();

		if (wPlayerCount>=2)
		{
			//设置用户
			m_curPos=GetNextPos(m_curPos);
		}
		else m_curPos=INVAILD_POS_QIPAI;

		//构造数据
		YingSanZhangS2CCompareCard send;
		send.m_IsFinal=0;
		if (wPlayerCount<2)
			send.m_IsFinal=1;

		//判断是否达到限制轮数
		if (CheckLimitLun(m_curPos))
		{
			send.m_CurrentUser = INVAILD_POS_QIPAI;
			send.m_IsFinal = 1;
		}
		else
			send.m_CurrentUser=m_curPos;

		send.m_LostUser=wLostUser;
		send.m_CompareUser[0]=wFirstChairID;
		send.m_CompareUser[1]=wNextChairID;

		//发送数据
		notify_desk(send);

		//结束游戏
		if (wPlayerCount<2)
		{
			LLOG_DEBUG("on_event_user_compare_card wPlayerCount<2   finish_round");
			//m_zhuangpos=wWinUser;
			finish_round(GAMG_CONCLUDE_COMPARECARD, wWinUser);
			return true;
			//OnEventGameConclude(DOUDIZHU_PLAY_USER_COUNT, NULL, GER_COMPARECARD);
		}
		else
		{  //判断是否达到限制轮数
			if (CheckLimitLun(m_curPos))
				return on_event_system_compare_card();

			//if (m_curPos!=YINGSANZHANG_INVAILD_POS_QIPAI)
			//{
			//	IServerUserItem * pIServreUserItemCurr=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
			//	if (pIServreUserItemCurr!=NULL)
			//	{
			//		//判断下个用户开启自动加注,且断线
			//		if (m_lAutoScore[m_wCurrentUser]&&pIServreUserItemCurr->GetUserStatus()==0x06)
			//		{
			//			//设置定时器
			//			m_pITableFrame->SetGameTimer(IDI_AUTO_SCORE_TIME, TIME_AUTO_SCORE_TIME, 1, 0);
			//			//LONGLONG userScore=(m_bMingZhu[m_wCurrentUser])?2*m_CurrentTimes:m_CurrentTimes;
			//			//OnUserAddScore(m_wCurrentUser, userScore, false, false);
			//		}
			//	}
			//}
		}

		//游戏开始计时
		if(send.m_CurrentUser!= INVAILD_POS_QIPAI)
			SetDelayTime(1);

		return true;

	}
	//特殊加分规则,返回加分值
	Lint  spec_add_score(Lint wWinUser)
	{
		if (m_playtype.gametypeGetSpecAddScore()==0)return 0;
		if (wWinUser>=YINGSANZHANG_PLAY_USER_MAX)return 0;

		if (CT_BAO_ZI==m_HandCardType[wWinUser]) return 10;
		else if (CT_SHUN_JIN==m_HandCardType[wWinUser]  || CT_JIN_DI_DRAGON ==  m_HandCardType[wWinUser] || CT_JIN_TIAN_DRAGON == m_HandCardType[wWinUser] )return 5;
		return 0;
	}
	//自动比牌
	bool on_event_system_compare_card()
	{
		LLOG_ERROR("deskId=[%d],on_event_system_compare_card",m_desk?m_desk->GetDeskId():0);
		m_GameEnd=true;
		m_sysCompareCount = GetPlayingUserCount();

		Lint wFirstChairID=0;
		if (m_zhuangpos>=0&&m_zhuangpos<YINGSANZHANG_PLAY_USER_MAX && GetPlayingUserCount()>=2)
			wFirstChairID=GetNextPos(m_zhuangpos);
		else
		{
			LLOG_ERROR("on_event_system_compare_card error!!! m_zhuangpos=[%d]",m_zhuangpos);
			if (m_user_playing_status[wFirstChairID]!=1)
				wFirstChairID=GetNextPos(wFirstChairID);
		}

		YingSanZhangS2CSystemCompareCard send;
		for (int i=0; i<m_player_count; i++)
		{
			if (GetPlayingUserCount()<2)break;

			Lint wNextChairID=GetNextPos(wFirstChairID);

			LLOG_DEBUG("on_event_system_compare_card:m_zhuangpos[%d],wFirstChairID[%d],wNextChairID[%d]", m_zhuangpos, wFirstChairID, wNextChairID);

			//比较大小
			BYTE cbWin=m_gamelogic.CompareCard(m_hand_card_data[wFirstChairID], m_hand_card_data[wNextChairID], YINGSANZHANG_HAND_CARD_MAX, m_playtype.gametypeGetTianDiLong());

			//胜利用户
			Lint wLostUser, wWinUser;
			if (m_playtype.gametypeGetAddMin() == 1 && m_bigOrSmall == 0)
			{
				if (cbWin == TRUE)
				{
					if (m_playtype.gametypeGetMinBaoziMax() == 1 && m_gamelogic.GetCardTypeSord(m_hand_card_data[wFirstChairID], YINGSANZHANG_HAND_CARD_MAX) == CT_BAO_ZI)
					{
						wWinUser = wFirstChairID;
						wLostUser = wNextChairID;
					}
					else
					{
						wWinUser = wNextChairID;
						wLostUser = wFirstChairID;
					}
				}
				else if (cbWin == DRAW)
				{
					wWinUser = wNextChairID;
					wLostUser = wFirstChairID;
				}
				else
				{
					if (m_playtype.gametypeGetMinBaoziMax() == 1 && m_gamelogic.GetCardTypeSord(m_hand_card_data[wNextChairID], YINGSANZHANG_HAND_CARD_MAX) == CT_BAO_ZI)
					{
						wWinUser = wNextChairID;
						wLostUser = wFirstChairID;
					}
					else
					{
						wWinUser = wFirstChairID;
						wLostUser = wNextChairID;
					}
				}

			}
			else
			{
				if (cbWin == TRUE)
				{
					wWinUser = wFirstChairID;
					wLostUser = wNextChairID;
				}
				else
				{
					wWinUser = wNextChairID;
					wLostUser = wFirstChairID;
				}
			}

			//设置数据
			m_CompardUser[wLostUser].insert(wWinUser);
			m_CompardUser[wWinUser].insert(wLostUser);
			m_user_playing_status[wLostUser]=FALSE;
			m_LostUser[wLostUser]=TRUE;

			LLOG_DEBUG("on_event_system_compare_card:wWinUser[%d],wLostUser[%d]", wWinUser, wLostUser);
			SystemCompareCard  info(wWinUser, wLostUser);
			send.m_info.push_back(info);

			//重置变量
			wFirstChairID=wWinUser;

		}
		//广播消息
		notify_desk(send);

		finish_round(GAMG_CONCLUDE_SYSTEM_COMPARECARD, wFirstChairID);

		return true;
	}

	bool on_event_user_buy_small_big(Lint pos, Lint  flag, Lint score)
	{
		LLOG_ERROR("deskId=[%d],on_event_user_buy_small_big pos[%d] flag[%d] score[%d] scoreTime[%d]", m_desk ? m_desk->GetDeskId() : 0,pos,flag,score, m_playtype.gametypeGetScoreTime());
		if (0 == flag)    	//买小模式下
		{
			if (m_playtype.gametypeGetScoreTime() == 1)
			{
				switch (score)
				{
				case 2:
				{
					m_buySmall_2.push_back(pos);
					break;
				}
				case 3:
				{
					m_buySmall_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buySamll_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_SMALL  desk[%d] userPos[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}
			else if (m_playtype.gametypeGetScoreTime() == 2)
			{
				switch (score)
				{
				case 4:
				{
					m_buySmall_2.push_back(pos);
					break;
				}
				case 8:
				{
					m_buySmall_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buySamll_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_SMALL  desk[%d] userPos[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}
			else if (m_playtype.gametypeGetScoreTime() == 3)
			{
				switch (score)
				{
				case 6:
				{
					m_buySmall_2.push_back(pos);
					break;
				}
				case 12:
				{
					m_buySmall_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buySamll_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_SMALL  desk[%d] userPos[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}
			else if (m_playtype.gametypeGetScoreTime() == 4)
			{
				switch (score)
				{
				case 2:
				{
					m_buySmall_2.push_back(pos);
					break;
				}
				case 4:
				{
					m_buySmall_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buySamll_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_SMALL  desk[%d] userPos[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}
			
			m_userBuySmall[pos] = score;
		}
		else                 //买大模式下
		{
			if (m_playtype.gametypeGetScoreTime() == 1)
			{
				switch (score)
				{
				case 3:
				{
					m_buyBig_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buyBig_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIG desk[%d] user[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}
			if (m_playtype.gametypeGetScoreTime() == 2)
			{
				switch (score)
				{
				case 8:
				{
					m_buyBig_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buyBig_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIG desk[%d] user[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}
			if (m_playtype.gametypeGetScoreTime() == 3)
			{
				switch (score)
				{
				case 12:
				{
					m_buyBig_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buyBig_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIG desk[%d] user[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}
			if (m_playtype.gametypeGetScoreTime() == 4)
			{
				switch (score)
				{
				case 4:
				{
					m_buyBig_3.push_back(pos);
					break;
				}
				case 0:
				{
					m_buyBig_0.push_back(pos);
					break;
				}
				default:
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIG desk[%d] user[%d] ,score[%d] error", m_desk ? m_desk->GetDeskId() : 0, pos, score);
					break;
				}

				}
			}

			m_userBuyBig[pos] = score;
		}
		//将买大买小的操作结果广播
		YingSanZhangS2CUserSelectBigSmall send;
		send.m_flag = flag;
		send.m_pos = pos;
		send.m_score = score;
		notify_desk(send);

		//判断是否所有人都操作完成
		Lint ret = is_big_small_over();
		switch (ret)
		{
		case 0:
		{
			m_play_status = GAME_STATUS_PLAY;
			break;
		}
		case 1:
		{
			m_play_status = GAME_STATUS_BUY_BIG;
			YingSanZhangS2CChangeBigSmallMode send1;
			SetDelayTime(4);
			notify_desk(send1);
			break;
		}
		}
		return true;
	}
};

struct YingSanZhangGameHandler : YingSanZhangGameCore {

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

	void notify_desk_playing_user(LMsg &msg)  {
		if (NULL==m_desk) return;
		m_desk->BoadCast2PlayingUser(msg);
	}

	void notify_desk_without_user(LMsg &msg, User* pUser)
	{
		if(NULL==m_desk) return;
		m_desk->BoadCastWithOutUser(msg,pUser);
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
		// 被 Desk 的 SetVip 调用，此时能得到 m_vip->m_maxCircle
		int round_limit=m_desk && m_desk->m_vip?m_desk->m_vip->m_maxCircle:0;
		setup_match(l_playtype, round_limit);
	}

	void MHSetDeskPlay(Lint PlayUserCount, Lint * PlayerStatus, Lint PlayerCount) {
		LLOG_DEBUG("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		for (int i = 0; i < PlayerCount; i++)
		{
			LLOG_DEBUG("MHSetDeskPlay,PlayerStatus[%d]=[%d]", i, PlayerStatus[i]);
		}

		if (!m_desk||!m_desk->m_vip) {
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		if (m_playtype.gametypeGetUserCount() == YINGSANZHANG_PLAY_USER_5)
		{
			if (PlayUserCount != YINGSANZHANG_PLAY_USER_5 || PlayerStatus == NULL || PlayerCount != YINGSANZHANG_PLAY_USER_5)
			{
				LLOG_ERROR("MHSetDeskPlay  error  !!!!PlayUserCount=[%d] ", PlayUserCount);
				return;
			}
		}
		else
		{
			if (PlayUserCount != YINGSANZHANG_PLAY_USER_8 || PlayerStatus == NULL || PlayerCount != YINGSANZHANG_PLAY_USER_8)
			{
				LLOG_ERROR("MHSetDeskPlay  error  !!!!PlayUserCount=[%d] ", PlayUserCount);
				return;
			}
		}

		m_player_count=PlayUserCount;
		//memcpy(m_user_playing_status, PlayerStatus, sizeof(m_user_playing_status));
		//
		//for(int i=0; i<YINGSANZHANG_PLAY_USER_MAX;i++)
		//	LLOG_DEBUG("m_user_playing_status[%d]=[%d]",i, m_user_playing_status[i]);

		start_round( PlayerStatus);
	}

	//游戏结束
	void OnGameOver(Lint result, Lint winpos, Lint bombpos) {
		if (m_desk==NULL||m_desk->m_vip==NULL) {
			LLOG_ERROR("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed=!!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d", m_desk?m_desk->GetDeskId():0, m_round_limit, m_round_offset, m_dismissed);
		finish_round(GAMG_CONCLUDE_DISMISS, 0, 0);
	}

	//游戏中断线重连
	void OnUserReconnect(User *pUser) {
		if (pUser==NULL||m_desk==NULL) {
			return;
		}

		// 发送当前圈数信息
		// 为什么要发给桌子，而不是只发给这个短线重连的玩家？
		notify_desk_match_state();

		Lint pos=GetUserPos(pUser);

		////广播轮数
		YingSanZhangS2CLun send;
		send.m_value=m_curLun;
		notify_user(send, pos);
		
		MHLOG_PLAYCARD("*******断线重连desk:%d userid:%d pos:%d m_curPos=%d ",
			m_desk?m_desk->GetDeskId():0, pUser->GetUserDataId(), pos, m_curPos);
		//m_needGetCard，m_curGetCard?ToTile(m_curGetCard):0, m_curOutCard?ToTile(m_outCard):0);
		Lint nCurPos=m_curPos;
		// 如果在一盘结束那个地方重连，该是个什么情况？
		//Lint nDeskPlayType=m_play_state;
		LLOG_DEBUG("Desk::OnUserReconnect send out");
		YingSanZhangS2CPlayScene reconn;

		reconn.m_GameStatus = m_play_status;
		reconn.m_IsBuy = is_user_already_buy(pos) ? 1 : 0;
		if (m_play_status == GAME_STATUS_BUY_SAMLL)
		{
			memcpy(reconn.m_UserBuyStatus, m_userBuySmall, sizeof(reconn.m_UserBuyStatus));
		}
		if (m_play_status == GAME_STATUS_BUY_BIG)
		{	
			memcpy(reconn.m_UserBuyStatus, m_userBuyBig, sizeof(reconn.m_UserBuyStatus));
		}
		if (m_play_status == GAME_STATUS_PLAY && m_bigOrSmall!=2)
		{
			reconn.m_BuyPos =m_bigSmallPos ;
			reconn.m_BuyScore = m_bigSmallScore;
			if (reconn.m_BuyPos < YINGSANZHANG_PLAY_USER_MAX && m_desk && m_desk->m_user[reconn.m_BuyPos] != NULL)
				reconn.m_BuyNike = m_desk->m_user[reconn.m_BuyPos]->m_userData.m_nike;

		}

		reconn.m_BuyType = m_bigOrSmall;


		//新加字段
		if(m_user_status[pos]==1)
			reconn.m_DynamicJoin=0;
		else
			reconn.m_DynamicJoin=1;

		reconn.m_TableTotalScore=m_tableScoreTotal;

		//用户弃牌输牌状态
		for (WORD i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			if (m_GiveUpUser[i])
			{
				reconn.m_UserStatus[i]=1;
				continue;
			}
			if (m_LostUser[i])
			{
				reconn.m_UserStatus[i]=2;
				continue;
			}
		}
		//桌面筹码情况
		memcpy(reconn.m_TableScoreCount, m_tableScoreCount, sizeof(m_tableScoreCount));

		reconn.m_AutoScore=m_AutoScore[pos];

		//加注信息
		reconn.m_MaxCellScore=0;
		reconn.m_CellScore=m_CurrentTimes;
		reconn.m_CurrentTimes=m_CurrentTimes;
		//reconn.m_UserMaxScore=m_UserMaxScore[pos];

		//设置变量
		reconn.m_BankerUser=m_zhuangpos;
		reconn.m_CurrentUser=m_curPos;
		for (Lint i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			reconn.m_MingZhu[i]=m_MingZhu[i];
			reconn.m_TableScore[i]=m_TableScore[i];
			reconn.m_PlayStatus[i]=m_user_status[i];
		}
		reconn.m_CompareState=m_GameEnd;

		//设置扑克
		if (m_MingZhu[pos]&&m_user_playing_status[pos])
		{
			for (int i=0; i<YINGSANZHANG_HAND_CARD_MAX; i++)
				reconn.m_HandCardData[i]=(Lint)m_hand_card_data[pos][i];
			
			reconn.m_CardType=m_HandCardType[pos];
		}
	
		if(m_play_status==GAME_STATUS_BUY_BIG || m_play_status==GAME_STATUS_BUY_SAMLL)
			reconn.m_remain_time = DELAY_TIME_BIG_SMALL - (time(NULL) - m_play_status_time.Secs());
		else
		{
			reconn.m_remain_time = DELAY_TIME_ADD_SCORE - (time(NULL) - m_play_status_time.Secs());
			if (m_playtype.gametypeGetGameType() == 1)
				reconn.m_remain_time = DELAY_TIME_ADD_SCORE_BIG_HAPPY - (time(NULL) - m_play_status_time.Secs());
		}
		
	
		notify_user(reconn, pos);

		////广播轮数
		//YingSanZhangS2CLun send;
		//send.m_value=m_curLun;
		//notify_user(send, pos);

		//if (m_GameResult)
		//{
		//	//设置桌子状态
		//	set_desk_state(DESK_WAIT);
		//	for (int i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		//	{
		//		notify_user(m_StGameEnd[i], i);
		//	}
		//
		//	if (m_round_offset>=m_round_limit && m_desk)
		//	{
		//		m_desk->HanderGameOver(1);
		//	}
		//}

	}

	//处理游戏中的消息
	bool HanderGameMessage(User* pUser, LMsgC2SGameCommon *msg)
	{
		if (pUser==NULL||msg==NULL)
		{
			LLOG_ERROR("HanderGameMessage  error,  pUser==NULL or msg==NULL");
			return false;
		}
		LLOG_DEBUG("HanderGameMessage  ....  subID=[%d]",msg->m_subID);

		switch (msg->m_subID)
		{
			//自动加注
			case MSG_YINGSANZHANG_C_2_S_AUTO_SCORE:
			{
				YingSanZhangC2SAutoScore * pAutoScore=(YingSanZhangC2SAutoScore*)msg;
				//检查位置和状态
				if (GetUserPos(pUser)==INVAILD_POS_QIPAI)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_AUTO_SCORE  user[%d] pos not ok  ", pUser->m_userData.m_id);
					return false;
				}
				if (m_user_playing_status[GetUserPos(pUser)]==0)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_AUTO_SCORE  user[%d] status 0 ", pUser->m_userData.m_id);
					return false;
				}
				//是否结束
				if (m_GameEnd)return false;

				return on_event_user_auto_score(pUser, pAutoScore->m_value==0?false:true);

			}
			//弃牌
			case MSG_YINGSANZHANG_C_2_S_GIVE_UP:
			{
				YingSanZhangC2SGiveUp * pGiveUp=(YingSanZhangC2SGiveUp*)msg;
				//检查位置和状态
				if (GetUserPos(pUser)==INVAILD_POS_QIPAI)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_GIVE_UP  user[%d] pos not ok  ", pUser->m_userData.m_id);
					return false;
				}
				if (m_user_playing_status[GetUserPos(pUser)]==0)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_GIVE_UP  user[%d] status 0 ", pUser->m_userData.m_id);
					return false;
				}
				//当前操作用户
				if (GetUserPos(pUser) != m_curPos)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_GIVE_UP  user[%d]  not curPos[%d]", pUser->m_userData.m_id, m_curPos);
					return false;
				}
				//闷牌判断,闷牌阶段不让比牌
				if (m_playtype.gametypeGetMenRound() >= m_curLun)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_GIVE_UP error!!!user[%d]  MenRound[%d]>=currLun[%d]", pUser->m_userData.m_id, m_playtype.gametypeGetMenRound(), m_curLun);
					return true;
				}

				//是否结束
				if (m_GameEnd)return false;

				//消息处理
				UnsetDelayTime();
				return on_event_user_give_up(pUser);

			}
			//看牌
			case MSG_YINGSANZHANG_C_2_S_LOOK_CARD:
			{
				//闷牌判断
				if (m_playtype.gametypeGetMenRound()>=m_curLun)return false;

				YingSanZhangC2SLookCard* pLookCard=(YingSanZhangC2SLookCard*)msg;
				//检查位置和状态
				if (GetUserPos(pUser)==INVAILD_POS_QIPAI)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_LOOK_CARD  user[%d] pos not ok  ", pUser->m_userData.m_id);
					return false;
				}
				if (m_user_playing_status[GetUserPos(pUser)]==0)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_LOOK_CARD  user[%d] status 0 ", pUser->m_userData.m_id);
					return false;
				}

				//是否结束
				if (m_GameEnd)return false;

				//消息处理
				return on_event_user_look_card(pUser);

			}
			//比牌
			case MSG_YINGSANZHANG_C_2_S_COMPARE_CARD:
			{
				YingSanZhangC2SCompareCard * pCompareCard=(YingSanZhangC2SCompareCard*)msg;
				//检查位置和状态
				if (GetUserPos(pUser)==INVAILD_POS_QIPAI  ||pCompareCard->m_value>=YINGSANZHANG_PLAY_USER_MAX)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_COMPARE_CARD  user pos not ok  ");
					return false;
				}
				if (m_user_playing_status[GetUserPos(pUser)]==0 ||m_user_playing_status[pCompareCard->m_value]==0)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_COMPARE_CARD  user status 0 ");
					return false;
				}
			
				//当前操作用户
				if (GetUserPos(pUser)!=m_curPos)
				{
					LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_COMPARE_CARD  user[%d]  not curPos[%d]", pUser->m_userData.m_id, m_curPos);
					return false;
				}

				//是否结束
				if (m_GameEnd)return false;

				UnsetDelayTime();
				return on_event_user_compare_card(pUser, GetUserPos(pUser), pCompareCard->m_value);

			}
			//完成动画
			case MSG_YINGSANZHANG_C_2_S_FINISH_FLASH:
			{
				////检查位置和状态
				//if (GetUserPos(pUser)==INVAILD_POS_QIPAI)
				//{
				//	LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_FINISH_FLASH  user[%d] pos not ok  ", pUser->m_userData.m_id);
				//	return false;
				//}
				////if (m_user_playing_status[GetUserPos(pUser)]==0)
				////{
				////	LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_FINISH_FLASH  user[%d] status 0 ", pUser->m_userData.m_id);
				////	return false;
				////}
				//
				//if (GetPlayingUserCount()>=2)return false;
				//
				////是否结束
				//if (!m_GameResult)return false;
				//
				////设置桌子状态
				//set_desk_state(DESK_WAIT);
				//
				//
				//for (int i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
				//{
				//	notify_user(m_StGameEnd[i], i);
				//}
				//
				//if (m_round_offset>=m_round_limit && m_desk)
				//{
				//	 m_desk->HanderGameOver(1);
				//}
				//break;

			}
			default:
			{
				LLOG_ERROR("HanderGameMessage subID error not find ");
				return false;
			}

		}
		
	}

	bool   HanderYingSanZhangAddScore(User* pUser, YingSanZhangC2SAddScore*msg)
	{
		YingSanZhangC2SAddScore* pAddScore=msg;
		if (pUser==NULL || pAddScore==NULL)return false;
		//检查位置和状态
		LLOG_ERROR("deskId=[%d] addScore: userPos = [%d],m_CurrentTimes[%d],addScore[%d],score[%d],state[%d]", m_desk?m_desk->GetDeskId():0,GetUserPos(pUser), m_CurrentTimes, pAddScore->m_AddScore, pAddScore->m_Score,pAddScore->m_State );

		if (GetUserPos(pUser)==INVAILD_POS_QIPAI)
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_ADD_SCORE  user[%d] pos not ok [%d] ", pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		if (m_user_playing_status[GetUserPos(pUser)]==0)
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_ADD_SCORE  user[%d] status 0 ", pUser->m_userData.m_id);
			return false;
		}
		//当前操作用户
		if (GetUserPos(pUser)!=m_curPos)
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_ADD_SCORE  user[%d]  not curPos[%d]", pUser->m_userData.m_id, m_curPos);
			return false;
		}
		//是否结束
		if (m_GameEnd)return false;

		if (m_play_status != GAME_STATUS_PLAY)
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_ADD_SCORE  user[%d]  play_status[%d] error", pUser->m_userData.m_id, m_play_status);
			return false;
		}

		if (pAddScore->m_AddScore!=0)     //加注
		{
			//判断注的数值
			if (pAddScore->m_AddScore<=m_CurrentTimes)return false;

			switch (m_playtype.gametypeGetScoreTime())
			{
			case 1:

				if (pAddScore->m_AddScore<m_playtype.gametypeGetScoreTime()||pAddScore->m_AddScore>5)return false;
				break;
			case 2:

				if (pAddScore->m_AddScore<m_playtype.gametypeGetScoreTime()||pAddScore->m_AddScore>20)return false;
				break;
			case 3:

				if (pAddScore->m_AddScore<m_playtype.gametypeGetScoreTime()||pAddScore->m_AddScore>24)return false;
				break;
			case 4:

				if (pAddScore->m_AddScore<1||pAddScore->m_AddScore>10)return false;
				break;
			//case 5:
			//
			//	if (pAddScore->m_AddScore<m_playtype.gametypeGetScoreTime()||pAddScore->m_AddScore>45)return false;
			//	break;
			}

			//修改基础倍数
			m_CurrentTimes=pAddScore->m_AddScore;
		}

		//判断下注数目
		Lint CanScore=(m_MingZhu[GetUserPos(pUser)])?2*m_CurrentTimes:m_CurrentTimes;
		if (m_playtype.gametypeGetPeiKai() == 1)
		{
			CanScore = (pAddScore->m_State>0) ? 2 * CanScore : CanScore;
		}
		if (pAddScore->m_Score!=CanScore)
		{
			LLOG_ERROR("HanderYingSanZhangAddScore error, user score[%d],not equal canScore[%d]",pAddScore->m_Score, CanScore);
			return false;
		}
		if (pAddScore->m_State>0)m_CompareCount=pAddScore->m_Score;

		//加注，比牌不能同时出现,校验
		if ((pAddScore->m_State>0)&&(pAddScore->m_AddScore>0))return false;

		//比牌下注，需要判断
		if (pAddScore->m_State>0)
		{
			//判断第一轮
			if (m_curLun==1)return false;

			//闷牌判断
			if (m_playtype.gametypeGetMenRound()>=m_curLun)return false;
		}

		//消息处理
		UnsetDelayTime();
		return on_event_user_add_score(pUser, pAddScore->m_Score, false, ((pAddScore->m_State>0)?true:false), ((pAddScore->m_AddScore>0)?true:false));

	}

	//买大买小处理
	bool HanderYingSanZhangUserSelectBigSmall(User* pUser, YingSanZhangC2SUserSelectBigSmall* msg)
	{
		if (pUser == NULL || msg == NULL)return false;
		//检查位置和状态
		LLOG_ERROR("deskId=[%d] userPos = [%d], m_big_small_flag[%d],m_score[%d]", m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser), msg->m_flag,msg->m_score);

		if (GetUserPos(pUser) == INVAILD_POS_QIPAI)
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL  user[%d] pos not ok [%d] ", pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		if (m_user_playing_status[GetUserPos(pUser)] == 0)
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL  user[%d] status 0 ", pUser->m_userData.m_id);
			return false;
		}
		if (  (m_play_status != GAME_STATUS_BUY_SAMLL && m_play_status != GAME_STATUS_BUY_BIG) || (m_play_status== GAME_STATUS_BUY_SAMLL && msg->m_flag !=0 )  || (m_play_status == GAME_STATUS_BUY_BIG && msg->m_flag == 0) )
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL  user[%d] ,m_play_status[%d],msg->m_flag[%d] error", pUser->m_userData.m_id, m_play_status,msg->m_flag);
			return false;
		}
		if (is_user_already_buy(GetUserPos(pUser)))
		{
			LLOG_ERROR("MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL  user[%d] ,m_play_status[%d],msg->m_flag[%d] error, already buy", pUser->m_userData.m_id, m_play_status, msg->m_flag);
			return false;
		}
	
		return on_event_user_buy_small_big(GetUserPos(pUser), msg->m_flag, msg->m_score);

	}

	void Tick(LTime& curr)
	{
		if (m_desk&&m_desk->m_deskState != DESK_PLAY)return;
		if (m_play_stauts_flag &&  m_playtype.gametypeGetAddMin()==1 &&  (m_play_status == GAME_STATUS_BUY_SAMLL || m_play_status == GAME_STATUS_BUY_BIG) )
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_delay_time_count)
			{
				for (int i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
				{
					if (m_user_status[i] == 0)continue;
					if (is_user_already_buy(i))continue;;
			
					if (m_play_status == GAME_STATUS_BUY_SAMLL)
					{
						on_event_user_buy_small_big(i, 0, 0);
					}
					if (m_play_status == GAME_STATUS_BUY_BIG)
					{
						on_event_user_buy_small_big(i, 1, 0);
					}
		
				}
				return;
			}
		}

		if (m_playtype.gametypeGetAuto() == 0)return;
		if (!m_play_stauts_flag)return;                                                            
		if (m_GameEnd)return;                                                                   		//是否结束
		//LLOG_DEBUG("YingSanZhangGameHandler--Tick,  play_status=[%d]", m_play_status);
		if (curr.Secs() >= m_play_status_time.Secs() + m_delay_time_count)
		{ 
			if (m_desk&&m_desk->m_user[m_curPos])
			{
				//闷牌判断
				if (m_playtype.gametypeGetMenRound() >= m_curLun || m_AutoScore[m_curPos])
					on_event_user_add_score(m_desk->m_user[m_curPos],m_CurrentTimes,false,false,false,true);
				else
					on_event_user_give_up(m_desk->m_user[m_curPos]);
			}
		}
	
	}

};



DECLARE_GAME_HANDLER_CREATOR(102, YingSanZhangGameHandler);

// 各个玩法的 ShanXiGameHandlerCreator 作为全局变量，将自己注册到工厂里面就行了，不用管注销
// 工厂实际上就是个全局的 GameType 到 GameHandlerCreator 的映射
// 发现工厂用 GameHandlerCreator 创建出来的 GameHandler 都不带删除的，创建多少缓存多少
//struct ShanXiGameHandlerCreator: GameHandlerCreator {
//	GameType game_type;
//	ShanXiGameHandlerCreator(GameType game_type): game_type(game_type) {
//		GameHandlerFactory::getInstance()->registerCreator(game_type, this);
//	}
//
//	GameHandler *create() {
//		//YingSanZhangGameHandler *gh = new YingSanZhangGameHandler();
//		//gh->setup_feature_toggles(game_type);
//		return gh;
//	}
//};
//
//#define XX(k, v, player_count) ShanXiGameHandlerCreator g_ShanXiGameHandlerCreator__##k(k);
//POKER_GAME_TYPE_MAP(XX)
//#undef XX


 
