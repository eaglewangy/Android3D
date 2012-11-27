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

#ifndef MESH_H_
#define MESH_H_

#include "Scene.h"

#include <GLES2/gl2.h>

namespace android3d
{

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	void setVertices(GLfloat* vertices, int size);
	void setNormals(GLfloat* normals, int size);
	void setUvs(GLfloat* uvs, int size);
	void setColors(GLubyte* colors, int size);
	void setIndices(GLushort* indices, int size);
	void setTextureId(GLint textureId);
	void setTriangleNums(GLint triangleNums);
	void setEnabled(GLboolean enabled);
	void setPosition(GLfloat x, GLfloat y, GLfloat z);
	void setRotate(GLfloat x, GLfloat y, GLfloat z);
	void setScale(GLfloat x, GLfloat y, GLfloat z);

	/**
	 * Render mesh
	 */
	void render();

protected:
	GLfloat* mVertex;
	//vertex array size in bytes
	int mVertexSize;
	GLushort* mIndex;
	//vertext array index size in bytes
	int mIndexSize;
	GLfloat* mUVS;

	GLfloat* mNormals;
	GLubyte* mColors;

	GLint    mTriangleNums;
	GLboolean   mEnabled;
	/* private functions */
	void initGlCmds();

	glm::mat4 mMVPMatrix;
	glm::mat4 mModelMatrix;
	glm::vec3* mTanslateVec;
	glm::vec3* mRotateVec;
	glm::vec3* mScaleVec;
	/* vetext position location */
	GLuint mVetextLocation;
	/* MVP matrix shader location */
	GLuint mMVPMatrixLocation;
	GLuint mTextureLocation;
	GLuint mSamplerLocation;
	/* shader program */
	static GLuint mShaderProgram;
	static GLint  mVertexShader;
	static GLint  mFragmentShader;
	GLuint mTextureId;
	GLuint* mTextureData;
	GLuint mVertexVBO[2];

	bool mHasInitialized;
}; //end mesh

} //end namespace

#endif /* MESH_H_ */
