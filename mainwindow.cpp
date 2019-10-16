#include"mainwindow.h"
#include"ui_mainwindow.h"
#include"trading.h"
#include"evolutionviewer.h"
#include"volumeviewer.h"
#include<QFileDialog>
#include<QLabel>
#include<QDebug>
#include<QChartView>
#include<QWidget>
#include<QtWidgets/QGridLayout>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QGroupBox>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Trading Simulator");

    setFixedSize(1600, 800);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);

    QIcon icon(":/src/icon.png");
    setWindowIcon(icon);

    setFixedSize(this->width(),this->height());

    label = new QLabel(this);
    label->setText("Ouvrir un document pour commencer.");
    label->setAlignment(Qt::AlignCenter);
    label->setGeometry(540, 360, 500, 50);

    QFont *font = new QFont("Courier New", 14);
    label->setFont(*font);

    date = new QDate[5000];
    date[0] = QDate(0, 0, 0);

    note = new QString("Ecrivez votre commentaire...");

    ui->actionMode_Manuel->setEnabled(false);
    ui->actionMode_automatique->setEnabled(false);
    ui->actionTransaction->setEnabled(false);
    ui->actionQuitter_le_mode_actuel->setEnabled(false);
    ui->actionAnnuler_la_derniere_transaction->setEnabled(false);
    ui->actionAjouter_une_strategie->setEnabled(false);
    ui->actionMode_pas_a_pas->setEnabled(false);
    ui->actionNote->setEnabled(false);
    ui->actionTransaction_Mode_pas_a_pas->setEnabled(false);
    ui->actionPasser_au_demain->setEnabled(false);

    num_fin = nb_donnee - 1;

}

MainWindow::~MainWindow()
{
    delete ui;

    delete [] date;
}

void MainWindow::on_actionOuvrier_un_file_csv_triggered()
{
    filename_csv = QFileDialog::getOpenFileName(this, tr("Select fichier"),
                       "C:/Users/guanj/OneDrive/UV/GI01/LO21/TradingSimulator/src",
                       tr("fichier csv(*.csv)"));
    qDebug() << filename_csv;
    if (filename_csv != "")
        importMsg(filename_csv);
}

void MainWindow::importMsg(QString filename)
{
    label->hide();
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Cannot open file for reading: "
                  << qPrintable(file.errorString()) << endl;
        return;
    }

    QStringList list;
    list.clear();

    QTextStream in(&file);
    nb_donnee = 0;
    QString fileLine = in.readLine();


    while(!in.atEnd())
    {
        fileLine = in.readLine();
        list = fileLine.split(",", QString::SkipEmptyParts);
        date[nb_donnee] = QDate::fromString(list.at(0), "yyyy-MM-dd");
        open[nb_donnee] = list.at(1).toDouble();
        high[nb_donnee] = list.at(2).toDouble();
        low[nb_donnee] = list.at(3).toDouble();
        close[nb_donnee] = list.at(4).toDouble();
        adj_close[nb_donnee] = list.at(5).toDouble();
        volume[nb_donnee] = list.at(6).toInt();

        nb_donnee++;
    }
    file.close();

    DevisesManager& m = DevisesManager::getManager();
    m.creationDevise("USD", "Dollar Américain", "USA");
    m.creationDevise("BTC", "BitCoin", "monnaie virtuelle");
    const PaireDevises & BTC_USD = m.getPaireDevises("BTC", "USD");
    e = new EvolutionCours(BTC_USD);

    for (int i = 0; i < 15 ; i++)
        e->addCours(open[i], high[i], low[i], close[i], date[i]);

    EvolutionViewer *v = new EvolutionViewer(*e, this);
    v->show();

    EvolutionVolume ev(BTC_USD);
    for (int i = 0; i < 15 ; i++)
    {
        ev.addVolume(volume[i], date[i]);
    }

    VolumeViewer *eview = new VolumeViewer(&ev, this);
    eview->show();

    periode = new QPushButton(this);
    periode->setText("Selectionner une date");
    periode->setFixedWidth(300);
    connect(periode, SIGNAL(clicked()), this, SLOT(createdatedialog()));

    create_table();
    nb_trans = 0;

    QVBoxLayout *layout1 = new QVBoxLayout;
    layout1->addWidget(v);
    layout1->addWidget(eview->getChartview());

    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(table);
    layout2->addLayout(layout1);

    precedent = new QPushButton;
    suivant = new QPushButton;
    precedent->setText("Precedent");
    precedent->setFixedWidth(120);
    suivant->setFixedWidth(120);
    suivant->setText("Suivant");
    if (num_debut == 0)
        precedent->setEnabled(false);
    if (num_debut == nb_donnee - 1)
        suivant->setEnabled(false);
    precedent->show();
    suivant->show();

    connect(precedent, SIGNAL(clicked()), this, SLOT(datePrecedent()));
    connect(suivant, SIGNAL(clicked()), this, SLOT(dateSuivant()));

    QHBoxLayout *layout3 = new QHBoxLayout;
    layout3->addWidget(precedent, Qt::AlignLeft);
    layout3->addWidget(periode, Qt::AlignHCenter);
    layout3->addWidget(suivant, Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    create_strat();

    ui->actionMode_Manuel->setEnabled(true);
    ui->actionMode_automatique->setEnabled(true);
    ui->actionParametres->setEnabled(true);
    ui->actionAjouter_une_strategie->setEnabled(true);
    ui->actionMode_pas_a_pas->setEnabled(true);
}

