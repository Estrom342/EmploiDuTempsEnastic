#include "localdatabase.h"

LocalDatabase::LocalDatabase(const QString &dbName, const QString &connexionName, QObject *parent) : QObject(parent), dbName(dbName)
{
    //mutex = new QMutex();
    //static QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connexionName);
    database= QSqlDatabase::addDatabase("QSQLITE", connexionName);

    QDir rep(QDir::homePath());
    if(QDir(QDir::homePath()+"/.emploiEnastic").exists() || rep.mkdir(QDir::homePath()+"/.emploiEnastic"))
        database.setDatabaseName(QDir::homePath()+"/.emploiEnastic/"+DATABASENAME);
    else{
        database.setDatabaseName(DATABASENAME);
        qDebug()<<"Error at LocalDatabase::LocalDatabase: impossible de creer le repertoire";
    }
    //database = db;//QSqlDatabase::addDatabase("QSQLITE", dbName);
    if(!database.open())
        qDebug()<<"Error at LocalDatabase::LocalDatabase :"<<database.lastError().text();
    //qDebug()<<"DATABASE NAME= "<<database.databaseName();
}

LocalDatabase::~LocalDatabase()
{
    if(database.isOpen())
        database.close();
}

//Renvoi la liste de tous les niveaux
QStringList LocalDatabase::getLsLevel() const
{
    //mutex->lock();
    auto query = QSqlQuery(database);
    QStringList lsLevel;
    if(query.exec("SELECT DISTINCT nomNiveau FROM Niveau;")){
        while(query.next())
            lsLevel<<query.value("nomNiveau").toString();
    }else
        qDebug()<<"Error at LocalDatabase::getLsLevel: "<<query.lastError().text();
    //mutex->unlock();
    return lsLevel;
}

//Ajout un niveau
bool LocalDatabase::addLevel(const QString &level, const QString &nomSalle) const
{
    //mutex->lock();
    if(getIdLevel(level, nomSalle)!= -1) return true;
    auto query = QSqlQuery(database);
    query.prepare("INSERT INTO Niveau(nomNiveau, nomSalle) VALUES(:level, :nomSalle);");
    query.bindValue(":level", level);
    query.bindValue(":nomSalle", nomSalle);
    if(!query.exec()){
       qDebug()<<"Error at LocalDatabase::getIdLevel: "<<query.lastError().text();
       //mutex->unlock();
       return false;
    }
    //mutex->unlock();
    return true;
}

//Retourne l'identifiant de level
int LocalDatabase::getIdLevel(QString const& level, const QString &nomSalle) const
{
    //mutex->lock();
    auto query = QSqlQuery(database);
    query.prepare("SELECT idNiveau FROM Niveau WHERE nomNiveau = :level AND nomSalle = :nomSalle;");
    query.bindValue(":level", level);
    query.bindValue(":nomSalle", nomSalle);
    if(query.exec()){
        while(query.next()){
            bool ok = false;
            int id = query.value("idNiveau").toInt(&ok);
            if(ok){
                //mutex->unlock();
                return id;
            }
        }
    }else
        qDebug()<<"Error at LocalDatabase::getIdLevel: "<<query.lastError().text();
    //mutex->unlock();
    return -1;
}

//Ajoute ou met a jour un nouvel emploi
bool LocalDatabase::addEmploi(const EmploieDuTemp &emploi) const
{
    if(emploi.lsMatieres().isEmpty()) return false;
    //mutex->lock();
    int idEmploi = getIdEmploi(emploi);
    if(idEmploi != -1){
        //mutex->unlock();
        return updateEmploi(emploi);
    }
    if(emploi.niveau().isEmpty()){
       //mutex->unlock();
        return false;
    }
    int idLevel = getIdLevel(emploi.niveau(), emploi.nomSalle());
    if(idLevel == -1)
        addLevel(emploi.niveau(), emploi.nomSalle());
    idLevel = getIdLevel(emploi.niveau(), emploi.nomSalle());
    if(idLevel == -1)
        qDebug()<<"Holala!!!!!!!!!!!!";

    //mutex->lock();//Verrouillage
    auto query = QSqlQuery(database);
    query.prepare("INSERT INTO Emploi(idNiveau, dateDebut, dateFin, dateCreation, lieuCreation, semestre)"
                        "VALUES(:idNiveau, :dateDebut, :dateFin, :dateCreation, :lieuCreation, :semestre);");
    query.bindValue(":idNiveau", idLevel);
    query.bindValue(":dateDebut", emploi.dateDebut());
    query.bindValue(":dateFin", emploi.dateFin());
    query.bindValue(":dateCreation", emploi.dateCreation());
    query.bindValue(":lieuCreation", emploi.lieuDeCreation());
    query.bindValue(":semestre", emploi.semestre());
    if(!query.exec()){
        qDebug()<<"LocalDatabase::addEmploi "<<emploi.dateCreation()<<emploi.lieuDeCreation();
        qDebug()<<"Error at LocalDatabase::addEmploi: "<<query.lastError().text();
       // mutex->unlock(); //Deverrouillage
        return false;
    }
    idEmploi = getIdEmploi(emploi); //assert(idEmploi!=-1);
    foreach (auto matiere, emploi.lsMatieres()) {
        matiere.setIdEmploi(idEmploi);
        addMatiere(matiere);
    }
    //Insertion de la liste des horaire
    foreach (auto h, emploi.lsHoraire()) {
        h.setIdEmploi(idEmploi);
        addHoraire(h);
    }
    //mutex->unlock(); // Deverouillage
    return true;
}



