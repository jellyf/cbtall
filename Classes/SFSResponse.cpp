#include "SFSResponse.h"
#include "Command.h"
#include "Utils.h"
#include "EventHandler.h"
#include "SFSRequest.h"
#include "Constant.h"

template<> SFSResponse* SingLeton<SFSResponse>::mSingleton = 0;
SFSResponse* SFSResponse::getSingletonPtr(void)
{
	return mSingleton;
}

SFSResponse& SFSResponse::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

SFSResponse::SFSResponse()
{
	mapFunctions["err"] = std::bind(&SFSResponse::onErrorResponse, this, std::placeholders::_1);
	mapFunctions[cmd::RESPONSE_PING] = std::bind(&SFSResponse::onPingResponse, this, std::placeholders::_1);
	mapFunctions[cmd::RESPONSE_CONFIG_ZONE] = std::bind(&SFSResponse::onConfigZoneResponse, this, std::placeholders::_1);
	mapFunctions[cmd::VIEW_USER_INFO] = std::bind(&SFSResponse::onUserDataResponse, this, std::placeholders::_1);
	mapFunctions[cmd::USER_INFO] = std::bind(&SFSResponse::onUserDataMeResponse, this, std::placeholders::_1);
	mapFunctions[cmd::RESPONSE_UPDATE_USERINFO] = std::bind(&SFSResponse::onUpdateUserDataMeResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_ROOM_DATA] = std::bind(&SFSResponse::onRoomDataResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_ROOM_DATA_GA] = std::bind(&SFSResponse::onGameRoomGaResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_READY] = std::bind(&SFSResponse::onGameReadyResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_START] = std::bind(&SFSResponse::onStartGameResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_STILT] = std::bind(&SFSResponse::onGameChooseStiltResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_HOST] = std::bind(&SFSResponse::onGameChooseHostResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_BASH] = std::bind(&SFSResponse::onBashResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_BASH_BACK] = std::bind(&SFSResponse::onBashBackResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_HOLD] = std::bind(&SFSResponse::onHoldResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_PICK] = std::bind(&SFSResponse::onPickResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_PENET] = std::bind(&SFSResponse::onPenetResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_FORWARD] = std::bind(&SFSResponse::onForwardResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_WIN] = std::bind(&SFSResponse::onUserWinResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_CALL_CREST] = std::bind(&SFSResponse::onCrestDataResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_END_MATCH] = std::bind(&SFSResponse::onEndMatchResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_END_MATCH_TIE] = std::bind(&SFSResponse::onEndMatchTieResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_END_MATCH_MONEY] = std::bind(&SFSResponse::onEndMatchMoneyResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_PUNISH] = std::bind(&SFSResponse::onGamePunishResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_TABLE_INFO] = std::bind(&SFSResponse::onGameTableResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_PLAYING_DATA] = std::bind(&SFSResponse::onGamePlayingTableResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_SPECTATOR_DATA] = std::bind(&SFSResponse::onGameSpectatorResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_MY_RECONNECT_DATA] = std::bind(&SFSResponse::onGameMyReconnectResponse, this, std::placeholders::_1);
	mapFunctions[cmd::GAME_RESPONSE_USER_RECONNECT_DATA] = std::bind(&SFSResponse::onGameUserReconnectResponse, this, std::placeholders::_1);
	mapFunctions[cmd::LOBBY_RESPONSE_DATA_ROOM] = std::bind(&SFSResponse::onLobbyTableResponse, this, std::placeholders::_1);
	mapFunctions[cmd::LOBBY_RESPONSE_DATA_ROOM_TYPE] = std::bind(&SFSResponse::onLobbyRoomTypeResponse, this, std::placeholders::_1);
	mapFunctions[cmd::LOBBY_RESPONSE_DATA_USER] = std::bind(&SFSResponse::onLobbyUserResponse, this, std::placeholders::_1);
	mapFunctions[cmd::LOBBY_RESPONSE_INVITE_PLAYER] = std::bind(&SFSResponse::onLobbyInviteResponse, this, std::placeholders::_1);
	mapFunctions[cmd::RESPONSE_RANK] = std::bind(&SFSResponse::onRankResponse, this, std::placeholders::_1);
	mapFunctions[cmd::RANK_WIN] = std::bind(&SFSResponse::onRankWinResponse, this, std::placeholders::_1);
	mapFunctions[cmd::PLAY_HISTORY] = std::bind(&SFSResponse::onPlayLogResponse, this, std::placeholders::_1);
	mapFunctions[cmd::SHOP_HISTORY] = std::bind(&SFSResponse::onShopHistoryResponse, this, std::placeholders::_1);
	mapFunctions[cmd::SHOP_ITEMS] = std::bind(&SFSResponse::onShopItemsResponse, this, std::placeholders::_1);
	mapFunctions[cmd::EXCHANGE_ITEM] = std::bind(&SFSResponse::onExchangeItemResponse, this, std::placeholders::_1);
	mapFunctions[cmd::NEW_MAIL] = std::bind(&SFSResponse::onNewMailResponse, this, std::placeholders::_1);
	mapFunctions[cmd::LIST_MAIL] = std::bind(&SFSResponse::onListMailResponse, this, std::placeholders::_1);
	mapFunctions[cmd::MAIL_CONTENT] = std::bind(&SFSResponse::onMailContentResponse, this, std::placeholders::_1);
	mapFunctions[cmd::NEWS] = std::bind(&SFSResponse::onNewsResponse, this, std::placeholders::_1);
	mapFunctions[cmd::EVENTS] = std::bind(&SFSResponse::onListEventResponse, this, std::placeholders::_1);
	mapFunctions[cmd::CURRENT_TABLE_RECONNECT] = std::bind(&SFSResponse::onTableReconnectResponse, this, std::placeholders::_1);
}

SFSResponse::~SFSResponse()
{
}

