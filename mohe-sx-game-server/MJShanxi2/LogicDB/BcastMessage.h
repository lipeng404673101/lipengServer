
#ifndef _BCASTMESSAGE_H_
#define _BCASTMESSAGE_H_


#include "LSingleton.h"
#include "LMsgS2S.h"
#include "LDBSession.h"

class bcastMessage :public LSingleton<bcastMessage>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();


	Lint        GetMessageListNum();

	bool       LoadMessageDataFromDB();
	void       produceBcastMessage(LMsgLDB2LMBCast& bCast);

private:
	std::vector<std::string> m_messageList; //所有广播信息

	LDBSession* m_dbsession;
};

#define gBcastMessage bcastMessage::Instance()

#endif