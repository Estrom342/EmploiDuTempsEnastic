#ifndef PARAMETRE_H
#define PARAMETRE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QObject>

class Parametre: public QObject
{
    Q_OBJECT
public:
    Parametre();
    void save();

    const QString &login() const;
    void setLogin(const QString &newLogin);

    const QString &password() const;
    void setPassword(const QString &newPassword);

    quint16 portServeur() const;
    void setPortServeur(quint16 newPortServeur);

    quint16 portDatabase() const;
    void setPortDatabase(quint16 newPortDatabase);

    const QString &hostName() const;
    void setHostName(const QString &newHostName);

private:
    void readData();
    QString m_login,
    m_password;
    quint16 m_portServeur,
    m_portDatabase;
    QString m_hostName;
    QSqlDatabase database;
signals:
    void information(QString, QString);
};

#endif // PARAMETRE_H
