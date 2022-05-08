#include "klient.h"
#include <chrono>
#include <random>
#include <fstream>
#include <regex>

Klient::Klient(){

}

Klient::Klient(const std::string& log, const std::string& pass, const std::string& name, const std::string& name2){
    std::default_random_engine los;
    los.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> rozklad(1000, 9999); //na potrzebny projektu tylko 4 znaki
    std::ofstream plik(log+".txt");
    plik << "Lista hasel jednorazowych: " << std::endl;
    for(int i=0;i<20;i++){
        this->jednorazowe.push_back(rozklad(los));
        plik << i+1 << ": " << this->jednorazowe.back() << std::endl;
    }
    plik.close();
    this->pesel=log;
    this->haslo=pass;
    this->imie=name;
    this->nazwisko=name2;
    this->nr_hasla=1;
    this->blokada=0;
    this->ile_kont=0;
}

std::string Klient::getPesel(){
    return this->pesel;
}

void Klient::setPesel(const std::string& x){
    this->pesel=x;
}

std::string Klient::getHaslo(){
    return this->haslo;
}

void Klient::setHaslo(const std::string& pass){
    this->haslo=pass;
}


int Klient::logTmp(const int& pass, const int& nr){
    if(this->blokada==5){
        return 3;
    }
    if(pass==this->jednorazowe[nr-1] && this->blokada==5){
        return 2;
    }
    else if(pass==this->jednorazowe[nr-1] && this->blokada!=5){
        this->blokada=0;
        this->nr_hasla++;
        return 1;
    }
    else{
        this->blokada++;
        return 0;
    }
}


void Klient::generujTmp(){
    std::default_random_engine los;
    los.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> rozklad(1000, 9999); //na potrzebny projektu tylko 4 znaki
    this->jednorazowe.clear();
    std::ofstream plik(this->getPesel()+".txt");
    plik << "Lista hasel jednorazowych: " << std::endl;
    for(int i=0;i<20;i++){
        this->jednorazowe.push_back(rozklad(los));
        plik << i+1 << ": " << this->jednorazowe.back() << std::endl;
    }
    this->nr_hasla=1;
}


int Klient::zmienHaslo(const std::string& oldpw, const std::string& newpw, const std::string& newpw2){
    std::regex wzorzec( "\\s+" );
    std::regex wz( "\\\\" );
    if(this->haslo!=oldpw){
        return 3;
    }
    if(this->haslo==oldpw && newpw!=newpw2){
        return 2;
    }
    if(std::regex_search(newpw, wzorzec)){
        return 0;
    }
    if(std::regex_search(newpw, wz)){
        return 4;
    }
    if(this->haslo==oldpw && newpw==newpw2){
        this->haslo=newpw;
        return 1;
    }
    else{
        return -1;
    }
}

int Klient::dodajKonto(const std::string& nr){
    this->konta.push_back(nr);
    this->ile_kont++;
    return 1;
}




