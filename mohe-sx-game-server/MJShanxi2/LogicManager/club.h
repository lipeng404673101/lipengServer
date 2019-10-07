#ifndef _CLUB_H_
#define _CLUB_H_

#include "LBase.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "DeskManager.h"
#include "SafeResourceLock.h"
#include "LTime.h"
#include "clubDesk.h"


//前置声明
//class User;
class UserBaseInfo;
struct clubFeeType;
struct UserAddToClubDeskInfo;


#define   MAX_DESK_COUNT_EVERY_TYPE  100
#define  MAX_PLAY_TYPE_SIZE_EACH_CLUB   3

#define FIRST_TABLE_RED_PACKET_COUNT 5

struct First_Table_Red_Packet
{
	std::set<Lint>             m_redPacketDesk;            

	Lint                                m_showRedPacketDesk;              //显示红包的桌子

	First_Table_Red_Packet()
	{
		
		m_showRedPacketDesk = 0;
	}

	~First_Table_Red_Packet(){}


	Lint getRedPacketDeskId()
	{
		return m_showRedPacketDesk;
	}

	void setRedPacketDeskId(Lint clubDeskId)
	{
		m_showRedPacketDesk = clubDeskId;
	}

	void unsetRedPacketDeskId()
	{
		m_redPacketDesk.insert(m_showRedPacketDesk);
		m_showRedPacketDesk = 0;

	}

	void clearRedPacketDesk()
	{
		m_redPacketDesk.clear();
	}

	bool isDeskRedPacket(Lint clubDeskId)
	{
		if (m_showRedPacketDesk == clubDeskId)return true;
		if (m_redPacketDesk.find(clubDeskId) != m_redPacketDesk.end())
		{
			return true;
		}
		return false;
	}


};

struct Red_Packet
{

	Red_Packet()
	{
		setRedPacketCount(FIRST_TABLE_RED_PACKET_COUNT);
		m_clubId = 0;
	}

	~Red_Packet() {};

	void init(Lint clubId)
	{
		m_clubId = clubId;
	}

	void setRedPacketCount(Lint redPacketCount)
	{
		m_count = redPacketCount;
	}

	bool haveRedPacket(LTime & cur)
	{
		if (cur.GetDate() >= m_time.GetDate() + 1)
		{
			m_count = FIRST_TABLE_RED_PACKET_COUNT;
			m_time = cur;
			//清空发过红包的桌子
			for (auto ItFirstRedPacket = m_mapPlayTypeId2RedPacket.begin(); ItFirstRedPacket != m_mapPlayTypeId2RedPacket.end(); ItFirstRedPacket++)
			{
				ItFirstRedPacket->second.clearRedPacketDesk();
			}
			
		}
		LLOG_DEBUG("haveRedPacket: club[%d] curDate[%d],redTime[%d] redCount[%d]", m_clubId,cur.GetDate(), m_time.GetDate(), m_count);
		return m_count > 0 ? true : false;
	}

	void addFirstTableRedPacket(Lint playId)
	{
		auto ItRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItRed == m_mapPlayTypeId2RedPacket.end())
		{
			First_Table_Red_Packet temp;
			m_mapPlayTypeId2RedPacket[playId] = temp;
		}
	}

	Lint  getRedPacketDeskId(Lint playId)
	{
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			return ItTableRed->second.getRedPacketDeskId();
		}

		return -1;
	}

	void setRedPacketDeskId(Lint playId, Lint clubDeskId)
	{
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			m_count--;
			LLOG_DEBUG("setRedPacketDeskId club[%d] redPacaketCount[%d]", m_clubId, m_count);

			return ItTableRed->second.setRedPacketDeskId(clubDeskId);
		}
		

	}

	void unsetRedPacketDeskId(Lint playId,Lint status=0)
	{
		if (status == 2)
		{
			m_count++;
			return;
		}
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			 ItTableRed->second.unsetRedPacketDeskId();
		}
		if (status == 1)
		{
			m_count++;
		}
		return;
	}

	bool isDeskRedPacket(Lint playId, Lint clubDeskId)
	{
		if (clubDeskId == 0 || playId == 0)return false;
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			return ItTableRed->second.isDeskRedPacket(clubDeskId);
		}
		return false;
	}


