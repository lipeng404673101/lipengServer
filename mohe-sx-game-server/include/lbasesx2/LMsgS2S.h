#ifndef _LMSG_S2S_H_
#define _LMSG_S2S_H_

#include "LMsg.h"
#include "LUser.h"
#include "LCharge.h"



//////////////////////////////////////////////////////////////////////////
//logic 跟 center 

//////////////////////////////////////////////////////////////////////////
struct LMsgL2CeLogin:public LMsg
{
	Lint	m_ID;//类型
	Lstring m_key;
	Lstring m_ServerName;
	Lint	m_needLoadUserInfo;  //center是否需要发送当前所有的玩家的信息 0:不发送 1：发送

	LMsgL2CeLogin() :LMsg(MSG_L_2_CE_LOGIN)
	{
		m_ID = 0;
		m_needLoadUserInfo = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_key);
		buff.Read(m_ServerName);
		buff.Read(m_needLoadUserInfo);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_key);
		buff.Write(m_ServerName);
		buff.Write(m_needLoadUserInfo);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeLogin();
	}
};


//////////////////////////////////////////////////////////////////////////

struct LogicInfo
{
	Lint		m_id;
	Lint		m_deskCount;
	Lstring		m_ip;
	Lshort		m_port;
	LSocketPtr	m_sp;
	Lint		m_flag;	//0,normal; -1,failover
	Lint     m_type;  // 0--logicServer  1--logigQiPai
	LogicInfo()
	{
		m_id = 0;
		m_deskCount = 0;
		m_port = 0;
		m_flag = 0;
		m_type=0;
	}
};

struct LMsgLMG2GateLogicInfo :public LMsg
{
	Lint		m_ID;
	Lint		m_count;
	LogicInfo	m_logic[256];

	LMsgLMG2GateLogicInfo() :LMsg(MSG_LMG_2_G_SYNC_LOGIC)
	{
		m_ID = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Read(m_logic[i].m_id);
			buff.Read(m_logic[i].m_deskCount);
			buff.Read(m_logic[i].m_ip);
			buff.Read(m_logic[i].m_port);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Write(m_logic[i].m_id);
			buff.Write(m_logic[i].m_deskCount);
			buff.Write(m_logic[i].m_ip);
			buff.Write(m_logic[i].m_port);
		} 
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GateLogicInfo();
	}
};

struct GateInfo
{
	Lint		m_id;
	Lint		m_userCount;
	Lstring		m_ip;
	Lshort		m_port;
	Lint        m_gaofang_flag; //1:非高防2高防
	LSocketPtr	m_sp;

	GateInfo()
	{
		m_id = 0;
		m_userCount = 0;
		m_port = 0;
		m_gaofang_flag = 0;
	}
};

struct LMsgL2CeGateInfo :public LMsg
{
	Lint		m_ID;
	Lint		m_count;
	GateInfo	m_gate[256];

	LMsgL2CeGateInfo() :LMsg(MSG_L_2_CE_GATE_INFO)
	{
		m_ID = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Read(m_gate[i].m_id);
			buff.Read(m_gate[i].m_userCount);
			buff.Read(m_gate[i].m_ip);
			buff.Read(m_gate[i].m_port);
			buff.Read(m_gate[i].m_gaofang_flag);
		}
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Write(m_gate[i].m_id);
			buff.Write(m_gate[i].m_userCount);
			buff.Write(m_gate[i].m_ip);
			buff.Write(m_gate[i].m_port);
			buff.Write(m_gate[i].m_gaofang_flag);
		} 
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeGateInfo();
	}
};


//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LUserLogin:public LMsg
{
	Lint		m_seed;
	LUser		user;

	LMsgCe2LUserLogin() :LMsg(MSG_CE_2_L_USER_LOGIN)
	{
		m_seed = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_seed);
		user.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_seed);
		user.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUserLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//center发送玩家id信息到

//玩家基本信息
struct UserIdInfo
{
	Lint		m_id;
	Lstring		m_openId;
	Lstring		m_unionId;
	Lstring		m_nike;
	Lstring		m_headImageUrl;
	Lint		m_sex;

	UserIdInfo()
	{
		m_id = 0;
		m_sex = 1;
	}
};

struct LMsgCe2LUserIdInfo:public LMsg
{
	Lint	m_count;
	std::vector<UserIdInfo> m_info;
	Lint	m_hasSentAll; //已经发送所有了的id。 0：没有 1：已经发送所有
	
	LMsgCe2LUserIdInfo() :LMsg(MSG_CE_2_L_USER_ID_INFO)
	{
		m_count = 0;
		m_hasSentAll = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			UserIdInfo info;
			buff.Read(info.m_id);  
			buff.Read(info.m_openId);
			buff.Read(info.m_unionId);
			buff.Read(info.m_nike);
			buff.Read(info.m_sex);
			buff.Read(info.m_headImageUrl);
			m_info.push_back(info);
		}
		buff.Read(m_hasSentAll);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_count = m_info.size();
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			buff.Write(m_info[i].m_id);
			buff.Write(m_info[i].m_openId);
			buff.Write(m_info[i].m_unionId);
			buff.Write(m_info[i].m_nike);
			buff.Write(m_info[i].m_sex);
			buff.Write(m_info[i].m_headImageUrl);
		}
		buff.Write(m_hasSentAll);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUserIdInfo();
	}

	virtual Lint preAllocSendMemorySize() //重新分配64K,由于消息长度是short型
	{
		return 1024 * 64;
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送保存 玩家信息
struct LMsgL2CeModifyUserNew:public LMsg
{
	Lstring			m_strUUID;
	Lint			m_value;	// 0表示新生成的用户 1表示老用户并给玩家送过卡
	LMsgL2CeModifyUserNew() :LMsg(MSG_L_2_CE_MODIFY_USER_NEW)
	{
		m_value = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_value);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_value);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeModifyUserNew();
	}
};

// Center发送更改GateInfo
struct MHLMsgCe2LUpdateGateIp : public LMsg
{
	Lint            m_server_id;
	Lint            m_gaofang_flag; // 高防标志0:原始的gateinfo, 1:阿里高防，2云漫高防
	Lstring			m_gate_ip;	   //  高防IP地址或DNS地址
	MHLMsgCe2LUpdateGateIp() :LMsg(MH_MSG_CE_2_L_UPDATE_GATE_IP)
	{
		m_server_id = 0;
		m_gaofang_flag = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_server_id);
		buff.Read(m_gaofang_flag);
		buff.Read(m_gate_ip);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_server_id);
		buff.Write(m_gaofang_flag);
		buff.Write(m_gate_ip);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new MHLMsgCe2LUpdateGateIp();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送更新 玩家金币
struct LMsgL2CeUpdateCoin :public LMsg
{
	Lstring			m_strUUID;
	Lstring         m_openID;  
	Lint            m_serverID;
	Lint            m_creatType;
/////////////////////////////////////////////

	Lint                m_credits;

	Lint				m_userid;
	Lint				m_gateId;
	Lint				m_deskID;
	Lstring				m_ip;
	LUser				m_usert;
	Lint                m_cellscore;       //底分

	Lint                m_deskType;         //创建的桌子类型
	Lint                m_feeType;          // 费用类型 ，0:房主付费 1:玩家分摊
	Lint                m_cheatAgainst;     // 是否防作弊，0:不防作弊 1：防作弊

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100
	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
											//CardValue			m_cardValue[CARD_COUNT];
	CardValue			m_Player0CardValue[13];
	CardValue			m_Player1CardValue[13];
	CardValue			m_Player2CardValue[13];
	CardValue			m_Player3CardValue[13];
	CardValue			m_SendCardValue[84];
	CardValue			m_HaoZiCardValue[2];

	Lint				m_playTypeCount;
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番
	Lint           m_Greater2CanStart;
	Lint           m_GpsLimit;          //GPS限制
	Lint		   m_allowLookOn;		//是否允许观战
	Lint			m_clubOwerLookOn;	//是否允许俱乐部会长名牌观战

	LMsgL2CeUpdateCoin() :LMsg(MSG_LMG_2_CE_UPDATE_COIN)
	{
		m_allowLookOn = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_openID);  
		buff.Read(m_serverID);
		buff.Read(m_creatType);
///////////////////////////////////////////////////

		buff.Read(m_credits);
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_deskID);
		buff.Read(m_ip);

		m_usert.Read(buff);

		buff.Read(m_cellscore);
		buff.Read(m_playTypeCount);
		buff.Read(m_deskType);
		buff.Read(m_feeType);
		buff.Read(m_cheatAgainst);

		buff.Read(m_flag);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);

		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player0CardValue[i].kColor);
			buff.Read(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player1CardValue[i].kColor);
			buff.Read(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player2CardValue[i].kColor);
			buff.Read(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player3CardValue[i].kColor);
			buff.Read(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Read(m_SendCardValue[i].kColor);
			buff.Read(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Read(m_HaoZiCardValue[i].kColor);
			buff.Read(m_HaoZiCardValue[i].kNumber);
		}
		buff.Read(m_playTypeCount);

		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_Greater2CanStart);
		buff.Read(m_GpsLimit);
		buff.Read(m_allowLookOn);
		buff.Read(m_clubOwerLookOn);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_openID);
		buff.Write(m_serverID);
		buff.Write(m_creatType);

/////////////////////////////////////////////////////
		buff.Write(m_credits);
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_deskID);
		buff.Write(m_ip);

		m_usert.Write(buff);

		buff.Write(m_cellscore);
		buff.Write(m_playTypeCount);
		buff.Write(m_deskType);
		buff.Write(m_feeType);
		buff.Write(m_cheatAgainst);


		buff.Write(m_flag);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player0CardValue[i].kColor);
			buff.Write(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player1CardValue[i].kColor);
			buff.Write(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player2CardValue[i].kColor);
			buff.Write(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player3CardValue[i].kColor);
			buff.Write(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Write(m_SendCardValue[i].kColor);
			buff.Write(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Write(m_HaoZiCardValue[i].kColor);
			buff.Write(m_HaoZiCardValue[i].kNumber);
		}
		buff.Write(m_playTypeCount);

		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_Greater2CanStart);
		buff.Write(m_GpsLimit);
		buff.Write(m_allowLookOn);
		buff.Write(m_clubOwerLookOn);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUpdateCoin();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送更新 玩家金币
struct LMsgCe2LUpdateCoin :public LMsg
{
	Lstring			m_strUUID;
	Lint            m_userID;
	Lint            m_coinNum;  //用户金币数量
	Lint            m_creatType;
	LBuffPtr		m_dataBuff;
	Lstring         m_userIp;
	Lstring         m_userGps;
	Lint            m_blocked;
	Lint            m_Free;
	Lint            m_startTime;
	Lint            m_endTime;
///////////////////////////////////////////////////////////////////////////////////

	Lint                m_credits;
	Lint				m_userid;
	Lint				m_gateId;
	Lint				m_deskID;
	Lstring				m_ip;
	LUser				m_usert;
	Lint                m_cellscore;       //底分

	Lint                m_deskType;         //创建的桌子类型
	Lint                m_feeType;          // 费用类型 ，0:房主付费 1:玩家分摊
	Lint                m_cheatAgainst;     // 是否防作弊，0:不防作弊 1：防作弊

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100
	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
											//CardValue			m_cardValue[CARD_COUNT];
	CardValue			m_Player0CardValue[13];
	CardValue			m_Player1CardValue[13];
	CardValue			m_Player2CardValue[13];
	CardValue			m_Player3CardValue[13];
	CardValue			m_SendCardValue[84];
	CardValue			m_HaoZiCardValue[2];
	Lint				m_playTypeCount;
	std::vector<Lint>	m_playType;			//玩法
	Lint                   m_Greater2CanStart;
	Lint                m_GpsLimit; // GPS: 0不限制, 1 限制

	Lint				m_allowLookOn;  //是否允许观战  0：不允许   1：允许，默认不允许0
	Lint				m_clubOwerLookOn;

	LMsgCe2LUpdateCoin() :LMsg(MSG_CE_2_LMG_UPDATE_COIN)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
		m_flag = 0;
		m_gold = 0;
		m_state = 0;
		m_robotNum = 0;
		m_cellscore = 0;
		m_playTypeCount = 0;
		m_deskType = 0;
		m_feeType = 0;
		m_cheatAgainst = 0;
		m_userIp = "";
		m_userGps = "";
		m_blocked = 0;
		m_Greater2CanStart=0;
		m_Free = 0;
		m_startTime = 0;
		m_endTime = 0;
		m_GpsLimit = 0;
		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userID);
		buff.Read(m_coinNum);
		buff.Read(m_creatType);
		buff.Read(m_userIp);
		buff.Read(m_userGps);
		buff.Read(m_blocked); 
		buff.Read(m_Free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);
///////////////////////////////////////////////////

		buff.Read(m_credits);
		buff.Read(m_userid); 
		buff.Read(m_gateId);
		buff.Read(m_deskID);
		buff.Read(m_ip);

		m_usert.Read(buff);

		buff.Read(m_cellscore);
		buff.Read(m_playTypeCount);
		buff.Read(m_deskType);
		buff.Read(m_feeType);
		buff.Read(m_cheatAgainst);

		buff.Read(m_flag);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);

		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player0CardValue[i].kColor);
			buff.Read(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player1CardValue[i].kColor);
			buff.Read(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player2CardValue[i].kColor);
			buff.Read(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player3CardValue[i].kColor);
			buff.Read(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Read(m_SendCardValue[i].kColor);
			buff.Read(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Read(m_HaoZiCardValue[i].kColor);
			buff.Read(m_HaoZiCardValue[i].kNumber);
		}

		buff.Read(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_Greater2CanStart);
		buff.Read(m_GpsLimit);
		buff.Read(m_allowLookOn);
		buff.Read(m_clubOwerLookOn);


		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userID);
		buff.Write(m_coinNum);
		buff.Write(m_creatType);
		buff.Write(m_userIp);
		buff.Write(m_userGps); 
		buff.Write(m_blocked);
		buff.Write(m_Free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);
////////////////////////////////////////////////////////
		buff.Write(m_credits);
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_deskID);
		buff.Write(m_ip);

		m_usert.Write(buff);

		buff.Write(m_cellscore);
		buff.Write(m_playTypeCount);
		buff.Write(m_deskType);
		buff.Write(m_feeType);
		buff.Write(m_cheatAgainst);


		buff.Write(m_flag);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);


		//for (Lint i = 0; i < CARD_COUNT; ++i)
		//{
		//	buff.Write(m_cardValue[i].m_color);
		//	buff.Write(m_cardValue[i].m_number);
		//}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player0CardValue[i].kColor);
			buff.Write(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player1CardValue[i].kColor);
			buff.Write(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player2CardValue[i].kColor);
			buff.Write(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player3CardValue[i].kColor);
			buff.Write(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Write(m_SendCardValue[i].kColor);
			buff.Write(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Write(m_HaoZiCardValue[i].kColor);
			buff.Write(m_HaoZiCardValue[i].kNumber);
		}
		buff.Write(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_Greater2CanStart);
		buff.Write(m_GpsLimit);

		buff.Write(m_allowLookOn);
		buff.Write(m_clubOwerLookOn);
		return true;
	}


	virtual LMsg* Clone()
	{
		return new LMsgCe2LUpdateCoin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送更新 玩家金币
struct LMsgL2CeUpdateCoinJoinDesk :public LMsg
{
	Lstring         m_openID;
	Lint            m_serverID;
	/////////////////////////////////////////////

	Lint			m_userid;
	Lint			m_gateId;
	Lstring			m_strUUID;
	Lint			m_deskID;
	Lstring			m_ip;
	LUser			m_usert;

	//添加俱乐部信息
	Lint       m_clubMasterId;  //会长id
	Lint       m_addDeskFlag;

	Lint       m_type;              // 消息来源类型  3：玩家俱乐部匹配加入房间  1:俱乐部加入桌子消息

	Lint       m_clubId;
	Lint       m_playTypeId;

	LMsgL2CeUpdateCoinJoinDesk() :LMsg(MSG_LMG_2_CE_UPDATE_COIN_JOIN_DESK)
	{
		m_serverID=0;
		m_userid=0;
		m_gateId=0;
		m_deskID=0;
		m_clubMasterId=0;
		m_addDeskFlag = 0;
		m_type = 0;
		m_clubId = 0;
		m_playTypeId = 0;

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		///////////////////////////////////////////////////

		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		buff.Read(m_clubMasterId);
		buff.Read(m_addDeskFlag);
		buff.Read(m_type);
		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		m_usert.Read(buff);


		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);

		/////////////////////////////////////////////////////
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		buff.Write(m_clubMasterId);
		buff.Write(m_addDeskFlag);
		buff.Write(m_type);
		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		m_usert.Write(buff);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUpdateCoinJoinDesk();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送更新 玩家GPS
struct LMsgL2LMGUpdateUserGPS :public LMsg
{
	Lstring         m_openID;
	Lint            m_serverID;
	/////////////////////////////////////////////

	Lint			m_userid;
	Lstring			m_strUUID;
	Lstring			m_Gps;


	LMsgL2LMGUpdateUserGPS() :LMsg(MSG_L_2_LMG_UPDATE_USER_GPS)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		///////////////////////////////////////////////////
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(m_Gps);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);

		/////////////////////////////////////////////////////
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(m_Gps);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUpdateUserGPS();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送更新 玩家GPS
struct LMsgLMG2CEUpdateUserGPS :public LMsg
{
	Lstring         m_openID;
	Lint            m_serverID;
	/////////////////////////////////////////////

	Lint			m_userid;
	Lstring			m_strUUID;
	Lstring			m_Gps;


	LMsgLMG2CEUpdateUserGPS() :LMsg(MSG_LMG_2_CE_UPDATE_USER_GPS)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		///////////////////////////////////////////////////
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(m_Gps);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);

		/////////////////////////////////////////////////////
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(m_Gps);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CEUpdateUserGPS();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送更新 玩家金币
struct LMsgCe2LUpdateCoinJoinDesk :public LMsg
{

	Lint            m_userID;
	Lint            m_coinNum;  //用户金币数量
	Lint            m_creatType;
	LBuffPtr		m_dataBuff;
	Lstring         m_userIp;
	Lstring         m_userGps;
	Lint            m_Block;
	Lint            m_Free;
	Lint            m_startTime;
	Lint            m_endTime;
	/////////////////////////////////////////////

	Lint			m_userid;
	Lint			m_gateId;
	Lstring			m_strUUID;
	Lint			m_deskID;
	Lstring			m_ip;
	LUser			m_usert;

	Lint           m_clubMasterCoinNum;
	Lint           m_clubMasterBlock;
	Lint           m_errorCode;             //0--成功，1--center没有找到这个用户        //添加这个字段避免找不到直接return不回信息
	Lint          m_addDeskFlag;      //加入房间标志

	Lint       m_type;              // 消息来源类型  3:玩家俱乐部匹配进入房间

	Lint       m_clubId;
	Lint       m_playTypeId;

	LMsgCe2LUpdateCoinJoinDesk() :LMsg(MSG_CE_2_LMG_UPDATE_COIN_JOIN_DESK)
	{
		m_Block = 0;
		m_Free = 0;
		m_startTime = 0;
		m_endTime = 0;

		m_clubMasterCoinNum=0;
		m_clubMasterBlock=0;
		m_errorCode=0;
		m_addDeskFlag = 0;

		m_type = 0;
		m_clubId = 0;
		m_playTypeId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userID);
		buff.Read(m_coinNum);
		buff.Read(m_creatType);
		buff.Read(m_userIp);
		buff.Read(m_userGps);
		buff.Read(m_Block);
		buff.Read(m_Free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);
		///////////////////////////////////////////////////

		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);

		buff.Read(m_clubMasterCoinNum);
		buff.Read(m_clubMasterBlock);
		buff.Read(m_errorCode);
		buff.Read(m_addDeskFlag);

		buff.Read(m_type);
		buff.Read(m_clubId);
		buff.Read(m_playTypeId);

		m_usert.Read(buff);


		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userID);
		buff.Write(m_coinNum);
		buff.Write(m_creatType);
		buff.Write(m_userIp);
		buff.Write(m_userGps);
		buff.Write(m_Block);
		buff.Write(m_Free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);
		/////////////////////////////////////////////////////
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);

		buff.Write(m_clubMasterCoinNum);
		buff.Write(m_clubMasterBlock);
		buff.Write(m_errorCode);
		buff.Write(m_addDeskFlag);

		buff.Write(m_type);
		buff.Write(m_clubId);
		buff.Write(m_playTypeId);

		m_usert.Write(buff);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUpdateCoinJoinDesk();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送保存 桌子信息
struct LMsgL2CeSaveLogItem:public LMsg
{
	Lint		m_type;//类型,0-插入,1-更新
	Lstring		m_sql;

	LMsgL2CeSaveLogItem() :LMsg(MSG_L_2_CE_ROOM_SAVE)
	{
		m_type = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeSaveLogItem();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 通知玩家登陆服务器
struct LMsgL2CeUserServerLogin :public LMsg
{
	Lint		m_serverID;//类型,0-插入,1-更新
	Lstring		m_openID;
	Lstring		m_unionId;
	LMsgL2CeUserServerLogin() :LMsg(MSG_L_2_CE_USER_LOGIN)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_openID);
		buff.Read(m_unionId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_openID);
		buff.Write(m_unionId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUserServerLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 通知玩家登出服务器
struct LMsgL2CeUserServerLogout:public LMsg
{
	Lint		m_serverID;//类型,0-插入,1-更新
	Lstring		m_openID;
	Lstring		m_unionId;
	LMsgL2CeUserServerLogout() :LMsg(MSG_L_2_CE_USER_LOGOUT)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_openID);  
		buff.Read(m_unionId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_openID);
		buff.Write(m_unionId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUserServerLogout();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送玩家充值
struct LMsgCe2LGMCharge:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_cardType;
	Lint		m_cardCount;
	Lint		m_oper;
	Lstring		m_admin;

	LMsgCe2LGMCharge() :LMsg(MSG_CE_2_L_GM_CHARGE)
	{
		m_userid = 0;
		m_cardType = 0;
		m_cardCount = 0;
		m_oper = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_cardType);
		buff.Read(m_cardCount);
		buff.Read(m_oper);
		buff.Read(m_admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_cardType);
		buff.Write(m_cardCount);
		buff.Write(m_oper);
		buff.Write(m_admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMCharge();
	}
};


//////////////////////////////////////////////////////////////////////////
//center 发送gm设置跑马灯
struct LMsgCe2LGMHorse :public LMsg
{
	Lstring		m_str;
	
	LMsgCe2LGMHorse() :LMsg(MSG_CE_2_L_GM_HORSE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMHorse();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送gm够买提示
struct LMsgCe2LGMBuyInfo:public LMsg
{
	Lstring		m_str;

	LMsgCe2LGMBuyInfo() :LMsg(MSG_CE_2_L_GM_BUYINFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMBuyInfo();
	}
};

struct LMsgCe2LGMCoins:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_coins;		// 充值的金币
	Lint		m_totalcoins;	// 充值后的总金币

	LMsgCe2LGMCoins() :LMsg(MSG_CE_2_L_GM_COINS)
	{
		m_userid = 0;
		m_coins = 0;
		m_totalcoins = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_coins);
		buff.Read(m_totalcoins);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_coins);
		buff.Write(m_totalcoins);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMCoins();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送gm够买提示
struct LMsgCe2LGMHide:public LMsg
{
	Lint		m_hide;

	LMsgCe2LGMHide() :LMsg(MSG_CE_2_L_GM_HIDE)
	{
		m_hide = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_hide);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_hide);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMHide();
	}
};
//////////////////////////////////////////////////////////////////////////
//logic 跟 gate 之间的消息交互
//////////////////////////////////////////////////////////////////////////

struct LMsgG2LLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgG2LLogin() :LMsg(MSG_G_2_L_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LLogin();
	}
};

struct LMsgG2LMGLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;
	Lint        m_gaofang_flag; // 1:非高防 2:高防

