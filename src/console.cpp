#include "console.h"

#include <QScrollBar>

Console::Console(QWidget *parent) :
    QTextBrowser(parent)
{
    document()->setMaximumBlockCount(100);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    pal.setColor(QPalette::Text, Qt::green);
    setPalette(pal);
}

void Console::putData(const QByteArray &data)
{
    insertPlainText(data);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}
