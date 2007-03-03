#ifndef AISQUARE_H
#define AISQUARE_H

#include "square.h"

namespace ai {
  class aisquare : public bovo::square {
  public:
    aisquare() : bovo::square(), point(0), grey(true) {}
    unsigned long int point;
    bool grey;
  };
}

#endif
