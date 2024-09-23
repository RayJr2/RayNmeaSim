#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "N2kTypes.h"
#include "convert.h"
#include "dataenums.h"
#include "dialogsetup.h"
#include "nmea2000handler.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_Setup_Form_triggered();
    void on_actionSend_Test_129026_triggered();

public slots:
    void PollTimerElapsed();

private:
    Ui::MainWindow *ui;
    Nmea2000Handler *nmea2000Handler;
    DialogSetup *dialogSetup;

    QTimer *pollTimer = new QTimer(this);

    void confSignalsSlots();
    void initilizeQtStyleIcons();
    void loadSettingGeometry();
    void loadSettings();
    void saveSettings();
    void initilizeOthers();
    void initilizeUnits();
    void initilizeGauges();

    QString styleSheetName = "darkeum";
};
#endif // MAINWINDOW_H
