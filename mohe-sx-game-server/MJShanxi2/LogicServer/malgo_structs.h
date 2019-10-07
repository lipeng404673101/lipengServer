#pragma once



/*
error C4996 : 'std::_Copy_backward' : Function call with parameters that may be unsafe -
this call relies on the caller to check that the passed values are correct.
To disable this warning, use - D_SCL_SECURE_NO_WARNINGS.See documentation on how to use Visual C++ 'Checked Iterators'
*/
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif
// 上面那种写法必须在标准库包含之前使用，可是有时候标准库已经在该文件之前被包含了
#pragma warning(disable:4996)

#include <algorithm>
#include <vector>
#include <new>
#include <ctime>


#include "malgo_tile.h"
#include "../mhmessage/mhmsghead.h"


struct Random {
	unsigned seed;
	Random(unsigned seed) : seed(seed) {}
	Random() : seed((unsigned)clock()) {}

	size_t operator ()(size_t n) { return mod((unsigned)n); }

	static Random &Instance() {
		static Random o;
		return o;
	}
	unsigned rand32() {
		unsigned a = seed = seed * 1103515245 + 12345;
		unsigned b = seed = seed * 1103515245 + 12345;
		return (a >> 16) | (b & 0xffff0000);
	}
	unsigned mod(unsigned n) {
		if (n == 0) n = 1; // 避免除零崩溃
		return rand32() % n;
	}
	// 如果传递的是空区间，那就返回first，否则就是正常的前闭后开
	unsigned range(unsigned first, unsigned last) {
		return first + mod(last - first);
	}
	unsigned minmax(unsigned a, unsigned b) {
		return a + mod(b - a + 1);
	}
	template <class T>
	typename T::value_type choice(T const &vec) {
		if (vec.empty()) return typename T::value_type(); // 避免后面崩溃
		return vec[mod(vec.size())];
	}
	template <class T, size_t N>
	T choice(T const (&a)[N]) {
		if (N == 0) return T(); // 避免后面崩溃，不过好像数组长度不能为0的
		return a[mod(N)];
	}
};

template <class T>
struct SmallBitVec {
	T mask;
	SmallBitVec() : mask(0) {}
	void set(int i) { mask |= (T)1 << i; }
	void reset(int i) { mask &= ~((T)1 << i); }
	void clear() { mask = 0; }
	bool test(int i) const { return (mask & ((T)1 << i)) != 0; }
	bool operator [](int i) const { return test(i); }
	bool empty() const { return mask == 0; }
};

template <class T, size_t N>
struct SmallVec {
	typedef T value_type;
	typedef char ASSERT_N_GT0[N > 0 ? 1 : -1];
	enum { CAPACITY = N };
	T m_a[CAPACITY];
	unsigned char m_n;
	// 全部填0对于越界访问也会有比较固定的错误结果
	SmallVec() : m_n(0) { memset(m_a, 0, sizeof(m_a)); }
	size_t size() const { return m_n; }
	bool empty() const { return 0 == m_n; }
	T const *begin() const { return m_a; };
	T const *end() const { return m_a + m_n; }
	T *begin() { return m_a; }
	T *end() { return m_a + m_n; }
	static size_t safe_index(size_t i) { return i >= N ? 0 : i; }
	T const &operator [](size_t i) const { return begin()[safe_index(i)]; }
	T &operator [](size_t i) { return begin()[safe_index(i)]; }
	// 业务逻辑上有错误没事，关键是服务器不要崩溃
	T front() const { return empty() ? T() : begin()[0]; }
	T back() const { return empty() ? T() : end()[-1]; }
	void clear() {
		memset(m_a, 0, sizeof(m_a));
		m_n = 0;
	}
	void fulfill() {
		memset(m_a, 0, sizeof(m_a));
		m_n = N;
	}

