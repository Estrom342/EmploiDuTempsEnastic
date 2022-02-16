#include "teacher.h"

Teacher::Teacher(){}

Teacher::Teacher(QString nom, QString prenom, QString grade, QString niveau):nom(nom), prenom(prenom),
    grade(grade), niveau(niveau), id(-1){}

const QString &Teacher::getNom() const
{
    return nom;
}

void Teacher::setNom(const QString &newNom)
{
    nom = newNom;
}

const QString &Teacher::getPrenom() const
{
    return prenom;
}

void Teacher::setPrenom(const QString &newPrenom)
{
    prenom = newPrenom;
}

const QString &Teacher::getGrade() const
{
    return grade;
}

void Teacher::setGrade(const QString &newGrade)
{
    grade = newGrade;
}

const QString &Teacher::getNiveau() const
{
    return niveau;
}

void Teacher::setNiveau(const QString &newNiveau)
{
    niveau = newNiveau;
}

const QString &Teacher::getNomMatiere() const
{
    return matiere;
}

void Teacher::setNomMatiere(const QString &newMatiere)
{
    matiere = newMatiere;
}

QString Teacher::toString() const
{
    QString text = nom+" "+prenom;
    return grade.isEmpty()?text : grade+". "+text;
}

int Teacher::getId() const
{
    return id;
}

void Teacher::setId(int newId)
{
    id = newId;
}

QDataStream& operator<<(QDataStream &stream, Teacher const& t){
    stream<<t.getGrade()<<t.getNom()<<t.getPrenom();
    return stream;
}
QDataStream& operator>>(QDataStream &stream, Teacher& t){
    QString grade, nom, prenom;
    stream>>grade>>nom>>prenom;
    t.setGrade(grade); t.setNom(nom); t.setPrenom(prenom);
    return stream;
}

QTextStream& operator<<(QTextStream &stream, Teacher const& t){
    stream<<t.getGrade()<<"\n";
    stream<<t.getNom()<<"\n";
    stream<<t.getPrenom()<<"\n";
    return stream;
}
QTextStream& operator>>(QTextStream &stream, Teacher& t){
    QString grade = stream.readLine(),
            nom = stream.readLine(),
            prenom = stream.readLine();
    t.setGrade(grade); t.setNom(nom); t.setPrenom(prenom);
    return stream;
}

