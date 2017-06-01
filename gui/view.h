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

#ifndef BOVO_VIEW_H
#define BOVO_VIEW_H

#include <QGraphicsView>

namespace gui {

class Scene;
class bgColor;

class View : public QGraphicsView
{
public:
    View(Scene* scene, const QColor& bgColor, QWidget *parent);

private:
    void resizeEvent( QResizeEvent* ) Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    Scene* m_scene;
};

} /* namespace gui */

#endif // BOVO_VIEW_H
