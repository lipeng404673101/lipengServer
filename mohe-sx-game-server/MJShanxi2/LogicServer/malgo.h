#pragma once


/*
error C4996 : 'std::_Copy_backward' : Function call with parameters that may be unsafe -
this call relies on the caller to check that the passed values are correct.
To disable this warning, use - D_SCL_SECURE_NO_WARNINGS.See documentation on how to use Visual C++ 'Checked Iterators'
*/
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif
// 上面那种写法必须在标准库包含之前使用，可是有时候标准库已经在该文件之前被包含了
#pragma warning(disable:4996)



#include <algorithm>
#include <vector>
#include <new>
#include <sstream>

#include <cstring>
#include <cstdlib>
#include <cstddef> // ptrdiff_t


// 注意 HuUnit 那里要同步修改
typedef unsigned long long FANXING;

// 这样处理就不用自己去核对常量值了，自动唯一
// 不能用 enum，因为 enum 只有32位，又不能用 C++11
static FANXING const FANXING_COUNTER_BASE = __COUNTER__ + 1;
#define FANXING_DEF(name) static FANXING const name = 1ull << (__COUNTER__ - FANXING_COUNTER_BASE)
#define FANXING_DEF_BY(name, value) static FANXING const name = value

static FANXING const FAN_NONE = 0;


FANXING_DEF(FAN_PingHu); // 平胡
FANXING_DEF(FAN_ShiSanYao); // 十三幺
FANXING_DEF(FAN_HaoHuaQiXiaoDui); // 豪华七小队
FANXING_DEF(FAN_QiXiaoDui); // 七小对
FANXING_DEF(FAN_YiTiaoLong); // 一条龙
FANXING_DEF(FAN_PengPengHu); // 碰碰胡
FANXING_DEF(FAN_FengYiSe); // 风一色
FANXING_DEF(FAN_QingYiSe); // 清一色
FANXING_DEF(FAN_CouYiSe); // 凑一色
FANXING_DEF(FAN_ZiYiSe); // 字一色：和风一色本质一样，不同地方叫法不一样

// 合并特殊番型后的组合叫法
FANXING_DEF(FAN_QingLong); // 青龙
FANXING_DEF(FAN_QingQiDui); // 清七对
FANXING_DEF(FAN_QingHaoHuaQiXiaoDui); // 清豪华七小对

FANXING_DEF(FAN_QueMen); // 缺门
FANXING_DEF(FAN_MenQing); // 门清
FANXING_DEF(FAN_DuanYao); // 断幺：胡牌时手中没有1、9和风牌
FANXING_DEF(FAN_KanHu); // 坎胡
FANXING_DEF(FAN_BianZhang); // 边张
FANXING_DEF(FAN_KanZhang); // 坎张
FANXING_DEF(FAN_DiaoZhang); // 吊张
FANXING_DEF(FAN_DanDiao); // 单吊
// 单吊为硬三嘴新增：吊张并且是一张赢称为单吊

// 缺门：缺一门1嘴，缺两门3嘴，缺三门按字一色算
// 缺门只针对数牌，不考虑字牌的
FANXING_DEF(FAN_QueYiMen); // 缺一门
FANXING_DEF(FAN_QueLiangMen); // 缺两门
// 孤将：1嘴，只有一门牌有一对将：风和中发白不算，手中的筒、饼、万不管哪一门有且只有一对牌就叫孤将
FANXING_DEF(FAN_GuJiang); // 孤将
FANXING_DEF(FAN_YiZhangYing); // 一张赢：就是听口大小为一，需要在听口计算时做检测
// 三风：1副1嘴、两副3嘴、三副10嘴，任意不一样的3个风就为一嘴
FANXING_DEF(FAN_SanFeng_X1); // 三风X1
FANXING_DEF(FAN_SanFeng_X2); // 三风X2
FANXING_DEF(FAN_SanFeng_X3); // 三风X3
FANXING_DEF(FAN_SanFeng_X4); // 三风X4
// 三元：有中、发、白、各一张成的顺，一副在手算1嘴，一副自摸算2嘴；两副在手算5个嘴，两幅自摸算7个嘴；3副在手算50嘴，3副自摸算50个嘴。
// 在手和自摸的区别是13张和14张的区别，13张在手，再自摸一张就是14张了
// 手里有2个中、2个发、1个白，最后摸到一个白，胡了，这个算2副自摸，因此只需检测最后一张牌有没有中发白就可以了
FANXING_DEF(FAN_SanYuan_X1); // 三元X1
FANXING_DEF(FAN_SanYuan_X2); // 三元X2
FANXING_DEF(FAN_SanYuan_X3); // 三元X3
FANXING_DEF(FAN_SanYuan_X4); // 三元X4
FANXING_DEF(FAN_SanYuanZaiShou); // 三元在手：最后摸得牌不是中发白

FANXING_DEF_BY(FAN_JUDGE_QueMen, FAN_QueMen | FAN_QueYiMen | FAN_QueLiangMen | FAN_FengYiSe | FAN_ZiYiSe);
FANXING_DEF_BY(FAN_BianKanDiao, FAN_BianZhang | FAN_KanZhang | FAN_DiaoZhang);
FANXING_DEF_BY(FAN_JUDGE_BianKanDiao, FAN_BianKanDiao);
FANXING_DEF_BY(FAN_JUDGE_SanFeng, FAN_SanFeng_X1 | FAN_SanFeng_X2 | FAN_SanFeng_X3 | FAN_SanFeng_X4);
FANXING_DEF_BY(FAN_JUDGE_SanYuan, FAN_SanYuan_X1 | FAN_SanYuan_X2 | FAN_SanYuan_X3 | FAN_SanYuan_X4 | FAN_SanYuanZaiShou);

