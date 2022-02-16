#include "horaireview.h"

HoraireView::HoraireView(QWidget *parent):QTextEdit(parent), m_numeroCase(-1), m_horaire(-1),
    m_updateAble(false), m_editable(false), m_idEmploi(-1), m_dataBase(nullptr){
    setHtml(m_horaire.toHtml());
    setReadOnly(true);
}

HoraireView::HoraireView(Horaire horair, QWidget *parent): QTextEdit(parent)
{
    m_idEmploi= -1;
    m_horaire = horair;
    m_updateAble = false;
    m_dataBase = nullptr;
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
    if(m_dataBase == nullptr) return;
    auto menu = new QMenu();
    auto me = this;
    auto aHoraire = menu->addAction(tr("Changer l'heure"), this, [me](){
        auto w = new QWidget();
        w->setWindowModality(Qt::ApplicationModal);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setWindowTitle(tr("Changement d'heure"));
        auto heureDebut = new QTimeEdit(me->horaire().heureDebut());
        auto heureFin = new QTimeEdit(me->horaire().heureFin());
        auto btnSave = new QPushButton(tr("Enregistrer"));
        auto btnCancel = new QPushButton(tr("Annuler"));
        auto lF =new QFormLayout();
        lF->addRow(tr("Heure de début"), heureDebut);
        lF->addRow(tr("Heure de fin"), heureFin);
        lF->addRow(btnCancel, btnSave);
        w->setLayout(lF);
        w->move(QCursor::pos());
        w->show();
        w->setFixedSize(w->size());

        connect(btnSave, &QPushButton::clicked, me, [me, heureDebut, heureFin, w](){
           if(heureDebut->time() > heureFin->time()){
            QMessageBox::critical(me, "", tr("L'heure de debut doit etre inferieur à l'heure de fin"));
            return;
           }
           me->m_horaire.setHeureDebut(heureDebut->time());
           me->m_horaire.setHeureFin(heureFin->time());
           if(me->isUpdateAble()){
               me->m_dataBase->updateHoraire(me->horaire());
               qDebug()<<me->m_horaire.numeroCase()<<me->m_horaire.idEmploi();
           }
           me->setHtml(me->m_horaire.toHtml());
           w->close();
        });
        connect(btnCancel, &QPushButton::clicked, w, &QWidget::close);
    });
    aHoraire->setEnabled(isEditable());
    menu->move(QCursor::pos());
    menu->show();
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

void HoraireView::setDataBase(EmploiDatabase *newDataBase)
{
    m_dataBase = newDataBase;
}

bool HoraireView::isUpdateAble() const
{
    return m_updateAble;
}

void HoraireView::setUpdateAble(bool newUpdateAble)
{
    m_updateAble = newUpdateAble;
}
