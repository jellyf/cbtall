#include "BaseScene.h"
#include "EventHandler.h"
#include "SFSRequest.h"
#include "Utils.h"
#include "Constant.h"
#include "AudioEngine.h"
#include "SFSResponse.h"

using namespace cocos2d;
using namespace std;

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

	mLayer = Layer::create();
	addChild(mLayer, 10);
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

	splash = ui::Scale9Sprite::create("white.png");
	splash->setContentSize(Size(1500, 1000));
	splash->setPosition(560, 350);
	splash->setColor(Color3B::BLACK);
	splash->setOpacity(150);
	splash->setVisible(false);
	mLayer->addChild(splash);

	Node* nodeWaiting = Node::create();
	nodeWaiting->setPosition(560, 350);
	nodeWaiting->setVisible(false);
	mLayer->addChild(nodeWaiting, constant::ZORDER_POPUP);
	autoScaleNode(nodeWaiting);

	spWaiting = Sprite::create("loadingicon.png");
	nodeWaiting->addChild(spWaiting);

	RotateBy* rotate = RotateBy::create(2, 360);
	spWaiting->runAction(RepeatForever::create(rotate));
	spWaiting->pauseSchedulerAndActions();

	initPopupUserInfo();

	spNetwork = Sprite::create("wifi0.png");
	//spNetwork->setAnchorPoint(Vec2(1, 0));
	//spNetwork->setPosition(1115, 5);
	spNetwork->setPosition(570, 650);
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
	SFSRequest::getSingleton().ProcessEvents();
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
}

bool BaseScene::onTouchBegan(Touch * touch, Event * _event)
{
	Point pos = touch->getLocation();
	pos = popupUserInfo->convertToNodeSpace(pos);

	tmpIndex = -1;
	int i = 0;
	for (Node* n : tmps) {
		if (n->isVisible() && n->getBoundingBox().containsPoint(pos)) {
			tmpIndex = i;
			return true;
		}
		i++;
	}
	return false;
}

void BaseScene::onTouchMoved(Touch * touch, Event * _event)
{
	Point pos = touch->getLocation();
	pos = popupUserInfo->convertToNodeSpace(pos);

	if (tmpIndex > -1)
		tmps.at(tmpIndex)->setPosition(pos);
}

