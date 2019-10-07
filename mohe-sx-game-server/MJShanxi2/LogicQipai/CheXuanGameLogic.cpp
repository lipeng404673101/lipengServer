#include "CheXuanGameLogic.h"

/* 扑克定义 */
int CXGameLogic::m_cardsPool[CHEXUAN_CELL_PACK] =
{
	0x0C,0x2C, 0x02,0x22, 0x08,0x28, 0x04,0x24,				/* 天(2红Q) > 地(2红2) > 人(2红8) > 和(2红4) */
	0x1A,0x3A, 0x14,0x34, 0x16,0x36,						/* 梅十(2黑10) = 板凳(2黑4) = 长三(2黑6) */
	0x1B,0x3B, 0x0A,0x2A, 0x06,0x26, 0x07,0x27,				/* 斧头(2黑J) = 苕十(2红10) = 猫猫(2红6) = 膏药(2红7) */
	0x19,0x39, 0x18,0x38, 0x17,0x37, 0x15,0x35, 0x23, 0x4F	/* 下四滥(2黑9、2黑8、2黑7、2黑5) = 丁丁(1红桃3) = 二皇(1大王) */
};

/* 构造函数 */
CXGameLogic::CXGameLogic()
{
	m_is_allow_di_jiu = false;
}

/* 析构函数 */
CXGameLogic::~CXGameLogic()
{

}

/* 获取地九王选项 */
bool CXGameLogic::getDiJiu()
{
	return m_is_allow_di_jiu;
}

/* 设置地九王选项 */
void CXGameLogic::setDiJiu(bool diJiu)
{
	m_is_allow_di_jiu = diJiu;
}

/* 获取牌的数值 */
int CXGameLogic::getCardValue(int card)
{
	return card & LOGIC_MASK_VALUE;
}

/* 获取牌的花色 */
int CXGameLogic::getCardColor(int card)
{
	return card & LOGIC_MASK_COLOR;
}

/* 获取颜色类型：方块&红桃 = 红  梅花&黑桃 = 黑 */
int CXGameLogic::getCardColorType(int card)
{
	int t_color = getCardColor(card);

	if (COLOR_FANG_KUAI == t_color || COLOR_HONG_TAO == t_color)
	{
		return COLOR_HONG;
	}

	else if (COLOR_MEI_HUA == t_color || COLOR_HEI_TAO == t_color)
	{
		return COLOR_HEI;
	}

	else
	{
		return -1;
	}
}

/* 是否为有效牌值 */
bool CXGameLogic::isValidCard(int card)
{
	return false;
}

/* 获取单牌逻辑值 */
SINGLE_TYPE CXGameLogic::getLogicCardValue(int card)
{
	if (card == 0) return STYTE_ERROR;

	//花色
	int t_color = getCardColor(card);

	//花色类型
	int t_color_type = getCardColorType(card);

	//牌值
	int t_value = getCardValue(card);


	//天牌类型：方块Q  红桃Q
	if (12 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_TIAN;
	}

	//地牌类型：方块2  红桃2
	if (2 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_DI;
	}

	//人牌类型：方块8  红桃8
	if (8 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_REN;
	}

	//和牌类型：方块4  红桃4
	if (4 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_HE;
	}

	//梅十：梅花10  黑桃10、板凳：梅花4  黑桃4、长三：梅花6  黑桃6
	if (COLOR_HEI == t_color_type &&
		(10 == t_value || 4 == t_value || 6 == t_value))
	{
		return STYPE_MEI_BAN_SAN;
	}

	//斧头：梅花J  黑桃J ，苕十：方块10  红条10 ，猫猫：方块6  红桃6 ， 膏药：方块7  红桃7
	if ((11 == t_value && COLOR_HEI == t_color_type) ||
		(COLOR_HONG == t_color_type && (10 == t_value || 6 == t_value || 7 == t_value)))
	{
		return STYPE_FU_SHI_MAO_GAO;
	}

	//下四滥 黑9  黑8  黑7  黑5  红桃3  大王
	return STYPE_XIA_SI_LAN;
}

/* 获取对牌类型 */
DOUBLE_TYPE CXGameLogic::getDoubleCardsType()
{
	return (DOUBLE_TYPE)0;
}

