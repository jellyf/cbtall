#pragma once
#include "LoginScene.h"
#include "Utils.h"
#include "SFSRequest.h"
#include "EventHandler.h"
#include "Constant.h"
#include "json/document.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

using namespace std;
using namespace cocos2d;

void LoginScene::onInit()
{
	BaseScene::onInit();
	Utils::getSingleton().loginType = -1;
	Utils::getSingleton().SoundEnabled = UserDefault::getInstance()->getBoolForKey(constant::KEY_SOUND.c_str(), true);
	Utils::getSingleton().IgnoreInvitation = UserDefault::getInstance()->getBoolForKey(constant::KEY_INVITATION.c_str(), false);
	UserDefault::getInstance()->setBoolForKey(constant::KEY_SOUND.c_str(), Utils::getSingleton().SoundEnabled);
	UserDefault::getInstance()->setBoolForKey(constant::KEY_INVITATION.c_str(), Utils::getSingleton().IgnoreInvitation);
	isReadyToMain = Utils::getSingleton().downloadedPlistTexture != 0;
	bool ispmE = Utils::getSingleton().ispmE();

	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGB565);
	Texture2D* bgTexture = TextureCache::getInstance()->addImage("login_bg.jpg");
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);

	Sprite* bg = Sprite::createWithTexture(bgTexture);
	bg->setPosition(560, 350);
	addChild(bg);

	/*Sprite* spTitle = Sprite::create("login/title.png");
	spTitle->setPosition(560, 550);
	mLayer->addChild(spTitle);
	autoScaleNode(spTitle);*/

	Sprite* spTab = Sprite::createWithSpriteFrameName("tab.png");
	spTab->setPosition(560, 350);
	mLayer->addChild(spTab);
	autoScaleNode(spTab);

	loginNode = Node::create();
	loginNode->setPosition(560, 350);
	mLayer->addChild(loginNode);
	autoScaleNode(loginNode);

	tfUsername = ui::EditBox::create(Size(440, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	tfUsername->setPosition(Vec2(0, 80));
	tfUsername->setFontName("Arial");
	tfUsername->setFontSize(25);
	tfUsername->setFontColor(Color3B::WHITE);
	tfUsername->setPlaceHolder(Utils::getSingleton().getStringForKey("login_name").c_str());
	tfUsername->setPlaceholderFontColor(Color3B::WHITE);
	tfUsername->setPlaceholderFont("Arial", 25);
	tfUsername->setMaxLength(16);
	tfUsername->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfUsername->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfUsername->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfUsername->setDelegate(this);
	loginNode->addChild(tfUsername);

	tfPassword = ui::EditBox::create(Size(440, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	tfPassword->setPosition(Vec2(0, 10));
	tfPassword->setFontName("Arial");
	tfPassword->setFontSize(25);
	tfPassword->setFontColor(Color3B::WHITE);
	tfPassword->setPlaceHolder(Utils::getSingleton().getStringForKey("password").c_str());
	tfPassword->setPlaceholderFontColor(Color3B::WHITE);
	tfPassword->setPlaceholderFont("Arial", 25);
	tfPassword->setMaxLength(16);
	tfPassword->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
	tfPassword->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfPassword->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfPassword->setDelegate(this);
	loginNode->addChild(tfPassword);

	ui::CheckBox* checkbox = ui::CheckBox::create();
	checkbox->loadTextureBackGround("box.png", ui::Widget::TextureResType::PLIST);
	checkbox->loadTextureFrontCross("check.png", ui::Widget::TextureResType::PLIST);
	checkbox->setPosition(Vec2(-195, -50));
	checkbox->setSelected(true);
	loginNode->addChild(checkbox);

	Label* lb1 = Label::createWithTTF("", "fonts/arialbd.ttf", 20);
	lb1->setString(Utils::getSingleton().getStringForKey("remember_password"));
	
	lb1->setPosition(-105, -50);
	loginNode->addChild(lb1);

	lbBtnForgotPass = Label::createWithTTF("", "fonts/arialbd.ttf", 20);
	lbBtnForgotPass->setString(Utils::getSingleton().getStringForKey("forgot_password"));
	lbBtnForgotPass->setPosition(145, -50);
	lbBtnForgotPass->setVisible(ispmE);
	loginNode->addChild(lbBtnForgotPass);

	btnForgotPass = ui::Button::create("white.png", "white.png", "", ui::Widget::TextureResType::PLIST);
	btnForgotPass->setContentSize(lbBtnForgotPass->getContentSize() + Size(40, 20));
	btnForgotPass->setPosition(lbBtnForgotPass->getPosition());
	btnForgotPass->setScale9Enabled(true);
	btnForgotPass->setOpacity(0);
	btnForgotPass->setVisible(ispmE);
	addTouchEventListener(btnForgotPass, [=]() {
		string str = Utils::getSingleton().gameConfig.smsMK;
		string strMsg = Utils::getSingleton().getStringForKey("open_sms_retake_password");
		strMsg = Utils::getSingleton().replaceString(strMsg, "sms", str);
		showPopupNotice(strMsg, [=]() {
			int i = str.find_last_of(' ');
			string number = str.substr(i + 1, str.length() - i - 1);
			string content = str.substr(0, i);
			Utils::getSingleton().openSMS(number, content);
		});
	});
	loginNode->addChild(btnForgotPass);

	ui::Button* btnLogin = ui::Button::create("btn_login.png", "btn_login_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnLogin->setPosition(Vec2(-110, -115));
	addTouchEventListener(btnLogin, [=]() {
		if (Utils::getSingleton().gameConfig.phone.length() == 0) {
            waitingLogin = 1;
			requestGameConfig(isRealConfig);
			return;
		}
        loginNormal();
	});
	loginNode->addChild(btnLogin);

	ui::Button* btnRegister = ui::Button::create("btn_register.png", "btn_register_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnRegister->setPosition(Vec2(110, -115));
	addTouchEventListener(btnRegister, [=]() {
		loginNode->setVisible(false);
		registerNode->setVisible(true);
	});
	loginNode->addChild(btnRegister);

	ui::Button* btnFB = ui::Button::create("btn_fb.png", "btn_fb_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnFB->setPosition(Vec2(0, -195));
	addTouchEventListener(btnFB, [=]() {
        if (Utils::getSingleton().gameConfig.phone.length() == 0) {
            waitingLogin = 2;
            requestGameConfig(isRealConfig);
            return;
        }
        loginFacebook();
	});
	loginNode->addChild(btnFB);

	initRegisterNode();

	ui::Scale9Sprite* spHeader = ui::Scale9Sprite::createWithSpriteFrameName("footer.png");
	spHeader->setContentSize(Size(1120 / scaleScene.y, 45 / scaleScene.x));
	spHeader->setAnchorPoint(Vec2(0, 0));
	spHeader->setPosition(0, 700);
	spHeader->setFlippedY(true);
	mLayer->addChild(spHeader);
	//autoScaleNode(spHeader);

	ui::Scale9Sprite* spFooter = ui::Scale9Sprite::createWithSpriteFrameName("footer.png");
	spFooter->setAnchorPoint(Vec2(0, 0));
	spFooter->setContentSize(Size(1120 / scaleScene.y, 45 / scaleScene.x));
	mLayer->addChild(spFooter);
	//autoScaleNode(spFooter);

	bool isImgReady = Utils::getSingleton().downloadedPlistTexture;
	btnPhone = ui::Button::create(isImgReady ? "btn_phone.png" : "empty.png", isImgReady ? "btn_phone_clicked.png" : "empty.png", "", ui::Widget::TextureResType::PLIST);
	btnPhone->setPosition(Vec2(60 * scaleScene.y, 55 * scaleScene.x));
	btnPhone->setVisible(ispmE);
	addTouchEventListener(btnPhone, [=]() {
		string phone = Utils::getSingleton().gameConfig.phone;
		if (phone.length() == 0) return;
		Utils::getSingleton().openTel(phone);
	});
	mLayer->addChild(btnPhone);
	autoScaleNode(btnPhone);

	labelPhone = Label::create("", "fonts/arialbd.ttf",25);
	labelPhone->setPosition(110 * scaleScene.y, 10 * scaleScene.x);
	labelPhone->setAnchorPoint(Vec2(0, 0));
	labelPhone->setVisible(false);
	mLayer->addChild(labelPhone);
	autoScaleNode(labelPhone);

	Label* labelVersion = Label::create(string("ver ") + Application::sharedApplication()->getVersion(), "fonts/arial.ttf", 18);
	labelVersion->setPosition(1120 - 10 * scaleScene.y, 3 * scaleScene.x);
	labelVersion->setAnchorPoint(Vec2(1, 0));
	mLayer->addChild(labelVersion);
	autoScaleNode(labelVersion);

	spNetwork->setPosition(1080, 660);

	std::string lastUsername = UserDefault::getInstance()->getStringForKey(constant::KEY_USERNAME.c_str());
	std::string lastPassword = UserDefault::getInstance()->getStringForKey(constant::KEY_PASSWORD.c_str());
	tfUsername->setText(lastUsername.c_str());
	tfPassword->setText(lastPassword.c_str());

    SFSRequest::getSingleton().ForceIPv6(false);
	if (Utils::getSingleton().gameConfig.phone.length() == 0) {
		requestGameConfig(isRealConfig);
	} else {
		showWaiting();
		SFSRequest::getSingleton().Connect();
	}

	loadTextureCache();
}

void LoginScene::registerEventListenner()
{
	BaseScene::registerEventListenner();
	EventHandler::getSingleton().onConnected = std::bind(&LoginScene::onConnected, this);
	EventHandler::getSingleton().onLoginZone = std::bind(&LoginScene::onLoginZone, this);
	EventHandler::getSingleton().onConnectionLost = std::bind(&LoginScene::onConnectionLost, this, std::placeholders::_1);
	EventHandler::getSingleton().onConfigZoneReceived = std::bind(&LoginScene::onConfigZoneReceived, this);
	EventHandler::getSingleton().onErrorSFSResponse = std::bind(&LoginScene::onErrorResponse, this, std::placeholders::_1, std::placeholders::_2);
	EventHandler::getSingleton().onUserDataMeSFSResponse = std::bind(&LoginScene::onUserDataMeResponse, this);
	EventHandler::getSingleton().onLoginFacebook = std::bind(&LoginScene::onLoginFacebook, this, std::placeholders::_1);
	EventHandler::getSingleton().onLobbyTableSFSResponse = bind(&LoginScene::onTableDataResponse, this, placeholders::_1);
}

void LoginScene::unregisterEventListenner()
{
	BaseScene::unregisterEventListenner();
	EventHandler::getSingleton().onConnected = NULL;
	EventHandler::getSingleton().onLoginZone = NULL;
	EventHandler::getSingleton().onConnectionLost = NULL;
	EventHandler::getSingleton().onConfigZoneReceived = NULL;
	EventHandler::getSingleton().onErrorSFSResponse = NULL;
	EventHandler::getSingleton().onUserDataMeSFSResponse = NULL;
	EventHandler::getSingleton().onLoginFacebook = NULL;
	EventHandler::getSingleton().onLobbyTableSFSResponse = NULL;
}

void LoginScene::editBoxReturn(ui::EditBox * editBox)
{
}

void LoginScene::onConnected()
{
	if (isReconnecting) {
		Utils::getSingleton().loginZoneByIndex(tmpZoneIndex / 10, tmpZoneIndex % 10);
	} else {
		SFSRequest::getSingleton().LoginZone("", "g", Utils::getSingleton().gameConfig.zone);
	}
}

void LoginScene::onLoginZone()
{
	isLogedInZone = true;
	if (isReconnecting) {
	} else if(isRequesting){
		if (loginNode->isVisible()) {
			if (fbToken.length() > 0) {
				SFSRequest::getSingleton().RequestLoginFacebook(fbToken);
			} else {
				SFSRequest::getSingleton().RequestLogin(tfUsername->getText(), md5(tfPassword->getText()));
			}
		} else {
			//SFSRequest::getSingleton().RequestRegister(tfResUname->getText(), md5(tfResPass->getText()), tfResEmail->getText());
			SFSRequest::getSingleton().RequestRegister(tfResUname->getText(), md5(tfResPass->getText()), md5(tfResPassAgain->getText()));
		}
	} else {
		hideWaiting();
	}
}

void LoginScene::onConnectionLost(std::string reason)
{
	isLogedInZone = false;
	if (isReconnecting) {
		Utils::getSingleton().connectZoneByIndex(tmpZoneIndex / 10, tmpZoneIndex % 10);
	}
}

void LoginScene::onConnectionFailed()
{
	isLogedInZone = false;
    if(isIPv4){
        isIPv4 = false;
        SFSRequest::getSingleton().ForceIPv6(true);
        SFSRequest::getSingleton().Connect();
		Utils::getSingleton().timeStartReconnect = Utils::getSingleton().getCurrentSystemTimeInSecs();
    }else{
        isIPv4 = CC_TARGET_PLATFORM == CC_PLATFORM_IOS;
        SFSRequest::getSingleton().ForceIPv6(false);

		double waitedTime = Utils::getSingleton().getCurrentSystemTimeInSecs() - Utils::getSingleton().timeStartReconnect;
		if (waitedTime < 0) waitedTime = 0;
		if (waitedTime > 3) waitedTime = 3;
		DelayTime* delay = DelayTime::create(3 - waitedTime);
		CallFunc* func = CallFunc::create([=]() {
			hideWaiting();
			showPopupNotice(Utils::getSingleton().getStringForKey("connection_failed"), [=]() {});
		});
		this->runAction(Sequence::createWithTwoActions(delay, func));
    }
}

void LoginScene::onConfigZoneReceived()
{
	string username = Utils::getSingleton().userDataMe.Name;
	double currentTime = Utils::getSingleton().getCurrentSystemTimeInSecs();
	double interruptedTime = UserDefault::getInstance()->getDoubleForKey((constant::KEY_RECONNECT_TIME + username).c_str(), 0);
	if (currentTime < interruptedTime) {
		tmpZoneIndex = UserDefault::getInstance()->getIntegerForKey((constant::KEY_RECONNECT_ZONE_INDEX + username).c_str(), 0);
		UserDefault::getInstance()->setDoubleForKey((constant::KEY_RECONNECT_TIME + username).c_str(), 0);
		SFSRequest::getSingleton().Disconnect();
		isReconnecting = true;
		return;
	} else {
		updateStateToGoToMain(1);
	}
}

void LoginScene::onUserDataMeResponse()
{
	BaseScene::onUserDataMeResponse();
	if (fbToken.length() > 0) {
		Utils::getSingleton().loginType = constant::LOGIN_FACEBOOK;
		Utils::getSingleton().saveUsernameAndPassword(fbToken, "");
	} else {
		Utils::getSingleton().loginType = constant::LOGIN_NORMAL;
		Utils::getSingleton().saveUsernameAndPassword(tfUsername->getText(), tfPassword->getText());
	}
}

void LoginScene::onLoginFacebook(std::string token)
{
	isLoginFacebook = false;
	hideWaiting();
	if (token.length() == 0) {
		showPopupNotice(Utils::getSingleton().getStringForKey("error_unknown"), [=]() {});
	} else if (token.compare("cancel") == 0) {
		
	} else {
		fbToken = token;
		showWaiting();
		if (isLogedInZone) {
			SFSRequest::getSingleton().RequestLoginFacebook(fbToken);
		} else {
			SFSRequest::getSingleton().Connect();
		}
	}
}

void LoginScene::onErrorResponse(unsigned char code, std::string msg)
{
	if (code == 49) return;
	hideWaiting();
	if (code == 48) {
		loginNode->setVisible(true);
		registerNode->setVisible(false);
		tfUsername->setText(tfResUname->getText());
		tfPassword->setText(tfResPass->getText());
		Utils::getSingleton().saveUsernameAndPassword(tfUsername->getText(), tfPassword->getText());
	}
	if (msg.length() == 0) return;
	showPopupNotice(msg, [=]() {});
	fbToken = "";
}

void LoginScene::onHttpResponse(int tag, std::string content)
{
	if (tag != constant::TAG_HTTP_GAME_CONFIG) return;
	if (content.length() == 0) {
		onHttpResponseFailed();
		return;
	}
	rapidjson::Document d;
	GameConfig config;
	d.Parse<0>(content.c_str());

	vector<string> keys = { "payment", "paymentIOS", "name", "host", "port", "websocket", "version", "versionIOS", "ip_rs", "phone",
		"smsVT", "smsVNPVMS", "smsKH", "smsMK", "fb", "a", "i", "updatenow", "inapp", "invite" };
	for (string k : keys) {
		if (d.FindMember(k.c_str()) == d.MemberEnd()) {
			onHttpResponseFailed();
			return;
		}
	}

	config.pmE = d["payment"].GetBool();
	config.pmEIOS = d["paymentIOS"].GetBool();
	config.zone = d["name"].GetString();
	config.host = d["host"].GetString();
	config.port = d["port"].GetInt();
	config.websocket = d["websocket"].GetInt();
	config.version = d["version"].GetInt();
	config.versionIOS = d["versionIOS"].GetInt();
	config.ip_rs = d["ip_rs"].GetString();
	config.phone = d["phone"].GetString();
	config.smsVT = d["smsVT"].GetString();
	config.smsVNPVMS = d["smsVNPVMS"].GetString();
	config.smsKH = d["smsKH"].GetString();
	config.smsMK = d["smsMK"].GetString();
	config.linkFb = d["fb"].GetString();
	config.linkAndroid = d["a"].GetString();
	config.linkIOS = d["i"].GetString();
	config.canUpdate = d["updatenow"].GetBool();
	config.inapp = d["inapp"].GetString();
	config.invite = d["invite"].GetBool();
		
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (isRealConfig && !config.pmEIOS) {
		isRealConfig = false;
		//requestGameConfig(false);
		//return;
	}
#endif

	string verstr = Application::sharedApplication()->getVersion();
	int i = verstr.find_last_of('.') + 1;
	verstr = verstr.substr(i, verstr.length() - i);
	int nver = atoi(verstr.c_str());
	config.pmE &= config.version > nver;
	config.pmEIOS &= config.versionIOS > nver;

	Utils::getSingleton().gameConfig = config;
    Utils::getSingleton().queryIAPProduct();

	if (Utils::getSingleton().ispmE()) {
		btnForgotPass->setVisible(true);
		lbBtnForgotPass->setVisible(true);
		Utils::getSingleton().downloadPlistTextures();
	}
	//string location = Utils::getSingleton().getUserCountry();
	//Utils::getSingleton().gameConfig.pmE = config.pmE && location.compare("vn") == 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (config.canUpdate && nver < config.versionIOS - 1) {
		showPopupNotice(Utils::getSingleton().getStringForKey("notice_update_new_version"), [=]() {
			Application::sharedApplication()->openURL(config.linkIOS);
		});
#else
	if (config.canUpdate && nver < config.version - 1) {
		showPopupNotice(Utils::getSingleton().getStringForKey("notice_update_new_version"), [=]() {
			Application::sharedApplication()->openURL(config.linkAndroid);
		});
#endif
		SFSRequest::getSingleton().Connect();
	} else if (waitingLogin > 0) {
        if(waitingLogin == 1){
            loginNormal();
        }else if(waitingLogin == 2){
            loginFacebook();
        }
        waitingLogin = 0;
	} else {
		SFSRequest::getSingleton().Connect();
	}
	//hideWaiting();
}

void LoginScene::onHttpResponseFailed()
{
	if (currentConfigLink == 0) {
		currentConfigLink = 1;
		hideWaiting();
		requestGameConfig(false);
	} else {
		currentConfigLink = 0;
		hideWaiting();
		if (isFirstLoadConfig) {
			isFirstLoadConfig = false;
		} else {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_connection"), [=]() {});
		}
	}
}

void LoginScene::onTableDataResponse(LobbyListTable data)
{
	Utils::getSingleton().goToLobbyScene();
}

void LoginScene::onDownloadedPlistTexture(int numb)
{
	BaseScene::onDownloadedPlistTexture(numb);
	btnPhone->loadTextures("btn_phone.png", "btn_phone_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnPhone->setVisible(true);
	//labelPhone->setVisible(true);
	labelPhone->setString(Utils::getSingleton().gameConfig.phone);
	if (numb == 1) {
		updateStateToGoToMain(numb);
	}
}

bool LoginScene::onKeyBack()
{
	bool canBack = BaseScene::onKeyBack();
	if (canBack) {
		if (registerNode->isVisible()) {
			loginNode->setVisible(true);
			registerNode->setVisible(false);
			return false;
		}if (isLoginFacebook) {
			return false;
		} else {
			showPopupNotice(Utils::getSingleton().getStringForKey("ban_muon_thoat_game"), [=]() {
				Director::sharedDirector()->end();
			});
			return false;
		}
	}
	return canBack;
}

void LoginScene::loginNormal()
{
	isRequesting = true;
    string username = tfUsername->getText();// Utils::getSingleton().trim(tfUsername->getText());
    if (username.length() == 0) {
        showPopupNotice(Utils::getSingleton().getStringForKey("hay_nhap_tai_khoan"), [=]() {});
        return;
    }
    string password = tfPassword->getText();// Utils::getSingleton().trim(tfPassword->getText());
    if (password.length() == 0) {
        showPopupNotice(Utils::getSingleton().getStringForKey("hay_nhap_mat_khau"), [=]() {});
        return;
    }
    showWaiting();
	if (isLogedInZone) {
		SFSRequest::getSingleton().RequestLogin(tfUsername->getText(), md5(tfPassword->getText()));
	} else {
		SFSRequest::getSingleton().Connect();
	}
}

void LoginScene::loginFacebook()
{
	isRequesting = true;
	isLoginFacebook = true;
    showWaiting();
	Utils::getSingleton().loginFacebook();
}

void LoginScene::initRegisterNode()
{
	registerNode = Node::create();
	registerNode->setPosition(560, 350);
	registerNode->setVisible(false);
	mLayer->addChild(registerNode);
	autoScaleNode(registerNode);

	tfResUname = ui::EditBox::create(Size(440, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	tfResUname->setPosition(Vec2(0, 80));
	tfResUname->setFontName("Arial");
	tfResUname->setFontSize(25);
	tfResUname->setFontColor(Color3B::WHITE);
	tfResUname->setPlaceHolder(Utils::getSingleton().getStringForKey("login_name").c_str());
	tfResUname->setPlaceholderFontColor(Color3B::WHITE);
	tfResUname->setPlaceholderFont("Arial", 25);
	tfResUname->setMaxLength(20);
	tfResUname->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfResUname->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfResUname->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfResUname->setDelegate(this);
	registerNode->addChild(tfResUname);

	tfResPass = ui::EditBox::create(Size(440, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	tfResPass->setPosition(Vec2(0, 10));
	tfResPass->setFontName("Arial");
	tfResPass->setFontSize(25);
	tfResPass->setFontColor(Color3B::WHITE);
	tfResPass->setPlaceHolder(Utils::getSingleton().getStringForKey("password").c_str());
	tfResPass->setPlaceholderFontColor(Color3B::WHITE);
	tfResPass->setPlaceholderFont("Arial", 25);
	tfResPass->setMaxLength(20);
	tfResPass->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
	tfResPass->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfResPass->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfResPass->setDelegate(this);
	registerNode->addChild(tfResPass);

	tfResPassAgain = ui::EditBox::create(Size(440, 55), "box0.png", ui::Widget::TextureResType::PLIST);
	tfResPassAgain->setPosition(Vec2(0, -60));
	tfResPassAgain->setFontName("Arial");
	tfResPassAgain->setFontSize(25);
	tfResPassAgain->setFontColor(Color3B::WHITE);
	tfResPassAgain->setPlaceHolder(Utils::getSingleton().getStringForKey("retype_password").c_str());
	tfResPassAgain->setPlaceholderFontColor(Color3B::WHITE);
	tfResPassAgain->setPlaceholderFont("Arial", 25);
	tfResPassAgain->setMaxLength(20);
	tfResPassAgain->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
	tfResPassAgain->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfResPassAgain->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfResPassAgain->setDelegate(this);
	registerNode->addChild(tfResPassAgain);

	/*tfResEmail = ui::EditBox::create(Size(440, 55), "login/box.png");
	tfResEmail->setPosition(Vec2(0, -60));
	tfResEmail->setFontName("Arial");
	tfResEmail->setFontSize(25);
	tfResEmail->setFontColor(Color3B::WHITE);
	tfResEmail->setPlaceHolder("Email");
	tfResEmail->setPlaceholderFontColor(Color3B::WHITE);
	tfResEmail->setPlaceholderFont("Arial", 25);
	tfResEmail->setMaxLength(30);
	tfResEmail->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	tfResEmail->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	tfResEmail->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	tfResEmail->setDelegate(this);
	registerNode->addChild(tfResEmail);*/

	ui::Button* btnBack = ui::Button::create("btn_turn_back.png", "btn_turn_back_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnBack->setPosition(Vec2(-110, -130));
	addTouchEventListener(btnBack, [=]() {
		loginNode->setVisible(true);
		registerNode->setVisible(false);
	});
	registerNode->addChild(btnBack);

	ui::Button* btnRegister = ui::Button::create("btn_register.png", "btn_register_clicked.png", "", ui::Widget::TextureResType::PLIST);
	btnRegister->setPosition(Vec2(110, -130));
	addTouchEventListener(btnRegister, [=]() {
		if (Utils::getSingleton().gameConfig.phone.length() == 0) {
			requestGameConfig(isRealConfig);
			return;
		}
		if (!Utils::getSingleton().isUsernameValid(tfResUname->getText())
			|| !Utils::getSingleton().isPasswordValid(tfResPass->getText())) {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_password_must_6_char"), [=]() {});
			return;
		}
		string retypePass = tfResPassAgain->getText();
		if (retypePass.compare(tfResPass->getText()) != 0) {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_retype_password"), [=]() {});
			return;
		}
		isRequesting = true;
		showWaiting();
		if (isLogedInZone) {
			SFSRequest::getSingleton().RequestRegister(tfResUname->getText(), md5(tfResPass->getText()), md5(tfResPassAgain->getText()));
		} else {
			SFSRequest::getSingleton().Connect();
		}
	});
	registerNode->addChild(btnRegister);
}

void LoginScene::requestGameConfig(bool realConfig)
{
	showWaiting();
	if (realConfig) {
		//SFSRequest::getSingleton().RequestHttpGet("http://kinhtuchi.com/configchanktc.txt", constant::TAG_HTTP_GAME_CONFIG);
		//SFSRequest::getSingleton().RequestHttpGet("http://115.84.179.242/configchanktc.txt", constant::TAG_HTTP_GAME_CONFIG);
		SFSRequest::getSingleton().RequestHttpGet("http://125.212.207.71/config/configChan.txt", constant::TAG_HTTP_GAME_CONFIG);
	} else {
		SFSRequest::getSingleton().RequestHttpGet("http://kinhtuchi.com/configchanktc.txt", constant::TAG_HTTP_GAME_CONFIG);
	}
}

void LoginScene::loadTextureCache()
{
	TextureCache::sharedTextureCache()->addImageAsync("game.png", [=](Texture2D* texture) {
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("game.plist");
	});
	TextureCache::sharedTextureCache()->addImageAsync("buttons.png", [=](Texture2D* texture) {
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("buttons.plist");
	});
}

void LoginScene::updateStateToGoToMain(int state)
{
	if (isReadyToMain || !Utils::getSingleton().ispmE()) {
		Utils::getSingleton().goToMainScene();
	} else {
		isReadyToMain = true;
	}
}
