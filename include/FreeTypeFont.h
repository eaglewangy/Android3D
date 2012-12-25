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
 *  File:       FreetypeFont.h
 *  Created on: Dec 15, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

#ifndef FREETYPEFONT_H_
#define FREETYPEFONT_H_

#include "android3d.h"
#include "ShaderManager.h"
#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace android3d
{

class FreeTypeFont : public Font
{
public:
	FreeTypeFont(std::string fontFile);
	bool initFont(int size);
	void initGlCmds();
	void drawString(const char* str);
	virtual void setPosition(int x, int y, DrawAnchor anchor = CENTER){};
	void renderText(const char *text, float x, float y, float sx, float sy);
	~FreeTypeFont();

private:
	std::string    mFontFile;
	FT_Face        mFace;
	ShaderManager* mShaderManager;
	GLuint         mHudMVPMatrixLocation;
	glm::mat4      mHudMVPMatrix;

	bool           mGLHasInitialized;

	GLuint         mVetextLocation;
	GLuint         mVertexVBO;
	GLuint         mTextureId;
	GLuint         mTextureVBO;
	GLuint         mTextureLocation;
	GLuint         mSamplerLocation;
	GLuint         mColorLocation;

	GLuint* mTextureData;
}; //end FreeTypeFont

} //end namespace
#endif /* FREETYPEFONT_H_ */
