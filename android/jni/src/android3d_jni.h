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

extern "C" {
    JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnCreate(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnResume(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnPause(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnStop(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface);
};

#endif // ANDROID3D_JNI_H
