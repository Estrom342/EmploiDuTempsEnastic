#include "emploidatabase.h"
#include <QThread>

EmploiDatabase::EmploiDatabase(const QString &hostName, QString databaseName, QObject *parent) :
    QObject(parent), m_hostName(hostName), m_login(""), m_password(""), databaseName(databaseName), portName(3306)
{
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName(hostName);
    database.setUserName(m_login);
    database.setPassword(m_password);
    database.setDatabaseName(databaseName);
}

EmploiDatabase::EmploiDatabase(const QSqlDatabase &database)
{
    this->database = database;
}


//Test si la connexion est etablie avec la base de données
bool EmploiDatabase::isOpen()
{
    if(!database.isOpen()){
        emit critical("", tr("Connectez-vous dabord à la base de données"));
        emit disConnexionSignal();
        return false;
    }
    return true;
}

bool EmploiDatabase::open()
{
    return database.open();
}

//Cree la base de donnés et les tables depuis le fichier de ressource
bool EmploiDatabase::createDatabase(QString user, QString pass)
{
    qDebug()<<"EmploiDatabase::createDatabase : Creation de la base de données";
    if(!database.isOpen()){
        if(!database.open(user, pass)){
            qDebug()<<"Error at EmploiDatabase::createDatabase: "<<database.lastError().text();
            return false;
        }
    }
    QFile file(":/data/database.sql");
    if(file.open(QFile::ReadOnly)){
        QTextStream textStream(&file);
        auto lsRequestes = textStream.readAll().split(';');
        auto query = QSqlQuery(database);
        foreach (auto request, lsRequestes) {
            if(!query.exec(request))
                qDebug()<<"Error at EmploiDatabase::createDatabase: "<<query.lastError().text();
        }
    }else{
        qDebug()<<"Error at EmploiDatabase::createDatabase: "<<file.errorString();
        return false;
    }
    return true;
}

//Connexion à la base de données
bool EmploiDatabase::connectToHost(const QString &login, QString password, quint16 portName, const QString &hostName)
{
    qDebug()<<"EmploiDatabase::connectToHost thread = "<<QThread::currentThread();
    if(database.isOpen())
        database.close();
    //database.setPort(portName);
    database.setHostName(hostName);
    emit statut(tr("Connexion en cours..."));
    if(database.open(login, password)){
        this->m_password = password;
        this->m_login = login;
        this->m_hostName = hostName;
        emit statut("connecté");
        emit information("", "Connecté");
        emit connexionSignal();
        m_erreur = "";
    }else{
        emit critical("Erreur de connection", database.lastError().text());
        emit statut("Non connecté");
        emit disConnexionSignal();
        m_erreur = database.lastError().text();
        qDebug()<<"Error at EmploiDatabase::connectToHost: "<<m_erreur;
        return false;
    }
    qDebug()<<"NNNNNNNNNNNNNNNNNN";
    this->m_login = login; this->m_password = password; this->portName = portName;
    return true;
}

//Ajouter un niveau ou filier dans la base de donné
bool EmploiDatabase::addNiveau(const QString &nomNiveau, const QString &nomSalle)
{
    auto query = QSqlQuery(database);
    if(getLsLevelDelete().contains(nomNiveau)){
        query.prepare("UPDATE Niveau SET isDeleted = 0 WHERE nomNiveau = :nomNiveau;");
        query.bindValue(":nomNiveau", nomNiveau);

        if(query.exec()){
            emit information("", tr("Ajouter!"));
            emit connexionSignal(); return true;
        }
        qDebug()<<"Error at EmploiDatabase::addNiveau(UPATE): "<<query.lastError().text();
        return false;
    }
    if(getIdNiveau(nomNiveau) != -1) return false;
    query.prepare("INSERT INTO Niveau(nomNiveau, nomSalle)VALUES(:nomNiveau, :nomSalle);");
    query.bindValue(":nomNiveau", nomNiveau);
    query.bindValue(":nomSalle", nomSalle);
    if(query.exec()){
        emit information("", tr("Enregistré"));
        emit connexionSignal();
        return true;
    }
    qDebug()<<"Error at EmploiDatabase::addNiveau: "<<query.lastError().text();
    return false;
}

bool EmploiDatabase::deleteNiveau(QString const &nomNiveau)
{
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Niveau SET isDeleted = 1 WHERE nomNiveau = :nomNiveau;");
    query.bindValue(":nomNiveau", nomNiveau);
    if(query.exec()){
        emit connexionSignal();
        emit information("", "Supprimer!");
        return true;
    }
    qDebug()<<"Error at EmploiDatabase::deleteNiveau: "<<query.lastError().text();
    return false;
}
//Changer le nom d'un niveau
bool EmploiDatabase::updateNiveau(QString oldName, QString newName, QString nomSalle)
{
    if(!isOpen()) return false;
    if(getIdNiveau(newName) != -1){
        emit information("Auccune modification effectuée", tr("Ce niveau existe déjà"));
        return false;
    }
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Niveau SET nomNiveau =:newName, nomSalle = :nomSalle, isDeleted = 0 "
                  "WHERE nomNiveau = :oldName;");
    query.bindValue(":newName", newName);
    query.bindValue(":oldName", oldName);
    query.bindValue(":nomSalle", nomSalle);
    if(query.exec()){
        qDebug()<<" EmploiDatabase::updateNiveau. Old= "+oldName+" new = "+newName;
        emit information("", "Niveau modifié");
        emit connexionSignal();
        return true;
    }
    qDebug()<<"Error at EmploiDatabase::updateNiveau: "<<query.lastError().text();
    return false;
}

QString EmploiDatabase::getSalle(QString level)
{
    auto query = QSqlQuery(database);
    query.prepare("SELECT nomSalle FROM Niveau WHERE nomNiveau = :niveau;");
    query.bindValue(":niveau", level);
    if(query.exec()){
        while(query.next())
            return query.value("nomSalle").toString();
    }
    return "";
}

