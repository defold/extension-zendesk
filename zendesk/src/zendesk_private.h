#if defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS)

#pragma once

namespace dmZendesk {

void Initialize_Ext();
void Finalize_Ext();

void Initialize(const char* channel);
void ShowMessaging();
void Login(const char* jwt);
void Logout();
void AddConversationFieldString(const char* key, const char* value);
void AddConversationFieldNumber(const char* key, const float value);
void AddConversationFieldBoolean(const char* key, const bool value);
void ClearConversationFields();
void AddConversationTag(const char* tag);
void ClearConversationTags();

} //namespace dmZendesk

#endif