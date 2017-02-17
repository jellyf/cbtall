#include "AppDelegate.h"
#include "LoginScene.h"
#include "GameScene.h"
#include "MainScene.h"
#include "LobbyScene.h"
#include "SFSRequest.h"
#include "SFSResponse.h"
#include "SFSConnector.h"
#include "EventHandler.h"
#include "Utils.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(800, 500);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 300);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(800, 500);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1120, 700);

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("ChanBachThu", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("ChanBachThu");
#endif
        director->setOpenGLView(glview);
    }
    //director->setDisplayStats(true);
    director->setAnimationInterval(1.0f / 60);
    glview->setDesignResolutionSize(largeResolutionSize.width, largeResolutionSize.height, ResolutionPolicy::NO_BORDER);

    register_all_packages();

	SFSConnector* connector = new SFSConnector();
	SFSRequest* request = new SFSRequest();
	SFSResponse* response = new SFSResponse();
	EventHandler* handler = new EventHandler();
	Utils* utils = new Utils();

	auto scene = LoginScene::create();
	//auto scene = GameScene::create();
	//auto scene = MainScene::create();
	//auto scene = LobbyScene::create();
	utils->currentScene = scene;
    director->runWithScene(scene);
    
    //CCLOG("getVisibleSize: %f %f", director->getVisibleSize().width, director->getVisibleSize().height);
    //CCLOG("getWinSize: %f %f", director->getWinSize().width, director->getWinSize().height);
    //CCLOG("getWinSizeInPixels: %f %f", director->getWinSizeInPixels().width, director->getWinSizeInPixels().height);
    //CCLOG("getVisibleSize: %f %f", glview->getVisibleSize().width, glview->getVisibleSize().height);
    //CCLOG("getViewPortRect: %f %f", glview->getViewPortRect().size.width, glview->getViewPortRect().size.height);
    //CCLOG("getVisibleRect: %f %f", glview->getVisibleRect().size.width, glview->getVisibleRect().size.height);
    //CCLOG("getFrameSize: %f %f", glview->getFrameSize().width, glview->getFrameSize().height);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
	if (EventHandler::getSingleton().onApplicationDidEnterBackground != NULL) {
		EventHandler::getSingleton().onApplicationDidEnterBackground();
	}
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
	if (EventHandler::getSingleton().onApplicationWillEnterForeground != NULL) {
		EventHandler::getSingleton().onApplicationWillEnterForeground();
	}
}
