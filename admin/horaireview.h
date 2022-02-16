#ifndef HORAIREVIEW_H
#define HORAIREVIEW_H

#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QTimeEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QMessageBox>
#include <QContextMenuEvent>
#include "model/horaire.h"
#include "model/emploidatabase.h"

class HoraireView : public QTextEdit
{
public:
    HoraireView(QWidget *parent=nullptr);
    HoraireView(Horaire horair, QWidget *parent=nullptr);

    Horaire horaire() const;
    void setHoraire(Horaire newHoraire);

    int numeroCase() const;
    void setNumeroCase(int newNumeroCase);
    bool isUpdateAble() const;
    void setUpdateAble(bool newUpdateAble);

    void setDataBase(EmploiDatabase *newDataBase);

    void setEditable(bool newEditable);

    bool isEditable() const;

    bool idEmploi() const;
    void setIdEmploi(bool newIdEmploi);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    int m_numeroCase;
    Horaire m_horaire;
    bool m_updateAble;
    bool m_editable, m_idEmploi;
    EmploiDatabase *m_dataBase;
};

#endif // HORAIREVIEW_H