void SFSResponse::onExtensionResponse(boost::shared_ptr<string> ptrNotifiedCmd, boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject)
{
	if (Utils::getSingleton().isWaitingScene) {
		cachedResponses.push_back(pair<boost::shared_ptr<string>, boost::shared_ptr<ISFSObject>>(ptrNotifiedCmd, ptrNotifiedISFSObject));
	} else {
		if (mapFunctions.find(*ptrNotifiedCmd) != mapFunctions.end()) {
			try {
				mapFunctions[*ptrNotifiedCmd](ptrNotifiedISFSObject);
			} catch (exception e) {
				CCLOG("SFSResponse::onExtensionResponse::Exception: %s", e.what());
			}
		}
	}
}

void SFSResponse::RunCachedResponses()
{
	//CCLOG("RunCachedResponses: %d", cachedResponses.size());
	for (int i = 0; i < cachedResponses.size(); i++) {
		SFSResponse::getSingleton().onExtensionResponse(cachedResponses[i].first, cachedResponses[i].second);
	}
	cachedResponses.clear();
}

void SFSResponse::onPingResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	boost::shared_ptr<long long> ptrTime = isfsObject->GetLong("s");
	SFSRequest::getSingleton().Ping(*ptrTime);
}

void SFSResponse::onErrorResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	ErrorData error;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadUTF(error.Message);
	byteArray->ReadByte(error.Code);
	//CCLOG("%d %s", error.Code, error.Message.c_str());
	if (EventHandler::getSingleton().onErrorSFSResponse != NULL) {
		EventHandler::getSingleton().onErrorSFSResponse(error.Code, error.Message);
	}
}

void SFSResponse::onConfigZoneResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	while (byteArray->Position() < byteArray->Length()) {
		short size;
		std::vector<ZoneData> vecZone;
		byteArray->ReadShort(size);
		int pos = byteArray->Position() + size;
		while (byteArray->Position() < pos) {
			ZoneData data;
			byteArray->ReadByte(data.Id);
			byteArray->ReadUTF(data.ZoneName);
			byteArray->ReadUTF(data.ZoneIpIos);
			byteArray->ReadUTF(data.ZoneIp);
			byteArray->ReadInt(data.ZonePort);
			byteArray->ReadByte(data.Status);
			byteArray->ReadByte(data.Money);
			vecZone.push_back(data);
			//CCLOG("%d %s %s %s %d %d %d", data.Id, data.ZoneName.c_str(), data.ZoneIpIos.c_str(), data.ZoneIp.c_str(), data.ZonePort, data.Status, data.Money);
		}
		Utils::getSingleton().zones.push_back(vecZone);
	}
	if (EventHandler::getSingleton().onConfigZoneReceived != NULL) {
		EventHandler::getSingleton().onConfigZoneReceived();
	}
}

void SFSResponse::getUserDataFromSFSObject(boost::shared_ptr<ISFSObject> isfsObject, UserData &userData)
{
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadUTF(userData.Name);
	byteArray->ReadUTF(userData.DisplayName);
	byteArray->ReadDouble(userData.MoneyFree);
	byteArray->ReadDouble(userData.MoneyReal);
	byteArray->ReadInt(userData.UserID);
	byteArray->ReadInt(userData.Exp);
	byteArray->ReadShort(userData.Level);
	byteArray->ReadInt(userData.Total);
	byteArray->ReadInt(userData.Win);
	if (byteArray->Position() < byteArray->Length()) {
		byteArray->ReadByte(userData.MoneyType);
	}
	while (byteArray->Position() < byteArray->Length()) {
		byteArray->ReadByte(userData.IsActived);
	}
	//CCLOG("%s %s %d %.0f %.0f %d %s", userData.Name.c_str(), userData.DisplayName.c_str(), userData.UserID, userData.MoneyFree, userData.MoneyReal, userData.MoneyType, userData.IsActived ? "actived" : "non-active");
}

void SFSResponse::onUserDataResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	UserData data;
	getUserDataFromSFSObject(isfsObject, data);
	if (EventHandler::getSingleton().onUserDataSFSResponse != NULL) {
		EventHandler::getSingleton().onUserDataSFSResponse(data);
	}
}

void SFSResponse::onUserDataMeResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	getUserDataFromSFSObject(isfsObject, Utils::getSingleton().userDataMe);
	if (Utils::getSingleton().moneyType == -1) {
		Utils::getSingleton().moneyType = Utils::getSingleton().userDataMe.MoneyType;
	}
	if (EventHandler::getSingleton().onUserDataMeSFSResponse != NULL) {
		EventHandler::getSingleton().onUserDataMeSFSResponse();
	}
}

void SFSResponse::onUpdateUserDataMeResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	getUserDataFromSFSObject(isfsObject, Utils::getSingleton().userDataMe);
	if (EventHandler::getSingleton().onUserDataMeSFSResponse != NULL) {
		EventHandler::getSingleton().onUserDataMeSFSResponse();
	}
}

void SFSResponse::onRoomDataResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	RoomData roomData;
	boost::shared_ptr<ByteArray> byteArray1 = isfsObject->GetByteArray("d");
	byteArray1->ReadByte(roomData.TimeStart);
	byteArray1->ReadByte(roomData.TimeDeal);
	byteArray1->ReadByte(roomData.TimeChooseHost);
	//CCLOG("%d %d %d", roomData.TimeStart, roomData.TimeDeal, roomData.TimeChooseHost);
	while (byteArray1->Position() < byteArray1->Length() - 10) {
		short size;
		vector<unsigned char> tmpByte;
		byteArray1->ReadShort(size);
		byteArray1->ReadBytes(size, tmpByte);
		boost::shared_ptr<ByteArray> byteArray = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmpByte))));

		PlayerData player;
		player.Info = UserData();
		player.Info.Name = "";
		byteArray->ReadBool(player.Ready);
		byteArray->ReadUTF(player.Info.Name);
		byteArray->ReadUTF(player.Info.DisplayName);
		byteArray->ReadByte(player.Index);
		byteArray->ReadInt(player.Info.SfsUserId);
		byteArray->ReadInt(player.Info.UserID);
		byteArray->ReadDouble(player.PMoney);
		byteArray->ReadShort(player.UType);
		byteArray->ReadUTF(player.Ip);
		byteArray->ReadDouble(player.TMoney);
		roomData.Players.push_back(player);
		//CCLOG("%s %s %d %d %d %f %f %d %s %f", player.Ready ? "true" : "false", player.Info.Name.c_str(), player.Index, player.Info.SfsUserId, player.Info.UserID,
		//	player.Info.MoneyFree, player.Info.MoneyReal, player.UType, player.Ip.c_str(), player.TMoney);
	}
	if (EventHandler::getSingleton().onRoomDataSFSResponse != NULL) {
		EventHandler::getSingleton().onRoomDataSFSResponse(roomData);
	}
}

