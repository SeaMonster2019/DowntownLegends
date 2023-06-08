#include "CRole.h"

#define _ABS(x) ((x)>0 ? (x) : -(x)) //返回绝对值

//程序的参数宏
#define ROLE_ENEMY_INFORMATION_DISPLAY 0//是否在头部Ui显示敌人的状态
#define FRIENDLY_FIRE 0//队友伤害


//一些参数宏
#define ROLE_HEIGHT  90
#define ROLE_WIGH    60

#define LATERAL_TRACKING_DISTANCE_X  50
#define LATERAL_TRACKING_DISTANCE_Y  30

#define MOVE_STEP_LENGHT   10        //单步移动距离
#define RUN_STEP_LENGHT    30       //单步跑步距离
#define MOVE_STEP_TIME     0.05f   //单步移动动画时间
#define RUN_STEP_TIME      0.05f  //单步跑步动画时间

#define FOLLOW_LEAD_DETECTION_RANGE_MOVE         10     //跟随主角移动时探测距离
#define FOLLOW_LEAD_DETECTION_RANGE_RUN          30    //跟随主角跑动时探测距离
#define FOLLOW_LEAD_DETECTION_RANGE_ANGLE        30   //跟随主角寻径时探测距离
#define FOLLOW_LEAD_DETECTION_MOVE_LENGHT_ANGLE  10  //跟随主角寻径时移动距离

#define HEAD_PORTRAIT_HEIGHT 16
#define HEAD_PORTRAIT_WIDTH 16

//初始化类内静态成员
Vector<CRole*>* CRole::vecRoles[4] = {
    new Vector<CRole*> ,
    new Vector<CRole*> ,
    new Vector<CRole*> ,
    new Vector<CRole*> };

Readjson* CRole::_readJsonAnimate = new Readjson, * CRole::_readJsonAbility = new Readjson;
vector<int> CRole::vecBadManRoleNumber[4] = { {0}, {0}, {0},{0} };
Vector<CRole*> CRole::vecBrother[3] = {};


//构析函数
CRole::~CRole()
{
    //清除动画容器
    this->_vecRoleAnimation.clear();
    SpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("NPC/" + this->_structRoleAbility.sRoleCode + ".plist");
    CCLOG("role NO.%d is die",this->_structRoleAbility.nRoleNumber);
}


//创建角色 ---- creat role
CRole* CRole::roleCreate()
{
    CRole* role = CRole::create();
	return  role;
}


//基类初始化初始化 ---- bacise class init
bool CRole::init()
{
    if (!Sprite::init())
    {
        return false;
    }
    return true;
}


//初始化角色类，加载相关数据、动画缓存
bool CRole::roleClassInit()
{

    //读取角色动画相关的json文件、角色人物属性的json文件
    if (!_readJsonAnimate->JsonClassInit(FileUtils::getInstance()->getStringFromFile("NPC/ationlist.json")))
    {
        CCLOG("read role ation jsonfile false!");
        return false;
    }
    if (!_readJsonAbility->JsonClassInit(FileUtils::getInstance()->getStringFromFile("NPC/role_ability_list.json")))
    {
        CCLOG("read role ability jsonfile false!");
        return false;
    }
    return true;

}


//角色数值图像动化等初始化 ----  role abiliti、Frame、animation init
//参数rolename:角色的名字，对应角色图像文件的文件名
bool CRole::roleSet(const string p_sRoleCode,const short p_nCampTab , float p_X, float p_Y)
{
    //初始化角色状态
    this->setScale(3.5f, 3.5f);
    this->_structRoleStateFlg.bIsMove            =   false;
    this->_structRoleStateFlg.bIsRun             =   false;
    this->_structRoleStateFlg.bIsJump            =   false;
    this->_structRoleStateFlg.bIsAttack          =   false;
    this->_structRoleStateFlg.bIsStupid          =   false;
    this->_structRoleStateFlg.bIsComa            =   false;
    this->_structRoleStateFlg.bIsSuperArmor      =   false;
    this->_structRoleStateFlg.bDirection         =   ROLE_DIRECTION_RIGHT;
    this->_structRoleStateFlg.bIsNormal          =   true;
    this->_structRoleStateFlg.bIsDie             =   false;
    this->_structRoleStateFlg.nCampTab           =    p_nCampTab | _readJsonAbility->json_object[p_sRoleCode][6].asInt();

    
    //初始化角色属性
    this->_structRoleAbility.sRoleCode              =   p_sRoleCode;
    this->_structRoleAbility.sRoleName = _readJsonAbility->json_object[p_sRoleCode][unsigned int(0)].asString();
    this->_structRoleAbility.nRoleNumberCode        =   _readJsonAbility->json_object[p_sRoleCode][1].asInt();
    this->_structRoleAbility.nMaxHP                 =   _readJsonAbility->json_object[p_sRoleCode][2].asInt();
    this->_structRoleAbility.nATK                   =   _readJsonAbility->json_object[p_sRoleCode][3].asInt();
    this->_structRoleAbility.nDEF                   =   _readJsonAbility->json_object[p_sRoleCode][4].asInt();
    this->_structRoleAbility.unCharacter            =   _readJsonAbility->json_object[p_sRoleCode][5].asInt();
    this->_structRoleAbility.nCurrentHP             =   this->_structRoleAbility.nMaxHP;
    this->_structRoleAbility.nHardness              =   this->_structRoleAbility.nDEF / 10;
    this->_structRoleAbility.nCurrentHardness       =   this->_structRoleAbility.nHardness;
    this->_structRoleAbility.nComaTimer             =   0;
    this->_structRoleAbility.nHardnessRecoveryTimer =   0;
    this->_structRoleAbility.nRoleNumber = vecRoles[p_nCampTab]->size();
    vecRoles[p_nCampTab]->pushBack(this);

    //角色行为参数设置
    this->_structRoleBehaviorTag.nTargetNumber = -1;
    this->_structRoleBehaviorTag.nTargetCamp = -1;
    this->_structRoleBehaviorTag.nTargetState = -1;
    this->_structRoleBehaviorTag.nAttctMode = -1;
    this->_structRoleBehaviorTag.fApproachDistanceX = -1;
    this->_structRoleBehaviorTag.fApproachDistanceY = -1;
    this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
    this->_structRoleBehaviorTag.nCounterBehavior = -1;           //计数器A
    this->_structRoleBehaviorTag.nCounterAttack = -1;          //计数器A
    this->_structRoleBehaviorTag.nlTimerA = -1;          //计时器A
    this->_structRoleBehaviorTag.nlTimerB = -1;         //计时器B

    this->setName(p_sRoleCode);

    this->repeatBevior = []()->int {return ROLE_THINK0_SEELP; };

    //生成角色动画
    //读取角色纹理文件
    SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("NPC/" + p_sRoleCode + ".plist");

    Vector<SpriteFrame*> vecSpriteFrame;
    int len = 0, len0 = 0;
    string ationtype_str;
    len = _readJsonAnimate->json_object["ationtype"][len].asInt();
    for (int j = 1; j <= len; j++)
    {
        //读取动作种类
        ationtype_str = _readJsonAnimate->json_object["ationtype"][j].asString();
        len0 = 0;
        len0 = _readJsonAnimate->json_object[ationtype_str][len0].asInt();
        //清理动画帧容器，准备组装下一个动作
        vecSpriteFrame.clear();
        for (int i = 2; i <= len0; i++)
        {
            SpriteFrame* spriteFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(p_sRoleCode + " (" + _readJsonAnimate->json_object[ationtype_str][i].asString() + ").png");
            vecSpriteFrame.pushBack(spriteFrame);
        }
        this->_vecRoleAnimation.pushBack(Animation::createWithSpriteFrames(vecSpriteFrame, _readJsonAnimate->json_object[ationtype_str][1].asDouble()));
    }

    //设置锚点
    this->setAnchorPoint(Vec2(0.5, 0));

    //设定坐标
    this->setPositionX(p_X);
    this->setPositionY(p_Y);
    this->_structRoleStateFlg.fTrueYAxis = p_Y;

    //初始化完成后，设置纹理为角色无任何动作状态图像并显示
    this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(p_sRoleCode + " (7).png"));


    //重新设置角色容器里角色编号
    vecRoleSort(p_nCampTab);

    //如果是小弟，将小弟压入小弟数组，切换游戏界面时重新加载小弟
    if ((this->_structRoleStateFlg.nCampTab & 0x20) != 0 && CRole::vecBrother->size() < 3)
    {
        CRole::vecBrother->pushBack(this);
    }


#if ROLE_ENEMY_INFORMATION_DISPLAY == 1 
    CUpMenu::getInstance()->displayRoleInformation(this);
#else
    //设置Ui
    if (ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab) == ROLE_CAMP1)
    {
        CUpMenu::getInstance()->displayRoleInformation(this);
    }
#endif        

    return true;
}


//重置角色属性
void CRole::roleResetAttribute()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();

    this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(this->_structRoleAbility.sRoleCode + " (7).png"));

    this->_structRoleAbility.nCurrentHP = this->_structRoleAbility.nMaxHP;
    this->_structRoleAbility.nHardness = this->_structRoleAbility.nDEF / 10;
    this->_structRoleAbility.nCurrentHardness = this->_structRoleAbility.nHardness;
    this->_structRoleAbility.nComaTimer = 0;
    this->_structRoleAbility.nHardnessRecoveryTimer = 0;

    this->_structRoleStateFlg.bIsMove = false;
    this->_structRoleStateFlg.bIsRun = false;
    this->_structRoleStateFlg.bIsJump = false;
    this->_structRoleStateFlg.bIsAttack = false;
    this->_structRoleStateFlg.bIsStupid = false;
    this->_structRoleStateFlg.bIsComa = false;
    this->_structRoleStateFlg.bIsSuperArmor = false;
    this->_structRoleStateFlg.bDirection = ROLE_DIRECTION_LEFT;
    this->_structRoleStateFlg.bIsNormal = true;
    this->_structRoleStateFlg.bIsDie = false;

    this->_structRoleBehaviorTag.nTargetNumber = -1;
    this->_structRoleBehaviorTag.nTargetCamp = -1;
    this->_structRoleBehaviorTag.nTargetState = -1;
    this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
    this->_structRoleBehaviorTag.fApproachDistanceX = -1;
    this->_structRoleBehaviorTag.fApproachDistanceY = -1;
    this->_structRoleBehaviorTag.nAttctMode = -1;
    

    //添加信息Ui
    CUpMenu::getInstance()->displayRoleInformation(this);

}

//NPC角色观察情况并作出判断  ----- npc role observe condition and judge the situation
void CRole::npcObserveCondition()
{



}

//判断角色在数组中是否存在
bool CRole::npcBeahviorTargetExistenceJudge()
{
    if (this->_structRoleBehaviorTag.nTargetCamp >= 0
        && this->_structRoleBehaviorTag.nTargetCamp <= 4
        && this->_structRoleBehaviorTag.nTargetNumber >= 0
        && vecRoles[_structRoleBehaviorTag.nTargetCamp]->size() > this->_structRoleBehaviorTag.nTargetNumber
        && this->_structRoleBehaviorTag.nTargetNumberCode == vecRoles[_structRoleBehaviorTag.nTargetCamp]->at(this->_structRoleBehaviorTag.nTargetNumber)->_structRoleAbility.nRoleNumberCode)
    {
        return true;
    }
    return false;
}