void BaseScene::onTouchEnded(Touch * touch, Event * _event)
{
	if (tmpIndex > -1)
		CCLOG("%d %d, %d", tmpIndex, (int)tmps.at(tmpIndex)->getPosition().x, (int)tmps.at(tmpIndex)->getPosition().y);
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

void BaseScene::showPopupNotice(std::string msg, std::function<void()> func, bool showBtnClose)
{
	Node* popupNotice = createPopupNotice();
	showPopup(popupNotice);
	Label* lbcontent = (Label*)popupNotice->getChildByName("lbcontent");
	lbcontent->setString(msg);
	ui::Button* btnClose = (ui::Button*)popupNotice->getChildByName("btnclose");
	btnClose->setVisible(showBtnClose);
	ui::Button* btnSubmit = (ui::Button*)popupNotice->getChildByName("btnsubmit");
	addTouchEventListener(btnSubmit, [=]() {
		func();
		hidePopup(popupNotice);
	}, false);
}

void BaseScene::showSplash()
{
	splash->setVisible(true);
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
			showPopupNotice(Utils::getSingleton().getStringForKey("connection_failed"), [=]() {
				SFSRequest::getSingleton().Disconnect();
				Utils::getSingleton().goToLoginScene();
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
	btn1->loadTextureNormal("popup/box1.png");
	btn->loadTextureNormal("popup/box2.png");
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
	for (int i = 0; i < listRanks[type].size(); i++) {
		Node* node = popupRank->getChildByTag(10 + i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scroll->getContentSize().width / 2, height - 30 - i * 55);
			node->setTag(10 + i);
			scroll->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::create("popup/box1.png");
			nbg->setContentSize(Size(width, 65));
			node->addChild(nbg);

			Label* lb1 = Label::create(to_string(i + 1), "fonts/guanine.ttf", 25);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(Color3B(50, 50, 50));
			lb1->setPosition(-width / 2 + 70, 5);
			lb1->setTag(1);
			node->addChild(lb1);

			Label* lb2 = Label::create(listRanks[type][i].Name, "fonts/arialbd.ttf", 25);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setColor(Color3B(50, 50, 50));
			lb2->setPosition(-width / 2 + 150, 0);
			lb2->setWidth(400);
			lb2->setHeight(30);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::create(Utils::getSingleton().formatMoneyWithComma(listRanks[type][i].Money), "fonts/arialbd.ttf", 25);
			lb3->setAnchorPoint(Vec2(1, .5f));
			lb3->setColor(Color3B(153, 76, 0));
			lb3->setPosition(width / 2 - 50, 0);
			lb3->setTag(3);
			node->addChild(lb3);
		} else {
			node->setVisible(true);
			Label* lb2 = (Label*)node->getChildByTag(2);
			Label* lb3 = (Label*)node->getChildByTag(3);
			lb2->setString(listRanks[type][i].Name);
			lb3->setString(Utils::getSingleton().formatMoneyWithComma(listRanks[type][i].Money));
		}
	}
	int i = 20;
	Node* n;
	while ((n = popupRank->getChildByTag(10 + i)) != nullptr) {
		n->setVisible(false);
	}
}

void BaseScene::showPopupRankWin()
{
	/*listRankWin.clear();
	for (int i = 0; i < 20; i++) {
		RankWinData data;
		data.Uid = 1000 + rand() % 10000;
		data.Name = "stormus" + to_string(data.Uid);
		data.Point = rand() % 60;
		data.Cuoc = "Nha lau xe hoi Hoa roi cua phat Ca nhay dau thuyen ngu ong bat ca Chua do nat hoa";
		data.Date = "05/01/2016";
		listRankWin.push_back(data);
	}*/

	ui::ScrollView* scroll = (ui::ScrollView*)popupRank->getChildByName("scroll");
	ui::ScrollView* scrollWin = (ui::ScrollView*)popupRank->getChildByName("scrollwin");
	scroll->setVisible(false);
	scrollWin->setVisible(true);
	int height = listRankWin.size() * 75;
	int width = scrollWin->getContentSize().width;
	scrollWin->setInnerContainerSize(Size(width, height));
	for (int i = 0; i < listRankWin.size(); i++) {
		Node* node = popupRank->getChildByTag(10 + i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scrollWin->getContentSize().width / 2, height - 40 - i * 75);
			node->setTag(10 + i);
			scrollWin->addChild(node);

			ui::Scale9Sprite* nbg = ui::Scale9Sprite::create("popup/box1.png");
			nbg->setContentSize(Size(width, 85));
			node->addChild(nbg);

			Label* lb1 = Label::create(to_string(i + 1), "fonts/guanine.ttf", 25);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(Color3B(50, 50, 50));
			lb1->setPosition(-width / 2 + 70, 5);
			lb1->setTag(1);
			node->addChild(lb1);

			Label* lb2 = Label::create(listRankWin[i].Name, "fonts/arial.ttf", 20);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setColor(Color3B(50, 50, 50));
			lb2->setPosition(-width / 2 + 150, 10);
			lb2->setWidth(400);
			lb2->setHeight(30);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::create(listRankWin[i].Cuoc, "fonts/guanine.ttf", 20);
			lb3->setAnchorPoint(Vec2(0, .5f));
			lb3->setColor(Color3B::RED);
			lb3->setPosition(-width / 2 + 150, -10);
			lb3->setWidth(600);
			lb3->setHeight(30);
			lb3->setTag(3);
			node->addChild(lb3);

			Label* lb4 = Label::create(Utils::getSingleton().formatMoneyWithComma(listRankWin[i].Point), "fonts/arialbd.ttf", 25);
			lb4->setAnchorPoint(Vec2(1, .5f));
			lb4->setColor(Color3B::RED);
			lb4->setPosition(width / 2 - 50, 0);
			lb4->setTag(4);
			node->addChild(lb4);
		} else {
			node->setVisible(true);
			Label* lb2 = (Label*)node->getChildByTag(2);
			Label* lb3 = (Label*)node->getChildByTag(3);
			Label* lb4 = (Label*)node->getChildByTag(4);
			lb2->setString(listRankWin[i].Name);
			lb3->setString(listRankWin[i].Cuoc);
			lb4->setString(Utils::getSingleton().formatMoneyWithComma(listRankWin[i].Point));
		}
	}
	int i = 20;
	Node* n;
	while ((n = popupRank->getChildByTag(10 + i)) != nullptr) {
		n->setVisible(false);
	}
}

void BaseScene::showPopupUserInfo(UserData data, bool showHistoryIfIsMe)
{
	showPopup(popupUserInfo);
	Node* btnHistory = popupUserInfo->getChildByName("btnhistory");
	Node* btnActive = popupUserInfo->getChildByName("btnactive");
    Node* btnFB = popupUserInfo->getChildByName("btnlogoutfb");
	Label* lbName = (Label*)popupUserInfo->getChildByName("lbname");
	Label* lbQuan = (Label*)popupUserInfo->getChildByName("lbquan");
	Label* lbXu = (Label*)popupUserInfo->getChildByName("lbxu");
	Label* lbId = (Label*)popupUserInfo->getChildByName("lbid");
	Label* lbLevel = (Label*)popupUserInfo->getChildByName("lblevel");
	Label* lbWin = (Label*)popupUserInfo->getChildByName("lbwin");
	Label* lbTotal = (Label*)popupUserInfo->getChildByName("lbtotal");

	btnHistory->setVisible(showHistoryIfIsMe && data.UserID == Utils::getSingleton().userDataMe.UserID);
	btnActive->setVisible(showHistoryIfIsMe && data.UserID == Utils::getSingleton().userDataMe.UserID && !Utils::getSingleton().userDataMe.IsActived);
    btnFB->setVisible(showHistoryIfIsMe && data.UserID == Utils::getSingleton().userDataMe.UserID && Utils::getSingleton().loginType == constant::LOGIN_FACEBOOK);
	lbName->setString(data.DisplayName);
	lbQuan->setString(Utils::getSingleton().formatMoneyWithComma(data.MoneyReal));
	lbXu->setString(Utils::getSingleton().formatMoneyWithComma(data.MoneyFree));
	lbId->setString("ID: " + (data.UserID == Utils::getSingleton().userDataMe.UserID ? to_string(data.UserID) : ""));
	lbLevel->setString(Utils::getSingleton().getStringForKey("cap_do") + ": " + to_string(data.Level));
	lbWin->setString(Utils::getSingleton().getStringForKey("thang") + ": " + to_string(data.Win));
	lbTotal->setString(Utils::getSingleton().getStringForKey("tong") + ": " + to_string(data.Total));
}

void BaseScene::setMoneyType(int type)
{
	moneyBg->setTag(type);
	chosenBg->setPosition(type == 0 ? 100 : -95, 0);
	//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), type == 1);
}

void BaseScene::handleClientDisconnectionReason(std::string reason)
{
	if (isOverlapLogin) {
		reason = "overlap_login";
	}
	showPopupNotice(Utils::getSingleton().getStringForKey("disconnection_" + reason), [=]() {
		if (reason.compare(constant::DISCONNECTION_REASON_UNKNOWN) == 0) {
			isReconnecting = true;
			Utils::getSingleton().reconnect();
			showWaiting();
		} else {
			Utils::getSingleton().goToLoginScene();
		}
	}, false);
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
	if (code == 38) {
		isOverlapLogin = true;
		return;
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
	showPopupNotice(Utils::getSingleton().getStringForKey("connection_failed"), [=]() {
		SFSRequest::getSingleton().Disconnect();
		Utils::getSingleton().goToLoginScene();
	}, false);
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
		SFSRequest::getSingleton().Disconnect();
		Utils::getSingleton().goToLoginScene();
	}
}

void BaseScene::initHeaderWithInfos()
{
	hasHeader = true;
	bool isRealMoney = Utils::getSingleton().moneyType == 1;

	std::vector<Vec2> vecPos;
	vecPos.push_back(Vec2(62, 650));
	vecPos.push_back(Vec2(330, 650));
	vecPos.push_back(Vec2(430, 650));
	vecPos.push_back(Vec2(165, 650));
	vecPos.push_back(Vec2(360, 650));
	vecPos.push_back(Vec2(720, 650));
	vecPos.push_back(Vec2(808, 650));
	vecPos.push_back(Vec2(904, 650));
	vecPos.push_back(Vec2(190, 650));
	vecPos.push_back(Vec2(386, 650));
	vecPos.push_back(Vec2(954, 675));
	vecPos.push_back(Vec2(953, 647));
	vecPos.push_back(Vec2(953, 624));
	vecPos.push_back(Vec2(560, 590));
	vecPos.push_back(Vec2(640, 650));

	ui::Button* btnBack = ui::Button::create("main/back.png");
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

	moneyBg = ui::Button::create("main/money_bg.png");
	moneyBg->setTag((int)isRealMoney);
	moneyNode->addChild(moneyBg, 0);

	chosenBg = Sprite::create("main/chosen_bg.png");
	chosenBg->setPosition(isRealMoney ? -95 : 100, 0);
	moneyNode->addChild(chosenBg, 1);
	
	moneyBg->setBright(false);
	addTouchEventListener(moneyBg, [=]() {
		if (moneyBg->getTag() == 0) {
			moneyBg->setTag(1);
			chosenBg->setPosition(-95, 0);
			onChangeMoneyType(1);
			//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), true);
		} else {
			moneyBg->setTag(0);
			chosenBg->setPosition(100, 0);
			onChangeMoneyType(0);
			//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), false);
		}
	});

	Sprite* iconGold = Sprite::create("main/icon_gold.png");
	iconGold->setPosition(-165, 0);
	moneyNode->addChild(iconGold, 2);

	Sprite* iconSilver = Sprite::create("main/icon_silver.png");
	iconSilver->setPosition(30, 0);
	moneyNode->addChild(iconSilver, 2);

	ui::Button* btnFacebook = ui::Button::create("main/facebook.png");
	btnFacebook->setPosition(vecPos[14]);
	addTouchEventListener(btnFacebook, [=]() {
		Application::sharedApplication()->openURL(Utils::getSingleton().gameConfig.linkFb);
	});
	mLayer->addChild(btnFacebook, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnFacebook);

	ui::Button* btnRank = ui::Button::create("main/rank.png");
	btnRank->setPosition(vecPos[5]);
	addTouchEventListener(btnRank, [=]() {
		if (listRanks.size() == 0) {
			SFSRequest::getSingleton().RequestRank();
			SFSRequest::getSingleton().RequestRankWin();
		} else {
			showPopupRank(0);
		}
	});
	mLayer->addChild(btnRank, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnRank);

	ui::Button* btnSettings = ui::Button::create("main/settings.png");
	btnSettings->setPosition(vecPos[6]);
	addTouchEventListener(btnSettings, [=]() {
		if (popupMainSettings == nullptr) {
			initPopupSettings();
		}
		showPopup(popupMainSettings);
	});
	mLayer->addChild(btnSettings, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnSettings);

	ui::Button* btnAvar = ui::Button::create("main/avatar.png");
	btnAvar->setPosition(vecPos[7]);
	addTouchEventListener(btnAvar, [=]() {
		showPopupUserInfo(Utils::getSingleton().userDataMe);
	});
	mLayer->addChild(btnAvar, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnAvar);

	lbGold = Label::create("0", "fonts/arialbd.ttf", 25);
	lbGold->setAnchorPoint(Vec2(0, .5f));
	lbGold->setPosition(vecPos[8] - vecPos[1]);
	lbGold->setColor(Color3B::YELLOW);
	moneyNode->addChild(lbGold, 2);

	lbSilver = Label::create("0", "fonts/arialbd.ttf", 25);
	lbSilver->setAnchorPoint(Vec2(0, .5f));
	lbSilver->setPosition(vecPos[9] - vecPos[1]);
	lbSilver->setColor(Color3B(0, 255, 255));
	moneyNode->addChild(lbSilver, 2);

	lbName = Label::create("Name", "fonts/arialbd.ttf", 23);
	lbName->setAnchorPoint(Vec2(0, .5f));
	lbName->setPosition(vecPos[10]);
	lbName->setWidth(150);
	lbName->setHeight(25);
	mLayer->addChild(lbName, constant::MAIN_ZORDER_HEADER);

	lbId = Label::create("ID: ", "fonts/arialbd.ttf", 23);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setPosition(vecPos[11]);
	mLayer->addChild(lbId, constant::MAIN_ZORDER_HEADER);

	lbLevel = Label::create("Level: ", "fonts/arialbd.ttf", 23);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setPosition(vecPos[12]);
	mLayer->addChild(lbLevel, constant::MAIN_ZORDER_HEADER);

	int x = 0;
	while (x < 1120) {
		Sprite* line = Sprite::create("main/line.png");
		line->setPosition(x, vecPos[13].y);
		mLayer->addChild(line, constant::MAIN_ZORDER_HEADER);
		x += line->getContentSize().width;
	}

	if (Utils::getSingleton().userDataMe.UserID > 0) {
		onUserDataMeResponse();
	}
}

void BaseScene::onBackScene()
{

}

void BaseScene::showPopupHistory()
{
	if (popupHistory == nullptr) {
		initPopupHistory();
	}
	showPopup(popupHistory);
	SFSRequest::getSingleton().RequestPlayHistory(0, 0);

	ui::Button* btn0 = (ui::Button*)popupHistory->getChildByTag(10);
	ui::Button* btn1 = (ui::Button*)popupHistory->getChildByTag(11);
	btn0->loadTextureNormal("popup/box2.png");
	btn1->loadTextureNormal("popup/box1.png");
	popupHistory->setTag(0);
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

void BaseScene::onChangeMoneyType(int type)
{
}

void BaseScene::onKeyBack()
{
}

void BaseScene::onKeyHome()
{
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

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("white.png");
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

void BaseScene::showToast(std::string msg, Vec2 pos, Color3B textColor, Color3B bgColor, int bgOpacity)
{
	Node* nodeChat = Node::create();
	nodeChat->setPosition(pos);
	nodeChat->setScale(0);
	mLayer->addChild(nodeChat, constant::GAME_ZORDER_BUTTON);

	cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/chat.xml");
	std::string ss = plist[msg].asString();
	if (ss.length() > 0) msg = ss;

	Label* lb = Label::create();
	lb->setString(msg);
	lb->setSystemFontName("Arial");
	lb->setSystemFontSize(25);
	lb->setColor(textColor);
	if (lb->getWidth() > 500) {
		lb->setWidth(500);
	}
	if (lb->getHeight() > 50) {
		lb->setHeight(50);
	}
	nodeChat->addChild(lb, 2);

	Size sizeContent = lb->getContentSize();
	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/box5.png");
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

	Sprite* bg = Sprite::create("popup/bg.png");
	popupNotice->addChild(bg);

	Sprite* title = Sprite::create("popup/title_thongbao.png");
	title->setPosition(0, 180);
	title->setScale(.8f);
	popupNotice->addChild(title);

	Label* lb = Label::create();
	lb->setColor(Color3B::BLACK);
	lb->setSystemFontSize(30);
	lb->setWidth(600);
	lb->setName("lbcontent");
	lb->setAlignment(TextHAlignment::CENTER);
	popupNotice->addChild(lb);

	ui::Button* btnok = ui::Button::create("popup/btn_submit.png", "popup/btn_submit_clicked.png");
	btnok->setPosition(Vec2(0, -190));
	btnok->setName("btnsubmit");
	addTouchEventListener(btnok, [=]() {});
	popupNotice->addChild(btnok);

	ui::Button* btndong = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btndong->setPosition(Vec2(290, 170));
	btndong->setScale(.7f);
	btndong->setName("btnclose");
	addTouchEventListener(btndong, [=]() {
		hidePopup(popupNotice);
	});
	popupNotice->addChild(btndong);

	return popupNotice;
}

cocos2d::Vec2 BaseScene::getScaleSmoothly(float scale)
{
	return cocos2d::Vec2(scale * scaleScene.y, scale * scaleScene.x);
}

void BaseScene::initPopupRank()
{
	popupRank = Node::create();
	popupRank->setPosition(560, 350);
	popupRank->setVisible(false);
	popupRank->setTag(0);
	mLayer->addChild(popupRank, constant::ZORDER_POPUP);
	autoScaleNode(popupRank);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupRank->addChild(bg);

	Sprite* title = Sprite::create("popup/title_bangxephang.png");
	title->setPosition(0, 250);
	popupRank->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(390, 250));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupRank);
	});
	popupRank->addChild(btnClose);

	int w = 850;
	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-w / 2, -227));
	scroll->setContentSize(Size(w, 350));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupRank->addChild(scroll);

	ui::ScrollView* scrollWin = ui::ScrollView::create();
	scrollWin->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollWin->setBounceEnabled(true);
	scrollWin->setPosition(Vec2(-w / 2, -227));
	scrollWin->setContentSize(Size(w, 350));
	scrollWin->setScrollBarEnabled(false);
	scrollWin->setName("scrollwin");
	popupRank->addChild(scrollWin);

	vector<string> texts = { "quan", "xu", "win" };
	int x = -315;
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "popup/box2.png" : "popup/box1.png");
		btn->setContentSize(Size(210, 70));
		btn->setPosition(Vec2(x, 160));
		btn->setScale9Enabled(true);
		btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			showPopupRank(btn->getTag());
		});
		popupRank->addChild(btn);

		Label* lb = Label::create(Utils::getSingleton().getStringForKey(texts[i]), "fonts/guanine.ttf", 30);
		lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 5);
		lb->setColor(Color3B::BLACK);
		btn->addChild(lb);

		x += 210;
	}
}

