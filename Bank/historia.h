#ifndef HISTORIA_H
#define HISTORIA_H
#include "data.h"
#include <string>

class Historia: public Data {
public:
    std::string operacja;
    Historia();
    Historia(const std::string &);

};

#endif // HISTORIA_H
