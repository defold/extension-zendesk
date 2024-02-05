#ifdef DM_PLATFORM_ANDROID

#include <dmsdk/dlib/android.h>
#include "com_defold_zendesk_ZendeskJNI.h"
#include "zendesk_private.h"
#include "zendesk_callback.h"


JNIEXPORT void JNICALL Java_com_defold_zendesk_ZendeskJNI_zendeskAddToQueue(JNIEnv * env, jclass cls, jint jmsg, jstring jjson)
{
    const char* json = env->GetStringUTFChars(jjson, 0);
    dmZendesk::AddToQueueCallback((dmZendesk::Message)jmsg, json);
    env->ReleaseStringUTFChars(jjson, json);
}

namespace dmZendesk
{

    struct ZendeskJNI {
        jobject        m_ZendeskJNI;

        jmethodID      m_Initialize;
        jmethodID      m_ShowMessaging;
        jmethodID      m_Login;
        jmethodID      m_Logout;
        jmethodID      m_AddConversationFieldString;
        jmethodID      m_AddConversationFieldFloat;
        jmethodID      m_AddConversationFieldBoolean;
        jmethodID      m_ClearConversationFields;
        jmethodID      m_AddConversationTag;
        jmethodID      m_ClearConversationTags;
    };

    static ZendeskJNI g_Zendesk;

    static void InitJNIMethods(JNIEnv* env, jclass cls)
    {
        g_Zendesk.m_Initialize = env->GetMethodID(cls, "initialize", "(Ljava/lang/String;)V");
        g_Zendesk.m_ShowMessaging = env->GetMethodID(cls, "showMessaging", "()V");
        g_Zendesk.m_Login = env->GetMethodID(cls, "login", "(Ljava/lang/String;)V");
        g_Zendesk.m_Logout = env->GetMethodID(cls, "logout", "()V");
        g_Zendesk.m_AddConversationFieldString = env->GetMethodID(cls, "addConversationFieldString", "(Ljava/lang/String;Ljava/lang/String;)V");
        g_Zendesk.m_AddConversationFieldFloat = env->GetMethodID(cls, "addConversationFieldFloat", "(Ljava/lang/String;F)V");
        g_Zendesk.m_AddConversationFieldBoolean = env->GetMethodID(cls, "addConversationFieldBoolean", "(Ljava/lang/String;Z)V");
        g_Zendesk.m_ClearConversationFields = env->GetMethodID(cls, "clearConversationFields", "()V");
        g_Zendesk.m_AddConversationTag = env->GetMethodID(cls, "addConversationTag", "(Ljava/lang/String;)V");
        g_Zendesk.m_ClearConversationTags = env->GetMethodID(cls, "clearConversationTags", "()V");
    }

    static void CallVoidMethod(jobject instance, jmethodID method)
    {
        dmAndroid::ThreadAttacher threadAttacher;
        JNIEnv* env = threadAttacher.GetEnv();

        env->CallVoidMethod(instance, method);
    }

    static void CallVoidMethodString(jobject instance, jmethodID method, const char* cstr)
    {
        dmAndroid::ThreadAttacher threadAttacher;
        JNIEnv* env = threadAttacher.GetEnv();

        jstring jstr = env->NewStringUTF(cstr);
        env->CallVoidMethod(instance, method, jstr);
        env->DeleteLocalRef(jstr);
    }

    static void CallVoidMethodStringString(jobject instance, jmethodID method, const char* cstr1, const char* cstr2)
    {
        dmAndroid::ThreadAttacher threadAttacher;
        JNIEnv* env = threadAttacher.GetEnv();

        jstring jstr1 = env->NewStringUTF(cstr1);
        jstring jstr2 = env->NewStringUTF(cstr2);
        env->CallVoidMethod(instance, method, jstr1, jstr2);
        env->DeleteLocalRef(jstr1);
        env->DeleteLocalRef(jstr2);
    }

    static void CallVoidMethodStringFloat(jobject instance, jmethodID method, const char* cstr, const float cfloat)
    {
        dmAndroid::ThreadAttacher threadAttacher;
        JNIEnv* env = threadAttacher.GetEnv();

        jstring jstr = env->NewStringUTF(cstr);
        env->CallVoidMethod(instance, method, jstr, cfloat);
        env->DeleteLocalRef(jstr);
    }

    static void CallVoidMethodStringBoolean(jobject instance, jmethodID method, const char* cstr, const bool cbool)
    {
        dmAndroid::ThreadAttacher threadAttacher;
        JNIEnv* env = threadAttacher.GetEnv();

        jstring jstr = env->NewStringUTF(cstr);
        env->CallVoidMethod(instance, method, jstr, cbool);
        env->DeleteLocalRef(jstr);
    }

    void Initialize_Ext() {
        dmAndroid::ThreadAttacher threadAttacher;
        JNIEnv* env = threadAttacher.GetEnv();

        jclass cls = dmAndroid::LoadClass(env, "com.defold.zendesk.ZendeskJNI");

        InitJNIMethods(env, cls);

        jmethodID jni_constructor = env->GetMethodID(cls, "<init>", "(Landroid/app/Activity;)V");

        g_Zendesk.m_ZendeskJNI = env->NewGlobalRef(env->NewObject(cls, jni_constructor, threadAttacher.GetActivity()->clazz));
    }

    void Finalize_Ext() {
        
    }

    void Initialize(const char* channel) {
        CallVoidMethodString(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_Initialize, channel);
    }

    void ShowMessaging() {
        CallVoidMethod(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_ShowMessaging);
    }

    void Login(const char* jwt) {
        CallVoidMethodString(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_Login, jwt);
    }

    void Logout() {
        CallVoidMethod(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_Logout);
    }

    void AddConversationFieldString(const char* key, const char* value) {
        CallVoidMethodStringString(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_AddConversationFieldString, key, value);
    }

    void AddConversationFieldNumber(const char* key, const float value) {
        CallVoidMethodStringFloat(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_AddConversationFieldString, key, value);
    }

    void AddConversationFieldBoolean(const char* key, const bool value) {
        CallVoidMethodStringBoolean(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_AddConversationFieldString, key, value);
    }

    void ClearConversationFields() {
        CallVoidMethod(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_ClearConversationFields);
    }

    void AddConversationTag(const char* tag) {
        CallVoidMethodString(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_AddConversationTag, tag);
    }

    void ClearConversationTags() {
        CallVoidMethod(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_ClearConversationTags);
    }


} // namespace
#endif // DM_PLATFORM_ANDROID
