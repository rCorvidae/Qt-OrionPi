#include "AsyncDeviceFinder.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QVariant>
#include <QDebug>
#include "DefaultSerialSettings.h"
#include "SerialFactory.h"

namespace {
    constexpr char PROPERTY_READ_ATTEMPT[] = "PROPERTY_READ_ATTEMPT";
}


AsyncDeviceFinder::AsyncDeviceFinder(IfceDeviceFactory *deviceFactory, QObject *parent)
    : IfceSerialFinder(parent),
      deviceFactory(deviceFactory),
      settings{QSharedPointer<DefaultSerialSettings>::create()}
{    
}

AsyncDeviceFinder::~AsyncDeviceFinder()
{
    for( auto serial : serials.values() )
        if( serial && serial->isOpen() )
            serial->close();
}

QList<QSharedPointer<IfceDevice>> AsyncDeviceFinder::discover()
{
    for( auto availableSerial : QSerialPortInfo::availablePorts() ) {
        availablePortNames.append(availableSerial.portName());
        prepare_serial(availableSerial);
    }

    return QList<QSharedPointer<IfceDevice>>();
}

void AsyncDeviceFinder::setSerialSettings(QSharedPointer<IfceSerialSettings> settings)
{
    this->settings = settings;
}

void AsyncDeviceFinder::onReadyRead()
{
    QPointer<QSerialPort> serial = qobject_cast<QSerialPort*>(sender());
    if( !serial ) return;
    if( !serial->canReadLine() ) {
        emit signalSerialError(QString("%1: cannot read a line.").arg(serial->portName()));
        return;
    }

    const auto portName { serial->portName() };
    auto sharedSerial { serials.value(portName) };
    auto line { sharedSerial->readLine() };
    const auto serialInfo { availableSerials.value(portName) };

    QSharedPointer<IfceDevice> device;
    try {
        device = deviceFactory->create(line, serialInfo, settings);
    } catch( ParsingException &e ) {
        emit signalSerialError(QString("%1: %2").arg(portName).arg(e.what()));
        action_parsing_error(sharedSerial);
        return;
    }

    emit signalDeviceFound(device);
    unregister_port(serial->portName());
    check_discovery_finished();
    sharedSerial->close();
    sharedSerial.clear();
}

void AsyncDeviceFinder::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if( error == QSerialPort::NoError )
        return;

    QPointer<QSerialPort> serial { qobject_cast<QSerialPort*>(sender()) };
    auto sharedSerial { serials.value(serial->portName()) };
    emit signalSerialError(QString("%1: %2").arg(sharedSerial->portName()).arg(sharedSerial->errorString()));

    unregister_port(serial->portName());
    check_discovery_finished();

    if( sharedSerial->isOpen() )
        sharedSerial->close();
    sharedSerial.clear();     //delete later
}

void AsyncDeviceFinder::prepare_serial(const QSerialPortInfo &serialInfo)
{
    SerialFactory serialFactory;
    serialFactory.setSerialSettings(settings);
    constexpr auto START_READ_LINE_ATTEMPT {0};

    auto serial { serialFactory.create(serialInfo, true) };
    connect(serial.data(), &QSerialPort::readyRead, this, &AsyncDeviceFinder::onReadyRead);
    connect(serial.data(), &QSerialPort::errorOccurred, this, &AsyncDeviceFinder::onErrorOccurred);
    serial->setProperty(PROPERTY_READ_ATTEMPT, START_READ_LINE_ATTEMPT);
    serials.insert(serialInfo.portName(), serial);
    availableSerials.insert(serialInfo.portName(), serialInfo);

    serial->open(QIODevice::ReadOnly);
}

void AsyncDeviceFinder::action_parsing_error(QSharedPointer<QSerialPort> serial)
{
    auto attemptCount { serial->property(PROPERTY_READ_ATTEMPT).toInt() };
    const auto MAX_READLINE_ATTEMPT { settings->getAttemptsForReadLine() };
    if( ++attemptCount < MAX_READLINE_ATTEMPT ) {
        serial->setProperty(PROPERTY_READ_ATTEMPT, attemptCount);
        return;
    }

    unregister_port(serial->portName());
    check_discovery_finished();
    serial->close();
    serial.clear();
}

void AsyncDeviceFinder::unregister_port(const QString &portName)
{
    availablePortNames.removeOne(portName);
}

void AsyncDeviceFinder::check_discovery_finished()
{
    if( availablePortNames.isEmpty() )
        emit signalDiscoveryFinished();
}
