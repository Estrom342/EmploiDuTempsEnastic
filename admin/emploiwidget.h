#ifndef EMPLOIWIDGET_H
#define EMPLOIWIDGET_H

#include <QtGui>
#include <QtWidgets>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include "matiereview.h"
#include "horaireview.h"
#include "model/emploidatabase.h"
#include "model/emploiedutemp.h"

class EmploiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EmploiWidget(QWidget *parent = nullptr);
    static QPushButton *pushButton(QString txt= "");

    void setDatabase(EmploiDatabase *newDatabase);

    const EmploieDuTemp &getEmploi() const;
    void setCurrentLevel(const QString &level);
    void setDefaultLevel(const QString level);

    const QMap<QString, EmploieDuTemp> &getLsEmplois() const;
    void setLsEmplois(const QMap<QString, EmploieDuTemp> &newLsEmplois);
    void setLabelText();

    void setUpdateAble(bool value);
    void setEditAble(bool value);
    bool isPrintable();

public slots:
    void lireEmploi();
    void actualiser(); //Actualise l'emploi afficher
    void actualiserMatiere(Matiere m); //Actualise l'emploi afficher
    void imprimer();
private slots:
    void next();
    void preview();
    QWidget* createEmploiVidWindow();
signals:

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *event) override;
private:
    EmploiDatabase *database;
    EmploieDuTemp emploi;
    QList<MatiereView*> lsMatiereView;
    QList<HoraireView*> lsHoraireView;
    QMap<QString, EmploieDuTemp> lsEmplois;
    int offset;
    QLabel *label;
    QDate dateDebut;
    QLabel *m_labelDateCreation;
    QLabel *m_labelSignature;

    QPushButton *btnNext , *btnPreview, *btnPrint;
    QStringList lsFiliere;

    QStackedWidget *centralWindow;
    QWidget *emploiWindow, *emploiVideWindow;
    QScrollArea *scrollEmploiWindow;
};

#endif // EMPLOIWIDGET_H
