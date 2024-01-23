package com.defold.zendesk;

import android.app.Activity;
import android.util.Log;

import org.json.JSONObject;
import org.json.JSONException;

import zendesk.android.Zendesk;
import zendesk.messaging.android.DefaultMessagingFactory;


public class ZendeskJNI {
    private static final String TAG = "ZendeskJNI";

    public static native void zendeskAddToQueue(int msg, String json);

    // duplicate of enums from zendesk_callback.h:
    // CONSTANTS:
    private static final int MSG_INIT_ERROR =              0;
    private static final int MSG_INIT_SUCCESS =            1;
    private static final int MSG_INTERNAL_ERROR =          2;
    private static final int MSG_ERROR =                   3;

    private Activity activity;

    public ZendeskJNI(Activity activity) {
        this.activity = activity;
    }

    public void initialize(String channel) {
        Zendesk.initialize(
                activity,
                channel,
                zendesk -> {
                    Log.i("IntegrationApplication", "Initialization successful");
                    sendSimpleMessage(MSG_INIT_SUCCESS);
                },
                error -> {
                    Log.e("IntegrationApplication", "Messaging failed to initialize", error);
                    sendSimpleMessage(MSG_INIT_ERROR, "error", error.getLocalizedMessage());
                },
                new DefaultMessagingFactory());
    }

    public void showMessaging() {
        Zendesk.getInstance().getMessaging().showMessaging(activity);
    }

    // https://www.baeldung.com/java-json-escaping
    private String getJsonConversionErrorMessage(String errorText) {
        String message = null;

        try {
            JSONObject obj = new JSONObject();
            obj.put("error", errorText);
            message = obj.toString();
        } catch (JSONException e) {
            message = "{ \"error\": \"Error while converting simple message to JSON.\"}";
        }

        return message;
    }

    private void sendErrorMessage(String errorText) {
        String message = getJsonConversionErrorMessage(errorText);
        Log.d(TAG, "Zendesk Error");
        Log.d(TAG, message);
        zendeskAddToQueue(MSG_ERROR, message);
    }

    private void sendSimpleMessage(int msg) {
        zendeskAddToQueue(msg, "{}");
    }

    private void sendSimpleMessage(int msg, String key, String value) {
        String message = null;

        try {
            JSONObject obj = new JSONObject();
            obj.put(key, value);
            message = obj.toString();
            zendeskAddToQueue(msg, message);
        } catch (JSONException e) {
            message = getJsonConversionErrorMessage(e.getLocalizedMessage());
            zendeskAddToQueue(MSG_INTERNAL_ERROR, message);
        }
    }
}