QString EmploiDatabase::getNiveau(QString nomSalle)
{
    auto query = QSqlQuery(database);
    query.prepare("SELECT nomNiveau FROM Niveau WHERE nomSalle = :nomSalle;");
    query.bindValue(":nomSalle", nomSalle);
    if(query.exec()){
        while(query.next())
            return query.value("nomSalle").toString();
    }
    return "";
}

//Retourn l'identifiant de niveau dans la base de données
int EmploiDatabase::getIdNiveau(QString niveau)
{
    if(!isOpen()) return -1;
    auto query = QSqlQuery(database);
    query.prepare("SELECT idNiveau FROM Niveau WHERE nomNiveau = :niveau AND isDeleted = 0;");
    query.bindValue(":niveau", niveau);
    if(query.exec()){
        while(query.next()){
            auto found = false;
            int id = query.value("idNiveau").toInt(&found);
            return found?id:-1;
        }
    }else
        qDebug()<<"Error at EmploiDatabase::getIdNiveau : "<<query.lastError().text();
    return -1;
}

//Ajout d'une matiere dans la base de données
bool EmploiDatabase::addStatictMatiere(const Matiere &matiere)
{
    if(!isOpen())
        return false;
    if(getIdMatiere(matiere) != -1){
        emit information("", tr("Cette matiere existe déjà en ")+ matiere.getNiveau());
        return false;
    }
    int idNiveau = getIdNiveau(matiere.getNiveau());
    if(idNiveau == -1){
        emit critical("Erreur", tr("Vous ne pouvez pas ajouter une matière pour une classe qui n'existe pas"));
        return false;
    }
    auto request = QSqlQuery(database);
    request.prepare("INSERT INTO MatiereStatique(nomMatiere, idNiveau) VALUES(:nomMatiere, :idNiveau)");
    request.bindValue(":nomMatiere", matiere.getName());
    request.bindValue(":idNiveau", idNiveau);

    if(request.exec()){
        emit information("", tr("Matière ajouté"));
        return true;
    }else
        qDebug()<<"Error at EmploiDatabase::addStatictMatiere: "<<request.lastError().text();
    return false;
}

bool EmploiDatabase::addMatiere(Matiere &matiere)
{
    int idMatiere = getIdMatiere(matiere);
    if(idMatiere == -1 && matiere.getIdEmploi() == -1)
        return false;
    auto query = QSqlQuery(database);
    query.prepare("INSERT INTO Matiere(idMatiereStatique, idEmploie, numCase, couleur, type_, commentaire)"
                               "VALUES(:idMatiere, :idEmploi, :numCase, :couleur, :type_, :commentaire);");
    query.bindValue(":idMatiere", idMatiere);
    query.bindValue(":idEmploi", matiere.getIdEmploi());
    query.bindValue(":numCase", matiere.getNumeroCase());
    query.bindValue(":couleur", matiere.getCouleur().name(QColor::HexRgb));
    query.bindValue(":type_", matiere.getType());
    query.bindValue(":commentaire", matiere.commentaire());
    if(query.exec()){
        matiere.setIdMatiere(getIdMatiere(matiere));
        auto lsTeacher = getlsTeacher(matiere.getNiveau());
        foreach (auto teacher, lsTeacher) {
            if(teacher.getNomMatiere() == matiere.getName()){
                matiere.setEnseignant(teacher);
                break;
            }
        }
        return true;
    }else{
        qDebug()<<"Error at EmploiDatabase::addMatiere : "<<query.lastError().text();
    }
    return false;
}

//Modifie le nom de la matiere
bool EmploiDatabase::updateStatictMatiere(const Matiere &ancienne, const Matiere &nouvelle)
{
    if(!isOpen())
        return false;
    int newIdNiveau = getIdNiveau(nouvelle.getNiveau());
    int oldIdNiveau = getIdNiveau(ancienne.getNiveau());
    if(newIdNiveau == -1 || oldIdNiveau == -1){
        emit critical("Erreur", tr("Vous ne pouvez pas mettre à jour une matière pour une classe qui n'existe pas"));
        return false;
    }
    auto request = QSqlQuery(database);
    request.prepare("UPDATE MatiereStatique SET nomMatiere = :newName , idNiveau = :newIdNiveau WHERE "
                    "nomMatiere = :oldName AND idNiveau = :oldIdNiveau;");
    request.bindValue(":newName", nouvelle.getName());
    request.bindValue(":newIdNiveau", newIdNiveau);
    request.bindValue(":oldName", ancienne.getName());
    request.bindValue(":oldIdNiveau", oldIdNiveau);
    if(request.exec()){
        emit information("", tr("Modification effectuée"));
        //return true;
    }else
        qDebug()<<"Error at EmploiDatabase::updateStatictMatiere: "<<request.lastError().text();
    //Mis a jour de la table Enseigner

    {
        auto request = QSqlQuery(database);
        int idNewEnseignant = getIdEnseignant(nouvelle.getEnseignant());
        int idOlEnseignant = getIdEnseignant(ancienne.getEnseignant());
        if(idOlEnseignant == -1 && idNewEnseignant != -1){
            request.prepare("INSERT INTO Enseigner(idMatiere, idEnseignant)"
                                    "VALUES(:newIdMatiere, :newIdEnseignant)");
        }
        else if(idOlEnseignant != -1 && idNewEnseignant != -1){
            request.prepare("UPDATE Enseigner SET idMatiere= :newIdMatiere, idEnseignant =:newIdEnseignant "
                                    "WHERE idMatiere= :oldIdMatiere AND idEnseignant =:oldIdEnseignant ;");
        }else if(idNewEnseignant == -1 && idOlEnseignant != 1){
            request.prepare("DELETE FROM Enseigner WHERE idMatiere= :newIdMatiere AND "
                             "idEnseignant =:oldIdEnseignant;");
        }
        else
            return true;
        request.bindValue(":newIdMatiere", getIdMatiere(nouvelle));
        request.bindValue(":newIdEnseignant", idNewEnseignant);
        request.bindValue(":oldIdMatiere", getIdMatiere(nouvelle));
        request.bindValue(":oldIdEnseignant", idOlEnseignant);
        if(request.exec()){
            emit information("", "Nom de l'enseignant changé");
            return true;
        }else
            qDebug()<<"Error at EmploiDatabase::updateStatictMatiere: "<<request.lastError().text();
        return false;
    }
}

