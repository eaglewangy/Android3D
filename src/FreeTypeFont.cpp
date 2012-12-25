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
 *  File:       FreetypeFont.cpp
 *  Created on: Dec 15, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

#include "FreeTypeFont.h"
#include "Utils.h"
#include "Scene.h"

namespace android3d
{

static const char* vertexSource = {
		"uniform mat4 u_mvpMatrix;                 \n"
		"attribute vec4 a_position;                \n"
		"attribute vec2 a_texCoord;                \n"
		"varying vec2 v_texCoord;                  \n"

		"void main() {                             \n"
		"  gl_Position = u_mvpMatrix * a_position; \n"
		"  v_texCoord = a_texCoord;                \n"
		"}                                         \n"
};

static const char* fragmentSource = {
		"precision mediump float;                                                          \n"
		"uniform sampler2D s_texture;                                                      \n"
		"uniform vec4 u_color;                                                             \n"
		"varying vec2 v_texCoord;                                                          \n"
		"void main() {                                                                     \n"
		"    gl_FragColor = texture2D(s_texture, v_texCoord);                              \n"
		//"    gl_FragColor = vec4(1, 1, 1, texture2D(s_texture, v_texCoord).a) * u_color;   \n"
		"}                                                                                 \n"
};

static GLfloat gVertex[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f,
};

static GLfloat gTexture[] =
{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
};

/*static GLfloat gTexture[] =
{
		0.0f, (float)6/(float)8,
		1/(float)16, (float)6/(float)8,
		1/(float)16, (float)7/(float)8,
		0.0f, (float)7/(float)8,
};*/

FreeTypeFont::FreeTypeFont(std::string fontFile) :
mFontFile(fontFile),
mShaderManager(NULL),
mHudMVPMatrixLocation(-1),
mVetextLocation(-1),
mVertexVBO(-1),
mTextureId(-1),
mTextureVBO(-1),
mTextureLocation(-1),
mSamplerLocation(-1),
mColorLocation(-1),
mGLHasInitialized(false),
Font(fontFile)
{
	initFont(48);
}

FreeTypeFont::~FreeTypeFont()
{
	DELETEANDNULL(mShaderManager, false);
	glDeleteBuffers(1, &mTextureVBO);
}

bool FreeTypeFont::initFont(int size)
{
	FT_Library ft;
	/*initialize freetype library*/
	if(FT_Init_FreeType(&ft))
	{
		LOGE("Failed to init freetype library.");
		return false;
	}
	/*Load font face. ex: FreeSans.ttf*/
	if(FT_New_Face(ft, mFontFile.c_str(), 0, &mFace))
	{
		LOGE("Failed to open %s", mFontFile.c_str());
		return false;
	}
	FT_Select_Charmap(mFace, ft_encoding_unicode);
	/*Set font size.*/
	FT_Set_Pixel_Sizes(mFace, 0, size);
	LOGI("Freetype font init...");
	return true;
}

void FreeTypeFont::initGlCmds()
{
	std::string vertex = vertexSource;
	std::string fragment = fragmentSource;
	mShaderManager = new ShaderManager(vertex, fragment, false);
	GLuint program = mShaderManager->getProgram();

	if (program == 0)
	{
		LOGE("In Font::initGlCmds() program is 0");
	}
	mHudMVPMatrixLocation = glGetUniformLocation(program, "u_mvpMatrix");
	mVetextLocation = glGetAttribLocation(program, "a_position");
	mTextureLocation = glGetAttribLocation(program, "a_texCoord");
	mSamplerLocation = glGetUniformLocation(program, "s_texture");
	mColorLocation = glGetUniformLocation(program, "u_color");

	glGenBuffers(1, &mVertexVBO);
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);

	if(FT_Load_Char(mFace, 'A', FT_LOAD_RENDER)) {
		LOGE("Could not load character 'X'\n");
		return ;
	}
	FT_GlyphSlot g = mFace->glyph;
	float sx = 2.0 / Scene::getInstance()->getWidth();
	float sy = 2.0 / Scene::getInstance()->getHeight();
	float x2 = 0.1 + g->bitmap_left * sx;
	float y2 = -0.1 - g->bitmap_top * sy;
	float w = g->bitmap.width * sx;
	float h = g->bitmap.rows * sy;

	GLfloat box[] = {
			x2,     -y2 ,
			x2 + w, -y2 ,
			x2,     -y2 - h,
			x2 + w, -y2 - h,
	};

	glGenTextures(1, &mTextureId);
	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	glTexImage2D(
	      GL_TEXTURE_2D,
	      0,
	      GL_ALPHA,
	      g->bitmap.width,
	      g->bitmap.rows,
	      0,
	      GL_ALPHA,
	      GL_UNSIGNED_BYTE,
	      g->bitmap.buffer
	    );

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, mTextureData);
	//delete[] mTextureData;
	float sceneWidth = Scene::getInstance()->getWidth();
	float sceneHeight = Scene::getInstance()->getHeight();
	float a = g->bitmap.width / sceneWidth;
	float b = g->bitmap.rows / sceneHeight;

	glm::mat4 mModelMatrix = glm::mat4(1.0);
	mModelMatrix = glm::translate(mModelMatrix, glm::vec3(0.5, 0.5, 0));
	mModelMatrix = glm::scale(mModelMatrix, glm::vec3(a, b, 0));
	Scene::getInstance()->getCamera()->updateHudMVP(128, 128);
	mHudMVPMatrix = Scene::getInstance()->getCamera()->getHudMVP() * mModelMatrix;

	mGLHasInitialized = true;
}

void FreeTypeFont::drawString(const char* str)
{
	if (!mGLHasInitialized)
		initGlCmds();

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glClear(GL_COLOR_BUFFER_BIT);

	GLfloat black[4] = {0, 0, 0, 1};
	glUniform4fv(mColorLocation, 1, black);

	glUseProgram(mShaderManager->getProgram());
	// Bind the VBO
	//glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);
	glVertexAttribPointer(mVetextLocation, 2, GL_FLOAT, GL_FALSE, 0, gVertex);
	glEnableVertexAttribArray(mVetextLocation);

	glBindTexture(GL_TEXTURE_2D, mTextureId);
	// Set the sampler texture unit to 0
	glUniform1i(mSamplerLocation, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, mTextureVBO);
	glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, gTexture);
	glEnableVertexAttribArray(mTextureLocation);

	glUniformMatrix4fv(mHudMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mHudMVPMatrix));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// Unbind the VBO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//renderText("The Quick Brown Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 50 * sy, sx, sy);
}

void FreeTypeFont::renderText(const char *text, float x, float y, float sx, float sy)
{
	const char *p;
	FT_GlyphSlot g = mFace->glyph;
	for(p = text; *p; p++)
	{
		if(FT_Load_Char(mFace, *p, FT_LOAD_RENDER))
			continue;

		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_ALPHA,
				g->bitmap.width,
				g->bitmap.rows,
				0,
				GL_ALPHA,
				GL_UNSIGNED_BYTE,
				g->bitmap.buffer
		);

		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		GLfloat box[4][4] = {
				{x2,     -y2    , 0, 0},
				{x2 + w, -y2    , 1, 0},
				{x2,     -y2 - h, 0, 1},
				{x2 + w, -y2 - h, 1, 1},
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}
}

} //end namespace
