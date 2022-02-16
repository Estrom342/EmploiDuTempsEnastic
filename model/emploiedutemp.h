#ifndef EMPLOIEDUTEMP_H
#define EMPLOIEDUTEMP_H

#include <QDate>
#include <QDebug>
#include "horaire.h"
#include "matiere.h"

class EmploieDuTemp
{
public:
    EmploieDuTemp(QString niveau= "", QString semestre= "");
    void addMatiere(Matiere &matiere);
    void updateMatiere(Matiere &matiere);

    const QList<Matiere> &lsMatieres() const;
    void setLsMatieres(const QList<Matiere> &newLsMatieres);

    const QString &niveau() const;
    void setNiveau(const QString &newNiveau);

    const QDate &dateDebut() const;
    void setDateDebut(const QDate &newDateDebut);
    void setDateFin(const QDate &newDateDebut);

    const QDate &dateFin() const;
    static QString dateToString(QDate date);

    int idEmploi() const;
    void setIdEmploi(int newIdEmploi);
    static QDate currentMonday();

    const QList<Horaire> &lsHoraire() const;
    void setLsHoraire(const QList<Horaire> &newLsHoraire);

    const QDate &dateCreation() const;
    void setDateCreation(const QDate &newDateCreation);

    const QString &lieuDeCreation() const;
    void setLieuDeCreation(const QString &newLieuDeCreation);

    const QByteArray &signature() const;
    void setSignature(const QByteArray &newSignature);

    const QString &nomSalle() const;
    void setNomSalle(const QString &newNomSalle);

    const QString &semestre() const;
    void setSemestre(const QString &newSemestre);

    const QString &createur() const;
    void setCreateur(const QString &newCreateur);

private:
    QString m_niveau, m_semestre;
    QList<Matiere> m_lsMatieres;
    QList<Horaire> m_lsHoraire;
    QDate m_dateDebut, m_dateFin, m_dateCreation;
    int m_idEmploi;
    QString m_lieuDeCreation;
    QByteArray m_signature;
    QString m_nomSalle;
    QString m_createur;
};
Q_DECLARE_METATYPE(EmploieDuTemp);

QDataStream& operator<<(QDataStream &stream, EmploieDuTemp const& em);
QDataStream& operator>>(QDataStream &stream, EmploieDuTemp& em);

QTextStream& operator<<(QTextStream &stream, EmploieDuTemp const& em);
QTextStream& operator>>(QTextStream &stream, EmploieDuTemp& em);


#endif // EMPLOIEDUTEMP_H
