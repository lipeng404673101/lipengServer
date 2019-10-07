#pragma once





typedef unsigned char tile_t;


typedef unsigned long long tile_mask_t;
enum {
	TILE_MIN = 11,
	TILE_MAX = 47,
};

#define TILE_TO_MASK(v) (1ull << (v))
#define TILE_INV_MASK(v) (~TILE_TO_MASK(v))
#define TILE_TO_MASK2(v1, v2) (TILE_TO_MASK(v1) | TILE_TO_MASK(v2))
#define TILE_TO_MASK4(v1, v2, v3, v4) (TILE_TO_MASK2(v1, v2) | TILE_TO_MASK2(v3, v4))
#define TILE_TO_MASK6(v1, v2, v3, v4, v5, v6) (TILE_TO_MASK2(v1, v2) | TILE_TO_MASK4(v3, v4, v5, v6))
#define TILE_TO_MASK7(v1, v2, v3, v4, v5, v6, v7) (TILE_TO_MASK6(v1, v2, v3, v4, v5, v6) | TILE_TO_MASK(v7))
#define TILE_BIND_MASK(v1, v2) ((~0ull << (v1)) & ~(~0ull << ((v2) + 1)))

// 剔除低端11个，保留低端48个即[0, 47]，剔除20、30、40，最终得到34张牌的mask
#define TILE_MASK_VALID		(TILE_BIND_MASK(11, 47) & TILE_INV_MASK(20) & TILE_INV_MASK(30) & TILE_INV_MASK(40))
#define TILE_MASK_SHUPAI	(TILE_BIND_MASK(11, 19) | TILE_BIND_MASK(21, 29) | TILE_BIND_MASK(31, 39))
#define TILE_MASK_SHUPAI39	(TILE_BIND_MASK(13, 19) | TILE_BIND_MASK(23, 29) | TILE_BIND_MASK(33, 39))
#define TILE_MASK_SHUPAI28	(TILE_BIND_MASK(12, 18) | TILE_BIND_MASK(22, 28) | TILE_BIND_MASK(32, 38))
#define TILE_MASK_SHUPAI17	(TILE_BIND_MASK(11, 17) | TILE_BIND_MASK(21, 27) | TILE_BIND_MASK(31, 37))
#define TILE_MASK_SHUPAI29	(TILE_BIND_MASK(12, 19) | TILE_BIND_MASK(22, 29) | TILE_BIND_MASK(32, 39))
#define TILE_MASK_SHUPAI18	(TILE_BIND_MASK(11, 18) | TILE_BIND_MASK(21, 28) | TILE_BIND_MASK(31, 38))
#define TILE_MASK_SHISANYAO	(TILE_TO_MASK6(11, 19, 21, 29, 31, 39) | TILE_TO_MASK7(41, 42, 43, 44, 45, 46, 47))
#define TILE_MASK_ZIPAI		TILE_BIND_MASK(41, 47)
#define TILE_MASK_WAN		TILE_BIND_MASK(11, 19)
#define TILE_MASK_TONG		TILE_BIND_MASK(21, 29)
#define TILE_MASK_TIAO		TILE_BIND_MASK(31, 39)
#define TILE_MASK_Feng		TILE_BIND_MASK(41, 44)
#define TILE_MASK_Jian		TILE_BIND_MASK(45, 47)
// 抠点只能胡点数6以上的牌，字牌按10点算
#define TILE_MASK_KouDian_ShouPao	(TILE_BIND_MASK(16, 19) | TILE_BIND_MASK(26, 29) | TILE_BIND_MASK(36, 39) | TILE_BIND_MASK(41, 47))
#define TILE_MASK_KouDian_ZiMo		(TILE_BIND_MASK(13, 19) | TILE_BIND_MASK(23, 29) | TILE_BIND_MASK(33, 39) | TILE_BIND_MASK(41, 47))
#define TILE_MASK_DuanYao			(TILE_BIND_MASK(12, 18) | TILE_BIND_MASK(22, 28) | TILE_BIND_MASK(32, 38))

inline bool tile_is_valid(tile_t v) { return (TILE_TO_MASK(v) & TILE_MASK_VALID) != 0; }
// 11-19万 21-29筒 31-39条 41-47东南西北中发白 总共34种牌
inline bool tile_is_zipai(tile_t v) { return 41 <= v && v <= 47; }
// 我们假设不可能出现20、30、40这几个值
inline bool tile_is_shupai(tile_t v) { return 11 <= v && v <= 39; }
inline bool tile_is_JianPai(tile_t v) { return 45 <= v && v <= 47; }
enum TILE_COLOR {
	TILE_COLOR_NULL = 0,
	TILE_COLOR_MIN = 1,
	TILE_COLOR_WAN = 1,
	TILE_COLOR_TONG = 2,
	TILE_COLOR_TIAO = 3,
	TILE_COLOR_ZI = 4,
	TILE_COLOR_MAX = 4,
	TILE_COLOR_COUNT = 4,
};
inline TILE_COLOR tile_to_color(tile_t v) { return (TILE_COLOR)(v / 10); }
inline unsigned tile_color_index(tile_t v) { return ((v / 10) - 1) & 0x3; }
inline unsigned tile_to_number(tile_t v) { return v % 10; }
inline unsigned tile_to_points(tile_t v) { return v >= 41 ? 10 : v % 10; }
inline tile_t tile_make(size_t color, size_t number) { return (tile_t)(color * 10 + number); }

enum {
	TILE_NUMBER_MIN = 1,
	TILE_NUMBER_MAX = 9,
	TILE_NUMBER_COUNT = 9,
};

inline tile_mask_t tile_color_to_mask(int tc) {
	switch (tc) {
	case TILE_COLOR_WAN: return TILE_MASK_WAN;
	case TILE_COLOR_TONG: return TILE_MASK_TONG;
	case TILE_COLOR_TIAO: return TILE_MASK_TIAO;
	case TILE_COLOR_ZI: return TILE_MASK_ZIPAI;
	default: return 0;
	}
}

inline bool tile_mask_is_subset_of(tile_mask_t subset, tile_mask_t bigset) {
	return (subset & ~bigset) == 0;
}




