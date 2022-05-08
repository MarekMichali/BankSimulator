#include "historia.h"

Historia::Historia(){

}

Historia::Historia(const std::string & tresc){
    time_t czas;
    czas = time(nullptr);
    tm date_now;
    localtime_s(&date_now,&czas);

    this->operacja=tresc;
    this->data.tm_year=date_now.tm_year+1900;
    this->data.tm_mon=date_now.tm_mon+1;
    this->data.tm_mday=date_now.tm_mday;
}
