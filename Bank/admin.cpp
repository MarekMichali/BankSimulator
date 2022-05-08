#include "admin.h"
#include <fstream>
#include <string>
#include <regex>
#include <string>

Admin::Admin(){

}

Admin::Admin(const std::string& login, const std::string& pass, const std::string& name, const std::string& name2, const std::string& privilige){
    this->pesel=login;
    this->haslo=pass;
    this->imie=name;
    this->nazwisko=name2;
    this->uprawnienia=stoi(privilige);
}

std::string Admin::getPesel(){
    return this->pesel;
}

void Admin::setPesel(const std::string& x){
    this->pesel=x;
}

std::string Admin::getHaslo(){
    return this->haslo;
}

void Admin::setHaslo(const std::string& pass){
    this->haslo=pass;
}

int Admin::zmienHaslo(const std::string& oldpw, const std::string& newpw, const std::string& newpw2){
    std::regex wzorzec( "\\s+" );
     std::regex wz( "\\\\" );
    if(this->haslo!=oldpw){
        return 3;
    }
    if(this->haslo==oldpw && newpw!=newpw2){
        return 2;
    }
    if(std::regex_search(newpw, wzorzec)){
        return 0;
    }
    if(std::regex_search(newpw, wz)){
        return 4;
    }
    if(this->haslo==oldpw && newpw==newpw2){
        this->haslo=newpw;
        return 1;
    }
    else{
        return -1;
    }
}




