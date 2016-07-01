#include "qanaqitmaindlg.h"
#include "ui_qanaqitmaindlg.h"
#include "Global.h"

#include <QDebug>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>

QanaQitMainDlg::QanaQitMainDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QanaQitMainDlg)
{
    global = Global::instance();
    ui->setupUi(this);
    initOutputGrp();
    initInputGrp();
    initController();
    initPortsList();

    ui->txtDataDir->setText(global->getDataFolder());
    ui->spnTotalTime->setValue(global->getDataCollectionTime());
    connect(ui->btnReconnect, SIGNAL(clicked()), this, SLOT(initPortsList()));
    connect(ui->btnDataDir, SIGNAL(clicked()), this, SLOT(setDataFolder()));
    connect(ui->txtDataDir, SIGNAL(textChanged(QString)), global, SLOT(setDataFolder(const QString&)));
    connect(ui->spnTotalTime, SIGNAL(valueChanged(int)), global, SLOT(setDataCollectionTime(int)));

    connect(&expMan, SIGNAL(expEnd()), this, SLOT(experimentEnded()));
    connect(&expMan, SIGNAL(expProgress(int,int)), this, SLOT(experimentProgressed(int,int)));

    connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(startExperiment()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancelExperiment()));
}

QanaQitMainDlg::~QanaQitMainDlg()
{
    delete ui;
}

void QanaQitMainDlg::initPortsList()
{
    cleanUp();
    if (controller->reconnect())
        ui->lblConnectionStatus->setText(
                    QString("<b><font color=green>CANA-KIT FOUND AT %1</font></b>")
                    .arg(controller->getPortName()));
    else
        ui->lblConnectionStatus->setText("<b><font color=red>CANA-KIT NOT FOUND!!!</font></b>");
    controller->start();
}

void QanaQitMainDlg::initOutputGrp()
{
    Global * g = global;
    QVector<RelaySettings> rs = g->getRelaySettings();
    for(int i = 0; i < Global::N_RELAYS; i++) {
        lblRelayStatus[i] = new QLabel("N/A");
        addCentered(ui->tblRelays, lblRelayStatus[i], i, 0);
       chkEnabled[i] = new QCheckBox();
       chkEnabled[i]->setChecked(rs[i].enabled);
       addCentered(ui->tblRelays, chkEnabled[i], i, 1);
       connect(chkEnabled[i], SIGNAL(stateChanged(int)), this, SLOT(relaySettingsChanged()));

       spnStartTime[i] = initSpinBox(ui->tblRelays, rs[i].start_time, i, 2);
       spnOnTime[i]    = initSpinBox(ui->tblRelays, rs[i].on_time, i, 3);
       spnOffTime[i]   = initSpinBox(ui->tblRelays, rs[i].off_time, i, 4);
    }
}

void QanaQitMainDlg::relaySettingsChanged()
{
    //qDebug() << "Relay Settings Changed!!!";
    Global * g = global;
    QVector<RelaySettings> rs(Global::N_RELAYS);
    for(int i = 0; i < Global::N_RELAYS; i++) {
        rs[i].enabled = chkEnabled[i]->isChecked();
        rs[i].start_time = spnStartTime[i]->value();
        rs[i].on_time = spnOnTime[i]->value();
        rs[i].off_time = spnOffTime[i]->value();
    }
    g->setRelaySettings(rs);
}

QSpinBox * QanaQitMainDlg::initSpinBox(QTableWidget *, int value, int row, int col)
{
    QSpinBox * spn = new QSpinBox();
    spn->setValue(value);
    spn->setMinimum(0);
    ui->tblRelays->setCellWidget(row, col, spn);
    connect(spn, SIGNAL(valueChanged(int)), this, SLOT(relaySettingsChanged()));
    return spn;
}

void QanaQitMainDlg::addCentered(QTableWidget *t, QWidget *w, int row, int col)
{
    QWidget * wdg = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout(wdg);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(w);
    layout->addStretch(1);
    wdg->setLayout(layout);
    t->setCellWidget(row, col, wdg);
}

void QanaQitMainDlg::cleanUp()
{
    delete controller;
    initController();
}

