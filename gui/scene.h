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

#ifndef __SCENE_H__
#define __SCENE_H__

#include <QGraphicsScene>
#include <QList>

#include "common.h"

class QSvgRenderer;
class QPainter;
class QTimer;

namespace bovo {
    class Move;
    class Game;
}

using namespace bovo;

namespace gui {
    class HintItem;

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    Scene(const QString& theme);
    ~Scene();
    void activate(bool activate);
    void setGame(Game* game, Player player = X, DemoMode demoMode = NotDemo);
    void resizeScene( int width, int height );
    bool isBusy() const;
    void replay(const QList<Move>& moves);
    void setTheme(const QString& theme);

public slots:
    void updateBoard(const Move& move);
    void slotPlayerTurn();
    void slotOposerTurn();
    void slotGameOver(const QList<Move>& winningMoves);
    void hint(const Move& hint);
    void hintTimeout();
    void destroyHint();
    void enableAnimation(bool enabled);
    void demandRepaint();
    void setWin();
    void replay();

signals:
    void move(const Move&);
    void replayFinished();

private slots:
    void continueReplay();

private:
    virtual void drawBackground( QPainter *p, const QRectF& rect );
    virtual void drawForeground( QPainter *p, const QRectF& rect );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );
    QPointF cellCenter( int x, int y ) const;
    QPointF cellTopLeft( int x, int y ) const;
    void killAnimations();
    bool m_activate;
    Game *m_game;
    QSvgRenderer* m_bkgndRenderer;
    QSvgRenderer* m_renderer;
    qreal m_curCellSize;
    QList<Move>::const_iterator m_replayEnd;
    QList<Move>::const_iterator m_replayIterator;
    QTimer *m_replayTimer;
    QTimer *m_hintTimer;
//    int m_hintCounter;
    HintItem *m_hintItem;
    Player m_player;
    bool m_animation;
    QList<Move> m_winningMoves;
};

} /* namespace gui */

#endif /* __SCENE_H__ */
