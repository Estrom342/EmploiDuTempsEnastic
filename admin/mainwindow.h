#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtWidgets>
#include "loginwidget.h"
#include "timetablehistorywidget.h"
#include "newemploiwidget.h"
#include "matierewidget.h"
#include "teacherwidget.h"
#include "model/emploidatabase.h"
#include "model/localdatabase.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void createNewEmploi();
    void afficherApropos();
    void afficherAide();
private:
    void createMenu();
    void createToolBar();
    void createCentralsWidgets();
    bool quitter();

    //Variable membres
    QLabel *labelInfo;
    QComboBox *lsNiveaux;
    QStackedWidget *centralWindow;
    LoginWidget *logineWidget;
    EmploiWidget *emploiWidget;
    NewEmploiWidget *newEmploiWidget;
    MatiereWidget *matiereWidget;
    TeacherWidget *teacherWidget;
    TimeTableHistoryWidget *historyWidget;

    //Les données
    EmploiDatabase *database;
    LocalDatabase *localDatabase;
};
#endif // MAINWINDOW_H
