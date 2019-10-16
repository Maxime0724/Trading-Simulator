#include "trading.cpp"
#include "evolutionviewer.cpp"
#include<QtCharts/QtCharts>
#include<QLineSeries>
#include<QString>
using namespace std;

class MM{
    EvolutionCours & evolution_cours;
    float & close_prices; //tableau des prix de clôture de chaque jour
    float & MMTab;
    int nbJours;
public:
    MM(EvolutionCours& e, int nb);
    ~MM(); //doit être redéfini car allocation dynamique de mémoire lors de la construction
    void afficherMM(); //méthodes définie dans classes filles
    float getMM(unsigned int i) const {if (i<0 || i>nbJours) thow MMException("indice non valide"); return MMTab[i];}
    void afficherMM(); //passer le graphique en argument !
};

class MMException{
public:
    MMEException(const QString& message) :info(message) {}
    QString getInfo() const { return info; }
private:
    QString info;
};


class MACD{
    EvolutionCours & evolution_cours;
    unsigned float * close_prices;
    unsigned float * MM26;
    unsigned float * MM12;
    unsigned float * MM9;
    unsigned float * MACDTab;
public:
    MACD(EvolutionCours& e);
    ~MMCD(){delete [] close_prices; delete [] MACDTab;}
    void afficherMACD();
    unsigned float getMM(i) const {return MACDTab[i];}
};


class Bollinger(){
    MM & mmTab;
    unsigned float & std_deviation;//tableau des écarts types (standard deviation)
    unsigned float & inf;
    unsigned float & sup;
public :
    Bollinger();
    ~Bollinger();
    unsigned float getStdDev(){return std_deviation;}
    void afficherBollinger();
}


class RSI{
private:
    EvolutionCours * evolution_cours;
    MM & mm20;
    MM & mm50;
    unsigned float valeur;
public:
    RSI(){};
    ~RSI();
    unsigned float getValue() const {return value;}

};
