#ifndef _USER_MESSAGE_MSG_H_DB
#define _USER_MESSAGE_MSG_H_DB

#include "UserMessage.h"
#include "LSingleton.h"

#define USER_MSG_COUNT	32

class CUserMessageMsg : public LSingleton<CUserMessageMsg>
{
public:
	CUserMessageMsg();
	virtual ~CUserMessageMsg();
public:
	virtual bool Init();
	virtual bool Final();
public:
	void Start();
	void Join();
	void Stop();
public:
	void handlerMessage(const Lint& userId, LMsg* pMsg);
private:
	CUserMessage m_userMessage[USER_MSG_COUNT];
};

#define gUserMessageMsg		CUserMessageMsg::Instance()

#endif