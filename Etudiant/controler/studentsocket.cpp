#include "studentsocket.h"

StudentSocket::StudentSocket(QObject *parent) : QTcpSocket(parent), connected(false)
{
    static int nbreConnexion = 0;
    database = new LocalDatabase(DATABASENAME, QString("CONNEXION %1").arg(nbreConnexion++)); connected = false;
    //setReadBufferSize(90000);
    connect(this, &QTcpSocket::connected, this, &StudentSocket::onConnected);
    connect(this, &QTcpSocket::disconnected, this, &StudentSocket::onDisconnected);
    connect(this, &QTcpSocket::readyRead, this, &StudentSocket::onReadyRead);
    connect(this, &QTcpSocket::stateChanged, this, &StudentSocket::onStateChange);
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onErrorOccurred(QAbstractSocket::SocketError)));
    tryToConnect();
}

StudentSocket::StudentSocket()
{
    static int nbreConnexion = -1;
    database = new LocalDatabase(DATABASENAME, QString("CONNEXION %1").arg(nbreConnexion--)); connected = false;
    connect(this, &QTcpSocket::readyRead, this, &StudentSocket::onReadyRead);
}

void StudentSocket::onConnected()
{
    connected= true;
    QByteArray data;
    QTextStream stream(&data, QIODevice::WriteOnly);
    stream<<FLAG; Qt::endl(stream);
    stream<<GET_LAST_EMPLOI_FLAG; Qt::endl(stream);
    write(data);
}

void StudentSocket::onDisconnected()
{
    connected= false;
    qDebug()<<"StudentSocket::onDisconnected()";
    tryToConnect();
}

//Reception de données depuis le serveur

void StudentSocket::onReadyRead()
{
    qDebug()<<QThread::currentThread();
    emit status(tr("Téléchargement d'emploi du temps en cours ..."));
    QByteArray data;
    while(!atEnd()){
        data+= read(sizeof (quint16));
        if(data.size() >= (int)sizeof(quint16)){
            break;
        }
    }
    QTextStream stream(&data, QIODevice::ReadOnly);
    int prevSize = -1;
    int round = 0;
    while(bytesAvailable()>0 || waitForReadyRead()){
        waitForReadyRead(1000);
        //qDebug()<<"data.size() = "<<data.size();
        prevSize = data.size();
        data+= readAll();
        if(prevSize == data.size() && round++ > 3)
            break;
    }
    //qDebug()<<"Taille de données recues: "<<taille<<" et data.size = "<<data.size();
    int nbreEmbloi =0;
    while(!stream.atEnd()){
        QString flag = stream.readLine();
        flag = stream.readLine();
        qDebug()<<objectName()<<flag;
        if(flag == EMPLOI_FLAG){
            EmploieDuTemp em;
            stream>>em;
            if(database->addEmploi(em)){
                qDebug()<<"//Demande de signature d'emploi";
                if(em.niveau().isEmpty()){
                    qDebug()<<"Nom de l'emploi vide"<<em.niveau()<<em.lsMatieres().size()<<em.lsHoraire().size();
                    continue;
                }
                EmploieDuTemp tmpEmploi(em.niveau());
                tmpEmploi.setNomSalle(em.nomSalle());
                tmpEmploi.setDateDebut(em.dateDebut());
                tmpEmploi.setLsHoraire(QList<Horaire>());
                tmpEmploi.setLsMatieres(QList<Matiere>());
                askSignatureEmploi(em);
                QThread::currentThread()->sleep(1);
                nbreEmbloi++;
            }
            //qDebug()<<"StudentSocket::onReadyRead: EMPLOI_FLAG "<<em.niveau()<<em.lsMatieres().size();
        }else if(flag == LAST_EMPLOI_FLAG){
            nbreEmbloi++;
            EmploieDuTemp em;
            stream>>em;
            if(em.lsMatieres().size()>0){
                database->addEmploi(em);
                emit emploisAvailables();
                emit emploiAvailable(em.dateDebut(), em.niveau());
                //Demande de signature d'emploi
                if(em.niveau().isEmpty()){
                    qDebug()<<"Nom de l'emploi vide"<<em.niveau()<<em.lsMatieres().size()<<em.lsHoraire().size();
                    continue;
                }
                EmploieDuTemp tmpEmploi(em.niveau());
                tmpEmploi.setNomSalle(em.nomSalle());
                tmpEmploi.setDateDebut(em.dateDebut());
                tmpEmploi.setLsHoraire(QList<Horaire>());
                tmpEmploi.setLsMatieres(QList<Matiere>());
                //askSignatureEmploi(em);
            }
            //emit emploisAvailables(em);
            //qDebug()<<"StudentSocket::onReadyRead (LAST_EMPLOI_FLAG): "<<em.niveau()<<em.lsMatieres().size();
        }else if(flag == SIGNATURE_EMPLOI_FLAG){
            EmploieDuTemp em;
            QByteArray signature;
            stream>>em;
            stream>>signature;
            em.setNomSalle(emploi().nomSalle());
            emit signatuereAvaible(em.niveau(), emploi().nomSalle(), em.dateDebut(), em.dateFin(), signature);
            qDebug()<<"StudentSocket::onReadyRead"<<SIGNATURE_EMPLOI_FLAG<<signature.size()<<
                      em.niveau()<<em.nomSalle();
        }
        else{
            qDebug()<<"StudentSocket::onReadyRead IGNORED FLAG = "<<bytesAvailable()<<objectName()<<flag;
        }
    }
    if(nbreEmbloi>0)
        emit emploisAvailables();
    emit status(tr("Connecté"));
    if(bytesAvailable() > 0){
        qDebug()<<"Données restantes = "<<bytesAvailable();
    }
    if(objectName() == TEMP_SOCKET_NAME){
        close();
        deleteLater();
    }
}

