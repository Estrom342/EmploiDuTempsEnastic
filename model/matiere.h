#ifndef MATIERE_H
#define MATIERE_H

#include <QObject>
#include <QColor>
#include "constante.h"
#include "teacher.h"

//class Matiere;

class Matiere: public QObject
{
    Q_OBJECT
public:
    explicit Matiere(QString name = "", QObject *parent = nullptr);
    Matiere(Matiere const& m);
    ~Matiere();

    const QString &getName() const;
    void setName(const QString &newName);

    const QString &getNomEnseigant() const;
    void setNomEnseigant(const QString &newNomEnseigant);

    const QColor getCouleur() const;
    void setCouleur(QColor newCouleur);
    QString toString() const;
    QString toStyleSheese();
    QString toHtml();

    const QString &getType() const;
    void setType(const QString &newType);

    int getNumeroCase() const;
    void setNumeroCase(int newNumeroCase);

    const QString &getNiveau() const;
    void setNiveau(const QString &newNiveau);

    int getIdEmploi() const;
    void setIdEmploi(int newIdEmploi);

    int getIdMatiere() const;
    void setIdMatiere(int newIdMatiere);

    const Teacher &getEnseignant() const;
    void setEnseignant(const Teacher &newEnseignant);
    Matiere &operator =(Matiere const& m);

    const QString commentaire() const;
    void setCommentaire(const QString &newCommentaire);

private:
    //numCase, type, Couleur, nomMatiere, nomEnseignant
    QString name, typeMatiere, niveau;
    QColor couleur; int numeroCase, idEmploi, idMatiere;
    Teacher enseignant; QString m_commentaire;
};
Q_DECLARE_METATYPE(Matiere);

QDataStream& operator<<(QDataStream &stream, Matiere const& m);
QDataStream& operator>>(QDataStream &stream, Matiere& m);

QTextStream& operator<<(QTextStream &stream, Matiere const& m);
QTextStream& operator>>(QTextStream &stream, Matiere& m);


bool operator<(Matiere const& m1, Matiere const& m2);
bool operator<=(Matiere const& m1, Matiere const& m2);
bool operator>(Matiere const& m1, Matiere const& m2);
bool operator>=(Matiere const& m1, Matiere const& m2);
bool operator==(Matiere const& m1, Matiere const& m2);

#endif // MATIERE_H
