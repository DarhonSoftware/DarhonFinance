#include <QtGui>
#include <QtSql>
#include "mainwindow0.h"

QPointer<CMainWindow> g_pMainWindow;

int main(int argc, char *argv[])
{
    QApplication Application(argc, argv);

    //Setup translations
    QTranslator Translator;
    if (Translator.load(QLocale(), "darhonfinance", "_", ":/i18n")) {
        Application.installTranslator(&Translator);
    }

    //Set application names
    QCoreApplication::setOrganizationName("Darhon");
    QCoreApplication::setApplicationName("DarhonFinance");
    QCoreApplication::setApplicationVersion("1.5.0");
    QCoreApplication::setOrganizationDomain("www.darhon.com");

    //Validate database driver and create instance
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        g_messageWarning(0,
                         QCoreApplication::translate("Global",
                                                     "Sqlite database driver wasn't found"),
                         QCoreApplication::translate("Global",
                                                     "The Sqlite driver needs to be installed "
                                                     "before running the application."));
        return 1;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    //Create and set up main window
    g_pMainWindow = new CMainWindow;
    g_pMainWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    g_pMainWindow->addForm("CAccountsView");

    g_pMainWindow->show();

    return Application.exec();
}
