#pragma once
#include "BaseScene.h"
#include "md5.h"
class LoginScene : public BaseScene
{
public:
	CREATE_FUNC(LoginScene);
	virtual void onInit();
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();
	virtual void onDownloadedPlistTexture(int numb);
	virtual void onHttpResponse(int tag, std::string content);
	virtual void onHttpResponseFailed(int tag);

	void onConnected();
	void onLoginZone();
	void onLogoutZone();
    void onConnectionException();
	void onConfigZoneReceived();
	void onUserDataMeResponse();
	void onLoginFacebook(std::string token);
	void onTableDataResponse(LobbyListTable data);
protected:
	virtual bool onErrorResponse(unsigned char code, std::string msg);
	virtual bool onConnectionLost(std::string reason);
	virtual void onConnectionFailed();
	virtual bool onKeyBack();
private:
	void loginNormal();
	void loginFacebook();
	void initRegisterNode();
	void requestGameConfig(bool realConfig);
	void loadTextureCache();
	void updateStateToGoToMain(int state);
    
	int waitingLogin = 0;
	int currentConfigLink = 0;
	int tmpZoneIndex;
	int isReadyToMain = 0;
	std::string fbToken = "";
	std::string load_host = "http://115.84.179.242/main_kinhtuchi/";
	bool isReconnecting = false;
	bool isLogedInZone = false;
	bool isRequesting = false;
	bool isRealConfig = true;
	bool isFirstLoadConfig = true;
	bool isLoginFacebook = false;
	bool isIPv4 = CC_TARGET_PLATFORM == CC_PLATFORM_IOS;
    bool connectionExceptionSolved = false;

	cocos2d::Node* loginNode;
	cocos2d::Node* registerNode;
	cocos2d::ui::Button* btnPhone;
	cocos2d::ui::Button* btnForgotPass;
	cocos2d::ui::EditBox* tfUsername;
	cocos2d::ui::EditBox* tfPassword;
	cocos2d::ui::EditBox* tfResUname;
	cocos2d::ui::EditBox* tfResPass;
	cocos2d::ui::EditBox* tfResPassAgain;
	//cocos2d::ui::EditBox* tfResEmail;
	cocos2d::Label* labelPhone;
	cocos2d::Label* lbBtnForgotPass;

	std::string testMessage;
	std::string testDevice;
};

