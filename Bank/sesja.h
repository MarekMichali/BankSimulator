#ifndef SESJA_H
#define SESJA_H

#include <QThread>

class sesja : public QThread
{
    Q_OBJECT
public:
    explicit sesja(QObject *parent=0);
    void run();
    bool stop=false;
signals:
    void zmiana_czas(int);
};

#endif // SESJA_H
