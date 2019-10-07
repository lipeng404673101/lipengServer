#include "UserMessageMsg.h"
#include "BaseCommonFunc.h"

CUserMessageMsg::CUserMessageMsg()
{

}

CUserMessageMsg::~CUserMessageMsg()
{

}

bool CUserMessageMsg::Init()
{
	return true;
}

bool CUserMessageMsg::Final()
{
	return true;
}

void CUserMessageMsg::Start()
{
	for(int i = 0; i < USER_MSG_COUNT; i++)
	{
		m_userMessage[i].Start();
	}
}

void CUserMessageMsg::Join()
{
	for(int i = 0; i < USER_MSG_COUNT; i++)
	{
		m_userMessage[i].Join();
	}
}

void CUserMessageMsg::Stop()
{
	for(int i = 0; i < USER_MSG_COUNT; i++)
	{
		m_userMessage[i].Stop();
	}
}

void CUserMessageMsg::handlerMessage(const Lint& userId, LMsg* pMsg)
{
	if(pMsg == NULL)
	{
		return;
	}

 	int iIndex = userId % USER_MSG_COUNT;
 	m_userMessage[iIndex].Push(pMsg);
}