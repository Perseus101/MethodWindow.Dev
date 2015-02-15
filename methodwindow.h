/****************************************************************************
** MethodWindow  for TouchScreen
** HeaderFile
** DDM 9-10-2014
** Designed for Touch screen Method development for TransDermDrip
** 2012 - Nov. 2014
** methodwindow is core of application to dev time dependent Drip control
****************************************************************************/


#ifndef METHODWINDOW_H
#define METHODWINDOW_H
#include <QtCore>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QVariant>
#include <QHostInfo>
#include <QTimeEdit>
#include "ui_methodwindow.h"

#include "globals.h"
#include "methodtablemodel.h"
#include "methodrun.h"
#include "spinboxdelegate.h"

// #include "soft_keyboard/softkeyboard.h"

namespace Ui {
class MethodWindow;
}

class MethodWindow : public QMainWindow
{
    Q_OBJECT
public:
    MethodWindow(QWidget *parent = 0);
    ~MethodWindow();

private:
    Ui::NewMethod *ui;
    MethodRun *methodManager;
    MethodTableModel model;
    //HMSDelegate delegate;
    //SoftKeyboard *softKeyboard;
    SettingsDialog *serialComms;
//    QLabel *mPumpStat;

    int settingsLoaded;

protected:
    void setupActions();
    void setupTables();
    void setupRecentSettings();
    void setupRecentFiles();
    void setupSoftKeyboard();

    void writeSettings();
    void readSettings();

    bool mayDiscardDocument();
    void saveFile(const QString&);

    void closeEvent(QCloseEvent *event);

protected slots:
    void newFile();
    void loadFile();
    void saveFile();
    void saveFileAs();
    void saveSettingsFile();
    void loadSettingsFile(int num = 0);
    void undo();
    void redo();
    void copy();
    void about();

    void cleanup();

};

#endif // METHODWINDOW_H
