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
#include "Image.h"
#include "Utils.h"

#include <glm/gtc/matrix_inverse.hpp>

namespace android3d
{

Mesh::Mesh() :
mVertex(NULL),
mVertexSize(0),
mVertexIndex(NULL),
mVertexIndexSize(0),
mVetextLocation(-1),
mVertexEnabled(GL_FALSE),
mTriangleNums(0),
mUVS(NULL),
mUVSSize(0),
mTextureImage(NULL),
mTextureId(-1),
mTextureLocation(-1),
mEnableTextureLocation(-1),
mTextureData(NULL),
mSamplerLocation(-1),
mTextureVBO(0),
mNormal(NULL),
mNormalSize(0),
mNormalLocation(-1),
mEnableLight(false),
mDirectionalLightDir(NULL),
mLightDirectionLoc(-1),
mNormalMatrixLoc(-1),
mNormalVBO(0),
mColors(NULL),
mColorSize(0),
mColorVBO(0),
mEnableColorLocation(-1),
mColorLocation(-1),
mTanslateVec(NULL),
mRotateVec(NULL),
mScaleVec(NULL),
mMVPMatrixLocation(-1),
mGLHasInitialized(false),
mIsRotated(false),
mShaderManager(NULL)
{
	memset(mVertexVBO, 0, sizeof(mVertexVBO));
}

Mesh::~Mesh()
{
	FREEANDNULL(mVertex);
	FREEANDNULL(mVertexIndex);
	FREEANDNULL(mTanslateVec);
	FREEANDNULL(mRotateVec);
	FREEANDNULL(mScaleVec);
	FREEANDNULL(mUVS);
	FREEANDNULL(mNormal);
	DELETEANDNULL(mTextureImage, false);
	DELETEANDNULL(mTextureData, true);

	DELETEANDNULL(mColors, true);

	glDeleteTextures(1, &mTextureId);
	mTextureId = -1;
	// Release Vertex buffer object.
	glDeleteBuffers(2, mVertexVBO);
	memset(mVertexVBO, 0, sizeof(mVertexVBO));
	//Relsase Color buffer object
	glDeleteBuffers(1, &mColorVBO);
	mColorVBO = 0;
	glDeleteBuffers(1, &mTextureVBO);
	mTextureVBO = 0;
	glDeleteBuffers(1, &mNormalVBO);
	mNormalVBO = 0;

	DELETEANDNULL(mShaderManager, false);

	mEnableColorLocation = -1;
}

void Mesh::setVertices(GLfloat* vertex, int size)
{
	FREEANDNULL(mVertex);
	mVertex = (GLfloat*)malloc(size);
	memcpy(mVertex, vertex, size);
	mVertexSize = size;

	setEnabled(GL_TRUE);
}

void Mesh::setUvs(GLfloat* uvs, int size)
{
	FREEANDNULL(mUVS);
	mUVS = (GLfloat*)malloc(size);
	memcpy(mUVS, uvs, size);

	mUVSSize = size;
}

void Mesh::setImage(std::string file)
{
	DELETEANDNULL(mTextureImage, false);
	mTextureImage = new Image(file);
	//mTextureImage->loadTexture();
}

void Mesh::setIndices(GLushort* index, int size)
{
	FREEANDNULL(mVertexIndex);
	mVertexIndex = (GLushort*)malloc(size);
	memcpy(mVertexIndex, index, size);

	mVertexIndexSize = size;
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

void Mesh::setRotate(GLfloat x, GLfloat y, GLfloat z, bool isRotate)
{
	if (mRotateVec == NULL)
	{
		mRotateVec = new glm::vec3();
	}
	mRotateVec->x = x;
	mRotateVec->y = y;
	mRotateVec->z = z;

	mIsRotated = isRotate;
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

void Mesh::setColors(GLfloat* colors, int size)
{
	FREEANDNULL(mColors);
	mColors = (GLfloat *) malloc(size);
	memcpy(mColors, colors, size);

	mColorSize = size;
}

void Mesh::setNormals(GLfloat* normals, int size)
{
	FREEANDNULL(mNormal);
	mNormal = (GLfloat *) malloc(size);
	memcpy(mNormal, normals, size);

	mNormalSize = size;
}

void Mesh::setEnabled(GLboolean enabled)
{
	mVertexEnabled = enabled;
}

void Mesh::initGlCmds()
{
	mMVPMatrix = Scene::getInstance()->getCamera()->getMVP();

	mShaderManager = new ShaderManager("mesh.vsh", "mesh.fsh");
	GLuint program = mShaderManager->getProgram();

	if (program == 0)
	{
		LOGE("In Mesh::initGlCmds() program is 0");
	}
	mMVPMatrixLocation = glGetUniformLocation(program, "u_mvpMatrix");
	mVetextLocation = glGetAttribLocation(program, "vPosition");
	mTextureLocation = glGetAttribLocation(program, "a_texCoord");
	mSamplerLocation = glGetUniformLocation(program, "s_texture");
	mEnableTextureLocation = glGetUniformLocation(program, "u_enableTexture");
	mColorLocation = glGetAttribLocation(program, "a_vcolor");
	mEnableColorLocation = glGetUniformLocation(program, "u_enableVertexColor");
	mNormalLocation =  glGetAttribLocation(program, "a_normal");
	mEnableLight = glGetUniformLocation(program, "u_enableLight");
	mLightDirectionLoc = glGetUniformLocation(program, "u_lightDir");
	mNormalMatrixLoc = glGetUniformLocation(program, "u_normalMatrix");

	mModelMatrix = glm::mat4(1.0);

	if (mVertexEnabled)
	{
		/*
		 * mVertextVBO[0] store vertex attribute data.
		 * mVertextVBO[1] store elements indices.
		 */
		glGenBuffers(2, mVertexVBO);
		// Bind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[0]);
		// Set the buffer's data
		glBufferData(GL_ARRAY_BUFFER, mVertexSize, mVertex, GL_STATIC_DRAW);
		// Unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	if (mColors != NULL)
	{
		glGenBuffers(1, &mColorVBO);
		// Bind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, mColorVBO);
		// Set the buffer's data
		glBufferData(GL_ARRAY_BUFFER, mColorSize, mColors, GL_STATIC_DRAW);
		// Unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	if (mVertexIndex != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexVBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mVertexIndexSize, mVertexIndex, GL_STATIC_DRAW);
		// Unbind the VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

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

	if (mUVS != NULL)
	{
		glGenBuffers(1, &mTextureVBO);
		// Bind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, mTextureVBO);
		// Set the buffer's data
		glBufferData(GL_ARRAY_BUFFER, mUVSSize, mUVS, GL_STATIC_DRAW);
		// Unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenTextures(1, &mTextureId);
#if 0
		int TEX_SIZE = 128;
		// Creates the data as a 32bits integer array (8bits per component)
		mTextureData = new GLuint[TEX_SIZE*TEX_SIZE];
		for (int i=0; i<TEX_SIZE; i++)
			for (int j=0; j<TEX_SIZE; j++)
			{
				// Fills the data with a fancy pattern
				GLuint col = (255<<24) + ((255-j*2)<<16) + ((255-i)<<8) + (255-i*2);
				if ( ((i*j)/8) % 2 ) col = (GLuint) (255<<24) + (255<<16) + (0<<8) + (255);
				mTextureData[j*TEX_SIZE+i] = col;
			}
#endif
		// Set the filtering mode
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Bind the texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTextureId);

		if(mTextureImage != NULL && mTextureImage->hasAlpha())
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureImage->getWidth(),
					mTextureImage->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, mTextureImage->getData());
		else if (mTextureImage != NULL && !mTextureImage->hasAlpha())
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mTextureImage->getWidth(),
					mTextureImage->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, mTextureImage->getData());
		}

		mTextureImage->deleteData();
	}

	if (mNormal != NULL)
	{
		glGenBuffers(1, &mNormalVBO);
		// Bind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, mNormalVBO);
		// Set the buffer's data
		glBufferData(GL_ARRAY_BUFFER, mNormalSize, mNormal, GL_STATIC_DRAW);
		// Unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		mDirectionalLightDir = glm::vec4(0.0, 0.0, 1.0, 0.0);
		mLightDirectionLoc = glGetUniformLocation(program, "u_lightDir");
		glm::mat4 projectMat = Scene::getInstance()->getCamera()->getProjextMatrix();
		glm::mat4 viewMat = Scene::getInstance()->getCamera()->getViewMatrix();
		glm::mat4 viewModelMat = viewMat * mModelMatrix;
		mNormalMatrix = glm::mat3(viewModelMat);
	}

	mTransform = mMVPMatrix * mModelMatrix;

	mGLHasInitialized = true;
}

void Mesh::render()
{
	if (!mVertexEnabled)
		return;

	if (!mGLHasInitialized)
		initGlCmds();

	glUseProgram(mShaderManager->getProgram());
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[0]);
	glVertexAttribPointer(mVetextLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(mVetextLocation);

	if (mTextureId != -1)
	{
		glBindTexture(GL_TEXTURE_2D, mTextureId);
		glUniform1i(mEnableTextureLocation, 1);
		// Set the sampler texture unit to 0
		glUniform1i(mSamplerLocation, 0);
		glBindBuffer(GL_ARRAY_BUFFER, mTextureVBO);

		glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, NULL);
		glEnableVertexAttribArray(mTextureLocation);
	}
	else
	{
		glUniform1i(mEnableTextureLocation, 0);
	}

