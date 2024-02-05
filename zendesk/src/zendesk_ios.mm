#if defined(DM_PLATFORM_IOS)

#include "zendesk_private.h"
#include "zendesk_callback.h"

#import <ZendeskSDKMessaging/ZendeskSDKMessaging.h>
#import <ZendeskSDK/ZendeskSDK.h>

@protocol MessagingFactory;

#import <ZendeskSDKMessaging/ZendeskSDKMessaging-Swift.h>
#import <ZendeskSDK/ZendeskSDK-Swift.h>

// https://zendesk.github.io/sdk_messaging_ios/
// https://zendesk.github.io/sdk_zendesk_ios/

@class Zendesk;
@class ZendeskSDKMessaging;
@class MessagingFactory;
@class ZDKDefaultMessagingFactory;
@class ZDKZendeskUser;

@interface ZendeskObserver : NSObject
@end

@implementation ZendeskObserver
@end

namespace dmZendesk
{
    struct ZendeskIOS
    {
        ZendeskIOS()
        {
            memset(this, 0, sizeof(*this));
        }

        ZendeskObserver*       m_ZendeskObserver;
        NSMutableDictionary*   m_ConversationFields;
        NSMutableArray*        m_ConversationTags;
        bool                   m_Initialized;
    };

    ZendeskIOS g_ZendeskIOS;

    // NSMutableDictionary *g_ConversationFields = [[NSMutableDictionary alloc] init];
    // bool g_Initialized = FALSE;
    // ZendeskExt* g_ZendeskExt = [[ZendeskExt alloc] init]

    void SendSimpleMessage(Message msg, id obj)
    {
        NSError* error;
        NSData* jsonData = [NSJSONSerialization dataWithJSONObject:obj options:(NSJSONWritingOptions)0 error:&error];

        if (jsonData)
        {
            NSString* nsstring = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            AddToQueueCallback(msg, (const char*)[nsstring UTF8String]);
            [nsstring release];
        }
        else
        {
            NSMutableDictionary *dict = [NSMutableDictionary dictionary];
            [dict setObject:error.localizedDescription forKey:@"error"];
            NSError* error2;
            NSData* errorJsonData = [NSJSONSerialization dataWithJSONObject:dict options:(NSJSONWritingOptions)0 error:&error2];
            if (errorJsonData)
            {
                NSString* nsstringError = [[NSString alloc] initWithData:errorJsonData encoding:NSUTF8StringEncoding];
                AddToQueueCallback(MSG_INTERNAL_ERROR, (const char*)[nsstringError UTF8String]);
                [nsstringError release];
            }
            else
            {
                AddToQueueCallback(MSG_INTERNAL_ERROR, [[NSString stringWithFormat:@"{ \"error\": \"Error while converting simple message to JSON.\"}"] UTF8String]);
            }
        }
    }

    void SendErrorMessage(NSError *error)
    {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        [dict setObject:error.localizedDescription forKey:@"error"];
        SendSimpleMessage(MSG_ERROR, dict);
    }

    void SendSimpleMessage(Message msg)
    {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        SendSimpleMessage(msg, dict);
    }

    void SendSimpleMessage(Message msg, NSString *key, NSString *value)
    {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        [dict setObject:value forKey:key];
        SendSimpleMessage(msg, dict);
    }

    void Initialize_Ext()
    {
        if (!g_ZendeskIOS.m_Initialized)
        {
            g_ZendeskIOS.m_Initialized = FALSE;
            g_ZendeskIOS.m_ConversationFields = [[NSMutableDictionary alloc] init];
            g_ZendeskIOS.m_ConversationTags = [[NSMutableArray alloc] init];
            g_ZendeskIOS.m_ZendeskObserver = [[ZendeskObserver alloc] init];
        }
    }

    void Finalize_Ext()
    {
        if (g_ZendeskIOS.m_Initialized)
        {
            [[Zendesk instance] removeEventObserver:g_ZendeskIOS.m_ZendeskObserver];
            delete g_ZendeskIOS.m_ConversationFields;
            delete g_ZendeskIOS.m_ConversationTags;
            g_ZendeskIOS.m_ConversationFields = NULL;
            g_ZendeskIOS.m_ConversationTags = NULL;
            g_ZendeskIOS.m_Initialized = FALSE;
        }
    }


