#include "emploiedutemp.h"

EmploieDuTemp::EmploieDuTemp(QString niveau, QString semestre): m_niveau(niveau), m_semestre(semestre), m_idEmploi(-1)
{
    m_lsMatieres.clear();
    m_lsHoraire.clear();
    for(int i=0; i< 5; i++){
        Horaire h;
        h.setNumeroCase(i+1);
        m_lsHoraire<<h;
    }
    for(int i=0; i< 18; i++) {
        Matiere m; m.setNiveau(niveau); m.setNumeroCase(i+1);
        m_lsMatieres.append(m);
    }
    int lundi= QDate::currentDate().dayOfWeek(), i=0;
    while(lundi % Qt::Sunday != Qt::Monday){
        lundi++; i++;
    }
    //setDateDebut(QDate::currentDate().addDays(i));
    m_dateCreation = QDate::currentDate();
    m_dateDebut = QDate(QDate::currentDate().year(), QDate::currentDate().month(), Qt::Monday);
    m_dateFin = QDate(QDate::currentDate().year(), QDate::currentDate().month(), Qt::Saturday);
    m_lieuDeCreation = "";
}

//Ajouter une matiere à l'emploi du temps
void EmploieDuTemp::addMatiere(Matiere &matiere)
{
    m_lsMatieres.append(matiere);
}

//Actualise la matiere
void EmploieDuTemp::updateMatiere(Matiere &matiere)
{
    for(int i= 0, j= m_lsMatieres.size(); i< j; i++){
        if(m_lsMatieres.at(i).getNumeroCase() == matiere.getNumeroCase()){
            m_lsMatieres[i] = matiere;
        }
    }
}

const QList<Matiere> &EmploieDuTemp::lsMatieres() const
{
    return m_lsMatieres;
}

void EmploieDuTemp::setLsMatieres(const QList<Matiere> &newLsMatieres)
{
    m_lsMatieres = newLsMatieres;
}

const QString &EmploieDuTemp::niveau() const
{
    return m_niveau;
}

void EmploieDuTemp::setNiveau(const QString &newNiveau)
{
    m_niveau = newNiveau;
    for(int i=0, j=m_lsMatieres.size(); i<j; i++){
        m_lsMatieres[i].setNiveau(m_niveau);
    }
}

const QDate &EmploieDuTemp::dateDebut() const
{
    return m_dateDebut;
}

void EmploieDuTemp::setDateDebut(const QDate &newDateDebut)
{
    m_dateDebut = newDateDebut;
}
void EmploieDuTemp::setDateFin(const QDate &newDateDebut)
{
    m_dateFin = newDateDebut;
}

const QDate &EmploieDuTemp::dateFin() const
{
    return m_dateFin;
}

QString EmploieDuTemp::dateToString(QDate date)
{
    if(!date.isValid()) return "";
    QVector<QString> days; days.clear();
    QVector<QString> moth; moth.clear();
    days<<"lundi"<<"mardi"<<"Mercrédi"<<"jeudi"<<"vendredi"<<"samedi"<<"dimanche";
    moth<<"janvier"<<"février"<<"mars"<<"avril"<<"mai"<<"juin"<<"juillet"<<"aout"<<"septembre"<<
          "actobre"<<"novembre"<<"décembre";
    QString txt = days[date.dayOfWeek()-1]+QString(" %1").arg(date.day());
    if(date.day() == 1) txt+="er";
    txt+= QString(" %1 %2").arg(moth[date.month()-1]).arg(date.year());
    return txt;
}

int EmploieDuTemp::idEmploi() const
{
    return m_idEmploi;
}

void EmploieDuTemp::setIdEmploi(int newIdEmploi)
{
    m_idEmploi = newIdEmploi;
    for(int i=0, j= m_lsMatieres.size(); i<j; i++){
        m_lsMatieres[i].setIdEmploi(m_idEmploi);
    }
    for(int i=0; i<m_lsHoraire.size(); i++)
        m_lsHoraire[i].setIdEmploi(newIdEmploi);
}

QDate EmploieDuTemp::currentMonday()
{
    int lundi= QDate::currentDate().dayOfWeek(), i=0;
    while(lundi % Qt::Sunday != Qt::Monday){
        lundi--; i--;
    }
    return QDate::currentDate().addDays(i);
}

const QList<Horaire> &EmploieDuTemp::lsHoraire() const
{
    return m_lsHoraire;
}

void EmploieDuTemp::setLsHoraire(const QList<Horaire> &newLsHoraire)
{
    m_lsHoraire = newLsHoraire;
}

const QDate &EmploieDuTemp::dateCreation() const
{
    return m_dateCreation;
}

void EmploieDuTemp::setDateCreation(const QDate &newDateCreation)
{
    m_dateCreation = newDateCreation;
}

const QString &EmploieDuTemp::lieuDeCreation() const
{
    return m_lieuDeCreation;
}

void EmploieDuTemp::setLieuDeCreation(const QString &newLieuDeCreation)
{
    m_lieuDeCreation = newLieuDeCreation;
}

const QByteArray &EmploieDuTemp::signature() const
{
    return m_signature;
}

void EmploieDuTemp::setSignature(const QByteArray &newSignature)
{
    m_signature = newSignature;
}

const QString &EmploieDuTemp::nomSalle() const
{
    return m_nomSalle;
}

void EmploieDuTemp::setNomSalle(const QString &newNomSalle)
{
    m_nomSalle = newNomSalle;
}

