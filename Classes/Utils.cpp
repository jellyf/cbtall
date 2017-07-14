#pragma once
#include "Utils.h"
#include "LoginScene.h"
#include "MainScene.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "Constant.h"
#include "SFSRequest.h"
#include "Tracker.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "IOSHelperCPlus.h"
#endif

using namespace std;
using namespace cocos2d;

template<> Utils* SingLeton<Utils>::mSingleton = 0;
Utils* Utils::getSingletonPtr(void)
{
	return mSingleton;
}
Utils& Utils::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

Utils::Utils()
{
	cofferMoney = 0;
	moneyType = -1;
	userDataMe.UserID = 0;
	downloadedPlistTexture = 0;
	allowEventPopup = true;
	isRunningEvent = false;
	gameConfig.pmE = false;
	gameConfig.pmEIOS = false;
	dynamicConfig.Popup = false;
	hasShowEventPopup = false;
	currentEventPosX = constant::EVENT_START_POSX;
	textureHost = "http://115.84.179.242/main_kinhtuchi/";
	cofferGuide = "";
	viLang = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/vi.xml");
	SFSRequest::getSingleton().onLoadTextureResponse = std::bind(&Utils::onLoadTextureResponse, this, std::placeholders::_1, std::placeholders::_2);

	TextureCache::sharedTextureCache()->addImageAsync("tmp.png", [=](Texture2D* texture) {
		string str1 = FileUtils::getInstance()->getStringFromFile("menu1.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(str1, texture);
		string str2 = FileUtils::getInstance()->getStringFromFile("menu2.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(str2, texture);
	});

	createAppellations();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSHelperCPlus::setLoginFBCallback([=](std::string token){
        if(EventHandler::getSingleton().onLoginFacebook != NULL){
            EventHandler::getSingleton().onLoginFacebook(token);
        }
    });
    IOSHelperCPlus::setPurchaseSuccessCallback([=](std::string token){
        Utils::onCallbackPurchaseSuccess(token);
    });
    IOSHelperCPlus::setFacebookInviteCallback([=](std::string token){
        if (EventHandler::getSingleton().onFacebookInvite != NULL) {
            EventHandler::getSingleton().onFacebookInvite(token);
        }
    });
#endif
}

Utils::~Utils()
{
}

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
string Utils::formatMoneyWithComma(long lmoney) {
    int money = lmoney;
    if(money == 0) return "0";

    bool isNegative = money < 0;
    if(money < 0) money = -money;
    string ret = "";
    while(money >= 1){
        ret = (char)(money % 10 + 48) + ret;
        money /= 10;
    }
    if(isNegative) ret = '-' + ret;
    
    unsigned int i = 3;
    while (i < ret.length())
    {
        if (ret[ret.length() - i - 1] == '-') break;
        ret.insert(ret.length() - i, ".");
        i = i + 3 + 1;
    }
    return ret;
}
#endif

cocos2d::SpriteFrame* Utils::getDownloadedTextureAsSpriteFrame(std::string key)
{
	Texture2D* texture = TextureCache::getInstance()->getTextureForKey(key);
	Size size = texture->getContentSize();
	return SpriteFrame::createWithTexture(texture, Rect(0, 0, size.width, size.height));
}

AppellationData& Utils::getAppellationByLevel(int level)
{
	int i = 0;
	while (i < appellations.size() && level >= appellations[i].Level) i++;
	return appellations[level == 0 ? 0 : i - 1];
}

string Utils::formatMoneyWithComma(double money) {
	stringstream ss;
	//ss.imbue(locale("de-VI"));
	ss << fixed << money;
	string ret = ss.str();
	int index = ret.find_last_of('.');
	ret = ret.substr(0, index);
    
	unsigned int i = 3;
	while (i < ret.length())
	{
		if (ret[ret.length() - i - 1] == '-') break;
		ret.insert(ret.length() - i, ",");
		i = i + 3 + 1;
	}
	return ret;
}

std::string Utils::getStringForKey(std::string key)
{
	return viLang[key].asString();
}

std::string Utils::replaceString(std::string str, std::string strSearch, std::string strReplace)
{
	std::string s = str;
	for (size_t pos = 0; ; pos += strReplace.length()) {
		pos = s.find(strSearch, pos);
		if (pos == string::npos)
			break;

		s.erase(pos, strSearch.length());
		s.insert(pos, strReplace);
	}
	return s;
}

std::string Utils::trim(std::string str)
{
	if (str.length() == 0) {
		return str;
	}
	int i = 0;
	int j = str.length() - 1;
	while (i < str.length() && str.at(i) == ' ') i++;
	while (j >= 0 && str.at(j) == ' ') j--;
	return str.substr(i, j + 1);
}

std::string Utils::getUserCountry()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "getUserCountry", "()Ljava/lang/String;")) {
		return "vn";
	}
	jstring s = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);

	std::string str = JniHelper::jstring2string(s);
	return str;
