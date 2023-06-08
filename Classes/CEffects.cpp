#include "CEffects.h"

bool CEffects::CEffectsInit()
{
	_vecEffectsAnimation = make_unique<Vector<Animation*>>();
	_vecAnimationTime = make_unique<vector<float>>();

	Readjson readjson;
	if(!readjson.JsonClassInit(FileUtils::getInstance()->getStringFromFile("Special effects/SpecialEffects.json")))
	{ 
		CCLOG("read effects animattion jsonfile false!");
		return false;
	}
	int len = 0;
	Vector<SpriteFrame*> vecSpriteFrame;
	len = readjson.json_object["list"][len].asInt();
	for (int i = 1; i <= len; i++)
	{
		string sKey = readjson.json_object["list"][i].asString();
		int len1 = 0;
		len1 = readjson.json_object[sKey][len1].asInt();
		_vecAnimationTime->push_back(len1 * readjson.json_object[sKey][1].asDouble());
		vecSpriteFrame.clear();
		SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Special effects/" + sKey + ".plist");
		for (int j = 2; j <= len1; j++)
		{
			SpriteFrame* spriteFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(sKey + " (" + readjson.json_object[sKey][j].asString() + ").png");
			vecSpriteFrame.pushBack(spriteFrame);
		}
		_vecEffectsAnimation->pushBack(Animation::createWithSpriteFrames(vecSpriteFrame, readjson.json_object[sKey][1].asDouble()));
	}
}


//运行特效
bool CEffects::runEffects(Node* p_node, float p_x, float p_y,const int p_sType)
{
	Animate* animate = NULL;

	//初始化特效精灵
	Sprite* effectsSprite = NULL;
	effectsSprite = Sprite::create();
	if (effectsSprite == nullptr)
	{
		CCLOG("effectsSprite create false!");
		return false;
	}
	static int nNumberFlag = 0;
	++nNumberFlag;
	string sNumberFlag = "effects" + to_string(nNumberFlag);
	effectsSprite->setName(sNumberFlag);

	switch (p_sType)
	{
	case WATERSPRAY:
	{
		animate = Animate::create(this->_vecEffectsAnimation->at(WATERSPRAY));
		animate->setTag(WATERSPRAY);
		effectsSprite->setPosition(Vec2(p_x, p_y));
		effectsSprite->setAnchorPoint(Vec2(0.5f, 0.f));
		p_node->addChild(effectsSprite);
		effectsSprite->runAction(animate);
		break;
	}
	default:
	{
		effectsSprite->release();
		--nNumberFlag;
		return false;
	}
	}
	float fTime = _vecAnimationTime->at(p_sType);

	//删掉特效的函数
	p_node->scheduleOnce([p_node,effectsSprite, sNumberFlag](float s) {
			p_node->removeChildByName(sNumberFlag);
			effectsSprite->release();
		}, fTime, "check_effects");

	return true;
}


//运行关窗帘特效
void CEffects::runCloseCurtain(Node *p_node)
{
	if (p_node == nullptr)
	{
		CCLOG("at CEffects::runCloseCurtain funtion, parameter p_node is noll");
		return;
	}

	//生成窗帘左右精灵
	Sprite* LefeCurtainSprite = nullptr;
	Sprite* RightCurtainSprite = nullptr;
	LefeCurtainSprite = Sprite::create("Special effects/curtain.png");
	RightCurtainSprite = Sprite::create("Special effects/curtain.png");
	if (LefeCurtainSprite == nullptr || RightCurtainSprite == nullptr)
	{
		CCLOG("at runCloseCurtain funtion ,CurtainSprite creat false");
		return;
	}
	//设置精灵属性
	LefeCurtainSprite->setAnchorPoint(Vec2(1, 0));
	LefeCurtainSprite->setScaleX(Director::getInstance()->getVisibleSize().width/2/ LefeCurtainSprite->getContentSize().width );
	LefeCurtainSprite->setScaleY(Director::getInstance()->getVisibleSize().height / LefeCurtainSprite->getContentSize().height);
	LefeCurtainSprite->setPosition(Vec2(0, 0));
	LefeCurtainSprite->setName("LefeCurtainSprite");
	RightCurtainSprite->setAnchorPoint(Vec2(0, 0));
	RightCurtainSprite->setScaleX(Director::getInstance()->getVisibleSize().width/2/ LefeCurtainSprite->getContentSize().width );
	RightCurtainSprite->setScaleY(Director::getInstance()->getVisibleSize().height / LefeCurtainSprite->getContentSize().height);
	RightCurtainSprite->setPosition(Vec2(Director::getInstance()->getVisibleSize().width , 0));
	RightCurtainSprite->setName("RightCurtainSprite");
	p_node->addChild(LefeCurtainSprite);
	p_node->addChild(RightCurtainSprite);
	//运行动画
	LefeCurtainSprite->runAction(MoveBy::create(0.5f,Vec2(Director::getInstance()->getVisibleSize().width/2,0)));
	RightCurtainSprite->runAction(MoveBy::create(0.5f, -Vec2(Director::getInstance()->getVisibleSize().width / 2, 0)));
}


//运行开窗帘特效
void CEffects::runOpenCurtain(Node* p_node)
{
	if (p_node == nullptr)
	{
		CCLOG("at CEffects::runCloseCurtain funtion, parameter p_node is noll");
		return;
	}
	//找到在关窗帘函数中生成的窗帘精灵
	Node* LefeCurtainSprite = p_node->getChildByName("LefeCurtainSprite");
	Node* RightCurtainSprite = p_node->getChildByName("RightCurtainSprite");
	if (LefeCurtainSprite == nullptr || RightCurtainSprite == nullptr)
	{
		CCLOG("at CEffects::runOpenCurtain funtion, can't find CurtainSprite");
		return;
	}
	//运行动画
	LefeCurtainSprite->runAction(MoveBy::create(0.5f, -Vec2(Director::getInstance()->getVisibleSize().width / 2, 0)));
	RightCurtainSprite->runAction(MoveBy::create(0.5f, Vec2(Director::getInstance()->getVisibleSize().width / 2, 0)));
	//生成清除窗帘精灵的调度器
	p_node->scheduleOnce([p_node = p_node] (float s){
		p_node->removeChildByName("LefeCurtainSprite");
		p_node->removeChildByName("RightCurtainSprite");
		},0.5f,"REMOVE_CURTAINSPRITE");
}
