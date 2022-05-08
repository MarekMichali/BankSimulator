#include "listaadminow.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>


void ListaAdminow::wczytaj(){
    std::ifstream plik("admini.txt");
    if(plik){
        std::string dane;
        int i=0;
        while (plik >> dane){
            if(i==0){
                Admin tmp;
                tmp.setPesel(dane);
                this->admini.push_back(tmp);
                i++;
            }
            else if(i==1){
                this->admini.back().setHaslo(dane);
                i++;
            }
            else if(i==2){
                this->admini.back().imie=dane;
                i++;
            }
            else if (i==3){
                this->admini.back().nazwisko=dane;
                i++;
            }
            else{
                this->admini.back().uprawnienia=stoi(dane);
                i=0;
            }
        }
        plik.close();
    }
    else{
        throw std::runtime_error("Nie udało się wczytać wszystkich danych");
    }

}


int ListaAdminow::logowanie(const std::string& log, const std::string& pass, std::string& id){
    if(this->admini.size()==0){
        return 0;
    }
    for(auto i=this->admini.begin(); i<this->admini.end();i++){
        if(log==i->getPesel() && pass==i->getHaslo()){
            id=i->getPesel();
            return 1;
        }
    }
    return 0;
}


void ListaAdminow::zapisz(){
    if(this->admini.size()==0){
        return;
    }
    std::ofstream plik("admini.txt");
    for(auto i=this->admini.begin(); i<this->admini.end();i++){
        plik << i->getPesel() << " " << i->getHaslo() << " " << i->imie << " " << i->nazwisko << " " << i->uprawnienia << std::endl;
    }
    plik.close();
}


int ListaAdminow::znajdzIndeks(const std::string& id){
    if(this->admini.size()==0){
        return -1;
    }
    int j=0;
    for(auto i=this->admini.begin(); i<this->admini.end();i++){
        if(i->getPesel()==id){
            return j;
        }
        j++;
    }
    return -1;
}


int ListaAdminow::dodaj(const std::string& log, const std::string& pass, const std::string& pass2, const std::string& name, const std::string& name2, const std::string& privilige){
     std::regex wzorzec( "\\s+" );
     std::regex wz( "\\\\" );
     std::regex pes( "^[0-9]{11}$" );
     if(std::regex_search(pass, wzorzec) || std::regex_search(log, wzorzec) || std::regex_search(name, wzorzec) || std::regex_search(name2, wzorzec) || std::regex_search(privilige, wzorzec)){
         return 2;
     }
     if(std::regex_search(pass, wz) || std::regex_search(log, wz) || std::regex_search(name, wz) || std::regex_search(name2, wz) || std::regex_search(privilige, wz)){
         return 5;
     }
     if(std::regex_match(log, pes)==0){
         return 6;
     }
     if(pass!=pass2){
         return 3;
     }
     std::string login=log;
     login.append(privilige);
     if(this->sprawdzCzyIstnieje(login)){
         return 4;
     }
    Admin tmp(login,pass,name,name2,privilige);
    this->admini.push_back(tmp);
    return 1;
}


bool ListaAdminow::sprawdzCzyIstnieje(const std::string& id, int ignore){
    if(this->admini.size()==0){
        return 0;
    }
    int index=0;
    for(auto i=this->admini.begin(); i<this->admini.end();i++){
        if(i->getPesel()==id && index!=ignore){
            return 1;
        }
        index++;
    }
    return 0;
}


int ListaAdminow::edytujAdmina(const std::string& login, const std::string& newpw, const std::string& newpw2, const std::string& name, const std::string& name2, const std::string& priv, const std::string& index){
    std::regex wzorzec( "\\s+" );
    std::regex wz( "\\\\" );
    std::regex pes( "^[0-9]{11}$" );
    if(newpw!=newpw2){
        return 3;
    }
    if(std::regex_search(newpw, wzorzec)){
        return 2;
    }
    if(std::regex_search(newpw, wz) || std::regex_search(login, wz) || std::regex_search(name, wz) || std::regex_search(name2, wz)){
        return 5;
    }
    if(std::regex_match(login, pes)==0){
        return 6;
    }
    std::string log=login;
    log.append(priv);
    if(this->sprawdzCzyIstnieje(log,stoi(index))){
        return 4;
    }
    if(newpw.size()!=0){
        this->admini[stoi(index)].setHaslo(newpw);
    }
    this->admini[stoi(index)].setPesel(log);
    this->admini[stoi(index)].imie=name;
    this->admini[stoi(index)].nazwisko=name2;
    this->admini[stoi(index)].uprawnienia=stoi(priv);
    return 1;

}

