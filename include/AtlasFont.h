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
 *  Created on: Dec 18, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */


#ifndef ATLASFONT_H_
#define ATLASFONT_H_

#include "android3d.h"
#include "ShaderManager.h"

#include <string>
#include <map>
#include <GLES2/gl2.h>

namespace android3d
{

struct CharCoord
{
	GLfloat coord[8];
};

class AtlasFont
{
public:
	AtlasFont();
	virtual ~AtlasFont();
	unsigned char* getData();
	inline int getWidth() {return mWidth;}
	inline int getHeight() {return mHeight;}
	inline bool hasAlpha() {return mHasAlpha;}
	void setPosition(int x, int y, DrawAnchor anchor = CENTER);
	void deleteData();

	void drawString(const char* str);

private:
	std::string    mName;
	unsigned char* mData;
	unsigned int   mWidth;
	unsigned int   mHeight;
	bool           mHasAlpha;
	int            mPixelFormat;
	bool           mGLHasInitialized;

	GLuint         mTextureId;
	GLuint         mTextureVBO;
	GLuint         mTextureLocation;
	GLuint         mSamplerLocation;

	GLuint         mHudMVPMatrixLocation;
	glm::mat4      mHudMVPMatrix;
	glm::mat4      mModelMatrix;
	GLuint         mVetextLocation;
	GLuint         mVertexVBO;
	ShaderManager* mShaderManager;

	GLfloat        mFontWidth;
	GLfloat        mFontHeight;
	int            mCoodX;
	int            mCoodY;

    std::map<char, std::pair<int, int> > mCharMap;

    void initCharMap();
	void initGlCmds();
	/**
	 * buffer uses to store image data
	 */
	void loadTexture(std::string fileName);
	void load_bmp();
	void fetchPallete(FILE *fd, Color pallete[], int count);
};

} //end namespace

#endif /* ATLASFONT_H_ */
