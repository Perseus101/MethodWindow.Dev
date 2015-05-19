#include "characterkeyboard.h"

const char keyChars[] =
{
  'q','w','e','r','t','y','u','i','o','p',  //'[',']','\\',
  'a','s','d','f','g','h','j','k','l',      //';','\'',
  'z','x','c','v','b','n','m'               //,',','.','/'
};

CharacterKeyboard::CharacterKeyboard(SoftKeyboard *parent) : SoftKeyboardMap(parent),
    canCapsLock(false), capsLock(false), caps(false)
{
    connect(this, SIGNAL(update(QString)), parent, SLOT(appendText(QString)));
    // Letter Keys
    connect(&signalMapper, SIGNAL(mapped(QString)), this, SLOT(addCharacter(QString)));
    int i;
    for(i = 0; i < 10; ++i)
    {
        QPushButton *key = new QPushButton(this);
        key->setText(QString(keyChars[i]).toUpper());
        key->setGeometry((i*50)+5, 5, 50, 50);
        connect(key, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(key, QString(keyChars[i]));
    }
    for(i = 10; i < 19; ++i)
    {
        QPushButton *key = new QPushButton(this);
        key->setText(QString(keyChars[i]).toUpper());
        key->setGeometry(((i-10)*50)+30, 55, 50, 50);
        connect(key, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(key, QString(keyChars[i]));
    }
    for(i = 19; i < 26; ++i)
    {
        QPushButton *key = new QPushButton(this);
        key->setText(QString(keyChars[i]).toUpper());
        key->setGeometry(((i-19)*50)+55, 105, 50, 50);
        connect(key, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(key, QString(keyChars[i]));
    }

    //Caps key
    capButton = new QPushButton(this);
    // TODO caps->setIcon();
    capButton->setText("caps");
    capButton->setCheckable(true);
    capButton->setChecked(false);
    capButton->setGeometry(5, 105, 50, 50);
    connect(capButton, SIGNAL(toggled(bool)), this, SLOT(toggleCaps(bool)));
}

CharacterKeyboard::~CharacterKeyboard()
{

}

// Public Slots

void CharacterKeyboard::init()
{
    canCapsLock = false;
    capsLock = false;
    caps = false;
}

// Private Slots

void CharacterKeyboard::toggleCaps(bool checked)
{
    if(canCapsLock)
    {
        capsLock = true;
        canCapsLock = false;
        capButton->setText("CAPS");
        capButton->setChecked(false);
        return;
    }
    if(capsLock)
    {
        canCapsLock = false;
        capButton->setChecked(false);
        checked = false;
    }
    if(checked)
    {
        capButton->setText("Caps");
        QTimer::singleShot(250, this, SLOT(expireCapsLock()));
        canCapsLock = true;
        caps = true;
        return;
    }
    else
    {
        canCapsLock = false;
        capsLock = false;
        caps = false;
        capButton->setText("caps");
        return;
    }
}

void CharacterKeyboard::addCharacter(QString character)
{
    emit update((caps || capsLock) ? character.toUpper(): character);
    if(caps && !capsLock)
    {
        canCapsLock = false;
        capsLock = false;
        caps = false;
        capButton->setText("caps");
        capButton->setChecked(false);
    }
}
