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

#include <stdint.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <android/asset_manager_jni.h>

#include "android3d_jni.h"
#include "Scene.h"

static ANativeWindow* gWindow = NULL;
static android3d::Scene* gScene = NULL;

JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnCreate(JNIEnv* jenv, jobject obj)
{
	gScene = android3d::Scene::getInstance();
    return;
}

JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnResume(JNIEnv* jenv, jobject obj)
{
	gScene->start();
    return;
}

JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnPause(JNIEnv* jenv, jobject obj)
{
	gScene->stop();
    return;
}

JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeOnStop(JNIEnv* jenv, jobject obj)
{
    android3d::Scene::release();
    return;
}

JNIEXPORT void JNICALL Java_com_peony_android3d_Android3D_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface)
{
    if (surface != NULL) {
    	gWindow = ANativeWindow_fromSurface(jenv, surface);
        LOGI("Got window %p", gWindow);
        gScene->setWindow(gWindow);
    } else {
        LOGI("Releasing window");
        ANativeWindow_release(gWindow);
    }

    return;
}

void JNICALL assetManager(JNIEnv* env, jobject aAssetManager)
{
	AAssetManager* mgr = AAssetManager_fromJava(env, aAssetManager);
	if (mgr == NULL){
		LOGE("AAssetManager is null.");
		return;
	}
	AAssetDir* rootDir = AAssetManager_openDir(mgr, "");
	if (rootDir == NULL){
		LOGE("AAssetDir root is null.");
		return;
	}
	AAsset* aasset = AAssetManager_open(mgr, "test.txt", AASSET_MODE_UNKNOWN);
	if (aasset == NULL){
		LOGE("AAssetManager_open failed.");
		return;
	}

	const GLuint* content = (GLuint* const)AAsset_getBuffer(aasset);
	LOGE("file content: %s", content);
}