private:
	LTime   m_time;    //时间
	Lint       m_count;   //红包个数
	Lint       m_clubId;  //俱乐部Id

	std::map<Lint, First_Table_Red_Packet>  m_mapPlayTypeId2RedPacket;

};

//俱乐部桌子用户信息
struct clubDeskUserInfo
{
	Lstring    m_deskUserName;
	Lstring    m_deskUserUrl;
	Lint       m_pos;
};

class Club:public CResourceLock
{
public:
	Club(Lint clubId);
	~Club();
	//加载俱乐部信息
	void initData(LMsgCe2LAddClub *msg);
	void resetData();
	
	//俱乐部会长的动作接口
	bool  addClubPlayType(clubPlayType &  aClubPlayType);
	bool  hidePlayType(Lint playTypeId,Lstring playType,Lint playTypeIdStatus);                                                                                 //隐藏玩法
	bool  alterClubName(Lstring newClubName);                                                             //修改俱乐部名字	
	bool  alterPlayTypeName(Lint playType, Lstring newPlayTypeName);

    bool  modifyClubFeeType(Lint feeType);

	//辅助函数
	bool  isClubValid() { m_valid==0?false:true; }

private:
	
	bool _addClubPlayType(Lint playId);
	bool _addClubPlayTypeWithDesk(Lint playId, Lint deskCount, std::vector<ManagerClubDeskInfo>& clubdeskList);
	Lint _createClubDeskList(Lint deskCount, Lint playId);                                                                                //创建俱乐部桌子列表
	Lint _fillClubDeskList(const std::vector<ManagerClubDeskInfo> &clubDeskinfo, Lint playId);



	//俱乐部会员行为接口
public:

	//返回客户端刷新界面的内容
	//std::deque<Lint>  reorderClubDesk(Lint playId,std::list<Lint> clubDeskList);

	//会长和管理员
	Lint getClubOwerId()
	{
		return m_ownerId;
	}
	Lstring  getClubOwerUUID()
	{
		return m_ownerUUioid;
	}
	Lstring  getClubOwerNike()
	{
		return m_ownerNike;
	}
	Lstring  getClubOwerPhoto()
	{
		return m_ownerHeadUrl;
	}
	void getClubOwnerAndAdmin(Lint *owner, std::set<Lint> &admin)  //获取会长和管理员
	{
		*owner = getClubOwerId();
		admin = m_adminUser;
	}
	//判断是否是管理员或者会长
	bool   isLeader(Lint userId) { if (userId == getClubOwerId() || m_adminUser.find(userId) != m_adminUser.end()) return true; return false; }
	Lint   setAdmin(Lint type, Lint setUserId, Lint adminUserId);

	//俱乐部性质
	Lint getClubId()
	{
		return m_id;
	}
	Lint getClubSq()
	{
		return m_sq;
	}
	Lstring getClubName() { return m_name; }
	//俱乐部id 和名称
	clubInfo& getClubInfo()
	{
		clubInfo  info(getClubId());
		info.m_name = getClubName();
		info.m_clubType = getClubType();
		return info;
	}
	///付费类型
	Lint getClubFee()
	{
		return m_feeType;
	}
	///房卡
	Lint  getCard() { return m_coin; }
	void setCard(Lint coin) { m_coin = coin; }
	///俱乐部性质
	Lint getClubType() { return m_clubType; }
	void setClubType(Lint clubType) { 
		m_clubType = clubType; 
		if (m_clubType == 1)  //如果改为竞技场，最小进入房间分数100
		{
			gClubPlayManager.updatePlayType(getCurrPlayId(), 100, 1);
		}
	
	}
	///管理员权限
	Lint getAdminCanChange() { return m_adminAuth.m_changePoint ; }
	void setAdminCanChange(Lint info) { m_adminAuth.m_changePoint = info; }

	Lint getAdminCanSeeAll() { return m_adminAuth.m_lookupPoint; }
	void setAdminCanSeeAll(Lint info) { m_adminAuth.m_lookupPoint = info; }

