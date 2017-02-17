#pragma once
#include "BaseScene.h"
#include "Data.h"

class MainScene : public BaseScene, public cocos2d::ui::EditBoxDelegate
{
public:
	CREATE_FUNC(MainScene);
	virtual void onInit();
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();
	virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);

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
protected:
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
	void initWebView();
	void initPopupGiftcode();
	void initPopupDisplayName();

	void showPopupMail();
	void showPopupNews();
	void showWebView(std::string url);

	cocos2d::Node* popupShop;
	cocos2d::Node* popupMail;
	cocos2d::Node* popupNews;
	cocos2d::Node* popupGuide;
	cocos2d::Node* popupCharge;
	cocos2d::Node* popupGiftcode;
	cocos2d::Node* popupDisplayName;
	cocos2d::Node* nodeWebview;

	cocos2d::Label* lbNewMail;

	int currentMoneyType = 0;
	int tmpZoneId = -1;
	bool isBackToLogin = false;
	bool isGoToLobby = false;
	std::string tmpDisplayName;
	std::vector<ShopItemData> listItems;
	std::map<std::string, cocos2d::Texture2D*> textures;
};

