#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "../Config.h"
#include "../RoomVip.h"
#include "../Work.h"
#include "LMsgL2C.h"
#include "../GameHandler.h"
#include "../Utils.h" 
#include "../User.h"
#include "../Desk.h"
#include "../mhgamedef.h"

#include "CMD_Game.h"
#include "CMD_Game_Linfen.h"
#include "CMD_Game_OneLaizi.h"

#include "GameLogicClassical.h"
#include "GameLogicLaizi.h"
#include "GameLogicLinfen.h"

//#include "../LQiPaiVideo.h"


#define ASSERT(f)
#define VERIFY ASSERT

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))


//斗地主配牌
static bool debugSendCards_doudizhu(BYTE* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
{
	if (conFileName == NULL || std::strlen(conFileName) == 0)
	{
		return false;
	}

	//配置文件中是否配置允许调牌
	bool can_assign = gConfig.GetDebugModel();

	FILE *fp = NULL;
	try
	{
		fp = fopen(conFileName, "r");
	}
	catch (const std::exception& e)
	{
		LLOG_ERROR("File card.ini is open failed...  desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);
		if (fp) fclose(fp);
		fp = NULL;
	}

	//允许服务器端配牌 && card.ini存在
	if (can_assign && fp)
	{
		LLOG_DEBUG("Send Cards...This is debug mode!!! desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//用于存储配牌的整体变量
		//BYTE t_cbRandCard[PLAY_TYPE_NORMAL_CARD_TOTAL];
		BYTE* t_cbRandCard = new BYTE[cellPackCount];
		memset(t_cbRandCard, 0, sizeof(BYTE) * cellPackCount);

		//解析card.ini得到的配牌
		std::vector<BYTE> t_sendCards;

		//解析牌后每个人配牌的合法张数
		size_t* t_sendCount = new size_t[playerCount];
		memset(t_sendCount, 0, sizeof(t_sendCount));
		size_t t_count = 0;

		//给每个玩家解析有效的配牌
		for (int i = 0; i < playerCount; ++i)
		{
			t_sendCards.clear();
			//解析个人玩家手牌
			LoadPlayerCards(t_sendCards, i + 1, conFileName);
			t_count = 0;

			for (int j = 0; j < t_sendCards.size(); ++j)  //配的牌vector
			{
				for (int k = 0; k < cellPackCount; ++k)  //1副牌
				{
					if (t_sendCards[j] == cbRandCard[k])
					{
						t_cbRandCard[i * handCardsCount + t_count++] = cbRandCard[k];
						cbRandCard[k] = 0;
						break;
					}
				}
			}
			t_sendCount[i] = t_count;
		}

		//解析配牌有效底牌
		t_sendCards.clear();
		LoadBaseCards(t_sendCards, conFileName);
		size_t t_baseCount = 0;
		for (int j = 0; j < t_sendCards.size(); ++j)  //配的牌vector
		{
			for (int k = 0; k < cellPackCount; ++k)  //1副牌
			{
				if (t_sendCards[j] == cbRandCard[k])
				{
					t_cbRandCard[playerCount * handCardsCount + t_baseCount++] = cbRandCard[k];
					cbRandCard[k] = 0;
					break;
				}
			}
		}

		//补全手牌
		for (int i = 0; i < playerCount; ++i)
		{
			for (int j = i * handCardsCount + t_sendCount[i]; j < (i+1) * handCardsCount; )
			{
				for (int k = 0; k < cellPackCount; ++k)
				{
					if (cbRandCard[k] != 0)
					{
						t_cbRandCard[j++] = cbRandCard[k];
						cbRandCard[k] = 0;
						break;
					}
				}
			}
		}

		//补全底牌
		for (int j = playerCount * handCardsCount + t_baseCount; j < cellPackCount; )
		{
			for (int k = 0; k < cellPackCount; ++k)
			{
				if (cbRandCard[k] != 0)
				{
					t_cbRandCard[j++] = cbRandCard[k];
					cbRandCard[k] = 0;
					break;
				}
			}
		}

		//将调好的牌复制给传出函数
		memcpy(cbRandCard, t_cbRandCard, sizeof(BYTE) * cellPackCount);

		if(t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_sendCount);
		t_sendCount = NULL;

		if (fp) fclose(fp);
		fp = NULL;
		return true;
	} //if (can_assign && fp)

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}


//斗地主特殊玩法
enum DOUDIZHU_PLAY_TYPE
{
	PLAY_TYPE_NORMAL=1,                       //普通斗地主
	PLAY_TYPE_LAIZI =2,                             //一个癞子斗地主
	PLAY_TYPE_LINFEN=3,                        //临汾去两二斗地主
};

//牌背面 
#define CARD_BACK_PATTERN      0x43

//////////////////////////////////////////////////////////////////////////////
//状态定义
#define DOUDIZHU_GAME_SCENE_CALL   1
#define DOUDIZHU_GAME_SCENE_PLAY  3

#define GAME_SCENE_LINFEN_TIPAI              21  			//踢牌状态
#define GAME_SCENE_LINFEN_GENTI              22  			//踢牌状态
#define GAME_SCENE_LINFEN_HUITI              23  			//回踢状态


//////////////////////////////////////////////////////////////////////////////


enum GAMG_CONCLUDE_REASON
{
	GAMG_CONCLUDE_COMPARECARD,
	GAMG_CONCLUDE_NOPLAYER,
	GAMG_CONCLUDE_SYSTEM_COMPARECARD,
	GAMG_CONCLUDE_DISMISS
};

//玩法小选项
struct DouDiZhuPlayType
{
public:
	void clear() { m_playtype.clear(); }
	void setAllTypes(const std::vector<Lint>& v) { m_playtype = v; }
	const std::vector<Lint>& getAllTypes() const { return m_playtype; }

	Lint   DouDiZhuGetGameMode() const;                      //1玩法类型              1：经典斗地主  2:一个癞子的斗地主   3：临汾去两二
	Lint   DouDiZhuDiPaiType() const;                            //2底牌显示或扣牌         1：底牌明牌    2：底牌扣牌（只对地主可见）
	Lint   DouDiZhuBombMaxCount() const;               //3炸弹封顶个数          0：为不封底。  其他为具体个数
	Lint   DouDiZhuCellScore() const;                            //4底分
	Lint   DouDiZhuAnonymous() const;                       //5匿名
	Lint   DouDiZhuPiPeiRenCount() const;                //6匹配的人数，manager 上用到，logic上没有用这个字段
	Lint   DouDiZhuCallZhuangMode() const;            //7抢庄类型
	Lint   DouDiZhuIsTi() const;                                    //8经典和带花，踢牌选项
	Lint   DouDiZhuIsTongHua3() const;                   //9只有同花色对3为炸
	Lint   DouDiZhuLinfenTiToZhaFengDing()const;       //10临汾选择，踢牌是否算入炸封顶
	
protected:
	std::vector<Lint>	m_playtype;
};

Lint DouDiZhuPlayType::DouDiZhuGetGameMode() const
{
	if (m_playtype.empty() || m_playtype[0]<= 0 || m_playtype[0] > 3)  return 1;
	return m_playtype[0];
}

Lint DouDiZhuPlayType::DouDiZhuDiPaiType() const
{
	if (m_playtype.size()<2)return 1;
	return m_playtype[1];
}

Lint DouDiZhuPlayType::DouDiZhuBombMaxCount() const
{
	if (m_playtype.size()<3) return 100;           //不封底
	return m_playtype[2]==0 ?100 : m_playtype[2];
}

Lint  DouDiZhuPlayType::DouDiZhuCellScore() const
{
	if (m_playtype.size()<4) return 1;           //
	if (m_playtype[3] > 0 && m_playtype[3] <= 5)return m_playtype[3];
	else
		return 1;
}

Lint DouDiZhuPlayType::DouDiZhuAnonymous() const
{
	if (m_playtype.size()<5) return 0;           //匿名
	return m_playtype[4];
}

Lint DouDiZhuPlayType::DouDiZhuCallZhuangMode() const
{
	if (m_playtype.size()<7) return 0;           //抢庄类型
	return m_playtype[6];
}

Lint DouDiZhuPlayType::DouDiZhuIsTi() const
{
	if (m_playtype.size()<8) return 0;           //不踢类型
	if (m_playtype[0] == 1 || m_playtype[0] == 2)
		return m_playtype[7];
	else
		return 0;
}

Lint   DouDiZhuPlayType::DouDiZhuIsTongHua3() const
{
	if (m_playtype.size()<9) return 0;           //同花色对3为炸
	return m_playtype[8];
}

//10临汾选择，踢牌是否算入炸封顶  1-算封顶  0-不算
Lint   DouDiZhuPlayType::DouDiZhuLinfenTiToZhaFengDing()const
{
	if (m_playtype.size()<10) return 0;           //
	if (DouDiZhuGetGameMode() != 3)return 0;
	return m_playtype[9];

}


// 不会自动初始化的字段放这里，每盘自动重置
struct DouDiZhuRoundState__c_part
{
	Lint         m_user_status[DOUDIZHU_PLAY_USER_COUNT];         //用户状态
	Lint         m_play_status;	                                                                     // 游戏状态
	
   //  斗地主变量定义
	WORD							m_wFirstUser;						//首叫用户
	WORD							m_wBankerUser;				   //庄家用户
	WORD							m_wCurrentUser;						//当前玩家
	BYTE							m_cbOutCardCount[DOUDIZHU_PLAY_USER_COUNT];		//出牌次数
	

	//炸弹信息
	BYTE							m_cbBombCount;						//炸弹个数
	BYTE							m_cbEachBombCount[DOUDIZHU_PLAY_USER_COUNT];		//炸弹个数

	//叫分信息
	BYTE							m_cbCallScoreCount;				//叫分次数
	BYTE							m_cbBankerScore;					//庄家叫分
	BYTE							m_cbScoreInfo[DOUDIZHU_PLAY_USER_COUNT];		//叫分信息 ,默认255

	//出牌信息
	WORD							m_wTurnWiner;						   //胜利玩家
	BYTE							m_cbTurnCardCount;					//出牌数目
	BYTE							m_cbTurnCardData[PLAY_TYPE_CARD_REST_MAX + PLAY_TYPE_CARD_HAND_MAX];		//出牌数据 改为21张
	
	//扑克信息
	BYTE							m_cbBankerCard[PLAY_TYPE_CARD_REST_MAX];					//游戏底牌-3张  改为4张
	BYTE							m_cbHandCardCount[DOUDIZHU_PLAY_USER_COUNT];		//扑克数目 0-21
	BYTE							m_cbHandCardData[DOUDIZHU_PLAY_USER_COUNT][PLAY_TYPE_CARD_REST_MAX +PLAY_TYPE_CARD_HAND_MAX];	//手上扑克 改为21张

	BYTE                          m_BombCountLimit;                        //炸弹限制  
	BYTE						  m_cbBombCountReal;						//实际炸弹个数,    m_cbBombCount 变量当达到limit后不再计数
	BYTE						  m_cbEachBombCountReal[DOUDIZHU_PLAY_USER_COUNT];		//每个用户实际炸弹个数, 同上

	//临汾去两二
	LONG							m_cbBeiShu[DOUDIZHU_PLAY_USER_COUNT];			//用户倍数

	BYTE							m_cbIsTi[DOUDIZHU_PLAY_USER_COUNT];			//，  0xff-初始状态，0-没有选择，1-踢牌，2-跟踢,3-回牌
	WORD                       m_wTiPaiUser;                        //踢牌用户                         
	WORD                       m_wGenTiUser;                        // 跟踢用户

	WORD                       m_onlyCallUser;                     //因为特殊牌型，只能叫庄的用户

	
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_play_status = DOUDIZHU_GAME_SCENE_CALL;	
		//m_curPos = DOUDIZHU_INVAILD_POS;
		m_wFirstUser = DOUDIZHU_INVAILD_POS;
		m_wBankerUser = DOUDIZHU_INVAILD_POS;
		m_wCurrentUser = DOUDIZHU_INVAILD_POS;
		m_wTurnWiner = DOUDIZHU_INVAILD_POS;
		m_wTiPaiUser = DOUDIZHU_INVAILD_POS;
		m_wGenTiUser = DOUDIZHU_INVAILD_POS;
		m_onlyCallUser = 100;
		memset(m_cbIsTi, 0xff, sizeof(m_cbIsTi));
		memset(m_cbScoreInfo, 0xff, sizeof(m_cbScoreInfo));
	}

};

// 有构造函数的字段放这里，每盘自动重置
struct DouDiZhuRoundState__cxx_part
{
	LTime m_play_status_time;
	void clear_round()
	{
		this->~DouDiZhuRoundState__cxx_part();
		new (this) DouDiZhuRoundState__cxx_part;
	}
};

struct DouDiZhuMatchState__c_part
{
	Desk*         m_desk;
	int           m_round_offset;
	int           m_round_limit;
	int           m_accum_score[DOUDIZHU_PLAY_USER_COUNT];
	bool          m_dismissed;

	int           m_registered_game_type;
	int           m_player_count;   // 玩法核心代码所使用的玩家数量字段
	Lint          m_zhuangPos;      //庄家位置
	Lint          m_firstCallZhuang;
	//BYTE           m_cbCustomGameRule[128];

	void clear_match()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct DouDiZhuMatchState__cxx_part
{	
	DouDiZhuPlayType m_playtype;	                                         // 桌子玩法

	CGameLogicClassical		 m_GameLogicClassical;			//游戏逻辑-经典
	CGameLogicLinfen             m_GameLogicLinfen;              //游戏逻辑-临汾					
	CGameLogicLaizi             m_GameLogicLaizi;                   //游戏逻辑-癞子

	void clear_match() {
		this->~DouDiZhuMatchState__cxx_part();
		new (this) DouDiZhuMatchState__cxx_part;
	}
};


struct DouDiZhuRoundState : DouDiZhuRoundState__c_part, DouDiZhuRoundState__cxx_part {
	// 不要在这里加字段
	void clear_round() {
		DouDiZhuRoundState__c_part::clear_round();
		DouDiZhuRoundState__cxx_part::clear_round();
	}
};

// 一场Match会有好几盘Round
struct DouDiZhuMatchState : DouDiZhuMatchState__c_part, DouDiZhuMatchState__cxx_part {
	// 不要在这里加字段
	void clear_match() {
		DouDiZhuMatchState__c_part::clear_match();
		DouDiZhuMatchState__cxx_part::clear_match();
	}
};

struct DouDiZhuDeskState :DouDiZhuRoundState, DouDiZhuMatchState {
	void clear_match(int player_count) {
		DouDiZhuMatchState::clear_match();
		clear_round();
		m_player_count = player_count;
	}

	void clear_round() {
		DouDiZhuRoundState::clear_round();
	}

	void setup_match(std::vector<Lint> &l_playtype, int round_limit) {
		m_round_limit = round_limit;
		//// 小选项
		m_playtype.setAllTypes(l_playtype);


	}

	void increase_round() {
		++m_round_offset;
		if (m_desk && m_desk->m_vip) ++m_desk->m_vip->m_curCircle;
	}

	void set_desk_state(DESK_STATE v) {
		if (m_desk) {
			if (v == DESK_WAIT) m_desk->SetDeskWait();
			else m_desk->setDeskState(v);
		}
	}
	DESK_STATE get_desk_state() {
		// 桌子就 DESK_PLAY/DESK_WAIT/DESK_FREE 三种状态
		// 单元测试时没有Desk对象，一直是 DESK_PLAY 状态
		if (m_desk) return (DESK_STATE)m_desk->getDeskState();
		else return DESK_PLAY;
	}

	void set_play_status(Lint play_status)
	{
		//if (play_status >= GAME_PLAY_SELECT_ZHUANG&&play_status <= GAME_PLAY_END)
			m_play_status = play_status;
	}

	Lint GetUserPos(User* pUser)
	{
		if (pUser == NULL) return DOUDIZHU_INVAILD_POS;
		Lint pos = DOUDIZHU_INVAILD_POS;
		if (m_desk)
			pos = m_desk->GetUserPos(pUser);
		return pos == INVAILD_POS_QIPAI ?  DOUDIZHU_INVAILD_POS : pos;
	}

	//取的有效的下个位置
	int GetNextPos(int pos) {
		if (m_player_count == 0) return 0; // 避免除零崩溃	
		Lint nextPos = (pos + 1) % m_player_count;
		for (int i = 0; (i<m_player_count) && (m_user_status[nextPos] != 1); i++)
		{
			nextPos = (nextPos + 1) % m_player_count;
		}
		return nextPos;
	}

	bool is_pos_valid(int pos) {
		if (NULL == m_desk) return false;
		return 0 <= pos && pos < DOUDIZHU_PLAY_USER_COUNT;
	}

};


struct DouDiZhuVideoSupport : DouDiZhuDeskState {
	QiPaiVideoLog		m_video;						//录像

	///////////////////////////////////////////////////////////////////
    // 这块集中存放和录像有关的函数
	///////////////////////////////////////////////////////////////////
	void VideoSave() {
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		//m_video.m_playType = m_desk->getPlayType();
		//m_video.m_flag= m_desk->m_state;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		LLOG_DEBUG("videoSave[%s]",video.m_sql.c_str());
		gWork.SendMsgToDb(video);
	}

	//斗地主 oper 操作  0--过  1--出牌
	void SaveUserOper(Lint oper, Lint pos, BYTE handcardCount, BYTE* cbHandCardData, Lint cardCount = 0, BYTE *cbCardData = NULL,Lint score=0)
	{
		LLOG_DEBUG("SaveUserOper  pos[%d],oper[%d]",pos,oper);
		
		std::vector<Lint> cards;
		std::vector<Lint> handcards;
		if (cardCount != 0)
		{
			if (cbCardData != NULL)
			{
				for (int i = 0; i < cardCount; i++)
				{
					LLOG_DEBUG("cards[%d]=[%d]",i, cbCardData[i]);
					cards.push_back((Lint)cbCardData[i]);
				}
			}
		}
		if (handcardCount != 0)
		{
			if (cbHandCardData != NULL)
			{
				for (int i = 0; i < handcardCount; i++)
				{
					LLOG_DEBUG("cbHandCardData[%d]=[%d]", i, cbHandCardData[i]);
					handcards.push_back((Lint)cbHandCardData[i]);
				}
			}

		}
		m_video.AddOper(oper, pos, cards,score,handcards);
	}

};

struct DouDiZhuGameCore : GameHandler, DouDiZhuVideoSupport{

	inline bool is_normal()
	{
		return m_playtype.DouDiZhuGetGameMode() == PLAY_TYPE_NORMAL;
	}
	inline bool is_linfen() {
		return m_playtype.DouDiZhuGetGameMode() == PLAY_TYPE_LINFEN;
	}
	inline bool is_laizi()
	{
		return m_playtype.DouDiZhuGetGameMode() == PLAY_TYPE_LAIZI;
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
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;	
		notify_desk(msg);
	}

	void add_round_log(Lint *score, int zhuangPos) {

		//保存回访
		m_video.addUserScore(score);
		if(m_desk && m_desk->m_vip&&m_desk->m_vip->m_reset==0)
			VideoSave();

		// 保存结果	
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, score, m_user_status, zhuangPos, m_video.m_Id);			
		}

	}

	//通知地主开始游戏
	void OnEventBankerOutCard()
	{
		LLOG_DEBUG("OnEventBankerOutCard");
	
		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_BankerOutCard bankerOutCard;
		bankerOutCard.m_Pos = m_wBankerUser;
		CopyMemory(&msg.cmdData.cmd_s2c_bank_out_card, &bankerOutCard, sizeof(bankerOutCard));
		msg.sub_cmd_code = SUB_S_LINFEN_BANKER_OUT_CARD;
		notify_desk(msg);
		//notify_user(msg, m_wBankerUser);

		//更新用户
		m_wCurrentUser = m_wBankerUser;

		//设置状态
		this->set_play_status(DOUDIZHU_GAME_SCENE_PLAY);

		Lint tmpTipai[DOUDIZHU_PLAY_USER_COUNT];
		memset(tmpTipai, 0x00, sizeof(tmpTipai));
		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			//LLOG_DEBUG("m_cbIsTi [%d]=[%d]", i, m_cbIsTi[i]);
			tmpTipai[i] =(Lint)( (m_cbIsTi[i] == 0xff) ? 0 : m_cbIsTi[i]);
			//LLOG_DEBUG("tempTipai [%d]=[%d]",i, tmpTipai[i]);
		}
		m_video.doudizhuTiPai(DOUDIZHU_PLAY_USER_COUNT, tmpTipai);
		if (is_linfen()&&m_playtype.DouDiZhuLinfenTiToZhaFengDing()!=0)
		{
			Lint tiCount = 0;
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (tmpTipai[i] != 0 && tmpTipai[i] != 3)tiCount++;
				if (tmpTipai[i] == 3)m_cbBombCount++;
			}
			if (tiCount == 2)m_cbBombCount++;
		}

	}

	//记录炸弹
	void OnEventRecordBombCount(WORD wChairID)
	{
		//if (m_cbBombCount < m_BombCountLimit || m_pGameServiceOption->wServerType == GAME_GENRE_SAISHI)
		if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
		{
			m_cbBombCount++;
			m_cbEachBombCount[wChairID]++;
			LLOG_ERROR("OnEventRecordBombCount desk:%d ,pos[%d]  bombNum[%d],maxBomb[%d] ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_cbBombCount, m_playtype.DouDiZhuBombMaxCount());
			//char szString[512] = ("");
			//_snprintf(szString, CountArray(szString), "炸弹，炸弹个数[%d]", m_cbBombCount);
			//LLOG_DEBUG(szString);
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
				m_cbBeiShu[i] *= 2;

			OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
		}
		else
		{
			LLOG_ERROR("OnEventRecordBombCount desk:%d  bombNum[%d],maxBomb[%d] ", m_desk ? m_desk->GetDeskId() : 0, m_cbBombCount,m_playtype.DouDiZhuBombMaxCount());
			//char szString[512] = ("");
			//_snprintf(szString, CountArray(szString), "炸弹封顶不再计分，炸弹个数[%d]", m_cbBombCount);
			//LLOG_DEBUG(szString);
		}

		m_cbBombCountReal++;
		m_cbEachBombCountReal[wChairID]++;

	}

	//刷新倍数
	bool OnEventUserFlushScore(WORD wChairID)
	{
		LLOG_DEBUG("OnEventUserFlushScore ");
		//群发信息
		if (wChairID == DOUDIZHU_INVAILD_POS)
		{
			//发送刷新倍数
			for (BYTE i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				struct CMD_S_ScoreFresh ScoreFresh;
				ZeroMemory(&ScoreFresh, sizeof(ScoreFresh));
				ScoreFresh.score = m_cbBeiShu[i];
				CopyMemory(&msg.cmdData.cmd_s2c_score_fresh, &ScoreFresh, sizeof(CMD_S_ScoreFresh));
				//msg.cmdData.cmd_s2c_score_fresh.score = m_cbBeiShu[i];
				msg.sub_cmd_code = SUB_S_LINFEN_SCORE_FRESH;
				notify_user(msg, i);
			}

		}
		if (wChairID >= 0 && wChairID<DOUDIZHU_PLAY_USER_COUNT)
		{
			MHLMsgDouDiZhuS2CGameMessage msg;
			//发送刷新倍数
			struct CMD_S_ScoreFresh ScoreFresh;
			ZeroMemory(&ScoreFresh, sizeof(ScoreFresh));
			ScoreFresh.score = m_cbBeiShu[wChairID];
			CopyMemory(&msg.cmdData.cmd_s2c_score_fresh, &ScoreFresh, sizeof(CMD_S_ScoreFresh));
			//msg.cmdData.cmd_s2c_score_fresh.score = m_cbBeiShu[wChairID];
			msg.sub_cmd_code = SUB_S_LINFEN_SCORE_FRESH;
			notify_user(msg, wChairID);
		}
		return true;
	}

	//广播用户踢的信息
	void OnEventBroadcaseTiResult(BYTE cmdType, WORD wChairID, BOOL isYes)
	{
		LLOG_DEBUG("OnEventBroadcaseTiResult  [%d],pos[%d],isYes[%d]", cmdType, wChairID, isYes);
		CMD_S_TiResult  TiResult;
		ZeroMemory(&TiResult, sizeof(TiResult));
		TiResult.wActUser = wChairID;
		TiResult.wActSelect = isYes;
		
		MHLMsgDouDiZhuS2CGameMessage msg;
		CopyMemory(&msg.cmdData.cmd_s2c_ti_result, &TiResult, sizeof(TiResult));
		switch (cmdType)
		{
		case SUB_S_LINFEN_TIPAI:
		{
			msg.sub_cmd_code = SUB_S_LINFEN_TIPAI_RESULT;
			break;
		}
		case SUB_S_LINFEN_GENTI:
		{
			msg.sub_cmd_code = SUB_S_LINFEN_GENTI_RESULT;
			break;
		}
		case SUB_S_LINFEN_HUIPAI:
		{
			msg.sub_cmd_code = SUB_S_LINFEN_HUIPAI_RESULT;
			break;
		}
		}
		notify_desk(msg);

	}

	//游戏开始
	bool OnEventGameStart()
	{
		
		////炸弹个数		
		m_BombCountLimit = m_playtype.DouDiZhuBombMaxCount();		 

		//出牌信息
		m_cbTurnCardCount = 0;
		m_wTurnWiner = DOUDIZHU_INVAILD_POS;
		ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));

		//设置状态
		set_play_status(DOUDIZHU_GAME_SCENE_CALL);

		//混乱扑克
		BYTE cbRandCard[PLAY_TYPE_CARD_MAX];

		switch (m_playtype.DouDiZhuGetGameMode())
		{
		case PLAY_TYPE_NORMAL:
		{
			m_GameLogicClassical.RandCardList(cbRandCard, PLAY_TYPE_NORMAL_CARD_TOTAL);

			//斗地主配牌
			debugSendCards_doudizhu(cbRandCard, m_desk, "card_doudizhu.ini", DOUDIZHU_PLAY_USER_COUNT, 17, PLAY_TYPE_NORMAL_CARD_TOTAL);

			break;
		}
		case PLAY_TYPE_LAIZI:
		{
			m_GameLogicLaizi.RandCardList(cbRandCard, PLAY_TYPE_LAIZI_CARD_TOTAL);

			//斗地主配牌
			debugSendCards_doudizhu(cbRandCard, m_desk, "card_doudizhu.ini", DOUDIZHU_PLAY_USER_COUNT, 17, PLAY_TYPE_LAIZI_CARD_TOTAL);

			break;
		}
		case PLAY_TYPE_LINFEN:
		{
			m_GameLogicLinfen.RandCardList(cbRandCard, PLAY_TYPE_LINFEN_CARD_TOTAL);

			//斗地主配牌
			debugSendCards_doudizhu(cbRandCard, m_desk, "card_doudizhu.ini", DOUDIZHU_PLAY_USER_COUNT, 16, PLAY_TYPE_LINFEN_CARD_TOTAL);

			break;
		}

		}
		
		//抽取明牌
		BYTE cbValidCardData = 0;
		BYTE cbValidCardIndex = 0;
		WORD wStartUser = m_wFirstUser;
		WORD wCurrentUser = m_wFirstUser;

		//抽取玩家
		if (wStartUser == DOUDIZHU_INVAILD_POS)
		{
			//抽取扑克
			cbValidCardIndex = rand() % 51;
			cbValidCardData = cbRandCard[cbValidCardIndex];

			////设置用户
			wStartUser = m_GameLogicClassical.GetCardValue(cbValidCardData) % DOUDIZHU_PLAY_USER_COUNT;
			wCurrentUser = (wStartUser + cbValidCardIndex / 54) % DOUDIZHU_PLAY_USER_COUNT;

			wStartUser = 0;

			wCurrentUser = wStartUser;

		}


		//用户扑克
		Lint userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT;
		Lint bankerCardCount = PLAY_TYPE_CARD_REST_COUNT;
		for (WORD i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			WORD wUserIndex = (wStartUser + i) % DOUDIZHU_PLAY_USER_COUNT;
			
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				m_cbHandCardCount[wUserIndex] = PLAY_TYPE_CARD_HAND_COUNT;
				break;
			}
			case PLAY_TYPE_LAIZI:
			{
				m_cbHandCardCount[wUserIndex] = PLAY_TYPE_CARD_HAND_COUNT;
				bankerCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				break;
			}
			case PLAY_TYPE_LINFEN:
			{
				userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT_LINFEN;
				m_cbHandCardCount[wUserIndex] = PLAY_TYPE_CARD_HAND_COUNT_LINFEN;
				bankerCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				break;
			}
			}
			//m_cbHandCardCount[wUserIndex] = NORMAL_COUNT;
			CopyMemory(&m_cbHandCardData[wUserIndex], &cbRandCard[i*m_cbHandCardCount[wUserIndex]], sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);
		}

		//设置底牌
		CopyMemory(m_cbBankerCard, &cbRandCard[userHandCardCount*DOUDIZHU_PLAY_USER_COUNT], sizeof(BYTE)*bankerCardCount);

		if (is_linfen())
		{
			m_onlyCallUser = 100;
			//特殊牌型用户
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_GameLogicLinfen.HaveSpecCard(m_cbHandCardData[i], m_cbHandCardCount[i]))
					m_onlyCallUser = i;
			}
		}
		else if (is_normal())
		{
			m_onlyCallUser = 100;
			//特殊牌型用户
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_GameLogicClassical.HaveSpecCard(m_cbHandCardData[i], m_cbHandCardCount[i]))
					m_onlyCallUser = i;
			}
		}


		//设置用户
		if (m_desk&&m_desk->m_vip->m_curCircle == 0)
		{
			m_wFirstUser = wCurrentUser;
			m_wCurrentUser = wCurrentUser;
			m_firstCallZhuang = wCurrentUser;
		}
		else
		{
			m_wFirstUser = m_firstCallZhuang;
			m_wCurrentUser = m_firstCallZhuang;
		}

		//发送数据
		for (WORD i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			MHLMsgDouDiZhuS2CGameMessage msg;
			//构造扑克
			CopyMemory(msg.cmdData.cmd_s2c_game_start.cbCardData, m_cbHandCardData[i], sizeof(BYTE)*m_cbHandCardCount[i]);		
			msg.sub_cmd_code = SUB_S_GAME_START;
			msg.cmdData.cmd_s2c_game_start.wStartUser = m_wFirstUser;
			msg.cmdData.cmd_s2c_game_start.wCurrentUser = m_wCurrentUser;
			msg.cmdData.cmd_s2c_game_start.cbValidCardData = cbValidCardData;
			msg.cmdData.cmd_s2c_game_start.cbValidCardIndex = cbValidCardIndex;
			msg.cmdData.cmd_s2c_game_start.cbCardCount = userHandCardCount;
			for (Lint i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_desk && m_desk->m_vip)
				{
					msg.m_totalScore.push_back(m_desk->m_vip->m_score[i]);
					//LLOG_DEBUG("startgame.score[%d]=[%d]", i, msg.cmdData.cmd_s2c_game_start.cbScore[i]);
				}
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null");
			}

			if (is_linfen() && i == m_onlyCallUser)
				msg.cmdData.cmd_s2c_game_start.cbOnlyCall = 1;
			if (is_normal() && i == m_onlyCallUser)
				msg.cmdData.cmd_s2c_game_start.cbOnlyCall = 1;

		

			notify_user(msg, i);
			std::stringstream ss;
			for (int k = 0; k < ARRAYSIZE( msg.cmdData.cmd_s2c_game_start.cbCardData); k++)
			{
				ss << (int)msg.cmdData.cmd_s2c_game_start.cbCardData[k] << ",";
				//LLOG_DEBUG(" desk[%d] pos[%d] Card index: %d value: %d", m_desk?m_desk->GetDeskId():0,i,k, msg.cmdData.cmd_s2c_game_start.cbCardData[k]);
				//LLOG_DEBUG("m_cbHandCardData card index: %d value: %d", k, m_cbHandCardData[i][k]);
			}
			LLOG_ERROR(" desk[%d] pos[%d] Card [%s]", m_desk ? m_desk->GetDeskId() : 0, i, ss.str().c_str());
		}

		if (is_linfen() || m_playtype.DouDiZhuIsTi()!=0)
		{
			//倍数，基础为1
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
				m_cbBeiShu[i] = 1;
		
			//刷新倍数
			//if (is_linfen())
				OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
		}

		//排列扑克
		for (WORD i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				m_GameLogicClassical.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i], ST_ORDER);
				break;
			}
			case PLAY_TYPE_LAIZI:
			{
				m_GameLogicLaizi.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i]);
				break;
			}
			case PLAY_TYPE_LINFEN:
			{
				m_GameLogicLinfen.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i], ST_ORDER);
				break;
			}

			}
			//m_GameLogicClassical.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i], ST_ORDER);
		}		

		return true;
	}

	//用户叫分
	bool OnUserCallScore(WORD wChairID, BYTE cbCallScore)
	{
		LLOG_ERROR("OnUserCallScore desk:%d pos:%d call_score:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbCallScore);
	
		//效验状态
		if (wChairID != m_wCurrentUser)
		{
			LLOG_ERROR("OnUserCallScore userPos[%d] not currentPos[%d]", wChairID, m_wCurrentUser);
			return true;
		}

		//设置叫分
		m_cbScoreInfo[wChairID] = cbCallScore;

		//效验参数
		if (  ((cbCallScore<1) || (cbCallScore>3) || (cbCallScore <= m_cbBankerScore))    && (cbCallScore != 255)  )
			cbCallScore = 255;

		//设置状态
		if (cbCallScore != 0xFF)
		{
			m_cbBankerScore = cbCallScore;
			m_wBankerUser = m_wCurrentUser;
		}

	

		//设置用户
		if ((m_cbBankerScore == 3) || (m_wFirstUser ==GetNextPos (wChairID) )  ) 
		{
			m_wCurrentUser = DOUDIZHU_INVAILD_POS;
		}
		else
		{
			m_wCurrentUser = GetNextPos(wChairID);
		}		

		//发送叫分
		MHLMsgDouDiZhuS2CGameMessage msg;
		msg.sub_cmd_code = SUB_S_CALL_SCORE;
		msg.cmdData.cmd_s2c_call_score.wCallScoreUser = wChairID;
		msg.cmdData.cmd_s2c_call_score.wCurrentUser = m_wCurrentUser;
		msg.cmdData.cmd_s2c_call_score.cbUserCallScore = m_cbBankerScore;
		msg.cmdData.cmd_s2c_call_score.cbCurrentScore = m_cbScoreInfo[wChairID];

		if (is_linfen() && m_wCurrentUser == m_onlyCallUser)
			msg.cmdData.cmd_s2c_call_score.cbOnlyCall = 1;
		if (is_normal() && m_wCurrentUser == m_onlyCallUser)
			msg.cmdData.cmd_s2c_call_score.cbOnlyCall = 1;

		notify_desk(msg);
	

		//开始判断
		if ((m_cbBankerScore == 3) || (m_wFirstUser == (wChairID + 1) % DOUDIZHU_PLAY_USER_COUNT))
		{
			//无人叫分
			if (m_cbBankerScore == 0)
			{
				//if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0)
				//{
				//	if (m_cbCallScoreCount < 2)
				//	{
				//		m_cbCallScoreCount++;
				//		return OnEventGameStart();
				//	}
				//}
				//return OnEventGameStart();
				//m_wBankerUser = m_wFirstUser;
				//m_cbBankerScore = 1;
				 start_round(m_user_status);
				 return true;
			}

		
			//设置状态			
			set_play_status(DOUDIZHU_GAME_SCENE_PLAY);

			//设置变量
			if (m_cbBankerScore == 0) m_cbBankerScore = 1;
			if (m_playtype.DouDiZhuIsTi() != 0 || is_linfen())m_cbBankerScore = 1;
			if (m_wBankerUser == DOUDIZHU_INVAILD_POS) m_wBankerUser = m_wFirstUser;
			m_cbCallScoreCount = 0;

			//发送底牌
			Lint userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT;
			Lint restCardCount = PLAY_TYPE_CARD_REST_COUNT;
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				m_cbHandCardCount[m_wBankerUser] += PLAY_TYPE_CARD_REST_COUNT;
				CopyMemory(&m_cbHandCardData[m_wBankerUser][userHandCardCount], m_cbBankerCard, sizeof(m_cbBankerCard));
				m_GameLogicClassical.SortCardList(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], ST_ORDER);
				break;
			}
			case PLAY_TYPE_LAIZI:
			{
				restCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				m_cbHandCardCount[m_wBankerUser] += PLAY_TYPE_CARD_REST_COUNT_4;
				CopyMemory(&m_cbHandCardData[m_wBankerUser][userHandCardCount], m_cbBankerCard, sizeof(m_cbBankerCard));
				m_GameLogicLaizi.SortCardList(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser]);
				break;
			}
			case PLAY_TYPE_LINFEN:
			{
				restCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT_LINFEN;
				m_cbHandCardCount[m_wBankerUser] += PLAY_TYPE_CARD_REST_COUNT_4;
				CopyMemory(&m_cbHandCardData[m_wBankerUser][userHandCardCount], m_cbBankerCard, sizeof(m_cbBankerCard));
				m_GameLogicLinfen.SortCardList(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], ST_ORDER);
				break;
			}
			}
		

			//设置用户
			m_wTurnWiner = (m_wBankerUser+2)% DOUDIZHU_PLAY_USER_COUNT;
			m_wCurrentUser = m_wBankerUser;

			//发送消息--庄家信息
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				//发送庄家叫分
				MHLMsgDouDiZhuS2CGameMessage msg;
				CMD_S_BankerInfo & bankerInfo = msg.cmdData.cmd_s2c_banker_info;
				msg.sub_cmd_code = SUB_S_BANKER_INFO;
				bankerInfo.wBankerUser = m_wBankerUser;
				bankerInfo.wCurrentUser = m_wCurrentUser;
				bankerInfo.cbBankerScore = m_cbBankerScore;
				bankerInfo.cbBankerCardCount = restCardCount;

			
				if (m_playtype.DouDiZhuDiPaiType() == 2 && i != m_wBankerUser)
				{
					RtlFillMemory(bankerInfo.cbBankerCard, sizeof(m_cbBankerCard), CARD_BACK_PATTERN);
					notify_user(msg, i);					
				}
				else
				{
					CopyMemory(bankerInfo.cbBankerCard, m_cbBankerCard, sizeof(m_cbBankerCard));
					notify_user(msg, i);					
				}
			}


			//保存回放手牌,底牌和局数
			if (m_desk)
			{
				std::vector<Lint> handCard[DOUDIZHU_PLAY_USER_COUNT];
				std::vector<Lint> retCard;
				for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
				{
					for (int j = 0; j < PLAY_TYPE_CARD_REST_MAX + PLAY_TYPE_CARD_HAND_MAX; j++)
						handCard[i].push_back((Lint)m_cbHandCardData[i][j]);
				}
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
					retCard.push_back((Lint)m_cbBankerCard[i]);
				LTime  tempNow;
				m_video.DealCard(m_round_offset, DOUDIZHU_PLAY_USER_COUNT, m_desk->m_vip->m_posUserId, handCard, retCard, tempNow.Secs(), (Lint)m_wBankerUser, m_cbBankerScore);
			}
	
			if (is_linfen() || m_playtype.DouDiZhuIsTi()!=0)
			{
				
					//庄家倍数+1
				m_cbBeiShu[m_wBankerUser]++;
					//刷新倍数
				//if (is_linfen())
				//{
					OnEventUserFlushScore(m_wBankerUser);
				//}

				//发送踢牌的消息
				if ((m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT != m_wFirstUser)
				{

					WORD tiPaiUser = (m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT;  //踢牌用户
					 //发送叫分
					MHLMsgDouDiZhuS2CGameMessage msg;
					msg.sub_cmd_code = SUB_S_LINFEN_TIPAI;
					msg.cmdData.cmd_s_notify_user_ti.m_pos = (int)tiPaiUser;
					notify_desk(msg);
					//notify_user(msg, tiPaiUser);

					this->set_play_status(GAME_SCENE_LINFEN_TIPAI);

					//更新用户
					m_wCurrentUser = tiPaiUser;
					m_wTiPaiUser = tiPaiUser;

				}
				else
				{
					OnEventBankerOutCard();
				}

			}
		}
		

		return true;
	}

	//用户出牌
	bool OnUserOutCardLaizi(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
	{
		LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
		//效验状态
		if (wChairID != m_wCurrentUser) return true;

		//获取类型
		BYTE cbCardType;
		if (m_GameLogicLaizi.IsHadRoguishCard(cbCardData, cbCardCount))     //  有癞子
			cbCardType = m_GameLogicLaizi.GetMagicCardType(cbCardData, cbCardCount);
		else                                                                                                               //无癞子
			cbCardType = m_GameLogicLaizi.GetCardType(cbCardData, cbCardCount, m_playtype.DouDiZhuIsTongHua3() == 0 ? false : true);

	
		//if ((cbCardType == CT_THREE && m_cbHandCardCount[wChairID] > 3) || (cbCardType == CT_THREE_LINE && m_cbHandCardCount[wChairID] > 9) || (cbCardType == 、C、//_HREE_LINE && m_cbHandCardCount[wChairID] > 12))
		//{
		//	LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d     cardtype [%d] error ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser, cbCardType);
		//	return false;
		//}
		

		//类型判断
		if (cbCardType == CT_ERROR)
		{
			LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d   ERROR  cardtype ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
			return false;
		}

		//出牌判断
		if (m_cbTurnCardCount != 0)
		{
			//对比扑克
			if (m_GameLogicLaizi.CompareCard(cbCardData, m_cbTurnCardData, cbCardCount, m_cbTurnCardCount,m_playtype.DouDiZhuIsTongHua3()==0?false:true) == false)
			{
				LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d   ERROR  CompareCard ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
				return false;
			}
		}

		//删除扑克
		if (m_GameLogicLaizi.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
		{
			ASSERT(FALSE);
			LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d     RemoveCardList ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser, cbCardType);
			return false;
		}

		//出牌变量
		m_cbOutCardCount[wChairID]++;

		//设置变量
		m_cbTurnCardCount = cbCardCount;
		m_cbHandCardCount[wChairID] -= cbCardCount;
		CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);

		//打印日志
		//OnEventPrintCards(wChairID, m_cbHandCardDataLaizi[wChairID], m_cbHandCardCount[wChairID], TEXT("出牌后手牌"));

		//炸弹判断
		if ((cbCardType == CT_BOMB_CARD) || (cbCardType == CT_MISSILE_CARD) || (cbCardType == CT_BOMB_SOFT))
		{
			OnEventRecordBombCount(wChairID);
			//m_cbBombCount++;
			//m_cbEachBombCount[wChairID]++;
		}

		//切换用户
		m_wTurnWiner = wChairID;
		if (m_cbHandCardCount[wChairID] != 0)
		{
			if (cbCardType != CT_MISSILE_CARD)
			{
				m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
			}
		}
		else m_wCurrentUser = DOUDIZHU_INVAILD_POS;

		//构造数据
		//CMD_S_OutCard OutCard;
		//OutCard.wOutCardUser = wChairID;
		//OutCard.cbCardCount = cbCardCount;
		//OutCard.wCurrentUser = m_wCurrentUser;
		//CopyMemory(OutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));

		//发送数据
		//WORD wHeadSize = sizeof(OutCard) - sizeof(OutCard.cbCardData);
		//WORD wSendSize = wHeadSize + OutCard.cbCardCount * sizeof(BYTE);

		//MHLMsgDouDiZhuS2CGameMessage msg;
		//msg.sub_cmd_code = SUB_S_LAIZI_OUT_CARD;
		//CopyMemory(&msg.cmdData.cmd_s2c_out_card, &OutCard, sizeof(OutCard));
		//notify_desk(msg);

		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_OutCard & cmdOutCard = msg.cmdData.cmd_s2c_out_card;
		msg.sub_cmd_code = SUB_S_OUT_CARD;
		cmdOutCard.wOutCardUser = wChairID;
		cmdOutCard.cbCardCount = cbCardCount;
		cmdOutCard.wCurrentUser = m_wCurrentUser;
		CopyMemory(cmdOutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));
		notify_desk(msg);

		//保存回访，用户操作
		SaveUserOper(1, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID],cbCardCount, cbCardData);

		//出牌最大
		if (cbCardType == CT_MISSILE_CARD) m_cbTurnCardCount = 0;

		//结束判断
		if (m_wCurrentUser == DOUDIZHU_INVAILD_POS)
		{
			finish_round(1, wChairID,1);
			//OnEventGameConcludeLaizi(wChairID, NULL, GER_NORMAL);
		    // todo 结束游戏
		}
		
		return true;
	}

	//用户出牌
	bool OnUserOutCardLinfen(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
	{
		LLOG_ERROR("OnUserOutCardLinfen desk:%d pos:%d curPos:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
		//效验状态
		ASSERT(wChairID == m_wCurrentUser);
		if (wChairID != m_wCurrentUser) return true;

		//获取类型
		BYTE cbCardType = m_GameLogicLinfen.GetCardType(cbCardData, cbCardCount);

		//类型判断
		if (cbCardType == CT_ERROR)
		{
			ASSERT(FALSE);
			return false;
		}

		//if (cbCardType==CT_FOUR_TAKE_TWO)return false;
		//if (cbCardType==CT_THREE_TAKE_TWO)return false;
		if ((cbCardType == CT_THREE && m_cbHandCardCount[wChairID]>3) || (cbCardType == CT_THREE_LINE && m_cbHandCardCount[wChairID]>9) || (cbCardType == CT_THREE_LINE && m_cbHandCardCount[wChairID]>12))return false;

		//出牌判断
		if (m_cbTurnCardCount != 0)
		{
			//对比扑克
			if (m_GameLogicLinfen.CompareCard(m_cbTurnCardData, cbCardData, m_cbTurnCardCount, cbCardCount) == false)
			{
				ASSERT(FALSE);
				return false;
			}
		}

		//删除扑克
		if (m_GameLogicLinfen.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//出牌变量
		m_cbOutCardCount[wChairID]++;

		//设置变量
		m_cbTurnCardCount = cbCardCount;
		m_cbHandCardCount[wChairID] -= cbCardCount;
		CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);

	

		//炸弹判断
		if ((cbCardType == CT_BOMB_CARD) || (cbCardType == CT_MISSILE_CARD))
		{
			if (m_cbBombCount<m_playtype.DouDiZhuBombMaxCount())
			{
				m_cbBombCount++;
				m_cbEachBombCount[wChairID]++;

				if (cbCardCount == 4 && m_GameLogicLinfen.GetCardLogicValue(cbCardData[0]) == 3)
				{
					for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
						m_cbBeiShu[i] *= 2;

					for (int tempi = 0; tempi < 2; tempi++)
					{
						if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
						{
							for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
								m_cbBeiShu[i] *= 2;
							m_cbBombCount++;
							m_cbEachBombCount[wChairID]++;
						}
					}


				}
				else
				{    //倍数
					for (BYTE i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
					{
						m_cbBeiShu[i] *= 2;
					}
				}

				OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);

				//打印日志
				//char szString[512] =("");
				//_snprintf(szString, CountArray(szString), ("炸弹，炸弹个数[%d]，用户倍数翻倍[0]=[%d],[1]-[%d],[2]=[%d]"), m_cbBombCount, m_cbBeiShu[0], m_cbBeiShu[1], m、//_BeiShu、[2]);
				//LLOG_DEBUG(szString);
			}
			else
			{
				//打印日志
				//char szString[512] = ("");
				//_snprintf(szString, CountArray(szString), ("炸弹封顶不再计分，炸弹个数[%d]"), m_cbBombCount);
				//LLOG_DEBUG(szString);
				LLOG_ERROR("desk[%d] bomb  max ,score no add .bombcount[%d],bombLimit[%d]", m_desk ? m_desk->GetDeskId() : 0, m_cbBombCount, m_playtype.DouDiZhuBombMaxCount());
			}

			m_cbBombCountReal++;
			m_cbEachBombCountReal[wChairID]++;

		}

		//切换用户
		m_wTurnWiner = wChairID;
		if (m_cbHandCardCount[wChairID] != 0)
		{
			if (cbCardType != CT_MISSILE_CARD)
			{
				m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
			}
		}
		else m_wCurrentUser = DOUDIZHU_INVAILD_POS;

		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_OutCard & cmdOutCard = msg.cmdData.cmd_s2c_out_card;
		msg.sub_cmd_code = SUB_S_OUT_CARD;
		cmdOutCard.wOutCardUser = wChairID;
		cmdOutCard.cbCardCount = cbCardCount;
		cmdOutCard.wCurrentUser = m_wCurrentUser;
		CopyMemory(cmdOutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));
		notify_desk(msg);
	
		//保存回访，用户操作
		SaveUserOper(1, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID], cbCardCount, cbCardData, m_cbBeiShu[wChairID]);

		//出牌最大
		if (cbCardType == CT_MISSILE_CARD) m_cbTurnCardCount = 0;

		//结束判断
		if (m_wCurrentUser == DOUDIZHU_INVAILD_POS)
		{
			finish_round(1, wChairID, 1);
			//OnEventGameConcludeLinfen(wChairID, NULL, GER_NORMAL);
			//todo 结束游戏
		}
		return true;
	}

	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
	{

		LLOG_ERROR("OnUserOutCard desk:%d pos:%d curPos:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
		
		//效验状态
		if (wChairID != m_wCurrentUser) return true;

		//for (int i = 0; i < cbCardCount; i++)
		//{
		//	LLOG_ERROR("card %d value: %d", i, cbCardData[i]);
		//}
		//打印用户出牌
		//OnEventPrintCards(wChairID, cbCardData, cbCardCount, TEXT("出牌"));

		//获取类型
		BYTE cbCardType = m_GameLogicClassical.GetCardType(cbCardData, cbCardCount);

		//类型判断
		if (cbCardType == CT_ERROR)
		{
			ASSERT(FALSE);
			LLOG_ERROR("m_GameLogicClassical.GetCardType Error");
			return false;
		}

		//出牌判断
		if (m_cbTurnCardCount != 0)
		{
			//对比扑克
			if (m_GameLogicClassical.CompareCard(m_cbTurnCardData, cbCardData, m_cbTurnCardCount, cbCardCount) == false)
			{
				return false;
			}
		}

		//删除扑克
		if (m_GameLogicClassical.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
		{
			return false;
		}

		//出牌变量
		m_cbOutCardCount[wChairID]++;

		//设置变量
		m_cbTurnCardCount = cbCardCount;
		m_cbHandCardCount[wChairID] -= cbCardCount;
		CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);

		//炸弹判断
		if ((cbCardType == CT_BOMB_CARD) || (cbCardType == CT_MISSILE_CARD))
		{
			OnEventRecordBombCount(wChairID);
			//m_cbBombCount++;
			//m_cbEachBombCount[wChairID]++;
		}

		//切换用户
		m_wTurnWiner = wChairID;
		if (m_cbHandCardCount[wChairID] != 0)
		{
			if (cbCardType != CT_MISSILE_CARD)
			{
				m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
			}
		}
		else m_wCurrentUser = DOUDIZHU_INVAILD_POS;

	
		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_OutCard & cmdOutCard = msg.cmdData.cmd_s2c_out_card;
		msg.sub_cmd_code = SUB_S_OUT_CARD;
		cmdOutCard.wOutCardUser = wChairID;
		cmdOutCard.cbCardCount = cbCardCount;
		cmdOutCard.wCurrentUser = m_wCurrentUser;
		CopyMemory(cmdOutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));
		notify_desk(msg);

		//保存回访，用户操作
		SaveUserOper(1, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID], cbCardCount, cbCardData);

		//出牌最大
		if (cbCardType == CT_MISSILE_CARD) m_cbTurnCardCount = 0;

		//结束判断
		if (m_wCurrentUser == DOUDIZHU_INVAILD_POS)
		{
			finish_round(1, wChairID,1);
			//this->OnGameOver(1, wChairID, DOUDIZHU_INVAILD_POS);
		}
	

		return true;
	}

	//用户踢牌
	bool OnUserTiPai(WORD wChairID, BYTE cbIsYes)
	{
		LLOG_ERROR("OnUserTiPai desk:%d pos:%d cbIsYes:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbIsYes);
		//效验状态
		if (wChairID != m_wCurrentUser) return true;

		//广播用户TI结果
		OnEventBroadcaseTiResult(SUB_S_LINFEN_TIPAI, wChairID, cbIsYes);

		//用户踢牌
		if (cbIsYes)
		{
			m_cbBeiShu[wChairID]++;
			m_cbBeiShu[m_wBankerUser]++;

			m_cbIsTi[wChairID] = 1;   //用户踢牌

			//发送刷新倍数
			if (is_linfen()|| m_playtype.DouDiZhuIsTi()!=0)
			{
				OnEventUserFlushScore(m_wBankerUser);
				OnEventUserFlushScore(wChairID);
			}

			//找到跟踢用户
			WORD nextUser = (wChairID + 1) % DOUDIZHU_PLAY_USER_COUNT;
			if (nextUser == m_wBankerUser)
				nextUser = (wChairID + 2) % DOUDIZHU_PLAY_USER_COUNT;
			//是否跟踢
			//m_pITableFrame->SendTableData(nextUser, SUB_S_LINFEN_GENTI, NULL, 0);
			MHLMsgDouDiZhuS2CGameMessage msg;
			msg.sub_cmd_code = SUB_S_LINFEN_GENTI;
			msg.cmdData.cmd_s_notify_user_ti.m_pos = nextUser;
			notify_desk(msg);
			//notify_user(msg, nextUser);

			//设置状态
			//m_pITableFrame->SetGameStatus(GAME_SCENE_LINFEN_GENTI);
			set_play_status(GAME_SCENE_LINFEN_GENTI);

			//更新用户
			m_wCurrentUser = nextUser;
			m_wGenTiUser = nextUser;

			//打印日志
			//char szString[512]=("");
			//_snprintf(szString, CountArray(szString), ("给跟踢用户chairid[%d]发送消息。地主[%d]倍数[%d],踢牌用户[%d]倍数[%d]"), nextUser, m_wBankerUser, m_cbBeiShu//[m_wBankerUser],wChairID, m_cbBeiShu[wChairID]);
			//LLOG_DEBUG(szString);
		}
		else // 用户不踢
		{
			//更新用户踢牌
			m_cbIsTi[wChairID] = 0;   //用户不踢

			WORD  nextUser = (wChairID + 1) % DOUDIZHU_PLAY_USER_COUNT;
			if (nextUser == m_wBankerUser || nextUser == m_wFirstUser)
			{
				OnEventBankerOutCard();
			}
			else
			{     //下个用户是否踢牌
				  //发送消息
				//m_pITableFrame->SendTableData(nextUser, SUB_S_LINFEN_TIPAI, NULL, 0);
				//m_pITableFrame->SendLookonData(nextUser, SUB_S_LINFEN_TIPAI, NULL, 0);

				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_TIPAI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = (Lint)nextUser;
				notify_desk(msg);
				//notify_user(msg, nextUser);

				//更新用户
				m_wCurrentUser = nextUser;
				m_wTiPaiUser = nextUser;
			}

		}

		return true;
	}

	//用户跟踢
	bool OnUserGenTi(WORD wChairID, BYTE cbIsYes)
	{
		LLOG_ERROR("OnUserGenTi desk:%d pos:%d cbIsYes:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbIsYes);
		//效验状态
		if (wChairID != m_wCurrentUser) return true;

		//效验参数

		//广播用户TI结果
		OnEventBroadcaseTiResult(SUB_S_LINFEN_GENTI, wChairID, cbIsYes);

		//用户跟踢
		if (cbIsYes)
		{
			m_cbBeiShu[wChairID]++;
			m_cbBeiShu[m_wBankerUser]++;

			m_cbIsTi[wChairID] = 2;   //用户跟踢

			 //发送刷新倍数
			if (is_linfen()|| m_playtype.DouDiZhuIsTi() != 0)
			{
				OnEventUserFlushScore(wChairID);
				OnEventUserFlushScore(m_wBankerUser);
			}
		}
		else
			m_cbIsTi[wChairID] = 0;   //用户不跟踢


		MHLMsgDouDiZhuS2CGameMessage msg;
		msg.sub_cmd_code = SUB_S_LINFEN_HUIPAI;
		msg.cmdData.cmd_s_notify_user_ti.m_pos = m_wBankerUser;
		notify_desk(msg);
		//notify_user(msg, m_wBankerUser);

		//更新用户
		m_wCurrentUser = m_wBankerUser;

		//设置状态
		//m_pITableFrame->SetGameStatus(GAME_SCENE_LINFEN_HUITI);
		this->set_play_status(GAME_SCENE_LINFEN_HUITI);

		//打印日志
		//TCHAR szString_end[512]=TEXT("");
		//_sntprintf(szString_end, CountArray(szString_end), TEXT("地主[%d]倍数[%d],跟踢用户[%d]倍数[%d]"), m_wBankerUser, m_cbBeiShu[m_wBankerUser], wChairID, m_cbBeiShu[wChairID]);
		//CTraceService::TraceString(szString_end, TraceLevel_Normal);

		return true;
	}

	//用户回牌
	bool OnUserHuiPai(WORD wChairID, BYTE cbIsYes)
	{
		LLOG_ERROR("OnUserHuiPai desk:%d pos:%d cbIsYes:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbIsYes);
		//打印日志
		//TCHAR szString[512]=TEXT("");
		//_sntprintf(szString, CountArray(szString), TEXT("用户chairId=[%d], 是否回牌[%d]"), wChairID, cbIsYes);
		//CTraceService::TraceString(szString, TraceLevel_Normal);

		//效验状态
		if (wChairID != m_wCurrentUser) return true;

		if (wChairID != m_wBankerUser) return true;

		//效验参数

		//广播用户TI结果
		OnEventBroadcaseTiResult(SUB_S_LINFEN_HUIPAI, wChairID, cbIsYes);

		//用户回牌
		if (cbIsYes)
		{
			m_cbIsTi[wChairID] = 3;   //用户回牌

									  //发送刷新倍数
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (i == m_wBankerUser)continue;
				if (m_cbIsTi[i]>0)
				{
					m_cbBeiShu[i] += 2;
					m_cbBeiShu[m_wBankerUser] += 2;
					//OnEventUserFlushScore(i);
				}
			}
			if(is_linfen()|| m_playtype.DouDiZhuIsTi() != 0)
				OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);

		}
		else
			m_cbIsTi[wChairID] = 0;   //用户不回牌

		OnEventBankerOutCard();

		return true;
	}

	//用户放弃
	bool OnUserPassCard(WORD wChairID)
	{
		LLOG_ERROR("OnUserPassCard desk:%d pos:%d ", m_desk ? m_desk->GetDeskId() : 0, wChairID);
		//效验状态
		if ((wChairID != m_wCurrentUser) || (m_cbTurnCardCount == 0)) return true;

		//设置变量
		m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
		if (m_wCurrentUser == m_wTurnWiner) m_cbTurnCardCount = 0;

		
		//通知桌子
		MHLMsgDouDiZhuS2CGameMessage msg;
		msg.cmdData.cmd_s2c_pass_card.wPassCardUser = wChairID;
		msg.cmdData.cmd_s2c_pass_card.wCurrentUser = m_wCurrentUser;
		msg.cmdData.cmd_s2c_pass_card.cbTurnOver = (m_cbTurnCardCount == 0) ? TRUE : FALSE;
		msg.sub_cmd_code = SUB_S_PASS_CARD;
		
		notify_desk(msg);

		//保存回访，用户操作
		SaveUserOper(0, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID]);

		return true;
	}

	//游戏消息
	bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, User * pUser)
	{
		switch (wSubCmdID)
		{
		case SUB_C_CALL_SCORE:	//用户叫分
		{
			if (m_play_status != DOUDIZHU_GAME_SCENE_CALL)
			{
				LLOG_ERROR("用户叫分m_play_status_error ");
				return true;
			}

			//变量定义
			CMD_C_CallScore * pCallScore = (CMD_C_CallScore *)pData;

			//消息处理
			WORD wChairID = GetUserPos(pUser);
			return OnUserCallScore(wChairID, pCallScore->cbCallScore);
		}
		case SUB_C_OUT_CARD:	//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard = (CMD_C_OutCard *)pData;
			
			if (m_play_status != DOUDIZHU_GAME_SCENE_PLAY)
			{
				LLOG_ERROR("m_play_status error");
			}

			//消息处理
			WORD wChairID = GetUserPos(pUser);
			
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				return OnUserOutCard(wChairID, pOutCard->cbCardData, pOutCard->cbCardCount);
			}
			case PLAY_TYPE_LAIZI:
			{
				return OnUserOutCardLaizi(wChairID, pOutCard->cbCardData, pOutCard->cbCardCount);

			}
			case PLAY_TYPE_LINFEN:
			{
				return OnUserOutCardLinfen(wChairID, pOutCard->cbCardData, pOutCard->cbCardCount);

			}
			}

			
		}
		case SUB_C_PASS_CARD:	//用户放弃
		{
			//状态效验
			if (m_play_status != DOUDIZHU_GAME_SCENE_PLAY) return true;

			//消息处理
			return OnUserPassCard(GetUserPos(pUser));
		}
		case SUB_C_TIPAI:
		{
			if (m_play_status != GAME_SCENE_LINFEN_TIPAI)
			{
				LLOG_ERROR("m_play_status != GAME_SCENE_LINFEN_TIPAI ERROR");
				return true;
			}

			//变量定义
			CMD_C_Ti * pTiPai = (CMD_C_Ti *)pData;

			//消息处理
			WORD wChairID = GetUserPos(pUser);
			LLOG_DEBUG("OnGameMessage....SUB_C_TIPAI[%d]", pTiPai->cbIsYes);
			return OnUserTiPai(wChairID, pTiPai->cbIsYes);

		}
		case SUB_C_GENTI:
		{
		
			if (m_play_status != GAME_SCENE_LINFEN_GENTI) return true;

			//变量定义
			CMD_C_Ti * pGenTi = (CMD_C_Ti *)pData;

			//消息处理
			WORD wChairID = GetUserPos(pUser);
			return OnUserGenTi(wChairID, pGenTi->cbIsYes);

		}
		case SUB_C_HUIPAI:
		{
		
			if (m_play_status != GAME_SCENE_LINFEN_HUITI) return true;

			//变量定义
			CMD_C_Ti * pHuiPai = (CMD_C_Ti *)pData;

			//消息处理
			WORD wChairID = GetUserPos(pUser);
			return OnUserHuiPai(wChairID, pHuiPai->cbIsYes);

		}
		}

		return false;
	}

	//一局游戏开始路口
	void start_game()
	{
		//LLOG_DEBUG("start_game  ....[%d]", m_play_status);
		//
		m_zhuangPos = m_desk->m_zhuangPos;

	
		OnEventGameStart();
		return;
	
			
	}

	//开始一局
	void start_round(Lint * PlayerStatus) {
		//LLOG_DEBUG("Logwyz  ...    START  GAME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		if (NULL == PlayerStatus)  return;

		Lint t_user_status[DOUDIZHU_PLAY_USER_COUNT];
		memcpy(t_user_status, PlayerStatus, sizeof(Lint) * DOUDIZHU_PLAY_USER_COUNT);

		clear_round();
		m_video.DrawClear();   //清空上一局回访记录
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);

		memcpy(m_user_status, t_user_status, sizeof(m_user_status));

		//通知manager局数变化
		if (m_desk && m_desk->m_vip)
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		LLOG_ERROR("****Desk %d start round %d/%d   player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);
		//LLOG_DEBUG("GameMode=[%d],TuiTongZiGetScoreTime=[%d],TuiTongZiGetAuto=[%d],TuiTongZiGetCuoPai=[%d]", m_playtype., m_playtype.TuiTongZiGetScoreTime(), m_playtype.TuiTongZiGetAuto(), m_playtype.TuiTongZiGetCuoPai());



		//游戏开始入口
		start_game();
		LLOG_ERROR("****Desk %d start round %d/%d, player_count=%d", m_desk->GetDeskId(), m_round_offset, m_round_limit, m_player_count);
	}

	//结束一局
	void  finish_round(Lint result, Lint winpos, Lint reason)
	{
		LLOG_DEBUG("Doudizhu - finish_round desk:%d", m_desk ? m_desk->GetDeskId() : 0);

		increase_round();

		MHLMsgDouDiZhuS2CGameMessage send;
		send.sub_cmd_code = SUB_S_GAME_CONCLUDE;

		CMD_S_GameConclude &over = send.cmdData.cmd_s2c_game_conclude;
		over.cbIsFinalDraw = m_dismissed || m_round_offset >= m_round_limit;
		over.cbBankerUser = m_wBankerUser;
		over.lCellScore = m_playtype.DouDiZhuCellScore();
		over.cbBombCount = m_cbBombCountReal;
		if(is_linfen())
			over.cbBombCount = m_cbBombCount;
		over.cbBankerCall = m_cbBankerScore;
		CopyMemory(over.cbEachBombCount, m_cbEachBombCountReal, sizeof(m_cbEachBombCountReal));

		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			over.cbActSelect[i] = (m_cbIsTi[i] == 0xff) ? 0 : m_cbIsTi[i];
		}

		for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
		{
			over.cbRestCardData[i] = m_cbBankerCard[i];
		}


		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			over.cbCardCount[i] = m_cbHandCardCount[i];
			for (int j = 0; j < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; j++)
			{
				over.cbHandCardData[i][j] = m_cbHandCardData[i][j];
			}
		}

		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			if (m_desk->m_user[i])
			{
				send.m_usernikes.push_back(m_desk->m_user[i]->GetUserData().m_nike);
				send.m_userids.push_back(m_desk->m_user[i]->GetUserDataId());
				send.m_headUrl.push_back(m_desk->m_user[i]->GetUserData().m_headImageUrl);
			}
		}


		// 存储分数
		//Lint scores[DOUDIZHU_PLAY_USER_COUNT] = { 0 };
		if (result == 1) // 正常结束
		{		
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				over.cbBankerScore[i] = m_cbBankerScore;
			}
			
			if (is_linfen())
			{
				//春天判断
				if (winpos == (int)m_wBankerUser)
				{
					//用户定义
					WORD wUser1 = (m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
					WORD wUser2 = (m_wBankerUser + 2) % DOUDIZHU_PLAY_USER_COUNT;

					//用户判断
					if ((m_cbOutCardCount[wUser1] == 0) && (m_cbOutCardCount[wUser2] == 0))
					{
						over.bChunTian = TRUE;
						if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
						{
							for (int k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
							{
								m_cbBeiShu[k] *= 2;
							}
							//临汾刷新倍数
							if (is_linfen())
							{
								OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
							}
						}
					}
				}
				else if (m_cbOutCardCount[m_wBankerUser] == 1)
				{
					over.bFanChunTian = TRUE;
					if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
					{
						for (BYTE k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
							m_cbBeiShu[k] *= 2;
						//临汾刷新倍数
						if (is_linfen())
						{
							OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
						}
					}
				}
				for (WORD i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
				{
					if ((winpos != m_wBankerUser && i != m_wBankerUser) || (winpos == m_wBankerUser && i == m_wBankerUser))
					{
						over.lGameScore[i] = m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
					}
					else if ((winpos != m_wBankerUser && i == m_wBankerUser) || (winpos == m_wBankerUser && i != m_wBankerUser))
					{
						over.lGameScore[i] = (-1)*m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
					}
				}
			}
			else
			{
				Lint lScoreTimes = 1 * m_cbBankerScore;

				//if ((is_laizi() || is_normal()) && m_playtype.  DouDiZhuIsTi() == 1)
				//	lScoreTimes = 1 * 1;

				for (BYTE i = 0; i < m_cbBombCount; i++)
				{
					lScoreTimes *= 2;
				}

				//春天判断
				if (winpos == (int)m_wBankerUser)
				{
					//用户定义
					WORD wUser1 = (m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
					WORD wUser2 = (m_wBankerUser + 2) % DOUDIZHU_PLAY_USER_COUNT;

					//用户判断
					if ((m_cbOutCardCount[wUser1] == 0) && (m_cbOutCardCount[wUser2] == 0))
					{
						lScoreTimes *= 2;
						over.bChunTian = TRUE;
						for (int k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
						{
							m_cbBeiShu[k] *= 2;
						}
						//临汾刷新倍数
						if (m_playtype.DouDiZhuIsTi()!=0)
						{
							OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
						}
					}
				}
				else if (m_cbOutCardCount[m_wBankerUser] == 1)
				{
					lScoreTimes *= 2;
					over.bFanChunTian = TRUE;
					for (BYTE k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
						m_cbBeiShu[k] *= 2;
					//临汾刷新倍数
					if (m_playtype.DouDiZhuIsTi() != 0)
					{
						OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
					}
				}

				//统计积分
				if (m_playtype.DouDiZhuIsTi() == 0)
				{
					for (WORD i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
					{
						//变量定义
						Lint lUserScore = 0L;
						Lint lCellScore = m_playtype.DouDiZhuCellScore();
						//积分基数
						if (i == m_wBankerUser)
						{
							lUserScore = (0 == m_cbHandCardCount[m_wBankerUser]) ? 2 : -2;
							//lUserScore = (0 == m_cbHandCardCount[m_wBankerUser]) ? m_cbBeiShu[m_wBankerUser]: -1* m_cbBeiShu[m_wBankerUser];
						}
						else
						{
							if (m_cbHandCardCount[m_wBankerUser] == 0)
							{
								lUserScore = -1;
							}
							else
							{
								lUserScore = 1;
							}
						}

						//计算积分
						over.lGameScore[i] = lUserScore * lCellScore * lScoreTimes;

						//m_accum_score[i] += scores[i];
						//over.lGameScore[i] = scores[i];		 
					}
				}
				else {
					for (WORD i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
					{
						if ((winpos != m_wBankerUser && i != m_wBankerUser) || (winpos == m_wBankerUser && i == m_wBankerUser))
						{
							over.lGameScore[i] = m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
						}
						else if ((winpos != m_wBankerUser && i == m_wBankerUser) || (winpos == m_wBankerUser && i != m_wBankerUser))
						{
							over.lGameScore[i] = (-1)*m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
						}
					}
				}
			}
			//切换用户
			m_wFirstUser = (WORD)winpos;
		
			add_round_log(over.lGameScore, m_wBankerUser);

			for (Lint i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_desk && m_desk->m_vip)
				{
					send.m_totalScore.push_back(m_desk->m_vip->m_score[i]);
					//LLOG_DEBUG("over.score[%d]=[%d]", i, send.m_totalScore[i]);
				}
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null");
			}

		}
		else if (result == 2)
		{
			add_round_log(over.lGameScore, m_wBankerUser);

			for (Lint i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_desk && m_desk->m_vip)
				{
					send.m_totalScore.push_back(m_desk->m_vip->m_score[i]);
					//LLOG_DEBUG("over.score[%d]=[%d]", i, send.m_totalScore[i]);
				}
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null");
			}
		}

		notify_desk(send);

		if (m_playtype.DouDiZhuCallZhuangMode() == 0)
			m_firstCallZhuang = winpos;
		else
			m_firstCallZhuang = GetNextPos(m_firstCallZhuang);

		set_desk_state(DESK_WAIT);
		if (m_desk) m_desk->HanderGameOver(1);		
	}
};


struct DouDiZhuGameHandler : DouDiZhuGameCore{

	void notify_user(LMsg &msg, int pos) override {
		if (NULL == m_desk) return;
		if (pos < 0 || pos >= m_player_count) return;
		User *u = m_desk->m_user[pos];
		if (NULL == u) return;
		u->Send(msg);
	}

	void notify_desk(LMsg &msg) override {
		if (NULL == m_desk) return;
		m_desk->BoadCast(msg);
	}

	void notify_desk_playing_user(LMsg &msg) {
		if (NULL == m_desk) return;
		m_desk->BoadCast2PlayingUser(msg);
	}

	void notify_desk_without_user(LMsg &msg, User* pUser)
	{
		if (NULL == m_desk) return;
		m_desk->BoadCastWithOutUser(msg, pUser);
	}

	bool startup(Desk *desk) {
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}

	void shutdown(void) {
		clear_match(0);
	}

	void SetPlayType(std::vector<Lint> &l_playtype) {
		// 被 Desk 的 SetVip 调用，此时能得到 m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		//保存回访基本信息
		if(m_desk)
			m_video.FillDeskBaseInfo(m_desk->GetDeskId(), m_desk->m_state, round_limit, l_playtype);

	}

	void MHSetDeskPlay(Lint PlayUserCount, Lint * PlayerStatus, Lint PlayerCount) {
		LLOG_DEBUG("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		if (!m_desk || !m_desk->m_vip) {
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		if (PlayUserCount != DOUDIZHU_PLAY_USER_COUNT || PlayerStatus == NULL || PlayerCount != DOUDIZHU_PLAY_USER_COUNT)
		{
			LLOG_ERROR("MHSetDeskPlay  error  !!!!PlayUserCount=[%d] ", PlayUserCount);
			return;
		}

		m_player_count = PlayUserCount;
		//memcpy(m_user_playing_status, PlayerStatus, sizeof(m_user_playing_status));
		//
		//for(int i=0; i<YINGSANZHANG_PLAY_USER_MAX;i++)
		//	LLOG_DEBUG("m_user_playing_status[%d]=[%d]",i, m_user_playing_status[i]);


		start_round(PlayerStatus);
	}

	//游戏结束
	void OnGameOver(Lint result, Lint winpos, Lint bombpos) {
		if (m_desk == NULL || m_desk->m_vip == NULL) {
			LLOG_ERROR("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;	
		finish_round(2, winpos, bombpos);
		LLOG_ERROR("***desk game over. desk:%d round_limit: %d round: %d dismiss: %d", m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);
	}

	//游戏中断线重连
	void OnUserReconnect(User *pUser) {
		if (pUser == NULL || m_desk == NULL) 
		{
			return;
		}

		// 发送当前圈数信息
		// 为什么要发给桌子，而不是只发给这个短线重连的玩家？
		notify_desk_match_state();

		Lint pos = GetUserPos(pUser);
		MHLOG_PLAYCARD("*******断线重连desk:%d userid:%d pos:%d m_wCurrentUser =%d ", m_desk->GetDeskId(), pUser->GetUserDataId(), pos, m_wCurrentUser);
		MHLMsgDouDiZhuS2CReconn reconn;
	
		reconn.kPlayStatus = (m_play_status > 20 ? 2 : m_play_status);
		switch(m_play_status)
		{
		case DOUDIZHU_GAME_SCENE_PLAY:
		{
			//reconn.kCellScore = m_desk->m_cellscore;
			//reconn.kCurrentUser = m_wCurrentUser;
			//reconn.kBankerUser = m_wBankerUser;
			//reconn.kBankerScore = m_cbBankerScore;
			//
			//for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			//{
			//	reconn.kScoreInfo[i] = m_cbScoreInfo[i];
			//	reconn.kHandCardCount[i] = m_cbHandCardCount[i];
			//}
			//for (int i = 0; i < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; i++)
			//{
			//	reconn.kHandCardData[i] = m_cbHandCardData[pos][i];
			//}

			reconn.kBombCount = m_cbBombCount;
			reconn.kTurnWiner = m_wTurnWiner;
			reconn.kTurnCardCount = m_cbTurnCardCount;
			for (int i = 0; i < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; i++)
			{
				reconn.kTurnCardData[i] = m_cbTurnCardData[i];
			}

			if (m_playtype.DouDiZhuDiPaiType())
			{
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
				{
					reconn.kBankerCard[i] = m_cbBankerCard[i];
				}
			}
			
		}
		case GAME_SCENE_LINFEN_TIPAI:
		case GAME_SCENE_LINFEN_GENTI:
		case GAME_SCENE_LINFEN_HUITI:
		{
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				switch (m_cbIsTi[i])
				{
				case 0:
					reconn.ktype[i] = 131;
					break;
				case 1:
					reconn.ktype[i] = 131;
					reconn.kActSelect[i] = 1;
					break;
				case 2:
					reconn.ktype[i] = 132;
					reconn.kActSelect[i] = 1;
					break;
				case 3:
					reconn.ktype[i] = 133;
					reconn.kActSelect[i] = 1;
					break;

				}
			}

			if (m_playtype.DouDiZhuDiPaiType() != 1 && pos != m_wBankerUser)
			{
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
				{
					reconn.kBankerCard[i] = CARD_BACK_PATTERN;
				}
			}
			else
			{
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
				{
					reconn.kBankerCard[i] = m_cbBankerCard[i];
				}
			}
			
		}
		case DOUDIZHU_GAME_SCENE_CALL:
		{
			reconn.kCellScore = m_playtype.DouDiZhuCellScore();
			reconn.kCurrentUser = m_wCurrentUser;
			reconn.kBankerUser = m_wBankerUser;
			reconn.kBankerScore = m_cbBankerScore;

			if (is_linfen() && m_onlyCallUser == m_wCurrentUser)
				reconn.kOnlyCall = 1;
			if (is_normal() && m_onlyCallUser == m_wCurrentUser)
				reconn.kOnlyCall = 1;

			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				reconn.kScoreInfo[i] = m_cbScoreInfo[i];
				reconn.kHandCardCount[i] = m_cbHandCardCount[i];
			}
			for (int i = 0; i < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; i++)
			{
				reconn.kHandCardData[i] = m_cbHandCardData[pos][i];
			}

			break;
		}
	
		default:break;
		}
		notify_user(reconn, pos);	

		if(is_linfen()|| m_playtype.DouDiZhuIsTi()!=0)
			OnEventUserFlushScore(pos);

		switch (m_play_status)
		{
		case GAME_SCENE_LINFEN_TIPAI:
		{
			if (pos == m_wTiPaiUser)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_TIPAI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = pos;
				notify_desk(msg);
				//notify_user(msg, pos);
			}
			break;
		}
		case GAME_SCENE_LINFEN_GENTI:
		{
			if (pos == m_wGenTiUser)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_GENTI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = pos;
				notify_desk(msg);
				//notify_user(msg, pos);
			}
			break;
		}
		case GAME_SCENE_LINFEN_HUITI:
		{
			if (pos == m_wBankerUser)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_HUIPAI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = m_wBankerUser;
				notify_desk(msg);
				//notify_user(msg, pos);
			}
			break;
		}

		}
	}

	// 客户端消息
	bool HandleDouDiZhuGameMessage(User * pUser, MHLMsgDouDiZhuC2SGameMessage * msg)
	{
		if (!pUser) return false;
		Lint pos =  GetUserPos(pUser);
		if (!is_pos_valid(pos))
		{
			LLOG_ERROR("HandleDouDiZhuGameMessage- desk:%d invalid pos:%d", m_desk->GetDeskId(), pos);
			return false;
		}

		LLOG_DEBUG("***斗地主客户端消息 deskId: %d userId: %d msg->sub_cmd_code: %d", m_desk->GetDeskId(), pUser->GetUserDataId(), msg->sub_cmd_code);

		//根据游戏类型提取出每个子命令的数据大小，发送给下层处理
		return OnGameMessage(msg->sub_cmd_code, &msg->cmdData, sizeof(msg->cmdData), pUser);
		
	}



	void Tick(LTime& curr)
	{
		//if (m_playtype.TuiTongZiGetAuto() == 0)return;
		//LLOG_DEBUG("TuiTongZiGameHandler--Tick,  play_status=[%d]", m_play_status);		
	}

};


DECLARE_GAME_HANDLER_CREATOR(101, DouDiZhuGameHandler);

