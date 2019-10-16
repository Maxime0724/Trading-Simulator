#ifndef ANALYSEBOUGIE_H
#define ANALYSEBOUGIE_H

#include <QWidget>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>
#include "trading.h"  //This include allow us to use the Bougie class

typedef struct _donneB
{
    QString typeBougie;
    QString tendencia;
    double tmechH;
    double tmechB;
    double tcorps;
}donneB;

donneB* analyseBougie(Bougie *b);

#endif // ANALYSEBOUGIE_H