void QanaQitMainDlg::closeEvent(QCloseEvent *e)
{
    delete controller;
    e->accept();
    QCoreApplication::exit(0);
    //qDebug() << "Here";
}

void QanaQitMainDlg::initController()
{
    controller = new CanaKitController;
    connect(controller, SIGNAL(relaysUpdated()), this, SLOT(relaysUpdated()));
    connect(controller, SIGNAL(daqUpdated()), this, SLOT(daqUpdated()));
}

void QanaQitMainDlg::relaysUpdated()
{
    if (controller->isConnected()) {
        for(int i = 0; i < Global::N_RELAYS; i++)
            lblRelayStatus[i]->setText(controller->getRelayState(i)?"On":"Off");
    } else
        for(int i = 0; i < Global::N_RELAYS; i++)
            lblRelayStatus[i]->setText("N/A");
}

void QanaQitMainDlg::daqUpdated()
{
    Global * g = global;
    QVector<bool> cSel = g->getChannelSelect();

    if (controller->isConnected()) {
        for(int i = 0; i < Global::N_DAQ; i++) {
            if(cSel[i])
                lblDAQ[i]->setText(QString("%1").arg(controller->getDAQ(i)));
            else lblDAQ[i]->setText("N/A");
        }
    } else
        for(int i = 0; i < Global::N_DAQ; i++)
            lblDAQ[i]->setText("N/A");
}

void QanaQitMainDlg::initInputGrp()
{
    Global * g = global;
    QVector<bool> cSel = g->getChannelSelect();
    for(int i = 0; i < Global::N_DAQ; i++) {
        lblDAQ[i] = new QLabel("N/A");
        addCentered(ui->tblADC, lblDAQ[i], 1, i);
        chkDAQ[i] = new QCheckBox;
        chkDAQ[i]->setChecked(cSel[i]);
        addCentered(ui->tblADC, chkDAQ[i], 0, i);
        connect(chkDAQ[i], SIGNAL(stateChanged(int)), this, SLOT(daqSettingsChanged()));
    }
}

void QanaQitMainDlg::daqSettingsChanged()
{
    Global * g = global;
    QVector<bool> cSel(Global::N_DAQ);
    for(int i = 0; i < Global::N_DAQ; i++) {
        cSel[i] = chkDAQ[i]->isChecked();
    }
    g->setChannelSelect(cSel);
}

void QanaQitMainDlg::setDataFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Data Folder",
                    ui->txtDataDir->text(), QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
        ui->txtDataDir->setText(dir);
}

void QanaQitMainDlg::setEnabledExperiment(bool b)
{
    ui->btnReconnect->setEnabled(b);
    ui->btnDataDir->setEnabled  (b);
    ui->btnStart->setEnabled    (b);
    ui->txtDataDir->setEnabled  (b);

    for(int i = 0; i < Global::N_RELAYS; i++) {
       chkEnabled  [i]->setEnabled(b);
       spnStartTime[i]->setEnabled(b);
       spnOnTime   [i]->setEnabled(b);
       spnOffTime  [i]->setEnabled(b);
    }

    for(int i = 0; i < Global::N_DAQ; i++)
        chkDAQ[i]->setEnabled(b);
}

void QanaQitMainDlg::startExperiment()
{
    setEnabledExperiment(false);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(ui->spnTotalTime->value()*1000);
    QDateTime qdt = QDateTime::currentDateTime();
    QString dirPath = ui->txtDataDir->text()
                    + "/" + qdt.toString("dd.MM.yyyy hh.mm.ss.zzz");
    QDir dir;
    if (!dir.mkdir(dirPath)) {
            QMessageBox::information(this,
                            "Impedance Measurement DAQ",
                            "Error while creating output directory!");
            return;
    }
    expMan.startExp(dirPath, controller, ui->spnTotalTime->value()*1000);
}

void QanaQitMainDlg::cancelExperiment()
{
    expMan.cancelExp();
}

void QanaQitMainDlg::experimentEnded()
{
    setEnabledExperiment(true);
}

void QanaQitMainDlg::experimentProgressed(int current, int total)
{
    ui->progressBar->setValue(current);
}
