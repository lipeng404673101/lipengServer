#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "LLog.h"
#include "LTool.h"
#include "BaseCommonFunc.h"
#include "Work.h"

bool InsideNet::Init()
{
	LNet::Init(gConfig.GetInsideIp(), gConfig.GetInsidePort(), 12, true, false);
	return true;
}

bool InsideNet::Final()
{
	return true;
}

void InsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s, bool bIsFromInternal)
{
	//接收内网的数据不需要提出，直接丢弃
	LMsg* pMsg = parserMsgPack(recv, s);
	if(pMsg)
	{
		gWork.Push(pMsg);
	}
}


//////////////////////////////////////////////////////////////////////////

bool GmNet::Init()
{
	LNet::Init(gConfig.GetGMIp(), gConfig.GetGMPot(), 4, true, true);
	return true;
}

bool GmNet::Final()
{
	return true;
}

void GmNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s, bool bIsFromInternal)
{
	LMsg* pMsg = parserMsgPack(recv, s);
	if(pMsg)
	{
		gWork.Push(pMsg);
	}
	else
	{
		s->Kick();
	}
}