void SFSResponse::onStartGameResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	StartGameData data;
	unsigned char size1, size2, size3, size4, size5;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(data.CardStilt);
	byteArray->ReadInt(data.LastWinner);
	byteArray->ReadInt(data.CurrentTurn);
	if (byteArray->Position() < byteArray->Length()) {
		byteArray->ReadBool(data.CanWin);
		byteArray->ReadByte(size1);
		byteArray->ReadBytes(size1, data.CardHand.ThienKhai);
		byteArray->ReadByte(size2);
		byteArray->ReadBytes(size2, data.CardHand.Chan);
		byteArray->ReadByte(size3);
		byteArray->ReadBytes(size3, data.CardHand.BaDau);
		byteArray->ReadByte(size4);
		byteArray->ReadBytes(size4, data.CardHand.Ca);
		byteArray->ReadByte(size5);
		byteArray->ReadBytes(size5, data.CardHand.Que);
	}
	//CCLOG("%d %d %d %s", data.CardStilt, data.LastWinner, data.CurrentTurn, data.CanWin ? "true" : "false");

	if (EventHandler::getSingleton().onStartGameDataSFSResponse != NULL) {
		EventHandler::getSingleton().onStartGameDataSFSResponse(data);
	}
}

void SFSResponse::onGameChooseStiltResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	unsigned char stilt;
	byteArray->ReadByte(stilt);
	if (EventHandler::getSingleton().onChooseStiltSFSResponse != NULL) {
		EventHandler::getSingleton().onChooseStiltSFSResponse(stilt);
	}
}

void SFSResponse::onGameChooseHostResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	unsigned char stilt1, stilt2, host;
	byteArray->ReadByte(stilt1);
	//byteArray->ReadByte(stilt2);
	stilt2 = 1;
	byteArray->ReadByte(host);
	if (EventHandler::getSingleton().onChooseHostSFSResponse != NULL) {
		EventHandler::getSingleton().onChooseHostSFSResponse(stilt1, stilt2, host);
	}
}

void SFSResponse::onBashResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	BashData data;
	unsigned char size1, size2, size3, size4, size5;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(data.UId);
	byteArray->ReadByte(data.CardId);
	byteArray->ReadInt(data.TurnId);
	byteArray->ReadBool(data.CanPenet);
	byteArray->ReadBool(data.CanPenetWin);
	byteArray->ReadByte(data.SoundId);
	if (byteArray->Position() < byteArray->Length() - 5) {
		byteArray->ReadByte(size1);
		byteArray->ReadBytes(size1, data.CardHand.ThienKhai);
		byteArray->ReadByte(size2);
		byteArray->ReadBytes(size2, data.CardHand.Chan);
		byteArray->ReadByte(size3);
		byteArray->ReadBytes(size3, data.CardHand.BaDau);
		byteArray->ReadByte(size4);
		byteArray->ReadBytes(size4, data.CardHand.Ca);
		byteArray->ReadByte(size5);
		byteArray->ReadBytes(size5, data.CardHand.Que);
	}
	//CCLOG("%d %d %d %s %s %d", data.UId, data.CardId, data.TurnId, data.CanPenet ? "true" : "false", data.CanPenetWin ? "true" : "false", data.SoundId);
	if (EventHandler::getSingleton().onUserBashSFSResponse != NULL) {
		EventHandler::getSingleton().onUserBashSFSResponse(data);
	}
}

void SFSResponse::onBashBackResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	BashBackData data;
	unsigned char size1, size2, size3, size4, size5;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	//byteArray->ReadByte(data.Status);
	byteArray->ReadInt(data.UId);
	byteArray->ReadByte(data.CardId);
	byteArray->ReadInt(data.BackId);
	byteArray->ReadBool(data.IsPicked);
	byteArray->ReadInt(data.TurnId);
	byteArray->ReadBool(data.CanPenet);
	byteArray->ReadBool(data.CanPenetWin);
	byteArray->ReadByte(data.SoundId);
	if (byteArray->Position() < byteArray->Length() - 5) {
		byteArray->ReadByte(size1);
		byteArray->ReadBytes(size1, data.CardHand.ThienKhai);
		byteArray->ReadByte(size2);
		byteArray->ReadBytes(size2, data.CardHand.Chan);
		byteArray->ReadByte(size3);
		byteArray->ReadBytes(size3, data.CardHand.BaDau);
		byteArray->ReadByte(size4);
		byteArray->ReadBytes(size4, data.CardHand.Ca);
		byteArray->ReadByte(size5);
		byteArray->ReadBytes(size5, data.CardHand.Que);
	}
	//CCLOG("%d %d %d %d %s %s %d", data.UId, data.CardId, data.BackId, data.TurnId, data.CanPenet ? "true" : "false", data.CanPenetWin ? "true" : "false", data.SoundId);
	if (EventHandler::getSingleton().onUserBashBackSFSResponse != NULL) {
		EventHandler::getSingleton().onUserBashBackSFSResponse(data);
	}
}

