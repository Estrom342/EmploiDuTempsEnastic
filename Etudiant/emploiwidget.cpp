#include "emploiwidget.h"
#include "model/matiere.h"

EmploiWidget::EmploiWidget(QWidget *parent) : QWidget(parent)
{
    database = new LocalDatabase(DATABASENAME, "EmploiWidget::EmploiWidget");
    lsEmplois.clear(); offset = 0;
    lsMatiereView.clear();
    auto logo = new QLabel();
    label = new QLabel("Emploi de temp de xx au yy 2021 L1 ITM");
    labelSignature = new QLabel("Signature");
    btnNext = pushButton();
    btnNext->setIcon(QIcon(":/img/res/next.png")); btnNext->setIconSize(QSize(32, 32)); btnNext->setFixedSize(45, 32);
    connect(btnNext, &QPushButton::clicked, this, &EmploiWidget::next);
    btnPreview = pushButton(); btnPreview->setEnabled(false); btnPreview->setFixedSize(45, 32);
    btnPreview->setIcon(QIcon(":/img/res/preview.png")); btnPreview->setIconSize(QSize(32, 32));
    connect(btnPreview, &QPushButton::clicked, this, &EmploiWidget::preview);

    label->setFont(QFont("", 14));
    //label->setMaximumWidth(QGuiApplication::primaryScreen()->availableSize().width()*4/5);
    label->setStyleSheet("QLabel{background-color: #4F68F7;}");
    label->setAlignment(Qt::AlignHCenter);

    logo->setPixmap(QPixmap(":/img/res/logoenastic1.png"));
    logo->setAlignment(Qt::AlignLeft);
    QStringList jours, horaire;
    jours<<"Horaire"<<"Lundi"<<"Mardi"<<"Mercredi"<<"Jeudi"<<"Vendredi"<<"Samedi";
    horaire<<"07h30 à 9h 30h"<<"9h30 à 09h40"<<"9h40 à 12h 40"<<"12h40 à 13h40"<<"13h40 à 15h40";

    auto lv = new QVBoxLayout();
    auto lh = new QHBoxLayout();
    lh->addWidget(logo,0, Qt::AlignLeft);
    lh->addWidget(btnPreview);
    lh->addWidget(btnNext);
    lh->setAlignment(btnPreview, Qt::AlignRight);

    lv->addItem(lh);
    lv->addWidget(label,0, Qt::AlignHCenter);

    auto lDays = new QHBoxLayout(); //lDays->setSpacing(10);
    foreach(auto jour, jours) {//Ajout de jour de la semaine
        auto l = new QLabel(jour);
        l->setStyleSheet("background-color: #4F68F7; border: 1px solid black; "
                    "text-align: center; vertical-align: middle;");
        //l->setMaximumWidth(QApplication::primaryScreen()->availableSize().width()/7 -1);
        l->setFont(QFont("", 13));
        l->setAlignment(Qt::AlignCenter);
        lDays->addWidget(l);
    }
    lv->addItem(lDays);
    auto table = new QGridLayout();
    int i=0, j=0;
    //ajout de la colone d'horaire
    for (int k = 0; k< horaire.size(); k++) {
        auto label = new HoraireView(Horaire(k+1));
        label->setNumeroCase(k+1);
        lsHoraireView.append(label);        //Ajout de liste de champ d'affichage des horaires
        if(k%2 != 0 && k!=0)
            label->setMaximumHeight(30);
        //label->setHtml("<div style= 'text-align:center';>"+ h + "</div>");
        label->setStyleSheet("background-color: rgb(255, 255, 255); border: 1px solid black; "
                    "text-align: center; vertical-align: middle;");
        //label->setMaximumWidth(QApplication::primaryScreen()->availableSize().width()/7);
        table->addWidget(label, i++, 0);
    }

    int numCase = 1;
    for(i=0;i< 5; i++){ //Ajout des matieres
        for(j=1; j<=6; j++){
            if(i == 1 || i== 3){
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
            matiereView->setNumeroCase(numCase++);
            matiereView->setUpdateable(true);
            lsMatiereView.append(matiereView);          //Ajout de liste de champ d'affichage des horaires
            table->addWidget(matiereView, i, j);
        }
    }
    table->setHorizontalSpacing(0);
    table->setVerticalSpacing(0);
    lv->addItem(table);
    //Label pour afficher la date et le lieux de creation de l'emploi
    labelDateLieu = new QLabel("Ndjaména le 10/10/2021");
    lv->addWidget(labelDateLieu, 0, Qt::AlignRight);
    lv->addWidget(labelSignature, 0, Qt::AlignRight);

    centralWindow = new QStackedWidget();

    emploiWindow = new QScrollArea();
    emploiCentent = new QWidget();
    emploiCentent->setLayout(lv);
    emploiWindow->setWidget(emploiCentent);
    lh->setAlignment(btnPreview, Qt::AlignLeft);
    lh->setAlignment(btnNext, Qt::AlignHCenter);
    btnNext->setFixedSize(QSize(50, 32)); btnNext->setIconSize(QSize(70, 32));
    btnPreview->setFixedSize(QSize(50, 32)); btnPreview->setIconSize(QSize(70, 32));

    //auto me = this;
    //connect(QApplication::primaryScreen(), &QScreen::primaryOrientationChanged, this, [me](
    //        Qt::ScreenOrientation orientation){
    //    //me->emploiWindow->resize(QApplication::primaryScreen()->availableSize() * 95/100);
    //    me->emploiWindow->resize(me->size());
    //    qDebug()<<orientation;
    //    qDebug()<<"Geometrie= "<<QApplication::primaryScreen()->availableGeometry();
    //    qDebug()<<"Size = "<<QApplication::primaryScreen()->availableSize();
    //    qDebug()<<"Virtual Size = "<<QApplication::primaryScreen()->virtualSize();
    //});
    emploiCentent->resize(size());
    emploiVideWindow = createEmploiVidWindow();
    centralWindow->addWidget(emploiWindow);
    centralWindow->addWidget(emploiVideWindow);
    centralWindow->setCurrentWidget(emploiVideWindow);
    setLayout(new QVBoxLayout());
    layout()->addWidget(centralWindow);



    //lECTURE DES DONNEES DE LA DATABASE LOCAL
    foreach (auto matiereView, lsMatiereView) {
        matiereView->setDatabase(database);
    }
    readLastEmploi();
}

QPushButton *EmploiWidget::pushButton(QString txt)
{
    auto btn = new QPushButton(txt);
    btn->setMaximumWidth(100);
    return btn;
}

const EmploieDuTemp &EmploiWidget::getEmploi() const
{
    return emploi;
}

void EmploiWidget::setCurrentLevel(const QString &level)
{
    //Enregistrement des matieres existantes
    if(!lsEmplois.contains(level)) return;

    QList<Matiere> lsMatiere; lsMatiere.clear();
    foreach (auto matiereView, lsMatiereView) {
        lsMatiere.append(matiereView->getMatiere());
    }
    //Recueration des horaires
    QList<Horaire> lsHoraires; lsHoraires.clear();
    foreach (auto hv, lsHoraireView) {
        lsHoraires.append(hv->horaire());
    }
    EmploieDuTemp em(objectName());
    em.setLsMatieres(lsMatiere); em.setLsHoraire(lsHoraires);
    em.setIdEmploi(lsEmplois[objectName()].idEmploi());
    em.setDateCreation(lsEmplois[objectName()].dateCreation());
    em.setLieuDeCreation(lsEmplois[objectName()].lieuDeCreation());
    em.setSemestre(lsEmplois[objectName()].semestre());
    em.setNomSalle(lsEmplois[objectName()].nomSalle());
    em.setSignature(lsEmplois[objectName()].signature());
    lsEmplois[objectName()]= em;

    //Chargement des matiere selon le niveau level
    lsMatiere = lsEmplois.value(level, EmploieDuTemp(level)).lsMatieres();
    foreach (auto matiereView, lsMatiereView) {
        matiereView->setIdEmploi(lsEmplois[level].idEmploi());
        Matiere matiere;
        matiere.setNumeroCase(matiereView->getNumeroCase());
        matiere.setNiveau(level);
        matiereView->setMatiere(matiere);
        for(int i=0, j= lsMatiere.size(); i<j; i++){
            if(lsMatiere.at(i).getNumeroCase() == matiereView->getNumeroCase()){
                lsMatiere[i].setNiveau(level);
                matiereView->setMatiere(lsMatiere.at(i));
                break;
            }
        }
    }
    setObjectName(level);
    dateDebut = lsEmplois[level].dateDebut();
    setLabelText();
    setLabelDateLieu();
    setLabelSignature();

    if(centralWindow->currentWidget() != emploiWindow)
        centralWindow->setCurrentWidget(emploiWindow);
}

void EmploiWidget::setDefaultLevel(const QString level)
{
    if(level.isEmpty() || !lsEmplois.contains(level)) return;
    auto lsMatiere = lsEmplois[level].lsMatieres();

    dateDebut = lsEmplois[level].dateDebut();
    foreach (auto matiereView, lsMatiereView) {
        matiereView->setIdEmploi(lsEmplois[level].idEmploi());
        Matiere matiere;
        matiere.setNumeroCase(matiereView->getNumeroCase());
        matiere.setNiveau(level);
        matiereView->setMatiere(matiere);
        foreach (auto matiere, lsMatiere) {
            if(matiereView->getNumeroCase() == matiere.getNumeroCase()){
                matiere.setNiveau(objectName());
                matiereView->setMatiere(matiere);
            }
        }
    }
    auto lsHoraire = lsEmplois[level].lsHoraire();
    foreach (auto hv, lsHoraireView) {
        Horaire h;
        h.setNumeroCase(hv->numeroCase());
        hv->setHoraire(h);
        foreach (auto h, lsHoraire) {
            if(h.numeroCase() == hv->numeroCase())
                hv->setHoraire(h);
        }
    }
    offset = lsFiliere.indexOf(level);
    setObjectName(level);
    setLabelText();
    setLabelDateLieu();
    setLabelSignature();
}

const QMap<QString, EmploieDuTemp> &EmploiWidget::getLsEmplois() const
{
    return lsEmplois;
}

void EmploiWidget::setLsEmplois(const QMap<QString, EmploieDuTemp> &newLsEmplois)
{
    lsEmplois = newLsEmplois;
    if(!lsEmplois.isEmpty()){
        offset = 0;

    }
}

void EmploiWidget::setLabelText()
{
    QString label = QString("Emploi du Temps du %1 au %2 %3<div style='background-color: #4CD259'>%4 %5</div>");
    label = label.arg(EmploieDuTemp::dateToString(lsEmplois[objectName()].dateDebut()));
    label = label.arg(EmploieDuTemp::dateToString(QDate(lsEmplois[objectName()].dateFin()).addDays(5)));
    label = label.arg(lsEmplois[objectName()].semestre());
    label = label.arg(objectName());
    label = label.arg(lsEmplois[objectName()].nomSalle());
    this->label->setText(label);
}

void EmploiWidget::setLabelDateLieu()
{
    QString txt = QString("%1 %2").arg(lsEmplois[objectName()].lieuDeCreation());
    txt = txt.arg(lsEmplois[objectName()].dateCreation().toString("dd/MM/yyyy"));
    labelDateLieu->setText(txt);
}

//Affiche un message dans la fentre s'il n'y pas d'emploi disponible
void EmploiWidget::setLabelInfos()
{
    auto txt = QString("<div style='color:#418CD4;'>" +
                       tr("Auccun emploi du temps disponible pour cette cette semaine <div>(du %1 au %2)</div>")+"</div>");
    txt = txt.arg(EmploieDuTemp::dateToString(dateDebut));
    txt = txt.arg(EmploieDuTemp::dateToString(QDate(dateDebut).addDays(5)));
    labelInfos->setHtml(txt);
}

//Affiche la signature en desous de l'emploi actuel
void EmploiWidget::setLabelSignature()
{
    QPixmap pix;
    pix.loadFromData(lsEmplois[objectName()].signature());
    if(pix.isNull())
        labelSignature->setPixmap(QPixmap());
    else
        labelSignature->setPixmap(pix.scaled(150, 100));
}

bool EmploiWidget::isPrintAble()
{
    return centralWindow->currentWidget() == emploiWindow;
}

void EmploiWidget::next()
{
    if(lsFiliere.isEmpty()) return;
    offset++;
    offset = offset % lsFiliere.size();
    setCurrentLevel(lsFiliere[offset]);
    updateCursor();
}

void EmploiWidget::preview()
{
    if(lsFiliere.isEmpty()) return;
    offset--;
    offset = offset % lsFiliere.size();
    setCurrentLevel(lsFiliere[offset]);
    updateCursor();
}

void EmploiWidget::updateCursor()
{
    if(offset>0 && offset <lsFiliere.size() -1){
        btnNext->setEnabled(true);
        btnPreview->setEnabled(true);
    }
    if(offset == lsFiliere.size() - 1){
        btnNext->setEnabled(false);
        btnPreview->setEnabled(true);
    }
    if(offset == 0){
        btnNext->setEnabled(true);
        btnPreview->setEnabled(false);
    }
}

void EmploiWidget::emitAskeEmploiSignal()
{
    emit askEmploiSignal(lsEmplois[objectName()].dateDebut(), lsEmplois[objectName()].niveau());
}

QWidget *EmploiWidget::createEmploiVidWindow()
{
    auto widget = new QWidget();
    auto lv = new QVBoxLayout();
    auto logo = new QLabel();
    labelInfos = new QTextEdit("<div style='color:#418CD4;'>" +
                            tr("Auccun emploi du temps disponible pour cette cette semaine")+"</div>");
    labelInfos->setReadOnly(true); labelInfos->setEnabled(false);
    labelInfos->setFont(QFont("", 35, 5, true));
    logo->setPixmap(QPixmap(":/img/img/logoenastic1.png"));
    logo->setAlignment(Qt::AlignLeft);
    labelInfos->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    lv->addWidget(logo);
    lv->addWidget(labelInfos);
    widget->setLayout(lv);
    return widget;
}

void EmploiWidget::contextMenuEvent(QContextMenuEvent *event)
{
    auto me = this;
    auto menu = new QMenu("");
    menu->addAction(tr("Actualiser"), this, &EmploiWidget::emitAskeEmploiSignal);

    if(lsFiliere.size() > 0){
        auto aller = menu->addMenu(tr("Aller à"));
        foreach (auto txt, lsFiliere) {
            aller->addAction(txt, this, [me](){
                auto s = qobject_cast<QAction*>(me->sender());
                if(s == 0) return;
                me->setCurrentLevel(s->text());
                me->offset = me->lsFiliere.indexOf(s->text());
                me->updateCursor();
            });
        }
    }

    menu->show();
    menu->move(QCursor::pos());
    QWidget::contextMenuEvent(event);
}

bool EmploiWidget::event(QEvent *event)
{
    if(event->type() == QEvent::ShortcutOverride){
        QKeyEvent* keyPresse = (QKeyEvent*)event;
        if(keyPresse != 0){
            if(keyPresse->key() == Qt::Key_Right && btnNext->isEnabled())
                next();
            if(keyPresse->key() == Qt::Key_Left && btnPreview->isEnabled())
                preview();

        }
    }
    return QWidget::event(event);
}

void EmploiWidget::resizeEvent(QResizeEvent *event)
{
#ifdef DESKTOP_EMPLOI
    emploiCentent->resize(event->size() + QSize(-40, 40));
#endif
#ifdef ANDROID_EMPLOI
    emploiCentent->resize(QApplication::primaryScreen()->size() + QSize(-40, -40));
    //emploiCentent->resize(QApplication::primaryScreen()->size() + QSize(-40, QApplication::primaryScreen()->size().height()*1/200));
#endif
    QWidget::resizeEvent(event);
}
void EmploiWidget::readLastEmploi(){
    lsFiliere.clear();
    lsEmplois.clear();
    auto lsAllFilieres = database->getLsLevel(); // Recupere la liste des filieres
    lsAllFilieres.sort();
    if(lsAllFilieres.isEmpty()) return;

    dateDebut = EmploieDuTemp::currentMonday();
    QString defaultLevel = "";
    //Lecture de la liste de dernier emploi
    foreach(auto filiere, lsAllFilieres){
        auto emploi = database->getLastEmploi(filiere);
        if(emploi.lsMatieres().isEmpty())
            continue;
        lsFiliere.append(filiere);
        lsEmplois[filiere] = emploi;
        if(lsEmplois.size() == 1)
            defaultLevel = filiere;
    }
    if(lsFiliere.isEmpty()){
        centralWindow->setCurrentWidget(emploiVideWindow);
        setLabelInfos();
        return;
    }
    lsFiliere.sort();
    lsAllFilieres = lsFiliere;

    auto level = database->getPrametre().getLevel();
    if(lsFiliere.contains(level)){
        auto objName= objectName();
        setDefaultLevel(level);
        if(!objName.isEmpty())
            setCurrentLevel(objName);
    }
    else{
        auto objName= objectName();
        setDefaultLevel(lsAllFilieres.first());
        if(!objName.isEmpty())
            setCurrentLevel(objName);
    }
    auto keys = lsEmplois.keys();
    offset = keys.indexOf(objectName());
    updateCursor();
    centralWindow->setCurrentWidget(emploiWindow);
    if(lsFiliere.size() <= 1)
        btnNext->setEnabled(false);
}

//Lit l'emploi du temps en fonction de la date de l'historique choisie
void EmploiWidget::readEmplois(QDate &dateDebut)
{
    auto me = this;
    lsFiliere.clear();
    lsEmplois.clear();
    auto lsFiliere = me->database->getLsLevel(); // Recupere la liste des filieres
    lsFiliere.sort();
    if(lsFiliere.isEmpty()) return;
    int lundi= dateDebut.dayOfWeek(), i=0;
    while(lundi % Qt::Sunday != Qt::Monday){
        lundi--; i--;
    }
    dateDebut = dateDebut.addDays(i);
    auto dateFin = QDate(dateDebut).addDays(5);
    me->dateDebut = dateDebut;
    QString defaultLevel = "";
    foreach(auto filiere, lsFiliere){
        auto emploi = EmploieDuTemp(filiere);
        emploi.setDateDebut(dateDebut);
        int idEmploi = me->database->getIdEmploi(emploi);
        if(idEmploi != -1){
            me->lsFiliere.append(filiere);
            emploi = me->database->getEmploi(filiere, dateDebut, dateFin);
            lsEmplois[filiere] = emploi;

            if(me->lsEmplois.size() == 1)
                defaultLevel = filiere;
        }
    }
    me->lsFiliere.sort();
    if(!me->lsFiliere.isEmpty()){
        offset = 0;
        auto level = database->getPrametre().getLevel();
        if(me->lsFiliere.contains(level)){
            me->setDefaultLevel(level);
        }
        else{
            me->setDefaultLevel(defaultLevel);
            me->setCurrentLevel(lsFiliere.first());
        }
        auto keys = me->lsEmplois.keys();
        offset = keys.indexOf(objectName());
        updateCursor();
        me->centralWindow->setCurrentWidget(me->emploiWindow);
    }else{
        me->centralWindow->setCurrentWidget(me->emploiVideWindow);
        me->setLabelInfos();
    }
    updateCursor();
}

//Lit l'emploi de niveau
void EmploiWidget::readEmploi(QDate dateDebut, QString niveu)
{
    auto em = database->getEmploi(niveu, dateDebut, QDate(dateDebut).addDays(5));
    lsEmplois[em.niveau()] = em;
    if(objectName() == niveu)
        setCurrentLevel(em.niveau());
    updateCursor();
}

//Affiche et enregistre la signature
void EmploiWidget::setSignature(const QString &level, QString const&nameSalle, const QDate &dateDebut, const QDate &dateFin, const QByteArray &signature)
{
    if(lsFiliere.contains(level)){
        foreach (QString niveau, lsFiliere) {
            if(lsEmplois[niveau].dateDebut() == dateDebut && lsEmplois[niveau].niveau() == level
                    && lsEmplois[niveau].dateFin() == dateFin){
                lsEmplois[niveau].setSignature(signature);
                if(objectName() == level)
                    setLabelSignature();
            }
        }
    }
    EmploieDuTemp em(level);
    em.setDateDebut(dateDebut);
    em.setSignature(signature);
    em.setNomSalle(nameSalle);
    database->updateSignature(em);
    //qDebug()<<"EmploiWidget::setSignature";
}

//Imprime l'emploi du temps
void EmploiWidget::imprimer()
{
    btnNext->setVisible(false); btnPreview->setVisible(false);
    QPrinter imprimante;
    imprimante.setOutputFileName(QDir::homePath()+"/enactic_"+objectName()+"_"+EmploieDuTemp::dateToString(dateDebut));
    QPainter rendu;
    auto dialog = new QPrintDialog(&imprimante);
    dialog->resize(680, 150);
    imprimante.setPageOrientation(QPageLayout::Landscape);
    if(dialog->exec() == QDialog::Accepted){
        rendu.begin(&imprimante);

        auto taillMax = emploiCentent->size();
        double sx = imprimante.pageLayout().fullRectPoints().width()/ (double)taillMax.width();
        double sy = imprimante.pageLayout().fullRectPoints().height()/ (double)taillMax.height();
        double s = sx<= sy ? sx: sy;
        rendu.translate(imprimante.pageLayout().fullRectPoints().x()+imprimante.pageLayout().fullRectPoints().width()/2,
                        imprimante.pageLayout().fullRectPoints().y()+imprimante.pageLayout().fullRectPoints().height()/2);
        rendu.scale(s, s);
        rendu.translate(-taillMax.width()/2, -taillMax.height()/2);
        emploiCentent->render(&rendu);
        rendu.end();
    }
    btnNext->setVisible(true); btnPreview->setVisible(true);
    QMessageBox::information(this, "", imprimante.outputFileName()+"\nImprimer !");
}
