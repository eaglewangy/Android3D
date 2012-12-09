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
#include "Mesh.h"

/*--------------------------------------Begin test data---------------------------------------*/
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

GLfloat gCubeNoraml[] = {
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,

		0, 0, -1,
		0, 0, -1,
		0, 0, -1,
		0, 0, -1,

		-1, 0, 0,
		-1, 0, 0,
		-1, 0, 0,
		-1, 0, 0,

		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,

		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,

		0, -1, 0,
		0, -1, 0,
		0, -1, 0,
		0, -1, 0,
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

GLfloat gTriangleVertices[] = {
		0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		-0.5f, 0.0f, 0.0f,
};
GLfloat gTriangleColors[] = {
		1.0f, 0.0f, 0.0f , 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
};
GLfloat gTriangleNormal[] = {
		0, 0, 1,
		0, 0, 1,
		0, 0, 1
};

GLfloat texture[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
};

GLfloat gSquareVertex[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
};

GLushort gSquareIndex[] = {
		0, 1, 2,
    0, 2, 3
};
GLfloat gNormal[] = {
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1
};
/*--------------------------------------End test data---------------------------------------*/

static android3d::Scene* gScene = NULL;
void JNICALL Java_com_peony_android3d_Android3DLib_init(JNIEnv* jenv, jobject obj, jint width, jint height)
{
	gScene->release();
	gScene = android3d::Scene::getInstance();
	gScene->prepareScene(width, height);

	android3d::Mesh* cube = new android3d::Mesh();
	cube = new android3d::Mesh();
	cube->setVertices(vertices, sizeof(vertices));
	cube->setIndices(indices, sizeof(indices));
	cube->setNormals(gCubeNoraml, sizeof(gCubeNoraml));
	//mesh1->setUvs(texture, sizeof(texture));
	cube->setRotate(45.0f, 45.0f, 0.0f, true);
	cube->setTriangleNums(12);
	gScene->addMesh(cube);

	android3d::Mesh* triangle = new android3d::Mesh();
	triangle->setVertices(gTriangleVertices, sizeof(gTriangleVertices));
	triangle->setScale(3.0f, 3.0f, 3.0f);
	triangle->setPosition(-3.0f, 5.0f, 0.0f);
	//triangle->setUvs(texture, sizeof(texture));
	triangle->setNormals(gTriangleNormal, sizeof(gTriangleNormal));
	triangle->setColors(gTriangleColors, sizeof(gTriangleColors));
	triangle->setTriangleNums(1);
	gScene->addMesh(triangle);

	android3d::Mesh* square1 = new android3d::Mesh();
	square1->setVertices(gSquareVertex, sizeof(gSquareVertex));
	square1->setIndices(gSquareIndex, sizeof(gSquareIndex));
	square1->setScale(3.0f, 3.0f, 3.0f);
	square1->setPosition(-3.0f, -5.0f, 0.0f);
	square1->setUvs(texture, sizeof(texture));
	std::string texturePath = android3d::Scene::ROOT_PATH + "logo_jpeg.jpg";
	square1->setImage(texturePath);
	//mesh2->setColors(gTriangleColors, sizeof(gTriangleColors));
	square1->setTriangleNums(2);
	gScene->addMesh(square1);

	android3d::Mesh* square2 = new android3d::Mesh();
	square2->setVertices(gSquareVertex, sizeof(gSquareVertex));
	square2->setIndices(gSquareIndex, sizeof(gSquareIndex));
	square2->setScale(3.0f, 3.0f, 3.0f);
	square2->setPosition(3.0f, -5.0f, 0.0f);
	square2->setUvs(texture, sizeof(texture));
	square2->setNormals(gNormal, sizeof(gNormal));
	texturePath = android3d::Scene::ROOT_PATH + "logo.png";
	square2->setImage(texturePath);
	//mesh2->setColors(gTriangleColors, sizeof(gTriangleColors));
	square2->setTriangleNums(2);
	gScene->addMesh(square2);

	android3d::Image* image = new android3d::Image(texturePath);
	gScene->addImage(image);
}

void JNICALL Java_com_peony_android3d_Android3DLib_step(JNIEnv* jenv, jobject obj)
{
	gScene->drawFrame();
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

