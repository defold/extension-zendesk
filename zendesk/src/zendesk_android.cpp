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
    };

    static ZendeskJNI g_Zendesk;

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

    static void InitJNIMethods(JNIEnv* env, jclass cls)
    {
        g_Zendesk.m_Initialize = env->GetMethodID(cls, "initialize", "(Ljava/lang/String;)V");
        g_Zendesk.m_ShowMessaging = env->GetMethodID(cls, "showMessaging", "()V");
        // g_Zendesk.m_GetInstallationAuthToken = env->GetMethodID(cls, "getInstallationAuthToken", "()V");
        // g_Zendesk.m_GetInstallationId = env->GetMethodID(cls, "getInstallationId", "()V");
        // g_Zendesk.m_SetOption = env->GetMethodID(cls, "setOption", "(Ljava/lang/String;Ljava/lang/String;)Z");
    }

    void Initialize_Ext() {
        dmAndroid::ThreadAttacher threadAttacher;
        JNIEnv* env = threadAttacher.GetEnv();

        jclass cls = dmAndroid::LoadClass(env, "com.defold.zendesk.ZendeskJNI");

        InitJNIMethods(env, cls);

        jmethodID jni_constructor = env->GetMethodID(cls, "<init>", "(Landroid/app/Activity;)V");

        g_Zendesk.m_ZendeskJNI = env->NewGlobalRef(env->NewObject(cls, jni_constructor, threadAttacher.GetActivity()->clazz));
    }


    void Initialize(const char* channel) {
        CallVoidMethodString(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_Initialize, channel);
    }

    void ShowMessaging() {
        CallVoidMethod(g_Zendesk.m_ZendeskJNI, g_Zendesk.m_ShowMessaging);
    }


} // namespace
#endif // DM_PLATFORM_ANDROID
