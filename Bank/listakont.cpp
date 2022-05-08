#include "listakont.h"
#include <chrono>
#include <random>
#include <stdlib.h>
#include "konto.h"
#include <regex>
#include <fstream>
#include "kredyt.h"
#include <algorithm>




void ListaKont::generujNr(std::string& nr){
    std::default_random_engine los;
    los.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> rozklad(1000, 9999); //na potrzebny projektu tylko 4 znaki
    int x=rozklad(los);
    if(this->konta.size()==0){
        nr.append(std::to_string(x));
        return;

    }
    std::string tmp=nr;
    tmp.append(std::to_string(x));
    for(auto i=this->konta.begin(); i<this->konta.end();){
        if(i->numer==tmp){
            tmp=nr;
            x=rozklad(los);
            tmp.append(std::to_string(x));
            i=this->konta.begin();
        }
        else{
            i++;
        }
    }
    nr.append(std::to_string(x));
}


int ListaKont::dodaj(const std::string& nr, const std::string& pesel, const std::string& balance){
   std::string kopia=balance;
   std::replace(kopia.begin(), kopia.end(), ',', '.');
   double kwota=atof(kopia.c_str());
   Konto tmp(kwota,nr,pesel);
   this->konta.push_back(tmp);
   return 1;
}


void ListaKont::zapisz(){
    if(this->konta.size()==0){
        return;
    }
    std::ofstream plik("konta.txt");
    plik << std::fixed;
    for(auto i=this->konta.begin(); i<this->konta.end();i++){
        plik << i->numer << " " << i->wlasciciel << " " << i->getSaldo(0) << " " << i->getSaldo(1) << " " << i->getSaldo(2) << " lokaty";
        for(auto j=i->lokaty.begin(); j<i->lokaty.end();j++){
            plik << " " << j->getKwota() << " " << j->procent << " " << j->data.tm_year << " " << j->data.tm_mon << " " << j->data.tm_mday << " "
                 << j->data2.tm_year << " " << j->data2.tm_mon << " " << j->data2.tm_mday;
        }
        plik << " endlokaty";
        plik << " kredyty";
        for(auto j=i->kredyty.begin(); j<i->kredyty.end();j++){
           plik << " " << j->getRata() << " " << j->getProcent() << " " << j->getKwota() << " " << j->m_zaplacone << " " << j->r_zaplacone << " "
                << j->data.tm_year << " " << j->data.tm_mon << " " << j->data.tm_mday << " "
                << j->data2.tm_year << " " << j->data2.tm_mon << " " << j->data2.tm_mday;
        }
        plik << " endkredyty";
        plik << " przelewy";
        for(auto j=i->przelewy.begin(); j<i->przelewy.end();j++){
            std::transform(j->tytul.begin(), j->tytul.end(), j->tytul.begin(), [](char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r' ? '|' : ch; }); //biale znaki nalezy odpowiednio zapisac w pliku
            std::transform(j->tytul.begin(), j->tytul.end(), j->tytul.begin(), [](char ch) { return ch == '\n' ? '~' : ch; });


            std::transform(j->tresc.begin(), j->tresc.end(), j->tresc.begin(), [](char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r' ? '|' : ch; });
            std::transform(j->tresc.begin(), j->tresc.end(), j->tresc.begin(), [](char ch) { return ch == '\n' ? '~' : ch; });

            plik << " " << j->kwota << " " << j->nr << " " << j->cykliczny << " " << j->tytul << " " << j->tresc << " " << j->zaplacone << " "

                 << j->data.tm_year << " " << j->data.tm_mon << " " << j->data.tm_mday << " "
                 << j->data2.tm_year << " " << j->data2.tm_mon << " " << j->data2.tm_mday;
        }
        plik << " endprzelewy";
        plik << " historia";
        for(auto j=i->history.begin(); j<i->history.end();j++){
            std::transform(j->operacja.begin(), j->operacja.end(), j->operacja.begin(), [](char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r' ? '|' : ch; });
            std::transform(j->operacja.begin(), j->operacja.end(), j->operacja.begin(), [](char ch) { return ch == '\n' ? '~' : ch; });
            plik << " " << j->operacja << " "
                 << j->data.tm_year << " " << j->data.tm_mon << " " << j->data.tm_mday;
        }
        plik << " endhistoria";
        plik << std::endl;
    }
    plik.close();
}

void ListaKont::wczytaj(){
    std::ifstream plik("konta.txt");
    if(plik){
        std::string dane;
        int i=0;
        while (plik >> dane){
            if(i==0){
                Konto tmp;
                tmp.numer=dane;
                this->konta.push_back(tmp);
                i++;
            }
            else if(i==1){
                this->konta.back().wlasciciel=dane;
                i++;
            }
            else if(i==2){
                this->konta.back().setSaldo(atof(dane.c_str()),0);
                i++;;
            }
            else if(i==3){
                this->konta.back().setSaldo(atof(dane.c_str()),1);
                i++;;
            }
            else if(i==4){
                this->konta.back().setSaldo(atof(dane.c_str()),2);
                i++;
            }
            else if(i==5){
                if(dane=="lokaty"){
                    i++;
                }
            }
            else if(i==6){
                if(dane=="endlokaty"){
                    i=15;
                }
                else{
                    Lokata lok;
                    this->konta.back().lokaty.push_back(lok);
                    this->konta.back().lokaty.back().setKwota(atof(dane.c_str()));
                    i++;
                }
            }
            else if(i==7){
                this->konta.back().lokaty.back().procent=atof(dane.c_str());
                i++;
            }
            else if(i==8){
                this->konta.back().lokaty.back().data.tm_year=stoi(dane);
                i++;
            }
            else if(i==9){
                this->konta.back().lokaty.back().data.tm_mon=stoi(dane);
                i++;
            }
            else if(i==10){
                this->konta.back().lokaty.back().data.tm_mday=stoi(dane);
                i++;
            }
            else if(i==11){
                this->konta.back().lokaty.back().data2.tm_year=stoi(dane);
                i++;
            }
            else if(i==12){
                this->konta.back().lokaty.back().data2.tm_mon=stoi(dane);
                i++;
            }
            else if(i==13){
                this->konta.back().lokaty.back().data2.tm_mday=stoi(dane);
                i++;
            }
            else if(i==14){
                if(dane=="endlokaty"){
                    i++;
                }
                else{
                    Lokata lok;
                    this->konta.back().lokaty.push_back(lok);
                    this->konta.back().lokaty.back().setKwota(atof(dane.c_str()));
                    i=7;
                }

            }
            else if(i==15){
                if(dane=="kredyty"){
                    i++;
                }

            }
            else if(i==16){
                if(dane=="endkredyty"){
                    i=28;
                }
                else{
                    Kredyt kre;
                    this->konta.back().kredyty.push_back(kre);
                    this->konta.back().kredyty.back().setRata(dane);
                    i++;
                }
            }
            else if(i==17){
                this->konta.back().kredyty.back().setProc(dane);
                i++;
            }
            else if(i==18){
                this->konta.back().kredyty.back().setKwota(dane);
                i++;
            }
            else if(i==19){
                this->konta.back().kredyty.back().m_zaplacone=stoi(dane);
                i++;
            }
            else if(i==20){
                this->konta.back().kredyty.back().r_zaplacone=stoi(dane);
                i++;
            }
            else if(i==21){
                this->konta.back().kredyty.back().data.tm_year=stoi(dane);
                i++;
            }
            else if(i==22){
                this->konta.back().kredyty.back().data.tm_mon=stoi(dane);
                i++;
            }
            else if(i==23){
                this->konta.back().kredyty.back().data.tm_mday=stoi(dane);
                i++;
            }
            else if(i==24){
                this->konta.back().kredyty.back().data2.tm_year=stoi(dane);
                i++;
            }
            else if(i==25){
                this->konta.back().kredyty.back().data2.tm_mon=stoi(dane);
                i++;
            }
            else if(i==26){
                this->konta.back().kredyty.back().data2.tm_mday=stoi(dane);
                i++;
            }
            else if(i==27){
                if(dane=="endkredyty"){
                    i++;
                }
                else{
                    Kredyt kre;
                    this->konta.back().kredyty.push_back(kre);
                    this->konta.back().kredyty.back().setRata(dane);
                    i=17;
                }
            }
            else if(i==28){
                if(dane=="przelewy"){
                    i++;
                }
            }
            else if(i==29){
                if(dane=="endprzelewy"){
                    i=41;
                }
                else{
                    Przelew prze;
                    this->konta.back().przelewy.push_back(prze);
                    this->konta.back().przelewy.back().kwota=atof(dane.c_str());
                    i++;

                }
            }
            else if(i==30){
                this->konta.back().przelewy.back().nr=dane;
                i++;
            }
            else if(i==31){
                this->konta.back().przelewy.back().cykliczny=stoi(dane);
                i++;
            }
            else if(i==32){
                std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '|' ? ' ' : ch; }); //biale znaki musza zostac odpowiednio odczytane z pliku
                std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '~' ? '\n' : ch; });
                this->konta.back().przelewy.back().tytul=dane;
                i++;
            }
            else if(i==33){
                std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '|' ? ' ' : ch; });
                std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '~' ? '\n' : ch; });
                this->konta.back().przelewy.back().tresc=dane;
                i=100;
            }
            else if(i==100){
                this->konta.back().przelewy.back().zaplacone=stoi(dane);
                i=34;
            }
            else if(i==34){
                this->konta.back().przelewy.back().data.tm_year=stoi(dane);
                i++;
            }
            else if(i==35){
                this->konta.back().przelewy.back().data.tm_mon=stoi(dane);
                i++;
            }
            else if(i==36){
                this->konta.back().przelewy.back().data.tm_mday=stoi(dane);
                i++;
            }
            else if(i==37){
                this->konta.back().przelewy.back().data2.tm_year=stoi(dane);
                i++;
            }
            else if(i==38){
                this->konta.back().przelewy.back().data2.tm_mon=stoi(dane);
                i++;
            }
            else if(i==39){
                this->konta.back().przelewy.back().data2.tm_mday=stoi(dane);
                i++;
            }
            else if(i==40){
                if(dane=="endprzelewy"){
                    i++;
                }
                else{
                    Przelew prze;
                    this->konta.back().przelewy.push_back(prze);
                    this->konta.back().przelewy.back().kwota=atof(dane.c_str());
                    i=30;
                }
            }
            else if(i==41){
                 if(dane=="historia"){
                     i++;
                 }
            }
            else if(i==42){
                if(dane=="endhistoria"){
                    i=0;
                }
                else{
                    std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '|' ? ' ' : ch; });
                    std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '~' ? '\n' : ch; });
                    Historia his;
                    this->konta.back().history.push_back(his);
                    this->konta.back().history.back().operacja=dane;
                    i++;
                }
            }
            else if(i==43){
                this->konta.back().history.back().data.tm_year=stoi(dane);
                i++;
            }
            else if(i==44){
                this->konta.back().history.back().data.tm_mon=stoi(dane);
                i++;
            }
            else if(i==45){
                this->konta.back().history.back().data.tm_mday=stoi(dane);
                i++;
            }
            else if(i==46){
                if(dane=="endhistoria"){
                    i=0;
                }
                else{
                    std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '|' ? ' ' : ch; });
                    std::transform(dane.begin(), dane.end(), dane.begin(), [](char ch) { return ch == '~' ? '\n' : ch; });
                    Historia his;
                    this->konta.back().history.push_back(his);
                    this->konta.back().history.back().operacja=dane;
                    i=43;
                }
            }
        }
        plik.close();
    }
    else{
        throw std::runtime_error("Nie udało się wczytać wszystkich danych");
    }
}


