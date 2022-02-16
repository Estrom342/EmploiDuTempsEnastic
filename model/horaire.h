#ifndef HORAIRE_H
#define HORAIRE_H

#include <QTime>
#include<QObject>
#include <QTextStream>
#include<QDebug>

class Horaire
{
public:
    Horaire();
    Horaire(int numCase);

    const QTime &heureDebut() const;
    void setHeureDebut(const QTime &newHeureDebut);

    const QTime &heureFin() const;
    void setHeureFin(const QTime &newHeureFin);

    int idEmploi() const;
    void setIdEmploi(int newIdEmploi);

    int numeroCase() const;
    void setNumeroCase(int newNumeroCase);
    static QStringList standardLsHoraire();

    QString toString() const;
    QString toHtml() const;

private:
    QTime m_heureDebut, m_heureFin;
    int m_idEmploi, m_numeroCase;
};
Q_DECLARE_METATYPE(Horaire);

QDataStream& operator<<(QDataStream &stream, Horaire const& h);
QDataStream& operator>>(QDataStream &stream, Horaire& h);

QTextStream& operator<<(QTextStream &stream, Horaire const& h);
QTextStream& operator>>(QTextStream &stream, Horaire& h);


QTextStream& operator<<(QTextStream &stream, QDate const& d);
QTextStream& operator>>(QTextStream &stream, QDate& d);

QTextStream& operator<<(QTextStream &stream, QTime const& d);
QTextStream& operator>>(QTextStream &stream, QTime& d);

#endif // HORAIRE_H
