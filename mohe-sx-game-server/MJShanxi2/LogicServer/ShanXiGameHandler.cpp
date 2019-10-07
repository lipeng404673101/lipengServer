


#include "Desk.h"
#include "CardSx.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "ShanXiFeatureToggles.h"
#include "LMsgL2C.h"

#define USE_NEW_DEALING 01
#define USE_NEW_TING    01 // 使用新的听牌逻辑


// 11-19万 21-29筒 31-39条 41-47东南西北中发白 总共34种牌
struct HongTongWangPaiTest : public LSingleton<HongTongWangPaiTest>
{
	tile_t tile_cards[13] = { 11, 11, 11, 12, 12, 12, 13, 13, 13, 14,14,14,15 }; // 
	tile_t tile_4men[13] = { 11,11,11, 21,21,21, 31,31,31, 41,41,41, 12 }; //四门牌不能听
	tile_t tile_3men[13] = { 11,11,11, 21,21,21, 31,31,31, 12,12,12, 13 }; //三门牌不可听
	tile_t tile_2men[13] = { 11,11,11, 21,21,21, 12,12,12, 41,41,41, 13 }; //2门牌可听，可胡
	tile_t tile_feng[13] = { 11,11,11, 12,12,12, 13,13,13, 41,42,43, 14 }; // 风成副
	tile_t tile_sanyan[13] = { 11,11,11, 12,12,12, 13,13,13, 45,46,47, 14 }; //字成副
	tile_t tile_3men7xiaodui[13] = { 11,11,12, 12,21,21, 31,31,41, 41,45,45, 14 }; //三门七小对
	tile_t tile_2men7xiaodui[13] = { 11,11,12, 12,21,21, 22,22,41, 41,45,45, 14 }; //2门七小队
	tile_t tile_yingsanzui[13] = { 12,13,14, 21,22,23, 41,41,42, 42,43,44, 19 }; // 
	virtual bool Init() { return true; }
	virtual bool Final() { return true; }
};

#define HTTestClass HongTongWangPaiTest::Instance()

std::vector<std::string> splitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored)
{
	std::vector<std::string> resultStringVector;
	std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}/*pred*/, delimStr.at(0));//将出现的所有分隔符都替换成为一个相同的字符（分隔符字符串的第一个）  
	size_t pos = srcStr.find(delimStr.at(0));
	std::string addedString = "";
	while (pos != std::string::npos) {
		addedString = srcStr.substr(0, pos);
		if (!addedString.empty() || !repeatedCharIgnored) {
			resultStringVector.push_back(addedString);
		}
		srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
		pos = srcStr.find(delimStr.at(0));
	}
	addedString = srcStr;
	if (!addedString.empty() || !repeatedCharIgnored) {
		resultStringVector.push_back(addedString);
	}
	return resultStringVector;
}

int LoadPlayerCards(std::vector<int> & cards, int playerIndex)
{
	std::string fileName("card.ini");
	LIniConfig config;
	config.LoadFile("card.ini");

	char szKey[32] = { 0 };
	if (playerIndex > 0 && playerIndex <= 4)
		sprintf(szKey, "player%dcards", playerIndex);
	else if( playerIndex == 5)
		strcpy(szKey, "rest");
	else if (playerIndex == 6)
	{
		strcpy(szKey, "haozi");
	}

	std::string cardsString = config.GetString(Lstring(szKey), "");
	std::vector<std::string> cardsStringVec = splitString(cardsString, ",", true);
	if (cardsStringVec.size() > 0)
	{
		cards.clear();
		for (auto it = cardsStringVec.begin(); it < cardsStringVec.end(); it++)
		{
			int nValue = atoi((*it).c_str());
			MHLOG("Card: %d  %s", nValue, (*it).c_str());
			cards.push_back(nValue);
		}
	}


	return 0;
}

enum {
	ShanXi_JiaDe1000FenZhiZaiTouXiangChuXianShi = 0,
};



struct LMsgS2CUserSpeakTest : LMsgSC {
	Lint		m_userId;//说话人id
	Lint		m_pos;//说话人的位置
	Lint		m_type;//说话类型
	Lint		m_id;//类型id
	Lstring		m_musicUrl;//语音url链接
	Lstring		m_msg;//聊天文字


	LMsgS2CUserSpeakTest(): LMsgSC(MSG_S_2_C_USER_SPEAK) {
		m_userId = 0;
		m_pos = INVAILD_POS;
		m_type = -1;
		m_id = 0;
	}


	MSGPACK_DEFINE_MAP(m_msgId, m_userId, m_pos, m_type, m_id, m_musicUrl, m_msg);

	virtual bool Write(msgpack::packer<msgpack::sbuffer> &pack) {
		pack.pack(*this);
		return true;
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

};

/*操作结构体*/
struct PosOp 
{
	// 玩家摸牌后可做的操作为主动操作，别人出牌后自己可做的操作为被动操作
	bool active;
	// 0123 哪个位置的玩家做的操作
	int pos;
	// THINK_OPERATOR_ 枚举，不能改里面的值，和客户端共用的(1、出牌，2、胡...)
	int code;
	// 最多带一张牌做参数
	tile_t tile;
	/*************************************************
	* 函数名：  PosOp()
	* 描述：    初始化函数
	* 参数：
	* /active   是否为主动操作，true:主动，false:被动
	* /pos      操作的玩家的位置
	* /code     操作类型THINK_OPERATOR_ 枚举，1:出牌，2:胡...
	* /tile     操作的牌的牌值
	*************************************************/
	PosOp(bool active, int pos, int code, tile_t tile): active(active), pos(pos), code(code), tile(tile) {}
};

/*玩家主动操作结构体，继承操作结构体*/
struct ActivePosOp: PosOp {
	ActivePosOp(int pos, int code, tile_t tile): PosOp(true, pos, code, tile) {}
};

/*玩家被动操作结构体，继承操作结构体*/
struct PassivePosOp: PosOp {
	PassivePosOp(int pos, int code, tile_t tile): PosOp(false, pos, code, tile) {}
};


// 不会自动初始化的字段放这里，每盘自动重置
struct ShanXiRoundState__c_part {

	Lint			 m_angang[DESK_USER_COUNT];//暗杠数量
	Lint			 m_minggang[DESK_USER_COUNT];//明杠数量
	Lint			 m_diangang[DESK_USER_COUNT];//点杠数量
	Lint			 m_adiangang[DESK_USER_COUNT];//被点杠数量
	Lint			 m_louHuCard[DESK_USER_COUNT];//是否有漏胡记录
	bool			 m_louZimohu[DESK_USER_COUNT];//自摸胡点过就被记录下来，用于过胡只可自摸
	Lint			 m_kouCount[DESK_USER_COUNT];
	Lint			 m_knowCardCount[40];
	Lint             m_getChardFromMAGang[DESK_USER_COUNT];  //摸牌是否因为明杠还是暗杠，0：不是杠后摸牌，1：暗杠摸牌，2：明杠摸牌(默认为0)---------------- true:暗杠，false:明杠 初始为：true 撵中子中使用，（**Ren 2017-11-30）
	Card             m_hongTongWangPai_ShuangWang[DESK_USER_COUNT];  //洪洞王牌双王，每个人自己选的王牌（**Ren 2017-12-12）

	Card*			 m_curOutCard;//当前出出来的牌
	Card*			 m_curGetCard;//当前获取的牌
	GangThink		 m_GangThink;	//当前是否在思考杠
	Lint			 m_beforePos;					//之前操作的位置
	Lint			 m_beforeType;					//之前操作的类型
	Lint			 m_curPos;						//当前操作玩家
	bool			 m_needGetCard;
	DESK_PLAY_STATE  m_play_state; // 和 m_desk->setDeskPlayState 完全同步，初始值为-1

	Llong            m_tingTimeBegin[DESK_USER_COUNT];
	Llong            m_tingTimeEnd[DESK_USER_COUNT];
	Llong            m_getcardTimeBegin[DESK_USER_COUNT];
	Llong            m_getcardTimeEnd[DESK_USER_COUNT];

	void clear_round() {
		memset(this, 0, sizeof(*this));
		// 上面那句清零不满足要求的话，下面写自定义重置
		m_GangThink = GangThink_over;
		m_beforePos = INVAILD_POS;
		for (Lint i = 0; i< DESK_USER_COUNT; i++) {
			m_kouCount[i] = -1;
			//撵中子：明杠摸牌标记默认为true(**Ren 2017-12-01)
			//m_getChardFromMAGang[i] = true;
		}
		m_play_state = (DESK_PLAY_STATE)-1;		 
	}
};

// 有构造函数的字段放这里，每盘自动重置
struct ShanXiRoundState__cxx_part {
	PlayerState     m_ps[DESK_USER_COUNT]; // 用于记录碰、杠历史

	CardVector      m_handCard[DESK_USER_COUNT]; // 玩家手上的牌
	CardVector		m_suoHandCard[DESK_USER_COUNT];//手上锁的牌
	CardVector		m_outCard[DESK_USER_COUNT];	//玩家出的牌
	CardVector		m_pengCard[DESK_USER_COUNT];//玩家碰的牌，
	CardVector		m_minggangCard[DESK_USER_COUNT];//玩家明杠的牌
	
	CardVector		m_angangCard[DESK_USER_COUNT];//玩家暗杠的牌
	CardVector		m_eatCard[DESK_USER_COUNT];//玩家吃的牌

	ThinkTool		m_thinkInfo[DESK_USER_COUNT];//当前打牌思考状态
	ThinkTool       m_thinkInfoForYingKou[DESK_USER_COUNT]; // 硬扣思考状态
	//ThinkTool       m_checkTingInfo[DESK_USER_COUNT];  //不报听查听听口数据（**Ren 2017-12-27）
	ThinkUnit		m_thinkRet[DESK_USER_COUNT];//玩家返回思考结果
	TingState		m_tingState[DESK_USER_COUNT];	//听牌状态

	CardVector		m_deskCard;						//桌子上剩余的牌
	CardVector      m_Cannot_gangCard_MH[DESK_USER_COUNT];  // 记录用户不能杠的牌，用于扣点和推倒胡
	CardVector      m_guoPengCard[DESK_USER_COUNT];         //过碰记录，过碰后不过自己不允许再碰这张牌,忻州扣点中使用（**Ren 2017-11-25）

	typedef unsigned char desk_mask_t;
	SmallBitVec<desk_mask_t> m_winConfirmed; // 玩家可以胡了，并且已经点了“胡”按钮
	SmallBitVec<desk_mask_t> m_decideGuoHu; // 决定过胡，这样再 CheckThink 就不会给它弹了，任意人出牌就清空
	SmallBitVec<desk_mask_t> m_bHongTongYingKou; // 听牌时选择的硬扣状态: 0:不硬扣 1:硬扣
	SmallBitVec<desk_mask_t> m_bInYingKouThink;  // 当前用户是否在硬扣思考中 1 是，2: 不是
	SmallBitVec<desk_mask_t> m_bHongTongYingKouFirstOutCard;// 听牌后第一次出牌，

#if USE_NEW_DEALING
#else
	CardVector		m_3_laizi; // 看起来是用于洗牌，每盘都重置的

	// 这4个看起来是InitCard的临时状态，应该挪到InitCard里面
	Card			wan_base_card[BASE_CARD_COUNT];
	Card			bing_base_card[BASE_CARD_COUNT];
	Card			tiao_base_card[BASE_CARD_COUNT];
	Card			zi_base_card[BASE_CARD_COUNT];
#endif

	void clear_round() {
		this->~ShanXiRoundState__cxx_part();
		new (this) ShanXiRoundState__cxx_part;
		for (Lint i = 0; i< DESK_USER_COUNT; i++) {
			m_thinkInfo[i].Reset();
			m_thinkRet[i].Clear();
			m_tingState[i].clear();
		}
	}
};

struct ShanXiMatchState__c_part {
	Desk *m_desk;
	int m_round_offset;
	int m_round_limit;
	int m_accum_score[DESK_USER_COUNT];
	bool m_dismissed;


	void clear_match() {
		memset(this, 0, sizeof(*this));
		// 上面那句清零不满足要求的话，下面写自定义重置
	}
};

struct ShanXiMatchState__cxx_part {


	void clear_match() {
		this->~ShanXiMatchState__cxx_part();
		new (this) ShanXiMatchState__cxx_part;
	}
};


struct ShanXiRoundState: ShanXiRoundState__c_part, ShanXiRoundState__cxx_part {
	// 不要在这里加字段
	void clear_round() {
		ShanXiRoundState__c_part::clear_round();
		ShanXiRoundState__cxx_part::clear_round();
	}
};

// 一场Match会有好几盘Round
struct ShanXiMatchState: ShanXiFeatureToggles, ShanXiMatchState__c_part, ShanXiMatchState__cxx_part {
	// 不要在这里加字段
	void clear_match() {
		ShanXiFeatureToggles::clear_match();
		ShanXiMatchState__c_part::clear_match();
		ShanXiMatchState__cxx_part::clear_match();
	}
};



// 为了兼容以前用Card指针表示牌值的写法
struct CardPool {
	Card black_hole;
	Card pool[TILE_MAX - TILE_MIN + 1];
	CardPool() {
		memset(this, 0, sizeof(*this));
		for (tile_t tile = TILE_MIN; tile <= TILE_MAX; ++tile) {
			if ((TILE_TO_MASK(tile) & TILE_MASK_VALID) == 0) continue;
			Card *card = (*this)[tile];
			card->m_color = tile_to_color(tile);
			card->m_number = tile_to_number(tile);
		}
	}

	Card *operator [](tile_t tile) {
		if (tile < TILE_MIN || tile > TILE_MAX) return &black_hole;
		return &pool[tile - TILE_MIN];
	}
};



struct ShanXiDeskState: ShanXiRoundState, ShanXiMatchState {
	OperateState	mGameInfo;
	CardPool card_pool;

	void clear_match(int player_count) {
		ShanXiMatchState::clear_match();
		clear_round();
		m_player_count = player_count;
	}

	void clear_round() {
		ShanXiRoundState::clear_round();
		mGameInfo.m_hCard.clear();
		mGameInfo.m_cards_guo_gang_MH.clear();		 
	}

	void setup_match(std::vector<Lint> &l_playtype, int round_limit) {
		m_playtype.init_playtype_info(l_playtype, FEAT_GAME_TYPE);
		m_round_limit = round_limit;
		// 根据小选项计算允许的胡牌番型
		init_allowed_fans();
		mGameInfo.allowed_fans = m_allowed_fans;
		init_desk_card_remain_count();		
	}

	void increase_round() {
		++m_round_offset;
		if (m_desk && m_desk->m_vip) ++m_desk->m_vip->m_curCircle;
	}

	void set_play_state(DESK_PLAY_STATE v) {
		m_play_state = v;
		if (m_desk) m_desk->setDeskPlayState(v);
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

	Lint CheckPositionPrior(Lint CurPos, Lint first_pos, Lint second_pos) {
		// 以 CurPos 为原点，计算 first_pos 和 second_pos 到 CurPos 的逆时针距离
		Lint dif1 = first_pos - CurPos > 0 ? first_pos - CurPos : first_pos - CurPos + m_player_count;
		Lint dif2 = second_pos - CurPos > 0 ? second_pos - CurPos : second_pos - CurPos + m_player_count;
		// 选取逆时针距离短的那个
		if (dif1 < dif2) return first_pos;
		else return second_pos;
	}

	int find_primary_winpos(int bombpos, int winpos) {
		if (bombpos == INVAILD_POS) return winpos;
		for (int d = 0; d < m_player_count; ++d) {
			int pos = (bombpos + d) % m_player_count;
			if (m_winConfirmed[pos]) return pos;
		}
		return winpos;
	}

	unsigned sync_hands_to_m_ps(int x, Card* winCard) {
		PlayerState &ps = m_ps[x];
		CardVector &handCards = m_handCard[x];
		unsigned num_huns = 0;
		for (size_t i = 0; i < handCards.size(); ++i) {
			//TODO:此处添加洪洞王牌双王耗子牌判断（**Ren 2017-12-12）暂时注释掉（2018-01-08）
			/*if (HongTongWangPai == FEAT_GAME_TYPE && 
				m_playtype[MH_PT_HongTongWangPai_ShuangWang] &&
				IsHaozipai_HongTongShuangWang(x, handCards[i]))
			{
				++num_huns;
			}
			else if (IsHaozipai(handCards[i])) ++num_huns;  //去掉上面的if所以才改成if
			*/
			if (IsHaozipai(handCards[i])) ++num_huns;
			else ps.hands.add(CardToTile(handCards[i]));
		}
		// 天胡可能手里就有14张了？
		if (winCard && ps.hands.size() < 14) {
			if (IsHaozipai(winCard)) ++num_huns;
			else ps.hands.add(CardToTile(winCard));
		}
		return num_huns;
	}

	HuUnit const * hongtong_find_hu_unit(int winpos, Card * winCard, bool is_zimo)
	{
		if (NULL == winCard)
		{
			return NULL;
		}
		if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
			const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;
			for (size_t k = 0; k < hu_units.size(); ++k) {
				if (ToTile(hu_units[k].hu_card) == ToTile((winCard))) return &hu_units[k];
			}
		}
		return NULL;
	}

	HuUnit const *find_hu_unit(int winpos, Card *winCard, bool is_zimo) {
		// 应该必有winCard吧？TODO: 弄个天胡调试一下
		if (NULL == winCard) return 0;
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{
		    return hongtong_find_hu_unit(winpos,winCard,is_zimo);
		}
		// 抠点胡耗子牌，得让耗子牌取个最大点数
		// 别人打出的万能牌只能当牌值本身来用，不能当万能牌用
		if (IsHaozipai(winCard) && is_zimo) {
			// 这个判断应该肯定成立，加上算安全监测
			if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
				const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;

				struct SortUnit {
					int score;
					HuUnit const *hu_unit;
				};
				// 在排序之前得按抠点的胡牌要求过滤掉点数低的
				std::vector<SortUnit> candidates;
				for (size_t k = 0; k < hu_units.size(); ++k) {
					tile_t tile = ToTile(hu_units[k].hu_card);
					SortUnit unit = { hu_units[k].score, &hu_units[k] };
					if (KouDian == FEAT_GAME_TYPE) {
						int points = tile_to_points(tile);
						int limit = is_zimo ? 3 : 6;
						if (points < limit) continue;
					}
					candidates.push_back(unit);
				}

				// 在抠点听口计算时，会保证至少有一个6点以上的，所以不会走这个分支的
				if (candidates.empty()) return NULL;

				struct hufan_less_than {
					//bool operator ()(const HuUnit& lhs, const HuUnit& rhs) { return tile_to_points(CardToTile(lhs.hu_card)) < tile_to_points(CardToTile(rhs.hu_card)); }
					bool operator ()(const SortUnit& lhs, const SortUnit& rhs) { return lhs.score < rhs.score; }
				};
				std::vector<SortUnit>::const_iterator the_big = std::max_element(candidates.begin(), candidates.end(), hufan_less_than());
				return the_big->hu_unit;
			}
		} 
		else {
			if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
				const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;
				for (size_t k = 0; k < hu_units.size(); ++k) {
					if (ToTile(hu_units[k].hu_card) == ToTile((winCard))) return &hu_units[k];
				}
			}
		}
		return NULL;
	}	

	void sync_to_m_ps(int win_result, Card* winCard, FANXING conf) {
		tile_t last_tile = CardToTile(winCard);
		for (int x = 0; x < m_player_count; x++) {
			PlayerState &ps = m_ps[x];
			if (m_winConfirmed[x]) {
				ps.num_huns = sync_hands_to_m_ps(x, winCard);
			}
			//庄家+庄家番型
			ps.make_over_status(m_winConfirmed[x], x == m_zhuangpos);
			//点 “胡” 按钮的玩家
			if (m_winConfirmed[x]) {
				ps.last_tile = CardToTile(winCard);
				if (IsHaozipai(winCard)) last_tile = 0;
				FANXING tmp_conf = conf;
				if (WIN_ZIMO == win_result) tmp_conf |= FAN_ZiMo;
				if (x == m_zhuangpos) tmp_conf |= FAN_Zhuang;
				
				FANXING hongtong_qixiaodui_fan = FAN_NONE;
				if (HongTongWangPai == FEAT_GAME_TYPE)
				{
					bool is_zimo = WIN_ZIMO == win_result;
					HuUnit const * pHuUnit = hongtong_find_hu_unit(x, winCard, is_zimo);
					if (pHuUnit && (pHuUnit->fans & FAN_QiXiaoDui))
					{
					    // 洪洞七小对的胡牌类型不带耗子牌处理
						MHLOG("*******洪洞王牌找到七小对的胡口番型:%s", malgo_format_fans(pHuUnit->fans).c_str());
						hongtong_qixiaodui_fan = pHuUnit->fans;
					}
					//for (int i = 0; i < m_tingState[x].hu_units.size(); i++)
					//{
					//	MHLOG("***********i = %d m_tingState[x].hu_units[i].hu_card = %d, fans=%s", i, ToTile(m_tingState[x].hu_units[i].hu_card), 
					//		malgo_format_fans(m_tingState[x].hu_units[i].fans).c_str());
					//}
				}
				FANXING ps_conf_fans = tmp_conf;
				if (HongTongWangPai == FEAT_GAME_TYPE && hongtong_qixiaodui_fan)
				{
					ps_conf_fans  &= ~FAN_QiXiaoDui; //洪洞王牌不计算7小对，从听口获取
				}
				 
				MHLOG("********* 开始计算胡牌计算的番型 ps_conf_fans = %s", malgo_format_fans(ps_conf_fans).c_str());
				MHLOG("********* 开始计算胡牌计算的番型 hongtong_qixiaodui_fan = %d tmp_conf = %s, ",(bool)hongtong_qixiaodui_fan, malgo_format_fans(tmp_conf).c_str());
				FANXING fans = call_judge(ps, ps.num_huns, ps.hands, last_tile, ps_conf_fans
					, ps.make_pg_mask(), ps.sum_pg_tile_count_by_color());
				MHLOG("*********胡牌计算的番型%s", malgo_format_fans(fans).c_str());
				MHLOG("*********ps的番型番型 %s ", malgo_format_fans(ps.fans).c_str());
				// 从听口导入胡牌番型，除了扰乱 FAN_ZiMo FAN_Zhuang 外，不知道还有没有其它影响，所以先限定给抠点
				// 硬三嘴必须报听的，也从听口导入 fans，后面不用检测一张赢了，听口计算时检测了
				if (KouDian == FEAT_GAME_TYPE || YingSanZui == FEAT_GAME_TYPE || LinFenYiMenPai == FEAT_GAME_TYPE || 
					HongTongWangPai == FEAT_GAME_TYPE || XinZhouKouDian == FEAT_GAME_TYPE || NianZhongZi == FEAT_GAME_TYPE || 
					JinZhong == FEAT_GAME_TYPE) {
					bool is_zimo = WIN_ZIMO == win_result;
					HuUnit const *hu_unit = find_hu_unit(x, winCard, is_zimo);
					if (hu_unit) {
						fans = hu_unit->fans;
						ps.good_last_tile = ToTile(hu_unit->hu_card);						
					}
				}
				
				// 洪洞王牌从听口导入的番型
				if (HongTongWangPai == FEAT_GAME_TYPE)
				{		
					// 叠加上七小队的番型
					if (hongtong_qixiaodui_fan)
					{
						fans |= hongtong_qixiaodui_fan;
					}
				}

				// ps 里面有一些根据 mask 做的番型判定
				if (fans) fans |= ps.fans;

				// 听牌前是门清，听后有杠，就在检测一下是不是门清，如果不是门清就把门清的番型去掉
				// 听后杠会影响听口计算时得到的 fans 所以这里要处理一下
				// 听后杠影响：门清
				if ((fans & FAN_MenQing) && !ps.is_menqing()) {
					fans &= ~FAN_MenQing;
				}

				// 将 judgement 同步到 ps
				ps.fans = fans;

				if (ps.fans) {
					// 最终胡牌时从听口导入胡牌番型的机制，可能扰乱 FAN_ZiMo FAN_Zhuang
					ps.fans &= ~(FAN_ZiMo | FAN_Zhuang);
					if (WIN_ZIMO == win_result) ps.fans |= FAN_ZiMo;
					if (x == m_zhuangpos) ps.fans |= FAN_Zhuang;

					// 测试砍胡：听口大小为1
					if (m_tingState[x].hu_units.size() == 1) {
						ps.fans |= FAN_KanHu;
					}
					// 拐三角不能听怎么测试听口大小为1啊
					if ((m_allowed_fans & FAN_KanHu) && m_tingState[x].hu_units.size() == 0) {
						int pos = x;
						tile_mask_t pg_mask = calc_pg_mask(m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos]);
						std::vector<HuUnit> tingInfo;
						gCardMgrSx.calc_ting_kou(m_ps[pos], m_handCard[pos], pg_mask, mGameInfo, *this, tingInfo);
						if (tingInfo.size() == 1) {
							ps.fans |= FAN_KanHu;
						}
					}
					if (FEAT_GAME_TYPE == GuaiSanJiao)
					{
						if (ps.fans & FAN_KanHu) ps.fans &= ~FAN_PingHu;
						if ( (ps.fans&FAN_KanHu) && (ps.fans&(FAN_HaoHuaQiXiaoDui|FAN_QiXiaoDui)))
							ps.fans &= ~FAN_KanHu;
					}
				}

				ps.fans &= tmp_conf; // 番型必须在限定范围内

			}
		}
	}

	void add_hand_card(int pos, Card *card) {
		m_handCard[pos].push_back(card);
		gCardMgrSx.SortCard(m_handCard[pos]);
	}

	void erase_hand_card(int pos, Card *card, int n = 1) {
		gCardMgrSx.EraseCard(m_handCard[pos], card, n);
	}

	// 删除手牌，自动处理立四的锁牌
	void erase_suo_card(int pos, Card *card, int n = 1) {
		// m_suoHandCard是m_handCard的一部分，m_handCard包括m_suoHandCard
		// 碰牌时优先从立牌出，但是不能把立牌弄空了
		if (n <= 0) return;
		if (NULL == card) return;
		erase_hand_card(pos, card, n);
		// 非立四玩法，这个m_suoHandCard是空的
		if (!m_suoHandCard[pos].empty()) {
			gCardMgrSx.EraseCard(m_suoHandCard[pos], card, n);
			// 因为优先从立牌里面删，所以直接删就是，删空了，就补回一张来
			if (m_suoHandCard[pos].empty()) {
				m_suoHandCard[pos].push_back(card);
			}
		}
	}

	void InsertIntoKnowCard(Card* the_card, Lint num) {
		m_knowCardCount[gCardMgrSx.get_card_index(the_card)] += num;
	}

	void InsertIntoKnowCard(CardVector the_cards) {
		for (size_t x = 0; x < the_cards.size(); x++) {
			m_knowCardCount[gCardMgrSx.get_card_index(the_cards[x])] ++;
		}
	}


	bool IsHaozipai(const Card& card) { return std::find(mGameInfo.m_hCard.begin(), mGameInfo.m_hCard.end(), card) != mGameInfo.m_hCard.end(); }
	bool IsHaozipai(const Card* card) { return card && IsHaozipai(*card); }
	/**************************************************************************
	* 函数名：     IsHaozipai_HongTongShuangWang()
	* 描述：       洪洞王牌双王玩法判断是否为耗子牌（每个玩家耗子牌不一样）
	* 参数：
	*  @ pos 玩家位置
	*  @ card 判断的牌
	* 返回：       (bool)
	***************************************************************************/
	bool IsHaozipai_HongTongShuangWang(int pos, const Card* card) 
	{
		unsigned char tmp_wang = m_hongTongWangPai_ShuangWang[pos].ToChar();
		if (tile_is_valid(tmp_wang) && tmp_wang == card->ToChar())
		{
			return true;
		}
		return false;
	}
};




