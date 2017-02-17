#pragma once
#include "GameScene.h"
#include "Constant.h"
#include "Data.h"
#include "SFSRequest.h"
#include "UserNode.h"
#include "Utils.h"
#include "EventHandler.h"
#include "AudioEngine.h"

using namespace std;
using namespace cocos2d;

void GameScene::onInit()
{
	BaseScene::onInit();

	UserDefault::getInstance()->setBoolForKey(constant::KEY_AUTO_READY.c_str(), false);

	state = NONE;
	myServerSlot = -1;
	timeTurn = 0;
	Size winSize = Director::getInstance()->getWinSize();

	dealPos.push_back(Vec2(-120, -170));
	dealPos.push_back(Vec2(120, -170));
	dealPos.push_back(Vec2(180, 15));
	dealPos.push_back(Vec2(0, 155));
	dealPos.push_back(Vec2(-180, 15));

	/*handPos.push_back(Vec2(560, 240));
	handPos.push_back(Vec2(720, 350));
	handPos.push_back(Vec2(560, 460));
	handPos.push_back(Vec2(400, 350));*/

	handPos.push_back(Vec2(560, 220));
	handPos.push_back(Vec2(690, 350));
	handPos.push_back(Vec2(560, 480));
	handPos.push_back(Vec2(430, 350));

	tableCardPos.push_back(Vec2(860, 150));
	tableCardPos.push_back(Vec2(30, 125));
	tableCardPos.push_back(Vec2(970, 320));
	tableCardPos.push_back(Vec2(1090, 522));
	tableCardPos.push_back(Vec2(480, 610));
	tableCardPos.push_back(Vec2(640, 610));
	tableCardPos.push_back(Vec2(150, 320));
	tableCardPos.push_back(Vec2(30, 522));
	tableCardPos.push_back(Vec2(640, 90));
	tableCardPos.push_back(Vec2(860, 150));

	tableCardDistance.push_back(Vec2(43, 0));
	tableCardDistance.push_back(Vec2(43, 0));
	tableCardDistance.push_back(Vec2(-43, 0));
	tableCardDistance.push_back(Vec2(-43, 0));
	tableCardDistance.push_back(Vec2(-43, 0));
	tableCardDistance.push_back(Vec2(43, 0));
	tableCardDistance.push_back(Vec2(43, 0));
	tableCardDistance.push_back(Vec2(43, 0));

	maxTableCardNumb.push_back(6);
	maxTableCardNumb.push_back(6);
	maxTableCardNumb.push_back(5);
	maxTableCardNumb.push_back(5);
	maxTableCardNumb.push_back(5);
	maxTableCardNumb.push_back(5);
	maxTableCardNumb.push_back(5);
	maxTableCardNumb.push_back(5);

	for (int i = 0; i < 8; i++) {
		tableCardNumb.push_back(0);
	}
	for (int i = 0; i < 39; i++) {
		maxChosenCuocs.push_back(0);
	}
	maxChosenCuocs[11] = 3;
	maxChosenCuocs[15] = 3;
	maxChosenCuocs[20] = 3;
	maxChosenCuocs[24] = 3;
	maxChosenCuocs[28] = 3;

	vecUserPos.push_back(Vec2(560, 90));
	vecUserPos.push_back(Vec2(1050, 320));
	vecUserPos.push_back(Vec2(560, 610));
	vecUserPos.push_back(Vec2(70, 320));

	for (int i = 0; i < tableCardPos.size(); i++) {
		int k = (i / 2) % 4;
		if (i == 1) k = 3;
		Vec2 d = vecUserPos[k] - tableCardPos[i];
		tableCardPos[i].x = vecUserPos[k].x / scaleScene.y - d.x;
		tableCardPos[i].y = vecUserPos[k].y / scaleScene.x - d.y;
	}

	vecSoundActions = { "anchoso", "angidanhnay", "angidanhnaychangmaymau", "anluon", "baovecheocanh", "batsach", "batvan", 
		"batvanj", "bomdo", "bottom", "botomomleo", "cakheo", "caulai", "chayrothoi", "chichi", "chiuchiu", "chonggay", 
		"chuachiroi", "cudida", "cuusach", "cuuvan", "cuuvanj", "danhcacaluon", "danhcailayaimau", "danhchingoichochidung", 
		"dayroi", "dinotluon", "duoian", "duoidung", "duoiluon", "hetbatroi", "hetcuuroi", "hetlucroi", "hetnguroi", "hettamroi", 
		"hetthatroi", "hetturoi", "lucsach", "lucvan", "lucvanj", "nguchua", "ngusach", "nguvan", "nguvangoichichi", "nguvanj", 
		"nhairoi", "nhatdinhiu", "nhidao", "nhisach", "nhivan", "nhivanj", "nhoxa", "nhoxacaybat", "nhoxacaycuu", "nhoxacayluc", 
		"nhoxacayngu", "nhoxacaythat", "nhoxacaytunhe", "nhoxacaynhi", "nhoxacaytam", "oihetnhiroi", "phatom", "phatom1", "quocdat", 
		"rarongthoi", "tamsach", "tamvan", "tamvanj", "taydo", "thatsach", "thatvan", "thatvanj", "thatvong", "tubung", "tusach", 
		"tuvan", "tuvanj", "tuxebo", "uluon", "unhanhnao", "uroi" };

	vecSoundCuocs = { "thong", "thienu", "diau", "chi", "bachthu", "bachthuchi", "chiuu", "ubon", "thapthanh", "bachdinh", "tamdo", "leo", 
		"haileo", "baleo", "bonleo", "tom", "haitom", "batom", "bontom", "kinhtuchi", "cothienkhai", "haithienkhai", "bathienkhai", 
		"bonthienkhai", "cobon", "haibon", "babon", "bonbon", "cochiu", "haichiu", "bachiu", "bonchiu", "hoaroicuaphat", "xuong", 
		"caloisandinh", "canhaydauthuyen", "nhalauxehoihoaroicuaphat", "thienu", "chuadonathoa", "nguongbatca" };
	
	string zone = Utils::getSingleton().currentZoneName;
	int index = zone.find_last_of("Q");
	if (index >= 0 && index < zone.length()) {
		zone = zone.substr(0, index);
	}

	if (zone.length() == 0) {
		zone = "VuongPhu";
	}

	Sprite* bg = Sprite::create("game/bg" + zone + ".jpg");
	bg->setPosition(560, 350);
	mLayer->addChild(bg);

	Sprite* centerBg = Sprite::create("game/center" + zone + ".png");
	centerBg->setPosition(560, 350);
	mLayer->addChild(centerBg);
	autoScaleNode(centerBg);

	if (zone.compare("VuongPhu") == 0) {
		Sprite* dragon1 = Sprite::create("game/dragon.png");
		dragon1->setPosition(330, 350);
		mLayer->addChild(dragon1);
		autoScaleNode(dragon1);

		Sprite* dragon2 = Sprite::create("game/dragon.png");
		dragon2->setPosition(790, 350);
		dragon2->setFlippedX(true);
		mLayer->addChild(dragon2);
		autoScaleNode(dragon2);
	}

	playLayer = Layer::create();
	mLayer->addChild(playLayer, 10);
	autoScaleNode(playLayer);
	playLayer->setPosition(1120 * (scaleScene.y - 1) / 2, 700 * (scaleScene.x - 1) / 2);

	endLayer = Layer::create();
	mLayer->addChild(endLayer, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(endLayer);
	//endLayer->setPosition(1120 * (scaleScene.y - 1) / 2, 700 * (scaleScene.x - 1) / 2);
	endLayer->setPositionY(700 * (scaleScene.x - 1) / 4);

	Vec2 topLeft = Vec2(0, winSize.height);
	btnBack = ui::Button::create("board/btn_back.png", "board/btn_back_clicked.png");
	btnBack->setPosition(topLeft + getScaleSceneDistance(Vec2(50, -50)));
	addTouchEventListener(btnBack, [=]() {
		if (state == NONE || state == READY || myServerSlot < 0) {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
			experimental::AudioEngine::stopAll();
		} else {
			hasRegisterOut = !hasRegisterOut;
			showSystemNotice(Utils::getSingleton().getStringForKey((hasRegisterOut ? "" : "huy_") + string("dang_ky_roi_ban_khi_het_van")));
		}
	});
	mLayer->addChild(btnBack, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnBack);

	Vec2 topRight = Vec2(winSize.width, winSize.height);
	ui::Button* btnSettings = ui::Button::create("board/btn_settings.png", "board/btn_settings_clicked.png");
	btnSettings->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	addTouchEventListener(btnSettings, [=]() {
		if (gameSplash->isVisible()) return;
		showSettings();

		/*if (isPause) {
			isPause = false;
			unscheduleUpdate();
		} else {
			isPause = true;
			scheduleUpdate();
		}*/

		/*nodeStilt->setRotation(0);
		nodeStilt->setVisible(true);
		for (int i = 0; i < vecStilts.size();i++) {
			vecStilts[i]->setVisible(true);
			vecStilts[i]->removeAllChildren();
			vecStilts[i]->setRotation(0);
			vecStilts[i]->setPosition(dealPos[i]);
		}
		state = CHOOSE_STILT;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < dealPos.size(); j++) {
				Sprite* sp = Sprite::create("cards/100.png");
				sp->setPosition(rand() % 20 - 10, rand() % 10 - 5);
				sp->setRotation(rand() % 60 - 30);
				sp->setScale(.7f);
				vecStilts[j]->addChild(sp);
			}
		}
		chosenStilt = rand() % 5 + 1;
		chosenStiltHost = 0;
		chosenHost = rand() % 4;
		startGameData.CardStilt = (1 + rand() % 8) * 3 + rand() % 3;
		for (int i = 0; i < 4; i++) {
			userIndexs[i] = i;
			vecPlayers.push_back(PlayerData());
		}
		startGameData.LastWinner = rand() % 4;
		onChooseStilt(chosenStilt);

		DelayTime* delay = DelayTime::create(1);
		CallFunc* func = CallFunc::create([=]() {
			onChooseHost(chosenStilt, chosenStiltHost, chosenHost);
		});
		runAction(Sequence::create(delay, func, nullptr));*/
	});
	mLayer->addChild(btnSettings, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnSettings);

	ui::Button* btnChat = ui::Button::create("board/btn_chat.png", "board/btn_chat_clicked.png");
	btnChat->setPosition(topRight + getScaleSceneDistance(Vec2(-145, -50)));// Vec2(975, 650));
	addTouchEventListener(btnChat, [=]() {
		if (gameSplash->isVisible()) return;
		showPopup(tableChat);
	});
	mLayer->addChild(btnChat, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnChat);

	iconGa = Sprite::create("board/btn_ga_on.png");
	iconGa->setPosition(topRight + getScaleSceneDistance(Vec2(-245, -50)));// 880, 650);
	mLayer->addChild(iconGa, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(iconGa);

	lbMoneyGa = Label::create("", "fonts/arial.ttf", 20);
	lbMoneyGa->setPosition(topRight + getScaleSceneDistance(Vec2(-240, -90)));// 880, 610);
	mLayer->addChild(lbMoneyGa, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(lbMoneyGa);

	spNetwork->setPosition(topRight + getScaleSceneDistance(Vec2(-325, -50)));

	btnReady = ui::Button::create("board/btn.png", "board/btn_clicked.png");
	btnReady->setPosition(Vec2(560, 350));
	btnReady->setVisible(false);
	addTouchEventListener(btnReady, [=]() {
		if (state != NONE && state != READY) return;
		state = READY;
		SFSRequest::getSingleton().RequestGameReady();
		btnReady->setVisible(false);
		btnCancelReady->setVisible(true);
	});
	mLayer->addChild(btnReady);
	autoScaleNode(btnReady);

	btnCancelReady = ui::Button::create("board/btn.png", "board/btn_clicked.png");
	btnCancelReady->setContentSize(Size(200, 65));
	btnCancelReady->setPosition(Vec2(560, 350));
	btnCancelReady->setScale9Enabled(true);
	btnCancelReady->setVisible(false);
	addTouchEventListener(btnCancelReady, [=]() {
		if (state != NONE && state != READY) return;
		SFSRequest::getSingleton().RequestGameReady();
		btnReady->setVisible(true);
		btnCancelReady->setVisible(false);
	});
	mLayer->addChild(btnCancelReady);
	autoScaleNode(btnCancelReady);

	btnBash = ui::Button::create("board/btn_danh.png", "board/btn_danh_clicked.png");
	btnBash->setPosition(Vec2(900, 35));
	btnBash->setVisible(false);
	addTouchEventListener(btnBash, [=]() {
		if (chosenCard < 0 || chosenCard >= spHandCards.size()) return;
		int numb = atoi(spHandCards[chosenCard]->getName().c_str());
		int card = numb % 1000;
		int group = numb / 1000;
		SFSRequest::getSingleton().RequestGameBash(card, group);

		progressTimer->setVisible(false);
		progressTimer->stopAllActions();
		noaction = 0;

		/*BashData data;
		data.UId = sfsIdMe;
		data.TurnId = 0;
		data.CanPenet = false;
		data.CanPenetWin = false;
		data.CardId = card;

		data.CardHand = myCardHand;
		for (int i = 0; i < 4; i++) {
			if (data.CardHand.Chan[i] == card) {
				data.CardHand.Chan.erase(data.CardHand.Chan.begin() + i);
				break;
			}
		}
		data.CardHand.Que.push_back(card);

		onUserBash(data);*/
	});
	mLayer->addChild(btnBash, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnBash);

	btnForward = ui::Button::create("board/btn_duoi.png", "board/btn_duoi_clicked.png");
	btnForward->setPosition(Vec2(900, 35));
	btnForward->setVisible(false);
	addTouchEventListener(btnForward, [=]() {
		SFSRequest::getSingleton().RequestGameForward();
		noaction = 0;
	});
	mLayer->addChild(btnForward, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnForward);

	btnBashBack = ui::Button::create("board/btn_danh.png", "board/btn_danh_clicked.png");
	btnBashBack->setPosition(Vec2(900, 35));
	btnBashBack->setVisible(false);
	addTouchEventListener(btnBashBack, [=]() {
		if (chosenCard < 0 || chosenCard >= spHandCards.size()) return;
		//btnBashBack->setVisible(false);
		int numb = atoi(spHandCards[chosenCard]->getName().c_str());
		int card = numb % 1000;
		int group = numb / 1000;
		SFSRequest::getSingleton().RequestGameBashBack(card, group);
		noaction = 0;
	});
	mLayer->addChild(btnBashBack, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnBashBack);

	btnHold = ui::Button::create("board/btn_an.png", "board/btn_an_clicked.png");
	btnHold->setPosition(Vec2(220, 35));
	btnHold->setVisible(false);
	addTouchEventListener(btnHold, [=]() {
		if (chosenCard < 0 || chosenCard >= spHandCards.size()) return;
		//btnHold->setVisible(false);
		//btnPick->setVisible(false);
		//btnForward->setVisible(false);

		int numb = atoi(spHandCards[chosenCard]->getName().c_str());
		int card = numb % 1000;
		int group = numb / 1000;
		SFSRequest::getSingleton().RequestGameHold(card, group);

		progressTimer->setVisible(false);
		progressTimer->stopAllActions();
		noaction = 0;

		//HoldData data;
		//data.UId = 2;// sfsIdMe;
		//data.TurnId = 0;
		//data.CardId = card;
		//data.CardIdHold = card;
		//onUserHold(data);

		//int index2 = (rand() % 4) * 2;
		//runningSpCard = getCardSprite(myCardHand.Que[rand() % myCardHand.Que.size()]);
		//runningSpCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD);
		//runningSpCard->setTag(constant::TAG_CARD_TABLE + index2);
		//runningSpCard->setPosition(tableCardPos[index2]);
		//runningSpCard->setRotation(0);
	});
	mLayer->addChild(btnHold, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnHold);

	btnPick = ui::Button::create("board/btn_boc.png", "board/btn_boc_clicked.png");
	btnPick->setPosition(Vec2(900, 35));
	btnPick->setVisible(false);
	addTouchEventListener(btnPick, [=]() {
		SFSRequest::getSingleton().RequestGamePick();
		//btnHold->setVisible(false);
		//btnPick->setVisible(false);

		progressTimer->setVisible(false);
		progressTimer->stopAllActions();
		noaction = 0;

		//PickData data;
		//data.UId = sfsIdMe;
		//data.TurnId = sfsIdMe;
		//data.CanPenet = false;
		//data.CanWin = false;
		//data.StiltCount = 20;
		//data.CardId = 6;// myCardHand.Que[rand() % myCardHand.Que.size()];
		//onUserPick(data);
	});
	mLayer->addChild(btnPick, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnPick);

	btnPenet = ui::Button::create("board/btn_chiu.png");
	btnPenet->setPosition(Vec2(900, 35));
	btnPenet->setVisible(false);
	addTouchEventListener(btnPenet, [=]() {
		SFSRequest::getSingleton().RequestGamePenet();
		btnPenet->stopAllActions();
        btnDropPenet->stopAllActions();
        btnPenet->setVisible(false);
        btnDropPenet->setVisible(false);
		btnHold->setVisible(false);
		btnPick->setVisible(false);

		progressTimer->setVisible(false);
		progressTimer->stopAllActions();
		noaction = 0;

		//PenetData data;
		//data.CardId = 5;
		//data.IsPicked = true;
		//data.CardHand = myCardHand;
		//data.UId = 2;// sfsIdMe;
		//data.CardHand.Chan.clear();
		//data.CardHand.Que.erase(data.CardHand.Que.begin() + 16);
		//onUserPenet(data);

		//int index2 = (rand() % 4) * 2;
		//runningSpCard = getCardSprite(myCardHand.Que[rand() % myCardHand.Que.size()]);
		//runningSpCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD);
		//runningSpCard->setTag(constant::TAG_CARD_TABLE + index2);
		//runningSpCard->setPosition(tableCardPos[index2]);
		//runningSpCard->setRotation(0);
	});
	mLayer->addChild(btnPenet, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnPenet);
    
    btnDropPenet = ui::Button::create("board/btn_bo_chiu.png", "board/btn_bo_chiu_clicked.png");
    btnDropPenet->setPosition(Vec2(220, 35));
    btnDropPenet->setVisible(false);
    addTouchEventListener(btnDropPenet, [=]() {
        dropPenet();
    });
    mLayer->addChild(btnDropPenet, constant::GAME_ZORDER_BUTTON);
    autoScaleNode(btnDropPenet);

	btnWin = ui::Button::create("board/btn_u.png");
	btnWin->setPosition(vecUserPos[0]);
	btnWin->setVisible(false);
	addTouchEventListener(btnWin, [=]() {
		SFSRequest::getSingleton().RequestGameWin();
		btnWin->stopAllActions();
        btnDropWin->stopAllActions();
        btnPenet->stopAllActions();
        btnDropPenet->stopAllActions();
		btnWin->setVisible(false);
		btnDropWin->setVisible(false);
		btnForward->setVisible(false);
		btnHold->setVisible(false); 
		btnPenet->setVisible(false);
        btnDropPenet->setVisible(false);
		noaction = 0;
	});
	mLayer->addChild(btnWin, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnWin);

	btnDropWin = ui::Button::create("board/btn_bou.png", "board/btn_bou_clicked.png");
	btnDropWin->setPosition(Vec2(220, 35));
	btnDropWin->setVisible(false);
	addTouchEventListener(btnDropWin, [=]() {
		dropWin();
	});
	mLayer->addChild(btnDropWin, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnDropWin);

	btnXemNoc = ui::Button::create("board/btn_xem_noc.png", "board/btn_xem_noc_clicked.png");
	btnXemNoc->setPosition(Vec2(560, 280));
	btnXemNoc->setVisible(false);
	addTouchEventListener(btnXemNoc, [=]() {
		showStiltCards();
	});
	mLayer->addChild(btnXemNoc, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(btnXemNoc);

	btnDongNoc = ui::Button::create("board/btn_dong_noc.png", "board/btn_dong_noc_clicked.png");
	btnDongNoc->setPosition(Vec2(560, 280));
	btnDongNoc->setVisible(false);
	addTouchEventListener(btnDongNoc, [=]() {
		showWinnerCards();
	});
	mLayer->addChild(btnDongNoc, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(btnDongNoc);

	Label* lbBtnReady = Label::createWithTTF(Utils::getSingleton().getStringForKey("ready"), "fonts/UTM AZUKI.ttf", 30);
	lbBtnReady->setPosition(btnReady->getContentSize().width/2, btnReady->getContentSize().height/2);
	lbBtnReady->setColor(Color3B::YELLOW);
	lbBtnReady->enableOutline(Color4B::BLACK, 2);
	btnReady->addChild(lbBtnReady);

	Label* lbBtnCancelReady = Label::createWithTTF(Utils::getSingleton().getStringForKey("cancel_ready"), "fonts/UTM AZUKI.ttf", 30);
	lbBtnCancelReady->setPosition(btnCancelReady->getContentSize().width / 2, btnCancelReady->getContentSize().height / 2);
	lbBtnCancelReady->setColor(Color3B::YELLOW);
	lbBtnCancelReady->enableOutline(Color4B::BLACK, 2);
	btnCancelReady->addChild(lbBtnCancelReady);

	lbNoticeAction = Label::createWithTTF("", "fonts/UTM AZUKI.ttf", 35);
	lbNoticeAction->setPosition(560, 350);
	lbNoticeAction->setColor(Color3B::YELLOW);
	lbNoticeAction->enableOutline(Color4B::BLACK, 2);
	lbNoticeAction->setVisible(false);
	mLayer->addChild(lbNoticeAction);
	autoScaleNode(lbNoticeAction);

	/*Sprite* bgDiaNoc = Sprite::create("board/bg_dianoc.png");
	bgDiaNoc->setPosition(564, 422);
	mLayer->addChild(bgDiaNoc);
	bgDiaNoc->setVisible(false);
	autoScaleNode(bgDiaNoc);*/

	ui::Button* bgDiaNoc = ui::Button::create("board/bg_dianoc.png", "board/bg_dianoc.png");
	bgDiaNoc->setPosition(Vec2(564, 422));
	mLayer->addChild(bgDiaNoc);
	bgDiaNoc->setVisible(false);
	autoScaleNode(bgDiaNoc);
	addTouchEventListener(bgDiaNoc, [=]() {
		if (btnPick->isVisible()) {
			SFSRequest::getSingleton().RequestGamePick();
			progressTimer->setVisible(false);
			progressTimer->stopAllActions();
		}
	});

	Sprite* spCardNoc = Sprite::create("board/bg_card_up.png");
	spCardNoc->setRotation(-20);
	spCardNoc->setPosition(bgDiaNoc->getContentSize().width / 2 + 25, bgDiaNoc->getContentSize().height / 2 + 20);
	spCardNoc->setName("spcardnoc");
	bgDiaNoc->addChild(spCardNoc, 1);

	Sprite* spNapNoc = Sprite::create("board/bg_napnoc.png");
	spNapNoc->setPosition(bgDiaNoc->getContentSize().width / 2 - 4, bgDiaNoc->getContentSize().height / 2 + 7);
	bgDiaNoc->addChild(spNapNoc, 2);

	lbCardNoc = Label::create();
	lbCardNoc->setString("23");
	lbCardNoc->setSystemFontSize(30);
	lbCardNoc->setPosition(bgDiaNoc->getContentSize().width / 2 + 55, bgDiaNoc->getContentSize().height / 2 + 31);
	bgDiaNoc->addChild(lbCardNoc, 3);

	nodeStilt = Node::create();
	nodeStilt->setPosition(getScaleScenePosition(Vec2(560, 350)));
	playLayer->addChild(nodeStilt, constant::GAME_ZORDER_USER + 10);

	for (Vec2 v : dealPos) {
		Node* node = Node::create();
		node->setPosition(v);
		node->setVisible(false);
		nodeStilt->addChild(node);
		vecStilts.push_back(node);
	}

	/*int ii = 0;
	for (Vec2 v : dealPos) {
		Node* node = Node::create();
		node->setPosition(v +Vec2(560, 350));
		node->setName(to_string(ii++));
		mLayer->addChild(node, constant::GAME_ZORDER_USER + 10);
		vecStilts.push_back(node);
	}*/

	for (int i = 0; i < vecUserPos.size(); i++) {
		Sprite* spInvite = Sprite::create("board/btn_invite.png");
		spInvite->setPosition(vecUserPos[i]);
		mLayer->addChild(spInvite);
		spInvites.push_back(spInvite);
		autoScaleNode(spInvite);

		UserNode* user = UserNode::create();
		user->setPosition(getScaleScenePosition(vecUserPos[i]));
		user->setVisible(false);
		playLayer->addChild(user, constant::GAME_ZORDER_USER);
		vecUsers.push_back(user);

		Sprite* spSS = Sprite::create("board/txt_sansang.png");
		spSS->setPosition(vecUserPos[i]);
		spSS->setVisible(false);
		mLayer->addChild(spSS, constant::GAME_ZORDER_USER + 9);
		spSanSangs.push_back(spSS);
		autoScaleNode(spSS);

		Sprite* spBB = Sprite::create("board/txt_bat_bao.png");
		spBB->setPosition(vecUserPos[i]);
		spBB->setVisible(false);
		mLayer->addChild(spBB, constant::GAME_ZORDER_USER + 9);
		spBatBaos.push_back(spBB);
		autoScaleNode(spBB);

		Label* lb1 = Label::createWithTTF("100,000", "fonts/UTM AZUKI.ttf", 45);
		mLayer->addChild(lb1, constant::GAME_ZORDER_USER + 11);
		lb1->setPosition(vecUserPos[i]);
		lb1->setOpacity(0);
		lbWinMoneys.push_back(lb1);
		autoScaleNode(lb1);
	}

	progressTimer = ProgressTimer::create(Sprite::create("board/time_line.png"));
	progressTimer->setType(ProgressTimer::Type::RADIAL);
	progressTimer->setPercentage(100);
	progressTimer->setReverseProgress(true);
	progressTimer->setVisible(false);
	progressTimer->setColor(Color3B::GREEN);
	playLayer->addChild(progressTimer, constant::GAME_ZORDER_USER + 9);

	spChuPhong = Sprite::create("board/chuphong.png");
	spChuPhong->setVisible(false);
	mLayer->addChild(spChuPhong, constant::GAME_ZORDER_USER + 10);
	autoScaleNode(spChuPhong);

	spChonCai = Sprite::create("board/txt_choncai.png");
	spChonCai->setVisible(false);
	mLayer->addChild(spChonCai, constant::GAME_ZORDER_USER + 9);
	autoScaleNode(spChonCai);

	lbCrestTime = Label::create("", "fonts/arialbd.ttf", 50);
	lbCrestTime->setPosition(btnXemNoc->getPosition() + getScaleSceneDistance(Vec2(190, 0)));
	lbCrestTime->setVisible(false);
	mLayer->addChild(lbCrestTime, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(lbCrestTime);

	DelayTime* delay = DelayTime::create(1);
	CallFunc* func = CallFunc::create([=]() {
		if (state != ENDING && (btnXemNoc->isVisible() || btnDongNoc->isVisible())) {
			btnXemNoc->setVisible(false);
			btnDongNoc->setVisible(false);
			endLayer->removeAllChildren();
		}
		if (lbCrestTime->getString().compare("0") == 0) {
			lbCrestTime->setVisible(false);
			lbCrestTime->pauseSchedulerAndActions();
			return;
		}
		int time = atoi(lbCrestTime->getString().c_str());
		time--;
		lbCrestTime->setString(to_string(time));
	});
	lbCrestTime->runAction(RepeatForever::create(Sequence::create(delay, func, nullptr)));
	lbCrestTime->pauseSchedulerAndActions();

	initChatTable();
	initCrestTable();
	initEndMatchTable();
	initInviteTable();
	initSettingsPopup();
	initTableInfo();
	initPopupUserInfo();
	//initEventView(Vec2(0, 680), Size(Director::sharedDirector()->getVisibleSize().width, 40));

	Node* nodeError = Node::create();
	nodeError->setPosition(560, 350);
	nodeError->setVisible(false);
	mLayer->addChild(nodeError, constant::ZORDER_POPUP + 10);
	autoScaleNode(nodeError);

	Sprite* bgError = Sprite::create("popup/bg.png");
	nodeError->addChild(bgError);

	lbError = Label::createWithTTF("", "fonts/arialbd.ttf", 30);
	lbError->setAlignment(TextHAlignment::CENTER);
	lbError->setColor(Color3B::YELLOW);
	lbError->setWidth(600);
	nodeError->addChild(lbError);

	gameSplash = ui::Scale9Sprite::create("white.png");
	gameSplash->setContentSize(Size(1500, 1000));
	gameSplash->setPosition(560, 350);
	gameSplash->setColor(Color3B::BLACK);
	gameSplash->setOpacity(150);
	gameSplash->setVisible(false);
	mLayer->addChild(gameSplash, constant::GAME_ZORDER_SPLASH);

	/*sfsIdMe = 1;
	state = PLAY;
	startGameData.LastWinner = sfsIdMe;
	myCardHand.ThienKhai = vector<unsigned char>();
	myCardHand.Chan = vector<unsigned char>();
	myCardHand.BaDau = vector<unsigned char>();
	myCardHand.Ca = vector<unsigned char>();
	myCardHand.Que = vector<unsigned char>();
	for (int i = 0; i < 1; i++) {
		myCardHand.Chan.push_back(5);
	}
	for (int i = 0; i < 16; i++) {
		myCardHand.Que.push_back((1 + rand() % 8) * 3 + rand() % 3);
	}
	myCardHand.Que.push_back(5);
	showMyCards();
	int index2 = (rand() % 4) * 2;
	runningSpCard = getCardSprite(myCardHand.Que[rand() % myCardHand.Que.size()]);
	runningSpCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD);
	runningSpCard->setTag(constant::TAG_CARD_TABLE + index2);
	runningSpCard->setPosition(tableCardPos[index2]);
	runningSpCard->setRotation(0);*/

	/*for (int i = 0; i < tableCardNumb.size(); i++) {
		for (int j = 0; j < 5; j++) {
			Sprite* sp = getCardSprite(5);
			sp->setScale(.85f);
			sp->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD);
			sp->setPosition(tableCardPos[i] + tableCardDistance[i] * j);

			if (i % 2 == 1) {
				Sprite* sp1 = getCardSprite(5);
				sp1->setScale(.85f);
				sp1->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD);
				sp1->setPosition(tableCardPos[i] + tableCardDistance[i] * j - Vec2(0, 40));
			}
		}
	}*/

	/*state = PLAY;
	btnXemNoc->setVisible(true);
	for (int i = 0; i < 7; i++) {
		endMatchData.ListChanU.push_back(5);
	}
	for (int i = 0; i < 2; i++) {
		endMatchData.ListCaU.push_back(10);
	}
	for (int i = 0; i < 4; i++) {
		endMatchData.ListCardU.push_back(20);
	}
	showWinnerCards();

	for (int i = 0; i < 13; i++) {
		endMatchData.ListStiltCard.push_back(8);
	}
	showStiltCards();*/

	//dealCards();

	showWaiting();
}

void GameScene::registerEventListenner()
{
	EventHandler::getSingleton().onApplicationDidEnterBackground = std::bind(&GameScene::onApplicationDidEnterBackground, this);
	EventHandler::getSingleton().onPingPong = std::bind(&GameScene::onPingPong, this, std::placeholders::_1);
	EventHandler::getSingleton().onConnected = std::bind(&GameScene::onConnected, this);
	EventHandler::getSingleton().onConnectionFailed = std::bind(&GameScene::onConnectionFailed, this);
	EventHandler::getSingleton().onConnectionLost = std::bind(&GameScene::onConnectionLost, this, std::placeholders::_1);
	EventHandler::getSingleton().onLoginZone = bind(&GameScene::onLoginZone, this);
	EventHandler::getSingleton().onLoginZoneError = bind(&GameScene::onLoginZoneError, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onUserDataSFSResponse = std::bind(&GameScene::onUserDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onUserExitRoom = std::bind(&GameScene::onUserExitRoom, this, std::placeholders::_1);
	EventHandler::getSingleton().onErrorSFSResponse = std::bind(&GameScene::onErrorResponse, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onPublicMessage = std::bind(&GameScene::onPublicMessage, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onRoomDataSFSResponse = std::bind(&GameScene::onRoomDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onStartGameDataSFSResponse = std::bind(&GameScene::onStartGameDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onChooseStiltSFSResponse = std::bind(&GameScene::onChooseStilt, this, std::placeholders::_1);
	EventHandler::getSingleton().onChooseHostSFSResponse = std::bind(&GameScene::onChooseHost, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	EventHandler::getSingleton().onUserBashSFSResponse = std::bind(&GameScene::onUserBash, this, std::placeholders::_1);
	EventHandler::getSingleton().onUserBashBackSFSResponse = std::bind(&GameScene::onUserBashBack, this, std::placeholders::_1);
	EventHandler::getSingleton().onUserHoldSFSResponse = std::bind(&GameScene::onUserHold, this, std::placeholders::_1);
	EventHandler::getSingleton().onUserPickSFSResponse = std::bind(&GameScene::onUserPick, this, std::placeholders::_1);
	EventHandler::getSingleton().onUserPenetSFSResponse = std::bind(&GameScene::onUserPenet, this, std::placeholders::_1);
	EventHandler::getSingleton().onUserForwardSFSResponse = std::bind(&GameScene::onUserForward, this, std::placeholders::_1);
	EventHandler::getSingleton().onUserWinSFSResponse = std::bind(&GameScene::onUserWin, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onCrestResponseSFSResponse = std::bind(&GameScene::onCrestResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onEndMatchSFSResponse = std::bind(&GameScene::onEndMatch, this, std::placeholders::_1);
	EventHandler::getSingleton().onEndMatchTieSFSResponse = std::bind(&GameScene::onEndMatchTie, this, std::placeholders::_1);
	EventHandler::getSingleton().onEndMatchMoneySFSResponse = std::bind(&GameScene::onEndMatchMoney, this, std::placeholders::_1);
	EventHandler::getSingleton().onGamePunishSFSResponse = std::bind(&GameScene::onPunishResponse, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onGameReadySFSResponse = std::bind(&GameScene::onUserReadyResponse, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onGameTableSFSResponse = std::bind(&GameScene::onTableResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onLobbyUserDataSFSResponse = std::bind(&GameScene::onLobbyUserResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onGameRoomDataGaSFSResponse = std::bind(&GameScene::onRoomDataGaResponse, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onGamePlayingDataSFSResponse = std::bind(&GameScene::onGamePlayingDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onGameSpectatorDataSFSResponse = std::bind(&GameScene::onGameSpectatorDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onGameMyReconnectDataSFSResponse = std::bind(&GameScene::onGameMyReconnectDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onGameUserReconnectDataSFSResponse = std::bind(&GameScene::onGameUserReconnectDataResponse, this, std::placeholders::_1);
	EventHandler::getSingleton().onLobbyTableSFSResponse = std::bind(&GameScene::onLobbyListTableResponse, this, std::placeholders::_1);
}

void GameScene::unregisterEventListenner()
{
	BaseScene::unregisterEventListenner();
	EventHandler::getSingleton().onConnected = NULL;
	EventHandler::getSingleton().onConnectionFailed = NULL;
	EventHandler::getSingleton().onConnectionLost = NULL;
	EventHandler::getSingleton().onUserDataSFSResponse = NULL;
	EventHandler::getSingleton().onUserExitRoom = NULL;
	EventHandler::getSingleton().onErrorSFSResponse = NULL;
	EventHandler::getSingleton().onPublicMessage = NULL;
	EventHandler::getSingleton().onRoomDataSFSResponse = NULL;
	EventHandler::getSingleton().onStartGameDataSFSResponse = NULL;
	EventHandler::getSingleton().onChooseStiltSFSResponse = NULL;
	EventHandler::getSingleton().onChooseHostSFSResponse = NULL;
	EventHandler::getSingleton().onUserBashSFSResponse = NULL;
	EventHandler::getSingleton().onUserBashBackSFSResponse = NULL;
	EventHandler::getSingleton().onUserHoldSFSResponse = NULL;
	EventHandler::getSingleton().onUserPickSFSResponse = NULL;
	EventHandler::getSingleton().onUserPenetSFSResponse = NULL;
	EventHandler::getSingleton().onUserForwardSFSResponse = NULL;
	EventHandler::getSingleton().onUserWinSFSResponse = NULL;
	EventHandler::getSingleton().onEndMatchSFSResponse = NULL;
	EventHandler::getSingleton().onEndMatchTieSFSResponse = NULL;
	EventHandler::getSingleton().onEndMatchMoneySFSResponse = NULL;
	EventHandler::getSingleton().onCrestResponseSFSResponse = NULL;
	EventHandler::getSingleton().onGamePunishSFSResponse = NULL;
	EventHandler::getSingleton().onGameReadySFSResponse = NULL;
	EventHandler::getSingleton().onGameTableSFSResponse = NULL;
	EventHandler::getSingleton().onLobbyUserDataSFSResponse = NULL;
	EventHandler::getSingleton().onGameRoomDataGaSFSResponse = NULL;
	EventHandler::getSingleton().onGamePlayingDataSFSResponse = NULL;
	EventHandler::getSingleton().onGameSpectatorDataSFSResponse = NULL;
	EventHandler::getSingleton().onGameMyReconnectDataSFSResponse = NULL;
	EventHandler::getSingleton().onGameUserReconnectDataSFSResponse = NULL;
	EventHandler::getSingleton().onLobbyTableSFSResponse = NULL;
}

void GameScene::editBoxReturn(cocos2d::ui::EditBox * editBox)
{
}

bool GameScene::onTouchBegan(Touch * touch, Event * _event)
{
	if (!BaseScene::onTouchBegan(touch, _event) && !splash->isVisible() && !gameSplash->isVisible()) {
		Point pos = touch->getLocation();
		pos = mLayer->convertToNodeSpace(pos);

		/*if (tableCrest->isVisible()) {
			Node* scroll = tableCrest->getChildByName("scroll");
			Point pos2 = touch->getLocation();
			pos2 = scroll->convertToNodeSpace(pos2);
			CCLOG("%.0f %.0f", pos2.x, pos2.y);
			for (Label* lb : vecCrests) {
				if (lb->getBoundingBox().containsPoint(pos2)) {
					int i = lb->getTag();
					if (i == 33) {
						if (chosenCuocs.size() == 1 && chosenCuocs[0] == 33) {
							chosenCuocs.clear();
							chosenCuocNumbs.clear();
							vecCrests[0]->setColor(Color3B::WHITE);
							lbChonCuoc->setString("");
							return true;
						}

						for (int j = 1; j < vecCrests.size(); j++) {
							vecCrests[j]->setColor(Color3B::WHITE);
						}
						vecCrests[0]->setColor(Color3B::YELLOW);
						chosenCuocs.clear();
						chosenCuocNumbs.clear();
						chosenCuocs.push_back(33);
						chosenCuocNumbs.push_back(0);
						lbChonCuoc->setString(Utils::getSingleton().getStringForKey("cuoc_33"));
					} else {
						if (chosenCuocs.size() == 1 && chosenCuocs[0] == 33) {
							chosenCuocs.clear();
							chosenCuocNumbs.clear();
							vecCrests[0]->setColor(Color3B::WHITE);
						}

						int index = 0;
						while (index < chosenCuocs.size() && chosenCuocs[index] != i) index++;
						if (index == chosenCuocs.size()) {
							chosenCuocs.push_back(i);
							chosenCuocNumbs.push_back(0);
							lb->setColor(Color3B::YELLOW);
						} else {
							chosenCuocNumbs[index] ++;
							if (chosenCuocNumbs[index] > maxChosenCuocs[i]) {
								chosenCuocs.erase(chosenCuocs.begin() + index);
								chosenCuocNumbs.erase(chosenCuocNumbs.begin() + index);
								lb->setColor(Color3B::WHITE);
							}
						}

						string strcuoc = "";
						for (int j = 0; j < chosenCuocs.size(); j++) {
							if (strcuoc.length() > 0) {
								strcuoc += ", ";
							}
							strcuoc += Utils::getSingleton().getStringForKey("cuoc_" + to_string(chosenCuocs[j] + chosenCuocNumbs[j]));
						}
						lbChonCuoc->setString(strcuoc);
					}
					return true;
				}
			}
			return false;
		}*/

		if (myServerSlot == 0 && (state == NONE || state == READY)
			&& iconGa->getBoundingBox().containsPoint(pos)) {
			SFSRequest::getSingleton().RequestGameGa();
			return true;
		}

		if (startGameData.LastWinner == sfsIdMe) {
			if (state == CHOOSE_STILT) {
				for (int i = 0; i < dealPos.size(); i++) {
					if (pos.distance(Vec2(560, 350) + dealPos[i]) < 100) {
						state = CHOOSE_HOST;
						SFSRequest::getSingleton().RequestGameChooseStilt(i + 1);
						return true;
					}
				}
			} else if (state == CHOOSE_HOST) {
				Point pos2 = touch->getLocation();
				pos2 = nodeStilt->convertToNodeSpace(pos2);
				for (int i = 0; i < dealPos.size(); i++) {
					if (i != chosenStilt && pos2.distance(vecStilts[i]->getPosition()) < 100) {
						state = WAIT;
						chosenStiltHost = i;
						SFSRequest::getSingleton().RequestGameChooseHost(i + 1);
						return true;
					}
				}
			}
		}

		/*if (state == PLAY && pos.distance(vecUsers[0]->getPosition()) < 300) {
			float rot = CC_RADIANS_TO_DEGREES(Vec2::angle(pos - vecUsers[0]->getPosition(), Vec2(0, 1)));
			if (pos.x < 560) rot *= -1;
			for (int i = 0; i < spHandCards.size(); i++) {
				if (abs(rot - spHandCards[i]->getRotation()) < 5) {
					if (chosenCard >= 0 && chosenCard < spHandCards.size()) {
						spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.2f));
					}
					if (chosenCard != i) {
						chosenCard = i;
						spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.35f));
					} else {
						chosenCard = -1;
					}
					return true;
					break;
				}
			}
		}*/

		for (int i = 0; i < 4; i++) {
			if (pos.distance(vecUserPos[i]) < 60) {
				if (vecUsers[i]->isVisible() && vecUsers[i]->getName().length() > 0) {
					if (vecUsers[i]->getName().compare(Utils::getSingleton().userDataMe.Name) == 0) {
						showPopupUserInfo(Utils::getSingleton().userDataMe, false);
					} else {
						SFSRequest::getSingleton().RequestUserInfo(vecUsers[i]->getName());
					}
					break;
				} else {
					hasClickInvite = true;
					SFSRequest::getSingleton().RequestGameLobbyUser();
				}
				return true;
			}
		}

		if (state == PLAY) {
			Point pos2 = touch->getLocation();
			pos2 = playLayer->convertToNodeSpace(pos2);
			if (pos2.distance(vecUsers[0]->getPosition()) < 300) {
				float rot = CC_RADIANS_TO_DEGREES(Vec2::angle(pos2 - vecUsers[0]->getPosition(), Vec2(0, 1)));
				if (pos2.x < vecUsers[0]->getPositionX()) rot *= -1;
				for (int i = 0; i < spHandCards.size(); i++) {
					if (abs(rot - spHandCards[i]->getRotation()) < 5) {
						if (chosenCard >= 0 && chosenCard < spHandCards.size()) {
							spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.2f));
						}
						if (chosenCard != i) {
							chosenCard = i;
							spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.35f));
						} else {
							chosenCard = -1;
						}
						return true;
						break;
					}
				}
			}
		}
	}
	
	return false;
}

void GameScene::onApplicationDidEnterBackground()
{
	BaseScene::onApplicationDidEnterBackground();
	if (state != NONE && state != READY && myServerSlot >= 0) {
		string username = Utils::getSingleton().userDataMe.Name;
		double timeSecs = Utils::getSingleton().getCurrentSystemTimeInSecs();
		UserDefault::getInstance()->setDoubleForKey((constant::KEY_RECONNECT_TIME + username).c_str(), timeSecs + 300);
		UserDefault::getInstance()->setIntegerForKey((constant::KEY_RECONNECT_ZONE_INDEX + username).c_str(), Utils::getSingleton().getCurrentZoneIndex());
	}
}

void GameScene::dealCards()
{
	state = DEAL;

	/*if (soundDealId == -1) {
		soundDealId = experimental::AudioEngine::play2d("sound/deal_card.mp3", true, .1f);
	} else {
		experimental::AudioEngine::setCurrentTime(soundDealId, 0);
		experimental::AudioEngine::resume(soundDealId);
	}
	DelayTime* delayStopSound = DelayTime::create(10);
	CallFunc* funcStopSound = CallFunc::create([=]() {
		experimental::AudioEngine::pause(soundDealId);
	});
	this->runAction(Sequence::create(delayStopSound, funcStopSound, nullptr));*/

	vector<Vec2> nodePos;
	nodePos.push_back(Vec2(320, 350));
	nodePos.push_back(Vec2(800, 350));

	vector<Node*> nodes;
	for (int i = 0; i < nodePos.size(); i++) {
		Node* node = Node::create();
		node->setPosition(getScaleScenePosition(nodePos[i]));
		playLayer->addChild(node, constant::GAME_ZORDER_USER + 1);
		nodes.push_back(node);
	}

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < dealPos.size(); j++) {
			for (int k = 0; k < nodePos.size(); k++) {
				Sprite* sp = Sprite::create("cards/100.png");
				sp->setScale(.6f);
				sp->setName(to_string(j));
				nodes[k]->addChild(sp);
				spDealCards.push_back(sp);

				DelayTime* delay = DelayTime::create(.2f * (i * dealPos.size() + j));
				DelayTime* delay2 = DelayTime::create(.1f + .2f * (i * dealPos.size() + j));
				MoveBy* move = MoveBy::create(.3f, dealPos[j] + Vec2(rand() % 20 - 10, rand() % 20 - 10));
				RotateBy* rotate = RotateBy::create(.3f, rand() % 60 - 30);
				CallFunc* func = CallFunc::create([=]() {
					if (Utils::getSingleton().SoundEnabled) {
						experimental::AudioEngine::play2d("sound/deal_card.mp3", false, .1f);
					}
				});

				sp->runAction(Sequence::create(delay, move, nullptr));
				sp->runAction(Sequence::create(delay->clone(), rotate, nullptr));
				sp->runAction(Sequence::create(delay2, func, nullptr));
			}
		}
	}

	for (Node* n : nodes) {
		DelayTime* delay = DelayTime::create(7);
		MoveTo* move = MoveTo::create(.5f, getScaleScenePosition(Vec2(560, 350)));
		n->runAction(Sequence::create(delay, move, nullptr));
	}

	DelayTime* delayStilt = DelayTime::create(8);
	CallFunc* func = CallFunc::create([=]() {
		experimental::AudioEngine::stopAll();
		for (Sprite* sp : spDealCards) {
			sp->stopAllActions();
			int i = atoi(sp->getName().c_str());
			sp->setParent(nullptr);
			sp->setPosition(sp->getPosition() - dealPos[i]);
			vecStilts[i]->addChild(sp);
		}
		for (Node* n : nodes) {
			n->removeAllChildren();
		}
		state = CHOOSE_STILT;
		if (startGameData.LastWinner == sfsIdMe) {
			lbNoticeAction->setVisible(true);
			lbNoticeAction->setString(Utils::getSingleton().getStringForKey("choose_stilt"));
		}
		spChonCai->setVisible(true);
		spChonCai->setPosition(vecUserPos[userIndexs[startGameData.LastWinner]]);

		runTimeWaiting(startGameData.LastWinner, timeChooseHost);
	});
	runAction(Sequence::create(delayStilt, func, nullptr));
}

void GameScene::showMyCards(bool runEffect)
{
	int k = 0;
	for (int i = 0; i < myCardHand.ThienKhai.size(); i++) {
		for (int j = 0; j < 4; j++) {
			getCardSpriteToHand(myCardHand.ThienKhai[i], constant::CARD_GROUP_THIENKHAI, k);
			k++;
		}
	}
	for (int i = 0; i < myCardHand.Chan.size(); i++) {
		for (int j = 0; j < 2; j++) {
			getCardSpriteToHand(myCardHand.Chan[i], constant::CARD_GROUP_CHAN, k);
			k++;
		}
	}
	for (int i = 0; i < myCardHand.Ca.size(); i++) {
		int id1 = (myCardHand.Ca[i] - 1) * 3 + (myCardHand.Ca[i + 1] - 1);
		getCardSpriteToHand(id1, constant::CARD_GROUP_CA, k);
		k++;

		int id2 = (myCardHand.Ca[i] - 1) * 3 + (myCardHand.Ca[i + 2] - 1);
		getCardSpriteToHand(id2, constant::CARD_GROUP_CA, k);
		k++;
		i += 2;
	}
	for (int i = 0; i < myCardHand.BaDau.size(); i++) {
		for (int j = 0; j < 3; j++) {
			int id = (myCardHand.BaDau[i] - 1) * 3 + j;
			getCardSpriteToHand(id, constant::CARD_GROUP_3DAU, k);
			k++;
		}
	}
	for (int i = 0; i < myCardHand.Que.size(); i++) {
		getCardSpriteToHand(myCardHand.Que[i], constant::CARD_GROUP_QUE, k);
		k++;
	}

	int rot = 11;
	int startRot = -(rot * spHandCards.size() / 2) + rot / 2;
	for (int i = 0; i < spHandCards.size(); i++) {
		if (runEffect) {
			spHandCards[i]->setRotation(startRot);
			RotateBy* rotate = RotateBy::create(i * .1f, i * rot);
			spHandCards[i]->runAction(rotate);
		} else {
			spHandCards[i]->setRotation(startRot + i * rot);
		}
	}
}

void GameScene::dropWin()
{
	SFSRequest::getSingleton().RequestGameDropWin();
	btnWin->stopAllActions();
	btnDropWin->stopAllActions();
	btnWin->setVisible(false);
	btnDropWin->setVisible(false);
	noaction = 0;

	if (waitAction == constant::GAME_ACTION_BASH) {
		btnPick->setVisible(true);
		btnHold->setVisible(true);
		waitAction = -1;
	} else if (waitAction == constant::GAME_ACTION_PICK) {
		btnForward->setVisible(true);
		btnHold->setVisible(true);
		waitAction = -1;
	}
}

void GameScene::dropPenet()
{
    SFSRequest::getSingleton().RequestGameDropPenet();
    btnPenet->stopAllActions();
    btnDropPenet->stopAllActions();
    btnPenet->setVisible(false);
    btnDropPenet->setVisible(false);
    noaction = 0;
    
    if (waitAction == constant::GAME_ACTION_BASH) {
        btnPick->setVisible(true);
        btnHold->setVisible(true);
        waitAction = -1;
    } else if (waitAction == constant::GAME_ACTION_PICK) {
        btnForward->setVisible(true);
        btnHold->setVisible(true);
        waitAction = -1;
    }
}

void GameScene::runTimeWaiting(long uid, float time)
{
	int index = userIndexs[uid];
	progressTimer->setTag(index);
	progressTimer->stopAllActions();
	progressTimer->setVisible(true);
	progressTimer->setPercentage(100);
	progressTimer->setPosition(getScaleScenePosition(vecUserPos[index]));
	progressTimer->runAction(ProgressTo::create(time, 0));

	if (uid == sfsIdMe && state == PLAY) {
		DelayTime* delay = DelayTime::create(time);
		CallFunc* func = CallFunc::create([=]() {
			if (chosenCard >= 0) {
				spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.2f));
				chosenCard = -1;
			}
		});
		progressTimer->runAction(Sequence::create(delay, func, nullptr));
	}
}

void GameScene::updateCardHand(CardHandData data)
{
	this->myCardHand = data;
	int k = 0;
	int rot = 11;
	int startRot = -(rot * spHandCards.size() / 2) + rot / 2;
	vector<int> ids, rots, groups;
	for (int i = 0; i < myCardHand.ThienKhai.size(); i++) {
		for (int j = 0; j < 4; j++) {
			ids.push_back(myCardHand.ThienKhai[i]);
			rots.push_back(startRot + k * rot);
			groups.push_back(constant::CARD_GROUP_THIENKHAI);
			k++;
		}
	}
	for (int i = 0; i < myCardHand.Chan.size(); i++) {
		for (int j = 0; j < 2; j++) {
			ids.push_back(myCardHand.Chan[i]);
			rots.push_back(startRot + k * rot);
			groups.push_back(constant::CARD_GROUP_CHAN);
			k++;
		}
	}
	for (int i = 0; i < myCardHand.Ca.size(); i++) {
		int id1 = (myCardHand.Ca[i] - 1) * 3 + (myCardHand.Ca[i + 1] - 1);
		ids.push_back(id1);
		rots.push_back(startRot + k * rot);
		groups.push_back(constant::CARD_GROUP_CA);
		k++;

		int id2 = (myCardHand.Ca[i] - 1) * 3 + (myCardHand.Ca[i + 2] - 1);
		ids.push_back(id2);
		rots.push_back(startRot + k * rot);
		groups.push_back(constant::CARD_GROUP_CA);
		k++;
		i += 2;
	}
	for (int i = 0; i < myCardHand.BaDau.size(); i++) {
		for (int j = 0; j < 3; j++) {
			int id = (myCardHand.BaDau[i] - 1) * 3 + j;
			ids.push_back(id);
			rots.push_back(startRot + k * rot);
			groups.push_back(constant::CARD_GROUP_3DAU);
			k++;
		}
	}
	for (int i = 0; i < myCardHand.Que.size(); i++) {
		ids.push_back(myCardHand.Que[i]);
		rots.push_back(startRot + k * rot);
		groups.push_back(constant::CARD_GROUP_QUE);
		k++;
	}

	vector<int> changedCards;
	vector<int> indexs;
	for (int i = 0; i < spHandCards.size(); i++) {
		indexs.push_back(i);
	}
	for (int i = 0; i < spHandCards.size(); i++) {
		int j = 0;
		for (; j < indexs.size(); j++) {
			if (atoi(spHandCards[i]->getName().c_str()) == groups[indexs[j]] + ids[indexs[j]]) {
				spHandCards[i]->setLocalZOrder(constant::GAME_ZORDER_HAND_CARD + indexs[j]);
				RotateTo* rotate = RotateTo::create(.5f, rots[indexs[j]]);
				spHandCards[i]->runAction(rotate);
				break;
			}
		}
		if (j == indexs.size()) {
			changedCards.push_back(i);
		} else {
			indexs.erase(indexs.begin() + j);
		}
	}

	for (int i = 0; i < changedCards.size(); i++) {
		for (int j = 0; j < indexs.size(); j++) {
			if (atoi(spHandCards[changedCards[i]]->getName().c_str()) % 1000 == ids[indexs[j]]) {
				spHandCards[changedCards[i]]->setName(to_string(groups[indexs[j]] + ids[indexs[j]]));
				spHandCards[changedCards[i]]->setLocalZOrder(constant::GAME_ZORDER_HAND_CARD + indexs[j]);
				if (rots[indexs[j]] - spHandCards[changedCards[i]]->getRotation() < 180) {
					RotateTo* rotate = RotateTo::create(.5f, rots[indexs[j]]);
					spHandCards[changedCards[i]]->runAction(rotate);
				} else {
					RotateBy* rotate = RotateBy::create(.5f, rots[indexs[j]] - spHandCards[changedCards[i]]->getRotation());
					spHandCards[changedCards[i]]->stopAllActions();
					spHandCards[changedCards[i]]->runAction(rotate);
				}
				break;
			}
		}
	}
}

void GameScene::showStiltCards()
{
	if (state == NONE || state == READY) return;
	endLayer->removeAllChildren();
	btnXemNoc->setVisible(false);
	btnDongNoc->setVisible(true);
	for (Sprite* sp : spCards) {
		sp->setVisible(false);
	}
	int size = endMatchData.ListStiltCard.size() > 20 ? 23 : 25;
	float scale = endMatchData.ListStiltCard.size() > 20 ? .92f : 1;
	int x = 575 - endMatchData.ListStiltCard.size() * size;
	Sprite* sp;
	for (unsigned char c : endMatchData.ListStiltCard) {
		//sp = getCardSprite(c);
		int cardName = getCardName(c);
		sp = Sprite::create(String::createWithFormat("cards/%d.png", getCardName(c))->getCString());
		endLayer->addChild(sp);

		sp->setScale(scale);
		sp->setPosition(x, 125);
		sp->setLocalZOrder(constant::ZORDER_POPUP + 10);
		x += size * 2;
	}
	sp->setPosition(x - 26, 125);
}

void GameScene::showWinnerCards()
{
	if (state == NONE || state == READY) return;
	endLayer->removeAllChildren();
	btnXemNoc->setVisible(true);
	btnDongNoc->setVisible(false);
	for (Sprite* sp : spCards) {
		sp->setVisible(false);
	}
	int x1 = 285;// 560 - endMatchData.ListChanU.size() * 46 + 23;
	if (endMatchData.ListChanU.size() == 10) {
		x1 += 50;
	}
	for (unsigned char c : endMatchData.ListChanU) {
		Color3B color;
		if (c > 100) {
			c = 256 - c;
			color = Color3B(200, 200, 255);
		} else {
			color = Color3B::WHITE;
		}
		for (int i = 0; i < 2; i++) {
			//Sprite* sp = getCardSprite(c);
			int cardName = getCardName(c);
			Sprite* sp = Sprite::create(String::createWithFormat("cards/%d.png", getCardName(c))->getCString());
			endLayer->addChild(sp);

			sp->setScale(1);
			sp->setRotation(0);
			sp->setColor(i == 0 ? color : Color3B::WHITE);
			sp->setPosition(x1, i == 0 ? 150 : 100);
			sp->setAnchorPoint(Vec2(.5f, .5f));
			sp->setLocalZOrder(constant::ZORDER_POPUP + 10 + i);
		}
		x1 += 50;
	}
	int x2 = x1 - 50;// 560 - endMatchData.ListCaU.size() * 23 + 23;
	if (endMatchData.ListCaU.size() > 0) {
		x2 += (endMatchData.ListCaU.size() / 2 + 1) * 50;
		for (int i = endMatchData.ListCaU.size() - 1; i >= 0; i--) {
			Color3B color;
			if (endMatchData.ListCaU[i] > 100) {
				color = Color3B(200, 200, 255);
				endMatchData.ListCaU[i] = 256 - endMatchData.ListCaU[i];
			} else {
				color = Color3B::WHITE;
			}
			//Sprite* sp = getCardSprite(endMatchData.ListCaU[i]);
			int cardName = getCardName(endMatchData.ListCaU[i]);
			Sprite* sp = Sprite::create(String::createWithFormat("cards/%d.png", getCardName(endMatchData.ListCaU[i]))->getCString());
			endLayer->addChild(sp);

			sp->setScale(1);
			sp->setRotation(0);
			sp->setColor(color);
			sp->setPosition(x2, i % 2 == 0 ? 100 : 150);
			sp->setAnchorPoint(Vec2(.5f, .5f));
			sp->setLocalZOrder(constant::ZORDER_POPUP + 10 + (1 - i % 2));
			if(i % 2 == 0) x2 -= 50;
		}
	}
	if (endMatchData.ListCardU.size() > 0) {
		int x3 = x2 + (endMatchData.ListCaU.size() / 2 + 2) * 50;
		int y3 = 150;
		for (int i = 0; i < endMatchData.ListCardU.size(); i ++) {
			Color3B color;
			if (endMatchData.ListCardU[i] > 100) {
				color = Color3B(200, 200, 255);
				endMatchData.ListCardU[i] = 256 - endMatchData.ListCardU[i];
			} else {
				color = Color3B::WHITE;
			}
			//Sprite* sp = getCardSprite(endMatchData.ListCardU[i]);
			int cardName = getCardName(endMatchData.ListCardU[i]);
			Sprite* sp = Sprite::create(String::createWithFormat("cards/%d.png", getCardName(endMatchData.ListCardU[i]))->getCString());
			endLayer->addChild(sp);

			sp->setScale(1);
			sp->setRotation(0);
			sp->setColor(color);
			sp->setPosition(x3, y3);
			sp->setAnchorPoint(Vec2(.5f, .5f));
			sp->setLocalZOrder(constant::ZORDER_POPUP + 10 + i);
			//x3 += 46;
			y3 -= 50;
			if (y3 < 100) {
				x3 += 50;
				y3 = 150;
			}
		}
	}

	/*for (Sprite* sp : spCards) {
		if (sp->isVisible() && sp->getParent() == playLayer) {
			sp->setParent(nullptr);
			endLayer->addChild(sp);
			playLayer->removeChild(sp);
		}
	}*/
}

void GameScene::showSettings()
{
	showPopup(popupSettings);

	int opacity = myServerSlot == 0 ? 255 : 200;
	for (int i = 0; i < 3; i++) {
		auto cb = (ui::CheckBox*)popupSettings->getChildByTag(i);
		//cb->setEnabled(myServerSlot == 0 && (state == NONE || state == READY));
		cb->setOpacity(opacity);
	}
	auto btnNo = (ui::Button*)popupSettings->getChildByName("btnno");
	btnNo->setVisible(!(myServerSlot == 0 && (state == NONE || state == READY)));
}

void GameScene::showError(std::string msg)
{
	//showPopup(lbError->getParent());
	//lbError->setString(msg);

	//DelayTime* delay = DelayTime::create(2);
	//CallFunc* func = CallFunc::create([=]() {
	//	hidePopup(lbError->getParent());
	//});
	////lbError->getParent()->stopAllActions();
	//lbError->getParent()->runAction(Sequence::create(delay, func, nullptr));

	showSystemNotice(msg);
}

void GameScene::showSystemNotice(std::string msg)
{
	showToast(msg, Vec2(560, 400), Color3B::BLACK, Color3B(200, 200, 200));
}

void GameScene::playSoundAction(unsigned char soundId)
{
	if (!Utils::getSingleton().SoundEnabled || soundId > vecSoundActions.size()) {
		return;
	}
	string soundName = "soundaction/" + vecSoundActions[soundId] + ".mp3";
	experimental::AudioEngine::play2d(soundName.c_str());
}

void GameScene::playSoundCuoc(unsigned char cuocId)
{
	if (!Utils::getSingleton().SoundEnabled || cuocId > vecSoundCuocs.size()) {
		return;
	}
	string soundName = "soundxuong/" + vecSoundCuocs[cuocId] + ".mp3";
	experimental::AudioEngine::play2d(soundName.c_str());
}

void GameScene::changeZOrderAfterFly(Sprite * card, int zorder)
{
	int tmp = card->getLocalZOrder();
	card->setLocalZOrder(zorder);
	DelayTime* delay = DelayTime::create(cardSpeed);
	CallFunc* func = CallFunc::create([=]() {
		card->setLocalZOrder(tmp);
		if (card != runningSpCard && card->getScale() == cardScaleTableNew) {
			card->setScale(cardScaleTable);
		}
	});
	card->runAction(Sequence::create(delay, func, nullptr));
}

void GameScene::delayFunction(Node * node, float time, std::function<void()> func)
{
	DelayTime* delay = DelayTime::create(time);
	CallFunc* callfunc = CallFunc::create(func);
	node->runAction(Sequence::create(delay, callfunc, nullptr));
}

void GameScene::beatenNodeAndHide(cocos2d::Node * node, float scale1, float scale2, float timeToBeaten, float timeToHide)
{
	node->setVisible(true);
	Vec2 scale = Vec2(node->getScaleX(), node->getScaleY());
	Vec2 scale11 = getScaleSmoothly(scale1);
	Vec2 scale22 = getScaleSmoothly(scale2);
	ScaleTo* scaleTo1 = ScaleTo::create(timeToBeaten, scale11.x, scale11.y);
	ScaleTo* scaleTo2 = ScaleTo::create(timeToBeaten, scale22.x, scale22.y);
	node->runAction(RepeatForever::create(Sequence::create(scaleTo1, scaleTo2, nullptr)));

	DelayTime* delay = DelayTime::create(timeToHide);
	CallFunc* func = CallFunc::create([=]() {
		node->setScale(scale.x, scale.y);
		node->setVisible(false);
	});
	node->runAction(Sequence::create(delay, func, nullptr));
}

int GameScene::getCardName(unsigned char cardId)
{
	if (cardId == 0) return 0;
	return (cardId / 3 + 1) * 10 + cardId % 3 + 1;
}

void GameScene::onConnectionFailed()
{
	SFSRequest::getSingleton().Disconnect();
	Utils::getSingleton().goToLoginScene();
	experimental::AudioEngine::stopAll();
}

void GameScene::onConnectionLost(std::string reason)
{
	experimental::AudioEngine::stopAll();
	mustGoToLobby = myServerSlot < 0 || state == NONE || state == READY;
	btnBack->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
	handleClientDisconnectionReason(reason);
}

void GameScene::onUserDataResponse(UserData data)
{
	showPopupUserInfo(data);
}

void GameScene::onUserExitRoom(long sfsUId)
{
	if (sfsUId == sfsIdMe) {
		btnBack->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
		if (isKickForNotReady) {
			showPopupNotice(Utils::getSingleton().getStringForKey("bi_thoat_do_khong_san_sang"), [=]() {
				SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
				Utils::getSingleton().goToLobbyScene();
			}, false);
		}else if (isReconnecting) {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
		} else {
			showPopupNotice(Utils::getSingleton().getStringForKey("bi_day_khoi_ban"), [=]() {
				SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
				Utils::getSingleton().goToLobbyScene();
			}, false);
		}
		experimental::AudioEngine::stopAll();
		return;
	}
	int index = userIndexs[sfsUId];
	if (vecUsers[index]->isVisible() && vecUsers[index]->getAlpha() == 150) {
		vecUsers[index]->setName("");
		vecUsers[index]->setVisible(false);
		spInvites[index]->setVisible(true);
		spSanSangs[index]->setVisible(false);
	}
	if (progressTimer->getTag() == index) {
		progressTimer->stopAllActions();
		progressTimer->setVisible(false);
	}
}

void GameScene::onErrorResponse(unsigned char code, std::string msg)
{
	BaseScene::onErrorResponse(code, msg);
	if (code == 31 || code == 30 || code == 29) {
		showSystemNotice(msg);
		return;
	}
	if (code == 19) {
		isKickForNotReady = true;
		/*unregisterEventListenner();
		showPopupNotice(Utils::getSingleton().getStringForKey("bi_thoat_do_khong_san_sang"), [=]() {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
		}, false);*/
		return;
	}
	showError(msg);
}

void GameScene::onPublicMessage(long uid, std::string msg)
{
	int index = userIndexs[uid];
	if (index < 0 || index > 3) return;
	showToast(msg, vecUserPos[index] + Vec2(0, 30));
}

void GameScene::onRoomDataResponse(RoomData roomData)
{
	if (noaction >= 3 || hasRegisterOut) {
		gameSplash->setVisible(false);
		tableEndMatch->setVisible(false);
		if (hasRegisterOut) {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
			experimental::AudioEngine::stopAll();
		} else {
			unregisterEventListenner();
			showPopupNotice(Utils::getSingleton().getStringForKey("bi_day_ra_vi_khong_thao_tac"), [=]() {
				SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
				Utils::getSingleton().goToLobbyScene();
				experimental::AudioEngine::stopAll();
			}, false);
		}
		return;
	}
	vecPlayers = roomData.Players;
	if(roomData.TimeStart > 0) timeStart = roomData.TimeStart;
	timeDeal = roomData.TimeDeal;
	timeChooseHost = roomData.TimeChooseHost;
	myServerSlot = -1;
	for (PlayerData player : roomData.Players) {
		if (player.Info.UserID == Utils::getSingleton().userDataMe.UserID) {
			myServerSlot = player.Index;
			sfsIdMe = player.Info.SfsUserId;
		}
	}
	if (myServerSlot == -1) {
		tableCardPos[0] = tableCardPos[8];
	} else {
		tableCardPos[0] = tableCardPos[9];
	}
	for (int i = 0; i < 4; i++) {
		spInvites[i]->setVisible(true);
		vecUsers[i]->setVisible(false);
		spBatBaos[i]->setVisible(false);
		spSanSangs[i]->setVisible(false);
	}
	int num = 0;
	for (int i = 0; i < 4; i++) {
		int index = -1;
		for (PlayerData player : roomData.Players) {
			if (player.Index == (myServerSlot + i) % vecUsers.size()) {
				index = i - num;
				if (roomData.Players.size() == 2 && (index == 1 || index == 3)) {
					index = 2;
				}
				userIndexs[player.Info.SfsUserId] = index;
				userIndexs2[player.Info.UserID] = index;
				spInvites[index]->setVisible(false);
				vecUsers[index]->setVisible(true);
				vecUsers[index]->setAlpha(255);
				vecUsers[index]->setPlayerName(player.Info.DisplayName);
				vecUsers[index]->setPlayerMoney(player.PMoney);
				vecUsers[index]->setName(player.Info.Name);
				spSanSangs[index]->setVisible(player.Ready);
				if (player.Index == 0) {
					spChuPhong->setVisible(true);
					spChuPhong->setPosition(vecUserPos[index] + Vec2(50 * scaleScene.y, 0));
				}
				if (player.Info.SfsUserId == sfsIdMe) {
					bool isAutoReady = UserDefault::getInstance()->getBoolForKey(constant::KEY_AUTO_READY.c_str());
					if (isAutoReady && !player.Ready) {
						state = READY;
						SFSRequest::getSingleton().RequestGameReady();
					} else {
						btnReady->setVisible(!player.Ready);
						btnCancelReady->setVisible(player.Ready);
					}
				}
			}
		}
		if (index == -1) {
			num++;
		}
	}

	if (roomData.Players.size() == 1) {
		lbCrestTime->setVisible(false);
		btnReady->setVisible(false);
		btnCancelReady->setVisible(false);
		spSanSangs[0]->setVisible(false);
	} else {
		if (roomData.TimeStart > 0 && !lbCrestTime->isVisible()) {
			Vec2 lbscale = getScaleSmoothly(1.5f);
			lbCrestTime->setVisible(true);
			lbCrestTime->setScale(lbscale.x, lbscale.y);
			lbCrestTime->setColor(Color3B::RED);
			lbCrestTime->setString(to_string((int)timeStart));
			lbCrestTime->resumeSchedulerAndActions();
		}
	}

	state = roomData.TimeStart > 0 ? NONE : READY;
	chosenStilt = -1;
	chosenStiltHost = -1;
	chosenHost = -1;
	chosenCard = -1;
	isBatBao = false;
	hostCard = nullptr;
	lbCardNoc->setString("23");
	btnXemNoc->setVisible(false);
	btnDongNoc->setVisible(false);
	tableCrest->setVisible(false);
	tableEndMatch->setVisible(false);
	lbCardNoc->getParent()->setVisible(false);
	lbCardNoc->getParent()->getChildByName("spcardnoc")->setVisible(true);
	gameSplash->setVisible(false);
	progressTimer->stopAllActions();
	progressTimer->setVisible(false);
	btnBack->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
	spHandCards.clear();
	chosenCuocs.clear();
	chosenCuocNumbs.clear();
	endLayer->removeAllChildren();
	for (int i = 0; i < tableCardNumb.size(); i++) {
		tableCardNumb[i] = 0;
	}
	for (Sprite* sp : spCards) {
		sp->setVisible(false);
	}
	for (int i = 0; i < vecStilts.size(); i++) {
		vecStilts[i]->setVisible(true);
		//vecStilts[i]->setPosition(dealPos[i] + Vec2(560, 350));
		vecStilts[i]->setPosition(dealPos[i]);
		vecStilts[i]->setRotation(0);
	}
	nodeStilt->setRotation(0);

	if (winMoneyData.ListUserId.size() > 0) {
		for (int i = 0; i < winMoneyData.ListUserId.size(); i++) {
			int index = userIndexs2[winMoneyData.ListUserId[i]];
			FadeIn* fadeIn = FadeIn::create(.2f);
			MoveTo* move = MoveTo::create(.5f, vecUserPos[index] + Vec2(0, 40));
			DelayTime* delay = DelayTime::create(1.5f);
			FadeOut* fadeOut = FadeOut::create(.5f);

			std::string str1 = Utils::getSingleton().formatMoneyWithComma(winMoneyData.ListUserAmount[i]);// > 0 ? money : -money);
			std::string moneystr = String::createWithFormat(winMoneyData.ListUserAmount[i] > 0 ? "+%s" : "%s", str1.c_str())->getCString();
			lbWinMoneys[index]->enableOutline(Color4B::BLACK, 1);
			lbWinMoneys[index]->setColor(winMoneyData.ListUserAmount[i] > 0 ? Color3B::YELLOW : Color3B::RED);
			lbWinMoneys[index]->setPosition(vecUserPos[index] + Vec2(0, -40));
			lbWinMoneys[index]->setString(moneystr);
			lbWinMoneys[index]->runAction(fadeIn);
			lbWinMoneys[index]->runAction(Sequence::create(move, delay, fadeOut, nullptr));
		}
		winMoneyData.ListUserId.clear();
		winMoneyData.ListUserAmount.clear();
	}
}

void GameScene::onRoomDataGaResponse(bool isGa, double gaMoney)
{
	if (isWaiting) {
		hideWaiting();
	}
	iconGa->setTag(isGa);
	iconGa->initWithFile(isGa ? "board/btn_ga_on.png" : "board/btn_ga_off.png");
	if (isGa) {
		lbMoneyGa->setString(Utils::getSingleton().formatMoneyWithComma(gaMoney));
	} else {
		lbMoneyGa->setString("");
	}
}

void GameScene::onStartGameDataResponse(StartGameData data)
{
    if(data.CardStilt < 0){
        data.CardStilt = -data.CardStilt;
    }
	state = START;
	this->startGameData = data;
	this->myCardHand = data.CardHand;
	btnReady->setVisible(false);
	btnCancelReady->setVisible(false);
	lbCrestTime->setVisible(false);
	lbCrestTime->pauseSchedulerAndActions();
	for (Sprite* sp : spSanSangs) {
		sp->setVisible(false);
	}
	dealCards();
	noaction = 0;
}

void GameScene::onChooseStilt(unsigned char stilt)
{
	//if (myServerSlot < 0) return;
	if (!vecStilts[0]->isVisible() && !vecStilts[1]->isVisible()) return;
	if (state == DEAL) {
		experimental::AudioEngine::stopAll();
		this->stopAllActions();
		Node* n1 = spDealCards[0]->getParent();
		Node* n2 = spDealCards[1]->getParent();
		for (Sprite* sp : spDealCards) {
			sp->stopAllActions();
			int i = atoi(sp->getName().c_str());
			sp->setParent(nullptr);
			sp->setRotation(rand() % 60 - 30);
			sp->setPosition(rand() % 20 - 10, rand() % 20 - 10);
			vecStilts[i]->addChild(sp);
		}
		n1->removeAllChildren();
		n2->removeAllChildren();
		state == CHOOSE_STILT;
	}

	chosenStilt = stilt - 1;
	vecStilts[chosenStilt]->setVisible(false);
	hostCard = Sprite::create("cards/100.png");
	hostCard->setName("hostCard");
	hostCard->setScale(.7f);
	hostCard->setPosition(vecStilts[chosenStilt]->getPosition() + getScaleScenePosition(Vec2(560, 350)));
	playLayer->addChild(hostCard, constant::GAME_ZORDER_USER + 10);

	if (startGameData.LastWinner == sfsIdMe) {
		lbNoticeAction->setVisible(false);
		//lbNoticeAction->setString(Utils::getSingleton().getStringForKey("choose_host"));
	}
}

void GameScene::onChooseHost(unsigned char stilt1, unsigned char stilt2, unsigned char host)
{
	//if (myServerSlot < 0) return;
	if (!vecStilts[0]->isVisible() && !vecStilts[1]->isVisible()) return;
	progressTimer->stopAllActions();
	progressTimer->setVisible(false);
	lbNoticeAction->setVisible(false);

	if (hostCard == nullptr) {
		onChooseStilt(stilt1);
	}

	if (chosenStiltHost < 0 || chosenStiltHost >= vecStilts.size()) {
		while ((chosenStiltHost = rand() % vecStilts.size()) == stilt1 - 1);
	}
	//chosenHost = (startGameData.CardStilt / 3 + userIndexs[startGameData.LastWinner] - myServerSlot + vecPlayers.size()) % vecPlayers.size();

	int winnerIndex = userIndexs[startGameData.LastWinner];
	if (vecPlayers.size() == 2 && winnerIndex == 2 ) {
		winnerIndex = 1;
	}

	chosenHost = (startGameData.CardStilt / 3 + winnerIndex) % vecPlayers.size();

	if (vecPlayers.size() == 2 && (chosenHost == 1 || chosenHost == 3)) {
		chosenHost = 2;
	}

	//MoveTo* move = MoveTo::create(.5f, vecStilts[chosenStiltHost]->getPosition());
	MoveTo* move = MoveTo::create(.5f, vecStilts[chosenStiltHost]->getPosition() + getScaleScenePosition(Vec2(560, 350)));
	CallFunc* func = CallFunc::create([=]() {
		int card = getCardName(startGameData.CardStilt);
		hostCard->initWithFile(String::createWithFormat("cards/%d.png", card)->getCString());
		hostCard->setParent(nullptr);
		hostCard->setPosition(0, 0);
		vecStilts[chosenStiltHost]->addChild(hostCard);
		playLayer->removeChildByName("hostCard");
	});
	hostCard->runAction(Sequence::create(move, func, nullptr));

	DelayTime* delay1 = DelayTime::create(1);
	CallFunc* func1 = CallFunc::create([=]() {
		spChonCai->setVisible(false);
		state = PLAY;

		//int p = 0;
		//for (int i = 0; i < vecStilts.size(); i++) {
		//	int k = (chosenStiltHost + i) % vecStilts.size();
		//	if (k == chosenStilt) {
		//		p = 1;
		//		continue;
		//	}
		//	int m = (chosenHost + i - p) % handPos.size();
		//	DelayTime* delay2 = DelayTime::create(.1f);
		//	//MoveTo* move2 = MoveTo::create(.5f, handPos[m]);
		//	MoveTo* move2 = MoveTo::create(.5f, handPos[m] - Vec2(560, 350));
		//	vecStilts[k]->runAction(Sequence::create(delay2, move2, nullptr));
		//}

		if (chosenStiltHost == chosenHost + 1) {
			int j = chosenHost;
			for (int i = 0; i < 5; i++) {
				Node* n = vecStilts[(chosenStiltHost + i) % vecStilts.size()];
				if (!n->isVisible()) continue;
				DelayTime* delay2 = DelayTime::create(.1f);
				MoveTo* move2 = MoveTo::create(.5f, handPos[j] - Vec2(560, 350));
				n->runAction(Sequence::create(delay2, move2, nullptr));
				j = (j + 1) % handPos.size();
			}
		} else {
			int i = 0;
			int tmpChosen = chosenStiltHost < chosenStilt ? chosenStiltHost : (chosenStiltHost - 1);
			int diff = (chosenHost + handPos.size() - tmpChosen) % handPos.size();
			int rotation = diff * 90;
			float runTime = diff * .5f + .5f;
			if (diff == 0) runTime = .5f;
			for (Node* n : vecStilts) {
				if (n->isVisible()) {
					DelayTime* delay2 = DelayTime::create(.1f);
					//MoveTo* move2 = MoveTo::create(.5f, handPos[i++] - Vec2(560, 350));
					MoveTo* move2 = MoveTo::create(runTime, handPos[i++] - Vec2(560, 350));
					n->runAction(Sequence::create(delay2, move2, nullptr));
					if (diff > 0) {
						//DelayTime* delay21 = DelayTime::create(1.1f);
						DelayTime* delay21 = DelayTime::create(.1f);
						RotateBy* rot21 = RotateBy::create(runTime, rotation);
						n->runAction(Sequence::create(delay21, rot21, nullptr));
					}
				}
			}

			if (diff > 0) {
				//DelayTime* delay22 = DelayTime::create(1.1f);
				DelayTime* delay22 = DelayTime::create(.1f);
				RotateBy* rot22 = RotateBy::create(runTime, -rotation);
				nodeStilt->runAction(Sequence::create(delay22, rot22, nullptr));
			}
		}

		DelayTime* delay3 = DelayTime::create(3);
		CallFunc* func3 = CallFunc::create([=]() {
			spDealCards.clear();
			for (Node* n : vecStilts) {
				n->setVisible(false);
				n->removeAllChildren();
			}
			showMyCards();
			lbCardNoc->getParent()->setVisible(true);
		});
		runAction(Sequence::create(delay3, func3, nullptr));
	});
	runAction(Sequence::create(delay1, func1, nullptr));

	if (!startGameData.CanWin) {
		DelayTime* delay4 = DelayTime::create(5);
		CallFunc* func4 = CallFunc::create([=]() {
			if (startGameData.CurrentTurn == sfsIdMe) {
				btnBash->setVisible(true);
			}
			runTimeWaiting(startGameData.CurrentTurn, timeTurn);
		});
		btnBash->runAction(Sequence::create(delay4, func4, nullptr));
	}
}

void GameScene::onUserBash(BashData data)
{
	Sprite* spCard = NULL;
	int zorder = 0;
	int index = userIndexs[data.UId];
	int index2 = index * 2;
	float scale = 1.0f;
	if (tableCardNumb[index2] >= maxTableCardNumb[index2]) {
		scale = ((float)maxTableCardNumb[index2] - 1) / tableCardNumb[index2];
		for (Sprite* sp : spCards) {
			if (sp->isVisible() && sp->getTag() == constant::TAG_CARD_TABLE + index2) {
				int index = sp->getLocalZOrder() - constant::GAME_ZORDER_TABLE_CARD - 1;
				Vec2 newPos = tableCardPos[index2] + index * tableCardDistance[index2] * scale;
				MoveTo* move = MoveTo::create(.2f, newPos);
				sp->runAction(move);
			}
		}
	}
	Vec2 pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale;
	tableCardNumb[index2] ++;
	if (data.UId == sfsIdMe) {
		if (chosenCard > 0 && atoi(spHandCards[chosenCard]->getName().c_str()) % 1000 == data.CardId) {
			spCard = spHandCards[chosenCard];
			spHandCards.erase(spHandCards.begin() + chosenCard);
			chosenCard = -1;
		} else {
			if (chosenCard >= 0) {
				spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.2f));
			}
			for (int i = 0; i < spHandCards.size(); i++) {
				if (atoi(spHandCards[i]->getName().c_str()) % 1000 == data.CardId) {
					spCard = spHandCards[i];
					spHandCards.erase(spHandCards.begin() + i);
					break;
				}
			}
		}
		if (spCard == NULL) return;
		int rot = spCard->getRotation();
		Vec2 scaledUserPos = getScaleScenePosition(vecUserPos[index]);
		float x = sin(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.x;
		float y = cos(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.y;
		spCard->setPosition(x, y);
		spCard->setName(to_string((int)data.CardId));
		RotateTo* rotate = RotateTo::create(cardSpeed, 0);
		spCard->stopAllActions();
		spCard->runAction(rotate);
		zorder = spCard->getLocalZOrder();
	} else {
		spCard = getCardSprite(data.CardId);
		spCard->setRotation(0);
		spCard->setScale(cardScaleTableNew);
		spCard->setPosition(getScaleScenePosition(vecUserPos[index]));
	}

	spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2]);
	spCard->setTag(constant::TAG_CARD_TABLE + index2);
	spCard->setName(to_string((int)data.CardId));
	spCard->setAnchorPoint(Vec2(.5f,.5f));

	MoveTo* move = MoveTo::create(cardSpeed, pos);
	ScaleTo* scaleTo = ScaleTo::create(cardSpeed, cardScaleTableNew);
	spCard->runAction(move);
	spCard->runAction(scaleTo);
	changeZOrderAfterFly(spCard, constant::GAME_ZORDER_CARD_FLY);

	if(runningSpCard != NULL) runningSpCard->setScale(cardScaleTable);
	runningSpCard = spCard;
	if (data.UId == sfsIdMe) {
		btnBash->setVisible(false);
		updateCardHand(data.CardHand);
	}
	if (!isBatBao) {
		if (data.CanPenetWin) {
			if (isU411) {
				noaction++;
				beatenNodeAndHide(btnWin, 1.1f, .9f, .7f, 10);
				btnDropWin->setVisible(true);
				delayFunction(btnDropWin, 10, [=]() {
					btnDropWin->setVisible(false);
				});
				if (data.TurnId == sfsIdMe) {
					waitAction = constant::GAME_ACTION_BASH;
				}
			}
		} else if (data.CanPenet) {
			beatenNodeAndHide(btnPenet, 1.2f, 1, .7f, 7);
            btnDropPenet->setVisible(true);
            delayFunction(btnDropPenet, 7, [=]() {
                btnDropPenet->setVisible(false);
            });
            if (data.TurnId == sfsIdMe) {
                waitAction = constant::GAME_ACTION_BASH;
            }
		} else if (data.TurnId == sfsIdMe) {
			noaction++;
			btnHold->setVisible(true);
			btnPick->setVisible(true);
		}
	}
	runTimeWaiting(data.TurnId, timeTurn);
	playSoundAction(data.SoundId);
	for (Sprite* sp : spHandCards) {
		if (sp->getScale() < 1.2f) {
			sp->setScale(1.2f);
		}
	}
}

void GameScene::onUserBashBack(BashBackData data)
{
	Sprite* spCard = NULL;
	int zorder = 0;
	int index = userIndexs[data.UId];
	int index1 = userIndexs[data.BackId];
	int index2 = index1 * 2;
	float scale = 1.0f;
	if (tableCardNumb[index2] >= maxTableCardNumb[index2]) {
		scale = ((float)maxTableCardNumb[index2] - 1) / tableCardNumb[index2];
		for (Sprite* sp : spCards) {
			if (sp->isVisible() && sp->getTag() == constant::TAG_CARD_TABLE + index2) {
				int index = sp->getLocalZOrder() - constant::GAME_ZORDER_TABLE_CARD - 1;
				Vec2 newPos = tableCardPos[index2] + index * tableCardDistance[index2] * scale;
				MoveTo* move = MoveTo::create(.2f, newPos);
				sp->runAction(move);
			}
		}
	}
	Vec2 pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale;
	tableCardNumb[index2] ++;
	if (data.UId == sfsIdMe) {
		if (chosenCard > 0 && atoi(spHandCards[chosenCard]->getName().c_str()) % 1000 == data.CardId) {
			spCard = spHandCards[chosenCard];
			spHandCards.erase(spHandCards.begin() + chosenCard);
			chosenCard = -1;
		} else {
			if (chosenCard >= 0) {
				spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.2f));
			}
			for (int i = 0; i < spHandCards.size(); i++) {
				if (atoi(spHandCards[i]->getName().c_str()) % 1000 == data.CardId) {
					spCard = spHandCards[i];
					spHandCards.erase(spHandCards.begin() + i);
					break;
				}
			}
		}
		if (spCard == NULL) return;
		int rot = spCard->getRotation();
		Vec2 scaledUserPos = getScaleScenePosition(vecUserPos[index]);
		float x = sin(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.x;
		float y = cos(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.y;
		spCard->setPosition(x, y);
		spCard->setName(to_string((int)data.CardId));
		RotateTo* rotate = RotateTo::create(cardSpeed, 0);
		spCard->stopAllActions();
		spCard->runAction(rotate);
		zorder = spCard->getLocalZOrder();
	} else {
		spCard = getCardSprite(data.CardId);
		spCard->setRotation(0);
		spCard->setScale(cardScaleTableNew);
		spCard->setPosition(getScaleScenePosition(vecUserPos[index]));
	}

	spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2]);
	spCard->setTag(constant::TAG_CARD_TABLE + index2);
	spCard->setName(to_string((int)data.CardId));
	spCard->setAnchorPoint(Vec2(.5f, .5f));

	MoveTo* move = MoveTo::create(cardSpeed, pos);
	ScaleTo* scaleTo = ScaleTo::create(cardSpeed, cardScaleTableNew);
	spCard->runAction(move);
	spCard->runAction(scaleTo);
	changeZOrderAfterFly(spCard, constant::GAME_ZORDER_CARD_FLY);

	if(runningSpCard != nullptr) 
		runningSpCard->setScale(cardScaleTable);
	runningSpCard = spCard;
	if (data.UId == sfsIdMe) {
		btnBashBack->setVisible(false);
		progressTimer->setVisible(false);
		progressTimer->stopAllActions();
		updateCardHand(data.CardHand);
	}
	if (!isBatBao) {
		if (data.CanPenetWin) {
			if (isU411) {
				noaction++;
				beatenNodeAndHide(btnWin, 1.1f, .9f, .7f, 10);
				btnDropWin->setVisible(true);
				delayFunction(btnDropWin, 10, [=]() {
					btnDropWin->setVisible(false);
				});
				if (data.TurnId == sfsIdMe) {
					if (data.IsPicked) {
						waitAction = constant::GAME_ACTION_PICK;
					} else {
						waitAction = constant::GAME_ACTION_BASH;
					}
				}
			}
		} else if (data.CanPenet) {
			beatenNodeAndHide(btnPenet, 1.2f, 1, .7f, 7);
            btnDropPenet->setVisible(true);
            delayFunction(btnDropPenet, 7, [=]() {
                btnDropPenet->setVisible(false);
            });
            if (data.TurnId == sfsIdMe) {
                if (data.IsPicked) {
                    waitAction = constant::GAME_ACTION_PICK;
                } else {
                    waitAction = constant::GAME_ACTION_BASH;
                }
            }
		} else if (data.TurnId == sfsIdMe) {
			noaction++;
			btnHold->setVisible(true);
			if (data.IsPicked) {
				if (data.UId != sfsIdMe) {
					btnForward->setVisible(true);
				}
			} else {
				btnPick->setVisible(true);
			}
		}
	}
	runTimeWaiting(data.TurnId, timeTurn);
	playSoundAction(data.SoundId);
}

void GameScene::onUserHold(HoldData data)
{
	int index = userIndexs[data.UId];
	if (runningSpCard == NULL) {
		if (runningCards[index] != NULL && data.CardIdHold == atoi(runningCards[index]->getName().c_str())) {
			runningSpCard = runningCards[index];
		}else if(runningCards.size() > 0){
			int tmpIndex = (index + 3) % 4;
			if (!vecUsers[tmpIndex]->isVisible() || vecUsers[tmpIndex]->getAlpha() < 255) {
				tmpIndex = (tmpIndex + 3) % 4;
			}
			runningSpCard = runningCards[tmpIndex];
		}
	}
	int zorder = 0;
	int index2 = index * 2 + 1;
	int index3 = runningSpCard->getTag() % 100;
	float scale = 1.0f;
	if (tableCardNumb[index2] >= maxTableCardNumb[index2]) {
		scale = ((float)maxTableCardNumb[index2] - 1) / tableCardNumb[index2];
		for (Sprite* sp : spCards) {
			if (sp->isVisible() && sp->getTag() == constant::TAG_CARD_TABLE + index2) {
				int index = (sp->getLocalZOrder() - constant::GAME_ZORDER_TABLE_CARD) / 4 - 1;
				Vec2 newPos = Vec2(tableCardPos[index2].x + index * tableCardDistance[index2].x * scale, sp->getPositionY());
				MoveTo* move = MoveTo::create(.2f, newPos);
				sp->runAction(move);
			}
		}
	}
	Vec2 pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale;
	tableCardNumb[index2] ++;
	tableCardNumb[index3] --;

	Sprite* spCard = NULL;
	if (data.UId == sfsIdMe) {
		if (chosenCard > 0 && atoi(spHandCards[chosenCard]->getName().c_str()) % 1000 == data.CardId) {
			spCard = spHandCards[chosenCard];
			spHandCards.erase(spHandCards.begin() + chosenCard);
			chosenCard = -1;
		} else {
			if (chosenCard >= 0) {
				spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.2f));
				chosenCard = -1;
			}
			for (int i = 0; i < spHandCards.size(); i++) {
				if (atoi(spHandCards[i]->getName().c_str()) % 1000 == data.CardId) {
					spCard = spHandCards[i];
					spHandCards.erase(spHandCards.begin() + i);
					break;
				}
			}
		}
		if (spCard == NULL) return;
		int rot = spCard->getRotation();
		Vec2 scaledUserPos = getScaleScenePosition(vecUserPos[index]);
		float x = sin(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.x;
		float y = cos(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.y;
		spCard->setPosition(x, y);
		spCard->setName(to_string((int)data.CardId));
	} else {
		spCard = getCardSprite(data.CardId);
		spCard->setRotation(0);
		spCard->setScale(cardScaleTable);
		spCard->setPosition(getScaleScenePosition(vecUserPos[index]));
	}

	spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2] * 4 + 1);
	spCard->setTag(constant::TAG_CARD_TABLE + index2);
	spCard->setAnchorPoint(Vec2(.5f, .5f));

	runningSpCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2] * 4);
	runningSpCard->setTag(constant::TAG_CARD_TABLE + index2);
	runningSpCard->setAnchorPoint(Vec2(.5f, .5f));
	runningSpCard->setRotation(0);

	MoveTo* move1 = MoveTo::create(cardSpeed, pos - Vec2(0, 40));
	MoveTo* move2 = MoveTo::create(cardSpeed, pos);
	ScaleTo* scaleTo = ScaleTo::create(cardSpeed, cardScaleTable);
	RotateTo* rotate = RotateTo::create(cardSpeed, 0);
	spCard->stopAllActions();
	spCard->runAction(rotate);
	spCard->runAction(move1);
	spCard->runAction(scaleTo);
	runningSpCard->stopAllActions();
	runningSpCard->setOpacity(255);
	runningSpCard->runAction(move2);
	runningSpCard->runAction(scaleTo->clone());
	changeZOrderAfterFly(spCard, constant::GAME_ZORDER_CARD_FLY + 1);
	changeZOrderAfterFly(runningSpCard, constant::GAME_ZORDER_CARD_FLY);

	runningSpCard = NULL;
	if (data.UId == sfsIdMe) {
		btnHold->setVisible(false);
		btnPick->setVisible(false);
		btnForward->setVisible(false);
		updateCardHand(data.CardHand);
	}
	if (!isBatBao && data.TurnId == sfsIdMe) {
		noaction++;
		btnBash->setVisible(true);
	}
	runTimeWaiting(data.TurnId, timeTurn);
	playSoundAction(data.SoundId);
}

void GameScene::onUserPick(PickData data)
{
	int index = userIndexs[data.UId];
	int index2 = index * 2;
	float scale = 1.0f;
	if (tableCardNumb[index2] >= maxTableCardNumb[index2]) {
		scale = ((float)maxTableCardNumb[index2] - 1) / tableCardNumb[index2];
		for (Sprite* sp : spCards) {
			if (sp->isVisible() && sp->getTag() == constant::TAG_CARD_TABLE + index2) {
				int index = sp->getLocalZOrder() - constant::GAME_ZORDER_TABLE_CARD - 1;
				Vec2 newPos = tableCardPos[index2] + index * tableCardDistance[index2] * scale;
				MoveTo* move = MoveTo::create(.2f, newPos);
				sp->runAction(move);
			}
		}
	}
	Vec2 pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale;
	tableCardNumb[index2] ++;

	lbCardNoc->setString(to_string((int)data.StiltCount));
	Sprite* spCard = getCardSprite(data.CardId);
	spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2]);
	spCard->setTag(constant::TAG_CARD_TABLE + index2);
	spCard->setName(to_string((int)data.CardId));
	spCard->setAnchorPoint(Vec2(.5f, .5f));
	spCard->setPosition(getScaleScenePosition(lbCardNoc->getParent()->getPosition()));
	spCard->setColor(Color3B(200, 200, 255));
	spCard->setRotation(70);
	spCard->setOpacity(0);
	spCard->setScale(.8f);

	MoveTo* move = MoveTo::create(cardSpeed, pos);
	RotateTo* rotate = RotateTo::create(cardSpeed, 0);
	ScaleTo* scaleto = ScaleTo::create(cardSpeed, cardScaleTableNew);
	FadeIn* fade = FadeIn::create(.05f);
	spCard->runAction(move);
	spCard->runAction(rotate);
	spCard->runAction(scaleto);
	spCard->runAction(fade);
	changeZOrderAfterFly(spCard, constant::GAME_ZORDER_CARD_FLY);

	if(runningSpCard != NULL) {
		runningSpCard->setScale(cardScaleTable);
	}
	runningSpCard = spCard;
	if (data.UId == sfsIdMe) {
		btnHold->setVisible(false);
		btnPick->setVisible(false);
		btnWin->setVisible(false);
		btnDropWin->setVisible(false);
		btnPenet->setVisible(false);
        btnDropPenet->setVisible(false);
		btnWin->stopAllActions();
		btnDropWin->stopAllActions();
		btnPenet->stopAllActions();
        btnDropPenet->stopAllActions();
	}
	if (!isBatBao) {
		if (data.CanWin) {
			if (isU411) {
				noaction++;
				beatenNodeAndHide(btnWin, 1.1f, .9f, .7f, 10);
				btnDropWin->setVisible(true);
				delayFunction(btnDropWin, 10, [=]() {
					btnDropWin->setVisible(false);
				});
				if (data.TurnId == sfsIdMe) {
					waitAction = constant::GAME_ACTION_PICK;
				}
			}
		} else if (data.CanPenet) {
			beatenNodeAndHide(btnPenet, 1.2f, 1, .7f, 7);
            btnDropPenet->setVisible(true);
            delayFunction(btnDropPenet, 7, [=]() {
                btnDropPenet->setVisible(false);
            });
            if (data.TurnId == sfsIdMe) {
                waitAction = constant::GAME_ACTION_PICK;
            }
		} else if (data.TurnId == sfsIdMe) {
			noaction++;
			btnHold->setVisible(true);
			btnForward->setVisible(true);
		}
	}
	runTimeWaiting(data.TurnId, timeTurn);
	playSoundAction(data.SoundId);
}

void GameScene::onUserPenet(PenetData data)
{
	if (runningSpCard == NULL) {
		for (Sprite* sp : runningCards) {
			if (sp != NULL && atoi(sp->getName().c_str()) == data.CardId) {
				runningSpCard = sp;
				break;
			}
		}
	}
	if (runningSpCard == NULL) {
		runningSpCard = getCardSprite(data.CardId);
		runningSpCard->setRotation(0);
		runningSpCard->setScale(cardScaleTable);
		runningSpCard->setAnchorPoint(Vec2(.5f, .5f));
		runningSpCard->setPosition(lbCardNoc->getParent()->getPosition());
	}
	vector<int> zorders;
	int index = userIndexs[data.UId];
	int index2 = index * 2 + 1;
	int index3 = runningSpCard->getTag() % 100;
	float scale = 1.0f;
	if (tableCardNumb[index2] >= maxTableCardNumb[index2]) {
		scale = ((float)maxTableCardNumb[index2] - 1) / tableCardNumb[index2];
		for (Sprite* sp : spCards) {
			if (sp->isVisible() && sp->getTag() == constant::TAG_CARD_TABLE + index2) {
				int index = (sp->getLocalZOrder() - constant::GAME_ZORDER_TABLE_CARD) / 4 - 1;
				Vec2 newPos = Vec2(tableCardPos[index2].x + index * tableCardDistance[index2].x * scale, sp->getPositionY());
				MoveTo* move = MoveTo::create(.2f, newPos);
				sp->runAction(move);
			}
		}
	}
	Vec2 pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale + Vec2(0, 5);
	tableCardNumb[index2] ++;
	tableCardNumb[index3] --;

	vector<Sprite*> cards;
	cards.push_back(runningSpCard);
	runningSpCard = NULL;
	if (data.UId == sfsIdMe) {
		for (int i = spHandCards.size() - 1; i >= 0; i--) {
			if (atoi(spHandCards[i]->getName().c_str()) % 1000 == data.CardId) {
				if (i == chosenCard) {
					chosenCard = -1;
				}
				cards.push_back(spHandCards[i]);

				int rot = spHandCards[i]->getRotation();
				Vec2 scaledUserPos = getScaleScenePosition(vecUserPos[index]);
				float x = sin(CC_DEGREES_TO_RADIANS(rot)) * spHandCards[i]->getContentSize().height + scaledUserPos.x;
				float y = cos(CC_DEGREES_TO_RADIANS(rot)) * spHandCards[i]->getContentSize().height + scaledUserPos.y;
				spHandCards[i]->setName(to_string((int)data.CardId));
				spHandCards[i]->setPosition(x, y);

				spHandCards.erase(spHandCards.begin() + i);
				//if (cards.size() == 4) break;
			} else {
				spHandCards[i]->setAnchorPoint(Vec2(.5f, -.2f));
			}
		}
		if (chosenCard >= 0) {
			chosenCard = -1;
		}
	} else {
		for (int i = 0; i < 3; i++) {
			Sprite* spCard = getCardSprite(data.CardId);
			spCard->setRotation(0);
			spCard->setScale(cardScaleTable);
			spCard->setPosition(getScaleScenePosition(vecUserPos[index]));
			cards.push_back(spCard);
		}
	}

	for (int i = 0; i < cards.size(); i++) {
		cards[i]->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2] * 4 + i);
		cards[i]->setTag(constant::TAG_CARD_TABLE + index2);
		cards[i]->setAnchorPoint(Vec2(.5f, .5f));

		MoveTo* move = MoveTo::create(cardSpeed, pos - Vec2(0, 15) * i);
		ScaleTo* scaleTo = ScaleTo::create(cardSpeed, cardScaleTable);
		RotateTo* rotate = RotateTo::create(cardSpeed, 0);
		cards[i]->stopAllActions();
		cards[i]->setOpacity(255);
		cards[i]->runAction(move);
		cards[i]->runAction(scaleTo);
		cards[i]->runAction(rotate);
		changeZOrderAfterFly(cards[i], constant::GAME_ZORDER_CARD_FLY + i);
	}
	if (data.UId == sfsIdMe) {
        btnPenet->stopAllActions();
        btnDropPenet->stopAllActions();
		btnHold->setVisible(false);
		btnPick->setVisible(false);
		btnPenet->setVisible(false);
        btnDropPenet->setVisible(false);
		btnForward->setVisible(false);
		btnBashBack->setVisible(true);
		updateCardHand(data.CardHand);
		runTimeWaiting(data.UId, timeTurn);
	} else {
		btnHold->setVisible(false);
		btnPick->setVisible(false);
		btnForward->setVisible(false);
	}
	playSoundAction(data.SoundId);
}

void GameScene::onUserForward(ForwardData data)
{
	if (data.UId == sfsIdMe) {
		btnHold->setVisible(false);
		btnPick->setVisible(false);
		btnForward->setVisible(false);
		btnWin->setVisible(false);
		btnDropWin->setVisible(false);
		btnPenet->setVisible(false);
        btnDropPenet->setVisible(false);
		btnWin->stopAllActions();
		btnDropWin->stopAllActions();
		btnPenet->stopAllActions();
        btnDropPenet->stopAllActions();

		progressTimer->setVisible(false);
		progressTimer->stopAllActions();
	}
	if (!isBatBao && data.TurnId == sfsIdMe) {
		noaction++;
		btnHold->setVisible(true);
		btnPick->setVisible(true);
	}
	runTimeWaiting(data.TurnId, timeTurn);
	playSoundAction(data.SoundId);
}

void GameScene::onUserWin(long uId, unsigned char sound)
{
	btnBash->setVisible(false);
	btnPenet->setVisible(false);
    btnDropPenet->setVisible(false);
	btnPick->setVisible(false);
	btnHold->setVisible(false);
	btnForward->setVisible(false);
	btnBash->stopAllActions();
	btnWin->stopAllActions();
	btnDropWin->stopAllActions();
	btnPenet->stopAllActions();
    btnDropPenet->stopAllActions();
	if (uId == sfsIdMe) {
		btnWin->setVisible(false);
		btnDropWin->setVisible(false);
		playSoundAction(sound);
        showSystemNotice(Utils::getSingleton().getStringForKey("ban_da_chon_u_roi"));
	} else {
		int index = userIndexs[uId];
		showSystemNotice(vecUsers[index]->getPlayerName() + Utils::getSingleton().getStringForKey("da_chon_u_roi"));
	}
}

void GameScene::onCrestResponse(CrestResponseData data)
{
	if (!btnXemNoc->isVisible() && !btnDongNoc->isVisible()) {
		state == ENDING;
		gameSplash->setVisible(true);
		for (Sprite* sp : spCards) {
			if (sp->isVisible()) {
				sp->setVisible(false);
			}
		}
	}
	lbCrestTime->setVisible(false);
	tableCrest->setVisible(false);
	tableEndMatch->setVisible(true);
	progressTimer->setVisible(false);
	progressTimer->stopAllActions();
	lbCrestTime->pauseSchedulerAndActions();

	string strwin = vecUsers[userIndexs[data.UId]]->getPlayerName() + " " + Utils::getSingleton().getStringForKey("win") + ": ";
	for (unsigned char c : data.CuocHo) {
		strwin += Utils::getSingleton().getStringForKey(string("cuoc_") + to_string((int)c)) + " ";
	}
	lbWinner->setString(strwin);

	string strPoint = to_string(data.Diem) + " " + Utils::getSingleton().getStringForKey("point");
	if(data.Ga > 0) strPoint += " " + to_string(data.Ga) + " " + Utils::getSingleton().getStringForKey("chicken");
	lbDiem->setString(strPoint);

	if (data.CuocSai.size() > 0) {
		string strsai = Utils::getSingleton().getStringForKey("cuoc_sai") + ": ";
		for (unsigned char c : data.CuocSai) {
			strsai += Utils::getSingleton().getStringForKey(string("cuoc_") + to_string((int)c)) + " ";
		}
		lbCuocSai->setString(strsai);
	} else {
		lbCuocSai->setString("");
	}

	if (data.Msg.length() > 0) {
		string strnoted = Utils::getSingleton().getStringForKey("noted") + ": " + data.Msg;
		lbNoted->setString(strnoted);
		lbNotedGa->setPosition(-235, -112);
	} else {
		lbNoted->setString("");
		lbNotedGa->setPosition(lbNoted->getPosition());
	}

	if (data.MsgAnGa.length() > 0) {
		lbNotedGa->setString(data.MsgAnGa);
	} else {
		lbNotedGa->setString("");
	}

	for (int i = 0; i < data.CuocHo.size(); i++) {
		DelayTime* delay = DelayTime::create(1 + i * 1);
		CallFunc* func = CallFunc::create([=]() {
			playSoundCuoc(data.CuocHo[i]);
		});
		tableEndMatch->runAction(Sequence::create(delay, func, nullptr));
	}
}

void GameScene::onEndMatch(EndMatchData data)
{
	if (state == NONE || state == READY) return;
	state = ENDING;
	this->endMatchData = data;
	gameSplash->setVisible(true);
	lbCrestTime->setVisible(true);
	lbCrestTime->setString("40");
	Vec2 lbscale = getScaleSmoothly(1);
	lbCrestTime->setScale(lbscale.x, lbscale.y);
	lbCrestTime->setColor(Color3B::WHITE);
	lbCrestTime->resumeSchedulerAndActions();
	lbCardNoc->getParent()->setVisible(false);
	btnPick->setVisible(false);
	btnBash->setVisible(false);
	btnHold->setVisible(false);
	btnForward->setVisible(false);
	btnWin->setVisible(false);
	btnDropWin->setVisible(false);
	btnPenet->setVisible(false);
    btnDropPenet->setVisible(false);
	btnWin->stopAllActions();
	btnDropWin->stopAllActions();
	btnPenet->stopAllActions();
    btnDropPenet->stopAllActions();
	runTimeWaiting(data.WinId, 40);
	showWinnerCards();
	if (data.WinId == sfsIdMe) {
		tableCrest->setVisible(true);
	} else {
		btnBack->setLocalZOrder(constant::ZORDER_POPUP);
	}
	playSoundAction(data.SoundId);
}

void GameScene::onEndMatchMoney(EndMatchMoneyData data)
{
	winMoneyData = data;
}

void GameScene::onEndMatchTie(std::vector<unsigned char> stiltCards)
{
	if (state == NONE || state == READY) return;
	state = ENDING;
	btnXemNoc->setVisible(false);
	btnDongNoc->setVisible(false);
	progressTimer->setVisible(false);
	btnBash->setVisible(false);
	btnPick->setVisible(false);
	btnHold->setVisible(false);
	btnForward->setVisible(false);
	btnWin->setVisible(false);
	btnDropWin->setVisible(false);
	btnPenet->setVisible(false);
    btnDropPenet->setVisible(false);
	lbCardNoc->getParent()->getChildByName("spcardnoc")->setVisible(false);
	lbCardNoc->setString("");
	btnWin->stopAllActions();
	btnDropWin->stopAllActions();
	btnPenet->stopAllActions();
    btnDropPenet->stopAllActions();
	progressTimer->stopAllActions();

	if (stiltCards.size() == 1) {
		Vec2 pos = lbCardNoc->getParent()->getPosition() + Vec2(0, 20);
		Sprite* sp = getCardSprite(stiltCards[0]);
		sp->setScale(.8f);
		sp->setRotation(0);
		sp->setAnchorPoint(Vec2(.5f, .5f));
		sp->setPosition(pos.x / scaleScene.y, pos.y / scaleScene.x);
	}

	/*DelayTime* delay = DelayTime::create(3);
	CallFunc* func = CallFunc::create([=]() {
		lbCardNoc->getParent()->setVisible(false);
		lbCardNoc->getParent()->getChildByName("spcardnoc")->setVisible(true);
		gameSplash->setVisible(true);
		for (Sprite* sp : spCards) {
			sp->setVisible(false);
		}
		tableEndMatch->setVisible(true);
		lbWinner->setString("");
		lbDiem->setString(Utils::getSingleton().getStringForKey("tie"));
		lbCuocSai->setString("");
		lbNoted->setString("");
		lbNotedGa->setString("");
	});
	runAction(Sequence::create(delay, func, nullptr));*/
	showSystemNotice(Utils::getSingleton().getStringForKey("van_nay_hoa_roi"));
}

void GameScene::onPunishResponse(long UiD, std::string msg)
{
	spBatBaos[userIndexs[UiD]]->setVisible(true);
	if (UiD != sfsIdMe) return;
	showError(msg);

	isBatBao = true;
	btnBash->setVisible(false);
	btnBashBack->setVisible(false);
	btnHold->setVisible(false);
	btnPick->setVisible(false);
	btnForward->setVisible(false);
}

void GameScene::onUserReadyResponse(long UiD, bool isReady)
{
	spSanSangs[userIndexs2[UiD]]->setVisible(isReady);
	if (UiD == sfsIdMe) {
		btnReady->setVisible(!isReady);
		btnCancelReady->setVisible(isReady);
	}
}

void GameScene::onTableResponse(GameTableData data)
{
	timeTurn = data.Time;
	isU411 = data.IsU411;
	string roomName = Utils::getSingleton().currentRoomName;
	string tableId = roomName.substr(roomName.find_last_of("b") + 1, roomName.length()).c_str();

	Label* lbName = (Label*)tableInfo->getChildByName("lbname");
	Label* lbBet = (Label*)tableInfo->getChildByName("lbbet");
	Label* lbType = (Label*)tableInfo->getChildByName("lbtype");
	Sprite* icMoney = (Sprite*)tableInfo->getChildByName("icmoney");
	lbName->setString(Utils::getSingleton().getStringForKey("table") + " " + tableId);
	lbBet->setString(Utils::getSingleton().formatMoneyWithComma(data.Money));
	lbType->setString(Utils::getSingleton().getStringForKey(data.IsU411 ? "win_411" : "win_free") + ", " + to_string((int)timeTurn) + "s");
	icMoney->initWithFile(Utils::getSingleton().moneyType == 1 ? "main/icon_gold.png" : "main/icon_silver.png");

	vector<ui::CheckBox*> cbs;
	for (int i = 0; i < 3; i++) {
		cbs.push_back((ui::CheckBox*)popupSettings->getChildByTag(i));
	}

	cbs[0]->setSelected(data.IsU411);
	cbs[1]->setSelected(data.IsQuick);
	cbs[2]->setSelected(!data.IsQuick);
}

void GameScene::onLobbyUserResponse(std::vector<UserData> listUser)
{
	/*int numb = rand() % 20;
	listUser.clear();
	for (int i = 0; i < numb; i++) {
		UserData data;
		data.DisplayName = "Stormus" + to_string(rand() % 1000);
		data.MoneyFree = rand() % 10000;
		data.MoneyReal = rand() % 10000;
		listUser.push_back(data);
	}*/

	if (hasClickInvite) {
		hasClickInvite = false;
		showPopup(tableInvite);
		ui::ScrollView* scroll = (ui::ScrollView*)tableInvite->getChildByName("scroll");
		int sx = tableInvite->getChildByName("inside")->getContentSize().width;

		int y = listUser.size() * 60;
		if (y < scroll->getContentSize().height)
			y = scroll->getContentSize().height - 25;
		scroll->setInnerContainerSize(Size(sx, y + 25));

		bool isRealMoney = Utils::getSingleton().moneyType == 1;
		int childCount = scroll->getChildrenCount();
		for (int i = 0; i < listUser.size(); i++) {
			int posY = y - i * 60;
			int tag = i * 4;
			Label* lb1;
			Label* lb2;
			Sprite* sp;
			ui::Button* btn;
			bool isNewBtn;
			if (tag >= childCount) {
				lb1 = Label::create();
				lb1->setSystemFontSize(25);
				lb1->setAnchorPoint(Vec2(0, .5f));
				lb1->setWidth(sx - 350);
				lb1->setHeight(30);
				lb1->setTag(tag);
				scroll->addChild(lb1);

				sp = Sprite::create();
				sp->setScale(.6f);
				sp->setTag(tag + 1);
				scroll->addChild(sp);

				lb2 = Label::create();
				lb2->setSystemFontSize(25);
				lb2->setAnchorPoint(Vec2(0, .5f));
				lb2->setTag(tag + 2);
				scroll->addChild(lb2);

				btn = ui::Button::create("board/btn_moi.png", "board/btn_moi_clicked.png");
				btn->setScale(.8f);
				btn->setTag(tag + 3);
				scroll->addChild(btn);
				isNewBtn = true;
			} else {
				lb1 = (Label*)scroll->getChildByTag(tag);
				sp = (Sprite*)scroll->getChildByTag(tag + 1);
				lb2 = (Label*)scroll->getChildByTag(tag + 2);
				btn = (ui::Button*)scroll->getChildByTag(tag + 3);

				lb1->setVisible(true);
				lb2->setVisible(true);
				sp->setVisible(true);
				btn->setVisible(true);
				isNewBtn = false;
			}

			lb1->setPosition(0, posY);
			sp->setPosition(sx - 320, posY);
			lb2->setPosition(sx - 300, posY);
			btn->setPosition(Vec2(sx - 60, posY));

			lb1->setString(listUser[i].DisplayName.length() > 0 ? listUser[i].DisplayName : listUser[i].Name);
			lb2->setString(Utils::getSingleton().formatMoneyWithComma(isRealMoney ? listUser[i].MoneyReal : listUser[i].MoneyFree));
			sp->initWithFile(isRealMoney ? "main/icon_gold.png" : "main/icon_silver.png");
			addTouchEventListener(btn, [=]() {
				btn->setVisible(false);
				SFSRequest::getSingleton().RequestGameInvitePlayer(listUser[i].SfsUserId);
			}, isNewBtn);
		}

		int count = listUser.size() * 4;
		for (int i = count; i < childCount; i++) {
			scroll->getChildByTag(i)->setVisible(false);
		}
	} else {

	}
}

void GameScene::onGamePlayingDataResponse(PlayingTableData data)
{
	state = PLAY;
	vecPlayers = data.Players;
	myServerSlot = -1;
	for (PlayerData player : data.Players) {
		if (player.Info.UserID == Utils::getSingleton().userDataMe.UserID) {
			myServerSlot = player.Index;
			sfsIdMe = player.Info.SfsUserId;
		}
	}
	if (myServerSlot < 0) {
		tableCardPos[0] = tableCardPos[8];
	} else {
		tableCardPos[0] = tableCardPos[9];
	}
	for (int i = 0; i < 4; i++) {
		runningCards.push_back(NULL);
	}
	int num = 0;
	for (int i = 0; i < 4; i++) {
		int index = -1;
		for (PlayerData player : data.Players) {
			if (player.Index == (myServerSlot + i) % vecUsers.size()) {
				index = i - num;
				if (data.Players.size() == 2 && (index == 1 || index == 3)) {
					index = 2;
				}
				userIndexs[player.Info.SfsUserId] = index;
				userIndexs2[player.Info.UserID] = index;
				spInvites[index]->setVisible(false);
				vecUsers[index]->setVisible(true);
				vecUsers[index]->setAlpha(255);
				vecUsers[index]->setPlayerName(player.Info.DisplayName);
				vecUsers[index]->setPlayerMoney(player.PMoney);
				vecUsers[index]->setName(player.Info.Name);
				if (player.Index == 0) {
					spChuPhong->setVisible(true);
					spChuPhong->setPosition(vecUserPos[index] + Vec2(50 * scaleScene.y, 0));
				}

				int index2 = index * 2;
				float scale = 1;

				//Test
				/*for (int i = 0; i < 5; i++) {
					player.SingleCards.push_back(0);
				}*/
				//endtest

				if (player.SingleCards.size() > maxTableCardNumb[index2]) {
					scale = ((float)maxTableCardNumb[index2]) / player.SingleCards.size();
				}
				for (int j = 0; j < player.SingleCards.size(); j++) {
					Sprite* spCard = getCardSprite(player.SingleCards[j]);
					spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2] + 1);
					spCard->setTag(constant::TAG_CARD_TABLE + index2);
					spCard->setName(to_string((int)std::abs(player.SingleCards[j])));
					spCard->setAnchorPoint(Vec2(.5f, .5f));
					spCard->setPosition(tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale);
					spCard->setColor(player.SingleCards[j] > 0 ? Color3B::WHITE : Color3B(200, 200, 255));
					spCard->setRotation(0);
					spCard->setScale(cardScaleTable);

					tableCardNumb[index2] ++;
					if (j == player.SingleCards.size() - 1) {
						runningCards[index] = spCard;
					}
				}

				index2 = index * 2 + 1;
				scale = 1;

				//Test
				/*for (int i = 0; i < 5; i++) {
					vector<char> vec;
					vec.push_back(0);
					vec.push_back(0);
					player.PairCards.push_back(vec);
				}*/
				//end test

				if (player.PairCards.size() > maxTableCardNumb[index2]) {
					scale = ((float)maxTableCardNumb[index2] - 1) / (player.PairCards.size() - 1);
				}
				for (vector<char> v : player.PairCards) {
					Vec2 pos;
					if (v.size() == 2) {
						pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale;
					} else {
						pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale + Vec2(0, 5);
					}
					for (int i = 0; i < v.size(); i++) {
						Sprite* spCard = getCardSprite(v[i]);
						spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + (tableCardNumb[index2] + 1) * 4 + i);
						spCard->setTag(constant::TAG_CARD_TABLE + index2);
						spCard->setName(to_string((int)std::abs(v[i])));
						spCard->setAnchorPoint(Vec2(.5f, .5f));
						spCard->setPosition(pos - (v.size() == 2 ? Vec2(0, 40) : Vec2(0, 15)) * i);
						spCard->setColor(v[i] > 0 ? Color3B::WHITE : Color3B(200, 200, 255));
						spCard->setRotation(0);
						spCard->setScale(cardScaleTable);
					}
					tableCardNumb[index2] ++;
				}
			}
		}
		if (index == -1) {
			num++;
		}
	}
	lbCardNoc->getParent()->setVisible(true);
	lbCardNoc->setString(to_string((int)data.StiltCount));

	DelayTime* delay = DelayTime::create(.5f);
	CallFunc* func = CallFunc::create([=]() {
		runTimeWaiting(data.TurnId, timeTurn);
	});
	this->runAction(Sequence::createWithTwoActions(delay, func));
}

