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
#include <KGameDifficulty>
#include <QStatusBar>
#include <KGameStandardAction>
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
  : KXmlGuiWindow(parent), m_scene(nullptr), m_game(nullptr), m_wins(0),
  m_losses(0), m_computerStarts(false), m_demoAi(nullptr),
  m_aiFactory(nullptr), m_animate(true),
  m_winsLabel (new QLabel(i18n("Wins: %1", m_wins))),
  m_lossesLabel (new QLabel(i18n("Losses: %1", m_losses))) {
    statusBar()->insertPermanentWidget(0, m_winsLabel);
    statusBar()->insertPermanentWidget(1, m_lossesLabel);

    m_aiFactory = new AiFactory();
    KGameDifficulty* diff = KGameDifficulty::global();
    diff->addStandardLevelRange(
        KGameDifficultyLevel::RidiculouslyEasy,
        KGameDifficultyLevel::Impossible,
        KGameDifficultyLevel::Medium //default level
    );
    connect(diff, &KGameDifficulty::currentLevelChanged, this, &MainWindow::changeSkill);
    KGameDifficultyGUI::init(this);
    diff->setGameRunning(true);

    setupThemes();
    readConfig();

    setupActions();
    slotNewGame();

    m_view = new View(m_scene, m_theme.backgroundColor(), this);
    setCentralWidget(m_view);
    m_view->show();
    setupGUI();

    QFontMetrics fm(font());
    auto base = fm.boundingRect(QLatin1Char('x'));
    setMinimumSize(base.width() * 45, base.height() * 55);
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
    if (m_game != nullptr) {
        m_scene->activate(false);
        QString rc = QStandardPaths::locate(QStandardPaths::ConfigLocation, QStringLiteral("bovorc"));
        KConfig savegame(rc);
        KConfigGroup lastGroup(&savegame, QStringLiteral("Game"));
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
    for (const QString &themeDir : themeDirs) {
    const QStringList entries = QDir(themeDir).entryList(QDir::Dirs);
    for(const QString &d : entries) {
        QString themeFile = themeDir + QLatin1Char('/') + d + QLatin1String("/themerc");
        if (QFile::exists(themeFile))
            themercs.append(themeFile);
        }
    }

    int i = 0;
    for (const QString &themerc : std::as_const(themercs)) {
        KConfig config(themerc);
        KConfigGroup configGroup(&config, QStringLiteral("Config"));
        const QString pathName = configGroup.readEntry("Path", QString());
        m_themes << Theme(pathName, i);
        ++i;
    }
}

void MainWindow::readConfig() {
    const QString themePath = Settings::theme();
    for (const Theme &tmpTheme : m_themes) {
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
    KConfigGroup lastGroup(&savegame, QStringLiteral("Game"));
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
    KGameStandardAction::gameNew(this, &MainWindow::slotNewGame, actionCollection());
    KGameStandardAction::quit(this, &MainWindow::close, actionCollection());

    auto replayAct = new QAction(QIcon::fromTheme( QStringLiteral( "media-playback-start" )),
                            i18nc("@action", "&Replay"), this);
    actionCollection()->addAction( QStringLiteral( "replay" ), replayAct);
    replayAct->setToolTip(i18nc("@info:tooltip", "Replay game"));
    replayAct->setWhatsThis(i18nc("@info:whatsthis", "Replays your last game for you to watch."));
    replayAct->setEnabled(false);

    m_hintAct = KGameStandardAction::hint(this, &MainWindow::hint, actionCollection());
    m_hintAct->setEnabled(false);

    auto animAct = new KToggleAction(i18nc("@option:check", "&Animation"),this);
    actionCollection()->addAction( QStringLiteral( "animation" ), animAct);
    animAct->setChecked(m_animate);
    connect(animAct, &QAction::toggled, this, &MainWindow::setAnimation);

    m_themeAct = new KSelectAction(i18nc("@title:menu", "Theme"), this);
    QStringList themes;
    for (const Theme &theme : std::as_const(m_themes)) {
        themes << theme.name();
    }
    m_themeAct->setItems(themes);
    int themeId = 0;
    for (const Theme &theme : std::as_const(m_themes)) {
        if (theme.path() == m_theme.path()) {
            themeId = theme.id();
            break;
        }
    }
    m_themeAct->setCurrentItem(themeId);
    actionCollection()->addAction( QStringLiteral( "themes" ), m_themeAct);
    m_themeAct->setIcon(QIcon::fromTheme( QStringLiteral( "games-config-theme" )));
    connect(m_themeAct, &KSelectAction::indexTriggered, this, &MainWindow::changeTheme);

    m_undoAct = KGameStandardAction::undo(this, &MainWindow::slotUndo, actionCollection());
    m_undoAct->setEnabled(false);

    addAction(replayAct);
    addAction(animAct);
    addAction(m_themeAct);
}

void MainWindow::hint() {
    if (m_game != nullptr) {
        if (!m_game->computerTurn()) {
            if (m_demoAi != nullptr) {
                m_demoAi->slotMove();
            }
        }
    }
}

void MainWindow::setAnimation(bool enabled) {
    if (m_scene != nullptr) {
        if (enabled != m_animate) {
            m_scene->enableAnimation(enabled);
        }
    }
    m_animate = enabled;
    saveSettings();
}

void MainWindow::slotNewGame() {
    m_demoMode = false;
    if (m_game != nullptr) {
        m_game->cancelAndWait();
        if (m_scene != nullptr) {
            disconnect(m_game, nullptr, m_scene, nullptr);
        }
        if (!m_game->isGameOver() && m_game->history().size() > 1) {
            m_lossesLabel->setText(i18n("Losses: %1",++m_losses));
        }
        if (m_game->history().size() > 1) {
            m_computerStarts = !m_computerStarts;
        }
        m_game->deleteLater();
        m_game = nullptr;
    }
    if (m_demoAi != nullptr) {
        m_demoAi->cancelAndWait();
        m_demoAi->deleteLater();
        m_demoAi = nullptr;
    }
    QAction* act = actionCollection()->action(QStringLiteral("replay"));
    if (act != nullptr) {
        act->setEnabled(false);
    }
    if (m_scene == nullptr && (m_lastGame.isEmpty())) { //first time, demo time
        m_scene = new Scene(m_theme, m_animate);
        m_demoMode = true;
        slotNewDemo();
    } else {
        KGameDifficulty::global()->setGameRunning(true);

        Dimension dimension(NUMCOLS, NUMCOLS);
        if (m_scene == nullptr) {
            m_scene = new Scene(m_theme, m_animate);
            if (!m_lastGame.empty()) {
                QString tmp = m_lastGame.first();
                m_computerStarts = tmp.startsWith(QLatin1Char('2')) ? true : false;
            }
            m_game = new Game(dimension, m_lastGame, KGameDifficulty::globalLevel(),
                              m_playbackSpeed, m_aiFactory);
        } else {
            m_game = new Game(dimension, m_computerStarts ? O : X,
                              KGameDifficulty::globalLevel(), NotDemo, m_playbackSpeed,
                              m_aiFactory);
        }
        m_demoAi = m_aiFactory->createAi(dimension, KGameDifficultyLevel::Easy, m_game->player(), Demo);
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
    if (m_game != nullptr) {
        m_game->deleteLater();
        m_game = nullptr;
    }
    if (m_demoAi != nullptr) {
        m_demoAi->deleteLater();
        m_demoAi = nullptr;
    }
    Dimension dimension(NUMCOLS, NUMCOLS);
    m_game = new Game(dimension, O, KGameDifficulty::globalLevel(), Demo, m_playbackSpeed,
                      m_aiFactory);
    m_demoAi = m_aiFactory->createAi(dimension, KGameDifficulty::globalLevel(), X, Demo);
    m_scene->setGame(m_game);
    connect(m_game, &Game::boardChanged,
            m_demoAi, &Ai::changeBoard);
    connect(m_game, &Game::playerTurn, m_demoAi, &Ai::slotMove,
            Qt::QueuedConnection);
    connect(m_demoAi, SIGNAL(move(Move)),
            m_game,  SLOT(move(Move)));
    connect(m_game, &Game::gameOver,
            this, &MainWindow::slotNewDemoWait);
    statusBar()->showMessage(i18n("Start a new game to play."));
    m_game->start();
    KGameDifficulty::global()->setGameRunning(false);
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
    disconnect(m_game, nullptr, m_demoAi, nullptr);
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
    if (m_game == nullptr)
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
    disconnect(m_game, nullptr, this, nullptr);
    connect(m_game, &Game::replayEnd,
            this, &MainWindow::reEnableReplay);
    disconnect(m_game, nullptr, m_scene, nullptr);
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
    if (m_game!=nullptr)
        m_game->setSkill(KGameDifficulty::globalLevel());
}

void MainWindow::changeTheme(int themeId) {
    for (const Theme &theme : std::as_const(m_themes)) {
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
#include "moc_mainwindow.cpp"
