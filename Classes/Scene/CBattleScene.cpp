#include "CBattleScene.h"

#define BROTHER 0x00
#define NPC_BEHAVIOR 1
#define NPC1_CONTROL 0

//返回战斗场景的单例
CBattleScene* CBattleScene::getInstance()
{
	static CBattleScene* battleScene = createScene();
	if (battleScene == nullptr)
	{
		battleScene = createScene();
		if (battleScene == nullptr)
		{
			CCLOG("CBattleScene create false!");
			return nullptr;
		}
	}
	return battleScene;
}


//场景创建 ---- Scene create
CBattleScene* CBattleScene::createScene()
{
	return CBattleScene::create();
}


//场景初始化 ---- scene creat
bool CBattleScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	//获取当前屏幕大小数据
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	//初始化
	this->sceneSetInit();//战斗场景设置初始化
	this->spiteInit();//战斗场景内精灵设置初始化
	this->resumControl();//开始游戏

	return true;

}


//为角色创建遮挡布
void CBattleScene::generateCover(CRole* p_Role)
{
	if (p_Role == nullptr)
	{
		return;
	}
	Sprite* mapSprite = dynamic_cast<Sprite*>(p_Role->getParent());
	Texture2D* mapTexture = mapSprite->getTexture();
	const Size mapTextureSize = mapTexture->getContentSize();
	const float coverPosionX = p_Role->getPositionX() - 150;
	const float coverPosionY = mapTextureSize.height - p_Role->getRolePosionY();
	Sprite* cover = Sprite::createWithTexture(mapTexture, Rect(coverPosionX, coverPosionY, 300, (p_Role->getPositionY() - p_Role->getRolePosionZ())> 200 
		? p_Role->getRolePosionY() : 200 ));
	if (cover == nullptr)
	{
		CCLOG("Cover create false!");
		return;
	}
	cover->setPosition(coverPosionX,p_Role->getRolePosionY());
	cover->setAnchorPoint(Vec2(0,1));
	mapSprite->addChild(cover);
	p_Role->setZOrder(0);
	cover->setZOrder(1);
	cover->scheduleOnce([cover=cover](float s) {
		cover->getParent()->removeChild(cover);
		},3.1f,"ROLE_DROWNING_COVER_RELEASE");
}


//主角死亡，游戏结束
void CBattleScene::gameOver()
{
	Label* gameOverLabel = Label::createWithSystemFont("GAME OVER","Arial",32.f);
	if (gameOverLabel == nullptr)
	{
		CCLOG("gameOverLabel creat false!");
		return;
	}
	gameOverLabel->setPosition(visibleSize.width/2, visibleSize.height/2);
	gameOverLabel->setAnchorPoint(Vec2(0.5f,0.5f));
	gameOverLabel->setName("gameOverLabel");
	this->addChild(gameOverLabel);
	this->unscheduleAllSelectors();

	this->scheduleOnce([](float s) {
		CBattleScene::getInstance()->removeChildByName("gameOverLabel");
		CBattleScene::getInstance()->resetGame();
		}, 1.5f, "GAME_RESET");
}


//重置战斗场景
void CBattleScene::resetGame()
{
	//CRole::sonmfunction();  //重新生成主角的小弟

	CRole::vecRoleResetAttribute_LeadCamp();//重置主角团角色状态数据

	this->changeMap(MAP_CHANGE_IN | CMapTools::getInstance().getMapIformation().nMapNumber);
}


