#if defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS)

#pragma once

namespace dmZendesk {

void Initialize_Ext();

void Initialize(const char* channel);
void ShowMessaging();

} //namespace dmZendesk

#endif