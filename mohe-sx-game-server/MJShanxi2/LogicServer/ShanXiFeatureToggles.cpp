
#include "malgo.h"

#include "ClientConstants.h"

#include "ShanXiFeatureToggles.h"
#define SUIJUN_LOG LLOG_ERROR
#include "..\mhmessage\mhmsghead.h"


void ShanXiFeatureToggles::setup_feature_toggles(int game_type) {
	// m_registered_game_type 指作为 GameHandler 注册的玩法编号，并且是客户端创建房间传入的玩法编号
	m_registered_game_type = game_type;
	// FEAT_GAME_TYPE 则是 ShanXiGameCore 内部使用的玩法编号，这个编号集合是注册编号的子集

	// 二人抠点在外面是绑定二人和抠点这两个信息的意思
	switch (game_type) {
	
	case ErRenKouDian:
	case SanRenKouDian:game_type = KouDian; break;
	case ErRenTuiDaoHu:
	case SanRenTuiDaoHu:
		game_type = TuiDaoHu;
		break;
	case ErRenJinZhong:
	case SanRenJinZhong:
		game_type = JinZhong;
		break;	
	case ErRenYingSanZui:
	case SanRenYingSanZui:
		game_type = YingSanZui;
		break;
	case ErRenHongTongWangPai:
	case SanRenHongTongWangPai:
		game_type = HongTongWangPai;
		break;
	case ErRenLinFenYiMenPai:
	case SanRenLinFenYiMenPai:
		game_type = LinFenYiMenPai;
		break;
	default:break;
	}

	FEAT_GAME_TYPE = game_type;

	switch (FEAT_GAME_TYPE) {

	case TuiDaoHu:
		break;

	case KouDian:
		break;

	case LiSi:
		break;

	case JinZhong:
		break;

	case TieJin:
		break;

	case GuaiSanJiao:
		break;

	}

	switch (FEAT_GAME_TYPE) {

	case TuiDaoHu:
		FEAT_DaiFeng = 0; // 推到胡默认不带风，需要玩家用小选项启用
		FEAT_DianGangBaoGang = 1; // 点杠包杠（补杠、暗杠不是点杠），立四不开启这个选项
		FEAT_TingPaiBuBaoDianGang = 1; // 听牌不包点杠---------和老版本麻将改成一致
		FEAT_YiPaoDuoXiang = 1; // 一炮多响
		FEAT_QiangGangHu = 1; // 抢杠胡
		SUIJUN_LOG("*******构造推倒胡玩法FeatureToggles game_type = %d", game_type);
		break;

	case KouDian:
		FEAT_BaoTing = 1; // 强制需要报听
		FEAT_DianGangBaoGang = 1; // 点杠包杠（补杠、暗杠不是点杠），立四不开启这个选项
		FEAT_TingPaiBuBaoDianGang = 1; // 听牌不包点杠
		FEAT_DealerRiskScore = 10; // 庄风险分
		FEAT_DealerRiskScoreDisabled = 1; // 抠点是通过小选项来启用庄算分的
		//FEAT_YiPaoDuoXiang = 1; // 一炮多响
		FEAT_QiangGangHu = 1; // 抢杠胡
		//FEAT_WeiShangTingGangBuSuanFen = 1; // 未上听杠不算分
		MHLOG("*******构造扣点玩法FeatureToggles game_type = %d", game_type);
		break;

	case LiSi:
		FEAT_BaoTing = 1; // 强制需要报听
		FEAT_SUO_COUNT = 4; // 立四要锁定4张牌
		FEAT_ZhiYouHuPaiWanJiaGangSuanFen = 1; // 只有胡家才能获得杠分，立四启用这个选项了
		FEAT_FangPaoExtraLoss = 1; // 放炮者多扣一分
		FEAT_DealerRiskScore = 1; // 庄风险分
		FEAT_TingPaiHouBuNengGuoHu = 1; // 听牌后不能过胡
		break;

	case JinZhong:
		//  A 特殊番型默认支持， 不显示
		//	B.荒庄荒杠 和 杠随胡家不显示，默认支持
		//	C.听牌可杠不显示， 默认支持
		//	D.暗杠可见 显示，默认选中支持
		//	E.过胡只可自摸 显示，默认不选中
		// 2018.1.13 听牌后不可杠牌
		// 2018.1.13 上听玩家点炮不额外扣1分
		// 2018.1.13 未上听玩家点炮包赔杠分
		// 2018.1.13 庄+1分，自摸翻倍
		// 2018.1.13 碰碰胡，胡牌手牌必须大于等于5张，1张只能算单掉
		// 2018.1.15 去掉豪华七对，改为7小对
		FEAT_BaoTing = 1; // 强制需要报听
		FEAT_TingPaiKeGang = 0; // 听后不能杠牌
		FEAT_ZhiYouHuPaiWanJiaGangSuanFen = 1; // 杠随胡家，默认选中
		FEAT_OnlyQueMenCanBaoTing = 1;
		FEAT_FangPaoExtraLoss = 1; // 听前炮出1分
		FEAT_FangPaoExtraLoss_TingHou = 0; // 听后点炮不出分
		FEAT_DealerRiskScore = 1; // 庄风险分
		FEAT_DealerRiskScoreDisabled = 0; // 开启庄风险分
		FEAT_DisableZiMoDoubleScore = 0;  // 庄算分翻倍
		FEAT_WeiShangTingDianPaoBaoGang = 1; // 未上听点炮包杠
		FEAT_QiangGangHu = 1;
		break;

	case TieJin:
		// 荒庄荒杠这个默认的，点杠包杠需开启，补杠不包杠是默认的
		// 未上听点炮包赔，上听点炮、自摸三家扣分，这是默认的
		// 荒庄不算金分，不荒庄，报听的都算金分
		// 1金3、2金9、3个金27分，三金算分封顶，但是仍然可以上金。
		// 听牌后遇到胡，玩家必须胡。需客户端不给“过”按钮
		// 别人出的金牌不能碰和杠
		// 自己手里的金牌也不能暗杠
		FEAT_BaoTing = 1; // 强制需要报听
		FEAT_TingPaiKeGang = 0; // 贴金：听牌后不能杠牌
		FEAT_DianGangBaoGang = 1; // 点杠包杠（补杠、暗杠不是点杠），立四不开启这个选项
		FEAT_HunPaiBuKeGang = 1; // 金牌不可杠
		FEAT_TingPaiHouBuNengGuoHu = 1; // 听牌后不能过胡
		break;

	case GuaiSanJiao:
		FEAT_DaiFeng = 0; // 拐三角默认不带风，需要玩家用小选项启用
		// FEAT_DianGangBaoGang = 1; // 点杠包杠（补杠、暗杠不是点杠），立四不开启这个选项
		FEAT_DianPaoDanRenKouFen = 1; // 点炮单人扣分
		FEAT_DianGangDanRenKouFen = 1; // 点杠单人扣分
		FEAT_QiangGangHu = 1;
		break;

	case PingLu:
		FEAT_DianGangBaoGang = 1; // 点杠包杠（补杠、暗杠不是点杠），立四不开启这个选项
		FEAT_GuoHuChuPaiQianKeShouPao = 1; // 过胡出牌前可收炮
		break;

		//// 临汾一门牌  by wyz
	case LinFenYiMenPai:
		MHLOG("*******构造临汾一门牌FeatureToggles game_type = %d", game_type);
	case YingSanZui:
		FEAT_BaoTing = 1; // 强制需要报听
		FEAT_GuoHuChuPaiQianKeShouPao = 1; // 过胡出牌前可收炮
		FEAT_ZhiKeZiMoHuPai = 1; // 只可自摸胡牌，不能收炮胡牌
		FEAT_DisableZiMoDoubleScore = 1; // 庄算分时禁止自摸双倍
		FEAT_ZhiYouHuPaiWanJiaGangSuanFen = 1; // 只有胡家才能获得杠分
		FEAT_DealerRiskScore = 1; // 庄风险分
		FEAT_DealerRiskScoreDisabled = 0; // 硬三嘴是默认庄一分，可以通过小选项选择2分		
		MHLOG("*******构造硬三嘴FeatureToggles game_type = %d", game_type);
		break;

	case BianKaDiao:
		FEAT_DaiFeng = 0; // 推到胡默认不带风，需要玩家用小选项启用
		FEAT_DianGangBaoGang = 1; // 点杠包杠（补杠、暗杠不是点杠），立四不开启这个选项
		break;
	//洪洞王牌：
	case HongTongWangPai:	  // Suijun洪洞王牌玩法
		FEAT_BaoTing = 1; // 强制需要报听
		FEAT_ZhiKeZiMoHuPai = 1; // 只可自摸胡牌，不能收炮胡牌
		FEAT_TingPaiKeGang = 1;  // 听牌可杠
		FEAT_OnlyQueMenCanBaoTing = 1; // 缺门报听
		FEAT_DealerRiskScore = 1; // 庄风险分
		FEAT_DealerRiskScoreDisabled = 0; // 默认开启
		FEAT_DisableZiMoDoubleScore = 1; // 庄算分时禁止自摸双倍	
		MHLOG("*******构造洪洞王牌FeatureToggles game_type = %d", game_type);
		break;

	//忻州扣点：强制报听、可抢杠胡、听牌可杠（但不能改变听口）（**Ren 2017-11-28）
	case XinZhouKouDian:
		FEAT_DaiFeng = 1;        // 必须带风
		FEAT_BaoTing = 1;        // 强制需要报听
		FEAT_QiangGangHu = 1;    // 抢杠胡
		FEAT_TingPaiKeGang = 1;  // 听牌可杠
		FEAT_TingPaiBuBaoDianGang = 1; //听牌不包点杠（暗杠、补杠不算点杠）
		m_remain_card_count = 12;// 座子上剩余12张牌算荒庄
		MHLOG("*******构造忻州扣点FeatureToggles game_type = %d", game_type);
		break;

	//撵中子：
	case NianZhongZi:
		FEAT_DaiFeng = 1;        // 必须带风
		FEAT_BaoTing = 1;        // 强制需要报听
		FEAT_QiangGangHu = 1;    // 抢杠胡
		FEAT_TingPaiKeGang = 1;  // 听牌可杠
		break;
	case LingChuanMaJiang:
		FEAT_BaoTing = 0; //强制报听
		FEAT_DaiFeng = 1; //带风
		FEAT_QiangGangHu = 1; //抢杠胡
		FEAT_FangPaoExtraLoss = FEAT_FangPaoExtraLoss_TingHou = 2; //点炮多赔2分
		FEAT_TingPaiKeGang = 1;  // 听牌可杠
		FEAT_YiPaoDuoXiang = 0;  // 不带一炮多响
		m_remain_card_count = 12;// 座子上剩余12张牌算荒庄
		MHLOG("*******构造陵川FeatureToggles game_type = %d", game_type);
		break;
	case JinChengMaJiang:
		FEAT_BaoTing = 1; //强制报听
		FEAT_DaiFeng = 0; //带风
		FEAT_QiangGangHu = 1; //抢杠胡
		FEAT_FangPaoExtraLoss = FEAT_FangPaoExtraLoss_TingHou = 2; //点炮多赔2分
		FEAT_TingPaiKeGang = 1;  // 听牌可杠
		FEAT_YiPaoDuoXiang = 0;  // 不带一炮多响
		FEAT_WeiShangTingDianPaoBaoGang = 1; //未上听包杠
		MHLOG("*******构造晋城*晋阳FeatureToggles game_type = %d", game_type);
		break;
	}
}

