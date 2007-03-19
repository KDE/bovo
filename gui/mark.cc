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
#include <QColor>

#include <kstandarddirs.h>

#include "mark.h"
#include "common.h"
#include "scene.h"

using namespace bovo;

namespace gui {
  Mark::Mark(Player player, Scene* scene, int x, int y ) : QGraphicsSvgItem(), m_scene(scene), m_player(player), m_row(y), m_col(x) {
    QString themeName = QString("themes/%1/pics/").arg("scribble"); // read scribble from some configuration, I guess
//    int themeNbrX = 6; //read this from some .desktop file, I guess
//    int themeNbrY = 5; //read this from some .desktop file, I guess
    QString filename = KStandardDirs::locate("appdata", themeName);
    filename += "xo.svg";
    m_sizeShrink = 1.0/(qrand()%5+7.0);
    m_renderer = new QSvgRenderer(filename);
    setSharedRenderer(m_renderer);
    setElementId(QString(m_player==X?"x%1":"o%1").arg(QString::number(qrand()%5+1)));
//    qreal factor = (m_scene->width() / m_renderer->defaultSize().width()) / static_cast<qreal>(NUMCOLS);
// What a pity! I can't use rotate() to create more individualized marks. That distorts glyphRectF into a useless state...
//    rotate(qrand()%20-10);
  }

  Mark::~Mark() {
    delete m_renderer;
  }

  void Mark::setPlayer( Player player ) {
    m_player = player;
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
    m_renderer->render(painter, elementId(), glyphRectF());
  }

  void Mark::setRowCol( int row, int col ) {
    m_row = row;
    m_col = col;
  }

  void Mark::setXY(int x, int y) {
    m_col = x;
    m_row = y;
  }

  int Mark::y() const {
    return m_row;
  }

  int Mark::row() const {
    return m_row;
  }

  int Mark::x() const {
    return m_col;
  }

  int Mark::col() const {
    return m_col;
  }

  Player Mark::player() const {
    return m_player;
  }

} //namespace gui
