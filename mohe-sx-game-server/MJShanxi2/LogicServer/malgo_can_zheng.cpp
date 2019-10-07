

#include "malgo.h"



// 检测边缘一张牌能不能和后面的牌成为顺子，行的话就剔除这个顺子
static bool malgo_can_zheng__remove_shunzi(TILE_COLOR tc, unsigned *a, HuBranch &branch, unsigned i) {
	if (i + 2 >= TILE_NUMBER_COUNT) return false;
	if (a[i + 1] == 0 || a[i + 2] == 0) return false;
	a[i] -= 1;
	a[i + 1] -= 1;
	a[i + 2] -= 1;
	branch.add(HunItem(HUN0_SHUN, tile_make(tc, i + 1)));
	return true;
}

// 已经知道边缘3张牌是刻子，剔除它们
static void malgo_can_zheng__remove_kezi(TILE_COLOR tc, unsigned *a, HuBranch &branch, unsigned i) {
	a[i] -= 3;
	branch.add(HunItem(HUN0_KE, tile_make(tc, i + 1)));
}

static bool malgo_can_zheng__shupai(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i);

// 走到这里，是说边缘是1张相同的牌
static bool malgo_can_zheng__on_1(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__shupai(tc, a, branch, conf, i);
}

// 走到这里，是说边缘是2张相同的牌
static bool malgo_can_zheng__on_2(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__shupai(tc, a, branch, conf, i);
}

static bool malgo_can_zheng__on_3__try_shunzi(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__shupai(tc, a, branch, conf, i);
}

// 走到这里，是说边缘是3张相同的牌
static bool malgo_can_zheng__on_3(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	// 在需要分支的时候才做副本
	unsigned saved_a[TILE_NUMBER_COUNT];
	memcpy(saved_a, a, sizeof(saved_a));
	malgo_can_zheng__remove_kezi(tc, a, branch, i);
	// 把边缘3张做为刻子用，剩下的牌胡不了的话，那边缘9张当3个顺子用肯定也胡不了
	// 因此只需要检测一种就行了
	// 立四优先用顺子，这样能搜到最优解
	bool kezi_checked = false;
	bool shunzi_checked = false;
	if (conf & FAN_BianKanDiao) goto check_shunzi;
check_kezi:
	if (malgo_can_zheng__shupai(tc, a, branch, conf, i + 1)) return true;
	kezi_checked = true;
	if (shunzi_checked) return false;
check_shunzi:
	if (malgo_can_zheng__on_3__try_shunzi(tc, saved_a, branch, conf, i)) return true;
	shunzi_checked = true;
	if (!kezi_checked) goto check_kezi;
	return false;
}

// 走到这里，是说边缘是4张相同的牌
static bool malgo_can_zheng__on_4(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__on_3(tc, a, branch, conf, i);
}

// 能不能按面子(刻子、顺子)完整拆分
// 并按conf指定的搜索顺序选定一个路径输出
// 整体架构是在有序牌向量上，从起始端开始判断，不可能胡就提前返回了，有可能胡就继续深入判断
// 用递归实现的深度优先搜索
static bool malgo_can_zheng__shupai(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	// 如果是空的，说明能按面子完整拆分
	while (i < TILE_NUMBER_COUNT && 0 == a[i]) ++i;
	if (i >= TILE_NUMBER_COUNT) return true;
	unsigned count = a[i];
	// 判断3n张牌是否能组成顺子或者是暗刻，是通过判断边缘牌的数量：
	// (1) 如果是1张或2张，要胡牌的话，它必须和后面的牌组成顺子，否自不能胡。
	// (3) 如果是3张，要胡牌有两种可能，一种是把3张当做是暗刻，另一种是这3张都与后面的牌组成顺子。
	//	     如果只是判断胡不胡牌，这两种任意一种都是一样的，如果要计算分数，就要自己分开算了。
	// (4) 如果是4张，要胡牌的话，有1张要和后面的牌组成顺子，剩下的3张牌又可以继续(3)的操作。
	switch (count) {
	case 1: return malgo_can_zheng__on_1(tc, a, branch, conf, i);
	case 2: return malgo_can_zheng__on_2(tc, a, branch, conf, i);
	case 3: return malgo_can_zheng__on_3(tc, a, branch, conf, i);
	case 4: return malgo_can_zheng__on_4(tc, a, branch, conf, i);
	}
	return false;
}

static bool malgo_can_zheng__FengPai(tile_t last_tile, unsigned *a, HuBranch &branch) {
	for (unsigned i = 0; i < 4; ++i) {
		if (a[i] % 3 != 0) return false;
		if (a[i]) branch.add(HunItem(HUN0_KE, tile_make(TILE_COLOR_ZI, i + 1)));
	}
	return true;
}

static bool malgo_can_zheng__JianPai(tile_t last_tile, unsigned *a, HuBranch &branch) {
	for (unsigned i = 4; i < 7; ++i) {
		if (a[i] % 3 != 0) return false;
		if (a[i]) branch.add(HunItem(HUN0_KE, tile_make(TILE_COLOR_ZI, i + 1)));
	}
	return true;
}