void GameScene::onGameSpectatorDataResponse(std::vector<PlayerData> spectators)
{
	this->vecSpectators = spectators;
	for (UserNode* n : vecUsers) {
		if (n->isVisible() && n->getAlpha() < 255) {
			n->setVisible(false);
		}
	}
	if (myServerSlot < 0) {
		for (PlayerData player : spectators) {
			if (player.Info.UserID == Utils::getSingleton().userDataMe.UserID) {
				sfsIdMe = player.Info.SfsUserId;
			}
		}
	}
	for (PlayerData player : spectators) {
		int index = player.Index;
		for (int i = 0; i < 4; i++) {
			if (!vecUsers[i]->isVisible()) {
				index = i;
				break;
			}
		}
		if (vecUsers[index]->isVisible()) {
			return;
		}
		userIndexs[player.Info.SfsUserId] = index;
		userIndexs2[player.Info.UserID] = index;
		spInvites[index]->setVisible(false);
		vecUsers[index]->setVisible(true);
		vecUsers[index]->setAlpha(150);
		vecUsers[index]->setPlayerName(player.Info.DisplayName);
		vecUsers[index]->setPlayerMoney(player.PMoney);
		vecUsers[index]->setName(player.Info.Name);
	}
}

void GameScene::onGameMyReconnectDataResponse(GameReconnectData data)
{
	if (isReconnecting) return;
	this->myCardHand = data.CardHand;
	if (data.PlayerState == 0) {
		lbCardNoc->getParent()->setVisible(false);
		for (Node* n : vecStilts) {
			n->setVisible(true);
		}
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < dealPos.size(); j++) {
				Sprite* sp = Sprite::create("cards/100.png");
				sp->setScale(.6f);
				sp->setRotation(rand() % 60 - 30);
				sp->setName(to_string(j));
				vecStilts[j]->addChild(sp);
				spDealCards.push_back(sp);
			}
		}
		state = CHOOSE_STILT;
		/*if (startGameData.LastWinner == sfsIdMe) {
			lbNoticeAction->setVisible(true);
			lbNoticeAction->setString(Utils::getSingleton().getStringForKey("choose_stilt"));
		}
		spChonCai->setVisible(true);
		spChonCai->setPosition(vecUserPos[userIndexs[startGameData.LastWinner]]);

		runTimeWaiting(startGameData.LastWinner, timeChooseHost);*/
	} else {
		lbCardNoc->getParent()->setVisible(true);
		showMyCards(false);
	}
}

