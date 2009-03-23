/*******************************************************************
*
* This file is part of the KDE project "Bovo"
*
* Bovo is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* Bovo is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Bovo; see the file COPYING.  If not, write to
* the Free Software Foundation, 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*
********************************************************************/


#ifndef __NODE_H__
#define __NODE_H__

#include <QLinkedList>

#include "ai_interface.h"
#include "standing.h"

// list of following steps
typedef QLinkedList<Standing*> steps_T;
// type of hash value
typedef unsigned long long hash_T;

// hash table entry type
typedef index_T entry_type_T;
enum { exact = 1, lower_bound = 2, upper_bound = 3 };

#pragma pack(push, 1)
// a hash table entry
struct NodeHashData {
	// is this data for the current position?
	hash_T checksum;
	// the result we are storing in the hash table
	heur_T value;
	// how deep we searched when we stored this result
	index_T remaining_depth;
	// type of the entry
	entry_type_T entry_type;
};
#pragma pack(pop)

// hash table maximum memory
const hash_T nodeHashMem = 16 * 1024 * 1024;
// how many entries we can store in the memory limit
const hash_T nodeHashSize = nodeHashMem / sizeof(NodeHashData);

// a node of the alphabeta tree
class Node {
public:
	// the standing this ndoe refers to
	Standing* standing;

	// parent node in the tree
	Node* parent;
	// child node in the tree
	Node* child;
	// depth of the current node
	index_T depth;

	// 1 for starting player, -1 for the other
	heur_T signum;
	// alpha value
	heur_T alpha;
	// beta value
	heur_T beta;
	// is this alpha/beta value exact, or coming from an upper level?
	bool is_exact;

	// true if this node has been fully evaluated
	bool evaluated;

	// the good steps that can be done from here
	steps_T steps;

	// how deep we can go in the tree
	int depth_limit;
	// limits the amount of following steps to be investigated
	unsigned int max_branch;

	// construct the root node
	Node(Standing* _standing, AiImpl* ai);
	// construct a child node
	Node(Standing* _standing, Node* _parent);

	// destructor
	~Node();

	// generate following steps
	void generateSteps();
	// calculate hash data
	void calcHash(hash_T* hash, NodeHashData* data);
};

#endif /* __NODE_H__ */