static bool all_have(unsigned *a, unsigned n, unsigned count) {
	for (unsigned i = 0; i < n; ++i) {
		if (a[i] < count) return false;
	}
	return true;
}
static void all_decrease(unsigned *a, unsigned n, unsigned count) {
	for (unsigned i = 0; i < n; ++i) a[i] -= count;
}

static int malgo_can_zheng__SanFeng__select_maximized_branch(tile_t last_tile, unsigned *a, HuBranch &branch, unsigned i, unsigned max_left);
static int malgo_can_zheng__SanFeng__go_this_branch(tile_t const last_tile, unsigned *a, HuBranch &branch, unsigned const i, unsigned const n, unsigned const max_left) {
	// 具体到某个位置的某种三风数，检测是否存在，如果不存在，说明这个分支无效，可以立即返回 -999
	if (n) {
		// 在目标位置上虚拟n个风，检测四风是否有n个
		a[i] += n;
		if (all_have(a, 4, n)) {
			all_decrease(a, 4, n);
		} else {
			a[i] -= n;
			return -999;
		}
	}
	// 看下一个位置上的5种三风数可能性
	// 如果没有下一个位置了，就看剩余的牌能不能整切
	// 如果没有剩余的三风数可去测试了，也要看剩余的牌能不能整切
	unsigned total = n;
	if (i + 1 >= 4 || n == max_left) {
		// 剩余的能整切就好，不然就返回一个大负数，表示这个分支无效
		if (!malgo_can_zheng__FengPai(last_tile, a, branch)) return -999;
	} else {
		// 根据上面的判断，到这里肯定还有剩余的三风数可供测试，那就继续看下一个位置
		total += malgo_can_zheng__SanFeng__select_maximized_branch(last_tile, a, branch, i + 1, max_left - n);
	}

	// 确定这个分支是有效的，并且有三风，才填充 HuBranch
	if (total > 0) {
		branch.add_n(n, HunItem(HUN0_SHUN_SanFeng, tile_make(TILE_COLOR_ZI, i + 1)));
	}
	return total;
}

static int malgo_can_zheng__SanFeng__select_maximized_branch(tile_t last_tile, unsigned *a, HuBranch &branch, unsigned i, unsigned max_left) {
	int counts[5];
	HuBranch branches[5];
	for (unsigned n = 0; n <= max_left; ++n) {
		unsigned a_copy[4];
		memcpy(a_copy, a, sizeof(a[0]) * 4);
		counts[n] = malgo_can_zheng__SanFeng__go_this_branch(last_tile, a_copy, branches[n], i, n, max_left);
	}
	size_t n = std::max_element(counts, counts + max_left + 1) - counts;
	int count = counts[n];
	if (count >= 0)	branch.extend(branches[n]);
	return count;
}

static bool malgo_can_zheng__SanFeng(tile_t last_tile, unsigned *a, HuBranch &branch) {
	// 任意不一样的3个风就为一嘴，当成一个顺子，能胡牌用的
	// 4个位置，每个位置有01234个三风这5种可能性，总的三风数最多是4个，搜索目标是三风数目最大化
	int count = malgo_can_zheng__SanFeng__select_maximized_branch(last_tile, a, branch, 0, 4);
	// 负值表示不能整切，正值表示这个分支上有几个三风
	return count >= 0;
}

static bool malgo_can_zheng__SanYuan(tile_t last_tile, unsigned *a, HuBranch &branch) {
	while (all_have(a + 4, 3, 1)) {
		all_decrease(a + 4, 3, 1);
		branch.add(HunItem(HUN0_SHUN_SanYuan, 45));
	}
	return malgo_can_zheng__JianPai(last_tile, a, branch);
}

typedef bool(*ZiPaiHandler)(tile_t last_tile, unsigned *a, HuBranch &branch);
static ZiPaiHandler GetFengPaiHandler(FANXING conf) {
	if (conf & FAN_JUDGE_SanFeng) return malgo_can_zheng__SanFeng;
	return malgo_can_zheng__FengPai;
}
static ZiPaiHandler GetJianPaiHandler(FANXING conf) {
	if (conf & FAN_JUDGE_SanYuan) return malgo_can_zheng__SanYuan;
	return malgo_can_zheng__JianPai;
}

bool malgo_can_zheng(TILE_COLOR tc, tile_t last_tile, unsigned *a, HuBranch &branch, FANXING conf) {
	if (tc == TILE_COLOR_ZI) {
		// 字牌风部和箭部对于整切来说，是无关的，所以分开计算也是没问题的
		return GetFengPaiHandler(conf)(last_tile, a, branch) && GetJianPaiHandler(conf)(last_tile, a, branch);
	}
	return malgo_can_zheng__shupai(tc, a, branch, conf, 0);
}




