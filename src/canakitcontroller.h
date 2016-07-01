/*
 * CanaKitController.h
 *
 *  Created on: Nov 17, 2011
 *      Author: PManandhar
 */
#ifndef CANAKITCONTROLLER_H
#define CANAKITCONTROLLER_H

#include <qextserialport.h>
#include <qextserialenumerator.h>
#include <QMutex>
#include <QThread>

#include "Global.h"
#include "qanameasurement.h"

class CanaKitController: public QThread
{
    Q_OBJECT
public:
    CanaKitController();
    ~CanaKitController();

    bool isConnected();
    QString getPortName ();
    bool reconnect();
    void close();
    bool getRelayState (int i);
    void setRelayState (int i, bool state);
    double getDAQ (int i);
    QanaMeasurement getMeasurement();

signals:
    void relaysUpdated();
    void daqUpdated();

protected:
    void run();

private:

    void _connect();
    void _closePort();
    void _readRelays();
    void _readDAQ();
    void _setDAQMode();

    volatile bool _isConnected;
    QextPortInfo _portInfo;
    QextSerialPort * _port;
    QanaMeasurement measurement;

    QString _readLine();

    QMutex mutex;
};

#endif // CANAKITCONTROLLER_H
