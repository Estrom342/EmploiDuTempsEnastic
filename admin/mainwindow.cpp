#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Emploi ENASTIC");
    database = new EmploiDatabase();
    localDatabase = new LocalDatabase("MainWindow::MainWindow");
    QApplication::setPalette(localDatabase->getPrametre().getPalete());
    lsNiveaux = new QComboBox();

    createMenu(); createToolBar();
    createCentralsWidgets();
    //createDocWidget();
    labelInfo = new QLabel("Non connecté");
    labelInfo->setStyleSheet("border: 1px solid rgb(218 , 218 , 218); border-radius:15px; padding:5px;");
    statusBar()->addWidget(labelInfo);

    auto me = this;
    connect(database, &EmploiDatabase::information, this, [me](QString title, QString message){
        QMessageBox::information(me, title, message);
    });
    connect(database, &EmploiDatabase::warning, this, [me](QString title, QString message){
        QMessageBox::warning(me, title, message);
    });
    connect(database, &EmploiDatabase::critical, this, [me](QString title, QString message){
        QMessageBox::critical(me, title, message);
    });
    connect(database, &EmploiDatabase::statut, this, [me](QString text){
        me->labelInfo->setText(text);
    });
    connect(database, &EmploiDatabase::connexionSignal, this, [me](){
        me->centralWindow->setCurrentWidget(me->emploiWidget);
    });
    connect(database, &EmploiDatabase::disConnexionSignal, this, [me](){
        me->centralWindow->setCurrentWidget(me->logineWidget);
    });
    database->setParent(nullptr);
    auto thread = new QThread();
    database->moveToThread(thread);
    connect(qApp, &QApplication::destroyed, database, [me](){
        if(me->database != 0){
            me->database->close();
            me->database->deleteLater();
        }
    });
    connect(database, &LocalDatabase::destroyed, thread, [thread](){
        thread->quit(); thread->wait(); thread->deleteLater();
    });
    thread->start();
    //qDebug()<<"MainWindow::MainWindow thread = "<<QThread::currentThread();
    //qDebug()<<"MainWindow::MainWindow thread  crée= "<<thread;
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    database->close();
    if(QMessageBox::question(this, "", tr("Fermer la fenetre et quitter l'appliction ?")) != QMessageBox::Yes)
        event->ignore();
}

