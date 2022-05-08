#ifndef LOKATA_H
#define LOKATA_H
#include "data.h"
#include <string>

class Lokata: public Data {
    double kwota;
public:
    float procent;
    double getKwota();
    void setKwota(const double&);
    Lokata();
    Lokata(double, float, int, int, int);
};

#endif // LOKATA_H
