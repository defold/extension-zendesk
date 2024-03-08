package com.defold.zendesk;

import android.app.Activity;
import android.util.Log;

import org.json.JSONObject;
import org.json.JSONException;

import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;

import kotlin.Unit;

// https://zendesk.github.io/mobile_sdk_javadocs/zendesk-sdks/zendesk-android/1.17.0/zendesk/android/package-summary.html
import zendesk.android.Zendesk;
import zendesk.android.ZendeskUser;
import zendesk.android.SuccessCallback;
import zendesk.android.FailureCallback;

// https://zendesk.github.io/mobile_sdk_javadocs/zendesk-sdks/zendesk-android/1.17.0/zendesk/android/events/package-summary.html
import zendesk.android.events.ZendeskEventListener;
import zendesk.android.events.ZendeskEvent;
import zendesk.android.events.ZendeskEvent.UnreadMessageCountChanged;
import zendesk.android.events.ZendeskEvent.AuthenticationFailed;
import zendesk.android.events.ZendeskEvent.FieldValidationFailed;

// https://zendesk.github.io/mobile_sdk_javadocs/zendesk-sdks/messaging-android/2.17.0/zendesk/messaging/android/package-summary.html
import zendesk.messaging.android.DefaultMessagingFactory;


public class ZendeskJNI implements ZendeskEventListener {
    private static final String TAG = "ZendeskJNI";

    public static native void zendeskAddToQueue(int msg, String json);

    // duplicate of enums from zendesk_callback.h:
    // CONSTANTS:
    private static final int MSG_INIT_ERROR =                   0;
    private static final int MSG_INIT_SUCCESS =                 1;
    private static final int MSG_INTERNAL_ERROR =               2;
    private static final int MSG_ERROR =                        3;
    private static final int MSG_UNREAD_MESSAGE_COUNT_CHANGED = 4;
    private static final int MSG_AUTHENTICATION_FAILED =        5;
    private static final int MSG_FIELD_VALIDATION_FAILED =      6;
    private static final int MSG_LOGIN_SUCCESS =                7;
    private static final int MSG_LOGIN_FAILED =                 8;
    private static final int MSG_LOGOUT_SUCCESS =               9;
    private static final int MSG_LOGOUT_FAILED =                10;

    private Activity activity;

    private Map<String, Object> conversationFields = new HashMap<>();
    private List<String> conversationTags = new ArrayList<>();

    public ZendeskJNI(Activity activity) {
        this.activity = activity;
    }

    public void initialize(String channel) {
        Zendesk.initialize(
            activity,
            channel,
            new SuccessCallback<Zendesk>() {
                @Override
                public void onSuccess(Zendesk zendesk) {
                    Log.i(TAG, "Initialized successfully");
                    sendSimpleMessage(MSG_INIT_SUCCESS);
                    zendesk.addEventListener(ZendeskJNI.this);
                }
            },
            new FailureCallback<Throwable>() {
                @Override
                public void onFailure(Throwable t) {
                    Log.e(TAG, "Failed to initialize", t);
                    sendSimpleMessage(MSG_INIT_ERROR, "error", t.getLocalizedMessage());
                }
            },
            new DefaultMessagingFactory());
    }

    public void showMessaging() {
        Log.i(TAG, "Showing messaging");
        Zendesk.getInstance().getMessaging().showMessaging(activity);
    }

    public void login(String jwt) {
        Log.i(TAG, "Login");
        Zendesk.getInstance().loginUser(
            jwt,
            new SuccessCallback<ZendeskUser>() {
                @Override
                public void onSuccess(ZendeskUser user) {
                    Log.i(TAG, "Login successful");
                    sendSimpleMessage(MSG_LOGIN_SUCCESS);
                }
            },
            new FailureCallback<Throwable>() {
                @Override
                public void onFailure(Throwable t) {
                    Log.e(TAG, "Failed to login", t);
                    sendSimpleMessage(MSG_LOGIN_FAILED, "error", t.getLocalizedMessage());
                }
            });
    }

