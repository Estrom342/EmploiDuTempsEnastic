#ifndef SERVEUREMPLOI_H
#define SERVEUREMPLOI_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include "model/emploidatabase.h"
#include "parametre.h"

class ServeurEmploi : public QTcpServer
{
    Q_OBJECT
public:
    ServeurEmploi();
    void demarer();
    static void readData(EmploiDatabase *database, QTcpSocket *student);

public slots:
    void connexionEntrant();

signals:
    void debug(QString);
private:
    EmploiDatabase *database;
    quint16 nbreConnexion;
    QList<QTcpSocket*> lsStudent;
};

#endif // SERVEUREMPLOI_H
