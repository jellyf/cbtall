#include "UserNode.h"
#include "Utils.h"
using namespace cocos2d;
bool UserNode::init()
{
	if (!Node::init()) {
		return false;
	}

	spAvatar = Sprite::createWithSpriteFrameName("avatar_default.png");
	addChild(spAvatar);

	spOnlineAvatar = Sprite::create();
	addChild(spOnlineAvatar);

	Sprite* bg = Sprite::createWithSpriteFrameName("bg_avatar.png");
	bg->setPosition(0, -3);
	addChild(bg);

	lbName = Label::createWithTTF("Stormus", "fonts/myriad.ttf", 30);
	lbName->setHorizontalAlignment(TextHAlignment::CENTER);
	lbName->setPosition(0, 67);
	lbName->setHeight(25);
	addChild(lbName);

	lbMoney = Label::createWithTTF("100,000,000", "fonts/myriad.ttf", 30);
	lbMoney->setPosition(0, -72);
	lbMoney->setColor(Color3B::YELLOW);
	addChild(lbMoney);

	return true;
}

int UserNode::getAlpha()
{
	return lbName->getOpacity();
}

void UserNode::setAlpha(int alpha)
{
	Vector<Node*> children = this->getChildren();
	for (Node* n : children) {
		n->setOpacity(alpha);
	}
}

void UserNode::setPlayerName(std::string name)
{
	fullName = name;
	if (name.length() > 10) {
		name = name.substr(0, 10);
	}
	lbName->setString(name);
	while (lbName->getContentSize().width > 110) {
		name = name.substr(0, name.length() - 1);
		lbName->setString(name);
	}
}

void UserNode::setPlayerMoney(double money)
{
	mMoney = money;
	lbMoney->setString(Utils::getSingleton().formatMoneyWithComma(money));
}

void UserNode::setSfsId(long id)
{
	sfsId = id;
}

void UserNode::setAvatarUrl(std::string avarUrl)
{
	if (avarUrl.length() > 0) {
		Utils::getSingleton().LoadTextureFromURL(avarUrl, [=](Texture2D* texture) {
			spOnlineAvatar->initWithTexture(texture);
			Size fsize = spAvatar->getContentSize();
			Size spsize = spOnlineAvatar->getContentSize();
			float scaleX = fsize.width / spsize.width;
			float scaleY = fsize.height / spsize.height;
			spOnlineAvatar->setScale((scaleX < scaleY ? scaleY : scaleX) + .05f);
		});
	}
}

std::string UserNode::getPlayerName()
{
	return fullName;
}

double UserNode::getPlayerMoney()
{
	return mMoney;
}

long UserNode::getSfsId()
{
	return sfsId;
}

