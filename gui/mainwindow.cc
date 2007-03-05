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

#include "mainwindow.h"
#include "game.h"
#include "scene.h"
#include "view.h"
#include "commondefs.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kicon.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>

#include <QGridLayout>
#include <QWidget>

namespace gui {

MainWindow::MainWindow(QWidget* parent) : KMainWindow(parent), m_scene(0), m_game(0), m_wins(0), m_losses(0) {
    statusBar()->insertPermanentItem(i18n("Wins: %0").arg(m_wins), 1);
    statusBar()->insertPermanentItem(i18n("Losses: %0").arg(m_losses), 2);
    slotNewGame();

    QWidget *mainWid = new QWidget;
    QGridLayout *lay = new QGridLayout(mainWid);
    lay->setColumnStretch( 0, 1 );
    lay->setMargin(1);
    m_view = new View(m_scene, mainWid);
    m_view->show();
    lay->addWidget(m_view, 0, 0, 2, 1);

    // read scribble from some configuration, I guess, as well as name of bg file
    QString bgFilename = KStandardDirs::locate("appdata", QString("themes/%1/pics/%2").arg("scribble").arg("bg.svg"));
    m_scene->setBackground(bgFilename);

    setupActions();
    setCentralWidget(mainWid);
    setupGUI();
}

void MainWindow::setupActions() {
    QAction *newGameAct = actionCollection()->addAction(KStandardAction::New, "new_game", this, SLOT(slotNewGame()));
    QAction *quitAct = actionCollection()->addAction(KStandardAction::Quit, "quit", this, SLOT(close()));
    QAction *replayAct = actionCollection()->addAction(KStandardAction::Next, "go_next", this, SLOT(reload()));
    replayAct->setText("&Replay");
    replayAct->setIconText("&Replay");
    replayAct->setToolTip("Replays game");
    replayAct->setStatusTip("Replay game");
    replayAct->setWhatsThis("Replays your last game for you to watch.");

    addAction(newGameAct);
    addAction(quitAct);
    addAction(replayAct);
}

void MainWindow::slotNewGame() {
    if (m_game != 0) {
      if (!m_game->isGameOver()) statusBar()->changeItem(i18n("Losses: %0").arg(++m_losses), 2);
    }
    delete m_game;
    m_game = new Game;
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );
    QAction* act = actionCollection()->action("go_next");
    if (act == 0)
      act = actionCollection()->action("&Replay");
    if (act != 0) 
      act->setEnabled(false);
    qDebug() << "Replay: " << act << endl;

    if(m_scene == 0) { //first time
        m_scene = new Scene(m_game);
        connect( m_scene, SIGNAL(moveFinished()), SLOT(slotMoveFinished()) );
    } else {
        m_scene->setGame( m_game );
    }
    connect( m_game, SIGNAL(moveFinished()), SLOT(slotMoveFinished()) );
    statusBar()->showMessage(i18n("New game and it's your turn."));
}

void MainWindow::slotGameOver() {
    statusBar()->changeItem( i18n("GAME OVER."), 0 );
    QString message;
    if (m_game->lastMove().p == X) {
      statusBar()->showMessage(i18n("GAME OVER. You won!"));
      statusBar()->changeItem(i18n("Wins: %0").arg(++m_wins), 1);
      message = i18n("You won!");
    } else {
      statusBar()->showMessage(i18n("GAME OVER. You lost!"));
      statusBar()->changeItem(i18n("Losses: %0").arg(++m_losses), 2);
      message = i18n("You lost!");
    }
    m_scene->setWin();
    actionCollection()->action("go_next")->setEnabled(true);
    connect(actionCollection()->action("go_next"), SIGNAL(triggered()), this, SLOT(replay()));
    KMessageBox::information(this, message, i18n("Game over"));
}

void MainWindow::slotMoveFinished() {
    if (!m_game->isGameOver())
      statusBar()->showMessage( m_game->isComputersTurn() ? i18n("Waiting for computer.") : i18n("It's your turn."), 0 );
}

void MainWindow::replay() {
    if (!m_game->isGameOver()) return;
    statusBar()->showMessage("Replaying game");
    actionCollection()->action("go_next")->setEnabled(false);
    //clear scene
    //reinsert everything with delays
    // at the same time handle collisions with actNewGame and actQuit
    //reset actions
    actionCollection()->action("go_next")->setEnabled(true);
}

} //namespace gui

#include "mainwindow.moc"
