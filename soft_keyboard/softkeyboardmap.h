#ifndef SOFTKEYBOARDMAP_H
#define SOFTKEYBOARDMAP_H

#include <QtCore>
#include <QWidget>

class SoftKeyboard;

class SoftKeyboardMap : public QWidget
{
    Q_OBJECT
public:
    SoftKeyboardMap(SoftKeyboard *parent);
    ~SoftKeyboardMap();
protected:
    QSignalMapper signalMapper;
signals:
    virtual void update(QString data);
public slots:
    virtual void init();
private slots:
    virtual void addCharacter(QString character);
};

#include "softkeyboard.h"

#endif // SOFTKEYBOARDMAP_H
