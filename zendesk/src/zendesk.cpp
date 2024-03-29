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

static int Lua_SetConversationFields(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    luaL_checktype(L, 1, LUA_TTABLE);

    lua_pushvalue(L, 1);                // stack now contains: -1 => table
    lua_pushnil(L);                     // stack now contains: -1 => nil; -2 => table

    while (lua_next(L, -2))             // stack now contains: -1 => value; -2 => key; -3 => table
    {
        lua_pushvalue(L, -2);           // copy the key so that lua_tostring does not modify the original
                                        // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table

        const char *key = lua_tostring(L, -1);
        int value_type = lua_type(L, -2);
        if (value_type == LUA_TNUMBER)
        {
            float n = (float)lua_tonumber(L, -2); 
            AddConversationFieldNumber(key, n);
        }
        else if (value_type == LUA_TBOOLEAN)
        {
            int b = lua_toboolean(L, -2);
            AddConversationFieldBoolean(key, b);
        }
        else if (value_type == LUA_TSTRING)
        {
            const char *s = lua_tostring(L, -2);
            AddConversationFieldString(key, s);
        }
        else
        {
            dmLogInfo("Conversation field values must be of type string, number or boolean");
        }

        lua_pop(L, 2);                  // pop value + copy of key, leaving original key
                                        // stack now contains: -1 => key; -2 => table
    }

    lua_pop(L, 1);                      // stack is now empty
    return 0;
}

static int Lua_ClearConversationFields(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    ClearConversationFields();
    return 0;
}

static int Lua_SetConversationTags(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    luaL_checktype(L, 1, LUA_TTABLE);

    size_t len = lua_objlen(L, 1);
    for (int index = 1; index <= len; index++) {
        lua_pushinteger(L, index);
        lua_gettable(L, 1); 
        const char* tag = luaL_checkstring(L, -1);
        lua_pop(L, 1);
        AddConversationTag(tag);
    }
    return 0;
}

static int Lua_ClearConversationTags(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    ClearConversationTags();
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

static int Lua_Login(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    const char* jwt = luaL_checkstring(L, 1);
    Login(jwt);
    return 0;
}

static int Lua_Logout(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    Logout();
    return 0;
}

static const luaL_reg Zendesk_methods[] =
{   
    {"initialize", Lua_Initialize},
    {"set_callback", Lua_SetCallback},
    {"set_conversation_fields", Lua_SetConversationFields},
    {"clear_conversation_fields", Lua_ClearConversationFields},
    {"set_conversation_tags", Lua_SetConversationTags},
    {"clear_conversation_tags", Lua_ClearConversationTags},
    {"show_messaging", Lua_ShowMessaging},
    {"login", Lua_Login},
    {"logout", Lua_Logout},
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
    SETCONSTANT(MSG_LOGIN_SUCCESS)
    SETCONSTANT(MSG_LOGIN_FAILED)
    SETCONSTANT(MSG_LOGOUT_SUCCESS)
    SETCONSTANT(MSG_LOGOUT_FAILED)

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