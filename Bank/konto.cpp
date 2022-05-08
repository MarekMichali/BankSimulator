#include "konto.h"
#include <algorithm>

Konto::Konto(){

}

Konto::Konto(double kwota, const std::string& nr, const std::string& pesel){
    this->saldozl=kwota;
    this->saldoeu=0;
    this->saldod=0;
    this->numer=nr;
    this->wlasciciel=pesel;
}

double Konto::getSaldo(int waluta){
    switch(waluta){
    case 0:
        return this->saldozl;
    case 1:
        return this->saldoeu;
    case 2:
        return this->saldod;
    default:
        return this->saldozl;
    }
}

void Konto::setSaldo(const double& kwota, int waluta){
    switch(waluta){
    case 0:
        this->saldozl=kwota;
        break;
    case 1:
        this->saldoeu=kwota;
        break;
    case 2:
        this->saldod=kwota;
        break;
    default:
        this->saldozl=kwota;
        break;
    }
}

void Konto::editSaldo(std::string& zl, std::string& eu, std::string& d){
    //liczby sa z przecinkiem dlatego trzeba zamienic na kropki, aby program obsluzyl
    std::replace(zl.begin(), zl.end(), ',', '.');
    double kwotazl= this->getSaldo(0)+atof(zl.c_str());
    std::replace(eu.begin(), eu.end(), ',', '.');
    double kwotaeu= this->getSaldo(1)+atof(eu.c_str());
    std::replace(d.begin(), d.end(), ',', '.');
    double kwotad= this->getSaldo(2)+atof(d.c_str());
    this->setSaldo(kwotazl,0);
    this->setSaldo(kwotaeu,1);
    this->setSaldo(kwotad,2);
    if(zl!="0"){
        std::string co="Zmiana stanu konta: ";
        co.append(std::to_string(atof(zl.c_str())));
        co.append("zl");
        Historia tmp2(co);
        this->history.push_back(tmp2);
    }
    if(eu!="0"){
        std::string co="Zmiana stanu konta: ";
        co.append(std::to_string(atof(eu.c_str())));
        co.append("eur");
        Historia tmp2(co);
        this->history.push_back(tmp2);
    }
    if(d!="0"){
        std::string co="Zmiana stanu konta: ";
        co.append(std::to_string(atof(d.c_str())));
        co.append("usd");
        Historia tmp2(co);
        this->history.push_back(tmp2);
    }

}


int Konto::dodajLokata(std::string& bal, std::string& per, int year, int mon, int day){
    std::replace(bal.begin(), bal.end(), ',', '.');
    std::replace(per.begin(), per.end(), ',', '.');
    double kwota=atof(bal.c_str());
    float procent=atof(per.c_str());
    if(kwota<=0 || procent<=0){
        return 4;
    }
    if(this->getSaldo(0)<kwota){
        return 2;
    }
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);
    if(370*year+32*mon+day<=370*(date_now.tm_year+1900)+32*(date_now.tm_mon+1)+date_now.tm_mday){
        return 3;   //data przeszla
    }
    Lokata tmp(kwota,procent,year,mon,day);
    this->setSaldo(this->getSaldo(0)-kwota,0);
    this->lokaty.push_back(tmp);

    std::string cos="Lokata zalozono: -";
    cos.append(std::to_string(kwota));
    cos.append("zl");
    Historia tmp2(cos);
    this->history.push_back(tmp2);

    return 1;
}


int Konto::przelewMozliwy(std::string & kwota){
     std::replace(kwota.begin(), kwota.end(), ',', '.');
     double ile=atof(kwota.c_str());
     if(ile<=0){
         return 2;
     }
     if(this->getSaldo(0)>ile){
         this->setSaldo(this->getSaldo(0)-ile,0);
         return 1;
     }
     else{
         return 0;
     }
}


double Konto::rataKredytu(std::string& kwota, std::string& procent, int rok, int miesiac, int dzien){
    std::replace(kwota.begin(), kwota.end(), ',', '.');
    double ile=atof(kwota.c_str());
    std::replace(procent.begin(), procent.end(), ',', '.');
    double pr=atof(procent.c_str());
    if(ile<=0){ //kredyt ujemny
        return -1;
    }
    if(pr<=0){ //procent ujemny
        return -2;
    }
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);
    int lata=rok-(date_now.tm_year+1900);
    int miesiace=miesiac-(date_now.tm_mon+1)+lata*12;
    if(miesiace<=0){
        return -3;
    }
    double rata=(ile*(1+pr/100))/miesiace;
    if(rata<0){
        return -4;
    }
    return rata;
}


int Konto::dodajKredyt(std::string& kwota, std::string& procent, std::string& rata, int year, int mon, int day){
    if(year==1111){
        return 0;
    }
    std::replace(kwota.begin(), kwota.end(), ',', '.');
    std::replace(procent.begin(), procent.end(), ',', '.');
    std::replace(rata.begin(), rata.end(), ',', '.');
    Kredyt tmp(kwota, procent, rata, year, mon, day);
    this->kredyty.push_back(tmp);

    std::string cos="Kredyt udzielono: ";
    cos.append(kwota);
    cos.append("zl");
    Historia tmp2(cos);
    this->history.push_back(tmp2);
    std::replace(kwota.begin(), kwota.end(), ',', '.');
    double ile=atof(kwota.c_str());
    this->setSaldo(this->getSaldo(0)+ile,0);
    return 1;
}


int Konto::dodajPrzelew(std::string & kwota, const std::string & nr, const std::string & tytul, const std::string & tekst, int dzien){
    double ile=atof(kwota.c_str());
    Przelew tmp(ile, nr, tekst, tytul, dzien);
    this->przelewy.push_back(tmp);
    return 1;
}

