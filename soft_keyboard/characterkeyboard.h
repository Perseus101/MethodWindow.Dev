#ifndef CHARACTERKEYBOARD_H
#define CHARACTERKEYBOARD_H

#include <QtCore>
#include <QWidget>
#include <QSignalMapper>
#include <QPushButton>
#include <QString>

#include "softkeyboardmap.h"

class CharacterKeyboard : public SoftKeyboardMap
{
    Q_OBJECT

public:
    CharacterKeyboard(SoftKeyboard *parent);
    ~CharacterKeyboard();
signals:
    void update(QString data);
public slots:
    void init();
private:
    QPushButton *capButton;
    bool canCapsLock, capsLock, caps;
private slots:
    void addCharacter(QString character);
    void toggleCaps(bool checked);
    void expireCapsLock() { canCapsLock = false; }
};

#endif // CHARACTERKEYBOARD_H
