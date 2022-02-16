#include "horaireview.h"

HoraireView::HoraireView(QWidget *parent):QTextEdit(parent), m_numeroCase(-1), m_horaire(-1),
    m_updateAble(false), m_editable(false), m_idEmploi(-1){
    setHtml(m_horaire.toHtml());
    setReadOnly(true);
}

HoraireView::HoraireView(Horaire horair, QWidget *parent): QTextEdit(parent)
{
    m_idEmploi= -1;
    m_horaire = horair;
    m_updateAble = false;
    setReadOnly(true);
    setNumeroCase(horair.numeroCase());
    setHtml(m_horaire.toHtml());
}

Horaire HoraireView::horaire() const
{
    return m_horaire;
}

void HoraireView::setHoraire(Horaire newHoraire)
{
    m_horaire = newHoraire;
    if(newHoraire.numeroCase() == -1)
        m_horaire.setNumeroCase(m_numeroCase);
    setHtml(m_horaire.toHtml());
}

int HoraireView::numeroCase() const
{
    return m_numeroCase;
}

void HoraireView::setNumeroCase(int newNumeroCase)
{
    m_numeroCase = newNumeroCase;
    m_horaire.setNumeroCase(m_numeroCase);
    setHtml(m_horaire.toHtml());
}

//Affiche le menu contexttuel
void HoraireView::contextMenuEvent(QContextMenuEvent *e)
{
    e->accept();
}

bool HoraireView::idEmploi() const
{
    return m_idEmploi;
}

void HoraireView::setIdEmploi(bool newIdEmploi)
{
    m_idEmploi = newIdEmploi;
    m_horaire.setIdEmploi(newIdEmploi);
}

bool HoraireView::isEditable() const
{
    return m_editable;
}

void HoraireView::setEditable(bool newEditable)
{
    m_editable = newEditable;
}

bool HoraireView::isUpdateAble() const
{
    return m_updateAble;
}

void HoraireView::setUpdateAble(bool newUpdateAble)
{
    m_updateAble = newUpdateAble;
}
