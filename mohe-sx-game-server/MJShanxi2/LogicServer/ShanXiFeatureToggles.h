#pragma once

// 胡牌算法
#include "malgo.h"
// 玩法编号
#include "GameDefine.h"
// 创建房间的小选项
#include "ClientConstants.h"



// 不同的玩法，如推到胡、抠点、立四存在一些小差异，我们在基类中给一些默认值
// 然后这些玩法子类可在构造函数中修改这些默认值
struct ShanXiFeatureToggles {
	int m_registered_game_type;
	int FEAT_GAME_TYPE; // 游戏玩法：推到胡、抠点、立四
	int FEAT_DaiFeng : 1; // 默认都是带风的，推到胡需要玩家自定义
	int FEAT_BaoTing : 1; // 强制报听，立四需要
	int FEAT_TingPaiKeGang : 1; // 听牌可杠
	int FEAT_OnlyQueMenCanBaoTing : 1; // 只有缺门才能报听，晋中需要
	int FEAT_ZhiYouHuPaiWanJiaGangSuanFen : 1; // 只有胡牌玩家杠算分
	int FEAT_DianGangBaoGang : 1; // 点杠包杠做成默认不开启的
	int FEAT_HunPaiBuKeGang : 1; // 混牌不可杠，用于贴金
	int FEAT_LianZhuangJiaFen : 1; // 连庄加分，拐三角需要
	int FEAT_GuoHuChuPaiQianKeShouPao : 1; // 过胡出牌前可收炮，平鲁需要
	int FEAT_ZhiKeZiMoHuPai : 1; // 只可自摸胡牌，就是不能收炮的意思
	int FEAT_TingPaiHouBuNengGuoHu : 1; // 听牌后不能过胡，目前是客户端处理的，不显示“过”按钮
	int FEAT_YiPaoDuoXiang : 1; // 一炮多响，目前都是小选项启用的，还没有固定启用的
	int FEAT_QiangGangHu : 1; // 抢杠胡
	int FEAT_WeiShangTingGangBuSuanFen : 1; // 未上听杠不算分
	int FEAT_TingPaiBuBaoDianGang : 1; // 听牌不包点杠
	int FEAT_DianPaoDanRenKouFen : 1; // 点炮单人扣分
	int FEAT_DealerRiskScoreDisabled : 1; // 关闭庄算分，然后好通过小选项控制
	int FEAT_DisableZiMoDoubleScore : 1; // 庄算分时禁止自摸双倍
	int FEAT_DianGangDanRenKouFen : 1; // 拐三角使用，点明杠只扣单人分
	int FEAT_WeiShangTingDianPaoBaoGang : 1;  // 未上听点玩家点炮包杠
	unsigned char FEAT_SUO_COUNT; // 锁几张牌，立四需要
	unsigned char FEAT_FangPaoExtraLoss;      // 未上听点炮额外扣分
	unsigned char FEAT_FangPaoExtraLoss_TingHou; // 听后点炮额外扣分，如果不分听前听后，这个和FEAT_FangPaoExtraLoss都设置相同值
	// 庄风险分，赢了多赢，输了多输，受自摸二倍影响，自摸赢了多赢二倍风险分，输了同理，但不受其它加倍影响
	unsigned char FEAT_DealerRiskScore;
	// 上面的是功能开关，由构造函数确定
	// 以下都是 MatchState，在 clear_match 时清空
	GameOptionSet m_playtype;
	FANXING m_allowed_fans; // 番型配置，需要玩法和小选项共同确定
	int m_lianzhuang_jiafen;
	int m_player_count; // 玩法核心代码所使用的玩家数量字段
	Lint m_zhuangpos; //庄家位置
	Lint m_remain_card_count; // 桌子上剩下多少張牌算荒莊

	ShanXiFeatureToggles() {
		memset(this, 0, sizeof(*this));
		FEAT_DaiFeng = 1;
		FEAT_TingPaiKeGang = 1; // 默认听牌可杠，然后晋中给禁用
		FEAT_FangPaoExtraLoss = 0;
		FEAT_FangPaoExtraLoss_TingHou = 0;
	}

	void clear_match() {
		m_playtype.clear();
		m_lianzhuang_jiafen = 0;
		m_player_count = 0;
		m_zhuangpos = 0;
		m_remain_card_count = 0;
	}

	int GetNextPos(int pos) {
		if (m_player_count == 0) return 0; // 避免除零崩溃
		return (pos + 1) % m_player_count;
	}

	// 合并玩法设定的功能开关与创建房间时指定的小选项
	// 也可能处理多个小选项指定同一个事的问题
	bool has_YiPaoDuoXiang() {
		return FEAT_YiPaoDuoXiang || m_playtype[PT_YiPaoDuoXiang];
	}
	bool has_QiangGangHu() {
		return FEAT_QiangGangHu || m_playtype[PT_QiangGangHu];
	}
	bool has_WeiShangTingGangBuSuanFen() {
		return FEAT_WeiShangTingGangBuSuanFen || m_playtype[PT_WeiShangTingGangBuSuanFen];
	}
	int get_DealerRiskScore() {

		if (FEAT_DealerRiskScore
			&&  !FEAT_DealerRiskScoreDisabled  && m_playtype[PT_ZhuangJiaYiFen]) {
			return FEAT_DealerRiskScore+1;
		}
		if (FEAT_DealerRiskScore
			&& (!FEAT_DealerRiskScoreDisabled || m_playtype[PT_ZhuangSuanFen] )) {
			return FEAT_DealerRiskScore;
		}
		return 0;
	}



	void setup_feature_toggles(int game_type);
	void init_allowed_fans();
	void vectorize_fans(FANXING fans, std::vector<Lint> &vec);
	tile_mask_t calc_hu_candidate_set(tile_mask_t mask) const;
	FANXING call_judge(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf
		, tile_mask_t pg_mask, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color) const;
	FANXING reduce_fans(FANXING fans, FANXING const conf) const;
	FANXING reduce_fans_in_TingKou_stage(FANXING fans, FANXING const conf) const;
	int calc_zui_score(FANXING fans) const;
	//// 硬三嘴 在胡的番型上为七小对，特殊的计算嘴数的函数  by wyz
	int calc_zui_score_spec_qixiaodui(TileVec const &hands, tile_mask_t pg_mask) const;
	int calc_common_loss(FANXING fans, tile_t last_tile, unsigned max_zone_card_count = 0) const;
	int calc_loss_factor(FANXING fans);

	int calc_next_zhuangpos(int curpos, int winpos, int total_gangs);
	int init_desk_card_remain_count();

};













