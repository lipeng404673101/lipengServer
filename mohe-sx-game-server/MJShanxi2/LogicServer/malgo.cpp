

#include "malgo.h"



std::string malgo_format_fans(FANXING fans) {
	std::ostringstream s;
	s << "(FANXING)(0";
#define FANXING_DEF(name) if (fans & name) s << " | " #name; fans &= ~name
#define FANXING_DEF_BY(name, value)


	
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
FANXING_DEF(FAN_DuanYao); // 断幺
FANXING_DEF(FAN_KanHu); // 坎胡
FANXING_DEF(FAN_BianZhang); // 边张
FANXING_DEF(FAN_KanZhang); // 坎张
FANXING_DEF(FAN_DiaoZhang); // 吊张，都改为叫单吊

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
FANXING_DEF(FAN_HongTong_LouShangLou); // 楼上楼 - 手中一副中发白，自摸另一副中的一张
FANXING_DEF(FAN_HongTong_YingKou);     // 硬扣  -  自摸一副中发白中的1张
FANXING_DEF(FAN_HongTong_DiaoWang);



#undef FANXING_DEF
#undef FANXING_DEF_BY
	if (fans) s << std::hex << fans;
	s << ")";
	return s.str();
}
