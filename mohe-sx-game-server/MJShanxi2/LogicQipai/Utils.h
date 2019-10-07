#pragma once

//#include "Card.h"
#include "LVideo.h"

//// 避免std::equal在CheckShiSanYiao中的警告
//#ifndef _SCL_SECURE_NO_WARNINGS
//#define _SCL_SECURE_NO_WARNINGS
//#endif
#include <algorithm>


//#include "malgo.h"


// C++11 或者 msvc 都支持 override 关键字
// https://msdn.microsoft.com/en-us/library/41w3sh1c%28VS.80%29.aspx
#if !defined(override) && __cplusplus <= 199711 && !defined(_MSC_VER)
#define override
#endif



//inline tile_t CardToTile(Card const &card) { return (tile_t)(card.m_color * 10 + card.m_number); }
//inline tile_t CardToTile(Card const *card) { return card ? CardToTile(*card) : 0; }
//inline Card TileToCard(tile_t tile) { return Card(tile_to_number(tile), tile_to_color(tile)); }
//inline Card ToCard(tile_t tile) { return Card(tile_to_number(tile), tile_to_color(tile)); }
//inline Card ToCard(Card *card) { return card ? *card : Card(); }
//
//inline tile_t ToTile(CardValue const &card) { return (tile_t)(card.m_color * 10 + card.m_number); }
//inline tile_t ToTile(Card const &card) { return (tile_t)(card.m_color * 10 + card.m_number); }
//inline tile_t ToTile(Card const *card) { return card ? CardToTile(*card) : 0; }

//inline CardValue ToCardValue(const Card& card) {
//	CardValue v;
//	v.m_color = card.m_color;
//	v.m_number = card.m_number;
//	return v;
//}
//inline CardValue ToCardValue(const Card* card) {
//	if (card) return ToCardValue(*card);
//	else return CardValue();
//}
//inline CardValue ToCardValue(tile_t tile) { return ToCardValue(TileToCard(tile)); }
//
//inline Card ToCard(const CardValue& v) {
//	Card card;
//	card.m_color = v.m_color;
//	card.m_number = v.m_number;
//	return card;
//}
//
//inline void AppendToTileVec(TileVec &lhs, CardVector const &rhs) {
//	for (size_t i = 0; i < rhs.size(); ++i) lhs.add(CardToTile(rhs[i]));
//}
//inline void OverwriteTileVec(TileVec &lhs, CardVector const &rhs) {
//	lhs.clear();
//	AppendToTileVec(lhs, rhs);
//}
//
//inline void VecExtend(std::vector<CardValue> &lhs, CardVector const &rhs) {
//	for (size_t i = 0; i < rhs.size(); ++i) lhs.push_back(ToCardValue(rhs[i]));
//}
//
//inline void VecExtend(CardValue *lhs, CardVector const &rhs) {
//	for (size_t i = 0; i < rhs.size(); ++i) lhs[i] = ToCardValue(rhs[i]);
//}
//
//// 用于客户端调牌
//template <size_t LN, size_t RN>
//inline size_t SetTileArray(tile_t(&lhs)[LN], Card const (&rhs)[RN]) {
//	size_t n = std::min(LN, RN);
//	size_t sum = 0;
//	for (size_t i = 0; i < n; ++i) {
//		tile_t tile = ToTile(rhs[i]);
//		// 不足13个，后面都是0
//		if (tile == 0) return sum;
//		// 错误牌值忽略掉
//		if (!tile_is_valid(tile)) continue;
//		lhs[sum++] = tile;
//	}
//	return sum;
//}



//inline bool operator ==(const Card* lhs, const CardValue& rhs) { return lhs->m_color == rhs.m_color && lhs->m_number == rhs.m_number; }
//inline bool operator ==(const CardValue& lhs, const Card* rhs) { return lhs.m_color == rhs->m_color && lhs.m_number == rhs->m_number; }
//inline bool operator !=(const Card* lhs, const CardValue& rhs) { return !(lhs == rhs); }
//inline bool operator !=(const CardValue& lhs, const Card* rhs) { return !(lhs == rhs); }
//
//inline bool operator ==(const TingUnit& lhs, const CardValue& rhs) { return lhs.out_card == rhs; }
//inline bool operator ==(const CardValue& lhs, const TingUnit& rhs) { return lhs == rhs.out_card; }
//inline bool operator !=(const TingUnit& lhs, const CardValue& rhs) { return !(lhs == rhs); }
//inline bool operator !=(const CardValue& lhs, const TingUnit& rhs) { return !(lhs == rhs); }




template <class T, class X>
inline bool VecHas(const T& vec, const X& x) {
	typename T::const_iterator it = std::find(vec.begin(), vec.end(), x);
	return it != vec.end();
}

template <class T, class X>
inline size_t VecGetPos(const T& vec, const X& x) {
	typename T::const_iterator it = std::find(vec.begin(), vec.end(), x);
	if (it != vec.end()) return it - vec.begin();
	return -1;
}

template <class T, class X>
inline const typename T::value_type* VecGetPtr(const T& vec, const X& x) {
	typename T::const_iterator it = std::find(vec.begin(), vec.end(), x);
	if (it != vec.end()) return &*it;
	return NULL;
}

template <class T, class X>
inline typename T::value_type* VecGetPtr(T& vec, const X& x) {
	typename T::iterator it = std::find(vec.begin(), vec.end(), x);
	if (it != vec.end()) return &*it;
	return NULL;
}

template <class T, class X>
inline void VecExtend(T &lhs, X const &rhs) {
	lhs.insert(lhs.end(), rhs.begin(), rhs.end());
}

template <class T>
inline typename T::value_type VecFront(T const &lhs) {
	return lhs.empty() ? typename T::value_type() : lhs.front();
}

template <class T>
inline typename T::value_type &VecAdd(T &lhs) {
	lhs.push_back(typename T::value_type());
	return lhs.back();
}

template <class T>
inline void VecErase(T &lhs, size_t i) {
	if (i >= lhs.size()) return;
	lhs.erase(lhs.begin() + i);
}








