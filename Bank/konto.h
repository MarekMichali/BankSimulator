#ifndef KONTO_H
#define KONTO_H
#include "lokata.h"
#include "kredyt.h"
#include "historia.h"
#include "przelew.h"
#include <vector>
#include <string>

class Konto {
    double saldozl;
    double saldoeu;
    double saldod;
public:
    std::string numer;
    std::string wlasciciel;
    std::vector<Lokata> lokaty;
    std::vector<Kredyt> kredyty;
    std::vector<Przelew> przelewy;
    std::vector<Historia> history;
    double getSaldo(int);
    void setSaldo(const double&, int);
    void editSaldo(std::string&, std::string&, std::string&);
    int dodajLokata(std::string&, std::string&, int, int, int);
    int przelewMozliwy(std::string &);
    double rataKredytu(std::string&, std::string&, int, int, int=21);
    int dodajKredyt(std::string&, std::string&, std::string&, int, int, int);
    int dodajPrzelew(std::string &, const std::string &, const std::string &, const std::string &, const int);
    Konto();
    Konto(double, const std::string&, const std::string&);


};

#endif // KONTO_H
