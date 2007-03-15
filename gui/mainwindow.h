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

#include "common.h"

using namespace bovo;

class QString;
class QComboBox;
class KSelectAction;

namespace gui {
  class Game;
  class Scene;
  class View;

  class MainWindow : public KMainWindow
  {
    Q_OBJECT
  public:
    explicit MainWindow(QWidget* parent=0);
    ~MainWindow();

  public slots:
    void slotNewGame();
    void slotMoveFinished();
    void slotGameOver();
    void replay();
    void changeSkill(int);
    void reEnableReplay();

  private:
    Scene *m_scene;
    View  *m_view;
    Game  *m_game;
    int m_wins, m_losses;
    void setupActions();
    Skill m_skill;
    QString getSkillName(Skill skill) const;
    QComboBox* m_sBarSkill;
    KSelectAction* m_skillsAct;
    bool m_computerStarts;
  };
} // namespace gui

#endif
