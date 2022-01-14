package td.enastic.emploienastic.controler;

import android.util.Log;
import td.enastic.emploienastic.model.*;

import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;

public class StudentSocket extends Thread implements SocketListener{

    private boolean connected;
    private LocalDatabase database;
    private LocalDatabase.Parametre setting;
    private EmploiDuTemp m_emploi;
    Socket socket = null;
    private  String objectName;
    BufferedReader inputStream = null;
    DataOutputStream outputStream = null;
    List<SocketListener> lsListeners;

    public StudentSocket(){
        lsListeners = new ArrayList<>();
        objectName = "";
        connected = false;
        socket = new Socket();
    }

    public void addListener(SocketListener l){
        lsListeners.add(l);
    }
    public void setDatabase(LocalDatabase database) {
        this.database = database;
        setting = database.getPrametre();
    }

    public void setSetting(LocalDatabase.Parametre setting) {
        this.setting = setting;
    }

    public LocalDatabase getDatabase() {
        return database;
    }

    public String getObjectName() {
        return objectName;
    }

    public void setObjectName(String objectName) {
        this.objectName = objectName;
    }

    public void tryToConnect() {
        if (socket!=null && socket.isConnected())
            return;
        while (true) {
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                System.out.println(e.getMessage() + Thread.currentThread() + " tryToConnect");
                //e.printStackTrace();
            }
            try {
                socket = new Socket(setting.hostName(), (int) setting.portName());
                Log.i("MYDEBUG","Connexion reussu");
                inputStream = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                outputStream = new DataOutputStream(socket.getOutputStream());
                status(Constant.ONLINE_ICON);
                onConnected();
                connected = true;
                break;
            } catch (UnknownHostException | SocketException | SocketTimeoutException e) {
                System.out.println(e.getMessage() + Thread.currentThread() + " tryToConnect");
                status(Constant.OFFLINE_ICON);
                connected = false;
                Log.i("MYDEBUG", "Error at StudentSocket.tryToconnect " + e.getMessage());
            } catch (IOException e) {
                connected = false;
                System.out.println(e.getMessage() + Thread.currentThread() + " tryToConnect");
                Log.i("MYDEBUG", "Error at StudentSocket.tryToconnect " + e.getMessage());
            }
        }
    }

    void onConnected(){
        if(!objectName.equals(Constant.MAIN_SOCKET_NAME))
            return;
        if(objectName.equals(Constant.MAIN_SOCKET_NAME)) {
            String data = Constant.FLAG + "\n" +
                    Constant.GET_LAST_EMPLOI_FLAG + "\n";
            writeUTF(data);
        }if(objectName.equals(Constant.ASK_TIMETABLE_SOCKET_NAME)){
            String data = Constant.FLAG+"\n"+
                    Constant.GET_SIGNATURE_EMPLOI_FLAG+"\n"+
                    m_emploi.toString();
            writeUTF(data);
        }
    }

    public void writeUTF(String data){
        try {
            outputStream.writeUTF(data);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void onReadyRead() {

        int nbreEmbloi =0;
        while(true) {
            String flag = null;
            flag = readLine();
            flag = readLine();
            if (flag.equals(Constant.EMPLOI_FLAG)) {
                EmploiDuTemp em = readEmploi();
                //Log.i("MYDEBUG", "Emploi du temps recu: " + em.toString());
                if (1<0 && database.addEmploi(em)) {
                    //Demande de signature d'emploi
                    EmploiDuTemp tmpEmploi = new EmploiDuTemp(em.niveau(), "");
                    tmpEmploi.setNomSalle(em.nomSalle());
                    tmpEmploi.setDateDebut(em.dateDebut());
                    tmpEmploi.setLsHoraire(new ArrayList<>());
                    tmpEmploi.setLsMatieres(new ArrayList<>());
                    //askSignatureEmploi(em);
                    //sleep(1000);
                    nbreEmbloi++;
                }
            } else if (flag.equals(Constant.LAST_EMPLOI_FLAG)) {
                nbreEmbloi++;
                EmploiDuTemp em = readEmploi();
                //Log.i("MYDEBUG", "Emploi du temps recu: " + em.description());
                if (em.lsMatieres().size() > 0) {
                    database.addEmploi(em);
                    timeTableAvailable(em);
                    //Demande de signature d'emploi
                    //EmploiDuTemp tmpEmploi = new EmploiDuTemp(em.niveau(), "");
                    //tmpEmploi.setNomSalle(em.nomSalle());
                    //tmpEmploi.setDateDebut(em.dateDebut());
                    //tmpEmploi.setLsHoraire(new ArrayList<>());
                    //tmpEmploi.setLsMatieres(new ArrayList<>());
                    //askSignatureEmploi(em);
                }
                //qDebug()<<"onReadyRead (LAST_EMPLOI_FLAG): "<<em.niveau()<<em.lsMatieres().size();
            } else if (flag.equals(Constant.SIGNATURE_EMPLOI_FLAG)) {
                EmploiDuTemp em = readEmploi();
                byte[] signature = new byte[999999];
                try {
                    String string = null;
                    while (string.equals("")){
                        string = inputStream.readLine();
                        signature = string.getBytes();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                //em.setNomSalle(emploi().nomSalle());
                //emit signatuereAvaible(em.niveau(), emploi().nomSalle(), em.dateDebut(), em.dateFin(), signature);
                for(SocketListener s: lsListeners)
                    s.signatureAvailable(em.niveau(), em.nomSalle(), em.dateDebut(), em.dateFin(), signature);
                quit();
            } else {
                Log.i("DEBUG", "onReadyRead unknow FLAG = " + flag);
            }
            if (objectName.equals(Constant.TEMP_SOCKET_NAME)) {
                try {
                    socket.close();
                    break;
                } catch (IOException e) {
                    e.printStackTrace();
                    break;
                }
            }
        }
    }

    void quit(){
        try {
            interrupt();
            socket.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    byte[] toBytes(String str){
        return str.getBytes();
    }

    //Lecture d'emploi du temps

    EmploiDuTemp readEmploi(){
        EmploiDuTemp em = new EmploiDuTemp("", "");
        String niveau = readLine(),
                nbreMH = readLine(),
                nomSalle = readLine(),
                semestre = readLine(), lieuCreation="";
        VDate debut = VDate.fromString(readLine()),
                fin = VDate.fromString(readLine()),
                dateCreation = VDate.fromString(readLine());
        lieuCreation = readLine();

        int nbreMatiere = 0, nbreHoraire= 0;
        try {
            nbreMatiere =  Integer.valueOf(nbreMH.split(":")[0]);
            nbreHoraire = Integer.valueOf(nbreMH.split(":")[1]);
        } catch (IndexOutOfBoundsException | NumberFormatException e) {
            Log.i("MYDEBUG", "Error at Student socket :"+e.getMessage());
        }
        //Recuperation des matieres
        List<Matiere> lsMatiere = new ArrayList<>();
        for(int i=0, j= nbreMatiere; i<j; i++){
            String typeMatiere = readLine();
            String nomMatire = readLine();
            String textNumCase = readLine();
            String gradeEnseignat = readLine();
            String nomEnseignant = readLine();
            String prenomEnseignat = readLine();
            String couleur = readLine();
            String commentaire = readLine();
            int numroCase = -1;
            try {
                numroCase = Integer.valueOf(textNumCase);
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
            Matiere matiere = new Matiere(nomMatire);
            matiere.setColor(VColor.fromString(couleur));
            matiere.setType(typeMatiere);
            Teacher t = new Teacher(nomEnseignant, prenomEnseignat, gradeEnseignat, "");
            matiere.setTeacher(t);
            matiere.setNumeroCase(numroCase);
            lsMatiere.add(matiere);

            /*Log.i("MYDEBUG", "Type de matiere ="+typeMatiere+
                    "\nNom de matiere= "+nomMatire +
                    "\nNum case = "+textNumCase+" | "+numroCase+
                    "\nTeacher=" +gradeEnseignat+" "+nomEnseignant+" "+prenomEnseignat+
                    "\nCouleur = "+couleur+"("+matiere.color().red()+", "+matiere.color().green()+", "+matiere.color().blue()+")\n"+
                    "\nComment= "+commentaire);*/
        }em.setLsMatieres(lsMatiere);
        //Recueration de la list des horaires
        List<Horaire> lsHoraires = new ArrayList<>();
        for(int i=0; i<nbreHoraire; i++){
            Horaire h = Horaire.fromString(readLine()+"\n"+readLine());
            lsHoraires.add(h);
            //Log.i("MYDEBUG", "Horaire "+h.toString() + " numCase = "+h.numeroCase()+"\n");
        }em.setLsHoraire(lsHoraires);

        nomSalle = nomSalle.equals("SALLE_VIDE") ? "" : nomSalle;
        semestre = semestre.equals("SEMESTRE VIDE") ? "" : semestre;

        em.setNiveau(niveau);
        em.setNomSalle(nomSalle);
        em.setSemestre(semestre);
        em.setDateDebut(debut);
        em.setLieuDeCreation(lieuCreation);
        em.setNomSalle(nomSalle);
        return em;
    }

    String readLine(){
        try {
            String str = inputStream.readLine();
            return str == null ? "" : str;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return "";
    }

    public void onDisconnected()
    {
        connected= false;
        for(SocketListener l: lsListeners)
            l.status(Constant.OFFLINE_ICON);
    }

    EmploiDuTemp emploi(){
        return m_emploi;
    }

    public void setEmploi(EmploiDuTemp newEmploi){
        m_emploi = newEmploi;
    }

    //Demande l'emploi au serveur
    public void askEmploi(VDate dateDebut, String niveau) throws IOException {
        EmploiDuTemp em = new EmploiDuTemp(niveau, "");
        em.setDateDebut(dateDebut);
        StudentSocket socket = new StudentSocket();
        socket.setDatabase(database);
        socket.setObjectName(Constant.SIGNATURE_SOCKET_NAME);
        setEmploi(em);
        socket.start();
        //outputStream.write(FLAG + GET_EMPLOI_FLAG);
    }

    //Demande au serveur la signature de l'emploi de temps
    public void askSignatureEmploi(EmploiDuTemp em)
    {
        if(!connected)
            return;
        Log.i("MYDEBUG", "Demande de signarure pour l'emploi de "+ emploi().nomSalle());
        StudentSocket socket = new StudentSocket();
        socket.setDatabase(database);
        socket.setObjectName(Constant.ASK_TIMETABLE_SOCKET_NAME);
        setEmploi(em);
        socket.start();
    }

    public boolean isConnected(){
        return connected && socket.isConnected();
    }

    @Override
    public void timeTableAvailable(EmploiDuTemp em) {
        for (SocketListener l: lsListeners)
            l.timeTableAvailable(em);
    }

    @Override
    public void status(String state) {
        for (SocketListener l: lsListeners)
            l.status(state);
    }

    @Override
    public void signatureAvailable(String level, String nomSalle, VDate dateDebut, VDate dateFin, byte[] signature) {
        for (SocketListener l: lsListeners)
            l.signatureAvailable(level, nomSalle, dateDebut, dateFin, signature);
    }

    @Override
    public void run() {
        while (true){
            tryToConnect();
            onReadyRead();
        }
    }
}
