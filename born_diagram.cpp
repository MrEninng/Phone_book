#include "born_diagram.h"
#include "ui_born_diagram.h"

#include <QPainter>
#include <QDebug>
#include <QSqlQuery>

born_diagram::born_diagram(QWidget *parent) : QMainWindow(parent), ui(new Ui::born_diagram)
{
    ui->setupUi(this);

    //добываем данные из базы
    QSqlQuery query;
    QStringList list_of_years = {};
    query.exec("SELECT date from phone_book_info");
    //считываем из базы данных даты
    while (query.next()) {
        list_of_years.append(query.value(0).toString().split('.'));
    }

    //удаление из списка дней и месяцев
    for (int i = 0; i < list_of_years.length(); i++) {
        if (list_of_years[i].toInt() < 1000)
            list_of_years.erase(list_of_years.begin() + i);
    }
    for (int i = 0; i < list_of_years.length(); i++)
        if (list_of_years[i].length() < 4)
            list_of_years.erase(list_of_years.begin() + i);

    QSet<QString> _set_of_years = QSet<QString>::fromList(list_of_years);
    QList<QString> set_of_years = _set_of_years.toList();
    //сортируем список по годам
    qSort(set_of_years.begin(), set_of_years.end());
    //qDebug() << list_of_years;
    //qDebug() << set_of_years;
    QStringList amount_list_of_years = {};
    for (int i = 0; i < set_of_years.length(); i++) {
        int amount = 0;
        int year = set_of_years[i].toInt();
        for (int j = 0; j < list_of_years.length(); j++)
            if (year == list_of_years[j].toInt())
                amount++;

        amount_list_of_years.append(QString::number(amount));
    }

    //qDebug() << amount_list_of_years;
    //qDebug() << set_of_years;

    QCPBars *year = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addPlottable(year);

    QPen pen;
    pen.setWidthF(1.2);
    year->setName("Years");
    pen.setColor(QColor(255, 131, 0));
    year->setPen(pen);
    year->setBrush(QColor(255, 131, 0, 50));
    int max = amount_list_of_years[0].toInt();
    for (int i = 1; i < amount_list_of_years.length(); i++)
        if (amount_list_of_years[i].toInt() > max)
            max = amount_list_of_years[i].toInt();
    //qDebug() << max;

    QVector<double> y_axis;
    for (int i = 0; i < max; i++)
        y_axis.append(i + 1);

    //qDebug() << y_axis;

    QVector<double> x_axis;
    for (int i = 0; i < amount_list_of_years.length(); i++)
        x_axis.append(amount_list_of_years[i].toDouble());

    //qDebug() << x_axis;

    ui->customPlot->xAxis->setAutoTicks(false);
    ui->customPlot->xAxis->setAutoTickLabels(false);
    ui->customPlot->xAxis->setTickVector(y_axis);
    ui->customPlot->xAxis->setTickVectorLabels(QVector<QString>::fromList(set_of_years));
    ui->customPlot->xAxis->setTickLabelRotation(60);
    ui->customPlot->xAxis->setSubTickCount(0);
    ui->customPlot->xAxis->setTickLength(0, 4);
    ui->customPlot->xAxis->grid()->setVisible(true);
    ui->customPlot->xAxis->setRange(0, x_axis.length() + 1);

   // prepare y axis:
    ui->customPlot->yAxis->setRange(0, max + 1);
    ui->customPlot->yAxis->setPadding(5); // a bit more space to the left border
    //ui->customPlot->yAxis->setLabel("Power Consumption in\nKilowatts per Capita (2007)");
    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    ui->customPlot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    ui->customPlot->yAxis->grid()->setSubGridPen(gridPen);
    year->setData(y_axis, x_axis);
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    ui->customPlot->legend->setBorderPen(legendPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

born_diagram::~born_diagram()
{
    delete ui;
}

