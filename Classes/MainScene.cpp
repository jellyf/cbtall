#pragma once
#include "MainScene.h"
#include "SFSRequest.h"
#include "Constant.h"
#include "Utils.h"
#include "EventHandler.h"

using namespace cocos2d;
using namespace std;

void MainScene::onInit()
{
	BaseScene::onInit();
	bool paymentEnabled = Utils::getSingleton().isPaymentEnabled();
	currentMoneyType = Utils::getSingleton().moneyType;

	std::vector<Vec2> vecPos;
	vecPos.push_back(Vec2(80, 65));
	vecPos.push_back(Vec2(240, 65));
	vecPos.push_back(Vec2(400, 65));
	vecPos.push_back(Vec2(560, 65));
	vecPos.push_back(Vec2(730, 65));
	vecPos.push_back(Vec2(900, 65));
	vecPos.push_back(Vec2(1040, 65));
	vecPos.push_back(Vec2(140, 335));
	vecPos.push_back(Vec2(426, 335));
	vecPos.push_back(Vec2(703, 335));
	vecPos.push_back(Vec2(988, 335));
	int m = 0;

	initHeaderWithInfos();

	Sprite* bg = Sprite::create("bg.jpg");
	bg->setPosition(560, 350);
	addChild(bg);

	ui::Button* btnAgency = ui::Button::create("main/icon_agency.png");
	btnAgency->setPosition(vecPos[m++]);
	btnAgency->setVisible(paymentEnabled);
	addTouchEventListener(btnAgency, [=]() {
		showPopupNotice(Utils::getSingleton().getStringForKey("feature_coming_soon"), [=]() {});
	});
	mLayer->addChild(btnAgency);
	autoScaleNode(btnAgency);

	ui::Button* btnGuide = ui::Button::create("main/icon_guide.png");
	btnGuide->setPosition(vecPos[m++]);
	btnGuide->setVisible(paymentEnabled);
	addTouchEventListener(btnGuide, [=]() {
		if (popupGuide == nullptr) {
			initPopupGuide();
		}
		showPopup(popupGuide);
	});
	mLayer->addChild(btnGuide);
	autoScaleNode(btnGuide);

	ui::Button* btnCharge = ui::Button::create("main/icon_charge.png");
	btnCharge->setPosition(vecPos[m++]);
	addTouchEventListener(btnCharge, [=]() {
		if (popupCharge == nullptr) {
			initPopupCharge();
		}
		showPopup(popupCharge);
	});
	mLayer->addChild(btnCharge);
	autoScaleNode(btnCharge);
    if(paymentEnabled){
        btnCharge->setPosition(vecPos[2]);
    }else{
		btnCharge->setPosition(vecPos[3]);
		btnCharge->setVisible(CC_TARGET_PLATFORM == CC_PLATFORM_IOS);
    }

	ui::Button* btnShop = ui::Button::create("main/icon_shop.png");
	btnShop->setPosition(vecPos[m++]);
	btnShop->setVisible(paymentEnabled);
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

	ui::Button* btnNews = ui::Button::create("main/icon_news.png");
	btnNews->setPosition(vecPos[m++]);
	btnNews->setVisible(paymentEnabled);
	addTouchEventListener(btnNews, [=]() {
		showPopupNews();
		/*cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("configs.xml");
		string url = plist["link_tin_tuc"].asString();
		showWebView(url);*/
	});
	mLayer->addChild(btnNews);
	autoScaleNode(btnNews);

	ui::Button* btnMail = ui::Button::create("main/icon_mail.png");
	btnMail->setPosition(vecPos[m++]);
	btnMail->setVisible(paymentEnabled);
	addTouchEventListener(btnMail, [=]() {
		showPopupMail();
	});
	mLayer->addChild(btnMail);
	autoScaleNode(btnMail);

	Sprite* circleNewMail = Sprite::create("main/circle_red.png");
	circleNewMail->setPosition(btnMail->getPosition() + Vec2(30, 25));
	circleNewMail->setScale(.7f);
	circleNewMail->setVisible(false);
	mLayer->addChild(circleNewMail);

	lbNewMail = Label::create("", "fonts/arial.ttf", 30);
	lbNewMail->setPosition(circleNewMail->getContentSize().width / 2 - 4, circleNewMail->getContentSize().height / 2);
	circleNewMail->addChild(lbNewMail);

	ui::Button* btnGiftcode = ui::Button::create("main/icon_giftcode.png");
	btnGiftcode->setPosition(vecPos[m++]);
	btnGiftcode->setVisible(paymentEnabled);
	addTouchEventListener(btnGiftcode, [=]() {
		if (popupGiftcode == nullptr) {
			initPopupGiftcode();
		}
		showPopup(popupGiftcode);
	});
	mLayer->addChild(btnGiftcode);
	autoScaleNode(btnGiftcode);

	ui::Button* btnNhaTranh = ui::Button::create("main/nhatranh.png", "main/nhatranh.png");
	btnNhaTranh->setPosition(vecPos[m++]);
	addTouchEventListener(btnNhaTranh, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 0;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnNhaTranh);

	ui::Button* btnDinhLang = ui::Button::create("main/dinhlang.png", "main/dinhlang.png");
	btnDinhLang->setPosition(vecPos[m++]);
	addTouchEventListener(btnDinhLang, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 1;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnDinhLang);

	ui::Button* btnPhuChua = ui::Button::create("main/phuchua.png", "main/phuchua.png");
	btnPhuChua->setPosition(vecPos[m++]);
	addTouchEventListener(btnPhuChua, [=]() {
		if (isWaiting) return;
		showWaiting();
		tmpZoneId = 2;
		isGoToLobby = true;
		SFSRequest::getSingleton().Disconnect();
	});
	mLayer->addChild(btnPhuChua);

	ui::Button* btnLoiDai = ui::Button::create("main/loidai.png", "main/loidai.png");
	btnLoiDai->setPosition(vecPos[m++]);
	addTouchEventListener(btnLoiDai, [=]() {
		showPopupNotice(Utils::getSingleton().getStringForKey("feature_coming_soon"), [=]() {});
	});
	mLayer->addChild(btnLoiDai);

	/*initPopupCharge();
	initPopupGuide();
	initPopupMail();
	initPopupNews();
	initPopupRank();
	initPopupShop();
	initPopupSettings();
	initPopupHistory();
	initPopupDisplayName();
	initPopupGiftcode();*/
	initEventView(Vec2(0, 562), Size(1120, 40));
	//initWebView();

	if (Utils::getSingleton().userDataMe.Name.length() > 0 && Utils::getSingleton().userDataMe.DisplayName.length() == 0) {
		if (popupDisplayName == nullptr) {
			initPopupDisplayName();
		}
		showPopup(popupDisplayName);
	}
	if (Utils::getSingleton().isRunningEvent) {
		runEventView(Utils::getSingleton().events, Utils::getSingleton().currentEventPosX);
	}
    Utils::getSingleton().solveCachedPurchases();

	for (int i = 1; i <= 4; i++) {
		string host = "http://125.212.192.96:8899/cbt/NhaMang/";
		string name1 = "provider" + to_string(i);
		string name2 = name1 + "_dark";
		Utils::getSingleton().LoadTextureFromURL(host + name1 + ".png", [=](Texture2D* texture) {
			if (Utils::getSingleton().currentScene != this) return;
			textures[name1] = texture;
			if (popupCharge != nullptr) {
				if (i == 1) {
					Sprite* sp = (Sprite*)popupCharge->getChildByName("providerimg" + to_string(i));
					sp->initWithTexture(texture);
				}
				if (textures.size() == 8) {
					for (int k = 1; k <= 4; k++) {
						ui::Button* btn = (ui::Button*)popupCharge->getChildByName("btn" + to_string(k));
						btn->setTouchEnabled(true);
					}
				}
			}
		});
		Utils::getSingleton().LoadTextureFromURL(host + name2 + ".png", [=](Texture2D* texture) {
			if (Utils::getSingleton().currentScene != this) return;
			textures[name2] = texture;
			if (popupCharge != nullptr) {
				if (i > 1) {
					Sprite* sp = (Sprite*)popupCharge->getChildByName("providerimg" + to_string(i));
					sp->initWithTexture(texture);
				}
				if (textures.size() == 8) {
					for (int k = 1; k <= 4; k++) {
						ui::Button* btn = (ui::Button*)popupCharge->getChildByName("btn" + to_string(k));
						btn->setTouchEnabled(true);
					}
				}
			}
		});
	}
	scheduleUpdate();
}