void BaseScene::initPopupSettings()
{
	popupMainSettings = Node::create();
	popupMainSettings->setPosition(560, 350);
	popupMainSettings->setVisible(false);
	mLayer->addChild(popupMainSettings, constant::ZORDER_POPUP);
	autoScaleNode(popupMainSettings);

	Sprite* bg = Sprite::create("popup/bg.png");
	popupMainSettings->addChild(bg);

	Sprite* title = Sprite::create("popup/title_caidat.png");
	title->setPosition(0, 180);
	title->setScale(.8f);
	popupMainSettings->addChild(title);

	vector<Vec2> vecPos;
	vecPos.push_back(Vec2(-180, 50));
	vecPos.push_back(Vec2(-180, -50));

	vector<Label*> lbs;
	vector<ui::CheckBox*> cbs;
	for (int i = 0; i < 2; i++) {
		ui::CheckBox* checkbox = ui::CheckBox::create();
		checkbox->loadTextureBackGround("popup/box.png");
		checkbox->loadTextureFrontCross("popup/check.png");
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
		lb->setColor(Color3B::BLACK);
		popupMainSettings->addChild(lb);
		lbs.push_back(lb);
	}

	cbs[0]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_SOUND.c_str()));
	cbs[1]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_INVITATION.c_str()));

	lbs[0]->setString(Utils::getSingleton().getStringForKey("sound"));
	lbs[1]->setString(Utils::getSingleton().getStringForKey("reject_all_invitation"));

	ui::Button* btnOK = ui::Button::create("popup/btn_submit.png", "popup/btn_submit_clicked.png");
	btnOK->setPosition(Vec2(0, -190));
	addTouchEventListener(btnOK, [=]() {
		hidePopup(popupMainSettings);
		Utils::getSingleton().SoundEnabled = cbs[0]->isSelected();
		Utils::getSingleton().IgnoreInvitation = cbs[1]->isSelected();
		UserDefault::getInstance()->setBoolForKey(constant::KEY_SOUND.c_str(), cbs[0]->isSelected());
		UserDefault::getInstance()->setBoolForKey(constant::KEY_INVITATION.c_str(), cbs[1]->isSelected());
	});
	popupMainSettings->addChild(btnOK);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(290, 170));
	btnClose->setScale(.7f);
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupMainSettings);
	});
	popupMainSettings->addChild(btnClose);
}

