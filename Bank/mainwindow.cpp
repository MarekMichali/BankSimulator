#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QErrorMessage>
#include "admin.h"
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <QtNetwork>
#include <regex>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    try{
        l_admini.wczytaj();
        l_klienci.wczytaj();
        l_konta.wczytaj();

    }
    catch(std::runtime_error &blad){

        QMessageBox::warning(this, "Blad", blad.what());

    }
    l_konta.naliczLokaty();
    l_konta.naliczRaty();
    l_konta.przelewCykl();
    watek=new sesja(this);
    connect(watek,SIGNAL(zmiana_czas(int)),this,SLOT(on_zmiana_czas(int)));
    ui->setupUi(this);
    ui->label_filtr_mode->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


//logowanie admina i klienmta
void MainWindow::on_pushButton_login_clicked()
{
    std::string log = ui->lineEdit_login->text().toUtf8().constData();
    std::string pass = ui->lineEdit_password->text().toUtf8().constData();
    ui->label_b_acc_indx->hide();
    ui->label_sel_acc_mode->hide();
    ui->label_ad_ad_mode->hide();
    ui->label_filtr_mode->hide();
    ui->label_edit_b_mode->hide();
    ui->label_acc_indx->hide();
    ui->label_nr_konta_lok->hide();
    int success=l_admini.logowanie(log,pass,id);
    if(success==0){
        success=l_klienci.logowanie(log,pass,id);
    }
    if(success==1){
        QMessageBox::information(this, "Login", "Logowanie się powiodło.");
        watek->stop=false;
        watek->start();
        ui->lineEdit_login->clear();
        ui->stackedWidget_main->setCurrentIndex(3);
    }
    else if(success==0){
         QMessageBox::warning(this, "Login", "Niepoprawne hasło lub login.");
    }
    else if(success==20){
         QMessageBox::warning(this, "Login", "Twoje konto jest zablokowane. Skontaktuj sie z pracownikiem banku w celu odblokownia.");
    }
    else if(success==30){
         QMessageBox::warning(this, "Login", "Błędne hasło.");
    }
    else if(success==10){
        QMessageBox::information(this, "Login", "Logowanie się powiodło.");
        ui->lineEdit_login->clear();
        int i=l_klienci.znajdzIndeks(id);
        int x=l_klienci.klienci[i].nr_hasla;
        ui->label_password_nr->setText(QString::number(x));
        ui->stackedWidget_main->setCurrentIndex(1);
    }
    ui->lineEdit_password->clear();
}


//zamykanie aplikacji
void MainWindow::on_pushButton_cancel_clicked()
{
    delete watek;
    l_admini.zapisz();
    l_klienci.zapisz();
    l_konta.zapisz();
    close();
}


//wylogowanie z admin panelu
void MainWindow::on_pushButton_logout_adm_clicked()
{
     watek->stop=true;
     QMessageBox::information(this, "", "Wylogowano.");
     foreach(QLineEdit* le, ui->stackedWidget_main->findChildren<QLineEdit*>()) {
         le->clear();
     }
     ui->stackedWidget_admin->setCurrentIndex(0);
     ui->stackedWidget_main->setCurrentIndex(0);
     id="0";
}

//zmiana hasla admina
void MainWindow::on_pushButton_change_pass_adm_clicked()
{
    ui->stackedWidget_admin->setCurrentIndex(1);
}

//zmiana hasla admina
void MainWindow::on_pushButton_submit_pass_adm_clicked()
{
    std::string oldpass = ui->lineEdit_old_pw_adm->text().toUtf8().constData();
    std::string newpass = ui->lineEdit_new_pw_adm->text().toUtf8().constData();
    std::string newpass2 = ui->lineEdit_new_pw2_adm->text().toUtf8().constData();
    int success=-1;
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        success=l_admini.admini[i].zmienHaslo(oldpass, newpass, newpass2);
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
    switch(success){
    case 0:
        QMessageBox::warning(this, " ", "Hasło nie może zawierać spacji.");
        break;
    case 1:
        QMessageBox::information(this, "", "Zmiana hasła powiodła się.");
        break;
    case 2:
        QMessageBox::warning(this, " ", "Wprowadzone nowe hasła różnią się od siebie.");
        break;
    case 3:
        QMessageBox::warning(this, " ", "Stare hasło jest błędne.");
        break;
    case 4:
        QMessageBox::warning(this, " ", "Hasło nie może zawierać znaku \\.");
        break;
    default:
        QMessageBox::warning(this, " ", "Nie udało się zmienić hasła.");
        break;
    }
    ui->lineEdit_old_pw_adm->clear();
    ui->lineEdit_new_pw_adm->clear();
    ui->lineEdit_new_pw2_adm->clear();
}


//dodanie klienta/admina do bazy
void MainWindow::on_pushButton_add_person_clicked()
{
    ui->stackedWidget_admin->setCurrentIndex(2);
}


//dodawanie admina zmiana okienka
void MainWindow::on_pushButton_add_admin_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia==1){
            ui->lineEdit_privilige ->setValidator(new QIntValidator(1,9,this));
            ui->stackedWidget_admin->setCurrentIndex(3);
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }

}


//dodawanie admina
void MainWindow::on_pushButton_add_adm_clicked()
{
    if(ui->lineEdit_pesel_adm->text().isEmpty() || ui->lineEdit_pass_adm->text().isEmpty() || ui->lineEdit_2pass_adm->text().isEmpty() ||
            ui->lineEdit_name_adm->text().isEmpty() || ui->lineEdit_2name_adm->text().isEmpty() || ui->lineEdit_privilige->text().isEmpty()){
        QMessageBox::information(this, "", "Wszystkie pola muszą być wypełnione.");
        return;
    }
    std::string pes= ui->lineEdit_pesel_adm->text().toUtf8().constData();
    std::string newpw= ui->lineEdit_pass_adm->text().toUtf8().constData();
    std::string newpw2= ui->lineEdit_2pass_adm->text().toUtf8().constData();
    std::string nam= ui->lineEdit_name_adm->text().toUtf8().constData();
    std::string nam2= ui->lineEdit_2name_adm->text().toUtf8().constData();
    std::string privi= ui->lineEdit_privilige->text().toUtf8().constData();
    int success=l_admini.dodaj(pes,newpw,newpw2,nam,nam2,privi);
    switch(success){
    case 1:{
        pes.append(privi);
        std::string info="Użytkownik został dodany. Jego login to pesel + poziom uprawnień: ";
        info.append(pes);
        QMessageBox::information(this, "", QString::fromStdString(info));
        foreach(QLineEdit* le, ui->stackedWidget_admin->findChildren<QLineEdit*>()) {
            le->clear();
        }
        break;
        }
    case 2:
        QMessageBox::warning(this, " ", "Żadne z pól nie może zawierać spacji, użyj zamiast tego znaku '_'.");
        break;
    case 3:
        QMessageBox::warning(this, " ", "Wprowadzone hasła różnią się od siebie.");
        break;
    case 4:
        QMessageBox::warning(this, " ", "Użytkownik o takim loginie/peselu już istnieje.");
        ui->lineEdit_pesel_adm->clear();
        break;
    case 5:
        QMessageBox::warning(this, " ", "Żadne z pól nie może zawierać znaku '\\'.");
        break;
    case 6:
        QMessageBox::warning(this, " ", "Pesel musi składać się z 11 cyfr.");
        ui->lineEdit_pesel_adm->clear();
        break;
    default:
        QMessageBox::warning(this, " ", "Nie udało się dodać użytkownika.");
        break;
    }
    ui->lineEdit_pass_adm->clear();
    ui->lineEdit_2pass_adm->clear();
}


//edytowanie konta
void MainWindow::on_pushButton_change_acc_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
         if(l_admini.admini[i].uprawnienia==1 || l_admini.admini[i].uprawnienia==2){
             ui->stackedWidget_admin->setCurrentIndex(4);
         }
         else{
             QMessageBox::warning(this, " ", "Brak uprawnień.");
         }
    }

}


//edytowanie admina - wczytanie tabelki adminow
void MainWindow::on_pushButton_edit_adm_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia==1 || l_admini.admini[i].uprawnienia==2){
            ui->lineEdit_edit_adm_privile->setValidator(new QIntValidator(1,9,this));
            ui->label_ad_ad_mode->setText("1");
            ui->stackedWidget_admin->setCurrentIndex(5);
            ui->tableWidget_admins->setColumnCount(4);
            QStringList naglowki;
            naglowki << "Login" << "Imię" << "Nazwisko" << "Uprawnienia";
            ui->tableWidget_admins->setHorizontalHeaderLabels(naglowki);
            ui->tableWidget_admins->setRowCount(l_admini.admini.size());
            for(int y=0; y<l_admini.admini.size();y++){
                for(int x=0;x<4;x++){
                    switch(x){
                    case 0:
                        ui->tableWidget_admins->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_admini.admini[y].getPesel())));
                        break;
                    case 1:
                        ui->tableWidget_admins->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_admini.admini[y].imie)));
                        break;
                    case 2:
                        ui->tableWidget_admins->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_admini.admini[y].nazwisko)));
                        break;
                    case 3:
                        ui->tableWidget_admins->setItem(y,x,new QTableWidgetItem(QString::number(l_admini.admini[y].uprawnienia)));
                        break;
                    }
                }
            }
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
}


//przycisk filtrowania adminow
void MainWindow::on_pushButton_filtr_adm_clicked()
{
    QString filter=ui->lineEdit_filtr_adm->text();
    for(int y=0; y<ui->tableWidget_admins->rowCount(); ++y){
        bool jest=false;
        for(int x=0; x<ui->tableWidget_admins->columnCount(); ++x){
            QTableWidgetItem *item=ui->tableWidget_admins->item(y,x);
            if(item->text().contains(filter)){
                jest = true;
                break;
            }
        }
        ui->tableWidget_admins->setRowHidden(y, !jest);
    }
}


