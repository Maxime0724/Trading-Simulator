#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QLabel>
#include<QPushButton>
#include<QLineEdit>
#include<QtCore/QTextStream>
#include<QtCore/QFile>
#include<QtCore/QIODevice>
#include"evolutionviewer.h"
#include"volumeviewer.h"
#include"trading.h"
#include"strategie.h"
#include"mode_pas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    EvolutionViewer *v;
    VolumeViewer *eview;
    EvolutionCours *e;
    QWidget *widget;
    QLabel *label;
    QString *limit;

    QString filename_csv;
    QString filename_save;

    QLineEdit *editdate1;
    QLineEdit *editdate2;
    QLineEdit *montant;
    QLineEdit *editbase;
    QLineEdit *editcontre;
    QLineEdit *editpourc;

    QDialog *datedialog;
    QDialog *datedialog2;
    QDialog *addstratdialog;
    QDialog *parametre;
    QDialog *modeauto;
    QDialog *modemanuel;
    QDialog *dia_quit;
    QDialog *notedialog;
    QDialog *datedialog3;

    QTextEdit *textedit;
    QString *note;

    QPushButton *precedent;
    QPushButton *periode;
    QPushButton *suivant;

    QErrorMessage *dateError;
    QErrorMessage *montantError;

    QStringList header;
    QTableWidget *table;

    QDate *date;
    double open[5000];
    double high[5000];
    double low[5000];
    double close[5000];
    double adj_close[5000];
    int volume[5000];

    int nb_donnee;
    long long int num_debut = 0;
    long long int num_fin;

    QDate *date_trans;
    int mont;
    long long int num_trans;
    int nb_trans = 0;

    int mont_base = 0;
    int mont_contre = 1000000;
    double pourcentage = 0.001;
    double roi;

    int base_init = 0;
    int contre_init = 1000000;

    int old_mont_base = 0;
    int old_mont_contre = 1000000;

    Strategie *strat;
    int nb_strat = 6;
    QButtonGroup *btn_grp;

    Mode_pas *pas;

    QDate date_auto;
    long long int num_date_auto;
    double mm9 = 0.0;
    double mm12;
    double mm26;
    double macd;
    double rsi;



public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void importMsg(QString filename);
    void dateException();
    void montantException();
    void regrapher(long long num_debut);
    void create_table();
    void ajout_table();
    void ajout_tablev();
    void calcul_roi();
    void create_strat();

    double getmm9(QDate d);
    double getmm12(QDate d);
    double getmm26(QDate d);
    double getrsi(QDate d);
    double getmacd(QDate d);

    int decision();

    void acheter_auto();
    void vendre_auto();

private slots:
    void on_actionOuvrier_un_file_csv_triggered();
    void createdatedialog();
    void dateSaisie();
    void datePrecedent();
    void dateSuivant();
    void on_actionMode_Manuel_triggered();
    void mode_manuel();
    void on_actionParametres_triggered();
    void saveparametres();
    void on_actionMode_automatique_triggered();
    void on_actionTransaction_triggered();
    void acheter();
    void vendre();
    void on_actionAnnuler_la_derniere_transaction_triggered();
    void on_actionQuitter_le_mode_actuel_triggered();
    void on_actionAjouter_une_strategie_triggered();
    void add_strat();
    void on_actionMode_pas_a_pas_triggered();
    void on_actionNote_triggered();
    void save_note();
    void save_csv();
    void save_table();
    void save_com();
    void quit_mode();
    void modepap();
    void mode_auto();
    void on_actionTransaction_Mode_pas_a_pas_triggered();
    void on_actionPasser_au_demain_triggered();
    void demain_auto();
    void quit_auto();
public slots:


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
