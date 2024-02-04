#if defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS)
#pragma once

#include <dmsdk/sdk.h>

namespace dmZendesk {

enum Message
{
    MSG_INIT_ERROR =                   0,
    MSG_INIT_SUCCESS =                 1,
    MSG_INTERNAL_ERROR =               2,
    MSG_ERROR =                        3,
    MSG_UNREAD_MESSAGE_COUNT_CHANGED = 4,
    MSG_AUTHENTICATION_SUCCESS =       5,
    MSG_AUTHENTICATION_FAILED =        6,
    MSG_FIELD_VALIDATION_FAILED =      7,
    MSG_LOGOUT_SUCCESS =               8,
    MSG_LOGOUT_FAILED =                9,
};

struct CallbackData
{
    Message msg;
    char* json;
};

void SetLuaCallback(lua_State* L, int pos);
void UpdateCallback();
void InitializeCallback();
void FinalizeCallback();

void AddToQueueCallback(Message type, const char* json);

} //namespace dmZendesk

#endif // defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS)