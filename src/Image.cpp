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

#include "Image.h"
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
static char tolower(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
	{
		return std::tolower(ch);
	}
	return ch;
}

struct android3d_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct android3d_error_mgr* jpeg_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */
METHODDEF(void)
jpeg_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	jpeg_error_ptr err = (jpeg_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(err->setjmp_buffer, 1);
}

GLfloat gVertex[] =
{
		-1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f,
};

GLushort gVertexIndex[] =
{
		0, 1, 2,
		0, 2, 3
};

GLfloat gTexture[] =
{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
};

//bmp file offset
#define BMP_TORASTER_OFFSET	10
#define BMP_SIZE_OFFSET		18
#define BMP_BPP_OFFSET		28
#define BMP_RLE_OFFSET		30
#define BMP_COLOR_OFFSET	54
#define fill4B(a)    (( 4 - ( (a) % 4 ) ) & 0x03)

Image::Image(std::string fileName) :
mName(fileName),
mData(NULL),
mWidth(0),
mHeight(0),
mImageType(TYPE_NONE),
mPixelFormat(0),
mHasAlpha(true),
mGLHasInitialized(false),
mHudMVPMatrixLocation(-1),
mVetextLocation(-1),
mShaderManager(NULL),
mTextureId(-1),
mTextureVBO(0)
{
	memset(mVertexVBO, 0, sizeof(mVertexVBO));
	loadTexture(mName);
}

Image::~Image()
{
	FREEANDNULL(mData);
	glDeleteBuffers(2, mVertexVBO);
	memset(mVertexVBO, 0, sizeof(mVertexVBO));
	DELETEANDNULL(mShaderManager, false);
	LOGE("Free Image...");
}

void Image::deleteData()
{
	FREEANDNULL(mData);
}

void Image::loadTexture(std::string fileName)
{
	int pos = fileName.find_last_of(".");
	std::string extension = fileName.substr(pos + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
	if (extension == "png")
		mImageType = TYPE_PNG;
	else if (extension == "jpeg" || extension == "jpg")
		mImageType = TYPE_JPEG;
	else if (extension == "bmp")
		mImageType = TYPE_BMP;
	else
		mImageType = TYPE_NONE;

	switch(mImageType)
	{
	case TYPE_NONE:
		LOGE("No support for this type of image.(%s)", fileName.c_str());
		break;
	case TYPE_PNG:
		load_png();
		break;
	case TYPE_JPEG:
		load_jpeg();
		break;
	case TYPE_BMP:
		load_bmp();
		break;
	default:
		LOGE("No support for this type of image.(%s)", fileName.c_str());
		break;
	}
}

void Image::initGlCmds(int x, int y, DrawAnchor anchor)
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
	glGenBuffers(2, mVertexVBO);
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[0]);
	// Set the buffer's data
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertex), gVertex, GL_STATIC_DRAW);
	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gVertexIndex), gVertexIndex, GL_STATIC_DRAW);
	// Unbind the VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &mTextureVBO);
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mTextureVBO);
	// Set the buffer's data
	glBufferData(GL_ARRAY_BUFFER, sizeof(gTexture), gTexture, GL_STATIC_DRAW);
	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenTextures(1, &mTextureId);
	// Set the filtering mode
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Bind the texture
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	if (mHasAlpha)
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mData);

	deleteData();

	float sceneWidth = Scene::getInstance()->getWidth();
	float sceneHeight = Scene::getInstance()->getHeight();
	float a = mWidth / sceneWidth;
	float b = mHeight / sceneHeight;
	int screenCoord[2];
	float glCoord[2];
	glm::mat4 mModelMatrix = glm::mat4(1.0);
	switch(anchor)
	{
	case TOP_LEFT:
		screenCoord[0] = mWidth/2;
		screenCoord[1] = mHeight/2;
		Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
		mModelMatrix = glm::translate(mModelMatrix, glm::vec3(glCoord[0], glCoord[1], 0));
		break;
	case BOTTOM_LEFT:
		screenCoord[0] = mWidth/2;
		screenCoord[1] = sceneHeight - mHeight/2;
		Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
		mModelMatrix = glm::translate(mModelMatrix, glm::vec3(glCoord[0], glCoord[1], 0));
		break;
	case TOP_RIGHT:
		screenCoord[0] = sceneWidth - mWidth/2;
		screenCoord[1] = mHeight/2;
		Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
		mModelMatrix = glm::translate(mModelMatrix, glm::vec3(glCoord[0], glCoord[1], 0));
		break;
	case BOTTOM_RIGHT:
		screenCoord[0] = sceneWidth - mWidth/2;
		screenCoord[1] = sceneHeight - mHeight/2;
		Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
		mModelMatrix = glm::translate(mModelMatrix, glm::vec3(glCoord[0], glCoord[1], 0));
		break;
	case CENTER:
		break;
	default:
		screenCoord[0] = x;
		screenCoord[1] = y;
		Utils::ScreenCoordsToGLCoords(screenCoord, glCoord);
		mModelMatrix = glm::translate(mModelMatrix, glm::vec3(glCoord[0], glCoord[1], 0));
		break;
	}
	mModelMatrix = glm::scale(mModelMatrix, glm::vec3(a, b, 0));
	Scene::getInstance()->getCamera()->updateHudMVP(mWidth, mHeight);
	mHudMVPMatrix = Scene::getInstance()->getCamera()->getHudMVP() * mModelMatrix;

	mGLHasInitialized = true;
}

