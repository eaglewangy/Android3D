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


#include <cstdlib>
#include "ShaderManager.h"
#include "Utils.h"
#include "Scene.h"

namespace android3d
{
std::string ShaderManager::ROOT_PATH = Scene::ROOT_PATH;

ShaderManager::ShaderManager(std::string vertexSource, std::string framentSource, bool fromFile) :
mVetexShader(0),
mFragmentShader(0),
mProgram(0)
{
	if (fromFile)
	{
		std::string vertexFile = ROOT_PATH + vertexSource;
		std::string fragmentFile = ROOT_PATH + framentSource;

		std::string vertexShader, fragmentShader;
		Utils::readFile(vertexFile, vertexShader);
		Utils::readFile(fragmentFile, fragmentShader);
		mProgram = createProgram(vertexShader.c_str(), fragmentShader.c_str());
	}
	else
	{
		mProgram = createProgram(vertexSource.c_str(), framentSource.c_str());
	}

}

ShaderManager::~ShaderManager()
{
	glDeleteShader(mVetexShader);
	glDeleteShader(mFragmentShader);
	glDetachShader(mProgram, mVetexShader);
	glDetachShader(mProgram, mFragmentShader);
	glDeleteProgram(mProgram);

	mVetexShader = 0;
	mFragmentShader = 0;
	mProgram = 0;
}

GLuint ShaderManager::loadShader(GLenum shaderType, const char* source)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader) {
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d:\n%s\n",
							shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}

	return shader;
}

GLuint ShaderManager::createProgram(const char* vertexSource, const char* fragmentSource)
{
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
	mVetexShader = vertexShader;
	if (!vertexShader) {
		return 0;
	}

	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
	if (!fragmentShader) {
		return 0;
	}
	mFragmentShader = fragmentShader;

	GLuint program = glCreateProgram();
	if (program) {
		glAttachShader(program, vertexShader);
		Utils::checkGlError("glAttachShader");
		glAttachShader(program, fragmentShader);
		Utils::checkGlError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = new char[bufLength];
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOGE("Could not link program:\n%s\n", buf);
					delete[] buf;
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	//LOGI("Vertext shader: %d, fragment shader: %d, program: %d", vertexShader, mFragmentShader, program);
	return program;
}

}//end namespace