	LMsgG2LMGLogin() :LMsg(MSG_G_2_LMG_LOGIN)
	{
		m_id = 0;
		m_port = 0;
		m_gaofang_flag = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_gaofang_flag);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_gaofang_flag);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LMGLogin();
	}
};

struct LMsgL2LMGLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;
	Lint            m_type;  // 0--logic  1--qipai 

	LMsgL2LMGLogin() :LMsg(MSG_L_2_LMG_LOGIN)
	{
		m_id = 0;
		m_port = 0;
		m_type=LOGIC_SERVER_MJ;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_type);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_type);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGLogin();
	}
};


//////////////////////////////////////////////////////////////////////////
//gate 发送玩家消息 到 logic
struct LMsgG2LUserMsg :public LMsg
{
	/*
	以下字段由gate进行填充
	*/
	Lstring			m_strUUID;	//玩家uuid
	Lstring			m_ip;
	LBuffPtr		m_dataBuff;

	/*
	以下字段由read中解析
	*/
	Lint			m_userMsgId;
	LMsg*			m_userMsg;

	LMsgG2LUserMsg() :LMsg(MSG_G_2_L_USER_MSG)
	{
		m_userMsgId = 0;
		m_userMsg = NULL;
	}

	virtual~LMsgG2LUserMsg()
	{
		if (m_userMsg)
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_ip);

		m_userMsgId = MSG_ERROR_MSG;
		try
		{
			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
			msgpack::object obj = unpack.get();

			//ReadMapData(obj, "m_msgId", m_userMsgId);
			ReadMapData(obj, "kMId", m_userMsgId);			
			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("MSG_G_2_L_USER_MSG::RecvMsgPack error");
		}
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_ip);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发现玩家消息到 gate
struct LMsgL2GUserMsg : public LMsg
{
	Lstring			m_strUUID;
	LBuffPtr		m_dataBuff;
	//Lint            m_symbol;
	
	LMsgL2GUserMsg() :LMsg(MSG_L_2_G_USER_MSG)
	{
		//m_symbol = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		//buff.Read(m_symbol);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		//buff.Write(m_symbol);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//gate 发送玩家掉线消息 到 logic
struct LMsgG2LUserOutMsg :public LMsg
{
	Lstring	 m_strUUID;		//玩家UUID
	
	LMsgG2LUserOutMsg():LMsg(MSG_G_2_L_USER_OUT_MSG)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserOutMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送玩家消息到 gate
struct LMsgL2GUserOutMsg : public LMsg
{
	Lstring		m_strUUID;	//玩家uuid

	LMsgL2GUserOutMsg():LMsg(MSG_L_2_G_USER_OUT_MSG)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserOutMsg();
	}
};
/////////////////////////////////////////////
enum AgencyActivityUpdatePlayCountType
{
	AgencyActivityUpdatePlayCountType_EveryScore=1,
	AgencyActivityUpdatePlayCountType_BigWin=2,
	AgencyActivityUpdatePlayCountType_Spec=3,
	AgencyActivityUpdatePlayCountType_ChangePoint = 4,


};

//logic 发送更新 玩家GPS
struct MHLMsgL2LMGNewAgencyActivityUpdatePlayCount : public LMsg
{
	Lstring       m_openID;

	Lint			    m_userid;
	Lstring		m_strUUID;
	Lint            m_deskId;
	Lint            m_playerCount;
	Lint            m_playerId[10];
	Lint            m_score[10];

	Lint            m_flag;               //活动标志        1--满局大结算 每个人分数   2--大赢家    3-特殊牌型  4-用于更新能量值

	Lint            m_playType;       //玩法
	Lint            m_maxCircle;
	Lint            m_startTime;
	Lstring			m_playInfoId;
	Lint           m_clubId;
	Lint          m_userId;            //大赢家和特殊牌型的用户id
	Lstring          m_userNike;   //大赢家和特殊牌型的用户
	Lint          m_userCount;    // 参加游戏中人数2，3，4
	Lint           m_paiXingCount;
	std::vector<Lint>          m_paiXing;


	MHLMsgL2LMGNewAgencyActivityUpdatePlayCount() : LMsg(MH_MSG_L_2_LMG_NEW_AGENCY_ACTIVITY_UPDATE_PLAY_COUNT)
	{
		m_deskId = 0;
		m_userid = 0;
		m_flag = 0;
		m_playerCount = 0;
		m_playType = 0;
		m_maxCircle = 0;
		m_startTime = 0;
		m_clubId = 0;
		m_paiXingCount = 0;
		m_userId = 0;
		m_userCount = 0;


		for (Lint i = 0; i < 10; i++)
		{
			m_playerId[i] = 0;
			m_score[i] = 0;
		}
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_openID);
		///////////////////////////////////////////////////
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(m_deskId);
		buff.Read(m_playerId);
		buff.Read(m_flag);
		buff.Read(m_playerCount);
		buff.Read(m_playType);
		buff.Read(m_score);
		buff.Read(m_maxCircle);
		buff.Read(m_startTime);
		buff.Read(m_playInfoId);
		buff.Read(m_clubId);
		buff.Read(m_userId);
		buff.Read(m_userNike);
		buff.Read(m_userCount);
		buff.Read(m_paiXingCount);
		for (Lint i = 0; i < m_paiXingCount; i++)
		{
			Lint paiXing;
			buff.Read(paiXing);
			m_paiXing.push_back(paiXing);
		}


		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_openID);

		/////////////////////////////////////////////////////
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(m_deskId);
		buff.Write(m_playerId);
		buff.Write(m_flag);
		buff.Write(m_playerCount);
		buff.Write(m_playType);
		buff.Write(m_score);
		buff.Write(m_maxCircle);
		buff.Write(m_startTime);
		buff.Write(m_playInfoId);

		buff.Write(m_clubId);
		buff.Write(m_userId);
		buff.Write(m_userNike);

		buff.Write(m_userCount);
		buff.Write(m_paiXingCount);
		for (Lint i = 0; i < m_paiXingCount; i++)
		{
			buff.Write(m_paiXing[i]);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new MHLMsgL2LMGNewAgencyActivityUpdatePlayCount();
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//logicmanager 跟 logicdb 之间的消息交互
struct LMsgLMG2LdbUserLogin:public LMsg
{
	Lint		m_userId;
	Lstring		m_strUUID;
	LMsgLMG2LdbUserLogin() :LMsg(MSG_LMG_2_LDB_USER_LOGIN)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LdbUserLogin();
	}
};

struct LMsgLDB2LMGUserMsg : public LMsg
{
	Lstring			m_strUUID;
	LBuffPtr		m_dataBuff;

	LMsgLDB2LMGUserMsg() :LMsg(MSG_LDB_2_LMG_USER_MSG)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGUserMsg();
	}
};

//logic 登录 logicdb
struct LMsgLMG2LdbLogin : public LMsg
{
	Lstring			m_key;
	Lint			m_serverID;
	LMsgLMG2LdbLogin():LMsg(MSG_LMG_2_LDB_LOGIN)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LdbLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 登录 logicdb
struct LMsgL2LdbLogin : public LMsg
{
	Lstring			m_key;
	Lint			m_serverID;
	LMsgL2LdbLogin():LMsg(MSG_L_2_LDB_LOGIN)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LdbLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送保存 桌子信息
struct LMsgL2LDBSaveLogItem:public LMsg
{
	Lint		m_type;//类型,0-插入,1-更新
	Lstring		m_sql;
	Lint		m_serverID;
	Lint        m_userId;  //add

	LMsgL2LDBSaveLogItem() :LMsg(MSG_L_2_LDB_ROOM_SAVE)
	{
		m_type = 0;
		m_serverID = 0;
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		buff.Read(m_serverID);
		buff.Read(m_userId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		buff.Write(m_serverID);
		buff.Write(m_userId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveLogItem();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送保存录像
struct LMsgL2LDBSaveVideo :public LMsg
{
	Lint		m_type;//类型,0-插入,1-更新
	Lstring		m_sql;
	Lint		m_serverID;
	Lint        m_userId;  //add

	LMsgL2LDBSaveVideo() :LMsg(MSG_L_2_LDB_VIDEO_SAVE)
	{
		m_type = 0;
		m_serverID = 0;
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		buff.Read(m_serverID);
		buff.Read(m_userId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		buff.Write(m_serverID);
		buff.Write(m_userId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求战绩
struct LMsgL2LBDReqVipLog:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userId;
	Lint	m_reqUserId;
	Lint    m_time;
	Lint   m_flag;    //0-麻将  1-扑克 （102赢三张）
	Lint  m_clubId;
	Lint  m_limit;
	Lint  m_allClub;   // 0--不是全俱乐部   1--全俱乐部数据

	LMsgL2LBDReqVipLog() :LMsg(MSG_L_2_LDB_VIP_LOG)
	{
		m_userId = 0;
		m_reqUserId = 0;
		m_time = 0;
		m_flag=0;
		m_clubId = 0;
		m_limit = 0;
		m_allClub = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_reqUserId);
		buff.Read(m_time);
		buff.Read(m_flag);
		buff.Read(m_clubId);
		buff.Read(m_limit);
		buff.Read(m_allClub);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_reqUserId);
		buff.Write(m_time);
		buff.Write(m_flag);
		buff.Write(m_clubId);
		buff.Write(m_limit);
		buff.Write(m_allClub);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LBDReqVipLog();
	}
};


//////////////////////////////////////////////////////////////////////////
//logicdb 返回 玩家战绩
struct LogInfo
{
	Lstring			m_id;		//id
	Lint			m_time;		//时间
	Lint			m_flag;		//房间规则
	Lint			m_deskId;	//桌子id
	Lstring			m_secret;	//密码
	Lint			m_maxCircle;//总圈数
	Lint			m_curCircle;//当前圈数
	Lint			m_posUserId[4];//0-3各个位置上的玩家id
	Lint			m_curZhuangPos;//当前庄家
	Lint			m_score[4];// 各个位置上面的积分
	Lint			m_reset;//是否解算
	Lstring			m_data;
	Lstring			m_playtype;	//玩法
	Lint               m_clubId;              //俱乐部Id
	Lint               m_showDeskId; //桌子号
	Lint           m_cellScore;

	LogInfo()
	{
		m_time = 0;
		m_flag = 0;	
		m_deskId = 0;
		m_maxCircle = 0;
		m_curCircle = 0;
		memset(m_posUserId, 0, sizeof(m_posUserId));
		m_curZhuangPos = 0;
		memset(m_score, 0, sizeof(m_score)); 
		m_reset = false;
		m_clubId = 0;
		m_showDeskId = 0;
		m_cellScore = 1;
	}
};

struct LMsgLBD2LReqVipLog:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	LogInfo		m_info[21];

	LMsgLBD2LReqVipLog() :LMsg(MSG_LDB_2_L_VIP_LOG)
	{
		m_userid = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Read(info.m_id);
			buff.Read(info.m_time);
			buff.Read(info.m_flag);
			buff.Read(info.m_deskId);
			buff.Read(info.m_secret);
			buff.Read(info.m_maxCircle);
			buff.Read(info.m_curCircle);
			buff.Read(info.m_posUserId[0]);
			buff.Read(info.m_posUserId[1]);
			buff.Read(info.m_posUserId[2]);
			buff.Read(info.m_posUserId[3]);

			buff.Read(info.m_curZhuangPos);
			buff.Read(info.m_score[0]);
			buff.Read(info.m_score[1]);
			buff.Read(info.m_score[2]);
			buff.Read(info.m_score[3]);
			buff.Read(info.m_reset);
			buff.Read(info.m_data);
			buff.Read(info.m_playtype);
			buff.Read(info.m_clubId);
			buff.Read(info.m_showDeskId);
			buff.Read(info.m_cellScore);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Write(info.m_id);
			buff.Write(info.m_time);
			buff.Write(info.m_flag);
			buff.Write(info.m_deskId);
			buff.Write(info.m_secret);
			buff.Write(info.m_maxCircle);
			buff.Write(info.m_curCircle);
			buff.Write(info.m_posUserId[0]);
			buff.Write(info.m_posUserId[1]);
			buff.Write(info.m_posUserId[2]);
			buff.Write(info.m_posUserId[3]);

			buff.Write(info.m_curZhuangPos);
			buff.Write(info.m_score[0]);
			buff.Write(info.m_score[1]);
			buff.Write(info.m_score[2]);
			buff.Write(info.m_score[3]);
			buff.Write(info.m_reset);
			buff.Write(info.m_data);
			buff.Write(info.m_playtype);
			buff.Write(info.m_clubId);
			buff.Write(info.m_showDeskId);
			buff.Write(info.m_cellScore);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLBD2LReqVipLog();
	}
};

//////////////////////////////////////////////////////////////////////////
//logicdb 返回 玩家战绩-poker
struct LogPokerInfo
{
	Lstring			m_id;		//id
	Lint			m_time;		//时间
	Lint			m_clubId;   //俱乐部桌子Id
	Lint			m_flag;		//房间规则
	Lint			m_deskId;	//桌子id
	//Lstring			m_secret;	//密码
	Lint			m_maxCircle;//总圈数
	Lint			m_curCircle;//当前圈数
	Lint			m_createUserId;  //房主ID
	Lint			m_posUserId[20];//0-3各个位置上的玩家id
	//Lint			m_curZhuangPos;//当前庄家
	Lint			m_score[20];// 各个位置上面的积分
	//Lint			m_reset;//是否解算
	//Lstring			m_data;
	Lstring			m_playtype;	//玩法小选项
	Lstring			m_optData;

	LogPokerInfo()
	{
		m_time=0;
		m_clubId = 0;
		m_flag=0;
		m_deskId=0;
		m_maxCircle=0;
		m_curCircle=0;
		m_createUserId = 0;
		memset(m_posUserId, 0, sizeof(m_posUserId));
		//m_curZhuangPos=0;
		memset(m_score, 0, sizeof(m_score));
		//m_reset=false;
	}
};

struct LMsgLBD2LReqPokerLog :public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	LogPokerInfo		m_info[20];
	Lint		m_playUserCount;

	LMsgLBD2LReqPokerLog() :LMsg(MSG_LDB_2_LM_POKER_LOG)
	{
		m_userid=0;
		m_count=0;
		m_playUserCount=0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		buff.Read(m_playUserCount);
		for (Lint i=0; i < m_count; ++i)
		{
			LogPokerInfo& info=m_info[i];
			buff.Read(info.m_id);
			buff.Read(info.m_time);
			buff.Read(info.m_clubId);
			buff.Read(info.m_flag);
			buff.Read(info.m_deskId);
			
			buff.Read(info.m_maxCircle);
			buff.Read(info.m_curCircle);
			buff.Read(info.m_createUserId);
			for (int j = 0; j < 20; j++)
			{
				buff.Read(info.m_posUserId[j]);
			}
			for (int j = 0; j < 20; j++)
			{
				buff.Read(info.m_score[j]);
			}
			buff.Read(info.m_playtype);
			buff.Read(info.m_optData);
			/*buff.Read(info.m_posUserId[0]);
			buff.Read(info.m_posUserId[1]);
			buff.Read(info.m_posUserId[2]);
			buff.Read(info.m_posUserId[3]);
			buff.Read(info.m_posUserId[4]);
			buff.Read(info.m_posUserId[5]);*/

			
			/*buff.Read(info.m_score[0]);
			buff.Read(info.m_score[1]);
			buff.Read(info.m_score[2]);
			buff.Read(info.m_score[3]);
			buff.Read(info.m_score[4]);
			buff.Read(info.m_score[5]);*/
			
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		buff.Write(m_playUserCount);
		for (Lint i=0; i < m_count; ++i)
		{
			LogPokerInfo& info=m_info[i];
			buff.Write(info.m_id);
			buff.Write(info.m_time);
			buff.Write(info.m_clubId);
			buff.Write(info.m_flag);
			buff.Write(info.m_deskId);
			//buff.Write(info.m_secret);
			buff.Write(info.m_maxCircle);
			buff.Write(info.m_curCircle);
			buff.Write(info.m_createUserId);
			for (int j = 0; j < 20; j++)
			{
				buff.Write(info.m_posUserId[j]);
			}
			for (int j = 0; j < 20; j++)
			{
				buff.Write(info.m_score[j]);
			}
			buff.Write(info.m_playtype);
			buff.Write(info.m_optData);
			/*buff.Write(info.m_posUserId[0]);
			buff.Write(info.m_posUserId[1]);
			buff.Write(info.m_posUserId[2]);
			buff.Write(info.m_posUserId[3]);
			buff.Write(info.m_posUserId[4]);
			buff.Write(info.m_posUserId[5]);*/

			//buff.Write(info.m_curZhuangPos);
			/*buff.Write(info.m_score[0]);
			buff.Write(info.m_score[1]);
			buff.Write(info.m_score[2]);
			buff.Write(info.m_score[3]);
			buff.Write(info.m_score[4]);
			buff.Write(info.m_score[5]);*/
			//buff.Write(info.m_reset);
			//buff.Write(info.m_data);
			//buff.Write(info.m_playtype);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLBD2LReqPokerLog();
	}
};

/////////////////////////////////////////////////////////////
//LogicManager 2 LogicDb
struct LMsgL2LDBReqRoomLog :public LMsg
{
	Lstring		m_strUUID;
	Lstring		m_id;
	Lint		m_userId;
	Lint        m_time;
	Lint        m_deskId;
	Lint        m_pos;
	Lint        m_flag;    //游戏类型标志  0-默认，麻将  1-扑克

	LMsgL2LDBReqRoomLog() :LMsg(MSG_L_2_LDB_ROOM_LOG)
	{
		m_userId = 0;
		m_time = 0;
		m_deskId = 0;
		m_pos = 0;
		m_flag = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_id);
		buff.Read(m_userId);
		buff.Read(m_time);
		buff.Read(m_deskId);
		buff.Read(m_pos);
		buff.Read(m_flag);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_id);
		buff.Write(m_userId);
		buff.Write(m_time);
		buff.Write(m_deskId);
		buff.Write(m_pos);
		buff.Write(m_flag);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqRoomLog();
	}
};

//LogicDB 2 LogicManager
struct LMsgLBD2LReqRoomLog:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_flag;
	Lstring     m_data;			//数据库playinfo.ScoreData内容
	Lint		m_type;         //游戏类型 0-麻将  1-扑克

	LMsgLBD2LReqRoomLog() :LMsg(MSG_LDB_2_L_ROOM_LOG)
	{
		m_userid = 0;
		m_flag = 0;
		m_data = "";
		m_type = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_flag);
		buff.Read(m_data);
		buff.Read(m_type);
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_flag);
		buff.Write(m_data);
		buff.Write(m_type);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLBD2LReqRoomLog();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求录像
struct LMsgL2LDBReqVideo:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userId;
	Lstring	m_videoId;
	Lint        m_flag;          //游戏类型标志  0-默认，麻将  1-扑克

	LMsgL2LDBReqVideo() :LMsg(MSG_L_2_LDB_VIDEO)
	{
		m_userId = 0;
		m_flag = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_videoId);
		buff.Read(m_flag);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_videoId);
		buff.Write(m_flag);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 返回 玩家战绩
struct LMsgLDB2LReqVideo:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	VideoLog	m_video;

	LMsgLDB2LReqVideo() :LMsg(MSG_LDB_2_L_VIDEO)
	{
		m_userid = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		m_video.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		m_video.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LReqVideo();
	}
};


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
struct MHLMsgLDB2LShareVideoResult :public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	VideoLog	m_video;

	MHLMsgLDB2LShareVideoResult() :LMsg(MH_MSG_LDB_2_L_SHARE_VIDEO_REULST)
	{
		m_userid = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		m_video.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		m_video.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new MHLMsgLDB2LShareVideoResult();
	}
};
//logic 查询分享录像
struct MHLMsgL2LDBQueryShareVideoByShareId:public LMsg
{
	Lstring m_strUUID;
	Lint m_userId;
	Lstring m_videoId;
    MHLMsgL2LDBQueryShareVideoByShareId():LMsg(MH_MSG_L_2_LDB_QUERY_SHARE_VIDEO_BY_SHARE_ID) 
	{
	}
    virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new MHLMsgL2LDBQueryShareVideoByShareId();
	}

};
//loigc 通过精确的videoid来查找sharevideo中的对应录像
struct LMsgL2LDBGetSsharedVideo:public LMsg
{
	Lstring	m_strUUID;
	Lint	m_userId;
	Lstring	m_videoId;

	LMsgL2LDBGetSsharedVideo() :LMsg(MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID),m_userId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBGetSsharedVideo();
	}
};


//////////////////////////////////////////////////////////////////////////
//loigc 通过精确的userid来查找自己的成绩
struct LMsgL2LDBGetUserPlayData :public LMsg
{
	Lstring	m_strUUID;
	Lint	m_userId;
	Lint    m_allAround;
	Lint    m_winAround;

	LMsgL2LDBGetUserPlayData() :LMsg(MSG_L_2_LDB_GETUSERPLAYDATA), m_userId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_allAround);
		buff.Read(m_winAround);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_allAround);
		buff.Write(m_winAround);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBGetUserPlayData();
	}
};


//////////////////////////////////////////////////////////////////////////
//loigcDB 返回 logic 通过精确的userid来查找自己的成绩
struct LMsgLDB2LGetUserPlayData :public LMsg
{
	Lstring m_errorStr;
	Lstring	m_strUUID;
	Lint	m_userId;
	Lint    m_allAround;
	Lint    m_winAround;

	LMsgLDB2LGetUserPlayData() :LMsg(MSG_LDB_2_L_GETUSERPLAYDATA), m_userId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_errorStr);
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_allAround);
		buff.Read(m_winAround);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_errorStr);
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_allAround);
		buff.Write(m_winAround);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LGetUserPlayData();
	}
};


//logic 玩家请求分享某录像，先去sharevideo中查看是否已被分享
struct LMsgL2LDBGGetShareVideoId:public LMsg
{
	Lstring	m_strUUID;
	Lint	m_userId;
	Lstring	m_videoId;

	LMsgL2LDBGGetShareVideoId() :LMsg(MSG_L_2_LDB_GETSHAREVIDEOID),m_userId(0), m_videoId("")
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBGGetShareVideoId();
	}
};

//logic 玩家通过shareid来获取该分享码下的录像 在sharevideo中查询
struct LMsgL2LDBGetShareVideo:public LMsg
{
	Lstring	m_strUUID;
	Lint	m_userId;
	Lint	m_shareId;

	LMsgL2LDBGetShareVideo() :LMsg(MSG_L_2_LDB_GETSHAREVIDEO),m_userId(0),m_shareId(0)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_shareId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_shareId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBGetShareVideo();
	}
};


//logic 请求保存某已转移到sharevideo中的某录像的shareid
struct LMsgL2LDBSaveShareVideoId:public LMsg
{
	Lstring	m_strUUID;
	Lint	m_userId;
	Lint	m_shareId;
	Lstring	m_videoId;

	LMsgL2LDBSaveShareVideoId() :LMsg(MSG_L_2_LDB_SAVESHAREVIDEOID),m_shareId(0),m_videoId("")
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_shareId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_shareId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveShareVideoId();
	}
};




//logicdb返回某录像是否有shareid，没有则将其从video中移至sharevideo中
struct LMsgLDB2LRetShareVideoId:public LMsg
{
	Lstring	m_strUUID;
	Lint	m_userId;
	Lint	m_shareId;
	Lstring	m_videoId;

