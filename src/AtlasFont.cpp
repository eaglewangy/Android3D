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

static GLfloat gVertex[] =
{
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f,
};

static GLfloat gVertex1[] =
{
		-0.1f, 0.0f,
		0.0f, 0.0f,
		-0.1f,  0.1f,
		0.0f,  0.1f,
};

static GLfloat gVertex2[] =
{
		0.0f, 0.0f,
		0.1f, 0.0f,
		0.0f,  0.1f,
		0.1f,  0.1f,
};

/*static GLfloat gTexture[] =
{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
};*/

static GLfloat gTexture[] =
{
		4/COLUMN_COUNT, 6/ROW_COUNT,
		5/COLUMN_COUNT, 6/ROW_COUNT,
		4/COLUMN_COUNT, 7/ROW_COUNT,
		5/COLUMN_COUNT, 7/ROW_COUNT,
};
static GLfloat gTexture1[] =
{
		0/COLUMN_COUNT, 6/ROW_COUNT,
		1/COLUMN_COUNT, 6/ROW_COUNT,
		0/COLUMN_COUNT, 7/ROW_COUNT,
		1/COLUMN_COUNT, 7/ROW_COUNT,
};

//bmp file offset
#define BMP_TORASTER_OFFSET	10
#define BMP_SIZE_OFFSET		18
#define BMP_BPP_OFFSET		28
#define BMP_RLE_OFFSET		30
#define BMP_COLOR_OFFSET	54
#define fill4B(a)    (( 4 - ( (a) % 4 ) ) & 0x03)

AtlasFont::AtlasFont() :
mData(NULL),
mWidth(0),
mHeight(0),
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
mTextureVBO(0)
{
	mName = android3d::Scene::ROOT_PATH + "font.bmp";
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

void AtlasFont::initGlCmds(int x, int y, DrawAnchor anchor)
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
	/*// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);
	// Set the buffer's data
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertex), gVertex, GL_STATIC_DRAW);
	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	glGenBuffers(1, &mTextureVBO);
	/*// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mTextureVBO);
	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	glGenTextures(1, &mTextureId);
	// Set the filtering mode
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mData);

	//deleteData();

	mWidth = mHeight = 16;
	float sceneWidth = Scene::getInstance()->getWidth();
	float sceneHeight = Scene::getInstance()->getHeight();
	int screenCoord[2];
	float glCoord[2];
	glm::mat4 mModelMatrix = glm::mat4(1.0);

	screenCoord[0] = x;
	screenCoord[1] = y;
	Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
	mModelMatrix = glm::translate(mModelMatrix, glm::vec3(glCoord[0], glCoord[1], 0));

	float a = mWidth / sceneWidth;
	float b = mHeight / sceneHeight;
	//mModelMatrix = glm::scale(mModelMatrix, glm::vec3(a, b, 0));
	Scene::getInstance()->getCamera()->updateHudMVP(mWidth, mHeight);
	mHudMVPMatrix = Scene::getInstance()->getCamera()->getHudMVP() * mModelMatrix;

	mGLHasInitialized = true;
}

void AtlasFont::drawString(const char* str, int x, int y, DrawAnchor anchor)
{
	if (!mGLHasInitialized)
		initGlCmds(x, y, anchor);

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
	int screenCoord[2];
	float glCoord[2];
    for (int i = 0; i < strlen(str); ++i)
    {
    	iter = mCharMap.find(str[i]);
    	if (iter == mCharMap.end())
    		continue;
    	leftBottom = iter->second;

    	int size = 300;
    	screenCoord[0] = x;
    	screenCoord[1] = y + size;
    	Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
    	float x0 = glCoord[0];
    	float y0 = glCoord[1];

    	screenCoord[0] = x + size;
    	screenCoord[1] = y + size;
    	Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
    	float x1 = glCoord[0];
    	float y1 = glCoord[1];

    	screenCoord[0] = x;
    	screenCoord[1] = y;
    	Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
    	float x2 = glCoord[0];
    	float y2 = glCoord[1];

    	screenCoord[0] = x + size;
    	screenCoord[1] = y;
    	Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
    	float x3 = glCoord[0];
    	float y3 = glCoord[1];
    	//LOGE("%lf, %lf", x1, y1);
    	GLfloat box[] = {
    			x0, y0,
    			x1, y1,
    			x2, y2,
    			x3, y3,
    	};

    	LOGE("box: %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf", x0, y0, x1, y1, x2, y2, x3, y3);
    	GLfloat texture[] = {
    			leftBottom.first/COLUMN_COUNT,     leftBottom.second/ROW_COUNT,
    			(leftBottom.first + 1)/COLUMN_COUNT, leftBottom.second/ROW_COUNT,
    			leftBottom.first/COLUMN_COUNT,     (leftBottom.second + 1)/ROW_COUNT,
    			(leftBottom.first + 1)/COLUMN_COUNT, (leftBottom.second + 1)/ROW_COUNT,
    	};
    	LOGE("tex: %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf", texture[0], texture[1], texture[2], texture[3],
    			texture[4], texture[5], texture[6], texture[7]);
    	glVertexAttribPointer(mVetextLocation, 2, GL_FLOAT, GL_FALSE, 0, gVertex1);
    	glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, texture);
    	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
	/*glVertexAttribPointer(mVetextLocation, 2, GL_FLOAT, GL_FALSE, 0, gVertex1);
	glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, gTexture1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/
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
	char ch = '0';
	std::pair<int, int> leftBottom;
	leftBottom.first = 0;
	leftBottom.second = 6;
	mCharMap[ch] = leftBottom;
}

} //end namespace
