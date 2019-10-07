#ifndef _L_SOCKET_H_
#define _L_SOCKET_H_

#include "LBuff.h"

class LNet;

enum SOCKET_CONNECT_STATUS
{
	SOCKET_CONNECT_STATUS_DISCONNECTING,	//正在断开
	SOCKET_CONNECT_STATUS_DISCONNECTED,		//已经断开
	SOCKET_CONNECT_STATUS_CONNECTING,		//正在连接
	SOCKET_CONNECT_STATUS_CONNECTED			//连接成功
};

/*
*	brief 对原始套接字的封装，实现基本的
*	连接，收发消息的功能
*/
class API_LBASESX LSocket:public boost::enable_shared_from_this<LSocket>
{
public:
	/*
	*	brief 构造函数
	*	param ls LNet引用
	*/
	LSocket(LNet& ls);

	/*
	*	brief 析构函数
	*/
	virtual~LSocket();

	/*
	*	brief 连接到指定ip和端口的服务器，阻塞方式
	*	param endpoint 指定的ip端口
	*/
	virtual bool Connect(boost::asio::ip::tcp::endpoint endpoint);

	/*
	*	brief 连接到指定ip和端口的服务器，阻塞方式
	*	param ip 指定的ip
	*	param port 指定的port
	*/
	virtual bool Connect(Lstring ip,Lshort port);

	/* 
	*	brief 连接到指定ip和端口的服务器，非阻塞
	*	param endpoint 指定的ip端口
	*/
	virtual void AsyncConnect(boost::asio::ip::tcp::endpoint endpoint);

	virtual void AsyncConnect(Lstring ip,Lshort port);

	/* 
	*	brief 异步连接回调函数
	*	param e 连接错误码
	*/
	virtual bool HandleAsyncConnect(const boost::system::error_code& e);

	/*	
	*	brief 连接成功，开始收发消息
	*/
	virtual void Start();

	/*	
	*	brief 主动停止该连接
	*/
	virtual void Stop(bool bKick = false);

	/* 
	*  brief 收到一个完整的消息体
		bIsFromInternal : 消息是否由内部产生
	*/
	virtual void OnRecvOneMsgPack(LBuffPtr buff, bool bIsFromInternal = false);

	/* 
	*	brief 发送消息，异步不用等待
	*/
	void Send(LBuffPtr pBuff); 

	/* 
	*	brief 获取原始socket
	*/
	boost::asio::ip::tcp::socket& Socket();

	/*
	获取本地地址
	*/
	Lstring getLocalIp();

	/*
	获取本地端口
	*/
	int		getLocalPort();

	/* 
	*	brief 获取远程主机的ip地址
	*/
	Lstring GetRemoteIp();
	
	/* 
	*	brief 获取远程主机的端口
	*/
	int GetRemotePort();

	/* 
	*	brief 获取这个Socket是否激活
	*/
	bool GetActive();

	/* 
	*	brief 踢掉这个socket
	*/
	void Kick();

	SOCKET_CONNECT_STATUS getSocketConnectStatus();

	int getSendPoolSize();
private:
	/*	
	*	brief 异步读回调
	*	param e 错误码
	*	param bytes_transferred 读取的消息长度
	*/
	void HandleRead(const boost::system::error_code& e,std::size_t readSize);

	/* 
	*	brief 异步写回调
	*	param e 错误码
	*/
	void HandleWrite(const boost::system::error_code& e, std::size_t writeSize);

	bool Check();
	bool OnRecv();

	bool onRecvHttp();
	bool onRecvBinary();
protected:
	void _completeReadData(Lint iCompleteLen);
	void _stop(bool bKick);
	void _sendKickMsg();
private:
	LNet& m_ns;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::strand m_strand;
	
	SOCKET_CONNECT_STATUS m_connectStatus;

	bool m_start;
	bool m_sending;

	int m_remotePort;
	Lstring m_strRemoteIp;

	int m_iLocalPort;
	Lstring m_strLocalIp;

	bool m_active;//检测连接上来后，一直不发消息的死连接，这样的连接出现的概率很低，先不管它
private:
	//for socket resource
	boost::mutex m_mutexSocketForRead;
	boost::mutex m_mutexSocketForWrite;
	boost::mutex m_mutexSocketForStop;

	//for write
	std::list<LBuffPtr> m_sendPool;
	LBuffPtr m_curretSendBuffer;
	int m_iCurrentSendSize;

	//for read
	char* m_pRecvBuff;
	LBuffPtr m_currentReadBuffer;
};

/*
*	LSocket的智能指针,所有与LSocket相关的东西都要用这个智能指针
*/
typedef boost::shared_ptr<LSocket> LSocketPtr;

#endif