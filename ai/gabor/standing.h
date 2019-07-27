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


#ifndef BOVO_STANDING_H
#define BOVO_STANDING_H

#include <QLinkedList>
#include <QVector>

#include "ai_interface.h"

// a mark on the table
typedef unsigned char mark_T;
// occurrences of a certain position
typedef unsigned char count_T;
// small index for a constant array
typedef unsigned char index_T;

// table symbols indexed with player number
const mark_T mark[] = {'o', 'x', '!', '\0'};

// the game table
typedef mark_T table_T[max_table_size][max_table_size];
// the value of the fields for the two players for the next step
typedef count_T suggest_T[2][max_table_size][max_table_size];

// heuristic type
typedef signed short heur_T;

// type for total number of marks on the table
typedef unsigned short stepCount_T;

// maximal and minimal value of the heuristic function
const heur_T MaxHeur = 10000;
const heur_T MinHeur = -MaxHeur;
// one hundredth of the maximum value
const heur_T HeurPercent = MaxHeur / 100;
// values above WinTreshold mean winning positions
const heur_T WinTreshold = 98 * HeurPercent;
// heuristic value decreases by this amount times the current depth
const heur_T depth_decay = 5;
// if the seed is too big, do not use the hash
const heur_T normal_heur_seed = 2;

// number of categories used in the heuristic function
const index_T heurLevels = 6;
// occurrence of patterns for the two players
typedef count_T PatternCount[2][heurLevels];

// a row, column or diagonal of the table
typedef QVector<mark_T> sample_T;
// interesting fields for the two players for the next step
typedef QLinkedList<Field> suggestions_T;

class Standing;
// callback function to convert a position of the sample into coordinates, and update suggestions accordingly
typedef void (Standing::*posf_T)(pos_T pos, int inv, count_T value0, count_T value1);

// game state class
class Standing {
public:
	// the current game table
	table_T table;
	// the size of the table
	pos_T table_size_x, table_size_y;
	// the heuristic value of the standing
	heur_T hval;
	// the random seed added to the heuristic value
	heur_T heur_seed;
	// true if a player has won
	bool target;
	// total number of marks on the table
	stepCount_T stepCount;
	// the player on turn
	index_T current;
	// the move that created this standing
	pos_T lastx, lasty;
	// following move suggestion values
	suggest_T suggest;

	// calculate constant values for refresh
	static void initRefresh();

	// new game
	Standing(pos_T _table_size_x, pos_T _table_size_y);
	// copy a game state
	Standing(const Standing& other) = default;
	Standing &operator=(const Standing& other) = default;


	// make a step
	void step(pos_T x, pos_T y);
	// mark a field a "wall", where neither player can step
	void step_server(pos_T x, pos_T y);

	// get the interesting fields for the two players for the next step
	void getSuggestions(suggestions_T& suggestions);
private:
	// global occurrence of patterns
	PatternCount matchCount;
	// occurrence of patterns in each row
	PatternCount matchCountRow[max_table_size];
	// occurrence of patterns in each column
	PatternCount matchCountColumn[max_table_size];
	// occurrence of patterns in each diagonal, where the sum of coordinates is constant
	PatternCount matchCountDiagonalSum[2 * max_table_size - 1];
	// occurrence of patterns in each diagonal, where the difference of coordinates is constant
	PatternCount matchCountDiagonalDiff[2 * max_table_size - 1];

	// interesting field values coming from row patterns
	suggest_T suggestRow;
	// interesting field values coming from column patterns
	suggest_T suggestColumn;
	// interesting field values coming from diagonal patterns, where the sum of coordinates is constant
	suggest_T suggestDiagonalSum;
	// interesting field values coming from diagonal patterns, where the difference of coordinates is constant
	suggest_T suggestDiagonalDiff;

	// callback function to convert a position of the row sample into coordinates, and update suggestions accordingly
	void posfRow(pos_T pos, int y, count_T value0, count_T value1);
	// callback function to convert a position of the column sample into coordinates, and update suggestions accordingly
	void posfColumn(pos_T pos, int x, count_T value0, count_T value1);
	// callback function to convert a position of the sum diagonal sample into coordinates, and update suggestions accordingly
	void posfDiagonalSum(pos_T pos, int sum, count_T value0, count_T value1);
	// callback function to convert a position of the diff diagonal sample into coordinates, and update suggestions accordingly
	void posfDiagonalDiff(pos_T pos, int diff, count_T value0, count_T value1);

	// calculate heuristic value
	void evaluate();

	// count occurrences of the patterns
	void countMatches();
	// refresh matchCount and suggest variables after a step
	// this function consumes 90% of the thinking time
	void refresh(sample_T& sample, PatternCount& local, int inv, posf_T posf);
	// process pattern occurrences
	void decide();
};

#endif // BOVO_STANDING_H
