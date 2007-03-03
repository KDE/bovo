#ifndef BOARD_H
#define BOARD_H

#include <list>

#include "square.h"
#include "dim.h"
#include "coord.h"

namespace bovo {
    
    struct outOfBounds {};
    struct gameover {};
    struct notValidPlayer {};
    
    class board {
    public:
        board(const unsigned short int width, const unsigned short int height);
        board(const dim&);
        virtual ~board();
        bool empty(const coord&) const throw(outOfBounds);
        bool setPlayer(const coord&, const unsigned short int val) throw(busy, outOfBounds, gameover, notValidPlayer);
        unsigned short int player(const coord&) const throw(outOfBounds);
        unsigned short int width() const;
        unsigned short int height() const;
        void echo() const;
        coord lastMove() const;
        bool isGameOver() const {return m_gameover;}
    private:
        dim m_dimension;
        square** m_board;
        void setup();
        bool win(const coord&) const;
        bool m_gameover;
        std::list<coord> history;
    };
}

#endif
