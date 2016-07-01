/*
 * Global.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: PManandhar
 */

#include "Global.h"
#include <QDebug>
#include <math.h>

Global * Global::pI = NULL;

Global::Global() {
        // load settings
        settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                        "p-manandhar.info", "QanaQit");
}

Global::~Global() {
        delete settings;
}

QVector<bool> Global::getChannelSelect()
{
        QVector<bool> csel(N_DAQ);
        QString s(settings->value("DAQChannelsSelect").toString());
        for (int i = 0; i < N_DAQ; i++)
        {
                if (s.size() <= i) csel[i] = false;
                else
                        csel[i] = (s[i] == QChar('1'));
        }
        return csel;
}

void Global::setChannelSelect(const QVector<bool>& csel)
{
        QString s;
        for (int i = 0; i < N_DAQ; i++)
        {
                if (csel[i]) s += '1';
                else s += '0';
        }
        settings->setValue("DAQChannelsSelect", s);
        settings->sync();
}

void Global::setDataFolder(const QString& v)
{
        settings->setValue("DataFolder", v);
        settings->sync();
}


QString Global::getDataFolder()
{
        QString v = (settings->value("DataFolder").toString());
        if (v.isEmpty())
        {
                v = "C:/QanaQit/Data";
                setDataFolder(v);
        }
        return v;
}

void Global::setRelaySettings(const QVector<RelaySettings> & rs)
{
    QString s;
    for (int i = 0; i < N_RELAYS; i++)
    {
            QString key = QString("StartTime%1").arg(i);
            settings->setValue(key, rs[i].start_time);
            key = QString("OnTime%1").arg(i);
            settings->setValue(key, rs[i].on_time);
            key = QString("OffTime%1").arg(i);
            settings->setValue(key, rs[i].off_time);
            key = QString("RelayEnabled%1").arg(i);
            settings->setValue(key, rs[i].enabled);
    }
    settings->sync();
}

QVector<RelaySettings> Global::getRelaySettings()
{
    QVector<RelaySettings> rs(Global::N_RELAYS);
    for (int i = 0; i < Global::N_RELAYS; i++)
    {
          QString key = QString("StartTime%1").arg(i);
          uint v = (settings->value(key).toUInt());
          rs[i].start_time = v;
          key = QString("OnTime%1").arg(i);
          v = (settings->value(key).toUInt());
          rs[i].on_time = v;
          key = QString("OffTime%1").arg(i);
          v = (settings->value(key).toUInt());
          rs[i].off_time = v;
          key = QString("RelayEnabled%1").arg(i);
          bool e = (settings->value(key).toBool());
          rs[i].enabled = e;
    }
    return rs;
}

QString Global::toHrMinSec (double T)
{
        QString strT = QString("%1s").arg(T);
        if (T > 60) {
                int h = floor(T/60/60);
                int m = floor((T - h*60*60)/60);
                double s = T - h*60*60 - m*60;
                strT += QString("\n [ %1 hr %2 min %3 secs]").arg(h).arg(m).arg(s);
        }
        return strT;
}

int Global::getDataCollectionTime()
{
    return settings->value("TOTAL_DATA_TIME").toInt();
}

void Global::setDataCollectionTime(const int &t)
{
    settings->setValue("TOTAL_DATA_TIME", t);
    settings->sync();
}
