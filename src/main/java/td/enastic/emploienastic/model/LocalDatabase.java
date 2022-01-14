package td.enastic.emploienastic.model;

import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.graphics.Color;
import android.util.Log;
import androidx.annotation.Nullable;

import java.sql.Date;
import java.util.ArrayList;

import static td.enastic.emploienastic.model.Constant.*;

public class LocalDatabase extends SQLiteOpenHelper {

    public LocalDatabase(@Nullable Context context, @Nullable String name, @Nullable SQLiteDatabase.CursorFactory factory, int version) {
        super(context, name, factory, version);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        String[] lsQuery = DATABASE_ON_CREATE.split(";\n");
        for(String query: lsQuery) {
            try {
                db.execSQL(query);
            }catch (SQLException e){
                Log.i("MYDEBUG", e.getMessage());
            }
        }
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL(Constant.ON_APDATE_DATABASE);
        onCreate(db);
    }

    //Renvoi la liste de tous les niveaux
    public ArrayList<String> getLsLevel(){
        ArrayList<String> lsLevel = new ArrayList<>();
        try {
            SQLiteDatabase database = getReadableDatabase();
            Cursor query = database.rawQuery("SELECT DISTINCT nomNiveau FROM Niveau;", new String[]{});
            while(query.moveToNext())
                lsLevel.add(query.getString(0));
            query.close();
        }catch (SQLException e) {
            Log.i(Constant.DATABASE_NAME, "Error at getLsLevel: " + e.getMessage());
        }
        return lsLevel;
    }
    //Ajout un niveau
    public boolean addLevel(String level, String nomSalle){
        //if(getIdLevel(level, nomSalle) != -1) return true;
        try{
            SQLiteDatabase database = getWritableDatabase();
            database.execSQL("INSERT INTO Niveau(nomNiveau, nomSalle) VALUES(?, ?)", new String[]{level, nomSalle});
            database.close();
        }catch (SQLException e){
            Log.i(Constant.DATABASE_NAME, "Error at getIdLevel: " +e.getMessage());
            return false;
        }
        return true;
    }
    //Retourne l'identifiant de level
    public int getIdLevel(String level, String nomSalle){
        try {
            Cursor query = getReadableDatabase().rawQuery("SELECT idNiveau FROM Niveau WHERE nomNiveau = ? AND " +
                    "nomSalle = ?;", new String[]{level, nomSalle});
            while(query.moveToNext()){
                int id = query.getInt(0);
                query.close();
                return id;
            }
            query.close();
        }catch (SQLException e) {
            Log.i(Constant.DATABASE_NAME, "Error at getIdLevel: " + e.getMessage());
        }
        return -1;
    }
    //Ajoute ou met a jour un nouvel emploi
    public boolean addEmploi(EmploiDuTemp emploi){
        if(emploi.lsMatieres().isEmpty()) return false;
        int idEmploi = getIdEmploi(emploi);
        if(idEmploi != -1){
            return updateEmploi(emploi);
        }
        if(emploi.niveau().isEmpty()){
            return false;
        }
        int idLevel = getIdLevel(emploi.niveau(), emploi.nomSalle());
        if(idLevel == -1) {
            Log.i("MYDEBUG", "addEmploi(EmploiDuTemp emploi): Ajout de niveau: "+addLevel(emploi.niveau(), emploi.nomSalle()));
        }
        idLevel = getIdLevel(emploi.niveau(), emploi.nomSalle());
        if(idLevel == -1)
            Log.i(Constant.DATABASE_NAME, "Holala!!!!!!!!!!!!");

        try {
            Log.i("MYDEBUG", "Ajout d'emploi du temp: "+emploi.description());
            SQLiteDatabase database = getWritableDatabase();
            database.execSQL("INSERT INTO Emploi(idNiveau, dateDebut, dateFin, dateCreation, lieuCreation, semestre) " +
                    "VALUES(?, ?, ?, ?, ?, ?);", new String[]{idLevel+"", emploi.dateDebut().toSqlFormat(), emploi.dateFin().toSqlFormat(), emploi.dateCreation().toString(),
                    emploi.lieuDeCreation(), emploi.semestre()});
            database.close();
            idEmploi = getIdEmploi(emploi);
            //assert (idEmploi != -1);
            for(Matiere matiere: emploi.lsMatieres()){
                Log.i("MYDEBUG", "Ajout de matiere: "+matiere.name());
                matiere.setIdEmploi(idEmploi);
                addMatiere(matiere);
            }
            //Insertion de la liste des horaire
            for(Horaire h: emploi.lsHoraire()) {
                Log.i("MYDEBUG", "Ajout d'heure: "+h.numeroCase());
                h.setIdEmploi(idEmploi);
                addHoraire(h);
            }
            return true;
        }catch (SQLException e){
            Log.i(Constant.DATABASE_NAME, "Error at addEmploi(EmploiDuTemp emploi) : "+ e.getMessage());
        }
        return false;
    }


