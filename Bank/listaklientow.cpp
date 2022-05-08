#include "listaklientow.h"
#include <regex>
#include <chrono>
#include <random>
#include <fstream>

int ListaKlientow::dodaj(const std::string& log, const std::string& pass, const std::string& pass2, const std::string& name, const std::string& name2){
    std::regex wzorzec( "\\s+" );
    std::regex wz( "\\\\" );
    std::regex pes( "^[0-9]{11}$" );
    if(std::regex_search(pass, wzorzec) || std::regex_search(log, wzorzec) || std::regex_search(name, wzorzec) || std::regex_search(name2, wzorzec)){
        return 2;
    }
    if(std::regex_search(pass, wz) || std::regex_search(log, wz) || std::regex_search(name, wz) || std::regex_search(name2, wz)){
        return 5;
    }
    if(std::regex_match(log, pes)==0){
        return 6;
    }
    if(pass!=pass2){
        return 3;
    }
    if(this->sprawdzCzyIstnieje(log)){
        return 4;
    }
    Klient tmp(log, pass, name, name2);
    this->klienci.push_back(tmp);
    return 1;
}


bool ListaKlientow::sprawdzCzyIstnieje(const std::string& id, int ignore){
    if(this->klienci.size()==0){
        return 0;
    }
    int index=0;
    for(auto i=this->klienci.begin(); i<this->klienci.end();i++){
        if(i->getPesel()==id && index!=ignore){
            return 1;
        }
        index++;
    }
    return 0;
}


void ListaKlientow::zapisz(){
    if(this->klienci.size()==0){
        return;
    }
    std::ofstream plik("klienci.txt");
    for(auto i=this->klienci.begin(); i<this->klienci.end();i++){
        plik << i->getPesel() << " " << i->getHaslo() << " " << i->imie << " " << i->nazwisko << " " << i->blokada << " " << i->nr_hasla;
        for(auto j=i->jednorazowe.begin(); j<i->jednorazowe.end();j++){
            plik << " " << *j;
        }
        plik << " " << i->ile_kont;

        for(auto j=i->konta.begin(); j<i->konta.end();j++){
            plik << " " << *j;
        }
        plik << std::endl;


    }
    plik.close();
}


void ListaKlientow::wczytaj(){
    std::ifstream plik("klienci.txt");
    if(plik){
        std::string dane;
        int i=0;
        int j=0;
        while (plik >> dane){
            if(i==0){
                Klient tmp;
                tmp.setPesel(dane);
                this->klienci.push_back(tmp);
                i++;
            }
            else if(i==1){
                this->klienci.back().setHaslo(dane);
                i++;
            }
            else if(i==2){
                this->klienci.back().imie=dane;
                i++;
            }
            else if (i==3){
                this->klienci.back().nazwisko=dane;
                i++;
            }
            else if(i==4){
                this->klienci.back().blokada=stoi(dane);
                i++;
            }
            else if(i==5){
                this->klienci.back().nr_hasla=stoi(dane);
                i++;
            }
            else if(i>5 && i<26){
                this->klienci.back().jednorazowe.push_back(stoi(dane));
                i++;
            }
            else if(i==26){
                this->klienci.back().ile_kont=stoi(dane);
                j=stoi(dane);
                i++;
            }
            else if(i==27){
                if(j==0){
                    Klient tmp;
                    tmp.setPesel(dane);
                    this->klienci.push_back(tmp);
                    i++;
                    i=1;
                }
                else{
                    this->klienci.back().konta.push_back((dane));
                    j--;
                }
            }
        }
        plik.close();
    }
    else{
        throw std::runtime_error("Nie udało się wczytać wszystkich danych");
    }

}


int ListaKlientow::logowanie(const std::string& log, const std::string& pass, std::string& id){
    if(this->klienci.size()==0){
        return 0;
    }
    for(auto i=this->klienci.begin(); i<this->klienci.end();i++){
        if(log==i->getPesel() && i->blokada==5){
            return 20;
        }
        if(log==i->getPesel() && pass!=i->getHaslo()){
            i->blokada++;
            return 30;
        }
        if(log==i->getPesel() && pass==i->getHaslo()){
            id=i->getPesel();
            i->blokada=0;
            return 10;
        }
    }
    return 0;
}


int ListaKlientow::znajdzIndeks(const std::string& id){
    if(this->klienci.size()==0){
        return -1;
    }
    int j=0;
    for(auto i=this->klienci.begin(); i<this->klienci.end();i++){
        if(i->getPesel()==id){
            return j;
        }
        j++;
    }
    return -1;
}


int ListaKlientow::znajdzBlokady(){
    int j=0;
    for(auto i=this->klienci.begin(); i<this->klienci.end();i++){
        if(i->blokada==5){
            j++;
        }
    }
    return j;
}


int ListaKlientow::edytujKlienta(const std::string& login, const std::string& newpw, const std::string& newpw2, const std::string& name, const std::string& name2, const std::string& index){
    std::regex wzorzec( "\\s+" );
    std::regex wz( "\\\\" );
    std::regex pes( "^[0-9]{11}$" );
    if(newpw!=newpw2){
        return 3;
    }
    if((std::regex_search(newpw, wzorzec) || std::regex_search(login, wzorzec) || std::regex_search(name, wzorzec) || std::regex_search(name2, wzorzec))){
        return 2;
    }
    if(std::regex_search(newpw, wz) || std::regex_search(login, wz) || std::regex_search(name, wz) || std::regex_search(name2, wz)){
        return 5;
    }
    if(std::regex_match(login, pes)==0){
        return 6;
    }
    if(this->sprawdzCzyIstnieje(login,stoi(index))){
        return 4;
    }
    if(newpw.size()!=0){
        this->klienci[stoi(index)].setHaslo(newpw);
    }
    this->klienci[stoi(index)].setPesel(login);
    this->klienci[stoi(index)].imie=name;
    this->klienci[stoi(index)].nazwisko=name2;
    return 1;
}