void MainWindow::createMenu()
{
    auto me = this;

    //Menu administrateur

    auto menuAdmin = menuBar()->addMenu(tr("Admin"));
    auto menuAdministrateur = menuAdmin->addMenu(tr("Administrateur"));
    auto menuMonCompte = menuAdministrateur->addMenu(tr("Gérer mon compte"));

    //Changer le mot de passe
    menuMonCompte->addAction(tr("Changer mon mot de passe"), this, [me](){
        auto w = new QWidget();
        w->setWindowModality(Qt::ApplicationModal);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setWindowTitle(tr("Changement de mot de passe"));
        auto newPasseword1 = new QLineEdit(); newPasseword1->setEchoMode(QLineEdit::Password);
        auto newPasseword2 = new QLineEdit(); newPasseword2->setEchoMode(QLineEdit::Password);
        auto btnSave = new QPushButton(tr("Valider")); btnSave->setMaximumWidth(100);
        auto btnCancel = new QPushButton(tr("Annuler"));

        auto lF = new QFormLayout();
        lF->addRow(tr("Nouveau mot de passe"), newPasseword1);
        lF->addRow(tr("Confirmer le mot de passe"), newPasseword2);
        lF->addRow(btnCancel, btnSave);
        w->setLayout(lF);
        w->setFixedSize(680, 100);
        w->move(me->x()+(me->width() - w->width())/2, me->y()+(me->height() - w->height())/2);
        w->show();

        connect(btnSave, &QPushButton::clicked, me, [me, newPasseword1, newPasseword2, w](){
            if(newPasseword1->text().isEmpty()){
                QMessageBox::information(me, "", tr("Le mot de passe ne doit pas etre vide")); return;
            }if(newPasseword1->text() != newPasseword2->text()){
                QMessageBox::information(me, "", tr("Les mots de passe ne se correspondent pase")); return;
            }if(me->database->changePassword(newPasseword1->text()))
                w->close();
        });
        connect(btnCancel, &QPushButton::clicked, w, &QWidget::close);
    });

    //Ajout d'un administrateur

    menuAdministrateur->addAction(tr("Ajouter un administrateur"), this, [me](){
        auto w = new QWidget();
        w->setWindowModality(Qt::ApplicationModal);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setWindowTitle(tr("Ajout d'un administrateur"));
        auto login = new QLineEdit();
        auto passWord1 = new QLineEdit(); passWord1->setEchoMode(QLineEdit::Password);
        auto passWord2 = new QLineEdit(); passWord2->setEchoMode(QLineEdit::Password);
        auto btnSave = new QPushButton(tr("Ajouter")); btnSave->setMaximumWidth(100);
        auto btnCancel = new QPushButton(tr("Annuler"));
        auto lF = new QFormLayout();
        lF->addRow(tr("Login: "), login);
        lF->addRow(tr("Mot de passe: "), passWord1);
        lF->addRow(tr("Confirmer le mot de passe: "), passWord2);
        lF->addRow(btnCancel, btnSave);
        w->setLayout(lF);
        w->setFixedSize(600, 150);
        w->move(me->x()+(me->width() - w->width())/2, me->y()+(me->height() - w->height())/2);
        w->show();

        connect(btnSave, &QPushButton::clicked, me, [me, login, passWord1, passWord2, w](){
            QStringList deniedLogin; deniedLogin.clear();
            deniedLogin<<"mariadb.sys"<<"mysql"<<"root";
            if(passWord1->text().isEmpty() || login->text().isEmpty()){
                QMessageBox::information(me, "", tr("Le mot de passe ou le login ne doit pas etre vide"));
                return;
            }if(passWord1->text() != passWord2->text()){
                QMessageBox::warning(me, "", tr("Les mots de passe ne se correspondent pas"));
                return;
            }if(me->database->getLsAdmin().contains(login->text()) || deniedLogin.contains(login->text())){
                QMessageBox::warning(me, "", tr("Ce login est déjà utilisé par un autre administrateur"));
                login->setText("");
                return;
            }if(me->database->addAdmin(login->text(), passWord1->text()))
                w->close();
        });
        connect(btnCancel, &QPushButton::clicked, w, &QWidget::close);
    });

    //Suppression d'un addminitrateur

    menuAdministrateur->addAction(tr("Supprimer un administrateur"), this, [me](){
        auto lsAdmin = me->database->getLsAdmin();
        bool ok = false;
        auto rep = QInputDialog::getItem(me, tr("Suppresion d'un administrateur"),
                                         tr("Choisir un administrateur à supprimer"), lsAdmin, 0, false, &ok);
        if(ok && !rep.isEmpty()){
            if(QMessageBox::question(me, tr("Demande de confirmation"),
                       QString("Êtes-vous sûr de voiloir supprimer %1 ?").arg(rep)) == QMessageBox::Yes)
                me->database->deleteAdmin(rep);
        }
    });
    //Section de filiere
    auto menuFiliere = menuAdmin->addMenu(tr("Filiere"));
    menuFiliere->addAction(tr("Creer un niveau"), this, [me](){
        auto w = new QWidget();
        w->setWindowModality(Qt::ApplicationModal);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setWindowTitle(tr("Ajout d'un niveau"));
        auto lF = new QFormLayout(); lF->setSpacing(10);
        auto niveau = new QLineEdit();
        auto nomSalle = new QComboBox(); nomSalle->setEditable(true);
        for(int i = 65; i< 65+26; i++)
            nomSalle->addItem(QString("Salle %1").arg(char(i)));
        auto btnSave = new QPushButton(tr("Enregistrer")); btnSave->setMaximumWidth(100);
        auto btnCancel = new QPushButton(tr("Annuler"));
        lF->addRow(tr("Nom de niveau/filiere: "), niveau);
        lF->addRow(tr("Nom de salle "), nomSalle);
        lF->addRow(btnCancel, btnSave);

        w->setLayout(lF);
        w->move(me->x()+(me->width() - w->width())/2, me->y()+(me->height() - w->height())/2);
        w->show();
        w->setFixedSize(w->size() + QSize(200, 0));
        connect(btnSave, &QPushButton::clicked, me, [me, niveau, w, nomSalle](){
           if(niveau->text().isEmpty()){
               QMessageBox::critical(me, tr("Niveau non enregistré"), tr("Vous ne pouvez pas ajouter un niveau sans nom."));
               return;
           }
           if(me->database->getIdNiveau(niveau->text()) != -1){
               int rep = QMessageBox::question(me, tr("Niveau non enregistré"), tr("Le niveau que vous "
                                "voulez ajouter existe déjà. Voulez-vous ajouter un autre?"));
               if(rep == QMessageBox::Yes){
                   niveau->setText("");
                   return;
               }
           }else{
               if(me->database->addNiveau(niveau->text(), nomSalle->currentText())){
                   QString currentText = me->lsNiveaux->currentText();
                   me->lsNiveaux->setCurrentText(currentText);
                   int rep = QMessageBox::question(me, tr("Niveau enregistré"), tr("Voulez-vous ajouter encore ?"));
                   if(rep == QMessageBox::Yes){
                       niveau->setText("");
                       return;
                   }
               }
           }
           w->close();
        });
        connect(btnCancel, &QPushButton::clicked, w, &QWidget::close);
    });

    //Modifier un niveau

    menuFiliere->addAction(tr("Modifier un niveau"), this, [me](){
        auto w = new QWidget();
        w->setWindowModality(Qt::ApplicationModal);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setWindowTitle(tr("Modification d'un niveau"));
        auto niveau = new QLineEdit();
        auto nomSalle = new QComboBox(); nomSalle->setEditable(true); nomSalle->addItems(me->database->getLsSalles());
        auto comobox = new QComboBox(); comobox->addItems(me->database->getLsLevel());
        auto lsLevel =  me->database->getLsSalles();

        auto btnSave = new QPushButton(tr("Modifier")); btnSave->setMaximumWidth(100);
        auto btnCancel = new QPushButton(tr("Annuler"));
        auto lF = new QFormLayout();
        lF->addRow(tr("Niveau à modifier: "), comobox);
        lF->addRow(tr("Nouveau nom: "), niveau);
        lF->addRow(tr("Nouveau de salle: "), nomSalle);
        lF->addRow(btnCancel, btnSave);
        w->setLayout(lF);

        w->setFixedSize(680, 150);
        w->move(me->x()+(me->width() - w->width())/2, me->y()+(me->height() - w->height())/2);
        w->show();
        connect(btnSave, &QPushButton::clicked, me, [me, comobox, niveau, w, nomSalle](){
            if(comobox->currentText() == niveau->text() || comobox->currentText().isEmpty()){
                QMessageBox::information(me, "", tr("Auccune modifiaction éffectuée"));
                w->close(); return;
            }
            me->database->updateNiveau(comobox->currentText(), niveau->text(), nomSalle->currentText());
            w->close();
        });
        connect(comobox, &QComboBox::currentTextChanged, nomSalle, [me, comobox, nomSalle](){
            nomSalle->setCurrentText(me->database->getSalle(comobox->currentText()));
        });
        connect(btnCancel, &QPushButton::clicked, w, &QWidget::close);
    });
    menuFiliere->addAction(tr("Supprimer un niveau"), this, [me](){
        bool ok = false;
        QString level = QInputDialog::getItem(me, tr("Suppression d'un niveau"), tr("Choisir un niveau à suprimer"), me->database->getLsLevel(), 0, false, &ok);
        if(ok){
            int rep = QMessageBox::question(me, tr("Confirmation"), tr("Etes-vous sûr de vouloir supprimer ")+"\""+level+"\" ?");
            if(rep == QMessageBox::Yes)
                me->database->deleteNiveau(level);
        }
    });

    //Sectiondes enseignants

    auto menuEnseignant = menuAdmin->addMenu(tr("Enseignant"));
    //Ajout d'un enseignant
    menuEnseignant->addAction(tr("Ajouter un enseignant"), this, [me](){
        //me->centralWindow->setCurrentWidget(me->teacherWidget);
        me->teacherWidget->showAddWidget();
    });
    //Suppression d'un enseignant
    menuEnseignant->addAction(tr("Supprimer un enseignant"), this, [me](){
        auto lsTeacher = me->database->getlsTeacher();
        if(lsTeacher.isEmpty()){
            QMessageBox::information(me, "", tr("Auccun enseignant trouvé"));
            return;
        }
        QStringList lsNames; lsNames.clear();
        foreach (auto t, lsTeacher) {
            lsNames<<t.toString()+QString("(enseignant de \"%1\" en \"%2\")").arg(t.getNomMatiere()).arg(t.getNiveau());
        }
        bool ok =false;
        auto rep = QInputDialog::getItem(me, tr("Suppression d'un enseignant"),tr("Choisir l'enseignant à supprimer"),
                                         lsNames, 0, false, &ok);
        if(ok){
            Teacher teacher;
            foreach (auto t, lsTeacher) {
                if(rep == t.toString()+QString("(enseignant de \"%1\" en \"%2\")").arg(t.getNomMatiere()).arg(t.getNiveau())){
                    teacher = t; break;
                }
            }
            if(QMessageBox::question(me, tr("Confirmez la suppression"),
                                     tr("Etes-vous sûr de vouloir supprimer ")+rep+"?") == QMessageBox::Yes)
                me->database->deleteTeacher(teacher);
        }
    });
    //Modification d'un enseignant
    menuEnseignant->addAction(tr("Modifier un enseignat"), this, [me](){
        auto lsTeacher = me->database->getlsTeacher();
        if(lsTeacher.isEmpty()){
            QMessageBox::information(me, "", tr("Auccun enseignant trouvé"));
            return;
        }
        QStringList lsNames; lsNames.clear();
        foreach (auto t, lsTeacher) {
            lsNames<<t.toString();
        }
        bool ok =false;
        auto rep = QInputDialog::getItem(me, tr("Modification d'un enseignant"),tr("Choisir l'enseignant à modifier"),
                                         lsNames, 0, false, &ok);
        if(ok){
            Teacher teacher;
            foreach (auto t, lsTeacher) {
                if(rep == t.toString()){
                    teacher = t; break;
                }
            }
            me->teacherWidget->showEditWidget(teacher);
        }
    });

    //Parametre

    menuAdmin->addSeparator();
    menuAdmin->addAction(tr("Parametre"), this, [me](){
        auto w = new QWidget();
        auto label = new QLabel("Parametre"); label->setAlignment(Qt::AlignCenter);
        label->setFont(QFont("", 18, -1, true));
        auto parametre = me->localDatabase->getPrametre();
        if(parametre.getPortName() == 2222) parametre.setPortName(3306);
        w->setAttribute(Qt::WA_DeleteOnClose, true);
        w->setWindowModality(Qt::ApplicationModal);
        auto hostName = new QLineEdit(parametre.getHostName());
        auto portName = new QLineEdit(QString("%1").arg(parametre.getPortName())); portName->setInputMask("9999");
        auto login = new QLineEdit(parametre.getStudentName());
        auto password = new QLineEdit(parametre.getPassword()); password->setEchoMode(QLineEdit::Password);
        auto theme = new QComboBox();
        theme->addItem(PALETTE_CLEAR_TXT); theme->addItem(PALETTE_SOMBRE_TXT); theme->setCurrentText(parametre.getTheme());
        auto btnCancel = new QPushButton(tr("Annuler"));
        auto btnSave = new QPushButton(tr("Enregistrer"));
        auto lF = new QFormLayout(); lF->setSpacing(15);
        auto lBtn = new QHBoxLayout();
        lF->addWidget(label);
        lF->addRow(tr("Adresse du serveur de base de données: "), hostName);
        lF->addRow(tr("Numero de port: "), portName);
        lF->addRow(tr("Votre login: "), login);
        lF->addRow(tr("Mot de passe: "), password);
        lF->addRow(tr("Thème de la fenetre: "), theme);
        lBtn->addWidget(btnCancel, 0, Qt::AlignLeft); lBtn->addWidget(btnSave, 0, Qt::AlignRight);
        lF->addRow(lBtn);
        w->setLayout(lF); w->setFixedSize(500, 350);
        w->move(me->x()+(me->width() - w->width())/2, me->y()+(me->height() - w->height())/2);
        w->show();

        connect(btnSave, &QPushButton::clicked, me, [hostName, portName, login, password, theme, w, me](){
            auto parametre = me->localDatabase->getPrametre();
            parametre.setHostName(hostName->text());
            parametre.setPortName(portName->text().toInt());
            parametre.setStudentName(login->text());
            parametre.setTheme(theme->currentText());
            parametre.setPassword(password->text());
            me->localDatabase->setParametre(parametre);

            QApplication::setPalette(parametre.getPalete());
            w->close();
        });
        connect(btnCancel, &QPushButton::clicked, w, &QWidget::close);
    });
    menuAdmin->addSeparator();
    menuAdmin->addAction(tr("Quitter"), this, [me](){
        if(me->quitter())
            qApp->quit();
    })->setShortcut(QKeySequence(QKeySequence::Quit));

    menuAdmin->setEnabled(false);


    auto aMatiere = menuBar()->addAction(tr("Matiere"), this, [me](){
        me->centralWindow->setCurrentWidget(me->matiereWidget);
    }); aMatiere->setStatusTip(tr("Voir la liste des matières"));
    auto aEnseignant = menuBar()->addAction(tr("Enseignant"), this, [me](){
        me->centralWindow->setCurrentWidget(me->teacherWidget);
    }); aEnseignant->setStatusTip(tr("Voir la liste des enseignants"));
    aEnseignant->setEnabled(false);
    aMatiere->setEnabled(false);

    connect(database, &EmploiDatabase::connexionSignal, this, [aMatiere, aEnseignant, menuAdmin](){
        aEnseignant->setEnabled(true);
        aMatiere->setEnabled(true);
        menuAdmin->setEnabled(true);
    });
    connect(database, &EmploiDatabase::disConnexionSignal, this, [aMatiere, aEnseignant, menuAdmin](){
        aEnseignant->setEnabled(false);
        aMatiere->setEnabled(false);
        menuAdmin->setEnabled(false);
    });
    auto mApropos = menuBar()->addMenu(tr("A propos"));
    mApropos->addAction(tr("A propos du programme"), this, &MainWindow::afficherApropos);
    mApropos->addAction(tr("Aide"), this, &MainWindow::afficherAide);
    menuBar()->addAction(tr("Quitter"), this, [me](){
        if(me->quitter())
            qApp->quit();
    })->setShortcut(QKeySequence(QKeySequence::Quit));
}

