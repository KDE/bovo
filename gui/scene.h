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

#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QList>

class QSvgRenderer;
class QPainter;
class QTimer;

namespace gui {
  class Game;
  class Move;

  class Scene : public QGraphicsScene
  {
    Q_OBJECT

  public:
    Scene(Game* game);
    ~Scene();
    void setGame( Game* game );
    void setBackground( const QString& bkgndPath);
    void resizeScene( int width, int height );
    bool isBusy() const;
    QPointF cellCenter( int x, int y ) const;
    QPointF cellTopLeft( int x, int y ) const;
    void setWin();
    void replay(const QList<Move>& moves);

  public slots:
    void updateBoard();
    void slotGameMoveFinished();

  signals:
    void moveFinished();
    void replayFinished();

  private slots:
    void continueReplay();

  private:
    void demandRepaint();
    virtual void drawBackground( QPainter *p, const QRectF& rect );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );
    Game *m_game;
    QSvgRenderer* m_bkgndRenderer;
    QSvgRenderer* m_renderer;
    qreal m_curCellSize;
    QList<Move> m_replayMoves;
    QList<Move>::const_iterator m_replayIterator;
    QTimer* m_replayTimer;
  };
} //namespace gui

#endif //SCENE_H