void GameScene::onGameUserReconnectDataResponse(std::vector<UserReconnectData> list)
{
	for (int i = 0; i < list.size(); i++) {
		for (PlayerData player : vecPlayers) {
			if (list[i].Name.compare(player.Info.Name) == 0) {
				userIndexs[list[i].SfsUserId] = userIndexs[player.Info.SfsUserId];
				player.Info.SfsUserId = list[i].SfsUserId;
			}
		}
	}
}

void GameScene::onLobbyListTableResponse(LobbyListTable data)
{
	if (mustGoToLobby) {
		Utils::getSingleton().goToLobbyScene();
	}
}

void GameScene::onKeyHome()
{
}

void GameScene::initChatTable()
{
	tableChat = Node::create();
	tableChat->setPosition(560, 200);
	tableChat->setVisible(false);
	mLayer->addChild(tableChat, constant::ZORDER_POPUP);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("white.png");
	bg->setContentSize(Size(1500, 400));
	bg->setColor(Color3B::BLACK);
	//bg->setOpacity(150);
	tableChat->addChild(bg);

	ui::EditBox* input = ui::EditBox::create(Size(750, 65), "board/box_input.png");
	input->setPosition(Vec2(-140, 150));
	input->setMaxLength(30);
	input->setFontName("Arial");
	input->setFontSize(25);
	input->setFontColor(Color3B::WHITE);
	input->setPlaceholderFont("Arial", 30);
	input->setPlaceholderFontColor(Color3B(150, 150, 150));
	input->setPlaceHolder(Utils::getSingleton().getStringForKey("type_here_to_chat").c_str());
	input->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	input->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	input->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	input->setDelegate(this);
	input->setName("tfinput");
	tableChat->addChild(input);

	ui::Button* btnSend = ui::Button::create("board/btn.png", "board/btn_clicked.png");
	btnSend->setPosition(Vec2(350, 150));
	btnSend->setTitleText(Utils::getSingleton().getStringForKey("send"));
	btnSend->setTitleFontSize(30);
	addTouchEventListener(btnSend, [=]() {
		if (string(input->getText()).length() == 0) return;
		SFSRequest::getSingleton().SendPublicMessage(string(input->getText()));
		input->setText("");
		hidePopup(tableChat);
	});
	tableChat->addChild(btnSend);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(470, 150));
	btnClose->setScale(.6f);
	addTouchEventListener(btnClose, [=]() {
		input->setText("");
		hidePopup(tableChat);
	});
	tableChat->addChild(btnClose);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-560, -200));
	scroll->setContentSize(Size(1120, 300));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	tableChat->addChild(scroll);

	ValueMap plist = FileUtils::getInstance()->getValueMapFromFile("lang/chat.xml");
	int i = 0;
	int height = (plist.size() / 2 + 1) * 80 + 20;
	scroll->setInnerContainerSize(Size(1120, height));
	for (auto iter = plist.begin(); iter != plist.end(); iter++) {
		ui::Button* btn = ui::Button::create("board/box_input.png");
		btn->setScale9Enabled(true);
		btn->setContentSize(Size(500, 64));
		btn->setTitleText(iter->second.asString());
		btn->setTitleFontSize(20);
		btn->setPosition(Vec2(290 + (i % 2) * 540, height - 50 - (i / 2) * 80));
		btn->setName(iter->first);
		addTouchEventListener(btn, [=]() {
			SFSRequest::getSingleton().SendPublicMessage(btn->getName());
			hidePopup(tableChat);
		});
		scroll->addChild(btn);
		i++;
	}
}

