#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QWidget>

#include "scene.h"

namespace gui {
  class View : public QGraphicsView
  {
  public:
    View( Scene* scene, QWidget *parent );
  private:
    virtual void resizeEvent( QResizeEvent* );
    virtual QSize sizeHint() const;

    Scene* m_scene;
  };
} // namespace gui

#endif
