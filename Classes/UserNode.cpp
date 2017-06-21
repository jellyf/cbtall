#pragma once
#include "UserNode.h"
#include "Utils.h"
using namespace cocos2d;
bool UserNode::init()
{
	if (!Node::init()) {
		return false;
	}

	Sprite* bg = Sprite::createWithSpriteFrameName("bg_avatar.png");
	bg->setPosition(0, -3);
	addChild(bg);

	Sprite* avatar = Sprite::createWithSpriteFrameName("avatar_default.png");
	addChild(avatar);

	/*Sprite* mask = Sprite::createWithSpriteFrameName("mask_avar.png");
	addChild(mask);*/

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

