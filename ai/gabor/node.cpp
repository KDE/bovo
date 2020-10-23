/*******************************************************************
*
* Copyright 2009  Pelladi Gabor <pelladigabor@gmail.com>
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


#include "node.h"
#include "ai_impl.h"

#include <assert.h>

Node::Node(Standing* _standing, AiImpl* ai):
	standing(_standing),
	parent(NULL), child(NULL), depth(0),
	signum(_standing->current == 0 ? 1 : -1), alpha(MinHeur - 1), beta(MaxHeur + 1), is_exact(false),
	evaluated(false), depth_limit(ai->depth_limit), max_branch(ai->max_branch)
{
	generateSteps();
}

Node::Node(Standing* _standing, Node* _parent):
	standing(_standing),
	parent(_parent), child(NULL), depth(_parent->depth + 1),
	signum(-(_parent->signum)), alpha(_parent->alpha), beta(_parent->beta), is_exact(false),
	evaluated(false), depth_limit(_parent->depth_limit), max_branch(_parent->max_branch)
{
	generateSteps();
}

Node::~Node() {
	delete child;
	delete standing;
	qDeleteAll(steps);
}

void Node::generateSteps() {
	steps.clear();
	if (depth >= depth_limit || standing->target) {
		heur_T hv = standing->hval;
		heur_T decay = depth_decay * depth;
		if (hv > 0) hv -= decay; else
		if (hv < 0) hv += decay;
		if (-decay <= hv && hv <= decay) hv = 0;
		alpha = beta = hv;
		evaluated = true;
		return;
	}

	suggestions_T suggestions;
	standing->getSuggestions(suggestions);

	for (suggestions_T::iterator s = suggestions.begin(); s != suggestions.end(); ++s) {
		pos_T x = s->x;
		pos_T y = s->y;
		assert(!standing->table[x][y]);
		Standing* p = new Standing(*this->standing);
		p->step(x, y);

		heur_T pv = p->hval * signum;
		steps_T::iterator i;
		for (i = steps.begin(); i != steps.end() && signum * (*i)->hval > pv; ++i) ;
		steps.insert(i, p);

		// we only keep the best looking positions
		if ((unsigned int) steps.size() > max_branch) {
			delete steps.back();
			steps.pop_back();
		}
	}
	assert(steps.size() > 0);
}

void Node::calcHash(hash_T* hash, NodeHashData* data) {
	data->remaining_depth = depth_limit - depth;
	assert(data->remaining_depth > 0);
	
	hash_T c_hash = 0;
	for (int i = 0; i < standing->table_size_x; i++) {
		for (int j = 0; j < standing->table_size_y; j++) {
			c_hash = c_hash * 3145 + standing->table[i][j];
		}
	}
	c_hash += standing->current;
	data->checksum = c_hash;
	
	c_hash = 0;
	for (int i = 0; i < standing->table_size_x; i++) {
		for (int j = 0; j < standing->table_size_y; j++) {
			c_hash = (c_hash * 5313 + standing->table[i][j]) % nodeHashSize;
		}
	}
	c_hash += standing->current;
	*hash = c_hash;
}