int ListaKont::znajdzIndeks(const std::string& numer){
    if(this->konta.size()==0){
        return -1;
    }
    int j=0;
    for(auto i=this->konta.begin(); i<this->konta.end();i++){
        if(i->numer==numer){
            return j;
        }
        j++;
    }
    return -1;
}


void ListaKont::naliczLokaty(){
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);
    date_now.tm_year+=1900;
    date_now.tm_mon++;
    for(auto i=this->konta.begin(); i<this->konta.end(); i++){
        for(auto j=i->lokaty.begin(); j<i->lokaty.end();){
            if(j->data2.tm_year*370+j->data2.tm_mon*32+j->data2.tm_mday <= date_now.tm_year*370+date_now.tm_mon*32+date_now.tm_mday){ //sprawdze jest czy juz minela data akonczenia lokaty
                double ile=j->getKwota()*(1+(j->procent/100));
                i->setSaldo(i->getSaldo(0)+ile,0);
                std::string cos="Lokata przychód: ";
                cos.append(std::to_string(ile));
                cos.append("zl");
                Historia tmp(cos);
                i->history.push_back(tmp);
                i->lokaty.erase(j);
                j=i->lokaty.begin();
                if(j==i->lokaty.end()){
                    break;
                }
            }
            else{
                j++;
            }
        }
    }
}


