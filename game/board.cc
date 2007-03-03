#include <iostream>
#include <list>

#include "dim.h"
#include "coord.h"
#include "square.h"
#include "board.h"

using namespace std;

namespace bovo {

    typedef unsigned short int usi;

    board::board(const unsigned short int width, const unsigned short int height) : m_dimension(width, height) {
      setup();
    }
    
    board::board(const dim& dimension) : m_dimension(dimension) {
      setup();
    }
    
    board::~board() {
      for (int x = 0; x < m_dimension.w; ++x) {
        delete[] m_board[x];
      }
      delete[] m_board;
    }
    
    void board::setup() {
      m_gameover = false;
      m_board = new square*[m_dimension.w];
      for (int x = 0; x < m_dimension.w; ++x)
        m_board[x] = new square[m_dimension.h];
    }
    
    bool board::empty(const coord& c) const throw(outOfBounds) {
      if (!m_dimension.ok(c)) throw outOfBounds();
      return m_board[c.x][c.y].empty();
    }
    
    bool board::setPlayer(const coord& c, const unsigned short int val) throw(busy, outOfBounds, gameover, notValidPlayer) {
      if (!m_dimension.ok(c)) throw outOfBounds();
      if (val != 1 && val != 2) throw notValidPlayer();
      if (m_gameover) throw gameover();
      m_board[c.x][c.y].setPlayer(val);
      history.push_back(c);
      if (win(c)) { m_gameover = true; return true; } else return false;
    }
    
    unsigned short int board::player(const coord& c) const throw(outOfBounds) {
      if (!m_dimension.ok(c)) throw outOfBounds();
      return m_board[c.x][c.y].player();
    }
    
    unsigned short int board::width() const { return m_dimension.w; }
    unsigned short int board::height() const { return m_dimension.h; }
    
    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    coord next(const coord& c, usi dir) {
      coord tmp = c;
      if (dir & LEFT) tmp = tmp.left();
      else if (dir & RIGHT) tmp = tmp.right();
      if (dir & UP) tmp = tmp.up();
      else if (dir & DOWN) tmp = tmp.down();
      return tmp;
    }
    
    usi DIR[8] = {LEFT, RIGHT, UP, DOWN, LEFT|UP, RIGHT|DOWN, LEFT|DOWN, RIGHT|UP};
    
    bool board::win(const coord& c) const {
      usi p = player(c);
      for (int i = 0; i < 4; ++i) {
        usi count = 1;
        coord tmp = next(c, DIR[2*i]);
        while (m_dimension.ok(tmp) && player(tmp) == p) {
          ++count;
          tmp = next(tmp, DIR[2*i]);
        }
        tmp = next(c, DIR[2*i+1]);
        while (m_dimension.ok(tmp) && player(tmp) == p) {
          ++count;
          tmp = next(tmp, DIR[2*i+1]);
        }
        if (count >= 5) return true;
      }
    }

    coord board::lastMove() const {
      if (history.empty())
        return coord(-1, -1);
      return history.back();
    }
}
