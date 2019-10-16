#include "analysefenetre.h"

analyseFenetre::analyseFenetre(Bougie* b):QWidget()
{
    setFixedSize(450, 200);
    setWindowTitle("Analyse de bougie");
    show();                            // We analyse the Bougie and return its data
    analyseB = analyseBougie(b);
    if(!analyseB)
    {
        return;
    }
    else
    {
        /* I use the instructions bellow to be claire in the manipulation
         * of the data, we could erase them from this file and from "analysefenetre.h"
         * and use directly the data from the structure analyseB
         */
        layoutFenetre = new QGridLayout;

        typeBougie = analyseB->typeBougie;
        tendencia = analyseB->tendencia;
        tmechH = QString::number(analyseB->tmechH);
        tmechB = QString::number(analyseB->tmechB);
        tcorps = QString::number(analyseB->tcorps);

        typeBougieL = new QLabel(typeBougie,this);
        tmechHL = new QLabel(tmechH,this);
        tmechBL = new QLabel(tmechB,this);
        tcorpsL = new QLabel(tcorps,this);
        text1 = new QLabel("La Bougie est d'une forme:",this);
        text2 = new QLabel("Difference (ouverture - fermeture):",this);

        if(tendencia == "arriba")
        { //If the bougie's price rises
            text3 = new QLabel("Difference (fermeture - plus haut):",this);
            text4 = new QLabel("Difference (ouverture - plus bass):",this);
        }
        else
        {//if bougie's price down
            text3 = new QLabel("Difference (ouverture - plus haut):",this);
            text4 = new QLabel("Difference (fermeture - plus bass):",this);
        }

        layoutFenetre->addWidget(text1,0,0);
        layoutFenetre->addWidget(typeBougieL,0,1);
        layoutFenetre->addWidget(text2,1,0);
        layoutFenetre->addWidget(tcorpsL,1,1);
        layoutFenetre->addWidget(text3,2,0);
        layoutFenetre->addWidget(tmechHL,2,1);
        layoutFenetre->addWidget(text4,3,0);
        layoutFenetre->addWidget(tmechBL,3,1);

        setLayout(layoutFenetre);
    }


}

analyseFenetre::~analyseFenetre(){
    delete analyseB;
    delete typeBougieL;
    delete tmechHL;
    delete tmechBL;
    delete tcorpsL;
    delete text1;
    delete text2;
    delete text3;
    delete text4;
}


