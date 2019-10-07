#ifndef _L_BASE_H_
#define _L_BASE_H_

/*
error C4996 : 'std::_Copy_backward' : Function call with parameters that may be unsafe -
this call relies on the caller to check that the passed values are correct.
To disable this warning, use - D_SCL_SECURE_NO_WARNINGS.See documentation on how to use Visual C++ 'Checked Iterators'
*/
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <cctype>
#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <sstream>
#include <algorithm>

#define  _WIN32_WINNT  0x0501


#include <boost/thread.hpp>
#include <boost/asio.hpp>
//#include <boost/make_shared.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/lexical_cast.hpp>

#include <mysql.h>
//#include <mysql/mysql.h>

//#include <msgpack/msgpack.hpp>
#include <msgpack.hpp>


#if _LINUX
#define sprintf_s snprintf
#define gmtime_s gmtime_r
#include <json/json.h>
#else
#include <jsoncpp/json.h>
#endif


//#include <Windows.h>


//基本的整数类型如下
//char(8) short(16) int(32) float(32) double(64)
//
//
#if !((defined(WIN32) || defined(WIN64)) && defined(_DEBUG))
typedef unsigned long       DWORD;
typedef void *PVOID;
#endif

typedef char Lchar;
typedef unsigned char Luchar;

typedef short Lshort;
typedef unsigned short Lushort;

typedef int	Lint;
typedef unsigned int Luint;

typedef long long Llong;

typedef float Lfloat;

typedef double Ldouble;

typedef std::size_t Lsize;

typedef std::string Lstring;

#if _LINUX
typedef bool BOOL;
#define TRUE                1
#else
#pragma warning(disable:4996)	// 关闭这个字符串格式化不安去警告
#endif


typedef char *NPSTR, *LPSTR, *PSTR;

//var 转换成字符串 var"
#define NAME_TO_STR(var)  (#var)

#ifdef LBASESX_EXPORTS
#define API_LBASESX _declspec(dllexport)
#else
#define API_LBASESX _declspec(dllimport)
#endif


#endif