//tylko wysyla pieniadze, inna metoda odbiera i sprawdza
int ListaKont::wyslijPrzelew(std::string & kwota, const std::string & nr, const std::string & tytul, const std::string & tekst){
    int index=this->znajdzIndeks(nr);
    if(index==-1){
        return 0;
    }
    else{
        std::replace(kwota.begin(), kwota.end(), ',', '.');
        double ile=atof(kwota.c_str());
        this->konta[index].setSaldo(this->konta[index].getSaldo(0)+ile,0);
        return 1;
    }
}

void ListaKont::przelewHistoria(std::string & kwota, const std::string & nr, const std::string & nr_nad, const std::string & tytul, const std::string & tekst){
    int index=this->znajdzIndeks(nr);
    int index2=this->znajdzIndeks(nr_nad);
    if(index!=-1){
        std::string tresc="Przelew otrzymany: ";
        tresc.append(kwota);
        tresc.append("zl Nadawca: ");
        tresc.append(nr_nad);
        tresc.append(" Tytul: ");
        tresc.append(tytul);
        tresc.append(" Tresc: ");
        tresc.append(tekst);
        Historia tmp(tresc);
        this->konta[index].history.push_back(tmp);
    }
    std::string tresc2="Przelew wyslany: ";
    tresc2.append(kwota);
    tresc2.append("zl Adresat: ");
    tresc2.append(nr);
    tresc2.append(" Tytul: ");
    tresc2.append(tytul);
    tresc2.append(" Tresc: ");
    tresc2.append(tekst);
    Historia tmp2(tresc2);
    this->konta[index2].history.push_back(tmp2);
}


