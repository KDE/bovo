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

#include <klocalizedstring.h>

static const char description[] = I18N_NOOP("KDE Five in a Row Board Game");
static const char copyleft[] = I18N_NOOP("(c) 2002-2007, Aron Boström");

using namespace gui;

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("bovo");

    KAboutData aboutData(QStringLiteral("bovo"), i18n("Bovo"),
            QStringLiteral("BOVO_VERSION"), i18n(description), KAboutLicense::GPL,
            i18n(copyleft), QString(), QStringLiteral("http://games.kde.org/bovo"));
    aboutData.addAuthor(i18n("Aron Boström"),i18n("Author"),
                        "aron.bostrom@gmail.com");

    aboutData.setOrganizationDomain(QByteArray("kde.org"));
    aboutData.setProgramIconName(QStringLiteral("bovo"));
    aboutData.setProductName(QByteArray("bovo"));
	    
    KAboutData::setApplicationData(aboutData);
	     
    app.setApplicationDisplayName(aboutData.displayName());
    app.setOrganizationDomain(aboutData.organizationDomain());
    app.setApplicationVersion(aboutData.version());
		      
    if( app.isSessionRestored() ) {
        RESTORE(MainWindow);
    } else {
        MainWindow *mainWin = new MainWindow();
        mainWin->show();
    }

    return app.exec();
}