void MainWindow::createToolBar()
{
    auto toolBAr = addToolBar(""); toolBAr->setEnabled(false);
    auto btnHEmploi = new QAction(tr("Derniers emplois du temps"));
    toolBAr->addAction(btnHEmploi);

    auto btnHistoty = new QAction(tr("Historique"));
    toolBAr->addAction(btnHistoty);

    auto btnAdd = new QAction("Nouvel Emploi");
    toolBAr->addAction(btnAdd);
    auto me = this;

    connect(btnAdd, &QAction::triggered, this, &MainWindow::createNewEmploi);
    connect(btnHEmploi, &QAction::triggered,this, [me](){
        me->centralWindow->setCurrentWidget(me->emploiWidget);
    });
    connect(btnHistoty, &QAction::triggered,this, [me](){
        me->centralWindow->setCurrentWidget(me->historyWidget);
    });
    //Charcher la liste des matiere apres la connexion a la base de donné et rendre la toolbar accesible
    connect(database, &EmploiDatabase::connexionSignal, this, [me, toolBAr](){
        toolBAr->setEnabled(true);
        me->menuBar()->setEnabled(true);
    });
    connect(database, &EmploiDatabase::disConnexionSignal, this, [me, toolBAr](){
        toolBAr->setEnabled(false);
        me->menuBar()->setEnabled(false);
    });

}

