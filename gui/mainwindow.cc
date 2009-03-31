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
#include <QWidget>
#include <QTimer>
#include <QBrush>

// KDE includes
#include <kaction.h>
#include <kactioncollection.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kgamedifficulty.h>
#include <kstatusbar.h>
#include <kstandardgameaction.h>
#include <kstandarddirs.h>
#include <kselectaction.h>
#include <ktoggleaction.h>
#include <klocale.h>
#include <kicon.h>
#include <kiconloader.h>

// Bovo includes
#include "ai.h"
#include "aifactory.h"
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
  m_aiFactory(0), m_animate(true) {
    statusBar()->insertItem("            ", 0, 10);
    statusBar()->setItemAlignment(0, Qt::AlignLeft);
    statusBar()->insertPermanentItem(i18n("Wins: %1", m_wins), 1);
    statusBar()->insertPermanentItem(i18n("Losses: %1", m_losses), 2, 1);

    m_aiFactory = new AiFactory();
    KGameDifficulty::init(this, this, SLOT(changeSkill()));
    KGameDifficulty::addStandardLevel(KGameDifficulty::RidiculouslyEasy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::VeryEasy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::VeryHard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::ExtremelyHard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Impossible);
    KGameDifficulty::setRestartOnChange(KGameDifficulty::RestartOnChange);

    setupThemes();
    readConfig();

    setupActions();
    slotNewGame();

    m_view = new View(m_scene, m_theme.backgroundColor(), this);
    setCentralWidget(m_view);
    m_view->show();
    setupGUI();
}

MainWindow::~MainWindow() {
    save();
    delete m_view;
    delete m_game;
    delete m_demoAi;
    delete m_aiFactory;
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
            lastGroup.writeXdgListEntry("Unfinished", lastGame); // XXX this is bogus
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
    foreach (const QString &themerc, themercs) {
        KConfig config(themerc);
        KConfigGroup configGroup(&config, "Config");
        QString pathName = configGroup.readEntry("Path", QString());
        m_themes << Theme(pathName, i);
        ++i;
    }
}

void MainWindow::readConfig() {
    QString themePath = Settings::theme();
    foreach (const Theme &tmpTheme, m_themes) {
        if (tmpTheme.path() == themePath) {
            m_theme = tmpTheme;
            break;
        }
    }

    m_playbackSpeed = Settings::playbackSpeed();
    m_animate       = Settings::animation();
    m_aiFactory->changeAi(Settings::ai());

    QString rc = KGlobal::dirs()->locate("config", "bovorc");
    KConfig savegame(rc);
    KConfigGroup lastGroup(&savegame, "Game");
    m_lastGame = lastGroup.readXdgListEntry("Unfinished", QStringList()); // XXX this is bogus
    QString wins = lastGroup.readEntry("Wins", QString());
    if (!wins.isEmpty()) {
        bool ok;
        updateWins(wins.toUInt(&ok));
        
    }
    QString losses = lastGroup.readEntry("Losses", QString());
    if (!losses.isEmpty()) {
        bool ok;
        updateLosses(losses.toUInt(&ok));
    }

    KGameDifficulty::setLevel((KGameDifficulty::standardLevel) (Settings::skill()));
    if (KGameDifficulty::level()==KGameDifficulty::NoLevel)
        KGameDifficulty::setLevel(KGameDifficulty::Medium); // default
}

void MainWindow::saveSettings() {
    Settings::setTheme(m_theme.path());
    Settings::setSkill((int)(KGameDifficulty::level()));
    Settings::setPlaybackSpeed(m_playbackSpeed);
    Settings::setAnimation(m_animate);
    Settings::setAi(m_aiFactory->ai());
    Settings::self()->writeConfig();
}

void MainWindow::setupActions() {
    KStandardGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    QAction *replayAct = new KAction(KIcon("media-playback-start"),
                            i18n("&Replay"), this);
    actionCollection()->addAction("replay", replayAct);
    replayAct->setToolTip(i18n("Replay game"));
    replayAct->setWhatsThis(i18n("Replays your last game for you to watch."));
    replayAct->setEnabled(false);

    m_hintAct = KStandardGameAction::hint(this, SLOT(hint()), actionCollection());
    m_hintAct->setEnabled(false);

    KToggleAction *animAct = new KToggleAction(i18n("&Animation"),this);
    actionCollection()->addAction("animation", animAct);
    animAct->setChecked(m_animate);
    connect(animAct, SIGNAL(toggled(bool)), this, SLOT(setAnimation(bool)));

    m_themeAct = new KSelectAction(i18n("Theme"), this);
    QStringList themes;
    foreach (const Theme &theme, m_themes) {
        themes << theme.name();
    }
    m_themeAct->setItems(themes);
    int themeId = 0;
    foreach (const Theme &theme, m_themes) {
        if (theme.path() == m_theme.path()) {
            themeId = theme.id();
            break;
        }
    }
    m_themeAct->setCurrentItem(themeId);
    actionCollection()->addAction("themes", m_themeAct);
    m_themeAct->setIcon(KIcon("games-config-theme"));
    connect(m_themeAct,SIGNAL(triggered(int)),this,SLOT(changeTheme(int)));

    m_undoAct = KStandardGameAction::undo(this, SLOT(slotUndo()), actionCollection());
    m_undoAct->setEnabled(false);

    addAction(replayAct);
    addAction(animAct);
    addAction(m_themeAct);
}

