/*
 * ExperimentManager.h
 *
 *  Created on: Nov 19, 2011
 *      Author: Prakash Manandhar
 */
#ifndef EXPERIMENTMANAGER_H
#define EXPERIMENTMANAGER_H

#include <QThread>
#include <QVector>

#include "qanameasurement.h"
#include "canakitcontroller.h"
#include "Global.h"

class ExperimentManager : public QThread
{
    Q_OBJECT
public:
    ExperimentManager();

    /**
     * dataDir       : data directory
     * controller    : pointer to controller
     * total_time_ms : total time in milliseconds to run the experiment
     */
    void startExp(QString dataDir, CanaKitController *controller, int total_time_ms);

signals:
    void expEnd();
    void expProgress(int current, int total);

public slots:
    void cancelExp();

protected:
    void run();

private:
    QVector<QanaMeasurement> measurements;
    CanaKitController * controller;
    QString dataDir;

    volatile bool isCancelled;
    int start_time_ms, end_time_ms, total_time_ms;
    QVector<RelaySettings> relaySettings;

    int rl_s_ms[Global::N_RELAYS]; // start time is ms
    int rl_1_ms[Global::N_RELAYS]; // on time in ms
    int rl_0_ms[Global::N_RELAYS]; // off time in ms
    int rl_p_ms[Global::N_RELAYS]; // period time in ms
    int current_time_progress;

    void relayControl();
    void writeData();
    void writeMATLAB();
};

#endif // EXPERIMENTMANAGER_H
