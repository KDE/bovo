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

#include "hintitem.h"

#include <QtGui>
#include <QtSvg>
#include <QtGlobal>
#include <QtGui/QColor>
#include <QtCore/QTimer>

#include "common.h"
#include "coord.h"
#include "move.h"
#include "scene.h"

using namespace bovo;

namespace gui {

HintItem::HintItem(Scene* scene, const Move& hint, bool animate)
  : QGraphicsSvgItem(), m_scene(scene), m_row(hint.coord().y()),
  m_col(hint.coord().x()) {
    m_sizeShrink = 1.0/(qrand()%5+7.0);
    setElementId(QString(hint.player() == X ? "x%1" : "o%1")
            .arg(QString::number(qrand() % 5 + 1)));
    m_tick = 16;
    m_tickUp = true;
    m_animate = animate;
    m_ticker = 0;
    if (m_animate) {
        m_ticker = new QTimer(this);
        m_opacity = 0.0;
        connect(m_ticker, SIGNAL(timeout()), this, SLOT(tick()));
        m_ticker->start(30);
    } else {
        m_opacity = 0.4;
    }
}

HintItem::~HintItem() {
    if (m_ticker != 0) {
        m_ticker->stop();
        disconnect(m_ticker, 0, this, 0);
        m_ticker->deleteLater();
        m_ticker = 0;
    }
}

void HintItem::killAnimation() {
    if (m_animate) {
        m_opacity = 0.4;
        m_scene->demandRepaint();
        m_animate = false;
    }
}

void HintItem::tick() {
    m_tick--;
    if (m_tick == 0) {
        killAnimation();
    } else if (m_animate) {
        if (m_tickUp && m_tick > 5) {
            m_opacity += 0.1;
        } else if (m_tickUp) {
            m_opacity -= 0.1;
            m_tickUp = false;
        } else {
            m_opacity -= 0.1;
        }
        m_scene->demandRepaint();
    }
}

// HintItem::setEnabled(enabled) {
//     m_enabled = enabled;
// }

QRectF HintItem::glyphRectF() const {
    qreal width = m_scene->width() / (NUMCOLS+2);
    qreal height = width;
    qreal margin = m_sizeShrink * width;
    return QRectF( (1+m_col) * width  + margin,
                    (1+m_row) * height + margin,
                    width  - 2.0*margin,
                    height - 2.0*margin);
}

void HintItem::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) {
    p->setOpacity(m_opacity);
    renderer()->render(p, elementId(), glyphRectF());
}

} /* namespace gui */

#include "hintitem.moc"