void BaseScene::initPopupUserInfo()
{
	popupUserInfo = Node::create();
	popupUserInfo->setPosition(560, 350);
	popupUserInfo->setVisible(false);
	mLayer->addChild(popupUserInfo, constant::ZORDER_POPUP);
	autoScaleNode(popupUserInfo);

	Sprite* bg = Sprite::create("popup/bg.png");
	popupUserInfo->addChild(bg);

	Sprite* title = Sprite::create("popup/title_thongtin.png");
	title->setPosition(0, 180);
	title->setScale(.8f);
	popupUserInfo->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(290, 170));
	btnClose->setScale(.7f);
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupUserInfo);
	});
	popupUserInfo->addChild(btnClose);

	Sprite* avatar = Sprite::create("board/avatar_default.png");
	avatar->setPosition(-186, 23);
	avatar->setScale(1.5f);
	avatar->setName("avatar");
	popupUserInfo->addChild(avatar);

	ui::Button* btnHistory = ui::Button::create("popup/btn_small_invite.png");
	btnHistory->setTitleText(Utils::getSingleton().getStringForKey("lich_su"));
	btnHistory->setTitleFontName("fonts/arial.ttf");
	btnHistory->setTitleFontSize(25);
	btnHistory->setTitleColor(Color3B::BLACK);
	btnHistory->setPosition(Vec2(-186, -80));
	btnHistory->setContentSize(Size(150, 50));
	btnHistory->setScale9Enabled(true);
	btnHistory->setName("btnhistory");
	addTouchEventListener(btnHistory, [=]() {
		showPopupHistory();
	});
	popupUserInfo->addChild(btnHistory);

	ui::Button* btnActive = ui::Button::create("popup/btn_small_invite.png");
	btnActive->setTitleText(Utils::getSingleton().getStringForKey("kich_hoat"));
	btnActive->setTitleFontName("fonts/arial.ttf");
	btnActive->setTitleFontSize(25);
	btnActive->setTitleColor(Color3B::BLACK);
	btnActive->setPosition(Vec2(-186, -135));
	btnActive->setContentSize(Size(150, 50));
	btnActive->setScale9Enabled(true);
	btnActive->setName("btnactive");
	addTouchEventListener(btnActive, [=]() {
		string str = Utils::getSingleton().gameConfig.smsKH;
		int index = str.find_last_of(' ');
		string number = str.substr(index + 1, str.length() - index);
		string content = str.substr(0, index);
		content = Utils::getSingleton().replaceString(content, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
		Utils::getSingleton().openSMS(number, content);
	});
	popupUserInfo->addChild(btnActive);

    ui::Button* btnLogoutFb = ui::Button::create("main/btn_logout_fb.png", "main/btn_logout_fb_clicked.png");
    btnLogoutFb->setPosition(Vec2(186, -135));
    btnLogoutFb->setName("btnlogoutfb");
    btnLogoutFb->setScale(.8f);
    addTouchEventListener(btnLogoutFb, [=]() {
        SFSRequest::getSingleton().Disconnect();
        Utils::getSingleton().logoutFacebook();
        Utils::getSingleton().goToLoginScene();
    });
    popupUserInfo->addChild(btnLogoutFb);

	Label* lbName = Label::create("Stormus", "fonts/arialbd.ttf", 25);
	lbName->setAnchorPoint(Vec2(0, .5f));
	lbName->setColor(Color3B::BLACK);
	lbName->setPosition(-70, 80);
	lbName->setName("lbname");
	popupUserInfo->addChild(lbName);

	Label* lbQuan = Label::create(Utils::getSingleton().getStringForKey("quan"), "fonts/arial.ttf", 25);
	lbQuan->setAnchorPoint(Vec2(0, .5f));
	lbQuan->setColor(Color3B::BLACK);
	lbQuan->setPosition(-70, 40);
	popupUserInfo->addChild(lbQuan);

	Label* lbQuan1 = Label::create("100,000", "fonts/arial.ttf", 25);
	lbQuan1->setAnchorPoint(Vec2(0, .5f));
	lbQuan1->setColor(Color3B::RED);
	lbQuan1->setPosition(30, 40);
	lbQuan1->setName("lbquan");
	popupUserInfo->addChild(lbQuan1);

	Label* lbXu = Label::create(Utils::getSingleton().getStringForKey("xu"), "fonts/arial.ttf", 25);
	lbXu->setAnchorPoint(Vec2(0, .5f));
	lbXu->setColor(Color3B::BLACK);
	lbXu->setPosition(-70, 0);
	popupUserInfo->addChild(lbXu);

	Label* lbXu1 = Label::create("100,000", "fonts/arial.ttf", 25);
	lbXu1->setAnchorPoint(Vec2(0, .5f));
	lbXu1->setColor(Color3B(0, 255, 255));
	lbXu1->setPosition(30, 0);
	lbXu1->setName("lbxu");
	popupUserInfo->addChild(lbXu1);

	Label* lbId = Label::create("ID: 847558", "fonts/arial.ttf", 25);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setColor(Color3B::BLACK);
	lbId->setPosition(-70, -40);
	lbId->setName("lbid");
	popupUserInfo->addChild(lbId);
	
	Label* lbLevel = Label::create(Utils::getSingleton().getStringForKey("cap_do") + ": 10", "fonts/arial.ttf", 25);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setColor(Color3B::BLACK);
	lbLevel->setPosition(-70, -80);
	lbLevel->setName("lblevel");
	popupUserInfo->addChild(lbLevel);

	Label* lbWin = Label::create(Utils::getSingleton().getStringForKey("thang") + ": 12", "fonts/arial.ttf", 25);
	lbWin->setAnchorPoint(Vec2(0, .5f));
	lbWin->setColor(Color3B::BLACK);
	lbWin->setPosition(115, -40);
	lbWin->setName("lbwin");
	popupUserInfo->addChild(lbWin);

	Label* lbTotal = Label::create(Utils::getSingleton().getStringForKey("tong") + ": 20", "fonts/arial.ttf", 25);
	lbTotal->setAnchorPoint(Vec2(0, .5f));
	lbTotal->setColor(Color3B::BLACK);
	lbTotal->setPosition(115, -80);
	lbTotal->setName("lbtotal");
	popupUserInfo->addChild(lbTotal);
}

