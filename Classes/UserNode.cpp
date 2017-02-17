#pragma once
#include "UserNode.h"
#include "Utils.h"
using namespace cocos2d;
bool UserNode::init()
{
	if (!Node::init()) {
		return false;
	}

	Sprite* bg = Sprite::create("board/bg_avatar.png");
	addChild(bg);

	Sprite* avatar = Sprite::create("board/avatar_default.png");
	addChild(avatar);

	Sprite* mask = Sprite::create("board/mask_avar.png");
	addChild(mask);

	lbName = Label::createWithTTF("Stormus", "fonts/arial.ttf", 20);
	lbName->setHorizontalAlignment(TextHAlignment::CENTER);
	lbName->setPosition(0, 65);
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
		name = name = name.substr(0, name.length() - 1);
		lbName->setString(name);
	}
}

void UserNode::setPlayerMoney(double money)
{
	mMoney = money;
	lbMoney->setString(Utils::getSingleton().formatMoneyWithComma(money));
}

std::string UserNode::getPlayerName()
{
	return fullName;
}

double UserNode::getPlayerMoney()
{
	return mMoney;
}