void SFSResponse::onHoldResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	HoldData data;
	unsigned char size1, size2, size3, size4, size5;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(data.UId);
	byteArray->ReadByte(data.CardId);
	byteArray->ReadByte(data.CardIdHold);
	byteArray->ReadInt(data.TurnId);
	byteArray->ReadBool(data.IsPicked);
	byteArray->ReadByte(data.SoundId);
	if (byteArray->Position() < byteArray->Length() - 5) {
		byteArray->ReadByte(size1);
		byteArray->ReadBytes(size1, data.CardHand.ThienKhai);
		byteArray->ReadByte(size2);
		byteArray->ReadBytes(size2, data.CardHand.Chan);
		byteArray->ReadByte(size3);
		byteArray->ReadBytes(size3, data.CardHand.BaDau);
		byteArray->ReadByte(size4);
		byteArray->ReadBytes(size4, data.CardHand.Ca);
		byteArray->ReadByte(size5);
		byteArray->ReadBytes(size5, data.CardHand.Que);
	}
	//CCLOG("%d %d %d %d %s %d", data.UId, data.CardId, data.CardIdHold, data.TurnId, data.IsPicked ? "true" : "false", data.SoundId);
	if (EventHandler::getSingleton().onUserHoldSFSResponse != NULL) {
		EventHandler::getSingleton().onUserHoldSFSResponse(data);
	}
}

void SFSResponse::onPickResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	PickData data;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(data.UId);
	byteArray->ReadByte(data.CardId);
	byteArray->ReadInt(data.TurnId);
	byteArray->ReadBool(data.CanPenet);
	byteArray->ReadBool(data.CanWin);
	byteArray->ReadByte(data.StiltCount);
	byteArray->ReadByte(data.SoundId);
	//CCLOG("%d %d %d %d %s %s %d", data.UId, data.CardId, data.TurnId, data.StiltCount, data.CanPenet ? "true" : "false", data.CanWin ? "true" : "false", data.SoundId);
	if (EventHandler::getSingleton().onUserPickSFSResponse != NULL) {
		EventHandler::getSingleton().onUserPickSFSResponse(data);
	}
}

void SFSResponse::onForwardResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	ForwardData data;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(data.UId);
	byteArray->ReadInt(data.TurnId);
	byteArray->ReadByte(data.SoundId);
	//CCLOG("%d %d %d", data.UId, data.TurnId, data.SoundId);
	if (EventHandler::getSingleton().onUserForwardSFSResponse != NULL) {
		EventHandler::getSingleton().onUserForwardSFSResponse(data);
	}
}

void SFSResponse::onPenetResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	PenetData data;
	unsigned char size1, size2, size3, size4, size5;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(data.UId);
	byteArray->ReadByte(data.CardId);
	byteArray->ReadBool(data.IsPicked);
	byteArray->ReadByte(data.SoundId);
	if (byteArray->Position() < byteArray->Length() - 5) {
		byteArray->ReadByte(size1);
		byteArray->ReadBytes(size1, data.CardHand.ThienKhai);
		byteArray->ReadByte(size2);
		byteArray->ReadBytes(size2, data.CardHand.Chan);
		byteArray->ReadByte(size3);
		byteArray->ReadBytes(size3, data.CardHand.BaDau);
		byteArray->ReadByte(size4);
		byteArray->ReadBytes(size4, data.CardHand.Ca);
		byteArray->ReadByte(size5);
		byteArray->ReadBytes(size5, data.CardHand.Que);
	}
	//CCLOG("%d %d %s %d", data.UId, data.CardId, data.IsPicked ? "true" : "false", data.SoundId);
	if (EventHandler::getSingleton().onUserPenetSFSResponse != NULL) {
		EventHandler::getSingleton().onUserPenetSFSResponse(data);
	}
}

void SFSResponse::onUserWinResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	long uid;
	unsigned char sound;
	byteArray->ReadInt(uid);
	byteArray->ReadByte(sound);
	if (EventHandler::getSingleton().onUserWinSFSResponse != NULL) {
		EventHandler::getSingleton().onUserWinSFSResponse(uid, sound);
	}
}

void SFSResponse::onCrestDataResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	CrestResponseData data;
	unsigned char size1, size2, size3;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(data.Status);
	byteArray->ReadInt(data.UId);
	byteArray->ReadShort(data.Ga);
	byteArray->ReadShort(data.Diem);
	byteArray->ReadInt(data.DenLangId);
	byteArray->ReadByte(size1);
	byteArray->ReadBytes(size1, data.CuocHo);
	/*string strcuoc;
	for (int i = 0; i < size1; i++) {
	strcuoc += to_string(data.CuocHo[i]) + " ";
	}
	//CCLOG("CUOC_HO: %s", strcuoc.c_str());*/
	byteArray->ReadByte(size2);
	byteArray->ReadBytes(size2, data.CuocSai);
	/*string strcuocs;
	for (int i = 0; i < size2; i++) {
	strcuocs += to_string(data.CuocSai[i]) + " ";
	}
	//CCLOG("CUOC_SAI: %s", strcuocs.c_str());*/
	byteArray->ReadUTF(data.Msg);
	byteArray->ReadUTF(data.MsgAnGa);
	if (byteArray->Position() < byteArray->Length()) {
		byteArray->ReadByte(size3);
		for (int i = 0; i < size3; i++) {
			long id;
			byteArray->ReadInt(id);
			data.ListUDe.push_back(id);
		}
	}
	//CCLOG("%d %d %d %d %d %s %s", data.Status, data.UId, data.Ga, data.Diem, data.DenLangId, data.Msg.c_str(), data.MsgAnGa.c_str());
	if (EventHandler::getSingleton().onCrestResponseSFSResponse != NULL) {
		EventHandler::getSingleton().onCrestResponseSFSResponse(data);
	}
}

void SFSResponse::onEndMatchTieResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	unsigned char size1;
	vector<unsigned char> stiltCards;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(size1);
	byteArray->ReadBytes(size1, stiltCards);
	if (EventHandler::getSingleton().onEndMatchTieSFSResponse != NULL) {
		EventHandler::getSingleton().onEndMatchTieSFSResponse(stiltCards);
	}
}

