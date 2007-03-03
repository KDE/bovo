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
//#include <kselectaction.h>

#include <QGridLayout>
#include <QWidget>

namespace gui {

MainWindow::MainWindow(QWidget* parent) : KMainWindow(parent), m_scene(0), m_game(0) {
    statusBar()->insertItem( i18n("Your turn."), 0 );
    slotNewGame();

    QWidget *mainWid = new QWidget;
    QGridLayout *lay = new QGridLayout(mainWid);
    lay->setColumnStretch( 0, 1 );
    lay->setMargin(1);
    m_view = new View(m_scene, mainWid);
    m_view->show();
    lay->addWidget(m_view, 0, 0, 2, 1);
    m_scene->setBackground("bg.svg");

    setupActions();
    setCentralWidget(mainWid);
    setupGUI();
}

void MainWindow::setupActions() {
    QAction *newGameAct = actionCollection()->addAction(KStandardAction::New, "new_game", this, SLOT(slotNewGame()));
    QAction *quitAct = actionCollection()->addAction(KStandardAction::Quit, "quit", this, SLOT(close()));

    addAction(newGameAct);
    addAction(quitAct);
}

void MainWindow::slotNewGame() {
    delete m_game;
    m_game = new Game;
    connect( m_game, SIGNAL(gameOver()), SLOT(slotGameOver()) );

    if(m_scene == 0) { //first time
        m_scene = new Scene(m_game);
        connect( m_scene, SIGNAL(moveFinished()), SLOT(slotMoveFinished()) );
        connect( m_game, SIGNAL(moveFinished()), SLOT(slotMoveFinished()) );
    } else {
        m_scene->setGame( m_game );
    }

    statusBar()->changeItem( i18n("Your turn."), 0 );
}

void MainWindow::slotGameOver() {
    statusBar()->changeItem( i18n("GAME OVER."), 0 );
    QString message = m_game->lastMove().p == X ? i18n("You won!") : i18n("You Lost!");

    KMessageBox::information( this, message, i18n("Game over") );
}

void MainWindow::slotMoveFinished() {
    statusBar()->changeItem( m_game->isComputersTurn() ? i18n("Computer's turn.") : i18n("Your turn."), 0 );
}

} //namespace gui

#include "mainwindow.moc"
