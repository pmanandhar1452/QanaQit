/*
 * CanaKitController.h
 *
 *  Created on: Nov 17, 2011
 *      Author: PManandhar
 */
#include "canakitcontroller.h"
#include <QDebug>

#include "LoggerTime.h"

CanaKitController::CanaKitController(): _port(NULL)
{
    //reconnect();
}

CanaKitController::~CanaKitController()
{
    _isConnected = false;
    wait();
    _closePort();
}

bool CanaKitController::reconnect()
{
    _closePort();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    int iCanaKit = -1;
    for (int i = 0; i < ports.size(); i++) {
        qDebug() << "Found " << ports.at(i).friendName;
        if (ports.at(i).friendName.contains(QString("Cana Kit"))) {
            iCanaKit = i;
            break;
        }
    }
    if (iCanaKit != -1) {
        _portInfo = ports[iCanaKit];
        _connect();
    }
    return isConnected();
}

bool CanaKitController::isConnected()
{
    if (_port != NULL && _port->isOpen())
        _isConnected = true;
    else _isConnected = false;
    return _isConnected;
}

QString CanaKitController::getPortName()
{
    if (_isConnected) return _portInfo.portName;
    else return "NOT CONNECTED";
}

void CanaKitController::_connect()
{
    _port = new QextSerialPort(
                _portInfo.portName,
                         QextSerialPort::Polling);
    _port->setBaudRate(BAUD9600);
    _port->setFlowControl(FLOW_XONXOFF);
    _port->setParity(PAR_NONE);
    _port->setDataBits(DATA_8);
    _port->setStopBits(STOP_1);
//        //set timeouts to 500 ms
    _port->setTimeout(500);
    qDebug() << "Opening Port";
    _port->open(QIODevice::ReadWrite|QIODevice::Unbuffered);
    if(_port->isOpen())
        _setDAQMode();

}

void CanaKitController::_closePort()
{
    qDebug() << "Closing Port";
    if (_port != NULL) {
        _port->close();
        delete _port;
        _isConnected = false;
    }
}

void CanaKitController::run()
{
    while(_isConnected)
    {
        int t_start = LoggerTime::timer();
        _readRelays();
        _readDAQ();
        usleep(100000);
        int t_end = LoggerTime::timer();
        measurement.time = (t_start + t_end)/2;
    }
    //this->exit(0);
}

void CanaKitController::_readRelays()
{
    //qDebug() << "Reading Relays";
    QString echo;
    QString strRelay = "RELS.GET \n\r";
    mutex.lock();
    _port->write(strRelay.toAscii(), strRelay.length());
    usleep(10000);
    QRegExp regx("([01] [01] [01] [01])");
    while(_port->bytesAvailable())
    {
        echo = _readLine();
        QString strEcho(echo);
        //qDebug() << strEcho;
        if (regx.indexIn(strEcho) >= 0) {
            //qDebug() << "Matched";
            for(int i = 0; i < Global::N_RELAYS; i++)
                measurement.relays[i] = regx.cap(1).at(i*2) == '1';
        }
    }
    mutex.unlock();
    emit relaysUpdated();
}

void CanaKitController::close()
{
    _closePort();
}

bool CanaKitController::getRelayState(int i)
{
    if (i >= 0 && i < Global::N_RELAYS)
        return measurement.relays[i];
    return false;
}

void CanaKitController::_setDAQMode()
{
    QByteArray echo;
    QString strInit = "\n\r";
    _port->write(strInit.toAscii(), strInit.length());
    usleep(10000);
    while(_port->bytesAvailable())
        _port->readLine();
    QString strMode = "CH6.SETMODE(3)\n\r";
    _port->write(strMode.toAscii(), strMode.length());
    usleep(10000);
    while(_port->bytesAvailable())
        _port->readLine();
}

void CanaKitController::_readDAQ()
{
    Global *g = Global::instance();
    QVector<bool> cSel = g->getChannelSelect();
    for (int i = 0; i < Global::N_DAQ; i++)
    {
        if(!cSel[i]) continue;
        QString echo;
        QString strCmd = QString("CH%1.GETANALOG\n\r").arg(i + 1);
        mutex.lock();
        _port->write(strCmd.toAscii(), strCmd.length());
        usleep(10000);
        QRegExp regx("\\b(\\d{1,4})\\b");
        while(_port->bytesAvailable())
        {
            echo = _readLine();
            QString strEcho(echo);
            //qDebug() << strEcho;
            if (regx.indexIn(strEcho) >= 0) {
                //qDebug() << "Matched " << regx.cap(1);
                measurement.daq[i] = regx.cap(1).toDouble()/1023*5;
            }
        }
        mutex.unlock();
    }
    if (_isConnected) emit daqUpdated();
}

double CanaKitController::getDAQ(int i)
{
    if (i >= 0 && i < Global::N_DAQ)
        return measurement.daq[i];
    return -1.0;
}

QString CanaKitController::_readLine()
{
    QString str;
    while(_port->bytesAvailable())
    {
        char c;
        bool success = _port->getChar(&c);
        if (success) str += c;
    }
    return str;
}

QanaMeasurement CanaKitController::getMeasurement()
{
    return measurement;
}

void CanaKitController::setRelayState(int i, bool state)
{
    QString echo;
    QString strState = state?"ON":"OFF";
    QString strCmd = QString("REL%1.%2\n\r").arg(i + 1).arg(strState);
    mutex.lock();
    _port->write(strCmd.toAscii(), strCmd.length());
    usleep(10000);
    while(_port->bytesAvailable())
        echo = _readLine();
    mutex.unlock();
}

