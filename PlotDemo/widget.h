#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QSerialPort>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void makeplot(double rpm, double corriente_ma);
    void on_pushButton_clicked();

    void readSerial();
    void processSerial(QByteArray datos);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Widget *ui;
    void setupPlot();
    //double x[100],y[100];
    QVector<double> x;
    QVector<double> y;
    QVector<double> z;
    QVector<double> w;
    QSerialPort *ttl;
    static const quint16 ttl_vendor_id = 9476;
    static const quint16 ttl_product_id = 768;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    QTimer *timer; // Temporizador
    int tiempo_ms; // Contador de tiempo en milisegundos
    float tiempo = 0;
    float vel_medida = 0;
};

#endif // WIDGET_H