	LMsgLDB2LRetShareVideoId() :LMsg(MSG_LDB_2_L_RETSHAREVIDEOID),m_userId(0),m_shareId(0),m_videoId("")
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_shareId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_shareId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LRetShareVideoId();
	}
};

struct ShareVideoInfo{
	Lstring	m_videoId;
	Lint	m_time;
	Lint	m_flag;
	Lint	m_userId[4];
	Lint	m_score[4];
	Lint    m_user_count;
	Lstring m_playtype;

	ShareVideoInfo():m_time(0),m_flag(0), m_user_count(4){
		memset(m_userId,0,sizeof(m_userId));
		memset(m_score,0,sizeof(m_score));
	}
};
//logicdb 返回 通过shareid查询到的可能的录像
struct LMsgLDB2LRetShareVideo:public LMsg
{
	Lstring	    m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	std::vector<ShareVideoInfo> m_info; 
	LMsgLDB2LRetShareVideo() :LMsg(MSG_LDB_2_L_RETSHAREVIDEO),m_count(0),m_userid(0)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		for(int x=0;x<m_count;x++)
		{	
			ShareVideoInfo info;
			buff.Read(info.m_videoId);
			buff.Read(info.m_time);
			buff.Read(info.m_flag);
			buff.Read(info.m_user_count);
			for(int i=0;i<4;i++)
			{
				buff.Read(info.m_userId[i]);
				buff.Read(info.m_score[i]);
			}
			m_info.push_back(info);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		for(int x=0;x<m_count;x++)
		{
			ShareVideoInfo& info = m_info[x];
			buff.Write(info.m_videoId);
			buff.Write(info.m_time);
			buff.Write(info.m_flag);
			buff.Write(info.m_user_count);
			for(int i=0;i<4;i++)
			{
				buff.Write(info.m_userId[i]);
				buff.Write(info.m_score[i]);
			}
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LRetShareVideo();
	}
};

struct LMsgLDB2LLogInfo:public LMsg
{
	Lint	m_lastId;
	Lint	m_videoId;
	Lint	m_count;
	std::vector<LogInfo> m_info;

	LMsgLDB2LLogInfo() :LMsg(MSG_LDB_2_L_LOG_INFO)
	{
		m_lastId = 0;
		m_videoId = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_lastId);
		buff.Read(m_videoId);
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo info;
			buff.Read(info.m_id);
			buff.Read(info.m_time);
			buff.Read(info.m_flag);
			buff.Read(info.m_deskId);
			buff.Read(info.m_secret);
			buff.Read(info.m_maxCircle);
			buff.Read(info.m_curCircle);
			buff.Read(info.m_posUserId[0]);
			buff.Read(info.m_posUserId[1]);
			buff.Read(info.m_posUserId[2]);
			buff.Read(info.m_posUserId[3]);

			buff.Read(info.m_curZhuangPos);
			buff.Read(info.m_score[0]);
			buff.Read(info.m_score[1]);
			buff.Read(info.m_score[2]);
			buff.Read(info.m_score[3]);
			buff.Read(info.m_reset);
			buff.Read(info.m_data);
			buff.Read(info.m_playtype);

			m_info.push_back(info);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_lastId);
		buff.Write(m_videoId);
		m_count = m_info.size();
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Write(info.m_id);
			buff.Write(info.m_time);
			buff.Write(info.m_flag);
			buff.Write(info.m_deskId);
			buff.Write(info.m_secret);
			buff.Write(info.m_maxCircle);
			buff.Write(info.m_curCircle);
			buff.Write(info.m_posUserId[0]);
			buff.Write(info.m_posUserId[1]);
			buff.Write(info.m_posUserId[2]);
			buff.Write(info.m_posUserId[3]);

			buff.Write(info.m_curZhuangPos);
			buff.Write(info.m_score[0]);
			buff.Write(info.m_score[1]);
			buff.Write(info.m_score[2]);
			buff.Write(info.m_score[3]);
			buff.Write(info.m_reset);
			buff.Write(info.m_data);
			buff.Write(info.m_playtype);
		}


		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LLogInfo();
	}
};

struct LMsgLMG2GHorseInfo :public LMsg
{
	Lstring	m_str;
	Lint    m_type;  //1= 免费收费消息推送 2= 推送金币房卡信息

	LMsgLMG2GHorseInfo():LMsg(MSG_LMG_2_G_HORSE_INFO)
	{
		m_type = 0;   //默认跑马灯 消息模式
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		buff.Read(m_type);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		buff.Write(m_type);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GHorseInfo();
	}
};


struct LMsgLMG2LLogin:public LMsg
{
	Lint		m_userid;
	Lint		m_gateId;
	Lstring		m_strUUID;
	Lstring		m_ip;
	Lstring		m_buyInfo;//够买信息
	Lint		m_hide;//购买隐藏0-隐藏 1- 不隐藏
	Lint		m_card_num;	//房卡数 （Card2）
	Lint		m_isMedal;	//玩家是否含有勇士勋章
	LMsgLMG2LLogin() :LMsg(MSG_LMG_2_L_USER_LOGIN)
	{
		m_userid = 0;
		m_gateId = 0;
		m_hide = 0;
		m_card_num = 0;
		m_isMedal = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_ip);
		buff.Read(m_buyInfo);
		buff.Read(m_hide);
		buff.Read(m_card_num);
		buff.Read(m_isMedal);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_ip);
		buff.Write(m_buyInfo);
		buff.Write(m_hide);
		buff.Write(m_card_num);
		buff.Write(m_isMedal);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LLogin();
	}
};

struct LMsgL2LMGModifyUserState:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userid;
	Lint	m_userstate;
	Lint	m_logicID;

	LMsgL2LMGModifyUserState() :LMsg(MSG_L_2_LMG_MODIFY_USER_STATE)
	{
		m_userid = 0;
		m_userstate = 0;
		m_logicID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_userstate);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_userstate);
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyUserState();
	}
};


struct LMsgL2LMGModifyUserCreatedDesk :public LMsg
{
	Lstring m_strUUID;
	Lint	m_userid;
	Lint	m_deskId;
	Lint	m_logicID;
	Lint    m_cost;

	LMsgL2LMGModifyUserCreatedDesk() :LMsg(MSG_L_2_LMG_MODIFY_USER_CREATED_DESK)
	{
		m_userid = 0;
		m_deskId = 0;
		m_logicID = 0;
		m_cost = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_deskId);
		buff.Read(m_cost);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_deskId);
		buff.Write(m_cost);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyUserCreatedDesk();
	}
};

struct LMsgL2LMGDeleteUserCreatedDesk :public LMsg
{
	Lstring m_strUUID;
	Lint	m_userid;
	Lint	m_deskId;
	Lint    m_cost;
	Lint    m_delType;     //0: 只是删除桌子 1: 只是减少金币 2：减少金币和桌子

	LMsgL2LMGDeleteUserCreatedDesk() :LMsg(MSG_L_2_LMG_DELETE_USER_CREATED_DESK)
	{
		m_userid = 0;
		m_deskId = 0;
		m_cost = 0;
		m_delType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_deskId);
		buff.Read(m_cost);
		buff.Read(m_delType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_deskId);
		buff.Write(m_cost);
		buff.Write(m_delType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGDeleteUserCreatedDesk();
	}
};

struct LMsgL2GModifyUserState:public LMsg
{
	Lstring m_strUUID;	 //玩家uuid
	Lint	m_userstate;
	Lint	m_logicID;

	LMsgL2GModifyUserState() :LMsg(MSG_L_2_G_MODIFY_USER_STATE)
	{
		m_userstate = 0;
		m_logicID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userstate);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userstate);
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GModifyUserState();
	}
};


struct LMsgLMG2LCreateDesk : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lstring				m_strUUID;
	Lint				m_deskID;
	Lstring				m_ip;
	LUser				m_usert;
	Lint                m_cellscore;       //底分

	Lint                m_deskType;         //创建的桌子类型
	Lint                m_feeType;          // 费用类型 ，0:房主付费 1:玩家分摊
	Lint                m_cheatAgainst;     // 是否防作弊，0:不防作弊 1：防作弊

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100
	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
	Lint                m_cost;             //花费
	Lint                m_even;             //均摊费用
	Lint                m_free;
	Lint                m_startTime;
	Lint                m_endTime;

	//CardValue			m_cardValue[CARD_COUNT];
	CardValue			m_Player0CardValue[13];
	CardValue			m_Player1CardValue[13];
	CardValue			m_Player2CardValue[13];
	CardValue			m_Player3CardValue[13];
	CardValue			m_SendCardValue[84];
	CardValue			m_HaoZiCardValue[2];
	Lint				m_playTypeCount;
	std::vector<Lint>	m_playType;			//玩法
	Lint  m_Greater2CanStart;
	Lint  m_GpsLimit;

	Lint  m_allowLookOn;			//允许观战，创建房间时候选项，0：不允许观战  1：允许观战   默认不允许观战
	Lint  m_clubOwerLookOn;			//俱乐部会长明牌观战
	Lint  m_isMedal;				//是否有勇士勋章

	LMsgLMG2LCreateDesk() :LMsg(MSG_LMG_2_L_CREATE_DESK)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
		m_flag = 0; 
		m_gold = 0;			
		m_state = 0;		
		m_robotNum = 0;		
		m_cellscore = 0;
		m_playTypeCount = 0;
		m_deskType = 0;
		m_feeType = 0;
		m_cheatAgainst = 0;
		m_cost = 0;
		m_even = 0;
		m_Greater2CanStart=0;
		m_free = 0;
		m_startTime = 0;
		m_endTime = 0;
		m_GpsLimit = 0;

		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;
		m_isMedal = 0;			//是否有勇士勋章

	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);

		m_usert.Read(buff);

		buff.Read(m_cellscore);
		buff.Read(m_playTypeCount);
		buff.Read(m_deskType);
		buff.Read(m_feeType);
		buff.Read(m_cheatAgainst);

		buff.Read(m_flag);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);

		buff.Read(m_cost);
		buff.Read(m_even); 
		buff.Read(m_free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);
	/*	for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}*/

		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player0CardValue[i].kColor);
			buff.Read(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player1CardValue[i].kColor);
			buff.Read(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player2CardValue[i].kColor);
			buff.Read(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player3CardValue[i].kColor);
			buff.Read(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Read(m_SendCardValue[i].kColor);
			buff.Read(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Read(m_HaoZiCardValue[i].kColor);
			buff.Read(m_HaoZiCardValue[i].kNumber);
		}
		buff.Read(m_playTypeCount);
		for (Lint i = 0 ; i < m_playTypeCount; i ++ )
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_Greater2CanStart);
		buff.Read(m_GpsLimit);

		buff.Read(m_allowLookOn);
		buff.Read(m_clubOwerLookOn);
		buff.Read(m_isMedal);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);

		m_usert.Write(buff);

		buff.Write(m_cellscore);
		buff.Write(m_playTypeCount);
		buff.Write(m_deskType);
		buff.Write(m_feeType);
		buff.Write(m_cheatAgainst);


		buff.Write(m_flag);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);

		buff.Write(m_cost);
		buff.Write(m_even);
		buff.Write(m_free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);
		//for (Lint i = 0; i < CARD_COUNT; ++i)
		//{
		//	buff.Write(m_cardValue[i].m_color);
		//	buff.Write(m_cardValue[i].m_number);
		//}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player0CardValue[i].kColor);
			buff.Write(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player1CardValue[i].kColor);
			buff.Write(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player2CardValue[i].kColor);
			buff.Write(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player3CardValue[i].kColor);
			buff.Write(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Write(m_SendCardValue[i].kColor);
			buff.Write(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Write(m_HaoZiCardValue[i].kColor);
			buff.Write(m_HaoZiCardValue[i].kNumber);
		}
		buff.Write(m_playTypeCount);
		for (Lint i = 0 ; i < m_playTypeCount; i ++ )
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_Greater2CanStart);
		buff.Write(m_GpsLimit);

		buff.Write(m_allowLookOn);
		buff.Write(m_clubOwerLookOn);
		buff.Write(m_isMedal);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreateDesk();
	}
};

struct LMsgLMG2LCreateDeskForOther : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lstring				m_strUUID;
	Lint				m_deskID;
	Lstring				m_ip;
	LUser				m_usert;

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈    
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100
	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
											//CardValue			m_cardValue[CARD_COUNT];
	Lint                m_cellscore;
	Lint                m_deskType;         //创建的桌子类型
	Lint                m_feeType;          // 费用类型 ，0:房主付费 1:玩家分摊
	Lint                m_cheatAgainst;     // 是否防作弊，0:不防作弊 1：防作弊

	Lint                m_cost;             //花费
	Lint                m_even;             //均摊费用
	Lint                m_free;
	Lint                m_startTime;
	Lint                m_endTime;

	CardValue			m_Player0CardValue[13];
	CardValue			m_Player1CardValue[13];
	CardValue			m_Player2CardValue[13];
	CardValue			m_Player3CardValue[13];
	CardValue			m_SendCardValue[84];
	CardValue			m_HaoZiCardValue[2];
	Lint				m_playTypeCount;
	std::vector<Lint>	m_playType;			//玩法
	Lint            m_Greater2CanStart;
	Lint  m_GpsLimit; 
	Lint m_tuoguan_mode;
	LMsgLMG2LCreateDeskForOther() :LMsg(MSG_LMG_2_L_CREATE_DESK_FOR_OTHER)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
		m_flag = 0;
		m_deskType = 0;
		m_gold = 0;
		m_state = 0;
		m_robotNum = 0;
		m_playTypeCount = 0;
		m_cellscore = 0;
		m_deskType = 0;
		m_feeType = 0;
		m_cheatAgainst = 0;
		m_cost = 0;
		m_even = 0;
		m_Greater2CanStart=0;
		m_free = 0;
		m_startTime = 0;
		m_endTime = 0;
		m_GpsLimit = 0;
		m_tuoguan_mode = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);

		m_usert.Read(buff);

		buff.Read(m_flag);
		buff.Read(m_deskType);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		buff.Read(m_cellscore);
		buff.Read(m_feeType);
		buff.Read(m_cheatAgainst);

		buff.Read(m_cost);
		buff.Read(m_even);
		buff.Read(m_free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);
		/*	for (Lint i = 0; i < CARD_COUNT; ++i)
		{
		buff.Read(m_cardValue[i].m_color);
		buff.Read(m_cardValue[i].m_number);
		}*/

		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player0CardValue[i].kColor);
			buff.Read(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player1CardValue[i].kColor);
			buff.Read(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player2CardValue[i].kColor);
			buff.Read(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Read(m_Player3CardValue[i].kColor);
			buff.Read(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Read(m_SendCardValue[i].kColor);
			buff.Read(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Read(m_HaoZiCardValue[i].kColor);
			buff.Read(m_HaoZiCardValue[i].kNumber);
		}
		buff.Read(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_Greater2CanStart);
		buff.Read(m_GpsLimit);
		buff.Read(m_tuoguan_mode);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);

		m_usert.Write(buff);

		buff.Write(m_flag);
		buff.Write(m_deskType);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum); 
		buff.Write(m_cellscore);
		buff.Write(m_feeType);
		buff.Write(m_cheatAgainst);

		buff.Write(m_cost);
		buff.Write(m_even); 
		buff.Write(m_free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);
		//for (Lint i = 0; i < CARD_COUNT; ++i)
		//{
		//	buff.Write(m_cardValue[i].m_color);
		//	buff.Write(m_cardValue[i].m_number);
		//}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player0CardValue[i].kColor);
			buff.Write(m_Player0CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player1CardValue[i].kColor);
			buff.Write(m_Player1CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player2CardValue[i].kColor);
			buff.Write(m_Player2CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 13; ++i)
		{
			buff.Write(m_Player3CardValue[i].kColor);
			buff.Write(m_Player3CardValue[i].kNumber);
		}
		for (Lint i = 0; i < 84; ++i)
		{
			buff.Write(m_SendCardValue[i].kColor);
			buff.Write(m_SendCardValue[i].kNumber);
		}
		for (Lint i = 0; i < 2; ++i)
		{
			buff.Write(m_HaoZiCardValue[i].kColor);
			buff.Write(m_HaoZiCardValue[i].kNumber);
		}
		buff.Write(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_Greater2CanStart);
		buff.Write(m_GpsLimit);
		buff.Write(m_tuoguan_mode);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreateDeskForOther();
	}
};


struct LMsgLMG2LDissmissCreatedDesk : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lint                m_deskId;
	Lstring				m_strUUID;
	LUser			    m_usert;

	LMsgLMG2LDissmissCreatedDesk() :LMsg(MSG_LMG_2_L_DISSMISS_CREATED_DESK)
	{
		m_userid = 0;
		m_gateId = 0;

	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_deskId);
		buff.Read(m_strUUID);
		m_usert.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_deskId);
		buff.Write(m_strUUID);
		m_usert.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDissmissCreatedDesk();
	}
};


struct LMsgLMG2LCreatedDeskList : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lint                m_deskId;
	Lstring				m_strUUID;
	LUser			    m_usert;

	Lint				m_deskListCount;
	std::vector<Lint>	m_deskList;

	LMsgLMG2LCreatedDeskList() :LMsg(MSG_LMG_2_L_CREATED_DESK_LIST)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskListCount = 0;

	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_deskId);
		buff.Read(m_strUUID);
		buff.Read(m_deskListCount);
		for (Lint i = 0; i < m_deskListCount; i++)
		{
			Lint deskId;
			buff.Read(deskId);
			m_deskList.push_back(deskId);
		}

		m_usert.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_deskId);
		buff.Write(m_strUUID);

		buff.Write(m_deskListCount);
		for (Lint i = 0; i < m_deskListCount; i++)
		{
			buff.Write(m_deskList[i]);
		}

		m_usert.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreatedDeskList();
	}
};


struct LMsgLMG2LAddToDesk : public LMsg
{
	Lint			m_userid;
	Lint			m_gateId;
	Lstring			m_strUUID;
	Lint			m_deskID;
	Lstring			m_ip;
	LUser			m_usert;
	Lint            m_creatDeskCost;
	Lint            m_Free;
	Lint            m_startTime;
	Lint            m_endTime;
	Lint            m_addDeskFlag;
	Lint			m_isMedal;			//该玩家是否有勇士勋章
	

	LMsgLMG2LAddToDesk() :LMsg(MSG_LMG_2_L_ADDTO_DESK)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
		m_creatDeskCost = 0;
		m_Free = 0;
		m_startTime = 0;
		m_endTime = 0;
		m_addDeskFlag = 0;   // 0-正常加入 1-加入check
		m_isMedal = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		buff.Read(m_creatDeskCost);
		buff.Read(m_Free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);
		buff.Read(m_addDeskFlag);
		buff.Read(m_isMedal);
		m_usert.Read(buff);
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		buff.Write(m_creatDeskCost);
		buff.Write(m_Free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);
		buff.Write(m_addDeskFlag);
		buff.Write(m_isMedal);
		m_usert.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LAddToDesk();
	}
};


struct LMsgL2LMGModifyCard :public LMsg
{
	Lint			m_userid;
	Lstring			m_strUUID;
	Lint			isAddCard;
	Lint			cardType;
	Lint			cardNum;
	Lint			operType;	// 参考 CARDS_OPER_TYPE
	Lstring			admin;
	LMsgL2LMGModifyCard() :LMsg(MSG_L_2_LMG_MODIFY_USER_CARD)
	{
		m_userid = 0;
		isAddCard = 0;
		cardType = 0;
		cardNum = 0;
		operType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(isAddCard);
		buff.Read(cardType);
		buff.Read(cardNum);
		buff.Read(operType);
		buff.Read(admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(isAddCard);
		buff.Write(cardType);
		buff.Write(cardNum);
		buff.Write(operType);
		buff.Write(admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyCard();
	}
};


struct LMsgL2LMGAddUserPlayCount :public LMsg
{
	Lint			m_userid;
	Lstring			m_strUUID;
	LMsgL2LMGAddUserPlayCount() :LMsg(MSG_L_2_LMG_ADD_USER_PLAYCOUNT)
	{
		m_userid = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGAddUserPlayCount();
	}
};


struct LMsgL2LMGAddUserCircleCount :public LMsg
{
	Lint			m_userid;
	Lstring			m_strUUID;
	Lint            m_winCircle;      //0：没赢  1：赢了
	LMsgL2LMGAddUserCircleCount() :LMsg(MSG_L_2_LMG_ADD_USER_CIRCLE_COUNT)
	{
		m_userid = 0;
		m_winCircle = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(m_winCircle);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(m_winCircle);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGAddUserCircleCount();
	}
};

struct PXActiveItem
{
	Lint m_iPXId;
	Lint m_iCardNum;
	PXActiveItem()
	{
		m_iPXId = -1;
		m_iCardNum = 0;
	}

	PXActiveItem(Lint id, Lint cards)
	{
		m_iPXId = id;
		m_iCardNum = cards;
	}
};

struct PXActive
{
	Lint		m_iGameId;
	Lstring		m_strBeginTime;
	Lstring		m_strEndTime;
	Lint		m_iRewardCount;
	std::vector<PXActiveItem> m_vecPXAndReward;

	PXActive()
	{
		m_iGameId = -1;
		m_strBeginTime = "";
		m_strEndTime = "";
		m_iRewardCount = 0;
	}
};

// LMtoL 发送牌型活动信息
struct LMsgLMG2LPXActive :public LMsg
{
	Lint     m_iGameSetCount;
	std::vector<PXActive> m_vecPXActive;
	
	LMsgLMG2LPXActive() :LMsg(MSG_LMG_2_L_PAIXING_ACTIVE)
	{
		m_iGameSetCount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_iGameSetCount);
		for(Lint i = 0 ; i < m_iGameSetCount; ++i)
		{
			PXActive Active;
			buff.Read(Active.m_iGameId);
			buff.Read(Active.m_strBeginTime);
			buff.Read(Active.m_strEndTime);
			buff.Read(Active.m_iRewardCount);
			for(Lint j = 0; j < Active.m_iRewardCount; ++j)
			{
				PXActiveItem item;
				buff.Read(item.m_iPXId);
				buff.Read(item.m_iCardNum);
				Active.m_vecPXAndReward.emplace_back(item);
			}
			m_vecPXActive.emplace_back(Active);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_iGameSetCount = m_vecPXActive.size();
		buff.Write(m_iGameSetCount);
		for(Lint i = 0 ; i < m_iGameSetCount; ++i)
		{
			buff.Write(m_vecPXActive[i].m_iGameId);
			buff.Write(m_vecPXActive[i].m_strBeginTime);
			buff.Write(m_vecPXActive[i].m_strEndTime);
			m_vecPXActive[i].m_iRewardCount = m_vecPXActive[i].m_vecPXAndReward.size();
			buff.Write(m_vecPXActive[i].m_iRewardCount);
			std::vector<PXActiveItem>& vec = m_vecPXActive[i].m_vecPXAndReward;
			for(Lint j = 0; j < m_vecPXActive[i].m_iRewardCount; ++j)
			{
				buff.Write(vec[j].m_iPXId);
				buff.Write(vec[j].m_iCardNum);
			}
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LPXActive();
	}
};

///////////////////////////////////////////////////////////////////
struct LMsgL2LMGRecyleDeskID :public LMsg
{
	Lint			m_serverID;
	Lint			m_deskID;

	Lint        m_clubId;
	Lint        m_playTypeId;
	Lint        m_clubDeskId;

	LMsgL2LMGRecyleDeskID() :LMsg(MSG_L_2_LMG_RECYLE_DESKID)
	{
		m_serverID = 0;
		m_deskID = 0;

		m_clubId=0;
		m_clubDeskId=0;
		m_playTypeId=0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_deskID);
		buff.Read(m_clubId);
		buff.Read(m_clubDeskId);
		buff.Read(m_playTypeId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_deskID);
		buff.Write(m_clubId);
		buff.Write(m_clubDeskId);
		buff.Write(m_playTypeId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGRecyleDeskID();
	}
};

//logic 通知活动免费
struct LMsgCE2LSetGameFree:public LMsg
{
	Lint		m_ServerID;
	Lstring		m_strFreeSet;
	LMsgCE2LSetGameFree() :LMsg(MSG_CE_2_L_SET_GAME_FREE)
	{
		m_ServerID = 0;
		m_strFreeSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strFreeSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strFreeSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetGameFree();
	}
};
struct deskFeeItem {
	Lint m_id;
	Lstring m_name;
	Lint m_round;
	Lint m_cost;
	Lint m_even;
	Lint m_roomCost;   //房卡
	deskFeeItem()
	{
		m_id = 0;
		m_round = 0;
		m_cost = 0;
		m_even = 0;
		m_roomCost = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_name);

		buff.Read(m_round);
		buff.Read(m_cost);
		buff.Read(m_even);
		buff.Read(m_roomCost);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_name);

		buff.Write(m_round);
		buff.Write(m_cost);
		buff.Write(m_even);
		buff.Write(m_roomCost);
		return true;
	}
};

struct commonItem {
	Lint m_id;
	std::string m_name;
	Lint m_value;
	commonItem()
	{
		m_id = 0;
		m_value = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_name);
		buff.Read(m_value);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_name);
		buff.Write(m_value);
		return true;
	}
};
//logic 通知活动免费
struct LMsgCE2LMGConfig :public LMsg
{
	Lint		m_ServerID;
	Lint        m_commonConLength;
	std::vector<commonItem> m_commonConList;
	Lint        m_deskConLength;
	std::vector<deskFeeItem> m_deskFeeList;

