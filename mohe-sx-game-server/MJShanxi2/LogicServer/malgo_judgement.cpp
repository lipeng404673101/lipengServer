

#pragma warning(push, 4)
// warning C4100 : “conf”: 未引用的形参
#pragma warning(disable:4100)

#include "malgo.h"
#include "..\mhmessage\mhmsghead.h"



static FANXING plus_common_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	FANXING fans = FAN_NONE;
	return fans;
}

static FANXING plus_pinghu_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	FANXING fans = FAN_NONE;
	return fans;
}


FANXING malgo_can_pinghu(size_t num_huns, TileZoneDivider &saved_stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch) {
	TileZoneDivider stats(saved_stats);
	if (0 == num_huns) return malgo_unhun_can_pinghu(stats, last_tile, conf, selected_branch);
	else return malgo_hun_can_pinghu(num_huns, stats, last_tile, conf, selected_branch);
}

// 比如不能有5个东风
static bool has_invalid_count(TileZoneDivider const &stats) {
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] > 4) return true;
		}
	}
	return false;
}

struct CardCount
{
	unsigned count;
	unsigned index;
	CardCount() : count(0), index(0)
	{}
	CardCount(unsigned c, unsigned i) : count(c), index(i) {}
};

bool comp(const CardCount & a, const CardCount & b)
{
	return a.count > b.count;
}

bool hongtong_search_SanFengShun(unsigned * p, unsigned * pIndex)
{
	std::vector<CardCount> vCardCount;
	for (unsigned i = 0; i < 4; i++)
	{
		vCardCount.push_back(CardCount(p[i], i));
	}
	std::sort(vCardCount.begin(), vCardCount.end(), comp);
	
	unsigned nCount = 0;	 
	//MHLOG("********排序后数据:");
	//MHLOG("Count: %d %d, %d %d", vCardCount[0].count, vCardCount[1].count, vCardCount[2].count, vCardCount[3].count);
	//MHLOG("Index: %d %d, %d %d", vCardCount[0].index, vCardCount[1].index, vCardCount[2].index, vCardCount[3].index);
	if (vCardCount[1].count == 1 && vCardCount[2].count == 1 && vCardCount[3].count == 1)
	{ 
		pIndex[0] = vCardCount[1].index;
		pIndex[1] = vCardCount[2].index;
		pIndex[2] = vCardCount[3].index;
		return true;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (vCardCount[i].count > 0) pIndex[nCount++] = vCardCount[i].index;
			if (nCount == 3)
			{
				return true;
			}
		}
	}
	return false;	 
}
// 洪洞王牌消除三风顺, 任意三风组成的顺子
unsigned hongtongwanpai_spec_remove_SanFengShun(TileZoneDivider & stats, tile_t last_tile, HuBranch & selected_branch)
{
	unsigned shun_count = 0;
	unsigned * p = stats.count_by_color_by_number[TILE_COLOR_ZI - TILE_COLOR_MIN];
	unsigned * pByColor = stats.count_by_color;

	// 消除掉最大数量的三风顺子
	 
	unsigned pI[3] = { 0 };
	bool bHasShun = hongtong_search_SanFengShun(p, pI);	
	 
	while (bHasShun)
	{
		//MHLOG("******************消除风牌测试: bHasHun = %d, index1=%d, index2=%d, index3=%d", bHasShun, pI[0], pI[1], pI[2]);
		p[pI[0]] -= 1; p[pI[1]] -= 1; p[pI[2]] -= 1;  // 消除掉
		pByColor[TILE_COLOR_ZI - TILE_COLOR_MIN] -= 3;
		stats.total -= 3;
		shun_count += 1;
		HunItem item(HUN0_SHUN_SanFeng, last_tile);
		selected_branch.add(item);	 
		bHasShun = hongtong_search_SanFengShun(p, pI);
	}
	if (p[0] < 1) stats.mask &= ~TILE_TO_MASK(41);
	if (p[1] < 1) stats.mask &= ~TILE_TO_MASK(42);
	if (p[2] < 1) stats.mask &= ~TILE_TO_MASK(43);
	if (p[3] < 1) stats.mask &= ~TILE_TO_MASK(44);
	
	return shun_count;
}

// 洪洞王牌消除中发白的顺子
unsigned hongtongwanpai_spec_remove_SanYuanShun(TileZoneDivider & stats, tile_t last_tile, HuBranch & selected_branch)
{ 
	unsigned shun_count = 0;
	unsigned * p = stats.count_by_color_by_number[TILE_COLOR_ZI - TILE_COLOR_MIN];
	unsigned * pByColor = stats.count_by_color;
	while ( (p[4] > 0) && (p[5] > 0) && (p[6] > 0) )     
	{
		p[4] -= 1; p[5] -= 1; p[6] -= 1;  // 消除掉
		pByColor[TILE_COLOR_ZI - TILE_COLOR_MIN] -= 3;
		stats.total -= 3;

		shun_count += 1;
		HunItem item(HUN0_SHUN_SanYuan, last_tile);
		selected_branch.add(item);
	}
	if (p[4] < 1) stats.mask &= ~TILE_TO_MASK(45);
	if (p[5] < 1) stats.mask &= ~TILE_TO_MASK(46);
	if (p[6] < 1) stats.mask &= ~TILE_TO_MASK(47);
	return shun_count;
}

