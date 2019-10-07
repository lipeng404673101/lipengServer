

#include "malgo.h"

// 洪洞王牌的7小对不带王牌，只当原来的牌使用
static FANXING hongtongwangpai_check_qixiaodui(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	// 七小对得14张牌，有杠、碰不行的
	if (num_huns + stats.total != 14) return FAN_NONE;
	if (num_huns % 2 != 0) return FAN_NONE;
	bool has4 = false; // 检测豪华七小对用
	bool ge6_or_zi_has_hun = false;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		if (zone_size % 2) return FAN_NONE;
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] % 2) {			
				return FAN_NONE;			 
			}
		
		}
	}
	
	// 七小队肯定是单吊，这里自动附加上，外面不需要的话会被外面消掉的	
	return FAN_QiXiaoDui | FAN_DiaoZhang;
}

static FANXING check_shisanyao(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	// 十三幺得14张牌，有杠、碰不行的
	if (num_huns + stats.total != 14) return FAN_NONE;
	tile_mask_t mask = stats.mask;
	tile_mask_t intersection = mask & TILE_MASK_SHISANYAO;
	tile_mask_t diff = mask & ~TILE_MASK_SHISANYAO;
	if (mjalgo_popcnt(intersection) + num_huns >= 13 && diff == 0) {
		if (conf & FAN_MUST_KouDian) {
			// last_tile 为 0 说明最后一张是万能牌，也就是 num_huns 肯定得大于 0
			if (last_tile == 0) {
				tile_mask_t koudian_hupai = (conf & FAN_ZiMo) ? TILE_MASK_KouDian_ZiMo : TILE_MASK_KouDian_ShouPao;
				// 缺的牌得用万能牌去补啊，如果这部分牌不符合抠点那个要求，抠点就胡不了了
				tile_mask_t lack = TILE_MASK_SHISANYAO & ~intersection;
				if ((lack & koudian_hupai) == 0) return FAN_NONE;
			}
		}
		return FAN_ShiSanYao;
	}
	return FAN_NONE;
}


static FANXING check_qixiaodui(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	// 七小对得14张牌，有杠、碰不行的
	if (num_huns + stats.total != 14) return FAN_NONE;
	bool has4 = false; // 检测豪华七小对用
	bool ge6_or_zi_has_hun = false;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		if (zone_size % 2 && 0 == num_huns) return FAN_NONE;
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] % 2) {
				if (0 == num_huns) return FAN_NONE;
				--num_huns;
				if (conf & FAN_MUST_KouDian) {
					unsigned min_limit = (conf & FAN_ZiMo) ? 2 : 5; // 自摸3点，收炮6点
					if (tc == TILE_COLOR_ZI || i >= min_limit) ge6_or_zi_has_hun = true;
				}
			}
			if (a[i] == 4 || a[i] == 3) has4 = true;
		}
	}
	if (conf & FAN_MUST_KouDian) {
		// 最后一张是万能牌，并且没有剩余的万能牌了，并且前面的万能牌也没能分配到高点数上去
		if (last_tile == 0 && 0 == num_huns && ge6_or_zi_has_hun == false) return FAN_NONE;
	}
	// 七小队肯定是单吊，这里自动附加上，外面不需要的话会被外面消掉的
	if (has4) return FAN_HaoHuaQiXiaoDui | FAN_DiaoZhang;
	return FAN_QiXiaoDui | FAN_DiaoZhang;
}

static FANXING check_yitiaolong_in_color(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch, TILE_COLOR tc) {
	if (tc == TILE_COLOR_ZI) return FAN_NONE;
	bool ge6_or_zi_has_hun = false;
	unsigned *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
	for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
		if (a[i]) {
			--a[i];
			--stats.total;
			--stats.count_by_color[tc - TILE_COLOR_MIN];
			// stats.mask 不用处理，后面的平胡判定用不到
		} else {
			if (0 == num_huns) return FAN_NONE;
			// 缺的用混牌补
			--num_huns;
			if (conf & FAN_MUST_KouDian) {
				unsigned min_limit = (conf & FAN_ZiMo) ? 2 : 5; // 自摸3点，收炮6点
				if (tc == TILE_COLOR_ZI || i >= min_limit) ge6_or_zi_has_hun = true;
			}
		}
	}
	// 提前将胡牌是万能牌，要6点以上的需求在这里解决了，就不需要要求后面的了
	if (last_tile == 0 && ge6_or_zi_has_hun == true) conf = (FANXING)(conf & ~FAN_MUST_KouDian);
	HuBranch branch;
	FANXING fans = malgo_can_pinghu(num_huns, stats, last_tile, conf, branch);
	if (fans) {
		selected_branch.extend(branch);
		fans |= FAN_YiTiaoLong;
	}
	return fans;
}

static FANXING check_yitiaolong(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch) {
	// (123)(456)(789)
	// 只有同一花色的123456789才算
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		// 一条龙检测会修改状态，所以需要一个临时副本
		TileZoneDivider tmp_stats(stats);
		FANXING fans = check_yitiaolong_in_color(num_huns, tmp_stats, last_tile, conf, selected_branch, tc);
		if (fans) return fans;
	}
	return FAN_NONE;
}

static FANXING check_pengpenghu(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {

	//手中至少5张牌
	if (stats.total < 5) 
	{
		return FAN_NONE;
	}
	// 全是刻子
	++num_huns; // 补一张万能牌，这样可以把将子当成刻子来检测
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			size_t needed_num_huns = (3 - a[i] % 3) % 3;
			if (num_huns < needed_num_huns) return FAN_NONE;
			num_huns -= needed_num_huns;
		}
	}

	//最后一张牌必须组成刻子，不能是将对
	if (stats.count_by_color_by_number[tile_color_index(last_tile)][tile_to_number(last_tile) - 1] < 3)
		return FAN_NONE;
	return FAN_PengPengHu;
}

FANXING malgo_judge_big_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch) {
	if (conf & FAN_ShiSanYao) {
		FANXING fans = check_shisanyao(num_huns, stats, last_tile, conf);
		if (fans) return fans;
	}
	if ( (conf & FAN_QiXiaoDui) && (conf & FAN_HongTong_YingKou))
	{
		FANXING fans = hongtongwangpai_check_qixiaodui(num_huns, stats, last_tile, conf);
		if (fans) return fans;
	}
	else
	{
		if (conf & FAN_QiXiaoDui) {
			FANXING fans = check_qixiaodui(num_huns, stats, last_tile, conf);
			if (fans) return fans;
		}
	}
	// 带万能牌的一条龙检测放到平胡算法里面很难计算，所以做成独立的提前检测
	if (conf & FAN_YiTiaoLong) {
		FANXING fans = check_yitiaolong(num_huns, stats, last_tile, conf, selected_branch);
		if (fans) return fans;
	}
	// 碰碰胡要优先搜索刻子，如果失败，会影响边坎吊的检测，所以独立出来提前检测
	if (conf & FAN_PengPengHu) {
		FANXING fans = check_pengpenghu(num_huns, stats, last_tile, conf);
		if (fans) return fans;
	}
	return FAN_NONE;
}