void BaseScene::initPopupHistory()
{
	popupHistory = Node::create();
	popupHistory->setPosition(560, 350);
	popupHistory->setVisible(false);
	popupHistory->setTag(0);
	mLayer->addChild(popupHistory, constant::ZORDER_POPUP);
	autoScaleNode(popupHistory);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupHistory->addChild(bg);

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::create("popup/box1.png");
	bgContent->setContentSize(Size(860, 370));
	bgContent->setPosition(0, -55);
	popupHistory->addChild(bgContent);

	Sprite* title = Sprite::create("popup/title_lichsu.png");
	title->setPosition(0, 250);
	title->setName("sptitle");
	popupHistory->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(390, 250));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupHistory);
	});
	popupHistory->addChild(btnClose);

	int w = 850;
	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-w / 2, -227));
	scroll->setContentSize(Size(w, 300));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupHistory->addChild(scroll);

	vector<string> texts = { "quan" , "xu" };
	int x = -315;
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "popup/box2.png" : "popup/box1.png");
		btn->setContentSize(Size(210, 70));
		btn->setPosition(Vec2(x, 160));
		btn->setScale9Enabled(true);
		btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(10 + i);
		addTouchEventListener(btn, [=]() {
			ui::Button* btn1 = (ui::Button*)popupHistory->getChildByTag(10 + popupHistory->getTag());
			btn1->loadTextureNormal("popup/box1.png");
			btn->loadTextureNormal("popup/box2.png");
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

		Label* lb = Label::create(Utils::getSingleton().getStringForKey(texts[i]), "fonts/guanine.ttf", 30);
		lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 5);
		lb->setColor(Color3B::BLACK);
		btn->addChild(lb);

		x += 210;
	}

	vector<int> posX = { -390, -300, -80, 160, 290, 380 };
	vector<int> widths = { 50, 100, 320, 210, 150, 60 };
	vector<string> historyTitles = { "STT", Utils::getSingleton().getStringForKey("ngay"), Utils::getSingleton().getStringForKey("thong_tin"),
		Utils::getSingleton().getStringForKey("tien_van"), Utils::getSingleton().getStringForKey("tien"), "ID" };
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lb = Label::create(historyTitles[i], "fonts/aurora.ttf", 30);
		lb->setColor(Color3B(50, 50, 50));
		lb->setPosition(posX[i], 98);
		popupHistory->addChild(lb);
	}

	Node* nodeDetail = Node::create();
	nodeDetail->setPosition(0, -75);
	nodeDetail->setVisible(false);
	nodeDetail->setName("nodedetail");
	popupHistory->addChild(nodeDetail);

	ui::Scale9Sprite* bgDetail = ui::Scale9Sprite::create("popup/box1.png");
	bgDetail->setContentSize(Size(860, 326));
	nodeDetail->addChild(bgDetail);

	for (int i = 0; i < posX.size(); i++) {
		Label* lbDetail = Label::create("s", "fonts/arial.ttf", 20);
		lbDetail->setWidth(widths[i]);
		lbDetail->setAnchorPoint(Vec2(.5f, 1));
		lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
		lbDetail->setPosition(posX[i], bgDetail->getContentSize().height / 2 - 20);
		lbDetail->setTag(i);
		lbDetail->setColor(Color3B(50, 50, 50));
		nodeDetail->addChild(lbDetail);
	}

	ui::Button* btnCloseDetail = ui::Button::create("popup/btn_dong.png");
	btnCloseDetail->setPosition(Vec2(bgDetail->getContentSize().width / 2 - 50, -bgDetail->getContentSize().height / 2 + 50));
	btnCloseDetail->setScale(.7f);
	addTouchEventListener(btnCloseDetail, [=]() {
		nodeDetail->setVisible(false);
	});
	nodeDetail->addChild(btnCloseDetail);

	addBtnChoosePage(-100, -260, popupHistory, [=](int page) {
		int type = popupHistory->getTag();
		SFSRequest::getSingleton().RequestPlayHistory(type, page - 1);
		//onPlayLogDataResponse(vector<PlayLogData>());
	});
}

