#include "kredyt.h"

Kredyt::Kredyt(){

}

Kredyt::Kredyt(const std::string & kw, const std::string & proc, const std::string & ra, int year, int mon, int day){
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);


    this->kwota=kw;
    this->procent=proc;
    this->rata=ra;
    this->data.tm_year=date_now.tm_year+1900;
    this->data.tm_mon=date_now.tm_mon+1;
    this->data.tm_mday=date_now.tm_mday;
    this->r_zaplacone=date_now.tm_year+1900;
    this->m_zaplacone=date_now.tm_mon+1;
    this->data2.tm_year=year;
    this->data2.tm_mon=mon;
    this->data2.tm_mday=day;
}

void Kredyt::setRata(const std::string & r){
    this->rata=r;
}

void Kredyt::setProc(const std::string & p){
    this->procent=p;
}
void Kredyt::setKwota(const std::string & k){
    this->kwota=k;
}

std::string Kredyt::getRata(){
    return this->rata;
}

std::string Kredyt::getKwota(){
    return this->kwota;
}

std::string Kredyt::getProcent(){
    return this->procent;
}


double Kredyt::ileDoSplaty(){
    int ile_miesiecy=((this->data2.tm_year*12)+this->data2.tm_mon) - (this->m_zaplacone+(this->r_zaplacone*12));
    double kwota=ile_miesiecy*stod(this->rata);
    return kwota;
}




