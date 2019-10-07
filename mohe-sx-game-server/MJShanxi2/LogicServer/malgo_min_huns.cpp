

#include "malgo.h"




#define TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES 1
//用在计算最小混牌的顺子组合
struct CombinItem {
	int card[3];
	CombinItem* parent;
	CombinItem* child;
	int nKePos;
	int huns;


	CombinItem() {
		card[3] = { 0 };
		parent = NULL;
		child = NULL;
		nKePos = 999;
		huns = 0;
	}
};

struct minHuns {
	CombinItem* curMinHunsItem = NULL;
	std::vector<CombinItem*> List;
	std::vector<CombinItem*> allList;
	int time;
	unsigned nKePos;

	minHuns() {
		time = 0;
		nKePos = 999;
	}

	~minHuns()
	{
		for (int i = 0; i < allList.size(); i++) {
			if (allList[i]!= NULL) {
				delete allList[i];
				allList[i] = NULL;
			}
		}

		allList.clear();
		List.clear();
	}

	//获取混牌的数量
	int  getHunsNum(unsigned* arrFengCard, int arr_len, int& kePos)
	{
		int huns = 0;
		for (int i = 0; i < arr_len; i++)
		{
			if (arrFengCard[i] == 2) {
				huns++;
			}
			if (arrFengCard[i] == 4) {
				huns = huns + 999;
			}

			if (arrFengCard[i] == 1) {
				huns = huns + 2;
			}
			if (arrFengCard[i] == 3) {
				kePos = i;
			}
		}
		return huns;
	}

	//获取顺牌组合的数量
	void  countShunsNumAndMinHunNum(int& hunNum, int& shunNum)
	{
		//获取顺牌组合的数量
		int size = List.size();
		int random = 0;
		if (size > 1) {
			random = rand() % (size - 1);
		};
		hunNum = List[random]->huns;
		CombinItem* parent = List[random]->parent;
		while (parent) {
			shunNum++;
			parent = parent->parent;
		}
		nKePos = List[random]->nKePos;
	}

	//剩下的牌是否可以再获取顺组合
	bool judgeCanDoCombine(unsigned* arrFengCard, int num)
	{
		int leftFengSum = 0;  //剩下可以组合的牌数
		for (int i = 0; i < 4; i++) {
			if (arrFengCard[i] > 0)
				leftFengSum++;
		}
		return (num > leftFengSum ? 0 : 1);
	}