static FANXING calc_allowed_fans_by_game_type(int FEAT_GAME_TYPE) {
	// 番型配置由玩法固定确定的部分
	switch (FEAT_GAME_TYPE) {
	case TuiDaoHu: return FAN_BASIC_ALLOWED | FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe;
	// 抠点的小选项“七小对”指的是：能胡七小对，并且七小对加番，并且豪华七小对分数四倍
	case KouDian: return FAN_BASIC_ALLOWED | FAN_MUST_KouDian | FAN_ShiSanYao | FAN_YiTiaoLong | FAN_QingYiSe
		//| FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui
		;
	case LiSi: return FAN_BASIC_ALLOWED | FAN_QueMen | FAN_BianKanDiao | FAN_YiTiaoLong | FAN_QingYiSe;
	case JinZhong: return FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_PengPengHu | FAN_BianKanDiao | FAN_FengYiSe | FAN_CouYiSe | FAN_QingYiSe | FAN_QiXiaoDui| FAN_HaoHuaQiXiaoDui
			| FAN_MenQing | FAN_DuanYao
			| FAN_MUST_QueMen // 晋中：必须缺门才能胡, MH修改加上 FAN_FengYiSe |FAN_CouYiSe | FAN_QingHaoHuaQiXiaoDui
			;
	case TieJin: return FAN_BASIC_ALLOWED 
		//| FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe
		;
	case GuaiSanJiao: return FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_QingYiSe | FAN_KanHu | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui |FAN_LianZhuang;
		// return FAN_BASIC_ALLOWED | FAN_MUST_YiMenGouBaZhang | FAN_YiTiaoLong | FAN_QingYiSe | FAN_KanHu 原来逻辑
			//| FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui
			 // 拐三角去掉了一门八张， 默认加入七小对，豪华七小对
	case PingLu: return FAN_BASIC_ALLOWED | FAN_MUST_QueMen | FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe;
		//// 临汾一门牌   by wyz
	case LinFenYiMenPai:

	case YingSanZui: return FAN_BASIC_ALLOWED
		| FAN_QueYiMen | FAN_QueLiangMen | FAN_GuJiang | FAN_YiZhangYing | FAN_DiaoZhang | FAN_DanDiao
		| FAN_JUDGE_SanFeng | FAN_JUDGE_SanYuan
		| FAN_MenQing
		| FAN_QingYiSe | FAN_ZiYiSe
		| FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_QingQiDui | FAN_QingHaoHuaQiXiaoDui
		| FAN_YiTiaoLong | FAN_QingLong
		;
	// SUIJUN添加洪洞王牌番型
	case HongTongWangPai: return FAN_BASIC_ALLOWED | FAN_QiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe | FAN_CouYiSe | FAN_ZiYiSe | FAN_MUST_QueMen
		 | FAN_JUDGE_SanFeng | FAN_JUDGE_SanYuan | FAN_HongTong_LouShangLou | FAN_HongTong_YingKou | FAN_HongTong_DiaoWang;
	//边卡吊：
	case BianKaDiao: return FAN_BASIC_ALLOWED | FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe;
		
	//忻州扣点:扣点胡牌点数、一条龙、清一色、字一色、七小对、青龙、清七对、字七对  (**Ren 2017-11-22)
	case XinZhouKouDian:
		return FAN_BASIC_ALLOWED
			| FAN_MUST_KouDian
			| FAN_YiTiaoLong
			| FAN_HaoHuaQiXiaoDui
			| FAN_QiXiaoDui
			| FAN_QingYiSe
			| FAN_ZiYiSe
			| FAN_QingLong
			| FAN_QingQiDui
			| FAN_QingHaoHuaQiXiaoDui;

	//撵中子：门清、缺门、断幺、坎张、边张、单吊、混一色(凑一色)、字一色、清一色、青龙、十三幺 （**Ren 2017-11-28）
	case NianZhongZi:
		return FAN_BASIC_ALLOWED
			| FAN_MenQing
			| FAN_QueMen
			| FAN_DuanYao
			| FAN_KanZhang
			| FAN_BianZhang
			| FAN_DiaoZhang
			| FAN_CouYiSe
			| FAN_ZiYiSe
			| FAN_QingYiSe
			| FAN_YiTiaoLong
			| FAN_QingLong
			| FAN_ShiSanYao;
	case LingChuanMaJiang:
		return FAN_BASIC_ALLOWED | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_ShiSanYao | FAN_LingChuan_Must_KaoBaZhang;
		break;
	case JinChengMaJiang:
		return FAN_BASIC_ALLOWED | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_ShiSanYao;
		break;
	default: return FAN_BASIC_ALLOWED;
	}
}
static FANXING revise_allowed_fans_by_game_options(FANXING fans, GameOptionSet const &m_playtype) {
	// 番型配置由小选项确定的部分
	// 小选项要由小选项白名单控制客户端输入，所以这里可以假定传过来的小选项都是合法的

	// 抠点的捉耗子玩法没有七小对和十三幺，由小选项添加七小对和十三幺
	if (m_playtype.b_HaoZi || m_playtype[PT_ShuangHaoZi]) fans = FAN_MUST_KouDian | FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_QingYiSe;

    // 新版山西麻将中扣点带耗子玩法允许七小对，十三幺不能胡
	if (m_playtype.b_HaoZi || m_playtype[PT_ShuangHaoZi] || m_playtype[MH_PT_FengHaoZi])
	{
		fans = FAN_MUST_KouDian | FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_QingYiSe | FAN_QiXiaoDui;
		fans = FAN_MUST_KouDian | FAN_BASIC_ALLOWED | FAN_QiXiaoDui;   // 带耗子的玩法去掉一条龙和清一色的番
	}

	// 这些都是暴力叠加了
	if (m_playtype[PT_QiXiaoDui]) fans |= FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui;
	if (m_playtype[PT_FengYiSe]) fans |= FAN_FengYiSe;
	if (m_playtype[PT_QingYiSe]) fans |= FAN_QingYiSe;
	if (m_playtype[PT_CouYiSe]) fans |= FAN_CouYiSe;
	if (m_playtype[PT_ShiSanYao]) fans |= FAN_ShiSanYao;
	if (m_playtype[PT_YiTiaoLong]) fans |= FAN_YiTiaoLong;
	if (m_playtype[PT_QueYiMen]) fans |= FAN_MUST_QueMen;  //忻州扣点：缺一门选项（**Ren 2017-11-28）

	
	if (m_playtype[PT_QueLiangMen])
	{
		fans |= FAN_HongTong_MustQueLiangMen; // 缺2門必然湊一色，就不加這個番了
		//fans &= ~FAN_CouYiSe;               // 玩家要求加上这个番，传回客户端的时候过滤掉
	}

	// 这些都得排在暴力叠加的后面，有选项互斥的效果
	// 推倒胡的小胡玩法只有平胡
	if (m_playtype.XiaoHu) fans = FAN_BASIC_ALLOWED;
	// 推倒胡的边卡吊玩法
	if (m_playtype[PT_BianKanDiao]) {
		fans |= FAN_MUST_BianKanDiao | FAN_BianKanDiao;
		fans &= ~(FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_ShiSanYao);
	}

	// 拐三角添加带风小选项，加上十三幺
	if (m_playtype[PT_DaiFeng])
	{
		fans |= FAN_ShiSanYao;
	}
	if (m_playtype[MH_PT_YingBaZhang])
	{
		fans |= FAN_MUST_YiMenGouBaZhang;
	}
	// 返回修订后的值
	return fans;
}
static FANXING exclude_allowed_fans_by_game_type(FANXING fans, int FEAT_GAME_TYPE) {
	// 有些玩法的七小对选项不包括豪华七小队的
	switch (FEAT_GAME_TYPE) {
	case TuiDaoHu: return fans;
	//case TieJin: return fans & ~(FAN_HaoHuaQiXiaoDui);
	default: return fans;
	}
}
void ShanXiFeatureToggles::init_allowed_fans() {
	// 番型配置由玩法固定确定的部分
	m_allowed_fans = calc_allowed_fans_by_game_type(FEAT_GAME_TYPE);
	LLOG_DEBUG("****1****GAME_TYPE = %d 能胡的番型 %s", FEAT_GAME_TYPE, malgo_format_fans(m_allowed_fans).c_str());
	// 番型配置由小选项确定的部分
	m_allowed_fans = revise_allowed_fans_by_game_options(m_allowed_fans, m_playtype);
	// 有些玩法的七小对选项不包括豪华七小队的
	m_allowed_fans = exclude_allowed_fans_by_game_type(m_allowed_fans, FEAT_GAME_TYPE);
	LLOG_DEBUG("****2****GAME_TYPE = %d 能胡的番型 %s", FEAT_GAME_TYPE, malgo_format_fans(m_allowed_fans).c_str());
}