#else
	return "vn";
#endif
}

std::string Utils::getCurrentSystemTimeString()
{
	timeval time;
	gettimeofday(&time, NULL);
	return String::createWithFormat("%d %d", time.tv_sec, time.tv_usec)->getCString();
}

std::string Utils::getPlatformOS()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return "ios";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return "android";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "android";// "win32";
#else
	return "unknown";
#endif
}

std::string Utils::getDeviceId()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return IOSHelperCPlus::getDeviceId();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "getDeviceId", "()Ljava/lang/String;")) {
		return "unknown";
	}
	jstring s = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);

	std::string str = JniHelper::jstring2string(s);
	return str;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "win32";
#else
	return "unknown";
#endif
}

std::string Utils::getCurrentZoneName()
{
	/*if (currentZoneName == "SoLoXu")
		return "NhaTranh";
	else if (currentZoneName == "SoLoQuan")
		return "NhaTranhQuan";*/
	return currentZoneName;
}

double Utils::getCurrentSystemTimeInSecs()
{
	timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec;
}

int Utils::getCurrentZoneIndex()
{
	for (int i = 0; i < zones[moneyType].size(); i++) {
		if (zones[moneyType][i].ZoneName.compare(currentZoneName) == 0) {
			return moneyType * 10 + i;
		}
	}
}

bool Utils::isEmailValid(std::string email)
{
	int length = email.length();
	for (int i = 0; i < length; i++) {
		char c = email.at(i);
		if (!(c == '.' || c == '_' || c == '@' || c == '-' || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
			return false;
		}
	}
	return true;
}

bool Utils::isUsernameValid(std::string uname)
{
	int length = uname.length();
	if (length < 4 || length > 16) {
		return false;
	}
	for (int i = 0; i < length; i++) {
		char c = uname.at(i);
		if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
			return false;
		}
	}
	return true;
}

bool Utils::isPasswordValid(std::string pw)
{
	int length = pw.length();
	if (length < 6 || length > 16) {
		return false;
	}
	for (int i = 0; i < length; i++) {
		char c = pw.at(i);
		if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
			return false;
		}
	}
	return true;
}

bool Utils::isDisplayNameValid(std::string displayname)
{
	int length = displayname.length();
	if (length < 6 || length > 16) {
		return false;
	}
	for (int i = 0; i < length; i++) {
		char c = displayname.at(i);
		if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
			return false;
		}
	}
	return true;
}

bool Utils::ispmE()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return gameConfig.pmEIOS;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return gameConfig.pmE;
#else
	return true;
#endif
}

bool Utils::isSoloGame()
{
	return currentZoneName.substr(0, 4).compare("SoLo") == 0;
}

void Utils::setPmEByLogin(bool pme)
{
	gameConfig.pmE &= pme;
	gameConfig.pmEIOS &= pme;
}

