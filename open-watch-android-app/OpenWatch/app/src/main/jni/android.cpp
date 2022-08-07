#include <jni.h>
#include <iostream>

#define GET(a) ((uint8_t) env->CallByteMethod(env->GetObjectArrayElement(bytes, (a) + 10), byteToInt))

extern "C"
JNIEXPORT jobject JNICALL
Java_com_openwatch_packet_PPGAlgorithms_test(JNIEnv *env, jclass clazz, jobjectArray bytes, jint len) {

    int lengthOfArray = len / 4;

    int array_red[lengthOfArray];
    int array_ir[lengthOfArray];

    jclass byteCls = env->FindClass("java/lang/Byte");
    jmethodID byteToInt = env->GetMethodID(byteCls, "byteValue", "()B");

    for (int i = 0; i < lengthOfArray; ++i) {
        array_red[i] = GET(i * 4 + 0) + GET(i * 4 + 1) * 256;
        array_ir[i] = GET(i * 4 + 2) + GET(i * 4 + 3) * 256;
    }

    jintArray result_red = env->NewIntArray(lengthOfArray);
    jintArray result_ir = env->NewIntArray(lengthOfArray);
    env->SetIntArrayRegion(result_red, 0, lengthOfArray, array_red);
    env->SetIntArrayRegion(result_ir, 0, lengthOfArray, array_ir);

    jclass javaLocalClass = env->FindClass("com/openwatch/packet/PPGData");
    auto javaGlobalClass = reinterpret_cast<jclass>(env->NewGlobalRef(javaLocalClass));
    jmethodID javaConstructor = env->GetMethodID(javaGlobalClass, "<init>", "([I[I)V");
    jobject obj = env->NewObject(javaGlobalClass, javaConstructor, result_red, result_ir);
    return obj;
}