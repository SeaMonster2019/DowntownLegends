#pragma once
#include <string>
#include "cocos2d.h"
#include "CMyUi.h"
#include "CRole.h"

USING_NS_CC;
using namespace std;

class IUiInterface 
{
public:
    
	void UiDisplayRoleInformation(Node* const p_node,const string p_sName,const int p_nHp,const int p_nMp);

    void UiStopDisplayRoleInformation(const string p_sName);

    //static void clearAllHeadUi(CUpMenu* const p_upMenu);
};