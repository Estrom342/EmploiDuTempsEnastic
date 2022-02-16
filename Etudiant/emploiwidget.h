#ifndef EMPLOIWIDGET_H
#define EMPLOIWIDGET_H

#include <QScrollArea>
#include <QPrintDialog>
#include <QStackedWidget>
#include <QPrinter>
#include <QPainter>
#include<QApplication>
#include <QScreen>
#include "matiereview.h"
#include "model/matiere.h"
#include "model/localdatabase.h"
#include "model/emploiedutemp.h"
#include  "controler/studentsocket.h"


class EmploiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EmploiWidget(QWidget *parent = nullptr);
    static QPushButton *pushButton(QString txt= "");

    //void setDatabase(LocalDatabase *newDatabase);

    const EmploieDuTemp &getEmploi() const;
    void setCurrentLevel(const QString &level);
    void setDefaultLevel(const QString level);

    const QMap<QString, EmploieDuTemp> &getLsEmplois() const;
    void setLsEmplois(const QMap<QString, EmploieDuTemp> &newLsEmplois);
    void setLabelText();
    void setLabelDateLieu();
    void setLabelInfos();
    void setLabelSignature();
    bool isPrintAble();

public slots:
    void readLastEmploi();
    void readEmplois(QDate &dateDebut);
    void readEmploi(QDate dateDebut, QString niveu);
    void setSignature(QString const&level, const QString &nameSalle, QDate const&dateDebut, QDate const& dateFin, QByteArray const& signature);
    void imprimer();
private slots:
    void next();
    void preview();
    void updateCursor();
    void emitAskeEmploiSignal();
    QWidget* createEmploiVidWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void askEmploiSignal(QDate dateDebut, QString niveau); //Signal emit lorsqu'on actualise aves le menu contextuel

private:
    LocalDatabase *database;
    EmploieDuTemp emploi;
    QList<MatiereView*> lsMatiereView; //Liste des champs pour afficher les matieres
    QList<HoraireView*> lsHoraireView; //Liste des champs pour afficher les horaires
    QMap<QString, EmploieDuTemp> lsEmplois;
    int offset;
    QLabel *label; QTextEdit *labelInfos;
    QDate dateDebut;

    QPushButton *btnNext , *btnPreview;
    QStringList lsFiliere;

    QStackedWidget *centralWindow;
    QWidget *emploiVideWindow;
    QWidget *emploiCentent;
    //signature, Date et lieu de creation d'emploi
    QLabel *labelSignature;
    QLabel *labelDateLieu;
    QScrollArea* emploiWindow;
};

#endif // EMPLOIWIDGET_H
