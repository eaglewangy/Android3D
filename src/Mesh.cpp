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

#include "Mesh.h"

#include "android3d.h"
#include "ShaderManager.h"

namespace android3d
{

static const char gVertexShader[] =
		// A constant representing the combined model/view/projection matrix.
		"uniform mat4 u_MVPMatrix;\n"

		"attribute vec4 vPosition;\n"
		"void main() {\n"
		"  gl_Position = u_MVPMatrix * vPosition;\n"
		"}\n";

static const char gFragmentShader[] =
		"precision mediump float;\n"
		"void main() {\n"
		"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
		"}\n";

Mesh::Mesh() :
mVertices(NULL),
mNormals(NULL),
mUVS(NULL),
mColors(NULL),
mIndices(NULL),
mPosition(NULL),
mTanslateVec(NULL),
mRotateVec(NULL),
mRotate(NULL),
mScale(NULL),
mTextureId(-1),
mTriangleNums(0),
mEnabled(GL_FALSE)
{
}

Mesh::~Mesh()
{
	FREEANDNULL(mVertices);
	FREEANDNULL(mTanslateVec);
	FREEANDNULL(mRotateVec);
	DELETEANDNULL(mNormals, true);
	DELETEANDNULL(mUVS, true);
	DELETEANDNULL(mColors, true);
	FREEANDNULL(mIndices);
	DELETEANDNULL(mPosition, true);
	DELETEANDNULL(mRotate, true);
	DELETEANDNULL(mScale, true);
	LOGI("Delete meshes...");
}

void Mesh::setVertices(GLfloat* vertices, int size)
{
	FREEANDNULL(mVertices);
	mVertices = (GLfloat*)malloc(size);
	memcpy(mVertices, vertices, size);

	setEnabled(GL_TRUE);
}

void Mesh::setNormals(GLfloat* normals, int size)
{
	DELETEANDNULL(mNormals, true);
	mNormals = new GLfloat[size];
	memcpy(mNormals, normals, size);
}

void Mesh::setUvs(GLfloat* uvs, int size)
{
	DELETEANDNULL(mUVS, true);
	mUVS = new GLfloat[size];
	memcpy(mUVS, uvs, size);
}

void Mesh::setIndices(GLushort* indices, int size)
{
	FREEANDNULL(mIndices);
	mIndices = (GLushort*)malloc(size);
	memcpy(mIndices, indices, size);
}

void Mesh::setTriangleNums(GLint triangleNums)
{
	mTriangleNums = triangleNums;
}

void Mesh::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
	if (mPosition == NULL)
	{
		mPosition = new GLfloat(3 * sizeof(GLfloat));
	}
	if (mTanslateVec == NULL)
	{
		mTanslateVec = new glm::vec3();
	}

	mTanslateVec->x = x;
	mTanslateVec->y = y;
	mTanslateVec->z = z;
	mPosition[0] = x;
	mPosition[1] = y;
	mPosition[2] = z;
}

void Mesh::setRotate(GLfloat x, GLfloat y, GLfloat z)
{
	if (mRotate == NULL) {
		mRotate = new GLfloat(3 * sizeof(GLfloat));
	}

	if (mRotateVec == NULL)
	{
		mRotateVec = new glm::vec3();
	}
	mRotateVec->x = x;
	mRotateVec->y = y;
	mRotateVec->z = z;

	mRotate[0] = x;
	mRotate[1] = y;
	mRotate[2] = z;
}

void Mesh::setScale(GLfloat x, GLfloat y, GLfloat z)
{
	if (mScale == NULL) {
		mScale = new GLfloat(3 * sizeof(GLfloat));
	}

	mScale[0] = x;
	mScale[1] = y;
	mScale[2] = z;
}

void Mesh::setEnabled(GLboolean enabled)
{
	mEnabled = enabled;
}

void Mesh::initGlCmds()
{
	mMVPMatrix = Scene::getInstance()->getCamera()->getMVP();

	mShaderProgram = android3d::ShaderManager::createProgram(gVertexShader, gFragmentShader);
	mMVPMatrixLocation = glGetUniformLocation(mShaderProgram, "u_MVPMatrix");
	mVetextLocation = glGetAttribLocation(mShaderProgram, "vPosition");

	glUseProgram(mShaderProgram);
	glVertexAttribPointer(mVetextLocation, 3, GL_FLOAT, GL_FALSE, 0, mVertices);
	glEnableVertexAttribArray(mVetextLocation);
}

void Mesh::render()
{
	if (!mEnabled)
		return;

	initGlCmds();

	mModelMatrix = glm::mat4(1.0);
	if (mTanslateVec != NULL)
	{
		mModelMatrix = glm::translate(mModelMatrix, *mTanslateVec);
	}
	if (mRotateVec != NULL)
	{
		if (fabs(mRotateVec->x) > ANDROID3D_EPSILON)
			mModelMatrix = glm::rotate(mModelMatrix, mRotateVec->x, glm::vec3(1.0f, 0.0f, 0.0f));
		if (fabs(mRotateVec->y) > ANDROID3D_EPSILON)
			mModelMatrix = glm::rotate(mModelMatrix, mRotateVec->y, glm::vec3(0.0f, 1.0f, 0.0f));
		if (fabs(mRotateVec->z) > ANDROID3D_EPSILON)
			mModelMatrix = glm::rotate(mModelMatrix, mRotateVec->z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	//glNormalPointer(GL_FLOAT, 0, mVertices);


	/*static int count;
	++count;
	if (count == 1)
	   printMatrix(const_cast<float*>(glm::value_ptr(camera->getMVP() * mModelMatrix)), 1);*/

	glUniformMatrix4fv(mMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mMVPMatrix * mModelMatrix));

	if (mIndices != NULL)
	{
		glDrawElements(GL_TRIANGLES, mTriangleNums * 3, GL_UNSIGNED_SHORT, mIndices);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, mTriangleNums * 3);
		//LOGE("this %d", mTriangleNums);
	}
}

}




