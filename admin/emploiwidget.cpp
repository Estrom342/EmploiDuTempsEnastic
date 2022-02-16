#include "emploiwidget.h"

EmploiWidget::EmploiWidget(QWidget *parent) : QWidget(parent)
{
    database = nullptr;
    lsEmplois.clear(); offset = 0;
    lsMatiereView.clear();
    lsHoraireView.clear();
    auto logo = new QLabel();
    label = new QLabel("Emploi de temp de xx au yy 2021 L1 ITM");
    btnNext = pushButton();
    btnNext->setIconSize(QSize(32, 32)); btnNext->setFixedSize(45, 32);
    btnNext->setIcon(QIcon(":/img/img/next.png"));
    connect(btnNext, &QPushButton::clicked, this, &EmploiWidget::next);
    btnPreview = pushButton();
    btnPreview->setIconSize(QSize(32, 32)); btnPreview->setFixedSize(45, 32);
    btnPreview->setIcon(QIcon(":/img/img/preview.png")); btnPreview->setEnabled(false);
    connect(btnPreview, &QPushButton::clicked, this, &EmploiWidget::preview);

    label->setFont(QFont("", 14));
    label->setStyleSheet("QLabel{background-color: #4F68F7;}");
    label->setAlignment(Qt::AlignHCenter);

    logo->setPixmap(QPixmap(":/img/img/logoenastic1.png"));
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
        l->setFont(QFont("", 13));
        l->setAlignment(Qt::AlignCenter);
        lDays->addWidget(l);
    }
    lv->addItem(lDays);
    auto table = new QGridLayout();
    int i=0, j=0;

    //ajout de la colone d'horaire

    //foreach (auto h, horaire) {
    for (int k=0; k<horaire.size(); k++) {
        //auto h = horaire.at(k);
        auto label = new HoraireView();
        label->setNumeroCase(k+1);
        if(k%2 == 1 && k!=0)
            label->setMaximumHeight(30);
        lsHoraireView.append(label); //Ajout le label de horaire
        label->setStyleSheet("border: 1px solid black; "
                    "text-align: center; vertical-align: middle;");
        label->setMaximumWidth(QApplication::primaryScreen()->availableSize().width()/7);
        label->setFont(MatiereView().font());
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
                table->addWidget(label, i, j);//Ajout de case de matiere
                continue;
            }
            Matiere matiere;
            matiere.setNumeroCase(numCase);
            auto matiereView=  new MatiereView(matiere);
            matiereView->setNumeroCase(numCase++);
            matiereView->setUpdateable(false);
            lsMatiereView.append(matiereView);
            table->addWidget(matiereView, i, j);
        }
    }

    //Connexion entre les matieres view

    foreach (MatiereView *m1, lsMatiereView) {
        foreach (MatiereView *m2, lsMatiereView) {
            if(m1 == m2) continue;
            connect(m2, &MatiereView::colorChanged, m1, &MatiereView::setColor);
        }
    }

    table->setHorizontalSpacing(0);
    table->setVerticalSpacing(0);
    lv->addItem(table);

    //Ajout de label de date de publlication et de signanture
    m_labelDateCreation = new QLabel("Fait à Ndjaména le 05/11/2021");
    m_labelSignature = new QLabel();
    auto lInfos = new QVBoxLayout();
    m_labelSignature->setPixmap(QPixmap(":/img/img/signature.jpg").scaled(150, 100));
    lInfos->addWidget(m_labelDateCreation);
    lInfos->addWidget(m_labelSignature);
    lv->addLayout(lInfos);
    lv->setAlignment(lInfos, Qt::AlignRight);

    //Ajou de bouton d'impression
    btnPrint = new QPushButton(tr("Imprimer")); btnPrint->setShortcut(QKeySequence(QKeySequence::Quit));
    lv->addWidget(btnPrint, 0, Qt::AlignRight);
    connect(btnPrint, &QPushButton::clicked, this, &EmploiWidget::imprimer);
    btnPrint->setShortcut(QKeySequence(Qt::Key_Print));

    centralWindow = new QStackedWidget();
    scrollEmploiWindow = new QScrollArea();
    emploiWindow = new QWidget();
    emploiVideWindow = createEmploiVidWindow();

    emploiWindow->setLayout(lv);
    scrollEmploiWindow->setWidget(emploiWindow);
    centralWindow->addWidget(scrollEmploiWindow);
    centralWindow->addWidget(emploiVideWindow);
    centralWindow->setCurrentWidget(emploiVideWindow);
    setLayout(new QVBoxLayout());
    layout()->addWidget(centralWindow);
}

