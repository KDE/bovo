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

#include <QApplication>

#include <kaboutdata.h>
#include <kdelibs4configmigrator.h>
#include <klocalizedstring.h>

#include "mainwindow.h"
#include "version.h"

static const char description[] = I18N_NOOP("KDE Five in a Row Board Game");
static const char copyleft[] = I18N_NOOP("(c) 2002-2007, Aron Boström");

using namespace gui;

int main(int argc, char **argv) {
    Kdelibs4ConfigMigrator migrate(QStringLiteral("bovo"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("bovorc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("bovoui.rc"));
    migrate.migrate();

    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("bovo");

    KAboutData aboutData(QStringLiteral("bovo"), i18n("Bovo"),
            QStringLiteral("BOVO_VERSION"), i18n(description), KAboutLicense::GPL,
            i18n(copyleft), QString(), QStringLiteral("http://games.kde.org/bovo"));
    aboutData.addAuthor(i18n("Aron Boström"),i18n("Author"),
                        "aron.bostrom@gmail.com");

    aboutData.setOrganizationDomain(QByteArray("kde.org"));
    app.setWindowIcon(QIcon::fromTheme(QLatin1String("bovo")));
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
