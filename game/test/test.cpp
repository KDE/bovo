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
#include <cstdlib>
#include <ctime>

#include "board.h"
#include "dim.h"

using namespace luffarschack;
using namespace std;

struct error {};

int tests = 0;
char ESC = 27;

void ok(const string& s) {
  cout << "   " << s << "... ";
  cout.setf(ios::right);
  cout << ESC << "[32m" << " [OK] " << ESC << "[0m" << endl; 
  cout.setf(ios::left);
  ++tests;
}

void fail(const string& s) {
  cout << "   " << s << "... ";
  cout.setf(ios::right);
  cout << ESC << "[31m" << " FAILED! " << ESC << "[0m" << endl; 
  cout.setf(ios::left);
}

void ass(const bool b, const string& s) throw(error) {
  if (b) { ok(s); return; } 
  else { fail(s); throw error(); }
}

void status(const string& s) {
  cout << endl << ESC << "[33m" << s << ':' << ESC << "[0m" << endl; 
}

void testWidthHeight() throw(error) {
  status("Testing width and height");
  short int WIDTH = 7;
  short int HEIGHT = 9;
  board b(WIDTH, HEIGHT);
  ass(b.width() == WIDTH, "width() for board(int, int)");
  ass(b.height() == HEIGHT, "height() for board(int, int)");
  ass(b.width() != HEIGHT, "width != height for board(int, int)");
  WIDTH = 13;
  board b2(dim(WIDTH, HEIGHT));
  ass(b2.width() == WIDTH, "width() for board(dim)");
  ass(b2.height() == HEIGHT, "height() for board(dim)");
  ass(b2.height() != WIDTH, "height != width for board(dim)");
}