	LMsgCE2LMGConfig() :LMsg(MSG_CE_2_LMG_CONFIG)
	{
		m_ServerID = 0;
		m_commonConLength = 0;
		m_deskConLength = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_commonConLength);
		for (Lint i = 0; i < m_commonConLength; i++)
		{
			commonItem Item;
			Item.Read(buff);
			m_commonConList.push_back(Item);
		}
		buff.Read(m_deskConLength);
		for (Lint i = 0; i < m_deskConLength; i++)
		{
			deskFeeItem Item;
			Item.Read(buff);
			m_deskFeeList.push_back(Item);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_commonConLength);

		for (Lsize i = 0; i < m_commonConLength; i++)
		{
			m_commonConList[i].Write(buff);
		}

		buff.Write(m_deskConLength);

		for (Lsize i = 0; i < m_deskConLength; i++)
		{
			m_deskFeeList[i].Write(buff);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGConfig();
	}
};



//logic 通知设置牌型活动配置
struct LMsgCE2LSetPXActive:public LMsg
{
	Lint	m_ServerID;
	Lstring	m_strActiveSet;

	LMsgCE2LSetPXActive() :LMsg(MSG_CE_2_L_SET_PXACTIVE)
	{
		m_ServerID = 0;
		m_strActiveSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strActiveSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strActiveSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetPXActive();
	}
};

//logic 通知设置老玩家送卡活动配置
struct LMsgCE2LSetOUGCActive:public LMsg
{
	Lint	m_ServerID;
	Lstring	m_strActiveSet;

	LMsgCE2LSetOUGCActive() :LMsg(MSG_CE_2_L_SET_OUGCACTIVE)
	{
		m_ServerID = 0;
		m_strActiveSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strActiveSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strActiveSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetOUGCActive();
	}
};

//logic 通知设置老玩家送卡活动配置
struct LMsgCE2LSetExchActive:public LMsg
{
	Lint	m_ServerID;
	Lstring	m_strActiveSet;

	LMsgCE2LSetExchActive() :LMsg(MSG_CE_2_L_SET_EXCHACTIVE)
	{
		m_ServerID = 0;
		m_strActiveSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strActiveSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strActiveSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetExchActive();
	}
};

// 设置活懂内容
struct LMsgCE2LSetActivity:public LMsg
{
	Lint	m_ServerID;
	ActivityID m_activityId;
	Lstring	m_strActivity;

	LMsgCE2LSetActivity() :LMsg(MSG_CE_2_L_SET_ACTIVITY)
	{
		m_ServerID = 0;
		m_strActivity = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_activityId);
		buff.Read(m_strActivity);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_activityId);
		buff.Write(m_strActivity);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetActivity();
	}
};

//inviter
struct LMsgLM_2_LDBBindInviter : public LMsgSC
{
	Lint				m_inviterId;
	Lint                m_userId;
	LMsgLM_2_LDBBindInviter() : LMsgSC(MSG_LM_2_LDB_BIND_INVITER) 
	{
		m_inviterId = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_inviterId), m_inviterId);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgLM_2_LDBBindInviter; }
};

struct LMsgLDB2LMGBindingRelationships : public LMsg
{
#define    NUM_MAX_INVITEE   40
	Lstring		m_strUUID;
	Lint		m_userId;
	Lint		m_inviterId;
	Lint		m_finished;
	Lint		m_inviteeCount;
	Lint		m_invitees[NUM_MAX_INVITEE];
	Lint		m_taskFinished[NUM_MAX_INVITEE];

	LMsgLDB2LMGBindingRelationships() : LMsg(MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS) 
	{
		m_userId = 0;
		m_inviterId = 0;
		m_finished = 0;
		m_inviteeCount = 0;
		memset(m_taskFinished,0,sizeof(m_taskFinished));
		memset(m_invitees,0,sizeof(m_invitees));
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_inviterId);
		buff.Read(m_finished);
		buff.Read(m_inviteeCount);
		for (int i = 0; i < m_inviteeCount; i++)
		{
			buff.Read(m_invitees[i]);
			buff.Read(m_taskFinished[i]);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_inviterId);
		buff.Write(m_finished);
		buff.Write(m_inviteeCount);
		for (int i = 0; i < m_inviteeCount; i++)
		{
			buff.Write(m_invitees[i]);
			buff.Write(m_taskFinished[i]);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGBindingRelationships;
	}
};

//请求 邀请人id及任务完成
struct LMsgLM_2_LDB_ReqInfo : public LMsgSC  
{
	enum ReqCode
	{
		ReqInviterInfo = 1,
		TaskFinished = 2,
		DeleteUser = 3,
	};

	Lint                m_reqCode;    //1:req 2:task finished 
	Lstring				m_strUUID;
	Lint                m_userId;
	Lint                m_param;
	LMsgLM_2_LDB_ReqInfo() : LMsgSC(MSG_LM_2_LDB_REQ_INFO) 
	{
		m_reqCode = 0;
		m_userId = 0;
		m_param = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_reqCode), m_reqCode);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_param), m_param);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_reqCode), m_reqCode);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_param), m_param);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgLM_2_LDB_ReqInfo; }
};

//返回 邀请人id及任务 完成情况
struct LMsgLDB_2_LM_RetInfo : public LMsgSC
{
	Lstring				m_strUUID;
	Lint                m_userId;
	Lint                m_inviterId;   //邀请人的id
	Lint                m_finished;
	LMsgLDB_2_LM_RetInfo() : LMsgSC(MSG_LDB_2_LM_RET_INFO) 
	{
		m_userId = 0;
		m_inviterId = 0;
		m_finished = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_inviterId), m_inviterId);
		ReadMapData(obj, NAME_TO_STR(m_finished), m_finished);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		WriteKeyValue(pack, NAME_TO_STR(m_finished), m_finished);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgLDB_2_LM_RetInfo; }
};

//LoginServer Msg
struct LMsgFromLoginServer:public LMsg
{
	Lint m_gaofang_flag;
	/*
	一下字段由LoginServer填写
	*/
	Lstring	 m_strUUID;
	LBuffPtr m_dataBuff;

	/*
	以下字段由read中解析
	*/
	Lint			m_userMsgId;
	LMsg*			m_userMsg;

	LMsgFromLoginServer() : LMsg(MSG_FROM_LOGINSERVER)
	{
		m_gaofang_flag = 0;
		m_userMsgId = 0;
		m_userMsg = NULL;
	}
	~LMsgFromLoginServer()
	{
		if(m_userMsg)
		{
			delete m_userMsg;
		}
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gaofang_flag);
		buff.Read(m_strUUID);

		m_userMsgId = MSG_ERROR_MSG;
		try
		{
			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
			msgpack::object obj = unpack.get();

			ReadMapData(obj, "kMId", m_userMsgId);
			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("MSG_G_2_L_USER_MSG::RecvMsgPack error");
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gaofang_flag);
		buff.Write(m_strUUID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgFromLoginServer();
	}
};

struct LMsgToLoginServer : public LMsg
{
	Lstring	 m_strUUID;
	LBuffPtr m_dataBuff;

	LMsgToLoginServer() : LMsg(MSG_TO_LOGINSERVER)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgToLoginServer();
	}
};

//客户端数据包经过解析后生成的结构
struct LMsgConvertClientMsg : public LMsg
{
	boost::shared_ptr<LMsgC2SVerifyHead> msgHead;
	boost::shared_ptr<LMsg>	msgEntity;
	boost::shared_ptr<LBuff> msgOriginData;

	LMsgConvertClientMsg() : LMsg(MSG_CONVERT_CLIENTMSG)
	{

	}

	virtual LMsg* Clone()
	{
		return new LMsgConvertClientMsg();
	}
};

//公共心跳包
enum HEARBEAT_WHO
{
	HEARBEAT_WHO_UNKNOWN = 0,
	HEARBEAT_WHO_CENTER,
	HEARBEAT_WHO_LOGICMANAGER,
	HEARBEAT_WHO_LOGICSERVER,
	HEARBEAT_WHO_LOGINSERVER,
	HEARBEAT_WHO_GATESERVER,
	HEARBEAT_WHO_LOGICDB,
	HEARBEAT_WHO_COINSSERVER,
	HEARBEAT_WHO_NUM
};
const char* const HearBeat_Name[HEARBEAT_WHO_NUM] =
{
	"Unknown", "Center", "Manager", "Logic", "Login", "Gate", "DB", "Coins"
};

struct LMsgHeartBeatRequestMsg : public LMsg
{
	HEARBEAT_WHO m_fromWho;
	int m_iServerId;		//服务器标识

	LMsgHeartBeatRequestMsg() : LMsg(MSG_HEARDBEAT_REQUEST)
	{
		m_fromWho   = HEARBEAT_WHO_UNKNOWN;
		m_iServerId = -1;
	}

	virtual bool Read(LBuff& buff)
	{
		int iFromWho = 0;
		buff.Read(iFromWho);
		buff.Read(m_iServerId);

		m_fromWho = (HEARBEAT_WHO)iFromWho;
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_fromWho);
		buff.Write(m_iServerId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHeartBeatRequestMsg();
	}
};

struct LMsgHeartBeatReplyMsg : public LMsg
{
	HEARBEAT_WHO m_fromWho;
	int m_iServerId;		//服务器标识

	LMsgHeartBeatReplyMsg() : LMsg(MSG_HEARDBEAT_REPLY)
	{
		m_fromWho   = HEARBEAT_WHO_UNKNOWN;
		m_iServerId = -1;
	}

	virtual bool Read(LBuff& buff)
	{
		int iFromWho = 0;
		buff.Read(iFromWho);
		buff.Read(m_iServerId);

		m_fromWho = (HEARBEAT_WHO)iFromWho;
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_fromWho);
		buff.Write(m_iServerId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHeartBeatReplyMsg();
	}
};

// 验证用户请求
struct LMsgNewUserVerifyRequest : public LMsg
{
	void* m_pUser;
	LSocketPtr m_gameloginSP;
	Lint m_severID;
	bool m_bWechatLogin;
	Lstring m_openId;
	Lstring m_unionId;
	Lstring m_accessToken;
	Lint m_login_flag;
	Lint m_type;   // 新用戶类型  0==老版本  1==新版
	LMsgNewUserVerifyRequest() : LMsg(MSG_NEWUSER_VERIFY_REQUEST)
	{
		m_login_flag = 0;
		m_pUser = NULL;
		m_severID = 0;
		m_bWechatLogin = false;
		m_type = 0;
	}
	~LMsgNewUserVerifyRequest()
	{

	}

	virtual LMsg* Clone()
	{
		return new LMsgNewUserVerifyRequest();
	}
};

struct LMsgNewUserVerifyReply : public LMsg
{
	void* m_pUser;
	LSocketPtr m_gameloginSP;
	Lint m_severID;
	Lint m_errorCode;	// 结果 和LMsgS2CMsg结果一直
	Lstring m_errorMsg;
	Lint m_login_flag;
	Lint m_type;   // 新用戶类型  0==老版本  1==新版
	LMsgNewUserVerifyReply() : LMsg(MSG_NEWUSER_VERIFY_REPLY)
	{
		m_login_flag = 0;
		m_pUser = NULL;
		m_severID = 0;
		m_errorCode = -1;
		m_type = 0;
	}
	~LMsgNewUserVerifyReply()
	{

	}

	virtual LMsg* Clone()
	{
		return new LMsgNewUserVerifyReply();
	}
};


// 金币服务器信息
struct CoinsInfo
{
	LSocketPtr	m_sp;			//不为空表示金币服务器连接上了
	Lstring		m_ip;			//m_sp有值时有效
	Lshort		m_port;			//m_sp有值时有效 
	Llong	  m_closeTickTime;	//m_sp有值时有效，不为0，表示金币服务器断开了连接，3分钟不恢复才进行剔除.默认值为0

	CoinsInfo()
	{
		m_port = 0;
		m_closeTickTime = 0;
	}
};

struct LMsgCN2LMGLogin :public LMsg
{
	Lstring		m_key;	// 用作验证
	Lstring		m_ip;
	Lshort		m_port;
	Lint		m_deskcount;	// 已经分配的桌子的数量 CoinsServer断线时 用来简单的判断

	LMsgCN2LMGLogin() :LMsg(MSG_CN_2_LMG_LOGIN)
	{
		m_port = 0;
		m_deskcount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_deskcount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_deskcount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGLogin();
	}
};

// 登录结果
struct LMsgLMG2CNLogin :public LMsg
{
	Lint		m_result;		// 0成功
	Lint		m_deskcount;	// 已经分配的桌子的数量 CoinsServer断线时 用来简单的判断

	LMsgLMG2CNLogin() :LMsg(MSG_LMG_2_CN_LOGIN)
	{
		m_result = -1;
		m_deskcount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_result);
		buff.Read(m_deskcount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_result);
		buff.Write(m_deskcount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNLogin();
	}
};

struct LMsgCN2LMGFreeDeskReq :public LMsg
{
	Lint		m_gameType;
	Lint		m_count;	// 请求的数量

	LMsgCN2LMGFreeDeskReq() :LMsg(MSG_CN_2_LMG_FREE_DESK_REQ)
	{
		m_gameType = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gameType);
		buff.Read(m_count);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gameType);
		buff.Write(m_count);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGFreeDeskReq();
	}
};

struct CoinsDesk
{
	CoinsDesk()
	{
		m_id = 0;
		m_logicID = 0;
	}

	Lint	m_id;		// 桌子号
	Lint	m_logicID;	// 要分配到的逻辑服务器

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_logicID);
		return true;
	}
};

struct LMsgLMG2CNFreeDeskReply :public LMsg
{
	Lint		m_gameType;
	std::vector<CoinsDesk> m_desk;

	LMsgLMG2CNFreeDeskReply() :LMsg(MSG_LMG_2_CN_FREE_DESK_REPLY)
	{
		m_gameType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gameType);
		int count = 0;
		buff.Read(count);
		for (Lint i = 0 ; i < count; i ++ )
		{
			CoinsDesk desk;
			desk.Read( buff );
			m_desk.push_back(desk);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gameType);
		buff.Write((int)m_desk.size());
		for (Lsize i = 0 ; i < m_desk.size(); i ++ )
		{
			m_desk[i].Write(buff);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNFreeDeskReply();
	}
};

struct LMsgCN2LMGRecycleDesk :public LMsg
{
	std::vector<int> m_deskid;

	LMsgCN2LMGRecycleDesk() :LMsg(MSG_CN_2_LMG_RECYCLE_DESK)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		int count = 0;
		buff.Read(count);
		for (Lint i = 0 ; i < count; i ++ )
		{
			Lint deskid;
			buff.Read(deskid);
			m_deskid.push_back(deskid);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_deskid.size());
		for (Lsize i = 0 ; i < m_deskid.size(); i ++ )
		{
			buff.Write(m_deskid[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGRecycleDesk();
	}
};

struct LMsgLMG2CNUserLogin:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_gateId;
	Lstring		m_ip;
	LUser		m_usert;	// 玩家基础数据 同步
	LMsgLMG2CNUserLogin() :LMsg(MSG_LMG_2_CN_USER_LOGIN)
	{
		m_gateId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_gateId);
		buff.Read(m_ip);
		m_usert.Read( buff );
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_gateId);
		buff.Write(m_ip);
		m_usert.Write( buff );
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNUserLogin();
	}
};

struct LMsgLMG2CNEnterCoinDesk : public LMsg
{
	Lstring				m_strUUID;
	Lint				m_gateId;
	Lstring				m_ip;
	LUser				m_usert;

	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
	CardValue			m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;			//玩法
	Lint				m_coins;			// 玩家的金币

	LMsgLMG2CNEnterCoinDesk() :LMsg(MSG_LMG_2_CN_ENTER_COIN_DESK)
	{
		m_gateId = 0;
		m_state = 0;
		m_robotNum = 0;
		m_coins = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_gateId);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].kColor);
			buff.Read(m_cardValue[i].kNumber);
		}
		int playTypeCount = 0;
		buff.Read(playTypeCount);
		for (int i = 0 ; i < playTypeCount; i ++ )
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_coins);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_gateId);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].kColor);
			buff.Write(m_cardValue[i].kNumber);
		}
		buff.Write((int)m_playType.size());
		for (Lsize i = 0 ; i < m_playType.size(); i ++ )
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_coins);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNEnterCoinDesk();
	}
};

struct LMsgCN2LMGModifyUserCoins :public LMsg
{
	Lint			m_userid;
	Lstring			m_strUUID;
	Lint			isAddCoins;
	Lint			coinsNum;
	Lint			operType;	// 参考COINS_OPER_TYPE
	LMsgCN2LMGModifyUserCoins() :LMsg(MSG_CN_2_LMG_MODIFY_USER_COINS)
	{
		m_userid = 0;
		isAddCoins = 0;
		coinsNum = 0;
		operType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(isAddCoins);
		buff.Read(coinsNum);
		buff.Read(operType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(isAddCoins);
		buff.Write(coinsNum);
		buff.Write(operType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGModifyUserCoins();
	}
};

struct LMsgLMG2CNGetCoins :public LMsg
{
	Lstring			m_strUUID;
	Lint			m_userid;
	Lint			m_gateid;

	LMsgLMG2CNGetCoins() :LMsg(MSG_LMG_2_CN_GET_COINS)
	{
		m_userid = 0;
		m_gateid = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_gateid);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_gateid);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNGetCoins();
	}
};

struct LMsgLMG2GLCoinsServerInfo :public LMsg
{
	CoinsInfo	m_conis;

	LMsgLMG2GLCoinsServerInfo() :LMsg(MSG_LMG_2_GL_COINS_SERVER_INFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_conis.m_ip);
		buff.Read(m_conis.m_port);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_conis.m_ip);
		buff.Write(m_conis.m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GLCoinsServerInfo();
	}
};

struct LMsgG2CNLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgG2CNLogin() :LMsg(MSG_G_2_CN_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2CNLogin();
	}
};

struct LMsgL2CNLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgL2CNLogin() :LMsg(MSG_L_2_CN_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CNLogin();
	}
};

#define DIFFOPOUTTIME 10	// 服务器倒计时和客户端倒计时的差距

struct LMsgCN2LCreateCoinDesk : public LMsg
{
	struct User
	{
		User()
		{
			m_gateId = 0;
			m_pos = INVAILD_POS;
		}

		Lint				m_gateId;
		Lstring				m_ip;
		LUser				m_usert;
		Lint				m_pos;

		bool Read(LBuff& buff)
		{
			buff.Read(m_gateId);
			buff.Read(m_ip);
			m_usert.Read(buff);
			buff.Read(m_pos);
			return true;
		}

		bool Write(LBuff& buff)
		{
			buff.Write(m_gateId);
			buff.Write(m_ip);
			m_usert.Write(buff);
			buff.Write(m_pos);
			return true;
		}
	};
	std::vector<User>  m_users;
	
	Lint                m_deskId;
	Lint				m_state;			// GameType
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
	CardValue			m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;			//玩法
	Lint				m_baseScore;		// 底分
	Lint				m_changeOutTime;	// 换牌的超时时间 <=0 不限制
	Lint				m_opOutTime;		// 操作倒计时 单位秒 <=0 表示不倒计时


	LMsgCN2LCreateCoinDesk() :LMsg(MSG_CN_2_L_CREATE_COIN_DESK)
	{
		m_deskId = 0;
		m_state = 0;
		m_robotNum = 0;
		m_baseScore = 0;
		m_changeOutTime = 0;
		m_opOutTime = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		int usercount = 0;
		buff.Read(usercount);
		for ( int i = 0; i < usercount; ++i )
		{
			m_users.push_back( User() );
			m_users.back().Read( buff );
		}
		
		buff.Read(m_deskId);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].kColor);
			buff.Read(m_cardValue[i].kNumber);
		}
		Lint playTypeCount = 0;
		buff.Read(playTypeCount);
		for (Lint i = 0 ; i < playTypeCount; i ++ )
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_baseScore);
		buff.Read(m_changeOutTime);
		buff.Read(m_opOutTime);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_users.size());
		for ( Lsize i = 0; i < m_users.size(); ++i )
		{
			m_users[i].Write( buff );
		}
		buff.Write(m_deskId);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].kColor);
			buff.Write(m_cardValue[i].kNumber);
		}
		buff.Write((int)m_playType.size());
		for (Lsize i = 0 ; i < m_playType.size(); i ++ )
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_baseScore);
		buff.Write(m_changeOutTime);
		buff.Write(m_opOutTime);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LCreateCoinDesk();
	}
};

struct LMsgL2CNCreateCoinDeskRet: public LMsg
{
	enum ErrorCode
	{
		CoinDesk_NoError = 0,
		CoinDesk_SizeError,
		CoinDesk_UserError,
		CoinDesk_PosError,
		CoinDesk_Unknown,
	};
	Lint                m_deskId;
	Lint				m_errorCode;			//0=成功

	LMsgL2CNCreateCoinDeskRet() :LMsg(MSG_L_2_CN_CREATE_COIN_DESK_RET)
	{
		m_deskId = 0;
		m_errorCode = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_deskId);
		buff.Read(m_errorCode);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_deskId);
		buff.Write(m_errorCode);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CNCreateCoinDeskRet();
	}
};

struct LMsgL2CNEndCoinDesk :public LMsg
{
	Lint		m_deskId;
	Lint        m_score[4];     //金币场结算得分
	LBuffPtr	m_dataResult;		//结算消息 有金币服务器发给客户端

	LMsgL2CNEndCoinDesk() :LMsg(MSG_L_2_CN_END_COIN_DESK)
	{
		m_deskId = 0;
		memset(m_score,0,sizeof(m_score));
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_deskId);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Read(m_score[i]);
		}
		m_dataResult = LBuffPtr(new LBuff);
		m_dataResult->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_deskId);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Write(m_score[i]);
		}
		buff.Write(m_dataResult->Data() + m_dataResult->GetOffset(), m_dataResult->Size() - m_dataResult->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CNEndCoinDesk();
	}
};