/* 获取对牌值 */
DOUBLE_TYPE_VALUE CXGameLogic::getDoubleCardsTypeValue(int card1, int card2)
{
	//花色
	int t_color1 = getCardColor(card1);
	int t_color2 = getCardColor(card2);

	//花色类型
	int t_color_type1 = getCardColorType(card1);
	int t_color_type2 = getCardColorType(card2);

	//牌值
	int t_value1 = getCardValue(card1);
	int t_value2 = getCardValue(card2);

	//丁二皇：红桃3 + 大王
	if ((card1 == 0x23 && card2 == 0x4F) || (card1 == 0x4F && card2 == 0x23)) return DTV_DING_ER_HUANG;

	//天牌对：2张红Q
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (12 == t_value1 && t_value1 == t_value2)) return DTV_TIAN_DUI;

	//地牌对：2张红2
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (2 == t_value1 && t_value1 == t_value2)) return DTV_DI_DUI;

	//人牌对：2张红8
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (8 == t_value1 && t_value1 == t_value2)) return DTV_REN_DUI;

	//和牌对：2张红4
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (4 == t_value1 && t_value1 == t_value2)) return DTV_HE_DUI;

	//梅板三对牌：黑4对、黑6对、黑10对
	else if (
		(COLOR_HEI == t_color_type1 && t_color_type1 == t_color_type2) &&
		((4 == t_value1 && t_value1 == t_value2) || (6 == t_value1 && t_value1 == t_value2) || (10 == t_value1 && t_value1 == t_value2))
		)
	{
		return DTV_MEI_BAN_SAN_DUI;
	}

	//斧十猫膏对：黑J对、红10对、红6对、红7对
	else if (
		((COLOR_HEI == t_color_type1 && t_color_type1 == t_color_type2) && (11 == t_value1 && t_value1 == t_value2)) ||
		((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (10 == t_value1 && t_value1 == t_value2)) ||
		((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (6 == t_value1 && t_value1 == t_value2)) ||
		((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (7 == t_value1 && t_value1 == t_value2))
		)
	{
		return DTV_FU_SHI_MAO_GAO_DUI;
	}

	//下四滥对：黑9对、黑8对、黑7对、黑5对
	else if ((COLOR_HEI == t_color_type1 && t_color_type1 == t_color_type2) &&
		(
		(9 == t_value1 && t_value1 == t_value2) ||
			(8 == t_value1 && t_value1 == t_value2) ||
			(7 == t_value1 && t_value1 == t_value2) ||
			(5 == t_value1 && t_value1 == t_value2))
		)
	{
		return DTV_XIA_SI_LAN_DUI;
	}

	//天九王：红Q（天牌） + 黑9
	else if ((COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1))
	{
		return DTV_TIAN_JIU_WANG;
	}

	//地九王：红2（地牌） + 黑9 （需要选项开关支持）
	else if ( m_is_allow_di_jiu &&
		((COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1))
		)
	{
		return DTV_DI_JIU_WANG;
	}

	//天杠：红Q（天牌）+ 红8（人牌） 或  红Q（天牌）+ 黑8
	else if ((COLOR_HONG == t_color_type1 && 12 == t_value1 && 8 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 12 == t_value2 && 8 == t_value1))
	{
		return DTV_TIAN_GANG;
	}

	//地杠：红2（地牌）+ 红8（人牌）|| 红2（地牌）+ 黑8
	else if ((COLOR_HONG == t_color_type1 && 2 == t_value1 && 8 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 2 == t_value2 && 8 == t_value1))
	{
		return DTV_DI_GANG;
	}

	//天官九：红Q（天牌）+ 红黑7
	else if ((COLOR_HONG == t_color_type1 && 12 == t_value1 && 7 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 12 == t_value2 && 7 == t_value1))
	{
		return DTV_DIAN_TIAN_GUAN_JIU;
	}

	//地官九：红2（地牌）+ 红黑7
	else if ((COLOR_HONG == t_color_type1 && 2 == t_value1 && 7 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 2 == t_value2 && 7 == t_value1))
	{
		return DTV_DIAN_DI_GUAN_JIU;
	}

	//人官九：红8（人牌）+ 黑J（斧头）
	else if ((COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1))
	{
		return DTV_DIAN_REN_GUAN_JIU;
	}

	//和官九：红4（和牌）+ 黑5
	else if ((COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1))
	{
		return DTV_DIAN_HE_GUAN_JIU;
	}

	//梅十九：黑10（梅十）+ 黑9 || 黑4（板凳）+ 黑5 || 红桃3（丁丁）+ 黑6（长三）
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 6 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 6 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_JIU;
	}

	//乌龙九：黑J（斧头）+ 黑8 || 红10（苕十）+ 黑9 || 红桃3（丁丁）+ 红6（猫猫）
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1)
		)
	{
		return DTV_DIAN_WU_LONG_JIU;
	}

	//天官八：红Q（天牌）+ 红黑6 || 红Q（天牌）+ 大王
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_BA;
	}

	//地官八：红2（地牌）+ 红黑6 || 红2（地牌）+ 大王
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_BA;
	}

	//人官八：红8（人牌）+ 红黑10
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_REN_GUAN_BA;
	}

	//和官八：红4（和牌）+ 黑4（板凳）
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 4 == t_value1)
		)
	{
		return DTV_DIAN_HE_GUAN_BA;
	}

	//梅十八：黑10（梅十）+ 黑8
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_BA;
	}

	//斧七八：黑J（斧头）+ 红黑7 || 红10（苕十）+ 黑8
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && 7 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_FU_QI_BA;
	}

	//丁五八：红桃3（丁丁）+ 黑5
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_DING_WU_BA;
	}

	//天官七：红Q（天牌）+ 黑5
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_QI;
	}

	//地官七：红2（地牌）+ 黑5
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_QI;
	}

	//人官七：红8（人牌）+ 黑9
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_REN_GUAN_QI;
	}

	//和官七：红4（和牌）+ 红桃3
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_HE_GUAN_QI;
	}

	//梅十七：黑10（梅十） + 红黑7 || 黑4（板凳） + 红桃3 || 黑J（斧头） + 黑6（长三）
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && 7 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HEI == t_color_type2 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HEI == t_color_type1 && 6 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_QI;
	}

	//斧猫七：黑J（斧头）+ 红6（猫猫）|| 黑J（斧头）+ 大王 || 红10（苕十）+ 红黑7
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && 15 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_FU_MAO_QI;
	}

	//非洲七：黑9 + 黑8
	else if (
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_FEI_ZHOU_QI;
	}

	//天官六：红Q（天牌）+ 红黑4
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 4 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_LIU;
	}

	//地官六：红2（地牌）+ 红黑4
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 4 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_LIU;
	}

	//人官六：红8（人牌）+ 黑8
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_REN_GUAN_LIU;
	}

	//梅十六：黑10（梅十）+ 红黑6 || 黑10（梅十）+ 大王 || 黑6（长三）+ 红10（苕十）
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && 6 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && 15 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HONG == t_color_type2 && 10 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HONG == t_color_type1 && 10 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_LIU;
	}

	//斧五六：黑J（斧头）+ 黑5 || 红10（苕十）+ 红6（猫猫）|| 红10（苕十）+ 大王 || 红7（膏药）+ 黑9
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && 15 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_FU_WU_LIU;
	}

	//七九六：黑9 + 黑7
	else if (
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && COLOR_HEI == t_color_type2 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && COLOR_HEI == t_color_type1 && 7 == t_value1)
		)
	{
		return DTV_DIAN_QI_JIU_LIU;
	}

	//天丁五：红Q（天牌）+ 红桃3
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_WU;
	}

	//地丁五：红2（地牌）+ 红桃3
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_WU;
	}

	//人十五：红8（人牌）+ 红黑7
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_WU;
	}

	//和斧五：红4（和牌）+ 黑J（斧头）
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1)
		)
	{
		return DTV_DIAN_HE_FU_WU;
	}

	//梅十五：黑10（梅十）+ 黑5 || 黑4（板凳）+ 黑J（斧头）|| 黑6（长三）+ 黑9
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_WU;
	}

	//苕十五：红10（苕十）+ 黑5 || 红6（猫猫）+ 黑9 || 红7（膏药）+ 黑8
	else if (
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_TIAO_SHI_WU;
	}

	//七八五：黑7 + 黑8 || 黑9 + 大王
	else if (
		(COLOR_HEI == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_QI_BA_WU;
	}

	//天地公子四：红Q（天牌）+ 红2（地牌）
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HONG == t_color_type2 && 2 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HONG == t_color_type1 && 2 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_DI_SI;
	}

	//人十四：红8（人牌）+ 红黑6 || 红8（人牌）+ 大王
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_SI;
	}

	//和十四：红4（和牌）+ 红黑10
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_SI;
	}

	//梅十四：黑10（梅十）+ 黑4（板凳）|| 黑6（长三）+ 黑8 || 红10 + 黑4（板凳）
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 4 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 4 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 4 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_SI;
	}

	//丁斧四：红桃3（丁丁）+ 黑J（斧头）|| 红6（猫猫）+ 黑8 || 红7（膏药）+ 黑7
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 7 == t_value1)
		)
	{
		return DTV_DIAN_DING_FU_SI;
	}

	//五九四：黑9 + 黑5 || 黑8 + 大王
	else if (
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 8 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 8 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_WU_JIU_SI;
	}

	//天斧三：红Q（天牌）+ 黑J（斧头）
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_FU_SAN;
	}

	//地斧三：红2（地牌）+ 黑J（斧头）
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1)
		)
	{
		return DTV_DIAN_DI_FU_SAN;
	}

	//人十三：红8（人牌）+ 黑5
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_SAN;
	}

	//和十三：红4（和牌）+ 黑9
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_SAN;
	}

	//梅十三：黑10（梅十）+ 红桃3（丁丁）|| 黑4（板凳）+ 黑9 || 黑6（长三）+ 红黑7
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_SAN;
	}

	//苕十三：红10（苕十）+ 红桃3（丁丁）|| 红6（猫猫）+ 红黑7 || 红7（膏药） + 大王
	else if (
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && 7 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_TIAO_SHI_SAN;
	}

	//五八三：黑8 + 黑5 || 黑7 + 大王
	else if (
		(COLOR_HEI == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 7 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 7 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_WU_BA_SAN;
	}

	//天十二：红Q（天牌）+ 红黑10
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_SHI_ER;
	}

	//地十二：红2（地牌）+ 红黑10
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_DI_SHI_ER;
	}

	//人十二：红8（人牌）+ 红黑4
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 4 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_ER;
	}

	//和十二：红4（和牌）+ 黑8
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_ER;
	}

	//四八二：黑4（板凳）+ 黑8 || 黑6（长三）+ 红6（猫猫）|| 黑6（长三）+ 大王
	else if (
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_SI_BA_ER;
	}

	//膏药二：红7（膏药）+ 黑5 || 红6（猫猫）+ 大王
	else if (
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_GAO_YAO_ER;
	}

	//丁九二：红桃3（丁丁）+ 黑9 || 黑7 + 黑5
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_DING_JIU_ER;
	}

	//地九一：红2（地牌）+ 黑9
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_DI_JIU_YI;
	}

	//人十一：红8（人牌）+ 红桃3（丁丁）
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_YI;
	}

	//和十一：红4（和牌）+ 红黑7
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_YI;
	}

	//梅十一：黑10（梅十）+ 黑J（斧头）|| 黑4（板凳）+ 红黑7 || 黑6（长三）+ 黑5
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && 7 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_YI;
	}

	//斧十一：黑J（斧头）+ 红10（苕十）|| 红6（猫猫）+ 黑5
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HONG == t_color_type2 && 10 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HONG == t_color_type1 && 10 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_FU_SHI_YI;
	}

	//丁八一：红桃3（丁丁）+ 黑8 || 黑5 + 大王
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 5 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 5 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_DING_BA_YI;
	}

	else
	{
		return DTV_DIAN_LING;
	}

}