//AJoute un emploi du temps
bool EmploiDatabase::addEmploie(EmploieDuTemp emploi)
{
    if(!isOpen())
        return false;
    int idEmploi = getIdEmploi(emploi);
    qDebug()<<"EmploiDatabase::addEmploie. idEmploi = "<<idEmploi;
    qDebug()<<"EmploiDatabase::addEmploie. emploi du "<<emploi.dateDebut().toString()<<" au "<<emploi.dateFin().toString();
    if(idEmploi != -1){
        emit information("", tr("Cet emploi du temps existe déjà"));
        return false;
    }
    int idNiveau = getIdNiveau(emploi.niveau());
    if(idNiveau == -1){
        emit critical("", tr("Vous ne pouvez pas ajouter un emploi pour un\n"
                             "niveau qui n'est pas encore enregistré"));
        return false;
    }
    if(emploi.lsMatieres().size() == 0){
        emit critical("Emploi non enregistré", tr("Vous ne pouvez pas ajouter un emploi qui ne contient aucunne matiere"));
        return false;
    }
    auto query = QSqlQuery(database);

    query.prepare("INSERT INTO Emploie(idNiveau, dateDebut, dateFin, dateCreation, lieuCreation, signature, semestre)"
                         "VALUES(:idNiveau, :dateDebut, :dateFin, :dateCreation, :lieuCreation, :signature, :semestre);");
    query.bindValue(":idNiveau", idNiveau);
    query.bindValue(":dateDebut", emploi.dateDebut());
    query.bindValue(":dateFin", emploi.dateFin());
    query.bindValue(":dateCreation", emploi.dateCreation());
    query.bindValue(":lieuCreation", emploi.lieuDeCreation());
    query.bindValue(":signature", emploi.signature());
    query.bindValue(":semestre", emploi.semestre());
    if(!query.exec()){
        qDebug()<<"Error at  EmploiDatabase::addEmploie: "<<query.lastError().text();
        return false;
    }
    idEmploi = getIdEmploi(emploi);
    int nbrMatiere=0;int i=0;
    foreach (auto matiere, emploi.lsMatieres()) {
        int idMatiere = getIdMatiere(matiere);
        if(idMatiere == -1)
            continue;
        matiere.setIdMatiere(idMatiere);
        matiere.setIdEmploi(idEmploi);
        auto query = QSqlQuery(database);
        query.prepare("INSERT INTO Matiere(idMatiereStatique, idEmploie, numCase, couleur, type_)"
                                   "VALUES(:idMatiere, :idEmploi, :numCase, :couleur, :type_);");
        query.bindValue(":idMatiere", matiere.getIdMatiere());
        query.bindValue(":idEmploi", idEmploi);
        query.bindValue(":numCase", matiere.getNumeroCase());
        query.bindValue(":couleur", matiere.getCouleur().name(QColor::HexRgb));
        query.bindValue(":type_", matiere.getType());
        if(query.exec()){
            nbrMatiere++;
        }else{
            qDebug()<<"Error at EmploiDatabase::addEmploie : "<<query.lastError().text();
        }
        qDebug()<<++i;
    }
    emit information("", tr("Emploi enregistré"));
    if(nbrMatiere == 0)
        emit information("", "Auccune matière enregistrée dans cet emploi");
    qDebug()<<nbrMatiere;
    return true;
}

//Met à jour le semestre de l'emploi
bool EmploiDatabase::updateEmploie(int idEmploi, const QString &semestre)
{
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Emploie SET semestre = :semestre WHERE idEmploie = :idEmploie;");
    query.bindValue(":semestre", semestre);
    query.bindValue(":idEmploie", idEmploi);
    if(query.exec())
        return true;
    qDebug()<<"Error at EmploiDatabase::updateEmploie : "<<query.lastError().text();
    return false;
}

//Modifie l'emploi du temps
bool EmploiDatabase::updateMatiere(Matiere &ancienne, Matiere &nouvelle)
{
    int newIdMatiere = getIdMatiere(nouvelle);
    int idMatiere = getIdMatiere(ancienne);

    if(idMatiere == -1){
        return addMatiere(nouvelle);
    }
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Matiere SET idMatiereStatique = :idMatiere, couleur= :couleur, type_ = :type, "
                   "commentaire= :commentaire WHERE idEmploie = :idEmploie AND numCase = :numCase;");
    query.bindValue(":idMatiere", newIdMatiere);
    query.bindValue(":couleur", nouvelle.getCouleur().name(QColor::HexRgb));
    query.bindValue(":type", nouvelle.getType());
    query.bindValue(":idEmploie", ancienne.getIdEmploi());
    query.bindValue(":numCase", ancienne.getNumeroCase());
    query.bindValue(":commentaire", nouvelle.commentaire());

    if(!query.exec()){
        qDebug()<<"Error at EmploiDatabase::updateMatiere: "<<query.lastError().text();
        return false;
    }
    auto lsTeacher = getlsTeacher(nouvelle.getNiveau());
    foreach (auto teacher, lsTeacher) {
        if(teacher.getNomMatiere() == nouvelle.getName()){
            nouvelle.setEnseignant(teacher);
            break;
        }
    }
    return true;
}

