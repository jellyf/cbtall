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

	/*handPos.push_back(Vec2(winSize.width / 2, 240));
	handPos.push_back(Vec2(720, winSize.height / 2));
	handPos.push_back(Vec2(winSize.width / 2, 460));
	handPos.push_back(Vec2(400, winSize.height / 2));*/

	handPos.push_back(Vec2(winSize.width / 2, winSize.height / 2 - 130));
	handPos.push_back(Vec2(winSize.width / 2 + 130, winSize.height / 2));
	handPos.push_back(Vec2(winSize.width / 2, winSize.height / 2 + 130));
	handPos.push_back(Vec2(winSize.width / 2 - 130, winSize.height / 2));

	tableCardPos.push_back(Vec2(winSize.width / 2 + 300, 150));
	tableCardPos.push_back(Vec2(30, 125));
	tableCardPos.push_back(Vec2(winSize.width - 155, winSize.height / 2 - 30));
	tableCardPos.push_back(Vec2(winSize.width - 30, winSize.height / 2 + 172));
	tableCardPos.push_back(Vec2(winSize.width / 2 - 85, winSize.height - 90));
	tableCardPos.push_back(Vec2(winSize.width / 2 + 85, winSize.height - 90));
	tableCardPos.push_back(Vec2(155, winSize.height / 2 - 30));
	tableCardPos.push_back(Vec2(30, winSize.height / 2 + 172));
	tableCardPos.push_back(Vec2(winSize.width / 2 + 85, 90));
	tableCardPos.push_back(Vec2(winSize.width / 2 + 300, 150));

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

	vecUserPos.push_back(Vec2(winSize.width / 2, 90));
	vecUserPos.push_back(Vec2(winSize.width - 70, winSize.height / 2 - 30));
	vecUserPos.push_back(Vec2(winSize.width / 2, winSize.height - 90));
	vecUserPos.push_back(Vec2(70, winSize.height / 2 - 30));

	int psize = tableCardPos.size();
	for (auto i = 0; i < psize; i++) {
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

	string bgName = "imgs/" + (isSolo ? "bgVuongPhu.jpg" : "bg" + zone + ".jpg");
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGB565);
	Texture2D* bgTexture = TextureCache::getInstance()->addImage(bgName);
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);

	Sprite* bg = Sprite::createWithTexture(bgTexture);
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	mLayer->addChild(bg);

	Sprite* centerBg = Sprite::createWithSpriteFrameName("center.png");
	centerBg->setPosition(bg->getPosition());
	//centerBg->setVisible(!isSolo);
	mLayer->addChild(centerBg);
	autoScaleNode(centerBg);

	if (isSolo) {
		Sprite* dragon1 = Sprite::create("dragon.png");
		dragon1->setPosition(bg->getPosition() - Vec2(350, 0));
		dragon1->setScale(1.4f);
		mLayer->addChild(dragon1);
		autoScaleNode(dragon1);

		Sprite* dragon2 = Sprite::create("dragon.png");
		dragon2->setPosition(bg->getPosition() + Vec2(350, 0));
		dragon2->setScale(1.4f);
		dragon2->setFlippedX(true);
		mLayer->addChild(dragon2);
		autoScaleNode(dragon2);
	}

	playLayer = Layer::create();
	mLayer->addChild(playLayer, 10);
	autoScaleNode(playLayer);
	playLayer->setPosition(winSize.width * (scaleScene.y - 1) / 2, winSize.height * (scaleScene.x - 1) / 2);

	endLayer = Layer::create();
	mLayer->addChild(endLayer, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(endLayer);
	//endLayer->setPosition(winSize.width * (scaleScene.y - 1) / 2, winSize.height * (scaleScene.x - 1) / 2);
	endLayer->setPositionY(winSize.height * (scaleScene.x - 1) / 4);

	Vec2 topLeft = Vec2(0, winSize.height);
	Vec2 topRight = Vec2(winSize.width, winSize.height);

	btnSettings = ui::Button::create("btn_settings.png", "", "", ui::Widget::TextureResType::PLIST);
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

	btnChat = ui::Button::create("btn_chat.png", "", "", ui::Widget::TextureResType::PLIST);
	btnChat->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));// Vec2(975, 650));
	btnChat->setTouchEnabled(false);
	addTouchEventListener(btnChat, [=]() {
		//if (gameSplash->isVisible()) return;
		showPopup(tableChat);
	});
	mLayer->addChild(btnChat, constant::ZORDER_POPUP - 3);
	autoScaleNode(btnChat);
	vecMenuBtns.push_back(btnChat);

	btnBack = ui::Button::create("btn_back.png", "", "", ui::Widget::TextureResType::PLIST);
	btnBack->setPosition(topLeft + getScaleSceneDistance(Vec2(50, -50)));
	addTouchEventListener(btnBack, [=]() {
		onBackScene();
	});
	mLayer->addChild(btnBack, constant::ZORDER_POPUP - 4);
	autoScaleNode(btnBack);
	//vecMenuBtns.push_back(btnBack);

	btnUp = ui::Button::create("btn_up.png", "", "", ui::Widget::TextureResType::PLIST);
	btnUp->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	btnUp->setVisible(false);
	addTouchEventListener(btnUp, [=]() {
		hideMenuButtons();
	});
	mLayer->addChild(btnUp, constant::ZORDER_POPUP - 1);
	autoScaleNode(btnUp);

	btnDown = ui::Button::create("btn_down.png", "", "", ui::Widget::TextureResType::PLIST);
	btnDown->setPosition(topRight + getScaleSceneDistance(Vec2(-50, -50)));
	addTouchEventListener(btnDown, [=]() {
		showMenuButtons();
	});
	mLayer->addChild(btnDown, constant::ZORDER_POPUP - 1);
	autoScaleNode(btnDown);

	initCofferView(topRight + getScaleSceneDistance(Vec2(-150, -50)), constant::GAME_ZORDER_BUTTON, .8f);

	iconGa = Sprite::createWithSpriteFrameName("ga_off.png");
	iconGa->setPosition(topRight + getScaleSceneDistance(Vec2(Utils::getSingleton().ispmE() ? -240 : -150, -45)));
	iconGa->setVisible(!isTourGame);
	mLayer->addChild(iconGa, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(iconGa);

	lbMoneyGa = Label::createWithTTF("", "fonts/myriad.ttf", 20);
	lbMoneyGa->setPosition(iconGa->getPosition() - getScaleSceneDistance(Vec2(0, 40)));
	mLayer->addChild(lbMoneyGa, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(lbMoneyGa);

	//spNetwork->setPosition(topRight + getScaleSceneDistance(Vec2(pmE ? -240 : -150, -50)));
	spNetwork->setPosition(Vec2(winSize.width, 0) + getScaleSceneDistance(Vec2(-40, 35)));

	btnReady = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnReady->setTitleText(Utils::getSingleton().getStringForKey("ready"));
	btnReady->setTitleFontName("fonts/myriadb.ttf");
	btnReady->setTitleFontSize(40);
	btnReady->setTitleDeviation(Vec2(0, -5));
	btnReady->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
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

	btnCancelReady = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnCancelReady->setTitleText(Utils::getSingleton().getStringForKey("cancel_ready"));
	btnCancelReady->setTitleFontName("fonts/myriadb.ttf");
	btnCancelReady->setTitleFontSize(40);
	btnCancelReady->setTitleDeviation(Vec2(0, -5));
	//btnCancelReady->setContentSize(Size(200, 65));
	btnCancelReady->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
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

	btnBash = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnBash->setTitleText(Utils::getSingleton().getStringForKey("danh"));
	btnBash->setTitleFontName("fonts/myriadb.ttf");
	btnBash->setTitleFontSize(40);
	btnBash->setTitleDeviation(Vec2(0, -5));
	btnBash->setPosition(Vec2(winSize.width / 2 + 340, 35));
	btnBash->setVisible(false);
	addTouchEventListener(btnBash, [=]() {
		int hcsize = spHandCards.size();
		if (chosenCard < 0 || chosenCard >= hcsize) return;
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

	btnForward = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnForward->setTitleText(Utils::getSingleton().getStringForKey("duoi"));
	btnForward->setTitleFontName("fonts/myriadb.ttf");
	btnForward->setTitleFontSize(40);
	btnForward->setTitleDeviation(Vec2(0, -5));
	btnForward->setPosition(btnBash->getPosition());
	btnForward->setVisible(false);
	addTouchEventListener(btnForward, [=]() {
		SFSRequest::getSingleton().RequestGameForward();
		noaction = 0;
	});
	mLayer->addChild(btnForward, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnForward);

	btnBashBack = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnBashBack->setTitleText(Utils::getSingleton().getStringForKey("danh"));
	btnBashBack->setTitleFontName("fonts/myriadb.ttf");
	btnBashBack->setTitleFontSize(40);
	btnBashBack->setTitleDeviation(Vec2(0, -5));
	btnBashBack->setPosition(btnBash->getPosition());
	btnBashBack->setVisible(false);
	addTouchEventListener(btnBashBack, [=]() {
		int hcsize = spHandCards.size();
		if (chosenCard < 0 || chosenCard >= hcsize) return;
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

	btnHold = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnHold->setTitleText(Utils::getSingleton().getStringForKey("an"));
	btnHold->setTitleFontName("fonts/myriadb.ttf");
	btnHold->setTitleFontSize(40);
	btnHold->setTitleDeviation(Vec2(0, -5));
	btnHold->setPosition(Vec2(winSize.width / 2 - 340, 35));
	btnHold->setVisible(false);
	addTouchEventListener(btnHold, [=]() {
		int hcsize = spHandCards.size();
		if (chosenCard < 0 || chosenCard >= hcsize) return;
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

	btnPick = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnPick->setTitleText(Utils::getSingleton().getStringForKey("boc"));
	btnPick->setTitleFontName("fonts/myriadb.ttf");
	btnPick->setTitleFontSize(40);
	btnPick->setTitleDeviation(Vec2(0, -5));
	btnPick->setPosition(btnBash->getPosition());
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

	btnPenet = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnPenet->setTitleText(Utils::getSingleton().getStringForKey("chiu"));
	btnPenet->setTitleFontName("fonts/myriadb.ttf");
	btnPenet->setTitleFontSize(40);
	btnPenet->setTitleDeviation(Vec2(0, -5));
	btnPenet->setPosition(btnBash->getPosition());
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
    
    btnDropPenet = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnDropPenet->setTitleText(Utils::getSingleton().getStringForKey("bo_chiu"));
	btnDropPenet->setTitleFontName("fonts/myriadb.ttf");
	btnDropPenet->setTitleFontSize(40);
	btnDropPenet->setTitleDeviation(Vec2(0, -5));
    btnDropPenet->setPosition(btnHold->getPosition());
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

	btnDropWin = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnDropWin->setTitleText(Utils::getSingleton().getStringForKey("bo_u"));
	btnDropWin->setTitleFontName("fonts/myriadb.ttf");
	btnDropWin->setTitleFontSize(40);
	btnDropWin->setTitleDeviation(Vec2(0, -5));
	btnDropWin->setPosition(btnHold->getPosition());
	btnDropWin->setVisible(false);
	addTouchEventListener(btnDropWin, [=]() {
		dropWin();
	});
	mLayer->addChild(btnDropWin, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(btnDropWin);

	btnXemNoc = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnXemNoc->setTitleText(Utils::getSingleton().getStringForKey("xem_noc"));
	btnXemNoc->setTitleFontName("fonts/myriadb.ttf");
	btnXemNoc->setTitleFontSize(40);
	btnXemNoc->setTitleDeviation(Vec2(0, -5));
	btnXemNoc->setPosition(Vec2(winSize.width / 2, 300));
	btnXemNoc->setVisible(false);
	addTouchEventListener(btnXemNoc, [=]() {
		showStiltCards();
	});
	mLayer->addChild(btnXemNoc, constant::GAME_ZORDER_SPLASH + 2);
	autoScaleNode(btnXemNoc);

	btnDongNoc = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnDongNoc->setTitleText(Utils::getSingleton().getStringForKey("dong_noc"));
	btnDongNoc->setTitleFontName("fonts/myriadb.ttf");
	btnDongNoc->setTitleFontSize(40);
	btnDongNoc->setTitleDeviation(Vec2(0, -5));
	btnDongNoc->setPosition(btnXemNoc->getPosition());
	btnDongNoc->setVisible(false);
	addTouchEventListener(btnDongNoc, [=]() {
		showWinnerCards();
	});
	mLayer->addChild(btnDongNoc, constant::GAME_ZORDER_SPLASH + 2);
	autoScaleNode(btnDongNoc);

	lbNoticeAction = Label::createWithTTF("", "fonts/myriadb.ttf", 35);
	lbNoticeAction->setPosition(winSize.width / 2, winSize.height / 2);
	lbNoticeAction->setColor(Color3B::YELLOW);
	lbNoticeAction->enableOutline(Color4B::BLACK, 2);
	lbNoticeAction->setVisible(false);
	mLayer->addChild(lbNoticeAction);
	autoScaleNode(lbNoticeAction);

	Sprite* bgDiaNoc = Sprite::createWithSpriteFrameName("bg_dianoc.png");
	bgDiaNoc->setPosition(Vec2(winSize.width / 2 + 4, winSize.height / 2 + 72));
	mLayer->addChild(bgDiaNoc);
	bgDiaNoc->setVisible(false);
	autoScaleNode(bgDiaNoc);

	Sprite* spCardNoc = Sprite::createWithSpriteFrameName("bg_card_up.png");
	//spCardNoc->setRotation(-20);
	spCardNoc->setPosition(bgDiaNoc->getContentSize().width / 2 + 30, bgDiaNoc->getContentSize().height / 2 + 10);
	spCardNoc->setName("spcardnoc");
	bgDiaNoc->addChild(spCardNoc, 1);

	ui::Button* spNapNoc = ui::Button::create("bg_napnoc.png", "", "", ui::Widget::TextureResType::PLIST);
	//Sprite* spNapNoc = Sprite::createWithSpriteFrameName("bg_napnoc.png");
	spNapNoc->setPosition(Vec2(bgDiaNoc->getContentSize().width / 2, bgDiaNoc->getContentSize().height / 2 + 3));
	bgDiaNoc->addChild(spNapNoc, 2);
	addTouchEventListener(spNapNoc, [=]() {
		if (btnPick->isVisible()) {
			SFSRequest::getSingleton().RequestGamePick();
			progressTimer->setVisible(false);
			progressTimer->stopAllActions();
		}
	});

	lbCardNoc = Label::createWithTTF("23", "fonts/myriad.ttf", 35);
	lbCardNoc->setString("23");
	lbCardNoc->setPosition(bgDiaNoc->getContentSize().width / 2 + 65, bgDiaNoc->getContentSize().height / 2 + 15);
	bgDiaNoc->addChild(lbCardNoc, 3);

	nodeStilt = Node::create();
	nodeStilt->setPosition(getScaleScenePosition(Vec2(winSize.width / 2, winSize.height / 2)));
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
		node->setPosition(v +Vec2(winSize.width / 2, winSize.height / 2));
		node->setName(to_string(ii++));
		mLayer->addChild(node, constant::GAME_ZORDER_USER + 10);
		vecStilts.push_back(node);
	}*/

	int usize = vecUserPos.size();
	for (int i = 0; i < usize; i++) {
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

		Sprite* spSS = Sprite::createWithSpriteFrameName("icon_sansang.png");
		spSS->setPosition(vecUserPos[i] + getScaleSceneDistance(Vec2(-35, -35)));
		spSS->setVisible(false);
		mLayer->addChild(spSS, constant::GAME_ZORDER_USER + 9);
		spSanSangs.push_back(spSS);
		autoScaleNode(spSS);

		Label* lbSS = Label::createWithTTF(Utils::getSingleton().getStringForKey("ready"), "fonts/myriadb.ttf", 40);
		lbSS->setPosition(vecUserPos[i] + getScaleSceneDistance(Vec2(0, i == 0 ? 100 : -115)));
		lbSS->setColor(Color3B(78, 248, 126));
		lbSS->enableOutline(Color4B(2, 49, 15, 255), 2);
		lbSS->setVisible(false);
		mLayer->addChild(lbSS, constant::GAME_ZORDER_USER + 9);
		lbSanSangs.push_back(lbSS);
		autoScaleNode(lbSS);

		/*Sprite* spBB = Sprite::createWithSpriteFrameName("txt_bat_bao.png");
		spBB->setPosition(vecUserPos[i]);
		spBB->setVisible(false);
		mLayer->addChild(spBB, constant::GAME_ZORDER_USER + 9);
		spBatBaos.push_back(spBB);
		autoScaleNode(spBB);*/

		Label* lbBB = Label::createWithTTF(Utils::getSingleton().getStringForKey("bat_bao"), "fonts/myriadb.ttf", 40);
		lbBB->setPosition(vecUserPos[i] + getScaleSceneDistance(Vec2(0, i == 0 ? 95 : -110)));
		lbBB->setColor(Color3B(120, 120, 120));
		lbBB->enableOutline(Color4B(50, 50, 50, 255), 2);
		lbBB->setVisible(false);
		mLayer->addChild(lbBB, constant::GAME_ZORDER_USER + 9);
		lbBatBaos.push_back(lbBB);
		autoScaleNode(lbBB);

		Label* lb1 = Label::createWithTTF("100,000", "fonts/myriadb.ttf", 45);
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

		Label *lbPoint = Label::createWithTTF("100", "fonts/myriadb.ttf", 25);
		lbPoint->setPosition(spPointBg->getContentSize().width/2, spPointBg->getContentSize().height/2 + 2);
		spPointBg->addChild(lbPoint);
		lbTourPoints.push_back(lbPoint);

		ui::Button* btnKick = ui::Button::create("empty.png", "empty.png", "", ui::Widget::TextureResType::PLIST);
		mLayer->addChild(btnKick, constant::GAME_ZORDER_USER + 12);
		btnKick->setPosition(vecUserPos[i] + getScaleSceneDistance(Vec2(40, 35)));
		btnKick->setContentSize(Size(70, 70));
		btnKick->setScale9Enabled(true);
		btnKick->setVisible(false);
		vecBtnKicks.push_back(btnKick);
		autoScaleNode(btnKick);

		Sprite* spKick = Sprite::createWithSpriteFrameName("btn_kick.png");
		spKick->setPosition(btnKick->getContentSize().width / 2, btnKick->getContentSize().height / 2);
		btnKick->addChild(spKick);

		addTouchEventListener(btnKick, [=](){
			showPopupNotice(Utils::getSingleton().getStringForKey("ask_to_kick_player"), [=]() {
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
	spChuPhong->setScale(.6f);
	spChuPhong->setVisible(false);
	mLayer->addChild(spChuPhong, constant::GAME_ZORDER_USER + 10);
	autoScaleNode(spChuPhong);

	/*spChonCai = Sprite::createWithSpriteFrameName("txt_choncai.png");
	spChonCai->setVisible(false);
	mLayer->addChild(spChonCai, constant::GAME_ZORDER_USER + 9);
	autoScaleNode(spChonCai);*/

	lbChonCai = Label::createWithTTF(Utils::getSingleton().getStringForKey("choose_host"), "fonts/myriadb.ttf", 40);
	lbChonCai->enableOutline(Color4B(77, 74, 0, 255), 2);
	lbChonCai->setColor(Color3B(255, 246, 0));
	lbChonCai->setVisible(false);
	mLayer->addChild(lbChonCai, constant::GAME_ZORDER_USER + 9);
	autoScaleNode(lbChonCai);

	Vec2 scale1 = getScaleSmoothly(1.1f);
	Vec2 scale2 = getScaleSmoothly(1);
	ScaleTo* scaleChonCai1 = ScaleTo::create(.5f, scale1.x, scale1.y);
	ScaleTo* scaleChonCai2 = ScaleTo::create(.5f, scale2.x, scale2.y);
	lbChonCai->runAction(RepeatForever::create(Sequence::create(scaleChonCai1, scaleChonCai2, nullptr)));
	lbChonCai->pause();

	Sprite* bgCrestTime = Sprite::createWithSpriteFrameName("bg_countdown.png");
	bgCrestTime->setPosition(btnXemNoc->getPosition() + getScaleSceneDistance(Vec2(170, 0)));
	bgCrestTime->setVisible(false);
	mLayer->addChild(bgCrestTime, constant::GAME_ZORDER_SPLASH + 2);
	autoScaleNode(bgCrestTime);

	lbCrestTime = Label::createWithTTF("", "fonts/myriadb.ttf", 50);
	lbCrestTime->setPosition(bgCrestTime->getContentSize().width/2, bgCrestTime->getContentSize().height/2 - 5);
	bgCrestTime->addChild(lbCrestTime);

	DelayTime* delay = DelayTime::create(1);
	CallFunc* func = CallFunc::create([=]() {
		if (state != CREST && state != ENDING && (btnXemNoc->isVisible() || btnDongNoc->isVisible())) {
			btnXemNoc->setVisible(false);
			btnDongNoc->setVisible(false);
			endLayer->removeAllChildren();
		}
		if (lbCrestTime->getString().compare("0") == 0) {
			lbCrestTime->getParent()->setVisible(false);
			lbCrestTime->pause();
			return;
		}
		int time = atoi(lbCrestTime->getString().c_str());
		time--;
		lbCrestTime->setString(to_string(time));
	});
	lbCrestTime->runAction(RepeatForever::create(Sequence::create(delay, func, nullptr)));
	lbCrestTime->pause();

	initChatTable();
	initCrestTable();
	initEndMatchTable();
	initInviteTable();
	initSettingsPopup();
	initTableInfo();
	initPopupUserInfo();
	initCofferEffects();
	//initEventView(Vec2(0, 680), Size(Director::getInstance()->getVisibleSize().width, 40));

	Node* nodeError = Node::create();
	nodeError->setPosition(winSize.width / 2, winSize.height / 2);
	nodeError->setVisible(false);
	mLayer->addChild(nodeError, constant::ZORDER_POPUP + 10);
	autoScaleNode(nodeError);

	Sprite* bgError = Sprite::createWithSpriteFrameName("popup_bg.png");
	nodeError->addChild(bgError);

	lbError = Label::createWithTTF("", "fonts/myriadb.ttf", 30);
	lbError->setAlignment(TextHAlignment::CENTER);
	lbError->setColor(Color3B::YELLOW);
	lbError->setWidth(600);
	nodeError->addChild(lbError);

	gameSplash = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	gameSplash->setContentSize(Size(1500, 1000));
	gameSplash->setPosition(winSize.width / 2, winSize.height / 2);
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

	SFSRequest::getSingleton().mapHttpResponseFailedCallbacks["scene"] = std::bind(&GameScene::onHttpResponseFailed, this, std::placeholders::_1);
	SFSRequest::getSingleton().mapHttpResponseCallbacks["scene"] = std::bind(&GameScene::onHttpResponse, this, std::placeholders::_1, std::placeholders::_2);
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

	SFSRequest::getSingleton().mapHttpResponseFailedCallbacks.erase("scene");
	SFSRequest::getSingleton().mapHttpResponseCallbacks.erase("scene");
}

bool GameScene::onTouchBegan(Touch * touch, Event * _event)
{
	if (!BaseScene::onTouchBegan(touch, _event) && !splash->isVisible() && !gameSplash->isVisible()) {
		Point pos = touch->getLocation();
		pos = mLayer->convertToNodeSpace(pos);

		if (myServerSlot == 0 && (state == NONE || state == READY)
			&& iconGa->getBoundingBox().containsPoint(pos)) {
			SFSRequest::getSingleton().RequestGameGa();
			return true;
		}

		if (startGameData.LastWinner == sfsIdMe) {
			int psize = dealPos.size();
			if (state == CHOOSE_STILT && chosenStilt == -1) {
				for (int i = 0; i < psize; i++) {
					if (pos.distance(Vec2(winSize.width / 2, winSize.height / 2) + dealPos[i]) < 100) {
						chosenStilt = i;
						state = CHOOSE_HOST;
						SFSRequest::getSingleton().RequestGameChooseStilt(i + 1);
						return true;
					}
				}
			} else if (state == CHOOSE_HOST && chosenStiltHost == -1) {
				Point pos2 = touch->getLocation();
				pos2 = nodeStilt->convertToNodeSpace(pos2);
				for (int i = 0; i < psize; i++) {
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
			if (pos.x < winSize.width / 2) rot *= -1;
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
				} else if(spInvites[i]->isVisible()){
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
				int hcsize = spHandCards.size();
				for (int i = 0; i < hcsize; i++) {
					if (abs(rot - spHandCards[i]->getRotation()) < 5) {
						if (chosenCard >= 0 && chosenCard < hcsize) {
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
}

void GameScene::onApplicationWillEnterForeground()
{
    // Do NOT call BaseScene::onApplicationWillEnterForeground();
    spNetwork->resume();
    lbNetwork->resume();
	if (isPauseApp) {
		isPauseApp = false;
		double curTime = Utils::getSingleton().getCurrentSystemTimeInSecs();
		float pauseTime = curTime - pauseTimeInSecs;
		if (pauseTime > 120) {
			float timeWait = pauseTime / 40;
			if (timeWait > 20) timeWait = 20;
			showWaiting(timeWait + 10);
			SFSGEvent::getSingleton().DoWork(false);
			this->delayFunction(this, timeWait, [=]() {
				SFSGEvent::getSingleton().DoWork(true);
				this->disconnectToSync();
			});
		}
	}
}

void GameScene::onKeyHome()
{
	isPauseApp = true;
	pauseTimeInSecs = Utils::getSingleton().getCurrentSystemTimeInSecs();
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
	nodePos.push_back(Vec2(winSize.width / 2 - 240, winSize.height / 2));
	nodePos.push_back(Vec2(winSize.width / 2 + 240, winSize.height / 2));

	int psize = nodePos.size();
	vector<Node*> nodes;
	for (int i = 0; i < psize; i++) {
		Node* node = Node::create();
		node->setPosition(getScaleScenePosition(nodePos[i]));
		playLayer->addChild(node, constant::GAME_ZORDER_USER + 1);
		nodes.push_back(node);
	}

	int dsize = dealPos.size();
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < dsize; j++) {
			for (int k = 0; k < psize; k++) {
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
		MoveTo* move = MoveTo::create(.5f, getScaleScenePosition(Vec2(winSize.width / 2, winSize.height / 2)));
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
			//lbNoticeAction->setVisible(true);
			lbNoticeAction->setString(Utils::getSingleton().getStringForKey("choose_stilt"));
		}
		//spChonCai->setVisible(true);
		//spChonCai->setPosition(vecUserPos[userIndexs[startGameData.LastWinner]]);
		int index = userIndexs[startGameData.LastWinner];
		lbChonCai->setVisible(true);
		lbChonCai->resume();
		lbChonCai->setPosition(vecUserPos[index] + getScaleSceneDistance(Vec2(0, index == 0 ? 100 : -115)));

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
	int tksize = myCardHand.ThienKhai.size();
	for (int i = 0; i < tksize; i++) {
		for (int j = 0; j < 4; j++) {
			getCardSpriteToHand(myCardHand.ThienKhai[i], constant::CARD_GROUP_THIENKHAI, k);
			k++;
		}
	}
	int chansize = myCardHand.Chan.size();
	for (int i = 0; i < chansize; i++) {
		for (int j = 0; j < 2; j++) {
			getCardSpriteToHand(myCardHand.Chan[i], constant::CARD_GROUP_CHAN, k);
			k++;
		}
	}
	int casize = myCardHand.Ca.size();
	for (int i = 0; i < casize; i++) {
		int id1 = (myCardHand.Ca[i] - 1) * 3 + (myCardHand.Ca[i + 1] - 1);
		getCardSpriteToHand(id1, constant::CARD_GROUP_CA, k);
		k++;

		int id2 = (myCardHand.Ca[i] - 1) * 3 + (myCardHand.Ca[i + 2] - 1);
		getCardSpriteToHand(id2, constant::CARD_GROUP_CA, k);
		k++;
		i += 2;
	}
	int bdsize = myCardHand.BaDau.size();
	for (int i = 0; i < bdsize; i++) {
		for (int j = 0; j < 3; j++) {
			int id = (myCardHand.BaDau[i] - 1) * 3 + j;
			getCardSpriteToHand(id, constant::CARD_GROUP_3DAU, k);
			k++;
		}
	}
	int qsize = myCardHand.Que.size();
	for (int i = 0; i < qsize; i++) {
		getCardSpriteToHand(myCardHand.Que[i], constant::CARD_GROUP_QUE, k);
		k++;
	}

	int rot = 11;
	int startRot = rot / 2 - rot * spHandCards.size() / 2;
	int hcsize = spHandCards.size();
	for (int i = 0; i < hcsize; i++) {
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
	} else if (waitAction == constant::GAME_ACTION_PICK) {
		btnForward->setVisible(true);
		btnHold->setVisible(true);
	} else if (waitAction == constant::GAME_ACTION_FORWARD) {
		btnForward->setVisible(true);
		btnHold->setVisible(true);
	}
	waitAction = constant::GAME_ACTION_NONE;
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
	int startRot = rot / 2 - rot * spHandCards.size() / 2;
	vector<int> ids, rots, groups;
	int tksize = myCardHand.ThienKhai.size();
	for (int i = 0; i < tksize; i++) {
		for (int j = 0; j < 4; j++) {
			ids.push_back(myCardHand.ThienKhai[i]);
			rots.push_back(startRot + k * rot);
			groups.push_back(constant::CARD_GROUP_THIENKHAI);
			k++;
		}
	}
	int chansize = myCardHand.Chan.size();
	for (int i = 0; i < chansize; i++) {
		for (int j = 0; j < 2; j++) {
			ids.push_back(myCardHand.Chan[i]);
			rots.push_back(startRot + k * rot);
			groups.push_back(constant::CARD_GROUP_CHAN);
			k++;
		}
	}
	int casize = myCardHand.Ca.size();
	for (int i = 0; i < casize; i++) {
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
	int bdsize = myCardHand.BaDau.size();
	for (int i = 0; i < bdsize; i++) {
		for (int j = 0; j < 3; j++) {
			int id = (myCardHand.BaDau[i] - 1) * 3 + j;
			ids.push_back(id);
			rots.push_back(startRot + k * rot);
			groups.push_back(constant::CARD_GROUP_3DAU);
			k++;
		}
	}
	int qsize = myCardHand.Que.size();
	for (int i = 0; i < qsize; i++) {
		ids.push_back(myCardHand.Que[i]);
		rots.push_back(startRot + k * rot);
		groups.push_back(constant::CARD_GROUP_QUE);
		k++;
	}

	vector<int> changedCards;
	vector<int> indexs;
	int hcsize = spHandCards.size();
	for (int i = 0; i < hcsize; i++) {
		indexs.push_back(i);
	}
	int isize = indexs.size();
	for (int i = 0; i < hcsize; i++) {
		spHandCards[i]->setScale(1.2f);
		int j = 0;
		for (; j < isize; j++) {
			if (atoi(spHandCards[i]->getName().c_str()) == groups[indexs[j]] + ids[indexs[j]]) {
				spHandCards[i]->setLocalZOrder(constant::GAME_ZORDER_HAND_CARD + indexs[j]);
				RotateTo* rotate = RotateTo::create(.5f, rots[indexs[j]]);
				spHandCards[i]->runAction(rotate);
				break;
			}
		}
		if (j == isize) {
			changedCards.push_back(i);
		} else {
			indexs.erase(indexs.begin() + j);
			isize--;
		}
	}

	int ccsize = changedCards.size();
	for (int i = 0; i < ccsize; i++) {
		for (int j = 0; j < isize; j++) {
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
		int startRot = rot / 2  - rot * spHandCards.size() / 2;
		bool ok = true;
		int hcsize = spHandCards.size();
		for (int i = 0; i < hcsize; i++) {
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
	int x = winSize.width / 2 + 15 - endMatchData.ListStiltCard.size() * size;
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
	int x1 = winSize.width / 2 - 275;// winSize.width / 2 - endMatchData.ListChanU.size() * 46 + 23;
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
	int x2 = x1 - 50;// winSize.width / 2 - endMatchData.ListCaU.size() * 23 + 23;
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
		int cusize = endMatchData.ListCardU.size();
		for (int i = 0; i < cusize; i ++) {
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

	int opacity = myServerSlot == 0 ? 255 : 150;
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
	showToast("system", msg, Vec2(winSize.width / 2, winSize.height / 2 + 50));
}

void GameScene::showCofferEffects(std::string money)
{
	if (!Utils::getSingleton().ispmE()) return;
	Label* lb = (Label*)cofferEffect->getChildByName("lbmoney");
	Sprite* spLight = (Sprite*)cofferEffect->getChildByName("splight");
	lb->setString(money);
	spLight->resume();

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
		cofferEffect->getChildByName("splight")->pause();
	});*/
	cofferEffect->setScale(0);
	cofferEffect->setVisible(true);
	//cofferEffect->runAction(Sequence::create(scaleTo1, scaleTo2, delay, scaleTo3, func, nullptr));
	cofferEffect->runAction(Sequence::createWithTwoActions(scaleTo1, scaleTo2));
	cofferSplash->setVisible(true);

    SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("icon_gold.png");
	ParticleSystemQuad *m_emitter = ParticleSystemQuad::create(Utils::getSingleton().mapParticle);
    m_emitter->setTextureWithRect(spriteFrame->getTexture(), spriteFrame->getRect());
	m_emitter->setPosition(0, 100);
	cofferEffect->addChild(m_emitter);
	m_emitter->runAction(Sequence::create(DelayTime::create(10), RemoveSelf::create(), NULL));
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
	vector<int> a, b;
	getCardCount(cardHand, a);
	getCardCount(myCardHand, a);
	int asize = a.size();
	for (int i = 0; i < asize; i++) {
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

void GameScene::getCardCount(CardHandData cardHand, std::vector<int>& a)
{
	for (int i = 0; i < 30; i++) a.push_back(0);
	int tksize = cardHand.ThienKhai.size();
	for (int i = 0; i < tksize; i++) {
		for (int j = 0; j < 4; j++) {
			a[cardHand.ThienKhai[i]] ++;
		}
	}
	int chansize = cardHand.Chan.size();
	for (int i = 0; i < chansize; i++) {
		for (int j = 0; j < 2; j++) {
			a[cardHand.Chan[i]] ++;
		}
	}
	int casize = cardHand.Ca.size();
	for (int i = 0; i < casize; i++) {
		int id1 = (cardHand.Ca[i] - 1) * 3 + (cardHand.Ca[i + 1] - 1);
		int id2 = (cardHand.Ca[i] - 1) * 3 + (cardHand.Ca[i + 2] - 1);
		a[id1] ++;
		a[id2] ++;
		i += 2;
	}
	int bdsize = cardHand.BaDau.size();
	for (int i = 0; i < bdsize; i++) {
		for (int j = 0; j < 3; j++) {
			int id = (cardHand.BaDau[i] - 1) * 3 + j;
			a[id] ++;
		}
	}
	int qsize = cardHand.Que.size();
	for (int i = 0; i < qsize; i++) {
		a[cardHand.Que[i]] ++;
	}
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
				//SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentRoomName);
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
		lbSanSangs[index]->setVisible(false);
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
	if (isTourGame && (code == 80 || code == 39 || code == 36 || code == 35)) {
		state = NONE;
		isMatchTimeEnd = true;
		//Utils::getSingleton().cachedErrors.push_back(pair<unsigned char, string>(code, msg));
		showPopupNotice(msg, [=]() {
			SFSRequest::getSingleton().RequestJoinRoom(Utils::getSingleton().currentLobbyName);
			Utils::getSingleton().goToLobbyScene();
		}, false);
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
	if (isPrepareToTour) {
		joinIntoTour();
		return;
	}
	if (state == ENDING && (noaction >= 3 || hasRegisterOut)) {
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
		//spBatBaos[i]->setVisible(false);
		lbBatBaos[i]->setVisible(false);
		spSanSangs[i]->setVisible(false);
		lbSanSangs[i]->setVisible(false);
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
				vecUsers[index]->setAvatarUrl(player.Info.AvatarUrl);
				spSanSangs[index]->setVisible(player.Ready);
				lbSanSangs[index]->setVisible(player.Ready);
				lbTourPoints[index]->setString(to_string(player.TPoint));
				lbTourPoints[index]->getParent()->setVisible(isTourGame);
				if (player.Index == 0) {
					spChuPhong->setVisible(true);
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
					vecUsers[index]->setAvatarUrl(Utils::getSingleton().userDataMe.AvatarUrl);
				}
			}
		}
		if (index == -1) {
			num++;
		}
	}

	if (roomData.Players.size() == 1) {
		lbCrestTime->getParent()->setVisible(false);
		btnReady->setVisible(false);
		btnCancelReady->setVisible(false);
		spSanSangs[0]->setVisible(false);
		lbSanSangs[0]->setVisible(false);
	} else if(roomData.Players.size() > 0){
		if (roomData.TimeStart > 0 && !lbCrestTime->getParent()->isVisible()) {
			lbCrestTime->getParent()->setVisible(true);
			lbCrestTime->setString(to_string((int)timeStart));
			lbCrestTime->resume();
			lbCrestTime->getParent()->setPosition(btnReady->getPosition() + getScaleSceneDistance(Vec2(0, 80)));
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
	btnCrest->setVisible(false);
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
	cofferEffect->getChildByName("splight")->pause();
	int tcsize = tableCardNumb.size();
	for (int i = 0; i < tcsize; i++) {
		tableCardNumb[i] = 0;
	}
	for (Sprite* sp : spCards) {
		sp->setVisible(false);
	}
	int stsize = vecStilts.size();
	for (int i = 0; i < stsize; i++) {
		vecStilts[i]->setVisible(true);
		//vecStilts[i]->setPosition(dealPos[i] + Vec2(winSize.width / 2, winSize.height / 2));
		vecStilts[i]->setPosition(dealPos[i]);
		vecStilts[i]->setRotation(0);
	}
	nodeStilt->setRotation(0);

	if (winMoneyData.ListUserId.size() > 0) {
		int usize = winMoneyData.ListUserId.size();
		for (int i = 0; i < usize; i++) {
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
	iconGa->initWithSpriteFrameName(isGa ? "ga_on.png" : "ga_off.png");
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
	lbCrestTime->getParent()->setVisible(false);
	lbCrestTime->pause();
	for (Sprite* sp : spSanSangs) {
		sp->setVisible(false);
	}
	for (Label* lb : lbSanSangs) {
		lb->setVisible(false);
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
			int stsize = vecStilts.size();
			Node* n1 = spDealCards[0]->getParent();
			Node* n2 = spDealCards[1]->getParent();
			for (Sprite* sp : spDealCards) {
				sp->stopAllActions();
				int i = atoi(sp->getName().c_str());
				if (i < 0 || i >= stsize) i = 0;
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
		state = CHOOSE_STILT;
	}

	chosenStilt = stilt - 1;
	int stsize = vecStilts.size();
	if (chosenStilt < 0 || chosenStilt >= stsize) {
		chosenStilt = 0;
	}
	vecStilts[chosenStilt]->setVisible(false);
	hostCard = Sprite::createWithSpriteFrameName("100.png");
	hostCard->setName("hostCard");
	hostCard->setScale(.7f);
	hostCard->setPosition(vecStilts[chosenStilt]->getPosition() + getScaleScenePosition(Vec2(winSize.width / 2, winSize.height / 2)));
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

	int stsize = vecStilts.size();
	if (chosenStiltHost < 0 || chosenStiltHost >= stsize || chosenStiltHost == chosenStilt) {
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
	MoveTo* move = MoveTo::create(.5f, vecStilts[chosenStiltHost]->getPosition() + getScaleScenePosition(Vec2(winSize.width / 2, winSize.height / 2)));
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
		//spChonCai->setVisible(false);
		lbChonCai->setVisible(false);
		lbChonCai->pause();
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
		//	MoveTo* move2 = MoveTo::create(.5f, handPos[m] - Vec2(winSize.width / 2, winSize.height / 2));
		//	vecStilts[k]->runAction(Sequence::create(delay2, move2, nullptr));
		//}

		if (chosenStiltHost == chosenHost + 1) {
			int j = chosenHost;
			for (int i = 0; i < 5; i++) {
				Node* n = vecStilts[(chosenStiltHost + i) % vecStilts.size()];
				if (!n->isVisible()) continue;
				DelayTime* delay2 = DelayTime::create(.1f);
				MoveTo* move2 = MoveTo::create(.5f, handPos[j] - Vec2(winSize.width / 2, winSize.height / 2));
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
					//MoveTo* move2 = MoveTo::create(.5f, handPos[i++] - Vec2(winSize.width / 2, winSize.height / 2));
					MoveTo* move2 = MoveTo::create(runTime, handPos[i++] - Vec2(winSize.width / 2, winSize.height / 2));
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
	int rcsize = runningCards.size();
	if (runningSpCard == NULL && rcsize > 0 && runningCards[index] != NULL) {
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
			int hcsize = spHandCards.size();
			for (int i = 0; i < hcsize; i++) {
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
		int hcsize = spHandCards.size();
		for (int i = hcsize - 1; i >= 0; i--) {
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

	int csize = cards.size();
	for (int i = 0; i < csize; i++) {
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
	state = ENDING;
	if (!btnXemNoc->isVisible() && !btnDongNoc->isVisible()) {
		gameSplash->setVisible(true);
		for (Sprite* sp : spCards) {
			if (sp->isVisible()) {
				sp->setVisible(false);
			}
		}
	}
	lbCrestTime->getParent()->setVisible(false);
	btnCrest->setVisible(false);
	tableCrest->setVisible(false);
	tableEndMatch->setVisible(true);
	progressTimer->setVisible(false);
	progressTimer->stopAllActions();
	lbCrestTime->pause();
	/*btnUp->setLocalZOrder(constant::ZORDER_POPUP);
	btnDown->setLocalZOrder(constant::ZORDER_POPUP);
	int i = 1;
	for (Node* n : vecMenuBtns) {
		n->setLocalZOrder(constant::ZORDER_POPUP - i++);
	}*/

	string strwin = vecUsers[userIndexs[data.UId]]->getPlayerName() + " " + Utils::getSingleton().getStringForKey("win");
	lbWinner->setString(strwin);

	string strcCrest = "";
	for (unsigned char c : data.CuocHo) {
		strcCrest += Utils::getSingleton().getStringForKey(string("cuoc_") + to_string((int)c)) + " ";
	}
	lbCrestWin->setString(strcCrest);

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
		lbNotedGa->setPosition(0, -140);
	} else {
		lbNoted->setString("");
		lbNotedGa->setPosition(lbNoted->getPosition());
	}

	if (data.MsgAnGa.length() > 0) {
		lbNotedGa->setString(data.MsgAnGa);
	} else {
		lbNotedGa->setString("");
	}

	int chsize = data.CuocHo.size();
	for (int i = 0; i < chsize; i++) {
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
	state = CREST;
	this->endMatchData = data;
	gameSplash->setVisible(true);
	lbCrestTime->getParent()->setVisible(true);
	lbCrestTime->setString("40");
	lbCrestTime->resume();
	lbCrestTime->getParent()->setPosition(btnXemNoc->getPosition() + getScaleSceneDistance(Vec2(170, 0)));
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
		btnCrest->setVisible(true);
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
	//spBatBaos[userIndexs[UiD]]->setVisible(true);
	lbBatBaos[userIndexs[UiD]]->setVisible(true);
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
	lbSanSangs[userIndexs2[UiD]]->setVisible(isReady);
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

	int usize = listUser.size();
	if (hasClickInvite) {
		hasClickInvite = false;
		showPopup(tableInvite);
		ui::ScrollView* scroll = (ui::ScrollView*)tableInvite->getChildByName("scroll");
		int sx = tableInvite->getChildByName("inside")->getContentSize().width;

		int y = usize * 60;
		if (y < scroll->getContentSize().height)
			y = scroll->getContentSize().height - 25;
		scroll->setInnerContainerSize(Size(sx, y + 25));

		bool isRealMoney = Utils::getSingleton().moneyType == 1;
		int childCount = scroll->getChildrenCount();
		for (int i = 0; i < usize; i++) {
			int posY = y - i * 60 - 5;
			int tag = i * 4;
			Label* lb1;
			Label* lb2;
			Sprite* sp;
			ui::Button* btn;
			bool isNewBtn;
			if (tag >= childCount) {
				lb1 = Label::createWithTTF("", "fonts/myriad.ttf", 30);
				lb1->setAnchorPoint(Vec2(0, .5f));
				lb1->setColor(Color3B::BLACK);
				lb1->setWidth(sx - winSize.height / 2);
				lb1->setHeight(30);
				lb1->setTag(tag);
				scroll->addChild(lb1);

				sp = Sprite::create();
				sp->setScale(.6f);
				sp->setTag(tag + 1);
				scroll->addChild(sp);

				lb2 = Label::createWithTTF("", "fonts/myriad.ttf", 30);
				lb2->setAnchorPoint(Vec2(0, .5f));
				lb2->setColor(Color3B::BLACK);
				lb2->setTag(tag + 2);
				scroll->addChild(lb2);

				btn = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
				btn->setTitleText(Utils::getSingleton().getStringForKey("moi"));
				btn->setTitleFontName("fonts/myriadb.ttf");
				btn->setTitleFontSize(40);
				btn->setTitleDeviation(Vec2(0, -5));
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
			sp->setPosition(sx - 360, posY + 5);
			lb2->setPosition(sx - 340, posY);
			btn->setPosition(Vec2(sx - 70, posY));

			lb1->setString(listUser[i].DisplayName.length() > 0 ? listUser[i].DisplayName : listUser[i].Name);
			lb2->setString(Utils::getSingleton().formatMoneyWithComma(isRealMoney ? listUser[i].MoneyReal : listUser[i].MoneyFree));
			lb2->setColor(Color3B::BLACK);// isRealMoney ? Color3B::YELLOW : Color3B(0, 255, 255));
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

		int count = usize * 4;
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
	int usize = vecUsers.size();
	int psize = data.Players.size();
	for (int i = 0; i < 4; i++) {
		int index = -1;
		for (PlayerData player : data.Players) {
			int scsize = player.SingleCards.size();
			int pcsize = player.PairCards.size();
			if (player.Index == ((myServerSlot < 0 ? 0 : myServerSlot) + i) % usize) {
				index = i - num;
				if (psize == 2 && (index == 1 || index == 3)) {
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
					spChuPhong->setVisible(true);
					spChuPhong->setPosition(vecUserPos[index] + getScaleSceneDistance(Vec2(-40, 35)));
				}

				int index2 = index * 2;
				float scale = 1;

				if (scsize > maxTableCardNumb[index2]) {
					scale = ((float)maxTableCardNumb[index2]) / scsize;
				}
				for (int j = 0; j < scsize; j++) {
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
					if (j == scsize - 1) {
						runningCards[index] = spCard;
					}
				}

				index2 = index * 2 + 1;
				scale = 1;

				if (pcsize > maxTableCardNumb[index2]) {
					scale = ((float)maxTableCardNumb[index2] - 1) / (pcsize - 1);
				}
				for (vector<char> v : player.PairCards) {
					Vec2 pos;
					int vsize = v.size();
					if (vsize == 2) {
						pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale;
					} else {
						pos = tableCardPos[index2] + tableCardNumb[index2] * tableCardDistance[index2] * scale + Vec2(0, 5);
					}
					for (int i = 0; i < vsize; i++) {
						Sprite* spCard = getCardSprite(v[i]);
						spCard->setLocalZOrder(constant::GAME_ZORDER_TABLE_CARD + (tableCardNumb[index2] + 1) * 4 + i);
						spCard->setTag(constant::TAG_CARD_TABLE + index2);
						spCard->setName(to_string((int)std::abs(v[i])));
						spCard->setAnchorPoint(Vec2(.5f, .5f));
						spCard->setPosition(pos - (vsize == 2 ? Vec2(0, 40) : Vec2(0, 15)) * i);
						spCard->setColor(v[i] > 0 ? Color3B::WHITE : Color3B(200, 200, 255));
						spCard->setRotation(0);
						spCard->setScale(cardScaleTable);
					}
					tableCardNumb[index2] ++;
				}
				break;
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
		int psize = dealPos.size();
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < psize; j++) {
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
	int lsize = list.size();
	for (int i = 0; i < lsize; i++) {
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
		lbTitleWaitPlayer = Label::createWithTTF(Utils::getSingleton().getStringForKey("giai_dau_se_bat_dau_sau"), "fonts/myriad.ttf", 40);
		lbTitleWaitPlayer->setPosition(winSize.width / 2, winSize.height / 2 + 50);
		lbTitleWaitPlayer->setName("lbTitleWaitPlayer");
		mLayer->addChild(lbTitleWaitPlayer, constant::GAME_ZORDER_BUTTON);
		autoScaleNode(lbTitleWaitPlayer);

		lbTimeWaitPlayer = Label::createWithTTF(timeWaitString, "fonts/myriadb.ttf", 50);
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
	//spChonCai->setVisible(false);
	lbChonCai->pause();
	lbChonCai->setVisible(false);
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
	tableChat->setPosition(winSize.width / 2, 200 * scaleScene.x);
	tableChat->setVisible(false);
	mLayer->addChild(tableChat, constant::ZORDER_POPUP);

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("white.png");
	bg->setContentSize(Size(1500, 400));
	bg->setColor(Color3B::BLACK);
	//bg->setOpacity(150);
	tableChat->addChild(bg);

	Sprite* bgInput = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgInput->setPosition(Vec2(-110, 150));
	bgInput->setScaleX(.9f);
	tableChat->addChild(bgInput);

	ui::EditBox* input = ui::EditBox::create(Size(750, 65), "empty.png", ui::Widget::TextureResType::PLIST);
	input->setPosition(bgInput->getPosition());
	input->setMaxLength(100);
	input->setFontName("Arial");
	input->setFontSize(30);
	input->setFontColor(Color3B::WHITE);
	input->setPlaceholderFont("Arial", 30);
	input->setPlaceholderFontColor(Color3B(200, 200, 200));
	input->setPlaceHolder(Utils::getSingleton().getStringForKey("type_here_to_chat").c_str());
	input->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	input->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	input->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	input->setDelegate(this);
	input->setName("tfinput");
	tableChat->addChild(input);

	ui::Button* btnSend = ui::Button::create("btn_small.png", "", "", ui::Widget::TextureResType::PLIST);
	btnSend->setTitleText(Utils::getSingleton().getStringForKey("send"));
	btnSend->setTitleFontName("fonts/myriadb.ttf");
	btnSend->setTitleFontSize(40);
	btnSend->setTitleDeviation(Vec2(0, -5));
	btnSend->setPosition(Vec2(410, 150));
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

	auto vsize = Director::getInstance()->getVisibleSize();
	float xscale = winSize.height / vsize.height;
	float yscale = winSize.width / vsize.width;
	ui::Button* btnClose = ui::Button::create("btn_dong.png", "", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2((winSize.width / 2 - 35) * xscale, 170));
	//btnClose->setScale(.8f);
	addTouchEventListener(btnClose, [=]() {
		input->setText("");
		hidePopup(tableChat);
	});
	tableChat->addChild(btnClose);

	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-winSize.width / 2, -200));
	scroll->setContentSize(Size(winSize.width, 300));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	tableChat->addChild(scroll);

	ValueMap plist = FileUtils::getInstance()->getValueMapFromFile("lang/chat.xml");
	int i = 0;
	int height = (plist.size() / 2 + 1) * 80 + 20;
	scroll->setInnerContainerSize(Size(winSize.width, height));
	for (auto iter = plist.begin(); iter != plist.end(); iter++) {
		ui::Button* btn = ui::Button::create("bg_message.png", "", "", ui::Widget::TextureResType::PLIST);
		//btn->setScale9Enabled(true);
		//btn->setContentSize(Size(500, 64));
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleText(iter->second.asString());
		btn->setTitleFontSize(25);
		btn->setTitleColor(Color3B::BLACK);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setPosition(Vec2(winSize.width / 4 + (i % 2) * winSize.width / 2, height - 50 - (i / 2) * 80));
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
	tableCrest->setPosition(winSize.width / 2, winSize.height - 160);
	tableCrest->setVisible(false);
	mLayer->addChild(tableCrest, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(tableCrest);

	/*ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setContentSize(Size(1040, 360));
	bg->setInsetLeft(300);
	bg->setInsetRight(30);
	bg->setInsetTop(30);
	bg->setInsetBottom(320);
	tableCrest->addChild(bg);*/

	Sprite* bg = Sprite::create("imgs/popup_bg1.png");
	bg->setPosition(0, 35);
	bg->setScale(.9f);
	tableCrest->addChild(bg);

	Sprite* bgChosen = Sprite::createWithSpriteFrameName("bg_tabs.png");
	bgChosen->setPosition(0, 110);
	bgChosen->setScaleX(.8f);
	tableCrest->addChild(bgChosen);

	lbChonCuoc = Label::createWithTTF("", "fonts/myriadb.ttf", 40);
	lbChonCuoc->setPosition(bgChosen->getPositionX(), bgChosen->getPositionY() - 5);
	lbChonCuoc->setColor(Color3B::WHITE);
	lbChonCuoc->setAlignment(TextHAlignment::CENTER);
	lbChonCuoc->setWidth(900);
	lbChonCuoc->setHeight(40);
	tableCrest->addChild(lbChonCuoc);

	int idsize = ids.size();
	Size size1 = Size(1000, 235);
	Size size2 = Size(1000, ((idsize - 2) / 4 + 2) * 46);
	ui::ScrollView* scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setScrollBarColor(Color3B::WHITE);
	scroll->setScrollBarOpacity(100);
	scroll->setScrollBarPositionFromCorner(Vec2(5, 10));
	scroll->setScrollBarAutoHideEnabled(false);
	scroll->setBounceEnabled(true);
	scroll->setContentSize(size1);
	scroll->setName("scroll");
	scroll->setInnerContainerSize(size2);
	scroll->setPosition(Vec2(-size1.width / 2, -size1.height / 2 - 60));
	tableCrest->addChild(scroll);

	int x = 160;
	int y = size2.height - 20;
	for (int i = 0; i < idsize; i++) {
		string name = String::createWithFormat("cuoc_%d", ids[i])->getCString();
		ui::Button* btn = ui::Button::create("", "");
		btn->setTitleText(Utils::getSingleton().getStringForKey(name));
		btn->setTitleFontName("fonts/myriadb.ttf");
		btn->setTitleColor(Color3B::BLACK);
		btn->setTitleFontSize(30);
		btn->setTitleDeviation(Vec2(0, -5));
		btn->setPosition(Vec2(x + (i % 4) * 230, y - (i / 4) * 46));
		btn->setContentSize(Size(220, 50));
		btn->setScale9Enabled(true);
		btn->setBright(false);
		btn->setTag(ids[i]);
		scroll->addChild(btn);
		vecCrests.push_back(btn);

		if (i == idsize -1) {
			btn->setContentSize(Size(430, 40));
			btn->setPosition(Vec2(x + 75, y - (i / 4) * 46));
		}

		addTouchEventListener(btn, [=]() {
			int csize = chosenCuocs.size();
			int i = btn->getTag();
			if (i == 33) {
				if (csize == 1 && chosenCuocs[0] == 33) {
					chosenCuocs.clear();
					chosenCuocNumbs.clear();
					vecCrests[0]->setTitleColor(Color3B::BLACK);
					lbChonCuoc->setString("");
				}

				int crsize = vecCrests.size();
				for (int j = 1; j < crsize; j++) {
					vecCrests[j]->setTitleColor(Color3B::BLACK);
				}
				vecCrests[0]->setTitleColor(Color3B::RED);
				chosenCuocs.clear();
				chosenCuocNumbs.clear();
				chosenCuocs.push_back(33);
				chosenCuocNumbs.push_back(0);
				lbChonCuoc->setString(Utils::getSingleton().getStringForKey("cuoc_33"));
			} else {
				if (csize == 1 && chosenCuocs[0] == 33) {
					chosenCuocs.clear();
					chosenCuocNumbs.clear();
					vecCrests[0]->setTitleColor(Color3B::BLACK);
				}

				int index = 0;
				while (index < csize && chosenCuocs[index] != i) index++;
				if (index == csize) {
					chosenCuocs.push_back(i);
					chosenCuocNumbs.push_back(0);
					btn->setTitleColor(Color3B::RED);
				} else {
					chosenCuocNumbs[index] ++;
					if (chosenCuocNumbs[index] > maxChosenCuocs[i]) {
						chosenCuocs.erase(chosenCuocs.begin() + index);
						chosenCuocNumbs.erase(chosenCuocNumbs.begin() + index);
						btn->setTitleColor(Color3B::BLACK);
					}
				}

				string strcuoc = "";
				csize = chosenCuocs.size();
				for (int j = 0; j < csize; j++) {
					if (strcuoc.length() > 0) {
						strcuoc += ", ";
					}
					strcuoc += Utils::getSingleton().getStringForKey("cuoc_" + to_string(chosenCuocs[j] + chosenCuocNumbs[j]));
				}
				lbChonCuoc->setString(strcuoc);
			}
		});
	}

	btnCrest = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnCrest->setTitleText(Utils::getSingleton().getStringForKey("xuong"));
	btnCrest->setTitleFontName("fonts/myriadb.ttf");
	btnCrest->setTitleFontSize(40);
	btnCrest->setTitleDeviation(Vec2(0, -5));
	btnCrest->setPosition(btnXemNoc->getPosition() + getScaleSceneDistance(Vec2(340, 0)));
	btnCrest->setVisible(false);
	addTouchEventListener(btnCrest, [=]() {
		vector<unsigned char> crestIds;
		int csize = chosenCuocs.size();
		for (auto i = 0; i < csize;i++) {
			crestIds.push_back(chosenCuocs[i] + chosenCuocNumbs[i]);
		}
		lbChonCuoc->setString("");
		tableCrest->setVisible(false);
		btnCrest->setVisible(false);
		lbCrestTime->getParent()->setVisible(false);
		lbCrestTime->pause();
		SFSRequest::getSingleton().RequestGameWinCrest(crestIds);
		for (ui::Button* btn : vecCrests) {
			btn->setTitleColor(Color3B::BLACK);
		}
	});
	mLayer->addChild(btnCrest, constant::GAME_ZORDER_SPLASH + 2);
	autoScaleNode(btnCrest);
}

void GameScene::initEndMatchTable()
{
	tableEndMatch = Node::create();
	tableEndMatch->setPosition(winSize.width / 2, winSize.height - 200);
	tableEndMatch->setVisible(false);
	mLayer->addChild(tableEndMatch, constant::GAME_ZORDER_SPLASH + 1);
	autoScaleNode(tableEndMatch);

	Sprite* bg = Sprite::createWithSpriteFrameName("bg_detail.png");
	//bg->setScaleY(.85f);
	//bg->setFlipY(true);
	tableEndMatch->addChild(bg);

	lbWinner = Label::createWithTTF("Buzzdesign U", "fonts/myriadb.ttf", 35);
	lbWinner->setPosition(0, 110);
	lbWinner->setColor(Color3B::BLACK);
	lbWinner->setAlignment(TextHAlignment::CENTER);
	lbWinner->setWidth(500);
	tableEndMatch->addChild(lbWinner);

	lbCrestWin = Label::createWithTTF("Xuong", "fonts/myriadb.ttf", 35);
	lbCrestWin->setPosition(0, 60);
	lbCrestWin->setColor(Color3B(233, 0, 0));
	lbCrestWin->setAlignment(TextHAlignment::CENTER);
	lbCrestWin->setWidth(550);
	tableEndMatch->addChild(lbCrestWin);

	lbDiem = Label::createWithTTF("-11 Diem", "fonts/myriadb.ttf", 35);
	lbDiem->setPosition(0, 5);
	lbDiem->setColor(Color3B(0, 144, 10));
	tableEndMatch->addChild(lbDiem);

	lbCuocSai = Label::createWithTTF("Cuoc sai: Hoa roi cua phat", "fonts/myriad.ttf", 35);
	lbCuocSai->setHorizontalAlignment(TextHAlignment::CENTER);
	lbCuocSai->setPosition(0, -45);
	lbCuocSai->setColor(Color3B::BLACK);
	lbCuocSai->setWidth(600);
	tableEndMatch->addChild(lbCuocSai);

	lbNoted = Label::createWithTTF("Reno bi den lang. Loi u duoi 4 diem", "fonts/myriad.ttf", 35);
	lbNoted->setHorizontalAlignment(TextHAlignment::CENTER);
	lbNoted->setLineSpacing(-15);
	lbNoted->setPosition(0, -95);
	lbNoted->setColor(Color3B::BLACK);
	lbNoted->setWidth(550);
	tableEndMatch->addChild(lbNoted);

	lbNotedGa = Label::createWithTTF("Stormus an ga", "fonts/myriad.ttf", 35);
	lbNotedGa->setHorizontalAlignment(TextHAlignment::CENTER);
	lbNotedGa->setPosition(0, -140);
	lbNotedGa->setColor(Color3B::BLACK);
	lbNotedGa->setWidth(550);
	tableEndMatch->addChild(lbNotedGa);
}

void GameScene::initInviteTable()
{
	tableInvite = Node::create();
	tableInvite->setPosition(winSize.width / 2, winSize.height / 2);
	tableInvite->setVisible(false);
	mLayer->addChild(tableInvite, constant::ZORDER_POPUP + 1);
	autoScaleNode(tableInvite);

	/*Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setScale(1.1f, 1.5f);
	tableInvite->addChild(bg);*/

	ui::Scale9Sprite* bg = ui::Scale9Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setInsetBottom(160);
	bg->setInsetTop(160);
	bg->setInsetLeft(169);
	bg->setInsetRight(128);
	bg->setContentSize(Size(750, 550));
	tableInvite->addChild(bg);

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
	scroll->setPosition(Vec2(-(sx - 20)/2, -220));
	scroll->setContentSize(Size(sx - 20, 430));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	tableInvite->addChild(scroll);

	ui::Button* btnDong = ui::Button::create("btn_dong.png", "", "", ui::Widget::TextureResType::PLIST);
	btnDong->setPosition(Vec2(bg->getContentSize().width / 2 - 55, bg->getContentSize().height / 2 - 35));
	//btnDong->setScale(.8f);
	addTouchEventListener(btnDong, [=]() {
		hidePopup(tableInvite);
	});
	tableInvite->addChild(btnDong);
}

void GameScene::initSettingsPopup()
{
	popupSettings = Node::create();
	popupSettings->setPosition(winSize.width / 2, winSize.height / 2);
	popupSettings->setVisible(false);
	mLayer->addChild(popupSettings, constant::ZORDER_POPUP);
	autoScaleNode(popupSettings);

	Vec2 scale = Vec2(1.2f, 1.2f);
	Sprite* bg = Sprite::createWithSpriteFrameName("popup_bg.png");
	bg->setScale(scale.x, scale.y);
	popupSettings->addChild(bg);

	Sprite* title = Sprite::createWithSpriteFrameName("title_caidat.png");
	title->setPosition(0, (bg->getContentSize().height / 2 - 35) * scale.y);
	title->setScale(scale.x, scale.y);
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
		checkbox->loadTextureBackGround("unchecked.png", ui::Widget::TextureResType::PLIST);
		checkbox->loadTextureFrontCross("checked.png", ui::Widget::TextureResType::PLIST);
		checkbox->setPosition(vecPos[i]);
		checkbox->setSelected(false);
		checkbox->setTag(i);
		popupSettings->addChild(checkbox);
		cbs.push_back(checkbox);

		Label* lb = Label::createWithTTF("", "fonts/myriadb.ttf", 35);
		lb->setPosition(vecPos[i] + Vec2(40, -5));
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

	cbs[3]->setSelected(isAutoReady);
	cbs[4]->setSelected(UserDefault::getInstance()->getBoolForKey(constant::KEY_SOUND.c_str()));

	lbs[0]->setString(Utils::getSingleton().getStringForKey("win") + " 4-11");
	lbs[1]->setString(Utils::getSingleton().getStringForKey("quick"));
	lbs[2]->setString(Utils::getSingleton().getStringForKey("slow"));
	lbs[3]->setString(Utils::getSingleton().getStringForKey("auto_ready"));
	lbs[4]->setString(Utils::getSingleton().getStringForKey("sound"));

	ui::Button* btnOK = ui::Button::create("btn.png", "", "", ui::Widget::TextureResType::PLIST);
	btnOK->setTitleText(Utils::getSingleton().getStringForKey("xac_nhan"));
	btnOK->setTitleFontName("fonts/myriadb.ttf");
	btnOK->setTitleFontSize(40);
	btnOK->setTitleDeviation(Vec2(0, -5));
	btnOK->setPosition(Vec2(0, -210));
	btnOK->setScale(scale.x, scale.y);
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

	ui::Button* btnClose = ui::Button::create("btn_dong.png", "", "", ui::Widget::TextureResType::PLIST);
	btnClose->setPosition(Vec2((bg->getContentSize().width / 2 - 55) * scale.x, (bg->getContentSize().height / 2 - 35) * scale.y));
	btnClose->setScale(scale.x, scale.y);
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
	//tableInfo->setPosition(Vec2(0, winSize.height) + getScaleSceneDistance(Vec2(105, -50)));
	tableInfo->setPosition(Vec2(0, winSize.height) + getScaleSceneDistance(Vec2(195, -50)));
	mLayer->addChild(tableInfo, constant::GAME_ZORDER_BUTTON);
	autoScaleNode(tableInfo);

	/*Sprite* bg = Sprite::createWithSpriteFrameName("bg_table_info.png");
	bg->setScaleX(.85f);
	tableInfo->addChild(bg);*/

	Label* lbTableName = Label::createWithTTF("", "fonts/myriad.ttf", 25);
	lbTableName->setPosition(-90, isTourGame ? 25 : 15);
	lbTableName->setAnchorPoint(Vec2(0, .5f));
	lbTableName->setName("lbname");
	tableInfo->addChild(lbTableName);

	Label* lbTableBet = Label::createWithTTF("", "fonts/myriad.ttf", 25);
	lbTableBet->setPosition(lbTableName->getPosition() + Vec2(100, 0));
	lbTableBet->setAnchorPoint(Vec2(0, .5f));
	lbTableBet->setColor(Color3B::YELLOW);
	lbTableBet->setName("lbbet");
	lbTableBet->setVisible(!isTourGame);
	tableInfo->addChild(lbTableBet);

	Sprite* icMoney = Sprite::createWithSpriteFrameName("icon_gold.png");
	icMoney->setPosition(lbTableBet->getPosition() - Vec2(20, -3));
	icMoney->setScale(.5f);
	icMoney->setName("icmoney");
	icMoney->setVisible(!isTourGame);
	tableInfo->addChild(icMoney);

	Label* lbType = Label::createWithTTF("", "fonts/myriad.ttf", 25);
	lbType->setPosition(lbTableName->getPosition() + Vec2(0, isTourGame ? -25 : -30));
	lbType->setAnchorPoint(Vec2(0, .5f));
	lbType->setName("lbtype");
	tableInfo->addChild(lbType);

	long maxMatch = Utils::getSingleton().tourInfo.MaxMatch;
	Label* lbMatch = Label::create(Utils::getSingleton().getStringForKey("van") + ": 0/ " + to_string(maxMatch), "fonts/myriad.ttf", 25);
	lbMatch->setPosition(-25, 25);
	lbMatch->setAnchorPoint(Vec2(0, .5f));
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

		Label* lbTourTime = Label::create(timestr, "fonts/myriad.ttf", 25);
		lbTourTime->setPosition(-90, -25);
		lbTourTime->setAnchorPoint(Vec2(0, .5f));
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
	cofferSplash->setPosition(winSize.width / 2, winSize.height / 2);
	cofferSplash->setColor(Color3B::BLACK);
	cofferSplash->setOpacity(100);
	cofferSplash->setVisible(false);
	mLayer->addChild(cofferSplash, constant::ZORDER_COFFER);

	cofferEffect = Node::create();
	cofferEffect->setPosition(winSize.width / 2, winSize.height / 2 + 30);
	cofferEffect->setVisible(false);
	mLayer->addChild(cofferEffect, constant::ZORDER_COFFER + 1);

	Sprite* spLight = Sprite::create();
	spLight->setName("splight");
	cofferEffect->addChild(spLight);
	spLight->runAction(RepeatForever::create(RotateBy::create(1, 90)));
	spLight->pause();

	Sprite* spCoffer = Sprite::create();
	cofferEffect->addChild(spCoffer);

	Label* lbMoney = Label::createWithBMFont("fonts/sonohu.fnt", "123.456");
	lbMoney->setColor(Color3B::YELLOW);
	lbMoney->setPosition(0, -210);
	lbMoney->setName("lbmoney");
	cofferEffect->addChild(lbMoney);

	Label* lbName = Label::createWithTTF("Stormus", "fonts/myriadb.ttf", 60);
	lbName->enableOutline(Color4B::BLACK, 2);
	lbName->setColor(Color3B::WHITE);
	lbName->setPosition(0, lbMoney->getPositionY() - 70);
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
			int csize = spHandCards.size();
			for (auto i = 0; i < csize; i++) {
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
