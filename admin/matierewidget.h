#ifndef MATIEREWIDGET_H
#define MATIEREWIDGET_H

#include <QtGui>
#include <QtWidgets>
#include "model/emploidatabase.h"

class MatiereWidget: public QWidget
{
public:
    MatiereWidget(QWidget *parent= nullptr);
    void setDatabase(EmploiDatabase *newDatabase);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void showAddWidget();
    void showEditWidget();
    void actualiser();


private:
    QTableView *vue;
    QComboBox *filieres;

    QPushButton* btnModifier;
    //Data
    EmploiDatabase *database;
};

#endif // MATIEREWIDGET_H
