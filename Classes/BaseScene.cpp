#include "BaseScene.h"
#include "EventHandler.h"
#include "SFSRequest.h"
#include "Utils.h"
#include "Constant.h"
#include "AudioEngine.h"
#include "SFSResponse.h"
#include "Tracker.h"
#include "SFSGEvent.h"
#include "md5.h"

using namespace std;
using namespace cocos2d;

BaseScene::BaseScene()
{
}

BaseScene::~BaseScene()
{
}

bool BaseScene::init()
{
	if (!Scene::init()) {
		return false;
	}
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(BaseScene::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(BaseScene::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(BaseScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	setIsTouch(true);

	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
		{
			onKeyBack();
		} 
		else if (keyCode == EventKeyboard::KeyCode::KEY_HOME)
		{
			onKeyHome();
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	tmps = Vector<Node*>();
	tmpIndex = 0;
	return true;
}

void BaseScene::onInit()
{
}

void BaseScene::onEnter()
{
	Scene::onEnter();
	bool ispmE = Utils::getSingleton().ispmE();
	isPopupReady = Utils::getSingleton().downloadedPlistTexture >= 2 || !Utils::getSingleton().ispmE();
	myRealMoney = Utils::getSingleton().userDataMe.MoneyReal;

	mLayer = Layer::create();
	addChild(mLayer, 10);

	winSize = Director::sharedDirector()->getWinSize();
	auto visibleSize = Director::sharedDirector()->getVisibleSize();
	visibleSize.width = std::ceil(visibleSize.width);
	visibleSize.height = std::ceil(visibleSize.height);
	
	scaleScene = Vec2(1, 1);
	if (visibleSize.width < winSize.width) {
		scaleScene.x = visibleSize.width / winSize.width;
		mLayer->setScaleX(scaleScene.x);
	} else if (visibleSize.height < winSize.height) {
		scaleScene.y = visibleSize.height / winSize.height;
		mLayer->setScaleY(scaleScene.y);
	}

	splash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	splash->setContentSize(Size(1500, 1000));
	splash->setPosition(winSize.width / 2, winSize.height / 2);
	splash->setColor(Color3B::BLACK);
	splash->setOpacity(150);
	splash->setVisible(false);
	mLayer->addChild(splash);

	Node* nodeWaiting = Node::create();
	nodeWaiting->setPosition(winSize.width / 2, winSize.height / 2);
	nodeWaiting->setVisible(false);
	mLayer->addChild(nodeWaiting, constant::ZORDER_POPUP);
	autoScaleNode(nodeWaiting);

	spWaiting = Sprite::createWithSpriteFrameName("loadingicon.png");
	nodeWaiting->addChild(spWaiting);

	RotateBy* rotate = RotateBy::create(2, 360);
	spWaiting->runAction(RepeatForever::create(rotate));
	spWaiting->pauseSchedulerAndActions();

	spNetwork = Sprite::createWithSpriteFrameName("wifi0.png");
	spNetwork->setPosition(winSize.width - 260, winSize.height - 55);
	mLayer->addChild(spNetwork, constant::GAME_ZORDER_SPLASH - 1);
	autoScaleNode(spNetwork);

	lbNetwork = Label::create("0ms", "fonts/myriad.ttf", 18);
	lbNetwork->setColor(Color3B::GREEN);
	lbNetwork->setAnchorPoint(Vec2(1, 0));
	lbNetwork->setPosition(1115, 3);
	lbNetwork->setVisible(false);
	mLayer->addChild(lbNetwork, constant::GAME_ZORDER_SPLASH - 1);
	autoScaleNode(lbNetwork);

	DelayTime* delayCheckPing = DelayTime::create(3.5f);
	CallFunc* funcCheckPing = CallFunc::create([=]() {
		if (pingId == spNetwork->getTag()) {
			onPingPong(2000);
		}
		spNetwork->setTag(pingId);

		/*if (pingId == lbNetwork->getTag()) {
			string str = lbNetwork->getString();
			long numb = atol(str.substr(0, str.length() - 2).c_str());
			onPingPong(numb + 1100);
		}
		lbNetwork->setTag(pingId);*/
	});
	Sequence* actionCheckPing = Sequence::createWithTwoActions(delayCheckPing, funcCheckPing);
	spNetwork->runAction(RepeatForever::create(actionCheckPing));
	//lbNetwork->runAction(RepeatForever::create(actionCheckPing));

	onInit();
	registerEventListenner();
	Utils::getSingleton().onInitSceneCompleted();
	SFSResponse::getSingleton().RunCachedResponses();
	scheduleUpdate();
}

void BaseScene::onExit()
{
	Scene::onExit();
	unscheduleUpdate();
	unregisterEventListenner();
	spNetwork->stopAllActions();
}

void BaseScene::update(float delta)
{
	Scene::update(delta);
	SFSGEvent::getSingleton().ProcessEvents();
}

void BaseScene::registerEventListenner()
{
	EventHandler::getSingleton().onApplicationDidEnterBackground = std::bind(&BaseScene::onApplicationDidEnterBackground, this);
	EventHandler::getSingleton().onApplicationWillEnterForeground = std::bind(&BaseScene::onApplicationWillEnterForeground, this);
	EventHandler::getSingleton().onPingPong = std::bind(&BaseScene::onPingPong, this, std::placeholders::_1);
	EventHandler::getSingleton().onConnected = bind(&BaseScene::onConnected, this);
	EventHandler::getSingleton().onConnectionFailed = std::bind(&BaseScene::onConnectionFailed, this);
	EventHandler::getSingleton().onLoginZone = bind(&BaseScene::onLoginZone, this);
	EventHandler::getSingleton().onLoginZoneError = bind(&BaseScene::onLoginZoneError, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onUserDataMeSFSResponse = std::bind(&BaseScene::onUserDataMeResponse, this);
	EventHandler::getSingleton().onRankDataSFSResponse = std::bind(&BaseScene::onRankDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onRankWinDataSFSResponse = std::bind(&BaseScene::onRankWinDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onListEventDataSFSResponse = std::bind(&BaseScene::onListEventDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onPlayLogDataSFSResponse = bind(&BaseScene::onPlayLogDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onErrorSFSResponse = bind(&BaseScene::onErrorResponse, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onCofferMoneySFSResponse = bind(&BaseScene::onCofferMoneyResponse, this, placeholders::_1);
	EventHandler::getSingleton().onCofferHistorySFSResponse = bind(&BaseScene::onCofferHistoryResponse, this, placeholders::_1);
	EventHandler::getSingleton().onDownloadedPlistTexture = bind(&BaseScene::onDownloadedPlistTexture, this, placeholders::_1);

	SFSRequest::getSingleton().onHttpResponseFailed = std::bind(&BaseScene::onHttpResponseFailed, this);
	SFSRequest::getSingleton().onHttpResponse = std::bind(&BaseScene::onHttpResponse, this, std::placeholders::_1, std::placeholders::_2);
}

void BaseScene::unregisterEventListenner()
{
	EventHandler::getSingleton().onApplicationDidEnterBackground = NULL;
	EventHandler::getSingleton().onApplicationWillEnterForeground = NULL;
	EventHandler::getSingleton().onPingPong = NULL;
	EventHandler::getSingleton().onConnectionFailed = NULL;
	EventHandler::getSingleton().onUserDataMeSFSResponse = NULL;
	EventHandler::getSingleton().onRankDataSFSResponse = NULL;
	EventHandler::getSingleton().onRankWinDataSFSResponse = NULL;
	EventHandler::getSingleton().onListEventDataSFSResponse = NULL;
	EventHandler::getSingleton().onPlayLogDataSFSResponse = NULL;
	EventHandler::getSingleton().onErrorSFSResponse = NULL;
	EventHandler::getSingleton().onCofferMoneySFSResponse = NULL;
	EventHandler::getSingleton().onCofferHistorySFSResponse = NULL;
	EventHandler::getSingleton().onDownloadedPlistTexture = NULL;

	SFSRequest::getSingleton().onHttpResponse = NULL;
	SFSRequest::getSingleton().onHttpResponseFailed = NULL;
}

bool BaseScene::onTouchBegan(Touch * touch, Event * _event)
{
	/*Point pos = touch->getLocation();
	pos = mLayer->convertToNodeSpace(pos);

	tmpIndex = -1;
	int i = 0;
	for (Node* n : tmps) {
		if (n->isVisible() && n->getBoundingBox().containsPoint(pos)) {
			tmpIndex = i;
			return true;
		}
		i++;
	}*/
	return false;
}

void BaseScene::onTouchMoved(Touch * touch, Event * _event)
{
	/*Point pos = touch->getLocation();
	pos = mLayer->convertToNodeSpace(pos);

	if (tmpIndex > -1)
		tmps.at(tmpIndex)->setPosition(pos);*/
}

void BaseScene::onTouchEnded(Touch * touch, Event * _event)
{
	/*if (tmpIndex > -1)
		CCLOG("%d %d, %d", tmpIndex, (int)tmps.at(tmpIndex)->getPosition().x, (int)tmps.at(tmpIndex)->getPosition().y);*/
}

void BaseScene::onApplicationDidEnterBackground()
{
	spNetwork->pause();
	lbNetwork->pause();
}

void BaseScene::onApplicationWillEnterForeground()
{
	spNetwork->resume();
	lbNetwork->resume();
}

void BaseScene::showPopupNotice(std::string msg, std::function<void()> func, bool showBtnClose, int timeToHide)
{
	Node* popupNotice = getPopupNotice();
	showPopup(popupNotice);
	Label* lbcontent = (Label*)popupNotice->getChildByName("lbcontent");
	lbcontent->setString(msg);
	ui::Button* btnClose = (ui::Button*)popupNotice->getChildByName("btnclose");
	btnClose->setVisible(showBtnClose);
	ui::Button* btnSubmit = (ui::Button*)popupNotice->getChildByName("btnsubmit");
	addTouchEventListener(btnSubmit, [=]() {
		popupNotice->stopAllActions();
		func();
		hidePopup(popupNotice);
	}, false);

	if (timeToHide > 0) {
		DelayTime* delay = DelayTime::create(15);
		CallFunc* funcHide = CallFunc::create([=]() {
			hidePopup(popupNotice);
		});
		popupNotice->runAction(Sequence::createWithTwoActions(delay, funcHide));
	}
}

void BaseScene::showSplash()
{
	splash->setVisible(true);
}

void BaseScene::showWebView(std::string url)
{
	if (nodeWebview == nullptr) {
		initWebView();
	}
	nodeWebview->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	experimental::ui::WebView* webView = (experimental::ui::WebView*)nodeWebview->getChildByName("webview");
	webView->loadURL(url);
	webView->setVisible(true);
#endif
}

void BaseScene::showWaiting(int time)
{
	isWaiting = true;
	showPopup(spWaiting->getParent(), false);
	spWaiting->resumeSchedulerAndActions();
	spWaiting->getParent()->stopAllActions();

	DelayTime* delay = DelayTime::create(time);
	CallFunc* func = CallFunc::create([=]() {
		if (isWaiting) {
			hideWaiting();
			showPopupNotice(Utils::getSingleton().getStringForKey("khong_nhan_duoc_phan_hoi_tu_server"), [=]() {
				if (SFSRequest::getSingleton().IsConnected()) {
					isBackToLogin = true;
					SFSRequest::getSingleton().Disconnect();
				} else {
					Utils::getSingleton().goToLoginScene();
				}
			}, true);
		}
	});
	spWaiting->getParent()->runAction(Sequence::create(delay, func, nullptr));
}

void BaseScene::showPopup(cocos2d::Node * popup, bool runEffect)
{
	if (popup->isVisible()) return;
	if (runEffect) {
		runEffectShowPopup(popup);
	}
	popup->setVisible(true);
	popups.push_back(popup);
	if (popups.size() == 1) {
		showSplash();
		popup->setLocalZOrder(constant::ZORDER_POPUP);
		setSplashZOrder(popup->getLocalZOrder() - 1);
	} else {
		popup->setLocalZOrder(popups[popups.size() - 2]->getLocalZOrder() + 2);
		setSplashZOrder(popup->getLocalZOrder() - 1);
	}
}

void BaseScene::setDisplayName(std::string name)
{
	lbName->setString(name);
}

void BaseScene::runEffectHidePopup(cocos2d::Node * popup)
{
	ScaleTo* scaleTo = ScaleTo::create(.1f, .3f);
	CallFunc* func = CallFunc::create([=]() {
		popup->setVisible(false);
	});
	popup->stopAllActions();
	popup->runAction(Sequence::create(scaleTo, func, nullptr));
}

void BaseScene::runEffectShowPopup(cocos2d::Node * popup)
{
	Vec2 scale1 = getScaleSmoothly(1.1f);
	Vec2 scale2 = getScaleSmoothly(1);
	ScaleTo* scaleTo1 = ScaleTo::create(.2f, scale1.x, scale1.y);
	ScaleTo* scaleTo2 = ScaleTo::create(.1f, scale2.x, scale2.y);
	popup->setScale(.3f);
	popup->stopAllActions();
	popup->runAction(Sequence::createWithTwoActions(scaleTo1, scaleTo2));
}

void BaseScene::runEventView(std::vector<EventData> list, int currentPosX)
{
	/*int numb = rand() % 5;
	for (int i = 0; i < numb; i++) {
	EventData data;
	data.Id = rand() % 100;
	data.Content = "This is a content " + to_string(data.Id);
	list.push_back(data);
	}*/

	if (eventView == nullptr || list.size() == 0) return;
	Node* scroll = eventView->getChildByName("scroll");
	Node* nodelb = scroll->getChildByName("nodelb");
	if (eventView->isVisible()) {
		nodelb->stopAllActions();
	} else {
		eventView->setVisible(true);
	}
	if (currentPosX > scroll->getContentSize().width) {
		currentPosX = scroll->getContentSize().width;
	}
	int x = 0;
	int distance = 100;
	for (int i = 0; i < list.size(); i++) {
		Label* lb = (Label*)nodelb->getChildByTag(i);
		if (lb == nullptr) {
			lb = Label::create("", "fonts/myriad.ttf", 21);
			lb->setAnchorPoint(Vec2(0, .5f));
			lb->setTag(i);
			nodelb->addChild(lb);
			autoScaleNode(lb);
		} else {
			lb->setVisible(true);
		}
		lb->setString(list[i].Content);
		lb->setPosition(x, 15);
		x += lb->getContentSize().width + distance;
	}
	int count = nodelb->getChildrenCount();
	for (int i = list.size(); i < count; i++) {
		nodelb->getChildByTag(i)->setVisible(false);
	}

	nodelb->setPosition(currentPosX, 0);
	MoveBy* moveBy = MoveBy::create(.5f, Vec2(-50, 0));
	CallFunc* func = CallFunc::create([=]() {
		if (nodelb->getPositionX() <= -x + distance) {
			if (eventView->getTag() == 1) {
				eventView->setTag(0);
				nodelb->stopAllActions();
				eventView->setVisible(false);
				Utils::getSingleton().isRunningEvent = false;
				Utils::getSingleton().currentEventPosX = constant::EVENT_START_POSX;
			} else {
				eventView->setTag(1);
				runEventView(Utils::getSingleton().events);
			}
		}
		Utils::getSingleton().currentEventPosX = nodelb->getPositionX();
	});
	Sequence* action = Sequence::create(moveBy, func, nullptr);
	nodelb->runAction(RepeatForever::create(action));
	Utils::getSingleton().isRunningEvent = true;
}

void BaseScene::showPopupRank(int type)
{
	if (popupRank == nullptr) {
		initPopupRank();
	}
	if (!popupRank->isVisible()) {
		showPopup(popupRank);
	}
	ui::Button* btn = (ui::Button*)popupRank->getChildByTag(type);
	ui::Button* btn1 = (ui::Button*)popupRank->getChildByTag(popupRank->getTag());
	btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
	btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
	btn1->setTitleColor(Color3B::WHITE);
	btn->setTitleColor(Color3B::YELLOW);
	popupRank->setTag(type);

	if (type == 2) {
		showPopupRankWin();
		return;
	}

	ui::ScrollView* scroll = (ui::ScrollView*)popupRank->getChildByName("scroll");
	ui::ScrollView* scrollWin = (ui::ScrollView*)popupRank->getChildByName("scrollwin");
	scroll->setVisible(true);
	scrollWin->setVisible(false);
	int height = listRanks[type].size() * 70 + 80;
	int width = scroll->getContentSize().width;
	scroll->setInnerContainerSize(Size(width, height));
	for (int i = 0; i < listRanks[type].size(); i++) {
		Label *lb1, *lb2, *lb3;
		Node* node = scroll->getChildByTag(i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scroll->getContentSize().width / 2, height - 40 - i * 70);
			node->setTag(i);
			scroll->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
			nbg->setContentSize(Size(width, 65));
			node->addChild(nbg);

			lb1 = Label::create(to_string(i + 1), i < 3 ? "fonts/myriadb.ttf" : "fonts/myriad.ttf", 30);
			//lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setPosition(-width / 2 + 80, 0);
			lb1->setTag(1);
			node->addChild(lb1);

			lb2 = Label::create("", i < 3  ? "fonts/myriadb.ttf" : "fonts/myriad.ttf", 30);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setPosition(lb1->getPositionX() + 145, 0);
			lb2->setWidth(400);
			lb2->setHeight(30);
			lb2->setTag(2);
			node->addChild(lb2);

			lb3 = Label::create("", i < 3 ? "fonts/myriadb.ttf" : "fonts/myriad.ttf", 30);
			//lb3->setAnchorPoint(Vec2(1, .5f));
			lb3->setPosition(lb2->getPositionX() + 570, 0);
			lb3->setTag(3);
			node->addChild(lb3);

			ui::Scale9Sprite* spbg1 = ui::Scale9Sprite::createWithSpriteFrameName(i == 0 ? "box10.png" : "box11.png");
			spbg1->setContentSize(Size(130, 60));
			spbg1->setPosition(lb1->getPositionX(), lb1->getPositionY() + 5);
			node->addChild(spbg1, -1);

			ui::Scale9Sprite* spbg2 = ui::Scale9Sprite::createWithSpriteFrameName(i == 0 ? "box10.png" : "box11.png");
			spbg2->setContentSize(Size(lb2->getWidth() + 110, 60));
			spbg2->setPosition(lb2->getPositionX() + lb2->getWidth() / 2 - 20, lb2->getPositionY() + 5);
			node->addChild(spbg2, -1);

			ui::Scale9Sprite* spbg3 = ui::Scale9Sprite::createWithSpriteFrameName(i == 0 ? "box10.png" : "box11.png");
			spbg3->setContentSize(Size(260, 60));
			spbg3->setPosition(lb3->getPositionX(), lb3->getPositionY() + 5);
			node->addChild(spbg3, -1);
		} else {
			node->setVisible(true);
			lb1 = (Label*)node->getChildByTag(1);
			lb2 = (Label*)node->getChildByTag(2);
			lb3 = (Label*)node->getChildByTag(3);
		}
		lb2->setString(listRanks[type][i].Name);
		lb3->setString(Utils::getSingleton().formatMoneyWithComma(listRanks[type][i].Money));
		lb1->setColor(i == 0 ? Color3B::YELLOW : Color3B::BLACK);
		lb2->setColor(i == 0 ? Color3B::YELLOW : Color3B::BLACK);
		lb3->setColor(i == 0 ? Color3B::YELLOW : Color3B::BLACK);
	}
	int i = listRanks[type].size();
	Node* n;
	while ((n = scroll->getChildByTag(i++)) != nullptr) {
		n->setVisible(false);
	}
}

void BaseScene::showPopupRankWin()
{
	/*listRankWin.clear();
	for (int i = 0; i < 20; i++) {
		RankWinData data;
		data.Uid = 1000 + rand() % 10000;
		data.Name = rand() % 2 == 0 ? "STORMUSSSSSSKASFHJW" : ("stormussssss" + to_string(data.Uid));
		data.Point = rand() % 60;
		data.Cuoc = "Nha lau xe hoi Hoa roi cua phat Ca nhay dau a a a thuyen ngu ong bat ca";
		data.Date = "05/01/2016";
		listRankWin.push_back(data);
	}*/
	ui::ScrollView* scroll = (ui::ScrollView*)popupRank->getChildByName("scroll");
	ui::ScrollView* scrollWin = (ui::ScrollView*)popupRank->getChildByName("scrollwin");
	scroll->setVisible(false);
	scrollWin->setVisible(true);
	int height = listRankWin.size() * 90 + 60;
	int width = scrollWin->getContentSize().width;
	if (height < scrollWin->getContentSize().height) {
		height = scrollWin->getContentSize().height;
	}
	scrollWin->setInnerContainerSize(Size(width, height));
	for (int i = 0; i < listRankWin.size(); i++) {
		Label *lb1, *lb2, *lb3, *lb4;
		Node* node = scrollWin->getChildByTag(i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scrollWin->getContentSize().width / 2, height - 50 - i * 90);
			node->setTag(i);
			scrollWin->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
			nbg->setContentSize(Size(width, 85));
			node->addChild(nbg);

			lb1 = Label::create(to_string(i + 1), i < 3 ? "fonts/myriadb.ttf" : "fonts/myriad.ttf", 27);
			lb1->setPosition(-width / 2 + 65, 0);
			lb1->setTag(1);
			node->addChild(lb1);

			lb2 = Label::create("", i < 3 ? "fonts/myriadb.ttf" : "fonts/myriad.ttf", 27);
			lb2->setPosition(lb1->getPositionX() + 150, 0);
			lb2->setTag(2);
			node->addChild(lb2);

			lb3 = Label::create("", i < 3 ? "fonts/myriadb.ttf" : "fonts/myriad.ttf", 27);
			lb3->setPosition(lb2->getPositionX() + 365, 0);
			lb3->setWidth(450);
			lb3->setTag(3);
			node->addChild(lb3);

			lb4 = Label::create("", i < 3 ? "fonts/myriadb.ttf" : "fonts/myriad.ttf", 27);
			lb4->setPosition(lb3->getPositionX() + 300, 0);
			lb4->setTag(4);
			node->addChild(lb4);

			ui::Scale9Sprite* spbg1 = ui::Scale9Sprite::createWithSpriteFrameName(i == 0 ? "box10.png" : "box11.png");
			spbg1->setContentSize(Size(80, 80));
			spbg1->setPosition(lb1->getPositionX(), lb1->getPositionY() + 5);
			node->addChild(spbg1, -1);

			ui::Scale9Sprite* spbg2 = ui::Scale9Sprite::createWithSpriteFrameName(i == 0 ? "box10.png" : "box11.png");
			spbg2->setContentSize(Size(210, 80));
			spbg2->setPosition(lb2->getPositionX(), lb2->getPositionY() + 5);
			node->addChild(spbg2, -1);

			ui::Scale9Sprite* spbg3 = ui::Scale9Sprite::createWithSpriteFrameName(i == 0 ? "box10.png" : "box11.png");
			spbg3->setContentSize(Size(510, 80));
			spbg3->setPosition(lb3->getPositionX(), lb3->getPositionY() + 5);
			node->addChild(spbg3, -1);

			ui::Scale9Sprite* spbg4 = ui::Scale9Sprite::createWithSpriteFrameName(i == 0 ? "box10.png" : "box11.png");
			spbg4->setContentSize(Size(80, 80));
			spbg4->setPosition(lb4->getPositionX(), lb4->getPositionY() + 5);
			node->addChild(spbg4, -1);
		} else {
			node->setVisible(true);
			lb1 = (Label*)node->getChildByTag(1);
			lb2 = (Label*)node->getChildByTag(2);
			lb3 = (Label*)node->getChildByTag(3);
			lb4 = (Label*)node->getChildByTag(4);
		}
		lb3->setString(listRankWin[i].Cuoc);
		lb4->setString(Utils::getSingleton().formatMoneyWithComma(listRankWin[i].Point));

		string strName = listRankWin[i].Name;
		lb2->setString(strName);
		while (lb2->getContentSize().width > 200) {
			strName = strName.substr(0, strName.length() - 1);
			lb2->setString(strName);
		}

		lb1->setColor(i == 0 ? Color3B::YELLOW : Color3B::BLACK);
		lb2->setColor(i == 0 ? Color3B::YELLOW : Color3B::BLACK);
		lb3->setColor(i == 0 ? Color3B::YELLOW : Color3B::BLACK);
		lb4->setColor(i == 0 ? Color3B::YELLOW : Color3B::BLACK);
	}
	int i = listRankWin.size();
	Node* n;
	while ((n = scrollWin->getChildByTag(i++)) != nullptr) {
		n->setVisible(false);
	}
}

void BaseScene::showPopupUserInfo(UserData data, bool showHistoryIfIsMe)
{
	if (popupUserInfo == nullptr) {
		initPopupUserInfo();
	}
	showPopup(popupUserInfo);
	Node* btnHistory = popupUserInfo->getChildByName("btnhistory");
	Node* btnActive = popupUserInfo->getChildByName("btnactive");
    Node* btnFB = popupUserInfo->getChildByName("btnlogoutfb");
	Node* nodeInfo = popupUserInfo->getChildByName("nodeinfo");
	Label* lbName = (Label*)popupUserInfo->getChildByName("lbname");
	Label* lbAppellation = (Label*)popupUserInfo->getChildByName("lbappellation");
	Label* lbUname = (Label*)nodeInfo->getChildByName("lbuname");
	Label* lbUname1 = (Label*)nodeInfo->getChildByName("lbuname1");
	Label* lbQuan = (Label*)nodeInfo->getChildByName("lbquan");
	Label* lbXu = (Label*)nodeInfo->getChildByName("lbxu");
	Label* lbId = (Label*)nodeInfo->getChildByName("lbid");
	Label* lbId1 = (Label*)nodeInfo->getChildByName("lbid1");
	//Label* lbLevel = (Label*)nodeInfo->getChildByName("lblevel");
	Label* lbWin = (Label*)nodeInfo->getChildByName("lbwin");
	Label* lbTotal = (Label*)nodeInfo->getChildByName("lbtotal");
	Label* lbBigWin = (Label*)nodeInfo->getChildByName("lbbigwin");
	Label* lbBigCrest = (Label*)nodeInfo->getChildByName("lbbigcrest");
	Sprite* iconGold = (Sprite*)nodeInfo->getChildByName("icongold");
	Sprite* iconSilver = (Sprite*)nodeInfo->getChildByName("iconsilver");
	AppellationData aplData = Utils::getSingleton().getAppellationByLevel(data.Level);

	bool isMe = data.UserID == Utils::getSingleton().userDataMe.UserID;
	btnHistory->setVisible(showHistoryIfIsMe && isMe);
	btnActive->setVisible(showHistoryIfIsMe && isMe && !Utils::getSingleton().userDataMe.IsActived && Utils::getSingleton().ispmE());
	btnFB->setVisible(showHistoryIfIsMe && isMe && Utils::getSingleton().loginType == constant::LOGIN_FACEBOOK);
	lbUname->setVisible(isMe);
	lbUname1->setVisible(isMe);
	lbUname1->setString(data.Name);
	lbName->setString(data.DisplayName);
	lbQuan->setString(Utils::getSingleton().formatMoneyWithComma(data.MoneyReal));
	lbXu->setString(Utils::getSingleton().formatMoneyWithComma(data.MoneyFree));
	lbId->setVisible(isMe);
	lbId1->setVisible(isMe);
	lbId1->setString(to_string(data.UserID));
	//lbLevel->setString(Utils::getSingleton().getStringForKey("cap_do") + ":");// +to_string(data.Level));
	lbWin->setString(Utils::getSingleton().getStringForKey("thang") + ": " + to_string(data.Win));
	lbTotal->setString(Utils::getSingleton().getStringForKey("tong") + ": " + to_string(data.Total));
	lbAppellation->setString(aplData.Name);
	//lbAppellation->setColor(aplData.Color);
	//lbAppellation->enableOutline(aplData.ColorOutline, 2);
	//lbBigWin->setString(Utils::getSingleton().formatMoneyWithComma(data.BigWin));
	//lbBigCrest->setString(data.BigCrest);
	iconGold->setPosition(lbQuan->getPosition() + Vec2(lbQuan->getContentSize().width + 20, 5));
	iconSilver->setPosition(lbXu->getPosition() + Vec2(lbXu->getContentSize().width + 20, 5));
	nodeInfo->setPosition(isMe ? lbName->getPosition() - Vec2(0, 45) : lbName->getPosition() + Vec2(0, 20));
}

void BaseScene::setMoneyType(int type)
{
	moneyBg0->setTag(type);
	moneyBg1->setTag(type);
	chosenBg->setPosition(type == 0 ? 100 : -100, 0);
	//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), type == 1);
}

void BaseScene::pauseApplication()
{
	isPauseApp = true;
	SFSRequest::getSingleton().Disconnect();
}

void BaseScene::handleClientDisconnectionReason(std::string reason)
{
    if(isPauseApp){
        isPauseApp = false;
        reason = constant::DISCONNECTION_REASON_SYNC;
    }
	if (isBackToLogin) {
		Utils::getSingleton().goToLoginScene();
		return;
	}
	if (reason.compare(constant::DISCONNECTION_REASON_SYNC) == 0) {
		showWaiting();
		isReconnecting = true;
		DelayTime* delay = DelayTime::create(2);
		CallFunc* func = CallFunc::create([=]() {
			SFSGEvent::getSingleton().Reset();
			Utils::getSingleton().reconnect();
			Utils::getSingleton().timeStartReconnect = Utils::getSingleton().getCurrentSystemTimeInSecs();
		});
		this->runAction(Sequence::createWithTwoActions(delay, func));
		return;
	}
	hideWaiting();
	if (isOverlapLogin) {
		reason = "overlap_login";
	}
    if(!isShowDisconnected){
        isShowDisconnected = true;
        showPopupNotice(Utils::getSingleton().getStringForKey("disconnection_" + reason), [=]() {
            isShowDisconnected = false;
			if (SFSRequest::getSingleton().IsConnected()) {
				isPauseApp = true;
				SFSRequest::getSingleton().Disconnect();
			} else {
				if (reason.compare(constant::DISCONNECTION_REASON_UNKNOWN) == 0
					|| reason.compare(constant::DISCONNECTION_REASON_IDLE) == 0) {
					showWaiting();
					isReconnecting = true;
					SFSGEvent::getSingleton().Reset();
					Utils::getSingleton().reconnect();
					Utils::getSingleton().timeStartReconnect = Utils::getSingleton().getCurrentSystemTimeInSecs();
				} else {
					Utils::getSingleton().goToLoginScene();
				}
			}
        }, false);
    }
}

void BaseScene::addTouchEventListener(ui::Button* btn, std::function<void()> func, bool isNew)
{
	if (isNew) {
		buttons.push_back(btn);
	}
	btn->addTouchEventListener([=](Ref* ref, ui::Widget::TouchEventType type) {
		switch (type)
		{
			case cocos2d::ui::Widget::TouchEventType::BEGAN:
				if (Utils::getSingleton().SoundEnabled) {
					experimental::AudioEngine::play2d("sound/click.mp3");
				}
				break;
			case cocos2d::ui::Widget::TouchEventType::ENDED:
				func();
				break;
			default:
				break;
		}
	});
}

void BaseScene::onErrorResponse(unsigned char code, std::string msg)
{
	if (code == 51 && isChangingDisplayName) {
		if (popupDisplayName != NULL) {
			hidePopup(popupDisplayName);
		}
		setDisplayName(tmpDisplayName);
		isChangingDisplayName = false;
		return;
	}
	else if (code == 38) {
		isOverlapLogin = true;
		return;
	}
	else if (code == 33 && isChangingPassword) {
		isChangingPassword = false;
		UserDefault::getInstance()->setStringForKey(constant::KEY_PASSWORD.c_str(), newPassword);
	}
}

void BaseScene::onConnected()
{
	if (isReconnecting) {
		Utils::getSingleton().reloginZone();
	}
}

void BaseScene::onConnectionFailed()
{
	double waitedTime = Utils::getSingleton().getCurrentSystemTimeInSecs() - Utils::getSingleton().timeStartReconnect;
	if (waitedTime < 0) waitedTime = 0;
	if (waitedTime > 5) waitedTime = 5;
	DelayTime* delay = DelayTime::create(5 - waitedTime);
	CallFunc* func = CallFunc::create([=]() {
		hideWaiting();
		showPopupNotice(Utils::getSingleton().getStringForKey("connection_failed"), [=]() {
			Utils::getSingleton().timeStartReconnect = Utils::getSingleton().getCurrentSystemTimeInSecs();
			Utils::getSingleton().goToLoginScene();
		}, false);
	});
	this->runAction(Sequence::createWithTwoActions(delay, func));
}

void BaseScene::onLoginZone()
{
	if (isReconnecting) {
		isReconnecting = false;
	}
}

void BaseScene::onLoginZoneError(short int code, std::string msg)
{
	if (isReconnecting) {
		isReconnecting = false;
		isBackToLogin = true;
		SFSRequest::getSingleton().Disconnect();
	}
}

void BaseScene::initHeaderWithInfos()
{
	hasHeader = true;
	bool isRealMoney = Utils::getSingleton().moneyType == 1;
	bool ispmE = Utils::getSingleton().ispmE();
	int posY = winSize.height - 55;

	Sprite* spHeader = Sprite::create("bg_header_bar.png");
	spHeader->setAnchorPoint(Vec2(0, 1));
	spHeader->setPosition(0, winSize.height);
	mLayer->addChild(spHeader);

	ui::Button* btnBack = ui::Button::create("btn_back.png", "", "", ui::Widget::TextureResType::PLIST);
	btnBack->setPosition(Vec2(60, posY));
	addTouchEventListener(btnBack, [=]() {
		onBackScene();
	});
	mLayer->addChild(btnBack, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnBack);

	initCofferView(btnBack->getPosition() + Vec2(105, 2), constant::MAIN_ZORDER_HEADER, .8f);

	Sprite* bgAvatar = Sprite::createWithSpriteFrameName("bg_avatar_main.png");
	bgAvatar->setPosition(btnBack->getPosition() + Vec2(ispmE ? 210 : 105, 0));
	mLayer->addChild(bgAvatar, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(bgAvatar);

	ui::Button* btnAvar = ui::Button::create("avatar.png", "avatar.png", "", ui::Widget::TextureResType::PLIST);
	btnAvar->setPosition(bgAvatar->getPosition());
	btnAvar->setScale(.85f);
	addTouchEventListener(btnAvar, [=]() {
		showPopupUserInfo(Utils::getSingleton().userDataMe);
	});
	mLayer->addChild(btnAvar, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnAvar);

	lbName = Label::create("Name", "fonts/myriadb.ttf", 27);
	lbName->setAnchorPoint(Vec2(0, .5f));
	lbName->setPosition(btnAvar->getPositionX() + 50 * scaleScene.y, btnAvar->getPositionY() + 30);
	lbName->setWidth(150);
	lbName->setHeight(25);
	mLayer->addChild(lbName, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbName);

	lbId = Label::create("ID: ", "fonts/myriadb.ttf", 27);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setPosition(btnAvar->getPositionX() + 50 * scaleScene.y, btnAvar->getPositionY() - 3);
	mLayer->addChild(lbId, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbId);

	lbLevel = Label::create(Utils::getSingleton().getStringForKey("level") + ": ", "fonts/myriadb.ttf", 27);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setPosition(btnAvar->getPositionX() + 50 * scaleScene.y, btnAvar->getPositionY() - 30);
	mLayer->addChild(lbLevel, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbLevel);

	ui::Button* btnSettings = ui::Button::create("btn_settings.png", "", "", ui::Widget::TextureResType::PLIST);
	btnSettings->setPosition(Vec2(winSize.width - 60, winSize.height - 55));
	addTouchEventListener(btnSettings, [=]() {
		if (popupMainSettings == nullptr) {
			initPopupSettings();
		}
		showPopup(popupMainSettings);
	});
	mLayer->addChild(btnSettings, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnSettings);

	ui::Button* btnRank = ui::Button::create("btn_rank.png", "", "", ui::Widget::TextureResType::PLIST);
	btnRank->setPosition(btnSettings->getPosition() - Vec2(105, 0));
	addTouchEventListener(btnRank, [=]() {
		if (listRanks.size() == 0) {
			SFSRequest::getSingleton().RequestRank();
			SFSRequest::getSingleton().RequestRankWin();
		} else {
			showPopupRank(ispmE ? 0 : 1);
		}
	});
	mLayer->addChild(btnRank, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnRank);

	int posMoney = (bgAvatar->getPositionX() + btnSettings->getPositionX() - 90) / 2;
	Node* moneyNode = Node::create();
	moneyNode->setPosition(posMoney, posY);
	mLayer->addChild(moneyNode, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(moneyNode);

	spMoneyBg = ui::Scale9Sprite::createWithSpriteFrameName("bg_slide_money.png");
	spMoneyBg->setContentSize(Size(500, 80));
	spMoneyBg->setInsetTop(0);
	spMoneyBg->setInsetBottom(0);
	spMoneyBg->setInsetLeft(50);
	spMoneyBg->setInsetRight(50);
	moneyNode->addChild(spMoneyBg);

	moneyBg0 = ui::Button::create("empty.png", "empty.png", "", ui::Widget::TextureResType::PLIST);
	moneyBg0->setContentSize(Size(240, 70));
	moneyBg0->setScale9Enabled(true);
	moneyBg0->setTag((int)isRealMoney);
	moneyBg0->setPosition(spMoneyBg->getPosition() + Vec2(150, 0));
	moneyNode->addChild(moneyBg0, 0);

	moneyBg1 = ui::Button::create("empty.png", "empty.png", "", ui::Widget::TextureResType::PLIST);
	moneyBg1->setContentSize(moneyBg0->getContentSize());
	moneyBg1->setScale9Enabled(true);
	moneyBg1->setTag((int)isRealMoney);
	moneyBg1->setPosition(spMoneyBg->getPosition() + Vec2(-150, 0));
	moneyNode->addChild(moneyBg1, 0);

	chosenBg = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	chosenBg->setContentSize(moneyBg0->getContentSize());
	chosenBg->setPosition(spMoneyBg->getPosition() + Vec2(isRealMoney ? -123 : 123, 0));
	moneyNode->addChild(chosenBg, 1);
	
	addTouchEventListener(moneyBg0, [=]() {
		if (moneyBg0->getTag() == 1) {
			moneyBg0->setTag(0);
			moneyBg1->setTag(0);
			chosenBg->setPosition(spMoneyBg->getPosition() + Vec2(123, 0));
			onChangeMoneyType(0);
			//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), false);
		}
	});
	addTouchEventListener(moneyBg1, [=]() {
		if (moneyBg0->getTag() == 0) {
			moneyBg0->setTag(1);
			moneyBg1->setTag(1);
			chosenBg->setPosition(spMoneyBg->getPosition() + Vec2(-123, 0));
			onChangeMoneyType(1);
			//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), true);
		}
	});

	Sprite* iconGold = Sprite::createWithSpriteFrameName("icon_gold.png");
	iconGold->setPosition(spMoneyBg->getPosition() + Vec2(-200, 0));
	iconGold->setScale(1);
	moneyNode->addChild(iconGold, 2);

	lbGold = Label::create("0", "fonts/myriadb.ttf", 35);
	lbGold->setPosition(iconGold->getPosition() + Vec2(40, -5));
	lbGold->setAnchorPoint(Vec2(0, .5f));
	lbGold->setColor(Color3B::YELLOW);
	moneyNode->addChild(lbGold, 2);

	Sprite* iconSilver = Sprite::createWithSpriteFrameName("icon_silver.png");
	iconSilver->setPosition(spMoneyBg->getPosition() + Vec2(40, 0));
	iconSilver->setScale(1);
	moneyNode->addChild(iconSilver, 2);

	lbSilver = Label::create("0", "fonts/myriadb.ttf", 35);
	lbSilver->setPosition(iconSilver->getPosition() + Vec2(40, -5));
	lbSilver->setAnchorPoint(Vec2(0, .5f));
	lbSilver->setColor(Color3B(0, 255, 255));
	moneyNode->addChild(lbSilver, 2);

	if (Utils::getSingleton().userDataMe.UserID > 0) {
		onUserDataMeResponse();
	}

	if (!ispmE) {
		chosenBg->setOpacity(0);
		//chosenBg->setPosition(spMoneyBg->getPosition() + Vec2(isRealMoney && ispmE ? -100 : 100, 0));
		moneyNode->setPosition(posMoney - 130, posY);
		spMoneyBg->setContentSize(Size(250, 76));
		spMoneyBg->setPositionX(125);
		moneyBg1->setVisible(false);
		iconGold->setVisible(false);
		lbGold->setVisible(false);
		lbCoffer->setVisible(false);
	}
}

void BaseScene::onBackScene()
{

}

void BaseScene::showPopupHistory()
{
	int moneyType = !Utils::getSingleton().ispmE();
	if (popupHistory == nullptr) {
		initPopupHistory();
	}
	showPopup(popupHistory);
	SFSRequest::getSingleton().RequestPlayHistory(moneyType, 0);

	ui::Button* btn0 = (ui::Button*)popupHistory->getChildByTag(10);
	ui::Button* btn1 = (ui::Button*)popupHistory->getChildByTag(11);
	btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
	btn0->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
	btn1->setTitleColor(Color3B::WHITE);
	btn0->setTitleColor(Color3B::YELLOW);
	popupHistory->setTag(moneyType);
	popupHistory->getChildByName("nodepage")->setTag(1);
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = (ui::Button*)popupHistory->getChildByTag(1000 + i);
		btn->setTitleText(to_string(i));
		if (i == 1) {
			btn->loadTextureNormal("bg_page_number_active.png", ui::Widget::TextureResType::PLIST);
		} else {
			btn->loadTextureNormal("bg_page_number.png", ui::Widget::TextureResType::PLIST);
		}
	}
}

void BaseScene::showPopupCoffer()
{
	if (popupCoffer == nullptr) {
		initPopupCoffer();
	}
	showPopup(popupCoffer);

	Node* nodeHistory = popupCoffer->getChildByName("nodehistory");
	ui::ScrollView* scrollHistory = (ui::ScrollView*)nodeHistory->getChildByName("scrollhistory");
	if (scrollHistory->getChildrenCount() == 0) {
		SFSRequest::getSingleton().RequestCofferHistory();
	}
}

void BaseScene::onChangeMoneyType(int type)
{
}

bool BaseScene::onKeyBack()
{
	if (popups.size() == 0) {
		return true;
	}
	Node* popup = popups[popups.size() - 1];
	if (popup->getName().compare("popupdisplayname") != 0) {
		hidePopup(popup);
	}
	return false;
}

void BaseScene::onKeyHome()
{
	pauseApplication();
}

void BaseScene::hideSplash()
{
	if (isWaiting) return;
	for (ui::Button* btn : blockedButtons) {
		btn->setTouchEnabled(true);
	}
	blockedButtons.clear();
	splash->setVisible(false);
	splash->setLocalZOrder(0);
}

void BaseScene::hideWaiting()
{
	isWaiting = false;
	hidePopup(spWaiting->getParent(), false);
	spWaiting->pauseSchedulerAndActions();
	spWaiting->getParent()->stopAllActions();
}

void BaseScene::hidePopup(cocos2d::Node * popup, bool runEffect)
{
	if (runEffect) {
		runEffectHidePopup(popup);
	} else {
		popup->setVisible(false);
	}
	if (popups.size() == 0) return;
	int i = popups.size() - 1;
	while (i >= 0 && popups[i] != popup) {
		i--;
	}
	if (i == -1) return;
	if (i == popups.size() - 1) {
		popups.pop_back();
		if (popups.size() == 0) {
			hideSplash();
		} else {
			setSplashZOrder(popups[i - 1]->getLocalZOrder() - 1);
		}
	} else {
		popups.erase(popups.begin() + i);
	}
}

void BaseScene::initEventView(Vec2 pos, Size size)
{
	eventView = Node::create();
	eventView->setPosition(pos);
	eventView->setVisible(false);
	mLayer->addChild(eventView, constant::GAME_ZORDER_BUTTON);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	bg->setAnchorPoint(Vec2(0, .5f));
	bg->setColor(Color3B::BLACK);
	bg->setContentSize(size);
	bg->setOpacity(50);
	eventView->addChild(bg);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scroll->setPosition(Vec2(0, -bg->getContentSize().height / 2));
	scroll->setBounceEnabled(true);
	scroll->setContentSize(size);
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	scroll->setEnabled(false);
	eventView->addChild(scroll);

	Node* nodelb = Node::create();
	nodelb->setName("nodelb");
	scroll->addChild(nodelb);
}

void BaseScene::showToast(std::string tag, ::string msg, Vec2 pos)
{
	mLayer->removeChildByName(tag);
	Node* nodeChat = Node::create();
	nodeChat->setPosition(pos);
	nodeChat->setScale(0);
	nodeChat->setName(tag);
	mLayer->addChild(nodeChat, constant::GAME_ZORDER_BUTTON);

	cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/chat.xml");
	std::string ss = plist[msg].asString();
	if (ss.length() > 0) msg = ss;

	Label* lb = Label::create();
	lb->setString(msg);
	lb->setSystemFontName("Arial Bold");
	lb->setSystemFontSize(27);
	lb->setColor(Color3B(0, 203, 131));
	nodeChat->addChild(lb, 2);

	Size sizeContent = lb->getContentSize();
	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("box12.png");
	bg->setContentSize(sizeContent + Size(200, 40));
	bg->setOpacity(200);
	nodeChat->addChild(bg, 1);

	float w = Director::getInstance()->getWinSize().width;
	if (pos.x > 0.6*w)
	{
		bg->setPosition(Vec2(-sizeContent.width*0.5, 0));
		lb->setPosition(Vec2(-sizeContent.width*0.5, 0));
	} else if (pos.x < 0.4*w)
	{
		bg->setPosition(Vec2(sizeContent.width*0.5, 0));
		lb->setPosition(Vec2(sizeContent.width*0.5, 0));
	}

	Vec2 smoothScale = getScaleSmoothly(1);
	nodeChat->runAction(ScaleTo::create(0.25f, smoothScale.x, smoothScale.y));
	nodeChat->runAction(Sequence::create(DelayTime::create(3), RemoveSelf::create(), nullptr));
}

cocos2d::Node * BaseScene::createPopup(std::string stitle, bool isBig, bool isHidden, cocos2d::Size bgsize)
{
	if (isHidden && !isPopupReady) {
		showWaiting();
		return nullptr;
	}

	Node* popup = Node::create();
	popup->setPosition(winSize.width / 2, winSize.height / 2);
	popup->setVisible(false);
	mLayer->addChild(popup, constant::ZORDER_POPUP);

	if (isBig) {
		Sprite* bg = Sprite::create("popup_bg1.png");
		bg->setName("spbg");
		popup->addChild(bg);
		bgsize = bg->getContentSize();
	} else {
		if (bgsize.width == 0 && bgsize.height == 0) {
			bgsize = Size(618, 373);
		}
		ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
		bg->setContentSize(bgsize);
		bg->setInsetLeft(169);
		bg->setInsetRight(128);
		bg->setInsetTop(160);
		bg->setInsetBottom(160);
		bg->setName("spbg");
		popup->addChild(bg);
	}

	Sprite* title = Sprite::createWithSpriteFrameName(stitle);
	title->setPosition(0, bgsize.height / 2 - (isBig ? 30 : 35));
	title->setName("sptitle");
	popup->addChild(title);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(bgsize.width / 2 - (isBig ? 50 : 55), bgsize.height / 2 - 35));
	btnClose->setName("btnclose");
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popup);
	});
	popup->addChild(btnClose);

	return popup;
}

cocos2d::Node * BaseScene::createPopupDetail()
{
	Node* popupDetail = nullptr;
	for (Node* n : vecPopupDetails) {
		if (!n->isVisible()) {
			popupDetail = n;
			break;
		}
	}
	if (popupDetail == nullptr) {
		popupDetail = Node::create();
	}

	popupDetail->setPosition(winSize.width / 2, winSize.height / 2);
	popupDetail->setVisible(false);
	mLayer->addChild(popupDetail, constant::ZORDER_POPUP_NOTICE);

	Sprite* bg = Sprite::createWithSpriteFrameName("bg_detail.png");
	popupDetail->addChild(bg);

	Label* lb = Label::createWithTTF("", "fonts/myriad.ttf", 40);
	lb->setColor(Color3B::BLACK);
	lb->setWidth(550);
	lb->setName("lbcontent");
	lb->setAlignment(TextHAlignment::CENTER);
	popupDetail->addChild(lb);

	Size bgSize = bg->getContentSize();
	ui::Button* btndong = ui::Button::create("btn_dong2.png", "", "", ui::Widget::TextureResType::PLIST);
	btndong->setPosition(Vec2(bgSize.width / 2 - 70, bgSize.height / 2 - 35));
	btndong->setName("btnclose");
	addTouchEventListener(btndong, [=]() {
		hidePopup(popupDetail);
	});
	popupDetail->addChild(btndong);

	return popupDetail;
}

Node* BaseScene::createPopupNotice()
{
	Node* popupNotice = Node::create();
	popupNotice->setPosition(winSize.width / 2, winSize.height / 2);
	popupNotice->setVisible(false);
	mLayer->addChild(popupNotice, constant::ZORDER_POPUP_NOTICE);

	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	popupNotice->addChild(bg);

	Sprite* title = Sprite::createWithSpriteFrameName("title_thongbao.png");
	title->setPosition(0, 155);
	//title->setScale(.8f);
	popupNotice->addChild(title);

	Label* lb = Label::createWithTTF("", "fonts/myriad.ttf", 40);
	lb->setColor(Color3B::BLACK);
	lb->setWidth(550);
	lb->setName("lbcontent");
	lb->setAlignment(TextHAlignment::CENTER);
	popupNotice->addChild(lb);

	ui::Button* btnok = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnok->setTitleText(Utils::getSingleton().getStringForKey("xac_nhan"));
	btnok->setTitleFontName("fonts/myriadb.ttf");
	btnok->setTitleFontSize(40);
	btnok->setTitleDeviation(Vec2(0, -5));
	btnok->setPosition(Vec2(0, -170));
	btnok->setName("btnsubmit");
	addTouchEventListener(btnok, [=]() {});
	popupNotice->addChild(btnok);

	Size bgSize = bg->getContentSize();
	ui::Button* btndong = ui::Button::create("btn_dong.png", "", "", ui::Widget::TextureResType::PLIST);
	btndong->setPosition(Vec2(bgSize.width/2 - 55, bgSize.height / 2 - 35));
	//btndong->setScale(.8f);
	btndong->setName("btnclose");
	addTouchEventListener(btndong, [=]() {
		hidePopup(popupNotice);
	});
	popupNotice->addChild(btndong);

	return popupNotice;
}

cocos2d::Node * BaseScene::getPopupNotice()
{
	for (Node* n : vecPopupNotices) {
		if (!n->isVisible()) {
			ui::Button* btnClose = (ui::Button*)n->getChildByName("btnclose");
			btnClose->setTouchEnabled(true);
			return n;
			break;
		}
	}
	Node* popupNotice = createPopupNotice();
	vecPopupNotices.pushBack(popupNotice);
	return popupNotice;
}

cocos2d::Vec2 BaseScene::getScaleSmoothly(float scale)
{
	return cocos2d::Vec2(scale * scaleScene.y, scale * scaleScene.x);
}

void BaseScene::initPopupRank()
{
	bool pmE = Utils::getSingleton().ispmE();
	popupRank = createPopup("title_bangxephang.png", true, true);
	if (!popupRank) return;
	popupRank->setTag(pmE ? 0 : 1);

	Size size = Size(960, 440);
	Size scrollSize = size - Size(20, 20);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2 - 50));
	scroll->setContentSize(scrollSize);
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupRank->addChild(scroll);

	ui::ScrollView* scrollWin = ui::ScrollView::create();
	scrollWin->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollWin->setBounceEnabled(true);
	scrollWin->setPosition(scroll->getPosition());
	scrollWin->setContentSize(scrollSize);
	scrollWin->setScrollBarEnabled(false);
	scrollWin->setName("scrollwin");
	popupRank->addChild(scrollWin);

	Sprite* bgMenu = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgMenu->setPosition(0, 200);
	popupRank->addChild(bgMenu);

	vector<string> texts = { "top_quan", "top_xu", "top_u" };
	int x = -280;
	int y = bgMenu->getPositionY();
	for (int i = pmE ? 0 : 1; i < texts.size(); i++) {
		string strBtn = i == 0 ? "box12.png" : "empty.png";
		ui::Button* btn = ui::Button::create(strBtn, strBtn, strBtn, ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleFontSize(35);
		btn->setTitleColor(i == 0 ? Color3B::YELLOW : Color3B::WHITE);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setContentSize(Size(255, 60));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(0, 0, 0, 0));
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			showPopupRank(btn->getTag());
		});
		popupRank->addChild(btn);

		x += 280;
	}
}

void BaseScene::initPopupSettings()
{
	Size bgsize = Size(800, 600);
	popupMainSettings = createPopup("title_caidat.png", false, false, bgsize);
	popupMainSettings->setTag(0);

	ui::CheckBox* checkboxSound = ui::CheckBox::create();
	checkboxSound->loadTextureBackGround("unchecked.png", ui::Widget::TextureResType::PLIST);
	checkboxSound->loadTextureFrontCross("checked.png", ui::Widget::TextureResType::PLIST);
	checkboxSound->setPosition(Vec2(-280, -bgsize.height / 2 + 80));
	checkboxSound->setSelected(Utils::getSingleton().SoundEnabled);
	popupMainSettings->addChild(checkboxSound);

	checkboxSound->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		bool isSelected = type == ui::CheckBox::EventType::SELECTED;
		Utils::getSingleton().SoundEnabled = isSelected;
		UserDefault::getInstance()->setBoolForKey(constant::KEY_SOUND.c_str(), isSelected);
	});

	Label* lbcbSound = Label::createWithTTF(Utils::getSingleton().getStringForKey("sound"), "fonts/myriadb.ttf", 35);
	lbcbSound->setPosition(checkboxSound->getPosition() + Vec2(40, -5));
	lbcbSound->setAnchorPoint(Vec2(0, .5f));
	lbcbSound->setColor(Color3B::BLACK);
	popupMainSettings->addChild(lbcbSound);

	ui::CheckBox* checkboxInvite = ui::CheckBox::create();
	checkboxInvite->loadTextureBackGround("unchecked.png", ui::Widget::TextureResType::PLIST);
	checkboxInvite->loadTextureFrontCross("checked.png", ui::Widget::TextureResType::PLIST);
	checkboxInvite->setPosition(Vec2(checkboxSound->getPositionX() + 300, checkboxSound->getPositionY()));
	checkboxInvite->setSelected(Utils::getSingleton().SoundEnabled);
	popupMainSettings->addChild(checkboxInvite);

	checkboxInvite->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		bool isSelected = type == ui::CheckBox::EventType::SELECTED;
		Utils::getSingleton().IgnoreInvitation = isSelected;
		UserDefault::getInstance()->setBoolForKey(constant::KEY_INVITATION.c_str(), isSelected);
	});

	Label* lbcbInvite = Label::createWithTTF(Utils::getSingleton().getStringForKey("reject_all_invitation"), "fonts/myriadb.ttf", 35);
	lbcbInvite->setPosition(checkboxInvite->getPosition() + Vec2(40, -5));
	lbcbInvite->setAnchorPoint(Vec2(0, .5f));
	lbcbInvite->setColor(Color3B::BLACK);
	popupMainSettings->addChild(lbcbInvite);

	Node* nodePass = Node::create();
	nodePass->setName("nodepass");
	nodePass->setVisible(true);
	nodePass->setTag(100);
	nodePass->setPosition(0, 0);
	popupMainSettings->addChild(nodePass);

	Node* nodeName = Node::create();
	nodeName->setName("nodename");
	nodeName->setVisible(false);
	nodeName->setTag(101);
	nodeName->setPosition(0, 0);
	popupMainSettings->addChild(nodeName);

	ui::EditBox* tfPass = ui::EditBox::create(Size(280, 55), "empty.png", ui::Widget::TextureResType::PLIST);
	ui::EditBox* tfPassRetype = ui::EditBox::create(Size(280, 55), "empty.png", ui::Widget::TextureResType::PLIST);
	ui::EditBox* tfName = ui::EditBox::create(Size(280, 55), "empty.png", ui::Widget::TextureResType::PLIST);

	ui::Scale9Sprite* bgMenu = ui::Scale9Sprite::createWithSpriteFrameName("bg_slide_money.png");
	bgMenu->setContentSize(Size(700, 80));
	bgMenu->setInsetTop(0);
	bgMenu->setInsetBottom(0);
	bgMenu->setInsetLeft(50);
	bgMenu->setInsetRight(50);
	bgMenu->setPosition(0, bgsize.height / 2 - 120);
	popupMainSettings->addChild(bgMenu);

	int x = -230;
	int y = bgMenu->getPositionY();
	vector<string> texts = { "doi_mat_khau", "doi_ten", "feedback" };
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box12.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleFontSize(35);
		btn->setTitleColor(i == 0 ? Color3B::YELLOW : Color3B::WHITE);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setContentSize(Size(215, 60));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		btn->setTag(10 + i);
		addTouchEventListener(btn, [=]() {
			if (i == 2) {
				showWebView(Utils::getSingleton().getFeedbackUrl());
				return;
			}
			if (popupMainSettings->getTag() == i) return;
			popupMainSettings->getChildByTag(100 + i)->setVisible(true);
			popupMainSettings->getChildByTag(100 + popupMainSettings->getTag())->setVisible(false);
			ui::Button* btn1 = (ui::Button*)popupMainSettings->getChildByTag(10 + popupMainSettings->getTag());
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::YELLOW);
			popupMainSettings->setTag(i);
		});
		popupMainSettings->addChild(btn);
		x += 230;
	}

	//Node Change Pass
	Label* lbPassRule = Label::createWithTTF(Utils::getSingleton().getStringForKey("quy_dinh_mat_khau"), "fonts/myriad.ttf", 30);
	lbPassRule->setAlignment(TextHAlignment::CENTER);
	lbPassRule->setColor(Color3B::BLACK);
	lbPassRule->setWidth(550);
	lbPassRule->setPosition(0, 60);
	nodePass->addChild(lbPassRule);

	Sprite* bgPass = Sprite::createWithSpriteFrameName("input_mathe.png");
	bgPass->setPosition(-135, lbPassRule->getPositionY() - 80);
	nodePass->addChild(bgPass);

	Sprite* bgPassRetype = Sprite::createWithSpriteFrameName("input_mathe.png");
	bgPassRetype->setPosition(bgPass->getPositionX(), bgPass->getPositionY() - 80);
	nodePass->addChild(bgPassRetype);

	tfPass->setPosition(bgPass->getPosition());
	tfPass->setFontName("Myriad Pro Condensed");
	tfPass->setFontSize(30);
	tfPass->setFontColor(Color3B::WHITE);
	tfPass->setMaxLength(24);
	tfPass->setPlaceholderFont("Myriad Pro Condensed", 25);
	tfPass->setPlaceholderFontColor(Color3B(250, 250, 250));
	tfPass->setPlaceHolder(Utils::getSingleton().getStringForKey("new_password").c_str());
	tfPass->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfPass->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
	tfPass->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfPass->setDelegate(this);
	nodePass->addChild(tfPass);

	tfPassRetype->setPosition(bgPassRetype->getPosition());
	tfPassRetype->setFontName("Myriad Pro Condensed");
	tfPassRetype->setFontSize(30);
	tfPassRetype->setFontColor(Color3B::WHITE);
	tfPassRetype->setMaxLength(24);
	tfPassRetype->setPlaceholderFont("Myriad Pro Condensed", 25);
	tfPassRetype->setPlaceholderFontColor(Color3B(250, 250, 250));
	tfPassRetype->setPlaceHolder(Utils::getSingleton().getStringForKey("retype_password").c_str());
	tfPassRetype->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfPassRetype->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
	tfPassRetype->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfPassRetype->setDelegate(this);
	nodePass->addChild(tfPassRetype);

	ui::Button* btnChangePass = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnChangePass->setPosition(Vec2(bgPass->getPositionX() + 330, bgPass->getPositionY() - 40));
	btnChangePass->setTitleText(Utils::getSingleton().getStringForKey("agree"));
	btnChangePass->setTitleFontName("fonts/myriadb.ttf");
	btnChangePass->setTitleFontSize(40);
	btnChangePass->setTitleDeviation(Vec2(0, -5));
	addTouchEventListener(btnChangePass, [=]() {
		string pass = tfPass->getText();
		if (!Utils::getSingleton().isPasswordValid(pass)) {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_password_must_6_char"), [=]() {});
			return;
		}
		string retypePass = tfPassRetype->getText();
		if (retypePass.compare(pass) != 0) {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_retype_password"), [=]() {});
			return;
		}
		showWaiting();
		tfPass->setText("");
		tfPassRetype->setText("");
		newPassword = pass;
		isChangingPassword = true;
		SFSRequest::getSingleton().RequestChangePassword(md5(pass), md5(retypePass));
	});
	nodePass->addChild(btnChangePass);

	//Node Change DisplayName
	Label* lbNoteName = Label::createWithTTF(Utils::getSingleton().getStringForKey("chu_y_chi_doi_ten_1_lan"), "fonts/myriad.ttf", 30);
	lbNoteName->setAlignment(TextHAlignment::CENTER);
	lbNoteName->setColor(Color3B::RED);
	lbNoteName->setPosition(0, 60);
	nodeName->addChild(lbNoteName);

	Sprite* bgName = Sprite::createWithSpriteFrameName("input_mathe.png");
	bgName->setPosition(-135, lbPassRule->getPositionY() - 80);
	nodeName->addChild(bgName);

	tfName->setPosition(bgName->getPosition());
	tfName->setFontName("Myriad Pro Condensed");
	tfName->setFontSize(30);
	tfName->setFontColor(Color3B::WHITE);
	tfName->setMaxLength(16);
	tfName->setPlaceholderFont("Myriad Pro Condensed", 25);
	tfName->setPlaceholderFontColor(Color3B(250, 250, 250));
	tfName->setPlaceHolder(Utils::getSingleton().getStringForKey("new_login_name").c_str());
	tfName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfName->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfName->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfName->setDelegate(this);
	nodeName->addChild(tfName);

	ui::Button* btnChangeName = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnChangeName->setPosition(Vec2(bgName->getPositionX() + 330, bgName->getPositionY()));
	btnChangeName->setTitleText(Utils::getSingleton().getStringForKey("agree"));
	btnChangeName->setTitleFontName("fonts/myriadb.ttf");
	btnChangeName->setTitleFontSize(40);
	btnChangeName->setTitleDeviation(Vec2(0, -5));
	btnChangeName->setScale(.9f);
	addTouchEventListener(btnChangeName, [=]() {
		string displayname = tfName->getText();
		if (!Utils::getSingleton().isDisplayNameValid(displayname)) {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_displayname_format"), [=]() {});
			return;
		}
		showWaiting();
		tfName->setText("");
		tmpDisplayName = displayname;
		isChangingDisplayName = true;
		SFSRequest::getSingleton().RequestChangeDisplayname(displayname);
	});
	nodeName->addChild(btnChangeName);
}

