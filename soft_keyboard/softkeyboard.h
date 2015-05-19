#ifndef SOFTKEYBOARD_H
#define SOFTKEYBOARD_H

#include <vector>
#include <map>

#include <QtCore>
#include <QWidget>
#include <QWindow>
#include <QPushButton>
#include <QSignalMapper>
#include <QTimer>
#include <QLineEdit>

#include "softkeyboardmap.h"

class SoftKeyboard : public QWindow
{
    Q_OBJECT

public:
    SoftKeyboard();
    ~SoftKeyboard();

    void addToHandle(QWidget* w);
    void addKeyboardMap(SoftKeyboardMap* map);

    bool isInitialized();

protected:

    void exposeEvent(QExposeEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    std::map<int, SoftKeyboardMap *> keyboards;
    std::vector<QString> handle;
    int handle_uid, keyboard_uid, currentKeyMap;
    QLineEdit *target;
    QString m_data;
    bool initialized;

    bool in_handle(QWidget* w);
    SoftKeyboardMap *getCurrentKeyMap();
signals:
    void update(QString data);

public slots:
    void cleanup();
    void backspace() { m_data.chop(1); emit update(m_data);}
    void appendText(QString data) { m_data.append(data); emit update(m_data); }
    void incrementKeyboard();
private slots:
    void focus(QWidget *out, QWidget *in);
    void init();
    void close();
};

#endif // SOFTKEYBOARD_H
