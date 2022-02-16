QT+=    gui
QT+=    network sql

CONFIG +=  c++11 console

INCLUDEPATH += "../"
SOURCES += \
    ../model/emploidatabase.cpp \
    ../model/emploiedutemp.cpp \
    ../model/horaire.cpp \
    ../model/matiere.cpp \
    ../model/teacher.cpp \
    controler/serveuremploi.cpp \
    main.cpp \
    #model/emploidatabase.cpp \
    #model/emploiedutemp.cpp \
    #model/matiere.cpp \
 \    #model/teacher.cpp
    parametre.cpp

HEADERS += \
    ../model/constante.h \
    ../model/emploidatabase.h \
    ../model/emploiedutemp.h \
    ../model/horaire.h \
    ../model/matiere.h \
    ../model/teacher.h \
    controler/serveuremploi.h \
    #model/emploidatabase.h \
    #model/emploiedutemp.h \
    #model/matiere.h \
 \    #model/teacher.h
    parametre.h
