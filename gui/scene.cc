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

#include "scene.h"

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsView>
#include <QtSvg/QSvgRenderer>
#include <QtDebug>

#include <kstandarddirs.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdesktopfile.h>

#include "common.h"
#include "coord.h"
#include "game.h"
#include "hintitem.h"
#include "mark.h"
#include "move.h"
#include "theme.h"

using namespace bovo;

namespace gui {

Scene::Scene(const Theme& theme, bool animation)
  : m_activate(false), m_game(0), m_curCellSize(10.0), m_player(No), m_animation(animation),
  m_paintMarker(false) {
    /** @todo read theme from some configuration, I guess */
    /** @todo read file names from from some configuration, I guess */
    m_renderer = 0;
    loadTheme(theme);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    m_hintTimer = new QTimer(this);
    m_hintTimer->setSingleShot(true);
    m_hintItem = 0;
    setSceneRect( 0, 0, m_curCellSize*(NUMCOLS+2), m_curCellSize*(NUMCOLS+2) );
}

Scene::~Scene() {
    if (m_renderer) {
        m_renderer->deleteLater();
    }
    if (m_hintItem) {
        m_hintItem->deleteLater();
    }
    if (m_hintTimer) {
        m_hintTimer->disconnect();
        m_hintTimer->stop();
        m_hintTimer->deleteLater();
    }
}

qreal Scene::squareSize() const {
    return m_curCellSize;
}

void Scene::loadTheme(const Theme& theme) {
    m_fill = theme.fill();
    QColor color(theme.backgroundColor());
    QPalette bgPal;
    foreach (QGraphicsView* view, views()) {
        bgPal.setColor(view->backgroundRole(), color.isValid() ? color : Qt::white);
        view->setPalette(bgPal);
    }
//    qDebug() << m_fill;
    if (m_renderer == 0)
        m_renderer = new QSvgRenderer(theme.svg());
    else
        m_renderer->load(theme.svg());
    QList<QGraphicsItem*> allMarks = items();
    foreach (QGraphicsItem* item, allMarks) {
        if (Mark* mark = qgraphicsitem_cast<Mark *>(item))
            mark->setFill(m_fill);
        else if (HintItem* hintItem = qgraphicsitem_cast<HintItem *>(item)) 
            hintItem->setFill(m_fill);
    }
}

void Scene::setTheme(const Theme& theme) {
    loadTheme(theme);
    invalidate(0.0, 0.0, width()*4, height()*4);
}

void Scene::activate(bool activate) {
    m_activate = activate;
}

void Scene::setWin() {
    if (!m_game->isGameOver()) {
        return;
    }
    invalidate(0, 0, width(), height());
}

void Scene::setGame(Game* game) {
    destroyHint();
    m_winningMoves = QList<Move>();
    m_game = game;
    m_player = m_game->player();
    connect(m_game, SIGNAL(boardChanged(const Move&)),
            SLOT(updateBoard(const Move&)));
    if (!m_game->demoMode()) {
        connect(m_game, SIGNAL(playerTurn()), SLOT(slotPlayerTurn()));
        connect(m_game, SIGNAL(oposerTurn()), SLOT(slotOposerTurn()));
    }
    connect(m_game, SIGNAL(gameOver(const QList<Move>&)),
            this, SLOT(slotGameOver(const QList<Move>&)));
    connect(this, SIGNAL(move(const Move&)),
            m_game, SLOT(move(const Move&)));

    QList<QGraphicsItem*> allMarks = items();
    foreach (QGraphicsItem* mark, allMarks) {
        removeItem( mark );
        delete mark;
    }
    QList<QRectF> tmp;
    tmp.push_back(QRectF(0,0,width(),height()));
    emit changed(tmp);
}

void Scene::updateBoard(const Move& move) {
    destroyHint();
    if (move.valid()) {
        Mark* mark = new Mark(this, move, m_animation, m_fill);
        mark->setSharedRenderer(m_renderer);
        addItem(mark);
    } else if (move.player() == No) {
        QList<QGraphicsItem*> allMarks = items();
        foreach (QGraphicsItem* item, allMarks) {
            if (Mark* mark = qgraphicsitem_cast<Mark *>(item)) {
                if (mark->row() == move.x() && mark->col() == move.y()) {
                    if (m_animation) {
                        connect(mark, SIGNAL(killed(Mark*)),
                                this, SLOT(killMark(Mark*)));
                        mark->kill();
                    } else {
                        removeItem(mark);
                        delete mark;
                    }
                }
            }
        }
    }
}

QPointF Scene::cellCenter(int x, int y) const {
    return cellTopLeft(x, y) + QPointF(m_curCellSize/2.0, m_curCellSize/2.0);
}

QPointF Scene::cellTopLeft(int x, int y) const {
    return QPointF((x+1) * m_curCellSize, (y+1) * m_curCellSize);
}

void Scene::drawBackground(QPainter *p, const QRectF&) {
    QRectF sRect = sceneRect();
    int minSize = qMin(static_cast<int>(sRect.width()),
                       static_cast<int>(sRect.height()));
    qreal shrinkSize = static_cast<qreal>(NUMCOLS) /
                       static_cast<qreal>(NUMCOLS+2);
    qreal size = static_cast<qreal>(minSize) /
                 static_cast<qreal>(NUMCOLS+2);
/*    sRect.setWidth(minSize*cellSize);
    sRect.setHeight(minSize*cellSize);
    sRect.setLeft(cellSize);*/
    QRectF tmpRect(size, size, minSize*shrinkSize, minSize*shrinkSize);
    m_renderer->render(p, "background");
    m_renderer->render(p, "grid", tmpRect);
}

void Scene::drawForeground(QPainter *p, const QRectF& bounds) {
    if (m_paintMarker) {
        QRectF rect(cellTopLeft(m_col, m_row), QSizeF(m_curCellSize,
                       m_curCellSize));
        qreal adjusting((1.0-m_fill)*m_curCellSize/2.0);
        rect.adjust(adjusting, adjusting, -adjusting, -adjusting);
        if (bounds.intersects(rect)) {
            p->setOpacity(0.4);
            m_renderer->render(p, "x1", rect);
            p->setOpacity(1);
        }
    }
    if (!m_winningMoves.empty()) {
        QList<Move>::const_iterator it = m_winningMoves.constBegin();
        QList<Move>::const_iterator end = m_winningMoves.constEnd();
        while (it != end) {
            QRectF tmpRect(cellTopLeft(it->x(), it->y()), QSizeF(m_curCellSize,
                           m_curCellSize));
            if (bounds.intersects(tmpRect)) {
                m_renderer->render(p, "win", tmpRect);
            }
            ++it;
        }
    }
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *ev) {
    if (!m_game->isGameOver() && !m_game->computerTurn() && m_activate) {
        QRectF boardRect(cellTopLeft(0, 0), QSizeF(m_curCellSize * NUMCOLS,
                         m_curCellSize * NUMCOLS));
        if (!boardRect.contains(ev->scenePos())) {
            setPaintMarker(false);
        } else {
            uint row = (uint)((ev->scenePos().y()-boardRect.y()) / m_curCellSize);
            uint col = (uint)((ev->scenePos().x()-boardRect.x()) / m_curCellSize);
            row = qMax(row, static_cast<uint>(0));
            row = qMin(static_cast<uint>(NUMCOLS-1), row);
            col = qMax(col, static_cast<uint>(0));
            col = qMin(static_cast<uint>(NUMCOLS-1), col);
            if (m_row != row || m_col != col || !m_paintMarker) {
                m_col = col;
                m_row = row;
                setPaintMarker(m_game->player(Coord(col, row)) == No);
            }
        }
    }
}

void Scene::mousePressEvent( QGraphicsSceneMouseEvent* ev ) {
    if (m_game->isGameOver() || m_game->computerTurn() || !m_activate) {
        return;
    }
    QRectF boardRect(cellTopLeft(0, 0), QSizeF(m_curCellSize * NUMCOLS,
                     m_curCellSize * NUMCOLS));
    if (!boardRect.contains(ev->scenePos())) {
        return;
    }
    int row = (int)((ev->scenePos().y()-boardRect.y()) / m_curCellSize);
    int col = (int)((ev->scenePos().x()-boardRect.x()) / m_curCellSize);
    row = qMax(row, 0);
    row = qMin(NUMCOLS-1, row);
    col = qMax(col, 0);
    col = qMin(NUMCOLS-1, col);
    emit move(Move(m_player, Coord(col, row)));
}

void Scene::setPaintMarker(bool enabled) {
    if (m_paintMarker || enabled) {
        m_paintMarker = enabled;
        QRectF boardRect(cellTopLeft(0, 0), QSizeF(m_curCellSize * NUMCOLS,
                         m_curCellSize * NUMCOLS));
        // I really only want to repaint this and the old square!
        demandRepaint();
    }
    m_paintMarker = enabled;
}

void Scene::slotPlayerTurn() {
    activate(true);
}

void Scene::slotOposerTurn() {
    setPaintMarker(false);
    activate(false);
}

void Scene::slotGameOver(const QList<Move>& win) {
    setPaintMarker(false);
    m_winningMoves = win;
    setWin();
    activate(false);
}

void Scene::demandRepaint() {
    QList<QRectF> tmp;
/** @TODO only update this square. Possible,
 * given the fact that we have x and y :) */
    tmp.push_back(QRectF(0, 0, width(), height()));
    emit changed(tmp);
}

void Scene::hint(const Move& hint) {
    destroyHint();
    m_hintItem = new HintItem(this, hint, m_animation, m_fill);
    m_hintItem->setSharedRenderer(m_renderer);
    addItem(m_hintItem);
    connect(m_hintTimer, SIGNAL(timeout()), this, SLOT(hintTimeout()));
    m_hintTimer->start(2000);
}

void Scene::destroyHint() {
    if (m_hintItem != 0) {
        m_hintTimer->disconnect();
        m_hintTimer->stop();
        m_hintItem->killAnimation();
        removeItem(m_hintItem);
        m_hintItem->deleteLater();
        m_hintItem = 0;
    }
}

void Scene::hintTimeout() {
    if (!m_animation) {
        destroyHint();
    } else if (m_hintItem != 0) {
        connect(m_hintItem, SIGNAL(killed()), this, SLOT(destroyHint()));
        m_hintItem->kill();
    }
}

void Scene::enableAnimation(bool enabled) {
    m_animation = enabled;
    if (!m_animation) {
        killAnimations();
    }
}

void Scene::killAnimations() {
    if (m_hintItem != 0) {
        m_hintItem->killAnimation();
    }
}

void Scene::killMark(Mark* mark) {
    removeItem(mark);
    mark->deleteLater();
}

void Scene::replay() {
    QList<QGraphicsItem*> allMarks = items();
    foreach (QGraphicsItem* mark, allMarks) {
        removeItem(mark);
        delete mark;
    }
    m_winningMoves = QList<Move>();
    invalidate(0, 0, width(), height());
    connect(m_game, SIGNAL(boardChanged(const Move&)),
            this, SLOT(updateBoard(const Move&)));
}


} /* namespace gui */

#include "scene.moc"
