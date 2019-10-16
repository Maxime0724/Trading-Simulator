#ifndef STRATEGIE_H
#define STRATEGIE_H
#include<QDate>

class Strategie
{
private:
    QDate debut;
    QDate fin;

public:
    void setdebut(QDate d)  { debut = d; }
    void setfin(QDate d)  { fin = d; }
    QDate getdebut() { return debut; }
    QDate getfin()  { return fin; }
    long long int getinter() { return debut.daysTo(fin); }

    QString getstrat();
};


#endif // STRATEGIE_H
