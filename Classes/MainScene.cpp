#pragma once
#include "MainScene.h"
#include "SFSRequest.h"
#include "Constant.h"
#include "Utils.h"
#include "EventHandler.h"
#include "Tracker.h"
#include "GameLogger.h"

using namespace cocos2d;
using namespace std;

void MainScene::onInit()
{
	BaseScene::onInit();
	bool pmE = Utils::getSingleton().ispmE();
	isChargeQuan = Utils::getSingleton().ispmE();
	currentMoneyType = Utils::getSingleton().moneyType;
	chosenProviderCard = Utils::getSingleton().chosenProviderCard;
	chosenProviderSms = Utils::getSingleton().chosenProviderSms;

	std::vector<Vec2> vecPos;
	vecPos.push_back(Vec2(80, 490));
	vecPos.push_back(Vec2(80, 350));
	vecPos.push_back(Vec2(80, 210));
	vecPos.push_back(Vec2(80, 70));
	vecPos.push_back(Vec2(1040, 490));
	vecPos.push_back(Vec2(1040, 350));
	vecPos.push_back(Vec2(1040, 210));
	vecPos.push_back(Vec2(1040, 70));

	vecPos.push_back(Vec2(190, 230));
	vecPos.push_back(Vec2(940, 255));
	vecPos.push_back(Vec2(670, 400));
	vecPos.push_back(Vec2(590, 215));
	/*vecPos.push_back(Vec2(560 - 170 * scaleScene.y, 285 + 135 * scaleScene.x));
	vecPos.push_back(Vec2(560 + 170 * scaleScene.y, 285 + 135 * scaleScene.x));
	vecPos.push_back(Vec2(560 - 170 * scaleScene.y, 285 - 135 * scaleScene.x));
	vecPos.push_back(Vec2(560 + 170 * scaleScene.y, 285 - 135 * scaleScene.x));*/
	int m = 0;

	initHeaderWithInfos();

	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGB565);
	Texture2D* bgTexture = TextureCache::getInstance()->addImage("main_bg.jpg");
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);

	Sprite* bg = Sprite::createWithTexture(bgTexture);
	bg->setPosition(560, 350);
	addChild(bg);

	int mdx = winSize.width / 8;
	int mPosY = 20;
	int mPosX = -mdx / 2;
	bool canInvite = Utils::getSingleton().gameConfig.invite;
	ui::Button* btnFBFriends = ui::Button::create("fb_friends.png", "", "", ui::Widget::TextureResType::PLIST);
	btnFBFriends->setPosition(Vec2(mPosX += mdx, mPosY));
	btnFBFriends->setVisible(pmE && canInvite);
	btnFBFriends->setAnchorPoint(Vec2(.5f, 0));
	addTouchEventListener(btnFBFriends, [=]() {
		Utils::getSingleton().inviteFacebookFriends();
	});
	mLayer->addChild(btnFBFriends);
	autoScaleNode(btnFBFriends);

	ui::Button* btnGuide = ui::Button::create("icon_guide.png", "", "", ui::Widget::TextureResType::PLIST);
	btnGuide->setPosition(Vec2(mPosX += mdx, mPosY));
	btnGuide->setAnchorPoint(Vec2(.5f, 0));
	addTouchEventListener(btnGuide, [=]() {
		if (popupGuide == nullptr) {
			initPopupGuide();
		}
		showPopup(popupGuide);
	});
	mLayer->addChild(btnGuide);
	autoScaleNode(btnGuide);

	ui::Button* btnNews = ui::Button::create("icon_news.png", "", "", ui::Widget::TextureResType::PLIST);
	btnNews->setPosition(Vec2(mPosX += mdx, mPosY));
	btnNews->setAnchorPoint(Vec2(.5f, 0));
	addTouchEventListener(btnNews, [=]() {
		showPopupNews();
	});
	mLayer->addChild(btnNews);
	autoScaleNode(btnNews);

	ui::Button* btnCharge = ui::Button::create("icon_charge.png", "", "", ui::Widget::TextureResType::PLIST);
	btnCharge->setPosition(Vec2(mPosX += mdx, mPosY));
	btnCharge->setAnchorPoint(Vec2(.5f, 0));
	addTouchEventListener(btnCharge, [=]() {
		if (pmE) {
			chosenProviderCard = "viettel";
			if (popupCharge == nullptr) {
				initPopupCharge();
			}
			showPopup(popupCharge);
		} else {
			if (popupIAP == nullptr) {
				initPopupIAP();
			}
			showPopup(popupIAP);
		}
	});
	mLayer->addChild(btnCharge);
	autoScaleNode(btnCharge);

	ui::Button* btnShop = ui::Button::create("icon_shop.png", "", "", ui::Widget::TextureResType::PLIST);
	btnShop->setPosition(Vec2(mPosX += mdx, mPosY));
	btnShop->setAnchorPoint(Vec2(.5f, 0));
	btnShop->setVisible(pmE);
	addTouchEventListener(btnShop, [=]() {
		if (popupShop == nullptr) {
			initPopupShop();
		}
		showPopup(popupShop);
		if (popupShop->getChildByTag(10)->getChildByName("scrollcard")->getChildrenCount() == 0) {
			SFSRequest::getSingleton().RequestShopItems();
		}
	});
	mLayer->addChild(btnShop);
	autoScaleNode(btnShop);

	ui::Button* btnMail = ui::Button::create("icon_mail.png", "", "", ui::Widget::TextureResType::PLIST);
	btnMail->setPosition(Vec2(mPosX += mdx, mPosY));
	btnMail->setAnchorPoint(Vec2(.5f, 0));
	addTouchEventListener(btnMail, [=]() {
		showPopupMail();
	});
	mLayer->addChild(btnMail);
	autoScaleNode(btnMail);

	Sprite* circleNewMail = Sprite::createWithSpriteFrameName("circle_red.png");
	circleNewMail->setPosition(btnMail->getPosition() + Vec2(30, 80));
	circleNewMail->setScale(.7f);
	circleNewMail->setVisible(false);
	mLayer->addChild(circleNewMail);

	lbNewMail = Label::create("", "fonts/arial.ttf", 30);
	lbNewMail->setPosition(circleNewMail->getContentSize().width / 2 - 2, circleNewMail->getContentSize().height / 2);
	circleNewMail->addChild(lbNewMail);

	ui::Button* btnGiftcode = ui::Button::create("icon_giftcode.png", "", "", ui::Widget::TextureResType::PLIST);
	btnGiftcode->setPosition(Vec2(mPosX += mdx, mPosY));
	btnGiftcode->setAnchorPoint(Vec2(.5f, 0));
	btnGiftcode->setVisible(pmE);
	addTouchEventListener(btnGiftcode, [=]() {
		if (popupGiftcode == nullptr) {
			initPopupGiftcode();
		}
		showPopup(popupGiftcode);
		//SFSRequest::getSingleton().RequestCofferHistory();
	});
	mLayer->addChild(btnGiftcode);
	autoScaleNode(btnGiftcode);

	ui::Button* btnFacebook = ui::Button::create("facebook.png", "", "", ui::Widget::TextureResType::PLIST);
	btnFacebook->setPosition(Vec2(mPosX += mdx, mPosY));
	btnFacebook->setAnchorPoint(Vec2(.5f, 0));
	btnFacebook->setVisible(pmE);
	addTouchEventListener(btnFacebook, [=]() {
		Application::sharedApplication()->openURL(Utils::getSingleton().gameConfig.linkFb);
	});
	mLayer->addChild(btnFacebook);
	autoScaleNode(btnFacebook);

	btnEvent = ui::Button::create("icon_event.png", "", "", ui::Widget::TextureResType::PLIST);
	btnEvent->setPosition(Vec2(445, 360));
	btnEvent->setVisible(pmE);
	addTouchEventListener(btnEvent, [=]() {
		Utils::getSingleton().hasShowEventPopup = true;
		showWebView(Utils::getSingleton().dynamicConfig.PopupUrl);
	});
	mLayer->addChild(btnEvent);
	autoScaleNode(btnEvent);

	ui::Button* btnPhuChua = ui::Button::create("phuchua.png", "phuchua.png", "", ui::Widget::TextureResType::PLIST);
	btnPhuChua->setPosition(vecPos[10]);
	//btnPhuChua->setScale(.95f);
	addTouchEventListener(btnPhuChua, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 2;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnPhuChua);
	autoScaleNode(btnPhuChua);

	Sprite* spTree1 = Sprite::createWithSpriteFrameName("cay_01.png");
	spTree1->setPosition(520, 250);
	spTree1->setAnchorPoint(Vec2(.5f, 0));
	mLayer->addChild(spTree1);
	autoScaleNode(spTree1);

	ui::Button* btnNhaTranh = ui::Button::create("nha_tranh.png", "nha_tranh.png", "", ui::Widget::TextureResType::PLIST);
	btnNhaTranh->setPosition(vecPos[8]);
	btnNhaTranh->setScale(1.2f);
	addTouchEventListener(btnNhaTranh, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 0;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnNhaTranh);
	autoScaleNode(btnNhaTranh);

	ui::Button* btnDinhLang = ui::Button::create("dinhlang.png", "dinhlang.png", "", ui::Widget::TextureResType::PLIST);
	btnDinhLang->setPosition(vecPos[9]);
	btnDinhLang->setScale(1.2f);
	addTouchEventListener(btnDinhLang, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 1;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnDinhLang);
	autoScaleNode(btnDinhLang);

	ui::Button* btnLoiDai = ui::Button::create("dtd.png", "dtd.png", "", ui::Widget::TextureResType::PLIST);
	btnLoiDai->setPosition(vecPos[11]);
	btnLoiDai->setScale(1.2f);
	addTouchEventListener(btnLoiDai, [=]() {
		showPopupNotice(Utils::getSingleton().getStringForKey("feature_coming_soon"));
		/*if (isWaiting) return;
		showWaiting();
		tmpZoneId = 3;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();*/
	});
	mLayer->addChild(btnLoiDai);
	autoScaleNode(btnLoiDai);

	Sprite* titleNhaTranh = Sprite::createWithSpriteFrameName("title_nhatranh.png");
	titleNhaTranh->setPosition(btnNhaTranh->getPosition() + Vec2(0, 125));
	mLayer->addChild(titleNhaTranh);
	autoScaleNode(titleNhaTranh);

	Sprite* titleDinhLang = Sprite::createWithSpriteFrameName("title_dinhlang.png");
	titleDinhLang->setPosition(btnDinhLang->getPosition() + Vec2(25, 140));
	mLayer->addChild(titleDinhLang);
	autoScaleNode(titleDinhLang);

	Sprite* titlePhuChua = Sprite::createWithSpriteFrameName("title_phuchua.png");
	titlePhuChua->setPosition(btnPhuChua->getPosition() + Vec2(0, 125));
	mLayer->addChild(titlePhuChua);
	autoScaleNode(titlePhuChua);

	Sprite* spTree2 = Sprite::createWithSpriteFrameName("cay_02.png");
	spTree2->setPosition(winSize.width, 150);
	spTree2->setAnchorPoint(Vec2(1, 0));
	mLayer->addChild(spTree2);
	autoScaleNode(spTree2);

	initEventView(Vec2(0, 575), Size(1120, 40));

	if (Utils::getSingleton().userDataMe.Name.length() > 0 && Utils::getSingleton().userDataMe.DisplayName.length() == 0) {
		if (popupDisplayName == nullptr) {
			initPopupDisplayName();
		}
		showPopup(popupDisplayName);
		if (Utils::getSingleton().loginType == constant::LOGIN_NORMAL) {
			Tracker::getSingleton().trackRegistration(tracker::REGISTER_METHOD_NORMAL);
		} else if (Utils::getSingleton().loginType == constant::LOGIN_FACEBOOK) {
			Tracker::getSingleton().trackRegistration(tracker::REGISTER_METHOD_FACEBOOK);
		}
	}
	if (Utils::getSingleton().isRunningEvent) {
		runEventView(Utils::getSingleton().events, Utils::getSingleton().currentEventPosX);
	}
    Utils::getSingleton().solveCachedPurchases();
	onDynamicConfigReceived();

	if (!Utils::getSingleton().ispmE()) {
		mPosX = winSize.width / 2 - mdx * 1.5f;
		btnGuide->setPositionX(mPosX);
		btnCharge->setPositionX(mPosX += mdx);
		btnNews->setPositionX(mPosX += mdx);
		btnMail->setPositionX(mPosX += mdx);
	}
}