void BaseScene::onPingPong(long timems)
{
	pingId++;
	if (timems < 150) {
		spNetwork->initWithFile("wifi0.png");
	} else if (timems < 500) {
		spNetwork->initWithFile("wifi1.png");
	} else if (timems < 1000) {
		spNetwork->initWithFile("wifi2.png");
	} else {
		spNetwork->initWithFile("wifi3.png");
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
	std::string strLevel = String::createWithFormat("Level: %d", Utils::getSingleton().userDataMe.Level)->getCString();

	lbName->setString(Utils::getSingleton().userDataMe.DisplayName);
	lbGold->setString(strGold);
	lbSilver->setString(strSilver);
	lbId->setString(strId);
	lbLevel->setString(strLevel);
}

void BaseScene::onRankDataResponse(std::vector<std::vector<RankData>> list)
{
	this->listRanks = list;
	showPopupRank(0);
}

void BaseScene::onRankWinDataResponse(std::vector<RankWinData> list)
{
	this->listRankWin = list;
}

void BaseScene::onListEventDataResponse(std::vector<EventData> list)
{
	if (eventView == nullptr || !Utils::getSingleton().gameConfig.paymentEnabled) return;
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

	vector<int> posX = { -390, -300, -80, 160, 290, 380 };
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
				lbDetail->setColor(Color3B(50, 50, 50));
				scroll->addChild(lbDetail);
				lbs.push_back(lbDetail);
			}

			btn = ui::Button::create("popup/box1.png");
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
		lbs[2]->setString(logs[i].Info);
		lbs[3]->setString(Utils::getSingleton().formatMoneyWithComma(logs[i].Money));
		lbs[4]->setString(Utils::getSingleton().formatMoneyWithComma(logs[i].Balance));
		lbs[5]->setString(to_string(logs[i].GameId));
	}

	int count = scroll->getChildrenCount();
	for (int i = logs.size() * 7; i < scroll->getChildrenCount(); i++) {
		scroll->getChildByTag(i)->setVisible(false);
	}
}