    //Pour obtenir l'identifiant d'un emploi
    public int getIdEmploi(String filiere, String nomSalle, VDate dateDebut, VDate dateFin){
        int idLevel = getIdLevel(filiere, nomSalle);
        if(idLevel == -1) {
            return -1;
        }
        try{
            Cursor query = getReadableDatabase().rawQuery("SELECT idEmploi FROM Emploi WHERE dateDebut = ? "+
                    "AND dateFin = ? AND idNiveau = ?;", new String[]{dateDebut.toSqlFormat(), dateFin.toSqlFormat(), idLevel+""});
            while(query.moveToNext())
                return query.getInt(0);
            query.close();
        }catch (SQLException e){
            Log.i(Constant.DATABASE_NAME, "Error at getIdEmploi: " + e.getMessage());
        }
        return -1;
    }

    //Memme que la fonction precedente
    public int getIdEmploi(EmploiDuTemp emploi){
        return getIdEmploi(emploi.niveau(), emploi.nomSalle(), emploi.dateDebut(), emploi.dateFin());
    }

    //Retourne l'emploi de niveau filiere de la semaine dateDebut et dateFin
    public EmploiDuTemp getEmploi(String filiere, VDate dateDebut, VDate dateFin){
        //mutex->lock();
        EmploiDuTemp emploi = new EmploiDuTemp(filiere, "");
        emploi.setDateDebut(dateDebut);
        emploi.setDateFin(dateFin);
        emploi.setNiveau(filiere);
        emploi.setLsMatieres(new ArrayList<>());

        try{
            SQLiteDatabase database = getReadableDatabase();
            Cursor query = database.rawQuery("SELECT nomMatiere, typeMatiere, nomEnseignant, couleur, " +
                    "numCase, commentaire , dateCreation, lieuCreation, semestre, nomNiveau, nomSalle, idEmploi, signature " +
                    "FROM EmploiView WHERE dateDebut = ? AND dateFin = ? AND nomNiveau = ?;",
                    new String[]{emploi.dateDebut().toSqlFormat(), emploi.dateFin().toSqlFormat(), filiere});
            ArrayList<Matiere> lsMatieres = new ArrayList<>(); int i=0;
            while (query.moveToNext()) {
                int index = 0;
                Matiere m = new Matiere(query.getString(index++));
                m.setType(query.getString(index++));
                m.setTeacherName(query.getString(index++));
                m.setCouleur(query.getString(index++));
                m.setColor(VColor.fromSqliteValue(m.couleur()));
                m.setNumeroCase(query.getInt(index++));
                m.setComment(query.getString(index++));
                lsMatieres.add(m);
                if(i == 0){
                    //index++;
                    emploi.setDateCreation(VDate.fromString(query.getString(index++)));
                    emploi.setLieuDeCreation(query.getString(index++));
                    emploi.setSemestre(query.getString(index++));
                    emploi.setNiveau(query.getString(index++));
                    emploi.setNomSalle(query.getString(index++));
                    emploi.setIdEmploi(query.getInt(index++));
                    emploi.setSignature(query.getBlob(index++));
                    //qDebug()<<"getEmploi "<<emploi.niveau()<<emploi.signature().size();

                    i++;
                }
            }
            database.close(); query.close();
            emploi.setLsMatieres(lsMatieres);
            try {
                //Selection de la liste des horaire
                SQLiteDatabase database1 = getReadableDatabase();
                query = database1.rawQuery("SELECT numCase, heureDebut, heureFin, idEmploi FROM Horaire WHERE idEmploi = ?;",
                        new String[]{String.valueOf(emploi.idEmploi())});
                ArrayList<Horaire> lsHoraires = new ArrayList<>();
                while (query.moveToNext()) {
                    int index = 0;
                    Horaire h = new Horaire();
                    h.setNumeroCase(query.getInt(index++));
                    h.setHeureDebut(VTime.fromString(query.getString(index++)));
                    h.setHeureFin(VTime.fromString(query.getString(index++)));
                    h.setIdEmploi(query.getInt(index++));
                    lsHoraires.add(h);
                }
                emploi.setLsHoraire(lsHoraires);
                database1.close();
                query.close();
            }catch (SQLException e) {
                Log.i(Constant.DATABASE_NAME, "Error at getEmploi(Recuperation de liste d'horaire) : "+e.getMessage());
            }
        }catch (SQLException e) {
            Log.i(Constant.DATABASE_NAME, "Error at getEmploi: " + e.getMessage());
        }
        return emploi;
    }