void MainScene::registerEventListenner()
{
	BaseScene::registerEventListenner();
	EventHandler::getSingleton().onConfigZoneReceived = std::bind(&MainScene::onConfigZoneReceived, this);
	EventHandler::getSingleton().onDynamicConfigReceived = std::bind(&MainScene::onDynamicConfigReceived, this);
	EventHandler::getSingleton().onConnectionLost = std::bind(&MainScene::onConnectionLost, this, std::placeholders::_1);
	EventHandler::getSingleton().onJoinRoom = std::bind(&MainScene::onJoinRoom, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onJoinRoomError = std::bind(&MainScene::onJoinRoomError, this, std::placeholders::_1);
	EventHandler::getSingleton().onLobbyTableSFSResponse = bind(&MainScene::onTableDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onShopHistoryDataSFSResponse = bind(&MainScene::onShopHistoryDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onShopItemsDataSFSResponse = bind(&MainScene::onShopItemsDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onNewMailSFSResponse = bind(&MainScene::onNewMailResponse, this, placeholders::_1);
	EventHandler::getSingleton().onListMailDataSFSResponse = bind(&MainScene::onListMailDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onMailContentSFSResponse = bind(&MainScene::onMailContentResponse, this, placeholders::_1);
	EventHandler::getSingleton().onNewsDataSFSResponse = bind(&MainScene::onNewsDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onExchangeItemSFSResponse = bind(&MainScene::onExchangeItemResponse, this, placeholders::_1);
	EventHandler::getSingleton().onPurchaseSuccess = bind(&MainScene::onPurchaseSuccess, this, placeholders::_1);
	EventHandler::getSingleton().onFacebookInvite = bind(&MainScene::onFacebookInvite, this, placeholders::_1);
}

void MainScene::unregisterEventListenner()
{
	BaseScene::unregisterEventListenner();
	EventHandler::getSingleton().onConnectionLost = NULL;
	EventHandler::getSingleton().onConfigZoneReceived = NULL;
	EventHandler::getSingleton().onDynamicConfigReceived = NULL;
	EventHandler::getSingleton().onJoinRoom = NULL;
	EventHandler::getSingleton().onJoinRoomError = NULL;
	EventHandler::getSingleton().onLobbyTableSFSResponse = NULL;
	EventHandler::getSingleton().onShopHistoryDataSFSResponse = NULL;
	EventHandler::getSingleton().onShopItemsDataSFSResponse = NULL;
	EventHandler::getSingleton().onNewMailSFSResponse = NULL;
	EventHandler::getSingleton().onListMailDataSFSResponse = NULL;
	EventHandler::getSingleton().onMailContentSFSResponse = NULL;
	EventHandler::getSingleton().onNewsDataSFSResponse = NULL;
	EventHandler::getSingleton().onExchangeItemSFSResponse = NULL;
	EventHandler::getSingleton().onPurchaseSuccess = NULL;
	EventHandler::getSingleton().onFacebookInvite = NULL;
}

void MainScene::editBoxReturn(cocos2d::ui::EditBox * editBox)
{
}

bool MainScene::onKeyBack()
{
	bool canBack = BaseScene::onKeyBack();
	if (canBack) {
		if (nodeWebview != nullptr && nodeWebview->isVisible()) {
			nodeWebview->setVisible(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			experimental::ui::WebView* webView = (experimental::ui::WebView*)nodeWebview->getChildByName("webview");
			webView->setVisible(false);
#endif
			return false;
		} else {
			onBackScene();
			return false;
		}
	}
	return canBack;
}

void MainScene::onConnected()
{
	BaseScene::onConnected();
	if (isGoToLobby) {
		if (tmpZoneId == -1) return;
		Utils::getSingleton().loginZoneByIndex(currentMoneyType, tmpZoneId);
		tmpZoneId = -1;
	}
}

void MainScene::onConnectionLost(std::string reason)
{
	Utils::getSingleton().chosenProviderCard = chosenProviderCard;
	Utils::getSingleton().chosenProviderSms = chosenProviderSms;
	if (isBackToLogin) {
		Utils::getSingleton().goToLoginScene();
		return;
	}
	if (isGoToLobby && tmpZoneId >= 0) {
		showWaiting();
		Utils::getSingleton().connectZoneByIndex(currentMoneyType, tmpZoneId);
	} else {
		handleClientDisconnectionReason(reason);
	}
}

void MainScene::onConfigZoneReceived()
{
	hideWaiting();
}

void MainScene::onLoginZoneError(short int code, std::string msg)
{
	BaseScene::onLoginZoneError(code, msg);
	if (isGoToLobby) {
		isGoToLobby = false;
		hideWaiting();
		showPopupNotice(msg);
	}
}

void MainScene::onErrorResponse(unsigned char code, std::string msg)
{
	BaseScene::onErrorResponse(code, msg);
	hideWaiting();
	if (code == 0 && popupShop->isVisible()) {
		showPopupNotice(msg, [=]() {
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
		}, false);
		return;
	}
	if (code == 51 && popupDisplayName->isVisible()) {
		hidePopup(popupDisplayName);
		setDisplayName(tmpDisplayName);
		return;
	}
	if (code == 32) {
		if (popupShop != NULL && popupShop->isVisible()) {
			Node* nodeHistory = popupShop->getChildByName("nodehistory");
			int page = nodeHistory->getChildByName("nodepage")->getTag();
			SFSRequest::getSingleton().RequestShopHistory(page);
		}else if (popupCharge != NULL && popupCharge->isVisible() && msg.find("Quan") == string::npos) {
			chargingProvider = "";
		}
	}
	if (msg.length() == 0) return;
	showPopupNotice(msg);
}

void MainScene::onJoinRoom(long roomId, std::string roomName)
{
	if (roomName.at(0) == 'g' && roomName.at(2) == 'b') {
		Utils::getSingleton().goToGameScene();
	}
}

void MainScene::onJoinRoomError(std::string msg)
{
	showPopupNotice(msg);
}

void MainScene::onTableDataResponse(LobbyListTable data)
{
	hideWaiting();
	Utils::getSingleton().moneyType = currentMoneyType;
	if (isGoToLobby) {
		Utils::getSingleton().goToLobbyScene();
	}
}

void MainScene::onShopHistoryDataResponse(std::vector<ShopHistoryData> list)
{
	/*list.clear();
	int numb = rand() % 20;
	for (int i = 0; i < numb; i++) {
		ShopHistoryData data;
		data.Id = rand() % 100;
		data.CreateDate = "02/12/2016 12:00:00";
		data.UpdateDate = "02/12/2016 12:00:00";
		data.Content = "Viettel 500k";
		data.ItemId = rand() % 4;
		data.Name = "viettel 500k";
		data.Price = 50;
		data.Status = rand() % 4 + 1;
		list.push_back(data);
	}*/

	int px = 60;
	vector<int> widths = { 120, 210, 210, 210, 150 };
	vector<int> posX = {};
	for (int i = 0; i < 4; i++) {
		posX.push_back(px);
		px += widths[i] / 2 + widths[i + 1] / 2;
	}
	posX.push_back(px);
	vector<string> strStatus = { "dat_hang", "xac_nhan", "hoan_tat", "huy", "hoan_tra" };
	ui::ScrollView* scrollHistory = (ui::ScrollView*)(popupShop->getChildByTag(12)->getChildByName("scrollhistory"));
	int heightHistory = list.size() * 40;
	if (heightHistory < scrollHistory->getContentSize().height) {
		heightHistory = scrollHistory->getContentSize().height;
	}
	scrollHistory->setInnerContainerSize(Size(scrollHistory->getContentSize().width, heightHistory));
	heightHistory -= 5;
	string fontName = "fonts/myriad.ttf";
	int fontSize = 25;
	float scaleX = 1;
	for (int i = 0; i < list.size(); i++) {
		int tag = i * 7;
		ui::Button* btn, *btnCancel;
		Label *lb1, *lb2, *lb3, *lb4, *lb5;
		lb1 = (Label*)scrollHistory->getChildByTag(tag);
		bool isNewBtn;
		if (lb1 == nullptr) {
			lb1 = Label::create("", fontName, fontSize);
			lb1->setPosition(posX[0], heightHistory - 10);
			lb1->setColor(Color3B::BLACK);
			lb1->setScaleX(scaleX);
			lb1->setTag(tag);
			scrollHistory->addChild(lb1);

			lb2 = Label::create("", fontName, fontSize);
			lb2->setPosition(posX[1], heightHistory - 10);
			lb2->setHorizontalAlignment(TextHAlignment::CENTER);
			lb2->setColor(Color3B::BLACK);
			lb2->setTag(tag + 1);
			lb2->setWidth(widths[1]);
			lb2->setHeight(27);
			lb2->setScaleX(scaleX);
			scrollHistory->addChild(lb2);

			lb3 = Label::create("", fontName, fontSize);
			lb3->setPosition(posX[2], heightHistory - 10);
			lb3->setTag(tag + 2);
			lb3->setScaleX(scaleX);
			lb3->setColor(Color3B::BLACK);
			scrollHistory->addChild(lb3);

			lb4 = Label::create("", fontName, fontSize);
			lb4->setPosition(posX[3], heightHistory - 10);
			lb4->setTag(tag + 3);
			lb4->setScaleX(scaleX);
			lb4->setColor(Color3B::BLACK);
			scrollHistory->addChild(lb4);

			lb5 = Label::create("", fontName, fontSize);
			lb5->setPosition(posX[4], heightHistory - 10);
			lb5->setColor(Color3B::YELLOW);
			lb5->setTag(tag + 4);
			lb5->setScaleX(scaleX);
			lb5->setColor(Color3B::BLACK);
			scrollHistory->addChild(lb5);

			btn = ui::Button::create("white.png", "", "", ui::Widget::TextureResType::PLIST);
			btn->setContentSize(Size(scrollHistory->getContentSize().width, 35));
			btn->setPosition(Vec2(scrollHistory->getContentSize().width / 2, heightHistory - 10));
			btn->setScale9Enabled(true);
			btn->setOpacity(0);
			btn->setTag(tag + 5);
			scrollHistory->addChild(btn);
			isNewBtn = true;

			btnCancel = ui::Button::create("empty.png", "empty.png", "", ui::Widget::TextureResType::PLIST);
			btnCancel->setTitleText("[" + Utils::getSingleton().getStringForKey("huy") + "]");
			btnCancel->setTitleFontName("fonts/aurora.ttf");
			btnCancel->setTitleFontSize(22);
			btnCancel->setTitleColor(Color3B::RED);
			btnCancel->setPosition(Vec2(lb5->getPositionX() + 50, heightHistory - 9));
			btnCancel->setContentSize(Size(50, 30));
			btnCancel->setScale9Enabled(true);
			btnCancel->setTag(tag + 6);
			scrollHistory->addChild(btnCancel);
		} else {
			for (int i = 0; i < 7; i++) {
				scrollHistory->getChildByTag(tag + i)->setPositionY(heightHistory - (i == 6 ? 15 : 10));
			}
			lb2 = (Label*)scrollHistory->getChildByTag(tag + 1);
			lb3 = (Label*)scrollHistory->getChildByTag(tag + 2);
			lb4 = (Label*)scrollHistory->getChildByTag(tag + 3);
			lb5 = (Label*)scrollHistory->getChildByTag(tag + 4);
			btn = (ui::Button*)scrollHistory->getChildByTag(tag + 5);
			btnCancel = (ui::Button*)scrollHistory->getChildByTag(tag + 6);
			btn->setTouchEnabled(true);
			isNewBtn = false;
		}
		addTouchEventListener(btn, [=]() {
			showPopupNotice(list[i].Content);
		}, isNewBtn);
		addTouchEventListener(btnCancel, [=]() {
			showPopupNotice(Utils::getSingleton().getStringForKey("ban_muon_huy_don_hang_nay"), [=]() {
				//int itemId = atoi(btnCancel->getName().c_str());
				SFSRequest::getSingleton().RequestCancelItemShop(list[i].Id);
			});
		}, isNewBtn);
		if (list[i].Status < 1) {
			list[i].Status = 1;
		} else if (list[i].Status > strStatus.size()) {
			list[i].Status = strStatus.size();
		}
		if (list[i].Status == 1) {
			lb5->setPositionX(posX[4] - 20);
			btnCancel->setPosition(Vec2(lb5->getPositionX() + 58, lb5->getPositionY() + 2));
		} else {
			lb5->setPositionX(posX[4]);
		}
		lb1->setString(to_string((int)list[i].Id));
		lb2->setString(Utils::getSingleton().trim(list[i].Name));
		lb3->setString(list[i].CreateDate);
		lb4->setString(list[i].UpdateDate);
		lb5->setString(Utils::getSingleton().getStringForKey(strStatus[list[i].Status - 1]));
		btnCancel->setName(to_string((int)list[i].ItemId));
		btnCancel->setVisible(list[i].Status == 1);

		heightHistory -= 40;
	}
	int count = scrollHistory->getChildrenCount();
	for (int j = list.size() * 7; j < count; j++) {
		if (j % 7 != 5 && j % 7 != 6) {
			((Label*)scrollHistory->getChildByTag(j))->setString("");
		} else {
			((ui::Button*)scrollHistory->getChildByTag(j))->setTouchEnabled(false);
		}
	}
}

void MainScene::onShopItemsDataResponse(std::vector<ShopItemData> list)
{
	ui::ScrollView* scrollCard = (ui::ScrollView*)(popupShop->getChildByTag(10)->getChildByName("scrollcard"));
	vector<vector<ShopItemData>> cards;
	for (int i = 0; i < 4; i++) {
		cards.push_back(vector<ShopItemData>());
	}
	for (int i = 0; i < list.size(); i++) {
		list[i].Name = Utils::getSingleton().trim(list[i].Name);
		if (list[i].ItemType == 1) {
			cards[0].push_back(list[i]);
		} else if (list[i].ItemType == 3) {
			cards[1].push_back(list[i]);
		} else if (list[i].ItemType == 4) {
			cards[2].push_back(list[i]);
		} else if (list[i].ItemType == 5) {
			cards[3].push_back(list[i]);
		}
	}

	/*int size = cards[0].size();
	if (size < cards[1].size()) size = cards[1].size();
	if (size < cards[2].size()) size = cards[2].size();
	if (size < cards[3].size()) size = cards[3].size();

	int widthCard = size * 165;
	if (widthCard < scrollCard->getContentSize().width) {
		widthCard = scrollCard->getContentSize().width;
	}
	scrollCard->setInnerContainerSize(Size(widthCard, scrollCard->getContentSize().height));*/

	int cardHeight = 120;
	int cardPerRow = 6;
	int heightCard = ((cards[0].size() + cards[1].size() + cards[2].size() + cards[3].size() - 1) / cardPerRow + 1) * cardHeight;
	if (heightCard < scrollCard->getContentSize().height) {
		heightCard = scrollCard->getContentSize().height;
	}
	scrollCard->setInnerContainerSize(Size(scrollCard->getContentSize().width, heightCard));

	int count = -1;
	for (int i = 0; i < cards.size(); i++) {
		for (int j = 0; j < cards[i].size(); j++) {
			count++;
			string str = Utils::getSingleton().getStringForKey("xac_nhan_mua_vat_pham");
			string strMoney = Utils::getSingleton().formatMoneyWithComma(cards[i][j].PriceChange);
			string msg = String::createWithFormat(str.c_str(), cards[i][j].Name.c_str(), strMoney.c_str())->getCString();

			ui::Button* btn = ui::Button::create("box_shop.png", "", "", ui::Widget::TextureResType::PLIST);
			btn->setPosition(Vec2(70 + (count % cardPerRow) * 152, heightCard - 40 - (count/ cardPerRow) * cardHeight));
			btn->setBright(false);
			btn->setTag((i + 1) * 100 + j);
			addTouchEventListener(btn, [=]() {
				if (Utils::getSingleton().userDataMe.MoneyReal < cards[i][j].Price) {
					showPopupNotice(Utils::getSingleton().getStringForKey("khong_du_tien_doi_thuong"));
				} else {
					showPopupNotice(msg, [=]() {
						SFSRequest::getSingleton().RequestExchangeItem(cards[i][j].Id);
					});
				}
			});
			scrollCard->addChild(btn);

			Sprite* sp = Sprite::create();
			sp->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height - 30);
			sp->setName("itemimage");
			btn->addChild(sp);

			Sprite* spCoin = Sprite::createWithSpriteFrameName("icon_gold.png");
			spCoin->setScale(.5f);
			btn->addChild(spCoin);

			Label* lb1 = Label::create(Utils::getSingleton().formatMoneyWithComma(cards[i][j].Price), "fonts/guanine.ttf", 18);
			lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, -10);
			lb1->setColor(Color3B::YELLOW);
			btn->addChild(lb1);

			Label* lb2 = Label::create(Utils::getSingleton().formatMoneyWithComma(cards[i][j].PriceChange), "fonts/guanine.ttf", 18);
			lb2->setPosition(btn->getContentSize().width / 2, 19);
			lb2->setColor(Color3B::WHITE);
			btn->addChild(lb2);

			spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
				+ spCoin->getContentSize().width * spCoin->getScale() / 2, lb1->getPositionY() - 2);

			Utils::getSingleton().LoadTextureFromURL(cards[i][j].Image, [=](Texture2D* texture) {
				sp->initWithTexture(texture);
				float scale1 = 106 / sp->getContentSize().width;
				float scale2 = 46 / sp->getContentSize().height;
				if (scale1 < scale2) sp->setScale(scale1);
				else sp->setScale(scale2);
			});
		}
	}

	map<unsigned char, string> names;
	for (int i = 0; i < list.size(); i++) {
		names[list[i].Id] = list[i].Name;
	}

	ui::ScrollView* scrollItem = (ui::ScrollView*)(popupShop->getChildByTag(11)->getChildByName("scrollitem"));

	vector<unsigned char> items;
	vector<string> images;
	vector<long> moneyItems;
	for (int i = 0; i < list.size(); i++) {
		if (list[i].ItemType == 2) {
			items.push_back(list[i].Id);
			images.push_back(list[i].Image);
			moneyItems.push_back(list[i].Price);
		}
	}

	/*int widthItem = (items.size() / 2) * 200;
	if (widthItem < scrollItem->getContentSize().width) {
		widthItem = scrollItem->getContentSize().width;
	}
	scrollItem->setInnerContainerSize(Size(widthItem, scrollItem->getContentSize().height));*/

	int itemHeight = 180;
	int itemPerRow = 4;
	int heightItem = ((items.size() - 1) / itemPerRow + 1) * itemHeight;
	if (heightItem < scrollItem->getContentSize().width) {
		heightItem = scrollItem->getContentSize().width;
	}
	scrollItem->setInnerContainerSize(Size(scrollItem->getContentSize().width, heightItem));
	for (int i = 0; i < items.size(); i++) {
		string str = Utils::getSingleton().getStringForKey("xac_nhan_mua_vat_pham");
		string strMoney = Utils::getSingleton().formatMoneyWithComma(moneyItems[i]);
		string msg = String::createWithFormat(str.c_str(), names[items[i]].c_str(), strMoney.c_str())->getCString();

		ui::Button* btn = ui::Button::create("box_shop.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setPosition(Vec2(105 + (i % itemPerRow) * 230, heightItem - 65 - (i / itemPerRow) * itemHeight));
		btn->setContentSize(Size(182, 128));
		btn->setScale9Enabled(true);
		btn->setBright(false);
		btn->setTag(i);
		//btn->setScale(.95f);
		addTouchEventListener(btn, [=]() {
			if (Utils::getSingleton().userDataMe.MoneyReal < moneyItems[i]) {
				showPopupNotice(Utils::getSingleton().getStringForKey("khong_du_tien_doi_thuong"));
			} else {
				showPopupNotice(msg, [=]() {
					SFSRequest::getSingleton().RequestExchangeItem(items[i]);
				});
			}
		});
		scrollItem->addChild(btn);

		Sprite* sp = Sprite::create();
		sp->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 15);
		sp->setName("itemimage");
		btn->addChild(sp);

		Sprite* spCoin = Sprite::createWithSpriteFrameName("icon_gold.png");
		spCoin->setScale(.5f);
		btn->addChild(spCoin);

		Label* lb1 = Label::create(Utils::getSingleton().formatMoneyWithComma(moneyItems[i]), "fonts/guanine.ttf", 20);
		lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, btn->getContentSize().height / 2 - 80);
		lb1->setColor(Color3B::YELLOW);
		btn->addChild(lb1);

		Label* lb2 = Label::create(names[items[i]], "fonts/guanine.ttf", 18);
		lb2->setWidth(175);
		lb2->setHeight(30);
		lb2->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 - 45);
		lb2->setColor(Color3B::WHITE);
		lb2->setHorizontalAlignment(TextHAlignment::CENTER);
		btn->addChild(lb2);

		spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
			+ spCoin->getContentSize().width * spCoin->getScale() / 2, lb1->getPositionY() - 3);

		Utils::getSingleton().LoadTextureFromURL(images[i], [=](Texture2D* texture) {
			sp->initWithTexture(texture);
			float scale1 = 170 / sp->getContentSize().width;
			float scale2 = 90 / sp->getContentSize().height;
			if (scale1 < scale2) sp->setScale(scale1);
			else sp->setScale(scale2);
		});
	}
}

void MainScene::onExchangeItemResponse(std::string msg)
{
	showPopupNotice(msg);
}

void MainScene::onNewMailResponse(unsigned char count)
{
	if (count > 0) {
		lbNewMail->setString(to_string((int)count));
		lbNewMail->getParent()->setVisible(true);
	} else {
		lbNewMail->getParent()->setVisible(false);
	}
}

void MainScene::onListMailDataResponse(std::vector<MailData> list)
{
	/*int numb = rand() % 20;
	for (int i = 0; i < numb; i++) {
		MailData data;
		data.Id = rand() % 100;
		data.Date = "02/12/2016";
		data.Title = "This is a mail title";
		data.Sender = "Stormus";
		list.push_back(data);
	}*/

	if (isWaitPopupMail) {
		isWaitPopupMail = false;
		hideWaiting();
		showPopup(popupMail);
	}

	vector<int> posX = { -420, -290, 0, 340 };
	vector<int> widths = { 50, 130, 420, 120 };
	ui::ScrollView* scroll = (ui::ScrollView*)popupMail->getChildByName("scroll");
	int height = list.size() * 60;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
	for (int i = 0; i < list.size(); i++) {
		vector<Label*> lbs;
		ui::Button* btn;
		if (i < scroll->getChildrenCount() / 5) {
			for (int j = 0; j < 4; j++) {
				Label* lb = (Label*)scroll->getChildByTag(i * 5 + j);
				lb->setPosition(posX[j] + scroll->getContentSize().width / 2, height - 5 - i * 60);
				lb->setVisible(true);
				lbs.push_back(lb);
			}

			btn = (ui::Button*)scroll->getChildByTag(i * 5 + 4);
			btn->setPosition(Vec2(scroll->getContentSize().width / 2, height - 5 - i * 60));
			btn->setVisible(true);
		} else {
			for (int j = 0; j < 4; j++) {
				Label* lbDetail = Label::create("", list[i].IsRead ? "fonts/arial.ttf" : "fonts/arialbd.ttf", 20);
				lbDetail->setWidth(widths[j]);
				lbDetail->setHeight(52);
				lbDetail->setAnchorPoint(Vec2(.5f, 1));
				lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
				lbDetail->setPosition(posX[j] + scroll->getContentSize().width / 2, height - 5 - i * 60);
				lbDetail->setTag(i * 5 + j);
				lbDetail->setColor(Color3B::BLACK);
				scroll->addChild(lbDetail);
				lbs.push_back(lbDetail);
			}

			btn = ui::Button::create("box1.png", "box1.png", "", ui::Widget::TextureResType::PLIST);
			btn->setContentSize(Size(scroll->getContentSize().width, lbs[0]->getHeight()));
			btn->setPosition(Vec2(scroll->getContentSize().width / 2, height - 5 - i * 60));
			btn->setAnchorPoint(Vec2(.5f, 1));
			btn->setScale9Enabled(true);
			btn->setOpacity(0);
			btn->setTag(i * 5 + 4);
			addTouchEventListener(btn, [=]() {
				Node* nodeDetail = popupMail->getChildByName("nodedetail");
				for (int i = 0; i < 4; i++) {
					Label* lb = (Label*)nodeDetail->getChildByTag(i);
					lb->setString(lbs[i]->getString());
					lbs[i]->setSystemFontName("fonts/arial.ttf");
					lbs[i]->setSystemFontSize(20);
				}
				SFSRequest::getSingleton().RequestMailContent(atoi(btn->getName().c_str()));
			});
			scroll->addChild(btn);
		}
		btn->setName(to_string((int)list[i].Id));
		lbs[0]->setString(to_string((popupMail->getChildByName("nodepage")->getTag() - 1) * 10 + i + 1) + ".");
		lbs[1]->setString(list[i].Date);
		lbs[2]->setString(Utils::getSingleton().trim(list[i].Title));
		lbs[3]->setString(list[i].Sender);
	}

	int count = scroll->getChildrenCount();
	for (int i = list.size() * 5; i < count; i++) {
		scroll->getChildByTag(i)->setVisible(false);
	}
}

void MainScene::onMailContentResponse(std::string content)
{
	Node* nodeDetail = popupMail->getChildByName("nodedetail");
	nodeDetail->setVisible(true);
	if (content.length() == 0) return;
	Label* lb = (Label*)nodeDetail->getChildByTag(2);
	lb->setString(Utils::getSingleton().trim(content));
}

void MainScene::onNewsDataResponse(std::vector<NewsData> list)
{
	/*int numb = rand() % 20;
	for (int i = 0; i < numb; i++) {
		NewsData data;
		data.Id = rand() % 100;
		data.Title = "Title news " + to_string(data.Id);
		data.Content = "Content news " + to_string(data.Id);
		data.Date = "02/12/2016";
		list.push_back(data);
	}*/
	if (isWaitPopupNews) {
		isWaitPopupNews = false;
		showPopup(popupNews);
		hideWaiting();
	}

	ui::ScrollView* scrollTitle = (ui::ScrollView*)popupNews->getChildByName("scrolltitle");
	ui::ScrollView* scrollContent = (ui::ScrollView*)popupNews->getChildByName("scrollcontent");
	Label* lbContent = (Label*)scrollContent->getChildByName("lbcontent");

	if (list.size() > 0) {
		lbContent->setString(list[0].Title + "\n\n" + list[0].Content + "\n\n" + list[0].Date);
	} else {
		lbContent->setString("");
	}
	int heightContent = lbContent->getContentSize().height;
	if (heightContent < scrollContent->getContentSize().height) {
		heightContent = scrollContent->getContentSize().height;
	}
	scrollContent->setInnerContainerSize(Size(scrollContent->getContentSize().width, heightContent));
	lbContent->setPosition(0, heightContent);

	int btnWidth = 175;
	int widthTitle = list.size() * btnWidth;
	if (widthTitle < scrollTitle->getContentSize().width) {
		widthTitle = scrollTitle->getContentSize().width;
	}
	scrollTitle->setInnerContainerSize(Size(widthTitle, scrollTitle->getContentSize().height));
	for (int i = 0; i < list.size(); i++) {
		string strTitle = list[i].Title.length() < 17 ? list[i].Title : list[i].Title.substr(0, 16);
		int indexTitle = strTitle.find_last_of(' ');
		strTitle = strTitle.substr(0, indexTitle);
		ui::Button* btn = (ui::Button*) scrollTitle->getChildByTag(i);
		bool isNewBtn;
		if (btn == nullptr) {
			isNewBtn = true;
			btn = ui::Button::create();
			btn->setTitleFontName("fonts/arialbd.ttf");
			btn->setTitleColor(Color3B::BLACK);
			btn->setTitleFontSize(25);
			btn->setContentSize(Size(btnWidth, scrollTitle->getContentSize().height));
			btn->setScale9Enabled(true);
			btn->setTag(i);
			scrollTitle->addChild(btn);
		} else {
			isNewBtn = false;
			btn->setVisible(true);
		}
		btn->setTitleColor(Color3B::BLACK);
		btn->loadTextures(i == 0 ? "box4.png" : "box2.png", "box4.png", "", ui::Widget::TextureResType::PLIST);
		btn->setPosition(Vec2(btnWidth/2 + i * btnWidth, scrollTitle->getContentSize().height / 2));
		btn->setTitleText(strTitle);
		addTouchEventListener(btn, [=]() {
			ui::Button* btn1 = (ui::Button*)scrollTitle->getChildByTag(scrollTitle->getTag());
			btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
			scrollTitle->setTag(i);

			lbContent->setString(list[i].Title + "\n\n" + list[i].Content + "\n\n" + list[i].Date);
			int height = lbContent->getContentSize().height;
			if (height < scrollContent->getContentSize().height) {
				height = scrollContent->getContentSize().height;
			}
			scrollContent->setInnerContainerSize(Size(scrollContent->getContentSize().width, height));
			lbContent->setPosition(0, height);
		}, isNewBtn);
	}
	int count = scrollTitle->getChildrenCount();
	for (int i = list.size(); i < count; i++) {
		scrollTitle->getChildByTag(i)->setVisible(false);
	}
}

void MainScene::onPurchaseSuccess(std::string token)
{
    if(token.length() > 0){
        SFSRequest::getSingleton().RequestPayment(token, isChargeQuan);
	} else {
		hideWaiting();
	}
}

void MainScene::onFacebookInvite(std::string token)
{
	if (token.length() > 0) {
		SFSRequest::getSingleton().RequestVerifyFBInvite(token);
	}
}

void MainScene::onDynamicConfigReceived()
{
	strProviders = Utils::getSingleton().dynamicConfig.CashValue;
	if (Utils::getSingleton().allowEventPopup 
		&& Utils::getSingleton().dynamicConfig.Popup
		&& !Utils::getSingleton().hasShowEventPopup
		&& Utils::getSingleton().ispmE()) {
		//Utils::getSingleton().hasShowEventPopup = true;
		//showWebView(Utils::getSingleton().dynamicConfig.PopupUrl);
		btnEvent->setVisible(true);
	}
	GameLogger::getSingleton().setEnabled(Utils::getSingleton().dynamicConfig.Log);
	GameLogger::getSingleton().setHost(Utils::getSingleton().dynamicConfig.LogHost);
	GameLogger::getSingleton().setUser(Utils::getSingleton().userDataMe);
}

void MainScene::onDownloadedPlistTexture(int numb)
{
	BaseScene::onDownloadedPlistTexture(numb);
	hideWaiting();
}

void MainScene::onBackScene()
{
	Utils::getSingleton().chosenProviderCard = chosenProviderCard;
	Utils::getSingleton().chosenProviderSms = chosenProviderSms;
	//showPopupNotice(Utils::getSingleton().getStringForKey("ban_muon_dang_xuat"), [=]() {
		showWaiting();
		isBackToLogin = true;
		SFSRequest::getSingleton().Disconnect();
		Utils::getSingleton().goToLoginScene();
	//});
}

void MainScene::onChangeMoneyType(int type)
{
	BaseScene::onChangeMoneyType(type);
	currentMoneyType = type;
}

void MainScene::initPopupCharge()
{
	bool pmE = Utils::getSingleton().ispmE();

	popupCharge = createPopup("", true, true);
	if (!popupCharge) return;
	popupCharge->setTag(0);

	Size size = Size(900, 446);
	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgScroll->setContentSize(size);
	bgScroll->setPosition(0, -25);
	popupCharge->addChild(bgScroll);

	Node* nodeCard = Node::create();
	nodeCard->setName("nodecard");
	nodeCard->setVisible(true);
	nodeCard->setTag(100);
	nodeCard->setPosition(0, 0);
	popupCharge->addChild(nodeCard);

	Node* nodeSms = Node::create();
	nodeSms->setName("nodesms");
	nodeSms->setVisible(false);
	nodeSms->setTag(101);
	nodeSms->setPosition(0, 0);
	popupCharge->addChild(nodeSms);
    
    Node* nodeStore = Node::create();
    nodeStore->setName("nodestore");
    nodeStore->setVisible(false);
	nodeStore->setTag(102);
	nodeStore->setPosition(0, 0);
    popupCharge->addChild(nodeStore);

	vector<Node*> nodes;
	nodes.push_back(nodeCard);
	nodes.push_back(nodeSms);
	nodes.push_back(nodeStore);

	int moneyTypePosX = -100;
	Node* nodeMoneyType = Node::create();
	nodeMoneyType->setName("nodemoneytype");
	nodeMoneyType->setPosition(moneyTypePosX, 20);
	nodeMoneyType->setVisible(pmE);
	popupCharge->addChild(nodeMoneyType);

	ui::EditBox* tfSeri = ui::EditBox::create(Size(350, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	ui::EditBox* tfCode = ui::EditBox::create(Size(350, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	ui::ScrollView* scrollProvider = ui::ScrollView::create();

	vector<string> texts = { "nap_the", "nap_sms", "nap_iap" }; 
	int x = -320;
	int y = 219;
	Size btnSize = Size(205, 50);
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box4.png" : "box2.png", "box4.png", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/arialbd.ttf");
		btn->setTitleFontSize(30);
		btn->setTitleColor(Color3B::BLACK);
		btn->setContentSize(btnSize);
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		btn->setTag(10 + i);
		addTouchEventListener(btn, [=]() {
			if (popupCharge->getTag() == i) return; 
			popupCharge->getChildByTag(100 + i)->setVisible(true);
			popupCharge->getChildByTag(100 + popupCharge->getTag())->setVisible(false);
			ui::Button* btn1 = (ui::Button*)popupCharge->getChildByTag(10 + popupCharge->getTag());
			btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
			popupCharge->setTag(i);

			if (i == 0) {
				scrollProvider->setVisible(true);
				nodeMoneyType->setPosition(moneyTypePosX, 20);
				for (Node* n : scrollProvider->getChildren()) {
					n->setTag(0);
					n->setVisible(true);
					n->setColor(Color3B::GRAY);
				}
				if (chosenProviderCard.length() > 0) {
					Node* chosenBtn = scrollProvider->getChildByName(chosenProviderCard);
					chosenBtn->setTag(1);
					chosenBtn->setColor(Color3B::WHITE);
				}
				onChooseProviderCard(chosenProviderCard);
			} else if (i == 1) {
				scrollProvider->scrollToLeft(.3f, true);
				scrollProvider->setVisible(true);
				nodeMoneyType->setPosition(moneyTypePosX, 20);
				for (Node* n : scrollProvider->getChildren()) {
					string name = n->getName();
					n->setTag(0);
					n->setColor(Color3B::GRAY);
					n->setVisible(name.compare("viettel") == 0 || name.compare("mobifone") == 0 || name.compare("vinaphone") == 0);
				}
				if (chosenProviderSms.length() > 0) {
					Node* chosenBtn = scrollProvider->getChildByName(chosenProviderSms);
					chosenBtn->setTag(1);
					chosenBtn->setColor(Color3B::WHITE);
				}
				if (popupChooseSms == NULL) {
					std::vector<std::string> smsProviders = { "viettel", "mobifone", "vinaphone" };
					popupChooseSms = createPopupChooseProvider(Utils::getSingleton().getStringForKey("chon_mang_sms"), smsProviders, [=](string provider) {
						onChooseProviderSms(provider);
					});
				} else {
					resetPopupChooseProvider(popupChooseSms);
				}
				showPopup(popupChooseSms);
			} else if(i == 2){
				scrollProvider->setVisible(false);
				nodeMoneyType->setPosition(moneyTypePosX, 150);
			}
		});
		popupCharge->addChild(btn);
		x += btnSize.width;
	}

	int scrollProviderHeight = 120;
	scrollProvider->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollProvider->setBounceEnabled(true);
	scrollProvider->setPosition(Vec2(-size.width/2 + 15, size.height/2 - 165));
	scrollProvider->setContentSize(Size(size.width - 30, scrollProviderHeight));
	scrollProvider->setInnerContainerSize(Size(strProviders.size() * 170, scrollProviderHeight));
	scrollProvider->setScrollBarEnabled(true);
	scrollProvider->setName("scrollprovider");
	popupCharge->addChild(scrollProvider);

	int xp = 80;
	int yp = 70;
	string smsContent = Utils::getSingleton().gameConfig.smsVT;
	//int strIndex = smsContent.find_last_of(' ');
	string smsTarget = Utils::getSingleton().dynamicConfig.SmsNap;// smsContent.substr(strIndex + 1, smsContent.length() - strIndex);
	//smsContent = smsContent.substr(0, strIndex);
	smsContent = Utils::getSingleton().replaceString(smsContent, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
	for (int i = 1; i <= strProviders.size(); i++) {
		string strimg = strProviders[i - 1] + ".png";
		ui::Button* btnProvider = ui::Button::create(strimg, strimg, "", ui::Widget::TextureResType::PLIST);
		btnProvider->setPosition(Vec2(xp, yp));
		btnProvider->setTag(0);
		btnProvider->setName(strProviders[i - 1]);
		btnProvider->setColor(Color3B::GRAY);
		addTouchEventListener(btnProvider, [=]() {
			if (btnProvider->getTag() == 1) return;
			if (nodeCard->isVisible()) {
				onChooseProviderCard(btnProvider->getName());
			} else {
				onChooseProviderSms(btnProvider->getName());
			}
		});
		scrollProvider->addChild(btnProvider);
		xp += 170;

		/*ui::Scale9Sprite* bgProvider = ui::Scale9Sprite::createWithSpriteFrameName("box8.png");
		bgProvider->setContentSize(btnProvider->getContentSize() + Size(40, 70));
		bgProvider->setPosition(btnProvider->getContentSize().width / 2, btnProvider->getContentSize().height / 2 - 20);
		bgProvider->setVisible(false);
		bgProvider->setTag(1);
		btnProvider->addChild(bgProvider, -1);*/

		if (btnProvider->getName().compare(chosenProviderCard) == 0) {
			btnProvider->setColor(Color3B::WHITE);
		}
	}
	
	vector<Label*> lbs;
	vector<ui::CheckBox*> cbs;
	for (int i = 0; i < 2; i++) {
		ui::CheckBox* checkbox = ui::CheckBox::create();
		checkbox->loadTextureBackGround("box0.png", ui::Widget::TextureResType::PLIST);
		checkbox->loadTextureFrontCross("check.png", ui::Widget::TextureResType::PLIST);
		checkbox->setPosition(Vec2(150 * i, 0));
		checkbox->setSelected(false);
		checkbox->setTag(i);
		nodeMoneyType->addChild(checkbox);
		cbs.push_back(checkbox);

		Label* lb = Label::create("", "fonts/arialbd.ttf", 22);
		lb->setPosition(checkbox->getPosition() + Vec2(30, 0));
		lb->setAnchorPoint(Vec2(0, .5f));
		lb->setColor(Color3B::BLACK);
		nodeMoneyType->addChild(lb);
		lbs.push_back(lb);
	}

	cbs[0]->setSelected(true);
	cbs[0]->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		if (type == ui::CheckBox::EventType::SELECTED) {
			cbs[1]->setSelected(false);
		} else if (type == ui::CheckBox::EventType::UNSELECTED) {
			cbs[1]->setSelected(true);
		}
		isChargeQuan = cbs[0]->isSelected();
		updateChargeRateCard(cbs[0]->isSelected());
		updateSmsInfo(cbs[0]->isSelected());
	});
	cbs[1]->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		if (type == ui::CheckBox::EventType::SELECTED) {
			cbs[0]->setSelected(false);
		} else if (type == ui::CheckBox::EventType::UNSELECTED) {
			cbs[0]->setSelected(true);
		}
		isChargeQuan = cbs[0]->isSelected();
		updateChargeRateCard(cbs[0]->isSelected());
		updateSmsInfo(cbs[0]->isSelected());
	});
	lbs[0]->setString(Utils::getSingleton().getStringForKey("quan"));
	lbs[1]->setString(Utils::getSingleton().getStringForKey("xu"));
	ui::CheckBox* cbQuan = cbs[0];

	//Node Card
	ui::Scale9Sprite* bgCardInfoInput = ui::Scale9Sprite::createWithSpriteFrameName("box7.png");
	bgCardInfoInput->setContentSize(Size(size.width - 20, 200));
	bgCardInfoInput->setPosition(0, -140);
	nodeCard->addChild(bgCardInfoInput);

	Node* nodeCardInfo = Node::create();
	nodeCardInfo->setPosition(-280, bgCardInfoInput->getPositionY());
	nodeCardInfo->setName("nodecardinfo");
	nodeCard->addChild(nodeCardInfo);

	ui::Scale9Sprite* bgCardInfo = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgCardInfo->setContentSize(Size(230, 200));
	bgCardInfo->setOpacity(100);
	nodeCardInfo->addChild(bgCardInfo);

	ui::ScrollView* scrollInfo = ui::ScrollView::create();
	scrollInfo->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollInfo->setBounceEnabled(true);
	scrollInfo->setPosition(Vec2(-bgCardInfo->getContentSize().width / 2 + 5, -bgCardInfo->getContentSize().height / 2 + 5));
	scrollInfo->setContentSize(bgCardInfo->getContentSize() - Size(10, 10));
	scrollInfo->setScrollBarEnabled(false);
	scrollInfo->setName("scrollinfo");
	nodeCardInfo->addChild(scrollInfo);

	int height = moneys.size() * 25;
	if (height < scrollInfo->getContentSize().height) {
		height = scrollInfo->getContentSize().height;
	}
	scrollInfo->setInnerContainerSize(Size(scrollInfo->getContentSize().width, height));
	//for (int i = 0; i < 2; i++) {
		for (int j = 0; j < moneys.size(); j++) {
			string str1 = Utils::getSingleton().getStringForKey("the") + " " + to_string(moneys[j]) + "k = ";
			string str2 = to_string(moneys[j]) + "k " + "Quan";//(i == 0 ? "Quan" : "Xu");

			Label* lb1 = Label::create(str1, "fonts/arial.ttf", 22);
			lb1->setTag(j * 2);
			lb1->setAnchorPoint(Vec2(1, .5f));
			lb1->setPosition(scrollInfo->getContentSize().width/2, height - 15);
			lb1->setColor(Color3B::BLACK);
			scrollInfo->addChild(lb1);

			Label* lb2 = Label::create(str2, "fonts/arial.ttf", 22);
			lb2->setTag(j * 2 + 1);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setPosition(scrollInfo->getContentSize().width/2 + 3, height - 15);
			lb2->setColor(Color3B::YELLOW);//i == 0 ? Color3B::YELLOW : Color3B(0, 255, 255));
			scrollInfo->addChild(lb2);

			height -= 32;
		}
	//}

	Node* nodeInput = Node::create();
	nodeInput->setPosition(120, bgCardInfoInput->getPositionY());
	nodeCard->addChild(nodeInput);

	ui::Scale9Sprite* bgInput = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgInput->setContentSize(Size(540, 200));
	bgInput->setOpacity(100);
	nodeInput->addChild(bgInput);

	Label* lbseri = Label::create(Utils::getSingleton().getStringForKey("so_seri"), "fonts/arial.ttf", 25);
	lbseri->setAnchorPoint(Vec2(0, .5f));
	lbseri->setPosition(-240, 60);
	lbseri->setColor(Color3B::BLACK);
	nodeInput->addChild(lbseri);

	Label* lbcode = Label::create(Utils::getSingleton().getStringForKey("ma_the"), "fonts/arial.ttf", 25);
	lbcode->setAnchorPoint(Vec2(0, .5f));
	lbcode->setPosition(-240, 0);
	lbcode->setColor(Color3B::BLACK);
	nodeInput->addChild(lbcode);

	tfSeri->setPosition(Vec2(60, 60));
	tfSeri->setFontName("Arial");
	tfSeri->setFontSize(25);
	tfSeri->setFontColor(Color3B::BLACK);
	tfSeri->setMaxLength(24);
	tfSeri->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfSeri->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfSeri->setInputMode(ui::EditBox::InputMode::NUMERIC);
	tfSeri->setDelegate(this);
	nodeInput->addChild(tfSeri);

	tfCode->setPosition(Vec2(60, 0));
	tfCode->setFontName("Arial");
	tfCode->setFontSize(25);
	tfCode->setFontColor(Color3B::BLACK);
	tfCode->setMaxLength(24);
	tfCode->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfCode->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfCode->setInputMode(ui::EditBox::InputMode::NUMERIC);
	tfCode->setDelegate(this);
	nodeInput->addChild(tfCode);

	/*ui::Button* btnMoney = ui::Button::create("popup/box.png");
	btnMoney->setTitleText("       " + Utils::getSingleton().getStringForKey("quan"));
	btnMoney->setTitleFontName("fonts/arialbd.ttf");
	btnMoney->setTitleFontSize(30);
	btnMoney->setTitleColor(Color3B::YELLOW);
	btnMoney->setPosition(Vec2(-30, -65));
	btnMoney->setContentSize(Size(190, 55));
	btnMoney->setScale9Enabled(true);
	btnMoney->setBright(false);
	btnMoney->setTag(1);
	nodeInput->addChild(btnMoney);

	Sprite* spMoney = Sprite::create("main/icon_gold.png");
	spMoney->setPosition(btnMoney->getPosition() - Vec2(40, 0));
	nodeInput->addChild(spMoney);

	addTouchEventListener(btnMoney, [=]() {
		if (btnMoney->getTag() == 0) {
			btnMoney->setTag(1);
			btnMoney->setTitleColor(Color3B::YELLOW);
			btnMoney->setTitleText("       " + Utils::getSingleton().getStringForKey("quan"));
			spMoney->initWithFile("main/icon_gold.png");
		} else {
			btnMoney->setTag(0);
			btnMoney->setTitleColor(Color3B(0, 255, 255));
			btnMoney->setTitleText("       " + Utils::getSingleton().getStringForKey("xu"));
			spMoney->initWithFile("main/icon_silver.png");
		}
	});*/

	ui::Button* btnCharge = ui::Button::create("btn_small.png", "btn_small_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnCharge->setTitleText(Utils::getSingleton().getStringForKey("nap"));
	btnCharge->setTitleFontName("fonts/azuki.ttf");
	btnCharge->setTitleColor(Color3B::BLACK);
	btnCharge->setTitleFontSize(40);
	btnCharge->setTitleDeviation(Vec2(0, 5));
	btnCharge->setContentSize(Size(150, 58));
	btnCharge->setScale9Enabled(true);
	btnCharge->setPosition(Vec2(60, -65));
	btnCharge->setScale(.9f);
	addTouchEventListener(btnCharge, [=]() {
		if (strProviders.size() == 0) return;
		string code = tfCode->getText();
		string seri = tfSeri->getText();
		if (code.length() == 0 || seri.length() == 0) return;
		int moneyType = cbQuan->isSelected() ? 2 : 1;

		if (chosenProviderCard.length() == 0) {
			if (popupChooseCard == NULL) {
				popupChooseCard = createPopupChooseProvider(Utils::getSingleton().getStringForKey("chon_the_nap"), strProviders, [=](string provider) {
					onChooseProviderCard(provider);
					SFSRequest::getSingleton().RequestChargeCard(code, seri, chosenProviderCard, moneyType);
					tfCode->setText("");
					tfSeri->setText("");
					showWaiting();
					chargingProvider = chosenProviderCard;
				});
			} else {
				resetPopupChooseProvider(popupChooseCard);
			}
			showPopup(popupChooseCard);
		} else {
			SFSRequest::getSingleton().RequestChargeCard(code, seri, chosenProviderCard, moneyType);
			tfCode->setText("");
			tfSeri->setText("");
			showWaiting();
			chargingProvider = chosenProviderCard;
		}
	});
	nodeInput->addChild(btnCharge);

	/*Label* lbTitleBtnCharge = Label::create(Utils::getSingleton().getStringForKey("nap"), "fonts/guanine.ttf", 30);
	lbTitleBtnCharge->setPosition(btnCharge->getContentSize().width / 2, btnCharge->getContentSize().height / 2 + 8);
	lbTitleBtnCharge->setColor(Color3B::BLACK);
	btnCharge->addChild(lbTitleBtnCharge);*/

	//Node SMS
	ui::Scale9Sprite* bgSms = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgSms->setPosition(0, -140);
	bgSms->setContentSize(Size(size.width, 200));
	bgSms->setOpacity(100);
	nodeSms->addChild(bgSms);

	ui::ScrollView* scrollSms = ui::ScrollView::create();
	scrollSms->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollSms->setBounceEnabled(true);
	scrollSms->setPosition(Vec2(-bgSms->getContentSize().width / 2 + 10, bgSms->getPositionY() - bgSms->getContentSize().height / 2));
	scrollSms->setContentSize(bgSms->getContentSize() - Size(20, 0));
	scrollSms->setScrollBarEnabled(false);
	scrollSms->setName("scrollsms");
	nodeSms->addChild(scrollSms);

	int width = moneys.size() * 250;
	if (width < scrollSms->getContentSize().width) {
		width = scrollSms->getContentSize().width;
	}
	scrollSms->setInnerContainerSize(Size(width, 170));
	//for (int k = 0; k < 2; k++) {
		for (int i = 0; i < moneys.size(); i++) {
			string strMoney = to_string(moneys[i]);
			string strMoney2 = to_string(moneys[i] / 2);
			string smsStr = Utils::getSingleton().replaceString(smsContent, "vnd", strMoney);
			smsStr = Utils::getSingleton().replaceString(smsStr, "uid", to_string(Utils::getSingleton().userDataMe.UserID));

			Node* itemSms = Node::create();
			itemSms->setPosition(125 + i * 250, 100);
			itemSms->setName(strMoney);
			scrollSms->addChild(itemSms);

			ui::Scale9Sprite* bgItemSms = ui::Scale9Sprite::createWithSpriteFrameName("box7.png");
			bgItemSms->setContentSize(Size(240, 170));
			itemSms->addChild(bgItemSms);

			Label* lbItemSms1 = Label::create("SMS " + strMoney + "k = ", "fonts/arial.ttf", 22);
			lbItemSms1->setAnchorPoint(Vec2(1, .5f));
			lbItemSms1->setPosition(5, 60);
			lbItemSms1->setColor(Color3B::BLACK);
			itemSms->addChild(lbItemSms1);

			Label* lbItemSms2 = Label::create(strMoney2 + "k Quan", "fonts/arial.ttf", 22);
			lbItemSms2->setAnchorPoint(Vec2(0, .5f));
			lbItemSms2->setColor(Color3B::YELLOW);
			lbItemSms2->setPosition(10, 60);
			lbItemSms2->setName("lbsmsmoney");
			itemSms->addChild(lbItemSms2);

			Label* lbItemSms3 = Label::create(smsStr, "fonts/arial.ttf", 22);
			lbItemSms3->setPosition(0, 30);
			lbItemSms3->setName("lbsmscontent");
			lbItemSms3->setColor(Color3B::BLACK);
			itemSms->addChild(lbItemSms3);

			Label* lbItemSms4 = Label::create(smsTarget, "fonts/arial.ttf", 22);
			lbItemSms4->setPosition(0, 0);
			lbItemSms4->setName("lbsmstarget");
			lbItemSms4->setColor(Color3B::BLACK);
			itemSms->addChild(lbItemSms4);

			ui::Button* btnItemSms = ui::Button::create("btn_small.png", "btn_small_clicked.png", "", ui::Widget::TextureResType::PLIST);
			btnItemSms->setTitleText(Utils::getSingleton().getStringForKey("nap"));
			btnItemSms->setTitleFontName("fonts/azuki.ttf");
			btnItemSms->setTitleColor(Color3B::BLACK);
			btnItemSms->setTitleFontSize(40);
			btnItemSms->setTitleDeviation(Vec2(0, 5));
			btnItemSms->setContentSize(Size(150, 58));
			btnItemSms->setScale9Enabled(true);
			btnItemSms->setPosition(Vec2(0, -50));
			btnItemSms->setScale(.9f);
			addTouchEventListener(btnItemSms, [=]() {
				if (strProviders.size() == 0) return;
				std::vector<std::string> smsProviders;
				for (int j = 1; j <= strProviders.size(); j++) {
					if (strProviders[j - 1].compare("viettel") == 0
						|| strProviders[j - 1].compare("mobifone") == 0
						|| strProviders[j - 1].compare("vinaphone") == 0) {
						smsProviders.push_back(strProviders[j - 1]);
					}
				}
				if (chosenProviderSms.length() == 0) {
					if (popupChooseSms == NULL) {
						popupChooseSms = createPopupChooseProvider(Utils::getSingleton().getStringForKey("chon_mang_sms"), smsProviders, [=](string provider) {
							onChooseProviderSms(provider);
							Utils::getSingleton().openSMS(lbItemSms4->getString(), lbItemSms3->getString());
							Tracker::getSingleton().trackPurchaseSuccess("ClickSMS", chosenProviderSms, "VND", moneys[i] * 1000);
						});
					} else {
						resetPopupChooseProvider(popupChooseSms);
					}
					showPopup(popupChooseSms);
				} else {
					Utils::getSingleton().openSMS(lbItemSms4->getString(), lbItemSms3->getString());
					Tracker::getSingleton().trackPurchaseSuccess("ClickSMS", chosenProviderSms, "VND", moneys[i] * 1000);
				}
			});
			itemSms->addChild(btnItemSms);

			/*Label* lbTitleBtnItemSms = Label::create(Utils::getSingleton().getStringForKey("nap"), "fonts/guanine.ttf", 25);
			lbTitleBtnItemSms->setPosition(btnItemSms->getContentSize().width / 2, btnItemSms->getContentSize().height / 2 + 5);
			lbTitleBtnItemSms->setColor(Color3B::BLACK);
			btnItemSms->addChild(lbTitleBtnItemSms);*/
		}
	//}
    
    //Nap CH
    vector<ProductData> products = Utils::getSingleton().products;
    Size storeSize = Size(780, 260);
    
    ui::ScrollView* scrollStore = ui::ScrollView::create();
    scrollStore->setDirection(ui::ScrollView::Direction::HORIZONTAL);
    scrollStore->setBounceEnabled(true);
    scrollStore->setPosition(Vec2(-storeSize.width/2, -storeSize.height/2));
    scrollStore->setContentSize(storeSize);
    scrollStore->setScrollBarEnabled(false);
    scrollStore->setName("scrollstore");
    nodeStore->addChild(scrollStore);

    int storeWidth = products.size() * 240;
    if(storeWidth < storeSize.width){
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
        btn->setPosition(Vec2(130 + i * 260, storeSize.height/2));
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
        
        Sprite* spCoin = Sprite::createWithSpriteFrameName(pmE ? "icon_gold.png" : "icon_silver.png");
        spCoin->setScale(.55f);
        btn->addChild(spCoin);
        
        Label* lb1 = Label::create(strValue, "fonts/arialbd.ttf", 30);
        lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, btn->getContentSize().height / 2 - 75);
        lb1->setColor(Color3B::YELLOW);
        btn->addChild(lb1);
        
        Label* lb2 = Label::create(strCost, "fonts/arialbd.ttf", 30);
        lb2->setWidth(175);
        lb2->setHeight(30);
        lb2->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 - 110);
        lb2->setColor(Color3B::BLACK);
        lb2->setHorizontalAlignment(TextHAlignment::CENTER);
        btn->addChild(lb2);
        
        spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
                            + spCoin->getContentSize().width * spCoin->getScale() / 2 + 5, lb1->getPositionY());
    }

    if(!pmE){
        nodeCard->setVisible(false);
        nodeStore->setVisible(true);
        nodeStore->setPosition(0, 10);
        for(int i=1;i<= strProviders.size();i++){
            popupCharge->getChildByName("btn" + to_string(i))->setVisible(false);
            //popupCharge->getChildByName("providerimg" + to_string(i))->setVisible(false);
        }
		for (int i = 0; i < texts.size(); i++) {
			popupCharge->getChildByTag(10 + i)->setVisible(false);
		}
    }
	if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS) {
		//popupCharge->getChildByTag(12)->setVisible(false);
	}
}

