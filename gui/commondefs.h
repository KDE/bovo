#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

#include <QtGui>

namespace gui {
#define NUMCOLS 14

  enum Player { X = 1, O = 2, No = 0 };
  
  struct Move {
    Move(Player player = No, int row = -1, int col = -1) : x(row), y(col), p(player) {}
    QString toQString() { QString ret; ret = (p==X?"X: (":"O: ("); ret+=QString::number(x);ret+=", ";ret+=QString::number(y);ret+=+")"; return ret; }
    int x;
    int y;
    Player p; 
  };
} // namespace gui

#endif