void BaseScene::initPopupUserInfo()
{
	bool ispmE = Utils::getSingleton().ispmE();

	popupUserInfo = Node::create();
	popupUserInfo->setPosition(winSize.width / 2, winSize.height / 2);
	popupUserInfo->setVisible(false);
	mLayer->addChild(popupUserInfo, constant::ZORDER_POPUP);

	Size psize = Size(800, 550);
	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setContentSize(psize);
	bg->setInsetLeft(169);
	bg->setInsetRight(128);
	bg->setInsetTop(160);
	bg->setInsetBottom(160);
	popupUserInfo->addChild(bg);

	Sprite* title = Sprite::createWithSpriteFrameName("title_thongtin.png");
	title->setPosition(0, psize.height / 2 - 30);
	popupUserInfo->addChild(title);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(psize.width / 2 - 50, psize.height / 2 - 35));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupUserInfo);
	});
	popupUserInfo->addChild(btnClose);

	Sprite* bgAvar = Sprite::createWithSpriteFrameName("bg_user_avatar.png");
	bgAvar->setPosition(-255, 75);
	bgAvar->setScale(1.4f);
	popupUserInfo->addChild(bgAvar);

	Sprite* avatar = Sprite::createWithSpriteFrameName("avatar_default.png");
	avatar->setPosition(bgAvar->getPositionX(), bgAvar->getPositionY() + 20);
	avatar->setScale(1.4f);
	avatar->setName("avatar");
	popupUserInfo->addChild(avatar);

	Label* lbAppellation = Label::create("Huong Truong", "fonts/davida.ttf", 25);
	lbAppellation->setColor(Color3B::BLACK);
	lbAppellation->setOpacity(200);
	lbAppellation->setPosition(avatar->getPositionX(), avatar->getPositionY() - 95);
	//lbAppellation->enableShadow(Color4B::BLACK);
	lbAppellation->setName("lbappellation");
	popupUserInfo->addChild(lbAppellation);

	ui::Button* btnHistory = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnHistory->setTitleText(Utils::getSingleton().getStringForKey("lich_su"));
	btnHistory->setTitleFontName("fonts/myriadb.ttf");
	btnHistory->setTitleFontSize(40);
	btnHistory->setTitleDeviation(Vec2(0, -5));
	btnHistory->setPosition(Vec2(lbAppellation->getPositionX(), lbAppellation->getPositionY() - 65));
	btnHistory->setName("btnhistory");
	btnHistory->setScale(.8f);
	addTouchEventListener(btnHistory, [=]() {
		showPopupHistory();
	});
	popupUserInfo->addChild(btnHistory);

	ui::Button* btnActive = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnActive->setTitleText(Utils::getSingleton().getStringForKey("kich_hoat"));
	btnActive->setTitleFontName("fonts/myriadb.ttf");
	btnActive->setTitleFontSize(40);
	btnActive->setTitleDeviation(Vec2(0, -5));
	btnActive->setPosition(Vec2(btnHistory->getPositionX(), btnHistory->getPositionY() - 60));
	btnActive->setName("btnactive");
	btnActive->setScale(.8f);
	addTouchEventListener(btnActive, [=]() {
		string str = Utils::getSingleton().gameConfig.smsKH;
		int index = str.find_last_of(' ');
		string number = str.substr(index + 1, str.length() - index);
		string content = str.substr(0, index);
		content = Utils::getSingleton().replaceString(content, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
		Utils::getSingleton().openSMS(number, content);
	});
	popupUserInfo->addChild(btnActive);

    ui::Button* btnLogoutFb = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnLogoutFb->setTitleText("Logout FB");
	btnLogoutFb->setTitleFontName("fonts/myriadb.ttf");
	btnLogoutFb->setTitleFontSize(40);
	btnLogoutFb->setTitleDeviation(Vec2(0, -5));
    btnLogoutFb->setPosition(Vec2(btnActive->getPositionX(), btnActive->getPositionY() - 60));
    btnLogoutFb->setName("btnlogoutfb");
    btnLogoutFb->setScale(.8f);
    addTouchEventListener(btnLogoutFb, [=]() {
		isBackToLogin = true;
        SFSRequest::getSingleton().Disconnect();
        Utils::getSingleton().logoutFacebook();
    });
    popupUserInfo->addChild(btnLogoutFb);

	int x = 45;
	Label* lbName = Label::create("Stormus", "fonts/myriadb.ttf", 40);
	lbName->setAnchorPoint(Vec2(0, .5f));
	lbName->setColor(Color3B::BLACK);
	lbName->setPosition(-130, 170);
	lbName->setName("lbname");
	popupUserInfo->addChild(lbName);

	Node* nodeInfo = Node::create();
	nodeInfo->setName("nodeinfo");
	nodeInfo->setPosition(lbName->getPositionX(), lbName->getPositionY() - x);
	popupUserInfo->addChild(nodeInfo);

	Label* lbUname = Label::create(Utils::getSingleton().getStringForKey("login") + ":", "fonts/myriad.ttf", 30);
	lbUname->setAnchorPoint(Vec2(0, .5f));
	lbUname->setColor(Color3B::BLACK);
	lbUname->setPosition(0, 0);
	lbUname->setName("lbuname");
	nodeInfo->addChild(lbUname);

	Label* lbUname1 = Label::create("stormus", "fonts/myriad.ttf", 30);
	lbUname1->setAnchorPoint(Vec2(0, .5f));
	lbUname1->setColor(Color3B::BLACK);
	lbUname1->setPosition(lbUname->getPositionX() + 150, lbUname->getPositionY());
	lbUname1->setName("lbuname1");
	nodeInfo->addChild(lbUname1);

	Label* lbId = Label::create("ID:", "fonts/myriad.ttf", 30);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setColor(Color3B::BLACK);
	lbId->setPosition(lbUname->getPositionX(), lbUname->getPositionY() - x);
	lbId->setName("lbid");
	nodeInfo->addChild(lbId);

	Label* lbId1 = Label::create("847558", "fonts/myriad.ttf", 30);
	lbId1->setAnchorPoint(Vec2(0, .5f));
	lbId1->setColor(Color3B::BLACK);
	lbId1->setPosition(lbId->getPositionX() + 150, lbId->getPositionY());
	lbId1->setName("lbid1");
	nodeInfo->addChild(lbId1);

	Label* lbQuan = Label::create(Utils::getSingleton().getStringForKey("quan") + ":", "fonts/myriad.ttf", 30);
	lbQuan->setAnchorPoint(Vec2(0, .5f));
	lbQuan->setColor(Color3B::BLACK);
	lbQuan->setPosition(lbId->getPositionX(), lbId->getPositionY() - x);
	lbQuan->setVisible(ispmE);
	nodeInfo->addChild(lbQuan);

	Label* lbQuan1 = Label::create("100,000", "fonts/myriad.ttf", 30);
	lbQuan1->setAnchorPoint(Vec2(0, .5f));
	lbQuan1->setColor(Color3B::BLACK);
	lbQuan1->setPosition(lbQuan->getPositionX() + 150, lbQuan->getPositionY());
	lbQuan1->setName("lbquan");
	lbQuan1->setVisible(ispmE);
	nodeInfo->addChild(lbQuan1);

	Sprite* icGold = Sprite::createWithSpriteFrameName("icon_gold.png");
	icGold->setPosition(lbQuan1->getPosition() + Vec2(lbQuan1->getContentSize().width + 20, 5));
	icGold->setScale(.5f);
	icGold->setName("icongold");
	nodeInfo->addChild(icGold);

	Label* lbXu = Label::create(Utils::getSingleton().getStringForKey("xu") + ":", "fonts/myriad.ttf", 30);
	lbXu->setAnchorPoint(Vec2(0, .5f));
	lbXu->setColor(Color3B::BLACK);
	lbXu->setPosition(lbQuan->getPositionX(), lbQuan->getPositionY() - x);
	nodeInfo->addChild(lbXu);

	Label* lbXu1 = Label::create("100,000", "fonts/myriad.ttf", 30);
	lbXu1->setAnchorPoint(Vec2(0, .5f));
	lbXu1->setColor(Color3B::BLACK);
	lbXu1->setPosition(lbXu->getPositionX() + 150, lbXu->getPositionY());
	lbXu1->setName("lbxu");
	nodeInfo->addChild(lbXu1);

	Sprite* icSilver = Sprite::createWithSpriteFrameName("icon_silver.png");
	icSilver->setPosition(lbXu1->getPosition() + Vec2(lbXu1->getContentSize().width + 20, 5));
	icSilver->setScale(.5f);
	icSilver->setName("iconsilver");
	nodeInfo->addChild(icSilver);
	
	/*Label* lbLevel = Label::create(Utils::getSingleton().getStringForKey("cap_do") + ":", "fonts/myriad.ttf", 30);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setColor(Color3B::WHITE);
	lbLevel->setPosition(-70, -70);
	lbLevel->setName("lblevel");
	popupUserInfo->addChild(lbLevel);*/

	Label* lbWin = Label::create(Utils::getSingleton().getStringForKey("thang") + ": 12", "fonts/myriad.ttf", 30);
	lbWin->setAnchorPoint(Vec2(0, .5f));
	lbWin->setColor(Color3B::BLACK);
	lbWin->setPosition(lbXu->getPositionX(), lbXu->getPositionY() - x);
	lbWin->setName("lbwin");
	nodeInfo->addChild(lbWin);

	Label* lbTotal = Label::create(Utils::getSingleton().getStringForKey("tong") + ": 20", "fonts/myriad.ttf", 30);
	lbTotal->setAnchorPoint(Vec2(0, .5f));
	lbTotal->setColor(Color3B::BLACK);
	lbTotal->setPosition(lbWin->getPositionX() + 250, lbWin->getPositionY());
	lbTotal->setName("lbtotal");
	nodeInfo->addChild(lbTotal);

	Label* lbBigWin = Label::create(Utils::getSingleton().getStringForKey("thang_lon_nhat") + ":", "fonts/myriad.ttf", 30);
	lbBigWin->setPosition(lbWin->getPositionX(), lbWin->getPositionY() - x);
	lbBigWin->setAnchorPoint(Vec2(0, .5f));
	lbBigWin->setColor(Color3B::BLACK);
	lbBigWin->setVisible(false);
	nodeInfo->addChild(lbBigWin);

	Label* lbBigWin1 = Label::create("1111", "fonts/myriad.ttf", 30);
	lbBigWin1->setPosition(lbBigWin->getPositionX() + lbBigWin->getContentSize().width + 10, lbBigWin->getPositionY());
	lbBigWin1->setAnchorPoint(Vec2(0, .5f));
	lbBigWin1->setColor(Color3B::BLACK);
	lbBigWin1->setName("lbbigwin");
	lbBigWin1->setVisible(false);
	nodeInfo->addChild(lbBigWin1);

	Label* lbBigCrest = Label::create(Utils::getSingleton().getStringForKey("u_to_nhat") + ": Nha lau xe hoi - Hoa roi cua pha t, a a a Ngu ong bat ca, Ca loi san dinh", "fonts/myriad.ttf", 30);
	lbBigCrest->setPosition(lbBigWin->getPositionX(), lbBigWin->getPositionY() - x + 20);
	lbBigCrest->setAnchorPoint(Vec2(0, 1));
	lbBigCrest->setColor(Color3B::BLACK);
	lbBigCrest->setWidth(480);
	lbBigCrest->setName("lbbigcrest");
	lbBigCrest->setVisible(false);
	nodeInfo->addChild(lbBigCrest);

	//Label* lbBigCrest1 = Label::create("Nha lau xe hoi - Hoa roi cua pha t, a a a Ngu ong bat ca, Ca loi san dinh", "fonts/myriad.ttf", 25);
	//lbBigCrest1->setPosition(lbBigCrest->getPositionX() + lbBigCrest->getContentSize().width + 10, lbBigCrest->getPositionY() + 15);
	////lbBigCrest1->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
	//lbBigCrest1->setAnchorPoint(Vec2(0, 1));
	//lbBigCrest1->setColor(Color3B::WHITE);
	//lbBigCrest1->setWidth(380);
	//lbBigCrest1->setName("lbbigcrest");
	//popupUserInfo->addChild(lbBigCrest1);
}

