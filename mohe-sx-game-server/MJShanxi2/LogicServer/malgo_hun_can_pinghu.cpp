

#include "malgo.h"



static bool hun_can_zheng(size_t remaining_huns, unsigned a[TILE_NUMBER_COUNT], bool is_zipai, tile_t last_tile, FANXING conf, unsigned jiang_pos, HuBranch & branch) {
	size_t needed_huns = 0;
	bool bHongTongWangPai = (conf & FAN_HongTong_YingKou) ? true : false;	
	if (bHongTongWangPai && is_zipai)
	{
		needed_huns = tile_min_huns_for_hongtongwangpai_zipai_sanyuan_sanfeng(a, 0, 8, jiang_pos, last_tile, branch);
	}
	else
		needed_huns = malgo_min_huns(a, is_zipai, jiang_pos);

	if (remaining_huns < needed_huns) return false;
	if (conf & FAN_MUST_KouDian) {
		if (remaining_huns && last_tile == 0) {
			// 需要验证一下万能牌的取值范围和 TILE_MASK_KOUDIAN_HUPAI 有没有交集
			// 这个算法是遍历抠点限定的胡牌范围，将一张万能牌换成这个具体牌，看看能不能整切
			conf = (FANXING)(conf & ~FAN_MUST_KouDian);
			remaining_huns -= 1;
			unsigned head = 0;
			unsigned tail = 6;
			if (!is_zipai) {
				head = 5;
				tail = 8;
				if (conf & FAN_ZiMo) head = 2;
			}
			for (unsigned i = head; i <= tail; ++i) {
				a[i] += 1;
				if (hun_can_zheng(remaining_huns, a, is_zipai, last_tile, conf, jiang_pos, branch)) return true;
				a[i] -= 1;
			}
			return false;
		}
	}
	return true;
}

// 洪洞王牌添加了HuBranch参数，用来存储三风，三元的数量
// 添加了字牌中检测三风，三元的功能
FANXING malgo_hun_can_pinghu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch & branch) {
	unsigned num_tiles = stats.total;
	if ((num_huns + num_tiles) % 3 != 2) return FAN_NONE;
	unsigned min_huns_by_color[TILE_COLOR_COUNT];
	unsigned total_min_huns = 0;
	bool too_big_zone_exists = false;
	HuBranch tempBranch;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		bool is_zipai = TILE_COLOR_ZI == tc;
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		// 同区的话有13或者14张牌，那肯定得把将安置到那里，再去计算最小万能牌数就没意义了
		bool is_too_big_zone = zone_size >= 13;
		if (is_too_big_zone) too_big_zone_exists = true;
		// jiang_pos 999 表示将不在这里
		if (is_zipai && (conf & FAN_HongTong_YingKou)) //洪洞王牌的字牌处理
		{
			total_min_huns += min_huns_by_color[tc - TILE_COLOR_MIN] = is_too_big_zone ? 999 : tile_min_huns_for_hongtongwangpai_zipai_sanyuan_sanfeng(a, 0, 8, 999, last_tile, tempBranch);
		}
		else {
			total_min_huns += min_huns_by_color[tc - TILE_COLOR_MIN] = is_too_big_zone ? 999 : malgo_min_huns(a, is_zipai, 999);
		}

	}

	// 万能牌足够多的话肯定能胡
	// 这种情况肯定符合抠点的要求 FAN_MUST_KouDian	 
	if (num_huns > total_min_huns)
	{
		if (conf & FAN_HongTong_DiaoWang) //洪洞王牌掉王牌型
		{
			branch.extend(tempBranch);			
			return FAN_PingHu;
		}
		else
			return FAN_PingHu;
	}



	// 逐个测试将在哪个区
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		bool is_too_big_zone = zone_size >= 13;
		if (is_too_big_zone) {
			// 将肯定得在这区
		} else {
			// 如果存在特大区，将肯定不在这个区
			if (too_big_zone_exists) continue;
		}

		// 其余3区所需的最小万能牌数目之和
		unsigned this_min_huns = min_huns_by_color[tc - TILE_COLOR_MIN];
		unsigned other_min_huns = total_min_huns - this_min_huns;
		// 现有万能牌数目如果都不够其余3区所需，那肯定胡不了
		if (num_huns < other_min_huns) continue;
		// 把万能牌给其它3区分配后，还剩多少
		size_t remaining_huns = num_huns - other_min_huns;
		// 通过数目判断将是否适合在这个区
		if ((remaining_huns + zone_size) % 3 != 2) continue;

		// 逐个测试万能牌投入到将中的数目
		// 两个万能牌当将行不行？
		// 这种情况肯定符合抠点的要求 FAN_MUST_KouDian
		if (remaining_huns >= 2 && remaining_huns - 2 >= this_min_huns) return FAN_PingHu;

		// 一个万能牌当将行不行？
		unsigned *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		bool is_zipai = TILE_COLOR_ZI == tc;

		if (remaining_huns >= 1 && zone_size < 14) {

			// 洪洞王牌吊王,优先配掉最后一张摸上牌为将,如果可以胡则加上吊王的番
			if (conf & FAN_HongTong_DiaoWang)
			{
				if (last_tile != 0)
				{
					if (tile_to_color(last_tile) == tc)
					{
						unsigned index = tile_to_number(last_tile) - 1;
						a[index] -= 1; if (is_zipai) tempBranch.clear();
						bool b = hun_can_zheng(remaining_huns - 1, a, is_zipai, last_tile, conf, index, tempBranch);
						a[index] += 1;
						if (b)
						{							
							branch.extend(tempBranch);
							return  FAN_PingHu;					 
						}
					}
				}
			}
			for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
				if (a[i] >= 1) {
					a[i] -= 1; if (is_zipai) tempBranch.clear();
					bool ok = hun_can_zheng(remaining_huns - 1, a, is_zipai, last_tile, conf, i, tempBranch);
					a[i] += 1;
					if (ok) { 						
						branch.extend(tempBranch); 
						return  FAN_PingHu; }
				}
			}
		}

		// 不用万能牌当将行不行
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] >= 2) {
				a[i] -= 2; if (is_zipai) tempBranch.clear();
				bool ok = hun_can_zheng(remaining_huns, a, is_zipai, last_tile, conf, i, tempBranch);
				a[i] += 2; 
				if (ok) { 					
					branch.extend(tempBranch); 
					return FAN_PingHu; }
			}
		}
	}
	return FAN_NONE;
}

