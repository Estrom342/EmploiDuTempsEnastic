#include "parametre.h"

Parametre::Parametre()
{
    database = QSqlDatabase::addDatabase("QSQLITE", "PARAMETRE");
    database.setDatabaseName("setting");

    m_login = "root";
    m_password = "superMotDePasse";
    m_portServeur = 2222;
    m_portDatabase = 3306;
    m_hostName = "localhost";
    readData();
}

void Parametre::readData()
{
    if(database.open()){
        QString txt = "CREATE TABLE IF NOT EXISTS Parametre("
                        "login VARCHAR(90),"
                        "password VARCHAR(90), hostName VARCHAR(90), "
                        "portServeur INTEGER,"
                        "portDatabase INTEGER);";
        auto query = QSqlQuery(database);
        if(!query.exec(txt)){
           qDebug()<<"Error at Parametre::Parametre: "<<query.lastError().text();
           emit information(tr("Erreur de création de fichier"), query.lastError().text());
        }
        else{
            txt = "SELECT * FROM Parametre;";
            if(query.exec(txt)){
                while(query.next()){
                    m_login = query.value("login").toString();
                    m_password = query.value("password").toString();
                    m_hostName = query.value("hostName").toString();
                    m_portServeur = query.value("portServeur").toUInt();
                    m_portDatabase = query.value("portDatabase").toUInt();
                }
            }else
                emit information(tr("Erreur de lecture de données"), query.lastError().text());
        }
    }else
        qDebug()<<"Error at Parametre::Parametre: (impossible d'ouvrir la base de données "<<database.lastError().text();
}


void Parametre::save()
{
    QString txt = "DELETE FROM Parametre;";
    auto query = QSqlQuery(database);
    if(query.exec(txt)){
        query.prepare("INSERT INTO Parametre(login, password, hostName, portServeur, portDatabase)"
                                   "VALUES(:login, :password, :hostName, :portServeur, :portDatabase);");
        query.bindValue(":login", m_login);
        query.bindValue(":password", m_password);
        query.bindValue(":hostName", m_hostName);
        query.bindValue(":portServeur", m_portServeur);
        query.bindValue(":portDatabase", m_portDatabase);
        if(query.exec())
            emit information("", "Enregistrer!");
        else
            emit information(tr("Erreur d'enregistrement"), query.lastError().text());
    }
    else
        emit information(tr("Erreur de mise à jour"), query.lastError().text());
}

const QString &Parametre::login() const
{
    return m_login;
}

void Parametre::setLogin(const QString &newLogin)
{
    m_login = newLogin;
}

const QString &Parametre::password() const
{
    return m_password;
}

void Parametre::setPassword(const QString &newPassword)
{
    m_password = newPassword;
}

quint16 Parametre::portServeur() const
{
    return m_portServeur;
}

void Parametre::setPortServeur(quint16 newPortServeur)
{
    m_portServeur = newPortServeur;
}

quint16 Parametre::portDatabase() const
{
    return m_portDatabase;
}

void Parametre::setPortDatabase(quint16 newPortDatabase)
{
    m_portDatabase = newPortDatabase;
}

const QString &Parametre::hostName() const
{
    return m_hostName;
}

void Parametre::setHostName(const QString &newHostName)
{
    m_hostName = newHostName;
}
