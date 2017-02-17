#pragma once
#include "LoginScene.h"
#include "Utils.h"
#include "SFSRequest.h"
#include "EventHandler.h"
#include "Constant.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

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

	Sprite* bg = Sprite::create("bg.jpg");
	bg->setPosition(560, 350);
	addChild(bg);

	Sprite* spTitle = Sprite::create("login/title.png");
	spTitle->setPosition(560, 550);
	mLayer->addChild(spTitle);
	autoScaleNode(spTitle);

	loginNode = Node::create();
	loginNode->setPosition(560, 350);
	mLayer->addChild(loginNode);
	autoScaleNode(loginNode);

	tfUsername = ui::EditBox::create(Size(440, 55), "login/box.png");
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

	tfPassword = ui::EditBox::create(Size(440, 55), "login/box.png");
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
	checkbox->loadTextureBackGround("popup/box.png");
	checkbox->loadTextureFrontCross("popup/check.png");
	checkbox->setPosition(Vec2(-195, -50));
	checkbox->setSelected(true);
	loginNode->addChild(checkbox);

	Label* lb1 = Label::createWithTTF("", "fonts/arialbd.ttf", 20);
	lb1->setString(Utils::getSingleton().getStringForKey("remember_password"));
	
	lb1->setPosition(-105, -50);
	loginNode->addChild(lb1);

	Label* lb2 = Label::createWithTTF("", "fonts/arialbd.ttf", 20);
	lb2->setString(Utils::getSingleton().getStringForKey("forgot_password"));
	lb2->setPosition(145, -50);
	loginNode->addChild(lb2);

	ui::Button* btnForgotPass = ui::Button::create("white.png", "white.png");
	btnForgotPass->setContentSize(lb2->getContentSize() + Size(40, 20));
	btnForgotPass->setPosition(lb2->getPosition());
	btnForgotPass->setScale9Enabled(true);
	btnForgotPass->setOpacity(0);
	addTouchEventListener(btnForgotPass, [=]() {
		showPopupNotice(Utils::getSingleton().getStringForKey("open_sms_retake_password"), [=]() {
			string str = Utils::getSingleton().gameConfig.smsMK;
			int i = str.find_last_of(' ');
			string number = str.substr(i + 1, str.length() - i - 1);
			string content = str.substr(0, i);
			Utils::getSingleton().openSMS(number, content);
		});
	});
	loginNode->addChild(btnForgotPass);

	ui::Button* btnLogin = ui::Button::create("login/btn_login.png", "login/btn_login_clicked.png");
	btnLogin->setPosition(Vec2(-110, -115));
	addTouchEventListener(btnLogin, [=]() {
		if (Utils::getSingleton().gameConfig.phone.length() == 0) {
            waitingLogin = 1;
			requestGameConfig();
			return;
		}
        loginNormal();
	});
	loginNode->addChild(btnLogin);

	ui::Button* btnRegister = ui::Button::create("login/btn_register.png", "login/btn_register_clicked.png");
	btnRegister->setPosition(Vec2(110, -115));
	addTouchEventListener(btnRegister, [=]() {
		loginNode->setVisible(false);
		registerNode->setVisible(true);
	});
	loginNode->addChild(btnRegister);

	ui::Button* btnFB = ui::Button::create("login/btn_fb.png", "login/btn_fb_clicked.png");
	btnFB->setPosition(Vec2(0, -195));
	addTouchEventListener(btnFB, [=]() {
        if (Utils::getSingleton().gameConfig.phone.length() == 0) {
            waitingLogin = 2;
            requestGameConfig();
            return;
        }
        loginFacebook();
	});
	loginNode->addChild(btnFB);

	initRegisterNode();

	ui::Button* btnPhone = ui::Button::create("login/btn_phone.png", "login/btn_phone_clicked.png");
	btnPhone->setPosition(Vec2(45, 40 * scaleScene.x));
	addTouchEventListener(btnPhone, [=]() {
		string phone = Utils::getSingleton().gameConfig.phone;
		if (phone.length() == 0) return;
		Utils::getSingleton().openTel(phone);
	});
	mLayer->addChild(btnPhone);
	autoScaleNode(btnPhone);

	labelPhone = Label::create("01639070707", "fonts/arialbd.ttf",25);
	labelPhone->setPosition(90, 3);
	labelPhone->setAnchorPoint(Vec2(0, 0));
	mLayer->addChild(labelPhone);
	autoScaleNode(labelPhone);

	Label* labelVersion = Label::create(string("ver ") + Application::sharedApplication()->getVersion(), "fonts/arial.ttf", 18);
	labelVersion->setPosition(1085, 3);
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
		requestGameConfig();
	} else {
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

	SFSRequest::getSingleton().onHttpResponseFailed = std::bind(&LoginScene::onHttpResponseFailed, this);
	SFSRequest::getSingleton().onHttpResponse = std::bind(&LoginScene::onHttpResponse, this, std::placeholders::_1, std::placeholders::_2);
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

	SFSRequest::getSingleton().onHttpResponse = NULL;
	SFSRequest::getSingleton().onHttpResponseFailed = NULL;
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
    }else{
        isIPv4 = CC_TARGET_PLATFORM == CC_PLATFORM_IOS;
        SFSRequest::getSingleton().ForceIPv6(false);
        hideWaiting();
        showPopupNotice(Utils::getSingleton().getStringForKey("connection_failed"), [=]() {});
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
		Utils::getSingleton().goToMainScene();
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
	rapidjson::Document d;
	d.Parse<0>(content.c_str());
	if (tag == 1) {
		GameConfig config;
		try {
			config.paymentEnabled = d["payment"].GetBool();
			config.paymentEnabledIOS = d["paymentIOS"].GetBool();
			config.zone = d["name"].GetString();
			config.host = d["host"].GetString();
			config.port = d["port"].GetInt();
			config.websocket = d["port"].GetInt();
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
		} catch (exception e) {
			showPopupNotice(Utils::getSingleton().getStringForKey("error_unknown"), [=]() {});
			return;
		}

		string verstr = Application::sharedApplication()->getVersion();
		int i = verstr.find_last_of('.') + 1;
		verstr = verstr.substr(i, verstr.length() - i);
		int nver = atoi(verstr.c_str());
		config.paymentEnabled &= config.version > nver;
		config.paymentEnabledIOS &= config.versionIOS > nver;

		Utils::getSingleton().gameConfig = config;
        Utils::getSingleton().queryIAPProduct();
		labelPhone->setString(config.phone);
		//string location = Utils::getSingleton().getUserCountry();
		//Utils::getSingleton().gameConfig.paymentEnabled = config.paymentEnabled && location.compare("vn") == 0;

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
	}
	//hideWaiting();
}

