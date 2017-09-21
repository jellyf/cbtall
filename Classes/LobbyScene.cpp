#include "LobbyScene.h"
#include "SFSRequest.h"
#include "Constant.h"
#include "Utils.h"
#include "EventHandler.h"
#include "md5.h"

using namespace cocos2d;
using namespace std;

void LobbyScene::onInit()
{
	setTag(constant::SCENE_LOBBY);
	BaseScene::onInit();

	playerPos.push_back(Vec2(97, 125));
	playerPos.push_back(Vec2(200, 124));
	playerPos.push_back(Vec2(88, 99));
	playerPos.push_back(Vec2(207, 97));

	initHeaderWithInfos();

	isSolo = Utils::getSingleton().isSoloGame();
	isTour = Utils::getSingleton().isTourGame();
	string zone = Utils::getSingleton().getCurrentZoneName();
	if (zone.length() > 0) {
		int index = zone.find_last_of("Q");
		if (index >= 0 && index < zone.length()) {
			zone = zone.substr(0, index);
		}
	} else {
		zone = "VuongPhu";
	}
	if (isTour || zone.compare("Authen") == 0) zone = "VuongPhu";
	if (zone.compare("SoLoXu") == 0) zone = "SoLo";

	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGB565);
	Texture2D* bgTexture = TextureCache::getInstance()->addImage("imgs/bg" + zone + ".jpg");
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);

	Sprite* bg = Sprite::createWithTexture(bgTexture);
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	addChild(bg);

	if (isTour) {
		if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) {
			string host = Utils::getSingleton().textureHost;
			Utils::getSingleton().LoadTextureFromURL(host + "text_loidaichien.png", [=](Texture2D* texture) {
				Sprite* spName = Sprite::createWithTexture(texture);
				spName->setAnchorPoint(Vec2(0, 1));
				spName->setPosition(0, 620);
				spName->setScale(.9f);
				mLayer->addChild(spName);
				autoScaleNode(spName);
			});
		} else {
			Sprite* spName = Sprite::create("text_loidaichien.png");
			spName->setAnchorPoint(Vec2(0, 1));
			spName->setPosition(0, 620);
			spName->setScale(.9f);
			mLayer->addChild(spName);
			autoScaleNode(spName);
		}

		initTourView();
	} else {
		Sprite* spName = Sprite::createWithSpriteFrameName(isSolo ? "SoLo.png" : zone + ".png");
		spName->setPosition(140, 560);
		mLayer->addChild(spName);
	}

	scrollListRoom = ui::ScrollView::create();
	scrollListRoom->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollListRoom->setBounceEnabled(true);
	scrollListRoom->setPosition(Vec2(15, 0));
	scrollListRoom->setContentSize(Size(240, 520));
	scrollListRoom->setScrollBarEnabled(false);
	scrollListRoom->setVisible(!isTour);
	mLayer->addChild(scrollListRoom);

	scrollListTable = ui::ScrollView::create();
	scrollListTable->setDirection(ui::ScrollView::Direction::VERTICAL);
	scrollListTable->setBounceEnabled(true);
	scrollListTable->setPosition(Vec2(300 + (isTour ? 210 : 0), 20));
	scrollListTable->setContentSize(Size(920 - (isTour ? 210 : 0), 515));
	scrollListTable->setScrollBarEnabled(false);
	mLayer->addChild(scrollListTable);

	string strGold = Utils::getSingleton().formatMoneyWithComma(Utils::getSingleton().userDataMe.MoneyReal);
	string strSilver = Utils::getSingleton().formatMoneyWithComma(Utils::getSingleton().userDataMe.MoneyFree);
	string strId = String::createWithFormat("ID: %ld", Utils::getSingleton().userDataMe.UserID)->getCString();
	string strLevel = String::createWithFormat((Utils::getSingleton().getStringForKey("level") + ": %d").c_str(), Utils::getSingleton().userDataMe.Level)->getCString();
	lbName->setString(Utils::getSingleton().userDataMe.DisplayName);
	lbGold->setString(strGold);
	lbSilver->setString(strSilver);
	lbId->setString(strId);
	lbLevel->setString(strLevel);

	if (!isTour) {
		initEventView(Vec2(0, winSize.height - 125), Size(winSize.width, 40));
	}

	if (Utils::getSingleton().lobbyListRoomType.ListRoomType.size() > 0) {
		onRoomTypeDataResponse(Utils::getSingleton().lobbyListRoomType);
	}
	if (Utils::getSingleton().lobbyListTable.Size > 0) {
		onTableDataResponse(Utils::getSingleton().lobbyListTable);
	}
	if (Utils::getSingleton().isRunningEvent) {
		runEventView(Utils::getSingleton().events, Utils::getSingleton().currentEventPosX);
	}
	if (Utils::getSingleton().tableReconnectData.Room.length() > 0) {
		onTableReconnectDataResponse(Utils::getSingleton().tableReconnectData);
		Utils::getSingleton().tableReconnectData.Room = "";
	}

	if (isTour) {
		/*string room = Utils::getSingleton().tourRoom;
		if (room.length() > 0) {
			onTourRoomToJoin(room);
		} else {
			delayFunction(this, 1, [=]() {
				SFSRequest::getSingleton().RequestListTourPlayers();
				SFSRequest::getSingleton().RequestTopTourPlayers();
			});
		}*/
		SFSRequest::getSingleton().RequestListTourPlayers();
		SFSRequest::getSingleton().RequestTopTourPlayers();
	}

	//runConnectionKeeper();

	/*vector<TourPlayer> players;
	for (int i = 0; i < 16; i++) {
		TourPlayer p;
		p.Id = i;
		p.Name = "Stormus_" + to_string(i);
		p.Money = 10000;
		p.Matches = rand() % 10;
		players.push_back(p);
	}
	onListTourPlayersResponse(players);

	vector<TourRoom> rooms;
	for (int i = 0; i < 4; i++) {
		TourRoom r;
		r.Name = "Ban " + to_string(i + 1);
		int numb = rand() % 3 + 2;
		for (int j = 0; j < numb; j++) {
			TourPlayer p;
			p.Id = i * 10 + j;
			p.Name = "Stormus_" + to_string(p.Id);
			p.Money = rand() % 50;
			p.Matches = rand() % 10;
			r.Players.push_back(p);
		}
		rooms.push_back(r);
	}
	onTopTourPlayersResponse(rooms);*/
}