//射线检测障碍物
//参数
//1.p_nDirection;墙壁方向、探测方向、射线方向
//2.p_fDistance:射线距离
bool CRole::_npcPathDetection(const int p_nDirection,const float p_fDistance)
{
    Vec3 RoleVec3 = (this->getRoleVec3());
    return CMapTools::getInstance().pathDetection(&RoleVec3,p_nDirection, p_fDistance);
}
//射线垂直规避障碍物，根据障碍物判断规避移动方向和规避移动距离
//参数
//1.p_nDirection;墙壁方向、探测方向、射线方向
//2.p_nMoveDirection:移动探测方向、遇障碍之后的规避方向
//3.p_nStepLength:移动探测时每步距离
//4.p_ProbeDistance:探测距离，默认10
//5.p_nMax_MoveDistance:最大移动探测距离，默认600
//返回值:需要规避的步数 
int CRole::_npcPathDetectionVertical(const int p_nDirection, const int p_nMoveDirection,const float p_fStepLength ,const float p_fProbeDistance, const float p_fMax_MoveDistance)
{
    Vec3 roleVec3 = Vec3(this->getRoleVec3());
    float* roleSinglePotionPtr = nullptr; 
    float fStepLength = p_fStepLength;
    switch (p_nDirection)
    {
    case ROLE_DIRECTION_4_UP:
    {
        roleSinglePotionPtr = &roleVec3.x;
        if (p_nMoveDirection == ROLE_DIRECTION_4_RIGHT)
        {
            fStepLength = _ABS(fStepLength);
        }
        else if(p_nMoveDirection == ROLE_DIRECTION_4_LEFT)
        {
            fStepLength = (-1)*_ABS(fStepLength);
        }
        else 
            return -1;

        break;
    }
    case ROLE_DIRECTION_4_DOWN:
    {
        roleSinglePotionPtr = &roleVec3.x;
        if (p_nMoveDirection == ROLE_DIRECTION_4_RIGHT)
        {
            fStepLength = _ABS(fStepLength);
        }
        else if (p_nMoveDirection == ROLE_DIRECTION_4_LEFT)
        {
            fStepLength = (-1) * _ABS(fStepLength);
        }
        else
            return -1;
        break;
    }
    case ROLE_DIRECTION_4_RIGHT:
    {
        roleSinglePotionPtr = &roleVec3.y;
        if (p_nMoveDirection == ROLE_DIRECTION_4_UP)
        {
            fStepLength = _ABS(fStepLength);
        }
        else if (p_nMoveDirection == ROLE_DIRECTION_4_DOWN)
        {
            fStepLength = (-1) * _ABS(fStepLength);
        }
        else
            return -1;
        break;
    }
    case ROLE_DIRECTION_4_LEFT:
    {
        roleSinglePotionPtr = &roleVec3.y;
        if (p_nMoveDirection == ROLE_DIRECTION_4_UP)
        {
            fStepLength = _ABS(fStepLength);
        }
        else if (p_nMoveDirection == ROLE_DIRECTION_4_DOWN)
        {
            fStepLength = (-1) * _ABS(fStepLength);
        }
        else
            return -1;
        break;
    }
    default:
    {
    #if DEBUG_MODE
        CCLOG("At funcion  CRole::_npcPathDetectionVertical, function parameter error");
    #endif
        return -1;
        break;
    }
    }

    void (*operation)(float* p_roleSinglePotionPtr, const float p_fStepLength)
        = [](float* p_roleSinglePotionPtr, const float p_fStepLength)->void
        {
            *p_roleSinglePotionPtr = *p_roleSinglePotionPtr + p_fStepLength;
        };

    int nFrequency = 0;
    for (nFrequency = 0; _ABS(nFrequency * p_fStepLength) < p_fMax_MoveDistance; nFrequency++)
    {

        if (CMapTools::getInstance().pathDetection(&roleVec3, p_nDirection, p_fProbeDistance))
        {
            return nFrequency;
        }

        if (CMapTools::getInstance().pathDetection(&roleVec3, p_nMoveDirection, p_fStepLength))
        {
            (*operation)(roleSinglePotionPtr, fStepLength);
        }
        else
        {
            break;
        }

    }
    return -1;
}
//根据射线垂直规避算法移动距离，Y轴方向
//参数
//1.p_nDirection;墙壁方向、探测方向
//2.p_nFirstMoveDirection:首要移动方向
//3p_nSecondMoveDirection:次要移动方向
//返回值:返回真则代表无障碍或者规避成功，返回假则代表遇到无法规避的障碍
bool CRole::_npcPathFindAndMove_Y(const int p_nDirection, const int p_nFirstMoveDirection, const int p_nSecondMoveDirection)
{
    int nStep = this->_npcPathDetectionVertical(p_nDirection,
        p_nFirstMoveDirection,
        FOLLOW_LEAD_DETECTION_MOVE_LENGHT_ANGLE,
        FOLLOW_LEAD_DETECTION_RANGE_ANGLE);
    int moveDirection = -1;
    if (nStep > 0)
    {
        //设定重复动作标记
        const int rolebehariorTagAgo = this->_structRoleBehaviorTag.nBehaviorTag;
        this->_structRoleBehaviorTag.nBehaviorTag = ROLE_REPEAT0_BEHAVIOR;

        //设定重复动作次数
        this->_structRoleBehaviorTag.nCounterBehavior = nStep + 2;

        //设定重复动作参数
        p_nFirstMoveDirection == ROLE_DIRECTION_4_DOWN
            ? (moveDirection = ROLE_MOVE_DOWN)
            : (moveDirection = ROLE_MOVE_UP);

        repeatBevior = [rolebehariorTagAgo, moveDirection, p_role = this](void)->int
        {
            p_role->runRoleAction(moveDirection);
            return rolebehariorTagAgo;
        };

        return true;
    }
    else//第一方向寻径失败，则进行第二方向寻径
    {
        nStep = this->_npcPathDetectionVertical(p_nDirection,
            p_nSecondMoveDirection,
            FOLLOW_LEAD_DETECTION_MOVE_LENGHT_ANGLE,
            FOLLOW_LEAD_DETECTION_RANGE_ANGLE);
        if (nStep > 0)
        {
            const int rolebehariorTagAgo = this->_structRoleBehaviorTag.nBehaviorTag;
            this->_structRoleBehaviorTag.nBehaviorTag = ROLE_REPEAT0_BEHAVIOR;
            this->_structRoleBehaviorTag.nCounterBehavior = nStep + 2;
            (p_nSecondMoveDirection == ROLE_DIRECTION_4_DOWN)
                ? (moveDirection = ROLE_MOVE_DOWN)
                : (moveDirection = ROLE_MOVE_UP);
            repeatBevior = [rolebehariorTagAgo, moveDirection, p_role = this](void)->int
            {
                p_role->runRoleAction(moveDirection);
                return rolebehariorTagAgo;
            };

            return true;
        }
    }
    return false;
}


//获取角色X轴数值 --- get role posion X
float CRole::getRolePosionX()
{
    return this->getPositionX();
}
//获取角色Y轴数值 --- get role posion Y
float CRole::getRolePosionY()
{
    return this->_structRoleStateFlg.fTrueYAxis;
}
//获取角色Z轴数值 --- get posion Z
float CRole::getRolePosionZ()
{
    return this->getPositionY() - this->_structRoleStateFlg.fTrueYAxis;
}
//获取角色三维数值
Vec3 CRole::getRoleVec3()
{
    return Vec3(this->getRolePosionX(),this->getRolePosionY(),this->getRolePosionZ());
}

//判断角色是否属于某一阵营
bool CRole::roleCampJudge(const int p_nCampTag)
{
    if (ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab) == p_nCampTag)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//获取角色代号
string CRole::getRoleCode()
{
    return this->_structRoleAbility.sRoleCode;
}
//获取角色状态
bool CRole::getRoleState(int p_type)
{
    switch (p_type)
    {
    case ROLE_NORMAL:
    {
        return this->_structRoleStateFlg.bIsNormal;
        break;
    }
    case ROLE_MOVE:
    {
        return this->_structRoleStateFlg.bIsMove;
        break;
    }
    case ROLE_RUN:
    {
        return this->_structRoleStateFlg.bIsRun;
        break;
    }
    case ROLE_JUMP:
    {
        return this->_structRoleStateFlg.bIsJump;
        break;
    }
    case ROLE_COMA:
    {
        return this->_structRoleStateFlg.bIsComa;
        break;
    }
    case ROLE_ANGRY:
    {
        return this->_structRoleStateFlg.bIsSuperArmor;
        break;
    }
    default:
        break;
    }
    return false;
}
//NPC性格判断
bool CRole::_npcGetCharacter(const int p_nCharacter)
{
    if( (this->_structRoleAbility.unCharacter & p_nCharacter) != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//获取角色方向  ----  get role airetion
bool CRole::getRoleDirection()
{
    if (this->_structRoleStateFlg.bDirection)
        return true;
    else
        return false;
}


//判断角色是否需要翻转  ----   role flip is necessary judge
//参数needflipairetion:目标方向，与角色当前方向不一致时翻转
void CRole::getRoleDirection(const bool p_bNeedFlipAiretion)
{
    if (this->_structRoleStateFlg.bDirection == ROLE_DIRECTION_RIGHT && p_bNeedFlipAiretion == ROLE_DIRECTION_LEFT)
    { 
        this->setFlippedX(true);
        this->_structRoleStateFlg.bDirection = ROLE_DIRECTION_LEFT;
    }
    else if (this->_structRoleStateFlg.bDirection == ROLE_DIRECTION_LEFT && p_bNeedFlipAiretion == ROLE_DIRECTION_RIGHT)
    {
        this->setFlippedX(false);
        this->_structRoleStateFlg.bDirection = ROLE_DIRECTION_RIGHT;
    }
}


//设置角色行动标志
void CRole::setRoleBehaviorTag(const int p_nBehavior)
{
    this->_structRoleBehaviorTag.nBehaviorTag = p_nBehavior;
}


//角色状态恢复函数  ----    restor role state funtion
void CRole::restoreState()
{
    
   //没地方写了,只好在这里写，当角色上下移动的时候，更新下角色真实Y坐标
   if (this->getActionByTag(ROLE_MOVE_UP) != nullptr  || this->getActionByTag(ROLE_MOVE_DOWN) != nullptr)
   {
        this->_structRoleStateFlg.fTrueYAxis = this->getPositionY();
        this->setZOrder(10000 - this->getPositionY());
   }

    //如果角色嗝屁，别恢复状态了等回收吧
    if (this->_structRoleStateFlg.bIsDie) 
    {
        return;
    }

    //如果角色昏迷，请先让角色躺一会，否则判断角色是否可以站起来
    if(this->_structRoleStateFlg.bIsComa)   
    {
        const int nCurrentTime = MyTool::GetSecond();
        if (nCurrentTime >= this->_structRoleAbility.nComaTimer + 3)//如果睡够三秒则让角色起床
        {
            this->stopAllActions();
            this->_structRoleAbility.nCurrentHardness =   this->_structRoleAbility.nHardness;  //站起来时，恢复满硬直
            this->_structRoleStateFlg.bIsNormal       =   true;  //除角色傻站状态为真，其余特殊状态为假
            this->_structRoleStateFlg.bIsMove         =   false;
            this->_structRoleStateFlg.bIsRun          =   false;                          
            this->_structRoleStateFlg.bIsJump         =   false;
            this->_structRoleStateFlg.bIsAttack       =   false;
            this->_structRoleStateFlg.bIsStupid       =   false;
            this->_structRoleStateFlg.bIsSuperArmor   =   false;
            this->_structRoleStateFlg.bIsComa         =   false;
            this->_structRoleStateFlg.fTrueYAxis      =   this->getPositionY();
            this->setZOrder(this->getPositionY());
            this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(this->_structRoleAbility.sRoleCode + " (7).png"));
        }
        else
        {
            if (this->getActionByTag(ROLE_COMA_ATTACKED) == nullptr
                && this->getRolePosionZ() < 5)
            {
                this->_structRoleStateFlg.bIsStupid = false;
            }
        }
        return; 
    }

    //否则如果角色没有运行任何动作，恢复站姿
    if (this->_structRoleStateFlg.bIsNormal == false  && this->getNumberOfRunningActions() == 0)
    {
        this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(this->_structRoleAbility.sRoleCode + " (7).png"));
        this->_structRoleStateFlg.bIsNormal     =   true;
        this->_structRoleStateFlg.bIsRun        =   false;
        this->_structRoleStateFlg.bIsJump       =   false;
        this->_structRoleStateFlg.bIsAttack     =   false;
        this->_structRoleStateFlg.bIsStupid     =   false;
        this->_structRoleStateFlg.bIsSuperArmor =   false;
        this->_structRoleStateFlg.bIsComa       =   false;
        this->_structRoleStateFlg.fTrueYAxis    =   this->getPositionY();
        this->setZOrder(this->getPositionY());
    }

    //如果角色硬直度不是满得，没受到攻击的每7秒恢复一点硬直
    if (this->_structRoleAbility.nCurrentHardness < this->_structRoleAbility.nHardness && !this->_structRoleStateFlg.bIsComa && !this->_structRoleStateFlg.bIsDie)
    {
        const int nCurrentTime = MyTool::GetSecond();
        if (nCurrentTime >= this->_structRoleAbility.nHardnessRecoveryTimer + 7)
        {
            this->_structRoleAbility.nCurrentHardness++;
            this->_structRoleAbility.nHardnessRecoveryTimer = nCurrentTime;
        }
    }

}


//角色攻击函数，判断是哪个倒霉鬼在攻击范围内    ----    role attack  funtion
void CRole::roleAttack(const int p_nType)
{
    //判断攻击的方向
    int attack_ariction = 1;
    if (this->getRoleDirection())
    {
        attack_ariction = 1;
    }
    else
    {
        attack_ariction = -1;
    }

    //判断是何种攻击方式，type已经宏定义
    switch(p_nType)
    { 
        case ROLE_FIST_ATTACK:
        {
            this->unschedule("COMBAT");
            this->_structRoleBehaviorTag.nCounterAttack = 4;
            if(!this->_structRoleStateFlg.bIsStupid
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsDie)
            {
            this->schedule(bind(&CRole::_attackJudgeFist,this, placeholders::_1),0.05f,5,0.0f,"COMBAT");
            }
            break;
        }
        case ROLE_LEG_ATTACK:
        {
            this->unschedule("COMBAT");
            this->_structRoleBehaviorTag.nCounterAttack = 5;
            if (!this->_structRoleStateFlg.bIsStupid
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsDie)
            {
                this->schedule(bind(&CRole::_attackJudgeLeg, this, placeholders::_1), 0.05f, 5, 0.1f, "COMBAT");
            }
            break;
        }
        case ROLE_UPPERCUT:
        {
            this->unschedule("COMBAT");
            this->_structRoleBehaviorTag.nCounterAttack = 12;
            if (!this->_structRoleStateFlg.bIsStupid
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsDie)
            {
                this->schedule(bind(&CRole::_attackJudgeUppercut, this, placeholders::_1), 0.05f, 13, 0.2f, "COMBAT");
            }
            break;
        }
        case ROLE_SLIDING_KICK:
        {
            this->unschedule("COMBAT");
            this->_structRoleBehaviorTag.nCounterAttack = 10;
            if (!this->_structRoleStateFlg.bIsStupid
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsDie)
            {
                this->schedule(bind(&CRole::_attackJudgeSlidingKick, this, placeholders::_1), 0.05f, 11, 0.0f, "COMBAT");
            }
            break;
        }
        case ROLE_FLYING_KICK:
        {
            this->unschedule("COMBAT");
            this->_structRoleBehaviorTag.nCounterAttack = 10;
            if (!this->_structRoleStateFlg.bIsStupid
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsDie)
            {
                this->schedule(bind(&CRole::_attackJudgeFlyinggKick, this, placeholders::_1), 0.05f, 11, 0.0f, "COMBAT");
            }
            break;
        }
        default:
            break;
    }
}


