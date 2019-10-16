#ifndef MODE_PAS_H
#define MODE_PAS_H
#include<QDate>

class Mode_pas
{
private:
    QDate aujour;
    int montant;
public:
    void setaujour(QDate d) { aujour = d; }
    void setmontant(int m) { montant = m; }
    QDate getaujour() { return aujour; }
    int getmont() { return montant; }


};

#endif // MODE_PAS_H