	//当前玩法
	bool  havePlayId()
	{
		return !m_showPlayType.empty();
	}
	Lint getCurrPlayId()
	{
		if (m_showPlayType.empty())return 0;
		return *(m_showPlayType.begin());
	}
	////////////////////////////////////////////////////////////////////////
	//获取玩法桌子列表
	std::list<Lint>  getClubDeskList();                                                      //获取当前俱乐部展示桌子的列表
	Lint getCurrDeskCount() { return getClubDeskList().size(); }
	bool ChangeShowDesk(Lint deskCount);                                        //改变当前展示桌子的个数
	Lint  getNotEmptyDeskCount();
	
private:
	//添加桌子，返回showdeskId ，如果返回值为0 ，加入失败
	Lint insertClubDesk(Lint clubDesk)
	{
		for (auto ItDesk = m_map2ClubDesk.begin(); ItDesk != m_map2ClubDesk.end(); ItDesk++)
		{
			if (ItDesk->second == clubDesk)return 0;
		}
		Lint key = m_map2ClubDesk.size()+1;
		m_map2ClubDesk[key] = clubDesk;
		m_showDeskList.insert(key);
		return key;
	}


////////////////////////////////////////////////////////////////////////
//会长和管理员已经权限
private:
	Lint   _setAdmin(Lint type, Lint setUserId, Lint adminUserId);
	Lint  _delAdmin(Lint type, Lint setUserId, Lint adminUserId);


/////////////////////////////////////////////////////////////////////////
//对能量值处理  -暂未使用，直接从数据库查询，后续优化效率可以用
public:
	void  LogPowerRecord(Lint type,boost::shared_ptr<UserPowerRecord> change);
	void  delPowerRecord();

	//能否查询能量列表  0==不能   1==都可以   2==只能看自己
	Lint  canPowPointList(Lint userId)
	{
		if (userId == getClubOwerId())return 1;
		if (m_adminUser.find(userId) == m_adminUser.end())return 0;
		if ( m_adminAuth.m_changePoint==1 &&  m_adminUser.find(userId) != m_adminUser.end())	return 1;
		if (m_adminAuth.m_lookupPoint == 1 && m_adminUser.find(userId) != m_adminUser.end())return 1;
		if (m_adminAuth.m_changePoint == 0 && m_adminAuth.m_lookupPoint == 0 && m_adminUser.find(userId) != m_adminUser.end())return 2;
		return 0;
	}
	//能否改变用户能量
	bool canChangePoint(Lint userId)
	{
		if (userId == getClubOwerId())return true;
		if (m_adminAuth.m_changePoint== 1)
		{
			if (m_adminUser.find(userId) != m_adminUser.end())
			{
				return true;
			}
		}
		return false;
	}
	//获取记录页面的类型
	Lint  getRecordMode(Lint userId)
	{
		if (userId == getClubOwerId())
			return 3;
		if (m_adminAuth.m_lookupPoint == 1 && m_adminUser.find(userId) != m_adminUser.end())
			return 3;
		if (m_adminAuth.m_changePoint == 1 && m_adminUser.find(userId) != m_adminUser.end())
			return 3;
		if (m_adminAuth.m_lookupPoint == 0 && m_adminUser.find(userId) != m_adminUser.end())
			return 2;
		return 1;
	}

	//获取用户能量能进记录
	std::list<boost::shared_ptr<UserPowerRecord> >   getUserRecord(Lint userId,Lint type= CHANGE_POINT_TYPE_ADD)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		if (CHANGE_POINT_TYPE_ADD == type)
		{
			record = _getUserAddRecord(userId);
		}
		else if (CHANGE_POINT_TYPE_REDUCE == type)
		{
			record = _getUserReduceRecord(userId);
		}
		return record;
	}
	//获取用户操作能量的变化
	std::list<boost::shared_ptr<UserPowerRecord> > getUserOperRecord(Lint userId, Lint type = CHANGE_POINT_TYPE_ADD)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		if (CHANGE_POINT_TYPE_ADD == type)
		{
			record = _getUserOperAddRecord(userId);
		}
		else if (CHANGE_POINT_TYPE_REDUCE == type)
		{
			record = _getUserOperReduceRecord(userId);
		}
		return record;
	}

	std::list<boost::shared_ptr<UserPowerRecord> > getClubOperRecord(Lint type = CHANGE_POINT_TYPE_ADD)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		if (CHANGE_POINT_TYPE_ADD == type)
		{
			record= m_PowerAddRecord;
		}
		else if (CHANGE_POINT_TYPE_REDUCE == type)
		{
			record= m_PowerReduceRecord;
		}
		return record;
	}