//撵中子：（**Ren 2017-11-29）
FANXING_DEF_BY(FAN_HUCHI_QueMen, FAN_QingYiSe | FAN_CouYiSe | FAN_QingLong | FAN_ZiYiSe);

FANXING_DEF(FAN_YiWuJiu); // 一五九

FANXING_DEF(FAN_ZiMo); // 自摸
FANXING_DEF(FAN_Zhuang); // 庄家

// 抠点对最后一张有要求，自摸时要求点数至少是3，收炮时要求点数至少是6
// last_tile为0表示最后捞的牌是万能牌
FANXING_DEF(FAN_MUST_KouDian); // 抠点最后一张牌点数要求
// 晋中要求必须缺门才能胡
FANXING_DEF(FAN_MUST_QueMen); // 必须缺门才能胡
// 拐三角要求必须有一门牌够8张或者以上方可胡牌
// 目前的实现未考虑万能牌
FANXING_DEF(FAN_MUST_YiMenGouBaZhang); // 必须一门够八张才能胡
FANXING_DEF(FAN_MUST_BianKanDiao); // 必须有边张、坎张、或者吊张才能胡

FANXING_DEF(FAN_HongTong_MustQueLiangMen);
// 洪洞王牌楼上楼的番型，要求手中至少1副箭副，胡中发白其中的1张
FANXING_DEF(FAN_HongTong_LouShangLou); // 洪洞楼上楼
FANXING_DEF(FAN_HongTong_YingKou);     // 洪洞硬扣
FANXING_DEF(FAN_HongTong_DiaoWang);    // 洪洞吊王

// 拐三角连庄分
FANXING_DEF(FAN_LianZhuang);

// 陵川麻将靠八张玩法，主牌至少2个副牌加一个将对，十三幺没有这个限制
FANXING_DEF(FAN_LingChuan_Must_KaoBaZhang);

// 七小对和一条龙是互斥的，放在一起算大胡，平胡就是小胡了
// 按这种定义，硬三嘴可能大胡分高，也可能小胡分高，必须两种都检测，然后比较一下分值
FANXING_DEF_BY(FAN_JUDGE_XiaoHu, FAN_PingHu);
FANXING_DEF_BY(FAN_JUDGE_QiDui, FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_QingQiDui | FAN_QingHaoHuaQiXiaoDui);
FANXING_DEF_BY(FAN_JUDGE_YiTiaoLong, FAN_YiTiaoLong | FAN_QingLong);
FANXING_DEF_BY(FAN_JUDGE_DaHu, FAN_JUDGE_QiDui | FAN_JUDGE_YiTiaoLong);

// 比如平胡清一色就不显示平胡
FANXING_DEF_BY(FAN_BIG_EFFECTS, FAN_ShiSanYao | FAN_JUDGE_QiDui | FAN_JUDGE_YiTiaoLong | FAN_FengYiSe | FAN_QingYiSe | FAN_CouYiSe | FAN_ZiYiSe | FAN_PengPengHu | FAN_KanHu);
// 特殊番型比如七小对、清一色时，就得屏蔽掉边坎吊、门清等
FANXING_DEF_BY(FAN_SMALL_EFFECTS, FAN_PingHu | FAN_BianKanDiao | FAN_QueMen | FAN_MenQing | FAN_DuanYao);
// 用户allowed_fans
FANXING_DEF_BY(FAN_BASIC_ALLOWED, FAN_PingHu);


#undef FANXING_DEF
#undef FANXING_DEF_BY


typedef char FANXING_ASSERT_MAX[__COUNTER__ - FANXING_COUNTER_BASE <= 64 ? 1 : -1];









#include "malgo_popcnt.h"
#include "malgo_tile.h"
#include "malgo_structs.h"




FANXING malgo_judge_big_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch);
FANXING malgo_can_pinghu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch);
FANXING malgo_can_hu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf
	, tile_mask_t pg_mask, pg_tile_count_by_color_t sum_pg_tile_count_by_color, HuBranch &selected_branch);


FANXING malgo_unhun_can_pinghu(TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch);

bool malgo_can_zheng(TILE_COLOR tc, tile_t last_tile, unsigned *a, HuBranch &branch, FANXING conf);

unsigned malgo_min_huns(unsigned const p[9], bool is_zipai, unsigned jiang_pos = -1);

unsigned tile_min_huns_for_hongtongwangpai_zipai_sanyuan_sanfeng(unsigned const p[9], unsigned head, unsigned tail, unsigned jiang_pos, tile_t last_tile, HuBranch &branch);

FANXING malgo_hun_can_pinghu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch & branch);
FANXING hongtong_judge_diaowang(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch & branch);

std::string malgo_format_fans(FANXING fans);
//***Ren
FANXING judge_biankandiao(HuBranch const &b, tile_t last_tile, FANXING conf);



/*
隐含约定：
1、last_tile 为 0 表示最后摸到的牌是万能牌
*/
struct HuJudgement: HuJudgementResult {
	// 输入参数，以及可以立即确定的中间状态
	size_t const num_huns;
	TileVec const hands;
	tile_t const last_tile;
	FANXING const conf;
	tile_mask_t const pg_mask;
	pg_tile_count_by_color_t const sum_pg_tile_count_by_color;
	TileZoneDivider const stats;
	tile_mask_t const all_mask;
	// 中间状态
	HuBranch selected_branch;
	// 中间状态2 -- 洪洞王牌使用
	HuBranch selected_branch_hongtong;

	// 执行函数
	HuJudgement(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf, tile_mask_t pg_mask
		, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color);
	FANXING later_stage(PlayerState &ps, FANXING fans);
};


