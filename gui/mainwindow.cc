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

// Class declaration
#include "mainwindow.h"

// Qt includes
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QTimer>
#include <QBrush>
#include <QLabel>

// KDE includes
#include <kaction.h>
#include <kactioncollection.h>
#include <kgamedifficulty.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <kselectaction.h>
#include <ktoggleaction.h>
#include <klocale.h>
#include <kicon.h>
#include <kdesktopfile.h>
#include <kiconloader.h>

// Bovo includes
#include "ai.h"
#include "common.h"
#include "dimension.h"
#include "game.h"
#include "move.h"
#include "scene.h"
#include "theme.h"
#include "view.h"

// KConfig XT includes
#include "settings.h"

using namespace bovo;
using namespace ai;

namespace gui {

MainWindow::MainWindow(QWidget* parent)
  : KXmlGuiWindow(parent), m_scene(0), m_game(0), m_wins(0),
  m_losses(0), m_computerStarts(false), m_demoAi(0),
  m_animate(true) {
    statusBar()->insertItem("            ", 0, 10);
    statusBar()->setItemAlignment(0, Qt::AlignLeft);
    statusBar()->insertPermanentItem(i18n("Wins: %1",m_wins), 1);
    statusBar()->insertPermanentItem(i18n("Losses: %1",m_losses), 2, 1);

    KGameDifficulty::init(this, this, SLOT(changeSkill()));
    KGameDifficulty::addStandardLevel(KGameDifficulty::ridiculouslyEasy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::veryEasy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::hard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::veryHard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::impossible);
    KGameDifficulty::setRestartOnChange(KGameDifficulty::noRestartOnChange);

    setupThemes();
    readConfig();

    setupActions();
    slotNewGame();

    m_view = new View(m_scene, this);
    m_view->show();
    setCentralWidget(m_view);
    setupGUI();
}

MainWindow::~MainWindow() {
    save();
    delete m_view;
    delete m_game;
}

void MainWindow::save() const {
    if (m_game != 0) {
        m_scene->activate(false);
        QString rc = KGlobal::dirs()->locate("config", "bovorc");
        KConfig savegame(rc);
        KConfigGroup lastGroup(&savegame, "Game");
        if (!m_game->isGameOver() && m_game->demoMode() == NotDemo) {
            QStringList lastGame;
            lastGame = m_game->saveLast();
            lastGroup.writeEntry("Unfinished", lastGame, ';');
        } else {
            lastGroup.deleteEntry("Unfinished");
        }
        lastGroup.writeEntry("Wins", m_wins);
        lastGroup.writeEntry("Losses", m_losses);
    }
}

void MainWindow::setupThemes() {
    QStringList themercs = KGlobal::dirs()->findAllResources("appdata",
                           "themes/*/themerc");
    int i = 0;
    foreach (QString themerc, themercs) {
        KDesktopFile themeConfig(themerc);
        QString themeName(themeConfig.readName());
        QString themeComment(themeConfig.readComment());
        KConfig config(themerc);
        KConfigGroup configGroup(&config, "Config");
        QString pathName = configGroup.readEntry("Path", QString());
        m_themes << Theme(themeName, pathName, i, themeComment);
        ++i;
    }
}

void MainWindow::readConfig() {
    QString themePath = Settings::theme();
    foreach (Theme tmpTheme, m_themes) {
        if (tmpTheme.path() == themePath) {
            m_theme = tmpTheme;
            break;
        }
    }

    KGameDifficulty::setLevel((KGameDifficulty::standardLevel) (Settings::skill()));
    changeSkill();

    m_playbackSpeed = Settings::playbackSpeed();
    m_animate       = Settings::animation();

    QString rc = KGlobal::dirs()->locate("config", "bovorc");
    KConfig savegame(rc);
    KConfigGroup lastGroup(&savegame, "Game");
    m_lastGame = lastGroup.readEntry("Unfinished", QStringList(), ';');
    QString wins = lastGroup.readEntry("Wins", QString());
    if (!wins.isEmpty()) {
        bool ok;
        m_wins = wins.toUInt(&ok);
    }
    QString losses = lastGroup.readEntry("Losses", QString());
    if (!losses.isEmpty()) {
        bool ok;
        m_losses = losses.toUInt(&ok);
    }
}

void MainWindow::saveSettings() {
    Settings::setTheme(m_theme.path());
    Settings::setSkill((int)(KGameDifficulty::level()));
    Settings::setPlaybackSpeed(m_playbackSpeed);
    Settings::setAnimation(m_animate);
    Settings::self()->writeConfig();
}

void MainWindow::setupActions() {
    QAction *newGameAct = actionCollection()->addAction(KStandardAction::New,
                            "new_game", this, SLOT(slotNewGame()));
    newGameAct->setToolTip(i18n("New game"));
    newGameAct->setWhatsThis(i18n("Start a new game."));

    QAction *quitAct = actionCollection()->addAction(KStandardAction::Quit,
                            "quit", this, SLOT(close()));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    QAction *replayAct = new KAction(KIcon("media-playback-start"),
                            i18n("&Replay"), this);
    actionCollection()->addAction("replay", replayAct);
    replayAct->setToolTip(i18n("Replay game"));
    replayAct->setWhatsThis(i18n("Replays your last game for you to watch."));
    replayAct->setEnabled(false);

    QAction *hintAct = new KAction(KIcon("idea"), i18n("&Hint"), this);
    actionCollection()->addAction("hint", hintAct);
    hintAct->setToolTip(i18n("Hint a move"));
    hintAct->setWhatsThis(i18n("This gives hints on a good move."));
    hintAct->setEnabled(false);

    KToggleAction *animAct = new KToggleAction(i18n("&Animation"),this);
    actionCollection()->addAction("animation", animAct);
    animAct->setChecked(m_animate);
    connect(animAct, SIGNAL(toggled(bool)), this, SLOT(setAnimation(bool)));

    m_themeAct = new KSelectAction(i18n("Theme"), this);
    QStringList themes;
    foreach (Theme theme, m_themes) {
        themes << theme.name();
    }
    m_themeAct->setItems(themes);
    int themeId = 0;
    foreach (Theme theme, m_themes) {
        if (theme.path() == m_theme.path()) {
            themeId = theme.id();
            break;
        }
    }
    m_themeAct->setCurrentItem(themeId);
    actionCollection()->addAction("themes", m_themeAct);
    connect(m_themeAct,SIGNAL(triggered(int)),this,SLOT(changeTheme(int)));

    QAction *undoAct = actionCollection()->addAction(KStandardAction::Undo,
                                        "undo", this);
    undoAct->setToolTip(i18n("Undo your latest move."));
    undoAct->setWhatsThis(i18n("This will undo the latest turn."));
    undoAct->setEnabled(false);

    addAction(newGameAct);
    addAction(quitAct);
    addAction(replayAct);
    addAction(hintAct);
    addAction(animAct);
    addAction(m_themeAct);
    addAction(undoAct);
}

void MainWindow::hint() {
}

void MainWindow::setAnimation(bool enabled) {
    if (m_scene != 0) {
        if (enabled != m_animate) {
            m_scene->enableAnimation(enabled);
        }
    }
    m_animate = enabled;
    saveSettings();
}

void MainWindow::slotNewGame() {
    if (m_game != 0) {
        if (m_scene != 0) {
            disconnect(m_game, 0, m_scene, 0);
        }
        if (!m_game->isGameOver()) {
            statusBar()->changeItem(i18n("Losses: %1",++m_losses), 2);
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
    if (m_scene == 0 && (m_lastGame.isEmpty())) { //first time, demo time
        m_scene = new Scene(m_theme, m_animate);
        slotNewDemo();
    } else {
        Dimension dimension(NUMCOLS, NUMCOLS);
        if (m_scene == 0) {
            m_scene = new Scene(m_theme, m_animate);
            m_game = new Game(dimension, m_lastGame, KGameDifficulty::level(),
                              m_playbackSpeed);
        } else {
            m_game = new Game(dimension, m_computerStarts ? O : X, 
                              KGameDifficulty::level(), NotDemo, m_playbackSpeed);
        }
        m_demoAi = new Ai(dimension, KGameDifficulty::impossible, m_game->player());
        m_scene->setGame(m_game);
        m_computerStarts = !m_computerStarts;
        connect(m_game, SIGNAL(undoAble()), this, SLOT(enableUndo()));
        connect(m_game, SIGNAL(undoNotAble()), this, SLOT(disableUndo()));
        connect(m_game, SIGNAL(playerTurn()), this, SLOT(slotPlayerTurn()));
        connect(m_game, SIGNAL(oposerTurn()), this, SLOT(slotOposerTurn()));
        connect(m_game, SIGNAL(gameOver(const QList<Move>&)),
                this, SLOT(slotGameOver()));
        connect(m_game, SIGNAL(boardChanged(const Move&)),
                m_demoAi, SLOT(changeBoard(const Move&)));
        connect(m_demoAi, SIGNAL(move(const Move&)),
                m_scene,  SLOT(hint(const Move&)));
        if (actionCollection()->action("hint") != 0) {
            actionCollection()->action("hint")->setEnabled(true);
            connect(actionCollection()->action("hint"), SIGNAL(triggered()),
                    m_demoAi, SLOT(slotMove()));
        }
        if (m_lastGame.isEmpty()) {
            m_game->start();
        } else {
            m_lastGame.clear();
            m_game->startRestored();
        }
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
    m_game = new Game(dimension, O, KGameDifficulty::level(), Demo, m_playbackSpeed);
    m_demoAi = new Ai(dimension, KGameDifficulty::level(), X);
    m_scene->setGame(m_game);
    connect(m_game, SIGNAL(boardChanged(const Move&)),
            m_demoAi, SLOT(changeBoard(const Move&)));
    connect(m_game, SIGNAL(playerTurn()), m_demoAi, SLOT(slotMove()),
            Qt::QueuedConnection);
    connect(m_demoAi, SIGNAL(move(const Move&)),
            m_game,  SLOT(move(const Move&)));
    connect(m_game, SIGNAL(gameOver(const QList<Move>&)),
            this, SLOT(slotNewDemoWait()));
    statusBar()->changeItem(i18n("Start a new Game to play"), 0);
    m_game->start();
}

void MainWindow::slotNewDemoWait() {
//    m_scene->setWin(m_game->history());
    QTimer::singleShot(8*m_playbackSpeed, this, SLOT(slotNewDemo()));
}

void MainWindow::slotGameOver() {
    QString message;
    if (m_game->latestMove().player() == X) {
        statusBar()->changeItem(i18n("GAME OVER. You won!"), 0);
        statusBar()->changeItem(i18n("Wins: %1",++m_wins), 1);
        message = i18n("You won!");
    } else {
        statusBar()->changeItem(i18n("GAME OVER. You lost!"), 0);
        statusBar()->changeItem(i18n("Losses: %1",++m_losses), 2);
        message = i18n("You lost!");
    }
//    m_scene->setWin(m_game->history());
    disconnect(m_game, 0, m_demoAi, 0);
    actionCollection()->action("hint")->setEnabled(false);
    actionCollection()->action("replay")->setEnabled(true);
    connect(actionCollection()->action("replay"), SIGNAL(triggered()),
            this, SLOT(replay()));
//     KMessageBox::information(this, message, i18n("Game over"));
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
    disconnect(actionCollection()->action("replay"), SIGNAL(triggered()),
            this, SLOT(replay()));
    disconnect(m_game, 0, this, 0);
    connect(m_game, SIGNAL(replayEnd(const QList<Move>&)),
            this, SLOT(reEnableReplay()));
    disconnect(m_game, 0, m_scene, 0);
    connect(m_game, SIGNAL(replayBegin()), m_scene, SLOT(replay()));
    connect(m_game, SIGNAL(replayEnd(const QList<Move>&)),
            m_scene, SLOT(slotGameOver(const QList<Move>&)));
    m_game->replay();
}

void MainWindow::reEnableReplay() {
    actionCollection()->action("replay")->setEnabled(true);
    statusBar()->changeItem(i18n("Game replayed."), 0);
    connect(actionCollection()->action("replay"), SIGNAL(triggered()),
               this, SLOT(replay()));
}

void MainWindow::changeSkill() {
    if (KGameDifficulty::level()==KGameDifficulty::noLevel)
        KGameDifficulty::setLevel(KGameDifficulty::medium); // default
    if (m_game!=0)
        m_game->setSkill(KGameDifficulty::level());
    saveSettings();
}

void MainWindow::changeTheme(int themeId) {
    foreach (Theme theme, m_themes) {
        if (themeId == theme.id()) {
            m_theme = theme;
            m_scene->setTheme(m_theme);
            saveSettings();
            return;
        }
    }
}

void MainWindow::enableUndo() {
    connect(actionCollection()->action("undo"), SIGNAL(triggered()),
           m_game, SLOT(undoLatest()));
    actionCollection()->action("undo")->setEnabled(true);
}

void MainWindow::disableUndo() {
    disconnect(actionCollection()->action("undo"), SIGNAL(triggered()),
               m_game, SLOT(undoLatest()));
    actionCollection()->action("undo")->setEnabled(false);
}

} /* namespace gui */

// Class moc
#include "mainwindow.moc"