QPushButton *EmploiWidget::pushButton(QString txt)
{
    auto btn = new QPushButton(txt);
    btn->setMaximumWidth(100);
    return btn;
}

void EmploiWidget::setDatabase(EmploiDatabase *newDatabase)
{
    database = newDatabase;
    //if(objectName() == QString("HISTORY"))
    //    return;
    foreach (auto matiereView, lsMatiereView) {
        matiereView->setDatabase(database);
    }
    foreach (auto hv, lsHoraireView) {
        hv->setDataBase(database);
    }
    //Apres la connexion a la base de données
    connect(database, &EmploiDatabase::connexionSignal, this, &EmploiWidget::lireEmploi);
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
    //Enregistrement des horaires
    QList<Horaire> lsHoraires;
    foreach (auto horaireView, lsHoraireView) {
        lsHoraires<<horaireView->horaire();
    }
    EmploieDuTemp em(objectName());
    em.setLsMatieres(lsMatiere); em.setLsHoraire(lsHoraires);
    em.setIdEmploi(lsEmplois[objectName()].idEmploi());
    em.setDateDebut(lsEmplois[objectName()].dateDebut());
    em.setDateCreation(lsEmplois[objectName()].dateCreation());
    em.setSignature(lsEmplois[objectName()].signature());
    em.setLieuDeCreation(lsEmplois[objectName()].lieuDeCreation());
    em.setSemestre(lsEmplois[objectName()].semestre());
    em.setNomSalle(lsEmplois[objectName()].nomSalle());
    lsEmplois[objectName()]= em;

    //Chargement des matiere selon le niveau level
    lsMatiere = lsEmplois.value(level, EmploieDuTemp(level)).lsMatieres();

    foreach (auto matiereView, lsMatiereView) {
        matiereView->setIdEmploi(lsEmplois[level].idEmploi());
        auto matiere = Matiere();
        matiere.setNumeroCase(matiereView->getNumeroCase());
        matiere.setNiveau(level);
        matiereView->setMatiere(matiere);
        foreach(auto matiere, lsMatiere){
            if(matiere.getNumeroCase() == matiereView->getNumeroCase()){
                matiere.setNiveau(level);
                matiereView->setMatiere(matiere);
                break;
            }
        }
    }

    //Chargement des horaires
    lsHoraires = lsEmplois[level].lsHoraire();
    foreach (auto hV, lsHoraireView) {
        hV->setHoraire(Horaire());
        foreach (auto h, lsHoraires) {
            if(h.numeroCase() == hV->numeroCase()){
                hV->setHoraire(h);
            }
        }
    }
    setObjectName(lsEmplois[level].niveau());
    dateDebut = lsEmplois[level].dateDebut();
    setLabelText();
    if(dateDebut >= EmploieDuTemp::currentMonday()){
        setUpdateAble(true);
        setEditAble(true);
    }
    else{
        setEditAble(false);
        setUpdateAble(false);
    }

    //Afficher la date de creation et la signature
    QString txt = QString("%1 le %2").arg(lsEmplois[level].lieuDeCreation());
    m_labelDateCreation->setText(txt.arg(lsEmplois[level].dateCreation().toString("dd/MM/yyyy")));
    QPixmap pix; pix.loadFromData(lsEmplois[level].signature());
    if(!pix.isNull())
        m_labelSignature->setPixmap(pix.scaled(150, 100));
    else
        m_labelSignature->setPixmap(QPixmap());
}

