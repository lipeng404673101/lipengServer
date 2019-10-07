
#include "mjConfig.h"
#include "config.h"
bool mjConfig::Init()
{
	return true;
}

bool mjConfig::Final()
{
	return true;
}


bool   mjConfig::GetCommonConfigValue(std::string name, Lint& nValue)
{
	auto itor = m_configList.begin();
	itor = m_configList.find(name);
	if (itor != m_configList.end()) {
		nValue = itor->second->m_value;
		return true;
	}
	return false;
}

bool   mjConfig::GetFeeTypeValue(Lint id, deskFeeItem& feeItem, Lint game_state)
{
	if (game_state > 1000)   //棋牌的游戏类型都是3位数
	{
		auto itor = m_feeList.begin();
		itor = m_feeList.find(id);
		if (itor != m_feeList.end()) {
			feeItem.m_id = itor->second->m_id;
			feeItem.m_name = itor->second->m_name;
			feeItem.m_round = itor->second->m_round;
			feeItem.m_cost = itor->second->m_cost;
			feeItem.m_even = itor->second->m_even;
			feeItem.m_roomCost = itor->second->m_roomCost;
			LLOG_DEBUG("GetFeeTypeValue  game_state[%d] fee cost =[%d], evet=[%d]", game_state, feeItem.m_cost, feeItem.m_even);
			return true;
		}
	}
	else
	{
		for(auto itor = m_feeList.begin(); itor!=m_feeList.end();itor++)
		{ 
			LLOG_DEBUG("GetFeeTypeValue id[%d]=[%d]",itor->second->m_id, atoi(itor->second->m_name.c_str()));
			if (game_state == atoi(itor->second->m_name.c_str()) && id == itor->second->m_round)
			{
				feeItem.m_id = itor->second->m_id;
				feeItem.m_name = itor->second->m_name;
				feeItem.m_round = itor->second->m_round;
				feeItem.m_cost = itor->second->m_cost;
				feeItem.m_even = itor->second->m_even;
				feeItem.m_roomCost = itor->second->m_roomCost;
				LLOG_DEBUG("GetFeeTypeValue  game_state[%d] fee cost =[%d], evet=[%d]", game_state,feeItem.m_cost, feeItem.m_even);
				return true;
			}
		}

	}
	LLOG_ERROR("GetFeeTypeValue error,id=[%d],game_state=[%d]", id, game_state);
	return false;
}

bool mjConfig::UpdateConfigFromCenter(LMsgCE2LMGConfig* msg)
{
	for (int i = 0; i < msg->m_commonConList.size(); i++) 
	{
		auto itor = m_configList.find(msg->m_commonConList[i].m_name);
		if (itor != m_configList.end()) {
			commonItem* temp = itor->second;
			temp->m_id = msg->m_commonConList[i].m_id;
			temp->m_name = msg->m_commonConList[i].m_name;
			temp->m_value = msg->m_commonConList[i].m_value;
			//LLOG_DEBUG("UpdateConfigFromCenter  temp->m_value = %d", temp->m_value);
			continue;
		}

		commonItem* pCommonItem = new commonItem();
		pCommonItem->m_id = msg->m_commonConList[i].m_id;
		pCommonItem->m_name = msg->m_commonConList[i].m_name;
		pCommonItem->m_value = msg->m_commonConList[i].m_value;
		m_configList.insert(std::map<std::string, commonItem*>::value_type(pCommonItem->m_name, pCommonItem));
	}



	for (int i = 0; i < msg->m_deskFeeList.size(); i++) {
		auto itor = m_feeList.find(msg->m_deskFeeList[i].m_id);
		if (itor != m_feeList.end()) {
			auto itor = m_feeList.find(msg->m_deskFeeList[i].m_id);
			if (itor != m_feeList.end()) {
				deskFeeItem* temp = itor->second;
				temp->m_id = msg->m_deskFeeList[i].m_id;
				temp->m_name = msg->m_deskFeeList[i].m_name;
				temp->m_round = msg->m_deskFeeList[i].m_round;
				temp->m_cost = msg->m_deskFeeList[i].m_cost;
				temp->m_even = msg->m_deskFeeList[i].m_even;
				temp->m_roomCost = msg->m_deskFeeList[i].m_roomCost;
				continue;
			}
		}

		deskFeeItem* pDeskFeeItem = new deskFeeItem();
		pDeskFeeItem->m_id = msg->m_deskFeeList[i].m_id;
		pDeskFeeItem->m_name = msg->m_deskFeeList[i].m_name;
		pDeskFeeItem->m_round = msg->m_deskFeeList[i].m_round;
		pDeskFeeItem->m_cost = msg->m_deskFeeList[i].m_cost;
		pDeskFeeItem->m_even = msg->m_deskFeeList[i].m_even;
		pDeskFeeItem->m_roomCost = msg->m_deskFeeList[i].m_roomCost;
		//LLOG_ERROR("UpdateConfigFromCenter  pDeskFeeItem->m_name = %s  pDeskFeeItem->m_round = %d  pDeskFeeItem->m_cost = %d  pDeskFeeItem->m_even = %d,m_roomCost[%d]", pDeskFeeItem->m_name.c_str(), 
		//	pDeskFeeItem->m_round, pDeskFeeItem->m_cost, pDeskFeeItem->m_even, pDeskFeeItem->m_roomCost);
		m_feeList.insert(std::map<int, deskFeeItem*>::value_type(pDeskFeeItem->m_id, pDeskFeeItem));
	}

	return true;
}
