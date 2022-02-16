#include "horaire.h"

Horaire::Horaire()
{
    m_heureDebut = QTime::currentTime();
    m_heureFin = QTime::currentTime().addMSecs(360 * 2);
    m_idEmploi = -1;
    m_numeroCase = -1;
}

Horaire::Horaire(int numCase)
{
    m_idEmploi = -1;
    setNumeroCase(numCase);
}

const QTime &Horaire::heureDebut() const
{
    return m_heureDebut;
}

void Horaire::setHeureDebut(const QTime &newHeureDebut)
{
    m_heureDebut = newHeureDebut;
}

const QTime &Horaire::heureFin() const
{
    return m_heureFin;
}

void Horaire::setHeureFin(const QTime &newHeureFin)
{
    m_heureFin = newHeureFin;
}

int Horaire::idEmploi() const
{
    return m_idEmploi;
}

void Horaire::setIdEmploi(int newIdEmploi)
{
    m_idEmploi = newIdEmploi;
}

int Horaire::numeroCase() const
{
    return m_numeroCase;
}

void Horaire::setNumeroCase(int newNumeroCase)
{
    m_numeroCase = newNumeroCase;
    switch (newNumeroCase) {
    case 1:
        m_heureDebut = QTime(7, 30);
        m_heureFin = QTime(9, 30);
        break;
    case 2:
        m_heureDebut = QTime(9, 30);
        m_heureFin = QTime(9, 40);
        break;
    case 3:
        m_heureDebut = QTime(9, 40);
        m_heureFin = QTime(12, 40);
        break;
    case 4:
        m_heureDebut = QTime(12, 40);
        m_heureFin = QTime(13, 40);
        break;
    case 5:
        m_heureDebut = QTime(13, 40);
        m_heureFin = QTime(15, 40);
        break;
    default:
        m_heureDebut = QTime::currentTime();
        m_heureFin = QTime::currentTime().addMSecs(360 * 2);
    }
}

QStringList Horaire::standardLsHoraire()
{
    QStringList lsHoraire;
    lsHoraire<<QTime(7, 30).toString()+" - "<<QTime(9, 30).toString();
    lsHoraire<<QTime(9, 30).toString()+" - "<<QTime(9, 40).toString();
    lsHoraire<<QTime(9, 40).toString()+" - "<<QTime(12, 40).toString();
    lsHoraire<<QTime(12, 40).toString()+" - "<<QTime(13, 40).toString();
    lsHoraire<<QTime(13, 40).toString()+" - "<<QTime(15, 40).toString();
    lsHoraire<<QTime(13, 40).toString()+" - "<<QTime(17, 40).toString();
    lsHoraire<<QTime(15, 40).toString()+" - "<<QTime(18, 40).toString();
    return lsHoraire;
}

QString Horaire::toString() const
{
    QString txt = "%1 - %2";
    txt = txt.arg(m_heureDebut.toString("hh%1 mm").arg("h"));
    return txt.arg(m_heureFin.toString("hh%1 mm").arg("h"));
}

QString Horaire::toHtml() const
{
    return "<div style='text-align:center'>"+ toString() +"</div>";
}


QDataStream& operator<<(QDataStream &stream, Horaire const& h){
    stream<<h.heureDebut()<<h.heureFin()<<QString("%1").arg(h.numeroCase());
    return stream;
}
QDataStream& operator>>(QDataStream &stream, Horaire& h){
    QTime heureDebut, heureFin;
    QString numCaseStr;
    stream>>heureDebut>>heureFin>>numCaseStr;
    h.setHeureDebut(heureDebut);
    h.setHeureFin(heureFin);
    h.setNumeroCase(numCaseStr.toInt());
    return stream;
}

QTextStream& operator<<(QTextStream &stream, Horaire const& h){
    QTime hd = h.heureDebut().addMSecs(h.numeroCase());
    QTime hF = h.heureFin().addMSecs(h.numeroCase());
    stream<<hd<<hF;
    return stream;
}
QTextStream& operator>>(QTextStream &stream, Horaire& h){
    QTime heureDebut, heureFin;
    stream>>heureDebut>>heureFin;
    h.setNumeroCase(heureDebut.msec());
    h.setHeureDebut(heureDebut);
    h.setHeureFin(heureFin);
    return stream;
}

QTextStream& operator<<(QTextStream &stream, QDate const& d){
    QString txt = QString("%1:%2:%3").arg(d.year());
    txt = txt.arg(d.month());
    txt = txt.arg(d.day());
    stream<<txt<<"\n";
    return stream;
}
QTextStream& operator>>(QTextStream &stream, QDate& d){
    auto txt = stream.readLine().split(":");
    if(txt.size()>=3){
        int y= txt[0].toInt(), m= txt[1].toInt(), day = txt[2].toInt();
        d.setDate(y, m, day);
    }else{
        d = QDate::currentDate();
    }
    return stream;
}

QTextStream& operator<<(QTextStream &stream, QTime const& t){
    QString txt = QString("%1:%2:%3:%4").arg(t.hour());
    txt = txt.arg(t.minute());
    txt = txt.arg(t.second());
    txt = txt.arg(t.msec());
    stream<<txt<<"\n";
    return stream;
}
QTextStream& operator>>(QTextStream &stream, QTime& t){
    auto txt = stream.readLine().split(":");
    if(txt.size()>=4){
        int h= txt[0].toInt(), m= txt[1].toInt(), s = txt[2].toInt(), ms =txt[3].toInt();
        t.setHMS(h, m, s, ms);
    }else{
        t = QTime::currentTime();
    }
    return stream;
}