struct LMsgC2SGoOnCoinsDesk :public LMsgSC
{
	Lint		m_state;				//玩法规则: 0 转转  1  长沙  101-血战到底  102-血流成河
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番

	LMsgC2SGoOnCoinsDesk() :LMsgSC(MSG_C_2_S_GOON_COINS_ROOM)
	{
		m_state = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_playType), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v = 0;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_playType.push_back(v);
			}
		}
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGoOnCoinsDesk(); }
};

struct LMsgS2CGoOnCoinsDeskRet :public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-金币不够，2-玩家不处于结算状态，3-未知错误 4-创建桌子失败,5-创建参数有问题
	Lint		m_remainCount; //m_errorCode=1时有效 表示玩家可以领取的金币的次数

	LMsgS2CGoOnCoinsDeskRet() :LMsgSC(MSG_S_2_C_GOON_COINS_ROOM)
	{
		m_errorCode = 0;
		m_remainCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGoOnCoinsDeskRet(); }
};

struct LMsgS2CKickCoinsDesk :public LMsgSC
{
	Lint		m_reasonCode;//0-准备超时，1-金币不够，2-玩家已在金币场，3-未知原因 4-房间被解散了

	LMsgS2CKickCoinsDesk() :LMsgSC(MSG_S_2_C_KICK_COINS_ROOM)
	{
		m_reasonCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_reasonCode), m_reasonCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_reasonCode), m_reasonCode);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CKickCoinsDesk(); }
};

struct LMsgS2CGiveCoins :public LMsgSC
{
	Lint		m_reasonCode;// 赠送原因 1-金币不足赠送
	Lint		m_coins;	 //金币数
	Lint		m_remainCount; // 剩余次数

	LMsgS2CGiveCoins() :LMsgSC(MSG_S_2_C_GIVE_COINS)
	{
		m_reasonCode = 0;
		m_coins = 0;
		m_remainCount = 0;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_reasonCode), m_reasonCode);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGiveCoins(); }
};

struct LMsgC2SGetCoins :public LMsgSC
{
	Lint m_userid;	// 用户ID 目前为了方便需要客户端填写下

	LMsgC2SGetCoins() :LMsgSC(MSG_C_2_S_GET_COINS)
	{
		m_userid = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_userid), m_userid);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userid), m_userid);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGetCoins(); }
};

struct LMsgS2CGetCoins :public LMsgSC
{
	Lint		m_result;// 0-成功 1-剩余次数为0
	Lint		m_coins;	 //金币数
	Lint		m_remainCount; // 剩余次数

	LMsgS2CGetCoins() :LMsgSC(MSG_S_2_C_GET_COINS)
	{
		m_result = 0;
		m_coins = 0;
		m_remainCount = 0;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGetCoins(); }
};

// 金币桌其他玩家的金币数
struct LMsgS2COtherCoins :public LMsgSC
{
	Lint	m_pos;
	Lint	m_coins;

	LMsgS2COtherCoins() :LMsgSC(MSG_S_2_C_OTHER_COINS)
	{
		m_pos = INVAILD_POS;
		m_coins = 0;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2COtherCoins(); }
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求 点赞记录
struct LMsgL2LBDReqCRELog:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userId;
	Lint    m_time;

	LMsgL2LBDReqCRELog() :LMsg(MSG_L_2_LDB_CRE_LOG)
	{
		m_userId = 0;
		m_time = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_time);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_time);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LBDReqCRELog();
	}
};

//////////////////////////////////////////////////////////////////////////
//一局结束，保存 本次可点赞记录
struct LMsgL2LDBSaveCRELog :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_id[4];    //玩家id
	Lint      m_deskID;   //桌子id
	Lstring   m_strLogId; //log id
	Lint      m_time;     //比赛时间

	LMsgL2LDBSaveCRELog() :LMsg(MSG_L_2_LDB_ENDCRE_LOG)
	{
		memset(m_id,0,sizeof(m_id));
		m_deskID = 0;
		m_time = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Read(m_id[i]);
		}
		buff.Read(m_deskID);
		buff.Read(m_strLogId);
		buff.Read(m_time);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Write(m_id[i]);
		}
		buff.Write(m_deskID);
		buff.Write(m_strLogId);
		buff.Write(m_time);
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgL2LDBSaveCRELog(); 
	}
};

// typedef struct credit_log_s
// {
//  	Lint	  m_id[4];    //玩家id
//  	Lint      m_deskID;   //桌子id
//  	Lstring   m_strLogId;
//  	Lint      m_time;     //时间
// }CRELog;

//////////////////////////////////////////////////////////////////////////
//logicdb 2 logicManger 返回点赞记录
struct LMsgLDB2LM_RecordCRELog :public LMsg
{
	enum
	{
		Length = 64,                   //最大容纳长度
	};
	Lstring             m_strUUID;     //用户uuid
	Lint                m_userId;      //用户id
	Lint                m_user[Length];//记录中的需要点赞玩家id
	Lint                m_count;       //记录条数
	std::vector<std::string> m_record; //记录

	LMsgLDB2LM_RecordCRELog() :LMsg(MSG_LDB_2_LM_RET_CRELOGHIS)
	{
		m_record.clear();
		m_count = 0;
		m_userId = 0;
		memset(m_user,0,sizeof(m_user));
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		for(Lint i = 0; i < Length; ++i)
		{
			buff.Read(m_user[i]);
		}
		buff.Read(m_count);
		for(Lint i = 0; i < m_count; ++i)
		{
			std::string log;
			buff.Read(log);

			m_record.push_back(log);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		Lint count = m_record.size();
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		for(Lint i = 0; i < Length; ++i)
		{
			buff.Write(m_user[i]);
		}
		buff.Write(count);
		for(Lsize i = 0; i < m_record.size(); ++i)
		{
			std::string& log = m_record[i];
			buff.Write(log);
		}
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgLDB2LM_RecordCRELog(); 
	}
};

// logicdb 删除点赞记录
struct LMsgL2LDBDEL_GTU :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_userId;    //玩家id  
	Lstring   m_strLog;    //点赞记录json

	LMsgL2LDBDEL_GTU() :LMsg(MSG_L_2_LDB_REQ_DELGTU)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_strLog);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_strLog);
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgL2LDBDEL_GTU(); 
	}
};

// logicdb 获取玩家成绩
struct LMsgL2LDBReqPlayerScore :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_userId;    //玩家id  


	LMsgL2LDBReqPlayerScore() :LMsg(MSG_L_2_LDB_REQ_PLAYER_SCORE)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_msgId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqPlayerScore();
	}
};

// logicdb 更新玩家成绩
struct LMsgL2LDBUpdatePlayerScore :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_userId;    //玩家id  
	Lint      m_win;       //0当局输 1当局赢


	LMsgL2LDBUpdatePlayerScore() :LMsg(MSG_L_2_LDB_UPDATE_PLAYER_SCORE)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_win);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_msgId);
		buff.Write(m_win);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBUpdatePlayerScore();
	}
};





//logicdb 2 logicManger 广播消息
struct LMsgLDB2LMBCast :public LMsg
{

	Lint                m_count;       //消息条数
	std::vector<std::string> m_message; //记录

	LMsgLDB2LMBCast() :LMsg(MSG_LDB_2_LM_BCAST)
	{
		m_message.clear();
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{


		buff.Read(m_count);
		for (Lint i = 0; i < m_count; ++i)
		{
			std::string item;
			buff.Read(item);

			m_message.push_back(item);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_count);
		for (Lsize i = 0; i < m_count; ++i)
		{
			std::string& tempStr = m_message[i];
			buff.Write(tempStr);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMBCast();
	}
};



//vip比赛结束
struct LMsgLM2CEN_ADD_CRE :public LMsg
{
	Lint      m_user[4];    //logicmanager2center 增加信用值

	LMsgLM2CEN_ADD_CRE() :LMsg(MSG_LM_2_CEN_ADD_CRE)
	{
		memset(m_user,0,sizeof(m_user));
	}

	virtual bool Read(LBuff& buff)
	{
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Read(m_user[i]);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Write(m_user[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgLM2CEN_ADD_CRE(); 
	}
};

//centerserver 保存用户 点赞记录入库
struct LMsgC2C_ADD_CRE :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_userId;    //玩家id   //桌子id

	LMsgC2C_ADD_CRE() :LMsg(MSG_C_2_C_ADD_CRE)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
	
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgC2C_ADD_CRE(); 
	}
};

struct LMsgL2LMGExchCard :public LMsg   
{
	Lint			m_userid;
	Lstring			m_strUUID;
	Lint			m_add;
	Lint			m_del;
	Lint			cardType;
	Lint			operType;
	Lstring			admin;
	LMsgL2LMGExchCard() :LMsg(MSG_LMG_2_L_EXCH_CARD)
	{
		m_userid = 0;
		m_add = 0;
		cardType = 0;
		m_del = 0;
		operType = 0;
		cardType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(m_add);
		buff.Read(m_del);
		buff.Read(cardType);
		buff.Read(operType);
		buff.Read(admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(m_add);
		buff.Write(m_del);
		buff.Write(cardType);
		buff.Write(operType);
		buff.Write(admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGExchCard();
	}
};

///////////////////////////////////////////////////////club--begin/////////////////////////////////////////////////////////
//center <==>logicManager

//////////////////////////////////////////////////////////////////////////
//俱乐部用户 logicManager 
struct myClubUser
{
	Lint             m_userId;                          //用户id
	Lint             m_status;                          //用户状态
	Lstring             m_nike;
	Lstring             m_headUrl;
	Lint                  m_type;


	myClubUser()
	{
		m_userId=0;
		m_status=0;
		m_type = 0;
	}
	myClubUser(Lint userId, Lint status=0)
	{
		m_userId=userId;
		m_status=status;
	}
	~myClubUser() {}
};

/* 玩家在俱乐部中的角色 */
enum USER_IN_CLUB_TYPE
{
	USER_IN_CLUB_TYPE_NORMAL = 1,     //普通会员
	USER_IN_CLUB_TYPE_ADMIN = 2,      //管理员
	USER_IN_CLUB_TYPE_OWNER = 3       //会长
};

struct clubUser
{
	Lint m_id;
	Lint m_clubSq;
	Lint m_clubId;
	Lint m_userId;
	Lint m_status;
	Lstring m_nike;
	Lstring m_headUrl;
	Lint m_type;
	Lint m_point;
	clubUser()
	{
		m_id=0;
		m_clubSq = 0;
		m_clubId=0;
		m_userId=0;
		m_status=0;
		m_type = 0;
		m_point = 0;
	}

	//clubUser(clubUser* temp)
	//{
	//	if (temp == NULL)return;
	//	m_id=temp->m_id;
	//	m_clubId=temp->m_clubId;
	//	m_userId=temp->m_userId;
	//	m_status=temp->m_status;
	//	m_nike = temp->m_nike;
	//	m_headUrl = temp->m_headUrl;
	//	m_type = temp->m_type;
	//	m_point = temp->m_point;
	//}

	bool init(boost::shared_ptr<clubUser> temp)
	{
		if (temp.get() == NULL)return false;
		m_id = temp->m_id;
		m_clubSq = temp->m_clubSq;
		m_clubId = temp->m_clubId;
		m_userId = temp->m_userId;
		m_status = temp->m_status;
		m_nike = temp->m_nike;
		m_headUrl = temp->m_headUrl;
		m_type = temp->m_type;
		m_point = temp->m_point;
		return true;
	}

	clubUser & operator=(clubUser & temp)
	{
		m_id = temp.m_id;
		m_clubSq = temp.m_clubSq;
		m_clubId = temp.m_clubId;
		m_userId = temp.m_userId;
		m_status = temp.m_status;
		m_nike = temp.m_nike;
		m_headUrl = temp.m_headUrl;
		m_type = temp.m_type;
		m_point = temp.m_point;
		return *this;
	}

};

struct ManagerClubDeskInfo
{
	Lint  m_clubDeskId;
	Lint  m_showDeskId;

	ManagerClubDeskInfo()
	{
		m_clubDeskId=0;
		m_showDeskId=0;
	}

	ManagerClubDeskInfo & operator=(ManagerClubDeskInfo & clubDeskInfo)
	{
		m_clubDeskId = clubDeskInfo.m_clubDeskId;
		m_showDeskId = clubDeskInfo.m_showDeskId;
		return *this;
	}

};

struct clubPlayType
{
	Lint m_id;
	Lint m_clubSq;
	Lint m_clubId;
	Lstring m_playTypes;
	Lint m_tableNum;
	Lint m_status;
	Lint  m_clubDeskCount;
	std::vector<ManagerClubDeskInfo> m_clubdeskList;
	clubPlayType()
	{
		m_id=0;
		m_clubId=0;
		m_clubSq = 0;
		m_playTypes="";
		m_tableNum=0;
		m_status=0;
		m_clubDeskCount=0;
	}

	clubPlayType(const clubPlayType& temp)
	{
		m_id=temp.m_id;
		m_clubSq = temp.m_clubSq;
		m_clubId=temp.m_clubId;
		m_playTypes=temp.m_playTypes;
		m_tableNum=temp.m_tableNum;
		m_status=temp.m_status;
		m_clubDeskCount=temp.m_clubDeskCount;
		m_clubdeskList=temp.m_clubdeskList;
	}

	bool init(boost::shared_ptr<clubPlayType> temp)
	{
		if (temp.get() == NULL)return false;
		m_id = temp->m_id;
		m_clubId = temp->m_clubId;
		m_playTypes = temp->m_playTypes;
		m_tableNum = temp->m_tableNum;
		m_status = temp->m_status;
		m_clubDeskCount = temp->m_clubDeskCount;
		m_clubdeskList = temp->m_clubdeskList;
		return true;
	}

	clubPlayType & operator=(clubPlayType & temp)
	{
		m_id = temp.m_id;
		m_clubSq = temp.m_clubSq;
		m_clubId = temp.m_clubId;
		m_playTypes = temp.m_playTypes;
		m_tableNum = temp.m_tableNum;
		m_status = temp.m_status;
		m_clubDeskCount = temp.m_clubDeskCount;
		m_clubdeskList = temp.m_clubdeskList;
		return *this;
	}

};



//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LClubInfo :public LMsg
{
	Lint m_id;
	Lint m_clubId;
	Lstring m_name;
	Lint m_coin;
	Lint m_ownerId;
	Lint m_userSum;
	Lstring m_clubUrl;
	Lint m_status;
	Lint m_type;
	Lint m_feeType;

	Lint m_clubDeskCount;
	std::vector<ManagerClubDeskInfo> m_clubDeskList;                       //俱乐部桌子

	Lint memberCount;
	std::vector<clubUser>	memberList;
	Lint playTypeCout;
	std::vector<clubPlayType> playTypeList;

	LMsgCe2LClubInfo() :LMsg(MSG_CE_2_LMG_CLUB)
	{
		m_id=0;
		m_clubId=0;
		m_name="";
		m_coin=0;
		m_ownerId=0;
		m_userSum=0;
		m_clubUrl="";
		m_status=0;
		m_type=0;
		m_feeType=0;
		memberCount=0;
		playTypeCout=0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_clubId);
		buff.Read(m_name);
		buff.Read(m_coin);
		buff.Read(m_ownerId);
		buff.Read(m_userSum);
		buff.Read(m_clubUrl);
		buff.Read(m_status);
		buff.Read(m_type);
		buff.Read(m_feeType);

		buff.Read(m_clubDeskCount);
		for (Lint i = 0;i<m_clubDeskCount; ++i)
		{
			ManagerClubDeskInfo  info;
			buff.Read(info.m_clubDeskId);
			buff.Read(info.m_showDeskId);
			m_clubDeskList.push_back(info);
		}

		buff.Read(memberCount);
		for (Lint i=0; i < memberCount; ++i)
		{
			clubUser info;
			buff.Read(info.m_id);
			buff.Read(info.m_clubSq);
			buff.Read(info.m_clubId);
			buff.Read(info.m_userId);
			buff.Read(info.m_status);
			buff.Read(info.m_nike);
			buff.Read(info.m_headUrl);
			buff.Read(info.m_type);
			memberList.push_back(info);
		}

		buff.Read(playTypeCout);
		for (Lint i=0; i < playTypeCout; ++i)
		{
			clubPlayType info;
			buff.Read(info.m_id);
			buff.Read(info.m_clubSq);
			buff.Read(info.m_clubId);
			buff.Read(info.m_playTypes);
			buff.Read(info.m_tableNum);
			buff.Read(info.m_status);
			buff.Read(info.m_clubDeskCount);
			for (int j=0; j<info.m_clubDeskCount; j++)
			{
				ManagerClubDeskInfo  deskInfo;
				buff.Read(deskInfo.m_clubDeskId);
				buff.Read(deskInfo.m_showDeskId);
				info.m_clubdeskList.push_back(deskInfo);

			}
			playTypeList.push_back(info);
		}

		return true;
	}


	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_clubId);
		buff.Write(m_name);
		buff.Write(m_coin);
		buff.Write(m_ownerId);
		buff.Write(m_userSum);
		buff.Write(m_clubUrl);
		buff.Write(m_status);
		buff.Write(m_type);
		buff.Write(m_feeType);

		m_clubDeskCount = m_clubDeskList.size();
		buff.Write(m_clubDeskCount);
		for (Lint i = 0; i < m_clubDeskCount; ++i)
		{
			buff.Write(m_clubDeskList[i].m_clubDeskId);
			buff.Write(m_clubDeskList[i].m_showDeskId);
		}

		memberCount=memberList.size();
		buff.Write(memberCount);
		for (Lint i=0; i < memberCount; ++i)
		{
			buff.Write(memberList[i].m_id);
			buff.Write(memberList[i].m_clubSq);
			buff.Write(memberList[i].m_clubId);
			buff.Write(memberList[i].m_userId);
			buff.Write(memberList[i].m_status);
			buff.Write(memberList[i].m_nike);
			buff.Write(memberList[i].m_headUrl);
			buff.Write(memberList[i].m_type);
		}


		playTypeCout=playTypeList.size();
		buff.Write(playTypeCout);
		for (Lint i=0; i < playTypeCout; ++i)
		{
			buff.Write(playTypeList[i].m_id);
			buff.Write(playTypeList[i].m_clubSq);
			buff.Write(playTypeList[i].m_clubId);
			buff.Write(playTypeList[i].m_playTypes);
			buff.Write(playTypeList[i].m_tableNum);
			buff.Write(playTypeList[i].m_status);
			buff.Write(playTypeList[i].m_clubDeskCount);

			for (Lint j=0; j<playTypeList[i].m_clubDeskCount; j++)
			{
				buff.Write(playTypeList[i].m_clubdeskList[j].m_clubDeskId);
				buff.Write(playTypeList[i].m_clubdeskList[j].m_showDeskId);
			}
		}

		return true;
	}


	virtual LMsg* Clone()
	{
		return new LMsgCe2LClubInfo();
	}

	virtual Lint preAllocSendMemorySize() //重新分配64K,由于消息长度是short型
	{
		return 1024*64;
	}
};

struct ClubAdminAuth
{
	Lint   m_clubSq;
	Lint   m_changePoint;
	Lint   m_lookupPoint;

	ClubAdminAuth()
	{
		m_clubSq = 0;
		m_changePoint = 0;
		m_lookupPoint = 0;
	}

	~ClubAdminAuth(){}

	ClubAdminAuth& operator=(ClubAdminAuth& auth)
	{
		m_clubSq = auth.m_clubSq;
		m_changePoint = auth.m_changePoint;
		m_lookupPoint = auth.m_lookupPoint;
		return *this;
	}

};

struct ClubItem
{
	Lint m_id;                  //数据库中id编号
	Lint m_clubId;          //俱乐部号
	Lstring m_name;
	Lint m_coin;
	Lint m_ownerId;
	Lint m_userSum;
	Lstring m_clubUrl;
	Lint m_status;
	Lint m_type;
	Lint m_feeType;
	Lint m_createDeskCount;
	ClubAdminAuth  m_adminAuth;         //管理员权限
	std::vector<ManagerClubDeskInfo> m_clubDeskList;                       //俱乐部桌子

	std::list<boost::shared_ptr<clubUser>>	memberList;                                  //会员
	std::list<boost::shared_ptr<clubPlayType>> playTypeList;                       //玩法

	
	
	ClubItem()
	{
		m_id=0;
		m_clubId=0;
		m_name="";
		m_coin=0;
		m_ownerId=0;
		m_userSum=0;
		m_clubUrl="";
		m_status=0;
		m_type=0;
		m_feeType=0;
		m_createDeskCount = 0;
	}

