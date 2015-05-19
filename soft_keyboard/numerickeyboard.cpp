#include "numerickeyboard.h"

const char numericChars[] =
{
    '1','2','3','4','5','6','7','8','9','0',
    '/','\\',':',';','(',')','$','&','@','-',
    '\"','\'','.',',','<','>','?','!'
};

NumericKeyboard::NumericKeyboard(SoftKeyboard *parent) : SoftKeyboardMap(parent)
{
    connect(this, SIGNAL(update(QString)), parent, SLOT(appendText(QString)));
    // Letter Keys
    connect(&signalMapper, SIGNAL(mapped(QString)), this, SLOT(addCharacter(QString)));
    int i;
    for(i = 0; i < 10; ++i)
    {
        QPushButton *key = new QPushButton(this);
        key->setText(QString(numericChars[i]).toUpper());
        key->setGeometry((i*50)+5, 5, 50, 50);
        connect(key, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(key, QString(numericChars[i]));
    }
    for(i = 10; i < 20; ++i)
    {
        QPushButton *key = new QPushButton(this);
        key->setText(QString(numericChars[i]).toUpper());
        key->setGeometry(((i-10)*50)+5, 55, 50, 50);
        connect(key, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(key, QString(numericChars[i]));
    }
    for(i = 20; i < 28; ++i)
    {
        QPushButton *key = new QPushButton(this);
        key->setText(QString(numericChars[i]).toUpper());
        key->setGeometry(((i-20)*50)+5, 105, 50, 50);
        connect(key, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(key, QString(numericChars[i]));
    }
    //Space bar
    QPushButton *spaceBar = new QPushButton(this);
    spaceBar->setText("");
    spaceBar->setGeometry(130, 155, 250, 50);
    connect(spaceBar, SIGNAL(clicked()), &signalMapper, SLOT(map()));
    signalMapper.setMapping(spaceBar, QString(" "));

    // Done key
    QPushButton *done = new QPushButton(this);
    done->setText("Done");
    done->setGeometry(380, 155, 100, 50);
    connect(done, SIGNAL(clicked()), parent, SLOT(close()));
}
