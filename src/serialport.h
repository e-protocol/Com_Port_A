#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QMap>
#include <QMenu>
#include <QApplication>
#include <QTimer>
#include <console.h>

#define DEVICE_NAME "Arduino"

class SerialPort : public QSerialPort
{
    Q_OBJECT
private:
    QSerialPort *m_serial;
    QVector<QSerialPortInfo> devices;
    QString stringDevice = nullptr;
    QMenu *m_menu = nullptr;
    Console *m_console;
    QTimer *timerCheckDevice;
    QTimer *timerInitDevice;
    bool isConnected = false;
    int timerInitDeviceCounter = 3; // 3 seconds to establish connection

    void checkDevicesList();
    void checkNewConnections() { emit newDevice(); } //call update devices list method every 3 seconds
    void initDevice(const QString portName);
    void initDeviceWaitFunc();
    void updateConsole(QByteArray data);
    void startConnection();

public:
    SerialPort(QMenu *menu, Console *console);
    void openSerialPort();
    void closeSerialPort();
    bool getIsConnected() {return isConnected; }
    QTimer* getTimer() { return timerCheckDevice; }
    QSerialPort* getSerialDevice() {return m_serial;}

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void setSelectedDevice(QAction*);

signals:
    bool newDevice();
    void sendData(QString str);
    void enableButtonConnect();
    void setButtonConnectText(QString text);
};

#endif // SERIALPORT_H