void MainScene::registerEventListenner()
{
	BaseScene::registerEventListenner();
	EventHandler::getSingleton().onConfigZoneReceived = std::bind(&MainScene::onConfigZoneReceived, this);
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
}

void MainScene::unregisterEventListenner()
{
	BaseScene::unregisterEventListenner();
	EventHandler::getSingleton().onConnectionLost = NULL;
	EventHandler::getSingleton().onConfigZoneReceived = NULL;
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
}

void MainScene::editBoxReturn(cocos2d::ui::EditBox * editBox)
{
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
	if (isBackToLogin) return;
	if (isGoToLobby && tmpZoneId >= 0) {
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
	if (code == 51 && popupDisplayName->isVisible()) {
		hidePopup(popupDisplayName);
		setDisplayName(tmpDisplayName);
		return;
	}
	if (msg.length() == 0) return;
	showPopupNotice(msg, [=]() {});
}

void MainScene::onJoinRoom(long roomId, std::string roomName)
{

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
		data.CreateDate = "02/12/2016";
		data.UpdateDate = "02/12/2016";
		data.Content = "Viettel 500k";
		data.ItemId = rand() % 4;
		data.Name = "viettel 500k";
		data.Price = 50;
		data.Status = rand() % 4 + 1;
		list.push_back(data);
	}*/

	vector<int> posX = { -360, -210, -15, 180, 344 };
	vector<string> strStatus = { "dat_hang", "xac_nhan", "hoan_tat", "huy", "hoan_tra" };
	ui::ScrollView* scrollHistory = (ui::ScrollView*)(popupShop->getChildByTag(12)->getChildByName("scrollhistory"));
	int heightHistory = list.size() * 30;
	if (heightHistory < scrollHistory->getContentSize().height) {
		heightHistory = scrollHistory->getContentSize().height;
	}
	scrollHistory->setInnerContainerSize(Size(scrollHistory->getContentSize().width, heightHistory));
	for (int i = 0; i < list.size(); i++) {
		int tag = i * 6;
		ui::Button* btn;
		Label *lb1, *lb2, *lb3, *lb4, *lb5;
		lb1 = (Label*)scrollHistory->getChildByTag(tag);
		bool isNewBtn;
		if (lb1 == nullptr) {
			lb1 = Label::create("", "fonts/aurora.ttf", 25);
			lb1->setPosition(posX[0] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
			lb1->setTag(tag);
			scrollHistory->addChild(lb1);

			lb2 = Label::create("", "fonts/aurora.ttf", 25);
			lb2->setPosition(posX[1] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
			lb2->setHorizontalAlignment(TextHAlignment::CENTER);
			lb2->setColor(Color3B::YELLOW);
			lb2->setTag(tag + 1);
			lb2->setWidth(210);
			lb2->setHeight(30);
			scrollHistory->addChild(lb2);

			lb3 = Label::create("", "fonts/aurora.ttf", 25);
			lb3->setPosition(posX[2] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
			lb3->setTag(tag + 2);
			scrollHistory->addChild(lb3);

			lb4 = Label::create("", "fonts/aurora.ttf", 25);
			lb4->setPosition(posX[3] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
			lb4->setTag(tag + 3);
			scrollHistory->addChild(lb4);

			lb5 = Label::create("", "fonts/aurora.ttf", 25);
			lb5->setPosition(posX[4] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
			lb5->setColor(Color3B::YELLOW);
			lb5->setTag(tag + 4);
			scrollHistory->addChild(lb5);

			btn = ui::Button::create("popup/box1.png");
			btn->setContentSize(Size(scrollHistory->getContentSize().width, 25));
			btn->setPosition(Vec2(scrollHistory->getContentSize().width / 2, heightHistory - 10));
			btn->setScale9Enabled(true);
			btn->setOpacity(0);
			btn->setTag(tag + 5);
			scrollHistory->addChild(btn);
			isNewBtn = true;
		} else {
			lb2 = (Label*)scrollHistory->getChildByTag(tag + 1);
			lb3 = (Label*)scrollHistory->getChildByTag(tag + 2);
			lb4 = (Label*)scrollHistory->getChildByTag(tag + 3);
			lb5 = (Label*)scrollHistory->getChildByTag(tag + 4);
			btn = (ui::Button*)scrollHistory->getChildByTag(tag + 5);
			btn->setTouchEnabled(true);
			isNewBtn = false;
		}
		addTouchEventListener(btn, [=]() {
			showPopupNotice(list[i].Content, [=]() {}, false);
		}, isNewBtn);
		if (list[i].Status < 1) {
			list[i].Status = 1;
		} else if (list[i].Status > strStatus.size()) {
			list[i].Status = strStatus.size();
		}
		lb1->setString(to_string((int)list[i].Id));
		lb2->setString(list[i].Name);
		lb3->setString(list[i].CreateDate);
		lb4->setString(list[i].UpdateDate);
		lb5->setString(Utils::getSingleton().getStringForKey(strStatus[list[i].Status - 1]));
		lb1->setPosition(posX[0] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
		lb2->setPosition(posX[1] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
		lb3->setPosition(posX[2] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
		lb4->setPosition(posX[3] + scrollHistory->getContentSize().width / 2, heightHistory - 10);
		lb5->setPosition(posX[4] + scrollHistory->getContentSize().width / 2, heightHistory - 10);

		heightHistory -= 30;
	}
	int count = scrollHistory->getChildrenCount();
	for (int j = list.size() * 6; j < count; j++) {
		if (j % 6 != 5) {
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

	int heightCard = ((cards[0].size() + cards[1].size() + cards[2].size() + cards[3].size() - 1) / 5 + 1) * 120;
	if (heightCard < scrollCard->getContentSize().height) {
		heightCard = scrollCard->getContentSize().height;
	}
	scrollCard->setInnerContainerSize(Size(scrollCard->getContentSize().width, heightCard));

	for (int i = 0; i < cards.size(); i++) {
		for (int j = 0; j < cards[i].size(); j++) {
			string str = Utils::getSingleton().getStringForKey("xac_nhan_mua_vat_pham");
			string strMoney = Utils::getSingleton().formatMoneyWithComma(cards[i][j].PriceChange);
			string msg = String::createWithFormat(str.c_str(), cards[i][j].Name.c_str(), strMoney.c_str())->getCString();

			ui::Button* btn = ui::Button::create("popup/box_shop.png");
			//btn->setPosition(Vec2(80 + j * 165, scrollCard->getContentSize().height - 50 - i * 110));
			btn->setPosition(Vec2(80 + (j % 5) * 165, heightCard - 50 - (i + j/5) * 120));
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

			Sprite* spCoin = Sprite::create("main/icon_gold.png");
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
	int widthItem = (items.size() / 2) * 200;
	if (widthItem < scrollItem->getContentSize().width) {
		widthItem = scrollItem->getContentSize().width;
	}
	scrollItem->setInnerContainerSize(Size(widthItem, scrollItem->getContentSize().height));
	for (int i = 0; i < items.size(); i++) {
		string str = Utils::getSingleton().getStringForKey("xac_nhan_mua_vat_pham");
		string strMoney = Utils::getSingleton().formatMoneyWithComma(moneyItems[i]);
		string msg = String::createWithFormat(str.c_str(), names[items[i]].c_str(), strMoney.c_str())->getCString();

		ui::Button* btn = ui::Button::create("popup/box_shop.png");
		btn->setPosition(Vec2(100 + (i / 2) * 200, scrollItem->getContentSize().height - 70 - (i % 2) * 170));
		btn->setContentSize(Size(182, 128));
		btn->setScale9Enabled(true);
		btn->setBright(false);
		btn->setTag(i);
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

		Sprite* spCoin = Sprite::create("main/icon_gold.png");
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
	showPopupNotice(msg, [=]() {});
}

void MainScene::onNewMailResponse(unsigned char count)
{
	if (count > 0 && Utils::getSingleton().isPaymentEnabled()) {
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

	vector<int> posX = { -390, -290, 0, 340 };
	vector<int> widths = { 50, 100, 420, 120 };
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
				Label* lbDetail = Label::create("", "fonts/arial.ttf", 20);
				lbDetail->setWidth(widths[j]);
				lbDetail->setHeight(46);
				lbDetail->setAnchorPoint(Vec2(.5f, 1));
				lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
				lbDetail->setPosition(posX[j] + scroll->getContentSize().width / 2, height - 5 - i * 60);
				lbDetail->setTag(i * 5 + j);
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
			btn->setTag(i * 5 + 4);
			btn->setName(to_string((int)list[i].Id));
			addTouchEventListener(btn, [=]() {
				Node* nodeDetail = popupMail->getChildByName("nodedetail");
				for (int i = 0; i < 4; i++) {
					Label* lb = (Label*)nodeDetail->getChildByTag(i);
					lb->setString(lbs[i]->getString());
				}
				SFSRequest::getSingleton().RequestMailContent(atoi(btn->getName().c_str()));
			});
			scroll->addChild(btn);
		}
		lbs[0]->setString(to_string((popupMail->getChildByName("nodepage")->getTag() - 1) * 10 + i + 1));
		lbs[1]->setString(list[i].Date);
		lbs[2]->setString(list[i].Title);
		lbs[3]->setString(list[i].Sender);
	}

	int count = scroll->getChildrenCount();
	for (int i = list.size() * 5; i < count; i++) {
		scroll->getChildByTag(i)->setVisible(false);
	}
}

void MainScene::onMailContentResponse(std::string content)
{
	if (content.length() == 0) return;
	Node* nodeDetail = popupMail->getChildByName("nodedetail");
	nodeDetail->setVisible(true);
	Label* lb = (Label*)nodeDetail->getChildByTag(2);
	lb->setString(content);
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

	int heightTitle = list.size() * 50;
	if (heightTitle < scrollTitle->getContentSize().height) {
		heightTitle = scrollTitle->getContentSize().height;
	}
	scrollTitle->setInnerContainerSize(Size(scrollTitle->getContentSize().width, heightTitle));
	for (int i = 0; i < list.size(); i++) {
		string strTitle = list[i].Title.length() < 24 ? list[i].Title : list[i].Title.substr(0, 23);
		int indexTitle = strTitle.find_last_of(' ');
		strTitle = strTitle.substr(0, indexTitle);
		ui::Button* btn = (ui::Button*) scrollTitle->getChildByTag(i);
		bool isNewBtn;
		if (btn == nullptr) {
			isNewBtn = true;
			btn = ui::Button::create();
			btn->setTitleFontName("fonts/arial.ttf");
			btn->setTitleColor(Color3B::BLACK);
			btn->setTitleFontSize(20);
			btn->setContentSize(Size(scrollTitle->getContentSize().width, 60));
			btn->setScale9Enabled(true);
			btn->setTag(i);
			scrollTitle->addChild(btn);
		} else {
			isNewBtn = false;
			btn->setVisible(true);
		}
		btn->loadTextureNormal(i == 0 ? "popup/box2.png" : "popup/box1.png");
		btn->setPosition(Vec2(scrollTitle->getContentSize().width / 2, heightTitle - 25 - i * 50));
		btn->setTitleText(strTitle);
		addTouchEventListener(btn, [=]() {
			ui::Button* btn1 = (ui::Button*)scrollTitle->getChildByTag(scrollTitle->getTag());
			btn1->loadTextureNormal("popup/box1.png");
			btn->loadTextureNormal("popup/box2.png");
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
    hideWaiting();
    if(token.length() > 0){
        SFSRequest::getSingleton().RequestPayment(token);
    }
}

void MainScene::onBackScene()
{
	showWaiting();
	isBackToLogin = true;
	SFSRequest::getSingleton().Disconnect();
	Utils::getSingleton().goToLoginScene();
}

void MainScene::onChangeMoneyType(int type)
{
	BaseScene::onChangeMoneyType(type);
	currentMoneyType = type;
}

void MainScene::initPopupCharge()
{
	vector<int> moneys = { 10, 20, 50, 100, 200, 500 };

	popupCharge = Node::create();
	popupCharge->setPosition(560, 350);
	popupCharge->setVisible(false);
	popupCharge->setTag(0);
	mLayer->addChild(popupCharge, constant::ZORDER_POPUP);
	//autoScaleNode(popupCharge);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupCharge->addChild(bg);

	Sprite* title = Sprite::create("popup/title_naptien.png");
	title->setPosition(0, 250);
	popupCharge->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(390, 250));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupCharge);
	});
	popupCharge->addChild(btnClose);

	Node* nodeCard = Node::create();
	nodeCard->setName("nodecard");
	nodeCard->setVisible(true);
	popupCharge->addChild(nodeCard);

	Node* nodeSms = Node::create();
	nodeSms->setName("nodesms");
	nodeSms->setVisible(false);
	popupCharge->addChild(nodeSms);
    
    Node* nodeStore = Node::create();
    nodeStore->setName("nodestore");
    nodeStore->setVisible(false);
    popupCharge->addChild(nodeStore);

	ui::Button* btnCard = ui::Button::create("popup/box2.png");
    ui::Button* btnSms = ui::Button::create("popup/box1.png");
    ui::Button* btnStore = ui::Button::create("popup/box1.png");

	btnCard->setPosition(Vec2(-270, 160));
	btnCard->setScale9Enabled(true);
	btnCard->setContentSize(Size(250, 80));
	addTouchEventListener(btnCard, [=]() {
		if (popupCharge->getTag() == 0) return;
		popupCharge->setTag(0);
		nodeCard->setVisible(true);
        nodeSms->setVisible(false);
        nodeStore->setVisible(false);
		btnCard->loadTextureNormal("popup/box2.png");
        btnSms->loadTextureNormal("popup/box1.png");
        btnStore->loadTextureNormal("popup/box1.png");
        for(int i=1;i<=4;i++){
            popupCharge->getChildByName("btn" + to_string(i))->setVisible(true);
            popupCharge->getChildByName("providerimg" + to_string(i))->setVisible(true);
        }
	});
	popupCharge->addChild(btnCard);

	Label* lbTitleBtnCard = Label::create(Utils::getSingleton().getStringForKey("nap_the"), "fonts/guanine.ttf", 35);
	lbTitleBtnCard->setPosition(btnCard->getContentSize().width / 2, btnCard->getContentSize().height / 2 + 7);
	lbTitleBtnCard->setColor(Color3B::BLACK);
	btnCard->addChild(lbTitleBtnCard);

	btnSms->setPosition(Vec2(0, 160));
	btnSms->setScale9Enabled(true);
	btnSms->setContentSize(Size(250, 80));
	addTouchEventListener(btnSms, [=]() {
		if (popupCharge->getTag() == 1) return;
		popupCharge->setTag(1);
		nodeCard->setVisible(false);
		nodeSms->setVisible(true);
        nodeStore->setVisible(false);
		btnCard->loadTextureNormal("popup/box1.png");
		btnSms->loadTextureNormal("popup/box2.png");
        btnStore->loadTextureNormal("popup/box1.png");
        for(int i=1;i<=3;i++){
            popupCharge->getChildByName("btn" + to_string(i))->setVisible(true);
            popupCharge->getChildByName("providerimg" + to_string(i))->setVisible(true);
        }

		int btnIndex = -1;
		Node* btn4 = popupCharge->getChildByName("btn4");
		Sprite* img4 = (Sprite*)popupCharge->getChildByName("providerimg4");
		if (btn4->getTag() == 1) {
			Node* btn1 = popupCharge->getChildByName("btn1");
			Sprite* img1 = (Sprite*)popupCharge->getChildByName("providerimg1");
			btn1->setTag(1);
			img1->initWithTexture(textures["provider1"]);
			btnIndex = 1;
		}
		btn4->setVisible(false);
		img4->setVisible(false);
		btn4->setTag(0);
		img4->initWithTexture(textures["provider4_dark"]);

		if (btnIndex == -1) {
			for (int i = 1; i < 4; i++) {
				ui::Button* btn = (ui::Button*)popupCharge->getChildByName("btn" + to_string(i));
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

		ui::ScrollView* scroll = (ui::ScrollView*)nodeSms->getChildByName("scrollsms");
		for (int i = 0; i < moneys.size(); i++) {
			string strMoney = to_string(moneys[i]);
			string smsStr = Utils::getSingleton().replaceString(smsct, "vnd", strMoney);

			Node* node = scroll->getChildByName(strMoney);
			Label* lbContent = (Label*)node->getChildByName("lbsmscontent");
			Label* lbTarget = (Label*)node->getChildByName("lbsmstarget");
			lbContent->setString(smsStr);
			lbTarget->setString(smstg);
		}
	});
	popupCharge->addChild(btnSms);

	Label* lbTitleBtnSms = Label::create(Utils::getSingleton().getStringForKey("nap_sms"), "fonts/guanine.ttf", 35);
	lbTitleBtnSms->setPosition(btnSms->getContentSize().width / 2, btnSms->getContentSize().height / 2 + 7);
	lbTitleBtnSms->setColor(Color3B::BLACK);
	btnSms->addChild(lbTitleBtnSms);
    
    btnStore->setPosition(Vec2(270, 160));
    btnStore->setScale9Enabled(true);
    btnStore->setContentSize(Size(250, 80));
    addTouchEventListener(btnStore, [=]() {
        if (popupCharge->getTag() == 2) return;
        popupCharge->setTag(2);
        nodeCard->setVisible(false);
        nodeSms->setVisible(false);
        nodeStore->setVisible(true);
        btnCard->loadTextureNormal("popup/box1.png");
        btnSms->loadTextureNormal("popup/box1.png");
        btnStore->loadTextureNormal("popup/box2.png");
        for(int i=1;i<=4;i++){
            popupCharge->getChildByName("btn" + to_string(i))->setVisible(false);
            popupCharge->getChildByName("providerimg" + to_string(i))->setVisible(false);
        }
    });
    popupCharge->addChild(btnStore);
    
    Label* lbTitleBtnStore = Label::create(Utils::getSingleton().getStringForKey("nap_iap"), "fonts/guanine.ttf", 35);
    lbTitleBtnStore->setPosition(btnStore->getContentSize().width / 2, btnStore->getContentSize().height / 2 + 7);
    lbTitleBtnStore->setColor(Color3B::BLACK);
    btnStore->addChild(lbTitleBtnStore);

	int x = -300;
	vector<string> strProviders = { "viettel", "mobifone", "vinaphone", "megacard" };

	string smsContent = Utils::getSingleton().gameConfig.smsVT;
	int strIndex = smsContent.find_last_of(' ');
	string smsTarget = smsContent.substr(strIndex + 1, smsContent.length() - strIndex);
	smsContent = smsContent.substr(0, strIndex);
	smsContent = Utils::getSingleton().replaceString(smsContent, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
	for (int i = 1; i <= strProviders.size(); i++) {
		string stri = to_string(i);
		Sprite* sp = Sprite::create();
		if (textures.size() == 8) {
			sp->initWithTexture(textures["provider" + stri + (i > 1 ? "_dark" : "")]);
		}

		//ui::Button* btnProvider = ui::Button::create("popup/provider" + stri + (i > 1 ? "_dark" : "") + ".png");
		ui::Button* btnProvider = ui::Button::create();
		btnProvider->loadTextureNormal("popup/box5.png");
		btnProvider->setContentSize(Size(200, 130));
		btnProvider->setScale9Enabled(true);
		btnProvider->setPosition(Vec2(x, 40));
		btnProvider->setTag(i == 1 ? 1 : 0);
		btnProvider->setName("btn" + stri);
		btnProvider->setBright(false);
		btnProvider->setTouchEnabled(textures.size() == 8);
		btnProvider->setScale(.9f);
		addTouchEventListener(btnProvider, [=]() {
			if (btnProvider->getTag() == 1) return;
			int btnIndex;
			for (int j = 1; j <= strProviders.size(); j++) {
				string strj = to_string(j);
				Sprite* img = (Sprite*)popupCharge->getChildByName("providerimg" + strj);
				ui::Button* btn = (ui::Button*)popupCharge->getChildByName("btn" + strj);
				if (btn != btnProvider) {
					btn->setTag(0);
					img->initWithTexture(textures["provider" + strj + "_dark"]);
				} else {
					btn->setTag(1);
					btnIndex = i;
				}
			}
			sp->initWithTexture(textures["provider" + stri]);

			if (i < strProviders.size() && nodeSms->isVisible()) {
				string smsct = btnIndex == 1 ? Utils::getSingleton().gameConfig.smsVT : Utils::getSingleton().gameConfig.smsVNPVMS;
				int strid = smsct.find_last_of(' ');
				string smstg = smsct.substr(strid + 1, smsct.length() - strid);
				smsct = smsct.substr(0, strid);
				smsct = Utils::getSingleton().replaceString(smsct, "uid", to_string(Utils::getSingleton().userDataMe.UserID));

				ui::ScrollView* scroll = (ui::ScrollView*)nodeSms->getChildByName("scrollsms");
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
		});
		popupCharge->addChild(btnProvider);

		sp->setPosition(btnProvider->getPosition());
		sp->setName("providerimg" + stri);
		sp->setScale(.9f);
		popupCharge->addChild(sp);

		x += 200;
	}

	//Node Card
	Node* nodeCardInfo = Node::create();
	nodeCardInfo->setPosition(-280, -140);
	nodeCard->addChild(nodeCardInfo);

	ui::Scale9Sprite* bgCardInfo = ui::Scale9Sprite::create("popup/box.png");
	bgCardInfo->setContentSize(Size(230, 200));
	bgCardInfo->setOpacity(100);
	nodeCardInfo->addChild(bgCardInfo);

	ui::ScrollView* scrollInfo = ui::ScrollView::create();
	scrollInfo->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollInfo->setBounceEnabled(true);
	scrollInfo->setPosition(Vec2(-bgCardInfo->getContentSize().width / 2 + 5, -bgCardInfo->getContentSize().height / 2 + 5));
	scrollInfo->setContentSize(bgCardInfo->getContentSize() - Size(10, 10));
	scrollInfo->setScrollBarEnabled(false);
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

			Label* lb1 = Label::create(str1, "fonts/aurora.ttf", 24);
			lb1->setAnchorPoint(Vec2(1, .5f));
			lb1->setPosition(scrollInfo->getContentSize().width/2, height - 15);
			scrollInfo->addChild(lb1);

			Label* lb2 = Label::create(str2, "fonts/aurora.ttf", 25);
			lb2->setAnchorPoint(Vec2(0, .5f));
			lb2->setPosition(scrollInfo->getContentSize().width/2, height - 15);
			lb2->setColor(Color3B::YELLOW);//i == 0 ? Color3B::YELLOW : Color3B(0, 255, 255));
			scrollInfo->addChild(lb2);

			height -= 30;
		}
	//}

	Node* nodeInput = Node::create();
	nodeInput->setPosition(120, -140);
	nodeCard->addChild(nodeInput);

	ui::Scale9Sprite* bgInput = ui::Scale9Sprite::create("popup/box.png");
	bgInput->setContentSize(Size(540, 200));
	bgInput->setOpacity(100);
	nodeInput->addChild(bgInput);

	Label* lbseri = Label::create(Utils::getSingleton().getStringForKey("so_seri"), "fonts/guanine.ttf", 30);
	lbseri->setAnchorPoint(Vec2(0, .5f));
	lbseri->setPosition(-240, 60);
	nodeInput->addChild(lbseri);

	Label* lbcode = Label::create(Utils::getSingleton().getStringForKey("ma_the"), "fonts/guanine.ttf", 30);
	lbcode->setAnchorPoint(Vec2(0, .5f));
	lbcode->setPosition(-240, 10);
	nodeInput->addChild(lbcode);

	ui::EditBox* tfSeri = ui::EditBox::create(Size(350, 55), "popup/box.png");
	tfSeri->setPosition(Vec2(60, 60));
	tfSeri->setFontName("Arial");
	tfSeri->setFontSize(30);
	tfSeri->setFontColor(Color3B::WHITE);
	tfSeri->setMaxLength(24);
	tfSeri->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfSeri->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfSeri->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfSeri->setDelegate(this);
	nodeInput->addChild(tfSeri);

	ui::EditBox* tfCode = ui::EditBox::create(Size(350, 55), "popup/box.png");
	tfCode->setPosition(Vec2(60, 0));
	tfCode->setFontName("Arial");
	tfCode->setFontSize(30);
	tfCode->setFontColor(Color3B::WHITE);
	tfCode->setMaxLength(24);
	tfCode->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfCode->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfCode->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
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

	ui::Button* btnCharge = ui::Button::create("popup/btn.png", "popup/btn_clicked.png");
	btnCharge->setPosition(Vec2(40, -65));
	btnCharge->setContentSize(Size(140, 55));
	btnCharge->setScale9Enabled(true);
	addTouchEventListener(btnCharge, [=]() {
		string code = tfCode->getText();
		string seri = tfSeri->getText();
		if (code.length() == 0 || seri.length() == 0) return;
		int providerId;
		for (int i = 1; i <= 4; i++) {
			Node* n = popupCharge->getChildByName("btn" + to_string(i));
			if (n->getTag() == 1) {
				providerId = i - 1;
				break;
			}
		}
		string provider = strProviders[providerId];
		SFSRequest::getSingleton().RequestChargeCard(code, seri, provider);
		showWaiting();
	});
	nodeInput->addChild(btnCharge);

	Label* lbTitleBtnCharge = Label::create(Utils::getSingleton().getStringForKey("nap"), "fonts/guanine.ttf", 30);
	lbTitleBtnCharge->setPosition(btnCharge->getContentSize().width / 2, btnCharge->getContentSize().height / 2 + 8);
	lbTitleBtnCharge->setColor(Color3B::BLACK);
	btnCharge->addChild(lbTitleBtnCharge);

	//Node SMS
	ui::Scale9Sprite* bgSms = ui::Scale9Sprite::create("popup/box.png");
	bgSms->setPosition(0, -140);
	bgSms->setContentSize(Size(800, 200));
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

			Node* itemSms = Node::create();
			itemSms->setPosition(125 + i * 250, 100);
			itemSms->setName(strMoney);
			scrollSms->addChild(itemSms);

			ui::Scale9Sprite* bgItemSms = ui::Scale9Sprite::create("popup/box3.png");
			bgItemSms->setContentSize(Size(240, 170));
			itemSms->addChild(bgItemSms);

			Label* lbItemSms1 = Label::create("SMS " + strMoney + "k = ", "fonts/aurora.ttf", 25);
			lbItemSms1->setAnchorPoint(Vec2(1, .5f));
			lbItemSms1->setPosition(5, 60);
			itemSms->addChild(lbItemSms1);

			Label* lbItemSms2 = Label::create(strMoney2 + "k Quan", "fonts/aurora.ttf", 25);
			lbItemSms2->setAnchorPoint(Vec2(0, .5f));
			lbItemSms2->setColor(Color3B::YELLOW);
			lbItemSms2->setPosition(5, 60);
			itemSms->addChild(lbItemSms2);

			Label* lbItemSms3 = Label::create(smsStr, "fonts/aurora.ttf", 25);
			lbItemSms3->setPosition(0, 30);
			lbItemSms3->setName("lbsmscontent");
			itemSms->addChild(lbItemSms3);

			Label* lbItemSms4 = Label::create(smsTarget, "fonts/aurora.ttf", 25);
			lbItemSms4->setPosition(0, 0);
			lbItemSms4->setName("lbsmstarget");
			itemSms->addChild(lbItemSms4);

			ui::Button* btnItemSms = ui::Button::create("popup/btn.png", "popup/btn_clicked.png");
			btnItemSms->setPosition(Vec2(0, -50));
			btnItemSms->setContentSize(Size(150, 58));
			btnItemSms->setScale9Enabled(true);
			addTouchEventListener(btnItemSms, [=]() {
				int btnIndex;
				for (int i = 1; i < 4; i++) {
					ui::Button* btn = (ui::Button*)popupCharge->getChildByName("btn" + to_string(i));
					if (btn->getTag() == 1) {
						btnIndex = i;
					}
				}

				string smsct = btnIndex == 1 ? Utils::getSingleton().gameConfig.smsVT : Utils::getSingleton().gameConfig.smsVNPVMS;
				int strid = smsct.find_last_of(' ');
				string smstg = smsct.substr(strid + 1, smsct.length() - strid);
				smsct = smsct.substr(0, strid);
				string strMoney = to_string(moneys[i]);
				string smsStr = Utils::getSingleton().replaceString(smsct, "vnd", strMoney);
				smsStr = Utils::getSingleton().replaceString(smsStr, "uid", to_string(Utils::getSingleton().userDataMe.UserID));
				Utils::getSingleton().openSMS(smstg, smsStr);
			});
			itemSms->addChild(btnItemSms);

			Label* lbTitleBtnItemSms = Label::create(Utils::getSingleton().getStringForKey("nap"), "fonts/guanine.ttf", 25);
			lbTitleBtnItemSms->setPosition(btnItemSms->getContentSize().width / 2, btnItemSms->getContentSize().height / 2 + 5);
			lbTitleBtnItemSms->setColor(Color3B::BLACK);
			btnItemSms->addChild(lbTitleBtnItemSms);
		}
	//}
    
    //Nap CH
    vector<ProductData> products = Utils::getSingleton().products;
    Size storeSize = Size(720, 220);
    
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
        
        ui::Button* btn = ui::Button::create("popup/box_shop.png");
        btn->setPosition(Vec2(120 + i * 240, storeSize.height/2));
        btn->setContentSize(Size(182, 150));
        btn->setScale9Enabled(true);
        btn->setBright(false);
        btn->setTag(i);
        addTouchEventListener(btn, [=]() {
            showWaiting(300);
            Utils::getSingleton().purchaseItem(strId);
        });
        scrollStore->addChild(btn);
        
        Sprite* sp = Sprite::create("main/icon_charge.png");
        sp->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 + 15);
        sp->setName("itemimage");
        btn->addChild(sp);
        
        Sprite* spCoin = Sprite::create("main/icon_gold.png");
        spCoin->setScale(.5f);
        btn->addChild(spCoin);
        
        Label* lb1 = Label::create(strValue, "fonts/guanine.ttf", 20);
        lb1->setPosition(btn->getContentSize().width / 2 - spCoin->getContentSize().width * spCoin->getScale() / 2, btn->getContentSize().height / 2 - 55);
        lb1->setColor(Color3B::YELLOW);
        btn->addChild(lb1);
        
        Label* lb2 = Label::create(strCost, "fonts/guanine.ttf", 20);
        lb2->setWidth(175);
        lb2->setHeight(30);
        lb2->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2 - 90);
        lb2->setColor(Color3B::WHITE);
        lb2->setHorizontalAlignment(TextHAlignment::CENTER);
        btn->addChild(lb2);
        
        spCoin->setPosition(lb1->getPositionX() + lb1->getContentSize().width / 2
                            + spCoin->getContentSize().width * spCoin->getScale() / 2 + 5, lb1->getPositionY() - 3);
    }
    
    bool paymentEnabled = Utils::getSingleton().isPaymentEnabled();
    if(!paymentEnabled){
        btnCard->setVisible(false);
        btnSms->setVisible(false);
        btnStore->setVisible(false);
        nodeCard->setVisible(false);
        nodeStore->setVisible(true);
        nodeStore->setPositionY(0);
        for(int i=1;i<=4;i++){
            popupCharge->getChildByName("btn" + to_string(i))->setVisible(false);
            popupCharge->getChildByName("providerimg" + to_string(i))->setVisible(false);
        }
    }else{
        nodeStore->setPositionY(-30);
    }
	if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS) {
		btnStore->setVisible(false);
	}
}

void MainScene::initPopupGuide()
{
	popupGuide = Node::create();
	popupGuide->setPosition(560, 350);
	popupGuide->setVisible(false);
	popupGuide->setTag(0);
	mLayer->addChild(popupGuide, constant::ZORDER_POPUP);
	//autoScaleNode(popupGuide);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupGuide->addChild(bg);

	Sprite* title = Sprite::create("popup/title_huongdan.png");
	title->setPosition(0, 250);
	popupGuide->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(390, 250));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupGuide);
	});
	popupGuide->addChild(btnClose);

	int w = 800;
	ui::Scale9Sprite* bgScroll = ui::Scale9Sprite::create("popup/box1.png");
	bgScroll->setContentSize(Size(w + 50, 370));
	bgScroll->setPosition(0, -60);
	popupGuide->addChild(bgScroll);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-w/2, -227));
	scroll->setContentSize(Size(w, 330));
	scroll->setScrollBarEnabled(false);
	popupGuide->addChild(scroll);

	cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
	Label* lb = Label::create(plist["tutorial_1"].asString(), "fonts/arial.ttf", 20);
	lb->setAnchorPoint(Vec2(0, 1));
	lb->setColor(Color3B::BLACK);
	lb->setName("lbcontent");
	lb->setWidth(w);
	scroll->addChild(lb);

	int height = lb->getContentSize().height;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	lb->setPosition(0, height);
	scroll->setInnerContainerSize(Size(w, height));

	vector<string> texts = { "noi_quy" , "cuoc_u", "tinh_loi", "tinh_diem" };
	int x = -315;
	for (int i = 0; i < 4; i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "popup/box2.png" : "popup/box1.png");
		btn->setTitleText(Utils::getSingleton().getStringForKey(texts[i]));
		btn->setTitleFontName("fonts/guanine.ttf");
		btn->setTitleFontSize(25);
		btn->setTitleColor(Color3B::BLACK);
		btn->setContentSize(Size(210, 70));
		btn->setPosition(Vec2(x, 160));
		btn->setScale9Enabled(true);
		btn->setCapInsets(Rect(25, 25, 0, 0));
		btn->setTag(i); 
		addTouchEventListener(btn, [=]() {
			ui::Button* btn1 = (ui::Button*)popupGuide->getChildByTag(popupGuide->getTag());
			cocos2d::ValueMap plist = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/tutorials.xml");
			string content = plist["tutorial_" + to_string(btn->getTag() + 1)].asString();

			btn1->loadTextureNormal("popup/box1.png");
			btn->loadTextureNormal("popup/box2.png");
			lb->setString(content);
			popupGuide->setTag(btn->getTag());

			int height = lb->getContentSize().height;
			if (height < scroll->getContentSize().height) {
				height = scroll->getContentSize().height;
			}
			lb->setPosition(0, height);
			scroll->setInnerContainerSize(Size(w, height));
		});
		popupGuide->addChild(btn);
		x += 210;
	}
}

void MainScene::initPopupMail()
{
	popupMail = Node::create();
	popupMail->setPosition(560, 350);
	popupMail->setVisible(false);
	mLayer->addChild(popupMail, constant::ZORDER_POPUP);
	//autoScaleNode(popupMail);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupMail->addChild(bg);

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::create("popup/box1.png");
	bgContent->setContentSize(Size(860, 420));
	bgContent->setPosition(0, -25);
	popupMail->addChild(bgContent);

	Sprite* title = Sprite::create("popup/title_hopthu.png");
	title->setPosition(0, 250);
	title->setName("sptitle");
	popupMail->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(390, 250));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupMail);
	});
	popupMail->addChild(btnClose);

	int w = 850;
	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-w / 2, -222));
	scroll->setContentSize(Size(w, 360));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	scroll->setTag(1);
	popupMail->addChild(scroll);

	vector<int> posX = { -390, -290, 0, 340 };
	vector<int> widths = { 50, 100, 420, 120 };
	vector<string> historyTitles = { "STT", Utils::getSingleton().getStringForKey("ngay"), 
		Utils::getSingleton().getStringForKey("thong_tin"), Utils::getSingleton().getStringForKey("nguoi_gui") };
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lb = Label::create(historyTitles[i], "fonts/aurora.ttf", 30);
		lb->setColor(Color3B(50, 50, 50));
		lb->setPosition(posX[i], bgContent->getPositionY() + bgContent->getContentSize().height / 2 - 25);
		popupMail->addChild(lb);
	}

	Node* nodeDetail = Node::create();
	nodeDetail->setPosition(0, -45);
	nodeDetail->setVisible(false);
	nodeDetail->setName("nodedetail");
	popupMail->addChild(nodeDetail);

	ui::Scale9Sprite* bgDetail = ui::Scale9Sprite::create("popup/box1.png");
	bgDetail->setContentSize(Size(860, 380));
	nodeDetail->addChild(bgDetail);

	for (int i = 0; i < posX.size(); i++) {
		Label* lbDetail = Label::create("", "fonts/arial.ttf", 20);
		lbDetail->setWidth(widths[i]);
		lbDetail->setAnchorPoint(Vec2(.5f, 1));
		lbDetail->setHorizontalAlignment(TextHAlignment::CENTER);
		lbDetail->setPosition(posX[i], bgDetail->getContentSize().height / 2 - 12);
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

	addBtnChoosePage(-100, -260, popupMail, [=](int page) {
		SFSRequest::getSingleton().RequestListMail(page);
		//onListMailDataResponse(vector<MailData>());
	});
}

