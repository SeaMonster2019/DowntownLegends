#pragma once
#include <random>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <memory>
#include <functional>
#include "cocos2d.h"
#include "jsonclass.h"
#include "MyTool.h"
#include "CMaptools.h"
#include "CBattleScene.h"
#include "CMyUi.h"

//角色左右方向标记宏
#define ROLE_DIRECTION_RIGHT true
#define RIGHT                true
#define ROLE_DIRECTION_LEFT  false
#define LEFT                 false

//伤害方向标记宏
#define FRONT  true
#define BEHIND false

//NPC性格宏定义
#define NPC_CHARACTER_INTEGRITY	       0x0001              //正直
#define NPC_CHARACTER_BOLDNESS		   0x0002             //大胆
#define NPC_CHARACTER_CUNING           0x0004            //"聪明"
#define NPC_CHARACTER_PATIENT          0x0008           //耐心
#define NPC_CHARACTER_LOYALTY          0x0010          //义气

#define NPC_CHARACTER_VICIOUS          0x0100        //恶毒
#define NPC_CHARACTER_TIMID            0x0200       //胆小
#define NPC_CHARACTER_STUPID           0x0400      //愚笨
#define NPC_CHARACTER_IMPATIENCE       0x0800     //急躁
#define NPC_CHARACTER_TREACHERY        0x1000    //奸诈

//角色阵营、身份宏
#define ROLE_CAMP_JUDGE(x)           ((x) &  0x03)     //提取阵营数          
#define ROLE_CAMP1                   0x00            //第一阵营(主角阵营)
#define ROLE_CAMP2                   0x01           //第二阵营
#define ROLE_CAMP3                   0x02          //第三阵营
#define ROLE_CAMP4                   0x03         //第四阵营
#define ROLE_IDENTILY_SUBORDINATE    0x04        //小弟
#define ROLE_IDENTILY_CADRE          0x08       //干部
#define ROLE_IDENTILY_BOSS           0x10      //boss
#define ROLE_IDENTILY_LEAD_CAMP      0x20     //打手小弟（可以跟随主角切换地图）


using namespace std;
USING_NS_CC;

struct SHeadUi;

//角色动画类型枚举
enum ERoleAtionType
{
	//单个动作动画标记宏,用于表示animation容器下标、设置animate tag 等。
	ROLE_NORMAL = 0,  //通常动画
	ROLE_MOVE = 1,  //移动动画
	ROLE_RUN = 2,  //跑步动画
	ROLE_JUMP = 3,  //跳跃动画
	ROLE_FIST_ATTACK = 4,  //拳击动画
	ROLE_LEG_ATTACK = 5,  //踢腿动画
	ROLE_UPPERCUT = 6,  //上勾拳动画
	ROLE_SLIDING_KICK = 7,  //滑踢动画
	ROLE_FLYING_KICK = 8,  //飞踢动画
	ROLE_FRONT_SHAKE_BACK = 9,  //正面被攻击后摇
	ROLE_BEHIND_SHAKE_BACK = 10, //背面被攻击后摇
	ROLE_FRONT_FLY = 11, //正面被攻击击飞
	ROLE_BEHIND_FLY = 12, //背面被攻击击飞
	ROLE_JUMPING_HIT_FRONT_FLY = 13, //角色跳跃时被击飞
	ROLE_JUMPING_HIT_BEHIND_FLY = 14, //角色跳跃时被击飞
	ROLE_ANGRY = 15, //生气动画
	ROLE_COMA = 16, //昏迷动画
	ROLE_COMA_ATTACKED = 17, //昏迷后被攻击动画
	MAX__ROLE_ATION_NUMBER = 18,//基础动画最大数目

	//移动动作动画标记宏
	ROLE_MOVE_UP = 20, //向上移动
	ROLE_MOVE_DOWN, //向下移动
	ROLE_MOVE_LEFT, //向左移动
	ROLE_MOVE_RIGHT, //向右移动
	ROLE_RUN_LEFT, //向左跑动
	ROLE_RUN_RIGHT, //向右跑动
	ROLE_JUMP_SITU, //原地跳动
	ROLE_JUMP_LEFT, //向左跳动
	ROLE_JUMP_RIGHT, //向右跳动
	ROLE_BRAKING, //紧急制动

