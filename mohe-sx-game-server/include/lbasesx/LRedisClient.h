#ifndef _L_REDIS_CLIENT_H_
#define _L_REDIS_CLIENT_H_

#include "LBase.h"
#include <string>
#include <boost/asio.hpp>
#include "redisclient/redissyncclient.h"

class API_LBASESX CRedisClient
{
public:
	CRedisClient();
	virtual ~CRedisClient();
public:
	virtual bool initRedis(std::string strRedisIp, unsigned short uRedisPort);
public:
	//String操作
	bool writeDataByString(const std::string& strKey, const std::string& strWrite, unsigned int iKeyTimeOut = -1);
	bool writeDataByString(const std::string& strKey, int iWrite, unsigned int iKeyTimeOut = -1);
	bool writeDataByString(const std::string& strKey, unsigned int uWrite, unsigned int iKeyTimeOut = -1);
	bool writeDataByString(const std::string& strKey, short sWrite, unsigned int iKeyTimeOut = -1);
	bool writeDataByString(const std::string& strKey, unsigned short suWrite, unsigned int iKeyTimeOut = -1);
	bool writeDataByString(const std::string& strKey, bool bWrite, unsigned int iKeyTimeOut = -1);

	bool readDataByString(const std::string& strKey, std::string& strRead);
	bool readDataByString(const std::string& strKey, int& iRead);
	bool readDataByString(const std::string& strKey, unsigned int& uRead);
	bool readDataByString(const std::string& strKey, short& sRead);
	bool readDataByString(const std::string& strKey, unsigned short& suRead);
	bool readDataByString(const std::string& strKey, bool& bRead);

	//Hash操作
	bool writeDataByHash(const std::string& strKey, const std::string& strField, const std::string& strWrite);
	bool writeDataByHash(const std::string& strKey, const std::string& strField, int iWrite);
	bool writeDataByHash(const std::string& strKey, const std::string& strField, unsigned int uWrite);
	bool writeDataByHash(const std::string& strKey, const std::string& strField, short sWrite);
	bool writeDataByHash(const std::string& strKey, const std::string& strField, unsigned short suWrite);
	bool writeDataByHash(const std::string& strKey, const std::string& strField, bool bWrite);

	bool readDataByHash(const std::string& strKey, const std::string& strField, std::string& strRead);
	bool readDataByHash(const std::string& strKey, const std::string& strField, int& iRead);
	bool readDataByHash(const std::string& strKey, const std::string& strField, unsigned int& uRead);
	bool readDataByHash(const std::string& strKey, const std::string& strField, short& sRead);
	bool readDataByHash(const std::string& strKey, const std::string& strField, unsigned short& uRead);
	bool readDataByHash(const std::string& strKey, const std::string& strField, bool& bRead);

	//list操作
	bool writeDataByList(const std::string& strKey, const std::string& strWrite);
	bool writeDataByList(const std::string& strKey, int& iWrite);
	bool writeDataByList(const std::string& strKey, unsigned int& uWrite);
	bool writeDataByList(const std::string& strKey, short& sWrite);
	bool writeDataByList(const std::string& strKey, unsigned short& suWrite);
	bool writeDataByList(const std::string& strKey, bool& bWrite);

	//扩展
	bool writeDataByList(const std::string& strKey, const std::string& strWrite, unsigned int limit);   //limit 20

	bool rmListDataByValue(const std::string& strKey, const std::string& strValue);

	bool popListDataByKey(const std::string& strKey, std::string& outValue);

	bool readDataByList(const std::string& strKey, unsigned int iFromIndex, unsigned int iReadCount, std::list<std::string>& listRead);

	bool setKeyTTLTime(const std::string& strKey, unsigned int tim_secs);

	//公共操作
	bool isExistKey(const std::string& strKey);

	bool delKey(const std::string& strKey);
	bool delHKey(const std::string& strKey, const std::string& strField);

	bool setKeyTimeOut(const std::string& strKey, unsigned int iKeyTimeOut);
protected:
	bool _connectRedis();

	//执行命令，参数不能多于七个
	bool _excuteCommand(const std::string& strCmd, const std::list<std::string>& listArg, RedisValue& result);
protected:
	bool m_bInit;
protected:
	boost::asio::io_service m_ioService;
	boost::shared_ptr<RedisSyncClient> m_redisClient;
protected:
	std::string m_strRedisIp;
	unsigned short m_uRedisPort;
};

#endif