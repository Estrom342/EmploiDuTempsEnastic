#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Emploi ENASTIC");
    database = new LocalDatabase();
    //database = globalDatabase;
    labelInfo = new QLabel("Non connecté");
    emploiWidget = new EmploiWidget();
    createSocket();
    createMenu();
    createToolBar();
    createCentralWindow();
    labelInfo->setStyleSheet("border: 1px solid rgb(218 , 218 , 218); border-radius:15px; padding:5px;");
    statusBar()->addWidget(labelInfo);
    auto p = database->getPrametre();
    QApplication::setPalette(p.getPalete());
    aPrint->setEnabled(false);
}

MainWindow::~MainWindow(){}

void MainWindow::setStatus(QString sms)
{
    labelInfo->setText(sms);
}


//Affiche l'aide d'utilisation du programme
void MainWindow::afficherAide()
{
    QFile file(":/img/res/aide.html");
    if(file.open(QFile::ReadOnly)){
        auto pageHtml = new QTextEdit();
        pageHtml->setWindowModality(Qt::ApplicationModal);
        pageHtml->setAttribute(Qt::WA_DeleteOnClose);
        pageHtml->setFixedSize(680, 480);
        QTextStream stream(&file);
        pageHtml->setHtml(stream.readAll());
        file.close();
        pageHtml->setReadOnly(true);
#ifdef DESKTOP_EMPLOI
        pageHtml->show();
        pageHtml->move(x()+(width() - pageHtml->width())/2, y()+(height() - pageHtml->height())/2);
#endif
#ifdef ANDROID_EMPLOI
        pageHtml->showMaximized();
#endif
    }else
        qDebug()<<"Error at  MainWindow::afficherAide: "<<file.errorString();
}

//Affiche a propos de l'application
void MainWindow::affchierApropos()
{
    QFile file(":/img/res/aPropos.html");
    if(file.open(QFile::ReadOnly)){
        QTextStream stream(&file);
        QMessageBox::information(this, tr("A propos"), stream.readAll());
    }else
        qDebug()<<"Error at  MainWindow::afficherAide: "<<file.errorString();
}

