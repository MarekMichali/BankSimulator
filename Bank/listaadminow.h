#ifndef LISTAADMINOW_H
#define LISTAADMINOW_H

#include "admin.h"
#include <vector>

class ListaAdminow {
public:
    std::vector<Admin> admini;
public:
    void wczytaj(); //wczytanie z pliku
    int logowanie(const std::string& ,const std::string&, std::string&); //logowanie na ekranie
    void zapisz(); //zapisanie do pliku
    int znajdzIndeks(const std::string&); //szukanie indeksu w wektorze na podstawie peselu
    int dodaj(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&); //dodawanie admina do bazy
    bool sprawdzCzyIstnieje(const std::string&, int ignore=-1); //sprawdzanie czy pesel jest unikalny
    int edytujAdmina(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&); // edycja admina w wektorze

};

#endif // LISTAADMINOW_H
