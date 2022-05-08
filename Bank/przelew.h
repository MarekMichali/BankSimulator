#ifndef PRZELEW_H
#define PRZELEW_H
#include "data.h"
#include <string>

class Przelew: public Data {
public:
    double kwota;
    std::string nr;
    std::string tresc;
    std::string tytul;
    int cykliczny;
    int zaplacone;
    Przelew();
    Przelew(double, const std::string&, const std::string&, const std::string&, int);
};

#endif // PRZELEW_H
