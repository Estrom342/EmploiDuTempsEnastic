#include "newemploiwidget.h"

NewEmploiWidget::NewEmploiWidget(QWidget *parent) : QWidget(parent)
{
    auto logo = new QLabel();
    m_updateAble = false;
    m_contentData = false;
    label = new QLabel("Nouvel Emploi du Temps du xx au yy zzzzzzzz 0000 <div style='background-color: #4CD259'>L1 ITM</div>");
    label->setFont(QFont("", 14));
    label->setStyleSheet("QLabel{background-color: #4F68F7;}");
    label->setAlignment(Qt::AlignHCenter);

    logo->setPixmap(QPixmap(":/img/img/logoenastic1.png"));
    logo->setAlignment(Qt::AlignLeft);
    QStringList jours;
    jours<<"Horaire"<<"Lundi"<<"Mardi"<<"Mercredi"<<"Jeudi"<<"Vendredi"<<"Samedi";

    lsHoraireView.clear();
    lsMatiereView.clear();

    //Semestre box
    m_semestreBox = new QComboBox();
    lsSemestres<<"S1"<<"S2"<<"S3"<<"S4"<<"S5"<<"S6";
    m_semestreBox->addItems(lsSemestres);
    auto lv = new QVBoxLayout();
    auto lh = new QHBoxLayout();
    lh->addWidget(logo,0, Qt::AlignLeft);
    lh->addWidget(m_semestreBox, 0, Qt::AlignRight);

    lv->addItem(lh);
    lv->addWidget(label,0, Qt::AlignHCenter);

    auto lDays = new QHBoxLayout(); //lDays->setSpacing(10);
    foreach(auto jour, jours) {
        auto l = new QLabel(jour);
        l->setStyleSheet("background-color: #4F68F7; border: 1px solid black; "
                    "text-align: center; vertical-align: middle;");
        l->setFont(QFont("", 13));
        l->setAlignment(Qt::AlignCenter);
        lDays->addWidget(l);
    }
    lv->addItem(lDays);
    auto table = new QGridLayout();
    int i=0, j=0;
    for(int k =0; k< 5; k++) {
        auto label = new HoraireView(Horaire(k+1));
        lsHoraireView<<label;
        //label->setHtml("<div style = 'text-align:center';>"+h+"</div>");
        if(k%2==1 && k!=0)
            label->setMaximumHeight(30);
        label->setStyleSheet("background-color: rgb(255, 255, 255); border: 1px solid black; "
                    "text-align: center; vertical-align: middle;");
        label->setMaximumWidth(MatiereView().maximumWidth());
        label->setFont(MatiereView().font());

        table->addWidget(label, i++, 0);
    }
    int numCase = 1;
    for(i=0;i< 5; i++){
        for(j=1; j<=6; j++){
            if(i == 1 || i == 3){
                auto label = new QLabel("Pause");
                label->setMaximumHeight(30);
                label->setStyleSheet("background-color: rgb(255, 255, 255); border: 1px solid black; "
                    "text-align: center; vertical-align: middle;");
                label->setFont(QFont("", 11));
                label->setAlignment(Qt::AlignCenter);
                table->addWidget(label, i, j);
                continue;
            }
            Matiere matiere;
            matiere.setNumeroCase(numCase);
            auto matiereView=  new MatiereView(matiere);
            matiereView->setNumeroCase(numCase);
            table->addWidget(matiereView, i, j);
            lsMatiereView.append(matiereView);
            numCase++;
        }

    }
    table->setHorizontalSpacing(0);
    table->setVerticalSpacing(0);
    lv->addItem(table);

    //Date et lieu d'enregistrement d'emploi

    dateCreation = new QDateEdit(QDate::currentDate());
    lieuCreation = new QComboBox();
    labelSignature = new QLabel();
    auto btnSignature = new QPushButton(tr("Choisir une signature"));
    labelSignature->setPixmap(QPixmap(":/img/img/signature.jpg").scaled(150, 100));
    lieuCreation->addItem(tr("Ndjaména")); lieuCreation->addItem(tr("Sarh")); lieuCreation->addItem(tr("Amdjarasse"));
    auto lInfo = new QGridLayout();
    lInfo->addWidget(lieuCreation, 0, 0);
    lInfo->addWidget(new QLabel(tr("le")), 0, 1);
    lInfo->addWidget(dateCreation, 0, 2);
    lInfo->addWidget(labelSignature, 1, 0);
    lInfo->addWidget(btnSignature, 2, 0);
    lInfo->setAlignment(Qt::AlignRight);
    lv->addLayout(lInfo);
    btnSave = new QPushButton("Enregistrer");
    //lv->addWidget(btnSave, 0, Qt::AlignHCenter);

    emploiWidget = new QWidget();
    emploiWidget->setLayout(lv);
    auto scroll = new QScrollArea();
    scroll->setWidget(emploiWidget);
    setLayout(new QVBoxLayout());
    layout()->addWidget(scroll);
    layout()->addWidget(btnSave);
    layout()->setAlignment(btnSave, Qt::AlignHCenter);

    //Connexion entre les matieres view et les signaux d'actualisation

    foreach (MatiereView *m1, lsMatiereView) {
        connect(m1, &MatiereView::actualiserMatiere, this, &NewEmploiWidget::actualiserMatiere);
        foreach (MatiereView *m2, lsMatiereView) {
            if(m1 == m2) continue;
            connect(m2, &MatiereView::colorChanged, m1, &MatiereView::setColor);
        }
    }
    connect(btnSave, &QPushButton::clicked, this, &NewEmploiWidget::saveEmploi);
    connect(btnSignature, &QPushButton::clicked, this, &NewEmploiWidget::choisirSignature);
    connect(m_semestreBox, &QComboBox::currentTextChanged, this, &NewEmploiWidget::updateSemeste);
}