//Ajoute l'horaire dans l'emploi du temps
bool EmploiDatabase::addHoraire(const Horaire &horaire)
{
    if(horaire.idEmploi() == -1){
        qDebug()<<"Error at EmploiDatabase::addHoraire : idEmploi negatif";
        return false;
    }
    auto query = QSqlQuery(database);
    query.prepare("INSERT INTO Horaire(numCase, heureDebut, heureFin, idEmploie)"
                               "VALUES(:numCase, :heureDebut, :heureFin, :idEmploie);");
    query.bindValue(":numCase", horaire.numeroCase());
    query.bindValue(":heureDebut", horaire.heureDebut());
    query.bindValue(":heureFin", horaire.heureFin());
    query.bindValue(":idEmploie", horaire.idEmploi());
    if(!query.exec()){
        qDebug()<<"Error at EmploiDatabase::addHoraire : "<<query.lastError().text();
        return false;
    }
    return true;
}
//Met a jour l'horair de l'emploi
bool EmploiDatabase::updateHoraire(const Horaire &horaire)
{
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Horaire SET heureDebut = :heureDebut, heureFin = :heureFin "
                    "WHERE idEmploie = :idEmploie AND numCase = :numCase;");
    query.bindValue(":heureDebut", horaire.heureDebut());
    query.bindValue(":heureFin", horaire.heureFin());
    query.bindValue(":idEmploie", horaire.idEmploi());
    query.bindValue(":numCase", horaire.numeroCase());
    qDebug()<<"EmploiDatabase::updateHoraire : idEmploi: "<<horaire.idEmploi();
    qDebug()<<" NumCase: "<<horaire.numeroCase()<<horaire.heureDebut()<<" à "<<horaire.heureFin();
    if(!query.exec()){
        qDebug()<<"Error at EmploiDatabase::updateHoraire : "<<query.lastError().text();
        return false;
    }
    return true;
}

//Supprimer une matiere de l'emploi du temp
bool EmploiDatabase::deleteMatiere(const Matiere &matiere)
{
    auto query = QSqlQuery(database);
    query.prepare("DELETE FROM Matiere WHERE idEmploie = :idEmploie AND numCase = :numCase;");
    query.bindValue(":idEmploie", matiere.getIdEmploi());
    query.bindValue(":numCase", matiere.getNumeroCase());
    if(query.exec())
        return true;
    else
        qDebug()<<"Error at EmploiDatabase::deleteMatiere: "<<query.lastError().text();
    return false;
}

//Ajoute un enseignant dans la base de données
bool EmploiDatabase::addTeacher(const Teacher &t)
{
    if(!isOpen())
        return false;
    int idEnseignant = getIdEnseignant(t);
    auto query = QSqlQuery(database);
    if(idEnseignant == -1){
        query.prepare("INSERT INTO Enseignant(nomEnseignant, prenomEnseignant, grade)"
                                    "VALUES(:nom, :prenom, :grade);");
        query.bindValue(":nom", t.getNom());
        query.bindValue(":prenom", t.getPrenom());
        query.bindValue(":grade", t.getGrade());
        if(!query.exec()){
            qDebug()<<"Error at EmploiDatabase::addTeacher"<<query.lastError().text();
            return false;
        }
        //Recuperation de l'identifiant d l' enseignant pour ajouter a la table "Enseigner"
        {
            if(query.exec("SELECT LAST_INSERT_ID() AS idEnseignant;")){
                auto found = false;
                while(query.next()){
                    idEnseignant = query.value("idEnseignant").toInt(&found);
                    if(found) break;
                }
            }else
                qDebug()<<"Error at EmploiDatabase::addTeacher : "<<query.lastError().text();
        }
    }else{
        emit information("", tr("Cet enseignant exist  déjà pour cette matière"));
        return false;
    }
    Matiere matiere(t.getNomMatiere()); matiere.setNiveau(t.getNiveau());
    int idMatiere = getIdMatiere(matiere);

    //On verifie si l'enseignant n'a pas ete supprimé
    query.prepare("SELECT idEnseignant FROM Enseigner WHERE idMatiere= :idMatiere AND "
                   "idEnseignant= :idEnseignant AND dateFin IS NOT NULL;");
    query.bindValue(":idMatiere", idMatiere);
    query.bindValue(":idEnseignant", idEnseignant);
    if(query.exec()){
        while(query.next()){
            int id = query.value("idEnseignant").toInt();
            if(id == idEnseignant){
                emit information("", tr("Ces information ont déjà été enregistrées"));
                return false;
            }break;
        }
    }

    //Insertion de données dans la tables Enseigner
    query.prepare("INSERT INTO Enseigner(idMatiere, idEnseignant)VALUES(:idMatiere, :idEnseignant);");
    query.bindValue(":idMatiere", idMatiere);
    query.bindValue(":idEnseignant", idEnseignant);
    if(query.exec()){
        emit information("", "Enregistré");
        return true;
    }else
        qDebug()<<"Error at EmploiDatabase::addTeacher"<<query.lastError().text();
    return false;
}

//Supprimer un enseignant de la base de donnée
bool EmploiDatabase::deleteTeacher(const Teacher &t)
{
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Enseignant SET isActived = 0 WHERE idEnseignant = :idEnseignant;");
    query.bindValue(":idEnseignant", t.getId());
    if(query.exec()){
        query.prepare("UPDATE Enseigner SET dateFin = :dateFin WHERE idEnseignant = :idEnseignant;");
        query.bindValue(":dateFin", QDate::currentDate());
        query.bindValue(":idEnseignant", t.getId());
        if(query.exec()){
            emit information("", tr("%1 supprimer!").arg(t.toString()));
            return true;
        }else
            qDebug()<<"Error at EmploiDatabase::deleteTeacher(Updatting Enseigner's table): "<<query.lastError().text();
        return false;
    }
    qDebug()<<"Error at EmploiDatabase::deleteTeacher: "<<query.lastError().text();
    return true;
}

