#ifndef TIMETABLEHISTORYWIDGET_H
#define TIMETABLEHISTORYWIDGET_H

#include <QWidget>
#include "emploiwidget.h"

class TimeTableHistoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimeTableHistoryWidget(QWidget *parent = nullptr);

    void setDatabase(EmploiDatabase *newDatabase);
public slots:
    void loadTimeTables();
signals:

private:
    QStackedWidget *centraleWidget;
    EmploiWidget *timeTableWidget;
    QTableView* lsTimeTable;
    QStandardItemModel *model;
    EmploiDatabase *database;
};

#endif // TIMETABLEHISTORYWIDGET_H
