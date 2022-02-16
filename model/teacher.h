#ifndef TEACHER_H
#define TEACHER_H

#include <QDataStream>
#include <QVariant>
#include <QTextStream>
#include<QString>

class Teacher
{
public:
    Teacher();
    Teacher(QString nom, QString prenom="", QString grade= "Mr", QString niveau="");

    const QString &getNom() const;
    void setNom(const QString &newNom);

    const QString &getPrenom() const;
    void setPrenom(const QString &newPrenom);

    const QString &getGrade() const;
    void setGrade(const QString &newGrade);

    const QString &getNiveau() const;
    void setNiveau(const QString &newNiveau);

    const QString &getNomMatiere() const;
    void setNomMatiere(const QString &newMatiere);
    QString toString() const;

    int getId() const;
    void setId(int newId);

private:
    QString nom, prenom, grade, niveau, matiere;
    int id;
};
Q_DECLARE_METATYPE(Teacher);

QDataStream& operator<<(QDataStream &stream, Teacher const& t);
QDataStream& operator>>(QDataStream &stream, Teacher& t);

QTextStream& operator<<(QTextStream &stream, Teacher const& t);
QTextStream& operator>>(QTextStream &stream, Teacher& t);

#endif // TEACHER_H
