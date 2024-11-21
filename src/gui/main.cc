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

#include <QApplication>
#include <QCommandLineParser>
#include <KAboutData>
#include <KLocalizedString>
#include <KDBusService>
#include <KCrash>

#include "mainwindow.h"
#include "bovo_version.h"

#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif
using namespace gui;

int main(int argc, char **argv) {
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif

    QApplication app(argc, argv);
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("bovo"));

    KAboutData aboutData(QStringLiteral("bovo"), i18n("Bovo"),
            QStringLiteral(BOVO_VERSION_STRING), i18n("KDE Five in a Row Board Game"), KAboutLicense::GPL,
            i18n("(c) 2002-2007, Aron Boström"),
            QString(),
            QStringLiteral("https://apps.kde.org/bovo"));
    aboutData.addAuthor(i18n("Aron Boström"),i18n("Author"),
                        QStringLiteral("aron.bostrom@gmail.com"));

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("bovo")));

    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
    KDBusService service;
    if( app.isSessionRestored() ) {
        kRestoreMainWindows<MainWindow>();
    } else {
        auto mainWin = new MainWindow();
        mainWin->show();
    }

    return app.exec();
}
