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

#include "AtlasFont.h"
#include "Utils.h"
#include "Scene.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <png.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <jpeglib.h>
#ifdef __cplusplus
}
#endif

namespace android3d
{

#define CHAR_SIZE 16
#define ROW_COUNT 8.0
#define COLUMN_COUNT 16.0
#define FILLCHARMAP(map, pair, ch, left, bottom) \
	pair.first = left; \
	pair.second = bottom; \
	map[ch] = pair;

//bmp file offset
#define BMP_TORASTER_OFFSET	10
#define BMP_SIZE_OFFSET		18
#define BMP_BPP_OFFSET		28
#define BMP_RLE_OFFSET		30
#define BMP_COLOR_OFFSET	54
#define fill4B(a)    (( 4 - ( (a) % 4 ) ) & 0x03)

AtlasFont::AtlasFont(std::string fontFile) :
mName(fontFile),
mData(NULL),
mWidth(0),
mHeight(0),
mFontWidth(-1),
mFontHeight(-1),
mCoodX(20),
mCoodY(20),
mPixelFormat(0),
mHasAlpha(true),
mGLHasInitialized(false),
mHudMVPMatrixLocation(-1),
mVetextLocation(-1),
mTextureLocation(-1),
mSamplerLocation(-1),
mShaderManager(NULL),
mVertexVBO(-1),
mTextureId(-1),
mTextureVBO(0),
Font(fontFile)
{
	loadTexture(mName);
	initCharMap();
}

AtlasFont::~AtlasFont()
{
	FREEANDNULL(mData);
	glDeleteBuffers(1, &mVertexVBO);
	DELETEANDNULL(mShaderManager, false);
	LOGE("Free Image...");
}

void AtlasFont::deleteData()
{
	FREEANDNULL(mData);
}

void AtlasFont::loadTexture(std::string fileName)
{
	int pos = fileName.find_last_of(".");
	std::string extension = fileName.substr(pos + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

    load_bmp();
}

void AtlasFont::setPosition(int x, int y, DrawAnchor anchor)
{
	mCoodX = x;
	mCoodY = y;
}

void AtlasFont::initGlCmds()
{
	mShaderManager = new ShaderManager("texture.vsh", "texture.fsh");
	GLuint program = mShaderManager->getProgram();

	if (program == 0)
	{
		LOGE("In Mesh::initGlCmds() program is 0");
	}
	mHudMVPMatrixLocation = glGetUniformLocation(program, "u_mvpMatrix");
	mVetextLocation = glGetAttribLocation(program, "a_position");
	mTextureLocation = glGetAttribLocation(program, "a_texCoord");
	mSamplerLocation = glGetUniformLocation(program, "s_texture");

	/*
	 * mVertextVBO[0] store vertex attribute data.
	 * mVertextVBO[1] store elements indices.
	 */
	glGenBuffers(1, &mVertexVBO);
	glGenBuffers(1, &mTextureVBO);
	glGenTextures(1, &mTextureId);
	// Set the filtering mode
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mData);
	deleteData();

	float sceneWidth = Scene::getInstance()->getWidth();
	float sceneHeight = Scene::getInstance()->getHeight();
	int screenCoord[2];
	float glCoord[2];
	glm::mat4 mModelMatrix = glm::mat4(1.0);

	screenCoord[0] = mCoodX;
	screenCoord[1] = mCoodY;
	Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
	mModelMatrix = glm::translate(mModelMatrix, glm::vec3(glCoord[0], glCoord[1], 0));

	mModelMatrix = glm::scale(mModelMatrix, glm::vec3(2, 2, 0));
	Scene::getInstance()->getCamera()->updateHudMVP(mWidth, mHeight);
	mHudMVPMatrix = Scene::getInstance()->getCamera()->getHudMVP() * mModelMatrix;
	mFontWidth = 2.0f * CHAR_SIZE / sceneWidth;
	mFontHeight = mFontWidth * sceneWidth/sceneHeight;
	mGLHasInitialized = true;
}

void AtlasFont::drawString(const char* str)
{
	if (!mGLHasInitialized)
		initGlCmds();

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(mShaderManager->getProgram());
	// Set the sampler texture unit to 0
	glUniform1i(mSamplerLocation, 0);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glEnableVertexAttribArray(mVetextLocation);
	glEnableVertexAttribArray(mTextureLocation);
	glUniformMatrix4fv(mHudMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mHudMVPMatrix));