    //Retourne le dernier emploi du temps pour le niveau level
    public EmploiDuTemp getLastEmploi(String filiere) {
        EmploiDuTemp emploi = new EmploiDuTemp(filiere, "");
        emploi.setLsMatieres(new ArrayList<>());

        try {
            VDate dateDebut = new VDate(), dateFin = new VDate();
            Cursor query = getReadableDatabase().rawQuery("SELECT MAX(dateDebut) AS dateDebut, MAX(dateFin) AS " +
                    "dateFin FROM Emploi LEFT JOIN Niveau ON Emploi.idNiveau = Niveau.idNiveau " +
                    "WHERE nomNiveau = ?;", new String[]{filiere});
            while (query.moveToNext()) {
                dateDebut = VDate.fromString(query.getString(0));
                dateFin = VDate.fromString(query.getString(1));
                break;
            }
            query.close();
            return getEmploi(filiere, dateDebut, dateFin);
        }catch (SQLException e) {
            Log.i("MYDEBUG", "Error at getLastEmploi: hmm " + e.getMessage());
        }
        return emploi;
    }

    @Override
    public void close(){
        try {
            close();
        }catch (Exception ignored){}
    }
    public Parametre getPrametre(){
        Parametre p = new Parametre();
        try {
            Cursor query = getReadableDatabase().rawQuery("SELECT hostName, studentLevel, portName, theme " +
                    "FROM Parametre;", new String[]{});
            while (query.moveToNext()) {
                int index = 0;
                p.setHostName(query.getString(index++));
                if (p.hostName().isEmpty())
                    p.setHostName(Constant.DEFAULT_HOSTNAME);
                p.setLevel(query.getString(index++));
                p.setPortName(query.getInt(index++));
                p.setTheme(query.getString(index++));
                return p;
            }
        }catch (Exception e) {
            Log.i("MYDEBUG", "Error at Parametre getPrametre "+ e.getMessage());
        }
        return p;
    }

    public void setParametre(Parametre p){
        //mutex->lock();
        try {
            SQLiteDatabase database = getWritableDatabase();

            database.execSQL("DELETE FROM Parametre;");
            database.execSQL("INSERT INTO Parametre(hostName, portName, studentName, studentLevel, theme)" +
                        "VALUES(? , ?, ? , ?, ?)", new String[]{p.hostName, String.valueOf(p.portName), p.studentName(), p.getLevel(), p.theme()});
        }catch (SQLException e){
            Log.i("MYDEBUG", "Error at Parametre setParametre " + e.getMessage());
        }
    }
    //Met a jour un emploi
    public boolean updateEmploi(EmploiDuTemp emploi){
        int idEmploi = getIdEmploi(emploi);
        if(idEmploi == -1){
            Log.i(Constant.DATABASE_NAME, "updateEmploi idEmploi = -1 "+emploi.dateCreation().toString());
            return false;
        }
        Log.i("MYDEBUG", "Mise a jour d'emploi du temp: id= "+idEmploi+"\nDescription:\n"
                +emploi.description());
        try {
            //Suppression des horaire existantes
            SQLiteDatabase database = getWritableDatabase();
            database.execSQL("DELETE FROM Horaire WHERE idEmploi = ?;", new String[]{String.valueOf(idEmploi)});
            database.close();
            //Remplacement des horaire par des nouveaux
            for (Horaire h : emploi.lsHoraire()) {
                h.setIdEmploi(idEmploi);
                addHoraire(h);
            }
        }catch (SQLException e) {
            Log.i(Constant.DATABASE_NAME, "Error at updateEmploi: (Suppression des Horaires)"+e.getMessage());
            return false;
        }try {
            SQLiteDatabase database = getWritableDatabase();
            //Suppression des Matiere existantes
            database.execSQL("DELETE FROM Matiere WHERE idEmploi = ?;", new String[]{String.valueOf(idEmploi)});
            database.close();
            for (Matiere matiere : emploi.lsMatieres()) {
                matiere.setIdEmploi(idEmploi);
                addMatiere(matiere);
            }
        }catch (SQLException e) {
            Log.i(Constant.DATABASE_NAME, "Error at updateEmploi: (Suppression des matieres)"+e.getMessage());
            return false;
        }try {
            //Mise a jour des information de l'emploi
            SQLiteDatabase database = getWritableDatabase();
            database.execSQL("UPDATE Emploi SET dateCreation = ?, lieuCreation = ?, semestre = ? " +
                    "WHERE idEmploi = ?;", new String[]{emploi.dateCreation().toString(), emploi.lieuDeCreation(), emploi.semestre(), idEmploi + ""});
            database.close();
        }catch (SQLException e) {
            Log.i(Constant.DATABASE_NAME, "Error at updateEmploi (UPDATE Emploi):  mise a jour de l'emploi" + e.getMessage());
            return false;
        }
        return true;
    }