//Modifier les information des enseignants
bool EmploiDatabase::updateTeacher(const Teacher &oldTeacher, const Teacher &newTeacher)
{
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Enseignant SET nomEnseignant = :nomEnseignant, prenomEnseignant = :prenomEnseignant,"
                   "grade = :grade WHERE nomEnseignant = :oldNomEnseignant AND prenomEnseignant = :oldPrenomEnseignant AND "
                    " grade = :oldGrade AND isActived = 1;");
    query.bindValue(":nomEnseignant", newTeacher.getNom());
    query.bindValue(":prenomEnseignant", newTeacher.getPrenom());
    query.bindValue(":grade", newTeacher.getGrade());
    query.bindValue(":oldNomEnseignant", oldTeacher.getNom());
    query.bindValue(":oldPrenomEnseignant", oldTeacher.getPrenom());
    query.bindValue(":oldGrade", oldTeacher.getGrade());
    if(query.exec()){
        emit information(tr("Modification d'un enseignant"), tr("Modifier!"));
        return true;
    }
    qDebug()<<"Error at EmploiDatabase::updateTeacher: "<<query.lastError().text();
    return false;
}

//Retourne la liste des matieres selon le niveau level
QList<Matiere> EmploiDatabase::getlsStaticMatiere(const QString &level)
{
    /*CREATE OR REPLACE VIEW MatiereStaticeView AS SELECT nomMatiere, nomEnseigant, nomNiveau AS niveau,
    FROM MatiereStatique, Enseignant, Niveau;*/
    QList<Matiere> lsMatiere;lsMatiere.clear();
    auto query = QSqlQuery(database);
    query.prepare("SELECT * FROM MatiereStaticeView WHERE niveau = :level ORDER BY nomMatiere;");
    query.bindValue(":level", level);
    if(level.isEmpty())
        query.prepare("SELECT * FROM MatiereStaticeView ORDER BY nomMatiere;");
    if(query.exec()){
        while(query.next()){
            auto teacher = Teacher(query.value("nomEnseignant").toString(), query.value("prenomEnseignant").toString(),
                                   query.value("grade").toString());
            teacher.setNiveau(level);
            auto matiere = Matiere(query.value("nomMatiere").toString());
            matiere.setEnseignant(teacher);
            matiere.setNiveau(query.value("niveau").toString());
            lsMatiere.append(matiere);
        }
    }else{
        qDebug()<<"Error at EmploiDatabase::getlsStaticMatiere: "<<query.lastError().text();
    }
    return lsMatiere;
}

//Recupere la matiere enseignée par teacher
QString EmploiDatabase::getMatiereName(const Teacher &teacher, const QString &level)
{
    auto query = QSqlQuery(database);
    query.prepare("SELECT nomMatiere FROM MatiereStaticeView WHERE idEnseignant = :idEnseignant AND niveau = :nomNiveau;");
    query.bindValue(":idEnseignant", teacher.getId());
    query.bindValue(":nomNiveau", level);
    if(query.exec()){
        while(query.next())
            return query.value("nomMatiere").toString();
    }else
        qDebug()<<"Error at EmploiDatabase::getMatiereName: "<<query.lastError().text();
    return "";
}

//Retourne la liste des enseignant selon le niveau level
QList<Teacher> EmploiDatabase::getlsTeacher(const QString &level)
{
    /*CREATE OR REPLACE VIEW EnseignantView AS SELECT nomEnseigant, prenomEnseignant, grade, nomMatiere, nomNiveau
    FROM Enseignant, Matiere, Niveau;*/
    QList<Teacher> lsTeacher; lsTeacher.clear();
    if(!isOpen())
        return lsTeacher;
    auto query = QSqlQuery(database);
    query.prepare("SELECT * FROM EnseignantView WHERE nomNiveau=:level AND isActived = 1 ORDER BY nomEnseignant;");
    if(level.isEmpty())
        query.prepare("SELECT * FROM EnseignantView WHERE isActived = 1 ORDER BY nomEnseignant;");
    query.bindValue(":level", level);
    if(query.exec()){
        while(query.next()){
            Teacher teacher(query.value("nomEnseignant").toString(), query.value("prenomEnseignant").toString(),
                            query.value("grade").toString(), query.value("nomNiveau").toString());
            teacher.setNomMatiere(query.value("nomMatiere").toString());
            teacher.setId(query.value("idEnseignant").toInt());
            lsTeacher.append(Teacher(teacher));
        }
    }else{
        qDebug()<<"Error at EmploiDatabase::getlsTeacher: "<<query.lastError().text();
    }
    return lsTeacher;
}

QList<Teacher> EmploiDatabase::getlsTeacher()
{
    QList<Teacher> lsTeacher; lsTeacher.clear();
    if(!isOpen())
        return lsTeacher;
    auto query = QSqlQuery(database);
    if(query.exec("SELECT * FROM EnseignantView WHERE isActived = 1 ORDER BY nomEnseignant;")){
        while(query.next()){
            Teacher teacher(query.value("nomEnseignant").toString(), query.value("prenomEnseignant").toString(),
                            query.value("grade").toString(), query.value("nomNiveau").toString());
            teacher.setNomMatiere(query.value("nomMatiere").toString());
            teacher.setId(query.value("idEnseignant").toInt());
            lsTeacher.append(Teacher(teacher));
        }
    }else{
        qDebug()<<"Error at EmploiDatabase::getlsTeacher: "<<query.lastError().text();
    }
    return lsTeacher;
}

//Retourne la liste de tous les niveaux
QStringList EmploiDatabase::getLsLevel()
{
    QStringList lsFiliere; lsFiliere.clear();
    auto query = QSqlQuery(database);
    if(query.exec("SELECT nomNiveau FROM Niveau WHERE isDeleted = 0;")){
        while(query.next()){
            lsFiliere.append(query.value("nomNiveau").toString());
        }
    }else{
        qDebug()<<"Error at EmploiDatabase::getLsLevel : "<<query.lastError().text();
    }
    return lsFiliere;
}

