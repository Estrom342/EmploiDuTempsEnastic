#include "loginwidget.h"

LoginWidget::LoginWidget(LocalDatabase *localdataBase):localDatabase(localdataBase)
{
    database = nullptr;
    //Les données locales
    auto p = localDatabase->getPrametre();

    auto lF = new QFormLayout(); lF->setSpacing(50);
    hostName = lineEdith(p.getHostName());
    login = lineEdith(p.getStudentName());
    password = lineEdith(p.getPassword());
    password->setEchoMode(QLineEdit::Password);
    centralWidget = new QWidget(this);

    auto label = new QLabel("Authentification"); label->setStyleSheet("color: blue");
    label->setFont(QFont("", 30, -1, true));
    auto logo = new QLabel(); logo->setPixmap(QPixmap(":/img/img/logoenastic1.png"));
    btnConnexion = new QPushButton(tr("Connexion"));
    btnConnexion->setMaximumWidth(100);
    btnConnexion->setCursor(QCursor(Qt::PointingHandCursor));

    lF->addWidget(logo); //lF->setAlignment(logo, Qt::AlignTop);
    lF->addWidget(label);
    lF->setSpacing(15);
    lF->addRow(tr("Host name: "), hostName);
    lF->addRow(tr("Login: "), login);
    lF->addRow(tr("Password: "), password);

    lF->addWidget(btnConnexion);
    centralWidget->setLayout(lF);
    centralWidget->resize(400, 400);

    auto me = this;
    connect(btnConnexion, &QPushButton::clicked, this, [me](){
        if(me->database == nullptr)
            return;
        me->btnConnexion->setEnabled(false);
        auto p = me->localDatabase->getPrametre();
        p.setHostName(me->hostName->text());
        emit me->connectToHostSignal(me->login->text(), me->password->text(), p.getPortName(), me->hostName->text());
    });
}

void LoginWidget::resizeEvent(QResizeEvent *event)
{
    int x = (width() - centralWidget->width())/2, y = (height() - centralWidget->height())/2;
    centralWidget->move(x, y);
    QWidget::resizeEvent(event);
}

QLineEdit *LoginWidget::lineEdith(QString text)
{
    auto line = new QLineEdit(text);
    //line->setMaximumWidth(200);
    return line;
}

EmploiDatabase *LoginWidget::getDatabase() const
{
    return database;
}

void LoginWidget::setDatabase(EmploiDatabase *newDatabase)
{
    auto me = this;
    database = newDatabase;
    connect(this, &LoginWidget::connectToHostSignal, database, &EmploiDatabase::connectToHost);
    connect(database, &EmploiDatabase::connexionSignal, this, [me](){
        auto p = me->localDatabase->getPrametre();
        p.setHostName(me->hostName->text());
        me->btnConnexion->setEnabled(false);
        if(p.getPassword() != me->password->text()){
            if(QMessageBox::question(me, "", tr("Memoriser le mot de passe?")) == QMessageBox::Yes)
                p.setPassword(me->password->text());
            else
                p.setPassword("");
        }
        me->localDatabase->setParametre(p);
    });
    connect(database, &EmploiDatabase::disConnexionSignal, this, [me](){
        me->btnConnexion->setEnabled(true);
    });
}
