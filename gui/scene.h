#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QPixmap>

#include "commondefs.h"
#include "game.h"

class QSvgRenderer;
class QPainter;

namespace gui {
  class Scene : public QGraphicsScene
  {
    Q_OBJECT
  public:
    Scene(Game* game);
    ~Scene();
    void setGame( Game* game );
    void setBackground( const QString& bkgndPath);
    void resizeScene( int width, int height );
    bool isBusy() const;
    QPointF cellCenter( int x, int y ) const;
    QPointF cellTopLeft( int x, int y ) const;
  public slots:
    void updateBoard();
    void slotGameMoveFinished();
  signals:
    void moveFinished();
  private:
    virtual void drawBackground( QPainter *p, const QRectF& rect );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* );
    Game *m_game;
    QSvgRenderer* m_bkgndRenderer;
    qreal m_curCellSize;
  };
} //namespace gui

#endif //SCENE_H