//double click na komorke, aby edytowac jakiegos admina
void MainWindow::on_tableWidget_admins_cellDoubleClicked(int row, int column)
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia!=1){
            QMessageBox::warning(this, " ", "Brak uprawnień.");
            return;
        }
    }
    QString trybb=ui->label_ad_ad_mode->text();
    int tryb=trybb.toInt();
    if(tryb==0){

    }
    else if(tryb==1){
        QString login=ui->tableWidget_admins->item(row,0)->text();
        int index=l_admini.znajdzIndeks(login.toUtf8().constData());
        ui->lineEdit_edit_adm_log->setText(QString::fromStdString(l_admini.admini[index].getPesel()));
        ui->lineEdit_edit_adm_name->setText(QString::fromStdString(l_admini.admini[index].imie));
        ui->lineEdit_edit_adm_name2->setText(QString::fromStdString(l_admini.admini[index].nazwisko));
        ui->lineEdit_edit_adm_privile->setText(QString::number(l_admini.admini[index].uprawnienia));
        ui->label_index_adm_editet->setText(QString::number(index));    //indeks edytowanego admina w vectorze
        ui->label_index_adm_editet->hide();
        if(id==login.toUtf8().constData()){
            ui->label_pesel_adm_editing->setText(QString::fromStdString(id));  //jesli zmieniam sam siebie
        }
        else{
            ui->label_pesel_adm_editing->setText(QString::number(-1));
        }
        ui->label_pesel_adm_editing->hide(); 
        ui->stackedWidget_admin->setCurrentIndex(6);
    }

}


//potwierdz edycje admina
void MainWindow::on_pushButton_edit_adm_submit_clicked()
{
    if(ui->lineEdit_edit_adm_log->text().isEmpty() || ui->lineEdit_edit_adm_newpw->text().isEmpty() || ui->lineEdit_edit_adm_newpw2->text().isEmpty()
            || ui->lineEdit_edit_adm_name->text().isEmpty() || ui->lineEdit_edit_adm_name2->text().isEmpty() || ui->lineEdit_edit_adm_privile->text().isEmpty()){
        QMessageBox::information(this, "", "Wszystkie pola muszą być wypełnione.");
        return;
    }
    std::string pes= ui->lineEdit_edit_adm_log->text().toUtf8().constData();
    std::string newpw= ui->lineEdit_edit_adm_newpw->text().toUtf8().constData();
    std::string newpw2= ui->lineEdit_edit_adm_newpw2->text().toUtf8().constData();
    std::string nam= ui->lineEdit_edit_adm_name->text().toUtf8().constData();
    std::string nam2= ui->lineEdit_edit_adm_name2->text().toUtf8().constData();
    std::string privi= ui->lineEdit_edit_adm_privile->text().toUtf8().constData();
    std::string ind= ui->label_index_adm_editet->text().toUtf8().constData();
    int success=0;
    success=l_admini.edytujAdmina(pes,newpw,newpw2,nam,nam2,privi,ind);
    QString p=ui->label_pesel_adm_editing->text();
    switch(success){
    case 1:{
        pes.append(privi);
        std::string info="Użytkownik został dodany. Jego login to pesel + poziom uprawnień: ";
        info.append(pes);
        QMessageBox::information(this, "", QString::fromStdString(info));
        foreach(QLineEdit* le, ui->stackedWidget_admin->findChildren<QLineEdit*>()) {
            le->clear();
        }
        if(p.toInt()!=-1){
            id=pes;
        }
        ui->stackedWidget_admin->setCurrentIndex(0);
        break;
    }
    case 2:
        QMessageBox::warning(this, " ", "Zadne z pol nie może zawierać spacji, użyj zamiast tego znaku '_'.");
        break;
    case 3:
        QMessageBox::warning(this, " ", "Wprowadzone hasła różniż się od siebie.");
        break;
    case 4:
        QMessageBox::warning(this, " ", "Użytkownik o takim loginie/peselu już istnieje.");
        ui->lineEdit_edit_adm_log->setText(QString::fromStdString(l_admini.admini[stoi(ind)].getPesel()));
        break;
    case 5:
        QMessageBox::warning(this, " ", "Żadne z pól nie może zawierać znaku '\\'.");
        break;
    case 6:
        QMessageBox::warning(this, " ", "Pesel musi składać się z 11 cyfr.");
        break;
    default:
        QMessageBox::warning(this, " ", "Nie udało się dodać użytkownika.");
        break;
    }
    ui->lineEdit_edit_adm_newpw->clear();
    ui->lineEdit_edit_adm_newpw2->clear();
}


//usuwanie wybranego admina z tabeli
void MainWindow::on_pushButton_delete_adm_clicked()
{
    if(id==ui->label_pesel_adm_editing->text().toUtf8().constData()){
         QMessageBox::warning(this, " ", "Nie możesz usunąć siebie samego.");
    }
    else{
        QString index=ui->label_index_adm_editet->text();
        l_admini.admini.erase(l_admini.admini.begin()+index.toInt());
        QMessageBox::information(this, "", "Użytkownik został usunięty.");
        ui->stackedWidget_admin->setCurrentIndex(0);
    }
}


//dodawanie klienta do bazy przez admina
void MainWindow::on_pushButton_add_client_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia==1 || l_admini.admini[i].uprawnienia==2){
            ui->stackedWidget_admin->setCurrentIndex(7);
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
}


//dodawanie klienta do bazy przez admina tworzenie konta
void MainWindow::on_pushButton_create_cl_clicked()
{
    if(ui->lineEdit_pesel_cl->text().isEmpty() || ui->lineEdit_pw_cl->text().isEmpty() || ui->lineEdit_pw2_cl->text().isEmpty() || ui->lineEdit_name_cl->text().isEmpty() || ui->lineEdit_name2_cl->text().isEmpty()){
        QMessageBox::information(this, "", "Wszystkie pola muszą być wypełnione.");
        return;
    }
    std::string pes= ui->lineEdit_pesel_cl->text().toUtf8().constData();
    std::string newpw= ui->lineEdit_pw_cl->text().toUtf8().constData();
    std::string newpw2= ui->lineEdit_pw2_cl->text().toUtf8().constData();
    std::string nam= ui->lineEdit_name_cl->text().toUtf8().constData();
    std::string nam2= ui->lineEdit_name2_cl->text().toUtf8().constData();
    int success=l_klienci.dodaj(pes,newpw,newpw2,nam,nam2);
    switch(success){
    case 1:
        QMessageBox::information(this, "", "Użytkownik zostal dodany. Nie zapomnij przekazać listy jednorazowych haseł.");
        foreach(QLineEdit* le, ui->stackedWidget_admin->findChildren<QLineEdit*>()) {
            le->clear();
        }
        break;
    case 2:
        QMessageBox::warning(this, " ", "Żadne z pól nie może zawierać spacji, użyj zamiast tego znaku '_'.");
        break;
    case 3:
        QMessageBox::warning(this, " ", "Wprowadzone hasła różnią się od siebie.");
        break;
    case 4:
        QMessageBox::warning(this, " ", "Użytkownik o takim loginie/peselu już istnieje.");
        ui->lineEdit_pesel_cl->clear();
        break;
    case 5:
        QMessageBox::warning(this, " ", "Żadne z pól nie może zawierać znaku '\\'.");
        break;
    case 6:
        QMessageBox::warning(this, " ", "Pesel musi składć się z 11 cyfr.");
        ui->lineEdit_pesel_cl->clear();
        break;
    default:
        QMessageBox::warning(this, " ", "Nie udało się dodać użytkownika.");
        break;
    }
    ui->lineEdit_pw_cl->clear();
    ui->lineEdit_pw2_cl->clear();
}


//anulowanie logowania klienta panel haslo jednorazowe
void MainWindow::on_pushButton_cancel_tmp_clicked()
{
    ui->lineEdit_password_tmp->clear();
    ui->stackedWidget_main->setCurrentIndex(0);
}


//logowanie haslo jednorazowe
void MainWindow::on_pushButton_login_tmp_clicked()
{
    int i=l_klienci.znajdzIndeks(id);
    int x=l_klienci.klienci[i].nr_hasla;
    QString haslo=ui->lineEdit_password_tmp->text();
    int pass=haslo.toInt();
    int success=0;
    success = l_klienci.klienci[i].logTmp(pass,x);
    if(success==1){
        QMessageBox::information(this, "", "Zalogowano.");
        ui->stackedWidget_main->setCurrentIndex(2);
        ui->lineEdit_password_tmp->clear();
        watek->stop=false;
        watek->start();
        int index=l_klienci.znajdzIndeks(id);
        if(l_klienci.klienci[index].nr_hasla>20){
            l_klienci.klienci[index].generujTmp();
            QMessageBox::information(this, "", "Ponieważ wykorzystałeś wszystkie hasła jednorazowe, nowa lista została stworzona w pliku o twoim loginie.");
        }
    }
    else if(success==2 || success==3){
        QMessageBox::warning(this, " ", "Twoje konto jest zablokowane. Proszę zgłosić się do banku w celu odblokowania");
        ui->lineEdit_password_tmp->clear();
    }
    else {
        QMessageBox::warning(this, " ", "Podano błędne hasło.");
        ui->lineEdit_password_tmp->clear();
    }
}


