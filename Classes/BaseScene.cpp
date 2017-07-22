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
	myRealMoney = Utils::getSingleton().userDataMe.MoneyReal;

	mLayer = Layer::create();
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

	splash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
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

	spWaiting = Sprite::createWithSpriteFrameName("loadingicon.png");
	nodeWaiting->addChild(spWaiting);

	RotateBy* rotate = RotateBy::create(2, 360);
	spWaiting->runAction(RepeatForever::create(rotate));
	spWaiting->pauseSchedulerAndActions();

	spNetwork = Sprite::createWithSpriteFrameName("wifi0.png");
	//spNetwork->setAnchorPoint(Vec2(1, 0));
	//spNetwork->setPosition(1115, 5);
	spNetwork->setPosition(winSize.width - (ispmE ? 360 : 240) * scaleScene.y, 650);
	spNetwork->setVisible(false);
	mLayer->addChild(spNetwork, constant::GAME_ZORDER_SPLASH - 1);
	autoScaleNode(spNetwork);

	lbNetwork = Label::createWithTTF("0ms", "fonts/arial.ttf", 18);
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
	Node* popupNotice = createPopupNotice();
	showPopup(popupNotice);
	Label* lbcontent = (Label*)popupNotice->getChildByName("lbcontent");
	lbcontent->setString(msg);
	ui::Button* btnClose = (ui::Button*)popupNotice->getChildByName("btnclose");
	btnClose->setVisible(showBtnClose);
	ui::Button* btnSubmit = (ui::Button*)popupNotice->getChildByName("btnsubmit");
	btnSubmit->setVisible(func != NULL);
	if (func == NULL) {
		btnClose->setPosition(Vec2(0, -180));
	} else {
		if (showBtnClose) {
			btnClose->setPosition(Vec2(110, -180));
			btnSubmit->setPosition(Vec2(-110, -180));
		} else {
			btnSubmit->setPosition(Vec2(0, -180));
		}
	}
	addTouchEventListener(btnSubmit, [=]() {
		popupNotice->stopAllActions();
		if(func != NULL) func();
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
			lb = Label::createWithTTF("", "fonts/arial.ttf", 20);
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
	btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
	btn->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
	btn1->setTitleColor(Color3B::WHITE);
	btn->setTitleColor(Color3B::BLACK);
	popupRank->setTag(type);

	if (type == 2) {
		showPopupRankWin();
		return;
	}

	ui::ScrollView* scroll = (ui::ScrollView*)popupRank->getChildByName("scroll");
	ui::ScrollView* scrollWin = (ui::ScrollView*)popupRank->getChildByName("scrollwin");
	Node* nodeTop = popupRank->getChildByName("nodetop");
	scroll->setVisible(true);
	nodeTop->setVisible(true);
	scrollWin->setVisible(false);
	int rowHeight = 43;
	int width = scroll->getContentSize().width;
	int height = (listRanks[type].size() - 3) * rowHeight;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(width, height));
	for (int i = 0; i < 3; i++) {
		Node* topItem = nodeTop->getChildByTag(i);
		Label* lbName = (Label*)topItem->getChildByName("lbname");
		Label* lbMoney = (Label*)topItem->getChildByName("lbmoney");
		if (i < listRanks[type].size()) {
			lbMoney->setString(Utils::getSingleton().formatMoneyWithComma(listRanks[type][i].Money));
			lbName->setString(listRanks[type][i].Name);
			cropLabel(lbName, 150);
		} else {
			lbName->setString("");
			lbMoney->setString("");
		}
	}
	for (int i = 3; i < listRanks[type].size(); i++) {
		Node* node = scroll->getChildByTag(i);
		Label *lb2, *lb3;
		if (node == nullptr) {
			node = Node::create();
			node->setTag(i);
			scroll->addChild(node);

			Label* lb1 = Label::createWithTTF("#" + to_string(i + 1), "fonts/arial.ttf", 22);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(Color3B::BLACK);
			lb1->setPosition(-width / 2 + 20, 0);
			lb1->setTag(1);
			node->addChild(lb1);

			lb2 = Label::createWithTTF("", "fonts/arial.ttf", 22);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setColor(Color3B::BLACK);
			lb2->setPosition(-width / 2 + 150, 0);
			lb2->setTag(2);
			node->addChild(lb2);

			lb3 = Label::createWithTTF("", "fonts/arial.ttf", 22);
			lb3->setAnchorPoint(Vec2(0, .5f));
			lb3->setColor(Color3B::BLACK);
			lb3->setPosition(width / 2 - 160, 0);
			lb3->setTag(3);
			node->addChild(lb3);
		} else {
			lb2 = (Label*)node->getChildByTag(2);
			lb3 = (Label*)node->getChildByTag(3);
		}
		node->setVisible(true);
		node->setPosition(scroll->getContentSize().width / 2, height - 20 - (i - 3) * rowHeight);
		lb3->setString(Utils::getSingleton().formatMoneyWithComma(listRanks[type][i].Money));
		lb2->setString(listRanks[type][i].Name);
		cropLabel(lb2, 200);
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
	Node* nodeTop = popupRank->getChildByName("nodetop");
	scroll->setVisible(false);
	nodeTop->setVisible(false);
	scrollWin->setVisible(true);
	int rowHeight = 60;
	int height = listRankWin.size() * rowHeight;
	int width = scrollWin->getContentSize().width;
	if (height < scrollWin->getContentSize().height) {
		height = scrollWin->getContentSize().height;
	}
	scrollWin->setInnerContainerSize(Size(width, height));
	for (int i = 0; i < listRankWin.size(); i++) {
		Label* lb2;
		Node* node = scrollWin->getChildByTag(i);
		if (node == nullptr) {
			node = Node::create();
			node->setPosition(scrollWin->getContentSize().width / 2, height - 30 - i * rowHeight);
			node->setTag(i);
			scrollWin->addChild(node);

			Label* lb1 = Label::createWithTTF("#" + to_string(i + 1), "fonts/arial.ttf", 22);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(Color3B::BLACK);
			lb1->setPosition(-width / 2 + 20, 0);
			lb1->setTag(1);
			node->addChild(lb1);

			lb2 = Label::createWithTTF(listRankWin[i].Name, "fonts/arial.ttf", 22);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setColor(Color3B::BLACK);
			lb2->setPosition(-width / 2 + 90, 0);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::createWithTTF(listRankWin[i].Cuoc, "fonts/arial.ttf", 22);
			lb3->setAnchorPoint(Vec2(0, .5f));
			lb3->setColor(Color3B::BLACK);
			lb3->setPosition(-width / 2 + 320, 0);
			lb3->setWidth(450);
			lb3->setTag(3);
			node->addChild(lb3);

			Label* lb4 = Label::createWithTTF(Utils::getSingleton().formatMoneyWithComma(listRankWin[i].Point), "fonts/arial.ttf", 22);
			lb4->setAnchorPoint(Vec2(1, .5f));
			lb4->setColor(Color3B::BLACK);
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

		lb2->setString(listRankWin[i].Name);
		cropLabel(lb2, 200);
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
	Label* lbDName = (Label*)popupUserInfo->getChildByName("lbname");
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
	Sprite* spAvar = (Sprite*)popupUserInfo->getChildByName("avatar");
	Sprite* spOlAvar = (Sprite*)popupUserInfo->getChildByName("olavar");
	AppellationData aplData = Utils::getSingleton().getAppellationByLevel(data.Level);

	bool isMe = data.UserID == Utils::getSingleton().userDataMe.UserID;
	btnHistory->setVisible(showHistoryIfIsMe && isMe);
	btnActive->setVisible(showHistoryIfIsMe && isMe && !Utils::getSingleton().userDataMe.IsActived && Utils::getSingleton().ispmE());
	btnFB->setVisible(showHistoryIfIsMe && isMe && Utils::getSingleton().loginType == constant::LOGIN_FACEBOOK);
	lbUname->setVisible(isMe);
	lbUname1->setVisible(isMe);
	lbUname1->setString(data.Name);
	lbDName->setString(data.DisplayName);
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
	lbAppellation->enableOutline(aplData.ColorOutline, 1);
	//lbBigWin->setString(Utils::getSingleton().formatMoneyWithComma(data.BigWin));
	//lbBigCrest->setString(data.BigCrest);
	nodeInfo->setPosition(isMe ? lbDName->getPosition() - Vec2(0, 45) : lbDName->getPosition() + Vec2(0, 20));

	lbDName->setString(data.DisplayName);
	cropLabel(lbDName, 450);

	if (data.AvatarUrl.length() > 0) {
		Utils::getSingleton().LoadTextureFromURL(data.AvatarUrl, [=](Texture2D* texture) {
			spOlAvar->initWithTexture(texture);
			float originScale = spAvar->getScale();
			Size fsize = spAvar->getContentSize();
			Size spsize = spOlAvar->getContentSize();
			float scaleX = fsize.width / spsize.width;
			float scaleY = fsize.height / spsize.height;
			spOlAvar->setScale(originScale * (scaleX < scaleY ? scaleY : scaleX));
		});
	}
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

	std::vector<Vec2> vecPos;
	vecPos.push_back(Vec2(50 * scaleScene.y, 650));
	vecPos.push_back(Vec2(545, 650));
	vecPos.push_back(Vec2(430, 650));
	vecPos.push_back(Vec2(165, 650));
	vecPos.push_back(Vec2(360, 650));
	vecPos.push_back(Vec2(winSize.width - 150 * scaleScene.y, 650));
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
	moneyBg0->setPosition(Vec2(120, 0));
	moneyNode->addChild(moneyBg0, 0);

	moneyBg1 = ui::Button::create("money_bg.png", "money_bg.png", "", ui::Widget::TextureResType::PLIST);
	moneyBg1->setTag((int)isRealMoney);
	moneyBg1->setPosition(Vec2(-120, 0));
	moneyNode->addChild(moneyBg1, 0);

	chosenBg = Sprite::createWithSpriteFrameName("chosen_bg.png");
	chosenBg->setPosition(isRealMoney ? -120 : 120, 0);
	moneyNode->addChild(chosenBg, 1);
	
	moneyBg0->setBright(false);
	moneyBg1->setBright(false);
	addTouchEventListener(moneyBg0, [=]() {
		if (moneyBg0->getTag() == 1) {
			moneyBg0->setTag(0);
			moneyBg1->setTag(0);
			chosenBg->setPosition(120, 0);
			onChangeMoneyType(0);
			//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), false);
		}
	});
	addTouchEventListener(moneyBg1, [=]() {
		if (moneyBg0->getTag() == 0) {
			moneyBg0->setTag(1);
			moneyBg1->setTag(1);
			chosenBg->setPosition(-120, 0);
			onChangeMoneyType(1);
			//UserDefault::getInstance()->setBoolForKey(constant::KEY_MONEY_TYPE.c_str(), true);
		}
	});

    Sprite* iconGold = Sprite::createWithSpriteFrameName(ispmE ? "icon_gold.png" : "icon_silver.png");
	iconGold->setPosition(moneyBg1->getPositionX() - 80, 0);
	moneyNode->addChild(iconGold, 2);

	Sprite* iconSilver = Sprite::createWithSpriteFrameName("icon_silver.png");
	iconSilver->setPosition(moneyBg0->getPositionX() - 80, 0);
	moneyNode->addChild(iconSilver, 2);

	lbGold = Label::createWithTTF("0", "fonts/arialbd.ttf", 25);
	lbGold->setPosition(iconGold->getPosition() + Vec2(25, 0));
	lbGold->setAnchorPoint(Vec2(0, .5f));
	lbGold->setColor(Color3B::YELLOW);
	moneyNode->addChild(lbGold, 2);

	lbSilver = Label::createWithTTF("0", "fonts/arialbd.ttf", 25);
	lbSilver->setPosition(iconSilver->getPosition() + Vec2(25, 0));
	lbSilver->setAnchorPoint(Vec2(0, .5f));
	lbSilver->setColor(Color3B(0, 255, 255));
	moneyNode->addChild(lbSilver, 2);

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

	btnAvar = ui::Button::create("avatar_default.png", "avatar_default.png", "", ui::Widget::TextureResType::PLIST);
	btnAvar->setPosition(vecPos[7]);
	btnAvar->setScale(.9f);
	addTouchEventListener(btnAvar, [=]() {
		showPopupUserInfo(Utils::getSingleton().userDataMe);
	});
	mLayer->addChild(btnAvar, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(btnAvar);

	spOnlineAvatar = Sprite::create();
	Sprite* stencil = Sprite::createWithSpriteFrameName("avatar_default.png");
	stencil->setScale(.9f);
	ClippingNode* clipper = ClippingNode::create();
	clipper->setStencil(stencil);
	clipper->setPosition(btnAvar->getPosition());
	clipper->addChild(spOnlineAvatar);
	clipper->setAlphaThreshold(.5f);
	mLayer->addChild(clipper, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(clipper);

	bgNoted = Sprite::createWithSpriteFrameName("circle_red.png");
	bgNoted->setPosition(btnAvar->getPositionX() + 27, btnAvar->getPositionY() + 30);
	bgNoted->setScale(.5);
	mLayer->addChild(bgNoted, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(bgNoted);

	Label* lbNoted = Label::createWithTTF("!", "fonts/arialbd.ttf", 35);
	lbNoted->setColor(Color3B::WHITE);
	lbNoted->setPosition(bgNoted->getContentSize().width / 2 - 2, bgNoted->getContentSize().height / 2);
	bgNoted->addChild(lbNoted, 1);

	lbName = Label::createWithTTF("Name", "fonts/arialbd.ttf", 23);
	lbName->setAnchorPoint(Vec2(0, .5f));
	lbName->setPosition(vecPos[10]);
	//lbName->setWidth(150);
	//lbName->setHeight(25);
	mLayer->addChild(lbName, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbName);

	lbId = Label::createWithTTF("ID: ", "fonts/arialbd.ttf", 23);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setPosition(vecPos[11]);
	mLayer->addChild(lbId, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbId);

    lbLevel = Label::createWithTTF(Utils::getSingleton().getStringForKey("level") + ": ", "fonts/arialbd.ttf", 23);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setPosition(vecPos[12]);
	mLayer->addChild(lbLevel, constant::MAIN_ZORDER_HEADER);
	autoScaleNode(lbLevel);

	initCofferView(vecPos[13], constant::MAIN_ZORDER_HEADER);

	ui::Scale9Sprite* spHeader = ui::Scale9Sprite::createWithSpriteFrameName("line.png");
	spHeader->setContentSize(Size(winSize.width / scaleScene.y, 6));
	spHeader->setAnchorPoint(Vec2(0, .5f));
	spHeader->setPosition(0, 595);
	mLayer->addChild(spHeader);

	/*int x = 0;
	while (x < 1120) {
		Sprite* line = Sprite::create("line.png");
		line->setPosition(x, vecPos[13].y);
		mLayer->addChild(line, constant::MAIN_ZORDER_HEADER);
		x += line->getContentSize().width;
	}*/

	if (Utils::getSingleton().userDataMe.UserID > 0) {
		onUserDataMeResponse();
	}

	if (!ispmE) {
		chosenBg->setPosition(isRealMoney && ispmE ? -120 : 120, 0);
		moneyNode->setPosition(vecPos[1] + Vec2(-60, 0));
		moneyBg1->setVisible(false);
		iconGold->setVisible(false);
		lbGold->setVisible(false);
		lbCoffer->setVisible(false);
	}
	//btnIAP->setVisible(!ispmE);
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
	btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
	btn0->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
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

	Label* lb = Label::createWithTTF(msg, "fonts/arial.ttf", 25);
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
	Node* popup = Node::create();
	popup->setPosition(560, 350);
	popup->setVisible(false);
	mLayer->addChild(popup, constant::ZORDER_POPUP);
	//autoScaleNode(popup);

	Sprite* bg = isBig ? Sprite::create("imgs/popup_bg1.png") : Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setName("spbg");
	popup->addChild(bg);

	if (!isBig) {
		ui::Scale9Sprite* spLine = ui::Scale9Sprite::createWithSpriteFrameName("popup_line.png");
		spLine->setContentSize(Size(bg->getContentSize().width - 250, 1));
		spLine->setPosition(0, bg->getContentSize().height / 2 - 90);
		spLine->setColor(Color3B::BLACK);
		popup->addChild(spLine);
	}

	Label* title = Label::createWithTTF(stitle, "fonts/azuki.ttf", 40);
	title->setPosition(0, bg->getContentSize().height / 2 - (isBig ? 120 : 65));
	title->setColor(Color3B::BLACK);
	title->setName("lbtitle");
	popup->addChild(title);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(0, -bg->getContentSize().height / 2 + (isBig ? 55 : 10)));
	btnClose->setName("btnclose");
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popup);
	});
	popup->addChild(btnClose);

	return popup;
}

cocos2d::Node * BaseScene::createPopupChooseProvider(std::string stitle, std::vector<std::string> providers, std::function<void(std::string chosenProvider)> funcCallback)
{
	Node* popup = Node::create();
	popup->setPosition(winSize.width / 2, winSize.height / 2);
	popup->setName("");
	popup->setVisible(false);
	mLayer->addChild(popup, constant::ZORDER_POPUP);

	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	popup->addChild(bg);
	Size bgsize = bg->getContentSize();

	ui::Scale9Sprite* spLine = ui::Scale9Sprite::createWithSpriteFrameName("popup_line.png");
	spLine->setContentSize(Size(bg->getContentSize().width - 250, 1));
	spLine->setPosition(0, bg->getContentSize().height / 2 - 90);
	spLine->setColor(Color3B::BLACK);
	popup->addChild(spLine);

	Label* title = Label::createWithTTF(stitle, "fonts/azuki.ttf", 40);
	title->setColor(Color3B::BLACK);
	title->setPosition(0, bg->getContentSize().height / 2 - 65);
	popup->addChild(title);

	Label* lbnote = Label::createWithTTF(Utils::getSingleton().getStringForKey("vui_long_chon_nha_cung_cap"), "fonts/arial.ttf", 25);
	lbnote->setPosition(0, -bg->getContentSize().height / 2 + 80);
	lbnote->setColor(Color3B::BLACK);
	popup->addChild(lbnote);

	ui::Button* btnok = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnok->setPosition(Vec2(0, -bg->getContentSize().height / 2 + 10));
	btnok->setName("btnsubmit");
	addTouchEventListener(btnok, [=]() {
		if (popup->getName().length() == 0) return;
		hidePopup(popup);
		funcCallback(popup->getName());
	});
	popup->addChild(btnok);

	Node* nodeProvider = Node::create();
	nodeProvider->setName("nodeprovider");
	popup->addChild(nodeProvider);

	int numPerRow = 3;
	int dx = 170;
	int dy = 150;
	int xp = -(numPerRow - 1) * dx / 2;
	int yp = (providers.size() - 1) / numPerRow * dy / 2;
	for (int i = 1; i <= providers.size(); i++) {
		string stri = to_string(i);
		string strimg = providers[i - 1] + ".png";
		ui::Button* btnProvider = ui::Button::create(strimg, strimg, "", ui::Widget::TextureResType::PLIST);
		btnProvider->setPosition(Vec2(xp + ((i - 1) % numPerRow) * dx, yp - ((i - 1) / numPerRow) * dy));
		btnProvider->setTag(0);
		btnProvider->setName("btn" + stri);
		btnProvider->setContentSize(Size(158, 99));
		btnProvider->setColor(Color3B::GRAY);
		addTouchEventListener(btnProvider, [=]() {
			if (btnProvider->getTag() == 1) return;
			for (int j = 1; j <= providers.size(); j++) {
				string strj = to_string(j);
				ui::Button* btn = (ui::Button*)nodeProvider->getChildByName("btn" + strj);
				if (btn != btnProvider) {
					btn->setTag(0);
					btn->setColor(Color3B::GRAY);
				} else {
					btn->setTag(1);
					btn->setColor(Color3B::WHITE);
					popup->setName(providers[j - 1]);
				}
			}
		});
		nodeProvider->addChild(btnProvider);

		Label* lbName = Label::createWithTTF(providers[i - 1], "fonts/arialbd.ttf", 30);
		lbName->setPosition(btnProvider->getContentSize().width / 2, btnProvider->getContentSize().height / 2);
		lbName->setColor(Color3B::BLACK);
		btnProvider->addChild(lbName, -1);

		/*ui::Scale9Sprite* bgProvider = ui::Scale9Sprite::createWithSpriteFrameName("box8.png");
		bgProvider->setContentSize(btnProvider->getContentSize() + Size(40, 40));
		bgProvider->setPosition(btnProvider->getContentSize().width / 2, btnProvider->getContentSize().height / 2);
		bgProvider->setTag(1);
		btnProvider->addChild(bgProvider, -1);*/
	}

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

	ui::Scale9Sprite* spLine = ui::Scale9Sprite::createWithSpriteFrameName("popup_line.png");
	spLine->setContentSize(Size(bg->getContentSize().width - 250, 1));
	spLine->setPosition(0, bg->getContentSize().height / 2 - 90);
	spLine->setColor(Color3B::BLACK);
	popupNotice->addChild(spLine);

	Label* title = Label::createWithTTF(Utils::getSingleton().getStringForKey("he_thong"), "fonts/azuki.ttf", 40);
	title->setColor(Color3B::BLACK);
	title->setPosition(0, bg->getContentSize().height / 2 - 65);
	popupNotice->addChild(title);

	Label* lb = Label::createWithTTF("", "fonts/arial.ttf", 25);
	lb->setColor(Color3B::BLACK);
	lb->setWidth(450);
	lb->setName("lbcontent");
	lb->setAlignment(TextHAlignment::CENTER);
	popupNotice->addChild(lb);

	ui::Button* btnok = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnok->setPosition(Vec2(-110, -bg->getContentSize().height / 2 + 10));
	btnok->setName("btnsubmit");
	addTouchEventListener(btnok, [=]() {});
	popupNotice->addChild(btnok);

	ui::Button* btndong = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btndong->setPosition(Vec2(110, -bg->getContentSize().height / 2 + 10));
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
	bool pmE = Utils::getSingleton().ispmE();
	popupRank = createPopup(Utils::getSingleton().getStringForKey("bang_xep_hang"), true, true);
	if (!popupRank) return;
	popupRank->setTag(pmE ? 0 : 1);

	Size size = Size(920, 406);
	Size scrollSize = size - Size(20, 20);

	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgScroll->setContentSize(size);
	bgScroll->setPosition(0, -45);
	popupRank->addChild(bgScroll);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-scrollSize.width / 2 + 300, -scrollSize.height / 2) + bgScroll->getPosition());
	scroll->setContentSize(Size(scrollSize.width - 300, scrollSize.height));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupRank->addChild(scroll);

	ui::ScrollView* scrollWin = ui::ScrollView::create();
	scrollWin->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollWin->setBounceEnabled(true);
	scrollWin->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + bgScroll->getPosition());
	scrollWin->setContentSize(scrollSize);
	scrollWin->setScrollBarEnabled(false);
	scrollWin->setName("scrollwin");
	popupRank->addChild(scrollWin);

	vector<string> texts = { "quan", "xu", "win" };
	int x = -370;
	int y = bgScroll->getPositionY() + size.height/2 - 4;
	Size btnSize = Size(135, 60);
	for (int i = pmE ? 0 : 1; i < texts.size(); i++) {
		string strBtn = i == 0 ? "box4.png" : "box2.png";
		ui::Button* btn = ui::Button::create(strBtn, strBtn, strBtn, ui::Widget::TextureResType::PLIST);
		btn->setAnchorPoint(Vec2(.5f, 0));
		btn->setContentSize(btnSize);
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			showPopupRank(btn->getTag());
		});
		popupRank->addChild(btn);

		Label* lb = Label::createWithTTF(Utils::getSingleton().getStringForKey(texts[i]), "fonts/azuki.ttf", 30);
		lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
		lb->setAnchorPoint(Vec2(i < 2 ? 0 : .5f, .5f));
		lb->setColor(Color3B::BLACK);
		btn->addChild(lb);

		if (i < 2) {
			Sprite* spMoney = Sprite::createWithSpriteFrameName(i == 0 ? (pmE ? "icon_gold.png" : "icon_silver.png") : "icon_silver.png");
			spMoney->setScale(.6f);
			btn->addChild(spMoney);

			spMoney->setPosition(btn->getContentSize().width / 2 - lb->getContentSize().width / 2, btn->getContentSize().height / 2);
			lb->setPosition(spMoney->getPositionX() + spMoney->getContentSize().width / 2 * spMoney->getScale(), spMoney->getPositionY() + 2);
		} else {
			lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
		}

		x += btnSize.width;
	}

	Node* nodeTop = Node::create();
	nodeTop->setPosition(-310, bgScroll->getPositionY());
	nodeTop->setName("nodetop");
	popupRank->addChild(nodeTop);

	Size topSize = Size(255, 110);
	int dy = topSize.height + 20;
	for (int i = 0; i < 3; i++) {
		Node* topItem = Node::create();
		topItem->setPosition(0, dy - dy * i);
		topItem->setTag(i);
		nodeTop->addChild(topItem);

		ui::Scale9Sprite* topItemBg = ui::Scale9Sprite::createWithSpriteFrameName("box7.png");
		topItemBg->setContentSize(topSize);
		topItem->addChild(topItemBg);

		ui::Scale9Sprite* topItemLine = ui::Scale9Sprite::createWithSpriteFrameName("popup_line.png");
		topItemLine->setContentSize(Size(topSize.width - 55, 1));
		topItemLine->setColor(Color3B(90, 50, 20));
		topItemLine->setOpacity(225);
		topItemLine->setPositionY(-5);
		topItem->addChild(topItemLine);

		Sprite* topItemIcon = Sprite::createWithSpriteFrameName("top" + to_string(i + 1) + ".png");
		topItemIcon->setPosition(-topSize.width / 2 + 40, 25);
		topItem->addChild(topItemIcon);

		Label* lbName = Label::createWithTTF("Stormus", "fonts/arial.ttf", 25);
		lbName->setPosition(-topSize.width / 2 + 75, topSize.height / 4 - 2);
		lbName->setAnchorPoint(Vec2(0, .5f));
		lbName->setName("lbname");
		topItem->addChild(lbName);

		Label* lbMoney = Label::createWithTTF("100.000.000", "fonts/arial.ttf", 25);
		lbMoney->setPosition(0, -topSize.height / 4);
		lbMoney->setName("lbmoney");
		topItem->addChild(lbMoney);
	}
}