void BaseScene::initPopupHistory()
{
	bool ispmE = Utils::getSingleton().ispmE();

	popupHistory = createPopup("title_lichsu.png", true, false);
	popupHistory->setTag(0);

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgContent->setContentSize(Size(960, ispmE ? 406 : 500));
	bgContent->setPosition(0, ispmE ? 0 : 30);
	popupHistory->addChild(bgContent);

	Size size = bgContent->getContentSize();

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-size.width / 2 + 5, -size.height / 2 + 5) + bgContent->getPosition());
	scroll->setContentSize(Size(size.width - 10, size.height - 50));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupHistory->addChild(scroll);

	Sprite* bgMenu = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgMenu->setPosition(0, 200);
	bgMenu->setVisible(ispmE);
	popupHistory->addChild(bgMenu);

	vector<string> texts = { "quan" , "xu" };
	int x = -300;
	int y = bgMenu->getPositionY();
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box12.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleFontSize(35);
		btn->setTitleColor(i == 0 ? Color3B::YELLOW : Color3B::WHITE);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setContentSize(Size(210, 60));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(10 + i);
		btn->setVisible(ispmE);
		addTouchEventListener(btn, [=]() {
			if (popupHistory->getTag() == i) return;
			ui::Button* btn1 = (ui::Button*)popupHistory->getChildByTag(10 + popupHistory->getTag());
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::YELLOW);
			popupHistory->setTag(i);
			SFSRequest::getSingleton().RequestPlayHistory(i, 0);

			popupHistory->getChildByName("nodepage")->setTag(1);
			for (int i = 1; i <= 5; i++) {
				ui::Button* btn = (ui::Button*)popupHistory->getChildByTag(1000 + i);
				btn->setTitleText(to_string(i));
				if (i == 1) {
					btn->loadTextureNormal("bg_page_number_active.png", ui::Widget::TextureResType::PLIST);
				} else {
					btn->loadTextureNormal("bg_page_number.png", ui::Widget::TextureResType::PLIST);
				}
			}
		});
		popupHistory->addChild(btn);

		x += 250;
	}

	addBtnChoosePage(0, -257, popupHistory, [=](int page) {
		int type = popupHistory->getTag();
		SFSRequest::getSingleton().RequestPlayHistory(type, page - 1);
		//onPlayLogDataResponse(vector<PlayLogData>());
	});
}