//角色挨打函数，用于结算血量和运行挨打动画  ——  role attacked funtion 
//该函数函数用于被攻击，被攻击者目标找到并调用
void CRole::roleAttacked(const int p_nAggressivity, const int p_nStrength, const bool p_ATKDirection)
{
    this->unschedule("COMBAT");//结束攻击判断调度器

    //优先进行血量结算
    if (this->_structRoleAbility.nDEF >= p_nAggressivity || this->_structRoleStateFlg.bIsComa)//
    {
        //被攻击角色防御力过高或者昏迷时被攻击，按百分比扣血
        this->_structRoleAbility.nCurrentHP = this->_structRoleAbility.nCurrentHP - 5;
        //this->_structRoleAbility.nCurrentHP = this->_structRoleAbility.nCurrentHP - this->_structRoleAbility.nMaxHP * 0.05;
    }
    else
    {
        //否则，按攻防差扣血
        this->_structRoleAbility.nCurrentHP = this->_structRoleAbility.nCurrentHP - (5 + p_nAggressivity - this->_structRoleAbility.nDEF);
    }

    if (this->_structRoleAbility.bShowInformation)
    {
        this->_structRoleAbility.headUiPtr->updateHP(this->_structRoleAbility.nCurrentHP > 0 ? this->_structRoleAbility.nCurrentHP : 0);
    }
        

    //判断根据血量和自身状态判断运行那种挨打动画
    //1.没血时
    if (this->_structRoleAbility.nCurrentHP <= 0 
        && this->_structRoleStateFlg.bIsJump == false) //如果嗝屁了
    {
        //运行角色击飞+昏迷+闪烁动画
        this->_structRoleStateFlg.bIsDie = true;
        if (p_ATKDirection == FRONT)
        {
            this->runRoleAction(ROLE_FRONT_FLY_AND_COMA_AND_DIE);
        }
        else
        {
            this->runRoleAction(ROLE_BEHIND_FLY_AND_COMA_AND_DIE);
        }
        return;
    }
    else if(this->_structRoleAbility.nCurrentHP <= 0
        && this->_structRoleStateFlg.bIsJump == true)
    {
        this->_structRoleStateFlg.bIsDie = true;
        if (p_ATKDirection == FRONT)
        {
            this->runRoleAction(ROLE_JUMPING_FRONT_FLY_AND_DIE);
        }
        else
        {
            this->runRoleAction(ROLE_JUMPING_BEHIND_FLY_AND_DIE);
        }
        return;
    }

    //2.有血跳跃时被击中，不管硬直结算，运行倒地昏迷动画
    if (this->_structRoleStateFlg.bIsJump)
    {
        this->_structRoleAbility.nCurrentHardness = 0;
        if (p_ATKDirection == FRONT)
        {
            this->runRoleAction(ROLE_JUMPING_FRONT_FLY_AND_COMA);
        }
        else
        {
            this->runRoleAction(ROLE_JUMPING_BEHIND_FLY_AND_COMA);
        }
        return;
    }

    //3.有血，判断硬直
    this->_structRoleAbility.nCurrentHardness -= p_nStrength;//结算硬直
    if (this->_structRoleStateFlg.bIsComa)//当前角色昏迷而且还被锤了，运行倒地还被锤动画
    {
        this->runRoleAction(ROLE_COMA_ATTACKED);
    }
    else if(this->_structRoleAbility.nCurrentHardness <= 0) //如果角色被攻击后硬直掉为0，运行被击飞然后倒地动画
    {
        if (p_ATKDirection == FRONT)
        {
            this->runRoleAction(ROLE_FRONT_FLY_AND_COMA);
        }
        else
        {
            this->runRoleAction(ROLE_BEHIND_FLY_AND_COMA);
        }
    }
    else //简单的被攻击了一下，硬直度还在
    {
        this->_structRoleAbility.nHardnessRecoveryTimer = MyTool::GetSecond();
        if (p_ATKDirection == FRONT)
        {
            this->runRoleAction(ROLE_FRONT_SHAKE_BACK);
        }
        else
        {
            this->runRoleAction(ROLE_BEHIND_SHAKE_BACK);
        }
    }
}
//该函数函数用于跳跃时撞墙，被自身调用
void CRole::roleAttacked(const bool p_bATKDirection)
{
    //结束相关调度器
    this->unschedule("COMBAT");
    this->unschedule("ROLE_DIE");
    this->stopAllActions();

    //如果跳跃时撞墙了
    this->_structRoleAbility.nCurrentHP = this->_structRoleAbility.nCurrentHP - this->_structRoleAbility.nMaxHP * 0.05;
    if (this->_structRoleAbility.bShowInformation)
    {
        this->_structRoleAbility.headUiPtr->updateHP(this->_structRoleAbility.nCurrentHP>0? this->_structRoleAbility.nCurrentHP:0);
    }

    if (this->_structRoleAbility.nCurrentHP > 0)
    {
        if (p_bATKDirection == FRONT)//正面跳跃撞墙
        {
            this->runRoleAction(ROLE_JUMPING_FRONT_FLY_AND_COMA);
        }
        else//跳跃后脑勺撞墙
        {
            this->runRoleAction(ROLE_JUMPING_BEHIND_FLY_AND_COMA);
        }
    }
    else
    {
        if (p_bATKDirection == FRONT)//正面跳跃撞墙
        {
            this->runRoleAction(ROLE_JUMPING_FRONT_FLY_AND_DIE);
        }
        else//跳跃后脑勺撞墙
        {
            this->runRoleAction(ROLE_JUMPING_BEHIND_FLY_AND_DIE);
        }

    }


    return;
}
//该函数用于走路、跑步时撞墙，被自身调用
void CRole::roleAttacked()
{    //结束相关调度器
    this->unschedule("COMBAT");
    this->unschedule("ROLE_DIE");
    this->stopAllActions();

    this->_structRoleAbility.nCurrentHP = this->_structRoleAbility.nCurrentHP - this->_structRoleAbility.nMaxHP * 0.05;
    this->_structRoleAbility.nCurrentHardness = 0;

    if (this->_structRoleAbility.bShowInformation)
    {
        this->_structRoleAbility.headUiPtr->updateHP(this->_structRoleAbility.nCurrentHP > 0 ? this->_structRoleAbility.nCurrentHP : 0);
    }

    if (this->_structRoleAbility.nCurrentHP > 0)
        this->runRoleAction(ROLE_FRONT_FLY_AND_COMA);
    else
        this->runRoleAction(ROLE_FRONT_FLY_AND_COMA_AND_DIE);


    return;
}


//攻击判断函数
void CRole::_attackJudgeFist(float s)
{
    const bool bRoleDirection = this->getRoleDirection();//角色方向
    int nAtttackDirection = -1;//攻击方向
    if (bRoleDirection)
    {
        nAtttackDirection = 1;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == 4)//判断攻击计数器
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
    }
    if(this->_structRoleBehaviorTag.nCounterAttack >= 0)
    {
        Vec3 roleHitVec3 = this->getRoleVec3();
        roleHitVec3.x = roleHitVec3.x + nAtttackDirection * (this->_structRoleBehaviorTag.nCounterAttack - 5) * (-1) * 12.5;
        roleHitVec3.z = 60;
        for (int j = 0 ; j < 4 ; j++)
        {

#if FRIENDLY_FIRE == 0
            if (j == ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab))
            {
                continue;
            }
#endif
            for (int i = 0; i < vecRoles[j]->size(); i++)
            {
                if (vecRoles[j]->at(i)->roleAttackedJudge(roleHitVec3))
                {
                    SRoleNumber attackedRoleNumber = SRoleNumber(j,i);
                    int k = 0;
                    for(;k < this->_structRoleBehaviorTag.AttackedRoleNumberList.size(); k++)
                    { 
                        if(attackedRoleNumber == this->_structRoleBehaviorTag.AttackedRoleNumberList.at(k))
                        { 
                            break;
                        }
                    }
                    if (k == this->_structRoleBehaviorTag.AttackedRoleNumberList.size())
                    {
                        vecRoles[j]->at(i)->roleAttacked(this->_structRoleAbility.nATK, 1, bRoleDirection == vecRoles[j]->at(i)->getRoleDirection() ? false : true);
                        this->_structRoleBehaviorTag.AttackedRoleNumberList.push_back(attackedRoleNumber);
                    }
                }
            }
        }
        this->_structRoleBehaviorTag.nCounterAttack--;
    }
    if(this->_structRoleBehaviorTag.nCounterAttack == -1)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
        this->unschedule("COMBAT");
    }
}
void CRole::_attackJudgeLeg(float s)
{
    const bool bRoleDirection = this->getRoleDirection();
    int nAtttackDirection = -1;

    if (bRoleDirection)
    {
        nAtttackDirection = 1;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == 5)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
    }

    if (this->_structRoleBehaviorTag.nCounterAttack >= 0)
    {
        Vec3 roleHitVec3 = this->getRoleVec3();
        roleHitVec3.x = roleHitVec3.x + nAtttackDirection * (this->_structRoleBehaviorTag.nCounterAttack - 5) * (-1) * 15;
        roleHitVec3.z = 30;
        for (int j = 0; j < 4; j++)
        {
#if FRIENDLY_FIRE == 0
            if (j == ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab))
            {
                continue;
            }
#endif  
            for (int i = 0; i < vecRoles[j]->size(); i++)
            {
                if (vecRoles[j]->at(i)->roleAttackedJudge(roleHitVec3))
                {
                    SRoleNumber attackedRoleNumber = SRoleNumber(j, i);
                    int k = 0;
                    for (; k < this->_structRoleBehaviorTag.AttackedRoleNumberList.size(); k++)
                    {
                        if (attackedRoleNumber == this->_structRoleBehaviorTag.AttackedRoleNumberList.at(k))
                        {
                            break;
                        }
                    }
                    if (k == this->_structRoleBehaviorTag.AttackedRoleNumberList.size())
                    {
                        vecRoles[j]->at(i)->roleAttacked(this->_structRoleAbility.nATK, 1, bRoleDirection == vecRoles[j]->at(i)->getRoleDirection() ? false : true);
                        this->_structRoleBehaviorTag.AttackedRoleNumberList.push_back(attackedRoleNumber);
                    }
                }
            }
        }
        this->_structRoleBehaviorTag.nCounterAttack--;
    }
    if (this->_structRoleBehaviorTag.nCounterAttack == -1)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
        this->unschedule("COMBAT");
    }
}
void CRole::_attackJudgeUppercut(float s)
{
    const bool bRoleDirection = this->getRoleDirection();
    int nAtttackDirection = -1;

    if (bRoleDirection)
    {
        nAtttackDirection = 1;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == 12)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
    }

    if (this->_structRoleBehaviorTag.nCounterAttack >= 0)
    {
        Vec3 roleHitVec3 = this->getRoleVec3();

        for (int j = 0; j < 4; j++)
        {
#if FRIENDLY_FIRE == 0
            if (j == ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab))
            {
                continue;
            }
#endif  
            for (int i = 0; i < vecRoles[j]->size(); i++)
            {
                if (vecRoles[j]->at(i)->roleAttackedJudge(Vec3(roleHitVec3.x, roleHitVec3.y, roleHitVec3.z + 60))
                    || vecRoles[j]->at(i)->roleAttackedJudge(Vec3(roleHitVec3.x + nAtttackDirection * 40, roleHitVec3.y, roleHitVec3.z + 60))
                    || vecRoles[j]->at(i)->roleAttackedJudge(Vec3(roleHitVec3.x + nAtttackDirection * 40, roleHitVec3.y, roleHitVec3.z + 80)))
                {
                    SRoleNumber attackedRoleNumber = SRoleNumber(j, i);
                    int k = 0;
                    for (; k < this->_structRoleBehaviorTag.AttackedRoleNumberList.size(); k++)
                    {
                        if (attackedRoleNumber == this->_structRoleBehaviorTag.AttackedRoleNumberList.at(k))
                        {
                            break;
                        }
                    }
                    if (k == this->_structRoleBehaviorTag.AttackedRoleNumberList.size())
                    {
                        vecRoles[j]->at(i)->roleAttacked(this->_structRoleAbility.nATK*1.8, 2, bRoleDirection == vecRoles[j]->at(i)->getRoleDirection() ? false : true);
                        this->_structRoleBehaviorTag.AttackedRoleNumberList.push_back(attackedRoleNumber);
                    }
                }
            }
        }
        this->_structRoleBehaviorTag.nCounterAttack--;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == -1)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
        this->unschedule("COMBAT");
    }

}
void CRole::_attackJudgeSlidingKick(float s)
{
    const bool bRoleDirection = this->getRoleDirection();
    int nAtttackDirection = -1;
    if (bRoleDirection)
    {
        nAtttackDirection = 1;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == 10)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
    }

    if (this->_structRoleBehaviorTag.nCounterAttack >= 0)
    {
        Vec3 roleHitVec3 = this->getRoleVec3();

        for (int j = 0; j < 4; j++)
        {
#if FRIENDLY_FIRE == 0
            if (j == ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab))
            {
                continue;
            }
#endif  
            for (int i = 0; i < vecRoles[j]->size(); i++)
            {
                if (vecRoles[j]->at(i)->roleAttackedJudge(Vec3(roleHitVec3.x, roleHitVec3.y, roleHitVec3.z + 30))
                    || vecRoles[j]->at(i)->roleAttackedJudge(Vec3(roleHitVec3.x + nAtttackDirection * 20, roleHitVec3.y, roleHitVec3.z + 40))
                    || vecRoles[j]->at(i)->roleAttackedJudge(Vec3(roleHitVec3.x + nAtttackDirection * 40, roleHitVec3.y, roleHitVec3.z + 50))
                    || vecRoles[j]->at(i)->roleAttackedJudge(Vec3(roleHitVec3.x + nAtttackDirection * 60, roleHitVec3.y, roleHitVec3.z + 60)))
                {
                    SRoleNumber attackedRoleNumber = SRoleNumber(j, i);
                    int k = 0;
                    for (; k < this->_structRoleBehaviorTag.AttackedRoleNumberList.size(); k++)
                    {
                        if (attackedRoleNumber == this->_structRoleBehaviorTag.AttackedRoleNumberList.at(k))
                        {
                            break;
                        }
                    }
                    if (k == this->_structRoleBehaviorTag.AttackedRoleNumberList.size())
                    {
                        vecRoles[j]->at(i)->roleAttacked(this->_structRoleAbility.nATK * 1.2, 2, bRoleDirection == vecRoles[j]->at(i)->getRoleDirection() ? false : true);
                        this->_structRoleBehaviorTag.AttackedRoleNumberList.push_back(attackedRoleNumber);
                    }
                }
            }
        }
        this->_structRoleBehaviorTag.nCounterAttack--;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == -1)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
        this->unschedule("COMBAT");
    }
}
void CRole::_attackJudgeFlyinggKick(float s)
{
    const bool bRoleDirection = this->getRoleDirection();
    int nAtttackDirection = -1;

    if (bRoleDirection)
    {
        nAtttackDirection = 1;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == 10)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
    }

    if (this->_structRoleBehaviorTag.nCounterAttack >= 0)
    {
        Vec3 roleHitVec3 = this->getRoleVec3();
        Vec3 roleHitVec3_1 = Vec3(roleHitVec3.x + nAtttackDirection * 20, roleHitVec3.y, roleHitVec3.z);
        Vec3 roleHitVec3_2 = Vec3(roleHitVec3.x + nAtttackDirection * 40, roleHitVec3.y, roleHitVec3.z-10);
        Vec3 roleHitVec3_3 = Vec3(roleHitVec3.x + nAtttackDirection * 60, roleHitVec3.y, roleHitVec3.z-20);

        for (int j = 0; j < 4; j++)
        {
#if FRIENDLY_FIRE == 0
            if (j == ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab))
            {
                continue;
            }
#endif  
            for (int i = 0; i < vecRoles[j]->size(); i++)
            {
                if (vecRoles[j]->at(i)->roleAttackedJudge(roleHitVec3_1)
                    || vecRoles[j]->at(i)->roleAttackedJudge(roleHitVec3_2)
                    || vecRoles[j]->at(i)->roleAttackedJudge(roleHitVec3_3)
                    || vecRoles[j]->at(i)->roleAttackedJudge(roleHitVec3))
                {
                    SRoleNumber attackedRoleNumber = SRoleNumber(j, i);
                    int k = 0;
                    for (; k < this->_structRoleBehaviorTag.AttackedRoleNumberList.size(); k++)
                    {
                        if (attackedRoleNumber == this->_structRoleBehaviorTag.AttackedRoleNumberList.at(k))
                        {
                            break;
                        }
                    }
                    if (k == this->_structRoleBehaviorTag.AttackedRoleNumberList.size())
                    {
                        vecRoles[j]->at(i)->roleAttacked(this->_structRoleAbility.nATK * 1.5, 2, bRoleDirection == vecRoles[j]->at(i)->getRoleDirection() ? false : true);
                        this->_structRoleBehaviorTag.AttackedRoleNumberList.push_back(attackedRoleNumber);
                    }
                }
            }
        }
        this->_structRoleBehaviorTag.nCounterAttack--;
    }

    if (this->_structRoleBehaviorTag.nCounterAttack == -1)
    {
        this->_structRoleBehaviorTag.AttackedRoleNumberList.clear();
        this->unschedule("COMBAT");
    }
}


