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

		/*
		     // front
		    -1.0, -1.0,  1.0,
		     1.0, -1.0,  1.0,
		     1.0,  1.0,  1.0,
		    -1.0,  1.0,  1.0,
		    // back
		    -1.0, -1.0, -1.0,
		     1.0, -1.0, -1.0,
		     1.0,  1.0, -1.0,
		    -1.0,  1.0, -1.0,
		 */
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

		/*
		 * // front
		    0, 1, 2,
		    2, 3, 0,
		    // top
		    1, 5, 6,
		    6, 2, 1,
		    // back
		    7, 6, 5,
		    5, 4, 7,
		    // bottom
		    4, 0, 3,
		    3, 7, 4,
		    // left
		    4, 5, 1,
		    1, 0, 4,
		    // right
		    3, 2, 6,
		    6, 7, 3,
		 */
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

#endif // ANDROID3D_JNI_H
