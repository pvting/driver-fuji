#include <jni.h>
#include <string>
#include "FujiProtocol.h"

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getRpt(JNIEnv *env, jobject instance, jint rpt_type) {

    jbyte gR[200];
    jint len=fp.getRpt(rpt_type,(unsigned char *)gR);
    jbyteArray ret= (*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret, 0, len, gR);
    return ret;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_setSalesMaxNum(JNIEnv *env, jobject instance,
                                                            jint m_type) {

    fp.setSalesMaxNum(m_type);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getSalesMaxNum(JNIEnv *env, jobject instance) {

    return fp.getSalesMaxNum();
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getVmcColumnPrice_1notCash(JNIEnv *env,
                                                                        jobject instance) {
    jbyte gVCNC[150];
    jint len=fp.getVmcColumnPrice_notCash((unsigned char *)gVCNC);
    jbyteArray ret= (*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret, 0, len, gVCNC);
    return ret;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_setColumnPrice_1notCash(JNIEnv *env, jobject instance,
                                                                     jbyteArray Column_Price_notCash_) {
    jbyte *Column_Price_notCash = env->GetByteArrayElements(Column_Price_notCash_, NULL);
    fp.setColumnPrice_notCash((unsigned char *)Column_Price_notCash);
    env->ReleaseByteArrayElements(Column_Price_notCash_, Column_Price_notCash, 0);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getVmcColumnPrice_1Cash(JNIEnv *env,
                                                                     jobject instance) {
    jbyte gVCC[150];
    jint len=fp.getVmcColumnPrice_Cash((unsigned char *)gVCC);
    jbyteArray ret= (*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret, 0, len, gVCC);
    return ret;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_setColumnPrice_1Cash(JNIEnv *env, jobject instance,
                                                                  jbyteArray Column_Price_Cash_) {
    jbyte *Column_Price_Cash = env->GetByteArrayElements(Column_Price_Cash_, NULL);
    fp.setColumnPrice_Cash((unsigned char *)Column_Price_Cash);
    env->ReleaseByteArrayElements(Column_Price_Cash_, Column_Price_Cash, 0);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getMachineId(JNIEnv *env, jobject instance) {

    jbyteArray ret= (*env).NewByteArray(8);
    jbyte gMI[8];
    fp.getMachineId((unsigned char *)gMI);
    (*env).SetByteArrayRegion(ret, 0, 8, gMI);
    return ret;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_setMachineId(JNIEnv *env, jobject instance,
                                                          jbyteArray machine_id_) {
    jbyte *machine_id = env->GetByteArrayElements(machine_id_, NULL);

    fp.setMachineId((unsigned char *)machine_id);

    env->ReleaseByteArrayElements(machine_id_, machine_id, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getInfo(JNIEnv *env, jobject instance, jint m_type) {

    fp.getInfo(m_type);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getVmcClmsalestatusRpt(JNIEnv *env, jobject instance) {

    jbyte gVCR[80];
    jint len=fp.getVmcClmsalestatusRpt((unsigned char *)gVCR);
    jbyteArray ret= (*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret, 0, len, gVCR);
    return ret;

}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getVmcSoldoutRpt(JNIEnv *env, jobject instance) {

    jbyte gVSOR[80];
    jint len=fp.getVmcSoldoutRpt((unsigned char *)gVSOR);
    jbyteArray ret= (*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret, 0, len, gVSOR);
    return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getVmcErrorRpt(JNIEnv *env, jobject instance) {

    jbyte gVER[40];
    jint len=fp.getVmcErrorRpt((unsigned char *)gVER);
    jbyteArray ret= (*env).NewByteArray(len);
    (*env).SetByteArrayRegion(ret, 0, len, gVER);
    return ret;

}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getVmcStatusRpt(JNIEnv *env, jobject instance) {

    jbyteArray ret= (*env).NewByteArray(33);
    jbyte gVSR[33];
    fp.getVmcStatusRpt((unsigned char *)gVSR);
    (*env).SetByteArrayRegion(ret, 0, 33, gVSR);
    return ret;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getStatus(JNIEnv *env, jobject instance, jint m_type) {

    fp.getStatus(m_type);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getVendoutRpt(JNIEnv *env, jobject instance) {

    jbyteArray ret= (*env).NewByteArray(49);
    jbyte gVR[49];
    fp.getVendoutRpt((unsigned char *)gVR);
    (*env).SetByteArrayRegion(ret, 0, 49, gVR);
    return ret;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_vendoutIndAction(JNIEnv *env, jobject instance,
                                                              jint payment, jint columnID) {

    fp.vendoutIndAction(payment,columnID);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_vendoutIndRequest(JNIEnv *env, jobject instance,
                                                               jint payment, jint columnID) {

    fp.vendoutIndRequest(payment,columnID);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_returnCoin(JNIEnv *env, jobject instance) {

    fp.returnCoin();
}

extern "C"
JNIEXPORT jshort JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getColumnNum(JNIEnv *env, jobject instance) {

    return fp.getColumnNum();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_lightButton(JNIEnv *env, jobject instance,
                                                         jint columnNo) {

    fp.lightButton(columnNo);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_charge(JNIEnv *env, jobject instance,
                                                    jint charge_cost) {

    fp.charge(charge_cost);
}

extern "C"
JNIEXPORT jshort JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getInputCash(JNIEnv *env, jobject instance) {

    return fp.getInputCash();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getSelectedColumnId(JNIEnv *env, jobject instance) {


    return fp.getSelectedColumnId();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_startProtocol(JNIEnv *env, jobject instance) {

    fp.startProtocol();

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_want_vmc_1impl_1fuji_FujiImpl_getEvent(JNIEnv *env, jobject instance) {

    return fp.getEvent();
}




