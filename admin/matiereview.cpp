#include "matiereview.h"
#include <QColorDialog>

MatiereView::MatiereView(Matiere m): matiere(m), m_updateable(false), idEmploi(-1)
{
    database = nullptr; numeroCase = m.getNumeroCase();
    m_editable = false;
    m_updateable = false;
    dataWidget = new QTextEdit();
    setFont(QFont("", 11));
    //matiere.setName("Nom de la matière");
    //matiere.setNomEnseigant("Nom de l'enseignant");
    setStyleSheet(matiere.toStyleSheese());
    setHtml(matiere.toHtml());
    setAlignment(Qt::AlignCenter);
    setReadOnly(true);

    matiere.setCouleur(QColor(255, 255, 255));
    setStyleSheet(matiere.toStyleSheese());
    setMaximumWidth(QApplication::primaryScreen()->availableSize().width()/7);
    setMaximumHeight(QApplication::primaryScreen()->availableSize().height()/3);
    disconnect(this);
}

const Matiere MatiereView::getMatiere()
{
    return matiere;
}

void MatiereView::contextMenuEvent(QContextMenuEvent *e)
{
    if(database == nullptr) return;
    auto menu = new QMenu();
    MatiereView *moi = this;

    auto aSuprimer = menu->addAction(tr("Supprimer"), this, [moi](){
        if(moi->getIdEmploi() != -1){
            moi->database->deleteMatiere(moi->matiere);
            emit moi->actualiserMatiere(moi->matiere);
        }
        auto niveau = moi->matiere.getNiveau();
        moi->setMatiere(Matiere());
        moi->matiere.setNiveau(niveau);
        moi->setHtml(moi->matiere.toHtml());
    });
    auto aCouleur = menu->addAction(tr("Modifier la couleur"), this, [moi](){
        QColor color = QColorDialog::getColor();
        if(color.isValid()){
            if(moi->isUpdateable()){
                if(moi->getIdEmploi() != -1){
                    auto oldMatiere = moi->getMatiere();
                    moi->matiere.setCouleur(color);
                    moi->database->updateMatiere(oldMatiere, moi->matiere);
                    emit moi->actualiserMatiere(moi->matiere);
                }
            }
            moi->matiere.setCouleur(color);
            moi->setStyleSheet(moi->matiere.toStyleSheese());
            emit moi->colorChanged(color.name());
        }
    });
    auto aComment = menu->addAction(tr("Ajouter un commentaire"), this, [moi](){
        QStringList lsComment;
        if(!moi->matiere.commentaire().isEmpty())
            lsComment<<moi->matiere.commentaire();
        lsComment<<tr("13h40-17h40")<<tr("14h40-17h40");
        bool ok = false;
        auto rep = QInputDialog::getItem(moi, tr("Commentaire"), tr("Selectionnez ou écrivez un commentaire"),
                                         lsComment, 0, true, &ok);
        if(ok){
            if(moi->isUpdateable()){
                auto oldMatiere = moi->getMatiere();
                moi->matiere.setCommentaire(rep);
                moi->database->updateMatiere(oldMatiere, moi->matiere);
                emit moi->actualiserMatiere(moi->matiere);
            }
            moi->matiere.setCommentaire(rep);
            moi->setHtml(moi->matiere.toHtml());
        }
    });
    if(!matiere.commentaire().isEmpty())
        aComment->setText(tr("Modifier le commentaire"));
    //Sous menu de choix de matiere
    auto lsMatiere = database->getlsStaticMatiere(matiere.getNiveau());
    QStringList lsMatiereName; lsMatiereName.clear();
    auto menuMatiere = menu->addMenu(tr("Choisir une matiere"));
    foreach (auto matiere, lsMatiere) {
        QVariant enseignant; enseignant.setValue(matiere.getEnseignant());
        menuMatiere->addAction(matiere.getName(), this, [moi](){
            auto aMatiere = qobject_cast<QAction*>(moi->sender()); if(aMatiere == 0) return;
            qDebug()<<moi->matiere.toString();
            auto ensei = qvariant_cast<Teacher>(aMatiere->data());
            qDebug()<<ensei.toString();
            if(moi->isUpdateable()){
                if(moi->getIdEmploi() != -1){
                    auto oldMatiere = moi->getMatiere();
                    moi->matiere.setEnseignant(ensei);
                    moi->matiere.setName(aMatiere->text());
                    moi->database->updateMatiere(oldMatiere, moi->matiere);
                    emit moi->actualiserMatiere(moi->matiere);
                }
            }
            moi->matiere.setEnseignant(ensei);
            moi->matiere.setName(aMatiere->text());
            moi->setHtml(moi->matiere.toHtml());
            qDebug()<<moi->matiere.toString();
        })->setData(enseignant);
    }

    //Sous menu de choix de l'enseignant
    auto menuEnseignant = menu->addMenu(tr("Changer le nom de l'enseignant"));
    auto lsTeacher = database->getlsTeacher(matiere.getNiveau());
    foreach (auto t, lsTeacher) {
        menuEnseignant->addAction(t.getGrade()+" "+t.getNom()+" "+t.getPrenom(), this, [moi](){
            auto aEnseignant = qobject_cast<QAction*>(moi->sender()); if(aEnseignant == 0) return;
            Teacher teacher;
            foreach (auto t2, moi->database->getlsTeacher(moi->matiere.getNiveau())) {
                if(t2.getGrade()+" "+t2.getNom()+" "+t2.getPrenom() == aEnseignant->text()){
                    teacher = t2;
                    break;
                }
            }
            if(moi->isUpdateable()){
                auto oldMatiere = moi->getMatiere();
                moi->matiere.setEnseignant(teacher);
                moi->matiere.setName(moi->database->getMatiereName(teacher, moi->matiere.getNiveau()));
                if(moi->getIdEmploi() != -1){
                    moi->database->updateMatiere(oldMatiere, moi->matiere);
                    emit moi->actualiserMatiere(moi->matiere);
                }
            }else{
                moi->matiere.setName(moi->database->getMatiereName(teacher, moi->matiere.getNiveau()));
                moi->matiere.setEnseignant(teacher);
            }
            moi->setHtml(moi->matiere.toHtml());
        });
    }

    //Marque le type de matiere
    auto menuTypeMatiere = menu->addMenu(tr("Marqué comme"));
    QStringList lsType;
    lsType<<tr("CM")<<tr("TD")<<tr("TP")<<tr("CC")<<tr("CC TP")<<tr("ET1")<<tr("ET2");
    foreach (auto txt, lsType) {
        auto aType = menuTypeMatiere->addAction(txt, this, [moi](){
            auto aType = qobject_cast<QAction*>(moi->sender()); if(aType == 0) return;
            if(aType->text() == QString("ET1") || aType->text() == QString("ET2")){
                moi->matiere.setCouleur(QColor("#F71010"));//Couleur rouge
                moi->setStyleSheet(moi->matiere.toStyleSheese());
            }
            if(moi->isUpdateable()){
                auto oldMatiere = moi->getMatiere();
                moi->matiere.setType(aType->text());
                if(moi->getIdEmploi() != -1){
                    moi->database->updateMatiere(oldMatiere, moi->matiere);
                    emit moi->actualiserMatiere(moi->matiere);
                }
            }
            moi->matiere.setType(aType->text());
            moi->setHtml(moi->matiere.toHtml());
        });
        aType->setEnabled((aType->text() != matiere.getType()) && isEditable());
    }

    //Desactivation des actions non permises
    if(matiere.getName().isEmpty()){
        aSuprimer->setEnabled(false); aCouleur->setEnabled(false);
        menuEnseignant->setEnabled(false);
        menuTypeMatiere->setEnabled(false);
        aComment->setEnabled(false);
    }

    if(!isEditable()){
        aSuprimer->setEnabled(false);
        aComment->setEnabled(false);
        aCouleur->setEnabled(false);
        menuEnseignant->setEnabled(false);
        menuMatiere->setEnabled(false);
        menuTypeMatiere->setEnabled(false);
    }
    menu->move(QCursor::pos()+QPoint(0, -70));
    menu->show();
    e->accept();
}