void MainWindow::createCentralsWidgets()
{
    centralWindow = new QStackedWidget();
    logineWidget = new LoginWidget(localDatabase); logineWidget->setDatabase(database);
    emploiWidget = new EmploiWidget(); emploiWidget->setDatabase(database);
    newEmploiWidget = new NewEmploiWidget(); newEmploiWidget->setDatabase(database); newEmploiWidget->setLocalDatabase(localDatabase);
    matiereWidget = new MatiereWidget(); matiereWidget->setDatabase(database);
    teacherWidget = new TeacherWidget; teacherWidget->setDatabase(database);
    historyWidget = new TimeTableHistoryWidget();
    historyWidget->setDatabase(database);

    centralWindow->addWidget(logineWidget);
    centralWindow->addWidget(emploiWidget);
    centralWindow->addWidget(newEmploiWidget);
    centralWindow->addWidget(matiereWidget);
    centralWindow->addWidget(teacherWidget);
    centralWindow->addWidget(historyWidget);

    centralWindow->setCurrentWidget(logineWidget);
    setCentralWidget(centralWindow);

    connect(newEmploiWidget, &NewEmploiWidget::saveSignal, emploiWidget, &EmploiWidget::lireEmploi);
    connect(newEmploiWidget, &NewEmploiWidget::actualiser, emploiWidget,  &EmploiWidget::actualiser);
    connect(newEmploiWidget, &NewEmploiWidget::actualiserMatiere, emploiWidget,  &EmploiWidget::actualiserMatiere);
}

