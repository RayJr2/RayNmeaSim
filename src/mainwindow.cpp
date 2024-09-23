#include "mainwindow.h"
#include <QAction>
#include <QColor>
#include <QDesktopServices>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QRegularExpression>
#include <QSettings>
#include <QShowEvent>
#include <QTextStream>
#include <QThread>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , nmea2000Handler(new Nmea2000Handler(this))
{
    ui->setupUi(this);

    loadSettings();
    confSignalsSlots();
    initilizeOthers();
    initilizeUnits();
    initilizeGauges();
    initilizeQtStyleIcons();
    loadSettingGeometry();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    if (event->isAccepted()) {
    }

    QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (event->isAccepted()) {
        saveSettings();
    }

    QMainWindow::closeEvent(event);
}

bool MainWindow::event(QEvent *event)
{
    return QMainWindow::event(event);
}

void MainWindow::PollTimerElapsed() {}

void MainWindow::confSignalsSlots() {}

void MainWindow::initilizeQtStyleIcons() {}

void MainWindow::loadSettingGeometry() {}

void MainWindow::loadSettings()
{
    QSettings settings(APP_COMPANY, APP_NAME);

    settings.beginGroup("Preference");
    styleSheetName = settings.value("StyleSheetName", "darkeum").toString();
    QString appPath = QApplication::applicationDirPath();
    if (styleSheetName != "None") {
        //Set the app style sheet
        //Sytle sheet downloaded from https://qss-stock.devsecstudio.com/templates.php
        QString StyleSheetFileName = "/stylesheets/" + styleSheetName + "/" + styleSheetName + ".qss";
        qInfo() << "AppPath" << appPath;
        qInfo() << "Stylesheet" << StyleSheetFileName;
        QFile styleSheetFile(appPath + StyleSheetFileName);
        styleSheetFile.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(styleSheetFile.readAll());
        styleSheet.replace("%%APP_FOLDER%%", appPath);
        this->setStyleSheet(styleSheet);
    } else {
        this->setStyleSheet("");
    }
    settings.endGroup();
}

void MainWindow::saveSettings() {}

void MainWindow::initilizeOthers()
{
    // Initialize the Nmea2000Handler
    nmea2000Handler->InitializeActisense("COM6");
}

void MainWindow::initilizeUnits() {}

void MainWindow::initilizeGauges() {}

void MainWindow::on_actionOpen_Setup_Form_triggered()
{
    dialogSetup->setStyleSheet(this->styleSheet());
    dialogSetup->setFont(this->font());
    dialogSetup->exec();
}

void MainWindow::on_actionSend_Test_129026_triggered()
{
    double cog = Convert::DegreesToRadians(45.0);
    double sog = Convert::MphToMetersPerSecond(20);

    nmea2000Handler->SendPgn129026(cog, sog);
}

