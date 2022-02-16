#include "matiere.h"
#include <QDebug>

Matiere::Matiere(QString name, QObject *parent): QObject(parent), name(name),  typeMatiere("CM"), couleur(QColor(Qt::white)), numeroCase(-1),
    idEmploi(-1), idMatiere(-1), m_commentaire(""){}

Matiere::Matiere(const Matiere &m): QObject(nullptr), name(m.getName()),
   typeMatiere(m.getType()), niveau(m.getNiveau()),
    couleur(m.getCouleur()), numeroCase(m.getNumeroCase()),
    idEmploi(m.getIdEmploi()), idMatiere(m.getIdMatiere()),
    enseignant(m.getEnseignant()), m_commentaire(m.commentaire())
{}

Matiere::~Matiere(){}

const QString &Matiere::getName() const{
    return name;
}

void Matiere::setName(const QString &newName){
    name = newName;
}

const QString &Matiere::getNomEnseigant() const{
    return enseignant.getNom();// + " "+ enseignant.getPrenom();
}

void Matiere::setNomEnseigant(const QString &newNomEnseigant){
    enseignant.setNom(newNomEnseigant);
}

const QColor Matiere::getCouleur() const{
    return couleur;
}

void Matiere::setCouleur(QColor newCouleur){
    couleur = newCouleur;
}

QString Matiere::toString() const{
    if(!name.isEmpty())
        return typeMatiere + " " + name+"\n"+enseignant.toString()+" "+m_commentaire;
    return name+"\n"+enseignant.toString()+" "+m_commentaire;
}

QString Matiere::toStyleSheese()
{
    return QString("QTextEdit{background-color: rgb(%1, %2, %3); border: 1px solid black; "
                    "text-align: center; vertical-align: middle;}").arg(couleur.red()).arg(
                couleur.green()).arg(couleur.blue());
}

QString Matiere::toHtml()
{
    auto txt = "<div style='text-align: center'>" + (name.isEmpty()? "" : (typeMatiere=="CM"?"" : typeMatiere)) + "</div><div style='text-align: center'>" +
            name+"</div><div style='text-align: center'>"+
            enseignant.toString()+"</div>";
    if(!m_commentaire.isEmpty())
        txt+= "<div style='text-align: center'>("+m_commentaire+")</div>";
    return txt;
}

const QString &Matiere::getType() const
{
    return typeMatiere;
}

void Matiere::setType(const QString &newType)
{
    typeMatiere = newType;
}

int Matiere::getNumeroCase() const
{
    return numeroCase;
}

void Matiere::setNumeroCase(int newNumeroCase)
{
    numeroCase = newNumeroCase;
}

const QString &Matiere::getNiveau() const
{
    return niveau;
}

void Matiere::setNiveau(const QString &newNiveau)
{
    niveau = newNiveau;
}

int Matiere::getIdEmploi() const
{
    return idEmploi;
}

void Matiere::setIdEmploi(int newIdEmploi)
{
    idEmploi = newIdEmploi;
}

int Matiere::getIdMatiere() const
{
    return idMatiere;
}

void Matiere::setIdMatiere(int newIdMatiere)
{
    idMatiere = newIdMatiere;
}

const Teacher &Matiere::getEnseignant() const
{
    return enseignant;
}

void Matiere::setEnseignant(const Teacher &newEnseignant)
{
    enseignant = newEnseignant;
}

Matiere& Matiere::operator=(const Matiere &m)
{
    name = m.getName(); typeMatiere = m.getType(); niveau = m.getNiveau();
    couleur = m.getCouleur(); numeroCase = m.getNumeroCase();
    idEmploi = m.getIdEmploi(); idMatiere = m.getIdMatiere();
    enseignant = m.getEnseignant();
    m_commentaire = m.commentaire();
    return *this;
}

const QString Matiere::commentaire() const
{
    return m_commentaire;
}

void Matiere::setCommentaire(const QString &newCommentaire)
{
    m_commentaire = newCommentaire;
}

QDataStream& operator<<(QDataStream &stream, Matiere const& m)
{
    QString comment = m.commentaire().isEmpty() ? DEFAULT_COMMENT : m.commentaire();
    stream<<m.getType()<<m.getName()<<m.getEnseignant()<<m.getCouleur().name()<<comment<<m.getNumeroCase();
    return stream;
}

QDataStream& operator>>(QDataStream &stream, Matiere& m)
{
    QString typeM, name, nomCouleur, comment;
    Teacher enseignant; int numCase=-1;
    stream>>typeM>>name>>enseignant>>nomCouleur>>comment>>numCase;
    m.setType(typeM); m.setName(name); m.setEnseignant(enseignant);
    m.setCouleur(QColor(nomCouleur)); m.setNumeroCase(numCase);
    m.setCommentaire(comment != DEFAULT_COMMENT ? comment : "");
    return stream;
}

QTextStream& operator<<(QTextStream &stream, Matiere const& m)
{
    QString comment = m.commentaire().isEmpty() ? DEFAULT_COMMENT : m.commentaire();
    stream<<m.getType()<<"\n";
    stream<<m.getName()<<"\n";
    stream<<QString("%1").arg(m.getNumeroCase())<<"\n";
    stream<<m.getEnseignant();
    stream<<m.getCouleur().name()<<"\n";
    stream<<comment<<"\n";
    return stream;
}

QTextStream& operator>>(QTextStream &stream, Matiere& m)
{
    QString typeM = stream.readLine(),
            name = stream.readLine(),
            textNumCase = stream.readLine();
    int numCase = textNumCase.toInt();
    Teacher enseignant;
    stream>>enseignant;
    QString nomCouleur = stream.readLine(),
            comment = stream.readLine();
    m.setNumeroCase(numCase);
    m.setType(typeM); m.setName(name); m.setEnseignant(enseignant);
    m.setCouleur(QColor(nomCouleur));
    m.setCommentaire(comment != DEFAULT_COMMENT ? comment : "");
    return stream;
}

bool operator<(Matiere const& m1, Matiere const& m2){
    return m1.getName() < m2.getName();
}
bool operator<=(Matiere const& m1, Matiere const& m2){
    return m1.getName() <= m2.getName();
}
bool operator>(Matiere const& m1, Matiere const& m2){
    return m1.getName() > m2.getName();
}
bool operator>=(Matiere const& m1, Matiere const& m2){
    return m1.getName()>=m2.getName();
}
bool operator==(Matiere const& m1, Matiere const& m2){
    return m1.getName() == m2.getName();
}