void MainScene::initPopupGuide()
{
	popupGuide = createPopup("", true, true);
	if (!popupGuide) return;
	popupGuide->setTag(0);

	Size size = Size(900, 436);
	Size scrollSize = size - Size(30, 20);

	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgScroll->setContentSize(size);
	bgScroll->setPosition(0, -20);
	popupGuide->addChild(bgScroll);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2) + bgScroll->getPosition());
	scroll->setContentSize(scrollSize);
	scroll->setScrollBarEnabled(false);
	popupGuide->addChild(scroll);

	cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
	Label* lb = Label::create(plist["tutorial_1"].asString(), "fonts/arial.ttf", 22);
	lb->setAnchorPoint(Vec2(0, 1));
	lb->setColor(Color3B::BLACK);
	lb->setName("lbcontent");
	lb->setWidth(scrollSize.width);
	scroll->addChild(lb);

	int height = lb->getContentSize().height;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	lb->setPosition(0, height);
	scroll->setInnerContainerSize(Size(scrollSize.width, height));

	vector<string> texts = { "noi_quy" , "cuoc_u", "tinh_loi", "tinh_diem" };
	int x = -320;
	int y = 219;
	Size btnSize = Size(205, 50);
	for (int i = 0; i < 4; i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box4.png" : "box2.png", "box4.png", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/arialbd.ttf");
		btn->setTitleFontSize(25);
		btn->setTitleColor(Color3B::BLACK);
		btn->setContentSize(btnSize);
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(i); 
		addTouchEventListener(btn, [=]() {
			ui::Button* btn1 = (ui::Button*)popupGuide->getChildByTag(popupGuide->getTag());
			cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
			string content = plist["tutorial_" + to_string(btn->getTag() + 1)].asString();

			btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
			lb->setString(content);
			popupGuide->setTag(btn->getTag());

			int height = lb->getContentSize().height;
			if (height < scroll->getContentSize().height) {
				height = scroll->getContentSize().height;
			}
			lb->setPosition(0, height);
			scroll->setInnerContainerSize(Size(scrollSize.width, height));
		});
		popupGuide->addChild(btn);
		x += btnSize.width;
	}
}

