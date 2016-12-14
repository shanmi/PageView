#include "ListView.h"

USING_NS_CC;
using namespace std;
using namespace std::placeholders;


ListView *ListView::create(const CCSize &size)
{
	ListView *view = new ListView(size);
	view->init();
	view->autorelease();
	return view;
}


ListView::ListView(const CCSize &size)
: m_size(size)
, m_touchPriority(0)
, m_spacingX(0)
, m_spacingY(0)
, m_columnSize(1)
, m_platHeight(0)
, m_isTouching(false)
, m_isSliding(false)
, m_isSliderVisible(false)
{
	
}

bool ListView::init()
{
	setContentSize(m_size);

	/*CCLayerColor *maskNode = CCLayerColor::create(ccc4(0, 255, 0, 75));
	maskNode->setContentSize(m_size);
	addChild(maskNode);*/

	CCLayerColor *back = CCLayerColor::create(ccc4(125, 0, 0, 255));
	back->setContentSize(m_size);
	m_clippingNode = CCClippingNode::create();
	m_clippingNode->setInverted(false);
	m_clippingNode->setAlphaThreshold(1.0f);
	m_clippingNode->setStencil(back);
	addChild(m_clippingNode);

	m_container = CCNode::create();
	m_clippingNode->addChild(m_container);
	m_sliderBg = CCSprite::create("shop/slider_2-1.png");
	m_slider = CCSprite::create("shop/slider_2-3.png");
	addChild(m_sliderBg);
	float scale = 1.0f;
	if (m_sliderBg->getContentSize().height + m_slider->getContentSize().height > m_size.height)
	{
		scale = m_size.height / (m_sliderBg->getContentSize().height + m_slider->getContentSize().height);
	}
	m_sliderBg->setScale(scale);
	m_sliderBg->setContentSize(m_sliderBg->getContentSize()*scale);
	m_sliderBg->setPosition(ccp(m_size.width - m_sliderBg->getContentSize().width / 2, m_size.height * scale/ 2));
	m_sliderBg->setVisible(m_isSliderVisible);

	auto slider1 = CCSprite::create("shop/slider_2-2.png");
	slider1->setPosition(ccp(m_sliderBg->getContentSize().width / (scale * 2), m_sliderBg->getContentSize().height / scale - slider1->getContentSize().height*0.1f / scale));
	slider1->setScaleY(-1);
	m_sliderBg->addChild(slider1);
	auto slider2 = CCSprite::create("shop/slider_2-2.png");
	slider2->setPosition(ccp(m_sliderBg->getContentSize().width / (scale * 2), slider2->getContentSize().height*0.1f / scale));
	m_sliderBg->addChild(slider2);

	m_slider->setScale(scale);
	m_slider->setContentSize(m_slider->getContentSize()*scale);
	m_slider->setPosition(ccp(m_size.width - m_slider->getContentSize().width / 2, m_sliderBg->getPositionY() + m_sliderBg->getContentSize().height/(2*scale) - m_slider->getContentSize().height*1.1f/ scale));
	addChild(m_slider);
	m_slider->setVisible(m_isSliderVisible);

	return true;
}
void ListView::setSliderVisible(bool visible)
{
	m_isSliderVisible = visible;
	m_sliderBg->setVisible(m_isSliderVisible);
	m_slider->setVisible(m_isSliderVisible);
}

int ListView::addNode(cocos2d::CCNode *node)
{
	float contentHeight = m_container->getContentSize().height;
	float contentWidth = m_container->getContentSize().width;

	m_container->addChild(node);
	int height = contentHeight;
	auto nodeSize = node->getContentSize();
	if (m_platHeight == 0)
	{
		m_platHeight = nodeSize.height;
	}

	int rate = 0;
	int startX = (m_size.width - m_columnSize*nodeSize.width - (m_columnSize - 1)*m_spacingX) / 2;
	int newWidth = startX + (nodeSize.width + m_spacingX) * (m_nodes.size() % m_columnSize);
	int newHeight = -(nodeSize.height + m_spacingY)*(m_nodes.size() / m_columnSize);
	if (m_nodes.size() % m_columnSize == 0)
	{
		rate = 1;
	}
	
	node->setPosition(ccp(newWidth, newHeight));
	
	m_nodes.push_back(node);

	float maxWidth = max(contentWidth, nodeSize.width * m_columnSize);
	float curHeight = height + rate*nodeSize.height;
	m_container->setContentSize(CCSize(maxWidth, curHeight));
	m_container->setPosition(ccp(0, m_size.height - nodeSize.height - m_spacingY));

	if (m_isSliderVisible)
	{
		if (m_container->getContentSize().height <= m_size.height)
		{
			setSliderVisible(false);
			m_isSliderVisible = true;
		}
		else
		{
			setSliderVisible(true);
		}
	}

	return m_nodes.size() - 1;
}

void ListView::removeNode(int index)
{
	auto node = getNode(index);
	if (node)
	{
		node->removeFromParent();
	}
}

cocos2d::CCNode *ListView::getNode(int index)
{
	int nodeAmount = count();
	if (index < 0 || index > nodeAmount - 1) return NULL;

	return m_nodes[index];
}

void ListView::onEnter()
{
	CCNode::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, m_touchPriority, true);
}

