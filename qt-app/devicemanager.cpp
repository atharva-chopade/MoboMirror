#include "devicemanager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
{
}

DeviceManager::~DeviceManager()
{
}

QString DeviceManager::getQvhPath() const
{
    // Try to find qvh binary
    QString appDir = QCoreApplication::applicationDirPath();
    
    // Check in parent directory (when running from qt-app/build)
    QString path = appDir + "/../qvh";
    if (QFile::exists(path)) {
        return path;
    }
    
    // Check in same directory
    path = appDir + "/qvh";
    if (QFile::exists(path)) {
        return path;
    }
    
    // Check two levels up (when running from qt-app/build)
    path = appDir + "/../../qvh";
    if (QFile::exists(path)) {
        return path;
    }
    
    // Default to system path
    return "qvh";
}

QList<DeviceInfo> DeviceManager::getDevices()
{
    QList<DeviceInfo> devices;
    
    QString program = getQvhPath();
    QStringList arguments;
    arguments << "devices";
    
    QProcess process;
    process.start(program, arguments);
    
    if (!process.waitForStarted(3000)) {
        qWarning() << "Failed to start qvh process";
        return devices;
    }
    
    if (!process.waitForFinished(5000)) {
        qWarning() << "qvh process timeout";
        process.kill();
        return devices;
    }
    
    QByteArray output = process.readAllStandardOutput();
    devices = parseDevicesOutput(output);
    
    return devices;
}

QList<DeviceInfo> DeviceManager::parseDevicesOutput(const QByteArray &output)
{
    QList<DeviceInfo> devices;
    
    // Parse JSON output from qvh devices command
    QJsonDocument doc = QJsonDocument::fromJson(output);
    
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Failed to parse JSON output:" << output;
        return devices;
    }
    
    QJsonObject root = doc.object();
    
    if (root.contains("device_list")) {
        QJsonArray deviceList = root["device_list"].toArray();
        
        for (const QJsonValue &value : deviceList) {
            if (value.isObject()) {
                QJsonObject deviceObj = value.toObject();
                
                DeviceInfo info;
                info.udid = deviceObj["serial_number"].toString();
                info.name = deviceObj["product_name"].toString();
                info.productName = deviceObj["product_name"].toString();
                info.isActivated = deviceObj["screen_mirroring_activated"].toBool();
                
                if (!info.udid.isEmpty()) {
                    devices.append(info);
                }
            }
        }
    }
    
    return devices;
}

bool DeviceManager::activateDevice(const QString &udid)
{
    if (udid.isEmpty()) {
        return false;
    }
    
    QString program = getQvhPath();
    QStringList arguments;
    arguments << "activate" << "--udid=" + udid;
    
    QProcess process;
    process.start(program, arguments);
    
    if (!process.waitForStarted(3000)) {
        qWarning() << "Failed to start activation process";
        return false;
    }
    
    if (!process.waitForFinished(10000)) {
        qWarning() << "Activation timeout";
        process.kill();
        return false;
    }
    
    return process.exitCode() == 0;
}

bool DeviceManager::deactivateDevice(const QString &udid)
{
    if (udid.isEmpty()) {
        return false;
    }
    
    QString program = getQvhPath();
    QStringList arguments;
    arguments << "deactivate" << "--udid=" + udid;
    
    QProcess process;
    process.start(program, arguments);
    
    if (!process.waitForStarted(3000)) {
        qWarning() << "Failed to start deactivation process";
        return false;
    }
    
    if (!process.waitForFinished(10000)) {
        qWarning() << "Deactivation timeout";
        process.kill();
        return false;
    }
    
    return process.exitCode() == 0;
}
