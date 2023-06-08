#include "CMyUi.h"

const Vec2 CUpMenu::vecHeadUiPosition[NUMBER_OF_HEAD_UI] = { Vec2(30,30),Vec2(335,30),Vec2(640,30),Vec2(945,30) };

//初始化
bool CUpMenu::init()
{
	if (Sprite::init() == false)
	{
		return false;
	}
	else
	{
		for (int i = 0; i < NUMBER_OF_HEAD_UI; i++)
		{
			this->vecHaedUi[i].init();
			this->vecHaedUi[i].nNumber = i;
			this->vecHaedUi[i].UiPosition = CUpMenu::vecHeadUiPosition[i];
			this->vecHaedUi[i].HeadPictureFrame->setAnchorPoint(Vec2(0.5, 0.5));
			this->vecHaedUi[i].HeadPictureFrame->setPosition(this->vecHaedUi[i].UiPosition.x, this->vecHaedUi[i].UiPosition.y);
			this->vecHaedUi[i].HeadPictureFrame->setScale(2.f, 2.f);
			this->vecHaedUi[i].HeadPictureFrame->setZOrder(2);

			this->vecHaedUi[i].HeadPicture->setAnchorPoint(Vec2(0.5, 0.5));
			this->vecHaedUi[i].HeadPicture->setPosition(this->vecHaedUi[i].UiPosition.x - 4, this->vecHaedUi[i].UiPosition.y + 3);
			this->vecHaedUi[i].HeadPicture->setScale(2.f, 2.f);
			this->vecHaedUi[i].HeadPicture->setZOrder(3);

			this->vecHaedUi[i].Name->setAnchorPoint(Vec2(0.5, 0.5));
			this->vecHaedUi[i].Name->setPosition(this->vecHaedUi[i].UiPosition.x + 60, this->vecHaedUi[i].UiPosition.y);

			this->vecHaedUi[i].HP->setAnchorPoint(Vec2(0.5, 0.5));
			this->vecHaedUi[i].HP->setPosition(this->vecHaedUi[i].UiPosition.x + 140, this->vecHaedUi[i].UiPosition.y);

			this->vecHaedUi[i].MP->setAnchorPoint(Vec2(0.5, 0.5));
			this->vecHaedUi[i].MP->setPosition(this->vecHaedUi[i].UiPosition.x + 200, this->vecHaedUi[i].UiPosition.y);

			this->vecHaedUi[i].vecHeadUiSpritePtr.pushBack(this->vecHaedUi[i].HeadPicture);
			this->vecHaedUi[i].vecHeadUiSpritePtr.pushBack(this->vecHaedUi[i].HeadPictureFrame);
			this->vecHaedUi[i].vecHeadUiSpritePtr.pushBack(this->vecHaedUi[i].Name);
			this->vecHaedUi[i].vecHeadUiSpritePtr.pushBack(this->vecHaedUi[i].HP);
			this->vecHaedUi[i].vecHeadUiSpritePtr.pushBack(this->vecHaedUi[i].MP);

			this->vecHaedUi[i].bDisplay = false;

			this->setTexture("Ui/upMuen.png");

		}
		return true;
	}
}

//显示一个角色信息
void CUpMenu::displayRoleInformation(CRole* const p_role)
{
	if (p_role == nullptr)
	{
#if DEBUG_MODE
	CCLOG("at funtion CUpMenu::showRoleInformation,parameter p_role is nullptr");
#endif
		return;
	}

	for (int i = 0; i < NUMBER_OF_HEAD_UI; i++)
	{
		if (this->vecHaedUi[i].bDisplay == false && !p_role->_structRoleAbility.bShowInformation)
		{
			this->vecHaedUi[i].bDisplay == true;

			this->vecHaedUi[i].nRoleId = p_role->_structRoleAbility.nRoleNumberCode;

			Rect spriteFrameRect = p_role->getSpriteFrame()->getRect();
			spriteFrameRect.origin.x += 0;
			spriteFrameRect.origin.y += 0;
			spriteFrameRect.size.width = 16;
			spriteFrameRect.size.height = 16;

			SpriteFrame* HeadPicturesSpriteFrame
				= SpriteFrame::createWithTexture(
					p_role->getTexture(),
					spriteFrameRect,
					p_role->getSpriteFrame()->isRotated(),
					p_role->getSpriteFrame()->getOffset(),
					Size(16, 16)
				);

			this->vecHaedUi[i].HeadPicture->setSpriteFrame(HeadPicturesSpriteFrame);
			this->vecHaedUi[i].Name->setString(p_role->_structRoleAbility.sRoleName);
			this->vecHaedUi[i].HP->setString("HP:" + to_string(p_role->_structRoleAbility.nCurrentHP));
			this->vecHaedUi[i].MP->setString("MP:0");
			this->vecHaedUi[i].bDisplay = true;
			for (int j = 0; j < this->vecHaedUi[i].vecHeadUiSpritePtr.size(); j++)
			{
				this->addChild(this->vecHaedUi[i].vecHeadUiSpritePtr.at(j));
			}

			p_role->_structRoleAbility.bShowInformation = true;
			p_role->_structRoleAbility.headUiPtr = &this->vecHaedUi[i];
			return; 
		}
	}
}

