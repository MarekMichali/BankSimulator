#include "przelew.h"

Przelew::Przelew(){

}

Przelew::Przelew(double ile, const std::string& numer, const std::string& tekst, const std::string& title, int dzien){
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);
    this->cykliczny=dzien;
    this->kwota=ile;
    this->nr=numer;
    this->tresc=tekst;
    this->tytul=title;
    this->data.tm_year=date_now.tm_year+1900;
    this->data.tm_mon=date_now.tm_mon+1;
    this->data.tm_mday=date_now.tm_mday;
    this->data2.tm_year=date_now.tm_year+1900;
    this->data2.tm_mon=date_now.tm_mon+1;
    this->data2.tm_mday=date_now.tm_mday;
    if(date_now.tm_mday>dzien){
        this->zaplacone=1;
    }
    else{
        this->zaplacone=0;
    }
}





