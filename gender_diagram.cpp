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
    ui->legend_widget->setObjectName("lel");

    //ui->legend_widget->setBackingStore();

    /*QCPBars *men_bar = new QCPBars(ui->legend_widget->xAxis, ui->legend_widget->yAxis);
    QCPBars *women_bar = new QCPBars(ui->legend_widget->xAxis, ui->legend_widget->yAxis);

    ui->legend_widget->addPlottable(men_bar);
    ui->legend_widget->addPlottable(women_bar);
    ui->legend_widget->addAction(men_bar);


    men_bar->setName("Men");
    pen.setColor(QColor(1, 92, 191));
    men_bar->setPen(pen);
    men_bar->setBrush(QColor(1, 92, 191, 50));
    women_bar->setName("Women");
    pen.setColor(QColor(150, 222, 0));
    women_bar->setPen(pen);
    women_bar->setBrush(QColor(150, 222, 0, 70));

    ui->legend_widget->legend->setVisible(true);
    ui->legend_widget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    ui->legend_widget->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    ui->legend_widget->legend->setBorderPen(legendPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->legend_widget->legend->setFont(legendFont);
    ui->legend_widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);*/

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

    //int sum = 360;
    int sum = _men + _women;
    //qDebug() << sum << "kek";
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

    //double women_percent = 180 / 360;
    double women_percent = _women / static_cast<double>(sum);
    double women_angle = women_percent * 360;
    end_angle = start_angle + women_angle;
    painter.setBrush(Qt::red);
    painter.drawPie(size, start_angle * 16, women_angle * 16);



//    QCPBars *men_bar = new QCPBars(ui->legend_widget->xAxis, ui->legend_widget->yAxis);
//    QCPBars *women_bar = new QCPBars(ui->legend_widget->xAxis, ui->legend_widget->yAxis);

//    ui->legend_widget->addPlottable(men_bar);
//    ui->legend_widget->addPlottable(women_bar);


//    men_bar->setName("Men");
//    pen.setColor(QColor(1, 92, 191));
//    men_bar->setPen(pen);
//    men_bar->setBrush(QColor(1, 92, 191, 50));
//    women_bar->setName("Women");
//    pen.setColor(QColor(150, 222, 0));
//    women_bar->setPen(pen);
//    women_bar->setBrush(QColor(150, 222, 0, 70));

//    ui->legend_widget->legend->setVisible(true);
//    ui->legend_widget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
//    ui->legend_widget->legend->setBrush(QColor(255, 255, 255, 200));
//    QPen legendPen;
//    legendPen.setColor(QColor(130, 130, 130, 200));
//    ui->legend_widget->legend->setBorderPen(legendPen);
//    QFont legendFont = font();
//    legendFont.setPointSize(10);
//    ui->legend_widget->legend->setFont(legendFont);
//    ui->legend_widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}


gender_diagram::~gender_diagram()
{
    delete ui;
}
