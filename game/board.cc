/*******************************************************************
*
* Copyright 2002,2007 Aron Boström <aron.bostrom@gmail.com>
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
    
    coord next(const coord& c, usi dir) {
      usi LEFT = 1;
      usi UP = 2;
      usi RIGHT = 4;
      usi DOWN = 8;
      coord tmp = c;
      if (dir & LEFT) tmp = tmp.left();
      else if (dir & RIGHT) tmp = tmp.right();
      if (dir & UP) tmp = tmp.up();
      else if (dir & DOWN) tmp = tmp.down();
      cout << "coord was (" << c.x << ", " << c.y << ") but now tmp is (" << tmp.x << ", " << tmp.y << ")" << endl;
      return tmp;
    }
    
    
    bool board::win(const coord& c) const {
      usi LEFT = 1;
      usi UP = 2;
      usi RIGHT = 4;
      usi DOWN = 8;
      usi DIR[8] = {LEFT, RIGHT, UP, DOWN, LEFT|UP, RIGHT|DOWN, LEFT|DOWN, RIGHT|UP};
      cout << "LEFT = " << left << "; ";
      cout << "UP = " << UP << "; ";
      cout << "RIGHT = " << RIGHT << "; ";
      cout << "DOWN = " << DOWN << ";" << endl;      
      for (int i = 0; i < 8; ++i)
        cout << DIR[i];
      cout << endl;
      usi p = player(c);
      for (int i = 0; i < 4; ++i) {
        cout << "Commencing counting line " << i << "..." << endl;
        usi count = 1;
        cout << "count: " << count << endl;
        coord tmp = next(c, DIR[2*i]);
        while (m_dimension.ok(tmp) && player(tmp) == p) {
          ++count;
          tmp = next(tmp, DIR[2*i]);
        }
        cout << "count: " << count << endl;
        tmp = next(c, DIR[2*i+1]);
        while (m_dimension.ok(tmp) && player(tmp) == p) {
          ++count;
          tmp = next(tmp, DIR[2*i+1]);
        }
        cout << "count: " << count << endl;
        if (count >= 5) return true;
      }
      return false;
    }

    coord board::lastMove() const {
      if (history.empty())
        return coord(-1, -1);
      return history.back();
    }
}