void BaseScene::initPopupCoffer()
{
	popupCoffer = createPopup("title_huvang.png", true, true);
	if (!popupCoffer) return;
	popupCoffer->setTag(0);

	Size size = Size(960, 440);
	Size scrollSize = size - Size(20, 20);

	Node* nodeGuide = Node::create();
	nodeGuide->setPosition(0, -50);
	nodeGuide->setName("nodeguide");
	nodeGuide->setTag(0);
	popupCoffer->addChild(nodeGuide);

	Node* nodeHistory = Node::create();
	nodeHistory->setPosition(nodeGuide->getPosition());
	nodeHistory->setName("nodehistory");
	nodeHistory->setTag(1);
	nodeHistory->setVisible(false);
	popupCoffer->addChild(nodeHistory);

	Sprite* bgMenu = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgMenu->setPosition(0, 200);
	popupCoffer->addChild(bgMenu);

	vector<string> texts = { "huong_dan" , "lich_su_no" };
	int x = -300;
	int y = bgMenu->getPositionY();
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box12.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleFontSize(35);
		btn->setTitleColor(i == 0 ? Color3B::YELLOW : Color3B::WHITE);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setContentSize(Size(255, 60));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(10 + i);
		addTouchEventListener(btn, [=]() {
			if (popupCoffer->getTag() == i) return;
			popupCoffer->getChildByTag(i)->setVisible(true);
			popupCoffer->getChildByTag(1 - i)->setVisible(false);
			ui::Button* btn1 = (ui::Button*)popupCoffer->getChildByTag(10 + popupCoffer->getTag());
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::YELLOW);
			popupCoffer->setTag(i);
		});
		popupCoffer->addChild(btn);
		x += 250;
	}

	//Node guide
	/*ui::Scale9Sprite* bgScrollGuide = ui::Scale9Sprite::createWithSpriteFrameName("box.png");
	bgScrollGuide->setContentSize(size);
	bgScrollGuide->setPosition(95, -25);
	nodeGuide->addChild(bgScrollGuide);*/

	ui::ScrollView* scrollGuide = ui::ScrollView::create();
	scrollGuide->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollGuide->setBounceEnabled(true);
	scrollGuide->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2));
	scrollGuide->setContentSize(scrollSize);
	scrollGuide->setScrollBarEnabled(false);
	scrollGuide->setName("scrollguide");
	nodeGuide->addChild(scrollGuide);

	std::string guideContent = Utils::getSingleton().cofferGuide;
	if (guideContent.length() == 0) {
		SFSRequest::getSingleton().RequestHttpGet("http://kinhtuchi.com/huvang.txt", constant::TAG_HTTP_COFFER_GUIDE);
		cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
		guideContent = plist["tutorial_5"].asString();
	}
	Label* lb = Label::create(guideContent, "fonts/myriad.ttf", 30);
	lb->setAnchorPoint(Vec2(0, 1));
	lb->setColor(Color3B::BLACK);
	lb->setName("lbcontent");
	lb->setWidth(scrollSize.width);
	scrollGuide->addChild(lb);

	int height = lb->getContentSize().height;
	if (height < scrollGuide->getContentSize().height) {
		height = scrollGuide->getContentSize().height;
	}
	lb->setPosition(0, height);
	scrollGuide->setInnerContainerSize(Size(scrollSize.width, height));

	//Node history
	ui::ScrollView* scrollHistory = ui::ScrollView::create();
	scrollHistory->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollHistory->setBounceEnabled(true);
	scrollHistory->setPosition(scrollGuide->getPosition());
	scrollHistory->setContentSize(scrollSize);
	scrollHistory->setScrollBarEnabled(false);
	scrollHistory->setName("scrollhistory");
	nodeHistory->addChild(scrollHistory);
}