//Creation d'un nouvel emploi du temps

void MainWindow::createNewEmploi()
{
    if(centralWindow->currentWidget() != newEmploiWidget && newEmploiWidget->contentData()){
        centralWindow->setCurrentWidget(newEmploiWidget);
        return;
    }
    if(newEmploiWidget->contentData()){
        int rep = QMessageBox::question(this, "", tr("Le nouveel emploi en cours d'édition n'est pas encore "
                                                    "enregistré. Voulez vous l'écrasé ?"));
        if(rep == QMessageBox::No)
            return;
    }
    auto me = this;
    auto w = new QWidget();
    w->setWindowModality(Qt::ApplicationModal);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowTitle(tr("Creation d'un nouvel emploi"));
    auto niveau = new QComboBox(); niveau->addItems(database->getLsLevel());
    auto dateBox = new QLineEdit(); dateBox->setReadOnly(true);
    auto dateDebut = new QDateEdit(QDate::currentDate());
    auto dateFin = new QDateEdit(QDate::currentDate().addDays(6));
    auto antenne = new QComboBox();
    QStringList lsAntennes; lsAntennes<<tr("Ndjaména")<<tr("Sarh")<<tr("Amdjarasse");
    antenne->addItems(lsAntennes);

    auto fctADisplayDate = [dateBox, dateDebut, dateFin](){
        QString date = EmploieDuTemp::dateToString(dateDebut->date())
                +tr(" au ")+EmploieDuTemp::dateToString(dateFin->date());
        dateBox->setText(date);
    };
    fctADisplayDate();
    connect(dateDebut, &QDateEdit::dateChanged, me, fctADisplayDate);
    connect(dateFin, &QDateEdit::dateChanged, me, fctADisplayDate);


    auto btnAdd = new QPushButton(tr("Créer")); btnAdd->setMaximumWidth(100);
    auto btnCancel = new QPushButton(tr("Annuler"));

    auto lF = new QFormLayout();
    lF->addRow(tr("Niveau: "), niveau);
    lF->addRow(tr("Date de debut: "), dateDebut);
    lF->addRow(tr("Date de fin: "), dateFin);
    lF->addRow(tr("Antenne: "), antenne);
    lF->addRow(tr("Date: "), dateBox);
    lF->addRow(btnCancel, btnAdd);
    w->setLayout(lF);
    w->show();
    w->setFixedSize(600, w->height());
    w->move(me->x()+(me->width() - w->width())/2, me->y()+(me->height() - w->height())/2);

    connect(btnAdd, &QPushButton::clicked, this, [me, niveau, dateDebut, dateFin, antenne, w](){
        if(dateDebut->date() > dateFin->date()){
            QMessageBox::warning(me, "", tr("La date de fin de l'emploi du temps doit etre supérieure à la date de debut"));
            return;
        }
        auto date = dateDebut->date();
        EmploieDuTemp em(niveau->currentText());
        em.setDateDebut(dateDebut->date());
        em.setDateFin(dateFin->date());
        em.setLieuDeCreation(antenne->currentText());

        if(me->database->getIdEmploi(em) != -1){
            QString message = tr("L'emploi du %1 au %2 existe déjà pour la classe de \" %3 \"\n"
                                 "Voulez-vous apporter des modifications?");
            message = message.arg(EmploieDuTemp::dateToString(date));
            message = message.arg(EmploieDuTemp::dateToString(date.addDays(5)));
            message = message.arg(niveau->currentText());
            int reponse = QMessageBox::question(me, "", message);
            if(reponse == QMessageBox::Yes){
                me->newEmploiWidget->setUpdateAble(true);
                me->newEmploiWidget->setEmploi(me->database->getEmploi(em.niveau(), em.dateDebut(), em.dateFin()));
                auto txt = QString("Mise à jour de l'emploi du Temps du %1 au %2 <div style='background-color: #4CD259'>%3</div>");
                txt = txt.arg(EmploieDuTemp::dateToString(em.dateDebut()));
                txt = txt.arg(EmploieDuTemp::dateToString(em.dateFin()));
                txt = txt.arg(em.niveau());
                me->newEmploiWidget->setLabelText(txt);
                me->newEmploiWidget->setContentData(false);
            }else{
                me->newEmploiWidget->setUpdateAble(false);
                return;
            }
        }else{
            me->newEmploiWidget->setEmploi(em);
            me->newEmploiWidget->setUpdateAble(false);
        }
        w->close();
        me->centralWindow->setCurrentWidget(me->newEmploiWidget);
    });

    connect(btnCancel, &QPushButton::clicked, w, &QWidget::deleteLater);
}

void MainWindow::afficherApropos()
{
    QFile file(":/img/res/aPropos.html");
    if(file.open(QFile::ReadOnly)){
        QTextStream stream(&file);
        QMessageBox::information(this, tr("A propos"), stream.readAll());
    }else
        qDebug()<<"Error at  MainWindow::afficherAide: "<<file.errorString();

}
//Affche l'aide d'utilisation du programme
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
            pageHtml->show();
            pageHtml->move(x()+(width() - pageHtml->width())/2, y()+(height() - pageHtml->height())/2);
        }else
            qDebug()<<"Error at  MainWindow::afficherAide: "<<file.errorString();
}

//Quitter l'application
bool MainWindow::quitter(){
    return QMessageBox::question(this, "", tr("Fermer la fenetre et quitter l'appliction ?")) == QMessageBox::Yes;
}