FANXING hongtong_judge_diaowang(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch & branch) {
	if (num_huns < 1) return FAN_NONE;
	unsigned num_tiles = stats.total;
	if ((num_huns + num_tiles) % 3 != 2) return FAN_NONE;
	unsigned min_huns_by_color[TILE_COLOR_COUNT];
	unsigned total_min_huns = 0;
	TILE_COLOR last_tile_color = last_tile == 0 ? TILE_COLOR_NULL : tile_to_color(last_tile);
	unsigned   last_tile_number = last_tile == 0 ? 0 : tile_to_number(last_tile);

	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		bool is_zipai = TILE_COLOR_ZI == tc;
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		if (last_tile != 0 && last_tile_color == tc)
		{
			a[last_tile_number - 1]--;
		}
	 
		// jiang_pos 999 表示将不在这里
		if (is_zipai && (conf & FAN_HongTong_YingKou)) //洪洞王牌的字牌处理
		{
			total_min_huns += min_huns_by_color[tc - TILE_COLOR_MIN] =  tile_min_huns_for_hongtongwangpai_zipai_sanyuan_sanfeng(a, 0, 8, 999, last_tile, branch);
			if (num_huns <= total_min_huns) return FAN_NONE;
		}
		else {
			total_min_huns += min_huns_by_color[tc - TILE_COLOR_MIN] =  malgo_min_huns(a, is_zipai, 999);
			if (num_huns <= total_min_huns) return FAN_NONE;
		}
		if (last_tile != 0 && last_tile_color == tc)
		{
			a[last_tile_number - 1]++;
		}

	}
	//MHLOG("********last_tile=%d num_huns=%d total_min_huns=%d", last_tile, num_huns, total_min_huns);
	// 万能牌足够多的话肯定能胡
	if ( num_huns > total_min_huns) // 
	{
	  return FAN_HongTong_DiaoWang;
	}
	return FAN_NONE;
}

