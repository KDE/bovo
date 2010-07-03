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


#ifndef BOVO_AI_INTERFACE_H
#define BOVO_AI_INTERFACE_H

// a coordinate of the table
typedef unsigned char pos_T;
// memory allocated for a standing
const pos_T max_table_size = 22;

// a pair of coordinates
struct Field {
	pos_T x, y;
	Field() : x(0), y(0) {}
	Field(pos_T x0, pos_T y0) : x(x0), y(y0) {}
	bool operator==(const Field& field) {return x == field.x && y == field.y;}
};

class AiInterface;
class AiImpl;

class AiTimeOver {
public:
    virtual ~AiTimeOver() {}
    virtual bool isTimeOver() = 0;
};

class AiInterface {
public:
	AiInterface();
	virtual ~AiInterface();

	// set a square table with the specified size
	void setTableSize(pos_T tableSize);
	// set the width of the table
	void setTableSizeX(pos_T tableSizeX);
	// set the height of the table
	void setTableSizeY(pos_T tableSizeY);

	// set a fixed depth for the search
	void setDepth(int depth);
	// set the starting depth for the search
	void setStartDepth(int startDepth);
	// set the maximum depth for the search
	void setMaxDepth(int startDepth);
	// set the increment of the depth in every iteration
	void setDepthIncrement(int startDepth);
	// if set, the ai will think in advance even when he has only one good move
	// prevents the ai from wasting thinking time in a competition environment
	void setForceThinking(bool forceThinking);
	// the amount of random seed added to the heuristic function in every standing
	void setRandomAmount(int randomAmount);
	// if set, the ai will print information on the standard output
	void setPrintInfo(bool printInfo);
	// interrupt function, returns true if time is over
	void setTimeOver(AiTimeOver* timeOver);
	
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
	// implementation class
	AiImpl* aiImpl;
};

#endif // BOVO_AI_INTERFACE_H
