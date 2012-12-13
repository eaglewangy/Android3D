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
 *  Created on: Nov 9, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */


#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <fstream>

#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

#define  LOG_TAG    "libandroid3d"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class Utils
{
public:
	static void readFile(std::string& fileName, std::string& content);
	static void printGLString(const char *name, GLenum s);
	static void checkGlError(const char* op);
	static void ScreenCoordsToGLCoords(int screenCoords[2], float glCoords[2]);
};


#if 0
template <typename T>
static std::string anyToString(T value)
{
	std::ostringstream ss;
	ss << value;
	return std::string(ss.str());
}

static void printMatrix(float matrix[], int logLevel)
{
	std::ostringstream ss;
	for (int i = 0; i < 16; ++i)
	{
		if (i % 4 == 0)
			ss << "\n";
		ss << matrix[i] << " ";
	}
	switch(logLevel)
	{
	case 0:
		LOGI(ss.str().c_str());
		break;
	case 1:
		LOGE(ss.str().c_str());
		break;
	default:
		LOGE("No such log level.");
		break;
	}
}
#endif

#endif


