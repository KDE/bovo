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

/**
 * @file file declaring the class AiBoard, which in fact is the AI
 */

#ifndef BOVO_AIBOARD_H
#define BOVO_AIBOARD_H

#include <KgDifficulty>

#include "common.h"

using namespace bovo;

/** namespace for game engine */
namespace bovo {
    class Coord;
    class Dimension;
    class Move;
} /* namespace bovo */

/** namespace for AI stuff */
namespace ai {

class AiSquare;

/**
 * An AI player
 *
 * This class might be somewhat missnamed. It doesn't just keep track of a
 * playing board on the behalf og the AI. It is the entire AI implementation.
 * it implements two algorithms to calculate best moves and it selects which 
 * move to play. It also can tell if a move is a winning move, but it doesn't
 * track a history yet (but maybe it should?).
 *
 * It uses the standard stuff in common.h way to little.
 *
 * It is supposed to be slaughtered and split up in two, easing the
 * implementation of AdaptingAi (the great feature to come! =P )
 *
 * Perhaps we want to use Qt4 rather than STL in most of these cases. When we
 * implement AdaptingAi we have to depend on Qt4 and/or ThreadWeaver anyways.
 *
 * @code
 * Dimension dimension(width, height);
 * AiBoard ai(dimension, Easy);
 * Coord move = getMoveFromPlayerEitherByNetworkOrGui();
 * Coord aiMove = ai.move(move);
 * doSomethingWithAiMoveLikeDisplayingItInTheGui(aiMove);
 * @endcode
 */
class AiBoard {
public:
    /**
     * @brief Constructs an AiBoard with width, height and Skill
     * @description Constructs a Board object with a specified width, height and
     * skill
     * @param width the width
     * @param height the height
     * @param skill the skill (difficulty level) the AI player will be playing with
     */
    AiBoard(const usi width, const usi height,
            KgDifficultyLevel::StandardLevel skill = KgDifficultyLevel::Medium,
            Player player = O);

    /**
     * @brief Constructs an AiBoard with width, height and Skill
     * @description Constructs a Board object with a specified width and height
     * specified by a Dimension and a skill
     * @param width the width
     * @param height the height
     * @param skill the skill (difficulty level) the AI player will be playing with
     */
    explicit AiBoard(const Dimension& dimension,
                     KgDifficultyLevel::StandardLevel skill = KgDifficultyLevel::Medium,
                     Player player=O);

    /**
     * @brief destructs this AiBoard
     * @description destructs this AiBoard object
     */
    ~AiBoard();

    /**
     * @brief is a Coord empty or set?
     * @description tells whether a given Coord is marked as empty or 
     * marked by a player
     * @throw outOfBounds when coord is not on playing board
     * @param coord Coord to check
     * @return @c true if coord is empty, @c false otherwise
     */
    bool empty(const Coord&) const;

    /**
     * @brief is a Coord empty or set?
     * @description tells whether a given Coord is marked as empty or 
     * marked by a player
     * @throw outOfBounds when coord is not on playing board
     * @param x X-part of coordinate to check
     * @param y X-part of coordinate to check
     * @return @c true if coord is empty, @c false otherwise
     */
    bool empty(const usi x, const usi y) const;

    /**
     * @brief height of AiBoard
     * @description tells the number of rows in the playing board
     * @return the number of rows
     */
    usi height() const;

    /**
     * @brief get move from AI
     * @description get the move the AI wants to play
     * @return the move the AI wants to play
     */
    Coord move();

    /**
     * @brief get move from AI
     * @description Feed the latest move from the player to the AI and get a
     * list of suggested AI moves in return. This should be a QList\<Coord>.
     * @param coord the move the player played his latest turn
     * @return the moves the AI suggest to play
     * @todo Implement!
     */
    Coord* moves();

    /**
     * @brief the player occupying a Coord
     * @description tells which players occupies a certain square in the board
     * @param coord the square to check
     * @return @c X if player 1, @c O if player 2, @c No if empty
     * @throw outOfBounds if coord isn't on the playing board
     */
    Player player(const Coord&) const;

    /**
     * @brief the player occupying a Coord
     * @description tells which players occupies a certain square in the board
     * @param x the X-part of the Coord to check
     * @param y the Y-part of the Coord to check
     * @return @c X if player 1, @c O if player 2, @c No if empty
     * @throw outOfBounds if coord isn't on the playing board
     */
    Player player(const usi x, const usi y) const;

    /**
     * @brief set the player of a Coord
     * @description sets which players should occupy a certain square in the
     * playing board. Returns whether the game ends with this move (i.e. it 
     * was the winning move).
     * @param move the Move to place
     * @return @c true if this move resulted in a Game Over,
     * @c false otherwise
     * @throw busy if coord was already occupied
     * @throw gameOver if game was already over
     * @throw notValidPlayer if player wasn't X or O
     */
    bool setPlayer(const Move& move);

    /**
     * @brief change Skill
     * @description changes the Skill (difficulty level) of the AI player
     */
    void setSkill(KgDifficultyLevel::StandardLevel skill);

    /**
     * @brief width of Board
     * @description tells the number of columns in the playing board
     * @return the number of columns
     */
    usi width() const;

private:
    /* Playing board property. */
    AiSquare** m_board;

    /* hasn't game really started? */
    bool m_cleanBoard;

    /* Dimension property of playing board. */
    Dimension *m_dimension;

    /* is Game Over? property */
    bool m_gameover;

    /* AI player property */
    Player m_player;

    /* AI Level property. */
    KgDifficultyLevel::StandardLevel m_skill;

    /* Return the best-fitting coordinate according to the specs.
     * Use this when all the board has been given points. */
    Coord evaluate() const;

    /* returns, adds och sets points on a given square. */
    uli points(const Coord&) const;
    void addPoints(const Coord&, const uli points);
    void setPoints(const Coord&, const uli points);

    /* initialize this class */
    void setup();

    /* Do the real calculation of points for a given square.
     * Player is the AI player you're optimizing. */
    uli value(const Coord&, const usi player) const;

    /* value2 performs a second evaluation in order to make out the tiny 
     * differences that haven't been spotted yet. 
     * Only run this when needed. */
    uli value2(const Coord&) const;

    /* is this move (coord) a winning move? */
    bool win(const Coord&) const;

    /* Marks all neighbour (5 squares in each 
     * direction) as in need of a recalculation 
     * of its points. */
    void zero(const Coord&); 
};

} /* namespace ai */

#endif // BOVO_AIBOARD_H