//判断打击点是否在自己角色身上
bool CRole::roleAttackedJudge(Vec3 vec3HitPoint)
{
    Vec3 objectRoleVec3 = this->getRoleVec3();

    if (this->roleStateJudge(ROLE_ATTACKED)
        && (vec3HitPoint.x< objectRoleVec3.x + 30 && vec3HitPoint.x > objectRoleVec3.x + -30)
        && (vec3HitPoint.y < objectRoleVec3.y + 30 && vec3HitPoint.y > objectRoleVec3.y + -30)
        && (vec3HitPoint.z < objectRoleVec3.z + 100 && vec3HitPoint.x > objectRoleVec3.z))
    {
        return true;
    }
    else
    {
        return false;
    }
}


//角色死亡函数，清除角色资源，释放内存
void CRole::roleDeath() 
{
    //如果死亡的角色是主角
    if((ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab) == ROLE_CAMP1)  && this->_structRoleAbility.nRoleNumber == 0)
    {
        this->getParent()->removeChild(this);
        CUpMenu::getInstance()->deleteAllRoleInformation();
        CBattleScene::getInstance()->gameOver();
    }
    else
    {
        CRole::vecRoleRemove(ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab), this->_structRoleAbility.nRoleNumber);
        //清除容器数据
        if (ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab) != ROLE_CAMP1)
        {
            CMapTools::getInstance().roleRodingOnce_Badman(ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab));
        }
        //删除子节点
        this->getParent()->removeChild(this);
        //清除显示的ui
        if (this->_structRoleAbility.bShowInformation)
        {
            CUpMenu::getInstance()->deleteRoleInformation(this);
        }
    }
}
//角色清除函数，切换界面时清除角色
void CRole::roleDelete()
{
    //清除角色容器内角色数据
    CRole::vecRoleRemove(ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab), this->_structRoleAbility.nRoleNumber);

    //清除该节点
    this->getParent()->removeChild(this);

    //删除显示的Ui
    if (this->_structRoleAbility.bShowInformation)
    {
        CUpMenu::getInstance()->deleteRoleInformation(this);
    }
}


//判断角色在数组中是否存在，判断是否越界
bool CRole::vecRolesExistenceJudge(const int nCampTag, const int nRoleNumber)
{
    if (vecRoles[nCampTag]->size() > nRoleNumber)
    {
        return true;
    }
    return false;
}

//对角色容器进行操作
//角色状态恢复
void CRole::vecRolesRecovery()
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < CRole::vecRoles[j]->size(); i++)
        {
            CRole::vecRoles[j]->at(i)->restoreState();
        }
    }
}
//重置容器内所有角色属性
void CRole::vecRoleResetAttributeAll()
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < vecRoles[j]->size(); i++)
        {
            vecRoles[j]->at(i)->roleResetAttribute();
        }
    }
}
//重置容器内主角团角色的属性
void CRole::vecRoleResetAttribute_LeadCamp()
{
    for (int i = 0; i < vecRoles[0]->size(); i++)
    {
        vecRoles[0]->at(i)->roleResetAttribute();
    }
}
//从新排序、编号角色容器
void CRole::vecRoleSort(const int p_nVecRolesNumber)
{
    for (int i = 0; i < CRole::vecRoles[p_nVecRolesNumber]->size(); i++)
    {
        CRole::vecRoles[p_nVecRolesNumber]->at(i)->_structRoleAbility.nRoleNumber = i;
    }
}
//从角色数组里删除除了主角以外的所有角色
void CRole::vecRoleLeaveBehindLead()
{
    for (int i = 1; i < CRole::vecRoles[0]->size(); i++)
    {
        CRole::vecRoles[0]->erase(i);
    }
    for(int j = 1; j < 4 ;j++)
    { 
        for (int i = 0; i < CRole::vecRoles[j]->size(); i++)
        {
            CRole::vecRoles[j]->erase(i);
        }
    }
}
//从角色数组里删除出了主角团阵营以外的所有角色 
void CRole::vecRoleLeaveBehindLeadCamp()
{
    for (int j = 1; j < 4; j++)
    {
        for (;CRole::vecRoles[j]->size() > 0 ;)
        {
            CRole::vecRoles[j]->popBack();
        }
    }
}
//从角色数组里请离除主角团阵营以外的所有角色（并非死亡和直接删除）
void CRole::vecRoleDeleteBehindLeadCamp() {
    for (int j = 1; j < 4; j++)
    {
        for (int i = CRole::vecRoles[j]->size() - 1;CRole::vecRoles[j]->size()>0; --i)
        {
            //从父节点中移除这个节点
            CRole::vecRoles[j]->at(i)->getParent()->removeChild(CRole::vecRoles[j]->at(i));
            //删除显示的Ui
            if (CRole::vecRoles[j]->at(i)->_structRoleAbility.bShowInformation)
            {
                CUpMenu::getInstance()->deleteRoleInformation(CRole::vecRoles[j]->at(i));
            }
            //从角色容器中清除这个角色
            CRole::vecRoles[j]->popBack();
        }
    }
}
//删除角色列表中的一个成员
void CRole::vecRoleRemove(const int p_nVecRolesNumber,const int p_nNuber)
{
    CRole::vecRoles[p_nVecRolesNumber]->erase(p_nNuber);
    vecRoleSort(p_nVecRolesNumber);
}
//停止角色容器里所有角色的动画
void CRole::vecRoleStopAllAtion()
{
    for(int j = 0 ; j<4 ;j++ )
    { 
        for (int i = 0; i < vecRoles[j]->size(); i++)
        {
            vecRoles[j]->at(i)->stopAllActions();
        }
    }
}
//停止角色容器里所有角色的调度器
void CRole::vecRoleStopAllScheduler()
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < vecRoles[j]->size(); i++)
        {
            vecRoles[j]->at(i)->unscheduleAllSelectors();
        }
    }
}
//转换角色指定容器里所有角色的方向
void CRole::vecRoleChangeDirection(const int p_nVecRolesNumber,const bool p_bIsTurnRight)
{
    for (int i = 0; i < vecRoles[p_nVecRolesNumber]->size(); i++)
    {
        vecRoles[p_nVecRolesNumber]->at(i)->getRoleDirection(p_bIsTurnRight);
    }
}

//切换地图时，角色类设置
void CRole::role_ChangeMap() 
{
    //请离主角团外的角色
    CRole::vecRoleLeaveBehindLeadCamp();

    for (int i = 0; i < CRole::vecRoles[0]->size(); ++i)
    {
        if (CRole::vecRoles[0]->at(i)->_structRoleAbility.nCurrentHP <= 0)
        {
            CRole::vecBrother->at(i)->roleResetAttribute();
        }
    }

    //恢复小弟的属性

    for (int i = 0,j = 0; i < CRole::vecBrother->size(); ++i)
    {

        for (j = 0; j < CRole::vecRoles[0]->size(); ++j)
        {
            if (CRole::vecBrother->at(i) == CRole::vecRoles[0]->at(j))
            {
                j = -1;
                break;
            }
        }

        if (j != -1)
        {
            CRole::vecRoles[0]->pushBack(CRole::vecBrother->at(i));
            CRole::vecBrother->at(i)->roleResetAttribute();
        }

    }

}


//敌对阵营角色编号数组初始化
void CRole::vecBadManNumberInit(const int p_nVecRoleNumber,const bool p_bHaveBoss)
{
    CRole::vecBadManRoleNumber[p_nVecRoleNumber].clear();
    vector<int> number;
    //如果地图有BOSS，提前塞入BOSS编号
    if (p_bHaveBoss)
    {
        CRole::vecBadManRoleNumber[p_nVecRoleNumber].push_back(9);
    }
    //先生成循序队列
    for (int i = 1; i <= 8; i++)
    {
        number.push_back(i);
    }
    //次而从顺序队列中抽出编号组成无序队列
    for (int i = 0; i < 8; i++)
    {
       const int j = rand()%number.size();
       CRole::vecBadManRoleNumber[p_nVecRoleNumber].push_back(number.at(j));
       number.erase(number.begin()+j);
    }
}


//判断角色当前状态,返回值代表是否可以运行p_nType所代表的动作  ----  judge the current status of the role
bool CRole::roleStateJudge(const int p_nType)
{
    switch (p_nType)
    {
        case ROLE_MOVE:       //判断角色是否可以进行移动
        {
            if (!this->_structRoleStateFlg.bIsJump 
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsAttack 
                && !this->_structRoleStateFlg.bIsStupid 
                && !this->_structRoleStateFlg.bIsDie
                )
                return true;
            break;
        }
        case ROLE_RUN:        //判断角色是否可以进行跑步
        {
            if (!this->_structRoleStateFlg.bIsJump 
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsAttack 
                && !this->_structRoleStateFlg.bIsStupid 
                && !this->_structRoleStateFlg.bIsDie
                )
                return true;
            break;
        }
        case ROLE_JUMP:       //判断角色是否可以进行跳跃
        {
            if (!this->_structRoleStateFlg.bIsJump 
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsAttack 
                && !this->_structRoleStateFlg.bIsStupid 
                && !this->_structRoleStateFlg.bIsDie
                )
                return true;
            break;
        }
        case ROLE_FIST_ATTACK:       //判断角色是否可以打拳
        {
            if (!this->_structRoleStateFlg.bIsAttack 
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsStupid 
                && !this->_structRoleStateFlg.bIsDie
                )
                return true;
            break;
        }
        case ROLE_LEG_ATTACK:       //判断角色是否可以踢腿
        {
            if (!this->_structRoleStateFlg.bIsAttack 
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsStupid 
                && !this->_structRoleStateFlg.bIsDie
                )
                return true;
            break;
        }
        case ROLE_UPPERCUT:       //判断角色是否可以上勾拳
        {
            if (((this->_structRoleStateFlg.bIsRun
                        ||this->_structRoleStateFlg.bIsJump
                        &&!this->_structRoleStateFlg.bIsAttack
                        )
                    &&!this->_structRoleStateFlg.bIsStupid 
                    && !this->_structRoleStateFlg.bIsComa
                    && !this->_structRoleStateFlg.bIsDie
                    )
                ||(this->_structRoleStateFlg.bIsAttack
                    && !this->_structRoleStateFlg.bIsStupid
                    && !this->_structRoleStateFlg.bIsComa
                    && !this->_structRoleStateFlg.bIsDie
                    && (this->getActionByTag(ROLE_SLIDING_KICK) != nullptr)  
                    )
                )
                return true;
            break;
        }
        case ROLE_SLIDING_KICK:       //判断角色是否可以滑铲
        {
            if (this->_structRoleStateFlg.bIsRun
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsStupid 
                && !this->_structRoleStateFlg.bIsJump 
                && !this->_structRoleStateFlg.bIsAttack 
                && !this->_structRoleStateFlg.bIsDie
                )
                return true;
            break;
        }
        case ROLE_FLYING_KICK:       //判断角色是否可以飞踢
        {
            if (this->_structRoleStateFlg.bIsJump
                && !this->_structRoleStateFlg.bIsRun
                && !this->_structRoleStateFlg.bIsComa
                && !this->_structRoleStateFlg.bIsStupid 
                && !this->_structRoleStateFlg.bIsAttack
                && !this->_structRoleStateFlg.bIsDie
                )
                return true;
            break;
        }
        case ROLE_ATTACKED:          //判断角色是否可以被攻击
        {
            if (this->_structRoleStateFlg.bIsDie 
                || this->_structRoleStateFlg.bIsStupid 
                || this->_structRoleStateFlg.bIsSuperArmor
                || this->getActionByTag(ROLE_FRONT_SHAKE_BACK)  != nullptr 
                || this->getActionByTag(ROLE_BEHIND_SHAKE_BACK) != nullptr 
                || this->getActionByTag(ROLE_COMA_ATTACKED)     != nullptr 
                || this->getActionByTag(ROLE_FRONT_FLY)         != nullptr
                || this->getActionByTag(ROLE_BEHIND_FLY)        != nullptr
                )
                return false;
            else
                return true;
            break;
        }
    }
    return false;
}


