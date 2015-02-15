/****************************************************************************
** main  for TouchScreen
** main.cpp
** DDM 9-10-2014
** Designed for Touch screen Method development for TransDermDrip
** 2012 - Nov. 2014
** methodwindow is core of application to dev. time dependent Drip control
****************************************************************************/

#include <stdio.h>

#include <QCoreApplication>
#include <QApplication>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTableView>

#include "methodwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MethodWindow method;

    return app.exec();
}