void MainScene::initPopupNews()
{
	popupNews = Node::create();
	popupNews->setPosition(560, 350);
	popupNews->setVisible(false);
	popupNews->setTag(1);
	mLayer->addChild(popupNews, constant::ZORDER_POPUP);
	//autoScaleNode(popupNews);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupNews->addChild(bg);

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::create("popup/box1.png");
	bgContent->setContentSize(Size(630, 420));
	bgContent->setPosition(110, -25);
	bgContent->setOpacity(200);
	popupNews->addChild(bgContent);

	Sprite* title = Sprite::create("popup/title_tintuc.png");
	title->setPosition(0, 250);
	title->setName("sptitle");
	popupNews->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(390, 250));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupNews);
	});
	popupNews->addChild(btnClose);

	ui::ScrollView* scrollContent = ui::ScrollView::create();
	scrollContent->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollContent->setBounceEnabled(true);
	scrollContent->setPosition(Vec2(bgContent->getPositionX() - bgContent->getContentSize().width / 2 + 20, bgContent->getPositionY() - bgContent->getContentSize().height / 2 + 20));
	scrollContent->setContentSize(Size(bgContent->getContentSize().width - 40, bgContent->getContentSize().height - 40));
	scrollContent->setScrollBarEnabled(false);
	scrollContent->setName("scrollcontent");
	popupNews->addChild(scrollContent);

	Label* lbContent = Label::create("", "fonts/arial.ttf", 20);
	lbContent->setWidth(scrollContent->getContentSize().width);
	lbContent->setAnchorPoint(Vec2(0, 1));
	lbContent->setColor(Color3B::BLACK);
	lbContent->setName("lbcontent");
	scrollContent->addChild(lbContent);

	ui::ScrollView* scrollTitle = ui::ScrollView::create();
	scrollTitle->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollTitle->setBounceEnabled(true);
	scrollTitle->setPosition(Vec2(-430 , bgContent->getPositionY() - bgContent->getContentSize().height / 2 + 7));
	scrollTitle->setContentSize(Size(220, bgContent->getContentSize().height - 14));
	scrollTitle->setScrollBarEnabled(false);
	scrollTitle->setName("scrolltitle");
	scrollTitle->setTag(0);
	popupNews->addChild(scrollTitle);

	addBtnChoosePage(-100, -260, popupNews, [=](int page) {
		scrollTitle->setTag(0);
		SFSRequest::getSingleton().RequestNews(page);
		//onNewsDataResponse(vector<NewsData>());
	});
}

