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

#include "hintitem.h"

#include <QTimer>
#include <QPainter>
#include <QSvgRenderer>

#include "common.h"
#include "coord.h"
#include "move.h"
#include "scene.h"

using namespace bovo;

namespace gui {

HintItem::HintItem(Scene* scene, const Move& hint, bool animate, qreal fill)
  : QGraphicsSvgItem(), m_scene(scene), m_row(hint.y()),
  m_col(hint.x()), m_fill(fill) {
    m_sizeShrink = 1.0/(qrand()%5+7.0);
    setElementId(QString(hint.player() == X ? QStringLiteral("x%1") : QStringLiteral("o%1"))
            .arg(QString::number(qrand() % 5 + 1)));
    m_tick = 16;
    m_tickUp = true;
    m_ticker = 0;
    if (animate) {
        m_ticker = new QTimer(this);
        m_opacity = 0.0;
        connect(m_ticker, &QTimer::timeout, this, &HintItem::tick);
        m_ticker->start(30);
    } else {
        m_opacity = 0.4;
    }

    setPos(m_scene->cellCenter(m_col, m_row));
}

HintItem::~HintItem() {
    if (m_ticker) {
        disconnect(m_ticker, 0, this, 0);
        m_ticker->stop();
        m_ticker->deleteLater();
    }
}

QRectF HintItem::boundingRect() const {
    qreal width = m_scene->squareSize();
    qreal height = width;
    qreal margin = (1.0-m_fill) * width / 2.0;
    return QRectF( -width / 2.0  + margin,
                -height / 2.0 + margin,
                width  - 2.0*margin,
                height - 2.0*margin);
}


void HintItem::killAnimation() {
    if (m_ticker) {
        m_ticker->stop();
        disconnect(m_ticker, 0, this, 0);
        m_opacity = 0.4;
        update();
    }
}

void HintItem::kill() {
    connect(m_ticker, &QTimer::timeout, this, &HintItem::killTick);
    m_ticker->start();
}

void HintItem::killTick() {
    m_opacity -= 0.05;
    update();
    if (m_opacity <= 0.05) {
        m_ticker->stop();
        emit killed();
    }
}

void HintItem::tick() {
    --m_tick;
    if (m_tick == 0) {
        killAnimation();
    } else {
        if (m_tickUp && m_tick > 5) {
            m_opacity += 0.1;
        } else if (m_tickUp) {
            m_opacity -= 0.1;
            m_tickUp = false;
        } else {
            m_opacity -= 0.1;
        }
        update();
    }
}

// HintItem::setEnabled(enabled) {
//     m_enabled = enabled;
// }

void HintItem::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) {
    p->setOpacity(m_opacity);
    renderer()->render(p, elementId(), boundingRect());
}

void HintItem::setFill(qreal fill) {
    m_fill = fill;
}

} /* namespace gui */


