#ifndef COORD_H
#define COORD_H

namespace bovo {
  /** 
   * Coordinate
   */
  class coord {
  public:
    coord(const unsigned short int col, const unsigned short int row) : x(col), y(row) {}
    coord(const coord& c) : x(c.x), y(c.y) {}
    unsigned short int x, y;
    unsigned short int row() const { return x; }
    unsigned short int col() const { return y; }
    coord left() const { return coord(x-1, y); }
    coord right() const { return coord(x+1, y); }
    coord leftup() const { return coord(x-1, y-1); }
    coord leftdown() const { return coord(x-1, y+1); }
    coord rightup() const { return coord(x+1, y-1); }
    coord rightdown() const { return coord(x+1, y+1); }
    coord up() const { return coord(x, y-1); }
    coord down() const { return coord(x, y+1); }
  };
}

#endif