QStringList EmploiDatabase::getLsSalles()
{
    QStringList lsSalle; lsSalle.clear();
    auto query = QSqlQuery(database);
    if(query.exec("SELECT DISTINCT nomSalle FROM Niveau;")){
        while(query.next()){
            lsSalle<<query.value("nomSalle").toString();
        }
    }else{
        qDebug()<<"Error at EmploiDatabase::getLsLevelWithClass : "<<query.lastError().text();
    }
    return lsSalle;
}

QStringList EmploiDatabase::getLsLevelDelete()
{
    QStringList lsFiliere; lsFiliere.clear();
    auto query = QSqlQuery(database);
    if(query.exec("SELECT nomNiveau FROM Niveau WHERE isDeleted = 1;")){
        while(query.next()){
            lsFiliere.append(query.value("nomNiveau").toString());
        }
    }else{
        qDebug()<<"Error at EmploiDatabase::getLsLevel : "<<query.lastError().text();
    }
    return lsFiliere;
}

//Recupere la liste de tous les utilisateurs de la base de donnée
QStringList EmploiDatabase::getLsAdmin()
{
    QStringList lsAdmin; lsAdmin.clear();
    auto query = QSqlQuery(database);
    if(query.exec("SELECT User FROM mysql.user WHERE User NOT IN('mariadb.sys', 'mysql', 'root');")){
        while(query.next())
            lsAdmin<<query.value("User").toString();
    }else
        qDebug()<<"Error at EmploiDatabase::getLsAdmin : "<<query.lastError().text();
    return lsAdmin;
}

//Ajout un adminitrateur a la base de donné
bool EmploiDatabase::addAdmin(QString login, QString password)
{
    auto query = QSqlQuery(database);    
    query.prepare("GRANT ALL ON *.* TO :login@'%' IDENTIFIED BY :password WITH GRANT OPTION;");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if(query.exec()){
        emit information(tr("Ajout d'un administrateur"), tr("Enregistrer!"));
        return true;
    }
    emit critical("Erreur d'ajout", query.lastError().text());
    return false;
}

//Supprime un addministrateur
bool EmploiDatabase::deleteAdmin(QString login)
{
    auto query=  QSqlQuery(database);
    query.prepare("DROP USER :login@'%';");
    query.bindValue(":login", login);
    if(query.exec()){
        emit information("", tr("%1 supprimé").arg(login));
        return true;
    }
    else
        emit information(tr("Erreur de suppression"), query.lastError().text());
    return false;
}

//Changer le mot de passe de l'adminitrateur courant
bool EmploiDatabase::changePassword(QString password)
{
    auto query = QSqlQuery(database);
    query.prepare("SET PASSWORD FOR CURRENT_USER = PASSWORD(:password);");
    query.bindValue(":password", password);
    if(query.exec()){
        emit information("", tr("Mot de passe changer!"));
        return true;
    }
    emit critical("De changement de mot de passe", query.lastError().text());
    return false;
}

//Retourne l'emploi du temps du dateDebut au dateFin de la classe niveau
EmploieDuTemp EmploiDatabase::getEmploi(const QString &niveau, const QDate &dateDebut, const QDate &dateFin)
{
    EmploieDuTemp emploi(niveau);
    emploi.setDateDebut(dateDebut);
    QList<Matiere> lsMatiere; lsMatiere.clear();
    auto query = QSqlQuery(database);
    query.prepare("SELECT * FROM EmploiView WHERE dateDebut= :dateDebut AND dateFin= :dateFin AND "
                                        "niveau= :niveau ORDER BY numeroCase;");
    query.bindValue(":dateDebut", dateDebut);
    query.bindValue(":dateFin", dateFin);
    query.bindValue(":niveau", niveau);
    int idEmploi = -1;
    if(query.exec()){
        int i=0; bool ok;
        while(query.next()){
            auto matiere = Matiere(query.value("nomMatiere").toString());
            matiere.setNomEnseigant(query.value("nomEnseignant").toString());
            matiere.setType(query.value("typeMatiere").toString());
            matiere.setNiveau(query.value("niveau").toString());
            matiere.setCouleur(QColor(query.value("couleur").toString()));
            matiere.setNumeroCase(query.value("numeroCase").toInt());
            matiere.setIdEmploi(query.value("idEmploi").toInt());
            matiere.setIdMatiere(query.value("idMatiereStatique").toInt());
            matiere.setCommentaire(query.value("commentaire").toString());
            matiere.setEnseignant(Teacher(matiere.getNomEnseigant(), query.value("prenomEnseignant").toString(),
                                          query.value("grade").toString(), matiere.getNiveau()));
            lsMatiere.append(matiere);
            //Emploie.dateCreation, Emploie.lieuCreation, Emploie.signature
            if(i == 0){
                idEmploi = query.value("idEmploi").toInt(&ok);
                emploi.setDateCreation(query.value("dateCreation").toDate());
                emploi.setLieuDeCreation(query.value("lieuCreation").toString());
                emploi.setSignature(getSignatureEmploi(niveau, dateDebut, dateFin));
                emploi.setSemestre(query.value("semestre").toString());
                emploi.setNomSalle(query.value("nomSalle").toString());
                if(ok)
                    i++;
                else idEmploi= -1;
            }
        }
        emploi.setIdEmploi(idEmploi);
        //Recupertion des horaires de l'emploi
         query.prepare("SELECT * FROM Horaire WHERE idEmploie = :idEmploie;");
        query.bindValue(":idEmploie", idEmploi);
        if(query.exec()){
            QList<Horaire> lsHoraires;
            while(query.next()){
                Horaire horaire;
                horaire.setNumeroCase(query.value("numCase").toInt());
                horaire.setHeureDebut(query.value("heureDebut").toTime());
                horaire.setHeureFin(query.value("heureFin").toTime());
                horaire.setIdEmploi(idEmploi);
                lsHoraires<<horaire;
            }
            emploi.setLsHoraire(lsHoraires);
        }else
            qDebug()<<"Error at EmploiDatabase::getEmploi (get ls horaire): "<<query.lastError().text();

    }else{
        qDebug()<<"Error at EmploiDatabase::getEmploi : "<<query.lastError().text();
    }
    emploi.setLsMatieres(lsMatiere);
    emploi.setIdEmploi(idEmploi);
    return emploi;
}

