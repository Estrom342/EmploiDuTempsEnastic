package td.enastic.emploienastic.model;

import android.os.Build;
import androidx.annotation.RequiresApi;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class EmploiDuTemp {

    private String m_niveau;
    private String m_semestre;
    private List<Matiere> m_lsMatieres;
    private List<Horaire> m_lsHoraire;
    private VDate m_VDateDebut;

    private VDate m_VDateFin;
    private VDate m_VDateCreation;
    private int m_idEmploi;
    private String m_lieuDeCreation;
    private byte []m_signature = new byte[]{};
    private String m_nomSalle = "";


    public EmploiDuTemp(String niveau, String semestre){
        m_niveau = niveau; m_semestre = semestre; m_idEmploi= -1;
        m_lsMatieres= new ArrayList<>();
        m_lsHoraire= defaultHoraire();
        for(int i=0; i< 5; i++){
            Horaire h = new Horaire();
            h.setNumeroCase(i+1);
            m_lsHoraire.add(h);
        }
        for(int i=0; i< 18; i++) {
            Matiere m = new Matiere(""); m.setNiveau(niveau); m.setNumeroCase(i+1);
            m_lsMatieres.add(m);
        }
        //setVDateDebut(VDate::currentVDate().addDays(i));
        //m_VDateCreation = VDate.from(new Instant());//currentVDate();
        //m_VDateDebut = VDate(VDate::currentVDate().year(), VDate::currentVDate().month(), Qt::Monday);
        //m_VDateFin = VDate(VDate::currentVDate().year(), VDate::currentVDate().month(), Qt::Saturday);

        m_lieuDeCreation = "";
        m_VDateCreation = new VDate();
        m_VDateDebut = new VDate();
        m_VDateFin = new VDate();
    }

    public String description(){
        return "Emploi du temp de "+m_niveau+ " salle "+m_nomSalle+"\n"+
                "du "+m_VDateDebut+" au "+m_VDateFin+"\nNombre de matiere= "+
                m_lsMatieres.size()+"\nNombre des horaires= "+m_lsHoraire.size();
    }

    public List<Matiere> lsMatieres(){
        return m_lsMatieres;
    }

    public void setLsMatieres(List<Matiere> newLsMatieres){
        m_lsMatieres = newLsMatieres;
    }

    public String niveau(){
        return m_niveau;
    }

    public void setNiveau(String newNiveau){
        m_niveau = newNiveau;
        for (Matiere m : m_lsMatieres) {
            m.setNiveau(m_niveau);
        }
    }

    public VDate dateDebut(){
        return m_VDateDebut;
    }

    public void setDateDebut(VDate newVDateDebut){
        m_VDateDebut = newVDateDebut;
        //m_VDateFin = newVDateDebut.(5);
    }

    public VDate dateFin(){
        return m_VDateFin;
    }

    int idEmploi(){
        return m_idEmploi;
    }

    void setIdEmploi(int newIdEmploi){
        m_idEmploi = newIdEmploi;
        for (Matiere m : m_lsMatieres) {
            m.setIdEmploi(m_idEmploi);
        }
        for (Horaire horaire : m_lsHoraire) horaire.setIdEmploi(newIdEmploi);
    }

    public List<Horaire> lsHoraire(){
        return m_lsHoraire;
    }

    public void setLsHoraire(List<Horaire> newLsHoraire){
        m_lsHoraire = newLsHoraire;
    }

    VDate dateCreation(){
        return m_VDateCreation;
    }

    void setDateCreation( VDate newVDateCreation){
        m_VDateCreation = newVDateCreation;
    }

    String lieuDeCreation(){
        return m_lieuDeCreation;
    }

    public void setLieuDeCreation(String newLieuDeCreation){
        m_lieuDeCreation = newLieuDeCreation;
    }

    byte[] signature(){
        return m_signature;
    }

    void setSignature(byte[] newSignature){
        m_signature = newSignature;
    }

    public String nomSalle(){
        return m_nomSalle;
    }

    public void setNomSalle(String newNomSalle){
        m_nomSalle = newNomSalle;
    }

    String semestre(){
        return m_semestre;
    }

    public void setSemestre(String newSemestre){
        m_semestre = newSemestre;
    }

    public String toHtml(){
        if(m_lsHoraire.size() == 0)
            m_lsHoraire = defaultHoraire();
        String txt = Constant.HTML_HEADER;
        txt+= "<table style=\"border-collapse: collapse; border: 2px solid black; text-align:center; width:100%;\">";
        txt+="<caption><div " +
                "style='text-align:center; background: blue; font-size:20px'>Emploi du Temps du "+dateDebut().toString()+" au "+dateFin().toString()+
                "<div style='text-align:center; background: rgb(76, 210, 89);'>"+niveau()+" "+ nomSalle()+"</div>"+
                "</div>" +
              "</caption>";
        int indexMatiere= 1; int indexHorair=1;
        //Ajout de Jour de la semain
        txt+="<tr>";
        for(int i=0, j= VDate.DayOfWeek.length; i<j; i++){
            if(i==0){
                txt+= "<td style=\"border: 2px solid black;\">Horaire</td>";
                continue;
            }
            txt+= "<td style=\"border: 2px solid black;\">" + VDate.DayOfWeek[i-1] + "</td>";
        }
        txt+="</tr>";
        //Ajout des matiere, horair et pause
        for (int i= 1; i<=5; i++) {
            String row = "<tr>";

            //Ajout de l'horaire
            boolean found = false;
            for (Horaire h : m_lsHoraire) {
                if (h.numeroCase() == indexHorair) {
                    if(h.numeroCase() == 2 || h.numeroCase() == 4)
                        row += "<td style=\"border: 2px solid black; text-align: center;\">" + h.durationTOString() + "</td>";
                    else
                        row += "<td style=\"border: 2px solid black; text-align: center;\">" + h.toHtml() + "</td>";
                    indexHorair++;
                    found = true;
                    break;
                }
            }
            if(!found)
                row += "<td style=\"border: 2px solid black; text-align: center;\"><br/><br/><br/></td>";

            //Ajout de pause
            if (i == 2 || i == 4) {
                for (int k = 1; k <=6; k++) {
                    row += "<td style=\"border: 2px solid black; height: 5px; \">Pause</td>";
                }
                row += "</tr>";
                txt += row;
                continue;
            }

            //Ajout des matieres
            for (int j = 1; j <= 6; j++) {
                found = false;
                for (Matiere m : m_lsMatieres) {
                    if(m.numeroCase() == indexMatiere){
                        row+= m.toHtml();
                        indexMatiere++;
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    row += "<td style=\"border: 2px solid black; text-align: center;\"><br/><br/><br/></td>";
                }
            }
            row+="</tr>";
            txt+=row;
        }
        txt+="</table>";
        txt+=Constant.HTML_FOOTER;
        return txt;
    }

    @Override
    public String toString() {
        String nomSalle = m_nomSalle.isEmpty() ? "SALLE_VIDE" : m_nomSalle,
                semestre = m_semestre.isEmpty() ? "SEMESTRE VIDE" : m_semestre,
                nbreMH = m_lsMatieres.size()+":"+m_lsHoraire.size();
        String string = m_niveau+"\n"+
                nbreMH+"\n"+
                nomSalle+"\n"+
                semestre+"\n"+
                m_VDateDebut.toString()+
                m_VDateFin.toString()+
                m_VDateCreation+
                m_lieuDeCreation+"\n";
        for(Matiere m: m_lsMatieres)
            string += m.toString();
        for(Horaire h : m_lsHoraire)
            string += h.toString();
        return string;
    }

    public static EmploiDuTemp fromString(String string){
        EmploiDuTemp em = new EmploiDuTemp("", "");

        String[] tab = string.split("\n");
        try{
            int i=0;
            String niveau = tab[i++],
                    nbrMH = tab[i++],
                    nomSalle = tab[i++],
                    semestre = tab[i++],
                    dateDebut = tab[i++],
                    dateFin = tab[i++],
                    lieuCreation = tab[i++];
            int nbreMatiere = 0, nbreHoraire = 0;
            try{
                nbreHoraire = Integer.parseInt(nbrMH.split(":")[0]);
                nbreHoraire = Integer.parseInt(nbrMH.split(":")[1]);
            } catch (NumberFormatException | IndexOutOfBoundsException e) {
                System.out.println("EmploiDuTemp.fromString: "+ e.getMessage());
            }

            //Lecture des matieres
            ArrayList<Matiere> lsMatiere = new ArrayList<>();
            for (int j= 0; j< nbreMatiere; j++){
                String txt="";
                for(int k = 0; k< 7; k++)
                    txt += tab[i++]+"\n";
                lsMatiere.add(Matiere.fromString(txt));
            } em.setLsMatieres(lsMatiere);

            //Lecture des horaires
            ArrayList<Horaire> lsHoraire = new ArrayList<>();
            for(int j = 0; j< nbreHoraire; j++){
                lsHoraire.add(Horaire.fromString(tab[i++]+"\n"+tab[i++]));
            } em.setLsHoraire(lsHoraire);

            nomSalle = nomSalle.equals("SALLE_VIDE") ? "" : nomSalle;
            semestre = semestre.equals("SEMESTRE VIDE") ? "" : semestre;
            em.setNiveau(niveau);
            em.setNomSalle(nomSalle);
            em.setSemestre(semestre);
            em.setDateDebut(VDate.fromString(dateDebut));
            em.setLieuDeCreation(lieuCreation);
            em.setNomSalle(nomSalle);
        } catch (IndexOutOfBoundsException e) {
            System.out.println("EmploiDuTemp.fromString: " + e.getMessage());
        }
        return em;
    }

    public static ArrayList<Horaire> defaultHoraire(){
        VTime[] lsTime = {
                new VTime(7, 30, 0, 0), new VTime(9, 30, 0, 0),
                new VTime(9, 30, 0, 0), new VTime(9, 40, 0, 0),
                new VTime(9, 40, 0, 0), new VTime(12, 40, 0, 0),
                new VTime(12, 40, 0, 0), new VTime(13, 40, 0, 0),
                new VTime(13, 40, 0, 0), new VTime(15, 40, 0, 0),
        };
        ArrayList<Horaire> lsHoraire = new ArrayList<>();
        for(int i = 1, j= 0; i<=5; i++){
            Horaire h = new Horaire(i);
            h.setHeureDebut(lsTime[j++]);
            h.setHeureFin(lsTime[j++]);
            lsHoraire.add(h);
        }
        return lsHoraire;
    }
    public void setDateFin(VDate dateFin) {
        m_VDateFin = dateFin;
    }
}
