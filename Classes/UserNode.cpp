#include "UserNode.h"
#include "Utils.h"
using namespace cocos2d;
bool UserNode::init()
{
	if (!Node::init()) {
		return false;
	}

	Sprite* bg = Sprite::createWithSpriteFrameName("bg_avatar.png");
	addChild(bg);

	spAvatar = Sprite::createWithSpriteFrameName("avatar_default.png");
	spAvatar->setPositionY(-3);
	addChild(spAvatar);

	spOnlineAvatar = Sprite::create();
	spOnlineAvatar->setPositionY(-3);
	addChild(spOnlineAvatar);

	Sprite* mask = Sprite::createWithSpriteFrameName("mask_avar.png");
	mask->setPosition(1, -3);
	addChild(mask);

	lbName = Label::createWithTTF("Stormus", "fonts/arial.ttf", 20);
	lbName->setHorizontalAlignment(TextHAlignment::CENTER);
	lbName->setPosition(0, 62);
	lbName->setHeight(25);
	addChild(lbName);

	lbMoney = Label::createWithTTF("100,000", "fonts/arial.ttf", 20);
	lbMoney->setPosition(0, -68);
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
			spOnlineAvatar->setScale((scaleX < scaleY ? scaleY : scaleX));
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