void BaseScene::initPopupIAP()
{
	popupIAP = createPopup("title_naptien.png", true, false);

	Size size = Size(810, 466);

	Node* nodeStore = Node::create();
	nodeStore->setName("nodestore");
	nodeStore->setTag(102);
	nodeStore->setPosition(0, 30);
	popupIAP->addChild(nodeStore);

	vector<ProductData> products = Utils::getSingleton().products;
	Size storeSize = Size(960, 360);

	ui::ScrollView* scrollStore = ui::ScrollView::create();
	scrollStore->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollStore->setBounceEnabled(true);
	scrollStore->setPosition(Vec2(-storeSize.width / 2, -storeSize.height / 2 - 50));
	scrollStore->setContentSize(storeSize);
	scrollStore->setScrollBarEnabled(false);
	scrollStore->setName("scrollstore");
	nodeStore->addChild(scrollStore);

	int storeWidth = products.size() * 240;
	if (storeWidth < storeSize.width) {
		storeWidth = storeSize.width;
	}
	scrollStore->setInnerContainerSize(Size(storeWidth, storeSize.height));
	string strCurrency = " " + Utils::getSingleton().getStringForKey("vnd");
	for (int i = 0; i < products.size(); i++) {
		int index = products[i].Description.find(' ');
		string strValue = products[i].Description.substr(0, index);
		string strCost = Utils::getSingleton().formatMoneyWithComma(products[i].Price) + strCurrency;
		string strId = products[i].Id;

		ui::Button* btn = ui::Button::create("box10.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setPosition(Vec2(170 + i * 310, storeSize.height / 2 + 45));
		btn->setContentSize(Size(270, 200));
		btn->setScale9Enabled(true);
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			showWaiting(300);
			Utils::getSingleton().purchaseItem(strId);
		});
		scrollStore->addChild(btn);

		Sprite* sp = Sprite::createWithSpriteFrameName("icon_money" + to_string(i) + ".png");
		sp->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 20);
		sp->setName("itemimage");
		btn->addChild(sp);

		Sprite* spCoin = Sprite::createWithSpriteFrameName("icon_gold.png");
		btn->addChild(spCoin);

		Label* lb1 = Label::create(strValue, "fonts/myriadb.ttf", 50);
		lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, btn->getContentSize().height / 2 - 80);
		lb1->setColor(Color3B::YELLOW);
		btn->addChild(lb1);

		ui::Scale9Sprite* bglb2 = ui::Scale9Sprite::createWithSpriteFrameName("box3.png");
		bglb2->setContentSize(Size(btn->getContentSize().width, 70));
		bglb2->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 - 145);
		btn->addChild(bglb2);

		Label* lb2 = Label::create(strCost, "fonts/myriadb.ttf", 50);
		lb2->enableOutline(Color4B::BLACK, 2);
		lb2->setPosition(bglb2->getPosition() + Vec2(0, -10));
		lb2->setColor(Color3B::WHITE);
		lb2->setHorizontalAlignment(TextHAlignment::CENTER);
		btn->addChild(lb2);

		spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
			+ spCoin->getContentSize().width * spCoin->getScale() / 2 + 5, lb1->getPositionY() + 10);
	}
}