void BaseScene::initPopupSettings()
{
	popupMainSettings = createPopup(Utils::getSingleton().getStringForKey("cai_dat"), false, false);

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

		Label* lb = Label::createWithTTF("", "fonts/arial.ttf", 30);
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

	Sprite* bg = (Sprite*)popupMainSettings->getChildByName("spbg");

	ui::Button* btnClose = (ui::Button*)popupMainSettings->getChildByName("btnclose");
	btnClose->setPositionX(110);

	ui::Button* btnOK = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnOK->setPosition(Vec2(-110, -bg->getContentSize().height / 2 + 10));
	addTouchEventListener(btnOK, [=]() {
		hidePopup(popupMainSettings);
		Utils::getSingleton().SoundEnabled = cbs[0]->isSelected();
		Utils::getSingleton().IgnoreInvitation = cbs[1]->isSelected();
		UserDefault::getInstance()->setBoolForKey(constant::KEY_SOUND.c_str(), cbs[0]->isSelected());
		UserDefault::getInstance()->setBoolForKey(constant::KEY_INVITATION.c_str(), cbs[1]->isSelected());
	});
	popupMainSettings->addChild(btnOK);
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
	bg->setInsetLeft(52);
	bg->setInsetRight(52);
	bg->setInsetTop(48);
	bg->setInsetBottom(48);
	popupUserInfo->addChild(bg);

	ui::Scale9Sprite* spLine = ui::Scale9Sprite::createWithSpriteFrameName("popup_line.png");
	spLine->setContentSize(Size(bg->getContentSize().width - 250, 1));
	spLine->setPosition(0, bg->getContentSize().height / 2 - 90);
	spLine->setColor(Color3B::BLACK);
	popupUserInfo->addChild(spLine);

	Label* title = Label::createWithTTF(Utils::getSingleton().getStringForKey("thong_tin_nguoi_choi"), "fonts/azuki.ttf", 40);
	title->setPosition(0, bg->getContentSize().height / 2 - 65);
	title->setColor(Color3B::BLACK);
	title->setName("lbtitle");
	popupUserInfo->addChild(title);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(0, -bg->getContentSize().height / 2 + 10));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupUserInfo);
	});
	popupUserInfo->addChild(btnClose);

	Sprite* avatar = Sprite::createWithSpriteFrameName("avatar_default.png");
	avatar->setPosition(-255, 80);
	avatar->setScale(1.7f);
	avatar->setName("avatar");
	popupUserInfo->addChild(avatar);

	Sprite* spOlAvar = Sprite::create();
	spOlAvar->setPosition(avatar->getPosition());
	spOlAvar->setName("olavar");
	popupUserInfo->addChild(spOlAvar);

	Label* lbAppellation = Label::createWithTTF("Huong Truong", "fonts/azuki.ttf", 35);
	lbAppellation->setColor(Color3B::WHITE);
	lbAppellation->setPosition(avatar->getPositionX(), avatar->getPositionY() - 113);
	lbAppellation->setName("lbappellation");
	popupUserInfo->addChild(lbAppellation);

	ui::Button* btnHistory = ui::Button::create("btn_small.png", "btn_small_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnHistory->setTitleText(Utils::getSingleton().getStringForKey("lich_su"));
	btnHistory->setTitleFontName("fonts/azuki.ttf");
	btnHistory->setTitleColor(Color3B::BLACK);
	btnHistory->setTitleFontSize(35);
	btnHistory->setContentSize(Size(206, 55));
	btnHistory->setScale9Enabled(true);
	btnHistory->setPosition(Vec2(lbAppellation->getPositionX(), lbAppellation->getPositionY() - 55));
	btnHistory->setName("btnhistory");
	btnHistory->setScale(.8f);
	addTouchEventListener(btnHistory, [=]() {
		showPopupHistory();
	});
	popupUserInfo->addChild(btnHistory);

	ui::Button* btnActive = ui::Button::create("btn_small.png", "btn_small_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnActive->setTitleText(Utils::getSingleton().getStringForKey("kich_hoat"));
	btnActive->setTitleFontName("fonts/azuki.ttf");
	btnActive->setTitleColor(Color3B::BLACK);
	btnActive->setTitleFontSize(35);
	btnActive->setContentSize(Size(206, 55));
	btnActive->setScale9Enabled(true);
	btnActive->setPosition(Vec2(btnHistory->getPositionX(), btnHistory->getPositionY() - 50));
	btnActive->setName("btnactive");
	btnActive->setScale(.8f);
	addTouchEventListener(btnActive, [=]() {
		if (popupChooseSmsKH == NULL) {
			std::vector<std::string> smsProviders = { "viettel", "mobifone", "vinaphone" };
			popupChooseSmsKH = createPopupChooseProvider(Utils::getSingleton().getStringForKey("chon_mang_sms"), smsProviders, [=](string provider) {
				string str = "";
				if (provider.compare("viettel") == 0) {
					str = Utils::getSingleton().gameConfig.smsKHVT;
				} else if (provider.compare("mobifone") == 0) {
					str = Utils::getSingleton().gameConfig.smsKHVMS;
				} else {
					str = Utils::getSingleton().gameConfig.smsKHVNP;
				}
				int index = str.find_last_of(' ');
				string number = str.substr(index + 1, str.length() - index);
				string content = str.substr(0, index);
				content = Utils::getSingleton().replaceString(content, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
				Utils::getSingleton().openSMS(number, content);
			});
		} else {
			resetPopupChooseProvider(popupChooseSmsKH);
		}
		showPopup(popupChooseSmsKH);
	});
	popupUserInfo->addChild(btnActive);

    ui::Button* btnLogoutFb = ui::Button::create("btn_small.png", "btn_small_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnLogoutFb->setTitleText("Logout FB");
	btnLogoutFb->setTitleFontName("fonts/azuki.ttf");
	btnLogoutFb->setTitleColor(Color3B::BLACK);
	btnLogoutFb->setTitleFontSize(35);
	btnLogoutFb->setContentSize(Size(206, 55));
	btnLogoutFb->setScale9Enabled(true);
    btnLogoutFb->setPosition(Vec2(btnActive->getPositionX(), btnActive->getPositionY() - 50));
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
	Label* lbDName = Label::createWithTTF("Stormus", "fonts/arialbd.ttf", 30);
	lbDName->setAnchorPoint(Vec2(0, .5f));
	lbDName->setColor(Color3B::BLACK);
	lbDName->setPosition(-130, 150);
	lbDName->setName("lbname");
	popupUserInfo->addChild(lbDName);

	Node* nodeInfo = Node::create();
	nodeInfo->setName("nodeinfo");
	nodeInfo->setPosition(lbDName->getPositionX(), lbDName->getPositionY() - x);
	popupUserInfo->addChild(nodeInfo);

	Label* lbUname = Label::createWithTTF(Utils::getSingleton().getStringForKey("login") + ":", "fonts/arial.ttf", 25);
	lbUname->setAnchorPoint(Vec2(0, .5f));
	lbUname->setColor(Color3B::BLACK);
	lbUname->setPosition(0, 0);
	lbUname->setName("lbuname");
	nodeInfo->addChild(lbUname);

	Label* lbUname1 = Label::createWithTTF("stormus", "fonts/arial.ttf", 25);
	lbUname1->setAnchorPoint(Vec2(0, .5f));
	lbUname1->setColor(Color3B::BLACK);
	lbUname1->setPosition(lbUname->getPositionX() + 150, lbUname->getPositionY());
	lbUname1->setName("lbuname1");
	nodeInfo->addChild(lbUname1);

	Label* lbId = Label::createWithTTF("ID:", "fonts/arial.ttf", 25);
	lbId->setAnchorPoint(Vec2(0, .5f));
	lbId->setColor(Color3B::BLACK);
	lbId->setPosition(lbUname->getPositionX(), lbUname->getPositionY() - x);
	lbId->setName("lbid");
	nodeInfo->addChild(lbId);

	Label* lbId1 = Label::createWithTTF("847558", "fonts/arial.ttf", 25);
	lbId1->setAnchorPoint(Vec2(0, .5f));
	lbId1->setColor(Color3B::BLACK);
	lbId1->setPosition(lbId->getPositionX() + 150, lbId->getPositionY());
	lbId1->setName("lbid1");
	nodeInfo->addChild(lbId1);

	Label* lbQuan = Label::createWithTTF(Utils::getSingleton().getStringForKey("quan") + ":", "fonts/arial.ttf", 25);
	lbQuan->setAnchorPoint(Vec2(0, .5f));
	lbQuan->setColor(Color3B::BLACK);
	lbQuan->setPosition(lbId->getPositionX(), lbId->getPositionY() - x);
	lbQuan->setVisible(ispmE);
	nodeInfo->addChild(lbQuan);

	Label* lbQuan1 = Label::createWithTTF("100,000", "fonts/arialbd.ttf", 25);
	lbQuan1->setAnchorPoint(Vec2(0, .5f));
	lbQuan1->setColor(Color3B::YELLOW);
	lbQuan1->enableOutline(Color4B(150, 150, 0, 255), 1);
	lbQuan1->setPosition(lbQuan->getPositionX() + 150, lbQuan->getPositionY());
	lbQuan1->setName("lbquan");
	lbQuan1->setVisible(ispmE);
	nodeInfo->addChild(lbQuan1);

	Label* lbXu = Label::createWithTTF(Utils::getSingleton().getStringForKey("xu") + ":", "fonts/arial.ttf", 25);
	lbXu->setAnchorPoint(Vec2(0, .5f));
	lbXu->setColor(Color3B::BLACK);
	lbXu->setPosition(lbQuan->getPositionX(), lbQuan->getPositionY() - x);
	nodeInfo->addChild(lbXu);

	Label* lbXu1 = Label::createWithTTF("100,000", "fonts/arialbd.ttf", 25);
	lbXu1->setAnchorPoint(Vec2(0, .5f));
	lbXu1->setColor(Color3B(0, 255, 255));
	lbXu1->enableOutline(Color4B(0, 150, 150, 255), 1);
	lbXu1->setPosition(lbXu->getPositionX() + 150, lbXu->getPositionY());
	lbXu1->setName("lbxu");
	nodeInfo->addChild(lbXu1);
	
	/*Label* lbLevel = Label::createWithTTF(Utils::getSingleton().getStringForKey("cap_do") + ":", "fonts/arial.ttf", 25);
	lbLevel->setAnchorPoint(Vec2(0, .5f));
	lbLevel->setColor(Color3B::WHITE);
	lbLevel->setPosition(-70, -70);
	lbLevel->setName("lblevel");
	popupUserInfo->addChild(lbLevel);*/

	Label* lbWin = Label::createWithTTF(Utils::getSingleton().getStringForKey("thang") + ": 12", "fonts/arial.ttf", 25);
	lbWin->setAnchorPoint(Vec2(0, .5f));
	lbWin->setColor(Color3B::BLACK);
	lbWin->setPosition(lbXu->getPositionX(), lbXu->getPositionY() - x);
	lbWin->setName("lbwin");
	nodeInfo->addChild(lbWin);

	Label* lbTotal = Label::createWithTTF(Utils::getSingleton().getStringForKey("tong") + ": 20", "fonts/arial.ttf", 25);
	lbTotal->setAnchorPoint(Vec2(0, .5f));
	lbTotal->setColor(Color3B::BLACK);
	lbTotal->setPosition(lbWin->getPositionX() + 250, lbWin->getPositionY());
	lbTotal->setName("lbtotal");
	nodeInfo->addChild(lbTotal);

	Label* lbBigWin = Label::createWithTTF(Utils::getSingleton().getStringForKey("thang_lon_nhat") + ":", "fonts/arial.ttf", 25);
	lbBigWin->setPosition(lbWin->getPositionX(), lbWin->getPositionY() - x);
	lbBigWin->setAnchorPoint(Vec2(0, .5f));
	lbBigWin->setColor(Color3B::BLACK);
	lbBigWin->setVisible(false);
	nodeInfo->addChild(lbBigWin);

	Label* lbBigWin1 = Label::createWithTTF("1111", "fonts/arial.ttf", 25);
	lbBigWin1->setPosition(lbBigWin->getPositionX() + lbBigWin->getContentSize().width + 10, lbBigWin->getPositionY());
	lbBigWin1->setAnchorPoint(Vec2(0, .5f));
	lbBigWin1->setColor(Color3B::BLACK);
	lbBigWin1->setName("lbbigwin");
	lbBigWin1->setVisible(false);
	nodeInfo->addChild(lbBigWin1);

	Label* lbBigCrest = Label::createWithTTF(Utils::getSingleton().getStringForKey("u_to_nhat") + ": Nha lau xe hoi - Hoa roi cua pha t, a a a Ngu ong bat ca, Ca loi san dinh", "fonts/arial.ttf", 25);
	lbBigCrest->setPosition(lbBigWin->getPositionX(), lbBigWin->getPositionY() - x + 20);
	lbBigCrest->setAnchorPoint(Vec2(0, 1));
	lbBigCrest->setColor(Color3B::BLACK);
	lbBigCrest->setWidth(480);
	lbBigCrest->setName("lbbigcrest");
	lbBigCrest->setVisible(false);
	nodeInfo->addChild(lbBigCrest);

	//Label* lbBigCrest1 = Label::createWithTTF("Nha lau xe hoi - Hoa roi cua pha t, a a a Ngu ong bat ca, Ca loi san dinh", "fonts/arial.ttf", 25);
	//lbBigCrest1->setPosition(lbBigCrest->getPositionX() + lbBigCrest->getContentSize().width + 10, lbBigCrest->getPositionY() + 15);
	////lbBigCrest1->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
	//lbBigCrest1->setAnchorPoint(Vec2(0, 1));
	//lbBigCrest1->setColor(Color3B::BLACK);
	//lbBigCrest1->setWidth(380);
	//lbBigCrest1->setName("lbbigcrest");
	//popupUserInfo->addChild(lbBigCrest1);
}

void BaseScene::initPopupHistory()
{
	bool ispmE = Utils::getSingleton().ispmE();

	popupHistory = createPopup(Utils::getSingleton().getStringForKey("log_van_danh"), true, false);
	popupHistory->setTag(0);

	Size size = Size(920, 406);
	Size scrollSize = size - Size(20, 20);

	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgScroll->setContentSize(size);
	bgScroll->setPosition(0, ispmE ? -45 : -25);
	popupHistory->addChild(bgScroll);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2 + 35) + bgScroll->getPosition());
	scroll->setContentSize(Size(scrollSize.width, scrollSize.height - 60));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	popupHistory->addChild(scroll);

	vector<string> texts = { "quan", "xu" };
	int x = -370;
	int y = bgScroll->getPositionY() + size.height / 2 - 4;
	Size btnSize = Size(135, 60);
	for (int i = 0; i < texts.size(); i++) {
		string strBtn = i == 0 ? "box4.png" : "box2.png";
		ui::Button* btn = ui::Button::create(strBtn, strBtn, strBtn, ui::Widget::TextureResType::PLIST);
		btn->setAnchorPoint(Vec2(.5f, 0));
		btn->setContentSize(btnSize);
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		btn->setVisible(ispmE);
		btn->setTag(10 + i);
		addTouchEventListener(btn, [=]() {
			if (popupHistory->getTag() == i) return;
			ui::Button* btn1 = (ui::Button*)popupHistory->getChildByTag(10 + popupHistory->getTag());
			btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
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

		Label* lb = Label::createWithTTF(Utils::getSingleton().getStringForKey(texts[i]), "fonts/azuki.ttf", 30);
		lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
		lb->setAnchorPoint(Vec2(i < 2 ? 0 : .5f, .5f));
		lb->setColor(Color3B::BLACK);
		btn->addChild(lb);

		if (i < 2) {
			Sprite* spMoney = Sprite::createWithSpriteFrameName(i == 0 ? (ispmE ? "icon_gold.png" : "icon_silver.png") : "icon_silver.png");
			spMoney->setScale(.6f);
			btn->addChild(spMoney);

			spMoney->setPosition(btn->getContentSize().width / 2 - lb->getContentSize().width / 2, btn->getContentSize().height / 2);
			lb->setPosition(spMoney->getPositionX() + spMoney->getContentSize().width / 2 * spMoney->getScale(), spMoney->getPositionY() + 2);
		} else {
			lb->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
		}

		x += btnSize.width;
	}
	
	int py = bgScroll->getPositionY() + size.height / 2 - 20;
	int px = scroll->getPositionX() + 25;
	vector<int> widths = { 70, 120, 350, 150, 150, 60 };
	vector<string> historyTitles = { "STT", Utils::getSingleton().getStringForKey("ngay"), Utils::getSingleton().getStringForKey("thong_tin"),
		Utils::getSingleton().getStringForKey("tien_van"), Utils::getSingleton().getStringForKey("tien"), "ID" };
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lb = Label::createWithTTF(historyTitles[i], "fonts/arialbd.ttf", 22);
		lb->setColor(Color3B::BLACK);
		lb->setPosition(px, py);
		popupHistory->addChild(lb);

		if (i < historyTitles.size() - 1) {
			px += widths[i] / 2 + widths[i + 1] / 2;
		}
	}

	Node* nodeDetail = Node::create();
	nodeDetail->setPosition(bgScroll->getPosition() + Vec2(0, -15));
	nodeDetail->setVisible(false);
	nodeDetail->setName("nodedetail");
	popupHistory->addChild(nodeDetail);

	ui::Scale9Sprite* bgDetail = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	bgDetail->setContentSize(size - Size(10, 40));
	bgDetail->setColor(Color3B(155, 155, 155));
	nodeDetail->addChild(bgDetail);

	px = scroll->getPositionX() + 25;
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lbDetail = Label::createWithTTF("s", "fonts/arial.ttf", 20);
		lbDetail->setWidth(widths[i] - (i == 2 ? 30 : 0));
		lbDetail->setAnchorPoint(Vec2(.5f, 1));
		lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
        lbDetail->setPosition(px, bgDetail->getContentSize().height / 2 - 15);
		lbDetail->setTag(i);
		lbDetail->setColor(Color3B::BLACK);
		nodeDetail->addChild(lbDetail);

		if (i < historyTitles.size() - 1) {
			px += widths[i] / 2 + widths[i + 1] / 2;
		}
	}

	ui::Button* btnCloseDetail = ui::Button::create("btn_dong2.png", "btn_dong2.png", "", ui::Widget::TextureResType::PLIST);
	btnCloseDetail->setPosition(Vec2(bgDetail->getContentSize().width / 2 - 50, -bgDetail->getContentSize().height / 2 + 50));
	//btnCloseDetail->setScale(.7f);
	addTouchEventListener(btnCloseDetail, [=]() {
		nodeDetail->setVisible(false);
	});
	nodeDetail->addChild(btnCloseDetail);

	addBtnChoosePage(0, ispmE ? -222 : -202, popupHistory, [=](int page) {
		int type = popupHistory->getTag();
		SFSRequest::getSingleton().RequestPlayHistory(type, page - 1);
		//onPlayLogDataResponse(vector<PlayLogData>());
	});
}

void BaseScene::initPopupCoffer()
{
	popupCoffer = createPopup("", true, true);
	if (!popupCoffer) return;
	popupCoffer->setTag(0);

	Size size = Size(900, 436);
	Size scrollSize = size - Size(30, 20);

	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgScroll->setContentSize(size);
	bgScroll->setPosition(0, -20);
	popupCoffer->addChild(bgScroll);

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
	int x = -320;
	int y = 219;
	Size btnSize = Size(205, 50);
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box4.png" : "box2.png", "box4.png", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/arialbd.ttf");
		btn->setTitleFontSize(25);
		btn->setTitleColor(Color3B::BLACK);
		btn->setContentSize(btnSize);
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(10 + i);
		addTouchEventListener(btn, [=]() {
			if (popupCoffer->getTag() == i) return;
			popupCoffer->getChildByTag(i)->setVisible(true);
			popupCoffer->getChildByTag(1 - i)->setVisible(false);
			ui::Button* btn1 = (ui::Button*)popupCoffer->getChildByTag(10 + popupCoffer->getTag());
			btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
			popupCoffer->setTag(i);
		});
		popupCoffer->addChild(btn);
		x += btnSize.width;
	}

	//Node guide
	/*ui::Scale9Sprite* bgScrollGuide = ui::Scale9Sprite::createWithSpriteFrameName("box1.png");
	bgScrollGuide->setContentSize(size);
	bgScrollGuide->setPosition(95, -25);
	nodeGuide->addChild(bgScrollGuide);*/

	ui::ScrollView* scrollGuide = ui::ScrollView::create();
	scrollGuide->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollGuide->setBounceEnabled(true);
	scrollGuide->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + bgScroll->getPosition());
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
	Label* lb = Label::createWithTTF(guideContent, "fonts/arial.ttf", 20);
	lb->setAnchorPoint(Vec2(0, 1));
	lb->setColor(Color3B::BLACK);
	lb->setName("lbcontent");
	lb->setWidth(scrollSize.width);
	scrollGuide->addChild(lb);

	int height = lb->getContentSize().height;
	if (height < scrollGuide->getContentSize().height) {
		height = scrollGuide->getContentSize().height;
	}
	lb->setPosition(0, height - 10);
	scrollGuide->setInnerContainerSize(Size(scrollSize.width, height));

	//Node history
	ui::ScrollView* scrollHistory = ui::ScrollView::create();
	scrollHistory->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollHistory->setBounceEnabled(true);
	scrollHistory->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + bgScroll->getPosition());
	scrollHistory->setContentSize(scrollSize);
	scrollHistory->setScrollBarEnabled(false);
	scrollHistory->setName("scrollhistory");
	nodeHistory->addChild(scrollHistory);
}