	//组合动作标记
	ROLE_DIE = 50, //角色死亡（倒地昏迷+闪烁）
	ROLE_ATTACKED, //角色被攻击
	ROLE_FRONT_FLY_AND_COMA, //角色正面击飞+倒地昏迷
	ROLE_BEHIND_FLY_AND_COMA, //角色背面击飞+倒地昏迷
	ROLE_FRONT_FLY_AND_COMA_AND_DIE, //角色正面击飞+倒地昏迷+闪烁
	ROLE_BEHIND_FLY_AND_COMA_AND_DIE, //角色背面击飞+倒地昏迷+闪烁
	ROLE_JUMPING_FRONT_FLY_AND_COMA, //角色跳跃时正面击飞+倒地昏迷
	ROLE_JUMPING_BEHIND_FLY_AND_COMA, //角色跳跃时背面击飞+倒地昏迷
	ROLE_JUMPING_FRONT_FLY_AND_DIE, //角色跳跃时正面击飞+倒地昏迷+死亡闪烁
	ROLE_JUMPING_BEHIND_FLY_AND_DIE, //角色跳跃时背面击飞+倒地昏迷+死亡闪烁
	ROLE_DROWNING  //角色溺水
};


//角色行为类型枚举
enum ERoleBehaviorType
{
	//思考动作
	ROLE_THINK0_SEELP = -1,    //睡眠
	ROLE_THINK0_WAKE = 0,     //唤醒
	ROLE_THINK0_STRATEGY = 1,     //战略思考
	ROLE_THINK0_DEFENSE = 2,    //防守思考
	ROLE_THINK0_PATHFINDING = 3,    //寻路
	ROLE_THINK0_TRANCE = 4,        //发呆
	ROLE_REPEAT0_BEHAVIOR = 5,    //重复行为
	

	//策略动作
	ROLE_BEHAVIOR1_LOCKING = 101,   //锁定
	ROLE_BEHAVIOR1_RETREAT = 102,   //撤退
	ROLE_BEHAVIOR1_LOCKING_LEAD = 103,   //锁定主角

	//预执行动作
	ROLE_BEHAVIOR2_TARGET_NEAR = 201,   //接近目标
	ROLE_BEHAVIOR2_PUSH = 202,   //冲击
	ROLE_BEHAVIOR2_SNEAK = 203,  //偷袭
	ROLE_BEHAVIOR2_RUN = 204,  //逃跑
	ROLE_BEHAVIOR2_NEAR_FREAD = 205,  //接近友军
	ROLE_BEHAVIOR2_FLOW_LEAD = 206,  //跟随主角

	//执行动作、攻击动作
	ROLE_BEHAVIOR3_FIST = 301,  //拳头攻击
	ROLE_BEHAVIOR3_LEG = 302,  //脚攻击
	ROLE_BEHAVIOR3_SLIDING_KICK = 303,  //滑踢攻击 
	ROLE_BEHAVIOR3_COMBINATION_BOXING = 304,  //随机组合拳
	ROLE_BEHAVIOR3_CONTINUOUS_JAB = 305,  //连续刺拳

	ROLE_BEHAVIOR3_UPPERCUT = 306,  //冲击上勾拳
	ROLE_BEHAVIOR3_FLY_UPPERCUT = 307, //飞天上勾拳
	ROLE_BEHAVIOR3_FLY_KICK = 308,  //飞踢攻击
	ROLE_BEHAVIOR3_RUN_FIND_WAY = 399, //逃跑时寻路  

	//收尾动作
	ROLE_BEHAVIOR4_SHORT_BACK = 401,  //短后退         
	ROLE_BEHAVIOR4_LONG_BACK = 402, //长后退


	NPC_CHEAK_TARGET_IS_ATTAINABLE = 1001,     //可被攻击
	NPC_CHEAK_TARGET_IS_COMA = 1002,     //对象是否昏迷
	NPC_CHEAK_TARGET_IS_DIE = 1003,  //对象是否死亡
	NPC_CHEAK_TARGET_IS_COMA_OR_DIE = 1004   //对象是否昏迷或死亡

};


//角色二维方向枚举
enum ERoleDirection4
{
	ROLE_DIRECTION_4_UP = 1,
	ROLE_DIRECTION_4_DOWN = 2,
	ROLE_DIRECTION_4_RIGHT = 4,
	ROLE_DIRECTION_4_LEFT = 8
};