void BaseScene::initWebView()
{
	nodeWebview = Node::create();
	nodeWebview->setPosition(winSize.width / 2, winSize.height / 2);
	nodeWebview->setVisible(false);
	mLayer->addChild(nodeWebview, constant::ZORDER_WEBVIEW);
	autoScaleNode(nodeWebview);

	ui::Scale9Sprite* webSplash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	webSplash->setContentSize(Size(1500, 1000));
	webSplash->setColor(Color3B::BLACK);
	webSplash->setOpacity(150);
	nodeWebview->addChild(webSplash);

	/*Sprite* bg = Sprite::create("popup_bg1.png");
	bg->setName("spbg");
	nodeWebview->addChild(bg);*/

	auto vsize = Director::getInstance()->getVisibleSize();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	Size wSize = vsize - Size(0, 160);
	auto webView = experimental::ui::WebView::create();
	webView->setContentSize(wSize);
	webView->setScalesPageToFit(true);
	webView->setName("webview");
	webView->setVisible(false);
	nodeWebview->addChild(webView);
#endif

	ui::CheckBox* checkbox = ui::CheckBox::create();
	checkbox->loadTextureBackGround("unchecked.png", ui::Widget::TextureResType::PLIST);
	checkbox->loadTextureFrontCross("checked.png", ui::Widget::TextureResType::PLIST);
	checkbox->setPosition(Vec2(250, winSize.height / 2 - 35));
	checkbox->setSelected(false);
	checkbox->setVisible(false);
	nodeWebview->addChild(checkbox);
	checkbox->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		if (type == ui::CheckBox::EventType::SELECTED) {
			Utils::getSingleton().allowEventPopup = false;
		} else if (type == ui::CheckBox::EventType::UNSELECTED) {
			Utils::getSingleton().allowEventPopup = true;
		}
	});

	Label* lb = Label::create(Utils::getSingleton().getStringForKey("khong_hien_lai"), "fonts/myriad.ttf", 30);
	lb->setPosition(checkbox->getPosition() + Vec2(40, -5));
	lb->setAnchorPoint(Vec2(0, .5f));
	lb->setColor(Color3B::WHITE);
	lb->setVisible(false);
	nodeWebview->addChild(lb);

	float xscale = winSize.height / vsize.height;
	float yscale = winSize.width / vsize.width;
	ui::Button* btnClose = ui::Button::create("btn_dong.png", "", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2((winSize.width / 2 - 35) * xscale, (winSize.height / 2 - 33) * yscale));
	addTouchEventListener(btnClose, [=]() {
		nodeWebview->setVisible(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		webView->setVisible(false);
#endif
	});
	nodeWebview->addChild(btnClose);
}

