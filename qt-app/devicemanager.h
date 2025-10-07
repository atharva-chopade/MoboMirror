#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QProcess>

struct DeviceInfo {
    QString udid;
    QString name;
    QString productName;
    bool isActivated;
};

class DeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();
    
    QList<DeviceInfo> getDevices();
    bool activateDevice(const QString &udid);
    bool deactivateDevice(const QString &udid);

private:
    QString getQvhPath() const;
    QList<DeviceInfo> parseDevicesOutput(const QByteArray &output);
};

#endif // DEVICEMANAGER_H