//Pour obtenir l'identifiant d'un emploi
int LocalDatabase::getIdEmploi(const QString &filiere, const QString &nomSalle, QDate dateDebut, QDate dateFin) const
{
    //mutex->lock();
    int idLevel = getIdLevel(filiere, nomSalle);
    if(idLevel == -1) {
        //mutex->unlock();
        return -1;
    }
    auto query = QSqlQuery(database);
    query.prepare("SELECT idEmploi FROM Emploi WHERE dateDebut = :dateDebut "
                   "AND dateFin = :dateFin AND idNiveau = :idNiveau;");
    query.bindValue(":dateDebut", dateDebut);
    query.bindValue(":dateFin", dateFin);
    query.bindValue(":idNiveau", idLevel);
    if(query.exec()){
        while(query.next()){
            bool ok = false;
            int id = query.value("idEmploi").toInt(&ok);
            if(ok){
                //mutex->unlock();
                return id;
            }
            //    qDebug()<<"Idemploi found"<<nomSalle;
        }
    }else
        qDebug()<<"Error at LocalDatabase::getIdEmploi: "<<query.lastError().text();
    qDebug()<<"Idemploi not found";
    //mutex->unlock();
    return -1;
}

//Memme que la fonction precedente
int LocalDatabase::getIdEmploi(const EmploieDuTemp &emploi) const
{
    return getIdEmploi(emploi.niveau(), emploi.nomSalle(), emploi.dateDebut(), emploi.dateFin());
}

//Retourne l'emploi de niveau filiere de la semaine dateDebut et dateFin
EmploieDuTemp LocalDatabase::getEmploi(QString filiere, QDate dateDebut, QDate dateFin) const
{
    //mutex->lock();
    EmploieDuTemp emploi(filiere);
    emploi.setDateDebut(dateDebut);
    emploi.setLsMatieres(QList<Matiere>());

    QList<Matiere> lsMatieres; lsMatieres.clear();
    auto query = QSqlQuery(database);
    query.prepare("SELECT * FROM EmploiView WHERE dateDebut = :dateDebut AND dateFin = :dateFin "
                    "AND nomNiveau = :nomNiveau;");
    query.bindValue(":dateDebut", dateDebut);
    query.bindValue(":dateFin", dateFin);
    query.bindValue(":nomNiveau", filiere);

    if(query.exec()){
        int i=0;
        while (query.next()) {
            Matiere m(query.value("nomMatiere").toString());
            m.setType(query.value("typeMatiere").toString());
            m.setNomEnseigant(query.value("nomEnseignant").toString());
            m.setCouleur(QColor(query.value("couleur").toString()));
            m.setNumeroCase(query.value("numCase").toInt());
            m.setCommentaire(query.value("commentaire").toString());
            lsMatieres.append(m);
            if(i == 0){
                emploi.setDateCreation(query.value("dateCreation").toDate());
                emploi.setLieuDeCreation(query.value("lieuCreation").toString());
                emploi.setSemestre(query.value("semestre").toString());
                emploi.setNiveau(query.value("nomNiveau").toString());
                emploi.setNomSalle(query.value("nomSalle").toString());
                emploi.setIdEmploi(query.value("idEmploi").toInt());
                emploi.setSignature(query.value("signature").toByteArray());
                //qDebug()<<"LocalDatabase::getEmploi "<<emploi.niveau()<<emploi.signature().size();
                i++;
            }
        }
        //Selection de la liste des horaire
        query.prepare("SELECT * FROM Horaire WHERE idEmploi = :idEmloi");
        query.bindValue(":idEmploi", emploi.idEmploi());
        if(query.exec()){
            QList<Horaire> lsHoraires;
            lsHoraires.clear();
            while (query.next()) {
                Horaire h;
                h.setNumeroCase(query.value("numCase").toInt());
                h.setHeureDebut(query.value("heureDebut").toTime());
                h.setHeureFin(query.value("heureFin").toTime());
                h.setIdEmploi(query.value("idEmploi").toInt());
                lsHoraires.append(h);
            }
            emploi.setLsHoraire(lsHoraires);
            //qDebug()<<"LsH.size = "<<emploi.lsHoraire().size();
        }
        else
            qDebug()<<"Error at LocalDatabase::getEmploi(Recuperation de liste d'horaire) : "<<query.lastError().text();
    }else
        qDebug()<<"Error at LocalDatabase::getEmploi: "<<query.lastError().text();
    emploi.setLsMatieres(lsMatieres);
    //mutex->unlock();
    return emploi;
}

