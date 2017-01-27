/****************************************************************************
** MethodWindow  for TouchScreen
** methodwindow.cpp
** DDM 9-10-2014
** Designed for Touch screen Method development for TransDermDrip
** 2012 - Nov. 2014
** methodwindow is core of application to dev. time dependent Drip control
****************************************************************************/

#include "methodwindow.h"

MethodWindow::MethodWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::NewMethod), model(0,SAMPLES,STEPS),
      serialComms(new SettingsDialog()),settingsLoaded(0)
{
    ui->setupUi(this);


    move(0, 0);

    methodManager = new MethodRun(&model, ui, serialComms);

    setupTables();
    setupActions();
    setupRecentSettings();
    setupRecentFiles();
//    setupSoftKeyboard();
//    mPumpStat = new QLabel;
//    statusBar()->addWidget(mPumpStat);

    this->show();
}

MethodWindow::~MethodWindow()
{
    delete ui;
    delete methodManager;
    //delete softKeyboard;
}

void MethodWindow::setupTables()
{
    // Data Table
    ui->tableView->setModel(&model);
    ui->tableView->setShowGrid(true);
   
    model.setHeaderData(0, Qt::Horizontal, QObject::tr("   --  Waste  --   "));
    model.setHeaderData(1, Qt::Horizontal, QObject::tr("   --  Sample  --  "));
    model.setHeaderData(2, Qt::Horizontal, QObject::tr("   --  Total   --  "));

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setWindowTitle(QObject::tr("Method Edit"));

    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), &model, SLOT(updateSelection(QItemSelection, QItemSelection)));

    connect(ui->wasteTime, SIGNAL(timeChanged(const QTime&)), &model, SLOT(updateWasteTime(const QTime&)));
    connect(&model, SIGNAL(setBaseWasteTime(QTime)), ui->wasteTime, SLOT(setTime(QTime)));
    ui->wasteTime->setDisplayFormat("HH:mm");
    ui->wasteTime->setTime(QTime(0,0,0));

    connect(ui->sampleTime, SIGNAL(timeChanged(const QTime&)), &model, SLOT(updateSampleTime(const QTime&)));
    connect(&model, SIGNAL(setBaseSampleTime(QTime)), ui->sampleTime, SLOT(setTime(QTime)));
    ui->sampleTime->setDisplayFormat("HH:mm");
    ui->sampleTime->setTime(QTime(0,0,0));

    connect(ui->copyToIndex, SIGNAL(valueChanged(int)), &model, SLOT(setPasteRow(int)));
    connect(ui->copyToAlt, SIGNAL(clicked()), &model, SLOT(paste()));

    ui->tableView->show();

    // Position Data
    posDatModel = new QStandardItemModel(SAMPLES, 1);
    ui->tablePosData->setModel(posDatModel);
    posDatModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Position (mm)"));
    for(int i = 0; i < SAMPLES; ++i)
    {
        posDatModel->setHeaderData(i, Qt::Vertical, QObject::tr((QString("Sample %1").arg(i + 1)).toStdString().c_str()));
    }
    ui->tablePosData->resizeRowsToContents();
    ui->tablePosData->resizeColumnsToContents();
    ui->tablePosData->horizontalHeader()->setStretchLastSection(true);

    ui->tablePosData->setItemDelegate(new SpinBoxDelegate());

    // Update Pos Data when position 1 and 13 are inputted

    connect(ui->positionOne, SIGNAL(valueChanged(double)), this, SLOT(pos_one(double)));
    connect(ui->positionThirteen, SIGNAL(valueChanged(double)), this, SLOT(pos_thirteen(double)));

    ui->tablePosData->show();
}

void MethodWindow::setupActions()
{
    connect(ui->actionExit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(loadFile()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(saveFile()));
    connect(ui->actionSave_As, SIGNAL(triggered(bool)), this, SLOT(saveFileAs()));
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(newFile()));

    connect(ui->actionSave_Settings_File, SIGNAL(triggered(bool)), this, SLOT(saveSettingsFile()));
    connect(ui->actionAbout_Method_Edit, SIGNAL(triggered(bool)), this, SLOT(about()));

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanup()));
}

