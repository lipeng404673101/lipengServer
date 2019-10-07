#include "stdafx.h"
#include "Net_Server_Inside.h"
#include "LMsg.h"
#include "ClientManager.h"
#include "LTool.h"

void Net_Server_Inside::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	Lint msgid = MSG_ERROR_MSG;
	msgpack::unpacked  unpack;
	msgpack::unpack(&unpack, recv->Data() + recv->GetOffset(), recv->Size() - recv->GetOffset());
	msgpack::object obj = unpack.get();		
	ReadMapData(obj, "m_msgId", msgid);

	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
	if(msg != NULL)
	{
		msg->Read(obj);
		msg->m_sp = s;
		Push(msg);
	}
	else
	{
		recv->Read(msgid);
		msg = LMsgFactory::Instance().CreateMsg(msgid);
		if (msg)
		{
			msg->Read(obj);
			msg->m_sp = s;
			Push(msg);
			LLOG_DEBUG("InsideNet::RecvMsgPack recv msgid=%d", msgid);
		}
		else
		{
			//LLOG_ERROR("InsideNet::RecvMsgPack error msgId=%d", msgid);
		}
	}
}

void Net_Server_Inside::Tick()
{
	while(1) 
	{
		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			return;
		}
		else
		{
			Handle_Msg_FuncCall(msg);
			delete msg;
		}
	}
}

void Net_Server_Inside::Swap()
{
	boost::mutex::scoped_lock l(m_msgMutex);
	std::list<void*>* tmp = m_msgDoing;
	m_msgDoing = m_msgRecv;
	m_msgRecv = tmp;
}

void Net_Server_Inside::Push(void* msg)
{
	boost::mutex::scoped_lock l(m_msgMutex);
	m_msgRecv->push_back(msg);
}

void* Net_Server_Inside::Pop()
{
	void* pack = NULL;
	if(m_msgDoing->empty())
	{
		Swap();
	}

	if(m_msgDoing->empty())
	{
		return pack;
	}

	pack = m_msgDoing->front();
	m_msgDoing->pop_front();
	return pack;
}

bool Net_Server_Inside::Start(Lstring Ip,Lshort Port,Lint ThreadCount)
{
	m_msgDoing = &m_msgQueue[0];
	m_msgRecv  = &m_msgQueue[1];
	if(LNet::Init(Ip, Port, ThreadCount, true, false))
	{
		LNet::Start();
	}
	return false;
}

void Net_Server_Inside::Handle_Msg_FuncCall(LMsg* msg)
{
	gServer_UserManager().Handler(msg);
}