void MainWindow::dateException()
{
    dateError = new QErrorMessage(this);
    dateError->setWindowTitle("Date Invalide");
    dateError->setFixedSize(400, 150);
    QString *error = new QString("La date est invalide!\n");
    error->append("Veuillez entrer une bonne date dans l'intervalle.");
    dateError->showMessage(*error);
}

void MainWindow::montantException()
{
    montantError = new QErrorMessage(this);
    montantError->setWindowTitle("Montant Invalide");
    montantError->setFixedSize(400, 150);
    QString *error = new QString("Le montant est invalide.\n");
    error->append("Veuillez entrer un bon montant.");
    montantError->showMessage(*error);
    montantError->show();
}


void MainWindow::createdatedialog()
{
    datedialog = new QDialog(this);
    datedialog->setWindowTitle("Selectionner une date");
    datedialog->setGeometry(750, 450, 350, 150);

    QLabel *date1 = new QLabel(datedialog);
    date1->setText("Date de debut: ");
    date1->setFixedSize(150, 20);
    date1->show();

    editdate1 = new QLineEdit(datedialog);
    editdate1->setInputMask("D999-99-99");
    editdate1->setFixedWidth(100);

    QLabel *label3 = new QLabel(this);
    limit = new QString("");
    limit->append("La date entree doit etre entre: \n");
    limit->append(date[0].toString("yyyy-MM-dd"));
    limit->append(" -- ");
    limit->append(date[nb_donnee-21].toString("yyyy-MM-dd"));
    limit->append("\n");
    label3->setText(*limit);
    QHBoxLayout *layout1 = new QHBoxLayout;
    QHBoxLayout *layout2 = new QHBoxLayout;

    layout1->addWidget(date1);
    layout1->addWidget(editdate1);

    QPushButton *valider = new QPushButton(datedialog);
    valider->setText("Valider");
    valider->setFixedSize(150, 30);
    valider->show();


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(layout1);
    layout->addLayout(layout2);
    layout->addWidget(label3, 0, Qt::AlignCenter);
    layout->addWidget(valider, 0, Qt::AlignCenter);

    datedialog->setLayout(layout);

    datedialog->show();

    connect(valider, SIGNAL(clicked()), this, SLOT(dateSaisie()));

}

void MainWindow::dateSaisie()
{

    QDate *debut = new QDate(QDate::fromString(editdate1->text(), "yyyy-MM-dd"));
    datedialog->close();

    if (*debut < date[0] || *debut > date[nb_donnee-1]
            || debut->daysTo(date[nb_donnee-1]) < 20)
    {
        dateException();
        return;
    }
    num_debut = date[0].daysTo(*debut);
    regrapher(num_debut);
}