//Retourne le dernier emploi du temps pour le niveau level
EmploieDuTemp LocalDatabase::getLastEmploi(QString filiere) const
{
    //mutex->lock();
    EmploieDuTemp emploi(filiere);
    emploi.setLsMatieres(QList<Matiere>());

    QList<Matiere> lsMatieres; lsMatieres.clear();    auto query = QSqlQuery(database);
    query.prepare("SELECT MAX(dateDebut) AS dateDebut, MAX(dateFin) AS dateFin FROM Emploi LEFT JOIN Niveau"
                    " ON Emploi.idNiveau = Niveau.idNiveau WHERE nomNiveau = :nomNiveau;");
    query.bindValue(":nomNiveau", filiere);
    if(query.exec()){
        while (query.next()) {
            auto dateDebut = query.value("dateDebut").toDate();
            auto dateFin = query.value("dateFin").toDate();
            return getEmploi(filiere, dateDebut, dateFin);
        }

    }else
        qDebug()<<"Error at LocalDatabase::getLastEmploi: hmm "<<query.lastError().text();
    emploi.setLsMatieres(lsMatieres);
    //mutex->unlock();
    return emploi;
}

void LocalDatabase::close()
{
    database.close();
}

//Cree la base de données locales
bool LocalDatabase::createDatabase(QString const& fileName)
{
    auto database = QSqlDatabase::addDatabase("QSQLITE", "LocalDatabase::createDatabase");
    QDir rep(QDir::homePath());
    if(QDir(QDir::homePath()+"/.emploiEnastic").exists() || rep.mkdir(QDir::homePath()+"/.emploiEnastic"))
        database.setDatabaseName(QDir::homePath()+"/.emploiEnastic/"+DATABASENAME);
    else{
        database.setDatabaseName(DATABASENAME);
        qDebug()<<"Error at LocalDatabase::createDatabase: impossible de creer le repertoire";
    }
    qDebug()<<"DATABASE NAME= "<<database.databaseName();
    if(database.open()){
        QFile file(fileName);

        if(file.open(QFile::ReadOnly)){
            QTextStream stream(&file);
            auto lsRequeste = stream.readAll().split(";");
            auto query = QSqlQuery(database);
            foreach (auto requeste, lsRequeste) {
                if(requeste.size()<10) continue;
                if(!query.exec(requeste)){
                    qDebug()<<"Error at LocalDatabase::createDatabase: "<<query.lastError().text();
                    qDebug()<<requeste;
                }
            }
            database.close();
            return true;
        }
    }else
        qDebug()<<"Error at LocalDatabase::createDatabase: "<<database.lastError().text();
    return false;
}

LocalDatabase::Parametre LocalDatabase::getPrametre() const
{
    //mutex->lock();
    Parametre p;
    auto query = QSqlQuery(database);
    if(query.exec("SELECT * FROM Parametre;")){
        while(query.next()){
            p.setHostName(query.value("hostName").toString());
            p.setStudentName(query.value("studentName").toString());
            if(p.getStudentName().isEmpty())
                p.setStudentName(QFileInfo(QDir::homePath()).fileName());
            if(p.getHostName().isEmpty())
                p.setHostName(DEFAUL_HOSTNAME);
            p.setLevel(query.value("studentLevel").toString());
            p.setPortName(query.value("portName").toInt());
            p.setTheme(query.value("theme").toString());
            p.setSignature(query.value("signature").toByteArray());
            p.setPassword(query.value("password").toString()); break;
        }
    }else
        qDebug()<<"Error at LocalDatabase::Parametre LocalDatabase::getPrametre "<<query.lastError().text();
    //mutex->unlock();
    return p;
}

