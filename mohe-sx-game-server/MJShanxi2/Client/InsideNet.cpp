#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"


bool InsideNet::Init()
{
	LNet::Init(gConfig.GetInsideIp(), gConfig.GetInsidePort(), 4, true, false);
	return true;
}

bool InsideNet::Final()
{
	return true;
}

void InsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	Lint msgId = 0;
	msgpack::unpacked  unpack;
	msgpack::unpack(&unpack, recv->Data() + recv->GetOffset(), recv->Size() - recv->GetOffset());
	msgpack::object obj = unpack.get();

	LLOG_DEBUG("InsideNet::RecvMsgPack %d", recv->Size());

	std::stringstream ss;
	ss << obj;

	Lstring str = ss.str();
	//LLOG_DEBUG("InsideNet::RecvMsgPack %s", str.c_str());

	ReadMapValue(obj, "m_msgId", msgId);

	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgId);
	if(msg != NULL)
	{
		msg->Read(obj);
		msg->m_sp = s;
		gWork.Push(msg);
		LLOG_DEBUG("UserNet::RecvMsgPack user recv msgid=%d",msgId);
	}
	else
	{
		recv->Read(msgId);
		msg = LMsgFactory::Instance().CreateMsg(msgId);
		if (msg)
		{
			msg->Read(*recv.get());
			msg->m_sp = s;
			gWork.Push(msg);
			LLOG_DEBUG("UserNet::RecvMsgPack recv msgid=%d", msgId);
		}
		else
		{
			LLOG_ERROR("UserNet::RecvMsgPack error msgId=%d", msgId);
		}
	}
}