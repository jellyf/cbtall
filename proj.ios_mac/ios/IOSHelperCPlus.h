//
//  IOSHelperCPlus.h
//  TestSmartfox
//
//  Created by thanhphonglan2003 on 12/9/16.
//
//

#ifndef IOSHelperCPlus_h
#define IOSHelperCPlus_h

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class IOSHelperCPlus
{
public :
    IOSHelperCPlus();
    ~IOSHelperCPlus();
    static void setRootView(void* rootView);
    static void setLoginFBCallback(std::function<void(std::string)> callback);
    static void setPurchaseSuccessCallback(std::function<void(std::string)> callback);
    static void setFacebookInviteCallback(std::function<void(std::string)> callback);
    
    static void openSMS(const char* number, const char* text);
    static void openTel(const char* number);
    static void loginFacebook();
    static void logoutFacebook();
    static void purchaseItem(std::string sku);
    static void queryIAPProducts(std::vector<std::string> productIds);
    static void inviteFacebookFriends();
	static string getDeviceId();
};

#endif /* IOSHelperCPlus_h */
