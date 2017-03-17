package org.cocos2dx.cpp;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.facebook.appevents.AppEventsConstants;
import com.facebook.appevents.AppEventsLogger;

import java.math.BigDecimal;
import java.util.Currency;

/**
 * Created by Stormus on 3/14/2017.
 */

public class Tracker {

    private static String EVENT_ACTIVE_ACCOUNT = "CompletedActive";

    private static Tracker _instance;
    private AppEventsLogger logger;

    public static void Init(Context context){
        _instance = new Tracker();
        _instance.ActiveApp(context);
    }

    public void ActiveApp(Context context){
        AppEventsLogger.activateApp(context);
        logger = AppEventsLogger.newLogger(context);
    }

    public void logRegistration(String method){
        Log.d("MyTracker::", "Registration:: " + method);
        Bundle params = new Bundle();
        params.putString(AppEventsConstants.EVENT_PARAM_REGISTRATION_METHOD, method);
        logger.logEvent(AppEventsConstants.EVENT_NAME_COMPLETED_REGISTRATION, params);
    }

    public void logActiveAccount(){
        Log.d("MyTracker::", "ActiveAccount::" + EVENT_ACTIVE_ACCOUNT);
        logger.logEvent(EVENT_ACTIVE_ACCOUNT);
    }

    public void logPurchasedSuccess (String contentType, String contentId, String currency, double price) {
        Log.d("MyTracker::", "PurchasedSuccess::" + contentType + " -- " + contentId + " -- " + currency + " -- " + price);
        Bundle params = new Bundle();
        params.putString(AppEventsConstants.EVENT_PARAM_CONTENT_TYPE, contentType);
        params.putString(AppEventsConstants.EVENT_PARAM_CONTENT_ID, contentId);
        params.putString(AppEventsConstants.EVENT_PARAM_CURRENCY, currency);
        logger.logPurchase(BigDecimal.valueOf(price), Currency.getInstance(currency), params);
    }

    public static void trackRegistration(String method){
        _instance.logRegistration(method);
    }

    public static void trackActiveAccount(){
        _instance.logActiveAccount();
    }

    public static void trackPurchaseSuccess(String contentType, String contentId, String currency, double price){
        _instance.logPurchasedSuccess(contentType, contentId, currency, price);
    }
}