/* 检测是否为三花牌型 */
bool CXGameLogic::checkSanHuaType(const int* handCards, int handCardsCount)
{
	if (handCards == NULL || handCardsCount < 3)
	{
		return false;
	}
	int t_color_type = COLOR_NT;
	int t_value = 0;

	//判断是否为三花十：红10、黑10、黑J
	int t_sanHuaShiCount = 0;
	int t_hongShiCount = 1;
	int t_heiShiCount = 1;
	int t_heiGouCount = 1;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//查找红10
		if (t_hongShiCount == 1 && t_color_type == COLOR_HONG && t_value == 10)
		{
			++t_sanHuaShiCount;
			t_hongShiCount = 0;
			continue;
		}
		//查找黑10
		else if (t_heiShiCount == 1 && t_color_type == COLOR_HEI && t_value == 10)
		{
			++t_sanHuaShiCount;
			t_heiShiCount = 0;
			continue;
		}
		//查找黑J
		else if (t_heiGouCount == 1 && t_color_type == COLOR_HEI && t_value == 11)
		{
			++t_sanHuaShiCount;
			t_heiGouCount = 0;
			continue;
		}
	}
	if (t_sanHuaShiCount == 3) return true;

	//判断是否为三花六：红6、黑6、大王
	int t_sanHuaLiuCount = 0;
	int t_hongLiuCount = 1;
	int t_heiLiuCount = 1;
	int t_daWangCount = 1;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//查找红6
		if (t_hongLiuCount == 1 && t_color_type == COLOR_HONG && t_value == 6)
		{
			++t_sanHuaLiuCount;
			t_hongLiuCount = 0;
			continue;
		}
		//查找黑6
		else if (t_heiLiuCount == 1 && t_color_type == COLOR_HEI && t_value == 6)
		{
			++t_sanHuaLiuCount;
			t_heiLiuCount = 0;
			continue;
		}
		//查找大王
		else if (t_daWangCount == 1 && handCards[i] == 0x4F)
		{
			++t_sanHuaLiuCount;
			t_daWangCount = 0;
			continue;
		}
	}
	if (t_sanHuaLiuCount == 3) return true;

	return false;
}

