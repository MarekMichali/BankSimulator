#ifndef ADMIN_H
#define ADMIN_H
#include "osoba.h"
#include <vector>

class Admin: public Osoba {
public:
    int uprawnienia;

public:
    virtual std::string getPesel();
    virtual void setPesel(const std::string&);
    virtual std::string getHaslo();
    virtual void setHaslo(const std::string&);
    int zmienHaslo(const std::string&, const std::string&, const std::string&); //zmiana swojego hasla
    Admin(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);
    Admin();



};

#endif // ADMIN_H
