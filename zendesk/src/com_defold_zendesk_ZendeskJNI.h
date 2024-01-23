#if defined(DM_PLATFORM_ANDROID)

#include <jni.h>
/* Header for class com_defold_zendesk_ZendeskJNI */

#ifndef COM_DEFOLD_ZENDESK_ZENDESKJNI_H
#define COM_DEFOLD_ZENDESK_ZENDESKJNI_H
#ifdef __cplusplus
extern "C" {
#endif
    /*
    * Class:     com_defold_zendesk_ZendeskJNI
    * Method:    zendeskAddToQueue_first_arg
    * Signature: (ILjava/lang/String;I)V
    */
    JNIEXPORT void JNICALL Java_com_defold_zendesk_ZendeskJNI_zendeskAddToQueue
        (JNIEnv *, jclass, jint, jstring);

#ifdef __cplusplus
}
#endif
#endif

#endif