//wylogowanie klient
void MainWindow::on_pushButton_logout_clicked()
{
    watek->stop=true;
    QMessageBox::information(this, "", "Wylogowano.");
    foreach(QLineEdit* le, ui->stackedWidget_main->findChildren<QLineEdit*>()) {
        le->clear();
    }
    ui->stackedWidget_client->setCurrentIndex(0);
    ui->stackedWidget_main->setCurrentIndex(0);
    id="0";
}


//odblokowanie konta klienta i sprawdzanie uprawnien
void MainWindow::on_pushButton_unlock_acc_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia==1 || l_admini.admini[i].uprawnienia==2){

            ui->tableWidget_locked_acc->setColumnCount(3);
            QStringList naglowki;
            naglowki << "Pesel" << "Imię" << "Nazwisko";
            ui->tableWidget_locked_acc->setHorizontalHeaderLabels(naglowki);
            int wiersze=l_klienci.znajdzBlokady();
            ui->tableWidget_locked_acc->setRowCount(wiersze);
            int y=0;
            for(auto i=l_klienci.klienci.begin(); i<l_klienci.klienci.end();i++){
                if(i->blokada==5){
                    ui->tableWidget_locked_acc->setItem(y,0,new QTableWidgetItem(QString::fromStdString(i->getPesel())));
                    ui->tableWidget_locked_acc->setItem(y,1,new QTableWidgetItem(QString::fromStdString(i->imie)));
                    ui->tableWidget_locked_acc->setItem(y,2,new QTableWidgetItem(QString::fromStdString(i->nazwisko)));
                    y++;
                }
            }
            ui->stackedWidget_admin->setCurrentIndex(8);
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
}


//tabelka do odblokowania konta, doublecick odblokuje
void MainWindow::on_tableWidget_locked_acc_cellDoubleClicked(int row, int column)
{
    int index=l_klienci.znajdzIndeks(ui->tableWidget_locked_acc->item(row,0)->text().toUtf8().constData());
    l_klienci.klienci[index].blokada=0;
    QMessageBox::information(this, "", "Konto zostało odblokowane.");
    ui->stackedWidget_admin->setCurrentIndex(0);
}


//filtrowanie w tabelce zablokowanych klientow
void MainWindow::on_pushButton_filtr_lock_acc_clicked()
{
    QString filter=ui->lineEdit_filtr_lock_acc->text();
    for(int y=0; y<ui->tableWidget_locked_acc->rowCount(); ++y){
        bool jest=false;
        for(int x=0; x<ui->tableWidget_locked_acc->columnCount(); ++x){
            QTableWidgetItem *item=ui->tableWidget_locked_acc->item(y,x);
            if(item->text().contains(filter)){
                jest = true;
                break;
            }
        }
        ui->tableWidget_locked_acc->setRowHidden(y, !jest);
    }
}


//generowanie nowej listy hasel
void MainWindow::on_pushButton_new_tmp_pw_clicked()
{
    int index=l_klienci.znajdzIndeks(id);
    l_klienci.klienci[index].generujTmp();
    QMessageBox::information(this, "", "Nowa lista haseł została wygenerowana i znajduje się w pliku o twoim loginie.");
}


//zmiana hasla przez klienta
void MainWindow::on_pushButton_account_clicked()
{
   ui->stackedWidget_client->setCurrentIndex(1);
}


//zmiana hasla przez klienta
void MainWindow::on_pushButton_change_pw_cl_clicked()
{
    std::string oldpass = ui->lineEdit_old_pw_cl->text().toUtf8().constData();
    std::string newpass = ui->lineEdit_new_pw_cl->text().toUtf8().constData();
    std::string newpass2 = ui->lineEdit_new2_pw_cl->text().toUtf8().constData();
    int success=-1;
    int i=l_klienci.znajdzIndeks(id);
    if(i!=-1){
        success=l_klienci.klienci[i].zmienHaslo(oldpass, newpass, newpass2);
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
    switch(success){
    case 0:
        QMessageBox::warning(this, " ", "Hasło nie może zawierać spacji.");
        break;
    case 1:
        QMessageBox::information(this, "", "Zmiana hasła powiodła się.");
        ui->stackedWidget_client->setCurrentIndex(0);
        break;
    case 2:
        QMessageBox::warning(this, " ", "Wprowadzone nowe hasła różnią się od siebie.");
        break;
    case 3:
        QMessageBox::warning(this, " ", "Stare hasło jest błędne.");
        break;
    case 4:
        QMessageBox::warning(this, " ", "Hasło nie może zawierać znaku \\.");
        break;
    default:
        QMessageBox::warning(this, " ", "Nie udało się zmienić hasła.");
        break;
    }
    ui->lineEdit_old_pw_cl->clear();
    ui->lineEdit_new_pw_cl->clear();
    ui->lineEdit_new2_pw_cl->clear();
}


//wyswietlanie listy klientow
void MainWindow::on_pushButton_account_adm_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia!=99){
            ui->label_filtr_mode->setText("1");
            ui->tableWidget_cl_acc->setColumnCount(4);
            QStringList naglowki;
            naglowki << "Pesel" << "Imię" << "Nazwisko" << "Liczba kont";
            ui->tableWidget_cl_acc->setHorizontalHeaderLabels(naglowki);
            ui->tableWidget_cl_acc->setRowCount(l_klienci.klienci.size());
            for(int y=0; y<l_klienci.klienci.size();y++){
                for(int x=0;x<4;x++){
                    switch(x){
                    case 0:
                        ui->tableWidget_cl_acc->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_klienci.klienci[y].getPesel())));
                        break;
                    case 1:
                        ui->tableWidget_cl_acc->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_klienci.klienci[y].imie)));
                        break;
                    case 2:
                        ui->tableWidget_cl_acc->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_klienci.klienci[y].nazwisko)));
                        break;
                    case 3:
                        ui->tableWidget_cl_acc->setItem(y,x,new QTableWidgetItem(QString::number(l_klienci.klienci[y].ile_kont)));
                        break;
                    }
                }
            }
            ui->stackedWidget_admin->setCurrentIndex(10);
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
}


//zarzadzanie kontami klientow
void MainWindow::on_pushButton_manage_acc_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia==1 || l_admini.admini[i].uprawnienia==2){
            ui->stackedWidget_admin->setCurrentIndex(9);
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }

}


//tworzenie konta bankowego dla klienta
void MainWindow::on_pushButton_create_b_acc_clicked()
{
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("2");
}


//filtrowanie w liscie klientow
void MainWindow::on_pushButton_filtr_lcl_acc__clicked()
{
    QString filter=ui->lineEdit_filtr_cl_acc_->text();
    for(int y=0; y<ui->tableWidget_cl_acc->rowCount(); ++y){
        bool jest=false;
        for(int x=0; x<ui->tableWidget_cl_acc->columnCount(); ++x){
            QTableWidgetItem *item=ui->tableWidget_cl_acc->item(y,x);
            if(item->text().contains(filter)){
                jest = true;
                break;
            }
        }
        ui->tableWidget_cl_acc->setRowHidden(y, !jest);
    }
}


//dobuleclick na tabelke, tryby pracy:
//label_filtr_mode == 2  dodawanie konta dla klienta
//label_filtr_mode == 3  zmiana stanu konta klienta
//label_filtr_mode == 4  dodawanie lokaty dla konta klienta
//label_filtr_mode == 5  udzielanie kredytu
//label_filtr_mode == 6  zmiana danych klienta
//label_filtr_mode == 7  usuwanie konta klienta
//label_filtr_mode == 8  usuwanie lokaty
//label_filtr_mode == 9  usuwanie kredytu
void MainWindow::on_tableWidget_cl_acc_cellDoubleClicked(int row, int column)
{
    QString operacja=ui->label_filtr_mode->text();
    int tryb=operacja.toInt();
    if(tryb==1){
        //tylko wyswietlanie
    }
    else if(tryb==2){ //dodawanie konta dla klienta
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        std::string nr="4523"; //nr banku, uproszczony
        l_konta.generujNr(nr);
        ui->label_b_acc_nr->setText(QString::fromStdString(nr));
        ui->label_b_acc_name->setText(QString::fromStdString(l_klienci.klienci[index].imie));
        ui->label_b_acc_name2->setText(QString::fromStdString(l_klienci.klienci[index].nazwisko));
        ui->label_b_acc_pesel->setText(QString::fromStdString(l_klienci.klienci[index].getPesel()));
        ui->lineEdit_start_saldo->setValidator(new QDoubleValidator(this));
        ui->label_filtr_mode->setText("1");
        ui->lineEdit_start_saldo->clear();
        ui->stackedWidget_admin->setCurrentIndex(11);
    }
    else if(tryb==3){   //dodawanie/usuwanie srodkow
        ui->listWidget_accounts->clear();
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        ui->label_edit_b_mode->setText("3");
        for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
            new QListWidgetItem(tr((*i).c_str()), ui->listWidget_accounts);
        }
        ui->stackedWidget_admin->setCurrentIndex(12);
    }
    else if(tryb==4){   //tworzenie lokaty
        ui->listWidget_accounts->clear();
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        ui->label_edit_b_mode->setText("4");
        for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
            new QListWidgetItem(tr((*i).c_str()), ui->listWidget_accounts);
        }
        ui->stackedWidget_admin->setCurrentIndex(12);
    }
    else if(tryb==5){   //udzielanie kredytu
        ui->listWidget_accounts->clear();
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        ui->label_edit_b_mode->setText("5");
        for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
            new QListWidgetItem(tr((*i).c_str()), ui->listWidget_accounts);
        }
        ui->stackedWidget_admin->setCurrentIndex(12);
    }
    else if(tryb==6){ //zmiana danych
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        ui->lineEdit_pes_edit_cl->setText(QString::fromStdString(l_klienci.klienci[index].getPesel()));
        ui->lineEdit__name_edit_cl->setText(QString::fromStdString(l_klienci.klienci[index].imie));
        ui->lineEdit_name2_edit_cl->setText(QString::fromStdString(l_klienci.klienci[index].nazwisko));
        ui->label_index_cl_editet->setText(QString::number(index));    //indeks edytowanej osoby
        ui->label_index_cl_editet->hide();
        ui->label_pesel_cl_editing->setText(QString::fromStdString(l_klienci.klienci[index].getPesel()));
        ui->label_pesel_cl_editing->hide();
        ui->stackedWidget_admin->setCurrentIndex(19);
    }
    else if(tryb==7){ //usuwanie konta klienta
        ui->listWidget_accounts->clear();
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        ui->label_edit_b_mode->setText("7");
        for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
            new QListWidgetItem(tr((*i).c_str()), ui->listWidget_accounts);
        }
        ui->stackedWidget_admin->setCurrentIndex(12);
    }
    else if(tryb==8){ //usuwanie lokaty
        ui->listWidget_accounts->clear();
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        ui->label_edit_b_mode->setText("8");
        for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
            new QListWidgetItem(tr((*i).c_str()), ui->listWidget_accounts);
        }
        ui->stackedWidget_admin->setCurrentIndex(12);

    }
    else if(tryb==9){ //usuwanie kredytu
        ui->listWidget_accounts->clear();
        QString login=ui->tableWidget_cl_acc->item(row,0)->text();
        int index=l_klienci.znajdzIndeks(login.toUtf8().constData());
        ui->label_edit_b_mode->setText("9");
        for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
            new QListWidgetItem(tr((*i).c_str()), ui->listWidget_accounts);
        }
        ui->stackedWidget_admin->setCurrentIndex(12);
    }
}