void ListaKont::naliczRaty(){
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);
    date_now.tm_year+=1900;
    date_now.tm_mon++;
    for(auto i=this->konta.begin(); i<this->konta.end(); i++){
        for(auto j=i->kredyty.begin(); j<i->kredyty.end();){
            if(j->r_zaplacone*12+j->m_zaplacone >= j->data2.tm_year*12+j->data2.tm_mon){ //ta sytuacja nigdy nie ma miejsca w programie, do uzyskaia jesr tylko poprzez reczne manipulowanie w bazie
                 // znaczy ze kredyt splacony i nalkezy usunac go
                i->kredyty.erase(j);
                j=i->kredyty.begin();
                if(j==i->kredyty.end()){
                    break;
                }
            }
            while(j->r_zaplacone*12+j->m_zaplacone < date_now.tm_year*12+date_now.tm_mon){
                if(j->r_zaplacone*12+j->m_zaplacone >= j->data2.tm_year*12+j->data2.tm_mon){
                    break; // znaczy ze kredyt splacony i nalkezy usunac go w warunku poza petla
                }
                else{
                    //trzeba jeszcze sprawdzicz czy jest juz za dniem placenia raty
                    if((j->r_zaplacone*12+j->m_zaplacone) - (date_now.tm_year*12+date_now.tm_mon)== -1){
                        //jestemy akurat w miesiacu kiedy placi sie rate i trzeba sprawdzic czy juz jest ten dzien
                        if(j->data2.tm_mday <= date_now.tm_mday){
                            //jest dzien 21 lub pozniejszy
                            i->setSaldo(i->getSaldo(0)-atof(j->getRata().c_str()),0);

                            std::string tresc="Splacono rate kredytu: -";
                            tresc.append(j->getRata().c_str());
                            Historia tmp(tresc);
                            i->history.push_back(tmp);

                            if(j->m_zaplacone==12){
                                j->m_zaplacone=1;
                                j->r_zaplacone++;
                            }
                            else{
                                j->m_zaplacone++;
                            }
                        }
                        else{ //jestesmy w miesiacu placenia raty i na biezaco, ale jest wczesniejszy dzien miesiac niz 21 dlatego nie trzeba splacic raty
                            break;
                        }
                    }
                    //jestemy wiecej niz miesiac do tylu z ratami
                    else{
                        i->setSaldo(i->getSaldo(0)-atof(j->getRata().c_str()),0);

                        std::string tresc="Splacono rate kredytu: -";
                        tresc.append(j->getRata().c_str());
                        Historia tmp(tresc);
                        i->history.push_back(tmp);

                        if(j->m_zaplacone==12){
                            j->m_zaplacone=1;
                            j->r_zaplacone++;
                        }
                        else{
                            j->m_zaplacone++;
                        }
                    }

                }
            }
            if(j->r_zaplacone*12+j->m_zaplacone >= j->data2.tm_year*12+j->data2.tm_mon){
                 // znaczy ze kredyt splacony i nalkezy usunac to naliczanie
                i->kredyty.erase(j);
                j=i->kredyty.begin();
                if(j==i->kredyty.end()){
                    break;
                }
            }
            else{
                j++;
            }
        }
    }
}


