#include "LoadScene.h"
#include "SFSRequest.h"
#include "Constant.h"
#include "EventHandler.h"
#include "Utils.h"
#include "json/document.h"

using namespace cocos2d;
using namespace std;

bool LoadScene::init()
{
	if (!Scene::init()) {
		return false;
	}
	textureHost = "http://115.84.179.242/main_kinhtuchi/";

	return true;
}

void LoadScene::onEnter()
{
	Scene::onEnter();

	Layer* mLayer = Layer::create();
	addChild(mLayer, 10);

	winSize = Director::getInstance()->getWinSize();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	visibleSize.width = std::ceil(visibleSize.width);
	visibleSize.height = std::ceil(visibleSize.height);

	scaleScene = Vec2(1, 1);
	if (visibleSize.width < 1120) {
		scaleScene.x = visibleSize.width / 1120;
		mLayer->setScaleX(scaleScene.x);
	} else if (visibleSize.height < 700) {
		scaleScene.y = visibleSize.height / 700;
		mLayer->setScaleY(scaleScene.y);
	}

	Sprite* bg = Sprite::create("imgs/loading_bg.jpg");
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	mLayer->addChild(bg);

	Sprite* icon = Sprite::create("imgs/loading_icon.png");
	icon->setPosition(winSize.width / 2, winSize.height / 2 + 50);
	mLayer->addChild(icon);
	autoScaleNode(icon);

	Sprite* processBg = Sprite::create("imgs/loading_process_bg.png");
	processBg->setPosition(winSize.width / 2, winSize.height / 4 - 60);
	mLayer->addChild(processBg);

	progressTimer = ProgressTimer::create(Sprite::create("imgs/loading_process.png"));
	progressTimer->setType(ProgressTimer::Type::BAR);
	progressTimer->setMidpoint(Vec2(0, 0));
	progressTimer->setBarChangeRate(Vec2(1, 0));
	progressTimer->setPercentage(0);
	progressTimer->setPosition(processBg->getPosition());
	mLayer->addChild(progressTimer);

	registerEventListenner();
	initActionQueue();
	startActionQueue();
}

void LoadScene::onExit()
{
	Scene::onExit();
	unregisterEventListenner();
}

void LoadScene::registerEventListenner()
{
	//EventHandler::getSingleton().onIAPProductReady = bind(&LoadScene::onIAPProductReady, this);
	SFSRequest::getSingleton().onHttpResponseFailed = std::bind(&LoadScene::onHttpResponseFailed, this);
	SFSRequest::getSingleton().onHttpResponse = std::bind(&LoadScene::onHttpResponse, this, std::placeholders::_1, std::placeholders::_2);
}

void LoadScene::unregisterEventListenner()
{
	//EventHandler::getSingleton().onIAPProductReady = NULL;
	SFSRequest::getSingleton().onHttpResponse = NULL;
	SFSRequest::getSingleton().onHttpResponseFailed = NULL;
}

void LoadScene::autoScaleNode(cocos2d::Node * node)
{
	if (scaleScene.x < 1) {
		node->setScaleY(node->getScaleY() * scaleScene.x);
	} else if (scaleScene.y < 1) {
		node->setScaleX(node->getScaleX() * scaleScene.y);
	}
}

void LoadScene::addToActionQueue(std::function<void()> func)
{
	actionQueue.push_back(func);
}

