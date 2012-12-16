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
 *  File:       Font.cpp
 *  Created on: Dec 15, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

#include "Font.h"
#include "Utils.h"

namespace android3d
{

Font::Font(std::string fontFile) :
mFontFile(fontFile)
{
}

bool Font::initFont(int size)
{
	FT_Library ft;
	/*initialize freetype library*/
	if(FT_Init_FreeType(&ft))
	{
		LOGE("Failed to init freetype library.");
		return false;
	}
	/*Load font face. ex: FreeSans.ttf*/
	FT_Face face;
	if(FT_New_Face(ft, mFontFile.c_str(), 0, &face))
	{
		LOGE("Failed to open %s", mFontFile.c_str());
		return false;
	}
	/*Set font size.*/
	FT_Set_Pixel_Sizes(face, 0, size);

}

void Font::drawString()
{
}

} //end namespace