//zatwierdzenie utworzenie konta
void MainWindow::on_pushButton_b_acc_create_clicked()
{
    std::string nr = ui->label_b_acc_nr->text().toUtf8().constData();
    std::string pes = ui->label_b_acc_pesel->text().toUtf8().constData();
    if(ui->lineEdit_start_saldo->text().isEmpty()){
        l_konta.dodaj(nr,pes,"0");
    }
    else{
        l_konta.dodaj(nr,pes,(ui->lineEdit_start_saldo->text().toUtf8().constData()));
    }
    int index=l_klienci.znajdzIndeks(pes);
    l_klienci.klienci[index].dodajKonto(nr);
    QMessageBox::information(this, "", "Konto zostało utworzone.");
    ui->stackedWidget_admin->setCurrentIndex(0);
}


//dodawanie/odejmowanie srodkow z konta opcja
void MainWindow::on_pushButton_add_rem_bal_to_acc_clicked()
{
    ui->label__edit_bal_id->hide();
    ui->lineEdit_edit_bal_zl->setValidator(new QDoubleValidator(this));
    ui->lineEdit_edit_bal_eu->setValidator(new QDoubleValidator(this));
    ui->lineEdit_edit_bal_d->setValidator(new QDoubleValidator(this));
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("3");
}


//cofanie do wyboru klienta do ktorego konta chcemy dodac/odjac pieniadze
void MainWindow::on_pushButton_back_sel_cl_clicked()
{
    on_pushButton_add_rem_bal_to_acc_clicked();
}

//wybor konta dla ktorego chcemy edytowac cos
//label_edit_b_mode == 3  wybor konta do ktorego chcemy dodac/usunac pieniadze
//label_edit_b_mode == 4  tworzenie lokaty
//label_edit_b_mode == 5  udzielanie kredytu
//label_edit_b_mode == 7  usuwanie konta klienta
//label_edit_b_mode == 8  usuwanie lokaty klienta
//label_edit_b_mode == 9  usuwanie kredytu klienta
void MainWindow::on_listWidget_accounts_itemDoubleClicked(QListWidgetItem *item)
{
    QString operacja=ui->label_edit_b_mode->text();
    int tryb=operacja.toInt();
    if(tryb==1){
        //tylko wyswietlanie
    }
    else if(tryb==3){   //wybor konta do ktorego chcemy dodac/usunac pieniadze
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_bal_zl->setText(QString::number(l_konta.konta[index].getSaldo(0),'d',2));
        ui->label_bal_eu->setText(QString::number(l_konta.konta[index].getSaldo(1),'d',2));
        ui->label_bal_d->setText(QString::number(l_konta.konta[index].getSaldo(2),'d',2));
        ui->label__edit_bal_id->setText(QString::number(index));
        ui->stackedWidget_admin->setCurrentIndex(13);
    }
    else if(tryb==4){   //tworzenie lokaty
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_acc_indx->setText(QString::number(index));
        ui->stackedWidget_admin->setCurrentIndex(15);
        ui->lineEdit_bal_lok->setValidator(new QDoubleValidator(this));
        ui->lineEdit_bal_lok->clear();
        ui->lineEdit_perc_lok->setValidator(new QDoubleValidator(this));
        ui->lineEdit_perc_lok->clear();

    }
    else if(tryb==5){   //udzielanie kredytu
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_acc_indx->setText(QString::number(index));
        ui->stackedWidget_admin->setCurrentIndex(17);
        ui->lineEdit_cr_money->setValidator(new QDoubleValidator(this));
        ui->lineEdit_cr_money->clear();
        ui->lineEdit_cr_per->setValidator(new QDoubleValidator(this));
        ui->lineEdit_cr_per->clear();
    }
    else if(tryb==7){   //usuwanie konta
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        l_konta.konta.erase(l_konta.konta.begin()+index);
        ui->stackedWidget_admin->setCurrentIndex(0);
    }
    else if(tryb==8){   //usuwanie lokaty
        ui->listWidget_lok->clear();
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        int j=0;
        ui->label_b_acc_indx->setText(QString::number(index));
        for(auto i=l_konta.konta[index].lokaty.begin();i<l_konta.konta[index].lokaty.end();i++){
            std::string tmp=std::to_string(j);
            tmp.append(" ");
            tmp.append(std::to_string(i->getKwota()));
            tmp.append(" ");
            tmp.append(std::to_string(i->procent));
            tmp.append("% ");
            tmp.append(std::to_string(i->data2.tm_mday));
            tmp.append(".");
            tmp.append(std::to_string(i->data2.tm_mon));
            tmp.append(".");
            tmp.append(std::to_string(i->data2.tm_year));
            new QListWidgetItem(tr(tmp.c_str()), ui->listWidget_lok);
            j++;
        }
        ui->stackedWidget_admin->setCurrentIndex(20);
    }
    else if(tryb==9){   //usuwanie kredytu
        ui->listWidget_lok->clear();
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        int j=0;
        ui->label_b_acc_indx->setText(QString::number(index));
        for(auto i=l_konta.konta[index].kredyty.begin();i<l_konta.konta[index].kredyty.end();i++){
            std::string tmp=std::to_string(j);
            tmp.append(" ");
            tmp.append(i->getKwota());
            tmp.append(" ");
            tmp.append(i->getProcent());
            tmp.append("% ");
            tmp.append(i->getRata());
            tmp.append(" ");
            tmp.append(std::to_string(i->data2.tm_mday));
            tmp.append(".");
            tmp.append(std::to_string(i->data2.tm_mon));
            tmp.append(".");
            tmp.append(std::to_string(i->data2.tm_year));
            new QListWidgetItem(tr(tmp.c_str()), ui->listWidget_lok);
            j++;
        }
        ui->stackedWidget_admin->setCurrentIndex(20);
    }
}


//zatwierdzenie zmiany stanu konta
void MainWindow::on_pushButton_edit_bal_clicked()
{
    std::string zl;
    std::string eu;
    std::string d;
    QString zloty=ui->lineEdit_edit_bal_zl->text();
    zl = zloty.toUtf8().constData();
    if(ui->lineEdit_edit_bal_zl->text().isEmpty()){
         zl="0";
    }
    QString euro=ui->lineEdit_edit_bal_eu->text();
    eu = euro.toUtf8().constData();
    if(ui->lineEdit_edit_bal_eu->text().isEmpty()){
         eu="0";
    }
    QString dolar=ui->lineEdit_edit_bal_d->text();
    d = dolar.toUtf8().constData();
    if(ui->lineEdit_edit_bal_d->text().isEmpty()){
         d="0";
    }
    l_konta.konta[ui->label__edit_bal_id->text().toInt()].editSaldo(zl,eu,d);
    foreach(QLineEdit* le, ui->stackedWidget_main->findChildren<QLineEdit*>()) {
        le->clear();
    }
    QMessageBox::information(this, "", "Saldo zostało zmienione");
    ui->stackedWidget_admin->setCurrentIndex(0);
}


//zarzdzanie lokatami
void MainWindow::on_pushButton_manage_lokaty_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia==1 || l_admini.admini[i].uprawnienia==3){
            ui->stackedWidget_admin->setCurrentIndex(14);
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
}


//wybor klienta dla ktorego chce sie utworzyc lokate
void MainWindow::on_pushButton_create_lokata_clicked()
{
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("4");
}


