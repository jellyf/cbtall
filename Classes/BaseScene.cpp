#include "BaseScene.h"
#include "EventHandler.h"
#include "SFSRequest.h"
#include "Utils.h"
#include "Constant.h"
#include "AudioEngine.h"
#include "SFSResponse.h"
#include "Tracker.h"
#include "SFSGEvent.h"

using namespace cocos2d;
using namespace std;

bool CoverLayer::init() {
	if (!cocos2d::LayerColor::initWithColor(cocos2d::Color4B(0, 0, 0, 0)))
	{
		return false;
	}

	auto director = cocos2d::Director::getInstance();
	cocos2d::Size size = director->getWinSize();

	this->setContentSize(size);
	this->setOpacity(128);

	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(CoverLayer::onTouch, this);
	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
	return true;
}

bool CoverLayer::onTouch(cocos2d::Touch* touch, cocos2d::Event* event)
{
	return this->isVisible();
}

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
	if (visibleSize.width < 1120) {
		scaleScene.x = visibleSize.width / 1120;
		mLayer->setScaleX(scaleScene.x);
	} else if (visibleSize.height < 700) {
		scaleScene.y = visibleSize.height / 700;
		mLayer->setScaleY(scaleScene.y);
	}

	/*splash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	splash->setContentSize(Size(1500, 1000));
	splash->setPosition(560, 350);
	splash->setColor(Color3B::BLACK);
	splash->setOpacity(150);
	splash->setVisible(false);
	mLayer->addChild(splash);*/

	splash = CoverLayer::create();
	splash->setVisible(false);
	mLayer->addChild(splash);

	Node* nodeWaiting = Node::create();
	nodeWaiting->setPosition(560, 350);
	nodeWaiting->setVisible(false);
	mLayer->addChild(nodeWaiting, constant::ZORDER_POPUP);
	autoScaleNode(nodeWaiting);

	spWaiting = Sprite::createWithSpriteFrameName("loadingicon.png");
	nodeWaiting->addChild(spWaiting);

	RotateBy* rotate = RotateBy::create(2, 360);
	spWaiting->runAction(RepeatForever::create(rotate));
	spWaiting->pauseSchedulerAndActions();

	spNetwork = Sprite::createWithSpriteFrameName("wifi0.png");
	//spNetwork->setAnchorPoint(Vec2(1, 0));
	//spNetwork->setPosition(1115, 5);
	spNetwork->setPosition(winSize.width - (ispmE ? 360 : 240) * scaleScene.y, 650);
	//spNetwork->setVisible(false);
	mLayer->addChild(spNetwork, constant::GAME_ZORDER_SPLASH - 1);
	autoScaleNode(spNetwork);

	lbNetwork = Label::create("0ms", "fonts/arial.ttf", 18);
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
	processCachedErrors();
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
	EventHandler::getSingleton().onConnectionLost = std::bind(&BaseScene::onConnectionLost, this, std::placeholders::_1);
	EventHandler::getSingleton().onLoginZone = bind(&BaseScene::onLoginZone, this);
	EventHandler::getSingleton().onLoginZoneError = bind(&BaseScene::onLoginZoneError, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onJoinRoom = bind(&BaseScene::onJoinRoom, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onJoinRoomError = bind(&BaseScene::onJoinRoomError, this, placeholders::_1);
	EventHandler::getSingleton().onUserDataMeSFSResponse = std::bind(&BaseScene::onUserDataMeResponse, this);
	EventHandler::getSingleton().onRankDataSFSResponse = std::bind(&BaseScene::onRankDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onRankWinDataSFSResponse = std::bind(&BaseScene::onRankWinDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onListEventDataSFSResponse = std::bind(&BaseScene::onListEventDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onPlayLogDataSFSResponse = bind(&BaseScene::onPlayLogDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onErrorSFSResponse = bind(&BaseScene::onErrorResponse, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onCofferMoneySFSResponse = bind(&BaseScene::onCofferMoneyResponse, this, placeholders::_1);
	EventHandler::getSingleton().onCofferHistorySFSResponse = bind(&BaseScene::onCofferHistoryResponse, this, placeholders::_1);
	EventHandler::getSingleton().onDownloadedPlistTexture = bind(&BaseScene::onDownloadedPlistTexture, this, placeholders::_1);
	EventHandler::getSingleton().onTourWinnersSFSResponse = bind(&BaseScene::onTourWinnersResponse, this, placeholders::_1);
	EventHandler::getSingleton().onTourRoomToJoinSFSResponse = bind(&BaseScene::onTourRoomToJoin, this, placeholders::_1);
	EventHandler::getSingleton().onTourInfoSFSResponse = bind(&BaseScene::onTourInfoResponse, this, placeholders::_1);

	SFSRequest::getSingleton().mapHttpResponseFailedCallbacks["scene"] = std::bind(&BaseScene::onHttpResponseFailed, this, std::placeholders::_1);
	SFSRequest::getSingleton().mapHttpResponseCallbacks["scene"] = std::bind(&BaseScene::onHttpResponse, this, std::placeholders::_1, std::placeholders::_2);
}

void BaseScene::unregisterEventListenner()
{
	EventHandler::getSingleton().onApplicationDidEnterBackground = NULL;
	EventHandler::getSingleton().onApplicationWillEnterForeground = NULL;
	EventHandler::getSingleton().onPingPong = NULL;
	EventHandler::getSingleton().onConnectionFailed = NULL;
	EventHandler::getSingleton().onConnectionLost = NULL;
	EventHandler::getSingleton().onLoginZone = NULL;
	EventHandler::getSingleton().onLoginZoneError = NULL;
	EventHandler::getSingleton().onJoinRoom = NULL;
	EventHandler::getSingleton().onJoinRoomError = NULL;
	EventHandler::getSingleton().onUserDataMeSFSResponse = NULL;
	EventHandler::getSingleton().onRankDataSFSResponse = NULL;
	EventHandler::getSingleton().onRankWinDataSFSResponse = NULL;
	EventHandler::getSingleton().onListEventDataSFSResponse = NULL;
	EventHandler::getSingleton().onPlayLogDataSFSResponse = NULL;
	EventHandler::getSingleton().onErrorSFSResponse = NULL;
	EventHandler::getSingleton().onCofferMoneySFSResponse = NULL;
	EventHandler::getSingleton().onCofferHistorySFSResponse = NULL;
	EventHandler::getSingleton().onDownloadedPlistTexture = NULL;
	EventHandler::getSingleton().onTourWinnersSFSResponse = NULL;
	EventHandler::getSingleton().onTourRoomToJoinSFSResponse = NULL;
	EventHandler::getSingleton().onTourInfoSFSResponse = NULL;

	SFSRequest::getSingleton().mapHttpResponseFailedCallbacks.erase("scene");
	SFSRequest::getSingleton().mapHttpResponseCallbacks.erase("scene");
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
	if (connectionKeeper) connectionKeeper->pause();
}

void BaseScene::onApplicationWillEnterForeground()
{
	spNetwork->resume();
	lbNetwork->resume();
	if (connectionKeeper) connectionKeeper->resume();
}

void BaseScene::showPopupNotice(std::string msg, std::function<void()> func, bool showBtnClose, int timeToHide)
{
	Node* popupNotice = createPopupNotice();
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
		DelayTime* delay = DelayTime::create(timeToHide);
		CallFunc* funcHide = CallFunc::create([=]() {
			hidePopup(popupNotice);
		});
		popupNotice->runAction(Sequence::createWithTwoActions(delay, funcHide));
	}
}

void BaseScene::showPopupNoticeMini(std::string msg, std::function<void()> func, Vec2 pos, bool showBtnClose, int timeToHide)
{
	Node* popupNotice = createPopupNoticeMini();
	popupNotice->setPosition(pos);
	runEffectShowPopup(popupNotice);
	popupNotice->setVisible(true);

	Label* lbcontent = (Label*)popupNotice->getChildByName("lbcontent");
	lbcontent->setString(msg);
	ui::Button* btnClose = (ui::Button*)popupNotice->getChildByName("btnclose");
	btnClose->setVisible(showBtnClose);
	ui::Button* btnSubmit = (ui::Button*)popupNotice->getChildByName("btnsubmit");
	addTouchEventListener(btnSubmit, [=]() {
		popupNotice->stopAllActions();
		func();
		runEffectHidePopup(popupNotice);
	}, false);

	if (timeToHide > 0) {
		DelayTime* delay = DelayTime::create(timeToHide);
		CallFunc* funcHide = CallFunc::create([=]() {
			runEffectHidePopup(popupNotice);
		});
		popupNotice->runAction(Sequence::createWithTwoActions(delay, funcHide));
	}
}

void BaseScene::showPopupConfirm(std::string msg, std::string titleOK, std::string titleCancel, std::function<void()> func)
{
	Node* popupConfirm = createPopupConfirm();
	showPopup(popupConfirm);
	Label* lbcontent = (Label*)popupConfirm->getChildByName("lbcontent");
	lbcontent->setString(msg);
	Label* lbBtnOk = (Label*)popupConfirm->getChildByName("btnsubmit")->getChildByName("btnoktitle");
	lbBtnOk->setString(titleOK);
	Label* lbBtnCancel = (Label*)popupConfirm->getChildByName("btnclose")->getChildByName("btncanceltitle");
	lbBtnCancel->setString(titleCancel);
	ui::Button* btnSubmit = (ui::Button*)popupConfirm->getChildByName("btnsubmit");
	addTouchEventListener(btnSubmit, [=]() {
		popupConfirm->stopAllActions();
		func();
		hidePopup(popupConfirm);
	}, false);
}

void BaseScene::showPopupConfirmMini(std::string msg, std::string titleOK, std::string titleCancel, cocos2d::Vec2 pos, std::function<void()> func)
{
	Node* popupConfirm = createPopupConfirmMini();
	popupConfirm->setPosition(pos);
	runEffectShowPopup(popupConfirm);
	popupConfirm->setVisible(true);

	Label* lbcontent = (Label*)popupConfirm->getChildByName("lbcontent");
	lbcontent->setString(msg);
	Label* lbBtnOk = (Label*)popupConfirm->getChildByName("btnsubmit")->getChildByName("btnoktitle");
	lbBtnOk->setString(titleOK);
	Label* lbBtnCancel = (Label*)popupConfirm->getChildByName("btnclose")->getChildByName("btncanceltitle");
	lbBtnCancel->setString(titleCancel);
	ui::Button* btnSubmit = (ui::Button*)popupConfirm->getChildByName("btnsubmit");
	addTouchEventListener(btnSubmit, [=]() {
		popupConfirm->stopAllActions();
		func();
		hidePopup(popupConfirm);
	}, false);
}

void BaseScene::showSplash()
{
	splash->setVisible(true);
}

void BaseScene::showWaiting(int time)
{
	if (spWaiting == NULL) return;
	isWaiting = true;
	showPopup(spWaiting->getParent(), false);
	spWaiting->resumeSchedulerAndActions();
	spWaiting->getParent()->stopAllActions();

	DelayTime* delay = DelayTime::create(time);
	CallFunc* func = CallFunc::create([=]() {
		if (isWaiting) {
			hideWaiting();
			showPopupNotice(Utils::getSingleton().getStringForKey("connection_failed"), [=]() {
				if (SFSRequest::getSingleton().IsConnected()) {
					isBackToLogin = true;
					SFSRequest::getSingleton().Disconnect();
				} else {
					Utils::getSingleton().goToLoginScene();
				}
			}, false);
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
			lb = Label::create("", "fonts/arial.ttf", 25);
			lb->setAnchorPoint(Vec2(0, .5f));
			lb->setTag(i);
			nodelb->addChild(lb);
			autoScaleNode(lb);
		} else {
			lb->setVisible(true);
		}
		lb->setString(list[i].Content);
		lb->setPosition(x, 20);
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
	btn->loadTextureNormal("btn_light.png", ui::Widget::TextureResType::PLIST);
	btn1->setTitleColor(Color3B::WHITE);
	btn->setTitleColor(Color3B::BLACK);
	popupRank->setTag(type);

	if (type == 2) {
		showPopupRankWin();
		return;
	}

	ui::ScrollView* scroll = (ui::ScrollView*)popupRank->getChildByName("scroll");
	ui::ScrollView* scrollWin = (ui::ScrollView*)popupRank->getChildByName("scrollwin");
	scroll->setVisible(true);
	scrollWin->setVisible(false);
	int height = listRanks[type].size() * 55;
	int width = scroll->getContentSize().width;
	scroll->setInnerContainerSize(Size(width, height));
	vector<Color3B> colors = { Color3B::RED, Color3B(255, 102, 0), Color3B(255, 153, 51) };
	//vector<Color3B> colors = { Color3B::YELLOW, Color3B(255, 255, 102), Color3B(255, 255, 153) };
	for (int i = 0; i < listRanks[type].size(); i++) {
		Node* node = scroll->getChildByTag(i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scroll->getContentSize().width / 2, height - 30 - i * 55);
			node->setTag(i);
			scroll->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
			nbg->setContentSize(Size(width, 65));
			node->addChild(nbg);

			Label* lb1 = Label::create(to_string(i + 1), "fonts/aristote.ttf", 25);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(i < 3 ? colors[i] : Color3B::WHITE);
			lb1->setPosition(-width / 2 + 70, 5);
			lb1->setTag(1);
			node->addChild(lb1);

			Label* lb2 = Label::create(listRanks[type][i].Name, "fonts/arialbd.ttf", 25);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setColor(i < 3 ? colors[i] : Color3B::WHITE);
			lb2->setPosition(-width / 2 + 220, 0);
			lb2->setWidth(400);
			lb2->setHeight(30);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::create(Utils::getSingleton().formatMoneyWithComma(listRanks[type][i].Money), "fonts/arialbd.ttf", 25);
			lb3->setAnchorPoint(Vec2(1, .5f));
			lb3->setColor(type == 0 ? Color3B::YELLOW : Color3B(0, 255, 255));
			lb3->setPosition(width / 2 - 80, 0);
			lb3->setTag(3);
			node->addChild(lb3);
		} else {
			node->setVisible(true);
			Label* lb2 = (Label*)node->getChildByTag(2);
			Label* lb3 = (Label*)node->getChildByTag(3);
			lb2->setString(listRanks[type][i].Name);
			lb3->setString(Utils::getSingleton().formatMoneyWithComma(listRanks[type][i].Money));
			lb3->setColor(type == 0 ? Color3B::YELLOW : Color3B(0, 255, 255));
		}
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
	int height = listRankWin.size() * 75;
	int width = scrollWin->getContentSize().width;
	if (height < scrollWin->getContentSize().height) {
		height = scrollWin->getContentSize().height;
	}
	scrollWin->setInnerContainerSize(Size(width, height));
	vector<Color3B> colors = { Color3B::RED, Color3B(255, 102, 0), Color3B(255, 153, 51) };
	for (int i = 0; i < listRankWin.size(); i++) {
		Label* lb2;
		Node* node = scrollWin->getChildByTag(i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scrollWin->getContentSize().width / 2, height - 40 - i * 75);
			node->setTag(i);
			scrollWin->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
			nbg->setContentSize(Size(width, 85));
			node->addChild(nbg);

			Label* lb1 = Label::create(to_string(i + 1), "fonts/aristote.ttf", 25);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(i < 3 ? colors[i] : Color3B::WHITE);
			lb1->setPosition(-width / 2 + 10, 5);
			lb1->setTag(1);
			node->addChild(lb1);

			lb2 = Label::create(listRankWin[i].Name, "fonts/arialbd.ttf", 22);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setColor(i < 3 ? colors[i] : Color3B::WHITE);
			lb2->setPosition(-width / 2 + 70, 0);
			//lb2->setWidth(400);
			//lb2->setHeight(30);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::create(listRankWin[i].Cuoc, "fonts/arialbd.ttf", 22);
			lb3->setAnchorPoint(Vec2(0, .5f));
			lb3->setColor(Color3B(255, 255, 102));
			lb3->setPosition(-width / 2 + 300, 0);
			lb3->setWidth(450);
			//lb3->setHeight(30);
			lb3->setTag(3);
			node->addChild(lb3);

			Label* lb4 = Label::create(Utils::getSingleton().formatMoneyWithComma(listRankWin[i].Point), "fonts/arialbd.ttf", 22);
			lb4->setAnchorPoint(Vec2(1, .5f));
			lb4->setColor(Color3B(102, 255, 51));
			lb4->setPosition(width / 2 - 30, 0);
			lb4->setTag(4);
			node->addChild(lb4);
		} else {
			node->setVisible(true);
			lb2 = (Label*)node->getChildByTag(2);
			Label* lb3 = (Label*)node->getChildByTag(3);
			Label* lb4 = (Label*)node->getChildByTag(4);
			lb2->setString(listRankWin[i].Name);
			lb3->setString(listRankWin[i].Cuoc);
			lb4->setString(Utils::getSingleton().formatMoneyWithComma(listRankWin[i].Point));
		}

		string strName = listRankWin[i].Name;
		/*if (strName.length() > 14) {
			strName = strName.substr(0, 14);
		}*/
		lb2->setString(strName);
		while (lb2->getContentSize().width > 200) {
			strName = strName.substr(0, strName.length() - 1);
			lb2->setString(strName);
		}
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
	lbAppellation->setColor(aplData.Color);
	//lbBigWin->setString(Utils::getSingleton().formatMoneyWithComma(data.BigWin));
	//lbBigCrest->setString(data.BigCrest);
	nodeInfo->setPosition(isMe ? lbName->getPosition() - Vec2(0, 45) : lbName->getPosition() + Vec2(0, 20));
}

void BaseScene::setMoneyType(int type)
{
	moneyBg0->setTag(type);
	moneyBg1->setTag(type);
	chosenBg->setPosition(type == 0 ? 100 : -100, 0);
	//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), type == 1);
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
	/*if (isNew) {
		buttons.push_back(btn);
	}*/
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

bool BaseScene::onErrorResponse(unsigned char code, std::string msg)
{
	CCLOG("%d %s", (int)code, msg.c_str());
	if (code == 34) {
		//Bat dau dang ky giai dau
		Utils::getSingleton().tourInfo.CanRegister = true;
		if (getTag() == constant::SCENE_GAME) {
			showPopupConfirmMini(msg, Utils::getSingleton().getStringForKey("dang_ky"), 
				Utils::getSingleton().getStringForKey("bo_qua"), Vec2(200, 200), [=]() {
				Utils::getSingleton().tourInfo.CanRegister = false;
				SFSRequest::getSingleton().RequestRegisterTour();
			});
		} else {
			showPopupConfirm(msg, Utils::getSingleton().getStringForKey("dang_ky"), 
				Utils::getSingleton().getStringForKey("bo_qua"), [=]() {
				if (popupTour && popupTour->isVisible()) {
					registerTour();
				} else {
					showPopupTour();
					popupTour->getChildByName("btnjoin")->setVisible(false);
					ui::Button* btnReg = (ui::Button*)popupTour->getChildByName("btnregister");
					btnReg->setVisible(true);
					btnReg->setColor(Color3B::WHITE);
					btnReg->setTouchEnabled(true);
				}
			});
		}
		if (popupTour) {
			popupTour->getChildByName("btnjoin")->setVisible(false);
			ui::Button* btnReg = (ui::Button*)popupTour->getChildByName("btnregister");
			btnReg->setVisible(true);
			btnReg->setColor(Color3B::WHITE);
			btnReg->setTouchEnabled(true);
		}
		return true;
	}
	if (code == 37) {
		//Bat dau tham gia giai dau
		if (getTag() == constant::SCENE_GAME) {
			showPopupConfirmMini(msg, Utils::getSingleton().getStringForKey("tham_gia"), 
				Utils::getSingleton().getStringForKey("bo_qua"), Vec2(200, 200), [=]() {
				joinIntoTour();
			});
		} else {
			showPopupConfirm(msg, Utils::getSingleton().getStringForKey("tham_gia"),
				Utils::getSingleton().getStringForKey("bo_qua"), [=]() {
				joinIntoTour();
			});
		}
		if (popupTour) {
			ui::Button* btnJoin = (ui::Button*)popupTour->getChildByName("btnjoin");
			btnJoin->setVisible(true);
			btnJoin->setColor(Color3B::WHITE);
			btnJoin->setTouchEnabled(true);
			btnJoin->getChildByTag(0)->setColor(Color3B(255, 255, 80));
		}
		return true;
	}
	if (code == 38) {
		isOverlapLogin = true;
	}
	if (isJoiningTour) {
		Utils::getSingleton().goToLobbyScene();
		return true;
	}
	return false;
}

void BaseScene::onConnected()
{
	if (isReconnecting) {
		Utils::getSingleton().reloginZone();
	} else if (isJoiningTour) {
		Utils::getSingleton().loginZoneByIndex(1, tmpZoneId);
		tmpZoneId = -1;
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

bool BaseScene::onConnectionLost(std::string reason)
{
	if (isJoiningTour) {
		Utils::getSingleton().connectZoneByIndex(1, tmpZoneId);
		return true;
	}
	return false;
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
	} else if (isJoiningTour) {
		isJoiningTour = false;
		showWaiting();
		isPauseApp = true;
		Utils::getSingleton().currentZoneName = zoneBeforeTour;
		SFSRequest::getSingleton().Disconnect();
		showPopupNotice(msg, [=]() {});
	} else {

	}
}

void BaseScene::initHeaderWithInfos()
{
	hasHeader = true;
	bool isRealMoney = Utils::getSingleton().moneyType == 1;
	bool ispmE = Utils::getSingleton().ispmE();
	bool isTour = Utils::getSingleton().isTourGame();

	std::vector<Vec2> vecPos;
	vecPos.push_back(Vec2(50 * scaleScene.y, 650));
	vecPos.push_back(Vec2(515, 650));
	vecPos.push_back(Vec2(430, 650));
	vecPos.push_back(Vec2(165, 650));
	vecPos.push_back(Vec2(360, 650));
	vecPos.push_back(Vec2(winSize.width - 145 * scaleScene.y, 650));
	vecPos.push_back(Vec2(winSize.width - 50 * scaleScene.y, 650));
	vecPos.push_back(Vec2(145 * scaleScene.y, 650));
	vecPos.push_back(Vec2(-147, 0));
	vecPos.push_back(Vec2(55, 0));
	vecPos.push_back(Vec2(190 * scaleScene.y, 675));
	vecPos.push_back(Vec2(190 * scaleScene.y, 647));
	vecPos.push_back(Vec2(190 * scaleScene.y, 624));
	vecPos.push_back(Vec2(winSize.width - 260 * scaleScene.y, 645));
	vecPos.push_back(Vec2(720, 647));

	ui::Button* btnBack = ui::Button::create("btn_back.png", "btn_back_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnBack->setPosition(vecPos[0]);
	addTouchEventListener(btnBack, [=]() {
		onBackScene();
	});
	mLayer->addChild(btnBack, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnBack);

	Node* moneyNode = Node::create();
	moneyNode->setPosition(vecPos[1]);
	mLayer->addChild(moneyNode, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(moneyNode);

	moneyBg0 = ui::Button::create("money_bg.png", "money_bg.png", "", ui::Widget::TextureResType::PLIST);
	moneyBg0->setTag((int)isRealMoney);
	moneyBg0->setPosition(Vec2(100, 0));
	moneyNode->addChild(moneyBg0, 0);

	moneyBg1 = ui::Button::create("money_bg.png", "money_bg.png", "", ui::Widget::TextureResType::PLIST);
	moneyBg1->setTag((int)isRealMoney);
	moneyBg1->setPosition(Vec2(-100, 0));
	moneyNode->addChild(moneyBg1, 0);

	chosenBg = Sprite::createWithSpriteFrameName("chosen_bg.png");
	chosenBg->setPosition(isRealMoney ? -100 : 100, 0);
	moneyNode->addChild(chosenBg, 1);
	
	moneyBg0->setBright(false);
	moneyBg1->setBright(false);
	addTouchEventListener(moneyBg0, [=]() {
		switchMoneyType(0);
	});
	addTouchEventListener(moneyBg1, [=]() {
		switchMoneyType(1);
	});

	Sprite* iconGold = Sprite::createWithSpriteFrameName(ispmE ? "icon_gold.png" : "empty.png");
	iconGold->setPosition(-170, 0);
	moneyNode->addChild(iconGold, 2);

	Sprite* iconSilver = Sprite::createWithSpriteFrameName("icon_silver.png");
	iconSilver->setPosition(30, 0);
	moneyNode->addChild(iconSilver, 2);

	/*ui::Button* btnIAP = ui::Button::create("icon_charge.png", "", "", ui::Widget::TextureResType::PLIST);
	btnIAP->setPosition(vecPos[14]);
	btnIAP->setScale(.8f);
	addTouchEventListener(btnIAP, [=]() {
		if (popupIAP == nullptr) {
			initPopupIAP();
		}
		showPopup(popupIAP);
	});
	mLayer->addChild(btnIAP, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnIAP);*/

	ui::Button* btnRank = ui::Button::create("btn_rank.png", "btn_rank_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnRank->setPosition(vecPos[5]);
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

	ui::Button* btnSettings = ui::Button::create("btn_settings.png", "btn_settings_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnSettings->setPosition(vecPos[6]);
	addTouchEventListener(btnSettings, [=]() {
		if (popupMainSettings == nullptr) {
			initPopupSettings();
		}
		showPopup(popupMainSettings);
	});
	mLayer->addChild(btnSettings, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnSettings);

	ui::Button* btnAvar = ui::Button::create("avatar.png", "avatar.png", "", ui::Widget::TextureResType::PLIST);
	btnAvar->setPosition(vecPos[7]);
	btnAvar->setScale(.9f);
	addTouchEventListener(btnAvar, [=]() {
		showPopupUserInfo(Utils::getSingleton().userDataMe);
	});
	mLayer->addChild(btnAvar, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnAvar);

	bgNoted = Sprite::createWithSpriteFrameName("circle_red.png");
	bgNoted->setPosition(btnAvar->getPositionX() + 27, btnAvar->getPositionY() + 30);
	bgNoted->setScale(.5);
	mLayer->addChild(bgNoted, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(bgNoted);

	Label* lbNoted = Label::createWithTTF("!", "fonts/arialbd.ttf", 35);
	lbNoted->setColor(Color3B::WHITE);
	lbNoted->setPosition(bgNoted->getContentSize().width / 2 - 2, bgNoted->getContentSize().height / 2);
	bgNoted->addChild(lbNoted, 1);

	lbGold = Label::create("0", "fonts/arialbd.ttf", 25);
	lbGold->setAnchorPoint(Vec2(0, .5f));
	lbGold->setPosition(vecPos[8]);
	lbGold->setColor(Color3B::YELLOW);
	moneyNode->addChild(lbGold, 2);

	lbSilver = Label::create("0", "fonts/arialbd.ttf", 25);
	lbSilver->setAnchorPoint(Vec2(0, .5f));
	lbSilver->setPosition(vecPos[9]);
	lbSilver->setColor(Color3B(0, 255, 255));
	moneyNode->addChild(lbSilver, 2);

	lbName = Label::create("Name", "fonts/arialbd.ttf", 23);
	lbName->setAnchorPoint(Vec2(0, .5f));
	lbName->setPosition(vecPos[10]);
	lbName->setWidth(150);
	lbName->setHeight(25);
	mLayer->addChild(lbName, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbName);

	lbId = Label::create("ID: ", "fonts/arialbd.ttf", 23);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setPosition(vecPos[11]);
	mLayer->addChild(lbId, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbId);

    lbLevel = Label::create(Utils::getSingleton().getStringForKey("level") + ": ", "fonts/arialbd.ttf", 23);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setPosition(vecPos[12]);
	mLayer->addChild(lbLevel, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbLevel);

	initCofferView(vecPos[13], constant::MAIN_ZORDER_HEADER);

	ui::Scale9Sprite* spHeader = ui::Scale9Sprite::createWithSpriteFrameName("header.png");
	spHeader->setAnchorPoint(Vec2(0, 1));
	spHeader->setContentSize(Size(1120 / scaleScene.y, 104));
	spHeader->setPosition(0, 700);
	mLayer->addChild(spHeader);

	/*int x = 0;
	while (x < 1120) {
		Sprite* line = Sprite::create("main/line.png");
		line->setPosition(x, vecPos[13].y);
		mLayer->addChild(line, constant::MAIN_ZORDER_HEADER);
		x += line->getContentSize().width;
	}*/

	if (Utils::getSingleton().userDataMe.UserID > 0) {
		onUserDataMeResponse();
	}

	if (!ispmE) {
		chosenBg->setPosition(isRealMoney && ispmE ? -100 : 100, 0);
		moneyNode->setPosition(vecPos[1] + Vec2(-60, 0));
		moneyBg1->setVisible(false);
		iconGold->setVisible(false);
		lbGold->setVisible(false);
		lbCoffer->setVisible(false);
	}
	//btnIAP->setVisible(!ispmE);
	if (isTour) {
		chosenBg->setPosition(-100, 0);
		moneyNode->setPosition(vecPos[1] + Vec2(100, 0));
		moneyBg0->setVisible(false);
		iconSilver->setVisible(false);
		lbSilver->setVisible(false);
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
	btn0->loadTextureNormal("btn_light.png", ui::Widget::TextureResType::PLIST);
	btn1->setTitleColor(Color3B::WHITE);
	btn0->setTitleColor(Color3B::BLACK);
	popupHistory->setTag(moneyType);
	popupHistory->getChildByName("nodepage")->setTag(1);
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = (ui::Button*)popupHistory->getChildByTag(1000 + i);
		btn->setTitleText(to_string(i));
		if (i == 1) {
			btn->setColor(pageColor2);
		} else {
			btn->setColor(pageColor1);
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
	isPauseApp = true;
	SFSRequest::getSingleton().Disconnect();
}

void BaseScene::onJoinRoom(long roomId, std::string roomName)
{
	if (roomName.at(0) == 'g' && roomName.at(2) == 'b') {
		Utils::getSingleton().goToGameScene();
	}
}

void BaseScene::onJoinRoomError(std::string msg)
{
	if (isJoiningTour) {
		Utils::getSingleton().goToLobbyScene();
	} else {
		hideWaiting();
		showPopupNotice(msg, [=]() {}, false);
	}
}

void BaseScene::hideSplash()
{
	if (isWaiting) return;
	/*for (ui::Button* btn : blockedButtons) {
		btn->setTouchEnabled(true);
	}
	blockedButtons.clear();*/
	splash->setVisible(false);
	splash->setLocalZOrder(0);
}

void BaseScene::hideWaiting()
{
	isWaiting = false;
	if (spWaiting == NULL) return;
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

void BaseScene::showToast(std::string tag, ::string msg, Vec2 pos, Color3B textColor, Color3B bgColor, int bgOpacity)
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
	lb->setSystemFontName("Arial");
	lb->setSystemFontSize(25);
	lb->setColor(textColor);
	nodeChat->addChild(lb, 2);

	Size sizeContent = lb->getContentSize();
	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("box5.png");
	bg->setContentSize(sizeContent + Size(20, 10));
	bg->setColor(bgColor);
	bg->setOpacity(bgOpacity);
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

cocos2d::Node * BaseScene::createPopup(std::string stitle, bool isBig, bool isHidden)
{
	if (isHidden && !isPopupReady) {
		showWaiting();
		return nullptr;
	}

	Node* popup = Node::create();
	popup->setPosition(560, 350);
	popup->setVisible(false);
	mLayer->addChild(popup, constant::ZORDER_POPUP);
	//autoScaleNode(popup);

	Sprite* bg = isBig ? Sprite::create("imgs/popup_bg1.png") : Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setName("spbg");
	popup->addChild(bg);

	Sprite* bgTitle = Sprite::createWithSpriteFrameName("title_bg.png");
	bgTitle->setPosition(0, bg->getContentSize().height / 2 - 5);
	bgTitle->setName("spbgtitle");
	popup->addChild(bgTitle);

	Sprite* title = Sprite::createWithSpriteFrameName(stitle);
	title->setPosition(0, bg->getContentSize().height / 2 - 5);
	title->setName("sptitle");
	popup->addChild(title);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(bg->getContentSize().width / 2 - 10, bg->getContentSize().height / 2 - 5));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popup);
	});
	popup->addChild(btnClose);

	return popup;
}

Node* BaseScene::createPopupNotice()
{
	Node* popupNotice = nullptr;
	for (Node* n : vecPopupNotices) {
		if (!n->isVisible()) {
			popupNotice = n;
			break;
		}
	}
	if (popupNotice == nullptr) {
		popupNotice = Node::create();
	}

	popupNotice->setPosition(560, 350);
	popupNotice->setVisible(false);
	mLayer->addChild(popupNotice, constant::ZORDER_POPUP_NOTICE);
	autoScaleNode(popupNotice);

	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	popupNotice->addChild(bg);

	Sprite* title = Sprite::createWithSpriteFrameName("title_thongbao.png");
	title->setPosition(0, 170);
	//title->setScale(.8f);
	popupNotice->addChild(title);

	Label* lb = Label::create();
	lb->setColor(Color3B::WHITE);
	lb->setSystemFontSize(30);
	lb->setWidth(550);
	lb->setName("lbcontent");
	lb->setAlignment(TextHAlignment::CENTER);
	popupNotice->addChild(lb);

	ui::Button* btnok = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnok->setPosition(Vec2(0, -170));
	btnok->setName("btnsubmit");
	addTouchEventListener(btnok, [=]() {});
	popupNotice->addChild(btnok);

	ui::Button* btndong = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btndong->setPosition(Vec2(310, 170));
	btndong->setScale(.8f);
	btndong->setName("btnclose");
	addTouchEventListener(btndong, [=]() {
		hidePopup(popupNotice);
	});
	popupNotice->addChild(btndong);

	return popupNotice;
}

cocos2d::Node * BaseScene::createPopupNoticeMini()
{
	Node* popupNotice = nullptr;
	for (Node* n : vecPopupNoticeMinis) {
		if (!n->isVisible()) {
			popupNotice = n;
			break;
		}
	}
	if (popupNotice == nullptr) {
		popupNotice = Node::create();
	}

	popupNotice->setPosition(560, 350);
	popupNotice->setVisible(false);
	mLayer->addChild(popupNotice, constant::ZORDER_POPUP_NOTICE);
	autoScaleNode(popupNotice);

	float bgScale = .6f;
	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setScale(bgScale);
	popupNotice->addChild(bg);

	Label* lb = Label::createWithTTF("", "fonts/arial.ttf", 25);
	lb->setAlignment(TextHAlignment::CENTER);
	lb->setColor(Color3B::WHITE);
	lb->setWidth(550 * bgScale);
	lb->setName("lbcontent");
	popupNotice->addChild(lb);

	ui::Button* btnok = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnok->setPosition(Vec2(0, -200 * bgScale));
	btnok->setName("btnsubmit");
	btnok->setScale(.8f);
	addTouchEventListener(btnok, [=]() {});
	popupNotice->addChild(btnok);

	ui::Button* btndong = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btndong->setPosition(Vec2(310 * bgScale, 170 * bgScale));
	btndong->setScale(.6f);
	btndong->setName("btnclose");
	addTouchEventListener(btndong, [=]() {
		runEffectHidePopup(popupNotice);
	});
	popupNotice->addChild(btndong);

	return popupNotice;
}

cocos2d::Node * BaseScene::createPopupConfirm()
{
	Node* popupConfirm = nullptr;
	for (Node* n : vecPopupConfirms) {
		if (!n->isVisible()) {
			popupConfirm = n;
			break;
		}
	}
	if (popupConfirm == nullptr) {
		popupConfirm = Node::create();
	}

	popupConfirm->setPosition(560, 350);
	popupConfirm->setVisible(false);
	mLayer->addChild(popupConfirm, constant::ZORDER_POPUP_NOTICE);
	autoScaleNode(popupConfirm);

	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	popupConfirm->addChild(bg);

	Sprite* title = Sprite::createWithSpriteFrameName("title_thongbao.png");
	title->setPosition(0, 170);
	//title->setScale(.8f);
	popupConfirm->addChild(title);

	Label* lb = Label::create();
	lb->setColor(Color3B::WHITE);
	lb->setSystemFontSize(30);
	lb->setWidth(550);
	lb->setName("lbcontent");
	lb->setAlignment(TextHAlignment::CENTER);
	popupConfirm->addChild(lb);

	ui::Button* btnok = ui::Button::create("btn.png", "btn_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnok->setScale9Enabled(true);
	btnok->setCapInsets(Rect(85, 10, 10, 40));
	btnok->setContentSize(Size(202, 59));
	btnok->setPosition(Vec2(-130, -170));
	btnok->setName("btnsubmit");
	addTouchEventListener(btnok, [=]() {});
	popupConfirm->addChild(btnok);

	ui::Button* btndong = ui::Button::create("btn.png", "btn_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btndong->setScale9Enabled(true);
	btndong->setCapInsets(Rect(85, 10, 10, 40));
	btndong->setContentSize(Size(202, 59));
	btndong->setPosition(Vec2(130, -170));
	btndong->setName("btnclose");
	addTouchEventListener(btndong, [=]() {
		hidePopup(popupConfirm);
	});
	popupConfirm->addChild(btndong);

	Label* okTitle = Label::createWithTTF(Utils::getSingleton().getStringForKey("dang_ky"), "fonts/staccato.ttf", 30);
	okTitle->setPosition(btnok->getContentSize().width / 2, btnok->getContentSize().height / 2 + 5);
	okTitle->setColor(Color3B(255, 255, 80));
	okTitle->enableOutline(Color4B(51, 0, 0, 255), 2);
	okTitle->setName("btnoktitle");
	btnok->addChild(okTitle);

	Label* cancelTitle = Label::createWithTTF(Utils::getSingleton().getStringForKey("bo_qua"), "fonts/staccato.ttf", 30);
	cancelTitle->setPosition(btndong->getContentSize().width / 2, btndong->getContentSize().height / 2 + 5);
	cancelTitle->setColor(Color3B(255, 255, 80));
	cancelTitle->enableOutline(Color4B(51, 0, 0, 255), 2);
	cancelTitle->setName("btncanceltitle");
	btndong->addChild(cancelTitle);

	return popupConfirm;
}

cocos2d::Node * BaseScene::createPopupConfirmMini()
{
	Node* popupConfirm = nullptr;
	for (Node* n : vecPopupConfirmMinis) {
		if (!n->isVisible()) {
			popupConfirm = n;
			break;
		}
	}
	if (popupConfirm == nullptr) {
		popupConfirm = Node::create();
	}

	popupConfirm->setPosition(560, 350);
	popupConfirm->setVisible(false);
	mLayer->addChild(popupConfirm, constant::ZORDER_POPUP_NOTICE);
	autoScaleNode(popupConfirm);

	float bgScale = .6f;
	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setScale(bgScale);
	popupConfirm->addChild(bg);

	Label* lb = Label::create();
	lb->setColor(Color3B::WHITE);
	lb->setSystemFontSize(25);
	lb->setWidth(550 * bgScale);
	lb->setName("lbcontent");
	lb->setAlignment(TextHAlignment::CENTER);
	popupConfirm->addChild(lb);

	ui::Button* btnok = ui::Button::create("btn.png", "btn_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnok->setScale9Enabled(true);
	btnok->setCapInsets(Rect(85, 10, 10, 40));
	btnok->setContentSize(Size(180, 59));
	btnok->setPosition(Vec2(-130 * .7f, -200 * bgScale));
	btnok->setName("btnsubmit");
	btnok->setScale(.8f);
	addTouchEventListener(btnok, [=]() {});
	popupConfirm->addChild(btnok);

	ui::Button* btndong = ui::Button::create("btn.png", "btn_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btndong->setScale9Enabled(true);
	btndong->setCapInsets(Rect(85, 10, 10, 40));
	btndong->setContentSize(Size(180, 59));
	btndong->setPosition(Vec2(130 * .7f, -200 * bgScale));
	btndong->setName("btnclose");
	btndong->setScale(.8f);
	addTouchEventListener(btndong, [=]() {
		runEffectHidePopup(popupConfirm);
	});
	popupConfirm->addChild(btndong);

	Label* okTitle = Label::createWithTTF(Utils::getSingleton().getStringForKey("dang_ky"), "fonts/staccato.ttf", 30);
	okTitle->setPosition(btnok->getContentSize().width / 2, btnok->getContentSize().height / 2 + 5);
	okTitle->setColor(Color3B(255, 255, 80));
	okTitle->enableOutline(Color4B(51, 0, 0, 255), 2);
	okTitle->setName("btnoktitle");
	btnok->addChild(okTitle);

	Label* cancelTitle = Label::createWithTTF(Utils::getSingleton().getStringForKey("bo_qua"), "fonts/staccato.ttf", 30);
	cancelTitle->setPosition(btndong->getContentSize().width / 2, btndong->getContentSize().height / 2 + 5);
	cancelTitle->setColor(Color3B(255, 255, 80));
	cancelTitle->enableOutline(Color4B(51, 0, 0, 255), 2);
	cancelTitle->setName("btncanceltitle");
	btndong->addChild(cancelTitle);

	return popupConfirm;
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

	/*popupRank = Node::create();
	popupRank->setPosition(560, 350);
	popupRank->setVisible(false);
	popupRank->setTag(0);
	mLayer->addChild(popupRank, constant::ZORDER_POPUP);
	autoScaleNode(popupRank);*/

	/*ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupRank->addChild(bg);*/

	/*Sprite* bg = Sprite::create("imgs/popup_bg1.png");
	popupRank->addChild(bg);*/

	Size size = Size(860, 466);
	Size scrollSize = size - Size(20, 20);
	DrawNode* rect = DrawNode::create();
	rect->drawRect(Vec2(-size.width / 2, -size.height / 2), Vec2(size.width / 2, size.height / 2), Color4F::BLACK);
	rect->setPosition(55, -25);
	popupRank->addChild(rect);

	/*Sprite* title = Sprite::createWithSpriteFrameName("title_bangxephang.png");
	title->setPosition(0, bg->getContentSize().height / 2 - 5);
	popupRank->addChild(title);*/

	/*ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(bg->getContentSize().width / 2 - 10, bg->getContentSize().height / 2 - 5));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupRank);
	});
	popupRank->addChild(btnClose);*/

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + rect->getPosition());
	scroll->setContentSize(scrollSize);
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupRank->addChild(scroll);

	ui::ScrollView* scrollWin = ui::ScrollView::create();
	scrollWin->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollWin->setBounceEnabled(true);
	scrollWin->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + rect->getPosition());
	scrollWin->setContentSize(scrollSize);
	scrollWin->setScrollBarEnabled(false);
	scrollWin->setName("scrollwin");
	popupRank->addChild(scrollWin);

	vector<string> texts = { "quan", "xu", "win" };
	int x = -440;
	int y = 180;
	for (int i = pmE ? 0 : 1; i < texts.size(); i++) {
		string strBtn = i == 0 ? "btn_light.png" : "empty.png";
		ui::Button* btn = ui::Button::create(strBtn, strBtn, strBtn, ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/aristote.ttf");
		btn->setTitleFontSize(30);
		btn->setTitleColor(i == 0 ? Color3B::BLACK : Color3B::WHITE);
		btn->setContentSize(Size(205, 50));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(0, 0, 0, 0));
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			showPopupRank(btn->getTag());
		});
		popupRank->addChild(btn);

		/*Label* lb = Label::create(Utils::getSingleton().getStringForKey(texts[i]), "fonts/guanine.ttf", 30);
		lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 5);
		lb->setColor(Color3B::BLACK);
		btn->addChild(lb);*/

		//x += 210;
		y -= 70;
	}
}

void BaseScene::initPopupSettings()
{
	popupMainSettings = createPopup("title_caidat.png", false, false);

	//popupMainSettings = Node::create();
	//popupMainSettings->setPosition(560, 350);
	//popupMainSettings->setVisible(false);
	//mLayer->addChild(popupMainSettings, constant::ZORDER_POPUP);
	//autoScaleNode(popupMainSettings);

	//Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	//popupMainSettings->addChild(bg);

	//Sprite* title = Sprite::createWithSpriteFrameName("title_caidat.png");
	//title->setPosition(0, 170);
	////title->setScale(.8f);
	//popupMainSettings->addChild(title);

	vector<Vec2> vecPos;
	vecPos.push_back(Vec2(-180, 50));
	vecPos.push_back(Vec2(-180, -50));

	vector<Label*> lbs;
	vector<ui::CheckBox*> cbs;
	for (int i = 0; i < 2; i++) {
		ui::CheckBox* checkbox = ui::CheckBox::create();
		checkbox->loadTextureBackGround("box0.png", ui::Widget::TextureResType::PLIST);
		checkbox->loadTextureFrontCross("check.png", ui::Widget::TextureResType::PLIST);
		checkbox->setPosition(vecPos[i]);
		checkbox->setSelected(false);
		checkbox->setTag(i);
		popupMainSettings->addChild(checkbox);
		cbs.push_back(checkbox);

		Label* lb = Label::create();
		lb->setSystemFontName("Arial");
		lb->setSystemFontSize(30);
		lb->setPosition(vecPos[i] + Vec2(40, 0));
		lb->setAnchorPoint(Vec2(0, .5f));
		lb->setColor(Color3B::WHITE);
		popupMainSettings->addChild(lb);
		lbs.push_back(lb);
	}

	cbs[0]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_SOUND.c_str()));
	cbs[1]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_INVITATION.c_str()));

	lbs[0]->setString(Utils::getSingleton().getStringForKey("sound"));
	lbs[1]->setString(Utils::getSingleton().getStringForKey("reject_all_invitation"));

	ui::Button* btnOK = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnOK->setPosition(Vec2(0, -170));
	addTouchEventListener(btnOK, [=]() {
		hidePopup(popupMainSettings);
		Utils::getSingleton().SoundEnabled = cbs[0]->isSelected();
		Utils::getSingleton().IgnoreInvitation = cbs[1]->isSelected();
		UserDefault::getInstance()->setBoolForKey(constant::KEY_SOUND.c_str(), cbs[0]->isSelected());
		UserDefault::getInstance()->setBoolForKey(constant::KEY_INVITATION.c_str(), cbs[1]->isSelected());
	});
	popupMainSettings->addChild(btnOK);

	/*ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(310, 170));
	btnClose->setScale(.8f);
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupMainSettings);
	});
	popupMainSettings->addChild(btnClose);*/
}

void BaseScene::initPopupUserInfo()
{
	bool ispmE = Utils::getSingleton().ispmE();

	popupUserInfo = Node::create();
	popupUserInfo->setPosition(560, 350);
	popupUserInfo->setVisible(false);
	mLayer->addChild(popupUserInfo, constant::ZORDER_POPUP);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setContentSize(Size(800, 550));
	bg->setInsetLeft(300);
	bg->setInsetRight(30);
	bg->setInsetTop(30);
	bg->setInsetBottom(320);
	popupUserInfo->addChild(bg);

	Sprite* bgTitle = Sprite::createWithSpriteFrameName("title_bg.png");
	bgTitle->setPosition(0, bg->getContentSize().height / 2 - 5);
	popupUserInfo->addChild(bgTitle);

	Sprite* title = Sprite::createWithSpriteFrameName("title_thongtin.png");
	title->setPosition(0, bg->getContentSize().height / 2 - 5);
	popupUserInfo->addChild(title);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(bg->getContentSize().width / 2 - 10, bg->getContentSize().height / 2 - 5));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupUserInfo);
	});
	popupUserInfo->addChild(btnClose);

	Sprite* avatar = Sprite::createWithSpriteFrameName("avatar_default.png");
	avatar->setPosition(-255, 100);
	avatar->setScale(1.7f);
	avatar->setName("avatar");
	popupUserInfo->addChild(avatar);

	Label* lbAppellation = Label::create("Huong Truong", "fonts/aristote.ttf", 25);
	lbAppellation->setColor(Color3B::WHITE);
	lbAppellation->setPosition(avatar->getPositionX(), avatar->getPositionY() - 113);
	lbAppellation->setName("lbappellation");
	popupUserInfo->addChild(lbAppellation);

	ui::Button* btnHistory = ui::Button::create("btn_lichsu.png", "btn_lichsu_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnHistory->setPosition(Vec2(lbAppellation->getPositionX(), lbAppellation->getPositionY() - 65));
	btnHistory->setName("btnhistory");
	btnHistory->setScale(.8f);
	addTouchEventListener(btnHistory, [=]() {
		showPopupHistory();
	});
	popupUserInfo->addChild(btnHistory);

	ui::Button* btnActive = ui::Button::create(ispmE ? "btn_kichhoat.png" : "", ispmE ? "btn_kickhoat_clicked.png" : "", "", ui::Widget::TextureResType::PLIST);
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

    ui::Button* btnLogoutFb = ui::Button::create("btn_logout_fb.png", "btn_logout_fb_clicked.png", "", ui::Widget::TextureResType::PLIST);
    btnLogoutFb->setPosition(Vec2(btnActive->getPositionX(), btnActive->getPositionY() - 60));
    btnLogoutFb->setName("btnlogoutfb");
    btnLogoutFb->setScale(.8f);
    addTouchEventListener(btnLogoutFb, [=]() {
		isBackToLogin = true;
        SFSRequest::getSingleton().Disconnect();
        Utils::getSingleton().logoutFacebook();
    });
    popupUserInfo->addChild(btnLogoutFb);

	Sprite* bgNoted = Sprite::createWithSpriteFrameName("circle_red.png");
	bgNoted->setPosition(btnActive->getContentSize().width - 15, btnActive->getContentSize().height - 15);
	bgNoted->setScale(.6);
	btnActive->addChild(bgNoted);

	Label* lbNoted = Label::createWithTTF("!", "fonts/arialbd.ttf", 35);
	lbNoted->setColor(Color3B::WHITE);
	lbNoted->setPosition(bgNoted->getContentSize().width / 2, bgNoted->getContentSize().height / 2);
	bgNoted->addChild(lbNoted, 1);

	int x = 45;
	Label* lbName = Label::create("Stormus", "fonts/arialbd.ttf", 35);
	lbName->setAnchorPoint(Vec2(0, .5f));
	lbName->setColor(Color3B::WHITE);
	lbName->setPosition(-130, 170);
	lbName->setName("lbname");
	popupUserInfo->addChild(lbName);

	Node* nodeInfo = Node::create();
	nodeInfo->setName("nodeinfo");
	nodeInfo->setPosition(lbName->getPositionX(), lbName->getPositionY() - x);
	popupUserInfo->addChild(nodeInfo);

	Label* lbUname = Label::create(Utils::getSingleton().getStringForKey("login") + ":", "fonts/arial.ttf", 25);
	lbUname->setAnchorPoint(Vec2(0, .5f));
	lbUname->setColor(Color3B::WHITE);
	lbUname->setPosition(0, 0);
	lbUname->setName("lbuname");
	nodeInfo->addChild(lbUname);

	Label* lbUname1 = Label::create("stormus", "fonts/arial.ttf", 25);
	lbUname1->setAnchorPoint(Vec2(0, .5f));
	lbUname1->setColor(Color3B::WHITE);
	lbUname1->setPosition(lbUname->getPositionX() + 150, lbUname->getPositionY());
	lbUname1->setName("lbuname1");
	nodeInfo->addChild(lbUname1);

	Label* lbId = Label::create("ID:", "fonts/arial.ttf", 25);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setColor(Color3B::WHITE);
	lbId->setPosition(lbUname->getPositionX(), lbUname->getPositionY() - x);
	lbId->setName("lbid");
	nodeInfo->addChild(lbId);

	Label* lbId1 = Label::create("847558", "fonts/arial.ttf", 25);
	lbId1->setAnchorPoint(Vec2(0, .5f));
	lbId1->setColor(Color3B::WHITE);
	lbId1->setPosition(lbId->getPositionX() + 150, lbId->getPositionY());
	lbId1->setName("lbid1");
	nodeInfo->addChild(lbId1);

	Label* lbQuan = Label::create(Utils::getSingleton().getStringForKey("quan") + ":", "fonts/arial.ttf", 25);
	lbQuan->setAnchorPoint(Vec2(0, .5f));
	lbQuan->setColor(Color3B::WHITE);
	lbQuan->setPosition(lbId->getPositionX(), lbId->getPositionY() - x);
	lbQuan->setVisible(ispmE);
	nodeInfo->addChild(lbQuan);

	Label* lbQuan1 = Label::create("100,000", "fonts/arial.ttf", 25);
	lbQuan1->setAnchorPoint(Vec2(0, .5f));
	lbQuan1->setColor(Color3B::YELLOW);
	lbQuan1->setPosition(lbQuan->getPositionX() + 150, lbQuan->getPositionY());
	lbQuan1->setName("lbquan");
	lbQuan1->setVisible(ispmE);
	nodeInfo->addChild(lbQuan1);

	Label* lbXu = Label::create(Utils::getSingleton().getStringForKey("xu") + ":", "fonts/arial.ttf", 25);
	lbXu->setAnchorPoint(Vec2(0, .5f));
	lbXu->setColor(Color3B::WHITE);
	lbXu->setPosition(lbQuan->getPositionX(), lbQuan->getPositionY() - x);
	nodeInfo->addChild(lbXu);

	Label* lbXu1 = Label::create("100,000", "fonts/arial.ttf", 25);
	lbXu1->setAnchorPoint(Vec2(0, .5f));
	lbXu1->setColor(Color3B(0, 255, 255));
	lbXu1->setPosition(lbXu->getPositionX() + 150, lbXu->getPositionY());
	lbXu1->setName("lbxu");
	nodeInfo->addChild(lbXu1);
	
	/*Label* lbLevel = Label::create(Utils::getSingleton().getStringForKey("cap_do") + ":", "fonts/arial.ttf", 25);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setColor(Color3B::WHITE);
	lbLevel->setPosition(-70, -70);
	lbLevel->setName("lblevel");
	popupUserInfo->addChild(lbLevel);*/

	Label* lbWin = Label::create(Utils::getSingleton().getStringForKey("thang") + ": 12", "fonts/arial.ttf", 25);
	lbWin->setAnchorPoint(Vec2(0, .5f));
	lbWin->setColor(Color3B::WHITE);
	lbWin->setPosition(lbXu->getPositionX(), lbXu->getPositionY() - x);
	lbWin->setName("lbwin");
	nodeInfo->addChild(lbWin);

	Label* lbTotal = Label::create(Utils::getSingleton().getStringForKey("tong") + ": 20", "fonts/arial.ttf", 25);
	lbTotal->setAnchorPoint(Vec2(0, .5f));
	lbTotal->setColor(Color3B::WHITE);
	lbTotal->setPosition(lbWin->getPositionX() + 250, lbWin->getPositionY());
	lbTotal->setName("lbtotal");
	nodeInfo->addChild(lbTotal);

	Label* lbBigWin = Label::create(Utils::getSingleton().getStringForKey("thang_lon_nhat") + ":", "fonts/arial.ttf", 25);
	lbBigWin->setPosition(lbWin->getPositionX(), lbWin->getPositionY() - x);
	lbBigWin->setAnchorPoint(Vec2(0, .5f));
	lbBigWin->setColor(Color3B::WHITE);
	lbBigWin->setVisible(false);
	nodeInfo->addChild(lbBigWin);

	Label* lbBigWin1 = Label::create("1111", "fonts/arial.ttf", 25);
	lbBigWin1->setPosition(lbBigWin->getPositionX() + lbBigWin->getContentSize().width + 10, lbBigWin->getPositionY());
	lbBigWin1->setAnchorPoint(Vec2(0, .5f));
	lbBigWin1->setColor(Color3B::YELLOW);
	lbBigWin1->setName("lbbigwin");
	lbBigWin1->setVisible(false);
	nodeInfo->addChild(lbBigWin1);

	Label* lbBigCrest = Label::create(Utils::getSingleton().getStringForKey("u_to_nhat") + ": Nha lau xe hoi - Hoa roi cua pha t, a a a Ngu ong bat ca, Ca loi san dinh", "fonts/arial.ttf", 25);
	lbBigCrest->setPosition(lbBigWin->getPositionX(), lbBigWin->getPositionY() - x + 20);
	lbBigCrest->setAnchorPoint(Vec2(0, 1));
	lbBigCrest->setColor(Color3B::WHITE);
	lbBigCrest->setWidth(480);
	lbBigCrest->setName("lbbigcrest");
	lbBigCrest->setVisible(false);
	nodeInfo->addChild(lbBigCrest);

	//Label* lbBigCrest1 = Label::create("Nha lau xe hoi - Hoa roi cua pha t, a a a Ngu ong bat ca, Ca loi san dinh", "fonts/arial.ttf", 25);
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

	/*popupHistory = Node::create();
	popupHistory->setPosition(560, 350);
	popupHistory->setVisible(false);
	popupHistory->setTag(0);
	mLayer->addChild(popupHistory, constant::ZORDER_POPUP);
	autoScaleNode(popupHistory);

	Sprite* bg = Sprite::create("imgs/popup_bg1.png");
	popupHistory->addChild(bg);

	Sprite* title = Sprite::createWithSpriteFrameName("title_lichsu.png");
	title->setPosition(0, bg->getContentSize().height / 2 - 5);
	title->setName("sptitle");
	popupHistory->addChild(title);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(bg->getContentSize().width / 2 - 10, bg->getContentSize().height / 2 - 5));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupHistory);
	});
	popupHistory->addChild(btnClose);*/

	/*ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupHistory->addChild(bg);*/

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgContent->setContentSize(Size(860, 446));
	bgContent->setPosition(ispmE ? 55 : 0, -10);
	popupHistory->addChild(bgContent);

	Size size = bgContent->getContentSize();
	DrawNode* rect = DrawNode::create();
	rect->drawRect(Vec2(-size.width / 2, -size.height / 2), Vec2(size.width / 2, size.height / 2), Color4F::BLACK);
	rect->setPosition(bgContent->getPosition());
	popupHistory->addChild(rect);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-size.width / 2 + 5, -size.height / 2 + 5) + bgContent->getPosition());
	scroll->setContentSize(Size(size.width - 10, size.height - 50));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupHistory->addChild(scroll);

	vector<string> texts = { "quan" , "xu" };
	int x = -450;
	int y = 180;
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "btn_light.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/aristote.ttf");
		btn->setTitleFontSize(30);
		btn->setTitleColor(i == 0 ? Color3B::BLACK : Color3B::WHITE);
		btn->setContentSize(Size(205, 50));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(10 + i);
		btn->setVisible(ispmE);
		addTouchEventListener(btn, [=]() {
			if (popupHistory->getTag() == i) return;
			ui::Button* btn1 = (ui::Button*)popupHistory->getChildByTag(10 + popupHistory->getTag());
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("btn_light.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::BLACK);
			popupHistory->setTag(i);
			SFSRequest::getSingleton().RequestPlayHistory(i, 0);

			popupHistory->getChildByName("nodepage")->setTag(1);
			for (int i = 1; i <= 5; i++) {
				ui::Button* btn = (ui::Button*)popupHistory->getChildByTag(1000 + i);
				btn->setTitleText(to_string(i));
				if (i == 1) {
					btn->setColor(pageColor2);
				} else {
					btn->setColor(pageColor1);
				}
			}
		});
		popupHistory->addChild(btn);

		/*Label* lb = Label::create(Utils::getSingleton().getStringForKey(texts[i]), "fonts/guanine.ttf", 30);
		lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 5);
		lb->setColor(Color3B::BLACK);
		btn->addChild(lb);*/

		//x += 210;
		y -= 70;
	}
	
	int py = bgContent->getPositionY() + size.height / 2 - 20;
	int px = bgContent->getPositionX() - size.width / 2 + 40;
	vector<int> posX = { px, px + 90, px + 330, px + 550, px + 680, px + 770 };
	vector<int> widths = { 50, 100, 320, 210, 150, 60 };
	vector<string> historyTitles = { "STT", Utils::getSingleton().getStringForKey("ngay"), Utils::getSingleton().getStringForKey("thong_tin"),
		Utils::getSingleton().getStringForKey("tien_van"), Utils::getSingleton().getStringForKey("tien"), "ID" };
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lb = Label::create(historyTitles[i], "fonts/aurora.ttf", 30);
		lb->setColor(Color3B::WHITE);
		lb->setPosition(posX[i], py);
		popupHistory->addChild(lb);
	}

	Node* nodeDetail = Node::create();
	nodeDetail->setPosition(scroll->getPosition() + Vec2(size.width / 2 - 5, size.height / 2 - 25));
	nodeDetail->setVisible(false);
	nodeDetail->setName("nodedetail");
	popupHistory->addChild(nodeDetail);

	/*ui::Scale9Sprite* bgDetail = ui::Scale9Sprite::createWithSpriteFrameName("box1.png");
	bgDetail->setContentSize(size - Size(0, 50));
	nodeDetail->addChild(bgDetail);*/

	ui::Scale9Sprite* bgDetail = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bgDetail->setInsetBottom(320);
	bgDetail->setInsetTop(30);
	bgDetail->setInsetLeft(300);
	bgDetail->setInsetRight(30);
	bgDetail->setContentSize(size - Size(-10, 30));
	nodeDetail->addChild(bgDetail);

	for (int i = 0; i < posX.size(); i++) {
		Label* lbDetail = Label::create("s", "fonts/arial.ttf", 20);
		lbDetail->setWidth(widths[i]);
		lbDetail->setAnchorPoint(Vec2(.5f, 1));
		lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
        lbDetail->setPosition(posX[i] - (ispmE ? 55 : 0), bgDetail->getContentSize().height / 2 - 15);
		lbDetail->setTag(i);
		lbDetail->setColor(Color3B::WHITE);
		nodeDetail->addChild(lbDetail);
	}

	ui::Button* btnCloseDetail = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnCloseDetail->setPosition(Vec2(bgDetail->getContentSize().width / 2 - 45, -bgDetail->getContentSize().height / 2 + 45));
	btnCloseDetail->setScale(.7f);
	addTouchEventListener(btnCloseDetail, [=]() {
		nodeDetail->setVisible(false);
	});
	nodeDetail->addChild(btnCloseDetail);

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

	/*popupCoffer = Node::create();
	popupCoffer->setPosition(560, 350);
	popupCoffer->setVisible(false);
	popupCoffer->setTag(0);
	mLayer->addChild(popupCoffer, constant::ZORDER_POPUP);*/

	/*ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupCoffer->addChild(bg);*/

	/*Sprite* bg = Sprite::create("imgs/popup_bg1.png");
	popupCoffer->addChild(bg);*/

	Size size = Size(780, 466);
	Size scrollSize = size - Size(20, 20);
	DrawNode* rect = DrawNode::create();
	rect->drawRect(Vec2(-size.width / 2, -size.height / 2), Vec2(size.width / 2, size.height / 2), Color4F::BLACK);
	rect->setPosition(95, -25);
	popupCoffer->addChild(rect);

	/*Sprite* title = Sprite::createWithSpriteFrameName("title_huvang.png");
	title->setPosition(0, bg->getContentSize().height / 2 - 5);
	popupCoffer->addChild(title);*/

	/*ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(bg->getContentSize().width / 2 - 10, bg->getContentSize().height / 2 - 5));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupCoffer);
	});
	popupCoffer->addChild(btnClose);*/

	Node* nodeGuide = Node::create();
	nodeGuide->setName("nodeguide");
	nodeGuide->setTag(0);
	popupCoffer->addChild(nodeGuide);

	Node* nodeHistory = Node::create();
	nodeHistory->setName("nodehistory");
	nodeHistory->setTag(1);
	nodeHistory->setVisible(false);
	popupCoffer->addChild(nodeHistory);

	vector<string> texts = { "huong_dan" , "lich_su_no" };
	int x = -400;
	int y = 180;
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "btn_light.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/aristote.ttf");
		btn->setTitleFontSize(25);
		btn->setTitleColor(i == 0 ? Color3B::BLACK : Color3B::WHITE);
		btn->setContentSize(Size(255, 50));
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
			btn->loadTextureNormal("btn_light.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::BLACK);
			popupCoffer->setTag(i);
		});
		popupCoffer->addChild(btn);
		//x += 210;
		y -= 70;
	}

	//Node guide
	/*ui::Scale9Sprite* bgScrollGuide = ui::Scale9Sprite::createWithSpriteFrameName("box1.png");
	bgScrollGuide->setContentSize(size);
	bgScrollGuide->setPosition(95, -25);
	nodeGuide->addChild(bgScrollGuide);*/

	ui::ScrollView* scrollGuide = ui::ScrollView::create();
	scrollGuide->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollGuide->setBounceEnabled(true);
	scrollGuide->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + rect->getPosition());
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
	Label* lb = Label::create(guideContent, "fonts/arial.ttf", 20);
	lb->setAnchorPoint(Vec2(0, 1));
	lb->setColor(Color3B::WHITE);
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
	scrollHistory->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + rect->getPosition());
	scrollHistory->setContentSize(scrollSize);
	scrollHistory->setScrollBarEnabled(false);
	scrollHistory->setName("scrollhistory");
	nodeHistory->addChild(scrollHistory);
}

