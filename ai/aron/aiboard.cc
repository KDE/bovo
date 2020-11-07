/*******************************************************************
*
* Copyright 2007  Aron Boström <c02ab@efd.lth.se>
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

#include "aiboard.h"

#include <ctime>

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include "aisquare.h"
#include "coord.h"
#include "dimension.h"
#include "move.h"

using namespace bovo;
using namespace std;

namespace ai {

AiBoard::AiBoard(const usi width, const usi height,
                 KgDifficultyLevel::StandardLevel skill, Player player)
  : m_cleanBoard(true), m_player(player), m_skill(skill) {
    m_dimension = new Dimension(width, height);
    setup();
}

AiBoard::AiBoard(const Dimension& dimension,
                 KgDifficultyLevel::StandardLevel skill, Player player) 
  : m_cleanBoard(true), m_player(player), m_skill(skill) {
    m_dimension = new Dimension(dimension.width(), dimension.height());
    setup();
}

AiBoard::~AiBoard() {
    for (int x = 0; x < m_dimension->width(); ++x) {
        delete[] m_board[x];
    }
    delete[] m_board;
    delete m_dimension;
}

bool AiBoard::empty(const Coord& c) const {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x()][c.y()].empty();
}

bool AiBoard::empty(const usi x, const usi y) const {
    return empty(Coord(x, y));
}

usi AiBoard::height() const {
    return m_dimension->height();
}

Coord AiBoard::move() {
    if (m_cleanBoard) {
        qsrand(static_cast<int>(std::time(nullptr)));
        usi randX = qrand()%(m_dimension->width()/3) + m_dimension->width()/3;
        usi randY = qrand()%(m_dimension->height()/3) + m_dimension->height()/3;
        return Coord(randX, randY);
    }
    for (usi x = 0; x < m_dimension->width(); ++x) {
        for (usi y = 0; y < m_dimension->height(); ++y) {
            if (m_board[x][y].status()) {
                Coord c(x, y);
                setPoints(c, value(c, m_player));
                addPoints(c, value(c, m_player == X ? O : X));
            }
        }
    }
    Coord out = evaluate();
    return out;
}

Coord* AiBoard::moves() {
#ifdef __GNUC__
#warning Implement - Coord* AiBoard::moves(const Coord& c)
#endif
    return new Coord();
}

Player AiBoard::player(const Coord& c) const {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x()][c.y()].player();
}

Player AiBoard::player(const usi x, const usi y) const {
    return player(Coord(x, y));
}

bool AiBoard::setPlayer(const Move& move) {
    m_cleanBoard = false;
    zero(move.coord());
    m_board[move.coord().x()][move.coord().y()].setPlayer(move.player());
    if (win(move.coord())) {
        m_gameover = true;
        return true;
    }
    return false;
}

void AiBoard::setSkill(KgDifficultyLevel::StandardLevel skill) {
    m_skill = skill;
}

usi AiBoard::width() const {
    return m_dimension->width();
}

/* secret helper functions */

