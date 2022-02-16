#include "mainwindow.h"
#include "homewidget.h"
#include <QApplication>

void textDate(){
    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);
    QDate d;
    d.setDate(2020, 11, 11);
    stream.seek(0);
    qDebug()<<d;
    stream<<d<<d<<d<<d<<d<<d<<d<<d<<d<<d<<d<<d;
    stream.seek(0);
    stream>>d>>d>>d>>d>>d>>d>>d>>d>>d>>d>>d>>d;
    qDebug()<<d;
}

void textTime(){
    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);
    QTime t = QTime::currentTime();
    t.setHMS(20, 11, 11);
    stream<<t<<t<<t;
    stream<<t<<t<<t;
    stream.seek(0);
    qDebug()<<t;
    stream>>t>>t>>t;
    stream>>t>>t>>t;
    qDebug()<<t;
}

void testTecher(){
    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);
    Teacher t("Nom", "Prenom");
    stream<<t<<t<<t;
    stream<<t<<t<<t;
    stream.seek(0);
    qDebug()<<t.toString();
    stream>>t>>t>>t;
    stream>>t>>t>>t;
    qDebug()<<t.toString();
}

void textHoraire(){
    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);
    Horaire h(5);
    QTime hDebut; hDebut.setHMS(7, 30, 0);
    h.setHeureDebut(hDebut);
    h.setHeureFin(hDebut.addSecs(3600));
    stream<<h<<h<<h<<h;
    qDebug()<<h.toString()<<h.numeroCase();
    stream.seek(0);
    stream>>h>>h>>h>>h;
    qDebug()<<h.toString()<<h.numeroCase();
}



void testeMatiere(){
    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);
    Matiere m("Nom Matiere");
    m.setType("TD");
    m.setNomEnseigant("Nom enseignant");
    m.setCommentaire("Commentaire");
    m.setNumeroCase(5);

    stream<<m<<m<<m<<m<<m<<m<<m;
    qDebug()<<m.toString()<<m.getNumeroCase();
    stream.seek(0);
    stream>>m>>m>>m>>m>>m>>m>>m;
    qDebug()<<m.toString()<<m.getNumeroCase();
}

void testEmploi(){
    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);
    EmploieDuTemp em("MTIC2", "S1");
    QDate date;
    date.setDate(2020, 11, 11);
    em.setDateDebut(date);
    em.setDateCreation(QDate::currentDate());
    em.setLieuDeCreation("Ndjamena");

    QList<Matiere> lsMatiere;
    for(int i = 1; i<=20; i++){
        Matiere m("Nom Matiere");
        m.setType("TD");
        m.setNomEnseigant("Nom enseignant");
        m.setCommentaire("Commentaire");
        m.setNumeroCase(i);
        lsMatiere.append(m);
    }
    em.setLsMatieres(lsMatiere);
    QList<Horaire> lsHoraires;
    for(int i = 1; i<=6; i++){
        Horaire h(i);
        QTime hDebut; hDebut.setHMS(7, 30, 0);
        h.setHeureDebut(hDebut);
        h.setHeureFin(hDebut.addSecs(3600));
        lsHoraires.append(h);
    }
    em.setLsHoraire(lsHoraires);

    qDebug()<<em.niveau()<<"Cree le"<<em.dateCreation()<<"a"<<em.lieuDeCreation()<<"du"<<em.dateDebut()
           <<"au"<<em.dateFin()<<"Matirers: "<<em.lsMatieres().size()<<"Horaire"<<em.lsHoraire().size();
    stream<<em<<em<<em<<em<<em<<em<<em<<em<<em<<em<<em<<em;
    stream<<em<<em<<em<<em<<em<<em<<em<<em<<em<<em<<em<<em;
    stream.seek(0);
    stream>>em>>em>>em>>em>>em>>em>>em>>em>>em>>em>>em>>em;
    stream>>em>>em>>em>>em>>em>>em>>em>>em>>em>>em>>em>>em;
    qDebug()<<em.niveau()<<"Cree le"<<em.dateCreation()<<"a"<<em.lieuDeCreation()<<"du"<<em.dateDebut()
           <<"au"<<em.dateFin()<<"Matirers: "<<em.lsMatieres().size()<<"Horaire"<<em.lsHoraire().size();
}

int main(int argc, char *argv[])
{
#ifdef ANDROID_EMPLOI
#ifdef DESKTOP_EMPLOI
    assert(false);
#endif
#endif
    Q_INIT_RESOURCE(resource);
    qRegisterMetaType<EmploieDuTemp>();
    QApplication a(argc, argv);
    //testEmploi();
    //return 0;
    qDebug()<<QThread::currentThread();
    LocalDatabase::createDatabase(":/img/model/localDataBase.sql");
    MainWindow w; w.resize(890, 600); w.showMaximized();
    QApplication::setFont(QFont("", 11, -1, true));
    QApplication::setWindowIcon(QIcon(":/img/res/icon.svg"));
    QApplication::setApplicationDisplayName("Emploi ENASTIC");
    QApplication::setApplicationName("Emploi ENASTIC");
    QApplication::setApplicationVersion("1.0");
    return a.exec();
}
