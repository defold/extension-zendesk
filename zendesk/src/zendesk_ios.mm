#if defined(DM_PLATFORM_IOS)

#include "zendesk_private.h"
#include "zendesk_callback.h"

#import <ZendeskSDKMessaging/ZendeskSDKMessaging.h>
#import <ZendeskSDK/ZendeskSDK.h>

namespace dmZendesk {
    void SendSimpleMessage(Message msg, id obj) {
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

    void SendErrorMessage(NSError *error) {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        [dict setObject:error.localizedDescription forKey:@"error"];
        SendSimpleMessage(MSG_ERROR, dict);
    }

    void SendSimpleMessage(Message msg) {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        SendSimpleMessage(msg, dict);
    }

    void SendSimpleMessage(Message msg, NSString *key, NSString *value) {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        [dict setObject:value forKey:key];
        SendSimpleMessage(msg, dict);
    }

    void Initialize_Ext() {

    }


    void Initialize(const char* channel) {
        [Zendesk initializeWithChannelKey:@"<channel_key>"
                         messagingFactory:[[ZDKDefaultMessagingFactory alloc] init]
                      completionHandler:^(Zendesk * _Nullable zendesk, NSError * _Nullable error) {
            if (error != nil) {
                NSLog(@"Zendesk did not initialize.\nError: %@", error.localizedDescription);
                SendSimpleMessage(MSG_INIT_ERROR, "error", error.localizedDescription);
            }
            else {
                SendSimpleMessage(MSG_INIT_SUCCESS);
            }
        }];
    }

    void ShowMessaging() {
        
    }


} // namespace dmZendesk

#endif // DM_PLATFORM_IOS