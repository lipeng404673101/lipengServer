#ifndef _USERPLAYDATA_H_
#define _USERPLAYDATA_H_


#include "LSingleton.h"
#include "LMsgS2S.h"
#include "LRedisClient.h"
#include "LDBSession.h"

class PlayData :public LSingleton<PlayData>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();

	bool	   GetUserPlayTotalAround(LMsgL2LDBGetUserPlayData* msg, Lint& totalAro, Lint& winAro);

	bool	   UpdateUserPlayScore(LMsgL2LDBUpdatePlayerScore* msg, Lint& totalAro, Lint& winAro);

	bool       GetUserPlayTotalAroundFromDB(const LSocketPtr m_sp, const Lint userId, Lint& totalAro);

	bool       GetUserPlayWinPercentFromDB(const LSocketPtr	m_sp, const Lint userId, Lint& winAro);

	MYSQL * GetMySQL();

private:
	LDBSession* m_dbsession;
	LDBSession* m_dbsessionBK;

	CRedisClient m_RedisClient;

};

#define gPlayData PlayData::Instance()

#endif