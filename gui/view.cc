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

// Declaration include
#include "view.h"

// Qt Includes
#include <QtGui/QColor>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPalette>
#include <QResizeEvent>

// Bovo includes
#include "scene.h"

namespace gui {

View::View(Scene* scene, const QColor& bgColor, QWidget *parent) : QGraphicsView(scene, parent),
           m_scene(scene) {
//    setCacheMode(QGraphicsView::CacheBackground);
    QPalette bgPal;
    bgPal.setColor(backgroundRole(), bgColor);
    setPalette(bgPal);
    setAutoFillBackground(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumSize(sizeHint());
    resize(sizeHint());
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void View::resizeEvent( QResizeEvent* ev ) {
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(ev);
}

QSize View::sizeHint() const {
    return QSize(static_cast<int>(m_scene->width()),
                 static_cast<int>(m_scene->height()));
}

} /* namespace gui */
