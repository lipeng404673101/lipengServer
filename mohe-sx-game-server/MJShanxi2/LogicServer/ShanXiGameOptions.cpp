

#include "ClientConstants.h"





// 定义各个玩法允许的小选项

// 晋中小选项需求：暗杠可见、七小对、风一色、清一色、凑一色、过胡只可自摸、荒庄不荒杠、听牌可杠、只有胡牌玩家杠算分
static int const ShanXiGameOptions_JinZhong[] = {
	PT_AnGangKeJian, 
	PT_GuoHuZhiKeZiMo, PT_TingPaiKeGang, PT_HuangZhuangBuHuangGang, PT_ZhiYouHuPaiWanJiaGangSuanFen, 
	PT_QiXiaoDui, PT_FengYiSe, PT_QingYiSe, PT_CouYiSe, 
};

// 平鲁：暗杠可见、未上听点炮包赔、报听胡牌、一炮多响、七小对、一五九(默认)、东领头(默认)、清一色(默认)、一条龙(默认)
static int const ShanXiGameOptions_PingLu[] = {
	PT_AnGangKeJian,
	PT_WeiShangTingDianPaoBaoPei, PT_BaoTing, PT_YiPaoDuoXiang, 
	PT_QiXiaoDui, PT_YiWuJiu, PT_DongLingTou, PT_QingYiSe, PT_YiTiaoLong, 
};

// 硬三嘴：暗杠可见、一炮多响、庄加1分(默认)、七小对(默认)、清一色(默认)、字一色(默认)、青龙(默认)、清七小对(默认)、豪华七小对(默认)、一条龙(默认)
static int const ShanXiGameOptions_YingSanZui[] = {
	PT_AnGangKeJian,
	PT_YiPaoDuoXiang, PT_ZhuangJiaYiFen,
	PT_QiXiaoDui, PT_QingYiSe, PT_ZiYiSe, PT_QingLong, PT_QingQiXiaoDui, PT_HaoHuaQiXiaoDui, PT_YiTiaoLong,
};

// 洪洞王牌：暗杠可见、杠不随胡、缺两门、字牌(默认)、三元自摸(默认)、吊王(默认)、清豪华七小对(默认)
static int const ShanXiGameOptions_HongTongWangPai[] = {
	PT_AnGangKeJian, PT_CouYiSe,
	PT_GangBuSuiHu, PT_QueLiangMen, PT_ZiPai, PT_SanYuanZiMo, PT_DiaoWang, 
	PT_QingHaoHuaQiXiaoDui, PT_HongTongBuKePengPai,PT_HuangZhuangBuHuangGang
};

// 临汾一门牌：暗杠可见、数页、庄加1分、一张赢(默认)、清一色(默认)、字一色(默认)、三元(默认)、字一色七大对(默认)、双豪华七小对(默认)、三风(默认)
static int const ShanXiGameOptions_LinFenYiMenPai[] = {
	PT_AnGangKeJian,
	PT_ShuYe, PT_ZhuangJiaYiFen, PT_YiZhangYing, PT_QingYiSe, PT_ZiYiSe, PT_SanYuan, 
	PT_ZiYiSeQiDaDui, PT_ShuangHaoHuaQiXiaoDui, PT_SanFeng,  MH_PT_HuangZhuangLunZhuang
};





bool shanxi_game_option_validate_by_game_type(int v, int game_type) {
	switch (game_type) {
#define CASE(NAME) \
		case NAME: {\
			int const *first = ShanXiGameOptions_##NAME;\
			int const *last = first + sizeof(ShanXiGameOptions_##NAME)/sizeof(ShanXiGameOptions_##NAME)[0];\
			return std::find(first, last, v) != last; /* found */\
		}
		// 启用这些玩法的小选项白名单校验
		CASE(JinZhong)
#undef CASE
	default: return true; // 没有做这套校验机制的玩法，就让它过
	}
}




