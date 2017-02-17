#pragma once
#include "cocos2d.h"
#include "SingLeton.h"
#include "Data.h"

class EventHandler : public SingLeton <EventHandler>
{

public:
	EventHandler();
	~EventHandler();

	static EventHandler& getSingleton(void);
	static EventHandler* getSingletonPtr(void);

	std::function<void()> onApplicationDidEnterBackground = NULL;
	std::function<void()> onApplicationWillEnterForeground = NULL;

	std::function<void(std::string)> onLoginFacebook = NULL;
    std::function<void(std::string)> onPurchaseSuccess = NULL;

	std::function<void()> onConnected = NULL;
	std::function<void()> onLoginZone = NULL;
	std::function<void()> onLogoutZone = NULL;
	std::function<void()> onConnectionFailed = NULL;
	std::function<void()> onConfigZoneReceived = NULL;
	std::function<void()> onUserDataMeSFSResponse = NULL;
	std::function<void(std::string)> onDisconnected = NULL;
	std::function<void(std::string)> onConnectionLost = NULL;
	std::function<void(UserData)> onUserDataSFSResponse = NULL;
	std::function<void(long)> onUserExitRoom = NULL;
	std::function<void(long)> onPingPong = NULL;
	std::function<void(short int, std::string)> onLoginZoneError = NULL;
	std::function<void(long, std::string)> onPublicMessage = NULL;
	std::function<void(long, std::string)> onJoinRoom = NULL;
	std::function<void(std::string)> onJoinRoomError = NULL;
	std::function<void(std::vector<std::vector<RankData>>)> onRankDataSFSResponse = NULL;
	std::function<void(std::vector<RankWinData>)> onRankWinDataSFSResponse = NULL;
	std::function<void(std::vector<PlayLogData>)> onPlayLogDataSFSResponse = NULL;
	std::function<void(std::vector<ShopHistoryData>)> onShopHistoryDataSFSResponse = NULL;
	std::function<void(std::vector<ShopItemData>)> onShopItemsDataSFSResponse = NULL;
	std::function<void(std::string)> onExchangeItemSFSResponse = NULL;
	std::function<void(unsigned char)> onNewMailSFSResponse = NULL;
	std::function<void(std::vector<MailData>)> onListMailDataSFSResponse = NULL;
	std::function<void(std::string)> onMailContentSFSResponse = NULL;
	std::function<void(std::vector<NewsData>)> onNewsDataSFSResponse = NULL;
	std::function<void(std::vector<EventData>)> onListEventDataSFSResponse = NULL;
	std::function<void(TableReconnectData)> onTableReconnectDataSFSResponse = NULL;

	std::function<void(unsigned char, std::string)> onErrorSFSResponse = NULL;
	std::function<void(RoomData)> onRoomDataSFSResponse = NULL;
	std::function<void(StartGameData)> onStartGameDataSFSResponse = NULL;
	std::function<void(unsigned char)> onChooseStiltSFSResponse = NULL;
	std::function<void(unsigned char, unsigned char, unsigned char)> onChooseHostSFSResponse = NULL;
	std::function<void(BashData)> onUserBashSFSResponse = NULL;
	std::function<void(BashBackData)> onUserBashBackSFSResponse = NULL;
	std::function<void(HoldData)> onUserHoldSFSResponse = NULL;
	std::function<void(PickData)> onUserPickSFSResponse = NULL;
	std::function<void(ForwardData)> onUserForwardSFSResponse = NULL;
	std::function<void(PenetData)> onUserPenetSFSResponse = NULL;
	std::function<void(long, unsigned char)> onUserWinSFSResponse = NULL;
	std::function<void(CrestResponseData)> onCrestResponseSFSResponse = NULL;
	std::function<void(EndMatchData)> onEndMatchSFSResponse = NULL;
	std::function<void(EndMatchMoneyData)> onEndMatchMoneySFSResponse = NULL;
	std::function<void(std::vector<unsigned char>)> onEndMatchTieSFSResponse = NULL;
	std::function<void(long, std::string)> onGamePunishSFSResponse = NULL;
	std::function<void(long, bool)> onGameReadySFSResponse = NULL;
	std::function<void(GameTableData)> onGameTableSFSResponse = NULL;
	std::function<void(bool, double)> onGameRoomDataGaSFSResponse = NULL;
	std::function<void(PlayingTableData)> onGamePlayingDataSFSResponse = NULL;
	std::function<void(std::vector<PlayerData>)> onGameSpectatorDataSFSResponse = NULL;
	std::function<void(GameReconnectData)> onGameMyReconnectDataSFSResponse = NULL;
	std::function<void(std::vector<UserReconnectData>)> onGameUserReconnectDataSFSResponse = NULL;

	std::function<void(LobbyListTable)> onLobbyTableSFSResponse = NULL;
	std::function<void(LobbyListRoomType)> onLobbyRoomTypeSFSResponse = NULL;
	std::function<void(std::vector<UserData>)> onLobbyUserDataSFSResponse = NULL;
	std::function<void(InviteData)> onLobbyInviteDataSFSResponse = NULL;

public:
	static void callbackLoginFacebook(std::string token) {
		if (EventHandler::getSingleton().onLoginFacebook != NULL) {
			EventHandler::getSingleton().onLoginFacebook(token);
		}
	}
};

