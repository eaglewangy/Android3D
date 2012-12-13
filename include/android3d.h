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
 *  Created on: Nov 16, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */


#ifndef ANDROID3D_H_
#define ANDROID3D_H_

#include "Camera.h"
#include "Utils.h"

#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

namespace android3d
{

#ifdef __cplusplus
extern "C" {
#endif

#define ANDROID3D_EPSILON 1.0e-6

#define FREEANDNULL(pointer) \
		if (pointer != NULL) { \
			free(pointer); \
			pointer = NULL; \
		}

#define DELETEANDNULL(pointer, isArray) \
		if (pointer != NULL) { \
			if (isArray) \
			delete [] pointer; \
			else \
			delete pointer; \
			pointer = NULL; \
		}

enum DrawAnchor {
	NONE = 0,
    /* draw the image from screen top & left */
    TOP_LEFT,
    /* draw the image from screen bottom & left, opengl es default use it */
    BOTTOM_LEFT,
    /* draw the image from screen top & right */
    TOP_RIGHT,
    /* draw the image from screen bottom & right */
    BOTTOM_RIGHT,
    /* draw the image from screen center */
    CENTER,
};

#ifdef __cplusplus
}
#endif //

}//end namespace

#endif /* ANDROID3D_H_ */
