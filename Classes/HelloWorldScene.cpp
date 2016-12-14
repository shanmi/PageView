#include "HelloWorldScene.h"
#include "PageView.h"
#include "PageNode.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!CCLayer::init())
	{
		return false;
	}

	CCDirector *pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, false);

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		this,
		menu_selector(HelloWorld::menuCloseCallback));

	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width / 2,
		origin.y + pCloseItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	CCSprite* pSprite = CCSprite::create("HelloWorld.png");
	pSprite->setPosition(ccp(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(pSprite, 0);

	auto size = ccp(visibleSize.width, visibleSize.height / 4);
	PageView *pageView = PageView::create(size);
	pageView->setPosition(ccp(visibleSize.width / 2, visibleSize.height));
	for (int i = 0; i < 4; i++)
	{
		auto node = PageNode::create(size, "banner.png", "http://desk.fd.zol-img.com.cn/t_s1920x1080c5/g5/M00/00/0C/ChMkJ1gF5IyIVpQZAAfWfSNJAuoAAXABwHjE1wAB9aV014.jpg");
		pageView->addNode(node);
	}
	addChild(pageView);

	return true;
}

void HelloWorld::explodeFire(float dt){
	CCParticleExplosion *pEmitter = CCParticleExplosion::create();
	pEmitter->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
	pEmitter->setAutoRemoveOnFinish(true);

	pEmitter->setPosition(mPoint);

	//Size
	pEmitter->setStartSize(3.0);
	pEmitter->setStartSizeVar(5.0);

	//pEmitter->setTotalParticles(10);

	pEmitter->setRadialAccel(10.0);
	pEmitter->setRadialAccelVar(0.0);

	pEmitter->setLife(0.2f);
	pEmitter->setLifeVar(1.5f);

	pEmitter->setDuration(0.5);

	addChild(pEmitter);

}


bool HelloWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
	int x = pTouch->getLocation().x;
	int y = pTouch->getLocation().y;
	mPoint = ccp(x, y);
	schedule(schedule_selector(HelloWorld::explodeFire), 0.0f);
	return true;
}

void HelloWorld::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
	int x = pTouch->getLocation().x;
	int y = pTouch->getLocation().y;
	mPoint = ccp(x, y);
}

void HelloWorld::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
	unschedule(schedule_selector(HelloWorld::explodeFire));
}

void HelloWorld::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent){

}



void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif
}