struct ShanXiVideoSupport: ShanXiDeskState {
	VideoLog		m_video;						//录像




	///////////////////////////////////////////////////////////////////
	// 这块集中存放和录像有关的函数
	///////////////////////////////////////////////////////////////////
	void VideoSave() {
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		//m_video.m_playType = m_desk->getPlayType();
		m_video.m_flag = m_registered_game_type;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		gWork.SendMsgToDb(video);
	}

	void VideoAdd__pos_oper_cards(int pos, int oper, std::vector<CardValue>& cards) {
		if (NULL == m_desk) return;

		m_video.AddOper(oper, pos, cards);
	}

	void VideoAdd__deal_card() {
		m_video.Clear();
		Lint id[4] = { 0 };
		Lint score[4] = { 0 };
		std::vector<CardValue> vec[4];
		for (Lint i = 0; i < m_player_count; ++i)
		{
			int user_data_id = m_desk ? m_desk->m_user[i]->GetUserDataId() : i;
			id[i] = user_data_id;
			score[i] = m_accum_score[i];
			for (size_t j = 0; j < m_handCard[i].size(); ++j)
			{
				CardValue v;
				v.kColor = m_handCard[i][j]->m_color;
				v.kNumber = m_handCard[i][j]->m_number;
				vec[i].push_back(v);
			}
		}
		int desk_id = m_desk ? m_desk->GetDeskId() : 0;
		std::vector<int> game_options;
		m_playtype.fill_vector_for_client(game_options, FEAT_GAME_TYPE);
		// 这个东西不知道做什么用的
		int desk_flag = m_desk ? m_desk->m_flag : 0;
		// 传过去的 score 会被存到它的 m_score 上，表示看这个录像时，玩家头像上显示的分数，不附加1000，客户端显示录像时会给它加上1000
		m_video.DealCard(id, vec, gWork.GetCurTime().Secs(), m_zhuangpos, score, desk_id, m_round_offset, m_round_limit, desk_flag, game_options);

	}

	void VideoAdd__pos_oper_card_n(Lint pos, Lint oper, const Card* card, int n) {
		std::vector<CardValue> cards;
		if (n > 0) cards.insert(cards.begin(), n, ToCardValue(card));
		VideoAdd__pos_oper_cards(pos, oper, cards);
	}
	void VideoAdd__pos_oper_card(Lint pos, Lint oper, const Card* card) {
		VideoAdd__pos_oper_card_n(pos, oper, card, 1);
	}
	void VideoAdd__pos_oper(Lint pos, Lint oper) {
		VideoAdd__pos_oper_card_n(pos, oper, NULL, 0);
	}
	void VideoAdd__doing(PosOp const &op) {
		std::vector<CardValue> cards;
		CardValue card;
		card.kNumber = op.code;
		card.kColor = op.tile;
		cards.push_back(card);
		VideoAdd__pos_oper_cards(op.pos, VIDEO_OPEN_DOING, cards);
		MHLOG_PLAYCARD("****Desk:%d 玩家操作 pos=%d type:%d card:%d",m_desk ? m_desk->GetDeskId():0, op.pos, op.code, op.tile);
	}

	void VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number) {
		// 这个是记录玩家执行了哪个操作
		std::vector<CardValue> cards;
		CardValue card;
		card.kNumber = op;
		card.kColor = card_color * 10 + card_number;
		cards.push_back(card);
		VideoAdd__pos_oper_cards(pos, VIDEO_OPEN_DOING, cards);
	}

	Lstring format_user_think(const ThinkVec & vect)
	{
		std::ostringstream ss;
		for (auto itr =  vect.begin(); itr != vect.end(); ++itr) {
			CardValue card;
			ss << '(' << itr->m_type << '-';
			if (itr->m_card.size() > 0)
				ss << (int)ToTile(itr->m_card[0]);	
			ss << ')' << ';';
		}
		return ss.str();
	}
	void VideoThink(Lint pos) {
		// 所谓玩家思考就是给玩家弹几个按钮以及允许出牌
		// 对于某个思考操作，可能不关联牌，可能关联一张牌，也可能关联多张牌
		// 下面的实现把CardValue当成通用数据结构了，用m_number存think枚举，用m_color存最多2张牌
		if (m_thinkInfo[pos].m_thinkData.size() > 0) {
			std::vector<CardValue> cards;
			for (auto itr = m_thinkInfo[pos].m_thinkData.begin(); itr != m_thinkInfo[pos].m_thinkData.end(); ++itr) {
				CardValue card;
				card.kNumber = itr->m_type;
				if (itr->m_card.size() > 0)
					card.kColor = itr->m_card[0]->m_color * 10 + itr->m_card[0]->m_number;
				if (itr->m_card.size() > 1)
					card.kColor = card.kColor * 1000 + itr->m_card[1]->m_color * 10 + itr->m_card[1]->m_number;
				cards.push_back(card);
			}
			VideoAdd__pos_oper_cards(pos, VIDEO_OPEN_THINK, cards);
			MHLOG_PLAYCARD("****Desk:%d Send 玩家思考  pos=%d think:%s", m_desk ? m_desk->GetDeskId() : 0, pos, format_user_think(m_thinkInfo[pos].m_thinkData).c_str());
		}
	}

	void VideoAdd__HaoZi(std::vector<Card> const &huns) {
		// 录像耗子
		std::vector<CardValue> cards;
		VecExtend(cards, huns);
		VideoAdd__pos_oper_cards(100, VIDEO_Oper_Haozi, cards);
	}

};


struct ShanXiGameCore: GameHandler, ShanXiVideoSupport {
	bool basic_can_hu_shoupao(int pos) {
		if (FEAT_ZhiKeZiMoHuPai) return false;
		if (m_decideGuoHu[pos]) return false;
		return true;
	}

	void add_gang_item(int pos, Card *card, int fire_pos, bool is_from_peng) {
		bool firer_has_ting = false;
		if (fire_pos != INVAILD_POS) firer_has_ting = m_tingState[fire_pos].m_ting_state == Ting_waitHu;		
		m_ps[pos].gangs.add(GangItem(ToTile(card), fire_pos, is_from_peng, firer_has_ting));		 
	}

	/******************************************************************
	* 函数名：    CheckYiMenPaiCanPG()
	* 描述：      检测一门牌中碰杠是否会破坏一门牌
	* 参数：
	*  @ pg_mask 碰杠掩码
	*  @ outCard 当前打出的牌
	* 返回：      (bool)
	*******************************************************************/
	bool CheckYiMenPaiCanPG(tile_mask_t pg_mask, const Card* curCard)
	{
		if (curCard == NULL)
		{
			return true;
		}
		tile_mask_t tmp_pgMask = pg_mask | TILE_TO_MASK(curCard->ToChar());
		int que_men_count = 0;
		if (0 == (tmp_pgMask & TILE_MASK_WAN)) ++que_men_count;
		if (0 == (tmp_pgMask & TILE_MASK_TONG)) ++que_men_count;
		if (0 == (tmp_pgMask & TILE_MASK_TIAO)) ++que_men_count;
		if (que_men_count >= 2)
		{
			return true;
		}
		return false;
	}

	// 未听牌状态，自己摸到一张牌，该有哪些可选操作
	void CheckGetCardOperator(int pos, bool can_gang) {
		mGameInfo.b_CanHu = true;
		mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;
		can_gang = can_gang && true;
		mGameInfo.b_CanAnGang = can_gang && true;
		mGameInfo.b_CanMingGang = can_gang && true;
		mGameInfo.b_CanDianGang = can_gang && false;
		mGameInfo.b_CanChi = false;

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.b_isHun = m_playtype.b_HaoZi;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;
		mGameInfo.m_pos = pos;
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{						
			if (m_playtype[PT_QueLiangMen]) // 洪洞王牌缺2门玩法中，如果玩家碰，杠了一门牌，其他门不再提示杠碰
			{				 
			}
			//洪洞王牌双王，将手牌中的双王传给gameInfo.m_hCard (**Ren 2017-12-12) 暂时注释掉（2018-01-08）
			/*if (m_playtype[MH_PT_HongTongWangPai_ShuangWang])
			{
				if (!is_invalid_card(m_hongTongWangPai_ShuangWang[pos]))
				{
					mGameInfo.m_hCard.push_back(m_hongTongWangPai_ShuangWang[pos]);
				}
			}*/
		}
		//如果不报听的可以查听（暂时不启用）
		/*if (!mGameInfo.b_CanTing)
		{
			m_checkTingInfo[pos].m_thinkData = gCardMgrSx.CheckCanTing_Check(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curGetCard, mGameInfo, *this);
		}*/
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckGetCardOperator(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curGetCard, mGameInfo, *this);
	}

	// 未听牌状态，别人出了一张牌，自己该有哪些可选操作
	void CheckOutCardOperator(int pos) {
		// 该分支是玩家没处于听的状态
		bool not_jin = TieJin != FEAT_GAME_TYPE || !IsHaozipai(m_curOutCard);
		// 别人出了一张牌，检测这个人可不可以接炮：只许自摸胡的玩法是不允许接炮的；有漏胡标记也不能接炮；这里是没报听呢但是玩法要求报听那这样也不能接炮；
		mGameInfo.b_CanHu = basic_can_hu_shoupao(pos) && not_jin && !(m_playtype.b_zimoHu || m_louHuCard[pos] || (FEAT_BaoTing || m_playtype.b_baoTing));
		mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;
		bool can_gang = not_jin;
		mGameInfo.b_CanAnGang = can_gang && false;
		mGameInfo.b_CanMingGang = can_gang && false;
		mGameInfo.b_CanDianGang = can_gang && m_GangThink != GangThink_qianggang;
		mGameInfo.b_CanPeng = not_jin && m_GangThink != GangThink_qianggang;		
		mGameInfo.b_CanChi = pos == GetNextPos(m_curPos) && m_GangThink != GangThink_qianggang;

		// 对于过胡的玩家不再提示杠和碰
		mGameInfo.b_CanDianGang = can_gang && (m_GangThink != GangThink_qianggang) && (!m_decideGuoHu[pos]);
		mGameInfo.b_CanPeng = not_jin && (m_GangThink != GangThink_qianggang) && (!m_decideGuoHu[pos]);

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.b_isHun = m_playtype.b_HaoZi;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;

		//一门牌检测碰杠是否会破坏一门牌（**Ren 2017-12-23）
		if (LinFenYiMenPai == FEAT_GAME_TYPE)
		{
			if (false == CheckYiMenPaiCanPG(calc_pg_mask(m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos]), m_curOutCard))
			{
				mGameInfo.b_CanDianGang = false;
				mGameInfo.b_CanPeng = false;
			}
		}
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{
			mGameInfo.b_CanPeng = !m_playtype[PT_HongTongBuKePengPai];
			mGameInfo.b_CanDianGang = !m_playtype[PT_HongTongBuKePengPai]; // 洪洞王牌不可碰牌，也不不可以点杠

			// 洪洞王牌缺2门玩法中，如果玩家碰，杠了一门牌，其他门不再提示杠碰
			if (m_playtype[PT_QueLiangMen])
			{
				tile_mask_t pg_mask = m_ps[pos].make_pg_mask();
				tile_mask_t out_tile_mask = TILE_TO_MASK(ToTile(m_curOutCard));

				//pg_mask &= ~TILE_TO_MASK(ToTile(mGameInfo.m_hCard[0])); // 排除万能牌(**Ren 2017-12-27)
				pg_mask &= ~TILE_MASK_ZIPAI;                       // 排除字牌				
				if (VecHas(mGameInfo.m_hCard, m_curOutCard)) //万能牌
				{
				}
				else
				{
					int quemen_count = m_ps[pos].get_quemen_count(pg_mask | out_tile_mask); // 计算缺门数量					
					if (quemen_count < 2)      //破了缺2门，不允许再碰和杠
					{
						mGameInfo.b_CanPeng = false;
						mGameInfo.b_CanDianGang = false;
					}
					else
					{
					}
				}
			}
		}
		// 忻州扣点：过碰后，自己出牌前不可再碰这张牌 （**Ren 2017-11-25）
		if (XinZhouKouDian == FEAT_GAME_TYPE && VecHas(m_guoPengCard[pos], m_curOutCard))
		{
			mGameInfo.b_CanPeng = false;
		}
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckOutCardOperator(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curOutCard, mGameInfo, *this);
	}

	// 听牌后自己摸牌出牌 检测操作类型
	void CheckGetCardOpeartorAfterTing(int pos) {

		MHLOG("*******************************听牌后用户出牌***********，这里检测用户是不是胡中发白????");
		if (FEAT_GAME_TYPE == HongTongWangPai && m_bHongTongYingKouFirstOutCard[pos])
		{
			MHLOG("**********进入硬扣决策功能，这里检测用户是不是胡中发白????");
			m_bHongTongYingKouFirstOutCard.clear();
		}
		mGameInfo.b_CanHu = true;
		// 抠点：这个判断似乎没必要，因为CardSx是根据已经计算好的听口来判断的，那个听口是按自摸计算的，接炮那里需要判断
		if (KouDian == FEAT_GAME_TYPE) {
			if (!(m_curGetCard && (m_curGetCard->m_number >= 3 || m_curGetCard->m_color > 3 || IsHaozipai(m_curGetCard)))) mGameInfo.b_CanHu = false;
		}
		mGameInfo.b_CanTing = false;
		bool can_gang = FEAT_TingPaiKeGang || m_playtype[PT_TingPaiKeGang];
		mGameInfo.b_CanAnGang = can_gang && true;
		mGameInfo.b_CanMingGang = can_gang && true;
		mGameInfo.b_CanDianGang = can_gang && false;
		mGameInfo.b_CanChi = false;

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;

		//撵中子：明杠刚上开花算点炮 （**Ren 2017-11-30）
		mGameInfo.b_getCardFromMAGang = true;
		if (NianZhongZi == FEAT_GAME_TYPE && 2 == m_getChardFromMAGang[pos])
		{
			mGameInfo.b_getCardFromMAGang = false;  //标记为明杠摸牌，杠上开花不算自摸
		}
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckGetCardOpeartorAfterTing(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curGetCard, mGameInfo, *this);
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{
			MHLOG("******Suijun print ThinkData pos = %d", pos);
			for (int i = 0; i < m_thinkInfo[pos].m_thinkData.size(); i++)
			{
				 
				if (m_thinkInfo[pos].m_thinkData[i].m_type == 2)
				{
					MHLOG("*****Suijun pos %d, has HU_THINK, 要胡的牌 %d ", pos, 0xff);
					for (int j = 0; j < m_thinkInfo[pos].m_thinkData[i].m_hu.size(); j++)
					{
					   Hu_type const & hu = m_thinkInfo[pos].m_thinkData[i].m_hu[j];
					   MHLOG("*******************Suijun 胡牌番型: hu.hu_type=%d ", hu.m_hu_type);
					}
					
				}
			}
		}
	}

	bool is_shangjin_shao(int pos) {
		// 贴金：上金少者只可自摸
		// 上金少的标准：存在两档以上并且处于最低的那一档
		struct shangjin_lessthan {
			bool operator ()(PlayerState const &lhs, PlayerState const &rhs) { return lhs.shangjin < rhs.shangjin; }
		};
		int min_shangjin =  std::min_element(m_ps + 0, m_ps + m_player_count, shangjin_lessthan()) - m_ps;
		int max_shangjin =  std::max_element(m_ps + 0, m_ps + m_player_count, shangjin_lessthan()) - m_ps;
		int min_v = 999, max_v = 0, my_v = 0;
		for (int i = 0; i < m_player_count; ++i) {
			int v = m_ps[i].shangjin;
			if (min_v > v) min_v = v;
			if (max_v < v) max_v = v;
			if (i == pos) my_v = v;
		}
		return min_v != max_v && my_v == min_v;
	}

	// 别人出牌，检测自己能做什么操作
	void CheckOutCardOpeartorAfterTing(int pos) {		
		// 该分支是玩家处于听的状态
		// 贴金：金牌不能用来点炮
		bool not_jin = TieJin != FEAT_GAME_TYPE || !IsHaozipai(m_curOutCard);
		// 贴金：上金少者只可自摸，就是上金多的才能收炮
		bool shangjin_enough = TieJin != FEAT_GAME_TYPE || !m_playtype[PT_ShangJinShaoZheZhiKeZiMo] || !is_shangjin_shao(pos);
		// 别人出了一张牌，检测这个人可不可以接炮：只许自摸胡的玩法是不允许接炮的；有漏胡标记也不能接炮；
		mGameInfo.b_CanHu = basic_can_hu_shoupao(pos) && shangjin_enough && not_jin && !(m_playtype.b_zimoHu || m_louHuCard[pos] || (m_louZimohu[pos] && m_playtype[PT_GuoHuZhiKeZiMo]));
		//忻州扣点：别人打出的牌6点以上才允许接炮胡  (**Ren)
		if (KouDian == FEAT_GAME_TYPE || XinZhouKouDian == FEAT_GAME_TYPE) {
			// 抠点：别人出的万能牌只能当牌值本身使用
			bool koudian_can_hupai = m_curOutCard && (m_curOutCard->m_number >= 6 || m_curOutCard->m_color > 3);
			if (!koudian_can_hupai) mGameInfo.b_CanHu = false;
		}
		mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;
		bool can_gang = not_jin && (FEAT_TingPaiKeGang || m_playtype[PT_TingPaiKeGang]);
		mGameInfo.b_CanAnGang = can_gang && true;
		mGameInfo.b_CanMingGang = can_gang && true;
		mGameInfo.b_CanDianGang = can_gang && true;
		mGameInfo.b_CanPeng = not_jin && false;
		mGameInfo.b_CanChi = false;
		// 对于过胡的玩家不再提示杠和碰
		mGameInfo.b_CanDianGang = can_gang && (m_GangThink != GangThink_qianggang) && (!m_decideGuoHu[pos]);
		if (FEAT_GAME_TYPE == HongTongWangPai)
		{
			mGameInfo.b_CanDianGang = !m_playtype[PT_HongTongBuKePengPai]; // 洪洞王牌不可碰牌，也不不可以点杠
		}		 

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.b_isHun = m_playtype.b_HaoZi;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckOutCardOpeartorAfterTing(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curOutCard, mGameInfo, *this);
	}

#if USE_NEW_DEALING
#else
	void InitCard()
	{
		m_3_laizi.clear();
		//万
		Lint index = 0;
		for (Lint k = 0; k < 9; k++)  //9张牌
		{
			for (Lint i = 0; i < 4; ++i)		//循环加四次
			{
				wan_base_card[index].m_color = 1;
				wan_base_card[index].m_number = k + 1;
				index++;
			}
		}
		//饼
		index = 0;
		for (Lint k = 0; k < 9; k++)  //9张牌
		{
			for (Lint i = 0; i < 4; ++i)		//循环加四次
			{

				bing_base_card[index].m_color = 2;
				bing_base_card[index].m_number = k + 1;
				index++;
			}
		}

		//条
		index = 0;
		for (Lint k = 0; k < 9; k++)  //9张牌
		{
			for (Lint i = 0; i < 4; ++i)		//循环加四次
			{

				tiao_base_card[index].m_color = 3;
				tiao_base_card[index].m_number = k + 1;
				index++;
			}
		}
		for (Lint i = 0; i < BASE_CARD_COUNT; ++i)
		{
			m_3_laizi.push_back(&wan_base_card[i]);
			m_3_laizi.push_back(&bing_base_card[i]);
			m_3_laizi.push_back(&tiao_base_card[i]);
		}

		//字
		if (FEAT_DaiFeng || m_playtype.b_daiFeng)
		{
			index = 0;
			for (Lint k = 0; k < 7; k++)  //6张牌
			{
				for (Lint i = 0; i < 4; ++i)		//循环加四次
				{

					zi_base_card[index].m_color = 4;
					zi_base_card[index].m_number = k + 1;
					index++;
				}
			}
			for (Lint i = 0; i < 28; ++i)
			{
				m_3_laizi.push_back(&zi_base_card[i]);
			}
		}
		gCardMgrSx.SortCard(m_3_laizi);
	}
#endif

	void SuijunPrintfFanxing()
	{
		MHLOG("********__COUNTER__ = %lld, FANXING_COUNTER_BASE = %lld ", FANXING_COUNTER_BASE - 1, FANXING_COUNTER_BASE);
		MHLOG("******* FAN_PingHu = %lld, FAN_ShiSanYao = %lld FAN_HaoHuaQiXiaoDui= %lld FAN_QiXiaoDui=%lld FAN_YiTiaoLong= %lld", FAN_PingHu, FAN_ShiSanYao, FAN_HaoHuaQiXiaoDui,
			FAN_QiXiaoDui, FAN_YiTiaoLong);
	}

	void start_round() {
		
		SuijunPrintfFanxing();
		clear_round();
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);

		//通知manager局数变化
		if (m_desk && m_desk->m_vip&&m_desk->m_clubInfo.m_clubId!=0)
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle+1, m_desk->m_vip->m_maxCircle);

#if USE_NEW_DEALING
		DealCardByDesk();
#else
		DealCardByDesk();
		DealHaoZi();
