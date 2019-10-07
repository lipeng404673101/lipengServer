#pragma once

// CMyHttpRequest ÃüÁîÄ¿±ê
using namespace std;

class CMyHttpRequest : public CObject
{
public:
	CMyHttpRequest();
	virtual ~CMyHttpRequest();
	static bool InvokeGet(const std::string& httpurl, std::string& result, bool isHttps, int timeout);
};