void LocalDatabase::setParametre(const Parametre& p) const
{
    //mutex->lock();
    auto query = QSqlQuery(database);
    if(query.exec("DELETE FROM Parametre;")){
        query.prepare("INSERT INTO Parametre(hostName, portName, studentName, studentLevel, theme, password, signature)"
                                   "VALUES(:hostName, :portName, :studentName, :studentLevel, :theme, :password, :signature)");
        query.bindValue(":hostName", p.getHostName());
        query.bindValue(":portName", p.getPortName());
        query.bindValue(":studentName", p.getStudentName());
        query.bindValue(":studentLevel", p.getLevel());
        query.bindValue(":theme", p.getTheme());
        query.bindValue(":password", p.getPassword());
        query.bindValue(":signature", p.signature());
        if(!query.exec())
            qDebug()<<"Error at LocalDatabase::Parametre LocalDatabase::setParametre "<<query.lastError().text();
    }else
        qDebug()<<"Error at LocalDatabase::Parametre LocalDatabase::setParametre "<<query.lastError().text();
    //mutex->unlock();
}

//Met a jour un emploi
bool LocalDatabase::updateEmploi(const EmploieDuTemp &emploi) const
{
    //mutex->lock();
    int idEmploi = getIdEmploi(emploi);
    if(idEmploi == -1){
        qDebug()<<"LocalDatabase::updateEmploi idEmploi = -1 "<<emploi.dateCreation()<<emploi.lieuDeCreation();
        //mutex->unlock();
        return false;
    }
    auto query = QSqlQuery(database);
    //Suppression des horaire existantes
    query.prepare("DELETE FROM Horaire WHERE idEmploi = :idEmploi;");
    query.bindValue(":idEmploi", idEmploi);
    if(query.exec()){
        //Remplacement des horaire par des nouveaux
        foreach(auto h, emploi.lsHoraire()){
            h.setIdEmploi(idEmploi);
            addHoraire(h);
        }
        //Suppression des Matiere existantes
        query.prepare("DELETE FROM Matiere WHERE idEmploi = :idEmploi;");
        query.bindValue(":idEmploi", idEmploi);
        if(query.exec()){
            foreach(auto matiere, emploi.lsMatieres()){
                matiere.setIdEmploi(idEmploi);
                addMatiere(matiere);
            }
            //Mise a jour des information de l'emploi
            query.prepare("UPDATE Emploi SET dateCreation = :dateCreation, lieuCreation = :lieuCreation, "
                            "semestre = :semestre WHERE idEmploi = :idEmploi;");
            query.bindValue(":dateCreation", emploi.dateCreation());
            query.bindValue(":lieuCreation", emploi.lieuDeCreation());
            query.bindValue(":semestre", emploi.semestre());
            query.bindValue(":idEmploi", idEmploi);
            if(!query.exec())
                qDebug()<<"Error at LocalDatabase::updateEmploi (UPDATE Emploi): "<<query.lastError().text();
            //mutex->unlock();
            return true;
        }
        else
            qDebug()<<"Error at LocalDatabase::updateEmploi: (Suppression des matieres)"<<query.lastError().text();
    }else
        qDebug()<<"Error at LocalDatabase::updateEmploi: (Suppression des horaires)"<<query.lastError().text();
    //mutex->unlock();
    return false;
}

bool LocalDatabase::updateSignature(const EmploieDuTemp &em)
{
    //mutex->lock();
    auto query = QSqlQuery(database);
    int idNiveau = getIdLevel(em.niveau(), em.nomSalle());
    if(idNiveau == -1)
        qDebug()<<"Error at LocalDatabase::updateSignature (idNiveau = -1)";
    query.prepare("UPDATE Emploi SET signature = :signature WHERE dateDebut = :dateDebut AND "
                    " dateFin = :dateFin AND idNiveau = :idNiveau;");
    query.bindValue(":dateDebut", em.dateDebut());
    query.bindValue(":dateFin", em.dateFin());
    query.bindValue(":idNiveau", idNiveau);
    query.bindValue(":signature", QVariant(em.signature()), QSql::Binary);
    if(query.exec()){
        //qDebug()<<"LocalDatabase::updateSignature Ok"<<em.signature().size()<<em.niveau()<<
        //          em.nomSalle()<<em.dateDebut()<<em.dateFin()<<query.numRowsAffected();
       // mutex->unlock();
        return true;
    }
    qDebug()<<"Error at LocalDatabase::updateSignature "<<query.lastError().text();
    return false;
}

