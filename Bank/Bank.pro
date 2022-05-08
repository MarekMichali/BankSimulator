QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    admin.cpp \
    data.cpp \
    historia.cpp \
    klient.cpp \
    konto.cpp \
    kredyt.cpp \
    listaadminow.cpp \
    listaklientow.cpp \
    listakont.cpp \
    lokata.cpp \
    main.cpp \
    mainwindow.cpp \
    przelew.cpp \
    sesja.cpp

HEADERS += \
    admin.h \
    data.h \
    historia.h \
    klient.h \
    konto.h \
    kredyt.h \
    listaadminow.h \
    listaklientow.h \
    listakont.h \
    lokata.h \
    mainwindow.h \
    osoba.h \
    przelew.h \
    sesja.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../build-Bank-Desktop_Qt_5_15_2_MSVC2019_32bit-Debug/admini.txt \
    ../build-Bank-Desktop_Qt_5_15_2_MSVC2019_32bit-Debug/klienci.txt \
    ../build-Bank-Desktop_Qt_5_15_2_MSVC2019_32bit-Debug/konta.txt \
    gotowe.txt
