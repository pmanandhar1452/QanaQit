/*
 * QanaKitMainDlg.h
 *
 *  Created on: Nov 17, 2011
 *      Author: PManandhar
 */
#ifndef QANAQITMAINDLG_H
#define QANAQITMAINDLG_H

#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QCloseEvent>
#include <QLabel>

#include "canakitcontroller.h"
#include "Global.h"
#include "experimentmanager.h"

namespace Ui {
    class QanaQitMainDlg;
}

class QanaQitMainDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QanaQitMainDlg(QWidget *parent = 0);
    ~QanaQitMainDlg();

protected:
    void closeEvent(QCloseEvent *);

private:

    Ui::QanaQitMainDlg *ui;

    CanaKitController *controller;
    ExperimentManager expMan;

    void cleanUp();
    void initController();
    void initOutputGrp();
    void initInputGrp();
    void initADC();

    void setEnabledExperiment(bool);

    void addCentered(QTableWidget *, QWidget *, int row, int col);
    QSpinBox * initSpinBox(QTableWidget *, int value, int row, int col);

    QLabel * lblRelayStatus [Global::N_RELAYS];
    QCheckBox * chkEnabled  [Global::N_RELAYS];
    QSpinBox  * spnStartTime[Global::N_RELAYS];
    QSpinBox  * spnOnTime   [Global::N_RELAYS];
    QSpinBox  * spnOffTime  [Global::N_RELAYS];

    QCheckBox * chkDAQ[Global::N_DAQ];
    QLabel * lblDAQ   [Global::N_DAQ];

    Global * global;

private slots:
    void initPortsList();
    void relaySettingsChanged();
    void daqSettingsChanged();

    void startExperiment();
    void cancelExperiment();

    void experimentEnded();
    void experimentProgressed(int current, int total);


    void relaysUpdated();
    void daqUpdated();
    void setDataFolder();

};

#endif // QANAQITMAINDLG_H
