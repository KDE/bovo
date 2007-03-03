#include <iostream>
#include <vector>

#include "aiboard.h"

#include "aisquare.h"
#include "coord.h"
#include "dim.h"

using namespace bovo;
using namespace std;

namespace ai {
    aiboard::~aiboard() {
      for (int x = 0; x < d.w; ++x) {
        delete[] b[x];
      }
      delete[] b;
    }
    
    void aiboard::setup() {
      m_gameover = false;
      b = new aisquare*[d.w];
      for (int x = 0; x < d.w; ++x)
        b[x] = new aisquare[d.h];
    }
    
    bool aiboard::empty(const coord& c) const throw(outOfBounds) {
      if (!d.ok(c)) throw outOfBounds();
      return b[c.x][c.y].empty();
    }
    
    bool aiboard::setPlayer(const coord& c, const usi val) throw(busy, outOfBounds, gameover, notValidPlayer) {
      if (!d.ok(c)) throw outOfBounds();
      if (val != 1 && val != 2) throw notValidPlayer();
      if (m_gameover) throw gameover();
      b[c.x][c.y].setPlayer(val);
      if (win(c)) { m_gameover = true; return true; } else return false;
    }
    
    usi aiboard::player(const coord& c) const throw(outOfBounds) {
      if (!d.ok(c)) throw outOfBounds();
      return b[c.x][c.y].player();
    }
    
    uli aiboard::points(const coord& c) const throw(outOfBounds) {
      if (!d.ok(c)) throw outOfBounds();
      return b[c.x][c.y].point;
    }
    
    void aiboard::setPoints(const coord& c, uli p) throw(outOfBounds) {
      if (!d.ok(c)) throw outOfBounds();
      b[c.x][c.y].point = p;
      b[c.x][c.y].grey = false;
    }
    
    void aiboard::addPoints(const coord& c, uli p) throw(outOfBounds) {
      if (!d.ok(c)) throw outOfBounds();
      b[c.x][c.y].point += p;
    }

    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    usi DIR[8] = {LEFT, RIGHT, UP, DOWN, LEFT|UP, RIGHT|DOWN, LEFT|DOWN, RIGHT|UP};
    coord next(const coord& c, usi dir) {
      coord tmp = c;
      if (dir & LEFT) tmp = tmp.left();
      else if (dir & RIGHT) tmp = tmp.right();
      if (dir & UP) tmp = tmp.up();
      else if (dir & DOWN) tmp = tmp.down(); 
      return tmp;
    }
    
    bool aiboard::win(const coord& c) const {
      usi p = player(c);
      for (int i = 0; i < 4; ++i) {
        usi count = 1;
        coord tmp = next(c, DIR[2*i]);
        while (d.ok(tmp) && player(tmp) == p) {
          ++count;
          tmp = next(tmp, DIR[2*i]);
        }
        tmp = next(c, DIR[2*i+1]);
        while (d.ok(tmp) && player(tmp) == p) {
          ++count;
          tmp = next(tmp, DIR[2*i+1]);
        }
        if (count >= 5) return true;
      }
    }

    /** IDÉ: ta in en koordinat som parameter och uppdatera bara de koordinater runtikkring den (4-5 i var riktning)
      *	Då får vi också reda på vilket id vår motståndare har (player på koordinaten vi får...), 
      *	men då måste vi även uppdatera de två senaste ändringarna från spelbrädet till AI-brädet. 
      *	Alltså måste vi ta in två koordinater, vår senate och motståndarens senaste och uppdatera hela områdena runt dem.
      *	Och kolla så att ingen koordinat är (-1, -1), eller till och med bägge. Och räkna ut player i så fall. 
      */
    coord aiboard::move(const coord& in) {
      if (in.x == static_cast<unsigned short int>(-1) && in.y == static_cast<unsigned short int>(-1)) {
        m_player = 1;
        cleanBoard = false;
        srand(static_cast<int>(time(0)));
        usi randX = rand()%(d.w/3) + d.w/3;
        usi randY = rand()%(d.h/3) + d.h/3;
        setPlayer(coord(randX, randY), m_player);
        return coord(randX, randY);
      } else if (cleanBoard) {
        m_player = 2;
        cleanBoard = false;
        setPlayer(in, m_player%2+1);
      } else {
        setPlayer(in, m_player%2+1);
      }
      zero(in);
      for (usi x = 0; x < d.w; ++x)
        for (usi y = 0; y < d.h; ++y) {
          if (b[x][y].grey) {
            coord c(x, y);
            setPoints(c, value(c, m_player));
            addPoints(c, value(c, m_player%2+1));
          }
        }
      coord out = evaluate();
      setPlayer(out, m_player);
      zero(out);
      return out;
    }

    bool cmp(const pair<uli, coord> a, const pair<uli, coord> b) {
      return a.first > b.first;
    }