	void combine_increase(unsigned* arrFengCard, int start, CombinItem* pItem, int count, const int NUM, const int arr_len)
	{
		int i = 0;
		for (i = start; i < arr_len + 1 - count; i++)
		{
			if (arrFengCard[i] <= 0) {
				continue;
			}
			pItem->card[count - 1] = i;
			if (count - 1 == 0)
			{
				unsigned arrFengCardC[4] = {0,0,0,0};

				for (int k = 0; k < arr_len; k++) {    //做备份
					arrFengCardC[k] = arrFengCard[k];
				}

				//根据得到的组合减少每种风牌的数量
				for (int m = 0; m < NUM; m++) {
					arrFengCardC[pItem->card[m]]--;
				}

				CombinItem* resultC = new CombinItem();
				allList.push_back(resultC);
				resultC->parent = pItem;
				bool doNext = false;
				doNext = judgeCanDoCombine(arrFengCardC, 3);
				if (doNext) {
					time++;
					combine_increase(arrFengCardC, 0, resultC, 3, 3, arr_len);
				}
				else {
					//得到混牌的值，并且更新最少混牌记录
					int kePos = 999;
					int huns = getHunsNum(arrFengCardC, arr_len, kePos);
					resultC->huns = huns;
					resultC->nKePos = kePos;
					if (curMinHunsItem == NULL)
					{
						curMinHunsItem = resultC;
						List.push_back(resultC);
					}
					else {
						if (curMinHunsItem->huns > resultC->huns)
						{
							curMinHunsItem = resultC;
							List.clear();
							List.push_back(resultC);
						}
						else if (curMinHunsItem->huns == resultC->huns)
						{
							List.push_back(resultC);
						}
					}
				}
			}
			else
				combine_increase(arrFengCard, i + 1, pItem, count - 1, NUM, arr_len);
		}
		/*
		if (curMinHunsItem == NULL) {
			//得到混牌的值，并且更新最少混牌记录
			int kePos = 999;
			int huns = getHunsNum(arrFengCard, arr_len, kePos);
			pItem->huns = huns;
			pItem->nKePos = kePos;

			curMinHunsItem = pItem;
			List.push_back(pItem);

		}*/


		//得到混牌的值，并且更新最少混牌记录
		int kePos = 999;
		int huns = getHunsNum(arrFengCard, arr_len, kePos);
		pItem->huns = huns;
		pItem->nKePos = kePos;
		if (curMinHunsItem != NULL && huns < curMinHunsItem->huns) 
		{
			curMinHunsItem = pItem;
			List.clear();
			List.push_back(pItem);
		}

		if (curMinHunsItem == NULL) 
		{
			curMinHunsItem = pItem;
			List.push_back(pItem);
		}


	}
    //获取混的数量和顺的数量
	void getShunsNumAndMinHunNum(unsigned* arrFengCard, int size, int& hunNum, int& shunNum) {
		int num = 3;
		CombinItem* curCombinItem = new CombinItem();
		allList.push_back(curCombinItem);
		combine_increase(arrFengCard, 0, curCombinItem, num, num, size);
		//增加特殊情形的处理：如2,3，3，最小混应该是1
		unsigned ArrFengCardCopy[4] = { 0,0,0,0 };
		for (int k = 0; k < size; k++) {    //做备份
			ArrFengCardCopy[k] = arrFengCard[k];
		}

		for (int i = 0; i < size; i++) {
			if (ArrFengCardCopy[i] > 3) {
				ArrFengCardCopy[i] = ArrFengCardCopy[i] - 3;

				unsigned tempArrFengCard[4] = { 0,0,0,0 };
				for (int k = 0; k < size; k++) {    //做备份
					tempArrFengCard[k] = ArrFengCardCopy[k];
				}

				CombinItem* tempCurCombinItem = new CombinItem();
				allList.push_back(tempCurCombinItem);
				combine_increase(tempArrFengCard, 0, tempCurCombinItem, num, num, size);
			}
		}

		//计算混的数量和顺的数量
		countShunsNumAndMinHunNum(hunNum, shunNum);
	}
};


// 字牌只能成刻子，所以很简单
static unsigned tile_block_min_huns__zipai(unsigned const p[9], unsigned head, unsigned tail) {
	unsigned min_hun = 0;
	for (unsigned i = head; i <= tail; ++i) {
		// 4张同样的字牌，不能组顺子，组了一个刻子，还剩一张，万能牌也不能再去取这个值，所以肯定胡不了
		if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && p[i] == 4) return 999; 
		min_hun += (3 - (p[i] % 3)) % 3;
	 
	}
	return min_hun;
}

static int  tile_min_huns_num_for_HongTong(unsigned const p[9], int& hunsNum, int& shunNum, unsigned jiang_pos, tile_t last_tile, HuBranch& branch);
// 洪洞王牌检查风顺，三元，返回最小混牌数量
// 参数jiang_pos 是尝试配将对的位置，不能再配出刻子
unsigned tile_min_huns_for_hongtongwangpai_zipai_sanyuan_sanfeng(unsigned const p[9], unsigned head, unsigned tail, unsigned jiang_pos, tile_t last_tile, HuBranch & branch) {
	unsigned min_hun = 0;
	for (unsigned i = head; i <= tail; ++i) {
		// 4张同样的字牌，不能组顺子，组了一个刻子，还剩一张，万能牌也不能再去取这个值，所以肯定胡不了
		if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && p[i] >= 5) return 999;		
	}
	int shunNum;			
	int hunsNum;
	tile_min_huns_num_for_HongTong(p, hunsNum, shunNum, jiang_pos, last_tile, branch);
	min_hun = hunsNum;
	return min_hun;
}



