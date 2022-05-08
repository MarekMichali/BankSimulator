#ifndef OSOBA_H
#define OSOBA_H

#include <iostream>

class Osoba{
protected:
   // int pesel;
    std::string pesel;

    std::string haslo;
public:
    std::string imie;
    std::string nazwisko;
    virtual std::string getPesel()=0;
    virtual void setPesel(const std::string&)=0;
    virtual std::string getHaslo()=0;
    virtual void setHaslo(const std::string&)=0;


};

#endif // OSOBA_H