QList<EmploieDuTemp> EmploiDatabase::lsEmplois()
{
    QList<EmploieDuTemp> lsEmplois;
    QList<int> lsId;
    auto query = QSqlQuery(database);
    //Selection des emploi  du temps
    if(query.exec("SELECT idEmploie, createur, nomNiveau, nomSalle, dateDebut, dateFin, dateCreation, lieuCreation, semestre "
                  "FROM Emploie LEFT JOIN Niveau ON Emploie.idNiveau = Niveau.idNiveau "
                  "ORDER BY Emploie.idEmploie DESC;")){
        while (query.next()) {
            EmploieDuTemp em;
            em.setNiveau(query.value("nomNiveau").toString());
            em.setNomSalle(query.value("nomSalle").toString());
            em.setDateDebut(query.value("dateDebut").toDate());
            em.setDateFin(query.value("dateFin").toDate());
            em.setDateCreation(query.value("dateCreation").toDate());
            em.setLieuDeCreation(query.value("lieuCreation").toString());
            em.setSemestre(query.value("semestre").toString());
            em.setCreateur(query.value("createur").toString());
            if(em.createur().split("@").size()>=2)
                em.setCreateur(em.createur().split("@").first());
            bool ok = false;
            int id = query.value("idEmploie").toInt(&ok);
            if(ok){
                lsId.append(id);
                em.setLsHoraire(lsHorairs(id));
                em.setLsMatieres(lsMatieres(id));
            }
            lsEmplois.append(em);
        }
    }else
        qDebug()<<"Error at EmploiDatabase::lsEmplois(): "<<query.lastError().text();
    return lsEmplois;
}

QList<Horaire> EmploiDatabase::lsHorairs(int idEmploi)
{
    QList<Horaire> lsH;
    if(idEmploi < 0) return lsH;
    //Recupertion des horaires de l'emploi
    auto query = QSqlQuery(database);
     query.prepare("SELECT * FROM Horaire WHERE idEmploie = :id;");
    query.bindValue(":id", idEmploi);
    if(query.exec()){
        QList<Horaire> lsHoraires;
        while(query.next()){
            Horaire horaire;
            horaire.setNumeroCase(query.value("numCase").toInt());
            horaire.setHeureDebut(query.value("heureDebut").toTime());
            horaire.setHeureFin(query.value("heureFin").toTime());
            horaire.setIdEmploi(idEmploi);
            lsHoraires<<horaire;
        }
    }else
        qDebug()<<"Error at EmploiDatabase::lsHorairs(int idEmploi): "<<query.lastError().text();
    return lsH;
}

QList<Matiere> EmploiDatabase::lsMatieres(int idEmploi)
{
    QList<Matiere> lsM;
    if(idEmploi < 0) return lsM;
    auto query = QSqlQuery(database);
    query.prepare("SELECT * FROM EmploiView WHERE idEmploi = :id;");
    query.bindValue(":id", idEmploi);
    if(query.exec()){
        while(query.next()){
            EmploieDuTemp emploi;
            auto matiere = Matiere(query.value("nomMatiere").toString());
            matiere.setNomEnseigant(query.value("nomEnseignant").toString());
            matiere.setType(query.value("typeMatiere").toString());
            matiere.setNiveau(query.value("niveau").toString());
            matiere.setCouleur(QColor(query.value("couleur").toString()));
            matiere.setNumeroCase(query.value("numeroCase").toInt());
            matiere.setIdEmploi(query.value("idEmploi").toInt());
            matiere.setIdMatiere(query.value("idMatiereStatique").toInt());
            matiere.setCommentaire(query.value("commentaire").toString());
            matiere.setEnseignant(Teacher(matiere.getNomEnseigant(), query.value("prenomEnseignant").toString(),
                                          query.value("grade").toString(), matiere.getNiveau()));
            lsM.append(matiere);
        }
    }else
        qDebug()<<"Error at EmploiDatabase::lsMatieres(int idEmploi): "<<query.lastError().text();
    return lsM;
}

EmploieDuTemp EmploiDatabase::getLastEmploi(const QString &niveau)
{
    auto query = QSqlQuery(database);
    query.prepare("SELECT MAX(dateDebut) AS debut, MAX(dateFin) AS fin FROM EmploiView WHERE niveau= :niveau;");
    query.bindValue(":niveau", niveau);
    if(query.exec()){
        while(query.next()){
            auto dateDebut = query.value("debut").toDate();
            auto dateFin = query.value("fin").toDate();
            if(dateDebut.isValid() && dateFin.isValid())
                return getEmploi(niveau, dateDebut, dateFin);
        }
    }else
        qDebug()<<"Error at EmploiDatabase::getLastEmploi(const QString &niveau): "<<query.lastError().text();
    EmploieDuTemp em(niveau);
    em.setLsMatieres(QList<Matiere>());
    return em;
}

