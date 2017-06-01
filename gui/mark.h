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

#ifndef BOVO_MARK_H
#define BOVO_MARK_H

#include <QGraphicsSvgItem>

#include "common.h"

namespace bovo {
    class Move;
}

using namespace bovo;

class QTimer;

namespace gui {

class Scene;

class Mark : public QGraphicsSvgItem {
    Q_OBJECT
public:
    Mark(Scene *scene, const Move& move, bool animate = true, qreal fill = 0.75);
    ~Mark();
    void killAnimation();
    void kill();
    usi row() const;
    usi col() const;
    void setFill(qreal fill);
    QRectF boundingRect() const Q_DECL_OVERRIDE;

public slots:
    void tick();
    void killTick();

signals:
    void killed(Mark* thisMark);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*,
               QWidget* widget=0) Q_DECL_OVERRIDE;

private:
    Scene* m_scene;
    int m_row;
    int m_col;
    qreal m_sizeShrink, m_fill;
    int m_tick;
    bool m_animate;
    qreal m_opacity;
    QTimer *m_ticker;
};

} /* namespace gui */

#endif // BOVO_MARH_H