	std::pair<int, int> leftBottom;
	std::map<char, std::pair<int, int> >::iterator iter;
	GLfloat x_center = mFontWidth/2;
	GLfloat y_center = mFontHeight / 2;
	GLfloat x2 = 0.0;
    for (int i = 0; i < strlen(str); ++i)
    {
    	iter = mCharMap.find(str[i]);
    	if (iter == mCharMap.end())
    		continue;
    	leftBottom = iter->second;

    	GLfloat box[] = {
    			x2 - x_center,   -y_center,
    			x_center + x2,   -y_center,
    			x2 - x_center,   y_center,
    			x_center + x2,   y_center,
    	};

    	GLfloat texture[] = {
    			leftBottom.first/COLUMN_COUNT,       leftBottom.second/ROW_COUNT,
    			(leftBottom.first + 1)/COLUMN_COUNT, leftBottom.second/ROW_COUNT,
    			leftBottom.first/COLUMN_COUNT,       (leftBottom.second + 1)/ROW_COUNT,
    			(leftBottom.first + 1)/COLUMN_COUNT, (leftBottom.second + 1)/ROW_COUNT,
    	};
    	glVertexAttribPointer(mVetextLocation, 2, GL_FLOAT, GL_FALSE, 0, box);
    	glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, texture);
    	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    	x2 += mFontWidth;
    }
}

void AtlasFont::load_bmp()
{
	int  bpp, raster, i, j, skip, compression, width, height, index = 0;
	GLubyte buff[4];
	GLubyte id[2];
	Color pallete[256];

	FILE* fd = NULL;
	if ((fd = fopen(mName.c_str(), "rb")) == NULL)
	{
		LOGE("Can't open %s\n", mName.c_str());
		return;
	}
	//check is real bmp file
	fread(id, 2, 1, fd);

	if (!(id[0]=='B' && id[1]=='M') )
    {
		return;
	}

	if (fseek(fd, BMP_TORASTER_OFFSET, SEEK_SET) == -1)
	{
		return;
	}
	//read raster
	fread (buff, 4, 1, fd);
	raster = buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);

	if (fseek(fd, BMP_SIZE_OFFSET, SEEK_SET) == -1)
	{
		return;
	}
	//read width
	fread (buff, 4, 1, fd);
	width = buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);
	//read height
	fread (buff, 4, 1, fd);
	height = buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);
#ifdef DEBUG
	LOGV("Image width: %d, height: %d.\n", width, height);
#endif
	mWidth = width;
	mHeight = height;

	//read compression
	if (fseek(fd, BMP_RLE_OFFSET, SEEK_SET) == -1)
	{
		return;
	}
	fread (buff, 4, 1, fd);
	compression = buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);

	if (compression != 0) {
#ifdef DEBUG
		LOGV("Only uncompressed bitmap is supported\n");
#endif
		return;
	}

	//read bpp
	if (fseek(fd, BMP_BPP_OFFSET, SEEK_SET) == -1)
	{
		return;
	}
	//read(fd, buff, 2);
	fread(buff, 2, 1, fd);
	bpp = buff[0] + (buff[1]<<8);
#ifdef DEBUG
	LOGV("Image bpp: %d.\n", bpp);
