/*******************************************************************
*
* Copyright 2007  Aron Boström <c02ab@efd.lth.se>
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

#ifndef BOVO_COMMON_H
#define BOVO_COMMON_H

/** @file file containing common system wide typedefs, enums and exceptions */

/* Number of columns and rows (the same) */
#define NUMCOLS 22

/** namespace for game engine */
namespace bovo {

/* a very short positive natural number, such as a X or Y coordinate */
typedef unsigned short int usi;

/* a very long positive natural number, such as score for a certain square */
typedef unsigned long int uli;

/**
 * @brief Exception for a busy square
 * @description Exception thrown when you tries to set a player of a square
 * that is already occupied by a player ("X" or "O").
 *
 * @code
 * try {
 *     throw busy();
 * } catch (busy) {
 *     //<i> error handling code</i>
 * }
 * @endcode
 */
struct busy {
};

/**
 * @brief Exception for a coordinate outside board
 * @description Exception thrown when a trying to refer to a coordinate
 * outside of playing board.
 *
 * @code
 * try {
 *     throw outOfBounds();
 * } catch (outOfBounds) {
 *     //<i> error handling code</i>
 * }
 * @endcode
 */
struct outOfBounds {
};

/**
 * @brief Exception for Game Over
 * @description Exception thrown when a Game is already over.
 *
 * @code
 * try {
 *     throw gameover();
 * } catch (gameover) {
 *     //<i> error handling code</i>
 * }
 * @endcode
 */
struct gameover {
};

/**
 * Exception thrown when a player isn't valid (neither "X", "O" nor "No")
 *
 * @code
 * try {
 *     throw notValidPlayer();
 * } catch (notValidPlayer) {
 *     //<i> error handling code</i>
 * }
 * @endcode
 */
struct notValidPlayer {
};

/**
 * Enum for the player id
 */
enum Player {
    X = 1, /**< Player 1 */
    O = 2, /**< Player 2 */
    No = 0 /**< No player (empty) */
};

/**
 * Enum for whether a game is in demo mode or playing mode
 */
enum DemoMode {
    Demo = true,    /**< Game is a demo */
    NotDemo = false /**< Game is not a demo */
};


} /* namespace bovo */

#endif // BOVO_COMMON_H
