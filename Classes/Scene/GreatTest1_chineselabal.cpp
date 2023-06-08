#include "GreatTest1_chineselabal.h"
#include "json.h"
#include "cocos2d.h"
#include "jsonclass.h"

using namespace std;
USING_NS_CC;

Scene* ChineseDemo::createScene()
{
	return ChineseDemo::create();
}


bool ChineseDemo::init()
{
	CCLOG("go to init!");
	if(!Scene::init()) return false;

	/*
	//找到场景原点和大小
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	//生成背景精灵
	auto sprite = Sprite::create("photo/underworld.jpeg");
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	sprite->setPosition(Vec2(visibleSize / 2));
	this->addChild(sprite);

	//生成标签
	_labelA = Label::createWithSystemFont("","微软雅黑",20);
	_labelA->setAnchorPoint(Vec2::ANCHOR_MIDDLE);  //设置锚点
	_labelA->setPosition(Vec2(visibleSize / 2));  //设置坐标
	_labelA->setDimensions(400, 0);
	_labelA->setTextColor(Color4B(20,200,20,200));
	_labelA->enableBold();
	this->addChild(_labelA);
	Readjson readjson;
	if (!readjson.JsonClassInit(FileUtils::getInstance()->getStringFromFile("res/Textfile.json")))
		CCLOG("json action false......");
	str = readjson.json_object["story1"].asString();
	static int i = 0;
	this->schedule([&](float dlt){
		char cheak = str[i];
		if (cheak > -127 && cheak < 0)
		{ 
			i += 3;
		}
		else
		{
			i++;
		}
		string showstr = str.substr(0, i);
		_labelA->setString(showstr);
		CCLOG("调度器正在运行第%d次",i);
		if (i > str.length())
		{
			CCLOG("退出调度器");
			this->unschedule("showstring");
		}
	}, 0.1f,"showstring");

	*/
	return true;
}


