#ifndef TEACHERWIDGET_H
#define TEACHERWIDGET_H

#include <QtWidgets>
#include <QtGui>
#include "model/emploidatabase.h"

class TeacherWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TeacherWidget(QWidget *parent = nullptr);

    void setDatabase(EmploiDatabase *newDatabase);

public slots:
    void showAddWidget();
    void showEditWidget(Teacher const &oldTeacher);

private slots:
    void actualiser(QString level);

signals:


private:
    EmploiDatabase *database;
    QComboBox *filiere;
    QTableView* vue;
    QPushButton *btnModifier;
};

#endif // TEACHERWIDGET_H
