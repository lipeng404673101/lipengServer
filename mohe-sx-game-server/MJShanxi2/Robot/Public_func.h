#pragma once
#include "LBase.h"
#include "LVideo.h"
#include "LMsg.h"
#include <string>
#include "..\LogicServer\Card.h"

void SetLog();

void SetNet();

void NetJoin();

void Net_Thread_Update() ;

std::string num2str(int i);

Lstring Md5Hex(const void* data,Lsize size);


void R_EraseCard(std::vector<Card>& src,Card* pCard,int n);
void R_EraseCard(std::vector<Card>& src,CardValue* pCard,int n);

bool IsSame(Card* c1,Card* c2);