#endif
		if(m_desk)
			m_desk->m_vip->ResetJinAnGangCount();
		CheckStartPlayCard();
		char buf[128] = {0};
		char buf2[128];
		MHLOG_PLAYCARD("****Desk %d start round %d/%d. flag=%d, player_count=%d", m_desk->GetDeskId(), m_round_offset, m_round_limit, FEAT_GAME_TYPE, m_player_count);
		for (int i = 0; i < m_player_count; i++) 
		{
			if (m_desk && m_desk->m_user[i])
			{
#ifdef WIN32
				strncpy_s((char*)buf, 128, m_desk->m_user[i]->GetUserData().m_nike.c_str(), 128);
				buf[127] = 0;
				MultiByteToWideChar(CP_UTF8, 0, buf, -1, (wchar_t*)buf2, 63);
				buf2[126] = buf2[127] = 0;
				WideCharToMultiByte(CP_ACP, 0, (wchar_t*)buf2, -1, buf, 127, NULL, NULL);
				buf[127] = 0;
#endif // WIN32	
				buf[127] = 0;
				MHLOG_PLAYCARD("****Desk %d user[pos:%d id:%d nike:%s]", m_desk->GetDeskId(), i, m_desk->m_user[i]->GetUserDataId(), buf);
			}
		}
	}

	int calc_haozi_count() {
		MHLOG("****Setup 耗子数量****");
		if (KouDian == FEAT_GAME_TYPE) {
			if (m_playtype[PT_ShuangHaoZi]) return 2; // 抠点双耗子玩法
			if (m_playtype.b_HaoZi) return 1; // 抠点捉耗子玩法
		} 
		else if (TieJin == FEAT_GAME_TYPE) {
			// 贴金默认4金，如果8金小选项被选定就走8金
			if (m_playtype[PT_BaJin]) return 2; // 贴金：8金
			else return 1; // 贴金：4金
		}
		//洪洞王牌不是双王，才随机选耗子（**Ren 2017-12-12）
		else if (HongTongWangPai == FEAT_GAME_TYPE /*&& !m_playtype[MH_PT_HongTongWangPai_ShuangWang]*/){
			return 1; // 洪洞王牌，王牌当耗子处理
		}
		// 红中耗子(**Ren2017-12-6)
		else if (m_playtype[MH_PT_HongZhongHaoZi]){
			return 1;
		}
		return 0;
	}
	int calc_haozi_style() {
		MHLOG("*****Enter setup 耗子模式 ****");
		switch (FEAT_GAME_TYPE) {
		//添加扣点风耗子
		case KouDian:  
			if (m_playtype[MH_PT_FengHaoZi]){
				return HAOAI_STYLE_KouDian_FengHaozi;
			}
			else{
				return HAOZI_STYLE_KouDian;
			}
			break;
		case TieJin: return HAOZI_STYLE_TieJin;
		case HongTongWangPai: return HAOZI_STYLE_HongTongWanPai;
		//推到胡红中耗子(**Ren 2017-12-6)
		case TuiDaoHu: 
			if (m_playtype[MH_PT_HongZhongHaoZi]) {
				return HAOZI_STYLE_HongZhongHaoZi;
			}
			return HAOZI_STYLE_NONE;
			break;
		default: return HAOZI_STYLE_NONE;
		}
	}

	TileDealing make_free_dealing() {
		// 洗牌  
		//红中耗子不带风，添加红中到数牌中（**Ren 2017-12-18）
		tile_mask_t allowed_tile_set = (FEAT_DaiFeng || m_playtype[PT_DaiFeng]) ? TILE_MASK_VALID : TILE_MASK_SHUPAI;
		if (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi] && false == m_playtype[PT_DaiFeng])
		{
			allowed_tile_set |= TILE_TO_MASK(45);
		}
		TileDealing dealing(m_player_count, allowed_tile_set, 13, calc_haozi_count(), calc_haozi_style());
		return dealing;
	}

	void SuijunSetupPlayerCard()
	{
		MHLOG("****** SuijunSetupPlayerCard调牌*******");
		Card * pCard = m_desk->m_player1Card;
		//tile_t * pTile = NULL;
		// pTile = HTTestClass.tile_4men; //四门不能听，胡
		// pTile = HTTestClass.tile_3men; //三门不可听，胡
		// pTile = HTTestClass.tile_2men;
		// pTile = HTTestClass.tile_feng; //风成副
		// pTile = HTTestClass.tile_sanyan; //字成
		// pTile = HTTestClass.tile_3men7xiaodui; // 三门全七小对
		//pTile = HTTestClass.tile_yingsanzui;


		std::vector<int> cardsVect;
		LoadPlayerCards(cardsVect, 1);	 
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 2);
		pCard = m_desk->m_player2Card;
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 3);
		pCard = m_desk->m_player3Card;
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 4);
		pCard = m_desk->m_player4Card;
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 5);
		for (int i = 0; i < 84; i++)
		{
			if (i < cardsVect.size())
			{
				m_desk->m_SendCard[i] = cardsVect[i];
			}
		}
		
		LoadPlayerCards(cardsVect, 6);
		for (int i = 0; i < 2; i++)
		{
			if (i <  cardsVect.size())
			{
				m_desk->m_HaoZiCard[i] = cardsVect[i];
			}
		}
	
	}

	TileDealing make_desk_dealing() {
		Lstring   strOut("");		
		MHLOG("****** Enter make_desk_dealing 开始洗牌");
		TileDealing dealing(make_free_dealing());
		bool can_assign = gConfig.GetDebugModel();
		MHLOG("******gConfig.GetDebugModel() = %d", can_assign);
		if (can_assign) {
			tile_t buf[34 * 4];
			size_t n;			
			SuijunSetupPlayerCard();
			n = SetTileArray(buf, m_desk->m_player1Card);
			Lint pos0 = m_desk->GetFirstZhuangPos();
			dealing.assign_to_player(pos0, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_player2Card);
			Lint pos1 = (pos0 + 1) % 4;
			dealing.assign_to_player(pos1, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_player3Card);
			Lint pos2 = (pos1 + 1) % 4;
			dealing.assign_to_player(pos2, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_player4Card);
			Lint pos3 = (pos2 + 1) % 4;
			dealing.assign_to_player(pos3, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_SendCard);
			dealing.assign_to_rest(buf, buf + n);
			dealing.finish();
			// 得先 finish 再 setup_haozi
			n = SetTileArray(buf, m_desk->m_HaoZiCard);
			dealing.setup_haozi(buf, buf + n);
		} else {
			dealing.finish();
			dealing.setup_haozi(NULL, NULL);
		}
		return dealing;
	}

	void DealCardByDesk() {
		MHLOG("**********Enter DealCardByDesk 开局洗牌---------------");
		// 洗牌
		TileDealing dealing(make_desk_dealing());

		dealing.DumpDeskInfo();
		dealing.DumpHaoziInfo();

		DealCard(dealing);
	}

	void DealCard(TileDealing &dealing) {
		// 调试发现：m_vip的m_curCircle是0起始，在每盘结束时递增，最后一盘结束后增长到m_maxCircle
		// 而m_vip->IsBegin()的实现看起来有问题，它判断m_curCircle是否为0，这样第一局开始返回false，第二、三、四局开始返回true
		// 是命名有问题，这里的使用没问题，第一局房主为庄，第二局以后这里不修改庄
		if (m_round_offset > 0) {
			// 第二局以后不在这里修改庄
		} else {
			// 第一局房主为庄
			m_zhuangpos = m_desk->GetFirstZhuangPos();//L_Rand(0, m_player_count - 1);
		}
		m_curPos = m_zhuangpos;

#if USE_NEW_DEALING

		// 设置立四的锁牌
		for (int i = 0; i < dealing.player_count; ++i) {
			TileVec &hands = dealing.players[i];
			CardVector &suo_cards = m_suoHandCard[i];
			// 手牌尚未排序，和调牌顺序一致，取前几个作为锁牌
			for (Lsize j = 0; j < FEAT_SUO_COUNT; ++j) {
				suo_cards.push_back(card_pool[hands[j]]);
			}
			// 不能删掉，因为在服务器，手牌包括锁牌
			//hands.pop_front(FEAT_SUO_COUNT);
		}

		// 设置手牌
		// 这里不能排序，会影响后面给客户端传立四的牌，客户端会自己处理排序的，不用服务器传排好序的牌
		//dealing.sort_hands();
		for (int i = 0; i < dealing.player_count; ++i) {
			CardVector &hand_cards = m_handCard[i];
			TileVec &hands = dealing.players[i];
			for (Lsize j = 0; j < hands.size(); ++j) {
				hand_cards.push_back(card_pool[hands[j]]);
			}
		}

		// 设置桌牌
		for (Lsize j = 0; j < dealing.rest.size(); ++j) {
			m_deskCard.push_back(card_pool[dealing.rest[j]]);
		}

		// 设置耗子
		for (size_t j = 0; j < dealing.haozi_count; ++j) {
			mGameInfo.m_hCard.push_back(ToCard(dealing.haozi_tiles[j]));
		}

		if (mGameInfo.m_hCard.size() == 1) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			Card haozi0 = mGameInfo.m_hCard[0];
			LLOG_ERROR("Desk %d create 1 haozi %d|%d", desk_id, haozi0.m_color, haozi0.m_number);
		} else if (mGameInfo.m_hCard.size() == 2) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			Card haozi0 = mGameInfo.m_hCard[0];
			Card haozi1 = mGameInfo.m_hCard[1];
			LLOG_ERROR("Desk %d create 2 haozi %d|%d, %d|d", desk_id, haozi0.m_color, haozi0.m_number, haozi1.m_color, haozi1.m_number);
		}

		// 庄家多发一张牌
		if (!m_deskCard.empty()) {
			Card *newCard = m_deskCard.back();
			m_deskCard.pop_back();
			// 这地方自动把玩家手牌排序了
			add_hand_card(m_curPos, newCard);
			// 也给 dealing 发下，后面给客户端传信息用
			dealing.players[m_curPos].add(ToTile(newCard));
		}

#else
		InitCard();
		//发牌   
		if (gConfig.GetDebugModel() && (m_desk->m_player1Card[0].m_color>0 || m_desk->m_player2Card[0].m_color>0 || m_desk->m_player3Card[0].m_color>0 || m_desk->m_player4Card[0].m_color>0 || m_desk->m_SendCard[0].m_color>0)) {
			// 调试指定发牌的牌局
			gCardMgrSx.DealCard2(m_3_laizi, m_handCard, m_player_count, m_deskCard, m_desk->m_player1Card, m_desk->m_player2Card, m_desk->m_player3Card, m_desk->m_player4Card, m_desk->m_SendCard, m_desk->getGameType(), m_playtype);
		} else {
			// 正常随机发牌的牌局
			gCardMgrSx.DealCard(m_3_laizi, m_handCard, m_player_count, m_deskCard, m_desk->getGameType(), m_playtype);
		}

		// 庄家多发一张牌
		Card *newCard = m_deskCard.back();
		m_deskCard.pop_back();
		add_hand_card(m_curPos, newCard);

		// 设置立四的锁牌
		for (Lint i = 0; i < m_player_count; ++i) {
			CardVector const &hand_cards = m_handCard[i];
			CardVector &suo_cards = m_suoHandCard[i];
			for (Lsize j = 0; j < FEAT_SUO_COUNT; ++j) {
				suo_cards.push_back(hand_cards[j]);
			}
		}
#endif

		// 因为锁牌会发生变化，所以直接让 mGameInfo.m_suoCard 指向 m_suoHandCard
		mGameInfo.m_suoCard = m_suoHandCard;

		// 发送消息给客户端
		for (Lint i = 0; i < m_player_count; ++i) {
			LMsgS2CPlayStart msg;
			msg.m_zhuang = m_curPos;
			msg.m_pos = i;
			// 桌上所有玩家的累计分数
			for (int x = 0; x < m_player_count; x++) {
				msg.m_score.push_back(m_accum_score[x] + ShanXi_JiaDe1000FenZhiZaiTouXiangChuXianShi);
			}
			CardVector const &hand_cards = m_handCard[i];
			// m_handCard 被排序了，用 dealing 的给客户端发
			TileVec &hands = dealing.players[i];
			CardVector &suo_cards = m_suoHandCard[i];
			for (Lsize j = 0; j < hands.size(); ++j) {
				if (j < FEAT_SUO_COUNT) {
					// 给客户端的锁牌
					msg.m_cardSuoValue[j] = ToCardValue(hands[j]);
				} else {
					// 给客户端的手牌不包括锁牌
					Lint index = j - FEAT_SUO_COUNT;
					msg.m_cardValue[index] = ToCardValue(hands[j]);
				}
			}

			for (Lint j = 0; j < m_player_count; ++j) {
				msg.m_cardCount[j] = m_handCard[j].size();
			}
			msg.m_dCount = (Lint)m_deskCard.size();

			//开局新加两个字段 by wyz
			msg.m_gamePlayerCount=m_player_count;
			if (m_desk!=NULL)   //如果随机选座这里还需要改
			{
				for (Lint i=0; i<m_player_count; ++i)
				{
					if (m_desk->m_user[i])
						msg.m_posUserid[i]=m_desk->m_user[i]->GetUserDataId();
				}
			}
			// 将消息发给这个玩家
			notify_user(msg, i);
			MHLOG("**********Send to client 游戏开始 LMsgS2CPlayStart chaird=%d", i);
		}

		//录像功能
		VideoAdd__deal_card();

#if USE_NEW_DEALING
		// 如果有耗子就刷给客户端、记录录像
		if (!mGameInfo.m_hCard.empty()) {
			MHLOG("************ Send to client 耗子牌 LLMsgS2CUserHaozi"); 
			notify_HaoZi();
			//调试发现耗子消息和起手决策消息会影响第2局开始时客户端接收起手决策消息，暂时加延迟再测试
			if (HongTongWangPai == FEAT_GAME_TYPE)
			{
				//Sleep(250);
			}
			// 录像耗子
			VideoAdd__HaoZi(mGameInfo.m_hCard);
		}