void SFSResponse::onEndMatchResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	EndMatchData data;
	unsigned char size1, size2, size3, size4, size5, size6, size7, size8, size9, size10, size11;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(data.WinId);
	byteArray->ReadByte(size1);
	byteArray->ReadBytes(size1, data.ListChanU);
	byteArray->ReadByte(size2);
	byteArray->ReadBytes(size2, data.ListCaU);
	byteArray->ReadByte(size3);
	byteArray->ReadBytes(size3, data.ListCardU);
	byteArray->ReadByte(size4);
	byteArray->ReadBytes(size4, data.ListStiltCard);
	byteArray->ReadByte(data.SoundId);
	byteArray->ReadByte(size5);
	byteArray->ReadBytes(size5, data.ListHandCardU);
	byteArray->ReadByte(size6);
	for (int i = 0; i < size6; i++) {
		PlayerCardHandData player;
		byteArray->ReadInt(player.UId);
		byteArray->ReadByte(size7);
		byteArray->ReadBytes(size7, player.CardHand.ThienKhai);
		byteArray->ReadByte(size8);
		byteArray->ReadBytes(size8, player.CardHand.Chan);
		byteArray->ReadByte(size9);
		byteArray->ReadBytes(size9, player.CardHand.BaDau);
		byteArray->ReadByte(size10);
		byteArray->ReadBytes(size10, player.CardHand.Ca);
		byteArray->ReadByte(size11);
		byteArray->ReadBytes(size11, player.CardHand.Que);
		data.ListPlayerCard.push_back(player);
	}
	if (EventHandler::getSingleton().onEndMatchSFSResponse != NULL) {
		EventHandler::getSingleton().onEndMatchSFSResponse(data);
	}
}

void SFSResponse::onEndMatchMoneyResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	EndMatchMoneyData data;
	unsigned char size;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(size);
	for (int i = 0; i < size; i++) {
		long uid;
		double amount;
		byteArray->ReadInt(uid);
		byteArray->ReadDouble(amount);
		data.ListUserId.push_back(uid);
		data.ListUserAmount.push_back(amount);
		//CCLOG("%d %.0f", uid, amount);
	}
	if (EventHandler::getSingleton().onEndMatchMoneySFSResponse != NULL) {
		EventHandler::getSingleton().onEndMatchMoneySFSResponse(data);
	}
}

void SFSResponse::onLobbyRoomTypeResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	LobbyListRoomType data;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadBool(data.IsRealMoney);
	//CCLOG("RealMoney: %s", data.IsRealMoney ? "true" : "false");
	while (byteArray->Position() < byteArray->Length()) {
		LobbyRoomType room;
		byteArray->ReadByte(room.Id);
		byteArray->ReadUTF(room.Group);
		byteArray->ReadUTF(room.Name);
		byteArray->ReadUTF(room.Description);
		data.ListRoomType.push_back(room);
		//CCLOG("%d %s %s %s", room.Id, room.Group.c_str(), room.Name.c_str(), room.Description.c_str());
	}
	Utils::getSingleton().lobbyListRoomType = data;
	Utils::getSingleton().moneyType = (int)data.IsRealMoney;
	if (EventHandler::getSingleton().onLobbyRoomTypeSFSResponse != NULL) {
		EventHandler::getSingleton().onLobbyRoomTypeSFSResponse(data);
	}
}

void SFSResponse::onLobbyTableResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	LobbyListTable listTable;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(listTable.Size);
	byteArray->ReadInt(listTable.Money);
	while (byteArray->Position() < byteArray->Length()) {
		LobbyTableData data;
		vector<unsigned char> tmp1, tmp2;
		short size1, size2, size3;
		byteArray->ReadShort(size1);
		byteArray->ReadBytes(size1, tmp1);

		boost::shared_ptr<ByteArray> byteArray1 = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmp1))));
		byteArray1->ReadInt(data.Money);
		byteArray1->ReadBool(data.IsAdvance);
		byteArray1->ReadBool(data.IsU411);
		byteArray1->ReadByte(data.HasPot);
		byteArray1->ReadByte(data.Time);
		byteArray1->ReadByte(data.RoomId);

		while (byteArray1->Position() < byteArray1->Length()) {
			byteArray1->ReadShort(size2);

			LobbyPlayerData pdata;
			byteArray1->ReadInt(pdata.UserId);
			byteArray1->ReadDouble(pdata.Money);
			byteArray1->ReadUTF(pdata.Name);

			data.ListPlayer.push_back(pdata);
		}
		listTable.ListTable.push_back(data);
	}
	Utils::getSingleton().lobbyListTable = listTable;
	if (EventHandler::getSingleton().onLobbyTableSFSResponse != NULL) {
		EventHandler::getSingleton().onLobbyTableSFSResponse(listTable);
	}
}

void SFSResponse::onGamePunishResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	long uid;
	std::string msg;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadUTF(msg);
	byteArray->ReadInt(uid);
	//CCLOG("%d %s", UiD, msg.c_str());
	if (EventHandler::getSingleton().onGamePunishSFSResponse != NULL) {
		EventHandler::getSingleton().onGamePunishSFSResponse(uid, msg);
	}
}

void SFSResponse::onGameReadyResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	long uid;
	bool isReady;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(uid);
	byteArray->ReadBool(isReady);
	//CCLOG("%d %s", uid, isReady ? "true" : "false");
	if (EventHandler::getSingleton().onGameReadySFSResponse != NULL) {
		EventHandler::getSingleton().onGameReadySFSResponse(uid, isReady);
	}
}

void SFSResponse::onGameTableResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	GameTableData data;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(data.Money);
	byteArray->ReadByte(data.Time);
	byteArray->ReadBool(data.IsAdvance);
	byteArray->ReadBool(data.IsU411);
	byteArray->ReadBool(data.IsQuick);
	////CCLOG("%d %d %s %s %s", data.Money, data.Time, data.IsAdvance ? "true" : "false", data.IsU411 ? "true" : "false", data.IsQuick ? "true" : "false");
	if (EventHandler::getSingleton().onGameTableSFSResponse != NULL) {
		EventHandler::getSingleton().onGameTableSFSResponse(data);
	}
}