void ShanXiFeatureToggles::vectorize_fans(FANXING fans, std::vector<Lint> &vec) {
	// 豪华七小对不和七小对同时显示
	if (fans & FAN_HaoHuaQiXiaoDui) fans &= ~FAN_QiXiaoDui;

	// 有些玩法强制不显示平胡
	switch (FEAT_GAME_TYPE) {
	//// 临汾一门牌   by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		fans &= ~FAN_PingHu;
		break;
	case HongTongWangPai:
		fans &= ~FAN_PingHu;
		fans &= ~FAN_ZiMo;
		if (m_playtype[PT_QueLiangMen])
		{
			fans &= ~FAN_CouYiSe;
		}
		break;
	//忻州扣点：豪华七小对显示七小对，清豪华七小对显示清七对（**Ren 2017-12-22）
	case XinZhouKouDian:
		if (fans & FAN_HaoHuaQiXiaoDui)
		{
			fans = fans & ~FAN_HaoHuaQiXiaoDui | FAN_QiXiaoDui;
		}
		if (fans & FAN_QingHaoHuaQiXiaoDui)
		{
			fans = fans & ~FAN_QingHaoHuaQiXiaoDui | FAN_QingQiDui;
		}
	//撵中子：不显示平胡（**Ren 2017-11-29）
	case NianZhongZi:
		fans &= ~FAN_PingHu;
	}

#define A(fan, hu) if (fans & (fan)) vec.push_back((hu))
	// 主体
	A(FAN_PingHu, HU_PingHu);
	A(FAN_QiXiaoDui, HU_Qixiaodui);
	A(FAN_HaoHuaQiXiaoDui, HU_DaQixiaodui);
	A(FAN_ShiSanYao, HU_ShiSanYao);
	// 番型组合
	A(FAN_QingLong, HU_QingLong);
	A(FAN_QingQiDui, HU_QingQiDui);
	A(FAN_QingHaoHuaQiXiaoDui, HU_QingHaoHuaQiXiaoDui);
	// 通用叠加
	A(FAN_QingYiSe, HU_QingYiSe);
	A(FAN_CouYiSe, HU_CouYiSe);
	A(FAN_FengYiSe, HU_FengYiSe);
	A(FAN_ZiYiSe, HU_ZiYiSe);
	A(FAN_KanHu, HU_KanHu);
	A(FAN_MenQing, HU_MenQing);
	A(FAN_DuanYao, HU_DuanYao);
	A(FAN_QueMen, HU_QUEMEN);
	A(FAN_QueYiMen, HU_QueYiMen);
	A(FAN_QueLiangMen, HU_QueLiangMen);
	A(FAN_GuJiang, HU_GuJiang);
	A(FAN_YiZhangYing, HU_YiZhangYing);
	// 平胡叠加
	A(FAN_BianZhang, HU_BIANZHANG);
	A(FAN_KanZhang, HU_KANZHANG);
	A(FAN_DiaoZhang, HU_DIAOZHANG);
	A(FAN_DanDiao, HU_DanDiao);
	A(FAN_PengPengHu, HU_PengPengHu);
	A(FAN_YiTiaoLong, HU_YiTiaoLong);
	// 硬三嘴的三风、三元
	A(FAN_SanFeng_X1, HU_SanFeng_X1);
	A(FAN_SanFeng_X2, HU_SanFeng_X2);
	A(FAN_SanFeng_X3, HU_SanFeng_X3);
	A(FAN_SanFeng_X4, HU_SanFeng_X4);
	A(FAN_SanYuan_X1, HU_SanYuan_X1);
	A(FAN_SanYuan_X2, HU_SanYuan_X2);
	A(FAN_SanYuan_X3, HU_SanYuan_X3);
	A(FAN_SanYuan_X4, HU_SanYuan_X4);
	A(FAN_HongTong_LouShangLou, HU_HongTong_LouShangLou);
	A(FAN_HongTong_YingKou, HU_HongTong_YingKou);
	A(FAN_HongTong_DiaoWang, HU_HongTong_DiaoWang);
	A(FAN_LianZhuang, HU_LianZhuang);
#undef A


}