//切换地图
void CBattleScene::changeMap(const int p_nMapType)
{

	CCLOG("change map");
	CRole::vecRoleStopAllAtion();//停止所有角色动画
	CRole::vecRoleStopAllScheduler();//停止所有角色调度器
	this->stopControl();//停止战斗场景的动画和角色控制
	CEffects::getInstance()->runCloseCurtain(this);//运行关黑幕的动画

	//关闭黑幕后的单次调度器
	this->scheduleOnce([p_nMapType = p_nMapType](float s) {

		CRole::role_ChangeMap();//设置角色类,请离敌对角色
		CBattleScene::getInstance()->mapSprite->removeAllChildren();//清理地图中节点
		CBattleScene::getInstance()->removeChild(CBattleScene::getInstance()->mapSprite);//移除地图精灵
		CBattleScene::getInstance()->mapSprite = CMapTools::getInstance().mapLoding(CBattleScene::getInstance()->getChildByName("BattleLayer"), p_nMapType);//加载新地图
		
		//接下来创建、设置地图里的角色
		CMapTools::getInstance().roleRoding(p_nMapType);//设置主角团角色数据
		
		//运行开窗帘特效
		CEffects::getInstance()->runOpenCurtain(CBattleScene::getInstance());

		//在完全打开黑幕后恢复控制
		CBattleScene::getInstance()->scheduleOnce([](float s) {
			
			//恢复战斗场景绘制和恢复角色控制
			CBattleScene::getInstance()->resumControl();
			
			//加载坏角色
			CMapTools::getInstance().roleRodingBegin_Badman();

			}, 0.8f, "GAME_START");
		},0.8f,"CHANGE_MAP");
}