void Image::drawImage(int x, int y, DrawAnchor anchor)
{
	if (!mGLHasInitialized)
		initGlCmds(x, y, anchor);

	glDisable(GL_CULL_FACE);

	glUseProgram(mShaderManager->getProgram());
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[0]);
	glVertexAttribPointer(mVetextLocation, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(mVetextLocation);

	glBindTexture(GL_TEXTURE_2D, mTextureId);
	// Set the sampler texture unit to 0
	glUniform1i(mSamplerLocation, 0);
	glBindBuffer(GL_ARRAY_BUFFER, mTextureVBO);

	glVertexAttribPointer(mTextureLocation, 2, GL_FLOAT, false, 0, NULL);
	glEnableVertexAttribArray(mTextureLocation);

	glUniformMatrix4fv(mHudMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mHudMVPMatrix));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexVBO[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	// Unbind the VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Image::load_png()
{
	png_byte header[8];
	png_uint_32 width = 0;
	png_uint_32 height = 0;

	FILE *fp = fopen(mName.c_str(), "rb");
	if (fp == 0)
	{
		LOGE("Failed to open %s to read.", mName.c_str());
		return;
	}

	// read the header
	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
	{
		LOGE("%s is not recognized as a PNG file.", mName.c_str());
		fclose(fp);
		return;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		LOGE("png_create_read_struct failed.");
		fclose(fp);
		return;
	}

	// create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		LOGE("png_create_info_struct failed.");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return;
	}

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		LOGE("Error during init_io in read_png");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		return;
	}

	// the code in this if statement gets called if libpng encounters an error
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		LOGE("Error during read_image in read_png");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return;
	}

	// init png reading
	png_init_io(png_ptr, fp);

	// let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// variables to pass to get info
	int bit_depth, color_type;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			NULL, NULL, NULL);

	if (width)
		mWidth = width;
	if (height)
		mHeight = height;

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes-1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte* image_data;
	int size = (size_t)rowbytes * height * sizeof(png_byte) + 15;
	image_data = (png_byte*)malloc(size);
	if (image_data == NULL)
	{
		LOGE("Error: could not allocate memory for PNG image data.");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return;
	}

	// row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep* row_pointers = (png_bytep*)malloc(height * sizeof(png_bytep));
	if (row_pointers == NULL)
	{
		LOGE("error: could not allocate memory for PNG row pointers.");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		free(image_data);
		fclose(fp);
		return;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	int i;
	for (i = 0; i < height; i++)
	{
		row_pointers[height - 1 - i] = image_data + i * rowbytes;
	}

	// read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	// Generate the OpenGL texture object
	/*GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/

	// clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

	mData = image_data;
	mPixelFormat = GL_RGBA;
	free(row_pointers);
	fclose(fp);
}

void Image::load_jpeg()
{
	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	struct android3d_error_mgr jerr;
	/* More stuff */
	FILE* infile;		/* source file */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	unsigned char* image;

	/* Open the input file before doing anything else,
	 * so that the setjmp() error recovery below can assume the file is open.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to read binary files.
	 */

	if ((infile = fopen(mName.c_str(), "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", mName.c_str());
		return;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* Set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = jpeg_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.doc for more info.
	 */

	/* Step 4: set parameters for decompression */

	/*we don't need to change any of the defaults set by
	 * jpeg_read_header(), so we do nothing here.
	 */

	/* Step 5: Start decompressor */

	(void) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* We may need to do some setup of our own at this point before reading
	 * the data.  After jpeg_start_decompress() we have the correct scaled
	 * output image dimensions available, as well as the output colormap
	 * if we asked for color quantization.
	 * we need to make an output work buffer of the right size.
	 */
	/* JSAMPLEs per row in output buffer */
	mWidth = cinfo.output_width;
	mHeight = cinfo.output_height;
	unsigned int depth = cinfo.num_components; //should always be 3
	mHasAlpha = false;
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
					((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	image = (unsigned char *) malloc(mWidth * mHeight * depth);
	unsigned long index = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		for(int i= 0; i < row_stride; i++)
		    image[index++] = buffer[0][i];
	}
	flipBufferByVertical(image, row_stride, cinfo.output_height);
	mData = image;
	mPixelFormat = GL_RGB;

	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit, but why assume anything...)
	 */
	fclose(infile);

	/* At this point you may want to check to see whether any corrupt-data
	 * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	 */
}

void Image::load_bmp()
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

void Image::fetchPallete(FILE *fd, Color pallete[], int count)
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

unsigned char* Image::getData()
{
	return mData;
}

bool Image::flipBufferByVertical(unsigned char* input, int widthBytes, int height)
{
	unsigned char* tb1;
	unsigned char* tb2;

	if (input == NULL)
		return false;

	int size = widthBytes;

	tb1 = (unsigned char*)new unsigned char[size];
	if (tb1 == NULL)
		return false;

	tb2 = (unsigned char*)new unsigned char[size];
	if (tb2 == NULL) {
		delete[] tb1;
		return false;
	}

	int rowCount = -1;
	long offset1 = 0;
	long offset2 = 0;

	for (rowCount = 0; rowCount < (height + 1)/2; rowCount++) {
		offset1 = rowCount * size;
		offset2 = ((height-1) - rowCount) * size;

		memcpy(tb1,input + offset1, size);
		memcpy(tb2,input + offset2, size);
		memcpy(input + offset1, tb2, size);
		memcpy(input + offset2, tb1, size);
	}

	delete[] tb1;
	delete[] tb2;
	return true;
}

} //end namespace
