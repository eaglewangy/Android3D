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
 *  Created on: Nov 14, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp> //vec3, vec4, ivec4, mat4
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective
#include <glm/gtc/type_ptr.hpp> //value_ptr

namespace android3d
{

class Camera
{
public:
	Camera();

	virtual ~Camera();

	void setEye(float eyex, float eyey, float eyez);
	void setCenter(float centerx, float centery, float centerz);
	void setUp(float upx, float upy, float upz);
	glm::mat4& getMVP() {return mMVP;}
	void updateMVP(int width, int height);

private:
	float mEyex;
	float mEyey;
	float mEyez;
	float mCenterx;
	float mCentery;
	float mCenterz;
	float mUpx;
	float mUpy;
	float mUpz;

	glm::mat4 mMVP;

};

}//end namespace


#endif /* CAMERA_H_ */
