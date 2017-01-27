﻿#include "methodrun.h"

MethodRun::MethodRun(MethodTableModel *p_data,  Ui::NewMethod *p_ui, SettingsDialog *serialComms)
    : handler(new SerialHandler(serialComms, p_ui->tablePosData)),m_data(p_data), ui(p_ui), mutex()

{
    reset();

    // Connect serial comm dialog

    connect(ui->openDeviceSettings, SIGNAL(clicked()), serialComms, SLOT(show()));

    // Connect start, stop, and pause
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startSequence()));
    connect(ui->pauseButton, SIGNAL(clicked()), this, SLOT(pauseSequence()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopSequence()));

    // Set up HMS displays
    ui->timeActionRemaining->setDigitCount(8);
    ui->timeStepRemaining->setDigitCount(8);
    ui->timeRunRemaining->setDigitCount(8);

    ui->timeActionTotal->setDigitCount(8);
    ui->timeStepTotal->setDigitCount(8);
    ui->timeRunTotal->setDigitCount(8);

    start();
}

void MethodRun::run()
{
    // Initialize thread
    QTimer timer (0);
    timer.moveToThread(this); // Make timer handled by this thread's execution loop
    timer.setInterval(ui->loopSpeed->value());
    connect(&timer, SIGNAL(timeout()), this, SLOT(stepLoop()));
    timer.start();

    reset();

    //Enter thread execution loop
    exec();
}

void MethodRun::reset()
{
    mutex.lock();

    running = false;
    pauseThread = false;

    startRun = false;
    startStep = false;
    startAction = false;
    pausePump = false;

    action = 0;
    sample = 0;
    elapsedActionMS = 0;
    elapsedStepMS = 0;
    elapsedRunMS = 0;


    ui->progressBarAction->setMinimum(0);
    ui->progressBarStep->setMinimum(0);
    ui->progressBarRun->setMinimum(0);

    setActionMS(1);
    setStepMS(1);
    setRunMS(1);

    ui->progressBarAction->setValue(0);
    ui->progressBarStep->setValue(0);
    ui->progressBarRun->setValue(0);

    ui->timeActionRemaining->display(0);
    ui->timeStepRemaining->display(0);
    ui->timeRunRemaining->display(0);

    ui->timeActionTotal->display(0);
    ui->timeStepTotal->display(0);
    ui->timeRunTotal->display(0);

    ui->currentRun->setText("");
    ui->currentStep->setText("");
    ui->currentAction->setText("");
    ui->pauseButton->setText("Pause Sequence");
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);

    if (!ui->checkNoCom->isChecked()) handler->closeSerialPort();
    ui->startButton->setEnabled(true);
    mutex.unlock();
}

void MethodRun::updatePumpStat()
{
    if (pumpOn)
        ui->pumpStat->setText("Pump On");
    else
        ui->pumpStat->setText("Pump Off");
}

void MethodRun::setActionMS(int ms)
{
    //    if(ms==0) ms=5000;
    actionMS = ms;
    ui->progressBarAction->setMaximum(ms);
}
void MethodRun::setStepMS(int ms)
{
    stepMS = ms;
    ui->progressBarStep->setMaximum(ms);
}
void MethodRun::setRunMS(int ms)
{
    runMS = ms;
    ui->progressBarRun->setMaximum(ms);
}

// Slots

void MethodRun::startSequence()
{
    mutex.lock();
    ui->startButton->setEnabled(false);

    if (!ui->checkNoCom->isChecked()) handler->openSerialPort();
    if (!ui->checkNoCom->isChecked()) handler->sendInit();
    if (!ui->checkNoCom->isChecked()) handler->sendMGHP();

    startRun = true;
    running = true;
    ui->stopButton->setEnabled(true);
    ui->pauseButton->setEnabled(true);

    mutex.unlock();
}


void MethodRun::pauseSequence()
{
    if(!pauseThread)
    {// Pause the process
        mutex.lock();
        pauseThread = true;
        if (pumpOn) {
            if (!ui->checkNoCom->isChecked()) handler->pumpOff();

            pausePump = true;
            pumpOn = false;
        }
        ui->pauseButton->setText("Resume Sequence");
        mutex.unlock();
        // Add pump control
    }
    else
    {// Unpause the process
        mutex.lock();
        pauseThread = false;
        if (pausePump) {

            if (!ui->checkNoCom->isChecked()) handler->pumpOn();

            pausePump = false;
            pumpOn = true;
        }
        ui->pauseButton->setText("Pause Sequence");
        mutex.unlock();
        //add pump control
    }
    updatePumpStat();
}

void MethodRun::stopSequence()
{
    // turn off access to other functions
    ui->stopButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    running = false;

    // shut off pump
    if (!ui->checkNoCom->isChecked()) handler->pumpOff();
    QTest::qWait(2000);   // avoid the drip
    pumpOn = false;
    updatePumpStat();

    // Move back to origin, check that, move to waste
    //if (!ui->checkNoCom->isChecked()) handler->moveToZero();
    if (!ui->checkNoCom->isChecked()) handler->moveToWaste(ui->wastePosition->value());


    // Wait a few for the dust to settle before reset
    QTest::qWait(500);

    mutex.unlock();
    reset();
}


