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
#include <unistd.h>
#include <pthread.h>
#include <android/native_window.h> // requires ndk r5 or newer

#include "Scene.h"

#include "ShaderManager.h"
#include "Utils.h"
#include "Mesh.h"

namespace android3d
{

Scene* Scene::mInstance = NULL;

Scene::Scene() :
mDisplay(NULL),
mSurface(NULL),
mContext(NULL),
mWidth(0),
mHeight(0),
mWindow(NULL),
mMsg(MSG_NONE),
mThreadID(-1)
{
    LOGI("Scene is created");
    pthread_mutex_init(&mMutex, NULL);
    mCamera = new Camera();
}

Scene::~Scene()
{
    LOGI("Scene is destroyed");
    pthread_mutex_destroy(&mMutex);
    if (mCamera != NULL)
    	delete mCamera;

    for (int i = 0; i < mMeshes.size(); ++i)
    {
    	delete mMeshes[i];
    }
}

void Scene::release()
{
	if (mInstance != NULL)
		delete mInstance;

	mInstance = NULL;
}

Scene* Scene::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new Scene();
	}

	return mInstance;
}

void Scene::start()
{
    LOGI("Creating renderer thread");
    pthread_create(&mThreadID, NULL, renderThread, this);
    return;
}

void Scene::stop()
{
    LOGI("Stopping renderer thread");

    // send message to render thread to stop rendering
    pthread_mutex_lock(&mMutex);
    mMsg = MSG_RENDER_EXIT;
    pthread_mutex_unlock(&mMutex);

    pthread_join(mThreadID, 0);
    LOGI("Renderer thread stopped");

    return;
}

void Scene::setWindow(ANativeWindow *window)
{
    // notify render thread that window has changed
    pthread_mutex_lock(&mMutex);
    mMsg = MSG_WINDOW_SET;
    mWindow = window;
    pthread_mutex_unlock(&mMutex);

    return;
}

void Scene::render()
{
    bool renderingEnabled = true;
    while (renderingEnabled) {

        pthread_mutex_lock(&mMutex);

        // process incoming messages
        switch (mMsg) {

            case MSG_WINDOW_SET:
                initialize();
                break;

            case MSG_RENDER_EXIT:
                renderingEnabled = false;
                destroy();
                break;

            default:
                break;
        }
        mMsg = MSG_NONE;
        
        if (mDisplay) {
            drawFrame();
            if (!eglSwapBuffers(mDisplay, mSurface)) {
                LOGE("eglSwapBuffers() returned error %d", eglGetError());
            }
        }
        
        pthread_mutex_unlock(&mMutex);
    }
    
    LOGI("Render loop exits");
    
    return;
}

bool Scene::initialize()
{
    const EGLint configAttribs[] = {
    	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };

    const EGLint contextAttrs[] = {
         EGL_CONTEXT_CLIENT_VERSION, 2,
         EGL_NONE
    };

    EGLConfig config;    
    EGLint numConfigs;
    EGLint format;
    EGLint major, minor;
    
    LOGI("Initializing context");
    
    if ((mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay() returned error %d", eglGetError());
        return false;
    }
    if (!eglInitialize(mDisplay, &major, &minor)) {
        LOGE("eglInitialize() returned error %d", eglGetError());
        return false;
    }
    LOGI("EGL Version: %d.%d", major, minor);

    if (!eglChooseConfig(mDisplay, configAttribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
        destroy();
        return false;
    }

    ANativeWindow_setBuffersGeometry(mWindow, 0, 0, format);

    if (!(mSurface = eglCreateWindowSurface(mDisplay, config, mWindow, NULL))) {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        destroy();
        return false;
    }
    
    if (!(mContext = eglCreateContext(mDisplay, config, NULL, contextAttrs))) {
        LOGE("eglCreateContext() returned error %d", eglGetError());
        destroy();
        return false;
    }
    
    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        LOGE("eglMakeCurrent() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mWidth) ||
        !eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mHeight)) {
        LOGE("eglQuerySurface() returned error %d", eglGetError());
        destroy();
        return false;
    }

    printGLString("Vendor: ", GL_VENDOR);
    printGLString("Version: ", GL_VERSION);
    printGLString("Renderer: ", GL_RENDERER);
    printGLString("Shading Language Version: ", GL_SHADING_LANGUAGE_VERSION);
    printGLString("Extensions: ", GL_EXTENSIONS);

    glViewport(0, 0, mWidth, mHeight);

    mCamera->updateMVP(mWidth, mHeight);

    return true;
}

void Scene::destroy() {
    LOGI("Destroying context");

    eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(mDisplay, mContext);
    eglDestroySurface(mDisplay, mSurface);
    eglTerminate(mDisplay);
    
    mDisplay = EGL_NO_DISPLAY;
    mSurface = EGL_NO_SURFACE;
    mContext = EGL_NO_CONTEXT;
    mWidth = 0;
    mHeight = 0;

    return;
}

void Scene::drawFrame()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clock_t time = clock() / (CLOCKS_PER_SEC / 1000);
    time = time % 4000L;
    float angle = 0.060f * ((int) time);
    for (int i = 0 ; i < mMeshes.size(); ++i)
    {
    	mMeshes[i]->render();
    }

    /*glUseProgram(mShaderProgram);
    //checkGlError("glUseProgram");
    glVertexAttribPointer(positionHandle, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    //checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(positionHandle);
    //checkGlError("glEnableVertexAttribArray");

    // Create a rotation for the triangle
    clock_t time = clock() / (CLOCKS_PER_SEC / 1000);
    time = time % 4000L;
    float angle = 0.060f * ((int) time);
    mModelMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(mMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mCamera->getMVP() * mModelMatrix));
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);*/
}

void* Scene::renderThread(void *myself)
{
	Scene* scene = (Scene*)myself;

	scene->render();
    pthread_exit(0);
    
    return 0;
}

void Scene::addMesh(Mesh* mesh)
{
	mMeshes.push_back(mesh);
}

} //end namespace

