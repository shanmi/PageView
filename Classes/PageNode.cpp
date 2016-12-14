#include "PageNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

PageNode::PageNode(const cocos2d::CCSize &size, const std::string &defImg, const std::string &imgUrl)
:m_size(size)
,m_defImg(defImg)
, m_imgUrl(imgUrl)
{

}

PageNode *PageNode::create(const cocos2d::CCSize &size, const std::string &defImg, const std::string &imgUrl)
{
	PageNode *node = new PageNode(size, defImg, imgUrl);
	node->init();
	node->autorelease();
	return node;
}

bool PageNode::init()
{
	setContentSize(m_size);

	imgSprite = CCSprite::create();
	imgSprite->initWithFile(m_defImg.c_str());
	addChild(imgSprite);
	imgSprite->setPosition(ccp(m_size.width / 2, m_size.height / 2));

	doPostRequest();

	return true;
}

void PageNode::doPostRequest()
{
	CCHttpRequest *request = new CCHttpRequest();
	request->setUrl(m_imgUrl.c_str());
	request->setRequestType(CCHttpRequest::kHttpGet);
	request->setResponseCallback(this, httpresponse_selector(PageNode::httpResponse));
	CCHttpClient::getInstance()->send(request);
	request->release();
}

void PageNode::httpResponse(CCHttpClient* client, CCHttpResponse* response)
{
	if (!response)
		return;
	if (!response->isSucceed()) {
		CCLOG("%s", response->getErrorBuffer());
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		//create image
		CCImage* img = new CCImage;
		img->initWithImageData((unsigned char*)buffer->data(), buffer->size());

		//create texture
		auto ptexture = new CCTexture2D();
		bool isImg = ptexture->initWithImage(img);
		img->release();
		if (!isImg)
		{
			ptexture = NULL;
		}
		else
		{
			imgSprite->initWithTexture(ptexture);
			imgSprite->setContentSize(ptexture->getContentSize());
			float scale = 1.0f;
			float asd = imgSprite->getContentSize().width;
			float scaleX = m_size.width / imgSprite->getContentSize().width;
			float scaleY = m_size.height / imgSprite->getContentSize().height;
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
			imgSprite->setScale(scale);
		}
	}
}