void BaseScene::initCofferView(Vec2 pos, int zorder, float scale)
{
	bool ispmE = Utils::getSingleton().ispmE();
	ui::Button* btnCoffer = ui::Button::create("coffer.png", "coffer.png", "", ui::Widget::TextureResType::PLIST);
	btnCoffer->setPosition(pos);
	btnCoffer->setScale(scale);
	btnCoffer->setVisible(ispmE);
	addTouchEventListener(btnCoffer, [=]() {
		showPopupCoffer();
	});
	mLayer->addChild(btnCoffer, zorder);
	autoScaleNode(btnCoffer);

	ui::Scale9Sprite* bgMoney = ui::Scale9Sprite::createWithSpriteFrameName("box7.png");
	bgMoney->setPosition(btnCoffer->getContentSize().width / 2, 15);
	bgMoney->setContentSize(Size(111, 37));
	btnCoffer->addChild(bgMoney);

	lbCoffer = Label::create(Utils::getSingleton().formatMoneyWithComma(Utils::getSingleton().cofferMoney), "fonts/myriad.ttf", 25);
	lbCoffer->setPosition(bgMoney->getPosition() + Vec2(0, -5));
	lbCoffer->setColor(Color3B::YELLOW);
	btnCoffer->addChild(lbCoffer);

	/*Sprite* lightPoint = Sprite::createWithSpriteFrameName("light_point.png");
	lightPoint->setPosition(btnCoffer->getContentSize().width / 2, btnCoffer->getContentSize().height / 2);
	btnCoffer->addChild(lightPoint, -1);

	ScaleTo* scale1 = ScaleTo::create(2, .7f);
	ScaleTo* scale2 = ScaleTo::create(2, 1);
	lightPoint->runAction(RepeatForever::create(Sequence::createWithTwoActions(scale1, scale2)));*/
}

void BaseScene::onPingPong(long timems)
{
	pingId++;
	if (timems < 200) {
		spNetwork->initWithSpriteFrameName("wifi0.png");
	} else if (timems < 1000) {
		spNetwork->initWithSpriteFrameName("wifi1.png");
	} else {
		spNetwork->initWithSpriteFrameName("wifi2.png");
	}
	//spNetwork->setAnchorPoint(Vec2(1, 0));
	//lbNetwork->setString(to_string(timems) + "ms");
}

void BaseScene::onUserDataMeResponse()
{
	if (!hasHeader) return;
	std::string strGold = Utils::getSingleton().formatMoneyWithComma(Utils::getSingleton().userDataMe.MoneyReal);
	std::string strSilver = Utils::getSingleton().formatMoneyWithComma(Utils::getSingleton().userDataMe.MoneyFree);
	std::string strId = String::createWithFormat("ID: %ld", Utils::getSingleton().userDataMe.UserID)->getCString();
	std::string strLevel = String::createWithFormat((Utils::getSingleton().getStringForKey("level") + ": %d").c_str(), Utils::getSingleton().userDataMe.Level)->getCString();

	lbName->setString(Utils::getSingleton().userDataMe.DisplayName);
	lbGold->setString(strGold);
	lbSilver->setString(strSilver);
	lbId->setString(strId);
	lbLevel->setString(strLevel);

	if (chargingProvider.length() > 0) {
		vector<double> moneys = { 10000, 20000, 30000, 50000, 100000, 200000, 300000, 500000 };
		double diff = Utils::getSingleton().userDataMe.MoneyReal - myRealMoney;
		int i = -1;
		while (++i < moneys.size() && diff >= moneys[i]);
		if (i > 0) {
			Tracker::getSingleton().trackPurchaseSuccess("Card", chargingProvider, "VND", moneys[i - 1]);
			chargingProvider = "";
		}
	}
}

void BaseScene::onRankDataResponse(std::vector<std::vector<RankData>> list)
{
	bool ispmE = Utils::getSingleton().ispmE();
	this->listRanks = list;
	showPopupRank(ispmE ? 0 : 1);
}

void BaseScene::onRankWinDataResponse(std::vector<RankWinData> list)
{
	this->listRankWin = list;
	if (popupRank != nullptr && popupRank->isVisible() && popupRank->getTag() == 2) {
		showPopupRankWin();
	}
}

void BaseScene::onListEventDataResponse(std::vector<EventData> list)
{
	if (eventView == nullptr || !Utils::getSingleton().ispmE()) return;
	eventView->setTag(0);
	runEventView(list);
}

void BaseScene::onPlayLogDataResponse(std::vector<PlayLogData> logs)
{
	if (popupHistory == nullptr) return;

	/*int numb = rand() % 20;
	for (int i = 0; i < numb; i++) {
	PlayLogData data;
	data.Date = "02/12/2016";
	data.Info = "Nha Tranh, Phong 500, u tu do, (Stormus, Reno)";
	data.Money = rand() % 10000;
	data.Balance = rand() % 100000;
	data.GameId = 1;
	logs.push_back(data);
	}*/

	vector<int> posX = { -440, -340, -90, 170, 325, 440 };
	vector<int> widths = { 70, 120, 340, 150, 150, 70 };
	ui::ScrollView* scroll = (ui::ScrollView*)popupHistory->getChildByName("scroll");

	int tag = scroll->getTag();
	if (tag > 0) {
		tag -= 6;
		for (int j = 0; j < 6; j++) {
			Label* lb = (Label*)scroll->getChildByTag(tag + j);
			lb->setColor(Color3B::BLACK);
			lb->getChildByTag(0)->setVisible(true);
			lb->getChildByTag(1)->setVisible(false);
		}
	}

	int height = logs.size() * 90;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
	scroll->setTag(0);
	int count = scroll->getChildrenCount();
	for (int i = 0; i < logs.size(); i++) {
		vector<Label*> lbs;
		ui::Button* btn;
		if (i < count / 7) {
			for (int j = 0; j < 6; j++) {
				Label* lb = (Label*)scroll->getChildByTag(i * 7 + j);
				lb->setVisible(true);
				lbs.push_back(lb);
			}

			btn = (ui::Button*)scroll->getChildByTag(i * 7 + 6);
			btn->setVisible(true);
		} else {
			for (int j = 0; j < 6; j++) {
				Label* lbDetail = Label::create("", "fonts/myriad.ttf", 25);
				lbDetail->setWidth(widths[j]);
				lbDetail->setHeight(50);
				lbDetail->setAnchorPoint(Vec2(.5f, 1));
				lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
				lbDetail->setTag(i * 7 + j);
				lbDetail->setColor(Color3B::BLACK);
				scroll->addChild(lbDetail);
				lbs.push_back(lbDetail);

				ui::Scale9Sprite* spbg1 = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
				spbg1->setPosition(lbDetail->getWidth() / 2, lbDetail->getHeight() / 2);
				spbg1->setContentSize(Size(widths[j] + (j == 2 ? 20 : 0), 80));
				spbg1->setTag(0);
				lbDetail->addChild(spbg1, -1);

				ui::Scale9Sprite* spbg2 = ui::Scale9Sprite::createWithSpriteFrameName("box12.png");
				spbg2->setPosition(spbg1->getPosition());
				spbg2->setContentSize(spbg1->getContentSize());
				spbg2->setVisible(false);
				spbg2->setTag(1);
				lbDetail->addChild(spbg2, -1);
			}

			btn = ui::Button::create("white.png", "", "", ui::Widget::TextureResType::PLIST);
			btn->setContentSize(Size(scroll->getContentSize().width, 80));
			btn->setAnchorPoint(Vec2(.5f, 1));
			btn->setScale9Enabled(true);
			btn->setOpacity(0);
			btn->setTag(i * 7 + 6);
			addTouchEventListener(btn, [=]() {
				int tag = scroll->getTag();
				if (tag != btn->getTag()) {
					for (int j = 0; j < 6; j++) {
						lbs[j]->setColor(Color3B::WHITE);
						lbs[j]->getChildByTag(0)->setVisible(false);
						lbs[j]->getChildByTag(1)->setVisible(true);
					}
					if (tag > 0) {
						tag -= 6;
						for (int j = 0; j < 6; j++) {
							Label* lb = (Label*)scroll->getChildByTag(tag + j);
							lb->setColor(Color3B::BLACK);
							lb->getChildByTag(0)->setVisible(true);
							lb->getChildByTag(1)->setVisible(false);
						}
					}
					scroll->setTag(btn->getTag());
				}
				Node* popupDetail = createPopupDetail();
				Label* lbContent = (Label*)popupDetail->getChildByName("lbcontent");
				lbContent->setString(lbs[2]->getString());
				showPopup(popupDetail);
			});
			scroll->addChild(btn);
		}
		btn->setPosition(Vec2(scroll->getContentSize().width / 2, height - i * 90));
		lbs[0]->setString(to_string((popupHistory->getChildByName("nodepage")->getTag() - 1) * 10 + i + 1));
		lbs[1]->setString(logs[i].Date);
		lbs[2]->setString(logs[i].Info);
		lbs[3]->setString(Utils::getSingleton().formatMoneyWithComma(logs[i].Money));
		lbs[4]->setString(Utils::getSingleton().formatMoneyWithComma(logs[i].Balance));
		lbs[5]->setString(to_string(logs[i].GameId));
		for (int j = 0; j < 6; j++) {
			lbs[j]->setPosition(posX[j] + btn->getPositionX(), btn->getPositionY() - 15);
		}
	}

	for (int i = logs.size() * 7; i < count; i++) {
		scroll->getChildByTag(i)->setVisible(false);
	}
}

