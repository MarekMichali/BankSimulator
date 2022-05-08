#include "sesja.h"
#include <QtCore>

sesja::sesja(QObject *parent): QThread(parent){

}

void sesja::run(){
    for(int i=300; i>=0; i--){
        QMutex mutex;
        mutex.lock();
        if(this->stop){
            break;
        }
        mutex.unlock();
        emit zmiana_czas(i);
        this->msleep(1000);
    }
}
