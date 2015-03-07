#include "serialhandler.h"

SerialHandler::SerialHandler(SettingsDialog *settings, QTableView *posData)
    :serial(new QSerialPort(this)), settings(settings), logFile(new QFile(this)),positionData(posData), open(false)
{
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    //connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
}

// Public Slots
void SerialHandler::openSerialPort()
{ 
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);


    if (serial->open(QIODevice::ReadWrite))
    {
        open = true;
        //Open Logfile
        QString filename="log/MethodLog-" + QDateTime::currentDateTime().toString()+".txt";
        logFile.setFileName(filename);
        if ( logFile.open(QIODevice::ReadWrite) )
        {
            QTextStream stream( &logFile );
            stream << QTime::currentTime().toString() <<"  --  Serial port Init" <<  endl;
        }
//        qDebug()<< "serialport open";
//            console->setEnabled(true);
//            console->setLocalEchoEnabled(p.localEchoEnabled);
//            ui->actionConnect->setEnabled(false);
//            ui->actionDisconnect->setEnabled(true);
//            ui->actionConfigure->setEnabled(false);
//              ui->actionMGHP
//            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
//                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
//                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    }
    else
    {
        QMessageBox::critical(NULL, QObject::tr("Error"), serial->errorString());
    }
}

void SerialHandler::closeSerialPort()
{
    //qDebug()<< "serialport check";
    if(open){
        serial->close();        
    }
    open = false;
//    console->setEnabled(false);
//    ui->actionConnect->setEnabled(true);
//    ui->actionDisconnect->setEnabled(false);
//    ui->actionConfigure->setEnabled(true);
//    ui->statusBar->showMessage(tr("Disconnected"));
}

void SerialHandler::moveToSample(int sample)
{
    //Log the position
    QTextStream stream( &logFile );
    stream << QTime::currentTime().toString() << QString(" ------- Moving to Sample #  %1 ------- ").arg(sample) <<  endl;


    float position = positionData->model()->data(positionData->model()->index(sample, 0)).toFloat();
    sendMoveAbsolute(position);
//    QByteArray data = "SIGREADY\n";
//    while (!msgList.contains("SIGREADY=1")){
//        qDebug() << "SigReady";
//        serial->write(data);
//         qDebug() << "loop  MOVE -  "<< msgList;
//         QTest::qWait(1000);
//     }
//    msgList = "EMPTY\n";
//    // qDebug(QString("Position: %1").arg(position).toStdString().c_str());
}

void SerialHandler::moveToWaste(float waste_position)
{
    // TODO Waste position???
    sendMoveAbsolute(waste_position);
}

void SerialHandler::moveToZero()
{
    sendMoveAbsolute(0);
}

void SerialHandler::pumpOn()
{
    sendSetOut(true);
//    qDebug("Pump on.");
}

void SerialHandler::pumpOff()
{
    sendSetOut(false);
//    qDebug("Pump off.");
}

// Private Slots
void SerialHandler::writeData(const QByteArray &data)
{
    serial->write(data);
}

void SerialHandler::readData()
{
    while(serial->bytesAvailable()){
        QByteArray data = serial->readAll();
        while (serial->waitForReadyRead(10))
            data += serial->readAll();
     msgList.append(data);
     QString tdata = QTime::currentTime().toString() + "  " + QString(data);
     qDebug() << tdata;
     QTextStream stream( &logFile );
     stream << QTime::currentTime().toString() <<" -- Serial Data   " << QString(data) <<  endl;

     //     stream << tdata;
//     QTextStream stream( &logFile );
//     stream << "something" << endl;

    }
}

//qDebug() << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss") << "Debug :" << var;

void SerialHandler::sendInit()
{
    SettingsDialog::Settings p = settings->settings();
    QByteArray data = p.initString.toLocal8Bit();
    serial->write(data);
    serial->waitForBytesWritten(10);
    data = "ECHO=1\n";
    serial->write(data);
    serial->waitForBytesWritten(10);
    data = "VERBOSE=1\n";
    serial->write(data);
    serial->waitForBytesWritten(10);
}


void SerialHandler::sendMGHP()
{
    QByteArray data = "MGHP\n";
    serial->write(data);
    data = "SIGHOMEP\n";
    while (!msgList.contains("SIGHOMEP=1")){
         serial->write(data);
         qDebug() << "loop HOME -  ";
         QTest::qWait(1000);
     }
    msgList = "EMPTY\n";
    // include statusBar()->message(tr("Homeing"));
}


void SerialHandler::sendMoveAbsolute(float position)
{

    QString data = QString("MA %1\n").arg(position);
    serial->write(data.toUtf8());
    data = QString("SIGREADY\n");
    while (!msgList.contains("SIGREADY=1")){
         serial->write(data.toUtf8());
         //qDebug() <<"loop  MOVE -  "<< msgList;
         QTest::qWait(1000);
     }
    msgList = "EMPTY\n";

    // insert loop here to send "SIGREADY" and wait for "SIGREADY=1"
    // include statusBar()->message(tr("In Motion"));
}


void SerialHandler::sendMoveIncrement()
{
    QByteArray data = "DIS 50; MI\n";
    serial->write(data);
}


void SerialHandler::sendSetOut(bool on)
{
    QString data = QString("OUT4=%1\n").arg(int(on)); //ouput 4 is pump, true turns it on.
    serial->write(data.toUtf8());
}


void SerialHandler::sendReadIn()
{
    QByteArray data = "IN";
    serial->write(data);
}

void SerialHandler::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(NULL, QObject::tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
