QT       += core gui sql printsupport
#QTPLUGIN += qsqlmysql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
INCLUDEPATH += "../"
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../model/emploidatabase.cpp \
    ../model/emploiedutemp.cpp \
    ../model/horaire.cpp \
    ../model/localdatabase.cpp \
    ../model/matiere.cpp \
    ../model/teacher.cpp \
    emploiwidget.cpp \
    horaireview.cpp \
    loginwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    matiereview.cpp \
    matierewidget.cpp \
    newemploiwidget.cpp \
    teacherwidget.cpp \
    timetablehistorywidget.cpp

HEADERS += \
    ../model/constante.h \
    ../model/emploidatabase.h \
    ../model/emploiedutemp.h \
    ../model/horaire.h \
    ../model/localdatabase.h \
    ../model/matiere.h \
    ../model/teacher.h \
    emploiwidget.h \
    horaireview.h \
    loginwidget.h \
    mainwindow.h \
    matiereview.h \
    matierewidget.h \
    newemploiwidget.h \
    teacherwidget.h \
    timetablehistorywidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    model/resource.qrc
