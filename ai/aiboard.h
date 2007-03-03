#ifndef AIBOARD_H
#define AIBOARD_H

#include "ai.h"

#include "aisquare.h"
#include "square.h"
#include "dim.h"
#include "coord.h"

using namespace bovo;

namespace ai {
  class aiboard {
  public:
    explicit aiboard(const usi width, const usi height) : d(width, height), cleanBoard(true) { setup(); } 
    explicit aiboard(const dim& dimension) : d(dimension), cleanBoard(true) { setup(); }
    ~aiboard();
    bool empty(const coord&) const throw(outOfBounds);
    bool empty(const usi x, const usi y) const throw(outOfBounds) { return empty(coord(x, y)); }
    bool setPlayer(const coord&, const usi) throw(busy, outOfBounds, gameover, notValidPlayer);
    bool setPlayer(const usi x, const usi y, const usi p) throw(busy, outOfBounds, gameover, notValidPlayer) { return setPlayer(coord(x, y), p); }
    usi player(const coord&) const throw(outOfBounds);
    usi player(const usi x, const usi y) const throw(outOfBounds) { return player(coord(x, y)); }
    usi width() const { return d.w; }
    usi height() const { return d.h; }
    coord move(const coord& c);
  private:
    void zero(const coord&);
    uli points(const coord&) const throw(outOfBounds);
    void setPoints(const coord&, const uli points) throw(outOfBounds);
    void addPoints(const coord&, const uli points) throw(outOfBounds);
    uli value(const coord&, const usi player) const;
    uli value2(const coord&) const;
    coord evaluate() const;
    dim d;
    aisquare** b;
    void setup();
    bool win(const coord&) const;
    bool m_gameover;
    bool cleanBoard;
    usi m_player;
  };
}

#endif
