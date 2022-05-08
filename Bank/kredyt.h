#ifndef KREDYT_H
#define KREDYT_H
#include "data.h"
#include <string>

class Kredyt: public Data{
private:
    std::string rata;
    std::string kwota;
    std::string procent;
public:
    int r_zaplacone;
    int m_zaplacone;
    void setRata(const std::string &);
    void setKwota(const std::string &);
    void setProc(const std::string &);
    std::string getRata();
    std::string getKwota();
    std::string getProcent();
    double ileDoSplaty();
    Kredyt();
    Kredyt(const std::string &, const std::string &, const std::string &, int, int, int);

};

#endif // KREDYT_H