static int  tile_min_huns_num_for_HongTong(unsigned const p[9], int& hunsNum, int& shunNum, unsigned jiang_pos, tile_t last_tile, HuBranch & branch)
{
	unsigned arrFengCard[4] = { 0 };
	for (int i = 0; i < 4; i++) {
		arrFengCard[i] = p[i];
	}


	minHuns tempMinHuns1;
	int fengHunsNum = 0;
	int fengShunNum = 0;
	//LLOG_DEBUG(" jiang_pos = %d ", jiang_pos);
	//LLOG_DEBUG(" arrFengCard = %d  %d  %d  %d ", arrFengCard[0], arrFengCard[1], arrFengCard[2], arrFengCard[3]);
	if (jiang_pos<4 && jiang_pos >=0 && arrFengCard[jiang_pos] == 3) {
		fengHunsNum = 999;
		//LLOG_DEBUG("将的位置不能有3个 改变混的值 fengHunsNum = %d  ", fengHunsNum);
	}
	else {
		tempMinHuns1.getShunsNumAndMinHunNum(arrFengCard, 4, fengHunsNum, fengShunNum);
	}
	//LLOG_DEBUG("fengHunsNum = %d   fengShunNum = %d ", fengHunsNum, fengShunNum);



	minHuns tempMinHuns2;
	int yunHunsNum = 0;
	int yunShunNum = 0;
	unsigned arrYunCard[3] = { 0 };
	for (int i = 0; i < 3; i++) {
		arrYunCard[i] = p[4+i];
	}

	//LLOG_DEBUG(" arrYunCard = %d  %d  %d ", arrYunCard[0], arrYunCard[1], arrYunCard[2]);

	if (jiang_pos != 999 && jiang_pos >= 4 && p[jiang_pos] >= 3)
	{
		yunHunsNum = 999;
	}
	else {
		tempMinHuns2.getShunsNumAndMinHunNum(arrYunCard, 3, yunHunsNum, yunShunNum);
	}

	//LLOG_DEBUG(" yunHunsNum = %d  yunShunNum = %d  ", yunHunsNum, yunShunNum);


	hunsNum = fengHunsNum + yunHunsNum;
	shunNum = fengShunNum + yunShunNum;
	for (int i = 0; i < fengShunNum;i++)
	{
		branch.add(HunItem(HUN0_SHUN_SanFeng, 41));
	}
	for (int i = 0; i < yunShunNum; i++)
	{
		branch.add(HunItem(HUN0_SHUN_SanYuan, last_tile));
	}
	//LLOG_DEBUG("branch.size = %d ",branch.size());

	return hunsNum;
}