	if (mColors != NULL)
	{
		glUniform1i(mEnableColorLocation, 1);

		glBindBuffer(GL_ARRAY_BUFFER, mColorVBO);
		glVertexAttribPointer(mColorLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(mColorLocation);
	}
	else
	{
		glUniform1i(mEnableColorLocation, 0);
		glDisableVertexAttribArray(mEnableColorLocation);
	}

	if (mNormal != NULL)
	{
		glUniform1i(mEnableLight, 1);

		glUniform4fv(mLightDirectionLoc, 1, glm::value_ptr(mDirectionalLightDir));
		glUniformMatrix3fv(mNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(mNormalMatrix));

		glBindBuffer(GL_ARRAY_BUFFER, mNormalVBO);
		glVertexAttribPointer(mNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(mNormalLocation);
	}
	else
	{
		glUniform1i(mEnableLight, 0);
		glDisableVertexAttribArray(mNormalLocation);
	}

	/*static int count;
	++count;
	if (count == 1)
	   printMatrix(const_cast<float*>(glm::value_ptr(camera->getMVP() * mModelMatrix)), 1);
	*/

	glUniformMatrix4fv(mMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mTransform));

	if (mVertexIndex != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexVBO[1]);
		glDrawElements(GL_TRIANGLES, mTriangleNums * 3, GL_UNSIGNED_SHORT, NULL);
		// Unbind the VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, mTriangleNums * 3);
	}

	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}