#endif
	}

	// 贴金：8金：系统每局随机定义2张牌为金，则该牌的8张都是金，但这两张牌必须是间隔相邻的。
	// 比如3万、5万，则4个3万和4个5万都是金；也可以3万，5筒；服务器处理时按照数字割开即可。花色不限制。
	// 特殊的金：东南西北为一副牌，中发白为一副牌，因为8斤必须间隔所以：红中白板、东风西风、南风北风都是8金，其他的不行。
	std::pair<tile_t, tile_t> tiejin_generate_bajin() {
		// 7x3x3 + 3 = 66 种可能
		int i = L_Rand(0, 65);
		tile_t jin1, jin2;
		if (i == 65) jin1 = 45, jin2 = 47; // 中
		else if (i == 64) jin1 = 42, jin2 = 44; // 南
		else if (i == 63) jin1 = 41, jin2 = 43; // 东
		else {
			int c2 = i / 21;
			i %= 21;
			int c1 = i / 7;
			i %= 7;
			jin1 = (tile_t)((c1+1)*10 + i);
			jin2 = (tile_t)((c2+1)*10 + i + 2);
		}
		return std::make_pair(jin1, jin2);
	}

	void DealHaoZi() {
		tile_t buf[2];
		size_t n = 0;
		bool can_assign = gConfig.GetDebugModel();
		if (can_assign) {
			n = SetTileArray(buf, m_desk->m_HaoZiCard);
		}
		DealHaoZiInternal(buf, n);
	}

	void DealHaoZiInternal(tile_t buf[], size_t n) {
		if (KouDian == FEAT_GAME_TYPE && m_playtype.b_HaoZi) {
			// 抠点捉耗子玩法
			goto haozi1;
		} else if (TieJin == FEAT_GAME_TYPE) {
			// 贴金默认4金，如果8金小选项被选定就走8金
			if (m_playtype[PT_BaJin]) {
				// 贴金：8金
				goto haozi2;
			} else {
				// 贴金：4金
				goto haozi1;
			}
		}
haozi1:
		if (n) {
			mGameInfo.m_hCard.push_back(ToCard(buf[0]));
		} else {
			// L_Rand(begin, end) 包括末端
			int index = L_Rand(0, m_deskCard.size() - 1);
			mGameInfo.m_hCard.push_back(ToCardValue(m_deskCard[index]));
		}
		goto haozi0;
haozi2:
		if (n) {
			mGameInfo.m_hCard.push_back(ToCard(buf[0]));
			mGameInfo.m_hCard.push_back(ToCard(n >= 2 ? buf[1] : buf[0] + 2));
		} else {
			std::pair<tile_t, tile_t> bajin = tiejin_generate_bajin();
			mGameInfo.m_hCard.push_back(TileToCard(bajin.first));
			mGameInfo.m_hCard.push_back(TileToCard(bajin.second));
		}
		goto haozi0;
haozi0:

		// 如果有耗子就刷给客户端、记录录像
		if (!mGameInfo.m_hCard.empty()) {
			notify_HaoZi();
			// 录像耗子
			VideoAdd__HaoZi(mGameInfo.m_hCard);
		}
	}

	/*********************************************************************************
	* 函数名：     hongTongWangPaiShuangWang()
	* 描述：       查找手牌中的两张及以上相同的牌给客户端选王，并构造ThinkTool结构
	* 参数：
	*  @ pos 玩家位置
	*  @ hand_cards 玩家手牌
	* 返回：       (void)
	**********************************************************************************/
	void hongTongWangPaiShuangWang(int pos, const CardVector& hand_cards)
	{
		CardVector tmp_hand_cards(hand_cards);
		CardVector tmp_same;  //存储重复的牌
		gCardMgrSx.SortCard(tmp_hand_cards);
		if (tmp_hand_cards.empty())
		{
			return;
		}
		Lint color = tmp_hand_cards.front()->m_color;
		Lint number = tmp_hand_cards.front()->m_number;
		auto it = tmp_hand_cards.begin();
		auto it_end = tmp_hand_cards.end();
		for (; it != it_end;)
		{
			if (color == (*it)->m_color && number == (*it)->m_number && !(tmp_same.back() == *it))
			{
				tmp_same.push_back(*it);
			}
			else
			{
				color = (*it)->m_color;
				number = (*it)->m_number;
			}
			++it;
		}
		ThinkUnit think_wang;
		//选王思考
		think_wang.m_type = THINK_OPERATOR_HONGTONG_XUANWANG;
		VecExtend(think_wang.m_card, tmp_same);
		m_thinkInfo[pos].m_thinkData.push_back(think_wang);
	}

	/********************************************************
	* 函数名：   CheckStartPlayCard()
	* 描述：     游戏开始
	* 返回：     (void)
	*********************************************************/
	void CheckStartPlayCard()
	{
		//洪洞王牌双王大将选王的过程，这里庄家已经多发了一张牌14张牌（**Ren 2017-12-12）暂时注释掉（2018-01-08）
		//if (HongTongWangPai == FEAT_GAME_TYPE && m_playtype[MH_PT_HongTongWangPai_ShuangWang])
		//{
		//	//计算每个人手牌中两张及两张以上的牌
		//	for (int pos = 0; pos < m_player_count; ++pos) 
		//	{
		//		hongTongWangPaiShuangWang(pos, m_handCard[pos]);
		//	}
		//	//将消息推送给客户端，让用户选王
		//	set_play_state(DESK_PLAY_THINK_CARD);
		//	SendThinkInfoToClient();

		//	//将所有人选王的结果发送给客户端
		//	notify_desk_xuanwang();
		//}

		SetPlayIng(m_curPos, false, true, true, true);
		m_curGetCard = m_handCard[m_curPos].back();
		//m_handCard[m_curPos].pop_back();
		m_needGetCard = true;
	}


	struct SetPlayIng__Options {
		bool needGetCard;
		bool needting;
		bool needotherthink;
		bool first_think;
		Lint pengPos;
		SetPlayIng__Options(): needGetCard(false), needting(false), needotherthink(false), first_think(false), pengPos(INVAILD_POS) {}
	};

	inline void SetPlayIng_SetTimeBegin(Lint pos)
	{
		LTime cur;
		m_getcardTimeBegin[pos] = cur.MSecs();
	}

	//摸牌
	void SetPlayIng(Lint pos, bool needGetCard, bool needting, bool needotherthink, bool first_think = false) {
		SetPlayIng_SetTimeBegin(pos); //SUIJUN
		SetPlayIng__Options opt;
		opt.needGetCard = needGetCard;
		opt.needting = needting;
		opt.needotherthink = needotherthink;
		opt.first_think = first_think;
		SetPlayIng__opt(pos, opt);

		while(true)
		{
			LTime end;
			m_getcardTimeEnd[pos] = end.MSecs();
			LLOG_ERROR("****SetPlayIng Desk:%d pos:%d calc_time:%ld ms, ting_state:%d", m_desk ? m_desk->GetDeskId():-1, pos, m_getcardTimeEnd[pos] - m_getcardTimeBegin[pos], m_tingState[pos].m_ting_state == Ting_waitOutput ? 1 : 0);
			break;
		}
	}

	void SetPlayIng__opt(Lint pos, const SetPlayIng__Options& opt) {
		if (m_desk == NULL) {
			LLOG_DEBUG("HanderUserEndSelect NULL ERROR ");
			return;
		}
		if (pos < 0 || pos >= INVAILD_POS) {
			LLOG_ERROR("Desk::SetPlayIng pos error ！");
			return;
		}
		// 洪洞剩餘14張牌
		if ((m_deskCard.size() <= m_remain_card_count && opt.needGetCard) )
		{
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		// 忻州扣点：剩余12张牌 (**Ren 2017-11-22)
		/*if (XinZhouKouDian == FEAT_GAME_TYPE && (m_deskCard.size() <= 12 && opt.needGetCard))
		{
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}*/
		// 穿庄
		if (m_deskCard.empty() && opt.needGetCard) {
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		m_curPos = pos;
		m_thinkInfo[pos].m_thinkData.clear();
		set_play_state(DESK_PLAY_GET_CARD);
		m_needGetCard = false;
		if (opt.needGetCard) {
			// 已经听后了
			if (m_tingState[m_beforePos].m_ting_state == Ting_waitHu) {

			}
			m_needGetCard = true;
			m_curGetCard = m_deskCard.back();
			m_deskCard.pop_back();
			//录像
			VideoAdd__pos_oper_card(pos, VIDEO_OPER_GET_CARD, m_curGetCard);
			m_tingState[pos].m_out_card = m_curGetCard;
		}
		if (opt.needotherthink) {
			if (m_tingState[pos].m_ting_state == Ting_waitHu) {
				CheckGetCardOpeartorAfterTing(pos);
			} else {
				CheckGetCardOperator(pos, true);
			}
			VideoThink(pos);
		} else if (opt.needting) {
			CheckGetCardOperator(pos, false);
			VideoThink(pos);
		}
		if (pos == opt.pengPos && opt.pengPos != INVAILD_POS && (FEAT_BaoTing || m_playtype.b_baoTing)) {
			// 碰牌后检测是否可以听
			mGameInfo.b_CanChi = false;
			mGameInfo.b_CanHu = true;
			mGameInfo.b_CanAnGang = false;
			mGameInfo.b_CanMingGang = false;
			mGameInfo.b_CanDianGang = false;
			mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;

			mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
			mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
			mGameInfo.b_isHun = m_playtype.b_HaoZi;
			mGameInfo.m_GameType = FEAT_GAME_TYPE;
			mGameInfo.m_pos = pos;
			m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckAfterPeng(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_curOutCard, mGameInfo, *this);
			bool think = !m_thinkInfo[pos].m_thinkData.empty();
			if (think) {
				mGameInfo.b_CanChi = false;
				mGameInfo.b_CanHu = true;
				mGameInfo.b_CanAnGang = false;
				mGameInfo.b_CanMingGang = false;
				mGameInfo.b_CanDianGang = false;
				mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;

				mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
				mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
				mGameInfo.b_isHun = m_playtype.b_HaoZi;
				mGameInfo.m_GameType = FEAT_GAME_TYPE;
				VideoThink(pos);
			}
		}

		//对于不报听的玩法，添加查听功能（**Ren 2017-12-27）（暂时不启用）
		/*if (pos == opt.pengPos && opt.pengPos != INVAILD_POS && !(FEAT_BaoTing || m_playtype.b_baoTing))
		{
			m_checkTingInfo[pos].m_thinkData = gCardMgrSx.CheckAfterPeng(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_curOutCard, mGameInfo, *this);
		}*/

		if (m_needGetCard) {
			add_hand_card(pos, m_curGetCard);
			MHLOG_PLAYCARD("****Desk:%d 玩家摸牌 pos: %d, card=%d[%s]", m_desk ? m_desk->GetDeskId() : 0, pos, ToTile(m_curGetCard), (*m_curGetCard).ToString().c_str());
		}

		SetPlayIng__update_client(pos, opt);
	}

	// 有耗子牌的听口添加上耗子牌（不影响听口，只是给客户端显示耗子牌）
	void Extend_TingChuKou_Haozi(LMsgS2COutCard & msg)
	{
		//添加推到胡红中耗子牌（**Ren 2017-12-07）
		if (FEAT_GAME_TYPE != KouDian && !(FEAT_GAME_TYPE == TuiDaoHu && m_playtype[MH_PT_HongZhongHaoZi]))
			return;
		for (size_t m = 0; m < mGameInfo.m_hCard.size(); m++)
		{
			tile_t hu_tile = ToTile(mGameInfo.m_hCard[m]);
			for (size_t n = 0; n < msg.kTingChuKou.size(); n++)
			{
				if (!VecHas(msg.kTingChuKou[n].kTingKou, hu_tile))
				{
					msg.kTingChuKou[n].kTingKou.push_back(hu_tile);
				}
			}
		}
	}

	void SetPlayIng__update_client(Lint pos, const SetPlayIng__Options& opt) {
		for (Lint i = 0; i < m_player_count; ++i) {
			LMsgS2COutCard msg;
			MHLOG("SetPlayIng__update_client m_round_offset =  %d", m_round_offset);

			msg.kTime = 15;
			msg.kPos = pos;
			msg.kDCount = (Lint)m_deskCard.size();
			msg.kTing = m_tingState[i].m_ting_state == Ting_waitHu ? 1 : 0;
			msg.kFlag = 1;

			if (m_needGetCard) {
				msg.kFlag = 0;
			}

			if (pos == i) {
				if (m_needGetCard) {
					msg.SetCurCard(ToCardValue(m_curGetCard));
					if (msg.kTing) {
						//msg.m_tingCards.push_back(msg.GetCurCard());
						m_tingState[pos].m_out_card = m_curGetCard; // 不知道前面设置没，这里多设置一次
						LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
						//扣点胡口需要加入耗子牌
						if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
						{
							Extend_TingChuKou_Haozi(msg);
						}
					}
				}
				ThinkVec &units = m_thinkInfo[pos].m_thinkData;
				MHLOG_TING("**** pos = %d, m_thinkInfo[pos].m_thinkData.size() = %d", pos , m_thinkInfo[pos].m_thinkData.size());
				for (Lsize j = 0; j < units.size(); ++j) {
					ThinkData info;
					info.kType = units[j].m_type;
					if (opt.first_think && info.kType == THINK_OPERATOR_BOMB && !m_handCard[pos].empty()) {
						Card *back = m_handCard[pos].back();
						if (back) {
							units[j].m_card.push_back(back);
							msg.SetCurCard(ToCardValue(back));
							MHLOG_PLAYCARD("****Desk:%d  天胡思考 pos:%d hu_card:%d", m_desk ? m_desk->GetDeskId() : 0, pos, ToTile(back));
						}
					}
#ifdef USE_NEW_TING
					MHLOG_TING("****j = %d info.m_type = %d", j, info.kType);
					// 可以听牌时将玩家的听口，胡口数据打包发送
					if (info.kType == THINK_OPERATOR_TING) // SUIJUN更改听牌逻辑
					{
						MHLOG_TING("****SUIJUN更改听牌逻辑,可以听牌时将玩家的听口，胡口数据打包发送pos=%d msg.m_ting = %d m_ting_state = %d",pos, msg.m_ting, m_tingState[i].m_ting_state);
						m_tingState[pos].m_ting_state = Ting_waitOutput;
						this->LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, units[j].ting_unit);
						MHLOG_TING("****units[j].ting_unit.size() = %d", units[j].ting_unit.size());
						for (int k = 0; k < units[j].ting_unit.size(); k++)
						{
							const TingUnit & tu = units[j].ting_unit[k];
							MHLOG_TING("****听出口k:%d, m_out_card: %d", k, ToTile(tu.out_card));
							for (int m = 0; m < tu.hu_units.size(); m++)
							{
								MHLOG_TING("*******胡口m:%d, hu_units[m].hu_card: %d", m, ToTile(tu.hu_units[m].hu_card));
							}
						}

						//扣点胡口需要加入耗子牌
						if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
						{	
							Extend_TingChuKou_Haozi(msg);
						}
						
					}
#endif
					VecExtend(info.kCard, units[j].m_card);				 
					//扣点胡口需要加入耗子牌
					msg.kThink.push_back(info);
				}
				//不报听的玩法添加查听功能（**Ren 2017-12-27）（暂时不启用）
				/*ThinkVec &check_ting_units = m_checkTingInfo[pos].m_thinkData;
				for (Lsize j = 0; j < check_ting_units.size() && !(FEAT_BaoTing || m_playtype.b_baoTing); ++j)
				{
					msg.kCheckTing = 1;
					this->LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, check_ting_units[j].ting_unit);
				}*/
			}
			notify_user(msg, i);
		}
	}


	// 听后摸牌
	void GetCard_AfterTing(Lint position, bool need_getCard = true) {
		if (m_desk == NULL) {
			LLOG_DEBUG("GetCard_AfterTing NULL ERROR ");
			return;
		}
		if (!is_pos_valid(position)) {
			LLOG_ERROR("Desk::GetCard_AfterTing pos error!");
			return;
		}
		// 忻州扣点：剩余12张牌 (**Ren 2017-11-22)
		if (XinZhouKouDian == FEAT_GAME_TYPE && (m_deskCard.size() <= 12 && need_getCard))
		{
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		// 荒庄
		if (m_deskCard.empty() && need_getCard) {
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		m_curPos = position;
		m_thinkInfo[position].m_thinkData.clear();
		set_play_state(DESK_PLAY_GET_CARD);
		m_needGetCard = false;
		if (need_getCard) {
			m_needGetCard = true;
			m_curGetCard = m_deskCard.back();
			m_deskCard.pop_back();
			// 录像
			VideoAdd__pos_oper_card(position, VIDEO_OPER_GET_CARD, m_curGetCard);

			std::vector<TingUnit> &units = m_tingState[position].ting_unit;
			for (size_t x = 0; x < units.size(); x++) {
				if (!units[x].out_card) units[x].out_card = m_curGetCard;
			}

			CheckGetCardOpeartorAfterTing(position);

			VideoThink(position);
			add_hand_card(position, m_curGetCard);
		}
		MHLOG_TING("**********听口逻辑 代码走到了这里，逻辑错误GetCard_AfterTing******************* pos = %d", position);
		m_tingState[position].m_ting_state = Ting_waitOutput;

		for (Lint i = 0; i < m_player_count; ++i) {
			LMsgS2COutCard msg;
			msg.kTime = 15;
			msg.kPos = position;
			msg.kDCount = (Lint)m_deskCard.size();
			msg.kTing = 1;
			msg.kFlag = 1;
			if (m_needGetCard) {
				msg.kFlag = 0;
			}

			if (position == i) {
				int pos = position;
				if (m_needGetCard) {
					msg.SetCurCard(ToCardValue(m_curGetCard));
					//msg.m_tingCards.push_back(msg.GetCurCard());
					m_tingState[pos].m_out_card = m_curGetCard; // 不知道前面设置没，这里多设置一次
					LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
					//扣点胡口需要加入耗子牌
					if (KouDian == FEAT_GAME_TYPE ||(TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
					{
						Extend_TingChuKou_Haozi(msg);
					}

					ThinkVec const &units = m_thinkInfo[position].m_thinkData;
					for (Lsize j = 0; j < units.size(); ++j) {
						ThinkData info;
						info.kType = units[j].m_type;
						VecExtend(info.kCard, units[j].m_card);
						msg.kThink.push_back(info);
					}
				} else {
					m_tingState[pos].m_out_card = m_curGetCard; // 不知道前面设置没，这里多设置一次
					LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
					//扣点胡口需要加入耗子牌
					if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
					{
						Extend_TingChuKou_Haozi(msg);
					}
				}
			}
			notify_user(msg, i);
		}
	}


	void ThinkEnd(Lint hu_position = INVAILD_POS, Lint ting_position = INVAILD_POS)
	{
		if (!m_desk)
			return;
		for (int i = 0; i < m_player_count; i++)
		{
			if (m_thinkInfo[i].NeedThink())
			{
				VideoDoing(99, i, 0, 0);
			}
			m_thinkInfo[i].Reset();
		}
		if (m_GangThink == GangThink_qianggang)
		{
			m_GangThink = GangThink_gangshangpao;
		}
		else if (m_GangThink == GangThink_gangshangpao)
		{
			m_GangThink = GangThink_over;
		}
		//有人胡
		if (hu_position != INVAILD_POS)
		{
			Card* winCard = m_thinkRet[hu_position].m_card.empty() ? NULL : m_thinkRet[hu_position].m_card.front();
			finish_round(WIN_BOMB, hu_position, m_beforePos, winCard);
			return;
		}

		//有人听
		if (ting_position != INVAILD_POS)
		{
			//m_louHuCard[ting_position] = 0;
			notify_desk_passive_op(ting_position);
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			return;
		}

		Lint pengPos = INVAILD_POS;
		Lint gangPos = INVAILD_POS;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)
			{
				gangPos = i;
				m_minggang[gangPos] += 1;
			}
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)
			{
				pengPos = i;
			}
		}

		//杠（补杠、点杠）
		// 这里只有点杠吧？自摸杠在 HanderUserPlayCard
		if (gangPos != INVAILD_POS)
		{
			//m_louHuCard[gangPos] = 0;
			ThinkUnit gangUnitRet;
			notify_desk_passive_op(gangPos);
			erase_suo_card(gangPos, m_thinkRet[gangPos].m_card[0], 3);

			//录像
			VideoAdd__pos_oper_card_n(gangPos, VIDEO_OPER_GANG, m_curOutCard, 4);
			for (int i = 0; i < 4; i++)
			{
				m_minggangCard[gangPos].push_back(m_curOutCard);
			}
			add_gang_item(gangPos, m_curOutCard, m_beforePos, false); // 非碰点杠
			MHLOG_PLAYCARD("****Desk:%d 玩家明杠pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, gangPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());

			m_diangang[gangPos] += 1;
			m_adiangang[m_beforePos] += 1;
			//添加杠上开花功能，修改杠后摸牌标记为明杠后摸牌（**Ren 2017-12-11）
			m_getChardFromMAGang[gangPos] = 2;

			//洪洞王牌需要使用思考單元修正听后杠胡口， For all type
			//if (FEAT_GAME_TYPE == HongTongWangPai) 
			{
				gangUnitRet = m_thinkRet[gangPos];
			}
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			//给玩家摸一张牌
			m_GangThink = GangThink_gangshangpao;

			if (m_tingState[gangPos].m_ting_state == Ting_waitHu)
			{	
				//洪洞王牌修正听后杠胡口, for all type
				//if (FEAT_GAME_TYPE == HongTongWangPai) 
				{
					MHLOG("************洪洞王牌修正听后杠胡口FEAT_GAME_TYPE == HongTongWangPai**********************");
					if (gangUnitRet.m_type == THINK_OPERATOR_MGANG && gangUnitRet.ting_unit.size() > 0 && gangUnitRet.ting_unit[0].hu_units.size() > 0)
					{
						m_tingState[gangPos].hu_units.clear();
						m_tingState[gangPos].hu_units = gangUnitRet.ting_unit[0].hu_units;
					}
				}
				SetPlayIng(gangPos, true, false, true);
			}
			else {
				SetPlayIng(gangPos, true, true, true);
			}
			return;
		}

		if (pengPos != INVAILD_POS)
		{
			// pengPos是抢牌的人
			//m_louHuCard[pengPos] = 0;
			notify_desk_passive_op(pengPos);
			erase_suo_card(pengPos, m_curOutCard, 2);
			// 对于抢牌人手中的牌，删掉2张同样的，插入3张一样的到m_knowCardCount
			InsertIntoKnowCard(m_curOutCard, 3);

			//录像
			VideoAdd__pos_oper_card_n(pengPos, VIDEO_OPER_PENG_CARD, m_curOutCard, 3);
			for (int i = 0; i < 3; i++)
			{
				m_pengCard[pengPos].push_back(m_curOutCard);
			}

			// 扣点推到胡:如果玩家手中仍然有当前碰的牌，记录下来，以后都不能再杠
			if (VecHas(m_handCard[pengPos], ToCard(m_curOutCard)))
			{
				m_Cannot_gangCard_MH[pengPos].push_back(m_curOutCard);
				mGameInfo.m_cards_guo_gang_MH.push_back(ToCard(m_curOutCard));
				MHLOG_PLAYCARD("****Desk:%d 玩家有杠碰牌 pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			// churunmin: m_beforePos应该就是先前出牌的那个人
			if (m_beforePos != INVAILD_POS) {
				m_ps[pengPos].pengs.add(PengItem(CardToTile(m_curOutCard), m_beforePos));
				MHLOG_PLAYCARD("****Desk:%d 玩家碰牌 pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			for (int i = 0; i <m_player_count; ++i) {
				m_thinkRet[i].Clear();
			}

			//碰完打一张牌
			m_curGetCard = NULL;
			SetPlayIng__Options opt;
			opt.pengPos = pengPos;
			SetPlayIng__opt(pengPos, opt);
			m_needGetCard = true;
			return;
		}

		//这里没有人操作
		if (m_beforeType == THINK_OPERATOR_MGANG)
		{
			//录像
			//int video_oper = m_beforeType == THINK_OPERATOR_MGANG ? VIDEO_OPER_MING_GANG : VIDEO_Oper_Ming1GangTing;
			int video_oper = VIDEO_OPER_MING_GANG;
			VideoAdd__pos_oper_card_n(m_beforePos, video_oper, m_curOutCard, 4);

			m_minggangCard[m_beforePos].push_back(m_curOutCard);
			CardVector::iterator it = m_pengCard[m_beforePos].begin();
			for (; it != m_pengCard[m_beforePos].end(); it += 3)
			{
				if (gCardMgrSx.IsSame(m_curOutCard, *it))
				{
					m_minggangCard[m_beforePos].insert(m_minggangCard[m_beforePos].end(), it, it + 3);
					m_pengCard[m_beforePos].erase(it, it + 3);
					break;
				}
			}
			PengItem const *peng_item = m_ps[m_beforePos].pengs.find_ptr(CardToTile((m_curOutCard)));
			if (peng_item) { // 安全检测
				add_gang_item(m_beforePos, m_curOutCard, peng_item->fire_pos, true); // 碰的自摸杠
				// 实际上还分两种情况，一种是自己新摸到一张，一种是自己以前就有那张
				MHLOG_PLAYCARD("****Desk:%d 玩家摸牌杠pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, m_beforePos, peng_item->fire_pos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}
			//修改明杠开花逻辑(**Ren2018-01-08)
			m_getChardFromMAGang[m_beforePos] = 2;
			//这里处理 明杠 明补发消息
			notify_desk_BuGang(m_beforePos, m_beforeType);

			m_minggang[m_beforePos] += 1;

			if (m_beforeType == THINK_OPERATOR_MGANG)
				// 调试发现，这时 gangPos 是4，所以pvs提示这里数组可能越界是对的
				//SetPlayIng(m_beforePos, true, m_tingState[gangPos].m_ting_state == Ting_Null, true);
				SetPlayIng(m_beforePos, true, m_tingState[m_beforePos].m_ting_state == Ting_Null, true);
			else
				GetCard_AfterTing(m_beforePos);
		}
		else
		{
			m_outCard[m_beforePos].push_back(m_curOutCard);
			InsertIntoKnowCard(m_curOutCard, 1);
			//m_louHuCard[GetNextPos(m_beforePos)] = 0;
			SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		}
	}

	void SetThinkIng()
	{
		bool think = false;
		if (!m_desk)
			return;
		MHLOG_PLAYCARD("****Desk:%d 玩家出牌 pos:%d, card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, m_curPos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
		// 首先清空桌子上所有玩家的思考状态
		for (int i = 0; i < m_player_count; ++i) {
			m_thinkRet[i].Clear();
			m_thinkInfo[i].Reset();
		}

		// 按逆时针顺序从当前出牌玩家的下家开始遍历，不包括当前出牌玩家
		int origin = m_curPos;
		for (int d = 1; d < m_player_count; ++d) {
			int i = (origin + d) % m_player_count;

			if (m_tingState[i].m_ting_state == Ting_waitHu) {
				CheckOutCardOpeartorAfterTing(i);
			} else {
				CheckOutCardOperator(i);
			}

			if (m_thinkInfo[i].NeedThink()) {
				think = true;
				VideoThink(i);
			}

			// 不开启一炮多响时：按逆时针顺序从当前出牌玩家的下家开始遍历，如果遇到可胡的，就不管剩下的了
			// 这个效果就是按优先级顺序，一个一个的问该胡了你胡不胡		 
			if (!has_YiPaoDuoXiang() && m_thinkInfo[i].HasHu()) {
				//break;
			}
		}

		if (think) {
			set_play_state(DESK_PLAY_THINK_CARD);
			SendThinkInfoToClient();
		} else {
			ThinkEnd();
		}
	}

	void SendThinkInfoToClient() {
		for (Lint pos = 0; pos < m_player_count; ++pos) {
			// 这个54号消息不能用来发送听按钮，得用51号消息 LMsgS2COutCard MSG_S_2_C_GET_CARD
			// 这个消息用来发送其他的按钮吧？杠、碰？
			LMsgS2CThink think;
			think.m_time = 15;
			think.m_card = ToCardValue(m_curOutCard);
			if (m_thinkInfo[pos].NeedThink()) {
				think.m_flag = 1;
				ThinkVec const &units = m_thinkInfo[pos].m_thinkData;
				for (Lsize j = 0; j < units.size(); ++j) {
					ThinkData info;
					info.kType = units[j].m_type;
					VecExtend(info.kCard, units[j].m_card);
					think.m_think.push_back(info);
				}
			} else {
				think.m_flag = 0;
			}
			notify_user(think, pos);
		}
	}

	// 让桌面上好几个人选择动作，现在有一个人选择了
	// m_thinkRet 是玩家选中了某个动作
	// m_thinkInfo 是玩家可选哪些动作
	void CheckThink()
	{
		if (!m_desk)
			return;
		bool hu = false;
		bool Peng = false;
		bool Gang = false;
		bool Ting = false;

		bool hu_New = false;
		bool Peng_New = false;
		bool Gang_New = false;
		bool Ting_New = false;

		Lint Hu_most_value_position = INVAILD_POS;
		Lint Ting_most_value_position = INVAILD_POS;

		// 这块检测已经做出动作选择的玩家
		for (Lint i = 0; i < m_player_count; ++i) {
			int pos = i;
			if (m_beforePos != INVAILD_POS)
			{
				pos = (m_beforePos - i) < 0 ? (m_beforePos - i + m_player_count) : m_beforePos - i;
			}
			// 相当于从 m_beforePos 开始顺时针遍历玩家，正常走牌是逆时针顺序
			// 如果没有 m_beforePos 呢，那相当于逆时针遍历玩家

			// 被遍历顺序影响，选取一个优先胡的玩家
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB) Hu_most_value_position = pos;

			// 检测出有没有人选取这些动作
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB)			hu = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_PENG)		Peng = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_MGANG)	Gang = true;
		}

		// 这块检测尚未做出动作选择的玩家
		for (Lint i = 0; i < m_player_count; ++i) {
			if (!m_thinkInfo[i].NeedThink()) continue;
			if (m_thinkInfo[i].HasHu()) {
				hu_New = true;
				if (hu) {
					// 选取到m_beforePos的逆时针距离最短的那个
					if (has_YiPaoDuoXiang() || CheckPositionPrior(m_beforePos, i, Hu_most_value_position) == i) {
						hu_New = true;
						break;
					} else {
						hu_New = false;
					}
				}
			} else if (m_thinkInfo[i].HasTing() && !Ting_New) {
				Ting_New = true;
				if (Ting) {
					if (CheckPositionPrior(m_beforePos, i, Ting_most_value_position) == i) {
						Ting_New = true;
					} else {
						Ting_New = false;
					}
				}
			}
			else if (m_thinkInfo[i].HasPeng())		Peng_New = true;
			else if (m_thinkInfo[i].HasMGang())		Gang_New = true;
		}

		bool think = false;

		if (hu_New) think = true;
		else {
			if (!hu)//无人胡
			{
				if (Ting_New)
				{
					think = true;
				}
				else {
					if (!Ting)
					{
						if (Peng_New || Gang_New)
							think = true;
					}
				}
			}
		}

		if (!think) ThinkEnd(Hu_most_value_position, Ting_most_value_position);
	}







	void send_active_op(ActivePosOp op) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = op.pos;
		sendMsg.m_card.kType = op.code;
		sendMsg.m_card.kCard.push_back(ToCardValue(op.tile));
		notify_desk(sendMsg);
	}

	void send_effect_ChuPai_in_TingKou(ActivePosOp op) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = op.pos;
		sendMsg.m_card.kType = op.code;
		sendMsg.m_card.kCard.push_back(ToCardValue(op.tile));
		sendMsg.m_flag = 5;
		notify_desk(sendMsg);
	}

	/*************************************************************************************************************
	* 描述：明杠开花胡广播给桌子
	*/
	void send_effect_MingGangHua(ActivePosOp op) {
		LMsgS2CUserOper send;
		send.m_pos = op.pos;
		send.m_errorCode = 0;
		send.m_think.kType = op.code;
		send.m_think.kCard.push_back(ToCardValue(op.tile));
		send.m_card = ToCardValue(op.tile);
		send.m_hu.push_back(VecFront(m_thinkRet[op.pos].m_hu).m_hu_type);
		CardVector &hand_cards = m_handCard[op.pos];
		send.m_cardCount = hand_cards.size();
		VecExtend(send.m_cardValue, hand_cards);
		notify_desk(send);
	}

	void send_effect_ZiMoHu(ActivePosOp op, tile_t winning_tile, std::vector<Hu_type> const &hu_types) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = op.pos;
		sendMsg.m_card.kType = op.code;
		sendMsg.m_card.kCard.push_back(ToCardValue(op.tile));

		sendMsg.m_huCard = ToCardValue(winning_tile);

		for (std::vector<Hu_type>::const_iterator p = hu_types.begin(); p != hu_types.end(); ++p) {
			sendMsg.m_hu.push_back(p->m_hu_type);
		}

		CardVector &hand_cards = m_handCard[op.pos];
		sendMsg.m_cardCount = hand_cards.size();
		VecExtend(sendMsg.m_cardValue, hand_cards);

		notify_desk(sendMsg);
	}

	void active_op_process_ZiMoHu(ActivePosOp op, ThinkUnit *unit) {
		int pos = op.pos;
		// 录像：记录玩家的主动操作
		VideoAdd__doing(op);
		// 广播给桌子
		if (NianZhongZi == FEAT_GAME_TYPE && 2 == m_getChardFromMAGang[op.pos])
		{
			//撵中子：明杠杠上开花不算自摸 （**Ren）
			//胡牌一瞬间发送给客户端胡了的消息，而不是显示自摸（撵中子）
			send_effect_MingGangHua(op);  
		}
		else
		{
			send_effect_ZiMoHu(op, ToTile(m_curGetCard), unit->m_hu);
		}

		if (m_curGetCard) {
			erase_hand_card(pos, m_curGetCard);
		}

		// 这个位置0的 ThinkUnit 一定是自摸胡那个 ThinkUnit 吗？
		m_thinkRet[m_curPos] = m_thinkInfo[m_curPos].m_thinkData[0];
		m_winConfirmed.set(m_curPos);
		finish_round(WIN_ZIMO, m_curPos, INVAILD_POS, m_curGetCard);
	}

	static void LMsgS2COutCard_add_TingChuKouUnit(LMsgS2COutCard &msg, Card *out_card, std::vector<HuUnit> const &hu_units) {
		msg.kTingCards.push_back(ToCardValue(out_card));
		// 点击“听”按钮后，以前只是要禁用不可出的牌高亮可出的牌
		// 现在新增一个需求：长按某个可出的牌，显示它对应的听口
		TingChuKouUnit &chu_unit = VecAdd(msg.kTingChuKou);
		// 填入出的哪张牌
		chu_unit.kOutTile = ToTile(out_card);
		// 把听口转换过来
		for (size_t k = 0; k < hu_units.size(); ++k) {
			tile_t hu_tile = ToTile(hu_units[k].hu_card);
			if (!hu_tile) continue; // 安全检测
			chu_unit.kTingKou.push_back(hu_tile);			
		}

		//扣点发出的消息中需要加入耗子牌
		//if(KouDian == FEAT_GAME_TYPE)
		//for (size_t m = 0; m < mGameInfo.m_hCards.size(); m++)
		//{
		//}


	}

	static void LMsgS2COutCard_setup_TingChuKou_waitOutput(LMsgS2COutCard &msg, std::vector<TingUnit> const &ting_units) {
		if (ting_units.empty()) return;

		for (size_t j = 0; j < ting_units.size(); ++j) {
			Card *out_card = ting_units[j].out_card;
			if (NULL == out_card) continue;
			LMsgS2COutCard_add_TingChuKouUnit(msg, out_card, ting_units[j].hu_units);
		}
	}



	static void LMsgS2COutCard_setup_TingChuKou(LMsgS2COutCard &msg, TingState &ting_state) {
		if (ting_state.m_ting_state == Ting_waitOutput) {
			std::vector<TingUnit> const &ting_units = ting_state.ting_unit;
			LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, ting_units);
		} else if (ting_state.m_ting_state == Ting_waitHu) {
			Card *out_card = ting_state.m_out_card;
			if (out_card) {
				LMsgS2COutCard_add_TingChuKouUnit(msg, out_card, ting_state.hu_units);
			}
		}
	}

	void send_effect_TingKou(ActivePosOp op) {
		int pos = op.pos;
		LMsgS2COutCard msg;
		msg.kTime = 15;
		msg.kPos = pos;
		msg.kDCount = (Lint)m_deskCard.size();
		msg.kTing = 1;
		msg.kFlag = 1;

		LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
		//扣点胡口需要加入耗子牌
		if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
		{
			Extend_TingChuKou_Haozi(msg);
		}

		notify_user(msg, pos);
	}

	Lstring format_cards_list(const std::vector<HuUnit> & units)
	{
		std::ostringstream ss;
		size_t size = units.size();
		for (int i = 0; i < size; i++)
		{
			if (i < size - 1)
			{
				ss << (int)ToTile(units[i].hu_card) << ',';
			}
			else
				ss << (int)ToTile(units[i].hu_card);
		}
		return ss.str();
	}

	// 替换原来的听牌逻辑，原来的听只是发送听口，这里用户已经确认听并打出了听口出的牌
	// 这里客户端连续收到2个消息，调试时需要注意
	void active_op_process_Ting_new_version(ActivePosOp op, ThinkUnit * unit)
	{
		MHLOG_PLAYCARD("****Desk:%d 玩家听牌 pos:%d out_card:%d*****",m_desk->GetDeskId(), op.pos, op.tile);
		MHLOG_TING("***听牌逻辑 op.pos = %d, op.tile = %d, op.code = %d", op.pos, op.tile, op.code);	
		if (NULL == unit)
		{
			LLOG_ERROR("Error can not find user think unit in think list. out_card:%d", op.tile);
			return;
		}
		// 校验客户端参数：出的牌是空的肯定不对
		if (op.tile == 0) return;
		//VideoAdd__doing(op);  //添加录像中报听录像（**Ren 2018-01-23）
		CardValue out_card_value = ToCardValue(op.tile);
		size_t i = VecGetPos(unit->ting_unit, out_card_value);
		if (i == -1)
		{
			MHLOG_TING("***错误，找不到对应的听口 unit->ting_unit.size() = %d", unit->ting_unit.size());
			return;
		}		

		// 广播给桌子
		send_active_op(op);
        int pos = op.pos;

		m_tingState[pos].ting_unit = unit->ting_unit; //保存听口，复制数组
		m_curPos = pos;
		m_thinkInfo[pos].m_thinkData.clear();		

		TingUnit const &ting_unit = m_tingState[pos].ting_unit[i];
		m_curOutCard = ting_unit.out_card;

		MHLOG_TING("***找到听单元 i = %d, 出牌ting_unit.out_card = %d", i, ToTile(ting_unit.out_card));
		for (int j = 0; j < ting_unit.hu_units.size(); j++)
		{
			MHLOG_TING("***找到胡牌 j = %d, ting_unit.hu_units[j].hu_card = %d", j, ToTile(ting_unit.hu_units[j].hu_card));
		}
		MHLOG_PLAYCARD("****Desk:%d 玩家听牌 pos:%d out_card:%d hu_cards: %s*****", m_desk->GetDeskId(), op.pos, op.tile, format_cards_list(ting_unit.hu_units).c_str());
		// 出牌
		active_op_process_ChuPai__common(pos);		
		ActivePosOp out_card_op(pos, THINK_OPERATOR_OUT, op.tile);
		active_op_process_ChuPai__in_TingKou(out_card_op, ting_unit); // 保存胡口，清除听口
	}

	// 发送听口之前询问用户是否要胡三元牌型，清除其他非三元的听口，并且记录用户的选择，胡牌的时候再
	// 进行决策
	void active_op_process_Ting(ActivePosOp op, ThinkUnit *unit) {

		MHLOG_TING("SUIJUN修改听牌逻辑，这里的unit就是用户听出口打出的牌，并且确认了听!");
		int pos = op.pos;
		// 广播给桌子
		send_active_op(op);

		m_tingState[pos].ting_unit = unit->ting_unit;
		m_curPos = pos;
		m_thinkInfo[pos].m_thinkData.clear();
		set_play_state(DESK_PLAY_GET_CARD);
		m_needGetCard = false;
		m_tingState[pos].m_ting_state = Ting_waitOutput;

		// 把听口发给这个玩家
		send_effect_TingKou(op);
		LTime cur;
		m_tingTimeEnd[pos] = cur.MSecs();
		LLOG_ERROR("***Ting time is %ld ms, user pos = %d", m_tingTimeEnd[pos] - m_tingTimeBegin[pos], pos);
		
	}

	void active_op_process_AnGang(ActivePosOp op, ThinkUnit *unit) {
		int pos = op.pos;
		// 录像：记录玩家的主动操作
		VideoAdd__doing(op);
		// 广播给桌子
		if (m_playtype[PT_AnGangKeJian] && HongTongWangPai == FEAT_GAME_TYPE)
		{
			send_active_op(op);
			//notify_desk_BuGang(pos, THINK_OPERATOR_MGANG);
		}
		else 
			send_active_op(op);
		

		erase_suo_card(pos, unit->m_card[0], 4);
		m_angang[pos] += 1;
		m_curOutCard = unit->m_card[0];
		// 录像
		VideoAdd__pos_oper_card_n(pos, VIDEO_OPER_AN_GANG, unit->m_card[0], 4);

		for (int i = 0; i < 4; i++) {
			m_angangCard[pos].push_back(unit->m_card[0]);
		}
		add_gang_item(pos, unit->m_card[0], INVAILD_POS, false); // 非碰自摸杠
		MHLOG_PLAYCARD("****Desk:%d 玩家暗杠 pos: %d, card: %d [%s]", m_desk ? m_desk->GetDeskId() : 0, pos, ToTile(unit->m_card[0]), unit->m_card[0]->ToString().c_str());
		m_beforeType = THINK_OPERATOR_AGANG;
		//添加刚上开花支持：修改杠后摸牌标记为暗杠后摸牌（**Ren 2017-12-11）
		m_getChardFromMAGang[pos] = 1;

		if (m_tingState[pos].m_ting_state == Ting_waitOutput) {
#ifndef USE_NEW_TING
			GetCard_AfterTing(pos, true);
#else
			//SUIJUN修改听牌逻辑，重新计算听
			MHLOG_TING("***听牌逻辑，暗杠后重新思考，不能走听后逻辑!");
			SetPlayIng(pos, true, true, true);
#endif // !USE_NEW_TING

		}
		else if (m_tingState[pos].m_ting_state == Ting_waitHu) {
			//洪洞王牌修正听后杠胡口，for all type
			//if (FEAT_GAME_TYPE == HongTongWangPai) 
			{				
				if (unit->ting_unit.size() > 0)
				{
					m_tingState[pos].hu_units.clear();
					m_tingState[pos].hu_units = unit->ting_unit[0].hu_units;
				}
			}
			SetPlayIng(pos, true, false, true);
		} 
		else {
			SetPlayIng(pos, true, true, true);
		}
	}

	void active_op_process_BuGang(ActivePosOp op, ThinkUnit *unit) {
		int pos = op.pos;
		m_curOutCard = unit->m_card[0];
		erase_hand_card(pos, m_curOutCard);
		//这里玩家思考
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_MGANG;
		m_GangThink = GangThink_qianggang;

		// 抢杠胡算收炮，如果开启了选项，就在碰的自摸杠点了之后，没杠成功之前，给可胡玩家点“胡”和“过”的按钮
		// 那个玩家就选胡就胡了，这个杠失败，而且算点炮
		// 那个玩家选过，杠就成功了
		// 直接调用ThinkEnd()基本相当于那个玩家点过
		if (has_QiangGangHu())
		{
			SetThinkIng();
		}
		else
		{
			//添加刚上开花标记，修改杠后摸牌为明杠后摸牌（**Ren 2017-12-11）
			m_getChardFromMAGang[pos] = 2;		
			ThinkEnd();
		}
	}

	// 客户端传递来的参数要进行校验，这个校验根据听的状态不同，校验方法也不同，校验后都走这里
	void active_op_process_ChuPai__common(int pos) {
		// 任意人出牌就清空这个决定标记
		m_decideGuoHu.clear();
		mGameInfo.m_playerPos = pos; //保存出牌玩家位置

		if (m_thinkInfo[pos].HasHu()) {
			m_louZimohu[pos] = true;
		}
		// 忻州扣点：出牌后就相当于过了自己了，把过碰记录清空（**Ren:2017-11-25）
		if (XinZhouKouDian == FEAT_GAME_TYPE)
		{
			m_guoPengCard[pos].clear();
		}
		// churunmin: 这个断点是在出牌请求之后，出牌界面显示之前
		// 自己出牌后，清掉自己的漏胡标记，这样就可以收炮了
		// 忻州扣点：过胡了就不能再胡了,m_louHuCard[pos]不会置零  （**Ren:2017-11-22）
		if (m_playtype[PT_GuoHuZhiKeZiMo] || XinZhouKouDian == FEAT_GAME_TYPE) {
			// 如果是选中了“过胡只可自摸”选项，那么就给始终保留着漏胡标记
			// 不在出牌时清除漏胡标记，这样这个漏胡的玩家就一直不能收炮了
		} else {
			m_louHuCard[pos] = 0;
		}

		// 贴金：累计上金
		if (IsHaozipai(m_curOutCard)) {
			++m_ps[pos].shangjin;
		}

		if (m_thinkInfo[pos].NeedThink()) {
			VideoDoing(99, pos, 0, 0);
		}
	}

	void active_op_process_ChuPai__before_Ting(ActivePosOp op) {
		int pos = op.pos;
		// 广播给桌子
		send_active_op(op);

		Lint size = m_handCard[pos].size();
		erase_hand_card(pos, m_curOutCard);
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		// 录像
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);
		// 这里玩家思考
		SetThinkIng();
	}

	void notify_YingkouThink(ActivePosOp op)
	{
		int pos = op.pos;
		if (pos != m_curPos)
		{
			LLOG_ERROR("Error think  pos %d", pos);
		}
		// 设置用户开始硬扣思考
		m_bHongTongYingKou.clear(); //默认不硬扣
		m_bInYingKouThink.set(pos);
		ThinkTool &tt = m_thinkInfoForYingKou[pos];
		tt.Reset();
		ThinkUnit tUnit1, tUnit2;
		tUnit1.m_type = THINK_OPERATOR_HONGTONG_YINGKOU;
		tUnit1.m_card.push_back(m_curOutCard);
		tt.m_thinkData.push_back(tUnit1);

		tUnit2.m_type = THINK_OPERATOR_HONGTONG_YINGKOU_NOT;
		tUnit2.m_card.push_back(m_curOutCard);
		tt.m_thinkData.push_back(tUnit2);
		LMsgS2CThink think;
		think.m_time = 15;
		think.m_card = ToCardValue(m_curOutCard);
		if (tt.NeedThink()) {
			think.m_flag = 1;
			ThinkVec const &units = tt.m_thinkData;

			for (Lsize j = 0; j < units.size(); ++j) {
				ThinkData info;
				info.kType = units[j].m_type;
				VecExtend(info.kCard, units[j].m_card);
				think.m_think.push_back(info);
			}
		}
		else {
			think.m_flag = 0;
		}
		notify_user(think, op.pos);

		MHLOG("*******************发送硬扣的指令给客户端 Think think.m_card = %d m_think.size=%d", think.m_card, think.m_think.size());
		for (int i = 0; i < think.m_think.size(); i++)
		{
			ThinkData & tData = think.m_think[i];
			MHLOG("******************ThinkData i =%d m_think[i].m_type=%d m_think[i].m_card.size() = %d", i, tData.m_type, tData.m_card.size());
			for (int j = 0; j < tData.kCard.size(); j++)
			{
				MHLOG(" ******************** i = %d, j =%d Card: %d ", i, j, ToTile(tData.m_card[j]));
			}
		}
	}

	//洪洞王牌  这个函数要被截断，等待客户端发送硬扣
	void active_op_process_ChuPai__in_TingKou_ForHongTong(ActivePosOp op, TingUnit const &unit) {

		MHLOG("********************** 洪洞王牌听口处理函数 pos = %d, tile = %d", op.pos, op.tile);
		//m_curOutCard;		

		//这里的代码需要用户回复的硬扣后继续执行
		int pos = op.pos;
		// 广播给桌子
		send_effect_ChuPai_in_TingKou(op);

		erase_suo_card(pos, m_curOutCard, 1);
		m_kouCount[pos] = m_outCard[pos].size();
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		//录像
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);

		m_tingState[pos].m_ting_state = Ting_waitHu;
		m_tingState[pos].hu_units = unit.hu_units;
		m_tingState[pos].ting_unit.clear();
		//这里玩家思考
		//SetThinkIng();
		m_outCard[m_beforePos].push_back(m_curOutCard);
		InsertIntoKnowCard(m_curOutCard, 1);


		notify_YingkouThink(op);
		//SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		

	}

	void SuijunPintfTIingKou(int pos)
	{
		MHLOG("************* 输出听口信息 pos = %d, m_tingStatus[pos] = %d", pos, m_tingState[pos]);
       // m_tin
	}

	//洪洞王牌  这个函数要被截断，等待客户端发送硬扣
	void active_op_process_ChuPai__in_TingKou(ActivePosOp op, TingUnit const &unit) {
		
		MHLOG("**********************玩家 pos = %d 在听口出牌 %d, 胡口大小=%d", op.pos, op.tile, unit.hu_units.size());
		
		if (FEAT_GAME_TYPE == HongTongWangPai)
		{
			bool bYingKouHuType = false;
			int  ting_kou_count = 0;
			for (int i = 0; i < unit.hu_units.size(); i++)
			{
				HuUnit const & h = unit.hu_units[i];
			    MHLOG("*****************能胡的牌:%d, unit.hu_units[i].fans = %s", ToTile(h.hu_card), malgo_format_fans(h.fans).c_str());	 
				 
				//return active_op_process_ChuPai__in_TingKou_ForHongTong(op, unit);				
			
				 if (h.fans & (FAN_HongTong_YingKou | FAN_HongTong_LouShangLou))
				 {
					 bYingKouHuType = true;
				 }
				 ting_kou_count += 1;				 
			}	
			MHLOG("**********************ting_kou_count = %d bYingKouHuType = %d", ting_kou_count, bYingKouHuType);
			if (ting_kou_count > 1 && bYingKouHuType)
			{
				return active_op_process_ChuPai__in_TingKou_ForHongTong(op, unit);
			}			 
		}

		int pos = op.pos;
		// 广播给桌子
		send_effect_ChuPai_in_TingKou(op);

		erase_suo_card(pos, m_curOutCard, 1);
		m_kouCount[pos] = m_outCard[pos].size();     //断线重连时访问的变量
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		//录像
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);

		m_tingState[pos].m_ting_state = Ting_waitHu;
		m_tingState[pos].hu_units = unit.hu_units;
		m_tingState[pos].ting_unit.clear();

		if (FEAT_GAME_TYPE == HongTongWangPai)
		{
			//这里玩家思考
			SetThinkIng();
		}
		else
		{
			m_outCard[m_beforePos].push_back(m_curOutCard);
			InsertIntoKnowCard(m_curOutCard, 1);
			SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		}

	}

	void active_op_process_ChuPai__after_Ting(ActivePosOp op) {
		int pos = op.pos;
		// 广播给桌子
		send_active_op(op);
		Lint size = m_handCard[pos].size();
		erase_hand_card(pos, m_curOutCard);
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		//录像
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);
		SetThinkIng();
	}

	void active_op_process_ChuPai(ActivePosOp op) {
		MHLOG_TING("****玩家出牌操作，pos = %d, card=%d", op.pos, op.tile);
		int pos = op.pos;
		// 校验客户端参数：出的牌是空的肯定不对
		if (op.tile == 0) return;
		CardValue out_card_value = ToCardValue(op.tile);

		//char buf[1024];
		//sprintf(buf, "ChuPai %d", op.tile);
		//notify_chat_test(pos, buf);

#ifdef USE_NEW_TING
		if (m_tingState[pos].m_ting_state == Ting_waitOutput) { //SUIJUN修改听牌逻辑，不会在进入这个听口出牌了
			m_tingState[pos].m_ting_state = Ting_Null;
			MHLOG_TING("****听牌逻辑玩家选择了过听pos = %d，要出的出牌 %d", pos, ToTile(out_card_value));
			size_t i = VecGetPos(m_handCard[pos], out_card_value);
			if (i != -1) {
				// 设置m_curOutCard、从手中删掉出的牌、告诉桌子这个玩家出牌了、录像、其它玩家思考
				m_curOutCard = m_handCard[pos][i];
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__before_Ting(op);
				MHLOG_TING("****听牌逻辑玩家选择了过听pos = %d，出牌成功 %d", pos, ToTile(out_card_value));
			}
		}
		else if (m_tingState[pos].m_ting_state == Ting_waitHu) {
			if (m_tingState[pos].m_out_card == out_card_value) {
				m_curOutCard = m_tingState[pos].m_out_card;
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__after_Ting(op);
			}
		}
		else {
			//SUIJUN修改听牌逻辑，玩家过听后普通出牌，这时候要清除掉听口出牌标志
			size_t i = VecGetPos(m_handCard[pos], out_card_value);
			if (i != -1) {
				// 设置m_curOutCard、从手中删掉出的牌、告诉桌子这个玩家出牌了、录像、其它玩家思考
				m_curOutCard = m_handCard[pos][i];
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__before_Ting(op);
			}
		}

#else // USE_NEW_TING		
		if (m_tingState[pos].m_ting_state == Ting_waitOutput) { //SUIJUN修改听牌逻辑，不会在进入这个听口出牌了
			size_t i = VecGetPos(m_tingState[pos].ting_unit, out_card_value);
			if (i != -1) {
				TingUnit const &unit = m_tingState[pos].ting_unit[i];
				m_curOutCard = unit.out_card;
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__in_TingKou(op, unit);
			}
		} else if (m_tingState[pos].m_ting_state == Ting_waitHu) {
			if (m_tingState[pos].m_out_card == out_card_value) {
				m_curOutCard = m_tingState[pos].m_out_card;
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__after_Ting(op);
			}
		} else {
			//SUIJUN修改听牌逻辑，玩家过听后普通出牌，这时候要清除掉听口出牌标志
			size_t i = VecGetPos(m_handCard[pos], out_card_value);
			if (i != -1) {
				// 设置m_curOutCard、从手中删掉出的牌、告诉桌子这个玩家出牌了、录像、其它玩家思考
				m_curOutCard = m_handCard[pos][i];
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__before_Ting(op);
			}
		}
#endif
	}


	void active_op_process(ActivePosOp op) {
		int pos = op.pos;
		if (op.code == THINK_OPERATOR_TING)
		{
			LTime cur;
			m_tingTimeBegin[pos] = cur.MSecs();
		}
		// 保证位置参数有效
		if (!is_pos_valid(pos)) {
			LLOG_DEBUG("active_op_process pos error: %d", pos);
			return;
		}
		// 当面桌面的状态必须是等待该玩家进行主动操作
		if (pos != m_curPos) {
			LLOG_ERROR("active_op_process not my pos: pos=%d m_curPos=%d", pos, m_curPos);
			return;
		}

		//杠上开花，如果不是摸牌后选择胡，则清空杠后摸牌的标记（**Ren 2017-12-11）
		if (THINK_OPERATOR_BOMB != op.code)
		{
			//杠后摸牌的标记清除，赋值为0（**Ren 2017-12-11）
			m_getChardFromMAGang[op.pos] = 0;
		}

		if (op.code == THINK_OPERATOR_HONGTONG_YINGKOU || op.code == THINK_OPERATOR_HONGTONG_YINGKOU_NOT)
		{
			if (THINK_OPERATOR_HONGTONG_YINGKOU == op.code)
			{
				MHLOG("****收到客户端消息THINK_OPERATOR_HONGTONG_YINGKOU tile = %d, 清除掉听口中非三元的胡口", op.tile);
				m_bHongTongYingKou.set(pos);
				std::vector<HuUnit> & units =  m_tingState[pos].hu_units;
				std::vector<HuUnit>::iterator it = units.begin();
				while ( it != units.end())
				{
					if ((*it).fans & (FAN_HongTong_LouShangLou | FAN_HongTong_YingKou))
					{
					 
						MHLOG("****保留胡口: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());
						++it;
					}
					else
					{
						MHLOG("****删除胡口: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());
						units.erase(it);
						it = units.begin();
					}					 
				}
			/*	for (it = units.begin(); it < units.end(); it++)
				{
					if ((*it).fans & (FAN_HongTong_LouShangLou | FAN_HongTong_YingKou))
					{
						MHLOG("****************保留胡口: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());
					}
					else
					{
						MHLOG("****************删除胡口: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());					
						units.erase(it);
					}
				}*/
				MHLOG("****剩下的胡口数量 count = %d ",  units.size());
				for (std::vector<HuUnit>::iterator it = units.begin(); it < units.end(); it++)
				{
					MHLOG("****胡口: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());			 
					 
				}
			}
			else if(THINK_OPERATOR_HONGTONG_YINGKOU_NOT == op.code)
			{
				MHLOG("****收到客户端消息THINK_OPERATOR_HONGTONG_YINGKOU_NOT, tile = %d", op.tile);
				m_bHongTongYingKou.reset(pos);
			}
			for (int i = 0; i < m_thinkInfoForYingKou[op.pos].m_thinkData.size(); i++)
			{
				MHLOG("****m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_type = %d m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_card.size()=%d", 
					m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_type,
				m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_card.size());				 
				
			}
			m_bInYingKouThink.reset(pos);
			SetThinkIng();
			//SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
			return;
		}

		//这里加入优化操作，如果玩家手里有杠，选择了过杠就不再提示杠
		// 暂时注释掉暗杠的检测，玩家杠不了不高兴了
		if (op.code == THINK_OPERATOR_OUT)
		{
			/*
			Card * pAnGangCard = NULL;
			if (m_thinkInfo[pos].HasAnGang(pAnGangCard))
			{
				if (pAnGangCard)
				{
					LLOG_ERROR("****这里记录玩家过杠的牌***out_tile = %d gang_card=%d", op.tile, ToTile(pAnGangCard));
					mGameInfo.m_cards_guo_gang_MH.push_back(*pAnGangCard);
				}
			}
			*/
			//忻州扣点：自摸过胡只可自摸 （** Ren 2017-11-27）
			if (m_thinkInfo[pos].HasHu() && XinZhouKouDian == FEAT_GAME_TYPE)
			{
				m_louHuCard[pos] = 1;
			}
		}

		if (op.code == THINK_OPERATOR_OUT) return active_op_process_ChuPai(op);
		// 摸牌后查找胡单元
		ThinkUnit *unit = find_think_unit(op);
		if (NULL == unit) {
			// 对于主动操作，玩家点“过”不会发指令到服务器，因为后面还会有出牌操作
			LLOG_ERROR("active_op_process invalid op: pos=%d code=%d", pos, op.code);
			return;
		}

		switch (op.code) {
		case THINK_OPERATOR_BOMB: return active_op_process_ZiMoHu(op, unit);
#ifndef USE_NEW_TING
		case THINK_OPERATOR_TING: return active_op_process_Ting(op, unit);
#else
		case THINK_OPERATOR_TING: return active_op_process_Ting_new_version(op, unit);
#endif
		case THINK_OPERATOR_AGANG: return active_op_process_AnGang(op, unit);
		case THINK_OPERATOR_MGANG: return active_op_process_BuGang(op, unit);
		case THINK_OPERATOR_HONGTONG_YINGKOU:
		case THINK_OPERATOR_HONGTONG_YINGKOU_NOT:
		{
		 
			 
		}
		break;
		}


	}



	void send_passive_op_error(PassivePosOp const &op, int err) {
		LMsgS2CUserOper sendMsg;
		sendMsg.m_pos = op.pos;
		sendMsg.m_think.kType = op.code;
		sendMsg.m_think.kCard.push_back(ToCardValue(op.tile));
		sendMsg.m_errorCode = err;
		notify_user(sendMsg, op.pos);
	}

	bool passive_op_is_valid_Guo(PassivePosOp op) {
		ThinkVec const &whitelist = m_thinkInfo[op.pos].m_thinkData;
		// 没有胡、杠、碰、吃，也就没有过
		if (whitelist.empty()) return false;
		if (op.code != THINK_OPERATOR_NULL) return false;
		return true;
	}

	/*******************************************************************************
	* 函数名称：  find_think_unit(PosOp op)
	* 描述：      查找胡单元
	* 参数：
	*  /op        别人出牌，其他玩家被动的操作
	* 返回：      ThinkUnit 胡单元
	*/
	ThinkUnit *find_think_unit(PosOp op) {
		ThinkVec &whitelist = m_thinkInfo[op.pos].m_thinkData;
		for (ThinkVec::iterator p = whitelist.begin(); p != whitelist.end(); ++p) {
			if (op.code == p->m_type) {
				// 听操作没有牌做参数，听了之后出牌是个单独的操作
				if (op.code == THINK_OPERATOR_TING)
				{
					//SUIJUN修改听牌逻辑，这里获取到用户确认TING的选择，需要带回打出的牌				 
#ifdef USE_NEW_TING
					MHLOG_TING("SUIJUN修改听牌逻辑，这里获取到用户确认TING的选择，需要带回打出的牌");
					if (VecHas(p->m_card, ToCardValue(op.tile))) // 用户打出的牌必须是系统允许的
						return &*p;
#else
					return &*p;
#endif // USE_NEW_TING
				}
				// 需要校验你想操作的这张牌是不是系统允许的那张：杠、碰、吃
				if (op.tile == ToTile(VecFront(p->m_card))) return &*p;
			}
		}
		return NULL;
	}

	bool is_pos_valid(int pos) {
		if (NULL == m_desk) return false;
		return 0 <= pos && pos < m_player_count;
	}

	void passive_op_process_Guo(PassivePosOp op) {
		int pos = op.pos;
		m_thinkRet[pos].m_type = THINK_OPERATOR_NULL;
		if (m_thinkInfo[pos].HasHu()) {
			// 该玩家决定过胡
			m_decideGuoHu.set(pos);
			// 标记该玩家漏了收炮胡
			m_louHuCard[pos] = 1;
			// 有的玩法如平鲁过胡后自己再出牌前仍然可以收炮
			if (FEAT_GuoHuChuPaiQianKeShouPao) m_louHuCard[pos] = 0;
		}
		//忻州扣点：过碰后不过自己出牌不允许再碰这张牌 （**Ren 2017-11-25）
		if (XinZhouKouDian == FEAT_GAME_TYPE && m_thinkInfo[pos].HasPeng())
		{
			m_guoPengCard[pos].push_back(m_curOutCard);
		}
	}

	void send_effect_ShouPaoHu(PassivePosOp op) {
		LMsgS2CUserOper send;
		send.m_pos = op.pos;
		send.m_errorCode = 0;
		send.m_think.kType = op.code;
		send.m_think.kCard.push_back(ToCardValue(op.tile));
		send.m_card = ToCardValue(op.tile);
		send.m_hu.push_back(VecFront(m_thinkRet[op.pos].m_hu).m_hu_type);
		CardVector &hand_cards = m_handCard[op.pos];
		send.m_cardCount = hand_cards.size();
		VecExtend(send.m_cardValue, hand_cards);
		notify_desk(send);
	}

	void passive_op_process_ShouPaoHu(PassivePosOp op) {
		send_effect_ShouPaoHu(op);
		m_winConfirmed.set(op.pos);
	}

	//新版本的代码
	void ThinkEnd_new_version(Lint hu_position = INVAILD_POS, Lint ting_position = INVAILD_POS)
	{
		if (!m_desk)
			return;
		for (int i = 0; i < m_player_count; i++)
		{
			if (m_thinkInfo[i].NeedThink())
			{
				VideoDoing(99, i, 0, 0);
			}
			m_thinkInfo[i].Reset();
		}
		if (m_GangThink == GangThink_qianggang)
		{
			m_GangThink = GangThink_gangshangpao;
		}
		else if (m_GangThink == GangThink_gangshangpao)
		{
			m_GangThink = GangThink_over;
		}
		//有人胡
		if (hu_position != INVAILD_POS)
		{
			Card* winCard = m_thinkRet[hu_position].m_card.empty() ? NULL : m_thinkRet[hu_position].m_card.front();
			PassivePosOp op(hu_position, THINK_OPERATOR_BOMB, ToTile(winCard));
			passive_op_process_ShouPaoHu(op);

			//每个确认胡位置
			if (has_YiPaoDuoXiang())
			{
				for (int i = 0; i < m_player_count; i++)
				{
					if (i == hu_position)
						continue;
					if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
					{
						Card* winCard2 = m_thinkRet[i].m_card.empty() ? NULL : m_thinkRet[i].m_card.front();
						PassivePosOp op2(i, THINK_OPERATOR_BOMB, ToTile(winCard2));
						passive_op_process_ShouPaoHu(op2);
					}
				}
			}


			finish_round(WIN_BOMB, hu_position, m_beforePos, winCard);
			return;
		}

		//有人听
		if (ting_position != INVAILD_POS)
		{
			//m_louHuCard[ting_position] = 0;
			notify_desk_passive_op(ting_position);
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			return;
		}

		Lint pengPos = INVAILD_POS;
		Lint gangPos = INVAILD_POS;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)
			{
				gangPos = i;
				m_minggang[gangPos] += 1;
			}
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)
			{
				pengPos = i;
			}
		}

		//杠（补杠、点杠）
		// 这里只有点杠吧？自摸杠在 HanderUserPlayCard
		if (gangPos != INVAILD_POS)
		{
			//m_louHuCard[gangPos] = 0;
			ThinkUnit gangUnitRet;
			notify_desk_passive_op(gangPos);
			erase_suo_card(gangPos, m_thinkRet[gangPos].m_card[0], 3);

			//录像
			VideoAdd__pos_oper_card_n(gangPos, VIDEO_OPER_GANG, m_curOutCard, 4);
			for (int i = 0; i < 4; i++)
			{
				m_minggangCard[gangPos].push_back(m_curOutCard);
			}
			add_gang_item(gangPos, m_curOutCard, m_beforePos, false); // 非碰点杠
			MHLOG_PLAYCARD("****Desk:%d 玩家明杠pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, gangPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());

			m_diangang[gangPos] += 1;
			m_adiangang[m_beforePos] += 1;
			//添加杠上开花功能，修改杠后摸牌标记为明杠后摸牌（**Ren 2017-12-11）
			m_getChardFromMAGang[gangPos] = 2;

			//洪洞王牌需要使用思考單元修正听后杠胡口， For all type
			//if (FEAT_GAME_TYPE == HongTongWangPai) 
			{
				gangUnitRet = m_thinkRet[gangPos];
			}
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			//给玩家摸一张牌
			m_GangThink = GangThink_gangshangpao;

			if (m_tingState[gangPos].m_ting_state == Ting_waitHu)
			{
				//洪洞王牌修正听后杠胡口, for all type
				//if (FEAT_GAME_TYPE == HongTongWangPai) 
				{
					MHLOG("************洪洞王牌修正听后杠胡口FEAT_GAME_TYPE == HongTongWangPai**********************");
					if (gangUnitRet.m_type == THINK_OPERATOR_MGANG && gangUnitRet.ting_unit.size() > 0 && gangUnitRet.ting_unit[0].hu_units.size() > 0)
					{
						m_tingState[gangPos].hu_units.clear();
						m_tingState[gangPos].hu_units = gangUnitRet.ting_unit[0].hu_units;
					}
				}
				SetPlayIng(gangPos, true, false, true);
			}
			else {
				SetPlayIng(gangPos, true, true, true);
			}
			return;
		}

		if (pengPos != INVAILD_POS)
		{
			// pengPos是抢牌的人
			//m_louHuCard[pengPos] = 0;
			notify_desk_passive_op(pengPos);
			erase_suo_card(pengPos, m_curOutCard, 2);
			// 对于抢牌人手中的牌，删掉2张同样的，插入3张一样的到m_knowCardCount
			InsertIntoKnowCard(m_curOutCard, 3);

			//录像
			VideoAdd__pos_oper_card_n(pengPos, VIDEO_OPER_PENG_CARD, m_curOutCard, 3);
			for (int i = 0; i < 3; i++)
			{
				m_pengCard[pengPos].push_back(m_curOutCard);
			}

			// 扣点推到胡:如果玩家手中仍然有当前碰的牌，记录下来，以后都不能再杠
			if (VecHas(m_handCard[pengPos], ToCard(m_curOutCard)))
			{
				m_Cannot_gangCard_MH[pengPos].push_back(m_curOutCard);
				mGameInfo.m_cards_guo_gang_MH.push_back(ToCard(m_curOutCard));
				MHLOG_PLAYCARD("****Desk:%d 玩家有杠碰牌 pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			// churunmin: m_beforePos应该就是先前出牌的那个人
			if (m_beforePos != INVAILD_POS) {
				m_ps[pengPos].pengs.add(PengItem(CardToTile(m_curOutCard), m_beforePos));
				MHLOG_PLAYCARD("****Desk:%d 玩家碰牌 pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			for (int i = 0; i <m_player_count; ++i) {
				m_thinkRet[i].Clear();
			}

			//碰完打一张牌
			m_curGetCard = NULL;
			SetPlayIng__Options opt;
			opt.pengPos = pengPos;
			SetPlayIng__opt(pengPos, opt);
			m_needGetCard = true;
			return;
		}

		//这里没有人操作
		if (m_beforeType == THINK_OPERATOR_MGANG)
		{
			//录像
			//int video_oper = m_beforeType == THINK_OPERATOR_MGANG ? VIDEO_OPER_MING_GANG : VIDEO_Oper_Ming1GangTing;
			int video_oper = VIDEO_OPER_MING_GANG;
			VideoAdd__pos_oper_card_n(m_beforePos, video_oper, m_curOutCard, 4);

			m_minggangCard[m_beforePos].push_back(m_curOutCard);
			CardVector::iterator it = m_pengCard[m_beforePos].begin();
			for (; it != m_pengCard[m_beforePos].end(); it += 3)
			{
				if (gCardMgrSx.IsSame(m_curOutCard, *it))
				{
					m_minggangCard[m_beforePos].insert(m_minggangCard[m_beforePos].end(), it, it + 3);
					m_pengCard[m_beforePos].erase(it, it + 3);
					break;
				}
			}
			PengItem const *peng_item = m_ps[m_beforePos].pengs.find_ptr(CardToTile((m_curOutCard)));
			if (peng_item) { // 安全检测
				add_gang_item(m_beforePos, m_curOutCard, peng_item->fire_pos, true); // 碰的自摸杠
																					 // 实际上还分两种情况，一种是自己新摸到一张，一种是自己以前就有那张
				MHLOG_PLAYCARD("****Desk:%d 玩家摸牌杠pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, m_beforePos, peng_item->fire_pos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}
			//这里处理 明杠 明补发消息
			notify_desk_BuGang(m_beforePos, m_beforeType);

			m_minggang[m_beforePos] += 1;

			if (m_beforeType == THINK_OPERATOR_MGANG)
				// 调试发现，这时 gangPos 是4，所以pvs提示这里数组可能越界是对的
				//SetPlayIng(m_beforePos, true, m_tingState[gangPos].m_ting_state == Ting_Null, true);
				SetPlayIng(m_beforePos, true, m_tingState[m_beforePos].m_ting_state == Ting_Null, true);
			else
				GetCard_AfterTing(m_beforePos);
		}
		else
		{
			m_outCard[m_beforePos].push_back(m_curOutCard);
			InsertIntoKnowCard(m_curOutCard, 1);
			//m_louHuCard[GetNextPos(m_beforePos)] = 0;
			SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		}
	}

	void CheckThink_new_version()
	{
		if (!m_desk)
			return;
		bool hu = false;
		bool Peng = false;
		bool Gang = false;
		bool Ting = false;

		bool hu_New = false;
		bool Peng_New = false;
		bool Gang_New = false;
		bool Ting_New = false;

		Lint Hu_most_value_position = INVAILD_POS;
		Lint Ting_most_value_position = INVAILD_POS;

		// 这块检测已经做出动作选择的玩家
		for (Lint i = 0; i < m_player_count; ++i) {
			int pos = i;
			if (m_beforePos != INVAILD_POS)
			{
				pos = (m_beforePos - i) < 0 ? (m_beforePos - i + m_player_count) : m_beforePos - i;
			}
			// 相当于从 m_beforePos 开始顺时针遍历玩家，正常走牌是逆时针顺序
			// 如果没有 m_beforePos 呢，那相当于逆时针遍历玩家

			// 被遍历顺序影响，选取一个优先胡的玩家
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB) Hu_most_value_position = pos;

			// 检测出有没有人选取这些动作
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB)			hu = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_PENG)		Peng = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_MGANG)	Gang = true;
		}
		LLOG_ERROR("--------has_YiPaoDuoXiang() :%d", has_YiPaoDuoXiang());
		// 这块检测尚未做出动作选择的玩家
		for (Lint i = 0; i < m_player_count; ++i) {
			if (!m_thinkInfo[i].NeedThink()) continue;
			if (m_thinkInfo[i].HasHu()) {
				hu_New = true;
				if (hu) {
					// 选取到m_beforePos的逆时针距离最短的那个
					if (has_YiPaoDuoXiang() || (CheckPositionPrior(m_beforePos, i, Hu_most_value_position) == i)) {
						hu_New = true;
						break;
					}
					else {
						hu_New = false;
					}
				}
				LLOG_ERROR("尚未选择状态 hu_New:%d", hu_New);
			}
			else if (m_thinkInfo[i].HasTing() && !Ting_New) {
				Ting_New = true;
				if (Ting) {
					if (CheckPositionPrior(m_beforePos, i, Ting_most_value_position) == i) {
						Ting_New = true;
					}
					else {
						Ting_New = false;
					}
				}
			}
			else if (m_thinkInfo[i].HasPeng())		Peng_New = true;
			else if (m_thinkInfo[i].HasMGang())		Gang_New = true;
		}

		bool think = false;
		LLOG_ERROR("这里的状态hu:%d ting:%d peng:%d gang:%d hu_New:%d ting_new:%d peng_new:%d gang_new:%d",
			hu, Ting, Peng, Gang, hu_New, Ting_New, Peng_New, Gang_New);
		if (hu_New) think = true;
		else {
			if (!hu)//无人胡,后面也没有可能再胡了
			{
				if (Ting_New) // 这里根本不会走到
				{
					think = true;
				}
				else {
					if (!Ting)
					{
						if (Peng_New || Gang_New)
							think = true;
					}
				}
			}
		}

		if (!think) ThinkEnd_new_version(Hu_most_value_position, Ting_most_value_position);
	}

	void passive_op_process_new_version(PassivePosOp op) {
		int pos = op.pos;
		// 保证位置参数有效
		if (!is_pos_valid(op.pos)) return;
		MHLOG_PLAYCARD("Desk:%d 收到玩家决策 code=%d pos=%d,tile=%d", m_desk?m_desk->GetDeskId():0, op.code, op.pos, op.tile);
		//洪洞王牌选王操作(**Ren 2017-12-12)
		if (HongTongWangPai == FEAT_GAME_TYPE &&
			m_playtype[MH_PT_HongTongWangPai_ShuangWang] &&
			THINK_OPERATOR_HONGTONG_XUANWANG == op.code &&
			!is_invalid_card(op.tile))
		{
			m_hongTongWangPai_ShuangWang[op.pos] = TileToCard(op.tile);
			//TODO：是否需要添加返回给客户端选王操作结果的消息
			return;
		}

		// 对于玩家点的“过”按钮需要进行特殊识别特殊处理
		if (passive_op_is_valid_Guo(op)) {
			MHLOG_PLAYCARD("Desk:%d 收到玩家过的选择 pos=%d,tile=%d", m_desk ? m_desk->GetDeskId():0, op.pos, op.tile);
			// 玩家点的“过”按钮			 
			passive_op_process_Guo(op);
		}
		else {
			// 每个玩家当前都有一个 ThinkUnit 列表，现在识别出玩家选了哪个
			// 别人出牌检查自己胡单元
			ThinkUnit *unit = find_think_unit(op);
			if (NULL == unit) {
				// 比如吃和碰，吃优先，一个用户点了吃，桌面状态就变了，另一个用户发来的碰请求就有可能跑到这个分支
				// 玩家用外挂乱发指令也会跑到这个分支
				// 似乎不应该返回错误码，应该直接忽略？
				return send_passive_op_error(op, 1);
			}
			// 标记这个玩家的选择，后面会汇总分析
			m_thinkRet[op.pos] = *unit;
		}

		// 收炮直接就显示这个人胡了，似乎不太对啊
		// 比如两个人可以收炮，一个次优先级的先选收炮，一个高优先级的后选收炮，次优先级的不成功却显示胡的特效
		//if (op.code == THINK_OPERATOR_BOMB) passive_op_process_ShouPaoHu(op);
		// 录像：别人出牌后，该玩家点了哪个按钮
		//VideoAdd__doing(op);
		// 清空该玩家的可选操作，表示该玩家已经做过选择了
		m_thinkInfo[op.pos].Reset();
		CheckThink_new_version();

		//if (!has_YiPaoDuoXiang() && m_decideGuoHu[pos]) {
		// 这个人选择的过胡，得重新发起思考
		//	SetThinkIng();
		//} else {
		// 汇总分析
		//	CheckThink();
		//}
	}

	void passive_op_process(PassivePosOp op) {
		
		return passive_op_process_new_version(op);
		int pos = op.pos;
		// 保证位置参数有效
		if (!is_pos_valid(op.pos)) return;

		//洪洞王牌选王操作(**Ren 2017-12-12) 暂时注释掉（2018-01-08）
		//if (HongTongWangPai == FEAT_GAME_TYPE && 
		//	m_playtype[MH_PT_HongTongWangPai_ShuangWang] &&
		//	THINK_OPERATOR_HONGTONG_XUANWANG == op.code &&
		//	!is_invalid_card(op.tile))
		//{
		//	m_hongTongWangPai_ShuangWang[op.pos] = TileToCard(op.tile);
		//	//TODO：是否需要添加返回给客户端选王操作结果的消息
		//	return;
		//}

		// 对于玩家点的“过”按钮需要进行特殊识别特殊处理
		if (passive_op_is_valid_Guo(op)) {
			// 玩家点的“过”按钮
			passive_op_process_Guo(op);
		} 
		else {
			// 每个玩家当前都有一个 ThinkUnit 列表，现在识别出玩家选了哪个
			// 别人出牌检查自己胡单元
			ThinkUnit *unit = find_think_unit(op);
			if (NULL == unit) {
				// 比如吃和碰，吃优先，一个用户点了吃，桌面状态就变了，另一个用户发来的碰请求就有可能跑到这个分支
				// 玩家用外挂乱发指令也会跑到这个分支
				// 似乎不应该返回错误码，应该直接忽略？
				return send_passive_op_error(op, 1);
			}
			// 标记这个玩家的选择，后面会汇总分析
			m_thinkRet[op.pos] = *unit;
		}

		// 收炮直接就显示这个人胡了，似乎不太对啊
		// 比如两个人可以收炮，一个次优先级的先选收炮，一个高优先级的后选收炮，次优先级的不成功却显示胡的特效
		if (op.code == THINK_OPERATOR_BOMB) passive_op_process_ShouPaoHu(op);
		// 录像：别人出牌后，该玩家点了哪个按钮
		VideoAdd__doing(op);
		// 清空该玩家的可选操作，表示该玩家已经做过选择了
		m_thinkInfo[op.pos].Reset();

		if (!has_YiPaoDuoXiang() && m_decideGuoHu[pos]) {
			// 这个人选择的过胡，得重新发起思考
			SetThinkIng();
		} else {
			// 汇总分析
			CheckThink();
		}
	}

	void PrintCard(int pos, std::string &text, int nCount, CardValue cards[])
	{
		std::ostringstream str;
		for (int i = 0; i < nCount; i++)
		{
			str << (int)ToTile(cards[i]) << ',';
		}
		MHLOG_PLAYCARD("pos:%d %s: cards_size:%d cards:%s", pos, text.c_str(), nCount, str.str().c_str());
	}

	void OnUserReconnect(User *pUser) {
		if (pUser == NULL || m_desk == NULL) {
			return;
		}

		// 发送当前圈数信息
		// 为什么要发给桌子，而不是只发给这个短线重连的玩家？
		notify_desk_match_state();

		Lint pos = m_desk->GetUserPos(pUser);
		if (!is_pos_valid(pos)) {
			LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
			return;
		}
		MHLOG_PLAYCARD("*******断线重连desk:%d userid:%d pos:%d m_curPos=%d, m_needGetCard:%d m_curGetCard:%d, m_curOutCard:%d",
			m_desk->GetDeskId(), pUser->GetUserDataId(), pos, m_curPos, m_needGetCard, m_curGetCard?ToTile(m_curGetCard):-1, m_curOutCard?ToTile(m_curOutCard):-1);
			//m_needGetCard，m_curGetCard?ToTile(m_curGetCard):0, m_curOutCard?ToTile(m_outCard):0);
		Lint nCurPos = m_curPos;
		// 如果在一盘结束那个地方重连，该是个什么情况？
		Lint nDeskPlayType = m_play_state;
		LLOG_DEBUG("Desk::OnUserReconnect send out");
		LMsgS2CDeskState reconn;
		reconn.m_user_count = m_player_count;
		reconn.m_state = get_desk_state();
		reconn.m_pos = nCurPos;
		reconn.m_time = 15;
		reconn.m_zhuang = m_zhuangpos;
		reconn.m_myPos = pos;
		reconn.m_deskCreatType = m_desk->GetCreatType();
		if (nCurPos != pos) {
			reconn.m_flag = 0;
		} else {
			reconn.m_flag = 1; // 不知道对不对
		}
		reconn.m_dCount = m_deskCard.size();

		for (int x = 0; x < m_player_count; x++) {
#ifdef USE_NEW_TING
			reconn.m_TingState.push_back(m_tingState[x].m_ting_state >= Ting_waitHu ? 1 : 0);
#else
			reconn.m_TingState.push_back(m_tingState[x].m_ting_state >= Ting_waitOutput ? 1 : 0);
#endif			
			reconn.m_iTing.push_back(m_tingState[x].m_ting_state);
			MHLOG_TING("*********短重連后发送给客户端的听状态 pos = %d, m_tingState[x].m_ting_state= %d, reconn.m_iTing %d", x, m_tingState[x].m_ting_state, reconn.m_iTing[x]);
			if ( x== pos && m_tingState[x].m_ting_state > Ting_waitOutput) {
				MHLOG_TING("***** x== pos && m_tingState[x].m_ting_state > Ting_waitOutput = true, pos = %d, m_tingState[x].hu_units.size() = %d", x, m_tingState[x].hu_units.size());
				for (Lint i = 0; i < m_tingState[x].hu_units.size(); i++) {

					HuUnit temp = m_tingState[x].hu_units[i];
					reconn.m_iTingHuCard.push_back(*temp.hu_card);
					MHLOG_TING("********* hu_cards: i=%d m_tingState[x].hu_units[i].hu_card = %d", i,  ToTile(*temp.hu_card));
				}

				//扣点需要加入耗子牌
				if (FEAT_GAME_TYPE == KouDian)
				{
					for (size_t m = 0; m < mGameInfo.m_hCard.size(); m++)
					{						
						if (!VecHas(reconn.m_iTingHuCard, ToCardValue(mGameInfo.m_hCard[m])))
						{
							reconn.m_iTingHuCard.push_back(ToCardValue(mGameInfo.m_hCard[m]));
						}
					}
				}
			}
		}

		
		for (Lint i = 0; i < m_player_count; ++i) {
			//reconn.m_cardCount[i] = m_handCard[i].size() - m_suoHandCard[i].size();
			reconn.m_cardCount[i] = m_handCard[i].size();
			// 对于自己的牌，要减掉锁牌数目，对于别人的牌，不减锁牌数目，别人牌就是显示有几张，看不到牌值
			if (pos == i) reconn.m_cardCount[i] -= m_suoHandCard[i].size();
			reconn.m_oCount[i] = m_outCard[i].size();
			reconn.m_aCount[i] = m_angangCard[i].size();
			reconn.m_mCount[i] = m_minggangCard[i].size();
			reconn.m_pCount[i] = m_pengCard[i].size();
			reconn.m_eCount[i] = m_eatCard[i].size();
			reconn.m_score.push_back(m_accum_score[i] + ShanXi_JiaDe1000FenZhiZaiTouXiangChuXianShi);
			reconn.m_kouCount[i] = m_kouCount[i];
			VecExtend(reconn.m_oCard[i], m_outCard[i]);
			VecExtend(reconn.m_aCard[i], m_angangCard[i]);
			VecExtend(reconn.m_mCard[i], m_minggangCard[i]);

			for (Lint m = 0; m < m_minggangCard[i].size(); m++) {
				tile_t m_tile = ToTile(m_minggangCard[i][m]);
				for (Lint n = 0; n <= m_ps[i].gangs.size(); n++)
				{
					if (m_ps[i].gangs[n].tile == m_tile) {
						reconn.m_mCardFirePos[i][m] = m_ps[i].gangs[n].fire_pos;
						break;
					}
				}
			}
		
			////////////////////////////////////////////////////////////
			VecExtend(reconn.m_pCard[i], m_pengCard[i]);

			for (Lint m = 0; m < m_pengCard[i].size(); m++) {
				tile_t m_tile = ToTile(m_pengCard[i][m]);
				for (Lint n = 0; n <= m_ps[i].pengs.size(); n++)
				{
					if (m_ps[i].pengs[n].tile == m_tile) {
						reconn.m_pCardFirePos[i][m] = m_ps[i].pengs[n].fire_pos;
						break;
					}
				}
			}
	
			//////////////////////////////////////////////////////////

		}

		// 耗子牌是哪些，重发给客户端
		//洪洞王牌双王大将玩法，每个人耗子牌都不一样，将每个人的王牌信息推送给客户端（**Ren 2017-12-13）暂时注释掉（2018-01-08）
		//if (HongTongWangPai == FEAT_GAME_TYPE && m_playtype[MH_PT_HongTongWangPai_ShuangWang])
		//{
		//	for (int i = 0; i < m_player_count; i++)
		//	{
		//		/*reconn.m_hongTongShuangWang[i].kColor = m_hongTongWangPai_ShuangWang[i].m_color;
		//		reconn.m_hongTongShuangWang[i].kNumber = m_hongTongWangPai_ShuangWang[i].m_number;*/
		//	}
		//}
		//else if (!mGameInfo.m_hCard.empty()) {  //注：因为注释上面的if，所以改else if 为if
		if (!mGameInfo.m_hCard.empty()) {
			VecExtend(reconn.m_hunCard, mGameInfo.m_hCard);
		}

		// 将锁牌从手牌中排除，发给客户端
		CardVector tmpHand = m_handCard[pos];
		gCardMgrSx.SortCard(tmpHand);
		for(Lsize j = 0; j < m_suoHandCard[pos].size(); ++j) {
			if(VecHas(tmpHand, m_suoHandCard[pos][j])) {
				gCardMgrSx.EraseCard(tmpHand, m_suoHandCard[pos][j], 1);
			}
		}
		// 我的牌，客户的重连，之前莫得牌的重新拿出来发给他
		if (nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos) {
			if (m_curGetCard) { 
				reconn.m_cardCount[pos] -= 1;
				gCardMgrSx.EraseCard(tmpHand, m_curGetCard);
				MHLOG_PLAYCARD("***Desk:%d 断线重连 我的牌，客户的重连，之前莫得牌的重新拿出来发给他 pos:%d userid:%d card=%d", m_desk ? m_desk->GetDeskId() : -1,
					pos, pUser ? pUser->GetUserDataId() : -1, ToTile(m_curGetCard));
			}
			VecExtend(reconn.m_cardValue, tmpHand);
		} else {
			VecExtend(reconn.m_cardValue, tmpHand);
		}
		VecExtend(reconn.m_cardSuoValue, m_suoHandCard[pos]);

		// 该出牌的玩家，多发一张牌，用于打出去。
		if (m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD) {
			if (m_curOutCard && pos != nCurPos) {
				// 这个地方只把牌数加1，却没有去添牌，是为什么？
				reconn.m_cardCount[nCurPos] ++;
			} else if (m_curOutCard && pos == m_beforePos && m_beforeType == THINK_OPERATOR_OUT) {
				// 修复碰，杠，胡，客户端断线多牌的问题
				//reconn.m_cardCount[pos]++;
				//reconn.m_oCount[pos]--;
				//reconn.m_cardValue[reconn.m_cardCount[pos] - 1] = ToCardValue(m_curOutCard);
				MHLOG_PLAYCARD("***Desk:%d 该出牌的玩家，多发一张牌，用于打出去 pos:%d userid:%d card=%d", m_desk ? m_desk->GetDeskId() : -1,
					pos, pUser ? pUser->GetUserDataId() : -1, ToTile(m_curOutCard));
			}
		}

		for (int x = 0; x < m_player_count; x++) {
			 
			//LLOG_DEBUG("*****************222222222发送给客户端的听状态 pos = %d, reconn.m_iTing %d", x, reconn.m_iTing[x]);
			if (x == pos)
			{
				PrintCard(x, std::string("hand_card"), reconn.m_cardCount[pos], reconn.m_cardValue);
			}
			PrintCard(x, std::string("out_card"), reconn.m_oCount[x], reconn.m_oCard[x]);
			PrintCard(x, std::string("angang_card"), reconn.m_aCount[x], reconn.m_aCard[x]);
			PrintCard(x, std::string("minggang_card"), reconn.m_mCount[x], reconn.m_mCard[x]);
			PrintCard(x, std::string("peng_card"), reconn.m_pCount[x], reconn.m_pCard[x]);
			 
		}
		notify_user(reconn, pos);

		// 我思考
		if (nDeskPlayType == DESK_PLAY_THINK_CARD) {
			if (m_thinkInfo[pos].NeedThink()) {
				LMsgS2CThink think;
				think.m_time = 15;
				think.m_flag = 1;
				think.m_card = ToCardValue(m_curOutCard);
				ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
				for (Lsize j = 0; j < think_units.size(); ++j) {
					ThinkData info;
					info.kType = think_units[j].m_type;
					VecExtend(info.kCard, think_units[j].m_card);
					think.m_think.push_back(info);
				}
				notify_user(think, pos);
			}
		}

		// 我出牌
		if (nDeskPlayType == DESK_PLAY_GET_CARD && pos == nCurPos) {
			LMsgS2COutCard msg;
			msg.kTime = 15;
			msg.kPos = pos;
			msg.kDCount = (Lint)m_deskCard.size();
			msg.kFlag = (m_curGetCard && m_needGetCard) ? 0 : 1;
#ifndef USE_NEW_TING
			msg.m_ting = m_tingState[pos].m_ting_state >= Ting_waitOutput;
			if (m_needGetCard && m_curGetCard) {
				msg.SetCurCard(ToCardValue(m_curGetCard));
			}

			LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
			//扣点胡口需要加入耗子牌
			if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
			{
				Extend_TingChuKou_Haozi(msg);
			}

			ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
			for (Lsize j = 0; j < think_units.size(); ++j) {
				ThinkData info;
				info.m_type = think_units[j].m_type;
				VecExtend(info.m_card, think_units[j].m_card);
				msg.m_think.push_back(info);
			}
#else 
			msg.kTing =  m_tingState[pos].m_ting_state == Ting_waitHu ? 1 : 0;
			if (m_needGetCard && m_curGetCard) {
				msg.SetCurCard(ToCardValue(m_curGetCard));
			}
			if (m_tingState[pos].m_ting_state == Ting_waitHu)
			{
				LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
				//扣点胡口需要加入耗子牌
				if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
				{
					Extend_TingChuKou_Haozi(msg);
				}
			}
			ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
			for (Lsize j = 0; j < think_units.size(); ++j) {
				ThinkData info;
				info.kType = think_units[j].m_type;				
				if (m_tingState[pos].m_ting_state == Ting_waitOutput)
				{
					MHLOG_TING("****断线重连SUIJUN更改听牌逻辑,可以听牌时将玩家的听口，胡口数据打包发送pos=%d msg.m_ting = %d m_ting_state = %d", pos, msg.m_ting, m_tingState[pos].m_ting_state);
					if (info.kType == THINK_OPERATOR_TING)
					{	
						this->LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, think_units[j].ting_unit);
						MHLOG_TING("****think_units[j].ting_unit.size() = %d", think_units[j].ting_unit.size());
						for (int k = 0; k < think_units[j].ting_unit.size(); k++)
						{
							const TingUnit & tu = think_units[j].ting_unit[k];
							MHLOG_TING("****听出口k:%d, m_out_card: %d", k, ToTile(tu.out_card));
							for (int m = 0; m < tu.hu_units.size(); m++)
							{
								MHLOG_TING("*******胡口m:%d, hu_units[m].hu_card: %d", m, ToTile(tu.hu_units[m].hu_card));
							}
						}
					}					
				}
				VecExtend(info.kCard, think_units[j].m_card);
				msg.kThink.push_back(info);
			}
#endif // !USE_NEW_TING
			notify_user(msg, pos);
			MHLOG_PLAYCARD("***我出牌LMsgS2COutCard msg.pos:%d, msg.m_dcount:%d msg.flag:%d msg.color:%d msg.number:%d", msg.kPos, msg.kDCount, msg.kFlag, msg.kColor, msg.kNumber);
			//我思考硬扣
			if (FEAT_GAME_TYPE == HongTongWangPai && m_tingState[pos].m_ting_state == Ting_waitHu && m_bInYingKouThink[pos] )
			{
				ActivePosOp op(pos, THINK_OPERATOR_OUT, ToTile(m_curOutCard));
				notify_YingkouThink(op);
			}
		}

		// 桌面上的牌重新发给玩家的桌牌
		if (m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD && m_GangThink != GangThink_qianggang) {
			if (m_curOutCard) {
				LMsgS2CUserPlay sendMsg;
				sendMsg.m_errorCode = 0;
				sendMsg.m_pos = nCurPos;
				sendMsg.m_card.kType = THINK_OPERATOR_OUT;
				sendMsg.m_card.kCard.push_back(ToCardValue(m_curOutCard));
				notify_user(sendMsg, pos);
			}
		}
	}



	void ProcessRobot(Lint pos, User *pUser) {
		if (!is_pos_valid(pos)) return;
		switch (m_play_state) {
		case DESK_PLAY_GET_CARD:
			// 打出牌去
			if (m_curPos == pos) {
				ActivePosOp op(pos, THINK_OPERATOR_OUT, ToTile(VecFront(m_handCard[pos])));
				active_op_process(op);
			}
			break;
		case DESK_PLAY_THINK_CARD:
			if (m_thinkInfo[pos].NeedThink()) {
				ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
				for (size_t i = 0; i < think_units.size(); i++) {
					int code = think_units[i].m_type;
					tile_t tile = ToTile(VecFront(think_units[i].m_card));
					if (code == THINK_OPERATOR_BOMB || code == THINK_OPERATOR_TING) {
						if (code == THINK_OPERATOR_TING)
						{
							MHLOG_TING("********机器人到了听口******* pos =%d", pos);
							for (int i = 0; i < m_handCard[pos].size(); i++)
							{
								MHLOG_TING("*******card i = %d, tile = %d", i, ToTile(m_handCard[pos][i]));
							}
						}
#ifdef USE_NEW_TING
						if (m_tingState[pos].m_ting_state == Ting_waitOutput)
						{
							m_tingState[pos].m_ting_state = Ting_Null;
						}
						ActivePosOp op(pos, THINK_OPERATOR_OUT, tile); //打出牌去
						active_op_process(op);
						return;

#else
						PassivePosOp op(pos, THINK_OPERATOR_NULL, tile);
						passive_op_process(op);
#endif // USE_NEW_TING
						return;
					} else {
						PassivePosOp op(pos, code, tile);
						passive_op_process(op);
						// 也得return
						return;
					}
				}
			}
			break;
		}
	}


	int calc_gang_loss(GangItem const &item) {
		int loss = 1;
		if (LingChuanMaJiang == FEAT_GAME_TYPE) loss = 0; //陵川靠八张杠算张数不单独算杠分
		if (KouDian == FEAT_GAME_TYPE) loss = tile_to_points(item.tile);
		if (GuaiSanJiao == FEAT_GAME_TYPE && m_playtype[MH_PT_GuaSanJiao_High_Score]) loss = 5; // 大唐高分，改为低分
		//忻州扣点、扣点：杠算点分（**Ren 2017-11-28）
		if (XinZhouKouDian == FEAT_GAME_TYPE) loss = tile_to_points(item.tile);
		if (item.is_agang()) loss *= 2;
		return loss;
	}

	/*************************************************************************
	* 描述：忻州扣点用于处理未上听点炮包杠包胡 （**Ren 2017-11-23）
	* 参数：over 游戏结束的消息
	* 参数：bombpos 点炮位置
	* 返回：int 总的杠数
	*/
	int balance_gang_score_baohubaogang(LMsgS2CGameOver& over, int bombpos)
	{
		int total_gangs = 0;
		for (int i = 0; i < m_player_count; ++i)
		{
			const GangVec& history = m_ps[i].gangs;
			for (int k = 0; k < history.size(); ++k)
			{
				const GangItem& item = history[k];
				Lint gang_score = calc_gang_loss(item);
				++total_gangs;
				over.kDgang[item.fire_pos]++;
				if (item.is_mgang()) over.kDgang[i]++;
				else over.kDgang[i]++;
				if (i == bombpos) continue;
				over.kGangScore[i] += gang_score * (m_player_count - 1);
				over.kGangScore[bombpos] -= gang_score * (m_player_count - 1);
			}
		}
		return total_gangs;
	}

	int balance_gang_score(LMsgS2CGameOver& over, bool is_win, int bombpos, bool is_unting_bomb) {
		int total_gangs = 0;
		// 计算杠牌得分
		for (Lint i = 0; i < m_player_count; ++i) {
			// 流局不计算杠分的，但开启“杠不随胡”选项就要计算了
			bool normal_gang_score_enabled = is_win || m_playtype[PT_HuangZhuangBuHuangGang];
			// 立四启用了这个选项，如果不是胡家，就不给杠分
			if ((FEAT_ZhiYouHuPaiWanJiaGangSuanFen || m_playtype[PT_ZhiYouHuPaiWanJiaGangSuanFen]) && !m_winConfirmed[i]) {
				normal_gang_score_enabled = false;
			}
			// 增加小选项：未上听杠不算分（抠点），不影响杠耗子算分
			if (has_WeiShangTingGangBuSuanFen()) {
				if (m_tingState[i].m_ting_state != Ting_waitHu) normal_gang_score_enabled = false;
			}
			// 遍历这个玩家的所有杠记录
			const GangVec& history = m_ps[i].gangs;
			for (size_t k = 0; k < history.size(); ++k) {
				const GangItem& item = history[k];
				// 抠点：杠耗子玩法
				bool is_ganghaozi = KouDian == FEAT_GAME_TYPE && IsHaozipai(TileToCard(item.tile));
				Lint gang_score;
				bool diangang_baogang_enabled;

				if (!is_ganghaozi) {
					if (!normal_gang_score_enabled) continue; // 流局不计算普通的杠分，只计算杠耗子的杠分
					gang_score = calc_gang_loss(item);
					diangang_baogang_enabled = FEAT_DianGangBaoGang && item.is_zhigang();
					// 如果这个玩法开启了听牌不包点杠，并且这个玩家处于听牌状态，就禁用点杠包杠
					if (FEAT_TingPaiBuBaoDianGang && item.firer_has_ting) diangang_baogang_enabled = false;
				} else {
					gang_score = 100; // 杠耗子标准分是100
					if (KouDian == FEAT_GAME_TYPE) gang_score = 40;  // 扣点新版本金暗杠标准分40
					if (HongTongWangPai == FEAT_GAME_TYPE) gang_score = calc_gang_loss(item); // 洪洞王牌按普通的杠处理
					m_desk->m_vip->m_JinAnGangCount[k] += 1;  // 保存杠耗子数量
					diangang_baogang_enabled = item.is_mgang();
				}

				++total_gangs;
				if (diangang_baogang_enabled) {
					// 对于点杠包杠的人，客户端显示“点杠”字样，还是“点杠包杠”字样  答：“点杠”
					// 立四没有点杠包杠，就不显示吧？  答：对
					// 点杠的这个人可能点了好几家的杠，或者点了同一家好几个杠，比如点了同一家3个杠，显示成“点杠X3”？  答：点几个就X几
					over.kDgang[item.fire_pos]++;
				}
				if (item.is_mgang()) over.kMgang[i]++;
				else over.kAgang[i]++;
				// 对于某个玩家的某个杠，从其他玩家抽分，可能由别人代掏（包杠）
				for (Lint j = 0; j < m_player_count; ++j) {
					if (j == i) continue; // 自己不给自己输分
					if (FEAT_DianGangDanRenKouFen && (j != item.fire_pos) && (item.fire_pos != INVAILD_POS)) continue; // 拐三角使用，明杠只算点杠人单人杠分
					Lint tmp_j = j;
					// 【明杠】上家打牌之后，如果你手上有三张跟那被打出的牌一样的，就可以“杠牌”，这种杠牌叫做“明杠”。
					// 【暗杠】如果是手内摸有四张相同的牌，取出杠牌，则叫做“暗杠”。
					// 【加杠】如果是已经碰牌了，却又再摸入一张相同的牌，也可以叫杠牌—这种杠牌叫做“加杠”。
					// 目前的实现，加杠也被归作明杠
					// 点杠包杠，其它都是三人掏，和未上听点炮没关系了
					if (diangang_baogang_enabled) tmp_j = item.fire_pos;

					//点炮包杠
					if (FEAT_WeiShangTingDianPaoBaoGang && is_unting_bomb && (bombpos != INVAILD_POS)) tmp_j = bombpos;
					over.kGangScore[tmp_j] -= gang_score;
					over.kGangScore[i] += gang_score;
				}
			}
		}
		return total_gangs;
	}


	int winpos_to_xianjia_loss(int winpos, tile_t last_tile) {
		FANXING fans = m_ps[winpos].fans;
		return calc_common_loss(fans, last_tile, m_ps[winpos].lingchuan_loss_score) * calc_loss_factor(fans);
	}

	// 临汾一门牌计算分数的独特的函数
	int winpos_to_xianjia_loss_for_linfenyimenpai(int winpos, tile_t last_tile,Card * winCard) {
		FANXING fans = m_ps[winpos].fans;
		//return calc_common_loss(fans, last_tile) * calc_loss_factor(fans);
		if (winpos == m_zhuangpos) {
			MHLOG("Logwyz ... calc_ye_zhuang=%d,calc_common_loss=%d, calc_loss_factor=%d", calc_ye_zhuang(m_zhuangpos, winCard), calc_common_loss(fans, last_tile), calc_loss_factor(fans));
			return (calc_ye_zhuang(m_zhuangpos, winCard) + calc_common_loss(fans, last_tile)) * calc_loss_factor(fans);
		}
		else {
			MHLOG("Logwyz ... calc_common_loss=%d, calc_loss_factor=%d",  calc_common_loss(fans, last_tile), calc_loss_factor(fans));
			return calc_common_loss(fans, last_tile) * calc_loss_factor(fans);
		}
	}

	// 一般来说 malgo 的胡牌算法得到的就是最优牌型，但抠点比较特殊，是按最后胡的牌的点数来算分的
	// malgo 判定的一条龙胡法不一定比平胡分高的
	// 所以对于胡牌是万能牌的最优判定，还应该基于听口进行计算
	tile_t calc_last_tile_for_hu_score(int winpos, Card *winCard) {
		// 这个东西弄出来是给抠点算分用的
		return m_ps[winpos].good_last_tile;


		// 应该必有winCard吧？TODO: 弄个天胡调试一下
		if (NULL == winCard) return 0;
		if (KouDian == FEAT_GAME_TYPE) {
			// 抠点胡耗子牌，得让耗子牌取个最大点数
			if (IsHaozipai(winCard)) {
				// 这个判断应该肯定成立，加上算安全监测
				if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
					const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;
					struct hufan_less_than {
						//bool operator ()(const HuUnit& lhs, const HuUnit& rhs) { return tile_to_points(CardToTile(lhs.hu_card)) < tile_to_points(CardToTile(rhs.hu_card)); }
						bool operator ()(const HuUnit& lhs, const HuUnit& rhs) { return lhs.score < rhs.score; }
					};
					std::vector<HuUnit>::const_iterator the_big = std::max_element(hu_units.begin(), hu_units.end(), hufan_less_than());
					return CardToTile(the_big->hu_card);
				}
			}
		}
		return CardToTile(winCard);
	}

	int calc_dealer_risk_score(int winpos, int losepos, bool is_zimo) {
		int risk = get_DealerRiskScore();
		int score = 0;
		if (winpos == m_zhuangpos) score = risk;
		else if (losepos == m_zhuangpos) score = risk;
		if (is_zimo && !FEAT_DisableZiMoDoubleScore) score *= 2;
		MHLOG("*************** score = %d , is_zimo && !FEAT_DisableZiMoDoubleScore = %d FEAT_DisableZiMoDoubleScore = %d",
			score, is_zimo && !FEAT_DisableZiMoDoubleScore, FEAT_DisableZiMoDoubleScore);
		return score;
	}

	void balance_hu_score(LMsgS2CGameOver& over, int bombpos, bool is_unting_bomb, Card *winCard) {
		// 流局不能计算胡分
		for (int winpos = 0; winpos < m_player_count; ++winpos) {
			// 如果玩家可以胡，并且点了“胡”按钮了，这个标记会被设置的
			// 遍历所有胡的玩家，分别赢分就行了
			if (!m_winConfirmed[winpos]) continue;

			//临汾一本牌 结算   by wyz
			//闲家输的分
			int common_loss = 0;
			if (FEAT_GAME_TYPE == LinFenYiMenPai && m_playtype[PT_ShuYe] && winpos == m_zhuangpos) {
				MHLOG("Logwyz ... 庄家赢,临汾一门牌结算");
				common_loss = winpos_to_xianjia_loss_for_linfenyimenpai(winpos, calc_last_tile_for_hu_score(winpos, winCard), winCard);
			}
			else{
				 common_loss = winpos_to_xianjia_loss(winpos, calc_last_tile_for_hu_score(winpos, winCard));
			}
			int dian_pao_extra_loss = 0;
			if (bombpos != INVAILD_POS)
			{
				if (is_unting_bomb)
					dian_pao_extra_loss = FEAT_FangPaoExtraLoss;
				else
					dian_pao_extra_loss = FEAT_FangPaoExtraLoss_TingHou;
			}

			for (int x = 0; x < m_player_count; ++x) {
				if (x == winpos) continue; // 自己不给自己输分
				// 如果启用了点炮单人扣分，并且这是点炮胡，并且这个玩家不是点炮的，就不扣分
				if (FEAT_DianPaoDanRenKouFen && bombpos != INVAILD_POS && x != bombpos) continue;

				int this_loss = common_loss;
				//陵川麻将，庄家输赢2倍积分
				if (FEAT_GAME_TYPE == LingChuanMaJiang && (winpos == m_zhuangpos || x == m_zhuangpos))
					this_loss *= 2;

				//晋城麻将，庄家输赢2倍积分
				if (FEAT_GAME_TYPE == JinChengMaJiang && (winpos == m_zhuangpos || x == m_zhuangpos))
					this_loss *= 2;

				if (x == bombpos) this_loss += dian_pao_extra_loss; // 放炮者额外扣分


				FANXING fans = m_ps[winpos].fans;
				bool is_zimo = !!(fans & FAN_ZiMo);
				this_loss += calc_dealer_risk_score(winpos, x, is_zimo); // 坐庄风险分

			    

				
				//// 临汾一门牌 庄家输，加上庄家的页数  by wyz  
				//// 规则变了，庄家输，加上的是赢家的页数， by wyz 20170918
				if (FEAT_GAME_TYPE == LinFenYiMenPai && m_playtype[PT_ShuYe] &&  x == m_zhuangpos  && m_zhuangpos!= winpos  ) {
					this_loss += calc_ye_zhuang(winpos, winCard);
					MHLOG("Logwyz 庄家输,临汾一门牌,加上赢家的页数[%d]", calc_ye_zhuang(winpos, winCard));
				}

				int paypos = x;
				if (is_unting_bomb) paypos = bombpos; // 未上听点炮包胡
				over.kHuScore[paypos] -= this_loss;
				over.kHuScore[winpos] += this_loss;
			}
		}
	}

	int calc_jin_loss(int pos) {
		int shangjin = m_ps[pos].shangjin;
		// 最多按上金3个计算：1个3分、2个9分、3个27分
		if (shangjin >= 3) return 27;
		if (shangjin >= 2) return 9;
		if (shangjin >= 1) return 3;
		return 0;
	}

	void balance_jin_score(LMsgS2CGameOver& over, bool is_win, int bombpos, bool is_unting_bomb) {
		// 贴金：金分计算
		if (TieJin != FEAT_GAME_TYPE) return;
		// 荒庄则没有金分
		if (!is_win) return;
		for (int winpos = 0; winpos < m_player_count; ++winpos) {
			// 听牌后才算分
			//if (!m_winConfirmed[winpos] && m_tingState[winpos].m_ting_state != Ting_waitHu) continue;
			// 金随胡走：只有胡牌玩家才能得金分
			if (!m_winConfirmed[winpos]) continue;
			over.kJinNum[winpos] = m_ps[winpos].shangjin;
			int loss = calc_jin_loss(winpos);
			if (loss == 0) continue;
			// 胡牌玩家的金分会被翻番
			// 最新情况：金分不翻番
#if 0
			if (m_winConfirmed[winpos]) loss *= calc_loss_factor(m_ps[winpos].fans);
#endif
			for (int x = 0; x < m_player_count; ++x) {
				if (x == winpos) continue; // 自己不给自己输分
				int this_loss = loss;
				int paypos = x;
				if (is_unting_bomb) paypos = bombpos; // 未上听点炮也包金分
				over.kJinScore[paypos] -= this_loss;
				over.kJinScore[winpos] += this_loss;
			}
		}
	}

	//// 计算赢家的页数   by wyz
	int calc_ye_zhuang(int zhuangpos, Card* winCard) {
		if (zhuangpos == 4)return 0;
		Lint zhuang_ye=0 , wan_num = 0 ,tuo_num = 0,suo_num = 0,zi_num = 0;
		Lint color = -1;
		CardVector* cards[5] = { &m_handCard[zhuangpos], &m_eatCard[zhuangpos], &m_pengCard[zhuangpos], &m_angangCard[zhuangpos], &m_minggangCard[zhuangpos] };
		for (int i = 0; i < 5; ++i)
		{
			int space = 1;
			// 吃碰杠只检查第一张牌 这样可以避免带混的牌 带混的牌放在了牌组的后面
			if (i == 1 || i == 2)
			{
				space = 3;
			}
			else if (i == 3 || i == 4)
			{
				space = 4;
			}
			int size = (int)cards[i]->size();
			for (int k = 0; k < size; k += space)
			{
				if (!cards[i]->at(k))
				{
					return false;
				}
				int color_ = cards[i]->at(k)->m_color;
				switch (color_)
				{
				case CARD_COLOR_ZI:
					if (i == 1 || i == 2)zi_num += 3;
					else if (i == 3 || i == 4)zi_num += 4;
					else zi_num++;
					break;
				case CARD_COLOR_WAN:
					if (i == 1 || i == 2)wan_num += 3;
					else if (i == 3 || i == 4)wan_num += 4;
					else wan_num++;
					break;
				case CARD_COLOR_TUO:
					if (i == 1 || i == 2)tuo_num += 3;
					else if (i == 3 || i == 4)tuo_num += 4;
					else tuo_num++;
					break;
				case CARD_COLOR_SUO:
					if (i == 1 || i == 2)suo_num += 3;
					else if (i == 3 || i == 4)suo_num += 4;
					else suo_num++;
					break;
				default:
					break;

				}
				
			}
		}

		if (winCard != NULL)
		{
			switch (winCard->m_color)
			{
			case CARD_COLOR_ZI:
				zi_num++;
				break;
			case CARD_COLOR_WAN:
				wan_num++;
				break;
			case CARD_COLOR_TUO:
				tuo_num++;
				break;
			case CARD_COLOR_SUO:
				suo_num++;
				break;

			}
		}

		zhuang_ye = zi_num;
		if (zi_num < wan_num)zhuang_ye = wan_num;
		if (zi_num < tuo_num)zhuang_ye = tuo_num;
		if (zi_num < suo_num)zhuang_ye = suo_num;
		MHLOG("Logwyz ... zhuang_ye=%d", zhuang_ye);
		return zhuang_ye;
	}

	//陵川麻将流局庄输2分
	void balance_liuju_score_for_lingchuan(LMsgS2CGameOver& over, bool is_win = false)
	{
		if (is_win) return;
		for (int pos = 0; pos < m_player_count; pos++)
		{
			if (pos != m_zhuangpos)
			{
				over.kHuScore[pos] += 2;
				over.kHuScore[m_zhuangpos] -= 2;
			}
		}
	}

	int calcScore(Lint result, Lint winpos, Lint bombpos, LMsgS2CGameOver& over, Card* winCard) {
		MHLOG("*****************************计算分数  GameType=%d winpos=%d bombpos=%d", FEAT_GAME_TYPE, winpos, bombpos);
		if (result == WIN_BOMB)
			if (bombpos < 0 || bombpos > m_player_count - 1)
				return 0;

		bool is_win = winpos != INVAILD_POS; // 区分胜局和流局，流局也要计算杠耗子的杠分，流局不计算其它的杠分
		

		// 未上听点炮是特殊情况：只包胡，不包杠
		// 这种情况下点炮玩家比较倒霉
		bool is_unting_bomb = result == WIN_BOMB && m_tingState[bombpos].m_ting_state != Ting_waitHu;

		// m_gangScore 和 m_huScore 在消息中为设计成 std::vector 了，应该用数组[4]
		while (over.kGangScore.size() < 4) over.kGangScore.push_back(0);
		while (over.kHuScore.size() < 4) over.kHuScore.push_back(0);

		//陵川麻将流局不算分
		if ((!is_win) && (FEAT_GAME_TYPE == LingChuanMaJiang))
		{
			//balance_liuju_score_for_lingchuan(over);
		}

		int total_gangs = 0;   //(**Ren 2017-11-23)
		//忻州扣点 （**Ren 2017-11-23）
		if (XinZhouKouDian == FEAT_GAME_TYPE && is_unting_bomb)
		{
			//忻州扣点有人未上听点炮，包胡包所有杠 （**Ren 2017-11-23）
			total_gangs = balance_gang_score_baohubaogang(over, bombpos);
		}
		else if (NianZhongZi == FEAT_GAME_TYPE)
		{
			//撵中子杠不算分
		}
		else
		{
		    total_gangs = balance_gang_score(over, is_win, bombpos, is_unting_bomb);
        }
		balance_hu_score(over, bombpos, is_unting_bomb, winCard);
		balance_jin_score(over, is_win, bombpos, is_unting_bomb);

		// 乘以桌子上底分
		Lint desk_cell_score = 1;
		if (this->m_desk)
		{
			MHLOG("*****balance_hu_score  desk->cellsore = %d", m_desk->m_cellscore);
			if (m_desk->m_cellscore > 0) desk_cell_score = m_desk->m_cellscore;
		}
		// 汇总杠分和胡分
		for (int x = 0; x < m_player_count; x++) 	{
			over.kGangScore[x] *= desk_cell_score;
			over.kHuScore[x] *= desk_cell_score;
			over.kJinScore[x] *= desk_cell_score;
			over.kScore[x] = over.kGangScore[x] + over.kHuScore[x] + over.kJinScore[x];
			
			m_accum_score[x] += over.kScore[x];
		}
		for (int i = 0; i < m_player_count; i++)
		{
			MHLOG_PLAYCARD("Desk:%d -- Score >> pos:%d, gang_fen:%d hu_fen:%d jin_fen:%d total:%d m_accum_score:%d", m_desk->GetDeskId(), i, over.kGangScore[i], over.kHuScore[i], over.kJinScore[i], over.kScore[i], m_accum_score[i]);
		}
		return total_gangs;
	}

	void deal_zhuangpos(int bombpos, int winpos, int total_gangs) {
		// 一局结束后，计算坐庄位置的变化
		Lint zhuangPos = m_zhuangpos;
		int primary_winpos = find_primary_winpos(bombpos, winpos);
		m_zhuangpos = calc_next_zhuangpos(m_curPos, primary_winpos, total_gangs);
		m_desk->m_zhuangPos = m_zhuangpos;
		// 所有情况下，庄不变，就不加局数
		//if (zhuangPos != m_zhuangpos) increase_round();
		// 任何情况下都加局数
		increase_round();
	}

	void add_round_log(LMsgS2CGameOver &over, int bombpos, int zhuangPos) {
		Lint mgang[4] = { 0 };
		for (Lint i = 0; i < m_player_count; ++i)
		{
			mgang[i] += m_minggang[i];
			mgang[i] += m_diangang[i];
		}

		// churunmin: AddLog似乎是从黑龙江代码中copy来的，接口和山西的不一样，得做一个HuInfo数组
		HuInfo huinfo[4];
		for (Lint i = 0; i < m_player_count; ++i) {
			WIN_TYPE_SUB type = (WIN_TYPE_SUB)over.kWin[i];
			Lint abomb = INVAILD_POS;
			// 如果收炮或者大胡收炮，就记录一下点炮玩家的位置
			if (WIN_SUB_BOMB == type || WIN_SUB_DBOMB == type) abomb = bombpos;
			huinfo[i] = HuInfo(type, abomb);
			huinfo[i].hu_types = over.m_hu(i);
		}

		// 保存结果	
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLog(m_desk->m_user, over.kScore, huinfo, zhuangPos, m_angang, mgang, m_video.m_Id, over.kTing);
			m_desk->m_vip->ResetJinAnGangCount(); // 使用后清除掉
		}

	}

	void finish_round(Lint result, Lint winpos, Lint bombpos, Card *winCard) {
		// 次优先级的玩家可能先确认了胡，如果没有一炮多响，这个确认应该是无效的
		if (!has_YiPaoDuoXiang()) {
			m_winConfirmed.clear();
			m_winConfirmed.set(winpos);
		}

		// 输赢确定后，先检测清空连庄分，这样这把的庄家次要赢的话，也不加连庄分
		// 如果庄没赢，就得把连庄加分给清了，后面计分要用
		// 如果庄不是首要赢的玩家的话，也得把连庄分给清了
		//if (!m_winConfirmed[m_zhuangpos]) m_lianzhuang_jiafen = 0;
		if (false) //
		{
			int primary_winpos = find_primary_winpos(bombpos, winpos);
			if (m_zhuangpos != primary_winpos) m_lianzhuang_jiafen = 0;
		}

		//广播结果
		LMsgS2CGameOver over;
		over.kUser_count = m_player_count;

		//撵中子：撵中子的明杠开花不算自摸（**Ren 2017-12-11）
		if (NianZhongZi == FEAT_GAME_TYPE && 2 == m_getChardFromMAGang[winpos] && result == WIN_ZIMO)
		{
			result = WIN_MINGGANGHUA;
		}

		// churunmin: 同步到新设计的状态对象上，用于后续计分
		FANXING allowed_fans = m_allowed_fans;
		sync_to_m_ps(result, winCard, allowed_fans);

		//计算分数
		int total_gangs = calcScore(result, winpos, bombpos, over, winCard);

		//拐三角增加连庄番并保存连庄加分
		if (FEAT_GAME_TYPE == GuaiSanJiao && (winpos != INVAILD_POS) 
			&& (winpos == m_zhuangpos) && (m_ps[winpos].fans & FAN_Zhuang)
			&& (m_lianzhuang_jiafen > 0))
		{
			m_ps[winpos].fans |= FAN_LianZhuang;
			over.kYbPiao[winpos] = m_lianzhuang_jiafen;
		}

		// 一局结束后，计算坐庄位置的变化
		Lint zhuangPos = m_zhuangpos;
		deal_zhuangpos(bombpos, winpos, total_gangs);

		// 客户端显示结算页面（录像）
		
		//添加杠上开花功能（**Ren 2017-12-11）
		if (result == WIN_ZIMO && winpos != INVAILD_POS && winCard) {
			//赢家显示：自摸胡
			over.kWin[winpos] = WIN_SUB_ZIMO;
			//暗杠后摸牌，赢家显示：自摸（暗杠开花）
			if (1 == m_getChardFromMAGang[winpos]) {
				over.kWin[winpos] = WIN_SUB_ZIMOANGANGHUA;  //9
			}
			//明杠后摸牌，赢家显示：自摸（明杠开花）
			else if (2 == m_getChardFromMAGang[winpos]) {
				over.kWin[winpos] = WIN_SUB_ZIMOMINGGANGHUA;  //10
			}

			VideoAdd__pos_oper_card(winpos, VIDEO_OPER_ZIMO, winCard);
			MHLOG_PLAYCARD("****Desk:%d 自摸胡. pos: %d, win_card:%d", m_desk ? m_desk->GetDeskId() : 0, winpos, ToTile(winCard));
		} 
		else if (result == WIN_BOMB && winpos != INVAILD_POS && winCard) {
			for (int x = 0; x < m_player_count; x++) {
				if (m_winConfirmed[x])
				{
					over.kWin[x] = WIN_SUB_BOMB;				
				}
			}
			if (bombpos != INVAILD_POS) over.kWin[bombpos] = WIN_SUB_ABOMB;
			VideoAdd__pos_oper_card(winpos, VIDEO_OPER_SHOUPAO, winCard);
			MHLOG_PLAYCARD("****Desk:%d 接炮胡. winpos:%d fire_pos:%d win_card:%d", m_desk ? m_desk->GetDeskId() : 0, winpos, bombpos, ToTile(winCard));
			for (int k = 1; k < m_player_count; k++)
			{
				int next_win_pos = (winpos + k) % m_player_count;
				if (m_winConfirmed[next_win_pos])
				{
					VideoAdd__pos_oper_card(next_win_pos, VIDEO_OPER_SHOUPAO, winCard);
					MHLOG_PLAYCARD("****Desk:%d 接炮胡. winpos:%d fire_pos:%d win_card:%d", m_desk ? m_desk->GetDeskId() : 0, next_win_pos, bombpos, ToTile(winCard));
				}			
			}
		} 
		// 撵中子：添加明杠开花不算自摸 （**Ren 2017-12-01）
		else if (result == WIN_MINGGANGHUA && winpos != INVAILD_POS && winCard){
			over.kWin[winpos] = WIN_SUB_MINGGANGHUA;  //8
			MHLOG_PLAYCARD("****Desk:%d 明杠开花. pos: %d, win_card:%d", m_desk ? m_desk->GetDeskId() : 0, winpos, ToTile(winCard));
		}
		else {
			VideoAdd__pos_oper(m_curPos, VIDEO_OPER_HUANGZHUANG);
			MHLOG_PLAYCARD("****Desk:%d 荒庄.", m_desk ? m_desk->GetDeskId() : 0);
		}

		// 保存录像
		VideoSave();

		over.kResult = result;  //撵中子：:添加明杠刚上花4 撵中子添加（**Ren 2017-12-01）

		for (Lint i = 0; i < m_player_count; ++i) {
			// 手牌数目
			// churunmin: 这个地方没减掉锁牌数目，客户端为什么没测出问题？
			over.kCount[i] = m_handCard[i].size();
			// 手牌
			for (Lint j = 0; j < over.kCount[i]; ++j) {
				over.kCard[i][j].kColor = m_handCard[i][j]->m_color;
				over.kCard[i][j].kNumber = m_handCard[i][j]->m_number;
				over.m_hand_cards(i).push_back(ToCardValue(m_handCard[i][j]));
			}
			// 胡牌类型
			if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB) {				
				vectorize_fans(m_ps[i].fans, over.m_hu(i));
				MHLOG_PLAYCARD("Desk:%d 胡牌番型- pos: %d hu_fans: %s", m_desk ? m_desk->GetDeskId() : 0, i, malgo_format_fans(m_ps[i].fans).c_str());
			}
			// 听牌状态
			over.kTing[i] = m_tingState[i].m_ting_state == Ting_waitHu;
		}
		if (winCard && winpos != INVAILD_POS) {
			over.m_hucards(winpos).push_back(ToCardValue(winCard));
		}

		VecExtend(over.kHunCard, mGameInfo.m_hCard);

		set_desk_state(DESK_WAIT);

		add_round_log(over, bombpos, zhuangPos);

		// 是否最后一局
		// 此时 m_round_offset 已经是递增过了的
		over.kEnd = m_dismissed || m_round_offset >= m_round_limit;
		// 把这盘的结算结果广播给桌子
		notify_desk(over);

		if (m_desk) m_desk->HanderGameOver(1);
	}



	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	void notify_desk_match_state() {
		LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset;
		msg.m_curMaxCircle = m_round_limit;
		m_playtype.fill_vector_for_client(msg.m_playtype, FEAT_GAME_TYPE);
		notify_desk(msg);
	}

	void notify_desk_passive_op(int pos) {
		LMsgS2CUserOper send;
		send.m_pos = pos;
		send.m_errorCode = 0;
		send.m_think.kType = m_thinkRet[pos].m_type;
		VecExtend(send.m_think.kCard, m_thinkRet[pos].m_card);
		send.m_card = ToCardValue(m_curOutCard);
		notify_desk(send);
	}

	/************************************************************************
	* 函数名：    notify_desk_xuanwang()
	* 描述：      洪洞王牌所有玩家选王结果通知给客户端
	* 返回：      (void)
	*************************************************************************/
	void notify_desk_xuanwang()
	{
		LMsgS2CUserOper send;
		send.m_xuanWang = 1;
		for (int i = 0; i < 4; ++i)
		{
			send.m_hongTongShuangWang[i] = m_hongTongWangPai_ShuangWang[i];
		}
		notify_desk(send);
	}

	void notify_desk_BuGang(int pos, int code) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = pos;
		sendMsg.m_card.kType = code;
		sendMsg.m_card.kCard.push_back(ToCardValue(m_curOutCard));
		notify_desk(sendMsg);
	}

	static bool is_invalid_card(Card card) {
		return !tile_is_valid(ToTile(card));
	}
	static bool is_invalid_card(CardValue card) {
		return !tile_is_valid(ToTile(card));
	}

	void notify_HaoZi() {

		MHLOG("*****Send to client LMsgS2CUserHaozi haozi_count=%d", mGameInfo.m_hCard.size());
		int index = 0; // 似乎没啥用，以前填的是耗子在桌牌上的索引
		LMsgS2CUserHaozi msg;
		msg.m_type = 1;
		msg.m_pos = m_curPos;
		msg.m_randNum.push_back(index);
		VecExtend(msg.m_HaoZiCards, mGameInfo.m_hCard);

		if (msg.m_HaoZiCards.size() == 1) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			CardValue haozi0 = msg.m_HaoZiCards[0];
			LLOG_ERROR("Desk %d notify 1 haozi %d|%d", desk_id, haozi0.kColor, haozi0.kNumber);
			if (is_invalid_card(haozi0)) {
				LLOG_ERROR("INVALID_CARD: desk %d notify 1 haozi %d|%d", desk_id, haozi0.kColor, haozi0.kNumber);
			}
		} else if (msg.m_HaoZiCards.size() == 2) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			CardValue haozi0 = msg.m_HaoZiCards[0];
			CardValue haozi1 = msg.m_HaoZiCards[1];
			LLOG_ERROR("desk %d notify 2 haozi %d|%d, %d|%d", desk_id, haozi0.kColor, haozi0.kNumber, haozi1.kColor, haozi1.kNumber);
			if (is_invalid_card(haozi0) || is_invalid_card(haozi1)) {
				LLOG_ERROR("INVALID_CARD: desk %d notify 2 haozi %d|%d, %d|%d", desk_id, haozi0.kColor, haozi0.kNumber, haozi1.kColor, haozi1.kNumber);
			}
		}


		ThinkData think;
		think.kCard = msg.m_HaoZiCards;
		think.kType = THINK_OPERATOR_NULL;
		msg.m_think.push_back(think);
		notify_desk(msg);
	}

	void notify_chat_test(int pos, char const *msg) {
		LMsgS2CUserSpeakTest speak;
		speak.m_userId = m_desk->m_user[pos]->GetUserDataId();
		speak.m_pos = pos;
		speak.m_type = 2;
		speak.m_msg = msg;
		notify_desk(speak);
	}

};