	void add(T v) {
		if (m_n >= CAPACITY) return;
		m_a[m_n] = v;
		++m_n;
	}
	void add_n(size_t n, T v) {
		size_t available = CAPACITY - m_n;
		if (n > available) n = available;
		std::fill_n(end(), n, v);
		m_n += (unsigned char)n;
	}
	void extend(T const *first, T const *last) {
		size_t n = last - first;
		size_t available = CAPACITY - m_n;
		if (n > available) n = available;
		std::copy(first, last, end());
		m_n += (unsigned char)n;
	}
	void pop_back() {
		if (m_n == 0) return;
		--m_n;
	}
	void pop_front(size_t n) {
		if (n > size()) n = size();
		std::copy(begin() + n, end(), begin());
		m_n -= (unsigned char)n;
	}

	template <class U>
	int find_pos(U const &v) const {
		T const *p = std::find(begin(), end(), v);
		if (p == end()) return -1;
		return p - begin();
	}
	template <class U>
	T const *find_ptr(U const &v) const {
		T const *p = std::find(begin(), end(), v);
		if (p == end()) return NULL;
		return p;
	}
};

enum { MAX_TILE_VEC = 14 };

struct TileVec : SmallVec<tile_t, MAX_TILE_VEC> {
};

struct SortedTileVec : SmallVec<tile_t, MAX_TILE_VEC> {
	// 最多14张牌，杠不存这里
	void add(tile_t v) {
		if (m_n >= CAPACITY) return;
		tile_t *p = std::lower_bound(begin(), end(), v);
		std::copy_backward(p, end(), p + 1);
		*p = v;
		++m_n;
	}
	void add_n(size_t n, tile_t v) {
		size_t available = CAPACITY - m_n;
		if (n > available) n = available;
		if (n == 0) return;
		tile_t *p = std::lower_bound(begin(), end(), v);
		std::copy_backward(p, end(), p + n);
		m_n += (unsigned char)n;
		while (n--) *p++ = v;
	}
	// 最多移除几个
	size_t remove_most(size_t n, tile_t v) {
		std::pair<tile_t *, tile_t *> r = std::equal_range(begin(), end(), v);
		size_t available = r.second - r.first;
		n = std::min(available, n);
		std::copy(r.first + n, end(), r.first);
		m_n -= (tile_t)n;
		return n;
	}
	// 固定移除几个，要么全部移除，要么一个都不移除
	size_t remove_fixed(size_t n, tile_t v) {
		std::pair<tile_t *, tile_t *> r = std::equal_range(begin(), end(), v);
		size_t available = r.second - r.first;
		if (available != n) return 0;
		std::copy(r.first + n, end(), r.first);
		m_n -= (tile_t)n;
		return n;
	}
	void erase(size_t i) { erase_n(i, 1); }
	SortedTileVec copied_erase(size_t i) { return copied_erase_n(i, 1); }
	void erase_n(size_t i, size_t n) {
		if (i >= size()) return;
		size_t available = size() - i;
		if (n > available) n = available;
		std::copy(begin() + i + n, end(), begin() + i);
		m_n -= (unsigned char)n;
	}
	SortedTileVec copied_erase_n(size_t i, size_t n) const {
		if (i > size()) i = size();
		size_t available = size() - i;
		if (n > available) n = available;
		SortedTileVec vec;
		std::copy(begin(), begin() + i, vec.begin());
		std::copy(begin() + i + n, end(), vec.begin() + i);
		vec.m_n = m_n - (unsigned char)n;
		return vec;
	}

	// [k, i) 这个区间都是相同的牌，计算这个区间的长度
	size_t count_backward(size_t i) const {
		if (i > size()) return 0; // 非法输入
		if (0 == i) return 0;
		--i;
		tile_t v = begin()[i];
		size_t sum = 1;
		while (i--) {
			if (begin()[i] == v) ++sum;
			else break;
		}
		return sum;
	}

	size_t count_back() const {
		return count_backward(size());
	}
	void pop_back(size_t n = 1) {
		if (n > size()) n = size();
		m_n -= (unsigned char)n;
	}

	void copy_from_sorted(tile_t const a[], size_t n) {
		if (n > CAPACITY) n = CAPACITY;
		std::copy(&a[0], &a[n], m_a);
		m_n = (unsigned char)n;
	}
	void copy_from_unsorted(tile_t const a[], size_t n) {
		copy_from_sorted(a, n);
		std::sort(begin(), end());
	}

