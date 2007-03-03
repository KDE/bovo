#include <QtGui>
#include <QtSvg>
#include <QtGlobal>

#include "mark.h"
#include "commondefs.h"
#include "scene.h"

namespace gui {
  Mark::Mark(Player player, Scene* scene, int x, int y ) : QGraphicsSvgItem(), m_scene(scene), m_player(player), m_row(y), m_col(x) {
    filename = m_player==X?"./x":"./o";
    filename += m_player==X?QString::number(qrand()%6+1):QString::number(qrand()%5+1);
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
