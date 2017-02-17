#include "SFSRequest.h"
#include "SFSConnector.h"
#include "Command.h"
#include "Utils.h"

template<> SFSRequest* SingLeton<SFSRequest>::mSingleton = 0;
SFSRequest* SFSRequest::getSingletonPtr(void)
{
	return mSingleton;
}
SFSRequest& SFSRequest::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

SFSRequest::SFSRequest()
{
}

SFSRequest::~SFSRequest()
{
}

void SFSRequest::Connect()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	std::string host = Utils::getSingleton().gameConfig.ip_rs;
#else
	std::string host = Utils::getSingleton().gameConfig.host;
#endif
	SFSConnector::getSingleton().Connect(host, Utils::getSingleton().gameConfig.port);
}

void SFSRequest::Connect(std::string host, int port)
{
	SFSConnector::getSingleton().Connect(host, port);
}

void SFSRequest::Disconnect()
{
	SFSConnector::getSingleton().Disconnect();
}

void SFSRequest::ProcessEvents()
{
	SFSConnector::getSingleton().ProcessEvents();
}

void SFSRequest::LoginZone(std::string username, std::string password, std::string zone)
{
	SFSConnector::getSingleton().LoginZone(username, password, zone);
}

void SFSRequest::LogoutZone()
{
	SFSConnector::getSingleton().LogoutZone();
}

void SFSRequest::SendPublicMessage(std::string message)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendPublicMessage(message, parameters);
}

void SFSRequest::Ping(long long time)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	parameters->PutLong("s", time);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::REQUEST_PING, parameters);
}

void SFSRequest::Ping()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::REQUEST_PING, parameters);
}

void SFSRequest::ForceIPv6(bool value)
{
    SFSConnector::getSingleton().ForceIPv6(value);
}

void SFSRequest::RequestLeaveRoom()
{
	SFSConnector::getSingleton().RequestLeaveRoom();
}

void SFSRequest::RequestChargeCard(std::string code, std::string seri, std::string serviceId)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());

	/*boost::shared_ptr<ByteArray> data1 = boost::shared_ptr<ByteArray>(new ByteArray());
	boost::shared_ptr<ByteArray> data2 = boost::shared_ptr<ByteArray>(new ByteArray());
	boost::shared_ptr<ByteArray> data3 = boost::shared_ptr<ByteArray>(new ByteArray());
	data1->WriteUTF(code);
	data2->WriteUTF(seri);
	data3->WriteUTF(serviceId);
	data->WriteShort(data1->Length());
	data->WriteBytes(data1->Bytes());
	data->WriteShort(data2->Length());
	data->WriteBytes(data2->Bytes());
	data->WriteShort(data3->Length());
	data->WriteBytes(data3->Bytes());*/

	data->WriteUTF(code);
	data->WriteUTF(seri);
	data->WriteUTF(serviceId);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::REQUEST_CHARGE_CARD, parameters);
}

void SFSRequest::RequestUserInfo(std::string username)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteUTF(username);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::VIEW_USER_INFO, parameters);
}

void SFSRequest::RequestRank()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::REQUEST_RANK, parameters);
}

void SFSRequest::RequestRankWin()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteBool(false);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::RANK_WIN, parameters);
}

void SFSRequest::RequestPlayHistory(int type, int page)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteBool(type == 0);
	data->WriteInt(page);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::PLAY_HISTORY, parameters);
}

void SFSRequest::RequestShopHistory(int page)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteInt(page);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::SHOP_HISTORY, parameters);
}

void SFSRequest::RequestShopItems()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::SHOP_ITEMS, parameters);
}

void SFSRequest::RequestExchangeItem(unsigned char id)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteByte(id);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::EXCHANGE_ITEM, parameters);
}

void SFSRequest::RequestListMail(int page)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteInt(page);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::LIST_MAIL, parameters);
}

void SFSRequest::RequestMailContent(long id)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteInt(id);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::MAIL_CONTENT, parameters);
}

void SFSRequest::RequestUpdateDisplayName(std::string name)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteUTF(name);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::UPDATE_DISPLAYNAME, parameters);
}

void SFSRequest::RequestNews(int page)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteInt(page);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::NEWS, parameters);
}

void SFSRequest::RequestGiftcode(std::string giftcode)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteUTF(giftcode);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GIFT_CODE, parameters);
}

void SFSRequest::RequestPayment(std::string token)
{
    boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    parameters->PutUtfString("1", token);
    parameters->PutUtfString("2", Utils::getSingleton().getPlatformOS());
    SFSConnector::getSingleton().SendExtensionRequest(cmd::PAYMENT, parameters);
}

void SFSRequest::RequestRegister(std::string username, std::string password, std::string email)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteUTF(username);
	data->WriteUTF(password);
	data->WriteUTF(email);
	parameters->PutByteArray("d", data);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::REGISTER, parameters);
}