/* 检测是否为三花成对 */
bool CXGameLogic::checkSanHuaDouble(const int* handCards, int handCardsCount)
{
	if (handCards == NULL || handCardsCount != 4 && !checkSanHuaType(handCards, handCardsCount))
	{
		return false;
	}

	int t_color_type = COLOR_NT;
	int t_value = 0;

	//判断是否为三花十成对：红10、黑10、黑J
	int t_hongShiCount = 0;
	int t_heiShiCount = 0;
	int t_heiGouCount = 0;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//查找红10
		if (t_color_type == COLOR_HONG && t_value == 10)
		{
			++t_hongShiCount;
			continue;
		}
		//查找黑10
		else if (t_color_type == COLOR_HEI && t_value == 10)
		{
			++t_heiShiCount;
			continue;
		}
		//查找黑J
		else if (t_color_type == COLOR_HEI && t_value == 11)
		{
			++t_heiGouCount;
			continue;
		}
	}
	if (t_hongShiCount + t_heiShiCount + t_heiGouCount == 4)
	{
		return true;
	}

	//判断是否为三花六成对：红6、黑6、大王
	int t_hongLiuCount = 0;
	int t_heiLiuCount = 0;
	int t_daWangCount = 0;
	int t_hongSanCount = 0;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//查找红6
		if (t_color_type == COLOR_HONG && t_value == 6)
		{
			++t_hongLiuCount;
			continue;
		}
		//查找黑6
		else if (t_color_type == COLOR_HEI && t_value == 6)
		{
			++t_heiLiuCount;
			continue;
		}
		//查找大王
		else if (handCards[i] == 0x4F)
		{
			++t_daWangCount;
			continue;
		}
		//查找红桃3
		else if (handCards[i] == 0x23)
		{
			++t_hongSanCount;
		}
	}

	if (t_hongLiuCount + t_heiLiuCount + t_daWangCount + t_hongSanCount == 4)
	{
		return true;
	}

	return false;
}

