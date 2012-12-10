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

#include "Camera.h"

namespace android3d
{

Camera::Camera() :
mEyex(0.0f),
mEyey(0.0f),
mEyez(10.0f),
mCenterx(0.0f),
mCentery(0.0f),
mCenterz(0.0f),
mUpx(0.0f),
mUpy(1.0f),
mUpz(0.0f),
mMVP(NULL)
{
}

Camera::~Camera()
{
}

void Camera::setEye(float eyex, float eyey, float eyez) {
	mEyex = eyex;
	mEyey = eyey;
	mEyez = eyez;
}

void Camera::setCenter(float centerx, float centery, float centerz) {
	mCenterx = centerx;
	mCentery = centery;
	mCenterz = centerz;
}

void Camera::setUp(float upx, float upy, float upz) {
	mUpx = upx;
	mUpy = upy;
	mUpz = upz;
}

void Camera::updateMVP(int width, int height)
{
	float ratio = (float) width / height;
	float left = -ratio;
	float right = ratio;
	float bottom = -1.0f;
	float top = 1.0f;
	float near = 1.0f;
	float far = 10.0f;

	glm::mat4 projectionMatrix = glm::frustum(left, right, bottom, top, near, far);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(mEyex, mEyey, mEyez),
			glm::vec3(mCenterx, mCentery, mCenterz),
			glm::vec3(mUpx, mUpy, mUpz));
	mViewMat = viewMatrix;
	mProjectMat = projectionMatrix;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	mMVP = projectionMatrix * viewMatrix * modelMatrix;
}

void Camera::updateHudMVP(float left, float right, float bottom, float top, float near, float far)
{
	mHudMVP = glm::ortho(left, right, bottom, top, near, far) * mViewMat;
}

}


