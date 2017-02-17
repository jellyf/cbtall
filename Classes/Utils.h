#pragma once
#include "cocos2d.h"
#include "SingLeton.h"
#include "Data.h"

class Utils : public SingLeton<Utils>
{
public:
	Utils();
	~Utils();

	static Utils& getSingleton(void);
	static Utils* getSingletonPtr(void);
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    std::string formatMoneyWithComma(long money);
#endif
    
    std::string formatMoneyWithComma(double money);
	std::string getStringForKey(std::string key);
	std::string replaceString(std::string str, std::string strSearch, std::string strReplace);
	std::string trim(std::string str);
	std::string getUserCountry();
	std::string getCurrentSystemTimeString();
	std::string getPlatformOS();
	double getCurrentSystemTimeInSecs();
	int getCurrentZoneIndex();
	bool isEmailValid(std::string email);
	bool isUsernameValid(std::string username);
	bool isPasswordValid(std::string password);
	bool isDisplayNameValid(std::string displayname);
	bool isPaymentEnabled();
	void split(const std::string &s, char delim, std::vector<std::string> &elems);
	void replaceScene(cocos2d::Scene* scene);
	void goToLoginScene();
	void goToMainScene();
	void goToLobbyScene();
	void goToGameScene();
	void saveUsernameAndPassword(std::string username, std::string password);
	void logoutGame();
	void logoutZone();
	void LoadTextureFromURL(std::string url, std::function<void(cocos2d::Texture2D*)> func);
	void onLoadTextureResponse(std::string url, cocos2d::Texture2D* texture);
	void openSMS(std::string number, std::string text);
	void openTel(std::string number);
	void loginFacebook();
	void logoutFacebook();
	void onInitSceneCompleted();
	void reconnect();
	void reloginZone();
	void rejoinRoom();
	void connectZoneByIndex(int moneyType, int index);
	void loginZoneByIndex(int moneyType, int index);
    void purchaseItem(std::string sku);
    void solveCachedPurchases();
    void setIAPProducts(std::vector<ProductData> vecProducts);
    void queryIAPProduct();
public:
	cocos2d::Scene* currentScene;
	UserData userDataMe;
	GameConfig gameConfig;
	LobbyListTable lobbyListTable;
	LobbyListRoomType lobbyListRoomType;
	TableReconnectData tableReconnectData;
    std::vector<std::vector<ZoneData>> zones;
    std::vector<ProductData> products;
	std::vector<EventData> events;
    std::vector<std::string> cachedPaymentTokens;
	std::string username;
	std::string password;
	std::string currentLobbyName;
	std::string currentRoomName;
	std::string currentZoneName;
	std::string currentZoneIp;
	long currentZonePort;
	int currentRoomId;
	int currentLobbyId;
	int loginType;
	int currentEventPosX;
	int moneyType;
	bool isRunningEvent;
	bool SoundEnabled;
	bool isWaitingScene;
	bool IgnoreInvitation;
private:
	cocos2d::ValueMap viLang;
	std::map<std::string, cocos2d::Texture2D*> textures;
	std::map<std::string, std::vector<std::function<void(cocos2d::Texture2D*)>>> callbacks;
};

#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
template<typename T>
std::string to_string(const T& t) {
	std::ostringstream os;
	os << t;
	return os.str();
}
#endif
