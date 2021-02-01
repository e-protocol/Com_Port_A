#include <datahandler.h>

DataHandler::DataHandler(Console *console, QMenu *menu_ports)
{
    serialPort = new SerialPort(menu_ports,console);
    m_serial = serialPort->getSerialDevice();
    m_console = console;

    QObject::connect(m_serial, &QSerialPort::readyRead, this, &DataHandler::readData,Qt::UniqueConnection);
    QObject::connect(serialPort, &SerialPort::sendData, this, &DataHandler::sendData,Qt::UniqueConnection);
}

//read data on recieve
void DataHandler::readData()
{
    const QByteArray data = m_serial->readAll();
    QString income = QString::fromLatin1(data); //convert byteArray to string

    //check for data full
    income_data += income;
    if(!income_data.contains("\n"))
        return;

    //get data from string
    upload.clear();
    floats.clear();
    precision.clear();
    isLength = false;
    min = 0;
    max = 0;
    precisionCount = 0; //count precision array
    dataLength = 0; //read number of digits in package
    uploadLength = 0; //count income number of digits

    for(int k = 0; k < income_data.size(); k++)
       if(!getData(k))
            break;

    QString input = income_data;
    income_data.clear();

    //check package length
    checkPackageLength(input);

    if(autoRequest)
        sendData("request package");
}

//request data from device
void DataHandler::sendData(const QString &bytes)
{
    m_console->putData("@" + QTime::currentTime().toString("hh:mm:ss").toLatin1() + " send: " + bytes.toLatin1() + "\n");
    m_serial->flush();
    m_serial->write(bytes.toLatin1(),bytes.size() + 1);
}

bool DataHandler::getData(int k)
{
    if((min == 0 && income_data[k] == ' '))
    {
        min = k + 1;
        return true;
    }
    if((max == 0 && (income_data[k] == ' ' || income_data[k] == '\n')))
        max = k;
    if(max != 0)
    {
        for(int j = min; j < max; j++)
        {
            if(income_data[j] != ' ')
                upload.push_back(income_data[j]);
        }
        min = max + 1;
        max = 0;

        //save data_length
        if(!isLength)
        {
            if(!convertationCheck("int"))
                return false;

            dataLength = upload.toInt(); //convert byte to number
            isLength = true;
        }
        //save precisions in array
        else if(precisionCount < 4)
        {
            if(!convertationCheck("precision"))
                return false;

            precision.push_back(upload.toInt());
            precisionCount++;
            uploadLength += upload.length(); //count income number of digits
        }
        //save float values
        else
        {
            if(!convertationCheck("float"))
                return false;

            floats.push_back(upload.toFloat());
            uploadLength += upload.length(); //count income number of digits
        }
        upload.clear();
    }
    return true;
}

bool DataHandler::convertationCheck(const QString text)
{
    QString message = "Convertation failed: data ";
    message.append(text);

    if( (text != "float" && !upload.toInt())
            || !upload.toFloat() )
    {
        qDebug() << message;
        return false;
    }
    return true;
}

void DataHandler::checkPackageLength(QString &input)
{
    if(dataLength == uploadLength - 1)
    {
        //create copy of struct for further data work
        DataStruct dataStruct{floats[0],floats[1],floats[2],floats[3],precision};

        //show that we recieved data and store it in struct
        input = "@" + QTime::currentTime().toString("hh:mm:ss") + " recieved:\n" + "data length = " + QString::number(dataLength) +
                "\nfloat x = " + QString::number(dataStruct.x,'f', dataStruct.precision[0]) + "  (precision: " + QString::number(dataStruct.precision[0]) + ")" +
                "\nfloat y = " + QString::number(dataStruct.y,'f', dataStruct.precision[1]) + "  (precision: " + QString::number(dataStruct.precision[1]) + ")" +
                "\nfloat z = " + QString::number(dataStruct.z,'f', dataStruct.precision[2]) + "  (precision: " + QString::number(dataStruct.precision[2]) + ")" +
                "\nfloat w = " + QString::number(dataStruct.w,'f', dataStruct.precision[3]) + "  (precision: " + QString::number(dataStruct.precision[3]) + ")" +  "\n";
    }
    else
        input = "@" + QTime::currentTime().toString("hh:mm:ss") + " recieved: Invalid package! Request package\n";

    m_console->putData(input.toLatin1());
}