void GameScene::initCrestTable()
{
	vector<unsigned char> ids = { 33, 0, 3, 15, 11, 4, 5, 2, 6, 8, 28, 1, 37, 10, 9, 19, 20, 7, 24, 32, 35, 38, 34, 39, 36 };

	tableCrest = Node::create();
	tableCrest->setPosition(560, 490);
	tableCrest->setVisible(false);
	mLayer->addChild(tableCrest, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(tableCrest);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::create("popup/bg.png");
	bg->setContentSize(Size(1130, 430));
	bg->setInsetLeft(100);
	bg->setInsetRight(100);
	bg->setInsetTop(0);
	bg->setInsetBottom(0);
	tableCrest->addChild(bg);

	Sprite* light = Sprite::create("game/light.png");
	light->setScale(20, 4);
	light->setPosition(0, 115);
	tableCrest->addChild(light);

	lbChonCuoc = Label::create("", "fonts/guanine.ttf", 25);
	lbChonCuoc->setPosition(0, 120);
	lbChonCuoc->setColor(Color3B(50, 50, 50));
	lbChonCuoc->setAlignment(TextHAlignment::CENTER);
	lbChonCuoc->setWidth(900);
	lbChonCuoc->setHeight(40);
	tableCrest->addChild(lbChonCuoc);

	Size size1 = Size(1000, 225);
	Size size2 = Size(1000, ((ids.size() - 2) / 4 + 2) * 46);
	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setScrollBarColor(Color3B::WHITE);
	scroll->setScrollBarOpacity(100);
	scroll->setScrollBarPositionFromCorner(Vec2(5, 10));
	scroll->setScrollBarAutoHideEnabled(false);
	scroll->setBounceEnabled(true);
	scroll->setContentSize(size1);
	scroll->setName("scroll");
	tableCrest->addChild(scroll);
	scroll->setInnerContainerSize(size2);
	scroll->setPosition(Vec2(-size1.width / 2, -size1.height / 2 - 30));

	int x = 130;
	int y = size2.height - 10;
	for (int i = 0; i < ids.size(); i++) {
		string name = String::createWithFormat("cuoc_%d", ids[i])->getCString();
		/*Label* lb = Label::createWithTTF(Utils::getSingleton().getStringForKey(name), "fonts/guanine.ttf", 25);
		lb->setPosition(x + (i % 4) * 250, y - (i/4) * 45);
		lb->setColor(Color3B::WHITE);
		lb->setTag(ids[i]);
		scroll->addChild(lb);
		vecCrests.push_back(lb);

		if (i == ids.size() - 1) {
			lb->setPosition(x + 80, y - (i / 4) * 45);
		}*/

		ui::Button* btn = ui::Button::create("", "");
		btn->setTitleText(Utils::getSingleton().getStringForKey(name));
		btn->setTitleFontName("fonts/guanine.ttf");
		btn->setTitleColor(Color3B::WHITE);
		btn->setTitleFontSize(23);
		btn->setPosition(Vec2(x + (i % 4) * 250, y - (i / 4) * 46));
		btn->setContentSize(Size(240, 40));
		btn->setScale9Enabled(true);
		btn->setBright(false);
		btn->setTag(ids[i]);
		scroll->addChild(btn);
		vecCrests.push_back(btn);

		if (i == ids.size() - 1) {
			btn->setContentSize(Size(430, 40));
			btn->setPosition(Vec2(x + 75, y - (i / 4) * 46));
		}

		addTouchEventListener(btn, [=]() {
			int i = btn->getTag();
			if (i == 33) {
				if (chosenCuocs.size() == 1 && chosenCuocs[0] == 33) {
					chosenCuocs.clear();
					chosenCuocNumbs.clear();
					vecCrests[0]->setTitleColor(Color3B::WHITE);
					lbChonCuoc->setString("");
					return true;
				}

				for (int j = 1; j < vecCrests.size(); j++) {
					vecCrests[j]->setTitleColor(Color3B::WHITE);
				}
				vecCrests[0]->setTitleColor(Color3B::YELLOW);
				chosenCuocs.clear();
				chosenCuocNumbs.clear();
				chosenCuocs.push_back(33);
				chosenCuocNumbs.push_back(0);
				lbChonCuoc->setString(Utils::getSingleton().getStringForKey("cuoc_33"));
			} else {
				if (chosenCuocs.size() == 1 && chosenCuocs[0] == 33) {
					chosenCuocs.clear();
					chosenCuocNumbs.clear();
					vecCrests[0]->setTitleColor(Color3B::WHITE);
				}

				int index = 0;
				while (index < chosenCuocs.size() && chosenCuocs[index] != i) index++;
				if (index == chosenCuocs.size()) {
					chosenCuocs.push_back(i);
					chosenCuocNumbs.push_back(0);
					btn->setTitleColor(Color3B::YELLOW);
				} else {
					chosenCuocNumbs[index] ++;
					if (chosenCuocNumbs[index] > maxChosenCuocs[i]) {
						chosenCuocs.erase(chosenCuocs.begin() + index);
						chosenCuocNumbs.erase(chosenCuocNumbs.begin() + index);
						btn->setTitleColor(Color3B::WHITE);
					}
				}

				string strcuoc = "";
				for (int j = 0; j < chosenCuocs.size(); j++) {
					if (strcuoc.length() > 0) {
						strcuoc += ", ";
					}
					strcuoc += Utils::getSingleton().getStringForKey("cuoc_" + to_string(chosenCuocs[j] + chosenCuocNumbs[j]));
				}
				lbChonCuoc->setString(strcuoc);
			}
		});
	}

	ui::Button* btnCrest = ui::Button::create("board/btn_xuong.png", "board/btn_xuong_clicked.png");
	btnCrest->setPosition(Vec2(370, -210));
	addTouchEventListener(btnCrest, [=]() {
		vector<unsigned char> crestIds;
		for (int i = 0; i < chosenCuocs.size();i++) {
			crestIds.push_back(chosenCuocs[i] + chosenCuocNumbs[i]);
		}
		lbChonCuoc->setString("");
		tableCrest->setVisible(false);
		lbCrestTime->setVisible(false);
		lbCrestTime->pauseSchedulerAndActions();
		SFSRequest::getSingleton().RequestGameWinCrest(crestIds);
		for (ui::Button* btn : vecCrests) {
			btn->setTitleColor(Color3B::WHITE);
		}
	});
	tableCrest->addChild(btnCrest);
}

void GameScene::initEndMatchTable()
{
	tableEndMatch = Node::create();
	tableEndMatch->setPosition(560, 500);
	tableEndMatch->setVisible(false);
	mLayer->addChild(tableEndMatch, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(tableEndMatch);

	Sprite* bg = Sprite::create("popup/bg.png");
	bg->setScaleY(.85f);
	bg->setFlipY(true);
	tableEndMatch->addChild(bg);

	lbWinner = Label::createWithTTF("", "fonts/guanine.ttf", 25);
	lbWinner->setPosition(0, 80);
	lbWinner->setColor(Color3B(50, 50, 50));
	lbWinner->setAlignment(TextHAlignment::CENTER);
	lbWinner->setWidth(600);
	tableEndMatch->addChild(lbWinner);

	lbCuocSai = Label::createWithTTF("", "fonts/arialbd.ttf", 25);
	lbCuocSai->setPosition(-300, -40);
	lbCuocSai->setAnchorPoint(Vec2(0, .5f));
	lbCuocSai->setColor(Color3B(75, 0, 0));
	lbCuocSai->setWidth(600);
	lbCuocSai->setHeight(30);
	tableEndMatch->addChild(lbCuocSai);

	lbDiem = Label::createWithTTF("", "fonts/arialbd.ttf", 25);
	lbDiem->setPosition(0, 0);
	lbDiem->setColor(Color3B::RED);
	tableEndMatch->addChild(lbDiem);

	lbNoted = Label::createWithTTF("", "fonts/arial.ttf", 20);
	lbNoted->setVerticalAlignment(TextVAlignment::CENTER);
	lbNoted->setPosition(-300, -75);
	lbNoted->setColor(Color3B(75, 0, 0));
	lbNoted->setAnchorPoint(Vec2(0, .5f));
	lbNoted->setWidth(600);
	lbNoted->setHeight(50);
	tableEndMatch->addChild(lbNoted);

	lbNotedGa = Label::createWithTTF("", "fonts/arial.ttf", 20);
	lbNotedGa->setPosition(-235, -112);
	lbNotedGa->setColor(Color3B(75, 0, 0));
	lbNotedGa->setAnchorPoint(Vec2(0, .5f));
	lbNotedGa->setWidth(600);
	lbNotedGa->setHeight(25);
	tableEndMatch->addChild(lbNotedGa);
}

void GameScene::initInviteTable()
{
	tableInvite = Node::create();
	tableInvite->setPosition(560, 350);
	tableInvite->setVisible(false);
	mLayer->addChild(tableInvite, constant::ZORDER_POPUP + 1);
	autoScaleNode(tableInvite);

	Sprite* bg = Sprite::create("popup/bg.png");
	bg->setScale(1.1f, 1.5f);
	tableInvite->addChild(bg);

	Sprite* title = Sprite::create("popup/title_moichoi.png");
	title->setPosition(0, 280);
	//title->setScale(.8f);
	tableInvite->addChild(title);

	int sx = 650;
	ui::Scale9Sprite* inside = ui::Scale9Sprite::create("popup/box.png");
	inside->setContentSize(Size(sx, 480));
	inside->setOpacity(200);
	inside->setPosition(0, -5);
	inside->setName("inside");
	tableInvite->addChild(inside);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-(sx - 20)/2, -240));
	scroll->setContentSize(Size(sx - 20, 470));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	tableInvite->addChild(scroll);

	ui::Button* btnDong = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnDong->setPosition(Vec2(300, 280));
	btnDong->setScale(.8f);
	addTouchEventListener(btnDong, [=]() {
		hidePopup(tableInvite);
	});
	tableInvite->addChild(btnDong);
}

