#pragma once
#include "cocos2d.h"

class UserNode : public cocos2d::Node
{
public:
	CREATE_FUNC(UserNode);
	virtual bool init();

	int getAlpha();
	void setAlpha(int alpha);
	void setPlayerName(std::string name);
	void setPlayerMoney(double money);
	std::string getPlayerName();
	double getPlayerMoney();
private:
	double mMoney;
	std::string fullName;
	cocos2d::Label* lbName;
	cocos2d::Label* lbMoney;
};