void LobbyScene::registerEventListenner()
{
	BaseScene::registerEventListenner();
	EventHandler::getSingleton().onConfigZoneReceived = bind(&LobbyScene::onConfigZoneReceived, this);
	EventHandler::getSingleton().onLobbyTableSFSResponse = bind(&LobbyScene::onTableDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onLobbyRoomTypeSFSResponse = bind(&LobbyScene::onRoomTypeDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onLobbyInviteDataSFSResponse = bind(&LobbyScene::onInviteDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onTableReconnectDataSFSResponse = bind(&LobbyScene::onTableReconnectDataResponse, this, placeholders::_1);
	EventHandler::getSingleton().onPurchaseSuccess = bind(&LobbyScene::onPurchaseSuccess, this, placeholders::_1);
	EventHandler::getSingleton().onListTourPlayersSFSResponse = bind(&LobbyScene::onListTourPlayersResponse, this, placeholders::_1);
	EventHandler::getSingleton().onTopTourPlayersSFSResponse = bind(&LobbyScene::onTopTourPlayersResponse, this, placeholders::_1);
	EventHandler::getSingleton().onTourNewRoundSFSResponse = bind(&LobbyScene::onTourNewRound, this, placeholders::_1);
}

void LobbyScene::unregisterEventListenner()
{
	BaseScene::unregisterEventListenner();
	EventHandler::getSingleton().onConfigZoneReceived = NULL;
	EventHandler::getSingleton().onLobbyTableSFSResponse = NULL;
	EventHandler::getSingleton().onLobbyRoomTypeSFSResponse = NULL;
	EventHandler::getSingleton().onLobbyInviteDataSFSResponse = NULL;
	EventHandler::getSingleton().onTableReconnectDataSFSResponse = NULL;
	EventHandler::getSingleton().onPurchaseSuccess = NULL;
	EventHandler::getSingleton().onListTourPlayersSFSResponse = NULL;
	EventHandler::getSingleton().onTopTourPlayersSFSResponse = NULL;
	EventHandler::getSingleton().onTourNewRoundSFSResponse = NULL;
}

bool LobbyScene::onKeyBack()
{
	bool canBack = BaseScene::onKeyBack();
	if (canBack) {
		onBackScene();
		return false;
	}
	return canBack;
}

void LobbyScene::onConnected()
{
	BaseScene::onConnected();
	if (isBackToMain) {
		if (Utils::getSingleton().loginType == constant::LOGIN_FACEBOOK) {
			SFSRequest::getSingleton().LoginZone(Utils::getSingleton().username, "", Utils::getSingleton().gameConfig.zone);
		} else {
			SFSRequest::getSingleton().LoginZone("", "g", Utils::getSingleton().gameConfig.zone);
		}
		return;
	} else if (isChangeMoney) {
		Utils::getSingleton().currentZoneName = tmpZoneName;
		SFSRequest::getSingleton().LoginZone(Utils::getSingleton().username, Utils::getSingleton().password, tmpZoneName);
		tmpZoneName = "";
	}
}

bool LobbyScene::onConnectionLost(std::string reason)
{
	if (BaseScene::onConnectionLost(reason)) return true;
	if (isBackToMain) {
		SFSRequest::getSingleton().Connect();
	} else if (isChangeMoney && tmpZoneName.length() > 0) {
		int index = tmpZoneName.find_last_of("Q");
		if (index > 0 && index < tmpZoneName.length()) {
			tmpZoneName = tmpZoneName.substr(0, index);
			if (tmpZoneName.compare("SoLo") == 0) {
				tmpZoneName += "Xu";
			}
		} else {
			if (tmpZoneName.compare("SoLoXu") == 0) {
				tmpZoneName = "SoLoQuan";
			}else tmpZoneName += "Quan";
		}

		vector<vector<ZoneData>> zones = Utils::getSingleton().zones;
		for (ZoneData z : zones[currentMoneyType]) {
			if (z.ZoneName.compare(tmpZoneName) == 0) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				SFSRequest::getSingleton().Connect(z.ZoneIpIos, z.ZonePort);
#else
				SFSRequest::getSingleton().Connect(z.ZoneIp, z.ZonePort);
#endif
				break;
			}
		}
	} else {
		handleClientDisconnectionReason(reason);
	}
	return true;
}

void LobbyScene::onLoginZone()
{
	BaseScene::onLoginZone();
	if (isBackToMain) {
		SFSRequest::getSingleton().RequestLogin(Utils::getSingleton().username, Utils::getSingleton().password);
	}
	Utils::getSingleton().moneyType = currentMoneyType;
}

void LobbyScene::onConfigZoneReceived()
{
	if (isBackToMain) {
		Utils::getSingleton().goToMainScene();
		return;
	}
}

void LobbyScene::onLoginZoneError(short int code, std::string msg)
{
	BaseScene::onLoginZoneError(code, msg);
	if (isChangeMoney) {
		hideWaiting();
		if (msg.length() == 0) return;
		showPopupNotice(msg, [=]() {});
		if (code == 28) setMoneyType(1 - currentMoneyType);
	}
}

bool LobbyScene::onErrorResponse(unsigned char code, std::string msg)
{
	if(BaseScene::onErrorResponse(code, msg)) return true;
	if (isTour && code == 36) {
		showPopupNotice(msg, [=]() {
			//SFSRequest::getSingleton().RequestJoinRoom(prepareTourRoom);
		});
		return true;
	}
	if (code != 48) hideWaiting();
	if (isChangeMoney && code == 0) {
		setMoneyType(1 - currentMoneyType);
		onChangeMoneyType(1 - currentMoneyType);
		return true;
	}
	if (msg.length() == 0) return false;
	showPopupNotice(msg, [=]() {});
	return true;
}

cocos2d::Node* LobbyScene::getPopupInvitation()
{
	for (Node* n : vecPopupInvitations) {
		if (!n->isVisible()) {
			ui::Button* btnClose = (ui::Button*)n->getChildByName("btnclose");
			btnClose->setTouchEnabled(true);
			return n;
			break;
		}
	}
	Node* popupInvitaion = createPopupNotice();
	vecPopupInvitations.push_back(popupInvitaion);

	/*ui::CheckBox* checkbox = ui::CheckBox::create();
	checkbox->loadTextureBackGround("unchecked.png", ui::Widget::TextureResType::PLIST);
	checkbox->loadTextureFrontCross("checked.png", ui::Widget::TextureResType::PLIST);
	checkbox->setPosition(Vec2(-200, -90));
	checkbox->setSelected(false);
	popupInvitaion->addChild(checkbox);

	checkbox->addEventListener([=](Ref* ref, ui::CheckBox::EventType type) {
		bool isSelected = type == ui::CheckBox::EventType::SELECTED;
		Utils::getSingleton().IgnoreInvitation = isSelected;
		UserDefault::getInstance()->setBoolForKey(constant::KEY_INVITATION.c_str(), isSelected);
	});

	Label* lb = Label::createWithTTF(Utils::getSingleton().getStringForKey("reject_all_invitation"), "fonts/myriadb.ttf", 35);
	lb->setPosition(checkbox->getPosition() + Vec2(40, -5));
	lb->setAnchorPoint(Vec2(0, .5f));
	lb->setColor(Color3B::BLACK);
	popupInvitaion->addChild(lb);*/

	return popupInvitaion;
}

void LobbyScene::initTourView()
{
	nodeTour = Node::create();
	nodeTour->setPosition(240, 235);
	mLayer->addChild(nodeTour);
	autoScaleNode(nodeTour);

	Sprite *bg = Sprite::create();
	bg->setName("spbg");
	nodeTour->addChild(bg);
	Size bgsize = Size(448, 457);

	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) {
		string host = Utils::getSingleton().textureHost;
		Utils::getSingleton().LoadTextureFromURL(host + "khung.png", [=](Texture2D* texture) {
			Sprite* bg = (Sprite*)nodeTour->getChildByName("spbg");
			bg->initWithTexture(texture);
		});
	} else {
		bg->initWithFile("khung.png");
	}

	ui::ScrollView *scroll = ui::ScrollView::create();
	scroll->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll->setBounceEnabled(true);
	scroll->setPosition(Vec2(-bgsize.width / 2 + 10, -bgsize.height / 2 + 20));
	scroll->setContentSize(Size(bgsize.width - 20, bgsize.height - 40));
	scroll->setScrollBarEnabled(false);
	scroll->setName("scroll");
	nodeTour->addChild(scroll);

	Size scrollsize = scroll->getContentSize();
	Node *nodeList = Node::create();
	nodeList->setPosition(scrollsize.width / 2, scrollsize.height / 2);
	nodeList->setName("nodelist");
	nodeList->setTag(0);
	nodeList->setVisible(true);
	scroll->addChild(nodeList);

	Node *nodeRank = Node::create();
	nodeRank->setPosition(nodeList->getPosition());
	nodeRank->setName("noderank");
	nodeRank->setTag(0);
	nodeRank->setVisible(false);
	scroll->addChild(nodeRank);

	ui::Button* btnList = ui::Button::create("btn_ds_chon.png", "btn_ds_chon.png", "", ui::Widget::TextureResType::PLIST);
	btnList->setAnchorPoint(Vec2(1, 0));
	btnList->setPosition(Vec2(0, bgsize.height / 2));
	nodeTour->addChild(btnList);

	ui::Button* btnRank = ui::Button::create("btn_bxh.png", "btn_bxh.png", "", ui::Widget::TextureResType::PLIST);
	btnRank->setAnchorPoint(Vec2(0, 0));
	btnRank->setPosition(Vec2(0, bgsize.height / 2));
	nodeTour->addChild(btnRank);

	addTouchEventListener(btnList, [=]() {
		if (nodeList->isVisible()) return;
		nodeList->setVisible(true);
		nodeRank->setVisible(false);
		btnList->loadTextures("btn_ds_chon.png", "btn_ds_chon.png", "", ui::Widget::TextureResType::PLIST);
		btnRank->loadTextures("btn_bxh.png", "btn_bxh.png", "", ui::Widget::TextureResType::PLIST);
		int height = nodeList->getTag();
		if (height > 0) {
			scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
		}
	});
	addTouchEventListener(btnRank, [=]() {
		if (nodeRank->isVisible()) return;
		nodeList->setVisible(false);
		nodeRank->setVisible(true);
		btnList->loadTextures("btn_ds.png", "btn_ds.png", "", ui::Widget::TextureResType::PLIST);
		btnRank->loadTextures("btn_bxh_chon.png", "btn_bxh_chon.png", "", ui::Widget::TextureResType::PLIST);
		int height = nodeRank->getTag();
		if (height > 0) {
			scroll->setInnerContainerSize(Size(scroll->getContentSize().width, height));
		}
	});
}

void LobbyScene::updateTableText(cocos2d::Node * table, std::vector<std::vector<std::string>> texts)
{
	if (texts.size() == 0) return;
	Node* nodeText = table->getChildByName("nodetext");
	int i = 0, j = 0;
	for (Node *n : nodeText->getChildren()) {
		Label *lb = (Label*)n;
		if (i < texts.size() && j < texts[i].size()) {
			lb->setString(texts[i][j]);

			j++;
			if (j == texts[i].size()) {
				j = 0;
				i++;
			}
		} else {
			lb->setString("");
		}
	}
}

cocos2d::Node* LobbyScene::drawTable(int numOfRows, int rowHeight, std::vector<int> widths, std::vector<std::string> titles, std::vector<std::vector<std::string>> texts)
{
	Node* table = Node::create();
	autoScaleNode(table);

	int height = numOfRows * rowHeight + rowHeight;
	int width = 0;
	for (int w : widths) {
		width += w;
	}

	ui::Scale9Sprite *bg = ui::Scale9Sprite::createWithSpriteFrameName("box2.png");
	bg->setContentSize(Size(width, height));
	bg->setName("bg");
	table->addChild(bg);

	int x = -width/2;
	for (int i = 0; i < widths.size()-1;i++) {
		x += widths[i];
		DrawNode *line = DrawNode::create(2);
		line->drawLine(Vec2(x, -height / 2), Vec2(x, height/2), Color4F(.56f, .38f, .21f, 1.0f));
		table->addChild(line);
	}
	int y = -height / 2;
	for (int i = 0; i < numOfRows; i++) {
		y += rowHeight;
		DrawNode *line = DrawNode::create(2);
		line->drawLine(Vec2(-width/2, y), Vec2(width/2, y), Color4F(.56f, .38f, .21f, 1.0f));
		table->addChild(line);
	}

	Node *nodeTitle = Node::create();
	nodeTitle->setName("nodetitle");
	table->addChild(nodeTitle);

	x = -width / 2;
	y = height / 2 - rowHeight / 2 - 5;
	for (int i = 0; i < titles.size();i++) {
		x += widths[i] / 2;
		Label* lb = Label::createWithTTF(titles[i], "fonts/myriad.ttf", 30);
        lb->setColor(Color3B::WHITE);
		lb->setPosition(x, y);
		lb->setName("lbtitle" + to_string(i));
		nodeTitle->addChild(lb);
		x += widths[i] / 2;
	}

	Node *nodeText = Node::create();
	nodeText->setName("nodetext");
	table->addChild(nodeText);
	y = height / 2 - rowHeight / 2 - 5;
	for (int i = 0; i < texts.size(); i++) {
		x = -width / 2;
		y -= rowHeight;
		for (int j = 0; j < texts[i].size(); j++) {
			x += widths[j] / 2;
			Label* lb = Label::createWithTTF(texts[i][j], "fonts/myriad.ttf", 30);
            lb->setColor(Color3B::WHITE);
			lb->setPosition(x, y);
			lb->setName("lbtext" + to_string(i) + to_string(j));
			nodeText->addChild(lb);
			cropLabel(lb, widths[j] - 10);
			x += widths[j] / 2;
		}
	}

	return table;
}

void LobbyScene::onInviteDataResponse(InviteData data)
{
	if (Utils::getSingleton().IgnoreInvitation) return;
	int index = data.RoomName.find_last_of("b");
	int tableId = atoi(data.RoomName.substr(index + 1, data.RoomName.length()).c_str());
	string strformat = Utils::getSingleton().getStringForKey("invite_content");
	string strmoney = Utils::getSingleton().formatMoneyWithComma(data.Money);
	string content = String::createWithFormat(strformat.c_str(), data.InviterName.c_str(), tableId, strmoney.c_str(), data.RoomTime)->getCString();

	Node* popupInvitation = getPopupInvitation();
	Label* lbcontent = (Label*)popupInvitation->getChildByName("lbcontent");
	lbcontent->setString(content);
	ui::Button* btnSubmit = (ui::Button*)popupInvitation->getChildByName("btnsubmit");
	addTouchEventListener(btnSubmit, [=]() {
		popupInvitation->stopAllActions();
		hidePopup(popupInvitation);
		showWaiting();
		SFSRequest::getSingleton().RequestJoinRoom(data.RoomName);
	}, false);

	showPopup(popupInvitation);
	DelayTime* delay = DelayTime::create(15);
	CallFunc* funcHide = CallFunc::create([=]() {
		hidePopup(popupInvitation);
	});
	popupInvitation->runAction(Sequence::createWithTwoActions(delay, funcHide));
}

void LobbyScene::onTableDataResponse(LobbyListTable data)
{
	hideWaiting();
	isChangeMoney = false;
	int height = (data.Size / 4 + (data.Size % 4 == 0 ? 0 : 1)) * 170;
	if (height < scrollListTable->getContentSize().height)
		height = scrollListTable->getContentSize().height;
	scrollListTable->setInnerContainerSize(Size(820, height));

	auto vecChildren = scrollListTable->getChildren();
	for (Node* c : vecChildren) {
		if (c->getTag() > data.Size) {
			c->setVisible(false);
		}
	}

	string zone = Utils::getSingleton().getCurrentZoneName();
	if (zone.length() > 0) {
		int index = zone.find_last_of("Q");
		if (index >= 0 && index < zone.length()) {
			zone = zone.substr(0, index);
		}
	} else {
		zone = "VuongPhu";
	}
	if (isTour || zone.compare("Authen") == 0) zone = "VuongPhu";
	bool isNhaTranh = zone.compare("NhaTranh") == 0;
	Color3B colorMoney = Utils::getSingleton().moneyType == 1 ? Color3B::YELLOW : Color3B(0, 255, 255);
	vector<Vec2> ppos;
	ppos.push_back(Vec2(65, 95));
	ppos.push_back(Vec2(145, 95));
	ppos.push_back(Vec2(37, 70));
	ppos.push_back(Vec2(170, 70));
	vector<int> prot = { -30, -30, 30, 30 };
	int numbPerRow = isTour ? 3 : 4;
	for (int i = 0; i < data.Size; i++) {
		ui::Button* btn;
		bool isNewBtn;
		if (i < vecTables.size()) {
			btn = vecTables[i];
			isNewBtn = false;
		} else {
			isNewBtn = true;
			string tbName = "table.png";// isSolo ? "table_SoLo.png" : "table_" + zone + ".png";
			btn = ui::Button::create(tbName, "", "", ui::Widget::TextureResType::PLIST);
			//btn->setPosition(Vec2(100 + (i % 4) * 210, height - 70 - (i / 4) * 170));
			btn->setTag(i + 1);
			//btn->setScale(.8f);
			scrollListTable->addChild(btn);
			vecTables.push_back(btn);
			autoScaleNode(btn);

			/*int x = 50;
			for (int j = 0; j < 4; j++) {
				Sprite* sp0 = Sprite::create("lobby/no_player.png");
				sp0->setPosition(x, 80);
				sp0->setTag(j);
				btn->addChild(sp0);

				Sprite* sp1 = Sprite::create("lobby/player_" + zone + ".png");
				sp1->setPosition(x, 80);
				sp1->setTag(10 + j);
				btn->addChild(sp1);

				x += 40;
			}*/

			Size bsize = btn->getContentSize();
			for (int j = 0; j < (isSolo ? 2 : 4); j++) {
				//Sprite* sp = Sprite::create("lobby/player" + to_string(j / 2 + 1) + zone + ".png");
				string playerName = "player" + to_string(j+1) + ".png";// isSolo ? "playerSoLo.png" : "player" + zone + ".png";
				Sprite* sp = Sprite::createWithSpriteFrameName(playerName);
				sp->setTag(j);
				btn->addChild(sp);
				if (isSolo) {
					sp->setPosition(j == 0 ? Vec2(40, 65) : Vec2(160, 75));
					//sp->setRotation(j == 0 ? 0 : 180);
				} else {
					sp->setPosition(ppos[j]);
					//sp->setFlipX(j == 3 || j == 1);
					//sp->setFlipY(j == 2 || j == 1);
					//sp->setRotation(prot[j]);
				}
			}

			Sprite* spStilt = Sprite::createWithSpriteFrameName("stilt.png");
			spStilt->setPosition(bsize.width / 2 + 5, bsize.height / 2 + 7);
			btn->addChild(spStilt);

			Label* lb411 = Label::createWithTTF("4-11", "fonts/myriadb.ttf", 30);
			lb411->setPosition(bsize.width / 2, 25);
			lb411->setAnchorPoint(Vec2(.5f, 1));
			lb411->setColor(isNhaTranh ? Color3B::WHITE : Color3B::BLACK);
			lb411->setName("lb411");
			btn->addChild(lb411);

			Sprite* spGa = Sprite::createWithSpriteFrameName("ga_off.png");
			spGa->setPosition(180, 5);
			spGa->setName("iconga");
			spGa->setScale(.8f);
			btn->addChild(spGa);

			Label* lbName = Label::createWithTTF(Utils::getSingleton().getStringForKey("table") + " " + to_string(i + 1), "fonts/myriadb.ttf", 30);
			lbName->setColor(lb411->getColor());
			lbName->setPosition(0, 15);
			lbName->setAnchorPoint(Vec2(0, 1));
			btn->addChild(lbName);

			Label* lbMoney = Label::createWithTTF("", "fonts/myriadb.ttf", 25);
			lbMoney->setColor(colorMoney);
			lbMoney->setPosition(210, 15);
			lbMoney->setAnchorPoint(Vec2(1, 1));
			lbMoney->setName("lbmoney");
			lbMoney->setVisible(false);
			btn->addChild(lbMoney);
		}
		btn->setVisible(true);
		btn->setPosition(Vec2(100 + (i % 4) * 240, height - 70 - (i / 4) * 175));
		addTouchEventListener(btn, [=]() {
			/*long requiredMoney = data.Money * 20;
			if ((Utils::getSingleton().moneyType == 1 && Utils::getSingleton().userDataMe.MoneyReal < requiredMoney)
				|| (!Utils::getSingleton().moneyType == 1 && Utils::getSingleton().userDataMe.MoneyFree < requiredMoney)) {
				double money = Utils::getSingleton().moneyType == 1 ? Utils::getSingleton().userDataMe.MoneyReal : Utils::getSingleton().userDataMe.MoneyFree;
				string str1 = Utils::getSingleton().formatMoneyWithComma(data.Money);
				string str2 = Utils::getSingleton().formatMoneyWithComma(requiredMoney);
				string str3 = Utils::getSingleton().formatMoneyWithComma(money);
				string str4 = Utils::getSingleton().getStringForKey("khong_du_tien_vao_ban");
				string notice = String::createWithFormat(str4.c_str(), str1.c_str(), str2.c_str(), str3.c_str())->getCString();
				showPopupNotice(notice, [=]() {});
				return;
			}*/
			string roomGroup = isTour ? tourGroup : listRoomData.ListRoomType[currentRoomType].Group;
			string roomName = roomGroup + to_string(btn->getTag());
			SFSRequest::getSingleton().RequestJoinRoom(roomName);
			showWaiting();
		}, isNewBtn);

		Sprite* spga = (Sprite*)btn->getChildByName("iconga");
		Label* lb411 = (Label*)btn->getChildByName("lb411");
		Label* lbMoney = (Label*)btn->getChildByName("lbmoney");
		spga->initWithSpriteFrameName("ga_off.png");
		lb411->setVisible(false);
		lbMoney->setColor(colorMoney);
		lbMoney->setString(Utils::getSingleton().formatMoneyWithComma(data.Money));
		if (data.Money < 1000000L) {
			lbMoney->setPositionX(210);
		} else {
			lbMoney->setPositionX(225);
		}
		
		for (int j = 0; j < (isSolo ? 2 : 4); j++) {
			/*Sprite* sp0 = (Sprite*)btn->getChildByTag(j);
			Sprite* sp1 = (Sprite*)btn->getChildByTag(10 + j);
			sp0->setVisible(true);
			sp1->setVisible(false);*/
			
			Sprite* sp = (Sprite*)btn->getChildByTag(j);
			sp->setVisible(false);
		}

		for (int k = 0; k < data.ListTable.size(); k++) {
			if (btn->getTag() == data.ListTable[k].RoomId) {
				lb411->setVisible(data.ListTable[k].IsU411);
				spga->initWithSpriteFrameName(data.ListTable[k].HasPot ? "ga_on.png" : "ga_off.png");
				for (int j = 0; j < (isSolo ? 2 : 4); j++) {
					/*Sprite* sp0 = (Sprite*)btn->getChildByTag(j);
					Sprite* sp1 = (Sprite*)btn->getChildByTag(10 + j);
					if (j < data.ListTable[k].ListPlayer.size()) {
						sp0->setVisible(false);
						sp1->setVisible(true);
					} else {
						sp0->setVisible(true);
						sp1->setVisible(false);
					}*/

					Sprite* sp = (Sprite*)btn->getChildByTag(j);
					sp->setVisible(j < data.ListTable[k].ListPlayer.size());
				}
				break;
			}
		}
	}
}

void LobbyScene::onRoomTypeDataResponse(LobbyListRoomType data)
{
	listRoomData = data;
	string currentRoom = Utils::getSingleton().currentRoomName;
	string zone = Utils::getSingleton().getCurrentZoneName();
	if (zone.compare("Authen") == 0) zone = "VuongPhuQuan";
	int index = zone.find_last_of("Q");
	if (index >= 0 && index < zone.length()) {
		zone = zone.substr(0, index);
	}
	int height = data.ListRoomType.size() * 78;
	if (height < scrollListRoom->getContentSize().height) 
		height = scrollListRoom->getContentSize().height;
	scrollListRoom->setInnerContainerSize(Size(180, height));
	for (int i = 0; i < data.ListRoomType.size(); i++) {
		ui::Button* btn = (ui::Button*)scrollListRoom->getChildByTag(i);
		string btnName = "btn_active.png";//isSolo ? "btn_VuongPhu.png" : "btn_" + zone + ".png";
		string btnName1 = "btn.png";//isSolo ? "btn_VuongPhu1.png" : "btn_" + zone + "1.png";
		if (btn == nullptr) {
			btn = ui::Button::create(btnName, btnName, "", ui::Widget::TextureResType::PLIST);
			btn->setTitleFontName("fonts/myriad.ttf");
			btn->setTitleFontSize(35);
			//btn->setTitleDeviation(Vec2(0, -5));
			btn->setPosition(Vec2(scrollListRoom->getContentSize().width / 2, height - 37 - i * 78));
			btn->setTag(i);
			addTouchEventListener(btn, [=]() {
				if (currentRoomType == btn->getTag()) return;
				ui::Button* lastBtn = (ui::Button*)scrollListRoom->getChildByTag(currentRoomType);
				lastBtn->setTitleFontName("fonts/myriad.ttf");
				btn->setTitleFontName("fonts/myriadb.ttf");
				lastBtn->loadTextures(btnName1, btnName1, "", ui::Widget::TextureResType::PLIST);
				btn->loadTextures(btnName, btnName, "", ui::Widget::TextureResType::PLIST);
				currentRoomType = i;
				Utils::getSingleton().currentLobbyName = data.ListRoomType[i].Name;
				SFSRequest::getSingleton().RequestJoinRoom(data.ListRoomType[i].Name);
				showWaiting();
			});
			scrollListRoom->addChild(btn);
			autoScaleNode(btn);
		} else {
			btn->setVisible(true);
		}
		btn->setTitleText(data.ListRoomType[i].Description);
		if (data.ListRoomType[i].Name.compare(currentRoom) != 0) {
			btn->setTitleFontName("fonts/myriad.ttf");
			btn->loadTextureNormal(btnName1, ui::Widget::TextureResType::PLIST);
		} else {
			currentRoomType = i;
			btn->setTitleFontName("fonts/myriadb.ttf");
			btn->loadTextureNormal(btnName, ui::Widget::TextureResType::PLIST);
			Utils::getSingleton().currentLobbyName = data.ListRoomType[i].Name;
		}
	}
	int i = data.ListRoomType.size();
	Node* tmpBtn;
	while ((tmpBtn = scrollListRoom->getChildByTag(i)) != nullptr) {
		tmpBtn->setVisible(false);
	}
}

void LobbyScene::onTableReconnectDataResponse(TableReconnectData data)
{
	SFSRequest::getSingleton().RequestJoinRoom(data.Room, false);
	showWaiting();
}

void LobbyScene::onPurchaseSuccess(std::string token)
{
	if (token.length() > 0) {
		SFSRequest::getSingleton().RequestPayment(token, Utils::getSingleton().ispmE());
	} else {
		hideWaiting();
	}
}

void LobbyScene::onListTourPlayersResponse(std::vector<TourPlayer> players)
{
	ui::ScrollView *scroll = (ui::ScrollView*)nodeTour->getChildByName("scroll");
	Node *nodeList = scroll->getChildByName("nodelist");

	std::vector<int> widths = { 70, 190, 150 };
	std::vector<std::vector<std::string>> texts;
	std::vector<std::string> titles;
	titles.push_back("STT");
	titles.push_back(Utils::getSingleton().getStringForKey("nguoi_choi_caps"));
	titles.push_back(Utils::getSingleton().getStringForKey("ban_choi_caps"));

	int i = 1;
	for (TourPlayer p : players) {
		vector<string> strs;
		strs.push_back(to_string(i));
		strs.push_back(p.Name);
		strs.push_back(p.Room);
		texts.push_back(strs);
		i++;
	}

	int psize = players.size();
	int height = psize * 50 + 50;
	Node *table = drawTable(psize, 50, widths, titles, texts);
	table->setPosition(0, -height / 2);
	nodeList->addChild(table);

	int width = scroll->getContentSize().width;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	nodeList->setPosition(width / 2, height);
	nodeList->setTag(height);
	if (nodeList->isVisible()) {
		scroll->setInnerContainerSize(Size(width, height));
	}
}

void LobbyScene::onTopTourPlayersResponse(std::vector<TourRoom> rooms)
{
	ui::ScrollView *scroll = (ui::ScrollView*)nodeTour->getChildByName("scroll");
	Node *nodeRank = scroll->getChildByName("noderank");

	std::vector<int> widths = { 60, 150, 120, 80 };
	std::vector<std::string> titles;
	titles.push_back("STT");
	titles.push_back(Utils::getSingleton().getStringForKey("nguoi_choi_caps"));
	titles.push_back(Utils::getSingleton().getStringForKey("van_danh_caps"));
	titles.push_back(Utils::getSingleton().getStringForKey("diem_caps"));

	int y = -5;
	for (TourRoom r : rooms) {
		Label* lb = Label::createWithTTF(r.Name, "fonts/myriad.ttf", 30);
		lb->setPosition(0, y);
		lb->setWidth(400);
		nodeRank->addChild(lb);

		int i = 1;
		std::vector<std::vector<std::string>> texts;
		for (TourPlayer p : r.Players) {
			vector<string> strs;
			strs.push_back(to_string(i));
			strs.push_back(p.Name);
			strs.push_back(to_string(p.Matches));
			strs.push_back(to_string(p.TotalCuoc));
			texts.push_back(strs);
			i++;
		}

		Node *table = drawTable(4, 50, widths, titles, texts);
		table->setPosition(0, y - 140);
		nodeRank->addChild(table);

		y -= 300;
	}

	int width = scroll->getContentSize().width;
	int height = -y;
	if (height < scroll->getContentSize().height) {
		height = scroll->getContentSize().height;
	}
	nodeRank->setPosition(width / 2, height - 15);
	nodeRank->setTag(height);
	if (nodeRank->isVisible()) {
		scroll->setInnerContainerSize(Size(width, height));
	}
}

void LobbyScene::onTourNewRound(std::string room)
{
	showPopupNotice(Utils::getSingleton().getStringForKey("vong_tiep_theo_da_bat_dau"), [=]() {
		onTourRoomToJoin(room);
	});
}

void LobbyScene::onBackScene()
{
	if (isTour) {
		showWaiting();
		isBackToMain = true;
		SFSRequest::getSingleton().Disconnect();
	} else {
		Utils::getSingleton().goToMainScene();
	}
}

void LobbyScene::onChangeMoneyType(int type)
{
	showWaiting();
	isChangeMoney = true;
	currentMoneyType = type;
	tmpZoneName = Utils::getSingleton().currentZoneName;
	SFSRequest::getSingleton().Disconnect();
}
