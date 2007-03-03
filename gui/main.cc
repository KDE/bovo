#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "mainwindow.h"
#include "version.h"

static const char description[] = I18N_NOOP("KDE Five in a Row Board Game");

using namespace gui;

int main(int argc, char **argv)
{
    KAboutData aboutData( "bovo", I18N_NOOP("Bovo"),
            BOVOCHESS_VERSION, description, KAboutData::License_GPL,
            "(c) 2002,2007 Aron Boström");
    aboutData.addAuthor("Aron Boström",I18N_NOOP("Author"), "aron.bostrom@gmail.com");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication application;
    if( application.isSessionRestored() )
        RESTORE(MainWindow)
    else {
        MainWindow *mainWin = new MainWindow;
        mainWin->show();
    }

    return application.exec();
}

