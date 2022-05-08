#ifndef LISTAKONT_H
#define LISTAKONT_H
#include "konto.h"
#include <vector>

class ListaKont {
public:
    std::vector<Konto> konta;
    int dodaj(const std::string&, const std::string&, const std::string&);
    void generujNr(std::string&);
    void zapisz();
    void wczytaj();
    int znajdzIndeks(const std::string&);
    void naliczLokaty();
    int wyslijPrzelew(std::string &, const std::string &, const std::string &, const std::string &);
    void naliczRaty();
    void przelewCykl();
    void przelewHistoria(std::string &, const std::string &, const std::string &, const std::string &, const std::string &);
    void zmienPesel(const std::string &, const std::string &);
    void usunKonto(const std::string &);
};

#endif // LISTAKONT_H
