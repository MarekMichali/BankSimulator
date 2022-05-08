#ifndef KLIENT_H
#define KLIENT_H
#include "osoba.h"
#include <vector>


class Klient: public Osoba {
public:
    int nr_hasla;
    int blokada;
    int ile_kont;
    std::vector<int> jednorazowe;
    std::vector<std::string> konta;

    virtual std::string getPesel();
    virtual void setPesel(const std::string&);
    virtual std::string getHaslo();
    virtual void setHaslo(const std::string&);
    int logTmp(const int&, const int&);
    void generujTmp();
    int zmienHaslo(const std::string&, const std::string&, const std::string&);
    int dodajKonto(const std::string&);
    Klient();
    Klient(const std::string&, const std::string&, const std::string&, const std::string&);
};

#endif // KLIENT_H
