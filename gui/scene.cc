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

#include "scene.h"

#include <QTime>
#include <QTimer>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QSvgRenderer>

#include <KConfig>
#include <KConfigGroup>
#include <KDesktopFile>

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
  m_paintMarker(false), m_showLast(false) {
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
    if (m_renderer == 0)
        m_renderer = new QSvgRenderer(theme.svg());
    else
        m_renderer->load(theme.svg());
    QList<QGraphicsItem*> allMarks = items();
    foreach (QGraphicsItem* item, allMarks) {
        if (Mark* mark = qgraphicsitem_cast<Mark *>(item)) {
            mark->setFill(m_fill);
            mark->update(mark->boundingRect());
        }
        else if (HintItem* hintItem = qgraphicsitem_cast<HintItem *>(item)) {
            hintItem->setFill(m_fill);
            hintItem->update(hintItem->boundingRect());
        }
    }
}

void Scene::setTheme(const Theme& theme) {
    loadTheme(theme);
    invalidate(0, 0, width(), height());
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
    connect(m_game, &Game::boardChanged, this, &Scene::updateBoard);
    if (!m_game->demoMode()) {
        connect(m_game, &Game::playerTurn, this, &Scene::slotPlayerTurn);
        connect(m_game, &Game::oposerTurn, this, &Scene::slotOposerTurn);
    }
    connect(m_game, &Game::gameOver, this, &Scene::slotGameOver);
    connect(this, &Scene::move, m_game, &Game::move);

    qDeleteAll(items()); //remove all marks
    m_paintMarker = false;
    m_showLast = false;
    invalidate(0, 0, width(), height());
}

void Scene::updateBoard(const Move& move) {
    destroyHint();
    if (move.valid()) {
        setShowLast(move.x(), move.y());
        Mark* mark = new Mark(this, move, m_animation, m_fill);
        mark->setSharedRenderer(m_renderer);
        addItem(mark);
    } else if (move.player() == No) {
        if (!m_game->isGameOver() && !m_winningMoves.empty()) {
            m_winningMoves = QList<Move>();
            invalidate(0, 0, width(), height());
        }
        removeShowLast();
        QList<QGraphicsItem*> allMarks = items();
        foreach (QGraphicsItem* item, allMarks) {
            if (Mark* mark = qgraphicsitem_cast<Mark *>(item)) {
                if (mark->row() == move.y() && mark->col() == move.x()) {
                    if (m_animation) {
                        connect(mark, &Mark::killed, this, &Scene::killMark);
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
    m_renderer->render(p, QStringLiteral("background"));
    m_renderer->render(p, QStringLiteral("grid"), tmpRect);
}

void Scene::drawForeground(QPainter *p, const QRectF& bounds) {
    if (m_paintMarker) {
        QRectF rect(cellTopLeft(m_col, m_row), QSizeF(m_curCellSize,
                       m_curCellSize));
        qreal adjusting((1.0-m_fill)*m_curCellSize/2.0);
        rect.adjust(adjusting, adjusting, -adjusting, -adjusting);
        if (bounds.intersects(rect)) {
            p->setOpacity(0.4);
            m_renderer->render(p, QStringLiteral("x1"), rect);
            p->setOpacity(1);
        }
    }
    if (m_showLast) {
        QRectF rect(cellTopLeft(m_lastCol, m_lastRow), QSizeF(m_curCellSize,
                       m_curCellSize));
        if (bounds.intersects(rect)) {
            m_renderer->render(p, QStringLiteral("last"), rect);
        }
    }
    if (!m_winningMoves.empty()) {
        QList<Move>::const_iterator it = m_winningMoves.constBegin();
        QList<Move>::const_iterator end = m_winningMoves.constEnd();
        while (it != end) {
            QRectF tmpRect(cellTopLeft(it->x(), it->y()), QSizeF(m_curCellSize,
                           m_curCellSize));
            if (bounds.intersects(tmpRect)) {
                m_renderer->render(p, QStringLiteral("win"), tmpRect);
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
            removePaintMarker();
        } else {
            uint row = (uint)((ev->scenePos().y()-boardRect.y()) / m_curCellSize);
            uint col = (uint)((ev->scenePos().x()-boardRect.x()) / m_curCellSize);
            row = qMax(row, static_cast<uint>(0));
            row = qMin(static_cast<uint>(NUMCOLS-1), row);
            col = qMax(col, static_cast<uint>(0));
            col = qMin(static_cast<uint>(NUMCOLS-1), col);
            if (m_row != row || m_col != col || !m_paintMarker) {
                if (m_game->player(Coord(col, row)) == No) {
                    setPaintMarker(col, row);
                } else {
                    removePaintMarker();
                }
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

void Scene::removePaintMarker() {
    if (!m_paintMarker) {
        return;
    }
    m_paintMarker = false;
    QRectF rectOld(cellTopLeft(m_col, m_row), QSizeF(m_curCellSize,
                   m_curCellSize));
    qreal adjusting((1.0-m_fill)*m_curCellSize/2.0);
    rectOld.adjust(adjusting, adjusting, -adjusting, -adjusting);
    invalidate(rectOld, ForegroundLayer);
}

void Scene::setPaintMarker(uint col, uint row) {
    if (m_paintMarker == true && m_col == col && m_row == row) {
        return;
    }
    removePaintMarker();
    m_paintMarker = true;
    m_col = col;
    m_row = row;
    QRectF rect(cellTopLeft(m_col, m_row), QSizeF(m_curCellSize,
                   m_curCellSize));
    qreal adjusting((1.0-m_fill)*m_curCellSize/2.0);
    rect.adjust(adjusting, adjusting, -adjusting, -adjusting);
    invalidate(rect, ForegroundLayer);
}

void Scene::removeShowLast() {
    if (!m_showLast) {
        return;
    }
    m_showLast = false;
    QRectF rectOld(cellTopLeft(m_lastCol, m_lastRow), QSizeF(m_curCellSize,
                   m_curCellSize));
    invalidate(rectOld, ForegroundLayer);
}

void Scene::setShowLast(uint col, uint row) {
    if (m_showLast == true && m_lastCol == col && m_lastRow == row) {
        return;
    }
    removeShowLast();
    m_showLast = true;
    m_lastCol = col;
    m_lastRow = row;
    QRectF rect(cellTopLeft(m_lastCol, m_lastRow), QSizeF(m_curCellSize,
                   m_curCellSize));
    invalidate(rect, ForegroundLayer);
}

void Scene::slotPlayerTurn() {
    activate(true);
}

void Scene::slotOposerTurn() {
    removePaintMarker();
    activate(false);
}

void Scene::slotGameOver(const QList<Move>& win) {
    removePaintMarker();
    removeShowLast();
    m_winningMoves = win;
    setWin();
    activate(false);
}

void Scene::hint(const Move& hint) {
    destroyHint();
    m_hintItem = new HintItem(this, hint, m_animation, m_fill);
    m_hintItem->setSharedRenderer(m_renderer);
    addItem(m_hintItem);
    connect(m_hintTimer, &QTimer::timeout, this, &Scene::hintTimeout);
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
        connect(m_hintItem, &HintItem::killed, this, &Scene::destroyHint);
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
    connect(m_game, &Game::boardChanged, this, &Scene::updateBoard);
}

bool Scene::event(QEvent *event) {
    bool ret = QGraphicsScene::event(event);
    if (event->type() == QEvent::Leave) {
        removePaintMarker();
    }
    return ret;
}

} /* namespace gui */


