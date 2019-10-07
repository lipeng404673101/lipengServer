#include "OutsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "BaseCommonFunc.h"

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
	//LMsgFactory::Instance().CreateMsg(33); 内存泄漏
	LMsg* pMsg = parserMsgPack(recv, s);
	if(pMsg)
	{
		gWork.Push(pMsg);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
OutsideNet::OutsideNet()
{
	m_uListenPort = 0;
}

OutsideNet::~OutsideNet()
{

}

bool OutsideNet::Init()
{
	m_uListenPort = gConfig.GetOutsidePort();

	LNet::Init("0.0.0.0", m_uListenPort, 8, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s, bool bIsFromInternal)
{
	//任何的包都将解开验证
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
		boost::shared_ptr<LMsgC2SVerifyHead> msgVerifyHead;
		boost::shared_ptr<LMsg> msgEntity;
		boost::shared_ptr<LBuff> msgOriginData;
		if(parserClientMsgPack(recv, msgVerifyHead, msgEntity, msgOriginData))
		{
			LMsgConvertClientMsg* pConvertMsg = new LMsgConvertClientMsg();
			pConvertMsg->msgHead = msgVerifyHead;
			pConvertMsg->msgEntity = msgEntity;
			pConvertMsg->msgOriginData = msgOriginData;

			pConvertMsg->msgHead->m_sp = s;
			pConvertMsg->msgEntity->m_sp = s;
			pConvertMsg->m_sp = s;

			gWork.Push(pConvertMsg);
			//LLOG_DEBUG("***Receive msg msgid:%d, m_strUserId:%s", pConvertMsg->msgHead->m_msgId, pConvertMsg->msgHead->m_strUserId.c_str());
		}
		else
		{
			LLOG_DEBUG("***parserClientMsgPack fail. disableIP(s->GetRemoteIp())");
			disableIP(s->GetRemoteIp());
			s->Kick();
		}
	}

// 	LMsg* pMsg = parserMsgPack(recv, s);
// 	if(pMsg)
// 	{
// 		Lint msgId = pMsg->m_msgId;
// 		if(msgId > MSG_ERROR_MSG && msgId < MSG_S_2_C_MAX)
// 		{
// 			LBuffPtr tmpBuf(new LBuff);
// 			tmpBuf->Write(recv->Data() + 1, recv->Size() - 1); //跳过包是否被打包的标志位
// 
// 			LMsgG2LUserMsg* g2lMsg = new LMsgG2LUserMsg();
// 			g2lMsg->m_userGateId	= 0;
// 			g2lMsg->m_userMsgId	= msgId;
// 			g2lMsg->m_dataBuff	= tmpBuf;
// 			g2lMsg->m_ip			= s->GetRemoteIp();
// 			g2lMsg->m_sp			= s;
// 			gWork.Push(g2lMsg);
// 
// 			//这个pMsg应该被释放,以后使用智能指针
// 			delete pMsg;
// 		}
// 		else
// 		{
// 			gWork.Push(pMsg);
// 		}
// 	}
// 	else
// 	{
// 		//屏蔽ip
// 		disableIP(s->GetRemoteIp());
// 
// 		s->Kick();
// 	}
}
