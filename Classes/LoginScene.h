#pragma once
#include "BaseScene.h"
#include "md5.h"
class LoginScene : public BaseScene, public cocos2d::ui::EditBoxDelegate
{
public:
	CREATE_FUNC(LoginScene);
	virtual void onInit();
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();
	virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);

	void onConnected();
	void onLoginZone();
	void onConnectionLost(std::string reason);
	void onConfigZoneReceived();
	void onUserDataMeResponse();
	void onLoginFacebook(std::string token);
	void onErrorResponse(unsigned char code, std::string msg);
	void onHttpResponse(int tag, std::string content);
	void onHttpResponseFailed();
	void onTableDataResponse(LobbyListTable data);
protected:
	virtual void onConnectionFailed();
	virtual void onKeyBack();
private:
	void loginNormal();
	void loginFacebook();
	void initRegisterNode();
	void requestGameConfig();
	void loadTextureCache();
    
	int waitingLogin = 0;
	int currentConfigLink = 0;
	int tmpZoneIndex;
	std::string fbToken = "";
	bool isReconnecting = false;
	bool isLogedInZone = false;
	bool isRequesting = false;
	bool isIPv4 = CC_TARGET_PLATFORM == CC_PLATFORM_IOS;

	cocos2d::Node* loginNode;
	cocos2d::Node* registerNode;
	cocos2d::ui::EditBox* tfUsername;
	cocos2d::ui::EditBox* tfPassword;
	cocos2d::ui::EditBox* tfResUname;
	cocos2d::ui::EditBox* tfResPass;
	cocos2d::ui::EditBox* tfResPassAgain;
	//cocos2d::ui::EditBox* tfResEmail;
	cocos2d::Label* labelPhone;
};