	template <size_t N>
	void copy_from_sorted(tile_t const (&a)[N]) {
		copy_from_sorted(a, N);
	}
	template <size_t N>
	void copy_from_unsorted(tile_t const (&a)[N]) {
		copy_from_unsorted(a, N);
	}

	size_t rfind(tile_t v) const {
		size_t i = size();
		while (i--) if (begin()[i] == v) return i;
		return (size_t)-1;
	}
};





// 牌是按顺序排列的，相同的牌会聚在一起，这里从末尾向开头遍历，相同的牌作为一个遍历单元
struct TileBackwardCounting {
	SortedTileVec const &m_vec;
	size_t m_i;
	size_t m_n;
	tile_t m_tile;
	TileBackwardCounting(SortedTileVec const &vec) : m_vec(vec) {
		m_n = vec.count_back();
		m_i = vec.size() - m_n;
		m_tile = vec.empty() ? 0 : vec[m_i];
	}
	//explicit operator bool() const { return m_n != 0; }
	operator bool() const { return m_n != 0; }
	TileBackwardCounting &operator --() {
		m_n = m_vec.count_backward(m_i);
		m_i -= m_n;
		return *this;
	}
};




struct TileColorStats {
	unsigned char sums[TILE_COLOR_MAX + 1];
	TileColorStats(tile_t const a[], size_t n) {
		make(a, a + n);
	}
	TileColorStats(tile_t const *first, tile_t const *last) {
		make(first, last);
	}
	TileColorStats(SortedTileVec const &vec) {
		make(vec.begin(), vec.end());
	}
	void make(tile_t const *first, tile_t const *last) {
		memset(this, 0, sizeof(*this));
		for (; first != last; ++first) sums[tile_to_color(*first)]++;
	}
};


// 4种颜色，都按9张存，这个类给算法计算使用的，所以用unsigned存数目
struct TileZoneDivider {
	unsigned count_by_color_by_number[TILE_COLOR_COUNT][9];
	unsigned count_by_color[TILE_COLOR_COUNT];
	tile_mask_t mask;
	unsigned total;
	TileZoneDivider(tile_t const a[], size_t n) {		
		make(a, a + n);
	}
	void make(tile_t const *first, tile_t const *last) {
		memset(this, 0, sizeof(*this));
		total = (unsigned)(last - first);
		for (; first != last; ++first) {
			mask |= 1ull << *first;
			unsigned tn = tile_to_number(*first);
			if (0 == tn) continue;
			TILE_COLOR tc = tile_to_color(*first);
			count_by_color_by_number[tc - TILE_COLOR_MIN][tn - 1]++;
			count_by_color[tc - TILE_COLOR_MIN]++;
		}
	}
};



enum HUN_ITEM_KIND {
	HUN_NONE,
	HUN0_JIANG,
	HUN1_JIANG,
	HUN2_JIANG,
	HUN0_SHUN,
	HUN0_SHUN_SanFeng, // 硬三嘴：任意不一样的3个风为一个顺子
	// 硬三嘴：中发白为一个顺子，还分三元在手和三元自摸
	// 手里有2个中、2个发、1个白，最后摸到一个白，胡了，这个算2副自摸，因此只需检测最后一张牌有没有中发白就可以了
	HUN0_SHUN_SanYuan,
	HUN1_SHUN,
	HUN1_KAN,
	HUN0_KE,
	HUN1_KE,
	HUN2_FREE,
};

struct HunItem {
	HUN_ITEM_KIND kind;
	tile_t tile;
	HunItem() : kind(HUN_NONE), tile(0) {}
	HunItem(HUN_ITEM_KIND kind, tile_t tile) : kind(kind), tile(tile) {}
};

// 4面子1将子
// 计算胡牌，可能有很多解，这个就是表示其中一个解用的
struct HuBranch: SmallVec<HunItem, 7> {
	void extend(HuBranch const &rhs) {
		size_t available = CAPACITY - size();
		size_t n = rhs.size();
		if (n > available) n = available;
		std::copy(rhs.begin(), rhs.begin() + n, end());
		m_n += (unsigned char)n;
	}
};

