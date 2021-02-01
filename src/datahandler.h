#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <serialport.h>
#include <QTime>

struct DataStruct
{
    float x;
    float y;
    float z;
    float w;
    QVector<int> precision;
};

class DataHandler : public QObject
{
    Q_OBJECT
private:
        QString income_data;
        QSerialPort *m_serial = nullptr;
        SerialPort *serialPort;
        Console *m_console;
        bool autoRequest = true;
        DataStruct dataStruct;
        QString upload;
        QVector<float> floats;
        QVector<int> precision;
        bool isLength = false;
        int precisionCount = 0; //count precision array
        int dataLength = 0; //read number of digits in package
        int uploadLength = 0; //count income number of digits
        int min = 0; //number of first symbol
        int max = 0; //number of last symbol aka space

        bool getData(int iteration);
        void checkPackageLength(QString &input);
        bool convertationCheck(const QString text);

public:
    DataHandler(Console *console = nullptr, QMenu *menuPorts = nullptr);
    SerialPort* getSerialPort() const { return serialPort; }
    bool getAutoRequest()       const { return autoRequest; }
    void setAutoRequest(const bool &status) { autoRequest = status; }
    void sendData(const QString &bytes);

private slots:
    void readData();
};

#endif // DATAHANDLER_H