    /**
      *	Iterera genom brädet, lägg till alla värden som är minst 90 % av MAX i en vektor. Kontinuerligt uppdatera MAX. Qsortera och ta ut maximum. 
      *	Vid flera maximum i listan, kom på något. Förmodligen är det i början på spelet då, så man ska prioritera rutor bredvid motståndare och centrala positioner.
      */
    coord aiboard::evaluate() const {
      std::vector<std::pair<uli, coord> > v, v2, v3;
      uli p = 0;
      for (int x = 0; x < d.w; ++x)
        for (int y = 0; y < d.h; ++y)
          v.push_back(make_pair(points(coord(x, y)), coord(x, y)));
      sort(v.begin(), v.end(), cmp);
      uli max = v.begin()->first;
      for (vector<pair<uli, coord> >::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (it->first == max)
          v2.push_back(*it);
      }
      if (v2.size() == 0) {
        cerr << static_cast<char>(27) << "[31m" << " *** FELFELFEL!!! ***    v2 är tom " << static_cast<char>(27) << "[0m" << endl;
        throw gameover();
      } else if (v2.size() == 1)
        return v2.begin()->second;
      for (vector<pair<uli, coord> >::const_iterator it = v2.begin(); it != v2.end(); ++it)
        v3.push_back(make_pair(value2(it->second), it->second));
      sort(v3.begin(), v3.end(), cmp);
      if (v3.size() > 1)
        random_shuffle(v3.begin(), v3.end());
      return v3.begin()->second;
    }

    uli aiboard::value2(const coord& c) const {
      uli p = 0;
      usi lp = 0;
      usi q = 1;
      coord tmp(c.x, c.y);
      bool test = true;
      for (usi u = 1; u < 3; ++u) {
        for (usi i = 0; i < 4; ++i) {
          while (test) {
            switch (i) {
              case 0: tmp = coord(c.x-q, c.y); break;
              case 1: tmp = coord(c.x,   c.y-q); break;
              case 2: tmp = coord(c.x-q, c.y-q); break;
              case 3: tmp = coord(c.x+q, c.y-q); break;
            }
            test = d.ok(tmp);
            if (test) test = player(tmp) == u;
            if (test) ++lp;
            ++q;
          }
          test = true; q = 1;
          while (test) {
            switch (i) {
              case 0: tmp = coord(c.x+q, c.y); break;
              case 1: tmp = coord(c.x,   c.y+q); break;
              case 2: tmp = coord(c.x+q, c.y+q); break;
              case 3: tmp = coord(c.x-q, c.y+q); break;
            }
            test = d.ok(tmp);
            if (test) test = player(tmp) == u;
            if (test) ++lp;
            ++q;
          }
          switch (lp) {
            case 0: case 1: p+=lp; break;
            case 2: p+=(9*lp); break;
            case 3: p+=(8*9*lp+1); break;
            default: p+= 1000; break;
          }
          test = true; q = 1;
        }
      }
      return p;
    }

    /** IDÉ: sätt till grey istf. poäng=0. När man sätter poäng avsätts alltid grey till white. Enkelt att kolla i traverseringen om en ruta behövs revalueras. */    
    void aiboard::zero(const coord& c) {
      usi minx = c.x-5 < 0 ? 0 : c.x-5;
      usi maxx = c.x+5 > d.w-1 ? d.w-1 : c.x+5;
      usi miny = c.y-5 < 0 ? 0 : c.y-5;
      usi maxy = c.y+5 > d.h-1 ? d.h-1 : c.y+5;
      for (int x = minx; x <= maxx; ++x)
        for (int y = miny; y <= maxy; ++y)
          b[x][y].grey = true;
//          setPoints(coord(x, y), 0);
    }
    
    uli aiboard::value(const coord& c, const usi pl) const {
      if (!empty(c)) return 0;
      usi oppositePlayer = (pl==1?2:1);
      usi tp, empty, avvakta, vanstertom;
      uli tmpPoint = 1;
      uli point = 0;
      bool enemy = false;
      for (int dir = 0; dir < 4; ++dir) {
        for (int i = 1; i <= 5; ++i) {
          tp = 0;
          enemy = false;
          empty = 0;
          avvakta = 0;
          vanstertom = 0;
          for (int diff = 5-i; diff > 0-i; --diff) {
            coord tmp = c;
            switch (dir) {
              case 0: tmp = coord(c.x-diff, c.y);      break;
              case 1: tmp = coord(c.x,      c.y-diff); break;
              case 2: tmp = coord(c.x-diff, c.y-diff); break;
              case 3: tmp = coord(c.x+diff, c.y-diff); break;
            }
            if (d.ok(tmp)) {
              if (player(tmp) == pl) {
                empty += avvakta;
                avvakta = 0;
                ++tp;
              } else if (player(tmp) == oppositePlayer) {
                enemy = true;
                tp = 0;
              } else {
                if (tp > 0) ++avvakta;
                else ++vanstertom;
              }
            } else {
              enemy = true;
              tp = 0;
            }
            if (enemy) break;
          }
          tmpPoint = 1;
          switch (tp) {
            case 4: tmpPoint *= 231;
            case 3: tmpPoint *= 231;
            case 2: tmpPoint *= 231; break;
            case 0: tmpPoint = 0;
          }
          if (pl == m_player) tmpPoint *= 21;
          if (empty < 2 && avvakta > 0 && vanstertom > 0) tmpPoint *= 8;
          point += tmpPoint;
        }
      }
      return point;
    }

}