void NewEmploiWidget::saveEmploi(){
    qDebug()<<"NewEmploiWidget::saveEmploi size = "<<currentEmploieDuTemp.lsMatieres().size();

    //Recuperation de liste des matieres
    QList<Matiere> lsMatieres;
    foreach (auto mv, lsMatiereView) {
        if(!mv->getMatiere().getName().isEmpty()){
            lsMatieres<<mv->getMatiere();
        }
    }
    currentEmploieDuTemp.setLsMatieres(lsMatieres);
    currentEmploieDuTemp.setSemestre(m_semestreBox->currentText());

    //Recuperation de liste des horaires
    QList<Horaire> lsHoraires;
    foreach (auto hv, lsHoraireView) {
        lsHoraires<<hv->horaire();
        qDebug()<<"NewEmploiWidget::saveEmploi: "<<hv->numeroCase()<<hv->horaire().toString();
    }
    currentEmploieDuTemp.setLsHoraire(lsHoraires);

    //Date et lieu de creation et signature
    currentEmploieDuTemp.setDateCreation(dateCreation->date());
    currentEmploieDuTemp.setLieuDeCreation(lieuCreation->currentText());
    currentEmploieDuTemp.setSignature(m_localDatabase->getPrametre().signature());

    //Enregistrement de l'emploi dans la base de données
    if(database->addEmploie(currentEmploieDuTemp)){
        currentEmploieDuTemp.setIdEmploi(database->getIdEmploi(currentEmploieDuTemp));
        emit saveSignal();
    }
    //Renitialiser l'emploi
    currentEmploieDuTemp = EmploieDuTemp();
    setEmploi(currentEmploieDuTemp);
    setUpdateAble(false);
    btnSave->setVisible(false);
    m_contentData = false;
}

//choisir une nouvelle signature
void NewEmploiWidget::choisirSignature()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Choisir une image de signature"), QDir::homePath(), "*.png *jpg *.jpeg *JPG *.svg");
    if(!fileName.isEmpty()){
        QFile file(fileName);
        if(file.open(QFile::ReadOnly)){
            auto data = file.readAll();
            currentEmploieDuTemp.setSignature(data);
            labelSignature->setPixmap(QPixmap(fileName).scaled(150, 100));
            file.close();
            auto p = m_localDatabase->getPrametre();
            p.setSignature(data);
            m_localDatabase->setParametre(p);
            if(m_updateAble){
                database->updateSignature(data, currentEmploieDuTemp.idEmploi());
                emit actualiser();
            }
        }
    }
}

void NewEmploiWidget::updateSemeste(const QString &txt)
{
    if(currentEmploieDuTemp.idEmploi() == -1 || database == 0)
        return;
    database->updateEmploie(currentEmploieDuTemp.idEmploi(), txt);
}

