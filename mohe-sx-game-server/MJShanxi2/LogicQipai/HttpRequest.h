#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "LBase.h"
#include "LSingleton.h"
#include <libcurl/curl.h>
#include "HttpRequest.h"

 
class HttpRequest:public LSingleton<HttpRequest>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	//get请求,支持https
	bool			HttpGet( const std::string& url, std::string& result,bool isHttps=false, int timeout = 8 );
	//post请求,支持https
	bool			HttpPost(std::string url, std::string postData,std::string& result,bool isHttps=false);
	//md5加密算法
	Lstring			Md5Hex(const void* data,Lsize size);

	//解码url
	void			DeodeUrlParams(const Lstring& str, std::map<Lstring, Lstring>& params, bool urldecode);

	Lstring			UrlDecode(const char *src);
};

struct MHAgencyActivityStatHelper
{
	Lstring base_url;
	Lstring target_url;

	MHAgencyActivityStatHelper() { base_url = "http://api.sxmj.coolplaystore.com/push/new_activity?"; }

	inline void Invoke(int roomId, int userIds[], const char * pAgencyId, const char * pIsExpire, const char * pPlayCount, const char * pFinishTime,
		const char * pToSendAfterExpire)
	{
		if (pAgencyId && pIsExpire && pPlayCount && pFinishTime && pToSendAfterExpire)
		{

			std::stringstream ss;
			ss << base_url;
			ss << "roomid=" << roomId;
			ss << "&uid1=" << userIds[0] << "&uid2=" << userIds[1] << "&uid3=" << userIds[2] << "&uid4=" << userIds[3];
			ss << "&agency_id=" << pAgencyId;
			ss << "&play_count=" << pPlayCount;
			ss << "&is_expire=" << pIsExpire;
			ss << "&to_send_after_expire=" << pToSendAfterExpire;
			ss << "&finish_time=" << pFinishTime;
			target_url = ss.str();
			Lstring result;
			HttpRequest::Instance().HttpGet(target_url, result);
		}
		else
		{

		}

	}

	/*
	if (lReturnValue == 0)
	{
	DWORD agency_id = m_TreasureDBAide.GetValue_DWORD(TEXT("AgencyID"));
	WORD is_expire = m_TreasureDBAide.GetValue_WORD(TEXT("IsExpire"));
	DWORD play_count = m_TreasureDBAide.GetValue_DWORD(TEXT("PlayCount"));
	SYSTEMTIME finish_time;
	m_TreasureDBAide.GetValue_SystemTime(TEXT("FinishTime"), finish_time);
	WORD to_send_after_expire = m_TreasureDBAide.GetValue_WORD(TEXT("ToSendAfterExpire")); */

	/*std::string str = "http://api.gfgood.com/push/new_activity?";
	str += "uid1=" + pPrivateInfo->dwUserID[0];
	str += "&uid2=" + pPrivateInfo->dwUserID[1];
	str += "&uid3=" + pPrivateInfo->dwUserID[2];
	str += "&uid4=" + pPrivateInfo->dwUserID[3];
	str += "&agency_id=" + agency_id;
	str += "&play_count=" + play_count;
	str += "&is_expire=" + is_expire;
	str += "&to_send_after_expire=" + to_send_after_expire;
	str += "&finish_time=" + finish_time.wYear;
	str += "" + finish_time.wMonth;
	str += "" + finish_time.wDay;
	str += "" + finish_time.wHour;
	str += "" + finish_time.wMinute;
	str += "" + finish_time.wSecond;*/

	/*	TCHAR str_c[400];
	sprintf_s(str_c, "http://api.sx.coolplaystore.com/push/new_activity?room=%d&uid1=%d&uid2=%d&uid3=%d&uid4=%d&agency_id=%d&play_count=%d&is_expire=%d&to_send_after_expire=%d&finish_time=%d-%d-%d %d:%d:%d",
	pPrivateInfo->dwRoomNumber,
	pPrivateInfo->dwUserID[0],
	pPrivateInfo->dwUserID[1],
	pPrivateInfo->dwUserID[2],
	pPrivateInfo->dwUserID[3],
	agency_id,
	play_count,
	is_expire,
	to_send_after_expire,
	finish_time.wYear,
	finish_time.wMonth,
	finish_time.wDay,
	finish_time.wHour,
	finish_time.wMinute,
	finish_time.wSecond);

	SendHttpRequest(str_c); */

};


struct MHOneDrawActivityHelper
{
	Lstring base_url;
	Lstring target_url;

	//MHOneDrawActivityHelper() { base_url = "http://api.haoyunlaiyule1.com/push/agency_task?"; }
	MHOneDrawActivityHelper() { base_url = "http://api.sxmj.coolplaystore.com/push/agency_task?"; }

	inline void Invoke(int roomId, int playType, int userIds[], int score[], int userCount, int rount, Lint startTime)
	{
		std::stringstream ss;
		ss << base_url;
		ss << "room=" << roomId;
		ss << "&play_type=" << playType;
		ss << "&round=" << rount;
		ss << "&start_time=" << startTime;

		for (int i = 0; i<10; i++)
		{
			if (userIds[i] == 0)
				ss << "&uid" << std::to_string(i + 1) << "=";
			else
				ss << "&uid" << std::to_string(i + 1) << "=" << userIds[i];

		}
		//ss<<"&uid1="<<userIds[0]<<"&uid2="<<userIds[1]<<"&uid3="<<userIds[2]<<"&uid4="<<userIds[3]<<"&uid5="<<userIds[4];
		for (int i = 0; i<10; i++)
		{
			if (userIds[i] == 0)
				ss << "&fen" << std::to_string(i + 1) << "=";
			else
				ss << "&fen" << std::to_string(i + 1) << "=" << score[i];
		}

		target_url = ss.str();


		//LLOG_DEBUG("target_url=[%s]", target_url.c_str());

		Lstring result;
		HttpRequest::Instance().HttpGet(target_url, result);

	}

};

#endif