void MethodWindow::setupRecentSettings()
{
    QSettings app_settings (QSettings::IniFormat, QSettings::UserScope, "MunnTech", "Flow Cell Control");

    app_settings.beginGroup("Settings_Files");

    int numSettingsFiles = app_settings.value("Num_Settings", 0).toInt();

    QSignalMapper *signalMapper = new QSignalMapper (this);
    for(int i = numSettingsFiles; i > settingsLoaded; --i)
    {
        QAction *action = ui->menuOpen_Settings_File->addAction(app_settings.value(QString("Settings_%1").arg(i)).toString());

        connect(action, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));

        signalMapper->setMapping(action, i);
    }

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(loadSettingsFile(int)));

    settingsLoaded = numSettingsFiles;

    app_settings.endGroup();
}

void MethodWindow::setupRecentFiles()
{
    QSettings app_settings (QSettings::IniFormat, QSettings::UserScope, "MunnTech", "Flow Cell Control");
    app_settings.beginGroup("Settings_Files");
//    QString name = app_settings.value("Last_Method","");
//    if (name != ""){
//        QFile file(name);
//        if (file.open(QIODevice::ReadOnly|QIODevice::Text)){
//            model.setDataFromCSV(QString::fromUtf8(file.readAll()));
//            model.m_FileName= name;
//            statusBar()->showMessage(tr("File successfully loaded."),3000);
//        }
//    }


    QString hostName = QHostInfo::localHostName();
    ui->lineEditSystemName->setText(hostName);
    int numSettingsFiles = app_settings.value("Num_Settings", 0).toInt();
    for(int i = numSettingsFiles; i > 0; --i)
    {
        if(app_settings.value(QString("Settings_%1").arg(i)).toString() == ui->lineEditSystemName->text())
        {
            loadSettingsFile(i);
        }
   }

    app_settings.endGroup();
}


//void MethodWindow::setupSoftKeyboard()
//{
//    softKeyboard = new SoftKeyboard();
//    softKeyboard->resize(520, 210);
//    softKeyboard->setPosition(75, 400);

//    softKeyboard->addToHandle(ui->lineEditSystemName);
//// Add more ui elements to the handle to have the soft keyboard open when those widgets are clicked
//}

void MethodWindow::closeEvent(QCloseEvent *e)
{
    cleanup();
    e->accept();
}

void MethodWindow::loadFile()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),
       "/home/pi/Method_Window/Methods/",
       tr("Methods (*.csv)"));
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)){
        model.setDataFromCSV(QString::fromUtf8(file.readAll()));
        model.m_FileName= filename;
        statusBar()->showMessage(tr("File successfully loaded."),3000);
    }
}

void MethodWindow::saveFile()
{
    if(model.m_FileName.isEmpty())
        saveFileAs();
    else
        saveFile(model.m_FileName);
}

void MethodWindow::saveFile(const QString &name)
{
    model.edited = false;
    QFile file(name);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        file.write(model.getDataAsCSV().toUtf8());
        statusBar()->showMessage(tr("File saved succcessfully."), 3000);
    }
}

void MethodWindow::saveFileAs()
{
    model.m_FileName = QFileDialog::getSaveFileName(this, tr("Save File"),
             "/home/pi/Method_Window/Methods/Method--" + QDateTime::currentDateTime().toString() +".csv",
              tr("Methods (*.csv)"));
    if(model.m_FileName.isEmpty())
        return;
    saveFile(model.m_FileName);
}



//        // Save this for future use at startup.
//        QSettings app_settings (QSettings::IniFormat, QSettings::UserScope, "MunnTech", "Flow Cell Control");
//        app_settings.beginGroup("Settings_Files");
//        app_settings.setValue("Last_Method", name->fileName());
//        app_settings.endGroup();

