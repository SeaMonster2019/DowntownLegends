#pragma once
#include<random>
#include "cocos2d.h"

USING_NS_CC;
#define DEBUG_MODE 1

namespace MyTool
{ 
	
	int GetSecond();//取得系统秒数

	long GetMilliSecond();//取得系统毫秒数

	int GetIntRandom(const int p_nMin,const int p_nMax);//获取随机整数数

	int GetIntRandom_Percent(); //获取0~99的随机整数

	bool GetRandomBool_Percent_True(const int p_nProbability);//根据概率返回真假

}
