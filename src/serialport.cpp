#include <serialport.h>

SerialPort::SerialPort(QMenu *menu, Console *console)
{
    m_serial = new QSerialPort;
    m_menu = menu;
    m_console = console;
    checkDevicesList();

    //set timer for updating device list
    timerCheckDevice = new QTimer;
    timerCheckDevice->setInterval(3000);
    timerCheckDevice->start();

    //set timer for updating connection time left
    timerInitDevice = new QTimer;
    timerInitDevice->setInterval(1000);

    QObject::connect(m_serial, &QSerialPort::errorOccurred, this, &SerialPort::handleError,Qt::AutoConnection);
    QObject::connect(this,&SerialPort::newDevice,this,&SerialPort::checkDevicesList,Qt::UniqueConnection); // connect signal from concurrent thread
    QObject::connect(timerCheckDevice,&QTimer::timeout,this,&SerialPort::checkNewConnections);
    QObject::connect(timerInitDevice,&QTimer::timeout,this,&SerialPort::initDeviceWaitFunc);
}

void SerialPort::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError || error == QSerialPort::ReadError)
    {
        disconnect(m_serial, &QSerialPort::errorOccurred, this, &SerialPort::handleError);
        QMessageBox::critical(nullptr, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
    checkDevicesList();
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialPort::handleError,Qt::AutoConnection);
}

//update device list
void SerialPort::checkDevicesList()
{
    //check for already connected device
    QList<QSerialPortInfo> ports_list = QSerialPortInfo::availablePorts();
    QMap<QString,QSerialPortInfo>::Iterator it;

    if(m_menu->isEmpty() || ports_list.isEmpty())
    {
        m_menu->clear();
        m_menu->addAction("No devices");
        devices.clear();
        stringDevice = nullptr;
        return;
    }

    if(ports_list.size() == devices.size())
        return;

    if(m_menu->actions().size() > ports_list.size())
    {
        QVector<int> num_data;
        int count = 0;
        for(auto elem : devices)
        {
            bool delete_item = true;
            for(auto elem_ports : ports_list)
            {
                if(elem.serialNumber() == elem_ports.serialNumber())
                {
                    delete_item = false;
                    break;
                }
            }

            if(delete_item)
            {
                num_data.push_back(count);
            }
            count++;
        }

        for(int k = 0; k < num_data.size(); k++)
        {
            devices.remove(num_data.at(k));
            m_menu->removeAction(m_menu->actions().at(num_data.at(k)));
        }
        return;
    }

    if(m_menu->actions().at(0)->text() == "No devices")
        m_menu->clear();

    for(auto elem : ports_list)
    {
        bool found = false;
        for(auto elem_devices : devices)
        {
            if(elem_devices.serialNumber() == elem.serialNumber())
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            devices.push_back(elem);
            //add menu port
            m_menu->addAction("&" + elem.portName() + " " + elem.manufacturer())->setData(elem.serialNumber());
            m_menu->actions().at(devices.size() - 1)->setCheckable(true);
            if(stringDevice == elem.serialNumber())
                m_menu->actions().at(devices.size() - 1)->setChecked(true);
            m_menu->actions().at(devices.size() - 1)->setChecked(false);
            connect(m_menu,&QMenu::triggered,this,&SerialPort::setSelectedDevice,Qt::UniqueConnection);
        }
    }
}

//choose Device from Ports
void SerialPort::setSelectedDevice(QAction *action)
{
    QString value = action->data().toString();
    bool found = false;
    int array_num = 0;

    for(auto elem_devices : devices)
    {
        if(elem_devices.serialNumber() == value)
        {
            found = true;
            break;
        }
        array_num++;
    }

    if(!found)
        return;

    if(m_serial->isOpen() && m_menu->actions().at(array_num)->isChecked())
        closeSerialPort();

    for(auto elem : m_menu->actions())
        elem->setChecked(false);

    if(!m_menu->actions().at(array_num)->isChecked())
    {
        m_menu->actions().at(array_num)->setChecked(true);
        stringDevice = value;
    }
    else
        stringDevice = nullptr;
}

void SerialPort::openSerialPort()
{
    if(stringDevice == nullptr)
    {
        QMessageBox::critical(nullptr,"Warning!","No Device Selected");
        emit enableButtonConnect();
        return;
    }

    if(m_serial->isOpen())
    {
        closeSerialPort();
        return;
    }

    QString device_name;
    QString portName;
    for(auto elem_devices : devices)
    {
        if(elem_devices.serialNumber() == stringDevice)
        {
            device_name = elem_devices.manufacturer();
            portName = elem_devices.portName();
            break;
        }
    }

    QString device_name_static = DEVICE_NAME;
    if(device_name.contains(device_name_static,Qt::CaseInsensitive))
        initDevice(portName);
    else
    {
        QMessageBox::critical(nullptr,"Invalid Device","Only connection to " + device_name_static +" is allowed!");
        emit enableButtonConnect();
    }
}

void SerialPort::initDevice(const QString portName)
{
    m_serial->setPortName(portName);
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    m_serial->open(QIODevice::ReadWrite);

    //wait for 3 seconds to setup arduino serial
    timerInitDevice->start();
    initDeviceWaitFunc();
}

void SerialPort::initDeviceWaitFunc()
{
    updateConsole(QString("Connecting..." + QString::number(timerInitDeviceCounter)).toLatin1());
    qApp->processEvents();
    timerInitDeviceCounter--;

    if(timerInitDeviceCounter < 0)
    {
        timerInitDevice->stop();
        timerInitDeviceCounter = 3; // 3 seconds to establish next connection
        startConnection();
    }
}

void SerialPort::startConnection()
{
    m_console->clear();
    emit setButtonConnectText("Disconnect");
    isConnected = true;
    emit sendData("request package"); //first request to device
    emit enableButtonConnect();
}

void SerialPort::closeSerialPort()
{
    m_serial->close();
    QString string = "Select device from Ports and Press Connect Button";
    updateConsole(string.toLatin1());
    isConnected = false;
    emit setButtonConnectText("Connect");
    QMessageBox::warning(nullptr,"Disconnect","Device " + m_serial->portName() + " disconnected");
    emit enableButtonConnect();
}

void SerialPort::updateConsole(QByteArray data)
{
    m_console->clear();
    m_console->putData(data);
}
