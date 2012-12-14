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
#include "Image.h"

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
#include <string>

namespace android3d
{
struct ImageArg
{
	Image* image;
	int x;
	int y;
	DrawAnchor anchor;
};

class Mesh;

class Scene {

public:
    virtual ~Scene();
    static void release();

    static Scene* getInstance();
    bool prepareScene(int width, int height);
    void drawFrame();
    static std::string ROOT_PATH;

    Camera* getCamera() {return mCamera;}

    void addMesh(Mesh* mesh);
    void addImage(Image* image, int x, int y, DrawAnchor anchor = CENTER);
    int getWidth() {return mWidth;}
    int getHeight() {return mHeight;}

private:
    Scene();
    static Scene* mInstance;

    int mWidth;
    int mHeight;
    
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
    std::vector<ImageArg> mImages;
};

} //end namespace

#endif // SCENE_H