// 计算胡牌，很多解呢，可能需要都收集起来再做处理
typedef std::vector<HuBranch> HuTree;

// 计算带万能牌的胡牌，解空间太大，可以分块表示
// 不同门的，同门下面距离>=2的分块，都是不相关的
typedef std::vector<HuTree> HuForest;




struct HuItem {
	TileVec m_tiles;
};
struct HuSet {
	std::vector<HuItem> m_items;
};


#define INVALID_POS 4
struct PengItem {
	tile_t tile;
	int fire_pos;
	PengItem() : tile(0), fire_pos(INVALID_POS) {}
	PengItem(tile_t tile, int fire_pos = INVALID_POS) : tile(tile), fire_pos(fire_pos) {}
	bool operator ==(tile_t rhs) const { return tile == rhs; }
};

struct PengVec : SmallVec<PengItem, 4> {

};

struct GangItem {
	tile_t tile; // 杠的什么牌，只存一张就行了
	int fire_pos; // 谁点的杠，INVAILD_POS表示自己摸的杠，如果来自于碰则表示谁点的碰，已经碰的3张是不可能被别人点杠的
	bool is_from_peng; // 那3张牌是不是先前碰的
	bool firer_has_ting;
	GangItem() : tile(0), fire_pos(INVALID_POS), is_from_peng(false), firer_has_ting(false) {}
	GangItem(tile_t tile, int fire_pos, bool is_from_peng, bool firer_has_ting)
		: tile(tile), fire_pos(fire_pos), is_from_peng(is_from_peng), firer_has_ting(firer_has_ting) {}
	bool operator ==(tile_t rhs) const { return tile == rhs; }
	// 碰后自摸杠：明杠
	// 非碰点杠：明杠
	// 非碰自摸杠：暗杠
	// 碰后点杠：没有这种说法，这个不算杠
	bool is_mgang() const { return fire_pos != INVALID_POS; }
	bool is_agang() const { return fire_pos == INVALID_POS; }
	bool is_zhigang() const { return fire_pos != INVALID_POS && !is_from_peng; }
};

struct GangVec : SmallVec<GangItem, 4> {

};



struct HuJudgementResult {
	// 输出结果
	FANXING fans;

	HuJudgementResult() {
		memset(this, 0, sizeof(*this));
	}
};

struct PlayerState__over_status: HuJudgementResult {
	tile_mask_t hands_mask;
	tile_mask_t all_mask;
	PlayerState__over_status() : hands_mask(0), all_mask(0) {}
	void clear() {
		this->~PlayerState__over_status();
		new (this) PlayerState__over_status;
	}
};

typedef SmallVec<unsigned char, TILE_COLOR_COUNT> pg_tile_count_by_color_t;

struct PlayerState: PlayerState__over_status {
	TileVec hands; // 不包括万能牌
	PengVec pengs;
	GangVec gangs;
	unsigned num_huns;
	tile_t last_tile;
	tile_t good_last_tile;
	unsigned char shangjin;
	bool  hongtong_yingkou; // 是否选择了硬扣胡三元牌型
	unsigned  lingchuan_loss_score; // 陵川麻将最后的分数

	PlayerState() : num_huns(0), last_tile(0), good_last_tile(0), shangjin(0), hongtong_yingkou(false), lingchuan_loss_score(0) {}

	void clear() {
		this->~PlayerState();
		new (this) PlayerState;
	}
	void make_over_status(bool is_winner, bool is_zhuang) {
		PlayerState__over_status::clear();
		if (is_winner) {
			if (is_zhuang) fans |= FAN_Zhuang;
		}
	}