void ListaKont::przelewCykl(){
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);
    date_now.tm_year+=1900;
    date_now.tm_mon++;
    for(auto i=this->konta.begin(); i<this->konta.end(); i++){
        for(auto j=i->przelewy.begin(); j<i->przelewy.end();j++){
            //miesiac kiedy utworzono zlecenie cykliczne, czy nalezy jeszcze wykonac w tym miesiacu przelew
           if(j->data2.tm_year*12+j->data2.tm_mon == date_now.tm_year*12+date_now.tm_mon){
                if(j->zaplacone==1){
                    continue;
                }
                else{
                    if(date_now.tm_mday>=j->cykliczny){
                        std::string ile=std::to_string(j->kwota);
                        if(i->przelewMozliwy(ile)==1){
                            this->przelewHistoria(ile,j->nr,i->numer,j->tytul,j->tresc); //test
                            this->wyslijPrzelew(ile,j->nr,j->tytul,j->tresc);
                        }
                        j->data2.tm_mday=date_now.tm_mday;
                        j->zaplacone=1;
                    }
                    else{
                        continue;
                    }
                }
           }
           if(j->data2.tm_year*12+j->data2.tm_mon < date_now.tm_year*12+date_now.tm_mon){
               if(j->zaplacone==0){
                   std::string ile=std::to_string(j->kwota);
                   if(i->przelewMozliwy(ile)==1){
                       this->przelewHistoria(ile,j->nr,i->numer,j->tytul,j->tresc);//test
                       this->wyslijPrzelew(ile,j->nr,j->tytul,j->tresc);
                       j->zaplacone=1;
                   }
               }
           }
           while(j->data2.tm_year*12+j->data2.tm_mon < date_now.tm_year*12+date_now.tm_mon){
               //ten if jest poniewaz jesli byly miesiace do przodu a miesiac kiedy zrobiono zlecenie nie zaplacono a nalezalo zaplacic to tu sie robi

               //tu powyzej dorobilem
               j->zaplacone=0;
               if(j->data2.tm_mon==12){
                   j->data2.tm_mon=1;
                   j->data2.tm_year++;
               }
               else{
                   j->data2.tm_mon++;
               }
               if(j->data2.tm_year*12+j->data2.tm_mon == date_now.tm_year*12+date_now.tm_mon){
                   if(date_now.tm_mday>=j->cykliczny){
                       std::string ile=std::to_string(j->kwota);
                       if(i->przelewMozliwy(ile)==1){
                           this->przelewHistoria(ile,j->nr,i->numer,j->tytul,j->tresc);//test
                           this->wyslijPrzelew(ile,j->nr,j->tytul,j->tresc);
                       }
                       j->data2.tm_mday=date_now.tm_mday;
                       j->zaplacone=1;
                   }
                   else{
                       break;
                   }
               }
               else if(j->data2.tm_year*12+j->data2.tm_mon < date_now.tm_year*12+date_now.tm_mon){
                   std::string ile=std::to_string(j->kwota);
                   if(i->przelewMozliwy(ile)==1){
                       this->przelewHistoria(ile,j->nr,i->numer,j->tytul,j->tresc); //test
                       this->wyslijPrzelew(ile,j->nr,j->tytul,j->tresc);
                   }
                   j->data2.tm_mday=date_now.tm_mday;
                   j->zaplacone=0;
               }
           }
        }
    }
}

void ListaKont::zmienPesel(const std::string & nr, const std::string & new_nr){
    if(nr==new_nr){
        return;
    }
    for(auto i=this->konta.begin(); i<this->konta.end();i++){
        if(i->wlasciciel==nr){
            i->wlasciciel=new_nr;
        }
    }
}

void ListaKont::usunKonto(const std::string & wl){
    for(auto i=this->konta.begin(); i<this->konta.end();){
        if(i->wlasciciel==wl){
            this->konta.erase(i);
            i=this->konta.begin();
            if(i==this->konta.end()){
                break;
            }
        }
        else{
            i++;
        }
    }
}


