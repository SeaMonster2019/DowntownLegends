#include "CMenuScene.h"

cocos2d::Scene* CMenuScene::createScene()
{
	return CMenuScene::create();
}

bool CMenuScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	//场景结束标志置否
	bEndFlag = false;

	//获取当前屏幕大小数据
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	//生成背景
	auto MenuBackgrounde = Sprite::create("photo/menu0.png");
	if (MenuBackgrounde == nullptr)
	{
		printf("\"photo/menu0.png\" Loading failed");
		exit(0);
	}
	MenuBackgrounde->setTextureRect(Rect(0, 0, visibleSize.width, visibleSize.height));
	MenuBackgrounde->setAnchorPoint(Vec2(0.5,0.5));
	MenuBackgrounde->setPosition(visibleSize.width/2, visibleSize.height/2);
	addChild(MenuBackgrounde);

	//读取标签文本
	if (!this->readJson.JsonClassInit(FileUtils::getInstance()->getStringFromFile("text/menu.json")))
	{
		printf("open json file \"text/menu.json\" fail");
		exit(0);
	}

	//生成标题
	auto TitleLabel = Label::createWithTTF(readJson.json_object["title"].asString(), "fonts/STXINGKA.ttf",216);
	if (TitleLabel == nullptr)
	{
		printf("\"fonts/boyang.fft\" Loading failed");
		exit(0);
	}
	TitleLabel->setAnchorPoint(Vec2(0.5,0.5)); 
	TitleLabel->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2 + 200));  
	TitleLabel->setTextColor(Color4B(250, 50, 50, 255));
	TitleLabel->enableBold();
	addChild(TitleLabel);

	//生成副标题
	auto SubtitleLabel = Label::createWithTTF(readJson.json_object["subtitle"].asString(), "fonts/STXINGKA.ttf", 72);
	if (SubtitleLabel == nullptr)
	{
		printf("\"fonts/boyang.fft\" Loading failed");
		exit(0);
	}
	SubtitleLabel->setAnchorPoint(Vec2(0.5, 0.5));
	SubtitleLabel->setPosition(Vec2(visibleSize.width / 2 + 300, visibleSize.height / 2 + 50 ));
	SubtitleLabel->setTextColor(Color4B(250, 50, 50, 255));
	SubtitleLabel->enableBold();
	addChild(SubtitleLabel);

	//生成主角和两个小弟以及他们的动画
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("NPC/NPC0_1.plist");
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("NPC/NPC0_2.plist");
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("NPC/NPC0_3.plist");

	Vector<SpriteFrame*> vecSpriteFrame;
	char FrameName[25] = "";
	const int nAtions[9]= {116,117,115,76,77,75 ,96,97,75};
	for (int i = 0; i < 3; i++)
	{
		sprintf(FrameName,"NPC0_%d (%d).png",i+1,nAtions[i * 3 + 0]);
		SpriteFrame* spriteFrame1 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(FrameName);
		vecSpriteFrame.pushBack(spriteFrame1);
		sprintf(FrameName, "NPC0_%d (%d).png", i+1, nAtions[i * 3 + 1]);
		SpriteFrame* spriteFrame2 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(FrameName);
		vecSpriteFrame.pushBack(spriteFrame2);
		vecAnimation.pushBack(Animation::createWithSpriteFrames(vecSpriteFrame, 0.2f));
		vecSpriteFrame.clear();

		sprintf(FrameName, "NPC0_%d (%d).png", i+1, nAtions[i * 3 + 0]);
		SpriteFrame* spriteFrame3 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(FrameName);
		vecSpriteFrame.pushBack(spriteFrame3);
		sprintf(FrameName, "NPC0_%d (%d).png", i+1, nAtions[i * 3 + 1]);
		SpriteFrame* spriteFrame4 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(FrameName);
		vecSpriteFrame.pushBack(spriteFrame4);
		sprintf(FrameName, "NPC0_%d (%d).png", i+1, nAtions[i * 3 + 2]);
		SpriteFrame* spriteFrame5 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(FrameName);
		vecSpriteFrame.pushBack(spriteFrame5);
		vecAnimation.pushBack(Animation::createWithSpriteFrames(vecSpriteFrame, 0.05f));
		vecSpriteFrame.clear();
	}
	
	//制作主角摔倒的动画
	SpriteFrame* spriteFrame1 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("NPC0_1 (34).png");
	vecSpriteFrame.pushBack(spriteFrame1);
	SpriteFrame* spriteFrame2 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("NPC0_1 (43).png");
	vecSpriteFrame.pushBack(spriteFrame2);
	SpriteFrame* spriteFrame3 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("NPC0_1 (43).png");
	vecSpriteFrame.pushBack(spriteFrame3);
	vecAnimation.pushBack(Animation::createWithSpriteFrames(vecSpriteFrame, 0.8f));
	vecSpriteFrame.clear();

	//制作小弟B回头的动画
	SpriteFrame* spriteFrame4 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("NPC0_3 (98).png");
	vecSpriteFrame.pushBack(spriteFrame4);
	SpriteFrame* spriteFrame5 = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("NPC0_3 (99).png");
	vecSpriteFrame.pushBack(spriteFrame5);
	vecAnimation.pushBack(Animation::createWithSpriteFrames(vecSpriteFrame, 0.8f));
	vecSpriteFrame.clear();


	//生成角色精灵状态
	vecNPCSprite.pushBack(Sprite::createWithSpriteFrameName("NPC0_1 (7).png"));
	vecNPCSprite.pushBack(Sprite::createWithSpriteFrameName("NPC0_2 (7).png"));
	vecNPCSprite.pushBack(Sprite::createWithSpriteFrameName("NPC0_3 (7).png"));

	vecNPCSprite.at(0)->setAnchorPoint(Vec2(0.5,0));
	vecNPCSprite.at(0)->setPosition(Vec2(-200,visibleSize.height/2 - 300));
	vecNPCSprite.at(0)->setScale(3.5f, 3.5f);
	addChild(vecNPCSprite.at(0));
	vecNPCSprite.at(1)->setAnchorPoint(Vec2(0.5, 0));
	vecNPCSprite.at(1)->setPosition(Vec2(-400, visibleSize.height / 2 - 300));
	vecNPCSprite.at(1)->setScale(3.5f, 3.5f);
	addChild(vecNPCSprite.at(1));
	vecNPCSprite.at(2)->setAnchorPoint(Vec2(0.5, 0));
	vecNPCSprite.at(2)->setPosition(Vec2(-470, visibleSize.height / 2 - 300));
	vecNPCSprite.at(2)->setScale(3.5f, 3.5f);
	addChild(vecNPCSprite.at(2));

	//制作菜单
	auto MenuLabel_State = Label::createWithTTF(readJson.json_object["start"].asString(), "fonts/miao.ttf", 36);
	if (MenuLabel_State == nullptr)
	{
		printf("\"fonts/miao.fft\" Loading failed");
		exit(0);
	}
	auto Menu_State = MenuItemLabel::create(MenuLabel_State, CC_CALLBACK_1(CMenuScene::OnStart, this));

	auto MenuLabel_Load = Label::createWithTTF(readJson.json_object["load"].asString(), "fonts/miao.ttf", 36);
	if (MenuLabel_Load == nullptr)
	{
		printf("\"fonts/miao.fft\" Loading failed");
		exit(0);
	}
	auto Menu_Load = MenuItemLabel::create(MenuLabel_Load, CC_CALLBACK_1(CMenuScene::OnLoad, this));

	auto MenuLabel_Set = Label::createWithTTF(readJson.json_object["seting"].asString(), "fonts/miao.ttf", 36);
	if (MenuLabel_Set == nullptr)
	{
		printf("\"fonts/miao.fft\" Loading failed");
		exit(0);
	}
	auto Menu_Set = MenuItemLabel::create(MenuLabel_Set, CC_CALLBACK_1(CMenuScene::OnSet, this));

	auto MenuLabel_End = Label::createWithTTF(readJson.json_object["end"].asString(), "fonts/miao.ttf", 36);
	if (MenuLabel_End == nullptr)
	{
		printf("\"fonts/miao.fft\" Loading failed");
		exit(0);
	}
	auto Menu_End = MenuItemLabel::create(MenuLabel_End, CC_CALLBACK_1(CMenuScene::OnEnd, this));

	GameMenu = Menu::create(Menu_State, Menu_Load, Menu_Set, Menu_End, NULL);
	GameMenu->alignItemsVertically();
	GameMenu->setAnchorPoint(Vec2(0.5, 0.5));
	GameMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 200));
	GameMenu->setColor(Color3B(150, 250, 150));
	this->addChild(GameMenu);
	GameMenu->setEnabled(false);

	//生成幕帘
	CurtainSpriteLeft = Sprite::create("photo/curtain.png");
	if (CurtainSpriteLeft == nullptr)
	{
		printf("\"photo/curtain.png\" Loading failed");
		exit(0);
	}
	CurtainSpriteLeft->setTextureRect(Rect(0, 0, visibleSize.width/2, visibleSize.height));
	CurtainSpriteLeft->setAnchorPoint(Vec2(0, 0));
	CurtainSpriteLeft->setPosition(0,0);
	addChild(CurtainSpriteLeft);

	CurtainSpriteRight = Sprite::create("photo/curtain.png");
	if (CurtainSpriteRight == nullptr)
	{
		printf("\"photo/curtain.png\" Loading failed");
		exit(0);
	}
	CurtainSpriteRight->setTextureRect(Rect(0, 0, visibleSize.width/2, visibleSize.height));
	CurtainSpriteRight->setAnchorPoint(Vec2(0, 0));
	CurtainSpriteRight->setPosition(visibleSize.width / 2, 0);
	addChild(CurtainSpriteRight);

	//启动特效
	this->scheduleOnce([&](float s)->void {

		CurtainSpriteLeft->runAction(MoveBy::create(0.8f,Vec2(-visibleSize.width/2,0)));
		CurtainSpriteRight->runAction(MoveBy::create(0.8f, Vec2(visibleSize.width/2,0)));

		Animate* animate[3];
		animate[0] = Animate::create(vecAnimation.at(2 * 0 + 0));
		animate[1] = Animate::create(vecAnimation.at(2 * 1 + 0));
		animate[2] = Animate::create(vecAnimation.at(2 * 2 + 0));
		RepeatForever *Fation[3];
		Fation[0] = RepeatForever::create(animate[0]);
		Fation[1] = RepeatForever::create(animate[1]);
		Fation[2] = RepeatForever::create(animate[2]);

		vecNPCSprite.at(0)->runAction(MoveBy::create(5.f, Vec2(visibleSize.width / 4 + 200, 0)));
		vecNPCSprite.at(0)->runAction(Fation[0]);
		vecNPCSprite.at(1)->runAction(MoveBy::create(5.f, Vec2(visibleSize.width / 4 + 200, 0)));
		vecNPCSprite.at(1)->runAction(Fation[1]);
		vecNPCSprite.at(2)->runAction(MoveBy::create(5.f, Vec2(visibleSize.width / 4 + 200, 0)));
		vecNPCSprite.at(2)->runAction(Fation[2]);

		}, 3.f, "open the curtains");

	this->GameMenu->setEnabled(true);

	SpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("NPC / NPC0_1.plist");
	SpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("NPC / NPC0_2.plist");
	SpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("NPC / NPC0_3.plist");

	return true;
}

