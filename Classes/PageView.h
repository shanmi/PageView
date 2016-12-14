#ifndef __PAGE_VIEW_H__
#define __PAGE_VIEW_H__
#include "cocos2d.h"

class PageView
	:public cocos2d::CCNode
	, public cocos2d::CCTouchDelegate
{
public:
	static PageView *create(const cocos2d::CCSize &size);
	void setTouchPriority(int touchPriority){ m_touchPriority = touchPriority; }
	cocos2d::CCNode *getNode(int index);
	void removeNode(int index);
	int getCount(){ return m_nodes.size(); }
	virtual void addNode(cocos2d::CCNode *node);

protected:
	PageView(const cocos2d::CCSize &size);
	virtual bool init();
	virtual void onTouchBegan(cocos2d::CCTouch *pTouch){}
	virtual void onTouchMoved(cocos2d::CCTouch *pTouch){}
	virtual void onTouchEnded(cocos2d::CCTouch *pTouch){}
	void doMove(cocos2d::CCPoint offset);
	void dragback();
	void checkNodePosition();
	void movingPage(float dt);
	void movePageEnd();
	void touchEnd();

private:
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

private:
	int m_touchPriority;
	cocos2d::CCSize m_size;
	cocos2d::CCNode *m_container;
	cocos2d::CCClippingNode *m_clippingNode;
	std::vector<cocos2d::CCNode *> m_nodes;
	bool m_isTouching;
	bool m_isMoving;
	int pageIndex;
};

#endif