

#include "malgo.h"



static bool can_pinghu__try_jiang_at(TILE_COLOR tc, tile_t last_tile, unsigned const *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (a[i] < 2) return false;
	branch.clear();
	unsigned tmp[TILE_NUMBER_COUNT];
	memcpy(tmp, a, sizeof(tmp));
	tmp[i] -= 2;
	return malgo_can_zheng(tc, last_tile, tmp, branch, conf);
}


FANXING malgo_unhun_can_pinghu(TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch) {
	unsigned num_tiles = stats.total;
	if ((num_tiles) % 3 != 2) return FAN_NONE;

	// 查找将所在的颜色
	TILE_COLOR jiang_tc = TILE_COLOR_NULL;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		if (zone_size % 3 == 2) jiang_tc = tc;
		else if (zone_size % 3 != 0) return FAN_NONE;
	}
	if (TILE_COLOR_NULL == jiang_tc) return FAN_NONE;

	// 测试非将区是不是整切
	// 注意这种测试会把牌面搞乱，后面不能再使用这个牌面了
	HuBranch branch_by_color[TILE_COLOR_COUNT];
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		if (tc == jiang_tc) continue;
		unsigned *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		if (!malgo_can_zheng(tc, last_tile, a, branch_by_color[tc - TILE_COLOR_MIN], conf)) return FAN_NONE;
	}

	unsigned *jiang_a = stats.count_by_color_by_number[jiang_tc - TILE_COLOR_MIN];
	HuBranch &jiang_branch = branch_by_color[jiang_tc - TILE_COLOR_MIN];
	// 优先测试胡的那张牌对应的number做将，也甭管它的color了
	// 这样相当于优先搜索单吊将
	unsigned last_tile_number = tile_to_number(last_tile);
	unsigned jiang_number = last_tile_number;
	if (last_tile_number && can_pinghu__try_jiang_at(jiang_tc, last_tile, jiang_a, jiang_branch, conf, last_tile_number - 1)) goto found;
	for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
		if (i == last_tile_number - 1) continue; // 已经优先检测过了
		jiang_number = i + 1;
		if (can_pinghu__try_jiang_at(jiang_tc, last_tile, jiang_a, jiang_branch, conf, i)) goto found;
	}
	return FAN_NONE;

found:
	selected_branch.add(HunItem(HUN0_JIANG, tile_make(jiang_tc, jiang_number)));
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		selected_branch.extend(branch_by_color[tc - TILE_COLOR_MIN]);
	}
	return FAN_PingHu;
}



