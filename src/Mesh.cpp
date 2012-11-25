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
#define TEXCOORD_ARRAY	1

static const char gVertexShader[] =
		// A constant representing the combined model/view/projection matrix.
		"uniform mat4 u_MVPMatrix;\n"
		"attribute vec2 a_texCoord;\n"
		"varying vec2 v_texCoord;\n"

		"attribute vec4 vPosition;\n"
		"void main() {\n"
		"  gl_Position = u_MVPMatrix * vPosition;\n"
		"  v_texCoord = a_texCoord;\n"
		"}\n";

static const char gFragmentShader[] =
		"precision mediump float;\n"
		"uniform sampler2D s_texture;\n"
		"varying vec2 v_texCoord;\n"
		"void main() {\n"
		"  gl_FragColor = texture2D(s_texture, v_texCoord);\n"
		//"  gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);\n"
		"}\n";

Mesh::Mesh() :
mVertices(NULL),
mNormals(NULL),
mUVS(NULL),
mColors(NULL),
mIndices(NULL),
mTanslateVec(NULL),
mRotateVec(NULL),
mScaleVec(NULL),
mTextureId(-1),
mTriangleNums(0),
mVetextLocation(-1),
mMVPMatrixLocation(-1),
mTextureLocation(-1),
mSamplerLocation(-1),
mHasInitialized(false),
mEnabled(GL_FALSE)
{
}

Mesh::~Mesh()
{
	FREEANDNULL(mVertices);
	FREEANDNULL(mTanslateVec);
	FREEANDNULL(mRotateVec);
	FREEANDNULL(mScaleVec);
	FREEANDNULL(mUVS);

	DELETEANDNULL(mNormals, true);
	DELETEANDNULL(mColors, true);
	FREEANDNULL(mIndices);

	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertextShader);
	glDeleteShader(mFragmentShader);
	glDeleteTextures(1, &mTextureId);
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
	FREEANDNULL(mUVS);
	mUVS = (GLfloat*)malloc(size);
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
	if (mTanslateVec == NULL)
	{
		mTanslateVec = new glm::vec3();
	}

	mTanslateVec->x = x;
	mTanslateVec->y = y;
	mTanslateVec->z = z;
}

void Mesh::setRotate(GLfloat x, GLfloat y, GLfloat z)
{
	if (mRotateVec == NULL)
	{
		mRotateVec = new glm::vec3();
	}
	mRotateVec->x = x;
	mRotateVec->y = y;
	mRotateVec->z = z;
}

void Mesh::setScale(GLfloat x, GLfloat y, GLfloat z)
{
	if (mScaleVec == NULL)
	{
		mScaleVec = new glm::vec3();
	}
	mScaleVec->x = x;
	mScaleVec->y = y;
	mScaleVec->z = z;
}

void Mesh::setEnabled(GLboolean enabled)
{
	mEnabled = enabled;
}

void Mesh::initGlCmds()
{
	mMVPMatrix = Scene::getInstance()->getCamera()->getMVP();

	mShaderProgram = android3d::ShaderManager::createProgram(mVertextShader, gVertexShader,
			mFragmentShader, gFragmentShader);
	if (mShaderProgram == 0)
	{
		LOGE("In Mesh::initGlCmds() create shader failed.");
		return;
	}
	mMVPMatrixLocation = glGetUniformLocation(mShaderProgram, "u_MVPMatrix");
	mVetextLocation = glGetAttribLocation(mShaderProgram, "vPosition");
	mTextureLocation = glGetAttribLocation(mShaderProgram, "a_texCoord");
	mSamplerLocation = glGetUniformLocation(mShaderProgram, "s_texture");

	glUseProgram(mShaderProgram);
	glVertexAttribPointer(mVetextLocation, 3, GL_FLOAT, GL_FALSE, 0, mVertices);
	glEnableVertexAttribArray(mVetextLocation);

	glGenTextures(1, &mTextureId);

	mHasInitialized = true;

}

void Mesh::render()
{
	if (!mEnabled)
		return;

    //if (!mHasInitialized)
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
	if (mScaleVec != NULL)
	{
		mModelMatrix = glm::scale(mModelMatrix, *mScaleVec);
	}

    //LOGE("mUVS: %d, textureID: %d", mUVS, mTextureId);
	if (mUVS != NULL && mTextureId != -1)
	{
		int TEX_SIZE = 128;
		// Creates the data as a 32bits integer array (8bits per component)
		GLuint* pTexData = new GLuint[TEX_SIZE*TEX_SIZE];
		for (int i=0; i<TEX_SIZE; i++)
			for (int j=0; j<TEX_SIZE; j++)
			{
				// Fills the data with a fancy pattern
				GLuint col = (255<<24) + ((255-j*2)<<16) + ((255-i)<<8) + (255-i*2);
				if ( ((i*j)/8) % 2 ) col = (GLuint) (255<<24) + (255<<16) + (0<<8) + (255);
				pTexData[j*TEX_SIZE+i] = col;
			}
		//glBindTexture(GL_TEXTURE_2D, mTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, pTexData);
		// Set the filtering mode
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, mUVS);
		glEnableVertexAttribArray(mTextureLocation);
		// Bind the texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTextureId);
		// Set the sampler texture unit to 0
		glUniform1i(mSamplerLocation, 0);
		LOGE("Using texture.");
	}
	else
	{
	   //glDisable(GL_TEXTURE_2D);
	}

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
	}

}

}




