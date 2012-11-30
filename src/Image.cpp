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

Image::Image() :
mData(NULL),
mWidth(0),
mHeight(0),
mImageType(TYPE_NONE)
{
}

Image::~Image()
{
	delete[] mData;
}

void Image::read(std::string fileName)
{
	int pos = fileName.find_last_of(".");
	std::string extension = fileName.substr(pos + 1);
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
		LOGE("No support for this type of image.(%s)", fileName.c_str());
		break;
	case TYPE_PNG:
		read_png(fileName);
		break;
	case TYPE_JPEG:
		break;
	default:
		LOGE("No support for this type of image.(%s)", fileName.c_str());
		break;
	}
}

void Image::read_png(std::string fileName)
{
	/*8 is the maximum size that can be checked.*/
	png_byte header[8];

	/* open file and test for it being a png */
	FILE *fp = fopen(fileName.c_str(), "rb");
	if (!fp)
	{
		LOGE("Failed to open %s to read.", fileName.c_str());
		return;
	}
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		LOGE("%s is not recognized as a PNG file.", fileName.c_str());
		return;
	}

	/* initialize stuff */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		LOGE("png_create_read_struct failed.");
		return;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		LOGE("png_create_info_struct failed.");
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		LOGE("Error during init_io in read_png");
		return;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	int width = png_get_image_width(png_ptr, info_ptr);
	int height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	int number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	 //read file
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		LOGE("Error during read_image in read_png");
		return;
	}

	int x = 0, y = 0;
	png_bytep* row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	//mData = (png_bytep*) malloc(sizeof(png_bytep) * height * );
	for (y = 0; y < height; y++)
		row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

	png_read_image(png_ptr, row_pointers);

	fclose(fp);

	/* Begin process png. */
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
	{
		LOGE("%s is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA (lacks the alpha channel)", fileName.c_str());
		return;
	}

	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
	{
		LOGE("color_type of %s must be PNG_COLOR_TYPE_RGBA (%d) (is %d)", fileName.c_str(), PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
		return;
	}

	mData = new GLuint[height * width];
	int k = 0;
	for (y = 0; y < height; y++)
	{
		png_byte* row = row_pointers[y];
		for (x = 0; x < width; x++)
		{
			png_byte* ptr = &(row[x*4]);
			/*LOGE("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
					x, y, ptr[0], ptr[1], ptr[2], ptr[3]);*/

			GLuint pixel = ((ptr[0]&0x0ff) << 24) +
					       ((ptr[1]&0x0ff) << 16) +
					       ((ptr[2]&0x0ff) << 8) +
					       (ptr[3]&0x0ff);

			mData[k] = pixel;
			++k;
			/* set red value to 0 and green value to the blue one */
			//ptr[0] = 0;
			//ptr[1] = ptr[2];
		}
	}

	for (y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);
}

GLuint* Image::getData()
{
	return mData;
}

} //end namespace