void BaseScene::initPopupIAP()
{
	popupIAP = createPopup("title_naptien.png", true, false);

	//popupIAP = Node::create();
	//popupIAP->setPosition(560, 350);
	//popupIAP->setVisible(false);
	//popupIAP->setTag(0);
	//mLayer->addChild(popupIAP, constant::ZORDER_POPUP);
	////autoScaleNode(popupCharge);

	//Sprite* bg = Sprite::create("imgs/popup_bg1.png");
	//popupIAP->addChild(bg);

	//Sprite* title = Sprite::createWithSpriteFrameName("title_naptien.png");
	//title->setPosition(0, bg->getContentSize().height / 2 - 5);
	//popupIAP->addChild(title);

	//ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	//btnClose->setPosition(Vec2(bg->getContentSize().width / 2 - 10, bg->getContentSize().height / 2 - 5));
	//addTouchEventListener(btnClose, [=]() {
	//	hidePopup(popupIAP);
	//});
	//popupIAP->addChild(btnClose);

	Size size = Size(810, 466);
	/*DrawNode* rect = DrawNode::create();
	rect->drawRect(Vec2(-size.width / 2, -size.height / 2), Vec2(size.width / 2, size.height / 2), Color4F::BLACK);
	rect->setPosition(80, -25);
	popupIAP->addChild(rect);*/

	Node* nodeStore = Node::create();
	nodeStore->setName("nodestore");
	nodeStore->setTag(102);
	//nodeStore->setPosition(80, 0);
	popupIAP->addChild(nodeStore);

	vector<ProductData> products = Utils::getSingleton().products;
	Size storeSize = Size(780, 260);

	ui::ScrollView* scrollStore = ui::ScrollView::create();
	scrollStore->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollStore->setBounceEnabled(true);
	scrollStore->setPosition(Vec2(-storeSize.width / 2, -storeSize.height / 2));
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

		ui::Button* btn = ui::Button::create("box_shop.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setPosition(Vec2(130 + i * 260, storeSize.height / 2));
		btn->setContentSize(Size(200, 180));
		btn->setScale9Enabled(true);
		btn->setBright(false);
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			showWaiting(300);
			Utils::getSingleton().purchaseItem(strId);
		});
		scrollStore->addChild(btn);

		Sprite* sp = Sprite::createWithSpriteFrameName("icon_money.png");
		sp->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 15);
		sp->setName("itemimage");
		btn->addChild(sp);

		Sprite* spCoin = Sprite::createWithSpriteFrameName("icon_silver.png");
		spCoin->setScale(.5f);
		btn->addChild(spCoin);

		Label* lb1 = Label::create(strValue, "fonts/guanine.ttf", 20);
		lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, btn->getContentSize().height / 2 - 70);
		lb1->setColor(Color3B::YELLOW);
		btn->addChild(lb1);

		Label* lb2 = Label::create(strCost, "fonts/guanine.ttf", 20);
		lb2->setWidth(175);
		lb2->setHeight(30);
		lb2->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 - 105);
		lb2->setColor(Color3B::WHITE);
		lb2->setHorizontalAlignment(TextHAlignment::CENTER);
		btn->addChild(lb2);

		spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
			+ spCoin->getContentSize().width * spCoin->getScale() / 2 + 5, lb1->getPositionY() - 3);
	}
}

