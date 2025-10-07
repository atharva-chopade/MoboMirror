#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include "devicemanager.h"
#include "videowidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRefreshDevices();
    void onStartMirroring();
    void onStopMirroring();
    void onDeviceSelected(int index);
    void onConnectionTypeChanged(int index);
    void onProcessOutput();
    void onProcessError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void updateDeviceList();
    void updateUI();
    void startUSBMirroring();
    void startWirelessMirroring();
    
    Ui::MainWindow *ui;
    DeviceManager *deviceManager;
    VideoWidget *videoWidget;
    QProcess *mirrorProcess;
    bool isMirroring;
    QString selectedDeviceUdid;
    int connectionType; // 0 = USB, 1 = Wireless
};

#endif // MAINWINDOW_H
