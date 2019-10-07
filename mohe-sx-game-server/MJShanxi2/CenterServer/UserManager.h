#ifndef _USER_MANAGER_H_
#define _USER_MANAGER_H_

#include "LSingleton.h"
#include "DUser.h"


class UserManager : public LSingleton<UserManager>
{
public:
	UserManager();
	~UserManager();

	virtual	bool		Init();
	virtual	bool		Final();

	void	addUser(boost::shared_ptr<DUser> user);
	void	delUser(const Lstring& strUserId);
	void	delUser(Lint iUserId);

	bool   addMd5User(Lstring nick, Lstring headUrl, Lint sex, boost::shared_ptr<DUser>);

	boost::shared_ptr<CSafeUser> getUser(const Lstring& strUserId);
	boost::shared_ptr<CSafeUser> getUser(Lint iUserId);
	boost::shared_ptr<CSafeUser> getUserbyMd5(const Lstring& strMd5);

	Lint  getPhoneTime(Lstring phone, bool isAdd = true);
	Lint  setPhoneTime(Lstring phone);
	
public:
	void LoadUserIdInfo(Lint serverID);
public:
	void SaveUser(DUser* user);
	void UpdateUserNumsCard2(Lint userId, Lint count);
	//æ„¿÷≤ø◊Í Ø»Ø
	void UpdateClubCreatorNumDiamond( Lint userId, Lint nFeeValue);

	void SaveCardInfo(DUser* user,Lint cardType, Lint count, Lint operType, const Lstring& admin);
	void SaveCardInfo(Lint userId, Lint cardType, Lint count, Lint operType, const Lstring& admin);

	void SaveUserLastLogin(DUser* user);
	void SaveUserLastGPS(DUser* user);
	bool loadUserInfoFromDBByUserId(Lint userID, Lint& goldNum, Lstring& userIp, Lstring& userGps, Lint& blocked);
	bool loadUserInfoFromDBByUserId(Lint userID, Lint& goldNum, Lstring& userIp, Lstring& userGps, Lint& blocked,Lint &numsDiamond);

	Lint  getUserIdByPhone(Lstring phone);
	Lint  checkUserPhoneVcode(Lstring phone, Lstring vcode);

	Lint    CheckInFreeTime();
	bool    GetFreeTime(Lint& nStart, Lint& nEnd);

	//md5º”√‹À„∑®
	Lstring			Md5Hex(const void* data, Lsize size);
	Lstring getMd5User(Lstring nick, Lstring headUrl, Lint sex);

protected:
	bool	_loadUserInfoFromDB();
	void	_addUser(boost::shared_ptr<DUser> user);

	void	_loadTestData();		//—π¡¶≤‚ ‘
protected:
	void	_loadUserInfoFromDB(int iBeginIndex, int iCount);
private:
	bool m_bLoadUserSuccess;
private:
	boost::mutex m_mutexQueue;

	std::map<Lstring, boost::shared_ptr<DUser> > m_mapUUID2User;
	std::map<Lint, boost::shared_ptr<DUser> > m_mapId2User;

	std::map<Lstring, boost::shared_ptr<DUser> > m_mapMD5User;

	std::map<Lstring, Lint>  m_mapPhone2Time;
	
};

#define gUserManager UserManager::Instance()

#endif