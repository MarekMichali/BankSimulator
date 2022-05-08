#include "lokata.h"

Lokata::Lokata(){

}

Lokata::Lokata(double bal, float per, int year, int mon, int day){
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);

    this->kwota=bal;
    this->procent=per;
    this->data.tm_year=date_now.tm_year+1900;
    this->data.tm_mon=date_now.tm_mon+1;
    this->data.tm_mday=date_now.tm_mday;
    this->data2.tm_year=year;
    this->data2.tm_mon=mon;
    this->data2.tm_mday=day;
}

double Lokata::getKwota(){
        return this->kwota;
}
void Lokata::setKwota(const double& kwota){
    this->kwota=kwota;
}


