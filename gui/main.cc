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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "mainwindow.h"
#include "version.h"

static const char description[] = I18N_NOOP("KDE Five in a Row Board Game");

using namespace gui;

int main(int argc, char **argv) {
    KAboutData aboutData("bovo", 0, ki18n("Bovo"),
            BOVO_VERSION, ki18n(description), KAboutData::License_GPL,
            ki18n("(c) 2002,2007 Aron Boström"));
    aboutData.addAuthor(ki18n("Aron Boström"),ki18n("Author"),
                        "aron.bostrom@gmail.com");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication application;

    if( application.isSessionRestored() ) {
        RESTORE(MainWindow);
    } else {
        MainWindow *mainWin = new MainWindow();
        mainWin->show();
    }

    return application.exec();
}