    public boolean updateSignature(EmploiDuTemp em){
        int idNiveau = getIdLevel(em.niveau(), em.nomSalle());
        if(idNiveau == -1) {
            Log.i(Constant.DATABASE_NAME, "Error at updateSignature (idNiveau = -1)");
            return false;
        }
        try {
            getWritableDatabase().rawQuery("UPDATE Emploi SET signature = :signature WHERE dateDebut = ? AND "+
                    " dateFin = ? AND idNiveau = ?;", new String[]{em.signature().toString(), em.dateDebut().toSqlFormat(),
                            em.dateFin().toSqlFormat(), idNiveau + ""}).close();
        }catch (SQLException e){
            Log.i(Constant.DATABASE_NAME, "Error at updateSignature "+e.getMessage());
            return false;
        }
        return true;
    }

    //Ajoute une matiere dans l'emploi du temps
    public boolean addMatiere(Matiere m){
        //mutex->lock();
        if(m.idemploi() == -1){
            Log.i(Constant.DATABASE_NAME, "Error at addMatiere idEmploi negatif: ");
            return false;
        }
        try {
            SQLiteDatabase database = getWritableDatabase();
            database.execSQL("INSERT INTO Matiere(idEmploi, type, nomMatiere, nomEnseignant, couleur, numCase, commentaire)"+
                    "VALUES(?, ?, ?, ?, ?, ?, ?)", new String[]{String.valueOf(m.idemploi()), m.type(), m.name(), m.teacherName(),
                            m.color().toSqliteFormat(), m.numeroCase() + "", m.comment()});
            database.close();
        }catch (SQLException e) {
            Log.i(Constant.DATABASE_NAME, "Error at updateEmploi: " + e.getMessage());
            return false;
        }
        //mutex->unlock();
        return true;
    }

    public boolean addHoraire(Horaire h){
        if(h.idEmploi() == -1){
            Log.i("MYDEBUG", "addHoraire(Horaire): idEmploi negatif");
            return false;
        }
        try {
            Log.i("MYDEBUG", "addHoraire(Horaire): idEmploi= "+ h.idEmploi());
            SQLiteDatabase database = getWritableDatabase();
            database.execSQL("INSERT INTO Horaire(numCase, heureDebut, heureFin, idEmploi)"+
                    "VALUES(?, ?, ?, ?);", new String[]{String.valueOf(h.numeroCase()), h.heureDebut().toSqlFormat(),
                    h.heureFin().toSqlFormat(), String.valueOf(h.idEmploi())});
            database.close();
        }catch (SQLException e) {
            Log.i("MYDEBUG", "Error at addHoraire(Horaire):" + e.getMessage());
            return false;
        }
        return true;
    }
    public class Parametre{
        public String password() {
            return password;
        }
        public void setPortName(long portName) {
            this.portName = portName;
        }

        public void setPalete(String palete) {
            this.palete = palete;
        }

        public byte[] signature() {
            return m_signature;
        }

        public void setSignature(byte[] m_signature) {
            this.m_signature = m_signature;
        }

        String hostName, studentName, level, theme, password;
        long portName; String palete;
        byte[] m_signature;
        Parametre(){
            hostName = Constant.DEFAULT_HOSTNAME;
            password = "";
            portName = Constant.DEFAULT_PORT_NAME;
            theme = PALETTE_CLEAR_TXT;
            palete = "";
            level = "";
            studentName = "";
        }

        public String level(){
            return level;
        }

        public long portName(){
            return portName;
        }

        public void setPortName(int newPortName){
            portName = newPortName;
        }

        public String theme(){
            return theme;
        }

        public void setTheme(String newTheme)
        {
            theme = newTheme;
            if(theme.equals(Constant.PALETTE_CLEAR_TXT))
                palete = "";
            if(theme.equals(PALETTE_SOMBRE_TXT))
                palete = (new Color()).toString();
        }
        public String palete(){
            return palete;
        }

        public String getLevel(){
            return level;
        }

        public void setLevel(String newLevel){
            level = newLevel;
        }

        String StudentName(){
            return studentName;
        }

        public void setStudentName(String newStudentName){
            studentName = newStudentName;
        }

        public String hostName(){
            return hostName;
        }

        public void setHostName(String newHostName){
            hostName = newHostName;
        }

        public String studentName() {
            return studentName;
        }
    }
}
