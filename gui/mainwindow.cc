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
#include <QDir>
#include <QLabel>
#include <QIcon>

// KDE includes
#include <kactioncollection.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kgdifficulty.h>
#include <qstatusbar.h>
#include <kstandardgameaction.h>
#include <kselectaction.h>
#include <ktoggleaction.h>
#include <kiconloader.h>
#include <klocalizedstring.h>

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
  m_aiFactory(0), m_animate(true),
  m_winsLabel (new QLabel(i18n("Wins: %1", m_wins))),
  m_lossesLabel (new QLabel(i18n("Losses: %1", m_losses))) {
    statusBar()->showMessage("            ");
    statusBar()->insertPermanentWidget(0, m_winsLabel);
    statusBar()->insertPermanentWidget(1, m_lossesLabel);

    m_aiFactory = new AiFactory();
    KgDifficulty* diff = Kg::difficulty();
    diff->addStandardLevelRange(
        KgDifficultyLevel::RidiculouslyEasy,
        KgDifficultyLevel::Impossible,
        KgDifficultyLevel::Medium //default level
    );
    connect(diff, SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), SLOT(changeSkill()));
    KgDifficultyGUI::init(this);
    diff->setGameRunning(true);

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
        QString rc = QStandardPaths::locate(QStandardPaths::ConfigLocation, "bovorc");
        KConfig savegame(rc);
        KConfigGroup lastGroup(&savegame, "Game");
        if (!m_game->isGameOver() && m_game->demoMode() == NotDemo) {
            const QStringList lastGame = m_game->saveLast();
            lastGroup.writeXdgListEntry("Unfinished", lastGame); // XXX this is bogus
        } else {
            lastGroup.deleteEntry("Unfinished");
        }
        lastGroup.writeEntry("Wins", m_wins);
        lastGroup.writeEntry("Losses", m_losses);
    }
}

void MainWindow::setupThemes() {
    QStringList themercs;
    const QStringList themeDirs = QStandardPaths::locateAll(QStandardPaths::DataLocation, QStringLiteral("themes"), QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString &themeDir, themeDirs) {
	const QStringList entries = QDir(themeDir).entryList(QDir::Dirs);
        Q_FOREACH(const QString &d, entries) {
            QString themeFile = themeDir + '/' + d + "/themerc";
	    if (QFile::exists(themeFile))
	        themercs.append(themeFile);
        }
    }

    int i = 0;
    foreach (const QString &themerc, themercs) {
        KConfig config(themerc);
        KConfigGroup configGroup(&config, "Config");
        const QString pathName = configGroup.readEntry("Path", QString());
        m_themes << Theme(pathName, i);
        ++i;
    }
}

void MainWindow::readConfig() {
    const QString themePath = Settings::theme();
    foreach (const Theme &tmpTheme, m_themes) {
        if (tmpTheme.path() == themePath) {
            m_theme = tmpTheme;
            break;
        }
    }

    m_playbackSpeed = Settings::playbackSpeed();
    m_animate       = Settings::animation();
    m_aiFactory->changeAi(Settings::ai());

    const QString rc = QStandardPaths::locate(QStandardPaths::ConfigLocation, "bovorc");
    KConfig savegame(rc);
    KConfigGroup lastGroup(&savegame, "Game");
    m_lastGame = lastGroup.readXdgListEntry("Unfinished", QStringList()); // XXX this is bogus
    const QString wins = lastGroup.readEntry("Wins", QString());
    if (!wins.isEmpty()) {
        bool ok;
        updateWins(wins.toUInt(&ok));
        
    }
    const QString losses = lastGroup.readEntry("Losses", QString());
    if (!losses.isEmpty()) {
        bool ok;
        updateLosses(losses.toUInt(&ok));
    }
}

void MainWindow::saveSettings() {
    Settings::setTheme(m_theme.path());
    Settings::setPlaybackSpeed(m_playbackSpeed);
    Settings::setAnimation(m_animate);
    Settings::setAi(m_aiFactory->ai());
    Settings::self()->save();
}