const QString &EmploieDuTemp::semestre() const
{
    return m_semestre;
}

void EmploieDuTemp::setSemestre(const QString &newSemestre)
{
    m_semestre = newSemestre;
}

const QString &EmploieDuTemp::createur() const
{
    return m_createur;
}

void EmploieDuTemp::setCreateur(const QString &newCreateur)
{
    m_createur = newCreateur;
}


QDataStream& operator<<(QDataStream &stream, EmploieDuTemp const& em){
    QString nomSalle("SALLE_VIDE");
    if(!em.nomSalle().isEmpty())
        nomSalle = em.nomSalle();
    QString semestre("SEMESTRE VIDE");
    semestre = !em.semestre().isEmpty() ? em.semestre() : semestre;
    stream<<em.niveau()<<nomSalle<<semestre<<em.dateDebut()<<em.dateFin()<<em.dateCreation()<<em.lieuDeCreation()<<em.lsMatieres().size();
    foreach (Matiere const& matiere, em.lsMatieres()) {
        stream<<matiere;
    }
    //Ajout de la liste des horaire
    stream<<em.lsHoraire().size();
    for(int i=0; i<em.lsHoraire().size(); i++){
        stream<<em.lsHoraire().at(i);
    }
    return stream;
}
QDataStream& operator>>(QDataStream &stream, EmploieDuTemp& em){
    QDate debut, fin; QString niveau, nomSalle, semestre; int nbreMatiere=0;
    QDate dateCreation; QString lieuCreation;
    QList<Matiere> lsMatiere; lsMatiere.clear();

    stream>>niveau>>nomSalle>>semestre>>debut>>fin>>dateCreation>>lieuCreation>>nbreMatiere;
    for(int i=0, j= nbreMatiere; i<j; i++){
        Matiere matiere;
        stream>>matiere;
        lsMatiere.append(matiere);
    }
    nomSalle = nomSalle == QString("SALLE_VIDE") ? "" : nomSalle;
    semestre = semestre == QString("SEMESTRE VIDE") ? "" : semestre;
    em.setNiveau(niveau);
    em.setNomSalle(nomSalle);
    em.setSemestre(semestre);
    em.setDateDebut(debut);
    em.setLsMatieres(lsMatiere);
    em.setLieuDeCreation(lieuCreation);
    em.setNomSalle(nomSalle);

    //Recueration de la list des horaires
    int nbreHoraire=0;
    QList<Horaire> lsHoraires; lsHoraires.clear();
    stream>>nbreHoraire;
    for(int i=0; i<nbreHoraire; i++){
        Horaire h;
        stream>>h;
        lsHoraires.append(h);
    }
    em.setLsHoraire(lsHoraires);
    return stream;
}

QTextStream& operator<<(QTextStream &stream, EmploieDuTemp const& em){
    QString nomSalle("SALLE_VIDE");
    QString nbreMH = QString("%1:%2").arg(em.lsMatieres().size());
    nbreMH = nbreMH.arg(em.lsHoraire().size());
    if(!em.nomSalle().isEmpty())
        nomSalle = em.nomSalle();
    QString semestre("SEMESTRE VIDE");
    semestre = !em.semestre().isEmpty() ? em.semestre() : semestre;
    stream<<em.niveau()<<"\n"<<
            nbreMH<<"\n"<<
            nomSalle<<"\n"<<
            semestre<<"\n"<<
            em.dateDebut()<<
            em.dateFin()<<
            em.dateCreation()<<
            em.lieuDeCreation()<<"\n";
    foreach (Matiere const& matiere, em.lsMatieres()) {
        stream<<matiere;
    }
    //Ajout de la liste des horaire
    for(int i=0; i<em.lsHoraire().size(); i++){
        stream<<em.lsHoraire().at(i);
    }
    return stream;
}
QTextStream& operator>>(QTextStream &stream, EmploieDuTemp& em){
     QString niveau = stream.readLine(),
             nbreMH = stream.readLine(),
             nomSalle = stream.readLine(),
             semestre = stream.readLine(), lieuCreation;
    QDate debut, fin, dateCreation;
    stream>>debut>>fin>>dateCreation;
    lieuCreation = stream.readLine();
    int nbreMatiere = 0, nbreHoraire= 0;
    if(nbreMH.split(":").size()>= 2){
        nbreMatiere = nbreMH.split(":").at(0).toInt();
        nbreHoraire = nbreMH.split(":").at(1).toInt();
    }
    QList<Matiere> lsMatiere; lsMatiere.clear();
    for(int i=0, j= nbreMatiere; i<j; i++){
        Matiere matiere;
        stream>>matiere;
        lsMatiere.append(matiere);
    }em.setLsMatieres(lsMatiere);

    //Recueration de la list des horaires
    QList<Horaire> lsHoraires; lsHoraires.clear();
    for(int i=0; i<nbreHoraire; i++){
        Horaire h;
        stream>>h;
        lsHoraires.append(h);
    }em.setLsHoraire(lsHoraires);

    nomSalle = nomSalle == QString("SALLE_VIDE") ? "" : nomSalle;
    semestre = semestre == QString("SEMESTRE VIDE") ? "" : semestre;
    em.setNiveau(niveau);
    em.setNomSalle(nomSalle);
    em.setSemestre(semestre);
    em.setDateDebut(debut);
    em.setLieuDeCreation(lieuCreation);
    em.setNomSalle(nomSalle);
    return stream;
}
