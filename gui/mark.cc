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

#include "mark.h"

#include <QTimer>
#include <QPainter>
#include <QSvgRenderer>

#include "common.h"
#include "coord.h"
#include "move.h"
#include "scene.h"

using namespace bovo;

namespace gui {

Mark::Mark(Scene* scene, const Move& move, bool animate, qreal fill) : QGraphicsSvgItem(),
  m_scene(scene), m_row(move.y()), m_col(move.x()), m_fill(fill) {
    m_sizeShrink = 1.0/12.0; //1.0/(qrand()%5+7.0);
    setElementId(QString(move.player() == X ? QStringLiteral("x%1") : QStringLiteral("o%1"))
            .arg(QString::number(qrand() % 5 + 1)));
    m_tick = 20;
    m_ticker = 0;
    if (animate) {
        m_ticker = new QTimer(this);
        m_opacity = 0.0;
        connect(m_ticker, &QTimer::timeout, this, &Mark::tick);
        m_ticker->start(30);
    } else {
        m_opacity = 1.0;
    }

    setPos(m_scene->cellCenter(m_col, m_row));
}

Mark::~Mark() {
    if (m_ticker) {
        disconnect(m_ticker, &QTimer::timeout, this, &Mark::tick);
        m_ticker->stop();
        m_ticker->deleteLater();
    }
}

QRectF Mark::boundingRect() const {
    qreal width = m_scene->squareSize();
    qreal height = width;
    qreal margin = (1.0-m_fill) * width / 2.0;
    return QRectF( -width / 2.0  + margin,
                -height / 2.0 + margin,
                width  - 2.0*margin,
                height - 2.0*margin);
}

void Mark::killAnimation() {
    if (m_ticker != 0) {
        m_ticker->stop();
        disconnect(m_ticker, 0, this, 0);
        m_ticker->deleteLater();
        m_ticker = 0;
        m_opacity = 1.0;
        update();
    }
}

void Mark::kill() {
    if (m_ticker) {
        disconnect(m_ticker, &QTimer::timeout, this, &Mark::tick);
        m_ticker->stop();
    } else {
        m_ticker = new QTimer(this);
    }
    connect(m_ticker, &QTimer::timeout, this, &Mark::killTick);
    m_ticker->start(30);
}

void Mark::killTick() {
    m_opacity -= 0.1;
    update();
    if (m_opacity <= 0.1) {
        m_ticker->stop();
        emit killed(this);
    }
}

void Mark::tick() {
    --m_tick;
    if (m_tick == 0) {
        killAnimation();
    } else {
        m_opacity += 0.1;
        update();
    }
}

void Mark::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) {
    p->setOpacity(m_opacity);
    renderer()->render(p, elementId(), boundingRect());
}

void Mark::setFill(qreal fill) {
    if (m_fill != fill) {
        m_fill = fill;
        prepareGeometryChange();
    }
}

usi Mark::col() const {
    return m_col;
}

usi Mark::row() const {
    return m_row;
}

} /* namespace gui */