void SFSRequest::RequestLogin(std::string username, std::string password)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	parameters->PutUtfString("1", username);
	parameters->PutUtfString("2", password);
	parameters->PutUtfString("3", Utils::getSingleton().getPlatformOS());
	parameters->PutUtfString("4", cocos2d::Application::sharedApplication()->getVersion());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::LOGIN, parameters);
}

void SFSRequest::RequestLoginFacebook(std::string token)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
	data->WriteUTF(token);
	parameters->PutByteArray("d", data);
	parameters->PutUtfString("3", Utils::getSingleton().getPlatformOS());
	parameters->PutUtfString("4", cocos2d::Application::sharedApplication()->getVersion());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::LOGIN_FACEBOOK, parameters);
}

void SFSRequest::RequestJoinRoom(std::string roomId, bool isReconnect)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	parameters->PutBool("p", true);
	parameters->PutUtfString("1", roomId);
	parameters->PutBool("2", isReconnect);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::JOIN_ROOM, parameters);
}

void SFSRequest::RequestGameReady()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_READY, parameters);
}

void SFSRequest::RequestGameChooseStilt(int stilt)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
//	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
//	data->WriteByte((unsigned char)stilt);
//	parameters->PutByteArray("d", data);
    parameters->PutInt("1", stilt);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_CHOOSE_STILT, parameters);
}

void SFSRequest::RequestGameChooseHost(int host)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
//	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
//	data->WriteByte((unsigned char)host);
//	parameters->PutByteArray("d", data);
    parameters->PutInt("1", host);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_CHOOSE_HOST, parameters);
}

void SFSRequest::RequestGameBash(unsigned char card, unsigned char group)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
//	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
//	data->WriteByte((unsigned char)card);
//	data->WriteByte((unsigned char)group);
//	parameters->PutByteArray("d", data);
    parameters->PutByte("1", card);
    parameters->PutByte("2", group);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_BASH, parameters);
}

void SFSRequest::RequestGameBashBack(unsigned char card, unsigned char group)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
//	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
//	data->WriteByte((unsigned char)card);
//	data->WriteByte((unsigned char)group);
//	parameters->PutByteArray("d", data);
    parameters->PutByte("1", card);
    parameters->PutByte("2", group);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_BASH_BACK, parameters);
}

void SFSRequest::RequestGameHold(unsigned char card, unsigned char group)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
//	boost::shared_ptr<ByteArray> data = boost::shared_ptr<ByteArray>(new ByteArray());
//	data->WriteByte((unsigned char)card);
//	data->WriteByte((unsigned char)group);
//	parameters->PutByteArray("d", data);
    parameters->PutByte("1", card);
    parameters->PutByte("2", group);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_HOLD, parameters);
}

void SFSRequest::RequestGamePick()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_PICK, parameters);
}

void SFSRequest::RequestGameForward()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_FORWARD, parameters);
}

void SFSRequest::RequestGamePenet()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_PENET, parameters);
}

void SFSRequest::RequestGameDropPenet()
{
    boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_DROP_PENET, parameters);
}

void SFSRequest::RequestGameWin()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_WIN, parameters);
}

void SFSRequest::RequestGameDropWin()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_DROP_WIN, parameters);
}

void SFSRequest::RequestGameWinCrest(vector<unsigned char> crests)
{
	boost::shared_ptr<ByteArray> byteArray = boost::shared_ptr<ByteArray>(new ByteArray());
	byteArray->WriteByte(crests.size());
	byteArray->WriteBytes(boost::shared_ptr<vector<unsigned char>>(new vector<unsigned char>(crests)));

	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	parameters->PutByteArray("d", byteArray);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_CALL_CREST, parameters);
}

void SFSRequest::RequestGameLobbyUser()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_LOBBY_USER, parameters);
}

void SFSRequest::RequestGameInvitePlayer(long uid)
{
	boost::shared_ptr<ByteArray> byteArray = boost::shared_ptr<ByteArray>(new ByteArray());
	byteArray->WriteInt(uid);

	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	parameters->PutByteArray("d", byteArray);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_INVITE_PLAYER, parameters);
}

void SFSRequest::RequestGameTableInfo(bool isQuick, bool isU411)
{
	boost::shared_ptr<ByteArray> byteArray = boost::shared_ptr<ByteArray>(new ByteArray());
	byteArray->WriteByte(isQuick);
	byteArray->WriteByte(isU411);

	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	parameters->PutByteArray("d", byteArray);
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_TABLE_INFO, parameters);
}

void SFSRequest::RequestGameGa()
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	SFSConnector::getSingleton().SendExtensionRequest(cmd::GAME_REQUEST_ROOM_DATA_GA, parameters);
}

void SFSRequest::RequestHttpGet(std::string url, int tag)
{
	// Setting HTTP Request Header
	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/json; charset=utf-8");

	// Creating a URL
	cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
	request->setUrl(url);

	// If you want to use GET method, replace kHttpPost to kHttpGet
	request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
	request->setResponseCallback(this, httpresponse_selector(SFSRequest::onHttpRequest));
	request->setHeaders(headers);
	request->setTag(to_string(tag));

	// Execute
	cocos2d::network::HttpClient::getInstance()->send(request);

	// Release the Connection
	request->release();
}