#endif
	GLubyte *buffer = (GLubyte *) malloc(width * height * (bpp == 32 ? 4 : 3) * sizeof(GLubyte));
	if (!buffer)
	{
		LOGE("Out of memory in Image::load_bmp().");
		return;
	}

	GLint type = (bpp == 32 ? GL_RGBA : GL_RGB);
	if (type == GL_RGBA)
		mHasAlpha = true;
	else
		mHasAlpha = false;

	switch (bpp) {
	case 8: /* 8bit palletized */
	skip = fill4B(width);
	//if 8bit, read pallete first
	fetchPallete(fd, pallete, 256);
	fseek(fd, raster, SEEK_SET);
	//really read image data
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++)
		{
			//read(fd, buff, 1);
			fread (buff, 1, 1, fd);
			buffer[index++] = pallete[buff[0]].red;
			buffer[index++] = pallete[buff[0]].green;
			buffer[index++] = pallete[buff[0]].blue;
		}
		if (skip)
		{
			//read(fd, buff, skip);
			fread (buff, skip, 1, fd);
		}
	}
	break;
	case 24: /* 24bit RGB */
		skip = fill4B(width * 3);
		fseek(fd, raster, SEEK_SET);
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++)
			{
				//read(fd, buff, 3);
				fread (buff, 3, 1, fd);
				buffer[index++] = buff[2];
				buffer[index++] = buff[1];
				buffer[index++] = buff[0];
			}
			if (skip) {
				//read(fd, buff, skip);
				fread (buff, skip, 1, fd);
			}
		}
		break;
	case 32: /* 32 RGB */
		fseek(fd, raster, SEEK_SET);
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++)
			{
				//read(fd, buff, 3);
				fread (buff, 4, 1, fd);
				buffer[index++] = buff[2];
				buffer[index++] = buff[1];
				buffer[index++] = buff[0];
				buffer[index++] = buff[3];
			}
		}
		break;
	default:
#ifdef DEBUG
		LOGV("Unsupport bpp: %d.\n", bpp);
#endif
		return;
	}

	mData = buffer;
}

void AtlasFont::fetchPallete(FILE *fd, Color pallete[], int count)
{
	GLubyte buff[4];
	int i;

	fseek(fd, BMP_COLOR_OFFSET, SEEK_SET);
	for (i = 0; i < count; i++) {
		//read(fd, buff, 4);
		fread (buff, 4, 1, fd);
		pallete[i].red = buff[2];
		pallete[i].green = buff[1];
		pallete[i].blue = buff[0];
		pallete[i].alpha = buff[3];
	}
	return;
}

unsigned char* AtlasFont::getData()
{
	return mData;
}

