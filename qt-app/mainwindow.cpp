#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , deviceManager(new DeviceManager(this))
    , videoWidget(nullptr)
    , mirrorProcess(nullptr)
    , isMirroring(false)
    , connectionType(0)
{
    ui->setupUi(this);
    
    setWindowTitle("MoboMirror - iPhone Screen Mirroring");
    resize(1024, 768);
    
    // Create video widget
    videoWidget = new VideoWidget(this);
    setCentralWidget(videoWidget);
    
    // Create toolbar widgets
    QLabel *deviceLabel = new QLabel("Device:", this);
    ui->toolBar->addWidget(deviceLabel);
    
    // Device combo box
    QComboBox *deviceComboBox = new QComboBox(this);
    deviceComboBox->setMinimumWidth(250);
    deviceComboBox->setObjectName("deviceComboBox");
    ui->toolBar->addWidget(deviceComboBox);
    
    ui->toolBar->addSeparator();
    
    QLabel *connectionLabel = new QLabel("Connection:", this);
    ui->toolBar->addWidget(connectionLabel);
    
    // Connection type combo box
    QComboBox *connectionTypeComboBox = new QComboBox(this);
    connectionTypeComboBox->addItem("USB");
    connectionTypeComboBox->addItem("Wireless");
    connectionTypeComboBox->setObjectName("connectionTypeComboBox");
    ui->toolBar->addWidget(connectionTypeComboBox);
    
    ui->toolBar->addSeparator();
    
    // Refresh button
    QPushButton *refreshButton = new QPushButton("Refresh Devices", this);
    refreshButton->setObjectName("refreshButton");
    ui->toolBar->addWidget(refreshButton);
    
    ui->toolBar->addSeparator();
    
    // Start button
    QPushButton *startButton = new QPushButton("Start Mirroring", this);
    startButton->setObjectName("startButton");
    ui->toolBar->addWidget(startButton);
    
    // Stop button
    QPushButton *stopButton = new QPushButton("Stop Mirroring", this);
    stopButton->setObjectName("stopButton");
    ui->toolBar->addWidget(stopButton);
    
    // Status label in status bar
    QLabel *statusLabel = new QLabel("Status: Ready", this);
    statusLabel->setObjectName("statusLabel");
    ui->statusbar->addWidget(statusLabel);
    
    // Connect signals
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshDevices);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartMirroring);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopMirroring);
    connect(deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onDeviceSelected);
    connect(connectionTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onConnectionTypeChanged);
    
    // Initial device list update
    updateDeviceList();
    updateUI();
}

MainWindow::~MainWindow()
{
    if (mirrorProcess) {
        mirrorProcess->kill();
        mirrorProcess->waitForFinished();
    }
    delete ui;
}

void MainWindow::onRefreshDevices()
{
    updateDeviceList();
}

void MainWindow::onStartMirroring()
{
    if (selectedDeviceUdid.isEmpty()) {
        QMessageBox::warning(this, "No Device", "Please select a device first.");
        return;
    }
    
    if (connectionType == 0) {
        startUSBMirroring();
    } else {
        startWirelessMirroring();
    }
}

void MainWindow::onStopMirroring()
{
    if (mirrorProcess) {
        mirrorProcess->terminate();
        if (!mirrorProcess->waitForFinished(3000)) {
            mirrorProcess->kill();
        }
        mirrorProcess = nullptr;
    }
    
    isMirroring = false;
    updateUI();
    videoWidget->clearScreen();
    
    QLabel *statusLabel = findChild<QLabel*>("statusLabel");
    if (statusLabel) statusLabel->setText("Status: Stopped");
}

void MainWindow::onDeviceSelected(int index)
{
    QComboBox *deviceComboBox = findChild<QComboBox*>("deviceComboBox");
    
    if (deviceComboBox && index >= 0 && index < deviceComboBox->count()) {
        selectedDeviceUdid = deviceComboBox->itemData(index).toString();
        qDebug() << "Selected device UDID:" << selectedDeviceUdid;
    }
}

void MainWindow::onConnectionTypeChanged(int index)
{
    connectionType = index;
    qDebug() << "Connection type changed to:" << (index == 0 ? "USB" : "Wireless");
}

void MainWindow::updateDeviceList()
{
    QLabel *statusLabel = findChild<QLabel*>("statusLabel");
    QComboBox *deviceComboBox = findChild<QComboBox*>("deviceComboBox");
    
    if (statusLabel) statusLabel->setText("Status: Discovering devices...");
    
    QList<DeviceInfo> devices = deviceManager->getDevices();
    
    if (deviceComboBox) {
        deviceComboBox->clear();
        
        if (devices.isEmpty()) {
            deviceComboBox->addItem("No devices found");
            if (statusLabel) statusLabel->setText("Status: No devices found");
            selectedDeviceUdid.clear();
        } else {
            for (const DeviceInfo &device : devices) {
                QString displayName = QString("%1 (%2)").arg(device.name, device.udid);
                deviceComboBox->addItem(displayName, device.udid);
            }
            selectedDeviceUdid = devices.first().udid;
            if (statusLabel) statusLabel->setText(QString("Status: Found %1 device(s)").arg(devices.size()));
        }
    }
    
    updateUI();
}

