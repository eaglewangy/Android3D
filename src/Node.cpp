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

#include "Node.h"

using namespace android3d;

Node::Node(const std::string &name, Scene *scene) :
mName(name),
mParent(NULL),
mChild(NULL),
mNext(NULL),
mScene(scene)
{
}

Node::~Node()
{
	if(mParent)
		mParent->RemoveChild(this);
}

//Add a node
void Node::AddChild(Node *node)
{
	if(mScene != node->mScene)
	{
		return;
	}
	
	// remove the node from its previous parent
	if(node->mParent)
		node->mParent->RemoveChild(node);
	
	node->mParent = this;
	
	if(mChild == NULL)
		mChild = node;
	else
	{
		Node *next = mChild;
		while(next->GetNext())
			next = next->mNext;
	
		next->mNext = node;
	}
}

// Remove a node
void Node::RemoveChild(Node *node)
{
	if(node->mParent == NULL)
		return;
	
	if(mChild == node)
		mChild = node->mNext;
	else
	{
		Node *child = mChild, *prev = child;
		while(child)
		{
			if(child == node)
			{
				prev->mNext = node->mNext;
				break;
			}
			
			prev = child;
			child = child->mNext;
		}
	}
	
	node->mParent = NULL;
	node->mNext = NULL;
}


// Search for a node by name
Node *Node::GetNodeByName(const std::string &name)
{
	if(mName == name)
		return this;

	Node* child = mChild;
	while(child)
	{
		Node *find = child->GetNodeByName(name);
		if(find)
			return find;

		child = child->mNext;
	}

	return NULL;
}