void BaseScene::addBtnChoosePage(int x, int y, cocos2d::Node * node, std::function<void(int)> funcPage)
{
	Node* nodePage = Node::create();
	nodePage->setTag(1);
	nodePage->setName("nodepage");
	node->addChild(nodePage);

	int x1 = -100;
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = ui::Button::create("popup/box5.png");
		btn->setTitleFontName("fonts/arial.ttf");
		btn->setTitleFontSize(20);
		btn->setTitleText(to_string(i));
		btn->setTitleColor(Color3B::BLACK);
		btn->setContentSize(Size(40, 40));
		btn->setPosition(Vec2(x1, -260));
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

	ui::Button* btnPre = ui::Button::create("popup/box5.png");
	btnPre->setTitleFontName("fonts/arial.ttf");
	btnPre->setTitleFontSize(20);
	btnPre->setTitleText("<<");
	btnPre->setTitleColor(Color3B::BLACK);
	btnPre->setContentSize(Size(60, 40));
	btnPre->setPosition(Vec2(-160, -260));
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

	ui::Button* btnNext = ui::Button::create("popup/box5.png");
	btnNext->setTitleFontName("fonts/arial.ttf");
	btnNext->setTitleFontSize(20);
	btnNext->setTitleText(">>");
	btnNext->setTitleColor(Color3B::BLACK);
	btnNext->setContentSize(Size(60, 40));
	btnNext->setPosition(Vec2(160, -260));
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