void Utils::split(const std::string & s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

void Utils::replaceScene(cocos2d::Scene* newScene) {
	callbacks.clear();
	if (currentScene != nullptr) {
		currentScene->unscheduleUpdate();
	}
	isWaitingScene = true;
	currentScene = newScene;
	cocos2d::Director::getInstance()->replaceScene(newScene);
}

void Utils::goToLoginScene()
{
	logoutGame();
	replaceScene(LoginScene::create());
}

void Utils::goToMainScene()
{
	replaceScene(MainScene::create());
}

void Utils::goToLobbyScene()
{
	replaceScene(LobbyScene::create());
}

void Utils::goToGameScene()
{
	replaceScene(GameScene::create());
}

void Utils::saveUsernameAndPassword(std::string username, std::string password)
{
	this->username = username;
	this->password = password.length() == 0 ? "" : md5(password);
	if (password.length() > 0) {
		UserDefault::getInstance()->setStringForKey(constant::KEY_USERNAME.c_str(), username);
		UserDefault::getInstance()->setStringForKey(constant::KEY_PASSWORD.c_str(), password);
	}
}

void Utils::logoutGame()
{
	moneyType = -1;
	userDataMe.UserID = 0;
	userDataMe.Name = "";
	logoutZone();
	hasShowEventPopup = false;
}

void Utils::logoutZone()
{
	currentRoomId = 0;
	currentLobbyId = 0;
	currentZoneName = "";
	currentRoomName = "";
	currentLobbyName = "";
	lobbyListTable.Size = 0;
	lobbyListRoomType.ListRoomType.clear();
}

void Utils::LoadTextureFromURL(std::string url, std::function<void(cocos2d::Texture2D*)> func, std::string textureName)
{
	if (textures[url] != nullptr) {
		func(textures[url]);
	} else {
		callbacks[url].push_back(func);
		if (callbacks[url].size() == 1) {
			SFSRequest::getSingleton().LoadTextureFromURL(url);
		}
	}
}

void Utils::onLoadTextureResponse(std::string url, cocos2d::Texture2D * texture)
{
	textures[url] = texture;
	for (auto func : callbacks[url]) {
		func(texture);
	}
	callbacks[url].clear();
}

void Utils::openSMS(std::string number, std::string text)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "openSMS", "(Ljava/lang/String;Ljava/lang/String;)V")) {
		return;
	}
	jstring jaddress = methodInfo.env->NewStringUTF(number.c_str());
	jstring jsmsBody = methodInfo.env->NewStringUTF(text.c_str());
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jaddress, jsmsBody);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSHelperCPlus::openSMS(number.c_str(), text.c_str());
#endif
}

void Utils::openTel(std::string number)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "openCall", "(Ljava/lang/String;)V")) {
		return;
	}
	jstring jphone = methodInfo.env->NewStringUTF(number.c_str());
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jphone);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSHelperCPlus::openTel(number.c_str());
#endif
}

void Utils::loginFacebook()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "loginFacebook", "()V")) {
		return;
	}
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSHelperCPlus::loginFacebook();
#endif
}

void Utils::logoutFacebook()
{
	if (loginType == constant::LOGIN_FACEBOOK) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo methodInfo;
		if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "logoutFacebook", "()V")) {
			return;
		}
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		IOSHelperCPlus::logoutFacebook();
#endif
	}
}

void Utils::onInitSceneCompleted()
{
	isWaitingScene = false;
}

void Utils::reconnect()
{
	vector<vector<ZoneData>> zones = Utils::getSingleton().zones;
	for (vector<ZoneData> v : zones) {
		for (ZoneData z : v) {
			if (z.ZoneName.compare(currentZoneName) == 0) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				SFSRequest::getSingleton().Connect(z.ZoneIpIos, z.ZonePort);
#else
				SFSRequest::getSingleton().Connect(z.ZoneIp, z.ZonePort);
#endif
				return;
			}
		}
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	SFSRequest::getSingleton().Connect(gameConfig.ip_rs, gameConfig.port);
#else
	SFSRequest::getSingleton().Connect(gameConfig.host, gameConfig.port);
#endif
}

void Utils::reloginZone()
{
	SFSRequest::getSingleton().LoginZone(username, password, currentZoneName);
}

void Utils::rejoinRoom()
{
	SFSRequest::getSingleton().RequestJoinRoom(currentRoomName, true);
}

void Utils::connectZoneByIndex(int moneyType, int index)
{
	if (moneyType < 0 || moneyType >= zones.size())
		moneyType = 0;
	if (zones.size() == 0) {
		return;
	}
	if (index < 0 || index >= zones[moneyType].size())
		index = 0;
	if (zones[moneyType].size() == 0) {
		return;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	std::string host = zones[moneyType][index].ZoneIpIos;
#else
	std::string host = zones[moneyType][index].ZoneIp;
#endif
	SFSRequest::getSingleton().Connect(host, zones[moneyType][index].ZonePort);
}

void Utils::loginZoneByIndex(int moneyType, int index)
{
	long zonePort =zones[moneyType][index].ZonePort;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	std::string zoneIp = zones[moneyType][index].ZoneIpIos;
#else
	std::string zoneIp = zones[moneyType][index].ZoneIp;
#endif
	std::string zoneName = zones[moneyType][index].ZoneName;
	currentZonePort = zonePort;
	currentZoneIp = zoneIp;
	currentZoneName = zoneName;
	//CCLOG("%d %s", index, zoneName.c_str());
	CCLOG("loginZoneByIndex: %s", username.c_str());
	SFSRequest::getSingleton().LoginZone(username, password, zoneName);
}

void Utils::purchaseItem(std::string sku)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSHelperCPlus::purchaseItem(sku);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "purchaseProduct", "(Ljava/lang/String;)V")) {
		return;
	}
	jstring jsku = methodInfo.env->NewStringUTF(sku.c_str());
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jsku);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);
#else
    if(EventHandler::getSingleton().onPurchaseSuccess != NULL){
        EventHandler::getSingleton().onPurchaseSuccess("");
    }