void MainScene::initPopupShop()
{
	popupShop = Node::create();
	popupShop->setPosition(560, 350);
	popupShop->setVisible(false);
	popupShop->setTag(0);
	mLayer->addChild(popupShop, constant::ZORDER_POPUP);
	//autoScaleNode(popupShop);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(1000, 700));
	popupShop->addChild(bg);

	ui::Scale9Sprite* bgContent = ui::Scale9Sprite::create("popup/box.png");
	bgContent->setContentSize(Size(830, 350));
	bgContent->setPosition(0, -50);
	bgContent->setOpacity(200);
	popupShop->addChild(bgContent);

	Sprite* title = Sprite::create("popup/title_cuahang.png");
	title->setPosition(0, 250);
	title->setName("sptitle");
	popupShop->addChild(title);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(390, 250));
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupShop);
	});
	popupShop->addChild(btnClose);

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
	popupShop->addChild(nodeHistory);

	int x = -300;
	vector<string> texts = { "the_cao" , "vat_pham", "lich_su" };
	for (int i = 0; i < texts.size(); i++) {
		ui::Button* btn = ui::Button::create(i == 0 ? "popup/box4.png" : "");
		btn->setContentSize(Size(200, 70));
		btn->setPosition(Vec2(x, 160));
		btn->setOpacity(200);
		btn->setScale9Enabled(true);
		btn->setBright(false);
		btn->setTag(i);
		addTouchEventListener(btn, [=]() {
			if (btn->getTag() == popupShop->getTag()) return;
			Node* node = popupShop->getChildByTag(btn->getTag() + 10);
			Node* node1 = popupShop->getChildByTag(popupShop->getTag() + 10);
			ui::Button* btn1 = (ui::Button*)popupShop->getChildByTag(popupShop->getTag());

			node1->setVisible(false);
			node->setVisible(true);
			btn1->loadTextureNormal("");
			btn->loadTextureNormal("popup/box4.png");
			popupShop->setTag(btn->getTag());

			if (i == 2) {
				SFSRequest::getSingleton().RequestShopHistory(1);
			}
		});
		popupShop->addChild(btn);

		Label* lbbtn = Label::create(Utils::getSingleton().getStringForKey(texts[i]), "fonts/guanine.ttf", 30);
		lbbtn->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
		btn->addChild(lbbtn);

		x += 200;
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
	scrollItem->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollItem->setBounceEnabled(true);
	scrollItem->setPosition(Vec2(bgContent->getPositionX() - bgContent->getContentSize().width / 2, bgContent->getPositionY() - bgContent->getContentSize().height / 2 + 5));
	scrollItem->setContentSize(Size(bgContent->getContentSize().width, bgContent->getContentSize().height - 10));
	scrollItem->setScrollBarEnabled(false);
	scrollItem->setName("scrollitem");
	nodeItem->addChild(scrollItem);

	vector<int> posX = { -360, -210, -15, 180, 344 };
	vector<int> widths = { 120, 210, 210, 210, 150 };
	vector<string> historyTitles = { "ma_dt", "san_pham_doi", "thoi_gian_doi", "thoi_gian_nhan", "trang_thai" };
	for (int i = 0; i < historyTitles.size(); i++) {
		Label* lb = Label::create(Utils::getSingleton().getStringForKey(historyTitles[i]), "fonts/guanine.ttf", 20);
		lb->setColor(Color3B(50, 50, 50));
		lb->setPosition(posX[i], 98);
		nodeHistory->addChild(lb);

		ui::Scale9Sprite* bgTitle = ui::Scale9Sprite::create("popup/box1.png");
		bgTitle->setContentSize(Size(widths[i], 60));
		bgTitle->setPosition(lb->getContentSize().width / 2, lb->getContentSize().height / 2);
		lb->addChild(bgTitle, -1);
	}
	
	ui::ScrollView* scrollHistory = ui::ScrollView::create();
	scrollHistory->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollHistory->setBounceEnabled(true);
	scrollHistory->setPosition(Vec2(bgContent->getPositionX() - bgContent->getContentSize().width / 2, bgContent->getPositionY() - bgContent->getContentSize().height / 2 + 5));
	scrollHistory->setContentSize(Size(bgContent->getContentSize().width, bgContent->getContentSize().height - 60));
	scrollHistory->setScrollBarEnabled(false);
	scrollHistory->setName("scrollhistory");
	nodeHistory->addChild(scrollHistory);

	addBtnChoosePage(-100, -260, nodeHistory, [=](int page) {
		SFSRequest::getSingleton().RequestShopHistory(page);
		//onShopHistoryDataResponse(vector<ShopHistoryData>());
	});
}
void MainScene::initWebView()
{
	nodeWebview = Node::create();
	nodeWebview->setPosition(560, 350);
	nodeWebview->setVisible(false);
	mLayer->addChild(nodeWebview, constant::ZORDER_POPUP);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(500, 310));
	btnClose->setScale(.5f);
	addTouchEventListener(btnClose, [=]() {
		hideSplash();
		nodeWebview->setVisible(false);
		nodeWebview->removeChildByName("webview");
	});
	nodeWebview->addChild(btnClose);
	autoScaleNode(btnClose);
}

