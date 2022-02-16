#ifndef MATIEREWIDGET_H
#define MATIEREWIDGET_H

#include <QtGui>
#include <QtWidgets>

class MatiereWidget: public QWidget
{
public:
    MatiereWidget(QWidget *parent= nullptr);
private slots:
    void showAddWidget();


private:
    QTableView *vue;
};

#endif // MATIEREWIDGET_H
