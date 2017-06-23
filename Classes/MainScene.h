#pragma once
#include "BaseScene.h"
#include "Data.h"

class MainScene : public BaseScene
{
public:
	CREATE_FUNC(MainScene);
	virtual void onInit();
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();
	virtual void onDownloadedPlistTexture(int numb);

	void onConfigZoneReceived();
	void onConnectionLost(std::string reason);
	void onJoinRoom(long roomId, std::string roomName);
	void onJoinRoomError(std::string msg);
	void onTableDataResponse(LobbyListTable data);
	void onShopHistoryDataResponse(std::vector<ShopHistoryData> list);
	void onShopItemsDataResponse(std::vector<ShopItemData> list);
	void onExchangeItemResponse(std::string msg);
	void onNewMailResponse(unsigned char count);
	void onListMailDataResponse(std::vector<MailData> list);
	void onMailContentResponse(std::string content);
	void onNewsDataResponse(std::vector<NewsData> list);
	void onPurchaseSuccess(std::string token);
	void onFacebookInvite(std::string token);
	void onDynamicConfigReceived();
protected:
	virtual bool onKeyBack();
	virtual void onConnected();
	virtual void onLoginZoneError(short int code, std::string msg);
	virtual void onBackScene();
	virtual void onChangeMoneyType(int type);
	virtual void onErrorResponse(unsigned char code, std::string msg);
private:
	void initPopupCharge();
	void initPopupGuide();
	void initPopupMail();
	void initPopupNews();
	void initPopupShop();
	void initPopupGiftcode();
	void initPopupDisplayName();

	void showPopupMail();
	void showPopupNews();
	void checkProviderToCharge();
	void updateChargeRateCard(bool isQuan);
	void updateSmsInfo(bool isQuan);

	cocos2d::Node* popupShop;
	cocos2d::Node* popupMail;
	cocos2d::Node* popupNews;
	cocos2d::Node* popupGuide;
	cocos2d::Node* popupCharge;
	cocos2d::Node* popupGiftcode;

	cocos2d::Label* lbNewMail;
	cocos2d::ui::Button* btnEvent;

	int currentMoneyType = 0;
	int tmpZoneId = -1;
	bool isBackToLogin = false;
	bool isGoToLobby = false;
	bool isWaitPopupNews = false;
	bool isWaitPopupMail = false;
	bool isChargeQuan = true;
	bool isEventReady = false;
	std::vector<ShopItemData> listItems;
	std::map<std::string, cocos2d::Texture2D*> textures;
	std::vector<int> moneys = { 10, 20, 50, 100, 200, 500 };
	std::vector<int> moneyxs = { 50, 100, 250, 500, 1000, 2500 };
	std::vector<std::string> strProviders;
};

