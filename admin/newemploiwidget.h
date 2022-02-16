#ifndef NEWEMPLOIWIDGET_H
#define NEWEMPLOIWIDGET_H

#include <QtGui>
#include <QtWidgets>
#include "model/emploiedutemp.h"
#include "model/emploidatabase.h"
#include "model/localdatabase.h"
#include "matiereview.h"
#include "horaireview.h"

class NewEmploiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NewEmploiWidget(QWidget *parent = nullptr);
    void setEmploi(EmploieDuTemp const& em);

    void setDatabase(EmploiDatabase *newDatabase);
    void setUpdateAble(bool value);
    void setEditAble(bool value);
    void setLabelText(QString const& txt);

    bool contentData() const;

    void setContentData(bool newContentData);

    LocalDatabase *localDatabase() const;
    void setLocalDatabase(LocalDatabase *newLocalDatabase);

protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void saveEmploi();
    void choisirSignature();
    void updateSemeste(QString const& txt);
signals:
    void saveSignal();
    void actualiser(); //Signal emit a chaque fois qu'une case change de contenu
    void actualiserMatiere(Matiere); //Signal emit a chaque fois qu'une case change de contenu
private:
    QList<MatiereView*> lsMatiereView;
    QList<HoraireView*> lsHoraireView;
    QLabel* label; QPushButton* btnSave;
    EmploieDuTemp currentEmploieDuTemp;
    EmploiDatabase *database;
    bool m_contentData;
    QWidget *emploiWidget;

    //Date lieu et signnature de l'emploi
    QDateEdit *dateCreation;
    QComboBox *lieuCreation, *m_semestreBox;QStringList lsSemestres;
    QLabel *labelSignature;
    bool m_updateAble;
    LocalDatabase *m_localDatabase;
};

#endif // NEWEMPLOIWIDGET_H
