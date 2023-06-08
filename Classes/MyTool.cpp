#include "MyTool.h"

static std::default_random_engine myToolsRandom(time(NULL));
static std::uniform_int_distribution<int> myRand_Percent(0,99);

//取得系统时间的秒数  ---- get sysem of second 
int MyTool::GetSecond()
{
	struct timeval newTime;
	gettimeofday(&newTime, NULL);
	return newTime.tv_sec;
}

//取得系统时间的毫秒  ---- get sysem of milli second 
long MyTool::GetMilliSecond()
{
	struct timeval newTime;
	gettimeofday(&newTime, NULL);
	return (newTime.tv_sec * 1000 + newTime.tv_usec / 1000);
}

//取得随机整数
int MyTool::GetIntRandom(const int p_nMin, const int p_nMax)
{
	std::uniform_int_distribution<int> myRand(p_nMin, p_nMax);
	return myRand(myToolsRandom);
}

//取得0~99的随机整数
int MyTool::GetIntRandom_Percent()
{
	return myRand_Percent(myToolsRandom);
}

//根据概率返回真假
bool MyTool::GetRandomBool_Percent_True(const int p_nProbability)
{
	if (GetIntRandom_Percent() < p_nProbability)
	{
		return true;
	}
	else
	{
		return false;
	}
}