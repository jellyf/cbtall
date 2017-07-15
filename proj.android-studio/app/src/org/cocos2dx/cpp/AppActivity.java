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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Locale;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.json.JSONArray;
import org.json.JSONObject;

import com.android.vending.billing.IInAppBillingService;
import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.applinks.AppLinkData;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.model.GameRequestContent;
import com.facebook.share.widget.AppInviteDialog;
import com.facebook.share.widget.GameRequestDialog;
import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.onesignal.OneSignal;

import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.provider.Settings;
import android.provider.Telephony;
import android.telephony.TelephonyManager;
import android.view.View;
import android.util.Log;

import bolts.AppLinks;

public class AppActivity extends Cocos2dxActivity {

    public static AppActivity _activity;
    private GameRequestDialog requestDialog;
    private AppInviteDialog appInviteDialog;
    private CallbackManager callbackManager;
    private IInAppBillingService mService;
    private ServiceConnection mServiceConn;
    private String lastDeveloperPayload = "";
    private int currentApiVersion;

    public static native void callbackLoginFacebook(String token);
    public static native void callbackPurchaseSuccess(String token);
    public static native void callbackQueryIAPProduct(Object[] jsonProducts);
    public static native void callbackFacebookInvite(String token);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Tracker.Init(this);
        OneSignal.startInit(this).inFocusDisplaying(OneSignal.OSInFocusDisplayOption.None).init();
        FacebookSdk.sdkInitialize(getApplicationContext());
        callbackManager = CallbackManager.Factory.create();
        LoginManager.getInstance().registerCallback(callbackManager,
                new FacebookCallback<LoginResult>() {

                    public void onCancel() {
                        //Log.d("Kinhtuchi::", "User canceled login facebook");
                        callbackLoginFacebook("cancel");
                    }

                    public void onError(FacebookException e) {
                        //Log.d("Kinhtuchi::", "exception");
                        e.printStackTrace();
                        callbackLoginFacebook("");
                    }

                    public void onSuccess(LoginResult result) {
                        //Log.d("KinhTuChi::", "FacebookAccessToken: " + AccessToken.getCurrentAccessToken().getToken());
                        callbackLoginFacebook(AccessToken.getCurrentAccessToken().getToken());
                    }
                }
        );

        requestDialog = new GameRequestDialog(this);
        requestDialog.registerCallback(callbackManager,
                new FacebookCallback<GameRequestDialog.Result>() {
                    public void onSuccess(GameRequestDialog.Result result) {
                        if(result != null && result.getRequestRecipients().size() > 0){
                            try {
                                JSONObject jo = new JSONObject();
                                jo.put("request", result.getRequestId());
                                jo.put("to", new JSONArray(result.getRequestRecipients()));
                                //Log.d("KinhTuChi::", "callbackFacebookInvite: " + jo.toString());
                                callbackFacebookInvite(jo.toString());
                            }catch (Exception e){
                                e.printStackTrace();
                                callbackFacebookInvite("");
                            }
                        }
                    }
                    public void onCancel() {
                        callbackFacebookInvite("");
                    }
                    public void onError(FacebookException error) {
                        callbackFacebookInvite("");
                    }
                }
        );

        Uri targetUrl = AppLinks.getTargetUrlFromInboundIntent(this, getIntent());
        if (targetUrl != null) {
            Log.i("Activity", "App Link Target URL: " + targetUrl.toString());
        } else {
            AppLinkData.fetchDeferredAppLinkData(this,
                    new AppLinkData.CompletionHandler() {
                        @Override
                        public void onDeferredAppLinkDataFetched(AppLinkData appLinkData) {
                            //Log.d("KinhTuChi::", "AppLinkData:: " + appLinkData.getRef());
                        }
                    });
        }

