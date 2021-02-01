#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_console = new Console();
    buttonConnect = new QPushButton("Connect");
    buttonRequest = new QPushButton("Auto Request: ON");
    QWidget *widget = new QWidget(this);
    QVBoxLayout *v_layout = new QVBoxLayout;
    QHBoxLayout *h_layout = new QHBoxLayout();
    v_layout->addWidget(m_console);
    v_layout->addLayout(h_layout);
    h_layout->addWidget(buttonConnect,  1,Qt::AlignLeft);
    h_layout->addWidget(buttonRequest,100,Qt::AlignLeft);
    widget->setLayout(v_layout);
    setCentralWidget(widget);

    mb = new QMenuBar;
    //menuBar File
    //Information
    QMenu * menu = new QMenu("&File");
    menu->addAction("&About",this, &MainWindow::about);
    //Clear Console
    menu->addAction("&Clear",this, &MainWindow::clearConsole);
    //Exit
    menu->addAction("&Exit", this, &MainWindow::quit);
    mb->addMenu(menu);

    //menuBar Ports
    menuPorts = new QMenu("&Ports");
    mb->addMenu(menuPorts);
    this->setMenuBar(mb);

    QString string = "Select device from Ports and Press Connect Button";
    m_console->putData(string.toUtf8());

    dataHandler = new DataHandler(m_console,menuPorts);

    connect(buttonConnect,&QPushButton::clicked,this,&MainWindow::buttonConnectClicked,Qt::UniqueConnection);// button connect/disconnect listener
    connect(buttonRequest,&QPushButton::clicked,this,&MainWindow::buttonAutoRequestClicked,Qt::UniqueConnection);// button connect/disconnect listener
    connect(getSerialPort(),&SerialPort::enableButtonConnect,this,&MainWindow::enableButtonConnect,Qt::UniqueConnection); //enableButton listener
    connect(getSerialPort(),&SerialPort::setButtonConnectText,this,&MainWindow::setButtonConnectText,Qt::UniqueConnection); //updateButton text listener
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::about()
{
    QString message = "This is a transfering data example between Arduino and Computer via Com port. "
                      "App recieves float numbers from device by request. Created by E-Protocol";
    QMessageBox::information(this,"About",message);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton question;
    question = QMessageBox::question(this,"Exit","Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No);
    if(question == QMessageBox::Yes)
    {
        closeEventDataHandler();
    }
    else
        event->ignore();
}

void MainWindow::closeEventDataHandler()
{
    getSerialPort()->getTimer()->stop(); //stop timer
    qApp->exit(0);
}

void MainWindow::consolePutData(QByteArray data)
{
    clearConsole();
    m_console->putData(data);
}

void MainWindow::buttonConnectClicked()
{
    buttonConnect->setEnabled(false);

    if(getSerialPort()->getIsConnected())
        getSerialPort()->closeSerialPort();
    else
        getSerialPort()->openSerialPort();
}

void MainWindow::buttonAutoRequestClicked()
{
    QString requestText = "Auto Request: ";
    bool autoReq = dataHandler->getAutoRequest();

    requestText.append(autoReq ? "OFF" : "ON");
    autoReq ? autoReq = false : autoReq = true;
    dataHandler->setAutoRequest(autoReq);
    buttonRequest->setText(requestText);

    QString sendText;

    if(!autoReq && getSerialPort()->getIsConnected())
        sendText = "cancel auto request"; //cancel auto request
    else
        sendText = "request package"; //start auto request

    if(getSerialPort()->getIsConnected())
        dataHandler->sendData(sendText);
}
