#ifndef ORIONENGINE_H
#define ORIONENGINE_H
#include <memory>

#include <QObject>
#include <QDebug>
#include <QScopedPointer>
#include <QScopedPointerObjectDeleteLater>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

#include "include/DeviceFinder.h"
#include "include/ThreadedDeviceFinder.h"
#include "include/JsonDeviceFactory.h"
#include "include/SerialManager.h"
#include "include/SerialController.h"
#include "Orion/Drive/WheelModel.h"
#include "Orion/Drive/JsonDriveModeDirect.h"
#include "Orion/Drive/ChassisModel.h"
#include "include/IfceServer.h"
#include "inputs/IfceInputSource.h"

class QTimer;


class OrionEngine : public QObject
{
    Q_OBJECT

public:
    explicit OrionEngine(QObject *parent = 0);
    ~OrionEngine();

signals:
    void aboutToClose();


private slots:
    void onRemoteMessageReceived(const QByteArray &message);
    void onFeedbackTimerTimeout();


private:
    void setup_serial();
    void setup_cmd_source();
    void connections();
    void echo_found_serials(const QList<QSharedPointer<IfceDevice> > &foundDevices);
    void echo_not_connected_devices(const QList<int> &notConnectedIds);
    void store_settings();

    QSharedPointer<SerialManager> serialManager;
    QSharedPointer<SerialController> serialController;
    QScopedPointer<Orion::ChassisModel> chassisModel;

    QScopedPointer<IfceInputSource> commandSource;
    QScopedPointer<QTimer> sendFeedbackTimer;
};

#endif // ORIONENGINE_H