struct ShanXiGameHandler : ShanXiGameCore {

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
	}

	void SetDeskPlay() {
		if (!m_desk || !m_desk->m_vip) {
			return;
		}		 
		start_round();
	}

	/*   //随机选座用
	void MHSetDeskPlay(Lint PlayUserCount, Lint * PlayerStatus, Lint PlayerCount) {
		LLOG_INFO("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		if (!m_desk||!m_desk->m_vip) {
			return;
		}
		//游戏人数只能是2，3，4人
		if (PlayUserCount>1&&PlayUserCount<=4)
		{
			m_player_count=PlayUserCount;
			memset(m_players_status, 0x00, sizeof(m_players_status));
			memcpy(m_players_status, PlayerStatus, sizeof(m_players_status));
			LLOG_INFO("Logwyz-------------MHSetDeskPlay:PlayerStatus[%d][%d][%d][%d],PlayerCount=[%d]", PlayerStatus[0], PlayerStatus[1], PlayerStatus[2], PlayerStatus[3], PlayerCount);
			LLOG_INFO("Logwyz-------------MHSetDeskPlay:m_players_status[%d][%d][%d][%d]", m_players_status[0], m_players_status[1], m_players_status[2], m_players_status[3]);
		}
		else
			return;
		start_round();
	}
	*/

	void MHSetDeskPlay(Lint PlayUserCount) {
		LLOG_INFO("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		if (!m_desk||!m_desk->m_vip) {
			return;
		}
		//游戏人数只能是2，3，4人
		if (PlayUserCount>1&&PlayUserCount<=4)
		{
			m_player_count=PlayUserCount;
		}
		else
			return;
		start_round();
	}


	Lstring format_user_oper(Lint op)
	{
		switch (op)
		{

#define A(v)  case v: return #v
			A(THINK_OPERATOR_NULL);
 
			A(THINK_OPERATOR_OUT);
			A(THINK_OPERATOR_BOMB);
			A(THINK_OPERATOR_AGANG);
			A(THINK_OPERATOR_MGANG);// = 4,//明杠
			A(THINK_OPERATOR_PENG);// = 5,//碰
			A(THINK_OPERATOR_CHI);// = 6,//吃
			A(THINK_OPERATOR_TING);// = 7,//听
			A(THINK_OPERATOR_MBU);// = 8, // 补杠

			A(THINK_OPERATOR_DINGQUE);// = -3,
			A(THINK_OPERATOR_CHANGE);// = -2,
			A(THINK_OPERATOR_REOUT); //= -1, // 断线续完再次告诉玩家打出去的牌
									   //THINK_OPERATOR_NULL  = 0,
									   //THINK_OPERATOR_OUT   = 1,
									   //THINK_OPERATOR_BOMB  = 2, // 胡
									   //THINK_OPERATOR_AGANG = 3, // 暗杠	目前没用到
									   //THINK_OPERATOR_MGANG = 4, // 明杠
									   //THINK_OPERATOR_PENG  = 5, // 碰
									   //THINK_OPERATOR_CHI   = 6, // 吃     目前没用到
			A(THINK_OPERATOR_ABU);// = 12, // 暗杠
			A(THINK_OPERATOR_DIANGANGPAO);// = 14,// 玩家点杠后自摸胡算点炮胡操作
			A(THINK_OPERATOR_QIANGANGH);// = 15, //抢杠胡
			A(THINK_OPERATOR_TINGCARD); //= 16,  //听牌
			A(THINK_OPERATOR_FLY);// = 17,   //飞
			A(THINK_OPERATOR_RETURN);// = 18,   //提
			A(THINK_OPERATOR_MUSTHU);// = 19,   //必须胡
			A(THINK_OPERATOR_HONGTONG_YINGKOU); //20
			A(THINK_OPERATOR_HONGTONG_YINGKOU_NOT); //21
#undef A

		default: 
			return "Unknow operator";
		}
	}
	// 拆解 HanderUserPlayCard 和 HanderUserOperCard
	// 下面这俩消息的内容差不多
	// LMsgC2SUserPlay: ThinkData m_thinkInfo;
	// LMsgC2SUserOper: ThinkData m_think;

	// 客户端发来的玩家出牌操作，以及关联了哪些牌，应该关联且只关联一张牌
	// 可能某些思考操作也会跑到这里？
	void HanderUserPlayCard(User *pUser, LMsgC2SUserPlay *msg) {
		// 这个函数主要对客户端传来的消息做简单的转换，然后重定向到内部处理函数
		if (m_desk == NULL || pUser == NULL || msg == NULL) {
			LLOG_DEBUG("HanderUserPlayCard NULL ERROR: %p %p %p", m_desk, pUser, msg);
			return;
		}
		MHLOG("****HanderUserPlayCard Get message LMsgC2SUserPlay msgid = %d op=%s", msg->m_msgId, format_user_oper(msg->m_thinkInfo.m_type).c_str());
		int pos = m_desk->GetUserPos(pUser);
		ActivePosOp op(pos, msg->m_thinkInfo.kType, ToTile(VecFront(msg->m_thinkInfo.kCard)));
		active_op_process(op);	 
	}


	// 客户端发来的玩家思考操作：胡、杠、碰、吃，以及关联了哪些牌
	// 该自己出牌的，胡的过，客户端不发消息，其它思考的过，发消息到这里
	void HanderUserOperCard(User *pUser, LMsgC2SUserOper *msg) {
		// 这个函数主要对客户端传来的消息做简单的转换，然后重定向到内部处理函数
		if (m_desk == NULL || pUser == NULL || msg == NULL) {
			LLOG_DEBUG("HanderUserOperCard NULL ERROR: %p %p %p", m_desk, pUser, msg);
			return;
		}
		MHLOG("*****HanderUserPlayCard Get message LMsgC2SUserOper msgid = %d op=%s", msg->m_msgId, format_user_oper(msg->m_think.m_type).c_str());
		// 用户被动操作走这里，即一人出牌后，询问其它人胡杠碰吃过
		int pos = m_desk->GetUserPos(pUser);
		PassivePosOp op(pos, msg->kThink.kType, ToTile(VecFront(msg->kThink.kCard)));
		passive_op_process(op);
	}

	void OnGameOver(Lint result, Lint winpos, Lint bombpos, Card* winCard) {
		if (m_desk == NULL || m_desk->m_vip == NULL) {
			LLOG_DEBUG("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d", m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);
		finish_round(result, winpos, bombpos, winCard);
	}


};





// 各个玩法的 ShanXiGameHandlerCreator 作为全局变量，将自己注册到工厂里面就行了，不用管注销
// 工厂实际上就是个全局的 GameType 到 GameHandlerCreator 的映射
// 发现工厂用 GameHandlerCreator 创建出来的 GameHandler 都不带删除的，创建多少缓存多少
struct ShanXiGameHandlerCreator: GameHandlerCreator {
	GameType game_type;
	ShanXiGameHandlerCreator(GameType game_type): game_type(game_type) {
		GameHandlerFactory::getInstance()->registerCreator(game_type, this);
	}

	GameHandler *create() {
		ShanXiGameHandler *gh = new ShanXiGameHandler();
		gh->setup_feature_toggles(game_type);
		return gh;
	}
};

#define XX(k, v, player_count) ShanXiGameHandlerCreator g_ShanXiGameHandlerCreator__##k(k);
ShanXi_GAME_TYPE_MAP(XX)
#undef XX


//struct MajinagTuiTongZiGameHandler : GameHandler
//{

//};

//DECLARE_GAME_HANDLER_CREATOR(20001, MajinagTuiTongZiGameHandler);
