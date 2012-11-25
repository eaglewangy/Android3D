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


#ifndef SHADERMANAGER_H_
#define SHADERMANAGER_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace android3d
{

class ShaderManager
{
public:
	static GLuint createProgram(GLint& vetexShader, const char* vertexSource,
			GLint& fragment, const char* fragmentSource);
private:
	static GLuint loadShader(GLenum shaderType, const char* source);
};

}//end namespace



#endif /* SHADERMANAGER_H_ */