	~ClubItem()
	{
		memberList.clear();
		playTypeList.clear();
		m_clubDeskList.clear();
	}

};

//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LAddClub :public LMsg
{
	Lint m_msgType;    //  0--  center->manager    1 -- manager->center 创建桌子
	Lint m_id;
	Lint m_clubId;
	Lstring m_name;
	Lstring m_district;
	Lint m_coin;
	Lint m_ownerId;
	Lint m_userSum;
	Lstring m_clubUrl;
	Lint m_status;
	Lint m_type;
	Lint m_feeType;
	Lint m_createDeskCount;       //生成桌子数量

	ClubAdminAuth  m_adminAuth;         //管理员权限

	Lint m_clubDeskCount;
	std::vector<ManagerClubDeskInfo> m_clubDeskList;                       //俱乐部桌子

	Lint m_pointRecordCount;
	std::vector<UserPowerRecord>     m_pointRecordList;

	Lint memberCount;
	std::vector<clubUser>	memberList;
	Lint playTypeCout;
	std::vector<clubPlayType> playTypeList;

	LMsgCe2LAddClub() :LMsg(MSG_CE_2_LMG_ADD_CLUB)
	{
		m_msgType = 0;
		m_id = 0;
		m_clubId = 0;
		m_name = "";
		m_coin = 0;
		m_ownerId = 0;
		m_userSum = 0;
		m_clubUrl = "";
		m_status = 0;
		m_type = 0;
		m_feeType = 0;
		memberCount = 0;
		playTypeCout = 0;
		m_createDeskCount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_msgType);
		buff.Read(m_id);
		buff.Read(m_clubId);
		buff.Read(m_name);
		buff.Read(m_district);
		buff.Read(m_coin);
		buff.Read(m_ownerId);
		buff.Read(m_userSum);
		buff.Read(m_clubUrl);
		buff.Read(m_status);
		buff.Read(m_type);
		buff.Read(m_feeType);
		buff.Read(m_createDeskCount);

		buff.Read(m_adminAuth.m_changePoint);
		buff.Read(m_adminAuth.m_lookupPoint);

		buff.Read(m_clubDeskCount);
		for (Lint i = 0; i<m_clubDeskCount; ++i)
		{
			ManagerClubDeskInfo  info;
			buff.Read(info.m_clubDeskId);
			buff.Read(info.m_showDeskId);
			m_clubDeskList.push_back(info);
		}

		buff.Read(m_pointRecordCount);
		for (Lint i = 0; i<m_pointRecordCount; ++i)
		{
			UserPowerRecord  m_record;
			buff.Read(m_record.m_time);
			buff.Read(m_record.m_userId);
			buff.Read(m_record.m_nike);
			buff.Read(m_record.m_photo);
			buff.Read(m_record.m_type);
			buff.Read(m_record.m_point);
			buff.Read(m_record.m_crruPoint);
			buff.Read(m_record.m_operId);
			buff.Read(m_record.m_operNike);
			buff.Read(m_record.m_operPhoto);
			buff.Read(m_record.m_operR);
			buff.Read(m_record.m_clubId);
			buff.Read(m_record.m_clubSq);
			m_pointRecordList.push_back(m_record);
		}

		buff.Read(memberCount);
		for (Lint i = 0; i < memberCount; ++i)
		{
			clubUser info;
			buff.Read(info.m_id);
			buff.Read(info.m_clubSq);
			buff.Read(info.m_clubId);
			buff.Read(info.m_userId);
			buff.Read(info.m_status);
			buff.Read(info.m_nike);
			buff.Read(info.m_headUrl);
			buff.Read(info.m_point);
			buff.Read(info.m_type);
			memberList.push_back(info);
		}

		buff.Read(playTypeCout);
		for (Lint i = 0; i < playTypeCout; ++i)
		{
			clubPlayType info;
			buff.Read(info.m_id);
			buff.Read(info.m_clubSq);
			buff.Read(info.m_clubId);
			buff.Read(info.m_playTypes);
			buff.Read(info.m_tableNum);
			buff.Read(info.m_status);
			buff.Read(info.m_clubDeskCount);
			for (int j = 0; j<info.m_clubDeskCount; j++)
			{
				ManagerClubDeskInfo  deskInfo;
				buff.Read(deskInfo.m_clubDeskId);
				buff.Read(deskInfo.m_showDeskId);
				info.m_clubdeskList.push_back(deskInfo);

			}
			playTypeList.push_back(info);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_msgType);
		buff.Write(m_id);
		buff.Write(m_clubId);
		buff.Write(m_name);
		buff.Write(m_district);
		buff.Write(m_coin);
		buff.Write(m_ownerId);
		buff.Write(m_userSum);
		buff.Write(m_clubUrl);
		buff.Write(m_status);
		buff.Write(m_type);
		buff.Write(m_feeType);
		buff.Write(m_createDeskCount);

		buff.Write(m_adminAuth.m_changePoint);
		buff.Write(m_adminAuth.m_lookupPoint);

		m_clubDeskCount = m_clubDeskList.size();
		buff.Write(m_clubDeskCount);
		for (Lint i = 0; i < m_clubDeskCount; ++i)
		{
			buff.Write(m_clubDeskList[i].m_clubDeskId);
			buff.Write(m_clubDeskList[i].m_showDeskId);
		}

		m_pointRecordCount = m_pointRecordList.size();
		buff.Write(m_pointRecordCount);
		for (Lint i = 0; i < m_pointRecordCount; ++i)
		{
			buff.Write(m_pointRecordList[i].m_time);
			buff.Write(m_pointRecordList[i].m_userId);
			buff.Write(m_pointRecordList[i].m_nike);
			buff.Write(m_pointRecordList[i].m_photo);
			buff.Write(m_pointRecordList[i].m_type);
			buff.Write(m_pointRecordList[i].m_point);
			buff.Write(m_pointRecordList[i].m_crruPoint);
			buff.Write(m_pointRecordList[i].m_operId);
			buff.Write(m_pointRecordList[i].m_operNike);
			buff.Write(m_pointRecordList[i].m_operPhoto);
			buff.Write(m_pointRecordList[i].m_operR);
			buff.Write(m_pointRecordList[i].m_clubId);
			buff.Write(m_pointRecordList[i].m_clubSq);
		}


		memberCount = memberList.size();
		buff.Write(memberCount);
		for (Lint i = 0; i < memberCount; ++i)
		{
			buff.Write(memberList[i].m_id);
			buff.Write(memberList[i].m_clubSq);
			buff.Write(memberList[i].m_clubId);
			buff.Write(memberList[i].m_userId);
			buff.Write(memberList[i].m_status);
			buff.Write(memberList[i].m_nike);
			buff.Write(memberList[i].m_headUrl);
			buff.Write(memberList[i].m_point);
			buff.Write(memberList[i].m_type);
		}


		playTypeCout = playTypeList.size();
		buff.Write(playTypeCout);
		for (Lint i = 0; i < playTypeCout; ++i)
		{
			buff.Write(playTypeList[i].m_id);
			buff.Write(playTypeList[i].m_clubSq);
			buff.Write(playTypeList[i].m_clubId);
			buff.Write(playTypeList[i].m_playTypes);
			buff.Write(playTypeList[i].m_tableNum);
			buff.Write(playTypeList[i].m_status);
			buff.Write(playTypeList[i].m_clubDeskCount);

			for (Lint j = 0; j<playTypeList[i].m_clubDeskCount; j++)
			{
				buff.Write(playTypeList[i].m_clubdeskList[j].m_clubDeskId);
				buff.Write(playTypeList[i].m_clubdeskList[j].m_showDeskId);
			}
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LAddClub();
	}

	virtual Lint preAllocSendMemorySize() //重新分配64K,由于消息长度是short型
	{
		return 1024*64;
	}
};

//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LMGClubAddPlayType :public LMsg
{
	Lint   m_msgType;                   // 0--center->manager    1-manager->center
	clubPlayType   m_playInfo;

	LMsgCe2LMGClubAddPlayType() :LMsg(MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE)
	{
		m_msgType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_msgType);
		buff.Read(m_playInfo.m_id);
		buff.Read(m_playInfo.m_clubSq);
		buff.Read(m_playInfo.m_clubId);
		buff.Read(m_playInfo.m_playTypes);
		buff.Read(m_playInfo.m_tableNum);
		buff.Read(m_playInfo.m_status);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_msgType);
		buff.Write(m_playInfo.m_id);
		buff.Write(m_playInfo.m_clubSq);
		buff.Write(m_playInfo.m_clubId);
		buff.Write(m_playInfo.m_playTypes);
		buff.Write(m_playInfo.m_tableNum);
		buff.Write(m_playInfo.m_status);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGClubAddPlayType();
	}

};

//////////////////////////////////////////////////////////////////////////
enum CLUB_USER_MSG_TYPE
{
	CLUB_USER_MSG_TYPE_APPLY_ADD = 0,
	CLUB_USER_MSG_TYPE_ADD_USER=1,
	CLUB_USER_MSG_TYPE_DEL_USER=2,
	CLUB_USER_MSG_TYPE_FLUSH_POINT=3,
};
struct LMsgCe2LMGClubAddUser :public LMsg
{
	Lint m_msgType;     //   0--用户申请加入  1--添加用户， 2--删除用户,  3--刷新用户内容(能量值) 

	clubUser m_userInfo;
	//Lint m_id;
	//Lint m_clubId;
	//Lint m_userId;
	//Lstring m_userNike;
	//Lstring m_headurl;
	//Lint m_status;
	//Lint m_type;

	LMsgCe2LMGClubAddUser() :LMsg(MSG_CE_2_LMG_CLUB_USER_INFO)
	{
		m_msgType = 0;
		//m_id=0;
		//m_clubId=0;
		//m_userId=0;
		//m_status=0;
		//m_type = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_msgType);
		buff.Read(m_userInfo.m_id);
		buff.Read(m_userInfo.m_clubSq);
		buff.Read(m_userInfo.m_clubId);
		buff.Read(m_userInfo.m_userId);
		buff.Read(m_userInfo.m_status);
		buff.Read(m_userInfo.m_nike);
		buff.Read(m_userInfo.m_headUrl);
		buff.Read(m_userInfo.m_type);
		buff.Read(m_userInfo.m_point);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_msgType);
		buff.Write(m_userInfo.m_id);
		buff.Write(m_userInfo.m_clubSq);
		buff.Write(m_userInfo.m_clubId);
		buff.Write(m_userInfo.m_userId);
		buff.Write(m_userInfo.m_status);
		buff.Write(m_userInfo.m_nike);
		buff.Write(m_userInfo.m_headUrl);
		buff.Write(m_userInfo.m_type);
		buff.Write(m_userInfo.m_point);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGClubAddUser();
	}

};

//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LMGClubDelUser :public LMsg
{
	Lint m_id;
	Lint m_clubId;
	Lint m_userId;
	Lint m_status;
	Lint m_type;
	Lstring m_clubName;
	Lint m_userType;       //用户身份

	LMsgCe2LMGClubDelUser() :LMsg(MSG_CE_2_LMG_CLUB_DEL_USER)
	{
		m_id=0;
		m_clubId=0;
		m_userId=0;
		m_status=0;
		m_type = 0;
		m_userType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_clubId);
		buff.Read(m_userId);
		buff.Read(m_status);
		buff.Read(m_type);
		buff.Read(m_clubName);
		buff.Read(m_userType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_clubId);
		buff.Write(m_userId);
		buff.Write(m_status);
		buff.Write(m_type);
		buff.Write(m_clubName);
		buff.Write(m_userType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGClubDelUser();
	}

};

//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LMGClubHidePlayType :public LMsg
{
	Lint m_id;
	Lint m_clubId;
	Lstring m_playTypes;
	Lint m_tableNum;
	Lint m_status;

	LMsgCe2LMGClubHidePlayType() :LMsg(MSG_CE_2_LMG_CLUB_HIDE_PLAYTYPE)
	{
		m_id=0;
		m_clubId=0;
		m_playTypes="";
		m_tableNum=0;
		m_status=0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_clubId);
		buff.Read(m_playTypes);
		buff.Read(m_tableNum);
		buff.Read(m_status);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_clubId);
		buff.Write(m_playTypes);
		buff.Write(m_tableNum);
		buff.Write(m_status);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGClubHidePlayType();
	}

};

//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LMGClubModifyClubName :public LMsg
{
	Lint m_type;           //类型， 0==俱乐部名字   1==玩法的名字
	Lint m_clubId;
	Lstring m_clubName;
	Lint     m_playTypeId;

	LMsgCe2LMGClubModifyClubName() :LMsg(MSG_CE_2_LMG_CLUB_MIDIFY_CLUB_NAME)
	{
		m_type = 0;
		m_clubId=0;
		m_playTypeId = 0;
		//m_clubName="";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_clubId);
		buff.Read(m_type);
		buff.Read(m_playTypeId);
		buff.Read(m_clubName);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_clubId);
		buff.Write(m_type);
		buff.Write(m_playTypeId);
		buff.Write(m_clubName);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGClubModifyClubName();
	}

};

//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LMGClubModifyClubFee :public LMsg
{
	Lint m_clubId;
	Lint m_clubFeeType;

	LMsgCe2LMGClubModifyClubFee() :LMsg(MSG_CE_2_LMG_CLUB_MODIFY_CLUB_FEE)
	{
		m_clubId = 0;
		m_clubFeeType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_clubId);
		buff.Read(m_clubFeeType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_clubId);
		buff.Write(m_clubFeeType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGClubModifyClubFee();
	}

};

//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LMGDismissDesk :public LMsg
{
	Lint m_clubId;
	Lint m_showDeskId;
	Lint m_6DeskId;
	Lint m_type;                 //解散房间类型    0-普通桌子  1-俱乐部桌子

	LMsgCe2LMGDismissDesk() :LMsg(MSG_CE_2_LMG_DISMISS_DESK)
	{
		m_clubId = 0;
		m_showDeskId = 0;
		m_6DeskId = 0;
		m_type = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_clubId);
		buff.Read(m_showDeskId);
		buff.Read(m_6DeskId);
		buff.Read(m_type);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_clubId);
		buff.Write(m_showDeskId);
		buff.Write(m_6DeskId);
		buff.Write(m_type);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGDismissDesk();
	}

};

//////////////////////////////////////////////////////////////////////////
//logic<==>logicManager
//俱乐部相关的 logic  与 logic Manager

//////////////////////////////////////////////////////////////////////////
//logicManager to logic  俱乐部  这个是批量建桌子，暂时不用了
struct LMsgLMG2LCreateDeskForClub : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lstring				m_strUUID;
	Lint                                       m_deskCount;       //桌子数
	std::vector<Lint>				m_deskIds;          //桌子ids
	Lstring				m_ip;
	LUser				m_usert;

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈    
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100
	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
											//CardValue			m_cardValue[CARD_COUNT];
	Lint                m_cellscore;
	Lint                m_deskType;         //创建的桌子类型
	Lint                m_feeType;          // 费用类型 ，0:房主付费 1:玩家分摊
	Lint                m_cheatAgainst;     // 是否防作弊，0:不防作弊 1：防作弊

	Lint                m_cost;             //花费
	Lint                m_even;             //均摊费用
	Lint                m_free;
	Lint                m_startTime;
	Lint                m_endTime;

	CardValue			m_Player0CardValue[13];
	CardValue			m_Player1CardValue[13];
	CardValue			m_Player2CardValue[13];
	CardValue			m_Player3CardValue[13];
	CardValue			m_SendCardValue[84];
	CardValue			m_HaoZiCardValue[2];
	Lint				m_playTypeCount;
	std::vector<Lint>	m_playType;			//玩法
	Lint            m_Greater2CanStart;
	Lint            m_clubId;
	Lint            m_playTypeId;

	LMsgLMG2LCreateDeskForClub() :LMsg(MSG_LMG_2_L_CREATE_DESK_FOR_CLUB)
	{
		m_userid=0;
		m_gateId=0;
		m_deskCount=0;
		m_flag=0;
		m_deskType=0;
		m_gold=0;
		m_state=0;
		m_robotNum=0;
		m_playTypeCount=0;
		m_cellscore=0;
		m_deskType=0;
		m_feeType=0;
		m_cheatAgainst=0;
		m_cost=0;
		m_even=0;
		m_Greater2CanStart=0;
		m_free=0;
		m_startTime=0;
		m_endTime=0;
		m_clubId=0;
		m_playTypeId;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskCount);
		for (Lint i=0; i < m_deskCount; i++)
		{
			Lint deskId;
			buff.Read(deskId);
			m_deskIds.push_back(deskId);
		}

		buff.Read(m_ip);

		m_usert.Read(buff);

		buff.Read(m_flag);
		buff.Read(m_deskType);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		buff.Read(m_cellscore);
		buff.Read(m_feeType);
		buff.Read(m_cheatAgainst);

		buff.Read(m_cost);
		buff.Read(m_even);
		buff.Read(m_free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);
		/*	for (Lint i = 0; i < CARD_COUNT; ++i)
		{
		buff.Read(m_cardValue[i].m_color);
		buff.Read(m_cardValue[i].m_number);
		}*/

		for (Lint i=0; i < 13; ++i)
		{
			buff.Read(m_Player0CardValue[i].kColor);
			buff.Read(m_Player0CardValue[i].kNumber);
		}
		for (Lint i=0; i < 13; ++i)
		{
			buff.Read(m_Player1CardValue[i].kColor);
			buff.Read(m_Player1CardValue[i].kNumber);
		}
		for (Lint i=0; i < 13; ++i)
		{
			buff.Read(m_Player2CardValue[i].kColor);
			buff.Read(m_Player2CardValue[i].kNumber);
		}
		for (Lint i=0; i < 13; ++i)
		{
			buff.Read(m_Player3CardValue[i].kColor);
			buff.Read(m_Player3CardValue[i].kNumber);
		}
		for (Lint i=0; i < 84; ++i)
		{
			buff.Read(m_SendCardValue[i].kColor);
			buff.Read(m_SendCardValue[i].kNumber);
		}
		for (Lint i=0; i < 2; ++i)
		{
			buff.Read(m_HaoZiCardValue[i].kColor);
			buff.Read(m_HaoZiCardValue[i].kNumber);
		}
		buff.Read(m_playTypeCount);
		for (Lint i=0; i < m_playTypeCount; i++)
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_Greater2CanStart);
		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskCount);
		for (Lint i=0; i < m_deskCount; i++)
		{
			buff.Write(m_deskIds[i]);
		}


		buff.Write(m_ip);

		m_usert.Write(buff);

		buff.Write(m_flag);
		buff.Write(m_deskType);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		buff.Write(m_cellscore);
		buff.Write(m_feeType);
		buff.Write(m_cheatAgainst);

		buff.Write(m_cost);
		buff.Write(m_even);
		buff.Write(m_free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);
		//for (Lint i = 0; i < CARD_COUNT; ++i)
		//{
		//	buff.Write(m_cardValue[i].m_color);
		//	buff.Write(m_cardValue[i].m_number);
		//}
		for (Lint i=0; i < 13; ++i)
		{
			buff.Write(m_Player0CardValue[i].kColor);
			buff.Write(m_Player0CardValue[i].kNumber);
		}
		for (Lint i=0; i < 13; ++i)
		{
			buff.Write(m_Player1CardValue[i].kColor);
			buff.Write(m_Player1CardValue[i].kNumber);
		}
		for (Lint i=0; i < 13; ++i)
		{
			buff.Write(m_Player2CardValue[i].kColor);
			buff.Write(m_Player2CardValue[i].kNumber);
		}
		for (Lint i=0; i < 13; ++i)
		{
			buff.Write(m_Player3CardValue[i].kColor);
			buff.Write(m_Player3CardValue[i].kNumber);
		}
		for (Lint i=0; i < 84; ++i)
		{
			buff.Write(m_SendCardValue[i].kColor);
			buff.Write(m_SendCardValue[i].kNumber);
		}
		for (Lint i=0; i < 2; ++i)
		{
			buff.Write(m_HaoZiCardValue[i].kColor);
			buff.Write(m_HaoZiCardValue[i].kNumber);
		}
		buff.Write(m_playTypeCount);
		for (Lint i=0; i < m_playTypeCount; i++)
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_Greater2CanStart);
		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreateDeskForClub();
	}
};

//////////////////////////////////////////////////////////////////////////
//加入并创建桌子
struct LMsgLMG2LCreateClubDeskAndEnter : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lstring				m_strUUID;
	Lint                m_deskId;			//桌子号
	Lstring				m_ip;
	LUser				m_usert;

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈    
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100
	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
											//CardValue			m_cardValue[CARD_COUNT];
	Lint                m_cellscore;
	Lint                m_deskType;         //创建的桌子类型
	Lint                m_feeType;          // 费用类型 ，0:房主付费 1:玩家分摊
	Lint                m_cheatAgainst;     // 是否防作弊，0:不防作弊 1：防作弊

	Lint                m_cost;             //花费
	Lint                m_even;             //均摊费用
	Lint                m_free;
	Lint                m_startTime;
	Lint                m_endTime;

	Lint				m_playTypeCount;
	std::vector<Lint>	m_playType;			//玩法
	Lint				m_Greater2CanStart;
	Lint				m_clubId;
	Lint				m_playTypeId;
	Lint				m_clubDeskId;
	Lint				m_showDeskId;
	Lstring				m_clubName;			//俱乐部名称

	//加入用户信息
	Lint				m_joinUserid;
	Lint				m_joinUserGateId;
	Lstring				m_joinUserStrUUID;
	Lstring				m_joinUserIp;
	LUser				m_joinUserUsert;
	Lint				m_joinUserFree;
	Lint				m_GpsLimit;			//俱乐部,GPS限制
	Lint				m_allowLookOn;		//是否允许观战  0：不允许  1：允许
	Lint				m_clubOwerLookOn;	//是否允许俱乐部会长名牌观战   0：不允许  1：允许
	Lstring				m_GpsLng;			//GPS经度
	Lstring				m_GpsLat;			//GPS纬度
	Lint				m_type;				//创建房间类型：3：从俱乐部匹配进入时创建房间

	Lint                m_redPacketFlag;    //0--没有红包的桌子  1--有红包的桌子
	Lint				m_tuoguan_mode;     // 桌子的托管模式：0 不托管， 1 超时30秒托管 2 超时60秒托管

	Lint				m_clubType;         //俱乐部性质   //类型，0-普通   1-竞技场
	Lint				m_minPointInRoom;   //加入游戏最小值
	Lint				m_minPointXiaZhu;   //最小下注值
	Lint				m_smallZero;        //是否可负分   0 不可以  1  可以
	Lint				m_warnValue;        //能量值提醒预警分值
	Lint				m_adminCount;		//俱乐部中管理员列表数量
	std::vector<Lint>	m_managerList;		//俱乐部中管理员列表
	Lint				m_isMedal;			//玩家是否有勇士勋章


	LMsgLMG2LCreateClubDeskAndEnter() :LMsg(MSG_LMG_2_L_CREATE_CLUB_DESK_AND_ENTER)
	{
		m_userid=0;
		m_gateId=0;
		m_deskId=0;
		m_flag=0;
		m_deskType=0;
		m_gold=0;
		m_state=0;
		m_robotNum=0;
		m_playTypeCount=0;
		m_cellscore=0;
		m_deskType=0;
		m_feeType=0;
		m_cheatAgainst=0;
		m_cost=0;
		m_even=0;
		m_Greater2CanStart=0;
		m_free=0;
		m_startTime=0;
		m_endTime=0;
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_showDeskId=0;

		m_joinUserid=0;
		m_joinUserGateId=0;
		m_joinUserFree=0;
		m_GpsLimit = 0;

		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;
		m_type = 0;

		m_redPacketFlag = 0;

		m_clubType = 0;
		m_minPointInRoom = 0;
		m_minPointXiaZhu = 0;
		m_smallZero = 0;
		m_warnValue = 0;
		m_adminCount = 0;
		m_isMedal = 0;

	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskId);

		buff.Read(m_ip);

		m_usert.Read(buff);

		buff.Read(m_flag);
		buff.Read(m_deskType);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		buff.Read(m_cellscore);
		buff.Read(m_feeType);
		buff.Read(m_cheatAgainst);

		buff.Read(m_cost);
		buff.Read(m_even);
		buff.Read(m_free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);

		buff.Read(m_playTypeCount);
		for (Lint i=0; i < m_playTypeCount; i++)
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_adminCount);
		for (Lint i = 0; i < m_adminCount; ++i)
		{
			Lint t_manager;
			buff.Read(t_manager);
			m_managerList.push_back(t_manager);
		}
		buff.Read(m_Greater2CanStart);
		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_clubDeskId);
		buff.Read(m_showDeskId);
		buff.Read(m_clubName);

		buff.Read(m_joinUserid);
		buff.Read(m_joinUserGateId);
		buff.Read(m_joinUserStrUUID);
		buff.Read(m_joinUserIp);
		buff.Read(m_joinUserFree);
		m_joinUserUsert.Read(buff);
		buff.Read(m_GpsLimit);
		buff.Read(m_GpsLng);
		buff.Read(m_GpsLat);

		buff.Read(m_allowLookOn);
		buff.Read(m_clubOwerLookOn);
		buff.Read(m_type);

		buff.Read(m_redPacketFlag);

		buff.Read(m_clubType);
		buff.Read(m_minPointInRoom);
		buff.Read(m_minPointXiaZhu);
		buff.Read(m_smallZero);
		buff.Read(m_warnValue);
		buff.Read(m_adminCount);
		for (Lint i = 0; i < m_adminCount; i++)
		{
			Lint admin=0;
			buff.Read(admin);
			m_managerList.push_back(admin);
		}
		buff.Read(m_isMedal);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskId);

		buff.Write(m_ip);

		m_usert.Write(buff);

		buff.Write(m_flag);
		buff.Write(m_deskType);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		buff.Write(m_cellscore);
		buff.Write(m_feeType);
		buff.Write(m_cheatAgainst);

		buff.Write(m_cost);
		buff.Write(m_even);
		buff.Write(m_free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);

		buff.Write(m_playTypeCount);
		for (Lint i=0; i < m_playTypeCount; i++)
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_adminCount);
		for (Lint i = 0; i < m_adminCount; ++i)
		{
			buff.Write(m_managerList[i]);
		}
		buff.Write(m_Greater2CanStart);
		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		buff.Write(m_clubDeskId);
		buff.Write(m_showDeskId);
		buff.Write(m_clubName);

		buff.Write(m_joinUserid);
		buff.Write(m_joinUserGateId);
		buff.Write(m_joinUserStrUUID);
		buff.Write(m_joinUserIp);
		buff.Write(m_joinUserFree);
		m_joinUserUsert.Write(buff);
		buff.Write(m_GpsLimit);
		buff.Write(m_GpsLng);
		buff.Write(m_GpsLat);

		buff.Write(m_allowLookOn);
		buff.Write(m_clubOwerLookOn);
		buff.Write(m_type);

		buff.Write(m_redPacketFlag);

		buff.Write(m_clubType);
		buff.Write(m_minPointInRoom);
		buff.Write(m_minPointXiaZhu);
		buff.Write(m_smallZero);
		buff.Write(m_warnValue);
		buff.Write(m_adminCount);
		for (Lint i = 0; i < m_adminCount; i++)
		{
			buff.Write(m_managerList[i]);
		}
		buff.Write(m_isMedal);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreateClubDeskAndEnter();
	}
};

