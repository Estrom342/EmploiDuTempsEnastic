#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTcpSocket>
#include <QThread>
#include <QMainWindow>
#include <QApplication>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QLineEdit>
#include <QComboBox>
#include <QToolBar>
#include <QCalendarWidget>
#include "emploiwidget.h"
#include "homewidget.h"
#include "controler/studentsocket.h"
#include "model/localdatabase.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setStatus(QString sms);
    void afficherAide();
    void affchierApropos();
private slots:
    bool quitter();

protected:
    virtual void closeEvent(QCloseEvent *ev) override;
private:
    void createMenu();
    void createToolBar();
    void createCentralWindow();
    void createSocket();

    //Variables membres
    QStackedWidget *centralWindow;
    EmploiWidget *emploiWidget;
    HomeWidget *homeWidget;

    QLabel *labelInfo;
    StudentSocket *studentSocket;
    LocalDatabase *database;
    QAction *aPrint, *aParametre;
};
#endif // MAINWINDOW_H