tile_mask_t ShanXiFeatureToggles::calc_hu_candidate_set(tile_mask_t mask) const {
	// 比如4个3万周围没牌，那就得一个刻子，再左或右一个顺子
	// 所以就把每张牌以及周围的4张牌加进来
	tile_mask_t dst = mask;
	dst |= (mask & TILE_MASK_SHUPAI39) >> 2; // 左2
	dst |= (mask & TILE_MASK_SHUPAI29) >> 1; // 左1
	dst |= (mask & TILE_MASK_SHUPAI18) << 1; // 右1
	dst |= (mask & TILE_MASK_SHUPAI17) << 2; // 右2
	if (m_allowed_fans & FAN_ShiSanYao) dst |= TILE_MASK_SHISANYAO;
	if (m_allowed_fans & FAN_JUDGE_SanFeng) {
		if (mask & TILE_MASK_Feng) dst |= TILE_MASK_Feng;
	}
	if (m_allowed_fans & FAN_JUDGE_SanYuan) {
		if (mask & TILE_MASK_Jian) dst |= TILE_MASK_Jian;
	}
	return dst;
}

FANXING ShanXiFeatureToggles::call_judge(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf
	, tile_mask_t pg_mask, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color) const {

	//MHLOG("*****************************计算番型************************");
	// 这里要处理这样一个问题：一般的玩法肯定七小对分高，但有的玩法七小对分高还是平胡分高不算出来确定不了
	FANXING fans = FAN_NONE;
	switch (FEAT_GAME_TYPE) {
	//// 临汾一门牌   by wyz
	case LinFenYiMenPai:
	case YingSanZui: {
		HuJudgement judgement1(ps, num_huns, hands, last_tile, conf & ~FAN_JUDGE_DaHu, pg_mask, sum_pg_tile_count_by_color);
		HuJudgement judgement2(ps, num_huns, hands, last_tile, conf & ~FAN_JUDGE_XiaoHu, pg_mask, sum_pg_tile_count_by_color);
		FANXING diaozhang1 = judgement1.fans & FAN_DiaoZhang;
		FANXING diaozhang2 = judgement2.fans & FAN_DiaoZhang;
		int score1 = calc_common_loss(judgement1.fans, last_tile);
		int score2 = calc_common_loss(judgement2.fans, last_tile);
		if (score1 > score2) fans = judgement1.fans;
		else fans = judgement2.fans;
		break;
	}
	default: {
		HuJudgement judgement(ps, num_huns, hands, last_tile, conf, pg_mask, sum_pg_tile_count_by_color);
		fans = judgement.fans;
	}
	}
	//MHLOG("***********fans = judgement.fans =%s", malgo_format_fans(fans).c_str());
	FANXING diaozhang3 = fans & FAN_DiaoZhang;
	// 洪洞王牌楼外楼的听口，增加楼外楼的番型 -- 利用三嘴的三元番型转为楼上楼番型
	if (HongTongWangPai == FEAT_GAME_TYPE)
	{	
			if ((fans &(FAN_SanYuan_X2|FAN_SanYuan_X3|FAN_SanYuan_X4))			 
				&& (TILE_MASK_Jian & TILE_TO_MASK(last_tile)))
			{
				fans |= FAN_HongTong_LouShangLou;
			}

			if ((fans &(FAN_SanYuan_X1))
				&& (TILE_MASK_Jian & TILE_TO_MASK(last_tile)))
			{
				fans |= FAN_HongTong_YingKou;
			}	

			if (fans & FAN_QiXiaoDui)   
			{
				//for(int i = 0; i < ps.ha )
			}
		 
	}

	fans = reduce_fans(fans, conf);
	FANXING diaozhang4 = fans & FAN_DiaoZhang;	
	//MHLOG("***********FanValue=%lld", fans);
	//MHLOG("***********FanOuput=%s", malgo_format_fans(fans).c_str());
	return fans;
}

