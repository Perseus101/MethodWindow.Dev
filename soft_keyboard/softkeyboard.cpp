#include "softkeyboard.h"

#include "characterkeyboard.h"
#include "numerickeyboard.h"

SoftKeyboard::SoftKeyboard() : QWindow(), m_data(),
    target(NULL), initialized(false), handle_uid(0), keyboard_uid(0)
  //, trueClose(false)
{
    create();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focus(QWidget*,QWidget*)));

    CharacterKeyboard *alphaKeyboard = new CharacterKeyboard(this);
    addKeyboardMap(alphaKeyboard);
    NumericKeyboard *numericKeyboard = new NumericKeyboard(this);
    addKeyboardMap(numericKeyboard);
    currentKeyMap = 0;
}

SoftKeyboard::~SoftKeyboard()
{
}

// Public

void SoftKeyboard::addToHandle(QWidget* w)
{
    w->setAccessibleName(QString("%1").arg(handle_uid));
    handle.push_back(w->accessibleName());
    handle_uid++;
}

void SoftKeyboard::addKeyboardMap(SoftKeyboardMap *map)
{
    keyboards[keyboard_uid] = map;
    keyboard_uid++;
}

// Protected

void SoftKeyboard::exposeEvent(QExposeEvent *e)
{
    if(isExposed() && isTopLevel())
    {
        getCurrentKeyMap()->setGeometry(geometry());
        getCurrentKeyMap()->raise();
    }
    else
    {
        close();
    }
}

void SoftKeyboard::focusInEvent(QFocusEvent *e)
{
}

void SoftKeyboard::focusOutEvent(QFocusEvent *e)
{
}

void SoftKeyboard::moveEvent(QMoveEvent *e)
{
    getCurrentKeyMap()->setGeometry(geometry());
    getCurrentKeyMap()->raise();
}

void SoftKeyboard::resizeEvent(QResizeEvent *e)
{
    getCurrentKeyMap()->setGeometry(QRect(QPoint(geometry().topLeft().x()-10, geometry().topLeft().y()-10), geometry().size()));
    getCurrentKeyMap()->raise();
}

//Private
bool SoftKeyboard::in_handle(QWidget* w)
{
    if(w != NULL)
        for(std::vector<QString>::iterator it = handle.begin(); it != handle.end(); ++it)
            if(w->accessibleName().toStdString() == it->toStdString())
                return true;
    return false;
}

SoftKeyboardMap *SoftKeyboard::getCurrentKeyMap()
{
    return (keyboards[currentKeyMap]);
}

// Slots
void SoftKeyboard::focus(QWidget *out, QWidget *in)
{
    if(in != NULL && out != NULL)
    {
        if(in_handle(in)) // Start Keyboard
        {
            target = qobject_cast<QLineEdit *>(in);
            init();
        }
    }
}

void SoftKeyboard::init()
{
    if(!initialized)
    {
        initialized = true;

        // Initialize or Reinitialize Data
        m_data = target->text();

        getCurrentKeyMap()->init();

        connect(this, SIGNAL(update(QString)), target, SLOT(setText(QString)));
    }
    // Show keyboard
    show();
    raise();

    getCurrentKeyMap()->show();
    getCurrentKeyMap()->activateWindow();
    getCurrentKeyMap()->raise();
}

void SoftKeyboard::close()
{
    if(initialized)
    {
        initialized = false;

        disconnect(target, SLOT(setText(QString)));
        target = NULL;

        getCurrentKeyMap()->hide();
        hide();
    }
}

void SoftKeyboard::cleanup()
{
    close();
    destroy();
}
void SoftKeyboard::incrementKeyboard()
{
//    QRect temp = getCurrentKeyMap()->geometry();
    getCurrentKeyMap()->hide();
    if(currentKeyMap == keyboard_uid-1)
        currentKeyMap = 0;
    else
        currentKeyMap++;
    getCurrentKeyMap()->setGeometry(geometry());
    getCurrentKeyMap()->show();
//    qDebug("Test:");
//    qDebug((QString("%1").arg(geometry().x())).toStdString().c_str());
//    qDebug((QString("%1").arg(geometry().y())).toStdString().c_str());
//    qDebug((QString("%1").arg(geometry().height())).toStdString().c_str());
//    qDebug((QString("%1").arg(geometry().width())).toStdString().c_str());
//    qDebug("Test2:");
//    qDebug((QString("%1").arg(temp.x())).toStdString().c_str());
//    qDebug((QString("%1").arg(temp.y())).toStdString().c_str());
//    qDebug((QString("%1").arg(temp.height())).toStdString().c_str());
//    qDebug((QString("%1").arg(temp.width())).toStdString().c_str());
}
