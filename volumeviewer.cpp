#include"volumeviewer.h"
#include"trading.h"
#include<QDebug>

VolumeViewer::VolumeViewer(EvolutionVolume * e, QWidget *parent) :
    QWidget(parent), evol(e)
{
    chart = new QChart();
    barset = new QBarSet(e->getPaireDevises().toString());

    QStringList categories;
    QBarCategoryAxis *axisX = new QBarCategoryAxis();

    for (int i = 0; i < e->getNbVolume(); i ++)
    {
        *barset << e->getVolume()[i].getVolume();
        categories << e->getVolume()[i].getDate().toString("MM.dd");
    }

    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);

    series = new QBarSeries();
    series->append(barset);
    chart->addSeries(series);

    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle("Volume");
    chart->setTheme(QChart::ChartThemeBlueCerulean);

    chartview = new QChartView(chart);
}