void EmploiWidget::setDefaultLevel(const QString level)
{
    if(level.isEmpty() || !lsEmplois.contains(level)) return;
    auto lsMatiere = lsEmplois[level].lsMatieres();
    auto lsHraire = lsEmplois[level].lsHoraire();

    foreach (auto matiereView, lsMatiereView) {
        matiereView->setIdEmploi(lsEmplois[level].idEmploi());
        auto matiere = Matiere();
        matiere.setNumeroCase(matiereView->getNumeroCase());
        matiere.setNiveau(level);
        matiereView->setMatiere(matiere);
        foreach (auto matiere, lsMatiere) {
            if(matiereView->getNumeroCase() == matiere.getNumeroCase()){
                matiere.setNiveau(level);
                matiereView->setMatiere(matiere);
            }
        }
    }
    int i=0;
    foreach(auto hv, lsHoraireView){
        hv->setIdEmploi(lsEmplois[level].idEmploi());
        Horaire horaire(i++);
        horaire.setIdEmploi(hv->idEmploi());
        hv->setHoraire(horaire);
        foreach (auto h, lsHraire) {
            if(hv->numeroCase() == h.numeroCase()){
                hv->setHoraire(h);
            }
        }
    }
    {
        offset = lsFiliere.indexOf(level);
        if(offset>0 && offset <lsFiliere.size() -1){
            btnNext->setEnabled(true);
            btnPreview->setEnabled(true);
        }if(offset == lsFiliere.size() - 1){
            btnNext->setEnabled(false);
            btnPreview->setEnabled(true);
        }if(offset == 0){
            btnNext->setEnabled(true);
            btnPreview->setEnabled(false);
        }
    }setObjectName(lsEmplois[level].niveau());
    dateDebut = lsEmplois[level].dateDebut();
    if(dateDebut >= EmploieDuTemp::currentMonday()){
        setUpdateAble(true);
        setEditAble(true);
    }
    else{
        setEditAble(false);
        setUpdateAble(false);
    }
    setLabelText();
    //Afficher la date de creation et la signature
    QString txt = QString("%1 le %2").arg(lsEmplois[level].lieuDeCreation());
    m_labelDateCreation->setText(txt.arg(lsEmplois[level].dateCreation().toString("dd/MM/yyyy")));
    QPixmap pix; pix.loadFromData(lsEmplois[level].signature());
    if(!pix.isNull())
        m_labelSignature->setPixmap(pix.scaled(150, 100));
    else
        m_labelSignature->setPixmap(QPixmap());
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
        lsFiliere = lsEmplois.keys();
        setDefaultLevel(lsEmplois.first().niveau());
        setCurrentLevel(lsEmplois.first().niveau());
        centralWindow->setCurrentWidget(scrollEmploiWindow);
    }
}

void EmploiWidget::setLabelText()
{
    QString txt = QString("Emploi du Temps du %1 au %2 %3"
                          "<div style='background-color: #4CD259'>%4 %5</div>");
    txt = txt.arg(EmploieDuTemp::dateToString(dateDebut));
    txt = txt.arg(EmploieDuTemp::dateToString(QDate(dateDebut).addDays(5)));
    txt = txt.arg(lsEmplois[objectName()].semestre());
    txt = txt.arg(objectName());
    txt = txt.arg(lsEmplois[objectName()].nomSalle());
    this->label->setText(txt);
}

void EmploiWidget::setUpdateAble(bool value)
{
    foreach (MatiereView* mv, lsMatiereView) {
        mv->setUpdateable(value);
    }
    foreach (HoraireView* hv, lsHoraireView) {
        hv->setUpdateAble(value);
    }
}

void EmploiWidget::setEditAble(bool value)
{
    foreach (MatiereView* mv, lsMatiereView) {
        mv->setEditable(value);
    }
    foreach (HoraireView* hv, lsHoraireView) {
        hv->setEditable(value);
    }
}

bool EmploiWidget::isPrintable()
{
    return centralWindow->currentWidget() == scrollEmploiWindow;
}

void EmploiWidget::next()
{
    if(lsFiliere.isEmpty()) return;
    offset++;
    offset = offset % lsFiliere.size();
    setCurrentLevel(lsFiliere[offset]);

    if(offset>0 && offset <lsFiliere.size() -1){
        btnNext->setEnabled(true);
        btnPreview->setEnabled(true);
    }
    if(offset == lsFiliere.size() - 1){
        btnNext->setEnabled(false);
        btnPreview->setEnabled(true);
    }if(offset == 0){
        btnNext->setEnabled(true);
        btnPreview->setEnabled(false);
    }
}

void EmploiWidget::preview()
{
    if(lsFiliere.isEmpty()) return;
    offset--;
    offset = offset % lsFiliere.size();
    setCurrentLevel(lsFiliere[offset]);
    if(offset>0 && offset <lsFiliere.size() -1){
        btnNext->setEnabled(true);
        btnPreview->setEnabled(true);
    }if(offset == lsFiliere.size() - 1){
        btnNext->setEnabled(false);
        btnPreview->setEnabled(true);
    }if(offset == 0){
        btnNext->setEnabled(true);
        btnPreview->setEnabled(false);
    }
}

QWidget *EmploiWidget::createEmploiVidWindow()
{
    auto widget = new QWidget();
    auto lv = new QVBoxLayout();
    auto logo = new QLabel();
    auto label = new QTextEdit("<div style='color:#418CD4;'>" +
                            tr("Auccun emploi du temps disponible pour cette cette semaine")+"</div>");
    label->setReadOnly(true); label->setEnabled(false);
    label->setFont(QFont("", 35, 5, true));
    logo->setPixmap(QPixmap(":/img/img/logoenastic1.png"));
    logo->setAlignment(Qt::AlignLeft);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    lv->addWidget(logo);
    lv->addWidget(label);
    widget->setLayout(lv);
    return widget;
}