//////////////////////////////////////////////////////////////////////////
//加入桌子
struct LMsgLMG2LEnterClubDesk : public LMsg
{

	Lint            m_deskId;       //桌子号
	Lint            m_clubId;
	Lint            m_playTypeId;
	Lint            m_clubDeskId;

	//加入用户信息
	Lint			m_joinUserid;
	Lint			m_joinUserGateId;
	Lstring			m_joinUserStrUUID;
	Lstring			m_joinUserIp;
	LUser			m_joinUserUsert;
	Lint            m_joinUserFree;
	Lint            m_addDeskFlag;

	Lint			m_type;  //玩家加入桌子类型  3：从俱乐部匹配进入
	Lint			m_isMedal;	//玩家是否有勇士勋章

	LMsgLMG2LEnterClubDesk() :LMsg(MSG_LMG_2_L_ENTER_CLUB_DESK)
	{
		m_deskId=0;
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId = 0;

		m_joinUserid=0;
		m_joinUserGateId=0;
		m_joinUserFree=0;
		m_addDeskFlag = 0;

		m_type = 0;
		m_isMedal = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_deskId);
		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_clubDeskId);

		buff.Read(m_joinUserid);
		buff.Read(m_joinUserGateId);
		buff.Read(m_joinUserStrUUID);
		buff.Read(m_joinUserIp);
		buff.Read(m_joinUserFree);
		buff.Read(m_addDeskFlag);
		m_joinUserUsert.Read(buff);
		buff.Read(m_type);
		buff.Read(m_isMedal);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_deskId);
		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		buff.Write(m_clubDeskId);

		buff.Write(m_joinUserid);
		buff.Write(m_joinUserGateId);
		buff.Write(m_joinUserStrUUID);
		buff.Write(m_joinUserIp);
		buff.Write(m_joinUserFree);
		buff.Write(m_addDeskFlag);
		m_joinUserUsert.Write(buff);
		buff.Write(m_type);
		buff.Write(m_isMedal);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LEnterClubDesk();
	}
};

//////////////////////////////////////////////////////////////////////////
//用户加入俱乐部桌子成功
struct LMsgL2LMGUserAddClubDesk : public LMsg
{

	Lint     m_clubId;					//俱乐部ID
	Lint     m_playTypeId;				//俱乐部玩法ID
	Lint     m_clubDeskId;				//俱乐部桌子ID
	Lint     m_realDeskId;
	Lint     m_showDeskId;    
	Lstring  m_strUUID;					//玩家UnionID
	//Lint	 m_userId;					//玩家ID
	//Lint	 m_pos;                     //用户座位号 0-3 东南西北  无效INVAILD_POS
	Lint	 m_logicId;					//玩家LogicID
	Lint	 m_isClubOwerLooking;		//俱乐部会长是否正在明牌观战  0：不在明牌观战  1：会长在明牌观战中
	Lint m_type;                           // 0==入座    1==旁观
	struct deskUserInfo   m_userInfo;

	//Lint  m_userCount;
	//std::vector<Lint>  m_userIds;

	LMsgL2LMGUserAddClubDesk() :LMsg(MSG_L_2_LMG_USER_ADD_CLUB_DESK)
	{

		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_type = 0;
		//m_userId=0;
		//m_pos=INVAILD_POS;
		m_logicId = 0;
		m_isClubOwerLooking = 0;

	}
	virtual bool Read(LBuff& buff)
	{

		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_clubDeskId);
		buff.Read(m_strUUID);

		buff.Read(m_userInfo.m_userId);
		buff.Read(m_userInfo.m_pos);
		buff.Read(m_userInfo.m_deskPlayerName);
		buff.Read(m_userInfo.m_deskPlayerUrl);

		buff.Read(m_type);
		buff.Read(m_logicId);
		buff.Read(m_isClubOwerLooking);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{

		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		buff.Write(m_clubDeskId);
		buff.Write(m_strUUID);
	
		buff.Write(m_userInfo.m_userId);
		buff.Write(m_userInfo.m_pos);
		buff.Write(m_userInfo.m_deskPlayerName);
		buff.Write(m_userInfo.m_deskPlayerUrl);

		buff.Write(m_type);
		buff.Write(m_logicId);
		buff.Write(m_isClubOwerLooking);

	

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUserAddClubDesk();
	}
};

//////////////////////////////////////////////////////////////////////////
//用户离开俱乐部桌子成功
struct LMsgL2LMGUserLeaveClubDesk : public LMsg
{
	Lint     m_clubId;					//俱乐部ID
	Lint     m_playTypeId;				//俱乐部玩法ID
	Lint     m_clubDeskId;				//俱乐部7位桌号
	Lstring  m_strUUID;					//离开玩家的UnionId
	Lint	 m_userId;					//离开俱乐部玩家Id
	Lint	 m_isClubOwerLooking;		//俱乐部会长是否正在明牌观战  0：不在明牌观战  1：会长在明牌观战中

	LMsgL2LMGUserLeaveClubDesk() :LMsg(MSG_L_2_LMG_USER_LEAVE_CLUB_DESK)
	{
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_userId=0;
		m_isClubOwerLooking = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_clubDeskId);
		buff.Read(m_userId);
		buff.Read(m_strUUID);
		buff.Read(m_isClubOwerLooking);
		//for (Lint i=0; i < m_userCount; ++i)
		//{
		//	Lint userId;
		//	buff.Read(userId);
		//	m_userIds.push_back(userId);
		//}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		buff.Write(m_clubDeskId);
		buff.Write(m_userId);
		buff.Write(m_strUUID);
		buff.Write(m_isClubOwerLooking);
		//buff.Write(m_userCount);
		//
		//buff.Write(m_userCount);
		//for (Lint i=0; i < m_userCount; ++i)
		//{
		//	Lint  userId=m_userIds[i];
		//	buff.Write(userId);
		//}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUserLeaveClubDesk();
	}
};

//////////////////////////////////////////////////////////////////////////
//用户离开俱乐部桌子成功--多个用户
struct LMsgL2LMGUserListLeaveClubDesk : public LMsg
{
	Lint     m_clubId;
	Lint     m_playTypeId;
	Lint     m_clubDeskId;
	Lint     m_userCount;
	std::vector<Lint>  m_userIdList;
	std::vector<Lstring>  m_strUUIDList;
	

	LMsgL2LMGUserListLeaveClubDesk() :LMsg(MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK)
	{

		m_clubId = 0;
		m_playTypeId = 0;
		m_clubDeskId = 0;
		m_userCount = 0;
		
	}
	virtual bool Read(LBuff& buff)
	{

		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_clubDeskId);
		//buff.Read(m_userId);
		//buff.Read(m_strUUID);
		buff.Read(m_userCount);
		for (Lint i=0; i < m_userCount; ++i)
		{
			Lint userId;
			buff.Read(userId);
			m_userIdList.push_back(userId);
		}

		for (Lint i = 0; i < m_userCount; ++i)
		{
			Lstring  userUUID;
			buff.Read(userUUID);
			m_strUUIDList.push_back(userUUID);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{

		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		buff.Write(m_clubDeskId);
		//buff.Write(m_userId);
		//buff.Write(m_strUUID);
		//buff.Write(m_userCount);
		//
		buff.Write(m_userCount);
		for (Lint i=0; i < m_userCount; ++i)
		{
			Lint  userId= m_userIdList[i];
			buff.Write(userId);
		}
		for (Lint i = 0; i < m_userCount; ++i)
		{
			Lstring  userUUID = m_strUUIDList[i];
			buff.Write(userUUID);
		}


		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUserListLeaveClubDesk();
	}


};

//////////////////////////////////////////////////////////////////////////
//发送manager刷新桌子信息
struct LMsgL2LMGFreshDeskInfo : public LMsg
{
	Lint m_clubId;
	Lint m_playTypeId;
	Lint m_clubDeskId;       //7位房间号
	Lint m_showDeskId;       //展示房间号

	Lint m_roomFull;         //满员标志  0--房间未满  1--房间已满  （在满员后，有人退出房间，会发送0）
	Lint m_currCircle;       //当前局数  0--未开始游戏，不用展示；开始游戏后，实际局数
	Lint m_totalCircle;      //总局数，8 or 16

	LMsgL2LMGFreshDeskInfo() :LMsg(MSG_L_2_LMG_FRESH_DESK_INFO)
	{

		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_showDeskId=0;
		m_roomFull=0;
		m_currCircle=0;
		m_totalCircle=0;
	}
	virtual bool Read(LBuff& buff)
	{

		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_clubDeskId);
		buff.Read(m_showDeskId);
		buff.Read(m_roomFull);
		buff.Read(m_currCircle);
		buff.Read(m_totalCircle);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{

		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		buff.Write(m_clubDeskId);
		buff.Write(m_showDeskId);
		buff.Write(m_roomFull);
		buff.Write(m_currCircle);
		buff.Write(m_totalCircle);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGFreshDeskInfo();
	}


};

//////////////////////////////////////////////////////////////////////////
//重启，manager请求数据
struct LMsgLMG2LGRequestSynchroData :public LMsg
{
	Lint	m_ID;//类型
	Lstring m_key;
	Lstring m_ServerName;
	Lint	m_needLoadUserInfo;  //是否需要发送当前所有的玩家的信息 0:不发送 1：发送

	LMsgLMG2LGRequestSynchroData() :LMsg(MSG_LMG_2_L_G_REQUEST_SYNCHRO_DATA)
	{
		m_ID=0;
		m_needLoadUserInfo=0;
		m_ServerName.assign("LogicManager");
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_key);
		buff.Read(m_ServerName);
		buff.Read(m_needLoadUserInfo);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_key);
		buff.Write(m_ServerName);
		buff.Write(m_needLoadUserInfo);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LGRequestSynchroData();
	}
};

//////////////////////////////////////////////////////////////////////////
struct LogicUserInfo
{
	Lint        m_userId;
	//Lstring  m_unioid;
	Lint       	m_userState;				//玩家当前状态
	Lint        m_gateId;
	Lstring	m_ip;

	LogicUserInfo()
	{
		m_userId=0;
		m_userState=LGU_STATE_DESK;
		m_gateId=0;
	}

};
struct LogicDeskUserInfo
{
	Lint           m_userId;                    //用户id
	Lint           m_pos;                          //用户位置
	Lstring     m_headUrl;                 //用户头像
	Lstring     m_nike;                       //用户昵称
	Lstring     m_ip;                            //用户ip

	LogicDeskUserInfo()
	{
		m_userId=0;
		m_pos=INVAILD_POS;
	}

};
struct LogicDeskInfo
{
	Lint  m_deskId;
	
	//俱乐部信息
	Lint m_clubId;
	Lint m_playTypeId;
	Lint m_clubDeskId;
	Lint m_showDeskId;
	
	//桌子信息
	Lint m_roomFull;         //满员标志  0--房间未满  1--房间已满  （在满员后，有人退出房间，会发送0）
	Lint m_currCircle;       //当前局数  0--未开始游戏，不用展示；开始游戏后，实际局数
	Lint m_totalCircle;      //总局数，8 or 16
	Lint m_seatUserCount;
	std::vector<LogicDeskUserInfo>   m_seatUser;           //入座用户信息
	Lint m_noSeatUserCount;
	std::vector<LogicDeskUserInfo>   m_noSeatUser;     //未入座用户信息


	LogicDeskInfo()
	{
		m_deskId=0;

		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
		m_showDeskId=0;

		m_roomFull=0;
		m_currCircle=0;
		m_totalCircle=0;

	}
};

//logic 重连manager后同步数据
struct LMsgL2LMGSynchroData :public LMsg
{
	Lint	        m_logicID;					//logicId
	Lint        m_count;
	std::vector<LogicUserInfo> m_info;
	//Lint  	m_hasSentAll; //已经发送所有了的id。 0：没有 1：已经发送所有
	Lint       m_deskCount;
	std::vector<LogicDeskInfo> m_deskInfo;

	LMsgL2LMGSynchroData() :LMsg(MSG_L_2_LMG_SYNCHRO_DATA)
	{
		m_logicID=0;
		m_count=0;
		m_deskCount=0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_logicID);
		buff.Read(m_count);
		buff.Read(m_deskCount);

		for (Lint i=0; i<m_count; ++i)
		{
			LogicUserInfo info;
			buff.Read(info.m_userId);
			//buff.Read(info.m_unioid);
			buff.Read(info.m_userState);
			buff.Read(info.m_gateId);
			buff.Read(info.m_ip);
			m_info.push_back(info);
		}

		for (Lint i=0; i<m_deskCount; ++i)
		{
			LogicDeskInfo info;
			buff.Read(info.m_deskId);

			buff.Read(info.m_clubId);
			buff.Read(info.m_playTypeId);
			buff.Read(info.m_clubDeskId);
			buff.Read(info.m_showDeskId);

			buff.Read(info.m_roomFull);
			buff.Read(info.m_currCircle);
			buff.Read(info.m_totalCircle);

			buff.Read(info.m_seatUserCount);
			for (Lint j=0; j<info.m_seatUserCount; ++j)
			{
				LogicDeskUserInfo deskUserInfo;
				buff.Read(deskUserInfo.m_userId);
				buff.Read(deskUserInfo.m_pos);
				buff.Read(deskUserInfo.m_headUrl);
				buff.Read(deskUserInfo.m_nike);
				buff.Read(deskUserInfo.m_ip);
				info.m_seatUser.push_back(deskUserInfo);
			}

			buff.Read(info.m_noSeatUserCount);
			for (Lint j=0; j<info.m_noSeatUserCount; ++j)
			{
				LogicDeskUserInfo deskUserInfo;
				buff.Read(deskUserInfo.m_userId);
				buff.Read(deskUserInfo.m_pos);
				buff.Read(deskUserInfo.m_headUrl);
				buff.Read(deskUserInfo.m_nike);
				buff.Read(deskUserInfo.m_ip);
				info.m_noSeatUser.push_back(deskUserInfo);
			}
			
			m_deskInfo.push_back(info);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_logicID);
		m_count=m_info.size();
		buff.Write(m_count);
		m_deskCount=m_deskInfo.size();
		buff.Write(m_deskCount);

		for (Lint i=0; i < m_count; ++i)
		{
			buff.Write(m_info[i].m_userId);
			//buff.Write(m_info[i].m_unioid);
			buff.Write(m_info[i].m_userState);
			buff.Write(m_info[i].m_gateId);
			buff.Write(m_info[i].m_ip);
		}

		for (Lint i=0; i < m_deskCount; ++i)
		{
			buff.Write(m_deskInfo[i].m_deskId);

			buff.Write(m_deskInfo[i].m_clubId);
			buff.Write(m_deskInfo[i].m_playTypeId);
			buff.Write(m_deskInfo[i].m_clubDeskId);
			buff.Write(m_deskInfo[i].m_showDeskId);

			buff.Write(m_deskInfo[i].m_roomFull);
			buff.Write(m_deskInfo[i].m_currCircle);
			buff.Write(m_deskInfo[i].m_totalCircle);

			m_deskInfo[i].m_seatUserCount=m_deskInfo[i].m_seatUser.size();
			buff.Write(m_deskInfo[i].m_seatUserCount);

			for (Lint j=0; j<m_deskInfo[i].m_seatUserCount; ++j)
			{
				buff.Write(m_deskInfo[i].m_seatUser[j].m_userId);
				buff.Write(m_deskInfo[i].m_seatUser[j].m_pos);
				buff.Write(m_deskInfo[i].m_seatUser[j].m_headUrl);
				buff.Write(m_deskInfo[i].m_seatUser[j].m_nike);
				buff.Write(m_deskInfo[i].m_seatUser[j].m_ip);
			}

			m_deskInfo[i].m_noSeatUserCount=m_deskInfo[i].m_noSeatUser.size();
			buff.Write(m_deskInfo[i].m_noSeatUserCount);

			for (Lint j=0; j<m_deskInfo[i].m_noSeatUserCount; ++j)
			{
				buff.Write(m_deskInfo[i].m_noSeatUser[j].m_userId);
				buff.Write(m_deskInfo[i].m_noSeatUser[j].m_pos);
				buff.Write(m_deskInfo[i].m_noSeatUser[j].m_headUrl);
				buff.Write(m_deskInfo[i].m_noSeatUser[j].m_nike);
				buff.Write(m_deskInfo[i].m_noSeatUser[j].m_ip);
			}
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGSynchroData();
	}

	virtual Lint preAllocSendMemorySize() //重新分配64K,由于消息长度是short型
	{
		return 1024*64;
	}
};

//////////////////////////////////////////////////////////////////////////
struct GateUserInfo
{
	Lstring			m_strUUID;					//玩家的UUID
	Lint               m_userState;                //玩家状态
};

//gate 重连manager后同步数据
struct LMsgG2LMGSynchroData :public LMsg
{
	Lint	        m_gateID;					//gateId
	Lint        m_count;
	std::vector<GateUserInfo> m_info;


	LMsgG2LMGSynchroData() :LMsg(MSG_G_2_LMG_SYNCHRO_DATA)
	{
		m_gateID=0;
		m_count=0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gateID);
		buff.Read(m_count);
		
		for (Lint i=0; i<m_count; ++i)
		{
			GateUserInfo info;
			buff.Read(info.m_strUUID);
			m_info.push_back(info);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gateID);
		m_count=m_info.size();
		buff.Write(m_count);

		for (Lint i=0; i < m_count; ++i)
		{
			buff.Write(m_info[i].m_strUUID);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LMGSynchroData();
	}

	virtual Lint preAllocSendMemorySize() //重新分配64K,由于消息长度是short型
	{
		return 1024*64;
	}
};


//////////////////////////////////////////////////////////////////////////

struct LMsgLMG2CEUpdateData :public LMsg
{
	Lint	        m_clubId;					//gateId
	Lint        m_playTypeId;
	Lint        m_count;
	std::vector<ManagerClubDeskInfo> m_info;

	LMsgLMG2CEUpdateData() :LMsg(MSG_LMG_2_CE_UPDATE_DATA)
	{
		m_clubId=0;
		m_playTypeId=0;
		m_count=0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_count);

		for (Lint i=0; i<m_count; ++i)
		{
			ManagerClubDeskInfo info;
			buff.Read(info.m_clubDeskId);
			buff.Read(info.m_showDeskId);
			m_info.push_back(info);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		m_count=m_info.size();
		buff.Write(m_count);

		for (Lint i=0; i < m_count; ++i)
		{
			buff.Write(m_info[i].m_clubDeskId);
			buff.Write(m_info[i].m_showDeskId);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CEUpdateData();
	}
};

//////////////////////////////////////////////////////////////////////////
struct LMsgLMG2LDissmissClubDesk : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lint            m_clubId;
	Lint            m_playTypeId;
	Lint            m_clubDeskId;
	Lint            m_showDeskId;
	Lint            m_deskId;
	Lstring				m_strUUID;
	LUser			    m_usert;

	LMsgLMG2LDissmissClubDesk() :LMsg(MSG_LMG_2_L_DISSMISS_CLUB_DESK)
	{
		m_userid = 0;
		m_gateId = 0;
		m_clubId = 0;
		m_playTypeId = 0;;
		m_clubDeskId=0;
		m_showDeskId=0;
		m_deskId=0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);

		buff.Read(m_clubId);
		buff.Read(m_playTypeId);
		buff.Read(m_clubDeskId);
		buff.Read(m_showDeskId);

		buff.Read(m_deskId);
		buff.Read(m_strUUID);
		m_usert.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);

		buff.Write(m_clubId);
		buff.Write(m_playTypeId);
		buff.Write(m_clubDeskId);
		buff.Write(m_showDeskId);

		buff.Write(m_deskId);
		buff.Write(m_strUUID);
		m_usert.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDissmissClubDesk();
	}
};

//////////////////////////////////////////////////////////////////////////
struct LMsgLMG2LAPIDissmissRoom : public LMsg
{	
	Lint            m_deskId;
	Lint            m_type;  

	LMsgLMG2LAPIDissmissRoom() :LMsg(MSG_LMG_2_L_API_DISMISS_ROOM)
	{
		m_deskId = 0;
		m_type = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_deskId);
		buff.Read(m_type);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_deskId);
		buff.Write(m_type);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LAPIDissmissRoom();
	}
};

///////////////////////////////////////////////////////club--end/////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//查询房间 GPS信息
struct MHLMsgLMG2LQueryRoomGPSInfo :public LMsg
{
	Lint			m_userid;
	Lint			m_gateId;
	Lstring			m_strUUID;
	Lint			m_deskID;	
	LUser			m_usert;	

	MHLMsgLMG2LQueryRoomGPSInfo() :LMsg(MH_MSG_LMG_2_L_QUERY_ROOM_GPS_INFO)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);		
		m_usert.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);		
		m_usert.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new MHLMsgLMG2LQueryRoomGPSInfo();
	}
};

//////////////////////////////////////////////////////////////////////////
//db玩家战绩  扑克   db-manager
struct LMsgLDB2LPokerReqVideo :public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	QiPaiVideoLog	m_video;

	LMsgLDB2LPokerReqVideo() :LMsg(MSG_LDB_2_L_POKER_VIDEO)
	{
		m_userid = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		m_video.Read(buff);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		m_video.Write(buff);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LPokerReqVideo();
	}
};


///////////////////////////////////////////////////////
//center->manager->logic
struct LMsgCe2LMGControlMsg :public LMsg
{
	Lint m_type;
	Lint m_argv;

	LMsgCe2LMGControlMsg() :LMsg(MSG_CE_2_LMG_CONTROL_MSG)
	{
		m_type = 0;
		m_argv = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_argv);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_argv);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LMGControlMsg();
	}

};

/*
MSG_C_2_S_ADD_ROOM_SEAT_DOWN = 62066
观战玩家请求入座消息
*/
struct LMsgG2LLookOnUserSearDown : public LMsgSC
{
	Lint			m_userid;
	Lint			m_gateId;
	Lstring			m_strUUID;
	Lint			m_deskID;
	Lstring			m_ip;
	LUser			m_usert;
	Lint            m_creatDeskCost;
	Lint            m_Free;
	Lint            m_startTime;
	Lint            m_endTime;
	Lint            m_addDeskFlag;


	LMsgG2LLookOnUserSearDown() :LMsgSC(MSG_C_2_S_ADD_ROOM_SEAT_DOWN)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
		m_creatDeskCost = 0;
		m_Free = 0;
		m_startTime = 0;
		m_endTime = 0;
		m_addDeskFlag = 0;   // 0-正常加入 1-加入check
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kDeskId", m_deskID);
		/*buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		buff.Read(m_creatDeskCost);
		buff.Read(m_Free);
		buff.Read(m_startTime);
		buff.Read(m_endTime);
		buff.Read(m_addDeskFlag);
		m_usert.Read(buff);*/
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		/*buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		buff.Write(m_creatDeskCost);
		buff.Write(m_Free);
		buff.Write(m_startTime);
		buff.Write(m_endTime);
		buff.Write(m_addDeskFlag);
		m_usert.Write(buff);*/
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LLookOnUserSearDown();
	}
};