void SFSRequest::LoadImageFromURL(std::string url, int tag)
{
	cocos2d::network::HttpRequest* request = new (std::nothrow) cocos2d::network::HttpRequest();
	request->setUrl(url);
	request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	request->setResponseCallback(this, httpresponse_selector(SFSRequest::onRequestImgCompleted));
	request->setTag(to_string(tag));
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void SFSRequest::LoadTextureFromURL(std::string url)
{
	cocos2d::network::HttpRequest* request = new (std::nothrow) cocos2d::network::HttpRequest();
	request->setUrl(url);
	request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	request->setResponseCallback(this, httpresponse_selector(SFSRequest::onRequestTextureCompleted));
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void SFSRequest::onHttpRequest(cocos2d::network::HttpClient * client, cocos2d::network::HttpResponse * response)
{
	if (!response)
	{
		return;
	}
	/*if (0 != strlen(response->getHttpRequest()->getTag()))
	{
		CCLOG("%s completed", response->getHttpRequest()->getTag());
	}*/

	// Check the HTTP Status Code
	int statusCode = response->getResponseCode();
	char statusString[64] = {};
	sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
	//CCLOG("response code: %d", statusCode);

	// A connection failure
	if (!response->isSucceed())
	{
		CCLOG("response failed");
		CCLOG("error buffer: %s", response->getErrorBuffer());
		if (SFSRequest::getSingleton().onHttpResponseFailed != NULL) {
			SFSRequest::getSingleton().onHttpResponseFailed();
		}
		return;
	}

	// The data will be placed in the buffer.
	std::vector<char> * buffer = response->getResponseData();
	char * concatenated = (char *)malloc(buffer->size() + 1);
	std::string s2(buffer->begin(), buffer->end());
	strcpy(concatenated, s2.c_str());

	if (SFSRequest::getSingleton().onHttpResponse != NULL) {
		int tag = atoi(response->getHttpRequest()->getTag());
		SFSRequest::getSingleton().onHttpResponse(tag, concatenated);
	}
}

void SFSRequest::onRequestImgCompleted(cocos2d::network::HttpClient * client, cocos2d::network::HttpResponse * response)
{
	//CCLOG("onHttpRequestCompleted BEGIN");
	if (!response)
	{
		//CCLOG("onHttpRequestCompleted - No Response");
		return;
	}

	//CCLOG("onHttpRequestCompleted - Response code: %lu", response->getResponseCode());

	if (!response->isSucceed())
	{
		//CCLOG("onHttpRequestCompleted - Response failed");
		//CCLOG("onHttpRequestCompleted - Error buffer: %s", response->getErrorBuffer());
		return;
	}
	//CCLOG("onHttpRequestCompleted - Response code: %s", response->getResponseDataString());

	try {
		std::vector<char> *buffer = response->getResponseData();
		const char* file_char = buffer->data();
		cocos2d::Image * image = new  cocos2d::Image();
		image->initWithImageData(reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
		cocos2d::Texture2D * texture = new  cocos2d::Texture2D();
		texture->initWithImage(image);

		//CCLOG("onHttpRequestCompleted height %d", image->getHeight());
		int tag = atoi(response->getHttpRequest()->getTag());
		if (SFSRequest::getSingleton().onLoadImageResponse != NULL) {
			SFSRequest::getSingleton().onLoadImageResponse(tag, texture);
		}
	} catch (exception e) {
		CCLOG("Load image error: %s", e.what());
	}
}

void SFSRequest::onRequestTextureCompleted(cocos2d::network::HttpClient * client, cocos2d::network::HttpResponse * response)
{
	//CCLOG("onHttpRequestCompleted BEGIN");
	if (!response)
	{
		//CCLOG("onHttpRequestCompleted - No Response");
		return;
	}

	//CCLOG("onHttpRequestCompleted - Response code: %lu", response->getResponseCode());

	if (!response->isSucceed())
	{
		//CCLOG("onHttpRequestCompleted - Response failed");
		//CCLOG("onHttpRequestCompleted - Error buffer: %s", response->getErrorBuffer());
		return;
	}
	//CCLOG("onHttpRequestCompleted - Response code: %s", response->getResponseDataString());

	try {
		std::vector<char> *buffer = response->getResponseData();
		const char* file_char = buffer->data();
		cocos2d::Image * image = new  cocos2d::Image();
		image->initWithImageData(reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
		cocos2d::Texture2D * texture = new  cocos2d::Texture2D();
		texture->initWithImage(image);

		//CCLOG("onHttpRequestCompleted height %d", image->getHeight());
		std::string url = response->getHttpRequest()->getUrl();
		if (SFSRequest::getSingleton().onLoadTextureResponse != NULL) {
			SFSRequest::getSingleton().onLoadTextureResponse(url, texture);
		}
	} catch (exception e) {
		CCLOG("Load texture error: %s", e.what());
	}
}
