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

Image::Image(std::string fileName) :
mName(fileName),
mData(NULL),
mWidth(0),
mHeight(0),
mImageType(TYPE_NONE),
mHasAlpha(true)
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
		read_jpeg();
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

#if 1
void Image::read_jpeg()
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
	FILE * infile;		/* source file */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	unsigned char* image;

	/* In this example we want to open the input file before doing anything else,
	 * so that the setjmp() error recovery below can assume the file is open.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to read binary files.
	 */

	if ((infile = fopen(mName.c_str(), "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", mName.c_str());
		return;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
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

	/* In this example, we don't need to change any of the defaults set by
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
	 * In this example, we need to make an output work buffer of the right size.
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
		/* Assume put_scanline_someplace wants a pointer and sample count. */
		//put_scanline_someplace(buffer[0], row_stride);
		for(int i= 0; i < row_stride; i++)
		    image[index++] = buffer[0][i];
	}
	mData = image;

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

	/* And we're done! */
}
#endif

unsigned char* Image::getData()
{
	return mData;
}

} //end namespace