/* 校验组手牌是否一样 */
bool CXGameLogic::checkTwoHandCardsIsSame(const int* srcCards, const int* desCards, int cardCount)
{
	if (cardCount < 1) return false;

	bool t_flag = false;
	for (int i = 0; i < cardCount; ++i)
	{
		t_flag = false;
		for (int j = 0; j < cardCount; ++j)
		{
			if (srcCards[i] == desCards[j])
			{
				t_flag = true;
				break;
			}
		}
		if (!t_flag)
		{
			return false;
		}
	}
	return true;
}

/* 排列手牌中的两组牌，大的在前 */
bool CXGameLogic::sortHandCardsByDouble(int* handCards, int cardCount, int* handCardsType)
{
	if (handCards == NULL || cardCount != 4)
	{
		return false;
	}

	int t_perType = getDoubleCardsTypeValue(handCards[0], handCards[1]);
	int t_afterType = getDoubleCardsTypeValue(handCards[2], handCards[3]);
	handCardsType[0] = t_perType;
	handCardsType[1] = t_afterType;
	int t_card1 = 0;
	int t_card2 = 0;
	if (t_afterType > t_perType)
	{
		t_card1 = handCards[0];
		t_card2 = handCards[1];
		handCards[0] = handCards[2];
		handCards[1] = handCards[3];
		handCards[2] = t_card1;
		handCards[3] = t_card2;

		handCardsType[0] = t_afterType;
		handCardsType[1] = t_perType;
	}
	return true;
}