	tile_mask_t make_hands_mask() const {
		tile_mask_t mask = 0;
		for (size_t i = 0; i < hands.size(); ++i) mask |= 1ull << hands[i];
		return mask;
	}
	tile_mask_t make_pengs_mask() const {
		tile_mask_t mask = 0;
		for (size_t i = 0; i < pengs.size(); ++i) mask |= 1ull << pengs[i].tile;
		return mask;
	}
	tile_mask_t make_gangs_mask() const {
		tile_mask_t mask = 0;
		for (size_t i = 0; i < gangs.size(); ++i) mask |= 1ull << gangs[i].tile;
		return mask;
	}
	tile_mask_t make_pg_mask() const {
		return make_pengs_mask() | make_gangs_mask();
	}
	tile_mask_t make_all_mask() const {
		return make_hands_mask() | make_pengs_mask() | make_gangs_mask();
	}
	pg_tile_count_by_color_t sum_pg_tile_count_by_color() {
		pg_tile_count_by_color_t counts;
		counts.fulfill();
		for (size_t i = 0; i < pengs.size(); ++i) {
			counts[tile_color_index(pengs[i].tile)] += 3;
		}
		for (size_t i = 0; i < gangs.size(); ++i) {
			if (gangs[i].is_from_peng)
			{
				counts[tile_color_index(gangs[i].tile)] += 1;
			}
			else
				counts[tile_color_index(gangs[i].tile)] += 4;

		}
		return counts;
	}
	// 门清：没有碰和明杠，有暗杠没事
	bool is_menqing() {
		if (!pengs.empty()) return false;
		for (size_t i = 0; i < gangs.size(); ++i) {
			if (gangs[i].is_agang()) continue;
			return false;
		}
		return true;
	}

	// 陵川麻将需要根据碰杠牌的记录计算出每门牌的张数
    // 胡牌前的张数按照碰杠的数量计算，胡牌后算分时明杠+1，暗杠+2	
	pg_tile_count_by_color_t sum_pg_tile_count_by_color_for_lingchuanmajiang( ) {
		pg_tile_count_by_color_t counts;
		counts.fulfill();
		for (size_t i = 0; i < pengs.size(); ++i) {
			counts[tile_color_index(pengs[i].tile)] += 3; //碰牌算3张
		}
		for (size_t i = 0; i < gangs.size(); ++i) {
		 	if (gangs[i].is_from_peng)
			{				
				counts[tile_color_index(gangs[i].tile)] += 0; //碰后补杠不加张				 
			}
			else
			{
				counts[tile_color_index(gangs[i].tile)] += 3; //明杠直杠，暗杠只算3张
			}
		}
		return counts;
	}

	// 陵川麻将计算最后的分数，明杠加1张，暗杠+2张
	// 最大门，该门手牌数量
	unsigned calc_last_score_for_lingchuanmajiang(TILE_COLOR big_men_color, unsigned hand_card_count)
	{
		unsigned score = 0;

		//玩家所有的碰牌
		for (size_t i = 0; i < pengs.size(); ++i)
		{
			if (tile_to_color(pengs[i].tile) == big_men_color)
			{
				score += 3;
			}
		}
		//玩家所有的杠牌
		for (size_t i = 0; i < gangs.size(); ++i) {
			if (gangs[i].is_mgang())
			{
				if (tile_to_color(gangs[i].tile) == big_men_color)
				{
					if (gangs[i].is_from_peng)
					{
						score += 2;
					}
					else
					{
						score += 5;
					}
				}
				else
				{
					score += 1;
				}				 	 
			}
			else if (gangs[i].is_agang())
			{
				if (tile_to_color(gangs[i].tile) == big_men_color)
				{
					score += 6;
				}
				else
				{
					score += 2;
				}
			}
		}

		return lingchuan_loss_score = (score += hand_card_count);
	}

	// 缺门：门数（除字牌外） <= 2
	static bool is_quemen(tile_mask_t mask) {
		if (0 == (mask & TILE_MASK_WAN)) return true;
		if (0 == (mask & TILE_MASK_TONG)) return true;
		if (0 == (mask & TILE_MASK_TIAO)) return true;
		return false;
	}

	static int get_quemen_count(tile_mask_t mask)
	{
		int count = 0;
		if ( 0 == (mask & TILE_MASK_WAN)) count++;
		if ( 0 == (mask & TILE_MASK_TONG)) count++;
		if ( 0 == (mask & TILE_MASK_TIAO)) count++;
		return count;
	}
};



