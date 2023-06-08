#include "CMapTools.h"


//加载地图
Sprite* CMapTools::mapLoding(Node* p_node,const int p_nMapName)
{
	if (p_node == nullptr)
	{
		CCLOG("at  CMapTools::mapLoding funtion, parameter p_node is nullptr!");
		return nullptr;
	}

	//创建精灵
	Sprite* mapSprite = nullptr;

	//读取屏幕参数
	const auto visibleSize = Director::getInstance()->getVisibleSize();
	const auto origin = Director::getInstance()->getVisibleOrigin();

	//判断参数中地图数据
	const short nMapName = MAP_GET_MAPMACRO(p_nMapName);

	this->struMapInformation.nCampNumber[0] = 0;
	this->struMapInformation.nCampNumber[1] = -1;
	this->struMapInformation.nCampNumber[2] = -1;
	this->struMapInformation.nCampNumber[3] = -1;

	//根据地图编号读取地图纹理
	switch (nMapName)
	{
		case MAP_PARK_LAKESIDE :
		{
			
			mapSprite = Sprite::create("map/park/park0.png");
			this->mapJudgeCallBack = &CMapTools::MapBoundaryJudge_Park0;
			
			this->struMapInformation.nBeforMapNumber = -1;
			this->struMapInformation.nNextMapNumber = MAP_PARK_MAIN_ROAD;
			this->struMapInformation.nMapNumber = MAP_PARK_LAKESIDE;
			this->struMapInformation.nCampNumber[1] = -1;
			this->struMapInformation.bExistenceBoss = false;
			this->struMapInformation.bIsSafetyZone = true;

			break;
		}
		case MAP_PARK_MAIN_ROAD:
		{
			mapSprite = Sprite::create("map/park/park1.png");
			this->mapJudgeCallBack = &CMapTools::MapBoundaryJudge_Park1;

			this->struMapInformation.nBeforMapNumber = MAP_PARK_LAKESIDE;
			this->struMapInformation.nNextMapNumber = MAP_PARK_PATH;
			this->struMapInformation.nMapNumber = MAP_PARK_MAIN_ROAD;
			this->struMapInformation.nCampNumber[1] = 7;
			this->struMapInformation.bExistenceBoss = false;
			this->struMapInformation.bIsSafetyZone = false;
			break;
		}
		case MAP_PARK_PATH:
		{
			this->struMapInformation.nMapNumber = MAP_PARK_PATH;
			mapSprite = Sprite::create("map/park/park2.png");
			break;
		}
		case MAP_PARK_CENTER:
		{
			this->struMapInformation.nMapNumber = MAP_PARK_CENTER;
			mapSprite = Sprite::create("map/park/park3.png");
			break;
		}
		case MAP_PARK_WC:
		{
			this->struMapInformation.nMapNumber = MAP_PARK_WC;
			mapSprite = Sprite::create("map/park/park4.png");
			break;
		}
		case MAP_PARK_DOORWAY:
		{
			this->struMapInformation.nMapNumber = MAP_PARK_DOORWAY;
			mapSprite = Sprite::create("map/park/park5.png");
			break;
		}
	default:
		printf("Wrong map type");
		return nullptr;
		break;
	}

	if (mapSprite == nullptr)
	{
		CCLOG("mapSprite create false!");
		return nullptr;
	}

	//设置地图参数
	this->struMapInformation.fMapSizeWidth = mapSprite->getContentSize().width;
	this->struMapInformation.fMapSizeHeight = mapSprite->getContentSize().height;
	this->struMapInformation.fViewEdgeLeft = -(struMapInformation.fMapSizeWidth - Director::getInstance()->getVisibleSize().width);
	this->struMapInformation.fViewEdgeRight = 0;
	mapSprite->setAnchorPoint(Vec2(0, 0.5));
	if ((p_nMapName & MAP_CHANGE_IN) == MAP_CHANGE_IN)
	{
		mapSprite->setPosition(0, visibleSize.height / 2 + 50);
	}
	else
	{
		mapSprite->setPosition(Director::sharedDirector()->getVisibleSize().width - mapSprite->getContentSize().width, visibleSize.height / 2 + 50);
	}
	p_node->addChild(mapSprite);

	//生成敌对角色编号
	CRole::vecBadManRoleNumber[1].clear();
	if (this->struMapInformation.nCampNumber[1] != -1 )
	{
		CRole::vecBadManNumberInit(1, this->struMapInformation.bExistenceBoss);
	}

	return mapSprite;
}