#endif
}

void Utils::solveCachedPurchases()
{
    if(cachedPaymentTokens.size() == 0) return;
    for(std::string token : cachedPaymentTokens){
        SFSRequest::getSingleton().RequestPayment(token, ispmE());
    }
    cachedPaymentTokens.clear();
}

void Utils::setIAPProducts(std::vector<ProductData> vecProducts)
{
    for(int i = 0;i<products.size();i++){
        for(ProductData p : vecProducts){
            if(products[i].Id.compare(p.Id) == 0){
                products[i].Price = p.Price;
                products[i].Description = p.Description;
                products[i].CurrencySymbol = p.CurrencySymbol;
                break;
            }
        }
    }
	if (EventHandler::getSingleton().onIAPProductReady != NULL) {
		EventHandler::getSingleton().onIAPProductReady();
	}
}

void Utils::queryIAPProduct()
{
	std::vector<std::string> ids;
    split(gameConfig.inapp, ',', ids);
	std::vector<std::string> descs = { "10 ", "20 ", "30 " };
	std::vector<double> prices = { 22000, 105000, 210000 };
    for(int i=0;i<ids.size();i++){
        ids[i] = "com.chan.kinhtuchi.goi." + ids[i];
        ProductData data;
        data.Id = ids[i];
		data.Price = prices[i];
		data.Description = descs[i];
		data.CurrencySymbol = "vnd";
        products.push_back(data);
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSHelperCPlus::queryIAPProducts(ids);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "queryIAPProducts", "([Ljava/lang/String;)V")) {
		return;
	}
	jobjectArray arr = methodInfo.env->NewObjectArray(5, methodInfo.env->FindClass("java/lang/String"), methodInfo.env->NewStringUTF(""));
	jsize count = ids.size();
	for (int i = 0; i < count; ++i) {
		methodInfo.env->SetObjectArrayElement(arr, i, methodInfo.env->NewStringUTF(ids[i].c_str()));
	}
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arr);
	methodInfo.env->DeleteLocalRef(arr);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);
#else
	if (EventHandler::getSingleton().onIAPProductReady != NULL) {
		EventHandler::getSingleton().onIAPProductReady();
	}
#endif
}

void Utils::downloadPlistTextures()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (downloadedPlistTexture == 0) {
		string str1 = FileUtils::getInstance()->getStringFromFile("menu1.plist");
		Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA8888);
		Utils::getSingleton().LoadTextureFromURL(textureHost + "menu3.png", [=](Texture2D* texture1) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(str1, texture1);
			downloadedPlistTexture = 1;
			if (EventHandler::getSingleton().onDownloadedPlistTexture != NULL) {
				EventHandler::getSingleton().onDownloadedPlistTexture(downloadedPlistTexture);
			}
			Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
			string str2 = FileUtils::getInstance()->getStringFromFile("menu2.plist");
			Utils::getSingleton().LoadTextureFromURL(textureHost + "menu4.png", [=](Texture2D* texture2) {
				SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(str2, texture2);
				downloadedPlistTexture = 2;
				if (EventHandler::getSingleton().onDownloadedPlistTexture != NULL) {
					EventHandler::getSingleton().onDownloadedPlistTexture(downloadedPlistTexture);
				}

				Utils::getSingleton().LoadTextureFromURL(textureHost + "hu.png", [=](Texture2D* texture3) {
					Utils::getSingleton().LoadTextureFromURL(textureHost + "as.png", [=](Texture2D* texture4) {});
				});
			});
		});
	} else if (downloadedPlistTexture == 1) {
		string str2 = FileUtils::getInstance()->getStringFromFile("menu2.plist");
		Utils::getSingleton().LoadTextureFromURL(textureHost + "menu4.png", [=](Texture2D* texture) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(str2, texture);
			downloadedPlistTexture = 2;
			if (EventHandler::getSingleton().onDownloadedPlistTexture != NULL) {
				EventHandler::getSingleton().onDownloadedPlistTexture(downloadedPlistTexture);
			}
		});
	} else {
		if (EventHandler::getSingleton().onDownloadedPlistTexture != NULL) {
			EventHandler::getSingleton().onDownloadedPlistTexture(downloadedPlistTexture);
		}
	}
