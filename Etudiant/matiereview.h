#ifndef MATIEREVIEW_H
#define MATIEREVIEW_H

#include "homewidget.h"
#include"model/matiere.h"
#include "model/localdatabase.h"


class MatiereView: public QTextEdit
{
public:
    MatiereView(Matiere const &m= Matiere());
    const Matiere &getMatiere() const;
    const Matiere getMatiere();

    void setMatiere(const Matiere &newMatiere);

    void setDatabase(LocalDatabase *newDatabase);

    int getNumeroCase() const;
    void setNumeroCase(int newNumeroCase);

    bool isUpdateable() const;
    void setUpdateable(bool newUpdateable);

    int getIdEmploi() const;
    void setIdEmploi(int newIdEmploi);

private:
    Matiere matiere;
    LocalDatabase* database;
    int numeroCase;
    bool updateable;
    int idEmploi;
};

#endif // MATIEREVIEW_H