        mServiceConn = new ServiceConnection() {
            @Override
            public void onServiceDisconnected(ComponentName name) {
                mService = null;
            }

            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                mService = IInAppBillingService.Stub.asInterface(service);
            }
        };

        Intent serviceIntent = new Intent("com.android.vending.billing.InAppBillingService.BIND");
        serviceIntent.setPackage("com.android.vending");
        bindService(serviceIntent, mServiceConn, Context.BIND_AUTO_CREATE);

        currentApiVersion = android.os.Build.VERSION.SDK_INT;
        final int flags = View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;

        if(currentApiVersion >= Build.VERSION_CODES.KITKAT){
            getWindow().getDecorView().setSystemUiVisibility(flags);
            final View decorView = getWindow().getDecorView();
            decorView.setOnSystemUiVisibilityChangeListener(
                    new View.OnSystemUiVisibilityChangeListener(){
                        @Override
                        public void onSystemUiVisibilityChange(int visibility){
                            if((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0){
                                decorView.setSystemUiVisibility(flags);
                            }
                        }
                    });
        }

        setKeepScreenOn(true);
        _activity = this;
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        if(currentApiVersion >= Build.VERSION_CODES.KITKAT && hasFocus){
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        //Log.d("KinhTuChi::", "onActivityResult::" + requestCode);
        if (requestCode == 1001) {
            int responseCode = data.getIntExtra("RESPONSE_CODE", 0);
            String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
            String dataSignature = data.getStringExtra("INAPP_DATA_SIGNATURE");

            //Log.d("KinhTuChi::", "OnPurchase::" + responseCode);
            if (resultCode == RESULT_OK) {
                //Log.d("KinhTuChi::", "OnPurchase::" + purchaseData);
                try {
                    JSONObject jo = new JSONObject(purchaseData);
                    String token = jo.getString("purchaseToken");
                    //String sku = jo.getString("productId");
                    //jo.put("signature", dataSignature);

                    //Log.d("KinhTuChi::", "OnPurchase::Success::" + jo.toString());
                    callbackPurchaseSuccess(purchaseData + " " + dataSignature + " " + lastDeveloperPayload);
                    int response = mService.consumePurchase(3, getPackageName(), token);
                    //Log.d("KinhTuChi::", "OnPurchase::Consume:" + response);
                    lastDeveloperPayload = "";
                }
                catch (Exception e) {
                    //Log.d("KinhTuChi::", "OnPurchase::Failed data");
                    e.printStackTrace();
                    callbackPurchaseSuccess("");
                }
            }else if (resultCode == RESULT_CANCELED) {
                //Log.d("KinhTuChi::", "OnPurchase::Cancel");
                callbackPurchaseSuccess("");
            }else if(resultCode == 7) { //Already owned
                consumeAllPurchasedItem();
            }else {
                //Log.d("KinhTuChi::", "OnPurchase::Failed");
                callbackPurchaseSuccess("");
            }
        }else{
            callbackManager.onActivityResult(requestCode, resultCode, data);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mService != null) {
            unbindService(mServiceConn);
        }
    }

    public void queryIAPItem(String[] ids) {
        ArrayList<String> skuList = new ArrayList<>();
        for(int i=0;i<ids.length;i++){
            //Log.d("KinhTuChi::", "QueryIAP::" + ids[i]);
            if(ids[i].length() > 0){
                skuList.add(ids[i]);
            }
        }
        Bundle querySkus = new Bundle();
        querySkus.putStringArrayList("ITEM_ID_LIST", skuList);

        try {
            Bundle skuDetails = mService.getSkuDetails(3, getPackageName(), "inapp", querySkus);

            int response = skuDetails.getInt("RESPONSE_CODE");
            //Log.d("KinhTuChi::", "QueryIAP::" + response);
            if (response == 0) {
                ArrayList<String> responseList = skuDetails.getStringArrayList("DETAILS_LIST");
                //Log.d("KinhTuChi::", "QueryIAP::" + responseList.size());
//                for (String thisResponse : responseList) {
//                    Log.d("KinhTuChi::", "QueryIAP::" + thisResponse);
//                    JSONObject object = new JSONObject(thisResponse);
//                    String sku = object.getString("productId");
//                    String price = object.getString("price");
//                    String title = object.getString("title");
//                    String description = object.getString("description");
//                    String currencyCode = object.getString("price_currency_code");
//                    String priceMicros = object.getString("price_amount_micros");
//                    Log.d("KinhTuChi::", "QueryIAP::" + sku + " -- " + price + " -- " + title + " -- " + description + " -- " + currencyCode + " -- " + priceMicros);
//                }
                callbackQueryIAPProduct(responseList.toArray());
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void purchaseItem(String sku) {
        lastDeveloperPayload = System.currentTimeMillis()+"";
        try {
            //Log.d("KinhTuChi::", "PurchaseItem::" + sku);
            Bundle buyIntentBundle = mService.getBuyIntent(3, getPackageName(), sku, "inapp", lastDeveloperPayload);
            PendingIntent pendingIntent = buyIntentBundle.getParcelable("BUY_INTENT");
            startIntentSenderForResult(pendingIntent.getIntentSender(), 1001, new Intent(), Integer.valueOf(0), Integer.valueOf(0), Integer.valueOf(0));
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public void consumeAllPurchasedItem(){
        try {
            //Log.d("KinhTuChi::", "ConsumeAllItem");
            Bundle ownedItems = mService.getPurchases(3, getPackageName(), "inapp", null);
            int response = ownedItems.getInt("RESPONSE_CODE");
            if (response == 0) {
                ArrayList<String> ownedSkus = ownedItems.getStringArrayList("INAPP_PURCHASE_ITEM_LIST");
                ArrayList<String> purchaseDataList = ownedItems.getStringArrayList("INAPP_PURCHASE_DATA_LIST");
                ArrayList<String> signatureList = ownedItems.getStringArrayList("INAPP_DATA_SIGNATURE_LIST");
                String continuationToken = ownedItems.getString("INAPP_CONTINUATION_TOKEN");

                for (int i = 0; i < purchaseDataList.size(); ++i) {
                    String purchaseData = purchaseDataList.get(i);
                    //String signature = signatureList.get(i);
                    //String sku = ownedSkus.get(i);
                    JSONObject object = new JSONObject(purchaseData);
                    String token = object.getString("purchaseToken");
                    //Log.d("KinhTuChi::", "ConsumeAllItem::" + sku + " -- " + purchaseData);
                    int responseCS = mService.consumePurchase(3, getPackageName(), token);
                    //Log.d("KinhTuChi::", "ConsumeAllItem::" + responseCS);
                }
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void openFacebookInvitable() {
        if(!FacebookSdk.isInitialized()){
            FacebookSdk.sdkInitialize(getApplicationContext(), new FacebookSdk.InitializeCallback() {
                @Override
                public void onInitialized() {
                    openFacebookInvitable();
                }
            });
            return;
        }
        GameRequestContent content = new GameRequestContent.Builder()
                .setMessage("Vào chơi Chắn Vương nào!")
                .build();
        requestDialog.show(content);
    }

    public static void purchaseProduct(String sku){
        AppActivity._activity.purchaseItem(sku);
    }

    public static void queryIAPProducts(String[] ids){
        AppActivity._activity.queryIAPItem(ids);
    }

    public static void openSMS(String address, String smsBody) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            String defaultSmsPackageName = Telephony.Sms.getDefaultSmsPackage(AppActivity._activity);
            Intent sendIntent = new Intent(Intent.ACTION_SEND);
            sendIntent.setType("text/plain");
            sendIntent.putExtra(Intent.EXTRA_TEXT, smsBody);
            sendIntent.putExtra("address", address);
            if (defaultSmsPackageName != null) {
                sendIntent.setPackage(defaultSmsPackageName);
            }
            AppActivity._activity.startActivity(sendIntent);
        } else {
            Intent sendIntent = new Intent(Intent.ACTION_VIEW);
            sendIntent.putExtra("address", address);
            sendIntent.putExtra("sms_body", smsBody);
            sendIntent.setType("vnd.android-dir/mms-sms");
            AppActivity._activity.startActivity(sendIntent);
        }
    }

    public static void openCall(String phone) {
        Intent intent = new Intent(Intent.ACTION_DIAL, Uri.fromParts("tel", phone, null));
        AppActivity._activity.startActivity(intent);
    }

    public static void loginFacebook(){
        if(!FacebookSdk.isInitialized()){
            FacebookSdk.sdkInitialize(AppActivity._activity.getApplicationContext(), new FacebookSdk.InitializeCallback() {
                @Override
                public void onInitialized() {
                    loginFacebook();
                }
            });
            return;
        }
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
            //LoginManager.getInstance().logInWithPublishPermissions(AppActivity._activity, Arrays.asList("publish_actions"));
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

    public static void inviteFacebookFriends() {
        AppActivity._activity.openFacebookInvitable();
    }
}
