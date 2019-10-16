#include"trading.h"

Devise::Devise(const QString& c, const QString& m, const QString& z) :
    monnaie(m), zone(z)
{
    if (c.size() != 3) throw TradingException("code devise incorrect");
    for (unsigned int i = 0; i < 3; i++)
    {
        if (c[i] <= 'A' || c[i] >= 'Z') throw TradingException("code devise incorrect");
    }
    code = c;
}

PaireDevises::PaireDevises(const Devise& b, const Devise& c, const QString& s) :
    base(&b), contrepartie(&c), surnom(s) {}

QString PaireDevises::toString() const
{
    QString str;
    str=getBase().getCode()+"/"+getContrepartie().getCode();
    if (getSurnom()!="") str+="("+getSurnom()+")";
    return str;
}

void CoursOHLC::setCours(double o, double h, double l, double c)
{
    if (o < 0 || h < 0 || l < 0 || c<0 || l>h)
        throw TradingException("cours OHLC incorrect");
    open = o;
    high = h;
    low = l;
    close = c;
}

CoursOHLC::CoursOHLC(double o, double h, double l, double c, const QDate& d):date(d)
{
    if (o < 0 || h < 0 || l < 0 || c<0 || l>h) throw TradingException("cours OHLC incorrect");
        open = o; high = h; low = l; close = c;
}

void EvolutionCours::addCours(double o, double h, double l, double c, const QDate& d)
{
    if (nbMaxCours == nbCours)
    {
        CoursOHLC* newtab = new CoursOHLC[nbMaxCours + 100];
        for (unsigned int i = 0; i < nbCours; i++) newtab[i] = cours[i];
        CoursOHLC* old = cours;
        cours = newtab;
        nbMaxCours += 100;
        delete[] old;
    }

    cours[nbCours].setCours(o, h, l, c);
    cours[nbCours].setDate(d);
    nbCours++;
}

EvolutionCours::~EvolutionCours()
{
    delete [] cours;
}

EvolutionCours::EvolutionCours(const EvolutionCours& e) :
    paire(e.paire),
    cours(new CoursOHLC[e.nbCours]),
    nbCours(e.nbCours),
    nbMaxCours(e.nbCours)
{
    for (unsigned int i = 0; i < nbCours; i++) cours[i] = e.cours[i];
}

EvolutionCours& EvolutionCours::operator=(const EvolutionCours& e)
{
    if (this != &e)
    {
        paire = e.paire;
        nbCours = 0;
        for (unsigned int i = 0; i < e.nbCours; i++)
            addCours(e.cours[i].getOpen(), e.cours[i].getHigh(),e.cours[i].getLow(),
                     e.cours[i].getClose(),e.cours[i].getDate());
    }
    return *this;
}

const Devise& DevisesManager::creationDevise(const QString& c, const QString& m, const QString& z)
{
    if (nbDevises == nbMaxDevises)
    {
        Devise** newtab = new Devise*[nbMaxDevises + 10];
        for (unsigned int i = 0; i < nbDevises; i++) newtab[i] = devises[i];

        Devise** old = devises;
        nbMaxDevises += 10;
        devises = newtab;
        delete[] old;
    }

    devises[nbDevises] = new Devise(c, m, z);
    return *devises[nbDevises++];
}

const Devise& DevisesManager::getDevise(const QString& c) const
{
    for (unsigned int i = 0; i < nbDevises; i++)
        if (devises[i]->getCode() == c) return *devises[i];
    throw TradingException("devise inexistante");
}

const PaireDevises& DevisesManager::getPaireDevises(const QString & c1, const QString & c2) const {
    const Devise& d1 = getDevise(c1);
    const Devise& d2 = getDevise(c2);
    for (unsigned int i = 0; i < nbPaires; i++)
        if (paires[i]->getBase().getCode() == c1 &&
            paires[i]->getContrepartie().getCode() == c2)
            return *paires[i];

    if (nbPaires == nbMaxPaires)
    {
        PaireDevises** newtab = new PaireDevises*[nbMaxPaires + 10];
        for (unsigned int i = 0; i < nbPaires; i++) newtab[i] = paires[i];
        PaireDevises** old = paires;
        nbMaxPaires += 10;
        paires = newtab;
        delete[] old;
    }
    paires[nbPaires] = new PaireDevises(d1, d2);
    return *paires[nbPaires++];
}

DevisesManager::~DevisesManager()
{
    for (unsigned int i = 0; i < nbPaires; i++)
        delete paires[i];
    for (unsigned int i = 0; i < nbDevises; i++)
        delete devises[i];
    delete[] paires;
    delete[] devises;
}

DevisesManager::Handler DevisesManager::handler=
                                DevisesManager::Handler();


double Bougie::seuilToupie=1;

void Bougie::setSeuilToupie(double seuilToupieNew)
{
    seuilToupie = seuilToupieNew;
}

std::string Bougie::getForme() const
{
    if (abs(static_cast<int>(cours->getClose() - cours->getOpen())) < seuilToupie)
        return "Toupie";
    return "";
}

void Bougie::viewCoursOHLC()
{
    emit clickBougie(this);
}

std::ostream &operator<<(std::ostream &os, const Bougie &bougie) {
    if (bougie.couleur == Couleur::rouge)
        os << "rouge";
    else os << "vert";
    return os;
}


void EvolutionVolume::addVolume(double v, const QDate& dat)
{
    if (nbMaxVolume == nbVolume)
    {
        Volume * newtab = new Volume[nbMaxVolume + 100];
        for (int i = 0; i < nbVolume; i++)
            newtab[i] = vol[i];
        Volume* old = vol;
        vol = newtab;
        nbMaxVolume += 100;
        delete [] old;
    }

    vol[nbVolume].setVolume(v);
    vol[nbVolume].setDate(dat);
    nbVolume++;
}

EvolutionVolume::~EvolutionVolume()
{
    delete [] vol;
}

EvolutionVolume::EvolutionVolume(const EvolutionVolume& e) :
    paire(e.paire),
    vol(new Volume[e.nbVolume]),
    nbVolume(e.nbVolume),
    nbMaxVolume(e.nbVolume)
{
    for ( int i = 0; i < nbVolume; i++) vol[i] = e.vol[i];
}

EvolutionVolume& EvolutionVolume::operator=(const EvolutionVolume& e)
{
    if (this != &e)
    {
        paire = e.paire;
        nbVolume = 0;
        for ( int i = 0; i < e.nbVolume; i++)
            addVolume(e.vol[i].getVolume(), e.vol[i].getDate());
    }
    return *this;
}
