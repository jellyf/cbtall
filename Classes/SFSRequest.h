#pragma once
#include "cocos2d.h"
#include "SingLeton.h"
#include "network/HttpClient.h"

class SFSRequest : public SingLeton<SFSRequest>, cocos2d::Ref
{
public:
	SFSRequest();
	~SFSRequest();

	static SFSRequest& getSingleton(void);
	static SFSRequest* getSingletonPtr(void);

	std::function<void()> onHttpResponseFailed = NULL;
	std::function<void(int, std::string)> onHttpResponse = NULL;
	std::function<void(int, cocos2d::Texture2D*)> onLoadImageResponse = NULL;
	std::function<void(std::string url, cocos2d::Texture2D*)> onLoadTextureResponse = NULL;

	void Connect();
	void Connect(std::string host, int port);
	void Disconnect();
	void ProcessEvents();
	void LoginZone(std::string username, std::string password, std::string zone);
	void LogoutZone();
	void SendPublicMessage(std::string message);
	void Ping(long long time);
	void Ping();
	void ForceIPv6(bool value);

	void RequestRegister(std::string username, std::string password, std::string email);
	void RequestLogin(std::string username, std::string password);
	void RequestLoginFacebook(std::string token);
	void RequestJoinRoom(std::string roomId, bool isReconnect = true);
	void RequestLeaveRoom();
	void RequestChargeCard(std::string code, std::string seri, std::string serviceId);
	void RequestUserInfo(std::string username);
	void RequestRank();
	void RequestRankWin();
	void RequestPlayHistory(int type, int page);
	void RequestShopHistory(int page);
	void RequestShopItems();
	void RequestExchangeItem(unsigned char id);
	void RequestListMail(int page);
	void RequestMailContent(long id);
	void RequestUpdateDisplayName(std::string name);
	void RequestNews(int page);
	void RequestGiftcode(std::string giftcode);
    void RequestPayment(std::string token);

	void RequestGameReady();
	void RequestGameChooseStilt(int stilt);
	void RequestGameChooseHost(int host);
	void RequestGameBash(unsigned char card, unsigned char group);
	void RequestGameBashBack(unsigned char card, unsigned char group);
	void RequestGameHold(unsigned char card, unsigned char group);
	void RequestGamePick();
	void RequestGameForward();
    void RequestGamePenet();
    void RequestGameDropPenet();
	void RequestGameWin();
	void RequestGameDropWin();
	void RequestGameWinCrest(std::vector<unsigned char> crests);
	void RequestGameLobbyUser();
	void RequestGameInvitePlayer(long uid);
	void RequestGameTableInfo(bool isQuick, bool isU411);
	void RequestGameGa();

	void RequestHttpGet(std::string url, int tag);
	void LoadImageFromURL(std::string url, int tag);
	void LoadTextureFromURL(std::string url);
private:
	void onHttpRequest(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);
	void onRequestImgCompleted(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);
	void onRequestTextureCompleted(cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response);
};