//按键按下回调函数   ----  callback funtion for press key 
void CBattleScene::onKeyPressed(EventKeyboard::KeyCode keycode, Event* event)
{
	switch (keycode)
	{
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:     //按下 上
		{
			if (MoveKeyState[KEY_UP_DOWN] == 0 || MoveKeyState[KEY_UP_DOWN] == -1 )
				MoveKeyState[KEY_UP_DOWN] = 1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:   //按下 下
		{
			if (MoveKeyState[KEY_UP_DOWN] == 0 || MoveKeyState[KEY_UP_DOWN] == 1)
				MoveKeyState[KEY_UP_DOWN] = -1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:   //按下 左
		{
			if (MoveKeyState[KEY_LEFT_RIGHT] != -1 && MoveKeyState[KEY_LEFT_RIGHT] != -2)
				MoveKeyState[KEY_LEFT_RIGHT] = -1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:  //按下 右
		{
			if (MoveKeyState[KEY_LEFT_RIGHT] != 1 && MoveKeyState[KEY_LEFT_RIGHT] != 2)
				MoveKeyState[KEY_LEFT_RIGHT] = 1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_C:   //按下 C  跳跃
		{
			if(lead->roleStateJudge(ROLE_JUMP))
			{ 
				lead->runRoleAction(ROLE_JUMP);
			}
			if(MoveKeyState[KEY_LEFT_RIGHT] != 0)
			{ 
				MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			if (MoveKeyState[KEY_UP_DOWN] != 0)
			{
				MoveKeyState[KEY_UP_DOWN] = 0;
			}
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_X:   //按下 X  拳头攻击
		{
			if (lead->roleStateJudge(ROLE_UPPERCUT))
			{
				lead->runRoleAction(ROLE_UPPERCUT);
				this->MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			else if(lead->roleStateJudge(ROLE_FIST_ATTACK))
			{ 
				lead->runRoleAction(ROLE_FIST_ATTACK);
			}
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_Z:   //按下 Z  踢腿攻击
		{
			if(lead->roleStateJudge(ROLE_SLIDING_KICK))
			{
				lead->runRoleAction(ROLE_SLIDING_KICK);
				this->MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			else if (lead->roleStateJudge(ROLE_FLYING_KICK))
			{
				lead->runRoleAction(ROLE_FLYING_KICK);
				this->MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			else if (lead->roleStateJudge(ROLE_LEG_ATTACK))
			{ 
				lead->runRoleAction(ROLE_LEG_ATTACK);
			}
			break;
		}



		//控制NPC1
		#if NPC1_CONTROL
		case cocos2d::EventKeyboard::KeyCode::KEY_I:     //按下 上
		{
			if (NPC_MoveKeyState[KEY_UP_DOWN] == 0 || NPC_MoveKeyState[KEY_UP_DOWN] == -1)
				NPC_MoveKeyState[KEY_UP_DOWN] = 1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_K:   //按下 下
		{
			if (NPC_MoveKeyState[KEY_UP_DOWN] == 0 || NPC_MoveKeyState[KEY_UP_DOWN] == 1)
				NPC_MoveKeyState[KEY_UP_DOWN] = -1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_J:   //按下 左
		{
			if (NPC_MoveKeyState[KEY_LEFT_RIGHT] != -1 && NPC_MoveKeyState[KEY_LEFT_RIGHT] != -2)
				NPC_MoveKeyState[KEY_LEFT_RIGHT] = -1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_L:  //按下 右
		{
			if (NPC_MoveKeyState[KEY_LEFT_RIGHT] != 1 && NPC_MoveKeyState[KEY_LEFT_RIGHT] != 2)
				NPC_MoveKeyState[KEY_LEFT_RIGHT] = 1;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_O:   //按下 C  跳跃
		{
			if (npc1->roleStateJudge(ROLE_JUMP))
			{
				npc1->runRoleAction(ROLE_JUMP);
			}
			if (NPC_MoveKeyState[KEY_LEFT_RIGHT] != 0)
			{
				NPC_MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			if (NPC_MoveKeyState[KEY_UP_DOWN] != 0)
			{
				NPC_MoveKeyState[KEY_UP_DOWN] = 0;
			}
			break;
		}
		#endif
	}
}


//按键松开回调函数   ----  callback funtion for releas key 
void CBattleScene::onKeyReleased(EventKeyboard::KeyCode keycode, Event* event)
{
	switch (keycode)
	{
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:    //松开 上
		{
			if (MoveKeyState[KEY_UP_DOWN] == 1)
				MoveKeyState[KEY_UP_DOWN] = 0;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:  //松开 下
		{
			if (MoveKeyState[KEY_UP_DOWN] == -1)
				MoveKeyState[KEY_UP_DOWN] = 0;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW: //松开 左
		{
			this->_structDoubleClickFlg.lCurrentTime = MyTool::GetMilliSecond();
			const long time_difference = this->_structDoubleClickFlg.lCurrentTime - this->_structDoubleClickFlg.lLastTime;
			if (time_difference < 200 && time_difference > 50 
				&& this->_structDoubleClickFlg.EkbKetCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
			{
				MoveKeyState[KEY_LEFT_RIGHT] = -2;
			}
			else if (MoveKeyState[KEY_LEFT_RIGHT] == -1)
			{ 
				MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			this->_structDoubleClickFlg.EkbKetCode = keycode;
			this->_structDoubleClickFlg.lLastTime = this->_structDoubleClickFlg.lCurrentTime;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW: //松开 右
		{
			this->_structDoubleClickFlg.lCurrentTime = MyTool::GetMilliSecond();
			const long time_difference = this->_structDoubleClickFlg.lCurrentTime - this->_structDoubleClickFlg.lLastTime;
			if (time_difference < 200 && time_difference > 50
				&& this->_structDoubleClickFlg.EkbKetCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
			{
				MoveKeyState[KEY_LEFT_RIGHT] = 2;
			}
			else if (MoveKeyState[KEY_LEFT_RIGHT] == 1)
			{ 
				MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			this->_structDoubleClickFlg.EkbKetCode = keycode;
			this->_structDoubleClickFlg.lLastTime = this->_structDoubleClickFlg.lCurrentTime;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_Z:   //松开 Z
		{
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_X:   //松开 X
		{
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_C:   //松开 C
		{
			break;
		}

#if NPC1_CONTROL
		case cocos2d::EventKeyboard::KeyCode::KEY_I:    //松开 上
		{
			if (NPC_MoveKeyState[KEY_UP_DOWN] == 1)
				NPC_MoveKeyState[KEY_UP_DOWN] = 0;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_K:  //松开 下
		{
			if (NPC_MoveKeyState[KEY_UP_DOWN] == -1)
				NPC_MoveKeyState[KEY_UP_DOWN] = 0;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_J: //松开 左
		{
			this->_structDoubleClickFlg_NPC.lCurrentTime = MyTool::GetMilliSecond();
			const long time_difference = this->_structDoubleClickFlg_NPC.lCurrentTime - this->_structDoubleClickFlg_NPC.lLastTime;
			if (time_difference < 200 && time_difference > 50
				&& this->_structDoubleClickFlg_NPC.EkbKetCode == EventKeyboard::KeyCode::KEY_J)
			{
				NPC_MoveKeyState[KEY_LEFT_RIGHT] = -2;
			}
			else if (NPC_MoveKeyState[KEY_LEFT_RIGHT] == -1)
			{
				NPC_MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			this->_structDoubleClickFlg_NPC.EkbKetCode = keycode;
			this->_structDoubleClickFlg_NPC.lLastTime = this->_structDoubleClickFlg_NPC.lCurrentTime;
			break;
		}
		case cocos2d::EventKeyboard::KeyCode::KEY_L: //松开 右
		{
			this->_structDoubleClickFlg_NPC.lCurrentTime = MyTool::GetMilliSecond();
			const long time_difference = this->_structDoubleClickFlg_NPC.lCurrentTime - this->_structDoubleClickFlg_NPC.lLastTime;
			if (time_difference < 200 && time_difference > 50
				&& this->_structDoubleClickFlg_NPC.EkbKetCode == EventKeyboard::KeyCode::KEY_L)
			{
				NPC_MoveKeyState[KEY_LEFT_RIGHT] = 2;
			}
			else if (NPC_MoveKeyState[KEY_LEFT_RIGHT] == 1)
			{
				NPC_MoveKeyState[KEY_LEFT_RIGHT] = 0;
			}
			this->_structDoubleClickFlg_NPC.EkbKetCode = keycode;
			this->_structDoubleClickFlg_NPC.lLastTime = this->_structDoubleClickFlg_NPC.lCurrentTime;
			break;
		}
#endif
	}
}


//触摸屏幕事件
bool CBattleScene::onTouchBegan(Touch* touch, Event* unused_event)
{
	Vec3 xyz = lead->getRoleVec3();
	CCLOG("LEAD : X = %3f , Y = %3f , Z = %3f ",xyz.x,xyz.y,xyz.z);
	CCLOG("touch.x = %f,touch.y = %f", touch->getLocation().x, touch->getLocation().y);
	//xyz = CRole::vecRoles[0]->at(1)->getRoleVec3();
	//CCLOG("ROLE1 : X = %3f , Y = %3f , Z = %3f ", xyz.x, xyz.y, xyz.z);
	return true;
}
void CBattleScene::onTouchMoved(Touch* touch, Event* unused_event)
{
}
void CBattleScene::onTouchEnded(Touch* touch, Event* unused_event)
{
}


//战斗场景设置初始化函数
bool CBattleScene::sceneSetInit()
{
	//初始化监听按键数组内容
	for (int i = 0; i < MAX_KEY_MONITOR_AMOUNT; i++)
	{
		MoveKeyState[i] = 0;
	}
	//初始化双击标志
	this->_structDoubleClickFlg.bKeyDoublePressFlg = false;
	this->_structDoubleClickFlg.lLastTime = MyTool::GetMilliSecond();

	return true;
}

//菜单、地图和角色初始化函数
bool CBattleScene::spiteInit()
{
	//初始化精灵衍生类
	CRole::roleClassInit();	//初始化角色类
	CEffects::getInstance(); //初始化特效类


	//读取角色清单文件
	Readjson role_plist_readjson;
	role_plist_readjson.JsonClassInit(FileUtils::getInstance()->getStringFromFile("NPC/rolelist.json"));

	//创建图层
	auto field = Layer::create();
	if (field == nullptr)
	{
		CCLOG("field create false!");
		return false;
	}
	field->setName("BattleLayer");
	field->setAnchorPoint(Vec2(0, 0));
	field->setPosition(0,0);
	addChild(field);

	//创建上菜单
	upMenu = CUpMenu::getInstance();
	if (upMenu == nullptr)
	{
		#if DEBUG_MODE
			CCLOG("upMenu create false!");
		#endif
			return false;
	}
	upMenu->setZOrder(6);
	upMenu->setAnchorPoint(Vec2(0, 1));
	upMenu->setPosition(0, Director::getInstance()->getVisibleSize().height);
	field->addChild(upMenu);

	//创建下菜单
	_downMuen = Sprite::create("photo/downMuen.png");
	if (_downMuen == nullptr)
	{
#if DEBUG_MODE
		CCLOG("downMuen create false!");
#endif
		return false;
	}
	_downMuen->setZOrder(5);
	_downMuen->setAnchorPoint(Vec2(0, 0));
	_downMuen->setPosition(0, 0);

	_downMuen->setScale(10,10);
	field->addChild(_downMuen);
	
	//创建背景
	mapSprite = CMapTools::getInstance().mapLoding(field, 0);
	Size mapSize = mapSprite->getContentSize();


	//创建主角
	CRole* role1 = CRole::roleCreate();
	if (role1 == nullptr)
	{
		return false;
	}
	role1->roleSet(role_plist_readjson.json_object["kowloon group"][1].asString(),0x00, 300, mapSize.height / 2 - 200);
	mapSprite->addChild(role1);
	role1->setZOrder(10000- role1->getPositionY());

	//创建小弟
	CRole* role2 = CRole::roleCreate();
	if (role2 == nullptr)
	{
		return false;
	}
	role2->roleSet(role_plist_readjson.json_object["kowloon group"][2].asString(), BROTHER, 400, mapSize.height / 2 - 200);
	mapSprite->addChild(role2);
	role2->setZOrder(10000 - role2->getPositionY());
	//创建小弟2
	CRole* role3 = CRole::roleCreate();
	if (role3 == nullptr)
	{
		return false;
	}
	role3->roleSet(role_plist_readjson.json_object["kowloon group"][3].asString(), BROTHER, 500, mapSize.height / 2 - 200);
	mapSprite->addChild(role3);
	role3->setZOrder(10000 - role3->getPositionY());

	return true;
}


//战斗场景清理函数
bool CBattleScene::battleSceneClear()
{
	return true;
}


//调度器A回调函数，用于判断按键状态，更新时间0.1s ---- 
//schedule of callback funtion, Used to judge key state,schedule Time parameter 0.1s
void CBattleScene::_OnCBattleSceneUpdateA(float s)
{
	//主角控制
	if(CRole::vecRoles[0]->size()>0)
	{
		if (MoveKeyState[KEY_UP_DOWN] == 1)
		{
			if (lead->roleStateJudge(ROLE_MOVE))
				lead->runRoleAction(ROLE_MOVE_UP);
		}
		else if (MoveKeyState[KEY_UP_DOWN] == -1)
		{
			if (lead->roleStateJudge(ROLE_MOVE))
				lead->runRoleAction(ROLE_MOVE_DOWN);
		}
		if (MoveKeyState[KEY_LEFT_RIGHT] == -1)
		{
			if (lead->roleStateJudge(ROLE_MOVE))
				lead->runRoleAction(ROLE_MOVE_LEFT);
		}
		else if (MoveKeyState[KEY_LEFT_RIGHT] == 1)
		{
			if (lead->roleStateJudge(ROLE_MOVE))
				lead->runRoleAction(ROLE_MOVE_RIGHT);
		}
		else if (MoveKeyState[KEY_LEFT_RIGHT] == -2)
		{
			if (lead->roleStateJudge(ROLE_RUN))
				lead->runRoleAction(ROLE_RUN_LEFT);
		}
		else if (MoveKeyState[KEY_LEFT_RIGHT] == 2)
		{
			if (lead->roleStateJudge(ROLE_RUN))
				lead->runRoleAction(ROLE_RUN_RIGHT);
		}

		if (!lead->roleStateJudge(ROLE_RUN)
			&& (MoveKeyState[KEY_LEFT_RIGHT] == 2
				|| MoveKeyState[KEY_LEFT_RIGHT] == -2))
		{
			MoveKeyState[KEY_LEFT_RIGHT] = 0;
		}
	}


//NPC1控制
#if NPC1_CONTROL
	if(CRole::vecRoles[0]->size()>0)
	{ 
		if (NPC_MoveKeyState[KEY_UP_DOWN] == 1)
		{
			if (npc1->roleStateJudge(ROLE_MOVE))
				npc1->runRoleAction(ROLE_MOVE_UP);
		}
		else if (NPC_MoveKeyState[KEY_UP_DOWN] == -1)
		{
			if (npc1->roleStateJudge(ROLE_MOVE))
				npc1->runRoleAction(ROLE_MOVE_DOWN);
		}
		if (NPC_MoveKeyState[KEY_LEFT_RIGHT] == -1)
		{
			if (npc1->roleStateJudge(ROLE_MOVE))
				npc1->runRoleAction(ROLE_MOVE_LEFT);
		}
		else if (NPC_MoveKeyState[KEY_LEFT_RIGHT] == 1)
		{
			if (npc1->roleStateJudge(ROLE_MOVE))
				npc1->runRoleAction(ROLE_MOVE_RIGHT);
		}
		else if (NPC_MoveKeyState[KEY_LEFT_RIGHT] == -2)
		{
			if (npc1->roleStateJudge(ROLE_RUN))
				npc1->runRoleAction(ROLE_RUN_LEFT);
		}
		else if (NPC_MoveKeyState[KEY_LEFT_RIGHT] == 2)
		{
			if (npc1->roleStateJudge(ROLE_RUN))
				npc1->runRoleAction(ROLE_RUN_RIGHT);
		}

		if (!npc1->roleStateJudge(ROLE_RUN)
			&& (NPC_MoveKeyState[KEY_LEFT_RIGHT] == 2
				|| NPC_MoveKeyState[KEY_LEFT_RIGHT] == -2))
		{
			NPC_MoveKeyState[KEY_LEFT_RIGHT] = 0;
		}

		if (!npc1->roleStateJudge(ROLE_RUN)
			&& (NPC_MoveKeyState[KEY_LEFT_RIGHT] == 2
				|| NPC_MoveKeyState[KEY_LEFT_RIGHT] == -2))
		{
			NPC_MoveKeyState[KEY_LEFT_RIGHT] = 0;
		}

	}
#endif


	//角色思考
#if NPC_BEHAVIOR == 1

	for (int i = 1; i < CRole::vecRoles[0]->size(); i++)
	{
		CRole::vecRoles[0]->at(i)->npcBehavior();
	}

	for(int j = 1 ; j < 4 ; j++)
	{ 
		for (int i = 0; i < CRole::vecRoles[j]->size(); i++)
			CRole::vecRoles[j]->at(i)->npcBehavior();
	}

#endif

}


//调度器B回调函数，用于向地图申请加入角色，更新时间1s
void CBattleScene::_OnCBattleSceneUpdataB(float s)
{



}


//每帧运行调度器
void CBattleScene::update(float s)
{
	//状态恢复
	CRole::vecRolesRecovery();
	//拉动视角
	CMapTools::getInstance().lensStretch();
}


//控制使能
void CBattleScene::resumControl()
{
	//重置按键状态
	MoveKeyState[KEY_UP_DOWN] = 0;
	MoveKeyState[KEY_LEFT_RIGHT] = 0;

	//按键监听
	listenerkeyPad = EventListenerKeyboard::create();
	listenerkeyPad->onKeyPressed = CC_CALLBACK_2(CBattleScene::onKeyPressed, this);
	listenerkeyPad->onKeyReleased = CC_CALLBACK_2(CBattleScene::onKeyReleased, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerkeyPad, this);

	//鼠标监听
	listenerMouse = EventListenerTouchOneByOne::create();
	listenerMouse->onTouchBegan = CC_CALLBACK_2(CBattleScene::onTouchBegan, this);
	listenerMouse->onTouchMoved = CC_CALLBACK_2(CBattleScene::onTouchMoved, this);
	listenerMouse->onTouchEnded = CC_CALLBACK_2(CBattleScene::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerMouse, this);

	//激活调度器
	this->schedule(schedule_selector(CBattleScene::_OnCBattleSceneUpdateA), 0.05f);
	this->scheduleUpdate();
}


//停止控制，使所有角色失去思考和活性
void CBattleScene::stopControl()
{
	Director::getInstance()->getEventDispatcher()->removeEventListener(listenerkeyPad);
	Director::getInstance()->getEventDispatcher()->removeEventListener(listenerMouse);
	this->unschedule(schedule_selector(CBattleScene::_OnCBattleSceneUpdateA));
	this->unscheduleUpdate();
}

