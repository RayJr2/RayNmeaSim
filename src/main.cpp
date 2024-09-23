#include "mainwindow.h"

#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QScrollArea>
#include <QSettings>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Set application settings
    QApplication::setApplicationName(APP_NAME);
    QApplication::setOrganizationName(APP_COMPANY);
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setOrganizationDomain("rfstateside.com");
    qInfo() << "Software Version:" << APP_VERSION;

    QSettings settings(APP_COMPANY, APP_NAME);

    // Handle style sheet settings
    settings.beginGroup("Preference");
    QString styleSheetName = settings.value("StyleSheetName", "darkeum").toString();
    settings.endGroup();

    QString appPath = QApplication::applicationDirPath();
    qDebug() << "Application Path:" << appPath;
    if (styleSheetName != "None") {
        QString styleSheetFileName = "/stylesheets/" + styleSheetName + "/" + styleSheetName + ".qss";
        QFile styleSheetFile(appPath + styleSheetFileName);
        if (styleSheetFile.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(styleSheetFile.readAll());
            styleSheet.replace("%%APP_FOLDER%%", appPath);
            a.setStyleSheet(styleSheet);
        }
    } else {
        a.setStyleSheet("");
    }

    w.show();
    return a.exec();
}
