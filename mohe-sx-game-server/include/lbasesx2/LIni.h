#ifndef _L_INI_H
#define _L_INI_H

#include "LBase.h"

class API_LBASESX LIniConfig
{
public:
	LIniConfig();
    LIniConfig(Lstring file);
	~LIniConfig();

	void	LoadFile(Lstring file);

	Lstring	GetString(Lstring key);
	Lstring GetString(Lstring key,Lstring def);

	Lint	GetInt(Lstring key);
	Lint	GetInt(Lstring key,Lint def);
    
    Lshort	GetShort(Lstring key);
    Lshort  GetShort(Lstring key,Lshort def);
   
    Lchar	GetChar(Lstring key);
    Lchar	GetChar(Lstring key,Lchar def);

private:
    std::map<Lstring,Lstring> m_ini_map;
	
};

#endif