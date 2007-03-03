#ifndef MARK_H
#define MARK_H

#include <QtSvg>
#include <QString>
#include <QtGui>

#include "commondefs.h"
#include "scene.h"

namespace gui {
  class Mark : public QGraphicsSvgItem {
  public:
    Mark(Player player, Scene *scene, int x, int y);
    ~Mark();
    void setPlayer( Player player );
    void setRowCol( int row, int col ) { m_row = row; m_col = col; };
    void setXY(int x, int y) { m_col = x; m_row = y; }
    void rePos();
    
    int y() const {return m_row;}
    int row() const { return m_row; }
    int x() const {return m_col;}
    int col() const { return m_col; }
    Player player() const { return m_player; }

  protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* widget=0);
  private:
    Scene* m_scene;
    Player m_player;
    int m_row;
    int m_col;
    QSvgRenderer* m_renderer;
    QString filename;
    QRectF m_bounding;
  };
} // namespace gui

#endif
