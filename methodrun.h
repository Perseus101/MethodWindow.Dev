#ifndef METHODRUN_H
#define METHODRUN_H

#include <QThread>
#include <QMutex>
#include <QTimer>

#include "ui_methodwindow.h"

#include "globals.h"

#include "methodtablemodel.h"
#include "settingsdialog.h"
#include "serialhandler.h"

class MethodRun : public QThread
{
    Q_OBJECT
public:
    MethodRun(MethodTableModel *p_data, Ui::NewMethod *ui, SettingsDialog *serialComms);

private:
    SerialHandler *handler;
    MethodTableModel *m_data;
    Ui::NewMethod *ui;
    QMutex mutex;
    bool running, pauseThread, pumpOn, pausePump;
    bool startRun, startStep, startAction;
    int action, sample,i;
    int elapsedActionMS, elapsedStepMS, elapsedRunMS;
    int actionMS, stepMS, runMS;

    void run();
    void reset();
    void updatePumpStat();

    void setActionMS(int ms);
    void setStepMS(int ms);
    void setRunMS(int ms);

public slots:
    void startSequence();
    void pauseSequence();
    void stopSequence();
    void stepLoop();

};

#endif // METHODRUN_H



