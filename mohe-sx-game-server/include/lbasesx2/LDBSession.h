#ifndef _L_DBSESSION_H_
#define _L_DBSESSION_H_

#include "LBase.h"

class API_LBASESX LDBSession
{
public:
	LDBSession();
	~LDBSession();

	bool Init( const std::string& host, const std::string& user, const std::string& pass, const std::string& dbname, const std::string& character, Lshort port );

	MYSQL* GetMysql();

private:
	Lstring m_Host;
	Lstring m_User;
	Lstring m_Pass;
	Lstring m_DBName;
	Lstring m_Character;
	Lshort  m_Port;
	MYSQL*  m_Mysql;

};

#endif