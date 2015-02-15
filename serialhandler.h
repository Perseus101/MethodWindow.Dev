#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QMessageBox>
#include <QSerialPort>
#include <QTableView>
#include <QTest>
#include <QByteArray>
#include <QDebug>



#include "settingsdialog.h"

class SerialHandler : public QObject
{
    Q_OBJECT
public:
    SerialHandler(SettingsDialog *settings, QTableView *posData);
    virtual ~SerialHandler() {}
    QSerialPort *serial;

public slots:
    void openSerialPort();
    void closeSerialPort();
    void moveToSample(int sample);
    void moveToWaste(int waste_position);
    void moveToZero();
    void pumpOn();
    void pumpOff();
    void sendInit();
    void sendMGHP();

private slots:
    void writeData(const QByteArray &data);
    void readData();
    void sendMoveAbsolute(int position);
    void sendMoveIncrement();
    void sendSetOut(bool on);
    void sendReadIn();

    void handleError(QSerialPort::SerialPortError error);
    void waitTillReady();

private:
    SettingsDialog *settings;
    QTableView *positionData;



    bool open;
    int i;
};

#endif // SERIALHANDLER_H
