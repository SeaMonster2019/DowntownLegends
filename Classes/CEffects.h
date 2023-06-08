#pragma once
#include <memory>
#include <string>
#include "cocos2d.h"
#include "jsonclass.h"

USING_NS_CC;
using namespace std;

//特效宏定义
#define WATERSPRAY 0 //水花特效


//特效类（单例）
class CEffects
{
public:
	bool CEffectsInit();

	static CEffects* getInstance()
	{
		static CEffects instance;
		return &(instance);
	}

	bool runEffects(Node* p_node, float p_x,float p_y, const int n_sType);

	void runCloseCurtain(Node* p_node);
	void runOpenCurtain(Node* p_node);

private:

	CEffects() {CEffectsInit();};
	~CEffects() {};
	CEffects(const CEffects&){};
	CEffects& operator=(const CEffects&){};

	unique_ptr<Vector<Animation*>> _vecEffectsAnimation;
	unique_ptr<vector<float>> _vecAnimationTime;

};