void testOutsideBoundaries() throw(error) {
  status("Testing boundaries");
  short int WIDTH = 3;
  short int HEIGHT = 4;
  board b(WIDTH, HEIGHT);
  board b2(dim(WIDTH, HEIGHT));
  bool test = false;
  try {
    for (short int i = 0; i < WIDTH; ++i)
      for (short int j = 0; j < HEIGHT; ++j) {
        coord c(i,j);
        b.empty(c); b2.empty(c);
        b.player(c); b2.player(c);
      }
    test = true;
  } catch (outOfBounds) {
    test = false;
  }
  ass(test, "inside boundaries");
  test = false; try { coord c(-1, 0); b.empty(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "empty() lesser than width");
  test = false; try { coord c(0, -1); b.empty(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "empty() lesser than height");
  test = false; try { coord c(WIDTH, 0); b.empty(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "empty() too wide");
  test = false; try { coord c(0, HEIGHT); b.empty(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "empty() too high");
  test = false; try { coord c(-1, 0); b.player(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "player() lesser than width");
  test = false; try { coord c(0, -1); b.player(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "player() lesser than height");
  test = false; try { coord c(WIDTH, 0); b.player(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "player() too wide");
  test = false; try { coord c(0, HEIGHT); b.player(c); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "player() too high");
  test = false; try { coord c(-1, 0); b.setPlayer(c, 1); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "setPlayer() lesser than width");
  test = false; try { coord c(0, -1); b.setPlayer(c, 1); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "setPlayer() lesser than height");
  test = false; try { coord c(WIDTH, 0); b.setPlayer(c, 1); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "setPlayer() too wide");
  test = false; try { coord c(0, HEIGHT); b.setPlayer(c, 1); test = false; } catch (outOfBounds) { test = true; } 
  ass(test, "setPlayer() too high");
}

void testSetPlayerAndEmpty() throw(error) {
  status("Testing setPlayer(coord, unsigned short int) and checking with empty(coord) and player(coord)");
  unsigned short int WIDTH = 3;
  unsigned short int HEIGHT = 4;
  board b(WIDTH, HEIGHT);
  bool test = false;
  try{
    for (unsigned short int i = 0; i < WIDTH; ++i)
      for (unsigned short int j = 0; j < HEIGHT; ++j) {
        coord c(i,j);
        if (!b.empty(c) || b.player(c) != 0) throw error();
      }
    test = true;
  } catch (error) {
    test = false;
  }
  ass(test, "board is empty and player is 0");
  test = false;
  try {
    coord c(1,1);
    b.setPlayer(c, 1);
    for (unsigned short int i = 0; i < WIDTH; ++i)
      for (unsigned short int j = 0; j < HEIGHT; ++j) {
        if (i == 1 && j == 1) continue;
        coord c2(i,j);
        if (!b.empty(c2) || b.player(c2) != 0) throw busy();
      }
    ass(b.player(c) == 1, "setPlayer(coord, 1) -> player(coord) == 1");
    test = true;
  } catch (busy) {
    test = false;
  }
  ass(test, "board should still be empty");
  test = false; try { coord c(1, 1); b.setPlayer(c, 1); test = false; } catch (busy) { test = true; } 
  ass(test, "setPlayer(coord, 1) again should fail");
  test = false; try { coord c(1, 1); b.setPlayer(c, 2); test = false; } catch (busy) { test = true; } 
  ass(test, "setPlayer(coord, 2) should also fail");
  ass(b.player(coord(1,1)) == 1, "player(coord) should still be 1");
  test = false;
  try {
    for (unsigned short int i = 0; i < WIDTH; ++i)
      for (unsigned short int j = 0; j < HEIGHT; ++j) {
        if (i == 1 && j == 1) continue;
        coord c2(i,j);
        b.setPlayer(c2, 2);
      }
    ass(b.player(coord(0,0)) == 2, "setPlayer(coord, 2) -> player(coord) == 2");
    test = true;
  } catch (busy) {
    test = false;
  }
  ass(test, "board should be setable");
}

void testGameOver() {
  status("Testing game over");
  unsigned short int WIDTH = 29;
  unsigned short int HEIGHT = 29;
  board b(dim(WIDTH, HEIGHT));
  ass(!b.setPlayer(coord(1,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(2,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(4,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(5,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(6,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(7,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(3,1), 2), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(8,1), 2), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(9,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(10,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(11,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(13,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(14,1), 1), "not five in row shouldn't win");
  ass(!b.setPlayer(coord(15,1), 1), "not five in row shouldn't win");
  ass(b.setPlayer(coord(12,1), 1), "five in row should win");
  try {
    b.setPlayer(coord(16,1), 1);
    ass(false, "Nothing should be done when game is over");
  } catch (gameover) {
    ass(b.empty(coord(16,1)), "Nothing should be done when game is over");
  }
  board b5(dim(WIDTH, HEIGHT));
  b5.setPlayer(coord(3,2), 2);
  b5.setPlayer(coord(4,2), 2);
  b5.setPlayer(coord(5,2), 2);
  ass(!b5.setPlayer(coord(6,2), 2), "not five in row shouldn't win");
  ass(b5.setPlayer(coord(7,2), 2), "five in row SHOULD win");
  board b4(dim(WIDTH, HEIGHT));
  b4.setPlayer(coord(0,0), 2);
  b4.setPlayer(coord(1,1), 2);
  b4.setPlayer(coord(2,2), 2);
  ass(!b4.setPlayer(coord(3,3), 2), "not five in diag shouldn't win");
  ass(b4.setPlayer(coord(4,4), 2), "five in diag SHOULD win");
  board b2(dim(WIDTH, HEIGHT));
  b2.setPlayer(coord(4,0), 2);
  b2.setPlayer(coord(3,1), 2);
  b2.setPlayer(coord(2,2), 2);
  ass(!b2.setPlayer(coord(1,3), 2), "not five in other diag shouldn't win");
  ass(b2.setPlayer(coord(0,4), 2), "five in other diag SHOULD win");
  board b3(dim(WIDTH, HEIGHT));
  b3.setPlayer(coord(4,0), 2);
  b3.setPlayer(coord(4,1), 2);
  b3.setPlayer(coord(4,2), 2);
  ass(!b3.setPlayer(coord(4,3), 2), "not five in col shouldn't win");
  ass(b3.setPlayer(coord(4,4), 2), "five in col SHOULD win");
}

void testSetNonvalidPlayer() {
  status("Testing setting a non-valid player");
  board b = board(dim(1,1));
  bool test = false;
  try { b.setPlayer(coord(0,0), 0); test = false; } catch (notValidPlayer) { test = true; }
  ass(test, "player 0 (empty) should not work");
  test = false;
  try { b.setPlayer(coord(0,0), 3); test = false; } catch (notValidPlayer) { test = true; }
  ass(test, "player 3 should not work either");
}

void testEcho() {
  board b = board(dim(20, 20));
  unsigned short int player = 1;
  qsrand((unsigned)time(0));
  try {
    while (true) {
      coord c(0,0);
      while (true) {
        c = coord(qrand()%20, qrand()%20);
        if (b.empty(c)) break;
      }
      b.setPlayer(c, player);
      player = player%2+1;
    }
  } catch (busy) {
  } catch (gameover) {
    cout << endl << "Player " << player%2+1 << " won" << endl;
  }
  b.echo();
}

int main() {
  try {
    testWidthHeight();
    testOutsideBoundaries();
    testSetPlayerAndEmpty();
    testSetNonvalidPlayer();
    testGameOver();
    testEcho();
    cout << endl << ESC << "[32m" << tests << " tests passed!" << ESC << "[0m" << endl;
  } catch (error) {
    cerr << endl << ESC << "[31m" << "FEL FEL FEL!!!" << ESC << "[0m" << endl;
  }
}