    public void logout() {
        Log.i(TAG, "Logout");
        Zendesk.getInstance().logoutUser(
            new SuccessCallback<Unit>() {
                @Override
                public void onSuccess(Unit u) {
                    Log.i(TAG, "Logout successful");
                    sendSimpleMessage(MSG_LOGOUT_SUCCESS);
                }
            },
            new FailureCallback<Throwable>() {
                @Override
                public void onFailure(Throwable t) {
                    Log.e(TAG, "Failed to logout", t);
                    sendSimpleMessage(MSG_LOGOUT_FAILED, "error", t.getLocalizedMessage());
                }
            });
    }

    public void addConversationFieldString(String key, String value) {
        Log.i(TAG, "Add conversation field string");
        conversationFields.put(key, value);
        Zendesk.getInstance().getMessaging().setConversationFields(conversationFields);
    }

    public void addConversationFieldFloat(String key, float value) {
        Log.i(TAG, "Add conversation field float");
        conversationFields.put(key, value);
        Zendesk.getInstance().getMessaging().setConversationFields(conversationFields);
    }

    public void addConversationFieldBoolean(String key, boolean value) {
        Log.i(TAG, "Add conversation field boolean");
        conversationFields.put(key, value);
        Zendesk.getInstance().getMessaging().setConversationFields(conversationFields);
    }

    public void clearConversationFields() {
        Log.i(TAG, "Clear conversation fields");
        conversationFields.clear();
        Zendesk.getInstance().getMessaging().clearConversationFields();
    }

    public void addConversationTag(String tag) {
        Log.i(TAG, "Add conversation tag");
        conversationTags.add(tag);
        Zendesk.getInstance().getMessaging().setConversationTags(conversationTags);
    }

    public void clearConversationTags() {
        Log.i(TAG, "Clear conversation tags");
        conversationTags.clear();
        Zendesk.getInstance().getMessaging().clearConversationTags();
    }

    @Override
    public void onEvent(ZendeskEvent zendeskEvent) {
        if (zendeskEvent instanceof UnreadMessageCountChanged) {
            Log.i(TAG, "UnreadMessageCountChanged");
            UnreadMessageCountChanged e = (UnreadMessageCountChanged)zendeskEvent;
            sendSimpleMessage(MSG_UNREAD_MESSAGE_COUNT_CHANGED, "unread_count", Integer.toString(e.getCurrentUnreadCount()));
        }
        else if (zendeskEvent instanceof AuthenticationFailed) {
            Log.i(TAG, "AuthenticationFailed");
            sendSimpleMessage(MSG_AUTHENTICATION_FAILED);
        }
        else if (zendeskEvent instanceof FieldValidationFailed) {
            Log.i(TAG, "FieldValidationFailed");
            sendSimpleMessage(MSG_FIELD_VALIDATION_FAILED);
        }
    }


    // PRIVATE FUNCTIONS FOR QUEING MESSAGES

    private String createKeyValueMessage(String key, String value) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(key, value);
        return obj.toString();
    }

    private String createErrorMessage(String errorText) {
        String message = null;
        try {
            message = createKeyValueMessage("error", errorText);
        }
        catch (JSONException e) {
            message = "{ \"error\": \"Error while converting simple message to JSON.\"}";
        }
        return message;
    }

    private void sendErrorMessage(String errorText) {
        String message = createErrorMessage(errorText);
        Log.d(TAG, "Sending error message " + message);
        zendeskAddToQueue(MSG_ERROR, message);
    }

    private void sendSimpleMessage(int msg) {
        Log.d(TAG, "Sending empty message");
        zendeskAddToQueue(msg, "{}");
    }

    private void sendSimpleMessage(int msg, String key, String value) {
        String message = null;
        try {
            message = createKeyValueMessage(key, value);
            Log.d(TAG, "Sending message " + message);
        }
        catch (JSONException e) {
            message = createErrorMessage(e.getLocalizedMessage());
            Log.e(TAG, "Sending error message", e);
            zendeskAddToQueue(MSG_INTERNAL_ERROR, message);
        }
    }
}