void BaseScene::onCofferMoneyResponse(long money)
{
	Utils::getSingleton().cofferMoney = money;
	if (lbCoffer != nullptr) {
		lbCoffer->setString(Utils::getSingleton().formatMoneyWithComma(money));
	}
}

void BaseScene::onCofferHistoryResponse(std::vector<CofferWinnerData> list)
{
	/*list.clear();
	int numb = 10 + rand() % 20;
	for (int i = 0; i < numb; i++) {
		CofferWinnerData data;
		data.Uid = 1000 + rand() % 10000;
		data.Name = rand() % 2 == 0 ? "STORMUSQWERTYUIO" : ("stormusssss" + to_string(data.Uid));
		data.Point = rand() % 60;
		data.Cuocs = rand() % 2 == 0 ? "Nha lau xe hoi Hoa roi cua phat Ca ca nhay dau thuyen ngu ong bat ca" : "Nha lau xe hoi Hoa roi cua phat";
		data.Date = "05/01/2016 19::00:00";
		data.Money = 10000L * (rand() % 10000);
		list.push_back(data);
	}*/
	
	Node* nodeHistory = popupCoffer->getChildByName("nodehistory");
	ui::ScrollView* scroll = (ui::ScrollView*)nodeHistory->getChildByName("scrollhistory");
	int height = list.size() * 80 + 40;
	int width = scroll->getContentSize().width;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(width, height));
	for (int i = 0; i < list.size(); i++) {
		Label* lb2;
		Node* node = scroll->getChildByTag(i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scroll->getContentSize().width / 2, height - 40 - i * 80);
			node->setTag(i);
			scroll->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
			nbg->setContentSize(Size(width, 85));
			node->addChild(nbg);

			Label* lb1 = Label::create(list[i].Date, "fonts/myriad.ttf", 25);
			//lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(Color3B::BLACK);
			lb1->setPosition(-width / 2 + 20, 0);
			lb1->setWidth(100);
			lb1->setTag(1);
			lb1->setVisible(false);
			node->addChild(lb1);

			lb2 = Label::create(list[i].Name, "fonts/myriad.ttf", 25);
			lb2->setColor(Color3B::BLACK);
			lb2->setPosition(lb1->getPositionX() + 80, 0);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::create(list[i].Cuocs, "fonts/myriad.ttf", 25);
			lb3->setHorizontalAlignment(TextHAlignment::CENTER);
			lb3->setColor(Color3B::BLACK);
			lb3->setPosition(lb2->getPositionX() + 355, 0);
			lb3->setWidth(380);
			lb3->setTag(3);
			node->addChild(lb3);

			Label* lb5 = Label::create(Utils::getSingleton().formatMoneyWithComma(list[i].Money), "fonts/myriad.ttf", 25);
			lb5->setColor(Color3B::BLACK);
			lb5->setPosition(lb3->getPositionX() + 330, 0);
			lb5->setTag(5);
			node->addChild(lb5);

			Label* lb4 = Label::create(Utils::getSingleton().formatMoneyWithComma(list[i].Point), "fonts/myriad.ttf", 25);
			lb4->setColor(Color3B::BLACK);
			lb4->setPosition(lb5->getPositionX() + 115, 0);
			lb4->setTag(4);
			node->addChild(lb4);

			ui::Scale9Sprite* spbg1 = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
			spbg1->setContentSize(Size(120, 70));
			spbg1->setPosition(lb1->getPositionX(), lb1->getPositionY() + 5);
			spbg1->setVisible(false);
			node->addChild(spbg1, -1);

			ui::Scale9Sprite* spbg2 = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
			spbg2->setContentSize(Size(200, 70));
			spbg2->setPosition(lb2->getPositionX(), lb2->getPositionY() + 5);
			node->addChild(spbg2, -1);

			ui::Scale9Sprite* spbg3 = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
			spbg3->setContentSize(Size(500, 70));
			spbg3->setPosition(lb3->getPositionX(), lb3->getPositionY() + 5);
			node->addChild(spbg3, -1);

			ui::Scale9Sprite* spbg4 = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
			spbg4->setContentSize(Size(70, 70));
			spbg4->setPosition(lb4->getPositionX(), lb4->getPositionY() + 5);
			node->addChild(spbg4, -1);

			ui::Scale9Sprite* spbg5 = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
			spbg5->setContentSize(Size(150, 70));
			spbg5->setPosition(lb5->getPositionX(), lb5->getPositionY() + 5);
			node->addChild(spbg5, -1);
		} else {
			node->setVisible(true);
			node->setPositionY(height - 40 - i * 75);
			Label* lb1 = (Label*)node->getChildByTag(1);
			lb2 = (Label*)node->getChildByTag(2);
			Label* lb3 = (Label*)node->getChildByTag(3);
			Label* lb4 = (Label*)node->getChildByTag(4);
			lb1->setString(list[i].Date);
			lb2->setString(list[i].Name);
			lb3->setString(list[i].Cuocs);
			lb4->setString(Utils::getSingleton().formatMoneyWithComma(list[i].Point));
		}

		string strName = list[i].Name;
		if (strName.length() > 14) {
			strName = strName.substr(0, 14);
		}
		lb2->setString(strName);
		while (lb2->getContentSize().width > 170) {
			strName = strName.substr(0, strName.length() - 1);
			lb2->setString(strName);
		}
	}
	int i = list.size();
	Node* n;
	while ((n = scroll->getChildByTag(i++)) != nullptr) {
		n->setVisible(false);
	}
}

void BaseScene::onDownloadedPlistTexture(int numb)
{
	isPopupReady = numb >= 2;
}

void BaseScene::onHttpResponse(int tag, std::string content)
{
	if (tag == constant::TAG_HTTP_COFFER_GUIDE) {
		Utils::getSingleton().cofferGuide = content;
		if (popupCoffer) {
			Node* nodeGuide = popupCoffer->getChildByName("nodeguide");
			ui::ScrollView* scrollGuide = (ui::ScrollView*)nodeGuide->getChildByName("scrollguide");
			Label* lb = (Label*)scrollGuide->getChildByName("lbcontent");
			lb->setString(content);
			int height = lb->getContentSize().height;
			if (height < scrollGuide->getContentSize().height) {
				height = scrollGuide->getContentSize().height;
			}
			lb->setPosition(0, height);
			scrollGuide->setInnerContainerSize(Size(scrollGuide->getContentSize().width, height));
		}
	}
}

void BaseScene::onHttpResponseFailed()
{
	CCLOG("falied");
}

void BaseScene::editBoxReturn(cocos2d::ui::EditBox * editBox)
{
}

void BaseScene::addBtnChoosePage(int x, int y, cocos2d::Node * node, std::function<void(int)> funcPage)
{
	Node* nodePage = Node::create();
	nodePage->setTag(1);
	nodePage->setName("nodepage");
	node->addChild(nodePage);

	int x1 = x - 140;
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = ui::Button::create(i == 1 ? "bg_page_number_active.png" : "bg_page_number.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleFontSize(35);
		btn->setTitleText(to_string(i));
		btn->setTitleColor(Color3B::WHITE);
		btn->setTitleDeviation(Vec2(0, -3));
		btn->setPosition(Vec2(x1, y));
		btn->setTag(1000 + i);
		addTouchEventListener(btn, [=]() {
			int curPage = nodePage->getTag();
			int numb = atoi(btn->getTitleText().c_str());
			if (numb == curPage) return;
			ui::Button* curBtn = (ui::Button*)node->getChildByTag(1000 + ((curPage - 1) % 5) + 1);
			curBtn->loadTextureNormal("bg_page_number.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("bg_page_number_active.png", ui::Widget::TextureResType::PLIST);
			nodePage->setTag(numb);
			funcPage(numb);
		});
		node->addChild(btn);
		x1 += 70;
	}

	ui::Button* btnPre = ui::Button::create("btn_page_number_prev.png", "", "", ui::Widget::TextureResType::PLIST);
	btnPre->setPosition(Vec2(x - 220, y));
	addTouchEventListener(btnPre, [=]() {
		for (int i = 1; i <= 5; i++) {
			ui::Button* btn = (ui::Button*)node->getChildByTag(1000 + i);
			int numb = atoi(btn->getTitleText().c_str());
			if (numb == 1) return;
			btn->setTitleText(to_string(numb - 5));
			if (numb - 5 == nodePage->getTag()) {
				btn->loadTextureNormal("bg_page_number_active.png", ui::Widget::TextureResType::PLIST);
			} else {
				btn->loadTextureNormal("bg_page_number.png", ui::Widget::TextureResType::PLIST);
			}
		}
	});
	node->addChild(btnPre);

	ui::Button* btnNext = ui::Button::create("btn_page_number_next.png", "", "", ui::Widget::TextureResType::PLIST);
	btnNext->setPosition(Vec2(x + 220, y));
	addTouchEventListener(btnNext, [=]() {
		for (int i = 1; i <= 5; i++) {
			ui::Button* btn = (ui::Button*)node->getChildByTag(1000 + i);
			int numb = atoi(btn->getTitleText().c_str());
			if (numb == 995) return;
			btn->setTitleText(to_string(numb + 5));
			if (numb + 5 == nodePage->getTag()) {
				btn->loadTextureNormal("bg_page_number_active.png", ui::Widget::TextureResType::PLIST);
			} else {
				btn->loadTextureNormal("bg_page_number.png", ui::Widget::TextureResType::PLIST);
			}
		}
	});
	node->addChild(btnNext);
}

void BaseScene::setSplashZOrder(int zorder)
{
	if (zorder == splash->getLocalZOrder() || !splash->isVisible()) return;
	bool increase = zorder > splash->getLocalZOrder();
	splash->setLocalZOrder(zorder);
	if (increase) {
		for (ui::Button* btn : buttons) {
			if (!btn->isTouchEnabled()) continue;
			Node* n = btn;
			while (n->getParent() != mLayer) {
				n = n->getParent();
			}
			if (n->isVisible() && n->getLocalZOrder() < splash->getLocalZOrder()) {
				btn->setTouchEnabled(false);
				blockedButtons.push_back(btn);
			}
		}
	} else {
		int i = blockedButtons.size() - 1;
		while (i >= 0) {
			Node* n = blockedButtons[i];
			while (n->getParent() != mLayer) {
				n = n->getParent();
			}
			if (n->getLocalZOrder() >= splash->getLocalZOrder()) {
				blockedButtons[i]->setTouchEnabled(true);
				//blockedButtons.erase(blockedButtons.begin() + i);
				blockedButtons.pop_back();
			}
			i--;
		}
	}
}

void BaseScene::autoScaleNode(cocos2d::Node * node)
{
	if (scaleScene.x < 1) {
		node->setScaleY(node->getScaleY() * scaleScene.x);
	} else if (scaleScene.y < 1) {
		node->setScaleX(node->getScaleX() * scaleScene.y);
	}
}

void BaseScene::delayFunction(Node * node, float time, std::function<void()> func)
{
    DelayTime* delay = DelayTime::create(time);
    CallFunc* callfunc = CallFunc::create(func);
    node->runAction(Sequence::create(delay, callfunc, nullptr));
}