void MainScene::initPopupGiftcode()
{
	popupGiftcode = Node::create();
	popupGiftcode->setPosition(560, 350);
	popupGiftcode->setVisible(false);
	mLayer->addChild(popupGiftcode, constant::ZORDER_POPUP);
	//autoScaleNode(popupGiftcode);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(800, 400));
	bg->setScale(.8f);
	popupGiftcode->addChild(bg);

	Sprite* title = Sprite::create("popup/title_giftcode.png");
	title->setPosition(0, 110);
	title->setScale(.8f);
	popupGiftcode->addChild(title);

	ui::EditBox* tfGiftcode = ui::EditBox::create(Size(340, 55), "login/box.png");
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
	lb->setPosition(0, 30);
	popupGiftcode->addChild(lb);

	ui::Button* btnSubmit = ui::Button::create("popup/btn_submit.png", "popup/btn_submit_clicked.png");
	btnSubmit->setPosition(Vec2(0, -105));
	addTouchEventListener(btnSubmit, [=]() {
		string code = Utils::getSingleton().trim(tfGiftcode->getText());
		if (code.length() > 0) {
			SFSRequest::getSingleton().RequestGiftcode(code);
			tfGiftcode->setText("");
			//hidePopup(popupGiftcode);
		}
	});
	popupGiftcode->addChild(btnSubmit);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(240, 110));
	btnClose->setScale(.7f);
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupGiftcode);
	});
	popupGiftcode->addChild(btnClose);
}

