#include"evolutionviewer.h"
#include"trading.h"


EvolutionViewer::EvolutionViewer(EvolutionCours & e, QWidget *parent)
    : QWidget(parent), evolution(e)
{
    lastBougieClicked = nullptr;
    series = new QCandlestickSeries(this);
    series->setName(evolution.getPaireDevises().toString());
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));

    QStringList categories;
    for (EvolutionCours::iterator it = evolution.begin(); it != evolution.end(); it++)
    {
        CoursOHLC & cours = *it;
        Bougie * bougie = new Bougie(cours.getOpen(), cours.getHigh(),
                                                       cours.getLow(), cours.getClose(), cours);
        categories << cours.getDate().toString("MM.dd");
        series->append(bougie);
        connect(bougie, SIGNAL(clickBougie(Bougie*)), this, SLOT(viewCoursOHLC(Bougie*)));
    }

    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Cours OHLC");

    chart->createDefaultAxes();
    QBarCategoryAxis * axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->setAxisX(axisX, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView = new QChartView(chart, this);

    open = new QLineEdit(this);
    high = new QLineEdit(this);
    low = new QLineEdit(this);
    close = new QLineEdit(this);

    openl = new QLabel("Open", this);
    highl = new QLabel("High", this);
    lowl = new QLabel("low", this);
    closel = new QLabel("close", this);

    save = new QPushButton("Save", this);
    getforme = new QPushButton("Analyser", this);
    save->setFixedWidth(200);
    getforme->setFixedWidth(200);
    connect(save, SIGNAL(clicked()), this, SLOT(saveCoursOHLC()));
    connect(getforme, SIGNAL(clicked()), this, SLOT(analyse()));
    open->setFixedWidth(100);
    high->setFixedWidth(100);
    low->setFixedWidth(100);
    close->setFixedWidth(100);
    save->setFixedWidth(140);

    copen = new QHBoxLayout;
    copen->addWidget(openl);
    copen->addWidget(open);

    chigh = new QHBoxLayout;
    chigh->addWidget(highl);
    chigh->addWidget(high);

    clow = new QHBoxLayout;
    clow->addWidget(lowl);
    clow->addWidget(low);

    cclose = new QHBoxLayout;
    cclose->addWidget(closel);
    cclose->addWidget(close);

    coucheCours = new QVBoxLayout;
    coucheCours->addLayout(copen);
    coucheCours->addLayout(chigh);
    coucheCours->addLayout(clow);
    coucheCours->addLayout(cclose);
    coucheCours->addWidget(save, 0, Qt::AlignHCenter);
    coucheCours->addWidget(getforme, 0, Qt::AlignHCenter);

    fenetre = new QHBoxLayout;
    fenetre->addWidget(chartView);
    fenetre->addLayout(coucheCours);
    fenetre->setStretchFactor(chartView, 8);
    fenetre->setStretchFactor(coucheCours, 1);  

    this->setLayout(fenetre);
}

void EvolutionViewer::viewCoursOHLC(Bougie* c)
{
    QString str;
    str.setNum(c->getCoursOHLC().getOpen());
    open->setText(str);
    str.setNum(c->getCoursOHLC().getHigh());
    high->setText(str);
    str.setNum(c->getCoursOHLC().getLow());
    low->setText(str);
    str.setNum(c->getCoursOHLC().getClose());
    close->setText(str);
    lastBougieClicked = c;
}

void EvolutionViewer::saveCoursOHLC()
{
    double o = open->text().toDouble();
    double h = high->text().toDouble();
    double l = low->text().toDouble();
    double c = close->text().toDouble();
    lastBougieClicked->getCoursOHLC().setCours(o, h, l, c);
}

void EvolutionViewer::analyse()
{
    if (lastBougieClicked == nullptr)
    {
        QErrorMessage *error = new QErrorMessage;
        error->setWindowTitle("Aucune bougie selectionnee");
        error->setFixedSize(400, 150);
        error->showMessage("Veuillez selectionner une bougie a analyser.");
    }
    else
        analyseFenetre* af =new analyseFenetre(lastBougieClicked);
}

