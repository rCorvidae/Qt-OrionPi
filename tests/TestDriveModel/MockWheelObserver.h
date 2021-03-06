#ifndef MOCKWHEELOBSERVER_H
#define MOCKWHEELOBSERVER_H
#include <QByteArray>
#include <QDebug>
#include <QHash>

#include "include/interface/IfceSerialObserver.h"

class   MockWheelObserver : public IfceSerialObserver
{
    QHash<int, QByteArray> dataById;
public:
    ~MockWheelObserver()
    {}
    void send(int id, const QByteArray &data) override
    {
        dataById[id] = data;
    }

    QByteArray getRawData(int id) const
    {
        return dataById.value(id);
    }
};

#endif // MOCKWHEELOBSERVER_H