void BaseScene::initPopupTour()
{
	popupTour = createPopup("title_loidai.png", true, true);
	if (!popupTour) return;
	popupTour->setTag(0);

	Size size = Size(780, 466);
	Size scrollSize = size - Size(20, 20);
	DrawNode* rect = DrawNode::create();
	rect->drawRect(Vec2(-size.width / 2, -size.height / 2), Vec2(size.width / 2, size.height / 2), Color4F::BLACK);
	rect->setPosition(95, -25);
	popupTour->addChild(rect);

	Node* nodeInfo = Node::create();
	nodeInfo->setName("nodeinfo");
	nodeInfo->setTag(0);
	popupTour->addChild(nodeInfo);

	Node* nodeHonor = Node::create();
	nodeHonor->setName("nodehonor");
	nodeHonor->setTag(1);
	nodeHonor->setVisible(false);
	popupTour->addChild(nodeHonor);

	vector<string> texts = { "thong_tin" , "vinh_danh" };
	int x = -400;
	int y = 180;
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "btn_light.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/aristote.ttf");
		btn->setTitleFontSize(25);
		btn->setTitleColor(i == 0 ? Color3B::BLACK : Color3B::WHITE);
		btn->setContentSize(Size(255, 50));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(10 + i);
		addTouchEventListener(btn, [=]() {
			if (popupTour->getTag() == i) return;
			popupTour->getChildByTag(i)->setVisible(true);
			popupTour->getChildByTag(1 - i)->setVisible(false);
			ui::Button* btn1 = (ui::Button*)popupTour->getChildByTag(10 + popupTour->getTag());
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("btn_light.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::BLACK);
			popupTour->setTag(i);

			if (i == 1) {
				SFSRequest::getSingleton().RequestTourWinners();
			}
		});
		popupTour->addChild(btn);
		//x += 210;
		y -= 70;
	}

	//Node info
	ui::ScrollView* scrollInfo = ui::ScrollView::create();
	scrollInfo->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollInfo->setBounceEnabled(true);
	scrollInfo->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + rect->getPosition());
	scrollInfo->setContentSize(scrollSize);
	scrollInfo->setScrollBarEnabled(false);
	scrollInfo->setName("scrollinfo");
	nodeInfo->addChild(scrollInfo);

	Node* nodeContent = Node::create();
	nodeContent->setName("nodecontent");
	scrollInfo->addChild(nodeContent);

	Node* nodelb1 = Node::create();
	nodelb1->setName("nodecontent1");
	nodeContent->addChild(nodelb1);

	float n1height = 0;
	for (int i = 0; i < 12; i++) {
		string str = Utils::getSingleton().getStringForKey("tour_info_" + to_string(i));
		Label* lbn1 = Label::createWithTTF(str, "fonts/arialbd.ttf", 22);
		lbn1->setPosition(0, n1height);
		lbn1->setAnchorPoint(Vec2(0, 1));
		lbn1->setColor(Color3B::WHITE);
		lbn1->setTag(i);
		nodelb1->addChild(lbn1);

		n1height -= 25.7f;
	}
	n1height = -n1height + 26;

	Label* lb2 = Label::create("", "fonts/arialit.ttf", 22);
	lb2->setAnchorPoint(Vec2(0, 1));
	lb2->setColor(Color3B::WHITE);
	lb2->setName("lbcontent2");
	lb2->setPosition(0, -n1height);
	nodeContent->addChild(lb2);

	string info3 = "";
	Label* lb3 = Label::create(info3, "fonts/arialbd.ttf", 22);
	lb3->setAnchorPoint(Vec2(0, 1));
	lb3->setColor(Color3B::WHITE);
	lb3->setName("lbcontent3");
	lb3->setWidth(scrollSize.width);
	lb3->setPosition(350, 0);
	nodeContent->addChild(lb3);

	//Node history
	ui::ScrollView* scrollHonor = ui::ScrollView::create();
	scrollHonor->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollHonor->setBounceEnabled(true);
	scrollHonor->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + rect->getPosition());
	scrollHonor->setContentSize(scrollSize);
	scrollHonor->setScrollBarEnabled(false);
	scrollHonor->setName("scrollhonor");
	nodeHonor->addChild(scrollHonor);

	ui::Button* btnRegister = ui::Button::create("btn_register.png", "btn_register_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnRegister->setPosition(Vec2(-400, -180));
	btnRegister->setScale(.9f);
	btnRegister->setName("btnregister");
	btnRegister->setColor(Color3B::GRAY);
	btnRegister->setTouchEnabled(false);
	popupTour->addChild(btnRegister);

	ui::Button* btnJoin = ui::Button::create("btn.png", "btn_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnJoin->setContentSize(btnRegister->getContentSize());
	btnJoin->setScale9Enabled(true);
	btnJoin->setCapInsets(Rect(85, 10, 10, 40));
	btnJoin->setPosition(btnRegister->getPosition());
	btnJoin->setScale(.9f);
	btnJoin->setName("btnjoin");
	btnJoin->setColor(Color3B::GRAY);
	btnJoin->setTouchEnabled(false);
	btnJoin->setVisible(false);
	popupTour->addChild(btnJoin);

	Label* titleJoin = Label::createWithTTF(Utils::getSingleton().getStringForKey("tham_gia"), "fonts/staccato.ttf", 32);
	titleJoin->setPosition(btnJoin->getContentSize().width / 2, btnJoin->getContentSize().height / 2 + 5);
	titleJoin->setColor(Color3B(255, 255, 80));
	titleJoin->enableOutline(Color4B(51, 0, 0, 255), 2);
	titleJoin->setTag(0);
	btnJoin->addChild(titleJoin);

	Label* lbCountDown = Label::createWithTTF("", "fonts/arialbd.ttf", 40);
	lbCountDown->setPosition(btnRegister->getPosition() - Vec2(80, 60));
	lbCountDown->setAnchorPoint(Vec2(0, .5f));
	lbCountDown->setTag(0);
	lbCountDown->setName("lbcountdown");
	popupTour->addChild(lbCountDown);

	Node* nodelbtime = Node::create();
	nodelbtime->setName("0");
	nodelbtime->setTag(0);
	lbCountDown->addChild(nodelbtime);

	addTouchEventListener(btnRegister, [=]() {
		registerTour();
	});

	addTouchEventListener(btnJoin, [=]() {
		joinIntoTour();
	});

	onTourInfoResponse(Utils::getSingleton().tourInfo);
}

void BaseScene::initCofferView(Vec2 pos, int zorder, float scale)
{
	bool ispmE = Utils::getSingleton().ispmE();
	string btnSprite = ispmE ? "coffer.png" : "empty.png";
	ui::Button* btnCoffer = ui::Button::create(btnSprite, btnSprite, "", ui::Widget::TextureResType::PLIST);
	btnCoffer->setPosition(pos);
	btnCoffer->setScale(scale);
	btnCoffer->setVisible(ispmE);
	addTouchEventListener(btnCoffer, [=]() {
		showPopupCoffer();
		//onErrorResponse(34, "Bat dau dang ky giai dau");
		//onErrorResponse(37, "Bat dau tham gia giai dau");
		//onErrorResponse(36, "Ban da vao vong trong");
		//onErrorResponse(80, "Giai dau ket thuc");
		//showPopupConfirm("Da co the dang ky giai dau", "stormus", "phantom", [=]() {});
		//showPopupConfirmMini("Da co the dang ky giai dau", "stormus", "phantom", Vec2(200, 200), [=]() {});
	});
	mLayer->addChild(btnCoffer, zorder);
	autoScaleNode(btnCoffer);
	lbCoffer = Label::create(Utils::getSingleton().formatMoneyWithComma(Utils::getSingleton().cofferMoney), "fonts/arialbd.ttf", 20);
	lbCoffer->setPosition(btnCoffer->getContentSize().width / 2, 15);
	lbCoffer->setColor(Color3B::YELLOW);
	btnCoffer->addChild(lbCoffer);

	Sprite* lightPoint = Sprite::createWithSpriteFrameName(ispmE ? "light_point.png" : "empty.png");
	lightPoint->setPosition(btnCoffer->getContentSize().width / 2, btnCoffer->getContentSize().height / 2);
	btnCoffer->addChild(lightPoint, -1);

	ScaleTo* scale1 = ScaleTo::create(2, .7f);
	ScaleTo* scale2 = ScaleTo::create(2, 1);
	lightPoint->runAction(RepeatForever::create(Sequence::createWithTwoActions(scale1, scale2)));
}

void BaseScene::onPingPong(long timems)
{
	pingId++;
	if (timems < 150) {
		spNetwork->initWithSpriteFrameName("wifi0.png");
	} else if (timems < 500) {
		spNetwork->initWithSpriteFrameName("wifi1.png");
	} else if (timems < 1000) {
		spNetwork->initWithSpriteFrameName("wifi2.png");
	} else {
		spNetwork->initWithSpriteFrameName("wifi3.png");
	}
	//spNetwork->setAnchorPoint(Vec2(1, 0));
	//lbNetwork->setString(to_string(timems) + "ms");
}

void BaseScene::onUserDataMeResponse()
{
	if (!hasHeader) return;
	bool ispmE = Utils::getSingleton().ispmE();
	UserData dataMe = Utils::getSingleton().userDataMe;
	std::string strGold = Utils::getSingleton().formatMoneyWithComma(dataMe.MoneyReal);
	std::string strSilver = Utils::getSingleton().formatMoneyWithComma(dataMe.MoneyFree);
	std::string strId = String::createWithFormat("ID: %ld", dataMe.UserID)->getCString();
	std::string strLevel = String::createWithFormat((Utils::getSingleton().getStringForKey("level") + ": %d").c_str(), dataMe.Level)->getCString();

	bgNoted->setVisible(ispmE && !dataMe.IsActived);
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

	vector<int> posX = { -390, -300, -60, 160, 290, 380 };
	vector<int> widths = { 50, 100, 320, 210, 150, 60 };
	ui::ScrollView* scroll = (ui::ScrollView*)popupHistory->getChildByName("scroll");
	int height = logs.size() * 60;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
	for (int i = 0; i < logs.size(); i++) {
		vector<Label*> lbs;
		ui::Button* btn;
		if (i < scroll->getChildrenCount() / 7) {
			for (int j = 0; j < 6; j++) {
				Label* lb = (Label*)scroll->getChildByTag(i * 7 + j);
				lb->setPosition(posX[j] + scroll->getContentSize().width / 2, height - 5 - i * 60);
				lb->setVisible(true);
				lbs.push_back(lb);
			}

			btn = (ui::Button*)scroll->getChildByTag(i * 7 + 6);
			btn->setPosition(Vec2(scroll->getContentSize().width / 2, height - 5 - i * 60));
			btn->setVisible(true);
		} else {
			for (int j = 0; j < 6; j++) {
				Label* lbDetail = Label::create("", "fonts/arial.ttf", 20);
				lbDetail->setWidth(widths[j]);
				lbDetail->setHeight(46);
				lbDetail->setAnchorPoint(Vec2(.5f, 1));
				lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
				lbDetail->setPosition(posX[j] + scroll->getContentSize().width / 2, height - 5 - i * 60);
				lbDetail->setTag(i * 7 + j);
				lbDetail->setColor(Color3B::WHITE);
				scroll->addChild(lbDetail);
				lbs.push_back(lbDetail);
			}

			btn = ui::Button::create("white.png", "", "", ui::Widget::TextureResType::PLIST);
			btn->setContentSize(Size(scroll->getContentSize().width, lbs[0]->getHeight()));
			btn->setPosition(Vec2(scroll->getContentSize().width / 2, height - 5 - i * 60));
			btn->setAnchorPoint(Vec2(.5f, 1));
			btn->setScale9Enabled(true);
			btn->setOpacity(0);
			btn->setTag(i * 7 + 6);
			addTouchEventListener(btn, [=]() {
				Node* nodeDetail = popupHistory->getChildByName("nodedetail");
				nodeDetail->setVisible(true);
				for (int i = 0; i < 6; i++) {
					Label* lb = (Label*)nodeDetail->getChildByTag(i);
					lb->setString(lbs[i]->getString());
				}
			});
			scroll->addChild(btn);
		}
		lbs[0]->setString(to_string((popupHistory->getChildByName("nodepage")->getTag() - 1) * 10 + i + 1));
		lbs[1]->setString(logs[i].Date);
		lbs[2]->setString(Utils::getSingleton().trim(logs[i].Info));
		lbs[3]->setString(Utils::getSingleton().formatMoneyWithComma(logs[i].Money));
		lbs[4]->setString(Utils::getSingleton().formatMoneyWithComma(logs[i].Balance));
		lbs[5]->setString(to_string(logs[i].GameId));
	}

	int count = scroll->getChildrenCount();
	for (int i = logs.size() * 7; i < scroll->getChildrenCount(); i++) {
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
	int height = list.size() * 75;
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
			node->setPosition(scroll->getContentSize().width / 2, height - 40 - i * 75);
			node->setTag(i);
			scroll->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
			nbg->setContentSize(Size(width, 85));
			node->addChild(nbg);

			Label* lb1 = Label::create(list[i].Date, "fonts/arialbd.ttf", 20);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(Color3B::WHITE);
			lb1->setPosition(-width / 2 + 20, 0);
			lb1->setWidth(100);
			lb1->setTag(1);
			lb1->setVisible(false);
			node->addChild(lb1);

			lb2 = Label::create(list[i].Name, "fonts/arialbd.ttf", 22);
			//lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setColor(Color3B::WHITE);
			//lb2->setPosition(-width / 2 + 150, 10);
			lb2->setPosition(-width / 2 + 80, 0);
			//lb2->setWidth(400);
			//lb2->setHeight(25);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::create(list[i].Cuocs, "fonts/arialbd.ttf", 22);
			//lb3->setAnchorPoint(Vec2(0, .5f));
			lb3->setHorizontalAlignment(TextHAlignment::CENTER);
			lb3->setColor(Color3B(255, 255, 102));
			lb3->setPosition(-10, 0);
			lb3->setWidth(380);
			//lb3->setHeight(50);
			lb3->setTag(3);
			node->addChild(lb3);

			Label* lb4 = Label::create(Utils::getSingleton().formatMoneyWithComma(list[i].Point), "fonts/arialbd.ttf", 22);
			//lb4->setAnchorPoint(Vec2(1, .5f));
			lb4->setColor(Color3B(102, 255, 51));
			lb4->setPosition(width / 2 - 30, 0);
			lb4->setTag(4);
			node->addChild(lb4);

			Label* lb5 = Label::create(Utils::getSingleton().formatMoneyWithComma(list[i].Money), "fonts/arialbd.ttf", 22);
			//lb5->setAnchorPoint(Vec2(1, .5f));
			lb5->setColor(Color3B::YELLOW);
			lb5->setPosition(width / 2 - 120, 0);
			lb5->setTag(5);
			node->addChild(lb5);
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

void BaseScene::onTourWinnersResponse(std::vector<TourAward> list)
{
	int nheight = 140;
	int iheight = 30;
	Node* nodeHonor = popupTour->getChildByName("nodehonor");
	ui::ScrollView* scroll = (ui::ScrollView*)nodeHonor->getChildByName("scrollhonor");
	int height = list.size() * nheight;
	int width = scroll->getContentSize().width;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(width, height));
	for (int i = 0; i < list.size(); i++) {
		ui::Scale9Sprite* nbg;
		Label* lbDate;
		vector<Node*> inodes;
		vector<vector<Label*>> lbs;
		Node* node = scroll->getChildByTag(i);
		if (node == nullptr) {
			node = Node::create();
			node->setTag(i);
			scroll->addChild(node);

			nbg = ui::Scale9Sprite::createWithSpriteFrameName("box3.png");
			nbg->setTag(10);
			node->addChild(nbg);

			lbDate = Label::create("", "fonts/arialbd.ttf", 20);
			lbDate->setHorizontalAlignment(TextHAlignment::CENTER);
			lbDate->setPosition(-width / 2 + 90, nheight / 2 - 40);
			lbDate->setColor(Color3B::WHITE);
			lbDate->setTag(11);
			lbDate->setWidth(150);
			node->addChild(lbDate);

			lbName = Label::create("", "fonts/arialbd.ttf", 20);
			lbName->setHorizontalAlignment(TextHAlignment::CENTER);
			lbName->setPosition(-width / 2 + 90, -nheight / 2 + 40);
			lbName->setColor(Color3B::WHITE);
			lbName->setTag(12);
			lbName->setWidth(150);
			node->addChild(lbName);

			for (int j = 0; j < 4; j++) {
				Node* inode = Node::create();
				inode->setTag(j);
				node->addChild(inode);
				inodes.push_back(inode);

				vector<Label*> ilbs;
				Label *lb0 = Label::create("", "fonts/arialbd.ttf", 20);
				lb0->setPosition(-width / 2 + 200, 0);
				lb0->setAnchorPoint(Vec2(0, .5f));
				lb0->setColor(Color3B::WHITE);
				lb0->setTag(0);
				inode->addChild(lb0);
				ilbs.push_back(lb0);

				Label *lb1 = Label::create("", "fonts/arialbd.ttf", 22);
				lb1->setPosition(lb0->getPositionX() + 130, 0);
				lb1->setAnchorPoint(Vec2(0, .5f));
				lb1->setColor(Color3B::WHITE);
				lb1->setTag(1);
				inode->addChild(lb1);
				ilbs.push_back(lb1);

				Label *lb2 = Label::create("", "fonts/arialbd.ttf", 22);
				lb2->setPosition(lb1->getPositionX() + 270, 0);
				lb2->setAnchorPoint(Vec2(0, .5f));
				lb2->setColor(Color3B::YELLOW);
				lb2->setTag(2);
				inode->addChild(lb2);
				ilbs.push_back(lb2);

				lbs.push_back(ilbs);
			}
		} else {
			node->setVisible(true);
			nbg = (ui::Scale9Sprite*)node->getChildByTag(10);
			lbDate = (Label*)node->getChildByTag(11);
			lbName = (Label*)node->getChildByTag(12);
			for (int j = 0; j < 4; j++) {
				Node* inode = node->getChildByTag(j);
				vector<Label*> ilbs;
				int childCount = inode->getChildrenCount();
				for (int k = 0; k < childCount; k++) {
					ilbs.push_back((Label*)inode->getChildByTag(k));
				}
				inodes.push_back(inode);
				lbs.push_back(ilbs);
			}
		}

		node->setPosition(scroll->getContentSize().width / 2, height - nheight / 2 - i * nheight);
		nbg->setContentSize(Size(width, nheight));
		lbDate->setString(list[i].KeyDate);
		lbName->setString(Utils::getSingleton().getStringForKey("giai_dau") + " " + to_string(list[i].Id));
		int j = 0;
		for (; j < list[i].Winners.size(); j++) {
			inodes[j]->setVisible(true);
			inodes[j]->setPosition(0, nheight / 2 - iheight / 2 - 10 - j * iheight);

			lbs[j][0]->setString(list[i].Winners[j].LevelTitle);
			lbs[j][1]->setString(list[i].Winners[j].Name);
			lbs[j][2]->setString(Utils::getSingleton().formatMoneyWithComma(list[i].Winners[j].Money));
			cropLabel(lbs[j][1], 200);
		}
		for (; j < 4; j++) {
			inodes[j]->setVisible(false);
		}
	}
	int i = list.size();
	Node* n;
	while ((n = scroll->getChildByTag(i++)) != nullptr) {
		n->setVisible(false);
	}
}

void BaseScene::onTourRoomToJoin(std::string room)
{
	if (isJoiningTour) {
		Utils::getSingleton().currentLobbyName = Utils::getSingleton().currentRoomName;
		SFSRequest::getSingleton().RequestJoinRoom(room);
	} else {
		tourGroup = room.substr(0, room.length() - 1);
		Utils::getSingleton().currentLobbyName = "cho3";
		prepareTourRoom = tourGroup + "1";

		/*if (Utils::getSingleton().tourRoom.length() > 0) {
			SFSRequest::getSingleton().RequestJoinRoom(room);
			Utils::getSingleton().tourRoom = "";
		}*/
	}
}

void BaseScene::onTourInfoResponse(TourInfo tourInfo)
{
	if (!popupTour) return;
	bool isTourExist = tourInfo.Name.length() > 0;
	Node* nodeInfo = popupTour->getChildByName("nodeinfo");
	ui::ScrollView* scroll = (ui::ScrollView*)nodeInfo->getChildByName("scrollinfo");
	Node* nodeContent = scroll->getChildByName("nodecontent");
	Node* nodeContent1 = nodeContent->getChildByName("nodecontent1");
	Label* lb2 = (Label*)nodeContent->getChildByName("lbcontent2");
	Label* lb3 = (Label*)nodeContent->getChildByName("lbcontent3");
	Label* lbCountDown = (Label*)popupTour->getChildByName("lbcountdown");

	float n1height = 0;
	nodeContent1->setVisible(isTourExist);
	if (isTourExist) {
		vector<Node*> lbs;
		for (int i = 0; i < 12; i++) {
			if (i == 10) {
				nodeContent1->getChildByTag(i)->setVisible(tourInfo.Race3Enabled);
				if (!tourInfo.Race3Enabled) continue;
			}
			lbs.push_back(nodeContent1->getChildByTag(i));
		}
		for (Node* lb : lbs) {
			lb->setPositionY(n1height);
			n1height -= 25.7f;
		}
		n1height = -n1height + 26;
	}

	string info2 = Utils::getSingleton().tourGuide;
	if (info2.length() == 0) {
		SFSRequest::getSingleton().RequestHttpGet("http://kinhtuchi.com/main_kinhtuchi/giaidau.txt", constant::TAG_HTTP_TOUR_GUIDE);
		cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
		info2 = plist["loi_dai_2"].asString();
	}
	string requiredMatchStr = isTourExist ? to_string(tourInfo.RequiredMatch) : "85";
	info2 = Utils::getSingleton().replaceString(info2, "[p1]", requiredMatchStr + "%");

	int scrollWidth = scroll->getContentSize().width;
	lb2->initWithTTF("", isTourExist ? "fonts/arialit.ttf" : "fonts/arial.ttf", 22);
	lb2->setWidth(scrollWidth);
	lb2->setString(info2);
	lb2->setPosition(0, isTourExist ? -n1height : 0);

	int height = (isTourExist ? n1height : 0) + lb2->getContentSize().height;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	nodeContent->setPosition(0, height);
	scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));

	string info3 = "";
	if (isTourExist) {
		std::vector<string> playModes = { "win_free", "win_free", "win_411" };
		string stimeFormat = Utils::getSingleton().getStringForKey("tu_den");
		string regTimeBegin = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.RegTimeBegin, "%H:%M:%S");
		string regTimeEnd = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.RegTimeEnd, "%H:%M:%S");
		string race1TimeBegin = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.Race1TimeBegin, "%H:%M:%S");
		string race1TimeEnd = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.Race1TimeEnd, "%H:%M:%S");
		string race2TimeBegin = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.Race2TimeBegin, "%H:%M:%S");
		string race2TimeEnd = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.Race2TimeEnd, "%H:%M:%S");
		string race3TimeBegin = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.Race3TimeBegin, "%H:%M:%S");
		string race3TimeEnd = Utils::getSingleton().getSystemTimeStringBySecs(tourInfo.Race3TimeEnd, "%H:%M:%S");
		string regTime(String::createWithFormat(stimeFormat.c_str(), regTimeBegin.c_str(), regTimeEnd.c_str())->getCString());
		string race1Time(String::createWithFormat(stimeFormat.c_str(), race1TimeBegin.c_str(), race1TimeEnd.c_str())->getCString());
		string race2Time(String::createWithFormat(stimeFormat.c_str(), race2TimeBegin.c_str(), race2TimeEnd.c_str())->getCString());
		string race3Time(String::createWithFormat(stimeFormat.c_str(), race3TimeBegin.c_str(), race3TimeEnd.c_str())->getCString());
		info3 += tourInfo.Name + "\n";
		info3 += to_string((int)tourInfo.MaxUser) + "\n";
		info3 += Utils::getSingleton().formatMoneyWithComma(tourInfo.MoneyCoc) + " Quan\n";
		info3 += Utils::getSingleton().formatMoneyWithComma(tourInfo.RequiredMoney) + " Quan\n";
		info3 += to_string(tourInfo.RequiredLevel) + "\n";
		//info3 += to_string(tourInfo.RequiredVip) + "\n";
		info3 += Utils::getSingleton().formatMoneyWithComma(tourInfo.MoneyCuoc) + " Quan\n";
		//info3 += to_string(tourInfo.MoneyTime) + "s\n";
		info3 += Utils::getSingleton().getStringForKey(playModes[(int)tourInfo.Mode]) + "\n";
		info3 += regTime + "\n";
		info3 += race1Time + "\n";
		info3 += race2Time + "\n";
		info3 += (tourInfo.Race3Enabled ? (race3Time + "\n") : "");
		info3 += to_string(tourInfo.MaxMatch);
	}

	lb3->setString(info3);
	calculateTourTimeOnLabel(lbCountDown);
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
	} else if (tag == constant::TAG_HTTP_TOUR_GUIDE) {
		Utils::getSingleton().tourGuide = content;
		if (!popupTour) return;
		TourInfo tourInfo = Utils::getSingleton().tourInfo;
		bool isTourExist = tourInfo.Name.length() > 0;
		Node* nodeInfo = popupTour->getChildByName("nodeinfo");
		ui::ScrollView* scroll = (ui::ScrollView*)nodeInfo->getChildByName("scrollinfo");
		Node* nodeContent = scroll->getChildByName("nodecontent");
		Label* lb2 = (Label*)nodeContent->getChildByName("lbcontent2");

		string requiredMatchStr = isTourExist ? to_string(tourInfo.RequiredMatch) : "85";
		string info2 = Utils::getSingleton().replaceString(content, "[p1]", requiredMatchStr + "%");
		lb2->setString(info2);

		int height = -lb2->getPositionY() + lb2->getContentSize().height;
		if (height < scroll->getContentSize().height) {
			height = scroll->getContentSize().height;
		}
		nodeContent->setPosition(0, height);
		scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
	}
}