void GameScene::initSettingsPopup()
{
	popupSettings = Node::create();
	popupSettings->setPosition(560, 350);
	popupSettings->setVisible(false);
	mLayer->addChild(popupSettings, constant::ZORDER_POPUP);
	autoScaleNode(popupSettings);

	Sprite* bg = Sprite::create("popup/bg.png");
	popupSettings->addChild(bg);

	Sprite* title = Sprite::create("popup/title_caidat.png");
	title->setPosition(0, 180);
	title->setScale(.8f);
	popupSettings->addChild(title);

	vector<Vec2> vecPos;
	vecPos.push_back(Vec2(-200, 100));
	vecPos.push_back(Vec2(-200, 25));
	vecPos.push_back(Vec2(100, 25));
	vecPos.push_back(Vec2(-200, -50));
	vecPos.push_back(Vec2(-200, -125));

	vector<Label*> lbs;
	vector<ui::CheckBox*> cbs;
	for (int i = 0; i < 5; i++) {
		ui::CheckBox* checkbox = ui::CheckBox::create();
		checkbox->loadTextureBackGround("popup/box.png");
		checkbox->loadTextureFrontCross("popup/check.png");
		checkbox->setPosition(vecPos[i]);
		checkbox->setSelected(false);
		checkbox->setTag(i);
		popupSettings->addChild(checkbox);
		cbs.push_back(checkbox);

		Label* lb = Label::create();
		lb->setSystemFontName("Arial");
		lb->setSystemFontSize(30);
		lb->setPosition(vecPos[i] + Vec2(40, 0));
		lb->setAnchorPoint(Vec2(0, .5f));
		lb->setColor(Color3B::BLACK);
		popupSettings->addChild(lb);
		lbs.push_back(lb);
	}

	cbs[1]->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		if (type == ui::CheckBox::EventType::SELECTED) {
			cbs[2]->setSelected(false);
		} else if (type == ui::CheckBox::EventType::UNSELECTED) {
			cbs[2]->setSelected(true);
		}
	});
	cbs[2]->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		if (type == ui::CheckBox::EventType::SELECTED) {
			cbs[1]->setSelected(false);
		}else if(type == ui::CheckBox::EventType::UNSELECTED){
			cbs[1]->setSelected(true);
		}
	});

	cbs[3]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_AUTO_READY.c_str()));
	cbs[4]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_SOUND.c_str()));

	lbs[0]->setString(Utils::getSingleton().getStringForKey("win") + " 4-11");
	lbs[1]->setString(Utils::getSingleton().getStringForKey("quick"));
	lbs[2]->setString(Utils::getSingleton().getStringForKey("slow"));
	lbs[3]->setString(Utils::getSingleton().getStringForKey("auto_ready"));
	lbs[4]->setString(Utils::getSingleton().getStringForKey("sound"));

	ui::Button* btnOK = ui::Button::create("popup/btn_submit.png", "popup/btn_submit_clicked.png");
	btnOK->setPosition(Vec2(0, -190));
	addTouchEventListener(btnOK, [=]() {
		hidePopup(popupSettings);
		UserDefault::getInstance()->setBoolForKey(constant::KEY_AUTO_READY.c_str(), cbs[3]->isSelected());
		UserDefault::getInstance()->setBoolForKey(constant::KEY_SOUND.c_str(), cbs[4]->isSelected());
		if (myServerSlot == 0 && (state == NONE || state == READY)){
			SFSRequest::getSingleton().RequestGameTableInfo(cbs[1]->isSelected(), cbs[0]->isSelected());
		}
		Utils::getSingleton().SoundEnabled = cbs[4]->isSelected();
	});
	popupSettings->addChild(btnOK);

	ui::Button* btnClose = ui::Button::create("popup/btn_dong.png", "popup/btn_dong_clicked.png");
	btnClose->setPosition(Vec2(290, 170));
	btnClose->setScale(.7f);
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupSettings);
	});
	popupSettings->addChild(btnClose);

	ui::Button* btnNo = ui::Button::create("white.png", "white.png");
	btnNo->setContentSize(Size(500, 150));
	btnNo->setPosition(Vec2(0, 60));
	btnNo->setScale9Enabled(true);
	btnNo->setOpacity(0);
	btnNo->setName("btnno");
	popupSettings->addChild(btnNo);
}

