#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "BaseCommonFunc.h"

bool InsideNet::Init()
{
	LNet::Init(gConfig.GetLogicDBIp(), gConfig.GetLogicDBPort(), 8, true, false);
	return true;
}

bool InsideNet::Final()
{
	return true;
}

void InsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s, bool bIsFromInternal)
{
	LMsg* pMsg = parserMsgPack(recv, s);
	if(pMsg)
	{
		gWork.Push(pMsg);
	}
}