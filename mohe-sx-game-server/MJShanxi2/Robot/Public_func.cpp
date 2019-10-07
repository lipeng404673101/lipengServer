#include "StdAfx.h"
#include "Public_func.h"
#include "Net_Server_Inside.h"
#include "ClientManager.h"
#include "openssl\md5.h"
#include "Config.h"

void Net_Thread_Update() 
{
	while (1)
	{
		gServer_UserManager().Update();
		gServer_InsideNet.Tick();
		Sleep(1);
	}
}

void SetLog()
{
	gConfig.Init();
	LLog::Instance().SetFileName("Log_Robot");
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());
}

void SetNet()
{
	//协义注册器;
	LMsgFactory::Instance().Init();

	//监听器开启;
	gServer_InsideNet.Start(gConfig.GetLocolIp(),gConfig.GetLocolPort(),gConfig.GetThreadCount());

	//用户管理器;
	gServer_UserManager().Init(gConfig.GetServerIp(),gConfig.GetServerPort(),gConfig.GetRobotBegin(),gConfig.GetRobotCount());

	boost::thread thrd2(Net_Thread_Update);
	thrd2.detach();
}

std::string num2str(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

Lstring Md5Hex(const void* src ,Lsize len)
{
	unsigned char sign[17];
	memset(sign,0,sizeof(sign));

	MD5((const unsigned char*)src,len,sign);

	//转换成16进制
	char signHex[33];
	memset(signHex,0,sizeof(signHex));
	for(int i = 0 ; i < 16; ++i)
	{
		sprintf(signHex + i*2,"%02x", sign[i]);
	}

	return std::string(signHex);
}

void R_EraseCard(std::vector<Card>& src,Card* pCard,int n)
{
	//std::cout<<"------------ 删牌 -------------- begin"<<std::endl;
	Lint cnt = 0;
	auto it2 = src.begin();
	for(; it2 != src.end();)
	{
		if(cnt >= n)
			break;

		if(IsSame(pCard,&(*it2)))
		{
			//std::cout<<"------------ Color="<<it2->m_color<<"	Number="<<it2->m_number<<std::endl;
			cnt ++;
			it2 = src.erase(it2);
		}
		else
		{
			++it2;
		}
	}
	//std::cout<<"------------ 删牌 -------------- End"<<std::endl;
}

void R_EraseCard(std::vector<Card>& src,CardValue* pCard,int n)
{
	//std::cout<<"------------ 删牌 -------------- begin"<<std::endl;
	Lint cnt = 0;
	auto it2 = src.begin();
	for(; it2 != src.end();)
	{
		if(cnt >= n)
			break;

		Card One;
		One.m_color = pCard->m_color;
		One.m_number= pCard->m_number;
		if(IsSame(&One,&(*it2)))
		{
			//std::cout<<"------------ Color="<<it2->m_color<<"	Number="<<it2->m_number<<std::endl;
			cnt ++;
			it2 = src.erase(it2);
		}
		else
		{
			++it2;
		}
	}
	//std::cout<<"------------ 删牌 -------------- End"<<std::endl;
}

bool IsSame(Card* c1,Card* c2)
{
	return c1->m_color==c2->m_color&&c1->m_number==c2->m_number;
}

void NetJoin()
{
	gServer_InsideNet.Join();
}
