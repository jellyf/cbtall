#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "EventHandler.h"
#include "json/document.h"
#include "Utils.h"

extern "C"
{
	JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackLoginFacebook(JNIEnv* env, jobject thiz, jstring stringParam);
	JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackPurchaseSuccess(JNIEnv* env, jobject thiz, jstring jPurchaseToken);
	JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackQueryIAPProduct(JNIEnv* env, jobject thiz, jobjectArray stringArray);
	JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackFacebookInvite(JNIEnv* env, jobject thiz, jstring stringParam);
};

JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackLoginFacebook(JNIEnv* env, jobject thiz, jstring stringParam)
{
	const char* str = env->GetStringUTFChars(stringParam, NULL);

	std::string token = std::string(str);
	EventHandler::callbackLoginFacebook(token);

	env->ReleaseStringUTFChars(stringParam, str);
}

JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackQueryIAPProduct(JNIEnv* env, jobject thiz, jobjectArray stringArray)
{
	std::vector<ProductData> products;
	int stringCount = env->GetArrayLength(stringArray);
	for (int i = 0; i < stringCount; i++) {
		jstring str = (jstring)(env->GetObjectArrayElement(stringArray, i));
		const char *rawString = env->GetStringUTFChars(str, 0);
		
		rapidjson::Document d;
		d.Parse<0>(rawString);
		ProductData data;
		data.Price = d["price_amount_micros"].GetDouble() / 1000000;
		data.Id = d["productId"].GetString();
		data.Description = d["description"].GetString();
		data.CurrencySymbol = d["price_currency_code"].GetString();

		//int index = data.Id.find_last_of('.');
		//data.Description = data.Id.substr(index + 1, data.Id.length() - index - 1) + ".000 Quan";

		products.push_back(data);

		env->ReleaseStringUTFChars(str, rawString);
	}
	Utils::getSingleton().setIAPProducts(products);
}

JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackPurchaseSuccess(JNIEnv* env, jobject thiz, jstring jPurchaseToken)
{
	const char* strToken = env->GetStringUTFChars(jPurchaseToken, NULL);

	std::string token = std::string(strToken);
	Utils::callbackPurchaseSuccess(token);

	env->ReleaseStringUTFChars(jPurchaseToken, strToken);
}

JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackFacebookInvite(JNIEnv* env, jobject thiz, jstring stringParam)
{
	const char* str = env->GetStringUTFChars(stringParam, NULL);

	std::string token = std::string(str);
	if (EventHandler::getSingleton().onFacebookInvite != NULL) {
		EventHandler::getSingleton().onFacebookInvite(token);
	}

	env->ReleaseStringUTFChars(stringParam, str);
}
#endif
