#include "softkeyboardmap.h"

SoftKeyboardMap::SoftKeyboardMap(SoftKeyboard *parent) : signalMapper(this)
{
    setWindowFlags(Qt::CustomizeWindowHint);

    connect(parent, SIGNAL(visibleChanged(bool)), this, SLOT(hide()));

    //Space bar
    QPushButton *spaceBar = new QPushButton(this);
    spaceBar->setText("");
    spaceBar->setGeometry(130, 155, 250, 50);
    connect(spaceBar, SIGNAL(clicked()), &signalMapper, SLOT(map()));
    signalMapper.setMapping(spaceBar, QString(" "));

    //Backspace Key
    QPushButton *back = new QPushButton(this);
    back->setText("Back");
    back->setGeometry(405, 105, 100, 50);
    connect(back, SIGNAL(clicked()), parent, SLOT(backspace()));

    // Done key
    QPushButton *done = new QPushButton(this);
    done->setText("Done");
    done->setGeometry(380, 155, 100, 50);
    connect(done, SIGNAL(clicked()), parent, SLOT(close()));

    // Switch Keyboard Key
    QPushButton *switchKeyboard = new QPushButton(this);
    switchKeyboard->setText("Switch");
    switchKeyboard->setGeometry(30, 155, 100, 50);
    connect(switchKeyboard, SIGNAL(clicked()), parent, SLOT(incrementKeyboard()));
}

SoftKeyboardMap::~SoftKeyboardMap()
{

}

void SoftKeyboardMap::init()
{

}

void SoftKeyboardMap::addCharacter(QString character)
{
    emit update(character);
}