//角色跳跃时位置状态判断
short CRole::RoleJumpStateJudge(const float p_fTimes)
{
    //如果是主角
    if (this->_structRoleAbility.nRoleNumber == 0 && (ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab) == ROLE_CAMP1))
    {
        this->schedule([p_role = this](float s) {
            Vec3 roleVec3 = p_role->getRoleVec3();
            short nLimit = CMapTools::getInstance().mapJudge(&roleVec3, 0);

            if (MAP_JUDGE(nLimit, MAP_CHANGE_IN))
            {
                CBattleScene::getInstance()->changeMap((MAP_CHANGE_IN | CMapTools::getInstance().getMapIformation().nNextMapNumber));
                p_role->unschedule("ROLE_JUMP_JUDGE");
            }
            else if (MAP_JUDGE(nLimit, MAP_CHANGE_OUT))
            {
                CBattleScene::getInstance()->changeMap((MAP_CHANGE_OUT | CMapTools::getInstance().getMapIformation().nBeforMapNumber));
                p_role->unschedule("ROLE_JUMP_JUDGE");
            }
            else if(MAP_JUDGE(nLimit, MAP_JUDGE_DROWNING))
            {
                    p_role->runRoleAction(ROLE_DROWNING);
            }
            else if(MAP_JUDGE(nLimit, CAN_NOT_RIGHT) && (p_role->getActionByTag(ROLE_JUMP_RIGHT) != 0))
            {
                
                p_role->roleAttacked(p_role->getRoleDirection() == ROLE_DIRECTION_RIGHT ? FRONT : BEHIND);
                p_role->unschedule("ROLE_JUMP_JUDGE");

            }
            else if(MAP_JUDGE(nLimit, CAN_NOT_LEFT) && (p_role->getActionByTag(ROLE_JUMP_LEFT) != 0))
            {
                p_role->roleAttacked(p_role->getRoleDirection() == ROLE_DIRECTION_LEFT ? FRONT : BEHIND);
                p_role->unschedule("ROLE_JUMP_JUDGE");
            }
        }, 0.05f, (p_fTimes / 0.05) + 1, 0.f, "ROLE_JUMP_JUDGE");
        return 0;
    }
    else//如果不是主角
    {
        this->schedule([p_role = this](float s) {

            Vec3 roleVec3 = p_role->getRoleVec3();
            short nLimit = CMapTools::getInstance().mapJudge(&roleVec3, 0);
            if (MAP_JUDGE(nLimit, MAP_JUDGE_DROWNING))
            {
                    p_role->runRoleAction(ROLE_DROWNING);
            }
            else if (MAP_JUDGE(nLimit, CAN_NOT_RIGHT) && (p_role->getActionByTag(ROLE_JUMP_RIGHT) != 0))
            {
                p_role->roleAttacked(p_role->getRoleDirection() == ROLE_DIRECTION_RIGHT ? FRONT : BEHIND);
                p_role->unschedule("ROLE_JUMP_JUDGE");
            }
            else if (MAP_JUDGE(nLimit, CAN_NOT_LEFT) && (p_role->getActionByTag(ROLE_JUMP_LEFT) != 0))
            {
                p_role->roleAttacked(p_role->getRoleDirection() == ROLE_DIRECTION_LEFT ? FRONT : BEHIND);
                p_role->unschedule("ROLE_JUMP_JUDGE");
            }
        }, 0.05f, (p_fTimes / 0.05) + 1, 0.f, "ROLE_JUMP_JUDGE");
    }
}


//角色移动后位置状态判断
short CRole::RoleMovingStateJudge(const short p_nLimit, const int n_pType)
{
    //如果是主角
    if (this->_structRoleAbility.nRoleNumber == 0 && (ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab) == ROLE_CAMP1))
    {
        if (MAP_JUDGE(p_nLimit, MAP_CHANGE))
        {
            CBattleScene::getInstance()->changeMap(CMapTools::getInstance().getLodingParameter(p_nLimit));
            return p_nLimit;
        }
        else if (MAP_JUDGE(p_nLimit, MAP_JUDGE_DROWNING))
        {
            this->runRoleAction(ROLE_DROWNING);
            return p_nLimit;
        }
        return 0;
    }
    else//果如不是主角
    {
        if (MAP_JUDGE(p_nLimit, MAP_JUDGE_DROWNING))
        {
            this->runRoleAction(ROLE_DROWNING);
            return p_nLimit;
        }
        return 0;
    }
}


//NPC对目标角色状态判断
bool CRole::_npcCheckTargetStatus(const int p_nCheakType)
{
    CRole* tagetRole = nullptr;

    if(this->npcBeahviorTargetExistenceJudge())
    {
        tagetRole = vecRoles[this->_structRoleBehaviorTag.nTargetCamp]->at(this->_structRoleBehaviorTag.nTargetNumber);
    }
    else
    {
        return false;
    }

    switch (p_nCheakType)
    {
    case NPC_CHEAK_TARGET_IS_ATTAINABLE:
    {
        if (tagetRole->_structRoleStateFlg.bIsDie
            || tagetRole->_structRoleStateFlg.bIsSuperArmor)
        {
            return true;
        }
        else
        {
            return false;
        }
        break;
    }
    case NPC_CHEAK_TARGET_IS_COMA:
    {
        if (tagetRole->_structRoleStateFlg.bIsComa)
        {
            return true;
        }
        else
        {
            return false;
        }
        break;
    }
    case NPC_CHEAK_TARGET_IS_DIE:
    {
        if (tagetRole->_structRoleStateFlg.bIsDie)
        {
            return true;
        }
        else
        {
            return false;
        }
        break;
    }
    case NPC_CHEAK_TARGET_IS_COMA_OR_DIE:
    {
        if (tagetRole->_structRoleStateFlg.bIsDie
            || tagetRole->_structRoleStateFlg.bIsComa)
        {
            return true;
        }
        else
        {
            return false;
        }
        break;
    }
    default:
    #if DEBUG_MODE
        CCLOG("at function CRole::_npcCheckTargetStatus,Parameter error,value is %d!", p_nCheakType);
    #endif
        break;
    }
    return false;
}