/* 按照第二类型大小排序所有参与玩家的手牌，方便后面比较 */
bool CXGameLogic::sortAllPlayerCards(HandCardsTypeInfo info[], int infoCount, int firstDangPos, int playerCount)
{
	if (info == NULL || infoCount < 1 || playerCount <= 0)
	{
		return false;
	}

	for (int i = 0; i < infoCount - 1; ++i)
	{
		for (int j = 0; j < infoCount - i - 1; ++j)
		{
			//按第二类型排序
			if (info[j].m_secondType < info[j + 1].m_secondType)
			{
				HandCardsTypeInfo t_tmp = info[j];
				info[j] = info[j + 1];
				info[j + 1] = t_tmp;
			}
			//如果第二类型相同，按第一类型大小排列
			else if (info[j].m_secondType == info[j + 1].m_secondType && info[j].m_firstType < info[j + 1].m_firstType)
			{
				HandCardsTypeInfo t_tmp = info[j];
				info[j] = info[j + 1];
				info[j + 1] = t_tmp;
			}
			//如果第二类型和第一类型都相同，则离发牌人近的玩家排在前面
			else if (info[j].m_secondType == info[j + 1].m_secondType && info[j].m_firstType == info[j + 1].m_firstType)
			{
				//离挡家最近的位置
				int t_dangJinPos = firstDangPos;
				for (int k = firstDangPos; k < infoCount + firstDangPos; ++k)
				{
					int t_pos = k % playerCount;
					if (t_pos == j)
					{
						t_dangJinPos = j;
						break;
					}
					if (t_pos == j + 1)
					{
						t_dangJinPos = j + 1;
						break;
					}
				}

				//j+1离挡家近，则将j+1排在前面
				if (t_dangJinPos == j + 1)
				{
					HandCardsTypeInfo t_tmp = info[j];
					info[j] = info[j + 1];
					info[j + 1] = t_tmp;
				}
			}
		}
	}

	int t_sameBegin = 0;
	int t_sameCount = 0;
	for (int i = 0; i < infoCount - 1; ++i)
	{
		if (info[i].m_firstType == info[i + 1].m_firstType && info[i].m_secondType == info[i + 1].m_secondType)
		{
			t_sameBegin = i;
			t_sameCount = 0;

			//查找之后又几个相同的
			for (int j = t_sameBegin; j < infoCount - 1; ++j)
			{
				if (info[j].m_firstType == info[j + 1].m_firstType && info[j].m_secondType == info[j + 1].m_secondType)
				{
					++t_sameCount;
				}
				else
				{
					break;
				}
			}

			for (int j = t_sameBegin; t_sameCount >= 0; ++j)
			{
				info[j].m_isSameNext = t_sameCount + 1;
				--t_sameCount;
				++i;
			}
		}
	}
}

/* 查找最大牌对应的位置 */
int CXGameLogic::getMaxCardPos(const int* srcCards, int cardCount, int playerCount, int firstSpeckPos)
{
	if (cardCount <= 0 || srcCards == NULL)
	{
		return -1;
	}
	int t_maxCard = 0;
	for (int i = 0; i < cardCount; ++i)
	{
		if (getLogicCardValue(srcCards[i]) > getLogicCardValue(t_maxCard))
		{
			t_maxCard = srcCards[i];
		}
	}

	for (int i = firstSpeckPos; i < playerCount + firstSpeckPos; ++i)
	{
		int t_pos = i % playerCount;
		if (getLogicCardValue(t_maxCard) == getLogicCardValue(srcCards[t_pos])) return t_pos;
	}
	return 0;
}

/* 计算两张牌的点数 */
int CXGameLogic::getDoubleCardsNum()
{
	return 0;
}

/* 洗牌 */
void CXGameLogic::RandCardList(int cardBuffer[], int cardBufferCount)
{
	//混乱准备
	int cbCardData[CHEXUAN_CELL_PACK];

	memcpy(cbCardData, m_cardsPool, sizeof(m_cardsPool));

	//混乱扑克
	int cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cardBufferCount - cbRandCount);
		cardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cardBufferCount - cbRandCount];
	} while (cbRandCount < cardBufferCount);

	return;
}