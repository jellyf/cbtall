#include "cocos2d.h"
#include "EventHandler.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
	JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackLoginFacebook(JNIEnv* env, jobject thiz, jstring stringParam);
};

JNIEXPORT jstring JNICALL Java_org_cocos2dx_cpp_AppActivity_callbackLoginFacebook(JNIEnv* env, jobject thiz, jstring stringParam)
{
	const char* str = env->GetStringUTFChars(stringParam, NULL);

	std::string token = std::string(str);
	EventHandler::callbackLoginFacebook(token);

	env->ReleaseStringUTFChars(stringParam, str);
}
#endif
