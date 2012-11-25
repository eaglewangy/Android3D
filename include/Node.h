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

#ifndef NODE_H
#define NODE_H

#include "Scene.h"

#include <string>

namespace android3d
{
	class Node
	{
	public:
		Node(const std::string &name, Scene *scene);
		virtual ~Node();
	
		void AddChild(Node *child);
		void RemoveChild(Node *child);

		Node *GetNodeByName(const std::string &name);
		const std::string &GetName()			{ return mName; }

		Node *GetChild()						{ return mChild; }
		Node *GetNext()							{ return mNext; }
		Node *GetParent()						{ return mParent; }
		
		Scene *GetScene()						{ return mScene; }
		
		virtual void UpdateGraph();
	
	protected:
		std::string mName;

		Node* mParent;
		Node* mChild;
		Node* mNext;

		Scene* mScene;
	};
}

#endif
