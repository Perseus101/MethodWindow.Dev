#include "methodrun.h"

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
    timer.setInterval(1000);
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

    if (!ui->checkNoCom->isChecked()) handler->closeSerialPort();
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
    ui->startButton->setEnabled(false);
    if (!ui->checkNoCom->isChecked()) handler->openSerialPort();
    if (!ui->checkNoCom->isChecked()) handler->sendInit();
    mutex.lock();
     if (!ui->checkNoCom->isChecked()) handler->sendMGHP();
    startRun = true;
    running = true;
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
     if (!ui->checkNoCom->isChecked()) handler->moveToZero();  //go back to zero at finish
    mutex.unlock();
    ui->startButton->setEnabled(true);
    QTest::qWait(1000);
     if (!ui->checkNoCom->isChecked()) handler->pumpOff();
    pumpOn = false;
    updatePumpStat();

    reset();
}


void MethodRun::stepLoop()
{
    if(running)
    {
        mutex.lock();

        if(!pauseThread)
        {
              // update time for all sections if not paused....
            elapsedActionMS+= 10 * ui->accelerationMultiplier->value();
            elapsedStepMS+= 10* ui->accelerationMultiplier->value();
            elapsedRunMS+= 10* ui->accelerationMultiplier->value();
            //chddm 12-29  cut clock cycle to 10 ms from one...

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
                while(stepMS == 0 && sample < SAMPLES); // Account for empty steps and go to next

                if(sample == SAMPLES) // All steps completed
                {
                    pumpOn=false;
                    if (!ui->checkNoCom->isChecked()) handler->pumpOff();
                    if (!ui->checkNoCom->isChecked()) handler->moveToZero();  //go back to zero to finish
                    ui->startButton->setEnabled(true);
                    mutex.unlock();
                    QTest::qWait(1000);

                    reset();
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
                while(actionMS == 0 && action < 1); //Account for empty actions and go to next

                switch(action)
                {

                case 0:// Waste Dwell
                {
                    // *** Move to waste ***
                    if (!ui->checkNoCom->isChecked()) handler->moveToWaste(ui->wastePosition->value());

                    //wait for transit
                    // qwait transit time

                    // *** Turn on nozel ***
                    pumpOn = true;
                    if (!ui->checkNoCom->isChecked()) handler->pumpOn();
                }
                    break;

                case 1:// Sample
                {
                    // *** Move to sample ***
                    if (!ui->checkNoCom->isChecked()) handler->moveToSample(sample);

                    // wait for transit
                    //qwait  transit time

                    // *** Turn on nozel ***
                    if (actionMS > 200 ) pumpOn=true;
                    if (!ui->checkNoCom->isChecked()) handler->pumpOn();
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
                updatePumpStat();
            }

            //Check for finished action
            if(elapsedActionMS >= actionMS)
            {
                // *** Turn off nozel ***
                pumpOn=false;
                if (!ui->checkNoCom->isChecked()) handler->pumpOff();

                //Reset to next action
                startAction = true;
            }

            // Update graphics with relevant information
            ui->progressBarAction->setValue(elapsedActionMS);
            ui->progressBarStep->setValue(elapsedStepMS);
            ui->progressBarRun->setValue(elapsedRunMS);

//            ui->timeActionRemaining->display(HMS((actionMS - elapsedActionMS)/1000).getHMS());
//            ui->timeStepRemaining->display(HMS((stepMS - elapsedStepMS)/1000).getHMS());
//            ui->timeRunRemaining->display(HMS((runMS - elapsedRunMS)/1000).getHMS());

//            ui->timeActionTotal->display(HMS(actionMS/1000).getHMS());
//            ui->timeStepTotal->display(HMS(stepMS/1000).getHMS());
//            ui->timeRunTotal->display(HMS(runMS/1000).getHMS());

            ui->currentRun->setText(m_data->m_FileName);
            ui->currentStep->setText(QString("%1").arg(sample + 1));
            ui->currentAction->setText(column_Headers[action]);
        }

        mutex.unlock();
    }
}
