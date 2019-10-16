#include"strategie.h"

QString Strategie::getstrat()
{
    QString *str = new QString;
    str->append(debut.toString("yyyy-MM-dd"));
    str->append(" -- ");
    str->append(fin.toString("yyyy-MM-dd"));

    return *str;
}
