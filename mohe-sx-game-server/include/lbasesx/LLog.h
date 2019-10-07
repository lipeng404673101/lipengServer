#ifndef _L_LOG_H_
#define _L_LOG_H_

#include "LRunnable.h"
#include "LSingleton.h"

enum LLOG_LEVEL
{
	LLOG_LEVEL_NULL = 0,//不输入任何的log
	LLOG_LEVEL_ERROR,//最高级别的log
	LLOG_LEVEL_INFO,//普通级别的log
	LLOG_LEVEL_DEBUG,//debug级别的log，这个适用于所有调试模式
};

class LLogData
{
public:
	LLogData()
	{
		m_logLevel = LLOG_LEVEL_NULL;
		m_iLineNumber = 0;
	}
	~LLogData(){}
public:
	LLOG_LEVEL m_logLevel;
	std::string m_strFuncName;
	std::string m_strLog;
	std::string m_strTime;
	int m_iLineNumber;
};

class API_LBASESX LLog :public LRunnable,public LSingleton<LLog>
{
public:
	LLog();
	virtual~LLog();

	virtual bool Init();
	virtual bool Final();

	virtual void Run();
	virtual void Clear();

	void SetFileName(const char* fileName);

	void SetLogLevel(Lint logLevel);

	void Log(LLOG_LEVEL level, const char* pchFuncName, int iLineNumber, const char* pchLog, ...);

	void WriteFile(char const* content);
protected:
	std::string _formatLog(LLogData* pLogData);
	void _outToConsole(LLogData* pLogData);
private:
	int	m_level;
	std::string m_fileName;
};

/*
*	brief 错误消息输入，级别高，所有影响到服务器正常运行的
*	      包括:配置文件错误，端口错误，系统崩溃等等都要用这个输出
*/
#define LLOG_ERROR(log, ...)	LLog::Instance().Log(LLOG_LEVEL_ERROR, __FUNCTION__, __LINE__, log, ##__VA_ARGS__)


/*
*	brief 一般的信息输出，级别一般
*		  服务器状态信息，监视信息的输出，用这个
*/
#define LLOG_INFO(log, ...)		LLog::Instance().Log(LLOG_LEVEL_INFO, __FUNCTION__, __LINE__, log, ##__VA_ARGS__)


/*
*	brief Debug模式下的log输出，所有用于调试输出的log都要用这个
*/
#define LLOG_DEBUG(log, ...)	LLog::Instance().Log(LLOG_LEVEL_DEBUG, __FUNCTION__, __LINE__, log, ##__VA_ARGS__)


#endif