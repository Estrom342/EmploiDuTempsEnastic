#include "serveuremploi.h"

ServeurEmploi::ServeurEmploi()
{
    nbreConnexion = 0;
    lsStudent.clear();
    database = new EmploiDatabase();
    setMaxPendingConnections(1000);
    connect(this, &QTcpServer::newConnection, this, &ServeurEmploi::connexionEntrant);
    connect(database, &EmploiDatabase::information, this, &ServeurEmploi::debug);
    connect(database, &EmploiDatabase::critical, this, &ServeurEmploi::debug);
}

void ServeurEmploi::demarer()
{
    Parametre p;
    if(listen(QHostAddress::Any, p.portServeur())){
        emit debug(QString("Serveur a l'ecoute sur le port %1").arg(serverPort()));
    }else
        emit debug(QString("Erreur de demarage du serveur: %1").arg(errorString()));
    if(database->connectToHost(p.login(), p.password(), p.portDatabase(), p.hostName()))
        emit debug("Connexion du serveur à la base de données reusue");
    else
        emit debug(QString("Connexion du serveur à la base de données echoué %1").arg(database->errorConnexion()));
}

void ServeurEmploi::connexionEntrant()
{
    if(!hasPendingConnections()) return;
    auto me = this;
    static quint16 nbreConnexion=0;
    QTcpSocket* student = nextPendingConnection();

    auto *studentThread = new QThread();

    connect(student, &QTcpSocket::readyRead, student, [student, me](){
        auto handle = QSqlDatabase::addDatabase("QMYSQL", QString("Etudiant numero %1").arg(++nbreConnexion));
        handle.setHostName(me->database->hostName());
        handle.setPassword(me->database->password());
        handle.setUserName(me->database->login());
        handle.setDatabaseName(me->database->getDatabase().databaseName());
        auto database = new EmploiDatabase(handle);

        if(database->open()){
            readData(database, student);
            database->close();
        }
        else
            qDebug()<<"QTcpSocket::readyRead "<<database->getDatabase().lastError();
        database->deleteLater();
    });
    connect(student, &QTcpSocket::disconnected, this,[me](){
        emit me->debug(QString("Nouvelle deconnexion \n%1 connectés").arg(--me->nbreConnexion));
    });
    connect(student, &QTcpSocket::disconnected, this, [student](){
        if(student != 0)
            student->deleteLater();
    });
    student->setParent(nullptr);
    student->moveToThread(studentThread);
    studentThread->start();

    emit debug(QString("Nouvelle connexion entranteM\n%1 connectés").arg(++me->nbreConnexion));
}


//Lecture et reponse a des requette des etuduant
void ServeurEmploi::readData(EmploiDatabase *database, QTcpSocket *student)
{
    //qDebug()<<"StudentThread::readeData. Current thread: "<<QThread::currentThread();
    QByteArray data;
    while(!student->atEnd()){
        //if(student->waitForReadyRead())
        //    data+= student->readAll();
        data+= student->readAll();
    }
    QTextStream stream(&data, QIODevice::ReadOnly);
    while(!stream.atEnd()){
        QString flag = stream.readLine();
        flag = stream.readLine();
        if(flag == GET_EMPLOI_FLAG){
            EmploieDuTemp em;
            stream>>em;
            em = database->getEmploi(em.niveau(), em.dateDebut(), em.dateFin());
            QByteArray reponse;
            QTextStream streamReponse(&reponse, QIODevice::WriteOnly);
            streamReponse<<FLAG;Qt::endl(streamReponse);
            stream<<EMPLOI_FLAG; Qt::endl(streamReponse);
            streamReponse<<em;
            student->write(reponse);
        }else if(flag == GET_LAST_EMPLOI_FLAG){
            qDebug()<<"StudentThread::readeData FLAG = "<<flag;
            auto lsFilieres = database->getLsLevel();
            QByteArray reponse;
            QTextStream streamReponse(&reponse, QIODevice::WriteOnly);
            foreach (QString const& filiere, lsFilieres) {
                auto emploi = database->getLastEmploi(filiere);
                if(emploi.lsMatieres().isEmpty())
                    continue;
                streamReponse<<FLAG;Qt::endl(streamReponse);
                streamReponse<<LAST_EMPLOI_FLAG; Qt::endl(streamReponse);
                streamReponse<<emploi;
            }
            student->write(reponse);
        }else if(flag == GET_SIGNATURE_EMPLOI_FLAG){
            EmploieDuTemp em;
            stream>>em;
            auto signature = database->getSignatureEmploi(em.niveau(), em.dateDebut(), em.dateFin());
            QByteArray reponse;
            QTextStream streamReponse(&reponse, QIODevice::WriteOnly);
            streamReponse<<FLAG<<"\n";
            streamReponse<<SIGNATURE_EMPLOI_FLAG<<"\n";
            streamReponse<<em;
            streamReponse<<signature;
            student->write(reponse);
            qDebug()<<em.niveau()<<em.dateDebut()<<em.dateFin()<<"StudentThread::readeData FLAG = "<<flag<<signature.size();
        }
        else{
            qDebug()<<"StudentThread::readeData: ignored FLAG = "<<flag;
            //student->close();
        }
        qDebug()<<flag<<"Byte avaible ="<<student->bytesAvailable();
    }
}
