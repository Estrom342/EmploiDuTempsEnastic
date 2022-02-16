#ifndef CONSTANTE_H
#define CONSTANTE_H

#include <QString>
#include <QPalette>

//#define ADMIN_V
//#define STUDENT_V

const QString DATABASENAME = "data.sql";
const QString DEFAUL_HOSTNAME = "emploi.enastic.lan";
const quint16 DEFAUL_PORTNAME = 2222;
const quint16 DEFAUL_PORTNAME_DB = 3306;
const  QString FLAG = "FLAG";
const QString DBNAME = "EmploieDuTemps";
//const QString DBNAME = "tmpdb";
const  QString GET_EMPLOI_FLAG = "GET_EMPLOI_FLAG";
const  QString EMPLOI_FLAG = "EMPLOI_FLAG";
const  QString GET_LAST_EMPLOI_FLAG = "GET_LAST_EMPLOI_FLAG";
const  QString LAST_EMPLOI_FLAG = "LAST_EMPLOI_FLAG";
const QString SIGNATURE_EMPLOI_FLAG = "SIGNATURE_EMPLOI_FLAG";
const QString GET_SIGNATURE_EMPLOI_FLAG = "GET_SIGNATURE_EMPLOI_FLAG";
const QString PALETTE_CLEAR_TXT = QString("Clear");
const QString PALETTE_SOMBRE_TXT = QString("Sombre");
const QString DEFAULT_COMMENT = "DEFAULT_COMMENT";
const QByteArray DEFAULT_SIGNATURE = QByteArray("DEFAULT_SIGNATURE");
const QString SIGNATURE_VIDE = "SIGNATURE_VIDE";
const QString MAIN_SOCKET_NAME = "MAIN_SOCKET_NAME";
const QString TEMP_SOCKET_NAME = "TEMP_SOCKET_NAME";

//Green color #6BEC39
//Blue #4A76E1
//Violet #B37DDE

//Les nombres constants

#define ANDROID_EMPLOI
//#define DESKTOP_EMPLOI
const int DEFAULT_FONT_SIZE = 11;

#endif // CONSTANTE_H
