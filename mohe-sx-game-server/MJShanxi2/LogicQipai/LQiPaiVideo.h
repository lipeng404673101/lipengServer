#ifndef _L_QIPAI_VIDEO_H_
#define _L_QIPAI_VIDEO_H_

#pragma once

//#include "LTool.h"
//#include "LBuff.h"
#include "GameDefine.h"
//#include "./mhgamedef.h"
#include "LMsgPoker.h"

#define QIPAI_VIDEO_USER_MAX   3

class  QiPaiVideoLog:public QiPaiVideoLogBase
{
public:
	QiPaiVideoLog();
	virtual~QiPaiVideoLog();
	void		AddOper(Lint oper, Lint pos, std::vector<BYTE>& cards);
	
	void       FillDeskBaseInfo(Lint deskId, Lint flag, Lint max, const std::vector<Lint>& playType);

	void		DealCard(Lint cur, Lint userCount,Lint* userId, std::vector<BYTE>* vec, std::vector<BYTE> &retCard, Lint time, Lint zhuang);

	void      addUserScore(Lint * score);

	Lstring		HandCardToString();
	Lstring	    RestCardToString();
	
	Lstring		ToString();
	//void		FromString(Lstring str);
	//
	//void		Write(LBuff& buff);
	//
	//void		Read(LBuff& buff);
	//
	void		Clear();

	Lstring		GetInsertSql();

	Lstring PlayTypeToStrint();

	//Lstring		PlayTypeToStrint();

	//void		PlayTypeFromString(const Lstring& str);
//public:
//	Lint							m_time;
//	Lstring							m_Id;
//	Lint							m_zhuang;
//	Lint							m_deskId;
//	Lint							m_curCircle;
//	Lint							m_maxCircle;
//	Lint							m_score[QIPAI_VIDEO_USER_MAX];
//	Lint							m_userId[QIPAI_VIDEO_USER_MAX];
//	Lint							m_flag;//牌局玩法
//
//	std::vector<BYTE>			    m_handCard[QIPAI_VIDEO_USER_MAX];
//	std::vector<QiPaiVideoOper>			m_oper;
//	std::vector<Lint>				                    m_playType;
//
//	//Lstring							m_str;
//	Lstring							m_str_playtype;
//	//Lstring                          m_sqlString;
//	//斗地主底牌
//	std::vector<BYTE>                               m_restCard;

};



QiPaiVideoLog::QiPaiVideoLog()
{
	m_time = 0;
	m_zhuang = 0;
	m_deskId = 0;
	m_curCircle = 0;
	m_maxCircle = 0;
	memset(m_score, 0x00, sizeof(m_score));
	memset(m_userId, 0x00, sizeof(m_userId));

	m_flag = 0;

}

QiPaiVideoLog::~QiPaiVideoLog()
{
}

void QiPaiVideoLog::Clear()
{
	m_Id.clear();
	m_zhuang = 0;
	m_curCircle = 0;
}

Lstring	QiPaiVideoLog::HandCardToString()
{
	std::stringstream ss;
	for (size_t i = 0; i< QIPAI_VIDEO_USER_MAX; i++)
	{
		for (int j = 0; j < m_handCard[i].size(); j++)
		{
			ss << m_handCard[i][j];
			if (j< m_handCard[i].size() - 1)
			{
				ss << ",";
			}
		}
		ss << ";";
	}

	return ss.str();
}

Lstring	QiPaiVideoLog::RestCardToString()
{
	std::stringstream ss;

	for (int j = 0; j < m_restCard.size(); j++)
	{
		ss << m_restCard[j];
		if (j< m_restCard.size() - 1)
		{
			ss << ",";
		}
	}
	ss << ";";
	return ss.str();
}

Lstring QiPaiVideoLog::ToString()
{
	std::stringstream ss;
	ss << HandCardToString() << "|";
	for (int i = 0; i < m_oper.size(); i++)
	{
		ss << m_oper[i].ToString() << "|";
	}

	return ss.str();
}

Lstring QiPaiVideoLog::PlayTypeToStrint()
{
	std::stringstream ss;
	for (Lsize i = 0; i < m_playType.size(); ++i)
	{
		ss << m_playType[i] << ";";
	}
	return ss.str();
}

Lstring QiPaiVideoLog::GetInsertSql()
{
	//LTime t;
	std::stringstream ss;
	ss << "INSERT INTO";
	ss << " video ";
	ss << "(Id, UserId1, Time, UserId2, UserId3, UserId4, Data, DeskId, Flag, CurCircle, MaxCircle, Score1, Score2, Score3, Score4, Zhuang, PlayType) VALUES (";
	ss << "'" << m_Id << "',";
	ss << "'" << m_userId[0] << "',";
	ss << "'" << 0 << "',";
	for (int i = 1; i < QIPAI_VIDEO_USER_MAX; i++)
	{
		ss << "'" << m_userId[i] << "',";
	}
	ss << "'" << ToString() << " ',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_flag << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_maxCircle << "',";
	for (int i = 0; i < QIPAI_VIDEO_USER_MAX; i++)
	{
		ss << "'" << m_score[i] << "',";
	}
	ss << "'" << m_zhuang << "',";
	ss << "'" << PlayTypeToStrint() << "')";

	return ss.str();
}

void QiPaiVideoLog::AddOper(Lint oper, Lint pos, std::vector<BYTE>& cards)
{
	QiPaiVideoOper  tempQPVideoOper;
	tempQPVideoOper.m_pos = pos;
	tempQPVideoOper.m_oper = oper;
	tempQPVideoOper.m_cards = cards;

	m_oper.push_back(tempQPVideoOper);

}

void QiPaiVideoLog::FillDeskBaseInfo(Lint deskId, Lint flag, Lint max, const std::vector<Lint>& playType)
{
	m_deskId = deskId;
	//m_curCircle = cur;
	m_maxCircle = max;
	m_flag = flag;
	m_playType = playType;
	m_str_playtype = PlayTypeToStrint();

}

void QiPaiVideoLog::DealCard(Lint cur, Lint userCount, Lint* userId, std::vector<BYTE>* vec, std::vector<BYTE> &retCard, Lint time, Lint zhuang)
{
	m_curCircle = cur;
	Lint userNum = QIPAI_VIDEO_USER_MAX;
	userNum = userNum > userCount ? userCount : userNum;
	for (int i = 0; i < userNum; i++)
	{
		m_userId[i] = userId[i];
		m_handCard[i] = vec[i];
	}
	m_restCard = retCard;
	m_zhuang = zhuang;
}

void QiPaiVideoLog::addUserScore(Lint * score)
{
	memcpy(m_score, score, sizeof(m_score));
}





#endif
