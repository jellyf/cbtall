#pragma once
#include "cocos2d.h"

class LoadScene : public cocos2d::Scene
{
public:
	LoadScene();
	~LoadScene();
	CREATE_FUNC(LoadScene);
	virtual bool init();
	virtual void onEnter();
	virtual void update(float delta);
};