struct TileTable {
	int black_hole;
	int counts[TILE_MAX - TILE_MIN + 1];
	TileTable() {
		memset(this, 0, sizeof(*this));
	}
	int &operator [](tile_t tile) {
		if (tile < TILE_MIN || tile > TILE_MAX) return black_hole;
		return counts[tile - TILE_MIN];
	}
	int const &operator [](tile_t tile) const {
		if (tile < TILE_MIN || tile > TILE_MAX) return black_hole;
		return counts[tile - TILE_MIN];
	}
};

enum {
	HAOZI_STYLE_NONE,
	HAOZI_STYLE_KouDian,            // 扣点随机耗子
	HAOZI_STYLE_TieJin,
	HAOZI_STYLE_HongTongWanPai,
	HAOAI_STYLE_KouDian_FengHaozi,  // 添加扣点风耗子
	HAOZI_STYLE_HongZhongHaoZi = 5,     //红中耗子
};

struct TileDealing {
	enum { PLAYER_COUNT = 4 };
	int player_count;
	tile_mask_t mask;
	size_t hand_size;
	size_t haozi_count;
	int haozi_style;
	size_t rest_size;
	TileTable desk;
	TileVec players[PLAYER_COUNT];
	SmallVec<tile_t, 34 * 4> rest;
	TileVec haozi_tiles;

	TileDealing(int player_count, tile_mask_t mask, size_t hand_size, size_t haozi_count, int haozi_style)
		: player_count(std::min((int)PLAYER_COUNT, player_count))
		, mask(mask)
		, hand_size(std::min((size_t)MAX_TILE_VEC, hand_size))
		, haozi_count(haozi_count)
		, haozi_style(haozi_style) {
		size_t total = 0;
		for (tile_t tile = TILE_MIN; tile <= TILE_MAX; ++tile) {
			if ((TILE_TO_MASK(tile) & mask) == 0) continue;
			desk[tile] = 4;
			total += 4;
		}
		rest_size = total - player_count * hand_size;
		if ((ptrdiff_t)rest_size < 0) rest_size = 0;
	}
	// 支持调牌
	void assign_to_player(int pos, tile_t const *first, tile_t const *last) {
		if (pos < 0 || pos >= player_count) return;
		TileVec &hands = players[pos];
		for (; first != last && hands.size() < hand_size; ++first) {
			tile_t tile = *first;
			if (desk[tile] <= 0) continue;
			--desk[tile];
			hands.add(tile);
		}
	}
	// 支持服务器调牌
	void assign_to_rest(tile_t const *first, tile_t const *last) {
		for (; first != last && rest.size() < rest_size; ++first) {
			tile_t tile = *first;
			if (desk[tile] <= 0) continue;
			--desk[tile];
			rest.add(tile);
		}
	}
	void finish() {
		// 展平后，desk表就没用了
		size_t assigned_count = rest.size(); // 调牌区
		for (tile_t tile = TILE_MIN; tile <= TILE_MAX; ++tile) {
			int n = desk[tile];
			if (n > 0) rest.add_n(n, tile);
		}
		// 对非调牌区做乱序处理
		
		// round 1
		//srand((unsigned int)(GetTickCount() * 123 + 234567));
		//std::random_shuffle(rest.begin()+ assigned_count, rest.end());
		//
		//// round 2
		//std::random_shuffle(rest.begin() + assigned_count, rest.end(), Random::Instance());

		// round 3
		//srand((unsigned int)(time(NULL)));
		std::random_shuffle(rest.begin() + assigned_count, rest.end());		

		//std::random_shuffle(rest.begin() + assigned_count, rest.end(), Random::Instance());

		// 利用rest尾部去补齐玩家的手牌
		for (int pos = 0; pos < player_count; ++pos) {
			_finish_player(pos);
		}
		// 发牌是从尾部开始发，所以这里逆序，好让调牌生效
		std::reverse(rest.begin(), rest.end());
	}
	void _finish_player(int pos) {
		TileVec &hands = players[pos];
		while (hands.size() < hand_size && !rest.empty()) {
			hands.add(rest.back());
			rest.pop_back();
		}
	}
	void sort_hands() {
		for (int pos = 0; pos < player_count; ++pos) {
			TileVec &hands = players[pos];
			// 手牌按顺序排列
			std::sort(hands.begin(), hands.end());
		}
	}
	void setup_haozi(tile_t const *first, tile_t const *last) {

		MHLOG("**********产生耗子牌逻辑************");
		size_t n = last - first;
		// 耗子限制为最多2个
		if (n > 2) n = 2;
		last = first + n;
		// 根据耗子数来判断是否是贴金8金
		if (n == haozi_count) {
			for (; first != last && haozi_tiles.size() < haozi_count; ++first) {
				tile_t tile = *first;
				haozi_tiles.add(tile);
			}
		} 
		else if (haozi_count == 1) {
			// 抠点、贴金的4金玩法
			if (rest.empty()) {
				haozi_tiles.add(31);
			} 
			else {
				tile_t tile = 11;
				if (HAOZI_STYLE_HongTongWanPai == haozi_style){
					tile = setup_hongtongwanpai_haozi();					
					haozi_tiles.add(tile);
				}
				else if(HAOAI_STYLE_KouDian_FengHaozi == haozi_style){
					tile = setup_KouDian_feng_haozi();
					haozi_tiles.add(tile);
				}
				//推到胡红中耗子(**Ren 2017-12-6)
				else if (HAOZI_STYLE_HongZhongHaoZi == haozi_style) {
					haozi_tiles.add(45);  //指定红中为耗子（**Ren 2017-12-7）
				}
				else{
#ifdef _MH_USE_FIX_HAO_ZI
					haozi_tiles.add(MH_HAOZI_1);
#else // _MH_USE_FIX_HAO_ZI
					haozi_tiles.add(Random::Instance().choice(rest));
#endif
				}       
			}
		} else if (haozi_count == 2) {
			std::pair<tile_t, tile_t> bajin;
			if (HAOZI_STYLE_KouDian == haozi_style) {
				// 抠点的双耗子产生规则：一到九是一副，东南西北是一副，中发白是一副，第二个耗子和第一个相邻
				// 比如白中、北东、九万一万、三万四万
				bajin = generate_KouDian_haozi2();
			} else {
				// 贴金的8金玩法
				bajin = tiejin_generate_bajin();
			}
			haozi_tiles.add(bajin.first);
			haozi_tiles.add(bajin.second);
		} else {
			// 不能识别就不给耗子
		}
	}

