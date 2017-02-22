LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)
$(call import-add-path,$(LOCAL_PATH)/../../../libs)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../../Classes/AppDelegate.cpp \
                   ../../../Classes/BaseScene.cpp \
                   ../../../Classes/EventHandler.cpp \
                   ../../../Classes/GameScene.cpp \
                   ../../../Classes/jnicall.cpp \
                   ../../../Classes/LoadScene.cpp \
                   ../../../Classes/LobbyScene.cpp \
                   ../../../Classes/LoginScene.cpp \
                   ../../../Classes/MainScene.cpp \
                   ../../../Classes/md5.cpp \
                   ../../../Classes/SFSConnector.cpp \
                   ../../../Classes/SFSRequest.cpp \
                   ../../../Classes/SFSResponse.cpp \
                   ../../../Classes/UserNode.cpp \
                   ../../../Classes/Utils.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static smartfoxclient_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,smartfox_163/android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
