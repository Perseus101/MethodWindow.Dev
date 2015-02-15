/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(apply()));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomBaudRatePolicy(int)));

    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx != -1) {
        QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
        ui->descriptionLabel->setText(tr("Description: %1").arg(list.at(1)));
        ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.at(2)));
        ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.at(3)));
        ui->locationLabel->setText(tr("Location: %1").arg(list.at(4)));
        ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.at(5)));
        ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.at(6)));
    }
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void SettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(QStringLiteral("Custom"));

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(QStringLiteral("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    static const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
}

void SettingsDialog::updateSettings()
{
    currentSettings.name = ui->serialPortInfoListBox->currentText();
    currentSettings.nameidx = ui->serialPortInfoListBox->currentIndex();

    if (ui->baudRateBox->currentIndex() == 4) {
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
    currentSettings.initString = ("VS "+ ui->spinVS->cleanText() + ";VR " + ui->spinVR->cleanText() + ";TA " + ui->spinTATD->cleanText() +  ";TD " + ui->spinTATD->cleanText() + "\n");
    // currentSettings.transitTime = ui->transitionTime->cleanText();
}

void SettingsDialog::saveSettings(QSettings *settings)
{
    settings->setValue("Port_Name_idx", currentSettings.nameidx);
    settings->setValue("Baud_Rate", currentSettings.stringBaudRate);
    settings->setValue("Data_Bits", currentSettings.stringDataBits);
    settings->setValue("Parity", currentSettings.stringParity);
    settings->setValue("Stop_Bits", currentSettings.stringStopBits);
    settings->setValue("Flow_Control", currentSettings.flowControl);

//    settings->setValue("Init_String", currentSettings.initString);  --Changed to break up init params

    settings->setValue("Init_String_VS", ui->spinVS->cleanText());
    settings->setValue("Init_String_VR", ui->spinVR->cleanText());
    settings->setValue("Init_String_TATD", ui->spinTATD->cleanText());
   // settings->setValue("Transition_Time", ui->transiitonTime->cleanText());

    settings->setValue("Local_Echo_Enabled", currentSettings.localEchoEnabled);
}

void SettingsDialog::loadSettings(QSettings *settings)
{   
    // Load the easy stuff
    ui->spinVS->setValue(settings->value("Init_String_VS", 0).toDouble());
    ui->spinVR->setValue(settings->value("Init_String_VR", 0).toDouble());
    ui->spinTATD->setValue(settings->value("Init_String_TATD", 0).toDouble());
    ui->transitionTime->setValue(settings->value("Transition_Time", 0).toInt());

    ui->serialPortInfoListBox->setCurrentIndex(settings->value("Port_Name_idx",0).toInt());

    currentSettings.localEchoEnabled = settings->value("Local_Echo_Enabled", false).toBool();
    ui->localEchoCheckBox->setChecked(settings->value("Local_Echo_Enabled", false).toBool());
    int temp_baudRate, temp_dataBits, temp_parity, temp_stopBits, temp_flowControl;

    // Load the less easy stuff
    temp_baudRate = settings->value("Baud_Rate", 0).toInt();
    temp_dataBits = settings->value("Data_Bits", 0).toInt();
    temp_parity = settings->value("Parity", 0).toInt();
    temp_stopBits = settings->value("Stop_Bits", 0).toInt();
    temp_flowControl = settings->value("Flow_Control", 0).toInt();

    // Set Baud Rate Input
    switch(temp_baudRate)
    {
    case QSerialPort::Baud9600:
        ui->baudRateBox->setCurrentIndex(0);
        break;
    case QSerialPort::Baud19200:
        ui->baudRateBox->setCurrentIndex(1);
        break;
    case QSerialPort::Baud38400:
        ui->baudRateBox->setCurrentIndex(2);
        break;
    case QSerialPort::Baud115200:
        ui->baudRateBox->setCurrentIndex(3);
        break;
    default:
        ui->baudRateBox->setCurrentIndex(4);
        ui->baudRateBox->lineEdit()->setText(QString("%1").arg(temp_baudRate));
        break;
    }

    // Set Data Bit Input
    switch(temp_dataBits)
    {
    case QSerialPort::Data5:
        ui->dataBitsBox->setCurrentIndex(0);
        break;
    case QSerialPort::Data6:
        ui->dataBitsBox->setCurrentIndex(1);
        break;
    case QSerialPort::Data7:
        ui->dataBitsBox->setCurrentIndex(2);
        break;
    case QSerialPort::Data8:
        ui->dataBitsBox->setCurrentIndex(3);
        break;
    default:
        // TODO Error
        break;
    }

    // Set Parity Input
    switch(temp_parity)
    {
    case QSerialPort::NoParity:
        ui->parityBox->setCurrentIndex(0);
        break;
    case QSerialPort::EvenParity:
        ui->parityBox->setCurrentIndex(1);
        break;
    case QSerialPort::OddParity:
        ui->parityBox->setCurrentIndex(2);
        break;
    case QSerialPort::MarkParity:
        ui->parityBox->setCurrentIndex(3);
        break;
    case QSerialPort::SpaceParity:
        ui->parityBox->setCurrentIndex(4);
        break;
    default:
        // TODO Error
        break;
    }

    // Set Stop Bits Input
    switch(temp_stopBits)
    {
    case QSerialPort::OneStop:
        ui->stopBitsBox->setCurrentIndex(0);
        break;
#ifdef Q_OS_WIN
    case QSerialPort::OneAndHalfStop:
        ui->stopBitsBox->setCurrentIndex(1);
        break;
    case QSerialPort::TwoStop:
        ui->stopBitsBox->setCurrentIndex(2);
        break;
#else
    case QSerialPort::TwoStop:
        ui->stopBitsBox->setCurrentIndex(1);
        break;
#endif

    default:
        // TODO Error
        break;
    }

    // Set Flow Control Input
    switch(temp_flowControl)
    {
    case QSerialPort::NoFlowControl:
        ui->flowControlBox->setCurrentIndex(0);
        break;
    case QSerialPort::HardwareControl:
        ui->flowControlBox->setCurrentIndex(1);
        break;
    case QSerialPort::SoftwareControl:
        ui->flowControlBox->setCurrentIndex(2);
        break;
    default:
        // TODO Error
        break;
    }

    updateSettings();
}
