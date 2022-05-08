#ifndef LISTAKLIENTOW_H
#define LISTAKLIENTOW_H
#include "klient.h"
#include <vector>

class ListaKlientow {
public:
    std::vector<Klient> klienci;
    int dodaj(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);
    bool sprawdzCzyIstnieje(const std::string&, int ignore=-1);
    void zapisz(); //zapisanie do pliku
    void wczytaj(); //wczytanie z pliku
    int logowanie(const std::string& ,const std::string&, std::string&);
    int znajdzIndeks(const std::string&);
    int znajdzBlokady();
    int edytujKlienta(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);
};
#endif // LISTAKLIENTOW_H
