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


#include "ai_impl.h"
#include "node.h"

#include <cstdlib>
#include <memory.h>
#include <assert.h>
#include <ctime>
#include <stdio.h>

// hash table
static NodeHashData hashData[nodeHashSize];

const int hashMaxDepth = 8;
const int hashMinRemainingDepth = 2;

bool rand_inited = false;

AiImpl::AiImpl() : table_size_x(20), table_size_y(20),
	start_depth(6), max_depth(6), depth_increment(2),
	force_thinking(false), heur_seed(normal_heur_seed), print_info(true), max_branch(100),
	timeOver(NULL), rememberedStanding(table_size_x, table_size_y)
{
	if (!rand_inited) {
		rand_inited = true;
		qsrand(static_cast<unsigned int>(std::time(nullptr)));
	}
	memset(hashData, 0, sizeof(hashData));
}

AiImpl::~AiImpl() {
}

void AiImpl::newGame() {
	Standing::initRefresh();
	previousStandings.clear();
	rememberedStanding = Standing(table_size_x, table_size_y);
}

void AiImpl::step(pos_T x, pos_T y) {
	previousStandings.push_back(rememberedStanding);
	rememberedStanding.step(x, y);
	if (print_info) {
		printf("hval = %d\n", rememberedStanding.hval);
		fflush(stdout);
	}
}

void AiImpl::stepServer(pos_T x, pos_T y) {
	rememberedStanding.step_server(x, y);
}

void AiImpl::undo() {
	assert(!previousStandings.empty());
	rememberedStanding = previousStandings.last();
	previousStandings.removeLast();
}

Field AiImpl::think() {
	rememberedStanding.heur_seed = heur_seed;

	// opening book
	Field f = openingBook();
	if (f.x < table_size_x && f.y < table_size_y && !rememberedStanding.table[f.x][f.y]) {
		return f;
	}
	// global best step
	pos_T bestX = max_table_size, bestY = max_table_size;

	// alpha-beta pruning
	Node* act;
	Node* root;
	hash_T currentHash;
	NodeHashData currentHashData;

	// step suggestion in the current depth limit
	pos_T suggestedX = max_table_size, suggestedY = max_table_size;
	// heuristic value of the search tree root
	heur_T rootValue = 0.0;

	// do a very fast initial search
	depth_limit = 1;
	do {
		if (print_info) printf("  depth limit: %2d", depth_limit);

		act = root = new Node(new Standing(rememberedStanding), this);

		// this prevents the AI from thinking if there is only one good move
		if (!force_thinking && root->steps.size() == 1) {
			suggestedX = root->steps.front()->lastx;
			suggestedY = root->steps.front()->lasty;
			rootValue = 0;
			act = NULL;
		}

		while (act && (timeOver == NULL || !timeOver->isTimeOver())) {
			// if this is a parent whose child has just been evaluated
			if (act->child) {
				if (act->signum > 0) {
					// MAX
					if (act->alpha < act->child->beta) {
						act->alpha = act->child->beta;
						act->is_exact = true;
						if (!act->parent) {
							suggestedX = act->child->standing->lastx;
							suggestedY = act->child->standing->lasty;
							rootValue = act->alpha;
						}
						if (act->alpha >= act->beta || act->alpha >= WinTreshold) {
							act->evaluated = true;
							currentHashData.entry_type = lower_bound;
							currentHashData.value = act->alpha;
						}
					}
				} else {
					// MIN
					if (act->beta > act->child->alpha) {
						act->beta = act->child->alpha;
						act->is_exact = true;
						if (!act->parent) {
							suggestedX = act->child->standing->lastx;
							suggestedY = act->child->standing->lasty;
							rootValue = act->beta;
						}
						if (act->alpha >= act->beta || act->beta <= -WinTreshold) {
							act->evaluated = true;
							currentHashData.entry_type = upper_bound;
							currentHashData.value = act->beta;
						}
					}
				}

				delete act->child;
				act->child = NULL;
			}

			// if this parent has no more children to process
			if (act->steps.empty()) {
				act->evaluated = true;
				if (act->signum > 0) {
					// MAX
					currentHashData.entry_type = act->is_exact ? exact : upper_bound;
					currentHashData.value = act->alpha;
				} else {
					// MIN
					currentHashData.entry_type = act->is_exact ? exact : lower_bound;
					currentHashData.value = act->beta;
				}
			}

			if (act->evaluated) {
				// store the current standing in the hash table
				if (act->depth <= hashMaxDepth && depth_limit - act->depth >= hashMinRemainingDepth && heur_seed <= normal_heur_seed) {
					act->calcHash(&currentHash, &currentHashData);
					hashData[currentHash] = currentHashData;
				}

				act = act->parent;
			} else {
				act->child = new Node(act->steps.front(), act);
				act->steps.pop_front();
				act = act->child;

				// if this is a leaf
				if (act->evaluated) {
					act = act->parent;
					continue;
				}

				// check whether we have already evaluated the standing elsewhere
				if (act->depth <= hashMaxDepth && depth_limit - act->depth >= hashMinRemainingDepth && heur_seed <= normal_heur_seed) {
					act->calcHash(&currentHash, &currentHashData);
					NodeHashData* storedHashData = &hashData[currentHash];
					if (storedHashData->remaining_depth >= depth_limit - act->depth && currentHashData.checksum == storedHashData->checksum) {
						if (act->signum > 0) {
							// MAX
							if (storedHashData->entry_type == exact)
								act->alpha = act->beta = storedHashData->value;
							if (storedHashData->entry_type == lower_bound && act->alpha < storedHashData->value)
								act->alpha = storedHashData->value;
							if (storedHashData->entry_type == upper_bound && act->beta > storedHashData->value)
								act->beta = storedHashData->value;
						} else {
							// MIN
							if (storedHashData->entry_type == exact)
								act->alpha = act->beta = storedHashData->value;
							if (storedHashData->entry_type == upper_bound && act->beta > storedHashData->value)
								act->beta = storedHashData->value;
							if (storedHashData->entry_type == lower_bound && act->alpha < storedHashData->value)
								act->alpha = storedHashData->value;
						}
						if (act->alpha >= act->beta) {
							act->evaluated = true;
							act = act->parent;
							continue;
						}
					}
				}
			}
		}

		// check why we have left the while loop
		if (!act) {
			bestX = suggestedX;
			bestY = suggestedY;
			if (print_info) {
				printf("    suggestion: (%2d, %2d)    score: %6d\n", suggestedX, suggestedY, rootValue);
				fflush(stdout);
			}
		} else {
			if (print_info) {
				printf("    time is over\n");
				fflush(stdout);
			}
		}
		delete root;

		if (depth_limit < start_depth) {
			depth_limit = start_depth;
		} else {
			depth_limit += depth_increment;
		}
	} while (depth_limit <= max_depth && (timeOver == NULL || !timeOver->isTimeOver()));
	
	assert((timeOver == NULL || !timeOver->isTimeOver()) || !rememberedStanding.table[bestX][bestY]);
	return Field(bestX, bestY);
}