//NPC行为函数
void CRole::npcBehavior()
{
    switch (this->_structRoleBehaviorTag.nBehaviorTag)
    {
    case ROLE_THINK0_SEELP://如果角色死亡、昏迷、被击中
    {
        if(!this->_structRoleStateFlg.bIsComa && !this->_structRoleStateFlg.bIsDie && !this->_structRoleStateFlg.bIsStupid)
        { 
            this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_WAKE;
            //this->CRole::npcBehavior();
        }
        else
        {

        }
        break;
    }
    case ROLE_THINK0_WAKE://角色开始行动
    {
        this->_structRoleBehaviorTag.nBehaviorTag = ROLE_BEHAVIOR1_LOCKING;
        break;
    }
    case ROLE_THINK0_TRANCE://发呆
    {
        if (!this->_structRoleStateFlg.bIsComa && !this->_structRoleStateFlg.bIsDie && !this->_structRoleStateFlg.bIsStupid)
        {
            --this->_structRoleBehaviorTag.nCounterBehavior;
            if (this->_structRoleBehaviorTag.nCounterBehavior <= 0)
            {
                this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
            }
        }
        else
        {
            this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
        }
        break;
    }
    case ROLE_REPEAT0_BEHAVIOR://重复行为
    {
        if (this->_structRoleBehaviorTag.nCounterBehavior > 0)
        {
            repeatBevior();
            this->_structRoleBehaviorTag.nCounterBehavior--;
        }
        else 
        { 
            this->_structRoleBehaviorTag.nBehaviorTag = repeatBevior();
        }

        if (this->_structRoleBehaviorTag.nBehaviorTag < -1)
        {
            this->_structRoleBehaviorTag.nBehaviorTag == -1;
        }
        break;
    }
    case ROLE_BEHAVIOR1_LOCKING_LEAD://锁定主角
    {
        bool bIsOverlap = false;
        do
        {
            this->_structRoleBehaviorTag.fApproachDistanceX = 300 + MyTool::GetIntRandom(0,3) * 50;
            this->_structRoleBehaviorTag.fApproachDistanceY = 50 + MyTool::GetIntRandom(0,2) * 25;
            bIsOverlap = false;
            for (int i = 1; i < vecRoles[ROLE_CAMP1]->size(); i++)
            {
                if (this == vecRoles[ROLE_CAMP1]->at(i))
                {
                    continue;
                }

                if ((this->_structRoleBehaviorTag.fApproachDistanceX == vecRoles[ROLE_CAMP1]->at(i)->_structRoleBehaviorTag.fApproachDistanceX)
                        && (this->_structRoleBehaviorTag.fApproachDistanceY == vecRoles[ROLE_CAMP1]->at(i)->_structRoleBehaviorTag.fApproachDistanceY))
                {
                    bIsOverlap = true;
                    break;
                }
            }
        }while (bIsOverlap);

        this->_structRoleBehaviorTag.nTargetNumber = 0;
        this->_structRoleBehaviorTag.nTargetCamp = 0;
        this->_structRoleBehaviorTag.nTargetNumberCode = vecRoles[0]->at(0)->_structRoleAbility.nRoleNumberCode;
        this->_structRoleBehaviorTag.nBehaviorTag = ROLE_BEHAVIOR2_FLOW_LEAD;
        break;
    }
    case ROLE_BEHAVIOR2_FLOW_LEAD://跟随主角
    {
        if (--this->_structRoleBehaviorTag.nCounterBehavior <= 0)
        {
            this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
        }

        if (this->roleStateJudge(ROLE_MOVE) && this->roleStateJudge(ROLE_RUN))
        {
            static long int numbertime = 0;
            Vec2 leadPosion = Vec2(vecRoles[0]->at(0)->getRolePosionX(), vecRoles[0]->at(0)->getRolePosionY());
            Vec2 SubjectPosition = Vec2(this->getRolePosionX(), this->getRolePosionY());
            
            //把脸面向主角

            int nDirectionFlgX = 1;//X方向
            int nDirectionFlgY = 1;//Y方向

            if (leadPosion.x > SubjectPosition.x)
            { 
                nDirectionFlgX = -1;
            }
            else
            { 
                nDirectionFlgX = 1;
            }

            if (leadPosion.y > SubjectPosition.y)
            {
                nDirectionFlgY = -1;
            }
            else
            {
                nDirectionFlgY = 1;
            }

            const int nOurDistanceX = leadPosion.x + nDirectionFlgX * this->_structRoleBehaviorTag.fApproachDistanceX;
            const int nOurDistanceY = leadPosion.y + nDirectionFlgY * this->_structRoleBehaviorTag.fApproachDistanceY;

            int firstMoveDirection = -1;
            int secondMoveDirection = -1;

            //X轴动作
            bool bNeedAtion_Y = true;
            if (nDirectionFlgX == -1 && (SubjectPosition.x < nOurDistanceX))//向右移动
            {
                const bool bIsRuningTime = (vecRoles[0]->at(0)->_structRoleStateFlg.bIsRun == true 
                    || (SubjectPosition.x < nOurDistanceX + nDirectionFlgX * 500));
                //路径探测
                if((bIsRuningTime == true && this->_npcPathDetection(ROLE_DIRECTION_4_RIGHT, FOLLOW_LEAD_DETECTION_RANGE_RUN))
                    || (bIsRuningTime == false && this->_npcPathDetection(ROLE_DIRECTION_4_RIGHT, FOLLOW_LEAD_DETECTION_RANGE_MOVE)))
                { 
                    //如果主角在跑或者距离太远了，跑步跟随，否则走路跟随
                    if (bIsRuningTime)
                        this->runRoleAction(ROLE_RUN_RIGHT);
                    else
                        this->runRoleAction(ROLE_MOVE_RIGHT);
                }
                else//如果不能往前走
                {
                    this->runRoleAction(ROLE_BRAKING);
                    bNeedAtion_Y = false;
                    if (leadPosion.y > SubjectPosition.y)
                    {
                        firstMoveDirection = ROLE_DIRECTION_4_UP;
                        secondMoveDirection = ROLE_DIRECTION_4_DOWN;
                    }
                    else
                    {
                        firstMoveDirection = ROLE_DIRECTION_4_DOWN;
                        secondMoveDirection = ROLE_DIRECTION_4_UP;
                    }
                    
                    this->_npcPathFindAndMove_Y(ROLE_DIRECTION_4_RIGHT,firstMoveDirection, secondMoveDirection);
                }
            }
            else if (nDirectionFlgX == 1 && (SubjectPosition.x > nOurDistanceX))//向左移动
            {
                const bool bIsRuningTime = (vecRoles[0]->at(0)->_structRoleStateFlg.bIsRun == true
                    || (SubjectPosition.x > nOurDistanceX + nDirectionFlgX * 500));
                //路径探测
                if ((bIsRuningTime == true && this->_npcPathDetection(ROLE_DIRECTION_4_LEFT, FOLLOW_LEAD_DETECTION_RANGE_RUN))
                    || (bIsRuningTime == false && this->_npcPathDetection(ROLE_DIRECTION_4_LEFT, FOLLOW_LEAD_DETECTION_RANGE_MOVE)))
                {
                    //如果主角在跑或者距离太远了，跑步跟随，否则走路跟随
                    if (bIsRuningTime)
                        this->runRoleAction(ROLE_RUN_LEFT);
                    else
                        this->runRoleAction(ROLE_MOVE_LEFT);
                }
                else//如果不能往前走
                {
                    this->runRoleAction(ROLE_BRAKING);
                    bNeedAtion_Y = false;
                    if (leadPosion.y > SubjectPosition.y)
                    {
                        firstMoveDirection = ROLE_DIRECTION_4_UP;
                        secondMoveDirection = ROLE_DIRECTION_4_DOWN;
                    }
                    else
                    {
                        firstMoveDirection = ROLE_DIRECTION_4_DOWN;
                        secondMoveDirection = ROLE_DIRECTION_4_UP;
                    }
                    this->_npcPathFindAndMove_Y(ROLE_DIRECTION_4_LEFT, firstMoveDirection, secondMoveDirection);
                }
            }
            else
            {
                this->getRoleDirection(vecRoles[0]->at(0)->_structRoleStateFlg.bDirection);
            }

            //Y轴动作
            if(bNeedAtion_Y)
            { 
                if (nDirectionFlgY == -1 && (SubjectPosition.y < nOurDistanceY))
                {
                    if(_npcPathDetection(ROLE_DIRECTION_4_UP,MOVE_STEP_LENGHT))
                    { 
                        this->runRoleAction(ROLE_MOVE_UP);
                    }
                }
                else if(nDirectionFlgY == 1 && (SubjectPosition.y > nOurDistanceY))
                {
                    if (_npcPathDetection(ROLE_DIRECTION_4_DOWN, MOVE_STEP_LENGHT))
                    { 
                        this->runRoleAction(ROLE_MOVE_DOWN);
                    }
                }
            }
        }
        break;
    }
    case ROLE_BEHAVIOR1_LOCKING://npc角色锁定目标行为
    {
    //判断阵营
    Vec2 SubjectPosition = this->getPosition();

    float fMinDistance = 99999999.0f;
    int nMinCampTag = -1;
    int nMinFlg = -1;


    //选取角色
    for(int j = 0 ; j < 4 ;j++)
    {

        if (j == ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab))
        {
            continue;
        }
        for (int i = 0; i < this->vecRoles[j]->size(); i++)
        {
            if (_ABS(this->vecRoles[j]->at(i)->getPositionX() - SubjectPosition.x) < fMinDistance)
            {
                fMinDistance = _ABS(this->vecRoles[j]->at(i)->getPositionX() - SubjectPosition.x);
                nMinCampTag = j;
                nMinFlg = i;
            }
        }
    }

    //如果没敌人，目标标志设为-1表示没有敌人
    if (nMinFlg == -1 && nMinCampTag == -1)
    { 
        this->_structRoleBehaviorTag.nTargetNumber = -1;
        this->_structRoleBehaviorTag.nTargetCamp = -1;
        this->_structRoleBehaviorTag.nTargetNumberCode = -1;
        //如果搜索不到敌人且搜索角色为主角阵营，则跟随主角移动
        if(ROLE_CAMP_JUDGE(this->_structRoleStateFlg.nCampTab) == ROLE_CAMP1)
        {
            this->_structRoleBehaviorTag.nBehaviorTag = ROLE_BEHAVIOR1_LOCKING_LEAD;
            //跟随时，应保证偶尔会看看是否出现敌人
            this->_structRoleBehaviorTag.nCounterBehavior = 10;
        }
        else//否则重新判断该运行什么动作
        {
            this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
        }
    }
    else//有敌人则确认目标，并决定用何种方式攻击
    {
        this->_structRoleBehaviorTag.nTargetNumber = nMinFlg;
        this->_structRoleBehaviorTag.nTargetCamp = nMinCampTag;
        this->_structRoleBehaviorTag.nTargetNumberCode = vecRoles[nMinCampTag]->at(nMinFlg)->_structRoleAbility.nRoleNumberCode;
        this->_structRoleBehaviorTag.fApproachDistanceX = 200 + rand() % 5 * 20;
        this->_structRoleBehaviorTag.fApproachDistanceY = 50 + rand() % 3 * 5;
        this->_structRoleBehaviorTag.nBehaviorTag = ROLE_BEHAVIOR2_TARGET_NEAR;
    }
    break;
    }
    case ROLE_BEHAVIOR2_TARGET_NEAR://npc角色接近目标行为
    {
        if (this->roleStateJudge(ROLE_MOVE)
            && this->npcBeahviorTargetExistenceJudge())
        {
            CRole* objectRole = vecRoles[this->_structRoleBehaviorTag.nTargetCamp]->at(this->_structRoleBehaviorTag.nTargetNumber);
            if(!objectRole->_structRoleStateFlg.bIsDie)
            {
                Vec3 subjectRoleVec3 = this->getRoleVec3();
                Vec3 objectRoleVec3 = objectRole->getRoleVec3();

                //判断角色和目标之间的相对方向
                int nRelativeDirectionX = ROLE_DIRECTION_4_RIGHT;
                int nRelativeDirectionY = ROLE_DIRECTION_4_UP;
                Vec2 TargetLocation = Vec2(0,0);//目标位置

                //判断X轴相对方向
                if (subjectRoleVec3.x > objectRoleVec3.x)
                {
                    nRelativeDirectionX = ROLE_DIRECTION_4_LEFT;
                    TargetLocation.x = objectRoleVec3.x + this->_structRoleBehaviorTag.fApproachDistanceX;
                }
                else
                {
                    nRelativeDirectionX = ROLE_DIRECTION_4_RIGHT;
                    TargetLocation.x = objectRoleVec3.x - this->_structRoleBehaviorTag.fApproachDistanceX;
                }

                //判断Y轴相对方向
                if (subjectRoleVec3.y > objectRoleVec3.y)
                {
                    nRelativeDirectionY = ROLE_DIRECTION_4_DOWN;
                    TargetLocation.y = objectRoleVec3.y + this->_structRoleBehaviorTag.fApproachDistanceY;
                }
                else                                                                   
                {
                    nRelativeDirectionY = ROLE_DIRECTION_4_UP;
                    TargetLocation.y = objectRoleVec3.y - this->_structRoleBehaviorTag.fApproachDistanceY;
                }

                //判断角色该运动的方向X轴

                int nMoveDirectionX = -1;//即将运动的X方向
                if (subjectRoleVec3.x < TargetLocation.x
                    && nRelativeDirectionX == ROLE_DIRECTION_4_RIGHT)
                {
                    nMoveDirectionX = ROLE_DIRECTION_4_RIGHT;
                }else if (subjectRoleVec3.x > TargetLocation.x
                    && nRelativeDirectionX == ROLE_DIRECTION_4_LEFT)
                {
                    nMoveDirectionX = ROLE_DIRECTION_4_LEFT;
                }

                //判断角色该运动的方向Y轴

                int nMoveDirectionY = -1;//即将运动的Y方向
                if (subjectRoleVec3.y < TargetLocation.y
                    && nRelativeDirectionY == ROLE_DIRECTION_4_UP)
                {
                    nMoveDirectionY = ROLE_DIRECTION_4_UP;
                }
                else if (subjectRoleVec3.y > TargetLocation.y
                    && nRelativeDirectionY == ROLE_DIRECTION_4_DOWN)
                {
                    nMoveDirectionY = ROLE_DIRECTION_4_DOWN;
                }

                //如果到达指定位置，决定使用何种方式进攻
                if (nMoveDirectionX == -1 && nMoveDirectionY == -1)
                {
                    this->_structRoleBehaviorTag.nBehaviorTag = ROLE_BEHAVIOR3_CONTINUOUS_JAB;
                    this->_structRoleBehaviorTag.fApproachDistanceX = 30;
                    this->_structRoleBehaviorTag.fApproachDistanceY = 30;
                    this->_structRoleBehaviorTag.nCounterBehavior = 5;
                    break;
                }

                //X轴方向移动
                if (nMoveDirectionX != -1)
                {
                    bool bIsRuningTime = (_ABS(TargetLocation.x - subjectRoleVec3.x)) > 400 ? true : false;//太远则应该跑步接近
                    if ((bIsRuningTime == true && this->_npcPathDetection(nMoveDirectionX, FOLLOW_LEAD_DETECTION_RANGE_RUN))
                        || (bIsRuningTime == false && this->_npcPathDetection(nMoveDirectionX, FOLLOW_LEAD_DETECTION_RANGE_MOVE)))
                    {
                        //如果距离太远，跑步接近该处位置
                        if (bIsRuningTime)
                        {
                            if(nMoveDirectionX == ROLE_DIRECTION_4_RIGHT)
                            { 
                                this->runRoleAction(ROLE_RUN_RIGHT);
                            }
                            else if (nMoveDirectionX == ROLE_DIRECTION_4_LEFT)
                            {
                                this->runRoleAction(ROLE_RUN_LEFT);
                            }
                        }
                        else
                        {
                            if (nMoveDirectionX == ROLE_DIRECTION_4_RIGHT)
                            {
                                this->runRoleAction(ROLE_MOVE_RIGHT);
                            }
                            else if (nMoveDirectionX == ROLE_DIRECTION_4_LEFT)
                            {
                                this->runRoleAction(ROLE_MOVE_LEFT);
                            }

                            //可能在接近的时候，角色会发呆
                            if (this->roleCampJudge(ROLE_CAMP1))
                            {
                                if (MyTool::GetRandomBool_Percent_True(3))
                                {
                                    this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_TRANCE;
                                    this->_structRoleBehaviorTag.nCounterBehavior = MyTool::GetIntRandom(10, 40);
                                }
                            }else if((this->_structRoleStateFlg.nCampTab & 0x10) != 0)
                            {
                                if (MyTool::GetRandomBool_Percent_True(2))
                                {
                                    this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_TRANCE;
                                    this->_structRoleBehaviorTag.nCounterBehavior = MyTool::GetIntRandom(10, 40);
                                }
                            }
                            else
                            {
                                if (MyTool::GetRandomBool_Percent_True(5))
                                {
                                    this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_TRANCE;
                                    this->_structRoleBehaviorTag.nCounterBehavior = MyTool::GetIntRandom(30, 80);
                                }
                            }

                        }
                    }
                    else//如果不能往前走
                    {
                        nMoveDirectionY = -1;
                        if (this->_npcPathFindAndMove_Y(nMoveDirectionX, ROLE_DIRECTION_4_DOWN, ROLE_DIRECTION_4_UP) == false)
                        {
                            #if DEBUG_MODE
                            CCLOG("npcPathFind is false, at npcBehavior function: ROLE_BEHAVIOR2_TARGET_NEAR" );
                            #endif 
                        }
                        break ;
                    }
                }

                //Y轴方向移动
                if (nMoveDirectionY != -1)
                {
                    if (nMoveDirectionY == ROLE_DIRECTION_4_UP)
                    {
                        if(_npcPathDetection(ROLE_DIRECTION_4_UP,MOVE_STEP_LENGHT))
                        { 
                            this->runRoleAction(ROLE_MOVE_UP);
                        }
                    }
                    else if (nMoveDirectionY == ROLE_DIRECTION_4_DOWN)
                    {
                        if (_npcPathDetection(ROLE_DIRECTION_4_DOWN, MOVE_STEP_LENGHT))
                        {
                            this->runRoleAction(ROLE_MOVE_DOWN);
                        }
                    }
                }
            }
            else
            {
                this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
            }
        }
        else
        {
            this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
        }
    break;
    }
    case ROLE_BEHAVIOR3_CONTINUOUS_JAB://疯狗拳
    {
        if (!this->_structRoleStateFlg.bIsComa
            && !this->_structRoleStateFlg.bIsStupid
            && !this->_structRoleStateFlg.bIsJump
            && this->npcBeahviorTargetExistenceJudge())
        {
            if (this->_npcCheckTargetStatus(NPC_CHEAK_TARGET_IS_COMA_OR_DIE))
            {
                this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_WAKE;
                break;
            }

            CRole* ObjectRole = this->vecRoles[this->_structRoleBehaviorTag.nTargetCamp]->at(this->_structRoleBehaviorTag.nTargetNumber);

            Vec3 ObjectRoleVec3 = ObjectRole->getRoleVec3();
            Vec3 subjectRoleVec3 = this->getRoleVec3();

            if (_ABS(ObjectRoleVec3.x - subjectRoleVec3.x) > 400
                || _ABS(ObjectRoleVec3.y - subjectRoleVec3.y) > 200)
            {
                this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_WAKE;
                break;
            }

            //判断是否需要接近目标

            int nMoveDirectionX = -1;  //即将横向移动的方向
            if (ObjectRoleVec3.x > subjectRoleVec3.x)//如果目标在右边
            {
                if (_ABS(ObjectRoleVec3.x - subjectRoleVec3.x) > this->_structRoleBehaviorTag.fApproachDistanceX)
                {
                    nMoveDirectionX = ROLE_DIRECTION_4_RIGHT;
                }
            }
            else if (ObjectRoleVec3.x < subjectRoleVec3.x)//如果目标在左边
            {
                if (_ABS(ObjectRoleVec3.x - subjectRoleVec3.x) > this->_structRoleBehaviorTag.fApproachDistanceX)
                {
                    nMoveDirectionX = ROLE_DIRECTION_4_LEFT;
                }
            }

            int nMoveDirectionY = -1;//即将竖向移动的方向
            if (ObjectRoleVec3.y > subjectRoleVec3.y)
            {
                if (_ABS(ObjectRoleVec3.y - subjectRoleVec3.y) > this->_structRoleBehaviorTag.fApproachDistanceY - 5)
                {
                    nMoveDirectionY = ROLE_DIRECTION_4_UP;
                }
            }
            else if (ObjectRoleVec3.y < subjectRoleVec3.y)
            {
                if (_ABS(ObjectRoleVec3.y - subjectRoleVec3.y) > this->_structRoleBehaviorTag.fApproachDistanceY - 5)
                {
                    nMoveDirectionY = ROLE_DIRECTION_4_DOWN;
                }
            }

            //横向移动
            if (nMoveDirectionX != -1)
            {
                if (this->_npcPathDetection(nMoveDirectionX,MOVE_STEP_LENGHT))
                {
                    this->runRoleAction((nMoveDirectionX == ROLE_DIRECTION_4_RIGHT) ?ROLE_MOVE_RIGHT : ROLE_MOVE_LEFT);
                }
                else
                {
                    this->_npcPathFindAndMove_Y(nMoveDirectionX, ROLE_DIRECTION_4_DOWN, ROLE_DIRECTION_4_UP);
                    break;
                }
            }

            if (nMoveDirectionY != -1)
            {
                if (_npcPathDetection(nMoveDirectionY, MOVE_STEP_LENGHT))
                {
                    this->runRoleAction((nMoveDirectionY == ROLE_DIRECTION_4_UP) ?
                        ROLE_MOVE_UP
                        : ROLE_MOVE_DOWN);
                }
            }


            if (nMoveDirectionX == -1 && nMoveDirectionY == -1)
            {
                if (this->_structRoleBehaviorTag.nCounterBehavior > 0)
                {

                    if(this->getActionByTag(ROLE_FIST_ATTACK) == nullptr)
                    {
                        --this->_structRoleBehaviorTag.nCounterBehavior;
                        this->runRoleAction(ROLE_FIST_ATTACK);
                    }

                }
                else
                {
                    this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
                }
            }
        }else 
        {
                this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
        }
        break;
    }
    default:
    {
        #if DEBUG_MODE
            CCLOG("at function CRole::nupBehavior,Parameter error,the value is %d!", this->_structRoleBehaviorTag.nBehaviorTag);
        #endif
        this->_structRoleBehaviorTag.nBehaviorTag = ROLE_THINK0_SEELP;
        break;
    }
    }
}


