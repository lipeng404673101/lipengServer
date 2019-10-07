#ifndef _D_USER_H_
#define _D_USER_H_


#include "LUser.h"

/************************************************************************************************
LUser 字段说明
1.  m_customInt[0]
2.  m_customIni[1]
3.  m_customInt[2]
4.  m_customInt[3] user -block 是否封号
5. m_customInit[4] user_info - level 用户级别
6. m_customInit[5] userid, -记录客户端保存的userid，掌赢换包的时候用--应用内记载userid  
7. m_customInit[6] userid, -记录客户端保存的userid，掌赢换包的时候用--系统内记载userid

//分数平衡  5个  11-15
m_customInt[10]  --累计分数
m_customInt[11]  --平衡分数
m_customInt[12]  --调整次数


1. m_customString1  IP
2. m_customString2 gps
3. m_customString3
4. m_customString4  phoneUUID,-记录客户端上传的手机机器符,
5. m_customString5   记录客户端保存的userid, 多个userId 连着用，掌赢换包的时候用   手机app内记录userid
6. m_customString6  记录客户端保存的userid, 多个userId 连着用，掌赢换包的时候用    手机系统内记录userid
7. m_customString7  记录客户登录用的手机型号

8.m_customString8   phoneUUID当前
9.m_customString9   记录客户登录用的手机型号  当前

************************************************************************************************/


class CSafeLock
{
public:
	CSafeLock()
	{
		m_uLockedSum = 0;
	}
	virtual ~CSafeLock(){}
protected:
	boost::thread::id m_lockedThreadId;
	unsigned int m_uLockedSum;
};

class DUser : public CSafeLock
{
	friend class CSafeUser;
public:
	DUser();
	virtual ~DUser();

	// 添加房卡
	void	AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin);
	//删除房卡
	void	DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin);

	// 添加金币
	void	AddCoinsCount(Lint count, Lint operType);
	// 删除金币
	void	DelCoinsCount(Lint count, Lint operType);

	void    ExchangeCardCoin(Lint add,Lint del,Lint operType,Lint cardType,const Lstring& admin);

	void    AddCreditValue();   //只会加信用

	// 增加玩家打牌的次数
	void	AddPlayCount();

	// 修改new状态 value见LMsgL2CeModifyUserNew中的注释
	void	ModifyNew( Lint value );
	
public:
	LUser m_usert;
	Lint  m_lastReqTime;	//最近一次请求登录的时间
	Lint  m_logicID;		//玩家当前所在服务器Index
};

class CSafeUser
{
public:
	CSafeUser(boost::shared_ptr<DUser> user);
	~CSafeUser();
public:
	boost::shared_ptr<DUser>& getUser();
	bool isValid();
protected:
	void _lockUser();
	void _unlockUser();
private:
	boost::shared_ptr<DUser> m_User;
	bool m_bLocked;
private:
	static boost::recursive_mutex m_mutexLockUser;
private:
	CSafeUser(){}
};

#endif