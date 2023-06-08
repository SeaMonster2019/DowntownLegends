#pragma once
#include <string>
#include "cocos2d.h"
#include "CRole.h"

USING_NS_CC;
using namespace std;

#define NUMBER_OF_HEAD_UI  4
#define NUMBER_OF_HEAD_UI_COMPONENTS 5

//ǰ������
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

	//��ʾ��ɫ��Ϣ
	void displayRoleInformation(CRole* const p_role);
	//ɾ����ʾ�Ľ�ɫ��Ϣ
	void deleteRoleInformation(CRole* const p_role);
	//ɾ��������ʾ�Ľ�ɫ��Ϣ
	void deleteAllRoleInformation();

private:

	//��ʼ��
	virtual bool init();

	//����
	void swapHeadUiData(const unsigned int p_nNumberOfHeadUiA, const unsigned int p_nNumberOfHeadUiB);


private:

	//Uiλ����Ϣ����
	static const  Vec2 vecHeadUiPosition[NUMBER_OF_HEAD_UI];

	//��ʾUi�ṹ������
	SHeadUi vecHaedUi[NUMBER_OF_HEAD_UI];

//��������
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
