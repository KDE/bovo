/*******************************************************************
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

/**
 * @file AiBoard implementation
 */

#include <vector>
#include <iostream>

#include "aiboard.h"

#include "aisquare.h"
#include "coord.h"
#include "dim.h"

using namespace bovo;
using namespace std;

namespace ai {

AiBoard::AiBoard(const usi width, const usi height, Skill skill)
  : m_cleanBoard(true), m_dimension(width, height), m_skill(skill) {
    setup();
}

AiBoard::AiBoard(const dim& dimension, Skill skill)
  : m_cleanBoard(true), m_dimension(dimension), m_skill(skill)  {
    setup();
}

AiBoard::~AiBoard() {
    for (int x = 0; x < m_dimension.w; ++x) {
        delete[] m_board[x];
    }
    delete[] m_board;
}

bool AiBoard::empty(const coord& c) const throw(outOfBounds) {
    if (!m_dimension.ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x][c.y].empty();
}

bool AiBoard::empty(const usi x, const usi y) const throw(outOfBounds) {
    return empty(coord(x, y));
}

usi AiBoard::height() const {
    return m_dimension.h;
}

coord AiBoard::move(const coord& in) {
    if (in.x == static_cast<usi>(-1) && 
        in.y == static_cast<usi>(-1)) { //in.isNull
        m_player = 1;
        m_cleanBoard = false;
        srand(static_cast<int>(time(0)));
        usi randX = rand()%(m_dimension.w/3) + m_dimension.w/3;
        usi randY = rand()%(m_dimension.h/3) + m_dimension.h/3;
        setPlayer(coord(randX, randY), m_player);
        return coord(randX, randY);
    } else if (m_cleanBoard) {
        m_player = 2;
        m_cleanBoard = false;
        setPlayer(in, m_player%2+1);
    } else {
        setPlayer(in, m_player%2+1);
    }
    zero(in);
    for (usi x = 0; x < m_dimension.w; ++x) {
        for (usi y = 0; y < m_dimension.h; ++y) {
            if (m_board[x][y].status()) {
                coord c(x, y);
                setPoints(c, value(c, m_player));
                addPoints(c, value(c, m_player%2+1));
            }
        }
    }
    coord out = evaluate();
    setPlayer(out, m_player);
    zero(out);
    return out;
}

coord* AiBoard::moves(const coord& c) {
#warning Implement - coord* AiBoard::moves(const coord& c)
    return new coord(c.x, c.y);
}

usi AiBoard::player(const coord& c) const throw(outOfBounds) {
    if (!m_dimension.ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x][c.y].player();
}

usi AiBoard::player(const usi x, const usi y) const throw(outOfBounds) {
    return player(coord(x, y));
}

bool AiBoard::setPlayer(const coord& c, const usi player) 
  throw(busy, outOfBounds, gameover, notValidPlayer) {
    if (!m_dimension.ok(c)) {
        throw outOfBounds();
    }
    if (player != 1 && player != 2) {
        throw notValidPlayer();
    }
    if (m_gameover) {
        throw gameover();
    }
    m_board[c.x][c.y].setPlayer(player);
    if (win(c)) { 
        m_gameover = true;
        return true;
    }
    return false;
}

bool AiBoard::setPlayer(const usi x, const usi y, const usi player) 
  throw(busy, outOfBounds, gameover, notValidPlayer) {
    return setPlayer(coord(x, y), player);
}

void AiBoard::setSkill(Skill skill) {
    m_skill = skill;
}

usi AiBoard::width() const {
    return m_dimension.w;
}

/* secret helper functions */

coord next(const coord& c, usi dir) {
    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    coord tmp = c;
    if (dir & LEFT) {
        tmp = tmp.left();
    } else if (dir & RIGHT) {
        tmp = tmp.right();
    }
    if (dir & UP) {
        tmp = tmp.up();
    } else if (dir & DOWN) {
        tmp = tmp.down(); 
    }
    return tmp;
}

bool cmp(const pair<uli, coord> a, const pair<uli, coord> b) {
    return a.first > b.first;
}

/* Private methods */

coord AiBoard::evaluate() const {
    std::vector<std::pair<uli, coord> > v, v2, v3;
    for (int x = 0; x < m_dimension.w; ++x) {
        for (int y = 0; y < m_dimension.h; ++y) {
            v.push_back(make_pair(points(coord(x, y)), coord(x, y)));
        }
    }
    sort(v.begin(), v.end(), cmp);
    uli max = v.begin()->first;
    for (vector<pair<uli, coord> >::const_iterator it = v.begin(); 
      it != v.end(); ++it) {
        bool doBreak = false;
        switch (m_skill) {
            case Zlatan:
            case VeryHard:
            case Hard:
                if (it->first == max) {
                    v2.push_back(*it);
                } else {
                    doBreak = true;
                }
                break;
            case Normal:
                if (it->first * 1.2 >= max) {
                    v2.push_back(*it);
                } else {
                    random_shuffle(v2.begin(), v2.end());
                    return v2.begin()->second;
                }               
                break;
            case Easy:
                if (it->first * 2 >= max) {
                    v2.push_back(*it);
                } else {
                    random_shuffle(v2.begin(), v2.end());
                    return v2.begin()->second;
                }
                break;
            case VeryEasy:
                if (it->first * 4 >= max) {
                    v2.push_back(*it);
                } else {
                    random_shuffle(v2.begin(), v2.end());
                    return v2.begin()->second;
                }
                break;
            case RidiculouslyEasy:
            default: // in case the gui sets the level to an illegal value
                if (it->first * 7 >= max) {
                    v2.push_back(*it);
                } else {
                    random_shuffle(v2.begin(), v2.end());
                    return v2.begin()->second;
                }
                break;
        }
        if (doBreak) { 
            break;
        }
    }
    if (v2.size() == 0) {
        throw gameover();
    } else if (v2.size() == 1) {
        return v2.begin()->second;
    }
    for (vector<pair<uli, coord> >::const_iterator it = v2.begin(); 
      it != v2.end(); ++it) {
        v3.push_back(make_pair(value2(it->second), it->second));
    }
    sort(v3.begin(), v3.end(), cmp);
    if (v3.size() > 1) {
        random_shuffle(v3.begin(), v3.end());
    }
    return v3.begin()->second;
}

uli AiBoard::points(const coord& c) const throw(outOfBounds) {
    if (!m_dimension.ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x][c.y].points();
}
    
void AiBoard::addPoints(const coord& c, uli points) throw(outOfBounds) {
    if (!m_dimension.ok(c)) {
        throw outOfBounds();
    }
    m_board[c.x][c.y].setPoints(m_board[c.x][c.y].points() + points);
}

void AiBoard::setPoints(const coord& c, uli points) throw(outOfBounds) {
    if (!m_dimension.ok(c)) {
        throw outOfBounds();
    }
    m_board[c.x][c.y].setPoints(points);
    m_board[c.x][c.y].setStatus(false);
}
    
void AiBoard::setup() {
    m_gameover = false;
    m_board = new AiSquare*[m_dimension.w];
    for (int x = 0; x < m_dimension.w; ++x) {
        m_board[x] = new AiSquare[m_dimension.h];
    }
}

uli AiBoard::value(const coord& c, const usi pl) const {
    if (!empty(c)) {
        return 0;
    }
    usi oppositePlayer = (pl==1?2:1);
    usi tp;
    usi empty;
    usi await;
    usi leftsideEmpty;
    uli tmpPoint = 1;
    uli point = 0;
    bool enemy = false;
    for (int dir = 0; dir < 4; ++dir) {
        for (int i = 1; i <= 5; ++i) {
            tp = 0;
            enemy = false;
            empty = 0;
            await = 0;
            leftsideEmpty = 0;
            for (int diff = 5-i; diff > 0-i; --diff) {
                coord tmp = c;
                switch (dir) {
                    case 0:
                        tmp = coord(c.x-diff, c.y);
                        break;
                    case 1:
                        tmp = coord(c.x,      c.y-diff);
                        break;
                    case 2:
                        tmp = coord(c.x-diff, c.y-diff);
                        break;
                    case 3:
                        tmp = coord(c.x+diff, c.y-diff);
                        break;
                }
                if (m_dimension.ok(tmp)) {
                    if (player(tmp) == pl) {
                        empty += await;
                        await = 0;
                        ++tp;
                    } else if (player(tmp) == oppositePlayer) {
                        enemy = true;
                        tp = 0;
                    } else {
                        if (tp > 0) {
                            ++await;
                        } else {
                            ++leftsideEmpty;
                        }
                    }
                } else {
                    enemy = true;
                    tp = 0;
                }
                if (enemy) {
                    break;
                }
            }
            tmpPoint = 1;
            switch (tp) {
                case 4:
                    tmpPoint *= (m_skill == RidiculouslyEasy ? 7 : 231);
                case 3:
                    tmpPoint *= (m_skill == VeryEasy ? 21 : 
                        (m_skill == RidiculouslyEasy ? 12 : 231));
                case 2:
                    tmpPoint *= (m_skill == VeryEasy ? 21 : 231 );
                    break;
                case 1:
                    tmpPoint *= m_skill == RidiculouslyEasy ? 3 : 1;
                    break;
                case 0:
                    tmpPoint = 0;
            }
            if (pl == m_player && m_skill != RidiculouslyEasy 
                               && m_skill != VeryEasy) {
                tmpPoint *= 21;
            }
            if (empty < 2 && await > 0 && leftsideEmpty > 0) {
                tmpPoint *= 8;
            }
            point += tmpPoint;
        }
    }
    return point;
}

uli AiBoard::value2(const coord& c) const {
    uli p = 0;
    usi lp = 0;
    usi q = 1;
    coord tmp(c.x, c.y);
    bool test = true;
    for (usi u = 1; u < 3; ++u) {
        for (usi i = 0; i < 4; ++i) {
            while (test) {
                switch (i) {
                    case 0:
                        tmp = coord(c.x-q, c.y);
                        break;
                    case 1:
                        tmp = coord(c.x,   c.y-q);
                        break;
                    case 2:
                        tmp = coord(c.x-q, c.y-q);
                        break;
                    case 3:
                        tmp = coord(c.x+q, c.y-q);
                        break;
                }
                test = m_dimension.ok(tmp);
                if (test) {
                    test = player(tmp) == u;
                }
                if (test) {
                    ++lp;
                }
                ++q;
            }
            test = true;
            q = 1;
            while (test) {
                switch (i) {
                    case 0:
                        tmp = coord(c.x+q, c.y);
                        break;
                    case 1:
                        tmp = coord(c.x,   c.y+q);
                        break;
                    case 2:
                        tmp = coord(c.x+q, c.y+q);
                        break;
                    case 3:
                        tmp = coord(c.x-q, c.y+q);
                        break;
                }
                test = m_dimension.ok(tmp);
                if (test) {
                    test = player(tmp) == u;
                }
                if (test) {
                    ++lp;
                }
                ++q;
            }
            switch (lp) {
                case 0:
                case 1:
                    p += lp;
                    break;
                case 2:
                    p += (9*lp);
                    break;
                case 3:
                    p += (8*9*lp+1);
                    break;
                default:
                    p += 1000;
                    break;
            }
            test = true;
            q = 1;
        }
    }
    return p;
}

bool AiBoard::win(const coord& c) const {
    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    usi DIR[8] = {
          LEFT,
          RIGHT,
          UP,
          DOWN,
          LEFT | UP,
          RIGHT | DOWN,
          LEFT|DOWN,
          RIGHT|UP
    };
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
        if (count >= 5) {
            return true;
        }
    }
    return false;
}

void AiBoard::zero(const coord& c) {
    usi minX = c.x-5 < 0 ? 0 : c.x-5;
    usi maxX = c.x+5 > m_dimension.w-1 ? m_dimension.w-1 : c.x+5;
    usi minY = c.y-5 < 0 ? 0 : c.y-5;
    usi maxY = c.y+5 > m_dimension.h-1 ? m_dimension.h-1 : c.y+5;
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            m_board[x][y].setStatus(true);
        }
    }
}
    
} /* namespace ai */