/*
  MSG_S_2_C_LOOKON_PLAYER_FULL = 62065
  服务器广播给观战玩家显示入座按钮信息
*/
struct LMsgS2CLookOnPlayerFill : public LMsgSC
{
	Lint  m_errorCode;		//错误代码    0：成功，1：游戏人数已满，2：不允许动态加入  3:玩家能量值够进入房间，从新显示入座按钮  4：游戏开始最后一局，让观战玩家隐藏入座按钮
	Lint  m_state;			//大玩法编号

	LMsgS2CLookOnPlayerFill() : LMsgSC(MSG_S_2_C_LOOKON_PLAYER_FULL)
	{
		m_errorCode = 0;
		m_state = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_state);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CLookOnPlayerFill();
	}
};

/*
MSG_S_2_C_ADD_ROOM_SEAT_DOWN = 62067
服务器返回观望玩家入座结果
*/
struct LMsgS2CLookOnUserSeatDownRet :public LMsgSC
{
	enum ErrorCode
	{
		Err_CreditNotEnough = 6,
		Err_GPS_INVALID = 9,
		Err_GPS_Limit = 10, //GPS房间，距离受限
		Err_Not_Dynamic_In = 11,  //不允许动态加入
		Err_Coins_Not_Enough = 12,  //能量值不够
		Err_Last_Circle_Not_Seat = 13,  //最后一局不允许入座
	};

	Lint		m_errorCode;//0-成功，1-人已满,2-房间不存在,3-未知错误 6-信用不够 4- 金币不足 5-进入俱乐部桌子（已创建过桌子，先解散）7-会长券不足 8-加入房间失败，请重试  9-GPS数据无效  10-GPS距离过近  11-不允许动态加入   12-玩家能量值不足  14--
	Lint		m_deskId;//桌子号码
	Lint		m_pos;	//玩家入座位置
	Lstring     m_userGPSList;
	std::vector<Lstring>  m_deskSeatPlayerName;
	std::vector<Lstring>  m_deskNoSeatPlayerName;


	LMsgS2CLookOnUserSeatDownRet() :LMsgSC(MSG_S_2_C_ADD_ROOM_SEAT_DOWN)
	{
		m_errorCode = -1;
		m_deskId = 0;
		m_pos = INVAILD_POS_QIPAI;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_deskSeatPlayerName), m_deskSeatPlayerName);
		ReadMapData(obj, NAME_TO_STR(m_deskNoSeatPlayerName), m_deskNoSeatPlayerName);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kUserGPSList), m_userGPSList);
		WriteKeyValue(pack, NAME_TO_STR(kDeskSeatPlayerName), m_deskSeatPlayerName);
		WriteKeyValue(pack, NAME_TO_STR(kDeskNoSeatPlayerName), m_deskNoSeatPlayerName);
		/*WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);*/
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CLookOnUserSeatDownRet(); }
};

/*
MSG_C_2_S_VIEW_LOOKON_LIST = 62068
客户端请求查看观战玩家列表
*/
struct LMsgC2SViewLookOnList : public LMsgSC
{
	Lint	m_pos;		//玩家位置

	LMsgC2SViewLookOnList() : LMsgSC(MSG_C_2_S_VIEW_LOOKON_LIST)
	{
		m_pos = INVAILD_POS_QIPAI;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SViewLookOnList();
	}
};

/*
MSG_S_2_C_RECT_LOOKON_LIST = 62069
服务器返回观战玩家列表
*/
struct LMsgS2CRectLookOnList : public LMsgSC
{
	Lint m_lookOnCount;
	std::vector<Lint> m_lookOnId;
	std::vector<Lstring> m_lookOnHead;
	std::vector<Lstring> m_lookOnNike;

	LMsgS2CRectLookOnList() : LMsgSC(MSG_S_2_C_RECT_LOOKON_LIST)
	{
		m_lookOnCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kLookOnCount", m_lookOnCount);
		WriteKeyValue(pack, "kLookOnId", m_lookOnId);
		WriteKeyValue(pack, "kLookOnHead", m_lookOnHead);
		WriteKeyValue(pack, "kLookOnNike", m_lookOnNike);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SViewLookOnList();
	}
};

/*
MSG_S_2_C_SHOW_LOOKON_COUNT = 62070
服务器返回观战玩家数量
*/
struct LMsgS2CShowLookOnCount : public LMsgSC
{
	Lint m_lookOnCount;

	LMsgS2CShowLookOnCount() : LMsgSC(MSG_S_2_C_SHOW_LOOKON_COUNT)
	{
		m_lookOnCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kLookOnCount", m_lookOnCount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CShowLookOnCount();
	}
};


/*
  服务器更新客户端玩家金币数量
  MSG_S_2_C_UPDATE_CONINS = 62071,			
*/
struct LMsgS2CUpdateCoins : public LMsgSC
{
	Lint m_userId;
	Lint m_pos;
	Lint m_coins;

	LMsgS2CUpdateCoins() : LMsgSC(MSG_S_2_C_UPDATE_CONINS)
	{
		m_userId = 0;
		m_pos = INVAILD_POS_QIPAI;
		m_coins = 0;
	}
	
	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kUserId", m_userId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kCoins", m_coins);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CUpdateCoins();
	}
};


//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LMGDelUserFromManager :public LMsg
{
	Lint m_userId;


	LMsgCe2LMGDelUserFromManager() :LMsg(MSG_CE_2_LMG_DEL_USER)
	{
	}

};


///////////////////////////////////////////////////////
//manager->center
struct LMsgLMG2CeSqlInfo :public LMsg
{
	Lint    m_which;            // 1--club   2-center
	Lint		m_type;             //类型,0-插入,1-更新
	Lint    m_reply;           // 0-不需要回复结果  1-需要回复结果
	Lint    m_msgId;         //消息号
	Lstring		m_sql;
	Lint   m_userId;         //用户id
	Lstring   m_strUUID;      

	
	LMsgLMG2CeSqlInfo() :LMsg(MSG_CE_2_LMG_SQL_INFO)
	{
		m_type = 0;
		m_which = 1;
		m_userId = 0;
		m_reply = 0;
		m_msgId = 0;
	}

	virtual bool Read(LBuff& buff)
	{

		buff.Read(m_which);
		buff.Read(m_type);
		buff.Read(m_reply);
		buff.Read(m_msgId);
		buff.Read(m_sql);
		buff.Read(m_userId);
		buff.Read(m_strUUID);
	
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_which);
		buff.Write(m_type);
		buff.Write(m_reply);
		buff.Write(m_msgId);
		buff.Write(m_sql);
		buff.Write(m_userId);
		buff.Write(m_strUUID);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CeSqlInfo();
	}

};

//////////////////////////////////////////////////////////////////////////
////特殊活动需要判断
enum SPEC_ACTIVE_TYPE
{
	SPEC_ACTIVE_PAIXING = 1,                         //特殊牌型  ，暂时不用
	SPEC_ACTIVE_CLUB_RED_PACKET = 2,   //俱乐部开局红包的活动
	SPEC_ACTIVE_YINGSANZHANGPAI_CONFIG=3,  //三张牌特殊配置

};

//特殊活动的结果
struct SpecActiveItem
{
	Lint  m_id;
	Lint m_activeType;
	Lint m_gameType;
	Lint m_gameState;
	Lint m_limitFlag;
	Lint m_beginTime;
	Lint m_endTime;
	Lint m_otherInt1;
	Lint m_otherInt2;
	Lint m_otherInt3;
	Lint m_otherInt4;
	Lint m_otherInt5;
	Lstring  m_otherData1;
	Lstring  m_otherData2;

	SpecActiveItem()
	{
		m_id = 0;
		m_activeType = 0;
		m_gameType = 0;
		m_gameState = 0;
		m_limitFlag = 0;
		m_beginTime = 0;
		m_endTime = 0;
		m_otherInt1 = 0;
		m_otherInt2 = 0;
		m_otherInt3 = 0;
		m_otherInt4 = 0;
		m_otherInt5 = 0;

	}

	SpecActiveItem(boost::shared_ptr<SpecActiveItem> c)
	{
		update(c);
	}
	void update(boost::shared_ptr<SpecActiveItem> c)
	{
		if (c != NULL)
		{
			m_id = c->m_id;
			m_activeType = c->m_activeType;
			m_gameType = c->m_gameType;
			m_gameState = c->m_gameState;
			m_limitFlag = c->m_limitFlag;
			m_beginTime = c->m_beginTime;
			m_endTime = c->m_endTime;
			m_otherInt1 = c->m_otherInt1;
			m_otherInt2 = c->m_otherInt2;
			m_otherInt3 = c->m_otherInt3;
			m_otherInt4 = c->m_otherInt4;
			m_otherInt5 = c->m_otherInt5;
			m_otherData1 = c->m_otherData1;
			m_otherData2 = c->m_otherData2;
			//LLOG_ERROR("UPDATE data1[%s]", m_otherData1.c_str());
		}
	}

};

struct LMsgLMG2LSpecActive :public LMsg
{
	Lint m_count;
	std::vector<SpecActiveItem>  m_ActiveList;

	LMsgLMG2LSpecActive() :LMsg(MSG_LMG_2_L_SPEC_ACTIVE)
	{
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_count);
		for (Lint i = 0; i < m_count; ++i)
		{
			SpecActiveItem Active;
			buff.Read(Active.m_id);
			buff.Read(Active.m_activeType);
			buff.Read(Active.m_gameType);
			buff.Read(Active.m_gameState);
			buff.Read(Active.m_limitFlag);
			buff.Read(Active.m_beginTime);
			buff.Read(Active.m_endTime);
			buff.Read(Active.m_otherInt1);
			buff.Read(Active.m_otherInt2);
			buff.Read(Active.m_otherInt3);
			buff.Read(Active.m_otherInt4);
			buff.Read(Active.m_otherInt5);

			buff.Read(Active.m_otherData1);
			buff.Read(Active.m_otherData2);


			m_ActiveList.push_back(Active);

		}
		return true;
	}


	virtual bool Write(LBuff& buff)
	{
		m_count = m_ActiveList.size();
		buff.Write(m_count);
		for (Lint i = 0; i < m_count; ++i)
		{
			buff.Write(m_ActiveList[i].m_id);
			buff.Write(m_ActiveList[i].m_activeType);
			buff.Write(m_ActiveList[i].m_gameType);
			buff.Write(m_ActiveList[i].m_gameState);
			buff.Write(m_ActiveList[i].m_limitFlag);
			buff.Write(m_ActiveList[i].m_beginTime);
			buff.Write(m_ActiveList[i].m_endTime);
			buff.Write(m_ActiveList[i].m_otherInt1);
			buff.Write(m_ActiveList[i].m_otherInt2);
			buff.Write(m_ActiveList[i].m_otherInt3);
			buff.Write(m_ActiveList[i].m_otherInt4);
			buff.Write(m_ActiveList[i].m_otherInt5);
			buff.Write(m_ActiveList[i].m_otherData1);
			buff.Write(m_ActiveList[i].m_otherData2);

		}
		return true;
	}


	virtual LMsg* Clone()
	{
		return new LMsgLMG2LSpecActive();
	}

};

///////////////////////////////////////////////////////
//logic->manager->center
struct LMsgL2CeClubFirstRedPacket :public LMsg
{
	Lint        m_time;
	Lint        m_userId;          //用户id
	Lstring  m_unioid;         //用户uniond
	Lint        m_clubId;         //俱乐部Id
	Lint        m_showDesk;       //俱乐部桌子
	Lint        m_redPacketVale;   //红包金额

	
	LMsgL2CeClubFirstRedPacket() :LMsg(MSG_L_2_CE_CLUB_FIRST_RED_PACKET)
	{
		m_time = 0;
		m_userId = 0;
		m_clubId = 0;
		m_showDesk = 0;
		m_redPacketVale = 0;
	}

	virtual bool Read(LBuff& buff)
	{

		buff.Read(m_time);
		buff.Read(m_userId);
		buff.Read(m_unioid);
		buff.Read(m_clubId);
		buff.Read(m_showDesk);
		buff.Read(m_redPacketVale);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_time);
		buff.Write(m_userId);
		buff.Write(m_unioid);
		buff.Write(m_clubId);
		buff.Write(m_showDesk);
		buff.Write(m_redPacketVale);
	
		return true;
	}

	virtual LMsg* Clone()
	{

		return new LMsgL2CeClubFirstRedPacket();
	}

};

///////////////////////////////////////////////////////
//manager->center->manager 能量记录
struct LMsgLMG2CEUpdatePointRecord :public LMsg
{
	Lint m_msgType;                         // 0:manager->center   1:center->manager
	Lint m_errorCode;                     // 0: 成功   1:失败
	struct UserPowerRecord  m_record;

	LMsgLMG2CEUpdatePointRecord() :LMsg(MSG_CE_2_LMG_UPDATE_POINT_RECORD)
	{
		m_msgType = 0;
		m_errorCode = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_msgType);
		buff.Read(m_errorCode);
		buff.Read(m_record.m_time);
		buff.Read(m_record.m_userId);
		buff.Read(m_record.m_nike);
		buff.Read(m_record.m_photo);
		buff.Read(m_record.m_type);
		buff.Read(m_record.m_point);
		buff.Read(m_record.m_crruPoint);
		buff.Read(m_record.m_operId);
		buff.Read(m_record.m_operNike);
		buff.Read(m_record.m_operPhoto);
		buff.Read(m_record.m_operR);
		buff.Read(m_record.m_clubId);
		buff.Read(m_record.m_clubSq);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_msgType);
		buff.Write(m_errorCode);
		buff.Write(m_record.m_time);
		buff.Write(m_record.m_userId);
		buff.Write(m_record.m_nike);
		buff.Write(m_record.m_photo);
		buff.Write(m_record.m_type);
		buff.Write(m_record.m_point);
		buff.Write(m_record.m_crruPoint);
		buff.Write(m_record.m_operId);
		buff.Write(m_record.m_operNike);
		buff.Write(m_record.m_operPhoto);
		buff.Write(m_record.m_operR);
		buff.Write(m_record.m_clubId);
		buff.Write(m_record.m_clubSq);
		return true;
	}

	virtual LMsg* Clone()
	{

		return new LMsgLMG2CEUpdatePointRecord();
	}
};

/*
manager更新logic上玩家的能量值
MSG_LMG_2_L_UPDATE_POINT_COINTS = 11044
*/
struct LMsgLMG2LUpdatePointCoins : public LMsg
{
	Lint		m_userId;
	Lint		m_changeCoins;
	Lint		m_currCoins;
	Lint		m_clubId;

	LMsgLMG2LUpdatePointCoins() : LMsg(MSG_LMG_2_L_UPDATE_POINT_COINTS)
	{
		m_userId = 0;
		m_changeCoins = 0;
		m_currCoins = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_changeCoins);
		buff.Read(m_currCoins);
		buff.Read(m_clubId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_changeCoins);
		buff.Write(m_currCoins);
		buff.Write(m_clubId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LUpdatePointCoins();
	}
};

/*
  manager更新desk上的m_managerList列表
  MSG_LMG_2_L_UPDATE_MANAGER_LIST = 11045
*/
struct LMsgLMG2LUpdateManagerList : public LMsg
{
	Lint m_clubId;
	Lint m_managerListCount;
	std::vector<Lint> m_managerList;		//俱乐部中管理员列表

	LMsgLMG2LUpdateManagerList() : LMsg(MSG_LMG_2_L_UPDATE_MANAGER_LIST)
	{
		m_clubId = 0;
		m_managerListCount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_clubId);
		buff.Read(m_managerListCount);
		for (int i = 0; i < m_managerListCount; ++i)
		{
			Lint t_manager;
			buff.Read(t_manager);
			m_managerList.push_back(t_manager);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_clubId);
		buff.Write(m_managerListCount);
		for (int i = 0; i < m_managerListCount; ++i)
		{
			buff.Write(m_managerList[i]);
		}
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LUpdatePointCoins();
	}

};
/*
  manager 发往center 查询能力值
*/
struct LMsgLMG2CERecordLog : public LMsg
{
	Lint m_mode;             //   3-俱乐部所有   2-操作人   1-被操作人
	Lint m_clubId;
	Lint m_clubSq;
	Lint m_userId;			//请求人Id
	Lint m_type;
	Lint m_time;

	Lint m_searchCount;
	std::vector<Lint> m_search;

	LMsgLMG2CERecordLog() : LMsg(MSG_LMG_2_CE_POINT_RECORD_LOG)
	{
		m_clubId = 0;
		m_mode = 0;
		m_clubSq = 0;
		m_searchCount = 0;
		m_userId = 0;
		m_type = 0;
		m_time = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_mode);
		buff.Read(m_clubId);
		buff.Read(m_clubSq);
		buff.Read(m_userId);
		buff.Read(m_type);
		buff.Read(m_time);
		buff.Read(m_searchCount);
		for (Lint i = 0; i < m_searchCount; ++i)
		{
			Lint userId;
			buff.Read(userId);
			m_search.push_back(userId);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_mode);
		buff.Write(m_clubId);
		buff.Write(m_clubSq);
		buff.Write(m_userId);
		buff.Write(m_type);
		buff.Write(m_time);
		buff.Write(m_searchCount);
		for (Lint i = 0; i < m_searchCount; ++i)
		{
			Lint userId = m_search[i];
			buff.Write(userId);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CERecordLog();
	}
};

struct LMsgCE2LMGRecordLog : public LMsg
{
	Lint			m_errorCode;    //错误码  0-成功 1-失败
	Lint            m_clubId;
	Lint            m_clubSq;
	Lint            m_type;
	Lint            m_mode;
	Lint            m_userId;              //查询人的ID
	Lint            m_HasSendAll;
	Lint            m_count;
	std::vector<UserPowerRecord>  m_List;

	Lint			m_searchCount;
	std::vector<Lint> m_search;

	LMsgCE2LMGRecordLog() : LMsg(MSG_CE_2_LMG_POINT_RECORD_LOG)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_clubSq = 0;
		m_type = 0;
		m_mode = 0;
		m_searchCount = 0;
		m_userId = 0;
		m_HasSendAll = 0;
		m_count = 0;

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_errorCode);
		buff.Read(m_clubId);
		buff.Read(m_clubSq);
		buff.Read(m_type);
		buff.Read(m_mode);
		buff.Read(m_userId);
		buff.Read(m_HasSendAll);
		buff.Read(m_count);

		for (Lint i = 0; i < m_count ;++i)
		{
			UserPowerRecord m_record;
			buff.Read(m_record.m_time);
			buff.Read(m_record.m_userId);
			buff.Read(m_record.m_nike);
			buff.Read(m_record.m_photo);
			buff.Read(m_record.m_type);
			buff.Read(m_record.m_point);
			buff.Read(m_record.m_crruPoint);
			buff.Read(m_record.m_operId);
			buff.Read(m_record.m_operNike);
			buff.Read(m_record.m_operPhoto);
			buff.Read(m_record.m_operR);
			buff.Read(m_record.m_clubId);
			buff.Read(m_record.m_clubSq);
			m_List.push_back(m_record);
		}
		buff.Read(m_searchCount);
		for (Lint i = 0; i < m_searchCount; ++i)
		{
			Lint userId;
			buff.Read(userId);
			m_search.push_back(userId);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{

		buff.Write(m_errorCode);
		buff.Write(m_clubId);
		buff.Write(m_clubSq);
		buff.Write(m_type);
		buff.Write(m_mode);
		buff.Write(m_userId);
		buff.Write(m_HasSendAll);
		buff.Write(m_count);

		for (Lint i = 0; i < m_count; ++i)
		{
			buff.Write(m_List[i].m_time);
			buff.Write(m_List[i].m_userId);
			buff.Write(m_List[i].m_nike);
			buff.Write(m_List[i].m_photo);
			buff.Write(m_List[i].m_type);
			buff.Write(m_List[i].m_point);
			buff.Write(m_List[i].m_crruPoint);
			buff.Write(m_List[i].m_operId);
			buff.Write(m_List[i].m_operNike);
			buff.Write(m_List[i].m_operPhoto);
			buff.Write(m_List[i].m_operR);
			buff.Write(m_List[i].m_clubId);
			buff.Write(m_List[i].m_clubSq);
		}
		buff.Write(m_searchCount);
		for (Lint i = 0; i < m_searchCount; ++i)
		{
			Lint userId = m_search[i];
			buff.Write(userId);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGRecordLog();
	}
};

//更新俱乐部内容
struct LMsgCE2LMGClubInfo : public LMsg
{
	Lint				m_errorCode;    //错误码  0-成功 1-失败
	Lint            m_msgType;
	Lint            m_operId;          //操作人的id
	Lint            m_clubId;
	Lint            m_clubSq;
	Lint m_clubType;                     //类型，0-普通   1-竞技场
	Lint m_adminCanChange;     // 管理员可以调整能量   0-没有选择  1-选择
	Lint m_adminCanSeeAll;        //管理员可以查询所有能量变化原因  0-没有  1-选择
	Lint m_tableCount;        //桌子个数

	LMsgCE2LMGClubInfo() : LMsg(MSG_CE_2_LMG_UPDATE_CLUB)
	{
		m_errorCode = 0;
		m_msgType = 0;
		m_operId = 0;
		m_clubId = 0;
		m_clubSq = 0;
		m_clubType = 0;
		m_adminCanChange = 0;
		m_adminCanSeeAll = 0;
		m_tableCount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_errorCode);
		buff.Read(m_msgType);
		buff.Read(m_operId);
		buff.Read(m_clubId);
		buff.Read(m_clubSq);
		buff.Read(m_clubType);
		buff.Read(m_adminCanChange);
		buff.Read(m_adminCanSeeAll);
		buff.Read(m_tableCount);
	
		return true;
	}

	virtual bool Write(LBuff& buff)
	{

		buff.Write(m_errorCode);
		buff.Write(m_msgType);
		buff.Write(m_operId);
		buff.Write(m_clubId);
		buff.Write(m_clubSq);
		buff.Write(m_clubType);
		buff.Write(m_adminCanChange);
		buff.Write(m_adminCanSeeAll);
		buff.Write(m_tableCount);
	
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGClubInfo();
	}

};


//更新俱乐部玩法
struct LMsgCE2LMGPlayInfo : public LMsg
{
	Lint				m_errorCode;    //错误码  0-成功 1-失败
	Lint            m_msgType;
	Lint            m_operId;
	Lint            m_clubId;
	Lint            m_clubSq;
	Lint  m_minPointInRoom;             //加入游戏最小值
	Lint  m_minPointXiaZhu;             //最小下注值
	Lint  m_smallZero;                                   //是否可负分   0 不可以  1  可以

	LMsgCE2LMGPlayInfo() : LMsg(MSG_CE_2_LMG_UPDATE_PLAY)
	{
		m_errorCode = 0;
		m_msgType = 0;
		m_operId = 0;
		m_clubId = 0;
		m_clubSq = 0;
		m_minPointInRoom = 0;
		m_minPointXiaZhu = 0;
		m_smallZero = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_errorCode);
		buff.Read(m_msgType);
		buff.Read(m_clubId);
		buff.Read(m_operId);
		buff.Read(m_clubSq);
		buff.Read(m_minPointInRoom);
		buff.Read(m_minPointXiaZhu);
		buff.Read(m_smallZero);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{

		buff.Write(m_errorCode);
		buff.Write(m_msgType);
		buff.Write(m_clubId);
		buff.Write(m_operId);
		buff.Write(m_clubSq);
		buff.Write(m_minPointInRoom);
		buff.Write(m_minPointXiaZhu);
		buff.Write(m_smallZero);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGPlayInfo();
	}

};

#endif