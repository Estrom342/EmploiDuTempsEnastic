#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include<QtGui>
#include<QtWidgets>
#include "model/emploidatabase.h"
#include "model/localdatabase.h"

class LoginWidget: public QWidget
{
    Q_OBJECT
public:
    LoginWidget(LocalDatabase* localdataBase = new LocalDatabase("LoginWidget::LoginWidget"));

    EmploiDatabase *getDatabase() const;
    void setDatabase(EmploiDatabase *newDatabase);

signals:
    void connectToHostSignal(QString const& login= "gek", QString password= "superMotDePAsse", quint16 portName = 3306, QString const& hostName = "localhost");
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    QLineEdit *lineEdith(QString text="");
    QPushButton *btnConnexion;

    QWidget *centralWidget;
    QLineEdit *hostName, *login, *password;
    EmploiDatabase *database;
    LocalDatabase *localDatabase;
};

#endif // LOGINWIDGET_H
