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

#include <QtSvg>
#include <QtGui>
#include <QtGlobal>
#include <QtCore>

#include <kdebug.h>

#include "scene.h"
#include "game.h"
#include "mark.h"
#include "commondefs.h"
#include "winitem.h"

namespace gui {

Scene::Scene( Game* game ) : m_game(0) {
    m_bkgndRenderer = new QSvgRenderer(this);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    setBackgroundBrush( Qt::lightGray );
    resizeScene( (int)m_curCellSize*(NUMCOLS+2), (int)m_curCellSize*(NUMCOLS+2));
    setGame(game);
}

Scene::~Scene() {}

bool isAWinner(unsigned short tmpX, unsigned short tmpY, Game* game, Player player) {
  return tmpX >= 0 && 
         tmpY >= 0 && 
         tmpX < NUMCOLS && 
         tmpY < NUMCOLS && 
         game->playerAt(tmpX, tmpY) == player;
}

void Scene::setWin() {
  if (!m_game->isGameOver()) return;
  Player player = m_game->lastMove().p;
  unsigned short x = m_game->lastMove().x;
  unsigned short y = m_game->lastMove().y;
  short dy, dx;
  switch (m_game->winDir()) {
    case 0: dx = 1; dy =  0; break;
    case 1: dx = 0; dy =  1; break;
    case 2: dx = 1; dy =  1; break;
    case 3: dx = 1; dy = -1; break;
  }
  unsigned short minX = x;
  unsigned short minY = y;
  unsigned short tmpX = x + dx;
  unsigned short tmpY = y + dy;
  while (isAWinner(tmpX, tmpY, m_game, player)) {
    minX = tmpX;
    minY = tmpY;
    tmpX += dx;
    tmpY += dy;
  }
  tmpX = x - dx;
  tmpY = y - dy;
  unsigned short maxX = x;
  unsigned short maxY = y;
  while (isAWinner(tmpX, tmpY, m_game, player)) {
    maxX = tmpX;
    maxY = tmpY;
    tmpX += -1*dx;
    tmpY += -1*dy;
  }
  WinItem* item = new WinItem(this, minX, minY, maxX, maxY);
  addItem(item);
  demandRepaint();
}

void Scene::resizeScene( int width, int height ) {
    int size = qMin(width, height);
    setSceneRect( 0, 0, size, size );
    qreal scale = static_cast<qreal>(size) / m_bkgndRenderer->defaultSize().width();
    m_curCellSize = m_bkgndRenderer->defaultSize().width() * scale / (NUMCOLS+2);
}

void Scene::setGame( Game* game ) {
    m_game = game;
    connect( m_game, SIGNAL(boardChanged()), SLOT(updateBoard()) );
    connect( m_game, SIGNAL(moveFinished()), SLOT(slotGameMoveFinished()));

    QList<QGraphicsItem*> allMarks = items();
    foreach( QGraphicsItem* mark, allMarks ) {
        removeItem( mark );
        delete mark;
    }
    QList<QRectF> tmp;
    tmp.push_back(QRectF(0,0,width(),height()));
    emit changed(tmp);
}

void Scene::updateBoard() {}

QPointF Scene::cellCenter( int x, int y ) const {
    return cellTopLeft(x, y) + QPointF( m_curCellSize / 2, m_curCellSize / 2 );
}

QPointF Scene::cellTopLeft( int x, int y ) const {
    return QPointF( (x+1) * m_curCellSize, (y+1) * m_curCellSize );
}

void Scene::setBackground( const QString& bkgndPath) {
    m_bkgndRenderer->load( bkgndPath );
}

void Scene::drawBackground( QPainter *p, const QRectF&) {
    QRectF sRect = sceneRect();
    int minSize = qMin( static_cast<int>(sRect.width()), static_cast<int>(sRect.height()) );
    sRect.setWidth( minSize );
    sRect.setHeight( minSize );
    m_bkgndRenderer->render(p, sRect);
}

void Scene::mousePressEvent( QGraphicsSceneMouseEvent* ev ) {
    if (m_game->isGameOver()) return;
    if (m_game->isComputersTurn()) return;

    QRectF boardRect( cellTopLeft(0, 0), QSizeF( m_curCellSize * NUMCOLS, m_curCellSize * NUMCOLS) );
    if( !boardRect.contains(ev->scenePos()) )
        return;
    int row = static_cast<int>((-boardRect.y() + ev->scenePos().y()) / m_curCellSize);
    int col = static_cast<int>((-boardRect.x() + ev->scenePos().x()) / m_curCellSize);

    if( row < 0 ) row = 0;
    if( row > NUMCOLS-1 ) row = NUMCOLS-1;
    if( col < 0 ) col = 0;
    if( col > NUMCOLS-1 ) col = NUMCOLS-1;
    
    m_game->makePlayerMove( col, row );
}

void Scene::slotGameMoveFinished() {
    Move move = m_game->lastMove();
    Mark* mark = new Mark(move.p, this, move.x, move.y);
    addItem(mark);
    demandRepaint();
    m_game->startNextTurn();
}

void Scene::demandRepaint() {
    QList<QRectF> tmp;
    tmp.push_back(QRectF(0,0,width(),height())); // only update this square. Possible, given the fact that we have x and y :)
    emit changed(tmp);
}

void Scene::replay(const Move* moves) {
}

} //namespace gui

#include "scene.moc"
