//
//  IOSHelper.m
//  TestSmartfox
//
//  Created by thanhphonglan2003 on 12/9/16.
//
//

#import "IOSHelper.h"
#import "RootViewController.h"

static RootViewController *rootView = nil;
static std::function<void(std::string)> callbackLoginFacebook = nil;
static std::function<void(std::string)> callbackPurchaseSuccess = nil;

void IOSHelperCPlus::openSMS(const char* number, const char* text)
{
    std::string str = "sms:" + std::string(number) + "&body=" + std::string(text);
    NSString *sms = @(str.c_str());
    NSString *url = [sms stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
}

void IOSHelperCPlus::openTel(const char* number)
{
    std::string strPhone = "tel:" + std::string(number);
    NSString *phoneNumber = @(strPhone.c_str());
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:phoneNumber]];
}

void IOSHelperCPlus::loginFacebook()
{
    if(rootView != nil){
        [rootView loginFacebook];
    }
}

void IOSHelperCPlus::logoutFacebook()
{
    if(rootView != nil){
        [rootView logoutFacebook];
    }
}

void IOSHelperCPlus::setRootView(void* view)
{
    rootView = (RootViewController*) view;
}

void IOSHelperCPlus::setLoginFBCallback(std::function<void(std::string)> callback)
{
    callbackLoginFacebook = callback;
}

void IOSHelperCPlus::setPurchaseSuccessCallback(std::function<void(std::string)> callback)
{
    callbackPurchaseSuccess = callback;
}

void IOSHelperCPlus::purchaseItem(std::string sku)
{
    if(rootView != nil){
        [rootView purchaseItem:@(sku.c_str())];
    }
}

void IOSHelperCPlus::queryIAPProducts(std::vector<std::string> productIds)
{
    if(productIds.size() == 0) return;
    NSSet *set = [[NSSet alloc ]init];
    for(std::string id : productIds){
        set = [set setByAddingObject:@(id.c_str())];
    }
    if(rootView != nil){
        [rootView queryIAPProducts:set];
    }
}

string IOSHelperCPlus::getDeviceId()
{
    return [[[[[UIDevice currentDevice] identifierForVendor] UUIDString] lowercaseString] UTF8String];
}

@implementation IOSNDKHelper

+(void)OnLoginFacebookCallback:(NSString *)token
{
    if(callbackLoginFacebook != nil){
        callbackLoginFacebook(std::string([token UTF8String]));
    }
}

+ (void)SetNDKReciever:(NSObject*)reciever
{
    IOSHelperCPlus::setRootView((void*)reciever);
}

+(void)OnPurchaseSuccess:(NSString *)token
{
    if(callbackPurchaseSuccess != nil){
        callbackPurchaseSuccess(std::string([token UTF8String]));
    }
}
@end
