#define EXTENSION_NAME zendesk
#define LIB_NAME "Zendesk"
#define MODULE_NAME "zendesk"

#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/dlib/log.h>
#include <dmsdk/sdk.h>

#include "zendesk_private.h"
#include "zendesk_callback.h"

#if defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS)

namespace dmZendesk {

static int Lua_Initialize(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    const char* channel = luaL_checkstring(L, 1);
    Initialize(channel);
    return 0;
}

static int Lua_SetCallback(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    SetLuaCallback(L, 1);
    return 0;
}

static int Lua_ShowMessaging(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    ShowMessaging();
    return 0;
}

static const luaL_reg Zendesk_methods[] =
{   
    {"initialize", Lua_Initialize},
    {"set_callback", Lua_SetCallback},
    {"show_messaging", Lua_ShowMessaging},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    luaL_register(L, MODULE_NAME, Zendesk_methods);

    #define SETCONSTANT(name) \
    lua_pushnumber(L, (lua_Number) name); \
    lua_setfield(L, -2, #name); \

    SETCONSTANT(MSG_INIT_ERROR)
    SETCONSTANT(MSG_INIT_SUCCESS)
    SETCONSTANT(MSG_INTERNAL_ERROR)
    SETCONSTANT(MSG_ERROR)
    SETCONSTANT(MSG_UNREAD_MESSAGE_COUNT_CHANGED)
    SETCONSTANT(MSG_AUTHENTICATION_FAILED)
    SETCONSTANT(MSG_FIELD_VALIDATION_FAILED)


    #undef SETCONSTANT
    
    lua_pop(L, 1);
}

dmExtension::Result AppInitializeZendeskExtension(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeZendeskExtension(dmExtension::Params* params) {
    dmLogInfo("Initialize Zendesk");

    LuaInit(params->m_L);
    Initialize_Ext();
    InitializeCallback();

    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeZendeskxtension(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeZendeskExtension(dmExtension::Params* params) {
    FinalizeCallback();
    return dmExtension::RESULT_OK;
}

dmExtension::Result UpdateZendeskExtension(dmExtension::Params* params) {
    UpdateCallback();
    return dmExtension::RESULT_OK;
}

} // namespace dmZendesk

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, dmZendesk::AppInitializeZendeskExtension, dmZendesk::AppFinalizeZendeskxtension, dmZendesk::InitializeZendeskExtension, dmZendesk::UpdateZendeskExtension, 0, dmZendesk::FinalizeZendeskExtension)

#else // defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS) 

//-------------------- Not supported for other platforms --------------------//

static dmExtension::Result InitializeZendesk(dmExtension::Params* params)
{
    dmLogInfo("Registered extension Zendesk (null)");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeZendesk(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializeZendesk, 0, 0, FinalizeZendesk)

#endif // defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS) 