//获取加载地图的参数
short CMapTools::getLodingParameter(const short p_nLimit)
{
	if ((this->struMapInformation.nBeforMapNumber == MAP_PARK_LAKESIDE)
		&& MAP_JUDGE(p_nLimit,MAP_CHANGE_OUT))
	{
		return MAP_CHANGE_IN | this->struMapInformation.nBeforMapNumber;
	}


	if (MAP_JUDGE(p_nLimit, MAP_CHANGE_IN))
	{
		return MAP_CHANGE_IN | this->struMapInformation.nNextMapNumber;
	}
	else if (MAP_JUDGE(p_nLimit, MAP_CHANGE_OUT))
	{
		return MAP_CHANGE_OUT | this->struMapInformation.nBeforMapNumber;
	}

	return 0;
}


//角色位置判断
short CMapTools::mapJudge(Vec3* const p_Rolevec3,const int p_nDirection)
{
	return (this->*mapJudgeCallBack)(p_Rolevec3, p_nDirection);
}


//角色探索路径
bool CMapTools::pathDetection(Vec3* const p_vec3,const int p_nDirection, const float p_fDistance)
{
	Vec3 roleVec3 = *(p_vec3);
	short nLimit = 0;
	void (*vce3Operation)(Vec3 *p_vec3) = [](Vec3* p_vec3){};
	switch (p_nDirection)
	{
	case  ROLE_DIRECTION_4_DOWN:
	{
		vce3Operation = [](Vec3* p_vec3) {
			p_vec3->y -= 10;
		};
		break;
	}
	case  ROLE_DIRECTION_4_UP:
	{
		vce3Operation = [](Vec3* p_vec3) {
			p_vec3->y += 10;
		};
		break;
	}
	case  ROLE_DIRECTION_4_LEFT:
	{
		vce3Operation = [](Vec3* p_vec3) {
			p_vec3->x -= 10;
		};
		break;
	}
	case  ROLE_DIRECTION_4_RIGHT:
	{
		vce3Operation = [](Vec3* p_vec3) {
			p_vec3->x += 10;
		};
		break;
	}
	default:
		return false;
	}

	
	int nFrequency = _ABS(p_fDistance) / 10;
	if (_ABS(p_fDistance) - nFrequency * 10 > 0)
	{
		nFrequency++;
	}

	int CanNotDirection = p_nDirection;


	for (int i = 0; i < nFrequency; i++)
	{
		vce3Operation(&roleVec3);
		nLimit |=  (this->*mapJudgeCallBack)(&roleVec3,p_nDirection);
		if ((MAP_JUDGE(nLimit,p_nDirection)))
		{
			return false;
		}
	}


	return true;
}


//视角移动
void CMapTools::lensStretch()
{
	const float fMapX = CBattleScene::getInstance()->mapSprite->getPositionX();
	if (fMapX > this->struMapInformation.fViewEdgeRight)
	{
		CBattleScene::getInstance()->mapSprite->setPositionX(this->struMapInformation.fViewEdgeRight);
		return;
	}
	else if (fMapX < this->struMapInformation.fViewEdgeLeft)
	{
		CBattleScene::getInstance()->mapSprite->setPositionX(this->struMapInformation.fViewEdgeLeft);
		return;
	}

	const float fRoleX = CRole::vecRoles[0]->at(0)->getPositionX();
	const float fRoleUiX = CRole::vecRoles[0]->at(0)->getPositionX() + fMapX;
	const Size visibleSize = Director::sharedDirector()->getVisibleSize();
	if (fRoleUiX > visibleSize.width * 3 / 4  && fMapX > this->struMapInformation.fViewEdgeLeft)
	{
		CBattleScene::getInstance()->mapSprite->setPositionX(fMapX + visibleSize.width * 3 / 4 - fRoleUiX);
	}
	else if (fRoleUiX < visibleSize.width / 4 && fMapX < this->struMapInformation.fViewEdgeRight)
	{
		CBattleScene::getInstance()->mapSprite->setPositionX(fMapX + visibleSize.width / 4 - fRoleUiX);
	}
}


//返回地图信息
SMapInformation& const CMapTools::getMapIformation()
{
	return this->struMapInformation;
}


