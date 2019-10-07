#pragma once


#include <cstring>
#include <vector>
#include <algorithm>
#include "mhmsghead.h"
/*
这里设置游戏类型编号，会自动注册为 GameHandler，自动给 Desk 设定人数

C 发 LMsgC2SCreateDesk 到 LM
LM 发 LMsgLMG2LCreateDesk 到 L
L 调用 CreateVipDesk 创建桌子
m_state 参数是原样从 C 转发到 L 的，但 LM 也会使用这个值

拼音名 编号 人数
*/
/*
	by wyz 20171128 添加扑克类游戏，游戏编号用101-999
*/

#define POKER_GAME_TYPE_MAP(XX) \
	XX(DouDiZhu			, 101, 3) \
	XX(YingSanZhang			, 102, 5) \
	XX(NiuNiu			, 103, 10) \
    XX(TuiTongZi			, 104, 8) \
	XX(ShuangSheng          , 106, 4) \
	XX(SanDaEr 				, 107, 5) \
	XX(NiuNiu_New			, 108, 10) \
	XX(SanDaYi				, 109, 4) \
    XX(WuRenBaiFen          , 110, 5) \
	XX(CheXuan              , 111, 8) \
	/*END*/


// GameType
enum {
#define XX(k, v, player_count) k = v,
	POKER_GAME_TYPE_MAP(XX)
#undef XX
};

