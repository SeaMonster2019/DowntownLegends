#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}


bool HelloWorld::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //Create background image Sprite、创建背景图片 
    auto sprite = Sprite::create("HelloWorld.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        this->addChild(sprite, 0);
    }


    CCLOG("1");
    //创建精灵动画
    SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("NPC/NPC1_0.plist");
    CCLOG("2");
    Sprite* spriterun = Sprite::createWithSpriteFrameName("NPC1_0 (1).png");
    CCLOG("3");
    spriterun->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    spriterun->setScale(3.5f, 3.5f);
    addChild(spriterun);
    Vector<SpriteFrame*> vecSpriteFrame;
    for(int i = 1; i <= 30; i++)
    {
        Ref;
        CCLOG("4");
        char photoname[20];
        sprintf(photoname, "NPC1_0 (%d).png",i);
        SpriteFrame* spriteFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(photoname);
        vecSpriteFrame.pushBack(spriteFrame);
    }
    Animation* ani = Animation::createWithSpriteFrames(vecSpriteFrame,0.05f);
    Animate* animate = Animate::create(ani);
    spriterun->runAction(RepeatForever::create(animate));

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