//Ajoute une matiere dans l'emploi du temps
bool LocalDatabase::addMatiere(const Matiere &m) const
{
    //mutex->lock();
    if(m.getIdEmploi() == -1){
        qDebug()<<"Error at LocalDatabase::addMatiere idEmploi negatif: ";
        //mutex->unlock();
        return false;
    }
    auto query = QSqlQuery(database);
    query.prepare("INSERT INTO Matiere(idEmploi, type, nomMatiere, nomEnseignant, couleur, numCase, commentaire)"
                               "VALUES(:idEmploi, :type, :nomMatiere, :nomEnseignant, :couleur, :numCase, :commentaire)");
    query.bindValue(":idEmploi", m.getIdEmploi());
    query.bindValue(":type", m.getType());
    query.bindValue(":nomMatiere", m.getName());
    query.bindValue(":nomEnseignant", m.getEnseignant().toString());
    query.bindValue(":couleur", m.getCouleur());
    query.bindValue(":numCase", m.getNumeroCase());
    query.bindValue(":commentaire", m.commentaire());
    if(query.exec()){
        //mutex->unlock();
        return true;
    }
    else
        qDebug()<<"Error at LocalDatabase::updateEmploi: "<<query.lastError().text();
    //mutex->unlock();
    return false;
}

bool LocalDatabase::addHoraire(const Horaire &h) const
{
    if(h.idEmploi() == -1){
        qDebug()<<"LocalDatabase::addHoraire idEmploi negatif";
        return false;
    }
    //mutex->lock();
    auto query = QSqlQuery(database);
    query.prepare("INSERT INTO Horaire(numCase, heureDebut, heureFin, idEmploi)"
                             "VALUES(:numCase, :heureDebut, :heureFin, :idEmploi);");
    query.bindValue(":numCase", h.numeroCase());
    query.bindValue(":heureDebut", h.heureDebut());
    query.bindValue(":heureFin", h.heureFin());
    query.bindValue(":idEmploi", h.idEmploi());
    if(!query.exec()){
        qDebug()<<"Error at LocalDatabase::addHoraire:"<<query.lastError().text();
        //mutex->unlock();
        return false;
    }
    //mutex->unlock();
    return true;
}

LocalDatabase::Parametre::Parametre(){
    hostName = DEFAUL_HOSTNAME;
    password = "";
    portName = DEFAUL_PORTNAME;
    theme = PALETTE_CLEAR_TXT;
    palete = QPalette();
    level = "";
    studentName = QFileInfo(QDir::homePath()).fileName();
}

quint16 LocalDatabase::Parametre::getPortName() const
{
    return portName;
}

void LocalDatabase::Parametre::setPortName(quint16 newPortName)
{
    portName = newPortName;
}

const QString &LocalDatabase::Parametre::getTheme() const
{
    return theme;
}

void LocalDatabase::Parametre::setTheme(const QString &newTheme)
{
    theme = newTheme;
    if(theme == PALETTE_CLEAR_TXT)
        palete = QPalette();
    if(theme == PALETTE_SOMBRE_TXT)
        palete = QPalette(QPalette(QColor(46, 46, 46, 46), QColor(72, 72, 72, 72)));
}
const QPalette &LocalDatabase::Parametre::getPalete() const
{
    return palete;
}

const QString &LocalDatabase::Parametre::getPassword() const
{
    return password;
}

void LocalDatabase::Parametre::setPassword(const QString &newPassword)
{
    password = newPassword;
}

const QByteArray &LocalDatabase::Parametre::signature() const
{
    return m_signature;
}

void LocalDatabase::Parametre::setSignature(const QByteArray &newSignature)
{
    m_signature = newSignature;
}

const QString &LocalDatabase::Parametre::getLevel() const
{
    return level;
}

void LocalDatabase::Parametre::setLevel(const QString &newLevel)
{
    level = newLevel;
}

const QString &LocalDatabase::Parametre::getStudentName() const
{
    return studentName;
}

void LocalDatabase::Parametre::setStudentName(const QString &newStudentName)
{
    studentName = newStudentName;
}

const QString &LocalDatabase::Parametre::getHostName() const
{
    return hostName;
}

void LocalDatabase::Parametre::setHostName(const QString &newHostName)
{
    hostName = newHostName;
}
