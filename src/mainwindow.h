#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <datahandler.h>
#include <QPushButton>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui = nullptr;
    DataHandler *dataHandler;
    Console *m_console = nullptr;
    QMenuBar *mb = nullptr;
    QMenu *menuPorts = nullptr;
    QPushButton *buttonConnect = nullptr;
    QPushButton *buttonRequest = nullptr;

    void showStatusMessage(const QString &message);
    void closeEvent(QCloseEvent *event) override;
    void closeEventDataHandler();
    void consolePutData(QByteArray data);
    void initActionsConnections();
    SerialPort* getSerialPort() const { return dataHandler->getSerialPort(); }

private slots:
    void about();
    void quit() { close(); };
    void clearConsole() { m_console->clear(); };
    void buttonConnectClicked();
    void buttonAutoRequestClicked();
    void enableButtonConnect() { buttonConnect->setEnabled(true); };
    void setButtonConnectText(QString text) { buttonConnect->setText(text); };
};

#endif // MAINWINDOW_H
