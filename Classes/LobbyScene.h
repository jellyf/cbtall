#pragma once
#include "BaseScene.h"
#include "Data.h"

class LobbyScene : public BaseScene
{
public:
	CREATE_FUNC(LobbyScene);
	virtual void onInit();
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();

	void onConfigZoneReceived();
	void onInviteDataResponse(InviteData data);
	void onTableDataResponse(LobbyListTable data);
	void onRoomTypeDataResponse(LobbyListRoomType data);
	void onTableReconnectDataResponse(TableReconnectData data);
	void onPurchaseSuccess(std::string token);
	void onListTourPlayersResponse(std::vector<TourPlayer> players);
	void onTopTourPlayersResponse(std::vector<TourRoom> rooms);
	void onTourNewRound(std::string room);
protected:
	virtual bool onKeyBack();
	virtual void onConnected();
	virtual bool onConnectionLost(std::string reason);
	virtual void onLoginZone();
	virtual void onLoginZoneError(short int code, std::string msg);
	virtual void onBackScene();
	virtual void onChangeMoneyType(int type);
	virtual bool onErrorResponse(unsigned char code, std::string msg);

	void initTourView();
	void updateTableText(cocos2d::Node* table, std::vector<std::vector<std::string>> texts);
	cocos2d::Node* drawTable(int numOfRows, int rowHeight, std::vector<int> widths, std::vector<std::string> titles, std::vector<std::vector<std::string>> texts);

	bool isSolo;
	bool isTour;
	bool isBackToMain = false;
	bool isChangeMoney = false;
	int currentRoomType;
	int currentMoneyType;
	std::string tmpZoneName;
	LobbyListRoomType listRoomData;

	cocos2d::Node* nodeTour;
	cocos2d::ui::ScrollView* scrollListRoom;
	cocos2d::ui::ScrollView* scrollListTable;
	std::vector<cocos2d::ui::Button*> vecTables;
	std::vector<cocos2d::Vec2> playerPos;
	std::vector<TourPlayer> listPlayers;
	std::map<std::string, std::string> mapPlayerRooms;
};

