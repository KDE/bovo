/*******************************************************************
*
* Copyright 2007  Aron Boström <c02ab@efd.lth.se>
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
#include <KActionCollection>
#include <KConfig>
#include <KConfigGroup>
#include <KgDifficulty>
#include <QStatusBar>
#include <KStandardGameAction>
#include <KSelectAction>
#include <KToggleAction>
#include <KLocalizedString>

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
    statusBar()->showMessage(QStringLiteral("            "));
    statusBar()->insertPermanentWidget(0, m_winsLabel);
    statusBar()->insertPermanentWidget(1, m_lossesLabel);

    m_aiFactory = new AiFactory();
    KgDifficulty* diff = Kg::difficulty();
    diff->addStandardLevelRange(
        KgDifficultyLevel::RidiculouslyEasy,
        KgDifficultyLevel::Impossible,
        KgDifficultyLevel::Medium //default level
    );
    connect(diff, &KgDifficulty::currentLevelChanged, this, &MainWindow::changeSkill);
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
    delete m_scene;
}

void MainWindow::save() const {
    if (m_game != 0) {
        m_scene->activate(false);
        QString rc = QStandardPaths::locate(QStandardPaths::ConfigLocation, QStringLiteral("bovorc"));
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
    const QStringList themeDirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("themes"), QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString &themeDir, themeDirs) {
    const QStringList entries = QDir(themeDir).entryList(QDir::Dirs);
    Q_FOREACH(const QString &d, entries) {
        QString themeFile = themeDir + QLatin1Char('/') + d + QLatin1String("/themerc");
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

    const QString rc = QStandardPaths::locate(QStandardPaths::ConfigLocation, QStringLiteral("bovorc"));
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

    QAction *replayAct = new QAction(QIcon::fromTheme( QStringLiteral( "media-playback-start" )),
                            i18n("&Replay"), this);
    actionCollection()->addAction( QStringLiteral( "replay" ), replayAct);
    replayAct->setToolTip(i18n("Replay game"));
    replayAct->setWhatsThis(i18n("Replays your last game for you to watch."));
    replayAct->setEnabled(false);

    m_hintAct = KStandardGameAction::hint(this, SLOT(hint()), actionCollection());
    m_hintAct->setEnabled(false);

    KToggleAction *animAct = new KToggleAction(i18n("&Animation"),this);
    actionCollection()->addAction( QStringLiteral( "animation" ), animAct);
    animAct->setChecked(m_animate);
    connect(animAct, &QAction::toggled, this, &MainWindow::setAnimation);

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
    actionCollection()->addAction( QStringLiteral( "themes" ), m_themeAct);
    m_themeAct->setIcon(QIcon::fromTheme( QStringLiteral( "games-config-theme" )));
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
    QAction* act = actionCollection()->action(QStringLiteral("replay"));
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
                m_computerStarts = tmp.startsWith(QLatin1Char('2')) ? true : false;
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
        connect(m_game, &Game::undoAble, this, &MainWindow::enableUndo);
        connect(m_game, &Game::undoNotAble, this, &MainWindow::disableUndo);
        connect(m_game, &Game::playerTurn, this, &MainWindow::slotPlayerTurn);
        connect(m_game, &Game::oposerTurn, this, &MainWindow::slotOposerTurn);
        connect(m_game, &Game::gameOver,
                this, &MainWindow::slotGameOver);
        connect(m_game, &Game::boardChanged,
                m_demoAi, &Ai::changeBoard);
        connect(m_demoAi, SIGNAL(move(Move)),
                m_scene,  SLOT(hint(Move)));
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
    connect(m_game, &Game::boardChanged,
            m_demoAi, &Ai::changeBoard);
    connect(m_game, &Game::playerTurn, m_demoAi, &Ai::slotMove,
            Qt::QueuedConnection);
    connect(m_demoAi, SIGNAL(move(Move)),
            m_game,  SLOT(move(Move)));
    connect(m_game, &Game::gameOver,
            this, &MainWindow::slotNewDemoWait);
    statusBar()->showMessage(i18n("Start a new Game to play"));
    m_game->start();
}

void MainWindow::slotNewDemoWait() {
//    m_scene->setWin(m_game->history());
    QTimer::singleShot(8*m_playbackSpeed, this, &MainWindow::slotNewDemo);
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
    actionCollection()->action(QStringLiteral("replay"))->setEnabled(true);
    connect(actionCollection()->action(QStringLiteral("replay")), &QAction::triggered,
            this, &MainWindow::replay);
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
        connect(m_game, &Game::boardChanged,
                m_demoAi, &Ai::changeBoard);
        m_hintAct->setEnabled(true);
        actionCollection()->action(QStringLiteral("replay"))->setEnabled(false);
        disconnect(actionCollection()->action(QStringLiteral("replay")), &QAction::triggered,
                this, &MainWindow::replay);
    }
    m_game->undoLatest();
}

void MainWindow::replay() {
    if (!m_game->isGameOver()) {
        return;
    }
    statusBar()->showMessage(i18n("Replaying game"));
    actionCollection()->action(QStringLiteral("replay"))->setEnabled(false);
    disableUndo();
    disconnect(actionCollection()->action(QStringLiteral("replay")), &QAction::triggered,
            this, &MainWindow::replay);
    disconnect(m_game, 0, this, 0);
    connect(m_game, &Game::replayEnd,
            this, &MainWindow::reEnableReplay);
    disconnect(m_game, 0, m_scene, 0);
    connect(m_game, &Game::replayBegin, m_scene, &Scene::replay);
    connect(m_game, &Game::replayEnd, m_scene, &Scene::slotGameOver);
    m_game->replay();
}

void MainWindow::reEnableReplay() {
    actionCollection()->action(QStringLiteral("replay"))->setEnabled(true);
    statusBar()->showMessage(i18n("Game replayed."));
    connect(actionCollection()->action(QStringLiteral("replay")), &QAction::triggered,
               this, &MainWindow::replay);
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

