#ifndef ANALYSEFENETRE_H
#define ANALYSEFENETRE_H

#include"analysebougie.h"
#include"trading.h"

class analyseFenetre : public QWidget
{
    QGridLayout *layoutFenetre;  //We organise the data in this layout

    QLabel* typeBougieL;
    QLabel* tmechHL;
    QLabel* tmechBL;
    QLabel* tcorpsL;

    donneB* analyseB;

    QString typeBougie;
    QString tendencia;
    QString tmechH;
    QString tmechB;
    QString tcorps;

    QLabel* text1;
    QLabel* text2;
    QLabel* text3;
    QLabel* text4;
public:
    analyseFenetre(Bougie* b);
    ~analyseFenetre();
private slots:
};

#endif // ANALYSEFENETRE_H
