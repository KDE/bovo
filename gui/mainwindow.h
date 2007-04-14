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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QString>

#include <kmainwindow.h>

#include "common.h"

namespace ai {
    class Ai;
}

namespace bovo {
    class Game;
}

using namespace bovo;
using namespace ai;

class QComboBox;
class KSelectAction;

namespace gui {

class Scene;
class Theme;
class View;

class MainWindow : public KMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString& theme = "scribble", QWidget* parent=0);
    ~MainWindow();

public slots:
    void hint();
    void slotNewGame();
    void slotPlayerTurn();
    void slotOposerTurn();
    void slotGameOver();
    void slotNewDemo();
    void slotNewDemoWait();
    void replay();
    void changeSkill(int);
    void changeTheme(int);
    void reEnableReplay();
    void enableUndo();
    void disableUndo();
    void setupThemes();

private:
    Scene *m_scene;
    View  *m_view;
    Game  *m_game;
    QString m_theme;
    QList<Theme> m_themes;
    int m_wins, m_losses;
    void setupActions();
    Skill m_skill;
    QString getSkillName(Skill skill) const;
    QComboBox* m_sBarSkill;
    KSelectAction* m_skillsAct;
    KSelectAction* m_themeAct;
    bool m_computerStarts;
    Ai *m_demoAi;
    void readConfig();
    void saveSettings();
    Skill idToSkill(int id) const;
    int skillToId(Skill skill) const;
    int m_playbackSpeed;
};

} /* namespace gui */

#endif /* __MAINWINDOW_H__ */