void GameScene::initTableInfo()
{
	tableInfo = Node::create();
	tableInfo->setPosition(Vec2(0, 700) + getScaleSceneDistance(Vec2(195, -50)));
	mLayer->addChild(tableInfo, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(tableInfo);

	Sprite* bg = Sprite::create("board/bg_table_info.png");
	bg->setScaleX(.85f);
	tableInfo->addChild(bg);

	Sprite* icMoney = Sprite::create("main/icon_gold.png");
	icMoney->setPosition(-5, 15);
	icMoney->setScale(.5f);
	icMoney->setName("icmoney");
	tableInfo->addChild(icMoney);

	Label* lbTableName = Label::create("", "fonts/arialbd.ttf", 18);
	lbTableName->setPosition(-90, 15);
	lbTableName->setAnchorPoint(Vec2(0, .5f));
	lbTableName->setColor(Color3B(255, 200, 0));
	lbTableName->setName("lbname");
	tableInfo->addChild(lbTableName);

	Label* lbTableBet = Label::create("", "fonts/arialbd.ttf", 18);
	lbTableBet->setPosition(8, 15);
	lbTableBet->setAnchorPoint(Vec2(0, .5f));
	lbTableBet->setColor(Color3B(255, 200, 0));
	lbTableBet->setName("lbbet");
	tableInfo->addChild(lbTableBet);

	Label* lbType = Label::create("", "fonts/arialbd.ttf", 18);
	lbType->setPosition(-90, -15);
	lbType->setAnchorPoint(Vec2(0, .5f));
	lbType->setColor(Color3B(255, 200, 0));
	lbType->setName("lbtype");
	tableInfo->addChild(lbType);
}

Sprite* GameScene::getCardSprite(int id)
{
	if (id < 0) id = -id;
	int cardName = getCardName(id);
	for (Sprite* sp : spCards) {
		if (sp && !sp->isVisible()) {
			sp->initWithFile(String::createWithFormat("cards/%d.png", cardName)->getCString());
			sp->setAnchorPoint(Vec2(.5f, .5f));
			sp->setColor(Color3B::WHITE);
			sp->setVisible(true);
			sp->setOpacity(255);
			sp->setScale(1);
			return sp;
		}
	}
	Sprite* sp = Sprite::create(String::createWithFormat("cards/%d.png", cardName)->getCString());
	playLayer->addChild(sp);
	spCards.push_back(sp);
	return sp;
}

cocos2d::Vec2 GameScene::getScaleScenePosition(Vec2 pos)
{
	return cocos2d::Vec2(pos.x / scaleScene.y, pos.y / scaleScene.x);
}

cocos2d::Vec2 GameScene::getScaleSceneDistance(cocos2d::Vec2 pos)
{
	return cocos2d::Vec2(pos.x * scaleScene.y, pos.y * scaleScene.x);
}

void GameScene::getCardSpriteToHand(int id, int group, int zorder)
{
	Sprite* sp = getCardSprite(id);
	sp->setTag(constant::TAG_CARD_HAND);
	sp->setName(to_string(group + id));
	sp->setScale(1.2f);
	sp->setAnchorPoint(Vec2(.5f, -.2f));
	sp->setPosition(vecUsers[0]->getPosition());
	sp->setLocalZOrder(constant::GAME_ZORDER_HAND_CARD + zorder);
	spHandCards.push_back(sp);
}
