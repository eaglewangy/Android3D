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

GLuint positionHandle;

static const char gVertexShader[] =
		// A constant representing the combined model/view/projection matrix.
		"uniform mat4 u_MVPMatrix;\n"

		"attribute vec4 vPosition;\n"
		"void main() {\n"
		"  gl_Position = u_MVPMatrix * vPosition;\n"
		//"  gl_Position = vPosition;\n"
		"}\n";

static const char gFragmentShader[] =
		"precision mediump float;\n"
		"void main() {\n"
		"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
		"}\n";

GLfloat vertices[] =
{
		//front
		//0->1->2->3
		-1.0f, -1.0f,  1.0f, //0
		1.0f, -1.0f,  1.0f,  //1
		1.0f,  1.0f,  1.0f,  //2
		-1.0f,  1.0f,  1.0f, //3
		//back
		//4->5->6->7
		-1.0f,  -1.0f,  -1.0f, //4
		1.0f,  -1.0f,  -1.0f,  //5
		1.0f,  1.0f, -1.0f,		//6
		-1.0f,  1.0f, -1.0f,    //7
		//left
		//0->4->7->3
		-1.0f, -1.0f, 1.0f,    //8
		-1.0f, -1.0f, -1.0f,   //9
		-1.0f, 1.0f, -1.0f,    //10
		-1.0f, 1.0f, 1.0f,     //11
		//right
		//1->5->6->2
		1.0f, -1.0f, 1.0f,     //12
		1.0f, -1.0f, -1.0f,    //13
		1.0f, 1.0f, -1.0f,     //14
		1.0f, 1.0f, 1.0f,      //15
		//top
		//3->2->6->7
		-1.0f, 1.0f, 1.0f,    //16
		1.0f, 1.0f, 1.0f,     //17
		1.0f, 1.0f, -1.0f,    //18
		-1.0f, 1.0f, -1.0f,   //19
		//bottom
		//0->1->5->4
		-1.0f, -1.0f, 1.0f,   //20
		1.0f, -1.0f, 1.0f,    //21
		1.0f, -1.0f, -1.0f,   //22
		-1.0f, -1.0f, -1.0f   //23
};

GLushort indices[] =
{
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		8, 11, 10,
		8, 10, 9,

		12, 14, 15,
		12, 13, 14,

		16, 17, 18,
		16, 18, 19,

		20, 22, 21,
		20, 23, 22
};

GLfloat gTriangleVertices[] = { 0.5f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f };
GLfloat texture[] = {
		0.0f, 0.0f, // TexCoord 0,
		0.0f, 1.0f, // TexCoord 1
		1.0f, 1.0f, // TexCoord 2
		1.0f, 0.0f // TexCoord 3
		};

Scene::Scene() :
mDisplay(NULL),
mSurface(NULL),
mContext(NULL)
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
    
    LOGI("render...");

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
    LOGI("OpenGL ES Version: major:%d, minor: %d", major, minor);

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

    /*mShaderProgram = android3d::ShaderManager::createProgram(gVertexShader, gFragmentShader);
    positionHandle = glGetAttribLocation(mShaderProgram, "vPosition");
    mMVPMatrixLocation = glGetUniformLocation(mShaderProgram, "u_MVPMatrix");*/

    glViewport(0, 0, mWidth, mHeight);

    mCamera->updateMVP(mWidth, mHeight);

    Mesh* mesh1 = new Mesh();
    mesh1->setVertices(vertices, sizeof(vertices));
    mesh1->setIndices(indices, sizeof(indices));
    mesh1->setUvs(texture, sizeof(texture));
    mesh1->setPosition(-3.0f, 5.0f, 0.0f);
    mesh1->setTriangleNums(12);
    addMesh(mesh1);

    Mesh* mesh2 = new Mesh();
    mesh2->setVertices(gTriangleVertices, sizeof(gTriangleVertices));
    mesh2->setScale(3.0f, 3.0f, 3.0f);
    mesh2->setUvs(texture, sizeof(texture));
    mesh2->setTriangleNums(1);
    addMesh(mesh2);

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
	//LOGE("Renderer::drawFrame()");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //LOGE("sizeof: %d", sizeof(vertices));

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


    static int count;
    	++count;
    	if (count == 1)
    		printMatrix(const_cast<float*>(glm::value_ptr(this->mCamera->getMVP())), 1) ;
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