void LoginScene::onHttpResponseFailed()
{
	if (currentConfigLink == 0) {
		currentConfigLink = 1;
		SFSRequest::getSingleton().RequestHttpGet("http://125.212.207.71/config/configChan.txt", 1);
	} else {
		hideWaiting();
		showPopupNotice(Utils::getSingleton().getStringForKey("error_connection"), [=]() {});
	}
}

void LoginScene::onTableDataResponse(LobbyListTable data)
{
	Utils::getSingleton().goToLobbyScene();
}

void LoginScene::onKeyBack()
{
	Director::sharedDirector()->end();
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

	tfResUname = ui::EditBox::create(Size(440, 55), "login/box.png");
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

	tfResPass = ui::EditBox::create(Size(440, 55), "login/box.png");
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

	tfResPassAgain = ui::EditBox::create(Size(440, 55), "login/box.png");
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

	ui::Button* btnBack = ui::Button::create("login/btn_back.png", "login/btn_back_clicked.png");
	btnBack->setPosition(Vec2(-110, -130));
	addTouchEventListener(btnBack, [=]() {
		loginNode->setVisible(true);
		registerNode->setVisible(false);
	});
	registerNode->addChild(btnBack);

	ui::Button* btnRegister = ui::Button::create("login/btn_register.png", "login/btn_register_clicked.png");
	btnRegister->setPosition(Vec2(110, -130));
	addTouchEventListener(btnRegister, [=]() {
		if (Utils::getSingleton().gameConfig.phone.length() == 0) {
			requestGameConfig();
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

void LoginScene::requestGameConfig()
{
	showWaiting();
	//SFSRequest::getSingleton().RequestHttpGet("http://125.212.207.71/config/configChan.txt", 1);
	SFSRequest::getSingleton().RequestHttpGet("http://125.212.192.96:8899/configchan.txt", 1);
}

void LoginScene::loadTextureCache()
{
	TextureCache::sharedTextureCache()->addImage("loadingicon.png");
	//TextureCache::sharedTextureCache()->addImage("popup/bg.png");
	//TextureCache::sharedTextureCache()->addImage("popup/bg1.png");
	TextureCache::sharedTextureCache()->addImage("popup/title_thongbao.png");
	TextureCache::sharedTextureCache()->addImage("popup/btn_submit.png");
	TextureCache::sharedTextureCache()->addImage("popup/btn_submit_clicked.png");
	TextureCache::sharedTextureCache()->addImage("main/nhatranh.png");
	TextureCache::sharedTextureCache()->addImage("main/dinhlang.png");
	TextureCache::sharedTextureCache()->addImage("main/phuchua.png");
	TextureCache::sharedTextureCache()->addImage("main/loidai.png");
}
