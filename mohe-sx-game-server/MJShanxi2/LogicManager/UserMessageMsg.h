#ifndef _USER_MESSAGE_MSG_H_
#define _USER_MESSAGE_MSG_H_

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
	void handlerMessage(const Lstring& strUUID, LMsg* pMsg);
	void handlerMessage(const Lint deskId, LMsg* pMsg);
private:
	CUserMessage m_userMessage[USER_MSG_COUNT];
};

#define gUserMessageMsg		CUserMessageMsg::Instance()

#endif