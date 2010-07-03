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


#ifndef BOVO_AI_IMPL_H
#define BOVO_AI_IMPL_H

#include <QLinkedList>

#include "ai_interface.h"
#include "standing.h"

typedef QLinkedList<Standing> previous_standings_T;

class AiImpl {
public:
	AiImpl();
	virtual ~AiImpl();

	// the width of the table
	pos_T table_size_x;
	// the height of the table
	pos_T table_size_y;
	// the starting depth for the search
	int start_depth;
	// the maximum depth for the search
	int max_depth;
	// the increment of the depth in every iteration
	int depth_increment;
	// if set, the ai will think in advance even when he has only one good move
	// prevents the ai from wasting thinking time in a competition environment
	bool force_thinking;
	// the amount of random seed added to the heuristic function in every standing
	heur_T heur_seed;
	// if set, the ai will print information on the standard output
	bool print_info;
	// limits the amount of following steps to be investigated from a standing
	unsigned int max_branch;

	// interrupt class, returns true if time is over
	AiTimeOver* timeOver;

	// current search depth limit
	int depth_limit;

	// a new game has started
	void newGame();
	// the current player made a step
	void step(pos_T x, pos_T y);
	// the server made a step
	void stepServer(pos_T x, pos_T y);
	// undo last move
	void undo();
	// suggest a move for the current player
	Field think();
private:
	// the standing persisted across steps
	Standing rememberedStanding;
	// the game history for undo
	previous_standings_T previousStandings;
	// suggest a move for the current player from the opening book
	Field openingBook();
};

#endif // BOVO_AI_IMPL_H
