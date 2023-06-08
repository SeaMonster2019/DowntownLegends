#pragma once
#include <string>
#include <random>
#include <ctime>
#include <functional>
#include "cocos2d.h"
#include "CRole.h"
#include "jsonclass.h"
#include "MyTool.h"
#include "CMapTools.h"
#include "CEffects.h"
#include "IUiInterface.h"


USING_NS_CC;

class CRole;
class CMapTools;

class CUpMenu;


#define MAX_KEY_MONITOR_AMOUNT   2
      
#define lead (*CRole::vecRoles[0]).at(0)     

#define npc1 (*CRole::vecRoles[0]).at(1)     


enum UsingKey
{
    KEY_UP_DOWN,
    KEY_LEFT_RIGHT,
    KEY_Z,
    KEY_X,
    KEY_C
};


struct StructDoubleClickFlg
{
    long lCurrentTime;
    long lLastTime;
    EventKeyboard::KeyCode EkbKetCode;
    bool bKeyDoublePressFlg;
};


class CBattleScene : public cocos2d::Scene
{
//元素
public:

    short int  MoveKeyState[MAX_KEY_MONITOR_AMOUNT]; //按键状态
    short int  NPC_MoveKeyState[MAX_KEY_MONITOR_AMOUNT]; //按键状态

#if NPC1_CONTROL
    short int  NPC_MoveKeyState[2];//NPC按键移动状态
#endif

    Size visibleSize;//桌面大小
    Vec2 origin;//原点坐标

    Sprite* mapSprite; //地图精灵

    CUpMenu* upMenu; //上菜单精灵
 
    Sprite* _downMuen;//下菜单精灵

private:

    StructDoubleClickFlg  _structDoubleClickFlg;//用于判断是否双击按键的标志
    StructDoubleClickFlg _structDoubleClickFlg_NPC;
#if NPC1_CONTROL
    StructDoubleClickFlg  _structDoubleClickFlg_NPC;//用于控制NPC时判断是否双击按键的标志
#endif

    EventListenerTouchOneByOne* listenerMouse;
    EventListenerKeyboard* listenerkeyPad;

//接口
public:

    static CBattleScene* getInstance();//获取战斗场景单例类指针

    virtual bool init() override;//继承的场景初始化

    void generateCover(CRole* p_Role);//为某一角色生成遮盖，
    void gameOver();//游戏结束

    void resetGame();//重新开始战斗场景

    void changeMap(const int p_nMapType);//转换地图

    void onKeyPressed(EventKeyboard::KeyCode keycode, Event * event);//在战斗场景时，按下按键的调度器回调函数
    void onKeyReleased(EventKeyboard::KeyCode keycode, Event* event);//在战斗场景时，释放按键的调度器回调函数

    bool onTouchBegan(Touch* touch, Event* unused_event);//点击屏幕的调度器函数，用来测试时找到某一点的相对坐标，下同
    void onTouchMoved(Touch* touch, Event* unused_event);
    void onTouchEnded(Touch* touch, Event* unused_event);

    bool sceneSetInit();//地图初始化
    bool spiteInit();//对角色类初始化
    bool battleSceneClear();//清除地图内的所有东西

    CREATE_FUNC(CBattleScene);

private:
    
    virtual void _OnCBattleSceneUpdateA(float s);//关于时间的调度器回调函数
    virtual void _OnCBattleSceneUpdataB(float s);//关于时间的调度器回调函数
    virtual void update(float s);//关于帧的调度器回调函数
    void resumControl();//回复控制的调度器
    void stopControl();//停止所有控制的调度器

//保持单例运行
private:
    CBattleScene(){};
    ~CBattleScene() {};
    CBattleScene(const CBattleScene&){};
    CBattleScene& operator=(const CBattleScene&) {};
    static CBattleScene* createScene();

};