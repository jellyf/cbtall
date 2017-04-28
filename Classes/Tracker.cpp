#include "Tracker.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "IOSHelperCPlus.h"
#endif

using namespace std;
using namespace cocos2d;

template<> Tracker* SingLeton<Tracker>::mSingleton = 0;
Tracker* Tracker::getSingletonPtr(void)
{
	return mSingleton;
}

Tracker& Tracker::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

Tracker::Tracker()
{
}

Tracker::~Tracker()
{
}

void Tracker::trackActiveAccount()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/Tracker", "trackActiveAccount", "()V")) {
		return;
	}
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSHelperCPlus::trackActiveAccount();
#endif
}

void Tracker::trackRegistration(std::string regisMethod)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/Tracker", "trackRegistration", "(Ljava/lang/String;)V")) {
		return;
	}
	jstring jregisMethod = methodInfo.env->NewStringUTF(regisMethod.c_str());
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jregisMethod);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSHelperCPlus::trackRegistration(regisMethod);
#endif
}

void Tracker::trackPurchaseSuccess(std::string type, std::string id, std::string currency, double price)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/cpp/Tracker", "trackPurchaseSuccess", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;D)V")) {
		return;
	}
	jstring jContentType = methodInfo.env->NewStringUTF(type.c_str());
	jstring jContentId = methodInfo.env->NewStringUTF(id.c_str());
	jstring jCurrency = methodInfo.env->NewStringUTF(currency.c_str());
	jdouble jprice = (jdouble)price;
	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jContentType, jContentId, jCurrency, jprice);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSHelperCPlus::trackPurchaseSuccess(type, id, currency, price);
#endif
}