// 经过测试，研究了一些例子，发现还是得递归去解，取最小值
static unsigned tile_block_min_huns__shupai(unsigned p[9], unsigned head, unsigned tail, unsigned jiang_pos) {
	unsigned min_hun = 0;
	// 从起始端开始消
	for (unsigned i = head; i <= tail; ++i) {
		// 先把这个位置上的所有的刻子消掉
		// 可以反证为什么这么做：假设这些刻子能节省万能牌，那它们肯定是和后面的牌组顺子用，那有分三种情况：
		//   能组1个顺子：组不组都需要一张万能牌，不组的话，后面这两张牌还可能跟再往后的组顺子用
		//   能组2个顺子：组的话，这个刻子就缺了，也得补万能牌啊，不组的话，后面的还可能跟更后面的组顺子用
		//   能组3个顺子：那也是能组3个刻子啊，都不需要补充万能牌
		p[i] = p[i] % 3;
		// 然后这个p[i]就有0、1、2这三种情况
		if (0 == p[i]) continue; // 这个位置空了，没事了
		unsigned has1 = i + 1 <= tail ? p[i + 1] : 0;
		unsigned has2 = i + 2 <= tail ? p[i + 2] : 0;
		if (1 == p[i]) {
			// 还剩一张牌，那最好去跟后面组顺子，不然得给它补两张万能牌呢
			// 彼此之间都是近距离的，除非在末端后面没牌了，不然最多补一张万能牌肯定能组出顺子
			if (has1 && has2) {
				// 能组顺子，那就消掉吧
				--p[i + 1];
				--p[i + 2];
			} else if (has1 || has2) {
				// 补一张万能牌就能组顺子了
				if (has1) --p[i + 1];
				else --p[i + 2];
				min_hun += 1;
			} else {
				// 不然得浪费两张万能牌了
				p[i] = 0;
				min_hun += 2;
			}
		} else {
			// 还剩两张牌，补1张万能牌能成刻子，也可以和后面组成2个顺子，也可以和后面组成1个顺子
			// 这三种解必须递归算，取最小值
			// 先看2个顺子这种分支的，有机会提前跳出的
			if (has1 >= 2 && has2 >= 2) {
				unsigned tmp[9];
				memcpy(tmp, p, sizeof(tmp));
				tmp[i] = 0;
				tmp[i + 1] -= 2;
				tmp[i + 2] -= 2;
				unsigned needed_hun__shunzi2_branch = tile_block_min_huns__shupai(tmp, i + 1, tail, jiang_pos);
				if (needed_hun__shunzi2_branch <= 1) {
					min_hun += needed_hun__shunzi2_branch;
					return min_hun;
				}
				// 自己补1张万能牌成刻子吧
				p[i] = 0;
				unsigned needed_hun__kezi_brance;
				// 在将的位置就不能用万能牌补刻子了
				if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && i == jiang_pos) needed_hun__kezi_brance = 999;
				else needed_hun__kezi_brance = 1 + tile_block_min_huns__shupai(p, i + 1, tail, jiang_pos);
				min_hun += std::min(needed_hun__kezi_brance, needed_hun__shunzi2_branch);
				return min_hun;
			} else if (has1 >= 1 && has2 >= 1) {
				unsigned tmp[9];
				memcpy(tmp, p, sizeof(tmp));
				tmp[i] -= 1;
				tmp[i + 1] -= 1;
				tmp[i + 2] -= 1;
				unsigned needed_hun__shunzi1_branch = tile_block_min_huns__shupai(tmp, i, tail, jiang_pos);
				if (needed_hun__shunzi1_branch <= 1) {
					min_hun += needed_hun__shunzi1_branch;
					return min_hun;
				}
				// 自己补1张万能牌成刻子吧
				p[i] = 0;
				unsigned needed_hun__kezi_brance;
				// 在将的位置就不能用万能牌补刻子了
				if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && i == jiang_pos) needed_hun__kezi_brance = 999;
				else needed_hun__kezi_brance = 1 + tile_block_min_huns__shupai(p, i + 1, tail, jiang_pos);
				min_hun += std::min(needed_hun__kezi_brance, needed_hun__shunzi1_branch);
				return min_hun;
			} else {
				min_hun += 1;
				// 相当于尾递归
			}
		}
	}
	return min_hun;
}



// 数组元素表示这个位置上有几张重复的牌，比如1万到9万9个位置
unsigned malgo_min_huns(unsigned const p[9], bool is_zipai, unsigned jiang_pos) {
	// 字牌就不用分块去算了
	if (is_zipai) return tile_block_min_huns__zipai(p, 0, 8);
	// 数牌按距离分块是为了降低分支组合，由乘法关系变成加法关系
	unsigned min_hun = 0;
	// 拆分成block，block之间至少要有2个空位
	for (unsigned i = 0; i <= 8; ) {
		if (0 == p[i]) {
			++i;
		} else {
			unsigned j = i;
			unsigned block[9] = { 0 };
			// 找到block的末尾 [i, j]
			for (;;) {
				block[j] = p[j];
				if (8 == j) break;
				if (0 == p[j + 1]) {
					if (7 == j) break;
					if (0 == p[j + 2]) break;
				}
				++j;
			}
			min_hun += tile_block_min_huns__shupai(block, i, j, jiang_pos);
			i = j + 3;
		}
	}
	return min_hun;
}

