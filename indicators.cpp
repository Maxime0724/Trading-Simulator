#include "indicators.h"

MM::MM(EvolutionCours& e, int nb):evolutionCours(e),nbJours(nb){

    close_prices=new unsigned float[evolution_cours->nbCours];
    MMTab=new unsigned float[evolution_cours->nbCours];

    unsigned int cpt=0,sum=0;
    for (EvolutionCours::iterator it = evolution.begin(); it != evolution.end(); it++){
        //On stocke les prix de clôtures de la paire dans un tableau
        CoursOHLC & cours = *it;
        close_prices[cpt]=evolution_cours.getCoursOHLC().getClose();
        cpt+=1;
    }
    for(int i=0;i<evolution_cours->nbCours;i++){
        //On calcule la somme des nbJours derniers prix de clôture pour en déduire le MMnbJours
        sum+=close_prices[i];
        if (i<nbJours) //on ne prend pas en compte les premiers nbJours
            MMTab[i]=0;
        else{
            sum=sum-MMTab[i-nbJours]; //on retire la valeur en i-nbJours
            MMTab[i]=sum/nbJours; //car MMK=somme{i=0 à i=K}(prix_cloture{i})/K
        }
    }
}

MM::~MM()
{
    delete[] close_prices;
    delete[] MMTab;
}

MM::afficherMM(){
    QLineSeries * series = new QLineSeries();
    for(int i=0;i<evolution_cours.getNbCours();i++){
        series->append(i,MMTab[i]);
    }
    chart->addSeries(series);
}

MACD::MACD(EvolutionCours& e):evolution_cours(e){

    close_prices=new unsigned float[evolution_cours.getNbCours()];
    MACDTab=new unsigned float[evolution_cours.getNbCours()];
    MM26=new unsigned float[evolution_cours.getNbCours()];
    MM12=new unsigned float[evolution_cours.getNbCours()];
    MM9=new unsigned float[evolution_cours.getNbCours()];

    unsigned int sum12=0, sum26=0,sum9=0;
    for (EvolutionCours::iterator it = evolution.begin(); it != evolution.end(); it++){
        //On stocke les prix de clôtures de la paire de devise dans un tableau
        CoursOHLC & cours = *it;
        close_prices[cpt]=evolution_cours.getCoursOHLC().getClose();
        cpt+=1;
    }
    for(int i=0;i<evolution_cours.getNbCours();i++){
        sum12+=close_prices[i];
        sum26+=close_prices[i];
        sum9+=close_prices[i];
        //on ne prend pas en compte les premiers jours car on n'a pas accès aux valeurs des jours précédents
        if(i>=9){
            sum9=sum9-close_prices[i-9];
            MM9[i]=sum/9;
            MACDTab[i]=0;

        }
        if (i>=12){
            sum12=sum12-close_prices[i-12];
            MM12[i]=sum/12;
        }
        if (i>=26){
            sum26=sum26-close_prices[i-26];
            MM26[i]=sum/26;
            MMTab[i]=MM26[i]-MME12[i];
        }

    }
}

MACD::afficherMACD(){
    afficher(MMTab); //affichage de la différence des MM26 et MM12
    afficher(MM9); //affichage de courbe MM9 pour comparaison
}

Bollinger::Bollinger(MM & m){
    mmTab(m);
    int nbCours=mmTab->evolution_cours.nbCours();
    std_deviation = new unsigned float[nbCours];
    inf = new unsigned float[nbCours];
    sup = new unsigned float[nbCours];
    unsigned float sum=0;

    for(unsigned int i=0;i<evolution_cours->nbCours;i++){
        sum+=(mmTab->close_prices[i]-mmTab->close_prices[i])**2;
        if (i<20) //s'il n'y a pas assez de valeurs, on ne prend en compte que les i derniers prix de clôtures
            std_deviation[i]=0;
        else{
            sum=sum-std_deviation[i-20]; //on retire la valeur à i-20 de manière à ne prendre en compte que les 20 dernières valeurs
            std_deviation[i]=sum/20;
        }
        inf[i]=mmTab->MMTab[i]+2*sqrt(std_deviation[i]);
        sup[i]=mmTab->MMTab[i]+2*sqrt(std_deviation[i]);
    }
}
Bollinger::afficherBollinger(){
    QLineSeries * MoyMob20=new QLineSeries;
    QLineSeries * BollInf=new QLineSeries;
    QLineSeries * BollSup=new QLineSeries;
    for (unsigned int i=0;i<mmTab->evolution_cours.nbCours();i++){
        BollInf->append(i,inf[i]);
        BollSup->append(i,sup[i]);
        MoyMob20->append(i,mmTab[i]);
    }
    chart->addSeries(MoyMob20);
    chart->addSeries(BollInf);
    chart->addSeries(BollSup);
}

Bollinger::~Bollinger(){
    delete[] std_deviation;
    delete[] inf;
    delete[] sup;
}