void MainScene::initPopupDisplayName()
{
	popupDisplayName = Node::create();
	popupDisplayName->setPosition(560, 350);
	popupDisplayName->setVisible(false);
	mLayer->addChild(popupDisplayName, constant::ZORDER_POPUP);
	//autoScaleNode(popupDisplayName);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setInsetBottom(0);
	bg->setInsetTop(0);
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setContentSize(Size(800, 400));
	bg->setScale(.8f);
	popupDisplayName->addChild(bg);

	ui::EditBox* tfDisplayName = ui::EditBox::create(Size(340, 55), "login/box.png");
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

	ui::Button* btnSubmit = ui::Button::create("popup/btn_submit.png", "popup/btn_submit_clicked.png");
	btnSubmit->setPosition(Vec2(0, -95));
	addTouchEventListener(btnSubmit, [=]() {
		tmpDisplayName = Utils::getSingleton().trim(tfDisplayName->getText());
		if (Utils::getSingleton().isDisplayNameValid(tmpDisplayName)) {
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
	}
	showPopup(popupMail);
	SFSRequest::getSingleton().RequestListMail(1);
	lbNewMail->getParent()->setVisible(false);

	popupMail->getChildByName("nodepage")->setTag(1);
	for (int i = 1; i <= 5; i++) {
		ui::Button* btn = (ui::Button*)popupMail->getChildByTag(1000 + i);
		btn->setTitleText(to_string(i));
		if (i == 1) {
			btn->setColor(pageColor2);
		} else {
			btn->setColor(pageColor1);
		}
	}
}

void MainScene::showPopupNews()
{
	if (popupNews == nullptr) {
		initPopupNews();
	}
	showPopup(popupNews);
	SFSRequest::getSingleton().RequestNews(0);

	ui::ScrollView* scrollTitle = (ui::ScrollView*)popupNews->getChildByName("scrolltitle");
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
	}
}

void MainScene::showWebView(std::string url)
{
	showSplash();
	nodeWebview->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
	Size  winSize = Director::getInstance()->getVisibleSize();
	auto webView = experimental::ui::WebView::create();
	webView->setContentSize(Size(winSize.width, winSize.height * 0.9));
	webView->setScalesPageToFit(true);
	webView->loadURL(url);
	webView->setName("webview");
	nodeWebview->addChild(webView);
#endif
}