void MainScene::initPopupMail()
{
	popupMail = createPopup(Utils::getSingleton().getStringForKey("hom_thu"), true, true);
	if (!popupMail) return;

	Sprite* bg = (Sprite*)popupMail->getChildByName("spbg");

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgContent->setContentSize(Size(920, 460));
	bgContent->setPosition(0, -18);
	popupMail->addChild(bgContent);

	Size size = bgContent->getContentSize();

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-size.width / 2 + 5, -size.height / 2 + 50) + bgContent->getPosition());
	scroll->setContentSize(Size(size.width - 10, size.height - 95));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	scroll->setTag(1);
	popupMail->addChild(scroll);

	vector<int> posX = { -420, -290, 0, 340 };
	vector<int> widths = { 50, 100, 420, 120 };
	vector<string> historyTitles = { "STT", Utils::getSingleton().getStringForKey("ngay"), 
		Utils::getSingleton().getStringForKey("thong_tin"), Utils::getSingleton().getStringForKey("nguoi_gui") };
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lb = Label::create(historyTitles[i], "fonts/arialbd.ttf", 25);
		lb->setColor(Color3B::BLACK);
		lb->setPosition(posX[i], bgContent->getPositionY() + bgContent->getContentSize().height / 2 - 25);
		popupMail->addChild(lb);
	}

	Node* nodeDetail = Node::create();
	nodeDetail->setPosition(0, bgContent->getPositionY() - 20);
	nodeDetail->setVisible(false);
	nodeDetail->setName("nodedetail");
	popupMail->addChild(nodeDetail);

	/*ui::Scale9Sprite* bgDetail = ui::Scale9Sprite::createWithSpriteFrameName("box1.png");
	bgDetail->setContentSize(Size(860, 380));
	nodeDetail->addChild(bgDetail);*/

	ui::Scale9Sprite* bgDetail = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	bgDetail->setContentSize(size - Size(10, 50));
	bgDetail->setColor(Color3B(155, 155, 155));
	nodeDetail->addChild(bgDetail);

	for (int i = 0; i < posX.size(); i++) {
		Label* lbDetail = Label::create("", "fonts/arial.ttf", 22);
		lbDetail->setWidth(widths[i]);
		lbDetail->setAnchorPoint(Vec2(.5f, 1));
		lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
		lbDetail->setPosition(posX[i], bgDetail->getContentSize().height / 2 - 15);
		lbDetail->setTag(i);
		lbDetail->setColor(Color3B::BLACK);
		nodeDetail->addChild(lbDetail);
	}

	ui::Button* btnCloseDetail = ui::Button::create("btn_dong2.png", "btn_dong2.png", "", ui::Widget::TextureResType::PLIST);
	btnCloseDetail->setPosition(Vec2(bgDetail->getContentSize().width / 2 - 50, -bgDetail->getContentSize().height / 2 + 50));
	//btnCloseDetail->setScale(.7f);
	addTouchEventListener(btnCloseDetail, [=]() {
		nodeDetail->setVisible(false);
	});
	nodeDetail->addChild(btnCloseDetail);

	addBtnChoosePage(0, -222, popupMail, [=](int page) {
		SFSRequest::getSingleton().RequestListMail(page);
		//onListMailDataResponse(vector<MailData>());
	});
}