void MainWindow::updateUI()
{
    bool hasDevice = !selectedDeviceUdid.isEmpty();
    
    QPushButton *startButton = findChild<QPushButton*>("startButton");
    QPushButton *stopButton = findChild<QPushButton*>("stopButton");
    QComboBox *deviceComboBox = findChild<QComboBox*>("deviceComboBox");
    QComboBox *connectionTypeComboBox = findChild<QComboBox*>("connectionTypeComboBox");
    QPushButton *refreshButton = findChild<QPushButton*>("refreshButton");
    
    if (startButton) startButton->setEnabled(hasDevice && !isMirroring);
    if (stopButton) stopButton->setEnabled(isMirroring);
    if (deviceComboBox) deviceComboBox->setEnabled(!isMirroring);
    if (connectionTypeComboBox) connectionTypeComboBox->setEnabled(!isMirroring);
    if (refreshButton) refreshButton->setEnabled(!isMirroring);
}

void MainWindow::startUSBMirroring()
{
    QLabel *statusLabel = findChild<QLabel*>("statusLabel");
    if (statusLabel) statusLabel->setText("Status: Starting USB mirroring...");
    
    // Create process
    mirrorProcess = new QProcess(this);
    
    // Connect process signals
    connect(mirrorProcess, &QProcess::readyReadStandardOutput, 
            this, &MainWindow::onProcessOutput);
    connect(mirrorProcess, &QProcess::readyReadStandardError, 
            this, &MainWindow::onProcessError);
    connect(mirrorProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);
    
    // Build command
    QString program = QCoreApplication::applicationDirPath() + "/../qvh";
    QStringList arguments;
    arguments << "gstreamer" << "--udid=" + selectedDeviceUdid << "-v";
    
    qDebug() << "Starting process:" << program << arguments;
    
    mirrorProcess->start(program, arguments);
    
    if (!mirrorProcess->waitForStarted(5000)) {
        QMessageBox::critical(this, "Error", 
            "Failed to start mirroring process.\nMake sure the qvh binary is available.");
        delete mirrorProcess;
        mirrorProcess = nullptr;
        if (statusLabel) statusLabel->setText("Status: Failed to start");
        return;
    }
    
    isMirroring = true;
    updateUI();
    if (statusLabel) statusLabel->setText("Status: Mirroring via USB");
}

void MainWindow::startWirelessMirroring()
{
    // Wireless mirroring would use go-ios tunnel functionality
    // For now, show a message that it's not yet fully implemented
    QMessageBox::information(this, "Wireless Mirroring", 
        "Wireless mirroring support is in development.\n"
        "Currently, USB mirroring is fully supported.\n\n"
        "To use wireless mirroring:\n"
        "1. Ensure device is paired and on the same network\n"
        "2. Use go-ios tunnel functionality\n"
        "3. Connect via usbmuxd over network");
    
    // For demonstration, fall back to USB
    startUSBMirroring();
}

void MainWindow::onProcessOutput()
{
    if (mirrorProcess) {
        QByteArray output = mirrorProcess->readAllStandardOutput();
        qDebug() << "Process output:" << output;
        
        // Parse JSON output if available
        QJsonDocument doc = QJsonDocument::fromJson(output);
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("msg")) {
                QString msg = obj["msg"].toString();
                QLabel *statusLabel = findChild<QLabel*>("statusLabel");
                if (statusLabel) statusLabel->setText("Status: " + msg);
            }
        }
    }
}

void MainWindow::onProcessError()
{
    if (mirrorProcess) {
        QByteArray error = mirrorProcess->readAllStandardError();
        qDebug() << "Process error:" << error;
    }
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Process finished with code:" << exitCode << "status:" << exitStatus;
    
    isMirroring = false;
    updateUI();
    
    QLabel *statusLabel = findChild<QLabel*>("statusLabel");
    
    if (exitStatus == QProcess::CrashExit) {
        if (statusLabel) statusLabel->setText("Status: Process crashed");
        QMessageBox::warning(this, "Process Error", "Mirroring process crashed unexpectedly.");
    } else if (exitCode != 0) {
        if (statusLabel) statusLabel->setText(QString("Status: Process exited with code %1").arg(exitCode));
    } else {
        if (statusLabel) statusLabel->setText("Status: Stopped");
    }
    
    if (mirrorProcess) {
        mirrorProcess->deleteLater();
        mirrorProcess = nullptr;
    }
}
