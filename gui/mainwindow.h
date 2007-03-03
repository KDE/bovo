#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <kmainwindow.h>

#include "scene.h"
#include "game.h"
#include "view.h"

namespace gui {
  class MainWindow : public KMainWindow
  {
    Q_OBJECT
  public:
    explicit MainWindow(QWidget* parent=0);
  public slots:
    void slotNewGame();
    void slotMoveFinished();
    void slotGameOver();
  private:
    void setupActions();
    Scene *m_scene;
    View  *m_view;
    Game  *m_game;
  };
}//namespace gui

#endif
