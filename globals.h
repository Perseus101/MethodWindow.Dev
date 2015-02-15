#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <QString>

#define SAMPLES 48
#define STEPS 3

const QString column_Headers[] = { QObject::tr("Waste Time"),
                                   QObject::tr("Sample Time"),
                                   QObject::tr("Total Time")};

#endif // GLOBALS_H
