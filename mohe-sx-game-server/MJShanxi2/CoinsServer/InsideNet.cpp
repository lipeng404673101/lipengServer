#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "LLog.h"
#include "LTool.h"
#include "BaseCommonFunc.h"
#include "Work.h"

bool InsideNet::Init()
{
	LNet::Init("0.0.0.0", gConfig.GetInsidePort(), 16, true, false);
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