//生成主角团角色，根据地图设置角色的位置
void CMapTools::roleRoding(const short p_nRodingType)
{
	//判断生成的位置
	if(MAP_JUDGE(p_nRodingType,MAP_CHANGE_IN))
	{ 
		for (int i = 0; i < CRole::vecRoles[0]->size(); i++)
		{
			CRole::vecRoles[ROLE_CAMP1]->at(i)->setPosition(CMapTools::getInstance().RoleLodingSite_LeadTeam[this->struMapInformation.nMapNumber][i]);

			CBattleScene::getInstance()->mapSprite->addChild(CRole::vecRoles[0]->at(i));
		}
		CRole::vecRoleChangeDirection(0,ROLE_DIRECTION_RIGHT);
	}
	else  if(MAP_JUDGE(p_nRodingType, MAP_CHANGE_OUT))
	{
		for (int i = 0; i < CRole::vecRoles[0]->size(); i++)
		{
			CRole::vecRoles[0]->at(i)->setPosition(CMapTools::getInstance().RoleLodingSite_LeadTeam[this->struMapInformation.nMapNumber][4+i]);
			CBattleScene::getInstance()->mapSprite->addChild(CRole::vecRoles[0]->at(i));
		}
		CRole::vecRoleChangeDirection(0,ROLE_DIRECTION_LEFT);
	}

}
//地图加载时生成四个敌对角色
void CMapTools::roleRodingBegin_Badman()
{
	if(this->struMapInformation.bIsSafetyZone == false)
	{
		for (int i = 0; i < 4 && CRole::vecBadManRoleNumber[1].size() > 1; i++)
		{
			roleRodingOnce_Badman(1);
		}
	}
}
//生成一个敌对阵营角色
bool CMapTools::roleRodingOnce_Badman(const int p_nVecRoleNumber) 
{
	if (CRole::vecBadManRoleNumber[p_nVecRoleNumber].size() == 0)
	{
		return false;
	}

	bool ExistenceBadMan;
	//检查是否还存在本阵营的角色
	if (CRole::vecRoles[p_nVecRoleNumber]->size() > 0)
		ExistenceBadMan = true;
	else
		ExistenceBadMan = false;


	//如果编号容器内只剩一个BOSS，应该等场上同阵营角色死光后，才让BOSS出场
	if (struMapInformation.bExistenceBoss == true && CRole::vecBadManRoleNumber[p_nVecRoleNumber].size() == 1 && ExistenceBadMan)
	{
		return false;
	}

	//BOSS出场
	if (struMapInformation.bExistenceBoss == true && CRole::vecBadManRoleNumber[p_nVecRoleNumber].size() == 1 && !ExistenceBadMan)
	{
		string roleCode = "NPC" + to_string(this->struMapInformation.nCampNumber[p_nVecRoleNumber]) + "_" + to_string(*CRole::vecBadManRoleNumber[p_nVecRoleNumber].end());
		CRole::vecBadManRoleNumber[p_nVecRoleNumber].pop_back();
		CRole *role = CRole::roleCreate();

		role->roleSet(roleCode,
			0x01, 
			RoleLodingSite_BadMan[struMapInformation.nMapNumber][4].x,
			RoleLodingSite_BadMan[struMapInformation.nMapNumber][4].y);

		CRole::vecRoles[p_nVecRoleNumber]->pushBack(role);
		CBattleScene::getInstance()->mapSprite->addChild(role);
		
		return true;
	}

	if((struMapInformation.bExistenceBoss == false && CRole::vecBadManRoleNumber[p_nVecRoleNumber].size()>0)
		|| (struMapInformation.bExistenceBoss == true && CRole::vecBadManRoleNumber[p_nVecRoleNumber].size() > 1)
		)
	{
		string roleCode = "NPC" + to_string(this->struMapInformation.nCampNumber[p_nVecRoleNumber]) + "_" + to_string(CRole::vecBadManRoleNumber[p_nVecRoleNumber].back());
		CRole::vecBadManRoleNumber[p_nVecRoleNumber].pop_back();
		CRole* role = CRole::roleCreate();
		Vec2 size = RoleLodingSite_BadMan[struMapInformation.nMapNumber][rand()%4];
		role->roleSet(roleCode,
			0x01,
			size.x,
			size.y);
		CRole::vecRoles[p_nVecRoleNumber]->pushBack(role);
		CBattleScene::getInstance()->mapSprite->addChild(role);

	}

	return true;
}


//空的地图判断函数，在地图类初始化时，地图指针指向该函数；
short CMapTools::MapBoundaryNull(Vec3* const p_vec3, const int p_nDirection)
{
	return 0;
}


