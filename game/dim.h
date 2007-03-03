#ifndef DIM_H
#define DIM_H

#include "coord.h"

namespace bovo {
  /** 
   * Dimension
   */
  class dim {
  public:
    dim(const unsigned short int width, const unsigned int height) : w(width), h(height) {}
    dim(const dim& dimension) : w(dimension.w), h(dimension.h) {}
    unsigned short int w, h;
    unsigned short int cols() const { return w; }
    unsigned short int width() const { return w; }
    unsigned short int rows() const { return h; }
    unsigned short int height() const { return h; }
    bool ok(const coord* c) const { return c->x < w && c->y < h; }
    bool ok(const coord& c) const { return c.x < w && c.y < h; }
  };
}

#endif
