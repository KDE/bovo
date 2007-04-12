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

#include "mainwindow.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QTimer>
#include <QBrush>

#include <kaction.h>
#include <kactioncollection.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <kselectaction.h>
#include <klocale.h>
#include <kicon.h>

#include "ai.h"
#include "common.h"
#include "dimension.h"
#include "game.h"
#include "move.h"
#include "scene.h"
#include "view.h"

using namespace bovo;
using namespace ai;

namespace gui {

MainWindow::MainWindow(const QString& theme, QWidget* parent)
  : KMainWindow(parent), m_scene(0), m_game(0), m_theme(theme), m_wins(0),
    m_losses(0), m_skill(Normal), m_computerStarts(false), m_demoAi(0) {
    statusBar()->insertItem("            ", 0, 10);
    statusBar()->setItemAlignment(0, Qt::AlignLeft);
    m_sBarSkill = new QComboBox();
    statusBar()->addPermanentWidget(m_sBarSkill);
    statusBar()->insertPermanentItem(i18n("Wins: %0").arg(m_wins), 1);
    statusBar()->insertPermanentItem(i18n("Losses: %0").arg(m_losses), 2);
    slotNewGame();

    m_view = new View(m_scene, this);
    m_view->show();
    setupActions();
    setCentralWidget(m_view);
    setupGUI();
}

MainWindow::~MainWindow() {
    delete m_sBarSkill;
    delete m_view;
    delete m_game;
    delete m_skillsAct;
}

void MainWindow::setupActions() {
    QAction *newGameAct = actionCollection()->addAction(KStandardAction::New,
                            "new_game", this, SLOT(slotNewGame()));
    newGameAct->setToolTip(i18n("New game"));
    newGameAct->setWhatsThis(i18n("Start a new game."));

    QAction *quitAct = actionCollection()->addAction(KStandardAction::Quit,
                            "quit", this, SLOT(close()));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    KAction *replayAct = new KAction(KIcon("media-playback-start"),
                            i18n("&Replay"), this);
    actionCollection()->addAction("replay", replayAct);
    replayAct->setToolTip(i18n("Replay game"));
    replayAct->setWhatsThis(i18n("Replays your last game for you to watch."));
    replayAct->setEnabled(false);

    KAction *hintAct = new KAction(KIcon("idea"), i18n("&Hint"), this);
    actionCollection()->addAction("hint", hintAct);
    hintAct->setToolTip(i18n("Hint a move"));
    hintAct->setWhatsThis(i18n("This gives hints on a good move."));
    hintAct->setEnabled(false);

    m_skillsAct = new KSelectAction(i18n("Computer Difficulty"), this);
    QStringList skills;
    skills << i18n("Ridiculously Easy") << i18n("Very Easy") << i18n("Easy")
           << i18n("Medium") << i18n("Hard") << i18n("Very Hard")
           << i18n("Impossible");
    m_skillsAct->setItems(skills);
    m_skillsAct->setCurrentItem(m_skill);
    actionCollection()->addAction("skill", m_skillsAct);
    connect(m_skillsAct, SIGNAL(triggered(int)),
            this, SLOT(changeSkill(int)));

    m_sBarSkill->addItems(skills);
    m_sBarSkill->setCurrentIndex(m_skill);
    connect(m_sBarSkill, SIGNAL(activated(int)),
            this, SLOT(changeSkill(int)));

    addAction(newGameAct);
    addAction(quitAct);
    addAction(replayAct);
    addAction(hintAct);
    addAction(m_skillsAct);
}

void MainWindow::hint() {
}

void MainWindow::slotNewGame() {
    if (m_game != 0) {
        if (!m_game->isGameOver()) {
            statusBar()->changeItem(i18n("Losses: %0").arg(++m_losses), 2);
        }
        m_game->deleteLater();
        m_game = 0;
    }
    if (m_demoAi != 0) {
        m_demoAi->deleteLater();
        m_demoAi = 0;
    }
    QAction* act = actionCollection()->action("replay");
    if (act != 0) {
        act->setEnabled(false);
    }
    if(m_scene == 0) { //first time, demo time
        m_scene = new Scene(m_theme);
        slotNewDemo();
    } else {
        Dimension dimension(NUMCOLS, NUMCOLS);
        m_game = new Game(dimension, m_computerStarts ? O : X, m_skill);
        m_demoAi = new Ai(dimension, Zlatan, X);
        m_scene->setGame(m_game, X, NotDemo);
        m_computerStarts = !m_computerStarts;
        connect(m_game, SIGNAL(playerTurn()), this, SLOT(slotPlayerTurn()));
        connect(m_game, SIGNAL(oposerTurn()), this, SLOT(slotOposerTurn()));
        connect(m_game, SIGNAL(gameOver()), SLOT(slotGameOver()));
        connect(m_game, SIGNAL(boardChanged(const Move&)),
                m_demoAi, SLOT(changeBoard(const Move&)));
        connect(m_demoAi, SIGNAL(move(const Move&)),
                m_scene,  SLOT(hint(const Move&)));
        disconnect(m_game, SIGNAL(gameOver()),
                   this, SLOT(slotNewDemoWait()));
        actionCollection()->action("hint")->setEnabled(true);
        connect(actionCollection()->action("hint"), SIGNAL(triggered()),
                m_demoAi, SLOT(slotMove()));
        m_game->start();
    }
}

void MainWindow::slotNewDemo() {
    if (m_game != 0) {
        m_game->deleteLater();
        m_game = 0;
    }
    if (m_demoAi != 0) {
        m_demoAi->deleteLater();
        m_demoAi = 0;
    }
    Dimension dimension(NUMCOLS, NUMCOLS);
    m_game = new Game(dimension, O, m_skill, Demo);
    m_demoAi = new Ai(dimension, m_skill, X);
    m_scene->setGame(m_game, No, Demo);
    connect(m_game, SIGNAL(boardChanged(const Move&)),
            m_demoAi, SLOT(changeBoard(const Move&)));
    connect(m_game, SIGNAL(playerTurn()), m_demoAi, SLOT(slotMove()),
            Qt::QueuedConnection);
    connect(m_demoAi, SIGNAL(move(const Move&)),
            m_game,  SLOT(move(const Move&)));
    connect(m_game, SIGNAL(gameOver()), this, SLOT(slotNewDemoWait()));
    statusBar()->changeItem(i18n("Start a new Game to play"), 0);
    m_game->start();
}

void MainWindow::slotNewDemoWait() {
    m_scene->setWin();
    QTimer::singleShot(2000, this, SLOT(slotNewDemo()));
}

void MainWindow::slotGameOver() {
    QString message;
    if (m_game->latestMove().player() == X) {
        statusBar()->changeItem(i18n("GAME OVER. You won!"), 0);
        statusBar()->changeItem(i18n("Wins: %0").arg(++m_wins), 1);
        message = i18n("You won!");
    } else {
        statusBar()->changeItem(i18n("GAME OVER. You lost!"), 0);
        statusBar()->changeItem(i18n("Losses: %0").arg(++m_losses), 2);
        message = i18n("You lost!");
    }
    m_scene->setWin();
    actionCollection()->action("hint")->setEnabled(false);
    actionCollection()->action("replay")->setEnabled(true);
    connect(actionCollection()->action("replay"), SIGNAL(triggered()),
            this, SLOT(replay()));
    KMessageBox::information(this, message, i18n("Game over"));
}

void MainWindow::slotPlayerTurn() {
    statusBar()->changeItem(i18n("It is your turn."), 0);
}

void MainWindow::slotOposerTurn() {
    statusBar()->changeItem(i18n("Waiting for computer."), 0);
}

void MainWindow::replay() {
    if (!m_game->isGameOver()) {
        return;
    }
    statusBar()->changeItem(i18n("Replaying game"), 0);
    actionCollection()->action("replay")->setEnabled(false);
    m_scene->replay(m_game->history());
    connect(m_scene, SIGNAL(replayFinished()),
            this, SLOT(reEnableReplay()));
}

void MainWindow::reEnableReplay() {
    actionCollection()->action("replay")->setEnabled(true);
    statusBar()->changeItem(i18n("Game replayed"), 0);
}

void MainWindow::changeSkill(int skill) {
    switch (skill) {
        case 0: m_skill = RidiculouslyEasy; break;
        case 1: m_skill = VeryEasy; break;
        case 2: m_skill = Easy; break;
        case 3: m_skill = Normal; break;
        case 4: m_skill = Hard; break;
        case 5: m_skill = VeryHard; break;
        case 6: m_skill = Zlatan; break;
    }
    m_sBarSkill->setCurrentIndex(skill);
    m_skillsAct->setCurrentItem(skill);
    m_game->setSkill(m_skill);
}

QString MainWindow::getSkillName(Skill skill) const {
    switch (skill) {
        case RidiculouslyEasy: return i18n("Ridiculously Easy");
        case VeryEasy: return i18n("Very Easy");
        case Easy: return i18n("Easy");
        case Normal: return i18n("Normal");
        case Hard: return i18n("Hard");
        case VeryHard: return i18n("Very Hard");
        case Zlatan: return i18n("Impossible");
        default: return i18n("Illegal skill");
    }
}

} /* namespace gui */

#include "mainwindow.moc"
