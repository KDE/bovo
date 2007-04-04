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

#include <QtGui>
#include <QtSvg>
#include <QtGlobal>
#include <QColor>

#include "mark.h"
#include "common.h"
#include "scene.h"

using namespace bovo;

namespace gui {
  Mark::Mark(Player player, Scene* scene, int x, int y ) : QGraphicsSvgItem(), m_scene(scene), m_row(y), m_col(x) {
    m_sizeShrink = 1.0/(qrand()%5+7.0);
    setElementId(QString(player==X?"x%1":"o%1").arg(QString::number(qrand()%5+1)));
  }

  Mark::~Mark() {
  }

  QRectF Mark::glyphRectF() const {
    qreal width = m_scene->width() / (NUMCOLS+2);
    qreal height = width;
    qreal margin = m_sizeShrink * width;
    return QRectF( (1+m_col) * width  + margin,
                   (1+m_row) * height + margin,
                   width  - 2.0*margin,
                   height - 2.0*margin);
  }

  void Mark::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) {
    renderer()->render(painter, elementId(), glyphRectF());
  }
} /* namespace gui */