void LoadScene::initActionQueue()
{
	/*addToActionQueue([=]() {
		Utils::getSingleton().queryIAPProduct();
	});*/
	addToActionQueue([=]() {
		Director::getInstance()->getTextureCache()->addImageAsync("imgs/login.png", [=](Texture2D* texture) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("imgs/login.plist");
			completeCurrentAction();
		});
	});
	addToActionQueue([=]() {
		Director::getInstance()->getTextureCache()->addImageAsync("imgs/buttons.png", [=](Texture2D* texture) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("imgs/buttons.plist");
			completeCurrentAction();
		});
	});
	addToActionQueue([=]() {
		Director::getInstance()->getTextureCache()->addImageAsync("imgs/game.png", [=](Texture2D* texture) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("imgs/game.plist");
			completeCurrentAction();
		});
	});
	addToActionQueue([=]() {
		SFSRequest::getSingleton().RequestHttpGet("http://ip171.api1chan.info/configcv.txt", constant::TAG_HTTP_GAME_CONFIG);
		//SFSRequest::getSingleton().RequestHttpGet("http://kinhtuchi.com/configchanktc.txt", constant::TAG_HTTP_GAME_CONFIG);
		//SFSRequest::getSingleton().RequestHttpGet("http://115.84.179.242/configchanktc.txt", constant::TAG_HTTP_GAME_CONFIG);
		//SFSRequest::getSingleton().RequestHttpGet("http://125.212.207.71/config/configChan.txt", constant::TAG_HTTP_GAME_CONFIG);
	});
	addToActionQueue([=]() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		SFSRequest::getSingleton().RequestHttpGet(textureHost + "vi2.xml", constant::TAG_HTTP_VILANG);
#else
		string content = FileUtils::getInstance()->getStringFromFile("lang/vi2.xml");
		onHttpResponse(constant::TAG_HTTP_VILANG, content);
#endif
	});
	addToActionQueue([=]() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		string str1 = FileUtils::getInstance()->getStringFromFile("menu1.plist");
		Utils::getSingleton().LoadTextureFromURL(textureHost + "menu1.png", [=](Texture2D* texture1) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(str1, texture1);
			completeCurrentAction();
		});
#else
		Director::getInstance()->getTextureCache()->addImageAsync("menu1.png", [=](Texture2D* texture) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("menu1.plist");
			completeCurrentAction();
		});
#endif
	});
	addToActionQueue([=]() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		string str2 = FileUtils::getInstance()->getStringFromFile("menu2.plist");
		Utils::getSingleton().LoadTextureFromURL(textureHost + "menu2.png", [=](Texture2D* texture2) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(str2, texture2);
			completeCurrentAction();
		});
#else
		Director::getInstance()->getTextureCache()->addImageAsync("menu2.png", [=](Texture2D* texture) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("menu2.plist");
			completeCurrentAction();
		});
#endif
	});
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	addToActionQueue([=]() {
		Utils::getSingleton().LoadTextureFromURL(textureHost + "hu.png", [=](Texture2D* texture4) {
			completeCurrentAction();
		});
	});
	addToActionQueue([=]() {
		Utils::getSingleton().LoadTextureFromURL(textureHost + "as.png", [=](Texture2D* texture4) {
			completeCurrentAction();
		});
	});
#endif
}

void LoadScene::startActionQueue()
{
	currentActionIndex = -1;
	completedActionNumb = 0;
	continueActionQueue();
}

void LoadScene::continueActionQueue()
{
	currentActionIndex++;
	actionQueue[currentActionIndex]();
}

void LoadScene::completeCurrentAction()
{
	completedActionNumb++;
	if (currentActionIndex < actionQueue.size() - 1) {
		updateLoadingView();
		continueActionQueue();
	} else {
		finishActionQueue();
	}
}

void LoadScene::finishActionQueue()
{
	currentActionIndex = actionQueue.size();
	completedActionNumb = actionQueue.size();
	updateLoadingView();

	DelayTime* delay = DelayTime::create(.3f);
	CallFunc* func = CallFunc::create([=]() {
		Utils::getSingleton().goToLoginScene();
	});
	runAction(Sequence::createWithTwoActions(delay, func));
}

void LoadScene::updateLoadingView()
{
	int currentViewPercentage = progressTimer->getPercentage();
	int completedPercentage = getCompletedPercentage();
	if (completedPercentage > currentViewPercentage) {
		progressTimer->runAction(ProgressTo::create(.2f, completedPercentage));
	}
}

int LoadScene::getCompletedPercentage()
{
	if (completedActionNumb == 0) return 0;
	if (completedActionNumb >= actionQueue.size()) return 100;
	return completedActionNumb * 100 / actionQueue.size();
}

void LoadScene::onIAPProductReady()
{
	//completeCurrentAction();
}

void LoadScene::onHttpResponseFailed()
{
	CCLOG("LoadScene::onHttpResponseFailed");
}

