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

namespace android3d
{

static const char gVertexShader[] =
		// A constant representing the combined model/view/projection matrix.
		"uniform mat4 u_MVPMatrix;               \n"
		"attribute vec4 vPosition;               \n"
		"attribute vec2 a_texCoord;              \n"
		"varying vec2 v_texCoord;                \n"
		"attribute vec4 a_vcolor;                \n"
		"varying vec4 v_color;                   \n"
		"uniform lowp int u_enableTexture;       \n"
		"uniform lowp int u_enableVertexColor;   \n"

		"void main() {                           \n"
		"  gl_Position = u_MVPMatrix * vPosition;\n"
		"  if(u_enableTexture == 1) {            \n"
		"      v_texCoord = a_texCoord;          \n"
		"  }                                     \n"
		"  if (u_enableVertexColor == 1){        \n"
		"      v_color = a_vcolor;               \n"
		"  }                                     \n"
		"}                                       \n";

static const char gFragmentShader[] =
		"precision mediump float;                              \n"
		"varying vec4 v_color;                                 \n"
		"uniform lowp int u_enableTexture;                     \n"
		"uniform lowp int u_enableVertexColor;                 \n"
		"uniform sampler2D s_texture;                          \n"
		"varying vec2 v_texCoord;                              \n"
		"void main() {                                         \n"
		"  if (u_enableTexture == 1) {                         \n"
		"      gl_FragColor = texture2D(s_texture, v_texCoord);\n"
		"  }                                                   \n"
		"  else if (u_enableVertexColor == 1) {                \n"
		"      gl_FragColor = v_color;                         \n"
		"  }                                                   \n"
		"  else {                                              \n"
		"      gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);        \n"
		"  }                                                   \n"
		"}                                                     \n";


GLuint Mesh::mShaderProgram = 0;
GLint  Mesh::mVertexShader = 0;
GLint  Mesh::mFragmentShader = 0;

Mesh::Mesh() :
mVertex(NULL),
mVertexSize(0),
mNormals(NULL),
mUVS(NULL),
mColors(NULL),
mColorSize(0),
mColorVBO(0),
mEnableVertexColorLocation(-1),
mIndex(NULL),
mIndexSize(0),
mTanslateVec(NULL),
mRotateVec(NULL),
mScaleVec(NULL),
mTextureId(-1),
mTriangleNums(0),
mVetextLocation(-1),
mMVPMatrixLocation(-1),
mTextureLocation(-1),
mEnableTextureLocation(-1),
mColorLocation(-1),
mTextureData(NULL),
mSamplerLocation(-1),
mHasInitialized(false),
mEnabled(GL_FALSE)
{
	memset(mVertexVBO, 0, sizeof(mVertexVBO));
}

Mesh::~Mesh()
{
	FREEANDNULL(mVertex);
	FREEANDNULL(mIndex);
	FREEANDNULL(mTanslateVec);
	FREEANDNULL(mRotateVec);
	FREEANDNULL(mScaleVec);
	FREEANDNULL(mUVS);
	DELETEANDNULL(mTextureData, true);

	DELETEANDNULL(mNormals, true);
	DELETEANDNULL(mColors, true);

	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragmentShader);
	glDeleteTextures(1, &mTextureId);
	mTextureId = -1;
	// Release Vertex buffer object.
	glDeleteBuffers(2, mVertexVBO);
	memset(mVertexVBO, 0, sizeof(mVertexVBO));
	//Relsase Color buffer object
	glDeleteBuffers(1, &mColorVBO);
	mColorVBO = 0;

	mShaderProgram = 0;
	mVertexShader = 0;
	mFragmentShader = 0;

	mEnableVertexColorLocation = -1;

	LOGI("Delete meshes...");
}

void Mesh::setVertices(GLfloat* vertex, int size)
{
	FREEANDNULL(mVertex);
	mVertex = (GLfloat*)malloc(size);
	memcpy(mVertex, vertex, size);
	mVertexSize = size;

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

void Mesh::setIndices(GLushort* index, int size)
{
	FREEANDNULL(mIndex);
	mIndex = (GLushort*)malloc(size);
	memcpy(mIndex, index, size);

	mIndexSize = size;
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

void Mesh::setColors(GLfloat* colors, int size)
{
	FREEANDNULL(mColors);
	mColors = (GLfloat *) malloc(size);
	memcpy(mColors, colors, size);

	mColorSize = size;
}

void Mesh::setEnabled(GLboolean enabled)
{
	mEnabled = enabled;
}

void Mesh::initGlCmds()
{
	mMVPMatrix = Scene::getInstance()->getCamera()->getMVP();

	if (mShaderProgram == 0 &&
		mVertexShader == 0 &&
		mFragmentShader == 0)
	{
		mShaderProgram = mShaderManager.createProgram(gVertexShader, gFragmentShader);
		mVertexShader = mShaderManager.getVertexShader();
		mFragmentShader = mShaderManager.getFragmentShader();
	}
	if (mShaderProgram == 0)
	{
		LOGE("In Mesh::initGlCmds() create shader failed.");
		return;
	}
	mMVPMatrixLocation = glGetUniformLocation(mShaderProgram, "u_MVPMatrix");
	mVetextLocation = glGetAttribLocation(mShaderProgram, "vPosition");
	mTextureLocation = glGetAttribLocation(mShaderProgram, "a_texCoord");
	mSamplerLocation = glGetUniformLocation(mShaderProgram, "s_texture");
	mEnableTextureLocation = glGetUniformLocation(mShaderProgram, "u_enableTexture");
	mColorLocation = glGetAttribLocation(mShaderProgram, "a_vcolor");
	mEnableVertexColorLocation = glGetUniformLocation(mShaderProgram, "u_enableVertexColor");

	mModelMatrix = glm::mat4(1.0);

	if (mEnabled)
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
	if (mIndex != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexVBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexSize, mIndex, GL_STATIC_DRAW);
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
		glGenTextures(1, &mTextureId);
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

		// Set the filtering mode
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, mUVS);
		glEnableVertexAttribArray(mTextureLocation);
		// Bind the texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTextureId);
	}

	std::string texture = Scene::ROOT_PATH + "logo.png";
	Image image;
	image.read(texture);

	mHasInitialized = true;

}

void Mesh::render()
{
	if (!mEnabled)
		return;

	if (!mHasInitialized)
		initGlCmds();

	glUseProgram(mShaderProgram);
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[0]);
	glVertexAttribPointer(mVetextLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(mVetextLocation);

	if (mTextureId != -1)
	{
		glUniform1i(mEnableTextureLocation, 1);
		// Set the sampler texture unit to 0
		glUniform1i(mSamplerLocation, 0);
		int TEX_SIZE = 128;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, mTextureData);
	}
	else
	{
		glUniform1i(mEnableTextureLocation, 0);
	}

	if (mColors != NULL)
	{
		glUniform1i(mEnableVertexColorLocation, 1);

		glBindBuffer(GL_ARRAY_BUFFER, mColorVBO);
		glVertexAttribPointer(mColorLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(mColorLocation);
	}
	else
	{
		glUniform1i(mEnableVertexColorLocation, 0);
		glDisableVertexAttribArray(mEnableVertexColorLocation);
	}

	/*static int count;
	++count;
	if (count == 1)
	   printMatrix(const_cast<float*>(glm::value_ptr(camera->getMVP() * mModelMatrix)), 1);
	*/

	glUniformMatrix4fv(mMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mMVPMatrix * mModelMatrix));

	if (mIndex != NULL)
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