QString formatHMS(int ms)
{
    int milliseconds = (int) (ms) % 1000;
    int seconds      = (int) (ms / 1000) % 60 ;
    int minutes      = (int) ((ms / (1000*60)) % 60);
    int hours        = (int) ((ms / (1000*60*60)) % 24);
//    int seconds      = (int) (ms / 1000) % 60 ;
//    int minutes      = (int) ((ms / 1000)/60) % 60;
//    int hours        = (int) (((ms / 1000)/60)/60) % 24;


    QString m,s;

    if(minutes < 10)
        m = QString("0%1:").arg(minutes);
    else
        m = QString("%1:").arg(minutes);
    if(seconds < 10)
        s = QString("0%1").arg(seconds);
    else
        s = QString("%1").arg(seconds);

    return QString("%1:").arg(hours).append(m).append(s);
}

void MethodRun::stepLoop()
{
    if(running)
    {
        mutex.lock();

        if(!pauseThread)
        {
            // update time for all sections if not paused....  and correct for increment passs through..(DDM)
            elapsedActionMS+= ui->loopSpeed->value() * ui->accelerationMultiplier->value();
            if(elapsedActionMS <= actionMS) elapsedStepMS+= ui->loopSpeed->value() * ui->accelerationMultiplier->value();
            if(elapsedActionMS <= actionMS && elapsedStepMS <=stepMS) elapsedRunMS+= ui->loopSpeed->value() * ui->accelerationMultiplier->value();

            if(startRun)          //  set all params for beginning of run
            {
                setRunMS(m_data->getRunSeconds() * 1000);


                startStep = true;
                startAction = true;

                startRun = false;
                sample = -1;
            }

            if(startStep)
            {
                do
                {
                    sample++;
                    setStepMS(m_data->getStepSeconds(sample) * 1000);

                }
                while(stepMS == 0 && sample < SAMPLES);
                // Account for empty steps and go to next

                if(sample == SAMPLES) // All steps completed
                {
                  stopSequence();
                  return;
                }
                startStep = false;
                startAction = true;
                elapsedStepMS = 0;
                action = -1;
            }
            if(startAction)
            {
                do
                {
                    action++;
                    setActionMS(m_data->getActionSeconds(sample,action) * 1000);
                }
                while(actionMS == 0 && action < 1);

                running=false;
                ui->stopButton->setEnabled(false);
                ui->pauseButton->setEnabled(false);
                switch(action)
                {

                case 0:// Waste Dwell
                {
                    // *** Move to waste ***
                    if (!ui->checkNoCom->isChecked()) handler->moveToWaste(ui->wastePosition->value());

                    // *** Turn on nozel ***
                    pumpOn = true;
                    if (!ui->checkNoCom->isChecked()) handler->pumpOn();

                }

                    break;

                case 1:// Sample
                {
                    if (actionMS > 2000 ){
                        // *** Move to sample ***
                        if (!ui->checkNoCom->isChecked()) handler->moveToSample(sample);

                        //  Turn on the pump
                        pumpOn=true;
                        if (!ui->checkNoCom->isChecked()) handler->pumpOn();
                    }
                }
                    break;

                default:
                {
                    startStep = true;
                }
                    break;
                }

                startAction = false;
                elapsedActionMS = 0;
                // updatePumpStat();
                ui->stopButton->setEnabled(true);
                ui->pauseButton->setEnabled(true);
                running = true;
            }

            //Check for finished action
            if(elapsedActionMS >= actionMS)
            {
               // *** Turn off nozel ***
                ui->stopButton->setEnabled(false);
                ui->pauseButton->setEnabled(false);
                running=false;
                pumpOn=false;
                if (!ui->checkNoCom->isChecked()) handler->pumpOff();

                // wait for pump to stop dripping
                QTest::qWait(2000);

                //Reset to next action
               // if (action >= 1) startStep=true;
                startAction = true;
                running = true;
                ui->stopButton->setEnabled(true);
                ui->pauseButton->setEnabled(true);


            }

            // Update graphics with relevant information

            ui->progressBarAction->setValue(elapsedActionMS);
            ui->progressBarStep->setValue(elapsedStepMS);
            ui->progressBarRun->setValue(elapsedRunMS);

            ui->timeActionRemaining->display(formatHMS(actionMS - elapsedActionMS));
            ui->timeStepRemaining->display(formatHMS(stepMS - elapsedStepMS));
            ui->timeRunRemaining->display(formatHMS(runMS - elapsedRunMS));

            ui->timeActionTotal->display(formatHMS(actionMS));
            ui->timeStepTotal->display(formatHMS(stepMS));
            ui->timeRunTotal->display(formatHMS(runMS));

            ui->currentRun->setText(m_data->m_FileName);
            ui->currentStep->setText(QString("%1").arg(sample + 1));
            ui->currentAction->setText(column_Headers[action]);
            updatePumpStat();
        }
        mutex.unlock();
    }
}
