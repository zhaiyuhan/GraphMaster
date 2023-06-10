#include "TermLineEdit.h"

TermLineEdit::TermLineEdit()
{
    this->setReadOnly(true);
}

void TermLineEdit::sendMessage(QString state, QString message)
{
    // 获取 QTextCursor 实例
    QTextCursor cursor = this->textCursor();
    // 创建 QTextCharFormat 实例并设置颜色
    QTextCharFormat format;
    if(state == "ERROR") {
        format.setForeground(Qt::red);
    }else if(state == "SUCCESS") {
        format.setForeground(Qt::green);
    }
    // 将格式应用于 QTextCursor 处理的文本区域
    cursor.setCharFormat(format);
    // 在 QTextCursor 处理的文本区域插入文本
    cursor.setCharFormat(format);
    cursor.insertText(message+"\n");
}
