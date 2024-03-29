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

#ifndef BOVO_HINTITEM_H
#define BOVO_HINTITEM_H

#include <QGraphicsSvgItem>

class QTimer;

namespace bovo
{
class Move;
}

using namespace bovo;

namespace gui
{

class Scene;

class HintItem : public QGraphicsSvgItem
{
    Q_OBJECT
public:
    HintItem(Scene *scene, const Move &move, bool animate = true, qreal fill = 0.75);
    ~HintItem() override;
    void killAnimation();
    void kill();
    void setFill(qreal fill);
    QRectF boundingRect() const override;

public Q_SLOTS:
    void tick();
    void killTick();

Q_SIGNALS:
    void killed();

protected:
    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *widget = nullptr) override;

private:
    Scene *m_scene;
    int m_row;
    int m_col;
    int m_tick;
    bool m_animate;
    qreal m_opacity;
    bool m_tickUp;
    qreal m_fill;
    QTimer *m_ticker;
};

} /* namespace gui */

#endif // BOVO_HINTITEM_H