void ListView::onExit()
{
	CCNode::onExit();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

void ListView::clear()
{
	m_container->removeAllChildren();	
	m_nodes.clear();
}

void ListView::doMove(cocos2d::CCPoint offset)
{
	CCPoint contentPos = m_container->getPosition();
	
	float newY = contentPos.y + offset.y;
	auto tarPos = ccp(0, newY);
	m_container->setPosition(tarPos);

	float posY = getSliderPosByListPos(newY);
	m_slider->setPositionY(posY);
}

bool ListView::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	auto localPos = convertToNodeSpace(pTouch->getLocation());
	auto size = getContentSize();
	if (m_slider->boundingBox().containsPoint(localPos))
	{
		m_isSliding = true;
		return true;
	}

	CCRect rect(0, 0, size.width, size.height);
	if (!rect.containsPoint(localPos)) return false;

	m_isTouching = true;
	onTouchBegan(pTouch);
	return true;
}	

void ListView::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	CCSize contentSize = m_container->getContentSize();
	if (contentSize.height < m_size.height) return;

	auto curPos = pTouch->getLocation();
	auto prePos = pTouch->getPreviousLocation();

	auto localPos = convertToNodeSpace(prePos);
	if (m_isSliding)
	{
		float newY = m_slider->getPositionY() + ccpSub(curPos, prePos).y;
		if (newY > m_sliderBg->getPositionY() + m_sliderBg->getContentSize().height / (2 * m_sliderBg->getScale()) - m_slider->getContentSize().height*1.1f / m_sliderBg->getScale())
		{
			newY = m_sliderBg->getPositionY() + m_sliderBg->getContentSize().height / (2 * m_sliderBg->getScale()) - m_slider->getContentSize().height*1.1f / m_sliderBg->getScale();
		}
		if (newY < m_slider->getContentSize().height * 2 * m_slider->getScale())
		{
			newY = m_slider->getContentSize().height * 2 * m_slider->getScale();
		}
		m_slider->setPositionY(newY);
		float curPos = getListPosBySliderPos(newY);
		m_container->setPositionY(curPos);

		return;
	}

	doMove(ccpSub(curPos, prePos));
	onTouchMoved(pTouch);
}

void ListView::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	auto startPos = pTouch->getStartLocation();
	auto prePos = pTouch->getPreviousLocation();
	auto curPos = pTouch->getLocation();
	m_isTouching = false;

	dragback();
	onTouchEnded(pTouch);
	m_isSliding = false;
}

bool ListView::isOutOfRange()
{
	auto containerSize = m_container->getContentSize();
	auto curPos = m_container->getPosition();

	float minY = m_size.height - m_platHeight - m_spacingY;
	float maxY = ((m_nodes.size() - 1) / m_columnSize) * (m_platHeight + m_spacingY) + m_spacingY;

	return curPos.y < minY || curPos.y > maxY;
}

void ListView::dragback()
{
	auto containerSize = m_container->getContentSize();
	auto curPos = m_container->getPosition();

	if (containerSize.height <= m_size.height)
	{
		return;
	}
	float minY = m_size.height - m_platHeight - m_spacingY;
	float maxY = ((m_nodes.size() - 1) / m_columnSize) * (m_platHeight + m_spacingY) + m_spacingY;

	float newY = min(max(curPos.y, minY), maxY);
	m_container->setPositionY(newY);

	/*float posY = getSliderPosByListPos(newY);
	m_slider->setPositionY(posY);*/

}

float ListView::getSliderPosByListPos(float posY)
{
	float minY = m_size.height - m_platHeight - m_spacingY;
	float maxY = ((m_nodes.size() - 1) / m_columnSize) * (m_platHeight + m_spacingY) + m_spacingY;
	float sliderMinY = m_slider->getContentSize().height * 2 * m_slider->getScale();
	float sliderMaxY = m_sliderBg->getPositionY() + m_sliderBg->getContentSize().height / (2 * m_sliderBg->getScale()) - m_slider->getContentSize().height*1.1f / m_sliderBg->getScale();
	float curPos = sliderMinY + (sliderMaxY - sliderMinY) * (1 - (posY - minY) / (maxY - minY));
	if (curPos > sliderMaxY)
	{
		curPos = sliderMaxY;
	}
	else if (curPos < sliderMinY)
	{
		curPos = sliderMinY;
	}
	return curPos;
}

float ListView::getListPosBySliderPos(float posY)
{
	float minY = m_size.height - m_platHeight - m_spacingY;
	float maxY = ((m_nodes.size() - 1) / m_columnSize) * (m_platHeight + m_spacingY) + m_spacingY;
	float sliderMinY = m_slider->getContentSize().height * 2 * m_slider->getScale();
	float sliderMaxY = m_sliderBg->getPositionY() + m_sliderBg->getContentSize().height / (2 * m_sliderBg->getScale()) - m_slider->getContentSize().height*1.1f / m_sliderBg->getScale();
	float curPos = minY + (maxY - minY) * (1 - (posY - sliderMinY) / (sliderMaxY - sliderMinY));
	if (curPos > maxY)
	{
		curPos = maxY;
	}
	else if (curPos < minY)
	{
		curPos = minY;
	}
	return curPos;
}