//交换两个角色的Ui
void CUpMenu::swapHeadUiData(const unsigned int p_nNumberOfHeadUiA, const unsigned int p_nNumberOfHeadUiB)
{
	int nRoleID = this->vecHaedUi[p_nNumberOfHeadUiA].nRoleId;
	SpriteFrame* spriteFrameTemp = this->vecHaedUi[p_nNumberOfHeadUiA].HeadPicture->getSpriteFrame()->clone();
	string name = this->vecHaedUi[p_nNumberOfHeadUiA].Name->getString();
	string hp = this->vecHaedUi[p_nNumberOfHeadUiA].HP->getString();
	string mp = this->vecHaedUi[p_nNumberOfHeadUiA].MP->getString();
	bool bDisplay = this->vecHaedUi[p_nNumberOfHeadUiA].bDisplay;


	this->vecHaedUi[p_nNumberOfHeadUiA].nRoleId = this->vecHaedUi[p_nNumberOfHeadUiB].nRoleId;
	this->vecHaedUi[p_nNumberOfHeadUiA].HeadPicture->setSpriteFrame(this->vecHaedUi[p_nNumberOfHeadUiB].HeadPicture->getSpriteFrame());
	this->vecHaedUi[p_nNumberOfHeadUiA].Name->setString(this->vecHaedUi[p_nNumberOfHeadUiB].Name->getString());
	this->vecHaedUi[p_nNumberOfHeadUiA].HP->setString(this->vecHaedUi[p_nNumberOfHeadUiB].HP->getString());
	this->vecHaedUi[p_nNumberOfHeadUiA].MP->setString(this->vecHaedUi[p_nNumberOfHeadUiB].MP->getString());
	this->vecHaedUi[p_nNumberOfHeadUiA].bDisplay = this->vecHaedUi[p_nNumberOfHeadUiB].bDisplay;


	this->vecHaedUi[p_nNumberOfHeadUiB].nRoleId = nRoleID;
	this->vecHaedUi[p_nNumberOfHeadUiB].HeadPicture->setSpriteFrame(spriteFrameTemp);
	this->vecHaedUi[p_nNumberOfHeadUiB].Name->setString(name);
	this->vecHaedUi[p_nNumberOfHeadUiB].HP->setString(hp);
	this->vecHaedUi[p_nNumberOfHeadUiB].MP->setString(mp);
	this->vecHaedUi[p_nNumberOfHeadUiB].bDisplay = bDisplay;

}

//删除一个正在显示的角色信息
void CUpMenu::deleteRoleInformation(CRole* const p_role)
{
	if (p_role == nullptr)
	{
#if DEBUG_MODE
		CCLOG("at funtion CUpMenu::deleteRoleInformation,parameter p_role is nullptr");
#endif
		return;
	}

	for (int i = 0; i < NUMBER_OF_HEAD_UI; i++)
	{
		if (CUpMenu::vecHaedUi[i].bDisplay == true
			&& CUpMenu::vecHaedUi[i].nRoleId == p_role->_structRoleAbility.nRoleNumberCode)
		{
			//将要删除的角色信息冒泡至显示UI队列底，在进行删除
			for (int j = i; j < NUMBER_OF_HEAD_UI - 1; j++)
			{
				if (j == NUMBER_OF_HEAD_UI - 1 || CUpMenu::vecHaedUi[j + 1].bDisplay == false)
				{
					for (int k = 0; k < CUpMenu::vecHaedUi[j].vecHeadUiSpritePtr.size(); k++)
					{
						CUpMenu::getInstance()->removeChild(CUpMenu::vecHaedUi[j].vecHeadUiSpritePtr.at(k), true);
					}
					CUpMenu::vecHaedUi[j].bDisplay = false;
					break;
				}
				else
				{
					CUpMenu::swapHeadUiData(j, j + 1);
				}
			}
			p_role->_structRoleAbility.bShowInformation = false;
			p_role->_structRoleAbility.headUiPtr = nullptr;
			break;
		}
	}
}

//删除所有角色信息
void CUpMenu::deleteAllRoleInformation()
{
	//删除显示的Ui
	for(int i = NUMBER_OF_HEAD_UI - 1; i >= 0 ; i--)
	{
		for (int j = 0; j < CUpMenu::vecHaedUi[i].vecHeadUiSpritePtr.size(); j++)
		{
			CUpMenu::getInstance()->removeChild(CUpMenu::vecHaedUi[i].vecHeadUiSpritePtr.at(j), true);
		}
		CUpMenu::vecHaedUi[i].bDisplay = false;
	}

	//设置角色容器内角色内部显示参数
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < CRole::vecRoles[j]->size(); i++)
		{
			CRole::vecRoles[j]->at(i)->_structRoleAbility.bShowInformation = false;
			CRole::vecRoles[j]->at(i)->_structRoleAbility.headUiPtr = nullptr;
		}
	}


}

