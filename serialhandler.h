#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QMessageBox>
#include <QSerialPort>
#include <QTableView>
#include <QTest>
#include <QTime>
#include <QByteArray>
#include <QDebug>
#include <QStringList>



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
    void moveToWaste(float waste_position);
    void moveToZero();
    void pumpOn();
    void pumpOff();
    void sendInit();
    void sendMGHP();

private slots:
    void writeData(const QByteArray &data);
    void readData();
    void sendMoveAbsolute(float position);
    void sendMoveIncrement();
    void sendSetOut(bool on);
    void sendReadIn();

    void handleError(QSerialPort::SerialPortError error);

private:
    SettingsDialog *settings;
    QTableView *positionData;
    QByteArray msgList;
    QFile logFile;
    bool open;
    int i;
};

#endif // SERIALHANDLER_H
