#include"analysebougie.h"

donneB* analyseBougie(Bougie* b){

    if(!b) return nullptr;
    donneB* analyseBogie = new donneB;
    double bopen = b->open();
    double bclose = b->close();
    double bhigh = b->high();
    double blow = b->low();
    if(bopen<=bclose)
    {  //La bougie est située dans une tendance haussiere
        double tmechH = bhigh-bclose;
        double tmechB = bopen-blow;
        double tcorps = bclose-bopen;
        analyseBogie->tendencia="arriba";
        analyseBogie->tmechH = tmechH;
        analyseBogie->tmechB = tmechB;
        analyseBogie->tcorps = tcorps;
        if(tcorps<tmechH || tcorps<tmechB) //La bougie est une << Toupie >>
        {
            if(bopen == bclose)
            {  //La bougie est << Doji >>
                analyseBogie->typeBougie="doji.";
                return analyseBogie;
            }
            else if (bhigh>bclose && bopen == blow)
            {  //La bougie est << etoile filante >>
                analyseBogie->typeBougie = "etoile filante.";
                return analyseBogie;
            }
            else if (blow < bopen && bclose == bhigh)
            {
                analyseBogie->typeBougie = "pendu.";
                return analyseBogie;
            }
            else
            {
                analyseBogie->typeBougie = "toupie.";
                return analyseBogie;
            }
        }
        analyseBogie->typeBougie="inconnue.";
        return analyseBogie;
    }
    else
    {  //La bougie est située dans une tendance baissiere
        double tmechH = bhigh-bopen;
        double tmechB = bclose-blow;
        double tcorps = bopen-bclose;
        analyseBogie->tendencia = "abajo";
        analyseBogie->tmechH = tmechH;
        analyseBogie->tmechB = tmechB;
        analyseBogie->tcorps = tcorps;
        if(tcorps<tmechH || tcorps<tmechB)
        { //La bougie est une << Toupie >>
            if(bopen==bclose)
            {  //La bougie est << Marteau >>
                analyseBogie->typeBougie="marteau.";
                return analyseBogie;
            }else
            {
                analyseBogie->typeBougie="toupie.";
                return analyseBogie;
            }
        }
        analyseBogie->typeBougie = "inconnue.";
        return analyseBogie;
    }

}