//运行角色动作 ---- run role ation
//参数type:动作类型，已经在Role.h中宏定义
void CRole::runRoleAction(const int p_nType)
{
    switch (p_nType)
    {
    case ROLE_MOVE:       //移动动画
    {
        if (this->getActionByTag(ROLE_MOVE) == nullptr)
        {
            Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_MOVE));
            animate->setTag(ROLE_MOVE);
            this->runAction(animate);
            this->_structRoleStateFlg.bIsMove   = true;
            this->_structRoleStateFlg.bIsRun    = false;
            this->_structRoleStateFlg.bIsNormal = false;
        }
        break;
    }
    case ROLE_RUN:        //跑步动画
    {
        if (this->getActionByTag(ROLE_RUN) == nullptr)
        {
            Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_RUN));
            animate->setTag(ROLE_RUN);
            this->runAction(animate);
            this->_structRoleStateFlg.bIsRun          = true;
            this->_structRoleStateFlg.bIsMove         = false;
            this->_structRoleStateFlg.bIsNormal       = false;
        }
        break;
    }
    case ROLE_JUMP:       //跳跃动画
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_JUMP));
        JumpBy* jumpAtion = nullptr;
        animate->setTag(ROLE_JUMP);
        this->runAction(animate);
        if (!this->_structRoleStateFlg.bIsRun)
        {
            jumpAtion = JumpBy::create(0.8f, Vec2(0, 0), 120, 1);
            jumpAtion->setTag(ROLE_JUMP_SITU);
            this->runAction(jumpAtion);
        }
        else
        {
            if (this->getRoleDirection())
            { 
                jumpAtion = JumpBy::create(0.8f, Vec2(180, 0), 120, 1);
                jumpAtion->setTag(ROLE_JUMP_RIGHT);
                this->runAction(jumpAtion);
                this->RoleJumpStateJudge(0.8f);
            }
            else
            { 
                jumpAtion = JumpBy::create(0.8f, Vec2(-180, 0), 120, 1);
                jumpAtion->setTag(ROLE_JUMP_LEFT);
                this->runAction(jumpAtion);
                this->RoleJumpStateJudge(0.8f);
            }
            this->_structRoleStateFlg.bIsRun = false;
        }
        this->_structRoleStateFlg.bIsJump         =  true;
        this->_structRoleStateFlg.bIsRun          =  false;
        this->_structRoleStateFlg.bIsMove         =  false;
        this->_structRoleStateFlg.bIsNormal       =  false;
        break;
    }
    case ROLE_MOVE_UP:    //角色向上移动动画
    {
        this->runRoleAction(ROLE_MOVE);
        Vec3 thisVec3 = this->getRoleVec3();
        thisVec3.y += 10;
        short nLimit = CMapTools::getInstance().mapJudge(&thisVec3, ROLE_DIRECTION_4_UP);
        if((nLimit & CAN_NOT_UP) == 0)
        {
            MoveBy* moveby = MoveBy::create(MOVE_STEP_TIME, Vec2(0, MOVE_STEP_LENGHT));
            moveby->setTag(ROLE_MOVE_UP);
            this->runAction(moveby);
        }
        else
        {
            this->stopActionByTag(ROLE_MOVE_UP);
            RoleMovingStateJudge(nLimit, ROLE_MOVE);
        }
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_MOVE_DOWN:  //角色向下移动动画
    {
        this->runRoleAction(ROLE_MOVE);
        Vec3 thisVec3 = this->getRoleVec3();
        thisVec3.y -= 10;
        short nLimit = (CMapTools::getInstance().mapJudge(&thisVec3, ROLE_DIRECTION_4_DOWN));
        if((nLimit & CAN_NOT_DOWN) == 0)
        {
            MoveBy* moveby = MoveBy::create(MOVE_STEP_TIME, Vec2(0, -MOVE_STEP_LENGHT));
            moveby->setTag(ROLE_MOVE_DOWN);
            this->runAction(moveby);
        }
        else
        {
            this->stopActionByTag(ROLE_MOVE_DOWN);
            RoleMovingStateJudge(nLimit, ROLE_MOVE);
        }
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_MOVE_LEFT:  //角色向左移动动画
    {
        this->getRoleDirection(ROLE_DIRECTION_LEFT);
        this->runRoleAction(ROLE_MOVE);  
        Vec3 thisVec3 = this->getRoleVec3();
        thisVec3.x -= 10;
        short nLimit = (CMapTools::getInstance().mapJudge(&thisVec3, ROLE_DIRECTION_4_LEFT));
        if ((nLimit & CAN_NOT_LEFT) == 0)
        {
            MoveBy* moveby = MoveBy::create(MOVE_STEP_TIME, Vec2(-MOVE_STEP_LENGHT, 0));
            moveby->setTag(ROLE_MOVE_LEFT);
            this->runAction(moveby);
        }
        else
        {
            this->stopActionByTag(ROLE_MOVE_LEFT);
            RoleMovingStateJudge(nLimit, ROLE_MOVE);
        }
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_MOVE_RIGHT: //角色向右移动动画
    {
        this->getRoleDirection(ROLE_DIRECTION_RIGHT);
        this->runRoleAction(ROLE_MOVE);
        Vec3 thisVec3 = this->getRoleVec3();
        thisVec3.x += 10;
        short nLimit = (CMapTools::getInstance().mapJudge(&thisVec3, ROLE_DIRECTION_4_RIGHT));
        if ((nLimit & CAN_NOT_RIGHT) == 0)
        {
            MoveBy* moveby = MoveBy::create(MOVE_STEP_TIME, Vec2(MOVE_STEP_LENGHT, 0));
            moveby->setTag(ROLE_MOVE_RIGHT);
            this->runAction(moveby);
        }
        else
        {
            this->stopActionByTag(ROLE_MOVE_RIGHT);
            RoleMovingStateJudge(nLimit, ROLE_MOVE);
        }
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_RUN_LEFT:  //角色向左跑动画
    {
        this->getRoleDirection(ROLE_DIRECTION_LEFT);
        this->runRoleAction(ROLE_RUN);
        Vec3 thisVec3 = this->getRoleVec3();
        thisVec3.x -= 30;
        short nLimit = (CMapTools::getInstance().mapJudge(&thisVec3, ROLE_DIRECTION_4_RIGHT));
        if ((nLimit & CAN_NOT_LEFT) == 0)
        {
            this->runAction(MoveBy::create(RUN_STEP_TIME, Vec2(-RUN_STEP_LENGHT, 0)));
        }
        else
        {
            this->stopActionByTag(ROLE_MOVE_RIGHT);
            if (RoleMovingStateJudge(nLimit, ROLE_MOVE) == 0)
            { 
                this->roleAttacked();
            }
        }
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_RUN_RIGHT: //角色向右跑动画
    {
        this->getRoleDirection(ROLE_DIRECTION_RIGHT);
        this->runRoleAction(ROLE_RUN);
        Vec3 thisVec3 = this->getRoleVec3();
        thisVec3.x += 30;
        short nLimit = (CMapTools::getInstance().mapJudge(&thisVec3, ROLE_DIRECTION_4_RIGHT));
        if ((nLimit & CAN_NOT_RIGHT) == 0)
        {
            this->runAction(MoveBy::create(RUN_STEP_TIME, Vec2(RUN_STEP_LENGHT, 0)));
        }
        else
        {
            if (RoleMovingStateJudge(nLimit, ROLE_MOVE) == 0)
            {
                this->roleAttacked();
            }
        }
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_BRAKING://角色制动
    {
        this->stopActionByTag(ROLE_MOVE_UP);
        this->stopActionByTag(ROLE_MOVE_DOWN);
        this->stopActionByTag(ROLE_MOVE_LEFT);
        this->stopActionByTag(ROLE_MOVE_RIGHT);
        this->stopActionByTag(ROLE_RUN_LEFT);
        this->stopActionByTag(ROLE_RUN_RIGHT);
        this->_structRoleStateFlg.bIsMove = false;
        this->_structRoleStateFlg.bIsRun = false;
        break;
    }
    case ROLE_FIST_ATTACK: //角色拳头攻击
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_FIST_ATTACK));
        animate->setTag(ROLE_FIST_ATTACK);
        this->runAction(animate);
        this->roleAttack(ROLE_FIST_ATTACK);//调用攻击函数
        this->_structRoleStateFlg.bIsAttack       = true;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_LEG_ATTACK: //角色脚攻击
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_LEG_ATTACK));
        animate->setTag(ROLE_LEG_ATTACK);
        this->runAction(animate);
        this->roleAttack(ROLE_LEG_ATTACK);//调用攻击函数
        this->_structRoleStateFlg.bIsAttack       = true;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_UPPERCUT: //角色上勾拳
    {
        this->stopActionByTag(ROLE_MOVE);
        this->stopActionByTag(ROLE_RUN);
        this->stopActionByTag(ROLE_JUMP);
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_UPPERCUT));
        animate->setTag(ROLE_UPPERCUT);
        this->runAction(animate);
        if(this->_structRoleStateFlg.bIsRun)
        {
            if (this->getRoleDirection()){
                this->runAction(MoveBy::create(0.4f, Vec2(80, 0)));}
            else {
                this->runAction(MoveBy::create(0.4f, Vec2(-80, 0)));}
            this->_structRoleStateFlg.bIsRun = false;
        }
        this->roleAttack(ROLE_UPPERCUT);
        this->_structRoleStateFlg.bIsAttack       = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_SLIDING_KICK: //角色滑铲
    {
        if(this->getActionByTag(ROLE_SLIDING_KICK) == nullptr)
        { 
            this->stopActionByTag(ROLE_RUN);
            this->stopActionByTag(ROLE_MOVE);
            Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_SLIDING_KICK));
            animate->setTag(ROLE_SLIDING_KICK);
            this->runAction(animate);
            if(this->getRoleDirection())
                this->runAction(MoveBy::create(0.4f, Vec2(80, 0)));
            else
                this->runAction(MoveBy::create(0.4f, Vec2(-80, 0)));
        }
        this->roleAttack(ROLE_SLIDING_KICK);
        this->_structRoleStateFlg.bIsAttack       = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_FLYING_KICK: //角色飞踢
    {
        this->stopActionByTag(ROLE_JUMP);
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_FLYING_KICK));
        animate->setTag(ROLE_FLYING_KICK);
        this->runAction(animate);
        this->roleAttack(ROLE_FLYING_KICK);
        this->_structRoleStateFlg.bIsAttack       = true;
        this->_structRoleStateFlg.bIsJump         = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_FRONT_SHAKE_BACK: //角色正面被攻击后摇
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_FRONT_SHAKE_BACK));
        animate->setTag(ROLE_FRONT_SHAKE_BACK);
        this->runAction(animate);
        if (this->getRoleDirection())
            this->runAction(MoveBy::create(0.1f, Vec2(-10, 0)));
        else
            this->runAction(MoveBy::create(0.1f, Vec2(10, 0)));
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsSuperArmor   = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_BEHIND_SHAKE_BACK: //角色背后被攻击后摇
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_BEHIND_SHAKE_BACK));
        animate->setTag(ROLE_BEHIND_SHAKE_BACK);
        this->runAction(animate);
        if (this->getRoleDirection())
            this->runAction(MoveBy::create(0.2f, Vec2(10, 0)));
        else
            this->runAction(MoveBy::create(0.2f, Vec2(-10, 0)));
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsSuperArmor   = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_FRONT_FLY: //角色正面被击飞
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_FRONT_FLY));
        animate->setTag(ROLE_FRONT_FLY);
        this->runAction(animate);
        JumpBy* jumpAction = nullptr;
        if (this->getRoleDirection())
        { 
            jumpAction = JumpBy::create(0.8f, Vec2(-150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
        }
        else
        { 
            jumpAction = JumpBy::create(0.8f, Vec2(150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
        }
        this->runAction(jumpAction);
        this->RoleJumpStateJudge(0.8f);
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_BEHIND_FLY: //角色背后被击飞
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_BEHIND_FLY));
        animate->setTag(ROLE_BEHIND_FLY);
        this->runAction(animate);
        JumpBy* jumpAction = nullptr;
        if (this->getRoleDirection())
        {
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
        }
        else
        {
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(-150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
        }
        this->runAction(jumpAction);
        this->RoleJumpStateJudge(0.8f);
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_JUMPING_HIT_FRONT_FLY://角色跳跃时正面被击飞
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_JUMPING_HIT_FRONT_FLY));
        this->runAction(animate1);
        animate1->setTag(ROLE_JUMPING_HIT_FRONT_FLY);
        if (this->getRoleDirection())
        {
            this->runAction(JumpBy::create(0.5,Vec2(-140.f,-(this->getRolePosionZ())),140.f-(this->getRolePosionZ()),1));
            
        }
        else
        {
            this->runAction(JumpBy::create(0.5, Vec2(140.f, -(this->getRolePosionZ())), 140.f - (this->getRolePosionZ()), 1));
        }
        this->RoleJumpStateJudge(0.5f);
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsJump   = false;
        this->_structRoleStateFlg.bIsRun    = false;
        this->_structRoleStateFlg.bIsMove   = false;
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_JUMPING_HIT_BEHIND_FLY://角色跳跃时正面被击飞
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_BEHIND_FLY));
        this->runAction(animate1);
        animate1->setTag(ROLE_JUMPING_HIT_BEHIND_FLY);
        if (this->getRoleDirection())
        {
            this->runAction(JumpBy::create(0.5, Vec2(140.f, -(this->getRolePosionZ())), 140.f - (this->getRolePosionZ()), 1));
        }
        else
        {
            this->runAction(JumpBy::create(0.5, Vec2(-140.f, -(this->getRolePosionZ())), 140.f - (this->getRolePosionZ()), 1));
        }
        this->RoleJumpStateJudge(0.5f);
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsJump   = false;
        this->_structRoleStateFlg.bIsRun    = false;
        this->_structRoleStateFlg.bIsMove   = false;
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_ANGRY: //角色生气
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_ANGRY));
        animate->setTag(ROLE_ANGRY);
        this->runAction(animate);
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_COMA: //角色昏迷
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate->setTag(ROLE_COMA);
        this->runAction(animate);
        this->_structRoleAbility.nComaTimer = MyTool::GetSecond();
        this->_structRoleStateFlg.bIsComa         = true;
        this->_structRoleStateFlg.bIsStupid       = false;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        break;
    }
    case ROLE_COMA_ATTACKED: //角色昏迷时被攻击
    {
        this->stopAllActions();
        Animate* animate = Animate::create(_vecRoleAnimation.at(ROLE_COMA_ATTACKED));
        animate->setTag(ROLE_COMA_ATTACKED);
        this->runAction(animate);
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }


    //组合动画
    case ROLE_DIE://角色死亡动画
    {
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        Spawn* spawn = Spawn::createWithTwoActions(animate1, Blink::create(2.5f, 6));
        spawn->setTag(ROLE_DIE);
        this->runAction(spawn);
        this->_structRoleStateFlg.bIsDie = true;
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsComa = true;
        this->_structRoleStateFlg.bIsSuperArmor = true;
        this->_structRoleStateFlg.bIsRun = false;
        this->_structRoleStateFlg.bIsMove = false;
        this->_structRoleStateFlg.bIsNormal = false;
        this->schedule([&](float s)->void {
            this->roleDeath();
            }, 3.f, "ROLE_DIE");
        break;
    }
    case ROLE_FRONT_FLY_AND_COMA: //角色正面被击飞然后倒地组合动作
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_FRONT_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_FRONT_FLY);
        animate2->setTag(ROLE_COMA);
        Sequence* sequence = Sequence::createWithTwoActions(animate1, animate2);
        if (this->getRoleDirection())
        { 
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(-150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        else
        { 
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        this->runAction(sequence);
        this->RoleJumpStateJudge(0.8f);
        this->_structRoleAbility.nComaTimer = MyTool::GetSecond();
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsComa         = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;

        this->_structRoleStateFlg.bIsSuperArmor = true;
        this->scheduleOnce([role = this](float s)->void {
            role->_structRoleStateFlg.bIsNormal = false;
            }, 0.3f,"SuperArmor");
        break;
    }
    case ROLE_BEHIND_FLY_AND_COMA: //角色被偷袭后击飞然后倒地组合动作
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_BEHIND_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_BEHIND_FLY);
        animate2->setTag(ROLE_COMA);
        Sequence* sequence = Sequence::createWithTwoActions(animate1, animate2);
        this->runAction(sequence);
        if (this->getRoleDirection())
        {
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        else
        { 
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(-150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        this->RoleJumpStateJudge(0.8f);
        this->_structRoleAbility.nComaTimer = MyTool::GetSecond();
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsComa         = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;

        this->_structRoleStateFlg.bIsSuperArmor = true;
        this->scheduleOnce([role = this](float s)->void {
            role->_structRoleStateFlg.bIsNormal = false;
            }, 0.3f, "SuperArmor");

        break;
    }
    //死亡-角色正面被击飞然后倒地组合动作,由ROLE_FRONT_FLY和ROLE_COMA，死亡闪烁顺序执行
    case ROLE_FRONT_FLY_AND_COMA_AND_DIE: 
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_FRONT_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_FRONT_FLY);
        animate2->setTag(ROLE_COMA);
        Spawn* spawn = Spawn::createWithTwoActions(animate2, Blink::create(2.5f, 6));
        Sequence* sequence = Sequence::createWithTwoActions(animate1, spawn);
        this->runAction(sequence);
        if (this->getRoleDirection())
        { 
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(-150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        else
        { 
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        this->RoleJumpStateJudge(0.8f);
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsComa         = true;
        this->_structRoleStateFlg.bIsDie          = true;
        this->_structRoleStateFlg.bIsSuperArmor   = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;

        //调度器延时死亡函数
        this->scheduleOnce([role = this](float s)->void {
            role->roleDeath();
            }, 3.7f, "ROLE_DIE");
        break;
    }
    //死亡-角色背面被击飞然后倒地组合动作,由ROLE_BEHIND_FLY和ROLE_COMA，死亡闪烁顺序执行
    case ROLE_BEHIND_FLY_AND_COMA_AND_DIE:
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_BEHIND_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_BEHIND_FLY);
        animate2->setTag(ROLE_COMA);
        Spawn* spawn = Spawn::createWithTwoActions(animate2, Blink::create(2.5f, 6));
        Sequence* sequence = Sequence::createWithTwoActions(animate1, spawn);
        this->runAction(sequence);
        if (this->getRoleDirection())
        { 
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        else
        { 
            JumpBy* jumpAction = JumpBy::create(0.8f, Vec2(-150, 0), 140, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        this->RoleJumpStateJudge(0.8f);
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsComa         = true;
        this->_structRoleStateFlg.bIsDie          = true;
        this->_structRoleStateFlg.bIsSuperArmor   = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        //调度器延时死亡函数
        this->schedule([&](float s)->void {
            this->roleDeath();
            }, 3.7f, "ROLE_DIE");
        break;
    }



    case ROLE_JUMPING_FRONT_FLY_AND_COMA://角色跳跃时正面被击飞然后倒地组合动作
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_JUMPING_HIT_FRONT_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_JUMPING_HIT_FRONT_FLY);
        animate2->setTag(ROLE_COMA);
        Sequence* sequence = Sequence::createWithTwoActions(animate1, animate2);
        if (this->getRoleDirection())
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(-70, -(this->getRolePosionZ())), 60, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        else
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(70, -(this->getRolePosionZ())), 60, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        this->runAction(sequence);
        this->RoleJumpStateJudge(0.5f);
        this->_structRoleAbility.nComaTimer = MyTool::GetSecond();
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsComa = true;
        this->_structRoleStateFlg.bIsRun = false;
        this->_structRoleStateFlg.bIsMove = false;
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_JUMPING_BEHIND_FLY_AND_COMA://角色跳跃时背面被击飞然后倒地组合动作
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_JUMPING_HIT_BEHIND_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_JUMPING_HIT_BEHIND_FLY);
        animate2->setTag(ROLE_COMA);
        Sequence* sequence = Sequence::createWithTwoActions(animate1, animate2);
        this->runAction(sequence);
        if (this->getRoleDirection())
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(30, -(this->getRolePosionZ())),60, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        else
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(-30, -(this->getRolePosionZ())), 60, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        this->RoleJumpStateJudge(0.5f);
        this->_structRoleAbility.nComaTimer = MyTool::GetSecond();
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsComa = true;
        this->_structRoleStateFlg.bIsRun = false;
        this->_structRoleStateFlg.bIsMove = false;
        this->_structRoleStateFlg.bIsNormal = false;
        break;
    }
    case ROLE_JUMPING_FRONT_FLY_AND_DIE://角色跳跃时正面撞墙击飞+死亡闪烁
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_JUMPING_HIT_FRONT_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_FRONT_FLY);
        animate2->setTag(ROLE_COMA);
        Spawn* spawn = Spawn::createWithTwoActions(animate2, Blink::create(2.5f, 6));
        Sequence* sequence = Sequence::createWithTwoActions(animate1, spawn);
        this->runAction(sequence);
        if (this->getRoleDirection())
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(-70, -(this->getRolePosionZ())), 60, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        else
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(70, -(this->getRolePosionZ())),60, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        this->RoleJumpStateJudge(0.5f);
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsComa = true;
        this->_structRoleStateFlg.bIsDie = true;
        this->_structRoleStateFlg.bIsSuperArmor = true;
        this->_structRoleStateFlg.bIsRun = false;
        this->_structRoleStateFlg.bIsMove = false;
        this->_structRoleStateFlg.bIsNormal = false;

        //调度器延时死亡函数
        this->scheduleOnce([&](float s)->void {
            this->roleDeath();
            }, 3.2f, "ROLE_DIE");
        break;
    }
    case ROLE_JUMPING_BEHIND_FLY_AND_DIE://角色跳跃时背面撞墙击飞+死亡闪烁
    {
        this->stopAllActions();
        Animate* animate1 = Animate::create(_vecRoleAnimation.at(ROLE_JUMPING_HIT_BEHIND_FLY));
        Animate* animate2 = Animate::create(_vecRoleAnimation.at(ROLE_COMA));
        animate1->setTag(ROLE_BEHIND_FLY);
        animate2->setTag(ROLE_COMA);
        Spawn* spawn = Spawn::createWithTwoActions(animate2, Blink::create(2.5f, 6));
        Sequence* sequence = Sequence::createWithTwoActions(animate1, spawn);
        this->runAction(sequence);
        if (this->getRoleDirection())
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(30, -(this->getRolePosionZ())),60, 1);
            jumpAction->setTag(ROLE_JUMP_LEFT);
            this->runAction(jumpAction);
        }
        else
        {
            JumpBy* jumpAction = JumpBy::create(0.5f, Vec2(-30, -(this->getRolePosionZ())), 60, 1);
            jumpAction->setTag(ROLE_JUMP_RIGHT);
            this->runAction(jumpAction);
        }
        this->RoleJumpStateJudge(0.5f);
        this->_structRoleStateFlg.bIsStupid = true;
        this->_structRoleStateFlg.bIsComa = true;
        this->_structRoleStateFlg.bIsDie = true;
        this->_structRoleStateFlg.bIsSuperArmor = true;
        this->_structRoleStateFlg.bIsRun = false;
        this->_structRoleStateFlg.bIsMove = false;
        this->_structRoleStateFlg.bIsNormal = false;

        //调度器延时死亡函数
        this->scheduleOnce([&](float s)->void {
            this->roleDeath();
            }, 3.2f, "ROLE_DIE");
        break;
    }
    case ROLE_DROWNING://角色溺水动画
    {
        this->stopAllActions();
        this->unscheduleAllCallbacks();
        Animate* animete = Animate::create(_vecRoleAnimation.at(ROLE_JUMP));
        Sequence* sequence = Sequence::createWithTwoActions(JumpBy::create(0.5f, Vec2(50, 0), 50, 1),
            MoveBy::create(0.3f,Vec2(0,-120)));
        sequence->setTag(ROLE_DROWNING);
        this->runAction(animete);
        this->runAction(sequence);
        this->_structRoleStateFlg.bIsDie          = true;
        this->_structRoleStateFlg.bIsStupid       = true;
        this->_structRoleStateFlg.bIsSuperArmor   = true;
        this->_structRoleStateFlg.bIsRun          = false;
        this->_structRoleStateFlg.bIsMove         = false;
        this->_structRoleStateFlg.bIsNormal       = false;
        this->scheduleOnce([&](float s)->void {
            CEffects::getInstance()->runEffects(this->getParent(),this->getPositionX() ,this->getRolePosionY() , WATERSPRAY);
            }, 0.5f, "ROLE_WATERSPRAY");
        CBattleScene::getInstance()->generateCover(this);
        //角色死亡
        this->scheduleOnce([&](float s)->void {
            this->roleDeath();
            }, 3.f, "ROLE_DIE");
    }
    default:
    {
        break;
    }
    }
}
