#include "view.h"
#include "scene.h"

#include <QGraphicsScene>
#include <QResizeEvent>

namespace gui {
  View::View(Scene* scene, QWidget *parent) : QGraphicsView(scene, parent), m_scene(scene) {
    setCacheMode( QGraphicsView::CacheBackground );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMinimumSize( sizeHint() );
    resize( sizeHint() );
  }

  void View::resizeEvent( QResizeEvent* ev ) {
    m_scene->resizeScene( ev->size().width(), ev->size().height() );
    QGraphicsView::resizeEvent(ev);
  }

  QSize View::sizeHint() const {
    return QSize( static_cast<int>(m_scene->width()), static_cast<int>(m_scene->height()));
  }
} //namespace gui
