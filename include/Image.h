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
 *  Created on: Nov 29, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */


#ifndef IMAGE_H_
#define IMAGE_H_

#include "android3d.h"
#include "ShaderManager.h"

#include <string>
#include <GLES2/gl2.h>

namespace android3d
{

enum ImageType {
	TYPE_NONE = 0,
	TYPE_PNG,
	TYPE_JPEG
};

class Image
{
public:
	Image(std::string fileName);
	virtual ~Image();
	unsigned char* getData();
	inline int getWidth() {return mWidth;};
	inline int getHeight() {return mHeight;};
	inline bool hasAlpha() {return mHasAlpha;};

	void drawImage();

private:
	std::string    mName;
	unsigned char* mData;
	unsigned int   mWidth;
	unsigned int   mHeight;
	bool           mHasAlpha;
	int            mPixelFormat;
	ImageType      mImageType;
	bool           mGLHasInitialized;

	GLuint         mTextureId;
	GLuint         mTextureVBO;
	GLuint         mTextureLocation;
	GLuint         mSamplerLocation;

	GLuint         mHudMVPMatrixLocation;
	glm::mat4      mHudMVPMatrix;
	GLuint         mVetextLocation;
	GLuint         mVertexVBO[2];
	ShaderManager* mShaderManager;

	void initGlCmds();
	/**
	 * buffer uses to store image data
	 */
	void loadTexture(std::string fileName);
	void read_png();
	void read_jpeg();

	/* vertically flip a buffer, note, this operates on a buffer of widthBytes bytes, not pixels!*/
	bool flipBufferByVertical(unsigned char* input, int widthBytes, int height);
};

} //end namespace

#endif /* IMAGE_H_ */
