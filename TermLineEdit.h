#ifndef TERMLINEEDIT_H
#define TERMLINEEDIT_H

#include <QTextEdit>

class TermLineEdit : public QTextEdit
{
    Q_OBJECT
public:
    TermLineEdit();
    void sendMessage(QString state, QString message);
};

#endif // TERMLINEEDIT_H