// 减掉扣分叠加的番型
FANXING ShanXiFeatureToggles::reduce_fans(FANXING fans, FANXING const conf) const {
	// 这个函数解决这样一个问题：大部分玩法的大胡都会消掉门清、单吊这些小分的，但有些玩法如硬三嘴就比较特殊
	switch (FEAT_GAME_TYPE) {
	//// 临汾一门牌  by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		// 硬三嘴：只有七小队时把门清砍掉，其它如清一色、一条龙都不砍的
		if (fans & FAN_JUDGE_QiDui) fans &= ~FAN_MenQing;
		// 硬三嘴：其它的如单吊、孤将什么的，要数嘴，都不砍的
		break;
	case HongTongWangPai: // 王牌减少叠番 
		//if (fans & FAN_QingYiSe)  fans &= ~(FAN_YiTiaoLong | FAN_QiXiaoDui | FAN_CouYiSe);
		//if (fans & FAN_ZiYiSe) fans &= ~(FAN_JUDGE_SanFeng | FAN_JUDGE_SanYuan | FAN_QiXiaoDui | FAN_HongTong_LouShangLou);
		if (fans & FAN_HongTong_LouShangLou) fans &= ~(FAN_QiXiaoDui | FAN_HongTong_DiaoWang);
		if (fans & FAN_HongTong_YingKou) fans &= ~(FAN_HongTong_DiaoWang | FAN_QiXiaoDui);
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_SMALL_EFFECTS;
		break;
	case GuaiSanJiao:
		if (fans & FAN_KanHu) fans &= ~FAN_PingHu; // 拐三角坎胡不计平胡分
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_SMALL_EFFECTS;
		break;
	//撵中子：门清、边卡吊、缺门、一条龙等是需要叠加的(**Ren 2017-11-29)
	case NianZhongZi:
		if (fans & FAN_HUCHI_QueMen) fans &= ~FAN_QueMen;
		if (fans & FAN_ShiSanYao) fans &= ~FAN_SMALL_EFFECTS;
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_PingHu;
		break;
	//晋中：门清、边卡吊、缺门、一条龙等是需要叠加的(**Ren 2017-11-29)
	case JinZhong:		
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_PingHu;
		if (fans & FAN_PengPengHu) fans |= FAN_PingHu;
		if (fans & FAN_HaoHuaQiXiaoDui)
		{
			fans &= (~FAN_HaoHuaQiXiaoDui); //晋中没有豪华7对
			fans |= FAN_QiXiaoDui;
		}
		if (fans & FAN_QiXiaoDui)
		{
			fans &= (~FAN_MenQing);   // 
			fans &= (~FAN_DiaoZhang); // 七小对去掉吊张和门清
		}
		break;
	default:
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_SMALL_EFFECTS;
		break;
	}
	// 最后要与允许的番型取交集
	return fans & conf;
}

