#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextBrowser>

class Console : public QTextBrowser
{
    Q_OBJECT

signals:
    void getData(const QByteArray &data);

public:
    explicit Console(QWidget *parent = nullptr);
    void putData(const QByteArray &data);
    void setLocalEchoEnabled(const bool &set) { m_localEchoEnabled = set; }

private:
    bool m_localEchoEnabled = false;
};


#endif // CONSOLE_H
