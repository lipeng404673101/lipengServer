#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "LLog.h"
#include "LTool.h"
#include "BaseCommonFunc.h"
#include "Work.h"

bool InsideNet::Init()
{
	LNet::Init("0.0.0.0", gConfig.GetInsidePort(), 4, true, false);
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
	else
	{
		LLOG_ERROR("*******test , unkonw message package. remoteip=%s, remoteport=%d local ip=%s , local port = %d",s->getLocalIp().c_str(),
			s->getLocalPort(), s->GetRemoteIp().c_str(), s->GetRemotePort());
	}
}

////////////////////////////////////////////////////

bool OutsideNet::Init()
{
	LNet::Init(gConfig.GetOutsideIp(), gConfig.GetOutsidePort(), 8, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s, bool bIsFromInternal)
{
	LLOG_DEBUG("OutsideNet::RecvMsgPack");
	//只接收客户端的包，除非内部包
	if(bIsFromInternal)
	{
		LMsg* pMsg = parserMsgPack(recv, s);
		if(pMsg)
		{
			gWork.Push(pMsg);
		}
	}
	else
	{
		LLOG_DEBUG("OutsideNet::RecvMsgPack else");
		boost::shared_ptr<LMsgC2SVerifyHead> msgVerifyHead;
		boost::shared_ptr<LMsg> msgEntity;
		boost::shared_ptr<LBuff> msgOriginData;
		if(parserClientMsgPack(recv, msgVerifyHead, msgEntity, msgOriginData))
		{

			LMsgConvertClientMsg* pConvertMsg = new LMsgConvertClientMsg();
			pConvertMsg->msgHead = msgVerifyHead;
			pConvertMsg->msgEntity = msgEntity;

			pConvertMsg->msgHead->m_sp = s;
			pConvertMsg->msgEntity->m_sp = s;
			pConvertMsg->m_sp = s;

			LLOG_DEBUG("***Recv client message m_msgId:%d m_strUserId:%s", pConvertMsg->msgEntity->m_msgId, pConvertMsg->msgHead->m_strUserId.c_str());
		
			gWork.Push(pConvertMsg);
		}
		else
		{
			LLOG_ERROR("***Recv unkown message package. local ip:%s local port:%d - remote ip:%s remote port:%d", s->getLocalIp().c_str(),
				s->getLocalPort(), s->GetRemoteIp().c_str(), s->GetRemotePort());
			disableIP(s->GetRemoteIp());
			s->Kick();
		}
	}
}