	/*
	单耗子：玩家摸完牌后，从牌堆尾部的第6垒牌上面那张翻出来，这张牌的下一张牌就是耗子，耗子可以作为赖子牌，充当任意牌。比如：翻出来的是7万，则耗子牌=赖子牌=鬼牌为8万。8万可以当赖子。
	万筒条耗子牌规则：1则为2、2为3、3为4.。。。。。9为1。
	东南西北：东为南、南为西、西为北、北为东。
	中发白：中为发、发为白、白为中。
	双耗子则是从玩家拿完4落牌之后，庄家跳牌，其他的玩家也拿牌，所有玩家拿完牌之后将之后的一张牌拿起来，则这张牌和这张牌的下一张都为耗子，比如抓起来的牌为7万，则7万和8万都是耗子，其他与单耗子相同
	*/
	std::pair<tile_t, tile_t> generate_KouDian_haozi2() {
		tile_t jin1, jin2;
		if (rest.empty()) {
			jin1 = 11;
		} else {
			jin1 = Random::Instance().choice(rest);
		}
		switch (jin1) {
		case 19: jin2 = 11; break;
		case 29: jin2 = 21; break;
		case 39: jin2 = 31; break;
		case 44: jin2 = 41; break;
		case 47: jin2 = 45; break;
		default: jin2 = jin1 + 1;
		}
#ifdef _MH_USE_FIX_HAO_ZI
		return std::make_pair(MH_HAOZI_1, MH_HAOZI_2);
#else 
		return std::make_pair(jin1, jin2);
#endif // _MH_USE_FIX_HAO_ZI

	}

