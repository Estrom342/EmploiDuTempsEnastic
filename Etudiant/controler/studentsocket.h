#ifndef STUDENTSOCKET_H
#define STUDENTSOCKET_H

#include <QDate>
#include <QDataStream>
#include <QThread>
#include <QMetaType>
#include "model/emploiedutemp.h"
#include "model/localdatabase.h"
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class StudentSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit StudentSocket(QObject *parent);
    StudentSocket();
    bool isConnected() const;

    //void setDatabase(LocalDatabase *newDatabase);

    const EmploieDuTemp &emploi() const;
    void setEmploi(const EmploieDuTemp &newEmploi);

public slots:
    void askEmploi(QDate dateDebut, QString niveau);
    void askSignatureEmploi(EmploieDuTemp const &em);
    void tryToConnect();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onStateChange(QAbstractSocket::SocketState stat);
    void onErrorOccurred(QAbstractSocket::SocketError error);

signals:
    void emploiAvailable(QDate, QString);
    void emploisAvailables();
    void status(QString); //Signal pour afficher le status a la barre de status
    void signatuereAvaible(QString const& level, QString const& nomSalle, QDate const& dateDebut, QDate const& dateFin, QByteArray const& signature); //Signal emit quand une signature de l'emploi est telechargée
    //void lsHor

private:
    bool connected;
    LocalDatabase *database;
    EmploieDuTemp m_emploi;
};

#endif // STUDENTSOCKET_H