//zatwierdz utworzenie lokaty
void MainWindow::on_pushButton_submit_lok_clicked()
{
    std::string kw = ui->lineEdit_bal_lok->text().toUtf8().constData();
    std::string pr = ui->lineEdit_perc_lok->text().toUtf8().constData();
    if(ui->lineEdit_bal_lok->text().isEmpty() || ui->lineEdit_perc_lok->text().isEmpty()){
      QMessageBox::warning(this, " ", "Nie mogą być puste pola.");
    }
    else{
        //dodawanie lokaty
        QString index=ui->label_acc_indx->text();
        int i=index.toInt();
        int success=l_konta.konta[i].dodajLokata(kw,pr,ui->label_add_lok_year->text().toInt(),ui->label_add_lok_mon->text().toInt(),ui->label_add_lok_day->text().toInt());
        switch(success){
        case 0:
            QMessageBox::warning(this, " ", "puste");
            break;
        case 1:
            QMessageBox::information(this, "", "Lokata została utworzna.");
            ui->stackedWidget_admin->setCurrentIndex(0);
            break;
        case 2:
            QMessageBox::warning(this, " ", "Klient nie posiada wystarczających środków na koncie.");
            break;
        case 3:
            QMessageBox::warning(this, " ", "Data zamknięcia lokaty nie może być przeszła.");
            break;
        case 4:
            QMessageBox::warning(this, " ", "Wartości muszą być dodatnie.");
            break;
        default:
            QMessageBox::warning(this, " ", "Nie udało się utworzyć lokaty.");
            break;
        }
    }
}


//wybor daty dla zamkniecia lokaty
void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    ui->label_add_lok_year->setText(QString::number(date.year()));
    ui->label_add_lok_mon->setText(QString::number(date.month()));
    ui->label_add_lok_day->setText(QString::number(date.day()));
}


//klient przycisk przelew
void MainWindow::on_pushButton_transfer_clicked()
{
    foreach(QLineEdit* le, ui->stackedWidget_client->findChildren<QLineEdit*>()) {
        le->clear();
    }
    foreach(QTextEdit* le, ui->stackedWidget_client->findChildren<QTextEdit*>()) {
        le->clear();
    }
    ui->listWidget_cl_acc->clear();
    ui->label_sel_acc_mode->setText("1");
    ui->stackedWidget_client->setCurrentIndex(2);
    int index=l_klienci.znajdzIndeks(id);
    for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
        new QListWidgetItem(tr((*i).c_str()), ui->listWidget_cl_acc);
    }
}

//double click przez klienta na jedno ze swoich kont
//label_sel_acc_mode == 0 tylko dane konta
//label_sel_acc_mode == 1 przelew
//label_sel_acc_mode == 2 wyswietlanie informacji o kredytach
//label_sel_acc_mode == 3 wyswietlanie informacji o lokatach
//label_sel_acc_mode == 4 wyswietlanie informacji o przelewach cyklicznych i ich anulowanie
//label_sel_acc_mode == 5 historia transakcji
//label_sel_acc_mode == 6 wymiana walut
void MainWindow::on_listWidget_cl_acc_itemDoubleClicked(QListWidgetItem *item)
{
    QString operacja=ui->label_sel_acc_mode->text();
    int tryb=operacja.toInt();
    if(tryb==0){
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_b_acc_indx->setText(QString::number(index));
        ui->label_zl_bal->setText(QString::number(l_konta.konta[index].getSaldo(0)));
        ui->label_eu_bal->setText(QString::number(l_konta.konta[index].getSaldo(1)));
        ui->label_d_bal->setText(QString::number(l_konta.konta[index].getSaldo(2)));
        ui->label_lok_count->setText(QString::number(l_konta.konta[index].lokaty.size()));
        ui->label_trans_count->setText(QString::number(l_konta.konta[index].przelewy.size()));
        ui->label_cred_count->setText(QString::number(l_konta.konta[index].kredyty.size()));
        ui->stackedWidget_client->setCurrentIndex(4);
    }
    else if(tryb==1){    //wykonanie przelewu z tego konta
        ui->lineEdit_recip->setValidator(new QIntValidator(this));
        ui->lineEdit_rec_mon->setValidator(new QDoubleValidator(this));
        ui->lineEdit_cycle ->setValidator(new QIntValidator(this));
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_b_acc_indx->setText(QString::number(index));
        ui->stackedWidget_client->setCurrentIndex(3);
    }
    else if(tryb==2){ //wyswietlanie informacji o kredytach
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_b_acc_indx->setText(QString::number(index));
        ui->tableWidget_cl_credits->setColumnCount(6);
        QStringList naglowki;
        naglowki << "Kwota kredytu" << "Oprocentowanie" << "Miesięczna rata" << "Data podpisania umowy" << "Data ostatniej wpłaconej raty" << "Data spłaty kredytu";
        ui->tableWidget_cl_credits->setHorizontalHeaderLabels(naglowki);
        ui->tableWidget_cl_credits->setRowCount(l_konta.konta[index].kredyty.size());
        for(int y=0; y<l_konta.konta[index].kredyty.size();y++){
            for(int x=0;x<6;x++){
                switch(x){
                case 0:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_konta.konta[index].kredyty[y].getKwota())));
                    break;
                }
                case 1:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_konta.konta[index].kredyty[y].getProcent())));
                    break;
                }
                case 2:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_konta.konta[index].kredyty[y].getRata())));
                    break;
                }
                case 3:{
                    std::string data=std::to_string(l_konta.konta[index].kredyty[y].data.tm_mday);
                    data.append(".");
                    data.append(std::to_string(l_konta.konta[index].kredyty[y].data.tm_mon));
                    data.append(".");
                    data.append(std::to_string(l_konta.konta[index].kredyty[y].data.tm_year));
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(data)));
                    break;
                }
                case 4:{
                    std::string data=std::to_string(l_konta.konta[index].kredyty[y].data.tm_mday);
                    data.append(".");
                    data.append(std::to_string(l_konta.konta[index].kredyty[y].m_zaplacone));
                    data.append(".");
                    data.append(std::to_string(l_konta.konta[index].kredyty[y].r_zaplacone));
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(data)));
                    break;
                }
                case 5:{
                    std::string data=std::to_string(l_konta.konta[index].kredyty[y].data2.tm_mday);
                    data.append(".");
                    data.append(std::to_string(l_konta.konta[index].kredyty[y].data2.tm_mon));
                    data.append(".");
                    data.append(std::to_string(l_konta.konta[index].kredyty[y].data2.tm_year));
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(data)));
                    break;
                }
                }
            }
        }
         ui->stackedWidget_client->setCurrentIndex(5);
    }
    else if(tryb==3){ //wyswietlanie informacji o lokatach
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_b_acc_indx->setText(QString::number(index));
        ui->tableWidget_cl_credits->setColumnCount(4);
        QStringList naglowki;
        naglowki << "Kwota na lokacie" << "Oprocentowanie" << "Data otwarcia lokaty" << "Data zamkniecia lokaty";
        ui->tableWidget_cl_credits->setHorizontalHeaderLabels(naglowki);
        ui->tableWidget_cl_credits->setRowCount(l_konta.konta[index].lokaty.size());
        for(int y=0; y<l_konta.konta[index].lokaty.size();y++){
             for(int x=0;x<4;x++){
                 switch(x){
                 case 0:{
                     ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::number(l_konta.konta[index].lokaty[y].getKwota())));
                     break;
                 }
                 case 1:{
                     ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::number(l_konta.konta[index].lokaty[y].procent)));
                     break;
                 }
                 case 2:{
                     std::string data=std::to_string(l_konta.konta[index].lokaty[y].data.tm_mday);
                     data.append(".");
                     data.append(std::to_string(l_konta.konta[index].lokaty[y].data.tm_mon));
                     data.append(".");
                     data.append(std::to_string(l_konta.konta[index].lokaty[y].data.tm_year));
                     ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(data)));
                     break;
                 }
                 case 3:{
                     std::string data=std::to_string(l_konta.konta[index].lokaty[y].data2.tm_mday);
                     data.append(".");
                     data.append(std::to_string(l_konta.konta[index].lokaty[y].data2.tm_mon));
                     data.append(".");
                     data.append(std::to_string(l_konta.konta[index].lokaty[y].data2.tm_year));
                     ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(data)));
                     break;
                 }
                 }
             }
        }
        ui->stackedWidget_client->setCurrentIndex(5);
    }
    else if(tryb==4){ //wyswietlanie informacji o przelewach cyklicznych i ich anulowanie
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_b_acc_indx->setText(QString::number(index));

        ui->tableWidget_cl_credits->setColumnCount(5);
        QStringList naglowki;
        naglowki << "Kwota przelewu" << "Dzien miesiaca wykonywania" << "Nr odbiorcy" << "Tytul" << "Tresc";
        ui->tableWidget_cl_credits->setHorizontalHeaderLabels(naglowki);
        ui->tableWidget_cl_credits->setRowCount(l_konta.konta[index].przelewy.size());
        for(int y=0; y<l_konta.konta[index].przelewy.size();y++){
            for(int x=0;x<5;x++){
                switch(x){
                case 0:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::number(l_konta.konta[index].przelewy[y].kwota)));
                    break;
                }
                case 1:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::number(l_konta.konta[index].przelewy[y].cykliczny)));
                    break;
                }
                case 2:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_konta.konta[index].przelewy[y].nr)));
                    break;
                }
                case 3:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_konta.konta[index].przelewy[y].tytul)));
                    break;
                }
                case 4:{
                    ui->tableWidget_cl_credits->setItem(y,x,new QTableWidgetItem(QString::fromStdString(l_konta.konta[index].przelewy[y].tresc)));
                    break;
                }


                }
            }
        }
        ui->stackedWidget_client->setCurrentIndex(5);
    }
    else if(tryb==5){ //wyswietlic historie w liscie i jak klikne to ma calosc wyswietlac w polu tekstowym
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_b_acc_indx->setText(QString::number(index));

        ui->listWidget_tr_hist->clear();
        int j=0;
        std::vector<Historia> tmp=l_konta.konta[index].history;
        std::reverse(tmp.begin(),tmp.end());
        for(auto i=tmp.begin();i<tmp.end();i++){
            std::string tresc=std::to_string(j);
            tresc.append(". ");
            tresc.append(std::to_string(i->data.tm_mday));
            tresc.append(".");
            tresc.append(std::to_string(i->data.tm_mon));
            tresc.append(".");
            tresc.append(std::to_string(i->data.tm_year));
            tresc.append(" ");
            tresc.append(i->operacja);
            std::transform(tresc.begin(), tresc.end(), tresc.begin(), [](char ch) { return ch == '\n' ? ' ' : ch; });
            new QListWidgetItem(tr(tresc.c_str()), ui->listWidget_tr_hist);
            j++;
        }
        ui->stackedWidget_client->setCurrentIndex(6);
    }
    else if(tryb==6){ //wymiana walut
        QString numer= item->text();
        std::string nr = numer.toUtf8().constData();
        int index=l_konta.znajdzIndeks(nr);
        ui->label_b_acc_indx->setText(QString::number(index));

        QNetworkAccessManager mand;
        QNetworkReply *d = mand.get(QNetworkRequest(QUrl("http://api.nbp.pl/api/exchangerates/rates/c/usd/")));
        QEventLoop pobd;
        connect(d,SIGNAL(finished()),&pobd,SLOT(quit()));
        pobd.exec();
        QString u = d->readAll();
        std::string usd=u.toUtf8().constData();

        QNetworkAccessManager mane;
        QNetworkReply *eu = mane.get(QNetworkRequest(QUrl("http://api.nbp.pl/api/exchangerates/rates/c/eur/")));
        QEventLoop pobe;
        connect(eu,SIGNAL(finished()),&pobe,SLOT(quit()));
        pobe.exec();
        QString e = eu->readAll();
        std::string eur=e.toUtf8().constData();
        if(usd.empty() || eur.empty()){ //jesli nie udaa sie pobrac kursu to nie jest wyswietlana tabelka
            QMessageBox::warning(this, " ", "Brak połączenia z internetem.");
            return;
        }

        std::regex wz("[+-]?([0-9]*[.])?[0-9]+"); //sluzy do wyluskania samego kursu z pobranego formularza
        auto pusd = std::sregex_iterator(usd.begin(), usd.end(), wz);
        std::smatch susd;
        auto peur = std::sregex_iterator(eur.begin(), eur.end(), wz);
        std::smatch seur;
        ui->tableWidget_exchange->setColumnCount(3);
        QStringList naglowki;
        naglowki << "Waluta" << "Kupno" << "Sprzedaż";

        ui->tableWidget_exchange->setHorizontalHeaderLabels(naglowki);
        ui->tableWidget_exchange->setRowCount(2);
        int j=0;
        ui->tableWidget_exchange->setItem(0,0,new QTableWidgetItem("Dolar"));
        for (auto i = pusd; i != std::sregex_iterator(); i++) {
                susd = *i;
                std::string dkurs=susd.str();
                if(j==5 || j==6){
                    std::replace(dkurs.begin(), dkurs.end(), '.', ',');
                    ui->tableWidget_exchange->setItem(0,j%4,new QTableWidgetItem(QString::fromStdString(dkurs)));
                }
                j++;
        }
        j=0;
        ui->tableWidget_exchange->setItem(1,0,new QTableWidgetItem("Euro"));
        for (auto i = peur; i != std::sregex_iterator(); i++) {
                seur = *i;
                std::string ekurs=seur.str();
                if(j==5 || j==6){
                    std::replace(ekurs.begin(), ekurs.end(), '.', ',');
                    ui->tableWidget_exchange->setItem(1,j%4,new QTableWidgetItem(QString::fromStdString(ekurs)));
                }
                j++;
        }
        ui->stackedWidget_client->setCurrentIndex(7);
    }
}