void SFSResponse::onLobbyUserResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::vector<UserData> listUser;
	boost::shared_ptr<ByteArray> byteArray1 = isfsObject->GetByteArray("d");
	while (byteArray1->Position() < byteArray1->Length()) {
		short size;
		vector<unsigned char> tmpByte;
		byteArray1->ReadShort(size);
		byteArray1->ReadBytes(size, tmpByte);
		boost::shared_ptr<ByteArray> byteArray = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmpByte))));

		UserData user;
		byteArray->ReadUTF(user.Name);
		byteArray->ReadDouble(user.MoneyFree);
		byteArray->ReadDouble(user.MoneyReal);
		byteArray->ReadInt(user.UserID);
		byteArray->ReadInt(user.Exp);
		byteArray->ReadShort(user.Level);
		byteArray->ReadInt(user.Total);
		byteArray->ReadInt(user.Win);
		byteArray->ReadInt(user.SfsUserId);
		byteArray->ReadUTF(user.DisplayName);

		listUser.push_back(user);
		//CCLOG("%s %.0f %.0f %d %d %d %d %d", user.Name.c_str(), user.MoneyFree, user.MoneyReal, user.UserID, user.Exp, user.Level, user.Total, user.Win);
	}
	if (EventHandler::getSingleton().onLobbyUserDataSFSResponse != NULL) {
		EventHandler::getSingleton().onLobbyUserDataSFSResponse(listUser);
	}
}

void SFSResponse::onLobbyInviteResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	InviteData data;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadUTF(data.InviterName);
	byteArray->ReadInt(data.InviterId);
	byteArray->ReadDouble(data.Money);
	byteArray->ReadByte(data.RoomTime);
	byteArray->ReadUTF(data.Msg);
	byteArray->ReadUTF(data.RoomName);
	byteArray->ReadUTF(data.Password);
	byteArray->ReadUTF(data.InviterName);
	////CCLOG("%s %d %.0f %d %s %s %s", data.InviterName.c_str(), data.InviterId, data.Money, data.RoomTime, data.Msg.c_str(), data.RoomName.c_str(), data.Password.c_str());
	if (EventHandler::getSingleton().onLobbyInviteDataSFSResponse != NULL) {
		EventHandler::getSingleton().onLobbyInviteDataSFSResponse(data);
	}
}

void SFSResponse::onGameRoomGaResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	bool isGa;
	double gaMoney;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadBool(isGa);
	if (isGa) byteArray->ReadDouble(gaMoney);
	else gaMoney = 0;
	//CCLOG("%s %.0f", isGa ? "true" : "false", gaMoney);
	if (EventHandler::getSingleton().onGameRoomDataGaSFSResponse != NULL) {
		EventHandler::getSingleton().onGameRoomDataGaSFSResponse(isGa, gaMoney);
	}
}

void SFSResponse::onGamePlayingTableResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	PlayingTableData data;
	boost::shared_ptr<ByteArray> byteArray1 = isfsObject->GetByteArray("d");
	byteArray1->ReadBool(data.IsReconnect);
	byteArray1->ReadInt(data.TurnId);
	byteArray1->ReadByte(data.StiltCount);
	while (byteArray1->Position() < byteArray1->Length()) {
		short size;
		std::string str1, str2;
		std::vector<std::vector<char>> vnumb;
		std::vector<unsigned char> tmpByte;
		byteArray1->ReadShort(size);
		byteArray1->ReadBytes(size, tmpByte);
		boost::shared_ptr<ByteArray> byteArray = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmpByte))));

		PlayerData player;
		player.Info = UserData();
		byteArray->ReadUTF(player.Info.Name);
		byteArray->ReadUTF(player.Info.DisplayName);
		byteArray->ReadByte(player.Index);
		byteArray->ReadInt(player.Info.SfsUserId);
		byteArray->ReadInt(player.Info.UserID);
		byteArray->ReadDouble(player.PMoney);
		byteArray->ReadShort(player.UType);
		byteArray->ReadUTF(player.Ip);
		byteArray->ReadDouble(player.TMoney);
		byteArray->ReadUTF(player.Info.GroupAvatar);
		byteArray->ReadByte(player.Info.Device);
		byteArray->ReadUTF(str1);
		byteArray->ReadUTF(str2);
		//CCLOG("%s ___ %s", str1.c_str(), str2.c_str());
		if (str1.length() > 2) {
			SFSResponse::onPlayingCardFromJson(str1, player.PairCards);
		}
		if (str2.length() > 2) {
			SFSResponse::onPlayingCardFromJson(str2, vnumb);
		}
		if (vnumb.size() > 0) {
			player.SingleCards = vnumb[0];
		}

		data.Players.push_back(player);
		//CCLOG("%s %d %d %d %d %s %.0f %s %d ___ %s ___ %s", player.Info.Name.c_str(), player.Index, player.Info.SfsUserId, player.Info.UserID,
		//	player.UType, player.Ip.c_str(), player.PMoney, player.Info.GroupAvatar.c_str(), player.Info.Device, str1.c_str(), str2.c_str());
	}
	if (EventHandler::getSingleton().onGamePlayingDataSFSResponse != NULL) {
		EventHandler::getSingleton().onGamePlayingDataSFSResponse(data);
	}
}

void SFSResponse::onPlayingCardFromJson(std::string json, std::vector<std::vector<char>> &vec) 
{
	char numb = -100;
	vector<char> vnumb;
	for (int i = 0; i < json.length(); i++) {
		if (json[i] >= '0' && json[i] <= '9') {
			if (numb == -100) {
				numb = json[i] - 48;
				if (i > 0 && json[i - 1] == '-') {
					numb = -numb;
				}
			} else {
				if (numb > 0) {
					numb = (numb * 10) + json[i] - 48;
				} else {
					numb = (numb * 10) - json[i] + 48;
				}
			}
		} else if (numb != -100) {
			vnumb.push_back(numb);
			if (json[i] == ']') {
				vector<char> newVec = vector<char>(vnumb);
				vec.push_back(newVec);
				vnumb.clear();
			}
			numb = -100;
		}
	}
}