void MainScene::initPopupNews()
{
	popupNews = createPopup("", true, true);
	if (!popupNews) return;
	popupNews->setTag(1);

	Size size = Size(920, 442);
	Size scrollSize = size - Size(30, 60);

	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgScroll->setContentSize(size);
	bgScroll->setPosition(0, -26);
	popupNews->addChild(bgScroll);

	ui::ScrollView* scrollContent = ui::ScrollView::create();
	scrollContent->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollContent->setBounceEnabled(true);
	scrollContent->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2 + 20) + bgScroll->getPosition());
	scrollContent->setContentSize(scrollSize);
	scrollContent->setScrollBarEnabled(false);
	scrollContent->setName("scrollcontent");
	popupNews->addChild(scrollContent);

	Label* lbContent = Label::create("", "fonts/arial.ttf", 22);
	lbContent->setWidth(scrollContent->getContentSize().width);
	lbContent->setAnchorPoint(Vec2(0, 1));
	lbContent->setColor(Color3B::BLACK);
	lbContent->setName("lbcontent");
	scrollContent->addChild(lbContent);

	ui::ScrollView* scrollTitle = ui::ScrollView::create();
	scrollTitle->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollTitle->setBounceEnabled(true);
	scrollTitle->setPosition(Vec2(-size.width / 2 + 15, size.height / 2 - 4) + bgScroll->getPosition());
	scrollTitle->setContentSize(Size(size.width - 30, 50));
	scrollTitle->setScrollBarEnabled(false);
	scrollTitle->setName("scrolltitle");
	scrollTitle->setTag(0);
	popupNews->addChild(scrollTitle);

	addBtnChoosePage(0, -222, popupNews, [=](int page) {
		scrollTitle->setTag(0);
		SFSRequest::getSingleton().RequestNews(page);
		//onNewsDataResponse(vector<NewsData>());
	});
}

