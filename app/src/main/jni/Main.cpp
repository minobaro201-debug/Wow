#include "Includes.h"
#include "CODM.h"

bool titleValid = false;

extern "C" {
JNIEXPORT jstring
JNICALL
Java_com_gotoubun_Floating_Title(JNIEnv *env, jobject thiz) {
    titleValid = true;
    return env->NewStringUTF("drcoconutyt");
}
}

// Returns current value of a config key, or 0 if not set
static u_long GetConfig(const char *key) {
    auto it = Config.find(key);
    if (it != Config.end()) return it->second;
    return 0;
}

void native_onSendConfig(JNIEnv *env, jobject thiz, jstring s, jstring v) {
    const char *config = env->GetStringUTFChars(s, 0);
    const char *value  = env->GetStringUTFChars(v, 0);

    Config[config] = (u_long) strtoul(value, 0, 0);

    // Performance feature hooks
    if (strcmp(config, "PERF::LOW_GRAPHICS") == 0) {
        bool enabled = Config[config] == 1;
        // Low graphics mode: read by onCanvasDraw to skip heavy rendering
        (void) enabled;
    }
    if (strcmp(config, "PERF::STRETCH") == 0) {
        u_long stretch = Config[config]; // 0-100 range from seekbar
        (void) stretch;
    }

    env->ReleaseStringUTFChars(s, config);
    env->ReleaseStringUTFChars(v, value);
}

int Register1(JNIEnv *env) {
    JNINativeMethod methods[] = {
        {"onSendConfig", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) native_onSendConfig},
        {"onCanvasDraw", "(Landroid/graphics/Canvas;IIF)V",         (void *) native_onCanvasDraw}
    };

    jclass clazz = env->FindClass("com/gotoubun/Floating");
    if (!clazz) return -1;

    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return -1;

    return 0;
}

int Register2(JNIEnv *env) {
    JNINativeMethod methods[] = {
        {"Init", "(Landroid/content/Context;)V", (void *) native_Init}
    };

    jclass clazz = env->FindClass("com/gotoubun/Launcher");
    if (!clazz) return -1;

    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return -1;

    return 0;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (Register1(env) != 0) return -1;
    if (Register2(env) != 0) return -1;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved) {
}