LocalDatabase *NewEmploiWidget::localDatabase() const
{
    return m_localDatabase;
}

void NewEmploiWidget::setLocalDatabase(LocalDatabase *newLocalDatabase)
{
    m_localDatabase = newLocalDatabase;
    QPixmap pix;
    pix.loadFromData(m_localDatabase->getPrametre().signature());
    if(!pix.isNull())
        label->setPixmap(pix.scaled(150, 100));
    else{
        labelSignature->setPixmap(QPixmap());
    }
}

void NewEmploiWidget::setContentData(bool newContentData)
{
    m_contentData = newContentData;
}

void NewEmploiWidget::resizeEvent(QResizeEvent *event)
{
    emploiWidget->resize(event->size()+QSize(-40, 100));
}

bool NewEmploiWidget::contentData() const
{
    return m_contentData;
}

//Rendre l'emploi éditatble en temps reel avec la base de données
void NewEmploiWidget::setUpdateAble(bool value)
{
    m_updateAble = value;
    foreach (auto matiereView, lsMatiereView) {
        matiereView->setUpdateable(value);
    }
    foreach (auto hv, lsHoraireView) {
        hv->setUpdateAble(value);
    }
    btnSave->setVisible(!value);
}

void NewEmploiWidget::setEditAble(bool value)
{
    foreach (auto mv, lsMatiereView) {
        mv->setEditable(value);
    }foreach (auto hv, lsHoraireView) {
        hv->setEditable(value);
    }
    m_semestreBox->setEnabled(value);
}

//Titre d el'emploi
void NewEmploiWidget::setLabelText(const QString &txt)
{
    label->setText(txt);
}

void NewEmploiWidget::setDatabase(EmploiDatabase *newDatabase)
{
    database = newDatabase;
    foreach(auto matiereView, lsMatiereView){
        matiereView->setDatabase(database);
    }
    foreach (auto hv, lsHoraireView) {
        hv->setDataBase(database);
    }
}

//Met un nouvell emploi du temps
void NewEmploiWidget::setEmploi(const EmploieDuTemp &em)
{
    qDebug()<<"NewEmploiWidget::setEmploi idEmlpo = "<<em.idEmploi();
    foreach (auto matiereView, lsMatiereView) {
        matiereView->setMatiere(Matiere());
        foreach (auto matiere, em.lsMatieres()) {
            if(matiere.getNumeroCase() == matiereView->getNumeroCase())
                matiereView->setMatiere(matiere);
        }
        auto m = matiereView->getMatiere();
        m.setIdEmploi(em.idEmploi());
        m.setNiveau(em.niveau());
        matiereView->setMatiere(m);
        matiereView->setIdEmploi(em.idEmploi());
    }
    currentEmploieDuTemp = em;
    auto txt = QString("Nouvel Emploi du Temps du %1 au %2 <div style='background-color: #4CD259'>%3</div>");
    txt = txt.arg(EmploieDuTemp::dateToString(em.dateDebut()));
    txt = txt.arg(EmploieDuTemp::dateToString(em.dateFin()));
    txt = txt.arg(em.niveau());
    label->setText(txt);
    lieuCreation->setCurrentText(em.lieuDeCreation());

    if(em.idEmploi() == -1){
        setEditAble(true);
        setUpdateAble(false);
    }
    else{
        if(em.dateDebut() >= EmploieDuTemp::currentMonday()){
            setUpdateAble(true);
            setEditAble(true);
        }else{
            setUpdateAble(false);
            setEditAble(false);
        }
    }
    m_contentData = true;

    //Affichage de date, lieu et signature
    dateCreation->setDate(em.dateCreation());
    lieuCreation->addItem(em.lieuDeCreation());
    QPixmap pix;
    pix.loadFromData(em.signature());
    if(pix.isNull())
        pix.loadFromData(m_localDatabase->getPrametre().signature());
    if(pix.isNull())
        pix = QPixmap();

    labelSignature->setPixmap(pix.scaled(150, 100));
    if(!em.semestre().isEmpty()){
        if(!lsSemestres.contains(em.semestre())){
            m_semestreBox->addItem(em.semestre());
            lsSemestres<<em.semestre();
        }
        m_semestreBox->setCurrentText(em.semestre());
    }
}