void CMenuScene::OnStart(Ref* p_Sender)
{
	//菜单禁止使能
	this->GameMenu->setEnabled(false);

	//生成三兄弟开始出动的小动画
	this->vecNPCSprite.at(0)->stopAllActions();
	this->vecNPCSprite.at(1)->stopAllActions();
	this->vecNPCSprite.at(2)->stopAllActions();

	Animate* animate[3];
	animate[0] = Animate::create(vecAnimation.at(2 * 0 + 1));
	animate[1] = Animate::create(vecAnimation.at(2 * 1 + 1));
	animate[2] = Animate::create(vecAnimation.at(2 * 2 + 1));

	RepeatForever* Fation[3];
	Fation[0] = RepeatForever::create(animate[0]);
	Fation[1] = RepeatForever::create(animate[1]);
	Fation[2] = RepeatForever::create(animate[2]);

	vecNPCSprite.at(0)->runAction(MoveBy::create(3.f, Vec2(visibleSize.width + 400, 0)));
	vecNPCSprite.at(0)->runAction(Fation[0]);
	vecNPCSprite.at(1)->runAction(MoveBy::create(3.f, Vec2(visibleSize.width + 400, 0)));
	vecNPCSprite.at(1)->runAction(Fation[1]);
	vecNPCSprite.at(2)->runAction(MoveBy::create(3.f, Vec2(visibleSize.width + 400, 0)));
	vecNPCSprite.at(2)->runAction(Fation[2]);

	//启动三兄弟开始出动的小动画
	this->schedule([&](float s)->void {
 
		//启动主角摔倒动画
		if (vecNPCSprite.at(0)->getPositionX() > visibleSize.width - 20 
			&& vecNPCSprite.at(0)->getActionByTag(1000) == nullptr 
			&& bEndFlag == false)
		{
			vecNPCSprite.at(0)->stopAllActions();
			Animate* LeadAnimate = Animate::create(vecAnimation.at(6));
			LeadAnimate->setTag(1000);
			vecNPCSprite.at(0)->runAction(LeadAnimate);
			vecNPCSprite.at(0)->runAction(JumpBy::create(0.8f, Vec2(-180, 0), 140, 1));
		}

		//启动小弟B回头的动画
		if (vecNPCSprite.at(2)->getPositionX() > visibleSize.width - 60
			&& vecNPCSprite.at(2)->getActionByTag(1001) == nullptr
			&& bEndFlag == false
			)
		{
			vecNPCSprite.at(2)->stopAllActions();
			Animate* BrotherAnimate = Animate::create(vecAnimation.at(7));
			BrotherAnimate->setTag(1001);
			vecNPCSprite.at(2)->runAction(BrotherAnimate);
			this->unschedule("State Role Animetion");
		}
	}, 0.05f, "State Role Animetion");

	//关窗帘
	this->scheduleOnce([&](float s)->void {
			CurtainSpriteLeft->runAction(MoveBy::create(0.8f, Vec2(visibleSize.width / 2, 0)));
			CurtainSpriteRight->runAction(MoveBy::create(0.8f, Vec2(-visibleSize.width / 2, 0)));
			bEndFlag == true;
	}, 5.5f, "close the curtains");

	//关完窗帘就转换场景
	this->scheduleOnce([&](float s)->void {
			this->ClearScene();
			auto scene = CBattleScene::getInstance();
			Director::getInstance()->replaceScene(scene);
			this->unschedule("Atate Animetion");
	}, 7.0f, "Convert scene");

}

void CMenuScene::OnLoad(Ref* p_Sender)
{
	this->GameMenu->setEnabled(false);
	CCLOG("Load");
}

void CMenuScene::OnSet(Ref* p_Sender)
{
	this->GameMenu->setEnabled(false);
	CCLOG("Set");
}

void CMenuScene::OnEnd(Ref* p_Sender)
{
	this->GameMenu->setEnabled(false);
	exit(0);
}

void CMenuScene::ClearScene()
{
	//CurtainSpriteLeft->release();
	//CurtainSpriteRight->release();
	//vecNPCSprite.at(0)->release();
	//vecNPCSprite.at(1)->release();
	//vecNPCSprite.at(2)->release();
	//vecNPCSprite.clear();

	readJson.json_object.clear();

}

