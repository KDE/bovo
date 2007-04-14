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

#include <QtSvg>
#include <QtGui>
#include <QtGlobal>
#include <QtCore>
#include <QTimer>
#include <QtCore/QObject>

#include <kstandarddirs.h>

#include "common.h"
#include "coord.h"
#include "game.h"
#include "hintitem.h"
#include "mark.h"
#include "move.h"
#include "theme.h"

using namespace bovo;

namespace gui {

Scene::Scene(const Theme& theme)
  : m_activate(false), m_game(0), m_player(No), m_animation(true) {
    /** @todo read theme from some configuration, I guess */
    /** @todo read file names from from some configuration, I guess */
    QString themePath = QString("themes/%1/").arg(theme.path());
    QString filename = KStandardDirs::locate("appdata", themePath);
            filename += "theme.svg";
    m_renderer = new QSvgRenderer(filename);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    m_hintTimer = new QTimer(this);
    m_hintTimer->setSingleShot(true);
    m_hintItem = 0;
    resizeScene(static_cast<int>(m_curCellSize*(NUMCOLS+2)),
                static_cast<int>(m_curCellSize*(NUMCOLS+2)));
}

Scene::~Scene() {
    m_renderer->deleteLater();
}

void Scene::setTheme(const Theme& theme) {
    QString themePath = QString("themes/%1/").arg(theme.path());
    QString filename = KStandardDirs::locate("appdata", themePath);
    filename += "theme.svg";
    m_renderer->load(filename);
    invalidate(0.0, 0.0, width()*10, height());
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

void Scene::resizeScene(int width, int height) {
    int size = qMin(width, height);
    setSceneRect( 0, 0, size, size );
    m_curCellSize = static_cast<qreal>(size) /
                    static_cast<qreal>(NUMCOLS+2);
}

void Scene::setGame(Game* game, Player player, DemoMode demoMode) {
    m_winningMoves = QList<Move>();
    m_game = game;
    m_player = player;
    connect(m_game, SIGNAL(boardChanged(const Move&)),
            SLOT(updateBoard(const Move&)));
    if (!demoMode) {
        connect(m_game, SIGNAL(playerTurn()), SLOT(slotPlayerTurn()));
        connect(m_game, SIGNAL(oposerTurn()), SLOT(slotOposerTurn()));
    }
    connect(m_game, SIGNAL(gameOver(const QList<Move>&)),
SLOT(slotGameOver(const QList<Move>&)));
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
        Mark* mark = new Mark(this, move, m_animation);
        mark->setSharedRenderer(m_renderer);
        addItem(mark);
        demandRepaint();
    } else if (move.player() == No) {
        qDebug() << "Here";
        QList<QGraphicsItem*> allMarks = items();
        QList<QGraphicsItem*>::iterator it = allMarks.begin();
        QList<QGraphicsItem*>::iterator end = allMarks.end();
        for (; it != end; ++it) {
            if (Mark* mark = qgraphicsitem_cast<Mark *>(*it)) {
                if (mark->x() == move.x() && mark->y() == move.y()) {
                    if (m_animation) {
                        connect(mark, SIGNAL(killed(Mark*)),
                                this, SLOT(killMark(Mark*)));
                        mark->kill();
                    } else {
                        removeItem(mark);
                        delete mark;
                        demandRepaint();
                        return;
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

void Scene::drawForeground(QPainter *p, const QRectF&) {
    if (m_winningMoves.empty()) {
        return;
    }
    QList<Move>::const_iterator it = m_winningMoves.begin();
    QList<Move>::const_iterator end = m_winningMoves.end();
    while (it != end) {
        QRectF tmpRect(cellTopLeft(it->x(), it->y()), QSizeF(m_curCellSize,
m_curCellSize));
        m_renderer->render(p, "win", tmpRect);
        ++it;
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
    int row = ((ev->scenePos().y()-boardRect.y()) / m_curCellSize) / 1;
    int col = ((ev->scenePos().x()-boardRect.x()) / m_curCellSize) / 1;

    if (row < 0) {
        row = 0;
    }
    if (row > NUMCOLS-1) {
        row = NUMCOLS-1;
    }
    if (col < 0) {
        col = 0;
    }
    if (col > NUMCOLS-1) {
        col = NUMCOLS-1;
    }
    emit move(Move(m_player, Coord(col, row)));
}

void Scene::slotPlayerTurn() {
    activate(true);
}

void Scene::slotOposerTurn() {
    activate(false);
}

void Scene::slotGameOver(const QList<Move>& win) {
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
    m_hintItem = new HintItem(this, hint, m_animation);
    m_hintItem->setSharedRenderer(m_renderer);
    addItem(m_hintItem);
    connect(m_hintTimer, SIGNAL(timeout()), this, SLOT(hintTimeout()));
    m_hintTimer->start(2000);
    demandRepaint();
}

void Scene::destroyHint() {
    if (m_hintItem != 0) {
        m_hintTimer->disconnect();
        m_hintTimer->stop();
        m_hintItem->killAnimation();
        removeItem(m_hintItem);
        m_hintItem->deleteLater();
        m_hintItem = 0;
        demandRepaint();
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
    demandRepaint();
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
