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
std::string Scene::ROOT_PATH = "/data/data/com.peony.android3d/files/";

Scene::Scene() :
mWidth(0),
mHeight(0)
{
	Utils::printGLString("Vendor: ", GL_VENDOR);
	Utils::printGLString("Version: ", GL_VERSION);
	Utils::printGLString("Renderer: ", GL_RENDERER);
	Utils::printGLString("Shading Language Version: ", GL_SHADING_LANGUAGE_VERSION);
	Utils::printGLString("Extensions: ", GL_EXTENSIONS);

	mCamera = new Camera();
}

Scene::~Scene()
{
    LOGI("Scene is destroyed");
    if (mCamera != NULL)
    {
    	delete mCamera;
    	mCamera = NULL;
    }

    for (int i = 0; i < mMeshes.size(); ++i)
    {
    	delete mMeshes[i];
    	mMeshes[i] = NULL;
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

bool Scene::prepareScene(int width, int height)
{
	mWidth = width;
	mHeight = height;

    glViewport(0, 0, mWidth, mHeight);

    mCamera->updateMVP(mWidth, mHeight);

    return true;
}

void Scene::drawFrame()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

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

void Scene::addMesh(Mesh* mesh)
{
	mMeshes.push_back(mesh);
}

} //end namespace

