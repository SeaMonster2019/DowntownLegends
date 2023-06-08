#pragma once
#include <string>
#include "cocos2d.h"
#include "CRole.h"

USING_NS_CC;
using namespace std;

#define NUMBER_OF_HEAD_UI  4
#define NUMBER_OF_HEAD_UI_COMPONENTS 5

//前向声明
class CRole;

struct SHeadUi
{
	int nNumber;
	int nRoleId;
	Vec2 UiPosition;
	Vector<Node*> vecHeadUiSpritePtr;
	Sprite* HeadPictureFrame;
	Sprite*  HeadPicture;
	Label* Name;
	Label*  HP;
	Label*  MP;
	bool bDisplay;

	void init() {
		this->HeadPictureFrame = Sprite::create("Ui/HeadPortraitBox.png");
		this->HeadPicture = Sprite::create("Ui/HeadPortraitBox.png");
		this->Name = Label::createWithSystemFont("name", "", 16);
		this->HP = Label::create("HP:", "", 16);
		this->MP = Label::create("MP:", "", 16);
	}

	void updateHP(int p_newHP){
		this->HP->setString("HP:" + to_string(p_newHP));
	}

	void updateMP(int p_newMP) {
		this->MP->setString("MP:" + to_string(p_newMP));
	}

};

class CUpMenu :public Sprite
{
friend class IUiInterface;
CREATE_FUNC(CUpMenu);

//interface 
public:

	//显示角色信息
	void displayRoleInformation(CRole* const p_role);
	//删除显示的角色信息
	void deleteRoleInformation(CRole* const p_role);
	//删除所有显示的角色信息
	void deleteAllRoleInformation();

private:

	//初始化
	virtual bool init();

	//交换
	void swapHeadUiData(const unsigned int p_nNumberOfHeadUiA, const unsigned int p_nNumberOfHeadUiB);


private:

	//Ui位置信息数组
	static const  Vec2 vecHeadUiPosition[NUMBER_OF_HEAD_UI];

	//显示Ui结构体数组
	SHeadUi vecHaedUi[NUMBER_OF_HEAD_UI];

//单例运行
public:
	static CUpMenu* getInstance()
	{
		static CUpMenu* const instance = CUpMenu::create();
		return instance;
	}
private:
	CUpMenu(){};
	~CUpMenu(){};
	CUpMenu(const CUpMenu&);
	CUpMenu& operator=(const CUpMenu&);

};
