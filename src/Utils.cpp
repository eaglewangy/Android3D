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
 *  Created on: Dec 3, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

#include "Utils.h"
#include "Scene.h"

void Utils::readFile(std::string& fileName, std::string& content)
{
	std::ifstream ifs(fileName.c_str());
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	content = buffer.str();
	ifs.close();
}

void Utils::printGLString(const char *name, GLenum s)
{
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGL ES %s = %s\n", name, v);
}

void Utils::checkGlError(const char* op)
{
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

void Utils::ScreenCoordsToGLCoords(int screenCoords[2], float glCoords[2])
{
	/*gl width = screen current width coord / scene width * 2 - 1; */
	glCoords[0] = (float)screenCoords[0]/(float)android3d::Scene::getInstance()->getWidth() * 2 - 1;
	glCoords[1] = 1 - (float)screenCoords[1]/(float)android3d::Scene::getInstance()->getHeight() * 2;
	//LOGE("%lf, %lf", glCoords[0], glCoords[1]);
}