void MethodWindow::saveSettingsFile()
{

    ui->dateTimeEdit->setDate(QDate::currentDate());
    ui->dateTimeEdit->setTime(QTime::currentTime());

    // Add the new settings file to the list of saved settings files
    QSettings app_settings (QSettings::IniFormat, QSettings::UserScope, "MunnTech", "Flow Cell Control");

    app_settings.beginGroup("Settings_Files");
    int numSettingsFiles = app_settings.value("Num_Settings", 0).toInt();
    bool newSettings = true;

    // Ensure there isn't another set of settings under the same name.
    for(int i = numSettingsFiles; i > 0; --i)
    {

        if(app_settings.value(QString("Settings_%1").arg(i)).toString() == ui->lineEditSystemName->text())
        {
            newSettings = false;
            QMessageBox msgBox;
            msgBox.setText("A setup under this name already exists.");
            msgBox.setInformativeText("Do you want to overwrite it?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            switch(ret)
            {
            case QMessageBox::Save:
                break;
            case QMessageBox::Cancel:
                return;
                break;
            }
        }
    }

    // Add the new settings file to the list of settings.
    if(newSettings)
    {
        app_settings.setValue("Num_Settings", (++numSettingsFiles));
        app_settings.setValue(QString("Settings_%1").arg(numSettingsFiles), ui->lineEditSystemName->text());
    }

    app_settings.endGroup();

    // Save the new settings file

    QSettings settings (QSettings::IniFormat, QSettings::UserScope, "MunnTech", ui->lineEditSystemName->text());
    settings.beginGroup("Instrument_Settings");
    settings.setValue("Last_Saved", ui->dateTimeEdit->dateTime());
    settings.setValue("Position_One", ui->positionOne->text());
    settings.setValue("Position_Thirteen", ui->positionThirteen->text());


    serialComms->saveSettings(&settings);

    settings.endGroup();
    
    settings.beginGroup("Position_Data");
    
    for(int i = 0; i < SAMPLES; ++i)
    {
        settings.setValue(QString("Position_%1").arg(i), ui->tablePosData->model()->data(ui->tablePosData->model()->index(i,0)).toString());
    }
    
    settings.setValue("Waste_Position", ui->wastePosition->cleanText());
    settings.endGroup();

    setupRecentSettings();    
}

void MethodWindow::loadSettingsFile(int num)
{
    QSettings app_settings (QSettings::IniFormat, QSettings::UserScope, "MunnTech", "Flow Cell Control");

    switch(num)
    {
    case -1: //Browse
    {
        // TODO
    }
        break;
    default: // Open settings file num
    {
        app_settings.beginGroup("Settings_Files");
        ui->lineEditSystemName->setText(app_settings.value(QString("Settings_%1").arg(num), "").toString());
        app_settings.endGroup();
    }
        break;
    }
    QSettings settings (QSettings::IniFormat, QSettings::UserScope, "MunnTech", ui->lineEditSystemName->text());
    settings.beginGroup("Instrument_Settings");
    ui->dateTimeEdit->setDateTime(settings.value("Last_Saved","").toDateTime());
    ui->positionOne->setValue(settings.value("Position_One", "").toDouble());
    ui->positionThirteen->setValue(settings.value("Position_Thirteen", "").toDouble());

    serialComms->loadSettings(&settings);
    settings.endGroup();
    settings.beginGroup("Position_Data");

    for(int i = 0; i < SAMPLES; ++i)
    {
        ui->tablePosData->model()->setData(ui->tablePosData->model()->index(i,0), settings.value(QString("Position_%1").arg(i), 0));
    }
    ui->wastePosition->setValue(settings.value("Waste_Position", "").toFloat());
    settings.endGroup();
}

void MethodWindow::newFile()
{
    if (model.edited)
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch(ret)
        {
        case QMessageBox::Save:
            saveFile();
            break;
        case QMessageBox::Cancel:
            return;
            break;
        case QMessageBox::Discard:
            break;
        }
    }
    model.reinitialize();
    model.edited = false;
    model.refresh();
    model.m_FileName = "";
}

void MethodWindow::undo()
{
    // TODO
}

void MethodWindow::redo()
{
    // TODO
}

void MethodWindow::copy()
{
    // TODO
}

void MethodWindow::about()
{
    QMessageBox::about(this, tr("About Edit"),tr("Flow Cell Method Editor 1.2.3 \nA Qt application.\n""(c) 2014-2015 MunnTech, Open Source Material"));
}

void MethodWindow::cleanup()
{
//    softKeyboard->cleanup();
}

void MethodWindow::pos_one(double in)
{
    for(int i = 0; i < 12; ++i)
        posDatModel->setData(posDatModel->index(i,0), in + 9*i);
    for(int i = 24; i < 36; ++i)
        posDatModel->setData(posDatModel->index(i,0), in + 9*(i-24));
}

void MethodWindow::pos_thirteen(double in)
{
    for(int i = 12; i < 24; ++i)
        posDatModel->setData(posDatModel->index(i,0), in + 9*(i-12));
    for(int i = 36; i < 48; ++i)
        posDatModel->setData(posDatModel->index(i,0), in + 9*(i-36));
}