void MatiereView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        m_activeDrag = true;
        dataWidget->setHtml(matiere.toHtml());
        dataWidget->setStyleSheet(matiere.toStyleSheese());
        dataWidget->resize(size());
        dataWidget->setVisible(true);
        dataWidget->move(QCursor::pos());
        e->accept();
    }
    //QTextEdit::mousePressEvent(e);
}

void MatiereView::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        m_activeDrag = false;
        dataWidget->setVisible(false);
    }
    QTextEdit::mouseReleaseEvent(e);
}

void MatiereView::mouseMoveEvent(QMouseEvent *e)
{
    if(m_activeDrag){
        dataWidget->move(QCursor::pos());
    }
    e->accept();
    //QTextEdit::mouseMoveEvent(e);
}

bool MatiereView::event(QEvent *e)
{
    if(e->type() == QEvent::Enter){
        m_mouseIn = true;
    }
    if(e->type() == QEvent::Leave){
        m_mouseIn = false;
    }
    return QTextEdit::event(e);
}

bool MatiereView::isEditable() const
{
    return m_editable;
}

void MatiereView::setEditable(bool newEditable)
{
    m_editable = newEditable;
}

bool MatiereView::mouseIn() const
{
    return m_mouseIn;
}

void MatiereView::setMouseIn(bool newMouseIn)
{
    m_mouseIn = newMouseIn;
}

const QString MatiereView::color() const{
    return matiere.getCouleur().name();
}

void MatiereView::setColor(const QString &newColor)
{
    MatiereView* s = qobject_cast<MatiereView*>(sender());
    if(s == 0){
        qDebug()<<"MatiereView::setColor null sender" <<s;
        return;
    }
    //if(s->getMatiere().getName() == matiere.getName()){
    if(s->getMatiere().getName() == matiere.getName() && s->getMatiere().getType() == matiere.getType()){
        QColor c(newColor);
        if(c.isValid()){
            if(isUpdateable()){
                auto oldMatiere = getMatiere();
                matiere.setCouleur(c);
                database->updateMatiere(oldMatiere, matiere);
                emit actualiserMatiere(matiere);
            }
            matiere.setCouleur(QColor(c));
            setStyleSheet(matiere.toStyleSheese());
        }
    }
}

int MatiereView::getIdEmploi() const
{
    return idEmploi;
}

void MatiereView::setIdEmploi(int newIdEmploi)
{
    idEmploi = newIdEmploi;
    matiere.setIdEmploi(idEmploi);
}

bool MatiereView::isUpdateable() const
{
    return m_updateable;
}

void MatiereView::setUpdateable(bool newUpdateable)
{
    m_updateable = newUpdateable;
}

int MatiereView::getNumeroCase() const
{
    return numeroCase;
}

void MatiereView::setNumeroCase(int newNumeroCase)
{
    numeroCase = newNumeroCase;
    matiere.setNumeroCase(newNumeroCase);
}

void MatiereView::setDatabase(EmploiDatabase *newDatabase)
{
    database = newDatabase;
}

void MatiereView::setMatiere(const Matiere &newMatiere)
{
    matiere = newMatiere;
    matiere.setNumeroCase(numeroCase);
    if(idEmploi != matiere.getIdEmploi()) matiere.setIdEmploi(idEmploi);
    setStyleSheet(matiere.toStyleSheese());
    setHtml(matiere.toHtml());
}
