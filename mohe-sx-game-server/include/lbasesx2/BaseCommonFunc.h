#ifndef _BASE_COMMON_FUNC_
#define _BASE_COMMON_FUNC_

#include "LBuff.h"
#include "LSocket.h"
#include "LRunnable.h"
#include "LMsg.h"

#ifdef WIN32

API_LBASESX void disableConsoleCloseButton();

#endif

API_LBASESX Lstring caculateMd5ToHex(const void* pData, unsigned int uDataLen);

/*
解析客户端发过来的消息

消息格式：
|--2 bytes(验证头长度)--|--N bytes(验证头)--|--2 bytes(消息实体长度)--|--1 byte(是否经过封包)--|--N bytes(消息实体)--|
*/
API_LBASESX bool parserClientMsgPack(LBuffPtr recv,
						 boost::shared_ptr<LMsgC2SVerifyHead>& msgVerifyHead, 
						 boost::shared_ptr<LMsg>& msgEntity,
						 boost::shared_ptr<LBuff>& msgOriginData);

//Format
// |--1 byte(isPacket)--|--N bytes(data)--|
API_LBASESX LMsg* parserMsgPack(LBuffPtr recv, LSocketPtr s);

API_LBASESX void disableIP(std::string strIP);

API_LBASESX unsigned int caculateHashKey(const char* pchData, int iDataLen);

//更新随机因子
API_LBASESX void updateRandomFactor();

///////////////////////Function///////////////////////////////////////////////////

API_LBASESX std::string convertInt2String(int iValue);
API_LBASESX std::string convertUInt2String(unsigned int uValue);

#endif