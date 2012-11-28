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

#ifndef SCENE_H
#define SCENE_H

#include "Camera.h"

#include <glm/glm.hpp> //vec3, vec4, ivec4, mat4
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective
#include <glm/gtc/type_ptr.hpp> //value_ptr

#include <zlib.h>
//#include <png.h>

#include <pthread.h>
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <vector>

namespace android3d
{

class Mesh;

class Scene {

public:
    virtual ~Scene();
    static void release();

    static Scene* getInstance();

    /*
     * Following methods can be called from any thread.
     * They send message to render thread which executes required actions.
    */
    void start();
    void stop();
    void setWindow(ANativeWindow* window);
    
    Camera* getCamera() {return mCamera;}

    void addMesh(Mesh* mesh);

private:
    Scene();
    static Scene* mInstance;

    enum RenderMessage {
        MSG_NONE = 0,
        MSG_WINDOW_SET,
        MSG_RENDER_EXIT
    };

    pthread_t mThreadID;
    pthread_mutex_t mMutex;
    enum RenderMessage mMsg;
    
    // android window, supported by NDK r5 and newer
    ANativeWindow* mWindow;

    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    int mWidth;
    int mHeight;
    
    // RenderLoop is called in a rendering thread started in start() method
    // It creates rendering context and renders scene until stop() is called
    void render();
    
    bool initialize();
    void destroy();

    void drawFrame();

    // Helper method for starting the thread 
    static void* renderThread(void *myself);

    /*
     * Scene model, view, project matrix
     *
    */
    glm::mat4 mModelMatrix;
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mMVPMatrix;

    Camera* mCamera;

    std::vector<Mesh*> mMeshes;

};

} //end namespace

#endif // SCENE_H
