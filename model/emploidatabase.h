#ifndef EMPLOIDATABASE_H
#define EMPLOIDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "matiere.h"
#include "teacher.h"
#include "emploiedutemp.h"
#include "model/constante.h"
#include <QPalette>

class EmploiDatabase : public QObject
{
    Q_OBJECT
public:
    explicit EmploiDatabase(QString const& hostName="localhost", QString databaseName=DBNAME, QObject *parent = nullptr);
    EmploiDatabase(QSqlDatabase const& database);
    bool isOpen();
    bool open();
    bool createDatabase(QString user, QString pass);
    bool addNiveau(QString const& nomNiveau, QString const& nomSalle="");
    bool deleteNiveau(const QString &nomNiveau);
    bool updateNiveau(QString oldName, QString newName, QString nomSalle);
    QString getSalle(QString level);
    QString getNiveau(QString nomSalle);
    int getIdNiveau(QString niveau);
    bool addStatictMatiere(Matiere const& matiere);
    bool addMatiere(Matiere &matiere);
    bool updateStatictMatiere(Matiere const& ancienne, const Matiere &nouvelle);
    bool addEmploie(EmploieDuTemp emploi);
    bool updateEmploie(int idEmploi, QString const& semestre);
    bool updateMatiere(Matiere &ancienne, Matiere &nouvelle);
    bool addHoraire(Horaire const& horaire);
    bool updateHoraire(Horaire const& horaire);
    bool deleteMatiere(const Matiere &matiere);
    bool addTeacher(Teacher const& t);
    bool deleteTeacher(Teacher const& t);
    bool updateTeacher(Teacher const& oldTeacher, Teacher const& newTeacher);
    //bool updateTeacher(Teacher const& t);

    QList<Matiere> getlsStaticMatiere(QString const& level);
    QString getMatiereName(Teacher const& teacher, QString const& level);
    QList<Teacher> getlsTeacher(QString const& level);
    QList<Teacher> getlsTeacher();
    QStringList getLsLevel();
    QStringList getLsSalles();
    QStringList getLsLevelDelete();
    QStringList getLsAdmin();
    bool addAdmin(QString login, QString password);
    bool deleteAdmin(QString login);
    bool changePassword(QString password);

    EmploieDuTemp getEmploi(QString const& niveau, QDate const& dateDebut, QDate const& dateFin);
    QList<EmploieDuTemp> lsEmplois();
    QList<Horaire> lsHorairs(int idEmploi);
    QList<Matiere> lsMatieres(int idEmploi);
    EmploieDuTemp getLastEmploi(QString const& niveau);
    QByteArray getSignatureEmploi(const QString &level, const QDate& dateDebut, const QDate& dateFin);

    int getIdEmploi(EmploieDuTemp const& emploi);
    int getIdMatiere(Matiere const& matiere);
    int getIdEnseignant(Teacher const&  t);

    const QSqlDatabase &getDatabase() const;
    void close();

    void setDatabase(const QSqlDatabase &newDatabase);

    const QString &hostName() const;
    void setHostName(const QString &newHostName);

    const QString &login() const;
    void setLogin(const QString &newLogin);

    const QString &password() const;
    void setPassword(const QString &newPassword);
    QString errorConnexion();
    bool updateSignature(QByteArray const& signature, int idEmploi);
    Teacher getTeacher(QString const& nomMatiere, QString const& niveau);
public slots:
    bool connectToHost(QString const& login= "gek", QString password= "superMotDePAsse", quint16 portName = 3306, QString const& hostName = "localhost");

signals:
    void information(QString, QString);
    void warning(QString, QString);
    void critical(QString, QString);
    void statut(QString);
    //void statutBar(QString);
    void connexionSignal();
    void disConnexionSignal();

private:
    QString m_hostName, m_login, m_password, databaseName;
    quint16 portName;
    QString m_erreur;
    QSqlDatabase database;
};

#endif // EMPLOIDATABASE_H
