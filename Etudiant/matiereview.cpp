#include "matiereview.h"
#include <QColorDialog>

MatiereView::MatiereView(const Matiere &m): matiere(m), updateable(false), idEmploi(-1)
{
    database = nullptr; numeroCase = m.getNumeroCase();
    setFont(QFont("", DEFAULT_FONT_SIZE));
    //matiere.setName("Nom de la matière");
    //matiere.setNomEnseigant("Nom de l'enseignant");
    setStyleSheet(matiere.toStyleSheese());
    setHtml(matiere.toHtml());
    setAlignment(Qt::AlignCenter);
    setReadOnly(true);

    matiere.setCouleur(QColor(255, 255, 255));
    setStyleSheet(matiere.toStyleSheese());
    disconnect(this);
}

const Matiere MatiereView::getMatiere()
{
    return matiere;
}

int MatiereView::getIdEmploi() const
{
    return idEmploi;
}

void MatiereView::setIdEmploi(int newIdEmploi)
{
    idEmploi = newIdEmploi;
    matiere.setIdEmploi(idEmploi);
}

bool MatiereView::isUpdateable() const
{
    return updateable;
}

void MatiereView::setUpdateable(bool newUpdateable)
{
    updateable = newUpdateable;
}

int MatiereView::getNumeroCase() const
{
    return numeroCase;
}

void MatiereView::setNumeroCase(int newNumeroCase)
{
    numeroCase = newNumeroCase;
}

void MatiereView::setDatabase(LocalDatabase *newDatabase)
{
    database = newDatabase;
}

void MatiereView::setMatiere(const Matiere &newMatiere)
{
    matiere = newMatiere;
    matiere.setNumeroCase(numeroCase);
    if(idEmploi != matiere.getIdEmploi()) matiere.setIdEmploi(idEmploi);
    setStyleSheet(matiere.toStyleSheese());
    setHtml(matiere.toHtml());
}