private:
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserAddRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserRecordAdded.find(userId);
		if (ItRecord != m_mapUserRecordAdded.end())
		{
			record = ItRecord->second;
		}
		return record;
	}
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserReduceRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserRecordReduced.find(userId);
		if (ItRecord != m_mapUserRecordReduced.end())
		{
			record = ItRecord->second;
		}
		return record;
	}
	
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserOperAddRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserAddOperRecord.find(userId);
		if (ItRecord != m_mapUserAddOperRecord.end())
		{
			record = ItRecord->second;
		}
		return record;
	}
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserOperReduceRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserReduceOperRecord.find(userId);
		if (ItRecord != m_mapUserReduceOperRecord.end())
		{
			record = ItRecord->second;
		}
		return record;
	}




	//内部用的函数
private:
	void _clearData();
	//客户端显示桌号的规则
	Lint  getShowDeskId(Lint sq);



private:
	//俱乐部属性
	Lint m_sq;                                              //俱乐部在数据库的id
	Lint m_id;                                             // 俱乐部ID
	Lstring m_name;                                //俱乐部名字
	Lint m_coin;                                        //？？会长金币  有啥用

	Lint m_ownerId;                               // 会长ID
	Lstring m_ownerNike;                    //会长昵称
	Lstring m_ownerHeadUrl;             //会长头像
	Lstring m_ownerUUioid;                 //会长

	std::set<Lint>  m_adminUser;            //管理员

	Lint m_userSum;                               //干啥用户？？？？
	Lint m_onLineUserCount;            //俱乐部在线人数
	Lstring m_clubUrl;                          //俱乐部头像url
	Lint m_status;                                  //俱乐部状态   
	Lint m_type;                                     //俱乐部类型
	Lint m_feeType;                              //俱乐部收费类型
	Lint m_valid;                                    //0-未启用状态, 1-启用状态
	Lint m_maxMedalPointId;							//俱乐部中最大赢的能量值的玩家ID
	Lint m_maxMedalPoint;							//俱乐部最大的赢的能量值分数

	Lint m_clubType;                      //俱乐部性质   //类型，0-普通   1-竞技场
	//Lint m_adminCanChange;      // // 管理员可以调整能量   0-没有选择  1-选择
	//Lint m_adminCanSeeAll;     ////管理员可以查询所有能量变化原因  0-没有  1-选择

	ClubAdminAuth  m_adminAuth;         //管理员权限

	std::set<Lint>     m_showDeskList;                       //当前显示的桌子数量 value = showdeskId
	std::map<Lint, Lint>      m_map2ClubDesk;          //key=showdeskId ,value=clubDeskId;
	//std::list<Lint>    m_clubDeskList;                             //俱乐部桌子

	////俱乐部玩法
	std::set<Lint>                                                          m_playType;                            //所有的玩法，隐藏+显示
	std::set<Lint>                                                       m_showPlayType;                 //允许展示玩法                 value=playTypeId   允许展示的玩法

	
	//能量充值
	std::list<boost::shared_ptr<UserPowerRecord> >   m_PowerAddRecord;              //加记录
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserRecordAdded;   //用户被加能量记录
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserAddOperRecord;       //用户加能量记录   管理员 和会长

	std::list<boost::shared_ptr<UserPowerRecord> >   m_PowerReduceRecord;        //减记录
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserRecordReduced;   //用户被减能量记录
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserReduceOperRecord;       //用户减能量记录   管理员 和会长

	//红包
	Red_Packet                        m_redPacket;


};



#endif