//地图边界判定函数
//判断书写方法：
//开直角写虚，闭直角写过
short CMapTools::MapBoundaryJudge_Park0(Vec3* const p_vec3,const int p_nDirection)
{
	float fX = 0, fY = 0, fZ = 0;
	fX = p_vec3->x;
	fY = p_vec3->y;
	fZ = p_vec3->z;
	short nLimit = 0;

	if (fY <= 10)
	{
		p_vec3->y = 10;
		fY = p_vec3->y;
		nLimit |= CAN_NOT_DOWN;
	}

	if (fX <= 60)
	{
		p_vec3->x = 60;
		nLimit |= CAN_NOT_LEFT;
	}

	if (fY >= 200)
	{
		p_vec3->y = 200;
		nLimit |= CAN_NOT_UP;
	}

	if (fX >= 20 && fX <= 80 && fY >= 80)
	{
		p_vec3->y = 80;
		nLimit |= MAP_CHANGE_IN | CAN_NOT_UP;
	}
	else if (fX >= 80 && fX < 115 && fY >= 80)
	{
		p_vec3->y = 80;
		nLimit |= CAN_NOT_UP;
	}
	else if (fX >= 115 && fX <= 255 && fY >= 1.2 * fX - 64) //Y=1.2x -64
	{
		if (p_nDirection == ROLE_DIRECTION_4_DOWN || p_nDirection == ROLE_DIRECTION_4_UP)
		{
			p_vec3->y = 1.2 * fX - 64.0;
		}
		else if (p_nDirection == ROLE_DIRECTION_4_LEFT || p_nDirection == ROLE_DIRECTION_4_RIGHT)
		{
			p_vec3->x = (fY + 64.0) / 1.2;
		}
		nLimit |= CAN_NOT_LEFT | CAN_NOT_UP;
	}
	else if (fX >= 975 && fX <= 1605 && fY >= -2.2222 * fX + 2399.978 && fZ < 1.f && fZ > -1.f) //Y=-2.2222x + 2399.978
	{
			nLimit |= (MAP_JUDGE_DROWNING |CAN_NOT_RIGHT | CAN_NOT_UP);
	}

	return nLimit;
}
short CMapTools::MapBoundaryJudge_Park1(Vec3* const p_vec3, const int p_nDirection)
{
	float fX = 0, fY = 0, fZ = 0;
	fX = p_vec3->x;
	fY = p_vec3->y;
	fZ = p_vec3->z;
	short nLimit = 0;

	//分段
	//1.fX <= 70
	//2.70<fX<=140 
	//3.
	//4.
	//5.
	
	// -∞ < x < -∞
	if (fY <= 10)
	{
		nLimit = nLimit | CAN_NOT_DOWN;
	}

	//第一段
	if (fX <= 70)
	{
		if (fX <= 10)
		{
			p_vec3->x = 10;
			nLimit = nLimit | CAN_NOT_LEFT;
		}

		if (fX <= 70 && fY >= 200)
		{
			p_vec3->y = 200;
			nLimit = nLimit | CAN_NOT_UP;
		}

		return nLimit;
	}
	//第二段
	if (fX > 70 && fX <= 120)
	{
		if (fX <= 120 && fY >= 200)
		{
			p_vec3->y = 200;
			nLimit = nLimit | CAN_NOT_UP | MAP_CHANGE_OUT;
		}

		if (fX >= 120 && fY > 140)
		{
			p_vec3->x = 120;
			nLimit = nLimit | CAN_NOT_RIGHT;
		}
		return nLimit;
	}

	//第三段
	if (fX > 120 && fX <= 1840 && fY >= 140)
	{
		p_vec3->y = 140;
		nLimit = nLimit | CAN_NOT_UP | CAN_NOT_RIGHT | CAN_NOT_LEFT;
	}

	//第四段
	if (fX >= 1840 && fX < 2170)
	{
		if (fX >= 1820 && fX <= 1860 && fY > 140)
		{
			p_vec3->x = 1860;
			nLimit = nLimit | CAN_NOT_LEFT;
		}
		if (fX >= 1840 && fX < 2170 && fY >= 170)
		{
			p_vec3->y = 170;
			nLimit = nLimit | CAN_NOT_UP;
		}
		return nLimit;
	}
	//第五段
	if (fX >= 2170 && fX < 2190 && fY >= 0.75 * fX - 1457.5)
	{
		if (p_nDirection == ROLE_DIRECTION_4_DOWN || p_nDirection == ROLE_DIRECTION_4_UP)
		{
			p_vec3->y = 0.75 * fX - 1457.5;
		}
		else if (p_nDirection == ROLE_DIRECTION_4_LEFT || p_nDirection == ROLE_DIRECTION_4_RIGHT)
		{
			p_vec3->x = (fY + 1457.5) / 0.75;
		}
		nLimit = nLimit | CAN_NOT_UP | CAN_NOT_LEFT;
	}
	if (fX >= 2190 && fY >= 200)
	{
		p_vec3->y = 200;
		nLimit = nLimit | CAN_NOT_UP;
	}
	if (fX >= 2370)
	{
		p_vec3->x = 2370;
		nLimit = nLimit | CAN_NOT_RIGHT;
	}
	return nLimit;
}