FANXING ShanXiFeatureToggles::reduce_fans_in_TingKou_stage(FANXING fans, FANXING const conf) const {
	switch (FEAT_GAME_TYPE) {
	//// 临汾一门牌  by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		// 硬三嘴：单吊和之前玩法的吊张不同，单吊要求只听一张牌，单吊的时候就数一张赢的嘴了
		if ((fans & FAN_DiaoZhang) && (fans & FAN_YiZhangYing)) {
			fans |= FAN_DanDiao;
			fans &= ~FAN_YiZhangYing;
		}
		fans &= ~FAN_DiaoZhang;
		// 硬三嘴：三元自摸不需要单独计算一张赢的分数
		if (!(fans & FAN_SanYuanZaiShou) && (fans & FAN_JUDGE_SanYuan)) {
			fans &= ~FAN_YiZhangYing;
		}
		break;
	default:
		break;
	}
	// 最后要与允许的番型取交集
	return fans & conf;
}

int ShanXiFeatureToggles::calc_zui_score(FANXING fans) const {
	int score = 0;
	if (fans & FAN_QueYiMen) score += 1;
	if ((fans & FAN_QueLiangMen) || (fans & FAN_ZiYiSe)||(fans & FAN_FengYiSe)) score += 3;
	//不再用了，用特殊函数处理这种情况了
	//if ( fans & FAN_ZiYiSe) score += 3;               // by wyz 解决全字牌问题，按缺两门算
	//if (fans & FAN_FengYiSe) score += 3;             // by wyz 解决全风牌问题，按缺两门算
	if (fans & FAN_GuJiang) score += 1;
	if (fans & FAN_YiZhangYing) score += 1;
	if (fans & FAN_DanDiao) score += 1;
	if (fans & FAN_SanFeng_X1) score += 1;
	if (fans & FAN_SanFeng_X2) score += 3;
	if (fans & FAN_SanFeng_X3) score += 10;
	if (fans & FAN_SanFeng_X4) score += 10;
	if (fans & FAN_SanYuan_X1) {
		if (fans & FAN_SanYuanZaiShou) score += 1;
		else score += 2;
	}
	if (fans & FAN_SanYuan_X2) {
		if (fans & FAN_SanYuanZaiShou) score += 5;
		else score += 7;
	}
	if (fans & FAN_SanYuan_X3) score += 50;
	if (fans & FAN_SanYuan_X4) score += 50;
	return score;
}

//// 硬三嘴 在胡的番型上为七小对，特殊的计算嘴数的函数  by wyz
int ShanXiFeatureToggles::calc_zui_score_spec_qixiaodui(TileVec const &hands, tile_mask_t pg_mask) const {
	struct TileZoneDivider stats(hands.begin(), hands.size());
	tile_mask_t const  all_mask(stats.mask | pg_mask);

	int zuiNum = 1;   // 这个1嘴时一张赢的，七小对可能时一张赢
					  //缺门
					  //int n = calc_quemen_count(all_mask);
	int sum = 0;
	if (0 == (all_mask & TILE_MASK_WAN)) ++sum;
	if (0 == (all_mask & TILE_MASK_TONG)) ++sum;
	if (0 == (all_mask & TILE_MASK_TIAO)) ++sum;
	switch (sum)
	{
	case 1:
		zuiNum++;
		break;
	case 2:
	case 3:
		zuiNum += 3;
		break;
	}
	MHLOG("Logwyz ... sum=[%d] zuiNum=[%d]", sum, zuiNum);
	//三风
	std::vector<int> temp;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_ZI; tc = (TILE_COLOR)(tc + 1))
		temp.push_back(stats.count_by_color_by_number[TILE_COLOR_ZI - TILE_COLOR_MIN][tc - TILE_COLOR_MIN]);
	std::sort(temp.begin(), temp.begin() + (TILE_COLOR_ZI - TILE_COLOR_MIN));
	switch (temp[1])
	{
	case 1:
		zuiNum++;
		break;
	case 2:
		zuiNum += 3;
		break;
	case 3:
		zuiNum += 10;
		break;
	}
	MHLOG("Logwyz ...ss temp[0]=[%d] temp[1]=[%d] temp[2]=[%d] temp[3]=[%d]zuiNum=[%d]", temp[0], temp[1], temp[2], temp[3], zuiNum);
	//三元
	temp.clear();
	for (TILE_COLOR tc = TILE_COLOR_ZI; tc <= 6; tc = (TILE_COLOR)(tc + 1))
		temp.push_back(stats.count_by_color_by_number[TILE_COLOR_ZI- TILE_COLOR_MIN][tc - TILE_COLOR_ZI]);
	std::sort(temp.begin(), temp.begin() + 3);
	switch (temp[0])
	{
	case 1:
		zuiNum++;
		break;
	case 2:
		zuiNum += 5;
		break;
	case 3:
		zuiNum += 50;
		break;
	}
	MHLOG("Logwyz ...ff temp[0]=[%d] temp[1]=[%d] temp[2]=[%d] zuiNum=[%d]", temp[0], temp[1], temp[2], zuiNum);
	return zuiNum;
}

