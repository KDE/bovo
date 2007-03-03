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
