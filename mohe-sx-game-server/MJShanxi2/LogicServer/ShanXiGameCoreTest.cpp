
#include "ShanXiFeatureToggles.h"

#include <cassert>
#include <iostream>



/*
设计一种测试的写法，针对 ShanXiGameCore 进行麻将玩法实现的测试
设想用一堆数字即可表示这样一个测试，需要表示玩法范围、小选项范围、调牌、走法、结算番型、结算分数

11-19万 21-29筒 31-39条 41-47字
0-3玩家位置
290 0碰 291 1碰
180 0暗杠或者补杠
280 0明杠
171 1听
263 3过
150 0自摸胡
250 0收炮胡
330 0的分数和番型 -1结尾
310 0的分数
666 其它玩家自动走，直到当前玩家可以操作
999 所有玩家自动走，直到结束
444 测试起始符，带行号参数
441 包括哪些玩法 -1结尾
440 排除哪些玩法 -1结尾
451 选中哪些选项 -1结尾
450 不选中哪些选项 -1结尾
449 测试终止符
880 0调牌  881 1调牌  884桌子调牌 885耗子调牌 遇到指令自动结尾


*/
struct ShanXiGameCoreTestParser {
	enum { MAX_PLAYER_COUNT = 4 };

	struct ShanXiGameCoreTestCase {
		std::vector<tile_t> player_assigned[MAX_PLAYER_COUNT];
		std::vector<tile_t> desk_assigned;
		std::vector<tile_t> haozi_assigned;
	};

	std::vector<ShanXiGameCoreTestCase> tests;
	ShanXiGameCoreTestCase *test;
	int const *par_p;
	int const *par_q;

	template <size_t N>
	ShanXiGameCoreTestParser(int const (&data)[N]) {
		parse_data(data, N);
	}
	void parse_data(int const *p, size_t n) {
		par_p = p;
		par_q = p + n;
		try {
			for (;;) {
				if (par_p >= par_q) break;
				int v = *par_p++;
				on_parser_op(v);
			}
		} catch (char const *err) {
			std::cout << "parser error: " << err << std::endl;
		}
	}
	void on_parser_op(int op) {
		switch (op) {
		case 444: return parser_op_new_test();
		case 449: return parser_op_finish_test();
		case 880: case 881: case 882: case 883: return parser_op_assign_player(op % 10);
		}
	}

	void parser_op_new_test() {
		if (test) throw "cannot new test before finish current test";
		tests.push_back(ShanXiGameCoreTestCase());
		test = &tests.back();
	}
	void parser_op_finish_test() {
		if (NULL == test) throw "there is no test to finish";
		test = NULL;
	}
	void check_test_available() {
		if (NULL == test) throw "there is no test to build";
	}
	void read_tiles(std::vector<tile_t> &out) {

	}
	void parser_op_assign_player(int pos) {
		check_test_available();
		std::vector<tile_t> &out = test->player_assigned[pos];
		read_tiles(out);
	}
};


#if defined(_WIN32)
struct ShanXiGameCoreTestStartup {
	ShanXiGameCoreTestStartup() {
		std::cout << "Hello ShanXiGameCoreTestStartup" << std::endl;
		static int const data[] = {
			444, __LINE__, 444, __LINE__,
		};
		ShanXiGameCoreTestParser par(data);
	}
};
static ShanXiGameCoreTestStartup g_ShanXiGameCoreTestStartup;
#endif