int ShanXiFeatureToggles::calc_common_loss(FANXING fans, tile_t last_tile, unsigned max_zone_card_count) const {
	if (fans == FAN_NONE) return 0;
	int loss = 1;
	switch (FEAT_GAME_TYPE) {

	case TuiDaoHu:
		// 这里不考虑互斥，只做暴力累加，该互斥的前面都应该互斥消掉了
		// 比如豪华七小队和七小对是互斥的，清一色和平胡是互斥的
		loss = 0;
		if (fans & FAN_PingHu) loss += 1;
		if (fans & FAN_ShiSanYao) loss += 9;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 6;
		if (fans & FAN_QiXiaoDui) loss += 3;
		if (fans & FAN_YiTiaoLong) loss += 3;
		if (fans & FAN_QingYiSe) loss += 3;
		// 平胡自摸时2倍，特殊番型自摸是3倍
		if (fans & FAN_ZiMo) loss *= (fans & FAN_PingHu) ? 2 : 3;
		break;

	case KouDian:
		loss = tile_to_points(last_tile);
		if (fans & FAN_ZiMo) loss *= 2;

		// 创建房间时有清一色加番和一条龙加番的选项，这个选项只影响计分，不影响盘末成绩显示
		if (m_playtype.b_HaoZi || m_playtype[PT_ShuangHaoZi]) {
			// 抠点：单耗子、双耗子：默认清一色、一条龙都加番
			//if (fans & FAN_YiTiaoLong) loss *= 2;
			//if (fans & FAN_QingYiSe) loss *= 2;
			// 带耗子的玩法不会进到这个分支
			if (fans & (FAN_YiTiaoLong | FAN_QingYiSe))
			{
				LLOG_ERROR("********代码错误 扣点番型fans & (FAN_YiTiaoLong | FAN_QingYiSe) = true**************");
			}
		} else {
			if (m_playtype.YiTiaoLongJiaFan && (fans & FAN_YiTiaoLong)) loss *= 2;
			if (m_playtype.b_QingYiSeJiaFan && (fans & FAN_QingYiSe)) loss *= 2;

			// 新版扣点不带耗子加番
			if (fans & FAN_HaoHuaQiXiaoDui) loss *= 4;
			if (fans & FAN_QiXiaoDui) loss *= 2;
			if (fans & FAN_ShiSanYao) loss *= 2;
		}

		//if (fans & FAN_HaoHuaQiXiaoDui) loss *= 4;
		//if (fans & FAN_QiXiaoDui) loss *= 2;
		//if (fans & FAN_ShiSanYao) loss *= 2;
		break;

	case LiSi:
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_BianKanDiao) loss += 1;
		if (fans & FAN_QueMen) loss += 1;
		if (fans & FAN_ZiMo) loss *= 2;
		break;

	case JinZhong:
		loss = 2;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 18;
		if (fans & FAN_QiXiaoDui) loss += 9;
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_PengPengHu) loss += 5;
		if (fans & FAN_BianKanDiao) loss += 1;
		if (fans & FAN_FengYiSe) loss += 20;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_CouYiSe) loss += 5;
		if (fans & FAN_MenQing) loss += 1;
		if (fans & FAN_DuanYao) loss += 1;
		//if (fans & FAN_QueMen) loss += 1;
		if (fans & FAN_ZiMo) loss *= 2;
		break;

	case TieJin:
		loss = 1;
		if (fans & FAN_ZiMo) loss *= 2;
		break;

	case GuaiSanJiao:
		// 这里不考虑互斥，只做暴力累加，该互斥的前面都应该互斥消掉了
		// 比如豪华七小队和七小对是互斥的，清一色和平胡是互斥的
		// 砍胡和谁有互斥嘛？
		loss = 0;
		if (m_playtype[MH_PT_GuaSanJiao_High_Score])
		{		
			// 大唐高分值
			if (fans & FAN_PingHu) loss += 5;
			if (fans & FAN_KanHu) loss += 10;
			if (fans & FAN_ShiSanYao) loss += 15;
			if (fans & FAN_HaoHuaQiXiaoDui) loss += 30;
			if (fans & FAN_QiXiaoDui) loss += 15;
			if (fans & FAN_YiTiaoLong) loss += 15;
			if (fans & FAN_QingYiSe) loss += 15;			
		}
		else // 大同棋牌低分值
		{
			if (fans & FAN_PingHu) loss += 1;
			if (fans & FAN_KanHu) loss += 2;
			if (fans & FAN_ShiSanYao) loss += 10;
			if (fans & FAN_HaoHuaQiXiaoDui) loss += 10;
			if (fans & FAN_QiXiaoDui) loss += 5;
			if (fans & FAN_YiTiaoLong) loss += 5;
			if (fans & FAN_QingYiSe) loss += 5;
		}
		// 连庄加分
		if (fans & FAN_Zhuang) loss += m_lianzhuang_jiafen;
		break;

	case BianKaDiao:
		// 这里不考虑互斥，只做暴力累加，该互斥的前面都应该互斥消掉了
		// 比如豪华七小队和七小对是互斥的，清一色和平胡是互斥的
		loss = 0;
		if (fans & FAN_PingHu) loss += 1;
		if (fans & FAN_ShiSanYao) loss += 9;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 6;
		if (fans & FAN_QiXiaoDui) loss += 3;
		if (fans & FAN_YiTiaoLong) loss += 3;
		if (fans & FAN_QingYiSe) loss += 3;
		// 平胡自摸时2倍，特殊番型自摸是3倍
		if (fans & FAN_ZiMo) loss *= (fans & FAN_PingHu) ? 2 : 3;
		break;

	//// 临汾一门牌  by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		loss = calc_zui_score(fans);
		MHLOG("Logwyz .. 嘴数:[%d]", loss);
		if (fans & FAN_MenQing) loss += 1;
		if (fans & FAN_QiXiaoDui) loss += 10;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 25;
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_QingLong) loss += 50;
		if (fans & FAN_QingQiDui) loss += 50;
		if (fans & FAN_QingHaoHuaQiXiaoDui) loss += 50;
		if (fans & FAN_ZiYiSe) loss += 50;
		//判断端锅
		if (m_playtype[MH_PT_Linfen_DuanGuo] && loss >= 50) loss = 50;
		break;
	case HongTongWangPai: //Suijun, 根据番型计算分数		 
		loss = 1;		
		if (fans & FAN_SanFeng_X1) loss += 1;
		if (fans & FAN_SanYuan_X1) loss += 1;
		if (fans & FAN_SanFeng_X2) loss += 2;
		if (fans & FAN_SanYuan_X2) loss += 2;
		if (fans & FAN_SanFeng_X3) loss += 3;
		if (fans & FAN_SanYuan_X3) loss += 3;
		if (fans & FAN_SanFeng_X4) loss += 4;
		if (fans & FAN_SanYuan_X4) loss += 4;
		if (fans & FAN_CouYiSe) loss += 2;       //凑一色，色牌，带风，带字
		if (fans & FAN_QiXiaoDui) loss += 10;    //七小对， 王牌当本来牌用 
		if (fans & FAN_YiTiaoLong) loss += 10;   //一条龙，包括带风   
		if (fans & FAN_HongTong_LouShangLou) loss += 10;	 //楼外楼
		if (fans & FAN_QingYiSe) loss += 20;     //纯一色，清一色，无字，无风
		if (fans & FAN_ZiYiSe) loss += 30;       //字一色，无万，条，筒  
		if (fans & FAN_HongTong_YingKou) loss += 4; //硬扣
		if (fans & FAN_HongTong_DiaoWang) loss += 2;
		//MHLOG("*****************************HongTongWangPai根据番型计算输掉分数  GameType=%d loss=%d", FEAT_GAME_TYPE, loss);
		//MHLOG("*****************************Fans= %s", malgo_format_fans(fans).c_str());
		break;
	//忻州扣点：番型计算分数（**Ren 2017-11-28）
	case XinZhouKouDian:
		loss = tile_to_points(last_tile);
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_QiXiaoDui) loss += 10;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 10;     //（**Ren 2017-12-21）
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_ZiYiSe) loss += 10;
		if (fans & FAN_QingLong) loss += 20;
		if (fans & FAN_QingQiDui) loss += 20;
		if (fans & FAN_QingHaoHuaQiXiaoDui) loss += 20;  //（**Ren 2017-12-21）
		if (fans & FAN_ZiMo) loss *= 2;
		break;
	//撵中子：番型计算分数（**Ren 2017-11-28）
	case NianZhongZi:
		loss = 0;
		if (fans & FAN_BianZhang) loss += 1;
		if (fans & FAN_KanZhang) loss += 1;
		if (fans & FAN_DiaoZhang) loss += 1;
		if (fans & FAN_DuanYao) loss += 1;
		if (fans & FAN_MenQing) loss += 1;
		if (fans & FAN_QueMen) loss += 1;
		if (fans & FAN_CouYiSe) loss += 5;
		if (fans & FAN_ZiYiSe) loss += 10;
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_QingLong) loss += 20;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_ShiSanYao) loss += 50;
		if (fans & FAN_ZiMo) loss *= 2;
		break;
	case LingChuanMaJiang:
		loss = max_zone_card_count;		
		if (fans & FAN_ZiMo) loss *= 2;
		break;
	case JinChengMaJiang:
		if (fans & FAN_ZiMo) loss *= 2;      //自摸分数加倍
		if (fans & (FAN_QiXiaoDui|FAN_HaoHuaQiXiaoDui|FAN_ShiSanYao)) loss *= 2; //特殊牌型分数加倍
		break;
	}

	return loss;
}

