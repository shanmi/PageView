#ifndef __PAGE_NODE_H__
#define __PAGE_NODE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class PageNode
	:public cocos2d::CCNode
{
public:
	static PageNode *create(const cocos2d::CCSize &size, const std::string &defImg, const std::string &imgUrl);
	~PageNode(){}
	virtual bool init();

public:
	void doPostRequest();
	void httpResponse(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);

private:
	PageNode(const cocos2d::CCSize &size, const std::string &defImg, const std::string &imgUrl);

private:
	cocos2d::CCSprite *imgSprite;
	cocos2d::CCSize m_size;
	std::string m_defImg;
	std::string m_imgUrl;
};
#endif