void AtlasFont::initCharMap()
{
	std::pair<int, int> leftBottom;
	/* number 0-9 */
	FILLCHARMAP(mCharMap, leftBottom, '0', 0, 6);
	FILLCHARMAP(mCharMap, leftBottom, '1', 1, 6);
	FILLCHARMAP(mCharMap, leftBottom, '2', 2, 6);
	FILLCHARMAP(mCharMap, leftBottom, '3', 3, 6);
	FILLCHARMAP(mCharMap, leftBottom, '4', 4, 6);
	FILLCHARMAP(mCharMap, leftBottom, '5', 5, 6);
	FILLCHARMAP(mCharMap, leftBottom, '6', 6, 6);
	FILLCHARMAP(mCharMap, leftBottom, '7', 7, 6);
	FILLCHARMAP(mCharMap, leftBottom, '8', 8, 6);
	FILLCHARMAP(mCharMap, leftBottom, '9', 9, 6);

	/* letter A-Z a-z*/
	FILLCHARMAP(mCharMap, leftBottom, 'A', 1,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'B', 2,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'C', 3,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'D', 4,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'E', 5,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'F', 6,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'G', 7,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'H', 8,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'I', 9,  5);
	FILLCHARMAP(mCharMap, leftBottom, 'J', 10, 5);
	FILLCHARMAP(mCharMap, leftBottom, 'K', 11, 5);
	FILLCHARMAP(mCharMap, leftBottom, 'L', 12, 5);
	FILLCHARMAP(mCharMap, leftBottom, 'M', 13, 5);
	FILLCHARMAP(mCharMap, leftBottom, 'N', 14, 5);
	FILLCHARMAP(mCharMap, leftBottom, 'O', 15, 5);
	FILLCHARMAP(mCharMap, leftBottom, 'P', 0,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'Q', 1,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'R', 2,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'S', 3,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'T', 4,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'U', 5,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'V', 6,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'W', 7,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'X', 8,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'Y', 9,  4);
	FILLCHARMAP(mCharMap, leftBottom, 'Z', 10, 4);
	FILLCHARMAP(mCharMap, leftBottom, 'a', 1,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'b', 2,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'c', 3,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'd', 4,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'e', 5,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'f', 6,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'g', 7,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'h', 8,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'i', 9,  3);
	FILLCHARMAP(mCharMap, leftBottom, 'j', 10, 3);
	FILLCHARMAP(mCharMap, leftBottom, 'k', 11, 3);
	FILLCHARMAP(mCharMap, leftBottom, 'l', 12, 3);
	FILLCHARMAP(mCharMap, leftBottom, 'm', 13, 3);
	FILLCHARMAP(mCharMap, leftBottom, 'n', 14, 3);
	FILLCHARMAP(mCharMap, leftBottom, 'o', 15, 3);
	FILLCHARMAP(mCharMap, leftBottom, 'p', 0,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'q', 1,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'r', 2,  2);
	FILLCHARMAP(mCharMap, leftBottom, 's', 3,  2);
	FILLCHARMAP(mCharMap, leftBottom, 't', 4,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'u', 5,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'v', 6,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'w', 7,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'x', 8,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'y', 9,  2);
	FILLCHARMAP(mCharMap, leftBottom, 'z', 10, 2);

	/* symbols */
	FILLCHARMAP(mCharMap, leftBottom, '!',   1,  7);
	FILLCHARMAP(mCharMap, leftBottom, '"',   2,  7);
	FILLCHARMAP(mCharMap, leftBottom, '#',   3,  7);
	FILLCHARMAP(mCharMap, leftBottom, '$',   4,  7);
	FILLCHARMAP(mCharMap, leftBottom, '%',   5,  7);
	FILLCHARMAP(mCharMap, leftBottom, '&',   6,  7);
	FILLCHARMAP(mCharMap, leftBottom, '\'',  7,  7);
	FILLCHARMAP(mCharMap, leftBottom, '(',   8,  7);
	FILLCHARMAP(mCharMap, leftBottom, ')',   9,  7);
	FILLCHARMAP(mCharMap, leftBottom, '+',   11, 7);
	FILLCHARMAP(mCharMap, leftBottom, ',',   12, 7);
	FILLCHARMAP(mCharMap, leftBottom, '-',   13, 7);
	FILLCHARMAP(mCharMap, leftBottom, '.',   14, 7);
	FILLCHARMAP(mCharMap, leftBottom, '/',   15, 7);
	FILLCHARMAP(mCharMap, leftBottom, ':',   10, 6);
	FILLCHARMAP(mCharMap, leftBottom, ';',   11, 6);
	FILLCHARMAP(mCharMap, leftBottom, '<',   12, 6);
	FILLCHARMAP(mCharMap, leftBottom, '=',   13, 6);
	FILLCHARMAP(mCharMap, leftBottom, '>',   14, 6);
	FILLCHARMAP(mCharMap, leftBottom, '?',   15, 6);
	FILLCHARMAP(mCharMap, leftBottom, '@',   0,  5);
	FILLCHARMAP(mCharMap, leftBottom, '[',   11, 4);
	FILLCHARMAP(mCharMap, leftBottom, '\\',  12, 4);
	FILLCHARMAP(mCharMap, leftBottom, ']',   13, 4);
	FILLCHARMAP(mCharMap, leftBottom, '^',   14, 4);
	FILLCHARMAP(mCharMap, leftBottom, '_',   15, 4);
	FILLCHARMAP(mCharMap, leftBottom, '`',   0,  3);
	FILLCHARMAP(mCharMap, leftBottom, '{',   11, 2);
	FILLCHARMAP(mCharMap, leftBottom, '|',   12, 2);
	FILLCHARMAP(mCharMap, leftBottom, '}',   13, 2);
	FILLCHARMAP(mCharMap, leftBottom, '~',   14, 2);
}

} //end namespace