Field AiImpl::openingBook() {
	if (rememberedStanding.stepCount == 0) {
		pos_T x, y;
		x = table_size_x / 2;
		y = table_size_y / 2;
		x += qrand() % 5 - 2;
		y += qrand() % 5 - 2;
		while (rememberedStanding.table[x][y]) x++;
		return Field(x, y);
	} else if (rememberedStanding.stepCount == 1) {
		pos_T x, y;
		x = rememberedStanding.lastx;
		y = rememberedStanding.lasty;
		int r = qrand() % 100;
		if (r >= 20) {
			if (x < table_size_x / 2) {
				x++;
			} else {
				x--;
			}
		}
		if (r < 80) {
			if (y < table_size_y / 2) {
				y++;
			} else {
				y--;
			}
		}
		return Field(x, y);
	} else if (rememberedStanding.stepCount == 2) {
		pos_T x1, y1, x2, y2;
		int dx, dy;
		x1 = previousStandings.last().lastx;
		y1 = previousStandings.last().lasty;
		if (!(1 <= x1 && x1 < table_size_x - 1 && 1 <= y1 && y1 < table_size_y - 1)) {
			return Field(max_table_size, max_table_size);
		}
		x2 = rememberedStanding.lastx;
		y2 = rememberedStanding.lasty;
		dx = (int) x1 - (int) x2;
		dy = (int) y1 - (int) y2;
		if (-1 <= dx && dx <= 1 && -1 <= dy && dy <= 1) {
			if (dx == 0) {
				return Field((int) x1 + (qrand() % 2) * 2 - 1, (int) y1 + qrand() % 3 - 1);
			}
			if (dy == 0) {
				return Field((int) x1 + qrand() % 3 - 1, (int) y1 + (qrand() % 2) * 2 - 1);
			}
			if (qrand() % 2) {
				if (qrand() % 2) {
					return Field((int) x1 + dx, y1);
				} else {
					return Field(x1, (int) y1 + dy);
				}
			} else {
				if (qrand() % 2) {
					return Field((int) x1 - dx, (int) y1 + dy);
				} else {
					return Field((int) x1 + dx, (int) y1 - dy);
				}
			}
		}
	}
	return Field(max_table_size, max_table_size);
}
