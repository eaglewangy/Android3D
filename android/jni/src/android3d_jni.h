/*
 * Copyright 2012
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  Created on: Nov 14, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

#ifndef ANDROID3D_JNI_H
#define ANDROID3D_JNI_H

#include "Utils.h"

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

extern "C" {
static JavaVM* gJVM = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved);
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved);

JNIEXPORT void JNICALL Java_com_peony_android3d_Android3DLib_init(JNIEnv* jenv, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_peony_android3d_Android3DLib_step(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_peony_android3d_Android3DLib_destroy(JNIEnv* jenv, jobject obj);
};

#ifdef __cplusplus
}
#endif //

jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
	JNIEnv *env;
	jclass cls;
	gJVM = jvm; /* cache the JavaVM pointer */
	if (jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK){
		LOGE("Android3D failed to initialize.");
		return JNI_ERR; /* JNI version not supported */
	}

	LOGI("Android3D has been initialized.");
	return JNI_VERSION_1_4;
}

void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved)
{
}

#endif // ANDROID3D_JNI_H
