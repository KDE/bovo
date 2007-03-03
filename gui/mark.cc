/*******************************************************************
*
* Copyright 2002,2007 Aron Boström <aron.bostrom@gmail.com>
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

#include <kstandarddirs.h>

#include "mark.h"
#include "commondefs.h"
#include "scene.h"

namespace gui {
  Mark::Mark(Player player, Scene* scene, int x, int y ) : QGraphicsSvgItem(), m_scene(scene), m_player(player), m_row(y), m_col(x) {
    QString themeName = QString("themes/%1/pics/").arg("scribble"); // read scribble from some configuration, I guess
    int themeNbrX = 6; //read this from some .desktop file, I guess
    int themeNbrY = 5; //read this from some .desktop file, I guess
    filename = KStandardDirs::locate("appdata", themeName);
    filename += m_player==X?'x':'o';
    filename += m_player==X?QString::number(qrand()%themeNbrX+1):QString::number(qrand()%themeNbrY+1);
    filename += ".svg";
    m_renderer = new QSvgRenderer(filename);
    setSharedRenderer(m_renderer);
//    rePos();
    qreal factor = (m_scene->width() / m_renderer->defaultSize().width()) / static_cast<qreal>(NUMCOLS);
//    scale(factor, factor);
//    setPos(m_scene->cellTopLeft(m_col, m_row));
    rotate(qrand()%20-10);
  }

  Mark::~Mark() {
    delete m_renderer;
  }

  void Mark::setPlayer( Player player ) {
    m_player = player;
  }

  void Mark::rePos() {
    m_renderer->load(filename);
    QRectF m_bounding = QRectF(static_cast<qreal>(m_col+1.0) * m_scene->width() / static_cast<qreal>(NUMCOLS+2),
                               static_cast<qreal>(m_row+1.0) * m_scene->height() / static_cast<qreal>(NUMCOLS+2),
                               m_scene->width() / static_cast<qreal>(NUMCOLS+2),
                               m_scene->height() / static_cast<qreal>(NUMCOLS+2)
                        );
  }
  
  void Mark::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QRectF sceneRect = m_scene->sceneRect();
    qreal onewidth = sceneRect.width() / static_cast<qreal>(NUMCOLS+2.0);
    qreal oneheight = onewidth;
    qreal totleft = sceneRect.left();
    qreal tottop = sceneRect.top();
    qreal xpos = 0 + onewidth * static_cast<qreal>(m_col+1.0);
    qreal ypos = 0 + oneheight * static_cast<qreal>(m_row+1.0);
    QRectF glyphRect(xpos, ypos, onewidth, oneheight);
    m_renderer->render(painter, glyphRect);
  }
} //namespace gui
