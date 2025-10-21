#include "widget.h"
#include "ui_widget.h"
#include <QVector>
#include <QSerialPort>
#include <QSerialPortInfo>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ttl = new QSerialPort(this);
    serialBuffer = "";
    parsed_data = "";

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        QString pname = serialPortInfo.portName();
        ui->comboBox->addItem(pname);
    }
    Widget:setupPlot();
}

Widget::~Widget()
{
   if(ttl->isOpen()){
       ttl->close();
       QObject::disconnect(ttl, SIGNAL(readyRead()), this, SLOT(readSerial()));
   }

    delete ui;
}

void Widget::setupPlot(){

    x.resize(101);
    y.resize(101);
    z.resize(101);
    w.resize(101);
    for (int i=0; i<101; ++i)
    {
      x[i] = (double)i;
      y[i] = (double)2;
      z[i] = (double)i;
      w[i] = (double)4;
    }

    ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis2);
    ui->customPlot->graph(0)->setData(x, y);
    ui->customPlot->graph(1)->setData(z, w);
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(0)->setName("Vel_Angular");
    ui->customPlot->graph(1)->setName("Vel_Lineal");
    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Velocidad - Motor DC", QFont("sans", 12, QFont::Bold)));


    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    //ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    ui->customPlot->xAxis->setLabel("Time Relative");
    ui->customPlot->yAxis->setLabel("Vel_Angular.");
    ui->customPlot->xAxis->setRange(0, 100);
    ui->customPlot->yAxis->setRange(0, 500);
    ui->customPlot->yAxis2->setLabel("Vel_Lineal.");
    ui->customPlot->yAxis2->setRange(0, 1000);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->replot();
}


void Widget::makeplot(double vel_angular, double vel_lineal){

    for (int i=0; i<100; ++i)
    {
      y[i] = y[i + 1];
      w[i] = w[i + 1];
    }
    y[100] = vel_angular;
    w[100] = vel_lineal;
    ui->customPlot->graph(0)->setData(x, y);
    ui->customPlot->graph(1)->setData(z, w);
    ui->customPlot->replot();
}

void Widget::on_pushButton_clicked()
{
    if(ui->lineEdit->text() != ""){
        double rpm = ui->lineEdit->text().toDouble();
        double corriente = ui->lineEdit_2->text().toDouble();
        makeplot(rpm, corriente);
    }else{
        QMessageBox msgBox;
        msgBox.setText("Escriba un numero de 0 a 50 en el campo");
        msgBox.exec();
    }

}

void Widget::readSerial()
{
        QByteArray buffer = ttl->readAll();
        serialData.append(buffer);
        //Aqui se analizan los datos del protocolo
        //Se debe tener en cuenta el caso de "else" cuando no cumple con el protocolo
        //Aquí no está desarrollado el protocolo, deben poner la parte que ustedes desarrollaron
        if(serialData.at(serialData.length()-1) == '\n'){
            //Una vez analizado el protocolo, los datos deben ser enviados a la siguiente funcion:
            processSerial(serialData);
           serialData.clear();
        }

}

void Widget::processSerial(QByteArray datos){

    while (datos.contains("\r\n")) {
        int index = datos.indexOf("\r\n");
        QByteArray packet = datos.mid(0, index);
        datos.remove(0, index + 1);

        // Convertir el paquete a QString
        QString dataString = QString(packet).trimmed();

        if (dataString.startsWith("vel_medida: ")) {

            QString numberString = dataString.mid(12); // Longitud de "vel_medida: " es 12
            bool ok;
            ui->textEdit->append(numberString);
            vel_medida = numberString.toFloat(&ok);
        }else{
            vel_medida = 0;
        }
    }

    makeplot(vel_medida, vel_medida * 35); //33= radio de la rueda!

}

void Widget::on_pushButton_2_clicked()
{
    QString ttl_port_name = ui->comboBox->currentText();
    if(ui->pushButton_2->text() == "Abrir"){
            ttl->setPortName(ttl_port_name);
            ttl->open(QSerialPort::ReadWrite);
            ttl->setBaudRate(QSerialPort::Baud115200);
            ttl->setDataBits(QSerialPort::Data8);
            ttl->setFlowControl(QSerialPort::NoFlowControl);
            ttl->setParity(QSerialPort::NoParity);
            ttl->setStopBits(QSerialPort::OneStop);
            QObject::connect(ttl, SIGNAL(readyRead()), this, SLOT(readSerial()));
            ui->pushButton_2->setText("Cerrar");
    }else{
        ttl->close();
        QObject::disconnect(ttl, SIGNAL(readyRead()), this, SLOT(readSerial()));
        ui->pushButton_2->setText("Abrir");
    }

}

void Widget::on_pushButton_3_clicked()
{
    ui->comboBox->clear();

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        QString pname = serialPortInfo.portName();
        ui->comboBox->addItem(pname);

    }
}
