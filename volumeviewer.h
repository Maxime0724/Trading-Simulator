#ifndef VOLUMEVIEWER_H
#define VOLUMEVIEWER_H

#include<QObject>
#include"trading.h"

class VolumeViewer : public QWidget
{
    Q_OBJECT
    EvolutionVolume *evol;
    QChart *chart;
    QBarSet *barset;
    QBarSeries *series;
    QChartView *chartview;
public:
    explicit VolumeViewer(EvolutionVolume * evol, QWidget *parent = nullptr);
    ~VolumeViewer() {  }
    QChart* getChart() { return chart; }
    QChartView*  getChartview() { return chartview; }

signals:

public slots:
};

#endif // VOLUMEVIEWER_H