void BaseScene::initPopupIAP()
{
	popupIAP = createPopup("", true, false);
	Size size = Size(900, 446);

	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgScroll->setContentSize(size);
	//bgScroll->setPosition(0, 0);
	popupIAP->addChild(bgScroll);

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

		ui::Button* btn = ui::Button::create("box7.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setPosition(Vec2(130 + i * 260, storeSize.height / 2));
		btn->setContentSize(Size(200, 180));
		btn->setScale9Enabled(true);
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
		spCoin->setScale(.55f);
		btn->addChild(spCoin);

		Label* lb1 = Label::createWithTTF(strValue, "fonts/arialbd.ttf", 30);
		lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, btn->getContentSize().height / 2 - 75);
		lb1->setColor(Color3B::YELLOW);
		btn->addChild(lb1);

		Label* lb2 = Label::createWithTTF(strCost, "fonts/arialbd.ttf", 30);
		lb2->setWidth(175);
		lb2->setHeight(30);
		lb2->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 - 110);
		lb2->setColor(Color3B::BLACK);
		lb2->setHorizontalAlignment(TextHAlignment::CENTER);
		btn->addChild(lb2);

		spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
			+ spCoin->getContentSize().width * spCoin->getScale() / 2 + 5, lb1->getPositionY());
	}
}