#else
	Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA8888);
	TextureCache::sharedTextureCache()->addImageAsync("menu1.png", [=](Texture2D* texture) {
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("menu1.plist");
		downloadedPlistTexture = 1;
		if (EventHandler::getSingleton().onDownloadedPlistTexture != NULL) {
			EventHandler::getSingleton().onDownloadedPlistTexture(downloadedPlistTexture);
		}
		Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
		TextureCache::sharedTextureCache()->addImageAsync("menu2.png", [=](Texture2D* texture) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("menu2.plist");
			downloadedPlistTexture = 2;
			if (EventHandler::getSingleton().onDownloadedPlistTexture != NULL) {
				EventHandler::getSingleton().onDownloadedPlistTexture(downloadedPlistTexture);
			}
		});
	});
#endif
}

void Utils::onCallbackPurchaseSuccess(std::string token)
{
	if (EventHandler::getSingleton().onPurchaseSuccess != NULL) {
		EventHandler::getSingleton().onPurchaseSuccess(token);
	} else if (token.length() > 0) {
		Utils::getSingleton().cachedPaymentTokens.push_back(token);
	}
}

void Utils::setUserDataMe(UserData myData)
{
	if (!userDataMe.IsActived && myData.IsActived) {
		Tracker::getSingleton().trackActiveAccount();
	}
	userDataMe = myData;
}

void Utils::inviteFacebookFriends()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	IOSHelperCPlus::inviteFacebookFriends();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/AppActivity", "inviteFacebookFriends", "()V")) {
		return;
	}
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);
#else
	return;
#endif
}

void Utils::createAppellations()
{
	std::vector<int> levelStones = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 18, 28, 40, 53, 70, 162, 287, 662 };
	std::vector<cocos2d::Color3B> levelColors = { Color3B(200, 0, 0), Color3B(255, 0, 0), Color3B(255, 0, 60), Color3B(255, 0, 120), Color3B(255, 0, 180), Color3B(255, 0, 240),
		Color3B(210, 0, 255), Color3B(0, 200, 255), Color3B(0, 255, 255), Color3B(0, 255, 200), Color3B(0, 255, 140), Color3B(0, 255, 80), Color3B(0, 255, 20),
		Color3B(30, 255, 0), Color3B(80, 255, 0), Color3B(130, 255, 0), Color3B(80, 255, 0), Color3B(225, 255, 0), Color3B(255, 225, 0) };
	std::vector<cocos2d::Color4B> levelOutlineColors = { Color4B(100, 0, 0, 255), Color4B(100, 0, 0, 255), Color4B(100, 0, 24, 255), Color4B(100, 0, 48, 255), Color4B(100, 0, 72, 255), Color4B(100, 0, 100, 255),
		Color4B(84, 0, 100, 255), Color4B(0, 80, 100, 255), Color4B(0, 100, 100, 255), Color4B(0, 100, 80, 255), Color4B(0, 100, 60, 255), Color4B(0, 100, 30, 255), Color4B(0, 100, 10, 255),
		Color4B(15, 100, 0, 255), Color4B(32, 100, 0, 255), Color4B(50, 100, 0, 255), Color4B(32, 100, 0, 255), Color4B(90, 100, 0, 255), Color4B(100, 100, 0, 255) };
	ValueMap aplMap = cocos2d::FileUtils::getInstance()->getValueMapFromFile("lang/level.xml");
	for (int i = 0; i < levelStones.size(); i++) {
		AppellationData apl;
		apl.Level = levelStones[i];
		apl.Color = levelColors[i];
		apl.ColorOutline = levelOutlineColors[i];
		apl.Name = aplMap["level_" + to_string(i)].asString();
		appellations.push_back(apl);
	}
}