FANXING malgo_can_hu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf
	, tile_mask_t pg_mask, pg_tile_count_by_color_t sum_pg_tile_count_by_color, HuBranch &selected_branch, HuBranch &selected_branch_hongtong) {
	if (has_invalid_count(stats)) return FAN_NONE;	 
	if (conf & FAN_MUST_QueMen) {
		if (!PlayerState::is_quemen(stats.mask | pg_mask)) return FAN_NONE;

		// 然后2张万能牌当将时不破缺门就行了，它肯定可以不破缺门啊，这个函数只是判断能不能胡
	}
	// 洪洞王牌中缺2门的玩法
	if (conf & FAN_HongTong_MustQueLiangMen)
	{
		int que_men_count = 0;
		if (0 == ((stats.mask | pg_mask)  & TILE_MASK_WAN)) ++que_men_count;
		if (0 == ((stats.mask | pg_mask) & TILE_MASK_TONG)) ++que_men_count;
		if (0 == ((stats.mask | pg_mask) & TILE_MASK_TIAO)) ++que_men_count;
		if (que_men_count < 2)
		{
			return FAN_NONE;
		}
	}

	if (conf & FAN_MUST_YiMenGouBaZhang) {
		bool has_big_men = false;
		for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
			unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
			zone_size += sum_pg_tile_count_by_color[tc - TILE_COLOR_MIN];			 
			if (zone_size >= 8) {
				has_big_men = true;
				break;
			}
		}
		if (!has_big_men) return FAN_NONE;
	}
	if (last_tile) {
		// 最后一张牌不是万能牌
		if (conf & FAN_MUST_KouDian) {
			unsigned min_points = (conf & FAN_ZiMo) ? 3 : 6; // 抠点胡牌限制：自摸点数3，收炮点数6
			if (tile_to_points(last_tile) < min_points) return FAN_NONE;
			// 然后清掉这个标记就行了，就不应该再要求后面的了
			conf = (FANXING)(conf & ~FAN_MUST_KouDian);
		}
	} else {
		// 最后是万能牌，对于 FAN_MUST_KouDian 还需要后续检测
	}
	 
	FANXING fans = malgo_judge_big_fans(num_huns, stats, last_tile, conf, selected_branch);	
	if (fans&FAN_PengPengHu) fans |= FAN_PingHu;     //晋中，碰碰胡算是平胡

	// 原始判断平胡逻辑（有大胡后不走这里，所以一条龙的边卡吊就检测不出来了）
	if (FAN_NONE == fans && (conf & FAN_PingHu)) {
		
		fans = malgo_can_pinghu(num_huns, stats, last_tile, conf, selected_branch);
	}

	// 如果判断边卡吊，需要单独再走一遍平胡的判断(**Ren 2017-12-5)
	if ((conf & FAN_BianKanDiao) && (fans & FAN_YiTiaoLong))
	{
		TileZoneDivider temp_stats_biankadiao(stats);
		HuBranch temp_branch_biankadiao;
		malgo_can_pinghu(num_huns, temp_stats_biankadiao, last_tile, conf, temp_branch_biankadiao);
		fans |= judge_biankandiao(temp_branch_biankadiao, last_tile, conf);	
	}
	
	FANXING fan_hongtong_diaowang = FAN_NONE;
	if ( fans && (conf & FAN_HongTong_DiaoWang))
	{
		if (num_huns > 0)
		{
			TileZoneDivider temp_stats(stats);
			HuBranch temp_branch;
			fan_hongtong_diaowang = hongtong_judge_diaowang(num_huns, temp_stats, last_tile, conf, temp_branch);			
		}
	}


	// 如果胡了，就把额外标记也给加上
	if (fans) {
		fans |= plus_common_fans(num_huns, stats, last_tile, conf);
		if (fans & FAN_PingHu) fans |= plus_pinghu_fans(num_huns, stats, last_tile, conf);
		fans |= fan_hongtong_diaowang;
	}

	return (FANXING)fans;
}



HuJudgement::HuJudgement(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf, tile_mask_t pg_mask
	, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color)
	: num_huns(num_huns), hands(hands), last_tile(last_tile), conf(conf)
	, pg_mask(pg_mask), sum_pg_tile_count_by_color(sum_pg_tile_count_by_color)
	, stats(hands.begin(), hands.size()), all_mask(stats.mask | pg_mask) {

	//for (int i = 0; i < hands.size(); i++)
	//{
		//LLOG_DEBUG("*******************hand card i=%d, card = %d", i, hands[i]);
	//}
	TileZoneDivider tmp_stats(stats);
	fans = malgo_can_hu(num_huns, tmp_stats, last_tile, conf, pg_mask, sum_pg_tile_count_by_color, selected_branch, selected_branch_hongtong);	
	//MHLOG("*******************malgo_can_hu output fans: %lld lasttile= %d, num_huns = %d", fans, last_tile, num_huns);

	//陵川麻将胡牌后检测主牌(最大门数) 的牌数量是否满足靠八张条件，满足则计算最后分数，否则不能胡牌
	if ( (conf & FAN_LingChuan_Must_KaoBaZhang) && (fans)) {
		if (fans & FAN_ShiSanYao)
		{
			ps.lingchuan_loss_score = 14; // 十三幺分数固定为14分
		}
		else
		{
			pg_tile_count_by_color_t pg_count_by_color = ps.sum_pg_tile_count_by_color_for_lingchuanmajiang();
			TILE_COLOR tc = TILE_COLOR_MIN;
			unsigned zone_size = 0;
			bool has_big_men = false;
			for (; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
				zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];				
				if ((zone_size + pg_count_by_color[tc - TILE_COLOR_MIN]) >= 8) {
					has_big_men = true;
					break;
				}
			}
			if (has_big_men)
			{
				ps.calc_last_score_for_lingchuanmajiang(tc,zone_size);
			}
			else      //不满足靠八张的条件，不能胡牌
			{
				fans = FAN_NONE;
			}
		}
	}
	if (fans) fans = later_stage(ps, fans);	
}





#pragma warning(pop)