Coord next(const Coord& c, usi dir) {
    const usi LEFT = 1;
    const usi UP = 2;
    const usi RIGHT = 4;
    const usi DOWN = 8;
    Coord tmp = c;
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

bool cmp(const pair<uli, Coord> &a, const pair<uli, Coord> &b) {
    return a.first > b.first;
}

/* Private methods */

Coord AiBoard::evaluate() const {
    std::vector<std::pair<uli, Coord> > v, v2, v3;
    for (int x = 0; x < m_dimension->width(); ++x) {
        for (int y = 0; y < m_dimension->height(); ++y) {
            v.push_back(make_pair(points(Coord(x, y)), Coord(x, y)));
        }
    }
    sort(v.begin(), v.end(), cmp);
    uli max = v.begin()->first;
    for (vector<pair<uli, Coord> >::const_iterator it = v.begin(); 
      it != v.end(); ++it) {
        bool doBreak = false;
        switch (m_skill) {
            case KgDifficultyLevel::Impossible: // @TODO: Implement Impossible
            case KgDifficultyLevel::VeryHard: //@TODO: Implement Very Hard
            case KgDifficultyLevel::Hard:
                if (it->first == max) {
                    v2.push_back(*it);
                } else {
                    doBreak = true;
                }
                break;
            case KgDifficultyLevel::Medium:
                if (it->first * 1.2 >= max) {
                    v2.push_back(*it);
                } else {
                    random_shuffle(v2.begin(), v2.end());
                    return v2.begin()->second;
                }
                break;
            case KgDifficultyLevel::Easy:
                if (it->first * 2 >= max) {
                    v2.push_back(*it);
                } else {
                    random_shuffle(v2.begin(), v2.end());
                    return v2.begin()->second;
                }
                break;
            case KgDifficultyLevel::VeryEasy:
                if (it->first * 4 >= max) {
                    v2.push_back(*it);
                } else {
                    random_shuffle(v2.begin(), v2.end());
                    return v2.begin()->second;
                }
                break;
            case KgDifficultyLevel::RidiculouslyEasy:
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
    if (v2.empty()) {
        throw gameover();
    } else if (v2.size() == 1) {
        return v2.begin()->second;
    }
    for (vector<pair<uli, Coord> >::const_iterator it = v2.begin(); 
      it != v2.end(); ++it) {
        v3.push_back(make_pair(value2(it->second), it->second));
    }
    sort(v3.begin(), v3.end(), cmp);
    if (v3.size() > 1) {
        random_shuffle(v3.begin(), v3.end());
    }
    return v3.begin()->second;
}

uli AiBoard::points(const Coord& c) const {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x()][c.y()].points();
}
    
void AiBoard::addPoints(const Coord& c, uli points) {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    m_board[c.x()][c.y()].setPoints(m_board[c.x()][c.y()].points() + points);
}

void AiBoard::setPoints(const Coord& c, uli points) {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    m_board[c.x()][c.y()].setPoints(points);
    m_board[c.x()][c.y()].setStatus(false);
}
    
void AiBoard::setup() {
    m_gameover = false;
    m_board = new AiSquare*[m_dimension->width()];
    for (int x = 0; x < m_dimension->width(); ++x) {
        m_board[x] = new AiSquare[m_dimension->height()];
    }
}

uli AiBoard::value(const Coord& c, const usi pl) const {
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
                Coord tmp = c;
                switch (dir) {
                    case 0:
                        tmp = Coord(c.x()-diff, c.y());
                        break;
                    case 1:
                        tmp = Coord(c.x(),      c.y()-diff);
                        break;
                    case 2:
                        tmp = Coord(c.x()-diff, c.y()-diff);
                        break;
                    case 3:
                        tmp = Coord(c.x()+diff, c.y()-diff);
                        break;
                }
                if (m_dimension->ok(tmp)) {
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
                    tmpPoint *= (m_skill == KgDifficultyLevel::RidiculouslyEasy ? 7 : 231);
                case 3:
                    tmpPoint *= (m_skill == KgDifficultyLevel::VeryEasy ? 21 : 
                        (m_skill == KgDifficultyLevel::RidiculouslyEasy ? 12 : 231));
                case 2:
                    tmpPoint *= (m_skill == KgDifficultyLevel::VeryEasy ? 21 : 231 );
                    break;
                case 1:
                    tmpPoint *= m_skill == KgDifficultyLevel::RidiculouslyEasy ? 3 : 1;
                    break;
                case 0:
                    tmpPoint = 0;
            }
            if (pl == m_player && m_skill != KgDifficultyLevel::RidiculouslyEasy 
                               && m_skill != KgDifficultyLevel::VeryEasy) {
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

uli AiBoard::value2(const Coord& c) const {
    uli p = 0;
    usi lp = 0;
    usi q = 1;
    Coord tmp(c.x(), c.y());
    bool test = true;
    for (usi u = 1; u < 3; ++u) {
        for (usi i = 0; i < 4; ++i) {
            while (test) {
                switch (i) {
                    case 0:
                        tmp = Coord(c.x()-q, c.y());
                        break;
                    case 1:
                        tmp = Coord(c.x(),   c.y()-q);
                        break;
                    case 2:
                        tmp = Coord(c.x()-q, c.y()-q);
                        break;
                    case 3:
                        tmp = Coord(c.x()+q, c.y()-q);
                        break;
                }
                test = m_dimension->ok(tmp);
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
                        tmp = Coord(c.x()+q, c.y());
                        break;
                    case 1:
                        tmp = Coord(c.x(),   c.y()+q);
                        break;
                    case 2:
                        tmp = Coord(c.x()+q, c.y()+q);
                        break;
                    case 3:
                        tmp = Coord(c.x()-q, c.y()+q);
                        break;
                }
                test = m_dimension->ok(tmp);
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

bool AiBoard::win(const Coord& c) const {
    const usi LEFT = 1;
    const usi UP = 2;
    const usi RIGHT = 4;
    const usi DOWN = 8;
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
        Coord tmp = next(c, DIR[2*i]);
        while (m_dimension->ok(tmp) && player(tmp) == p) {
            ++count;
            tmp = next(tmp, DIR[2*i]);
        }
        tmp = next(c, DIR[2*i+1]);
        while (m_dimension->ok(tmp) && player(tmp) == p) {
            ++count;
            tmp = next(tmp, DIR[2*i+1]);
        }
        if (count >= 5) {
            return true;
        }
    }
    return false;
}

void AiBoard::zero(const Coord& c) {
    usi minX = c.x()-5 < 0 ? 0 : c.x()-5;
    usi maxX = (c.x()+5 > m_dimension->width()-1) ? (m_dimension->width()-1) : c.x()+5;
    usi minY = c.y()-5 < 0 ? 0 : c.y()-5;
    usi maxY = (c.y()+5 > m_dimension->height()-1) ? (m_dimension->height()-1) : c.y()+5;
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            m_board[x][y].setStatus(true);
        }
    }
}
    
} /* namespace ai */