void MainWindow::setupActions() {
    KStandardGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    QAction *replayAct = new QAction(QIcon::fromTheme( QLatin1String( "media-playback-start" )),
                            i18n("&Replay"), this);
    actionCollection()->addAction( QLatin1String( "replay" ), replayAct);
    replayAct->setToolTip(i18n("Replay game"));
    replayAct->setWhatsThis(i18n("Replays your last game for you to watch."));
    replayAct->setEnabled(false);

    m_hintAct = KStandardGameAction::hint(this, SLOT(hint()), actionCollection());
    m_hintAct->setEnabled(false);

    KToggleAction *animAct = new KToggleAction(i18n("&Animation"),this);
    actionCollection()->addAction( QLatin1String( "animation" ), animAct);
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
    actionCollection()->addAction( QLatin1String( "themes" ), m_themeAct);
    m_themeAct->setIcon(QIcon( QLatin1String( "games-config-theme" )));
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
    m_demoMode = false;
    if (m_game != 0) {
        m_game->cancelAndWait();
        if (m_scene != 0) {
            disconnect(m_game, 0, m_scene, 0);
        }
        if (!m_game->isGameOver() && m_game->history().size() > 1) {
            m_lossesLabel->setText(i18n("Losses: %1",++m_losses));
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
        m_demoMode = true;
        slotNewDemo();
    } else {
        Dimension dimension(NUMCOLS, NUMCOLS);
        if (m_scene == 0) {
            m_scene = new Scene(m_theme, m_animate);
            if (!m_lastGame.empty()) {
                QString tmp = m_lastGame.first();
                m_computerStarts = tmp.startsWith('2') ? true : false;
            }
            m_game = new Game(dimension, m_lastGame, Kg::difficultyLevel(),
                              m_playbackSpeed, m_aiFactory);
        } else {
            m_game = new Game(dimension, m_computerStarts ? O : X, 
                              Kg::difficultyLevel(), NotDemo, m_playbackSpeed,
                              m_aiFactory);
        }
        m_demoAi = m_aiFactory->createAi(dimension, KgDifficultyLevel::Easy, m_game->player(), Demo);
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
    if (!m_demoMode) {
        // a new game already started, do not start demo
        return;
    }
    if (m_game != 0) {
        m_game->deleteLater();
        m_game = 0;
    }
    if (m_demoAi != 0) {
        m_demoAi->deleteLater();
        m_demoAi = 0;
    }
    Dimension dimension(NUMCOLS, NUMCOLS);
    m_game = new Game(dimension, O, Kg::difficultyLevel(), Demo, m_playbackSpeed,
                      m_aiFactory);
    m_demoAi = m_aiFactory->createAi(dimension, Kg::difficultyLevel(), X, Demo);
    m_scene->setGame(m_game);
    connect(m_game, SIGNAL(boardChanged(const Move&)),
            m_demoAi, SLOT(changeBoard(const Move&)));
    connect(m_game, SIGNAL(playerTurn()), m_demoAi, SLOT(slotMove()),
            Qt::QueuedConnection);
    connect(m_demoAi, SIGNAL(move(const Move&)),
            m_game,  SLOT(move(const Move&)));
    connect(m_game, SIGNAL(gameOver(const QList<Move>&)),
            this, SLOT(slotNewDemoWait()));
    statusBar()->showMessage(i18n("Start a new Game to play"));
    m_game->start();
}

void MainWindow::slotNewDemoWait() {
//    m_scene->setWin(m_game->history());
    QTimer::singleShot(8*m_playbackSpeed, this, SLOT(slotNewDemo()));
}

void MainWindow::increaseWins() {
    updateWins(m_wins + 1);
}

void MainWindow::decreaseWins() {
    updateWins(m_wins > 0 ? m_wins - 1 : 0);
}

void MainWindow::updateWins(const int wins) {
    m_wins = wins;
    m_winsLabel->setText(i18n("Wins: %1", m_wins));
}

void MainWindow::increaseLosses() {
    updateLosses(m_losses + 1);
}

void MainWindow::decreaseLosses() {
    updateLosses(m_losses > 0 ? m_losses - 1 : 0);
}

void MainWindow::updateLosses(const int losses) {
    m_losses = losses;
    m_lossesLabel->setText(i18n("Losses: %1", m_losses));
}

void MainWindow::slotGameOver() {
    if (m_game->boardFull()) {
        statusBar()->showMessage(i18n("GAME OVER. Tie!"));
    } else {
        if (m_game->latestMove().player() == X) {
            statusBar()->showMessage(i18n("GAME OVER. You won!"));
            increaseWins();
        } else {
            statusBar()->showMessage(i18n("GAME OVER. You lost!"));
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
    statusBar()->showMessage(i18n("It is your turn."));
}

void MainWindow::slotOposerTurn() {
    statusBar()->showMessage(i18n("Waiting for computer."));
}

void MainWindow::slotUndo() {
    if (m_game == 0)
        return;
    if (m_game->isGameOver()) {
        if (!m_game->boardFull()) {
            if (m_game->latestMove().player() == X) {
                decreaseWins();
            } else {
                decreaseLosses();
            }
        }
        connect(m_game, SIGNAL(boardChanged(const Move&)),
                m_demoAi, SLOT(changeBoard(const Move&)));
        m_hintAct->setEnabled(true);
        actionCollection()->action("replay")->setEnabled(false);
        disconnect(actionCollection()->action("replay"), SIGNAL(triggered()),
                this, SLOT(replay()));
    }
    m_game->undoLatest();
}

void MainWindow::replay() {
    if (!m_game->isGameOver()) {
        return;
    }
    statusBar()->showMessage(i18n("Replaying game"));
    actionCollection()->action("replay")->setEnabled(false);
    disableUndo();
    disconnect(actionCollection()->action("replay"), SIGNAL(triggered()),
            this, SLOT(replay()));
    disconnect(m_game, 0, this, 0);
    connect(m_game, SIGNAL(replayEnd(const QList<Move>&)),
            this, SLOT(reEnableReplay()));
    disconnect(m_game, 0, m_scene, 0);
    connect(m_game, &Game::replayBegin, m_scene, &Scene::replay);
    connect(m_game, &Game::replayEnd, m_scene, &Scene::slotGameOver);
    m_game->replay();
}

void MainWindow::reEnableReplay() {
    actionCollection()->action("replay")->setEnabled(true);
    statusBar()->showMessage(i18n("Game replayed."));
    connect(actionCollection()->action("replay"), SIGNAL(triggered()),
               this, SLOT(replay()));
}

void MainWindow::changeSkill() {
    if (m_game!=0)
        m_game->setSkill(Kg::difficultyLevel());
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