//
void StudentSocket::onStateChange(SocketState stat)
{
    switch (stat) {
    case QTcpSocket::ConnectingState:
        emit status(("Tentative de connexion en cours ...")); break;
    case QTcpSocket::ConnectedState:
        emit status(tr("Connecté")); break;
    case QTcpSocket::UnconnectedState:
        emit status(tr("Non connecté"));
    default:
        break;
    }
}

void StudentSocket::onErrorOccurred(SocketError error)
{
    switch (error) {
    case QTcpSocket::ConnectionRefusedError:
        emit status(tr("Connexion réfusée")); break;
    case QTcpSocket::HostNotFoundError:
        emit status(tr("Erreur de connexion: Adresse du serveur non trouvé")); break;
    case QTcpSocket::SocketTimeoutError:
        if(!isConnected())
            emit status(tr("Temps de connexion expiré"));
        break;
    case QTcpSocket::RemoteHostClosedError:
        emit status(tr("Erreur de connexion: le serveur à fermer la connexion")); break;
    default: break;
    }
    auto timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &StudentSocket::tryToConnect);
    connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(5000);
}

const EmploieDuTemp &StudentSocket::emploi() const
{
    return m_emploi;
}

void StudentSocket::setEmploi(const EmploieDuTemp &newEmploi)
{
    m_emploi = newEmploi;
}

void StudentSocket::tryToConnect()
{
    if(connected) emit status(tr("Connecté"));
    if(state() == ConnectingState || state() == HostLookupState || connected == true) return;
    auto pr = database->getPrametre();
    connectToHost(pr.getHostName(), pr.getPortName());
}

//Demande l'emploi au serveur
void StudentSocket::askEmploi(QDate dateDebut, QString niveau)
{
    QByteArray data;
    QTextStream stream(&data, QIODevice::WriteOnly);
    EmploieDuTemp em(niveau); em.setDateDebut(dateDebut);
    stream<<FLAG; Qt::endl(stream);
    stream<<GET_EMPLOI_FLAG; Qt::endl(stream);
    stream<<em;
    write(data);
    qDebug()<<"StudentSocket::askEmploi"<<em.dateDebut()<<em.dateFin();
}

//Demande au serveur la signature de l'emploi de temps
void StudentSocket::askSignatureEmploi(const EmploieDuTemp &em)
{
    if(!connected)
        return;
    //qDebug()<<"Demande de signarure pour l'emploi de "<<emploi().nomSalle();
    auto tmpSock = new StudentSocket();
    tmpSock->setEmploi(em);
    connect(tmpSock, &StudentSocket::status, this, &StudentSocket::status);
    connect(tmpSock, &StudentSocket::signatuereAvaible, this, &StudentSocket::signatuereAvaible);
    tmpSock->setObjectName(TEMP_SOCKET_NAME);//pour detruire a la fin de lecture de données

    connect(tmpSock, &QTcpSocket::connected, tmpSock, [tmpSock](){
        QByteArray data;
        QTextStream stream(&data, QIODevice::WriteOnly);
        stream<<FLAG; Qt::endl(stream)<<GET_SIGNATURE_EMPLOI_FLAG;
        Qt::endl(stream)<<tmpSock->emploi();
        tmpSock->write(data);
        //qDebug()<<"Demande de signarure pour l'emploi de "<<tmpSock->emploi().nomSalle()<<tmpSock->emploi().niveau();
    });
    auto thread = new QThread();
    tmpSock->setParent(nullptr);
    tmpSock->moveToThread(thread);
    connect(thread, &QThread::started, tmpSock, [tmpSock](){
        tmpSock->tryToConnect();// Se connecter pour telecharger la signature

    });
    connect(tmpSock, &QTcpSocket::destroyed, thread, [thread](){
        thread->exit();
        if(thread->wait()){
            thread->deleteLater();
        }
    });
    thread->start();
}

bool StudentSocket::isConnected() const
{
    return connected;
}
