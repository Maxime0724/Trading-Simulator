#ifndef EVOLUTIONVIEWER_H
#define EVOLUTIONVIEWER_H
#include<QWidget>
#include<QLabel>
#include<QPushButton>
#include<QLineEdit>
#include<QtCharts>
#include<QDate>
#include"trading.h"
#include"analysebougie.h"
#include"analysefenetre.h"

class EvolutionViewer : public QWidget
{
    Q_OBJECT
    EvolutionCours & evolution;
    QCandlestickSeries* series; // un ensemble de bougies
    QChart *chart; // un graphique sur un ensemble de bougies
    QChartView *chartView; // un viewer de graphique
    QLineEdit* open; // barres d’édition
    QLineEdit* high;
    QLineEdit* low;
    QLineEdit* close;
    QLabel* openl; // labels
    QLabel* highl;
    QLabel* lowl;
    QLabel* closel;
    QPushButton *save; // bouton
    QPushButton *getforme;
    QHBoxLayout* copen; // couches de placement
    QHBoxLayout* chigh;
    QHBoxLayout* clow;
    QHBoxLayout* cclose;
    QVBoxLayout* coucheCours;
    QHBoxLayout* fenetre;
    Bougie *lastBougieClicked;



public:
    explicit EvolutionViewer(EvolutionCours& e, QWidget *parent = nullptr);
    ~EvolutionViewer() { }  
    QPushButton* getsave() { return save; }

signals:

private slots:
    void viewCoursOHLC(Bougie *c);
    void saveCoursOHLC();
    void analyse();

public slots:

};

#endif // EVOLUTIONVIEWER_H