void MainWindow::regrapher(long long int num_debut)
{
    widget->hide();
    //delete widget;

    DevisesManager& m = DevisesManager::getManager();
    m.creationDevise("USD", "Dollar Américain", "USA");
    m.creationDevise("BTC", "BitCoin", "monnaie virtuelle");
    const PaireDevises & BTC_USD = m.getPaireDevises("BTC", "USD");
    e = new EvolutionCours(BTC_USD);

    for (long long int i = num_debut; i < num_debut + 15; i++)
        e->addCours(open[i], high[i], low[i], close[i], date[i]);

    v = new EvolutionViewer(*e, this);
    v->show();

    EvolutionVolume ev(BTC_USD);
    for (long long int i = num_debut; i < num_debut + 15; i++)
    {
        ev.addVolume(volume[i], date[i]);
    }

    eview = new VolumeViewer(&ev, this);
    eview->show();

    periode = new QPushButton(this);
    periode->setText("Selectionner une date");
    periode->setFixedWidth(300);
    connect(periode, SIGNAL(clicked()), this, SLOT(createdatedialog()));

    QVBoxLayout *layout1 = new QVBoxLayout;
    layout1->addWidget(v);
    layout1->addWidget(eview->getChartview());

    QHBoxLayout *layout2 = new QHBoxLayout;


    layout2->addWidget(table);
    layout2->addLayout(layout1);

    precedent = new QPushButton;
    suivant = new QPushButton;
    precedent->setText("Precedent");
    precedent->setFixedWidth(120);
    suivant->setFixedWidth(120);
    suivant->setText("Suivant");
    if (num_debut == 0)
        precedent->setEnabled(false);

    if (num_debut == num_fin - 15)
        suivant->setEnabled(false);
    precedent->show();
    suivant->show();

    connect(precedent, SIGNAL(clicked()), this, SLOT(datePrecedent()));
    connect(suivant, SIGNAL(clicked()), this, SLOT(dateSuivant()));

    QHBoxLayout *layout3 = new QHBoxLayout;
    layout3->addWidget(precedent, Qt::AlignLeft);
    layout3->addWidget(periode, Qt::AlignHCenter);
    layout3->addWidget(suivant, Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    if (num_debut + 15 > num_fin)
        ui->actionPasser_au_demain->setEnabled(false);
}

void MainWindow::create_table()
{
    header << "Date" << "A/V" << "montant cotation" << "montant base" << "ROI";
    table = new QTableWidget(40, 5, this);
    table->setHorizontalHeaderLabels(header);
    table->horizontalHeader()->setStretchLastSection(true);

    table->setFixedWidth(360);
    table->setColumnWidth(0, 100);
    table->setColumnWidth(1, 50);
    table->setColumnWidth(2, 140);
    table->setColumnWidth(3, 120);
    table->setColumnWidth(4, 80);

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->show();
}

void MainWindow::ajout_table()
{
    table->setItem(nb_trans-1, 0, new QTableWidgetItem(date_trans->toString("yyyy-MM-dd")));
    table->setItem(nb_trans-1, 1, new QTableWidgetItem("A"));

    QString *cotation = new QString(QString::number(mont_contre));
    cotation->append("(+");
    cotation->append(QString::number(mont));
    cotation->append(")");
    table->setItem(nb_trans-1, 2, new QTableWidgetItem(*cotation));

    QString *base = new QString(QString::number(mont_base));
    base->append("(-");
    base->append(QString::number(abs(mont_base - old_mont_base)));
    base->append(")");
    table->setItem(nb_trans-1, 3, new QTableWidgetItem(*base));

    QString *roi_str = new QString(QString::number(roi*100));
    roi_str->append("%");
    table->setItem(nb_trans-1, 4, new QTableWidgetItem(*roi_str));
}

void MainWindow::ajout_tablev()
{
    table->setItem(nb_trans-1, 0, new QTableWidgetItem(date_trans->toString("yyyy-MM-dd")));
    table->setItem(nb_trans-1, 1, new QTableWidgetItem("V"));

    QString *cotation = new QString(QString::number(mont_contre));
    cotation->append("(-");
    cotation->append(QString::number(abs(old_mont_contre - mont_contre)));
    cotation->append(")");
    table->setItem(nb_trans-1, 2, new QTableWidgetItem(*cotation));

    QString *base = new QString(QString::number(mont_base));
    base->append("(+");
    base->append(QString::number(mont));
    base->append(")");
    table->setItem(nb_trans-1, 3, new QTableWidgetItem(*base));

    QString *roi_str = new QString(QString::number(roi*100));
    roi_str->append("%");
    table->setItem(nb_trans-1, 4, new QTableWidgetItem(*roi_str));
}

void MainWindow::calcul_roi()
{
    roi = (mont_base * close[num_trans] + mont_contre) / contre_init;
}

void MainWindow::create_strat()
{
    strat = new Strategie[7];

    strat[0].setdebut(date[27]);
    strat[0].setfin(date[57]);

    strat[1].setdebut(date[31]);
    strat[1].setfin(date[50]);

    strat[2].setdebut(date[51]);
    strat[2].setfin(date[63]);

    strat[3].setdebut(date[64]);
    strat[3].setfin(date[80]);

    strat[4].setdebut(date[81]);
    strat[4].setfin(date[95]);

    strat[5].setdebut(date[96]);
    strat[5].setfin(date[110]);
}

double MainWindow::getmm9(QDate d)
{
    num_date_auto = date[0].daysTo(d);
    double somme9 = 0;
    for (int i = 1; i < 10; i++)
        somme9 += close[num_date_auto - i];
    mm9 = somme9/9;
    return mm9;
}

double MainWindow::getmm12(QDate d)
{
    num_date_auto = date[0].daysTo(d);
    double somme12 = 0;
    for (int i = 1; i < 13; i++)
        somme12 += close[num_date_auto - i];
    mm12 = somme12/12;
    return mm12;
}

double MainWindow::getmm26(QDate d)
{
    num_date_auto = date[0].daysTo(d);
    double somme26 = 0;
    for (int i = 1; i < 27; i++)
        somme26 += close[num_date_auto - i];
    mm26 = somme26/26;
    return mm26;
}

double MainWindow::getrsi(QDate d)
{
    num_date_auto = date[0].daysTo(d);
    int b = 0;
    int h = 0;
    for (int i = 10; i > 0; i--)
    {
        if (open[num_date_auto - i] < close[num_date_auto - i])
            h++;
        else
            b++;
    }
    rsi = 100 * h/(h+b);
    return rsi;
}

double MainWindow::getmacd(QDate d)
{
    macd = getmm26(d) - getmm9(d);
    return macd;
}

int MainWindow::decision()
{
    // 1 pour acheter, -1 pour vender, 0 pour rien faire
    qDebug() << getmacd(date_auto.addDays(-1));
    qDebug() << getmm9(date_auto.addDays(-1));
    qDebug() << getmacd(date_auto);
    qDebug() << getmm9(date_auto);
    qDebug() << getrsi(date_auto);
    qDebug() << getrsi(date_auto.addDays(-1));

    if (getmacd(date_auto.addDays(-1)) > getmm9(date_auto.addDays(-1))
            && getmacd(date_auto) < getmm9(date_auto))// non acheter
    {
        if (getrsi(date_auto) >= 70 && getrsi(date_auto.addDays(-1)) < 70)
            return -1;
        else
            return 0;
    }
    if (getmacd(date_auto.addDays(-1)) < getmm9(date_auto.addDays(-1))
        && getmacd(date_auto) > getmm9(date_auto)) // non vendre
    {
        if (getrsi(date_auto) <= 30 && getrsi(date_auto.addDays(-1)) > 30)
            return 1;
        else
            return 0;
    }
    if (getrsi(date_auto) <= 30 && getrsi(date_auto.addDays(-1)) > 30)
        return 1;

    if (getrsi(date_auto) >= 70 && getrsi(date_auto.addDays(-1)) < 70)
        return -1;
    return 0;
}

void MainWindow::acheter_auto()
{
    date_trans = &date_auto;
    mont = mont_contre;

    num_trans = date[0].daysTo(*date_trans);

    old_mont_base = mont_base;
    old_mont_contre = mont_contre;

    nb_trans++;
    mont_base += mont / open[num_trans] * (1-pourcentage);
    mont_contre -= mont;
    calcul_roi();

    ajout_table();
}

void MainWindow::vendre_auto()
{
    date_trans = &date_auto;
    mont = mont_base;

    num_trans = date[0].daysTo(*date_trans);

    old_mont_base = mont_base;
    old_mont_contre = mont_contre;

    nb_trans++;
    mont_contre += mont * (1 - pourcentage) * open[num_trans];
    mont_base -= mont;
    calcul_roi();

    ajout_tablev();
}

void MainWindow::datePrecedent()
{
    num_debut -= 1;
    regrapher(num_debut);
}

void MainWindow::dateSuivant()
{
    num_debut += 1;
    regrapher(num_debut);
}

void MainWindow::on_actionMode_Manuel_triggered()
{
    modemanuel = new QDialog(this);
    modemanuel->setWindowTitle("Bienvenu dans Mode Manuel");
    modemanuel->setFixedSize(400, 200);
    modemanuel->show();

    QLabel *label1 = new QLabel(modemanuel);
    limit = new QString("Bienvenu dans le mode manuel.\n");
    limit->append("Vous pouvez faire des transactions.\n\n");
    limit->append("Les dates des donnees sont: \n");
    limit->append(date[0].toString("yyyy-MM-dd"));
    limit->append(" -- ");
    limit->append(date[nb_donnee-2].toString("yyyy-MM-dd"));
    label1->setFont(QFont("Calibri", 12));
    label1->setText(*limit);


    QPushButton *commencer = new QPushButton(modemanuel);
    commencer->setText("C'est parti");
    commencer->setFixedWidth(150);
    connect(commencer, SIGNAL(clicked()), this, SLOT(mode_manuel()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1, Qt::AlignTop);
    layout->addWidget(commencer, 0, Qt::AlignCenter);
    modemanuel->setLayout(layout);

}

void MainWindow::mode_manuel()
{
    modemanuel->close();
    ui->actionTransaction->setEnabled(true);
    ui->actionMode_Manuel->setEnabled(false);
    ui->actionMode_automatique->setEnabled(false);
    ui->actionQuitter_le_mode_actuel->setEnabled(true);
    ui->actionParametres->setEnabled(false);
    ui->actionAjouter_une_strategie->setEnabled(false);
    ui->actionNote->setEnabled(true);
    ui->actionMode_pas_a_pas->setEnabled(false);

    ui->statusBar->showMessage("Vous etes dans le mode manuel.");
    ui->statusBar->show();

}

void MainWindow::on_actionParametres_triggered()
{
    parametre = new QDialog(this);
    parametre->setFixedSize(500, 300);
    parametre->setWindowTitle("Configuration des parametres");
    parametre->show();

    QLabel *label1 = new QLabel(parametre);
    label1->setText("Vous pouvez configurer les parametres ci-dessous.\n");

    QLabel *label2 = new QLabel(parametre);
    QLabel *label3 = new QLabel(parametre);
    QLabel *label4 = new QLabel(parametre);
    label2->setText("Montant de la devise de base: ");
    label3->setText("Montant de la devise de contrepartie: ");
    label4->setText("Pourcentage preleve: ");

    editbase = new QLineEdit(parametre);
    editcontre = new QLineEdit(parametre);
    editpourc = new QLineEdit(parametre);

    editbase->setText(QString::number(base_init));
    editbase->setFixedWidth(150);
    editcontre->setText(QString::number(contre_init));
    editcontre->setFixedWidth(150);
    editpourc->setText(QString::number(pourcentage));
    editpourc->setFixedWidth(150);

    QPushButton *valider = new QPushButton(parametre);
    valider->setFixedWidth(200);
    valider->setText("Valider");

    QHBoxLayout *layout1 = new QHBoxLayout;
    QHBoxLayout *layout2 = new QHBoxLayout;
    QHBoxLayout *layout3 = new QHBoxLayout;

    layout1->addWidget(label2);
    layout1->addWidget(editbase, Qt::AlignRight);
    layout2->addWidget(label3);
    layout2->addWidget(editcontre, Qt::AlignRight);
    layout3->addWidget(label4);
    layout3->addWidget(editpourc, Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1);
    layout->addLayout(layout1);
    layout->addLayout(layout2);
    layout->addLayout(layout3);
    layout->addWidget(valider, 0, Qt::AlignCenter);

    parametre->setLayout(layout);

    connect(valider, SIGNAL(clicked()), this, SLOT(saveparametres()));
}

void MainWindow::saveparametres()
{
    mont_base = editbase->text().toInt();
    mont_contre = editcontre->text().toInt();
    pourcentage = editpourc->text().toDouble();


    base_init = mont_base;
    contre_init = mont_contre;
    parametre->close();
}

void MainWindow::on_actionMode_automatique_triggered()
{
    modeauto = new QDialog(this);
    modeauto->setFixedSize(450, 280);
    modeauto->setWindowTitle("Selectionnez une strategie");
    QLabel *label1 = new QLabel(modeauto);
    label1->setText("Bienvenu dans le mode automatique.\n"
                    "Veuillez selectionner une strategie entre ci-dussous.\n");

    btn_grp = new QButtonGroup;

    QRadioButton *strat0 = new QRadioButton(this);
    strat0->setText(strat[0].getstrat());
    btn_grp->addButton(strat0, 0);

    QRadioButton *strat1 = new QRadioButton(this);
    strat1->setText(strat[1].getstrat());
    btn_grp->addButton(strat1, 1);

    QRadioButton *strat2 = new QRadioButton(this);
    strat2->setText(strat[2].getstrat());
    btn_grp->addButton(strat2, 2);

    QRadioButton *strat3 = new QRadioButton(this);
    strat3->setText(strat[3].getstrat());
    btn_grp->addButton(strat3, 3);

    QRadioButton *strat4 = new QRadioButton(this);
    strat4->setText(strat[4].getstrat());
    btn_grp->addButton(strat4, 4);

    QRadioButton *strat5 = new QRadioButton(this);
    strat5->setText(strat[5].getstrat());
    btn_grp->addButton(strat5, 5);


    QPushButton *valider = new QPushButton(modeauto);
    valider->setText("Valider");
    valider->setFixedWidth(200);
    connect(valider, SIGNAL(clicked()), this, SLOT(mode_auto()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1, Qt::AlignHCenter);
    layout->addWidget(strat0);
    layout->addWidget(strat1);
    layout->addWidget(strat2);
    layout->addWidget(strat3);
    layout->addWidget(strat4);
    layout->addWidget(strat5);
    layout->addWidget(valider, 0, Qt::AlignCenter);
    modeauto->setLayout(layout);
    modeauto->show();
}

void MainWindow::on_actionTransaction_triggered()
{
    datedialog2 = new QDialog(this);
    datedialog2->setWindowTitle("Mode Manuel");
    datedialog2->setFixedSize(400, 300);
    datedialog2->show();

    QLabel *label1 = new QLabel(datedialog2);
    limit = new QString("");
    limit->append("Vous pouvez faire des transactions.\n\n");
    limit->append("Les dates des donnees sont: \n");
    limit->append(date[0].toString("yyyy-MM-dd"));
    limit->append(" -- ");
    limit->append(date[nb_donnee-2].toString("yyyy-MM-dd"));

    limit->append("\n\nDevise de base: ");
    limit->append(QString::number(mont_base));
    limit->append("\nDevise de contrepartie: ");
    limit->append(QString::number(mont_contre));
    limit->append("\nPourcentage du prelevement: ");
    limit->append(QString::number(pourcentage));
    label1->setText(*limit);

    editdate1 = new QLineEdit(datedialog);
    editdate1->setInputMask("D999-99-99");
    editdate1->setFixedWidth(100);

    QLabel *date1 = new QLabel(datedialog2);
    date1->setText("Date d'achat / vente: ");

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(date1, Qt::AlignCenter);
    layout1->addWidget(editdate1, Qt::AlignCenter);

    QLabel *label_montant = new QLabel(datedialog2);
    label_montant->setText("Le montant: ");
    montant = new QLineEdit(datedialog2);
    montant->setFixedWidth(100);

    QHBoxLayout *layout3 = new QHBoxLayout;
    layout3->addWidget(label_montant);
    layout3->addWidget(montant);


    QPushButton *acheter = new QPushButton(datedialog2);
    acheter->setText("Acheter");
    acheter->setFixedWidth(150);
    connect(acheter, SIGNAL(clicked()), this, SLOT(acheter()));

    QPushButton *vendre = new QPushButton(datedialog2);
    vendre->setText("Vendre");
    vendre->setFixedWidth(150);
    connect(vendre, SIGNAL(clicked()), this, SLOT(vendre()));

    QHBoxLayout *layout4 = new QHBoxLayout;
    layout4->addWidget(acheter);
    layout4->addWidget(vendre);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1, Qt::AlignTop);
    layout->addLayout(layout1);
    layout->addLayout(layout3);
    layout->addLayout(layout4);
    datedialog2->setLayout(layout);
}

void MainWindow::acheter()
{
    date_trans = new QDate(QDate::fromString(editdate1->text(), "yyyy-MM-dd"));
    mont = montant->text().toInt();

    if (*date_trans < date[0] || *date_trans > date[nb_donnee-1])
    {
        dateException();
        return;
    }

    num_trans = date[0].daysTo(*date_trans);

    if (mont > mont_contre || mont <= 0)
    {
        montantException();
        return;
    }

    datedialog2->close();
    old_mont_base = mont_base;
    old_mont_contre = mont_contre;

    nb_trans++;
    mont_base += mont / open[num_trans] * (1-pourcentage);
    mont_contre -= mont;
    calcul_roi();

    ajout_table();

    if (nb_trans > 0)
        ui->actionAnnuler_la_derniere_transaction->setEnabled(true);
}

void MainWindow::vendre()
{
    date_trans = new QDate(QDate::fromString(editdate1->text(), "yyyy-MM-dd"));
    mont = montant->text().toInt();

    if (*date_trans < date[0] || *date_trans > date[nb_donnee-1])
    {
        dateException();
        return;
    }
    num_trans = date[0].daysTo(*date_trans);

    if (mont > mont_base || mont <= 0)
    {
        montantException();
        return;
    }
    datedialog2->close();

    old_mont_base = mont_base;
    old_mont_contre = mont_contre;

    nb_trans++;
    mont_contre += mont * (1 - pourcentage) * open[num_trans];
    mont_base -= mont;
    calcul_roi();

    ajout_tablev();

    if (nb_trans > 0)
        ui->actionAnnuler_la_derniere_transaction->setEnabled(true);
}

void MainWindow::on_actionAnnuler_la_derniere_transaction_triggered()
{
    mont_base = old_mont_base;
    mont_contre = old_mont_contre;

    table->removeRow(nb_trans-1);
    nb_trans--;
    if (nb_trans == 0)
        ui->actionAnnuler_la_derniere_transaction->setEnabled(false);
}

void MainWindow::on_actionQuitter_le_mode_actuel_triggered()
{
    mont_base = base_init;
    mont_contre = contre_init;

    dia_quit = new QDialog(this);
    dia_quit->setFixedSize(400, 360);
    dia_quit->show();

    QLabel *quitter = new QLabel(dia_quit);
    quitter->setText("Vous avez bien quitte du mode actuel.");

    textedit = new QTextEdit(dia_quit);
    textedit->setText(*note);
    textedit->setFixedHeight(160);


    QPushButton *ok = new QPushButton(dia_quit);
    ok->setText("Quit");
    ok->setFixedWidth(120);
    connect(ok, SIGNAL(clicked()), this, SLOT(quit_mode()));

    QPushButton *save1 = new QPushButton(dia_quit);
    save1->setText("Save fichier de donnees");
    save1->setFixedWidth(240);
    connect(save1, SIGNAL(clicked()), this, SLOT(save_csv()));

    QPushButton *save2 = new QPushButton(dia_quit);
    save2->setText("Save tableau de transactions");
    save2->setFixedWidth(240);
    connect(save2, SIGNAL(clicked()), this, SLOT(save_table()));

    QPushButton *save3 = new QPushButton(dia_quit);
    save3->setText("Save commentaire");
    save3->setFixedWidth(240);
    connect(save3, SIGNAL(clicked()), this, SLOT(save_com()));



    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(quitter);
    layout->addWidget(textedit);
    layout->addWidget(save1, 0, Qt::AlignCenter);
    layout->addWidget(save2, 0, Qt::AlignCenter);
    layout->addWidget(save3, 0, Qt::AlignCenter);

    layout->addWidget(ok, 0, Qt::AlignCenter);
    dia_quit->setLayout(layout);
}

void MainWindow::on_actionAjouter_une_strategie_triggered()
{
    addstratdialog = new QDialog(this);
    addstratdialog->setFixedSize(300, 200);
    addstratdialog->setWindowTitle("Ajout de strategie");
    addstratdialog->show();
    limit = new QString("Vous pouvez ajouter une strategie.\n\n");
    limit->append("Les dates des donnees sont: \n");
    limit->append(date[27].toString("yyyy-MM-dd"));
    limit->append(" -- ");
    limit->append(date[nb_donnee-2].toString("yyyy-MM-dd"));

    QLabel *label1 = new QLabel(addstratdialog);
    label1->setText("Date de debut: ");
    editdate1 = new QLineEdit(addstratdialog);
    editdate1->setInputMask("D999-99-99");
    editdate1->setFixedWidth(100);

    QLabel *label2 = new QLabel(addstratdialog);
    label2->setText("Date de fin: ");
    editdate2 = new QLineEdit(addstratdialog);
    editdate2->setInputMask("D999-99-99");
    editdate2->setFixedWidth(100);

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(label1);
    layout1->addWidget(editdate1);

    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(label2);
    layout2->addWidget(editdate2);

    QPushButton *valider = new QPushButton(addstratdialog);
    valider->setText("Valider");
    valider->setFixedWidth(150);
    connect(valider, SIGNAL(clicked()), this, SLOT(add_strat()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(*limit));
    layout->addLayout(layout1);
    layout->addLayout(layout2);
    layout->addWidget(valider, 0, Qt::AlignCenter);

    addstratdialog->setLayout(layout);


}

void MainWindow::add_strat()
{
    QDate date1 = QDate(QDate::fromString(editdate1->text(), "yyyy-MM-dd"));
    QDate date2 = QDate(QDate::fromString(editdate2->text(), "yyyy-MM-dd"));
    if (date1 < date[27] || date2 > date[nb_donnee - 2])
    {
        dateException();
        return;
    }

    addstratdialog->close();
    strat[5].setdebut(date1);
    strat[5].setfin(date2);

    QDialog *enregis = new QDialog(this);
    enregis->setFixedSize(400, 120);
    enregis->show();

    QLabel *label1 = new QLabel(enregis);
    QString *s = new QString("La nouvelle strategie est bien enregistree.\n");
    s->append(strat[nb_strat-1].getstrat());
    label1->setText(*s);

    QPushButton *ok = new QPushButton(enregis);
    ok->setText("OK");
    ok->setFixedWidth(120);
    connect(ok, SIGNAL(clicked()), enregis, SLOT(close()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1);
    layout->addWidget(ok, 0, Qt::AlignCenter);

    enregis->setLayout(layout);
}

void MainWindow::on_actionMode_pas_a_pas_triggered()
{
    datedialog3 = new QDialog(this);
    datedialog3->setFixedSize(350, 180);
    datedialog3->show();

    limit = new QString;
    limit->append("Entrez la date de debut.\n");
    limit->append("Les dates des donnees sont: \n");
    limit->append(date[16].toString("yyyy-MM-dd"));
    limit->append(" -- ");
    limit->append(date[nb_donnee-2].toString("yyyy-MM-dd"));

    QLabel *label1 = new QLabel(datedialog3);
    label1->setText(*limit);

    QLabel *label2 = new QLabel(datedialog3);
    label2->setText("Date de debut: ");

    editdate1 = new QLineEdit(datedialog3);
    editdate1->setInputMask("D999-99-99");
    editdate1->setFixedWidth(100);

    QPushButton *valider = new QPushButton(datedialog3);
    valider->setText("Valider");
    valider->setFixedWidth(120);
    connect(valider, SIGNAL(clicked()), this, SLOT(modepap()));

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(label2);
    layout1->addWidget(editdate1);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1);
    layout->addLayout(layout1);
    layout->addWidget(valider, 0, Qt::AlignCenter);

    datedialog3->setLayout(layout);

    pas = new Mode_pas;
    pas->setaujour(QDate(0, 0, 0));
    pas->setmontant(0);
}

void MainWindow::on_actionNote_triggered()
{
    notedialog = new QDialog(this);
    notedialog->setFixedSize(400, 280);
    notedialog->setWindowTitle("Note");
    notedialog->show();

    textedit = new QTextEdit(notedialog);
    textedit->setText(*note);
    textedit->setFixedHeight(200);

    QPushButton *save = new QPushButton(notedialog);
    save->setText("Enregistrer");
    save->setFixedWidth(120);
    connect(save, SIGNAL(clicked()), this, SLOT(save_note()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textedit);
    layout->addWidget(save, 0, Qt::AlignCenter);

    notedialog->setLayout(layout);
}

void MainWindow::save_note()
{
    *note = textedit->toPlainText();
    notedialog->close();
}

void MainWindow::save_csv()
{
    filename_save = QFileDialog::getSaveFileName(this,
            tr("Save fichiers"), "", tr("*.csv"));

    if(filename_save.isEmpty())
        return;

    QFile file(filename_csv);
    file.copy(filename_save);

}


void MainWindow::save_com()
{
    *note = textedit->toPlainText();
    note->replace(QString("\n"), QString("\r\n"));

    filename_save = QFileDialog::getSaveFileName(this,
            tr("Save fichiers"), "", tr("*.txt"));

    if(filename_save.isEmpty())
        return;

    QFile file(filename_save);

    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return;

    QTextStream infile(&file);
    infile << *note << "\r\n";
    file.close();
}

void MainWindow::quit_mode()
{
    dia_quit->close();

    table->clearContents();

    *note = "Ecrivez votre commentaire...";
    num_fin = nb_donnee;

    ui->actionNote->setEnabled(false);
    periode->setEnabled(true);

    suivant->setEnabled(true);
    ui->actionQuitter_le_mode_actuel->setEnabled(false);
    ui->actionParametres->setEnabled(true);
    ui->actionMode_Manuel->setEnabled(true);
    ui->actionMode_automatique->setEnabled(true);
    ui->actionAnnuler_la_derniere_transaction->setEnabled(false);
    ui->actionTransaction->setEnabled(false);
    ui->actionAjouter_une_strategie->setEnabled(true);
    ui->actionMode_pas_a_pas->setEnabled(true);
    ui->actionTransaction_Mode_pas_a_pas->setEnabled(false);
    ui->actionPasser_au_demain->setEnabled(false);
}

void MainWindow::modepap()
{
    ui->actionTransaction_Mode_pas_a_pas->setEnabled(true);
    ui->actionPasser_au_demain->setEnabled(true);
    ui->actionNote->setEnabled(true);
    ui->actionQuitter_le_mode_actuel->setEnabled(true);

    periode->setEnabled(false);

    pas->setaujour(QDate::fromString(editdate1->text(), "yyyy-MM-dd"));

    if (pas->getaujour() < date[0] || pas->getaujour() > date[nb_donnee-1])
    {
        dateException();
        return;
    }

    datedialog3->close();

    num_debut = date[0].daysTo(pas->getaujour())-15;
    num_fin = date[0].daysTo(pas->getaujour());

    regrapher(num_debut);

    ui->actionMode_Manuel->setEnabled(false);
    ui->actionMode_pas_a_pas->setEnabled(false);
    ui->actionMode_automatique->setEnabled(false);
    ui->actionAjouter_une_strategie->setEnabled(false);

}

void MainWindow::mode_auto()
{
    int a = btn_grp->checkedId();
    if (a == -1)
    {
        modeauto->close();
        return;
    }

    editdate1 = new QLineEdit;
    editdate1->setText(date_auto.toString("yyyy-MM-dd"));
    montant = new QLineEdit;

    modeauto->close();

    if (mm9 == 0.0)
        date_auto = strat[a].getdebut();

    modeauto = new QDialog(this);
    modeauto->setFixedSize(500, 300);
    modeauto->show();
    //modeauto->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    QString *str = new QString("");
    str->append("Bienvenu dans mode automatique.\n");
    str->append("Les transactions sont automatiquement proposee et appliquee.\n\n");
    str->append("La date d'aujourd'hui est ");
    str->append(date_auto.toString("yyyy-MM-dd"));
    str->append("\nLe prix d'ouverture est ");
    num_date_auto = date[0].daysTo(date_auto);
    str->append(QString::number(open[num_date_auto]));
    str->append("\n\nDevise de base: ");
    str->append(QString::number(mont_base));
    str->append("\nDevise de contrepartie: ");
    str->append(QString::number(mont_contre));
    str->append("\nPourcentage du prelevement: ");
    str->append(QString::number(pourcentage));


    QString *dec = new QString("");
    dec->append("\n\nLa decision est ");
    switch (decision())
    {
        case -1:
            dec->append("VENDRE.\n");
            montant->setText(QString::number(mont_base));
            break;
        case 1:
            dec->append("ACHETER.\n");
            montant->setText(QString::number(mont_contre));
            break;
        case 0:
            dec->append("RESTER.\n");
            break;
    }

    QLabel *label1 = new QLabel(modeauto);
    label1->setText(*str);

    QLabel *label2 = new QLabel(modeauto);
    label2->setText(*dec);

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    label2->setPalette(pe);

    QPushButton *demain = new QPushButton(modeauto);
    if (date_auto == strat[a].getfin())
        demain->setEnabled(false);
    demain->setText("Passer au demain");
    demain->setFixedWidth(160);
    connect(demain, SIGNAL(clicked()), this, SLOT(demain_auto()));

    QPushButton *quit = new QPushButton(modeauto);

    quit->setText("Quitter");
    quit->setFixedWidth(120);
    connect(quit, SIGNAL(clicked()), this, SLOT(quit_auto()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1);
    layout->addWidget(label2);
    layout->addWidget(demain, 0, Qt::AlignCenter);
    layout->addWidget(quit, 0, Qt::AlignCenter);

    modeauto->setLayout(layout);

    ui->actionMode_Manuel->setEnabled(false);
    ui->actionMode_pas_a_pas->setEnabled(false);
    ui->actionMode_automatique->setEnabled(false);
    ui->actionAjouter_une_strategie->setEnabled(false);
    ui->actionParametres->setEnabled(false);
}

void MainWindow::save_table()
{
    filename_save = QFileDialog::getSaveFileName(this,
            tr("Save fichiers"), "", tr("*.txt"));
    if(filename_save.isEmpty())
        return;

    int row = nb_trans;
    qDebug() << row;
    int col = table->columnCount();

    QList<QString> list;

    QString HeaderRow;
    for(int i = 0; i < col; i++)
    {
        HeaderRow.append(table->horizontalHeaderItem(i)->text()+"\t");
    }
    list.push_back(HeaderRow);

    for(int i = 0; i < row; i++)
    {
        QString rowStr = "";
        for(int j = 0; j < col; j++)
        {
            rowStr.append(table->item(i,j)->text());
            rowStr.append("\t");
        }
        list.push_back(rowStr);
    }

    QTextEdit textEdit;
    for(int i = 0; i < list.size(); i++)
    {
        textEdit.append(list.at(i));
    }

    QFile file(filename_save);

    if(file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&file);
        ts.setCodec("UTF-8");
        ts<<textEdit.document()->toPlainText();
        file.close();
    }

    nb_trans = 0;
}


void MainWindow::on_actionTransaction_Mode_pas_a_pas_triggered()
{
    datedialog2 = new QDialog(this);
    datedialog2->setWindowTitle("Mode pas a pas");
    datedialog2->setFixedSize(450, 260);
    datedialog2->show();

    limit = new QString("Vous voulez effectuer une transaction aujourd'hui ?\n");
    limit->append("La date d'aujourd'hui est ");
    limit->append(pas->getaujour().toString("yyyy-MM-dd"));

    limit->append("\n\nLe prix d'ouverture d'aujourd'hui est: ");
    limit->append(QString::number(open[date[0].daysTo(pas->getaujour())]));

    limit->append("\n\nDevise de base: ");
    limit->append(QString::number(mont_base));
    limit->append("\nDevise de contrepartie: ");
    limit->append(QString::number(mont_contre));
    limit->append("\nPourcentage du prelevement: ");
    limit->append(QString::number(pourcentage));

    QLabel *label1 = new QLabel(datedialog2);
    label1->setText(*limit);

    editdate1->setText(pas->getaujour().toString("yyyy-MM-dd"));

    QLabel *label_montant = new QLabel(datedialog2);
    label_montant->setText("Le montant: ");
    montant = new QLineEdit(datedialog2);
    montant->setFixedWidth(100);

    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(label_montant);
    layout2->addWidget(montant);

    QPushButton *acheter = new QPushButton(datedialog2);
    acheter->setText("Acheter");
    acheter->setFixedWidth(150);
    connect(acheter, SIGNAL(clicked()), this, SLOT(acheter()));

    QPushButton *vendre = new QPushButton(datedialog2);
    vendre->setText("Vendre");
    vendre->setFixedWidth(150);
    connect(vendre, SIGNAL(clicked()), this, SLOT(vendre()));

    QHBoxLayout *layout4 = new QHBoxLayout;
    layout4->addWidget(acheter);
    layout4->addWidget(vendre);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1);
    layout->addLayout(layout2);
    layout->addLayout(layout4);

    datedialog2->setLayout(layout);
}

void MainWindow::on_actionPasser_au_demain_triggered()
{
    pas->setaujour(pas->getaujour().addDays(1));
    num_debut++;
    num_fin++;
    regrapher(num_debut);
}

void MainWindow::demain_auto()
{
    switch (decision())
    {
        case -1:
            vendre_auto();
        break;

        case 1:
            acheter_auto();
        break;
    }

    date_auto = date_auto.addDays(1);
    modeauto->close();
    mode_auto();
}

void MainWindow::quit_auto()
{
    mont_base = base_init;
    mont_contre = contre_init;

    mm9 = 0.0;
    mm12 = 0.0;
    mm26 = 0.0;
    rsi = 0.0;
    macd = 0.0;

    dia_quit = new QDialog(this);
    dia_quit->setFixedSize(400, 360);
    dia_quit->show();

    QLabel *quitter = new QLabel(dia_quit);
    quitter->setText("Vous avez bien quitte du mode automatique.");

    textedit = new QTextEdit(dia_quit);
    textedit->setText(*note);
    textedit->setFixedHeight(160);

    QPushButton *ok = new QPushButton(dia_quit);
    ok->setText("Quit");
    ok->setFixedWidth(120);
    connect(ok, SIGNAL(clicked()), this, SLOT(quit_mode()));

    QPushButton *save1 = new QPushButton(dia_quit);
    save1->setText("Save fichier de donnees");
    save1->setFixedWidth(240);
    connect(save1, SIGNAL(clicked()), this, SLOT(save_csv()));

    QPushButton *save2 = new QPushButton(dia_quit);
    save2->setText("Save tableau de transactions");
    save2->setFixedWidth(240);
    connect(save2, SIGNAL(clicked()), this, SLOT(save_table()));

    QPushButton *save3 = new QPushButton(dia_quit);
    save3->setText("Save commentaire");
    save3->setFixedWidth(240);
    connect(save3, SIGNAL(clicked()), this, SLOT(save_com()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(quitter);
    layout->addWidget(textedit);
    layout->addWidget(save1, 0, Qt::AlignCenter);
    layout->addWidget(save2, 0, Qt::AlignCenter);
    layout->addWidget(save3, 0, Qt::AlignCenter);

    layout->addWidget(ok, 0, Qt::AlignCenter);
    dia_quit->setLayout(layout);

    modeauto->close();
}
