#pragma once
#include <string>
#include"cocos2d.h"
#include"jsonclass.h"
#include"CBattleScene.h"

USING_NS_CC;

class CMenuScene : public cocos2d::Scene
{
public:

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(CMenuScene);

	void OnStart(Ref* p_Sender);
	void OnLoad(Ref* p_Sender);
	void OnSet(Ref* p_Sender);
	void OnEnd(Ref* p_Sender);

	void ClearScene();

private:

	Readjson readJson;

	Vector<Animation*> vecAnimation;

	Sprite* CurtainSpriteLeft, * CurtainSpriteRight;
	Vector<Sprite*> vecNPCSprite;

	Size visibleSize;
	Vec2 origin;

	Menu* GameMenu;

	bool bEndFlag;

};
