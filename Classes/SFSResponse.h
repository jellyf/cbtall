#pragma once
#include "Data.h"
#include "SmartFox.h"
#include "SingLeton.h"

class SFSResponse : public SingLeton<SFSResponse> {
public:
	SFSResponse();
	~SFSResponse();

	static SFSResponse& getSingleton(void);
	static SFSResponse* getSingletonPtr(void);

	void onExtensionResponse(boost::shared_ptr<string> ptrNotifiedCmd, boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject);
	void RunCachedResponses();

private:
	void onPingResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onErrorResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onConfigZoneResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void getUserDataFromSFSObject(boost::shared_ptr<ISFSObject> isfsObject, UserData &userData);
	void onUserDataResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onUserDataMeResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onUpdateUserDataMeResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onRoomDataResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onStartGameResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGameChooseStiltResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGameChooseHostResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onBashResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onBashBackResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onHoldResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onPickResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onForwardResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onPenetResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onUserWinResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onCrestDataResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onEndMatchTieResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onEndMatchResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onEndMatchMoneyResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onLobbyRoomTypeResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onLobbyTableResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGamePunishResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGameReadyResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGameTableResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onLobbyUserResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onLobbyInviteResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGameRoomGaResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGamePlayingTableResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onPlayingCardFromJson(std::string json, std::vector<std::vector<char>> &vec);
	void onGameSpectatorResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onRankResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onRankWinResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onPlayLogResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGameMyReconnectResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onGameUserReconnectResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onShopHistoryResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onShopItemsResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onExchangeItemResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onNewMailResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onListMailResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onMailContentResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onNewsResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onListEventResponse(boost::shared_ptr<ISFSObject> isfsObject);
	void onTableReconnectResponse(boost::shared_ptr<ISFSObject> isfsObject);

	map<string, function<void(boost::shared_ptr<ISFSObject>)>> mapFunctions;
	vector<pair<boost::shared_ptr<string>, boost::shared_ptr<ISFSObject>>> cachedResponses;
};