#pragma once

#include "cocos2d.h"
#include <iostream>

USING_NS_CC;
using namespace std;

class ChineseDemo : public Scene
{ 
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(ChineseDemo);
	Label* _labelA;
	string str;
};