	// 贴金：8金：系统每局随机定义2张牌为金，则该牌的8张都是金，但这两张牌必须是间隔相邻的。
	// 比如3万、5万，则4个3万和4个5万都是金；也可以3万，5筒；服务器处理时按照数字割开即可。花色不限制。
	// 特殊的金：东南西北为一副牌，中发白为一副牌，因为8斤必须间隔所以：红中白板、东风西风、南风北风都是8金，其他的不行。
	std::pair<tile_t, tile_t> tiejin_generate_bajin() {
		// 7x3x3 + 3 = 66 种可能
		size_t i = Random::Instance().mod(66);
		tile_t jin1, jin2;
		if (i == 65) jin1 = 45, jin2 = 47; // 中
		else if (i == 64) jin1 = 42, jin2 = 44; // 南
		else if (i == 63) jin1 = 41, jin2 = 43; // 东
		else {
			size_t c2 = i / 21;
			i %= 21;
			size_t c1 = i / 7;
			i %= 7;
			i += 1; // 牌值的 m_number 是 1 起始的
			jin1 = (tile_t)((c1 + 1) * 10 + i);
			jin2 = (tile_t)((c2 + 1) * 10 + i + 2);
		}
		return std::make_pair(jin1, jin2);
	}

	// 选取洪洞王牌中的混牌,这里不让产生中，发，白的混子牌
	tile_t setup_hongtongwanpai_haozi()
	{
		tile_t tile = 11;
		MHLOG("*********Setup 洪洞王牌耗子牌 setup_hongtongwanpai_haozi rest.size() = %d*****************", rest.size());
		LLOG_ERROR("*********Setup 洪洞王牌耗子牌 setup_hongtongwanpai_haozi rest.size() = %d*****************", rest.size());
		do
		{
			tile = Random::Instance().choice(rest);
			if (TILE_TO_MASK(tile) & TILE_MASK_Jian)
			{
				continue;
			}
			break;
		} while (true);
#ifdef _MH_USE_FIX_HAO_ZI
		tile = MH_HAOZI_1;
#endif // _MH_USE_FIX_HAO_ZI

		return tile;
	}

	// 生成扣点风耗子的逻辑，新版山西麻将
	tile_t setup_KouDian_feng_haozi()
	{
		tile_t tile = 11;
		tile_t tiles[4] = {41, 42, 43, 44};
		tile = Random::Instance().choice(tiles);
#ifdef _MH_USE_FIX_HAO_ZI
		tile = MH_HAOZI_1;
#endif // _MH_USE_FIX_HAO_ZI
		return tile;

	}

	void DumpDeskInfo()
	{
		MHLOG("***Dump TileDealing");
		MHLOG("***player_count: %d, reset_size: %d, hand_size:%d", player_count, rest_size, hand_size);

		Lstring  s("");
		char buf[16];
		for (int i = 0; i < PLAYER_COUNT; i++)
		{

			tile_t *  itr = players[i].begin();
			s.clear();
			for (int j = 0; j < players[i].size(); j++)
			{
				sprintf(buf, "%d ,", (int)itr[j]);
				s.append(buf);

			}
			MHLOG("***player: %d -- hand: %s", i, s.c_str());
		}

		tile_t *  it = rest.begin();
		s.clear();
		for (int j = 0; j < rest.size(); j++)
		{
			sprintf(buf, "%d ,", (int)it[j]);
			s.append(buf);

		}
		MHLOG("***rest count: %d , reset: %s", rest_size, s.c_str());
	}

	void DumpHaoziInfo()
	{
		MHLOG("***Dump TileDealing -- HaoziInfo");		
		MHLOG("*** haozi count: %d, haozi_style: %d", haozi_count, haozi_style);
	 
		Lstring  s("");
		char buf[16];
		for (int i = 0; i < haozi_tiles.size(); i++)
		{
			 
			tile_t *  itr = haozi_tiles.begin();			 
			sprintf(buf, "%d ,", (int)itr[i]);
				s.append(buf);		 
			 
		}		 
	    MHLOG("***Haozi_pai: %s",s.c_str());
	}

};