//enum  PLAYTYPE
//{
//	PT_NONE = 0,
//	PT_BaoTing = 1, // 报听
//	PT_DaiFeng = 2, // 带风
//	PT_ZhiKeZiMoHu = 3, // 只可自摸和
//	PT_QingYiSeJiaFan = 4, // 清一色加番
//	PT_YiTiaoLongJiaFan = 5, // 一条龙加番
//	PT_ZhuoHaoZi = 6, // 捉耗子
//
//	PT_GuoHuZhiKeZiMo = 7, // 过胡只可自摸
//	PT_QiangGangHu = 8, // 抢杠胡
//	PT_HuangZhuangBuHuangGang = 9, // 杠不随胡，改名，荒庄不荒杠
//	PT_WeiShangTingGangBuSuanFen = 10, // 未上听杠不算分
//	PT_YiPaoDuoXiang = 11, // 一炮多响
//
//	// 晋中小选项需求：七小对、风一色、清一色、凑一色、过胡只可自摸、荒庄不荒杠、听牌可杠、只有胡牌玩家杠算分
//	// 做晋中新增小选项
//	PT_TingPaiKeGang = 12, // 听牌可杠
//	PT_QiXiaoDui = 13, // 七小对
//	PT_ZhiYouHuPaiWanJiaGangSuanFen = 14, // 只有胡牌玩家杠算分，改名，杠随胡家
//	PT_FengYiSe = 15, // 风一色
//	PT_QingYiSe = 16, // 清一色，不是清一色加番
//	PT_CouYiSe = 17, // 凑一色
//	PT_AnGangKeJian = 18, // 暗杠可见
//
//	// 贴金小选项需求：七小对、一条龙、清一色、十三幺、未上金者只可自摸、4金、8金
//	// 做贴金新增小选项
//	PT_ShiSanYao = 19, // 十三幺
//	PT_YiTiaoLong = 20, // 一条龙
//	PT_SiJin = 21, // 4金
//	PT_BaJin = 22, // 8金
//	PT_ShangJinShaoZheZhiKeZiMo = 23, // 上金少者只可自摸
//
//	// 拐三角小选项需求：七小对、十三幺、一炮多响（默认选中）
//
//	// 2017-3-14: 推倒胡补充小选项
//	PT_BianKanDiao = 24, // 边坎吊
//
//	// 做平鲁新增小选项编号
//	PT_WeiShangTingDianPaoBaoPei = 25, // 未上听点炮包赔
//	PT_YiWuJiu = 26, // 一五九
//	PT_DongLingTou = 27, // 东领头
//
//	// 做硬三嘴新增小选项编号
//  //// by wyz 20170906 PT_ZhuangJiaYiFen 作以下说明，庄加一分的意思是在设定的庄家风险分基础上加一分，且庄家风险分不为0：FEAT_DealerRiskScore+1。
//	////为了处理默认情况庄家一分，用户可以选择两分的情况,具体实现get_DealerRiskScore()
//	////是否启用庄家风险分，可以用FEAT_DealerRiskScoreDisabled设置,或者小选项PT_ZhuangSuanFen控制
//	PT_ZhuangJiaYiFen = 28, // 庄加1分：和“庄算分”功能相同，都表示启用该玩法的庄算分
//	PT_ZiYiSe = 29, // 字一色
//	PT_QingLong = 30, // 青龙
//	PT_QingQiXiaoDui = 31, // 清七小对
//	PT_HaoHuaQiXiaoDui = 32, // 豪华七小对
//
//	// 做洪洞王牌新增小选项编号
//	PT_GangBuSuiHu = 33, // 杠不随胡
//	PT_QueLiangMen = 34, // 缺两门
//	PT_ZiPai = 35, // 字牌
//	PT_SanYuanZiMo = 36, // 三元自摸
//	PT_DiaoWang = 37, // 吊王
//	PT_QingHaoHuaQiXiaoDui = 38, // 清豪华七小对
//
//	// 做临汾一门牌新增小选项编号
//	PT_ShuYe = 39, // 数页
//	PT_YiZhangYing = 40, // 一张赢
//	PT_SanYuan = 41, // 三元
//	PT_ZiYiSeQiDaDui = 42, // 字一色七大对
//	PT_ShuangHaoHuaQiXiaoDui = 43, // 双豪华七小对
//	PT_SanFeng = 44, // 三风
//
//	// 2017-3-20: 抠点补充小选项
//	PT_ShuangHaoZi = 45, // 双耗子
//	PT_ZhuangSuanFen = 46, // 庄算分
//
//	// 2017-3-24: 抠点补充小选项
//	PT_YiTiaoLongKeHuSanSiWu = 47, // 一条龙可胡345
//	// 抠点：听口计算时，首先这些胡牌必须都是3点以上的
//	// 如果没有6点以上的胡牌，这个听口就作废
//	// 现在呢，如果某个胡牌是胡的一条龙，并且是胡345，就保留这个胡法，然后听口不作废
//	// 之后呢，也只能自摸胡，不能被别人点345的
//	PT_HongTongBuKePengPai = 48, // 洪洞王牌不可碰小项	
//
//	// 2017-10-8: 扣点补充小选项
//	MH_PT_FengHaoZi = 49, // 扣点用牌做耗子
//
//	// 2017-11-3: 拐三角添加小选项
//	MH_PT_YingBaZhang = 50,              // 硬八张，胡牌时必须有一门牌数大于8张
//	MH_PT_LianZhuangJiaFen = 51,         // 连庄加分，庄家连续胡牌加连庄分
//	MH_PT_HuangZhuangLunZhuang = 52,      // 荒庄轮庄 荒庄后下家坐庄小选项， 一门牌需要
//
//	PT_MAX = 63, // 用于 GameOptionSet
//
//	// 这俩常量定的数太大了，用于推倒胡
//	PT_PingHu = 10001, // 小胡玩法、也用于显示“平胡”字样
//	PT_DaHu = 10002,   // 
//
//};
//
//bool shanxi_game_option_validate_by_game_type(int v, int game_type);
//
//struct GameOptionSet {
//	typedef char CHECK_PT_MAX[PT_MAX <= 63 ? 1 : -1];
//	unsigned long long mask;
//	bool operator [](int pt) const { return 0 != (mask & (1ull << pt)); }
//	void set(int pt) { mask |= 1ull << pt; }
//
//	bool b_37Jia; //37夹
//	bool b_baoTing;//报听
//	bool b_daiFeng;//带风
//	bool b_zimoHu;//只可自摸和
//	bool b_HaoZi;//捉耗子
//	bool b_QingYiSeJiaFan;//清一色
//	bool YiTiaoLongJiaFan;//清一色或是一条龙
//	bool DaHu;
//	bool XiaoHu;
//
//	GameOptionSet() {
//		clear();
//	}
//	void clear() {
//		memset(this, 0, sizeof(*this));
//	}
//
//	// 对于推倒胡和扣点，服务器端限定玩法小选项开启和关闭
//	void mh_fill_default_playtype_by_GameType(int game_type)
//	{
//		switch (game_type)
//		{
//		case TuiDaoHu:
//			set(PT_QiangGangHu);
//			//set(PT_HuangZhuangBuHuangGang);
//			//set(PT_YiPaoDuoXiang);
//			break;
//		case KouDian:
//			set(PT_QiangGangHu);
//			//set(PT_HuangZhuangBuHuangGang);
//			set(PT_QiXiaoDui);			
//#ifdef _MH_ENABLE_LOG_DEBUG
//			for (int i = 0; i < PT_MAX; i++)
//			{
//				if( (*this)[i])
//				   MHLOG("*******服务器默认打开选项 option= %d, game_type=%d", i,  game_type);
//			}
//#endif // _MH_ENABLE_LOG_DEBUG			
//			break;
//		case GuaiSanJiao:
//			//set(MH_PT_YingBaZhang);
//			break;
//		default:
//			break;
//		}
//	}
//
//	bool mh_is_default_playtype_by_GameType(int game_type, int play_type_value)
//	{
//		// 对于推倒胡和扣点，服务器端限定玩法小选项开启和关闭
//		switch (game_type)
//		{
//		case TuiDaoHu:
//			if (PT_QiangGangHu == play_type_value )
//				return true;
//			break;
//		case KouDian:
//			if (PT_QiangGangHu == play_type_value || PT_QiXiaoDui == play_type_value)
//				return true;
//			break;
//		default:
//			break;
//		}
//		return false;
//	}
//	
//
//	void init_playtype_info(std::vector<int>& l_play_type, int game_type) {
//
//		MHLOG("******启用玩法小选项 playtype.size() = %d", l_play_type.size());
//		for (int i = 0; i < l_play_type.size(); i++)
//		{
//			MHLOG("*****小选项 i = %d value = %d", i, l_play_type[i]);
//		}
//		clear();
//		for (size_t x = 0; x < l_play_type.size(); x++) {
//			int v = l_play_type[x];
//			// 没有通过校验的小选项，简单忽略
//			if (!shanxi_game_option_validate_by_game_type(v, game_type)) continue;
//			MHLOG("**********验证小选项通过  value = %d", v);
//			if (v >= 0 && v <= PT_MAX) set(v);
//			switch (v) {
//			case PT_BaoTing: b_baoTing = true; break;
//			case PT_DaiFeng: b_daiFeng = true; break;
//			case PT_ZhiKeZiMoHu: b_zimoHu = true; break;
//			case PT_ZhuoHaoZi:
//			case MH_PT_FengHaoZi:       // 添加扣点风耗子
//				b_HaoZi = true; break;
//			case PT_QingYiSeJiaFan: b_QingYiSeJiaFan = true; break;
//			case PT_YiTiaoLongJiaFan: YiTiaoLongJiaFan = true; break;
//			case PT_PingHu: XiaoHu = true; break;
//			case PT_DaHu: DaHu = true; break;
//			}
//		}
//		// 增加服务默认支持的玩法选项
//		mh_fill_default_playtype_by_GameType(game_type);
//	}
//
//	void fill_vector_for_client(std::vector<int> &a, int game_type) {
//		for (int i = 0; i <= PT_MAX; ++i) {
//			if (mh_is_default_playtype_by_GameType(game_type, i)) // 服务器默认支持的选项不发送给客户端
//			{
//				MHLOG("****服务器默认打开选项，不发送给客户端 option=%d, game_type=%d", i, game_type);
//				continue;
//			}
//			if ((*this)[i]) a.push_back(i);
//		}
//		if (XiaoHu) a.push_back(PT_PingHu);
//		if (DaHu) a.push_back(PT_DaHu);
//	}
//
//
//};
//
//
//// 过几天再移除
//typedef GameOptionSet PlayTypeInfo;
//
//
////胡牌类型
//enum HU_PAI_LEIXING
//{
//	HU_NULL = 0,					//没胡
//	HU_XIAOHU = 1,					//小胡
//
//
//	HU_SPECIAL_XIAOQIDUI = 2,			//龙七对
//	HU_XIAOQIDUI = 3,					//七小对
//
//	HU_QINGYISE = 4,					//清一色
//	HU_JIANGJIANGHU = 5,				//将将胡
//	HU_PENGPENGHU = 6,				//对对胡
//	HU_QUANQIUREN = 7,				//全球人
//	HU_GANG1 = 8,						//杠上开花
//	HU_GANG2 = 9,						//杠上炮
//	HU_END1 = 10,						//海底捞(扫底胡)
//	HU_END2 = 11,						//海底炮
//	HU_QIANGGANGHU = 12,				//抢扛胡
//	HU_QISHOUFOURLAN = 13,			//起手四个赖子
//	HU_DOUBLESPECIAL_XIAOQIDUI = 14, //双豪华七小队
//	HU_TIANHU = 15,					//天胡
//	HU_DIHU = 16,						//地胡
//	HU_DANDIAO = 17,				//单钓
//	HU_JINGOUDIAO = 18,				//金钩钓
//
//	HU_QINGDUI = 19,				//清对
//	HU_QINGQIDUI = 20,				//清七对
//	HU_QINGLONGQIDUI = 21,			//清龙七对
//
//	HU_JIANGDUI = 22,               //将对
//	HU_JIANGDUI_7 = 23,              //将7对
//	HU_ONENINE = 24,                 //全幺九
//	HU_MENQING = 25,				//门清
//	HU_NOONENINE = 26,				//断幺九 中张
//
//	HU_CA2TIAO = 27,                //4人2房 卡2条
//
//	HU_CA5XIN = 28,                 //夹心5
//	HU_ONE2NINE = 29,               //一条龙
//	HU_SISTERPAIRS = 30,            //姊妹对
//	HU_TRIPLESPECIAL_XIAOQIDUI = 31,//超超豪华七小队
//
//	HU_GOLDGOUPAO = 41,             //宜宾 金钩炮
//	HU_NOGOLDCARD = 42,             //无听用
//	HU_MIDGOLD = 43,             //本金
//	HU_TING = 44,				//报听
//
//	HU_QINGSANDA = 80,				//清三搭
//	HU_QIANSI = 81,				//前四
//	HU_HOUSI = 82,				//后四
//
//	HU_BADCARD = 101,                //烂牌 字不重复 花色牌隔开两张
//	HU_QIXIN = 102,				    //七心 东南西北中发白各一个的烂牌
//	HU_YAOCARD = 103,				//幺牌 全字和19组成的牌
//	HU_JIAXINWU = 104,				//夹心五（只能听五）4番
//	HU_HUNYISE = 105,				//混一色
//	HU_DASANYUAN = 106,				//大三元
//	HU_XIAOSANYUAN = 107,			//小三元
//	HU_SHIFENG = 108,				//十风
//	HU_SHISANYAO_FEIXIAOJI = 109,	//十三幺-云南飞小鸡
//	HU_LONGZHUABEI = 110,			//龙爪背
//	HU_SIYAOJI = 111,				//四幺鸡
//	HU_GANGSHANGWUMEIHUA = 112,     //杠上五梅花
//	HU_WUJI = 113,					//无鸡
//	HU_XIAOJIGUIWEI = 114,			//小鸡归位
//	HU_DOUBLEGANG = 115,			//两杠
//
//	HU_ALLTYPE = 149,				//总番型		 --保证最大
//
//	GANG_MINGGANG = 150,			//明杠
//	GANG_ANGANG = 151,			//暗杠
//	GANG_BUGANG = 152,			//补杠
//	GANG_ZHUANYI = 153,			//呼叫转移
//	GANG_HANBAOG = 154,          //汉堡杠
//
//	HU_Qixiaodui = 1003, // 7小队
//	HU_DaQixiaodui = 1004, // 豪华7小队
//	HU_QingYiSe = 1005, // 清一色
//	HU_YiTiaoLong = 1006, // 一条龙
//	HU_ShiSanYao = 1007, // 十三幺
//
//	HU_PingHu = 10001, // 和 PT_xiaoHu 取值相同
//
//	// 做立四新增番型
//	HU_QUEMEN = 1010, // 缺门
//	HU_BIANZHANG = 1011, // 边张
//	HU_KANZHANG = 1012, // 坎张
//	HU_DIAOZHANG = 1013, // 吊张
//
//	// 做晋中新增番型
//	HU_CouYiSe = 1021, // 凑一色
//	HU_FengYiSe = 1022, // 风一色
//	HU_MenQing = 1023, // 门清
//	HU_DuanYao = 1024, // 断幺
//	HU_PengPengHu = 1025, // 碰碰胡
//
//	// 做拐三角新增番型
//	HU_KanHu = 1026, // 坎胡
//
//	// 做硬三嘴新增番型
//	HU_QingLong = 1027, // 青龙
//	HU_QingQiDui = 1028, // 清七对
//	HU_ZiYiSe = 1029, // 字一色
//	HU_QingHaoHuaQiXiaoDui = 1030, // 清豪华七小队
//	HU_QueYiMen = 1031, // 缺一门
//	HU_QueLiangMen = 1032, // 缺两门
//	HU_GuJiang = 1033, // 孤将
//	HU_YiZhangYing = 1034, // 一张赢
//	HU_DanDiao = 1035, // 单吊
//	HU_SanFeng_X1 = 1041, // 三风X1
//	HU_SanFeng_X2 = 1042, // 三风X2
//	HU_SanFeng_X3 = 1043, // 三风X3
//	HU_SanFeng_X4 = 1044, // 三风X4
//	HU_SanYuan_X1 = 1051, // 三元X1
//	HU_SanYuan_X2 = 1052, // 三元X2
//	HU_SanYuan_X3 = 1053, // 三元X3
//	HU_SanYuan_X4 = 1054, // 三元X4
//
//    // 洪洞王牌番型
//	HU_HongTong_LouShangLou = 1055, // 楼上楼
//	HU_HongTong_YingKou = 1056,
//	HU_HongTong_DiaoWang = 1057, //吊王
//
//	// 拐三角连庄 
//	HU_LianZhuang = 1058
//};



