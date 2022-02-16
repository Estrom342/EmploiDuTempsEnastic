#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QMutex>
#include "emploiedutemp.h"
#include "constante.h"

class LocalDatabase : public QObject
{
    Q_OBJECT
public:
    explicit LocalDatabase(const QString &dbName = DATABASENAME, const QString& connexionName = DATABASENAME, QObject *parent = nullptr);
    ~LocalDatabase();
    QStringList getLsLevel() const;
    bool addLevel(const QString &level, const QString &nomSalle) const;
    int getIdLevel(const QString &level, const QString& nomSalle) const;
    bool addEmploi(EmploieDuTemp const& emploi) const;
    int getIdEmploi(const QString &filiere,const QString &nomSalle, QDate dateDebut, QDate dateFin) const;
    int getIdEmploi(EmploieDuTemp const& emploi) const;
    EmploieDuTemp getEmploi(QString filiere, QDate dateDebut, QDate dateFin) const;
    EmploieDuTemp getLastEmploi(QString filiere) const;
    void close();
    static bool createDatabase(const QString &fileName = ":/database/model/localDataBase.sql");
private:
    class Parametre{
    public:
        Parametre();
        const QString &getHostName() const;
        void setHostName(const QString &newHostName);

        const QString &getStudentName() const;
        void setStudentName(const QString &newStudentName);

        const QString &getLevel() const;
        void setLevel(const QString &newLevel);

        quint16 getPortName() const;
        void setPortName(quint16 newPortName);

        const QString &getTheme() const;
        void setTheme(const QString &newTheme);

        const QPalette &getPalete() const;

        const QString &getPassword() const;
        void setPassword(const QString &newPassword);

        const QByteArray &signature() const;
        void setSignature(const QByteArray &newSignature);

    private:
        QString hostName, studentName, level, theme, password;
        quint16 portName; QPalette palete;
        QByteArray m_signature;
    };
public:
    Parametre getPrametre() const;
    void setParametre(const Parametre &p) const;
    bool updateSignature(EmploieDuTemp const& em);

signals:
    //void emploisAvailables();
    //void newLevelAvailble();
private:
    //Methode private
    bool updateEmploi(EmploieDuTemp const& emploi) const;
    bool addMatiere(Matiere const& m) const;
    bool addHoraire(Horaire const& h) const;
    //Variables membre
    QSqlDatabase database;
    QString dbName;
    //QMutex *mutex;
};

#endif // LOCALDATABASE_H