//wyslanie przelewu/ustawienie cyklicznego
void MainWindow::on_pushButton_sub_trans_clicked()
{
    QString numer=ui->lineEdit_recip->text();
    QString tytul=ui->textEdit_title->toPlainText();
    QString tresc=ui->textEdit_text->toPlainText();
    QString kwota=ui->lineEdit_rec_mon->text();
    QString cykliczny=ui->lineEdit_cycle->text();
    std::string nr = numer.toUtf8().constData();
    std::string ty = tytul.toUtf8().constData();
    std::string tr = tresc.toUtf8().constData();
    std::string kw = kwota.toUtf8().constData();
    std::regex wz("^([0-9]+|[0-9]+\\,[0-9]{1,2})$"); //kwota przelewu moze byc liczba z max 2 miesjcami po przecinku
    if (std::regex_match(kw, wz)){

    }else{
        QMessageBox::warning(this, " ", "Wprowadź poprawną kwotę przelewu.");
        return;
    }
    int cyk=cykliczny.toInt();
    if(ui->lineEdit_recip->text().isEmpty() || ui->textEdit_title->document()->isEmpty() || ui->textEdit_text->document()->isEmpty() || ui->lineEdit_rec_mon->text().isEmpty()){
        QMessageBox::warning(this, " ", "Nie możesz posiadać pustych pól poza polem przelewu cyklicznego.");
        return;
    }
    if(ui->lineEdit_cycle->text().isEmpty()){
        int mozliwe=l_konta.konta[ui->label_b_acc_indx->text().toInt()].przelewMozliwy(kw);
        if(mozliwe==1){
            l_konta.przelewHistoria(kw,nr,l_konta.konta[ui->label_b_acc_indx->text().toInt()].numer,ty,tr);
            l_konta.wyslijPrzelew(kw,nr,ty,tr);
            QMessageBox::information(this, "", "Przelew wysłany.");
            ui->stackedWidget_client->setCurrentIndex(0);
        }
        else if(mozliwe==2){
            QMessageBox::warning(this, " ", "Kwota przelewu musi być większa od 0.");
        }
        else{
            QMessageBox::warning(this, " ", "Nie posiadasz na koncie wystarczającej ilościi środków.");
        }
    }
    else{
        if(cyk >=1 && cyk <=28){
            int index=ui->label_b_acc_indx->text().toInt();
            l_konta.konta[index].dodajPrzelew(kw,nr,ty,tr,cyk);
            QMessageBox::information(this, "", "Przelew cykliczny utworzony.");
            ui->stackedWidget_client->setCurrentIndex(0);
        }
        else{
            QMessageBox::warning(this, " ", "Wprowadź dzień z zakresu 1-28.");
        }
    }


}


//opcja kredyty admin
void MainWindow::on_pushButton_create_credit_clicked()
{
    int i=l_admini.znajdzIndeks(id);
    if(i!=-1){
        if(l_admini.admini[i].uprawnienia==1 || l_admini.admini[i].uprawnienia==4){
            ui->stackedWidget_admin->setCurrentIndex(16);
        }
        else{
            QMessageBox::warning(this, " ", "Brak uprawnień.");
        }
    }
    else{
        QMessageBox::warning(this, " ", "Nie istnieje w bazie?");
    }
}


//udziel kredytu
void MainWindow::on_pushButton_credit_sell_clicked()
{
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("5");
}


//wybieranie daty splaty kredytu
void MainWindow::on_calendarWidget_2_currentPageChanged(int year, int month)
{
    ui->label_cr_year->setText(QString::number(year));
    ui->label_cr_mon->setText(QString::number(month));
    ui->label_cr_day->setText(QString::number(21));
    int index=ui->label_acc_indx->text().toInt();
    std::string kw = ui->lineEdit_cr_money->text().toUtf8().constData();
    std::string per = ui->lineEdit_cr_per->text().toUtf8().constData();
    double rata=l_konta.konta[index].rataKredytu(kw, per, year, month);
    std::string cos=std::to_string(rata);
    std::replace(cos.begin(), cos.end(), '.', ',');
    ui->label_cr_rata->setText(QString::fromStdString(cos));
}


//zmiana na strone ustalania daty kredytu
void MainWindow::on_pushButton_cr_accept_clicked()
{
     if(ui->lineEdit_cr_money->text().isEmpty() || ui->lineEdit_cr_per->text().isEmpty()){
         QMessageBox::warning(this, " ", "Żadne pole nie może być puste.");
         return;
     }
     std::string wartosc=ui->lineEdit_cr_money->text().toUtf8().constData();
     std::replace(wartosc.begin(), wartosc.end(), ',', '.');
     double w=std::stod(wartosc);
     std::string wartoscw=ui->lineEdit_cr_per->text().toUtf8().constData();
     std::replace(wartoscw.begin(), wartoscw.end(), ',', '.');
     double ww=std::stod(wartoscw);
     if(w<=0 || ww<=0){
         QMessageBox::warning(this, " ", "Wartości muszą być większe od 0.");
         return;
     }
     ui->stackedWidget_admin->setCurrentIndex(18);
}


//zatwierdz udzielenie kredytu
void MainWindow::on_pushButton_cr_submit_clicked()
{
    QString year=ui->label_cr_year->text();
    QString mon=ui->label_cr_mon->text();
    QString day=ui->label_cr_day->text();
    std::string kw = ui->lineEdit_cr_money->text().toUtf8().constData();
    std::string per = ui->lineEdit_cr_per->text().toUtf8().constData();
    std::string rat = ui->label_cr_rata->text().toUtf8().constData();
    std::replace(rat.begin(), rat.end(), ',', '.');
    if(std::stod(rat)<=0){
        QMessageBox::warning(this, " ", "Wybierz poprawny miesiąc i rok.");
        return;
    }
    int index=ui->label_acc_indx->text().toInt();
    int success=l_konta.konta[index].dodajKredyt(kw, per, rat, year.toInt(), mon.toInt(), day.toInt());
    switch(success){
    case 0:
        QMessageBox::warning(this, " ", "Wybierz miesiąc i rok.");
        break;
    case 1:
        QMessageBox::information(this, "", "Kredyt został udzielony.");
        ui->stackedWidget_admin->setCurrentIndex(0);
        ui->label_cr_year->setText("1111");
        ui->label_cr_mon->setText("11");
        ui->label_cr_day->setText("11");
        ui->label_cr_rata->setText("0");
        break;
    }
}