//Menu contextuel

void EmploiWidget::contextMenuEvent(QContextMenuEvent *event)
{
    auto me = this;
    auto menu = new QMenu();
    menu->addAction(tr("Actualiser"), this, [me](){
        if(!me->lsEmplois.contains(me->objectName())) return ;
        me->lsEmplois[me->objectName()] = me->database->getEmploi(me->objectName(), me->lsEmplois[me->objectName()].dateDebut(),
                me->lsEmplois[me->objectName()].dateFin());
        me->setCurrentLevel(me->objectName());
    });
    auto aller = menu->addMenu(tr("Aller à"));
    foreach (auto txt, lsFiliere) {
        aller->addAction(txt, this, [me](){
            auto s = qobject_cast<QAction*>(me->sender());
            if(s == 0) return;
            me->setCurrentLevel(s->text());
            me->offset = me->lsFiliere.indexOf(s->text());
            if(me->offset>0 && me->offset < me->lsFiliere.size() -1){
                me->btnNext->setEnabled(true);
                me->btnPreview->setEnabled(true);
            }if(me->offset == me->lsFiliere.size() - 1){
                me->btnNext->setEnabled(false);
                me->btnPreview->setEnabled(true);
            }if(me->offset == 0){
                me->btnNext->setEnabled(true);
                me->btnPreview->setEnabled(false);
            }
        });
    }
    menu->move(QCursor::pos());
    menu->show();
    QWidget::contextMenuEvent(event);
}

void EmploiWidget::resizeEvent(QResizeEvent *event)
{
    if(centralWindow->currentWidget() == scrollEmploiWindow)
        emploiWindow->resize(event->size() + QSize(-40, 100));
    QWidget::resizeEvent(event);
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

void EmploiWidget::lireEmploi(){
    lsFiliere.clear();
    auto lsFiliereTmp = database->getLsLevel(); // Recupere la liste des filieres
    lsFiliereTmp.sort();
    if(lsFiliereTmp.isEmpty()) return;

    foreach(auto filiere, lsFiliereTmp){
        auto emploi = database->getLastEmploi(filiere);
        if(emploi.idEmploi() == -1) continue;
        lsFiliere.append(filiere);
        lsEmplois[filiere] = emploi;
    }
    if(!lsFiliere.isEmpty()){
        setDefaultLevel(lsFiliere.first());
        setCurrentLevel(lsFiliere.first());
        centralWindow->setCurrentWidget(scrollEmploiWindow);
    }else
        centralWindow->setCurrentWidget(emploiVideWindow);
    if(this->lsFiliere.size() <= 1)
        btnNext->setEnabled(false);
}

void EmploiWidget::actualiser()
{
    if(!lsEmplois.contains(objectName())) return;
    qDebug()<<"EmploiWidget::actualiser: On update";
    lsEmplois[objectName()] = database->getEmploi(objectName(), lsEmplois[objectName()].dateDebut(), lsEmplois[objectName()].dateFin());
    setCurrentLevel(objectName());
}

void EmploiWidget::actualiserMatiere(Matiere m)
{
    foreach (auto mv, lsMatiereView) {
        if(m.getNumeroCase() == mv->getNumeroCase() && mv->getIdEmploi() == m.getIdEmploi())
            mv->setMatiere(m);
    }
}

void EmploiWidget::imprimer()
{
    btnNext->setVisible(false); btnPreview->setVisible(false);btnPrint->setVisible(false);
    QPrinter imprimante;
    imprimante.setOutputFileName(QDir::homePath()+"/emploi_"+objectName()+"_"+EmploieDuTemp::dateToString(dateDebut));
    QPainter rendu;
    auto dialog = new QPrintDialog(&imprimante);
    imprimante.setPageOrientation(QPageLayout::Landscape);
    if(dialog->exec() == QDialog::Accepted){
        rendu.begin(&imprimante);

        auto size = emploiWindow->size();
        double sx = imprimante.pageLayout().fullRectPoints().width()/ (double)size.width();
        double sy = imprimante.pageLayout().fullRectPoints().height()/ (double)size.height();
        double s = sx<= sy ? sx: sy;
        rendu.translate(imprimante.pageLayout().fullRectPoints().x()+imprimante.pageLayout().fullRectPoints().width()/2,
                        imprimante.pageLayout().fullRectPoints().y()+imprimante.pageLayout().fullRectPoints().height()/2);
        rendu.scale(s, s);
        rendu.translate(-size.width()/2, -size.height()/2);
        emploiWindow->render(&rendu);
        rendu.end();
    }

    btnNext->setVisible(true); btnPreview->setVisible(true); btnPrint->setVisible(true);
}
