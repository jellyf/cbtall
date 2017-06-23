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
	isChargeQuan = Utils::getSingleton().ispmE();
	bool pmE = Utils::getSingleton().ispmE();
	currentMoneyType = Utils::getSingleton().moneyType;

	std::vector<Vec2> vecMenuPos;
	int numbOfMenuBtns = 9;
	float dx = winSize.width / numbOfMenuBtns;
	for (int i = 0; i < numbOfMenuBtns; i++) {
		vecMenuPos.push_back(Vec2(dx / 2 + i * dx, 100));
	}

	std::vector<Vec2> vecZonePos;
	int numbOfZoneBtns = 5;
	dx = winSize.width / numbOfZoneBtns;
	for (int i = 0; i < numbOfZoneBtns; i++) {
		vecZonePos.push_back(Vec2(dx / 2 + i * dx, winSize.height / 2));
	}

	initHeaderWithInfos();

	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGB565);
	Texture2D* bgTexture = TextureCache::getInstance()->addImage("bg.jpg");
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);

	Sprite* bg = Sprite::createWithTexture(bgTexture);
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	addChild(bg);

	btnEvent = ui::Button::create("icon_event.png", "", "", ui::Widget::TextureResType::PLIST);
	btnEvent->setPosition(vecMenuPos[0]);
	addTouchEventListener(btnEvent, [=]() {
		if (isEventReady) {
			Utils::getSingleton().hasShowEventPopup = true;
			showWebView(Utils::getSingleton().dynamicConfig.PopupUrl);
		} else {
			showPopupNotice(Utils::getSingleton().getStringForKey("hien_chua_co_su_kien"), [=]() {});
		}
	});
	mLayer->addChild(btnEvent);
	autoScaleNode(btnEvent);

	Label* lbEvent = Label::createWithTTF(Utils::getSingleton().getStringForKey("su_kien"), "fonts/myriad.ttf", 30);
	lbEvent->setPosition(btnEvent->getContentSize().width/2, 0);
	btnEvent->addChild(lbEvent);

	bool canInvite = Utils::getSingleton().gameConfig.invite;
	ui::Button* btnFBFriends = ui::Button::create("fb_friends.png", "", "", ui::Widget::TextureResType::PLIST);
	btnFBFriends->setPosition(vecMenuPos[1]);
	btnFBFriends->setVisible(pmE && canInvite);
	addTouchEventListener(btnFBFriends, [=]() {
		Utils::getSingleton().inviteFacebookFriends();
	});
	mLayer->addChild(btnFBFriends);
	autoScaleNode(btnFBFriends);

	Label* lbFBFriends = Label::createWithTTF(Utils::getSingleton().getStringForKey("moi_ban"), "fonts/myriad.ttf", 30);
	lbFBFriends->setPosition(btnFBFriends->getContentSize().width / 2, 0);
	btnFBFriends->addChild(lbFBFriends);

	ui::Button* btnGuide = ui::Button::create("icon_guide.png", "", "", ui::Widget::TextureResType::PLIST);
	btnGuide->setPosition(vecMenuPos[2]);
	addTouchEventListener(btnGuide, [=]() {
		if (popupGuide == nullptr) {
			initPopupGuide();
		}
		showPopup(popupGuide);
	});
	mLayer->addChild(btnGuide);
	autoScaleNode(btnGuide);

	Label* lbGuide = Label::createWithTTF(Utils::getSingleton().getStringForKey("huong_dan"), "fonts/myriad.ttf", 30);
	lbGuide->setPosition(btnGuide->getContentSize().width / 2, 0);
	btnGuide->addChild(lbGuide);

	ui::Button* btnCharge = ui::Button::create("icon_charge.png", "", "", ui::Widget::TextureResType::PLIST);
	btnCharge->setPosition(vecMenuPos[3]);
	addTouchEventListener(btnCharge, [=]() {
		if (pmE) {
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

	Label* lbCharge = Label::createWithTTF(Utils::getSingleton().getStringForKey("nap_quan"), "fonts/myriad.ttf", 30);
	lbCharge->setPosition(btnCharge->getContentSize().width / 2, 0);
	btnCharge->addChild(lbCharge);

	ui::Button* btnShop = ui::Button::create("icon_shop.png", "", "", ui::Widget::TextureResType::PLIST);
	btnShop->setPosition(vecMenuPos[4]);
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

	Label* lbShop = Label::createWithTTF(Utils::getSingleton().getStringForKey("cua_hang"), "fonts/myriad.ttf", 30);
	lbShop->setPosition(btnShop->getContentSize().width / 2, 0);
	btnShop->addChild(lbShop);

	ui::Button* btnNews = ui::Button::create("icon_news.png", "", "", ui::Widget::TextureResType::PLIST);
	btnNews->setPosition(vecMenuPos[5]);
	addTouchEventListener(btnNews, [=]() {
		showPopupNews();
		/*cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("configs.xml");
		string url = plist["link_tin_tuc"].asString();
		showWebView(url);*/
	});
	mLayer->addChild(btnNews);
	autoScaleNode(btnNews);

	Label* lbNews = Label::createWithTTF(Utils::getSingleton().getStringForKey("tin_tuc"), "fonts/myriad.ttf", 30);
	lbNews->setPosition(btnNews->getContentSize().width / 2, 0);
	btnNews->addChild(lbNews);

	ui::Button* btnMail = ui::Button::create("icon_mail.png", "", "", ui::Widget::TextureResType::PLIST);
	btnMail->setPosition(vecMenuPos[6]);
	addTouchEventListener(btnMail, [=]() {
		showPopupMail();
	});
	mLayer->addChild(btnMail);
	autoScaleNode(btnMail);

	Label* lbMail = Label::createWithTTF(Utils::getSingleton().getStringForKey("hop_thu"), "fonts/myriad.ttf", 30);
	lbMail->setPosition(btnMail->getContentSize().width / 2, 0);
	btnMail->addChild(lbMail);

	Sprite* circleNewMail = Sprite::createWithSpriteFrameName("circle_red.png");
	circleNewMail->setPosition(btnMail->getPosition() + Vec2(30, 25));
	circleNewMail->setScale(.7f);
	circleNewMail->setVisible(false);
	mLayer->addChild(circleNewMail);

	lbNewMail = Label::create("", "fonts/myriad.ttf", 25);
	lbNewMail->setPosition(circleNewMail->getContentSize().width / 2 - 4, circleNewMail->getContentSize().height / 2);
	circleNewMail->addChild(lbNewMail);

	ui::Button* btnGiftcode = ui::Button::create("icon_giftcode.png", "", "", ui::Widget::TextureResType::PLIST);
	btnGiftcode->setPosition(vecMenuPos[7]);
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

	Label* lbGiftcode = Label::createWithTTF("Giftcode", "fonts/myriad.ttf", 30);
	lbGiftcode->setPosition(btnGiftcode->getContentSize().width / 2, 0);
	btnGiftcode->addChild(lbGiftcode);

	ui::Button* btnFacebook = ui::Button::create("facebook.png", "", "", ui::Widget::TextureResType::PLIST);
	btnFacebook->setPosition(vecMenuPos[8]);
	btnFacebook->setVisible(pmE);
	addTouchEventListener(btnFacebook, [=]() {
		Application::sharedApplication()->openURL(Utils::getSingleton().gameConfig.linkFb);
	});
	mLayer->addChild(btnFacebook);
	autoScaleNode(btnFacebook);

	Label* lbFacebook = Label::createWithTTF("Facebook", "fonts/myriad.ttf", 30);
	lbFacebook->setPosition(btnFacebook->getContentSize().width / 2, 0);
	btnFacebook->addChild(lbFacebook);

	ui::Button* btnNhaTranh = ui::Button::create("nha_tranh.png", "", "", ui::Widget::TextureResType::PLIST);
	btnNhaTranh->setPosition(vecZonePos[0]);
	//btnNhaTranh->setScale(.95f);
	addTouchEventListener(btnNhaTranh, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 0;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnNhaTranh);
	autoScaleNode(btnNhaTranh);

	ui::Button* btnDinhLang = ui::Button::create("dinhlang.png", "", "", ui::Widget::TextureResType::PLIST);
	btnDinhLang->setPosition(vecZonePos[1]);
	//btnDinhLang->setScale(.95f);
	addTouchEventListener(btnDinhLang, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 1;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnDinhLang);
	autoScaleNode(btnDinhLang);

	ui::Button* btnPhuChua = ui::Button::create("phuchua.png", "", "", ui::Widget::TextureResType::PLIST);
	btnPhuChua->setPosition(vecZonePos[2]);
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

	ui::Button* btnLoiDai = ui::Button::create("dtd.png", "", "", ui::Widget::TextureResType::PLIST);
	btnLoiDai->setPosition(vecZonePos[3]);
	//btnLoiDai->setScale(.95f);
	addTouchEventListener(btnLoiDai, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 3;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnLoiDai);
	autoScaleNode(btnLoiDai);

	ui::Button* btnGiaiDau = ui::Button::create("giaidau.png", "", "", ui::Widget::TextureResType::PLIST);
	btnGiaiDau->setPosition(vecZonePos[4]);
	//btnGiaiDau->setScale(.95f);
	addTouchEventListener(btnGiaiDau, [=]() {
		/*if (isWaiting) return;
		showWaiting();
		tmpZoneId = 4;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();*/
		showPopupNotice(Utils::getSingleton().getStringForKey("feature_coming_soon"), [=] {});
	});
	mLayer->addChild(btnGiaiDau);
	autoScaleNode(btnGiaiDau);

	initEventView(Vec2(0, winSize.height - 125), Size(winSize.width, 40));

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

bool MainScene::onKeyBack()
{
	bool canBack = BaseScene::onKeyBack();
	if (canBack) {
		if (nodeWebview->isVisible()) {
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
		showPopupNotice(msg, [=]() {});
	}
}

void MainScene::onErrorResponse(unsigned char code, std::string msg)
{
	BaseScene::onErrorResponse(code, msg);
	hideWaiting();
	if (code == 0 && popupShop->isVisible()) {
		showPopupNotice(msg, [=]() {
			string str = Utils::getSingleton().gameConfig.smsKH;
			int index = str.find_last_of(' ');
			string number = str.substr(index + 1, str.length() - index);
			string content = str.substr(0, index);
			content = Utils::getSingleton().replaceString(content, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
			Utils::getSingleton().openSMS(number, content);
		}, false);
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
	showPopupNotice(msg, [=]() {});
}

void MainScene::onJoinRoom(long roomId, std::string roomName)
{
	if (roomName.at(0) == 'g' && roomName.at(2) == 'b') {
		Utils::getSingleton().goToGameScene();
	}
}

void MainScene::onJoinRoomError(std::string msg)
{
	showPopupNotice(msg, [=]() {}, false);
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
	
	vector<string> strStatus = { "dat_hang", "xac_nhan", "hoan_tat", "huy", "hoan_tra" };
	ui::ScrollView* scrollHistory = (ui::ScrollView*)(popupShop->getChildByTag(12)->getChildByName("scrollhistory"));

	scrollHistory->getChildByTag(9999)->setPosition(0, -500);
	int tag = scrollHistory->getTag();
	if (tag > 0) {
		tag -= 5;
		for (int j = 0; j < 5; j++) {
			Label* lb = (Label*)scrollHistory->getChildByTag(tag + j);
			lb->setColor(Color3B::BLACK);
			lb->getChildByTag(0)->setVisible(true);
			lb->getChildByTag(1)->setVisible(false);
		}
	}
	scrollHistory->setTag(0);

	int heightHistory = list.size() * 70;
	if (heightHistory < scrollHistory->getContentSize().height) {
		heightHistory = scrollHistory->getContentSize().height;
	}
	scrollHistory->setInnerContainerSize(Size(scrollHistory->getContentSize().width, heightHistory));
	vector<int> lsize = { 100, 250, 220, 220, 150 };
	for (int i = 0; i < list.size(); i++) {
		int tag = i * 7;
		ui::Button* btn, *btnCancel;
		Label *lb1, *lb2, *lb3, *lb4, *lb5;
		vector<Label*> lbs;
		lb1 = (Label*)scrollHistory->getChildByTag(tag);
		bool isNewBtn;
		if (lb1 == nullptr) {
			lb1 = Label::create("", "fonts/myriad.ttf", 25);
			lb1->setColor(Color3B::BLACK);
			lb1->setTag(tag);
			scrollHistory->addChild(lb1);
			lbs.push_back(lb1);

			lb2 = Label::create("", "fonts/myriad.ttf", 25);
			lb2->setHorizontalAlignment(TextHAlignment::CENTER);
			lb2->setColor(Color3B::BLACK);
			lb2->setTag(tag + 1);
			lb2->setWidth(210);
			lb2->setHeight(27);
			scrollHistory->addChild(lb2);
			lbs.push_back(lb2);

			lb3 = Label::create("", "fonts/myriad.ttf", 25);
			lb3->setColor(Color3B::BLACK);
			lb3->setTag(tag + 2);
			scrollHistory->addChild(lb3);
			lbs.push_back(lb3);

			lb4 = Label::create("", "fonts/myriad.ttf", 25);
			lb4->setColor(Color3B::BLACK);
			lb4->setTag(tag + 3);
			scrollHistory->addChild(lb4);
			lbs.push_back(lb4);

			lb5 = Label::create("", "fonts/myriad.ttf", 25);
			lb5->setColor(Color3B::BLACK);
			lb5->setTag(tag + 4);
			scrollHistory->addChild(lb5);
			lbs.push_back(lb5);

			btn = ui::Button::create("white.png", "", "", ui::Widget::TextureResType::PLIST);
			btn->setContentSize(Size(scrollHistory->getContentSize().width, 60));
			btn->setScale9Enabled(true);
			btn->setOpacity(0);
			btn->setTag(tag + 5);
			scrollHistory->addChild(btn);
			isNewBtn = true;

			btnCancel = ui::Button::create("empty.png", "empty.png", "", ui::Widget::TextureResType::PLIST);
			btnCancel->setTitleText("[" + Utils::getSingleton().getStringForKey("huy") + "]");
			btnCancel->setTitleFontName("fonts/myriad.ttf");
			btnCancel->setTitleFontSize(25);
			btnCancel->setTitleColor(Color3B::RED);
			btnCancel->setTitleDeviation(Vec2(0, -5));
			btnCancel->setContentSize(Size(50, 30));
			btnCancel->setScale9Enabled(true);
			btnCancel->setTag(tag + 6);
			scrollHistory->addChild(btnCancel);

			for (int j = 0; j < 5; j++) {
				ui::Scale9Sprite* spbg1 = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
				spbg1->setContentSize(Size(lsize[j], 60));
				spbg1->setVisible(true);
				spbg1->setTag(0);
				lbs[j]->addChild(spbg1, -1);

				ui::Scale9Sprite* spbg11 = ui::Scale9Sprite::createWithSpriteFrameName("box12.png");
				spbg11->setContentSize(Size(lsize[j], 60));
				spbg11->setVisible(false);
				spbg11->setTag(1);
				lbs[j]->addChild(spbg11, -1);
			}

		} else {
			/*for (int i = 0; i < 7; i++) {
				scrollHistory->getChildByTag(tag + i)->setPositionY(heightHistory - (i == 6 ? 15 : 10));
			}*/
			lb2 = (Label*)scrollHistory->getChildByTag(tag + 1);
			lb3 = (Label*)scrollHistory->getChildByTag(tag + 2);
			lb4 = (Label*)scrollHistory->getChildByTag(tag + 3);
			lb5 = (Label*)scrollHistory->getChildByTag(tag + 4);
			btn = (ui::Button*)scrollHistory->getChildByTag(tag + 5);
			btnCancel = (ui::Button*)scrollHistory->getChildByTag(tag + 6);
			btn->setTouchEnabled(true);
			isNewBtn = false;
			lbs.push_back(lb1);
			lbs.push_back(lb2);
			lbs.push_back(lb3);
			lbs.push_back(lb4);
			lbs.push_back(lb5);
		}
		addTouchEventListener(btn, [=]() {
			int tag = scrollHistory->getTag();
			if (tag != btn->getTag()) {
				scrollHistory->getChildByTag(9999)->setPosition(btn->getPosition());
				for (int j = 0; j < 5; j++) {
					lbs[j]->setColor(Color3B::WHITE);
					lbs[j]->getChildByTag(0)->setVisible(false);
					lbs[j]->getChildByTag(1)->setVisible(true);
				}
				if (tag > 0) {
					tag -= 5;
					for (int j = 0; j < 5; j++) {
						Label* lb = (Label*)scrollHistory->getChildByTag(tag + j);
						lb->setColor(Color3B::BLACK);
						lb->getChildByTag(0)->setVisible(true);
						lb->getChildByTag(1)->setVisible(false);
					}
				}
				scrollHistory->setTag(btn->getTag());
			}
			showPopupNotice(list[i].Content, [=]() {}, false);
		}, isNewBtn);
		addTouchEventListener(btnCancel, [=]() {
			showPopupNotice(Utils::getSingleton().getStringForKey("ban_muon_huy_don_hang_nay"), [=]() {
				//int itemId = atoi(btnCancel->getName().c_str());
				SFSRequest::getSingleton().RequestCancelItemShop(list[i].Id);
			});
		}, isNewBtn);

		btn->setPosition(Vec2(scrollHistory->getContentSize().width / 2, heightHistory - 35));
		lb1->setPosition(65, btn->getPositionY() - 5);
		lb2->setPosition(lb1->getPositionX() + 175, lb1->getPositionY());
		lb3->setPosition(lb2->getPositionX() + 235, lb2->getPositionY());
		lb4->setPosition(lb3->getPositionX() + 220, lb3->getPositionY());

		if (list[i].Status < 1) {
			list[i].Status = 1;
		} else if (list[i].Status > strStatus.size()) {
			list[i].Status = strStatus.size();
		}
		if (list[i].Status == 1) {
			lb5->setPosition(lb4->getPositionX() + 160, lb4->getPositionY());
			btnCancel->setPosition(Vec2(lb5->getPositionX() + 65, lb5->getPositionY() + 5));
		} else {
			lb5->setPosition(lb4->getPositionX() + 185, lb4->getPositionY());
		}
		lb1->setString(to_string((int)list[i].Id));
		lb2->setString(list[i].Name);
		lb3->setString(list[i].CreateDate);
		lb4->setString(list[i].UpdateDate);
		lb5->setString(Utils::getSingleton().getStringForKey(strStatus[list[i].Status - 1]));
		btnCancel->setName(to_string((int)list[i].ItemId));
		btnCancel->setVisible(list[i].Status == 1);

		for (int j = 0; j < 5; j++) {
			lbs[j]->setVisible(true);
			lbs[j]->getChildByTag(0)->setPosition(lbs[j]->getContentSize().width / 2, lbs[j]->getContentSize().height / 2 + 5);
			lbs[j]->getChildByTag(1)->setPosition(lbs[j]->getContentSize().width / 2, lbs[j]->getContentSize().height / 2 + 5);
		}
		lb5->getChildByTag(0)->setPosition(lb5->getContentSize().width / 2 + (list[i].Status == 1 ? 25 : 0), lb5->getContentSize().height / 2 + 5);
		lb5->getChildByTag(1)->setPosition(lb5->getContentSize().width / 2 + (list[i].Status == 1 ? 25 : 0), lb5->getContentSize().height / 2 + 5);

		heightHistory -= 70;
	}
	int count = scrollHistory->getChildrenCount() - 1;
	for (int j = list.size() * 7; j < count; j++) {
		if (j % 7 != 5 && j % 7 != 6) {
			((Label*)scrollHistory->getChildByTag(j))->setVisible(false);
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

	int heightCard = ((cards[0].size() + cards[1].size() + cards[2].size() + cards[3].size() - 1) / 5 + 1) * 120;
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
			btn->setPosition(Vec2(90 + (count % 5) * 162, heightCard - 45 - (count/5) * 120));
			btn->setBright(false);
			btn->setTag((i + 1) * 100 + j);
			addTouchEventListener(btn, [=]() {
				if (Utils::getSingleton().userDataMe.MoneyReal < cards[i][j].Price) {
					showPopupNotice(Utils::getSingleton().getStringForKey("khong_du_tien_doi_thuong"), [=]() {});
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

			Label* lb1 = Label::create(Utils::getSingleton().formatMoneyWithComma(cards[i][j].Price), "fonts/myriadb.ttf", 25);
			lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, -20);
            lb1->enableOutline(Color4B(150, 150, 0, 255), 1);
			lb1->setColor(Color3B::YELLOW);
			btn->addChild(lb1);

			Label* lb2 = Label::create(Utils::getSingleton().formatMoneyWithComma(cards[i][j].PriceChange), "fonts/myriadb.ttf", 25);
			lb2->setPosition(btn->getContentSize().width / 2, 10);
			lb2->setColor(Color3B::WHITE);
			btn->addChild(lb2);

			spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
				+ spCoin->getContentSize().width * spCoin->getScale() / 2, lb1->getPositionY() + 5);

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

	int heightItem = ((items.size() - 1) / 4 + 1) * 170;
	if (heightItem < scrollItem->getContentSize().width) {
		heightItem = scrollItem->getContentSize().width;
	}
	scrollItem->setInnerContainerSize(Size(scrollItem->getContentSize().width, heightItem));
	for (int i = 0; i < items.size(); i++) {
		string str = Utils::getSingleton().getStringForKey("xac_nhan_mua_vat_pham");
		string strMoney = Utils::getSingleton().formatMoneyWithComma(moneyItems[i]);
		string msg = String::createWithFormat(str.c_str(), names[items[i]].c_str(), strMoney.c_str())->getCString();

		ui::Button* btn = ui::Button::create("box_shop.png", "", "", ui::Widget::TextureResType::PLIST);
		//btn->setPosition(Vec2(100 + (i % 4) * 200, scrollItem->getContentSize().height - 70 - (i / 4) * 170));
		btn->setPosition(Vec2(115 + (i % 4) * 200, heightItem - 65 - (i / 4) * 170));
		btn->setContentSize(Size(182, 128));
		btn->setScale9Enabled(true);
		btn->setBright(false);
		btn->setTag(i);
		//btn->setScale(.95f);
		addTouchEventListener(btn, [=]() {
			if (Utils::getSingleton().userDataMe.MoneyReal < moneyItems[i]) {
				showPopupNotice(Utils::getSingleton().getStringForKey("khong_du_tien_doi_thuong"), [=]() {});
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

		Label* lb1 = Label::create(Utils::getSingleton().formatMoneyWithComma(moneyItems[i]), "fonts/myriadb.ttf", 25);
		lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, btn->getContentSize().height / 2 - 80);
        lb1->enableOutline(Color4B(150, 150, 0, 255), 1);
        lb1->setColor(Color3B::YELLOW);
		btn->addChild(lb1);

		Label* lb2 = Label::create(names[items[i]], "fonts/myriadb.ttf", 25);
		lb2->setWidth(175);
		lb2->setHeight(30);
		lb2->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 - 50);
		lb2->setColor(Color3B::WHITE);
		lb2->setHorizontalAlignment(TextHAlignment::CENTER);
		btn->addChild(lb2);

		spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
			+ spCoin->getContentSize().width * spCoin->getScale() / 2, lb1->getPositionY() + 5);

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
	showPopupNotice(msg, [=]() {});
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

	int xx = -400;
	vector<int> posX = { xx, xx + 120, xx + 445, xx + 765 };
	vector<int> widths = { 50, 130, 460, 120 };
	ui::ScrollView* scroll = (ui::ScrollView*)popupMail->getChildByName("scroll");

	scroll->getChildByTag(9999)->setPosition(0, -500);
	int tag = scroll->getTag();
	if (tag > 0) {
		tag -= 4;
		for (int j = 0; j < 4; j++) {
			Label* lb = (Label*)scroll->getChildByTag(tag + j);
			lb->setColor(Color3B::BLACK);
			lb->getChildByTag(0)->setVisible(true);
			lb->getChildByTag(1)->setVisible(false);
		}
	}
	scroll->setTag(0);

	int height = list.size() * 70;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
	int count = scroll->getChildrenCount() - 1;
	for (int i = 0; i < list.size(); i++) {
		vector<Label*> lbs;
		ui::Button* btn;
		if (i < count / 5) {
			for (int j = 0; j < 4; j++) {
				Label* lb = (Label*)scroll->getChildByTag(i * 5 + j);
				lb->setVisible(true);
				lbs.push_back(lb);
			}

			btn = (ui::Button*)scroll->getChildByTag(i * 5 + 4);
			btn->setVisible(true);
		} else {
			btn = ui::Button::create("box.png", "box.png", "", ui::Widget::TextureResType::PLIST);
			btn->setContentSize(Size(scroll->getContentSize().width, 60));
			btn->setScale9Enabled(true);
			btn->setOpacity(0);
			btn->setTag(i * 5 + 4);
			scroll->addChild(btn);

			for (int j = 0; j < 4; j++) {
				Label* lbDetail = Label::createWithTTF("", list[i].IsRead ? "fonts/myriad.ttf" : "fonts/myriadb.ttf", 30);
				lbDetail->setWidth(widths[j]);
				lbDetail->setHeight(30);
				lbDetail->setAnchorPoint(Vec2(.5f, .5f));
				lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
				lbDetail->setTag(i * 5 + j);
				scroll->addChild(lbDetail);
				lbs.push_back(lbDetail);

				ui::Scale9Sprite* spbg = ui::Scale9Sprite::createWithSpriteFrameName("box11.png");
				spbg->setContentSize(Size(widths[j] + 30, 60));
				spbg->setTag(0);
				lbDetail->addChild(spbg, -1);

				ui::Scale9Sprite* spbg2 = ui::Scale9Sprite::createWithSpriteFrameName("box12.png");
				spbg2->setContentSize(Size(widths[j] + 30, 60));
				spbg2->setVisible(false);
				spbg2->setTag(1);
				lbDetail->addChild(spbg2, -1);
			}

			addTouchEventListener(btn, [=]() {
				int tag = scroll->getTag();
				if (tag != btn->getTag()) {
					scroll->getChildByTag(9999)->setPosition(btn->getPosition());
					for (int j = 0; j < 4; j++) {
						lbs[j]->setColor(Color3B::WHITE);
						lbs[j]->getChildByTag(0)->setVisible(false);
						lbs[j]->getChildByTag(1)->setVisible(true);
					}
					if (tag > 0) {
						tag -= 4;
						for (int j = 0; j < 4; j++) {
							Label* lb = (Label*)scroll->getChildByTag(tag + j);
							lb->setColor(Color3B::BLACK);
							lb->getChildByTag(0)->setVisible(true);
							lb->getChildByTag(1)->setVisible(false);
						}
					}
					scroll->setTag(btn->getTag());
				}
				SFSRequest::getSingleton().RequestMailContent(atoi(btn->getName().c_str()));
			});
		}
		btn->setName(to_string((int)list[i].Id));
		btn->setPosition(Vec2(scroll->getContentSize().width / 2, height - 40 - i * 70));
		lbs[0]->setString(to_string((popupMail->getChildByName("nodepage")->getTag() - 1) * 10 + i + 1));
		lbs[1]->setString(list[i].Date);
		lbs[2]->setString(list[i].Title);
		lbs[3]->setString(list[i].Sender);
		for (int j = 0; j < 4; j++) {
			lbs[j]->setColor(Color3B::BLACK);
			lbs[j]->setPosition(posX[j] + scroll->getContentSize().width / 2, btn->getPositionY());
			Size lbsize = lbs[j]->getContentSize();
			lbs[j]->getChildByTag(0)->setVisible(true);
			lbs[j]->getChildByTag(1)->setVisible(false);
			lbs[j]->getChildByTag(0)->setPosition(lbsize.width / 2, lbsize.height / 2);
			lbs[j]->getChildByTag(1)->setPosition(lbsize.width / 2, lbsize.height / 2);
		}
	}

	for (int i = list.size() * 5; i < count; i++) {
		scroll->getChildByTag(i)->setVisible(false);
	}
}

void MainScene::onMailContentResponse(std::string content)
{
	Node* popupDetail = createPopupDetail();
	Label* lb = (Label*)popupDetail->getChildByName("lbcontent");
	lb->setString(content);
	showPopup(popupDetail);
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

	int widthTitle = list.size() * 190 + 15;
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
			btn->setTitleFontName("fonts/myriadb.ttf");
			btn->setTitleFontSize(35);
			btn->setTitleDeviation(Vec2(0, -5));
			btn->setContentSize(Size(180, 60));
			btn->setScale9Enabled(true);
			btn->setTag(i);
			scrollTitle->addChild(btn);
		} else {
			isNewBtn = false;
			btn->setVisible(true);
		}
		btn->setTitleColor(i == 0 ? Color3B::YELLOW : Color3B::WHITE);
		btn->loadTextureNormal(i == 0 ? "box12.png" : "empty.png", ui::Widget::TextureResType::PLIST);
		btn->setPosition(Vec2(90 + i * 190, scrollTitle->getContentSize().height / 2));
		btn->setTitleText(strTitle);
		addTouchEventListener(btn, [=]() {
			ui::Button* btn1 = (ui::Button*)scrollTitle->getChildByTag(scrollTitle->getTag());
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::YELLOW);
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
	if (Utils::getSingleton().allowEventPopup 
		&& Utils::getSingleton().dynamicConfig.Popup
		&& !Utils::getSingleton().hasShowEventPopup
		&& Utils::getSingleton().ispmE()) {
		//Utils::getSingleton().hasShowEventPopup = true;
		//showWebView(Utils::getSingleton().dynamicConfig.PopupUrl);
		isEventReady = true;
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
	strProviders = Utils::getSingleton().dynamicConfig.CashValue;
	bool pmE = Utils::getSingleton().ispmE();

	popupCharge = createPopup("title_naptien.png", true, true);
	if (!popupCharge) return;
	popupCharge->setTag(0);

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
	nodeStore->setPosition(0, -50);
    popupCharge->addChild(nodeStore);

	vector<Node*> nodes;
	nodes.push_back(nodeCard);
	nodes.push_back(nodeSms);
	nodes.push_back(nodeStore);

	Node* nodeMoneyType = Node::create();
	nodeMoneyType->setName("nodemoneytype");
	nodeMoneyType->setPosition(0, -20);
	nodeMoneyType->setVisible(pmE);
	popupCharge->addChild(nodeMoneyType);

	Sprite* bgMenu = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgMenu->setPosition(0, 200);
	popupCharge->addChild(bgMenu);

	ui::EditBox* tfSeri = ui::EditBox::create(Size(320, 55), "empty.png", ui::Widget::TextureResType::PLIST);
	ui::EditBox* tfCode = ui::EditBox::create(Size(320, 55), "empty.png", ui::Widget::TextureResType::PLIST);
	ui::ScrollView* scrollProvider = ui::ScrollView::create();

	int x = -250;
	int y = bgMenu->getPositionY();
	vector<string> texts = { "nap_the", "nap_sms", "nap_iap" };
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
			if (popupCharge->getTag() == i) return; 
			popupCharge->getChildByTag(100 + i)->setVisible(true);
			popupCharge->getChildByTag(100 + popupCharge->getTag())->setVisible(false);
			ui::Button* btn1 = (ui::Button*)popupCharge->getChildByTag(10 + popupCharge->getTag());
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::YELLOW);
			popupCharge->setTag(i);

			if (i == 0) {
				scrollProvider->setVisible(true);
				nodeMoneyType->setPosition(0, -20);
				for (int i = 1; i <= strProviders.size(); i++) {
					scrollProvider->getChildByName("btn" + to_string(i))->setVisible(true);
				}
			} else if (i == 1) {
				scrollProvider->scrollToLeft(.3f, true);
				scrollProvider->setVisible(true);
				nodeMoneyType->setPosition(0, -20);
				for (int i = 1; i <= 3 && i <= strProviders.size(); i++) {
					scrollProvider->getChildByName("btn" + to_string(i))->setVisible(true);
				}
				checkProviderToCharge();
			} else if(i == 2){
				scrollProvider->setVisible(false);
				nodeMoneyType->setPosition(0, 110);
			}
		});
		popupCharge->addChild(btn);
		x += 250;
	}

	scrollProvider->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollProvider->setBounceEnabled(true);
	scrollProvider->setPosition(Vec2(-480, 5));
	scrollProvider->setContentSize(Size(960, 150));
	scrollProvider->setInnerContainerSize(Size(strProviders.size() * 170, 120));
	scrollProvider->setScrollBarEnabled(true);
	scrollProvider->setName("scrollprovider");
	popupCharge->addChild(scrollProvider);

	int xp = 98;
	int yp = 75;
	string smsContent = Utils::getSingleton().gameConfig.smsVT;
	int strIndex = smsContent.find_last_of(' ');
	string smsTarget = smsContent.substr(strIndex + 1, smsContent.length() - strIndex);
	smsContent = smsContent.substr(0, strIndex);
	smsContent = Utils::getSingleton().replaceString(smsContent, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
	for (int i = 1; i <= strProviders.size(); i++) {
		string stri = to_string(i);
		string strimg = strProviders[i-1] + ".png";

		ui::Button* btnProvider = ui::Button::create(strimg, strimg, "", ui::Widget::TextureResType::PLIST);
		btnProvider->setPosition(Vec2(xp, yp));
		btnProvider->setTag(i == 1 ? 1 : 0);
		btnProvider->setName("btn" + stri);
		addTouchEventListener(btnProvider, [=]() {
			if (btnProvider->getTag() == 1) return;
			int btnIndex;
			int btnIndexLast;
			for (int j = 1; j <= strProviders.size(); j++) {
				string strj = to_string(j);
				ui::Button* btn = (ui::Button*)scrollProvider->getChildByName("btn" + strj);
				if (btn->getColor() == Color3B::WHITE) {
					btnIndexLast = j;
				}
				if (btn != btnProvider) {
					btn->setTag(0);
					btn->setColor(Color3B::GRAY);
					btn->getChildByTag(1)->setVisible(false);
				} else {
					btn->setTag(1);
					btn->setColor(Color3B::WHITE);
					btn->getChildByTag(1)->setVisible(true);
					btnIndex = i;
				}
			}

			if (i < strProviders.size() && nodeSms->isVisible()) {
				ui::CheckBox* cbQuan = (ui::CheckBox*)nodeMoneyType->getChildByTag(0);
				updateSmsInfo(cbQuan->isSelected());
			}

			if (i > 3) {
				tfCode->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
				tfSeri->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
			} else {
				if (btnIndexLast > 3) {
					tfCode->setText("");
					tfSeri->setText("");
				}
				tfCode->setInputMode(ui::EditBox::InputMode::NUMERIC);
				tfSeri->setInputMode(ui::EditBox::InputMode::NUMERIC);
			}
		});
		scrollProvider->addChild(btnProvider);
		xp += 190;

		ui::Scale9Sprite* bgProvider = ui::Scale9Sprite::createWithSpriteFrameName("box8.png");
		bgProvider->setContentSize(btnProvider->getContentSize() + Size(40, 70));
		bgProvider->setPosition(btnProvider->getContentSize().width / 2, btnProvider->getContentSize().height / 2 - 20);
		bgProvider->setTag(1);
		btnProvider->addChild(bgProvider, -1);

		if (i > 1) {
			btnProvider->setColor(Color3B::GRAY);
			bgProvider->setVisible(false);
		}
	}
	
	ui::Scale9Sprite* bgMoneyType = ui::Scale9Sprite::createWithSpriteFrameName("box8.png");
	bgMoneyType->setContentSize(Size(960, 80));
	nodeMoneyType->addChild(bgMoneyType);

	vector<Label*> lbs;
	vector<ui::CheckBox*> cbs;
	for (int i = 0; i < 2; i++) {
		ui::CheckBox* checkbox = ui::CheckBox::create();
		checkbox->loadTextureBackGround("unchecked.png", ui::Widget::TextureResType::PLIST);
		checkbox->loadTextureFrontCross("checked.png", ui::Widget::TextureResType::PLIST);
		checkbox->setPosition(Vec2(-100 + 200 * i, 0));
		checkbox->setSelected(false);
		checkbox->setTag(i);
		nodeMoneyType->addChild(checkbox);
		cbs.push_back(checkbox);

		Label* lb = Label::create("", "fonts/myriadb.ttf", 35);
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
	ui::Scale9Sprite* bgNodeCard = ui::Scale9Sprite::createWithSpriteFrameName("box3.png");
	bgNodeCard->setContentSize(Size(960, 180));
	bgNodeCard->setPosition(0, -160);
	nodeCard->addChild(bgNodeCard);

	Node* nodeCardInfo = Node::create();
	nodeCardInfo->setPosition(280, bgNodeCard->getPositionY());
	nodeCardInfo->setName("nodecardinfo");
	nodeCard->addChild(nodeCardInfo);

	ui::Scale9Sprite* bgCardInfo = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgCardInfo->setContentSize(Size(480, bgNodeCard->getContentSize().height));
	nodeCardInfo->addChild(bgCardInfo);

	ui::ScrollView* scrollInfo = ui::ScrollView::create();
	scrollInfo->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollInfo->setBounceEnabled(true);
	scrollInfo->setPosition(Vec2(-bgCardInfo->getContentSize().width / 2 + 5, -bgCardInfo->getContentSize().height / 2 + 5));
	scrollInfo->setContentSize(bgCardInfo->getContentSize() - Size(10, 10));
	scrollInfo->setScrollBarEnabled(false);
	scrollInfo->setName("scrollinfo");
	nodeCardInfo->addChild(scrollInfo);

	int height = moneys.size() * 50 / 2;
	if (height < scrollInfo->getContentSize().height) {
		height = scrollInfo->getContentSize().height;
	}
	scrollInfo->setInnerContainerSize(Size(scrollInfo->getContentSize().width, height));
	//for (int i = 0; i < 2; i++) {
		for (int j = 0; j < moneys.size(); j++) {
			string str1 = Utils::getSingleton().getStringForKey("the") + " " + to_string(moneys[j]) + "k = ";
			string str2 = to_string(moneys[j]) + "k " + "Quan";//(i == 0 ? "Quan" : "Xu");

			Label* lb1 = Label::create(str1, "fonts/myriadb.ttf", 25);
			lb1->setTag(j * 2);
			lb1->setAnchorPoint(Vec2(1, .5f));
			lb1->setPosition(100 + 210 * (j / 3) , height - 40 - 50 * (j % 3));
			scrollInfo->addChild(lb1);

			Label* lb2 = Label::create(str2, "fonts/myriadb.ttf", 25);
			lb2->setTag(j * 2 + 1);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setPosition(lb1->getPositionX() + 3, lb1->getPositionY());
			lb2->setColor(Color3B::YELLOW);//i == 0 ? Color3B::YELLOW : Color3B(0, 255, 255));
			scrollInfo->addChild(lb2);
		}
	//}

	Node* nodeInput = Node::create();
	nodeInput->setPosition(-280, bgNodeCard->getPositionY());
	nodeCard->addChild(nodeInput);

	Sprite* bgSeri = Sprite::createWithSpriteFrameName("input_mathe.png");
	bgSeri->setPosition(0, 40);
	nodeInput->addChild(bgSeri);

	Sprite* bgCode = Sprite::createWithSpriteFrameName("input_mathe.png");
	bgCode->setPosition(0, -40);
	nodeInput->addChild(bgCode);

	tfSeri->setPosition(bgSeri->getPosition());
	tfSeri->setFontName("Arial");
	tfSeri->setFontSize(30);
	tfSeri->setFontColor(Color3B::WHITE);
	tfSeri->setMaxLength(24);
	tfSeri->setPlaceholderFont("Arial", 30);
	tfSeri->setPlaceholderFontColor(Color3B(200, 200, 200));
	tfSeri->setPlaceHolder(Utils::getSingleton().getStringForKey("nhap_so_seri").c_str());
	tfSeri->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfSeri->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfSeri->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfSeri->setDelegate(this);
	nodeInput->addChild(tfSeri);

	tfCode->setPosition(bgCode->getPosition());
	tfCode->setFontName("Arial");
	tfCode->setFontSize(30);
	tfCode->setFontColor(Color3B::WHITE);
	tfCode->setMaxLength(24);
	tfCode->setPlaceholderFont("Arial", 30);
	tfCode->setPlaceholderFontColor(Color3B(200, 200, 200));
	tfCode->setPlaceHolder(Utils::getSingleton().getStringForKey("nhap_ma_the").c_str());
	tfCode->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfCode->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfCode->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfCode->setDelegate(this);
	nodeInput->addChild(tfCode);

	ui::Button* btnCharge = ui::Button::create("btn_nap.png", "", "", ui::Widget::TextureResType::PLIST);
	btnCharge->setPosition(Vec2(bgSeri->getPositionX() + 250, bgSeri->getPositionY()));
	btnCharge->setScale(.9f);
	addTouchEventListener(btnCharge, [=]() {
		if (strProviders.size() == 0) return;
		string code = tfCode->getText();
		string seri = tfSeri->getText();
		if (code.length() == 0 || seri.length() == 0) return;
		int providerId;
		for (int i = 1; i <= strProviders.size(); i++) {
			Node* n = scrollProvider->getChildByName("btn" + to_string(i));
			if (n->getTag() == 1) {
				providerId = i - 1;
				break;
			}
		}
		string provider = strProviders[providerId];
		int moneyType = cbQuan->isSelected() ? 2 : 1;
		SFSRequest::getSingleton().RequestChargeCard(code, seri, provider, moneyType);
		tfCode->setText("");
		tfSeri->setText("");
		showWaiting();
		chargingProvider = provider;
	});
	nodeInput->addChild(btnCharge);

	//Node SMS
	ui::Scale9Sprite* bgSms = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgSms->setPosition(0, -170);
	bgSms->setContentSize(Size(980, 250));
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
			itemSms->setPosition(125 + i * 250, 125);
			itemSms->setName(strMoney);
			scrollSms->addChild(itemSms);

			ui::Scale9Sprite* bgItemSms = ui::Scale9Sprite::createWithSpriteFrameName("box3.png");
			bgItemSms->setContentSize(Size(240, 170));
			itemSms->addChild(bgItemSms);

			Label* lbItemSms1 = Label::create("SMS " + strMoney + "k = ", "fonts/myriadb.ttf", 30);
			lbItemSms1->setAnchorPoint(Vec2(1, .5f));
			lbItemSms1->setPosition(5, 40);
			itemSms->addChild(lbItemSms1);

			Label* lbItemSms2 = Label::create(strMoney2 + "k Quan", "fonts/myriadb.ttf", 30);
			lbItemSms2->setAnchorPoint(Vec2(0, .5f));
			lbItemSms2->setColor(Color3B::YELLOW);
			lbItemSms2->setPosition(lbItemSms1->getPositionX() + 5, lbItemSms1->getPositionY());
			lbItemSms2->setName("lbsmsmoney");
			itemSms->addChild(lbItemSms2);

			Label* lbItemSms3 = Label::create(smsStr, "fonts/myriadb.ttf", 30);
			lbItemSms3->setPosition(0, lbItemSms1->getPositionY() - 35);
			lbItemSms3->setName("lbsmscontent");
			itemSms->addChild(lbItemSms3);

			Label* lbItemSms4 = Label::create(smsTarget, "fonts/myriadb.ttf", 30);
			lbItemSms4->setPosition(0, lbItemSms3->getPositionY() - 35);
			lbItemSms4->setName("lbsmstarget");
			itemSms->addChild(lbItemSms4);

			ui::Button* btnItemSms = ui::Button::create("btn_nap.png", "", "", ui::Widget::TextureResType::PLIST);
			btnItemSms->setPosition(Vec2(0, -85));
			addTouchEventListener(btnItemSms, [=]() {
				if (strProviders.size() == 0) return;
				int btnIndex;
				for (int i = 1; i < strProviders.size(); i++) {
					ui::Button* btn = (ui::Button*)scrollProvider->getChildByName("btn" + to_string(i));
					if (btn->getTag() == 1) {
						btnIndex = i;
					}
				}
				//CCLOG("%s %s", lbItemSms4->getString().c_str(), lbItemSms3->getString().c_str());
				Utils::getSingleton().openSMS(lbItemSms4->getString(), lbItemSms3->getString());
				Tracker::getSingleton().trackPurchaseSuccess("ClickSMS", strProviders[btnIndex - 1], "VND", moneys[i] * 1000);
			});
			itemSms->addChild(btnItemSms);
		}
	//}
    
    //Nap CH
    vector<ProductData> products = Utils::getSingleton().products;
    Size storeSize = Size(960, 360);
    
    ui::ScrollView* scrollStore = ui::ScrollView::create();
    scrollStore->setDirection(ui::ScrollView::Direction::HORIZONTAL);
    scrollStore->setBounceEnabled(true);
    scrollStore->setPosition(Vec2(-storeSize.width/2, -storeSize.height/2 - 50));
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
        
        ui::Button* btn = ui::Button::create("box10.png", "", "", ui::Widget::TextureResType::PLIST);
        btn->setPosition(Vec2(170 + i * 310, storeSize.height/2 + 45));
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
        
        Sprite* spCoin = Sprite::createWithSpriteFrameName(pmE ? "icon_gold.png" : "icon_silver.png");
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

    if(!pmE){
		bgMenu->setVisible(false);
        nodeCard->setVisible(false);
        nodeStore->setVisible(true);
        nodeStore->setPosition(0, 30);
		nodeMoneyType->setVisible(false);
		popupCharge->getChildByName("scrollprovider")->setVisible(false);
		for (int i = 0; i < texts.size(); i++) {
			popupCharge->getChildByTag(10 + i)->setVisible(false);
		}
    }
}

void MainScene::initPopupGuide()
{
	popupGuide = createPopup("title_huongdan.png", true, true);
	if (!popupGuide) return;
	popupGuide->setTag(0);

	Size size = Size(850, 416);
	Size scrollSize = size - Size(20, 20);

	Sprite* bgScrollMenu = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgScrollMenu->setPosition(0, 200);
	popupGuide->addChild(bgScrollMenu);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2 - 50));
	scroll->setContentSize(scrollSize);
	scroll->setScrollBarEnabled(false);
	popupGuide->addChild(scroll);

	cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
	Label* lb = Label::create(plist["tutorial_1"].asString(), "fonts/myriad.ttf", 30);
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
	int x = -300;
	int y = bgScrollMenu->getPositionY();
	for (int i = 0; i < 4; i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box12.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleColor(i == 0 ? Color3B::YELLOW : Color3B::WHITE);
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleFontSize(35);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setContentSize(Size(210, 60));
		btn->setPosition(Vec2(x, y));
		btn->setScale9Enabled(true);
		//btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(i); 
		addTouchEventListener(btn, [=]() {
			ui::Button* btn1 = (ui::Button*)popupGuide->getChildByTag(popupGuide->getTag());
			cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
			string content = plist["tutorial_" + to_string(btn->getTag() + 1)].asString();

			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::YELLOW);
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
		x += 210;
	}
}

void MainScene::initPopupMail()
{
	popupMail = createPopup("title_hopthu.png", true, true);
	if (!popupMail) return;

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgContent->setContentSize(Size(980, 480));
	bgContent->setPosition(0, 20);
	popupMail->addChild(bgContent);

	Size size = bgContent->getContentSize();
	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-size.width / 2 + 5, -size.height / 2 + 5) + bgContent->getPosition());
	scroll->setContentSize(Size(size.width - 10, size.height - 50));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	scroll->setTag(0);
	popupMail->addChild(scroll);

	Sprite* bgChosen = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgChosen->setPosition(0, -500);
	bgChosen->setTag(9999);
	scroll->addChild(bgChosen);

	addBtnChoosePage(0, -260, popupMail, [=](int page) {
		SFSRequest::getSingleton().RequestListMail(page);
		//onListMailDataResponse(vector<MailData>());
	});
}

void MainScene::initPopupNews()
{
	popupNews = createPopup("title_tintuc.png", true, true);
	if (!popupNews) return;
	popupNews->setTag(1);

	Size size = Size(850, 386);
	Size scrollSize = size - Size(20, 20);

	Sprite* bgScrollMenu = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgScrollMenu->setPosition(0, 200);
	popupNews->addChild(bgScrollMenu);

	ui::ScrollView* scrollContent = ui::ScrollView::create();
	scrollContent->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollContent->setBounceEnabled(true);
	scrollContent->setPosition(Vec2(-scrollSize.width / 2, -scrollSize.height / 2 - 35));
	scrollContent->setContentSize(scrollSize);
	scrollContent->setScrollBarEnabled(false);
	scrollContent->setName("scrollcontent");
	popupNews->addChild(scrollContent);

	Label* lbContent = Label::create("", "fonts/myriad.ttf", 30);
	lbContent->setWidth(scrollContent->getContentSize().width);
	lbContent->setAnchorPoint(Vec2(0, 1));
	lbContent->setColor(Color3B::BLACK);
	lbContent->setName("lbcontent");
	scrollContent->addChild(lbContent);

	Size tsize = bgScrollMenu->getContentSize() - Size(50, 20);
	ui::ScrollView* scrollTitle = ui::ScrollView::create();
	scrollTitle->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollTitle->setBounceEnabled(true);
	scrollTitle->setPosition(Vec2(-tsize.width / 2, -tsize.height / 2 + bgScrollMenu->getPositionY()));
	scrollTitle->setContentSize(tsize);
	scrollTitle->setScrollBarEnabled(false);
	scrollTitle->setName("scrolltitle");
	scrollTitle->setTag(0);
	popupNews->addChild(scrollTitle);

	addBtnChoosePage(0, -260, popupNews, [=](int page) {
		scrollTitle->setTag(0);
		SFSRequest::getSingleton().RequestNews(page);
		//onNewsDataResponse(vector<NewsData>());
	});
}

void MainScene::initPopupShop()
{
	popupShop = createPopup("title_cuahang.png", true, true);
	if (!popupShop) return;
	popupShop->setTag(0);

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::createWithSpriteFrameName("empty.png");
	bgContent->setContentSize(Size(830, 420));
	bgContent->setPosition(0, -50);
	bgContent->setOpacity(200);
	popupShop->addChild(bgContent);

	Size size = bgContent->getContentSize();
	Size scrollSize = size - Size(20, 20);

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

	Sprite* bgMenu = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgMenu->setPosition(0, 200);
	popupShop->addChild(bgMenu);

	int x = -250;
	int y = bgMenu->getPositionY();
	vector<string> texts = { "the_cao" , "vat_pham", "lich_su" };
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "box12.png" : "empty.png", "", "", ui::Widget::TextureResType::PLIST);
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleFontSize(35);
		btn->setTitleColor(i == 0 ? Color3B::YELLOW : Color3B::WHITE);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setContentSize(Size(215, 60));
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
			btn1->loadTextureNormal("empty.png", ui::Widget::TextureResType::PLIST);
			btn->loadTextureNormal("box12.png", ui::Widget::TextureResType::PLIST);
			btn1->setTitleColor(Color3B::WHITE);
			btn->setTitleColor(Color3B::YELLOW);
			popupShop->setTag(btn->getTag());

			if (i == 2) {
				SFSRequest::getSingleton().RequestShopHistory(1);
			}
		});
		popupShop->addChild(btn);

		x += 250;
	}

	ui::ScrollView* scrollCard = ui::ScrollView::create();
	//scrollCard->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollCard->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollCard->setBounceEnabled(true);
	scrollCard->setPosition(Vec2(bgContent->getPositionX() - bgContent->getContentSize().width / 2, bgContent->getPositionY() - bgContent->getContentSize().height / 2 + 5));
	scrollCard->setContentSize(Size(bgContent->getContentSize().width, bgContent->getContentSize().height - 10));
	scrollCard->setScrollBarEnabled(false);
	scrollCard->setName("scrollcard");
	nodeCard->addChild(scrollCard);

	ui::ScrollView* scrollItem = ui::ScrollView::create();
	scrollItem->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollItem->setBounceEnabled(true);
	scrollItem->setPosition(scrollCard->getPosition());
	scrollItem->setContentSize(scrollCard->getContentSize());
	scrollItem->setScrollBarEnabled(false);
	scrollItem->setName("scrollitem");
	nodeItem->addChild(scrollItem);
	
	ui::ScrollView* scrollHistory = ui::ScrollView::create();
	scrollHistory->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollHistory->setBounceEnabled(true);
	scrollHistory->setPosition(Vec2(bgContent->getPositionX() - bgContent->getContentSize().width / 2 - 70, bgContent->getPositionY() - bgContent->getContentSize().height / 2 + 30));
	scrollHistory->setContentSize(Size(bgContent->getContentSize().width + 140, bgContent->getContentSize().height - 35));
	scrollHistory->setScrollBarEnabled(false);
	scrollHistory->setName("scrollhistory");
	nodeHistory->addChild(scrollHistory);

	Sprite* bgChosen = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgChosen->setPosition(0, -500);
	bgChosen->setTag(9999);
	scrollHistory->addChild(bgChosen);

	addBtnChoosePage(0, -265, nodeHistory, [=](int page) {
		SFSRequest::getSingleton().RequestShopHistory(page);
		//onShopHistoryDataResponse(vector<ShopHistoryData>());
	});
}

void MainScene::initPopupGiftcode()
{
	popupGiftcode = createPopup("title_giftcode.png", false, true);
	if (!popupGiftcode) return;

	Sprite* bgText = Sprite::createWithSpriteFrameName("bg_slide_money.png");
	bgText->setPosition(Vec2(0, -20));
	popupGiftcode->addChild(bgText);

	ui::EditBox* tfGiftcode = ui::EditBox::create(Size(360, 55), "empty.png", ui::Widget::TextureResType::PLIST);
	tfGiftcode->setPosition(bgText->getPosition());
	tfGiftcode->setFontName("Arial");
	tfGiftcode->setFontSize(25);
	tfGiftcode->setFontColor(Color3B::WHITE);
	tfGiftcode->setMaxLength(20);
	tfGiftcode->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfGiftcode->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfGiftcode->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfGiftcode->setDelegate(this);
	popupGiftcode->addChild(tfGiftcode);

	Label* lb = Label::create(Utils::getSingleton().getStringForKey("nhap_giftcode"), "fonts/myriad.ttf", 40);
	lb->setColor(Color3B::BLACK);
	lb->setPosition(0, 60);
	popupGiftcode->addChild(lb);

	Sprite* bg = (Sprite*)popupGiftcode->getChildByName("spbg");
	ui::Button* btnSubmit = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnSubmit->setTitleText(Utils::getSingleton().getStringForKey("xac_nhan"));
	btnSubmit->setTitleFontName("fonts/myriadb.ttf");
	btnSubmit->setTitleFontSize(40);
	btnSubmit->setTitleDeviation(Vec2(0, -5));
	btnSubmit->setPosition(Vec2(0, -bg->getContentSize().height / 2 + 10));
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
	popupDisplayName->setPosition(winSize.width / 2, winSize.height / 2);
	popupDisplayName->setVisible(false);
	popupDisplayName->setName("popupdisplayname");
	mLayer->addChild(popupDisplayName, constant::ZORDER_POPUP);
	//autoScaleNode(popupDisplayName);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(800, 400));
	bg->setScale(.8f);
	popupDisplayName->addChild(bg);

	Sprite* bgText = Sprite::createWithSpriteFrameName("input_mathe.png");
	popupDisplayName->addChild(bgText);

	ui::EditBox* tfDisplayName = ui::EditBox::create(Size(314, 65), "empty.png", ui::Widget::TextureResType::PLIST);
	tfDisplayName->setPosition(bgText->getPosition());
	tfDisplayName->setFontName("Arial");
	tfDisplayName->setFontSize(25);
	tfDisplayName->setFontColor(Color3B::WHITE);
	tfDisplayName->setMaxLength(16);
	tfDisplayName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfDisplayName->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfDisplayName->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfDisplayName->setDelegate(this);
	popupDisplayName->addChild(tfDisplayName);

	Label* lb = Label::create(Utils::getSingleton().getStringForKey("nhap_ten_hien_thi"), "fonts/myriadb.ttf", 35);
	lb->setColor(Color3B::BLACK);
	lb->setPosition(0, 70);
	popupDisplayName->addChild(lb);

	ui::Button* btnSubmit = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnSubmit->setTitleText(Utils::getSingleton().getStringForKey("xac_nhan"));
	btnSubmit->setTitleFontName("fonts/myriadb.ttf");
	btnSubmit->setTitleFontSize(40);
	btnSubmit->setTitleDeviation(Vec2(0, -5));
	btnSubmit->setPosition(Vec2(0, -100));
	addTouchEventListener(btnSubmit, [=]() {
		tmpDisplayName = Utils::getSingleton().trim(tfDisplayName->getText());
		if (Utils::getSingleton().isDisplayNameValid(tmpDisplayName)) {
			isChangingDisplayName = true;
			SFSRequest::getSingleton().RequestUpdateDisplayName(tmpDisplayName);
		} else {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_displayname_format"), [=]() {});
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

void MainScene::checkProviderToCharge()
{
	int btnIndex = -1;
	Node* scrollProvider = popupCharge->getChildByName("scrollprovider");
	for (int i = 4; i <= strProviders.size(); i++) {
		Node* btni = scrollProvider->getChildByName("btn" + to_string(i));
		if (btni->getTag() == 1) {
			Node* btn1 = scrollProvider->getChildByName("btn1");
			btn1->setTag(1);
			btn1->setColor(Color3B::WHITE);
			btn1->getChildByTag(1)->setVisible(true);
			btnIndex = 1;
		}
		btni->setVisible(false);
		btni->setTag(0);
		btni->setColor(Color3B::GRAY);
		btni->getChildByTag(1)->setVisible(false);
	}

	if (btnIndex == -1) {
		for (int i = 1; i <= strProviders.size(); i++) {
			ui::Button* btn = (ui::Button*)scrollProvider->getChildByName("btn" + to_string(i));
			if (btn->getTag() == 1) {
				btnIndex = i;
				break;
			}
		}
	}
	string smsct = btnIndex == 1 ? Utils::getSingleton().gameConfig.smsVT : Utils::getSingleton().gameConfig.smsVNPVMS;
	int strid = smsct.find_last_of(' ');
	string smstg = smsct.substr(strid + 1, smsct.length() - strid);
	smsct = smsct.substr(0, strid);
	smsct = Utils::getSingleton().replaceString(smsct, "uid", to_string(Utils::getSingleton().userDataMe.UserID));

	ui::ScrollView* scroll = (ui::ScrollView*)(popupCharge->getChildByName("nodesms")->getChildByName("scrollsms"));
	for (int i = 0; i < moneys.size(); i++) {
		string strMoney = to_string(moneys[i]);
		string smsStr = Utils::getSingleton().replaceString(smsct, "vnd", strMoney);

		Node* node = scroll->getChildByName(strMoney);
		Label* lbContent = (Label*)node->getChildByName("lbsmscontent");
		Label* lbTarget = (Label*)node->getChildByName("lbsmstarget");
		lbContent->setString(smsStr);
		lbTarget->setString(smstg);
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
	int btnIndex = 0;
	for (int j = 1; j <= strProviders.size(); j++) {
		string strj = to_string(j);
		ui::Button* btn = (ui::Button*)scrollProvider->getChildByName("btn" + strj);
		if (btn->getTag() == 1) {
			btnIndex = j;
			break;
		}
	}
	string smsct = btnIndex == 1 ? Utils::getSingleton().gameConfig.smsVT : Utils::getSingleton().gameConfig.smsVNPVMS;
	int strid = smsct.find_last_of(' ');
	string smstg = smsct.substr(strid + 1, smsct.length() - strid);
	smsct = smsct.substr(0, strid);
	smsct = Utils::getSingleton().replaceString(smsct, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
	if(!isQuan) smsct = Utils::getSingleton().replaceString(smsct, "T", "A");

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
