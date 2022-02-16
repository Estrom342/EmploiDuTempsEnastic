#ifndef MATIEREVIEW_H
#define MATIEREVIEW_H

#include<QtGui>
#include<QtWidgets>
#include "model/matiere.h"
#include "model/matiere.h"
#include "model/emploidatabase.h"


class MatiereView: public QTextEdit
{
    Q_OBJECT
public:
    MatiereView(Matiere m= Matiere());
    const Matiere &getMatiere() const;
    const Matiere getMatiere();

    void setMatiere(const Matiere &newMatiere);

    void setDatabase(EmploiDatabase *newDatabase);

    int getNumeroCase() const;
    void setNumeroCase(int newNumeroCase);

    bool isUpdateable() const;
    void setUpdateable(bool newUpdateable);

    int getIdEmploi() const;
    void setIdEmploi(int newIdEmploi);
    const QString color() const;
    bool mouseIn() const;
    void setMouseIn(bool newMouseIn);

    bool isEditable() const;
    void setEditable(bool newEditable);

public slots:
    void setColor(const QString &newColor);

signals:
    void colorChanged(QString); //Signal emit lorsqu'on change la couleur de la matiere
    void actualiserMatiere(Matiere);//Signal emit a chaque modification de la matiere

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    bool event(QEvent *e) override;

private:
    Matiere matiere;
    EmploiDatabase* database;
    int numeroCase;
    bool m_updateable;
    bool m_editable;
    int idEmploi;
    QString m_color;
    QTextEdit *dataWidget;
    bool m_activeDrag;
    bool m_mouseIn;
};

#endif // MATIEREVIEW_H