int ShanXiFeatureToggles::calc_loss_factor(FANXING fans) {
	int factor = 1;
	if (TieJin == FEAT_GAME_TYPE) {
		// 七小对和豪华七小队在胡牌算法中就是互斥的，所以这里直接暴力写就行了
		if (fans & FAN_QiXiaoDui) factor *= 2;
		if (fans & FAN_HaoHuaQiXiaoDui) factor *= 4;
		if (fans & FAN_YiTiaoLong) factor *= 2;
		if (fans & FAN_QingYiSe) factor *= 2;
		if (fans & FAN_ShiSanYao) factor *= 2;
	}
	MHLOG("*****根据番型计算Factor  GameType=%d factor=%d", FEAT_GAME_TYPE, factor);
	return factor;
}


// 定庄规则
// 在一炮多响时，定庄是按首要赢的玩家计算
int ShanXiFeatureToggles::calc_next_zhuangpos(int curpos, int winpos, int total_gangs) {

	MHLOG("*****************************计算庄家位置************************");
	if (winpos == INVAILD_POS) { // 无人赢
		if (TieJin == FEAT_GAME_TYPE) {
#if 1
			return GetNextPos(m_zhuangpos); // 庄家的下一家坐庄
#else
			// 大唐贴金：有杠荒庄则下家坐庄，没有杠则连庄。这个是按大唐的规则写的。
			if (total_gangs) return GetNextPos(m_zhuangpos); // 庄家的下一家坐庄
			else return m_zhuangpos;
#endif
		}
		//if (GuaiSanJiao == FEAT_GAME_TYPE) return curpos; // 大唐拐三角：若荒庄则摸到最后一张牌的人坐庄
		if (GuaiSanJiao == FEAT_GAME_TYPE) { 
			++m_lianzhuang_jiafen;     // 大唐拐三角荒庄也继续加连庄分
			return m_zhuangpos; }
		if (LinFenYiMenPai == FEAT_GAME_TYPE && m_playtype[MH_PT_HuangZhuangLunZhuang])
		{
			return GetNextPos(m_zhuangpos);
		}
		if (YingSanZui == FEAT_GAME_TYPE && m_playtype[MH_PT_HuangZhuangLunZhuang])
		{
			return GetNextPos(m_zhuangpos);
		}

		switch (FEAT_GAME_TYPE) {
		case TuiDaoHu:
			return curpos; // 若荒庄则摸到最后一张牌的人坐庄
		case PingLu:
		case YingSanZui:
		case HongTongWangPai:
		//// 临汾一门牌  by wyz
		case LinFenYiMenPai:
		case LingChuanMaJiang:
		case JinChengMaJiang:
			return m_zhuangpos; // 荒庄时庄家继续坐庄
		}
		// 荒庄时的定庄规则：默认庄家的下一家坐庄
		return GetNextPos(m_zhuangpos); // 庄家的下一家坐庄
	} else if (m_zhuangpos == winpos) { // 庄赢
		// 庄赢加一，闲赢清零，荒庄不变
		++m_lianzhuang_jiafen;
		// 庄赢时的定庄规则：默认庄家继续坐庄
		return winpos;
	} else { // 闲赢
		// 清空连庄计分
		m_lianzhuang_jiafen = 0;
		// 闲赢时的定庄规则：默认是获胜闲家坐庄
		switch (FEAT_GAME_TYPE) {
		case JinZhong:
		case TieJin:
		//case GuaiSanJiao: 拐三角谁赢谁是庄
		case YingSanZui:
		case HongTongWangPai:
		//// 临汾一门牌  by wyz
		case LinFenYiMenPai:
		case LingChuanMaJiang:
		case JinChengMaJiang:
			return GetNextPos(m_zhuangpos); // 庄家的下一家坐庄
		default:
			return winpos; // 首要的获胜闲家下盘坐庄
		}
	}
}

int ShanXiFeatureToggles::init_desk_card_remain_count()
{
	switch (FEAT_GAME_TYPE)
	{
	case LingChuanMaJiang:
		m_remain_card_count = 12;
		break;
	case XinZhouKouDian:
		m_remain_card_count = 12;
		break;
	default:
		m_remain_card_count = 0;
		break;
	}
	if (FEAT_GAME_TYPE == HongTongWangPai && (!m_playtype[PT_HongTongBuKePengPai]))
	{
		m_remain_card_count = 14;
	}
	return m_remain_card_count;
}

