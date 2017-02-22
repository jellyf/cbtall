/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.cpp;

import java.util.Arrays;
import java.util.Locale;

import org.cocos2dx.lib.Cocos2dxActivity;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.onesignal.OneSignal;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;

public class AppActivity extends Cocos2dxActivity {

    public static Activity _activity;
    private CallbackManager callbackManager;

    public static native void callbackLoginFacebook(String token);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        OneSignal.startInit(this).inFocusDisplaying(OneSignal.OSInFocusDisplayOption.None).init();
        FacebookSdk.sdkInitialize(getApplicationContext());
        callbackManager = CallbackManager.Factory.create();
        LoginManager.getInstance().registerCallback(callbackManager,
                new FacebookCallback<LoginResult>() {

                    public void onCancel() {
                        Log.d("ChanBachThu", "User canceled login facebook");
                        callbackLoginFacebook("cancel");
                    }

                    public void onError(FacebookException e) {
                        e.printStackTrace();
                        callbackLoginFacebook("");
                    }

                    public void onSuccess(LoginResult result) {
                        Log.d("LoginFacebook", "AccessToken: " + AccessToken.getCurrentAccessToken().getToken());
                        callbackLoginFacebook(AccessToken.getCurrentAccessToken().getToken());
                    }
                });

        setKeepScreenOn(true);
        _activity = this;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        callbackManager.onActivityResult(requestCode, resultCode, data);
    }

    public static void openSMS(String address, String smsBody) {
        Intent sendIntent = new Intent(Intent.ACTION_VIEW);
        sendIntent.putExtra("address", address);
        sendIntent.putExtra("sms_body", smsBody);
        sendIntent.setType("vnd.android-dir/mms-sms");
        AppActivity._activity.startActivity(sendIntent);
    }

    public static void openCall(String phone) {
        Intent intent = new Intent(Intent.ACTION_DIAL, Uri.fromParts("tel", phone, null));
        AppActivity._activity.startActivity(intent);
    }

    public static void loginFacebook(){
        if(AccessToken.getCurrentAccessToken() != null){
            if(AccessToken.getCurrentAccessToken().isExpired()){
                AccessToken.refreshCurrentAccessTokenAsync(new AccessToken.AccessTokenRefreshCallback() {
                    @Override
                    public void OnTokenRefreshed(AccessToken accessToken) {
                        callbackLoginFacebook(AccessToken.getCurrentAccessToken().getToken());
                    }
                    @Override
                    public void OnTokenRefreshFailed(FacebookException e) {
                        e.printStackTrace();
                        callbackLoginFacebook("");
                    }
                });
            }else {
                callbackLoginFacebook(AccessToken.getCurrentAccessToken().getToken());
            }
        }else {
            LoginManager.getInstance().logInWithReadPermissions(AppActivity._activity, Arrays.asList("public_profile"));
        }
    }

    public static void logoutFacebook(){
        LoginManager.getInstance().logOut();
    }

    public static String getUserCountry() {
        try {
            final TelephonyManager tm = (TelephonyManager)AppActivity._activity.getSystemService(Context.TELEPHONY_SERVICE);
            final String simCountry = tm.getSimCountryIso();
            if (simCountry != null && simCountry.length() == 2) {
                // SIM country code is available

                return simCountry.toLowerCase(Locale.US);
            } else if (tm.getPhoneType() != TelephonyManager.PHONE_TYPE_CDMA) {
                // device is not3G (would be unreliable)

                String networkCountry = tm.getNetworkCountryIso();
                if (networkCountry != null && networkCountry.length() == 2) {
                    // network country code is
                    // available

                    return networkCountry.toLowerCase(Locale.US);
                }
            } else {
                return simCountry.toLowerCase(Locale.US);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
        return "vn";
    }

    public static String getDeviceId() {
        try {
            return AdvertisingIdClient.getAdvertisingIdInfo(getContext()).getId();
        }catch (Exception e){
            e.printStackTrace();
        }
        return Settings.System.getString(getContext().getContentResolver(),Settings.Secure.ANDROID_ID);
    }
}