void SFSResponse::onGameSpectatorResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::vector<PlayerData> list;
	boost::shared_ptr<ByteArray> byteArray1 = isfsObject->GetByteArray("d");
	while (byteArray1->Position() < byteArray1->Length()) {
		short size;
		std::vector<unsigned char> tmpByte;
		byteArray1->ReadShort(size);
		byteArray1->ReadBytes(size, tmpByte);
		boost::shared_ptr<ByteArray> byteArray = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmpByte))));

		PlayerData player;
		player.Info = UserData();
		byteArray->ReadBool(player.Ready);
		byteArray->ReadUTF(player.Info.Name);
		byteArray->ReadUTF(player.Info.DisplayName);
		byteArray->ReadByte(player.Index);
		byteArray->ReadInt(player.Info.SfsUserId);
		byteArray->ReadInt(player.Info.UserID);
		byteArray->ReadDouble(player.PMoney);
		byteArray->ReadShort(player.UType);
		byteArray->ReadUTF(player.Ip);
		byteArray->ReadShort(player.Info.Level);
		byteArray->ReadDouble(player.TMoney);
		byteArray->ReadUTF(player.Info.GroupAvatar);
		byteArray->ReadByte(player.Info.Device);

		list.push_back(player);
		//CCLOG("%s %d %d %d %.0f %d %s %s %d", player.Info.Name.c_str(), player.Index, player.Info.SfsUserId, player.Info.UserID,
		//	player.PMoney, player.UType, player.Ip.c_str(), player.Info.GroupAvatar.c_str(), player.Info.Device);
	}
	if (EventHandler::getSingleton().onGameSpectatorDataSFSResponse != NULL) {
		EventHandler::getSingleton().onGameSpectatorDataSFSResponse(list);
	}
}

void SFSResponse::onRankResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::vector<std::vector<RankData>> list;
	short size1, size2;
	std::vector<RankData> list1, list2;
	std::vector<unsigned char> tmp1, tmp2;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadShort(size1);
	byteArray->ReadBytes(size1, tmp1);
	byteArray->ReadShort(size2);
	byteArray->ReadBytes(size2, tmp2);

	boost::shared_ptr<ByteArray> byteArray1 = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmp1))));
	while (byteArray1->Position() < byteArray1->Length()) {
		unsigned char size3;
		std::vector<unsigned char> tmp3;
		byteArray1->ReadByte(size3);
		byteArray1->ReadBytes(size3, tmp3);
		boost::shared_ptr<ByteArray> byteArray3 = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmp3))));

		RankData data;
		byteArray3->ReadUTF(data.Name);
		byteArray3->ReadDouble(data.Money);
		byteArray3->ReadInt(data.Exp);
		byteArray3->ReadInt(data.Uid);
		list1.push_back(data);
	}

	boost::shared_ptr<ByteArray> byteArray2 = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmp2))));
	while (byteArray2->Position() < byteArray2->Length()) {
		unsigned char size4;
		std::vector<unsigned char> tmp4;
		byteArray2->ReadByte(size4);
		byteArray2->ReadBytes(size4, tmp4);
		boost::shared_ptr<ByteArray> byteArray4 = boost::shared_ptr<ByteArray>(new ByteArray(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(tmp4))));

		RankData data;
		byteArray4->ReadUTF(data.Name);
		byteArray4->ReadDouble(data.Money);
		byteArray4->ReadInt(data.Exp);
		byteArray4->ReadInt(data.Uid);
		list2.push_back(data);
	}

	list.push_back(list1);
	list.push_back(list2);
	if (EventHandler::getSingleton().onRankDataSFSResponse != NULL) {
		EventHandler::getSingleton().onRankDataSFSResponse(list);
	}
}

void SFSResponse::onRankWinResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	unsigned char size;
	std::vector<RankWinData > list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(size);
	while (byteArray->Position() < byteArray->Length()) {
		RankWinData data;
		byteArray->ReadInt(data.Uid);
		byteArray->ReadUTF(data.Name);
		byteArray->ReadInt(data.Point);
		byteArray->ReadUTF(data.Cuoc);
		byteArray->ReadUTF(data.Date);
		list.push_back(data);
		//CCLOG("%d %s %d %s %s", data.Uid, data.Name.c_str(), data.Point, data.Cuoc.c_str(), data.Date.c_str());
	}
	if (EventHandler::getSingleton().onRankWinDataSFSResponse != NULL) {
		EventHandler::getSingleton().onRankWinDataSFSResponse(list);
	}
}

void SFSResponse::onPlayLogResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::vector<PlayLogData> list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	long tmp1;
	unsigned char tmp2;
	byteArray->ReadInt(tmp1);
	byteArray->ReadByte(tmp2);
	while (byteArray->Position() < byteArray->Length()) {
		PlayLogData data;
		byteArray->ReadUTF(data.Date);
		byteArray->ReadUTF(data.Info);
		byteArray->ReadInt(data.Money);
		byteArray->ReadDouble(data.Balance);
		byteArray->ReadInt(data.GameId);
		list.push_back(data);
		//CCLOG("%s %s %d %.0f %d", data.Date.c_str(), data.Info.c_str(), data.Money, data.Balance, data.GameId);
	}
	if (EventHandler::getSingleton().onPlayLogDataSFSResponse != NULL) {
		EventHandler::getSingleton().onPlayLogDataSFSResponse(list);
	}
}

void SFSResponse::onGameMyReconnectResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	GameReconnectData data;
	unsigned char size1, size2, size3, size4, size5;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(data.PlayerState);
	byteArray->ReadByte(size1);
	byteArray->ReadBytes(size1, data.CardHand.ThienKhai);
	byteArray->ReadByte(size2);
	byteArray->ReadBytes(size2, data.CardHand.Chan);
	byteArray->ReadByte(size3);
	byteArray->ReadBytes(size3, data.CardHand.BaDau);
	byteArray->ReadByte(size4);
	byteArray->ReadBytes(size4, data.CardHand.Ca);
	byteArray->ReadByte(size5);
	byteArray->ReadBytes(size5, data.CardHand.Que);
	byteArray->ReadShort(data.IndexCard);
	if (EventHandler::getSingleton().onGameMyReconnectDataSFSResponse != NULL) {
		EventHandler::getSingleton().onGameMyReconnectDataSFSResponse(data);
	}
}