    void Initialize(const char* channel) {
        [Zendesk initializeWithChannelKey:[NSString stringWithUTF8String:channel]
                         messagingFactory:[[ZDKDefaultMessagingFactory alloc] init]
                      completionHandler:^(Zendesk * _Nullable zendesk, NSError * _Nullable error) {
            if (error != nil)
            {
                NSLog(@"Zendesk did not initialize.\nError: %@", error.localizedDescription);
                SendSimpleMessage(MSG_INIT_ERROR, @"error", error.localizedDescription);
            }
            else
            {
                g_ZendeskIOS.m_Initialized = TRUE;
                SendSimpleMessage(MSG_INIT_SUCCESS);
                [zendesk addEventObserver:g_ZendeskIOS.m_ZendeskObserver :^(enum ZDKZendeskEvent event, id _Nullable value) {
                    switch (event)
                    {
                    case ZDKZendeskEventUnreadMessageCountChanged:
                        SendSimpleMessage(MSG_UNREAD_MESSAGE_COUNT_CHANGED);
                        break;
                    case ZDKZendeskEventAuthenticationFailed: 
                        SendSimpleMessage(MSG_AUTHENTICATION_FAILED);
                        break;
                    default:
                        break;
                    }
                }];
            }
        }];
    }

    void ShowMessaging()
    {
        UIViewController *viewController = [Zendesk.instance.messaging messagingViewController];
        if (viewController != NULL)
        {
            UIWindow* window = (UIWindow*)dmGraphics::GetNativeiOSUIWindow();
            [window.rootViewController.navigationController showViewController:viewController sender:window.rootViewController];
        }
    }

    void Login(const char* jwt)
    {
        [Zendesk.instance loginUserWith:[NSString stringWithUTF8String:jwt] completionHandler:^(ZDKZendeskUser * _Nullable user, NSError * _Nullable error) {
            if (error)
            {
                SendSimpleMessage(MSG_AUTHENTICATION_FAILED);
            }
            else
            {
                SendSimpleMessage(MSG_AUTHENTICATION_SUCCESS);
            }
        }];

    }

    void Logout()
    {
        [Zendesk.instance logoutUserWithCompletionHandler:^(NSError * _Nullable error) {
            if (error)
            {
                SendSimpleMessage(MSG_LOGOUT_FAILED);
            }
            else
            {
                SendSimpleMessage(MSG_LOGOUT_SUCCESS);
            }
        }];
    }

    void AddConversationFieldString(const char* key, const char* value)
    {
        [g_ZendeskIOS.m_ConversationFields setValue:[NSString stringWithUTF8String:value] forKey:[NSString stringWithUTF8String:key]];
        [Zendesk.instance.messaging setConversationFields:g_ZendeskIOS.m_ConversationFields];
    }

    void AddConversationFieldNumber(const char* key, const float value)
    {
        [g_ZendeskIOS.m_ConversationFields setValue:[NSNumber numberWithFloat:value] forKey:[NSString stringWithUTF8String:key]];
        [Zendesk.instance.messaging setConversationFields:g_ZendeskIOS.m_ConversationFields];
    }

    void AddConversationFieldBoolean(const char* key, const bool value)
    {
        [g_ZendeskIOS.m_ConversationFields setValue:[NSNumber numberWithBool:value] forKey:[NSString stringWithUTF8String:key]];
        [Zendesk.instance.messaging setConversationFields:g_ZendeskIOS.m_ConversationFields];
    }

    void ClearConversationFields()
    {
        [g_ZendeskIOS.m_ConversationFields removeAllObjects];
        [Zendesk.instance.messaging clearConversationFields];
    }

    void AddConversationTag(const char* tag) {
        [g_ZendeskIOS.m_ConversationTags addObject:[NSString stringWithUTF8String:tag]];
        [Zendesk.instance.messaging setConversationTags:g_ZendeskIOS.m_ConversationTags];
    }

    void ClearConversationTags() {
        [g_ZendeskIOS.m_ConversationTags removeAllObjects];
        [Zendesk.instance.messaging clearConversationTags];
    }

} // namespace dmZendesk

#endif // DM_PLATFORM_IOS