void MainScene::initPopupShop()
{
	popupShop = createPopup("", true, true);
	if (!popupShop) return;
	popupShop->setTag(0);

	Size size = Size(900, 446);
	Size scrollSize = size - Size(30, 20);

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::createWithSpriteFrameName("box6.png");
	bgContent->setContentSize(size);
	bgContent->setPosition(0, -25);
	popupShop->addChild(bgContent);

	Node* nodeCard = Node::create();
	nodeCard->setTag(10);
	nodeCard->setVisible(true);
	popupShop->addChild(nodeCard);

	Node* nodeItem = Node::create();
	nodeItem->setTag(11);
	nodeItem->setVisible(false);
	popupShop->addChild(nodeItem);

	Node* nodeHistory = Node::create();
	nodeHistory->setTag(12);
	nodeHistory->setVisible(false);
	nodeHistory->setName("nodehistory");
	popupShop->addChild(nodeHistory);

	int x = -320;
	int y = 219;
	Size btnSize = Size(205, 50);
	vector<string> texts = { "the_cao" , "vat_pham", "lich_su" };
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box4.png" : "box2.png", "box4.png", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/arialbd.ttf");
		btn->setTitleFontSize(30);
		btn->setTitleColor(Color3B::BLACK);
		btn->setContentSize(btnSize);
		btn->setPosition(Vec2(x, y));
		//btn->setOpacity(200);
		btn->setScale9Enabled(true);
		//btn->setBright(false);
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			if (btn->getTag() == popupShop->getTag()) return;
			Node* node = popupShop->getChildByTag(btn->getTag() + 10);
			Node* node1 = popupShop->getChildByTag(popupShop->getTag() + 10);
			ui::Button* btn1 = (ui::Button*)popupShop->getChildByTag(popupShop->getTag());

			node1->setVisible(false);
			node->setVisible(true);
			btn1->loadTextureNormal("box2.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box4.png", ui::Widget::TextureResType::PLIST);
			popupShop->setTag(btn->getTag());

			if (i == 2) {
				SFSRequest::getSingleton().RequestShopHistory(1);
			}
		});
		popupShop->addChild(btn);

		/*Label* lbbtn = Label::create(Utils::getSingleton().getStringForKey(texts[i]), "fonts/guanine.ttf", 30);
		lbbtn->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
		btn->addChild(lbbtn);*/

		x += btnSize.width;
	}

	ui::ScrollView* scrollCard = ui::ScrollView::create();
	//scrollCard->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollCard->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollCard->setBounceEnabled(true);
	scrollCard->setPosition(Vec2(bgContent->getPositionX() - size.width / 2, bgContent->getPositionY() - size.height / 2 + 10));
	scrollCard->setContentSize(Size(size.width, size.height - 20));
	scrollCard->setScrollBarEnabled(false);
	scrollCard->setName("scrollcard");
	nodeCard->addChild(scrollCard);

	ui::ScrollView* scrollItem = ui::ScrollView::create();
	scrollItem->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollItem->setBounceEnabled(true);
	scrollItem->setPosition(Vec2(bgContent->getPositionX() - size.width / 2, bgContent->getPositionY() - size.height / 2 + 10));
	scrollItem->setContentSize(Size(size.width, size.height - 20));
	scrollItem->setScrollBarEnabled(false);
	scrollItem->setName("scrollitem");
	nodeItem->addChild(scrollItem);

	int py = bgContent->getPositionY() + size.height / 2 - 25;
	int px = -390;
	vector<int> widths = { 120, 210, 210, 210, 150 };
	vector<string> historyTitles = { "ma_dt", "san_pham_doi", "thoi_gian_doi", "thoi_gian_nhan", "trang_thai" };
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lb = Label::create(Utils::getSingleton().getStringForKey(historyTitles[i]), "fonts/arialbd.ttf", 22);
		lb->setColor(Color3B::BLACK);
		lb->setPosition(px, py);
		nodeHistory->addChild(lb);

		ui::Scale9Sprite* bgTitle = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
		bgTitle->setContentSize(Size(widths[i], 60));
		bgTitle->setPosition(lb->getContentSize().width / 2, lb->getContentSize().height / 2);
		lb->addChild(bgTitle, -1);

		if (i < historyTitles.size() - 1) {
			px += widths[i] / 2 + widths[i + 1] / 2;
		}
	}
	
	ui::ScrollView* scrollHistory = ui::ScrollView::create();
	scrollHistory->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollHistory->setBounceEnabled(true);
	scrollHistory->setPosition(Vec2(bgContent->getPositionX() - size.width / 2, bgContent->getPositionY() - size.height / 2 + 45));
	scrollHistory->setContentSize(Size(size.width, size.height - 90));
	scrollHistory->setScrollBarEnabled(false);
	scrollHistory->setName("scrollhistory");
	nodeHistory->addChild(scrollHistory);

	addBtnChoosePage(0, -222, nodeHistory, [=](int page) {
		SFSRequest::getSingleton().RequestShopHistory(page);
		//onShopHistoryDataResponse(vector<ShopHistoryData>());
	});
}
void MainScene::initWebView()
{
	nodeWebview = Node::create();
	nodeWebview->setPosition(winSize.width/2, winSize.height/2);
	nodeWebview->setVisible(false);
	mLayer->addChild(nodeWebview, constant::ZORDER_POPUP);
	autoScaleNode(nodeWebview);

	ui::Scale9Sprite* webSplash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	webSplash->setContentSize(Size(1500, 1000));
	webSplash->setColor(Color3B::BLACK);
	webSplash->setOpacity(150);
	nodeWebview->addChild(webSplash);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	Size visibleSize = Director::getInstance()->getOpenGLView()->getFrameSize();
	Size wSize = Size(visibleSize.width, visibleSize.height - 200);
	auto webView = experimental::ui::WebView::create();
	webView->setContentSize(wSize);
	webView->setScalesPageToFit(true);
	webView->setName("webview");
	webView->setVisible(false);
	nodeWebview->addChild(webView);
#endif

	ui::CheckBox* checkbox = ui::CheckBox::create();
	checkbox->loadTextureBackGround("box0.png", ui::Widget::TextureResType::PLIST);
	checkbox->loadTextureFrontCross("check.png", ui::Widget::TextureResType::PLIST);
	checkbox->setPosition(Vec2(-500, -320));
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

	Label* lb = Label::create(Utils::getSingleton().getStringForKey("khong_hien_lai"), "fonts/arial.ttf", 30);
	lb->setPosition(checkbox->getPosition() + Vec2(40, 0));
	lb->setAnchorPoint(Vec2(0, .5f));
	lb->setColor(Color3B::WHITE);
	lb->setVisible(false);
	nodeWebview->addChild(lb);

	ui::Button* btnClose = ui::Button::create("btn_dong2.png", "btn_dong2.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(winSize.width / 2 - 35, winSize.height / 2 - 50));
	addTouchEventListener(btnClose, [=]() {
		//hideSplash();
		nodeWebview->setVisible(false);
		//nodeWebview->removeChildByName("webview");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		webView->setVisible(false);
#endif
	});
	nodeWebview->addChild(btnClose);

	Label* lbDong = Label::createWithTTF(Utils::getSingleton().getStringForKey("close"), "fonts/arial.ttf", 40);
	lbDong->setPosition(btnClose->getPosition() - Vec2(45, 0));
	lbDong->setAnchorPoint(Vec2(1, .5f));
	lbDong->setColor(Color3B::YELLOW);
	nodeWebview->addChild(lbDong);
}

void MainScene::initPopupGiftcode()
{
	popupGiftcode = createPopup("Giftcode", false, true);
	if (!popupGiftcode) return;

	ui::EditBox* tfGiftcode = ui::EditBox::create(Size(340, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	tfGiftcode->setPosition(Vec2(0, -20));
	tfGiftcode->setFontName("Arial");
	tfGiftcode->setFontSize(25);
	tfGiftcode->setFontColor(Color3B::WHITE);
	tfGiftcode->setMaxLength(20);
	tfGiftcode->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfGiftcode->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfGiftcode->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfGiftcode->setDelegate(this);
	popupGiftcode->addChild(tfGiftcode);

	Label* lb = Label::create(Utils::getSingleton().getStringForKey("nhap_giftcode"), "fonts/arial.ttf", 30);
	lb->setColor(Color3B::BLACK);
	lb->setPosition(0, 60);
	popupGiftcode->addChild(lb);

	Sprite* bg = (Sprite*)popupGiftcode->getChildByName("spbg");

	ui::Button* btnClose = (ui::Button*)popupGiftcode->getChildByName("btnclose");
	btnClose->setPositionX(110);

	ui::Button* btnSubmit = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnSubmit->setPosition(Vec2(-110, -bg->getContentSize().height / 2 + 10));
	addTouchEventListener(btnSubmit, [=]() {
		string code = Utils::getSingleton().trim(tfGiftcode->getText());
		if (code.length() > 0) {
			SFSRequest::getSingleton().RequestGiftcode(code);
			tfGiftcode->setText("");
			//hidePopup(popupGiftcode);
		}
	});
	popupGiftcode->addChild(btnSubmit);
}

void MainScene::initPopupDisplayName()
{
	popupDisplayName = Node::create();
	popupDisplayName->setPosition(560, 350);
	popupDisplayName->setVisible(false);
	popupDisplayName->setName("popupdisplayname");
	mLayer->addChild(popupDisplayName, constant::ZORDER_POPUP);
	//autoScaleNode(popupDisplayName);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setInsetBottom(48);
	bg->setInsetTop(48);
	bg->setInsetLeft(52);
	bg->setInsetRight(52);
	bg->setContentSize(Size(800, 400));
	bg->setScale(.8f);
	popupDisplayName->addChild(bg);

	ui::EditBox* tfDisplayName = ui::EditBox::create(Size(340, 55), "box.png", ui::Widget::TextureResType::PLIST);
	tfDisplayName->setPosition(Vec2(0, 0));
	tfDisplayName->setFontName("Arial");
	tfDisplayName->setFontSize(25);
	tfDisplayName->setFontColor(Color3B::WHITE);
	tfDisplayName->setMaxLength(16);
	tfDisplayName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfDisplayName->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfDisplayName->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfDisplayName->setDelegate(this);
	popupDisplayName->addChild(tfDisplayName);

	Label* lb = Label::create(Utils::getSingleton().getStringForKey("nhap_ten_hien_thi"), "fonts/arial.ttf", 30);
	lb->setPosition(0, 60);
	popupDisplayName->addChild(lb);

	ui::Button* btnSubmit = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnSubmit->setPosition(Vec2(0, -95));
	addTouchEventListener(btnSubmit, [=]() {
		tmpDisplayName = Utils::getSingleton().trim(tfDisplayName->getText());
		if (Utils::getSingleton().isDisplayNameValid(tmpDisplayName)) {
			SFSRequest::getSingleton().RequestUpdateDisplayName(tmpDisplayName);
		} else {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_displayname_format"));
		}
	});
	popupDisplayName->addChild(btnSubmit);
}

void MainScene::showPopupMail()
{
	if (popupMail == nullptr) {
		initPopupMail();
		showWaiting();
		isWaitPopupMail = true;
		SFSRequest::getSingleton().RequestListMail(1);
		return;
	}
	showPopup(popupMail);
	lbNewMail->getParent()->setVisible(false);

	/*popupMail->getChildByName("nodepage")->setTag(1);
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = (ui::Button*)popupMail->getChildByTag(1000 + i);
		btn->setTitleText(to_string(i));
		if (i == 1) {
			btn->setColor(pageColor2);
		} else {
			btn->setColor(pageColor1);
		}
	}*/
}

void MainScene::showPopupNews()
{
	if (popupNews == nullptr) {
		initPopupNews();
		showWaiting();
		isWaitPopupNews = true;
		SFSRequest::getSingleton().RequestNews(0);
		return;
	} else {
		showPopup(popupNews);
	}

	/*ui::ScrollView* scrollTitle = (ui::ScrollView*)popupNews->getChildByName("scrolltitle");
	ui::ScrollView* scrollContent = (ui::ScrollView*)popupNews->getChildByName("scrollcontent");
	Label* lbContent = (Label*)scrollContent->getChildByName("lbcontent");
	lbContent->setString("");
	scrollTitle->setTag(0);
	int count = scrollTitle->getChildrenCount();
	for (int i = 0; i < count; i++) {
		scrollTitle->getChildByTag(i)->setVisible(false);
	}

	popupNews->getChildByName("nodepage")->setTag(1);
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = (ui::Button*)popupNews->getChildByTag(1000 + i);
		btn->setTitleText(to_string(i));
		if (i == 1) {
			btn->setColor(pageColor2);
		} else {
			btn->setColor(pageColor1);
		}
	}*/
}

void MainScene::showWebView(std::string url)
{
	if (nodeWebview == NULL) {
		initWebView();
	}
	nodeWebview->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	experimental::ui::WebView* webView = (experimental::ui::WebView*)nodeWebview->getChildByName("webview");
	webView->loadURL(url);
	webView->setVisible(true);
#endif
}

void MainScene::checkProviderToChargeSms()
{
	int btnIndex = -1;
	Node* scrollProvider = popupCharge->getChildByName("scrollprovider");
	for (int i = 1; i <= strProviders.size(); i++) {
		string btnName = strProviders[i - 1];
		Node* btni = scrollProvider->getChildByName(btnName);
		if (btnName.compare("viettel") != 0
			&& btnName.compare("mobifone") != 0 && btnName.compare("vinaphone") != 0) {
			if (btni->getTag() == 1) {
				chosenProviderSms = "";
			}
			btni->setVisible(false);
			btni->setTag(0);
			btni->getChildByTag(1)->setVisible(false);
		}
	}
}

void MainScene::updateChargeRateCard(bool isQuan)
{
	ui::ScrollView* scroll = (ui::ScrollView*)(popupCharge->getChildByName("nodecard")->getChildByName("nodecardinfo")->getChildByName("scrollinfo"));
	for (int i = 0; i < moneys.size(); i++) {
		string type = isQuan ? Utils::getSingleton().getStringForKey("quan") : Utils::getSingleton().getStringForKey("xu");
		Label* lb = (Label*)scroll->getChildByTag(i * 2 + 1);
		lb->setString(to_string(isQuan ? moneys[i] : moneyxs[i]) + "k " + type);
		lb->setColor(isQuan ? Color3B::YELLOW : Color3B(0, 255, 255));
	}
}

void MainScene::updateSmsInfo(bool isQuan)
{
	Node* scrollProvider = popupCharge->getChildByName("scrollprovider");
	string smsct = "";
	if (chosenProviderSms.compare("viettel") == 0) {
		smsct = Utils::getSingleton().gameConfig.smsVT;
	} else if (chosenProviderSms.compare("mobifone") == 0) {
		smsct = Utils::getSingleton().gameConfig.smsVMS;
	} else {
		smsct = Utils::getSingleton().gameConfig.smsVNP;
	}
	//int strid = smsct.find_last_of(' ');
	string smstg = Utils::getSingleton().dynamicConfig.SmsNap;// smsct.substr(strid + 1, smsct.length() - strid);
	//smsct = smsct.substr(0, strid);
	smsct = Utils::getSingleton().replaceString(smsct, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
	if (!isQuan) smsct = Utils::getSingleton().replaceString(smsct, " T", " A");

	Node* nodeSms = popupCharge->getChildByName("nodesms");
	ui::ScrollView* scroll = (ui::ScrollView*)nodeSms->getChildByName("scrollsms");
	for (int i = 0; i < moneys.size(); i++) {
		string strMoney = to_string(moneys[i]);
		string smsStr = Utils::getSingleton().replaceString(smsct, "vnd", strMoney);

		Node* node = scroll->getChildByName(strMoney);
		Label* lbMoney = (Label*)node->getChildByName("lbsmsmoney");
		Label* lbContent = (Label*)node->getChildByName("lbsmscontent");
		Label* lbTarget = (Label*)node->getChildByName("lbsmstarget");
		lbMoney->setString(to_string((isQuan ? moneys[i] : moneyxs[i]) / 2) + "k " + (isQuan ? "Quan" : "Xu"));
		lbMoney->setColor(isQuan ? Color3B::YELLOW : Color3B(0, 255, 255));
		lbContent->setString(smsStr);
		lbTarget->setString(smstg);
	}
}

void MainScene::onChooseProviderCard(std::string provider)
{
	if (chosenProviderCard.compare(provider) == 0) return;
	Node* scrollProvider = popupCharge->getChildByName("scrollprovider");
	Node *lastBtn = nullptr, *curBtn;
	for (int i = 1; i <= strProviders.size(); i++) {
		std::string btnName = strProviders[i - 1];
		if (chosenProviderCard.compare(btnName) == 0) {
			lastBtn = scrollProvider->getChildByName(btnName);
		}
		if (provider.compare(btnName) == 0) {
			curBtn = scrollProvider->getChildByName(btnName);
		}
	}
	chosenProviderCard = provider;
	if (lastBtn != NULL) {
		lastBtn->setTag(0);
		lastBtn->setColor(Color3B::GRAY);
	}
	curBtn->setTag(1);
	curBtn->setColor(Color3B::WHITE);
}

void MainScene::onChooseProviderSms(std::string provider)
{
	if (chosenProviderSms.compare(provider) == 0) return;
	Node* scrollProvider = popupCharge->getChildByName("scrollprovider");
	Node *lastBtn = nullptr, *curBtn;
	for (int i = 1; i <= strProviders.size(); i++) {
		std::string btnName = strProviders[i - 1];
		if (chosenProviderSms.compare(btnName) == 0) {
			lastBtn = scrollProvider->getChildByName(btnName);
		}
		if (provider.compare(btnName) == 0) {
			curBtn = scrollProvider->getChildByName(btnName);
		}
	}
	chosenProviderSms = provider;
	if (lastBtn != NULL) {
		lastBtn->setTag(0);
		lastBtn->setColor(Color3B::GRAY);
	}
	curBtn->setTag(1);
	curBtn->setColor(Color3B::WHITE);

	Node* nodeSms = popupCharge->getChildByName("nodesms");
	if (nodeSms->isVisible()) {
		Node* nodeMoneyType = popupCharge->getChildByName("nodemoneytype");
		ui::CheckBox* cbQuan = (ui::CheckBox*)nodeMoneyType->getChildByTag(0);
		updateSmsInfo(cbQuan->isSelected());
	}
}