void BaseScene::onHttpResponseFailed(int tag)
{
	CCLOG("falied");
}

void BaseScene::addBtnChoosePage(int x, int y, cocos2d::Node * node, std::function<void(int)> funcPage)
{
	Node* nodePage = Node::create();
	nodePage->setTag(1);
	nodePage->setName("nodepage");
	node->addChild(nodePage);

	int x1 = x - 100;
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = ui::Button::create("box5.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleFontName("fonts/arial.ttf");
		btn->setTitleFontSize(20);
		btn->setTitleText(to_string(i));
		btn->setTitleColor(Color3B::BLACK);
		btn->setContentSize(Size(40, 40));
		btn->setPosition(Vec2(x1, y));
		btn->setScale9Enabled(true);
		btn->setColor(i == 1 ? pageColor2 : pageColor1);
		btn->setTag(1000 + i);
		addTouchEventListener(btn, [=]() {
			int curPage = nodePage->getTag();
			int numb = atoi(btn->getTitleText().c_str());
			if (numb == curPage) return;
			node->getChildByTag(1000 + ((curPage - 1) % 5) + 1)->setColor(pageColor1);
			btn->setColor(pageColor2);
			nodePage->setTag(numb);
			funcPage(numb);
		});
		node->addChild(btn);
		x1 += 50;
	}

	ui::Button* btnPre = ui::Button::create("box5.png", "", "", ui::Widget::TextureResType::PLIST);
	btnPre->setTitleFontName("fonts/arial.ttf");
	btnPre->setTitleFontSize(20);
	btnPre->setTitleText("<<");
	btnPre->setTitleColor(Color3B::BLACK);
	btnPre->setContentSize(Size(60, 40));
	btnPre->setPosition(Vec2(x - 160, y));
	btnPre->setScale9Enabled(true);
	btnPre->setColor(pageColor1);
	addTouchEventListener(btnPre, [=]() {
		for (int i = 1; i <= 5; i++) {
			ui::Button* btn = (ui::Button*)node->getChildByTag(1000 + i);
			int numb = atoi(btn->getTitleText().c_str());
			if (numb == 1) return;
			btn->setTitleText(to_string(numb - 5));
			if (numb - 5 == nodePage->getTag()) {
				btn->setColor(pageColor2);
			} else {
				btn->setColor(pageColor1);
			}
		}
	});
	node->addChild(btnPre);

	ui::Button* btnNext = ui::Button::create("box5.png", "", "", ui::Widget::TextureResType::PLIST);
	btnNext->setTitleFontName("fonts/arial.ttf");
	btnNext->setTitleFontSize(20);
	btnNext->setTitleText(">>");
	btnNext->setTitleColor(Color3B::BLACK);
	btnNext->setContentSize(Size(60, 40));
	btnNext->setPosition(Vec2(x + 160, y));
	btnNext->setScale9Enabled(true);
	btnNext->setColor(pageColor1);
	addTouchEventListener(btnNext, [=]() {
		for (int i = 1; i <= 5; i++) {
			ui::Button* btn = (ui::Button*)node->getChildByTag(1000 + i);
			int numb = atoi(btn->getTitleText().c_str());
			if (numb == 995) return;
			btn->setTitleText(to_string(numb + 5));
			if (numb + 5 == nodePage->getTag()) {
				btn->setColor(pageColor2);
			} else {
				btn->setColor(pageColor1);
			}
		}
	});
	node->addChild(btnNext);
}