void SFSResponse::onGameUserReconnectResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::vector<UserReconnectData> list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	while (byteArray->Position() < byteArray->Length()) {
		UserReconnectData data;
		byteArray->ReadInt(data.SfsUserId);
		byteArray->ReadUTF(data.Name);
		byteArray->ReadUTF(data.DisplayName);
		byteArray->ReadInt(data.UserId);
		list.push_back(data);
	}
	if (EventHandler::getSingleton().onGameUserReconnectDataSFSResponse != NULL) {
		EventHandler::getSingleton().onGameUserReconnectDataSFSResponse(list);
	}
}

void SFSResponse::onShopHistoryResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	long numb;
	std::vector<ShopHistoryData> list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(numb);
	while (byteArray->Position() < byteArray->Length()) {
		ShopHistoryData data;
		byteArray->ReadByte(data.Id);
		byteArray->ReadByte(data.ItemId);
		byteArray->ReadInt(data.UserId);
		byteArray->ReadUTF(data.Name);
		byteArray->ReadByte(data.Price);
		byteArray->ReadByte(data.Status);
		byteArray->ReadUTF(data.CreateDate);
		byteArray->ReadUTF(data.UpdateDate);
		byteArray->ReadUTF(data.Content);
		list.push_back(data);
		//CCLOG("%d %d %s %d %d %s %s %s", data.Id, data.ItemId, data.Name.c_str(), data.Price, data.Status, data.CreateDate.c_str(), data.Content);
	}
	if (EventHandler::getSingleton().onShopHistoryDataSFSResponse != NULL) {
		EventHandler::getSingleton().onShopHistoryDataSFSResponse(list);
	}
}

void SFSResponse::onShopItemsResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::vector<ShopItemData> list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	while (byteArray->Position() < byteArray->Length()) {
		ShopItemData data;
		byteArray->ReadByte(data.Id);
		byteArray->ReadUTF(data.Name);
		byteArray->ReadInt(data.Price);
		byteArray->ReadInt(data.PriceChange);
		byteArray->ReadShort(data.ItemType);
		byteArray->ReadUTF(data.Image);
		byteArray->ReadShort(data.IsActived);
		list.push_back(data);
		//CCLOG("%d %s %d %d %d %s %d", data.Id, data.Name.c_str(), data.Price, data.PriceChange, data.ItemType, data.Image.c_str(), data.IsActived);
	}
	if (EventHandler::getSingleton().onShopItemsDataSFSResponse != NULL) {
		EventHandler::getSingleton().onShopItemsDataSFSResponse(list);
	}
}

void SFSResponse::onExchangeItemResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::string msg;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadUTF(msg);
	if (EventHandler::getSingleton().onExchangeItemSFSResponse != NULL) {
		EventHandler::getSingleton().onExchangeItemSFSResponse(msg);
	}
}

void SFSResponse::onNewMailResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	unsigned char count;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadByte(count);
	if (EventHandler::getSingleton().onNewMailSFSResponse != NULL) {
		EventHandler::getSingleton().onNewMailSFSResponse(count);
	}
}

void SFSResponse::onListMailResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	long tmp;
	std::vector<MailData> list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(tmp);
	while (byteArray->Position() < byteArray->Length()) {
		MailData data;
		//unsigned char c;
		byteArray->ReadInt(data.Id);
		//byteArray->ReadByte(c);
		//data.Id = c;
		byteArray->ReadUTF(data.Sender);
		byteArray->ReadUTF(data.Title);
		byteArray->ReadBool(data.IsRead);
		byteArray->ReadUTF(data.Date);
		list.push_back(data);
		//CCLOG("%d %s %s %s %s", data.Id, data.Date.c_str(), data.Title.c_str(), data.Sender.c_str(), data.IsRead ? "true" : "false");
	}
	if (EventHandler::getSingleton().onListMailDataSFSResponse != NULL) {
		EventHandler::getSingleton().onListMailDataSFSResponse(list);
	}
}

void SFSResponse::onMailContentResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	long tmp;
	std::string content;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	if (byteArray->Length() > 0) {
		byteArray->ReadInt(tmp);
		byteArray->ReadUTF(content);
	}
	if (EventHandler::getSingleton().onMailContentSFSResponse != NULL) {
		EventHandler::getSingleton().onMailContentSFSResponse(content);
	}
}

void SFSResponse::onNewsResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	long tmp;
	std::vector<NewsData> list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadInt(tmp);
	while (byteArray->Position() < byteArray->Length()) {
		NewsData data;
		byteArray->ReadByte(data.Id);
		byteArray->ReadUTF(data.Title);
		byteArray->ReadUTF(data.Content);
		byteArray->ReadUTF(data.Image);
		byteArray->ReadByte(data.Type);
		byteArray->ReadUTF(data.Date);
		list.push_back(data);
	}
	if (EventHandler::getSingleton().onNewsDataSFSResponse != NULL) {
		EventHandler::getSingleton().onNewsDataSFSResponse(list);
	}
}

void SFSResponse::onListEventResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	std::vector<EventData> list;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	while (byteArray->Position() < byteArray->Length()) {
		EventData data;
		byteArray->ReadByte(data.Id);
		byteArray->ReadUTF(data.Content);
		byteArray->ReadUTF(data.Date);
		list.push_back(data);
	}
	Utils::getSingleton().events = list;
	Utils::getSingleton().currentEventPosX = constant::EVENT_START_POSX;
	if (EventHandler::getSingleton().onListEventDataSFSResponse != NULL) {
		EventHandler::getSingleton().onListEventDataSFSResponse(list);
	}
}

void SFSResponse::onTableReconnectResponse(boost::shared_ptr<ISFSObject> isfsObject)
{
	TableReconnectData data;
	boost::shared_ptr<ByteArray> byteArray = isfsObject->GetByteArray("d");
	byteArray->ReadUTF(data.Room);
	byteArray->ReadUTF(data.Message);

	Utils::getSingleton().tableReconnectData = data;
	if (EventHandler::getSingleton().onTableReconnectDataSFSResponse != NULL) {
		EventHandler::getSingleton().onTableReconnectDataSFSResponse(data);
	}
}