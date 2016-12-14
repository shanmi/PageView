#include "PageView.h"

USING_NS_CC;

PageView *PageView::create(const CCSize &size)
{
	PageView *view = new PageView(size);
	view->init();
	view->autorelease();
	return view;
}

PageView::PageView(const CCSize &size)
:m_size(size)
, m_isTouching(false)
, m_isMoving(false)
, pageIndex(0)
{

}

bool PageView::init()
{
	setContentSize(m_size);
	setAnchorPoint(ccp(0.5f, 1.0f));

	auto colorLayer = CCLayerColor::create(ccc4(125, 0, 0, 255));
	colorLayer->setContentSize(m_size);
	addChild(colorLayer);

	auto back = CCLayerColor::create(ccc4(125, 0, 0, 255));
	m_clippingNode = CCClippingNode::create();
	m_clippingNode->setContentSize(m_size);
	m_clippingNode->setPosition(ccp(0, 0));
	m_clippingNode->setInverted(false);
	m_clippingNode->setAlphaThreshold(1.0f);
	m_clippingNode->setStencil(back);
	addChild(m_clippingNode);

	m_container = CCNode::create();
	m_container->setPosition(ccp(0, m_size.height / 2));
	m_clippingNode->addChild(m_container);

	schedule(schedule_selector(PageView::movingPage), 3.0f);

	return true;
}

void PageView::addNode(CCNode *node)
{
	float contentWidth = m_container->getContentSize().width;
	float contentHeight = m_container->getContentSize().height;

	float nodeWidth = node->getContentSize().width;
	float nodeHeight = node->getContentSize().height;

	float scale = 1.0f;
	float scaleX = m_size.width / nodeWidth;
	float scaleY = m_size.height / nodeHeight;
	if (scaleX < 1 || scaleY < 1)
	{
		if (scaleX < scaleY)
		{
			scale = scaleX;
		}
		else
		{
			scale = scaleY;
		}
	}
	node->setScale(scale);
	node->ignoreAnchorPointForPosition(false);
	node->setAnchorPoint(ccp(0.5f, 0.5f));
	m_container->addChild(node);
	m_nodes.push_back(node);

	float newPosX = contentWidth + m_size.width / 2;
	float newPosY = m_container->getContentSize().height / 2;
	node->setPosition(ccp(newPosX, newPosY));

	float curWidth = contentWidth + m_size.width;
	m_container->setContentSize(ccp(curWidth, contentHeight));
}

void PageView::removeNode(int index)
{
	auto node = getNode(index);
	auto iter = find(m_nodes.begin(), m_nodes.end(), node);
	if (iter != m_nodes.end())
	{
		m_nodes.erase(iter);
		node->removeFromParent();
	}
}

CCNode *PageView::getNode(int index)
{
	int count = getCount();
	if (index < 0 || index > count - 1) return NULL;
	return m_nodes[index];
}

void PageView::onEnter()
{
	CCNode::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, m_touchPriority, true);
}

void PageView::onExit()
{
	CCNode::onExit();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool PageView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (m_isMoving) return false;

	auto localPos = convertToNodeSpace(pTouch->getLocation());
	auto size = getContentSize();

	CCRect rect(0, 0, size.width, size.height);
	if (!rect.containsPoint(localPos)) return false;

	unschedule(schedule_selector(PageView::movingPage));

	m_isTouching = true;
	onTouchBegan(pTouch);
	return true;
}

void PageView::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	m_isTouching = true;

	auto curPos = pTouch->getLocation();
	auto prePos = pTouch->getPreviousLocation();

	doMove(ccpSub(curPos, prePos));
	onTouchMoved(pTouch);
}

void PageView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	dragback();
	checkNodePosition();
	onTouchEnded(pTouch);
}

void PageView::doMove(CCPoint offset)
{
	CCPoint contentPos = m_container->getPosition();

	float newX = contentPos.x + offset.x;
	float newY = contentPos.y;
	auto tarPos = ccp(newX, newY);
	m_container->setPosition(tarPos);
}

void PageView::dragback()
{
	auto containerSize = m_container->getContentSize();
	auto curPos = m_container->getPosition();

	if (containerSize.width <= m_size.width)
	{
		return;
	}

	float minX = -m_container->getContentSize().width + m_size.width;
	float maxX = 0;

	float newX = min(max(curPos.x, minX), maxX);
	m_container->setPositionX(newX);

}

void PageView::checkNodePosition()
{
	CCPoint pos = m_container->getPosition();
	auto callback = CCCallFunc::create(this, callfunc_selector(PageView::touchEnd));
	if (pos.x < -m_size.width*pageIndex - m_size.width / 2)
	{
		auto newPos = ccp(-m_size.width*(++pageIndex), m_container->getPositionY());
		auto moveTo = CCMoveTo::create(0.6f, newPos);
		auto ease = CCEaseSineInOut::create(moveTo);
		auto sequence = CCSequence::create(ease, callback, NULL);
		m_container->runAction(sequence);
	}
	else if (pos.x > -m_size.width*pageIndex + m_size.width / 2)
	{
		auto newPos = ccp(-m_size.width*(--pageIndex), m_container->getPositionY());
		auto moveTo = CCMoveTo::create(0.6f, newPos);
		auto ease = CCEaseSineInOut::create(moveTo);
		auto sequence = CCSequence::create(ease, callback, NULL);
		m_container->runAction(sequence);
	}
	else
	{
		auto newPos = ccp(-m_size.width*(pageIndex), m_container->getPositionY());
		auto moveTo = CCMoveTo::create(0.3f, newPos);
		auto ease = CCEaseSineInOut::create(moveTo);
		auto sequence = CCSequence::create(ease, callback, NULL);
		m_container->runAction(sequence);
	}
}

void PageView::movingPage(float dt)
{
	if (m_isTouching) return;

	m_isMoving = true;
	if (++pageIndex > getCount() - 1)
	{
		pageIndex = 0;
	}
	float moveX = -m_size.width * pageIndex;
	auto moveTo = CCMoveTo::create(1.2f, ccp(moveX, m_container->getPositionY()));
	auto ease = CCEaseSineInOut::create(moveTo);
	auto callback = CCCallFunc::create(this, callfunc_selector(PageView::movePageEnd));
	auto sequence = CCSequence::create(ease, callback, NULL);
	m_container->runAction(sequence);
}

void PageView::movePageEnd()
{
	m_isMoving = false;
}

void PageView::touchEnd()
{
	m_isTouching = false;
	schedule(schedule_selector(PageView::movingPage), 3.0f);
}