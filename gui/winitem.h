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

#ifndef WINITEM_H
#define WINITEM_H

#include <QGraphicsItem>

namespace gui {
  class Scene;

  class WinItem : public QGraphicsItem {
  public:
    WinItem(Scene *scene, unsigned short minX, unsigned short minY, unsigned short maxX, unsigned short maxY);
    QRectF boundingRect() const;

  protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* widget=0);    

  private:
    QRectF boundingRect(unsigned short x, unsigned short y) const;
    Scene* m_scene;
    unsigned short m_maxX;
    unsigned short m_maxY;
    unsigned short m_minX;
    unsigned short m_minY;
  };
} // namespace gui

#endif