void BaseScene::initCofferView(Vec2 pos, int zorder, float scale)
{
	bool ispmE = Utils::getSingleton().ispmE();
    ui::Button* btnCoffer = ui::Button::create(ispmE ? "coffer.png" : "empty.png", ispmE ? "coffer.png" : "empty.png", "", ui::Widget::TextureResType::PLIST);
	btnCoffer->setPosition(pos);
	btnCoffer->setScale(scale);
	btnCoffer->setVisible(ispmE);
	addTouchEventListener(btnCoffer, [=]() {
		showPopupCoffer();
	});
	mLayer->addChild(btnCoffer, zorder);
	autoScaleNode(btnCoffer);

	lbCoffer = Label::createWithTTF(Utils::getSingleton().formatMoneyWithComma(Utils::getSingleton().cofferMoney), "fonts/arial.ttf", 18);
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
	UserData dataMe = Utils::getSingleton().userDataMe;
	std::string strGold = Utils::getSingleton().formatMoneyWithComma(dataMe.MoneyReal);
	std::string strSilver = Utils::getSingleton().formatMoneyWithComma(dataMe.MoneyFree);
	std::string strId = String::createWithFormat("ID: %ld", dataMe.UserID)->getCString();
	std::string strLevel = String::createWithFormat((Utils::getSingleton().getStringForKey("level") + ": %d").c_str(), dataMe.Level)->getCString();

	string strName = Utils::getSingleton().userDataMe.DisplayName;
	lbName->setString(strName);
	cropLabel(lbName, 120);

	bgNoted->setVisible(!dataMe.IsActived);
	lbGold->setString(strGold);
	lbSilver->setString(strSilver);
	lbId->setString(strId);
	lbLevel->setString(strLevel);
	if (dataMe.AvatarUrl.length() > 0) {
		Utils::getSingleton().LoadTextureFromURL(dataMe.AvatarUrl, [=](Texture2D* texture) {
			spOnlineAvatar->initWithTexture(texture);
			Size fsize = btnAvar->getContentSize();
			Size spsize = spOnlineAvatar->getContentSize();
			float scaleX = fsize.width / spsize.width;
			float scaleY = fsize.height / spsize.height;
			Vec2 scale = getScaleSmoothly(scaleX < scaleY ? scaleY : scaleX);
			spOnlineAvatar->setScale(scale.x, scale.y);
		});
	}

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

	ui::ScrollView* scroll = (ui::ScrollView*)popupHistory->getChildByName("scroll");
	int height = logs.size() * 60;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
	int px = scroll->getPositionX() + 25;
	vector<int> widths = { 70, 120, 350, 150, 150, 60 };
	vector<int> posX = {};
	for (int i = 0; i < 5; i++) {
		posX.push_back(px);
		px += widths[i] / 2 + widths[i + 1] / 2;
	}
	posX.push_back(px);
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
				Label* lbDetail = Label::createWithTTF("", "fonts/arial.ttf", 20);
				lbDetail->setWidth(widths[j] - (j == 2 ? 30 : 0));
				lbDetail->setHeight(46);
				lbDetail->setAnchorPoint(Vec2(.5f, 1));
				lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
				lbDetail->setPosition(posX[j] + scroll->getContentSize().width / 2, height - 5 - i * 60);
				lbDetail->setTag(i * 7 + j);
				lbDetail->setColor(Color3B::BLACK);
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
		data.Name = rand() % 2 == 0 ? "STORMUSadfsdfsdfgsfdgQWERTYUIO" : ("stormsdfgsdfgsdfgusssss" + to_string(data.Uid));
		data.Point = rand() % 60;
		data.Cuocs = rand() % 2 == 0 ? "Nha lau xe hoi Hoa roi cua phat Ca ca nhay dau thuyen ngu ong bat ca" : "Nha lau xe hoi Hoa roi cua phat";
		data.Date = "05/01/2016 19::00:00";
		data.Money = 10000L * (rand() % 10000);
		list.push_back(data);
	}*/
	
	Node* nodeHistory = popupCoffer->getChildByName("nodehistory");
	ui::ScrollView* scroll = (ui::ScrollView*)nodeHistory->getChildByName("scrollhistory");
	int rowHeight = 70;
	int height = list.size() * rowHeight;
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
			node->setTag(i);
			scroll->addChild(node);

			Label* lb1 = Label::createWithTTF(list[i].Date, "fonts/arial.ttf", 22);
			lb1->setAnchorPoint(Vec2(0, .5f));
			lb1->setColor(Color3B::BLACK);
			lb1->setPosition(-width / 2 + 20, 0);
			lb1->setWidth(100);
			lb1->setTag(1);
			lb1->setVisible(false);
			node->addChild(lb1);

			lb2 = Label::createWithTTF(list[i].Name, "fonts/arial.ttf", 22);
			lb2->setColor(Color3B::BLACK);
			lb2->setPosition(-width / 2 + 95, 0);
			lb2->setTag(2);
			node->addChild(lb2);

			Label* lb3 = Label::createWithTTF(list[i].Cuocs, "fonts/arial.ttf", 22);
			lb3->setHorizontalAlignment(TextHAlignment::CENTER);
			lb3->setColor(Color3B::BLACK);
			lb3->setPosition(-10, 0);
			lb3->setWidth(380);
			lb3->setTag(3);
			node->addChild(lb3);

			Label* lb4 = Label::createWithTTF(Utils::getSingleton().formatMoneyWithComma(list[i].Point), "fonts/arial.ttf", 22);
			lb4->setColor(Color3B::BLACK);
			lb4->setPosition(width / 2 - 30, 0);
			lb4->setTag(4);
			node->addChild(lb4);

			Label* lb5 = Label::createWithTTF(Utils::getSingleton().formatMoneyWithComma(list[i].Money), "fonts/arial.ttf", 22);
			lb5->setColor(Color3B::BLACK);
			lb5->setPosition(width / 2 - 130, 0);
			lb5->setTag(5);
			node->addChild(lb5);
		} else {
			node->setVisible(true);
			Label* lb1 = (Label*)node->getChildByTag(1);
			lb2 = (Label*)node->getChildByTag(2);
			Label* lb3 = (Label*)node->getChildByTag(3);
			Label* lb4 = (Label*)node->getChildByTag(4);
			lb1->setString(list[i].Date);
			lb2->setString(list[i].Name);
			lb3->setString(list[i].Cuocs);
			lb4->setString(Utils::getSingleton().formatMoneyWithComma(list[i].Point));
		}
		node->setPosition(scroll->getContentSize().width / 2, height - 35 - i * rowHeight);
		cropLabel(lb2, 170);
	}
	int i = list.size();
	Node* n;
	while ((n = scroll->getChildByTag(i++)) != nullptr) {
		n->setVisible(false);
	}
}