void LoadScene::onHttpResponse(int tag, std::string content)
{
	if (tag == constant::TAG_HTTP_VILANG) {
		Utils::getSingleton().addViLangFromData(content);
		completeCurrentAction();
		return;
	}
	if (tag != constant::TAG_HTTP_GAME_CONFIG) return;
	if (content.length() == 0) {
		onHttpResponseFailed();
		return;
	}
	rapidjson::Document d;
	GameConfig config;
	d.Parse<0>(content.c_str());

	/*vector<string> keys = { "payment", "paymentIOS", "name", "host", "port", "websocket", "version", "versionIOS", "ip_rs", "phone",
	"smsVT", "smsVNPVMS", "smsKH", "smsMK", "fb", "a", "i", "updatenow", "inapp", "invite" };
	for (string k : keys) {
	if (d.FindMember(k.c_str()) == d.MemberEnd()) {
	onHttpResponseFailed();
	return;
	}
	}*/

	config.zone = d["name"].GetString();
	config.host = d["host"].GetString();
	config.port = d["port"].GetInt();
	config.websocket = d["websocket"].GetInt();
	config.version = d["version"].GetInt();
	config.versionIOS = d["versionIOS"].GetInt();
	config.ip_rs = d["ip_rs"].GetString();
	config.phone = d["phone"].GetString();
	config.smsKHVT = d["SMSKHVTT"].GetString();
	config.smsKHVMS = d["SMSKHMOBI"].GetString();
	config.smsKHVNP = d["SMSKHVINA"].GetString();

	if (d.FindMember("payment") != d.MemberEnd()) {
		config.pmE = d["payment"].GetBool();
	} else config.pmE = false;
	if (d.FindMember("paymentIOS") != d.MemberEnd()) {
		config.pmEIOS = d["paymentIOS"].GetBool();
	} else config.pmEIOS = false;
	if (d.FindMember("updatenow") != d.MemberEnd()) {
		config.canUpdate = d["updatenow"].GetBool();
	} else config.canUpdate = false;
	if (d.FindMember("invite") != d.MemberEnd()) {
		config.invite = d["invite"].GetBool();
	} else config.invite = false;
	if (d.FindMember("fb") != d.MemberEnd()) {
		config.linkFb = d["fb"].GetString();
	} else config.linkFb = "";
	if (d.FindMember("a") != d.MemberEnd()) {
		config.linkAndroid = d["a"].GetString();
	} else config.linkAndroid = "";
	if (d.FindMember("i") != d.MemberEnd()) {
		config.linkIOS = d["i"].GetString();
	} else config.linkIOS = "";
	if (d.FindMember("inapp") != d.MemberEnd()) {
		config.inapp = d["inapp"].GetString();
	} else config.inapp = "";

	config.linkAndroid = "https://play.google.com/store/apps/details?id=" + config.linkAndroid;
	config.smsMKVT = Utils::getSingleton().replaceString(config.smsKHVT, "KHuid", "MKuid");
	config.smsMKVNP = Utils::getSingleton().replaceString(config.smsKHVNP, "KHuid", "MKuid");
	config.smsMKVMS = Utils::getSingleton().replaceString(config.smsKHVMS, "KHuid", "MKuid");

	int verValue = getVersionValue();
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	config.pmEIOS &= verValue < config.versionIOS;
	config.canUpdate &= verValue < config.versionIOS - 1;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	config.pmE &= verValue < config.version;
	config.canUpdate &= verValue < config.version - 1;
#else
	config.pmE = true;
	config.canUpdate = false;
#endif

	Utils::getSingleton().gameConfig = config;
	Utils::getSingleton().queryIAPProduct();

	if (Utils::getSingleton().ispmE()) {
		completeCurrentAction();
	} else {
		finishActionQueue();
	}
}

int LoadScene::getVersionValue()
{
	string verstr = Application::getInstance()->getVersion();

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	std::vector<std::string> numbs;
	Utils::getSingleton().split(verstr, '.', numbs);
	int nver = atoi(numbs[0].c_str()) * 100 + atoi(numbs[1].c_str()) * 10 + atoi(numbs[2].c_str());
#else
	int i = verstr.find_last_of('.') + 1;
	verstr = verstr.substr(i, verstr.length() - i);
	int nver = atoi(verstr.c_str());
#endif

	return nver;
}