bool MainWindow::quitter()
{
    int rep = QMessageBox::question(this, "", tr("Voulez-vous fermer la fenetre et quitter l'application ?"));
    if(rep == QMessageBox::Yes){
        QApplication::quit();
        return true;
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    if(quitter())
        ev->accept();
    else
        ev->ignore();
}

void MainWindow::createMenu()
{
    auto me = this;
    auto menuFile = menuBar()->addMenu(tr("&Fichier"));
    //menuFile->addAction(tr("Enregistrer"));
    auto aHistorique = menuFile->addAction(tr("Historique"));aHistorique->setStatusTip(tr("Voir les emplois des semaines passées"));
    aPrint= menuFile->addAction(tr("Imprimer"), this, [me](){
        me->centralWindow->setCurrentWidget(me->emploiWidget);
        me->emploiWidget->imprimer();
    });
    aPrint->setShortcuts(QKeySequence::Print);
    connect(aHistorique, &QAction::triggered, this, [me](){
        auto calendrier = new QCalendarWidget();
        calendrier->setMinimumDate(QDate(2021, 1, 1));
        calendrier->setMaximumDate(QDate::currentDate());
        auto cadre = new QWidget();
        auto btnChoose = new QPushButton(tr("Choisir"));
        cadre->setWindowTitle("Choisir une date");
        auto lv = new QVBoxLayout();
        lv->addWidget(calendrier);
        lv->addWidget(btnChoose, 0, Qt::AlignCenter);
        cadre->setLayout(lv);
        cadre->setWindowModality(Qt::ApplicationModal);
        cadre->setAttribute(Qt::WA_DeleteOnClose, true);
        cadre->show();
        cadre->setMaximumSize(cadre->size());
        cadre->move((me->width() - cadre->width())/2, (me->height() - cadre->height())/2);

        connect(btnChoose, &QPushButton::clicked, me, [me, calendrier, cadre](){
           auto dateDebut = calendrier->selectedDate();
           me->centralWindow->setCurrentWidget(me->emploiWidget);
           me->emploiWidget->readEmplois(dateDebut);
           me->aPrint->setEnabled(me->emploiWidget->isPrintAble());
           cadre->close();
        });
    });
    menuFile->addSeparator();
    aParametre = menuFile->addAction(tr("Parametre"), this, [me](){
        auto w = new QWidget();
        auto label = new QLabel("Parametre"); label->setAlignment(Qt::AlignCenter);
        label->setFont(QFont("", 18, -1, true));
        auto parametre = me->database->getPrametre();
        w->setAttribute(Qt::WA_DeleteOnClose, true);
        w->setWindowModality(Qt::ApplicationModal);
        auto hostName = new QLineEdit(parametre.getHostName());
        auto portName = new QLineEdit(QString("%1").arg(parametre.getPortName())); portName->setInputMask("9999");
        auto niveau = new QComboBox(); niveau->addItems(me->database->getLsLevel()); niveau->setCurrentText(parametre.getLevel());
        auto studentname = new QLineEdit(parametre.getStudentName());
        auto theme = new QComboBox();
        theme->addItem(PALETTE_CLEAR_TXT); theme->addItem(PALETTE_SOMBRE_TXT); theme->setCurrentText(parametre.getTheme());
        auto btnCancel = new QPushButton(tr("Annuler"));
        auto btnSave = new QPushButton(tr("Enregistrer"));
        auto lF = new QFormLayout(); lF->setSpacing(15);
        auto lBtn = new QHBoxLayout();
        lF->addWidget(label);
        lF->addRow(tr("Adresse du serveur d'emploi: "), hostName);
        lF->addRow(tr("Numero de port: "), portName);
        lF->addRow(tr("Votre filiere: "), niveau);
        lF->addRow(tr("Votre nom: "), studentname);
        lF->addRow(tr("Thème de la fenetre: "), theme);
        lBtn->addWidget(btnCancel, 0, Qt::AlignLeft); lBtn->addWidget(btnSave, 0, Qt::AlignRight);
        lF->addRow(lBtn);
        w->setLayout(lF); w->setFixedSize(500, 350);
        w->move(me->x()+(me->width() - w->width())/2, me->y()+(me->height() - w->height())/2);
        w->show();

        connect(btnSave, &QPushButton::clicked, me, [hostName, portName, niveau, studentname, theme, w, me](){
            auto parametre = me->database->getPrametre();
            parametre.setHostName(hostName->text());
            parametre.setPortName(portName->text().toInt());
            parametre.setLevel(niveau->currentText());
            parametre.setStudentName(studentname->text());
            parametre.setTheme(theme->currentText());
            me->database->setParametre(parametre);

            QApplication::setPalette(parametre.getPalete());
            w->close();
        });
        connect(btnCancel, &QPushButton::clicked, w, &QWidget::close);
    });
    menuFile->addSeparator();
    menuFile->addAction(tr("Quitter"), this, &MainWindow::quitter)->setShortcut(QKeySequence::Quit);
    //menuBar()->addAction(tr("Enregistrer"));
    menuFile->addAction(tr("A propos"), this, &MainWindow::affchierApropos);
    menuFile->addAction(tr("Aide"), this, &MainWindow::afficherAide);
    menuBar()->addAction(tr("A propos"), this, &MainWindow::affchierApropos);
    menuBar()->addAction(tr("Aide"), this, &MainWindow::afficherAide);
    menuBar()->addAction(tr("Quitter"), this, &MainWindow::quitter)->setStatusTip(tr("Fermer la fenentre et quitter l'application"));
}

void MainWindow::createToolBar()
{
    auto toolBar = addToolBar("");
    auto lsNiveau = database->getLsLevel();

    auto btnHome = new QAction(tr("Home"));
    toolBar->addAction(btnHome);
    toolBar->addAction(aParametre);

    auto me = this;
    connect(btnHome, &QAction::triggered, this, [me](){
        me->centralWindow->setCurrentWidget(me->homeWidget);
        me->aPrint->setEnabled(false);
    });
    toolBar->addAction(aPrint);aPrint->setEnabled(false);
}

void MainWindow::createCentralWindow()
{
    centralWindow = new QStackedWidget();
    homeWidget = new HomeWidget();

    centralWindow->addWidget(emploiWidget);
    centralWindow->addWidget(homeWidget);
    centralWindow->setCurrentWidget(homeWidget); aPrint->setEnabled(emploiWidget->isPrintAble());
    setCentralWidget(centralWindow);

    //Connection
    auto me = this;
    connect(homeWidget, &HomeWidget::showEmploiSignal, this, [me](){

        QStringList lsLevel = me->database->getLsLevel();

        if(!lsLevel.isEmpty()){
            auto w = new QWidget();
            w->setWindowTitle(" ");
            w->setWindowModality(Qt::ApplicationModal);
            w->setAttribute(Qt::WA_DeleteOnClose);
            auto lS = new QVBoxLayout();
            auto lW = new QVBoxLayout();
            auto label = new QLabel(tr("Choisir un niveau"));
            label->setAlignment(Qt::AlignCenter); label->setFont(QFont("", 16, -1, true));
            lW->addWidget(label);
            auto scroll = new QScrollArea();
            foreach(auto l, lsLevel){
                auto btn = new QPushButton(l);
                btn->setFlat(true);
                lS->addWidget(btn);
                auto line = new QLabel();
                line->setFixedHeight(1); line->setMinimumWidth(150);
                line->setStyleSheet("border: 1px solid black;");
                lS->addWidget(line);

                connect(btn, &QPushButton::clicked, me, [btn, me, w]{
                    me->centralWindow->setCurrentWidget(me->emploiWidget);
                    me->aPrint->setEnabled(me->emploiWidget->isPrintAble());
                    me->emploiWidget->setCurrentLevel(btn->text());
                    w->close();
                });
            }
            auto WScroll = new QWidget();
            WScroll->setLayout(lS);
            scroll->setWidget(WScroll);
            lW->addWidget(scroll);
            w->setLayout(lW);
            w->show();
            w->move((me->width() - w->width())/2, (me->height() - w->height())/2);
            w->setFixedSize(w->size());
        }else{
            me->centralWindow->setCurrentWidget(me->emploiWidget);
            me->aPrint->setEnabled(me->emploiWidget->isPrintAble());
        }
    });
}

void MainWindow::createSocket()
{
    studentSocket = new StudentSocket(nullptr);
    studentSocket->setObjectName(MAIN_SOCKET_NAME); //Motre que c'est la connexion principale
    connect(studentSocket, &StudentSocket::status, this, &MainWindow::setStatus);
    studentSocket->tryToConnect();
    auto t = new QThread();
    studentSocket->setParent(nullptr);
    studentSocket->moveToThread(t);

    connect(studentSocket, &StudentSocket::emploisAvailables, emploiWidget, &EmploiWidget::readLastEmploi);
    connect(studentSocket, &StudentSocket::emploiAvailable, emploiWidget, &EmploiWidget::readEmploi);
    connect(studentSocket, &StudentSocket::signatuereAvaible, emploiWidget, &EmploiWidget::setSignature);
    connect(emploiWidget, &EmploiWidget::askEmploiSignal, studentSocket, &StudentSocket::askEmploi);
    connect(qApp, &QApplication::destroyed, t, [t](){t->exit();});
    connect(t, &QThread::finished, t, &QThread::deleteLater);
    t->start();
}