//角色编号结构体
struct SRoleNumber
{
public:
	int capTag;
	int roleNuber;
	SRoleNumber(int x, int y) :capTag(x), roleNuber(y) {};
	bool operator==(SRoleNumber& p)
	{
		if (this->capTag == p.capTag && this->roleNuber == p.roleNuber)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	bool operator!=(SRoleNumber& p)
	{
		if (this->capTag != p.capTag || this->roleNuber != p.roleNuber)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
};


//角色属性结构体
struct SRoleBaciseAbility
{
	//需要从文件读取的属性
	string sRoleCode;           //角色的代号  文件名 全部人物唯一的字符串标识
	string sRoleName;          //角色的名字  人物名字
	int nRoleNumber;          //角色的号码  在角色数组里的位址
	int nRoleNumberCode;     //角色编号代码  全部人物唯一的数字标识 
	bool bShowInformation;  //是否在显示角色信息  是否在UI上显示角色的当前数值
	SHeadUi* headUiPtr;    //Ui显示部件指针

	int nMaxHP;                      //角色的最大生命值
	int nATK;                       //角色的攻击力
	int nDEF;                      //角色的防御力
	unsigned short unCharacter;   //角色性格
	
	//需要程序生成的属性
	int nCurrentHP;                //角色当前生命值
	int nHardness;                //角色抗击倒能力
	int nCurrentHardness;        //角色硬直度
	unsigned int nHardnessRecoveryTimer; //硬直度恢复计时器
	unsigned int nComaTimer;            //昏迷时间计时器
	
};


//角色状态标志结构体
struct SRoleStateFlg
{
	float fTrueYAxis;                    //辅助Y轴
	short nCampTab;                     //阵营标志
	bool  bIsNormal;                   //通常状态
	bool  bIsMove;                    //移动状态
	bool  bIsRun;                    //奔跑状态
	bool  bIsJump;                  //跳跃状态
	bool  bIsAttack;               //攻击状态
	bool  bIsStupid;              //发呆状态
	bool  bIsComa;               //倒地状态
	bool  bIsSuperArmor;        //霸体状态
	bool  bIsDie;              //是否死亡
	bool  bDirection;         //角色方向
};


//角色行为标志结构体
struct SRoleBehavior
{
	int nBehaviorTag;                  //行为标志
	int nTargetNumber;                //目标编号
	int nTargetCamp;                 //目标阵营
	int nTargetNumberCode;          //目标编号代码
	int nTargetState;              //目标状态
	int nAttctMode;               //攻击方式
	float fApproachDistanceX;    //横向接近距离
	float fApproachDistanceY;   //纵轴接近距离
	int   nCounterBehavior;    //行为计数器
	int   nCounterAttack;          //计数器A
	long  nlTimerA;          //计时器A
	long  nlTimerB;         //计时器B
	vector<SRoleNumber> AttackedRoleNumberList;//被攻击目标角色容器 
};


//角色类
class CRole: public  Sprite
{
//友元类：地图类
friend class CMapTools;
friend class CUpMenu;

//角色类属性、资源
private:

	SRoleBaciseAbility _structRoleAbility; //角色能力数据结构体成员

	SRoleStateFlg _structRoleStateFlg;//角色状态数据结构体成员

	SRoleBehavior _structRoleBehaviorTag;//角色行为数据结构体成员

	Vector<Animation*> _vecRoleAnimation;//装载角色动画的vector容器

	static Readjson* _readJsonAnimate, * _readJsonAbility;//角色纹理、动画信息文件]

	static Vector<CRole*> vecBrother[3];

public:

	static std::default_random_engine Role_Random_Engine;

	static Vector<CRole*>* vecRoles[4];  //角色列表,阵营1

	static vector<int> vecBadManRoleNumber[4];//敌对角色编号数组



//角色类接口
public: 

	static CRole*  roleCreate();//create函数

	static bool roleClassInit();//角色类初始化

	virtual bool init() ;//sprite初始化函数

	bool roleSet(const string p_SroleCode,const short p_nCampTab, float p_X,float p_Y);//角色对象初始化函数，参数是角色纹理文件名,阵营，XY轴坐标

	void roleResetAttribute();//重置角色属性

	void runRoleAction(const int p_nType);//运行角色动画，参数为动画类型，已经宏定义

	bool getRoleDirection();//取得角色方向函数，返回真则角色向右，返回假则角色向左
	void getRoleDirection(const bool p_bNeedFlipAiretion);//改变角色方向函数，Bool型参数与角色当前方向（bool型数据）不一致时，绕X翻转图像
	
	void restoreState();//复原函数，当角色没有运行任何动画时，复原角色状态和图像

	//角色攻击函数，对一个装载角色的vector容器里的每个角色判断位置，在攻击范围内的对象调用其被攻击函数
	void roleAttack(const int p_nType);

	//被攻击函数
	void roleAttacked();
	void roleAttacked(const bool p_bATKDirection);
	void roleAttacked(const int p_nAggressivity, const int strength, const bool p_ATKdirection);//角色被攻击函数，被攻击时调用
	bool roleAttackedJudge(Vec3 vec3HitPoint);

	bool roleStateJudge(const int p_nType);//判断角色是否可以运行某一个动作

	short RoleJumpStateJudge(const float p_fTimes);
	short RoleMovingStateJudge(const short p_nLimit,const int n_pType);//判断角色移动后的状态


	//NPC行动三函数 —— AI funtion
	void npcBehavior();//NPC行为函数
	void npcObserveCondition();//NPC角色对形势的判断
	bool npcBeahviorTargetExistenceJudge();

	bool roleCampJudge(const int p_nCampTag);//角色阵营判断
	string getRoleCode();//获取角色代号
	float getRolePosionX();//获取角色X轴数值
	float getRolePosionY();//获取角色Y轴数值
	float getRolePosionZ();//获取角色Z轴数值
	Vec3 getRoleVec3();//获取角色三维
	bool getRoleState(int p_type);//获取角色状态


	
	void roleDeath();//角色死亡函数
	void roleDelete();//角色请离函数

	void setRoleBehaviorTag(const int p_nBehavior);//设置角色行为参数

	static bool vecRolesExistenceJudge(const int nCampTag,const int nRoleNumber);
	static void vecRolesRecovery();//角色容器角色恢复
	static void vecRoleSort(const int p_nVecRolesNumber);//角色容器排序
	static void vecRoleLeaveBehindLead();//从角色容器中删除出了主角外的角色
	static void vecRoleLeaveBehindLeadCamp();//从角色容器中删除除了主角团外的角色
	static void vecRoleDeleteBehindLeadCamp();//从角色容器中请离除了主角团外的角色
	static void vecRoleRemove(const int p_nVecRolesNumber,const int p_nNuber);//从容器中删除一个角色
	static void vecRoleStopAllAtion();//停止角色容器中所有角色的动作
	static void	vecRoleStopAllScheduler();//停止角色容器中所有角色的调度器
	static void vecRoleChangeDirection(const int p_nVecRolesNumber,const bool p_bIsTurnRight);//转换角色容器里所有角色的方向
	static void vecRoleResetAttributeAll();//重置容器内所有角色的数值
	static void vecRoleResetAttribute_LeadCamp();//重置容器内主角团角色的数值
	
	
	static void role_ChangeMap();//切换地图时角色类的设置

	static void vecBadManNumberInit(const int p_nVecRoleNumber,const bool p_bHaveBoss);//初始化敌对角色编号数组

	~CRole();//构析函数

	CREATE_FUNC(CRole);

private:
	
	bool _npcGetCharacter(const int p_nCharacter);//判断角色性格

	void _attackJudgeFist(float s);
	void _attackJudgeLeg(float s);
	void _attackJudgeUppercut(float s);
	void _attackJudgeSlidingKick(float s);
	void _attackJudgeFlyinggKick(float s);

	bool _npcPathDetection(const int p_nDirection, const float p_fDistance);//查看前方是否有阻拦
	int _npcPathDetectionVertical(const int p_nDirection, const int p_nMoveDirection, const float p_fStepLength, const float p_fProbeDistance = 10, const float p_fMax_MoveDistance = 400);
	bool _npcPathFindAndMove_Y(const int p_nDirection, const int p_nFirstMoveDirection, const int p_nSecondMoveDirection);

	bool _npcCheckTargetStatus(const int p_nCheakType);

	std::function<int()> repeatBevior;//运行重复动作
};

