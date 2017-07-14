#pragma once
#include "cocos2d.h"

class LoadScene : public cocos2d::Scene
{
public:
	CREATE_FUNC(LoadScene);
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();
	virtual void onIAPProductReady();
	virtual void onHttpResponse(int tag, std::string content);
	virtual void onHttpResponseFailed();

private:
	void autoScaleNode(cocos2d::Node * node);
	void addToActionQueue(std::function<void()> func);
	void initActionQueue();
	void startActionQueue();
	void continueActionQueue();
	void completeCurrentAction();
	void finishActionQueue();
	void updateLoadingView();
	int getCompletedPercentage();

	cocos2d::ProgressTimer* progressTimer;
	cocos2d::Vec2 scaleScene;
	cocos2d::Size winSize;
	std::string textureHost;

	std::vector<std::function<void()>> actionQueue;
	int currentActionIndex;
	int completedActionNumb;
};