void BaseScene::onDownloadedPlistTexture(int numb)
{
	
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
			lb->setPosition(0, height - 10);
			scrollGuide->setInnerContainerSize(Size(scrollGuide->getContentSize().width, height));
		}
	}
}

void BaseScene::onHttpResponseFailed()
{
	CCLOG("falied");
}

void BaseScene::addBtnChoosePage(int x, int y, cocos2d::Node * node, std::function<void(int)> funcPage)
{
	Node* nodePage = Node::create();
	nodePage->setTag(1);
	nodePage->setName("nodepage");
	node->addChild(nodePage);

	int btnFontSize = 18;
	int btnHeight = 30;
	int dx = 45;
	int x1 = x - dx * 2;
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = ui::Button::create("box1.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleFontName("fonts/arial.ttf");
		btn->setTitleFontSize(btnFontSize);
		btn->setTitleText(to_string(i));
		btn->setTitleColor(Color3B::BLACK);
		btn->setTitleDeviation(Vec2(-1, -1));
		btn->setContentSize(Size(btnHeight, btnHeight));
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
		x1 += dx;
	}

	ui::Button* btnPre = ui::Button::create("box1.png", "", "", ui::Widget::TextureResType::PLIST);
	btnPre->setTitleFontName("fonts/arial.ttf");
	btnPre->setTitleFontSize(btnFontSize);
	btnPre->setTitleText("<<");
	btnPre->setTitleColor(Color3B::BLACK);
	btnPre->setTitleDeviation(Vec2(-1, -1));
	btnPre->setContentSize(Size(btnHeight * 1.5f, btnHeight));
	btnPre->setPosition(Vec2(x - (dx * 3 + 10), y));
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

	ui::Button* btnNext = ui::Button::create("box1.png", "", "", ui::Widget::TextureResType::PLIST);
	btnNext->setTitleFontName("fonts/arial.ttf");
	btnNext->setTitleFontSize(btnFontSize);
	btnNext->setTitleText(">>");
	btnNext->setTitleColor(Color3B::BLACK);
	btnNext->setTitleDeviation(Vec2(-1, -1));
	btnNext->setContentSize(Size(btnHeight * 1.5f, btnHeight));
	btnNext->setPosition(Vec2(x + dx * 3 + 10, y));
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

void BaseScene::delayFunction(Node * node, float time, std::function<void()> func)
{
    DelayTime* delay = DelayTime::create(time);
    CallFunc* callfunc = CallFunc::create(func);
    node->runAction(Sequence::create(delay, callfunc, nullptr));
}

void BaseScene::cropLabel(cocos2d::Label * label, int width)
{
	bool isCut = false;
	string str = label->getString();
	while (label->getContentSize().width > width) {
		str = str.substr(0, str.length() - 1);
		label->setString(str);
		isCut = true;
	}
	if (isCut) {
		str += "..";
		label->setString(str);
	}
}

void BaseScene::resetPopupChooseProvider(Node * popup)
{
	popup->setName("");
	Node* nodeProvider = popup->getChildByName("nodeprovider");
	for (int i = 1; i < 10; i++) {
		Node* btn = nodeProvider->getChildByName("btn" + to_string(i));
		if (btn == nullptr) return;
		btn->setTag(0);
		btn->setColor(Color3B::GRAY);
	}
}
