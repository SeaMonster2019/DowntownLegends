#pragma once

#include <random>
#include <functional>
#include"CRole.h"
#include"cocos2d.h"
#include"MyTool.h"

using namespace std;
USING_NS_CC;

#define _ABS(x) ((x)>0 ? (x) : -(x)) //返回绝对值

//地图代号宏
#define MAP_PARK_LAKESIDE    0
#define MAP_PARK_MAIN_ROAD   1
#define MAP_PARK_PATH        2
#define MAP_PARK_CENTER      3
#define MAP_PARK_WC          4
#define MAP_PARK_DOORWAY     5

//行动参数宏
#define MAP_JUDGE(x,y)        (((x) & (y)) != 0 )  //提取变量中的行动限制参数
#define MAP_GET_MAPMACRO(x)  ((x) & 0x00ff)      //提取变量中的地图宏定义信息
#define CAN_NOT_UP           0x0001             //*00000001(2)不能向上移动
#define CAN_NOT_DOWN         0x0002            //*00000010(2)不能向下移动
#define CAN_NOT_RIGHT        0x0004          //*00001000(2)不能向右移动
#define CAN_NOT_LEFT         0x0008           //*00000100(2)不能向左移动
#define MAP_JUDGE_DROWNING   0x0010         //*00010000(2)溺水
#define MAP_JUDGE_CAN_JUMP   0x0020        //*00100000(2)可以跳上屋顶
#define MAP_CHANGE           0x0300       //00000011*(2)地图切换
#define MAP_CHANGE_IN        0x0100      //00000010*(2)进入到下一张地图
#define MAP_CHANGE_OUT       0x0200     //00000001*(2)退出到上一张地图   

//单例类获取地图对象宏
#define MAP_TOOLS CMapTools::getInstance()

class CRole;

//地图信息
struct SMapInformation
{
	bool bExistenceBoss;    //地图是否存在BOSS
	bool bIsGoIn;          //前进或退后进入地图的方式
	bool bIsSafetyZone;   //是否是安全区
	int nBeforMapNumber;        //上一张地图编号
	int nNextMapNumber;        //下一张地图编号
	int nMapNumber;           //地图编号
	int nCampNumber[4];      //阵营角色编号
	float fMapSizeWidth;        //屏幕长度
	float fMapSizeHeight;      //屏幕高度
	float fViewEdgeLeft;      //左顶点
	float fViewEdgeRight;    //右顶点
};


//单例地图类
class CMapTools
{
public:

	//地图加载函数
	Sprite* mapLoding(Node *p_node,const int p_nMapName);
	short getLodingParameter(const short p_nLimit);

	//地图边界判断函数

	short mapJudge(Vec3* const p_Rolevec3, const int p_nDirection);

	bool pathDetection(Vec3* const p_vec3, const int p_nDirection, const float p_fDistance);


	//镜头拉伸函数，每帧调用
	void lensStretch();

	//获取地图信息函数
	SMapInformation& const getMapIformation();

	//生成角色
	void roleRoding(const short p_nRodingType);
	void roleRodingBegin_Badman();
	bool roleRodingOnce_Badman(const int p_nVecRoleNumber);


	//各种地图函数
	short MapBoundaryNull(Vec3* const  p_vec3, const int p_nDirection);// 空地图
	//公园
	short MapBoundaryJudge_Park0(Vec3* const  p_vec3, const int p_nDirection);
	short MapBoundaryJudge_Park1(Vec3* const  p_vec3, const int p_nDirection);
	short MapBoundaryJudge_Park2(Vec3* const  p_vec3, const int p_nDirection);
	short MapBoundaryJudge_Park3(Vec3* const  p_vec3, const int p_nDirection);
	short MapBoundaryJudge_Park4(Vec3* const  p_vec3, const int p_nDirection);
	short MapBoundaryJudge_Park5(Vec3* const  p_vec3, const int p_nDirection);



public:

	static CMapTools& getInstance()
	{
		static CMapTools instance;
		return instance;
	}

private:
	
	//地图信息
	SMapInformation  struMapInformation;

	//地图判断回调函数指针
	short (CMapTools::* mapJudgeCallBack)(Vec3* const p_vec3, const int p_nDirection);

	CMapTools() { mapJudgeCallBack = &CMapTools::MapBoundaryNull;};
	~CMapTools() { };
	CMapTools(const CMapTools&);
	CMapTools& operator=(const CMapTools&);

	//角色出生坐标表
	const Vec2 RoleLodingSite_LeadTeam[2][8] =
	{
		{Vec2(180,40),Vec2(170,40),Vec2(160,40),Vec2(150,40),},
		{Vec2(110,150),Vec2(100,150),Vec2(90,150),Vec2(80,150),Vec2(2120,60),Vec2(2300,130),Vec2(2300,60),Vec2(2300,10),}
	};
	const Vec2 RoleLodingSite_BadMan[2][5] =
	{
		{Vec2(700, 130), Vec2(840, 130), Vec2(700, 30), Vec2(840, 30), Vec2(930, 80),},
		{Vec2(-100, 90), Vec2(100, 20), Vec2(2500, 90), Vec2(2500, 20) , Vec2(0, 0)}
	};
};               

