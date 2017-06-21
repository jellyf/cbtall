#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Data.h"

class BaseScene : public cocos2d::Scene
{
public:
	BaseScene();
	~BaseScene();
	virtual bool init();
	virtual void onInit();
	virtual void onEnter();
	virtual void onExit();
	virtual void update(float delta);
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();
	virtual void onDownloadedPlistTexture(int numb);
	virtual void onHttpResponse(int tag, std::string content);
	virtual void onHttpResponseFailed();

	CC_SYNTHESIZE(bool, mIsTouch, IsTouch);
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* _event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* _event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* _event);

	virtual void onApplicationDidEnterBackground();
	virtual void onApplicationWillEnterForeground();

	void onPingPong(long timems);
	void onUserDataMeResponse();
	void onRankDataResponse(std::vector<std::vector<RankData>> list);
	void onRankWinDataResponse(std::vector<RankWinData> list);
	void onListEventDataResponse(std::vector<EventData> list);
	void onPlayLogDataResponse(std::vector<PlayLogData> logs);
	void onCofferMoneyResponse(long money);
	void onCofferHistoryResponse(std::vector<CofferWinnerData> list);
protected:
	virtual void onConnected();
	virtual void onConnectionFailed();
	virtual void onLoginZone();
	virtual void onLoginZoneError(short int code, std::string msg);
	virtual void onErrorResponse(unsigned char code, std::string msg);
	virtual void initHeaderWithInfos();
	virtual void onBackScene();
	virtual void onChangeMoneyType(int type);
	virtual bool onKeyBack();
	virtual void onKeyHome();

	void pauseApplication();
	void handleClientDisconnectionReason(std::string reason);
	void addTouchEventListener(cocos2d::ui::Button* btn, std::function<void()> func, bool isNew = true);
	void hideSplash();
	void hideWaiting();
	void hidePopup(cocos2d::Node* popup, bool runEffect = true);
	void initEventView(cocos2d::Vec2 pos, cocos2d::Size size);
	void initPopupRank();
	void initPopupSettings();
	void initPopupUserInfo();
	void initPopupHistory();
	void initPopupCoffer();
	void initPopupIAP();
	void initCofferView(cocos2d::Vec2 pos, int zorder, float scale = 1.0f);
	void setMoneyType(int type);
	void showPopupNotice(std::string msg, std::function<void()> func, bool showBtnClose = true, int timeToHide = -1);
	void showPopupRank(int type);
	void showPopupRankWin();
	void showPopupUserInfo(UserData userData, bool showHistoryIfIsMe = true);
	void showPopupHistory();
	void showPopupCoffer();
	void showSplash();
	void showToast(std::string tag, std::string msg, cocos2d::Vec2 pos, cocos2d::Color3B textColor = cocos2d::Color3B::WHITE, cocos2d::Color3B bgColor = cocos2d::Color3B(80, 80, 80), int bgOpacity = 200);
	void showWaiting(int time = 10);
	void showPopup(cocos2d::Node* popup, bool runEffect = true);
	void setDisplayName(std::string name);
	void runEffectHidePopup(cocos2d::Node* popup);
	void runEffectShowPopup(cocos2d::Node* popup);
	void runEventView(std::vector<EventData> list, int currentPosX = 1500);
	void addBtnChoosePage(int x, int y, cocos2d::Node* node, std::function<void(int)> funcPage);
	void setSplashZOrder(int zorder);
    void autoScaleNode(cocos2d::Node* node);
    void delayFunction(Node* node, float time, std::function<void()> func);

	cocos2d::Node* createPopup(std::string stitle, bool isBig, bool isHidden);
	cocos2d::Node* createPopupDetail();
	cocos2d::Node* createPopupNotice();
	cocos2d::Vec2 getScaleSmoothly(float scale);

	std::string chargingProvider = "";
	bool isReconnecting = false;
	bool isOverlapLogin = false;
	bool hasHeader = false;
	bool isWaiting = false;
	bool isPopupReady = false;
	bool isBackToLogin = false;
	int pingId = 0;
	double myRealMoney;

	cocos2d::Vec2 scaleScene;
	cocos2d::Size winSize;

	cocos2d::ui::Scale9Sprite* splash;
	cocos2d::ui::Scale9Sprite* chosenBg;
	cocos2d::ui::Scale9Sprite* spMoneyBg;
	cocos2d::ui::Button* moneyBg0;
	cocos2d::ui::Button* moneyBg1;
	cocos2d::Sprite* spWaiting;
	cocos2d::Sprite* spNetwork;
	cocos2d::Layer* mLayer;
	cocos2d::Label* lbGold;
	cocos2d::Label* lbSilver;
	cocos2d::Label* lbName;
	cocos2d::Label* lbId;
	cocos2d::Label* lbLevel;
	cocos2d::Label* lbNetwork;
	cocos2d::Label* lbCoffer;

	cocos2d::Node* popupRank;
	cocos2d::Node* popupMainSettings;
	cocos2d::Node* popupUserInfo;
	cocos2d::Node* popupHistory;
	cocos2d::Node* popupCoffer;
	cocos2d::Node* popupIAP;
	cocos2d::Node* eventView;

	std::vector<cocos2d::ui::Button*> buttons;
	std::vector<cocos2d::ui::Button*> blockedButtons;
	std::vector<cocos2d::Node*> popups;

	cocos2d::Color3B colorGold = cocos2d::Color3B(179, 179, 0);
	cocos2d::Color3B colorSilver = cocos2d::Color3B(0, 179, 179);

	cocos2d::Vector<Node*> tmps;
	int tmpIndex;

private:
    bool isPauseApp = false;
    bool isShowDisconnected = false;
	std::vector<RankWinData> listRankWin;
	std::vector<std::vector<RankData>> listRanks;
	cocos2d::Vector<Node*> vecPopupNotices;
	cocos2d::Vector<Node*> vecPopupDetails;
};

