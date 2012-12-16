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
 *  File:       Font.h
 *  Created on: Dec 15, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

#ifndef FONT_H_
#define FONT_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace android3d
{

class Font
{
public:
	Font(std::string fontFile);
	bool initFont(int size);
	void drawString();

private:
	std::string mFontFile;
}; //end Font

} //end namespace
#endif /* FONT_H_ */
