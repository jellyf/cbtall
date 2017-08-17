﻿#pragma once
#include "GameScene.h"
#include "Constant.h"
#include "Data.h"
#include "SFSRequest.h"
#include "UserNode.h"
#include "Utils.h"
#include "EventHandler.h"
#include "AudioEngine.h"
#include "GameLogger.h"
#include "SFSGEvent.h"

using namespace std;
using namespace cocos2d;

void GameScene::onInit()
{
	setTag(constant::SCENE_GAME);
	BaseScene::onInit();

	bool pmE = Utils::getSingleton().ispmE();
	isSolo = Utils::getSingleton().isSoloGame();
	isTourGame = Utils::getSingleton().isTourGame();

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
	for (int i = 0; i < 40; i++) {
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
	
	string zone = Utils::getSingleton().getCurrentZoneName();
	int index = zone.find_last_of("Q");
	if (index >= 0 && index < zone.length()) {
		zone = zone.substr(0, index);
	}
	if (zone.length() == 0) {
		zone = "VuongPhu";
	}
	if (isTourGame) zone = "VuongPhu";

	string bgName = isSolo ? "bgVuongPhu.jpg" : "bg" + zone + ".jpg";
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGB565);
	Texture2D* bgTexture = TextureCache::getInstance()->addImage(bgName);
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);

	Sprite* bg = Sprite::createWithTexture(bgTexture);
	bg->setPosition(560, 350);
	mLayer->addChild(bg);

	//Sprite* centerBg = Sprite::create("center.png");
	Sprite* centerBg = Sprite::createWithSpriteFrameName("center.png");
	centerBg->setPosition(560, 350);
	//centerBg->setVisible(!isSolo);
	mLayer->addChild(centerBg);
	autoScaleNode(centerBg);

	if (isSolo) {
		Sprite* dragon1 = Sprite::create("dragon.png");
		dragon1->setPosition(310, 350);
		mLayer->addChild(dragon1);
		autoScaleNode(dragon1);

		Sprite* dragon2 = Sprite::create("dragon.png");
		dragon2->setPosition(810, 350);
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
	Vec2 topRight = Vec2(winSize.width, winSize.height);

	btnSettings = ui::Button::create("btn_settings.png", "btn_settings_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnSettings->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	btnSettings->setTouchEnabled(false);
	addTouchEventListener(btnSettings, [=]() {
		//if (gameSplash->isVisible()) return;
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
	mLayer->addChild(btnSettings, constant::ZORDER_POPUP - 2);
	autoScaleNode(btnSettings);
	vecMenuBtns.push_back(btnSettings);
	
	btnChat = ui::Button::create("btn_chat.png", "btn_chat_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnChat->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));// Vec2(975, 650));
	btnChat->setTouchEnabled(false);
	addTouchEventListener(btnChat, [=]() {
		//if (gameSplash->isVisible()) return;
		showPopup(tableChat);
	});
	mLayer->addChild(btnChat, constant::ZORDER_POPUP - 3);
	autoScaleNode(btnChat);
	vecMenuBtns.push_back(btnChat);

	btnBack = ui::Button::create("btn_back.png", "btn_back_clicked.png", "", ui::Widget::TextureResType::PLIST);
	//btnBack = ui::Button::create("btn_exit.png", "btn_exit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	//btnBack->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	btnBack->setPosition(topLeft + getScaleSceneDistance(Vec2(50, -50)));
	//btnBack->setTouchEnabled(false);
	addTouchEventListener(btnBack, [=]() {
		onBackScene();
	});
	mLayer->addChild(btnBack, constant::ZORDER_POPUP - 4);
	autoScaleNode(btnBack);
	//vecMenuBtns.push_back(btnBack);

	btnBag = ui::Button::create("btn_bag.png", "btn_bag_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnBag->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	//btnBag->setTouchEnabled(false);
	btnBag->setVisible(false);
	addTouchEventListener(btnBag, [=]() {

	});
	mLayer->addChild(btnBag, constant::ZORDER_POPUP - 5);
	autoScaleNode(btnBag);
	//vecMenuBtns.push_back(btnBag);

	btnUp = ui::Button::create("btn_up.png", "btn_up_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnUp->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	btnUp->setVisible(false);
	addTouchEventListener(btnUp, [=]() {
		hideMenuButtons();
	});
	mLayer->addChild(btnUp, constant::ZORDER_POPUP - 1);
	autoScaleNode(btnUp);

	btnDown = ui::Button::create("btn_down.png", "btn_down_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnDown->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	addTouchEventListener(btnDown, [=]() {
		showMenuButtons();
	});
	mLayer->addChild(btnDown, constant::ZORDER_POPUP - 1);
	autoScaleNode(btnDown);

	initCofferView(topRight + getScaleSceneDistance(Vec2(-150, -50)), constant::GAME_ZORDER_BUTTON, .8f);

	iconGa = Sprite::createWithSpriteFrameName("btn_ga_off.png");
	//iconGa->setPosition(topLeft + getScaleSceneDistance(Vec2(260, -50)));// 880, 650);
	//iconGa->setPosition(topLeft + getScaleSceneDistance(Vec2(340, -50)));
	iconGa->setPosition(topRight + getScaleSceneDistance(Vec2(Utils::getSingleton().ispmE() ? -240 : -150, -50)));
	iconGa->setVisible(!isTourGame);
	mLayer->addChild(iconGa, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(iconGa);

	lbMoneyGa = Label::create("", "fonts/arial.ttf", 20);
	lbMoneyGa->setPosition(iconGa->getPosition() - getScaleSceneDistance(Vec2(0, 40)));
	mLayer->addChild(lbMoneyGa, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(lbMoneyGa);

	//spNetwork->setPosition(topRight + getScaleSceneDistance(Vec2(pmE ? -240 : -150, -50)));
	spNetwork->setPosition(Vec2(winSize.width, 0) + getScaleSceneDistance(Vec2(-40, 35)));

	btnReady = ui::Button::create("btn_ready.png", "btn_ready_clicked.png", "", ui::Widget::TextureResType::PLIST);
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

	btnCancelReady = ui::Button::create("btn_unready.png", "btn_unready_clicked.png", "", ui::Widget::TextureResType::PLIST);
	//btnCancelReady->setContentSize(Size(200, 65));
	btnCancelReady->setPosition(Vec2(560, 350));
	//btnCancelReady->setScale9Enabled(true);
	btnCancelReady->setVisible(false);
	addTouchEventListener(btnCancelReady, [=]() {
		if (state != NONE && state != READY) return;
		SFSRequest::getSingleton().RequestGameReady();
		btnReady->setVisible(true);
		btnCancelReady->setVisible(false);
	});
	mLayer->addChild(btnCancelReady);
	autoScaleNode(btnCancelReady);

	btnBash = ui::Button::create("btn_danh.png", "btn_danh_clicked.png", "", ui::Widget::TextureResType::PLIST);
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

		//autoBash(card, group);
		btnBash->setVisible(false);
	});
	mLayer->addChild(btnBash, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnBash);

	btnForward = ui::Button::create("btn_duoi.png", "btn_duoi_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnForward->setPosition(Vec2(900, 35));
	btnForward->setVisible(false);
	addTouchEventListener(btnForward, [=]() {
		SFSRequest::getSingleton().RequestGameForward();
		noaction = 0;
	});
	mLayer->addChild(btnForward, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnForward);

	btnBashBack = ui::Button::create("btn_danh.png", "btn_danh_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnBashBack->setPosition(Vec2(900, 35));
	btnBashBack->setVisible(false);
	addTouchEventListener(btnBashBack, [=]() {
		if (chosenCard < 0 || chosenCard >= spHandCards.size()) return;
		//btnBashBack->setVisible(false);
		int numb = atoi(spHandCards[chosenCard]->getName().c_str());
		int card = numb % 1000;
		int group = numb / 1000;
		SFSRequest::getSingleton().RequestGameBashBack(card, group);

		progressTimer->setVisible(false);
		progressTimer->stopAllActions();
		noaction = 0;

		//autoBash(card, group);
		btnBashBack->setVisible(false);
	});
	mLayer->addChild(btnBashBack, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnBashBack);

	btnHold = ui::Button::create("btn_an.png", "btn_an_clicked.png", "", ui::Widget::TextureResType::PLIST);
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

	btnPick = ui::Button::create("btn_boc.png", "btn_boc_clicked.png", "", ui::Widget::TextureResType::PLIST);
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

	btnPenet = ui::Button::create("btn_chiu.png", "btn_chiu_clicked.png", "", ui::Widget::TextureResType::PLIST);
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
    
    btnDropPenet = ui::Button::create("btn_bo_chiu.png", "btn_bo_chiu_clicked.png", "", ui::Widget::TextureResType::PLIST);
    btnDropPenet->setPosition(Vec2(220, 35));
    btnDropPenet->setVisible(false);
    addTouchEventListener(btnDropPenet, [=]() {
        dropPenet();
    });
    mLayer->addChild(btnDropPenet, constant::GAME_ZORDER_BUTTON);
    autoScaleNode(btnDropPenet);

	btnWin = ui::Button::create("btn_u.png", "", "", ui::Widget::TextureResType::PLIST);
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

	btnDropWin = ui::Button::create("btn_bou.png", "btn_bou_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnDropWin->setPosition(Vec2(220, 35));
	btnDropWin->setVisible(false);
	addTouchEventListener(btnDropWin, [=]() {
		dropWin();
	});
	mLayer->addChild(btnDropWin, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnDropWin);

	btnXemNoc = ui::Button::create("btn_xem_noc.png", "btn_xem_noc_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnXemNoc->setPosition(Vec2(560, 280));
	btnXemNoc->setVisible(false);
	addTouchEventListener(btnXemNoc, [=]() {
		showStiltCards();
	});
	mLayer->addChild(btnXemNoc, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(btnXemNoc);

	btnDongNoc = ui::Button::create("btn_dong_noc.png", "btn_dong_noc_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnDongNoc->setPosition(Vec2(560, 280));
	btnDongNoc->setVisible(false);
	addTouchEventListener(btnDongNoc, [=]() {
		showWinnerCards();
	});
	mLayer->addChild(btnDongNoc, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(btnDongNoc);

	/*Label* lbBtnReady = Label::createWithTTF(Utils::getSingleton().getStringForKey("ready"), "fonts/UTM AZUKI.ttf", 30);
	lbBtnReady->setPosition(btnReady->getContentSize().width/2, btnReady->getContentSize().height/2);
	lbBtnReady->setColor(Color3B::YELLOW);
	lbBtnReady->enableOutline(Color4B::BLACK, 2);
	btnReady->addChild(lbBtnReady);

	Label* lbBtnCancelReady = Label::createWithTTF(Utils::getSingleton().getStringForKey("cancel_ready"), "fonts/UTM AZUKI.ttf", 30);
	lbBtnCancelReady->setPosition(btnCancelReady->getContentSize().width / 2, btnCancelReady->getContentSize().height / 2);
	lbBtnCancelReady->setColor(Color3B::YELLOW);
	lbBtnCancelReady->enableOutline(Color4B::BLACK, 2);
	btnCancelReady->addChild(lbBtnCancelReady);*/

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

	Sprite* bgDiaNoc = Sprite::createWithSpriteFrameName("bg_dianoc.png");
	//ui::Button* bgDiaNoc = ui::Button::create("bg_dianoc.png", "", "", ui::Widget::TextureResType::PLIST);
	bgDiaNoc->setPosition(Vec2(564, 422));
	mLayer->addChild(bgDiaNoc);
	bgDiaNoc->setVisible(false);
	autoScaleNode(bgDiaNoc);
	/*addTouchEventListener(bgDiaNoc, [=]() {
		if (btnPick->isVisible()) {
			SFSRequest::getSingleton().RequestGamePick();
			progressTimer->setVisible(false);
			progressTimer->stopAllActions();
		}
	});*/

	Sprite* spCardNoc = Sprite::createWithSpriteFrameName("bg_card_up.png");
	spCardNoc->setRotation(-20);
	spCardNoc->setPosition(bgDiaNoc->getContentSize().width / 2 + 25, bgDiaNoc->getContentSize().height / 2 + 20);
	spCardNoc->setName("spcardnoc");
	bgDiaNoc->addChild(spCardNoc, 1);

	ui::Button* spNapNoc = ui::Button::create("bg_napnoc.png", "bg_napnoc_hit.png", "", ui::Widget::TextureResType::PLIST);
	//Sprite* spNapNoc = Sprite::createWithSpriteFrameName("bg_napnoc.png");
	spNapNoc->setPosition(Vec2(bgDiaNoc->getContentSize().width / 2 - 6, bgDiaNoc->getContentSize().height / 2 + 8));
	bgDiaNoc->addChild(spNapNoc, 2);
	addTouchEventListener(spNapNoc, [=]() {
		if (btnPick->isVisible()) {
			SFSRequest::getSingleton().RequestGamePick();
			progressTimer->setVisible(false);
			progressTimer->stopAllActions();
		}
	});

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
		Sprite* spInvite = Sprite::createWithSpriteFrameName("btn_invite.png");
		spInvite->setPosition(vecUserPos[i]);
		mLayer->addChild(spInvite);
		spInvites.push_back(spInvite);
		autoScaleNode(spInvite);

		UserNode* user = UserNode::create();
		user->setPosition(getScaleScenePosition(vecUserPos[i]));
		user->setVisible(false);
		playLayer->addChild(user, constant::GAME_ZORDER_USER);
		vecUsers.push_back(user);

		Sprite* spSS = Sprite::createWithSpriteFrameName("txt_sansang.png");
		spSS->setPosition(vecUserPos[i]);
		spSS->setVisible(false);
		mLayer->addChild(spSS, constant::GAME_ZORDER_USER + 9);
		spSanSangs.push_back(spSS);
		autoScaleNode(spSS);

		Sprite* spBB = Sprite::createWithSpriteFrameName("txt_bat_bao.png");
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

		Sprite* spPointBg = Sprite::createWithSpriteFrameName("circle_red.png");
		spPointBg->setPosition(vecUserPos[i] + Vec2(40, -35));
		spPointBg->setVisible(false);
		spPointBg->setScale(.9f);
		mLayer->addChild(spPointBg, constant::GAME_ZORDER_USER + 12);
		autoScaleNode(spPointBg);

		Label *lbPoint = Label::createWithTTF("100", "fonts/aurora.ttf", 25);
		lbPoint->setPosition(spPointBg->getContentSize().width/2, spPointBg->getContentSize().height/2 + 2);
		spPointBg->addChild(lbPoint);
		lbTourPoints.push_back(lbPoint);

		ui::Button* btnKick = ui::Button::create("empty.png", "empty.png", "", ui::Widget::TextureResType::PLIST);
		mLayer->addChild(btnKick, constant::GAME_ZORDER_USER + 12);
		btnKick->setPosition(vecUserPos[i] + Vec2(45, 40));
		btnKick->setContentSize(Size(70, 70));
		btnKick->setScale9Enabled(true);
		btnKick->setVisible(false);
		vecBtnKicks.push_back(btnKick);
		autoScaleNode(btnKick);

		Sprite* spKick = Sprite::createWithSpriteFrameName("btn_dong.png");
		spKick->setPosition(btnKick->getContentSize().width / 2, btnKick->getContentSize().height / 2);
		btnKick->addChild(spKick);
		spKick->setScale(.45f);

		addTouchEventListener(btnKick, [=](){
			showPopupNotice(Utils::getSingleton().getStringForKey("ban_muon_da_nguoi_nay_ra"), [=]() {
				SFSRequest::getSingleton().RequestGameKickPlayer(user->getSfsId());
			});
		});
	}

	progressTimer = ProgressTimer::create(Sprite::createWithSpriteFrameName("time_line.png"));
	progressTimer->setType(ProgressTimer::Type::RADIAL);
	progressTimer->setPercentage(100);
	progressTimer->setReverseProgress(true);
	progressTimer->setVisible(false);
	progressTimer->setColor(Color3B::GREEN);
	playLayer->addChild(progressTimer, constant::GAME_ZORDER_USER + 9);

	spChuPhong = Sprite::createWithSpriteFrameName("chuphong.png");
	spChuPhong->setVisible(!isTourGame);
	mLayer->addChild(spChuPhong, constant::GAME_ZORDER_USER + 10);
	autoScaleNode(spChuPhong);

	spChonCai = Sprite::createWithSpriteFrameName("txt_choncai.png");
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
	initCofferEffects();
	//initEventView(Vec2(0, 680), Size(Director::sharedDirector()->getVisibleSize().width, 40));

	Node* nodeError = Node::create();
	nodeError->setPosition(560, 350);
	nodeError->setVisible(false);
	mLayer->addChild(nodeError, constant::ZORDER_POPUP + 10);
	autoScaleNode(nodeError);

	Sprite* bgError = Sprite::createWithSpriteFrameName("popup_bg.png");
	nodeError->addChild(bgError);

	lbError = Label::createWithTTF("", "fonts/arialbd.ttf", 30);
	lbError->setAlignment(TextHAlignment::CENTER);
	lbError->setColor(Color3B::YELLOW);
	lbError->setWidth(600);
	nodeError->addChild(lbError);

	gameSplash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
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

	//showWaiting();
}

void GameScene::registerEventListenner()
{
    EventHandler::getSingleton().onApplicationDidEnterBackground = std::bind(&GameScene::onApplicationDidEnterBackground, this);
    EventHandler::getSingleton().onApplicationWillEnterForeground = std::bind(&GameScene::onApplicationWillEnterForeground, this);
	EventHandler::getSingleton().onPingPong = std::bind(&GameScene::onPingPong, this, std::placeholders::_1);
	EventHandler::getSingleton().onConnected = std::bind(&GameScene::onConnected, this);
	EventHandler::getSingleton().onConnectionFailed = std::bind(&GameScene::onConnectionFailed, this);
	EventHandler::getSingleton().onConnectionLost = std::bind(&GameScene::onConnectionLost, this, std::placeholders::_1);
	EventHandler::getSingleton().onLoginZone = bind(&GameScene::onLoginZone, this);
	EventHandler::getSingleton().onLoginZoneError = bind(&GameScene::onLoginZoneError, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onJoinRoom = bind(&GameScene::onJoinRoom, this, placeholders::_1, placeholders::_2);
	EventHandler::getSingleton().onJoinRoomError = bind(&GameScene::onJoinRoomError, this, placeholders::_1);
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
	EventHandler::getSingleton().onCofferMoneySFSResponse = bind(&GameScene::onCofferMoneyResponse, this, placeholders::_1);
	EventHandler::getSingleton().onCofferHistorySFSResponse = bind(&GameScene::onCofferHistoryResponse, this, placeholders::_1);
	EventHandler::getSingleton().onTableReconnectDataSFSResponse = bind(&GameScene::onTableReconnectDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onTourRoomMatchSFSResponse = bind(&GameScene::onTourRoomMatch, this, placeholders::_1);
	EventHandler::getSingleton().onTourWinnersSFSResponse = bind(&GameScene::onTourWinnersResponse, this, placeholders::_1);
	EventHandler::getSingleton().onTourRoomToJoinSFSResponse = bind(&GameScene::onTourRoomToJoin, this, placeholders::_1);
	EventHandler::getSingleton().onTourTimeWaitPlayerSFSResponse = bind(&GameScene::onTourTimeWaitPlayer, this, placeholders::_1);

	SFSRequest::getSingleton().onHttpResponseFailed = std::bind(&GameScene::onHttpResponseFailed, this);
	SFSRequest::getSingleton().onHttpResponse = std::bind(&GameScene::onHttpResponse, this, std::placeholders::_1, std::placeholders::_2);
}

void GameScene::unregisterEventListenner()
{
	BaseScene::unregisterEventListenner();
	EventHandler::getSingleton().onConnected = NULL;
	EventHandler::getSingleton().onConnectionFailed = NULL;
	EventHandler::getSingleton().onConnectionLost = NULL;
	EventHandler::getSingleton().onJoinRoom = NULL;
	EventHandler::getSingleton().onJoinRoomError = NULL;
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
	EventHandler::getSingleton().onCofferMoneySFSResponse = NULL;
	EventHandler::getSingleton().onCofferHistorySFSResponse = NULL;
	EventHandler::getSingleton().onTableReconnectDataSFSResponse = NULL;
	EventHandler::getSingleton().onTourRoomMatchSFSResponse = NULL;
	EventHandler::getSingleton().onTourWinnersSFSResponse = NULL;
	EventHandler::getSingleton().onTourRoomToJoinSFSResponse = NULL;

	SFSRequest::getSingleton().onHttpResponse = NULL;
	SFSRequest::getSingleton().onHttpResponseFailed = NULL;
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

		if (!isTourGame && myServerSlot == 0 && (state == NONE || state == READY)
			&& iconGa->getBoundingBox().containsPoint(pos)) {
			SFSRequest::getSingleton().RequestGameGa();
			return true;
		}

		if (startGameData.LastWinner == sfsIdMe) {
			if (state == CHOOSE_STILT && chosenStilt == -1) {
				for (int i = 0; i < dealPos.size(); i++) {
					if (pos.distance(Vec2(560, 350) + dealPos[i]) < 100) {
						chosenStilt = i;
						state = CHOOSE_HOST;
						SFSRequest::getSingleton().RequestGameChooseStilt(i + 1);
						return true;
					}
				}
			} else if (state == CHOOSE_HOST && chosenStiltHost == -1) {
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
					/*if (vecUsers[i]->getName().compare(Utils::getSingleton().userDataMe.Name) == 0) {
						showPopupUserInfo(Utils::getSingleton().userDataMe, false);
					} else {*/
						SFSRequest::getSingleton().RequestUserInfo(vecUsers[i]->getName());
					//}
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
	// Do NOT call BaseScene::onApplicationDidEnterBackground();
    spNetwork->pause();
    lbNetwork->pause();
    pauseTimeInSecs = Utils::getSingleton().getCurrentSystemTimeInSecs();
	if (state != NONE && state != READY && myServerSlot >= 0) {
		string username = Utils::getSingleton().userDataMe.Name;
		double timeSecs = Utils::getSingleton().getCurrentSystemTimeInSecs();
		UserDefault::getInstance()->setDoubleForKey((constant::KEY_RECONNECT_TIME + username).c_str(), timeSecs + 300);
		UserDefault::getInstance()->setIntegerForKey((constant::KEY_RECONNECT_ZONE_INDEX + username).c_str(), Utils::getSingleton().getCurrentZoneIndex());
	}
}

void GameScene::onApplicationWillEnterForeground()
{
    // Do NOT call BaseScene::onApplicationWillEnterForeground();
    spNetwork->resume();
    lbNetwork->resume();
    double curTime = Utils::getSingleton().getCurrentSystemTimeInSecs();
    float pauseTime = curTime - pauseTimeInSecs;
    if(pauseTime > 120){
        float timeWait = pauseTime / 40;
		if (timeWait > 20) timeWait = 20;
        showWaiting(timeWait + 10);
        SFSGEvent::getSingleton().DoWork(false);
        this->delayFunction(this, timeWait, [=](){
            SFSGEvent::getSingleton().DoWork(true);
            this->disconnectToSync();
        });
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
				Sprite* sp = Sprite::createWithSpriteFrameName("100.png");
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
	for (Sprite* sp : spHandCards) {
		sp->setVisible(false);
		sp->stopAllActions();
	}
	spHandCards.clear();

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
	processWaitAction();
}

void GameScene::dropPenet()
{
    SFSRequest::getSingleton().RequestGameDropPenet();
    btnPenet->stopAllActions();
    btnDropPenet->stopAllActions();
    btnPenet->setVisible(false);
    btnDropPenet->setVisible(false);
    noaction = 0;
	processWaitAction();
}

void GameScene::disconnectToSync()
{
	btnBash->setVisible(false);
	btnBashBack->setVisible(false);
	btnPick->setVisible(false);
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

	isSynchronizing = true;
	showWaiting();
    if(SFSRequest::getSingleton().IsConnected()){
        SFSRequest::getSingleton().Disconnect();
    }else{
        onConnectionLost("unknown");
    }
}

void GameScene::processWaitAction()
{
	if (waitAction == constant::GAME_ACTION_BASH) {
		btnPick->setVisible(true);
		btnHold->setVisible(true);
		waitAction = constant::GAME_ACTION_NONE;
	} else if (waitAction == constant::GAME_ACTION_PICK) {
		btnForward->setVisible(true);
		btnHold->setVisible(true);
		waitAction = constant::GAME_ACTION_NONE;
	} else if (waitAction == constant::GAME_ACTION_FORWARD) {
		btnForward->setVisible(true);
		btnHold->setVisible(true);
		waitAction = constant::GAME_ACTION_NONE;
	}
}

void GameScene::syncHandCard(CardHandData cardHand)
{
	myCardHand = cardHand;
	showMyCards(false);
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
		spHandCards[i]->setScale(1.2f);
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

	DelayTime* delayCheck = DelayTime::create(1);
	CallFunc* funcCheck = CallFunc::create([=]() {
		int rot = 11;
		int startRot = -(rot * spHandCards.size() / 2) + rot / 2;
		bool ok = true;
		for (int i = 0; i < spHandCards.size(); i++) {
			if (spHandCards[i]->getRotation() != startRot + i * rot) {
				ok = false;
				break;
			}
		}
		if (!ok) syncHandCard(data);
	});
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
		sp = Sprite::createWithSpriteFrameName(String::createWithFormat("%d.png", getCardName(c))->getCString());
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
			Sprite* sp = Sprite::createWithSpriteFrameName(String::createWithFormat("%d.png", getCardName(c))->getCString());
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
			Sprite* sp = Sprite::createWithSpriteFrameName(String::createWithFormat("%d.png", getCardName(endMatchData.ListCaU[i]))->getCString());
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
			Sprite* sp = Sprite::createWithSpriteFrameName(String::createWithFormat("%d.png", getCardName(endMatchData.ListCardU[i]))->getCString());
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

	Node* btnNo = popupSettings->getChildByName("btnno");
	Node* btnNoAutoReady = popupSettings->getChildByName("btnnoautoready");
	btnNo->setVisible(isTourGame || !(myServerSlot == 0 && (state == NONE || state == READY)));
	btnNoAutoReady->setVisible(isTourGame);
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
	showToast("system", msg, Vec2(560, 400), Color3B::BLACK, Color3B(200, 200, 200));
}

void GameScene::showCofferEffects(std::string money)
{
	if (!Utils::getSingleton().ispmE()) return;
	Label* lb = (Label*)cofferEffect->getChildByName("lbmoney");
	Sprite* spLight = (Sprite*)cofferEffect->getChildByName("splight");
	lb->setString(money);
	spLight->resumeSchedulerAndActions();

	Vec2 scale1 = getScaleSmoothly(1.1f);
	Vec2 scale2 = getScaleSmoothly(1);
	Vec2 scale3 = getScaleSmoothly(0);
	ScaleTo* scaleTo1 = ScaleTo::create(.2f, scale1.x, scale1.y);
	ScaleTo* scaleTo2 = ScaleTo::create(.1f, scale2.x, scale2.y);
	/*ScaleTo* scaleTo3 = ScaleTo::create(.3f, scale3.x, scale3.y);
	DelayTime* delay = DelayTime::create(3);
	CallFunc* func = CallFunc::create([=]() {
		cofferSplash->setVisible(false);
		cofferEffect->setVisible(false);
		cofferEffect->getChildByName("splight")->pauseSchedulerAndActions();
	});*/
	cofferEffect->setScale(0);
	cofferEffect->setVisible(true);
	//cofferEffect->runAction(Sequence::create(scaleTo1, scaleTo2, delay, scaleTo3, func, nullptr));
	cofferEffect->runAction(Sequence::createWithTwoActions(scaleTo1, scaleTo2));
	cofferSplash->setVisible(true);
}

void GameScene::showMenuButtons()
{
	if (vecMenuBtns[vecMenuBtns.size() - 1]->getNumberOfRunningActions() > 0) return;
	btnUp->setVisible(true);
	btnDown->setVisible(false);
	Vec2 pos = Vec2(winSize.width, winSize.height) + getScaleSceneDistance(Vec2(-50, -50));
	int i = 1;
	for (ui::Button* n : vecMenuBtns) {
		n->setTouchEnabled(true);
		MoveTo* move = MoveTo::create(.2f, pos + getScaleSceneDistance(Vec2(0, -90 * i++)));
		n->runAction(move);
	}
}

void GameScene::hideMenuButtons()
{
	if (vecMenuBtns[vecMenuBtns.size() - 1]->getNumberOfRunningActions() > 0) return;
	btnUp->setVisible(false);
	btnDown->setVisible(true);
	Vec2 pos = Vec2(winSize.width, winSize.height) + getScaleSceneDistance(Vec2(-50, -50));
	int i = 1;
	for (ui::Button* n : vecMenuBtns) {
		n->setTouchEnabled(false);
		MoveTo* move = MoveTo::create(.2f, pos);
		n->runAction(move);
	}
}

void GameScene::updateTourMatch()
{
	playerMe.TMatch++;
	TourInfo tour = Utils::getSingleton().tourInfo;
	int tourMatch = tour.MaxMatch * tour.RequiredMatch / 100;
	Label* lbMatch = (Label*)tableInfo->getChildByName("lbmatch");
	lbMatch->setString(Utils::getSingleton().getStringForKey("van") + ": " + to_string(playerMe.TMatch) + "/ " + to_string(tourMatch));
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

bool GameScene::isCardHandDataSync(CardHandData cardHand)
{
	vector<int> a = getCardCount(cardHand);
	vector<int> b = getCardCount(myCardHand);
	for (int i = 0; i < a.size(); i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

int GameScene::getCardName(unsigned char cardId)
{
	if (cardId > 100) {
		cardId = 256 - cardId;
	}
	if (cardId == 0) return 0;
	return (cardId / 3 + 1) * 10 + cardId % 3 + 1;
}

int GameScene::getNextPlayerIndexFrom(int index)
{
	int ret = (index + 1) % 4;
	if (vecUsers[ret]->isVisible() && vecUsers[ret]->getAlpha() == 255) {
		return ret;
	}
	return (ret + 1) % 4;
}

std::vector<int>& GameScene::getCardCount(CardHandData cardHand)
{
	vector<int> a;
	for (int i = 0; i < 30; i++) a.push_back(0);

	for (int i = 0; i < cardHand.ThienKhai.size(); i++) {
		for (int j = 0; j < 4; j++) {
			a[cardHand.ThienKhai[i]] ++;
		}
	}
	for (int i = 0; i < cardHand.Chan.size(); i++) {
		for (int j = 0; j < 2; j++) {
			a[cardHand.Chan[i]] ++;
		}
	}
	for (int i = 0; i < cardHand.Ca.size(); i++) {
		int id1 = (cardHand.Ca[i] - 1) * 3 + (cardHand.Ca[i + 1] - 1);
		int id2 = (cardHand.Ca[i] - 1) * 3 + (cardHand.Ca[i + 2] - 1);
		a[id1] ++;
		a[id2] ++;
		i += 2;
	}
	for (int i = 0; i < cardHand.BaDau.size(); i++) {
		for (int j = 0; j < 3; j++) {
			int id = (cardHand.BaDau[i] - 1) * 3 + j;
			a[id] ++;
		}
	}
	for (int i = 0; i < cardHand.Que.size(); i++) {
		a[cardHand.Que[i]] ++;
	}
	return a;
}

void GameScene::onConnectionFailed()
{
	BaseScene::onConnectionFailed();
	experimental::AudioEngine::stopAll();
}

bool GameScene::onConnectionLost(std::string reason)
{
	if(BaseScene::onConnectionLost(reason)) return true;
	experimental::AudioEngine::stopAll();
	mustGoToLobby = myServerSlot < 0 || state == NONE || state == READY;
	/*btnUp->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
	btnDown->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
	int i = 1;
	for (Node* n : vecMenuBtns) {
		n->setLocalZOrder(constant::GAME_ZORDER_BUTTON - i++);
	}*/
	if (isSynchronizing) {
		isSynchronizing = false;
        SFSGEvent::getSingleton().DoWork(true);
		handleClientDisconnectionReason(constant::DISCONNECTION_REASON_SYNC);
	} else {
		handleClientDisconnectionReason(reason);
	}
	return true;
}

void GameScene::onJoinRoom(long roomId, std::string roomName)
{
	if (roomName.at(0) == 'g' && roomName.at(2) == 'b') {
		if (isJoiningTour) {
			Utils::getSingleton().goToGameScene();
		} else {
			reset();
		}
	} else {
		if (isPrepareToTour && Utils::getSingleton().currentZoneName.compare("AutoTourKTC") != 0) {
			Utils::getSingleton().goToLobbyScene();
		}
	}
}

void GameScene::onUserDataResponse(UserData data)
{
	showPopupUserInfo(data, false);
}

void GameScene::onUserExitRoom(long sfsUId)
{
	if (sfsUId == sfsIdMe) {
		/*btnUp->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
		btnDown->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
		int i = 1;
		for (Node* n : vecMenuBtns) {
			n->setLocalZOrder(constant::GAME_ZORDER_BUTTON - i++);
		}*/
		if (isKickForNotReady) {
			/*showPopupNotice(Utils::getSingleton().getStringForKey("bi_thoat_do_khong_san_sang"), [=]() {
				SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
				Utils::getSingleton().goToLobbyScene();
			}, false);*/
		} else if (isReconnecting) {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
		} else if(isMatchTimeEnd){
			//DO nothing
		} else {
			//showPopupNotice(Utils::getSingleton().getStringForKey("bi_day_khoi_ban"), [=]() {
				SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentRoomName);
				//Utils::getSingleton().goToLobbyScene();
			//}, false);
		}
		experimental::AudioEngine::stopAll();
		return;
	}
	int index = userIndexs[sfsUId];
	if (vecUsers[index]->isVisible() && vecUsers[index]->getAlpha() == 150) {
		vecUsers[index]->setName("");
		vecUsers[index]->setVisible(false);
		spInvites[index]->setVisible(state == NONE || state == READY);
		spSanSangs[index]->setVisible(false);
		vecBtnKicks[index]->setVisible(false);
		lbTourPoints[index]->getParent()->setVisible(false);
	}
	if (progressTimer->getTag() == index) {
		progressTimer->stopAllActions();
		progressTimer->setVisible(false);
	}
}

bool GameScene::onErrorResponse(unsigned char code, std::string msg)
{
	//error =	4: co nguoi doi u
	//			3: Bạn chưa tới lượt bốc
	//			4: Bạn chưa tới lượt dưới
	//			2: Bạn chưa tới lượt ăn
	//			20: Hết ván đấu Tour
	//			36: Bị loại khỏi Tour || vào vòng trong
	//			39: Giải thưởng tour
	//			40: Đăng ký Tour thành công
	//			80: Tour kết thúc
	if(BaseScene::onErrorResponse(code, msg)) return true;
	if (code == 31 || code == 30 || code == 29) {
		showSystemNotice(msg);
		return true;
	}
	if (code == 19) {
		isKickForNotReady = true;
		showPopupNotice(msg, [=]() {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
		}, false);
		return true;
	}
	if (isTourGame && (code == 80 || code == 39)) {
		state = NONE;
		isMatchTimeEnd = true;
		//Utils::getSingleton().cachedErrors.push_back(pair<unsigned char, string>(code, msg));
		showPopupNotice(msg, [=]() {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
		}, false);
		return true;
	}
	if (isTourGame && (code == 36 || code == 35)) {
		state = NONE;
		//Utils::getSingleton().cachedErrors.push_back(pair<unsigned char, string>(code, msg));
		showPopupNotice(msg, [=]() {});
		return true;
	}
	if (code == 42) {
		isKickForNotEnoughMoney = true;
		unregisterEventListenner();
		showPopupNotice(msg, [=]() {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
		}, false);
		return true;
	}
	if (code == 52) {
		showCofferEffects(msg);
		return true;
	}
	if (code == 40) {
		showPopupNotice(msg, [=]() {});
		return true;
	}
	showError(msg);
	return true;
}

void GameScene::joinIntoTour()
{
	if (isPrepareToTour || state == NONE || state == READY || myServerSlot < 0) {
		BaseScene::joinIntoTour();
	} else {
		isPrepareToTour = true;
		showSystemNotice(Utils::getSingleton().getStringForKey("ban_se_vao_giai_dau_sau_khi_het_van"));
	}
}

void GameScene::onPublicMessage(long uid, std::string msg)
{
	//CCLOG("onPublicMessage: %d %s", uid, msg.c_str());
	if (msg.length() > 10) {
		std::string code_data = msg.substr(0, 10);
		if (code_data.compare("codedata::") == 0) {

			return;
		}
	}
	int index = userIndexs[uid];
	if (index < 0 || index > 3) return;
	showToast(to_string(uid), msg, vecUserPos[index] + Vec2(0, 30));
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
	} else if (isPrepareToTour) {
		joinIntoTour();
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
			playIdMe = player.Info.UserID;
			playerMe = player;
		}
	}
	if (myServerSlot == -1) {
		tableCardPos[0] = tableCardPos[8];
	} else {
		tableCardPos[0] = tableCardPos[9];
	}
	for (int i = 0; i < 4; i++) {
		vecUsers[i]->setVisible(false);
		spBatBaos[i]->setVisible(false);
		spSanSangs[i]->setVisible(false);
		vecBtnKicks[i]->setVisible(false);
		lbTourPoints[i]->getParent()->setVisible(false);
	}
	spInvites[0]->setVisible(true);
	spInvites[2]->setVisible(true);
	spInvites[1]->setVisible(!isSolo);
	spInvites[3]->setVisible(!isSolo);
	int num = 0;
	bool isMeHost = false;
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
				vecUsers[index]->setSfsId(player.Info.SfsUserId);
				spSanSangs[index]->setVisible(player.Ready);
				lbTourPoints[index]->setString(to_string(player.TPoint));
				lbTourPoints[index]->getParent()->setVisible(isTourGame);
				if (player.Index == 0) {
					spChuPhong->setPosition(vecUserPos[index] + Vec2(50 * scaleScene.y, 0));
					spChuPhong->setTag(player.Info.SfsUserId == sfsIdMe ? 1 : 0);
				}
				if (player.Info.SfsUserId == sfsIdMe) {
					if (isAutoReady && !player.Ready) {
						state = READY;
						btnCancelReady->setVisible(true);
						SFSRequest::getSingleton().RequestGameReady();
					} else if(!isTourGame){
						btnReady->setVisible(!player.Ready);
						btnCancelReady->setVisible(player.Ready);
					}
					isMeHost = player.Index == 0;
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
	} else if(roomData.Players.size() > 0){
		if (roomData.TimeStart > 0 && !lbCrestTime->isVisible()) {
			Vec2 lbscale = getScaleSmoothly(1.5f);
			lbCrestTime->setVisible(true);
			lbCrestTime->setScale(lbscale.x, lbscale.y);
			lbCrestTime->setColor(Color3B::RED);
			lbCrestTime->setString(to_string((int)timeStart));
			lbCrestTime->resumeSchedulerAndActions();
		}
		if (isMeHost && !isTourGame) {
			for (int i = 1; i < 4; i++) {
				vecBtnKicks[i]->setVisible(vecUsers[i]->isVisible() && Utils::getSingleton().dynamicConfig.Kick);
			}
		}
	}
	waitAction = constant::GAME_ACTION_NONE;
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
	btnHold->setVisible(false);
	btnForward->setVisible(false);
	tableCrest->setVisible(false);
	tableEndMatch->setVisible(false);
	lbCardNoc->getParent()->setVisible(false);
	lbCardNoc->getParent()->getChildByName("spcardnoc")->setVisible(true);
	gameSplash->setVisible(false);
	progressTimer->stopAllActions();
	progressTimer->setVisible(false);
	/*btnUp->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
	btnDown->setLocalZOrder(constant::GAME_ZORDER_BUTTON);
	int i = 1;
	for (Node* n : vecMenuBtns) {
		n->setLocalZOrder(constant::GAME_ZORDER_BUTTON - i++);
	}*/
	runningSpCard = NULL;
	spHandCards.clear();
	chosenCuocs.clear();
	chosenCuocNumbs.clear();
	endLayer->removeAllChildren();
	cofferSplash->setVisible(false);
	cofferEffect->setVisible(false);
	cofferEffect->getChildByName("splight")->pauseSchedulerAndActions();
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
			/*CallFunc* logFunc = CallFunc::create([=]() {
				Vec2 pos = lbWinMoneys[index]->getPosition();
				CCLOG("winmoney %d: %.0f %.0f", index, pos.x, pos.y);
			});*/

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
	iconGa->initWithSpriteFrameName(isGa ? "btn_ga_on.png" : "btn_ga_off.png");
	if (isGa) {
		lbMoneyGa->setString(Utils::getSingleton().formatMoneyWithComma(gaMoney));
	} else {
		lbMoneyGa->setString("");
	}
}

void GameScene::onStartGameDataResponse(StartGameData data)
{
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
	for (Sprite* sp : spInvites) {
		sp->setVisible(false);
	}
	for (int i = 1; i < 4; i++) {
		vecBtnKicks[i]->setVisible(false);
	}
	dealCards();
	noaction = 0;

	Node* lbTimeWaitPlayer = mLayer->getChildByName("lbtimewaitplayer");
	if (lbTimeWaitPlayer) {
		lbTimeWaitPlayer->pause();
		lbTimeWaitPlayer->setVisible(false);
		mLayer->getChildByName("lbTitleWaitPlayer")->setVisible(false);
	}
}

void GameScene::onChooseStilt(unsigned char stilt)
{
	//if (myServerSlot < 0) return;
	for (Node* n : vecStilts) {
		if (!n->isVisible()) return;
	}
	if (state == DEAL) {
		experimental::AudioEngine::stopAll();
		this->stopAllActions();
		if (spDealCards.size() > 0) {
			Node* n1 = spDealCards[0]->getParent();
			Node* n2 = spDealCards[1]->getParent();
			for (Sprite* sp : spDealCards) {
				sp->stopAllActions();
				int i = atoi(sp->getName().c_str());
				if (i < 0 || i >= vecStilts.size()) i = 0;
				sp->setRotation(rand() % 60 - 30);
				sp->setPosition(rand() % 20 - 10, rand() % 20 - 10);
				if (sp->getParent() != vecStilts[i]) {
					sp->setParent(nullptr);
					vecStilts[i]->addChild(sp);
				}
			}
			n1->removeAllChildren();
			n2->removeAllChildren();
		}
		state == CHOOSE_STILT;
	}

	chosenStilt = stilt - 1;
	if (chosenStilt < 0 || chosenStilt >= vecStilts.size()) {
		chosenStilt = 0;
	}
	vecStilts[chosenStilt]->setVisible(false);
	hostCard = Sprite::createWithSpriteFrameName("100.png");
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

	if (chosenStiltHost < 0 || chosenStiltHost >= vecStilts.size() || chosenStiltHost == chosenStilt) {
		while ((chosenStiltHost = rand() % vecStilts.size()) == chosenStilt);
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
		hostCard->initWithSpriteFrameName(String::createWithFormat("%d.png", card)->getCString());
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
		DelayTime* delay4 = DelayTime::create(6);
		CallFunc* func4 = CallFunc::create([=]() {
			int num = startGameData.CardHand.BaDau.size() * 3 + startGameData.CardHand.Ca.size() / 3 * 2 + startGameData.CardHand.Chan.size() * 2
				+ startGameData.CardHand.Que.size() + startGameData.CardHand.ThienKhai.size() * 4;
			if (startGameData.CurrentTurn == sfsIdMe || num == 20) {
				btnBash->setVisible(true);
			}
			runTimeWaiting(startGameData.CurrentTurn, timeTurn);
		});
		btnBash->runAction(Sequence::create(delay4, func4, nullptr));
	}
}

void GameScene::onUserBash(BashData data)
{
	bool isMe = data.UId == sfsIdMe;
	bool isToMe = data.TurnId == sfsIdMe;
	int index = userIndexs[data.UId];
	GameLogger::getSingleton().logUserBash(data);
	if (myServerSlot >= 0 && index == 0 && !isMe) {
		disconnectToSync();
		return;
	}
	btnBash->setVisible(false);
	if (isAutoBash) {
		isAutoBash = false;
		if (isMe) updateCardHand(data.CardHand);
	} else {
		bool rightData = bashCardDown(index, index, data.CardId, isMe);
		if (!rightData) {
			syncHandCard(data.CardHand);
		} else if (isMe) {
			updateCardHand(data.CardHand);
		}
	}

	if (!isBatBao) {
		if (data.CanPenetWin) {
			if (isU411) {
				noaction++;
				beatenNodeAndHide(btnWin, 1.1f, .9f, .7f, 10);
				btnDropWin->setVisible(true);
				delayFunction(btnDropWin, 10, [=]() {
					btnDropWin->setVisible(false);
					onUserBashToMe(data);
				});
				if (isToMe) {
					waitAction = constant::GAME_ACTION_BASH;
				}
			}
		} else if (data.CanPenet) {
			beatenNodeAndHide(btnPenet, 1.2f, 1, .7f, 7);
            btnDropPenet->setVisible(true);
            delayFunction(btnDropPenet, 7, [=]() {
                btnDropPenet->setVisible(false);
				onUserBashToMe(data);
            });
            if (isToMe) {
                waitAction = constant::GAME_ACTION_BASH;
            }
		} else onUserBashToMe(data);
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
	bool isMe = data.UId == sfsIdMe;
	bool isToMe = data.TurnId == sfsIdMe;
	GameLogger::getSingleton().logUserBashBack(data);
	int index = userIndexs[data.UId];
	int index1 = userIndexs[data.BackId];
	if (myServerSlot >= 0 && index == 0 && !isMe) {
		disconnectToSync();
		return;
	}
	btnBashBack->setVisible(false);
	if (isAutoBash) {
		isAutoBash = false;
		if (isMe) updateCardHand(data.CardHand);
	} else {
		bool rightData = bashCardDown(index, index1, data.CardId, isMe);
		if (!rightData) {
			syncHandCard(data.CardHand);
		} else if (isMe) {
			updateCardHand(data.CardHand);
		}
	}

	if (!isBatBao) {
		if (data.CanPenetWin) {
			if (isU411) {
				noaction++;
				beatenNodeAndHide(btnWin, 1.1f, .9f, .7f, 10);
				btnDropWin->setVisible(true);
				delayFunction(btnDropWin, 10, [=]() {
					btnDropWin->setVisible(false);
					onUserBashBackToMe(data);
				});
				if (isToMe) {
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
				onUserBashBackToMe(data);
            });
            if (isToMe) {
                if (data.IsPicked) {
                    waitAction = constant::GAME_ACTION_PICK;
                } else {
                    waitAction = constant::GAME_ACTION_BASH;
                }
            }
		} else onUserBashBackToMe(data);
	}
	runTimeWaiting(data.TurnId, timeTurn);
	playSoundAction(data.SoundId);
}

void GameScene::onUserHold(HoldData data)
{
	GameLogger::getSingleton().logUserHold(data);
	int index = userIndexs[data.UId];
	if (myServerSlot >= 0 && index == 0 && data.UId != sfsIdMe) {
		index = userIndexs[data.TurnId];
		if (index == 0 && data.TurnId != sfsIdMe) {
			disconnectToSync();
			return;
		}
	}
	if (runningSpCard == NULL && runningCards.size() > 0 && runningCards[index] != NULL) {
		int cid = atoi(runningCards[index]->getName().c_str());
		if (cid == data.CardIdHold || cid == 256 - data.CardIdHold) {
			runningSpCard = runningCards[index];
		}
	}
	if (runningSpCard == NULL) {
		int tmpIndex = (index + 3) % 4;
		if (!vecUsers[tmpIndex]->isVisible() || vecUsers[tmpIndex]->getAlpha() < 255) {
			tmpIndex = (tmpIndex + 3) % 4;
		}
		runningSpCard = runningCards[tmpIndex];
	}
	if (runningSpCard == NULL) {
		disconnectToSync();
		return;
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

	bool hasSync = false;
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
		if (spCard == NULL) {
			hasSync = true;
			syncHandCard(data.CardHand);
			spCard = getCardSprite(data.CardId);
		}
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
		if(!hasSync) updateCardHand(data.CardHand);
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
	GameLogger::getSingleton().logUserPick(data);
	int index = userIndexs[data.UId];
	if (myServerSlot >= 0 && index == 0 && data.UId != sfsIdMe) {
		disconnectToSync();
		return;
	}
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
					onUserPickToMe(data);
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
				onUserPickToMe(data);
            });
            if (data.TurnId == sfsIdMe) {
                waitAction = constant::GAME_ACTION_PICK;
            }
		} else onUserPickToMe(data);
	}
	runTimeWaiting(data.TurnId, timeTurn);
	playSoundAction(data.SoundId);

	for (Sprite* sp : spHandCards) {
		if (sp->getScale() != 1.2f) {
			sp->setScale(1.2f);
		}
	}
}

void GameScene::onUserPenet(PenetData data)
{
	GameLogger::getSingleton().logUserPenet(data);
	int index = userIndexs[data.UId];
	if (myServerSlot >= 0 && index == 0 && data.UId != sfsIdMe) {
		disconnectToSync();
		return;
	}
	if (runningSpCard == NULL) {
		for (Sprite* sp : runningCards) {
			if (sp != NULL) {
				int cid = atoi(sp->getName().c_str());
				if (cid == data.CardId || cid == 256 - data.CardId) {
					runningSpCard = sp;
					break;
				}
			}
		}
	}
	if (runningSpCard == NULL) {
		disconnectToSync();
		return;
	}
	vector<int> zorders;
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
		waitAction = constant::GAME_ACTION_NONE;
	} else {
		btnHold->setVisible(false);
		btnPick->setVisible(false);
		btnForward->setVisible(false);
	}
	playSoundAction(data.SoundId);
}

void GameScene::onUserForward(ForwardData data)
{
	GameLogger::getSingleton().logUserForward(data);
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
	/*btnUp->setLocalZOrder(constant::ZORDER_POPUP);
	btnDown->setLocalZOrder(constant::ZORDER_POPUP);
	int i = 1;
	for (Node* n : vecMenuBtns) {
		n->setLocalZOrder(constant::ZORDER_POPUP - i++);
	}*/

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

	Label* lbNameCoffer = (Label*)cofferEffect->getChildByName("lbname");
	if (data.UId == sfsIdMe) {
		lbNameCoffer->setString("");
	} else {
		lbNameCoffer->setString(vecUsers[userIndexs[data.UId]]->getPlayerName());
	}
}

void GameScene::onEndMatch(EndMatchData data)
{
	GameLogger::getSingleton().logEndMatch(data);
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
		/*btnUp->setLocalZOrder(constant::ZORDER_POPUP);
		btnDown->setLocalZOrder(constant::ZORDER_POPUP);
		int i = 1;
		for (Node* n : vecMenuBtns) {
			n->setLocalZOrder(constant::ZORDER_POPUP - i++);
		}*/
	}
	playSoundAction(data.SoundId);
}

void GameScene::onEndMatchMoney(EndMatchMoneyData data)
{
	winMoneyData = data;
}

void GameScene::onEndMatchTie(std::vector<unsigned char> stiltCards)
{
	GameLogger::getSingleton().logEndMatchTie();
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
	if (!isTourGame && UiD == playIdMe) {
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
	bool isQuan = Utils::getSingleton().moneyType == 1;
	GameLogger::getSingleton().setRoom(isQuan, data.Money, tableId);

	Label* lbName = (Label*)tableInfo->getChildByName("lbname");
	Label* lbBet = (Label*)tableInfo->getChildByName("lbbet");
	Label* lbType = (Label*)tableInfo->getChildByName("lbtype");
	Sprite* icMoney = (Sprite*)tableInfo->getChildByName("icmoney");
	lbName->setString(Utils::getSingleton().getStringForKey("table") + " " + tableId);
	lbBet->setString(Utils::getSingleton().formatMoneyWithComma(data.Money));
	lbType->setString(Utils::getSingleton().getStringForKey(data.IsU411 ? "win_411" : "win_free") + ", " + to_string((int)timeTurn) + "s");
	if (isQuan) {
		icMoney->initWithSpriteFrameName("icon_gold.png");
	} else {
		icMoney->initWithSpriteFrameName("icon_silver.png");
	}

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

				btn = ui::Button::create("btn_moi.png", "btn_moi_clicked.png", "", ui::Widget::TextureResType::PLIST);
				btn->setScale(.7f);
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
			sp->setPosition(sx - 360, posY);
			lb2->setPosition(sx - 340, posY);
			btn->setPosition(Vec2(sx - 70, posY));

			lb1->setString(listUser[i].DisplayName.length() > 0 ? listUser[i].DisplayName : listUser[i].Name);
			lb2->setString(Utils::getSingleton().formatMoneyWithComma(isRealMoney ? listUser[i].MoneyReal : listUser[i].MoneyFree));
			lb2->setColor(isRealMoney ? Color3B::YELLOW : Color3B(0, 255, 255));
			if (isRealMoney) {
				sp->initWithSpriteFrameName("icon_gold.png");
			} else {
				sp->initWithSpriteFrameName("icon_silver.png");
			}
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
			playIdMe = player.Info.UserID;
			playerMe = player;
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
	for (Node* n : spInvites) {
		n->setVisible(false);
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
				vecUsers[index]->setVisible(true);
				vecUsers[index]->setAlpha(255);
				vecUsers[index]->setPlayerName(player.Info.DisplayName);
				vecUsers[index]->setPlayerMoney(player.PMoney);
				vecUsers[index]->setName(player.Info.Name);
				lbTourPoints[index]->setString(to_string(player.TPoint));
				lbTourPoints[index]->getParent()->setVisible(isTourGame);
				if (player.Index == 0) {
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
				playIdMe = player.Info.UserID;
				playerMe = player;
			}
		}
	}
	if (!isSolo) {
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
				Sprite* sp = Sprite::createWithSpriteFrameName("100.png");
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

void GameScene::onTableReconnectDataResponse(TableReconnectData data)
{
	SFSRequest::getSingleton().RequestJoinRoom(data.Room, false);
	showWaiting();
}

void GameScene::onTourRoomMatch(long totalMatch)
{
	TourInfo tour = Utils::getSingleton().tourInfo;
	Label* lbMatch = (Label*)tableInfo->getChildByName("lbmatch");
	lbMatch->setString(Utils::getSingleton().getStringForKey("van") + ": " + to_string(totalMatch) + "/ " + to_string(tour.MaxMatch));
}

void GameScene::onTourTimeWaitPlayer(long timeWait)
{
	if (state != NONE && state != READY) return;
	timeWaitPlayer = timeWait;
	Label* lbTitleWaitPlayer = (Label*)mLayer->getChildByName("lbTitleWaitPlayer");
	Label* lbTimeWaitPlayer = (Label*)mLayer->getChildByName("lbtimewaitplayer");
	string timeWaitString = Utils::getSingleton().getCountTimeStringBySecs(timeWaitPlayer, "%H:%M:%S");
	if (!lbTimeWaitPlayer) {
		lbTitleWaitPlayer = Label::createWithTTF(Utils::getSingleton().getStringForKey("giai_dau_se_bat_dau_sau"), "fonts/arial.ttf", 40);
		lbTitleWaitPlayer->setPosition(winSize.width / 2, winSize.height / 2 + 50);
		lbTitleWaitPlayer->setName("lbTitleWaitPlayer");
		mLayer->addChild(lbTitleWaitPlayer, constant::GAME_ZORDER_BUTTON);
		autoScaleNode(lbTitleWaitPlayer);

		lbTimeWaitPlayer = Label::createWithTTF(timeWaitString, "fonts/arialbd.ttf", 50);
		lbTimeWaitPlayer->setPosition(winSize.width / 2, winSize.height / 2);
		lbTimeWaitPlayer->setName("lbtimewaitplayer");
		mLayer->addChild(lbTimeWaitPlayer, constant::GAME_ZORDER_BUTTON);
		autoScaleNode(lbTimeWaitPlayer);

		DelayTime *delayTime = DelayTime::create(1);
		CallFunc *func = CallFunc::create([=]() {
			if (timeWaitPlayer > 1) {
				timeWaitPlayer -= 1;
				string str = Utils::getSingleton().getCountTimeStringBySecs(timeWaitPlayer, "%H:%M:%S");
				lbTimeWaitPlayer->setString(str);
			} else {
				lbTitleWaitPlayer->setVisible(false);
				lbTimeWaitPlayer->setVisible(false);
				lbTimeWaitPlayer->setString("");
				lbTimeWaitPlayer->pause();
			}
		});
		Action* actionCount = RepeatForever::create(Sequence::createWithTwoActions(delayTime, func));
		lbTimeWaitPlayer->runAction(actionCount);
	} else {
		lbTimeWaitPlayer->setString(timeWaitString);
		lbTimeWaitPlayer->setVisible(true);
		lbTimeWaitPlayer->resume();
		lbTitleWaitPlayer->setVisible(true);
	}
}

bool GameScene::onKeyBack()
{
	bool canBack = BaseScene::onKeyBack();
	if (canBack) {
		onBackScene();
		return false;
	}
	return canBack;
}

void GameScene::onKeyHome()
{
}

void GameScene::onBackScene()
{
	if (state == NONE || state == READY || myServerSlot < 0) {
		SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
		Utils::getSingleton().goToLobbyScene();
		experimental::AudioEngine::stopAll();
	} else {
		hasRegisterOut = !hasRegisterOut;
		showSystemNotice(Utils::getSingleton().getStringForKey((hasRegisterOut ? "" : "huy_") + string("dang_ky_roi_ban_khi_het_van")));
		if (isTourGame) {
			SFSRequest::getSingleton().RequestTourLeaveTable();
		}
	}
}

void GameScene::reset()
{
	spChonCai->setVisible(false);
	btnBash->setVisible(false);
	btnBashBack->setVisible(false);
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
	spDealCards.clear();
	for (Node* n : vecStilts) {
		n->setVisible(false);
		n->removeAllChildren();
	}
	for (Sprite* sp : spHandCards) {
		sp->setVisible(false);
		sp->stopAllActions();
	}
	spHandCards.clear();
	RoomData roomData;
	roomData.TimeChooseHost = timeChooseHost;
	roomData.TimeDeal = timeDeal;
	roomData.TimeStart = timeStart;
	winMoneyData.ListUserId.clear();
	onRoomDataResponse(roomData);
}

void GameScene::initChatTable()
{
	tableChat = Node::create();
	tableChat->setPosition(560, 200);
	tableChat->setVisible(false);
	mLayer->addChild(tableChat, constant::ZORDER_POPUP);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	bg->setContentSize(Size(1500, 400));
	bg->setColor(Color3B::BLACK);
	//bg->setOpacity(150);
	tableChat->addChild(bg);

	ui::EditBox* input = ui::EditBox::create(Size(750, 65), "box_input.png", ui::Widget::TextureResType::PLIST);
	input->setPosition(Vec2(-140, 150));
	input->setMaxLength(100);
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

	ui::Button* btnSend = ui::Button::create("btn_send.png", "btn_send_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnSend->setPosition(Vec2(350, 150));
	btnSend->setScale(1.1f);
	addTouchEventListener(btnSend, [=]() {
		if (string(input->getText()).length() == 0) return;
		string text = string(input->getText());
		while (text.length() > 10) {
			std::string code_data = text.substr(0, 10);
			if (code_data.compare("codedata::") == 0) {
				text = text.substr(10, text.length() - 10);
			} else break;
		}
		SFSRequest::getSingleton().SendPublicMessage(text);
		input->setText("");
		hidePopup(tableChat);
	});
	tableChat->addChild(btnSend);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2(490, 150));
	btnClose->setScale(.8f);
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
		ui::Button* btn = ui::Button::create("box_input.png", "", "", ui::Widget::TextureResType::PLIST);
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

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setContentSize(Size(1040, 360));
	bg->setInsetLeft(300);
	bg->setInsetRight(30);
	bg->setInsetTop(30);
	bg->setInsetBottom(320);
	tableCrest->addChild(bg);

	Sprite* light = Sprite::createWithSpriteFrameName("btn_light.png");
	//light->setScale(20, 4);
	light->setScale(4, .09f);
	light->setPosition(0, 105);
	//light->setOpacity(100);
	tableCrest->addChild(light);

	lbChonCuoc = Label::create("", "fonts/guanine.ttf", 25);
	lbChonCuoc->setPosition(0, 130);
	lbChonCuoc->setColor(Color3B::YELLOW);
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
	int y = size2.height - 20;
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

	ui::Button* btnCrest = ui::Button::create("btn_xuong.png", "btn_xuong_clicked.png", "", ui::Widget::TextureResType::PLIST);
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

	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setScaleY(.85f);
	bg->setFlipY(true);
	tableEndMatch->addChild(bg);

	lbWinner = Label::createWithTTF("Stormus: Nha lau xe hoi hoa roi cua phat", "fonts/guanine.ttf", 25);
	lbWinner->setPosition(0, 80);
	lbWinner->setColor(Color3B(255, 255, 102));
	lbWinner->setAlignment(TextHAlignment::CENTER);
	lbWinner->setWidth(600);
	tableEndMatch->addChild(lbWinner);

	lbCuocSai = Label::createWithTTF("Hoa roi cua phat", "fonts/arialbd.ttf", 25);
	lbCuocSai->setPosition(-300, -40);
	lbCuocSai->setAnchorPoint(Vec2(0, .5f));
	lbCuocSai->setColor(Color3B(255, 102, 0));
	lbCuocSai->setWidth(600);
	lbCuocSai->setHeight(30);
	tableEndMatch->addChild(lbCuocSai);

	lbDiem = Label::createWithTTF("33", "fonts/arialbd.ttf", 25);
	lbDiem->setPosition(0, 0);
	lbDiem->setColor(Color3B(102, 255, 51));
	tableEndMatch->addChild(lbDiem);

	lbNoted = Label::createWithTTF("Reno bi den lang", "fonts/arial.ttf", 20);
	lbNoted->setVerticalAlignment(TextVAlignment::CENTER);
	lbNoted->setPosition(-300, -75);
	lbNoted->setColor(Color3B(255, 204, 102));
	lbNoted->setAnchorPoint(Vec2(0, .5f));
	lbNoted->setWidth(600);
	lbNoted->setHeight(50);
	tableEndMatch->addChild(lbNoted);

	lbNotedGa = Label::createWithTTF("Stoemus an ga", "fonts/arial.ttf", 20);
	lbNotedGa->setPosition(-235, -112);
	lbNotedGa->setColor(Color3B(255, 204, 102));
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

	/*Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setScale(1.1f, 1.5f);
	tableInvite->addChild(bg);*/

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setInsetBottom(320);
	bg->setInsetTop(30);
	bg->setInsetLeft(300);
	bg->setInsetRight(30);
	bg->setContentSize(Size(750, 550));
	tableInvite->addChild(bg);

	Sprite* bgTitle = Sprite::createWithSpriteFrameName("title_bg.png");
	bgTitle->setPosition(0, bg->getContentSize().height / 2 - 5);
	bgTitle->setName("spbgtitle");
	tableInvite->addChild(bgTitle);

	Sprite* title = Sprite::createWithSpriteFrameName("title_moichoi.png");
	title->setPosition(0, bg->getContentSize().height / 2 - 5);
	title->setScale(.8f);
	tableInvite->addChild(title);

	int sx = 650;
	ui::Scale9Sprite* inside = ui::Scale9Sprite::createWithSpriteFrameName("box.png");
	inside->setContentSize(Size(sx, 460));
	inside->setOpacity(200);
	inside->setPosition(0, -15);
	inside->setName("inside");
	inside->setVisible(false);
	tableInvite->addChild(inside);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-(sx - 20)/2, -240));
	scroll->setContentSize(Size(sx - 20, 450));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	tableInvite->addChild(scroll);

	ui::Button* btnDong = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnDong->setPosition(Vec2(bg->getContentSize().width / 2 - 5, bg->getContentSize().height / 2 - 5));
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

	Vec2 scale = getScaleSmoothly(1.2f);
	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setScale(scale.x, scale.y);
	popupSettings->addChild(bg);

	Sprite* bgTitle = Sprite::createWithSpriteFrameName("title_bg.png");
	bgTitle->setPosition(0, (bg->getContentSize().height / 2 - 5) * scale.y);
	bgTitle->setName("spbgtitle");
	popupSettings->addChild(bgTitle);

	Sprite* title = Sprite::createWithSpriteFrameName("title_caidat.png");
	title->setPosition(0, 205);
	//title->setScale(.8f);
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
		checkbox->loadTextureBackGround("box0.png", ui::Widget::TextureResType::PLIST);
		checkbox->loadTextureFrontCross("check.png", ui::Widget::TextureResType::PLIST);
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
		lb->setColor(Color3B::WHITE);
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

	cbs[3]->setSelected(isAutoReady);
	cbs[4]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_SOUND.c_str()));

	lbs[0]->setString(Utils::getSingleton().getStringForKey("win") + " 4-11");
	lbs[1]->setString(Utils::getSingleton().getStringForKey("quick"));
	lbs[2]->setString(Utils::getSingleton().getStringForKey("slow"));
	lbs[3]->setString(Utils::getSingleton().getStringForKey("auto_ready"));
	lbs[4]->setString(Utils::getSingleton().getStringForKey("sound"));

	ui::Button* btnOK = ui::Button::create("btn_submit.png", "btn_submit_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnOK->setPosition(Vec2(0, -205));
	addTouchEventListener(btnOK, [=]() {
		hidePopup(popupSettings);
		isAutoReady = cbs[3]->isSelected();
		UserDefault::getInstance()->setBoolForKey(constant::KEY_SOUND.c_str(), cbs[4]->isSelected());
		if (myServerSlot == 0 && (state == NONE || state == READY)){
			SFSRequest::getSingleton().RequestGameTableInfo(cbs[1]->isSelected(), cbs[0]->isSelected());
		}
		Utils::getSingleton().SoundEnabled = cbs[4]->isSelected();
		if (isAutoReady && btnReady->isVisible()) {
			state = READY;
			SFSRequest::getSingleton().RequestGameReady();
		}
	});
	popupSettings->addChild(btnOK);

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "btn_dong_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2((bg->getContentSize().width / 2 - 10) * scale.x, (bg->getContentSize().height / 2 - 5) * scale.y));
	//btnClose->setScale(.8f);
	addTouchEventListener(btnClose, [=]() {
		hidePopup(popupSettings);
	});
	popupSettings->addChild(btnClose);

	ui::Button* btnNo = ui::Button::create("white.png", "white.png", "", ui::Widget::TextureResType::PLIST);
	btnNo->setContentSize(Size(500, 150));
	btnNo->setPosition(Vec2(0, 60));
	btnNo->setScale9Enabled(true);
	btnNo->setOpacity(0);
	btnNo->setName("btnno");
	popupSettings->addChild(btnNo);

	ui::Button* btnNoAutoReady = ui::Button::create("white.png", "white.png", "", ui::Widget::TextureResType::PLIST);
	btnNoAutoReady->setContentSize(Size(500, 50));
	btnNoAutoReady->setPosition(cbs[3]->getPosition());
	btnNoAutoReady->setScale9Enabled(true);
	btnNoAutoReady->setOpacity(0);
	btnNoAutoReady->setName("btnnoautoready");
	popupSettings->addChild(btnNoAutoReady);
}

void GameScene::initTableInfo()
{
	tableInfo = Node::create();
	//tableInfo->setPosition(Vec2(0, 700) + getScaleSceneDistance(Vec2(105, -50)));
	tableInfo->setPosition(Vec2(0, 700) + getScaleSceneDistance(Vec2(195, -50)));
	mLayer->addChild(tableInfo, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(tableInfo);

	/*Sprite* bg = Sprite::createWithSpriteFrameName("bg_table_info.png");
	bg->setScaleX(.85f);
	tableInfo->addChild(bg);*/

	Sprite* icMoney = Sprite::createWithSpriteFrameName("icon_silver.png");
	icMoney->setPosition(-5, 15);
	icMoney->setScale(.5f);
	icMoney->setName("icmoney");
	icMoney->setVisible(!isTourGame);
	tableInfo->addChild(icMoney);

	Label* lbTableName = Label::create("", "fonts/arialbd.ttf", 18);
	lbTableName->setPosition(-90, isTourGame ? 25 : 15);
	lbTableName->setAnchorPoint(Vec2(0, .5f));
	lbTableName->setColor(Color3B(255, 200, 0));
	lbTableName->setName("lbname");
	tableInfo->addChild(lbTableName);

	Label* lbTableBet = Label::create("", "fonts/arialbd.ttf", 18);
	lbTableBet->setPosition(8, 15);
	lbTableBet->setAnchorPoint(Vec2(0, .5f));
	lbTableBet->setColor(Color3B(255, 200, 0));
	lbTableBet->setName("lbbet");
	lbTableBet->setVisible(!isTourGame);
	tableInfo->addChild(lbTableBet);

	Label* lbType = Label::create("", "fonts/arialbd.ttf", 18);
	lbType->setPosition(-90, isTourGame ? 0 : -15);
	lbType->setAnchorPoint(Vec2(0, .5f));
	lbType->setColor(Color3B(255, 200, 0));
	lbType->setName("lbtype");
	tableInfo->addChild(lbType);

	Label* lbMatch = Label::create(Utils::getSingleton().getStringForKey("van") + ": 0/ 12", "fonts/arialbd.ttf", 18);
	lbMatch->setPosition(-30, 25);
	lbMatch->setAnchorPoint(Vec2(0, .5f));
	lbMatch->setColor(Color3B(255, 200, 0));
	lbMatch->setName("lbmatch");
	lbMatch->setVisible(isTourGame);
	tableInfo->addChild(lbMatch);

	if (isTourGame) {
		time_t rawtime;
		time(&rawtime);
		TourInfo tour = Utils::getSingleton().tourInfo;
		tourTimeRemain = tour.Race1TimeEnd - rawtime + Utils::getSingleton().serverTimeDiff;
		if (tourTimeRemain <= 0) {
			tourTimeRemain = tour.Race2TimeEnd - rawtime + Utils::getSingleton().serverTimeDiff;
		}
		string timestr = Utils::getSingleton().getStringForKey("con") + ": " + Utils::getSingleton().getCountTimeStringBySecs(tourTimeRemain, "%H:%M:%S");

		Label* lbTourTime = Label::create(timestr, "fonts/arialbd.ttf", 18);
		lbTourTime->setPosition(-90, -25);
		lbTourTime->setAnchorPoint(Vec2(0, .5f));
		lbTourTime->setColor(Color3B(255, 200, 0));
		lbTourTime->setName("lbtourtime");
		lbTourTime->setVisible(isTourGame);
		tableInfo->addChild(lbTourTime);

		DelayTime *delayTime = DelayTime::create(1);
		CallFunc *func = CallFunc::create([=]() {
			if (tourTimeRemain > 1) {
				tourTimeRemain -= 1;
				string str = Utils::getSingleton().getStringForKey("con") + ": " + Utils::getSingleton().getCountTimeStringBySecs(tourTimeRemain, "%H:%M:%S");
				lbTourTime->setString(str);
			} else {
				lbTourTime->stopActionByTag(3);
				lbTourTime->setString(Utils::getSingleton().getStringForKey("con") + ": 00:00:00");
			}
		});
		Action* actionCount = RepeatForever::create(Sequence::createWithTwoActions(delayTime, func));
		actionCount->setTag(3);
		lbTourTime->runAction(actionCount);
	}
}

void GameScene::initCofferEffects()
{
	cofferSplash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	cofferSplash->setContentSize(Size(1500, 1000));
	cofferSplash->setPosition(560, 350);
	cofferSplash->setColor(Color3B::BLACK);
	cofferSplash->setOpacity(100);
	cofferSplash->setVisible(false);
	mLayer->addChild(cofferSplash, constant::ZORDER_COFFER);

	cofferEffect = Node::create();
	cofferEffect->setPosition(560, 350);
	cofferEffect->setVisible(false);
	mLayer->addChild(cofferEffect, constant::ZORDER_COFFER + 1);

	Sprite* spLight = Sprite::create();
	spLight->setName("splight");
	cofferEffect->addChild(spLight);
	spLight->runAction(RepeatForever::create(RotateBy::create(1, 90)));
	spLight->pauseSchedulerAndActions();

	Sprite* spCoffer = Sprite::create();
	cofferEffect->addChild(spCoffer);

	Label* lbMoney = Label::create("123.456", "fonts/arialbd.ttf", 50);
	lbMoney->setColor(Color3B::YELLOW);
	lbMoney->setPosition(0, -150);
	lbMoney->setName("lbmoney");
	cofferEffect->addChild(lbMoney);

	Label* lbName = Label::create("Stormus", "fonts/arialbd.ttf", 40);
	lbName->enableOutline(Color4B::BLACK, 2);
	lbName->setColor(Color3B::WHITE);
	lbName->setPosition(0, -210);
	lbName->setName("lbname");
	cofferEffect->addChild(lbName);

	if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS) {
		Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA8888);
		spCoffer->initWithFile("hu.png");
		spLight->initWithFile("as.png");
		Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
	} else if (Utils::getSingleton().ispmE()) {
		Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA8888);
		string host = Utils::getSingleton().textureHost;
		Utils::getSingleton().LoadTextureFromURL(host + "hu.png", [=](Texture2D* texture1) {
			spCoffer->initWithTexture(texture1);
			Utils::getSingleton().LoadTextureFromURL(host + "as.png", [=](Texture2D* texture2) {
				spLight->initWithTexture(texture2);
				Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
			});
		});
	}
}

bool GameScene::bashCardDown(int indexFrom, int indexTo, int cardId, bool isMe)
{
	bool rightData = true;
	if (cardId > 100) cardId = 256 - cardId;
	Sprite* spCard = NULL;
	int zorder = 0;
	int index2 = indexTo * 2;
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
	if (isMe) {
		if (chosenCard > 0 && atoi(spHandCards[chosenCard]->getName().c_str()) % 1000 == cardId) {
			spCard = spHandCards[chosenCard];
			spHandCards.erase(spHandCards.begin() + chosenCard);
			chosenCard = -1;
		} else {
			if (chosenCard >= 0) {
				spHandCards[chosenCard]->setAnchorPoint(Vec2(.5f, -.2f));
			}
			for (int i = 0; i < spHandCards.size(); i++) {
				if (atoi(spHandCards[i]->getName().c_str()) % 1000 == cardId) {
					spCard = spHandCards[i];
					spHandCards.erase(spHandCards.begin() + i);
					break;
				}
			}
		}
		if (spCard == NULL) {
			spCard = getCardSprite(cardId);
			rightData = false;
		}
		int rot = spCard->getRotation();
		Vec2 scaledUserPos = getScaleScenePosition(vecUserPos[indexFrom]);
		float x = sin(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.x;
		float y = cos(CC_DEGREES_TO_RADIANS(rot)) * spCard->getContentSize().height + scaledUserPos.y;
		spCard->setPosition(x, y);
		spCard->setName(to_string((int)cardId));
		RotateTo* rotate = RotateTo::create(cardSpeed, 0);
		spCard->stopAllActions();
		spCard->runAction(rotate);
		zorder = spCard->getLocalZOrder();
	} else {
		spCard = getCardSprite(cardId);
		spCard->setRotation(0);
		spCard->setScale(cardScaleTableNew);
		spCard->setPosition(getScaleScenePosition(vecUserPos[indexFrom]));
	}

	spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + tableCardNumb[index2]);
	spCard->setTag(constant::TAG_CARD_TABLE + index2);
	spCard->setName(to_string((int)cardId));
	spCard->setAnchorPoint(Vec2(.5f, .5f));

	MoveTo* move = MoveTo::create(cardSpeed, pos);
	ScaleTo* scaleTo = ScaleTo::create(cardSpeed, cardScaleTableNew);
	spCard->runAction(move);
	spCard->runAction(scaleTo);
	changeZOrderAfterFly(spCard, constant::GAME_ZORDER_CARD_FLY);

	if (runningSpCard != NULL) runningSpCard->setScale(cardScaleTable);
	runningSpCard = spCard;
	return rightData;
}

void GameScene::autoBash(int card, int group)
{
	CCLOG("autoBash");
	bashCardDown(0, 0, card, true);
	isAutoBash = true;
}

void GameScene::onUserBashToMe(BashData data)
{
	if (data.TurnId == sfsIdMe) {
		noaction++;
		btnHold->setVisible(true);
		btnPick->setVisible(true);
	}
}

void GameScene::onUserBashBackToMe(BashBackData data)
{
	if (data.TurnId == sfsIdMe) {
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

void GameScene::onUserPickToMe(PickData data)
{
	DelayTime* delay = DelayTime::create(.5f);
	CallFunc* func = CallFunc::create([=]() {
		if (state == PLAY && data.TurnId == sfsIdMe) {
			noaction++;
			btnHold->setVisible(true);
			btnForward->setVisible(true);
		}
	});
	this->runAction(Sequence::createWithTwoActions(delay, func));
}

Sprite* GameScene::getCardSprite(int id)
{
	if (id < 0) id = -id;
	int cardName = getCardName(id);
	for (Sprite* sp : spCards) {
		if (sp && !sp->isVisible()) {
			sp->initWithSpriteFrameName(String::createWithFormat("%d.png", cardName)->getCString());
			sp->setAnchorPoint(Vec2(.5f, .5f));
			sp->setColor(Color3B::WHITE);
			sp->setVisible(true);
			sp->setOpacity(255);
			sp->setScale(1);
			return sp;
		}
	}
	Sprite* sp = Sprite::createWithSpriteFrameName(String::createWithFormat("%d.png", cardName)->getCString());
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
