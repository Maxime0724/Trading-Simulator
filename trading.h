#ifndef TRADING_H
#define TRADING_H
#include<QString>
#include<QDate>
#include<cmath>
#include<QCandlestickSet>
#include<QtCore/QArgument>
#include<QtCharts/QtCharts>


using namespace std;

class TradingException
{
public:
    TradingException(const QString& message) :info(message) {}
    QString getInfo() const { return info; }
private:
    QString info;
};

class Devise
{
    QString code;
    QString monnaie, zone;
    Devise(const QString& c, const QString& m, const QString& z = "");
    ~Devise() = default;
    friend class DevisesManager;
public:
    QString getCode() const { return code; }
    const QString& getMonnaie() const { return monnaie; }
    const QString& getZoneGeographique() const { return zone; }
};

class PaireDevises
{
    const Devise* base;
    const Devise* contrepartie;
    QString surnom;
    PaireDevises(const Devise& b, const Devise& c, const QString& s = "");
    ~PaireDevises() = default;
    friend class DevisesManager;
public:
    const Devise& getBase() const { return *base; }
    const Devise& getContrepartie() const { return *contrepartie; }
    const QString& getSurnom() const { return surnom; }
    QString toString() const;
};

class CoursOHLC
{
    double open = 0, high = 0, low = 0, close = 0;
    QDate date;
public:
    CoursOHLC() {}
    CoursOHLC(double o, double h, double l, double c, const QDate& d);
    double getOpen() const { return open; }
    double getHigh() const { return high; }
    double getLow() const { return low; }
    double getClose() const { return close; }
    void setCours(double o, double h, double l, double c);
    QDate getDate() const { return date; }
    void setDate(const QDate& d) { date = d; }
};

class EvolutionCours
{
    const PaireDevises* paire;
    CoursOHLC* cours = nullptr;
    unsigned int nbCours = 0;
    unsigned int nbMaxCours = 0;
public:
    EvolutionCours(const PaireDevises& p) :paire(&p) {}
    void addCours(double o, double h, double l, double c, const QDate& d);
    ~EvolutionCours();
    EvolutionCours(const EvolutionCours& e);
    EvolutionCours& operator=(const EvolutionCours& e);
    unsigned int getNbCours() const { return nbCours; }
    const CoursOHLC & getCoursOHLC() { return *cours; }
    const PaireDevises& getPaireDevises() const { return *paire; }
    using iterator = CoursOHLC*;
    iterator begin() { return iterator(cours); }
    iterator end() { return iterator(cours + nbCours); }
    using const_iterator = const CoursOHLC*;
    const_iterator begin() const { return cours; }
    const_iterator end() const { return cours + nbCours; }
    const_iterator cbegin() const { return cours; }
    const_iterator cend() const { return cours + nbCours; }
};

class DevisesManager
{
    Devise** devises = nullptr; // tableau de pointeurs sur objets Devise
    unsigned int nbDevises = 0;
    unsigned int nbMaxDevises = 0;
    mutable PaireDevises** paires = nullptr; // tableau de pointeurs sur objets PaireDevises
    mutable unsigned int nbPaires = 0;
    mutable unsigned int nbMaxPaires = 0;
    DevisesManager(const DevisesManager& m) = delete;
    DevisesManager& operator=(const DevisesManager& m) = delete;
    // pour le singleton
    DevisesManager() {}
    ~DevisesManager();
    struct Handler {
        DevisesManager* instance = nullptr;
        ~Handler() { delete instance; }
    };
    static Handler handler;
public:
    static DevisesManager& getManager()
    {
        if (handler.instance == nullptr)
            handler.instance = new DevisesManager;
        return *handler.instance;
    }
    static void libererManager()
    {
        delete handler.instance;
        handler.instance = nullptr;
    }
    const Devise& creationDevise(const QString& c, const QString& m,
                                 const QString& z);
    const Devise& getDevise(const QString& c) const;
    const PaireDevises& getPaireDevises(const QString & c1,
                                        const QString & c2) const;
};

enum Couleur {rouge, vert};

class Bougie : public QCandlestickSet
{
    Q_OBJECT
    CoursOHLC *cours;
    static double seuilToupie;
    Couleur couleur;
public:
    Bougie(CoursOHLC &cours) : cours(&cours) {}
    Bougie(qreal open, qreal high, qreal low, qreal close, CoursOHLC &c,
           qreal timestamp = 0.0, QObject *parent = nullptr) :
        QCandlestickSet(open, high, low, close, timestamp, parent), cours(&c)
    {
        connect(this, SIGNAL(clicked()), this, SLOT(viewCoursOHLC()));
    }
    CoursOHLC & getCoursOHLC()  { return *cours; }
    const CoursOHLC & getCoursOHLC() const { return *cours;}
    Couleur getCouleur() const { return couleur; }
    void setSeuilToupie(double seuilToupieNew);
    string getForme() const;
    friend ostream & operator<<(ostream &os, const Bougie &bougie);

signals:
    void clickBougie(Bougie *c);

private slots:
    void viewCoursOHLC();
};

class Volume
{
private:
    QDate date;
    double vol;
public:
    Volume() {}
    Volume(double d, QDate dat) : date(dat), vol(d) { }
    double & getVolume() { return vol; }
    QDate & getDate() { return date; }
    void setVolume(double d) { vol = d; }
    void setDate(const QDate& d) { date=d; }

};

class EvolutionVolume
{
private:
    const PaireDevises * paire;
    Volume * vol = nullptr;
    int nbVolume = 0;
    int nbMaxVolume = 0;

public:
    EvolutionVolume(const PaireDevises& p) : paire(&p) {}
    void addVolume(double v, const QDate & d);
    ~EvolutionVolume();
    EvolutionVolume(const EvolutionVolume& e);
    EvolutionVolume& operator=(const EvolutionVolume& e);
    int getNbVolume() const { return nbVolume; }
    Volume* getVolume() { return vol; }
    const PaireDevises& getPaireDevises() const { return *paire; }
    using iterator = Volume*;
    iterator begin() { return iterator(vol); }
    iterator end() { return iterator(vol+ nbVolume); }
    using const_iterator = const Volume*;
    const_iterator begin() const { return vol; }
    const_iterator end() const { return vol + nbVolume; }
    const_iterator cbegin() const { return vol; }
    const_iterator cend() const { return vol + nbVolume; }
};

#endif