void BaseScene::setSplashZOrder(int zorder)
{
	if (zorder == splash->getLocalZOrder() || !splash->isVisible()) return;
	splash->setLocalZOrder(zorder);
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

void BaseScene::switchMoneyType(int type)
{
	if (moneyBg0 != NULL && moneyBg0->getTag() != type) {
		moneyBg0->setTag(type);
		moneyBg1->setTag(type);
		chosenBg->setPosition(type == 0 ? 100 : -100, 0);
		onChangeMoneyType(type);
		//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), type == 1);
	}
}

void BaseScene::cropLabel(cocos2d::Label *label, int width, bool dots)
{
	bool isCut = false;
	string str = label->getString();
	while (label->getContentSize().width > width) {
		str = str.substr(0, str.length() - 1);
		label->setString(str);
		isCut = true;
	}
	if (isCut && dots) {
		str += "..";
		label->setString(str);
	}
}

void BaseScene::calculateTourTimeOnLabel(cocos2d::Label *lbCountDown)
{
	time_t rawtime;
	time(&rawtime);
	double timeDiff = Utils::getSingleton().serverTimeDiff;
	TourInfo tourInfo = Utils::getSingleton().tourInfo;
	int state = lbCountDown->getTag();

	Utils::getSingleton().tourRemindId = tourInfo.RegTimeBegin;
	if (state == 0) {
		if (rawtime < tourInfo.RegTimeBegin + timeDiff) {
			setTourTimeState(0);
			lbCountDown->setTag(1);
			tourTimeRemain = -1;
			double timeDelay = tourInfo.RegTimeBegin + timeDiff - rawtime + 3;
			delayFunction(lbCountDown, timeDelay, [=]() {
				Utils::getSingleton().tourInfo.CanRegister = true;
				calculateTourTimeOnLabel(lbCountDown);
			});
		} else if (rawtime >= tourInfo.RegTimeBegin + timeDiff
			&& rawtime < tourInfo.RegTimeEnd + timeDiff) {
			setTourTimeState(1);
			lbCountDown->setTag(2);
			tourTimeRemain = tourInfo.RegTimeEnd + timeDiff - rawtime + 3;
		} else if (rawtime >= tourInfo.RegTimeEnd + timeDiff
			&& rawtime < tourInfo.Race1TimeBegin + timeDiff) {
			setTourTimeState(2);
			lbCountDown->setTag(3);
			tourTimeRemain = -1;
			double timeDelay = tourInfo.Race1TimeBegin + timeDiff - rawtime + 3;
			delayFunction(lbCountDown, timeDelay, [=]() {
				Utils::getSingleton().tourInfo.IsTouring = true;
				calculateTourTimeOnLabel(lbCountDown);
			});
		} else if (rawtime >= tourInfo.Race1TimeBegin + timeDiff
			&& rawtime < tourInfo.Race2TimeEnd + timeDiff) {
			setTourTimeState(3);
			lbCountDown->setTag(4);
			tourTimeRemain = tourInfo.Race2TimeEnd + timeDiff - rawtime + 3;
		} else if (rawtime >= tourInfo.Race2TimeEnd + timeDiff) {
			setTourTimeState(4);
			lbCountDown->setTag(5);
			tourTimeRemain = -1;
		}
	} else if (state == 1) {
		setTourTimeState(1);
		lbCountDown->setTag(2);
		tourTimeRemain = tourInfo.RegTimeEnd - tourInfo.RegTimeBegin;
	} else if (state == 2) {
		setTourTimeState(2);
		lbCountDown->setTag(3);
		tourTimeRemain = -1;// tourInfo.Race1TimeBegin - tourInfo.RegTimeEnd;
		double timeDelay = tourInfo.Race1TimeBegin + timeDiff - rawtime + 3;
		delayFunction(lbCountDown, timeDelay, [=]() {
			Utils::getSingleton().tourInfo.IsTouring = true;
			calculateTourTimeOnLabel(lbCountDown);
		});
	} else if (state == 3) {
		setTourTimeState(3);
		lbCountDown->setTag(4);
		tourTimeRemain = tourInfo.Race2TimeEnd - tourInfo.Race1TimeBegin;
	} else if (state == 4) {
		setTourTimeState(4);
		lbCountDown->setTag(5);
		tourTimeRemain = -1;
	}

	if (tourTimeRemain >= 0) {
		showTourCountDown(lbCountDown, [=]() {
			calculateTourTimeOnLabel(lbCountDown);
		});
	}
}

void BaseScene::setTourTimeState(int state)
{
	if (!popupTour) return;
	TourInfo tourInfo = Utils::getSingleton().tourInfo;
	ui::Button *btnJoin = (ui::Button*)popupTour->getChildByName("btnjoin");
	ui::Button *btnRegister = (ui::Button*)popupTour->getChildByName("btnregister");
	Label* lbCountDown = (Label*)popupTour->getChildByName("lbcountdown");

	if (state == 0) {
		lbCountDown->setTag(1);
		btnJoin->setVisible(false);
		btnRegister->setVisible(true);
		btnRegister->setColor(Color3B::GRAY);
		btnRegister->setTouchEnabled(false);
	} else if (state == 1) {
		lbCountDown->setTag(2);
		if (tourInfo.CanRegister) {
			btnJoin->setVisible(false);
			btnRegister->setVisible(true);
			btnRegister->setColor(Color3B::WHITE);
			btnRegister->setTouchEnabled(true);
		} else {
			btnRegister->setVisible(false);
			btnJoin->setVisible(true);
			btnJoin->setColor(Color3B::GRAY);
			btnJoin->setTouchEnabled(false);
			btnJoin->getChildByTag(0)->setColor(Color3B(130, 130, 80));
		}
	} else if (state == 2) {
		lbCountDown->setTag(3);
		btnRegister->setVisible(false);
		btnJoin->setVisible(true);
		btnJoin->setColor(Color3B::GRAY);
		btnJoin->setTouchEnabled(false);
		btnJoin->getChildByTag(0)->setColor(Color3B(130, 130, 80));
	} else if (state == 3) {
		lbCountDown->setTag(4);
		if (tourInfo.CanRegister) {
			btnJoin->setVisible(false);
			btnRegister->setVisible(true);
			btnRegister->setColor(Color3B::GRAY);
			btnRegister->setTouchEnabled(false);
		} else {
			btnRegister->setVisible(false);
			btnJoin->setVisible(true);
			btnJoin->setColor(Color3B::WHITE);
			btnJoin->setTouchEnabled(true);
			btnJoin->getChildByTag(0)->setColor(Color3B(255, 255, 80));
		}
	} else if (state == 4) {
		lbCountDown->setTag(5);
		btnJoin->setVisible(false);
		btnRegister->setVisible(true);
		btnRegister->setColor(Color3B::GRAY);
		btnRegister->setTouchEnabled(false);
	}
}

void BaseScene::showTourCountDown(Label* lbCountDown, std::function<void()> callback)
{
	string timeRemainString = Utils::getSingleton().getCountTimeStringBySecs(tourTimeRemain, "%H:%M:%S");
	lbCountDown->setString(timeRemainString);
	lbCountDown->getChildByTag(0)->setName(to_string((long)tourTimeRemain));

	DelayTime *delayTime = DelayTime::create(1);
	CallFunc *func = CallFunc::create([=]() {
		long timeRemain = atol(lbCountDown->getChildByTag(0)->getName().c_str());
		if (timeRemain > 1) {
			timeRemain -= 1;
			string str = Utils::getSingleton().getCountTimeStringBySecs(timeRemain, "%H:%M:%S");
			lbCountDown->setString(str);
			lbCountDown->getChildByTag(0)->setName(to_string(timeRemain));
		} else {
			lbCountDown->stopActionByTag(3);
			lbCountDown->setString("");
			callback();
		}
	});
	Action* actionCount = RepeatForever::create(Sequence::createWithTwoActions(delayTime, func));
	actionCount->setTag(3);
	lbCountDown->stopActionByTag(3);
	lbCountDown->runAction(actionCount);
}

void BaseScene::joinIntoLobby(int lobby)
{
	if (isWaiting) return;
	showWaiting();
	tmpZoneId = lobby;
	isGoToLobby = true;
	SFSRequest::getSingleton().Disconnect();
}

void BaseScene::processCachedErrors()
{
	for (pair<unsigned char, string> p : Utils::getSingleton().cachedErrors) {
		showPopupNotice(p.second, [=]() {});
	}
	Utils::getSingleton().cachedErrors.clear();
}

void BaseScene::runConnectionKeeper()
{
	if (connectionKeeper) {
		connectionKeeper->resume();
		return;
	}

	connectionKeeper = Node::create();
	mLayer->addChild(connectionKeeper);
	DelayTime* delayCK = DelayTime::create(100);
	CallFunc* funcCK = CallFunc::create([=]() {
		//SFSRequest::getSingleton().Ping();
		SFSRequest::getSingleton().RequestKeepConnection();
	});
	Sequence* actionCK = Sequence::createWithTwoActions(delayCK, funcCK);
	connectionKeeper->runAction(RepeatForever::create(actionCK));
}

void BaseScene::showPopupTour()
{
	if (popupTour == NULL) {
		initPopupTour();
	}
	showPopup(popupTour);
	if (this->getTag() == constant::SCENE_MAIN) {
		runConnectionKeeper();
	}
}

void BaseScene::registerTour()
{
	ui::Button *btnRegister = (ui::Button*)popupTour->getChildByName("btnregister");
	ui::Button *btnJoin = (ui::Button*)popupTour->getChildByName("btnjoin");
	btnRegister->setVisible(false);
	btnJoin->setVisible(true);
	btnJoin->setColor(Color3B::GRAY);
	btnJoin->setTouchEnabled(false);
	btnJoin->getChildByTag(0)->setColor(Color3B(130, 130, 80));
	//lbCountDown->stopAllActions();
	//lbCountDown->setString("");
	//tourTimeRemain = -1;
	Utils::getSingleton().tourInfo.CanRegister = false;
	SFSRequest::getSingleton().RequestRegisterTour();
}

void BaseScene::joinIntoTour()
{
	if (isWaiting) return;
	switchMoneyType(1);
	isJoiningTour = true;
	showWaiting();
	tmpZoneId = 5;
	zoneBeforeTour = Utils::getSingleton().currentZoneName;
	SFSRequest::getSingleton().Disconnect();
}
