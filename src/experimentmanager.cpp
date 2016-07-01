/*
 * ExperimentManager.cpp
 *
 *  Created on: Nov 19, 2011
 *      Author: Prakash Manandhar
 */
#include "experimentmanager.h"
#include "LoggerTime.h"
#include "Global.h"

#include <QDebug>
#include <QFile>

ExperimentManager::ExperimentManager()
{
}

void ExperimentManager::startExp(QString dataDir, CanaKitController *controller, int total_time_ms)
{
    measurements.clear();
    measurements.reserve(total_time_ms/100);
    isCancelled = false;
    this->dataDir = dataDir;
    this->controller = controller;
    this->total_time_ms = total_time_ms;
    this->start_time_ms = LoggerTime::timer();
    this->end_time_ms   = start_time_ms + total_time_ms;
    relaySettings = Global::instance()->getRelaySettings();
    for (int i = 0; i < Global::N_RELAYS; i++)
    {
        if(relaySettings[i].enabled) {
            this->rl_s_ms[i] = relaySettings[i].start_time*1000;
            this->rl_1_ms[i] = relaySettings[i].on_time*1000;
            this->rl_0_ms[i] = relaySettings[i].off_time*1000;
            this->rl_p_ms[i] = rl_1_ms[i] + rl_0_ms[i];
        }
    }
    this->start();
}

void ExperimentManager::run()
{
    emit expProgress(0, total_time_ms);
    int current_time_ms = LoggerTime::timer();
    while( !isCancelled && (current_time_ms < end_time_ms) )
    {
        current_time_progress = current_time_ms - start_time_ms;
        //qDebug() << "Current Time Progress:" << current_time_progress;
        relayControl();
        measurements.append(controller->getMeasurement());
        emit expProgress(current_time_progress, total_time_ms);
        usleep(100000);
        current_time_ms = LoggerTime::timer();
    }
    for(int i = 0; i < Global::N_RELAYS; i++)
        controller->setRelayState(i, false);
    writeData();
    writeMATLAB();
    emit expProgress(total_time_ms, total_time_ms);
    emit expEnd();
}

void ExperimentManager::cancelExp()
{
    isCancelled = true;
}

/**
 * Determine whether relay needs to be switched and if yes, switch it. The state of a relay
 *  is determined from the first start time, on time and off time as follows:
 *   1) first_on_time is substracted from current_time_progress to get effective_time.
        If this time is negative, relay should be OFF.
     2) the period_time for a relay is determined by adding the on_time + off_time
     3) the mod operation determines mod_time = effective_time%period_time
     4) if mod_time < on_time => relay should be ON, otherwise relay should be OFF
 */
void ExperimentManager::relayControl()
{
    for(int i = 0; i < Global::N_RELAYS; i++)
    {
        if (relaySettings[i].enabled) {
            bool relay_state;
            int eff_time = current_time_progress - rl_s_ms[i];
            if (eff_time < 0) relay_state = false;
            else {
                int mod_time = eff_time%rl_p_ms[i];
                if (mod_time < rl_1_ms[i])
                    relay_state = true;
                else relay_state = false;
            }
            if (relay_state != controller->getRelayState(i))
                controller->setRelayState(i, relay_state);
        }
    }
}

void ExperimentManager::writeData()
{
    QVector<bool> cSel = Global::instance()->getChannelSelect();
    QFile fileI(dataDir + "/QanaData.dat");
    fileI.open(QIODevice::WriteOnly);
    for (int i = 0; i < measurements.size(); i++) {
        double t = measurements[i].time;
        fileI.write((char*)&t, sizeof(double));
        for(int nr = 0; nr < Global::N_RELAYS; nr++)
        {
            double v = measurements[i].relays[nr]?1.0:0.0;
            fileI.write((char*)&v, sizeof(double));
        }
        for (int nv = 0; nv < Global::N_DAQ; nv++) {
            if(cSel[nv]) {
                double v = measurements[i].daq[nv];
                fileI.write((char*)&v, sizeof(double));
            }
        }
    }
    fileI.close();
}

void ExperimentManager::writeMATLAB()
{
    QFile fileI(dataDir + "/LoadQanaData.m");
    fileI.open(QIODevice::WriteOnly);

    QVector<bool> cSel = Global::instance()->getChannelSelect();
    int nChan = 0;
    for (int i = 0; i < Global::N_DAQ; i++)
        if (cSel[i]) nChan++;
    QString tStr = QString (
                    "fid = fopen('QanaData.dat', 'r');\n"
                    "data = fread(fid, [%1 inf], 'double');\n"
                    "fclose(fid);\n"
                    "t = data(1,:)';\n"
                    "r = data(2:5,:)';\n"
                    "V = data(6:end,:)';\n"
                    "clear data;\n"
                    "t = t - t(1);\n"
                    "t = t/1e3;\n"
            ).arg(nChan + 5);
    fileI.write(tStr.toAscii());
    fileI.close();

}