QByteArray EmploiDatabase::getSignatureEmploi(const QString &level, const QDate &dateDebut, const QDate &dateFin)
{
    auto query = QSqlQuery(database);
    int idNiveau = getIdNiveau(level);
    if(idNiveau == -1)
        qDebug()<<"Error at EmploiDatabase::getSignatureEmploi: idNiveau"<<level<<"negatif";
    query.prepare("SELECT signature FROM Emploie WHERE idNiveau = :idNiveau AND "
                   "dateDebut = :dateDebut AND dateFin = :dateFin;");
    query.bindValue(":idNiveau", idNiveau);
    query.bindValue(":dateDebut", dateDebut);
    query.bindValue(":dateFin", dateFin);
    if(query.exec()){
        while(query.next()){
            return query.value("signature").toByteArray();
        }
    }else
        qDebug()<<"Error at EmploiDatabase::getSignatureEmploi: "<<query.lastError().text();
    qDebug()<<"EmploiDatabase::getSignatureEmploi: non trouvé"<<level<<dateDebut<<dateFin<<idNiveau;
    return QByteArray();
}

//Retourne l'identifiant de l;emploi
int EmploiDatabase::getIdEmploi(const EmploieDuTemp &emploi)
{
    int idNiveau = getIdNiveau(emploi.niveau());
    auto query = QSqlQuery(database);
    query.prepare("SELECT idEmploie FROM Emploie WHERE dateDebut = :dateDebut AND dateFin = :dateFin AND idNiveau = :idNiveau;");

    query.bindValue(":dateDebut", emploi.dateDebut());
    query.bindValue(":dateFin", emploi.dateFin());
    query.bindValue(":idNiveau", idNiveau);
    if(query.exec()){
        while(query.next()){
            auto found = false;
            int id = query.value("idEmploie").toInt(&found);
            return found?id:-1;
        }
    }else
        qDebug()<<"Error at EmploiDatabase::getIdEmploi : "<<query.lastError().text();
    return -1;
}

int EmploiDatabase::getIdMatiere(const Matiere &matiere)
{
    auto query = QSqlQuery(database);
    query.prepare("SELECT idMatiere FROM MatiereStatique LEFT JOIN Niveau USING (idNiveau) "
                   "WHERE nomMatiere = :nomMatiere AND nomNiveau = :nomNiveau;");
    query.bindValue(":nomMatiere", matiere.getName());
    query.bindValue(":nomNiveau", matiere.getNiveau());
    if(query.exec()){
        while(query.next()){
            auto found = false;
            int id = query.value("idMatiere").toInt(&found);
            return found?id:-1;
        }
    }else
        qDebug()<<"Error at EmploiDatabase::getIdMatiere : "<<query.lastError().text();
    return -1;
}

int EmploiDatabase::getIdEnseignant(const Teacher &t)
{
    auto query = QSqlQuery(database);

    query.prepare("SELECT idEnseignant FROM EnseignantView WHERE nomEnseignant = :nom AND nomNiveau = :nomNiveau AND "
                    "prenomEnseignant = :prenom AND grade = :grade AND nomMatiere = :nomMatiere AND isActived = 1;");
    query.bindValue(":nom", t.getNom());
    query.bindValue(":prenom", t.getPrenom());
    query.bindValue(":grade", t.getGrade());
    query.bindValue(":nomMatiere", t.getNomMatiere());
    query.bindValue(":nomNiveau", t.getNiveau());
    if(query.exec()){
        while(query.next()){
            auto found = false;
            int id = query.value("idEnseignant").toInt(&found);
            return found?id:-1;
        }
    }else
        qDebug()<<"Error at EmploiDatabase::getIdEnseignant : "<<query.lastError().text();
    return -1;
}

const QSqlDatabase &EmploiDatabase::getDatabase() const
{
    return database;
}

void EmploiDatabase::close()
{
    if(!database.isOpen())
        database.close();
}

void EmploiDatabase::setDatabase(const QSqlDatabase &newDatabase)
{
    database = newDatabase;
}

const QString &EmploiDatabase::hostName() const
{
    return m_hostName;
}

void EmploiDatabase::setHostName(const QString &newHostName)
{
    m_hostName = newHostName;
}

const QString &EmploiDatabase::login() const
{
    return m_login;
}

void EmploiDatabase::setLogin(const QString &newLogin)
{
    m_login = newLogin;
}

const QString &EmploiDatabase::password() const
{
    return m_password;
}

void EmploiDatabase::setPassword(const QString &newPassword)
{
    m_password = newPassword;
}

QString EmploiDatabase::errorConnexion()
{
    return m_erreur;
}

bool EmploiDatabase::updateSignature(const QByteArray &signature, int idEmploi)
{
    if(idEmploi == -1) return false;
    auto query = QSqlQuery(database);
    query.prepare("UPDATE Emploie SET signature = :signature WHERE idEmploie = :idEmploi;");
    query.bindValue(":signature", QVariant(signature), QSql::Binary);
    query.bindValue(":idEmploi", idEmploi);
    if(!query.exec()){
        qDebug()<<"Error at EmploiDatabase::updateSignature: "<<query.lastError().text();
        return false;
    }
    return true;
}

//Retourne un enseignant qui enseigne la matiere nomMatiere en niveau
Teacher EmploiDatabase::getTeacher(const QString &nomMatiere, const QString &niveau)
{
    auto query = QSqlQuery(database);
    query.prepare("SELECT nomEnseignant, prenomEnseignant, grade, nomMatiere, nomNiveau, "
                   "idEnseignant FROM EnseignantView WHERE nomMatiere = :nomMatiere AND "
                   "nomNiveau = :nomNiveau AND isActived = 1;");
    query.bindValue(":nomMatiere", nomMatiere);
    query.bindValue(":nomNiveau", niveau);
    if(query.exec()){
        Teacher t("", "");
        while(query.next()){
            t.setNom(query.value("nomEnseignant").toString());
            t.setPrenom(query.value("prenomEnseignant").toString());
            t.setGrade(query.value("grade").toString());
            t.setNiveau(query.value("nomNiveau").toString());
            t.setNomMatiere(query.value("nomMatiere").toString());
            t.setId(query.value("idEnseignant").toInt());
            return t;
        }
    }
    return Teacher("","");
}
