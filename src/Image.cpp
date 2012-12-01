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

#include <algorithm>
#include <cctype>
#include <iostream>
#include <png.h>
#include <stdlib.h>

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

Image::Image(std::string fileName) :
mName(fileName),
mData(NULL),
mWidth(0),
mHeight(0),
mImageType(TYPE_NONE)
{
}

Image::~Image()
{
	free(mData);
	LOGE("Free Image...");
}

void Image::read()
{
	int pos = mName.find_last_of(".");
	std::string extension = mName.substr(pos + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
	if (extension == "png")
		mImageType = TYPE_PNG;
	else if (extension == "jpeg" || extension == "jpg")
		mImageType = TYPE_JPEG;
	else
		mImageType = TYPE_NONE;

	switch(mImageType)
	{
	case TYPE_NONE:
		LOGE("No support for this type of image.(%s)", mName.c_str());
		break;
	case TYPE_PNG:
		read_png();
		break;
	case TYPE_JPEG:
		break;
	default:
		LOGE("No support for this type of image.(%s)", mName.c_str());
		break;
	}
}

void Image::read_png()
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
	int size = (size_t)rowbytes * height * sizeof(png_byte)+15;
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
	free(row_pointers);
	fclose(fp);
}

unsigned char* Image::getData()
{
	return mData;
}

} //end namespace