void MainWindow::hint() {
    if (m_game != 0) {
        if (!m_game->computerTurn()) {
            if (m_demoAi != 0) {
                m_demoAi->slotMove();
            }
        }
    }
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
        m_game->cancelAndWait();
        if (m_scene != 0) {
            disconnect(m_game, 0, m_scene, 0);
        }
        if (!m_game->isGameOver() && m_game->history().size() > 1) {
            statusBar()->changeItem(i18n("Losses: %1",++m_losses), 2);
        }
        if (m_game->history().size() > 1) {
            m_computerStarts = !m_computerStarts;
        }
        m_game->deleteLater();
        m_game = 0;
    }
    if (m_demoAi != 0) {
        m_demoAi->cancelAndWait();
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
            if (!m_lastGame.empty()) {
                QString tmp = m_lastGame.first();
                m_computerStarts = tmp.startsWith('2') ? true : false;
            }
            m_game = new Game(dimension, m_lastGame, KGameDifficulty::level(),
                              m_playbackSpeed, m_aiFactory);
        } else {
            m_game = new Game(dimension, m_computerStarts ? O : X, 
                              KGameDifficulty::level(), NotDemo, m_playbackSpeed,
                              m_aiFactory);
        }
        m_demoAi = m_aiFactory->createAi(dimension, KGameDifficulty::Easy, m_game->player(), Demo);
        m_scene->setGame(m_game);
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
        m_hintAct->setEnabled(true);
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
    m_game = new Game(dimension, O, KGameDifficulty::level(), Demo, m_playbackSpeed,
                      m_aiFactory);
    m_demoAi = m_aiFactory->createAi(dimension, KGameDifficulty::level(), X, Demo);
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

void MainWindow::increaseWins() {
    statusBar()->changeItem(i18n("Wins: %1", ++m_wins), 1);
}

void MainWindow::updateWins(const int wins) {
    m_wins = wins;
    statusBar()->changeItem(i18n("Wins: %1", m_wins), 1);
}

void MainWindow::increaseLosses() {
    statusBar()->changeItem(i18n("Losses: %1", ++m_losses), 2);
}

void MainWindow::updateLosses(const int losses) {
    m_losses = losses;
    statusBar()->changeItem(i18n("Losses: %1", m_losses), 2);
}

void MainWindow::slotGameOver() {
    if (m_game->boardFull()) {
        statusBar()->changeItem(i18n("GAME OVER. Tie!"), 0);
    } else {
        if (m_game->latestMove().player() == X) {
            statusBar()->changeItem(i18n("GAME OVER. You won!"), 0);
            increaseWins();
        } else {
            statusBar()->changeItem(i18n("GAME OVER. You lost!"), 0);
            increaseLosses();
        }
    }
    disconnect(m_game, 0, m_demoAi, 0);
    m_hintAct->setEnabled(false);
    actionCollection()->action("replay")->setEnabled(true);
    connect(actionCollection()->action("replay"), SIGNAL(triggered()),
            this, SLOT(replay()));
}

void MainWindow::slotPlayerTurn() {
    statusBar()->changeItem(i18n("It is your turn."), 0);
}

void MainWindow::slotOposerTurn() {
    statusBar()->changeItem(i18n("Waiting for computer."), 0);
}

void MainWindow::slotUndo() {
    if (m_game != 0)
        m_game->undoLatest();
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
    if (KGameDifficulty::level()==KGameDifficulty::NoLevel)
        KGameDifficulty::setLevel(KGameDifficulty::Medium); // default
    if (m_game!=0)
        m_game->setSkill(KGameDifficulty::level());
    saveSettings();
}

void MainWindow::changeTheme(int themeId) {
    foreach (const Theme &theme, m_themes) {
        if (themeId == theme.id()) {
            m_theme = theme;
            m_scene->setTheme(m_theme);
            saveSettings();
            return;
        }
    }
}

void MainWindow::enableUndo() {
    m_undoAct->setEnabled(true);
}

void MainWindow::disableUndo() {
    m_undoAct->setEnabled(false);
}

} /* namespace gui */

// Class moc
#include "mainwindow.moc"
