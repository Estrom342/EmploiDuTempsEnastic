QT       += core gui sql network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += "../"
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../model/emploiedutemp.cpp \
    ../model/horaire.cpp \
    ../model/localdatabase.cpp \
    ../model/matiere.cpp \
    ../model/teacher.cpp \
    emploiwidget.cpp \
    homewidget.cpp \
    horaireview.cpp \
    main.cpp \
    mainwindow.cpp \
    matiereview.cpp \
    #matierewidget.cpp \
    controler/studentsocket.cpp

HEADERS += \
    ../model/constante.h \
    ../model/emploiedutemp.h \
    ../model/horaire.h \
    ../model/localdatabase.h \
    ../model/matiere.h \
    ../model/teacher.h \
    controler/studentsocket.h \
    emploiwidget.h \
    homewidget.h \
    horaireview.h \
    mainwindow.h \
    matiereview.h
    #matierewidget.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