//wyswietlanie kont klienta
void MainWindow::on_pushButton_acc_clicked()
{
    ui->listWidget_cl_acc->clear();
    ui->label_sel_acc_mode->setText("0");
    ui->stackedWidget_client->setCurrentIndex(2);
    int index=l_klienci.znajdzIndeks(id);
    for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
        new QListWidgetItem(tr((*i).c_str()), ui->listWidget_cl_acc);
    }
}


//wyswietlanie kredytow klienta
void MainWindow::on_pushButton_credit_clicked()
{
    ui->listWidget_cl_acc->clear();
    ui->label_sel_acc_mode->setText("2");
    ui->stackedWidget_client->setCurrentIndex(2);
    int index=l_klienci.znajdzIndeks(id);
    for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
        new QListWidgetItem(tr((*i).c_str()), ui->listWidget_cl_acc);
    }
}


//wyswietlanie lokat
void MainWindow::on_pushButton_loka_clicked()
{
    ui->listWidget_cl_acc->clear();
    ui->label_sel_acc_mode->setText("3");
    ui->stackedWidget_client->setCurrentIndex(2);
    int index=l_klienci.znajdzIndeks(id);
    for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
        new QListWidgetItem(tr((*i).c_str()), ui->listWidget_cl_acc);
    }
}


//wyswietlanie i usuwanie przelewow cyklicznych
void MainWindow::on_pushButton_cycl_trans_clicked()
{
    ui->listWidget_cl_acc->clear();
    ui->label_sel_acc_mode->setText("4");
    ui->stackedWidget_client->setCurrentIndex(2);
    int index=l_klienci.znajdzIndeks(id);
    for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
        new QListWidgetItem(tr((*i).c_str()), ui->listWidget_cl_acc);
    }
}


//usuwanie przelewu cyklicznego przez klienta
void MainWindow::on_tableWidget_cl_credits_cellDoubleClicked(int row, int column)
{
    QString operacja=ui->label_sel_acc_mode->text();
    int tryb=operacja.toInt();
    if(tryb==4){
        QString indx= ui->label_b_acc_indx->text();
        l_konta.konta[indx.toInt()].przelewy.erase(l_konta.konta[indx.toInt()].przelewy.begin()+row);
        QMessageBox::information(this, "", "Przelew usunięty.");
        ui->stackedWidget_client->setCurrentIndex(0);
    }
}


//przejdz do historii
void MainWindow::on_pushButton_history_clicked()
{
    ui->listWidget_cl_acc->clear();
    ui->label_sel_acc_mode->setText("5");
    ui->stackedWidget_client->setCurrentIndex(2);
    int index=l_klienci.znajdzIndeks(id);
    for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
        new QListWidgetItem(tr((*i).c_str()), ui->listWidget_cl_acc);
    }
}


//klikniecie na historie
void MainWindow::on_listWidget_tr_hist_itemClicked(QListWidgetItem *item)
{
    std::string tresc;
    QString nr=ui->label_b_acc_indx->text();
    int index_konta=nr.toInt();
    QString tekst=item->text();
    std::string txt = tekst.toUtf8().constData();
    int index_historia=txt.at(0)-'0';
    std::vector<Historia> tmp=l_konta.konta[index_konta].history;
    std::reverse(tmp.begin(),tmp.end());
    tresc.append(std::to_string(tmp[index_historia].data.tm_mday));
    tresc.append(".");
    tresc.append(std::to_string(tmp[index_historia].data.tm_mon));
    tresc.append(".");
    tresc.append(std::to_string(tmp[index_historia].data.tm_year));
    tresc.append("\n");
    tresc.append(tmp[index_historia].operacja);
    ui->textEdit_tr_hist->setText(QString::fromStdString(tresc));
}


//przedluzenie sesji
void MainWindow::on_pushButton_session_2_clicked()
{
    watek->stop=true;
    std::this_thread::sleep_for(std::chrono::milliseconds(1001));
    watek->stop=false;
    watek->start();
}


//pilnowanie czasu sesji
void MainWindow::on_zmiana_czas(int nr){
    if(nr==300){
        ui->label_session_adm->setText(QString::number(5));
        ui->label_session->setText(QString::number(5));
    }
    else if(nr<300 && nr>=240){
        ui->label_session_adm->setText(QString::number(4));
        ui->label_session->setText(QString::number(4));
    }
    else if(nr<240 && nr>=180){
        ui->label_session_adm->setText(QString::number(3));
        ui->label_session->setText(QString::number(3));
    }
    else if(nr<180 && nr>=120){
        ui->label_session_adm->setText(QString::number(2));
        ui->label_session->setText(QString::number(2));
    }
    else if(nr<120 && nr>=60){
        ui->label_session_adm->setText(QString::number(1));
        ui->label_session->setText(QString::number(1));
    }
    else if(nr<60){
        ui->label_session_adm->setText(QString::number(0));
        ui->label_session->setText(QString::number(0));
    }
    if(nr%60==9){
        ui->label_session_adm2->setText("09");
        ui->label_session2->setText("09");
    }
    else if(nr%60==8){
        ui->label_session_adm2->setText("08");
        ui->label_session2->setText("08");
    }
    else if(nr%60==7){
        ui->label_session_adm2->setText("07");
        ui->label_session2->setText("07");
    }
    else if(nr%60==6){
        ui->label_session_adm2->setText("06");
        ui->label_session2->setText("06");
    }
    else if(nr%60==5){
        ui->label_session_adm2->setText("05");
        ui->label_session2->setText("05");
    }
    else if(nr%60==4){
        ui->label_session_adm2->setText("04");
        ui->label_session2->setText("04");
    }
    else if(nr%60==3){
        ui->label_session_adm2->setText("03");
        ui->label_session2->setText("03");
    }
    else if(nr%60==2){
        ui->label_session_adm2->setText("02");
        ui->label_session2->setText("02");
    }
    else if(nr%60==1){
        ui->label_session_adm2->setText("01");
        ui->label_session2->setText("01");
    }
    else if(nr%60==0){
        ui->label_session_adm2->setText("00");
        ui->label_session2->setText("00");
    }
    else{
        ui->label_session_adm2->setText(QString::number(nr%60));
        ui->label_session2->setText(QString::number(nr%60));
    }
    if(nr==0){
        ui->stackedWidget_main->setCurrentIndex(0);
        ui->stackedWidget_admin->setCurrentIndex(0);
        ui->stackedWidget_client->setCurrentIndex(0);
    }
}


//przedluanie sesji client
void MainWindow::on_pushButton_session_clicked()
{
    watek->stop=true;
    std::this_thread::sleep_for(std::chrono::milliseconds(1001));
    watek->stop=false;
    watek->start();
}


//edytowanie klienta
void MainWindow::on_pushButton_edit_client_clicked()
{
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("6");
}


//zatwierdzenie edytowanie klienta
void MainWindow::on_pushButton_edit_cl_submit_clicked()
{
    if(ui->lineEdit_pes_edit_cl->text().isEmpty() || ui->lineEdit_pass_edit_cl->text().isEmpty() || ui->lineEdit_pass2_edit_cl->text().isEmpty()
            || ui->lineEdit__name_edit_cl->text().isEmpty() || ui->lineEdit_name2_edit_cl->text().isEmpty()){
        QMessageBox::information(this, "", "Wszystkie pola muszą być wypełnione.");
        return;
    }
    std::string pes= ui->lineEdit_pes_edit_cl->text().toUtf8().constData();
    std::string newpw= ui->lineEdit_pass_edit_cl->text().toUtf8().constData();
    std::string newpw2= ui->lineEdit_pass2_edit_cl->text().toUtf8().constData();
    std::string nam= ui->lineEdit__name_edit_cl->text().toUtf8().constData();
    std::string nam2= ui->lineEdit_name2_edit_cl->text().toUtf8().constData();
    std::string ind= ui->label_index_cl_editet->text().toUtf8().constData();
    int success=l_klienci.edytujKlienta(pes,newpw,newpw2,nam,nam2,ind);
    QString p=ui->label_pesel_cl_editing->text();
    switch(success){
    case 1:
        QMessageBox::information(this, "", "Użytkownik został zmieniony.");
        l_konta.zmienPesel(p.toUtf8().constData(),pes);
        ui->stackedWidget_admin->setCurrentIndex(0);
        break;
    case 2:
        QMessageBox::warning(this, " ", "Żadne z pól nie może zawierać spacji, użyj zamiast tego znaku '_'.");
        break;
    case 3:
        QMessageBox::warning(this, " ", "Wprowadzone hasła różnią się od siebie.");
        break;
    case 4:
        QMessageBox::warning(this, " ", "Użytkownik o takim loginie/peselu już istnieje.");
        ui->lineEdit_pes_edit_cl->setText(QString::fromStdString(l_klienci.klienci[stoi(ind)].getPesel()));
        break;
    case 5:
        QMessageBox::warning(this, " ", "Żadne z pól nie może zawierać znaku '\\'.");
        break;
    case 6:
        QMessageBox::warning(this, " ", "Pesel musi skladać się z 11 cyfr.");
        break;
    default:
        QMessageBox::warning(this, " ", "Nie udało się edytować użytkownika.");
        break;
    }
    ui->lineEdit_pass_edit_cl->clear();
    ui->lineEdit_pass2_edit_cl->clear();
}


//wyswietlenie pracownikow
void MainWindow::on_pushButton_workers_clicked()
{
    on_pushButton_edit_adm_clicked();
    ui->label_ad_ad_mode->setText("0");
}


