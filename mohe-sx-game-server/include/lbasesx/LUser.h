#ifndef _L_USER_H_
#define _L_USER_H_ 

#include "LBase.h"
#include "LBuff.h"


class API_LBASESX LUser
{
public:
	LUser();
	~LUser();
	void	Read(LBuff& buff);
	void	Write(LBuff& buff);

public:
	Lint		m_id;
	Lstring		m_openid;
	Lstring		m_nike;
	Lint		m_sex;
	Lstring		m_provice;
	Lstring		m_city;
	Lstring		m_country;
	Lstring		m_headImageUrl;
	Lstring		m_unioid;
	Lint		m_numOfCard1s;
	Lint		m_numOfCard2s;
	Lint		m_numOfCard3s;
	Lint		m_regTime;
	Lint		m_lastLoginTime;
	Lint		m_new;
	Lint		m_gm;
	Lint		m_totalbuynum;
	Lint		m_totalplaynum;
	Lint        m_creditValue;
	Lint        m_count;
	Lint        m_coins;  //½ð±Ò
	Lstring m_customString1, m_customString2, m_customString3, m_customString4;
	Lstring m_customString5, m_customString6, m_customString7, m_customString8;
	Lstring m_customString9, m_customString10, m_customString11, m_customString12;
	Lstring m_customString13, m_customString14, m_customString15, m_customString16;	 
	Lint    m_customInt[16];	
};

#endif