#include "gender_diagram.h"
#include "ui_gender_diagram.h"
//#include <qcustomplot.h>
#include <QPainter>
#include <QDebug>

gender_diagram::gender_diagram(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gender_diagram)
{
    ui->setupUi(this);
    ui->legend_widget->setObjectName("Gender Diagram");

}

void gender_diagram::set_data(int men, int women)
{
    _men = men;
    _women = women;
    //qDebug() << "kek men " << _men << "women_ " << _women;
    repaint();
    update();

}

void gender_diagram::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    QRectF size;
    pen.setColor(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);

    if (this->height() > this->width())
        size = QRectF (5, 5, this->width() - 10, this->width() - 10);
    else
        size = QRectF (5, 5, this->height() - 10, this->height() - 10);

    int sum = _men + _women;
    double start_angle = 0.0;
    double end_angle;

    //double men_percent = 180 / sum;
    double men_percent = _men / static_cast<double>(sum);
    //qDebug() << "pers " << men_percent;
    double men_angle = men_percent * 360;
    end_angle = start_angle + men_angle;
    //qDebug() << end_angle << " " << start_angle << " " << men_angle;
    painter.setBrush(Qt::blue);
    painter.drawPie(size, start_angle * 16, men_angle * 16);
    start_angle = end_angle;

    double women_percent = _women / static_cast<double>(sum);
    double women_angle = women_percent * 360;
    end_angle = start_angle + women_angle;
    painter.setBrush(Qt::red);
    painter.drawPie(size, start_angle * 16, women_angle * 16);



}


gender_diagram::~gender_diagram()
{
    delete ui;
}