//usuwanie klienta
void MainWindow::on_pushButton_delete_cl_clicked()
{
    QString p=ui->label_pesel_cl_editing->text();
    QString index=ui->label_index_cl_editet->text();
    l_klienci.klienci.erase(l_klienci.klienci.begin()+index.toInt());
    l_konta.usunKonto(p.toUtf8().constData());
    QMessageBox::information(this, "", "Użytkownik został usunięty.");
    ui->stackedWidget_admin->setCurrentIndex(0);
}


//usuwanie calego konta
void MainWindow::on_pushButton_delt_b_acc_clicked()
{
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("7");
}


//usuwanie lokaty
void MainWindow::on_pushButton_delete_lokata_clicked()
{
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("8");
}

//usuniecie lokaty klikajac
//label_edit_b_mode == 8  usuwanie lokaty klienta
//label_edit_b_mode == 9  usuwanie kredytu klienta
void MainWindow::on_listWidget_lok_itemDoubleClicked(QListWidgetItem *item)
{
    QString operacja=ui->label_edit_b_mode->text();
    int tryb=operacja.toInt();
    if(tryb==8){
        std::string tresc;
        QString nr=ui->label_b_acc_indx->text();
        int index_konta=nr.toInt();
        QString tekst=item->text();
        std::string txt = tekst.toUtf8().constData();
        int index_lokaty=txt.at(0)-'0';
        double ile=l_konta.konta[index_konta].lokaty[index_lokaty].getKwota();
        std::string kw= std::to_string(ile);
        std::string z="0";
        std::string z2="0";
        l_konta.konta[index_konta].lokaty.erase(l_konta.konta[index_konta].lokaty.begin()+index_lokaty);
        l_konta.konta[index_konta].editSaldo(kw,z,z2);
        QMessageBox::information(this, "", "Lokata usunięta.");
        ui->stackedWidget_admin->setCurrentIndex(0);
    }
    else if(tryb==9){
        std::string tresc;
        QString nr=ui->label_b_acc_indx->text();
        int index_konta=nr.toInt();
        QString tekst=item->text();
        std::string txt = tekst.toUtf8().constData();
        int index_kredytu=txt.at(0)-'0';
        double ile=-l_konta.konta[index_konta].kredyty[index_kredytu].ileDoSplaty();
        std::string kw= std::to_string(ile);
        std::string z="0";
        std::string z2="0";
        l_konta.konta[index_konta].kredyty.erase(l_konta.konta[index_konta].kredyty.begin()+index_kredytu);
        l_konta.konta[index_konta].editSaldo(kw,z,z2);
        QMessageBox::information(this, "", "Kredyt usunięty.");
        ui->stackedWidget_admin->setCurrentIndex(0);
    }

}


//usuniecie kredytu
void MainWindow::on_pushButton_credit_del_clicked()
{
    on_pushButton_account_adm_clicked();
    ui->label_filtr_mode->setText("9");
}


//wymiana walut
void MainWindow::on_pushButton_exchange_clicked()
{
    ui->listWidget_cl_acc->clear();
    ui->label_sel_acc_mode->setText("6");
    ui->stackedWidget_client->setCurrentIndex(2);
    int index=l_klienci.znajdzIndeks(id);
    for(auto i=l_klienci.klienci[index].konta.begin();i<l_klienci.klienci[index].konta.end();i++){
        new QListWidgetItem(tr((*i).c_str()), ui->listWidget_cl_acc);
    }
}


//wybor waluty do wymiany
void MainWindow::on_tableWidget_exchange_cellDoubleClicked(int row, int column)
{
     foreach(QLineEdit* le, ui->stackedWidget_admin->findChildren<QLineEdit*>()) {
         le->clear();
     }
     ui->lineEdit_cur_buy ->setValidator(new QDoubleValidator(this));
     ui->lineEdit_cur_sell  ->setValidator(new QDoubleValidator(this));
     QString waluta=ui->tableWidget_exchange->item(row,0)->text();
     QString kupno=ui->tableWidget_exchange->item(row,1)->text();
     QString sprzedaz=ui->tableWidget_exchange->item(row,2)->text();
     ui->label_currency->setText(waluta);
     ui->label_cur_buy->setText(kupno);
     ui->label_cur_sell->setText(sprzedaz);
     ui->stackedWidget_client->setCurrentIndex(8);
}


//podusmowanie wymiany waluty
void MainWindow::on_pushButton_cur_sum_clicked()
{
    std::regex wz("^([0-9]+|[0-9]+\\,[0-9]{1,2})$"); //kwota moze miec max 2 cyfry po przecinku
    if(ui->lineEdit_cur_buy->text().isEmpty() && ui->lineEdit_cur_sell->text().isEmpty()){
        QMessageBox::warning(this, " ", "Wprowdź kwotę.");
        return;
    }
    if(ui->lineEdit_cur_buy->text().isEmpty()){
        ui->lineEdit_cur_buy->setText("0");
    }
    if(ui->lineEdit_cur_sell->text().isEmpty()){
        ui->lineEdit_cur_sell->setText("0");
    }
    int mozliwe=1;
    int index_konta=ui->label_b_acc_indx->text().toInt();
    QString lcl=ui->label_cur_sell->text();
    std::string slcl=lcl.toUtf8().constData();
    std::replace(slcl.begin(), slcl.end(), ',', '.');
    QString lcb=ui->lineEdit_cur_buy->text();
    std::string slcb=lcb.toUtf8().constData();
    if (std::regex_match(slcb, wz)){

    }else{
        QMessageBox::warning(this, " ", "Wprowadź poprawną kwotę.");
        return;
    }
    std::replace(slcb.begin(), slcb.end(), ',', '.');
    double placisz=(std::stod(slcl)*std::stod(slcb));
    if(l_konta.konta[index_konta].getSaldo(0)<placisz){
        mozliwe=0;
    }
    std::string zaplata=std::to_string(placisz);
    std::replace(zaplata.begin(), zaplata.end(), '.', ',');
    ui->label_cur_pay->setText(QString::fromStdString(zaplata));
    ui->label_cur_count_b->setText(ui->lineEdit_cur_buy->text());
    if(ui->label_currency->text()=="Dolar"){
        ui->label_cur_buy_nom->setText("usd");
    }
    else if(ui->label_currency->text()=="Euro"){
        ui->label_cur_buy_nom->setText("eur");
    }
    QString lcl2=ui->lineEdit_cur_sell->text();
    std::string slcl2=lcl2.toUtf8().constData();
    if (std::regex_match(slcl2, wz)){

    }else{
        QMessageBox::warning(this, " ", "Wprowadź poprawną kwotę.");
        return;
    }
    std::replace(slcl2.begin(), slcl2.end(), ',', '.');
    QString lcb2=ui->label_cur_buy->text();
    std::string slcb2=lcb2.toUtf8().constData();
    std::replace(slcb2.begin(), slcb2.end(), ',', '.');
    double otrzymasz=(std::stod(slcl2)*std::stod(slcb2));
    if(std::stod(slcl2)==0 && std::stod(slcl)==0){
        QMessageBox::warning(this, " ", "Wprowadź poprawną kwotę.");
        return;
    }
    std::string otrzymane=std::to_string(otrzymasz);
    std::replace(otrzymane.begin(), otrzymane.end(), '.', ',');
    ui->label_cur_sell_2->setText(QString::fromStdString(otrzymane));
    ui->label_cur_count_s->setText(ui->lineEdit_cur_sell->text());
    if(ui->label_currency->text()=="Dolar"){
        ui->label_cur_sell_nom->setText("usd");
        if(std::stod(slcl2)>l_konta.konta[index_konta].getSaldo(2)){
            mozliwe=0;
        }
    }
    else if(ui->label_currency->text()=="Euro"){
        ui->label_cur_sell_nom->setText("eur");
        if(std::stod(slcl2)>l_konta.konta[index_konta].getSaldo(1)){
            mozliwe=0;
        }
    }
    if(mozliwe==1){
        ui->stackedWidget_client->setCurrentIndex(9);
    }
    else{
         QMessageBox::warning(this, " ", "Nie masz wystarczających środków na koncie.");
    }
}


//zatwierdzenie wymiony walut
void MainWindow::on_pushButton_cur_submit_clicked()
{
    int index_konta=ui->label_b_acc_indx->text().toInt();
    QString zl1=ui->label_cur_pay->text();
    QString zl2=ui->label_cur_sell_2->text();
    std::string zll1=zl1.toUtf8().constData();
    std::string zll2=zl2.toUtf8().constData();
    std::replace(zll1.begin(), zll1.end(), ',', '.');
    std::replace(zll2.begin(), zll2.end(), ',', '.');
    double kwotazl=std::stod(zll2)-std::stod(zll1);
    QString wal1=ui->label_cur_count_b->text();
    QString wal2=ui->label_cur_count_s->text();
    std::string wall1=wal1.toUtf8().constData();
    std::string wall2=wal2.toUtf8().constData();
    std::replace(wall1.begin(), wall1.end(), ',', '.');
    std::replace(wall2.begin(), wall2.end(), ',', '.');
    double kwotawal=std::stod(wall1)-std::stod(wall2);
    std::string zloty=std::to_string(kwotazl);
    std::string obca=std::to_string(kwotawal);
    std::string puste="0";
    if(ui->label_currency->text()=="Dolar"){
        l_konta.konta[index_konta].editSaldo(zloty,puste,obca);
    }
    else if(ui->label_currency->text()=="Euro"){
        l_konta.konta[index_konta].editSaldo(zloty,obca,puste);
    }
    QMessageBox::information(this, "", "Transakcja zakończona.");
    ui->stackedWidget_client->setCurrentIndex(0);
}
